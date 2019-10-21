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

		const char* Linker::Version()
		{
			return "CoD4";
		}

		bool Linker::InUse()
		{
			return !strncmp(reinterpret_cast<char *>(0x006CF584), this->Version(), 4);
		}

		typedef void*(__cdecl * Dvar_RegisterBool_t)(const char*, bool, unsigned int, const char*);
		Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x56C600;

		void* Linker::Dedicated_RegisterDvarBool(const char* name, bool defaultValue, unsigned int flags,
		                                         const char* description)
		{
			return Dvar_RegisterBool(name, true, 0x2000, description);
		}

		const char* Linker::GetAssetName(XAsset* asset)
		{
			// todo
			if (asset->type == image)
			{
				return asset->ptr.gfximage->name;
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
			static std::vector<std::pair<XAssetType, std::string>> referencedAssets;

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
		__ASSET__::dump(asset->ptr.__TYPE__); \
	}

			// fastfile name
			auto fastfile = static_cast<std::string>(*(const char**)0xE344CC);

			if (asset->type == rawfile && GetAssetName(asset) == currentDumpingZone)
			{
                ZONETOOL_INFO("Zone \"%s\" dumped.", &fastfile[0]);

				// clear referenced assets array because we are done dumping
				referencedAssets.clear();

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
					DECLARE_ASSET(xmodel, IXModel);
					DECLARE_ASSET(material, IMaterial);
					DECLARE_ASSET(xanim, IXAnimParts);
					DECLARE_ASSET(techset, ITechset);
					DECLARE_ASSET(loaded_sound, ILoadedSound);
					DECLARE_ASSET(sound, ISound);
				}
			}
		}

		void* Linker::DB_AddXAsset(XAsset* asset, int unk)
		{
			HandleAsset(asset);

			// call original function
			return Memory::Func<void*(XAsset* asset, int unk)>(0x489B00)(asset, unk);
		}

		void** DB_XAssetPool = (void**)0x7265E0;
		unsigned int* g_poolSize = (unsigned int*)0x7263A0;

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

		void Linker::Startup()
		{
			if (this->InUse())
			{
				// Realloc asset pools
				ReallocateAssetPoolM(localize, 2);
				ReallocateAssetPoolM(material, 2);
				ReallocateAssetPoolM(font, 2);
				ReallocateAssetPoolM(image, 2);
				ReallocateAssetPoolM(techset, 2);
				ReallocateAssetPoolM(fx, 2);
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

				// Asset dump hook
				Memory(0x00489E72).Call(DB_AddXAsset);

				// Always use dedicated mode
				Memory(0x4FEA9E).Call(Dedicated_RegisterDvarBool);
				Memory(0x4FEAC2).Call(Dedicated_RegisterDvarBool);
				Memory(0x4FFE37).Call(Dedicated_RegisterDvarBool);
				Memory(0x4FFE5D).Call(Dedicated_RegisterDvarBool);

				// idc if you can't initialise PunkBuster
				Memory(0x5776DF).Nop(5);
				Memory(0x5776EC).Nop(5);

				// Initialise console_mp.log
				Memory(0x4FCBA3).Nop(2);

				// We don't need recommended settings
				Memory(0x4FE99A).Set<std::uint8_t>(0xEB);
				Memory(0x4FE993).Nop(7);

				// We do not need to load the config_mp.cfg
				Memory(0x55EEA6).Set<std::uint8_t>(0xEB);

				// Don't give a frametime warning
				Memory(0x4FFD9D).Nop(5);

				// Disabling loadedsound touching
				Memory(0x4794C2).Nop(5);

				// No huffmann message
				Memory(0x507982).Nop(5);

				// Disable console window
				Memory(0x0046CE55).Nop(5);
			}
		}

		std::shared_ptr<IZone> Linker::AllocZone(std::string& zone)
		{
			ZONETOOL_ERROR("AllocZone called but IW3 is not intended to compile zones!");
			return nullptr;
		}

		std::shared_ptr<ZoneBuffer> Linker::AllocBuffer()
		{
			ZONETOOL_ERROR("AllocBuffer called but IW3 is not intended to compile zones!");
			return nullptr;
		}

		void Linker::LoadZone(std::string& name)
		{
			static XZoneInfo zone;
			zone.zone = _strdup(&name[0]);
			zone.loadFlags = 0;
			zone.unloadFlags = 0;

			Memory::Func<void(XZoneInfo*, int, int)>(0x48A2B0)(&zone, 1, 0);
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
			auto xassettypes = reinterpret_cast<char**>(0x00726840);

			for (std::int32_t i = 0; i < max; i++)
			{
				if (xassettypes[i] == type)
					return i;
			}

			return -1;
		}

		std::string Linker::TypeToString(std::int32_t type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x00726840);
			return xassettypes[type];
		}

        bool Linker::SupportsBuilding()
        {
            return false;
        }

        void Linker::DumpZone(std::string& name)
		{
			isDumping = true;
			currentDumpingZone = name;
			LoadZone(name);
		}

		void Linker::VerifyZone(std::string& name)
		{
			isVerifying = true;
			currentDumpingZone = name;
			LoadZone(name);
		}
	}
}
