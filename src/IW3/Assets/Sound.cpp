// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW4/Assets/Sound.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		void ISound::dump(snd_alias_list_t* asset, ZoneMemory* mem)
		{
			const auto iw4_asset = mem->Alloc<IW4::snd_alias_list_t>(); // new IW4::snd_alias_list_t;
			memcpy(iw4_asset, asset, sizeof snd_alias_list_t);

			iw4_asset->head = mem->Alloc<IW4::snd_alias_t>(iw4_asset->count);
			memset(iw4_asset->head, 0, sizeof IW4::snd_alias_t * iw4_asset->count);

			for (auto i = 0; i < asset->count; i++)
			{
				memcpy(&iw4_asset->head[i], &asset->head[i], 16);
				memcpy(&iw4_asset->head[i].soundFile, &asset->head[i].soundFile, sizeof snd_alias_t - 16 - 20);
				memcpy(&iw4_asset->head[i].volumeFalloffCurve, &asset->head[i].volumeFalloffCurve, 20);
			}

			IW4::ISound::dump(iw4_asset);
		}
	}
}
