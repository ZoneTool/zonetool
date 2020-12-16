// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool::CODO
{
	union XAssetHeader;

	// TODO: fix offsets!
	static Function<XAssetHeader(std::int32_t, const char*)> DB_FindXAssetHeader = 0x407930;
	static Function<void(XZoneInfo*, std::uint32_t, std::uint32_t)> DB_LoadXAssets = 0x4E5930;

	typedef int (__cdecl * DB_GetXAssetSizeHandler_t)();
	static DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x799488;

	static const char* SL_ConvertToString(std::uint16_t index)
	{
		return Memory::func<const char*(int)>(0x004EC1D0)(index);
	}

	static short SL_AllocString(const std::string& string)
	{
		return Memory::func<short(const char*, std::uint32_t, std::size_t)>(0x00436B40)(
			string.data(), 1, string.size() + 1);
	}
}
