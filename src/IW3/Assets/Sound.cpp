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
		void ISound::dump(snd_alias_list_t* asset)
		{
			auto iw4_asset = new IW4::snd_alias_list_t;
			memcpy(iw4_asset, asset, sizeof snd_alias_list_t);

			iw4_asset->head = new IW4::snd_alias_t[iw4_asset->count];
			memset(iw4_asset->head, 0, sizeof IW4::snd_alias_t * iw4_asset->count);

			for (auto i = 0; i < asset->count; i++)
			{
				memcpy(&iw4_asset->head[i], &asset->head[i], 36);
				memcpy(&iw4_asset->head[i].pitchMin, &asset->head[i].pitchMin, 16);
				iw4_asset->head[i].flags = asset->head[i].flags;
				memcpy(&iw4_asset->head[i].slavePercentage, &asset->head[i].slavePercentage, 20);
				memcpy(&iw4_asset->head[i].volumeFalloffCurve, &asset->head[i].volumeFalloffCurve, 20);
			}

			IW4::ISound::dump(iw4_asset);

			delete[] iw4_asset->head;
			delete[] iw4_asset;
		}
	}
}
