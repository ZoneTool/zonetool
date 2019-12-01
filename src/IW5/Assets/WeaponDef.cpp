// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		WeaponCompleteDef* IWeaponDef::parse(const std::string& name, ZoneMemory* mem)
		{
			return nullptr;
		}

		void IWeaponDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).weapon;
			}
		}

		void IWeaponDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
			auto weapon = mem->Alloc<WeaponCompleteDef>();
			memcpy(weapon, this->asset_, sizeof WeaponCompleteDef);

			weapon->hideTags = mem->Alloc<unsigned short>(32);
			memcpy(weapon->hideTags, this->asset_->hideTags, sizeof(short) * 32);

			if (weapon->hideTags)
			{
				for (int i = 0; i < 32; i++)
				{
					if (weapon->hideTags[i])
					{
						std::string tag = SL_ConvertToString(weapon->hideTags[i]);
						weapon->hideTags[i] = buf->write_scriptstring(tag);
					}
				}
			}

			if (weapon->notetrackOverrides)
			{
				for (int i = 0; i < weapon->numNotetrackOverrides; i++)
				{
					if (weapon->notetrackOverrides[i].notetrackSoundMapKeys)
					{
						weapon->notetrackOverrides[i].notetrackSoundMapKeys = mem->Alloc<short>(24);
						memcpy(weapon->notetrackOverrides[i].notetrackSoundMapKeys,
						       this->asset_->notetrackOverrides[i].notetrackSoundMapKeys, sizeof(short) * 24);

						for (int nt = 0; nt < 24; nt++)
						{
							if (weapon->notetrackOverrides[i].notetrackSoundMapKeys[nt])
							{
								std::string tag = SL_ConvertToString(
									this->asset_->notetrackOverrides[i].notetrackSoundMapKeys[nt]);
								weapon->notetrackOverrides[i].notetrackSoundMapKeys[nt] = buf->write_scriptstring(tag);
							}
						}
					}

					if (weapon->notetrackOverrides[i].notetrackSoundMapValues)
					{
						weapon->notetrackOverrides[i].notetrackSoundMapValues = mem->Alloc<short>(24);
						memcpy(weapon->notetrackOverrides[i].notetrackSoundMapValues,
						       this->asset_->notetrackOverrides[i].notetrackSoundMapValues, sizeof(short) * 24);

						for (int nt = 0; nt < 24; nt++)
						{
							if (weapon->notetrackOverrides[i].notetrackSoundMapValues[nt])
							{
								std::string tag = SL_ConvertToString(
									this->asset_->notetrackOverrides[i].notetrackSoundMapValues[nt]);
								weapon->notetrackOverrides[i].notetrackSoundMapValues[nt] = buf->
									write_scriptstring(tag);
							}
						}
					}
				}
			}

			// weaponDef shit
			auto data = mem->Alloc<WeaponDef>(); // weapon->WeaponDef;
			memcpy(data, weapon->weapDef, sizeof WeaponDef);

#define WEAPON_SCRIPTSTRING_ARRAY(__field__,__count__) \
			if (data->__field__) \
			{ \
				data->__field__ = mem->Alloc<short>(__count__); \
				memcpy(data->__field__, this->asset_->weapDef->__field__, sizeof(short) * __count__); \
\
				for (int nt = 0; nt < __count__; nt++) \
				{ \
					std::string tag = SL_ConvertToString(data->__field__[nt]); \
					data->__field__[nt] = buf->write_scriptstring(tag); \
				} \
			}

			WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapKeys, 24);
			WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapValues, 24);
			WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapKeys, 16);
			WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapValues, 16);

			weapon->weapDef = data;
			this->asset_ = weapon;
		}

		void IWeaponDef::load_depending_WeaponDef(IZone* zone, WeaponDef* data)
		{
#define WEAPON_SUBASSET(__field__,__type__,__struct__) \
			if (data->__field__) \
			{ \
				zone->add_asset_of_type(__type__, data->__field__->name); \
			}

			for (auto i = 0u; i < 16; i++)
			{
				if (data->worldModel && data->worldModel[i])
				{
					zone->add_asset_of_type(xmodel, data->worldModel[i]->name);
				}
				if (data->gunXModel && data->gunXModel[i])
				{
					zone->add_asset_of_type(xmodel, data->gunXModel[i]->name);
				}
			}

			WEAPON_SUBASSET(viewFlashEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(worldFlashEffect, fx, FxEffectDef);

			for (auto i = 0u; i < 48; i++)
			{
				WEAPON_SUBASSET(sounds[i], sound, snd_alias_list_t);
			}

			if (data->bounceSound)
			{
				for (auto i = 0u; i < 31; i++)
				{
					if (data->bounceSound[i])
					{
						zone->add_asset_of_type(sound, data->bounceSound[i]->name);
					}
				}
			}

			if (data->rollingSound)
			{
				for (auto i = 0u; i < 31; i++)
				{
					if (data->rollingSound[i])
					{
						zone->add_asset_of_type(sound, data->rollingSound[i]->name);
					}
				}
			}

			WEAPON_SUBASSET(viewShellEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(worldShellEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(viewLastShotEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(worldLastShotEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(reticleCenter, material, Material);
			WEAPON_SUBASSET(reticleSide, material, Material);

			WEAPON_SUBASSET(worldClipModel, xmodel, XModel);
			WEAPON_SUBASSET(rocketModel, xmodel, XModel);
			WEAPON_SUBASSET(knifeModel, xmodel, XModel);
			WEAPON_SUBASSET(worldKnifeModel, xmodel, XModel);

			WEAPON_SUBASSET(hudIcon, material, Material);
			WEAPON_SUBASSET(pickupIcon, material, Material);
			WEAPON_SUBASSET(ammoCounterIcon, material, Material);

			WEAPON_SUBASSET(overlay.shader, material, Material);
			WEAPON_SUBASSET(overlay.shaderLowRes, material, Material);
			WEAPON_SUBASSET(overlay.shaderEMP, material, Material);
			WEAPON_SUBASSET(overlay.shaderEMPLowRes, material, Material);

			WEAPON_SUBASSET(physCollmap, phys_collmap, PhysCollmap);

			WEAPON_SUBASSET(projectileModel, xmodel, XModel);

			WEAPON_SUBASSET(projExplosionEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(projDudEffect, fx, FxEffectDef);

			WEAPON_SUBASSET(projExplosionSound, sound, snd_alias_list_t);
			WEAPON_SUBASSET(projDudSound, sound, snd_alias_list_t);

			WEAPON_SUBASSET(projTrailEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(projBeaconEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(projIgnitionEffect, fx, FxEffectDef);

			WEAPON_SUBASSET(projIgnitionSound, sound, snd_alias_list_t);

			WEAPON_SUBASSET(tracerType, tracer, TracerDef);

			WEAPON_SUBASSET(turretOverheatSound, sound, snd_alias_list_t);
			WEAPON_SUBASSET(turretOverheatEffect, fx, FxEffectDef);

			WEAPON_SUBASSET(turretBarrelSpinMaxSnd, sound, snd_alias_list_t);

			for (int i = 0; i < 4; i++)
			{
				WEAPON_SUBASSET(turretBarrelSpinUpSnd[i], sound, snd_alias_list_t);
				WEAPON_SUBASSET(turretBarrelSpinDownSnd[i], sound, snd_alias_list_t);
			}

			WEAPON_SUBASSET(missileConeSoundAlias, sound, snd_alias_list_t);
			WEAPON_SUBASSET(missileConeSoundAliasAtBase, sound, snd_alias_list_t);
		}

		void IWeaponDef::load_depending(IZone* zone)
		{
			auto data = this->asset_;

			for (auto i = 0u; i < 6; i++)
			{
				if (data->scopes && data->scopes[i])
				{
					zone->add_asset_of_type(attachment, this->asset_->scopes[i]->szInternalName);
				}

				if (i >= 3) continue;
				if (data->others && data->others[i])
				{
					zone->add_asset_of_type(attachment, this->asset_->others[i]->szInternalName);
				}

				// Projectile attachments require fixing.
				// if (i >= 4) continue;
				// if (data->attachment2[i])
				// {
				// 	zone->AddAssetOfType(attachment, this->asset_->attachment2[i]->szInternalName);
				// }
			}

			if (data->soundOverrides)
			{
				for (int i = 0; i < data->numSoundOverrides; i++)
				{
					if (data->soundOverrides[i].overrideSound)
					{
						zone->add_asset_of_type(sound, data->soundOverrides[i].overrideSound->name);
					}

					if (data->soundOverrides[i].altmodeSound)
					{
						zone->add_asset_of_type(sound, data->soundOverrides[i].altmodeSound->name);
					}
				}
			}

			if (data->fxOverrides)
			{
				for (int i = 0; i < data->numSoundOverrides; i++)
				{
					// if (data->fxOverrides[i].overrideFX)
					// {
					// 	zone->AddAssetOfType(fx, data->fxOverrides[i].overrideFX->name);
					// }

					// if (data->fxOverrides[i].altmodeFX)
					// {
					// 	zone->AddAssetOfType(fx, data->fxOverrides[i].altmodeFX->name);
					// }
				}
			}

			if (data->dpadIcon)
			{
				zone->add_asset_of_type(material, data->dpadIcon->name);
			}

			if (data->killIcon)
			{
				zone->add_asset_of_type(material, data->killIcon->name);
			}

			if (data->szXAnims)
			{
				for (int i = 0; i < 42; i++)
				{
					if (data->szXAnims[i])
					{
						zone->add_asset_of_type(xanim, data->szXAnims[i]);
					}
				}
			}

			load_depending_WeaponDef(zone, data->weapDef);
		}

		std::string IWeaponDef::name()
		{
			return this->name_;
		}

		std::int32_t IWeaponDef::type()
		{
			return weapon;
		}

		void IWeaponDef::write_WeaponDef(IZone* zone, ZoneBuffer* buf, WeaponCompleteDef* complete,
		                                 WeaponDef* data)
		{
			auto dest = buf->write(data);

			if (data->szOverlayName)
			{
				dest->szOverlayName = buf->write_str(data->szOverlayName);
			}

			if (data->gunXModel)
			{
				buf->align(3);
				auto destModels = buf->write(data->gunXModel, 16);

				for (auto i = 0u; i < 16; i++)
				{
					if (destModels[i])
					{
						destModels[i] = reinterpret_cast<XModel*>(
							zone->get_asset_pointer(xmodel, destModels[i]->name)
						);
					}
				}

				ZoneBuffer::ClearPointer(&dest->gunXModel);
			}

			if (data->handXModel)
			{
				dest->handXModel = reinterpret_cast<XModel*>(
					zone->get_asset_pointer(xmodel, data->handXModel->name)
				);
			}

			if (data->szXAnimsRightHanded)
			{
				buf->align(3);
				auto strings = buf->write(data->szXAnimsRightHanded, 42);

				for (auto i = 0u; i < 42; i++)
				{
					if (strings[i])
					{
						strings[i] = buf->write_str(strings[i]);
					}
				}

				ZoneBuffer::ClearPointer(&dest->szXAnimsRightHanded);
			}

			if (data->szXAnimsLeftHanded)
			{
				buf->align(3);
				auto strings = buf->write(data->szXAnimsLeftHanded, 42);

				for (auto i = 0u; i < 42; i++)
				{
					if (strings[i])
					{
						strings[i] = buf->write_str(strings[i]);
					}
				}

				ZoneBuffer::ClearPointer(&dest->szXAnimsLeftHanded);
			}

			if (data->szModeName)
			{
				dest->szModeName = buf->write_str(data->szModeName);
			}

#define WEAPON_SCRIPTSTRING_ARRAY(__field__,__count__) \
			if (data->__field__) \
			{ \
				buf->align(1); \
				buf->write(data->__field__,__count__); \
				ZoneBuffer::ClearPointer(&dest->__field__); \
			}

			WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapKeys, 24);
			WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapValues, 24);
			WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapKeys, 16);
			WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapValues, 16);

#define WEAPON_SUBASSET(__field__,__type__,__struct__) \
			if (data->__field__) \
			{ \
				dest->__field__ = reinterpret_cast<__struct__*>(zone->get_asset_pointer(__type__, data->__field__->name)); \
			}

			WEAPON_SUBASSET(viewFlashEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(worldFlashEffect, fx, FxEffectDef);

			for (auto i = 0u; i < 48; i++)
			{
				if (!data->sounds[i]) continue;

				auto ptr = -1;

				buf->align(3);
				buf->write(&ptr);
				buf->write_str(data->sounds[i]->name);
				ZoneBuffer::ClearPointer(&dest->sounds[i]);
			}

			if (data->bounceSound)
			{
				buf->align(3);
				auto destSounds = buf->write(data->bounceSound, 31);

				for (auto i = 0u; i < 31; i++)
				{
					if (destSounds[i])
					{
						destSounds[i] = reinterpret_cast<snd_alias_list_t*>(
							zone->get_asset_pointer(sound, destSounds[i]->name)
						);
					}
				}

				ZoneBuffer::ClearPointer(&dest->bounceSound);
			}

			if (data->rollingSound)
			{
				buf->align(3);
				auto destSounds = buf->write(data->rollingSound, 31);

				for (auto i = 0u; i < 31; i++)
				{
					if (destSounds[i])
					{
						destSounds[i] = reinterpret_cast<snd_alias_list_t*>(
							zone->get_asset_pointer(sound, destSounds[i]->name)
						);
					}
				}

				ZoneBuffer::ClearPointer(&dest->rollingSound);
			}

			WEAPON_SUBASSET(viewShellEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(worldShellEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(viewLastShotEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(worldLastShotEjectEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(reticleCenter, material, Material);
			WEAPON_SUBASSET(reticleSide, material, Material);

			if (data->worldModel)
			{
				buf->align(3);
				auto destModels = buf->write(data->worldModel, 16);

				for (auto i = 0u; i < 16; i++)
				{
					if (destModels[i])
					{
						destModels[i] = reinterpret_cast<XModel*>(
							zone->get_asset_pointer(xmodel, destModels[i]->name)
						);
					}
				}

				ZoneBuffer::ClearPointer(&dest->worldModel);
			}

			WEAPON_SUBASSET(worldClipModel, xmodel, XModel);
			WEAPON_SUBASSET(rocketModel, xmodel, XModel);
			WEAPON_SUBASSET(knifeModel, xmodel, XModel);
			WEAPON_SUBASSET(worldKnifeModel, xmodel, XModel);

			WEAPON_SUBASSET(hudIcon, material, Material);
			WEAPON_SUBASSET(pickupIcon, material, Material);
			WEAPON_SUBASSET(ammoCounterIcon, material, Material);

			if (data->szAmmoName)
			{
				dest->szAmmoName = buf->write_str(data->szAmmoName);
			}

			if (data->szClipName)
			{
				dest->szClipName = buf->write_str(data->szClipName);
			}

			if (data->szSharedAmmoCapName)
			{
				dest->szSharedAmmoCapName = buf->write_str(data->szSharedAmmoCapName);
			}

			WEAPON_SUBASSET(overlay.shader, material, Material);
			WEAPON_SUBASSET(overlay.shaderLowRes, material, Material);
			WEAPON_SUBASSET(overlay.shaderEMP, material, Material);
			WEAPON_SUBASSET(overlay.shaderEMPLowRes, material, Material);

			WEAPON_SUBASSET(physCollmap, phys_collmap, PhysCollmap);

			WEAPON_SUBASSET(projectileModel, xmodel, XModel);

			WEAPON_SUBASSET(projExplosionEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(projDudEffect, fx, FxEffectDef);

			WEAPON_SUBASSET(projExplosionSound, sound, snd_alias_list_t);
			WEAPON_SUBASSET(projDudSound, sound, snd_alias_list_t);

			if (data->parallelBounce)
			{
				buf->align(3);
				buf->write(data->parallelBounce, 124);
				ZoneBuffer::ClearPointer(&dest->parallelBounce);
			}

			if (data->perpendicularBounce)
			{
				buf->align(3);
				buf->write(data->perpendicularBounce, 124);
				ZoneBuffer::ClearPointer(&dest->perpendicularBounce);
			}

			WEAPON_SUBASSET(projTrailEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(projBeaconEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(projIgnitionEffect, fx, FxEffectDef);

			WEAPON_SUBASSET(projIgnitionSound, sound, snd_alias_list_t);

			if (data->accuracyGraphName[0])
			{
				dest->accuracyGraphName[0] = buf->write_str(data->accuracyGraphName[0]);
			}

			if (data->accuracyGraphKnots)
			{
				buf->align(3);
				buf->write(data->accuracyGraphKnots, data->accuracyGraphKnotCount);
				ZoneBuffer::ClearPointer(&dest->accuracyGraphKnots);
			}

			if (data->accuracyGraphName[1])
			{
				dest->accuracyGraphName[1] = buf->write_str(data->accuracyGraphName[1]);
			}

			if (data->originalAccuracyGraphKnots)
			{
				buf->align(3);
				buf->write(data->originalAccuracyGraphKnots, data->originalAccuracyGraphKnotCount);
				ZoneBuffer::ClearPointer(&dest->originalAccuracyGraphKnots);
			}

			if (data->szUseHintString)
			{
				dest->szUseHintString = buf->write_str(data->szUseHintString);
			}

			if (data->dropHintString)
			{
				dest->dropHintString = buf->write_str(data->dropHintString);
			}

			if (data->szScript)
			{
				dest->szScript = buf->write_str(data->szScript);
			}

			if (data->locationDamageMultipliers)
			{
				buf->align(3);
				buf->write(data->locationDamageMultipliers, 20);
				ZoneBuffer::ClearPointer(&dest->locationDamageMultipliers);
			}

			if (data->fireRumble)
			{
				dest->fireRumble = buf->write_str(data->fireRumble);
			}

			if (data->meleeImpactRumble)
			{
				dest->meleeImpactRumble = buf->write_str(data->meleeImpactRumble);
			}

			WEAPON_SUBASSET(tracerType, tracer, TracerDef);

			WEAPON_SUBASSET(turretOverheatSound, sound, snd_alias_list_t);
			WEAPON_SUBASSET(turretOverheatEffect, fx, FxEffectDef);

			if (data->turretBarrelSpinRumble)
			{
				dest->turretBarrelSpinRumble = buf->write_str(data->turretBarrelSpinRumble);
			}

			WEAPON_SUBASSET(turretBarrelSpinMaxSnd, sound, snd_alias_list_t);

			for (int i = 0; i < 4; i++)
			{
				WEAPON_SUBASSET(turretBarrelSpinUpSnd[i], sound, snd_alias_list_t);
				WEAPON_SUBASSET(turretBarrelSpinDownSnd[i], sound, snd_alias_list_t);
			}

			WEAPON_SUBASSET(missileConeSoundAlias, sound, snd_alias_list_t);
			WEAPON_SUBASSET(missileConeSoundAliasAtBase, sound, snd_alias_list_t);

			dest->stowOffsetModel = nullptr;
			dest->stowTag = 0;
		}
		
		void IWeaponDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->szInternalName = buf->write_str(this->name());

			if (data->weapDef)
			{
				buf->align(3);
				write_WeaponDef(zone, buf, data, data->weapDef);
				ZoneBuffer::ClearPointer(&dest->weapDef);
			}

			if (data->szDisplayName)
			{
				dest->szDisplayName = buf->write_str(data->szDisplayName);
			}

			if (data->hideTags)
			{
				buf->align(1);
				buf->write(data->hideTags, 32);
				ZoneBuffer::ClearPointer(&dest->hideTags);
			}

#define WEAPON_ATTACHMENT(__field__,__max__) \
			if (data->__field__) \
			{ \
				buf->align(3); \
				auto destAttachments = buf->write(data->__field__,__max__); \
\
				for (auto i = 0u; i < __max__; i++) \
				{ \
					if (destAttachments && destAttachments[i]) \
					{ \
						destAttachments[i] = reinterpret_cast<AttachmentDef*>( \
							zone->get_asset_pointer(attachment, destAttachments[i]->szInternalName) \
						); \
					} \
				} \
 \
				ZoneBuffer::ClearPointer(&dest->__field__); \
			}

			WEAPON_ATTACHMENT(scopes, 6);
			WEAPON_ATTACHMENT(underBarrels, 3);
			WEAPON_ATTACHMENT(others, 4);

			if (data->szXAnims)
			{
				buf->align(3);
				auto destStrings = buf->write(data->szXAnims, 42);

				for (auto i = 0u; i < 42; i++)
				{
					if (destStrings[i])
					{
						destStrings[i] = buf->write_str(destStrings[i]);
					}
				}

				ZoneBuffer::ClearPointer(&dest->szXAnims);
			}

			if (data->animOverrides)
			{
				buf->align(3);
				auto destAnimOverrides = buf->write(data->animOverrides, data->numAnimOverrides);

				for (auto i = 0u; i < data->numAnimOverrides; i++)
				{
					if (destAnimOverrides[i].overrideAnim)
					{
						destAnimOverrides[i].overrideAnim = buf->write_str(destAnimOverrides[i].overrideAnim);
					}

					if (destAnimOverrides[i].altmodeAnim)
					{
						destAnimOverrides[i].altmodeAnim = buf->write_str(destAnimOverrides[i].altmodeAnim);
					}
				}

				ZoneBuffer::ClearPointer(&dest->animOverrides);
			}

			if (data->soundOverrides)
			{
				buf->align(3);
				auto destSoundOverrides = buf->write(data->soundOverrides, data->numSoundOverrides);

				for (auto i = 0u; i < data->numSoundOverrides; i++)
				{
					if (destSoundOverrides[i].overrideSound)
					{
						auto ptr = -1;
						buf->align(3);
						buf->write(&ptr);
						buf->write_str(destSoundOverrides[i].overrideSound->name);
						ZoneBuffer::ClearPointer(&destSoundOverrides[i].overrideSound);
					}

					if (destSoundOverrides[i].altmodeSound)
					{
						auto ptr = -1;
						buf->align(3);
						buf->write(&ptr);
						buf->write_str(destSoundOverrides[i].altmodeSound->name);
						ZoneBuffer::ClearPointer(&destSoundOverrides[i].altmodeSound);
					}
				}

				ZoneBuffer::ClearPointer(&dest->soundOverrides);
			}

			if (data->fxOverrides)
			{
				buf->align(3);
				auto destFxOverrides = buf->write(data->fxOverrides, data->numFXOverrides);

				for (auto i = 0u; i < data->numFXOverrides; i++)
				{
					if (destFxOverrides[i].overrideFX)
					{
						destFxOverrides[i].overrideFX = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
							fx, destFxOverrides[i].overrideFX->name
						));
					}

					if (destFxOverrides[i].altmodeFX)
					{
						destFxOverrides[i].altmodeFX = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
							fx, destFxOverrides[i].altmodeFX->name
						));
					}
				}

				ZoneBuffer::ClearPointer(&dest->fxOverrides);
			}

			if (data->reloadOverrides)
			{
				buf->align(3);
				buf->write(data->reloadOverrides, data->numReloadStateTimerOverrides);
				ZoneBuffer::ClearPointer(&dest->reloadOverrides);
			}

			if (data->notetrackOverrides)
			{
				buf->align(3);
				auto destNoteTrackOverrides = buf->write(data->notetrackOverrides, data->numNotetrackOverrides);

				for (auto i = 0u; i < data->numNotetrackOverrides; i++)
				{
					if (destNoteTrackOverrides[i].notetrackSoundMapKeys)
					{
						buf->align(1);
						buf->write(destNoteTrackOverrides[i].notetrackSoundMapKeys, 24);
						ZoneBuffer::ClearPointer(&destNoteTrackOverrides[i].notetrackSoundMapKeys);
					}

					if (destNoteTrackOverrides[i].notetrackSoundMapValues)
					{
						buf->align(1);
						buf->write(destNoteTrackOverrides[i].notetrackSoundMapValues, 24);
						ZoneBuffer::ClearPointer(&destNoteTrackOverrides[i].notetrackSoundMapValues);
					}
				}

				ZoneBuffer::ClearPointer(&dest->notetrackOverrides);
			}

			if (data->szAltWeaponName)
			{
				dest->szAltWeaponName = buf->write_str(data->szAltWeaponName);
			}

			if (data->killIcon)
			{
				dest->killIcon = reinterpret_cast<Material*>(
					zone->get_asset_pointer(material, dest->killIcon->name)
				);
			}

			if (data->dpadIcon)
			{
				dest->dpadIcon = reinterpret_cast<Material*>(
					zone->get_asset_pointer(material, dest->dpadIcon->name)
				);
			}

			if (data->accuracyGraphKnots[0])
			{
				buf->align(3);
				buf->write(data->accuracyGraphKnots[0], data->accuracyGraphKnotCount[0]);
				ZoneBuffer::ClearPointer(&dest->accuracyGraphKnots[0]);
			}

			if (data->accuracyGraphKnots[1])
			{
				buf->align(3);
				buf->write(data->accuracyGraphKnots[1], data->accuracyGraphKnotCount[1]);
				ZoneBuffer::ClearPointer(&dest->accuracyGraphKnots[1]);
			}

			END_LOG_STREAM;
			buf->pop_stream();

			encrypt_data(dest, sizeof WeaponCompleteDef);
		}

		void IWeaponDef::dump(WeaponCompleteDef* asset, const std::function<const char*(uint16_t)>& convertToString)
		{
			
		}
	}
}
