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
		IVertexDecl::IVertexDecl()
		{
		}

		IVertexDecl::~IVertexDecl()
		{
		}

		VertexDecl* IVertexDecl::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem, bool preferLocal)
		{
			auto path = "techsets\\" + name + ".vertexdecl";

			AssetReader read(mem);
			if (!read.Open(path, preferLocal))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing vertexdecl \"%s\"...", name.data());

			auto asset = read.Array<VertexDecl>();
			asset->name = read.String();
			read.Close();

			return asset;
		}

		void IVertexDecl::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_FATAL("VertexDecl %s not found.", &name[0]);
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data(), 1).vertexdecl;
			}
		}

		void IVertexDecl::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IVertexDecl::load_depending(IZone* zone)
		{
		}

		std::string IVertexDecl::name()
		{
			return this->m_name;
		}

		std::int32_t IVertexDecl::type()
		{
			return vertexdecl;
		}

		void IVertexDecl::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IVertexDecl::dump(VertexDecl* asset)
		{
			if (FileSystem::FileExists("techsets\\"s + asset->name + ".vertexdecl"s))
			{
				return;
			}

			AssetDumper write;
			if (!write.Open("techsets\\"s + asset->name + ".vertexdecl"s))
			{
				return;
			}

			write.Array(asset, 1);
			write.String(asset->name);
			write.Close();
		}
	}
}
