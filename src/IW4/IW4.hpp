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

namespace ZoneTool
{
	namespace IW4
	{
		using GfxImageFileHeader = IW5::GfxImageFileHeader;

		class Linker : public ILinker
		{
		public:
			const char* version() override;
			bool is_used() override;
			void startup() override;
			std::shared_ptr<IZone> alloc_zone(const std::string& zone) override;
			std::shared_ptr<ZoneBuffer> alloc_buffer() override;
			void load_zone(const std::string& name) override;
			void unload_zones() override;
			bool is_valid_asset_type(const std::string& type) override;
			std::int32_t type_to_int(std::string type) override;
			std::string type_to_string(std::int32_t type) override;
            bool supports_building() override;
			bool supports_version(const zone_target_version version) override;
			
			void dump_zone(const std::string& name) override;
			void verify_zone(const std::string& name) override;
			
			static void run();
			static void load_default_zones();
			static const char* get_asset_name(XAssetType type, XAssetHeader header);
			
			static void DB_AddXAsset(XAssetType type, XAssetHeader header);
			static void DB_AddXAssetStub();
			static void IncreaseReadPointer();
			static void IncreaseReadPointer2();
			static void ReadHeader(void* ptr, int size);
			static void Load_XSurfaceArray(int shouldLoad, int count);
            static const char* GetZonePath(const char* zoneName);

		private:
			
		};
	}
}
