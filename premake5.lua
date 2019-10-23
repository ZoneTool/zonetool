newoption {
	trigger = "set-version",
	description = "Sets the version information of zonetool",
}

-- Functions for locating commonly used folders
local _DependencyFolder = path.getabsolute("dep")
function DependencyFolder()
	return path.getrelative(os.getcwd(), _DependencyFolder)
end

local _ProjectFolder = path.getabsolute("src")
function ProjectFolder()
	return path.getrelative(os.getcwd(), _ProjectFolder)
end

-- ========================
-- Workspace
-- ========================
workspace "zonetool"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin"
	targetname "%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}"
	warnings "Off"

	configurations {
		"debug",
		"release",
	}

	platforms {
		"x86",
		"x64"
	}

	filter "platforms:x86"
		architecture "x86"
		defines "CPU_32BIT"
	filter {}

	filter "platforms:x64"
		architecture "x86_64"
		defines "CPU_64BIT"
	filter {}

	buildoptions "/std:c++latest"
	systemversion "latest"
	symbols "On"
	editandcontinue "Off"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks"
	}

	filter "configurations:debug"
		optimize "Debug"
		defines {
			"DEBUG",
			"_DEBUG",
		}
	filter {}

	filter "configurations:release"
		optimize "Full"
		defines {
			"NDEBUG",
		}
		flags {
			"FatalCompileWarnings",
		}
	filter{}

	includedirs {
		ProjectFolder()
	}

	startproject "ZoneTool"

	-- ========================
	-- Dependencies
	-- ========================

	include "dep/libtomcrypt.lua"
	include "dep/libtommath.lua"
	include "dep/steam_api.lua"
	include "dep/zlib.lua"
	include "dep/zstd.lua"

	-- All projects here should be in the thirdparty folder
	group "thirdparty"

	libtommath:project()
	libtomcrypt:project()
	zlib:project()
	zstd:project()

	-- Reset group
	group ""

	-- ========================
	-- Projects
	-- ========================

	include "src/ZoneTool.lua"
	include "src/ZoneUtils.lua"
	include "src/IW3.lua"
	include "src/IW4.lua"
	include "src/IW5.lua"
	include "src/CODO.lua"

	ZoneTool:project()
	ZoneUtils:project()
	IW3:project()
	IW4:project()
	IW5:project()
	CODO:project()