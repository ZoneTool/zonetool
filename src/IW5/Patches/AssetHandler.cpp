// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		extern bool is_dumping_complete;
		extern bool is_dumping;

		XAssetEntry AssetHandler::XAssetEntries[AssetEntries];
		std::uint32_t AssetHandler::db_hashmap[AssetEntries];

		bool AssetHandler::verify = false;
		bool AssetHandler::dump = false;

		void** AssetHandler::DB_XAssetPool = (void**)0x7C5E38;
		unsigned int* AssetHandler::g_poolSize = (unsigned int*)0x7C5B58;
		AssetHandler::DB_GetXAssetSizeHandler_t* AssetHandler::DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)
			0x7C6430;

		std::unordered_map<std::string, XAsset> AssetHandler::StoredAssets;

		// std::shared_ptr < ZoneMemory > mem;

		XAssetHeader AssetHandler::FindXAsset(std::int32_t type, const char* name, std::uint32_t unk)
		{
			if (StoredAssets.find(name) != StoredAssets.end())
			{
				if (StoredAssets[name].type == type)
				{
					return StoredAssets[name].header;
				}
			}

			return XAssetHeader(); // DB_FindXAssetHeader(type, name, flags);
		}

		void AssetHandler::AddXAsset(std::int32_t type, std::string name, void* pointer)
		{
			if (StoredAssets.find(name) == StoredAssets.end())
			{
				StoredAssets[name] = {static_cast<XAssetType>(type), reinterpret_cast<RawFile*>(pointer)};
			}
		}

		void AssetHandler::SetVerify(bool state)
		{
			verify = state;
		}

		void AssetHandler::SetDump(bool state)
		{
			dump = state;
		}

		const char* GetAssetName(std::int32_t type, void* ptr)
		{
			if (type == image)
			{
				return reinterpret_cast<GfxImage*>(ptr)->name;
			}

			if (type == localize)
			{
				return reinterpret_cast<LocalizeEntry*>(ptr)->name;
			}

			if (type == menu)
			{
				return reinterpret_cast<menuDef_t*>(ptr)->window.name;
			}

			return reinterpret_cast<RawFile*>(ptr)->name;
		}

		static std::vector<std::pair<std::int32_t, std::string>> CommonAssets;

		void* DB_FindXAssetHeader_Unsafe(const XAssetType type, const std::string& name)
		{
			const static auto DB_FindXAssetHeader_Internal = 0x44E540;
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

			return (asset_header) ? asset_header->header.data : nullptr;
		}
		
		void AssetHandler::DB_LogLoadedAsset(void* ptr, std::int32_t type)
		{
			static std::vector<std::pair<XAssetType, std::string>> referencedAssets;

#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::DB_LogLoadedAsset");
#endif

			auto fastfile = static_cast<std::string>(reinterpret_cast<const char*>(*reinterpret_cast<DWORD*>(0x1294A00)
				+ 4));

			// store all common assets
			if (fastfile == "common_mp")
			{
				CommonAssets.push_back({type, GetAssetName(type, ptr)});
			}

			if (dump)
			{
				FileSystem::SetFastFile(fastfile);

				static FILE* csvFile = nullptr;

				// open csv file for dumping 
				if (!csvFile)
				{
					csvFile = FileSystem::FileOpen(fastfile + ".csv", "wb");
				}

				// dump assets to disk
				if (csvFile)
				{
					auto xassettypes = reinterpret_cast<char**>(0x7C6208);
					fprintf(csvFile, "%s,%s\n", xassettypes[type], GetAssetName(type, ptr));
				}

				// check if we're done loading the fastfile
				if (type == rawfile && GetAssetName(type, ptr) == fastfile)
				{
					for (auto& asset : referencedAssets)
					{
						if (asset.second.length() <= 1 || asset.first == XAssetType::loaded_sound)
						{
							continue;
						}

						const auto asset_name = &asset.second[1];
						const auto ref_asset = DB_FindXAssetHeader_Unsafe(asset.first, asset_name);

						if (ref_asset == nullptr)
						{
							ZONETOOL_ERROR("Could not find referenced asset \"%s\"!", asset_name);
							continue;
						}

						ZONETOOL_INFO("Dumping additional asset \"%s\" because it is referenced by %s.", asset_name, fastfile.data());

						DB_LogLoadedAsset(ref_asset, asset.first);
					}

					ZONETOOL_INFO("Zone \"%s\" dumped.", &fastfile[0]);

					referencedAssets.clear();
					FileSystem::FileClose(csvFile);
					csvFile = nullptr;

					is_dumping_complete = true;
				}

				if (GetAssetName(type, ptr)[0] == ',')
				{
					referencedAssets.push_back({ XAssetType(type), GetAssetName(type, ptr) });
				}
				else
				{
#define DUMPCASE(__type__,__interface__,__struct__) \
				if (type == __type__) \
				{ \
					auto asset = reinterpret_cast<__struct__*>(ptr); \
					__interface__::dump(asset); \
				}

					DUMPCASE(attachment, IAttachmentDef, AttachmentDef);
					DUMPCASE(leaderboarddef, ILeaderBoardDef, LeaderBoardDef);
					DUMPCASE(material, IMaterial, Material);

					DUMPCASE(com_map, IComWorld, ComWorld);
					DUMPCASE(gfx_map, IGfxWorld, GfxWorld);
					DUMPCASE(fx_map, IFxWorld, FxWorld);
					DUMPCASE(glass_map, IGlassWorld, GlassWorld);
					DUMPCASE(col_map_mp, IClipMap, clipMap_t);
					DUMPCASE(map_ents, IMapEnts, MapEnts);
					DUMPCASE(lightdef, ILightDef, GfxLightDef);

					DUMPCASE(xanim, IXAnimParts, XAnimParts);
					DUMPCASE(xmodel, IXModel, XModel);
					DUMPCASE(xmodelsurfs, IXSurface, ModelSurface);
					DUMPCASE(fx, IFxEffectDef, FxEffectDef);
					DUMPCASE(sound, ISound, snd_alias_list_t);
					DUMPCASE(stringtable, IStringTable, StringTable);
					DUMPCASE(rawfile, IRawFile, RawFile);
					DUMPCASE(scriptfile, IScriptFile, ScriptFile);
					DUMPCASE(weapon, IWeaponDef, WeaponCompleteDef);
					DUMPCASE(image, IGfxImage, GfxImage);
					DUMPCASE(phys_collmap, IPhysCollmap, PhysCollmap);
					DUMPCASE(loaded_sound, ILoadedSound, LoadedSound);
					DUMPCASE(structureddatadef, IStructuredDataDef, StructuredDataDefSet);

					DUMPCASE(techset, ITechset, MaterialTechniqueSet);
					DUMPCASE(pixelshader, IPixelShader, PixelShader);
					DUMPCASE(vertexshader, IVertexShader, VertexShader);
					DUMPCASE(vertexdecl, IVertexDecl, VertexDecl);

					DUMPCASE(font, IFontDef, Font_s);
				}
			}
			else
			{
				/*std::string fastfile = reinterpret_cast<const char*>(*reinterpret_cast<DWORD*>(0x1294A00) + 4);
				if (fastfile.find("mp_") != std::string::npos || fastfile == "common_mp"s)
				{
					FileSystem::SetFastFile("");

					// dump everything techset related!
					DUMPCASE(material, IMaterial, Material);
					DUMPCASE(techset, ITechset, MaterialTechniqueSet);
					DUMPCASE(pixelshader, IPixelShader, PixelShader);
					DUMPCASE(vertexshader, IVertexShader, VertexShader);
					DUMPCASE(vertexdecl, IVertexDecl, VertexDecl);
				}*/
			}

			if (verify || FFCompression::ff_version > 1)
			{
				ZONETOOL_INFO("Loading asset \"%s\" of type %s.", GetAssetName(type, ptr), reinterpret_cast<char**>(
0x7C6208)[type]);
			}

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		bool AssetHandler::IsCommonAsset(std::int32_t type, const std::string& name)
		{
			for (auto& commonAsset : CommonAssets)
			{
				if (commonAsset.first == type && commonAsset.second == name)
				{
					return true;
				}
			}

			return false;
		}

		__declspec(naked) void AssetHandler::DB_AddXAsset()
		{
			__asm
			{
				pushad;

				push ebp;
				push ecx;
				call AssetHandler::DB_LogLoadedAsset;
				add esp, 8;

				popad;

				jmp_back:
				mov eax, DWORD PTR[ebp * 4 + 0x7c62c0];

				push 0x44EFF6;
				retn;
			}
		}

		void* AssetHandler::ReallocateAssetPool(uint32_t type, unsigned int newSize)
		{
			int elSize = DB_GetXAssetSizeHandlers[type]();

			void* poolEntry = malloc(newSize * elSize);
			DB_XAssetPool[type] = poolEntry;
			g_poolSize[type] = newSize;

			return poolEntry;
		}

		void* AssetHandler::ReallocateAssetPoolM(uint32_t type, int multiplier)
		{
			int elSize = DB_GetXAssetSizeHandlers[type]();
			int newSize = multiplier * g_poolSize[type];

			void* poolEntry = malloc(newSize * elSize);
			DB_XAssetPool[type] = poolEntry;
			g_poolSize[type] = newSize;

			return poolEntry;
		}

		__declspec(naked) void AssetHandler::FixupAssetLoading()
		{
			__asm
			{
				cmp edi, 0;
				je asset_null;

				cmp byte ptr[edi + 8], 0;
				je asset_continue;

				push 0x44EE2E;
				retn;

				asset_continue:
				push 0x44EF00;
				retn;

				asset_null:
				pop edi;
				pop esi;
				pop ebp;
				pop ebx;
				pop ecx;
				retn;
			}
		}

		void logEntry(std::uint32_t entry)
		{
			ZONETOOL_INFO("Hashmap entry is %u.", entry);
		}

		__declspec(naked) void TestHashMap()
		{
			static std::uintptr_t calladdr = 0x44E360;

			__asm
			{
				call calladdr;

				pushad;

				push eax;
				call logEntry;
				add esp, 4;

				popad;

				push 0x44F026;
				retn;
			}
		}

		std::uintptr_t initialZoneStreams[9];

		void InitLogStreamPos()
		{
			auto g_streamPos = reinterpret_cast<std::uintptr_t*>(0x016634FC);

			// ZONETOOL_INFO("Setting initial zone stream positions...");

			for (int i = 0; i < 9; i++)
			{
				initialZoneStreams[i] = g_streamPos[i];
			}
		}

		void LogStreamPos()
		{
			auto fastfile = static_cast<std::string>(reinterpret_cast<const char*>(*reinterpret_cast<DWORD*>(0x1294A00)
				+ 4));

			auto g_streamPos = reinterpret_cast<std::uintptr_t*>(0x016634FC);
			auto g_streamPosArr = reinterpret_cast<std::uintptr_t*>(0x01683304);

			if (fastfile.find("mp_") != std::string::npos)
			{
				for (auto i = 0u; i < 9; i++)
				{
					// ZONETOOL_INFO("Stream %i: 0x%08X 0x%08X", i, g_streamPos[i], g_streamPosArr[i]);
				}
			}
		}

		__declspec(naked) void ShitOutTheCurrentStreamPositionsStub()
		{
			static std::uintptr_t DB_AuthLoad_InflateEnd = 0x004368E0;

			__asm
			{
				// log the fuck out of this shit
				pushad;
				call LogStreamPos;
				popad;

				// call original function
				call DB_AuthLoad_InflateEnd;

				// jump back
				push 0x00436CAD;
				retn;
			}
		}

		__declspec(naked) void InitStreamsStub()
		{
			static std::uintptr_t original = 0x00436970;

			__asm
			{
				pushad;
				call InitLogStreamPos;
				popad;

				call original;

				push 0x00436C0C;
				retn;
			}
		}

		/*__declspec(naked) void hkRestoreSoundData()
		{
			static std::uintptr_t origFunc = 0x00436F20;
			__asm
			{
				// call original function first
				call origFunc;

				// mov data into the loadedSound struct

			}
		}*/

		AssetHandler::AssetHandler()
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::AssetHandler");
#endif

			// Alloc zonememory
			// mem = std::make_shared < ZoneMemory >();

			// Stream position logging
			Memory(0x00436CA8).jump(ShitOutTheCurrentStreamPositionsStub);
			Memory(0x00436C07).jump(InitStreamsStub);

#ifdef REALLOC_XASSETS
			// Empty the arrays
			memset(AssetHandler::XAssetEntries, 0,	sizeof AssetHandler::XAssetEntries);
			memset(AssetHandler::db_hashmap, 0,		sizeof AssetHandler::db_hashmap);

			// Hashmap entry checking
			Memory(0x44F021).Jump(TestHashMap);

			// Reallocate XAssetEntries
			[]()
			{
				for (std::uint32_t addr = 0x401000; addr < 0x700000; addr++)
				{
					// realloc xassetentries
					if (*reinterpret_cast<std::uint32_t*>(addr) == 0x14C91D0)
					{
						// ZONETOOL_INFO("Address 0x%08X patched.", addr);
						Memory(addr).Set(XAssetEntries);
					}

					// realloc hashmap
					if (*reinterpret_cast<std::uint32_t*>(addr) == 0x1381FB8)
					{
						// Memory(addr).Set(db_hashmap);
					}
				}

				Memory(0x133DA9C).Set(AssetEntries);
			}();

			// Reallocate XAssetEntry poolsize
			Memory(0x44E440).Set(AssetEntries);
			Memory(0x44EBF4).Set(AssetEntries);
			Memory(0x4507F5).Set(AssetEntries);
			Memory(0x4508A8).Set(AssetEntries);
			Memory(0x450908).Set(AssetEntries);
			Memory(0x450D42).Set(AssetEntries);
			Memory(0x44EBF2).Set(AssetEntries);
			Memory(0x4508A8).Set(AssetEntries);
			Memory(0x450908).Set(AssetEntries);
			Memory(0x450D50).Set(AssetEntries);
			Memory(0x44E3A8).Set(AssetEntries);
#endif

			// Always allocate a default asset, even for map assets.
			Memory(0x4505EE).nop(6);

			// Reallocate AssetPools
			ReallocateAssetPoolM(techset, 2);
			ReallocateAssetPoolM(map_ents, 5);
			ReallocateAssetPoolM(com_map, 5);
			ReallocateAssetPoolM(col_map_mp, 5);
			ReallocateAssetPoolM(gfx_map, 5);
			ReallocateAssetPoolM(fx_map, 5);
			ReallocateAssetPoolM(glass_map, 5);
			ReallocateAssetPoolM(localize, 2);
			ReallocateAssetPoolM(material, 2);
			ReallocateAssetPoolM(font, 2);
			ReallocateAssetPoolM(image, 2);
			ReallocateAssetPoolM(fx, 2);
			ReallocateAssetPoolM(xanim, 2);
			ReallocateAssetPoolM(xmodel, 2);
			ReallocateAssetPoolM(xmodelsurfs, 2);
			ReallocateAssetPoolM(physpreset, 2);
			ReallocateAssetPoolM(loaded_sound, 2);
			ReallocateAssetPoolM(sound, 2);
			ReallocateAssetPoolM(weapon, 2);
			ReallocateAssetPoolM(attachment, 2);

			// DB_AddXAssetHeader hook
			Memory(0x44EFEF).jump(DB_AddXAsset);

			// Do not modify loadedsound struct after loading
			Memory(0x0043856E).nop(19);
			Memory(0x00438556).nop(12);

			// Prevent sound data from getting lost
			// Memory(0x00438551).Jump(hkRestoreSoundData);

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		AssetHandler::~AssetHandler()
		{
		}
	}
}
