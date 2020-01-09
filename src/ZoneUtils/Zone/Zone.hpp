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
		virtual IAsset* find_asset(std::int32_t type, const std::string& name)
		{
			return nullptr;
		}

		virtual void* get_asset_pointer(std::int32_t type, const std::string& name) = 0;

		virtual void add_asset_of_type_by_pointer(std::int32_t type, void* pointer) = 0;

		virtual void add_asset_of_type(const std::string& type, const std::string& name) = 0;
		virtual void add_asset_of_type(std::int32_t type, const std::string& name) = 0;
		virtual std::int32_t get_type_by_name(const std::string& type) = 0;

		virtual void build(ZoneBuffer* buf) = 0;

		virtual zone_target get_target()
		{
			return target_;
		}
		virtual zone_target_version get_target_version()
		{
			return target_version_;
		}
		virtual void set_target(const zone_target target)
		{
			target_ = target;
		}
		virtual void set_target_version(const zone_target_version version)
		{
			target_version_ = version;
		}

	protected:
		zone_target target_ = zone_target::pc;
		zone_target_version target_version_ = zone_target_version::iw4_release;
		
	};
}
