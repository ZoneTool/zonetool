#include "stdafx.hpp"
#include <string>
#include <vector>
#include <filesystem>

#include <zlib.h>

#include <ZoneUtils.hpp>

using namespace std::literals;

class file
{
public:
	file()
	{

	}

	file(const std::string& filename) : filename_(filename)
	{
		auto fp = fopen(filename_.data(), "rb");
		if (fp)
		{
			const auto filesize = std::filesystem::file_size(filename_);
			buffer_.resize(filesize);
			fread(&buffer_[0], 1, filesize, fp);
			fclose(fp);
		}
	}

	template <typename T> T* get(const std::size_t offset)
	{
		return reinterpret_cast<T*>(&buffer_[offset]);
	}

	void save()
	{
		auto fp = fopen(filename_.data(), "wb");
		if (fp)
		{
			fwrite(&buffer_[0], 1, buffer_.size(), fp);
			fclose(fp);
		}
	}

	void destroy()
	{
		buffer_.clear();
	}

	std::string name()
	{
		return filename_;
	}

protected:
	std::string filename_;
	std::vector<std::uint8_t> buffer_;

};

void endian_convert_entries(ZoneTool::XAssetStreamFile* entries, std::uint32_t count)
{
	for (auto i = 0u; i < count; i++)
	{
		ZoneTool::endian_convert(&entries[i].fileIndex);
		ZoneTool::endian_convert(&entries[i].offset);
		ZoneTool::endian_convert(&entries[i].offsetEnd);
	}
}

void handle_entries(file& ff, std::vector<file*>& paks, ZoneTool::PakFile& img5)
{
	auto count = *ff.get<std::uint32_t>(0x19);						// get count
	ZoneTool::endian_convert(&count);								// endian convert count

	auto entries = ff.get<ZoneTool::XAssetStreamFile>(0x1D);		// entry array

	// convert to little endian so we can use the data
	endian_convert_entries(entries, count);

	// generate new image pak
	for (auto i = 0u; i < count; i++)
	{
		if (entries[i].fileIndex == 0) continue;
		if (entries[i].fileIndex == 5)
		{
			printf("FF %s seems to be converted already.\n", ff.name().data());
			break;
		}

		// get pointer to correct image pak
		auto cur_pak = paks[entries[i].fileIndex - 1];

		// grab image data
		auto image_data = cur_pak->get<std::uint8_t>(entries[i].offset);

		// store image in new imgpak
		auto new_image_data = img5.add_entry(image_data, entries[i].offsetEnd - entries[i].offset, true);

		// replace existing image data
		entries[i].offset = new_image_data.first;
		entries[i].offsetEnd = new_image_data.second;
		entries[i].fileIndex = 5;
	}

	// convert back to big endian
	endian_convert_entries(entries, count);
}

std::pair<std::string, std::uint32_t> read_sortkey_and_techset(const std::filesystem::path& path)
{
	auto json_data = ""s;
	auto file_size = std::filesystem::file_size(path);
	json_data.resize(file_size);

	auto path_nigger = path.string();

	auto fp = fopen(path_nigger.data(), "rb");
	if (fp)
	{
		fread(&json_data[0], file_size, 1, fp);
		fclose(fp);
	}

	Json json = Json::parse(json_data);
	return { json["techniqueSet->name"].get<std::string>(), json["sortKey"].get<std::uint32_t>() };
}

int main(int argc, char** argv)
{
	file img1("imagefile1.pakm");
	file img2("imagefile2.pakm");
	file img3("imagefile3.pakm");
	file img4("imagefile4.pakm");

	std::vector<file*> paks = { &img1, &img2, &img3, &img4 };

	file ff("mp_shipment.ff");
	file loadscreen("mp_shipment_load.ff");

	// alloc new image pak
	ZoneTool::PakFile img5(269);

	handle_entries(ff, paks, img5);
	handle_entries(loadscreen, paks, img5);

	// destroy paks
	for (auto& pak : paks)
	{
		pak->destroy();
	}

	// save data
	ff.save();
	loadscreen.save();

	// save image pak
	img5.save("imagefile5.pak");

	return 0;

	//std::vector<std::filesystem::path> iw3_materials;
	//std::vector<std::filesystem::path> iw5_materials;

	//std::vector<std::pair<std::string, std::uint32_t>> iw3_keys;
	//std::vector<std::pair<std::string, std::uint32_t>> iw5_keys;

	//std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> mapped_keys;
	//std::map<std::uint32_t, std::uint32_t> final_map;

	//// find materials
	//for (auto& itr : std::filesystem::recursive_directory_iterator("E:\\SteamLibrary\\steamapps\\common\\Call of Duty 4\\dump"))
	//{
	//	if (std::filesystem::is_regular_file(itr) && strstr(itr.path().string().data(), "\\materials\\"))
	//	{
	//		iw3_materials.push_back(itr.path());
	//	}
	//}
	//for (auto& itr : std::filesystem::recursive_directory_iterator("E:\\SteamLibrary\\steamapps\\common\\Call of Duty Modern Warfare 2\\dump"))
	//{
	//	if (std::filesystem::is_regular_file(itr) && strstr(itr.path().string().data(), "\\materials\\"))
	//	{
	//		iw5_materials.push_back(itr.path());
	//	}
	//}

	//printf("found a total of %u materials to analyse...\n", iw3_materials.size() + iw5_materials.size());
	//printf("parsing...\n");

	//// map techsets & sort keys
	//for (auto& iw3_mat : iw3_materials)
	//{
	//	iw3_keys.push_back(read_sortkey_and_techset(iw3_mat));
	//}
	//for (auto& iw5_mat : iw5_materials)
	//{
	//	iw5_keys.push_back(read_sortkey_and_techset(iw5_mat));
	//}

	//printf("mapping...\n");

	//// 
	//for (auto& iw5_key : iw5_keys)
	//{
	//	const auto itr = std::find_if(iw3_keys.begin(), iw3_keys.end(), [iw5_key](auto& iw3_key)
	//	{
	//		if (iw5_key.first == iw3_key.first)
	//		{
	//			return true;
	//		}

	//		if (iw5_key.first.find("_sat") != std::string::npos)
	//		{
	//			if (iw5_key.first == iw3_key.first + "_sat")
	//			{
	//				printf("mapped iw5 %s to iw3 %s because it looks similar.\n", iw5_key.first.data(), iw3_key.first.data());
	//				return true;
	//			}
	//		}

	//		return false;
	//	});

	//	if (itr != iw3_keys.end())
	//	{
	//		auto map_vec_itr = mapped_keys.find(itr->second);
	//		if (map_vec_itr != mapped_keys.end())
	//		{
	//			map_vec_itr->second.push_back(iw5_key.second);
	//		}
	//		else
	//		{
	//			mapped_keys[itr->second].push_back(iw5_key.second);
	//		}
	//	}
	//}

	//// go through all maps and extract the maps that occur the most
	//for (auto& key_pair : mapped_keys)
	//{
	//	auto iw3_key = key_pair.first;
	//	auto iw5_key = 0u;

	//	std::unordered_map<std::uint32_t, std::size_t> key_count;

	//	// find best match
	//	for (auto& iw5_keys : key_pair.second)
	//	{
	//		if (key_count.find(iw5_keys) != key_count.end())
	//		{
	//			key_count[iw5_keys] += 1;
	//		}
	//		else
	//		{
	//			key_count[iw5_keys] = 1;
	//		}
	//	}

	//	// yeet
	//	auto cur_count = 0u;
	//	for (auto& iw5_keys : key_count)
	//	{
	//		if (cur_count < iw5_keys.second)
	//		{
	//			iw5_key = iw5_keys.first;
	//			cur_count = iw5_keys.second;
	//		}
	//	}

	//	// finally
	//	final_map[iw3_key] = iw5_key;
	//}

	//printf("done!\n\n");

	//printf("std::map<std::uint32_t, std::uint32_t> mapped_keys {\n");
	//for (auto& key : final_map)
	//{
	//	printf("\t{ %u, %u },\n", key.first, key.second);
	//}
	//printf("};\n");

	//while (true) {}

	//return 0;
}
