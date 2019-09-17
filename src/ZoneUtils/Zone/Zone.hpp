// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	class IAsset;

	class IZone
	{
	public:
		virtual IAsset* FindAsset(std::int32_t type, std::string name)
		{
			return nullptr;
		}

		virtual void* GetAssetPointer(std::int32_t type, std::string name) = 0;

		virtual void AddAssetOfTypePtr(std::int32_t type, void* pointer) = 0;

		virtual void AddAssetOfType(const std::string& type, const std::string& name) = 0;
		virtual void AddAssetOfType(std::int32_t type, const std::string& name) = 0;
		virtual std::int32_t GetTypeByName(const std::string& type) = 0;

		virtual void Build(std::shared_ptr<ZoneBuffer>& buf) = 0;
	};
}
