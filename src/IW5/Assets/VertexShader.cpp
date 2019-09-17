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
		IVertexShader::IVertexShader()
		{
		}

		IVertexShader::~IVertexShader()
		{
		}

		VertexShader* IVertexShader::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem, bool preferLocal)
		{
			auto path = "vertexshader\\" + name;

			if (!FileSystem::FileExists(path))
			{
				path = "techsets\\" + name + ".vertexshader";

				if (!FileSystem::FileExists(path))
				{
					return nullptr;
				}

				AssetReader read(mem);
				if (!read.Open(path))
				{
					return nullptr;
				}

				ZONETOOL_INFO("Parsing vertexshader \"%s\"...", name.data());

				auto asset = read.Array<VertexShader>();
				asset->name = read.String();
				asset->bytecode = read.Array<DWORD>();
				read.Close();

				return asset;
			}

			ZONETOOL_INFO("Parsing custom DirectX vertexshader \"%s\"...", name.data());

			auto fp = FileSystem::FileOpen(path, "rb");

			auto asset = mem->Alloc<VertexShader>();
			asset->name = mem->StrDup(name);
			asset->codeLen = FileSystem::FileSize(fp);
			asset->bytecode = mem->ManualAlloc<DWORD>(asset->codeLen);
			asset->shader = nullptr;
			fread(asset->bytecode, asset->codeLen, 1, fp);

			FileSystem::FileClose(fp);

			return asset;
		}

		void IVertexShader::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_FATAL("VertexShader %s not found.", &name[0]);
				// this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).vertexshader;
			}
		}

		void IVertexShader::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IVertexShader::load_depending(IZone* zone)
		{
		}

		std::string IVertexShader::name()
		{
			return this->m_name;
		}

		std::int32_t IVertexShader::type()
		{
			return vertexshader;
		}

		void IVertexShader::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->bytecode)
			{
				buf->align(3);
				buf->write(data->bytecode, data->codeLen);
				ZoneBuffer::ClearPointer(&dest->bytecode);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IVertexShader::dump(VertexShader* asset)
		{
			if (FileSystem::FileExists("techsets\\"s + asset->name + ".vertexshader"s))
			{
				return;
			}

			AssetDumper write;
			if (!write.Open("techsets\\"s + asset->name + ".vertexshader"s))
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
