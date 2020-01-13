// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/GfxImage.hpp"


namespace ZoneTool
{
	namespace IW4
	{
		std::string IGfxImage::clean_name(const std::string& name)
		{
			auto newName = name;

			for (auto i = 0u; i < name.size(); i++)
			{
				switch (newName[i])
				{
				case '*':
					newName[i] = '_';
					break;
				}
			}

			return newName;
		}

		GfxImage* IGfxImage::parse(const std::string& name, ZoneMemory* mem)
		{
			auto path = "images\\" + this->clean_name(name) + ".ffimg";

			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			auto fp = FileSystem::FileOpen(path, "rb");
			if (!fp)
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing GfxImage \"%s\"...", name.data());

			auto reader = FileSystem::ToReader(fp);

			auto img = mem->Alloc<GfxImage>();
			img->mapType = reader->Read<char>();
			img->semantic = reader->Read<char>();
			img->category = reader->Read<char>();
			img->flags = reader->Read<char>();
			img->cardMemory = reader->Read<int>();
			img->dataLen1 = reader->Read<int>();
			img->dataLen2 = reader->Read<int>();
			img->height = reader->Read<int>();
			img->width = reader->Read<int>();
			img->depth = reader->Read<int>();
			img->loaded = reader->Read<char>();
			img->name = mem->StrDup(reader->ReadString());

			auto loaddef = mem->Alloc<GfxImageLoadDef>();
			loaddef->mipLevels = reader->Read<char>();
			loaddef->flags = reader->Read<char>();
			loaddef->dimensions[0] = reader->Read<int>();
			loaddef->dimensions[1] = reader->Read<int>();
			loaddef->dimensions[2] = reader->Read<int>();
			loaddef->format = reader->Read<int>();
			loaddef->dataSize = reader->Read<int>();

			GfxImageLoadDef* finalLoaddef = nullptr;

			if (loaddef->dataSize > 4)
			{
				finalLoaddef = mem->ManualAlloc<GfxImageLoadDef>(
					sizeof GfxImageLoadDef +
					(loaddef->dataSize - 4));
				memcpy(finalLoaddef, loaddef, sizeof GfxImageLoadDef);
				reader->ReadManual(&finalLoaddef->texture, loaddef->dataSize, 1);
			}
			else
			{
				finalLoaddef = loaddef;
			}


			img->texture = finalLoaddef;

			FileSystem::FileClose(fp);

			return img;
		}

		void IGfxImage::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);
			this->isMapImage = (this->name_.size() >= 6)
				                   ? ((this->name_.substr(0, 6) == "*light" || this->name_.substr(0, 6) == "*refle" ||
					                      this->name_ == "$outdoor")
					                      ? true
					                      : false)
				                   : false;

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data()).gfximage;
			}
		}

		void IGfxImage::init(void* asset, ZoneMemory* mem)
		{
			this->asset_ = reinterpret_cast<GfxImage*>(asset);
			this->name_ = this->asset_->name;
			this->isMapImage = (this->name_.size() >= 6)
				                   ? ((this->name_.substr(0, 6) == "*light" || this->name_.substr(0, 6) == "*refle" ||
					                      this->name_ == "$outdoor")
					                      ? true
					                      : false)
				                   : false;

			auto parsed = this->parse(this->name_, mem);
			if (parsed)
			{
				this->asset_ = parsed;
			}
		}

		void IGfxImage::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IGfxImage::load_depending(IZone* zone)
		{
		}

		std::string IGfxImage::name()
		{
			return this->name_;
		}

		std::int32_t IGfxImage::type()
		{
			return image;
		}
		
		void IGfxImage::write(IZone* zone, ZoneBuffer* buf)
		{
			if (zone->get_target() == zone_target::pc)
			{
				IW5::IGfxImage::dump_iwi(this->name());

				auto data = this->asset_;
				auto dest = buf->write(data);

				buf->push_stream(3);
				START_LOG_STREAM;

				dest->name = buf->write_str(this->name());

				// set loaded to false
				dest->loaded = false;

				buf->push_stream(0);
				if (data->texture)
				{
					buf->align(3);

					auto desttext = buf->at<GfxImageLoadDef>();
					buf->write_stream(data->texture, sizeof GfxImageLoadDef - sizeof std::uintptr_t);

					if (isMapImage && desttext->dataSize)
					{
						buf->write_stream(&data->texture->texture, data->texture->dataSize);
					}
					else
					{
						desttext->dataSize = 0;
					}

					ZoneBuffer::ClearPointer(&dest->texture);
				}
				buf->pop_stream();

				END_LOG_STREAM;
				buf->pop_stream();
			}
			else
			{
				alpha::GfxImage alpha_image = {};

				// transform iwi
				if (!FileSystem::FileExists(va("images/%s.iwi", this->name().data())) && !this->isMapImage)
				{
					ZONETOOL_FATAL("Image %s is missing!", this->name().data());
				}

				std::vector<std::uint8_t> pixels;
				auto fp = FileSystem::FileOpen(va("images/%s.iwi", this->name().data()), "rb");
				if (fp || this->isMapImage)
				{
					if (!this->isMapImage)
					{
						auto file_size = FileSystem::FileSize(fp);
						auto img_data = FileSystem::ReadBytes(fp, file_size);

						auto iwi_header = (GfxImageFileHeader*)img_data.data();

						sizeof GfxImageFileHeader;

						auto pixel_data = img_data.data() + 32;
						auto pixel_data_size = img_data.size() - 32;

						pixels.resize(pixel_data_size);
						memcpy(&pixels[0], pixel_data, pixel_data_size);

						// add image to imagepak


						// zone images
						alpha_image.cached = false;
						alpha_image.cardMemory.platform[0] = pixels.size();

						// pakfile images
						//alpha_image.cached = true;
						//alpha_image.cardMemory.platform[0] = 0;
						//alpha_image.streams[0].width = iwi_header->dimensions[0];
						//alpha_image.streams[0].height = iwi_header->dimensions[1];
						//alpha_image.streams[0].pixelSize = 1;
						//buf->add_image(pixels);

						alpha_image.format = 0x1A200154;
						alpha_image.width = iwi_header->dimensions[0];
						alpha_image.height = iwi_header->dimensions[1];
						alpha_image.depth = iwi_header->dimensions[2];
						alpha_image.levelCount = 1;
						alpha_image.mapType = 3;
						alpha_image.category = 3;
					}
					else
					{
						pixels.resize(this->asset_->texture->dataSize);
						memcpy(&pixels[0], &this->asset_->texture->texture, pixels.size());
						
						alpha_image.cached = false;
						alpha_image.cardMemory.platform[0] = pixels.size();
						alpha_image.format = 0x1A200154;
						alpha_image.width = this->asset_->width;
						alpha_image.height = this->asset_->height;
						alpha_image.depth = this->asset_->depth;
						alpha_image.levelCount = 1;
						alpha_image.mapType = 3;
						alpha_image.category = 3;
					}
					
				}
				else
				{
					ZONETOOL_FATAL("Cannot open image %s!", this->name().data());
				}
				

				auto data = &alpha_image;
				auto dest = buf->write(data);

				buf->push_stream(3);

				dest->name = buf->write_str(this->name());

				buf->push_stream(1);
				if (data->cardMemory.platform[0])
				{
					buf->align(4095);
					buf->write(pixels.data(), pixels.size());
					ZoneBuffer::ClearPointer(&dest->pixels);
				}
				else
				{
					dest->pixels = nullptr;
				}
				buf->pop_stream();
				
				buf->pop_stream();

				endian_convert(&dest->name);
				endian_convert(&dest->format);
				endian_convert(&dest->width);
				endian_convert(&dest->height);
				endian_convert(&dest->depth);
				endian_convert(&dest->pixels);
				endian_convert(&dest->cardMemory.platform[0]);
				for (auto i = 0; i < 4; i++)
				{
					endian_convert(&dest->streams[i].width);
					endian_convert(&dest->streams[i].height);
					endian_convert(&dest->streams[i].pixelSize);
				}
			}
		}

		// Legacy cancer code
		void fwritestr(FILE* file, const char* str)
		{
			if (!str)
				return;
			while (*str)
			{
				fwrite(str, 1, 1, file);
				str++;
			}
			fwrite(str, 1, 1, file);
		}

		void fwriteint(FILE* file, int value)
		{
			int _val = value;
			fwrite(&_val, 4, 1, file);
		}

		void fwriteuint(FILE* file, unsigned int value)
		{
			unsigned int _val = value;
			fwrite(&_val, 4, 1, file);
		}

		void fwritechar(FILE* file, char c)
		{
			char _val = c;
			fwrite(&_val, 1, 1, file);
		}

		char cleanAssetName[50];

		char* ClearAssetName(char* name, int maxSize = 50)
		{
			int size = strnlen(name, maxSize);
			char* newName = cleanAssetName;
			memset(newName, 0, size + 1);
			strncpy(newName, name, maxSize);
			for (int i = 0; i < size; i++)
			{
				switch (newName[i])
				{
				case '*':
					newName[i] = '_';
					break;
				}
			}
			return newName;
		}

		void IGfxImage::dump(GfxImage* asset)
		{
			if (asset->texture && asset->texture->dataSize)
			{
				char* newName = ClearAssetName((char*)asset->name);
				auto fp = FileSystem::FileOpen("images/"s + newName + ".ffImg"s, "wb");

				if (!fp) return;

#define fwstr(_str) fwritestr(fp, _str)
#define fwint(_int) fwriteint(fp, _int)
#define fwchr(_chr) fwritechar(fp, _chr)
#define frstr() freadstr(fp)
#define frint() freadint(fp)
#define frchr() freadchar(fp)

				// Header
				fwchr(asset->mapType);
				fwchr(asset->semantic);
				fwchr(asset->category);
				fwchr(asset->flags);
				fwint((int)asset->cardMemory);
				fwint(asset->dataLen1);
				fwint(asset->dataLen2);
				fwint(asset->height);
				fwint(asset->width);
				fwint(asset->depth);
				fwstr(asset->name);

				// LoadDef
				fwchr(asset->texture->mipLevels);
				fwchr(asset->texture->flags);
				fwint(asset->texture->dimensions[0]);
				fwint(asset->texture->dimensions[1]);
				fwint(asset->texture->dimensions[2]);
				fwint(asset->texture->format);
				fwint(asset->texture->dataSize);

				fwrite(&asset->texture->texture, 1, asset->texture->dataSize, fp);

				FileSystem::FileClose(fp);
			}
		}
	}
}
