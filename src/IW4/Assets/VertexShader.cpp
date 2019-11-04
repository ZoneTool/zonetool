// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/VertexShader.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		VertexShader* IVertexShader::parse(const std::string& name, ZoneMemory* mem, bool preferLocal)
		{
			return reinterpret_cast<VertexShader*>(IW5::IVertexShader::parse(name, mem, preferLocal));
		}

		void IVertexShader::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				ZONETOOL_FATAL("VertexShader %s not found.", &name[0]);
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).vertexshader;
			}
		}

		void IVertexShader::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IVertexShader::load_depending(IZone* zone)
		{
		}

		std::string IVertexShader::name()
		{
			return this->name_;
		}

		std::int32_t IVertexShader::type()
		{
			return vertexshader;
		}

		void IVertexShader::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
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
			IW5::IVertexShader::dump(reinterpret_cast<IW5::VertexShader*>(asset));
		}
	}
}
