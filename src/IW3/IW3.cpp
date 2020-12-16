// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
// #include "ZoneTool.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		bool isDumping = false;
		bool isVerifying = false;
		auto currentDumpingZone = ""s;

		Linker::Linker()
		{
		}

		Linker::~Linker()
		{
		}

		const char* Linker::version()
		{
			return "CoD4";
		}

		bool Linker::is_used()
		{
			return !strncmp(reinterpret_cast<char *>(0x006CF584), this->version(), 4);
		}

		typedef void*(__cdecl * Dvar_RegisterBool_t)(const char*, bool, unsigned int, const char*);
		Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x56C600;

		void* Linker::Dedicated_RegisterDvarBool(const char* name, bool defaultValue, unsigned int flags,
		                                         const char* description)
		{
			return Dvar_RegisterBool(name, true, 0x2000, description);
		}

		void** DB_XAssetPool = (void**)0x7265E0;
		unsigned int* g_poolSize = (unsigned int*)0x7263A0;

		void* DB_FindXAssetHeader_Unsafe(const XAssetType type, const std::string& name)
		{
			const static auto DB_FindXAssetHeader_Internal = 0x4892A0;
			const auto name_ptr = name.data();
			const auto type_int = static_cast<std::int32_t>(type);
			
			const XAsset* asset_header = nullptr;
			
			__asm
			{
				mov edi, name_ptr;
				push type_int;
				call DB_FindXAssetHeader_Internal;
				add esp, 4;
				mov asset_header, eax;
			}

			return (asset_header) ? asset_header->ptr.data : nullptr;
		}
		
		const char* Linker::GetAssetName(XAsset* asset)
		{
			// todo
			if (asset->type == image)
			{
				return asset->ptr.image->name;
			}
			if (asset->type == menu)
			{
				// return asset->ptr.menu->name;
			}
			else
			{
				return asset->ptr.rawfile->name;
			}

			return "";
		}

		void Linker::HandleAsset(XAsset* asset)
		{
			static std::shared_ptr<ZoneMemory> memory;
			static std::vector<std::pair<XAssetType, std::string>> referencedAssets;

			if (!memory)
			{
				memory = std::make_shared<ZoneMemory>(1024 * 1024 * 128);		// 128mb
			}
			
			// nice meme
			if (isVerifying)
			{
				// print asset name to console
				ZONETOOL_INFO("Loading asset \"%s\" of type %s.", Linker::GetAssetName(asset), reinterpret_cast<char**>(
0x00726840)[asset->type]);
			}

#define DECLARE_ASSET(__TYPE__, __ASSET__) \
	if (asset->type == __TYPE__) \
	{ \
		__ASSET__::dump(asset->ptr.__TYPE__, memory.get()); \
	}

			// fastfile name
			auto fastfile = static_cast<std::string>(*(const char**)0xE344CC);

			if (asset->type == rawfile && GetAssetName(asset) == currentDumpingZone)
			{
				for (auto& ref : referencedAssets)
				{
					if (ref.second.length() <= 1 || ref.first == XAssetType::loaded_sound)
					{
						continue;
					}

					const auto asset_name = &ref.second[1];
					const auto ref_asset = DB_FindXAssetHeader_Unsafe(ref.first, asset_name);

					if (ref_asset == nullptr)
					{
						ZONETOOL_ERROR("Could not find referenced asset \"%s\"!", asset_name);
						continue;
					}
					
					XAsset asset;
					asset.type = ref.first;
					asset.ptr.data = ref_asset;

					ZONETOOL_INFO("Dumping additional asset \"%s\" because it is referenced by %s.", asset_name, currentDumpingZone.data());
					
					HandleAsset(&asset);
				}

				ZONETOOL_INFO("Zone \"%s\" dumped.", &fastfile[0]);
				
				// clear referenced assets array because we are done dumping
				referencedAssets.clear();

				// free memory
				memory->Free();
				memory = nullptr;
				
				FileSystem::SetFastFile("");
				isDumping = false;
				isVerifying = false;
			}

			// dump shit
			if (isDumping)
			{
				FileSystem::SetFastFile(fastfile);

				// check if the asset is a reference asset
				if (GetAssetName(asset)[0] == ',')
				{
					referencedAssets.push_back({asset->type, GetAssetName(asset)});
				}
				else
				{
					// DECLARE_ASSET(image, IGfxImage);
					DECLARE_ASSET(xmodel, IXModel);
					DECLARE_ASSET(material, IMaterial);
					DECLARE_ASSET(xanim, IXAnimParts);
					DECLARE_ASSET(techset, ITechset);
					DECLARE_ASSET(loaded_sound, ILoadedSound);
					DECLARE_ASSET(sound, ISound);
					DECLARE_ASSET(fx, IFxEffectDef);
					DECLARE_ASSET(gfx_map, IGfxWorld);
					DECLARE_ASSET(col_map_mp, IClipMap);
					DECLARE_ASSET(map_ents, IMapEnts);
					DECLARE_ASSET(com_map, IComWorld);
				}
			}
		}

		void* Linker::DB_AddXAsset(XAsset* asset, int unk)
		{
			HandleAsset(asset);

			// call original function
			return Memory::func<void*(XAsset* asset, int unk)>(0x489B00)(asset, unk);
		}

		void* ReallocateAssetPool(uint32_t type, unsigned int newSize)
		{
			int elSize = DB_GetXAssetSizeHandlers[type]();

			void* poolEntry = malloc(newSize * elSize);
			DB_XAssetPool[type] = poolEntry;
			g_poolSize[type] = newSize;

			return poolEntry;
		}

		void* ReallocateAssetPoolM(uint32_t type, int multiplier)
		{
			int elSize = DB_GetXAssetSizeHandlers[type]();
			int newSize = multiplier * g_poolSize[type];

			void* poolEntry = malloc(newSize * elSize);
			DB_XAssetPool[type] = poolEntry;
			g_poolSize[type] = newSize;

			return poolEntry;
		}

		void Com_PrintfHook(int channel, const char* data, int unk)
		{
			printf(data);
		}
		
		void Linker::startup()
		{
			if (this->is_used())
			{
				// Realloc asset pools
				ReallocateAssetPoolM(localize, 2);
				ReallocateAssetPoolM(material, 2);
				ReallocateAssetPoolM(font, 2);
				ReallocateAssetPoolM(image, 2);
				ReallocateAssetPoolM(techset, 2);
				ReallocateAssetPoolM(fx, 4);
				ReallocateAssetPoolM(xanim, 2);
				ReallocateAssetPoolM(xmodel, 2);
				ReallocateAssetPoolM(physpreset, 2);
				ReallocateAssetPoolM(weapon, 2);
				ReallocateAssetPoolM(game_map_sp, 2);
				ReallocateAssetPoolM(game_map_mp, 2);
				ReallocateAssetPoolM(map_ents, 5);
				ReallocateAssetPoolM(com_map, 5);
				ReallocateAssetPoolM(col_map_mp, 5);
				ReallocateAssetPoolM(gfx_map, 5);
				ReallocateAssetPoolM(rawfile, 2);
				ReallocateAssetPoolM(loaded_sound, 2);
				ReallocateAssetPoolM(sound, 2);
				ReallocateAssetPoolM(stringtable, 2);

				// Asset dump hook
				Memory(0x00489E72).call(DB_AddXAsset);

				// Always use dedicated mode
				Memory(0x4FEA9E).call(Dedicated_RegisterDvarBool);
				Memory(0x4FEAC2).call(Dedicated_RegisterDvarBool);
				Memory(0x4FFE37).call(Dedicated_RegisterDvarBool);
				Memory(0x4FFE5D).call(Dedicated_RegisterDvarBool);

				// Don't touch image data
				Memory(0x616E9C).nop(3);
				
				// idc if you can't initialise PunkBuster
				Memory(0x5776DF).nop(5);
				Memory(0x5776EC).nop(5);

				// Initialise console_mp.log
				Memory(0x4FCBA3).nop(2);

				// We don't need recommended settings
				Memory(0x4FE99A).set<std::uint8_t>(0xEB);
				Memory(0x4FE993).nop(7);

				// We do not need to load the config_mp.cfg
				Memory(0x55EEA6).set<std::uint8_t>(0xEB);

				// Don't give a frametime warning
				Memory(0x4FFD9D).nop(5);

				// Disabling loadedsound touching
				Memory(0x4794C2).nop(5);

				// No huffmann message
				Memory(0x507982).nop(5);

				// Disable console window
				Memory(0x0046CE55).nop(5);

				// Obtain console output from IW3
				Memory(0x4FCC00).call(Com_PrintfHook);
			}
		}

		std::shared_ptr<IZone> Linker::alloc_zone(const std::string& zone)
		{
			ZONETOOL_ERROR("AllocZone called but IW3 is not intended to compile zones!");
			return nullptr;
		}

		std::shared_ptr<ZoneBuffer> Linker::alloc_buffer()
		{
			ZONETOOL_ERROR("AllocBuffer called but IW3 is not intended to compile zones!");
			return nullptr;
		}

		void Linker::load_zone(const std::string& name)
		{
			static XZoneInfo zone;
			zone.zone = _strdup(&name[0]);
			zone.loadFlags = 0;
			zone.unloadFlags = 0;

			Memory::func<void(XZoneInfo*, int, int)>(0x48A2B0)(&zone, 1, 0);
		}

		void Linker::unload_zones()
		{
		}

		bool Linker::is_valid_asset_type(const std::string& type)
		{
			return this->type_to_int(type) >= 0;
		}

		std::int32_t Linker::type_to_int(std::string type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x00726840);

			for (std::int32_t i = 0; i < max; i++)
			{
				if (xassettypes[i] == type)
					return i;
			}

			return -1;
		}

		std::string Linker::type_to_string(std::int32_t type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x00726840);
			return xassettypes[type];
		}

        bool Linker::supports_building()
        {
            return false;
        }

		bool Linker::supports_version(const zone_target_version version)
		{
			return version == zone_target_version::iw3_alpha_253 || version == zone_target_version::iw3_alpha_290 || 
				version == zone_target_version::iw3_alpha_328;
		}

        void Linker::dump_zone(const std::string& name)
		{
			isDumping = true;
			currentDumpingZone = name;
			load_zone(name);
		}

		void Linker::verify_zone(const std::string& name)
		{
			isVerifying = true;
			currentDumpingZone = name;
			load_zone(name);
		}
	}
}
