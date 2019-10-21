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
			Linker();
			~Linker();

			const char* Version() override;
			bool InUse() override;
			void Startup() override;
			std::shared_ptr<IZone> AllocZone(std::string& zone) override;
			std::shared_ptr<ZoneBuffer> AllocBuffer() override;
			void LoadZone(std::string& name) override;
			void UnloadZones() override;
			bool IsValidAssetType(std::string& type) override;
			std::int32_t TypeToInt(std::string type) override;
			std::string TypeToString(std::int32_t type) override;
            bool SupportsBuilding() override;

			void DumpZone(std::string& name) override;
			void VerifyZone(std::string& name) override;

			static void Run();
			static void LoadDefaultZones();
			static const char* Linker::GetAssetName(XAssetType type, XAssetHeader header);
			static void DB_AddXAsset(XAssetType type, XAssetHeader header);
			static void DB_AddXAssetStub();
			static void IncreaseReadPointer();
			static void IncreaseReadPointer2();
			static void ReadHeader(void* ptr, int size);
			static void Load_XSurfaceArray(int shouldLoad, int count);
            static const char* GetZonePath(const char* zoneName);
		};
	}
}
