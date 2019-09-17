#pragma once

//!!!!!!!!!!!!!!!!!
//!!!!!!!TODO!!!!!!
//!!!CHANGE OFFSETS
//!!!!!!!!!!!!!!!!!

namespace ZoneTool
{
	namespace IW3
	{
		union XAssetHeader;

		static Function<XAssetHeader(std::int32_t, const char*)> DB_FindXAssetHeader = 0x489570;
		static Function<void(XZoneInfo*, std::uint32_t, std::uint32_t)> DB_LoadXAssets; //add offset

		typedef int (__cdecl * DB_GetXAssetSizeHandler_t)();
		static DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x726A10;

		static const char* SL_ConvertToString(std::uint16_t index)
		{
			return reinterpret_cast<const char*>(*reinterpret_cast<char**>(0x14E8A04) + 12 * index + 4);
		}

		static short SL_AllocString(const std::string& string)
		{
			// TODO
		}
	}
}
