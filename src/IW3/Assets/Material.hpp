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
	namespace IW3
	{
		class IMaterial
		{
		public:
			static void dump(Material* asset, ZoneMemory* mem);
			static void dump_statebits(Material* mat);
		};
	}
}
