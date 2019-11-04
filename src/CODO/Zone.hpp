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

// #include "Assets/RawFile.hpp"
// #include "Assets/PhysPreset.hpp"
// #include "Assets/WeaponDef.hpp"
// #include "Assets/VertexDecl.hpp"
// #include "Assets/PixelShader.hpp"
// #include "Assets/VertexShader.hpp"
// #include "Assets/Techset.hpp"
// #include "Assets/GfxImage.hpp"
// #include "Assets/Material.hpp"
// #include "Assets/XSurface.hpp"
// #include "Assets/Sound.hpp"
// #include "Assets/LoadedSound.hpp"
// #include "Assets/SoundCurve.hpp"
// #include "Assets/FontDef.hpp"
// #include "Assets/PhysCollmap.hpp"
// #include "Assets/Xmodel.hpp"
// #include "Assets/GameWorldMp.hpp"
// #include "Assets/GameWorldSp.hpp"
// #include "Assets/FxWorld.hpp"
// #include "Assets/MapEnts.hpp"
// #include "Assets/ComWorld.hpp"
// #include "Assets/XAnimParts.hpp"
#include "Assets/StringTable.hpp"
#include "Assets/LocalizeEntry.hpp"
// #include "Assets/TracerDef.hpp"
// #include "Assets/GfxWorld.hpp"
// #include "Assets/ClipMap.hpp"
// #include "Assets/FxEffectDef.hpp"
// #include "Assets/LightDef.hpp"

namespace ZoneTool::CODO
{
	class Zone : public IZone
	{
	private:
		std::uintptr_t m_assetbase;
		std::string name_;
		ILinker* m_linker;
		std::vector<std::shared_ptr<IAsset>> m_assets;
		std::shared_ptr<ZoneMemory> m_zonemem;

	public:
		Zone(std::string name, ILinker* linker);
		~Zone();

		IAsset* find_asset(std::int32_t type, const std::string& name) override;
		void* Zone::get_asset_pointer(std::int32_t type, const std::string& name) override;

		void add_asset_of_type_by_pointer(std::int32_t type, void* pointer) override;

		void add_asset_of_type(std::int32_t type, const std::string& name) override;
		void add_asset_of_type(const std::string& type, const std::string& name) override;
		std::int32_t get_type_by_name(const std::string& type) override;

		void build(ZoneBuffer* buf) override;
	};
}
