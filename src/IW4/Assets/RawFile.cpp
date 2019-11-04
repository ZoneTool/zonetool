// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include <zlib.h>

#define ZONETOOL_BRANDING		"Compiled using ZoneTool by RektInator, localhost & X3RX35."

namespace ZoneTool
{
	namespace IW4
	{
		RawFile* IRawFile::parse(const std::string& name, ZoneMemory* mem)
		{
			if (FileSystem::FileExists(name))
			{
				auto rawfile = mem->Alloc<RawFile>();
				rawfile->name = mem->StrDup(name);

				auto file = FileSystem::FileOpen(name, "rb"s);
				if (file)
				{
					auto size = FileSystem::FileSize(file);
					auto data = FileSystem::ReadBytes(file, size);

					ZoneBuffer buf(data);
					auto compressed = buf.compress_zlib();

					rawfile->len = size;
					rawfile->compressedLen = compressed.size();
					rawfile->buffer = mem->Alloc<char>(size);
					memcpy(
						const_cast<char*>(rawfile->buffer),
						compressed.data(),
						size);

					FileSystem::FileClose(file);
				}
				else
				{
					rawfile->buffer = ZONETOOL_BRANDING;
					rawfile->compressedLen = strlen(ZONETOOL_BRANDING);
					rawfile->len = strlen(ZONETOOL_BRANDING);
				}

				return rawfile;
			}

			return nullptr;
		}

		void IRawFile::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data()).rawfile;
			}
		}

		void IRawFile::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IRawFile::load_depending(IZone* zone)
		{
		}

		std::string IRawFile::name()
		{
			return this->name_;
		}

		std::int32_t IRawFile::type()
		{
			return rawfile;
		}

		void IRawFile::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write<RawFile>(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (!data->compressedLen && !data->len)
			{
				// clear out buffer pointer
				dest->buffer = nullptr;
			}
			else if (data->buffer)
			{
				buf->write(data->buffer, data->compressedLen);
				ZoneBuffer::ClearPointer(&dest->buffer);
			}

			if (zone->get_target() != zone_target::pc)
			{
				endian_convert(&dest->name);
				endian_convert(&dest->compressedLen);
				endian_convert(&dest->len);
				endian_convert(&dest->buffer);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IRawFile::dump(RawFile* asset)
		{
			auto fp = FileSystem::FileOpen(asset->name, "wb");

			if (fp)
			{
				if (asset->compressedLen)
				{
					std::vector<std::uint8_t> uncompressed;
					uncompressed.resize(asset->len);

					auto status = uncompress(uncompressed.data(), (uLongf*)&asset->len, (Bytef*)asset->buffer,
					                         asset->compressedLen);

					fwrite(uncompressed.data(), uncompressed.size(), 1, fp);
				}
			}

			FileSystem::FileClose(fp);
		}

		IRawFile::IRawFile()
		{
		}

		IRawFile::~IRawFile()
		{
		}
	}
}
