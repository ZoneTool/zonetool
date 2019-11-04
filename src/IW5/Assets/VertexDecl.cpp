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

		VertexDecl* IVertexDecl::parse(const std::string& name, ZoneMemory* mem, bool preferLocal)
		{
			auto path = "techsets\\" + name + ".vertexdecl";

			AssetReader read(mem);
			if (!read.open(path, preferLocal))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing vertexdecl \"%s\"...", name.data());

			auto asset = read.read_array<VertexDecl>();
			asset->name = read.read_string();
			read.close();

			return asset;
		}

		void IVertexDecl::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				ZONETOOL_FATAL("VertexDecl %s not found.", &name[0]);
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).vertexdecl;
			}
		}

		void IVertexDecl::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IVertexDecl::load_depending(IZone* zone)
		{
		}

		std::string IVertexDecl::name()
		{
			return this->name_;
		}

		std::int32_t IVertexDecl::type()
		{
			return vertexdecl;
		}

		void IVertexDecl::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
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
			if (!write.open("techsets\\"s + asset->name + ".vertexdecl"s))
			{
				return;
			}

			write.dump_array(asset, 1);
			write.dump_string(asset->name);
			write.close();
		}
	}
}
