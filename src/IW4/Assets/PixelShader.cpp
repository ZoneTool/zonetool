// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/PixelShader.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		PixelShader* IPixelShader::parse(const std::string& name, ZoneMemory* mem, bool preferLocal)
		{
			return reinterpret_cast<PixelShader*>(IW5::IPixelShader::parse(name, mem, preferLocal));
		}

		void IPixelShader::init(void* asset, ZoneMemory* mem)
		{
			this->asset_ = reinterpret_cast<PixelShader*>(asset);
			this->name_ = this->asset_->name + "_IW5"s;
		}

		void IPixelShader::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				ZONETOOL_FATAL("PixelShader %s not found.", &name[0]);
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).pixelshader;
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
			return this->name_;
		}

		std::int32_t IPixelShader::type()
		{
			return pixelshader;
		}

		void IPixelShader::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
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
			return IW5::IPixelShader::dump(reinterpret_cast<IW5::PixelShader*>(asset));
		}
	}
}
