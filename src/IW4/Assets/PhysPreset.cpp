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
		void IPhysPreset::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).physpreset;
		}

		void IPhysPreset::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IPhysPreset::load_depending(IZone* zone)
		{
		}

		std::string IPhysPreset::name()
		{
			return this->name_;
		}

		std::int32_t IPhysPreset::type()
		{
			return physpreset;
		}

		void IPhysPreset::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);

			dest->name = buf->write_str(this->name());

			if (data->sndAliasPrefix)
			{
				dest->sndAliasPrefix = buf->write_str(data->sndAliasPrefix);
			}

			buf->pop_stream();
		}

		void IPhysPreset::dump(PhysPreset* asset)
		{
		}
	}
}
