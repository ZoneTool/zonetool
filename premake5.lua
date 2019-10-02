newoption {
	trigger = "set-version",
	description = "Sets the version information of zonetool",
}

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
		"win32",
		"win64",
	}

	filter "platforms:win32"
		architecture "x86"
		defines "CPU_32BIT"
	filter {}

	filter "platforms:win64"
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

	configuration "windows"
		defines {
			"_WINDOWS",
			"WIN32",
		}
	configuration{}

	configuration "release"
		optimize "Full"
		defines {
			"NDEBUG",
		}
		flags {
			"FatalCompileWarnings",
		}
	configuration{}

	configuration "debug"
		optimize "Debug"
		defines {
			"DEBUG",
			"_DEBUG",
		}
	configuration {}

	startproject "ZoneTool"

	project "ZoneTool"
		kind "SharedLib"
		language "C++"
		dependson "ZoneUtils"
		dependson "IW3"
		dependson "IW4"
		dependson "IW5"
		dependson "CODO"

		pchheader "stdafx.hpp"
		pchsource "src/ZoneTool/stdafx.cpp"

		files {
			"./src/ZoneTool/**.h",
			"./src/ZoneTool/**.hpp",
			"./src/ZoneTool/**.cpp",
		}

		links {
			"IW3-%{cfg.platform}-%{cfg.buildcfg}",
			"IW4-%{cfg.platform}-%{cfg.buildcfg}",
			"IW5-%{cfg.platform}-%{cfg.buildcfg}",
			"CODO-%{cfg.platform}-%{cfg.buildcfg}",
			"ZoneUtils-%{cfg.platform}-%{cfg.buildcfg}",
			"steam_api",
			"tomcrypt",
			"tommath",
			"zlib",
			"zstd",
		}

		syslibdirs {
			"./build/bin",
			"./dep/bin",
		}

		includedirs {
			"./src",
			"%{prj.location}/src",
			"./src/ZoneTool",
			"./src/ZoneUtils",
			"./src/IW3",
			"./src/IW4",
			"./src/IW5",
			"./dep/include",
		}

		if _OPTIONS["set-version"] then
			defines {
				"ZONETOOL_VERSION=\"" .. _OPTIONS["set-version"] .. "\""
			}
		end

		filter "toolset:msc*"
			postbuildcommands {
				"if \"%COMPUTERNAME%\" == \"DESKTOP-CDFBECH\" ( copy /y \"$(TargetPath)\" \"E:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty 4\\zoneiw3.dll\" )",
				"if \"%COMPUTERNAME%\" == \"DESKTOP-CDFBECH\" ( copy /y \"$(TargetPath)\" \"F:\\iw4x_full_game\\zonetool.dll\" )",
				"if \"%COMPUTERNAME%\" == \"DESKTOP-CDFBECH\" ( copy /y \"$(TargetPath)\" \"E:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty Modern Warfare 3\\zonetool.dll\" )",
			}
		filter {}

	project "ZoneUtils"
		kind "StaticLib"
		language "C++"
		
		pchheader "stdafx.hpp"
		pchsource "src/ZoneUtils/stdafx.cpp"

		files {
			"./src/ZoneUtils/**.h",
			"./src/ZoneUtils/**.hpp",
			"./src/ZoneUtils/**.cpp",
		}

		syslibdirs {
			"./build/bin",
			"./dep/bin",
		}

		includedirs {
			"./src",
			"%{prj.location}/src",
			"./src/ZoneTool",
			"./src/ZoneUtils",
			"./dep/include",
		}

	project "IW3"
		kind "StaticLib"
		language "C++"
		dependson "IW4"
		
		pchheader "stdafx.hpp"
		pchsource "src/IW3/stdafx.cpp"

		files {
			"./src/IW3/**.h",
			"./src/IW3/**.hpp",
			"./src/IW3/**.cpp",
		}

		syslibdirs {
			"./build/bin",
			"./dep/bin",
		}

		includedirs {
			"./src",
			"%{prj.location}/src",
			"./src/ZoneTool",
			"./src/ZoneUtils",
			"./src/IW3",
			"./src/IW4",
			"./dep/include",
		}

	project "IW4"
		kind "StaticLib"
		language "C++"
		dependson "IW5"
		
		pchheader "stdafx.hpp"
		pchsource "src/IW4/stdafx.cpp"

		files {
			"./src/IW4/**.h",
			"./src/IW4/**.hpp",
			"./src/IW4/**.cpp",
		}

		syslibdirs {
			"./build/bin",
			"./dep/bin",
		}

		includedirs {
			"./src",
			"%{prj.location}/src",
			"./src/ZoneTool",
			"./src/ZoneUtils",
			"./src/IW3",
			"./src/IW5",
			"./dep/include",
		}

	project "IW5"
		kind "StaticLib"
		language "C++"

		pchheader "stdafx.hpp"
		pchsource "src/IW5/stdafx.cpp"

		files {
			"./src/IW5/**.h",
			"./src/IW5/**.hpp",
			"./src/IW5/**.cpp",
		}

		syslibdirs {
			"./build/bin",
			"./dep/bin",
		}

		includedirs {
			"./src",
			"%{prj.location}/src",
			"./src/ZoneTool",
			"./src/ZoneUtils",
			"./src/IW3",
			"./src/IW4",
			"./src/IW5",
			"./dep/include",
		}

	project "CODO"
		kind "StaticLib"
		language "C++"
		dependson "IW5"
		dependson "IW4"
		
		pchheader "stdafx.hpp"
		pchsource "src/CODO/stdafx.cpp"

		files {
			"./src/CODO/**.h",
			"./src/CODO/**.hpp",
			"./src/CODO/**.cpp",
		}

		syslibdirs {
			"./build/bin",
			"./dep/bin",
		}

		includedirs {
			"./src",
			"%{prj.location}/src",
			"./src/ZoneTool",
			"./src/ZoneUtils",
			"./src/CODO",
			"./src/IW3",
			"./src/IW4",
			"./src/IW5",
			"./dep/include",
		}