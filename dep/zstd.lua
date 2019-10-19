zstd = {}

function zstd:include()
	local folder = DependencyFolder();
	
	includedirs {
		path.join(folder, "zstd/lib"),
		path.join(folder, "zstd/lib/common")
	}
end

function zstd:link()
	self:include()
	links {
		"zstd"
	}
end

function zstd:project()
	local folder = DependencyFolder();

	project "zstd"
	
		location "%{wks.location}/dep"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "zstd/lib/**.h"),
			path.join(folder, "zstd/lib/**.c")
		}
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
