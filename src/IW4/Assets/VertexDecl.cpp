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

			if (asset->streamCount > 13)
			{
				ZONETOOL_FATAL("Vertexdecl %s has more than 13 streams.", &name[0]);
			}

			// 
			for (int i = 0; i < asset->streamCount; i++)
			{
				if ((asset->streams[i].source > 9 || asset->streams[i].source < 0) ||
					(asset->streams[i].dest > 32 || asset->streams[i].dest < 0))
				{
					ZONETOOL_ERROR("%i %i", asset->streams[i].source, asset->streams[i].dest);
					ZONETOOL_FATAL("Stream %u for vertex %s is invalid!", i, name);
				}
			}

			return asset;
		}

		void IVertexDecl::init(void* asset, ZoneMemory* mem)
		{
			this->m_asset = reinterpret_cast<VertexDecl*>(asset);
			this->m_name = this->m_asset->name + "_IW5"s;
		}

		std::unordered_map<std::uint32_t, std::uint32_t> mappedStreams;

		void IVertexDecl::init(const std::string& name, ZoneMemory* mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_FATAL("VertexDecl %s not found.", &name[0]);
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).vertexdecl;
			}

			FILE* vertexDecls = fopen("vertexdecls.txt", "a");
			fprintf(vertexDecls, "%s has %i streams.\n", &name[0], this->m_asset->streamCount);
			fclose(vertexDecls);
		}

		void IVertexDecl::prepare(ZoneBuffer* buf, ZoneMemory* mem)
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

		void IVertexDecl::write(IZone* zone, ZoneBuffer* buf)
		{
			sizeof VertexDecl;

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
