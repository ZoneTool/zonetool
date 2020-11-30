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
			// experimental iw5 dump code
			auto* iw5_weapon = new IW5::WeaponCompleteDef;
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
			memset(iw5_weapon->weapDef, 0, sizeof IW5::WeaponDef);

#define CALC_SIZE(__type__, __start_field__, __end_field__) \
	((std::size_t(&(((__type__*)nullptr)->__end_field__)) - std::size_t(&(((__type__*)nullptr)->__start_field__))) + sizeof(weapon->weapDef->__end_field__))

#define COPY_STRUCT_AREA(__type__, __start_field__, __end_field__) \
	static_assert(CALC_SIZE(IW5::WeaponDef, __start_field__, __end_field__) == CALC_SIZE(IW4::WeaponDef, __start_field__, __end_field__)); \
	memcpy(&iw5_weapon->weapDef->__start_field__, &weapon->weapDef->__start_field__, CALC_SIZE(__type__, __start_field__, __end_field__));
			
			// copy weapondef data
			COPY_STRUCT_AREA(WeaponDef, gunXModel, scanSound);
			COPY_STRUCT_AREA(WeaponDef, viewShellEjectEffect, iDamageType);
			COPY_STRUCT_AREA(WeaponDef, autoAimRange, fAdsZoomOutFrac);
			COPY_STRUCT_AREA(WeaponDef, fAdsBobFactor, fGunMaxYaw);
			COPY_STRUCT_AREA(WeaponDef, swayMaxAngle, ricochetChance);
			COPY_STRUCT_AREA(WeaponDef, parallelBounce, tracerType);
			COPY_STRUCT_AREA(WeaponDef, turretScopeZoomRate, requireLockonToFire);
			COPY_STRUCT_AREA(WeaponDef, bigExplosion, aimDownSight);
			COPY_STRUCT_AREA(WeaponDef, bRechamberWhileAds, stickToPlayers);
			COPY_STRUCT_AREA(WeaponDef, hasDetonator, offhandHoldIsCancelable);

			iw5_weapon->weapDef->bounceSound = reinterpret_cast<IW5::snd_alias_list_t**>(weapon->weapDef->bounceSound);
			iw5_weapon->weapDef->stowTag = SL_AllocString("tag_stowed_back");
			
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
