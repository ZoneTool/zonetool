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

extern std::string currentzone;

#ifdef ZONETOOL_COMPILING
#define ZONETOOL_LIB __declspec(dllexport)
#else
#define ZONETOOL_LIB __declspec(dllimport)
#endif

namespace ZoneTool
{
	void startup();
	void register_command(const std::string& name, std::function<void(std::vector<std::string>)> cb);
}
