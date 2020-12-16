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

#define ZONETOOL_BRANDING "Compiled using ZoneTool by RektInator."

namespace ZoneTool
{
	namespace IW5
	{
		RawFile* IRawFile::parse(const std::string& name, ZoneMemory* mem)
		{
			if (FileSystem::FileExists(name))
			{
				auto* rawfile = mem->Alloc<RawFile>();
				rawfile->name = mem->StrDup(name);

				auto* file = FileSystem::FileOpen(name, "rb"s);
				if (file)
				{
					const auto size = FileSystem::FileSize(file);
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

				return rawfile;
			}

			return nullptr;
		}

		void IRawFile::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = parse(name, mem);

			if (name == FileSystem::GetFastFile())
			{
				this->asset_ = mem->Alloc<RawFile>();
				this->asset_->name = mem->StrDup(name);
				this->asset_->buffer = mem->StrDup(ZONETOOL_BRANDING);
				this->asset_->len = strlen(this->asset_->buffer);
			}
			else if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data(), 1).rawfile;
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
			auto* data = this->asset_;
			auto* dest = buf->write<RawFile>(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->buffer)
			{
				buf->align(0);
				buf->write(data->buffer, data->compressedLen ? data->compressedLen : data->len + 1);
				ZoneBuffer::clear_pointer(&dest->buffer);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IRawFile::dump(RawFile* asset)
		{
			auto fp = FileSystem::FileOpen(asset->name, "wb");

			if (fp)
			{
				if (asset->compressedLen > 0)
				{
					std::vector<std::uint8_t> uncompressed;
					uncompressed.resize(asset->len);

					auto status = uncompress(uncompressed.data(), (uLongf*)&asset->len, (Bytef*)asset->buffer,
					                         asset->compressedLen);

					fwrite(uncompressed.data(), uncompressed.size(), 1, fp);
				}
				else if (asset->len > 0)
				{
					fwrite(asset->buffer, asset->len, 1, fp);
				}
			}

			FileSystem::FileClose(fp);
		}
	}
}
