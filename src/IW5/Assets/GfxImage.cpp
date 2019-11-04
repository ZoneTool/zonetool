// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		IGfxImage::IGfxImage()
		{
		}

		IGfxImage::~IGfxImage()
		{
		}

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
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data(), 1).gfximage;
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

		struct IW3_GfxImageFileHeader
		{
			char tag[3];
			char version;
			char format;
			std::uint8_t flags;
			__int16 dimensions[3];
			int fileSizeForPicmip[4];
		};

		enum class iw3_file_image_flags_t : std::uint8_t
		{
			IMG_FLAG_NOPICMIP = 0x1,
			IMG_FLAG_NOMIPMAPS = 0x2,
			IMG_FLAG_CUBEMAP = 0x4,
			IMG_FLAG_VOLMAP = 0x8,
			IMG_FLAG_STREAMING = 0x10,
			IMG_FLAG_LEGACY_NORMALS = 0x20,
			IMG_FLAG_CLAMP_U = 0x40,
			IMG_FLAG_CLAMP_V = 0x80,
		};
		enum class file_image_flags_t : std::uint32_t
		{
			IMG_FLAG_NOPICMIP = 0x1,
			IMG_FLAG_NOMIPMAPS = 0x2,
			IMG_FLAG_STREAMING = 0x4,
			IMG_FLAG_LEGACY_NORMALS = 0x8,
			IMG_FLAG_CLAMP_U = 0x10,
			IMG_FLAG_CLAMP_V = 0x20,
			IMG_FLAG_ALPHA_WEIGHTED_COLORS = 0x40,
			IMG_FLAG_DXTC_APPROX_WEIGHTS = 0x80,
			IMG_FLAG_GAMMA_NONE = 0x0,
			IMG_FLAG_GAMMA_SRGB = 0x100,
			IMG_FLAG_GAMMA_PWL = 0x200,
			IMG_FLAG_GAMMA_2 = 0x300,
			IMG_FLAG_MAPTYPE_2D = 0x0,
			IMG_FLAG_MAPTYPE_CUBE = 0x10000,
			IMG_FLAG_MAPTYPE_3D = 0x20000,
			IMG_FLAG_MAPTYPE_1D = 0x30000,
			IMG_FLAG_NORMALMAP = 0x40000,
			IMG_FLAG_INTENSITY_TO_ALPHA = 0x80000,
			IMG_FLAG_DYNAMIC = 0x1000000,
			IMG_FLAG_RENDER_TARGET = 0x2000000,
			IMG_FLAG_SYSTEMMEM = 0x4000000,
		};
		
		void translate_flags(IW3_GfxImageFileHeader* iw3_header, GfxImageFileHeader* iw5_header, iw3_file_image_flags_t iw3_flag, file_image_flags_t iw5_flag)
		{
			if ((iw3_header->flags & static_cast<std::uint8_t>(iw3_flag)) == static_cast<std::uint8_t>(iw3_flag))
			{
				iw5_header->flags |= static_cast<std::uint32_t>(iw5_flag);
			}
		}
		
		void IGfxImage::dump_iwi(const std::string& name)
		{
			if (FileSystem::FileExists(name + ".iwi") && !std::filesystem::exists(
				"main\\iw4_images\\" + name + ".iwi"))
			{
				auto fp = fopen(
					va("main\\%s\\images\\%s.iwi", FileSystem::GetFastFile().data(), name.data()).data(), "wb");

				if (fp)
				{
					auto origfp = FileSystem::FileOpen(name + ".iwi", "rb");

					if (origfp)
					{
						auto origsize = FileSystem::FileSize(origfp);
						auto bytes = FileSystem::ReadBytes(origfp, origsize);

						if (bytes.size() > 3)
						{
							const auto version = bytes[3];

							if (version != 8 && version != 9)
							{
								if (version == 6)
								{
									constexpr auto iw5_header_size = sizeof(GfxImageFileHeader);
									constexpr auto iw3_header_size = sizeof(IW3_GfxImageFileHeader);
									
									ZONETOOL_INFO("Converting IWI %s from version %u to version %u...", name.data(), version, 8);

									// parse iw3 header
									IW3_GfxImageFileHeader iw3_header;
									memcpy(&iw3_header, bytes.data(), iw3_header_size);

									// generate iw5 header
									GfxImageFileHeader header = {};
									memset(&header, 0, sizeof GfxImageFileHeader);
									
									memcpy(header.tag, iw3_header.tag, 3);
									header.version = 8;
									header.format = iw3_header.format;
									header.unused = false;
									memcpy(header.dimensions, iw3_header.dimensions, sizeof(__int16[3]));
									memcpy(header.fileSizeForPicmip, iw3_header.fileSizeForPicmip, sizeof(int[4]));

									// transform iwi flags
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_NOPICMIP, file_image_flags_t::IMG_FLAG_NOPICMIP);
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_NOMIPMAPS, file_image_flags_t::IMG_FLAG_NOMIPMAPS);
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_CUBEMAP, file_image_flags_t::IMG_FLAG_MAPTYPE_CUBE);
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_VOLMAP, file_image_flags_t::IMG_FLAG_MAPTYPE_3D);
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_STREAMING, file_image_flags_t::IMG_FLAG_STREAMING);
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_LEGACY_NORMALS, file_image_flags_t::IMG_FLAG_LEGACY_NORMALS);
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_CLAMP_U, file_image_flags_t::IMG_FLAG_CLAMP_U);
									translate_flags(&iw3_header, &header, iw3_file_image_flags_t::IMG_FLAG_CLAMP_V, file_image_flags_t::IMG_FLAG_CLAMP_V);
									
									// write iw5 header
									fwrite(&header, iw5_header_size, 1, fp);
									
									// write iw3 image buffer
									fwrite(&bytes[iw3_header_size], bytes.size() - iw3_header_size, 1, fp);
									fclose(fp);
								}
								else
								{
									ZONETOOL_FATAL("IWI of version %u is not supported for conversion. IWI file was %s.", version, name.data());
								}

								return;
							}
						}

						// write original data
						fwrite(&bytes[0], bytes.size(), 1, fp);
						fclose(origfp);
					}

					fclose(fp);
				}
			}
		}
		
		void IGfxImage::write(IZone* zone, ZoneBuffer* buf)
		{
			dump_iwi(this->name());

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
