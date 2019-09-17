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
			memcpy(iw5_weapon->_portpad0, weapon->_portpad0, sizeof weapon->_portpad0);
			memcpy(iw5_weapon->_portpad1, weapon->_portpad1, sizeof weapon->_portpad1);
			memcpy(iw5_weapon->_portpad2, weapon->_portpad2, sizeof weapon->_portpad2);
			memcpy(iw5_weapon->_portpad3, weapon->_portpad3, sizeof weapon->_portpad3);
			memcpy(iw5_weapon->_portpad4, weapon->_portpad4, sizeof weapon->_portpad4);

			// copy over xanims
			iw5_weapon->szXAnims = new const char*[42];
			memset(iw5_weapon->szXAnims, 0, sizeof(const char*) * 42);
			memcpy(iw5_weapon->szXAnims, weapon->szXAnims, sizeof(const char*) * 37);

			// alloc weapondef
			iw5_weapon->WeaponDef = new IW5::WeaponDef;
			memset(iw5_weapon->WeaponDef, 0, sizeof WeaponDef);

			// copy weapondef data
			memcpy(iw5_weapon->WeaponDef->_portpad0, weapon->weapDef->_portpad0, sizeof weapon->weapDef->_portpad0);
			memcpy(iw5_weapon->WeaponDef->_portpad1, weapon->weapDef->_portpad1, sizeof weapon->weapDef->_portpad1);
			memcpy(iw5_weapon->WeaponDef->_portpad2, weapon->weapDef->_portpad2, sizeof weapon->weapDef->_portpad2);
			memcpy(iw5_weapon->WeaponDef->_portpad3, weapon->weapDef->_portpad3, sizeof weapon->weapDef->_portpad3);
			memcpy(iw5_weapon->WeaponDef->_portpad4, weapon->weapDef->_portpad4, sizeof weapon->weapDef->_portpad4);
			memcpy(iw5_weapon->WeaponDef->_portpad5, weapon->weapDef->_portpad5, sizeof weapon->weapDef->_portpad5);
			memcpy(iw5_weapon->WeaponDef->_portpad6, weapon->weapDef->_portpad6, sizeof weapon->weapDef->_portpad6);

			iw5_weapon->WeaponDef->sndArray1 = reinterpret_cast<IW5::snd_alias_list_t**>(weapon->weapDef->bounceSound);
			iw5_weapon->WeaponDef->sndArray2 = reinterpret_cast<IW5::snd_alias_list_t**>(weapon->weapDef->bounceSound);

			// fixup weapon name
			iw5_weapon->name = _strdup(va("iw5_%s", weapon->szInternalName).data());
			iw5_weapon->WeaponDef->szInternalName = _strdup(va("iw5_%s", weapon->szInternalName).data());

			// dump iw5 weapon file
			IW5::IWeaponDef::dump(iw5_weapon, SL_ConvertToString);
		}
	}
}
