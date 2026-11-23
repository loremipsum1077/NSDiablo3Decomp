// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Powers/PowerHelper.h
// Subset: declarations of the PowerHelper.cpp functions other files call.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "PowerAPI.h"
#include "Powers.h"

// _Z28PowerHelper_DoFirstTimeCostsii -- called from
// X/Server/Powers/Power_Enchantress.cpp:48 and :78 (mangled (int, int) -> BOOL).
BOOL PowerHelper_DoFirstTimeCosts(PowerID idPower, BOOL bFirstTimeCosts);  // PowerHelper.cpp
void PowerHelper_AttackPayloadInit_Standard(PowerID idPower, PowerAPIAttackPayload & tPayload);  // PowerHelper.cpp:257
// _Z52PowerHelper_ChooseTargetsInitAroundTargetFromPayloadiR21PowerAPIChooseTargets
void PowerHelper_ChooseTargetsInitAroundTargetFromPayload(PowerID idPower, PowerAPIChooseTargets & tTargets);  // PowerHelper.cpp
void PowerHelper_ChooseTargetsInitAroundSourceFromPayload(PowerID idPower, PowerAPIChooseTargets & tTargets);  // PowerHelper.cpp
void PowerHelper_DamageInit(PowerAPIDamage & tDamage, PowerID idPower, const ACDID idACDTarget);  // PowerHelper.cpp:488
void PowerHelper_StopProjectileInit(PowerID idPower, ACDID idACDProjectile, ACDID idACDIntersected, PowerAPIDestroyProjectile & tProjectile);  // PowerHelper.cpp:800
void PowerHelper_HitSoundInit(Power * ptPower);  // PowerHelper.cpp:1295
void PowerHelper_AnimationInit(PowerID idPower, PowerAPIAnimation & tAnimation);  // PowerHelper.cpp:1331
void PowerHelper_PopulateCommonAttributes(const PowerID idPower, AttribGroupID idFastAttrib);  // PowerHelper.cpp:1547
void PowerHelper_UpdateTargetStandard(const PowerID idPower, const UpdateTargetData * tUpdateTargetData);  // PowerHelper.cpp:1578
BOOL PowerHelper_SetDefaultContactAnimations(PowerID idPower, uint32 dwAnimationTag);  // PowerHelper.cpp:1690
void PowerHelper_ExplosionInit(PowerID idPower, const WorldPlace & placeSource, PowerAPIExplosion & tExplosion);  // PowerHelper.cpp:1259
struct NavPlace;
void PowerHelper_ProjectileInit(PowerID idPower, ACDID idACDSource, const NavPlace & tNavPlace, const Target & tTarget, const SpellEffect eSpellEffect, PowerAPICreateProjectile & tProjectile);  // PowerHelper.cpp:728
BOOL PowerHelper_SummonActorInit(PowerID idPower, SNO snoActor, ACDID idACDCorpse, PowerAPISummonActor & tSummonInfo, BOOL fSearchForSpot);  // PowerHelper.cpp:373
void PowerHelper_ChooseTargetsInitFromPayload(PowerID idPower, const WorldPlace & placeSource, const Target & tTarget, PowerAPIChooseTargets & tTargetInfo);  // PowerHelper.cpp:1056
void PowerHelper_AddCooldown(PowerID idPower);  // PowerHelper.cpp:1395
void PowerHelper_AddCooldown(ActorCommonData & tACD, SNO snoPower, uint32 uCooldownTicks, BOOL bReapplyPassivesWhenFinished);  // PowerHelper.cpp:1498
void PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(const ActorCommonData * ptACDPlayer, const SNO snoPower, WorldPlace & wpTarget, const BOOL bFromPlayersFeet);  // PowerHelper.cpp:1845
void PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(const ActorCommonData * ptACDPlayer, const PowerDefinition * pdefPower, WorldPlace & wpTarget, const BOOL bFromPlayersFeet);  // PowerHelper.cpp:1860
void PowerHelper_AttackPayloadInit_None(PowerID idPower, PowerAPIAttackPayload & tPayload);  // PowerHelper.cpp:247
void PowerHelper_ChooseTargetsInit(PowerID idPower, PowerAPIChooseTargets & tTargetInfo);  // PowerHelper.cpp:847
void PowerHelper_ChooseTargetsInitGeometrySphere(const WorldPlace & placeCenter, WorldDistance wdRadius, ChooseTargetGeometry & tGeometry);  // PowerHelper.cpp:893
void PowerHelper_ChooseTargetsInitGeometrySingleTarget(const WorldPlace & placeSource, const Target & tTarget, const WorldDistance wdSearchRadius, ChooseTargetGeometry & tGeometry);  // PowerHelper.cpp:858
void PowerHelper_ChooseTargetsInitGeometryMelee(const ACDID idACDSource, const Target & tTarget, int32 nMeleeRange, ChooseTargetGeometry & tGeometry);  // PowerHelper.cpp:876
void PowerHelper_ChooseTargetsInitGeometryPie(const WorldPlace & placeCenter, const Target & tTarget, WorldDistance wdRadius, WorldDistance wdHeight, int32 nDegrees, ChooseTargetGeometry & tGeometry);  // PowerHelper.cpp:938
void PowerHelper_ChooseTargetsInitGeometryCylinder(const WorldPlace & placeCenter, const float flRadius, const float flHeight, ChooseTargetGeometry & tGeometry);  // PowerHelper.cpp:965
void PowerHelper_ChooseTargetsInitGeometryCapsule(const WorldPlace & placeSource, const WorldPlace & placeTarget, WorldDistance wdRadius, float flBonusHeight, ChooseTargetGeometry & tGeometry);  // PowerHelper.cpp:982
void PowerHelper_ChooseTargetsInitGeometryBox(const WorldPlace & placeCenter, const Vector3D & vDir, WorldDistance wdLength, WorldDistance wdWidth, ChooseTargetGeometry & tGeometry);  // PowerHelper.cpp:1027
void PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(PowerID idPower, const Target & tTargetBeingShotAt, PowerAPIChooseTargets & tTargetInfo);  // PowerHelper.cpp:1246
struct NavPlace;
void PowerHelperGetNavPlaceFromPowerSource(PowerID idPower, NavPlace & tNavPlace);  // PowerHelper.cpp:1630
void PowerHelper_GetContactOffsetForPower(const PowerID idPower, const int32 nContactFrame, Vector3D & wpContactOffset, BOOL bWantWorldPosition);  // PowerHelper.cpp:1722

void PowerHelper_DamageAdd(PowerAPIDamage & tDamageInfo, FastAttribKey tKey, float flAmount, int32 nDuration);

void PowerHelper_ProjectileInitForScriptedStandardProjectile(PowerID idPower, ACDID idACDSource, const NavPlace & tNavPlace, const Target & tTarget, const SpellEffect eSpellEffect, float flProjectileSpeed, PowerAPICreateProjectile & tProjectile, float flDistanceToTravel, BOOL bHugGround, BOOL bDontPerformMarbleRollToCaster, BOOL bUseProjectileStaticFlagsForSpawn, BOOL bNavReflect, BOOL bIgnoreMonsterCollision);  // PowerHelper.cpp:576

void PowerHelper_ProjectileInitForScriptedLobbedProjectile(PowerID idPower, ACDID idACDSource, const NavPlace & tNavPlace, const Target & tTarget, const SpellEffect eSpellEffect, float flGravity, float flHeightAboveSource, float flUnitBounceSpeed, float flLifetimeInSecs, ProjectileHitType eHitType, BOOL bDontDestroyOnImpact, BOOL bDontPerformMarbleRollToCaster, BOOL bUseProjectileStaticFlagsForSpawn, PowerAPICreateProjectile & tProjectile);  // PowerHelper.cpp:657
