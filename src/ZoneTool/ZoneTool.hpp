// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

// include zonetool utilities
#include <ZoneUtils/ZoneUtils.hpp>

// include zonetool linkers
#include <IW3/IW3.hpp>
#include <IW4/IW4.hpp>
#include <IW5/IW5.hpp>
#include <CODO/CODO.hpp>

extern std::string currentzone;

namespace ZoneTool
{
	void Startup();
}
