// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

extern std::string currentzone;

#include "Assets/PhysCollmap.hpp"
#include "Assets/PhysPreset.hpp"
#include "Assets/PixelShader.hpp"
#include "Assets/VertexDecl.hpp"
#include "Assets/VertexShader.hpp"
#include "Assets/Techset.hpp"
#include "Assets/GfxImage.hpp"
#include "Assets/Material.hpp"
#include "Assets/XSurface.hpp"
#include "Assets/XModel.hpp"
#include "Assets/XAnimParts.hpp"
#include "Assets/MapEnts.hpp"
#include "Assets/ClipMap.hpp"
#include "Assets/GfxWorld.hpp"
#include "Assets/RawFile.hpp"
#include "Assets/SoundCurve.hpp"
#include "Assets/LoadedSound.hpp"
#include "Assets/Sound.hpp"
#include "Assets/TracerDef.hpp"
#include "Assets/LocalizeEntry.hpp"
#include "Assets/FontDef.hpp"
#include "Assets/LeaderBoardDef.hpp"
#include "Assets/ComWorld.hpp"
#include "Assets/FxEffectDef.hpp"
#include "Assets/AttachmentDef.hpp"
#include "Assets/WeaponDef.hpp"
#include "Assets/StringTable.hpp"
#include "Assets/FxWorld.hpp"
#include "Assets/GlassWorld.hpp"
#include "Assets/StructuredDataDef.hpp"
#include "Assets/MenuDef.hpp"
#include "Assets/ScriptFile.hpp"
#include "Assets/LightDef.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		class Linker;

		class Zone : public IZone
		{
		private:
			std::uintptr_t m_assetbase;
			std::string m_name;
			ILinker* m_linker;
			std::vector<std::shared_ptr<IAsset>> m_assets;
			std::shared_ptr<ZoneMemory> m_zonemem;

		public:
			Zone(std::string name, ILinker* linker);
			~Zone();

			void* GetAssetPointer(std::int32_t type, std::string name) override;

			void AddAssetOfTypePtr(std::int32_t type, void* pointer) override;

			void AddAssetOfType(std::int32_t type, const std::string& name) override;
			void AddAssetOfType(const std::string& type, const std::string& name) override;
			std::int32_t GetTypeByName(const std::string& type) override;

			void Build(std::shared_ptr<ZoneBuffer>& buf) override;
		};
	}
}
