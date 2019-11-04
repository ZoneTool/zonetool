// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/VertexDecl.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		VertexDecl* IVertexDecl::parse(const std::string& name, ZoneMemory* mem, bool preferLocal)
		{
			return reinterpret_cast<VertexDecl*>(IW5::IVertexDecl::parse(name, mem, preferLocal));
		}

		void IVertexDecl::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				ZONETOOL_FATAL("VertexDecl %s not found.", &name[0]);
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).vertexdecl;
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
			sizeof VertexDecl;

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
			IW5::IVertexDecl::dump(reinterpret_cast<IW5::VertexDecl*>(asset));
		}
	}
}
