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

std::uint32_t read_sortkey(const std::filesystem::path& path)
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
	return json["sortKey"].get<std::uint32_t>();
}

int main(int argc, char** argv)
{
	//file img1("imagefile1.pakm");
	//file img2("imagefile2.pakm");
	//file img3("imagefile3.pakm");
	//file img4("imagefile4.pakm");

	//std::vector<file*> paks = { &img1, &img2, &img3, &img4 };

	//file ff("mp_shipment.ff");
	//file loadscreen("mp_shipment_load.ff");

	//// alloc new image pak
	//ZoneTool::PakFile img5(269);

	//handle_entries(ff, paks, img5);
	//handle_entries(loadscreen, paks, img5);

	//// destroy paks
	//for (auto& pak : paks)
	//{
	//	pak->destroy();
	//}

	//// save data
	//ff.save();
	//loadscreen.save();

	//// save image pak
	//img5.save("imagefile5.pak");

	//return 0;

	std::vector<std::filesystem::path> iw3_materials;
	std::vector<std::filesystem::path> iw5_materials;

	std::map<std::uint32_t, std::uint32_t> mapped_keys;

	for (auto& itr : std::filesystem::directory_iterator("Z:\\Sortkey mapping\\iw3"))
	{
		if (std::filesystem::is_regular_file(itr))
		{
			iw3_materials.push_back(itr.path());
		}
	}

	for (auto& itr : std::filesystem::directory_iterator("Z:\\Sortkey mapping\\iw5"))
	{
		if (std::filesystem::is_regular_file(itr))
		{
			iw5_materials.push_back(itr.path());
		}
	}

	for (auto& iw3_mat : iw3_materials)
	{
		auto itr = std::find_if(iw5_materials.begin(), iw5_materials.end(), [iw3_mat](auto& iw5)
		{
			return iw5.filename() == iw3_mat.filename();
		});

		if (itr != iw5_materials.end())
		{
			printf("material %s exists in both games! using for comparison.\n", itr->filename().string().data());

			auto iw3_key = read_sortkey(iw3_mat);
			auto iw5_key = read_sortkey(*itr);
			auto mapped_itr = mapped_keys.find(iw3_key);

			if (mapped_itr == mapped_keys.end())
			{
				mapped_keys[iw3_key] = iw5_key;
			}
			else
			{
				if (mapped_itr->second != iw5_key)
				{
					printf("FOOPSIE WOOPS! %u was previously mapped to %u, but we just found a material trying to map key %u to %u\n", mapped_itr->first, mapped_itr->second, mapped_itr->first, iw5_key);
				}
			}
		}
	}

	printf("std::map<std::uint32_t, std::uint32_t> mapped_keys {\n");
	for (auto& key : mapped_keys)
	{
		printf("\t{ %u, %u },\n", key.first, key.second);
	}
	printf("};\n");

	while (true) {}

	return 0;
}
