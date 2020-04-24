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
		class IClipMap
		{
		public:
			static void dump(clipMap_t* asset, ZoneMemory* mem);
		};
	}
}
