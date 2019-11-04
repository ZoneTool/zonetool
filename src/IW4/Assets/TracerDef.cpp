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
		ITracerDef::ITracerDef()
		{
		}

		ITracerDef::~ITracerDef()
		{
		}

		void ITracerDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).tracer;
		}

		void ITracerDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ITracerDef::load_depending(IZone* zone)
		{
		}

		std::string ITracerDef::name()
		{
			return this->name_;
		}

		std::int32_t ITracerDef::type()
		{
			return tracer;
		}

		void ITracerDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->material)
			{
				dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(material, data->material->name));
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ITracerDef::dump(TracerDef* asset)
		{
		}
	}
}
