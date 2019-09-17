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
		IFontDef::IFontDef()
		{
		}

		IFontDef::~IFontDef()
		{
		}

		void IFontDef::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data(), 1).font;
		}

		void IFontDef::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IFontDef::load_depending(IZone* zone)
		{
			auto data = this->m_asset;

			if (data->material)
			{
				zone->AddAssetOfType(material, data->material->name);
			}

			if (data->glowMaterial)
			{
				zone->AddAssetOfType(material, data->glowMaterial->name);
			}
		}

		std::string IFontDef::name()
		{
			return this->m_name;
		}

		std::int32_t IFontDef::type()
		{
			return font;
		}

		void IFontDef::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			Font_s;

			buf->push_stream(3);
			START_LOG_STREAM;

			// TODO
			dest->name = buf->write_str(this->name());

			if (data->material)
			{
				dest->material = reinterpret_cast<Material*>(
					zone->GetAssetPointer(material, data->material->name)
				);
			}

			if (data->glowMaterial)
			{
				dest->glowMaterial = reinterpret_cast<Material*>(
					zone->GetAssetPointer(material, data->glowMaterial->name)
				);
			}

			if (data->glyphs)
			{
				buf->align(3);
				buf->write(data->glyphs, data->glyphCount);
				ZoneBuffer::ClearPointer(&dest->glyphs);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IFontDef::dump(Font_s* asset)
		{
		}
	}
}
