#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IPhysPreset::IPhysPreset()
		{
		}

		IPhysPreset::~IPhysPreset()
		{
		}

		void IPhysPreset::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).physpreset;
		}

		void IPhysPreset::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IPhysPreset::load_depending(IZone* zone)
		{
		}

		std::string IPhysPreset::name()
		{
			return this->m_name;
		}

		std::int32_t IPhysPreset::type()
		{
			return physpreset;
		}

		void IPhysPreset::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
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
