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

			if (zone->get_target() != zone_target::pc)
			{
				endian_convert(&dest->name);
				endian_convert(&dest->bounce);
				endian_convert(&dest->bulletForceScale);
				endian_convert(&dest->explosiveForceScale);
				endian_convert(&dest->friction);
				endian_convert(&dest->mass);
				endian_convert(&dest->piecesSpreadFraction);
				endian_convert(&dest->piecesUpwardVelocity);
				endian_convert(&dest->sndAliasPrefix);
				endian_convert(&dest->tempDefaultToCylinder);
				endian_convert(&dest->type);
			}
		}

		void IPhysPreset::dump(PhysPreset* asset)
		{
		}
	}
}
