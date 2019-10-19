CODO = {}

function CODO:include()
    includedirs {
        path.join(ProjectFolder(), "CODO")
    }
end

function CODO:link()
    self:include()
	links {
		"CODO"
	}
end

function CODO:project()
    local folder = ProjectFolder();
    
    project "CODO"
        kind "StaticLib"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "CODO/stdafx.cpp"))

        files {
            path.join(folder, "CODO/**.h"),
            path.join(folder, "CODO/**.hpp"),
            path.join(folder, "CODO/**.cpp")
        }

        self:include()
        ZoneUtils:include()
end