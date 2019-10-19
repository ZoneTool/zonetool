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

#include "Assets/XModel.hpp"
#include "Assets/Material.hpp"
#include "Assets/XAnimParts.hpp"
#include "Assets/Techset.hpp"
#include "Assets/GfxWorld.hpp"
#include "Assets/GfxImage.hpp"
#include "Assets/GameWorldMp.hpp"
#include "Assets/LoadedSound.hpp"
#include "Assets/Sound.hpp"

// oh nee toch niet

namespace ZoneTool
{
	namespace IW3
	{
		struct XAsset
		{
			XAssetType type;
			XAssetHeader ptr;
		};

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

			void DumpZone(std::string& name) override;
			void VerifyZone(std::string& name) override;

			static void* Dedicated_RegisterDvarBool(const char* name, bool defaultValue, unsigned int flags,
			                                        const char* description);
			static void* DB_AddXAsset(XAsset* asset, int unk);
			static const char* GetAssetName(XAsset* asset);
			static void HandleAsset(XAsset* asset);
		};
	}
}
