// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: momo5502 (https://github.com/momo5502)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	namespace IW3
	{
		class IMapEnts
		{
		public:
			static void dump(MapEnts* asset, ZoneMemory* mem);
		};
	}
}
