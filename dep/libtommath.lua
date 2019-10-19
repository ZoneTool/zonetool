libtommath = {}

function libtommath:include()
	includedirs {
		path.join(DependencyFolder(), "libtommath")
	}
end

function libtommath:link()
	self:include()
	links {
		"libtommath"
	}
end

function libtommath:project()
	local folder = DependencyFolder();

	project "libtommath"
	
		location "%{wks.location}/dep"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "libtommath/*.h"), 
			path.join(folder, "libtommath/*.c") 
		}
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
