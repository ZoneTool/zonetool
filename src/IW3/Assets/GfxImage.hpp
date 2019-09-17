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
		class IGfxImage
		{
		public:
			// static IW4::GfxImage* GenerateIW4Image(GfxImage* image);
			static void createIWI(GfxImage* asset);
			static void dump(GfxImage* asset);
		};
	}
}
