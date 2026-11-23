// X/Server/Powers/PowerHelper.cpp
// Functions sit on their original line numbers.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "PowerHelper.h"
#include "PowerAPI.h"
#include "Powers.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/GameConstants.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../Actors/Actors.h"
#include "../../Common/Player.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Networking/GameMessage.h"
// Belongs in X/Server/Powers/PowerAPI.h (line 261); kept here because PowerAPI.h does not include NavigationMesh.h.
#include "../../Common/Nav/NavZoneIntersect.h"
struct PowerAPIChooseOpenGround {  // size 0x20
    NavPlace tNavPlace;  // 0x00
    float flSkipDistance;  // 0x18
    BOOL bUseActorPassability;  // 0x1c
};
static_assert(sizeof(PowerAPIChooseOpenGround) == 0x20, "PowerAPIChooseOpenGround");
#include "../../Common/Nav/NavCellPath.h"
#include "../../Common/Movement/ActorMovement.h"  // below: X/Server/Powers/Powers.cpp
void InitializeDamage(DamageAttribute & tDamageAttribute, FastAttribKey tKey, float flValue, int32 nValue, int32 nDuration);
#include "../../Common/WorldCommonCollision.h"  // below: X/Server/Powers/PowerAttributes.cpp
float PowerAttributesGetFloat(PowerID idPower, FastAttribKey tKey);
int32 PowerAttributesGetInt(const PowerID idPower, FastAttribKey tKey);
#include "../../Common/Nav/NavZone.h"  // below: X/Server/Powers/Powers.cpp
SNO PowerEffectTagGetSNO(PowerID idPower, uint32 dwTag);
BOOL PowerContactTagGetBool(PowerID idPower, uint32 dwTag);
int32 PowerContactTagGetInt(PowerID idPower, uint32 dwTag);
float PowerContactTagGetFloat(PowerID idPower, uint32 dwTag);
SNO PowerContactTagGetSNO(PowerID idPower, uint32 dwTag);
/* declared here for the functions below (PowersCommon.cpp, Powers.cpp, PowerAPI.cpp, Actors.cpp, ActorInventory.cpp, Items.cpp, SGame.cpp, AnimPlayer.cpp) */ void PowerGetNavPlaceFromACD(const ActorCommonData * ptACD, NavPlace & tNavPlace); BOOL PowerRollToTargetNavPlace(ACDID idACD, const Target & tTarget, const PassabilityType ePassabilityType, const BOOL fIgnoreDestinationHeight, NavPlace & tNavPlace); void PowerHelperGetNavPlaceFromPowerSource(PowerID idPower, NavPlace & tNavPlace); BOOL PowerHelperRollToPowerTarget(PowerID idPower, const PassabilityType ePassabilityType, NavPlace & tNavPlace); SNO PowerGetExplosionSNO(const PowerID idPower);
BOOL PowerAPI_DoCosts(const PowerAPICosts & tCostInfo, float flResourceCostModifier); void PowerHelper_CostsInit(PowerID idPower, PowerAPICosts & tCosts); BOOL ActorFlagIsSet(const ActorID idActor, const ActorFlag eFlag); BOOL PowerEffectTagExists(PowerID idPower, uint32 dwTag); int32 PowerEffectTagGetInt(PowerID idPower, uint32 dwTag);
ACDID ACDInventoryGetActiveWeapon(const ActorCommonData & tACD); BOOL ItemIsBroken(const ActorCommonData & tACDItem); RNG & SGameGetRNG(); BOOL AnimPlayerGetAnimationTiming(ActorCommonData & tACD, const uint32 dwAnimationTag, const uint32 dwLength, const float flSpeedScalar, RNG & rng, AnimationTiming & tAnimationTiming); uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim);
float PowerGetCostReductionPercent(const ActorCommonData & tACD, const ResourceType eResourceType, const DamageType eDamageType, const SNO snoPower); ResourceType PowerGetResourceType(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower); AttribGroupID PowerGetAttributeGroup(const PowerID idPower); float PowerGetResourceCost(PowerID idPower); float PowerGetAnimationSpeedForPower(const ACDID idACD, AutoPowerDefinitionR pdefPower, const int32 nComboLevel);
BOOL PowerAPI_DoChooseOpenGround(SNO snoActor, float flScale, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & placeOpenSpot, NavMeshHandle & tNavMeshHandleResult); void PowerHelper_ChooseOpenGroundInit(PowerID idPower, const NavPlace & tNavPlace, PowerAPIChooseOpenGround & tOpenGroundInfo, float flSkipDistance); void PowerGetTargetPlace(AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldPlace & tPlace); PayloadType PowerGetPayload(PowerID idPower); float PowerGetPayloadParam0(PowerID idPower); float PowerGetPayloadParam1(PowerID idPower); uint32 PowerTagGetDamageMinTag(DamageType eDamageType); uint32 PowerTagGetDamageDeltaTag(DamageType eDamageType); SNO PowerGetTargetingPassabilitySNOBySNO(SNO snoPower); uint32 NavMeshHandleToStaticFlags(const NavMeshHandle & tNavMeshHandle, BOOL fSpecialOnly); BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, const WorldPlace & placeStart, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax); BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult); BOOL WorldCollSearchForLocationOnLineSegment(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flLocationRadius, const Vector3D & vDest, ACDID idACDIgnore, float flDesiredDistBetweenChecks, int32 nMaxChecks, Vector3D & wpEnd, Team eTeam); uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability); void ACDCollGetSettings(SNO snoActor, ACDCollSettings & tCollSettings); BOOL PowerGetCollFlags(AutoPowerDefinitionR pdefPower, PowerPassabilityType ePassability, ActorCollisionFlags & tCollFlags); WorldPlace PowerHelper_ComputePathableDestination(const SNO snoPassability, const WorldPlace & wpStart, const WorldPlace & wpEnd, const float flRadius, ActorCommonData & tACD); BOOL NavZoneCollideCircle(const NavCellHandle & hNavCell, const Circle & wc, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, NavCellCollideResult & tResult); BOOL ACDCollCollideCircle(SWorldID idSWorld, const Circle & wc, const ACDCollParams & tCollParams, ACDID idACDIgnore, ACDCollNarrowResult & tResult); BOOL PowerHelper_ComputeDestinationFromBadLocation(ACDID idACDPlayer, const Vector3D & vPosBad, Vector3D & vPosNew, BOOL bSkipRedNavMeshRoll);
void PowerHelper_AttackPayloadInit_Blank(PowerAPIAttackPayload & tPayload)
{
    SigmaMemoryClear(&tPayload, sizeof(tPayload));
    tPayload.idPower = POWER_NONE;
    tPayload.eDeliveryMechanism = 0;
    tPayload.snoProjectileToStick = SNO_NONE;
    tPayload.eDeathReplacementType = 0;
    tPayload.eAttackDirection = ATTACKDIRECTION_STRAIGHT;
    tPayload.snoKnockbackPower = SNO_NONE;
    tPayload.snoTargetImpactParticle = SNO_NONE;
    tPayload.snoTargetImpactEffectGroup = SNO_NONE;
    tPayload.snoTargetAttackedParticle = SNO_NONE;
    tPayload.eKnockBack = 0;

    tPayload.flPrimaryAttributeBonusMultiplier = 1.0f;
    tPayload.flClassBonusMultiplier = 1.0f;
    tPayload.flSkillsBonusMultiplier = 1.0f;
    tPayload.flCritCap = 1000.0f;
    tPayload.idACDFloatingCombatTextSourceOverride = ACDID_INVALID;
    tPayload.flProcScalar = 0.0f;
    tPayload.bIsAOEThorns = FALSE;
}





static void sAttackPayloadInit_Standard(PowerID idPower, PowerAPIAttackPayload & tPayload)
{
    Power * ptPower;

    PowerHelper_AttackPayloadInit_Blank(tPayload);

    tPayload.idPower = idPower;

    tPayload.flBaseDamageScalar = 1.0f;
    tPayload.flPetDamageScalar = 1.0f;

    tPayload.snoTargetImpactParticle = PowerEffectTagGetSNO(idPower, TAG_POWER_TARGET_IMPACT_SNO);
    tPayload.snoTargetImpactEffectGroup = PowerEffectTagGetSNO(idPower, TAG_POWER_TARGET_IMPACT_EFFECT_GROUP);
    tPayload.snoTargetAttackedParticle = PowerEffectTagGetSNO(idPower, TAG_POWER_TARGET_ATTACKED_SNO);

    tPayload.flChanceToStun = PowerGetFormulaValue(idPower, TAG_POWER_STUN_CHANCE);
    tPayload.flChanceToStun += PowerAttributesGetFloat(idPower, FastAttribKey(ATTRIBUTE_STUN_CHANCE));
    tPayload.rangeStunLength.flMin = PowerGetFormulaValue(idPower, TAG_POWER_STUN_DURATION_MIN);
    tPayload.rangeStunLength.flMin += PowerAttributesGetFloat(idPower, FastAttribKey(ATTRIBUTE_STUN_LENGTH));
    tPayload.rangeStunLength.flMin *= TICKS_PER_SECOND;
    tPayload.rangeStunLength.flDelta = PowerGetFormulaValue(idPower, TAG_POWER_STUN_DURATION_DELTA) * TICKS_PER_SECOND;

    tPayload.rangeFearLength.flMin = PowerGetFormulaValue(idPower, TAG_POWER_FEAR_DURATION_MIN) * TICKS_PER_SECOND;
    tPayload.rangeFearLength.flDelta = PowerGetFormulaValue(idPower, TAG_POWER_FEAR_DURATION_DELTA) * TICKS_PER_SECOND;

    tPayload.rangeFreezeLength.flMin = PowerGetFormulaValue(idPower, TAG_POWER_FREEZE_DURATION_MIN) * TICKS_PER_SECOND;
    tPayload.rangeFreezeLength.flDelta = PowerGetFormulaValue(idPower, TAG_POWER_FREEZE_DURATION_DELTA) * TICKS_PER_SECOND;
    tPayload.flFreezeBreakChance = 1.0f;

    tPayload.rangeWebLength.flMin = PowerGetFormulaValue(idPower, TAG_POWER_WEB_DURATION_MIN) * TICKS_PER_SECOND;
    tPayload.rangeWebLength.flDelta = PowerGetFormulaValue(idPower, TAG_POWER_WEB_DURATION_DELTA) * TICKS_PER_SECOND;

    tPayload.rangeImmobolizeLength.flMin = PowerGetFormulaValue(idPower, TAG_POWER_IMMOBILIZE_DURATION_MIN) * TICKS_PER_SECOND;
    tPayload.rangeImmobolizeLength.flDelta = PowerGetFormulaValue(idPower, TAG_POWER_IMMOBILIZE_DURATION_DELTA) * TICKS_PER_SECOND;

    tPayload.rangeSlowLength.flMin = PowerGetFormulaValue(idPower, TAG_POWER_SLOW_DURATION_MIN) * TICKS_PER_SECOND;
    tPayload.rangeSlowLength.flDelta = PowerGetFormulaValue(idPower, TAG_POWER_SLOW_DURATION_DELTA) * TICKS_PER_SECOND;
    tPayload.flSlowAmount = PowerGetFormulaValue(idPower, TAG_POWER_SLOW_AMOUNT);

    tPayload.rangeManaDrain.flMin = PowerGetFormulaValue(idPower, TAG_POWER_MANA_DRAIN_AMOUNT_MIN);
    tPayload.rangeManaDrain.flDelta = PowerGetFormulaValue(idPower, TAG_POWER_MANA_DRAIN_AMOUNT_DELTA);

    tPayload.rangeBlindLength.flMin = PowerGetFormulaValue(idPower, TAG_BLIND_DURATION_MIN) * TICKS_PER_SECOND;
    tPayload.rangeBlindLength.flDelta = PowerGetFormulaValue(idPower, TAG_BLIND_DURATION_DELTA) * TICKS_PER_SECOND;
    tPayload.bBlindBreaksOnDamage = TRUE;

    tPayload.bChill = FALSE;
    tPayload.flChillMagnitude = 0.0f;
    tPayload.rangeChillLength.flMin = 0.0f;
    tPayload.rangeChillLength.flDelta = 0.0f;

    tPayload.flPowerAdditionalChanceToCriticalHit = PowerGetFormulaValue(idPower, TAG_POWER_CRIT_CHANCE);
    tPayload.flPowerCritDamageBonusPercent = 0.0f;
    tPayload.bCanCrit = TRUE;
    tPayload.bAlwaysHits = PowerContactTagGetBool(idPower, TAG_POWER_ALWAYS_HITS);
    tPayload.flPowerAdditionalChanceToCrushingBlow = 0.0f;

    tPayload.bTriggersHitProcs = TRUE;
    tPayload.bIgnoreCharmDamageScalar = FALSE;
    tPayload.bSplashDamage = FALSE;
    tPayload.bDisableHitEffects = FALSE;
    tPayload.bUseHighestElementalDamageBonus = FALSE;
    tPayload.bIsAOEThorns = FALSE;

    if (PowerContactTagGetBool(idPower, TAG_POWER_PAYLOAD_AFFECTED_BY_PET_AOE_SCALAR))
    {
        ScopedGlobalDefinitionLockR pdefGlobal;

        float flScalar = Math::Max(0.0f, pdefGlobal->ptServerData->flPetAoEDamageScalar);
        tPayload.flPetDamageScalar = flScalar;
    }

    ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        tPayload.flProcScalar = PowerGetProcScalar(*ptPower);
        tPayload.bTriggersHitProcs = tPayload.flProcScalar > EPSILON;
    }
}

























static void sAttackPayloadInit_ApplyAttackFear(PowerID idPower, PowerAPIAttackPayload & tPayload)
{
    // Attack fear from items
    //
    //
    float flFearChance = PowerAttributesGetFloat(idPower, FastAttribKey(ATTRIBUTE_ATTACK_FEAR_CHANCE));
    if (flFearChance > EPSILON)
    {
        if (RandFloat_Internal(SGameGlobalsGet().rng) <= flFearChance)
        {
            float flFearTimeMin = PowerAttributesGetFloat(idPower, FastAttribKey(ATTRIBUTE_ATTACK_FEAR_TIME_MIN));
            float flFearTimeDelta = PowerAttributesGetFloat(idPower, FastAttribKey(ATTRIBUTE_ATTACK_FEAR_TIME_DELTA));
            float flFearTimeTotal = flFearTimeMin + flFearTimeDelta;
            if (flFearTimeTotal > tPayload.rangeFearLength.flMin + tPayload.rangeFearLength.flDelta)
            {
                tPayload.rangeFearLength.flMin = flFearTimeMin * TICKS_PER_SECOND;
                tPayload.rangeFearLength.flDelta = flFearTimeDelta * TICKS_PER_SECOND;
            }
        }
    }
}




static void sAttackPayloadInit_ApplyDamageTypeOverrides(PowerAPIAttackPayload & tPayload)
{
    PowerID idPower = tPayload.idPower;
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL)
    {
        return;
    }

    //
    DamageType eDamageTypeOverride = (DamageType)ptACDOwner->AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_TYPE_OVERRIDE));

    if (eDamageTypeOverride == DAMAGE_TYPE_NONE)
    {
        return;
    }

    float flMinTotal = 0.0f;
    float flDeltaTotal = 0.0f;

    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; nDamageType++)
    {
        flMinTotal += tPayload.arCombatDamage[nDamageType].flMin;
        flDeltaTotal += tPayload.arCombatDamage[nDamageType].flDelta;

        tPayload.arCombatDamage[nDamageType].flMin = 0.0f;
        tPayload.arCombatDamage[nDamageType].flDelta = 0.0f;
    }

    //
    tPayload.arCombatDamage[eDamageTypeOverride].flMin = flMinTotal;
    tPayload.arCombatDamage[eDamageTypeOverride].flDelta = flDeltaTotal;
}



void PowerHelper_AttackPayloadInit_None(PowerID idPower, PowerAPIAttackPayload & tPayload)
{
    sAttackPayloadInit_Standard(idPower, tPayload);
}






void PowerHelper_AttackPayloadInit_Standard(PowerID idPower, PowerAPIAttackPayload & tPayload)
{
    //
    sAttackPayloadInit_Standard(idPower, tPayload);

    SNO snoPower = PowerGetSNO(idPower);
    PowerDamageData tData;
    //
    PowerGetDamageData(snoPower, PowerGetOwner(idPower), idPower, tData, NULL, FALSE);

    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; nDamageType++)
    {
        ElementDamage & tDmg = tData.tElementDamage[nDamageType];
        tPayload.arCombatDamage[nDamageType].flMin = tDmg.flTotalMin;
        tPayload.arCombatDamage[nDamageType].flDelta = tDmg.flTotalDelta;
    }

    tPayload.flPrimaryAttributeBonusMultiplier = tData.flPrimaryAttributeBonusMultiplier;
    tPayload.flClassBonusMultiplier = tData.flClassBonusMultiplier;
    tPayload.flSkillsBonusMultiplier = tData.flSkillsBonusMultiplier;

    tPayload.flDamageDurationInSecs[DAMAGE_TYPE_PHYSICAL] = PowerAttributesGetFloat(idPower, FastAttribKey(ATTRIBUTE_BLEED_DURATION));

    if (tPayload.flDamageDurationInSecs[DAMAGE_TYPE_PHYSICAL] < EPSILON)
    {
        tPayload.flDamageDurationInSecs[DAMAGE_TYPE_PHYSICAL] = PowerGetFormulaValue(idPower, TAG_POWER_BLEED_DURATION);
    }
    if (tPayload.flDamageDurationInSecs[DAMAGE_TYPE_FIRE] < EPSILON)
    {
        tPayload.flDamageDurationInSecs[DAMAGE_TYPE_FIRE] = PowerGetFormulaValue(idPower, TAG_POWER_FIRE_DURATION);
    }
    if (tPayload.flDamageDurationInSecs[DAMAGE_TYPE_ARCANE] < EPSILON)
    {
        tPayload.flDamageDurationInSecs[DAMAGE_TYPE_ARCANE] = PowerGetFormulaValue(idPower, TAG_POWER_ARCANE_DURATION);
    }
    if (tPayload.flDamageDurationInSecs[DAMAGE_TYPE_HOLY] < EPSILON)
    {
        tPayload.flDamageDurationInSecs[DAMAGE_TYPE_HOLY] = PowerGetFormulaValue(idPower, TAG_POWER_HOLY_DURATION);
    }

    tPayload.eDOTStackType = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DOT_STACKING_METHOD));
    tPayload.nDOTMaxStackCount = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DOT_MAX_STACK_COUNT));

    sAttackPayloadInit_ApplyDamageTypeOverrides(tPayload);

    tPayload.wdKnockbackMagnitude = PowerGetFormulaValue(idPower, TAG_POWER_KNOCKBACK_MAGNITUDE);
    tPayload.flKnockbackHeightMin = PowerGetFormulaValue(idPower, TAG_POWER_KNOCKBACK_HEIGHT_MIN);
    tPayload.flKnockbackHeightDelta = PowerGetFormulaValue(idPower, TAG_POWER_KNOCKBACK_HEIGHT_DELTA);
    tPayload.flKnockbackGravityMin = PowerGetFormulaValue(idPower, TAG_POWER_KNOCKBACK_GRAVITY_MIN);
    tPayload.flKnockbackGravityDelta = PowerGetFormulaValue(idPower, TAG_POWER_KNOCKBACK_GRAVITY_DELTA);
    tPayload.snoKnockbackPower = PowerContactTagGetSNO(idPower, TAG_POWER_KNOCKBACK_POWER);
    tPayload.eDeliveryMechanism = PowerContactTagGetInt(idPower, TAG_POWER_DELIVERY_MECHANISM);
    tPayload.bKnockbackAffectedByReduction = TRUE;
    tPayload.bContinuousKnockbackPower = FALSE;
    //
    tPayload.bKnockbackIgnoresWeight = FALSE;
    tPayload.bTriggersHitProcs = PowerContactTagGetBool(idPower, TAG_POWER_TRIGGERS_HIT_PROCS);

    if (PowerGetFormulaValue(idPower, TAG_POWER_CAUSES_KNOCKBACK) != 0.0f)
    {
        tPayload.eKnockBack = 1;
    }

    if (PowerContactTagGetBool(idPower, TAG_POWER_CAUSES_KNOCKDOWN))
    {
        tPayload.bKnockdown = TRUE;
    }

    //
    sAttackPayloadInit_ApplyAttackFear(idPower, tPayload);
}




void PowerHelper_AttackPayloadInit_Scripted(PowerID idPower, PowerAPIAttackPayload & tPayload, const ActorCommonData * ptACDToUse, float flBaseDamageScalar, BOOL bForcedPhysical, BOOL bForceUseACDAttributes)
{
    //
    //
    //
    //
    //
    sAttackPayloadInit_Standard(idPower, tPayload);
    tPayload.flBaseDamageScalar = flBaseDamageScalar;
    tPayload.bForcedPhysical = bForcedPhysical;

    SNO snoPower = PowerGetSNO(idPower);
    PowerDamageData tData;
    //
    PowerGetDamageData(snoPower, ptACDToUse, idPower, tData, &tPayload, bForceUseACDAttributes);

    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; nDamageType++)
    {
        ElementDamage & tDmg = tData.tElementDamage[nDamageType];
        tPayload.arCombatDamage[nDamageType].flMin = tDmg.flTotalMin;
        tPayload.arCombatDamage[nDamageType].flDelta = tDmg.flTotalDelta;
    }

    tPayload.flPrimaryAttributeBonusMultiplier = tData.flPrimaryAttributeBonusMultiplier;
    tPayload.flClassBonusMultiplier = tData.flClassBonusMultiplier;
    tPayload.flSkillsBonusMultiplier = tData.flSkillsBonusMultiplier;

    sAttackPayloadInit_ApplyDamageTypeOverrides(tPayload);

    tPayload.eDOTStackType = 1;
    tPayload.nDOTMaxStackCount = 0x10000;

    tPayload.snoKnockbackPower = SNO_NONE;
    tPayload.bKnockbackIgnoresWeight = FALSE;
    tPayload.bContinuousKnockbackPower = FALSE;
}





BOOL PowerHelper_SummonActorInit(PowerID idPower, SNO snoActor, ACDID idACDCorpse, PowerAPISummonActor & tSummonInfo, BOOL fSearchForSpot)
{
    uint32 dwAnimTag;
    BOOL fSpotFound;
    NavPlace tNavPlace;



    if (fSearchForSpot)
    {
        fSpotFound = PowerHelperRollToPowerTarget(idPower, 4, tNavPlace);
    }

    SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
    tSummonInfo.idPower = idPower;

    tSummonInfo.idACDCorpse = idACDCorpse;
    tSummonInfo.snoActor = snoActor;
    tSummonInfo.nActorLevel = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SUMMONED_ACTOR_LEVEL));
    tSummonInfo.dwAddFlags = 0;

    dwAnimTag = PowerContactTagGetInt(idPower, TAG_POWER_SUMMONED_ANIMATION);
    if (dwAnimTag != (uint32)-1)
    {
        tSummonInfo.dwAnimTag = dwAnimTag;
    }





    if (fSearchForSpot)
    {
        PowerAPIChooseOpenGround tOpenGroundInfo;
        NavMeshHandle hNavMeshCurr;
        PowerHelper_ChooseOpenGroundInit(idPower, tNavPlace, tOpenGroundInfo, 0.0f);
        float flScale = 1.0f;

        fSpotFound = PowerAPI_DoChooseOpenGround(snoActor, flScale, tOpenGroundInfo, tSummonInfo.placeSummonLocation, hNavMeshCurr);
    }





    if (fSearchForSpot && !fSpotFound)
    {
        tSummonInfo.qOrientation = cgqIdentity;
        tSummonInfo.placeSummonLocation = cgplaceNull;
        return FALSE;
    }

    if (idACDCorpse != ACDID_INVALID)
    {
        ActorCommonData & tACDCorpse = ACDGet(idACDCorpse);

        ActorFlagForDeletion(ActorID(ACDGetOwner(idACDCorpse)));


        if (tSummonInfo.nActorLevel == 1)
        {
            tSummonInfo.nActorLevel = tACDCorpse.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
        }

        tSummonInfo.qOrientation = tACDCorpse.GetOrientation();
    }
    else
    {
        tSummonInfo.qOrientation.FromYaw(RandAngle(SGameGlobalsGet().rng));
    }

    tSummonInfo.eTeamOverride = -1;

    return TRUE;
}



void PowerHelper_ApplyDamageBonusMultipliers(const ActorCommonData & tACDTarget, SNO snoPower, DamageType eDamageType, float flAttackBonusMultiplier, float flClassBonusMultiplier, float flSkillsBonusMultiplier, float & flDamage)
{
    //
    //
    //
    //
    //
    //
    //
    float flAmplifyDamage = tACDTarget.AttributesGetFloat(FastAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_PERCENT)) +
                            tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_TYPE_PERCENT, eDamageType)) +
                            tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_AMPLIFYDAMAGE_SKILL_PERCENT, snoPower));

    if (flAmplifyDamage > 0.0f)
    {
        flSkillsBonusMultiplier += flAmplifyDamage;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    flDamage *= flAttackBonusMultiplier;
    flDamage *= flClassBonusMultiplier;
    flDamage *= flSkillsBonusMultiplier;
}



void PowerHelper_DamageInit(PowerAPIDamage & tDamage, PowerID idPower, const ACDID idACDTarget)
{
    //
    //
    SigmaMemoryClear(&tDamage, sizeof(tDamage));
    tDamage.listDamage.Initialize(&SGameManager::GameGet().ptPowerGlobals->tDamageAttributePool);
    tDamage.idPower = idPower;
    tDamage.idACDSource = ACDID_INVALID;
    tDamage.idACDTarget = idACDTarget;
    tDamage.idACDFloatingCombatTextSourceOverride = ACDID_INVALID;
    tDamage.bCausesTransform = FALSE;
    tDamage.bPhysical = FALSE;
    tDamage.flDamageBlocked = 0.0f;
    tDamage.bWasImmune = FALSE;
    tDamage.snoAltDamageSourcePower = SNO_NONE;
    tDamage.eDeathReplacementType = 0;
    tDamage.bWasChill = FALSE;
    tDamage.bProcessedShields = FALSE;
    tDamage.bForceCriticalDeath = FALSE;
    tDamage.bTriggersHitProcs = TRUE;
    tDamage.bDamageWasBlocked = FALSE;
    tDamage.bIgnoreDamageBonusAndReduction = FALSE;
    tDamage.bDisableHitEffects = FALSE;
    tDamage.bIsStaggerDamage = FALSE;
    tDamage.bSplashDamage = FALSE;
    //
    tDamage.bDontTriggerThreatListUpdate = FALSE;

    if (idPower == POWER_NONE)
    {
        tDamage.idACDMainActor = ACDID_INVALID;
        tDamage.eHitsound = 0;
        tDamage.flProcScalar = 0.0f;
    }
    else
    {
        Power * ptPower = PowerTryToGet(idPower);
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        tDamage.idACDMainActor = tDamage.idACDSource = ptACDOwner ? ptACDOwner->id : ACDID_INVALID;

        //
        //
        if (ptPower)
        {
            //
            if (ptPower->dwFlags & (1 << PF_WAS_REFLECTED))
            {
                tDamage.idACDMainActor = ptPower->idACDReflectedBy;
            }

            tDamage.eHitsound = ptPower->eHitsound;
            tDamage.flProcScalar = PowerGetProcScalar(*ptPower);
            if (ptPower->bDoesntAnimate && !ptPower->bAllowHitProcs)
            {
                //
                tDamage.bTriggersHitProcs = FALSE;
            }
        }
    }
}


void PowerHelper_DamageAdd(PowerAPIDamage & tDamageInfo, FastAttribKey tKey, float flAmount, int32 nDuration)
{
    //
    //
    //
    DamageAttribute attribDamage;
    InitializeDamage(attribDamage, tKey, flAmount, 0, nDuration);
    //
    tDamageInfo.listDamage.AddTail_Tracked(attribDamage);
}
















void PowerHelper_ProjectileInitForScriptedStandardProjectile(PowerID idPower,
                                                             ACDID idACDSource,
                                                             const NavPlace & tNavPlace,
                                                             const Target & tTarget,
                                                             const SpellEffect eSpellEffect,
                                                             float flProjectileSpeed,
                                                             PowerAPICreateProjectile & tProjectile,
                                                             float flDistanceToTravel,
                                                             BOOL bHugGround,
                                                             BOOL bDontPerformMarbleRollToCaster,
                                                             BOOL bUseProjectileStaticFlagsForSpawn,
                                                             BOOL bNavReflect,
                                                             BOOL bIgnoreMonsterCollision)

{
    Power * ptPower = PowerTryToGet(idPower);
    WorldDistance wdExtraDistanceOnFirstStep = 0.0f;
    if (!ptPower)
    {
        return;
    }

    tProjectile.idPower = idPower;
    tProjectile.tProjectileData.dwFlags = 1 << PROJECTILE_DETONATE_AFTER_FIXED_TIME_BIT;
    if (bHugGround)
    {
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_HUGS_GROUND_BIT;
    }
    if (bDontPerformMarbleRollToCaster)
    {
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_DONT_PERFORM_MARBLE_ROLL_TO_CASTER_BIT;
    }
    if (bNavReflect)
    {
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_NAV_REFLECT;
    }
    if (bUseProjectileStaticFlagsForSpawn)
    {
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN;
    }

    if (bIgnoreMonsterCollision)
    {
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_DONT_HIT_MONSTERS_BIT;
    }

    tProjectile.tProjectileData.eSpellEffect = eSpellEffect;

    tProjectile.tProjectileData.snoPower = ptPower->snoPower;


    tProjectile.tProjectileData.tAimingData.eAimMethod = AIMMETHOD_LINEAR;
    tProjectile.tProjectileData.tAimingData.wdGravity = 0.0f;
    tProjectile.tProjectileData.tAimingData.flHeightAboveSource = 0.0f;



    ActorCommonData * ptACDSource = ACDTryToGet(idACDSource);
    if (ptACDSource)
    {

        float flProjectileSpeedIncreasePercent = ptACDSource->AttributesGetFloat(FastAttribKey(ATTRIBUTE_PROJECTILE_SPEED_INCREASE_PERCENT));
        flProjectileSpeed += flProjectileSpeedIncreasePercent * flProjectileSpeed * 0.01f;
    }

    tProjectile.tProjectileData.tAimingData.tDest = tTarget;
    tProjectile.tProjectileData.tAimingData.wvTargetVel = cgwvNull;
    tProjectile.tProjectileData.tAimingData.wdExtraDistanceOnFirstStep = wdExtraDistanceOnFirstStep;
    tProjectile.tProjectileData.tAnimationData.wdSpeed = flProjectileSpeed;
    tProjectile.tProjectileData.tAnimationData.placeStart = tNavPlace.tPlace;
    tProjectile.tProjectileData.tDurationData.dwTime = (uint32)(flDistanceToTravel / flProjectileSpeed);
    tProjectile.tProjectileData.tRenderingData.snoPR = ptPower->snoProjectile;
    tProjectile.tProjectileData.tRenderingData.wdRopeLength = 0.0f;
    tProjectile.tProjectileData.tResponseData.flUnitBounceSpeed = 0.0f;
    tProjectile.tProjectileData.idACDSnapSource = idACDSource;
    tProjectile.tProjectileData.nSnapHardPoint = -1;
    tProjectile.tProjectileData.eDPath = 0;
}



void PowerHelper_ProjectileInitForScriptedLobbedProjectile(PowerID idPower,
                                                           ACDID idACDSource,
                                                           const NavPlace & tNavPlace,
                                                           const Target & tTarget,
                                                           const SpellEffect eSpellEffect,
                                                           float flGravity,
                                                           float flHeightAboveSource,
                                                           float flUnitBounceSpeed,
                                                           float flLifetimeInSecs,
                                                           ProjectileHitType eHitType,
                                                           BOOL bDontDestroyOnImpact,
                                                           BOOL bDontPerformMarbleRollToCaster,
                                                           BOOL bUseProjectileStaticFlagsForSpawn,
                                                           PowerAPICreateProjectile & tProjectile)

{
    Power * ptPower = PowerGet(idPower);
    WorldDistance wdExtraDistanceOnFirstStep = 0.0f;
    tProjectile.idPower = idPower;
    tProjectile.tProjectileData.dwFlags = 1 << PROJECTILE_DETONATE_AFTER_FIXED_TIME_BIT;
    tProjectile.tProjectileData.eSpellEffect = eSpellEffect;

    tProjectile.tProjectileData.snoPower = ptPower->snoPower;

    tProjectile.tProjectileData.tAimingData.eAimMethod = AIMMETHOD_FIXED_GRAVITY;
    tProjectile.tProjectileData.tAimingData.wdGravity = flGravity;
    tProjectile.tProjectileData.tAimingData.flHeightAboveSource = flHeightAboveSource;

    switch (eHitType)
    {
    case PROJECTILE_HIT_NO_ACTORS:
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_DONT_HIT_ACDS_BIT;
        break;
    case PROJECTILE_HIT_NO_MONSTERS:
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_DONT_HIT_MONSTERS_BIT;
        break;
    }
    if (bDontDestroyOnImpact) { tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_DONT_DESTROY_ON_IMPACT; } else { tProjectile.tProjectileData.dwFlags &= ~(1 << PROJECTILE_DONT_DESTROY_ON_IMPACT); }
    if (bDontPerformMarbleRollToCaster) { tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_DONT_PERFORM_MARBLE_ROLL_TO_CASTER_BIT; } else { tProjectile.tProjectileData.dwFlags &= ~(1 << PROJECTILE_DONT_PERFORM_MARBLE_ROLL_TO_CASTER_BIT); }

    if (bUseProjectileStaticFlagsForSpawn)
    {
        tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN;
    }


    tProjectile.tProjectileData.tAimingData.tDest = tTarget;
    tProjectile.tProjectileData.tAimingData.wvTargetVel = cgwvNull;
    tProjectile.tProjectileData.tAimingData.wdExtraDistanceOnFirstStep = wdExtraDistanceOnFirstStep;
    tProjectile.tProjectileData.tAnimationData.wdSpeed = 1.0f;
    tProjectile.tProjectileData.tAnimationData.placeStart = tNavPlace.tPlace;
    if (flLifetimeInSecs < EPSILON)
    {
        tProjectile.tProjectileData.tDurationData.dwTime = 75;
    }
    else
    {

        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flLifetimeInSecs * 60.0f);
    }
    tProjectile.tProjectileData.tRenderingData.snoPR = ptPower->snoProjectile;
    tProjectile.tProjectileData.tRenderingData.wdRopeLength = 0.0f;
    tProjectile.tProjectileData.tResponseData.flUnitBounceSpeed = flUnitBounceSpeed;
    tProjectile.tProjectileData.idACDSnapSource = idACDSource;
    tProjectile.tProjectileData.nSnapHardPoint = -1;
    tProjectile.tProjectileData.eDPath = 0;

    WorldPlace wpTarget;
    PowerGetTargetPlace(AutoPowerDefinitionR(ptPower->snoPower), tTarget, wpTarget);
}

void PowerHelper_ProjectileInit(PowerID idPower,
                                ACDID idACDSource,
                                const NavPlace & tNavPlace,
                                const Target & tTarget,
                                const SpellEffect eSpellEffect,
                                PowerAPICreateProjectile & tProjectile)

{
    Power * ptPower = PowerGet(idPower);
    WorldDistance wdExtraDistanceOnFirstStep = 0.0f;
    tProjectile.idPower = idPower;
    tProjectile.tProjectileData.dwFlags = 1 << PROJECTILE_DETONATE_AFTER_FIXED_TIME_BIT;
    tProjectile.tProjectileData.eSpellEffect = eSpellEffect;

    tProjectile.tProjectileData.snoPower = ptPower->snoPower;
    if (!PowerContactTagGetBool(idPower, TAG_POWER_IS_LOBBED))
    {
        tProjectile.tProjectileData.tAimingData.eAimMethod = AIMMETHOD_LINEAR;
        tProjectile.tProjectileData.tAimingData.wdGravity = 0.0f;
        tProjectile.tProjectileData.tAimingData.flHeightAboveSource = 0.0f;
    }
    else
    {
        tProjectile.tProjectileData.tAimingData.eAimMethod = AIMMETHOD_FIXED_GRAVITY;
        tProjectile.tProjectileData.tAimingData.wdGravity = PowerContactTagGetFloat(idPower, TAG_POWER_LOBBED_PROJECTILE_GRAVITY);
        tProjectile.tProjectileData.tAimingData.flHeightAboveSource = PowerGetFormulaValue(idPower, TAG_POWER_LOBBED_HEIGHT_ABOVE_SOURCE);

        if (PowerContactTagGetBool(idPower, TAG_POWER_PROJECTILE_THROW_OVER))
        {
            tProjectile.tProjectileData.dwFlags |= 1 << PROJECTILE_DONT_HIT_ACDS_BIT;
        }
    }



    float flProjectileSpeed = PowerAttributesGetFloat(idPower, FastAttribKey(ATTRIBUTE_PROJECTILE_SPEED));
    if (idACDSource != ACDID_INVALID)
    {
        ActorCommonData & tACDSource = ACDGet(idACDSource);
        float flProjectileSpeedIncreasePercent = tACDSource.AttributesGetFloat(FastAttribKey(ATTRIBUTE_PROJECTILE_SPEED_INCREASE_PERCENT));
        flProjectileSpeed += flProjectileSpeed * flProjectileSpeedIncreasePercent * 0.01f;
    }

    tProjectile.tProjectileData.tAimingData.tDest = tTarget;
    tProjectile.tProjectileData.tAimingData.wvTargetVel = cgwvNull;
    tProjectile.tProjectileData.tAimingData.wdExtraDistanceOnFirstStep = wdExtraDistanceOnFirstStep;
    tProjectile.tProjectileData.tAnimationData.wdSpeed = flProjectileSpeed;
    tProjectile.tProjectileData.tAnimationData.placeStart = tNavPlace.tPlace;
    tProjectile.tProjectileData.tDurationData.dwTime = (uint32)(75.0f / flProjectileSpeed);
    tProjectile.tProjectileData.tRenderingData.snoPR = ptPower->snoProjectile;
    tProjectile.tProjectileData.tRenderingData.wdRopeLength = 0.0f;
    tProjectile.tProjectileData.tResponseData.flUnitBounceSpeed = 0.0f;
    tProjectile.tProjectileData.idACDSnapSource = idACDSource;
    tProjectile.tProjectileData.nSnapHardPoint = -1;
    tProjectile.tProjectileData.eDPath = 0;
    SigmaMemoryClear(&tProjectile.tProjectileData.tDPathSinData, sizeof(tProjectile.tProjectileData.tDPathSinData));

    if (PowerEffectTagGetInt(idPower, TAG_POWER_PROJECTILE_SCALE_VELOCITY))
    {
        WorldPlace wpTarget;
        PowerGetTargetPlace(AutoPowerDefinitionR(ptPower->snoPower), tTarget, wpTarget);
        WorldPlace wpSource = ACDGet(ptPower->idACD).GetPlace();
        WorldDistance wdToTarget = Vector3D::Distance(wpSource.wp, wpTarget.wp);
        WorldDistance wdCurrentSpeed = tProjectile.tProjectileData.tAnimationData.wdSpeed;

        tProjectile.tProjectileData.tAnimationData.wdSpeed = Math::Lerp(wdCurrentSpeed * 0.75f, wdCurrentSpeed * 1.75f, wdToTarget / 40.0f);
    }
}




void PowerHelper_StopProjectileInit(PowerID idPower, ACDID idACDProjectile, ACDID idACDIntersected, PowerAPIDestroyProjectile & tProjectile)
{
    //
    //
    //
    //
    tProjectile.idPower = idPower;
    tProjectile.idACDProjectile = idACDProjectile;
    tProjectile.idACDIntersected = idACDIntersected;
}























void PowerHelper_ChooseOpenGroundInit(PowerID, const NavPlace & tNavPlace, PowerAPIChooseOpenGround & tOpenGroundInfo, float flSkipDistance)
{
    //
    //
    //
    //
    tOpenGroundInfo.tNavPlace = tNavPlace;
    tOpenGroundInfo.flSkipDistance = flSkipDistance;
    tOpenGroundInfo.bUseActorPassability = FALSE;
}




void PowerHelper_ChooseTargetsInit(PowerID idPower, PowerAPIChooseTargets & tTargetInfo)
{
    //
    //
    tTargetInfo.idPower = idPower;
    tTargetInfo.ePassabilityType = 2;
}




void PowerHelper_ChooseTargetsInitGeometrySingleTarget(const WorldPlace & placeSource, const Target & tTarget, const WorldDistance wdSearchRadius, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_SINGLE_TARGET;
    tGeometry.tSingleTarget.tTarget = tTarget;
    tGeometry.tSingleTarget.wdSearchRadius = wdSearchRadius;
    tGeometry.tSingleTarget.placeSource = placeSource;
}





void PowerHelper_ChooseTargetsInitGeometryMelee(const ACDID idACDSource, const Target & tTarget, int32 nMeleeRange, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_MELEE;
    tGeometry.tMelee.idACDSource = idACDSource;
    tGeometry.tMelee.tTarget = tTarget;
    tGeometry.tMelee.nMeleeRange = nMeleeRange;
}




void PowerHelper_ChooseTargetsInitGeometrySphere(const WorldPlace & placeCenter, WorldDistance wdRadius, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_SPHERE;
    tGeometry.tSphere.placeCenter = placeCenter;
    tGeometry.tSphere.wdRadius = wdRadius;
}



void PowerHelper_ChooseTargetsInitGeometryFront(const WorldPlace & placeCenter, WorldDistance wdRadius, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_FRONT;
    tGeometry.tSphere.placeCenter = placeCenter;
    tGeometry.tSphere.wdRadius = wdRadius;
}



void PowerHelper_ChooseTargetsInitGeometryBack(const WorldPlace & placeCenter, WorldDistance wdRadius, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_BACK;
    tGeometry.tSphere.placeCenter = placeCenter;
    tGeometry.tSphere.wdRadius = wdRadius;
}



void PowerHelper_ChooseTargetsInitGeometryPie(const WorldPlace & placeCenter, const Target & tTarget, WorldDistance wdRadius, WorldDistance wdHeight, int32 nDegrees, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_PIE;
    tGeometry.tPie.placeCenter = placeCenter;
    tGeometry.tPie.tTarget = tTarget;
    tGeometry.tPie.wdRadius = wdRadius;
    tGeometry.tPie.wdHeight = wdHeight;
    //
    //
    tGeometry.tPie.nDegrees = nDegrees;
    tGeometry.tPie.wvPieVector.x = 0.0f;
    tGeometry.tPie.wvPieVector.y = 0.0f;
    tGeometry.tPie.wvPieVector.z = 0.0f;
}





void PowerHelper_ChooseTargetsInitGeometryCylinder(const WorldPlace & placeCenter, const float flRadius, const float flHeight, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_CYLINDER;
    tGeometry.tCylinder.flRadius = flRadius;
    tGeometry.tCylinder.flHeight = flHeight;
    tGeometry.tCylinder.place0 = placeCenter;
}




void PowerHelper_ChooseTargetsInitGeometryCapsule(const WorldPlace & placeSource, const WorldPlace & placeTarget, WorldDistance wdRadius, float flBonusHeight, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    //
    //
    tGeometry.eType = CTT_CAPSULE;
    tGeometry.tCapsule.place0 = placeSource;
    tGeometry.tCapsule.vDir = placeTarget.wp - placeSource.wp;
    if (tGeometry.tCapsule.vDir.MagnitudeSquared() < 1e-12f)
    {
        tGeometry.tCapsule.vDir = cgwvXAxis;
        tGeometry.tCapsule.flLength = 0.0f;
    }
    else
    {
        //
        tGeometry.tCapsule.flLength = tGeometry.tCapsule.vDir.Normalize();
    }
    //
    tGeometry.tCapsule.flRadius = wdRadius;
    tGeometry.tCapsule.flBonusHeight = flBonusHeight;
}



void PowerHelper_ChooseTargetsInitGeometryRay(const WorldPlace & placeStart, const WorldPlace & placeEnd, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_RAY;
    tGeometry.tRay.placeStart = placeStart;
    tGeometry.tRay.placeEnd = placeEnd;
}


void PowerHelper_ChooseTargetsInitGeometryBox(const WorldPlace & placeCenter, const Vector3D & vDir, WorldDistance wdLength, WorldDistance wdWidth, ChooseTargetGeometry & tGeometry)
{
    //
    //
    //
    //
    //
    SigmaMemoryClear(&tGeometry, sizeof(tGeometry));
    //
    tGeometry.eType = CTT_BOX;
    tGeometry.tBox.placeCenter = placeCenter;
    tGeometry.tBox.vDir = vDir;
    //
    if (tGeometry.tBox.vDir.MagnitudeSquared() < 1e-12f)
    {
        tGeometry.tBox.vDir = cgwvXAxis;
    }
    else
    {
        //
        tGeometry.tBox.vDir.Normalize();
    }
    //
    //
    tGeometry.tBox.flLength = wdLength;
    tGeometry.tBox.flWidth = wdWidth;
}


void PowerHelper_ChooseTargetsInitFromPayload(PowerID idPower,
                                              const WorldPlace & placeSource,
                                              const Target & tTarget,
                                              PowerAPIChooseTargets & tTargetInfo)

{
    PowerHelper_ChooseTargetsInit(idPower, tTargetInfo);
    Power * ptPower = PowerGet(idPower);

    SNO snoPower = ptPower->snoPower;
    PayloadType ePayload = PowerGetPayload(idPower);
    float flPayloadParam = PowerGetPayloadParam0(idPower);

    switch (ePayload)
    {
    case 1:
        {
            if (ptPower->tTarget.IsNone())
            {
                return;
            }




            WorldPlace placeTarget;
            PowerGetTargetPlace(AutoPowerDefinitionR(snoPower), tTarget, placeTarget);
            PowerHelper_ChooseTargetsInitGeometryRay(placeSource, placeTarget, tTargetInfo.tGeometry);
            tTargetInfo.dwFlags = 1;
        }
        break;

    case 0:
        PowerHelper_ChooseTargetsInitGeometrySingleTarget(placeSource, tTarget, flPayloadParam, tTargetInfo.tGeometry);
        tTargetInfo.dwFlags = 1;
        break;

    case 5:
        {
            int32 nMeleeRange = Math::FloatRoundToInt(flPayloadParam);
            PowerHelper_ChooseTargetsInitGeometryMelee(ptPower->idACD, tTarget, nMeleeRange, tTargetInfo.tGeometry);
        }
        break;





    case 2:
    case 3:
        PowerHelper_ChooseTargetsInitGeometrySphere(placeSource, flPayloadParam, tTargetInfo.tGeometry);
        break;







    case 4:
        {
            WorldPlace placeTarget;
            PowerGetTargetPlace(AutoPowerDefinitionR(snoPower), tTarget, placeTarget);
            Vector3D wvSourceToTarget;
            WorldPlaceGetVectorBetween(placeSource, placeTarget, wvSourceToTarget);
            wvSourceToTarget.z = 0.0f;
            wvSourceToTarget.Normalize();


            WorldPlaceOffset(placeSource, wvSourceToTarget * flPayloadParam, placeTarget);

            PowerHelper_ChooseTargetsInitGeometryRay(placeSource, placeTarget, tTargetInfo.tGeometry);
        }
        break;

    case 6:
        PowerHelper_ChooseTargetsInitGeometryFront(placeSource, flPayloadParam, tTargetInfo.tGeometry);
        break;


    case 7:
        PowerHelper_ChooseTargetsInitGeometryBack(placeSource, flPayloadParam, tTargetInfo.tGeometry);
        break;


    case 8:
        {
            int32 nDegrees = Math::FloatToInt(PowerGetPayloadParam1(idPower));
            if (nDegrees == 0)
            {
                nDegrees = 60;
            }
            ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
            WorldDistance wdHeight = 10.0f;
            if (ptACDOwner)
            {
                wdHeight = ptACDOwner->GetHeight();
            }

            PowerHelper_ChooseTargetsInitGeometryPie(placeSource, tTarget, flPayloadParam, wdHeight, nDegrees, tTargetInfo.tGeometry);

            if (PowerContactTagGetBool(idPower, TAG_POWER_PAYLOAD_PIE_USE_ACTOR_FACING) && ptACDOwner)
            {


                tTargetInfo.tGeometry.tPie.wvPieVector = ptACDOwner->GetForward();
            }
        }
        break;





    case 9:
        {
            WorldPlace placeTarget;
            PowerGetTargetPlace(AutoPowerDefinitionR(snoPower), tTarget, placeTarget);

            PowerHelper_ChooseTargetsInitGeometryCapsule(placeSource, placeTarget, flPayloadParam, 0.0f, tTargetInfo.tGeometry);
        }
        break;


    case 10:
        {
            float flWidth = PowerGetPayloadParam1(idPower);
            if (flWidth == 0.0f)
            {
                flWidth = flPayloadParam;
            }
            WorldPlace placeTarget; PowerGetTargetPlace(AutoPowerDefinitionR(snoPower), tTarget, placeTarget);
            Vector3D vDir = placeTarget.wp - placeSource.wp;

            PowerHelper_ChooseTargetsInitGeometryBox(placeSource, vDir, flPayloadParam, flWidth, tTargetInfo.tGeometry);
        }
        break;
    default:
        DisplayInternalError(NULL, "", 1194, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




void PowerHelper_ChooseTargetsInitAroundSourceFromPayload(PowerID idPower, PowerAPIChooseTargets & tTargetInfo)
{
    //
    //
    //
    Power * ptPower = PowerGet(idPower);

    //
    NavPlace tNavPlace;
    PowerHelperGetNavPlaceFromPowerSource(idPower, tNavPlace);
    //
    PowerHelper_ChooseTargetsInitFromPayload(idPower, tNavPlace.tPlace, ptPower->tTarget, tTargetInfo);
}





void PowerHelper_ChooseTargetsInitAroundTargetFromPayload(PowerID idPower, PowerAPIChooseTargets & tTargetInfo)
{
    //
    //
    //
    //
    NavPlace tNavPlaceTargetArea;
    //
    BOOL bSuccess = PowerHelperRollToPowerTarget(idPower, 2, tNavPlaceTargetArea);
    //
    if (!bSuccess)
    {
        //
        //
        //
        //
        PowerHelper_ChooseTargetsInit(idPower, tTargetInfo);
        tTargetInfo.tGeometry.eType = CTT_NO_LINE_OF_SIGHT;
        //
        //
        //
        return;
    }
    Power * ptPower = PowerGet(idPower);
    PowerHelper_ChooseTargetsInitFromPayload(idPower, tNavPlaceTargetArea.tPlace, ptPower->tTarget, tTargetInfo);
}

void PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(PowerID idPower, const Target & tTargetBeingShotAt, PowerAPIChooseTargets & tTargetInfo)
{
    //
    //
    //
    WorldPlace placeTarget;
    tTargetBeingShotAt.GetPlace(placeTarget);
    PowerHelper_ChooseTargetsInitFromPayload(idPower, placeTarget, tTargetBeingShotAt, tTargetInfo);
}




void PowerHelper_ExplosionInit(PowerID idPower, const WorldPlace & placeSource, PowerAPIExplosion & tExplosion)
{
    //
    //
    //
    tExplosion.snoExplosionActor = PowerGetExplosionSNO(idPower);
    tExplosion.qOrientation = cgqIdentity;
    tExplosion.placeSource = placeSource;
    tExplosion.dwDuration = TICKS_PER_SECOND;
}




void PowerHelper_ExplosionProjectileInit(PowerID idPower, const WorldPlace & placeSource, XPooledDataIDList & listTargets, const ImpactData * pImpactData, PowerAPIExplosion & tExplosion)
{
    //
    //
    //
    //
    //
    PowerHelper_ExplosionInit(idPower, placeSource, tExplosion);

    SNO snoWallExplosion = PowerEffectTagGetSNO(idPower, TAG_POWER_WALL_FLOOR_EXPLOSION_SNO);
    if (listTargets.Count() == 0)
    {
        if (pImpactData->idACDIntersected == ACDID_INVALID && snoWallExplosion != SNO_NONE)
        {
            //
            //
            tExplosion.snoExplosionActor = snoWallExplosion;
        }
    }
}


void PowerHelper_HitSoundInit(Power * ptPower)
{
    //
    if (PowerEffectTagExists(ptPower->id, TAG_POWER_HITSOUND_OVERRIDE))
    {
        ptPower->eHitsound = PowerEffectTagGetInt(ptPower->id, TAG_POWER_HITSOUND_OVERRIDE);
        return;
    }

    //
    ActorCommonData & tACD = ACDGet(ptPower->idACD);
    ACDID idACDActiveWeapon = ACDInventoryGetActiveWeapon(tACD);
    //
    ActorCommonData * ptACDActiveWeapon = ACDTryToGet(idACDActiveWeapon);
    if (ptACDActiveWeapon && !ItemIsBroken(*ptACDActiveWeapon))
    {
        SNO snoWeaponActor = ACDGet(idACDActiveWeapon).snoActor;
        ScopedActorDefinitionLockR pdefActor(snoWeaponActor);
        ptPower->eHitsound = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_WEAPON_HITSOUND);
        //
        //
        //
        //
    }
    else if (ACDIsPlayer(tACD.id) || ACDIsHireling(tACD))
    {
        ptPower->eHitsound = 3;
    }
    else
    {
        //
        ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
        ptPower->eHitsound = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_WEAPON_HITSOUND);
    }
}

void PowerHelper_AnimationInit(PowerID idPower, PowerAPIAnimation & tAnimation)
{
    //
    //
    Power * ptPower = PowerGet(idPower);
    tAnimation.idPower = ptPower->id;
    tAnimation.idACDSource = ptPower->idACD;
    tAnimation.tTarget = ptPower->tTarget;
    tAnimation.nCooldown = 0;
    tAnimation.bSetAimTarget = TRUE;
    tAnimation.dwAnimationTag = ptPower->dwAnimationTag;
    tAnimation.dwDuration = (uint32)-1;
    //
    tAnimation.flSpeedScalar = PowerGetAnimationSpeedForPower(ptPower->idACD, AutoPowerDefinitionR(ptPower->snoPower), ptPower->nComboLevel);
    //
    //
    SigmaMemoryClear(&tAnimation.tPreplayedAnimData, sizeof(tAnimation.tPreplayedAnimData));
    //
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD &&
        ptACD->IsGizmo())
        tAnimation.bTurnToFaceTarget = FALSE;
    else
        tAnimation.bTurnToFaceTarget = TRUE;
}







BOOL PowerHelper_DoFirstTimeCosts(PowerID idPower, BOOL bWantCooldown)
{
    //
    //
    Power * ptPower = PowerGet(idPower);
    //
    if (ptPower->dwFlags & (1 << PF_INITAL_COSTS_PAID))
    {
        return TRUE;
    }
    ptPower->dwFlags |= (1 << PF_INITAL_COSTS_PAID);
    //
    if (ACDTryToGet(ptPower->idACD) == NULL)
    {
        return FALSE;
    }

    //
    PowerAPICosts tCosts;
    PowerHelper_CostsInit(idPower, tCosts);
    BOOL bSuccess = PowerAPI_DoCosts(tCosts, 1.0f);
    //
    if (bWantCooldown && bSuccess)
    {
        PowerHelper_AddCooldown(idPower);
    }
    //
    return bSuccess;
}



void PowerHelper_AddCooldown(PowerID idPower)
{
    //
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower && !ptPower->bFreeCast)
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
        if (ptACD)
        {
            //
            uint32 dwCooldownTicks = PowerGetCastingCooldown(ptPower->snoPower, *ptACD);
            PowerHelper_AddCooldown(*ptACD, ptPower->snoPower, dwCooldownTicks, FALSE);
        }
    }
}



void PowerHelper_RefundCosts(PowerID idPower)
{
    //
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL)
    {
        return;
    }
    //
    //
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD == NULL)
    {
        return;
    }
    //
    SNO snoPower = ptPower->snoPower;
    ResourceType eResourceType = PowerGetResourceType(*ptACD, snoPower);
    //
    float flResourceCost = PowerGetResourceCost(idPower);
    //
    float flHealthCostPercentOfMaxHealth = PowerGetFormulaValue(ptACD,
                                                                snoPower,
                                                                TAG_POWER_COST_HEALTH,
                                                                ptPower->nSkillLevel,
                                                                //
                                                                PowerGetAttributeGroup(idPower), 0);
    float flHealthCost = flHealthCostPercentOfMaxHealth * ptACD->AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
    //
    //
    if (flResourceCost > EPSILON)
    {
        //
        float flCostReductionPercent = PowerGetCostReductionPercent(*ptACD, eResourceType, PowerGetDamageType(ptPower), snoPower);
        flResourceCost *= flCostReductionPercent;
        //
        //
        float flCostReduction = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_TOTAL, eResourceType) + ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION, snoPower));
        float flPowerCostReductionCoefficient = PowerGetFormulaValue(idPower, TAG_POWER_COST_RESOURCE_REDUCTION_COEFFICIENT);
        flResourceCost -= flCostReduction * flPowerCostReductionCoefficient;
        //
        ptACD->AddResource(eResourceType, flResourceCost, FALSE);
    }
    //
    if (flHealthCost > EPSILON)
    {
        float flHPCurrent = ptACD->GetHitPointsCurrent();
        flHPCurrent += flHealthCost;
        ptACD->SetHitPoints(flHPCurrent, TRUE, TRUE);
    }
    //
    SNO snoPowersCooldown = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
    //
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos)
    {
        //
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        //
        if (ptBuff && !ptBuff->bDeleted && ptBuff->snoPower == snoPowersCooldown &&
            FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower)) > 0)
        {
            //
            PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, 0, TRUE);
        }
    }
}

static void sReapplyPassives(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    //
    //
    //
    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    //
    //
    //
    //
    //
    if (ptACD)
        ptACD->ApplyAllPassives(FALSE);
}


void PowerHelper_AddCooldown(ActorCommonData & tACD, SNO snoPower, uint32 uCooldownTicks, BOOL bReapplyPassivesWhenFinished)
{
    //
    //
    //
    //
    if (uCooldownTicks > 0 && !ActorFlagIsSet((ActorID)ACDGetOwner(tACD.id), ACTOR_GOD_MODE_BIT))
    {
        uint32 dwNow = GameGetServerTime();
        const int32 nMaxCooldownStackCount = 1024;
        //
        PowerAPIBuffStartData tBuffStartData(POWER_NONE);
        tBuffStartData.idACDSource = tACD.id;
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
        tBuffStartData.dwDurationMin = uCooldownTicks;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.eStackType = 1;
        tBuffStartData.nMaxStackCount = nMaxCooldownStackCount;
        tBuffStartData.bRemoveOnDeath = FALSE;
        if (bReapplyPassivesWhenFinished)
        {
            //
            tBuffStartData.pRemovalEffect = sReapplyPassives;
            tBuffStartData.pRemovalEffectData = NULL;
        }
        //
        //
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower), dwNow + uCooldownTicks);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, snoPower), dwNow);
        //
        PowerAPI_BuffAddToACD(tBuffStartData, tACD);
    }
}


void PowerHelper_CostsInit(PowerID idPower, PowerAPICosts & tCosts)
{
    //
    //
    Power * ptPower = PowerGet(idPower);

    tCosts.idPower = idPower;
    tCosts.idACD = ptPower->idACD;
}





void PowerHelper_PopulateCommonAttributes(const PowerID idPower, AttribGroupID idFastAttrib)


{
    SNO snoPower = PowerGetSNO(idPower);

    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);

    for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++)
    {
        float flDamageMin = PowerGetFormulaValue(ptACDOwner, AutoPowerDefinitionR(snoPower), PowerTagGetDamageMinTag(iDamageType), -1, -1, 0);
        float flDamageDelta = PowerGetFormulaValue(ptACDOwner, AutoPowerDefinitionR(snoPower), PowerTagGetDamageDeltaTag(iDamageType), -1, -1, 0);

        if (flDamageMin != 0.0f || flDamageDelta != 0.0f)
        {
            FastAttribSetValueFloat(idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_POWER_MIN, iDamageType), flDamageMin);
            FastAttribSetValueFloat(idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_POWER_DELTA, iDamageType), flDamageDelta);
        }
    }

    float flProjectileSpeed = PowerGetFormulaValue(ptACDOwner, AutoPowerDefinitionR(snoPower), TAG_POWER_PROJECTILE_SPEED, -1, -1, 0);
    FastAttribSetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_PROJECTILE_SPEED), flProjectileSpeed);

    float nBleedDuration = PowerGetFormulaValue(ptACDOwner, AutoPowerDefinitionR(snoPower), TAG_POWER_BLEED_DURATION, -1, -1, 0);
    FastAttribSetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_BLEED_DURATION), nBleedDuration);
}




void PowerHelper_UpdateTargetStandard(const PowerID idPower, const UpdateTargetData * tUpdateTargetData)
{
    //
    //
    Power * ptPower = PowerGet(idPower);
    //
    //
    if (!ptPower->tTarget.IsTargetingACD())
    {
        //
        ptPower->tTarget = tUpdateTargetData->tNewTarget;
    }
}



BOOL PowerHelper_IsValidOwner(const PowerID idPower)
{
    //
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    //
    if (ptACD != NULL)
    {
        return TRUE;
    }
    //
    //
    //
    return FALSE;
}


BOOL PowerHelperCheckLineOfSiteOnTarget(PowerID idPower, PassabilityType ePassabilityType)
{
    //
    //
    Power * ptPower = PowerGet(idPower);
    NavPlace tNavPlace;
    //
    BOOL bLOS = PowerRollToTargetNavPlace(ptPower->idACD,
                                          ptPower->tTarget,
                                          ePassabilityType, FALSE, tNavPlace);
    //
    //
    //
    //
    return bLOS;
}




void PowerHelperGetNavPlaceFromPowerSource(PowerID idPower, NavPlace & tNavPlace)
{
    //
    //
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        PowerGetNavPlaceFromACD(ptACDOwner, tNavPlace);
    }
    else
    {
        //
        //
        Power * ptPower = PowerGet(idPower);
        tNavPlace.tPlace = ptPower->wpStartLocation;
        ScenesCommonNavMeshNodeFromPlace(ptPower->wpStartLocation, &tNavPlace.tNavMeshHandle, TRUE, FALSE);
    }
}


BOOL PowerHelperRollToPowerTarget(PowerID idPower, const PassabilityType ePassabilityType, NavPlace & tNavPlace)
{
    //
    //
    //
    //
    Power * ptPower = PowerGet(idPower);
    //
    BOOL bResult = PowerRollToTargetNavPlace(ptPower->idACD,
                                             ptPower->tTarget,
                                             ePassabilityType, FALSE, tNavPlace);
    //
    return bResult;
}





SNO PowerHelper_GetAnimForPowerOwner(const PowerID idPower)
{
    //
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
    {
        //
        //
        //
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            //
            //
            //
            uint32 dwAnimTag = ptPower->dwAnimationTag;
            return ActorGetAnim(ptACDOwner->snoActor, ptACDOwner->GetWeaponClassForAnim(NULL), dwAnimTag);
        }
    }
    return SNO_NONE;
}
BOOL PowerHelper_SetDefaultContactAnimations(PowerID idPower, uint32 dwAnimationTag)
{
    //
    //
    Power * ptPower = PowerGet(idPower);
    if (ptPower && !PowerHasScaledAnimationTiming(ptPower->snoPower))
    {
        //
        //
        //
        //
        ACDID idACDMainActor = ptPower->idACD;
        ActorCommonData * ptACDMainActor = ACDTryToGet(idACDMainActor);
        if (ptACDMainActor)
        {
            //
            AnimationTiming tAnimationTiming;
            AnimPlayerGetAnimationTiming(*ptACDMainActor, dwAnimationTag, ANIM_USE_ANIM_LENGTH, 1.0f, SGameGetRNG(), tAnimationTiming);
            //
            ptPower->nContactAnimationPermutation[0] = tAnimationTiming.tAnimSpec[0].nPermutationIndex;
            ptPower->snoContactAnimation[0] = tAnimationTiming.tAnimSpec[0].snoAnim;
            ptPower->nContactAnimationPermutation[1] = tAnimationTiming.tAnimSpec[1].nPermutationIndex;
            ptPower->snoContactAnimation[1] = tAnimationTiming.tAnimSpec[1].snoAnim;
            ptPower->nContactAnimationPermutation[2] = tAnimationTiming.tAnimSpec[2].nPermutationIndex;
            ptPower->snoContactAnimation[2] = tAnimationTiming.tAnimSpec[2].snoAnim;
            return TRUE;
        }
    }
    //
    return FALSE;
}

void PowerHelper_GetContactOffsetForPower(const PowerID idPower, const int32 nContactFrame, Vector3D & wpContactOffset, BOOL bWantWorldPosition)
{
    //
    //
    //
    //
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL)
    {
        //
        return;
    }
    //
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (ptACDOwner == NULL)
    {
        //
        return;
    }

    //
    SNO snoAnim;
    uint32 dwAnimPerm;
    //
    ContactFrameType eContactFrameType = PowerGetContactFrameType(ptPower->snoPower, nContactFrame);
    switch (eContactFrameType)
    {
        //
        case 2:
            snoAnim = ptPower->snoContactAnimation[1];
            dwAnimPerm = ptPower->nContactAnimationPermutation[1];
            break;
        //
        case 0:
            snoAnim = ptPower->snoContactAnimation[0];
            dwAnimPerm = ptPower->nContactAnimationPermutation[0];
            break;
        //
        case 1:
            snoAnim = ptPower->snoContactAnimation[2];
            dwAnimPerm = ptPower->nContactAnimationPermutation[2];
            break;
        //
        default:
            snoAnim = SNO_NONE;
            dwAnimPerm = 0;
            break;
    }

    //
    ptACDOwner->GetContactOffsetForAnim(snoAnim, wpContactOffset, nContactFrame, dwAnimPerm, bWantWorldPosition);
}







































































void PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(const ActorCommonData * ptACDPlayer, const SNO snoPower, WorldPlace & wpTarget, const BOOL bFromPlayersFeet)
{
    //
    //
    //
    //
    if (ptACDPlayer)
    {
        //
        ScopedPowerDefinitionLockR pdefPower(snoPower);
        PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(ptACDPlayer, pdefPower, wpTarget, bFromPlayersFeet);
    }
}


void PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(const ActorCommonData * ptACDPlayer, const PowerDefinition * pdefPower, WorldPlace & wpTarget, const BOOL bFromPlayersFeet)
{
    //
    //
    //
    //
    if (ptACDPlayer)
    {
        //
        uint32 dwAnimationTag = PowerDetermineAnimation(pdefPower, ptACDPlayer->id, cgTargetNone, FALSE);
        WeaponClass eWeaponClass = ptACDPlayer->GetWeaponClassForAnim(NULL);
        SNO snoAnim = ActorGetAnim(ptACDPlayer->snoActor, eWeaponClass, dwAnimationTag);
        //
        if (snoAnim != SNO_NONE)
        {
            //
            //
            Vector3D wdContactOffset;
            ptACDPlayer->GetContactOffsetForAnim(snoAnim, wdContactOffset, 0, 0, FALSE);
            //
            if (bFromPlayersFeet)
            {
                //
                wpTarget.wp.z = ptACDPlayer->GetPlace().wp.z;
            }
            //
            wpTarget.wp.z += wdContactOffset.z;
        }
    }
}
void PowerHelper_ClipTargetMessageToPowerAttackRadius(const ACDID idACDPlayer, TargetMessage * ptTargetMsg)


{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptTargetMsg && ptACDPlayer)
    {
        SNO snoTargetingPassability = PowerGetTargetingPassabilitySNOBySNO(ptTargetMsg->snoPower);
        SNO snoPower = ptTargetMsg->snoPower;
        SNO snoPowerPassability = (snoTargetingPassability != SNO_NONE) ? snoTargetingPassability : snoPower;

        WorldPlace wpPlayer = ptACDPlayer->GetPlace();
        WorldPlace wpTarget;
        ActorCommonData * ptACDTarget = ACDTryToGet(ACDFromANN(ptTargetMsg->annTarget));
        if (ptTargetMsg->eType == TT_POINT || !ptACDTarget)
        {
            wpTarget = ptTargetMsg->tPlace;
        }
        else if (ptTargetMsg->eType == TT_ACTOR)
        {
            wpTarget = ptACDTarget->GetPlace();
        }

        float flPowerRadius = PowerGetRange(ptACDPlayer, AutoPowerDefinitionR(snoPower), TRUE);
        float flPowerRadiusSq = flPowerRadius * flPowerRadius;
        float flDistSqToTarget = Vector3D::DistanceSquared(wpTarget.wp, wpPlayer.wp);
        if (flPowerRadius > EPSILON && flDistSqToTarget > flPowerRadiusSq)
        {

            if (ptTargetMsg->eType == TT_ACTOR)
            {
                ptTargetMsg->eType = TT_POINT;
            }

            Vector3D wvToTarget = wpTarget.wp - ptACDPlayer->GetPlace().wp;
            wvToTarget.Normalize();
            wvToTarget *= flPowerRadius;


            ptTargetMsg->tPlace.idSWorld = wpPlayer.idSWorld;
            ptTargetMsg->tPlace.wp = wpPlayer.wp + wvToTarget;

            if (PowerTargetNavMeshOnly(AutoPowerDefinitionR(snoPower)))
            {
                NavMeshHandle tNavMeshHandle;
                BOOL bOnMesh = ScenesCommonNavMeshNodeFromPlace(ptTargetMsg->tPlace, &tNavMeshHandle, FALSE, FALSE);
                uint32 dwNavStaticFlags = 0;


                if (bOnMesh)
                {
                    dwNavStaticFlags = NavMeshHandleToStaticFlags(tNavMeshHandle, FALSE);
                }

                if (!dwNavStaticFlags)
                {


                    const ActorMovement * ptACM = ptACDPlayer->GetActorMovement();
                    if (ptACM)
                    {
                        WorldPlace tPlaceFound;
                        PowerComputeDestination(snoPower, *ptACM, ptTargetMsg->tPlace, wpPlayer, tPlaceFound, FALSE, 10.0f);
                        ptTargetMsg->tPlace = tPlaceFound;
                    }
                }
            }
            else if (PowerTargetPathableOnly(AutoPowerDefinitionR(snoPower)))
            {
                const ActorMovement * ptMove = ptACDPlayer->GetActorMovement();
                if (ptMove)
                {
                    PathPoint2DList listPathPoints;
                    listPathPoints.SetPool(NavCellPathGetPointPool());

                    ACDCollSettings tCollSettings;
                    ptMove->GetCollSettings(tCollSettings);

                    BOOL bPathValid = TRUE;
                    PathResult eResult = ptMove->FindPath(ptTargetMsg->tPlace, listPathPoints, tCollSettings.uStaticOneRequiredFlags, tCollSettings.uStaticPreventFlags, NULL);
                    if (eResult == PATH_RESULT_NOT_FOUND) { bPathValid = FALSE; } else
                    {
                        POS pos = listPathPoints.GetHeadPosition();

                        float flTotalDist = 0.0f;
                        Vector2D vLastPoint(wpPlayer.wp.x, wpPlayer.wp.y);
                        while (pos)
                        {
                            Vector2D & vPoint = listPathPoints.GetNext(pos);
                            Vector2D vDelta = vPoint - vLastPoint;
                            flTotalDist += vDelta.Magnitude();
                            vLastPoint = vPoint;













                            if (flTotalDist >= flPowerRadius) { bPathValid = FALSE; break; }
                        } }
                    if (!bPathValid) { WorldPlace tPlaceFound;
                        PowerComputeDestination(snoPowerPassability, *ptMove, ptTargetMsg->tPlace, wpPlayer, tPlaceFound, FALSE, 10.0f);
                        ptTargetMsg->tPlace = tPlaceFound;
                    }
                }
            }
            else if (PowerTargetsContactPlaneOnly(AutoPowerDefinitionR(snoPower)))
            {


                uint32 dwAnimationTag = PowerDetermineAnimation(AutoPowerDefinitionR(snoPower), ptACDPlayer->id, cgTargetNone, FALSE);
                WeaponClass eWeaponClass = ptACDPlayer->GetWeaponClassForAnim(NULL);
                SNO snoAnim = ActorGetAnim(ptACDPlayer->snoActor, eWeaponClass, dwAnimationTag);

                if (snoAnim != SNO_NONE)
                {
                    Vector3D wdContactOffset;
                    ptACDPlayer->GetContactOffsetForAnim(snoAnim, wdContactOffset, 0, 0, FALSE);

                    WorldPlace wpContactOffset = wpPlayer;
                    wpContactOffset.wp += wdContactOffset;

                    Vector3D wvToTargetFromContactOffset = wpTarget.wp - wpContactOffset.wp;
                    wvToTargetFromContactOffset.Normalize();
                    wvToTargetFromContactOffset *= flPowerRadius;

                    ptTargetMsg->tPlace.wp = wpContactOffset.wp + wvToTargetFromContactOffset;
                    ptTargetMsg->tPlace.idSWorld = wpContactOffset.idSWorld;
                }
            }
        }
    }
}




BOOL PowerHelper_ComputeDestinationFromBadLocation(ACDID idACDPlayer,
                                                   const Vector3D & vPosBad,
                                                   Vector3D & vPosNew,
                                                   BOOL bSkipRedNavMeshRoll)

{
    vPosNew = vPosBad;

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer && ptACDPlayer->IsInWorld())
    {
        WorldPlace placePlayer = ptACDPlayer->GetPlace();
        NavCellHandle hNavCellStart;
        WorldPlace placeBad;
        placeBad.idSWorld = placePlayer.idSWorld;
        placeBad.wp = vPosBad;
        if (NavCellHandleFromWorldPlace(placeBad, hNavCellStart))
        {
            Circle circleStart;
            ActorMovement * ptActorMovement = ptACDPlayer->GetActorMovement();
            float flRadius = 0.0f;
            if (ptActorMovement)
            {
                WorldPlace placeMover;
                ptActorMovement->GetPlaceAndRadius(placeMover, flRadius, TRUE);

                circleStart.Set(vPosBad.As2D(), flRadius);
                WorldCollParams tWorldCollParamsDuring;
                tWorldCollParamsDuring.uStaticPreventFlags = 1;
                WorldCollResult tResultDuring;

                if (bSkipRedNavMeshRoll)
                {
                    tResultDuring.wpEnd = placeBad.wp;
                }
                else
                {
                    WorldCollSweptCircle(placePlayer.idSWorld, hNavCellStart, circleStart, placePlayer.wp.As2D(), tWorldCollParamsDuring, 0x13, 1, idACDPlayer, tResultDuring);
                }











                Vector3D vDelta = placePlayer.wp - tResultDuring.wpEnd;
                vDelta.z = 0.0f;
                WorldPlace placeSearch;
                placeSearch.idSWorld = placePlayer.idSWorld;
                placeSearch.wp = placePlayer.wp;

                if (flRadius > EPSILON && vDelta.MagnitudeSquared() >= flRadius * flRadius)
                {
                    vDelta.Normalize();
                    vDelta *= flRadius + 2.0f;

                    placeSearch.wp = tResultDuring.wpEnd + vDelta;
                }

                Vector3D vPosFinal;
                uint16 uOneRequiredFlags = 1;
                uint32 uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(1)
                                               | 0x860;
                BOOL bFoundLoc = WorldCollSearchForLocationOnLineSegment(placeSearch,
                                                                         uOneRequiredFlags,
                                                                         uObjectNoOverlapFlags,

                                                                         flRadius,
                                                                         placePlayer.wp,
                                                                         ptACDPlayer->id,
                                                                         6.0f,
                                                                         16,
                                                                         vPosFinal,
                                                                         ptACDPlayer->GetTeam());

                if (bFoundLoc)
                {

                    WorldPlace placeFinal;
                    placeFinal.idSWorld = placePlayer.idSWorld;
                    placeFinal.wp = vPosFinal;
                    NavCellHandle hNavCellFinal;
                    if (NavCellHandleFromWorldPlace(placeFinal, hNavCellFinal))
                    {
                        ACDCollSettings tCollSettings;
                        ptActorMovement->GetCollSettings(tCollSettings);
                        WorldCollParams tWorldCollParams(tCollSettings);
                        WorldCollResult tResultFinal;
                        Circle circleFinal;
                        circleFinal.Set(vPosFinal.As2D(), flRadius);
                        WorldCollSweptCircle(placeFinal.idSWorld, hNavCellFinal, circleFinal, tResultDuring.wpEnd.As2D(), tWorldCollParams, 3, 1, idACDPlayer, tResultFinal);










                        WorldPlace wpGroundEndPlace;
                        wpGroundEndPlace.idSWorld = placePlayer.idSWorld;
                        wpGroundEndPlace.wp = tResultFinal.wpEnd;
                        ptActorMovement->SnapPlaceToGround(wpGroundEndPlace);

                        vPosNew = tResultFinal.wpEnd;

                        vPosNew.z = wpGroundEndPlace.wp.z;
                        return TRUE;
                    }
                }
                else
                {




                    vPosNew = placePlayer.wp;
                }
            }
        }
    }


    return FALSE;
}



void PowerHelper_CheckPathAndClipToPathableAttackRadius(const ACDID idACDPlayer,
                                                        TargetMessage * ptTargetMsg)
{
    if (idACDPlayer == ACDID_INVALID || !ptTargetMsg)
    {
        return;
    }


    ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
    if (!ptACD)
    {
        return;
    }

    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (!ptMove)
    {
        return;
    }

    SNO snoTargetingPassability = PowerGetTargetingPassabilitySNOBySNO(ptTargetMsg->snoPower);
    SNO snoPower = ptTargetMsg->snoPower;
    SNO snoPowerPassability = (snoTargetingPassability != SNO_NONE) ? snoTargetingPassability : snoPower;

    ActorCollisionFlags tDestFlags;
    BOOL bHasFlags = PowerGetCollFlags(AutoPowerDefinitionR(snoPowerPassability), 1, tDestFlags);
    if (!bHasFlags)
    {
        return;
    }


    ActorCollisionFlags tCollFlags;
    ACDCollParams tACDCollParams;
    if (PowerGetCollFlags(AutoPowerDefinitionR(snoPowerPassability), 1, tCollFlags))
    {
        tACDCollParams.uObjectNoOverlapFlags = tCollFlags.uDynamicFlagsNoOverlap;
    }
    else
    {

        ACDCollSettings tCollSettings;
        ACDCollGetSettings(ptACD->snoActor, tCollSettings);
        tACDCollParams.uObjectNoOverlapFlags = tCollSettings.uDynamicFlagsNoOverlap;
    }
    tACDCollParams.uSpecialFlags |= 1;
    tACDCollParams.eTeam = ptACD->GetTeam();


    WorldPlace wpPlayer = ptACD->GetPlace();
    WorldPlace wpTarget;
    ActorCommonData * ptACDTarget = ACDTryToGet(ACDFromANN(ptTargetMsg->annTarget));
    if (ptTargetMsg->eType == TT_POINT || !ptACDTarget)
    {
        wpTarget = ptTargetMsg->tPlace;
    }
    else if (ptTargetMsg->eType == TT_ACTOR)
    {
        wpTarget = ptACDTarget->GetPlace();
    }



    float flPowerRadius = PowerGetRange(ptACD, AutoPowerDefinitionR(snoPower), TRUE);
    float flPowerRadiusSq = flPowerRadius * flPowerRadius;
    float flDistSqToTarget = Vector3D::DistanceSquared(wpTarget.wp, wpPlayer.wp);
    if (flPowerRadius > EPSILON && flDistSqToTarget > flPowerRadiusSq)
    {
        if (ptTargetMsg->eType == TT_ACTOR)
        {
            ptTargetMsg->eType = TT_POINT;
        }


        Vector3D wvToTarget = wpTarget.wp - ptACD->GetPlace().wp;
        wvToTarget.Normalize();
        wvToTarget *= flPowerRadius;

        ptTargetMsg->tPlace.idSWorld = wpPlayer.idSWorld;
        ptTargetMsg->tPlace.wp = wpPlayer.wp + wvToTarget;


        if (PowerTargetNavMeshOnly(AutoPowerDefinitionR(snoPower)))
        {
            NavPlace tNavPlaceSource;
            tNavPlaceSource.tPlace = ptTargetMsg->tPlace;
            ScenesCommonNavMeshNodeFromPlace(ptTargetMsg->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);

            if (!NavMeshHandleIsNone(tNavPlaceSource.tNavMeshHandle) &&
                NavMeshInstGetByScene(tNavPlaceSource.tNavMeshHandle.idSScene))
            {
                ptTargetMsg->tPlace.wp.z = NavMeshGetHeightWorldSpace(tNavPlaceSource.tNavMeshHandle);
            }
        }
        else if (PowerTargetGroundOnly(AutoPowerDefinitionR(snoPower)))
        {
            ptTargetMsg->tPlace.wp.z = ptACD->GetPlace().wp.z;
        }
    }




    ptTargetMsg->tPlace = PowerHelper_ComputePathableDestination(snoPowerPassability, ptACD->GetPlace(), ptTargetMsg->tPlace, ptMove->GetRadius(TRUE), *ptACD);
}




WorldPlace PowerHelper_ComputePathableDestination(const SNO snoPassability,
                                                  const WorldPlace & wpStart,
                                                  const WorldPlace & wpEnd,
                                                  const float flRadius,
                                                  ActorCommonData & tACD)

{
    WorldPlace wpResult = wpEnd;

    ActorMovement * ptMove = tACD.GetActorMovement();
    if (!ptMove)
    {
        return wpStart;
    }

    ActorCollisionFlags tDestFlags;
    BOOL bHasFlags = PowerGetCollFlags(AutoPowerDefinitionR(snoPassability), 1, tDestFlags);
    if (!bHasFlags)
    {
        return wpStart;
    }

    ActorCollisionFlags tCollFlags;
    ACDCollParams tACDCollParams;
    if (PowerGetCollFlags(AutoPowerDefinitionR(snoPassability), 1, tCollFlags))
    {
        tACDCollParams.uObjectNoOverlapFlags = tCollFlags.uDynamicFlagsNoOverlap;
    }
    else
    {

        ACDCollSettings tCollSettings;
        ACDCollGetSettings(tACD.snoActor, tCollSettings);
        tACDCollParams.uObjectNoOverlapFlags = tCollSettings.uDynamicFlagsNoOverlap;
    }
    tACDCollParams.uSpecialFlags |= 1;
    tACDCollParams.eTeam = tACD.GetTeam();



    NavCellHandle hNavCellCheck;
    NavCellHandleFromWorldPlace(wpEnd, hNavCellCheck);
    Circle wc;
    wc.Set(wpResult.wp.As2D(), flRadius);

    NavCellCollideResult tResultCircle;
    if (NavZoneCollideCircle(hNavCellCheck, wc, tCollFlags.uStaticOneRequiredFlags, tCollFlags.uStaticPreventFlags, 0, tResultCircle))
    {

        WorldPlace wpNew = wpResult;
        PowerHelper_ComputeDestinationFromBadLocation(tACD.id, wpResult.wp, wpNew.wp, FALSE);
        wpResult = wpNew;
    }


    ACDCollNarrowResult tCollResult;
    Circle tCollCircle;
    tCollCircle.Set(wpResult.wp.As2D(), ptMove->GetRadius(FALSE));
    if (ACDCollCollideCircle(wpResult.idSWorld, tCollCircle, tACDCollParams, tACD.id, tCollResult))
    {

        WorldPlace wpNew = wpResult;
        PowerHelper_ComputeDestinationFromBadLocation(tACD.id, wpResult.wp, wpNew.wp, TRUE);
        wpResult = wpNew;
    }


    tACDCollParams.uObjectNoOverlapFlags = 0x800;


    PathPoint2DList listPathPoints;
    listPathPoints.SetPool(NavCellPathGetPointPool());
    PathResult eResult = ptMove->FindPath(wpResult, listPathPoints, tCollFlags.uStaticOneRequiredFlags, tCollFlags.uStaticPreventFlags, &tACDCollParams);
    if (eResult != PATH_RESULT_NOT_FOUND)
    {
        return wpResult;
    }
    else
    {

        WorldPlace tPlaceFound;
        PowerComputeDestination(snoPassability, *ptMove, wpEnd, wpStart, tPlaceFound, FALSE, 10.0f);
        wpResult = tPlaceFound;
    }

    return wpResult;
}