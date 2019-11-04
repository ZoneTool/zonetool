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
		IMenuDef::IMenuDef()
		{
		}

		IMenuDef::~IMenuDef()
		{
		}

		void IMenuDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).menu;
		}

		void IMenuDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IMenuDef::load_depending(IZone* zone)
		{
		}

		std::string IMenuDef::name()
		{
			return this->name_;
		}

		std::int32_t IMenuDef::type()
		{
			return menu;
		}

		void IMenuDef::write_MenuData(IZone* zone, ZoneBuffer* buf, menuData_t* data)
		{
			auto dest = buf->write(data);
			memset(dest, 0, sizeof menuData_t);
		}

		void IMenuDef::write(IZone* zone, ZoneBuffer* buf)
		{
			// the only purpose for this is to decrease load time of the game...
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			// empty out menu
			memset(dest, 0, sizeof menuDef_t);

			// menudata
			if (data->data)
			{
				buf->align(3);
				write_MenuData(zone, buf, data->data);
				ZoneBuffer::ClearPointer(&dest->data);
			}

			// write name
			dest->window.name = buf->write_str(this->name());

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IMenuDef::dump(menuDef_t* asset)
		{
		}
	}
}
