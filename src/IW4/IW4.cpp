#include "stdafx.hpp"
#include <unordered_map>

namespace ZoneTool
{
	namespace IW4
	{
		bool isDumpingComplete = false;
		bool isDumping = false;
		bool isVerifying = false;
		auto currentDumpingZone = ""s;

		const char* Linker::Version()
		{
			return "IW4";
		}

		bool Linker::InUse()
		{
			return !strncmp(reinterpret_cast<char*>(0x71B85C), this->Version(), 3);
		}

		void Linker::LoadDefaultZones()
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

		void Linker::Run()
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
			LargeLocalInit();
			FS_Init();

			LoadDefaultZones();

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

			// haHAAAAAAAAAAAAAAAAAA
			return "";
		}

		void Linker::DB_AddXAsset(XAssetType type, XAssetHeader header)
		{
			static std::unordered_map<std::string, std::vector<std::pair<std::string, std::uint32_t>>> mappedShaders;

			// nice meme
			if (isVerifying)
			{
				// print asset name to console
				ZONETOOL_INFO("Loading asset \"%s\" of type %s.", Linker::GetAssetName(type, header), reinterpret_cast<
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
					fprintf(csvFile, "%s,%s\n", xassettypes[type], GetAssetName(type, header));
				}
			}

			// dump shit gaylord
			if (isDumping)
			{
				// dont dump empty assets
				if (GetAssetName(type, header)[0] == ',')
				{
					return;
				}

				// check if we're done loading the fastfile
				if (type == rawfile && GetAssetName(type, header) == fastfile)
				{
					// mark dumping as complete to exit the process if it has been started using the command line
					if (currentDumpingZone == fastfile)
					{
						isDumpingComplete = true;
					}

					auto fp = fopen("mappedShaders.txt", "wb");
					if (fp)
					{
						for (auto& shader : mappedShaders)
						{
							fprintf(fp, "shader \"%s\" uses the following amount of arguments per technique:\n",
							        shader.first.data());
							for (auto& techset : shader.second)
							{
								fprintf(fp, "\t%s - %i\n", techset.first.data(), techset.second);
							}
						}
						fclose(fp);
					}

					mappedShaders.clear();

					ZONETOOL_INFO("Zone \"%s\" dumped.", &fastfile[0]);
				}
				if (type == techset)
				{
					for (int i = 0; i < 48; i++)
					{
						if (header.techset->techniques[i])
						{
							for (int j = 0; j < header.techset->techniques[i]->hdr.numPasses; j++)
							{
								if (header.techset->techniques[i]->pass[j].vertexShader)
								{
									mappedShaders[header.techset->techniques[i]->pass[j].vertexShader->name].push_back(
										{
											header.techset->name,
											header.techset->techniques[i]->pass[j].stableArgCount + header
											                                                        .techset->techniques
											[i]->pass[j].perObjArgCount +
											header.techset->techniques[i]->pass[j].perPrimArgCount
										}
									);
								}
							}
						}
					}
				}

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
			return Memory::Func<void(void*, int)>(0x00445460)(ptr, size);
		}

		void Linker::Load_XSurfaceArray(int shouldLoad, int count)
		{
			// read the actual count from the varXModelSurfs ptr
			auto surface = *reinterpret_cast<ModelSurface**>(0x0112A95C);

			// call original read function with the correct count
			return Memory::Func<void(int, int)>(0x004925B0)(shouldLoad, surface->xSurficiesCount);
		}

		void ExitZoneTool()
		{
			std::exit(0);
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
				ReallocateAssetPoolM(fx_map, 5);
				ReallocateAssetPoolM(xmodelsurfs, 2);
				ReallocateAssetPoolM(vertexshader, 2);
				ReallocateAssetPoolM(vertexdecl, 16);
				ReallocateAssetPoolM(pixelshader, 2);
				ReallocateAssetPoolM(rawfile, 2);
				ReallocateAssetPoolM(lightdef, 2);

				// Kill Com_Error
				Memory(0x004B22D0).Jump(ExitZoneTool);

				// Tool init func
				Memory(0x6BABA1).Call(Run);
				Memory(0x4AA88B).Call(printf);

				// r_loadForRenderer
				Memory(0x519DDF).Set<BYTE>(0x0);

				// dirty disk breakpoint
				// Memory(0x4CF7F0).Set<BYTE>(0xCC);

				// delay loading of images, disable it
				Memory(0x51F450).Set<BYTE>(0xC3);

				// don't remove the 'texture data' pointer from GfxImage	
				Memory(0x51F4FA).Nop(6);

				// needed for the above to make Image_Release not misinterpret the texture data as a D3D texture
				Memory(0x51F03D).Set<BYTE>(0xEB);

				// don't zero out pixel shaders
				Memory(0x505AFB).Nop(7);

				// don't zero out vertex shaders
				Memory(0x505BDB).Nop(7);

				// don't memset vertex declarations (not needed?)
				Memory(0x00431B91).Nop(5);

				// allow loading of IWffu (unsigned) files
				Memory(0x4158D9).Set<BYTE>(0xEB); //main function
				Memory(0x4A1D97).Nop(2); //DB_AuthLoad_InflateInit

				// basic checks (hash jumps, both normal and playlist)
				Memory(0x5B97A3).Nop(2);
				Memory(0x5BA493).Nop(2);

				Memory(0x5B991C).Nop(2);
				Memory(0x5BA60C).Nop(2);

				Memory(0x5B97B4).Nop(2);
				Memory(0x5BA4A4).Nop(2);

				// some other, unknown, check
				Memory(0x5B9912).Set<BYTE>(0xB8);
				Memory(0x5B9913).Set<DWORD>(1);

				Memory(0x5BA602).Set<BYTE>(0xB8);
				Memory(0x5BA603).Set<DWORD>(1);

				// something related to image loading
				Memory(0x54ADB0).Set<BYTE>(0xC3);

				// dvar setting function, unknown stuff related to server thread sync
				Memory(0x647781).Set<BYTE>(0xEB);

				// fs_basegame
				Memory(0x6431D1).Set("zonetool");

				// hunk size (was 300 MiB)
				Memory(0x64A029).Set<DWORD>(0x3F000000);
				Memory(0x64A057).Set<DWORD>(0x3F000000); // 0x1C200000

				// allow loading of IWffu (unsigned) files
				Memory(0x4158D9).Set<BYTE>(0xEB); // main function
				Memory(0x4A1D97).Nop(2); // DB_AuthLoad_InflateInit

				// basic checks (hash jumps, both normal and playlist)
				Memory(0x5B97A3).Nop(2);
				Memory(0x5BA493).Nop(2);

				Memory(0x5B991C).Nop(2);
				Memory(0x5BA60C).Nop(2);

				Memory(0x5B97B4).Nop(2);
				Memory(0x5BA4A4).Nop(2);

				// Disabling loadedsound touching
				Memory(0x492EFC).Nop(5);

				// weaponfile patches
				Memory(0x408228).Nop(5); // find asset header
				Memory(0x408230).Nop(5); // is asset default
				Memory(0x40823A).Nop(2); // jump

				// menu stuff
				// disable the 2 new tokens in ItemParse_rect
				Memory(0x640693).Set<BYTE>(0xEB);

				// Dont load ASSET_TYPE_MENU anymore, we dont need it.
				Memory(0x453406).Nop(5);

				// DB_AddXAsset hook
				Memory(0x005BB650).Jump(DB_AddXAssetStub);

				// Fix fucking XSurface assets
				Memory(0x0048E8A5).Call(Load_XSurfaceArray);

				// Fastfile debugging
				Memory(0x0044546D).Jump(IncreaseReadPointer);
				Memory(0x00470E51).Jump(IncreaseReadPointer2);
				Memory(0x004159E2).Call(ReadHeader);
			}
		}

		std::shared_ptr<IZone> Linker::AllocZone(std::string& zone)
		{
			// Patch current thread
			Memory(0x1CDE7FC).Set(GetCurrentThreadId());

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
			DB_LoadXAssets(&zone, 1, 0);

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
			isDumpingComplete = false;
			isDumping = true;
			currentDumpingZone = name;
			LoadZone(name);

			while (!isDumpingComplete)
			{
				Sleep(1);
			}
		}

		void Linker::VerifyZone(std::string& name)
		{
			isVerifying = true;
			currentDumpingZone = name;
			LoadZone(name);
		}

		Linker::Linker()
		{
		}

		Linker::~Linker()
		{
		}
	}
}
