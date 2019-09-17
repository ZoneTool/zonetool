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
	class ILinker
	{
	public:
		virtual const char* Version() = 0;
		virtual bool InUse() = 0;
		virtual void Startup() = 0;
		virtual std::shared_ptr<ZoneBuffer> AllocBuffer() = 0;
		virtual std::shared_ptr<IZone> AllocZone(std::string& zone) = 0;

		virtual void DumpZone(std::string& name) = 0;
		virtual void VerifyZone(std::string& name) = 0;
		virtual void LoadZone(std::string& name) = 0;
		virtual void UnloadZones() = 0;

		virtual bool IsValidAssetType(std::string& type) = 0;
		virtual std::int32_t TypeToInt(std::string type) = 0;
		virtual std::string TypeToString(std::int32_t type) = 0;

	private:
	};
}
