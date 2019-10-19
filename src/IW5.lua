IW5 = {}

function IW5:include()
    includedirs {
        -- Due to files having the same name in the game projects
        -- the source folder cannot be included directly
        ProjectFolder()
    }
end

function IW5:link()
    self:include()
	links {
		"IW5"
	}
end

function IW5:project()
    local folder = ProjectFolder();

    project "IW5"
        kind "StaticLib"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "IW5/stdafx.cpp"))

        files {
            path.join(folder, "IW5/**.h"),
            path.join(folder, "IW5/**.hpp"),
            path.join(folder, "IW5/**.cpp")
        }

        self:include()
        ZoneUtils:include()
        
        zstd:include()
        zlib:include()
end