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

#define WEAPON_READ_FIELD(__type__, __field__) \
	if (!data[#__field__].is_null()) *(__type__*)(&weapon->__field__) = (__type__)data[#__field__].get<__type__>()

#define WEAPON_READ_FIELD_ARR(__type__, __field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < __size__; idx##__field__++) \
		{ \
			*(__type__*)(&weapon->__field__[idx##__field__]) = (__type__)data[#__field__][idx##__field__].get<__type__>(); \
		} \
	}

#define WEAPON_READ_STRING(__field__) \
	if (!data[#__field__].is_null()) weapon->__field__ = mem->StrDup(data[#__field__].get<std::string>())

#define WEAPON_READ_ASSET(__type__, __datafield__, __field__) \
	if (!data[#__field__].is_null() && data[#__field__].is_string()) \
	{ \
		auto asset##__field__ = data[#__field__].get<std::string>(); \
		if (asset##__field__.empty()) \
		{ \
			weapon->__field__ = nullptr; \
		} \
		else \
		{ \
			weapon->__field__ = DB_FindXAssetHeader(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
		} \
	}

		void parse_overlay(ADSOverlay* weapon, Json& data)
		{
			WEAPON_READ_ASSET(material, material, shader);
			WEAPON_READ_ASSET(material, material, shaderLowRes);
			WEAPON_READ_ASSET(material, material, shaderEMP);
			WEAPON_READ_ASSET(material, material, shaderEMPLowRes);
			WEAPON_READ_FIELD(int, reticle);
			WEAPON_READ_FIELD(float, width);
			WEAPON_READ_FIELD(float, height);
			WEAPON_READ_FIELD(float, widthSplitscreen);
			WEAPON_READ_FIELD(float, heightSplitscreen);
		}

		void parse_statetimers(StateTimers* weapon, Json& data)
		{
			WEAPON_READ_FIELD(int,  iFireDelay);
			WEAPON_READ_FIELD(int,  iMeleeDelay);
			WEAPON_READ_FIELD(int,  meleeChargeDelay);
			WEAPON_READ_FIELD(int,  iDetonateDelay);
			WEAPON_READ_FIELD(int,  iRechamberTime);
			WEAPON_READ_FIELD(int,  rechamberTimeOneHanded);
			WEAPON_READ_FIELD(int,  iRechamberBoltTime);
			WEAPON_READ_FIELD(int,  iHoldFireTime);
			WEAPON_READ_FIELD(int,  iDetonateTime);
			WEAPON_READ_FIELD(int,  iMeleeTime);
			WEAPON_READ_FIELD(int,  meleeChargeTime);
			WEAPON_READ_FIELD(int,  iReloadTime);
			WEAPON_READ_FIELD(int,  reloadShowRocketTime);
			WEAPON_READ_FIELD(int,  iReloadEmptyTime);
			WEAPON_READ_FIELD(int,  iReloadAddTime);
			WEAPON_READ_FIELD(int,  iReloadStartTime);
			WEAPON_READ_FIELD(int,  iReloadStartAddTime);
			WEAPON_READ_FIELD(int,  iReloadEndTime);
			WEAPON_READ_FIELD(int,  iDropTime);
			WEAPON_READ_FIELD(int,  iRaiseTime);
			WEAPON_READ_FIELD(int,  iAltDropTime);
			WEAPON_READ_FIELD(int,  quickDropTime);
			WEAPON_READ_FIELD(int,  quickRaiseTime);
			WEAPON_READ_FIELD(int,  iBreachRaiseTime);
			WEAPON_READ_FIELD(int,  iEmptyRaiseTime);
			WEAPON_READ_FIELD(int,  iEmptyDropTime);
			WEAPON_READ_FIELD(int, sprintInTime);
			WEAPON_READ_FIELD(int, sprintLoopTime);
			WEAPON_READ_FIELD(int, sprintOutTime);
			WEAPON_READ_FIELD(int,  stunnedTimeBegin);
			WEAPON_READ_FIELD(int,  stunnedTimeLoop);
			WEAPON_READ_FIELD(int,  stunnedTimeEnd);
			WEAPON_READ_FIELD(int,  nightVisionWearTime);
			WEAPON_READ_FIELD(int,  nightVisionWearTimeFadeOutEnd);
			WEAPON_READ_FIELD(int,  nightVisionWearTimePowerUp);
			WEAPON_READ_FIELD(int,  nightVisionRemoveTime);
			WEAPON_READ_FIELD(int,  nightVisionRemoveTimePowerDown);
			WEAPON_READ_FIELD(int,  nightVisionRemoveTimeFadeInStart);
			WEAPON_READ_FIELD(int,  fuseTime);
			WEAPON_READ_FIELD(int,  aiFuseTime);
			WEAPON_READ_FIELD(int,  blastFrontTime);
			WEAPON_READ_FIELD(int,  blastRightTime);
			WEAPON_READ_FIELD(int,  blastBackTime);
			WEAPON_READ_FIELD(int,  blastLeftTime);
			WEAPON_READ_FIELD(int, raiseInterruptableTime);
			WEAPON_READ_FIELD(int, firstRaiseInterruptableTime);
			WEAPON_READ_FIELD(int, reloadInterruptableTime);
			WEAPON_READ_FIELD(int, reloadEmptyInterruptableTime);
			WEAPON_READ_FIELD(int, fireInterruptableTime);
		}

		WeaponDef* parse_weapondef(Json& data, WeaponCompleteDef* baseAsset,
			ZoneMemory* mem)
		{
			auto weapon = mem->Alloc<WeaponDef>();

			if (baseAsset)
			{
				memcpy(weapon, baseAsset->weapDef, sizeof WeaponDef);
			}

			WEAPON_READ_STRING(szOverlayName);
			WEAPON_READ_ASSET(xmodel, xmodel, handXModel);
			WEAPON_READ_STRING(szModeName);
			WEAPON_READ_FIELD(int, playerAnimType);
			WEAPON_READ_FIELD(int, weapType);
			WEAPON_READ_FIELD(int, weapClass);
			WEAPON_READ_FIELD(int, penetrateType);
			WEAPON_READ_FIELD(int, inventoryType);
			WEAPON_READ_FIELD(int, fireType);
			WEAPON_READ_FIELD(int, offhandClass);
			WEAPON_READ_FIELD(int, stance);
			WEAPON_READ_ASSET(fx, fx, viewFlashEffect);
			WEAPON_READ_ASSET(fx, fx, worldFlashEffect);
			WEAPON_READ_ASSET(fx, fx, viewShellEjectEffect);
			WEAPON_READ_ASSET(fx, fx, worldShellEjectEffect);
			WEAPON_READ_ASSET(fx, fx, viewLastShotEjectEffect);
			WEAPON_READ_ASSET(fx, fx, worldLastShotEjectEffect);
			WEAPON_READ_ASSET(material, material, reticleCenter);
			WEAPON_READ_ASSET(material, material, reticleSide);
			WEAPON_READ_FIELD(int, iReticleCenterSize);
			WEAPON_READ_FIELD(int, iReticleSideSize);
			WEAPON_READ_FIELD(int, iReticleMinOfs);
			WEAPON_READ_FIELD(int, activeReticleType);
			WEAPON_READ_FIELD_ARR(float, vStandMove, 3);
			WEAPON_READ_FIELD_ARR(float, vStandRot, 3);
			WEAPON_READ_FIELD_ARR(float, strafeMove, 3);
			WEAPON_READ_FIELD_ARR(float, strafeRot, 3);
			WEAPON_READ_FIELD_ARR(float, vDuckedOfs, 3);
			WEAPON_READ_FIELD_ARR(float, vDuckedMove, 3);
			WEAPON_READ_FIELD_ARR(float, vDuckedRot, 3);
			WEAPON_READ_FIELD_ARR(float, vProneOfs, 3);
			WEAPON_READ_FIELD_ARR(float, vProneMove, 3);
			WEAPON_READ_FIELD_ARR(float, vProneRot, 3);
			WEAPON_READ_FIELD(float, fPosMoveRate);
			WEAPON_READ_FIELD(float, fPosProneMoveRate);
			WEAPON_READ_FIELD(float, fStandMoveMinSpeed);
			WEAPON_READ_FIELD(float, fDuckedMoveMinSpeed);
			WEAPON_READ_FIELD(float, fProneMoveMinSpeed);
			WEAPON_READ_FIELD(float, fPosRotRate);
			WEAPON_READ_FIELD(float, fPosProneRotRate);
			WEAPON_READ_FIELD(float, fStandRotMinSpeed);
			WEAPON_READ_FIELD(float, fDuckedRotMinSpeed);
			WEAPON_READ_FIELD(float, fProneRotMinSpeed);
			WEAPON_READ_ASSET(xmodel, xmodel, worldClipModel);
			WEAPON_READ_ASSET(xmodel, xmodel, rocketModel);
			WEAPON_READ_ASSET(xmodel, xmodel, knifeModel);
			WEAPON_READ_ASSET(xmodel, xmodel, worldKnifeModel);
			WEAPON_READ_ASSET(material, material, hudIcon);
			WEAPON_READ_FIELD(int, hudIconRatio);
			WEAPON_READ_ASSET(material, material, pickupIcon);
			WEAPON_READ_FIELD(int, pickupIconRatio);
			WEAPON_READ_ASSET(material, material, ammoCounterIcon);
			WEAPON_READ_FIELD(int, ammoCounterIconRatio);
			WEAPON_READ_FIELD(int, ammoCounterClip);
			WEAPON_READ_FIELD(int, iStartAmmo);
			WEAPON_READ_STRING(szAmmoName);
			WEAPON_READ_FIELD(int, iAmmoIndex);
			WEAPON_READ_STRING(szClipName);
			WEAPON_READ_FIELD(int, iClipIndex);
			WEAPON_READ_FIELD(int, iMaxAmmo);
			WEAPON_READ_FIELD(int, shotCount);
			WEAPON_READ_STRING(szSharedAmmoCapName);
			WEAPON_READ_FIELD(int, iSharedAmmoCapIndex);
			WEAPON_READ_FIELD(int, iSharedAmmoCap);
			WEAPON_READ_FIELD(int, damage);
			WEAPON_READ_FIELD(int, playerDamage);
			WEAPON_READ_FIELD(int, iMeleeDamage);
			WEAPON_READ_FIELD(int, iDamageType);
			parse_statetimers(&weapon->stateTimers, data["stateTimers"]);
			parse_statetimers(&weapon->akimboStateTimers, data["akimboStateTimers"]);
			WEAPON_READ_FIELD(float, autoAimRange);
			WEAPON_READ_FIELD(float, aimAssistRange);
			WEAPON_READ_FIELD(float, aimAssistRangeAds);
			WEAPON_READ_FIELD(float, aimPadding);
			WEAPON_READ_FIELD(float, enemyCrosshairRange);
			WEAPON_READ_FIELD(float, moveSpeedScale);
			WEAPON_READ_FIELD(float, adsMoveSpeedScale);
			WEAPON_READ_FIELD(float, sprintDurationScale);
			WEAPON_READ_FIELD(float, fAdsZoomInFrac);
			WEAPON_READ_FIELD(float, fAdsZoomOutFrac);
			parse_overlay(&weapon->overlay, data["overlay"]);
			WEAPON_READ_FIELD(int, overlayInterface);
			WEAPON_READ_FIELD(float, fAdsBobFactor);
			WEAPON_READ_FIELD(float, fAdsViewBobMult);
			WEAPON_READ_FIELD(float, fHipSpreadStandMin);
			WEAPON_READ_FIELD(float, fHipSpreadDuckedMin);
			WEAPON_READ_FIELD(float, fHipSpreadProneMin);
			WEAPON_READ_FIELD(float, hipSpreadStandMax);
			WEAPON_READ_FIELD(float, hipSpreadDuckedMax);
			WEAPON_READ_FIELD(float, hipSpreadProneMax);
			WEAPON_READ_FIELD(float, fHipSpreadDecayRate);
			WEAPON_READ_FIELD(float, fHipSpreadFireAdd);
			WEAPON_READ_FIELD(float, fHipSpreadTurnAdd);
			WEAPON_READ_FIELD(float, fHipSpreadMoveAdd);
			WEAPON_READ_FIELD(float, fHipSpreadDuckedDecay);
			WEAPON_READ_FIELD(float, fHipSpreadProneDecay);
			WEAPON_READ_FIELD(float, fHipReticleSidePos);
			WEAPON_READ_FIELD(float, fAdsIdleAmount);
			WEAPON_READ_FIELD(float, fHipIdleAmount);
			WEAPON_READ_FIELD(float, adsIdleSpeed);
			WEAPON_READ_FIELD(float, hipIdleSpeed);
			WEAPON_READ_FIELD(float, fIdleCrouchFactor);
			WEAPON_READ_FIELD(float, fIdleProneFactor);
			WEAPON_READ_FIELD(float, fGunMaxPitch);
			WEAPON_READ_FIELD(float, fGunMaxYaw);
			WEAPON_READ_FIELD(float, adsIdleLerpStartTime);
			WEAPON_READ_FIELD(float, adsIdleLerpTime);
			WEAPON_READ_FIELD(float, swayMaxAngle);
			WEAPON_READ_FIELD(float, swayLerpSpeed);
			WEAPON_READ_FIELD(float, swayPitchScale);
			WEAPON_READ_FIELD(float, swayYawScale);
			WEAPON_READ_FIELD(float, swayHorizScale);
			WEAPON_READ_FIELD(float, swayVertScale);
			WEAPON_READ_FIELD(float, swayShellShockScale);
			WEAPON_READ_FIELD(float, adsSwayMaxAngle);
			WEAPON_READ_FIELD(float, adsSwayLerpSpeed);
			WEAPON_READ_FIELD(float, adsSwayPitchScale);
			WEAPON_READ_FIELD(float, adsSwayYawScale);
			WEAPON_READ_FIELD(float, adsSwayHorizScale);
			WEAPON_READ_FIELD(float, adsSwayVertScale);
			WEAPON_READ_FIELD(float, adsViewErrorMin);
			WEAPON_READ_FIELD(float, adsViewErrorMax);
			// WEAPON_READ_ASSET(phys_collmap, phys_collmap, physCollmap);
			WEAPON_READ_FIELD(float, dualWieldViewModelOffset);
			WEAPON_READ_FIELD(int, killIconRatio);
			WEAPON_READ_FIELD(int, iReloadAmmoAdd);
			WEAPON_READ_FIELD(int, iReloadStartAdd);
			WEAPON_READ_FIELD(int, ammoDropStockMin);
			WEAPON_READ_FIELD(int, ammoDropClipPercentMin);
			WEAPON_READ_FIELD(int, ammoDropClipPercentMax);
			WEAPON_READ_FIELD(int, iExplosionRadius);
			WEAPON_READ_FIELD(int, iExplosionRadiusMin);
			WEAPON_READ_FIELD(int, iExplosionInnerDamage);
			WEAPON_READ_FIELD(int, iExplosionOuterDamage);
			WEAPON_READ_FIELD(float, damageConeAngle);
			WEAPON_READ_FIELD(float, bulletExplDmgMult);
			WEAPON_READ_FIELD(float, bulletExplRadiusMult);
			WEAPON_READ_FIELD(int, iProjectileSpeed);
			WEAPON_READ_FIELD(int, iProjectileSpeedUp);
			WEAPON_READ_FIELD(int, iProjectileSpeedForward);
			WEAPON_READ_FIELD(int, iProjectileActivateDist);
			WEAPON_READ_FIELD(float, projLifetime);
			WEAPON_READ_FIELD(float, timeToAccelerate);
			WEAPON_READ_FIELD(float, projectileCurvature);
			WEAPON_READ_ASSET(xmodel, xmodel, projectileModel);
			WEAPON_READ_FIELD(int, projExplosion);
			WEAPON_READ_ASSET(fx, fx, projExplosionEffect);
			WEAPON_READ_ASSET(fx, fx, projDudEffect);
			WEAPON_READ_ASSET(sound, sound, projExplosionSound);
			WEAPON_READ_ASSET(sound, sound, projDudSound);
			WEAPON_READ_FIELD(int, stickiness);
			WEAPON_READ_FIELD(float, lowAmmoWarningThreshold);
			WEAPON_READ_FIELD(float, ricochetChance);
			WEAPON_READ_FIELD(bool, riotShieldEnableDamage);
			WEAPON_READ_FIELD(int, riotShieldHealth);
			WEAPON_READ_FIELD(float, riotShieldDamageMult);
			WEAPON_READ_ASSET(fx, fx, projTrailEffect);
			WEAPON_READ_ASSET(fx, fx, projBeaconEffect);
			WEAPON_READ_FIELD(float, vProjectileColor[3]);
			WEAPON_READ_FIELD(int, guidedMissileType);
			WEAPON_READ_FIELD(float, maxSteeringAccel);
			WEAPON_READ_FIELD(int, projIgnitionDelay);
			WEAPON_READ_ASSET(fx, fx, projIgnitionEffect);
			WEAPON_READ_ASSET(sound, sound, projIgnitionSound);
			WEAPON_READ_FIELD(float, fAdsAimPitch);
			WEAPON_READ_FIELD(float, fAdsCrosshairInFrac);
			WEAPON_READ_FIELD(float, fAdsCrosshairOutFrac);
			WEAPON_READ_FIELD(int, adsGunKickReducedKickBullets);
			WEAPON_READ_FIELD(float, adsGunKickReducedKickPercent);
			WEAPON_READ_FIELD(float, fAdsGunKickPitchMin);
			WEAPON_READ_FIELD(float, fAdsGunKickPitchMax);
			WEAPON_READ_FIELD(float, fAdsGunKickYawMin);
			WEAPON_READ_FIELD(float, fAdsGunKickYawMax);
			WEAPON_READ_FIELD(float, fAdsGunKickAccel);
			WEAPON_READ_FIELD(float, fAdsGunKickSpeedMax);
			WEAPON_READ_FIELD(float, fAdsGunKickSpeedDecay);
			WEAPON_READ_FIELD(float, fAdsGunKickStaticDecay);
			WEAPON_READ_FIELD(float, fAdsViewKickPitchMin);
			WEAPON_READ_FIELD(float, fAdsViewKickPitchMax);
			WEAPON_READ_FIELD(float, fAdsViewKickYawMin);
			WEAPON_READ_FIELD(float, fAdsViewKickYawMax);
			WEAPON_READ_FIELD(float, fAdsViewScatterMin);
			WEAPON_READ_FIELD(float, fAdsViewScatterMax);
			WEAPON_READ_FIELD(float, fAdsSpread);
			WEAPON_READ_FIELD(int, hipGunKickReducedKickBullets);
			WEAPON_READ_FIELD(float, hipGunKickReducedKickPercent);
			WEAPON_READ_FIELD(float, fHipGunKickPitchMin);
			WEAPON_READ_FIELD(float, fHipGunKickPitchMax);
			WEAPON_READ_FIELD(float, fHipGunKickYawMin);
			WEAPON_READ_FIELD(float, fHipGunKickYawMax);
			WEAPON_READ_FIELD(float, fHipGunKickAccel);
			WEAPON_READ_FIELD(float, fHipGunKickSpeedMax);
			WEAPON_READ_FIELD(float, fHipGunKickSpeedDecay);
			WEAPON_READ_FIELD(float, fHipGunKickStaticDecay);
			WEAPON_READ_FIELD(float, fHipViewKickPitchMin);
			WEAPON_READ_FIELD(float, fHipViewKickPitchMax);
			WEAPON_READ_FIELD(float, fHipViewKickYawMin);
			WEAPON_READ_FIELD(float, fHipViewKickYawMax);
			WEAPON_READ_FIELD(float, fHipViewScatterMin);
			WEAPON_READ_FIELD(float, fHipViewScatterMax);
			WEAPON_READ_FIELD(float, fightDist);
			WEAPON_READ_FIELD(float, maxDist);
			// WEAPON_READ_STRING(accuracyGraphName[2]);
			WEAPON_READ_FIELD(short, accuracyGraphKnotCount);
			WEAPON_READ_FIELD(short, originalAccuracyGraphKnotCount);
			WEAPON_READ_FIELD(int, iPositionReloadTransTime);
			WEAPON_READ_FIELD(float, leftArc);
			WEAPON_READ_FIELD(float, rightArc);
			WEAPON_READ_FIELD(float, topArc);
			WEAPON_READ_FIELD(float, bottomArc);
			WEAPON_READ_FIELD(float, accuracy);
			WEAPON_READ_FIELD(float, aiSpread);
			WEAPON_READ_FIELD(float, playerSpread);
			WEAPON_READ_FIELD(float, minTurnSpeed[2]);
			WEAPON_READ_FIELD(float, maxTurnSpeed[2]);
			WEAPON_READ_FIELD(float, pitchConvergenceTime);
			WEAPON_READ_FIELD(float, yawConvergenceTime);
			WEAPON_READ_FIELD(float, suppressTime);
			WEAPON_READ_FIELD(float, maxRange);
			WEAPON_READ_FIELD(float, fAnimHorRotateInc);
			WEAPON_READ_FIELD(float, fPlayerPositionDist);
			WEAPON_READ_STRING(szUseHintString);
			WEAPON_READ_STRING(dropHintString);
			WEAPON_READ_FIELD(int, iUseHintStringIndex);
			WEAPON_READ_FIELD(int, dropHintStringIndex);
			WEAPON_READ_FIELD(float, horizViewJitter);
			WEAPON_READ_FIELD(float, vertViewJitter);
			WEAPON_READ_FIELD(float, scanSpeed);
			WEAPON_READ_FIELD(float, scanAccel);
			WEAPON_READ_FIELD(int, scanPauseTime);
			WEAPON_READ_STRING(szScript);
			WEAPON_READ_FIELD_ARR(float, fOOPosAnimLength, 2);
			WEAPON_READ_FIELD(int, minDamage);
			WEAPON_READ_FIELD(int, minPlayerDamage);
			WEAPON_READ_FIELD(float, fMaxDamageRange);
			WEAPON_READ_FIELD(float, fMinDamageRange);
			WEAPON_READ_FIELD(float, destabilizationRateTime);
			WEAPON_READ_FIELD(float, destabilizationCurvatureMax);
			WEAPON_READ_FIELD(int, destabilizeDistance);
			WEAPON_READ_FIELD_ARR(float, locationDamageMultipliers, 20);
			WEAPON_READ_STRING(fireRumble);
			WEAPON_READ_STRING(meleeImpactRumble);
			WEAPON_READ_ASSET(tracer, tracer, tracerType);
			WEAPON_READ_FIELD(bool, turretADSEnabled);
			WEAPON_READ_FIELD(float, turretADSTime);
			WEAPON_READ_FIELD(float, turretFov);
			WEAPON_READ_FIELD(float, turretFovADS);
			WEAPON_READ_FIELD(float, turretScopeZoomRate);
			WEAPON_READ_FIELD(float, turretScopeZoomMin);
			WEAPON_READ_FIELD(float, turretScopeZoomMax);
			WEAPON_READ_FIELD(float, turretOverheatUpRate);
			WEAPON_READ_FIELD(float, turretOverheatDownRate);
			WEAPON_READ_FIELD(float, turretOverheatPenalty);
			WEAPON_READ_ASSET(sound, sound, turretOverheatSound);
			WEAPON_READ_ASSET(fx, fx, turretOverheatEffect);
			WEAPON_READ_STRING(turretBarrelSpinRumble);
			WEAPON_READ_FIELD(float, turretBarrelSpinSpeed);
			WEAPON_READ_FIELD(float, turretBarrelSpinUpTime);
			WEAPON_READ_FIELD(float, turretBarrelSpinDownTime);
			WEAPON_READ_ASSET(sound, sound, turretBarrelSpinMaxSnd);
			WEAPON_READ_ASSET(sound, sound, missileConeSoundAlias);
			WEAPON_READ_ASSET(sound, sound, missileConeSoundAliasAtBase);
			WEAPON_READ_FIELD(float, missileConeSoundRadiusAtTop);
			WEAPON_READ_FIELD(float, missileConeSoundRadiusAtBase);
			WEAPON_READ_FIELD(float, missileConeSoundHeight);
			WEAPON_READ_FIELD(float, missileConeSoundOriginOffset);
			WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtCore);
			WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtEdge);
			WEAPON_READ_FIELD(float, missileConeSoundVolumescaleCoreSize);
			WEAPON_READ_FIELD(float, missileConeSoundPitchAtTop);
			WEAPON_READ_FIELD(float, missileConeSoundPitchAtBottom);
			WEAPON_READ_FIELD(float, missileConeSoundPitchTopSize);
			WEAPON_READ_FIELD(float, missileConeSoundPitchBottomSize);
			WEAPON_READ_FIELD(float, missileConeSoundCrossfadeTopSize);
			WEAPON_READ_FIELD(float, missileConeSoundCrossfadeBottomSize);
			WEAPON_READ_FIELD(bool, sharedAmmo);
			WEAPON_READ_FIELD(bool, lockonSupported);
			WEAPON_READ_FIELD(bool, requireLockonToFire);
			WEAPON_READ_FIELD(bool, isAirburstWeapon);
			WEAPON_READ_FIELD(bool, bigExplosion);
			WEAPON_READ_FIELD(bool, noAdsWhenMagEmpty);
			WEAPON_READ_FIELD(bool, avoidDropCleanup);
			WEAPON_READ_FIELD(bool, inheritsPerks);
			WEAPON_READ_FIELD(bool, crosshairColorChange);
			WEAPON_READ_FIELD(bool, bRifleBullet);
			WEAPON_READ_FIELD(bool, armorPiercing);
			WEAPON_READ_FIELD(bool, bBoltAction);
			WEAPON_READ_FIELD(bool, aimDownSight);
			WEAPON_READ_FIELD(bool, canHoldBreath);
			WEAPON_READ_FIELD(bool, canVariableZoom);
			WEAPON_READ_FIELD(bool, bRechamberWhileAds);
			WEAPON_READ_FIELD(bool, bBulletExplosiveDamage);
			WEAPON_READ_FIELD(bool, bCookOffHold);
			WEAPON_READ_FIELD(bool, bClipOnly);
			WEAPON_READ_FIELD(bool, noAmmoPickup);
			WEAPON_READ_FIELD(bool, adsFireOnly);
			WEAPON_READ_FIELD(bool, cancelAutoHolsterWhenEmpty);
			WEAPON_READ_FIELD(bool, disableSwitchToWhenEmpty);
			WEAPON_READ_FIELD(bool, suppressAmmoReserveDisplay);
			WEAPON_READ_FIELD(bool, laserSightDuringNightvision);
			WEAPON_READ_FIELD(bool, markableViewmodel);
			WEAPON_READ_FIELD(bool, noDualWield);
			WEAPON_READ_FIELD(bool, flipKillIcon);
			WEAPON_READ_FIELD(bool, bNoPartialReload);
			WEAPON_READ_FIELD(bool, bSegmentedReload);
			WEAPON_READ_FIELD(bool, blocksProne);
			WEAPON_READ_FIELD(bool, silenced);
			WEAPON_READ_FIELD(bool, isRollingGrenade);
			WEAPON_READ_FIELD(bool, projExplosionEffectForceNormalUp);
			WEAPON_READ_FIELD(bool, bProjImpactExplode);
			WEAPON_READ_FIELD(bool, stickToPlayers);
			WEAPON_READ_FIELD(bool, stickToVehicles);
			WEAPON_READ_FIELD(bool, stickToTurrets);
			WEAPON_READ_FIELD(bool, hasDetonator);
			WEAPON_READ_FIELD(bool, disableFiring);
			WEAPON_READ_FIELD(bool, timedDetonation);
			WEAPON_READ_FIELD(bool, rotate);
			WEAPON_READ_FIELD(bool, holdButtonToThrow);
			WEAPON_READ_FIELD(bool, freezeMovementWhenFiring);
			WEAPON_READ_FIELD(bool, thermalScope);
			WEAPON_READ_FIELD(bool, altModeSameWeapon);
			WEAPON_READ_FIELD(bool, turretBarrelSpinEnabled);
			WEAPON_READ_FIELD(bool, missileConeSoundEnabled);
			WEAPON_READ_FIELD(bool, missileConeSoundPitchshiftEnabled);
			WEAPON_READ_FIELD(bool, missileConeSoundCrossfadeEnabled);
			WEAPON_READ_FIELD(bool, offhandHoldIsCancelable);
			WEAPON_READ_FIELD(bool, doNotAllowAttachmentsToOverrideSpread);
			WEAPON_READ_ASSET(xmodel, xmodel, stowOffsetModel);

			// parse stowtag
			if (!data["stowTag"].is_null())
			{
				auto stowTag = data["stowTag"].get<std::string>();
				weapon->stowTag = SL_AllocString(stowTag);
			}


			weapon->accuracyGraphName[0] = nullptr;
			weapon->accuracyGraphName[1] = nullptr;
			weapon->accuracyGraphKnots = nullptr;
			weapon->originalAccuracyGraphKnots = nullptr;
			weapon->accuracyGraphKnotCount = 0;
			weapon->originalAccuracyGraphKnotCount = 0;
			weapon->parallelBounce = nullptr;
			weapon->perpendicularBounce = nullptr;

			weapon->gunXModel = mem->Alloc<XModel*>(16);
			weapon->worldModel = mem->Alloc<XModel*>(16);

			for (int i = 0; i < 16; i++)
			{
				auto gunmodel = data["gunXModel"][i].get<std::string>();

				if (!gunmodel.empty())
				{
					weapon->gunXModel[i] = DB_FindXAssetHeader(xmodel, gunmodel.data(), 1).xmodel;
				}
			}
			for (int i = 0; i < 16; i++)
			{
				auto gunmodel = data["worldXModel"][i].get<std::string>();

				if (!gunmodel.empty())
				{
					weapon->worldModel[i] = DB_FindXAssetHeader(xmodel, gunmodel.data(), 1).xmodel;
				}
			}

			weapon->notetrackSoundMapKeys = mem->Alloc<short>(24);
			weapon->notetrackSoundMapValues = mem->Alloc<short>(24);
			for (int i = 0; i < 24; i++)
			{
				auto notetrack = data["notetrackSoundMapKeys"][i].get<std::string>();
				weapon->notetrackSoundMapKeys[i] = SL_AllocString(notetrack);
			}
			for (int i = 0; i < 24; i++)
			{
				auto notetrack = data["notetrackSoundMapValues"][i].get<std::string>();
				weapon->notetrackSoundMapValues[i] = SL_AllocString(notetrack);
			}

			weapon->notetrackRumbleMapKeys = mem->Alloc<short>(16);
			weapon->notetrackRumbleMapValues = mem->Alloc<short>(16);
			for (int i = 0; i < 16; i++)
			{
				auto notetrack = data["notetrackRumbleMapKeys"][i].get<std::string>();
				weapon->notetrackRumbleMapKeys[i] = SL_AllocString(notetrack);
			}
			for (int i = 0; i < 16; i++)
			{
				auto notetrack = data["notetrackRumbleMapValues"][i].get<std::string>();
				weapon->notetrackRumbleMapValues[i] = SL_AllocString(notetrack);
			}

			for (int i = 0; i < 48; i++)
			{
				auto sound = data["sounds"][i].get<std::string>();

				if (!sound.empty())
				{
					weapon->sounds[i] = DB_FindXAssetHeader(XAssetType::sound, sound.data(), 1).sound;
				}
			}

			return weapon;
		}

		WeaponCompleteDef* IWeaponDef::parse(const std::string& name, ZoneMemory* mem)
		{
			sizeof WeaponCompleteDef;

			auto path = "weapons/mp/" + name;

			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing weapon \"%s\"...", name.data());

			auto weapon = mem->Alloc<WeaponCompleteDef>();

			// parse json file
			auto file = FileSystem::FileOpen(path, "rb");
			auto size = FileSystem::FileSize(file);
			auto bytes = FileSystem::ReadBytes(file, size);
			FileSystem::FileClose(file);
			nlohmann::json data = nlohmann::json::parse(bytes);

			// base asset
			auto base = data["baseAsset"].get<std::string>();
			WeaponCompleteDef* baseAsset = nullptr;

			if (!base.empty())
			{
				baseAsset = DB_FindXAssetHeader(XAssetType::weapon, base.data(), 1).weapon;
				memcpy(weapon, baseAsset, sizeof WeaponCompleteDef);
			}
			else
			{
				ZONETOOL_WARNING("No base asset is defined for weapon %s, stuff might go wrong!", name.data());
			}

			WEAPON_READ_STRING(szInternalName);
			WEAPON_READ_STRING(szDisplayName);
			WEAPON_READ_FIELD(unsigned int, numAnimOverrides);
			WEAPON_READ_FIELD(unsigned int, numSoundOverrides);
			WEAPON_READ_FIELD(unsigned int, numFXOverrides);
			WEAPON_READ_FIELD(unsigned int, numReloadStateTimerOverrides);
			WEAPON_READ_FIELD(unsigned int, numNotetrackOverrides);
			WEAPON_READ_FIELD(float, fAdsZoomFov);
			WEAPON_READ_FIELD(int, iAdsTransInTime);
			WEAPON_READ_FIELD(int, iAdsTransOutTime);
			WEAPON_READ_FIELD(int, iClipSize);
			WEAPON_READ_FIELD(int, impactType);
			WEAPON_READ_FIELD(int, iFireTime);
			WEAPON_READ_FIELD(int, iFireTimeAkimbo);
			WEAPON_READ_FIELD(int, dpadIconRatio);
			WEAPON_READ_FIELD(float, penetrateMultiplier);
			WEAPON_READ_FIELD(float, fAdsViewKickCenterSpeed);
			WEAPON_READ_FIELD(float, fHipViewKickCenterSpeed);
			WEAPON_READ_STRING(szAltWeaponName);
			WEAPON_READ_FIELD(int, altWeapon);
			WEAPON_READ_FIELD(int, iAltRaiseTime);
			WEAPON_READ_FIELD(int, iAltRaiseTimeAkimbo);
			WEAPON_READ_ASSET(material, material, killIcon);
			WEAPON_READ_ASSET(material, material, dpadIcon);
			WEAPON_READ_FIELD(int, fireAnimLength);
			WEAPON_READ_FIELD(int, fireAnimLengthAkimbo);
			WEAPON_READ_FIELD(int, iFirstRaiseTime);
			WEAPON_READ_FIELD(int, iFirstRaiseTimeAkimbo);
			WEAPON_READ_FIELD(int, ammoDropStockMax);
			WEAPON_READ_FIELD(float, adsDofStart);
			WEAPON_READ_FIELD(float, adsDofEnd);
			WEAPON_READ_FIELD_ARR(unsigned __int16,  accuracyGraphKnotCount, 2);
			WEAPON_READ_FIELD(bool, motionTracker);
			WEAPON_READ_FIELD(bool, enhanced);
			WEAPON_READ_FIELD(bool, dpadIconShowsAmmo);

			// parse weapondef
			weapon->weapDef = parse_weapondef(data["weapDef"], baseAsset, mem);

			weapon->hideTags = mem->Alloc<unsigned __int16>(32);
			for (int i = 0; i < 32; i++)
			{
				weapon->hideTags[i] = SL_AllocString(data["hideTags"][i].get<std::string>());
			}

			weapon->scopes = mem->Alloc<AttachmentDef*>(6);
			for (int i = 0; i < 6; i++)
			{
				auto attachment = data["scopes"][i].get<std::string>();

				if (!attachment.empty())
				{
					weapon->scopes[i] = DB_FindXAssetHeader(XAssetType::attachment, attachment.data(), 1).
						attachment;
				}
			}
			weapon->underBarrels = mem->Alloc<AttachmentDef*>(3);
			for (int i = 0; i < 3; i++)
			{
				auto attachment = data["underBarrels"][i].get<std::string>();

				if (!attachment.empty())
				{
					weapon->underBarrels[i] = DB_FindXAssetHeader(XAssetType::attachment, attachment.data(), 1).
						attachment;
				}
			}
			weapon->others = mem->Alloc<AttachmentDef*>(4);
			for (int i = 0; i < 4; i++)
			{
				auto attachment = data["others"][i].get<std::string>();

				if (!attachment.empty())
				{
					weapon->others[i] = DB_FindXAssetHeader(XAssetType::attachment, attachment.data(), 1).
						attachment;
				}
			}

			weapon->szXAnims = mem->Alloc<const char*>(42);
			for (int i = 0; i < 42; i++)
			{
				weapon->szXAnims[i] = mem->StrDup(data["szXAnims"][i].get<std::string>());
			}

			if (weapon->numAnimOverrides)
			{
				weapon->animOverrides = mem->Alloc<AnimOverrideEntry>(weapon->numAnimOverrides);
				for (int i = 0; i < weapon->numAnimOverrides; i++)
				{
					weapon->animOverrides[i].altmodeAnim = mem->StrDup(
						data["animOverrides"][i]["altmodeAnim"].is_string()
						? data["animOverrides"][i]["altmodeAnim"].get<std::string>()
						: "");
					weapon->animOverrides[i].overrideAnim = mem->StrDup(
						data["animOverrides"][i]["overrideAnim"].is_string()
						? data["animOverrides"][i]["overrideAnim"].get<std::string>()
						: "");
					weapon->animOverrides[i].attachment1 = data["animOverrides"][i]["attachment1"].get<unsigned short
					>();
					weapon->animOverrides[i].attachment2 = data["animOverrides"][i]["attachment2"].get<unsigned short
					>();
					weapon->animOverrides[i].altTime = data["animOverrides"][i]["altTime"].get<int>();
					weapon->animOverrides[i].animTime = data["animOverrides"][i]["animTime"].get<int>();
					weapon->animOverrides[i].animTreeType = data["animOverrides"][i]["animTreeType"].get<unsigned int
					>();
				}
			}

			if (weapon->numSoundOverrides)
			{
				weapon->soundOverrides = mem->Alloc<SoundOverrideEntry>(weapon->numSoundOverrides);
				for (int i = 0; i < weapon->numSoundOverrides; i++)
				{
					weapon->soundOverrides[i].altmodeSound = (data["soundOverrides"][i]["altmodeSound"].is_string())
						? DB_FindXAssetHeader(
							sound,
							data["soundOverrides"][i]["altmodeSound"]
							.get<std::string>().data(), 1).sound
						: nullptr;
					weapon->soundOverrides[i].overrideSound = (data["soundOverrides"][i]["overrideSound"].is_string())
						? DB_FindXAssetHeader(
							sound,
							data["soundOverrides"][i]["overrideSound"]
							.get<std::string>().data(), 1).sound
						: nullptr;
					weapon->soundOverrides[i].attachment1 = data["soundOverrides"][i]["attachment1"].get<unsigned short
					>();
					weapon->soundOverrides[i].attachment2 = data["soundOverrides"][i]["attachment2"].get<unsigned short
					>();
					weapon->soundOverrides[i].soundType = data["soundOverrides"][i]["soundType"].get<unsigned int>();
				}
			}

			if (weapon->numFXOverrides)
			{
				weapon->fxOverrides = mem->Alloc<FXOverrideEntry>(weapon->numFXOverrides);
				for (int i = 0; i < weapon->numFXOverrides; i++)
				{
					weapon->fxOverrides[i].altmodeFX = (data["fxOverrides"][i]["altmodeFX"].is_string())
						? DB_FindXAssetHeader(
							fx, data["fxOverrides"][i]["altmodeFX"]
							.get<std::string>().data(), 1).fx
						: nullptr;
					weapon->fxOverrides[i].overrideFX = (data["fxOverrides"][i]["overrideFX"].is_string())
						? DB_FindXAssetHeader(
							fx, data["fxOverrides"][i]["overrideFX"]
							.get<std::string>().data(), 1).fx
						: nullptr;
					weapon->fxOverrides[i].attachment1 = data["fxOverrides"][i]["attachment1"].get<unsigned short>();
					weapon->fxOverrides[i].attachment2 = data["fxOverrides"][i]["attachment2"].get<unsigned short>();
					weapon->fxOverrides[i].fxType = data["fxOverrides"][i]["fxType"].get<unsigned int>();
				}
			}

			if (weapon->numReloadStateTimerOverrides)
			{
				weapon->reloadOverrides = mem->Alloc<ReloadStateTimerEntry>(weapon->numReloadStateTimerOverrides);
				for (int i = 0; i < weapon->numReloadStateTimerOverrides; i++)
				{
					weapon->reloadOverrides[i].attachment = data["attachment"].get<int>();
					weapon->reloadOverrides[i].reloadAddTime = data["reloadAddTime"].get<int>();
					weapon->reloadOverrides[i].reloadEmptyAddTime = data["reloadEmptyAddTime"].get<int>();
				}
			}

			if (weapon->numNotetrackOverrides)
			{
				// TODO
				weapon->numNotetrackOverrides = 0;
			}



			return weapon;
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

			data->stowTag = buf->write_scriptstring(SL_ConvertToString(data->stowTag));

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

			if (data->handXModel)
			{
				zone->add_asset_of_type(xmodel, data->handXModel->name);
			}

			WEAPON_SUBASSET(viewFlashEffect, fx, FxEffectDef);
			WEAPON_SUBASSET(worldFlashEffect, fx, FxEffectDef);

			if (data->notetrackSoundMapValues)
			{
				for (auto i = 0; i < 24; i++)
				{
					if (data->notetrackSoundMapValues[i])
					{
						zone->add_asset_of_type(sound, SL_ConvertToString(data->notetrackSoundMapValues[i]));
					}
				}
			}
			
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

			WEAPON_SUBASSET(stowOffsetModel, xmodel, XModel);

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
					 if (data->fxOverrides[i].overrideFX)
					 {
					 	zone->add_asset_of_type(fx, data->fxOverrides[i].overrideFX->name);
					 }

					 if (data->fxOverrides[i].altmodeFX)
					 {
					 	zone->add_asset_of_type(fx, data->fxOverrides[i].altmodeFX->name);
					 }
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

			if (data->animOverrides)
			{
				for (auto i = 0u; i < data->numAnimOverrides; i++)
				{
					if (data->animOverrides[i].overrideAnim)
					{
						zone->add_asset_of_type(xanim, data->animOverrides[i].overrideAnim);
					}

					if (data->animOverrides[i].altmodeAnim)
					{
						zone->add_asset_of_type(xanim, data->animOverrides[i].altmodeAnim);
					}
				}
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

				ZoneBuffer::clear_pointer(&dest->gunXModel);
			}

			// Seems to be writting NULL, should fix at some point
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

				ZoneBuffer::clear_pointer(&dest->szXAnimsRightHanded);
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

				ZoneBuffer::clear_pointer(&dest->szXAnimsLeftHanded);
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
				ZoneBuffer::clear_pointer(&dest->__field__); \
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
				ZoneBuffer::clear_pointer(&dest->sounds[i]);
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

				ZoneBuffer::clear_pointer(&dest->bounceSound);
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

				ZoneBuffer::clear_pointer(&dest->rollingSound);
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

				ZoneBuffer::clear_pointer(&dest->worldModel);
			}

			WEAPON_SUBASSET(worldClipModel, xmodel, XModel);
			WEAPON_SUBASSET(rocketModel, xmodel, XModel);
			WEAPON_SUBASSET(knifeModel, xmodel, XModel);
			WEAPON_SUBASSET(worldKnifeModel, xmodel, XModel);

			WEAPON_SUBASSET(stowOffsetModel, xmodel, XModel);

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
				ZoneBuffer::clear_pointer(&dest->parallelBounce);
			}

			if (data->perpendicularBounce)
			{
				buf->align(3);
				buf->write(data->perpendicularBounce, 124);
				ZoneBuffer::clear_pointer(&dest->perpendicularBounce);
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
				ZoneBuffer::clear_pointer(&dest->accuracyGraphKnots);
			}

			if (data->accuracyGraphName[1])
			{
				dest->accuracyGraphName[1] = buf->write_str(data->accuracyGraphName[1]);
			}

			if (data->originalAccuracyGraphKnots)
			{
				buf->align(3);
				buf->write(data->originalAccuracyGraphKnots, data->originalAccuracyGraphKnotCount);
				ZoneBuffer::clear_pointer(&dest->originalAccuracyGraphKnots);
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
				ZoneBuffer::clear_pointer(&dest->locationDamageMultipliers);
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
		}
		
		void IWeaponDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->szInternalName = buf->write_str(this->name());

			if (data->weapDef)
			{
				buf->align(3);
				write_WeaponDef(zone, buf, data, data->weapDef);
				ZoneBuffer::clear_pointer(&dest->weapDef);
			}

			if (data->szDisplayName)
			{
				dest->szDisplayName = buf->write_str(data->szDisplayName);
			}

			if (data->hideTags)
			{
				buf->align(1);
				buf->write(data->hideTags, 32);
				ZoneBuffer::clear_pointer(&dest->hideTags);
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
				ZoneBuffer::clear_pointer(&dest->__field__); \
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

				ZoneBuffer::clear_pointer(&dest->szXAnims);
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

				ZoneBuffer::clear_pointer(&dest->animOverrides);
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
						ZoneBuffer::clear_pointer(&destSoundOverrides[i].overrideSound);
					}

					if (destSoundOverrides[i].altmodeSound)
					{
						auto ptr = -1;
						buf->align(3);
						buf->write(&ptr);
						buf->write_str(destSoundOverrides[i].altmodeSound->name);
						ZoneBuffer::clear_pointer(&destSoundOverrides[i].altmodeSound);
					}
				}

				ZoneBuffer::clear_pointer(&dest->soundOverrides);
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

				ZoneBuffer::clear_pointer(&dest->fxOverrides);
			}

			if (data->reloadOverrides)
			{
				buf->align(3);
				buf->write(data->reloadOverrides, data->numReloadStateTimerOverrides);
				ZoneBuffer::clear_pointer(&dest->reloadOverrides);
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
						ZoneBuffer::clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapKeys);
					}

					if (destNoteTrackOverrides[i].notetrackSoundMapValues)
					{
						buf->align(1);
						buf->write(destNoteTrackOverrides[i].notetrackSoundMapValues, 24);
						ZoneBuffer::clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapValues);
					}
				}

				ZoneBuffer::clear_pointer(&dest->notetrackOverrides);
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
				ZoneBuffer::clear_pointer(&dest->accuracyGraphKnots[0]);
			}

			if (data->accuracyGraphKnots[1])
			{
				buf->align(3);
				buf->write(data->accuracyGraphKnots[1], data->accuracyGraphKnotCount[1]);
				ZoneBuffer::clear_pointer(&dest->accuracyGraphKnots[1]);
			}

			END_LOG_STREAM;
			buf->pop_stream();

			encrypt_data(dest, sizeof WeaponCompleteDef);
		}

#define WEAPON_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__

#define WEAPON_DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define WEAPON_DUMP_FIELD_ARR_STR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		if (asset->__field__[idx##__field__] != nullptr) \
			data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
		else \
			data[#__field__][idx##__field__] = ""; \
	}

#define WEAPON_DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

		Json dump_overlay(ADSOverlay* asset)
		{
			Json data;

			WEAPON_DUMP_ASSET(shader);
			WEAPON_DUMP_ASSET(shaderLowRes);
			WEAPON_DUMP_ASSET(shaderEMP);
			WEAPON_DUMP_ASSET(shaderEMPLowRes);
			WEAPON_DUMP_FIELD(reticle);
			WEAPON_DUMP_FIELD(width);
			WEAPON_DUMP_FIELD(height);
			WEAPON_DUMP_FIELD(widthSplitscreen);
			WEAPON_DUMP_FIELD(heightSplitscreen);

			return data;
		}

		Json dump_statetimers(StateTimers* asset)
		{
			Json data;

			WEAPON_DUMP_FIELD(iFireDelay);
			WEAPON_DUMP_FIELD(iMeleeDelay);
			WEAPON_DUMP_FIELD(meleeChargeDelay);
			WEAPON_DUMP_FIELD(iDetonateDelay);
			WEAPON_DUMP_FIELD(iRechamberTime);
			WEAPON_DUMP_FIELD(rechamberTimeOneHanded);
			WEAPON_DUMP_FIELD(iRechamberBoltTime);
			WEAPON_DUMP_FIELD(iHoldFireTime);
			WEAPON_DUMP_FIELD(iDetonateTime);
			WEAPON_DUMP_FIELD(iMeleeTime);
			WEAPON_DUMP_FIELD(meleeChargeTime);
			WEAPON_DUMP_FIELD(iReloadTime);
			WEAPON_DUMP_FIELD(reloadShowRocketTime);
			WEAPON_DUMP_FIELD(iReloadEmptyTime);
			WEAPON_DUMP_FIELD(iReloadAddTime);
			WEAPON_DUMP_FIELD(iReloadStartTime);
			WEAPON_DUMP_FIELD(iReloadStartAddTime);
			WEAPON_DUMP_FIELD(iReloadEndTime);
			WEAPON_DUMP_FIELD(iDropTime);
			WEAPON_DUMP_FIELD(iRaiseTime);
			WEAPON_DUMP_FIELD(iAltDropTime);
			WEAPON_DUMP_FIELD(quickDropTime);
			WEAPON_DUMP_FIELD(quickRaiseTime);
			WEAPON_DUMP_FIELD(iBreachRaiseTime);
			WEAPON_DUMP_FIELD(iEmptyRaiseTime);
			WEAPON_DUMP_FIELD(iEmptyDropTime);
			WEAPON_DUMP_FIELD(sprintInTime);
			WEAPON_DUMP_FIELD(sprintLoopTime);
			WEAPON_DUMP_FIELD(sprintOutTime);
			WEAPON_DUMP_FIELD(stunnedTimeBegin);
			WEAPON_DUMP_FIELD(stunnedTimeLoop);
			WEAPON_DUMP_FIELD(stunnedTimeEnd);
			WEAPON_DUMP_FIELD(nightVisionWearTime);
			WEAPON_DUMP_FIELD(nightVisionWearTimeFadeOutEnd);
			WEAPON_DUMP_FIELD(nightVisionWearTimePowerUp);
			WEAPON_DUMP_FIELD(nightVisionRemoveTime);
			WEAPON_DUMP_FIELD(nightVisionRemoveTimePowerDown);
			WEAPON_DUMP_FIELD(nightVisionRemoveTimeFadeInStart);
			WEAPON_DUMP_FIELD(fuseTime);
			WEAPON_DUMP_FIELD(aiFuseTime);
			WEAPON_DUMP_FIELD(blastFrontTime);
			WEAPON_DUMP_FIELD(blastRightTime);
			WEAPON_DUMP_FIELD(blastBackTime);
			WEAPON_DUMP_FIELD(blastLeftTime);
			WEAPON_DUMP_FIELD(raiseInterruptableTime);
			WEAPON_DUMP_FIELD(firstRaiseInterruptableTime);
			WEAPON_DUMP_FIELD(reloadInterruptableTime);
			WEAPON_DUMP_FIELD(reloadEmptyInterruptableTime);
			WEAPON_DUMP_FIELD(fireInterruptableTime);

			return data;
		}

		Json dump_weapondef(WeaponDef* asset, const std::function<const char* (uint16_t)>& convertToString)
		{
			Json data;

			assert(sizeof(WeaponDef) == 1956);

			for (int i = 0; i < 16; i++)
			{
				if (asset->gunXModel && asset->gunXModel[i])
				{
					data["gunXModel"][i] = asset->gunXModel[i]->name;
				}
				else
				{
					data["gunXModel"][i] = "";
				}

				if (asset->worldModel && asset->worldModel[i])
				{
					data["worldXModel"][i] = asset->worldModel[i]->name;
				}
				else
				{
					data["worldXModel"][i] = "";
				}
			}

			for (int i = 0; i < 24; i++)
			{
				if (asset->notetrackSoundMapKeys && asset->notetrackSoundMapKeys[i])
				{
					data["notetrackSoundMapKeys"][i] = convertToString(asset->notetrackSoundMapKeys[i]);
				}
				else
				{
					data["notetrackSoundMapKeys"][i] = "";
				}

				if (asset->notetrackSoundMapValues && asset->notetrackSoundMapValues[i])
				{
					data["notetrackSoundMapValues"][i] = convertToString(asset->notetrackSoundMapValues[i]);
				}
				else
				{
					data["notetrackSoundMapValues"][i] = "";
				}
			}

			for (int i = 0; i < 16; i++)
			{
				if (asset->notetrackRumbleMapKeys && asset->notetrackRumbleMapKeys[i])
				{
					data["notetrackRumbleMapKeys"][i] = convertToString(asset->notetrackRumbleMapKeys[i]);
				}
				else
				{
					data["notetrackRumbleMapKeys"][i] = "";
				}

				if (asset->notetrackRumbleMapValues && asset->notetrackRumbleMapValues[i])
				{
					data["notetrackRumbleMapValues"][i] = convertToString(asset->notetrackRumbleMapValues[i]);
				}
				else
				{
					data["notetrackRumbleMapValues"][i] = "";
				}
			}

			for (int i = 0; i < 48; i++)
			{
				if (asset->sounds && asset->sounds[i])
				{
					data["sounds"][i] = asset->sounds[i]->name;
				}
				else
				{
					data["sounds"][i] = "";
				}
			}

			for (int i = 0; i < 31; i++)
			{
				if (asset->bounceSound && asset->bounceSound[i])
				{
					data["bounceSound"][i] = asset->bounceSound[i]->name;
				}
				else
				{
					data["bounceSound"][i] = "";
				}

				if (asset->rollingSound && asset->rollingSound[i])
				{
					data["rollingSound"][i] = asset->rollingSound[i]->name;
				}
				else
				{
					data["rollingSound"][i] = "";
				}
			}

			for (int i = 0; i < 4; i++)
			{
				if (asset->turretBarrelSpinUpSnd && asset->turretBarrelSpinUpSnd[i])
				{
					data["turretBarrelSpinUpSnd"][i] = asset->turretBarrelSpinUpSnd[i]->name;
				}
				else
				{
					data["turretBarrelSpinUpSnd"][i] = "";
				}

				if (asset->turretBarrelSpinDownSnd && asset->turretBarrelSpinDownSnd[i])
				{
					data["turretBarrelSpinDownSnd"][i] = asset->turretBarrelSpinDownSnd[i]->name;
				}
				else
				{
					data["turretBarrelSpinDownSnd"][i] = "";
				}
			}


			WEAPON_DUMP_FIELD(szOverlayName);
			WEAPON_DUMP_ASSET(handXModel);
			WEAPON_DUMP_FIELD(szModeName);
			WEAPON_DUMP_FIELD(playerAnimType);
			WEAPON_DUMP_FIELD(weapType);
			WEAPON_DUMP_FIELD(weapClass);
			WEAPON_DUMP_FIELD(penetrateType);
			WEAPON_DUMP_FIELD(inventoryType);
			WEAPON_DUMP_FIELD(fireType);
			WEAPON_DUMP_FIELD(offhandClass);
			WEAPON_DUMP_FIELD(stance);
			WEAPON_DUMP_ASSET(viewFlashEffect);
			WEAPON_DUMP_ASSET(worldFlashEffect);
			WEAPON_DUMP_ASSET(viewShellEjectEffect);
			WEAPON_DUMP_ASSET(worldShellEjectEffect);
			WEAPON_DUMP_ASSET(viewLastShotEjectEffect);
			WEAPON_DUMP_ASSET(worldLastShotEjectEffect);
			WEAPON_DUMP_ASSET(reticleCenter);
			WEAPON_DUMP_ASSET(reticleSide);
			WEAPON_DUMP_FIELD(iReticleCenterSize);
			WEAPON_DUMP_FIELD(iReticleSideSize);
			WEAPON_DUMP_FIELD(iReticleMinOfs);
			WEAPON_DUMP_FIELD(activeReticleType);

			WEAPON_DUMP_FIELD_ARR(vStandMove, 3);
			WEAPON_DUMP_FIELD_ARR(vStandRot, 3);
			WEAPON_DUMP_FIELD_ARR(strafeMove, 3);
			WEAPON_DUMP_FIELD_ARR(strafeRot, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedMove, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedRot, 3);
			WEAPON_DUMP_FIELD_ARR(vProneOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vProneMove, 3);
			WEAPON_DUMP_FIELD_ARR(vProneRot, 3);

			WEAPON_DUMP_FIELD(fPosMoveRate);
			WEAPON_DUMP_FIELD(fPosProneMoveRate);
			WEAPON_DUMP_FIELD(fStandMoveMinSpeed);
			WEAPON_DUMP_FIELD(fDuckedMoveMinSpeed);
			WEAPON_DUMP_FIELD(fProneMoveMinSpeed);
			WEAPON_DUMP_FIELD(fPosRotRate);
			WEAPON_DUMP_FIELD(fPosProneRotRate);
			WEAPON_DUMP_FIELD(fStandRotMinSpeed);
			WEAPON_DUMP_FIELD(fDuckedRotMinSpeed);
			WEAPON_DUMP_FIELD(fProneRotMinSpeed);
			WEAPON_DUMP_ASSET(worldClipModel);
			WEAPON_DUMP_ASSET(rocketModel);
			WEAPON_DUMP_ASSET(knifeModel);
			WEAPON_DUMP_ASSET(worldKnifeModel);
			WEAPON_DUMP_ASSET(hudIcon);
			WEAPON_DUMP_FIELD(hudIconRatio);
			WEAPON_DUMP_ASSET(pickupIcon);
			WEAPON_DUMP_FIELD(pickupIconRatio);
			WEAPON_DUMP_ASSET(ammoCounterIcon);
			WEAPON_DUMP_FIELD(ammoCounterIconRatio);
			WEAPON_DUMP_FIELD(ammoCounterClip);
			WEAPON_DUMP_FIELD(iStartAmmo);
			WEAPON_DUMP_FIELD(szAmmoName);
			WEAPON_DUMP_FIELD(iAmmoIndex);
			WEAPON_DUMP_FIELD(szClipName);
			WEAPON_DUMP_FIELD(iClipIndex);
			WEAPON_DUMP_FIELD(iMaxAmmo);
			WEAPON_DUMP_FIELD(shotCount);
			WEAPON_DUMP_FIELD(szSharedAmmoCapName);
			WEAPON_DUMP_FIELD(iSharedAmmoCapIndex);
			WEAPON_DUMP_FIELD(iSharedAmmoCap);
			WEAPON_DUMP_FIELD(damage);
			WEAPON_DUMP_FIELD(playerDamage);
			WEAPON_DUMP_FIELD(iMeleeDamage);
			WEAPON_DUMP_FIELD(iDamageType);

			data["stateTimers"] = dump_statetimers(&asset->stateTimers);
			data["akimboStateTimers"] = dump_statetimers(&asset->akimboStateTimers);

			WEAPON_DUMP_FIELD(autoAimRange);
			WEAPON_DUMP_FIELD(aimAssistRange);
			WEAPON_DUMP_FIELD(aimAssistRangeAds);
			WEAPON_DUMP_FIELD(aimPadding);
			WEAPON_DUMP_FIELD(enemyCrosshairRange);
			WEAPON_DUMP_FIELD(moveSpeedScale);
			WEAPON_DUMP_FIELD(adsMoveSpeedScale);
			WEAPON_DUMP_FIELD(sprintDurationScale);
			WEAPON_DUMP_FIELD(fAdsZoomInFrac);
			WEAPON_DUMP_FIELD(fAdsZoomOutFrac);
			
			data["overlay"] = dump_overlay(&asset->overlay);

			WEAPON_DUMP_FIELD(overlayInterface);
			WEAPON_DUMP_FIELD(fAdsBobFactor);
			WEAPON_DUMP_FIELD(fAdsViewBobMult);
			WEAPON_DUMP_FIELD(fHipSpreadStandMin);
			WEAPON_DUMP_FIELD(fHipSpreadDuckedMin);
			WEAPON_DUMP_FIELD(fHipSpreadProneMin);
			WEAPON_DUMP_FIELD(hipSpreadStandMax);
			WEAPON_DUMP_FIELD(hipSpreadDuckedMax);
			WEAPON_DUMP_FIELD(hipSpreadProneMax);
			WEAPON_DUMP_FIELD(fHipSpreadDecayRate);
			WEAPON_DUMP_FIELD(fHipSpreadFireAdd);
			WEAPON_DUMP_FIELD(fHipSpreadTurnAdd);
			WEAPON_DUMP_FIELD(fHipSpreadMoveAdd);
			WEAPON_DUMP_FIELD(fHipSpreadDuckedDecay);
			WEAPON_DUMP_FIELD(fHipSpreadProneDecay);
			WEAPON_DUMP_FIELD(fHipReticleSidePos);
			WEAPON_DUMP_FIELD(fAdsIdleAmount);
			WEAPON_DUMP_FIELD(fHipIdleAmount);
			WEAPON_DUMP_FIELD(adsIdleSpeed);
			WEAPON_DUMP_FIELD(hipIdleSpeed);
			WEAPON_DUMP_FIELD(fIdleCrouchFactor);
			WEAPON_DUMP_FIELD(fIdleProneFactor);
			WEAPON_DUMP_FIELD(fGunMaxPitch);
			WEAPON_DUMP_FIELD(fGunMaxYaw);
			WEAPON_DUMP_FIELD(adsIdleLerpStartTime);
			WEAPON_DUMP_FIELD(adsIdleLerpTime);
			WEAPON_DUMP_FIELD(swayMaxAngle);
			WEAPON_DUMP_FIELD(swayLerpSpeed);
			WEAPON_DUMP_FIELD(swayPitchScale);
			WEAPON_DUMP_FIELD(swayYawScale);
			WEAPON_DUMP_FIELD(swayHorizScale);
			WEAPON_DUMP_FIELD(swayVertScale);
			WEAPON_DUMP_FIELD(swayShellShockScale);
			WEAPON_DUMP_FIELD(adsSwayMaxAngle);
			WEAPON_DUMP_FIELD(adsSwayLerpSpeed);
			WEAPON_DUMP_FIELD(adsSwayPitchScale);
			WEAPON_DUMP_FIELD(adsSwayYawScale);
			WEAPON_DUMP_FIELD(adsSwayHorizScale);
			WEAPON_DUMP_FIELD(adsSwayVertScale);
			WEAPON_DUMP_FIELD(adsViewErrorMin);
			WEAPON_DUMP_FIELD(adsViewErrorMax);
			WEAPON_DUMP_ASSET(physCollmap);
			WEAPON_DUMP_FIELD(dualWieldViewModelOffset);
			WEAPON_DUMP_FIELD(killIconRatio);
			WEAPON_DUMP_FIELD(iReloadAmmoAdd);
			WEAPON_DUMP_FIELD(iReloadStartAdd);
			WEAPON_DUMP_FIELD(ammoDropStockMin);
			WEAPON_DUMP_FIELD(ammoDropClipPercentMin);
			WEAPON_DUMP_FIELD(ammoDropClipPercentMax);
			WEAPON_DUMP_FIELD(iExplosionRadius);
			WEAPON_DUMP_FIELD(iExplosionRadiusMin);
			WEAPON_DUMP_FIELD(iExplosionInnerDamage);
			WEAPON_DUMP_FIELD(iExplosionOuterDamage);
			WEAPON_DUMP_FIELD(damageConeAngle);
			WEAPON_DUMP_FIELD(bulletExplDmgMult);
			WEAPON_DUMP_FIELD(bulletExplRadiusMult);
			WEAPON_DUMP_FIELD(iProjectileSpeed);
			WEAPON_DUMP_FIELD(iProjectileSpeedUp);
			WEAPON_DUMP_FIELD(iProjectileSpeedForward);
			WEAPON_DUMP_FIELD(iProjectileActivateDist);
			WEAPON_DUMP_FIELD(projLifetime);
			WEAPON_DUMP_FIELD(timeToAccelerate);
			WEAPON_DUMP_FIELD(projectileCurvature);
			WEAPON_DUMP_ASSET(projectileModel);
			WEAPON_DUMP_FIELD(projExplosion);
			WEAPON_DUMP_ASSET(projExplosionEffect);
			WEAPON_DUMP_ASSET(projDudEffect);
			WEAPON_DUMP_ASSET(projExplosionSound);
			WEAPON_DUMP_ASSET(projDudSound);
			WEAPON_DUMP_FIELD(stickiness);
			WEAPON_DUMP_FIELD(lowAmmoWarningThreshold);
			WEAPON_DUMP_FIELD(ricochetChance);
			WEAPON_DUMP_FIELD(riotShieldEnableDamage);
			WEAPON_DUMP_FIELD(riotShieldHealth);
			WEAPON_DUMP_FIELD(riotShieldDamageMult);

			WEAPON_DUMP_ASSET(projTrailEffect);
			WEAPON_DUMP_ASSET(projBeaconEffect);

			WEAPON_DUMP_FIELD_ARR(vProjectileColor, 3);
			WEAPON_DUMP_FIELD(guidedMissileType);
			WEAPON_DUMP_FIELD(maxSteeringAccel);
			WEAPON_DUMP_FIELD(projIgnitionDelay);
			WEAPON_DUMP_ASSET(projIgnitionEffect);
			WEAPON_DUMP_ASSET(projIgnitionSound);
			WEAPON_DUMP_FIELD(fAdsAimPitch);
			WEAPON_DUMP_FIELD(fAdsCrosshairInFrac);
			WEAPON_DUMP_FIELD(fAdsCrosshairOutFrac);
			WEAPON_DUMP_FIELD(adsGunKickReducedKickBullets);
			WEAPON_DUMP_FIELD(adsGunKickReducedKickPercent);
			WEAPON_DUMP_FIELD(fAdsGunKickPitchMin);
			WEAPON_DUMP_FIELD(fAdsGunKickPitchMax);
			WEAPON_DUMP_FIELD(fAdsGunKickYawMin);
			WEAPON_DUMP_FIELD(fAdsGunKickYawMax);
			WEAPON_DUMP_FIELD(fAdsGunKickAccel);
			WEAPON_DUMP_FIELD(fAdsGunKickSpeedMax);
			WEAPON_DUMP_FIELD(fAdsGunKickSpeedDecay);
			WEAPON_DUMP_FIELD(fAdsGunKickStaticDecay);
			WEAPON_DUMP_FIELD(fAdsViewKickPitchMin);
			WEAPON_DUMP_FIELD(fAdsViewKickPitchMax);
			WEAPON_DUMP_FIELD(fAdsViewKickYawMin);
			WEAPON_DUMP_FIELD(fAdsViewKickYawMax);
			WEAPON_DUMP_FIELD(fAdsViewScatterMin);
			WEAPON_DUMP_FIELD(fAdsViewScatterMax);
			WEAPON_DUMP_FIELD(fAdsSpread);
			WEAPON_DUMP_FIELD(hipGunKickReducedKickBullets);
			WEAPON_DUMP_FIELD(hipGunKickReducedKickPercent);
			WEAPON_DUMP_FIELD(fHipGunKickPitchMin);
			WEAPON_DUMP_FIELD(fHipGunKickPitchMax);
			WEAPON_DUMP_FIELD(fHipGunKickYawMin);
			WEAPON_DUMP_FIELD(fHipGunKickYawMax);
			WEAPON_DUMP_FIELD(fHipGunKickAccel);
			WEAPON_DUMP_FIELD(fHipGunKickSpeedMax);
			WEAPON_DUMP_FIELD(fHipGunKickSpeedDecay);
			WEAPON_DUMP_FIELD(fHipGunKickStaticDecay);
			WEAPON_DUMP_FIELD(fHipViewKickPitchMin);
			WEAPON_DUMP_FIELD(fHipViewKickPitchMax);
			WEAPON_DUMP_FIELD(fHipViewKickYawMin);
			WEAPON_DUMP_FIELD(fHipViewKickYawMax);
			WEAPON_DUMP_FIELD(fHipViewScatterMin);
			WEAPON_DUMP_FIELD(fHipViewScatterMax);
			WEAPON_DUMP_FIELD(fightDist);
			WEAPON_DUMP_FIELD(maxDist);
			WEAPON_DUMP_FIELD_ARR_STR(accuracyGraphName, 2);
			// vec2_t* accuracyGraphKnots);
			// vec2_t* originalAccuracyGraphKnots);
			WEAPON_DUMP_FIELD(accuracyGraphKnotCount);
			WEAPON_DUMP_FIELD(originalAccuracyGraphKnotCount);
			WEAPON_DUMP_FIELD(iPositionReloadTransTime);
			WEAPON_DUMP_FIELD(leftArc);
			WEAPON_DUMP_FIELD(rightArc);
			WEAPON_DUMP_FIELD(topArc);
			WEAPON_DUMP_FIELD(bottomArc);
			WEAPON_DUMP_FIELD(accuracy);
			WEAPON_DUMP_FIELD(aiSpread);
			WEAPON_DUMP_FIELD(playerSpread);
			WEAPON_DUMP_FIELD_ARR(minTurnSpeed, 2);
			WEAPON_DUMP_FIELD_ARR(maxTurnSpeed, 2);
			WEAPON_DUMP_FIELD(pitchConvergenceTime);
			WEAPON_DUMP_FIELD(yawConvergenceTime);
			WEAPON_DUMP_FIELD(suppressTime);
			WEAPON_DUMP_FIELD(maxRange);
			WEAPON_DUMP_FIELD(fAnimHorRotateInc);
			WEAPON_DUMP_FIELD(fPlayerPositionDist);
			WEAPON_DUMP_FIELD(szUseHintString);
			WEAPON_DUMP_FIELD(dropHintString);
			WEAPON_DUMP_FIELD(iUseHintStringIndex);
			WEAPON_DUMP_FIELD(dropHintStringIndex);
			WEAPON_DUMP_FIELD(horizViewJitter);
			WEAPON_DUMP_FIELD(vertViewJitter);
			WEAPON_DUMP_FIELD(scanSpeed);
			WEAPON_DUMP_FIELD(scanAccel);
			WEAPON_DUMP_FIELD(scanPauseTime);
			WEAPON_DUMP_FIELD(szScript);
			WEAPON_DUMP_FIELD_ARR(fOOPosAnimLength, 2);
			WEAPON_DUMP_FIELD(minDamage);
			WEAPON_DUMP_FIELD(minPlayerDamage);
			WEAPON_DUMP_FIELD(fMaxDamageRange);
			WEAPON_DUMP_FIELD(fMinDamageRange);
			WEAPON_DUMP_FIELD(destabilizationRateTime);
			WEAPON_DUMP_FIELD(destabilizationCurvatureMax);
			WEAPON_DUMP_FIELD(destabilizeDistance);
			WEAPON_DUMP_FIELD_ARR(locationDamageMultipliers, 20);
			WEAPON_DUMP_FIELD(fireRumble);
			WEAPON_DUMP_FIELD(meleeImpactRumble);
			WEAPON_DUMP_ASSET(tracerType);
			WEAPON_DUMP_FIELD(turretADSEnabled);
			WEAPON_DUMP_FIELD(turretADSTime);
			WEAPON_DUMP_FIELD(turretFov);
			WEAPON_DUMP_FIELD(turretFovADS);
			WEAPON_DUMP_FIELD(turretScopeZoomRate);
			WEAPON_DUMP_FIELD(turretScopeZoomMin);
			WEAPON_DUMP_FIELD(turretScopeZoomMax);
			WEAPON_DUMP_FIELD(turretOverheatUpRate);
			WEAPON_DUMP_FIELD(turretOverheatDownRate);
			WEAPON_DUMP_FIELD(turretOverheatPenalty);
			WEAPON_DUMP_ASSET(turretOverheatSound);
			WEAPON_DUMP_ASSET(turretOverheatEffect);
			WEAPON_DUMP_FIELD(turretBarrelSpinRumble);
			WEAPON_DUMP_FIELD(turretBarrelSpinSpeed);
			WEAPON_DUMP_FIELD(turretBarrelSpinUpTime);
			WEAPON_DUMP_FIELD(turretBarrelSpinDownTime);
			WEAPON_DUMP_ASSET(turretBarrelSpinMaxSnd);
			WEAPON_DUMP_ASSET(missileConeSoundAlias);
			WEAPON_DUMP_ASSET(missileConeSoundAliasAtBase);
			WEAPON_DUMP_FIELD(missileConeSoundRadiusAtTop);
			WEAPON_DUMP_FIELD(missileConeSoundRadiusAtBase);
			WEAPON_DUMP_FIELD(missileConeSoundHeight);
			WEAPON_DUMP_FIELD(missileConeSoundOriginOffset);
			WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtCore);
			WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtEdge);
			WEAPON_DUMP_FIELD(missileConeSoundVolumescaleCoreSize);
			WEAPON_DUMP_FIELD(missileConeSoundPitchAtTop);
			WEAPON_DUMP_FIELD(missileConeSoundPitchAtBottom);
			WEAPON_DUMP_FIELD(missileConeSoundPitchTopSize);
			WEAPON_DUMP_FIELD(missileConeSoundPitchBottomSize);
			WEAPON_DUMP_FIELD(missileConeSoundCrossfadeTopSize);
			WEAPON_DUMP_FIELD(missileConeSoundCrossfadeBottomSize);
			WEAPON_DUMP_FIELD(sharedAmmo);
			WEAPON_DUMP_FIELD(lockonSupported);
			WEAPON_DUMP_FIELD(requireLockonToFire);
			WEAPON_DUMP_FIELD(isAirburstWeapon);
			WEAPON_DUMP_FIELD(bigExplosion);
			WEAPON_DUMP_FIELD(noAdsWhenMagEmpty);
			WEAPON_DUMP_FIELD(avoidDropCleanup);
			WEAPON_DUMP_FIELD(inheritsPerks);
			WEAPON_DUMP_FIELD(crosshairColorChange);
			WEAPON_DUMP_FIELD(bRifleBullet);
			WEAPON_DUMP_FIELD(armorPiercing);
			WEAPON_DUMP_FIELD(bBoltAction);
			WEAPON_DUMP_FIELD(aimDownSight);
			WEAPON_DUMP_FIELD(canHoldBreath);
			WEAPON_DUMP_FIELD(canVariableZoom);
			WEAPON_DUMP_FIELD(bRechamberWhileAds);
			WEAPON_DUMP_FIELD(bBulletExplosiveDamage);
			WEAPON_DUMP_FIELD(bCookOffHold);
			WEAPON_DUMP_FIELD(bClipOnly);
			WEAPON_DUMP_FIELD(noAmmoPickup);
			WEAPON_DUMP_FIELD(adsFireOnly);
			WEAPON_DUMP_FIELD(cancelAutoHolsterWhenEmpty);
			WEAPON_DUMP_FIELD(disableSwitchToWhenEmpty);
			WEAPON_DUMP_FIELD(suppressAmmoReserveDisplay);
			WEAPON_DUMP_FIELD(laserSightDuringNightvision);
			WEAPON_DUMP_FIELD(markableViewmodel);
			WEAPON_DUMP_FIELD(noDualWield);
			WEAPON_DUMP_FIELD(flipKillIcon);
			WEAPON_DUMP_FIELD(bNoPartialReload);
			WEAPON_DUMP_FIELD(bSegmentedReload);
			WEAPON_DUMP_FIELD(blocksProne);
			WEAPON_DUMP_FIELD(silenced);
			WEAPON_DUMP_FIELD(isRollingGrenade);
			WEAPON_DUMP_FIELD(projExplosionEffectForceNormalUp);
			WEAPON_DUMP_FIELD(bProjImpactExplode);
			WEAPON_DUMP_FIELD(stickToPlayers);
			WEAPON_DUMP_FIELD(stickToVehicles);
			WEAPON_DUMP_FIELD(stickToTurrets);
			WEAPON_DUMP_FIELD(hasDetonator);
			WEAPON_DUMP_FIELD(disableFiring);
			WEAPON_DUMP_FIELD(timedDetonation);
			WEAPON_DUMP_FIELD(rotate);
			WEAPON_DUMP_FIELD(holdButtonToThrow);
			WEAPON_DUMP_FIELD(freezeMovementWhenFiring);
			WEAPON_DUMP_FIELD(thermalScope);
			WEAPON_DUMP_FIELD(altModeSameWeapon);
			WEAPON_DUMP_FIELD(turretBarrelSpinEnabled);
			WEAPON_DUMP_FIELD(missileConeSoundEnabled);
			WEAPON_DUMP_FIELD(missileConeSoundPitchshiftEnabled);
			WEAPON_DUMP_FIELD(missileConeSoundCrossfadeEnabled);
			WEAPON_DUMP_FIELD(offhandHoldIsCancelable);
			WEAPON_DUMP_FIELD(doNotAllowAttachmentsToOverrideSpread);
			data["stowTag"] = convertToString(asset->stowTag);
			WEAPON_DUMP_ASSET(stowOffsetModel);

			return data;
		}

		Json dump_complete(WeaponCompleteDef* asset,
			const std::function<const char* (uint16_t)>& convertToString)
		{
			Json data;
			data["baseAsset"] = asset->szInternalName;
			data["weapDef"] = dump_weapondef(asset->weapDef, convertToString);

			for (int i = 0; i < 32; i++)
			{
				if (asset->hideTags && asset->hideTags[i])
				{
					data["hideTags"][i] = convertToString(asset->hideTags[i]);
				}
				else
				{
					data["hideTags"][i] = "";
				}
			}

			for (int i = 0; i < 6; i++)
			{
				if (asset->scopes && asset->scopes[i])
				{
					data["scopes"][i] = asset->scopes[i]->szInternalName;
				}
				else
				{
					data["scopes"][i] = "";
				}

				if (i >= 4) continue;

				if (asset->others && asset->others[i])
				{
					data["others"][i] = asset->others[i]->szInternalName;
				}
				else
				{
					data["others"][i] = "";
				}

				if (i >= 3) continue;

				if (asset->underBarrels && asset->underBarrels[i])
				{
					data["underBarrels"][i] = asset->underBarrels[i]->szInternalName;
				}
				else
				{
					data["underBarrels"][i] = "";
				}
			}

			for (int i = 0; i < 42; i++)
			{
				if (asset->szXAnims && asset->szXAnims[i])
				{
					data["szXAnims"][i] = asset->szXAnims[i];
				}
				else
				{
					data["szXAnims"][i] = "";
				}
			}

			for (int i = 0; i < asset->numAnimOverrides; i++)
			{
				data["animOverrides"][i]["altmodeAnim"] = (asset->animOverrides[i].altmodeAnim)
					? asset->animOverrides[i].altmodeAnim
					: "";
				data["animOverrides"][i]["overrideAnim"] = (asset->animOverrides[i].overrideAnim)
					? asset->animOverrides[i].overrideAnim
					: "";
				data["animOverrides"][i]["attachment1"] = asset->animOverrides[i].attachment1;
				data["animOverrides"][i]["attachment2"] = asset->animOverrides[i].attachment2;
				data["animOverrides"][i]["altTime"] = asset->animOverrides[i].altTime;
				data["animOverrides"][i]["animTime"] = asset->animOverrides[i].animTime;
				data["animOverrides"][i]["animTreeType"] = asset->animOverrides[i].animTreeType;
			}

			for (int i = 0; i < asset->numSoundOverrides; i++)
			{
				data["soundOverrides"][i]["altmodeSound"] = (asset->soundOverrides[i].altmodeSound)
					? asset->soundOverrides[i].altmodeSound->name
					: "";
				data["soundOverrides"][i]["attachment1"] = asset->soundOverrides[i].attachment1;
				data["soundOverrides"][i]["attachment2"] = asset->soundOverrides[i].attachment2;
				data["soundOverrides"][i]["overrideSound"] = (asset->soundOverrides[i].overrideSound)
					? asset->soundOverrides[i].overrideSound->name
					: "";
				data["soundOverrides"][i]["soundType"] = asset->soundOverrides[i].soundType;
			}

			for (int i = 0; i < asset->numFXOverrides; i++)
			{
				data["fxOverrides"][i]["altmodeFX"] = (asset->fxOverrides[i].altmodeFX)
					? asset->fxOverrides[i].altmodeFX->name
					: "";
				data["fxOverrides"][i]["attachment1"] = asset->fxOverrides[i].attachment1;
				data["fxOverrides"][i]["attachment2"] = asset->fxOverrides[i].attachment2;
				data["fxOverrides"][i]["fxType"] = asset->fxOverrides[i].fxType;
				data["fxOverrides"][i]["overrideFX"] = (asset->fxOverrides[i].overrideFX)
					? asset->fxOverrides[i].overrideFX->name
					: "";
			}

			for (int i = 0; i < asset->numReloadStateTimerOverrides; i++)
			{
				data["reloadOverrides"][i]["attachment"] = asset->reloadOverrides[i].attachment;
				data["reloadOverrides"][i]["reloadAddTime"] = asset->reloadOverrides[i].reloadAddTime;
				data["reloadOverrides"][i]["reloadEmptyAddTime"] = asset->reloadOverrides[i].reloadEmptyAddTime;
			}

			for (int i = 0; i < asset->numNotetrackOverrides; i++)
			{
				data["notetrackOverrides"][i]["attachment"] = asset->notetrackOverrides[i].attachment;

				for (int j = 0; j < 24; j++)
				{
					data["notetrackOverrides"][i]["notetrackSoundMapKeys"][j] = convertToString(
						asset->notetrackOverrides[i].notetrackSoundMapKeys[j]);
					data["notetrackOverrides"][i]["notetrackSoundMapValues"][j] = convertToString(
						asset->notetrackOverrides[i].notetrackSoundMapValues[j]);
				}
			}

			WEAPON_DUMP_FIELD(szInternalName);
			WEAPON_DUMP_FIELD(szDisplayName);
			WEAPON_DUMP_FIELD(numAnimOverrides);
			WEAPON_DUMP_FIELD(numSoundOverrides);
			WEAPON_DUMP_FIELD(numFXOverrides);
			WEAPON_DUMP_FIELD(numReloadStateTimerOverrides);
			WEAPON_DUMP_FIELD(numNotetrackOverrides);
			WEAPON_DUMP_FIELD(fAdsZoomFov);
			WEAPON_DUMP_FIELD(iAdsTransInTime);
			WEAPON_DUMP_FIELD(iAdsTransOutTime);
			WEAPON_DUMP_FIELD(iClipSize);
			WEAPON_DUMP_FIELD(impactType);
			WEAPON_DUMP_FIELD(iFireTime);
			WEAPON_DUMP_FIELD(iFireTimeAkimbo);
			WEAPON_DUMP_FIELD(dpadIconRatio);
			WEAPON_DUMP_FIELD(penetrateMultiplier);
			WEAPON_DUMP_FIELD(fAdsViewKickCenterSpeed);
			WEAPON_DUMP_FIELD(fHipViewKickCenterSpeed);
			WEAPON_DUMP_FIELD(szAltWeaponName);
			WEAPON_DUMP_FIELD(altWeapon);
			WEAPON_DUMP_FIELD(iAltRaiseTime);
			WEAPON_DUMP_FIELD(iAltRaiseTimeAkimbo);
			WEAPON_DUMP_ASSET(killIcon);
			WEAPON_DUMP_ASSET(dpadIcon);
			WEAPON_DUMP_FIELD(fireAnimLength);
			WEAPON_DUMP_FIELD(fireAnimLengthAkimbo);
			WEAPON_DUMP_FIELD(iFirstRaiseTime);
			WEAPON_DUMP_FIELD(iFirstRaiseTimeAkimbo);
			WEAPON_DUMP_FIELD(ammoDropStockMax);
			WEAPON_DUMP_FIELD(adsDofStart);
			WEAPON_DUMP_FIELD(adsDofEnd);
			WEAPON_DUMP_FIELD_ARR(accuracyGraphKnotCount, 2);
			// float(*accuracyGraphKnots[2])[2]);
			WEAPON_DUMP_FIELD(motionTracker);
			WEAPON_DUMP_FIELD(enhanced);
			WEAPON_DUMP_FIELD(dpadIconShowsAmmo);

			return data;
		}

		void IWeaponDef::dump(WeaponCompleteDef* asset, const std::function<const char* (uint16_t)>& convertToString)
		{
			std::string path = "weapons/mp/"s + asset->szInternalName;
			std::string json = dump_complete(asset, convertToString).dump(4);

			auto file = FileSystem::FileOpen(path, "w"s);
			fwrite(json.data(), json.size(), 1, file);
			FileSystem::FileClose(file);
		}
	}
}
