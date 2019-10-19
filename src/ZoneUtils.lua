ZoneUtils = {}

function ZoneUtils:include()
    includedirs {
        path.join(ProjectFolder(), "ZoneUtils")
    }
end

function ZoneUtils:link()
    self:include()
	links {
		"ZoneUtils"
	}
end

function ZoneUtils:project()
    local folder = ProjectFolder();

    project "ZoneUtils"
        kind "StaticLib"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "ZoneUtils/stdafx.cpp"))

        files {
            path.join(folder, "ZoneUtils/**.h"),
            path.join(folder, "ZoneUtils/**.hpp"),
            path.join(folder, "ZoneUtils/**.cpp")
        }
        
        self:include()

        libtommath:include()
        libtomcrypt:include()
        zstd:include()
        zlib:include()
end