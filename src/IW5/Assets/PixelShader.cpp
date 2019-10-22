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
		IPixelShader::IPixelShader()
		{
		}

		IPixelShader::~IPixelShader()
		{
		}

		PixelShader* IPixelShader::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem, bool preferLocal)
		{
			auto path = "pixelshader\\" + name;

			if (!FileSystem::FileExists(path))
			{
				path = "techsets\\" + name + ".pixelshader";

				AssetReader read(mem);
				if (!read.Open(path, preferLocal))
				{
					return nullptr;
				}

				ZONETOOL_INFO("Parsing pixelshader \"%s\"...", name.data());

				auto asset = read.Array<PixelShader>();
				asset->name = read.String();
				asset->bytecode = read.Array<DWORD>();
				read.Close();

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

		void IPixelShader::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_FATAL("PixelShader %s not found.", &name[0]);
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data(), 1).pixelshader;
			}
		}

		void IPixelShader::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
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

		void IPixelShader::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->bytecode)
			{
				/*buf->align(3);
				buf->write(
				ZoneBuffer::ClearPointer(&dest->bytecode);*/
				dest->bytecode = buf->write_s(3, data->bytecode, data->codeLen);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IPixelShader::dump(PixelShader* asset)
		{
			if (FileSystem::FileExists("techsets\\"s + asset->name + ".pixelshader"s))
			{
				return;
			}

			AssetDumper write;
			if (!write.Open("techsets\\"s + asset->name + ".pixelshader"s))
			{
				return;
			}

			write.Array(asset, 1);
			write.String(asset->name);
			write.Array(asset->bytecode, asset->codeLen);
			write.Close();
		}
	}
}
