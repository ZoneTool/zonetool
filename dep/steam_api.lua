SteamApi = {}

function SteamApi:include()
	includedirs {
		path.join(DependencyFolder(), "steam_api")
	}
end

function SteamApi:link()
	self:include()

	syslibdirs {
		path.join(DependencyFolder(), "bin")
	}

	links {
		"steam_api"
	}
end

function SteamApi:project()
end
