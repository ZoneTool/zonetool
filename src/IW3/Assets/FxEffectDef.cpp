// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW4/Assets/FxEffectDef.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		void IFxEffectDef::dump(FxEffectDef* asset, ZoneMemory* mem)
		{
			const auto iw4_fx = new IW4::FxEffectDef;
			memcpy(iw4_fx, asset, sizeof IW4::FxEffectDef);

			const auto elem_count = iw4_fx->elemDefCountEmission + iw4_fx->elemDefCountOneShot + iw4_fx->elemDefCountLooping;
			iw4_fx->elemDefs = new IW4::FxElemDef[elem_count];

			for (auto i = 0; i < elem_count; i++)
			{
				memcpy(&iw4_fx->elemDefs[i], &asset->elemDefs[i], sizeof IW4::FxElemDef);

				if (iw4_fx->elemDefs[i].elemType >= 5)
				{
					iw4_fx->elemDefs[i].elemType += 2;
				}

				iw4_fx->elemDefs[i].collBounds.compute(iw4_fx->elemDefs[i].collBounds.midPoint, iw4_fx->elemDefs[i].collBounds.halfSize);

				if (iw4_fx->elemDefs[i].elemType != 3 && iw4_fx->elemDefs[i].extended.trailDef)
				{
					iw4_fx->elemDefs[i].extended.trailDef = nullptr;
				}
			}
			
			IW4::IFxEffectDef::dump(iw4_fx);
		}
	}
}
