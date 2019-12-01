// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/WeaponDef.hpp"

namespace ZoneTool
{
	namespace IW4
	{		
		void IWeaponDef::dump(WeaponCompleteDef* weapon)
		{
			auto iw5_weapon = new IW5::WeaponCompleteDef;
			memset(iw5_weapon, 0, sizeof IW5::WeaponCompleteDef);

			// copy weapon data
			memcpy(iw5_weapon, weapon, 16);
			memcpy(&iw5_weapon->fAdsZoomFov, &weapon->fAdsZoomFov, 24);
			memcpy(&iw5_weapon->dpadIconRatio, &weapon->dpadIconRatio, 28);
			memcpy(&iw5_weapon->killIcon, &weapon->killIcon, 12);
			iw5_weapon->fireAnimLength = weapon->fireAnimLength;
			iw5_weapon->fireAnimLengthAkimbo = iw5_weapon->fireAnimLength;
			iw5_weapon->iFirstRaiseTime = weapon->iFirstRaiseTime;
			iw5_weapon->iFirstRaiseTimeAkimbo = iw5_weapon->iFirstRaiseTime;
			iw5_weapon->iAltRaiseTime = weapon->iAltRaiseTime;
			iw5_weapon->iAltRaiseTimeAkimbo = iw5_weapon->iAltRaiseTime;
			iw5_weapon->iFireTime = weapon->iFireTime;
			iw5_weapon->iFireTimeAkimbo = iw5_weapon->iFireTime;
			memcpy(&iw5_weapon->ammoDropStockMax, &weapon->killIcon, 28);

			// copy over xanims
			iw5_weapon->szXAnims = new const char*[42];
			memset(iw5_weapon->szXAnims, 0, sizeof(const char*) * 42);
			memcpy(iw5_weapon->szXAnims, weapon->szXAnims, sizeof(const char*) * 37);

			// alloc weapondef
			iw5_weapon->weapDef = new IW5::WeaponDef;
			memset(iw5_weapon->weapDef, 0, sizeof WeaponDef);

			// copy weapondef data
			//memcpy(iw5_weapon->WeaponDef->_portpad0, weapon->weapDef->_portpad0, sizeof weapon->weapDef->_portpad0);
			//memcpy(iw5_weapon->WeaponDef->_portpad1, weapon->weapDef->_portpad1, sizeof weapon->weapDef->_portpad1);
			//memcpy(iw5_weapon->WeaponDef->_portpad2, weapon->weapDef->_portpad2, sizeof weapon->weapDef->_portpad2);
			//memcpy(iw5_weapon->WeaponDef->_portpad3, weapon->weapDef->_portpad3, sizeof weapon->weapDef->_portpad3);
			//memcpy(iw5_weapon->WeaponDef->_portpad4, weapon->weapDef->_portpad4, sizeof weapon->weapDef->_portpad4);
			//memcpy(iw5_weapon->WeaponDef->_portpad5, weapon->weapDef->_portpad5, sizeof weapon->weapDef->_portpad5);
			//memcpy(iw5_weapon->WeaponDef->_portpad6, weapon->weapDef->_portpad6, sizeof weapon->weapDef->_portpad6);

			iw5_weapon->weapDef->bounceSound = reinterpret_cast<IW5::snd_alias_list_t**>(weapon->weapDef->bounceSound);
			iw5_weapon->weapDef->rollingSound = nullptr;
			
			// fixup weapon name
			iw5_weapon->szInternalName = _strdup(va("iw5_%s", weapon->szInternalName).data());
			iw5_weapon->weapDef->szOverlayName = _strdup(va("iw5_%s", weapon->szInternalName).data());

			// dump iw5 weapon file
			IW5::IWeaponDef::dump(iw5_weapon, SL_ConvertToString);

			// free memory
			free((void*)iw5_weapon->szInternalName);
			free((void*)iw5_weapon->weapDef->szOverlayName);
			delete[] iw5_weapon->weapDef;
			delete[] iw5_weapon->szXAnims;
			delete iw5_weapon;
		}
	}
}
