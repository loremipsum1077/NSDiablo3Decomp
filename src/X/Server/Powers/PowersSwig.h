// Adopted from the DWARF reference; declarations below come from the owning source file.
// Original file: X/Server/Powers/PowersSwig.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 BuffStackType;

typedef int32 PetType;

typedef int32 MonsterFormulaIndex;




// Lua binding native declarations, recovered from the owning .cpp DWARF.
#include "../../Sigma/Unicode.h"
#include "../../Common/GameSwig.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/Powers/PowerAPISwig.h"
#include "../LuaScript/ScriptUtil.h"

int32 PowerDoHealthCostsForTargets(int32 nNumTargets);  // PowersSwig.cpp:479
int32 PowerGetLevel();  // PowersSwig.cpp:487
int32 PowerGetCasterLevel();  // PowersSwig.cpp:495
ACDID PowerGetOwner();  // PowersSwig.cpp:509
VACDID PowerGetAlternateSourceActor();  // PowersSwig.cpp:521
SNO PowerGetSNO();  // PowersSwig.cpp:529
int32 PowerGetID();  // PowersSwig.cpp:565
RuneType PowerGetRuneTypeFromID(int32 idPower);  // PowersSwig.cpp:573
float PowerGetProcScalarFromID(int32 idPower);  // PowersSwig.cpp:601
BOOL PowerInstanceIsValid(int32 idPower);  // PowersSwig.cpp:615
float PowerGetCooldown(int32 idPower);  // PowersSwig.cpp:623
BOOL PowerIsAssigned(SNO snoPower, ACDID idACDPlayer);  // PowersSwig.cpp:636
void PowerSetTargetFromCursor(int32 nCursorID);  // PowersSwig.cpp:13858
float PowerGetFormulaValue(int32 nFormulaIndex);  // PowersSwig.cpp:686
void ActorPowerForceInterruptPrimary(ACDID idActor);  // PowersSwig.cpp:704
void PowerForceInterrupt();  // PowersSwig.cpp:719
int32 PowerSNOFromName(LPCSTR szPowerName);  // PowersSwig.cpp:778
float PowerGetOwnerWeaponRange();  // PowersSwig.cpp:790
float PowerGetActorWeaponRange(ACDID idActor);  // PowersSwig.cpp:807
void PowerRemoveNegativeBuffs(ACDID idActor);  // PowersSwig.cpp:12920
void PowerRemoveNegativeBuff(ACDID idActor);  // PowersSwig.cpp:12936
void PowerProxyActorSetCanDeactivate(ACDID idActor, BOOL bValue);  // PowersSwig.cpp:12977
BOOL PowerBrainStanceStartPowerAtCursor(ACDID idACD, int32 nCursorID, LPCSTR szPowerName);  // PowersSwig.cpp:13181
BOOL PowerStartPassive(ACDID idOwner, LPCSTR szPowerName);  // PowersSwig.cpp:13248
void PowerActorKill(ACDID idTarget, ACDID idKiller);  // PowersSwig.cpp:13297
void PowerConsumeCorpse(ACDID idCorpse, BOOL bFakeCorpse);  // PowersSwig.cpp:13319
void PowerProjectileKill(ACDID idProjectile);  // PowersSwig.cpp:13596
void PowerSwitchDualWieldHand();  // PowersSwig.cpp:13609
float PowerActorGetCritChance(ACDID idActor);  // PowersSwig.cpp:13636
BOOL PowerActorDoesNotHaveHeadHardpoint(ACDID idActor);  // PowersSwig.cpp:13673
float PowerGetVerticalDistanceOfContactOffset(int32 nContactFrame);  // PowersSwig.cpp:1714
void PowerPreventSkillOverridePowerCastErrors(LPCSTR szHotbarSkill1, LPCSTR szHotbarSkill2, LPCSTR szHotbarSkill3, LPCSTR szHotbarSkill4, LPCSTR szHotbarSkill5, LPCSTR szLMBSkill, LPCSTR szRMBSkill);  // PowersSwig.cpp:9371
void PowerRetakeMeleeSnapshot();  // PowersSwig.cpp:9436
void PowerClearTargetCriteria();  // PowersSwig.cpp:4149
void PowerSetTargetsAllies(BOOL bEnable);  // PowersSwig.cpp:4167
void PowerSetTargetsEnemies(BOOL bEnable);  // PowersSwig.cpp:4178
void PowerSetTargetsPlayersOnly(BOOL bEnable);  // PowersSwig.cpp:4188
void PowerSetTargetsWreckables(BOOL bEnable);  // PowersSwig.cpp:4199
void PowerSetTargetsCorpses(BOOL bEnable);  // PowersSwig.cpp:4211
void PowerSetTargetsNecroCorpses(BOOL bEnable);  // PowersSwig.cpp:4229
void PowerSetIgnoresPets(BOOL bEnable);  // PowersSwig.cpp:4240
void PowerSetIgnoresPlayers(BOOL bEnable);  // PowersSwig.cpp:4250
void PowerSetIgnoresOwner(BOOL bEnable);  // PowersSwig.cpp:4260
void PowerTargetAreaSetIgnoreLOS(int32 nTargetAreaID, BOOL bIgnore);  // PowersSwig.cpp:1581
void PowerTargetAreaSetLOSTypeWalkableOnly(int32 nTargetAreaID);  // PowersSwig.cpp:1598
void PowerTargetAreaSetLOSTypeSpider(int32 nTargetAreaID);  // PowersSwig.cpp:1614
void PowerTargetAreaSetLOSTypeFlyer(int32 nTargetAreaID);  // PowersSwig.cpp:1629
void PowerTargetAreaSetLOSTypeGhost(int32 nTargetAreaID);  // PowersSwig.cpp:1645
void PowerTargetAreaSetUseMeleeSnapshot(int32 nTargetAreaID, BOOL bUseMeleeSnapshot);  // PowersSwig.cpp:1661
void PowerTargetAreaSetIgnoreStealthed(int32 nTargetAreaID, BOOL bIgnoreStealthed);  // PowersSwig.cpp:2201
void PowerTargetAreaSetUseProjectileLOS(int32 nTargetAreaID, BOOL bUseProjectileLOS);  // PowersSwig.cpp:2218
void PowerTargetAreaSetTargetsItemsOnly(int32 nTargetAreaID, BOOL bItemsOnly);  // PowersSwig.cpp:1678
void PowerTargetAreaSetTargetsItemsWithLabel(int32 nTargetAreaID, ItemLabel eItemLabel);  // PowersSwig.cpp:1696
void PowerTargetAreaDeleteLastCreated();  // PowersSwig.cpp:1557
int32 PowerCreateCursor();  // PowersSwig.cpp:4271
void PowerCursorDeleteLastCreated();  // PowersSwig.cpp:4293
void PowerCursorReset(int32 nCursorID);  // PowersSwig.cpp:4320
void PowerCursorSetToCursor(int32 nCursorID, int32 nCursorToCopyID);  // PowersSwig.cpp:4362
void PowerCursorSetToCursorHeight(int32 nCursorID, int32 nCursorToCopyID);  // PowersSwig.cpp:4381
BOOL PowerCursorCheckForNavMeshIntersectionBetweenCursors(int32 nCursorID, int32 nDestCursorID, int32 nIntersectionCursorID);  // PowersSwig.cpp:4519
void PowerCursorSetToContactOffset(int32 nCursorID, int32 nContactFrame);  // PowersSwig.cpp:4664
void PowerCursorSnapToGround(int32 nCursorID);  // PowersSwig.cpp:4698
void PowerCursorSetToProjectileCastTarget(int32 nCursorID);  // PowersSwig.cpp:4726
void PowerCursorSetToLobbedProjectileCastTarget(int32 nCursorID);  // PowersSwig.cpp:4822
BOOL PowerCursorIsInSameWorldAsActor(int32 nCursorID, ACDID idActor);  // PowersSwig.cpp:4642
void PowerCursorSetToPowerSource(int32 nCursorID);  // PowersSwig.cpp:4858
void PowerCursorRotateTowardsCursor(int32 nCursorID, int32 nTargetCursorID);  // PowersSwig.cpp:5230
void PowerPlayExplosionOnCursor(int32 nCursorID, LPCSTR szExplosionActorName);  // PowersSwig.cpp:5558
void PowerPayloadSetImpactEffectGroup(int32 nPayloadID, LPCSTR szEffectGroupName);  // PowersSwig.cpp:2767
void PowerPayloadSetImpactParticle(int32 nPayloadID, LPCSTR szParticleName);  // PowersSwig.cpp:2828
void PowerPayloadSetAttackedParticle(int32 nPayloadID, LPCSTR szParticleName);  // PowersSwig.cpp:2867
void PowerPayloadSetPetAOEDamageScalar(int32 nPayloadID);  // PowersSwig.cpp:2923
void PowerPayloadSetUseHighestElementalDamageBonus(int32 nPayloadID, BOOL bEnabled);  // PowersSwig.cpp:2942
void PowerSetTriggerAnim(ActorAnimationTag eAnimTag);  // PowersSwig.cpp:3017
void PowerFinishAnimation();  // PowersSwig.cpp:3043
BOOL PowerGetIsUsingAlternateAnim();  // PowersSwig.cpp:3086
void PowerSendCancelTargetLock(ACDID idActor);  // PowersSwig.cpp:3094
void PowerPayloadDeleteLastCreated();  // PowersSwig.cpp:3198
float PowerPayloadGetAdditionalCritChance(int32 nPayloadID);  // PowersSwig.cpp:3389
float PowerPayloadGetAdditionalCrushingBlowChance(int32 nPayloadID);  // PowersSwig.cpp:3420
void PowerPayloadSetCanCrit(int32 nPayloadID, BOOL bCanCrit);  // PowersSwig.cpp:3484
void PowerPayloadSetForceCriticalDeath(int32 nPayloadID, BOOL bForceCriticalDeath);  // PowersSwig.cpp:3501
void PowerPayloadSetAlwaysHits(int32 nPayloadID, BOOL bAlwaysHits);  // PowersSwig.cpp:3518
void PowerPayloadSetAlwaysCauseGetHit(int32 nPayloadID, BOOL bAlwaysCauseGetHit);  // PowersSwig.cpp:3534
void PowerPayloadSetNeverCauseGetHit(int32 nPayloadID, BOOL bNeverCauseGetHit);  // PowersSwig.cpp:3550
void PowerPayloadSetIgnoreDamageBonusAndReduction(int32 nPayloadID, BOOL bIgnore);  // PowersSwig.cpp:3566
void PowerPayloadSetTriggersHitProcs(int32 nPayloadID, BOOL bTriggerHitProcs);  // PowersSwig.cpp:3582
void PowerPayloadSetKnockbackStyle(int32 nPayloadID, KnockbackStyle eKnockbackStyle);  // PowersSwig.cpp:3812
void PowerPayloadSetKnockbackFromPlacePositionFromCursorPosition(int32 nPayloadID, int32 nCursorID);  // PowersSwig.cpp:3829
void PowerPayloadSetKnockbackDirectionFromCursorFacing(int32 nPayloadID, int32 nCursorID);  // PowersSwig.cpp:3848
BOOL PowerActorIsBeingKnockedBack(ACDID idActor);  // PowersSwig.cpp:3869
BOOL PowerActorIsSpecialWalking(ACDID idACD);  // PowersSwig.cpp:3892
void PowerPayloadSetDeliveryMechanismMelee(int32 nPayloadID);  // PowersSwig.cpp:3919
void PowerPayloadSetDeliveryMechanismRanged(int32 nPayloadID);  // PowersSwig.cpp:3935
void PowerPayloadSetDeliveryMechanismDOT(int32 nPayloadID);  // PowersSwig.cpp:3950
void PowerPayloadSetDeliveryMechanismThorns(int32 nPayloadID);  // PowersSwig.cpp:3965
void PowerPayloadSetDeliveryMechanismMagic(int32 nPayloadID);  // PowersSwig.cpp:3980
void PowerPayloadSetIgnoreCharmDamageScalar(int32 nPayloadID, BOOL bIgnore);  // PowersSwig.cpp:4105
void PowerPayloadDisableHitEffects(int32 nPayloadID);  // PowersSwig.cpp:4127
void PowerProjectileClearHitList(ACDID idACDProjectile);  // PowersSwig.cpp:6720
void PowerProjectileSetDetonateTime(ACDID idACDProjectile, int32 nDetonateTime);  // PowersSwig.cpp:6953
BOOL PowerProjectileIsReflected(ACDID idACDProjectile);  // PowersSwig.cpp:6992
ACDID PowerProjectileGetOwner(ACDID idACDProjectile);  // PowersSwig.cpp:6966
void PowerProjectileStopNow(ACDID idACDProjectile);  // PowersSwig.cpp:7004
void PowerProjectileSetStopOnImpact(ACDID idACDProjectile, BOOL bStopOnImpact);  // PowersSwig.cpp:7024
PetType PowerGetPetTypeFromItemSlot();  // PowersSwig.cpp:7973
int32 PowerGetSummonMaxConcurrent();  // PowersSwig.cpp:8076
int32 PowerGetSummonMaxTotal();  // PowersSwig.cpp:8091
void PowerBuffSetMaxInstances(int32 nBuffID, int32 nMaxInstances);  // PowersSwig.cpp:9456
void PowerBuffSetStackType(int32 nBuffID, BuffStackType eStackType, int32 nMaxStackCount);  // PowersSwig.cpp:9472
void PowerBuffSetTriggersHitProcs(int nBuffID, BOOL bEnable);  // PowersSwig.cpp:9496
void PowerBuffSetUndispellable(int nBuffID, BOOL bEnable);  // PowersSwig.cpp:9518
void PowerBuffSetSaveCooldown(int nBuffID, BOOL bEnable);  // PowersSwig.cpp:9533
void PowerBuffSetPetAOEDamageScalar(int32 nBuffID);  // PowersSwig.cpp:9606
void PowerBuffAddStun(int32 nBuffID, BOOL bBuffFailsIfStunImmune);  // PowersSwig.cpp:9649
void PowerBuffAddFreeze(int32 nBuffID, BOOL bBuffFailsIfFreezeImmune);  // PowersSwig.cpp:9666
void PowerBuffAddForceGrip(int32 nBuffID, BOOL bBuffFailsIfForceGripImmune);  // PowersSwig.cpp:9683
void PowerBuffAddBlind(int32 nBuffID, BOOL bBuffFailsIfBlindImmune);  // PowersSwig.cpp:9700
void PowerBuffAddFear(int32 nBuffID, BOOL bBuffFailsIfFearImmune);  // PowersSwig.cpp:9717
void PowerBuffAddImmobilize(int32 nBuffID, BOOL bBuffFailsIfImmobilizeImmune);  // PowersSwig.cpp:9849
void PowerBuffAddCharm(int32 nBuffID, BOOL bBuffFailsIfCharmImmune);  // PowersSwig.cpp:9734
void PowerBuffAddForcedTarget(int32 nBuffID, ACDID idTarget);  // PowersSwig.cpp:9776
void PowerBuffSetChanneled(int32 nBuffID);  // PowersSwig.cpp:9866
void PowerBuffSetUsesLockout(int32 nBuffID);  // PowersSwig.cpp:9881
int32 PowerBuffCallbackGetStoredData();  // PowersSwig.cpp:10481
float PowerBuffCallbackGetStoredDataFloat();  // PowersSwig.cpp:10504
void PowerBuffRemoveDuringCallback();  // PowersSwig.cpp:8896
BOOL PowerBuffIsOnActorFromActor(LPCSTR szPowerName, int32 nBuffID, ACDID idActor, ACDID idActorSource);  // PowersSwig.cpp:9050
void PowerBuffAddCCImmunity(int32 nBuffID, bool bValue);  // PowersSwig.cpp:9193
void PowerBuffAddLookSwitch(int32 nBuffID, LPCSTR szLookName);  // PowersSwig.cpp:9219
void PowerBuffSetSkillOverrides(int32 nBuffID, LPCSTR szHotbarSkill1, LPCSTR szHotbarSkill2, LPCSTR szHotbarSkill3, LPCSTR szHotbarSkill4, LPCSTR, LPCSTR szLMBSkill, LPCSTR szRMBSkill);  // PowersSwig.cpp:9310
void PowerBuffEnableOnGetHitEffect(int32 nBuffID);  // PowersSwig.cpp:10773
void PowerBuffEnableOnAttackEffect(int32 nBuffID);  // PowersSwig.cpp:10788
void PowerBuffEnableOnDodgeEffect(int32 nBuffID);  // PowersSwig.cpp:10803
void PowerBuffEnableOnCostEffect(int32 nBuffID);  // PowersSwig.cpp:10818
void PowerBuffEnableOnHealFromSkillEffect(int32 nBuffID);  // PowersSwig.cpp:10833
void PowerBuffEnableOnKillEffect(int32 nBuffID);  // PowersSwig.cpp:10848
void PowerBuffEnableOnAssistEffect(int32 nBuffID);  // PowersSwig.cpp:10864
void PowerBuffEnableOnItemPickupEffect(int32 nBuffID);  // PowersSwig.cpp:10879
void PowerBuffEnableOnExpirationEffect(int32 nBuffID);  // PowersSwig.cpp:10913
void PowerBuffEnableOnRemovalEffect(int32 nBuffID);  // PowersSwig.cpp:10928
void PowerBuffEnableOnDeathEffect(int32 nBuffID);  // PowersSwig.cpp:10943
void PowerBuffEnableOnCalloutEventEffect(int32 nBuffID);  // PowersSwig.cpp:10958
void PowerBuffEnableOnGizmoOperatedEffect(int32 nBuffID);  // PowersSwig.cpp:10973
void PowerBuffEnableOnWreckableDestroyedEffect(int32 nBuffID);  // PowersSwig.cpp:10988
void PowerBuffEnableOnTeleportedEffect(int32 nBuffID);  // PowersSwig.cpp:11003
void PowerBuffEnableOnPowerDamageInitEffect(int32 nBuffID);  // PowersSwig.cpp:11018
void PowerBuffEnableOnLootDropEffect(int32 nBuffID);  // PowersSwig.cpp:11033
void PowerBuffEnableOnItemSoldEffect(int32 nBuffID);  // PowersSwig.cpp:11050
void PowerBuffEnableOnLevelEffect(int32 nBuffID);  // PowersSwig.cpp:11066
void PowerBuffEnableOnCorpseConsumedEffect(int32 nBuffID);  // PowersSwig.cpp:11082
void PowerBuffEnableOnPetDismissEffect(int32 nBuffID);  // PowersSwig.cpp:11098
void PowerBuffEnableOnProjectileCreateEffect(int32 nBuffID);  // PowersSwig.cpp:11171
void PowerBuffEnableOnKnockbackLandEffect(int32 nBuffID);  // PowersSwig.cpp:11186
void PowerBuffEnableOnPetAddEffect(int32 nBuffID);  // PowersSwig.cpp:11201
void PowerBuffEnableOnEquipmentChangeEffect(int32 nBuffID);  // PowersSwig.cpp:11248
void PowerBuffEnableOnResourceGainEffect(int32 nBuffID);  // PowersSwig.cpp:11263
void PowerBuffEnableOnFirstHitAttemptEffect(int32 nBuffID);  // PowersSwig.cpp:11278
void PowerBuffEnableOnMessageEffect(int32 nBuffID);  // PowersSwig.cpp:11293
void PowerBuffEnablePersistAfterDeath(int32 nBuffID, BOOL bEnable);  // PowersSwig.cpp:11308
void PowerBuffSetRemoveOnRespec(int32 nBuffID, BOOL bEnable);  // PowersSwig.cpp:11324
void PowerBuffEnableOnSkillChangeEffect(int32 nBuffID);  // PowersSwig.cpp:11341
void PowerBuffSetRemoveOnEnteringGreaterRift(int32 nBuffID, BOOL bEnable);  // PowersSwig.cpp:11372
void PowerBuffSetRemoveOnPassiveApplication(int32 nBuffID, BOOL bEnable);  // PowersSwig.cpp:11356
void PowerBuffSetKeepForProfileSnapshot(int32 nBuffID, BOOL bEnable);  // PowersSwig.cpp:11388
void PowerBuffAddAIStateMerge(int32 nBuffID, LPCSTR szAIStateSNOToMerge);  // PowersSwig.cpp:11404
float PowerBuffDamageShieldGetIncomingDamageTotal();  // PowersSwig.cpp:11464
float PowerBuffDamageShieldGetRemainingAmount(ACDID idActor);  // PowersSwig.cpp:11764
BOOL PowerBuffAppliedDamageIsMelee();  // PowersSwig.cpp:11847
BOOL PowerBuffAppliedDamageIsRanged();  // PowersSwig.cpp:11866
BOOL PowerBuffAppliedDamageIsThorns();  // PowersSwig.cpp:11885
BOOL PowerBuffAppliedDamageIsDOT();  // PowersSwig.cpp:11904
BOOL PowerBuffAppliedDamageIsMagic();  // PowersSwig.cpp:11923
BOOL PowerBuffAppliedDamageIsCrit();  // PowersSwig.cpp:11942
BOOL PowerBuffAppliedDamageIsCrushingBlow();  // PowersSwig.cpp:11961
BOOL PowerBuffAppliedDamageIsPhysical();  // PowersSwig.cpp:11980
BOOL PowerBuffAppliedDamageIsKnockback();  // PowersSwig.cpp:11999
BOOL PowerBuffAppliedDamageIsChill();  // PowersSwig.cpp:12018
BOOL PowerBuffAppliedDamageWasBlocked();  // PowersSwig.cpp:12042
float PowerBuffAppliedDamageGetBlockedAmount();  // PowersSwig.cpp:12061
SNO PowerBuffAppliedDamageGetPower();  // PowersSwig.cpp:12080
int32 PowerBuffAppliedDamageGetPowerID();  // PowersSwig.cpp:12099
float PowerBuffAppliedDamageGetTotal();  // PowersSwig.cpp:12148
void PowerBuffAppliedDamageSetDamagingPowersOneTimeEventFlag(BOOL bValue);  // PowersSwig.cpp:12345
BOOL PowerBuffAppliedDamageGetDamagingPowersOneTimeEventFlag();  // PowersSwig.cpp:12374
float PowerBuffAppliedDamageGetProcCoefficient();  // PowersSwig.cpp:12465
float PowerBuffAppliedDamageGetAttackSpeed();  // PowersSwig.cpp:12493
ACDID PowerBuffAppliedDamageGetPowerActorTarget();  // PowersSwig.cpp:12523
void PowerBuffAppliedDamageCursorSetToPowerTarget(int32 nCursorIndex);  // PowersSwig.cpp:12552
void PowerBuffAppliedDamageRetakeMeleeSnapshot();  // PowersSwig.cpp:12587
void PowerBuffAppliedDamageSetForceCriticalDeath(BOOL bForceCritDeath);  // PowersSwig.cpp:12623
void PowerBuffAppliedDamageForceCrit(BOOL bForceCrit);  // PowersSwig.cpp:12645
void PowerBuffSetAsAura(int32 nBuffID, int32 nPriority);  // PowersSwig.cpp:12710
void PowerBuffAuraSetEffect(int32 nBuffID, LPCSTR szEffect);  // PowersSwig.cpp:12732
BOOL PowerBuffInternalCooldownIsReady(int32 nBuffID, ACDID idActor);  // PowersSwig.cpp:12762
void PowerBuffInternalCooldownTrigger(int32 nBuffID, ACDID idActor);  // PowersSwig.cpp:12794
void PowerBuffInternalCooldownReset(int32 nBuffID, ACDID idActor);  // PowersSwig.cpp:12821
void PowerBuffSetExclusiveType(int32 nBuffID, BuffExclusiveType eExclusiveType);  // PowersSwig.cpp:12848
void PowerBuffSetDoNotCopy(int32 nBuffID);  // PowersSwig.cpp:12879
int32 PowerGetRuneLevelForRuneType(RuneType eRuneType);  // PowersSwig.cpp:13759
void PowerSwapActorPlaces(ACDID idACDFirst, ACDID idACDSecond);  // PowersSwig.cpp:11435
double PowerCursorDistanceBetweenCursors(int32 nCursorIDFrom, int32 nCursorIDTo);  // PowersSwig.cpp:14034
double PowerCursorDistanceSquaredBetweenCursors(int32 nCursorIDFrom, int32 nCursorIDTo);  // PowersSwig.cpp:14057
double PowerCursorDistanceToProjectedPointAlongLine(int32 nCursorIDLineOrigin, int32 nCursorIDLineEnd, int32 nCursorIDTarget);  // PowersSwig.cpp:14080
double PowerCursorDistanceAlongCursorFacing(int32 nCursorIDFacing, int32 nCursorIDCheck);  // PowersSwig.cpp:14132
double PowerGetAttackDistanceToTarget(ACDID idACDTarget);  // PowersSwig.cpp:14234
float PowerCursorAngleToCursor(int32 nCursorSource, int32 nCursorDest);  // PowersSwig.cpp:14695
float PowerCursorAngleToActor(int32 nCursorSource, ACDID idActorDest);  // PowersSwig.cpp:14714
float PowerActorAngleToCursor(ACDID idActorSource, int32 nCursorDest);  // PowersSwig.cpp:14738
float PowerActorAngleToActor(ACDID idActorSource, ACDID idActorDest);  // PowersSwig.cpp:14762
float PowerCursorGetYaw(int32 nCursor);  // PowersSwig.cpp:14778
float PowerCursorGetHeight(int32 nCursor);  // PowersSwig.cpp:14794
int32 PowerGetItemLevel();  // PowersSwig.cpp:8056
BOOL PowerItemLabelIsEquipped(ItemLabel eItemLabel);  // PowersSwig.cpp:14846
BOOL PowerItemHasLabel(ACDID idActorItem, ItemLabel eItemLabel);  // PowersSwig.cpp:14878
ACDID PowerGetMainHandWeapon(ACDID idACDPlayer);  // PowersSwig.cpp:14898
ACDID PowerGetOffHandWeapon(ACDID idACDPlayer);  // PowersSwig.cpp:14917
BOOL PowerItemIsEquipped(ACDID idACDItem);  // PowersSwig.cpp:14936
void PowerGoldDropOnMonster(ACDID idAttacker, ACDID idTarget, int32 nGoldToDrop);  // PowersSwig.cpp:14949
void PowerHostItemApplyTemporaryTransmog(LPCSTR szTransmogItemName);  // PowersSwig.cpp:15203
void PowerHostItemRemoveTemporaryTransmog();  // PowersSwig.cpp:15229
void PowerActorAttachActorToActor(ACDID idACDToAttach, ACDID idACDAttachTo);  // PowersSwig.cpp:15800
void PowerActorDetachActorFromActor(ACDID idACDToAttach, ACDID idACDAttachTo);  // PowersSwig.cpp:15816
void PowerSetStoredInt(LPCSTR szName, int32 nValue);  // PowersSwig.cpp:15249
int32 PowerGetStoredInt(LPCSTR szName);  // PowersSwig.cpp:15259
float PowerGetStoredFloat(LPCSTR szName);  // PowersSwig.cpp:15278
void PowerRemoveTrackedActor(ACDID idACD);  // PowersSwig.cpp:15601
BOOL PowerIsTrackedActor(ACDID idACD);  // PowersSwig.cpp:15617
void PowerClearTrackedActors();  // PowersSwig.cpp:15664
void PowerIncrementLockGlobalsCounter();  // PowersSwig.cpp:15791
void PowerUpdateSnapshotAttributes();  // PowersSwig.cpp:15716
void PowerActorAddCharges(ACDID idACDActor, SNO snoPower, int32 nAmount);  // PowersSwig.cpp:15302
float PowerActorGetHitPointsMax(ACDID idACD);  // PowersSwig.cpp:8541
float PowerActorGetHitPointsPercent(ACDID idActor);  // PowersSwig.cpp:15485
float PowerActorGetHitPoints(ACDID idActor);  // PowersSwig.cpp:15498
float PowerGetMonsterFormulaValue(MonsterFormulaIndex eFormulaIndex);  // PowersSwig.cpp:15948
float PowerGetLookupTableValue(LPCSTR szTable, int32 nValueIndex);  // PowersSwig.cpp:15985
void PowerCursorSetToLeftStick(int32 nCursorIndex);  // PowersSwig.cpp:16024
void PowerCursorSetToRightStick(int32 nCursorIndex);  // PowersSwig.cpp:16057
BOOL PowerCanDoExpensiveProc(ACDID idACDTarget);  // PowersSwig.cpp:16089
void PowerIncExpensiveProcCounter(ACDID idACDTarget);  // PowersSwig.cpp:16106
void PowerPreventEvade(ACDID idACDTarget, BOOL bBlock);  // PowersSwig.cpp:16143
int32 PowerGetNumberOfCursedEnemies(ACDID idPlayer);  // PowersSwig.cpp:16264
int32 PowerGetNumberOfCursedEnemiesLeech(ACDID idPlayer);  // PowersSwig.cpp:16296
int32 PowerGetNumberOfCursedEnemiesDecrepify(ACDID idPlayer);  // PowersSwig.cpp:16328
int32 PowerGetNumberOfCursedEnemiesFrailty(ACDID idPlayer);  // PowersSwig.cpp:16360
void PowerSetTargetFromActor(VACDID idActor);  // PowersSwig.cpp:13843
void PowerRemoveCooldown(VACDID idActor, LPCSTR szPowerName);  // PowersSwig.cpp:1887
float PowerGetRemainingCooldown(VACDID idActor, LPCSTR szPowerName);  // PowersSwig.cpp:2124
void PowerResetAllCooldowns(VACDID idActor);  // PowersSwig.cpp:2175
BOOL PowerCanTarget(VACDID idACDTarget);  // PowersSwig.cpp:2289
BOOL PowerActorIsFullyContainedInTargetAreaAOE(VACDID idACDTarget, int32 nTargetAreaID);  // PowersSwig.cpp:2717
void PowerCursorRotateTowardsActor(int32 nCursorID, VACDID idActor);  // PowersSwig.cpp:5196
void PowerPlayExplosionOnActor(VACDID idActor, LPCSTR szExplosionActorName);  // PowersSwig.cpp:5509
void PowerPayloadSetFloatingCombatTextSourceOverride(int32 nPayloadID, VACDID idACDDest);  // PowersSwig.cpp:4054
void PowerPlayProjectileExplosion(VACDID idActorProjectile, ACDID idActorHit, LPCSTR szExplosionActorName, LPCSTR szGroundExplosionActorName);  // PowersSwig.cpp:6520
ACDID PowerProjectileGetImpactTarget(VACDID idActorProjectile, ACDID idActorHit);  // PowersSwig.cpp:6613
void PowerProjectileAddActorToHitList(VACDID idACDProjectile, VACDID idACDActor);  // PowersSwig.cpp:6740
BOOL PowerProjectileActorIsOnHitList(VACDID idACDProjectile, VACDID idACDActor);  // PowersSwig.cpp:6776
void PowerProjectilePlayAnim(VACDID idProjectile, ActorAnimationTag eAnimTag);  // PowersSwig.cpp:7045
void PowerProjectileSetIdleAnim(VACDID idProjectile, ActorAnimationTag eAnimTag);  // PowersSwig.cpp:7064
ACDID PowerActorSummonHireling(VACDID idPlayer, HirelingClass eHirelingClass);  // PowersSwig.cpp:7820
ACDID PowerCloneMonsterOnCursor(VACDID idACDSource, int32 nCursorID);  // PowersSwig.cpp:14261
void PowerActorCopySkillsAndRunes(ACDID idACDSource, ACDID idACDTarget);  // PowersSwig.cpp:14623
void PowerPlayerSetPetCanBeDismissed(VACDID idACDPet, BOOL bCanBeDismissed);  // PowersSwig.cpp:8360
int32 PowerBuffGetNumStacks(int32 nBuffID, VACDID idACD);  // PowersSwig.cpp:10700
double PowerActorDistanceToCursor(VACDID idACDActorFrom, int32 nCursorIDTo);  // PowersSwig.cpp:14152
double PowerActorDistanceSquaredToCursor(VACDID idACDActorFrom, int32 nCursorIDTo);  // PowersSwig.cpp:14180
double PowerActorVerticalDistanceToActor(VACDID idACDActorFrom, VACDID idACDActorTo);  // PowersSwig.cpp:14208
double PowerActorDotProductBetweenCursors(VACDID idActor, int32 nCursorFirstID, int32 nCursorSecondID);  // PowersSwig.cpp:5381
double PowerActorCrossProductBetweenCursors(VACDID idActor, int32 nCursorFirstID, int32 nCursorSecondID);  // PowersSwig.cpp:5427
BOOL PowerPlayerIsDualWielding(VACDID idPlayer);  // PowersSwig.cpp:14997
ACDID PowerFindNearestTrackedActorToActor(ACDID idACD);  // PowersSwig.cpp:15673
float PowerGetRemainingRechargeTime(VACDID idActor, LPCSTR szPowerName);  // PowersSwig.cpp:16163
BOOL PowerActorPlayResourceGainedEffectGroup(VACDID idActor, ResourceType eResourceType);  // PowersSwig.cpp:16427
void PowerActorSetLookSwitch(VACDID idActor, LPCSTR szLookSwitchName);  // PowersSwig.cpp:16461
BOOL PowerDoFirstTimeCosts(BOOL bWantCooldown);  // PowersSwig.cpp:460
BOOL PowerIsAutoCastInstance(int32 idPower);  // PowersSwig.cpp:652
ACDID PowerGetTargetActor(int32 idPower);  // PowersSwig.cpp:13832
int32 PowerGetComboLevel(int32 idPower);  // PowersSwig.cpp:13814
float PowerGetOwnerWeaponAverageDamage(const BOOL bWantBonuses);  // PowersSwig.cpp:823
float PowerGetActorWeaponAverageDamage(ACDID idActor, const BOOL bWantBonuses);  // PowersSwig.cpp:842
float PowerGetOwnerWeaponDPS(const BOOL bWantBonuses);  // PowersSwig.cpp:861
float PowerGetActorWeaponDPS(ACDID idActor, const BOOL bWantBonuses);  // PowersSwig.cpp:884
void PowerRemoveCCBuffs(ACDID idActor, BOOL bHardCCOnly);  // PowersSwig.cpp:12899
BOOL PowerStartPower(ACDID idOwner, ACDID idTarget, LPCSTR szPowerName, BOOL bFreeCast, BOOL bDoesntAnimate, RuneType eRuneType, BOOL bIgnoreRange, BOOL bForceCast, uint32 dwAnimTagOverride, BOOL bAllowHitProcs);  // PowersSwig.cpp:13005
BOOL PowerStartPowerOnCursor(ACDID idOwner, int32 nCursorID, LPCSTR szPowerName, BOOL bFreeCast, BOOL bDoesntAnimate, RuneType eRuneType, BOOL bIgnoreRange, BOOL bForceCast, BOOL bAllowHitProcs);  // PowersSwig.cpp:13027
void PowerSpawnCorpse(int32 nCursorID, ACDID idOwnerOverride, LPCSTR szReviveActorName, BOOL bQuiet);  // PowersSwig.cpp:13401
void PowerActorBurrowIn(VACDID idActor, BOOL bInvisibleDuring, WeaponClass eBurrowWeaponClass);  // PowersSwig.cpp:13567
void PowerActorBurrowOut(VACDID idActor, BOOL bSetUsedSpecialSpawn);  // PowersSwig.cpp:13582
float PowerActorGetMeleeDodgeChance(ACDID idActor, ACDID idTarget);  // PowersSwig.cpp:13650
BOOL PowerActorHasEnoughResourceForPower(ACDID idActor, SNO snoPower, int32 idPower);  // PowersSwig.cpp:2312
void PowerCursorPathToCursor(int32 nCursorID, int32 nDestCursorID, LPCSTR szPowerPassability);  // PowersSwig.cpp:4466
BOOL PowerCursorIsOnNavMesh(int32 nCursorID, BOOL bWalkableOnly);  // PowersSwig.cpp:4584
void PowerCursorSetToCastTarget(int32 nCursorID, int32 idPowerForTarget);  // PowersSwig.cpp:4336
void PowerCursorSetToActor(int32 nCursorID, VACDID idActor, BOOL bWantOrientation);  // PowersSwig.cpp:4610
BOOL PowerCursorSetToOpenLocationNearCursor(int32 nCursorID, int32 nTargetCursorID, LPCSTR szActorSNO);  // PowersSwig.cpp:4940
void PowerCursorGetAverageCursorPosition(int32 nCursorIDOut, int32 nCursorID1, int32 nCursorID2, int32 nCursorID3, int32 nCursorID4, int32 nCursorID5);  // PowersSwig.cpp:5472
void PowerPayloadAddAmbientDamageEffect(int32 nPayloadID, BOOL bCausesAmbientDamageEffects);  // PowersSwig.cpp:3744
BOOL PowerPayloadApplyToTarget(int32 nPayloadID, VACDID idACDTarget, VACDID idACDOverrideAttacker);  // PowersSwig.cpp:3996
int32 PowerPayloadApplyToTargets(int32 nPayloadID, int32 nTargetAreaID, VACDID idACDOverrideAttacker);  // PowersSwig.cpp:4021
ACDID PowerSummonActorOnCursor(LPCSTR szSummonActorName, int32 nCursorID, float flDuration, BOOL bUseExactLocation, int32 dwAnimTag, int32 nSummonLevel, BOOL bCopySummonerAttribs, BOOL bUpdateAttributesFromOwner, BOOL bNeverDeactivate, BOOL bUseActorStaticFlagsForSpawn, BOOL bTryToSpawnOnPlayerLocationUponFail, BOOL bNoSpawnAnimPower, ACDID idACDOwnerToUse);  // PowersSwig.cpp:7587
void PowerPlayerAddPet(VACDID idACDPet, PetType ePetType, int32 nMaxCount, VACDID idACDPlayer, BOOL bIgnoreFade, BOOL bDontKillWhenEnteringRift);  // PowersSwig.cpp:8265
void PowerPlayerRemovePet(VACDID idACDPet, VACDID idACDPlayer);  // PowersSwig.cpp:8321
void PowerRemoveBuffs(ACDID idActor, LPCSTR szPowerName, int32 nBuffIndex, BuffRemovalReason eRemovalReason, ACDID idActorSource);  // PowersSwig.cpp:8666
void PowerBuffAddTaunt(int32 nBuffID, ACDID idTauntTarget, BOOL bBuffFailsIfCharmImmune, BOOL bSuppressTauntEffects);  // PowersSwig.cpp:9754
void PowerBuffRefreshDuration(int32 nBuffID, VACDID idACD, BOOL bCasterAsSource, BOOL bOnlyFirstBuff);  // PowersSwig.cpp:10051
float PowerBuffGetDuration(int32 nBuffID, VACDID idACD, BOOL bCasterAsSource);  // PowersSwig.cpp:10113
float PowerBuffGetDurationByPowerName(LPCSTR szPowerName, int32 nBuffID, VACDID idACDActor, VACDID idACDSource);  // PowersSwig.cpp:10148
BOOL PowerBuffCheckForAuraApplication(int32 nBuffID, VACDID idActor, BOOL bCasterAsSource);  // PowersSwig.cpp:10202
void PowerBuffApplyToTarget(int32 nBuffID, VACDID idActor, BOOL bAllowInOnDoDamage);  // PowersSwig.cpp:8707
int32 PowerBuffApplyToTargets(int32 nBuffID, int32 nTargetAreaID, BOOL bAllowInOnDoDamage);  // PowersSwig.cpp:8733
void PowerBuffRemove(int32 nBuffID, VACDID idActor, ACDID idActorSource, BOOL bFromThisPowerInstanceOnly, BOOL bRemoveSingleStack, BuffRemovalReason eRemovalReason);  // PowersSwig.cpp:8797
void PowerBuffRemoveByBuffData(int32 nBuffID, int32 nBuffData, VACDID idActor, ACDID idActorSource, BOOL bFromThisPowerInstanceOnly, BOOL bRemoveSingleStack, BuffRemovalReason eRemovalReason);  // PowersSwig.cpp:8845
BOOL PowerBuffIsOnActorByBuffData(int32 nBuffID, int32 nBuffData, ACDID idActor, BOOL bIncludeDeleted);  // PowersSwig.cpp:9094
void PowerBuffAddWalkPassabilityOverride(int32 nBuffID, LPCSTR szPower);  // PowersSwig.cpp:9238
void PowerBuffAddPassabilityOverride(int32 nBuffID, LPCSTR szPower);  // PowersSwig.cpp:9274
void PowerBuffEnableOnDoDamageEffect(int32 nBuffID, BOOL bTriggerForPets);  // PowersSwig.cpp:10739
void PowerBuffEnableOnHitEffect(int32 nBuffID, BOOL bTriggerForPets);  // PowersSwig.cpp:10756
void PowerBuffEnableDamageShieldEffect(int32 nBuffID, int32 nPriority, BOOL bStaggerDamageEnabled);  // PowersSwig.cpp:10894
void PowerBuffEnableOnGetHitWithCrowdControlEffect(int32 nBuffID, BOOL bCallOnRefresh);  // PowersSwig.cpp:11216
void PowerBuffEnableOnHitWithCrowdControlEffect(int32 nBuffID, BOOL bCallOnRefresh);  // PowersSwig.cpp:11232
float PowerBuffDamageShieldAbsorbFromBuff(BOOL bWantAbsorbedMessage);  // PowersSwig.cpp:11587
float PowerBuffDamageShieldGetRemainingAmountOnBuff(int32 nBuffID, VACDID idActor);  // PowersSwig.cpp:11717
void PowerActorFaceCursor(VACDID idActor, int32 nCursorID, BOOL bSnapToFacing);  // PowersSwig.cpp:14004
void PowerHealthDropOnMonster(ACDID idDropper, LPCSTR szItemName);  // PowersSwig.cpp:15006
void PowerDropArcaneGlobeOnActor(ACDID idDropper, ACDID idLootee);  // PowersSwig.cpp:15055
void PowerDropLootRunProgressGlobeOnActor(ACDID idDropper, ACDID idLootee, BOOL bForGoblin);  // PowersSwig.cpp:15124
void PowerAddTrackedActor(ACDID idACD, BOOL bAddReference);  // PowersSwig.cpp:15536
void PowerAddTrackedActors(int32 nTargetAreaID, BOOL bAddReference);  // PowersSwig.cpp:15557
void PowerAddAffectedActor(ACDID idACDTarget, ACDID idACDSource);  // PowersSwig.cpp:15706
void PowerTriggerOnAttackCallbacks(ACDID idACD, ACDID idACDTarget);  // PowersSwig.cpp:16120
int32 PowerActorPlayEffectGroup(VACDID idActor, LPCSTR szEffectGroupName, BOOL bUseComplexEffect, ACDID idDestinationPlayer);  // PowersSwig.cpp:16416
int32 PowerActorPlayEffectGroupActorToActor(VACDID idActorFrom, VACDID idActorTo, LPCSTR szEffectGroupName, BOOL bUseComplexEffect);  // PowersSwig.cpp:16436
void PowerDebugDrawLine(int32, int32, uint32);  // PowersSwig.cpp:16594
void PowerDebugDrawText(int32, LPCSTR, uint32);  // PowersSwig.cpp:16597

#include "../../Common/Actors/ActorEffectSwig.h"

#include "../Actors/ActorEffect.h"
void PowerPayloadSetDeathReplacementType(int32 nPayloadID, DeathReplacementType eDeathReplacementType);  // PowersSwig.cpp:2805
void PowerPayloadConvertToDamageType(int32 nPayloadID, DamageType eDamageType);  // PowersSwig.cpp:3302
float PowerBuffAppliedDamageGetDamageTypeAmount(DamageType eDamageType);  // PowersSwig.cpp:12118
void PowerBuffAppliedDamageConvertToDamageType(DamageType eDamageType);  // PowersSwig.cpp:12280
void PowerOffsetTranslatePowerTargetPosition(float flOffset);  // PowersSwig.cpp:13703
void PowerReduceCooldown(VACDID idActor, LPCTSTR szPowerName, float flReduction);  // PowersSwig.cpp:1945
void PowerReduceCooldownByPercentage(VACDID idActor, LPCSTR szPowerName, float flReductionPct);  // PowersSwig.cpp:1997
void PowerReduceAllCooldowns(ACDID idActor, float flReduction);  // PowersSwig.cpp:2046
void PowerReduceAllCooldownsByPercentage(ACDID idActor, float flReductionPct);  // PowersSwig.cpp:2083
int32 PowerCreateTargetAreaMelee(float flSearchRadius);  // PowersSwig.cpp:934
int32 PowerCreateTargetAreaSingleTarget(float flSearchRadius);  // PowersSwig.cpp:997
int32 PowerCreateTargetAreaSingleTargetAroundActor(VACDID idActor, float flSearchRadius);  // PowersSwig.cpp:1008
int32 PowerCreateTargetAreaSingleTargetAroundCursor(int32 nCursorID, float flSearchRadius);  // PowersSwig.cpp:1028
int32 PowerCreateTargetAreaAOE(float flRadius);  // PowersSwig.cpp:1075
int32 PowerCreateTargetAreaAOEAroundActor(VACDID idActor, float flRadius);  // PowersSwig.cpp:1087
int32 PowerCreateTargetAreaAOEAroundCursor(int32 nCursorID, float flRadius);  // PowersSwig.cpp:1111
int32 PowerCreateTargetAreaBox(float flLength, float flWidth);  // PowersSwig.cpp:1513
int32 PowerCreateTargetAreaBoxAroundCursor(int32 nCursorID, float flLength, float flWidth);  // PowersSwig.cpp:1532
void PowerCursorRotate(int32 nCursorID, float flDegrees);  // PowersSwig.cpp:5177
void PowerCursorRotateAbsolute(int32 nCursorID, float flDegrees);  // PowersSwig.cpp:5262
void PowerCursorOffset(int32 nCursorID, float flDistance);  // PowersSwig.cpp:5280
void PowerCursorOffsetLaterally(int32 nCursorID, float flDistance);  // PowersSwig.cpp:5301
void PowerCursorOffsetVertically(int32 nCursorID, float flDistance);  // PowersSwig.cpp:5323
void PowerCursorOffsetFromContactOffset(int32 nCursorID, int32 nContactFrame, float flDistance);  // PowersSwig.cpp:5340
void PowerCursorRotateAroundOwner(int32 nCursorID, float flDegrees);  // PowersSwig.cpp:5013
void PowerCursorRotateAroundActor(int32 nCursorID, VACDID idActor, float flDegrees);  // PowersSwig.cpp:5041
void PowerCursorRotateAroundCursor(int32 nCursorID, int32 nCursorIDTarget, float flDegrees);  // PowersSwig.cpp:5069
void PowerCursorPinAtProjectileDistFromOwner(int32 nCursorID, float flMinDist, float flMaxDist);  // PowersSwig.cpp:5137
void PowerPayloadSetPetDamageScalar(int32 nPayloadID, float flScalar);  // PowersSwig.cpp:2905
void PowerPayloadSetAdditionalCritChance(int32 nPayloadID, float flCritChance);  // PowersSwig.cpp:3404
void PowerPayloadSetAdditionalCrushingBlowChance(int32 nPayloadID, float flCrushingBlowChance);  // PowersSwig.cpp:3435
void PowerPayloadSetCritDamageBonusPercent(int32 nPayloadID, float flCritBonusPercent);  // PowersSwig.cpp:3451
void PowerPayloadSetCritCap(int32 nPayloadID, float flCritCap);  // PowersSwig.cpp:3468
void PowerPayloadAddStun(int32 nPayloadID, float flStunChance, float flDuration, float flDurationDelta);  // PowersSwig.cpp:3600
void PowerPayloadAddFear(int32 nPayloadID, float flDuration, float flDurationDelta);  // PowersSwig.cpp:3621
void PowerPayloadAddRoot(int32 nPayloadID, float flDuration, float flDurationDelta);  // PowersSwig.cpp:3640
void PowerPayloadAddSlow(int32 nPayloadID, float flSlowAmount, float flDuration, float flDurationDelta);  // PowersSwig.cpp:3680
void PowerPayloadSetProcScalar(int32 nPayloadID, float flProcScalar);  // PowersSwig.cpp:4076
void PowerProjectileSetPierceChance(ACDID idACDProjectile, float flPierceChance);  // PowersSwig.cpp:6939
ACDID PowerCreateProxyActorOnCursor(LPCSTR szProxyActorName, int32 nCursorID, float flDuration, BOOL bUseExactLocation);  // PowersSwig.cpp:8106
void PowerActorSetDamageAttributes(ACDID idACD, DamageType eDamageType, float flDamageMin, float flDamageDelta);  // PowersSwig.cpp:8238
void PowerPlayerSetPetProcScalar(VACDID idACDPet, float flPetProcScalar);  // PowersSwig.cpp:8351
ACDID PowerPlayerReviveActorOnCursor(LPCSTR szReviveActorName, int32 nCursorID, float flDuration, BOOL bUseExactLocation, PetType ePetType, int32 nMaxCount);  // PowersSwig.cpp:8463
void PowerBuffSetPetDamageScalar(int32 nBuffID, float flScalar);  // PowersSwig.cpp:9588
void PowerBuffSetProcScalar(int32 nBuffID, float flProcScalar);  // PowersSwig.cpp:9625
void PowerBuffAddSlow(int32 nBuffID, float flSlowAmount, BOOL bBuffFailsIfSlowImmune);  // PowersSwig.cpp:9792
void PowerBuffAddChill(int32 nBuffID, float flChillMagnitude, BOOL bBuffFailsIfChillImmune);  // PowersSwig.cpp:9830
void PowerBuffAddAttackSpeedReduction(int32 nBuffID, float flAttackSpeedReductionAmount, BOOL bBuffFailsIfAttackSpeedReductionImmune);  // PowersSwig.cpp:9811
void PowerBuffAddAttributeModifier(int32 nBuffID, int32 eAttrib, int32 nParam, float flValue);  // PowersSwig.cpp:9145
void PowerBuffSetPeriodicRefreshRate(int32 nBuffID, float flRefreshInterval, ACDID idTarget);  // PowersSwig.cpp:9117
void PowerBuffEnablePeriodicEffect(int32 nBuffID, float flRefreshInterval, BOOL bHitsOnFirstTick);  // PowersSwig.cpp:10719
BOOL PowerBuffDamageShieldSetAmountOnBuff(int32 nBuffID, float flShieldAmount);  // PowersSwig.cpp:11698
float PowerBuffDamageShieldReduceDamageByPercent(float flPercentToReduce);  // PowersSwig.cpp:11792
void PowerBuffAppliedDamageSetDamageTypeAmount(DamageType eDamageType, float flAmount);  // PowersSwig.cpp:12232
void PowerBuffAppliedDamageModifyByPercent(float flPercent);  // PowersSwig.cpp:12254
BOOL PowerBuffAppliedDamageDoRollAgainstProcCoefficient(float flChance);  // PowersSwig.cpp:12410
void PowerBuffInternalCooldownSet(int32 nBuffID, float flDuration);  // PowersSwig.cpp:12741
BOOL PowerActorIsFacingCursor(int32 nCursor, ACDID idActor, float flDegrees);  // PowersSwig.cpp:14809
void PowerSetStoredFloat(LPCSTR szName, float flValue);  // PowersSwig.cpp:15268
void PowerInstanceSetDamagePercentBonus(int32 idPower, float flDamagePercentBonus);  // PowersSwig.cpp:15777
void PowerDamageActor(ACDID idACDActor, float flAmount);  // PowersSwig.cpp:15460
void PowerActorSetHitPointsMax(ACDID idACD, float flHitPointsMax);  // PowersSwig.cpp:8555
void PowerActorSetHitPointsPercent(ACDID idActor, float flPercent);  // PowersSwig.cpp:15511
void PowerActorSetHitPoints(ACDID idActor, float flHP);  // PowersSwig.cpp:15524
void PowerUpdateRechargeTimer(VACDID idPlayer, SNO snoPower, float flTimeReduction);  // PowersSwig.cpp:16154
BOOL PowerDoChanneledContinualCosts(float fResourceCostModifier);  // PowersSwig.cpp:471
void PowerTriggerCooldown(VACDID idActor, LPCSTR szPowerName, float flOverridenCooldown);  // PowersSwig.cpp:2190
int32 PowerCreateTargetAreaPie(float flRadius, int32 nDegrees, float flHeight);  // PowersSwig.cpp:1189
int32 PowerCreateTargetAreaPieAroundActor(VACDID idActor, float flRadius, int32 nDegrees, float flHeight);  // PowersSwig.cpp:1202
int32 PowerCreateTargetAreaPieAroundCursor(int32 nCursorID, float flRadius, int32 nDegrees, float flHeight);  // PowersSwig.cpp:1222
int32 PowerCreateTargetAreaCapsuleAroundActors(VACDID idActor1, VACDID idActor2, float flRadius, float flBonusHeight);  // PowersSwig.cpp:1402
int32 PowerCreateTargetAreaCapsuleAroundActorAndCursor(VACDID idActor, int32 nCursorID, float flRadius, float flBonusHeight);  // PowersSwig.cpp:1432
int32 PowerCreateTargetAreaCapsuleAroundCursors(int32 nCursorID1, int32 nCursorID2, float flRadius, float flBonusHeight);  // PowersSwig.cpp:1460
void PowerCursorRollToCursor(int32 nCursorID, int32 nDestCursorID, float flDestPerturbRadiusMax, LPCSTR szPowerPassability);  // PowersSwig.cpp:4399
BOOL PowerCursorSetToOpenLocationNearActor(int32 nCursorID, VACDID idActor, float flSkipDistance);  // PowersSwig.cpp:4895
void PowerCursorOffsetFromOwner(int32 nCursorID, float flDistance, BOOL bAs2D);  // PowersSwig.cpp:5095
int32 PowerCreatePayloadNew(PayloadDeliveryMechanism eDeliveryMechanism, float flBaseDamageScalar, BOOL bForcedPhysical, ACDID idACDToUse);  // PowersSwig.cpp:3127
void PowerPayloadAddDamage(int32 nPayloadID, DamageType eDamageType, float flDamage, float flDamageDelta, BOOL bIgnoreBonuses);  // PowersSwig.cpp:3219
void PowerPayloadAddPeriodicDamage(int32 nPayloadID, DamageType eDamageType, float flDuration, float flDamage, float flDamageDelta, BOOL bIgnoreBonuses, BOOL bTriggersHitProcsAndThorns);  // PowersSwig.cpp:3259
void PowerPayloadConvertToDOTDamageType(int32 nPayloadID, DamageType eDamageType, float flDuration, BOOL bTriggersHitProcsAndThorns);  // PowersSwig.cpp:3348
void PowerPayloadAddFreeze(int32 nPayloadID, float flDuration, float flDurationDelta, float flBreakChance);  // PowersSwig.cpp:3659
void PowerPayloadAddChill(int32 nPayloadID, float flChillAmount, float flDuration, float flDurationDelta);  // PowersSwig.cpp:3702
void PowerPayloadAddBlind(int32 nPayloadID, float flDuration, float flDurationDelta, BOOL bBreaksOnDamage);  // PowersSwig.cpp:3724
void PowerPayloadSetKnockback(int32 nPayloadID, float flMagnitude, float flHeight, float flGravity, BOOL bIgnoreWeight, BOOL bAffectedByReduction, LPCSTR szKnockbackPower, BOOL bContinuousKnockbackPower);  // PowersSwig.cpp:3761
ACDID PowerCreateProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flProjectileSpeed, float flDistanceToTravel, BOOL bHugGround, BOOL bDontPerformMarbleRollToCaster, BOOL bWallBounce, BOOL bUseProjectileStaticFlagsForSpawn, BOOL bIgnoreMonsterCollision);  // PowersSwig.cpp:5602
ACDID PowerCreateLobbedProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flGravity, float flHeightAboveSource, ProjectileHitType eHitType, BOOL bDontPerformMarbleRollToCaster, BOOL bUseProjectileStaticFlagsForSpawn);  // PowersSwig.cpp:5689
ACDID PowerCreateLobbedProjectileWithBounce(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flGravity, float flHeightAboveSource, float flUnitBounceSpeed, float flLifetimeInSecs, ProjectileHitType eHitType, BOOL bDontDestroyOnImpact, BOOL bDontPerformMarbleRollToCaster, BOOL bUseProjectileStaticFlagsForSpawn);  // PowersSwig.cpp:5703
ACDID PowerCreateDeterministicProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flProjectileSpeed, float flLifetime, int32 ePathType, BOOL bDontPerformMarbleRollToCaster, BOOL bHugGround, BOOL bWallBounce, BOOL bUseProjectileStaticFlagsForSpawn, BOOL bDontUsePowerPassability);  // PowersSwig.cpp:5783
ACDID PowerCreateSinWaveProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flProjectileSpeed, float flLifetime, float flOscillationSpeed, float flMaxWidth, float flLateralDistanceToScaleToMax, float flLateralDistanceToStartScale, BOOL bDontPerformMarbleRollToCaster, BOOL bHugGround, BOOL bUseProjectileStaticFlagsForSpawn, BOOL bIgnoreMonsterCollision);  // PowersSwig.cpp:5877
ACDID PowerCreatePedalCurveProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flOscillationSpeed, float flLifetime, float flMaxSize, float flHorizScale, BOOL bDontPerformMarbleRollToCaster, BOOL bHugGround, BOOL bUseProjectileStaticFlagsForSpawn);  // PowersSwig.cpp:5987
ACDID PowerCreateSpiralProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flRotationSpeed, float flRotationAccel, float flLifetime, float flMaxSize, float flStartSize, float flSpiralGrowthSpeed, BOOL bDontPerformMarbleRollToCaster, BOOL bHugGround, BOOL bRotateAroundOwner, BOOL bUseProjectileStaticFlagsForSpawn);  // PowersSwig.cpp:6082
ACDID PowerCreateImmediateSpiralProjectile(int32 nSourceCursorID, int32 nSpiralCenterCursorID, LPCSTR szProjectileName, float flRotationSpeed, float flRotationAccel, float flLifetime, float flMaxSize, float flSpiralGrowthSpeed, BOOL bDontPerformMarbleRollToCaster, BOOL bHugGround, BOOL bRotateAroundOwner, BOOL bUseProjectileStaticFlagsForSpawn);  // PowersSwig.cpp:6191
ACDID PowerCreatePieWedgeProjectile(int32 nSourceCursorID, int32 nFirstTargetCursorID, int32 nSecondTargetCursorID, LPCSTR szProjectileName, float flStraightSpeed, float flArcSpeed, float flMaxLifetime, BOOL bHugGround, BOOL bDontPerformMarbleRollToCaster, BOOL bUseProjectileStaticFlagsForSpawn);  // PowersSwig.cpp:6318
ACDID PowerCreateCurvedOffsetProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flProjectileSpeed, float flLifetime, float flLateralOffset, BOOL bDontPerformMarbleRollToCaster, BOOL bHugGround, BOOL bUseProjectileStaticFlagsForSpawn);  // PowersSwig.cpp:6425
void PowerProjectileSeekActor(ACDID idACDProjectile, ACDID idACDTarget, float flMaxAngleToRotate, BOOL bYawOnly);  // PowersSwig.cpp:6666
void PowerProjectileSeekCursor(ACDID idACDProjectile, int32 nCursorID, float flMaxAngleToRotate, BOOL bYawOnly);  // PowersSwig.cpp:6628
void PowerProjectileReflect(ACDID idACDProjectile, BOOL bReflectYawOnly, int32 nDegreesOffset, float flSpeedMultiplier, ACDID idACDForcedTarget, float flReflectDamageScalar, float flReflectHealthPercentDamageCapScalar);  // PowersSwig.cpp:6812
void PowerProjectileSetSpeedMultiplier(ACDID idACDProjectile, float flMultiplier, float flDuration, BOOL bAdjustDetonationTimes, BOOL bSmoothSlow, BOOL bForce);  // PowersSwig.cpp:6908
ACDID PowerCreateBeam(LPCSTR szBeamEffectGroupName, LPCSTR szBeamTargetActorName, float flMaxLength, BOOL bUseRollToClickPositionOnGround, SNO snoPowerPassability);  // PowersSwig.cpp:7399
void PowerBeamUpdate(ACDID idACDTarget, float flMaxLength, BOOL bUseRollToClickPositionOnGround, SNO snoPowerPassability);  // PowersSwig.cpp:7500
int32 PowerActorGetNearbySummonCount(ACDID idActor, float fRadii);  // PowersSwig.cpp:7865
void PowerBuffSetDOTDPS(int32 nBuffID, DamageType eDamageType, float flDPS, BOOL bWantBonuses, BOOL bCanCrit, BOOL bHideDOTDamageEffects, BOOL bUseHighestElementalDamageBonus);  // PowersSwig.cpp:9548
void PowerBuffEnableOnReflectEffect(int32 nBuffID, float flReflectChance);  // PowersSwig.cpp:11152
float PowerBuffDamageShieldAbsorb(float flAbsorbAmount, BOOL bWantAbsorbedMessage);  // PowersSwig.cpp:11492
void PowerTeleportActorToCursor(VACDID idActor, int32 nCursorID, BOOL bDoRollout, BOOL bCheckForPath, float flCameraSmoothingSeconds, BOOL bSnapToGround);  // PowersSwig.cpp:13875
void PowerActorAddResource(ACDID idACDActor, float flAmount, BOOL bPrimary, BOOL bApplyOnHitBonuses);  // PowersSwig.cpp:15287
void PowerActorSpendResource(VACDID idACDActor, float flAmount, BOOL bPrimary, BOOL bDoResourceCostReduction, BOOL bTriggerOnCostCallbacks, BOOL bDoHeal);  // PowersSwig.cpp:15324
BOOL PowerActorCanSpendResource(VACDID idACDActor, float flAmount, BOOL bPrimary, BOOL bDoResourceCostReduction);  // PowersSwig.cpp:15372
void PowerActorHeal(ACDID idACDActor, float flAmount, HealType eHealType, ACDID idACDSource, BOOL bPlayEffect, int32 nNumberOfHeals);  // PowersSwig.cpp:15407
void PowerDebugDrawSphere(int32, float, uint32);  // PowersSwig.cpp:16593
void PowerDebugDrawCapsule(int32, int32, float, uint32);  // PowersSwig.cpp:16595
void PowerDebugDrawPie(int32, float, float, uint32);  // PowersSwig.cpp:16596
CRefString PowerGetNameFromID(int32 idPower);  // PowersSwig.cpp:587
CRefString PowerGetReviveMonsterFromNecroCorpse(VACDID idCorpse);  // PowersSwig.cpp:2491
CRefString PowerReviveGetBestMonster(int32 nCursorID);  // PowersSwig.cpp:2518
CRefString PowerGetMonsterSNONameFormulaValue(MonsterFormulaIndex eFormulaIndex);  // PowersSwig.cpp:15966
ActorIDArray * PowerGetTargets(int32 nTargetAreaID);  // PowersSwig.cpp:2236
ActorIDArray * PowerGetTargetsSortedByDistance(int32 nTargetAreaID);  // PowersSwig.cpp:2358
ActorIDArray * PowerGetNecroCorpsesAroundCursor(int32 nCursorID);  // PowersSwig.cpp:2450
ActorIDArray * PowerGetSeparatedTargets(int32 nTargetAreaID, float flSeparationDist);  // PowersSwig.cpp:2566
ActorIDArray * PowerGetProjectileTargets(int32 nTargetAreaID);  // PowersSwig.cpp:2650
ActorIDArray * PowerGetTrackedActors();  // PowersSwig.cpp:15635
ActorIDArray * PowerSpawnMirrorImages(ACDID idACDSource, LPCSTR szMonsterActorName, float flSpawnRadius, int32 nCount, int32 nMaxCount, BOOL bMoveCaster, BOOL bDoAnimation, LPCSTR szPowerForPassability, PetType ePetType);  // PowersSwig.cpp:14431
ActorIDArray * PowerPlayerGetPets(PetType ePetType, VACDID idACDPlayer);  // PowersSwig.cpp:8402
ActorIDArray * PowerActorGetNearbySummons(ACDID idActor, float fRadii);  // PowersSwig.cpp:7893
int32 PowerCreatePayload(float flBaseDamageScalar, BOOL bForcedPhysical, ACDID idACDToUse); // PowersSwig.cpp:3116
BOOL PowerBuffIsOnActor(LPCSTR szPowerName, ACDID idActor, BOOL bIncludeDeleted); // PowersSwig.cpp:8919
BOOL PowerBuffIsOnActor(ACDID idActor, BOOL bIncludeDeleted); // PowersSwig.cpp:8962
BOOL PowerBuffIsOnActor(int32 nBuffID, ACDID idActor, BOOL bIncludeDeleted); // PowersSwig.cpp:8983
BOOL PowerBuffIsOnActor(LPCSTR szPowerName, int32 nBuffID, ACDID idActor, BOOL bIncludeDeleted, int32 idPower); // PowersSwig.cpp:9005
void PowerForceStopPrimary(BOOL bStopReasonIsLookswitch);  // PowersSwig.cpp:748
float PowerGetOwnerAttacksPerSecondTotal(BOOL bFromPower);  // PowersSwig.cpp:907
int32 PowerPlayRopeActorToActor(VACDID idActorFrom, RopePosition ePositionFrom, VACDID idActorTo, RopePosition ePositionTo, LPCSTR szRopeName, BOOL bUseComplexEffect, BOOL bIgnoreOwnerAlpha);  // PowersSwig.cpp:2959
void PowerPlayRopeActorToCursor(VACDID idActorFrom, RopePosition ePositionFrom, int32 nCursorIDTo, LPCSTR szRopeName, BOOL bIgnoreOwnerAlpha);  // PowersSwig.cpp:2973
double PowerBuffAppliedDamageGetAttribute(Attrib eAttrib, int32 nParam);  // PowersSwig.cpp:12180
DamageType PowerBuffAppliedDamageGetPredominantDamageType(BOOL bExcludePhysical);  // PowersSwig.cpp:12319
int32 PowerActorPlayEffectGroupRopeActorToActor(VACDID idActorFrom, RopePosition ePositionFrom, VACDID idActorTo, RopePosition ePositionTo, LPCSTR szRopeName, BOOL bUseComplexEffect, BOOL bIgnoreOwnerAlpha);  // PowersSwig.cpp:16447

DamageType PowerGetDamageType(int32 idPower);  // X/Server/Powers/PowersSwig.cpp:537

DamageType PowerGetDamageType(SNO snoPower, RuneType eRuneType);  // X/Server/Powers/PowersSwig.cpp:550

void PowerSetCooldown(float flDuration, BOOL bIgnoreCooldownReduction);  // X/Server/Powers/PowersSwig.cpp:1743

void PowerSetCooldown(VACDID idActor, LPCSTR szPowerName, float flDuration, BOOL bIgnoreCooldownReduction);  // X/Server/Powers/PowersSwig.cpp:1802

int32 PowerCreateTargetAreaCylinder(float flRadius);  // X/Server/Powers/PowersSwig.cpp:1277

int32 PowerCreateTargetAreaCylinder(float flRadius, float flHeight);  // X/Server/Powers/PowersSwig.cpp:1283

int32 PowerCreateTargetAreaCylinderAroundActor(VACDID idActor, float flRadius);  // X/Server/Powers/PowersSwig.cpp:1296

int32 PowerCreateTargetAreaCylinderAroundActor(VACDID idActor, float flRadius, float flHeight);  // X/Server/Powers/PowersSwig.cpp:1303

int32 PowerCreateTargetAreaCylinderAroundCursor(int32 nCursorID, float flRadius);  // X/Server/Powers/PowersSwig.cpp:1324

int32 PowerCreateTargetAreaCylinderAroundCursor(int32 nCursorID, float flRadius, float flHeight);  // X/Server/Powers/PowersSwig.cpp:1331

int32 PowerCreateBuff(BOOL bPositive, float flDuration);  // X/Server/Powers/PowersSwig.cpp:8568

int32 PowerCreateBuff(int32 nBuffIndex, BOOL bPositive, float flDuration);  // X/Server/Powers/PowersSwig.cpp:8580

void PowerBuffSetDuration(int32 nBuffID, VACDID idACD, float flDuration, BOOL bCasterAsSource, BOOL bOnlyFirstBuff, BOOL bResetStartTick);  // X/Server/Powers/PowersSwig.cpp:9999

void PowerBuffSetDuration(LPCSTR szPowerName, int32 nBuffID, ACDID idACD, float flDuration, BOOL bCasterAsSource, BOOL bOnlyFirstBuff, int32 idPower, BOOL bResetStartTick);  // X/Server/Powers/PowersSwig.cpp:9962

void PowerBuffSetDuration(int nBuffID, float flDuration);  // X/Server/Powers/PowersSwig.cpp:10018

void PowerBuffSetStoredData(int32 nBuffID, int32 nValue);  // X/Server/Powers/PowersSwig.cpp:10245

void PowerBuffSetStoredData(ACDID idACD, int32 nBuffID, int32 nValue, BOOL bFromPowerOwnerOnly);  // X/Server/Powers/PowersSwig.cpp:10289

void PowerBuffSetStoredDataFloat(int32 nBuffID, float flValue);  // X/Server/Powers/PowersSwig.cpp:10267

void PowerBuffSetStoredDataFloat(ACDID idACD, int32 nBuffID, float flValue, BOOL bFromPowerOwnerOnly);  // X/Server/Powers/PowersSwig.cpp:10315

int32 PowerBuffGetStoredData(ACDID idACD, int32 nBuffID, BOOL bFromPowerOwnerOnly, BOOL bAllowDeleted);  // X/Server/Powers/PowersSwig.cpp:10341

int32 PowerBuffGetStoredData(LPCSTR szPowerName, ACDID idACD, int32 nBuffID, BOOL bAllowDeleted);  // X/Server/Powers/PowersSwig.cpp:10395

float PowerBuffGetStoredDataFloat(ACDID idACD, int32 nBuffID, BOOL bFromPowerOwnerOnly, BOOL bAllowDeleted);  // X/Server/Powers/PowersSwig.cpp:10368

float PowerBuffGetStoredDataFloat(LPCSTR szPowerName, ACDID idACD, int32 nBuffID, BOOL bAllowDeleted);  // X/Server/Powers/PowersSwig.cpp:10438

void PowerBuffSetUINumber(int32 nBuffID, int32 nValue);  // X/Server/Powers/PowersSwig.cpp:10527

void PowerBuffSetUINumber(ACDID idACD, int32 nBuffID, int32 nValue);  // X/Server/Powers/PowersSwig.cpp:10550

void PowerBuffSetUINumber(ACDID idACD, LPCSTR szPowerName, int32 nBuffID, int32 nValue);  // X/Server/Powers/PowersSwig.cpp:10581

int32 PowerBuffGetUINumber(ACDID idACD, int32 nBuffID);  // X/Server/Powers/PowersSwig.cpp:10629

int32 PowerBuffGetUINumber(ACDID idACD, LPCSTR szPowerName, int32 nBuffID);  // X/Server/Powers/PowersSwig.cpp:10659

void PowerSendMessageToBuff(LPCSTR szTargetPower, int32 nBuffIndex, uint32 nParam0, uint32 nParam1, float flParam2);  // X/Server/Powers/PowersSwig.cpp:12667

void PowerSendMessageToBuff(ACDID idActor, LPCSTR szTargetPower, int32 nBuffIndex, uint32 nParam0, uint32 nParam1, float flParam2);  // X/Server/Powers/PowersSwig.cpp:12688

int32 PowerGetStoredInt(Power * ptPower, LPCSTR szName);  // PowersSwig.cpp:5818
