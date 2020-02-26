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

template <std::uint32_t Offset> class compressed_file : public file
{
public:
	compressed_file(const std::string& filename)
	{
		auto fp = fopen(filename.data(), "rb");
		if (fp)
		{
			std::vector<std::uint8_t> compressed_buffer;

			const auto filesize = std::filesystem::file_size(filename);
			compressed_buffer.resize(filesize);
			fread(&compressed_buffer[0], 1, filesize, fp);
			fclose(fp);

			buffer_.resize(filesize);
			uLongf buffer_len = buffer_.size();
			uncompress(buffer_.data(), &buffer_len, &compressed_buffer[Offset], compressed_buffer.size() - Offset);

			compressed_buffer.clear();
		}
	}

private:

};
using compressed_pak = compressed_file<0xC>;

void endian_convert_entries(ZoneTool::XAssetStreamFile* entries, std::uint32_t count)
{
	for (auto i = 0u; i < count; i++)
	{
		ZoneTool::endian_convert(&entries[i].fileIndex);
		ZoneTool::endian_convert(&entries[i].offset);
		ZoneTool::endian_convert(&entries[i].offsetEnd);
	}
}

void handle_entries(file& ff, std::vector<compressed_pak*>& paks, ZoneTool::PakFile& img5)
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
		auto new_image_data = img5.add_entry(image_data, entries[i].offsetEnd - entries[i].offset);

		// replace existing image data
		entries[i].offset = new_image_data.first;
		entries[i].offsetEnd = new_image_data.second;
		entries[i].fileIndex = 5;
	}

	// convert back to big endian
	endian_convert_entries(entries, count);
}

int main(int argc, char** argv)
{
	compressed_pak img1("imagefile1.pakm");
	compressed_pak img2("imagefile2.pakm");
	compressed_pak img3("imagefile3.pakm");
	compressed_pak img4("imagefile4.pakm");

	std::vector<compressed_pak*> paks = { &img1, &img2, &img3, &img4 };

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
}
