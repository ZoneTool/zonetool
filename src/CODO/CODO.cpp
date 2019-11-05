// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include <unordered_map>

namespace ZoneTool::CODO
{
	const char* Linker::version()
	{
		return "CODO";
	}

	bool Linker::is_used()
	{
		return false;
	}

	const char* Linker::GetAssetName(XAssetType type, XAssetHeader header)
	{
		 // todo
		 if (type == image)
		 {
		 	return header.gfximage->name;
		 }
		 if (type == menu)
		 {
		 	// 
		 }
		 else
		 {
		 	return header.rawfile->name;
		 }

		 return "";
	}

	void Linker::startup()
	{
		if (this->is_used())
		{
			// todo
		}
	}

	std::shared_ptr<IZone> Linker::alloc_zone(const std::string& zone)
	{
		// allocate zone
		auto ptr = std::make_shared<Zone>(zone, this);
		return ptr;
	}

	std::shared_ptr<ZoneBuffer> Linker::alloc_buffer()
	{
		auto ptr = std::make_shared<ZoneBuffer>();
		ptr->init_streams(8);
		return ptr;
	}

	void Linker::load_zone(const std::string& name)
	{
		ZONETOOL_INFO("Loading zone \"%s\"...", name.data());

		XZoneInfo zone = {name.data(), 20, 0};
		// DB_LoadXAssets(&zone, 1, 0);

		ZONETOOL_INFO("Zone \"%s\" loaded.", name.data());
	}

	void Linker::unload_zones()
	{
	}

	bool Linker::is_valid_asset_type(std::string& type)
	{
		return this->type_to_int(type) >= 0;
	}

	std::int32_t Linker::type_to_int(std::string type)
	{
		auto xassettypes = reinterpret_cast<char**>(0x00799278);

		for (std::int32_t i = 0; i < max; i++)
		{
			if (xassettypes[i] == type)
				return i;
		}

		return -1;
	}

	std::string Linker::type_to_string(std::int32_t type)
	{
		auto xassettypes = reinterpret_cast<char**>(0x00799278);
		return xassettypes[type];
	}

    bool Linker::supports_building()
    {
        return false;
    }

	bool Linker::supports_version(const zone_target_version version)
	{
		return false;
	}

    void Linker::dump_zone(const std::string& name)
	{
		//is_dumping_complete = false;
		//is_dumping = true;
		//currentDumpingZone = name;
		load_zone(name);

		//while (!is_dumping_complete)
		//{
		//	Sleep(1);
		//}
	}

	void Linker::verify_zone(const std::string& name)
	{
		//isVerifying = true;
		//currentDumpingZone = name;
		load_zone(name);
	}

	Linker::Linker()
	{
	}

	Linker::~Linker()
	{
	}
}
