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

		void ITracerDef::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).tracer;
		}

		void ITracerDef::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void ITracerDef::load_depending(IZone* zone)
		{
		}

		std::string ITracerDef::name()
		{
			return this->m_name;
		}

		std::int32_t ITracerDef::type()
		{
			return tracer;
		}

		void ITracerDef::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->material)
			{
				dest->material = reinterpret_cast<Material*>(zone->GetAssetPointer(material, data->material->name));
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ITracerDef::dump(TracerDef* asset)
		{
		}
	}
}
