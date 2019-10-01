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
	const char* Linker::Version()
	{
		return "CODO";
	}

	bool Linker::InUse()
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

	void Linker::Startup()
	{
		if (this->InUse())
		{
			// todo
		}
	}

	std::shared_ptr<IZone> Linker::AllocZone(std::string& zone)
	{
		// allocate zone
		auto ptr = std::make_shared<Zone>(zone, this);
		return ptr;
	}

	std::shared_ptr<ZoneBuffer> Linker::AllocBuffer()
	{
		auto ptr = std::make_shared<ZoneBuffer>();
		ptr->init_streams(8);
		return ptr;
	}

	void Linker::LoadZone(std::string& name)
	{
		ZONETOOL_INFO("Loading zone \"%s\"...", name.data());

		XZoneInfo zone = {name.data(), 20, 0};
		// DB_LoadXAssets(&zone, 1, 0);

		ZONETOOL_INFO("Zone \"%s\" loaded.", name.data());
	}

	void Linker::UnloadZones()
	{
	}

	bool Linker::IsValidAssetType(std::string& type)
	{
		return this->TypeToInt(type) >= 0;
	}

	std::int32_t Linker::TypeToInt(std::string type)
	{
		auto xassettypes = reinterpret_cast<char**>(0x00799278);

		for (std::int32_t i = 0; i < max; i++)
		{
			if (xassettypes[i] == type)
				return i;
		}

		return -1;
	}

	std::string Linker::TypeToString(std::int32_t type)
	{
		auto xassettypes = reinterpret_cast<char**>(0x00799278);
		return xassettypes[type];
	}

	void Linker::DumpZone(std::string& name)
	{
		//isDumpingComplete = false;
		//isDumping = true;
		//currentDumpingZone = name;
		LoadZone(name);

		//while (!isDumpingComplete)
		//{
		//	Sleep(1);
		//}
	}

	void Linker::VerifyZone(std::string& name)
	{
		//isVerifying = true;
		//currentDumpingZone = name;
		LoadZone(name);
	}

	Linker::Linker()
	{
	}

	Linker::~Linker()
	{
	}
}
