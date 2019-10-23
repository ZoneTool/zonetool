ZoneTool = {}

function ZoneTool:include()
    includedirs {
        path.join(ProjectFolder(), "ZoneTool")
    }
end

function ZoneTool:link()
    self:include()
	links {
		"ZoneTool"
	}
end

function ZoneTool:project()
    local folder = ProjectFolder();

    project "ZoneTool"
        kind "SharedLib"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "ZoneTool/stdafx.cpp"))

        files {
            path.join(folder, "ZoneTool/**.h"),
            path.join(folder, "ZoneTool/**.hpp"),
            path.join(folder, "ZoneTool/**.cpp")
        }

        -- Linked projects
        self:include()
        IW3:link()
        IW4:link()
        IW5:link()
        CODO:link()
        ZoneUtils:link()
        SteamApi:link()
        
        -- ThirdParty
        libtommath:link()
        libtomcrypt:link()
        SteamApi:link()
        zstd:link()
        zlib:link()

        if _OPTIONS["set-version"] then
            defines {
                "ZONETOOL_VERSION=\"" .. _OPTIONS["set-version"] .. "\""
            }
        end

        filter "toolset:msc*"
            postbuildcommands {
                "if \"%COMPUTERNAME%\" == \"DESKTOP-CDFBECH\" ( copy /y \"$(TargetPath)\" \"E:\\SteamLibrary\\steamapps\\common\\Call of Duty 4\\zoneiw3.dll\" )",
                "if \"%COMPUTERNAME%\" == \"DESKTOP-CDFBECH\" ( copy /y \"$(TargetPath)\" \"E:\\SteamLibrary\\steamapps\\common\\Call of Duty Modern Warfare 2\\zonetool.dll\" )",
                "if \"%COMPUTERNAME%\" == \"DESKTOP-CDFBECH\" ( copy /y \"$(TargetPath)\" \"E:\\SteamLibrary\\steamapps\\common\\Call of Duty Modern Warfare 3\\zonetool.dll\" )",
                "if \"%COMPUTERNAME%\" == \"DESKTOP-G5EPE91\" ( copy /y \"$(TargetPath)\" \"D:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty 4\\zoneiw3.dll\" )",
                "if \"%COMPUTERNAME%\" == \"DESKTOP-G5EPE91\" ( copy /y \"$(TargetPath)\" \"D:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty Modern Warfare 2\\zonetool.dll\" )",
                "if \"%COMPUTERNAME%\" == \"DESKTOP-G5EPE91\" ( copy /y \"$(TargetPath)\" \"D:\\Program Files (x86)\\Steam\\steamapps\\common\\Call of Duty Modern Warfare 3\\zonetool.dll\" )",
            }
        filter {}
end