// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

#include <ZoneUtils.hpp>
#include "Functions.hpp"
#include "Structs.hpp"
#include "../IW5/Structs.hpp"
#include "Zone.hpp"

namespace ZoneTool::CODO
{
	using GfxImageFileHeader = IW5::GfxImageFileHeader;

	class Linker : public ILinker
	{
	public:
		Linker();
		~Linker();

		const char* version() override;
		bool is_used() override;
		void startup() override;
		std::shared_ptr<IZone> alloc_zone(const std::string& zone) override;
		std::shared_ptr<ZoneBuffer> alloc_buffer() override;
		void load_zone(const std::string& name) override;
		void unload_zones() override;
		bool is_valid_asset_type(std::string& type) override;
		std::int32_t type_to_int(std::string type) override;
		std::string type_to_string(std::int32_t type) override;
        bool supports_building() override;
		bool supports_version(const zone_target_version version) override;

		void dump_zone(const std::string& name) override;
		void verify_zone(const std::string& name) override;

		static const char* GetAssetName(XAssetType type, XAssetHeader header);
	};
}
