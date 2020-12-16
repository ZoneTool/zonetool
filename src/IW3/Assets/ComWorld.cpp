// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW4/Assets/ComWorld.hpp"

namespace ZoneTool::IW3
{
	void IComWorld::dump(ComWorld* asset, [[maybe_unused]] ZoneMemory* mem)
	{
		IW4::IComWorld::dump(reinterpret_cast<IW4::ComWorld*>(asset));
	}
}
