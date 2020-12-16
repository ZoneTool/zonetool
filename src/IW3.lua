IW3 = {}

function IW3:include()
    includedirs {
        path.join(ProjectFolder(), "IW3")
    }
end

function IW3:link()
    self:include()
	links {
		"IW3"
	}
end

function IW3:project()
    local folder = ProjectFolder();

    project "IW3"
        kind "StaticLib"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "IW3/stdafx.cpp"))

        files {
            path.join(folder, "IW3/**.h"),
            path.join(folder, "IW3/**.hpp"),
            path.join(folder, "IW3/**.cpp")
        }

        self:include()
        IW4:include()
        ZoneUtils:include()
end