// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#undef min
#undef max
#undef add

#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

// Namespaces
using namespace std::literals;
using namespace string_literals;

#include "CODO.hpp"
