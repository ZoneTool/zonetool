ImgPak = {}

function ImgPak:include()
    includedirs {
        path.join(ProjectFolder(), "imgpak")
    }
end

function ImgPak:link()
    self:include()
	links {
		"ImgPak"
	}
end

function ImgPak:project()
    local folder = ProjectFolder();

    project "ImgPak"
        kind "ConsoleApp"
        language "C++"
        
        pchheader "stdafx.hpp"
        pchsource(path.join(folder, "imgpak/stdafx.cpp"))

        files {
            path.join(folder, "imgpak/**.h"),
            path.join(folder, "imgpak/**.hpp"),
            path.join(folder, "imgpak/**.cpp")
        }

        -- Linked projects
        self:include()
        ZoneUtils:link()
        
        -- ThirdParty
        libtommath:link()
        libtomcrypt:link()
        zstd:link()
        zlib:link()
end