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
		union XAssetHeader;

		static Function<XAssetHeader(std::int32_t, const char*, std::uint32_t)> DB_FindXAssetHeader = 0x44E770;
		static Function<void(XZoneInfo*, std::uint32_t, std::uint32_t)> DB_LoadXAssets = 0x44F740;

		static const char* SL_ConvertToString(std::uint16_t index)
		{
			return reinterpret_cast<const char*>(*reinterpret_cast<char**>(0x01C122A4) + 12 * index + 4);
		}

		static short SL_AllocString(const std::string& string)
		{
			return Memory::func<short(const char*, std::uint32_t, std::size_t)>(0x4E75A0)(
				string.data(), 1, string.size() + 1);
		}
	}
}
