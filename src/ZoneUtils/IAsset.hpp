// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	class IAsset
	{
	public:
		virtual void init(const std::string& name, ZoneMemory* mem)
		{
		}

		virtual void init(void* asset, ZoneMemory* mem)
		{
		}

		virtual void prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		virtual std::string name() { return ""; }
		virtual std::int32_t type() { return -1; }
		virtual void* pointer() { return nullptr; }

		virtual void write(IZone* zone, ZoneBuffer* buffer)
		{
		}

		virtual void load_depending(IZone* zone)
		{
		}
	};
}
