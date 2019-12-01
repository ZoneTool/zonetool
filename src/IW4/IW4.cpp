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
#include "ZoneTool/ZoneTool.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		bool isDumpingComplete = false;
		bool isDumping = false;
		bool isVerifying = false;
		auto currentDumpingZone = ""s;

		const char* Linker::version()
		{
			return "IW4";
		}

		bool Linker::is_used()
		{
			return !strncmp(reinterpret_cast<char*>(0x71B85C), this->version(), 3);
		}

		void Linker::load_default_zones()
		{
			static std::vector<std::string> defaultzones =
			{
				"code_pre_gfx_mp",
				"localized_code_pre_gfx_mp",
				"code_post_gfx_mp",
				"localized_code_post_gfx_mp",
				"common_mp",
			};

			static XZoneInfo zones[16];
			memset(zones, 0, sizeof XZoneInfo * 16);

			// Load our custom zones
			for (std::size_t i = 0; i < defaultzones.size(); i++)
			{
				zones[i].zone = defaultzones[i].c_str();
				zones[i].loadFlags = 0;
				zones[i].unloadFlags = 0;
			}

			return DB_LoadXAssets(zones, defaultzones.size(), 0);
		}

		void Linker::run()
		{
			// function definitions
			typedef void (__cdecl * Win_InitLocalization_t)(int);
			typedef void (__cdecl * SL_Init_t)();
			typedef void (__cdecl * Swap_Init_t)();
			typedef void (__cdecl * Com_InitHunkMemory_t)();
			typedef void (__cdecl * Sys_InitializeCriticalSections_t)();
			typedef void (__cdecl * DB_InitThread_t)();
			typedef void (__cdecl * Com_InitDvars_t)();
			typedef void (__cdecl * PMem_Init_t)();
			typedef void (__cdecl * R_RegisterDvars_t)();
			typedef void (__cdecl * FS_Init_t)(); // wrong name, but lazy
			typedef void (__cdecl * LargeLocalInit_t)(); // guessed name
			typedef void (__cdecl * Cmd_ExecuteSingleCommand_t)(int controller, int a2, const char* cmd);

			SL_Init_t SL_Init = (SL_Init_t)0x4D2280;
			Swap_Init_t Swap_Init = (Swap_Init_t)0x47F390;
			Com_InitHunkMemory_t Com_InitHunkMemory = (Com_InitHunkMemory_t)0x420830;
			Sys_InitializeCriticalSections_t Sys_InitializeCriticalSections = (Sys_InitializeCriticalSections_t)
				0x42F0A0;
			Sys_InitializeCriticalSections_t Sys_InitMainThread = (Sys_InitializeCriticalSections_t)0x4301B0;
			DB_InitThread_t DB_InitThread = (DB_InitThread_t)0x4E0FB0;
			Com_InitDvars_t Com_InitDvars = (Com_InitDvars_t)0x60AD10;
			Win_InitLocalization_t Win_InitLocalization = (Win_InitLocalization_t)0x406D10;
			PMem_Init_t PMem_Init = (PMem_Init_t)0x64A020;
			R_RegisterDvars_t R_RegisterDvars = (R_RegisterDvars_t)0x5196C0;
			FS_Init_t FS_Init = (FS_Init_t)0x429080;
			LargeLocalInit_t LargeLocalInit = (LargeLocalInit_t)0x4A62A0;
			Cmd_ExecuteSingleCommand_t Cmd_ExecuteSingleCommand = (Cmd_ExecuteSingleCommand_t)0x609540;
			DWORD G_SetupWeaponDef = 0x4E1F30;
			DWORD Scr_BeginLoadScripts = 0x4E1ED0;
			DWORD Scr_BeginLoadScripts2 = 0x4541F0;
			DWORD Scr_InitAllocNodes = 0x4B8740;

			Sys_InitializeCriticalSections();
			Sys_InitMainThread();
			Win_InitLocalization(0);
			SL_Init();
			Swap_Init();
			Com_InitHunkMemory();
			PMem_Init();
			DB_InitThread();
			Com_InitDvars();
			R_RegisterDvars();

			// gsc stuff
			__asm call Scr_BeginLoadScripts;
			__asm
			{
				push 0x201A45C;
				call Scr_BeginLoadScripts2;
				add esp, 4;
			}
			__asm call Scr_InitAllocNodes;
			
			LargeLocalInit();
			FS_Init();

			load_default_zones();

			// Cmd_RegisterCommands();
			// ZoneTool_LoadZones(nullptr, 0, 0);

			// menu stuff
			//DWORD Menu_Setup1 = 0x4454C0;
			//DWORD Menu_Setup2 = 0x501BC0;
			//__asm call Menu_Setup1;
			//__asm call Menu_Setup2;

			// load weapons
			__asm call G_SetupWeaponDef;

			while (true)
			{
			}
		}

		const char* Linker::get_asset_name(XAssetType type, XAssetHeader header)
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

		void* DB_FindXAssetHeader_Unsafe(const XAssetType type, const std::string& name)
		{
			const static auto DB_FindXAssetHeader_Internal = 0x5BB1B0;
			const auto name_ptr = name.data();
			const auto type_int = static_cast<std::int32_t>(type);

			const XAsset* asset_header = nullptr;

			__asm
			{
				mov edi, type_int;
				push name_ptr;
				call DB_FindXAssetHeader_Internal;
				add esp, 4;
				mov asset_header, eax;
			}

			return (asset_header) ? asset_header->ptr.data : nullptr;
		}
		
		void Linker::DB_AddXAsset(XAssetType type, XAssetHeader header)
		{
			static std::vector<std::pair<XAssetType, std::string>> referencedAssets;
			
			// nice meme
			if (isVerifying)
			{
				// print asset name to console
				ZONETOOL_INFO("Loading asset \"%s\" of type %s.", Linker::get_asset_name(type, header), reinterpret_cast<
char**>(0x00799278)[type]);
			}

#define DECLARE_ASSET(__TYPE__, __ASSET__) \
	if (type == __TYPE__) \
	{ \
		__ASSET__::dump(header.__TYPE__); \
	}

			// fastfile name
			auto fastfile = static_cast<std::string>((char*)(*(DWORD*)0x112A680 + 4));

			// generate CSV for fastfile
			if (isVerifying || isDumping)
			{
				FileSystem::SetFastFile(fastfile);

				static FILE* csvFile;

				// open csv file for dumping 
				if (!csvFile)
				{
					csvFile = FileSystem::FileOpen(fastfile + ".csv", "wb");
				}

				// dump assets to disk
				if (csvFile)
				{
					auto xassettypes = reinterpret_cast<char**>(0x00799278);
					fprintf(csvFile, "%s,%s\n", xassettypes[type], get_asset_name(type, header));
				}
			}

			if (isDumping)
			{
				// check if we're done loading the fastfile
				if (type == rawfile && get_asset_name(type, header) == fastfile)
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

						ZONETOOL_INFO("Dumping additional asset \"%s\" because it is referenced by %s.", asset_name, currentDumpingZone.data());

						XAssetHeader header;
						header.data = ref_asset;
						
						DB_AddXAsset(ref.first, header);
					}

					ZONETOOL_INFO("Zone \"%s\" dumped.", &fastfile[0]);

					// clear referenced assets array because we are done dumping
					referencedAssets.clear();

					// mark dumping as complete to exit the process if it has been started using the command line
					if (currentDumpingZone == fastfile)
					{
						isDumpingComplete = true;
					}
				}

				if (get_asset_name(type, header)[0] == ',')
				{
					referencedAssets.push_back({ type, get_asset_name(type, header) });
				}
				else
				{
					DECLARE_ASSET(phys_collmap, IPhysCollmap);
					DECLARE_ASSET(tracer, ITracerDef);
					DECLARE_ASSET(xmodelsurfs, IXSurface);
					DECLARE_ASSET(xmodel, IXModel);
					DECLARE_ASSET(material, IMaterial);
					DECLARE_ASSET(xanim, IXAnimParts);
					DECLARE_ASSET(techset, ITechset);
					DECLARE_ASSET(gfx_map, IGfxWorld);
					DECLARE_ASSET(col_map_mp, IClipMap);
					DECLARE_ASSET(map_ents, IMapEnts);
					DECLARE_ASSET(fx_map, IFxWorld);
					DECLARE_ASSET(com_map, IComWorld);
					DECLARE_ASSET(sound, ISound);
					DECLARE_ASSET(sndcurve, ISoundCurve);
					DECLARE_ASSET(loaded_sound, ILoadedSound);
					DECLARE_ASSET(rawfile, IRawFile);
					DECLARE_ASSET(stringtable, IStringTable);
					DECLARE_ASSET(stringtable, IStringTable);
					DECLARE_ASSET(vertexdecl, IVertexDecl);
					DECLARE_ASSET(pixelshader, IPixelShader);
					DECLARE_ASSET(vertexshader, IVertexShader);
					DECLARE_ASSET(techset, ITechset);
					DECLARE_ASSET(game_map_mp, IGameWorldMp);
					DECLARE_ASSET(image, IGfxImage);
					DECLARE_ASSET(fx, IFxEffectDef);
					DECLARE_ASSET(lightdef, ILightDef);
					DECLARE_ASSET(weapon, IWeaponDef);
					DECLARE_ASSET(addon_map_ents, IAddonMapEnts);
				}
			}
		}

		void __declspec(naked) Linker::DB_AddXAssetStub()
		{
			// return to original function
			__asm
			{
				// original code
				sub esp, 0x14;
				mov eax, [esp + 1Ch];
				mov ecx, [eax];
				push ebx;
				push ebp;
				mov ebp, [esp + 20h];

				// call our DB_AddXAsset function
				pushad;
				push ecx;
				push ebp;
				call DB_AddXAsset;
				add esp, 8;
				popad;

				// jump back
				push 0x005BB65F;
				retn;
			}
		}

		void** DB_XAssetPool = (void**)0x7998A8;
		unsigned int* g_poolSize = (unsigned int*)0x7995E8;

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

		int readPosition = 0;

		void logStreamPosition(int bytesRead)
		{
			if (isVerifying)
			{
				// ZONETOOL_INFO("Read %u bytes. (Currently at position 0x%08X in file)", bytesRead, readPosition - bytesRead);
			}
		}

		__declspec(naked) void Linker::IncreaseReadPointer()
		{
			__asm
			{
				// store read position
				add readPosition, ecx;
				mov DWORD PTR ds : 0x112a6c4, ecx;

				// log data
				pushad;
				push ecx;
				call logStreamPosition;
				add esp, 4;
				popad;

				// go back
				push 0x00445473;
				retn;
			}
		}

		__declspec(naked) void Linker::IncreaseReadPointer2()
		{
			__asm
			{
				// store read position
				add readPosition, esi;

				// original code
				mov eax, 0x006B4B80;
				call eax;

				// log data
				pushad;
				push esi;
				call logStreamPosition;
				add esp, 4;
				popad;

				// go back
				push 0x00470E56;
				retn;
			}
		}

		void Linker::ReadHeader(void* ptr, int size)
		{
			// reset readPosition
			readPosition = 0;

			// read header
			return Memory::func<void(void*, int)>(0x00445460)(ptr, size);
		}

		void Linker::Load_XSurfaceArray(int shouldLoad, int count)
		{
			// read the actual count from the varXModelSurfs ptr
			auto surface = *reinterpret_cast<XModelSurfs**>(0x0112A95C);

			// call original read function with the correct count
			return Memory::func<void(int, int)>(0x004925B0)(shouldLoad, surface->numsurfs);
		}

        const char* Linker::GetZonePath(const char* zoneName)
        {
            static std::string lastZonePath;
            static std::vector<std::string> zonePaths =
            {
                "zone\\dlc\\",
                "zone\\patch\\"
            };

            const std::string zoneFileName = zoneName;
            const char* languageName = Memory::func<const char* ()>(0x45CBA0)();

            // Priority 1: localized zone folder
            const std::string localizedZonePath = va("zone\\%s\\", languageName, zoneName);
            if(std::filesystem::exists(localizedZonePath + zoneFileName))
            {
                lastZonePath = localizedZonePath;
                return lastZonePath.c_str();
            }

            // Priority 2: custom zone paths
            for(auto customZonePath : zonePaths)
            {
                if (std::filesystem::exists(customZonePath + zoneFileName))
                {
                    lastZonePath = customZonePath;
                    return lastZonePath.c_str();
                }
            }

            // If no file could be found return the default location. The game will notice itself that there is no fastfile.
            lastZonePath = localizedZonePath;
            return lastZonePath.c_str();
        }

		void ExitZoneTool()
		{
			std::exit(0);
		}

		void gsc_compile_error(int unk, const char* fmt, ...)
		{
			char error_message[4096] = {};
			
			va_list va;
			va_start(va, fmt);
			_vsnprintf(error_message, sizeof error_message, fmt, va);

			ZONETOOL_ERROR("script compile error: %s", error_message);
		}

		void emit_opcode(int opcode, int a2, int a3)
		{
			ZONETOOL_INFO("compiling opcode %u", opcode);
		}
		
		void Linker::startup()
		{
			if (this->is_used())
			{
				// for compiling GSC scripts
				ZoneTool::register_command("compilescript", [](auto args)
				{
					//
					if (args.size() < 2)
					{
						ZONETOOL_INFO("Usage: compilescript <scriptfile>\n");
						return;
					}

					if (FileSystem::FileExists(args[1] + ".gsc"))
					{
						ZONETOOL_INFO("Compiling script \"%s\"...", args[1].data());
						
						auto fp = FileSystem::FileOpen(args[1] + ".gsc", "rb");
						if (fp)
						{
							const auto file_size = FileSystem::FileSize(fp);
							const auto bytes = FileSystem::ReadBytes(fp, file_size);
							auto bytes_ptr = bytes.data();

							// set bytes ptr
							Memory(0x1CFEEE8).set(bytes_ptr);

							// patch current thread
							Memory(0x1CDE7FC).set(GetCurrentThreadId());

							// load gsc
							Function<void(const char*, int, int)>(0x427D00)(args[1].data(), 0, 0);
							
							FileSystem::FileClose(fp);

							ZONETOOL_INFO("Successfully compiled script \"%s\"!", args[1].data());
						}
					}
					else
					{
						ZONETOOL_ERROR("Cannot find script \"%s\".", args[1].data());
					}
				});

				// dump emitted opcodes
				Memory(0x613FD0).jump(emit_opcode);

				// force compiling gsc
				Memory(0x427DB4).set<std::uint8_t>(0xEB);
				Memory(0x427D22).set<std::uint8_t>(0xEB);
				
				// 
				Memory(0x427DED).nop(6);
				
				// do nothing with online sessions
				Memory(0x441650).set<std::uint8_t>(0xC3);

				// temp fix for GSC compiling
				Memory(0x434260).jump(gsc_compile_error);
				
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
				ReallocateAssetPoolM(fx_map, 5);
				ReallocateAssetPoolM(xmodelsurfs, 2);
				ReallocateAssetPoolM(vertexshader, 2);
				ReallocateAssetPoolM(vertexdecl, 16);
				ReallocateAssetPoolM(pixelshader, 2);
				ReallocateAssetPoolM(rawfile, 2);
				ReallocateAssetPoolM(lightdef, 2);

				// Kill "missing asset" errors from the game to prevent confusion
				Memory(0x5BB380).set<std::uint8_t>(0xC3);
				
				// Kill Com_Error
				Memory(0x004B22D0).jump(ExitZoneTool);

				// Tool init func
				Memory(0x6BABA1).call(run);
				Memory(0x4AA88B).call(printf);

				// r_loadForRenderer
				Memory(0x519DDF).set<BYTE>(0x0);

				// dirty disk breakpoint
				// Memory(0x4CF7F0).Set<BYTE>(0xCC);

				// delay loading of images, disable it
				Memory(0x51F450).set<BYTE>(0xC3);

				// don't remove the 'texture data' pointer from GfxImage	
				Memory(0x51F4FA).nop(6);

				// needed for the above to make Image_Release not misinterpret the texture data as a D3D texture
				Memory(0x51F03D).set<BYTE>(0xEB);

				// don't zero out pixel shaders
				Memory(0x505AFB).nop(7);

				// don't zero out vertex shaders
				Memory(0x505BDB).nop(7);

				// don't memset vertex declarations (not needed?)
				Memory(0x00431B91).nop(5);

				// allow loading of IWffu (unsigned) files
				Memory(0x4158D9).set<BYTE>(0xEB); //main function
				Memory(0x4A1D97).nop(2); //DB_AuthLoad_InflateInit

				// basic checks (hash jumps, both normal and playlist)
				Memory(0x5B97A3).nop(2);
				Memory(0x5BA493).nop(2);

				Memory(0x5B991C).nop(2);
				Memory(0x5BA60C).nop(2);

				Memory(0x5B97B4).nop(2);
				Memory(0x5BA4A4).nop(2);

				// some other, unknown, check
				Memory(0x5B9912).set<BYTE>(0xB8);
				Memory(0x5B9913).set<DWORD>(1);

				Memory(0x5BA602).set<BYTE>(0xB8);
				Memory(0x5BA603).set<DWORD>(1);

				// something related to image loading
				Memory(0x54ADB0).set<BYTE>(0xC3);

				// dvar setting function, unknown stuff related to server thread sync
				Memory(0x647781).set<BYTE>(0xEB);

				// fs_basegame
				Memory(0x6431D1).set("zonetool");

				// hunk size (was 300 MiB)
				Memory(0x64A029).set<DWORD>(0x3F000000);
				Memory(0x64A057).set<DWORD>(0x3F000000); // 0x1C200000

				// allow loading of IWffu (unsigned) files
				Memory(0x4158D9).set<BYTE>(0xEB); // main function
				Memory(0x4A1D97).nop(2); // DB_AuthLoad_InflateInit

				// basic checks (hash jumps, both normal and playlist)
				Memory(0x5B97A3).nop(2);
				Memory(0x5BA493).nop(2);

				Memory(0x5B991C).nop(2);
				Memory(0x5BA60C).nop(2);

				Memory(0x5B97B4).nop(2);
				Memory(0x5BA4A4).nop(2);

				// Disabling loadedsound touching
				Memory(0x492EFC).nop(5);

				// weaponfile patches
				Memory(0x408228).nop(5); // find asset header
				Memory(0x408230).nop(5); // is asset default
				Memory(0x40823A).nop(2); // jump

				// menu stuff
				// disable the 2 new tokens in ItemParse_rect
				Memory(0x640693).set<BYTE>(0xEB);

				// Dont load ASSET_TYPE_MENU anymore, we dont need it.
				Memory(0x453406).nop(5);

				// DB_AddXAsset hook
				Memory(0x005BB650).jump(DB_AddXAssetStub);

				// Fix fucking XSurface assets
				Memory(0x0048E8A5).call(Load_XSurfaceArray);

				// Fastfile debugging
				Memory(0x0044546D).jump(IncreaseReadPointer);
				Memory(0x00470E51).jump(IncreaseReadPointer2);
				Memory(0x004159E2).call(ReadHeader);

                // Load fastfiles from custom zone folders
                Memory(0x44DA90).jump(GetZonePath);
			}
		}
		
		std::shared_ptr<IZone> Linker::alloc_zone(const std::string& zone)
		{
			// Patch current thread
			Memory(0x1CDE7FC).set(GetCurrentThreadId());

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
			DB_LoadXAssets(&zone, 1, 0);

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
            return true;
        }

		bool Linker::supports_version(const zone_target_version version)
		{
			return version == zone_target_version::iw4_release || version == zone_target_version::iw4_release_console || 
				version == zone_target_version::iw4_alpha_482 || version == zone_target_version::iw4_alpha_491;
		}

        void Linker::dump_zone(const std::string& name)
		{
			isDumpingComplete = false;
			isDumping = true;
			currentDumpingZone = name;
			load_zone(name);

			while (!isDumpingComplete)
			{
				Sleep(1);
			}
		}

		void Linker::verify_zone(const std::string& name)
		{
			isVerifying = true;
			currentDumpingZone = name;
			load_zone(name);
		}
	}
}
