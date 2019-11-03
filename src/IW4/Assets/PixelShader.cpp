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
	namespace IW4
	{
		PixelShader* IPixelShader::parse(const std::string& name, ZoneMemory* mem, bool preferLocal)
		{
			auto path = "pixelshader\\" + name;

			if (!FileSystem::FileExists(path))
			{
				path = "techsets\\" + name + ".pixelshader";

				AssetReader read(mem);
				if (!read.open(path, preferLocal))
				{
					return nullptr;
				}

				ZONETOOL_INFO("Parsing pixelshader \"%s\"...", name.data());

				auto asset = read.read_array<PixelShader>();
				asset->name = read.read_string();
				asset->bytecode = read.read_array<DWORD>();
				read.close();

				return asset;
			}

			ZONETOOL_INFO("Parsing custom DirectX pixelshader \"%s\"...", name.data());

			auto fp = FileSystem::FileOpen(path, "rb");

			auto asset = mem->Alloc<PixelShader>();
			asset->name = mem->StrDup(name);
			asset->codeLen = FileSystem::FileSize(fp);
			asset->bytecode = mem->ManualAlloc<DWORD>(asset->codeLen);
			asset->shader = nullptr;
			fread(asset->bytecode, asset->codeLen, 1, fp);

			FileSystem::FileClose(fp);

			return asset;
		}

		void IPixelShader::init(void* asset, ZoneMemory* mem)
		{
			this->m_asset = reinterpret_cast<PixelShader*>(asset);
			this->m_name = this->m_asset->name + "_IW5"s;
		}

		void IPixelShader::init(const std::string& name, ZoneMemory* mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_FATAL("PixelShader %s not found.", &name[0]);
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).pixelshader;
			}
		}

		void IPixelShader::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IPixelShader::load_depending(IZone* zone)
		{
		}

		std::string IPixelShader::name()
		{
			return this->m_name;
		}

		std::int32_t IPixelShader::type()
		{
			return pixelshader;
		}

		void IPixelShader::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->bytecode)
			{
				dest->bytecode = buf->write_s(3, data->bytecode, data->codeLen);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IPixelShader::dump(PixelShader* asset)
		{
			AssetDumper write;
			if (!write.open("techsets\\"s + asset->name + ".pixelshader"s))
			{
				return;
			}

			write.dump_array(asset, 1);
			write.dump_string(asset->name);
			write.dump_array(asset->bytecode, asset->codeLen);
			write.close();
		}
	}
}
