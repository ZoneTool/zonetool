// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		struct XAsset
		{
			XAssetType type;
			XAssetHeader header;
		};

		struct XAssetEntry
		{
			XAsset asset;
			unsigned int nextHash;
			unsigned int nextOverride;
			unsigned int nextPoolEntry;
		};


		class AssetHandler : public IPatch
		{
		private:
			static constexpr std::uint32_t AssetEntries = 86000;

			static XAssetEntry XAssetEntries[AssetEntries];
			static std::uint32_t db_hashmap[AssetEntries];

			static std::unordered_map<std::string, XAsset> StoredAssets;

			static std::unordered_map<std::int32_t, std::function<void*(void*)>> DuplicateAssetHandlers;

			static bool verify;
			static bool dump;

			typedef int (__cdecl * DB_GetXAssetSizeHandler_t)();
			static DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers;

			static void** DB_XAssetPool;
			static unsigned int* g_poolSize;

			static void DB_LogLoadedAsset(void* ptr, std::int32_t type);
			static void DB_AddXAsset();

			static void* ReallocateAssetPool(uint32_t type, unsigned int newSize);
			static void* ReallocateAssetPoolM(uint32_t type, int multiplier);

			static void FixupAssetLoading();

			static void* DuplicatePixelShader(void* ptr);
			static void* DuplicateVertexDecl(void* ptr);
			static void* DuplicateVertexShader(void* ptr);
			static void* DuplicateTechset(void* ptr);

		public:

			static bool IsCommonAsset(std::int32_t type, const std::string& name);

			static XAssetHeader FindXAsset(std::int32_t type, const char* name, std::uint32_t unk);
			static void AddXAsset(std::int32_t type, std::string name, void* pointer);
			static void SetVerify(bool state);
			static void SetDump(bool state);

			AssetHandler();
			~AssetHandler();
		};
	}
}
