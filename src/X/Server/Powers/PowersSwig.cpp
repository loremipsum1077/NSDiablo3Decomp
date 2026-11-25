// Original owner: X/Server/Powers/PowersSwig.cpp.
#include "PowersSwig.h"
#include "../../Common/Powers/PowersStruct.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/GameConstants.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Definitions/SkillKitDefinition.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Player.h"
#include "../Actors/ActorPowers.h"
#include "../Actors/Projectile.h"
#include "../Actors/Actors.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/VisualInventory.h"
#include "../../Common/Items.h"
#include "../AI/BrainComponents.h"
#include "../AI/BrainStance.h"
#include "../../Common/SceneCommon.h"
#include "../Movement/SActorMovement.h"
#include "SpellEffect.h"
#include "../../Common/Actors/ActorEffectSwig.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "PowerHelper.h"
#include "Powers.h"
#include "PowerAPI.h"
#include "../LuaScript/LuaScript.h"
#include "../SWorld.h"
#include "../SGame.h"
#include "../../Common/AnimPlayer.h"
#include "PowerTrigger.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/TagMaps/MonsterTags.h"
#include "../../Sigma/MemoryPools.h"
#include "../PlayerList.h"
#include "../SPlayer.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include <float.h>
#include "../../../Contrib/Contrib/blz/include/blz/algo_minmax.h"

// Belongs in X/Server/Powers/PowerAPI.h (line 261); same local copy as PowerHelper.cpp.
struct PowerAPIChooseOpenGround {  // size 0x20
    NavPlace tNavPlace;  // 0x00
    float flSkipDistance;  // 0x18
    BOOL bUseActorPassability;  // 0x1c
};
static_assert(sizeof(PowerAPIChooseOpenGround) == 0x20, "PowerAPIChooseOpenGround");
// X/Server/Powers/Powers.cpp:5497
BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, const WorldPlace & placeStart, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax);
// X/Server/Powers/PowerHelper.cpp
WorldPlace PowerHelper_ComputePathableDestination(const SNO snoPassability, const WorldPlace & wpStart, const WorldPlace & wpEnd, const float flRadius, ActorCommonData & tACD);  // :2279
void PowerHelper_ChooseOpenGroundInit(PowerID, const NavPlace & tNavPlace, PowerAPIChooseOpenGround & tOpenGroundInfo, float flSkipDistance);  // :833
// X/Server/Powers/PowerAPI.cpp
BOOL PowerAPI_DoChooseOpenGround(SNO snoActor, float flScale, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & placeOpenSpot, NavMeshHandle & tNavMeshHandleResult);  // :6190
BOOL PowerAPI_DoChooseOpenGround(ActorCommonData & tACD, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & placeOpenSpot, NavMeshHandle & tNavMeshHandleResult);  // :6235
// X/Common/ActorCommonData.cpp:2997
Angle ACDGetYaw(ACDID idACD);
// X/Common/Powers/PowersCommon.cpp
void PowerGetTargetPlace(AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldPlace & tPlace);  // :2949
BOOL PowerPlaceToNavMeshHandle(const WorldPlace & tPlace, NavMeshHandle & tNavMeshHandle);  // :5266

// PowersSwig.cpp:81-83.
static const int32 MAX_TARGET_AREAS_PER_POWER = 32;
static const int32 MAX_PAYLOADS_PER_POWER = 32;
static const int32 MAX_CURSORS_PER_POWER = 16;

// Defined further down in the original file (PowersSwig.cpp:1046,1246,1482); not written yet.
int32 sPowerCreateTargetAreaAOE(WorldPlace wpSource, float flRadius);
int32 sPowerCreateTargetAreaCylinder(const WorldPlace & wpSource, const float flRadius, const float flHeight);
int32 sPowerCreateTargetAreaBox(WorldPlace wpSource, const Vector3D & vDir, float flLength, float flWidth);

// PowersSwig.cpp:93; inlined into the original native Lua wrappers.
static PowerID sGetCurrentPower()
{
    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();
    DataID idAttachment;
    ScriptAttachmentType eAttachmentType;
    LuaScriptInstanceGetAttachmentIDAndType(idScriptInstance, idAttachment, eAttachmentType);
    return idAttachment;
}

// PowersSwig.cpp:197.
static BuffID sBuffFindForACD(PowerID idPower, ACDID idACD, int32 buffIndex, ACDID idACDSource, BOOL bAllowDeletedBuffs)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return BUFFID_INVALID;
    }

    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower != NULL) {
        return PowerAPI_BuffFindOnACD(ptPower->snoPower, buffIndex, *ptACD, idACDSource, bAllowDeletedBuffs);
    }

    // The power is gone: walk the actor's buffs by hand.
    BuffID idDeletedBuff = BUFFID_INVALID;
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos != NULL) {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff->idPower == idPower && ptBuff->nBuffIndex == buffIndex && (idACDSource == ACDID_INVALID || ptBuff->idACDSource == idACDSource)) {
            if (!ptBuff->bDeleted) {
                return idBuff;
            }
            idDeletedBuff = idBuff;
        }
    }
    return bAllowDeletedBuffs ? idDeletedBuff : BUFFID_INVALID;
}

// PowersSwig.cpp:250.
static BOOL sPowerValidateCursor(Power * ptPower, int32 nCursorID)
{
    return nCursorID < ptPower->listCursors.Count() && nCursorID >= 0;  // Inferred: Count() first keeps its load in the caller's PowerGet block, as the kept ptPower NULL tests in the original callers require.
}

// PowersSwig.cpp:266.
static BOOL sPowerValidateTargetArea(Power * ptPower, int32 nTargetAreaID)
{
    return nTargetAreaID < ptPower->listChooseTargets.Count() && nTargetAreaID >= 0;  // Inferred: Count() first keeps its load in the caller's PowerGet block, as the kept ptPower NULL tests in the original callers require.
}

// PowersSwig.cpp:296.
static BOOL sPowerValidateBuff(Power * ptPower, int32 nBuffID)
{
    return nBuffID < ptPower->listBuffs.Count() && nBuffID >= 0;  // Inferred: same order as the other validators (gives PowerBuffSetPeriodicRefreshRate's test order).
}

// PowersSwig.cpp:311.
static PowerAPIAttackPayload * sPowerGetAttackPayloadAtIndex(Power * ptPower, int32 nPayloadID)
{
    // DWARF shape: block with posPayload 317 / x 318, sAttackPayload 321 (as sPowerGetChooseTargetsAtIndex).
    if (ptPower && nPayloadID >= 0 && nPayloadID < ptPower->listPayloads.Count()) {
        POS posPayload = ptPower->listPayloads.GetHeadPosition();
        int32 x = 0;
        while (posPayload) {
            PowerAPIAttackPayload & sAttackPayload = ptPower->listPayloads.GetNext(posPayload);
            if (x++ >= nPayloadID)
                return &sAttackPayload;
        }
    }

    return NULL;
}


// PowersSwig.cpp:334. Shape from DWARF: locals posChooseTarget (340), x (341) and sChooseTargets (344) sit in a lexical block under the line-338 condition (null test at 338:17).
static PowerAPIChooseTargets * sPowerGetChooseTargetsAtIndex(Power * ptPower, int32 nChooseTargetID)
{
    if (ptPower != NULL && nChooseTargetID >= 0 && nChooseTargetID < ptPower->listChooseTargets.Count())
    {
        POS posChooseTarget = ptPower->listChooseTargets.GetHeadPosition();
        int32 x = 0;
        while (posChooseTarget)
        {
            PowerAPIChooseTargets & sChooseTargets = ptPower->listChooseTargets.GetNext(posChooseTarget);
            if (x++ >= nChooseTargetID)
                return &sChooseTargets;
        }
    }
    return NULL;
}

// PowersSwig.cpp:357; reset explicitly skips the world validation used by getters.
static PowerCursor * sPowerGetCursorAtIndex(Power * ptPower, int32 nCursorID, BOOL bValidateWorld)
{
    PowerCursor * ptCursor = NULL;  // DWARF shape: ptCursor 362, block with posCursor 366 / x 367, sCursor 370, world test 381.
    if (ptPower && nCursorID >= 0 && nCursorID < ptPower->listCursors.Count()) {
        POS posCursor = ptPower->listCursors.GetHeadPosition();
        int32 x = 0;
        while (posCursor) {
            PowerCursor & sCursor = ptPower->listCursors.GetNext(posCursor);
            if (x++ >= nCursorID) {
                ptCursor = &sCursor;
                break;
            }
        }
    }
    if (ptCursor && bValidateWorld && SWorldTryToGet(ptCursor->tPlace.idSWorld) == NULL)
        ptCursor = NULL;
    return ptCursor;
}


// PowersSwig.cpp:392; bounded indexing into the original value-list specialization.
static PowerAPIBuffStartData * sPowerGetBuffStartDataAtIndex(Power * ptPower, int32 nBuffStartDataID)
{
    // DWARF shape: block with posBuff 398 / x 399, sBuff 402 (as sPowerGetChooseTargetsAtIndex).
    if (ptPower && nBuffStartDataID >= 0 && nBuffStartDataID < ptPower->listBuffs.Count()) {
        POS posBuff = ptPower->listBuffs.GetHeadPosition();
        int32 x = 0;
        while (posBuff) {
            PowerAPIBuffStartData & sBuff = ptPower->listBuffs.GetNext(posBuff);
            if (x++ >= nBuffStartDataID) {
                return sBuff.IsInitialized() ? &sBuff : NULL;
            }
        }
    }

    return NULL;
}

BOOL PowerDoFirstTimeCosts(BOOL bWantCooldown)
{
    PowerID idPower = sGetCurrentPower();
    return PowerHelper_DoFirstTimeCosts(idPower, bWantCooldown);
}

BOOL PowerDoChanneledContinualCosts(float fResourceCostModifier)
{
    PowerID idPower = sGetCurrentPower();
    return PowerAPI_ChanneledSpellDoContinualCosts(idPower, fResourceCostModifier);
}

int32 PowerDoHealthCostsForTargets(int32 nNumTargets)
{
    PowerID idPower = sGetCurrentPower();
    return PowerAPI_HealthCostsForTargets(idPower, nNumTargets);
}

int32 PowerGetLevel()
{
    PowerID idPower = sGetCurrentPower();
    return PowerGetLevel(idPower);
}

int32 PowerGetCasterLevel()
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD != NULL) {
        return ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
    }
    return 1;
}

ACDID PowerGetOwner()
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner != NULL) {
        return ptACDOwner->id;
    }
    return ACDID_INVALID;
}

VACDID PowerGetAlternateSourceActor()
{
    PowerID idPower = sGetCurrentPower();
    return PowerGetAlternateSource(idPower);
}

SNO PowerGetSNO()
{
    PowerID idPower = sGetCurrentPower();
    return PowerGetSNO(idPower);
}

DamageType PowerGetDamageType(int32 idPower)
{
    if (idPower == POWERID_INVALID) {
        idPower = sGetCurrentPower();
    }
    Power * ptPower = PowerTryToGet(idPower);
    return ptPower != NULL ? PowerGetDamageType(ptPower) : DAMAGE_TYPE_NONE;
}

DamageType PowerGetDamageType(SNO snoPower, RuneType eRuneType)
{
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    if (pdefPower == NULL) {
        return DAMAGE_TYPE_NONE;
    }
    return PowerGetDamageType(AutoPowerDefinitionR(pdefPower), eRuneType);
}

int32 PowerGetID()
{
    return sGetCurrentPower();
}

RuneType PowerGetRuneTypeFromID(int32 idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower != NULL) {
        return PowerGetRuneType(ptPower);
    }
    return RUNE_TYPE_INVALID;
}

CRefString PowerGetNameFromID(int32 idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower != NULL) {
        return SNOToString(POWER_SNO, ptPower->snoPower, FALSE);
    }
    return CRefString("");
}

float PowerGetProcScalarFromID(int32 idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower != NULL) {
        return PowerGetProcScalar(*ptPower);
    }
    return 0.0f;
}

BOOL PowerInstanceIsValid(int32 idPower)
{
    return PowerTryToGet(idPower) != NULL;
}

// Original PowersSwig.cpp:623.
float PowerGetCooldown(int32 idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower != NULL) {
        return PowerGetFormulaValue(idPower, TAG_POWER_COOLDOWN);
    }
    return -1.0f;
}

// Original PowersSwig.cpp:636.
BOOL PowerIsAssigned(SNO snoPower, ACDID idACDPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer == NULL || !ACDIsPlayer(idACDPlayer)) {
        return FALSE;
    }
    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    return PowerIsAssigned(snoPower, ptPlayer);
}

// Original PowersSwig.cpp:652.
BOOL PowerIsAutoCastInstance(int32 idPower)
{
    Power * ptPower;
    if (idPower == POWERID_INVALID) {
        ptPower = PowerTryToGet(sGetCurrentPower());
    } else {
        ptPower = PowerTryToGet(idPower);
    }
    if (ptPower == NULL) {
        return FALSE;
    }
    return ptPower->bDoesntAnimate;
}

// Original PowersSwig.cpp:671.
SNO PowerGetProjectileStickSNO(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    SNO snoProjectile = ptPower->snoProjectile;
    if (snoProjectile == SNO_NONE) {
        return SNO_NONE;
    }
    return ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_PROJECTILE_STICK_SNO, snoProjectile);
}

// Original PowersSwig.cpp:686.
float PowerGetFormulaValue(int32 nFormulaIndex)
{
    if (nFormulaIndex < 0 || nFormulaIndex >= 64) {
        return 0.0f;
    }
    PowerID idPower = sGetCurrentPower();
    return PowerGetFormulaValue(idPower, s_arFormulaTags[nFormulaIndex]);
}

// Original PowersSwig.cpp:704.
void ActorPowerForceInterruptPrimary(ACDID idActor)
{
    const ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD != NULL) {
        PowerID idPrimary = ActorPowersGetPrimary((ActorID)ptACD->idOwner);
        if (idPrimary != POWERID_INVALID) {
            PowerInterrupt(idPrimary, TRUE);
        }
    }
}

// Original PowersSwig.cpp:719.
void PowerForceInterrupt()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower != NULL) {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner != NULL && PowerShouldUseClientWalking(*ptACDOwner, AutoPowerDefinitionR(ptPower->snoPower))) {
            SActorMovement * ptMove = (SActorMovement *)ptACDOwner->GetActorMovement();
            if (ptMove != NULL) {
                ptMove->ForceAcceptNextClientPowerWalk();
            }
        }
    }
    PowerInterrupt(idPower, TRUE);
}

// Original PowersSwig.cpp:748.
void PowerForceStopPrimary(BOOL bStopReasonIsLookswitch)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return;
    }
    PowerID idPrimary = ActorPowersGetPrimary((ActorID)ptACDOwner->idOwner);
    if (idPower == idPrimary) {
        return;
    }
    if (bStopReasonIsLookswitch) {
        Power * ptPowerPrimary = PowerTryToGet(idPrimary);
        if (ptPowerPrimary != NULL) {
            ScopedPowerDefinitionLockR pdefPowerPrimary(ptPowerPrimary->snoPower);
            if (PowerGeneralTagGetBool(AutoPowerDefinitionR(pdefPowerPrimary), TAG_POWER_LOOKSWITCH_DOESNT_INTERRUPT)) {
                return;
            }
        }
    }
    PowerForceStop(idPrimary, TRUE);
}

// Original PowersSwig.cpp:778.
int32 PowerSNOFromName(LPCSTR szPowerName)
{
    return SNOFileLookupGUID(POWER_SNO, szPowerName);
}

// Original PowersSwig.cpp:790.
float PowerGetOwnerWeaponRange()
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return 0.0f;
    }
    SNO snoWeaponPower = SpellEffect_GetWeaponPower(ptACDOwner->id);
    if (snoWeaponPower == SNO_NONE) {
        return 0.0f;
    }
    return PowerGetRange(ptACDOwner, AutoPowerDefinitionR(snoWeaponPower), FALSE);
}

// Original PowersSwig.cpp:807.
float PowerGetActorWeaponRange(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return 0.0f;
    }
    SNO snoWeaponPower = SpellEffect_GetWeaponPower(idActor);
    if (snoWeaponPower == SNO_NONE) {
        return 0.0f;
    }
    return PowerGetRange(ptACD, AutoPowerDefinitionR(snoWeaponPower), TRUE);
}

// Original PowersSwig.cpp:823.
float PowerGetOwnerWeaponAverageDamage(const BOOL bWantBonuses)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return 0.0f;
    }
    float flDamage = 0.0f;
    float flAttackSpeed = 0.0f;
    PowerActorGetWeaponDamageAndAttackSpeed(*ptACDOwner, flDamage, flAttackSpeed, bWantBonuses, idPower, FALSE);
    return flDamage;
}

// Original PowersSwig.cpp:842.
float PowerGetActorWeaponAverageDamage(ACDID idActor, const BOOL bWantBonuses)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return 0.0f;
    }
    float flDamage = 0.0f;
    float flAttackSpeed = 0.0f;
    PowerActorGetWeaponDamageAndAttackSpeed(*ptACD, flDamage, flAttackSpeed, bWantBonuses, POWERID_INVALID, FALSE);
    return flDamage;
}

// Original PowersSwig.cpp:861.
float PowerGetOwnerWeaponDPS(const BOOL bWantBonuses)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return 0.0f;
    }
    float flDamage = 0.0f;
    float flAttackSpeed = 0.0f;
    PowerActorGetWeaponDamageAndAttackSpeed(*ptACDOwner, flDamage, flAttackSpeed, bWantBonuses, idPower, FALSE);
    return flDamage * flAttackSpeed;
}

// Original PowersSwig.cpp:884.
float PowerGetActorWeaponDPS(ACDID idActor, const BOOL bWantBonuses)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return 0.0f;
    }
    float flDamage = 0.0f;
    float flAttackSpeed = 0.0f;
    PowerActorGetWeaponDamageAndAttackSpeed(*ptACD, flDamage, flAttackSpeed, bWantBonuses, POWERID_INVALID, FALSE);
    return flDamage * flAttackSpeed;
}

// Original PowersSwig.cpp:907.
float PowerGetOwnerAttacksPerSecondTotal(BOOL bFromPower)
{
    PowerID idPower = sGetCurrentPower();
    if (bFromPower) {
        Power * ptPower = PowerGet(idPower);
        if (ptPower != NULL) {
            return FastAttribGetValueFloat(ptPower->idFastAttrib, FastAttribKey(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL));
        }
        return 0.0f;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner != NULL) {
        return ptACDOwner->AttributesGetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL);
    }
    return 0.0f;
}

// Original PowersSwig.cpp:934.
int32 PowerCreateTargetAreaMelee(float flSearchRadius)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    NavPlace tNavPlace;
    PowerHelperGetNavPlaceFromPowerSource(idPower, tNavPlace);

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = 2;
    tChooseTargets.dwFlags = 0;
    PowerHelper_ChooseTargetsInitGeometryMelee(ptPower->idACD, ptPower->tTarget, Math::FloatRoundToInt(flSearchRadius), tChooseTargets.tGeometry);

    int32 nIndex = ptPower->listChooseTargets.Count();
    ptPower->listChooseTargets.AddTail_Tracked(tChooseTargets);

    if (nIndex >= MAX_TARGET_AREAS_PER_POWER) {
        CRefString sError;
        sError.Format("PowerCreateTargetAreaMelee: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated target areas per power (%d)! Likely the script forgot to call Hydra.PowerTargetAreaDeleteLastCreated() and is leaking target areas!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_TARGET_AREAS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:966.
static int32 sPowerCreateTargetAreaSingleTarget(Target tTarget, WorldPlace wpSource, float flSearchRadius)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = 2;
    PowerHelper_ChooseTargetsInitGeometrySingleTarget(wpSource, tTarget, flSearchRadius, tChooseTargets.tGeometry);
    tChooseTargets.dwFlags = (1 << CTF_CLOSEST_ONLY_BIT);

    int32 nIndex = ptPower->listChooseTargets.Count();
    ptPower->listChooseTargets.AddTail_Tracked(tChooseTargets);

    if (nIndex >= MAX_TARGET_AREAS_PER_POWER) {
        CRefString sError;
        sError.Format("sPowerCreateTargetAreaSingleTarget: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated target areas per power (%d)! Likely the script forgot to call Hydra.PowerTargetAreaDeleteLastCreated() and is leaking target areas!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_TARGET_AREAS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:997.
int32 PowerCreateTargetAreaSingleTarget(float flSearchRadius)
{
    PowerID idPower = sGetCurrentPower();
    NavPlace tNavPlace;
    PowerHelperGetNavPlaceFromPowerSource(idPower, tNavPlace);
    Target tTarget;
    tTarget.InitializeFromPlace(tNavPlace.tPlace);
    return sPowerCreateTargetAreaSingleTarget(tTarget, tNavPlace.tPlace, flSearchRadius);
}

// Original PowersSwig.cpp:1008.
int32 PowerCreateTargetAreaSingleTargetAroundActor(VACDID idActor, float flSearchRadius)
{
    if (ACDTryToGet(idActor) != NULL) {
        WorldPlace wpSource = ACDGet(idActor).GetPlace();
        Target tTarget;
        tTarget.InitializeFromACD(idActor, FALSE, FALSE);
        return sPowerCreateTargetAreaSingleTarget(tTarget, wpSource, flSearchRadius);
    }
    return PowerCreateTargetAreaSingleTarget(flSearchRadius);
}

// Original PowersSwig.cpp:1028.

int32 PowerCreateTargetAreaSingleTargetAroundCursor(int32 nCursorID, float flSearchRadius)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    Target tTarget;

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor != NULL) {
        tTarget.InitializeFromPlace(ptCursor->tPlace);
        return sPowerCreateTargetAreaSingleTarget(tTarget, ptCursor->tPlace, flSearchRadius);
    }
    return PowerCreateTargetAreaSingleTarget(flSearchRadius);
}

// Original PowersSwig.cpp:1046.
int32 sPowerCreateTargetAreaAOE(WorldPlace wpSource, float flRadius)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = 2;
    tChooseTargets.dwFlags = 0;
    PowerHelper_ChooseTargetsInitGeometrySphere(wpSource, flRadius, tChooseTargets.tGeometry);

    int32 nIndex = ptPower->listChooseTargets.Count();
    ptPower->listChooseTargets.AddTail_Tracked(tChooseTargets);

    if (nIndex >= MAX_TARGET_AREAS_PER_POWER) {
        CRefString sError;
        sError.Format("sPowerCreateTargetAreaAOE: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated target areas per power (%d)! Likely the script forgot to call Hydra.PowerTargetAreaDeleteLastCreated() and is leaking target areas!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_TARGET_AREAS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:1075.
int32 PowerCreateTargetAreaAOE(float flRadius)
{
    PowerID idPower = sGetCurrentPower();
    NavPlace tNavPlace;
    PowerHelperGetNavPlaceFromPowerSource(idPower, tNavPlace);
    return sPowerCreateTargetAreaAOE(tNavPlace.tPlace, flRadius);
}

// Original PowersSwig.cpp:1087.
int32 PowerCreateTargetAreaAOEAroundActor(VACDID idActor, float flRadius)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptPower != NULL && ptACD != NULL) {
        WorldPlace wpSource = ptACD->GetPlace();
        return sPowerCreateTargetAreaAOE(wpSource, flRadius);
    }
    return PowerCreateTargetAreaAOE(flRadius);
}

// Original PowersSwig.cpp:1111.

int32 PowerCreateTargetAreaAOEAroundCursor(int32 nCursorID, float flRadius)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return PowerCreateTargetAreaAOE(flRadius);
    }
    return sPowerCreateTargetAreaAOE(ptCursor->tPlace, flRadius);
}

// Original PowersSwig.cpp:1129.
static int32 sPowerCreateTargetAreaPie(WorldPlace wpSource, float flRadius, int32 nDegrees, float flHeight, Quaternion * pRot)
{
    WorldDistance wdHeight = flHeight;
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = 2;
    tChooseTargets.dwFlags = 0;

    if (wdHeight > 1e-06f) {
        wpSource.wp.z -= wdHeight * 0.5f;
    } else {
        wdHeight = (ptACD != NULL ? ptACD->GetHeight() : 10.0f) + 12.0f;
        wpSource.wp.z -= 12.0f;
    }

    PowerHelper_ChooseTargetsInitGeometryPie(wpSource, ptPower->tTarget, flRadius, wdHeight, nDegrees, tChooseTargets.tGeometry);
    if (pRot != NULL) {
        tChooseTargets.tGeometry.tPie.wvPieVector = cgwvXAxis;
        Angle aFacing = pRot->ToYaw();
        Vector3D::Rotate(cgwvZAxis, aFacing, tChooseTargets.tGeometry.tPie.wvPieVector);
    }

    int32 nIndex = ptPower->listChooseTargets.Count();
    ptPower->listChooseTargets.AddTail_Tracked(tChooseTargets);

    if (nIndex >= MAX_TARGET_AREAS_PER_POWER) {
        CRefString sError;
        sError.Format("sPowerCreateTargetAreaPie: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated target areas per power (%d)! Likely the script forgot to call Hydra.PowerTargetAreaDeleteLastCreated() and is leaking target areas!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_TARGET_AREAS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:1189.
int32 PowerCreateTargetAreaPie(float flRadius, int32 nDegrees, float flHeight)
{
    PowerID idPower = sGetCurrentPower();
    NavPlace tNavPlace;
    PowerHelperGetNavPlaceFromPowerSource(idPower, tNavPlace);
    return sPowerCreateTargetAreaPie(tNavPlace.tPlace, flRadius, nDegrees, flHeight, NULL);
}

// Original PowersSwig.cpp:1202.
int32 PowerCreateTargetAreaPieAroundActor(VACDID idActor, float flRadius, int32 nDegrees, float flHeight)
{
    if (ACDTryToGet(idActor) != NULL) {
        WorldPlace wpSource = ACDGet(idActor).GetPlace();
        return sPowerCreateTargetAreaPie(wpSource, flRadius, nDegrees, flHeight, NULL);
    }
    return PowerCreateTargetAreaPie(flRadius, nDegrees, flHeight);
}

// Original PowersSwig.cpp:1222.

int32 PowerCreateTargetAreaPieAroundCursor(int32 nCursorID, float flRadius, int32 nDegrees, float flHeight)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (!sPowerValidateCursor(ptPower, nCursorID)) {
        return PowerCreateTargetAreaPie(flRadius, nDegrees, flHeight);
    }
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return PowerCreateTargetAreaPie(flRadius, nDegrees, flHeight);
    }
    return sPowerCreateTargetAreaPie(ptCursor->tPlace, flRadius, nDegrees, flHeight, &ptCursor->qOrientation);
}

// Original PowersSwig.cpp:1246.
int32 sPowerCreateTargetAreaCylinder(const WorldPlace & wpSource, const float flRadius, const float flHeight)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = 2;
    tChooseTargets.dwFlags = 0;
    PowerHelper_ChooseTargetsInitGeometryCylinder(wpSource, flRadius, flHeight, tChooseTargets.tGeometry);

    int32 nIndex = ptPower->listChooseTargets.Count();
    ptPower->listChooseTargets.AddTail_Tracked(tChooseTargets);

    if (nIndex >= MAX_TARGET_AREAS_PER_POWER) {
        CRefString sError;
        sError.Format("sPowerCreateTargetAreaCylinder: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated target areas per power (%d)! Likely the script forgot to call Hydra.PowerTargetAreaDeleteLastCreated() and is leaking target areas!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_TARGET_AREAS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:1277.
int32 PowerCreateTargetAreaCylinder(float flRadius)
{
    return PowerCreateTargetAreaCylinder(flRadius, flRadius);
}

// Original PowersSwig.cpp:1283.
int32 PowerCreateTargetAreaCylinder(float flRadius, float flHeight)
{
    PowerID idPower = sGetCurrentPower();
    NavPlace tNavPlace;
    PowerHelperGetNavPlaceFromPowerSource(idPower, tNavPlace);
    return sPowerCreateTargetAreaCylinder(tNavPlace.tPlace, flRadius, flHeight);
}

// Original PowersSwig.cpp:1296.
int32 PowerCreateTargetAreaCylinderAroundActor(VACDID idActor, float flRadius)
{
    return PowerCreateTargetAreaCylinderAroundActor(idActor, flRadius, flRadius);
}

// Original PowersSwig.cpp:1303.
int32 PowerCreateTargetAreaCylinderAroundActor(VACDID idActor, float flRadius, float flHeight)
{
    const ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD != NULL) {
        WorldPlace wpSource = ptACD->GetPlace();
        return sPowerCreateTargetAreaCylinder(wpSource, flRadius, flHeight);
    }
    return PowerCreateTargetAreaCylinder(flRadius, flHeight);
}

// Original PowersSwig.cpp:1324.
int32 PowerCreateTargetAreaCylinderAroundCursor(int32 nCursorID, float flRadius)
{
    return PowerCreateTargetAreaCylinderAroundCursor(nCursorID, flRadius, flRadius);
}

// Original PowersSwig.cpp:1331.

int32 PowerCreateTargetAreaCylinderAroundCursor(int32 nCursorID, float flRadius, float flHeight)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (!sPowerValidateCursor(ptPower, nCursorID)) {
        return PowerCreateTargetAreaCylinder(flRadius, flHeight);
    }
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return PowerCreateTargetAreaCylinder(flRadius, flHeight);
    }
    return sPowerCreateTargetAreaCylinder(ptCursor->tPlace, flRadius, flHeight);
}

// Original PowersSwig.cpp:1363.
static int32 sPowerCreateTargetAreaCapsule(WorldPlace wpPlace1, WorldPlace wpPlace2, float flRadius, float flBonusHeight)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (wpPlace1.idSWorld != wpPlace2.idSWorld) {
        return PowerCreateTargetAreaAOE(flRadius);
    }

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = 2;
    tChooseTargets.dwFlags = 0;
    PowerHelper_ChooseTargetsInitGeometryCapsule(wpPlace1, wpPlace2, flRadius, flBonusHeight, tChooseTargets.tGeometry);

    int32 nIndex = ptPower->listChooseTargets.Count();
    ptPower->listChooseTargets.AddTail_Tracked(tChooseTargets);

    if (nIndex >= MAX_TARGET_AREAS_PER_POWER) {
        CRefString sError;
        sError.Format("sPowerCreateTargetAreaCapsule: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated target areas per power (%d)! Likely the script forgot to call Hydra.PowerTargetAreaDeleteLastCreated() and is leaking target areas!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_TARGET_AREAS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:1402.
int32 PowerCreateTargetAreaCapsuleAroundActors(VACDID idActor1, VACDID idActor2, float flRadius, float flBonusHeight)
{
    ActorCommonData * ptACD1 = ACDTryToGet(idActor1);
    ActorCommonData * ptACD2 = ACDTryToGet(idActor2);
    if (ptACD1 != NULL && ptACD2 != NULL) {
        return sPowerCreateTargetAreaCapsule(ptACD1->GetPlace(), ptACD2->GetPlace(), flRadius, flBonusHeight);
    }
    return PowerCreateTargetAreaAOE(flRadius);
}

// Original PowersSwig.cpp:1432.
int32 PowerCreateTargetAreaCapsuleAroundActorAndCursor(VACDID idActor, int32 nCursorID, float flRadius, float flBonusHeight)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return PowerCreateTargetAreaAOE(flRadius);
    }
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return PowerCreateTargetAreaAOE(flRadius);
    }
    return sPowerCreateTargetAreaCapsule(ptACD->GetPlace(), ptCursor->tPlace, flRadius, flBonusHeight);
}

// Original PowersSwig.cpp:1460.
int32 PowerCreateTargetAreaCapsuleAroundCursors(int32 nCursorID1, int32 nCursorID2, float flRadius, float flBonusHeight)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor1 = sPowerGetCursorAtIndex(ptPower, nCursorID1, TRUE);
    PowerCursor * ptCursor2 = sPowerGetCursorAtIndex(ptPower, nCursorID2, TRUE);
    if (ptCursor1 && ptCursor2) {
        return sPowerCreateTargetAreaCapsule(ptCursor1->tPlace, ptCursor2->tPlace, flRadius, flBonusHeight);
    }
    return PowerCreateTargetAreaAOE(flRadius);
}

// Original PowersSwig.cpp:1482.
int32 sPowerCreateTargetAreaBox(WorldPlace wpSource, const Vector3D & vDir, float flLength, float flWidth)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = 2;
    tChooseTargets.dwFlags = 0;
    PowerHelper_ChooseTargetsInitGeometryBox(wpSource, vDir, flLength, flWidth, tChooseTargets.tGeometry);

    int32 nIndex = ptPower->listChooseTargets.Count();
    ptPower->listChooseTargets.AddTail_Tracked(tChooseTargets);

    if (nIndex >= MAX_TARGET_AREAS_PER_POWER) {
        CRefString sError;
        sError.Format("sPowerCreateTargetAreaBox: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated target areas per power (%d)! Likely the script forgot to call Hydra.PowerTargetAreaDeleteLastCreated() and is leaking target areas!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_TARGET_AREAS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:1513.
int32 PowerCreateTargetAreaBox(float flLength, float flWidth)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    NavPlace tNavPlace;
    PowerHelperGetNavPlaceFromPowerSource(idPower, tNavPlace);
    WorldPlace placeTarget;
    ptPower->tTarget.GetPlace(placeTarget);
    Vector3D vDir = placeTarget.wp - tNavPlace.tPlace.wp;
    return sPowerCreateTargetAreaBox(tNavPlace.tPlace, vDir, flLength, flWidth);
}

// Original PowersSwig.cpp:1532.

int32 PowerCreateTargetAreaBoxAroundCursor(int32 nCursorID, float flLength, float flWidth)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    Angle aFacing;

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return PowerCreateTargetAreaBox(flLength, flWidth);
    }
    Vector3D vDir = cgwvXAxis;
    aFacing = ptCursor->qOrientation.ToYaw();
    Vector3D::Rotate(cgwvZAxis, aFacing, vDir);
    return sPowerCreateTargetAreaBox(ptCursor->tPlace, vDir, flLength, flWidth);
}

// Original PowersSwig.cpp:1557.
void PowerTargetAreaDeleteLastCreated()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listChooseTargets.Count() != 0) {
        ptPower->listChooseTargets.RemoveTail();
    }
}

// Original PowersSwig.cpp:1581.
void PowerTargetAreaSetIgnoreLOS(int32 nTargetAreaID, BOOL bIgnore)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (bIgnore) {
        ptChooseTargets->dwFlags |= (1 << CTF_DONT_PERFORM_LOS_CHECK);
    } else {
        ptChooseTargets->dwFlags &= ~(1 << CTF_DONT_PERFORM_LOS_CHECK);
    }
}

// Original PowersSwig.cpp:1598.
void PowerTargetAreaSetLOSTypeWalkableOnly(int32 nTargetAreaID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    ptChooseTargets->ePassabilityType = PASSABILITY_MONSTER;
}

// Original PowersSwig.cpp:1614.
void PowerTargetAreaSetLOSTypeSpider(int32 nTargetAreaID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    ptChooseTargets->ePassabilityType = PASSABILITY_SPIDER;
}

// Original PowersSwig.cpp:1629.
void PowerTargetAreaSetLOSTypeFlyer(int32 nTargetAreaID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    ptChooseTargets->ePassabilityType = PASSABILITY_FLYER;
}

// Original PowersSwig.cpp:1645.
void PowerTargetAreaSetLOSTypeGhost(int32 nTargetAreaID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    ptChooseTargets->ePassabilityType = PASSABILITY_GHOST;
}

// Original PowersSwig.cpp:1661.
void PowerTargetAreaSetUseMeleeSnapshot(int32 nTargetAreaID, BOOL bUseMeleeSnapshot)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (bUseMeleeSnapshot) {
        ptChooseTargets->dwFlags |= (1 << CTF_USE_MELEE_SNAPSHOT);
    } else {
        ptChooseTargets->dwFlags &= ~(1 << CTF_USE_MELEE_SNAPSHOT);
    }
}

// Original PowersSwig.cpp:1678.
void PowerTargetAreaSetTargetsItemsOnly(int32 nTargetAreaID, BOOL bItemsOnly)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (bItemsOnly) {
        ptChooseTargets->dwFlags |= (1 << CTF_TARGET_ITEMS_ONLY);
    } else {
        ptChooseTargets->dwFlags &= ~(1 << CTF_TARGET_ITEMS_ONLY);
    }
    ptPower->tTargetCriteria.eLifeStatus = bItemsOnly ? TCLS_ANY : TCLS_ALIVE_ONLY;
}

// Original PowersSwig.cpp:1696.
void PowerTargetAreaSetTargetsItemsWithLabel(int32 nTargetAreaID, ItemLabel eItemLabel)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    ptChooseTargets->dwFlags |= (1 << CTF_FILTER_ITEMS_BY_LABEL);
    ptChooseTargets->dwItemLabelMask[eItemLabel >> 5] |= (1 << (eItemLabel & 31));
}

// Original PowersSwig.cpp:1714.
float PowerGetVerticalDistanceOfContactOffset(int32 nContactFrame)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (nContactFrame < 0 || nContactFrame >= 4 || ptACDOwner == NULL) {
        return 0.0f;
    }
    Vector3D wpContactOffset;
    PowerHelper_GetContactOffsetForPower(idPower, nContactFrame, wpContactOffset, TRUE);
    return Math::Abs(ptACDOwner->GetPlace().wp.z - wpContactOffset.z);
}

// Original PowersSwig.cpp:1743.

void PowerSetCooldown(float flDuration, BOOL bIgnoreCooldownReduction)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (flDuration <= 1e-06f || ptACDOwner == NULL) {
        return;
    }

    SNO snoCurrentPower = PowerGetSNO(idPower);
    if (!bIgnoreCooldownReduction) {
        flDuration = Math::Max(0.0f, flDuration - ptACDOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_REDUCTION, snoCurrentPower)));
        flDuration = Math::Max(0.0f, flDuration - ptACDOwner->AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_ALL));
        float flReductionPercent = PowerGetCoolDownReductionPercent(snoCurrentPower, *ptACDOwner);
        flDuration *= (1.0f - flReductionPercent);
    }

    float flCooldownMinTime = ptACDOwner->AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME);
    flCooldownMinTime = ptACDOwner->AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME);
    flDuration = Math::Max(flDuration, flCooldownMinTime);

    if (flDuration > 1e-06f && !ActorFlagIsSet((ActorID)ACDGetOwner(ptACDOwner->id), ACTOR_GOD_MODE_BIT)) {
        uint32 dwCooldownTicks = Math::FloatToInt(flDuration * TICKS_PER_SECOND);
        const int32 nMaxCooldownStackCount = 1024;
        uint32 dwNow = GameGetServerTime();
        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.idACDSource = ptACDOwner->id;
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
        tBuffStartData.dwDurationMin = dwCooldownTicks;  // Store order from the line table (1788-1792 / 1874-1878).
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
        tBuffStartData.nMaxStackCount = nMaxCooldownStackCount;
        tBuffStartData.bRemoveOnDeath = FALSE;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoCurrentPower), dwNow + dwCooldownTicks);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, snoCurrentPower), dwNow);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    }
}

// Original PowersSwig.cpp:1802.

void PowerSetCooldown(VACDID idActor, LPCSTR szPowerName, float flDuration, BOOL bIgnoreCooldownReduction)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (idActor == ACDID_INVALID || ptACDOwner == NULL) {
        return;
    }
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL) {
        return;
    }

    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (flDuration <= 1e-06f) {
        return;
    }
    if (!bIgnoreCooldownReduction) {
        flDuration = Math::Max(0.0f, flDuration - ptACDOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_REDUCTION, snoPower)));
        flDuration = Math::Max(0.0f, flDuration - ptACDOwner->AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_ALL));
        float flReductionPercent = PowerGetCoolDownReductionPercent(snoPower, *ptACDOwner);
        flDuration *= (1.0f - flReductionPercent);
    }

    float flCooldownMinTime = ptACDOwner->AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME);
    flCooldownMinTime = ptACDOwner->AttributesGetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME);
    flDuration = Math::Max(flDuration, flCooldownMinTime);

    BOOL bExists = (snoPower != SNO_NONE);
    if (bExists && flDuration > 1e-06f && !ActorFlagIsSet((ActorID)ACDGetOwner(ptACDActor->id), ACTOR_GOD_MODE_BIT)) {
        uint32 dwCooldownTicks = Math::FloatToInt(flDuration * TICKS_PER_SECOND);
        const int32 nMaxCooldownStackCount = 1024;
        uint32 dwNow = GameGetServerTime();
        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.idACDSource = ptACDOwner->id;
        tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
        tBuffStartData.dwDurationMin = dwCooldownTicks;  // Store order from the line table (1788-1792 / 1874-1878).
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
        tBuffStartData.nMaxStackCount = nMaxCooldownStackCount;
        tBuffStartData.bRemoveOnDeath = FALSE;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower), dwNow + dwCooldownTicks);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, snoPower), dwNow);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACDActor);
    }
}

// Original PowersSwig.cpp:1887.
void PowerRemoveCooldown(VACDID idActor, LPCSTR szPowerName)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (idActor == ACDID_INVALID || ptACDOwner == NULL) {
        return;
    }
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL) {
        return;
    }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    SNO snoPowersCooldown = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
    POS pos = ptACDActor->listBuffs.GetHeadPosition();
    while (pos != NULL) {
        BuffID idBuff = ptACDActor->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff != NULL && !ptBuff->bDeleted && ptBuff->snoPower == snoPowersCooldown) {
            if (FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower)) > 0) {
                PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
    }
}

// Original PowersSwig.cpp:1945.
void PowerReduceCooldown(VACDID idActor, LPCTSTR szPowerName, float flReduction)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (idActor == ACDID_INVALID || ptACDOwner == NULL) {
        return;
    }
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL) {
        return;
    }
    int32 nReductionInTicks = Math::FloatToInt(flReduction * TICKS_PER_SECOND);
    if (nReductionInTicks > 0) {
        SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
        PowerReduceCooldown(ptACDActor, snoPower, nReductionInTicks);
    }
}

// Original PowersSwig.cpp:1997.
// No explicit idActor test: ACDTryToGet (2011) carries the INVALID test; 2005/2013 are separate ifs (tab-indented columns).
#line 1997
void PowerReduceCooldownByPercentage(
	VACDID idActor,
	LPCSTR szPowerName,
	float flReductionPct)
{
	PowerID idPower = sGetCurrentPower();
	ActorCommonData * ptACDOwner = PowerGetOwner(idPower);

	if (ptACDOwner == NULL)
	{
		return;
	}


	ActorCommonData * ptACDActor = ACDTryToGet(idActor);

	if (ptACDActor == NULL)
	{
		return;
	}


	if (flReductionPct <= EPSILON)
	{
		return;
	}

	if (flReductionPct - 1.0f > EPSILON)
	{
		return;
	}


	SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);











	PowerReduceCooldownByPercentage(ptACDActor, snoPower, flReductionPct);
}
#line 1243

// Original PowersSwig.cpp:2046.
void PowerReduceAllCooldowns(ACDID idActor, float flReduction)
{
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL) {
        return;
    }
    int32 nReductionInTicks = Math::FloatToInt(flReduction * TICKS_PER_SECOND);
    if (nReductionInTicks <= 0) {
        return;
    }
    SNO snoSkillKit = ptACDActor->GetSkillKit();
    if (snoSkillKit != SNO_NONE) {
        int32 nCount;
        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
        nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
        for (int32 i = 0; i < nCount; i++) {
            const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
            if (tEntry.snoPower != SNO_NONE) {
                PowerReduceCooldown(ptACDActor, tEntry.snoPower, nReductionInTicks);
            }
        }
    }
}

// Original PowersSwig.cpp:2083.

void PowerReduceAllCooldownsByPercentage(ACDID idActor, float flReductionPct)
{
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL) { return; }  // Separate ifs, as in PowerReduceCooldownByPercentage.
    if (flReductionPct <= EPSILON) { return; }
    if (flReductionPct - 1.0f > EPSILON) { return; }
    {
        SNO snoSkillKit = ptACDActor->GetSkillKit();
        if (snoSkillKit != SNO_NONE) {
            int32 nCount;
            ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
            nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
            for (int32 i = 0; i < nCount; i++) {
                const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
                if (tEntry.snoPower != SNO_NONE) {
                    PowerReduceCooldownByPercentage(ptACDActor, tEntry.snoPower, flReductionPct);
                }
            }
        }
    }
}

// Original PowersSwig.cpp:2124.
float PowerGetRemainingCooldown(VACDID idActor, LPCSTR szPowerName)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (idActor == ACDID_INVALID || ptACDOwner == NULL) {
        return 0.0f;
    }
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL) {
        return 0.0f;
    }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    uint32 dwNow = GameGetServerTime();
    uint32 dwCooldownEndTick = ptACDActor->AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower));
    if (dwCooldownEndTick != (uint32)-1 && dwCooldownEndTick > dwNow) {
        return (dwCooldownEndTick - dwNow) * OO_TICKS_PER_SECOND;
    }
    return 0.0f;
}

// Original PowersSwig.cpp:2175.
void PowerResetAllCooldowns(VACDID idActor)
{
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor != NULL) {
        PowerAPI_ResetCooldowns(*ptACDActor);
    }
}

// Original PowersSwig.cpp:2190.
void PowerTriggerCooldown(VACDID idActor, LPCSTR szPowerName, float flOverridenCooldown)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    PowerTriggerCooldown(ptACD, snoPower, flOverridenCooldown);
}

// Original PowersSwig.cpp:2201.
void PowerTargetAreaSetIgnoreStealthed(int32 nTargetAreaID, BOOL bIgnoreStealthed)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (bIgnoreStealthed) {
        ptChooseTargets->dwFlags |= (1 << CTF_IGNORE_STEALTHED);
    } else {
        ptChooseTargets->dwFlags &= ~(1 << CTF_IGNORE_STEALTHED);
    }
}

// Original PowersSwig.cpp:2218.
void PowerTargetAreaSetUseProjectileLOS(int32 nTargetAreaID, BOOL bUseProjectileLOS)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        return;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (bUseProjectileLOS) {
        ptChooseTargets->dwFlags |= (1 << CTF_PERFORM_PROJECTILE_LOS_CHECK);
    } else {
        ptChooseTargets->dwFlags &= ~(1 << CTF_PERFORM_PROJECTILE_LOS_CHECK);
    }
}

// Original PowersSwig.cpp:2236.

ActorIDArray * PowerGetTargets(int32 nTargetAreaID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID)) {
        ActorIDArray & tEmptyArrACDs = LuaScriptGetThreadSafeActorIDArray();
        tEmptyArrACDs.nNumActorIDs = 0;
        return &tEmptyArrACDs;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(*ptChooseTargets, listTargets);

    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;
    int32 nMaxTargets = Math::Min(PowerGetMaxAreaEffectTargetCount(), 64);
    POS pos = listTargets.GetHeadPosition();
    while (pos != NULL) {
        ACDID idACD = (ACDID)listTargets.GetNext(pos);
        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACD;
        // Inferred: increment and limit test share original line 2267 (column 30, the ++), as in PowerGetSeparatedTargets.
        tArrACDs.nNumActorIDs++; if (tArrACDs.nNumActorIDs >= nMaxTargets) break;
    }
    return &tArrACDs;
}

// Original PowersSwig.cpp:2289.
BOOL PowerCanTarget(VACDID idACDTarget)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL) {
        return FALSE;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return FALSE;
    }
    return PowerCanTarget(idPower, idACDTarget, ptACDOwner->id);
}

// Original PowersSwig.cpp:2312.
BOOL PowerActorHasEnoughResourceForPower(ACDID idActor, SNO snoPower, int32 idPower)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return FALSE;
    }
    if (idPower != POWERID_INVALID) {
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower == NULL) {
            return FALSE;
        }
        PowerActorHasResourceMinCostToStart(*ptACD, AutoPowerDefinitionR(ptPower->snoPower));
    }
    return PowerActorHasResourceMinCostToStart(*ptACD, AutoPowerDefinitionR(snoPower));
}

// Original PowersSwig.cpp:2491.
CRefString PowerGetReviveMonsterFromNecroCorpse(VACDID idCorpse)
{
    ActorCommonData * ptACDCorpse = ACDTryToGet(idCorpse);
    if (ptACDCorpse != NULL) {
        SNO snoMonster = ptACDCorpse->AttributesGetInt(ATTRIBUTE_NECROMANCER_CORPSE_SOURCE_MONSTER_SNO);
        if (snoMonster != SNO_NONE) {
            ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
            const CTagMap * pTagMap = pdefMonster->hTagMap;
            SNO snoValue = TagMapGetSNO(pTagMap, TAG_MONSTER_NECRO_REVIVED_MONSTER_SNO);
            const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, TAG_MONSTER_NECRO_REVIVED_MONSTER_SNO);
            SNOGroup snoGroup = AxeTagTypeToSNOGroup(ptInfo->eType);
            return SNOToString(snoGroup, snoValue, TRUE);
        }
    }
    return CRefString("");
}

// Original PowersSwig.cpp:2717.
// No explicit idACDTarget test: the cmn at 2732 col 9 is ACDTryToGet's own INVALID test, hoisted (2732 col 36 is eType).
#line 2717
BOOL PowerActorIsFullyContainedInTargetAreaAOE(
    VACDID idACDTarget,
    int32 nTargetAreaID)
{
    PowerID idPower = sGetCurrentPower();
    Power *ptPower = PowerGet(idPower);


    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (ptChooseTargets == NULL)
    {
        return FALSE;
    }


    if (ptChooseTargets->tGeometry.eType != CTT_SPHERE)
    {
        return FALSE;
    }


    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
    {
        return FALSE;
    }


    WorldPlace placeTarget = ptACDTarget->GetPlace();
    if (placeTarget.idSWorld != ptChooseTargets->tGeometry.tSphere.placeCenter.idSWorld)
    {
        return FALSE;
    }


    float flFullyContainedRadius = ptChooseTargets->tGeometry.tSphere.wdRadius - ptACDTarget->GetRadius() - EPSILON;


    if (flFullyContainedRadius < 0.0f)
    {
        return FALSE;
    }

    float flDistSq = Vector3D::DistanceSquared(placeTarget.wp, ptChooseTargets->tGeometry.tSphere.placeCenter.wp);
    return flDistSq <= flFullyContainedRadius * flFullyContainedRadius;
}
#line 1476

// Original PowersSwig.cpp:2767.
void PowerPayloadSetImpactEffectGroup(int32 nPayloadID, LPCSTR szEffectGroupName)
{
    SNO snoEffectGroup = SNOFileLookupGUID(EFFECTGROUP_SNO, szEffectGroupName);
    BOOL bExists = snoEffectGroup != SNO_NONE;
    if (bExists) {
        PowerID idPower = sGetCurrentPower();
        Power * ptPower = PowerGet(idPower);
        PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
        if (ptPayload != NULL) {
            ptPayload->snoTargetImpactEffectGroup = snoEffectGroup;
        }
    }
}

// Original PowersSwig.cpp:2805.
void PowerPayloadSetDeathReplacementType(int32 nPayloadID, DeathReplacementType eDeathReplacementType)
{
    if (eDeathReplacementType < DEATH_REPLACEMENT_NONE || eDeathReplacementType >= DEATH_REPLACEMENT_COUNT) {
        return;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->eDeathReplacementType = eDeathReplacementType;
    }
}

// Original PowersSwig.cpp:2828.
void PowerPayloadSetImpactParticle(int32 nPayloadID, LPCSTR szParticleName)
{
    SNO snoParticle = SNOFileLookupGUID(PARTICLE_SNO, szParticleName);
    BOOL bExists = snoParticle != SNO_NONE;
    if (bExists) {
        PowerID idPower = sGetCurrentPower();
        Power * ptPower = PowerGet(idPower);
        PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
        if (ptPayload != NULL) {
            ptPayload->snoTargetImpactParticle = snoParticle;
        }
    }
}

// Original PowersSwig.cpp:2867.
void PowerPayloadSetAttackedParticle(int32 nPayloadID, LPCSTR szParticleName)
{
    SNO snoParticle = SNOFileLookupGUID(PARTICLE_SNO, szParticleName);
    BOOL bExists = snoParticle != SNO_NONE;
    if (bExists) {
        PowerID idPower = sGetCurrentPower();
        Power * ptPower = PowerGet(idPower);
        PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
        if (ptPayload != NULL) {
            ptPayload->snoTargetAttackedParticle = snoParticle;
        }
    }
}

// Original PowersSwig.cpp:2905.
void PowerPayloadSetPetDamageScalar(int32 nPayloadID, float flScalar)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->flPetDamageScalar = Math::Max(0.0f, flScalar);
    }
}

// Original PowersSwig.cpp:2923.
void PowerPayloadSetPetAOEDamageScalar(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ScopedGlobalDefinitionLockR pdefGlobal;
        ptPayload->flPetDamageScalar = Math::Max(0.0f, pdefGlobal->ptServerData->flPetAoEDamageScalar);
    }
}

// Original PowersSwig.cpp:2942.
void PowerPayloadSetUseHighestElementalDamageBonus(int32 nPayloadID, BOOL bEnabled)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bUseHighestElementalDamageBonus = bEnabled;
    }
}

// Original PowersSwig.cpp:2959.
int32 PowerPlayRopeActorToActor(VACDID idActorFrom, RopePosition ePositionFrom, VACDID idActorTo, RopePosition ePositionTo, LPCSTR szRopeName, BOOL bUseComplexEffect, BOOL bIgnoreOwnerAlpha)
{
    return ActorPlayEffectGroupRopeActorToActor(idActorFrom, ePositionFrom, idActorTo, ePositionTo, szRopeName, bUseComplexEffect, bIgnoreOwnerAlpha);
}

// Original PowersSwig.cpp:2973.
void PowerPlayRopeActorToCursor(VACDID idActorFrom, RopePosition ePositionFrom, int32 nCursorIDTo, LPCSTR szRopeName, BOOL bIgnoreOwnerAlpha)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorTo = sPowerGetCursorAtIndex(ptPower, nCursorIDTo, TRUE);
    if (ptCursorTo == NULL) {
        return;
    }
    SNO snoRope = SNOFileLookupGUID(ROPE_SNO, szRopeName);
    if (snoRope == SNO_NONE) {
        return;
    }
    ACDPlayEffectRopeACDToPlace(snoRope, idActorFrom, ePositionFrom, ptCursorTo->tPlace, bIgnoreOwnerAlpha);
}

// Original PowersSwig.cpp:3017.
void PowerSetTriggerAnim(ActorAnimationTag eAnimTag)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower != NULL) {
        ptPower->dwAnimationTag = eAnimTag;
    }
}

// Original PowersSwig.cpp:3033.
void sEndPower(PowerID idPower, void * ptData)
{
    SpecificPowerSendMessage(idPower, POWER_END, (void *)-1);
    PowerInterrupt(idPower, TRUE);
}

// Original PowersSwig.cpp:3043.
void PowerFinishAnimation()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (ptACDOwner == NULL || ACDIsPlayer(ptACDOwner->id)) {
        return;
    }
    if (ptPower->dwFlags & (1 << PF_FINISHING_ANIM)) {
        return;
    }
    ptPower->dwFlags |= (1 << PF_FINISHING_ANIM);
    uint32 dwCooldownAnimDuration = 0;
    if (ptACDOwner->ptAnimPlayer != NULL) {
        AnimPlayerPlayNextAnimation(*ptACDOwner);
        AnimationTiming tAnimationTiming;
        AnimPlayerGetAnimationTiming(*ptACDOwner, ptPower->dwAnimationTag, (uint32)-1, 1.0f, tAnimationTiming);
        dwCooldownAnimDuration = tAnimationTiming.tAnimSpec[2].dwDuration;
    }
    PowerTriggerAddTimerTrigger(idPower, dwCooldownAnimDuration, sEndPower, NULL);
}

// Original PowersSwig.cpp:3086.
BOOL PowerGetIsUsingAlternateAnim()
{
    PowerID idPower = sGetCurrentPower();
    return PowerUseAlternateAnim(idPower);
}

// Original PowersSwig.cpp:3094.


void PowerSendCancelTargetLock(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

    ANNDataMessage tMsg;
    tMsg.ann = ANNFromACD(idActor);

    MessageSendToClient(listDestPlayers, SMSG_CANCEL_ACD_TARGET, &tMsg, sizeof(tMsg));
}

// 2.7.7 command (0x71009bf420, unnamed)
void PowerSendOwnerCheatedDeath()
{
    ActorCommonData * ptACDOwner = PowerGetOwner(sGetCurrentPower());
    if (!ACDIsPlayer(ptACDOwner->id))
        return;
    IntDataMessage tMessage;
    tMessage.nData = PlayerGetByACD(ptACDOwner->id)->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    MessageSendToClient(listDestPlayers, SMSG_PLAYER_CHEATED_DEATH, &tMessage, sizeof(tMessage));
}

// Original PowersSwig.cpp:3116.
int32 PowerCreatePayload(float flBaseDamageScalar, BOOL bForcedPhysical, ACDID idACDToUse)
{
    return PowerCreatePayloadNew(PDM_UNKNOWN, flBaseDamageScalar, bForcedPhysical, idACDToUse);
}

// Original PowersSwig.cpp:3198.
void PowerPayloadDeleteLastCreated()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPayloads.Count() != 0) {
        ptPower->listPayloads.RemoveTail();
    }
}

// Original PowersSwig.cpp:3219.
void PowerPayloadAddDamage(int32 nPayloadID, DamageType eDamageType, float flDamage, float flDamageDelta, BOOL bIgnoreBonuses)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload == NULL) {
        return;
    }
    if (bIgnoreBonuses) {
        ptPayload->flPrimaryAttributeBonusMultiplier = 1.0f;
        ptPayload->flClassBonusMultiplier = 1.0f;
        ptPayload->flSkillsBonusMultiplier = 1.0f;
        ptPayload->bCanCrit = FALSE;
    } else {
        PowerPayloadGetBonusMultipliers(ptPower->idFastAttrib, ptPower->snoPower, ptPower->id, PlayerGetPlayerClass(PowerGetOwner(idPower)), ptPayload->flPrimaryAttributeBonusMultiplier, ptPayload->flClassBonusMultiplier, ptPayload->flSkillsBonusMultiplier);
    }
    ptPayload->arCombatDamage[eDamageType].flMin += flDamage;
    ptPayload->arCombatDamage[eDamageType].flDelta += flDamageDelta;
}

// Original PowersSwig.cpp:3259.
void PowerPayloadAddPeriodicDamage(int32 nPayloadID, DamageType eDamageType, float flDuration, float flDamage, float flDamageDelta, BOOL bIgnoreBonuses, BOOL bTriggersHitProcsAndThorns)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload == NULL) {
        return;
    }
    if (bIgnoreBonuses) {
        ptPayload->flPrimaryAttributeBonusMultiplier = 1.0f;
        ptPayload->flClassBonusMultiplier = 1.0f;
        ptPayload->flSkillsBonusMultiplier = 1.0f;
        ptPayload->bCanCrit = FALSE;
    } else {
        PowerPayloadGetBonusMultipliers(ptPower->idFastAttrib, ptPower->snoPower, ptPower->id, PlayerGetPlayerClass(PowerGetOwner(idPower)), ptPayload->flPrimaryAttributeBonusMultiplier, ptPayload->flClassBonusMultiplier, ptPayload->flSkillsBonusMultiplier);
    }
    ptPayload->flDamageDurationInSecs[eDamageType] = flDuration;
    ptPayload->arCombatDamage[eDamageType].flMin += flDamage;
    ptPayload->arCombatDamage[eDamageType].flDelta += flDamageDelta;
    ptPayload->bTriggersHitProcs = (ptPower->bDoesntAnimate && !ptPower->bAllowHitProcs) ? FALSE : bTriggersHitProcsAndThorns;
}

// Original PowersSwig.cpp:3302.
void PowerPayloadConvertToDamageType(int32 nPayloadID, DamageType eDamageType)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload == NULL) {
        return;
    }
    ActorCommonData * ptACDPowerOwner = ACDTryToGet(ptPower->idACD);
    if (ptACDPowerOwner != NULL) {
        DamageType eDamageTypeOverride = ptACDPowerOwner->AttributesGetInt(ATTRIBUTE_DAMAGE_TYPE_OVERRIDE);
        if (eDamageTypeOverride != DAMAGE_TYPE_NONE) {
            eDamageType = eDamageTypeOverride;
        }
    }
    float flDamageTotal = 0.0f;
    float flDamageDeltaTotal = 0.0f;
    for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++) {
        flDamageTotal += ptPayload->arCombatDamage[i].flMin;
        flDamageDeltaTotal += ptPayload->arCombatDamage[i].flDelta;
        ptPayload->arCombatDamage[i].flMin = 0.0f;
        ptPayload->arCombatDamage[i].flDelta = 0.0f;
    }
    ptPayload->arCombatDamage[eDamageType].flMin = flDamageTotal;
    ptPayload->arCombatDamage[eDamageType].flDelta = flDamageDeltaTotal;
}

// Original PowersSwig.cpp:3348.
void PowerPayloadConvertToDOTDamageType(int32 nPayloadID, DamageType eDamageType, float flDuration, BOOL bTriggersHitProcsAndThorns)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload == NULL || flDuration <= 1e-06f) {
        return;
    }
    float flDamageTotal = 0.0f;
    float flDamageDeltaTotal = 0.0f;
    for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++) {
        flDamageTotal += ptPayload->arCombatDamage[i].flMin;
        flDamageDeltaTotal += ptPayload->arCombatDamage[i].flDelta;
        ptPayload->arCombatDamage[i].flMin = 0.0f;
        ptPayload->arCombatDamage[i].flDelta = 0.0f;
    }
    ptPayload->arCombatDamage[eDamageType].flMin = flDamageTotal;
    ptPayload->arCombatDamage[eDamageType].flDelta = flDamageDeltaTotal;
    ptPayload->flDamageDurationInSecs[eDamageType] = flDuration;
    ptPayload->eDeliveryMechanism = PDM_DOT;
    ptPayload->bTriggersHitProcs = (ptPower->bDoesntAnimate && !ptPower->bAllowHitProcs) ? FALSE : bTriggersHitProcsAndThorns;
}

// Original PowersSwig.cpp:3389.
float PowerPayloadGetAdditionalCritChance(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        return ptPayload->flPowerAdditionalChanceToCriticalHit;
    }
    return 0.0f;
}

// Original PowersSwig.cpp:3404.
void PowerPayloadSetAdditionalCritChance(int32 nPayloadID, float flCritChance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->flPowerAdditionalChanceToCriticalHit = flCritChance;
    }
}

// Original PowersSwig.cpp:3420.
float PowerPayloadGetAdditionalCrushingBlowChance(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        return ptPayload->flPowerAdditionalChanceToCrushingBlow;
    }
    return 0.0f;
}

// Original PowersSwig.cpp:3435.
void PowerPayloadSetAdditionalCrushingBlowChance(int32 nPayloadID, float flCrushingBlowChance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->flPowerAdditionalChanceToCrushingBlow = flCrushingBlowChance;
    }
}

// Original PowersSwig.cpp:3451.
void PowerPayloadSetCritDamageBonusPercent(int32 nPayloadID, float flCritBonusPercent)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->flPowerCritDamageBonusPercent = flCritBonusPercent;
    }
}

// Original PowersSwig.cpp:3468.
void PowerPayloadSetCritCap(int32 nPayloadID, float flCritCap)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->flCritCap = flCritCap;
    }
}

// Original PowersSwig.cpp:3484.
void PowerPayloadSetCanCrit(int32 nPayloadID, BOOL bCanCrit)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bCanCrit = bCanCrit;
    }
}

// Original PowersSwig.cpp:3501.
void PowerPayloadSetForceCriticalDeath(int32 nPayloadID, BOOL bForceCriticalDeath)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bForceCriticalDeath = bForceCriticalDeath;
    }
}

// Original PowersSwig.cpp:3518.
void PowerPayloadSetAlwaysHits(int32 nPayloadID, BOOL bAlwaysHits)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bAlwaysHits = bAlwaysHits;
    }
}

// Original PowersSwig.cpp:3534.
void PowerPayloadSetAlwaysCauseGetHit(int32 nPayloadID, BOOL bAlwaysCauseGetHit)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bAlwaysCauseGetHit = bAlwaysCauseGetHit;
    }
}

// Original PowersSwig.cpp:3550.
void PowerPayloadSetNeverCauseGetHit(int32 nPayloadID, BOOL bNeverCauseGetHit)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bNeverCauseGetHit = bNeverCauseGetHit;
    }
}

// Original PowersSwig.cpp:3566.
void PowerPayloadSetIgnoreDamageBonusAndReduction(int32 nPayloadID, BOOL bIgnore)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bIgnoreDamageBonusAndReduction = bIgnore;
    }
}

// Original PowersSwig.cpp:3582.
void PowerPayloadSetTriggersHitProcs(int32 nPayloadID, BOOL bTriggerHitProcs)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bTriggersHitProcs = bTriggerHitProcs;
    }
}

// Original PowersSwig.cpp:3600.
void PowerPayloadAddStun(int32 nPayloadID, float flStunChance, float flDuration, float flDurationDelta)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->flChanceToStun = flStunChance;
        ptPayload->rangeStunLength.flMin = flDuration * TICKS_PER_SECOND;
        ptPayload->rangeStunLength.flDelta = flDurationDelta * TICKS_PER_SECOND;
    }
}

// Original PowersSwig.cpp:3621.
void PowerPayloadAddFear(int32 nPayloadID, float flDuration, float flDurationDelta)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->rangeFearLength.flMin = flDuration * TICKS_PER_SECOND;
        ptPayload->rangeFearLength.flDelta = flDurationDelta * TICKS_PER_SECOND;
    }
}

// Original PowersSwig.cpp:3640.
void PowerPayloadAddRoot(int32 nPayloadID, float flDuration, float flDurationDelta)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->rangeImmobolizeLength.flMin = flDuration * TICKS_PER_SECOND;
        ptPayload->rangeImmobolizeLength.flDelta = flDurationDelta * TICKS_PER_SECOND;
    }
}

// Original PowersSwig.cpp:3659.
void PowerPayloadAddFreeze(int32 nPayloadID, float flDuration, float flDurationDelta, float flBreakChance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->rangeFreezeLength.flMin = flDuration * TICKS_PER_SECOND;
        ptPayload->rangeFreezeLength.flDelta = flDurationDelta * TICKS_PER_SECOND;
        ptPayload->flFreezeBreakChance = flBreakChance;
    }
}

// Original PowersSwig.cpp:3680.
void PowerPayloadAddSlow(int32 nPayloadID, float flSlowAmount, float flDuration, float flDurationDelta)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->flSlowAmount = flSlowAmount;
        ptPayload->rangeSlowLength.flMin = flDuration * TICKS_PER_SECOND;
        ptPayload->rangeSlowLength.flDelta = flDurationDelta * TICKS_PER_SECOND;
    }
}

// Original PowersSwig.cpp:3702.
void PowerPayloadAddChill(int32 nPayloadID, float flChillAmount, float flDuration, float flDurationDelta)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bChill = TRUE;
        ptPayload->flChillMagnitude = flChillAmount;
        ptPayload->rangeChillLength.flMin = flDuration * TICKS_PER_SECOND;
        ptPayload->rangeChillLength.flDelta = flDurationDelta * TICKS_PER_SECOND;
    }
}

// Original PowersSwig.cpp:3724.
void PowerPayloadAddBlind(int32 nPayloadID, float flDuration, float flDurationDelta, BOOL bBreaksOnDamage)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->rangeBlindLength.flMin = flDuration * TICKS_PER_SECOND;
        ptPayload->rangeBlindLength.flDelta = flDurationDelta * TICKS_PER_SECOND;
        ptPayload->bBlindBreaksOnDamage = bBreaksOnDamage;
    }
}

// Original PowersSwig.cpp:3744.
void PowerPayloadAddAmbientDamageEffect(int32 nPayloadID, BOOL bCausesAmbientDamageEffects)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bCausesAmbientDamageEffects = bCausesAmbientDamageEffects;
    }
}

// Original PowersSwig.cpp:3761.
void PowerPayloadSetKnockback(int32 nPayloadID, float flMagnitude, float flHeight, float flGravity, BOOL bIgnoreWeight, BOOL bAffectedByReduction, LPCSTR szKnockbackPower, BOOL bContinuousKnockbackPower)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload == NULL) {
        return;
    }
    if (szKnockbackPower == NULL) {
        szKnockbackPower = "";
    }
    ptPayload->wdKnockbackMagnitude = flMagnitude;
    ptPayload->bKnockbackIgnoresWeight = bIgnoreWeight;
    ptPayload->flKnockbackHeightMin = flHeight;
    ptPayload->flKnockbackHeightDelta = 0.0f;
    ptPayload->flKnockbackGravityMin = flGravity;
    ptPayload->flKnockbackGravityDelta = 0.0f;
    ptPayload->snoKnockbackPower = SNOFileLookupGUID(POWER_SNO, szKnockbackPower);
    ptPayload->bKnockbackAffectedByReduction = bAffectedByReduction;
    ptPayload->bContinuousKnockbackPower = bContinuousKnockbackPower;
    if (flHeight < 1e-06f || (flGravity > -1e-06f && flGravity < 1e-06f)) {
        CRefString sError;
        sError.Format("PowerPayloadSetKnockback: PowerSNO (%s) Power ID (ID - %d) Index (%d) tried to apply payload that has a knockback with Height or Gravity <= Epsilon (.000001), This causes undefined behaviour, use a force grip CC instead if meaning to do standstill CC!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), ptPower->id, nPayloadID);
    }
    LuaVerifySnosAreReferenced(ptPayload->snoKnockbackPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
}

// Original PowersSwig.cpp:3812.
void PowerPayloadSetKnockbackStyle(int32 nPayloadID, KnockbackStyle eKnockbackStyle)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->eKnockBack = eKnockbackStyle;
    }
}

// Original PowersSwig.cpp:3829.
void PowerPayloadSetKnockbackFromPlacePositionFromCursorPosition(int32 nPayloadID, int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptPayload != NULL && ptCursor != NULL) {
        ptPayload->tKnockbackPlace = ptCursor->tPlace;
    }
}

// Original PowersSwig.cpp:3848.
void PowerPayloadSetKnockbackDirectionFromCursorFacing(int32 nPayloadID, int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptPayload != NULL && ptCursor != NULL) {
        Vector3D wvDirection = cgwvXAxis;
        Vector3D::Rotate(cgwvZAxis, ptCursor->qOrientation.ToYaw(), wvDirection);
        ptPayload->wvKnockbackDirection = wvDirection;
    }
}

// Original PowersSwig.cpp:3869.
BOOL PowerActorIsBeingKnockedBack(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return FALSE;
    }
    return PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_KNOCKBACK), ptACD, ACDID_INVALID) != BUFFID_INVALID;
}

// Original PowersSwig.cpp:3892.
BOOL PowerActorIsSpecialWalking(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return FALSE;
    }
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    SNO snoCurrentPower = ActorPowersGetPrimaryPower(idActor);
    if (snoCurrentPower == SNO_NONE) {
        return FALSE;
    }
    if (PowerGetTemplate(AutoPowerDefinitionR(snoCurrentPower), idACD) == SPECIALWALK_POWER) {
        return TRUE;
    }
    return FALSE;
}

// Original PowersSwig.cpp:3919.
void PowerPayloadSetDeliveryMechanismMelee(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->eDeliveryMechanism = PDM_MELEE;
    }
}

// Original PowersSwig.cpp:3935.
void PowerPayloadSetDeliveryMechanismRanged(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->eDeliveryMechanism = PDM_RANGED;
    }
}

// Original PowersSwig.cpp:3950.
void PowerPayloadSetDeliveryMechanismDOT(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->eDeliveryMechanism = PDM_DOT;
    }
}

// Original PowersSwig.cpp:3965.
void PowerPayloadSetDeliveryMechanismThorns(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->eDeliveryMechanism = PDM_THORNS;
    }
}

// Original PowersSwig.cpp:3980.
void PowerPayloadSetDeliveryMechanismMagic(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->eDeliveryMechanism = PDM_MAGIC;
    }
}

// Original PowersSwig.cpp:3996.
BOOL PowerPayloadApplyToTarget(int32 nPayloadID, VACDID idACDTarget, VACDID idACDOverrideAttacker)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->dwFlags & (1 << PF_DISALLOW_PAYLOAD_APPLICATION)) {
        return FALSE;
    }
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload == NULL) {
        return FALSE;
    }
    return PowerAPI_DoPayload(*ptPayload, idACDTarget, idACDOverrideAttacker);
}

// Original PowersSwig.cpp:4021.
// DWARF: sPowerGetChooseTargetsAtIndex inlined at 4037, listTargets at 4042, sPowerGetAttackPayloadAtIndex at 4045.
int32 PowerPayloadApplyToTargets(int32 nPayloadID, int32 nTargetAreaID, VACDID idACDOverrideAttacker)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->dwFlags & (1 << PF_DISALLOW_PAYLOAD_APPLICATION)) {
        return 0;
    }
    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (ptChooseTargets == NULL) {
        return 0;
    }
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(*ptChooseTargets, listTargets);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload == NULL) {
        return 0;
    }
    return PowerAPI_DoPayload(*ptPayload, listTargets, idACDOverrideAttacker);
}










// Original PowersSwig.cpp:4054.
void PowerPayloadSetFloatingCombatTextSourceOverride(int32 nPayloadID, VACDID idACDDest)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->idACDFloatingCombatTextSourceOverride = idACDDest;
    }
}

// Original PowersSwig.cpp:4076.
void PowerPayloadSetProcScalar(int32 nPayloadID, float flProcScalar)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    // Powers that do not animate only proc when they explicitly allow it.

    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        if (ptPower->bDoesntAnimate && !ptPower->bAllowHitProcs) {
            ptPayload->flProcScalar = 0.0f;
        } else {
            ptPayload->flProcScalar = flProcScalar;
        }
    }
}

// Original PowersSwig.cpp:4105.
void PowerPayloadSetIgnoreCharmDamageScalar(int32 nPayloadID, BOOL bIgnore)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bIgnoreCharmDamageScalar = bIgnore;
    }
}

// Original PowersSwig.cpp:4127.
void PowerPayloadDisableHitEffects(int32 nPayloadID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload * ptPayload = sPowerGetAttackPayloadAtIndex(ptPower, nPayloadID);
    if (ptPayload != NULL) {
        ptPayload->bDisableHitEffects = TRUE;
    }
}


// Original PowersSwig.cpp:4149.
void PowerClearTargetCriteria()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ptPower->tTargetCriteria.eLifeStatus = TCLS_ALIVE_ONLY;
    ptPower->tTargetCriteria.dwTeamRelationshipFlags = (1 << TCTR_IGNORES_WRECKABLES);
}

// Original PowersSwig.cpp:4167.
void PowerSetTargetsAllies(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_ALLIES);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_AFFECTS_ALLIES);
    }
}

// Original PowersSwig.cpp:4178.
void PowerSetTargetsEnemies(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_ENEMIES);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_AFFECTS_ENEMIES);
    }
}

// Original PowersSwig.cpp:4188.
void PowerSetTargetsPlayersOnly(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_IGNORES_NON_PLAYERS);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_NON_PLAYERS);
    }
}

// Original PowersSwig.cpp:4199.
void PowerSetTargetsWreckables(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (!bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_IGNORES_WRECKABLES);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_WRECKABLES);
    }
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_SCENERY);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_AFFECTS_SCENERY);
    }
}

// Original PowersSwig.cpp:4211.
void PowerSetTargetsCorpses(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.eLifeStatus = TCLS_DEAD_ONLY;
    } else {
        ptPower->tTargetCriteria.eLifeStatus = TCLS_ALIVE_ONLY;
    }
}

// Original PowersSwig.cpp:4229.
void PowerSetTargetsNecroCorpses(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_NECRO_CORPSES_ONLY);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_AFFECTS_NECRO_CORPSES_ONLY);
    }
}

// Original PowersSwig.cpp:4240.
void PowerSetIgnoresPets(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_IGNORES_PETS);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_PETS);
    }
}

// Original PowersSwig.cpp:4250.
void PowerSetIgnoresPlayers(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_IGNORES_PLAYERS);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_PLAYERS);
    }
}

// Original PowersSwig.cpp:4260.
void PowerSetIgnoresOwner(BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bEnable) {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_IGNORES_CASTER);
    } else {
        ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_CASTER);
    }
}


// Original PowersSwig.cpp:4271.
int32 PowerCreateCursor()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    int32 nIndex = ptPower->listCursors.Count();
    PowerCursor tCursor;
    tCursor.tPlace = ptPower->wpStartLocation;
    tCursor.qOrientation = ptPower->qStartOrientation;
    ptPower->listCursors.AddTail_Tracked(tCursor);
    if (nIndex >= MAX_CURSORS_PER_POWER) {
        CRefString sError;
        sError.Format("PowerCreateCursor: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated cursors per power (%d)! Likely the script is creating cursors in a callback and is not reusing them; causing a large leak of cursors!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_PAYLOADS_PER_POWER);
    }
    return nIndex;
}

// Original PowersSwig.cpp:4293.
void PowerCursorDeleteLastCreated()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower != NULL && ptPower->listCursors.Count() > 0) {
        ptPower->listCursors.RemoveTail();
    }
}

// Original PowersSwig.cpp:4320.
void PowerCursorReset(int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor == NULL) {
        return;
    }
    ptCursor->tPlace = ptPower->wpStartLocation;
    ptCursor->qOrientation = ptPower->qStartOrientation;
}

// Declarations from PowerScript.cpp:1135,1221,1949,2007.
// PowerScript.h has not adopted the callbacks.
extern void ScriptedPower_BuffOnDoDamageEffectCallback(PowerAPIBuffInstance * ptBuff, PowerAPIDamage & tPowerDamage, float * flDamageDone, ACDID idACDAttacker, ACDID idACDTarget);
extern void ScriptedPower_BuffOnHitEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tPowerDamage, float flHitpointDamage, float * flDamageDone, ACDID idACDAttacker, ACDID idACDTarget, ACDID idACDFromPet);
extern void ScriptedPower_BuffOnGetHitWithCrowdControlEffectCallback(PowerID idPowerFrom, PowerAPIBuffInstance * ptBuff, ACDID idACDAttacker, ACDID idACDTarget, SNO snoPower, CrowdControlType eCCtype, float flDuration, float flProcScalar);
extern void ScriptedPower_BuffOnHitWithCrowdControlEffectCallback(PowerID idPowerFrom, PowerAPIBuffInstance * ptBuff, ACDID idACDAttacker, ACDID idACDTarget, SNO snoPower, CrowdControlType eCCtype, float flDuration, float flProcScalar);
extern BOOL ScriptedPower_BuffOnReflectEffectCallback(PowerAPIBuffInstance * ptBuff, ImpactData * ptImpact, void * pData);  // PowerScript.cpp:1780

// Declarations from PowerScript.cpp:1178,1309,1358,1404,1466,1548,1612,1648,1815.
// PowerScript.h has not adopted the callbacks.
extern void ScriptedPower_BuffOnGetHitEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDTarget);
extern void ScriptedPower_BuffOnAttackEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACDAttacker, ACDID idACDTarget, PowerID idPowerFrom, SNO snoPower);
extern void ScriptedPower_BuffOnDodgeEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACDAttacker, ACDID idACDTarget, const PowerAPIAttackPayload & tPayload);
extern void ScriptedPower_BuffOnCostEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACD, PowerID idPowerFrom, SNO snoPower, float flResourceCost, ResourceType eResourceType, float flHealthCost, BOOL bCostsWereCharged, float flHealthCostUndiscounted);
extern void ScriptedPower_BuffOnHealFromSkillEffectCallback(PowerAPIBuffInstance * ptBuff, ACDID idACD, PowerID idPowerFrom, SNO snoPower, float flHealAmount, float flOverhealAmount, int32 nNumberOfHeals);
extern void ScriptedPower_BuffOnKillEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, float * flDamageDone, PowerAPIDamage * ptPowerDamage, ACDID idACDAttacker, ACDID idACDKilled);
extern void ScriptedPower_BuffOnAssistEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACDAttacker, ACDID idACDKilled);
extern void ScriptedPower_BuffOnItemPickUpEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACD, ACDID idACDItem, PickupAction ePickupAction);
extern void ScriptedPower_BuffOnProjectileCreateEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACDAttacker, ACDID idACDProjectile);

// Original PowerScript.cpp callbacks verified independently in P06-F11 evidence.
extern void ScriptedPower_BuffOnExpirationEffectCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, void * pData);
extern void ScriptedPower_BuffOnRemovalEffectCallback(PowerAPIBuffInstance * ptBuff, BuffRemovalReason eBuffRemovalReason, void * pData);
extern void ScriptedPower_BuffOnDeathEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACDTarget, ACDID idACDKiller);
extern void ScriptedPower_BuffOnCalloutEventCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDActor, CalloutEventType eEvent);
extern void ScriptedPower_BuffOnGizmoOperatedCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDActor, ActorCommonData & tACDGizmo);
extern void ScriptedPower_BuffOnWreckableDestroyedCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDAttacker, ACDID idACDKilled);
extern void ScriptedPower_BuffOnTeleportedCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDActor);
extern void ScriptedPower_BuffOnPowerDamageInitCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDSource, ACDID idACDTarget, PowerAPIDamage & tPowerDamage);
extern void ScriptedPower_BuffOnLootDropCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDPlayer, ActorCommonData & tACDItem, LootContext eLootContext);
extern void ScriptedPower_BuffOnItemSoldCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDPlayer, ActorCommonData & tACDItem);
extern void ScriptedPower_BuffOnLevelCallback(PowerAPIBuffInstance * ptBuff, ACDID idACD);
extern void ScriptedPower_BuffOnCorpseConsumedCallback(PowerAPIBuffInstance * ptBuff, ACDID idACD, ACDID idACDCorpse);
extern void ScriptedPower_BuffOnPetDismissCallback(PowerAPIBuffInstance * ptBuff, ACDID idACD, ACDID idACDPet);
extern void ScriptedPower_BuffOnKnockbackLandEffectCallback(PowerAPIBuffInstance * ptBuff);
extern void ScriptedPower_BuffOnPetAddEffectCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDSummoner, ACDID idACDPet, PetType ePetType, SNO snoPet);
extern void ScriptedPower_BuffOnEquipmentChangeEffectCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDActor, InventorySlotType eSlot, EquipmentChangeReason eReason);
extern void ScriptedPower_BuffOnResourceGainEffectCallback(PowerAPIBuffInstance * ptBuff, float flResourceAdded);
extern void ScriptedPower_BuffOnFirstHitAttemptEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData, ACDID idACDAttacker, ACDID idACDTarget, PowerID idFirstHitPower, SNO snoFirstHitPower);
extern void ScriptedPower_BuffOnMessageEffectCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDAffected, int32 dwParam0, int32 dwParam1, float flParam2);
extern void ScriptedPower_BuffOnSkillChangeEffectCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDActor);
extern void ScriptedPower_BuffPeriodicSpellEffectCallback(PowerAPIBuffInstance * ptBuff, void * pData);  // PowerScript.cpp:1107
extern void ScriptedPower_BuffDamageShieldEffectCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, float * flDamageDone, PowerAPIDamage & tPowerDamage, void * pData);  // PowerScript.cpp:1508

// Declared here because no adopted header has them yet.
extern uint32 StringHashI(LPCSTR pszName);  // X/Sigma/TypeDescriptor/SymbolicValue.cpp:117
extern BOOL PowerGetBuffAttributes(int32 nBuffIndex, Attrib & eAttribStart, Attrib & eAttribEnd, Attrib & eAttribCount);  // X/Common/Powers/PowersCommon.cpp:5977

// Original PowersSwig.cpp:4336.
void PowerCursorSetToCastTarget(int32 nCursorID, int32 idPowerForTarget)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor == NULL) {
        return;
    }
    Power * ptPowerForTarget = idPowerForTarget != POWERID_INVALID ? PowerTryToGet(idPowerForTarget) : NULL;
    if (ptPowerForTarget == NULL) {
        ptPowerForTarget = ptPower;
    }
    if (ptPowerForTarget->tTarget.IsInvalid()) {
        return;
    }
    ptPowerForTarget->tTarget.GetPlace(ptCursor->tPlace);
}

// Original PowersSwig.cpp:4362.

void PowerCursorSetToCursor(int32 nCursorID, int32 nCursorToCopyID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorToCopy = sPowerGetCursorAtIndex(ptPower, nCursorToCopyID, TRUE);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursorToCopy != NULL && ptCursor != NULL) {
        *ptCursor = *ptCursorToCopy;
    }
}

// Original PowersSwig.cpp:4381.
void PowerCursorSetToCursorHeight(int32 nCursorID, int32 nCursorToCopyID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorToCopy = sPowerGetCursorAtIndex(ptPower, nCursorToCopyID, TRUE);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursorToCopy != NULL && ptCursor != NULL) {
        ptCursor->tPlace.wp.z = ptCursorToCopy->tPlace.wp.z;
    }
}

// Original PowersSwig.cpp:4399.
void PowerCursorRollToCursor(int32 nCursorID, int32 nDestCursorID, float flDestPerturbRadiusMax, LPCSTR szPowerPassability)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    PowerCursor * ptDestCursor = sPowerGetCursorAtIndex(ptPower, nDestCursorID, TRUE);
    if (ptCursor && ptDestCursor) {
        ptCursor->qOrientation = ptDestCursor->qOrientation;

        const ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
        if (ptACD == NULL) {
            return;
        }
        const ActorMovement * ptACM = ptACD->GetActorMovement();
        if (ptACM == NULL) {
            return;
        }

        SNO snoPowerPassability = SNO_NONE;
        if (szPowerPassability != NULL) {
            snoPowerPassability = SNOFileLookupGUID(POWER_SNO, szPowerPassability);
        }
        if (snoPowerPassability == SNO_NONE) {
            snoPowerPassability = ptPower->snoPower;
        }

        WorldPlace tPlaceFound;
        PowerComputeDestination(snoPowerPassability, *ptACM, ptDestCursor->tPlace, ptCursor->tPlace, tPlaceFound, FALSE, flDestPerturbRadiusMax);
        ptCursor->tPlace = tPlaceFound;
    }
}

// Original PowersSwig.cpp:4466.
void PowerCursorPathToCursor(int32 nCursorID, int32 nDestCursorID, LPCSTR szPowerPassability)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    PowerCursor * ptDestCursor = sPowerGetCursorAtIndex(ptPower, nDestCursorID, TRUE);
    if (ptCursor && ptDestCursor) {
        SNO snoPowerPassability = SNO_NONE;
        if (szPowerPassability != NULL) {
            snoPowerPassability = SNOFileLookupGUID(POWER_SNO, szPowerPassability);
        }
        if (snoPowerPassability == SNO_NONE) {
            snoPowerPassability = ptPower->snoPower;
        }

        ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);
        if (ptACDOwner == NULL) {
            return;
        }
        ActorMovement * ptMove = ptACDOwner->GetActorMovement();
        if (ptMove == NULL) {
            return;
        }

        ptCursor->tPlace = PowerHelper_ComputePathableDestination(snoPowerPassability, ptCursor->tPlace, ptDestCursor->tPlace, ptMove->GetRadius(FALSE), *ptACDOwner);
    }
}

// Original PowersSwig.cpp:4584.
BOOL PowerCursorIsOnNavMesh(int32 nCursorID, BOOL bWalkableOnly)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return FALSE;
    }
    if (bWalkableOnly) {
        return NavCellGetStaticFlags(ptCursor->tPlace) & 1;
    }
    NavCellHandle hNavCell;
    return NavCellHandleFromWorldPlace(ptCursor->tPlace, hNavCell) && hNavCell.uCellIndex != (uint32)-1;
}

// Original PowersSwig.cpp:4610.
void PowerCursorSetToActor(int32 nCursorID, VACDID idActor, BOOL bWantOrientation)
{
    if (ACDTryToGet(idActor) == NULL) {
        return;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    const ActorCommonData & tACD = ACDGet(idActor);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor == NULL) {
        return;
    }
    ptCursor->tPlace = tACD.GetPlace();
    if (bWantOrientation) {
        ptCursor->qOrientation = tACD.GetOrientation();
    }
}

// Original PowersSwig.cpp:4642.
BOOL PowerCursorIsInSameWorldAsActor(int32 nCursorID, ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return FALSE;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return FALSE;
    }
    return WorldPlaceInSameWorld(ptCursor->tPlace, ptACD->GetPlace());
}

// Original PowersSwig.cpp:4664.
void PowerCursorSetToContactOffset(int32 nCursorID, int32 nContactFrame)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursorSetToActor(nCursorID, ptPower->idACD, TRUE);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor != NULL) {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (nContactFrame >= 0 && nContactFrame < 4 && ptACDOwner != NULL) {
            PowerHelper_GetContactOffsetForPower(idPower, nContactFrame, ptCursor->tPlace.wp, TRUE);
        }
    }
}

// Original PowersSwig.cpp:4698.
void PowerCursorSnapToGround(int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    if (NavMeshHandleIsNone(tNavPlaceSource.tNavMeshHandle)) {
        return;
    }
    if (NavMeshInstGetByScene(tNavPlaceSource.tNavMeshHandle.idSScene) == NULL) {
        return;
    }
    ptCursor->tPlace.wp.z = NavMeshGetHeightWorldSpace(tNavPlaceSource.tNavMeshHandle);
}

// Original PowersSwig.cpp:4726.
void PowerCursorSetToProjectileCastTarget(int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor == NULL) {
        return;
    }

    ActorCommonData * ptACDOwner = ACDTryToGet(PowerGetOwner());
    Vector3D wdContactOffset(0.0f, 0.0f, 0.0f);
    PowerHelper_GetContactOffsetForPower(idPower, ptPower->dwCurrentContactFrame, wdContactOffset, FALSE);

    const Target tTarget = ptPower->tTarget;
    TargetType eTargetType = tTarget.eType;
    if (eTargetType == TT_POINT) {
        ptCursor->tPlace = tTarget.tPlace;
        if (ACDIsPlayer(ptPower->idACD)) {
            return;
        }
        ptCursor->tPlace.wp += wdContactOffset;
        return;
    }

    if (eTargetType == TT_ACTOR || eTargetType == TT_ACTOR_WITH_FALLBACK_POINT) {
        const ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.GetACD());
        if (ptACDTarget != NULL) {
            ptCursor->tPlace = ptACDTarget->GetPlace();
            ptCursor->tPlace.wp.z += wdContactOffset.z;
            if (ptACDOwner != NULL) {
                Vector3D vecTarget = ptACDTarget->GetPlace().wp;
                Vector3D vecOwner = ptACDOwner->GetPlace().wp;
                Vector3D vecToTarget = vecTarget - vecOwner;

                float flContact2DMagSqr = ((Vector2D &)wdContactOffset).MagnitudeSquared();
                float flToTarget2DMagSqr = ((Vector2D &)vecToTarget).MagnitudeSquared();

                if (flToTarget2DMagSqr < flContact2DMagSqr + 20.0f) {
                    Vector3D wpNewTarget = vecOwner;
                    wpNewTarget += wdContactOffset;
                    vecTarget.Normalize();
                    wpNewTarget += vecToTarget * 4.4721f;
                    ptCursor->tPlace.wp = wpNewTarget;
                }
            }
            return;
        }

        if (eTargetType == TT_ACTOR_WITH_FALLBACK_POINT) {
            ptCursor->tPlace = tTarget.tPlace;
        } else if (ptACDOwner == NULL) {
            ptCursor->tPlace = ptPower->wpStartLocation;
        } else {
            ptCursor->tPlace = ptACDOwner->GetPlace();
            (Vector2D &)ptCursor->tPlace.wp += (Vector2D &)wdContactOffset;
            PowerCursorOffsetFromOwner(nCursorID, 4.4721f, TRUE);
            ptCursor->tPlace.wp.z = wdContactOffset.z;
        }
    }
}

// Original PowersSwig.cpp:4822.
void PowerCursorSetToLobbedProjectileCastTarget(int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);
    if (ptACDOwner == NULL) {
        return;
    }
    if (ptPower->tTarget.IsInvalidForSource(PowerGetOwner(idPower))) {
        return;
    }

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor == NULL) {
        return;
    }

    WorldPlace placeTarget;
    PowerGetTargetPlace(AutoPowerDefinitionR(ptPower->snoPower), ptPower->tTarget, placeTarget);
    if (!WorldPlaceInSameWorld(ptACDOwner->GetPlace(), placeTarget)) {
        return;
    }
    ptCursor->tPlace = placeTarget;
}

// Original PowersSwig.cpp:4858.
void PowerCursorSetToPowerSource(int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return;
    }
    if (PowerGetOwner() == 0) {
        return;
    }
    NavPlace tNavPlace;
    PowerGetACDPoint(idPower, ptPower->idACD, tNavPlace, -1, -1, NULL);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor == NULL) {
        return;
    }
    ptCursor->tPlace = tNavPlace.tPlace;
}

// Original PowersSwig.cpp:4895.
BOOL PowerCursorSetToOpenLocationNearActor(int32 nCursorID, VACDID idActor, float flSkipDistance)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return FALSE;
    }
    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (ptACDTarget == NULL) {
        return FALSE;
    }
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    if (ptCursor == NULL) {
        return FALSE;
    }

    WorldPlace tPlaceOpen = ptACDTarget->GetPlace();
    PowerAPIChooseOpenGround tOpenGroundInfo;
    flSkipDistance = Math::Pin(flSkipDistance, 0, 20);
    NavPlace tNavPlace;
    PowerPlaceToNavMeshHandle(tPlaceOpen, tNavPlace.tNavMeshHandle);
    tNavPlace.tPlace = tPlaceOpen;
    PowerHelper_ChooseOpenGroundInit(idPower, tNavPlace, tOpenGroundInfo, flSkipDistance);
    NavMeshHandle hNavMeshCurr;
    BOOL bSuccess = PowerAPI_DoChooseOpenGround(*ptACDOwner, tOpenGroundInfo, tPlaceOpen, hNavMeshCurr);
    if (bSuccess) {
        ptCursor->tPlace = tPlaceOpen;
    }
    return bSuccess;
}

// Original PowersSwig.cpp:4940.
BOOL PowerCursorSetToOpenLocationNearCursor(int32 nCursorID, int32 nTargetCursorID, LPCSTR szActorSNO)
{
    float flScale;
    SNO snoActor = SNO_NONE;
    PowerID idPower = sGetCurrentPower();
    if (szActorSNO != NULL) {
        snoActor = SNOFileLookupGUID(ACTOR_SNO, szActorSNO);
    }
    if (snoActor == SNO_NONE) {
        ActorCommonData * ptACD = PowerGetOwner(idPower);
        if (ptACD == NULL) {
            return FALSE;
        }
        snoActor = ptACD->snoActor;
        flScale = ptACD->GetScale();
    } else {
        flScale = ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_SCALE_MIN, snoActor) + ActorDefinitionGetTagMapValueFloat(TAG_ACTOR_SCALE_DELTA, snoActor);
    }

    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, FALSE);
    PowerCursor * ptCursorTarget = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (ptCursor && ptCursorTarget) {
        WorldPlace tPlaceOpen = ptCursorTarget->tPlace;
        PowerAPIChooseOpenGround tOpenGroundInfo;
        NavPlace tNavPlace;
        BOOL bSuccess = FALSE;
        if (PowerPlaceToNavMeshHandle(tPlaceOpen, tNavPlace.tNavMeshHandle)) {
            NavMeshHandle hNavMeshCurr;
            tNavPlace.tPlace = tPlaceOpen;
            PowerHelper_ChooseOpenGroundInit(idPower, tNavPlace, tOpenGroundInfo, 0.0f);
            tOpenGroundInfo.bUseActorPassability = (szActorSNO != NULL);
            bSuccess = PowerAPI_DoChooseOpenGround(snoActor, flScale, tOpenGroundInfo, tPlaceOpen, hNavMeshCurr);
            if (bSuccess) {
                ptCursor->tPlace = tPlaceOpen;
            }
        }
        return bSuccess;
    }
    return FALSE;
}

// Original PowersSwig.cpp:5013.
void PowerCursorRotateAroundOwner(int32 nCursorID, float flDegrees)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    Angle aRotation = Math::DegToAngle(flDegrees);
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD != NULL) {
        PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
        if (ptCursor != NULL) {
            WorldPlace wpOwner = ptACD->GetPlace();
            Vector3D wvToCursor = ptCursor->tPlace.wp - wpOwner.wp;
            Vector3D::Rotate(cgwvZAxis, aRotation, wvToCursor);
            ptCursor->tPlace.wp = wpOwner.wp + wvToCursor;
        }
    }
}

// Original PowersSwig.cpp:5041.
void PowerCursorRotateAroundActor(int32 nCursorID, VACDID idActor, float flDegrees)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptPower != NULL && ptACD != NULL) {
        Angle aRotation = Math::DegToAngle(flDegrees);
        PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
        if (ptCursor != NULL) {
            WorldPlace wpOwner = ptACD->GetPlace();
            Vector3D wvToCursor = ptCursor->tPlace.wp - wpOwner.wp;
            Vector3D::Rotate(cgwvZAxis, aRotation, wvToCursor);
            ptCursor->tPlace.wp = wpOwner.wp + wvToCursor;
        }
    }
}

// Original PowersSwig.cpp:5069.
void PowerCursorRotateAroundCursor(int32 nCursorID, int32 nCursorIDTarget, float flDegrees)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    PowerCursor * ptCursorTarget = sPowerGetCursorAtIndex(ptPower, nCursorIDTarget, TRUE);
    if (ptCursor && ptCursorTarget) {
        Vector3D wvOffset = ptCursor->tPlace.wp - ptCursorTarget->tPlace.wp;
        Vector3D::Rotate(cgwvZAxis, Math::DegToAngle(flDegrees), wvOffset);
        ptCursor->tPlace.wp = ptCursorTarget->tPlace.wp + wvOffset;
    }
}

// Original PowersSwig.cpp:5095.
void PowerCursorOffsetFromOwner(int32 nCursorID, float flDistance, BOOL bAs2D)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD != NULL) {
        WorldPlace wpOwner = ptACD->GetPlace();
        const float flMinDistanceThreshold = 0.0001f;
        PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
        if (ptCursor == NULL) {
            return;
        }

        if (bAs2D && Vector3D::Distance2DSquared(wpOwner.wp, ptCursor->tPlace.wp) < flMinDistanceThreshold) {
            Angle aFacing;
            Vector3D wvFacing = cgwvXAxis;
            aFacing = ptACD->GetOrientation().ToYaw();
            Vector3D::Rotate(cgwvZAxis, aFacing, wvFacing);
            wvFacing *= flDistance;
            ptCursor->tPlace.wp = wpOwner.wp + wvFacing;
        } else {
            Vector3D wvToCursor = ptCursor->tPlace.wp - wpOwner.wp;
            wvToCursor.Normalize();
            ptCursor->tPlace.wp = ptCursor->tPlace.wp + wvToCursor * flDistance;
        }
    }
}

// Original PowersSwig.cpp:5137.
void PowerCursorPinAtProjectileDistFromOwner(int32 nCursorID, float flMinDist, float flMaxDist)
{
    flMaxDist = Math::Max(flMinDist, flMaxDist);
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD != NULL) {
        WorldPlace wpOwner = ptACD->GetPlace();
        float flDistSqr;
        PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
        if (ptCursor == NULL) {
            return;
        }

        Vector3D vToCursor = ptCursor->tPlace.wp - wpOwner.wp;
        vToCursor.z = 0.0f;
        flDistSqr = vToCursor.MagnitudeSquared();
        if (flDistSqr > flMaxDist * flMaxDist) {
            vToCursor.Normalize();
            vToCursor *= flMaxDist;
            ptCursor->tPlace.wp = wpOwner.wp + vToCursor;
        } else if (flDistSqr < flMinDist * flMinDist) {
            Angle aYaw = ACDGetYaw(ptACD->id);
            Vector3D vFacing(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
            vFacing *= flMinDist;
            ptCursor->tPlace.wp = wpOwner.wp + vFacing;
        }
    }
}

// Original PowersSwig.cpp:5177.
void PowerCursorRotate(int32 nCursorID, float flDegrees)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    Angle aNewYaw = ptCursor->qOrientation.ToYaw() + Math::DegToAngle(flDegrees);
    ptCursor->qOrientation.SetRotation(cgwvZAxis, aNewYaw);
}

// Original PowersSwig.cpp:5196.
void PowerCursorRotateTowardsActor(int32 nCursorID, VACDID idActor)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }

    ActorCommonData & tACD = ACDGet(idActor);
    Angle aNewYaw;
    WorldPlace wpCursor = ptCursor->tPlace;
    WorldPlace wpTarget = tACD.GetPlace();
    if (wpCursor.idSWorld != wpTarget.idSWorld) {
        return;
    }
    Vector3D wvToTarget = wpTarget.wp - wpCursor.wp;
    Vector2D vToTarget(wvToTarget.x, wvToTarget.y);
    if (vToTarget.MagnitudeSquared() < 1e-06f) {
        return;
    }
    aNewYaw = vToTarget.ToYaw();
    ptCursor->qOrientation.SetRotation(cgwvZAxis, aNewYaw);
}

// Original PowersSwig.cpp:5230.
void PowerCursorRotateTowardsCursor(int32 nCursorID, int32 nTargetCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (ptCursor && ptTargetCursor) {
        Angle aNewYaw;
        WorldPlace wpCursor = ptCursor->tPlace;
        WorldPlace wpTarget = ptTargetCursor->tPlace;
        if (wpCursor.idSWorld != wpTarget.idSWorld) {
            return;
        }
        Vector3D wvToTarget = wpTarget.wp - wpCursor.wp;
        Vector2D vToTarget(wvToTarget.x, wvToTarget.y);
        if (vToTarget.MagnitudeSquared() < 1e-06f) {
            return;
        }
        aNewYaw = vToTarget.ToYaw();
        ptCursor->qOrientation.SetRotation(cgwvZAxis, aNewYaw);
    }
}

// Original PowersSwig.cpp:5262.
void PowerCursorRotateAbsolute(int32 nCursorID, float flDegrees)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    ptCursor->qOrientation.SetRotation(cgwvZAxis, Math::DegToAngle(flDegrees));
}

// Original PowersSwig.cpp:5280.
void PowerCursorOffset(int32 nCursorID, float flDistance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    Vector3D wvDirection = cgwvXAxis;
    Vector3D::Rotate(cgwvZAxis, ptCursor->qOrientation.ToYaw(), wvDirection);
    wvDirection *= flDistance;
    ptCursor->tPlace.wp = ptCursor->tPlace.wp + wvDirection;
}

// Original PowersSwig.cpp:5301.
void PowerCursorOffsetLaterally(int32 nCursorID, float flDistance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    Vector3D wvDirection = cgwvYAxis;
    Vector3D::Rotate(cgwvZAxis, ptCursor->qOrientation.ToYaw(), wvDirection);
    wvDirection *= flDistance;
    ptCursor->tPlace.wp = ptCursor->tPlace.wp + wvDirection;
}

// Original PowersSwig.cpp:5323.
void PowerCursorOffsetVertically(int32 nCursorID, float flDistance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    ptCursor->tPlace.wp.z += flDistance;
}

// Original PowersSwig.cpp:5340.
// The original subtracts wvContactOffset (fsub cursor - contact), not wpOffsetPos.wp; the += result is unused.
#line 5340
void PowerCursorOffsetFromContactOffset(
    int32 nCursorID,
    int32 nContactFrame,
    float flDistance)
{
    PowerID idPower = sGetCurrentPower();
    Power *ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL)
    {
        return;
    }


    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (nContactFrame < 0 || nContactFrame >= 4 || ptACDOwner == NULL)
    {
        return;
    }







    Vector3D wvContactOffset;
    PowerHelper_GetContactOffsetForPower(idPower, nContactFrame, wvContactOffset, TRUE);

    WorldPlace wpOffsetPos = ptACDOwner->GetPlace();
    wpOffsetPos.wp += wvContactOffset;

    Vector3D wvToCursor = ptCursor->tPlace.wp - wvContactOffset;
    wvToCursor.Normalize();

    wvToCursor *= flDistance;

    ptCursor->tPlace.wp += wvToCursor;
}
#line 3126

// Original PowersSwig.cpp:5381.
double PowerActorDotProductBetweenCursors(VACDID idActor, int32 nCursorFirstID, int32 nCursorSecondID)
{
    if (ACDTryToGet(idActor) == NULL) {
        return FLT_MAX;
    }

    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData & tACD = ACDGet(idActor);
    WorldPlace placeActor = tACD.GetPlace();
    PowerCursor * ptCursorFirst = sPowerGetCursorAtIndex(ptPower, nCursorFirstID, TRUE);
    PowerCursor * ptCursorSecond = sPowerGetCursorAtIndex(ptPower, nCursorSecondID, TRUE);
    if (ptCursorFirst && ptCursorSecond) {
        if (ptCursorFirst->tPlace.idSWorld != placeActor.idSWorld || ptCursorSecond->tPlace.idSWorld != ptCursorFirst->tPlace.idSWorld) {
            return FLT_MAX;
        }

    float dot;
    Vector3D wvFirst = ptCursorFirst->tPlace.wp - placeActor.wp;
    Vector3D wvSecond = ptCursorSecond->tPlace.wp - placeActor.wp;
    wvFirst.Normalize();
    wvSecond.Normalize();
        dot = Vector3D::DotProduct(wvFirst, wvSecond);
        return dot;
    }
    return FLT_MAX;
}

// Original PowersSwig.cpp:5427.
double PowerActorCrossProductBetweenCursors(VACDID idActor, int32 nCursorFirstID, int32 nCursorSecondID)
{
    if (ACDTryToGet(idActor) == NULL) {
        return FLT_MAX;
    }

    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData & tACD = ACDGet(idActor);
    WorldPlace placeActor = tACD.GetPlace();
    PowerCursor * ptCursorFirst = sPowerGetCursorAtIndex(ptPower, nCursorFirstID, TRUE);
    PowerCursor * ptCursorSecond = sPowerGetCursorAtIndex(ptPower, nCursorSecondID, TRUE);
    if (ptCursorFirst && ptCursorSecond) {
        if (ptCursorFirst->tPlace.idSWorld != placeActor.idSWorld || ptCursorSecond->tPlace.idSWorld != ptCursorFirst->tPlace.idSWorld) {
            return FLT_MAX;
        }

    Vector3D wvFirst = ptCursorFirst->tPlace.wp - placeActor.wp;
    Vector3D wvSecond = ptCursorSecond->tPlace.wp - placeActor.wp;
    wvFirst.Normalize();
    wvSecond.Normalize();
        Vector3D vCross = Vector3D::CrossProduct(wvFirst, wvSecond);
        return vCross.z;
    }
    return FLT_MAX;
}

// Original PowersSwig.cpp:5509.
void PowerPlayExplosionOnActor(VACDID idActor, LPCSTR szExplosionActorName)
{
    if (ACDTryToGet(idActor) == NULL) {
        return;
    }
    SNO snoExplosionActor = SNOFileLookupGUID(ACTOR_SNO, szExplosionActorName);
    if (snoExplosionActor == SNO_NONE) {
        return;
    }
    ActorCommonData & tACD = ACDGet(idActor);
    if (!tACD.IsInWorld()) {
        return;
    }
    PowerAPIExplosion tExplosion;
    tExplosion.snoExplosionActor = snoExplosionActor;
    tExplosion.qOrientation = tACD.GetOrientation();
    tExplosion.placeSource = tACD.GetPlace();
    tExplosion.dwDuration = TICKS_PER_SECOND;
    PowerAPI_DoExplosion(tExplosion);
}

// Original PowersSwig.cpp:5558.
void PowerPlayExplosionOnCursor(int32 nCursorID, LPCSTR szExplosionActorName)
{
    SNO snoExplosionActor = SNOFileLookupGUID(ACTOR_SNO, szExplosionActorName);
    if (snoExplosionActor == SNO_NONE) {
        return;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    PowerAPIExplosion tExplosion;
    tExplosion.snoExplosionActor = snoExplosionActor;
    tExplosion.qOrientation = ptCursor->qOrientation;
    tExplosion.placeSource = ptCursor->tPlace;
    tExplosion.dwDuration = TICKS_PER_SECOND;
    PowerAPI_DoExplosion(tExplosion);
}

// Original PowersSwig.cpp:5689.
ACDID PowerCreateLobbedProjectile(int32 nSourceCursorID, int32 nTargetCursorID, LPCSTR szProjectileName, float flGravity, float flHeightAboveSource, ProjectileHitType eHitType, BOOL bDontPerformMarbleRollToCaster, BOOL bUseProjectileStaticFlagsForSpawn)
{
    return PowerCreateLobbedProjectileWithBounce(nSourceCursorID, nTargetCursorID, szProjectileName, flGravity, flHeightAboveSource, 0.0f, 0.0f, eHitType, FALSE, bDontPerformMarbleRollToCaster, bUseProjectileStaticFlagsForSpawn);
}

// Original PowersSwig.cpp:6520.
// 6596 condition spelled from its columns (! at 13, || at 31, == at 53).
#line 6520
void PowerPlayProjectileExplosion(
    VACDID idActorProjectile,
    ACDID idActorHit,
    LPCSTR szExplosionActorName,
    LPCSTR szGroundExplosionActorName)
{
    WorldPlace placeExplosion;
    BOOL bGroundExplosion = FALSE;

    ActorCommonData * ptACDProjectile = ACDTryToGet(idActorProjectile);
    ActorCommonData * ptACDHit = ACDTryToGet(idActorHit);

    if (ptACDProjectile == NULL)
    {
        return;
    }




    if (!ptACDHit || !ptACDHit->IsTargetable())
    {

        placeExplosion = ptACDProjectile->GetPlace();
        bGroundExplosion = TRUE;
    }
    else
    {




        placeExplosion = ptACDHit->GetPlace();
        WorldPlace placeProj = ptACDProjectile->GetPlace();
        Vector3D vDelta = placeExplosion.wp - placeProj.wp;
        vDelta *= 0.5f;
        placeExplosion.wp.x = placeProj.wp.x + vDelta.x;
        placeExplosion.wp.y = placeProj.wp.y + vDelta.y;
        placeExplosion.wp.z = placeProj.wp.z;
    }

    SNO snoExplosion = SNOFileLookupGUID(ACTOR_SNO, szExplosionActorName);
    SNO snoGroundExplosion = SNO_NONE;
    BOOL bExists = (snoExplosion != SNO_NONE);

    if (szGroundExplosionActorName[0] != '\0')
    {
        snoGroundExplosion = SNOFileLookupGUID(ACTOR_SNO, szGroundExplosionActorName);
    }























    if (bExists)
    {
        PowerAPIExplosion tExplosion;

        if (!bGroundExplosion || snoGroundExplosion == SNO_NONE)
        {
            tExplosion.snoExplosionActor = snoExplosion;
        }
        else
        {
            tExplosion.snoExplosionActor = snoGroundExplosion;
        }

        tExplosion.qOrientation = cgqIdentity;
        tExplosion.placeSource = placeExplosion;
        tExplosion.dwDuration = TICKS_PER_SECOND;
        PowerAPI_DoExplosion(tExplosion);
    }
}
#line 3282

// Original PowersSwig.cpp:6613.
ACDID PowerProjectileGetImpactTarget(VACDID idActorProjectile, ACDID idActorHit)
{
    ActorCommonData * ptACDIntersected = ACDTryToGet(idActorHit);
    if (ptACDIntersected != NULL) {
        return idActorHit;
    }
    return idActorProjectile;
}

// Original PowersSwig.cpp:6628.
void PowerProjectileSeekCursor(ACDID idACDProjectile, int32 nCursorID, float flMaxAngleToRotate, BOOL bYawOnly)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL || !ACDIsProjectile(idACDProjectile)) {
        return;
    }
    ProjectileSeekPlace(idACDProjectile, ptCursor->tPlace, flMaxAngleToRotate, bYawOnly);
}

// Original PowersSwig.cpp:6666.
void ProjectileSeekActor(ACDID idACDProjectile, ACDID idACDTarget, float flMaxAngleToRotate, BOOL bYawOnly);  // Projectile.cpp:1490

void PowerProjectileSeekActor(ACDID idACDProjectile, ACDID idACDTarget, float flMaxAngleToRotate, BOOL bYawOnly)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (idACDTarget == ACDID_INVALID || ptPower == NULL) {
        return;
    }

    const ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL) {
        return;
    }

    if (!ACDIsProjectile(idACDProjectile)) {
        return;
    }

    if (PowerTargetsContactPlaneOnly(AutoPowerDefinitionR(ptPower->snoPower))) {
        Vector3D wdContactOffset(0.0f, 0.0f, 0.0f);
        PowerHelper_GetContactOffsetForPower(idPower, ptPower->dwCurrentContactFrame, wdContactOffset, FALSE);
        WorldPlace wpSeekDest = ptACDTarget->GetPlace();
        wpSeekDest.wp.z += wdContactOffset.z;
        ProjectileSeekPlace(idACDProjectile, wpSeekDest, flMaxAngleToRotate, bYawOnly);
    } else {
        ProjectileSeekActor(idACDProjectile, idACDTarget, flMaxAngleToRotate, bYawOnly);
    }
}

// Original PowersSwig.cpp:6720.
void ProjectileClearHitList(ACDID idACDProjectile);  // Projectile.cpp
void ProjectileAddActorToHitList(ACDID idACDProjectile, ACDID idACDActor);  // Projectile.cpp
BOOL ProjectileActorIsOnHitList(ACDID idACDProjectile, ACDID idACDActor);  // Projectile.cpp

void PowerProjectileClearHitList(ACDID idACDProjectile)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile == NULL) {
        return;
    }

    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ptPower->listAffectedACDs.RemoveAll();
    ProjectileClearHitList(idACDProjectile);
}

// Original PowersSwig.cpp:6740.
void PowerProjectileAddActorToHitList(VACDID idACDProjectile, VACDID idACDActor)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile == NULL) {
        return;
    }

    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorTryToGet(idActorProjectile);
    if (ptActorProjectile == NULL || ptActorProjectile->ptProjectile == NULL) {
        return;
    }

    ActorCommonData * ptACDActor = ACDTryToGet(idACDActor);
    if (ptACDActor == NULL) {
        return;
    }

    ProjectileAddActorToHitList(idACDProjectile, idACDActor);
}

// Original PowersSwig.cpp:6776.
BOOL PowerProjectileActorIsOnHitList(VACDID idACDProjectile, VACDID idACDActor)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile == NULL) {
        return FALSE;
    }

    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActorProjectile = ActorTryToGet(idActorProjectile);
    if (ptActorProjectile == NULL || ptActorProjectile->ptProjectile == NULL) {
        return FALSE;
    }

    ActorCommonData * ptACDActor = ACDTryToGet(idACDActor);
    if (ptACDActor == NULL) {
        return FALSE;
    }

    return ProjectileActorIsOnHitList(idACDProjectile, idACDActor);
}

// Original PowersSwig.cpp:6908.
void PowerProjectileSetSpeedMultiplier(ACDID idACDProjectile, float flMultiplier, float flDuration, BOOL bAdjustDetonationTimes, BOOL bSmoothSlow, BOOL bForce)
{
    if (ACDTryToGet(idACDProjectile) == NULL || flMultiplier < 1e-06f || !ACDIsProjectile(idACDProjectile)) {
        return;
    }
    ProjectileSetSpeedMultiplier(idACDProjectile, flMultiplier, Math::FloatToInt(flDuration * TICKS_PER_SECOND), bAdjustDetonationTimes, bSmoothSlow, bForce);
}

// Original PowersSwig.cpp:6939.
void PowerProjectileSetPierceChance(ACDID idACDProjectile, float flPierceChance)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile != NULL) {
        ptACDProjectile->AttributesSetFloat(ATTRIBUTE_PIERCE_CHANCE, flPierceChance);
    }
}

// Original PowersSwig.cpp:6953.
void PowerProjectileSetDetonateTime(ACDID idACDProjectile, int32 nDetonateTime)
{
    ProjectileSetDetonationTime(idACDProjectile, Math::Max(nDetonateTime, 0));
}

// Original PowersSwig.cpp:6966.
ACDID PowerProjectileGetOwner(ACDID idACDProjectile)
{
    if (!ACDIsProjectile(idACDProjectile)) {
        return ACDID_INVALID;
    }
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile == NULL) {
        return ACDID_INVALID;
    }
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    PowerID idPower = ActorPowersGetOwnerPower(idActorProjectile);
    Power * ptProjectilePower = PowerTryToGet(idPower);
    if (ptProjectilePower == NULL) {
        return ACDID_INVALID;
    }
    return ACDGetControllingActor(ptProjectilePower->idACD);
}

// Original PowersSwig.cpp:6992.
BOOL PowerProjectileIsReflected(ACDID idACDProjectile)
{
    if (!ACDIsProjectile(idACDProjectile)) {
        return FALSE;
    }
    return ProjectileIsReflected(idACDProjectile);
}

// Original PowersSwig.cpp:7004.
void PowerProjectileStopNow(ACDID idACDProjectile)
{
    if (!ACDIsProjectile(idACDProjectile)) {
        return;
    }
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActor = ActorTryToGet(idActorProjectile);
    if (ptActor != NULL) {
        ptActor->ptSActorMovement->StopProjectile();
    }
}

// Original PowersSwig.cpp:7024.
void PowerProjectileSetStopOnImpact(ACDID idACDProjectile, BOOL bStopOnImpact)
{
    if (!ACDIsProjectile(idACDProjectile)) {
        return;
    }
    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    Actor * ptActor = ActorTryToGet(idActorProjectile);
    if (ptActor != NULL) {
        ptActor->ptSActorMovement->SetStopOnImpact(bStopOnImpact);
    }
}

// Original PowersSwig.cpp:7045.
void PowerProjectilePlayAnim(VACDID idProjectile, ActorAnimationTag eAnimTag)
{
    if (!ACDIsProjectile(idProjectile)) {
        return;
    }
    ActorCommonData & tACD = ACDGet(idProjectile);
    if (AnimPlayerAnimExists(tACD, eAnimTag)) {
        AnimPlayerPlayAnimation(tACD, eAnimTag, (uint32)-1, FALSE, ANIM_REASON_POWER, 0.0f, FALSE, 1.0f);
    }
}

// Original PowersSwig.cpp:7064.
void PowerProjectileSetIdleAnim(VACDID idProjectile, ActorAnimationTag eAnimTag)
{
    if (!ACDIsProjectile(idProjectile)) {
        return;
    }
    ActorCommonData & tACD = ACDGet(idProjectile);
    if (AnimPlayerAnimExists(tACD, eAnimTag)) {
        AnimPlayerSetIdleAnimation(tACD, eAnimTag);
        if (AnimPlayerGetAnimReason(tACD) == ANIM_REASON_IDLE) {
            AnimPlayerSendIdle(tACD);
        }
    }
}

// Original PowersSwig.cpp:7399.
BOOL sPowerBeamUpdate(PowerID idPower, ACDID idACDTarget, float flMaxLength, BOOL bUseRollToClickPositionOnGround, SNO snoPowerPassability);  // PowersSwig.cpp; defined later, not written yet
int32 SComplexEffectAdd(ActorCommonData & tACD, const SNO snoEffectGroup, const ACDID idACDDestParam, int32 eType, uint32 dwParam1, uint32 dwParam2, BOOL bIgnoreOwnerAlpha);  // SComplexEffect.cpp:175; local declaration
enum { COMPLEXEFFECT_TYPE_BEAM_TARGET = 1 };  // ScriptUtil.cpp:286

ACDID PowerCreateBeam(LPCSTR szBeamEffectGroupName, LPCSTR szBeamTargetActorName, float flMaxLength, BOOL bUseRollToClickPositionOnGround, SNO snoPowerPassability)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);

    SNO snoSummonActor = SNOFileLookupGUID(ACTOR_SNO, szBeamTargetActorName);
    if (snoPowerPassability == SNO_NONE) {
        snoPowerPassability = ptPower->snoPower;
    }

    BOOL bTargetExists = (snoSummonActor != SNO_NONE);
    if (!bTargetExists) {
        return ACDID_INVALID;
    }

    PowerAPISummonActor tSummonInfo;
    BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoSummonActor, ACDID_INVALID, tSummonInfo, FALSE);
    if (ptACDOwner == NULL || !bSuccess) {
        return ACDID_INVALID;
    }

    WorldPlace tPlace = ptACDOwner->GetPlace();
    tSummonInfo.placeSummonLocation = tPlace;
    tSummonInfo.dwAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    tSummonInfo.eTeamOverride = TEAM_INVALID;

    ACDID idACDTarget = PowerAPI_SummonActor(tSummonInfo);
    if (idACDTarget == ACDID_INVALID) {
        return ACDID_INVALID;
    }

    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);

    tACDTarget.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDTarget.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    tACDTarget.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
    ActorFlagSet(idActorTarget, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    tACDTarget.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);

    if (!sPowerBeamUpdate(idPower, idACDTarget, flMaxLength, bUseRollToClickPositionOnGround, snoPowerPassability)) {
        BrainAPIDoDeath(idActorTarget, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        PowerInterrupt(idPower, TRUE);
        return ACDID_INVALID;
    }

    SNO snoEffectGroup = SNOFileLookupGUID(EFFECTGROUP_SNO, szBeamEffectGroupName);
    BOOL bExists = (snoEffectGroup != SNO_NONE);
    if (bExists) {
        SComplexEffectAdd(*ptACDOwner, snoEffectGroup, idACDTarget, COMPLEXEFFECT_TYPE_BEAM_TARGET, 0, 0, FALSE);
    }
    return idACDTarget;
}

// Original PowersSwig.cpp:7820.
BOOL HirelingTestTypeAvailable(ACDID idACDPlayer, HirelingClass eClass);  // X/Common/Hireling.h
HirelingClass HirelingGetClass(const ActorCommonData & tACDHireling);  // X/Common/Hireling.cpp:222
void NPCDismissProxy(ActorCommonData & tACDPlayer, HirelingClass eClass);  // X/Server/SNPC.cpp:793
ACDID SHirelingCreate(ActorCommonData & tACDPlayer, ActorCommonData & tACDNextTo, const HirelingClass eHirelingClass, BOOL bForceCreate, int32 eType);  // X/Server/SHireling.cpp:642

ACDID PowerActorSummonHireling(VACDID idPlayer, HirelingClass eHirelingClass)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD == NULL) {
        return ACDID_INVALID;
    }

    if (!HirelingTestTypeAvailable(idPlayer, eHirelingClass)) {
        return ACDID_INVALID;
    }

    ACDID idACDCurrentHireling = PlayerGetHireling(idPlayer);
    ActorCommonData * ptACDCurrentHireling = ACDTryToGet(idACDCurrentHireling);
    HirelingClass eClass = ptACDCurrentHireling ? HirelingGetClass(*ptACDCurrentHireling) : 0;
    if (eClass == eHirelingClass) {
        return ACDID_INVALID;
    }

    ActorCommonData & tACDPlayer = ACDGet(idPlayer);
    NPCDismissProxy(tACDPlayer, eHirelingClass);

    ACDID idACDSummoned = SHirelingCreate(tACDPlayer, tACDPlayer, eHirelingClass, TRUE, 2);
    if (idACDSummoned != ACDID_INVALID) {
        ACDGet(idACDSummoned).AttributesSetInt(ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, TRUE);
    }
    return idACDSummoned;
}

// Original PowersSwig.cpp:7865.
int32 PowerActorGetNearbySummonCount(ACDID idActor, float fRadii)
{
    ActorCommonData * ptACDSummoner = ACDTryToGet(idActor);
    if (ptACDSummoner == NULL) {
        return 0;
    }
    return ACDCountNearbySummonedUnits(*ptACDSummoner, fRadii);
}

// Original PowersSwig.cpp:7893.

ActorIDArray * PowerActorGetNearbySummons(ACDID idActor, float fRadii)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;  // inferred: the original's 8-byte zero store at 7898 covers 0x100 and 0x104
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) return &tArrACDs;
    XPooledDataIDList listSummons(MemoryPoolsGetDATAIDListPool());
    ACDGetNearbySummonedUnits(*ptACD, fRadii, listSummons);
    if (listSummons.Count() == 0) return &tArrACDs;
    POS pos = listSummons.GetHeadPosition();
    while (pos) {
        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs++] = (ACDID)listSummons.GetNext(pos);
        if (tArrACDs.nNumActorIDs == 64) {
            break;
        }
    }
    return &tArrACDs;
}

// Original PowersSwig.cpp:8076.
int32 PowerGetSummonMaxConcurrent()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return 0;
    }
    return PowerAPI_GetSummonMaxConcurrent(*ptACDOwner, ptPower->snoPower);
}

// Original PowersSwig.cpp:8091.
int32 PowerGetSummonMaxTotal()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return 0;
    }
    return PowerAPI_GetSummonMaxTotal(*ptACDOwner, ptPower->snoPower);
}

// Original PowersSwig.cpp:8238.
void PowerActorSetDamageAttributes(ACDID idACD, DamageType eDamageType, float flDamageMin, float flDamageDelta)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD != NULL) {
        ptACD->AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, eDamageType), flDamageMin);
        ptACD->AttributesSetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, eDamageType), flDamageDelta);
    }
}

// Original PowersSwig.cpp:8256.
static float sCompareLifeTime(ACDID idACD)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    Actor * ptActor = ActorGet(idActor);
    return (float)ptActor->dwTimeCreated;
}

// Original PowersSwig.cpp:8265.
void PowerPlayerAddPet(VACDID idACDPet, PetType ePetType, int32 nMaxCount, VACDID idACDPlayer, BOOL bIgnoreFade, BOOL bDontKillWhenEnteringRift)
{
    ActorCommonData * ptACDOwner;
    PowerID idPower = sGetCurrentPower();
    if (idACDPlayer != ACDID_INVALID) {
        ptACDOwner = ACDTryToGet(idACDPlayer);
    } else {
        ptACDOwner = PowerGetOwner(idPower);
    }
    if (ptACDOwner == NULL) {
        return;
    }

    if (!ACDIsPlayer(ptACDOwner->id)) {
        return;
    }

    ActorCommonData & tACDPet = ACDGet(idACDPet);
    if (tACDPet.GetSWorldID() != ptACDOwner->GetSWorldID()) {
        return;
    }

    if (bDontKillWhenEnteringRift) {
        tACDPet.AttributesSetInt(ATTRIBUTE_PET_DONT_KILL_WHEN_ENTERING_RIFTS, TRUE);
    }

    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    SPlayerAddPet(*ptPlayer, ePetType, idACDPet, TRUE, nMaxCount, sCompareLifeTime, bIgnoreFade);
}

// Original PowersSwig.cpp:8321.
void PowerPlayerRemovePet(VACDID idACDPet, VACDID idACDPlayer)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDPlayer;
    if (idACDPlayer == ACDID_INVALID) {
        ptACDPlayer = PowerGetOwner(idPower);
    } else {
        ptACDPlayer = ACDTryToGet(idACDPlayer);
    }
    if (ptACDPlayer != NULL && ACDIsPlayer(ptACDPlayer->id)) {
        PlayerRemovePet(*PlayerGetByACD(ptACDPlayer->id), idACDPet, FALSE);
    }
}

// Original PowersSwig.cpp:8351.
void PowerPlayerSetPetProcScalar(VACDID idACDPet, float flPetProcScalar)
{
    ACDGet(idACDPet).AttributesSetFloat(ATTRIBUTE_PET_PROC_SCALAR, flPetProcScalar);
}

// Original PowersSwig.cpp:8360.
void PowerPlayerSetPetCanBeDismissed(VACDID idACDPet, BOOL bCanBeDismissed)
{
    ACDGet(idACDPet).AttributesSetInt(ATTRIBUTE_PET_CANNOT_BE_DISMISSED, !bCanBeDismissed);
}

// Original PowersSwig.cpp:8541.
float PowerActorGetHitPointsMax(ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD != NULL) {
        return ptACD->GetHitPointsMaxTotal();
    }
    return 0.0f;
}

// Original PowersSwig.cpp:8555.
void PowerActorSetHitPointsMax(ACDID idACD, float flHitPointsMax)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD != NULL) {
        ptACD->SetHitPointsMax(flHitPointsMax);
    }
}

// Original PowersSwig.cpp:8568.
int32 PowerCreateBuff(BOOL bPositive, float flDuration)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    return PowerCreateBuff(ptPower->listBuffs.Count(), bPositive, flDuration);
}

// Original PowersSwig.cpp:8580.
int32 PowerCreateBuff(int32 nBuffIndex, BOOL bPositive, float flDuration)
{
    uint32 dwDuration = Math::FloatToInt(flDuration * TICKS_PER_SECOND);
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (flDuration < 0.0f) {
        dwDuration = 1;
    }

    while (ptPower->listBuffs.Count() <= nBuffIndex) {
        PowerAPIBuffStartData tBuffStartData;
        ptPower->listBuffs.AddTail_Tracked(tBuffStartData);
    }

    int nCurIndex = 0;
    POS buffPos = ptPower->listBuffs.GetHeadPosition();
    POS pos = buffPos;
    while (pos != NULL) {
        if (nCurIndex == nBuffIndex) {
            buffPos = pos;
        }
        nCurIndex++;
        ptPower->listBuffs.GetNext(pos);
    }

    PowerAPIBuffStartData & tNewBuffStartData = *(PowerAPIBuffStartData *)buffPos;
    tNewBuffStartData.Initialize();
    tNewBuffStartData.bPositiveEffect = bPositive;
    tNewBuffStartData.nBuffIndex = nBuffIndex;
    tNewBuffStartData.idPower = idPower;
    tNewBuffStartData.snoPower = PowerGetSNO(idPower);
    tNewBuffStartData.idACDSource = ptPower->idACD;
    tNewBuffStartData.nBuffLevel = PowerGetLevel(idPower);

    RuneType eRuneType = PowerGetRuneType(ptPower);
    Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(ptPower->snoPower, eRuneType, nBuffIndex);
    if (eAttrib != ATTRIBUTE_NONE) {
        FastAttribSetValueInt(tNewBuffStartData.idFastAttrib, MakeAttribKey(eAttrib, tNewBuffStartData.snoPower), TRUE);
    }

    if (dwDuration == 0) {
        tNewBuffStartData.bInfinite = TRUE;
    } else {
        tNewBuffStartData.dwDurationMin = dwDuration;
    }
    tNewBuffStartData.flProcScalar = PowerGetProcScalar(*ptPower);
    return nBuffIndex;
}

// Original PowersSwig.cpp:8666.
void PowerRemoveBuffs(ACDID idActor, LPCSTR szPowerName, int32 nBuffIndex, BuffRemovalReason eRemovalReason, ACDID idActorSource)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (snoPower == SNO_NONE) {
        return;
    }
    PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACD, snoPower, nBuffIndex, eRemovalReason, idActorSource);
}

// Original PowersSwig.cpp:8707.
// Condition order from the line table: flag test at 8716:9, the && at column 69.
void PowerBuffApplyToTarget(int32 nBuffID, VACDID idActor, BOOL bAllowInOnDoDamage)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if ((ptPower->dwFlags & (1 << PF_DISALLOW_PAYLOAD_APPLICATION)) && !bAllowInOnDoDamage) {
        return;
    }
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    PowerAPI_BuffAddToACD(*ptBuffStartData, ACDGet(idActor));
}

// Original PowersSwig.cpp:8797.
void PowerBuffRemove(int32 nBuffID, VACDID idActor, ACDID idActorSource, BOOL bFromThisPowerInstanceOnly, BOOL bRemoveSingleStack, BuffRemovalReason eRemovalReason)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData & tACD = ACDGet(idActor);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos != NULL) {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff == NULL || ptBuff->bDeleted) {
            continue;
        }
        if (ptBuff->nBuffIndex != nBuffID || ptBuff->snoPower != PowerGetSNO(idPower)) {
            continue;
        }
        if (idActorSource != ACDID_INVALID && ptBuff->idACDSource != idActorSource) {
            continue;
        }
        if (!bFromThisPowerInstanceOnly || ptBuff->idPower == idPower) {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, eRemovalReason, TRUE);
        }
        if (bRemoveSingleStack) {
            return;
        }
    }
}

// Original PowersSwig.cpp:8845.
void PowerBuffRemoveByBuffData(int32 nBuffID, int32 nBuffData, VACDID idActor, ACDID idActorSource, BOOL bFromThisPowerInstanceOnly, BOOL bRemoveSingleStack, BuffRemovalReason eRemovalReason)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData & tACD = ACDGet(idActor);
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos != NULL) {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff == NULL) {
            continue;
        }
        if (ptBuff->nBuffIndex != nBuffID ||
            ptBuff->snoPower != PowerGetSNO(idPower) ||
            ptBuff->nBuffData != nBuffData) {
            continue;
        }
        if (idActorSource != ACDID_INVALID && ptBuff->idACDSource != idActorSource) {
            continue;
        }
        if (!bFromThisPowerInstanceOnly || ptBuff->idPower == idPower) {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, eRemovalReason, TRUE);
        }
        if (bRemoveSingleStack) {
            return;
        }
    }
}

// Original PowersSwig.cpp:8896.
void PowerBuffRemoveDuringCallback()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listActiveCallbackBuffs.Count() == 0) {
        return;
    }
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(ptPower->listActiveCallbackBuffs.GetHead());
    if (ptBuff != NULL && !ptBuff->bDeleted) {
        PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}

// Original PowersSwig.cpp:8919.
// DWARF: ACDTryToGet at 8924 precedes snoPower (8929) and bExists (8930); idBuff is a block local on 8950.
BOOL PowerBuffIsOnActor(LPCSTR szPowerName, ACDID idActor, BOOL bIncludeDeleted)
{
    if (ACDTryToGet(idActor) == NULL) { return FALSE; }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    BOOL bExists = snoPower != SNO_NONE;  // Inferred: DWARF gives bExists the constant 1 from the 8948 test on.
    if (bExists) {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, -1, ACDGet(idActor), ACDID_INVALID, bIncludeDeleted);
        if (idBuff != BUFFID_INVALID) { return TRUE; }
    }
    return FALSE;
}
// Original PowersSwig.cpp:8962.
BOOL PowerBuffIsOnActor(ACDID idActor, BOOL bIncludeDeleted)
{
    if (ACDTryToGet(idActor) == NULL) {
        return FALSE;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return FALSE;
    }
    return PowerAPI_BuffFindOnACD(ptPower->snoPower, -1, ACDGet(idActor), ACDID_INVALID, bIncludeDeleted) != BUFFID_INVALID;
}

// Original PowersSwig.cpp:8983.
BOOL PowerBuffIsOnActor(int32 nBuffID, ACDID idActor, BOOL bIncludeDeleted)
{
    if (ACDTryToGet(idActor) == NULL) {
        return FALSE;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return FALSE;
    }
    return PowerAPI_BuffFindOnACD(ptPower->snoPower, nBuffID, ACDGet(idActor), ACDID_INVALID, bIncludeDeleted) != BUFFID_INVALID;
}

// Original PowersSwig.cpp:9005.
// Same shape as PowerBuffIsOnActor(LPCSTR, ACDID, BOOL): bExists (9018) is the constant 1 after the snoPower test.
BOOL PowerBuffIsOnActor(LPCSTR szPowerName, int32 nBuffID, ACDID idActor, BOOL bIncludeDeleted, int32 idPower)
{
    if (ACDTryToGet(idActor) == NULL) { return FALSE; }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    BOOL bExists = snoPower != SNO_NONE;
    if (bExists) {
        BuffID idBuff = PowerAPI_BuffFindOnACDByPowerID(snoPower, nBuffID, ACDTryToGet(idActor), idPower, ACDID_INVALID, bIncludeDeleted);
        if (idBuff != BUFFID_INVALID) { return TRUE; }
    }
    return FALSE;
}
// Original PowersSwig.cpp:9050.
// Same shape as PowerBuffIsOnActor(LPCSTR, ACDID, BOOL): bExists (9062) is the constant 1 after the snoPower test.
BOOL PowerBuffIsOnActorFromActor(LPCSTR szPowerName, int32 nBuffID, ACDID idActor, ACDID idActorSource)
{
    if (ACDTryToGet(idActor) == NULL) { return FALSE; }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    BOOL bExists = snoPower != SNO_NONE;
    if (bExists) {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffID, ACDGet(idActor), idActorSource, FALSE);
        if (idBuff != BUFFID_INVALID) { return TRUE; }
    }
    return FALSE;
}
// Original PowersSwig.cpp:9094.
BOOL PowerBuffIsOnActorByBuffData(int32 nBuffID, int32 nBuffData, ACDID idActor, BOOL bIncludeDeleted)
{
    if (ACDTryToGet(idActor) == NULL) {
        return FALSE;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return FALSE;
    }
    return PowerAPI_BuffFindOnACDByBuffData(ptPower->snoPower, nBuffID, nBuffData, ACDGet(idActor), ACDID_INVALID, bIncludeDeleted) != BUFFID_INVALID;
}

// Original PowersSwig.cpp:9117.

void PowerBuffSetPeriodicRefreshRate(int32 nBuffID, float flRefreshInterval, ACDID idTarget)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (idTarget == ACDID_INVALID || !sPowerValidateBuff(ptPower, nBuffID)) {
        return;
    }
    ActorCommonData * ptACDTarget = ACDTryToGet(idTarget);
    if (ptACDTarget == NULL) {
        return;
    }
    BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, nBuffID, *ptACDTarget, ptPower->idACD, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff != NULL) {
        ptBuff->dwRefreshRate = (uint32)(flRefreshInterval * TICKS_PER_SECOND);
    }
}

// Original PowersSwig.cpp:9145.
void PowerBuffAddAttributeModifier(int32 nBuffID, int32 eAttrib, int32 nParam, float flValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    FastAttribKey tKey = MakeAttribKey((Attrib)eAttrib, nParam);
    AttributeDataType eDataType = KeyGetDataType(tKey);
    if (KeyGetParamType(tKey) == -1) {
        if (eDataType == ATTRIBUTEDT_INT) {
            FastAttribSetValueInt(ptBuffStartData->idFastAttrib, FastAttribKey((Attrib)eAttrib), (int32)flValue);
        } else if (eDataType == ATTRIBUTEDT_FLOAT) {
            FastAttribSetValueFloat(ptBuffStartData->idFastAttrib, FastAttribKey((Attrib)eAttrib), flValue);
        }
    } else {
        if (eDataType == ATTRIBUTEDT_INT) {
            FastAttribSetValueInt(ptBuffStartData->idFastAttrib, tKey, (int32)flValue);
        } else if (eDataType == ATTRIBUTEDT_FLOAT) {
            FastAttribSetValueFloat(ptBuffStartData->idFastAttrib, tKey, flValue);
        }
    }
}

// Original PowersSwig.cpp:9193.
void PowerBuffAddCCImmunity(int32 nBuffID, bool bValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_STUN_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_ROOT_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_CHARM_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_BLIND_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_SLOWDOWN_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_FREEZE_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, bValue);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_ATTACKSPEED_REDUCTION_IMMUNE, bValue);
}

// Original PowersSwig.cpp:9219.
void PowerBuffAddLookSwitch(int32 nBuffID, LPCSTR szLookName)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_HAS_LOOK_OVERRIDE, TRUE);
        ptBuffStartData->nLookOverrideHash = StringHashI(szLookName);
    }
}

// Original PowersSwig.cpp:9238.
void PowerBuffAddWalkPassabilityOverride(int32 nBuffID, LPCSTR szPower)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        SNO snoPower = SNO_NONE;
        if (szPower != NULL) {
            snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
        }
        if (snoPower == SNO_NONE) {
            snoPower = ptPower->snoPower;
        }
        FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_WALK_PASSABILITY_POWER_SNO, snoPower);
    }
}

// Original PowersSwig.cpp:9274.
void PowerBuffAddPassabilityOverride(int32 nBuffID, LPCSTR szPower)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        SNO snoPower = SNO_NONE;
        if (szPower != NULL) {
            snoPower = SNOFileLookupGUID(POWER_SNO, szPower);
        }
        if (snoPower == SNO_NONE) {
            snoPower = ptPower->snoPower;
        }
        FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_PASSABILITY_POWER_SNO, snoPower);
    }
}

// Original PowersSwig.cpp:9310.
void PowerBuffSetSkillOverrides(int32 nBuffID, LPCSTR szHotbarSkill1, LPCSTR szHotbarSkill2, LPCSTR szHotbarSkill3, LPCSTR szHotbarSkill4, LPCSTR, LPCSTR szLMBSkill, LPCSTR szRMBSkill)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }

    SNO snoHotbarSkill1 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill1);
    SNO snoHotbarSkill2 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill2);
    SNO snoHotbarSkill3 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill3);
    SNO snoHotbarSkill4 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill4);
    SNO snoLMBSkill = SNOFileLookupGUID(POWER_SNO, szLMBSkill);
    SNO snoRMBSkill = SNOFileLookupGUID(POWER_SNO, szRMBSkill);

    LuaVerifySnosAreReferenced(snoHotbarSkill1, snoHotbarSkill2, snoHotbarSkill3, snoHotbarSkill4, snoLMBSkill, snoRMBSkill, SNO_NONE);

    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB1), snoHotbarSkill1);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB2), snoHotbarSkill2);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB3), snoHotbarSkill3);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB4), snoHotbarSkill4);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_LMB), snoLMBSkill);
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_RMB), snoRMBSkill);

    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_SKILL_OVERRIDE_ACTIVE, TRUE);
}

// Original PowersSwig.cpp:9436.
void PowerRetakeMeleeSnapshot()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL) {
        return;
    }
    if (ptPower->listInitialMeleeRangeACDs.Count() > 0) {
        ptPower->listInitialMeleeRangeACDs.RemoveAll();
    }
    PowerSnapshotMeleeRange(ptPower, ptPower->idACD, 1.0f, ptPower->tTarget);
}

// Original PowersSwig.cpp:9456.
void PowerBuffSetMaxInstances(int32 nBuffID, int32 nMaxInstances)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->nMaxInstanceCount = nMaxInstances;
    }
}

// Original PowersSwig.cpp:9472.
void PowerBuffSetStackType(int32 nBuffID, BuffStackType eStackType, int32 nMaxStackCount)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (eStackType >= BUFF_STACKTYPE_NEVER && eStackType < BUFF_STACKTYPE_COUNT && ptBuffStartData != NULL) {
        ptBuffStartData->eStackType = eStackType;
        ptBuffStartData->nMaxStackCount = nMaxStackCount;
    }
}

// Original PowersSwig.cpp:9496.
void PowerBuffSetTriggersHitProcs(int nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bTriggersHitProcs = bEnable;
        if (!bEnable) {
            ptBuffStartData->flProcScalar = 0.0f;
            ptBuffStartData->bUsePowersProcScalar = FALSE;
        }
    }
}

// Original PowersSwig.cpp:9518.
void PowerBuffSetUndispellable(int nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bUndispellable = bEnable;
    }
}

// Original PowersSwig.cpp:9533.
void PowerBuffSetSaveCooldown(int nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bSaveCooldown = bEnable;
    }
}

// Original PowersSwig.cpp:9548.
void PowerBuffSetDOTDPS(int32 nBuffID, DamageType eDamageType, float flDPS, BOOL bWantBonuses, BOOL bCanCrit, BOOL bHideDOTDamageEffects, BOOL bUseHighestElementalDamageBonus)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }

    if (bWantBonuses) {
        PowerPayloadGetBonusMultipliers(ptPower->idFastAttrib,
                                        ptPower->snoPower,
                                        ptPower->id,
                                        PlayerGetPlayerClass(PowerGetOwner(idPower)),
                                        ptBuffStartData->flPrimaryAttributeBonusMultiplier,
                                        ptBuffStartData->flClassBonusMultiplier,
                                        ptBuffStartData->flSkillsBonusMultiplier);
        ptBuffStartData->bDOTCanCrit = bCanCrit;
    } else {
        ptBuffStartData->bDOTCanCrit = FALSE;
    }

    ptBuffStartData->eDamageType = eDamageType;
    ptBuffStartData->flDamagePerSecond = flDPS;
    ptBuffStartData->bTriggersHitProcs = ptBuffStartData->flProcScalar > 0.0f;
    ptBuffStartData->bHideDOTDamageEffects = bHideDOTDamageEffects;
    ptBuffStartData->bUseHighestElementalDamageBonus = bUseHighestElementalDamageBonus;
}

// Original PowersSwig.cpp:9588.
void PowerBuffSetPetDamageScalar(int32 nBuffID, float flScalar)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->flPetDamageScalar = Math::Max(0.0f, flScalar);
    }
}

// Original PowersSwig.cpp:9606.
void PowerBuffSetPetAOEDamageScalar(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ScopedGlobalDefinitionLockR pdefGlobal;
        ptBuffStartData->flPetDamageScalar = Math::Max(0.0f, pdefGlobal->ptServerData->flPetAoEDamageScalar);
    }
}

// Original PowersSwig.cpp:9625.
void PowerBuffSetProcScalar(int32 nBuffID, float flProcScalar)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        if (ptPower->bDoesntAnimate && !ptPower->bAllowHitProcs) {
            ptBuffStartData->flProcScalar = 0.0f;
        } else {
            ptBuffStartData->flProcScalar = flProcScalar;
        }
        ptBuffStartData->bUsePowersProcScalar = FALSE;
    }
}

// Original PowersSwig.cpp:9649.
void PowerBuffAddStun(int32 nBuffID, BOOL bBuffFailsIfStunImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bStun = TRUE;
        ptBuffStartData->bFailsIfStunImmune = bBuffFailsIfStunImmune;
    }
}

// Original PowersSwig.cpp:9666.
void PowerBuffAddFreeze(int32 nBuffID, BOOL bBuffFailsIfFreezeImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bFreeze = TRUE;
        ptBuffStartData->bFailsIfFreezeImmune = bBuffFailsIfFreezeImmune;
    }
}

// Original PowersSwig.cpp:9683.
void PowerBuffAddForceGrip(int32 nBuffID, BOOL bBuffFailsIfForceGripImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bForceGrip = TRUE;
        ptBuffStartData->bFailsIfForceGripImmune = bBuffFailsIfForceGripImmune;
    }
}

// Original PowersSwig.cpp:9700.
void PowerBuffAddBlind(int32 nBuffID, BOOL bBuffFailsIfBlindImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bBlind = TRUE;
        ptBuffStartData->bFailsIfBlindImmune = bBuffFailsIfBlindImmune;
    }
}

// Original PowersSwig.cpp:9717.
void PowerBuffAddFear(int32 nBuffID, BOOL bBuffFailsIfFearImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bFear = TRUE;
        ptBuffStartData->bFailsIfFearImmune = bBuffFailsIfFearImmune;
    }
}

// Original PowersSwig.cpp:9734.
void PowerBuffAddCharm(int32 nBuffID, BOOL bBuffFailsIfCharmImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_CHARM_SOURCE_ACDID, ptACDOwner != NULL ? ptACDOwner->id : ACDID_INVALID);
        ptBuffStartData->bCharm = TRUE;
        ptBuffStartData->bFailsIfCharmImmune = bBuffFailsIfCharmImmune;
    }
}

// Original PowersSwig.cpp:9754.
void PowerBuffAddTaunt(int32 nBuffID, ACDID idTauntTarget, BOOL bBuffFailsIfCharmImmune, BOOL bSuppressTauntEffects)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_TAUNT_TARGET_ACDID, idTauntTarget);
    ptBuffStartData->bTaunt = true;
    ptBuffStartData->bSuppressTauntEffects = bSuppressTauntEffects != FALSE;
    ptBuffStartData->bFailsIfCharmImmune = bBuffFailsIfCharmImmune != FALSE;
}

// Original PowersSwig.cpp:9776.
void PowerBuffAddForcedTarget(int32 nBuffID, ACDID idTarget)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        FastAttribSetValueInt(ptBuffStartData->idFastAttrib, ATTRIBUTE_FORCED_ENEMY_ACDID, idTarget);
    }
}

// Original PowersSwig.cpp:9792.
void PowerBuffAddSlow(int32 nBuffID, float flSlowAmount, BOOL bBuffFailsIfSlowImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bSlow = TRUE;
        ptBuffStartData->flSlowAmount = flSlowAmount;
        ptBuffStartData->bFailsIfSlowImmune = bBuffFailsIfSlowImmune;
    }
}

// Original PowersSwig.cpp:9811.
void PowerBuffAddAttackSpeedReduction(int32 nBuffID, float flAttackSpeedReductionAmount, BOOL bBuffFailsIfAttackSpeedReductionImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bAttackSpeedReduction = TRUE;
        ptBuffStartData->flAttackSpeedReductionAmount = flAttackSpeedReductionAmount;
        ptBuffStartData->bFailsIfAttackSpeedReductionImmune = bBuffFailsIfAttackSpeedReductionImmune;
    }
}

// Original PowersSwig.cpp:9830.
void PowerBuffAddChill(int32 nBuffID, float flChillMagnitude, BOOL bBuffFailsIfChillImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bChill = TRUE;
        ptBuffStartData->flChillMagnitude = flChillMagnitude;
        ptBuffStartData->bFailsIfChillImmune = bBuffFailsIfChillImmune;
    }
}

// Original PowersSwig.cpp:9849.
void PowerBuffAddImmobilize(int32 nBuffID, BOOL bBuffFailsIfImmobilizeImmune)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bImmobilize = TRUE;
        ptBuffStartData->bFailsIfImmobilizeImmune = bBuffFailsIfImmobilizeImmune;
    }
}

// Original PowersSwig.cpp:9866.
void PowerBuffSetChanneled(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bChanneled = TRUE;
    }
}

// Original PowersSwig.cpp:9881.
void PowerBuffSetUsesLockout(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bUsesLockout = TRUE;
    }
}

// Original PowersSwig.cpp:9895.
void sPowerBuffSetDuration(SNO snoPower, int32 nBuffID, ACDID idACD, float flDuration, PowerID idPower, BOOL bCasterAsSource, BOOL bOnlyFirstBuff, BOOL bResetStartTick)
{
    ACDID idACDSource = ACDID_INVALID;
    if (bCasterAsSource) {
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower != NULL) {
            ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
            if (ptACDOwner != NULL) {
                idACDSource = ptACDOwner->id;
            }
        }
    }
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return;
    }
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos != NULL) {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff == NULL || ptBuff->bDeleted || ptBuff->snoPower != snoPower || ptBuff->nBuffIndex != nBuffID) {
            continue;
        }
        uint32 dwNow = GameGetServerTime();
        uint32 nTicksLeft = ptBuff->dwDuration - dwNow + ptBuff->dwStartTick;
        uint32 nNewRemainingDuration = (uint32)(flDuration * TICKS_PER_SECOND) - (ptBuff->bInfinite ? 0 : nTicksLeft);
        BOOL bWantInfinite = flDuration < 1e-6f;
        if (idACDSource == ACDID_INVALID) {
            PowerAPI_BuffModifyDuration(idBuff, nNewRemainingDuration, bWantInfinite, bResetStartTick);
            if (bOnlyFirstBuff) {
                return;
            }
        } else if (ptBuff->idACDSource == idACDSource) {
            PowerAPI_BuffModifyDuration(idBuff, nNewRemainingDuration, bWantInfinite, bResetStartTick);
            if (bOnlyFirstBuff) {
                return;
            }
        }
    }
}

// Original PowersSwig.cpp:9962.
void PowerBuffSetDuration(LPCSTR szPowerName, int32 nBuffID, ACDID idACD, float flDuration, BOOL bCasterAsSource, BOOL bOnlyFirstBuff, int32 idPower, BOOL bResetStartTick)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (snoPower == SNO_NONE) {
        return;
    }
    sPowerBuffSetDuration(snoPower, nBuffID, idACD, flDuration, idPower, bCasterAsSource, bOnlyFirstBuff, bResetStartTick);
}

// Original PowersSwig.cpp:9999.
void PowerBuffSetDuration(int32 nBuffID, VACDID idACD, float flDuration, BOOL bCasterAsSource, BOOL bOnlyFirstBuff, BOOL bResetStartTick)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return;
    }
    sPowerBuffSetDuration(ptPower->snoPower, nBuffID, idACD, flDuration, idPower, bCasterAsSource, bOnlyFirstBuff, bResetStartTick);
}

// Original PowersSwig.cpp:10018.
void PowerBuffSetDuration(int nBuffID, float flDuration)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    uint32 dwDuration;
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        dwDuration = Math::FloatToInt(flDuration * TICKS_PER_SECOND);
        if (flDuration < 0.0f) {
            dwDuration = 1;
        }
        if (dwDuration == 0) {
            ptBuffStartData->bInfinite = TRUE;
        } else {
            ptBuffStartData->dwDurationMin = dwDuration;
        }
    }
}

// Original PowersSwig.cpp:10051.
void PowerAPI_BuffRefreshDuration(BuffID idBuff);  // PowerAPI.cpp; local declaration

void PowerBuffRefreshDuration(int32 nBuffID, VACDID idACD, BOOL bCasterAsSource, BOOL bOnlyFirstBuff)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL) {
        return;
    }

    ACDID idACDSource = ACDID_INVALID;
    if (bCasterAsSource) {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        if (ptACDOwner != NULL) {
            idACDSource = ptACDOwner->id;
        }
    }

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return;
    }

    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos != NULL) {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff != NULL &&
            !ptBuff->bDeleted &&
            ptBuff->snoPower == ptPower->snoPower &&
            ptBuff->nBuffIndex == nBuffID) {
            if (idACDSource == ACDID_INVALID) {
                PowerAPI_BuffRefreshDuration(idBuff);
                if (bOnlyFirstBuff) {
                    return;
                }
            } else if (ptBuff->idACDSource == idACDSource) {
                PowerAPI_BuffRefreshDuration(idBuff);
                if (bOnlyFirstBuff) {
                    return;
                }
            }
        }
    }
}

// Original PowersSwig.cpp:10113.
float PowerBuffGetDuration(int32 nBuffID, VACDID idACD, BOOL bCasterAsSource)
{
    PowerID idPower = sGetCurrentPower();
    ACDID idACDSource = ACDID_INVALID;
    if (bCasterAsSource) {
        ActorCommonData * ptACDOwner = PowerGetOwner(PowerGet(idPower));
        if (ptACDOwner != NULL) {
            idACDSource = ptACDOwner->id;
        }
    }
    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, idACDSource, TRUE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0.0f;
    }
    uint32 dwNow = GameGetServerTime();
    return (float)(ptBuff->dwDuration - dwNow + ptBuff->dwStartTick) / TICKS_PER_SECOND;
}

// Original PowersSwig.cpp:10148.
float PowerBuffGetDurationByPowerName(LPCSTR szPowerName, int32 nBuffID, VACDID idACDActor, VACDID idACDSource)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (idACDActor == ACDID_INVALID || snoPower == SNO_NONE) {
        return 0.0f;
    }
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD == NULL) {
        return 0.0f;
    }
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffID, *ptACD, idACDSource, TRUE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0.0f;
    }
    uint32 dwNow = GameGetServerTime();
    return (float)(ptBuff->dwDuration - dwNow + ptBuff->dwStartTick) / TICKS_PER_SECOND;
}

// Original PowersSwig.cpp:10202.
BOOL PowerBuffCheckForAuraApplication(int32 nBuffID, VACDID idActor, BOOL bCasterAsSource)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return FALSE;
    }
    ACDID idACDSource = ACDID_INVALID;
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (bCasterAsSource && ptACDOwner != NULL) {
        idACDSource = ptACDOwner->id;
    }
    BuffID idBuff = sBuffFindForACD(idPower, idActor, nBuffID, idACDSource, TRUE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return TRUE;
    }
    PowerID idBuffPower = ptBuff->idPower;
    Power * ptBuffPower = PowerTryToGet(idBuffPower);
    if (ptBuffPower == NULL) {
        return TRUE;
    }
    return ptPower->dwStartTick >= ptBuffPower->dwStartTick;
}

// Original PowersSwig.cpp:10245.
void PowerBuffSetStoredData(int32 nBuffID, int32 nValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    Attrib eAttribStart = ATTRIBUTE_NONE;
    Attrib eAttribEnd = ATTRIBUTE_NONE;
    Attrib eAttribCount = ATTRIBUTE_NONE;
    BOOL bValid = PowerGetBuffAttributes(nBuffID, eAttribStart, eAttribEnd, eAttribCount);
    (void)bValid;

    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->nBuffData = nValue;
    }
}

// Original PowersSwig.cpp:10267.
void PowerBuffSetStoredDataFloat(int32 nBuffID, float flValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    Attrib eAttribStart = ATTRIBUTE_NONE;
    Attrib eAttribEnd = ATTRIBUTE_NONE;
    Attrib eAttribCount = ATTRIBUTE_NONE;
    BOOL bValid = PowerGetBuffAttributes(nBuffID, eAttribStart, eAttribEnd, eAttribCount);
    (void)bValid;

    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->flBuffData = flValue;
    }
}

// Original PowersSwig.cpp:10289.
void PowerBuffSetStoredData(ACDID idACD, int32 nBuffID, int32 nValue, BOOL bFromPowerOwnerOnly)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ACDTryToGet(idACD) == NULL) {
        return;
    }
    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, bFromPowerOwnerOnly ? ptPower->idACD : ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff != NULL) {
        ptBuff->nBuffData = nValue;
    }
}

// Original PowersSwig.cpp:10315.
void PowerBuffSetStoredDataFloat(ACDID idACD, int32 nBuffID, float flValue, BOOL bFromPowerOwnerOnly)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ACDTryToGet(idACD) == NULL) {
        return;
    }
    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, bFromPowerOwnerOnly ? ptPower->idACD : ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff != NULL) {
        ptBuff->flBuffData = flValue;
    }
}

// Original PowersSwig.cpp:10341.
int32 PowerBuffGetStoredData(ACDID idACD, int32 nBuffID, BOOL bFromPowerOwnerOnly, BOOL bAllowDeleted)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ACDTryToGet(idACD) == NULL) {
        return 0;
    }
    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, bFromPowerOwnerOnly ? ptPower->idACD : ACDID_INVALID, bAllowDeleted);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0;
    }
    return ptBuff->nBuffData;
}

// Original PowersSwig.cpp:10368.
float PowerBuffGetStoredDataFloat(ACDID idACD, int32 nBuffID, BOOL bFromPowerOwnerOnly, BOOL bAllowDeleted)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ACDTryToGet(idACD) == NULL) {
        return 0.0f;
    }
    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, bFromPowerOwnerOnly ? ptPower->idACD : ACDID_INVALID, bAllowDeleted);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0.0f;
    }
    return ptBuff->flBuffData;
}

// Original PowersSwig.cpp:10395.
int32 PowerBuffGetStoredData(LPCSTR szPowerName, ACDID idACD, int32 nBuffID, BOOL bAllowDeleted)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (idACD == ACDID_INVALID || snoPower == SNO_NONE) {
        return 0;
    }
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return 0;
    }
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffID, *ptACD, ACDID_INVALID, bAllowDeleted);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0;
    }
    return ptBuff->nBuffData;
}

// Original PowersSwig.cpp:10438.
float PowerBuffGetStoredDataFloat(LPCSTR szPowerName, ACDID idACD, int32 nBuffID, BOOL bAllowDeleted)
{
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (idACD == ACDID_INVALID || snoPower == SNO_NONE) {
        return 0.0f;
    }
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return 0.0f;
    }
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffID, *ptACD, ACDID_INVALID, bAllowDeleted);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0.0f;
    }
    return ptBuff->flBuffData;
}

// Original PowersSwig.cpp:10481.
int32 PowerBuffCallbackGetStoredData()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listActiveCallbackBuffs.IsEmpty()) {
        return 0;
    }
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(ptPower->listActiveCallbackBuffs.GetHead());
    if (ptBuff == NULL) {
        return 0;
    }
    return ptBuff->nBuffData;
}

// Original PowersSwig.cpp:10504.
float PowerBuffCallbackGetStoredDataFloat()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listActiveCallbackBuffs.IsEmpty()) {
        return 0.0f;
    }
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(ptPower->listActiveCallbackBuffs.GetHead());
    if (ptBuff == NULL) {
        return 0.0f;
    }
    return ptBuff->flBuffData;
}

// Original PowersSwig.cpp:10527.
void PowerBuffSetUINumber(int32 nBuffID, int32 nValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    Attrib eAttribStart = ATTRIBUTE_NONE;
    Attrib eAttribEnd = ATTRIBUTE_NONE;
    Attrib eAttribCount = ATTRIBUTE_NONE;
    PowerGetBuffAttributes(nBuffID, eAttribStart, eAttribEnd, eAttribCount);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(eAttribCount, ptPower->snoPower), nValue);
}

// Original PowersSwig.cpp:10550.
void PowerBuffSetUINumber(ACDID idACD, int32 nBuffID, int32 nValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (idACD == ACDID_INVALID || !sPowerValidateBuff(ptPower, nBuffID)) {
        return;
    }
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return;
    }
    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return;
    }
    Attrib eAttribStart = ATTRIBUTE_NONE;
    Attrib eAttribEnd = ATTRIBUTE_NONE;
    Attrib eAttribCount = ATTRIBUTE_NONE;
    PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);
    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribCount, ptPower->snoPower), nValue);
    ptACD->ModifiedMark(FALSE);
}

// Original PowersSwig.cpp:10581.
void PowerBuffSetUINumber(ACDID idACD, LPCSTR szPowerName, int32 nBuffID, int32 nValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return;
    }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (snoPower == SNO_NONE) {
        return;
    }
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffID, *ptACD, ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (nValue < 0 || ptBuff == NULL) {
        return;
    }
    Attrib eAttribStart = ATTRIBUTE_NONE;
    Attrib eAttribEnd = ATTRIBUTE_NONE;
    Attrib eAttribCount = ATTRIBUTE_NONE;
    PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);
    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribCount, snoPower), nValue);
    ptACD->ModifiedMark(FALSE);
}

// Original PowersSwig.cpp:438 (abstract DIE: params snoPower, ptBuff; locals eAttribStart/End/Count, bValid);
// inlined into both PowerBuffGetUINumber overloads. bValid has no test in the original's code (445 -> 448).
static int32 sPowerGetBuffAttributeValue(
    const SNO & snoPower,
    PowerAPIBuffInstance * ptBuff)
{
    Attrib eAttribStart = ATTRIBUTE_NONE;
    Attrib eAttribEnd = ATTRIBUTE_NONE;
    Attrib eAttribCount = ATTRIBUTE_NONE;
    BOOL bValid = PowerGetBuffAttributes(ptBuff->nBuffIndex, eAttribStart, eAttribEnd, eAttribCount);
    if (!FastAttribGroupIsValid(ptBuff->idFastAttrib)) {
        return 0;
    }
    return FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(eAttribCount, snoPower));
}

// Original PowersSwig.cpp:10629.
// No explicit idACD test: ACDTryToGet (10641) carries the INVALID test that the original hoists (10636 col 9 is the !).
#line 10629
int32 PowerBuffGetUINumber(
    ACDID idACD,
    int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power *ptPower = PowerGet(idPower);

    if (!sPowerValidateBuff(ptPower, nBuffID))
    {
        return 0;
    }

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
    {
        return 0;
    }

    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, ACDID_INVALID, TRUE);

    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL)
    {
        return 0;
    }

    return sPowerGetBuffAttributeValue(ptPower->snoPower, ptBuff);
}
#line 4908



// Original PowersSwig.cpp:10659.
int32 PowerBuffGetUINumber(ACDID idACD, LPCSTR szPowerName, int32 nBuffID)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return 0;
    }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (snoPower == SNO_NONE) {
        return 0;
    }
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, nBuffID, *ptACD, ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0;
    }
    return sPowerGetBuffAttributeValue(snoPower, ptBuff);
}

// Original PowersSwig.cpp:10700.
int32 PowerBuffGetNumStacks(int32 nBuffID, VACDID idACD)
{
    PowerID idPower = sGetCurrentPower();
    BuffID idBuff = sBuffFindForACD(idPower, idACD, nBuffID, ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return 0;
    }
    return PowerAPI_BuffCountOnACD(ptBuff->snoPower, ptBuff->nBuffIndex, ACDGet(idACD), ACDID_INVALID);
}

// Original PowersSwig.cpp:10719.
void PowerBuffEnablePeriodicEffect(int32 nBuffID, float flRefreshInterval, BOOL bHitsOnFirstTick)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pPeriodicSpellEffect = ScriptedPower_BuffPeriodicSpellEffectCallback;
        ptBuffStartData->pPeriodicSpellEffectData = (void *)(int64)nBuffID;
        ptBuffStartData->dwRefreshRate = Math::FloatToInt(flRefreshInterval * TICKS_PER_SECOND);
        ptBuffStartData->bHitsOnFirstTick = bHitsOnFirstTick;
    }
}

// Original PowersSwig.cpp:10739.
void PowerBuffEnableOnDoDamageEffect(int32 nBuffID, BOOL bTriggerForPets)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnDoDamageEffect = ScriptedPower_BuffOnDoDamageEffectCallback;
        ptBuffStartData->bTriggerOnDamageForPets = bTriggerForPets;
    }
}

// Original PowersSwig.cpp:10756.
void PowerBuffEnableOnHitEffect(int32 nBuffID, BOOL bTriggerForPets)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnHitEffect = ScriptedPower_BuffOnHitEffectCallback;
        ptBuffStartData->bTriggerOnHitForPets = bTriggerForPets;
    }
}

// Original PowersSwig.cpp:10773.
void PowerBuffEnableOnGetHitEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnGetHitEffect = ScriptedPower_BuffOnGetHitEffectCallback;
    }
}

// Original PowersSwig.cpp:10788.
void PowerBuffEnableOnAttackEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnAttackEffect = ScriptedPower_BuffOnAttackEffectCallback;
    }
}

// Original PowersSwig.cpp:10803.
void PowerBuffEnableOnDodgeEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnDodgeEffect = ScriptedPower_BuffOnDodgeEffectCallback;
    }
}

// Original PowersSwig.cpp:10818.
void PowerBuffEnableOnCostEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnCostEffect = ScriptedPower_BuffOnCostEffectCallback;
    }
}

// Original PowersSwig.cpp:10833.
void PowerBuffEnableOnHealFromSkillEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnHealFromSkillEffect = ScriptedPower_BuffOnHealFromSkillEffectCallback;
    }
}

// Original PowersSwig.cpp:10848.
void PowerBuffEnableOnKillEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnKillEffect = ScriptedPower_BuffOnKillEffectCallback;
    }
}

// Original PowersSwig.cpp:10864.
void PowerBuffEnableOnAssistEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnAssistEffect = ScriptedPower_BuffOnAssistEffectCallback;
    }
}

// Original PowersSwig.cpp:10879.
void PowerBuffEnableOnItemPickupEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnItemPickUpEffect = ScriptedPower_BuffOnItemPickUpEffectCallback;
    }
}

// Original PowersSwig.cpp:10894.
void PowerBuffEnableDamageShieldEffect(int32 nBuffID, int32 nPriority, BOOL bStaggerDamageEnabled)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pDamageShieldEffect = ScriptedPower_BuffDamageShieldEffectCallback;
        ptBuffStartData->nDamageShieldPriority = nPriority;
        ptBuffStartData->bProcessDamageShieldOnStaggerDamage = bStaggerDamageEnabled;
    }
}

// Original PowersSwig.cpp:10913.
void PowerBuffEnableOnExpirationEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pExpirationEffect = ScriptedPower_BuffOnExpirationEffectCallback;
    }
}

// Original PowersSwig.cpp:10928.
void PowerBuffEnableOnRemovalEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pRemovalEffect = ScriptedPower_BuffOnRemovalEffectCallback;
    }
}

// Original PowersSwig.cpp:10943.
void PowerBuffEnableOnDeathEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pDeathEffect = ScriptedPower_BuffOnDeathEffectCallback;
    }
}

// Original PowersSwig.cpp:10958.
void PowerBuffEnableOnCalloutEventEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnCalloutEventEffect = ScriptedPower_BuffOnCalloutEventCallback;
    }
}

// Original PowersSwig.cpp:10973.
void PowerBuffEnableOnGizmoOperatedEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnGizmoOperatedEffect = ScriptedPower_BuffOnGizmoOperatedCallback;
    }
}

// Original PowersSwig.cpp:10988.
void PowerBuffEnableOnWreckableDestroyedEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnWreckableDestroyedEffect = ScriptedPower_BuffOnWreckableDestroyedCallback;
    }
}

// Original PowersSwig.cpp:11003.
void PowerBuffEnableOnTeleportedEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnTeleportedEffect = ScriptedPower_BuffOnTeleportedCallback;
    }
}

// Original PowersSwig.cpp:11018.
void PowerBuffEnableOnPowerDamageInitEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnPowerDamageInitEffect = ScriptedPower_BuffOnPowerDamageInitCallback;
    }
}

// Original PowersSwig.cpp:11033.
void PowerBuffEnableOnLootDropEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnLootDropEffect = ScriptedPower_BuffOnLootDropCallback;
    }
}

// Original PowersSwig.cpp:11050.
void PowerBuffEnableOnItemSoldEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnItemSoldEffect = ScriptedPower_BuffOnItemSoldCallback;
    }
}

// Original PowersSwig.cpp:11066.
void PowerBuffEnableOnLevelEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnLevelEffect = ScriptedPower_BuffOnLevelCallback;
    }
}

// Original PowersSwig.cpp:11082.
void PowerBuffEnableOnCorpseConsumedEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnCorpseConsumedEffect = ScriptedPower_BuffOnCorpseConsumedCallback;
    }
}

// Original PowersSwig.cpp:11098.
void PowerBuffEnableOnPetDismissEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnPetDismissEffect = ScriptedPower_BuffOnPetDismissCallback;
    }
}

// Original PowersSwig.cpp:11115.
static BOOL sScriptedShouldReflect(PowerAPIBuffInstance * ptBuff, ImpactData * ptImpact, void * pData)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(ptImpact->idACDProjectile);
    ActorCommonData * ptACDReflector = ACDTryToGet(ptBuff->idACDAffected);
    if (ptACDProjectile == NULL || ptACDReflector == NULL) {
        return FALSE;
    }
    ScopedActorDefinitionLockR pdefActor(ptACDProjectile->snoActor);
    BOOL bCanBeReflected = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_CAN_BE_REFLECTED);
    if (!bCanBeReflected) {
        return FALSE;
    }
    float flReflectChance = (float)(int64)ptBuff->pReflectEffectData / 100.0f;
    if (flReflectChance < EPSILON) {
        return FALSE;
    }
    float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
    if (flRoll >= flReflectChance) {
        return FALSE;
    }
    return TRUE;
}

// Original PowersSwig.cpp:11152.
void PowerBuffEnableOnReflectEffect(int32 nBuffID, float flReflectChance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pShouldReflectTest = sScriptedShouldReflect;
        ptBuffStartData->pReflectEffect = ScriptedPower_BuffOnReflectEffectCallback;
        ptBuffStartData->pReflectEffectData = (void *)(int64)(flReflectChance * 100.0f);
    }
}

// Original PowersSwig.cpp:11171.
void PowerBuffEnableOnProjectileCreateEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnProjectileCreateEffect = ScriptedPower_BuffOnProjectileCreateEffectCallback;
    }
}

// Original PowersSwig.cpp:11186.
void PowerBuffEnableOnKnockbackLandEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnKnockbackLandEffect = ScriptedPower_BuffOnKnockbackLandEffectCallback;
    }
}

// Original PowersSwig.cpp:11201.
void PowerBuffEnableOnPetAddEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnPetAddEffect = ScriptedPower_BuffOnPetAddEffectCallback;
    }
}

// Original PowersSwig.cpp:11216.
void PowerBuffEnableOnGetHitWithCrowdControlEffect(int32 nBuffID, BOOL bCallOnRefresh)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnGetHitWithCrowdControlEffect = ScriptedPower_BuffOnGetHitWithCrowdControlEffectCallback;
        ptBuffStartData->bOnGetHitWithCrowdControlCallOnRefresh = bCallOnRefresh;
    }
}

// Original PowersSwig.cpp:11232.
void PowerBuffEnableOnHitWithCrowdControlEffect(int32 nBuffID, BOOL bCallOnRefresh)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnHitWithCrowdControlEffect = ScriptedPower_BuffOnHitWithCrowdControlEffectCallback;
        ptBuffStartData->bOnHitWithCrowdControlCallOnRefresh = bCallOnRefresh;
    }
}

// Original PowersSwig.cpp:11248.
void PowerBuffEnableOnEquipmentChangeEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnEquipmentChangeEffect = ScriptedPower_BuffOnEquipmentChangeEffectCallback;
    }
}

// Original PowersSwig.cpp:11263.
void PowerBuffEnableOnResourceGainEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnResourceGainEffect = ScriptedPower_BuffOnResourceGainEffectCallback;
    }
}

// Original PowersSwig.cpp:11278.
void PowerBuffEnableOnFirstHitAttemptEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnFirstHitAttemptEffect = ScriptedPower_BuffOnFirstHitAttemptEffectCallback;
    }
}

// Original PowersSwig.cpp:11293.
void PowerBuffEnableOnMessageEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnMessageEffect = ScriptedPower_BuffOnMessageEffectCallback;
    }
}

// Original PowersSwig.cpp:11308.
void PowerBuffEnablePersistAfterDeath(int32 nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bRemoveOnDeath = !bEnable;
    }
}

// Original PowersSwig.cpp:11324.
void PowerBuffSetRemoveOnRespec(int32 nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bRemoveOnRespec = bEnable;
    }
}

// Original PowersSwig.cpp:11341.
void PowerBuffEnableOnSkillChangeEffect(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->pOnSkillChangeEffect = ScriptedPower_BuffOnSkillChangeEffectCallback;
    }
}

// Original PowersSwig.cpp:11356.
void PowerBuffSetRemoveOnPassiveApplication(int32 nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bRemoveOnPassiveApplication = bEnable;
    }
}

// Original PowersSwig.cpp:11372.
void PowerBuffSetRemoveOnEnteringGreaterRift(int32 nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bRemoveOnGreaterRiftEntry = bEnable;
    }
}

// Original PowersSwig.cpp:11388.
void PowerBuffSetKeepForProfileSnapshot(int32 nBuffID, BOOL bEnable)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->bKeepForProfileSnapshot = bEnable;
    }
}

// Original PowersSwig.cpp:11407.
void PowerBuffAddAIStateMerge(int32 nBuffID, LPCSTR szAIStateSNOToMerge)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    SNO snoAIStateToMerge = SNOFileLookupGUID(AISTATE_SNO, szAIStateSNOToMerge);
    if (snoAIStateToMerge == SNO_NONE) {
        return;
    }
    LuaVerifySnosAreReferenced(snoAIStateToMerge, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    if (ptBuffStartData->listAIStatesToMerge.Find(snoAIStateToMerge) == NULL) {
        ptBuffStartData->listAIStatesToMerge.AddTail_Tracked(snoAIStateToMerge);
    }
}

// Original PowersSwig.cpp:11435.
void PowerSwapActorPlaces(ACDID idACDFirst, ACDID idACDSecond)
{
    ActorCommonData * ptACDFirst = ACDTryToGet(idACDFirst);
    ActorCommonData * ptACDSecond = ACDTryToGet(idACDSecond);

    if (ptACDFirst == NULL || ptACDSecond == NULL) {
        return;
    }

    WorldPlace placeFirst = ptACDFirst->GetPlace();
    WorldPlace placeSecond = ptACDSecond->GetPlace();

    Quaternion qRotFirst = ptACDFirst->GetOrientation();
    Quaternion qRotSecond = ptACDSecond->GetOrientation();

    ActorMovement * pACM = ptACDFirst->GetActorMovement();
    pACM->SnapToNewPlace(placeSecond, TRUE, FALSE, TRUE, 0);
    pACM->SetNewFacing(qRotSecond.ToYaw(), FALSE, TRUE);

    pACM = ptACDSecond->GetActorMovement();
    pACM->SnapToNewPlace(placeFirst, TRUE, FALSE, TRUE, 0);
    pACM->SetNewFacing(qRotFirst.ToYaw(), FALSE, TRUE);
}

// Original PowersSwig.cpp:11464.
float PowerBuffDamageShieldGetIncomingDamageTotal()
{
    PowerID idPower = sGetCurrentPower();
    float flDamageTotal = 0.0f;
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listDamage.Count() != 0) {
        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone != NULL) {
            for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++) {
                flDamageTotal += flDamageDone[i];
            }
        }
    }
    return flDamageTotal;
}

// Original PowersSwig.cpp:11698.
BOOL PowerBuffDamageShieldSetAmountOnBuff(int32 nBuffID, float flShieldAmount)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData != NULL) {
        ptBuffStartData->flShieldAmount = flShieldAmount;
        return TRUE;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11717.
float PowerBuffDamageShieldGetRemainingAmountOnBuff(int32 nBuffID, VACDID idActor)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (nBuffID == -1 || idActor == ACDID_INVALID) {
        if (ptPower == NULL || ptPower->listActiveCallbackBuffs.IsEmpty()) {
            return 0.0f;
        }
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(ptPower->listActiveCallbackBuffs.GetHead());
        if (ptBuff != NULL && !ptBuff->bDeleted) {
            return ptBuff->flShieldAmount;
        }
    } else {
        ActorCommonData * ptACD = ACDTryToGet(idActor);
        if (ptACD == NULL) {
            return 0.0f;
        }
        BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, nBuffID, ACDGet(idActor), ACDID_INVALID, FALSE);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff != NULL) {
            return ptBuff->flShieldAmount;
        }
    }
    return 0.0f;
}

// Original PowersSwig.cpp:11764.
float PowerBuffDamageShieldGetRemainingAmount(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    float flDamageShieldTotal = 0.0f;
    if (ptACD != NULL) {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos != NULL) {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff != NULL) {
                flDamageShieldTotal += ptBuff->flShieldAmount;
            }
        }
    }
    return flDamageShieldTotal;
}

// Original PowersSwig.cpp:11794.
#include "../Achievements/AchievementModifierEvaluationLogic.h"

float PowerBuffDamageShieldReduceDamageByPercent(float flPercentToReduce)
{
    flPercentToReduce = Math::Pin(flPercentToReduce, -10000.0f, 10000.0f);

    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    float flAmountAbsorbed = 0.0f;

    if (ptPower->listDamage.Count() == 0) {
        return 0.0f;
    }

    {
        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone != NULL) {
            for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++) {
                if (flDamageDone[iDamageType] > 1e-06f) {
                    flAmountAbsorbed += flPercentToReduce * flDamageDone[iDamageType];
                    flDamageDone[iDamageType] = (1.0f - flPercentToReduce) * flDamageDone[iDamageType];
                }
            }
        }
    }

    Player * ptPlayer = ACDGetControllingPlayer(ptPower->idACD);
    if (ptPlayer != NULL) {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);
        tEvaluationData.EvaluateEvent(ptPlayer,
                                      Achievement::ACH_EVENT_PLAYER_ABSORBED_DAMAGE_WITH_POWER,
                                      ptPower->snoPower,
                                      Math::FloatToInt(flAmountAbsorbed), (uint32)sub_71007b5e70());
    }

    return flAmountAbsorbed;
}

// Original PowersSwig.cpp:11847.
BOOL PowerBuffAppliedDamageIsMelee()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->eDeliveryMechanism == PDM_MELEE;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11866.
BOOL PowerBuffAppliedDamageIsRanged()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->eDeliveryMechanism == PDM_RANGED;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11885.
BOOL PowerBuffAppliedDamageIsThorns()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->eDeliveryMechanism == PDM_THORNS;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11904.
BOOL PowerBuffAppliedDamageIsDOT()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->eDeliveryMechanism == PDM_DOT;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11923.
BOOL PowerBuffAppliedDamageIsMagic()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->eDeliveryMechanism == PDM_MAGIC;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11942.
BOOL PowerBuffAppliedDamageIsCrit()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->bCriticalHit;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11961.
BOOL PowerBuffAppliedDamageIsCrushingBlow()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->bCrushingBlow;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11980.
BOOL PowerBuffAppliedDamageIsPhysical()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->bPhysical;
    }
    return FALSE;
}

// Original PowersSwig.cpp:11999.
BOOL PowerBuffAppliedDamageIsKnockback()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->eKnockBack != KNOCKBACK_NONE;
    }
    return FALSE;
}

// Original PowersSwig.cpp:12018.
BOOL PowerBuffAppliedDamageIsChill()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        if (ptPowerAPIDamage != NULL) {
            return ptPowerAPIDamage->bWasChill;
        }
    }
    return FALSE;
}

// Original PowersSwig.cpp:12042.
BOOL PowerBuffAppliedDamageWasBlocked()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->bDamageWasBlocked;
    }
    return FALSE;
}

// Original PowersSwig.cpp:12061.
float PowerBuffAppliedDamageGetBlockedAmount()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->flDamageBlocked;
    }
    return 0.0f;
}

// Original PowersSwig.cpp:12080.
SNO PowerBuffAppliedDamageGetPower()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return PowerAPI_GetDamageSourcePowerSNO(ptPowerAPIDamage, NULL);
    }
    return SNO_NONE;
}

// Original PowersSwig.cpp:12099.
int32 PowerBuffAppliedDamageGetPowerID()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() != 0) {
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        return ptPowerAPIDamage->idPower;
    }
    return POWERID_INVALID;
}

// Original PowersSwig.cpp:12118.
float PowerBuffAppliedDamageGetDamageTypeAmount(DamageType eDamageType)
{
    if (eDamageType < DAMAGE_TYPE_PHYSICAL || eDamageType >= NUM_DAMAGE_TYPES) {
        return 0.0f;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listDamage.Count() != 0) {
        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone != NULL) {
            return flDamageDone[eDamageType];
        }
    }
    return 0.0f;
}

// Original PowersSwig.cpp:12148.
float PowerBuffAppliedDamageGetTotal()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    float flTotal = 0.0f;
    if (ptPower->listDamage.Count() != 0) {
        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone != NULL) {
            for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++) {
                flTotal += flDamageDone[i];
            }
        }
    }
    return flTotal;
}

// Original PowersSwig.cpp:12180.
double PowerBuffAppliedDamageGetAttribute(Attrib eAttrib, int32 nParam)
{
    if (eAttrib <= ATTRIBUTE_NONE + 1 || eAttrib >= NUM_ATTRIBUTES) {
        return 0.0;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listPowerAPIDamage.Count() == 0) {
        return 0.0;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return 0.0;
    }
    Power * ptDamagePower = PowerTryToGet(ptPowerAPIDamage->idPower);
    if (ptDamagePower == NULL) {
        return 0.0;
    }
    double dValue = 0.0;
    switch (KeyGetDataType(MakeAttribKey(eAttrib, nParam))) {
        case ATTRIBUTEDT_FLOAT:
            dValue = FastAttribGetValueFloat(ptDamagePower->idFastAttrib, MakeAttribKey(eAttrib, nParam));
            break;
        case ATTRIBUTEDT_INT:
            dValue = FastAttribGetValueInt(ptDamagePower->idFastAttrib, MakeAttribKey(eAttrib, nParam));
            break;
    }
    return dValue;
}

// Original PowersSwig.cpp:12232.
void PowerBuffAppliedDamageSetDamageTypeAmount(DamageType eDamageType, float flAmount)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listDamage.Count() != 0) {
        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone != NULL) {
            flDamageDone[eDamageType] = flAmount;
        }
    }
}

// Original PowersSwig.cpp:12254.
void PowerBuffAppliedDamageModifyByPercent(float flPercent)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (ptPower->listDamage.Count() != 0 && ptPower->listPowerAPIDamage.Count() != 0) {
        float * flDamageDone = ptPower->listDamage.GetHead();
        PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
        if (flDamageDone == NULL || ptPowerAPIDamage->bIgnoreDamageBonusAndReduction) {
            return;
        }
        for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++) {
            flDamageDone[i] += flDamageDone[i] * flPercent;
            flDamageDone[i] = blz::max(0.0f, flDamageDone[i]);
        }
    }
}

// Original PowersSwig.cpp:12280.
void PowerBuffAppliedDamageConvertToDamageType(DamageType eDamageType)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listDamage.Count() == 0 || ptPower->listPowerAPIDamage.Count() == 0) {
        return;
    }
    float * flDamageDone = ptPower->listDamage.GetHead();
    if (flDamageDone == NULL) {
        return;
    }
    float flDamageTotal = 0.0f;
    for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++) {
        flDamageTotal += flDamageDone[i];
        flDamageDone[i] = 0.0f;
    }
    flDamageDone[eDamageType] = flDamageTotal;
}

// Original PowersSwig.cpp:12319.
DamageType PowerBuffAppliedDamageGetPredominantDamageType(BOOL bExcludePhysical)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower->listDamage.Count() != 0) {
        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone != NULL) {
            return GetPredominantDamageType(flDamageDone, bExcludePhysical);
        }
    }
    return DAMAGE_TYPE_NONE;
}


// Original PowersSwig.cpp:12345.
void PowerBuffAppliedDamageSetDamagingPowersOneTimeEventFlag(BOOL bValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() == 0) {
        return;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return;
    }
    Power * ptDamagePower = PowerTryToGet(ptPowerAPIDamage->idPower);
    if (ptDamagePower == NULL) {
        return;
    }
    if (bValue) {
        ptDamagePower->dwFlags |= (1 << PF_ONE_TIME_EVENT_USED);
    } else {
        ptDamagePower->dwFlags &= ~(1 << PF_ONE_TIME_EVENT_USED);
    }
}

// Original PowersSwig.cpp:12374.
BOOL PowerBuffAppliedDamageGetDamagingPowersOneTimeEventFlag()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() == 0) {
        return FALSE;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return FALSE;
    }
    Power * ptDamagePower = PowerTryToGet(ptPowerAPIDamage->idPower);
    if (ptDamagePower == NULL) {
        return FALSE;
    }
    return (ptDamagePower->dwFlags & (1 << PF_ONE_TIME_EVENT_USED)) != 0;
}

// Original PowersSwig.cpp:12410.
BOOL PowerBuffAppliedDamageDoRollAgainstProcCoefficient(float flChance)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() <= 0) {
        return FALSE;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return FALSE;
    }
    if (PowerTryToGet(ptPowerAPIDamage->idPower) == NULL) {
        return FALSE;
    }
    float flProcChance = ptPowerAPIDamage->flProcScalar * flChance;
    if (flProcChance > EPSILON && RandFloat_Internal(SGameGlobalsGet().rng) <= flProcChance) {
        return TRUE;
    }
    return FALSE;
}

// Original PowersSwig.cpp:12465.
float PowerBuffAppliedDamageGetProcCoefficient()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() <= 0) {
        return 0.0f;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return 0.0f;
    }
    return ptPowerAPIDamage->flProcScalar;
}

// Original PowersSwig.cpp:12493.
float PowerBuffAppliedDamageGetAttackSpeed()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() == 0) {
        return 1.0f;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return 1.0f;
    }
    PowerID idDamagePower = ptPowerAPIDamage->idPower;
    if (PowerTryToGet(idDamagePower) == NULL) {
        return 1.0f;
    }
    return PowerGetAttackSpeed(idDamagePower);
}

// Original PowersSwig.cpp:12523.
ACDID PowerBuffAppliedDamageGetPowerActorTarget()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() == 0) {
        return ACDID_INVALID;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return ACDID_INVALID;
    }
    Power * ptDamagePower = PowerTryToGet(ptPowerAPIDamage->idPower);
    if (ptDamagePower == NULL) {
        return ACDID_INVALID;
    }
    return ptDamagePower->tTarget.GetACD();
}

// Original PowersSwig.cpp:12552.
void PowerBuffAppliedDamageCursorSetToPowerTarget(int32 nCursorIndex)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() == 0) {
        return;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return;
    }
    Power * ptDamagePower = PowerTryToGet(ptPowerAPIDamage->idPower);
    if (nCursorIndex < 0 || ptDamagePower == NULL || nCursorIndex >= ptPower->listCursors.Count()) {
        return;
    }
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorIndex, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    ptDamagePower->tTarget.GetPlace(ptCursor->tPlace);
}

// Original PowersSwig.cpp:12588.
void PowerBuffAppliedDamageRetakeMeleeSnapshot()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL) {
        return;
    }

    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.IsEmpty() ? NULL : ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage == NULL) {
        return;
    }

    PowerID idPowerDamaging = ptPowerAPIDamage->idPower;
    Power * ptPowerDamaging = PowerTryToGet(idPowerDamaging);
    if (ptPowerDamaging == NULL) {
        return;
    }

    if (ptPowerDamaging->listInitialMeleeRangeACDs.Count() > 0) {
        ptPowerDamaging->listInitialMeleeRangeACDs.RemoveAll();
    }

    PowerSnapshotMeleeRange(ptPowerDamaging, ptPowerDamaging->idACD, 1.0f, ptPowerDamaging->tTarget);
}

// Original PowersSwig.cpp:12623.
void PowerBuffAppliedDamageSetForceCriticalDeath(BOOL bForceCritDeath)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() == 0) {
        return;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage != NULL) {
        ptPowerAPIDamage->bForceCriticalDeath = bForceCritDeath;
    }
}

// Original PowersSwig.cpp:12645.
void PowerBuffAppliedDamageForceCrit(BOOL bForceCrit)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL || ptPower->listPowerAPIDamage.Count() == 0) {
        return;
    }
    PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
    if (ptPowerAPIDamage != NULL) {
        ptPowerAPIDamage->bCriticalHit = bForceCrit;
    }
}

// Original PowersSwig.cpp:12667.
void PowerSendMessageToBuff(LPCSTR szTargetPower, int32 nBuffIndex, uint32 nParam0, uint32 nParam1, float flParam2)
{
    SNO snoTargetPower = SNOFileLookupGUID(POWER_SNO, szTargetPower);
    if (snoTargetPower == SNO_NONE) {
        return;
    }
    ACDID idACDCaster = ACDID_INVALID;
    LuaVerifySnosAreReferenced(snoTargetPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner != NULL) {
        idACDCaster = ptACDOwner->id;
    }
    PowerAPI_BuffApplyOnMessageEffects(idACDCaster, snoTargetPower, nBuffIndex, nParam0, nParam1, flParam2);
}

// Original PowersSwig.cpp:12688.
void PowerSendMessageToBuff(ACDID idActor, LPCSTR szTargetPower, int32 nBuffIndex, uint32 nParam0, uint32 nParam1, float flParam2)
{
    SNO snoTargetPower = SNOFileLookupGUID(POWER_SNO, szTargetPower);
    if (snoTargetPower == SNO_NONE) {
        return;
    }
    LuaVerifySnosAreReferenced(snoTargetPower, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE, SNO_NONE);
    PowerAPI_BuffApplyOnMessageEffects(idActor, snoTargetPower, nBuffIndex, nParam0, nParam1, flParam2);
}

// Original PowersSwig.cpp:12710.
void PowerBuffSetAsAura(int32 nBuffID, int32 nPriority)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    ptBuffStartData->bAura = true;
    ptBuffStartData->nAuraPriority = nPriority;
}

// Original PowersSwig.cpp:12732.
void PowerBuffAuraSetEffect(int32 nBuffID, LPCSTR szEffect)
{
}

// Original PowersSwig.cpp:12741.
void PowerBuffInternalCooldownSet(int32 nBuffID, float flDuration)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    ptBuffStartData->dwInternalCooldownDuration = (uint32)(flDuration * TICKS_PER_SECOND);
}

// Original PowersSwig.cpp:12762.
BOOL PowerBuffInternalCooldownIsReady(int32 nBuffID, ACDID idActor)
{
    PowerID idPower = sGetCurrentPower();
    BuffID idBuff = sBuffFindForACD(idPower, idActor, nBuffID, ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff == NULL) {
        return FALSE;
    }
    return ptBuff->dwInternalCooldownReadyTick <= GameGetServerTime();
}

// Original PowersSwig.cpp:12794.
void PowerBuffInternalCooldownTrigger(int32 nBuffID, ACDID idActor)
{
    PowerID idPower = sGetCurrentPower();
    BuffID idBuff = sBuffFindForACD(idPower, idActor, nBuffID, ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff != NULL) {
        ptBuff->dwInternalCooldownReadyTick = GameGetServerTime() + ptBuff->dwInternalCooldownDuration;
    }
}

// Original PowersSwig.cpp:12821.
void PowerBuffInternalCooldownReset(int32 nBuffID, ACDID idActor)
{
    PowerID idPower = sGetCurrentPower();
    BuffID idBuff = sBuffFindForACD(idPower, idActor, nBuffID, ACDID_INVALID, FALSE);
    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
    if (ptBuff != NULL) {
        ptBuff->dwInternalCooldownReadyTick = 0;
    }
}

// Original PowersSwig.cpp:12848.
void PowerBuffSetExclusiveType(int32 nBuffID, BuffExclusiveType eExclusiveType)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    if (ptBuffStartData->eExclusiveType != BUFF_EXCLUSIVE_TYPE_NONE) {
        FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_EXCLUSIVE_TYPE_ACTIVE, ptBuffStartData->eExclusiveType), FALSE);
    }
    ptBuffStartData->eExclusiveType = eExclusiveType;
    if (eExclusiveType != BUFF_EXCLUSIVE_TYPE_NONE) {
        FastAttribSetValueInt(ptBuffStartData->idFastAttrib, MakeAttribKey(ATTRIBUTE_BUFF_EXCLUSIVE_TYPE_ACTIVE, eExclusiveType), TRUE);
    }
}

// Original PowersSwig.cpp:12879.
void PowerBuffSetDoNotCopy(int32 nBuffID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData == NULL) {
        return;
    }
    ptBuffStartData->bDoNotCopy = true;
}

// Original PowersSwig.cpp:12899.
void PowerRemoveCCBuffs(ACDID idActor, BOOL bHardCCOnly)
{
    if (ACDTryToGet(idActor) == NULL) {
        return;
    }
    PowerAPI_FreeFromRoot(idActor);
    PowerAPI_FreeFromStun(idActor);
    PowerAPI_FreeFromFear(idActor);
    if (!bHardCCOnly) {
        PowerAPI_FreeFromSnare(idActor);
    }
}

// Original PowersSwig.cpp:12920.
void PowerRemoveNegativeBuffs(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }
    PowerRemoveCCBuffs(idActor, FALSE);
    PowerAPI_BuffRemoveAllNegativeBuffs(*ptACD);
}

// Original PowersSwig.cpp:12936.
void PowerRemoveNegativeBuff(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }

    BuffID idBuffLongest = BUFFID_INVALID;
    uint32 dwLongestDuration = 0;

    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos != NULL) {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff != NULL &&
            !ptBuff->bDeleted &&
            !ptBuff->bInfinite &&
            !ptBuff->bPositiveEffect &&
            ptBuff->flDamagePerSecond > 1e-06f &&
            !ptBuff->bUndispellable) {
            uint32 dwEndTick = ptBuff->dwStartTick + ptBuff->dwDuration;
            uint32 dwNow = GameGetServerTime();

            uint32 dwTimeRemaining = dwEndTick - dwNow;

            if (dwTimeRemaining >= dwLongestDuration) {
                idBuffLongest = ptBuff->id;
                dwLongestDuration = dwTimeRemaining;
            }
        }
    }

    PowerAPI_BuffFlagForDeletion(idBuffLongest, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
}

// Original PowersSwig.cpp:12977.

void PowerProxyActorSetCanDeactivate(ACDID idActor, BOOL bValue)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }
    BOOL bCanDeactivate = !bValue;  // Inferred: the negation is computed at 12987:28, before ACDGetOwner (12989).
    ActorFlagSet((ActorID)ACDGetOwner(idActor), ACTOR_DOESNT_DEACTIVATE_BIT, bCanDeactivate);
}

// Original PowersSwig.cpp:13005.
static BOOL sStartPower(ACDID idOwner, const Target & tTarget, LPCSTR szPowerName, BOOL bFreeCast, BOOL bDoesntAnimate, RuneType eRuneType, BOOL bIgnoreRange, BOOL bForceCast, uint32 dwAnimTagOverride, BOOL bAllowHitProcs);
BOOL PowerStartPower(ACDID idOwner, ACDID idTarget, LPCSTR szPowerName, BOOL bFreeCast, BOOL bDoesntAnimate, RuneType eRuneType, BOOL bIgnoreRange, BOOL bForceCast, uint32 dwAnimTagOverride, BOOL bAllowHitProcs)
{
    Target tTarget;
    tTarget.InitializeFromACD(idTarget, FALSE, FALSE);
    PowerID idPower = sGetCurrentPower();
    PowerTryToGet(idPower);
    return sStartPower(idOwner, tTarget, szPowerName, bFreeCast, bDoesntAnimate, eRuneType, bIgnoreRange, bForceCast, dwAnimTagOverride, bAllowHitProcs);
}

// Original PowersSwig.cpp:13027.
BOOL PowerStartPowerOnCursor(ACDID idOwner, int32 nCursorID, LPCSTR szPowerName, BOOL bFreeCast, BOOL bDoesntAnimate, RuneType eRuneType, BOOL bIgnoreRange, BOOL bForceCast, BOOL bAllowHitProcs)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return FALSE;
    }
    Target tTarget;
    tTarget.InitializeFromPlace(ptCursor->tPlace);
    return sStartPower(idOwner, tTarget, szPowerName, bFreeCast, bDoesntAnimate, eRuneType, bIgnoreRange, bForceCast, (uint32)-1, bAllowHitProcs);
}

// Original PowersSwig.cpp:13055.
// Condition order of 13139 read from the original's ccmp chain (column 47 not explained).
#line 13055
static BOOL sStartPower(
    ACDID idOwner,
    const Target & tTarget,
    LPCSTR szPowerName,
    BOOL bFreeCast,
    BOOL bDoesntAnimate,
    RuneType eRuneType,
    BOOL bIgnoreRange,
    BOOL bForceCast,
    uint32 dwAnimTagOverride,
    BOOL bAllowHitProcs)
{
    ActorCommonData * ptACDOwner = ACDTryToGet(idOwner);
    if (ptACDOwner == NULL) {
        return FALSE;
    }


    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    BOOL bExists = (snoPower != SNO_NONE);  // DWARF local; constant 1 after the 13092 test.

















    if (!bExists) {
        return FALSE;
    }



    if (bDoesntAnimate) {
        bFreeCast = TRUE;
    }


    uint32 dwFailureReasons = PowerFailureReasons(*ptACDOwner, AutoPowerDefinitionR(snoPower), &tTarget, FALSE, bFreeCast);

    if (bFreeCast && dwFailureReasons)
    {

        dwFailureReasons &= ~((1 << PS_NEED_CHARGES_BIT) | (1 << PS_NEED_RESOURCE_BIT));
    }

    if (bDoesntAnimate && dwFailureReasons)
    {
        dwFailureReasons &= ~(1 << PS_UNCANCELABLE_SKILLS_ACTIVE);
    }

    if (bIgnoreRange && dwFailureReasons)
    {
        dwFailureReasons &= ~(1 << PS_NOT_CLOSE_ENOUGH_BIT);
    }

    if (bForceCast && dwFailureReasons)
    {
        dwFailureReasons &= ~(1 << PS_CASTER_CANT_START_DISPLAYED_POWERS);
    }

    if (!PowerCanUseNow(*ptACDOwner, AutoPowerDefinitionR(snoPower), &tTarget, &dwFailureReasons)) {
        return FALSE;
    }


    ActorID idActor = (ActorID)ptACDOwner->idOwner;
    PowerID idPower = sGetCurrentPower();
    if (PowerIsPrimary(snoPower, ptACDOwner->id) && !bDoesntAnimate)
    {
        PowerID idCurrentPower = ActorPowersGetPrimary(idActor);



        if (idPower != idCurrentPower && !bForceCast && idCurrentPower != POWERID_INVALID)
        {
            if (PowerActorCanDo(idActor, snoPower) == FALSE)
            {
                return FALSE;
            }
        }
    }


    if (ACDIsPlayer(idOwner))
    {
        if (PowerAffectedByDualWield(AutoPowerDefinitionR(snoPower)) && !PowerIsPassive(snoPower) && ACDIsDualWielding(*ptACDOwner, TRUE))
        {
            ptACDOwner->AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, ptACDOwner->AttributesGetInt(ATTRIBUTE_DUALWIELD_HAND_NEXT));
            PlayerDetermineNextDualWieldHand(idOwner);
        }
    }

    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = idOwner;
    tPowerStartData.snoPower = snoPower;
    tPowerStartData.tTarget = tTarget;
    tPowerStartData.bFreeCast = bFreeCast;
    tPowerStartData.bDoesntAnimate = bDoesntAnimate;
    tPowerStartData.bAllowHitProcs = bAllowHitProcs;
    tPowerStartData.eRuneTypeOverride = eRuneType;
    tPowerStartData.dwAnimTagToUse = dwAnimTagOverride;

    PowerID idNewPower = PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, bForceCast, bIgnoreRange);
    return idNewPower != POWERID_INVALID;







}
#line 6363

// Original PowersSwig.cpp:13181.
PowerID BrainStanceDoPower(ActorID idActor, const Target & tTarget, SNO snoPower, EBrainPowerResult eReason, void * pData, const PreplayedAnimData * ptPreplayedAnimData, int32 nComboLevel, uint32 dwFlags);  // BrainStance.cpp; local declaration

BOOL PowerBrainStanceStartPowerAtCursor(ACDID idACD, int32 nCursorID, LPCSTR szPowerName)
{
    ActorCommonData * ptACDOwner = ACDTryToGet(idACD);
    if (ptACDOwner == NULL) {
        return FALSE;
    }

    if (ACDIsPlayer(idACD)) {
        return FALSE;
    }

    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    BOOL bExists = (snoPower != SNO_NONE);
    if (!bExists) {
        return FALSE;
    }

    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return FALSE;
    }

    Target tTarget;
    tTarget.InitializeFromPlace(ptCursor->tPlace);

    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    PowerID idPowerNew = BrainStanceDoPower(idActor, tTarget, snoPower, BRAIN_POWER_START, NULL, NULL, 0, 0);
    return idPowerNew != POWERID_INVALID;
}

// Original PowersSwig.cpp:13248.
BOOL PowerStartPassive(ACDID idOwner, LPCSTR szPowerName)
{
    ActorCommonData * ptACDOwner = ACDTryToGet(idOwner);
    if (ptACDOwner == NULL) {
        return FALSE;
    }
    PowerID idPower = sGetCurrentPower();
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (snoPower == SNO_NONE) {
        return FALSE;
    }
    if (!PowerIsPassive(snoPower)) {
        return FALSE;
    }
    ptACDOwner->ApplyPassive(snoPower, FALSE);
    return TRUE;
}

// Original PowersSwig.cpp:13297.
void PowerActorKill(ACDID idTarget, ACDID idKiller)
{
    ActorCommonData * ptACD = ACDTryToGet(idTarget);
    if (ptACD == NULL) {
        return;
    }
    if (ptACD->IsAlive()) {
        const BOOL fNoExp = TRUE;
        const BOOL fNoLoot = TRUE;
        const BOOL fNoHealth = TRUE;
        ActorID idActor = (ActorID)ACDGetOwner(idTarget);
        ActorID idKillerActor = ACTORID_INVALID;
        if (idKiller != ACDID_INVALID) {
            idKillerActor = (ActorID)ACDGetOwner(idKiller);
        }
        BrainAPIDoDeath(idActor, idKillerActor, fNoExp, fNoLoot, fNoHealth, NULL, NULL);
    }
}

// Original PowersSwig.cpp:13319.
void PowerConsumeCorpse(ACDID idCorpse, BOOL bFakeCorpse)
{
    ActorCommonData * ptACDCorpse = ACDTryToGet(idCorpse);
    if (ptACDCorpse == NULL) {
        return;
    }
    if (!bFakeCorpse && !ptACDCorpse->IsNecroCorpse()) {
        return;
    }

    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return;
    }

    if (ptACDOwner->IsPet()) {
        Player * ptPlayer = ptACDOwner->GetPetOwnerPlayer();
        if (ptPlayer != NULL) {
            ptACDOwner = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDOwner == NULL) {
                return;
            }
        }
    }

    BOOL bDeleteCorpse = FALSE;
    if (!bFakeCorpse) {
        Power * ptPower = PowerGet(idPower);
        int32 nCharges = ptACDCorpse->AttributesGetInt(ATTRIBUTE_NECROMANCER_CORPSE_CHARGES);
        nCharges--;
        if (nCharges > 0) {
            ptACDCorpse->AttributesSetInt(ATTRIBUTE_NECROMANCER_CORPSE_CHARGES, nCharges);
        } else {
            bDeleteCorpse = TRUE;
        }
    }

    PowerAPI_BuffApplyOnCorpseConsumedEffect(*ptACDOwner, idCorpse);

    if (bDeleteCorpse && ACDTryToGet(idCorpse) != NULL) {
        ActorDelete(idCorpse);
    }
}

// Original PowersSwig.cpp:13567.
void PowerActorBurrowIn(VACDID idActor, BOOL bInvisibleDuring, WeaponClass eBurrowWeaponClass)
{
    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (ptACDTarget == NULL) {
        return;
    }
    PowerAPI_DoBurrowIn(ptACDTarget, bInvisibleDuring, eBurrowWeaponClass, -1, TRUE);
}

// Original PowersSwig.cpp:13582.
void PowerActorBurrowOut(VACDID idActor, BOOL bSetUsedSpecialSpawn)
{
    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (ptACDTarget == NULL) {
        return;
    }
    PowerAPI_DoBurrowOut(ptACDTarget, bSetUsedSpecialSpawn);
}

// Original PowersSwig.cpp:13596.
// DWARF: tImpactData is built with ImpactData(ACDID) (PowerAPI.h:55) at line 13603.
void PowerProjectileKill(ACDID idProjectile)
{
    ActorCommonData * ptACDProjectile = ACDTryToGet(idProjectile);
    if (ptACDProjectile == NULL || !ACDIsProjectile(idProjectile)) {
        return;
    }
    ImpactData tImpactData(ptACDProjectile->id);


    ProjectileHadCollision(tImpactData);
}

// Original PowersSwig.cpp:13609.
void PowerSwitchDualWieldHand()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return;
    }
    if (PowerAffectedByDualWield(AutoPowerDefinitionR(ptPower->snoPower)) && ACDIsDualWielding(*ptACDOwner, TRUE)) {
        int32 nHand = FastAttribGetValueInt(ptPower->idFastAttrib, ATTRIBUTE_DUALWIELD_HAND);
        FastAttribSetValueInt(ptPower->idFastAttrib, ATTRIBUTE_DUALWIELD_HAND, nHand == 0);
        FastAttribSetValueInt(ptPower->idFastAttrib, ATTRIBUTE_DUALWIELD_HAND_NEXT, nHand);
        ptACDOwner->AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, ptACDOwner->AttributesGetInt(ATTRIBUTE_DUALWIELD_HAND_NEXT));
        PlayerDetermineNextDualWieldHand(ptACDOwner->id);
    }
}

// Original PowersSwig.cpp:13636.
float PowerActorGetCritChance(ACDID idActor)
{
    ActorCommonData * ptACDAttacker = ACDTryToGet(idActor);
    if (ptACDAttacker == NULL) {
        return 0.0f;
    }
    return PowerComputeCritChance(ptACDAttacker, FALSE);
}

// Original PowersSwig.cpp:13650.
float PowerActorGetMeleeDodgeChance(ACDID idActor, ACDID idTarget)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    ActorCommonData * ptACDTarget = ACDTryToGet(idTarget);
    if (ptACD == NULL) {
        return 0.0f;
    }
    int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
    if (ptACDTarget != NULL) {
        nLevel = ptACDTarget->AttributesGetInt(ATTRIBUTE_LEVEL);
    }
    return PowerComputeDodgeChance(*ptACD, nLevel, PDM_MELEE, FALSE);
}

// Original PowersSwig.cpp:13673.
BOOL PowerActorDoesNotHaveHeadHardpoint(ACDID idActor)
{
    ActorCommonData * ptACDActor = ACDTryToGet(idActor);
    if (ptACDActor == NULL) {
        return FALSE;
    }

    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(idActor));
    if (ptActor == NULL) {
        return FALSE;
    }

    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    if (pdefActor == NULL) {
        return FALSE;
    }

    return TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_DOES_NOT_HAVE_HEAD_HARDPOINT) != 0;
}

// Original PowersSwig.cpp:13703.
// NON_MATCHING: the offset multiply has its operands swapped (fmul vec, offset vs fmul offset, vec); 3 insns
#line 13703
void PowerOffsetTranslatePowerTargetPosition(
    float flOffset)
{
    PowerID idPower = sGetCurrentPower();
    Power *ptPower = PowerGet(idPower);

    ACDID idActor = PowerGetOwner();
    const ActorCommonData * ptACD = ACDTryToGet(idActor);

    if (ptPower && ptACD)
    {
        Target tTarget = ptPower->tTarget;

        WorldPlace wpTarget;
        tTarget.GetPlace(wpTarget);

        WorldPlace wpActor = ptACD->GetPlace();

        if(wpTarget.idSWorld != wpActor.idSWorld)
        {
            return;
        }



        Vector3D vecTowardsTarget = wpTarget.wp - wpActor.wp;

        float flDistanceMin = PowerGetFormulaValue(idPower, TAG_POWER_WALKING_DISTANCE_MIN);
        float flTargetDistance = vecTowardsTarget.Magnitude();


        float flModifiedDistOffset = flOffset;
        if(flTargetDistance - flOffset <= flDistanceMin)
        {
            flModifiedDistOffset = flTargetDistance - flOffset;
        }


        vecTowardsTarget.Normalize();



        WorldPlace wpTargetOffset = wpTarget;
        wpTargetOffset.wp -= vecTowardsTarget * flModifiedDistOffset;

        const ActorMovement *ptMove = ptACD->GetActorMovement();
        WorldPlace wpResult;
        if (ptMove && PowerComputeDestination(ptPower->snoPower, *ptMove, wpTargetOffset, wpTarget, wpResult, FALSE, 10.0f))
        {
            ptPower->tTarget.tPlace = wpResult;
        }
    }
}
#line 6624


// Original PowersSwig.cpp:13759.
#line 13759
int32 PowerGetRuneLevelForRuneType(
    RuneType eRuneType)
{
    PowerID idPower = sGetCurrentPower();
    Power *ptPower = PowerGet(idPower);

    int32 nRuneLevel = 0; // DWARF local: constant 0 over the whole function, never read.
    Attrib eAttrib = ATTRIBUTE_NONE;
    Attrib eAttribOverride = ATTRIBUTE_NONE;

    switch (eRuneType)
    {
    case RUNE_TYPE_A:
        eAttrib = ATTRIBUTE_RUNE_A;
        eAttribOverride = ATTRIBUTE_RUNE_A_OVERRIDE;
        break;
    case RUNE_TYPE_B:
        eAttrib = ATTRIBUTE_RUNE_B;
        eAttribOverride = ATTRIBUTE_RUNE_B_OVERRIDE;
        break;
    case RUNE_TYPE_C:
        eAttrib = ATTRIBUTE_RUNE_C;
        eAttribOverride = ATTRIBUTE_RUNE_C_OVERRIDE;
        break;
    case RUNE_TYPE_D:
        eAttrib = ATTRIBUTE_RUNE_D;
        eAttribOverride = ATTRIBUTE_RUNE_D_OVERRIDE;
        break;
    case RUNE_TYPE_E:
        eAttrib = ATTRIBUTE_RUNE_E;
        eAttribOverride = ATTRIBUTE_RUNE_E_OVERRIDE;
        break;
    default:
        DisplayInternalError(NULL, "", 13836, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }

    if (ptPower)
    {
        ActorCommonData *ptACDOwner = ACDTryToGet(ptPower->idACD);
        if (ptACDOwner && FastAttribExists(ptACDOwner->idFastAttrib, MakeAttribKey(eAttribOverride, ptPower->snoPower)))
        {
            return FastAttribGetValueInt(ptACDOwner->idFastAttrib, MakeAttribKey(eAttribOverride, ptPower->snoPower));
        }
    }



        return FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(eAttrib, ptPower->snoPower));



}
#line 6644

// Original PowersSwig.cpp:13814.
int32 PowerGetComboLevel(int32 idPower)
{
    if (idPower == POWERID_INVALID || PowerTryToGet(idPower) == NULL) {
        idPower = sGetCurrentPower();
    }
    Power * ptPower = PowerGet(idPower);
    if (!PowerIsComboPower(ptPower->snoPower)) {
        return 0;
    }
    return ptPower->nComboLevel;
}

// Original PowersSwig.cpp:13832.
ACDID PowerGetTargetActor(int32 idPower)
{
    if (idPower == POWERID_INVALID || PowerTryToGet(idPower) == NULL) {
        idPower = sGetCurrentPower();
    }
    return PowerGetTargetACD(idPower);
}

// Original PowersSwig.cpp:13843.
void PowerSetTargetFromActor(VACDID idActor)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (ACDTryToGet(idActor) == NULL) {
        return;
    }
    ptPower->tTarget.InitializeFromACD(idActor, FALSE, FALSE);
}

// Original PowersSwig.cpp:13858.
void PowerSetTargetFromCursor(int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    ptPower->tTarget.InitializeFromPlace(ptCursor->tPlace);
}

// Original PowersSwig.cpp:14004.
void PowerActorFaceCursor(VACDID idActor, int32 nCursorID, BOOL bSnapToFacing)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (ptCursor == NULL) {
        return;
    }
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }
    if (ptACD->GetPlace().idSWorld != ptCursor->tPlace.idSWorld) {
        return;
    }
    ptACD->SetFacing(ptCursor->tPlace, bSnapToFacing);
}

// Original PowersSwig.cpp:14034.
double PowerCursorDistanceBetweenCursors(int32 nCursorIDFrom, int32 nCursorIDTo)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorFrom = sPowerGetCursorAtIndex(ptPower, nCursorIDFrom, TRUE);
    PowerCursor * ptCursorTo = sPowerGetCursorAtIndex(ptPower, nCursorIDTo, TRUE);
    if (ptCursorFrom && ptCursorTo && ptCursorFrom->tPlace.idSWorld == ptCursorTo->tPlace.idSWorld) {
        return Vector3D::Distance(ptCursorFrom->tPlace.wp, ptCursorTo->tPlace.wp);
    }
    return 3.4028234663852886e+38f;
}

// Original PowersSwig.cpp:14057.
double PowerCursorDistanceSquaredBetweenCursors(int32 nCursorIDFrom, int32 nCursorIDTo)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorFrom = sPowerGetCursorAtIndex(ptPower, nCursorIDFrom, TRUE);
    PowerCursor * ptCursorTo = sPowerGetCursorAtIndex(ptPower, nCursorIDTo, TRUE);
    if (ptCursorFrom && ptCursorTo && ptCursorFrom->tPlace.idSWorld == ptCursorTo->tPlace.idSWorld) {
        return Vector3D::DistanceSquared(ptCursorFrom->tPlace.wp, ptCursorTo->tPlace.wp);
    }
    return 3.4028234663852886e+38f;
}

// Original PowersSwig.cpp:14152.
double PowerActorDistanceToCursor(VACDID idACDActorFrom, int32 nCursorIDTo)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDFrom = ACDTryToGet(idACDActorFrom);
    if (ptACDFrom == NULL) {
        return 3.4028234663852886e+38f;
    }
    PowerCursor * ptCursorTo = sPowerGetCursorAtIndex(ptPower, nCursorIDTo, TRUE);
    if (ptCursorTo == NULL || ptACDFrom->GetPlace().idSWorld != ptCursorTo->tPlace.idSWorld) {
        return 3.4028234663852886e+38f;
    }
    return Vector3D::Distance(ptACDFrom->GetPlace().wp, ptCursorTo->tPlace.wp);
}

// Original PowersSwig.cpp:14180.
double PowerActorDistanceSquaredToCursor(VACDID idACDActorFrom, int32 nCursorIDTo)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDFrom = ACDTryToGet(idACDActorFrom);
    if (ptACDFrom == NULL) {
        return 3.4028234663852886e+38f;
    }
    PowerCursor * ptCursorTo = sPowerGetCursorAtIndex(ptPower, nCursorIDTo, TRUE);
    if (ptCursorTo == NULL || ptACDFrom->GetPlace().idSWorld != ptCursorTo->tPlace.idSWorld) {
        return 3.4028234663852886e+38f;
    }
    return Vector3D::DistanceSquared(ptACDFrom->GetPlace().wp, ptCursorTo->tPlace.wp);
}

// Original PowersSwig.cpp:14208.
double PowerActorVerticalDistanceToActor(VACDID idACDActorFrom, VACDID idACDActorTo)
{
    ActorCommonData * ptACDFrom = ACDTryToGet(idACDActorFrom);
    if (ptACDFrom == NULL) {
        return 3.4028234663852886e+38f;
    }
    ActorCommonData * ptACDTo = ACDTryToGet(idACDActorTo);
    if (ptACDTo == NULL) {
        return 3.4028234663852886e+38f;
    }
    if (ptACDFrom->GetPlace().idSWorld != ptACDTo->GetPlace().idSWorld) {
        return 3.4028234663852886e+38f;
    }
    return Math::Abs(ptACDFrom->GetPlace().wp.z - ptACDTo->GetPlace().wp.z);
}

// Original PowersSwig.cpp:14672.
static float sPowerGetAngleBetween(const WorldPlace & tPlaceSource, const Quaternion & qSource, const WorldPlace & tPlaceDest)
{
    if (tPlaceSource.idSWorld != tPlaceDest.idSWorld) {
        return 0.0f;
    }
    Vector3D vFacing = cgwvXAxis;
    Vector3D::Rotate(cgwvZAxis, qSource.ToYaw(), vFacing);
    Vector3D vBetween;
    WorldPlaceGetVectorBetween(tPlaceSource, tPlaceDest, vBetween);
    return Math::AngleToDeg(Math::NormalizeAngle(Vector3D::AngleBetween(vFacing, vBetween)));
}

// Original PowersSwig.cpp:14695.
float PowerCursorAngleToCursor(int32 nCursorSource, int32 nCursorDest)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorSource = sPowerGetCursorAtIndex(ptPower, nCursorSource, TRUE);
    PowerCursor * ptCursorDest = sPowerGetCursorAtIndex(ptPower, nCursorDest, TRUE);
    if (ptCursorSource && ptCursorDest) {
        return sPowerGetAngleBetween(ptCursorSource->tPlace, ptCursorSource->qOrientation, ptCursorDest->tPlace);
    }
    return 0.0f;
}

// Original PowersSwig.cpp:14714.
float PowerCursorAngleToActor(int32 nCursorSource, ACDID idActorDest)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDDest = ACDTryToGet(idActorDest);
    PowerCursor * ptCursorSource = sPowerGetCursorAtIndex(ptPower, nCursorSource, TRUE);
    if (ptACDDest == NULL || ptCursorSource == NULL) {
        return 0.0f;
    }
    return sPowerGetAngleBetween(ptCursorSource->tPlace, ptCursorSource->qOrientation, ptACDDest->GetPlace());
}

// Original PowersSwig.cpp:14738.
float PowerActorAngleToCursor(ACDID idActorSource, int32 nCursorDest)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDSource = ACDTryToGet(idActorSource);
    PowerCursor * ptCursorDest = sPowerGetCursorAtIndex(ptPower, nCursorDest, TRUE);
    if (ptACDSource == NULL || ptCursorDest == NULL) {
        return 0.0f;
    }
    return sPowerGetAngleBetween(ptACDSource->GetPlace(), ptACDSource->GetOrientation(), ptCursorDest->tPlace);
}

// Original PowersSwig.cpp:14762.

float PowerActorAngleToActor(ACDID idActorSource, ACDID idActorDest)
{
    ActorCommonData * ptACDSource = ACDTryToGet(idActorSource);
    ActorCommonData * ptACDDest = ACDTryToGet(idActorDest);
    if (ptACDSource == NULL || ptACDDest == NULL) {
        return 0.0f;
    }
    return sPowerGetAngleBetween(ptACDSource->GetPlace(), ptACDSource->GetOrientation(), ptACDDest->GetPlace());
}

// Original PowersSwig.cpp:14778.
float PowerCursorGetYaw(int32 nCursor)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursor, TRUE);
    if (ptCursor == NULL) {
        return 0.0f;
    }
    return Math::AngleToDeg(ptCursor->qOrientation.ToYaw());
}

// Original PowersSwig.cpp:14794.
float PowerCursorGetHeight(int32 nCursor)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorDest = sPowerGetCursorAtIndex(ptPower, nCursor, TRUE);
    if (ptCursorDest == NULL) {
        return 0.0f;
    }
    return ptCursorDest->tPlace.wp.z;
}

// Original PowersSwig.cpp:14846.
BOOL PowerItemLabelIsEquipped(ItemLabel eItemLabel)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);
    if (ptACDOwner == NULL) {
        return FALSE;
    }
    const VisualInventory * ptVisualInventory = VisualInventoryGet(ptACDOwner->id);
    if (ptVisualInventory == NULL) {
        return FALSE;
    }
    VisualInventorySlot eVisualSlotLeftHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_LEFT_HAND);
    VisualInventorySlot eVisualSlotRightHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_RIGHT_HAND);
    GBID gbidItemLeft = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotLeftHand].gbidItem;
    GBID gbidItemRight = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotRightHand].gbidItem;

    return ItemHasLabel(gbidItemLeft, eItemLabel) || ItemHasLabel(gbidItemRight, eItemLabel);
}

// Original PowersSwig.cpp:14898.
ACDID PowerGetMainHandWeapon(ACDID idACDPlayer)
{
    if (!ACDIsPlayer(idACDPlayer)) {
        return ACDID_INVALID;
    }
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer == NULL) {
        return ACDID_INVALID;
    }
    return ACDInventoryGetSquareItem(*ptACDPlayer, SLOT_PLAYER_LEFT_HAND, IVector2D(0, 0));
}

// Original PowersSwig.cpp:14917.
ACDID PowerGetOffHandWeapon(ACDID idACDPlayer)
{
    if (!ACDIsPlayer(idACDPlayer)) {
        return ACDID_INVALID;
    }
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer == NULL) {
        return ACDID_INVALID;
    }
    return ACDInventoryGetSquareItem(*ptACDPlayer, SLOT_PLAYER_RIGHT_HAND, IVector2D(0, 0));
}

// Original PowersSwig.cpp:14936.
BOOL PowerItemIsEquipped(ACDID idACDItem)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL) {
        return FALSE;
    }
    return ItemIsEquipped(*ptACDItem);
}

// Original PowersSwig.cpp:14997.
BOOL PowerPlayerIsDualWielding(VACDID idPlayer)
{
    return ACDIsDualWielding(ACDGet(idPlayer), TRUE);
}

// Original PowersSwig.cpp:15249.
void PowerSetStoredInt(LPCSTR szName, int32 nValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerSetStoredInt(ptPower, szName, nValue);
}

// Original PowersSwig.cpp:15259.
int32 PowerGetStoredInt(LPCSTR szName)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    return PowerGetStoredInt(ptPower, szName);
}

// Original PowersSwig.cpp:15268.
void PowerSetStoredFloat(LPCSTR szName, float flValue)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerSetStoredFloat(ptPower, szName, flValue);
}

// Original PowersSwig.cpp:15278.
float PowerGetStoredFloat(LPCSTR szName)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    return PowerGetStoredFloat(ptPower, szName);
}

// Original PowersSwig.cpp:15287.
void PowerActorAddResource(ACDID idACDActor, float flAmount, BOOL bPrimary, BOOL bApplyOnHitBonuses)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD == NULL) {
        return;
    }
    ResourceType eResourceType = bPrimary ? ptACD->GetPrimaryResourceType() : ptACD->GetSecondaryResourceType();
    ptACD->AddResource(eResourceType, flAmount, bApplyOnHitBonuses);
}

// Original PowersSwig.cpp:15302.
void PowerActorAddCharges(ACDID idACDActor, SNO snoPower, int32 nAmount)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD == NULL) {
        return;
    }
    int32 nCharges = Math::Max(ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower)) + nAmount, 0);
    nCharges = Math::Min(PowerActorGetMaxCharges(*ptACD, snoPower), nCharges);
    if (nCharges > 98) {
        nCharges = 99;
    }
    ptACD->AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower), nCharges);
}

// Original PowersSwig.cpp:15460.
void PowerDamageActor(ACDID idACDActor, float flAmount)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (ptACD == NULL) {
        return;
    }
    if (ActorFlagIsSet((ActorID)ptACD->idOwner, ACTOR_GOD_MODE_BIT)) {
        return;
    }
    ptACD->SetHitPoints(ptACD->GetHitPointsCurrent() - flAmount, TRUE, TRUE);
    if (ptACD->GetHitPointsCurrent() <= 0.0f) {
        BrainAPIDoDeath((ActorID)ptACD->idOwner, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    }
}

// Original PowersSwig.cpp:15485.
float PowerActorGetHitPointsPercent(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return 0.0f;
    }
    return ptACD->GetHitpointPercent();
}

// Original PowersSwig.cpp:15498.
float PowerActorGetHitPoints(ACDID idActor)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return 0.0f;
    }
    return ptACD->GetHitPointsCurrent();
}

// Original PowersSwig.cpp:15511.
void PowerActorSetHitPointsPercent(ACDID idActor, float flPercent)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }
    ptACD->SetHitPoints(ptACD->GetHitPointsMaxTotal() * flPercent, TRUE, FALSE);
}

// Original PowersSwig.cpp:15524.
void PowerActorSetHitPoints(ACDID idActor, float flHP)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return;
    }
    ptACD->SetHitPoints(flHP, TRUE, FALSE);
}

// Original PowersSwig.cpp:15536.
void PowerAddTrackedActor(ACDID idACD, BOOL bAddReference)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    if (bAddReference) {
        LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACD);
    }
    if (ptPower->listACDsTracked.Find(idACD) == NULL) {
        ptPower->listACDsTracked.AddTail_Tracked(idACD);
    }
}

// Original PowersSwig.cpp:15601.
void PowerRemoveTrackedActor(ACDID idACD)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    POS pos = ptPower->listACDsTracked.Find(idACD);
    if (pos != NULL) {
        ptPower->listACDsTracked.RemoveAt(pos);
    }
}

// Original PowersSwig.cpp:15617.
BOOL PowerIsTrackedActor(ACDID idACD)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    return ptPower->listACDsTracked.Find(idACD) != NULL;
}

// Original PowersSwig.cpp:15635.
ActorIDArray * PowerGetTrackedActors()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;
    POS pos = ptPower->listACDsTracked.GetHeadPosition();
    while (pos != NULL) {
        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = (ACDID)ptPower->listACDsTracked.GetNext(pos);
        tArrACDs.nNumActorIDs++;
        if (tArrACDs.nNumActorIDs == 64) {
            break;
        }
    }
    return &tArrACDs;
}

// Original PowersSwig.cpp:15664.
void PowerClearTrackedActors()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ptPower->listACDsTracked.RemoveAll();
}

// Original PowersSwig.cpp:15706.
void PowerAddAffectedActor(ACDID idACDTarget, ACDID idACDSource)
{
    PowerID idPower = sGetCurrentPower();
    PowerAddAffectedACD(idPower, idACDTarget, idACDSource);
}

// Original PowersSwig.cpp:15777.
void PowerInstanceSetDamagePercentBonus(int32 idPower, float flDamagePercentBonus)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return;
    }
    FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_INSTANCE_DAMAGE_PERCENT_BONUS, idPower), flDamagePercentBonus);
}

// Original PowersSwig.cpp:15791.
void PowerIncrementLockGlobalsCounter()
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    ptPower->nLockGlobalsCount++;
}

// Original PowersSwig.cpp:15800.
void PowerActorAttachActorToActor(ACDID idACDToAttach, ACDID idACDAttachTo)
{
    ActorCommonData * ptACDAttachment = ACDTryToGet(idACDToAttach);
    ActorCommonData * ptACDAttachTo = ACDTryToGet(idACDAttachTo);
    if (ptACDAttachment == NULL || ptACDAttachTo == NULL) {
        return;
    }
    ptACDAttachTo->AttachACD(*ptACDAttachment);
}

// Original PowersSwig.cpp:15816.
void PowerActorDetachActorFromActor(ACDID idACDToAttach, ACDID idACDAttachTo)
{
    ActorCommonData * ptACDAttachment = ACDTryToGet(idACDToAttach);
    ActorCommonData * ptACDAttachTo = ACDTryToGet(idACDAttachTo);
    if (ptACDAttachment == NULL || ptACDAttachTo == NULL) {
        return;
    }
    ptACDAttachTo->DetachACD(*ptACDAttachment);
}

// Original PowersSwig.cpp:16089.
static const int32 EXPENSIVE_PROC_PER_TARGET_CAP = 30;  // PowersSwig.cpp:16088 (DWARF const_value); nCount (16095) is a block local.
BOOL PowerCanDoExpensiveProc(ACDID idACDTarget)
{
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget != NULL) {
        int32 nCount = ptACDTarget->AttributesGetInt(ATTRIBUTE_EXPENSIVE_PROC_COUNT);
        if (nCount >= EXPENSIVE_PROC_PER_TARGET_CAP) { return FALSE; }
    }
    return TRUE;
}

// Original PowersSwig.cpp:16106.
void PowerIncExpensiveProcCounter(ACDID idACDTarget)
{
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL) {
        return;
    }
    ptACDTarget->AttributesSetInt(ATTRIBUTE_EXPENSIVE_PROC_COUNT, ptACDTarget->AttributesGetInt(ATTRIBUTE_EXPENSIVE_PROC_COUNT) + 1);
}

// Original PowersSwig.cpp:16120.
void PowerTriggerOnAttackCallbacks(ACDID idACD, ACDID idACDTarget)
{
    if (ACDTryToGet(idACD) == NULL) {
        return;
    }
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL) {
        return;
    }
    PowerAPI_BuffApplyOnAttackEffects(idACD, idACDTarget, idPower, ptPower->snoPower);
}

// Original PowersSwig.cpp:16143.
void PowerPreventEvade(ACDID idACDTarget, BOOL bBlock)
{
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL) {
        return;
    }
    ptACDTarget->AttributesSetInt(ATTRIBUTE_PREVENT_EVADE_DURING, bBlock);
}

// Original PowersSwig.cpp:16163.
float PowerGetRemainingRechargeTime(VACDID idActor, LPCSTR szPowerName)
{
    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (ptACD == NULL) {
        return 0.0f;
    }
    SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerName);
    if (snoPower == SNO_NONE) {
        return 0.0f;
    }
    uint32 dwNow = GameGetServerTime();
    uint32 dwNextCharge = ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_NEXT_CHARGE_GAINED_TIME, snoPower));
    if (dwNextCharge == (uint32)-1 || dwNextCharge <= dwNow) {
        return 0.0f;
    }
    return (float)(dwNextCharge - dwNow) * OO_TICKS_PER_SECOND;
}

// Original PowersSwig.cpp:16392.
static void sForceRemoteEffects(VACDID idActor)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL) {
        return;
    }
    ActorCommonData * ptACD = ACDTryToGet(ptACDOwner->id);
    if (ptACD == NULL) {
        return;
    }
    ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
    if (pdefActor == NULL || pdefActor->snoMonster == SNO_NONE) {
        return;
    }
    ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
    if (pdefMonster == NULL || !TagMapGetInt(pdefMonster->hTagMap, TAG_MONSTER_FORCE_REMOTE_PLAYER_EFFECTS)) {
        return;
    }
    ActorCommonData * ptACDTarget = ACDTryToGet(idActor);
    if (ptACDTarget != NULL) {
        ptACDTarget->AttributesSetInt(ATTRIBUTE_FORCE_REMOTE_FX, TRUE);
    }
}

// Original PowersSwig.cpp:16416.
int32 PowerActorPlayEffectGroup(VACDID idActor, LPCSTR szEffectGroupName, BOOL bUseComplexEffect, ACDID idDestinationPlayer)
{
    sForceRemoteEffects(idActor);
    return ActorPlayEffectGroup(idActor, szEffectGroupName, bUseComplexEffect, idDestinationPlayer);
}

// Original PowersSwig.cpp:16427.
BOOL PowerActorPlayResourceGainedEffectGroup(VACDID idActor, ResourceType eResourceType)
{
    sForceRemoteEffects(idActor);
    return ActorPlayResourceGainedEffectGroup(idActor, eResourceType);
}

// Original PowersSwig.cpp:16436.
int32 PowerActorPlayEffectGroupActorToActor(VACDID idActorFrom, VACDID idActorTo, LPCSTR szEffectGroupName, BOOL bUseComplexEffect)
{
    sForceRemoteEffects(idActorFrom);
    return ActorPlayEffectGroupActorToActor(idActorFrom, idActorTo, szEffectGroupName, bUseComplexEffect);
}

// Original PowersSwig.cpp:16447.
int32 PowerActorPlayEffectGroupRopeActorToActor(VACDID idActorFrom, RopePosition ePositionFrom, VACDID idActorTo, RopePosition ePositionTo, LPCSTR szRopeName, BOOL bUseComplexEffect, BOOL bIgnoreOwnerAlpha)
{
    sForceRemoteEffects(idActorFrom);
    return ActorPlayEffectGroupRopeActorToActor(idActorFrom, ePositionFrom, idActorTo, ePositionTo, szRopeName, bUseComplexEffect, bIgnoreOwnerAlpha);
}

// Original PowersSwig.cpp:16593.
void PowerDebugDrawSphere(int32, float, uint32)
{
}

// Original PowersSwig.cpp:16594.
void PowerDebugDrawLine(int32, int32, uint32)
{
}

// Original PowersSwig.cpp:16595.
void PowerDebugDrawCapsule(int32, int32, float, uint32)
{
}

// Original PowersSwig.cpp:16596.
void PowerDebugDrawPie(int32, float, float, uint32)
{
}

// Original PowersSwig.cpp:16597.
void PowerDebugDrawText(int32, LPCSTR, uint32)
{
}


#line 14878
BOOL PowerItemHasLabel(
    ACDID idActorItem,
    ItemLabel eItemLabel)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idActorItem);
    if (!ptACDItem)
        return FALSE;
#line 14889
    if (!ptACDItem->IsItem())
        return FALSE;
#line 14894
    return ItemHasLabel(ptACDItem->hGB.gbid, eItemLabel);
}

#include "../../Common/BBEUtil.h"

#line 15985
float PowerGetLookupTableValue(
    LPCSTR szTable,
    int32 nValueIndex)
{
    float flValue = 0.0f;


    uint32 nEntryIndex = BBEPowerFormulaTableIndexFromString(szTable);
    if (nEntryIndex == (uint32)-1)
        return flValue;
#line 16002
    const GameBalanceDefinition * ptGameBalDef = PowerFormulaTableLockR();
    const PowerFormulaTable & tPowerFormulaTable = ptGameBalDef->tPowerFormulaTable;
    if ((uint32)nValueIndex < POWER_FORMULA_TABLE_MAX_VALUES && nEntryIndex < tPowerFormulaTable.serPowerFormulaTableEntries.dwSize / sizeof(PowerFormulaTableEntry))
    {
#line 16007
        flValue = tPowerFormulaTable.ptPowerFormulaTableEntries[nEntryIndex].flValue[nValueIndex];
    }
#line 16016
    PowerFormulaTableUnlockR(ptGameBalDef);
#line 16019
    return flValue;
}

#line 106
static ActorCommonData * sGetCurrentItemPassivePowerHostItem()

{


    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    SNO snoPower = PowerGetSNO(idPower);
    if (!ptACD || snoPower == SNO_NONE)
        return NULL;



    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower);
    if (!ptACD->AttributesGetInt(tKey))
        return NULL;



    if (ACDIsHireling(*ptACD))
    {
        static const InventorySlotType sgePaperDollSlots[] = {
            SLOT_HIRELING_LEFT_HAND,
            SLOT_HIRELING_RIGHT_HAND,
            SLOT_HIRELING_SPECIAL,
            SLOT_HIRELING_LEFT_FINGER,
            SLOT_HIRELING_RIGHT_FINGER,
            SLOT_HIRELING_NECK,
            SLOT_HIRELING_HEAD,
            SLOT_HIRELING_TORSO,
            SLOT_HIRELING_HANDS,
            SLOT_HIRELING_WAIST,
            SLOT_HIRELING_FEET,
            SLOT_HIRELING_SHOULDERS,
            SLOT_HIRELING_LEGS,
            SLOT_HIRELING_BRACERS,
            SLOT_NONE
        };



        for (int32 nIndex = 0; sgePaperDollSlots[nIndex] != SLOT_NONE; ++nIndex)
        {

            InventoryLocation tInvLocSlot;
            ACDInventoryLocationSet(tInvLocSlot, ptACD->id, sgePaperDollSlots[nIndex], 0, 0);

            ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
            ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
            if (ptACDItemInSlot && ptACDItemInSlot->AttributesGetInt(tKey))
                return ptACDItemInSlot;


        }


    }
    else if (ACDIsPlayer(ptACD->id))
    {
        static const InventorySlotType sgePaperDollSlots[] = {
            SLOT_PLAYER_HEAD,
            SLOT_PLAYER_TORSO,
            SLOT_PLAYER_LEFT_HAND,
            SLOT_PLAYER_RIGHT_HAND,
            SLOT_PLAYER_HANDS,
            SLOT_PLAYER_WAIST,
            SLOT_PLAYER_FEET,
            SLOT_PLAYER_SHOULDERS,
            SLOT_PLAYER_LEGS,
            SLOT_PLAYER_BRACERS,
            SLOT_PLAYER_NECK,
            SLOT_PLAYER_LEFT_FINGER,
            SLOT_PLAYER_RIGHT_FINGER,
            SLOT_NONE
        };



        for (int32 nIndex = 0; sgePaperDollSlots[nIndex] != SLOT_NONE; ++nIndex)
        {

            InventoryLocation tInvLocSlot;
            ACDInventoryLocationSet(tInvLocSlot, ptACD->id, sgePaperDollSlots[nIndex], 0, 0);

            ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
            ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
            if (ptACDItemInSlot && ptACDItemInSlot->AttributesGetInt(tKey))
                return ptACDItemInSlot;


        }

    }


    return NULL;
}

#line 8056
int32 PowerGetItemLevel()

{
    ActorCommonData * ptACDItemSource = sGetCurrentItemPassivePowerHostItem();
    if (!ptACDItemSource)
        return 0;



    int32 nItemLevel = ItemGetItemLevel(ptACDItemSource->hGB.gbid);
    int32 nLegendaryItemLevel = ptACDItemSource->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
    return nLegendaryItemLevel ? nLegendaryItemLevel : nItemLevel;





}

#include "../SItems.h"

#line 15203
void PowerHostItemApplyTemporaryTransmog(
    LPCSTR szTransmogItemName)
{
    ActorCommonData * ptACDHostItem = sGetCurrentItemPassivePowerHostItem();
    if (!ptACDHostItem)
        return;
#line 15212
    ACDID idACDHostItemOwner = ptACDHostItem->GetInventoryContainerOwner();
    ActorCommonData * ptACDHostItemOwner = ACDTryToGet(idACDHostItemOwner);
    if (!ptACDHostItemOwner)
        return;
#line 15219
    GBID gbidTransmog = StringToGBID(szTransmogItemName);
    if (gbidTransmog == GBID_NONE)
        return;
#line 15225
    SItemApplyTemporaryTransmog(*ptACDHostItem, gbidTransmog, *ptACDHostItemOwner);
}

#line 15229
void PowerHostItemRemoveTemporaryTransmog()

{
    ActorCommonData * ptACDHostItem = sGetCurrentItemPassivePowerHostItem();
    if (!ptACDHostItem)
        return;
#line 15238
    ACDID idACDHostItemOwner = ptACDHostItem->GetInventoryContainerOwner();
    ActorCommonData * ptACDHostItemOwner = ACDTryToGet(idACDHostItemOwner);
    if (!ptACDHostItemOwner)
        return;
#line 15245
    SItemRemoveTemporaryTransmog(*ptACDHostItem, *ptACDHostItemOwner);
}

#line 15673
ACDID PowerFindNearestTrackedActorToActor(
    ACDID idACD)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    float flClosestDistSq = FLT_MAX;
    ACDID idACDClosest = ACDID_INVALID;

    ActorCommonData & tACD = ACDGet(idACD);
    const WorldPlace & tPlace = tACD.GetPlace();

    POS pos = ptPower->listACDsTracked.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTracked = (ACDID)ptPower->listACDsTracked.GetNext(pos);
        ActorCommonData * ptACDTracked = ACDTryToGet(idACDTracked);
        if (ptACDTracked)
        {

            float flDistSq = WorldPlaceDistanceSquared(tPlace, ptACDTracked->GetPlace());
            if (flDistSq < flClosestDistSq)
            {
                flClosestDistSq = flDistSq;
                idACDClosest = idACDTracked;
            }
        }
    }

    return idACDClosest;
}

#line 16461
void PowerActorSetLookSwitch(
    VACDID idActor,
    LPCSTR szLookSwitchName)
{
    uint32 dwLookOverrideHash = StringHashI(szLookSwitchName);

    if (dwLookOverrideHash)
    {
        ActorLookOverrideChangedMessage tOverrideMessage;
        tOverrideMessage.ann = ANNFromACD(idActor);
        tOverrideMessage.nChange = 1;
        tOverrideMessage.nLookOverrideHash = dwLookOverrideHash;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

        MessageSendToClient(listDestPlayers, SMSG_ACTOR_LOOK_OVERRIDE_CHANGED, &tOverrideMessage, sizeof(tOverrideMessage));
    }
}

#include "../../Client/Console/ControllerUtils.h"

#line 16024
void PowerCursorSetToLeftStick(
    int32 nCursorIndex)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorIndex, FALSE);
    if (!ptCursor)
        return;



    const ActorCommonData * ptACDOwner = ACDTryToGet(PowerGetOwner());
    if (!ptACDOwner)
        return;



    const Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
        return;



    Vector2D vStick = Console::ControllerUtils::GetLStickPosition(ptPlayer->nControllerIndex, TRUE);
    ptCursor->tPlace = ptACDOwner->GetPlace();
    ptCursor->qOrientation.FromYaw(vStick.ToYaw());

}

#line 16057
void PowerCursorSetToRightStick(
    int32 nCursorIndex)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorIndex, FALSE);
    if (!ptCursor)
        return;



    const ActorCommonData * ptACDOwner = ACDTryToGet(PowerGetOwner());
    if (!ptACDOwner)
        return;



    const Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
        return;



    Vector2D vStick = Console::ControllerUtils::GetRStickPosition(ptPlayer->nControllerIndex);
    ptCursor->tPlace = ptACDOwner->GetPlace();
    ptCursor->qOrientation.FromYaw(vStick.ToYaw());

}

#include "../../Common/GameTemplate.h"
#include "../LuaScript/ScriptUtil.h"
#include "../Loot/Loot.h"

#line 15006
void PowerHealthDropOnMonster(
    ACDID idDropper,
    LPCSTR szItemName)
{

    if (GameTypeIsPvP())
        return;



    ActorCommonData * ptACD = ACDTryToGet(idDropper);
    if (!ptACD)
        return;





    if (!ActorIsValid(idDropper))
        return;




    if (ptACD->tLocation.eActorLocation != AL_WORLD)
        return;




    if (ActorFlagIsSet((ActorID)ACDGetOwner(idDropper), ACTOR_DELETING_BIT))
        return;





    GBID gbidItem = GBID_NONE;
    if (szItemName)
    {
        gbidItem = StringToGBID(szItemName);
    }

    uint32 dwGameEventNumber = SGameGetEventNumber();
    LootDropHealth(idDropper, dwGameEventNumber, ACDID_INVALID, gbidItem);
}

#line 15372
BOOL PowerActorCanSpendResource(
    VACDID idACDActor,
    float flAmount,
    BOOL bPrimary,
    BOOL bDoResourceCostReduction)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (!ptACD ||
        ActorFlagIsSet((ActorID)ptACD->idOwner, ACTOR_GOD_MODE_BIT))
        return TRUE;



    ResourceType eResourceType = bPrimary ? ptACD->GetPrimaryResourceType() : ptACD->GetSecondaryResourceType();
    if (bDoResourceCostReduction)
    {
        float flCostReductionPercent = PowerGetCostReductionPercent(*ptACD, eResourceType, PowerGetDamageType(ptPower), ptPower->snoPower);
        flAmount *= flCostReductionPercent;

        float flCostReduction = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_TOTAL, eResourceType) + ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION, ptPower->snoPower));
        float flPowerCostReductionCoefficient = PowerGetFormulaValue(idPower, TAG_POWER_COST_RESOURCE_REDUCTION_COEFFICIENT);
        flAmount -= flCostReduction * flPowerCostReductionCoefficient;

        flAmount = Math::Max(flAmount, 0.0f);
    }

    float flCurResource = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eResourceType);

    return flCurResource >= flAmount;
}

// Condition order from the line table: flag test at 8742:9, the && at column 69.
#line 8733
int32 PowerBuffApplyToTargets(
    int32 nBuffID,
    int32 nTargetAreaID,
    BOOL bAllowInOnDoDamage)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);


    if ((ptPower->dwFlags & (1 << PF_DISALLOW_PAYLOAD_APPLICATION)) && !bAllowInOnDoDamage)
        return 0;




    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (!ptChooseTargets)
        return 0;



    int32 nAffected = 0;
    int32 nAffectedCap = PowerGetMaxAreaEffectTargetCount();

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(*ptChooseTargets, listTargets);

    PowerAPIBuffStartData * ptBuffStartData = sPowerGetBuffStartDataAtIndex(ptPower, nBuffID);
    if (ptBuffStartData)
    {


        POS pos = listTargets.GetHeadPosition();
        while (pos)
        {

            ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
            BuffID idBuff = PowerAPI_BuffAddToACD(*ptBuffStartData, ACDGet(idACDTarget));
            if (idBuff != BUFFID_INVALID)
            {
                ++nAffected;


                if (nAffected >= nAffectedCap)
                    break;
            }
        }













    }
#line 8793
    return nAffected;
}

#line 15324
void PowerActorSpendResource(
    VACDID idACDActor,
    float flAmount,
    BOOL bPrimary,
    BOOL bDoResourceCostReduction,
    BOOL bTriggerOnCostCallbacks,
    BOOL bDoHeal)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (!ptACD ||
        ActorFlagIsSet((ActorID)ptACD->idOwner, ACTOR_GOD_MODE_BIT))
        return;



    ResourceType eResourceType = bPrimary ? ptACD->GetPrimaryResourceType() : ptACD->GetSecondaryResourceType();
    if (bDoResourceCostReduction)
    {
        if (ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, ptPower->snoPower)) ||
            ptACD->AttributesGetInt(ATTRIBUTE_FREE_CAST_ALL))
            return;



        float flCostReductionPercent = PowerGetCostReductionPercent(*ptACD, eResourceType, PowerGetDamageType(ptPower), ptPower->snoPower);
        flAmount *= flCostReductionPercent;

        float flCostReduction = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_TOTAL, eResourceType) + ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION, ptPower->snoPower));
        float flPowerCostReductionCoefficient = PowerGetFormulaValue(idPower, TAG_POWER_COST_RESOURCE_REDUCTION_COEFFICIENT);
        flAmount -= flCostReduction * flPowerCostReductionCoefficient;

        flAmount = Math::Max(flAmount, 0.0f);
    }

    ptACD->SpendResource(flAmount, TRUE, eResourceType, ptPower, bDoHeal);



    if (bTriggerOnCostCallbacks)
    {
        PowerAPI_BuffApplyOnCostEffects(idACDActor, idPower, ptPower->snoPower, flAmount, eResourceType, 0.0f, TRUE, 0.0f);
    }
}


#line 15557
void PowerAddTrackedActors(
    int32 nTargetAreaID,
    BOOL bAddReference)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (!ptChooseTargets)
        return;



    int32 nAffected = 0;
    int32 nAffectedCap = PowerGetMaxAreaEffectTargetCount();

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(*ptChooseTargets, listTargets);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {

        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);

        if (bAddReference)
        {
            LuaScriptInstanceAddRefACD(LuaScriptGetExecutingScriptInstance(), idACDTarget);
        }

        if (!ptPower->listACDsTracked.Find(idACDTarget))
        {
            ptPower->listACDsTracked.AddTail_Tracked(idACDTarget);
            ++nAffected;

            if (nAffected >= nAffectedCap)
                break;
        }
    }


}

#line 14234
double PowerGetAttackDistanceToTarget(
    ACDID idACDTarget)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACDFrom = ACDTryToGet(ptPower->idACD);
    if (!ptACDFrom)
        return FLT_MAX;



    ActorCommonData * ptACDTo = ACDTryToGet(idACDTarget);
    if (!ptACDTo)
        return FLT_MAX;




    return PowerGetDistanceViaWorldPlace(
        ptACDFrom->GetPlace(), ptACDTo->GetPlace(),
        ptACDFrom->GetHeight(), ptACDFrom->GetRadius(),
        ptACDTo->GetHeight(), ptACDTo->GetRadius(),
        PowerAPI_ComputeMeleeReach(idPower, idACDTarget));
}

#include "../SGameStats.h"

#line 15407
void PowerActorHeal(
    ACDID idACDActor,
    float flAmount,
    HealType eHealType,
    ACDID idACDSource,
    BOOL bPlayEffect,
    int32 nNumberOfHeals)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACD = ACDTryToGet(idACDActor);
    if (!ptACD)
        return;



    float flMaxHP = ptACD->GetHitPointsMaxTotal();
    float flOldHP = ptACD->GetHitPointsCurrent();

    ActorHeal(idACDActor, flAmount, eHealType, ptPower->snoPower, idACDSource, bPlayEffect);

    float flNewHP = ptACD->GetHitPointsCurrent();

    if (ACDIsPlayer(idACDActor))
    {
        Player * ptPlayerHealed = PlayerGetByACD(idACDActor);
        Player * ptPlayerHealing = ACDGetControllingPlayer(ptPower->idACD);

        SGameStatsPlayerCounterIncrement(ptPlayerHealed, STAT_COUNTER_PLAYER_POWER_HEALING_RECEIVED, ptPower->snoPower, PowerGetRuneType(ptPower), flAmount);
        SGameStatsPlayerCounterIncrement(ptPlayerHealed, STAT_COUNTER_PLAYER_HEALING_RECEIVED_TOTAL, 0, 0, flAmount);


        if (ptPlayerHealing)
        {
            SGameStatsPlayerCounterIncrement(ptPlayerHealing, STAT_COUNTER_PLAYER_POWER_HEALING_DONE, ptPower->snoPower, PowerGetRuneType(ptPower), flAmount);
            SGameStatsPlayerCounterIncrement(ptPlayerHealing, STAT_COUNTER_PLAYER_HEALING_DONE_TOTAL, 0, 0, flAmount);
        }


        float flOverheal = 0.0f;
        if (flNewHP >= flMaxHP - EPSILON)
        {
            float flAmountHealed = flMaxHP - flOldHP;
            flOverheal = Math::Max(0.0f, flAmount - flAmountHealed);
        }

        PowerAPI_BuffApplyOnHealFromSkillEffects(idACDActor, idPower, ptPower->snoPower, flAmount, flOverheal, nNumberOfHeals);
    }

}

#line 14809
BOOL PowerActorIsFacingCursor(
    int32 nCursor,
    ACDID idActor,
    float flDegrees)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursor, TRUE);
    if (!ptCursor)
        return FALSE;



    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
        return FALSE;



    Vector2D vFacing = ptACD->GetForward().As2D();
    Vector2D vToCursor = ptCursor->tPlace.wp.As2D() - ptACD->GetPlace().wp.As2D();

    Angle aAngleBetween = Vector2D::AngleBetween(vFacing, vToCursor);
    float flBetweenDegrees = Math::AngleToDeg(aAngleBetween);


    if (flBetweenDegrees < 180.0f)
    {
        return flBetweenDegrees < flDegrees;
    }
    else
    {
        return flBetweenDegrees > 360.0 - flDegrees;
    }
}

#line 14132
double PowerCursorDistanceAlongCursorFacing(
    int32 nCursorIDFacing,
    int32 nCursorIDCheck)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorFacing = sPowerGetCursorAtIndex(ptPower, nCursorIDFacing, TRUE);
    PowerCursor * ptCursorCheck = sPowerGetCursorAtIndex(ptPower, nCursorIDCheck, TRUE);

    if (!ptCursorFacing || !ptCursorCheck ||
        ptCursorFacing->tPlace.idSWorld != ptCursorCheck->tPlace.idSWorld)
    {
        return FLT_MAX;
    }

    Vector3D vDir = ptCursorFacing->qOrientation.Rotate(cgwvXAxis);
    return Vector3D::DotProduct(vDir, ptCursorCheck->tPlace.wp - ptCursorFacing->tPlace.wp);
}

#line 14623
void PowerActorCopySkillsAndRunes(
    ACDID idACDSource,
    ACDID idACDTarget)
{



    const ActorCommonData & tACDSource = ACDGet(idACDSource);
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);


    AttribGroupID idFastAttrib = tACDSource.AttributesGetAttributeGroup();
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        FastAttribValue tValue = tIt.tEntryBuffer.tValue;
        if (KeyGetAttrib(tKey) != ATTRIBUTE_SKILL_TOTAL)
        {
            continue;
        }

        int32 nLevel = tValue.nValue;
        if (nLevel > 0)
        {
            SNO snoPower = KeyGetParam(tKey);


            tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), nLevel);


            int32 nRuneALevel = tACDSource.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower));
            int32 nRuneBLevel = tACDSource.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower));
            int32 nRuneCLevel = tACDSource.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower));
            int32 nRuneDLevel = tACDSource.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower));
            int32 nRuneELevel = tACDSource.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower));
            tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower), nRuneALevel);
            tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower), nRuneBLevel);
            tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower), nRuneCLevel);
            tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower), nRuneDLevel);
            tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower), nRuneELevel);


            tACDTarget.AttributesSetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, snoPower), TRUE);
        }
    }
}

int32 ExperienceGetPlayersEligibleForKill(ACDID idACDVictim, PlayerList & listPlayersToEarnXP);  // Experience.cpp:882; no header declaration in DWARF.

#line 14949
void PowerGoldDropOnMonster(
    ACDID idAttacker,
    ACDID idTarget,
    int32 nGoldToDrop)
{

    if (GameTypeIsPvP())
        return;



    ActorCommonData * ptACDTarget = ACDTryToGet(idTarget);
    if (!ptACDTarget)
        return;



    ActorCommonData * ptACDAttacker = ACDTryToGet(idAttacker);
    ACDID idACDGoldOwner = idAttacker;
    if (ptACDAttacker &&
        ptACDAttacker->IsPet())
    {
        idACDGoldOwner = ACDGetPetOwner(ptACDAttacker->id);
    }

    if (idACDGoldOwner == ACDID_INVALID)
    {


        PlayerList listPlayersToEarnXP(PlayerListGetElementPool());
        int32 nNumPlayers = ExperienceGetPlayersEligibleForKill(idTarget, listPlayersToEarnXP);
        if (nNumPlayers > 0)
        {
            POS pos = listPlayersToEarnXP.GetHeadPosition();
            while (pos)
            {
                Player * ptPlayer = listPlayersToEarnXP.GetNext(pos);
                LootDropExplicitGold(*ptACDTarget, ptPlayer->idPlayerACD, nGoldToDrop);
            }
        }
    }
    else
    {
        LootDropExplicitGold(*ptACDTarget, idACDGoldOwner, nGoldToDrop);
    }
}

extern void FlippiesCreateOnActor(const ActorCommonData & tACDSpawningActor, const ACDID idACDAssigned, const LootSpecifierArray & arLootSpecifiersToSpawn, const uint32 dwFlippyFlags, uint32 dwGameEventNumber, float flCameraBiasDist, const ACDID idACDKiller, const LooteeInfo * ptLootee, const Vector3D * pvForcedPositionBias, const LootContext eLootContext, XPooledDataIDList * plistSpawnedItemACDs);  // Flippies.cpp:958; no header declaration in DWARF.


#line 15055
void PowerDropArcaneGlobeOnActor(
    ACDID idDropper,
    ACDID idLootee)
{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (idDropper == ACDID_INVALID || !ptACDOwner)
        return;



    ActorCommonData * ptACDDropper = ACDTryToGet(idDropper);
    if (!ptACDDropper)
        return;




    GBID gbidItem = StringToGBID("ArcaneGlobe");
    if (gbidItem == GBID_NONE)
        return;





    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();


    tSpecifier.hGBItem.eType = GB_ITEMS;
    tSpecifier.hGBItem.gbid = gbidItem;


    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
        return;






    tSpecifier.eIQL = IQL_NORMAL; tSpecifier.nAffixLevel = 1;
    tSpecifier.fAutoIdentify = TRUE;

    uint32 dwGameEventNumber = SGameGetEventNumber();
    XPooledDataIDList listSpawnedItemACDs(MemoryPoolsGetDATAIDListPool());
    FlippiesCreateOnActor(*ptACDDropper, idLootee, arLootSpecifiersToSpawn, 0, dwGameEventNumber, 0.0f, idLootee, NULL, NULL, LC_UNKNOWN, &listSpawnedItemACDs);


    POS pos = listSpawnedItemACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDGlobe = (ACDID)listSpawnedItemACDs.GetNext(pos);
        ActorCommonData * ptACDGlobe = ACDTryToGet(idACDGlobe);
        if (!ptACDGlobe)
            continue;



        ptACDGlobe->AttributesSetInt(ATTRIBUTE_SPAWNED_BY_ACDID, ptACDOwner->id);
    }
}

#line 14261
ACDID PowerCloneMonsterOnCursor(
    VACDID idACDSource,
    int32 nCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (!ptCursor)
        return ACDID_INVALID;




    ActorCommonData & tACDSource = ACDGet(idACDSource);

    PowerAPISummonActor tSummonInfo;
    SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
    tSummonInfo.idPower = idPower;
    tSummonInfo.snoActor = tACDSource.snoActor;
    tSummonInfo.nActorLevel = tACDSource.AttributesGetInt(ATTRIBUTE_LEVEL);
    tSummonInfo.idACDCorpse = ACDID_INVALID;
    tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    tSummonInfo.placeSummonLocation = ptCursor->tPlace;
    tSummonInfo.qOrientation = ptCursor->qOrientation;


    tSummonInfo.eTeamOverride = TEAM_INVALID;
    tSummonInfo.bNoSpawnAnimPower = TRUE;

    ACDID idACDClone = PowerAPI_SummonActor(tSummonInfo);
    if (idACDClone != ACDID_INVALID)
    {
        ActorCommonData & tACDClone = ACDGet(idACDClone);


        float flHpPercent = tACDSource.GetHitpointPercent();
        float flHpMaxTotal = tACDSource.GetHitPointsMaxTotal();
        tACDClone.AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX, flHpMaxTotal);
        tACDClone.AttributesSetFloat(ATTRIBUTE_HITPOINTS_CUR, Math::Max(1.0f, flHpPercent * flHpMaxTotal));

        ActorLocationSetScale(tACDClone.tLocation, tACDSource.GetScale());
    }

    return idACDClone;
}

#line 14080
double PowerCursorDistanceToProjectedPointAlongLine(
    int32 nCursorIDLineOrigin,
    int32 nCursorIDLineEnd,
    int32 nCursorIDTarget)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorLineOrigin = sPowerGetCursorAtIndex(ptPower, nCursorIDLineOrigin, TRUE);
    PowerCursor * ptCursorLineEnd = sPowerGetCursorAtIndex(ptPower, nCursorIDLineEnd, TRUE);
    PowerCursor * ptCursorTarget = sPowerGetCursorAtIndex(ptPower, nCursorIDTarget, TRUE);

    if (!ptCursorLineOrigin || !ptCursorLineEnd || !ptCursorTarget)
        return FLT_MAX;



    SWorldID idLineOriginWorld = ptCursorLineOrigin->tPlace.idSWorld;
    SWorldID idLineEndWorld = ptCursorLineEnd->tPlace.idSWorld;
    SWorldID idTargetWorld = ptCursorTarget->tPlace.idSWorld;

    if (idLineOriginWorld != idLineEndWorld || idLineOriginWorld != idTargetWorld)
        return FLT_MAX;




    Vector3D wvTargetVector = ptCursorTarget->tPlace.wp - ptCursorLineOrigin->tPlace.wp;
    Vector3D wvLineVector = ptCursorLineEnd->tPlace.wp - ptCursorLineOrigin->tPlace.wp;

    wvTargetVector.z = 0.0f;
    wvLineVector.z = 0.0f;


    if (wvLineVector.Normalize() >= EPSILON)
    {

        float flDotProd = Vector3D::DotProduct(wvTargetVector, wvLineVector);
        wvLineVector *= flDotProd;

        Vector3D wvLineVectorProjPoint = wvLineVector + ptCursorLineOrigin->tPlace.wp;


        Vector3D wvLinePointToCursorTarget = ptCursorTarget->tPlace.wp - wvLineVectorProjPoint;
        wvLinePointToCursorTarget.z = 0.0f;

        return wvLinePointToCursorTarget.Magnitude();
    }

    return wvTargetVector.Magnitude();
}

#line 7500
void PowerBeamUpdate(
    ACDID idACDTarget,
    float flMaxLength,
    BOOL bUseRollToClickPositionOnGround,
    SNO snoPowerPassability)
{
    PowerID idPower = sGetCurrentPower();




    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDOwner || !ptACDTarget)
    {
        PowerInterrupt(idPower, TRUE);
        return;
    }

    Power * ptPower = PowerGet(idPower);

    if (!ptACDOwner->IsInWorld() || !ptACDTarget->IsInWorld())
    {
        PowerInterrupt(idPower, TRUE);
        return;
    }









    ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);
    Actor * ptActorTarget = ActorTryToGet(idActorTarget);
    if (ptActorTarget && !ptActorTarget->ptAI)
    {
        PowerInterrupt(idPower, TRUE);
        return;
    }









    const uint32 nCancelOutTime = 120;
    if (ptACDOwner->GetPlace().idSWorld != ptACDTarget->GetPlace().idSWorld ||
        ptPower->dwLastContactTick + nCancelOutTime < GameGetServerTime())
    {
        BrainAPIDoDeath(idActorTarget, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        PowerInterrupt(idPower, TRUE);
        return;
    }

    if (snoPowerPassability == SNO_NONE)
    {
        snoPowerPassability = ptPower->snoPower;
    }

    if (!sPowerBeamUpdate(idPower, idACDTarget, flMaxLength, bUseRollToClickPositionOnGround, snoPowerPassability))
    {
        BrainAPIDoDeath(idActorTarget, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        PowerInterrupt(idPower, TRUE);
    }




}

#include "../../Common/Powers/SpellEffectCommon.h"
#line 5602
ACDID PowerCreateProjectile(
    int32 nSourceCursorID,
    int32 nTargetCursorID,
    LPCSTR szProjectileName,
    float flProjectileSpeed,
    float flDistanceToTravel,
    BOOL bHugGround,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bWallBounce,
    BOOL bUseProjectileStaticFlagsForSpawn,
    BOOL bIgnoreMonsterCollision)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);


    if (flProjectileSpeed < EPSILON)
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptTargetCursor)
        return ACDID_INVALID;



    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(ptTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            flProjectileSpeed,
            tProjectile,
            flDistanceToTravel,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            bWallBounce,
            bIgnoreMonsterCollision);

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 5703
ACDID PowerCreateLobbedProjectileWithBounce(
    int32 nSourceCursorID,
    int32 nTargetCursorID,
    LPCSTR szProjectileName,
    float flGravity,
    float flHeightAboveSource,
    float flUnitBounceSpeed,
    float flLifetimeInSecs,
    ProjectileHitType eHitType,
    BOOL bDontDestroyOnImpact,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bUseProjectileStaticFlagsForSpawn)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);



    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptTargetCursor)
        return ACDID_INVALID;



    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(ptTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;

















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedLobbedProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            flGravity,
            flHeightAboveSource,
            flUnitBounceSpeed,
            flLifetimeInSecs,
            eHitType,
            bDontDestroyOnImpact,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            tProjectile);

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 5783
ACDID PowerCreateDeterministicProjectile(
    int32 nSourceCursorID,
    int32 nTargetCursorID,
    LPCSTR szProjectileName,
    float flProjectileSpeed,
    float flLifetime,
    int32 ePathType,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bHugGround,
    BOOL bWallBounce,
    BOOL bUseProjectileStaticFlagsForSpawn,
    BOOL bDontUsePowerPassability)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);



    if (flProjectileSpeed < EPSILON)
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptTargetCursor)
        return ACDID_INVALID;


    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(ptTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            flProjectileSpeed,
            tProjectile,
            0.0f,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            bWallBounce,
            FALSE);
        tProjectile.tProjectileData.eDPath = ePathType;
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flLifetime * TICKS_PER_SECOND);

        if (bDontUsePowerPassability)
        {
            tProjectile.tProjectileData.snoPower = SNO_NONE;
        }

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 5877
ACDID PowerCreateSinWaveProjectile(
    int32 nSourceCursorID,
    int32 nTargetCursorID,
    LPCSTR szProjectileName,
    float flProjectileSpeed,
    float flLifetime,
    float flOscillationSpeed,
    float flMaxWidth,
    float flLateralDistanceToScaleToMax,
    float flLateralDistanceToStartScale,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bHugGround,
    BOOL bUseProjectileStaticFlagsForSpawn,
    BOOL bIgnoreMonsterCollision)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);



    if (flProjectileSpeed < EPSILON)
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptTargetCursor)
        return ACDID_INVALID;


    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(ptTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            flProjectileSpeed,
            tProjectile,
            0.0f,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            FALSE,
            bIgnoreMonsterCollision);

        tProjectile.tProjectileData.eDPath = DPATH_SIN_WAVE;
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flLifetime * TICKS_PER_SECOND);
        tProjectile.tProjectileData.tDPathSinData.flSpeed = flProjectileSpeed;
        tProjectile.tProjectileData.tDPathSinData.flLateralMaxDistance = flMaxWidth;
        tProjectile.tProjectileData.tDPathSinData.annOwner = ANN_INVALID;

        float flRadPerSec = flOscillationSpeed * 2.0f * PI;
        float flRadPerTick = flRadPerSec * OO_TICKS_PER_SECOND;
        tProjectile.tProjectileData.tDPathSinData.flSinIncPerTick = flRadPerTick;

        if (flLateralDistanceToScaleToMax < EPSILON)
        {
            tProjectile.tProjectileData.tDPathSinData.flOOLateralDistanceToScale = 1.0f;
        }
        else
        {
            tProjectile.tProjectileData.tDPathSinData.flOOLateralDistanceToScale = 1.0f / flLateralDistanceToScaleToMax;
        }

        tProjectile.tProjectileData.tDPathSinData.flLateralStartDistance = flLateralDistanceToStartScale;

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 5987
ACDID PowerCreatePedalCurveProjectile(
    int32 nSourceCursorID,
    int32 nTargetCursorID,
    LPCSTR szProjectileName,
    float flOscillationSpeed,
    float flLifetime,
    float flMaxSize,
    float flHorizScale,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bHugGround,
    BOOL bUseProjectileStaticFlagsForSpawn)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);



    if (flOscillationSpeed < EPSILON)
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptTargetCursor)
        return ACDID_INVALID;


    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(ptTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            1.0f,
            tProjectile,
            0.0f,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            FALSE, FALSE);
        tProjectile.tProjectileData.eDPath = DPATH_PEDAL_CURVE;
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flLifetime * TICKS_PER_SECOND);
        tProjectile.tProjectileData.tDPathSinData.flSpeed = 1.0f;
        tProjectile.tProjectileData.tDPathSinData.flLateralStartDistance = flHorizScale;
        tProjectile.tProjectileData.tDPathSinData.flLateralMaxDistance = flMaxSize;
        tProjectile.tProjectileData.tDPathSinData.annOwner = ANN_INVALID;

        float flRadPerSec = flOscillationSpeed * 2.0f * PI;
        float flRadPerTick = flRadPerSec * OO_TICKS_PER_SECOND;
        tProjectile.tProjectileData.tDPathSinData.flSinIncPerTick = flRadPerTick;

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 6425
ACDID PowerCreateCurvedOffsetProjectile(
    int32 nSourceCursorID,
    int32 nTargetCursorID,
    LPCSTR szProjectileName,
    float flProjectileSpeed,
    float flLifetime,
    float flLateralOffset,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bHugGround,
    BOOL bUseProjectileStaticFlagsForSpawn)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);



    if (flProjectileSpeed < EPSILON)
        return ACDID_INVALID;






    if (Math::FloatEqual(flLateralOffset, 0.0f))
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptTargetCursor)
        return ACDID_INVALID;


    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(ptTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            flProjectileSpeed,
            tProjectile,
            0.0f,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            FALSE, FALSE);
        tProjectile.tProjectileData.eDPath = DPATH_CURVED_OFFSET;
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flLifetime * TICKS_PER_SECOND);
        tProjectile.tProjectileData.tDPathCurvedOffsetData.flLateralOffset = flLateralOffset;

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 6082
ACDID PowerCreateSpiralProjectile(
    int32 nSourceCursorID,
    int32 nTargetCursorID,
    LPCSTR szProjectileName,
    float flRotationSpeed,
    float flRotationAccel,
    float flLifetime,
    float flMaxSize,
    float flStartSize,
    float flSpiralGrowthSpeed,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bHugGround,
    BOOL bRotateAroundOwner,
    BOOL bUseProjectileStaticFlagsForSpawn)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);




    if (Math::FloatEqual(flRotationSpeed, 0.0f))
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptTargetCursor = sPowerGetCursorAtIndex(ptPower, nTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptTargetCursor)
        return ACDID_INVALID;


    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);
    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(ptTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            1.0f,
            tProjectile,
            0.0f,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            FALSE, FALSE);
        tProjectile.tProjectileData.eDPath = DPATH_SPIRAL;
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flLifetime * TICKS_PER_SECOND);
        tProjectile.tProjectileData.tDPathSinData.flSpeed = flSpiralGrowthSpeed;
        tProjectile.tProjectileData.tDPathSinData.flLateralMaxDistance = flMaxSize;
        tProjectile.tProjectileData.tDPathSinData.flLateralStartDistance = flStartSize;
        tProjectile.tProjectileData.tDPathSpiralData.placeSpiralCenter = ptTargetCursor->tPlace;
        tProjectile.tProjectileData.tDPathSinData.annOwner = ANN_INVALID;

        float flRadPerSec = flRotationSpeed * 2.0f * PI;
        float flRadPerTick = flRadPerSec * OO_TICKS_PER_SECOND;
        tProjectile.tProjectileData.tDPathSinData.flSinIncPerTick = flRadPerTick;

        float flRadPerSecAccel = flRotationAccel * 2.0f * PI;
        tProjectile.tProjectileData.tDPathSinData.flSinIncAccel = flRadPerSecAccel * OO_TICKS_PER_SECOND;

        if (bRotateAroundOwner)
        {
            ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
            tProjectile.tProjectileData.tDPathSinData.annOwner = ANNFromACD(ptACDOwner->id);
        }

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 6191
ACDID PowerCreateImmediateSpiralProjectile(
    int32 nSourceCursorID,
    int32 nSpiralCenterCursorID,
    LPCSTR szProjectileName,
    float flRotationSpeed,
    float flRotationAccel,
    float flLifetime,
    float flMaxSize,
    float flSpiralGrowthSpeed,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bHugGround,
    BOOL bRotateAroundOwner,
    BOOL bUseProjectileStaticFlagsForSpawn)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);




    if (Math::FloatEqual(flRotationSpeed, 0.0f))
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptSpiralCenterCursor = sPowerGetCursorAtIndex(ptPower, nSpiralCenterCursorID, TRUE);
    if (!ptSourceCursor || !ptSpiralCenterCursor ||
        ptSpiralCenterCursor->tPlace.idSWorld != ptSourceCursor->tPlace.idSWorld)
        return ACDID_INVALID;


    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);


    Vector3D vTangent = Vector3D::CrossProduct(ptSpiralCenterCursor->tPlace.wp - ptSourceCursor->tPlace.wp, cgwvZAxis);
    vTangent.Normalize();
    if (flRotationSpeed < 0.0f)
    {
        vTangent = -vTangent;
    }

    WorldPlace wpTarget = ptSourceCursor->tPlace;
    wpTarget.wp += vTangent;

    Target tTargetToShootAt;
    tTargetToShootAt.InitializeFromPlace(wpTarget);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        float flStartSize = Vector3D::Distance(tNavPlaceSource.tPlace.wp, ptSpiralCenterCursor->tPlace.wp);
        if ((flStartSize - EPSILON < flMaxSize) != (flSpiralGrowthSpeed >= 0.0f))
        {
            flSpiralGrowthSpeed = -flSpiralGrowthSpeed;
        }

        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tTargetToShootAt,
            SPELL_NONE,
            1.0f,
            tProjectile,
            0.0f,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            FALSE, FALSE);
        tProjectile.tProjectileData.eDPath = DPATH_SPIRAL_IMMEDIATE;
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flLifetime * TICKS_PER_SECOND);
        tProjectile.tProjectileData.tDPathSinData.flSpeed = flSpiralGrowthSpeed;
        tProjectile.tProjectileData.tDPathSinData.flLateralMaxDistance = flMaxSize;
        tProjectile.tProjectileData.tDPathSinData.flLateralStartDistance = flStartSize;
        tProjectile.tProjectileData.tDPathSpiralData.placeSpiralCenter = ptSpiralCenterCursor->tPlace;
        tProjectile.tProjectileData.tDPathSinData.annOwner = ANN_INVALID;

        float flRadPerSec = flRotationSpeed * 2.0f * PI;
        float flRadPerTick = flRadPerSec * OO_TICKS_PER_SECOND;
        tProjectile.tProjectileData.tDPathSinData.flSinIncPerTick = flRadPerTick;

        float flRadPerSecAccel = flRotationAccel * 2.0f * PI;
        tProjectile.tProjectileData.tDPathSinData.flSinIncAccel = flRadPerSecAccel * OO_TICKS_PER_SECOND;

        if (bRotateAroundOwner)
        {
            ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
            tProjectile.tProjectileData.tDPathSinData.annOwner = ANNFromACD(ptACDOwner->id);
        }

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 6318
ACDID PowerCreatePieWedgeProjectile(
    int32 nSourceCursorID,
    int32 nFirstTargetCursorID,
    int32 nSecondTargetCursorID,
    LPCSTR szProjectileName,
    float flStraightSpeed,
    float flArcSpeed,
    float flMaxLifetime,
    BOOL bHugGround,
    BOOL bDontPerformMarbleRollToCaster,
    BOOL bUseProjectileStaticFlagsForSpawn)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);



    if (flStraightSpeed < EPSILON || flArcSpeed < EPSILON)
        return ACDID_INVALID;






    PowerCursor * ptSourceCursor = sPowerGetCursorAtIndex(ptPower, nSourceCursorID, TRUE);
    PowerCursor * ptFirstTargetCursor = sPowerGetCursorAtIndex(ptPower, nFirstTargetCursorID, TRUE);
    PowerCursor * ptSecondTargetCursor = sPowerGetCursorAtIndex(ptPower, nSecondTargetCursorID, TRUE);
    if (!ptSourceCursor || !ptFirstTargetCursor || !ptSecondTargetCursor)
        return ACDID_INVALID;



    NavPlace tNavPlaceSource;
    tNavPlaceSource.tPlace = ptSourceCursor->tPlace;
    ScenesCommonNavMeshNodeFromPlace(ptSourceCursor->tPlace, &tNavPlaceSource.tNavMeshHandle, FALSE, FALSE);

    Target tFirstTarget;
    tFirstTarget.InitializeFromPlace(ptFirstTargetCursor->tPlace);

    SNO snoProjectile = SNOFileLookupGUID(ACTOR_SNO, szProjectileName);
    BOOL bExists = TRUE;


















    ptPower->snoProjectile = snoProjectile;

    ACDID idACDProjectile = ACDID_INVALID;

    if (snoProjectile != SNO_NONE)
    {
        WorldPlace placeTarget1 = ptFirstTargetCursor->tPlace;
        WorldPlace placeTarget2 = ptSecondTargetCursor->tPlace;
        if (!WorldPlaceInSameWorld(placeTarget1, placeTarget2))
            return ACDID_INVALID;







        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInitForScriptedStandardProjectile(
            idPower,
            ptPower->idACDAlternateSource,
            tNavPlaceSource,
            tFirstTarget,
            SPELL_NONE,
            1.0f,
            tProjectile,
            0.0f,
            bHugGround,
            bDontPerformMarbleRollToCaster,
            bUseProjectileStaticFlagsForSpawn,
            FALSE, FALSE);
        tProjectile.tProjectileData.eDPath = DPATH_PIE_WEDGE;
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatToInt(flMaxLifetime * TICKS_PER_SECOND);
        tProjectile.tProjectileData.tDPathPieData.vTargetPos2 = ptSecondTargetCursor->tPlace.wp;
        tProjectile.tProjectileData.tDPathPieData.flStraightSpeed = flStraightSpeed;
        tProjectile.tProjectileData.tDPathPieData.flArcSpeed = flArcSpeed;

        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        tProjectile.tProjectileData.tDPathPieData.annOwner = ANNFromACD(ptACDOwner->id);

        idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    }

    return idACDProjectile;
}

#line 2650
ActorIDArray * PowerGetProjectileTargets(int32 nTargetAreaID)

{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    const ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);



    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    if (!ptChooseTargets || !ptACDOwner)
    {
        ActorIDArray & tEmptyArrACDs = LuaScriptGetThreadSafeActorIDArray();
        tEmptyArrACDs.nNumActorIDs = 0;
        return &tEmptyArrACDs;
    }

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoFindAllPossibleTargets(*ptChooseTargets, listTargets);

    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;



    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listTargets.GetNext(pos);

        if (ACDIsProjectile(idACD) && idACD != ACDID_INVALID)
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(idACD);
            if (ptACDTarget && ptACDTarget->IsTargetable())
            {
                const ActorCommonData * ptACDProjectileOwner = ProjectileGetOwnerACD(idACD);
                BOOL bTeamsAreAllied = FALSE;
                if (ptACDProjectileOwner)
                {
                    bTeamsAreAllied = ACDIsFriendly(*ptACDOwner, *ptACDProjectileOwner, TRUE);
                }

                if (ProjectileIsReflected(idACD))
                {
                    bTeamsAreAllied = !bTeamsAreAllied;
                }

                if (((ptPower->tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_AFFECTS_ENEMIES)) && !bTeamsAreAllied) ||
                    ((ptPower->tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_AFFECTS_ALLIES)) && bTeamsAreAllied))
                {
                    tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACD;
                    ++tArrACDs.nNumActorIDs;
                }
            }
        }

        if (tArrACDs.nNumActorIDs == ACTOR_ID_ARRAY_MAX_ACTORS)
            break;
    }



    return &tArrACDs;
}

// Experience.cpp:840; no Experience.h declaration is present in the recovered DWARF.
extern ACDID ExperienceGetUltimateOwner(const ActorCommonData & tACD);
#line 2450
ActorIDArray * PowerGetNecroCorpsesAroundCursor(
    int32 nCursorID)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;

    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    const ActorCommonData * ptACDPowerOwner = PowerGetOwner(ptPower);

    if (ptACDPowerOwner)
    {
        ACDID idUltimateOwner = ExperienceGetUltimateOwner(*ptACDPowerOwner);
        ActorCommonData * ptACDUltimateOwner = ACDTryToGet(idUltimateOwner);
        if (ptACDUltimateOwner)
        {
            Player * ptPlayer = PlayerGetByACD(idUltimateOwner);
            if (ptPlayer)
            {
                int32 nMaxTargets = Math::Min(PowerGetMaxAreaEffectTargetCount(), (int32)ACTOR_ID_ARRAY_MAX_ACTORS);

                PowerCursor * ptCursorTo = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
                if (ptCursorTo)
                {
                    blz::vector<ACDID> listCorpses = ACDGetNecroCorpsesForPlayerAroundPosition(ptPlayer, ptCursorTo->tPlace.wp, ptPower->snoPower);

                    for (uint32 i = 0; i < nMaxTargets && i < listCorpses.size(); ++i)
                    {
                        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = listCorpses[i];
                        ++tArrACDs.nNumActorIDs;
                    }
                }
            }
        }
    }
    return &tArrACDs;
}
#line 2518
CRefString PowerReviveGetBestMonster(
    int32 nCursorID)
{
    CRefString sSnoName;
    float flMostHP = 0.0f;

    ActorIDArray * tCorpseList = PowerGetNecroCorpsesAroundCursor(nCursorID);
    for (int32 i = 0; i < tCorpseList->nNumActorIDs; ++i)
    {
        ACDID idCorpse = tCorpseList->arActorIDs[i];
        ActorCommonData * ptACDCorpse = ACDTryToGet(idCorpse);
        if (ptACDCorpse)
        {
            SNO snoMonster = ptACDCorpse->AttributesGetInt(ATTRIBUTE_NECROMANCER_CORPSE_SOURCE_MONSTER_SNO);
            if (snoMonster != SNO_NONE)
            {
                ScopedMonsterDefinitionLockR pdefMonster(snoMonster);


                const CTagMap * pTagMap = pdefMonster->hTagMap;


                SNO snoRevivedMonsterActor = TagMapGetSNO(pTagMap, TAG_MONSTER_NECRO_REVIVED_MONSTER_SNO);
                if (snoRevivedMonsterActor != SNO_NONE)
                {
                    ScopedActorDefinitionLockR pdefRevivedMonsterActor(snoRevivedMonsterActor);


                    ScopedMonsterDefinitionLockR pdefRevivedMonster(pdefRevivedMonsterActor->snoMonster);


                    if (flMostHP < pdefRevivedMonster->pctAttributeModifiers[GBA_HITPOINTS])
                    {
                        flMostHP = pdefRevivedMonster->pctAttributeModifiers[GBA_HITPOINTS];

                        const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, TAG_MONSTER_NECRO_REVIVED_MONSTER_SNO);
                        SNOGroup snoGroup = AxeTagTypeToSNOGroup(ptInfo->eType);
                        sSnoName = SNOToString(snoGroup, snoRevivedMonsterActor, TRUE);
                    }
                }
            }
        }
    }

    return sSnoName;
}
#line 16251
static int32 sGetNumberOfCursedEnemies(
    ACDID idPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
    if (ptACDPlayer)
    {
        return ptACDPlayer->AttributesGetInt(ATTRIBUTE_NECROMANCER_UNIQUE_CURSE_COUNT);
    }
    return 0;
}


#line 16264
int32 PowerGetNumberOfCursedEnemies(
    ACDID idPlayer)
{
    if (idPlayer != ACDID_INVALID)
    {
        if (!ACDIsPlayer(idPlayer))
        {
            return 0;
        }


        return sGetNumberOfCursedEnemies(idPlayer);
    }
    else
    {
        int32 nCount = 0;
        for (int32 i = 0; i < MAX_PLAYERS; ++i)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(i);
            if (ptPlayer)
            {
                nCount += sGetNumberOfCursedEnemies(ptPlayer->idPlayerACD);
            }
        }

        return nCount;
    }


}

#line 16206
static int32 sGetNumberOfCursedEnemiesLeech(
    ACDID idPlayer)
{
    static const FastAttribKey LEECH_COUNT_ATTRIBUTE_KEY = MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSE_COUNT, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_LEECH_PASSIVE_MANAGER));

    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
    if (ptACDPlayer)
    {
        return ptACDPlayer->AttributesGetInt(LEECH_COUNT_ATTRIBUTE_KEY);
    }
    return 0;
}

#line 16296
int32 PowerGetNumberOfCursedEnemiesLeech(
    ACDID idPlayer)
{
    if (idPlayer != ACDID_INVALID)
    {
        if (!ACDIsPlayer(idPlayer))
        {
            return 0;
        }


        return sGetNumberOfCursedEnemiesLeech(idPlayer);
    }
    else
    {
        int32 nCount = 0;
        for (int32 i = 0; i < MAX_PLAYERS; ++i)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(i);
            if (ptPlayer)
            {
                nCount += sGetNumberOfCursedEnemiesLeech(ptPlayer->idPlayerACD);
            }
        }

        return nCount;
    }


}

#line 16221
static int32 sGetNumberOfCursedEnemiesDecrepify(
    ACDID idPlayer)
{
    static const FastAttribKey DECREPIFY_COUNT_ATTRIBUTE_KEY = MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSE_COUNT, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_DECREPIFY_PASSIVE_MANAGER));

    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
    if (ptACDPlayer)
    {
        return ptACDPlayer->AttributesGetInt(DECREPIFY_COUNT_ATTRIBUTE_KEY);
    }
    return 0;
}

#line 16328
int32 PowerGetNumberOfCursedEnemiesDecrepify(
    ACDID idPlayer)
{
    if (idPlayer != ACDID_INVALID)
    {
        if (!ACDIsPlayer(idPlayer))
        {
            return 0;
        }


        return sGetNumberOfCursedEnemiesDecrepify(idPlayer);
    }
    else
    {
        int32 nCount = 0;
        for (int32 i = 0; i < MAX_PLAYERS; ++i)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(i);
            if (ptPlayer)
            {
                nCount += sGetNumberOfCursedEnemiesDecrepify(ptPlayer->idPlayerACD);
            }
        }

        return nCount;
    }


}

#line 16236
static int32 sGetNumberOfCursedEnemiesFrailty(
    ACDID idPlayer)
{
    static const FastAttribKey FRAILTY_COUNT_ATTRIBUTE_KEY = MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSE_COUNT, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_FRAILTY_PASSIVE_MANAGER));

    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
    if (ptACDPlayer)
    {
        return ptACDPlayer->AttributesGetInt(FRAILTY_COUNT_ATTRIBUTE_KEY);
    }
    return 0;
}

#line 16360
int32 PowerGetNumberOfCursedEnemiesFrailty(
    ACDID idPlayer)
{
    if (idPlayer != ACDID_INVALID)
    {
        if (!ACDIsPlayer(idPlayer))
        {
            return 0;
        }


        return sGetNumberOfCursedEnemiesFrailty(idPlayer);
    }
    else
    {
        int32 nCount = 0;
        for (int32 i = 0; i < MAX_PLAYERS; ++i)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(i);
            if (ptPlayer)
            {
                nCount += sGetNumberOfCursedEnemiesFrailty(ptPlayer->idPlayerACD);
            }
        }

        return nCount;
    }


}

// Raw CU 06e5fe74 records only Powers.cpp:5228/5307 for these overloads; no recovered header declarations.
float PowerGetMonsterFormulaValue(Power * ptPower, uint32 dwMonsterTag);
CRefString PowerGetMonsterSNONameFormulaValue(Power * ptPower, uint32 dwMonsterTag);
#line 15832 "X/Server/Powers/PowersSwig.cpp"
static uint32 sMonsterFormulaIndexToMonsterTag(
    MonsterFormulaIndex eFormulaIndex)
{
    switch (eFormulaIndex)
    {
    case 0: return TAG_MONSTER_GENERIC_PROJECTILE_COUNT;
    case 1: return TAG_MONSTER_GENERIC_PROJECTILE_SPEED;
    case 2: return TAG_MONSTER_GENERIC_PROJECTILE_SPREAD_ANGLE;
    case 3: return TAG_MONSTER_GENERIC_PROJECTILE_WAVE_COUNT;
    case 4: return TAG_MONSTER_GENERIC_PROJECTILE_WAVE_INTERVAL;
    case 5: return TAG_MONSTER_GENERIC_PROJECTILE_DAMAGE_TYPE;
    case 6: return TAG_MONSTER_GENERIC_PROJECTILE_DAMAGE_SCALAR;
    case 7: return TAG_MONSTER_GENERIC_PROJECTILE_TRAVEL_DISTANCE;
    case 8: return TAG_MONSTER_GENERIC_PROJECTILE2_COUNT;
    case 9: return TAG_MONSTER_GENERIC_PROJECTILE2_SPEED;
    case 10: return TAG_MONSTER_GENERIC_PROJECTILE2_SPREAD_ANGLE;
    case 11: return TAG_MONSTER_GENERIC_PROJECTILE2_WAVE_COUNT;
    case 12: return TAG_MONSTER_GENERIC_PROJECTILE2_WAVE_INTERVAL;
    case 13: return TAG_MONSTER_GENERIC_PROJECTILE2_DAMAGE_TYPE;
    case 14: return TAG_MONSTER_GENERIC_PROJECTILE2_DAMAGE_SCALAR;
    case 15: return TAG_MONSTER_GENERIC_PROJECTILE2_TRAVEL_DISTANCE;
    case 16: return TAG_MONSTER_GENERIC_PROJECTILE_COUNT_SECONDARY;
    case 17: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_DAMAGE_TYPE;
    case 18: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_DAMAGE_SCALAR;
    case 19: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_ANGLE;
    case 20: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_RADIUS;
    case 21: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_PULSE_COUNT;
    case 22: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_PULSE_INTERVAL;
    case 23: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_KNOCKBACK_MAGNITUDE;
    case 24: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_KNOCKBACK_HEIGHT;
    case 25: return TAG_MONSTER_GENERIC_PIE_SHOCKWAVE_EFFECT_GROUP;
    case 26: return TAG_MONSTER_GENERIC_PIE_BREATH_DAMAGE_TYPE;
    case 27: return TAG_MONSTER_GENERIC_PIE_BREATH_DAMAGE_SCALAR;
    case 28: return TAG_MONSTER_GENERIC_PIE_BREATH_ANGLE;
    case 29: return TAG_MONSTER_GENERIC_PIE_BREATH_RADIUS;
    case 30: return TAG_MONSTER_GENERIC_PIE_BREATH_PULSE_COUNT;
    case 31: return TAG_MONSTER_GENERIC_PIE_BREATH_PULSE_INTERVAL;
    case 32: return TAG_MONSTER_GENERIC_PIE_BREATH_EFFECT_GROUP;
    case 33: return TAG_MONSTER_GENERIC_AOE_NOVA_DAMAGE_TYPE;
    case 34: return TAG_MONSTER_GENERIC_AOE_NOVA_DAMAGE_SCALAR;
    case 35: return TAG_MONSTER_GENERIC_AOE_NOVA_RADIUS;
    case 36: return TAG_MONSTER_GENERIC_AOE_NOVA_EFFECT_GROUP;
    case 37: return TAG_MONSTER_GENERIC_AOE_NOVA_DOT_DAMAGE_SCALAR;
    case 38: return TAG_MONSTER_GENERIC_AOE_NOVA_DOT_DURATION;
    case 39: return TAG_MONSTER_GENERIC_AOE_NOVA_DOT_PROXY_ACTOR;
    case 40: return TAG_MONSTER_GENERIC_AOE_NOVA_MAX_CONCURRENT;
    case 41: return TAG_MONSTER_GENERIC_AOE_NOVA2_DAMAGE_TYPE;
    case 42: return TAG_MONSTER_GENERIC_AOE_NOVA2_DAMAGE_SCALAR;
    case 43: return TAG_MONSTER_GENERIC_AOE_NOVA2_RADIUS;
    case 44: return TAG_MONSTER_GENERIC_AOE_NOVA2_EFFECT_GROUP;
    case 45: return TAG_MONSTER_GENERIC_AOE_NOVA2_DOT_DAMAGE_SCALAR;
    case 46: return TAG_MONSTER_GENERIC_AOE_NOVA2_DOT_DURATION;
    case 47: return TAG_MONSTER_GENERIC_AOE_NOVA2_DOT_PROXY_ACTOR;
    case 48: return TAG_MONSTER_GENERIC_AOE_NOVA2_MAX_CONCURRENT;
    case 49: return TAG_MONSTER_GENERIC_AOE_TARGETED_DAMAGE_TYPE;
    case 50: return TAG_MONSTER_GENERIC_AOE_TARGETED_DAMAGE_SCALAR;
    case 51: return TAG_MONSTER_GENERIC_AOE_TARGETED_RADIUS;
    case 52: return TAG_MONSTER_GENERIC_AOE_TARGETED_EFFECT_GROUP;
    case 53: return TAG_MONSTER_GENERIC_AOE_TARGETED_DOT_DAMAGE_SCALAR;
    case 54: return TAG_MONSTER_GENERIC_AOE_TARGETED_DOT_DURATION;
    case 55: return TAG_MONSTER_GENERIC_AOE_TARGETED_DOT_PROXY_ACTOR;
    case 56: return TAG_MONSTER_GENERIC_AOE_TARGETED_MAX_CONCURRENT;
    case 57: return TAG_MONSTER_GENERIC_SUMMON_MONSTER_TYPE;
    case 58: return TAG_MONSTER_GENERIC_SUMMON_ROPE_EFFECT;
    case 59: return TAG_MONSTER_GENERIC_SUMMON_MIN_COUNT;
    case 60: return TAG_MONSTER_GENERIC_SUMMON_DELTA_COUNT;
    case 61: return TAG_MONSTER_GENERIC_SUMMON_DURATION;
    case 62: return TAG_MONSTER_GENERIC_SUMMON_MAX_TOTAL_SUMMONS;
    case 63: return TAG_MONSTER_GENERIC_SUMMON_MAX_CONCURRENT_SUMMONS;
    case 64: return TAG_MONSTER_GENERIC_SUMMON_KILL_SUMMONS_ON_DEATH;
    case 65: return TAG_MONSTER_GENERIC_SUMMON_ATTRACTOR_EFFECT;
    case 66: return TAG_MONSTER_GENERIC_SUMMON_SPAWN_DELAY;
    case 67: return TAG_MONSTER_GENERIC_SUMMON_SPAWN_EFFECT;
    case 68: return TAG_MONSTER_GENERIC_SUMMON2_MONSTER_TYPE;
    case 69: return TAG_MONSTER_GENERIC_SUMMON2_ROPE_EFFECT;
    case 70: return TAG_MONSTER_GENERIC_SUMMON2_MIN_COUNT;
    case 71: return TAG_MONSTER_GENERIC_SUMMON2_DELTA_COUNT;
    case 72: return TAG_MONSTER_GENERIC_SUMMON2_DURATION;
    case 73: return TAG_MONSTER_GENERIC_SUMMON2_MAX_TOTAL_SUMMONS;
    case 74: return TAG_MONSTER_GENERIC_SUMMON2_MAX_CONCURRENT_SUMMONS;
    case 75: return TAG_MONSTER_GENERIC_SUMMON2_KILL_SUMMONS_ON_DEATH;
    case 76: return TAG_MONSTER_GENERIC_SUMMON2_ATTRACTOR_EFFECT;
    case 77: return TAG_MONSTER_GENERIC_SUMMON2_SPAWN_DELAY;
    case 78: return TAG_MONSTER_GENERIC_SUMMON2_SPAWN_EFFECT;
    case 79: return TAG_MONSTER_GENERIC_AOE_RANDOM_DAMAGE_TYPE;
    case 80: return TAG_MONSTER_GENERIC_AOE_RANDOM_DAMAGE_SCALAR;
    case 81: return TAG_MONSTER_GENERIC_AOE_RANDOM_RADIUS;
    case 82: return TAG_MONSTER_GENERIC_AOE_RANDOM_EFFECT_GROUP;
    case 83: return TAG_MONSTER_GENERIC_AOE_RANDOM_DOT_DAMAGE_SCALAR;
    case 84: return TAG_MONSTER_GENERIC_AOE_RANDOM_DOT_DURATION;
    case 85: return TAG_MONSTER_GENERIC_AOE_RANDOM_DOT_PROXY_ACTOR;
    case 86: return TAG_MONSTER_GENERIC_AOE_RANDOM_SPAWN_RANGE_MIN;
    case 87: return TAG_MONSTER_GENERIC_AOE_RANDOM_SPAWN_RANGE_DELTA;
    case 88: return TAG_MONSTER_GENERIC_AOE_RANDOM_SPAWN_COUNT_MIN;
    case 89: return TAG_MONSTER_GENERIC_AOE_RANDOM_SPAWN_COUNT_DELTA;
    case 90: return TAG_MONSTER_GENERIC_AOE_RANDOM_SPAWN_INTERVAL_MIN;
    case 91: return TAG_MONSTER_GENERIC_AOE_RANDOM_SPAWN_INTERVAL_DELTA;
    case 92: return TAG_MONSTER_GENERIC_AOE_RANDOM_MAX_CONCURRENT;













    default: return 0;
    }
}


float PowerGetMonsterFormulaValue(
    MonsterFormulaIndex eFormulaIndex)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (ActorGetType(SNOName(ACTOR_SNO, ptPower->snoCasterActor)) == ACTORTYPE_INVALID)
    {




        return 0.0f;
    }

    return PowerGetMonsterFormulaValue(ptPower, sMonsterFormulaIndexToMonsterTag(eFormulaIndex));
}

CRefString PowerGetMonsterSNONameFormulaValue(
    MonsterFormulaIndex eFormulaIndex)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (ActorGetType(SNOName(ACTOR_SNO, ptPower->snoCasterActor)) == ACTORTYPE_INVALID)
    {




        return CRefString("");
    }

    return PowerGetMonsterSNONameFormulaValue(ptPower, sMonsterFormulaIndexToMonsterTag(eFormulaIndex));
}

// Raw CU 06e0e329 records only PowerHelper.cpp:332; no recovered header declaration.
void PowerHelper_AttackPayloadInit_Scripted(PowerID idPower, PowerAPIAttackPayload & tPayload, const ActorCommonData * ptACDToUse, float flBaseDamageScalar, BOOL bForcedPhysical, BOOL bForceUseACDAttributes);

#line 3127 "X/Server/Powers/PowersSwig.cpp"
int32 PowerCreatePayloadNew(
    PayloadDeliveryMechanism eDeliveryMechanism,
    float flBaseDamageScalar,
    BOOL bForcedPhysical,
    ACDID idACDToUse)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);


    const ActorCommonData * ptACDToUse;
    BOOL bForceUseACDAttributes = FALSE;
    if (idACDToUse != ACDID_INVALID)
    {
        ptACDToUse = ACDTryToGet(idACDToUse);
        if (ptACDToUse)
            bForceUseACDAttributes = TRUE;
    }
    else
    {


        ptACDToUse = PowerGetOwner(idPower);
    }

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Scripted(idPower, tPayload, ptACDToUse, flBaseDamageScalar, bForcedPhysical, bForceUseACDAttributes);

    tPayload.idPower = idPower;
    tPayload.eDeliveryMechanism = eDeliveryMechanism;
    tPayload.eAttackDirection = ATTACKDIRECTION_STRAIGHT;
    tPayload.snoTargetImpactParticle = SNO_NONE;
    tPayload.snoTargetImpactEffectGroup = SNO_NONE;
    tPayload.snoTargetAttackedParticle = SNO_NONE;
    tPayload.eDeathReplacementType = DEATH_REPLACEMENT_NONE;  // the original stores 0 here (mov w8, #-1 then a 64-bit store over dc/e0).
    tPayload.eKnockBack = KNOCKBACK_NONE;

    tPayload.idACDFloatingCombatTextSourceOverride = ACDID_INVALID;

    const ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);
    if (ptACDOwner && ptACDOwner->AttributesGetInt(ATTRIBUTE_SUMMONED_BY_AUTOCAST))
    {
        tPayload.flProcScalar = 0.0f;
        tPayload.bTriggersHitProcs = FALSE;
    }












    int32 nIndex = ptPower->listPayloads.Count();
    ptPower->listPayloads.AddTail_Tracked(tPayload);

    if (nIndex >= MAX_PAYLOADS_PER_POWER)
    {
        CRefString sError;
        sError.Format("PowerCreatePayloadNew: PowerSNO (%s) Power ID (ID - %d) has exceeded the max number of instantiated Payloads per power (%d)! Likely the script forgot to call Hydra.PowerPayloadDeleteLastCreated() and is leaking payloads!\n", SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str(), idPower, MAX_PAYLOADS_PER_POWER);

    }

    return nIndex;
}

#line 5472 "X/Server/Powers/PowersSwig.cpp"
void PowerCursorGetAverageCursorPosition(
    int32 nCursorIDOut,
    int32 nCursorID1,
    int32 nCursorID2,
    int32 nCursorID3,
    int32 nCursorID4,
    int32 nCursorID5)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursorOut = sPowerGetCursorAtIndex(ptPower, nCursorIDOut, FALSE);
    PowerCursor * ptCursorStart = sPowerGetCursorAtIndex(ptPower, nCursorID1, TRUE);
    if (!ptCursorOut || !ptCursorStart)
        return;



    WorldPlace tAveragePlace = ptCursorStart->tPlace;
    int32 nCursorsAveraged = 1;

    int32 arCursorsToAverage[5] = { nCursorID1, nCursorID2, nCursorID3, nCursorID4, nCursorID5 };
    for (int32 i = 1; i < 5; ++i)
    {
        PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, arCursorsToAverage[i], TRUE);
        if (!ptCursor || ptCursor->tPlace.idSWorld != tAveragePlace.idSWorld)
            continue;



        ++nCursorsAveraged;
        tAveragePlace.wp += (ptCursor->tPlace.wp - tAveragePlace.wp) * (1.0f / nCursorsAveraged);
    }

    ptCursorOut->tPlace = tAveragePlace;
}

#include "../../Common/WorldCommonCollision.h"
// Raw CU 051890b2 records only PowersCommon.cpp:7737; no recovered header declaration.
BOOL PowerFillCollParams(ACDID idACD, AutoPowerDefinitionR pdefPower, PowerPassabilityType ePassability, WorldCollParams & tCollParams);
// Raw CU 050c85aa records only NavZoneIntersect.cpp:2173; no recovered header declaration.
BOOL NavZoneCollideSweptPoint3D(SWorldID idSWorld, const Vector3D & wpStart, const Vector3D & wvDirection, uint16 uOneRequiredFlag, uint16 uPreventFlags, uint32 uCollFlags, float & flISectT, Vector3D & wpCollision);
#line 4519 "X/Server/Powers/PowersSwig.cpp"
BOOL PowerCursorCheckForNavMeshIntersectionBetweenCursors(
    int32 nCursorID,
    int32 nDestCursorID,
    int32 nIntersectionCursorID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    PowerCursor * ptDestCursor = sPowerGetCursorAtIndex(ptPower, nDestCursorID, TRUE);
    PowerCursor * ptIntersectionCursor = sPowerGetCursorAtIndex(ptPower, nIntersectionCursorID, TRUE);

    if (!ptCursor || !ptDestCursor || !ptIntersectionCursor)
        return FALSE;



    ptCursor->qOrientation = ptDestCursor->qOrientation;

    const ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
        return FALSE;







    const ActorMovement * ptACM = ptACD->GetActorMovement();
    if (!ptACM)
        return FALSE;







    Vector3D vecToDest = ptDestCursor->tPlace.wp - ptCursor->tPlace.wp;


    WorldCollParams tCollParams;
    BOOL bHasFlags = PowerFillCollParams(ptACD->id, AutoPowerDefinitionR(ptPower->snoPower), POWER_PASSABILITY_DURING, tCollParams);
    if (!bHasFlags)
        return FALSE;







    Vector3D vecCollision;
    float flT;
    if (NavZoneCollideSweptPoint3D(ptCursor->tPlace.idSWorld, ptCursor->tPlace.wp, vecToDest, tCollParams.uOneRequiredStaticFlags, tCollParams.uStaticPreventFlags, 0, flT, vecCollision))
    {
        ptIntersectionCursor->tPlace.wp = vecCollision;
        return TRUE;
    }

    return FALSE;
}

#line 15716 "X/Server/Powers/PowersSwig.cpp"
void PowerUpdateSnapshotAttributes()

{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        const uint32 SNAPSHOT_COOLDOWN_TIME_FRAMES = 60;

        uint32 dwNow = GameGetServerTime();
        uint32 dwLastSnapshotTime = FastAttribGetValueInt(ptPower->idFastAttrib, ATTRIBUTE_POWER_LAST_ATTRIBUTE_SNAPSHOT_UPDATE_TIME);
        if (dwLastSnapshotTime + SNAPSHOT_COOLDOWN_TIME_FRAMES < dwNow)
        {
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_STEAL_HEALTH_PERCENT, ptACD->AttributesGetFloat(ATTRIBUTE_STEAL_HEALTH_PERCENT));
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_STEAL_MANA_PERCENT, ptACD->AttributesGetFloat(ATTRIBUTE_STEAL_MANA_PERCENT));

            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_HITPOINTS_ON_HIT, ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_ON_HIT));
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_STEAL_HEALTH_PERCENT, ptACD->AttributesGetFloat(ATTRIBUTE_STEAL_HEALTH_PERCENT));

            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_CRIT_DAMAGE_PERCENT, ptACD->AttributesGetFloat(ATTRIBUTE_CRIT_DAMAGE_PERCENT));
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED, ptACD->AttributesGetFloat(ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED));

            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_STRENGTH_TOTAL, ptACD->AttributesGetFloat(ATTRIBUTE_STRENGTH_TOTAL));
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_DEXTERITY_TOTAL, ptACD->AttributesGetFloat(ATTRIBUTE_DEXTERITY_TOTAL));
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_INTELLIGENCE_TOTAL, ptACD->AttributesGetFloat(ATTRIBUTE_INTELLIGENCE_TOTAL));
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_VITALITY_TOTAL, ptACD->AttributesGetFloat(ATTRIBUTE_VITALITY_TOTAL));

            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS));
            FastAttribSetValueFloat(ptPower->idFastAttrib, ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT, ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT));

            for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType)
            {
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN_TOTAL, nDamageType),
                    ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_MIN_TOTAL, nDamageType)));
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA_TOTAL, nDamageType),
                    ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA_TOTAL, nDamageType)));
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, nDamageType),
                    ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, nDamageType)));
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, nDamageType),
                    ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DEALT_PERCENT_BONUS, nDamageType)));
            }

            for (int32 i = 0; i < RESOURCE_TYPE_COUNT; ++i)
            {
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, i), ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT, i)));
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT_BONUS_PCT, i), ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT_BONUS_PCT, i)));
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT_BONUS, i), ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_ON_HIT_BONUS, i)));
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, i), ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_ON_KILL, i)));
                FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, i), ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_ON_CRIT, i)));
            }

            FastAttribSetValueFloat(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_INSTANCE_DAMAGE_PERCENT_BONUS, idPower), ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_INSTANCE_DAMAGE_PERCENT_BONUS, idPower)));
            FastAttribSetValueInt(ptPower->idFastAttrib, ATTRIBUTE_POWER_LAST_ATTRIBUTE_SNAPSHOT_UPDATE_TIME, dwNow);
        }
    }
}

#line 2339 "X/Server/Powers/PowersSwig.cpp"
static void sPowerGetTargetsSortedByDistanceInsertSortedTarget(
    XPooledACDSortedTargetList & listSortedACDs,
    ACDSortedTarget & tTargetToInsert)
{
    POS pos = listSortedACDs.GetHeadPosition();
    while (pos)
    {
        POS oldpos = pos;
        const ACDSortedTarget & tCurrTarget = listSortedACDs.GetNext(pos);
        if (tCurrTarget.flDistance > tTargetToInsert.flDistance)
        {
            listSortedACDs.InsertBefore_Tracked(oldpos, tTargetToInsert);
            return;
        }
    }

    listSortedACDs.AddTail_Tracked(tTargetToInsert);
}

ActorIDArray * PowerGetTargetsSortedByDistance(
    int32 nTargetAreaID)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);


    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    WorldPlace placeCenter = cgplaceNull;

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);

    if (!ptChooseTargets || (!ptACDOwner && ptChooseTargets->tGeometry.eType == CTT_MELEE))
    {
        ActorIDArray & tEmptyArrACDs = LuaScriptGetThreadSafeActorIDArray();
        tEmptyArrACDs.nNumActorIDs = 0;
        return &tEmptyArrACDs;
    }

    switch (ptChooseTargets->tGeometry.eType)
    {
    case CTT_SPHERE: case CTT_NO_LINE_OF_SIGHT: case CTT_FRONT: case CTT_BACK:
        placeCenter = ptChooseTargets->tGeometry.tSphere.placeCenter;
        break;
    case CTT_SINGLE_TARGET:
        placeCenter = ptChooseTargets->tGeometry.tSingleTarget.placeSource;
        break;
    case CTT_RAY:
        placeCenter = ptChooseTargets->tGeometry.tRay.placeStart;
        break;
    case CTT_MELEE:
        placeCenter = ptACDOwner->GetPlace();
        break;





    case CTT_PIE:
        placeCenter = ptChooseTargets->tGeometry.tPie.placeCenter;
        break;
    case CTT_CAPSULE:
        placeCenter = ptChooseTargets->tGeometry.tCapsule.place0;
        break;
    case CTT_BOX:
        placeCenter = ptChooseTargets->tGeometry.tBox.placeCenter;
        break;
    }




    ActorIDArray * ptArrACDs = PowerGetTargets(nTargetAreaID);

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

    XPooledACDSortedTargetList listSortedACDs(&ptPowerGlobals->tACDSortedTargetPool);

    for (int32 i = 0; i < ptArrACDs->nNumActorIDs; ++i)
    {
        ACDID idACDCurrent = ptArrACDs->arActorIDs[i];

        WorldPlace placeCurrent = ACDGet(idACDCurrent).GetPlace();
        float flDistance = FLT_MAX;

        if (placeCurrent.idSWorld == placeCenter.idSWorld)
        {
            flDistance = Vector3D::DistanceSquared(placeCurrent.wp, placeCenter.wp);
        }

        ACDSortedTarget tSortedTarget;
        tSortedTarget.idACD = idACDCurrent;
        tSortedTarget.flDistance = flDistance;

        sPowerGetTargetsSortedByDistanceInsertSortedTarget(listSortedACDs, tSortedTarget);
    }



    POS pos = listSortedACDs.GetHeadPosition();
    int32 nSortedActorIndex = 0;
    while (pos)
    {
        const ACDSortedTarget & tCurrTarget = listSortedACDs.GetNext(pos);
        ptArrACDs->arActorIDs[nSortedActorIndex++] = tCurrTarget.idACD;
    }


    return ptArrACDs;
}

#include "../Achievements/AchievementModifierEvaluationLogic.h"
#line 11492 "X/Server/Powers/PowersSwig.cpp"
float PowerBuffDamageShieldAbsorb(
    float flAbsorbAmount,
    BOOL bWantAbsorbedMessage)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    float flAmountLeft = flAbsorbAmount;
    float flAmountAbsorbed = 0.0f;

    if (ptPower->listDamage.Count() != 0)
    {





        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone)
        {
            for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; ++iDamageType)
            {
                if (flDamageDone[iDamageType] > EPSILON)
                {

                    if (flDamageDone[iDamageType] < flAmountLeft)
                    {
                        flAmountLeft -= flDamageDone[iDamageType];
                        flAmountAbsorbed += flDamageDone[iDamageType];
                        flDamageDone[iDamageType] = 0.0f;
                    }
                    else
                    {
                        flAmountAbsorbed += flAmountLeft;
                        flDamageDone[iDamageType] -= flAmountLeft;
                        flAmountLeft = 0.0f;
                    }
                }
            }
        }

        if (bWantAbsorbedMessage && ptPower->listPowerAPIDamage.Count() != 0)
        {






            PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
            Player * ptPlayerAttacker = ACDGetControllingPlayer(ptPowerAPIDamage->idACDSource);
            FloatingNumberMessage tMessage;
            tMessage.annTarget = ANNFromACD(ptPowerAPIDamage->idACDTarget);
            tMessage.flDamage = flAmountAbsorbed;
            tMessage.eType = FLOATING_NUMBER_TYPE_ABSORB;

            Player * ptPlayer = ACDGetControllingPlayer(ptPowerAPIDamage->idACDTarget);
            PlayerList listDestPlayers(PlayerListGetElementPool());
            if (ptPlayer)
            {
                listDestPlayers.AddTail_Tracked(ptPlayer);
            }
            if (ptPlayerAttacker)
            {
                listDestPlayers.AddTail_Tracked(ptPlayerAttacker);
            }
            if (listDestPlayers.Count() > 0)
            {
                MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
            }





            if (ptPlayer)
            {
                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.SetPlayer(ptPlayer);

                tEvaluationData.EvaluateEvent(
                    ptPlayer,
                    Achievement::ACH_EVENT_PLAYER_ABSORBED_DAMAGE_WITH_POWER,
                    ptPower->snoPower,
                    Math::FloatToInt(flAmountAbsorbed), (uint32)sub_71007b5e70());
            }

        }
    }



    return flAmountLeft;
}

#line 11587 "X/Server/Powers/PowersSwig.cpp"
float PowerBuffDamageShieldAbsorbFromBuff(
    BOOL bWantAbsorbedMessage)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    if (!ptPower || ptPower->listActiveCallbackBuffs.IsEmpty())
    {
        return 0.0f;
    }


    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(ptPower->listActiveCallbackBuffs.GetHead());
    if (!ptBuff || ptBuff->bDeleted)
    {
        return 0.0f;
    }


    float flAmountLeftOnBuff = ptBuff->flShieldAmount;
    float flAmountAbsorbed = 0.0f;

    if (ptPower->listDamage.Count() != 0)
    {



        if (flAmountLeftOnBuff <= EPSILON)
        {
            return 0.0f;
        }


        float * flDamageDone = ptPower->listDamage.GetHead();
        if (flDamageDone)
        {
            for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; ++iDamageType)
            {
                if (flDamageDone[iDamageType] > EPSILON)
                {

                    if (flDamageDone[iDamageType] < flAmountLeftOnBuff)
                    {
                        flAmountLeftOnBuff -= flDamageDone[iDamageType];
                        flAmountAbsorbed += flDamageDone[iDamageType];
                        flDamageDone[iDamageType] = 0.0f;
                    }
                    else
                    {
                        flAmountAbsorbed += flAmountLeftOnBuff;
                        flDamageDone[iDamageType] -= flAmountLeftOnBuff;
                        flAmountLeftOnBuff = 0.0f;
                    }
                }
            }
        }

        if (bWantAbsorbedMessage && ptPower->listPowerAPIDamage.Count() != 0)
        {






            PowerAPIDamage * ptPowerAPIDamage = ptPower->listPowerAPIDamage.GetHead();
            Player * ptPlayerAttacker = ACDGetControllingPlayer(ptPowerAPIDamage->idACDSource);
            FloatingNumberMessage tMessage;
            tMessage.annTarget = ANNFromACD(ptPowerAPIDamage->idACDTarget);
            tMessage.flDamage = flAmountAbsorbed;
            tMessage.eType = FLOATING_NUMBER_TYPE_ABSORB;

            Player * ptPlayer = ACDGetControllingPlayer(ptPowerAPIDamage->idACDTarget);
            PlayerList listDestPlayers(PlayerListGetElementPool());
            if (ptPlayer)
            {
                listDestPlayers.AddTail_Tracked(ptPlayer);
            }
            if (ptPlayerAttacker)
            {
                listDestPlayers.AddTail_Tracked(ptPlayerAttacker);
            }
            if (listDestPlayers.Count() > 0)
            {
                MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
            }





            if (ptPlayer)
            {
                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.SetPlayer(ptPlayer);

                tEvaluationData.EvaluateEvent(
                    ptPlayer,
                    Achievement::ACH_EVENT_PLAYER_ABSORBED_DAMAGE_WITH_POWER,
                    ptPower->snoPower,
                    Math::FloatToInt(flAmountAbsorbed), (uint32)sub_71007b5e70());
            }

        }


        ptBuff->flShieldAmount = flAmountLeftOnBuff;
    }
    return flAmountLeftOnBuff;
}

#line 9359 "X/Server/Powers/PowersSwig.cpp"
static void sPreventSkillOverrideBuffAddPowerSNOAttribute(
    PowerAPIBuffStartData & tBuffStartData,
    const SNO snoHotbarOverridePower)
{
    if (snoHotbarOverridePower != SNO_NONE)
    {
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE_STARTED_OR_ENDED, snoHotbarOverridePower), TRUE);
    }
}

static const float SKILL_OVERRIDE_STARTED_OR_ENDED_BUFF_DURATION = 1.0f;

void PowerPreventSkillOverridePowerCastErrors(
    LPCSTR szHotbarSkill1,
    LPCSTR szHotbarSkill2,
    LPCSTR szHotbarSkill3,
    LPCSTR szHotbarSkill4,
    LPCSTR szHotbarSkill5,
    LPCSTR szLMBSkill,
    LPCSTR szRMBSkill)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    SNO snoHotbarSkill1 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill1);
    SNO snoHotbarSkill2 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill2);
    SNO snoHotbarSkill3 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill3);
    SNO snoHotbarSkill4 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill4);
    SNO snoHotbarSkill5 = SNOFileLookupGUID(POWER_SNO, szHotbarSkill5);
    SNO snoLMBSkill = SNOFileLookupGUID(POWER_SNO, szLMBSkill);
    SNO snoRMBSkill = SNOFileLookupGUID(POWER_SNO, szRMBSkill);


    LuaVerifySnosAreReferenced(snoHotbarSkill1, snoHotbarSkill2, snoHotbarSkill3, snoHotbarSkill4, snoHotbarSkill5, snoLMBSkill, snoRMBSkill);








    SNO snoOverridePower = GlobalSNOGet(GLOBALSNO_POWERS_SKILL_OVERRIDE_STARTED_OR_ENDED);

    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.snoPower = snoOverridePower;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.bRemoveOnDeath = FALSE;
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.dwDurationMin = Math::FloatToInt(SKILL_OVERRIDE_STARTED_OR_ENDED_BUFF_DURATION * TICKS_PER_SECOND);
    tBuffStartData.dwDurationDelta = 0;

    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_SKILL_OVERRIDE_STARTED_OR_ENDED_ACTIVE, TRUE);

    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, ptPower->snoPower);
    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, snoHotbarSkill1);
    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, snoHotbarSkill2);
    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, snoHotbarSkill3);
    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, snoHotbarSkill4);
    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, snoHotbarSkill5);
    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, snoLMBSkill);
    sPreventSkillOverrideBuffAddPowerSNOAttribute(tBuffStartData, snoRMBSkill);

    ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);
    if (ptACDOwner)
    {
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    }
}

// ProjectileReflect: raw DWARF has only Projectile.cpp:1127, no recovered header declaration.
BOOL ProjectileReflect(ACDID idACDProjectile, ACDID idACDReflector, BOOL bReflectYawOnly, Angle aYawOffset, Team eTeam, float flSpeedMult);
#line 6812 "X/Server/Powers/PowersSwig.cpp"
void PowerProjectileReflect(
    ACDID idACDProjectile,
    BOOL bReflectYawOnly,
    int32 nDegreesOffset,
    float flSpeedMultiplier,
    ACDID idACDForcedTarget,
    float flReflectDamageScalar,
    float flReflectHealthPercentDamageCapScalar)
{

    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (!ptACDProjectile)
    {
        return;
    }

    if (flSpeedMultiplier < EPSILON)
    {
        return;
    }




    PowerID idPower = sGetCurrentPower();
    Angle aAngleOffset = Math::DegToAngle(nDegreesOffset);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {

        BOOL bReflected = ProjectileReflect(idACDProjectile, ptACDOwner->id, bReflectYawOnly, aAngleOffset, -1, flSpeedMultiplier);
        if (!bReflected)
        {
            return;
        }

        PowerID idProjectilePower = ActorPowersGetOwnerPower((ActorID)ACDGetOwner(idACDProjectile));
        Power * ptProjectilePower = PowerTryToGet(idProjectilePower);
        if (ptProjectilePower)
        {
            ptProjectilePower->snoPowerLastReflectedBy = PowerGetSNO(idPower);
            ptProjectilePower->idACDLastReflectedBy = ptACDOwner->id;
        }

        if (flReflectDamageScalar < 1.0f)
        {
            FastAttribSetValueFloat(ptProjectilePower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_SCALAR, flReflectDamageScalar);
        }

        ActorCommonData * ptACDForcedTarget = ACDTryToGet(idACDForcedTarget);
        if (ptACDForcedTarget)
        {
            ptACDProjectile->AttributesSetInt(ATTRIBUTE_PROJECTILE_REFLECT_FORCED_TARGET, idACDForcedTarget);

            if (flReflectHealthPercentDamageCapScalar > EPSILON)
            {

                BOOL bHasDamageCap = FastAttribGetValueInt(ptProjectilePower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_HAS_DAMAGE_CAP);
                if (!bHasDamageCap)
                {
                    FastAttribSetValueInt(ptProjectilePower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_HAS_DAMAGE_CAP, TRUE);
                    FastAttribSetValueFloat(ptProjectilePower->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_CAP_REMAINING, ptACDForcedTarget->GetHitPointsMaxTotal() * flReflectHealthPercentDamageCapScalar);
                }
            }
        }






        if (ACDIsPlayer(ptACDOwner->id))
        {
            Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);

            SNO snoPower = SNO_NONE;
            Power * ptPower = PowerGet(idPower);
            if (ptPower)
            {
                snoPower = ptPower->snoPower;
            }

            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);

            tEvaluationData.EvaluateEvent(
                ptPlayer,
                Achievement::ACH_EVENT_PLAYER_USED_POWER_TO_REFLECTE_PROJECTILE,
                snoPower,
                1, (uint32)sub_71007b5e70());
        }
    }

}

#line 8463 "X/Server/Powers/PowersSwig.cpp"
ACDID PowerPlayerReviveActorOnCursor(
    LPCSTR szReviveActorName,
    int32 nCursorID,
    float flDuration,
    BOOL bUseExactLocation,
    PetType ePetType,
    int32 nMaxCount)
{

    ACDID idACDRevived = PowerSummonActorOnCursor(szReviveActorName, nCursorID, flDuration, bUseExactLocation, -1, 0, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, ACDID_INVALID);
    if (idACDRevived != ACDID_INVALID)
    {

        ActorCommonData * ptACD = ACDTryToGet(idACDRevived);
        if (ptACD)
        {
            ScopedActorDefinitionLockR pdefPet(ptACD->snoActor);
            if (pdefPet)
            {
                ScopedMonsterDefinitionLockR pdefMonster(pdefPet->snoMonster);
                if (pdefMonster)
                {
                    float flHPBase = pdefMonster->pctAttributeModifiers[GBA_HITPOINTS];
                    float flHPDelta = pdefMonster->pctAttributeModifiers[GBA_HPRANGE];
                    float flBonusHealth = flHPBase + RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, flHPDelta);
                    ptACD->AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX_PERCENT_BONUS, flBonusHealth);


                    static GameBalanceAttribute damageBaseAttribs[7] = {
                        GBA_DAMAGE, GBA_DAMAGE_FIRE, GBA_DAMAGE_LIGHTNING, GBA_DAMAGE_COLD, GBA_DAMAGE_POISON, GBA_DAMAGE_ARCANE, GBA_DAMAGE_SIEGE
                    };

                    static GameBalanceAttribute damageDeltaAttribs[7] = {
                        GBA_DAMAGERANGE, GBA_DAMAGERANGE_FIRE, GBA_DAMAGERANGE_LIGHTNING, GBA_DAMAGERANGE_COLD, GBA_DAMAGERANGE_POISON, GBA_DAMAGERANGE_ARCANE, GBA_DAMAGERANGE_SIEGE
                    };


                    static DamageType eDamageTypes[7] = {
                        DAMAGE_TYPE_PHYSICAL, DAMAGE_TYPE_FIRE, DAMAGE_TYPE_LIGHTNING, DAMAGE_TYPE_COLD, DAMAGE_TYPE_POISON, DAMAGE_TYPE_ARCANE, DAMAGE_TYPE_HOLY
                    };







                    float flMaxDamageBase = 0.0f;
                    float flDamageDelta = 0.0f;
                    DamageType eMaxDamageType = DAMAGE_TYPE_PHYSICAL;
                    for (int32 i = 0; i < NUM_DAMAGE_TYPES; ++i)
                    {
                        if (pdefMonster->pctAttributeModifiers[damageBaseAttribs[i]] > flMaxDamageBase)
                        {
                            flMaxDamageBase = pdefMonster->pctAttributeModifiers[damageBaseAttribs[i]];
                            flDamageDelta = pdefMonster->pctAttributeModifiers[damageDeltaAttribs[i]];
                            eMaxDamageType = eDamageTypes[i];
                        }
                    }

                    float flBonusDamage = flMaxDamageBase + RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, flDamageDelta);
                    ptACD->AttributesSetFloat(ATTRIBUTE_MULTIPLICATIVE_DAMAGE_PERCENT_BONUS, flBonusDamage);


                    float flOwnerDPS = PowerGetOwnerWeaponDPS(FALSE);
                    PowerActorSetDamageAttributes(idACDRevived, eMaxDamageType, flOwnerDPS, 0.0f);
                }
            }
        }


        PowerPlayerAddPet(idACDRevived, ePetType, nMaxCount, ACDID_INVALID, FALSE, FALSE);
    }

    return idACDRevived;
}


#line 8369 "X/Server/Powers/PowersSwig.cpp"
static void sPowerPlayerGetPetsInsertByCreateTime(
    XPooledDataIDList & listSortedACDs,
    ACDID idACD)
{
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(idACD));

    if (!ptActor)
    {
        return;
    }

    POS pos = listSortedACDs.GetHeadPosition();
    while (pos)
    {
        POS oldpos = pos;
        ACDID idACDCurr = (ACDID)listSortedACDs.GetNext(pos);
        Actor * ptActorCurr = ActorTryToGet((ActorID)ACDGetOwner(idACDCurr));

        if (!ptActorCurr)
        {
            continue;
        }

        if (ptActorCurr->dwTimeCreated > ptActor->dwTimeCreated)
        {
            listSortedACDs.InsertBefore_Tracked(oldpos, idACD);
            return;
        }
    }
    listSortedACDs.AddTail_Tracked(idACD);
}


ActorIDArray * PowerPlayerGetPets(
    PetType ePetType,
    VACDID idACDPlayer)
{
    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;

    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACDOwner;
    if (idACDPlayer != ACDID_INVALID)
    {
        ptACDOwner = ACDTryToGet(idACDPlayer);
    }
    else
    {
        ptACDOwner = PowerGetOwner(idPower);
    }

    if (!ptACDOwner)
    {
        return &tArrACDs;
    }

    if (!ACDIsPlayer(ptACDOwner->id))
    {
        return &tArrACDs;
    }

    int32 nPos;
    ACDID idACDPet = PlayerGetFirstPet(ptACDOwner->id, ePetType, nPos);

    XPooledDataIDList listPets(MemoryPoolsGetDATAIDListPool());

    while (idACDPet != ACDID_INVALID)
    {

        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet)
        {
            sPowerPlayerGetPetsInsertByCreateTime(listPets, idACDPet);
        }



        idACDPet = PlayerGetNextPet(ptACDOwner->id, ePetType, nPos);
    }

    POS pos = listPets.GetHeadPosition();
    while (pos)
    {

        idACDPet = (ACDID)listPets.GetNext(pos);
        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDPet;
        ++tArrACDs.nNumActorIDs;
    }


    return &tArrACDs;
}

#include "../../Sigma/Math/Sphere.h"
// DWARF records only WorldCommonCollision.cpp:147; no header declaration was found.
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);
// DWARF records only Powers.cpp:5481; no header declaration was found.
BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax);
#line 7087 "X/Server/Powers/PowersSwig.cpp"
static const float BEAM_TARGET_Z_HEIGHT_DIFFERENCE_FUDGE = 2.0f;

static BOOL sPowerBeamGetTargetPlace(
    const PowerID idPower,
    ACDID idACDTarget,
    WorldPlace & tTargetPlace,
    float flMaxLength,
    BOOL bUseRollToClickPositionOnGround,
    SNO snoPowerPassability)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);

    Target tTarget;
    if (PowerGetTarget(idPower, tTarget) && idACDTarget != ACDID_INVALID)
    {

        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (!ptACDTarget)
        {
            return FALSE;
        }

        ActorID idActorOwner = ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID;
        Actor * ptActor = ActorTryToGet(idActorOwner);
        if (!ptActor)
        {
            return FALSE;
        }

        SActorMovement * ptMove = ptActor->ptSActorMovement;
        if (!ptMove)
        {
            return FALSE;
        }


        WorldCollParams tCollParams;
        BOOL bHasFlags = PowerFillCollParams(ptACDOwner->id, AutoPowerDefinitionR(snoPowerPassability), POWER_PASSABILITY_DURING, tCollParams);
        if (!bHasFlags)
        {
            return FALSE;
        }




        NavPlace tNavPlaceSource;
        ACDGetNavMeshHandleWithPlace(*ptACDOwner, tNavPlaceSource.tNavMeshHandle, tNavPlaceSource.tPlace);
        float flRadius = flMaxLength;

        NavPlace tNavPlaceContact;
        Vector3D vStartOffset;
        PowerGetACDPoint(idPower, ptACDOwner->id, tNavPlaceContact, SNO_NONE, -1, &vStartOffset);

        WorldPlace placeCurr = ptACDOwner->GetPlace();
        placeCurr.wp.z += vStartOffset.z;

        if (flRadius > EPSILON)
        {
            Vector3D vDelta;
            BOOL bValidPlace = FALSE;
            if (!tTarget.IsInvalidForSource(ptACDOwner))
            {
                tTarget.GetPlace(tTargetPlace);
            }
            else
            {

                ACDID idACDClientsTarget = tTarget.GetACD();
                ActorCommonData * ptACDClientsTarget = ACDTryToGet(idACDClientsTarget);

                if (idACDClientsTarget != ACDID_INVALID && !ptACDClientsTarget)
                {
                    tTargetPlace = ptACDTarget->GetPlace();
                }
                else
                {

                    WorldPlace wpErrorPlace;
                    tTarget.GetPlace(wpErrorPlace);









                    return FALSE;
                }
            }

            SGameGlobalsGet();





            if (tTarget.IsTargetingACD())
            {
                tTargetPlace.wp.z += vStartOffset.z;
            }



            float flTargetHeightDifference = Math::Abs(tTargetPlace.wp.z - placeCurr.wp.z);
            if (flTargetHeightDifference <= BEAM_TARGET_Z_HEIGHT_DIFFERENCE_FUDGE)
            {
                tTargetPlace.wp.z = placeCurr.wp.z;
            }


            vDelta = tTargetPlace.wp - placeCurr.wp;

            if (vDelta.MagnitudeSquared() >= EPSILON * EPSILON)
            {
                vDelta.Normalize();
                bValidPlace = TRUE;
            }



            if (bUseRollToClickPositionOnGround)
            {
                WorldPlace wp;
                PowerComputeDestination(ptPower->snoPower, *ptMove, tTargetPlace, wp, FALSE, flRadius);
                tTargetPlace = wp;
                return TRUE;
            }

            float flDistSquaredToTarget = Vector3D::Distance2DSquared(tTargetPlace.wp, ptACDOwner->GetPlace().wp);
            if (!bValidPlace || flDistSquaredToTarget <= 0.1f)
            {


                Angle aYaw = ACDGetYaw(ptACDOwner->id);
                vDelta.Set(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
            }

            SGameGlobalsGet();




            WorldPlace wpOriginalTarget;


            vDelta *= flRadius;
            tTargetPlace = tNavPlaceSource.tPlace;
            tTargetPlace.wp += vDelta;







            tTargetPlace.wp.z += vStartOffset.z;

            SGameGlobalsGet();




            WorldPlace wpOriginalAdjustedTarget;




            Circle wcStart;
            wcStart.wpCenter = placeCurr.wp.As2D();
            wcStart.wdRadius = 0.5f;

            WorldCollResult tResult;
            WorldCollSweptCircle(
                placeCurr.idSWorld,
                ptMove->GetNavCell(),
                wcStart,
                tTargetPlace.wp.As2D(),
                tCollParams,
                0,
                0,
                ptACDOwner->id,
                tResult);

            SGameGlobalsGet();




            if (tResult.bCollision)
            {
                WorldPlace wpResult;
                wpResult.idSWorld = tTargetPlace.idSWorld;
                wpResult.wp = tResult.wpEnd;


















                float flFlatDist = Vector2D::DistanceSquared(wpResult.wp.As2D(), wcStart.wpCenter);


                ptMove->SnapPlaceToGround(wpResult);
                wpResult.wp.z += vStartOffset.z;
                float flZDifference = placeCurr.wp.z - wpResult.wp.z;


                float flHypotenuseDist = Math::Sqrt(flFlatDist + flZDifference * flZDifference);

                vDelta.Normalize();
                vDelta *= flHypotenuseDist;

                tTargetPlace = tNavPlaceSource.tPlace;
                tTargetPlace.wp.z += vStartOffset.z;
                tTargetPlace.wp += vDelta;
            }

            SGameGlobalsGet();




            float flT;
            Vector3D wpCollision;
            Vector3D wvToTarget = tTargetPlace.wp - placeCurr.wp;


            if (NavZoneCollideSweptPoint3D(placeCurr.idSWorld, placeCurr.wp, wvToTarget, tCollParams.uOneRequiredStaticFlags, tCollParams.uStaticPreventFlags, 0, flT, wpCollision))
            {
                tTargetPlace.wp = wpCollision;
                SGameGlobalsGet();
            }

            return TRUE;
        }
    }

    return FALSE;
}




BOOL sPowerBeamUpdate(
    PowerID idPower,
    ACDID idACDTarget,
    float flMaxLength,
    BOOL bUseRollToClickPositionOnGround,
    SNO snoPowerPassability)
{
    ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);

    WorldPlace tTargetPlace;
    if (sPowerBeamGetTargetPlace(idPower, idACDTarget, tTargetPlace, flMaxLength, bUseRollToClickPositionOnGround, snoPowerPassability))
    {
        ActorCommonData & tACDOwner = *PowerGetOwner(idPower);


        WorldPlace placeOwner = tACDOwner.GetPlace();
        NavPlace tNavPlaceContact;
        PowerGetACDPoint(idPower, tACDOwner.id, tNavPlaceContact, SNO_NONE, -1, NULL);

        Vector3D vDistTravelled = tTargetPlace.wp - placeOwner.wp;
        if (vDistTravelled.MagnitudeSquared() < 1.0f)
        {
            ActorSnapToPosition(idActorTarget, tNavPlaceContact.tPlace, FALSE, FALSE);
        }
        else
        {

            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (!ptACDTarget)
            {
                return FALSE;
            }


            if (!ptACDTarget->IsInWorld())
            {
                return FALSE;
            }

            if (ptACDTarget->GetPlace().idSWorld != tTargetPlace.idSWorld)
            {
                return FALSE;
            }

            ActorSnapToPosition(idActorTarget, tTargetPlace, FALSE, FALSE);
        }

        return TRUE;
    }

    return FALSE;
}

#include "../../Common/Networking/Network.h"
// DWARF records only SPlayer.cpp:12152; no header declaration was found.
BOOL SPlayerFindValidWalkableSpotNearPlayer(const ActorCommonData & tACDPlayer, WorldPlace & placeSpot);
// DWARF records only PowerAPI.cpp:13626; no header declaration was found.
void PowerAPI_UpdateSummonedActorAttributes(ActorCommonData & tACDSummoned, ActorCommonData & tACDOwner, Power * ptPower);
#line 7577 "X/Server/Powers/PowersSwig.cpp"
static BOOL sPlaceIsValidForSpawn(
    const WorldPlace & placeCheck)
{
    uint32 uStaticFlags = NavCellGetStaticFlags(placeCheck);
    BOOL bValidSpot = (uStaticFlags & (1 << CELL_STATIC_ALLOW_WALK)) != 0;
    return bValidSpot;
}



ACDID PowerSummonActorOnCursor(
    LPCSTR szSummonActorName,
    int32 nCursorID,
    float flDuration,
    BOOL bUseExactLocation,
    int32 dwAnimTag,
    int32 nSummonLevel,
    BOOL bCopySummonerAttribs,
    BOOL bUpdateAttributesFromOwner,
    BOOL bNeverDeactivate,
    BOOL bUseActorStaticFlagsForSpawn,
    BOOL bTryToSpawnOnPlayerLocationUponFail,
    BOOL bNoSpawnAnimPower,
    ACDID idACDOwnerToUse)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    ACDID idACDSummoned = ACDID_INVALID;

    if (!sPowerValidateCursor(ptPower, nCursorID))
    {
        return idACDSummoned;
    }

    SNO snoSummonActor = SNOFileLookupGUID(ACTOR_SNO, szSummonActorName);
    BOOL bExists;

















    if (ptPower && snoSummonActor != SNO_NONE)
    {
        PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
        if (!ptCursor)
        {
            return idACDSummoned;
        }

        ActorCommonData * ptACD;
        if (idACDOwnerToUse != ACDID_INVALID)
        {
            ptACD = ACDTryToGet(idACDOwnerToUse);
        }
        else
        {
            ptACD = PowerGetOwner(idPower);
        }

        if (!ptACD)
        {
            return idACDSummoned;
        }

        PowerAPISummonActor tSummonInfo;
        SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
        tSummonInfo.idPower = idPower;
        tSummonInfo.snoActor = snoSummonActor;
        tSummonInfo.bNoSpawnAnimPower = bNoSpawnAnimPower;
        if (nSummonLevel > 0)
        {
            tSummonInfo.nActorLevel = nSummonLevel;
        }
        else
        {
            tSummonInfo.nActorLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
        }

        tSummonInfo.idACDCorpse = ACDID_INVALID;
        tSummonInfo.placeSummonLocation = ptCursor->tPlace;
        tSummonInfo.qOrientation = ptCursor->qOrientation;
        if (bNeverDeactivate)
        {
            tSummonInfo.dwAddFlags = 1 << ACTORADD_DOESNT_DEACTIVATE;
        }

        if (dwAnimTag != TAG_ACTOR_ANIMATION_INVALID)
        {
            tSummonInfo.dwAnimTag = dwAnimTag;
        }

        if (bUseExactLocation)
        {
            tSummonInfo.dwAddFlags |= 1 << ACTORADD_DONT_PERTURB;
            tSummonInfo.dwAddFlags |= 1 << ACTORADD_DONT_SEARCH_FOR_LOCATION;
            tSummonInfo.dwAddFlags |= 1 << ACTORADD_EVEN_WHEN_STUCK;
        }

        if (bUseActorStaticFlagsForSpawn)
        {
            tSummonInfo.dwAddFlags |= 1 << ACTORADD_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN;
        }

        if (bTryToSpawnOnPlayerLocationUponFail)
        {
            if (!sPlaceIsValidForSpawn(tSummonInfo.placeSummonLocation))
            {
                PlayerList listEligiblePlayers(PlayerListGetElementPool());
                PlayerListGetAllInGame(listEligiblePlayers, -1, FALSE, GAMECONNECTIONID_INVALID);
                POS posPlayer = listEligiblePlayers.GetHeadPosition();
                while (posPlayer)
                {
                    Player * ptPlayer = listEligiblePlayers.GetNext(posPlayer);
                    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptACDPlayer)
                    {
                        WorldPlace placePlayer = ptACDPlayer->GetPlace();
                        if (placePlayer.idSWorld == tSummonInfo.placeSummonLocation.idSWorld)
                        {
                            WorldPlace placeSpot;
                            BOOL bFound = SPlayerFindValidWalkableSpotNearPlayer(*ptACDPlayer, placeSpot);
                            if (bFound)
                            {
                                tSummonInfo.placeSummonLocation = placeSpot;
                                break;
                            }
                        }
                    }
                }
            }
        }

        tSummonInfo.eTeamOverride = TEAM_INVALID;

        idACDSummoned = PowerAPI_SummonActor(tSummonInfo);

        if (idACDSummoned != ACDID_INVALID)
        {
            ActorCommonData & tACDSummoned = ACDGet(idACDSummoned);

            if (flDuration > EPSILON)
            {
                uint32 dwSummonDuration = Math::FloatToInt(flDuration * TICKS_PER_SECOND);
                tACDSummoned.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, GameGetServerTime() + dwSummonDuration);
            }

            if (bCopySummonerAttribs)
            {

                if (ACDIsPlayer(ptACD->id))
                {

                    if (bUpdateAttributesFromOwner)
                    {
                        tACDSummoned.AttributesSetInt(ATTRIBUTE_ACTOR_UPDATES_ATTRIBUTES_FROM_OWNER, TRUE);
                    }

                    FastAttribGroup * ptPowerFastAttrib = FastAttribGroupGet(ptPower->idFastAttrib);

                    PowerAPI_UpdateSummonedActorAttributes(tACDSummoned, *ptACD, ptPower);
                    tACDSummoned.SetHitPoints(tACDSummoned.GetHitPointsMaxTotal(), FALSE, FALSE);


                    int32 nSkillLevel = FastAttribGetValueInt(ptPowerFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, ptPower->snoPower));
                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, ptPower->snoPower), nSkillLevel);


                    int32 nRuneALevel = FastAttribGetValueInt(ptPowerFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_A, ptPower->snoPower));
                    int32 nRuneBLevel = FastAttribGetValueInt(ptPowerFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_B, ptPower->snoPower));
                    int32 nRuneCLevel = FastAttribGetValueInt(ptPowerFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_C, ptPower->snoPower));
                    int32 nRuneDLevel = FastAttribGetValueInt(ptPowerFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_D, ptPower->snoPower));
                    int32 nRuneELevel = FastAttribGetValueInt(ptPowerFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_E, ptPower->snoPower));
                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, ptPower->snoPower), nRuneALevel);
                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, ptPower->snoPower), nRuneBLevel);
                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, ptPower->snoPower), nRuneCLevel);
                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, ptPower->snoPower), nRuneDLevel);
                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, ptPower->snoPower), nRuneELevel);

                    if (ptPower->bDoesntAnimate)
                    {
                        tACDSummoned.AttributesSetInt(ATTRIBUTE_SUMMONED_BY_AUTOCAST, TRUE);
                    }


                    SNO snoSkillKit = ptACD->GetSkillKit();
                    if (snoSkillKit != SNO_NONE)
                    {
                        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);

                        int32 nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
                        for (int32 i = 0; i < nCount; i++)
                        {
                            TraitEntry & tEntry = pdefSkillKit->arTraitEntries[i];
                            if (tEntry.snoPower != SNO_NONE)
                            {
                                int32 nSkillLevelScoped = FastAttribGetValueInt(ptPowerFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, tEntry.snoPower));
                                if (nSkillLevelScoped > 0)
                                {
                                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, tEntry.snoPower), nSkillLevelScoped);
                                }
                            }
                        }
                    }



                    tACDSummoned.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT)), 1);
                }
                else
                {



                    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS));
                    tACDSummoned.AttributesSetFloat(ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT, ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT));
                }
            }

            if (!ACDIsPlayer(ptACD->id))
            {
                int nStoredCount = ptACD->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
                ptACD->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nStoredCount + 1);
            }
        }
    }

    return idACDSummoned;
}
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../Actors/ActorEffect.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#line 13400 "X/Server/Powers/PowersSwig.cpp"

void PowerSpawnCorpse(
    int32 nCursorID,
    ACDID idOwnerOverride,
    LPCSTR szReviveActorName,
    BOOL bQuiet)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);
    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);

    ActorCommonData * ptACDOwner;
    if (idOwnerOverride != ACDID_INVALID)
    {
        ptACDOwner = ACDTryToGet(idOwnerOverride);
    }
    else
    {
        ptACDOwner = PowerGetOwner(idPower);
    }

    if (!ptACDOwner)
    {
        return;
    }


    if (ptACDOwner->IsPet())
    {
        Player * ptPlayer = ptACDOwner->GetPetOwnerPlayer();
        if (ptPlayer)
        {
            ptACDOwner = ACDTryToGet(ptPlayer->idPlayerACD);
            if (!ptACDOwner)
            {
                return;
            }
        }
    }


    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }

    ScopedGlobalDefinitionLockR pdefGlobal;
    int32 nMaxCorpses = pdefGlobal->ptServerData->nCorpsesMax;

    NavCellHandle hNavCellStart, hNavCellSearch;
    Vector3D wpSearch;
    float flMaxSearchRadius = 5.0f;

    WorldPlace placeStart = ptCursor->tPlace;
    WorldPlace placeSpawn = ptCursor->tPlace;
    uint16 uOneRequiredFlags = 1 << CELL_STATIC_ALLOW_WALK;

    BOOL bFoundPlace = FALSE;


    if (NavCellHandleFromWorldPlace(placeStart, hNavCellStart))
    {
        const NavZoneInstance * ptNavZoneInstance = NavCellHandleToZoneInstance(hNavCellStart);
        NavCellModifyForRoundedCorners(ptNavZoneInstance, hNavCellStart, placeStart.wp.As2D(), uOneRequiredFlags);

        uint32 uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(PASSABILITY_FLIPPIES);
        uint16 uObjectNoPassthroughFlags = (1 << COLL_DYN_LOS_BLOCKER) | (1 << COLL_DYN_GATE) | (1 << COLL_DYN_TELEPORT_BLOCKER);
        if (WorldCollSearchForLocation(placeStart, uOneRequiredFlags, uObjectNoOverlapFlags, 1.25f, 0.0f, flMaxSearchRadius, ACDID_INVALID, wpSearch, hNavCellSearch, TEAM_NEUTRAL, uObjectNoPassthroughFlags, ACDID_INVALID))
        {










            bFoundPlace = TRUE;
            placeSpawn.idSWorld = placeStart.idSWorld;
            placeSpawn.wp = wpSearch;
        }
    }




    NavMeshHandle tNavMeshHandle;
    if (ScenesCommonNavMeshNodeFromPlace(placeStart, &tNavMeshHandle, FALSE, FALSE))
    {
        uint32 dwSquareStaticFlags = NavMeshHandleGetStaticFlags(tNavMeshHandle);
        if (dwSquareStaticFlags & (1 << CELL_STATIC_ALLOW_WALK))
        {
            NavMeshGetCenterPlace(tNavMeshHandle, placeSpawn);
            bFoundPlace = TRUE;
        }
    }

    if (bFoundPlace)
    {
        SNO snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_CORPSE_FOR_NECROMANCER);
        ActorAddStruct tActorAdd;
        tActorAdd.dwActorAddFlags = (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_ADD_IF_LOCATION_SEARCH_FAILS);
        tActorAdd.tPlace = ptCursor->tPlace;
        tActorAdd.qOrientation.FromYaw(RandAngle(SGameGlobalsGet().rng));
        tActorAdd.eTeam = ptACDOwner->GetTeam();
        tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
        tActorAdd.tSNOName.snoHandle = snoActor;
        tActorAdd.snoActor = snoActor;

        ActorID idCorpse = GBAdd(tActorAdd);

        ACDID idACDCorpse = ACDFindByActor(idCorpse);
        ActorCommonData * ptACDCorpse = ACDTryToGet(idACDCorpse);

        if (ptACDCorpse)
        {
            SNO snoEffect = GlobalSNOGet(bQuiet ? GLOBALSNO_EFFECTGROUP_NECRO_CORPSE_SPAWN_QUIET : GLOBALSNO_EFFECTGROUP_NECRO_CORPSE_SPAWN);
            ACDPlayEffectForPlayer(ptACDOwner->id, idACDCorpse, EFFECT_NECRO_CORPSE_SPAWN, snoEffect);

            if (szReviveActorName)
            {
                snoActor = SNOFileLookupGUID(ACTOR_SNO, szReviveActorName);
                SNO snoMonster = ActorGetMonsterSNO(snoActor);
                ptACDCorpse->AttributesSetInt(ATTRIBUTE_NECROMANCER_CORPSE_SOURCE_MONSTER_SNO, snoMonster);
            }

            ptACDCorpse->SetAssignedHeroID(ptPlayer);
            ptACDCorpse->ModifiedMark(ACDMODIFIED_ATTRIBUTES);




            PlayerList listLocalPlayers(PlayerListGetElementPool());
            PlayerListGetAllPlayersLocalToPlayer(listLocalPlayers, ptPlayer);
            POS posPlayerCorpseOwner = listLocalPlayers.GetHeadPosition();
            uint32 nPlayerIndex = 0;
            while (posPlayerCorpseOwner)
            {
                Player * ptLocalPlayer = listLocalPlayers.GetNext(posPlayerCorpseOwner);

                if (PlayerGetPlayerClass(ptLocalPlayer) == PLAYER_CLASS_NECROMANCER)
                {
                    ptLocalPlayer->listNecroCorpses.push_back(idACDCorpse);
                    ptACDCorpse->SetNecroCorpseOwnerPlayerID(ptLocalPlayer->tAccount.tHandle.m_id, nPlayerIndex++);
                }
            }


            ptACDCorpse->AttributesSetInt(ATTRIBUTE_NECROMANCER_CORPSE_CHARGES, 1);



            if (ptPlayer->listNecroCorpses.size() > nMaxCorpses)
            {

                ACDID idCorpseToDelete = ptPlayer->listNecroCorpses[0];
                blzext::vector_erase_at(ptPlayer->listNecroCorpses, 0);
                ActorDelete((ActorID)ACDGetOwner(idCorpseToDelete));
            }
        }
    }
}

// Cached raw DWARF records only X/Common/ActorCommonData.cpp:3027; no header declaration.
void ACDSetYaw(ACDID idACD, const Angle aYaw);
// Cached raw DWARF records only X/Server/SPlayer.cpp:2987 for this overload; no header declaration.
int32 SPlayerAddPet(ACDID idACDPlayer, PetType eType, ACDID idACDPet, BOOL fSendNow, int32 nMaxCount, PetComparisonValueFunc pCompareFunc);
// Cached raw DWARF records only X/Server/Powers/PowerAPI.cpp:10423; no header declaration.
void PowerAPI_BuffCopyAllVisualsToACD(const ActorCommonData & tACD, ActorCommonData & tACDDest);

#line 14309
ACDID sPowerSummonMirrorImage(
    PowerID idPower,
    SNO snoActor,
    PetType ePetType,
    int32 nMaxCount,
    NavPlace tNavPlace,
    Quaternion & qOrientation)
{

    ActorCommonData & tACDSource = *PowerGetOwner(idPower);
    BOOL bIsPlayer = ACDIsPlayer(tACDSource.id);
    Power * ptPower = PowerGet(idPower);

    PowerAPISummonActor tSummonInfo;
    SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
    tSummonInfo.idPower = idPower;
    tSummonInfo.snoActor = snoActor;
    tSummonInfo.nActorLevel = tACDSource.AttributesGetInt(ATTRIBUTE_LEVEL);
    tSummonInfo.idACDCorpse = ACDID_INVALID;
    tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_NO_AFFIXES);



    tSummonInfo.placeSummonLocation = tNavPlace.tPlace;
    tSummonInfo.qOrientation = qOrientation;
    tSummonInfo.eTeamOverride = TEAM_INVALID;
    tSummonInfo.bNoSpawnAnimPower = TRUE;

    ACDID idACDClone = PowerAPI_SummonActor(tSummonInfo);
    if (idACDClone != ACDID_INVALID)
    {
        ActorCommonData & tACDClone = ACDGet(idACDClone);


        float flHpPercent = tACDSource.GetHitpointPercent();
        float flHpMaxTotal = tACDSource.GetHitPointsMaxTotal();
        tACDClone.AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX, flHpMaxTotal);
        tACDClone.AttributesSetFloat(ATTRIBUTE_HITPOINTS_CUR, Math::Max(1.0f, flHpPercent * flHpMaxTotal));


        ActorLocationSetScale(tACDClone.tLocation, tACDSource.GetScale());


        ACDSetYaw(tACDClone.id, ACDGetYaw(tACDSource.id));

        if (tACDClone.IsMonster())
        {
            tACDClone.eMonsterRarity = tACDSource.eMonsterRarity;
            for (int32 i = 0; i < 2; ++i)
            {
                tACDClone.pRareNameGBIDs[i] = tACDSource.pRareNameGBIDs[i];
            }
            for (int32 i = 0; i < 8; ++i)
            {
                tACDClone.pMonsterAffixGBIDs[i] = tACDSource.pMonsterAffixGBIDs[i];
            }
        }


        if (bIsPlayer)
        {

            tACDClone.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
            tACDClone.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);

            SPlayerAddPet(tACDSource.id, ePetType, idACDClone, FALSE, nMaxCount, sCompareLifeTime);


            tACDClone.AttributesSetInt(ATTRIBUTE_IS_PLAYER_DECOY, TRUE);


            if (ePetType != PETTYPE_NECROMANCER_SIMULACRUM)
            {
                WeaponClass eWeaponClass = tACDSource.GetWeaponClassForAnim(NULL);
                tACDClone.AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, eWeaponClass);
            }












            tACDClone.AttributesSetInt(ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, TRUE);


            tACDClone.tCollSettings.uDynamicMyFlags = 1 << COLL_DYN_PET;


            if (ePetType != PETTYPE_NECROMANCER_SIMULACRUM)
            {
                PowerAPI_BuffCopyAllVisualsToACD(tACDSource, tACDClone);
            }


            tACDClone.AttributesSetInt(ATTRIBUTE_ACTOR_UPDATES_ATTRIBUTES_FROM_OWNER, TRUE);

            PowerAPI_UpdateSummonedActorAttributes(tACDClone, tACDSource, ptPower);
            tACDClone.SetHitPoints(tACDClone.GetHitPointsMaxTotal(), FALSE, FALSE);

            for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType)
            {
                tACDClone.AttributesSetFloat(
                    MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, nDamageType),
                    tACDSource.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_MIN_TOTAL, nDamageType)));
            }


            PowerActorCopySkillsAndRunes(tACDSource.id, idACDClone);


            tACDClone.AttributesSetInt(ATTRIBUTE_FREE_CAST_ALL, TRUE);
        }
    }
    return idACDClone;
}

#line 14431
ActorIDArray * PowerSpawnMirrorImages(
    ACDID idACDSource,
    LPCSTR szMonsterActorName,
    float flSpawnRadius,
    int32 nCount,
    int32 nMaxCount,
    BOOL bMoveCaster,
    BOOL bDoAnimation,
    LPCSTR szPowerForPassability,
    PetType ePetType)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    SNO snoActor = SNOFileLookupGUID(ACTOR_SNO, szMonsterActorName);
    BOOL bExists;

    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;



















    SNO snoPowerPassability;

    if (szPowerForPassability)
    {
        SNO snoPower = SNOFileLookupGUID(POWER_SNO, szPowerForPassability);















        snoPowerPassability = snoPower;
    }
    else
    {
        snoPowerPassability = ptPower->snoPower;
    }




    ActorCommonData * ptACD = ACDTryToGet(idACDSource);
    if (!ptACD)
    {
        return &tArrACDs;
    }

    NavPlace tNavPlace;
    ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
    Quaternion qOrientation = ptACD->GetOrientation();

    Circle circleStart;
    ptACD->GetRadius();

    Angle aAngle = RandAngle(SGameGlobalsGet().rng);

    WorldCollParams tWorldCollParams;
    BOOL bPowerHasCollFlags = PowerFillCollParams(idACDSource, AutoPowerDefinitionR(snoPowerPassability), POWER_PASSABILITY_DURING, tWorldCollParams);
    if (!bPowerHasCollFlags)
    {
        return &tArrACDs;
    }


    NavCellHandle hNavCell;
    NavCellHandleFromWorldPlace(tNavPlace.tPlace, hNavCell);

    for (int32 i = 0; i < nCount + 1; ++i)
    {
        WorldPlace wpDesiredDest = tNavPlace.tPlace;
        wpDesiredDest.wp.x += Math::Sin(aAngle) * flSpawnRadius;
        wpDesiredDest.wp.y += Math::Cos(aAngle) * flSpawnRadius;

        WorldPlace placeDest;
        const ActorMovement * ptMove = ACDGet(ptPower->idACD).GetActorMovement();
        PowerComputeDestination(PowerGetSNO(idPower), *ptMove, wpDesiredDest, placeDest, FALSE, 10.0f);


        float flSpeed = 1.0f;


        if (i == nCount)
        {
            if (bMoveCaster)
            {
                ActorID idActor = (ActorID)ptACD->idOwner;
                Actor * ptActor = ActorTryToGet(idActor);
                if (ptActor && ptActor->ptSActorMovement)
                {
                    uint32 uMoveFlags = (1 << MOVEBIT_IGNORE_COLLISION) | (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING) | (1 << MOVEBIT_ACTIVATE_TRIGGERS);



                    ptActor->ptSActorMovement->MoveTo(placeDest, flSpeed, 0.0f, TAG_ACTOR_ANIMATION_INVALID, uMoveFlags, 0.0f, ACDID_INVALID, ptPower->snoPower, 0, -1, 0.0f, NULL);

                    Player * ptPlayer = PlayerGetByACD(ptACD->id);
                    if (ptPlayer)
                    {



                        ptPlayer->tQueuedPower.dwPowerLastEndTime = GameGetServerTime();
                    }
                }
            }
        }
        else
        {
            NavPlace tCurrentNavPlace;
            ACDGetNavMeshHandleWithPlace(*ptACD, tCurrentNavPlace.tNavMeshHandle, tCurrentNavPlace.tPlace);
            ACDID idACDMirrorImage = sPowerSummonMirrorImage(idPower, snoActor, ePetType, nMaxCount, tCurrentNavPlace, qOrientation);

            if (idACDMirrorImage == ACDID_INVALID)
            {
                continue;
            }

            tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACDMirrorImage;
            ++tArrACDs.nNumActorIDs;



            ActorCommonData & tACDMirrorImage = ACDGet(idACDMirrorImage);
            ActorID idActor = (ActorID)ACDGetOwner(tACDMirrorImage.id);
            Actor * ptActor = ActorTryToGet(idActor);
            if (ptActor && ptActor->ptSActorMovement)
            {
                uint32 uMoveFlags = (1 << MOVEBIT_IGNORE_COLLISION) | (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING) | (1 << MOVEBIT_ACTIVATE_TRIGGERS);



                ptActor->ptSActorMovement->MoveTo(placeDest, flSpeed, 0.0f, TAG_ACTOR_ANIMATION_INVALID, uMoveFlags, 0.0f, ACDID_INVALID, ptPower->snoPower, 0, -1, 0.0f, NULL);
            }


            if (bDoAnimation)
            {
                AnimPlayerPlayAnimation(tACDMirrorImage, ptPower->dwAnimationTag, ANIM_USE_ANIM_LENGTH, FALSE, ANIM_REASON_POWER, 0.5f, FALSE, 1.0f);
            }
        }


        aAngle = Math::NormalizeAngle(aAngle + FULL_CIRCLE / (nCount + 1));
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);

    ANNDataMessage tMsg;
    tMsg.ann = ANNFromACD(ptPower->idACD);


    MessageSendToClient(listDestPlayers, SMSG_CANCEL_ACD_TARGET, &tMsg, sizeof(tMsg));

    if (ePetType != PETTYPE_NECROMANCER_SIMULACRUM)
    {

        MessageSendToClient(listDestPlayers, SMSG_RESET_ACD_EFFECTS, &tMsg, sizeof(tMsg));
    }

    return &tArrACDs;
}

// X/Server/SPlayer.cpp:7139 (not declared in src SPlayer.h; same local declaration as Powers.cpp:80)
void SPlayerUpdateRechargeTimer(ACDID idACDPlayer, SNO snoPower, float flTimeReduction);

#line 16154
void PowerUpdateRechargeTimer(
    VACDID idPlayer,
    SNO snoPower,
    float flTimeReduction)
{
    SPlayerUpdateRechargeTimer(idPlayer, snoPower, flTimeReduction);
}

// nAngleIncrementBase/Delta values from their DWARF const locations (0x32, 0x14); Rand_Internal's nDelta is 19 (Random.h const), so the draw is written as nAngleIncrementDelta - 1 (the original likely used a Rand macro not in src).

#line 15124
void PowerDropLootRunProgressGlobeOnActor(
    ACDID idDropper,
    ACDID idLootee,
    BOOL bForGoblin)
{
    ActorCommonData * ptACDDropper = ACDTryToGet(idDropper);
    if (!ptACDDropper)
        return;




    GBID gbidItem = StringToGBID(GameIsWeeklyChallenge() ? "Challenge_Rifts_Orb" : "p1_tiered_rifts_Orb");
    if (gbidItem == GBID_NONE)
        return;




    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();


    tSpecifier.hGBItem.eType = GB_ITEMS;
    tSpecifier.hGBItem.gbid = gbidItem;


    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
        return;






    tSpecifier.eIQL = IQL_NORMAL; tSpecifier.nAffixLevel = 1;
    tSpecifier.fAutoIdentify = TRUE;

    ScopedGlobalDefinitionLockR pdefGlobals;

    int32 nNumGlobesToDrop = bForGoblin ? pdefGlobals->ptServerData->nLootRunGoblinProgressGlobeCount : 1;

    Angle aYaw = RandAngle(SGameGlobalsGet().rng);
    float flOffset = pdefGlobals->ptServerData->flLootRunProgressGlobeDropRadiusMin;

    const int nAngleIncrementBase = 50;
    const int nAngleIncrementDelta = 20;

    for (int i = 0; i < nNumGlobesToDrop; i++)
    {
        aYaw = aYaw + Math::DegToAngle((float)nAngleIncrementBase) + Math::DegToAngle((float)Rand_Internal(SGameGlobalsGet().rng, nAngleIncrementDelta - 1));
        flOffset += RandFloat_Internal(SGameGlobalsGet().rng) * pdefGlobals->ptServerData->flLootRunProgressGlobeDropRadiusDelta;
        Vector3D vPosOffset(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
        vPosOffset *= flOffset;

        uint32 dwGameEventNumber = SGameGetEventNumber();
        XPooledDataIDList listSpawnedItemACDs(MemoryPoolsGetDATAIDListPool());
        FlippiesCreateOnActor(*ptACDDropper, idLootee, arLootSpecifiersToSpawn, 0, dwGameEventNumber, 0.0f, idLootee, NULL, &vPosOffset, LC_UNKNOWN, &listSpawnedItemACDs);

        POS pos = listSpawnedItemACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACDGlobe = (ACDID)listSpawnedItemACDs.GetNext(pos);
            ActorCommonData * ptACDGlobe = ACDTryToGet(idACDGlobe);
            if (!ptACDGlobe)
                continue;



            ptACDGlobe->AttributesSetInt(ATTRIBUTE_SPAWNED_BY_ACDID, idDropper);
            ptACDGlobe->AttributesSetInt(ATTRIBUTE_ITEM_IGNORES_PICKUP_RADIUS_FOR_PICKUP, TRUE);

            ACDPlayEffect(ptACDGlobe->id, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GameIsWeeklyChallenge() ? GLOBALSNO_EFFECT_GROUP_CHALLENGE_RIFT_ORB_FLIPPY_IN_AIR : GLOBALSNO_EFFECT_GROUP_TIERED_LOOT_RUN_ORB_FLIPPY_IN_AIR), ACDID_INVALID);
        }
    }
}

// EPSILON (MathGeneral.h) is the 1e-6 constant added at line 2590 (original rodata 0x7100e3bea0).
// Inferred: bSeparated = FALSE at 2628 (w22 cleared after SetAt_Tracked); the == 64 break shares line 2638 (cmp #0x40 / b.eq rows carry 2638:30); arActorIDs holds 64 ids (LuaScript.h).
// NON_MATCHING: 980 bytes, 14/245 insns: only x23/x24 swapped (idACD2 vs the hoisted ThreadGlobals GOT pointer). The kept ptPower NULL test came from the validate order (Count() first); the inner world test is a continue (2619).
#line 2566
ActorIDArray * PowerGetSeparatedTargets(
    int32 nTargetAreaID,
    float flSeparationDist)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);



    if (!sPowerValidateTargetArea(ptPower, nTargetAreaID))
    {
        ActorIDArray & tEmptyArrACDs = LuaScriptGetThreadSafeActorIDArray();
        tEmptyArrACDs.nNumActorIDs = 0;
        return &tEmptyArrACDs;
    }

    PowerAPIChooseTargets * ptChooseTargets = sPowerGetChooseTargetsAtIndex(ptPower, nTargetAreaID);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(*ptChooseTargets, listTargets);

    ActorIDArray & tArrACDs = LuaScriptGetThreadSafeActorIDArray();
    tArrACDs.nNumActorIDs = 0;
    tArrACDs.bWantAddRef = FALSE;

    float flSeparationDistSq = flSeparationDist * flSeparationDist + EPSILON;
    XGrowableMap<ACDID, ACDID, int, int> arSeparatedActors;

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACD1 = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACD1 = ACDTryToGet(idACD1);
        if (!ptACD1)
            continue;



        WorldPlace placeACD1 = ptACD1->GetPlace();

        BOOL bSeparated = TRUE;
        arSeparatedActors.Lookup(idACD1, bSeparated);

        POS posDistCheck = pos;
        while (posDistCheck)
        {
            ACDID idACD2 = (ACDID)listTargets.GetNext(posDistCheck);
            ActorCommonData * ptACD2 = ACDTryToGet(idACD2);
            if (!ptACD2)
                continue;



            WorldPlace placeACD2 = ptACD2->GetPlace();
            if (placeACD1.idSWorld != placeACD2.idSWorld)  // 2619: condition at column 17; flDistanceSq (2624) shares placeACD2's block.
                continue;



            float flDistanceSq = Vector3D::DistanceSquared(placeACD1.wp, placeACD2.wp);
            if (flDistanceSq <= flSeparationDistSq)
            {
                arSeparatedActors.SetAt_Tracked(idACD2, FALSE);
                bSeparated = FALSE;
            }

        }
        if (!bSeparated)
            continue;



        tArrACDs.arActorIDs[tArrACDs.nNumActorIDs] = idACD1;
        tArrACDs.nNumActorIDs++; if (tArrACDs.nNumActorIDs == 64) break;
    }






    return &tArrACDs;
}

// sInventorySlotTypeToPetType: external, not declared inline (DW_INL_inlined); its out-of-line copy is absent from the binary (linker gc). Case table read from the original lookup table at 0x7100e5d090.
#line 7945
PetType sInventorySlotTypeToPetType(InventorySlotType eInventorySlot)
{
    switch (eInventorySlot)
    {
    case SLOT_PLAYER_HEAD: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_HEAD;
    case SLOT_PLAYER_TORSO: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_TORSO;
    case SLOT_PLAYER_RIGHT_HAND: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_RIGHT_HAND;
    case SLOT_PLAYER_LEFT_HAND: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_LEFT_HAND;
    case SLOT_PLAYER_HANDS: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_HANDS;
    case SLOT_PLAYER_WAIST: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_WAIST;
    case SLOT_PLAYER_FEET: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_FEET;
    case SLOT_PLAYER_SHOULDERS: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_SHOULDERS;
    case SLOT_PLAYER_LEGS: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_LEGS;
    case SLOT_PLAYER_BRACERS: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_BRACERS;
    case SLOT_PLAYER_RIGHT_FINGER: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_RIGHT_FINGER;
    case SLOT_PLAYER_LEFT_FINGER: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_LEFT_FINGER;
    case SLOT_PLAYER_NECK: return PETTYPE_ITEM_PROC_PET_SLOT_PLAYER_NECK;
    case SLOT_HIRELING_RIGHT_HAND: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_RIGHT_HAND;
    case SLOT_HIRELING_LEFT_HAND: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_LEFT_HAND;
    case SLOT_HIRELING_SPECIAL: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_SPECIAL;
    case SLOT_HIRELING_NECK: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_NECK;
    case SLOT_HIRELING_RIGHT_FINGER: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_RIGHT_FINGER;
    case SLOT_HIRELING_LEFT_FINGER: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_LEFT_FINGER;
    case SLOT_HIRELING_HEAD: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_HEAD;
    case SLOT_HIRELING_TORSO: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_TORSO;
    case SLOT_HIRELING_HANDS: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_HANDS;
    case SLOT_HIRELING_WAIST: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_WAIST;
    case SLOT_HIRELING_FEET: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_FEET;
    case SLOT_HIRELING_SHOULDERS: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_SHOULDERS;
    case SLOT_HIRELING_LEGS: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_LEGS;
    case SLOT_HIRELING_BRACERS: return PETTYPE_ITEM_PROC_PET_SLOT_HIRELING_BRACERS;
    default: return PETTYPE_NONE;
    }
}


PetType PowerGetPetTypeFromItemSlot()

{
    PowerID idPower = sGetCurrentPower();
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    SNO snoPower = PowerGetSNO(idPower);
    if (!ptACD || snoPower == SNO_NONE)
        return PETTYPE_NONE;
    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_ITEM_POWER_PASSIVE, snoPower);
    if (!ptACD->AttributesGetInt(tKey))
        return PETTYPE_NONE;
    if (ACDIsHireling(*ptACD))
    {
        static const InventorySlotType sgePaperDollSlots[] = {
            SLOT_HIRELING_LEFT_HAND,
            SLOT_HIRELING_RIGHT_HAND,
            SLOT_HIRELING_SPECIAL,
            SLOT_HIRELING_LEFT_FINGER,
            SLOT_HIRELING_RIGHT_FINGER,
            SLOT_HIRELING_NECK,
            SLOT_HIRELING_HEAD,
            SLOT_HIRELING_TORSO,
            SLOT_HIRELING_HANDS,
            SLOT_HIRELING_WAIST,
            SLOT_HIRELING_FEET,
            SLOT_HIRELING_SHOULDERS,
            SLOT_HIRELING_LEGS,
            SLOT_HIRELING_BRACERS,
            SLOT_NONE
        };



        for (int32 nIndex = 0; sgePaperDollSlots[nIndex] != SLOT_NONE; ++nIndex)
        {

            InventoryLocation tInvLocSlot;
            ACDInventoryLocationSet(tInvLocSlot, ptACD->id, sgePaperDollSlots[nIndex], 0, 0);
            ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
            ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
            if (ptACDItemInSlot && ptACDItemInSlot->AttributesGetInt(tKey))
                return sInventorySlotTypeToPetType(sgePaperDollSlots[nIndex]);
        }




    }
    else if (ACDIsPlayer(ptACD->id))
    {
        static const InventorySlotType sgePaperDollSlots[] = {
            SLOT_PLAYER_HEAD,
            SLOT_PLAYER_TORSO,
            SLOT_PLAYER_LEFT_HAND,
            SLOT_PLAYER_RIGHT_HAND,
            SLOT_PLAYER_HANDS,
            SLOT_PLAYER_WAIST,
            SLOT_PLAYER_FEET,
            SLOT_PLAYER_SHOULDERS,
            SLOT_PLAYER_LEGS,
            SLOT_PLAYER_BRACERS,
            SLOT_PLAYER_NECK,
            SLOT_PLAYER_LEFT_FINGER,
            SLOT_PLAYER_RIGHT_FINGER,
            SLOT_NONE
        };



        for (int32 nIndex = 0; sgePaperDollSlots[nIndex] != SLOT_NONE; ++nIndex)
        {

            InventoryLocation tInvLocSlot;
            ACDInventoryLocationSet(tInvLocSlot, ptACD->id, sgePaperDollSlots[nIndex], 0, 0);
            ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
            ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
            if (ptACDItemInSlot && ptACDItemInSlot->AttributesGetInt(tKey))
            {
                return sInventorySlotTypeToPetType(sgePaperDollSlots[nIndex]);
            }
        }
    }





    return PETTYPE_NONE;
}

// X/Common/Marker/Marker.cpp:204 (no src header declares it; same local declaration as SpellEffect.cpp:325)
uint32 MarkerStringHash(LPCSTR pszName);
// Inferred (PowerCreateProxyActorOnCursor): bExists = snoProxyActor != SNO_NONE (8123; only the 8141 test on snoProxyActor survives); the 100000.0f bound is the original rodata value at 0x7100e3c104 (column room suggests a 17-character named constant not found in src).

#line 8106
ACDID PowerCreateProxyActorOnCursor(
    LPCSTR szProxyActorName,
    int32 nCursorID,
    float flDuration,
    BOOL bUseExactLocation)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    ACDID idACDProxy = ACDID_INVALID;

    if (!sPowerValidateCursor(ptPower, nCursorID))
        return idACDProxy;



    SNO snoProxyActor = SNOFileLookupGUID(ACTOR_SNO, szProxyActorName);
    BOOL bExists = snoProxyActor != SNO_NONE;

















    if (bExists)
    {
        PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
        if (!ptCursor)
            return idACDProxy;



        if (Math::Abs(ptCursor->tPlace.wp.x) >= 100000.0f ||
            Math::Abs(ptCursor->tPlace.wp.y) >= 100000.0f ||
            Math::Abs(ptCursor->tPlace.wp.z) >= 100000.0f)
            return idACDProxy;








        ActorAddStruct tActorAdd;
        tActorAdd.snoActor            = snoProxyActor;
        tActorAdd.dwActorAddFlags     = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE) | (1 << ACTORADD_KEEP_MARKER_NAME);
        tActorAdd.eFastAttribType     = FAT_SIMPLE;
        tActorAdd.eTeam               = PowerGetTeam(idPower);
        tActorAdd.tPlace = ptCursor->tPlace;
        tActorAdd.qOrientation = ptCursor->qOrientation;

        CRefString szProxyName;
        szProxyName.Format("%s (%s)", szProxyActorName, SNOToString(POWER_SNO, ptPower->snoPower).str());
        tActorAdd.tMarkersName = MarkerStringHash(szProxyName);

        ScopedActorDefinitionLockR pdefActor(snoProxyActor);
        float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
        float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);

        tActorAdd.flScale = RandRangeFloat_Internal(SGameGlobalsGet().rng, flMinScale, flDeltaScale);
        tActorAdd.tSNOName          = SNOName(ACTOR_SNO, snoProxyActor);

        if (bUseExactLocation)
        {
            tActorAdd.dwActorAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK) |
                                         (1 << ACTORADD_DONT_PERTURB) |
                                         (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
        }


        ActorID idActorProxy = ActorAdd(tActorAdd);
        if (idActorProxy == ACTORID_INVALID)
            return ACDID_INVALID;



        idACDProxy = ACDFindByActor(idActorProxy);
        if (idACDProxy == ACDID_INVALID)
            return ACDID_INVALID;



        ActorCommonData & tACDProxy = ACDGet(idACDProxy);
        tACDProxy.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
        ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_IS_POWER_PROXY, TRUE);

        if (flDuration >= FLT_EPSILON)
        {
            uint32 dwDuration = Math::FloatToInt(flDuration * TICKS_PER_SECOND);

            ActorDeleteSoon(idActorProxy, dwDuration);
        }


        ActorCommonData * ptACD = PowerGetOwner(idPower);
        if (ptACD && ACDIsPlayer(ptACD->id))
        {

            int32 nRuneALevel = FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_A, ptPower->snoPower));
            int32 nRuneBLevel = FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_B, ptPower->snoPower));
            int32 nRuneCLevel = FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_C, ptPower->snoPower));
            int32 nRuneDLevel = FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_D, ptPower->snoPower));
            int32 nRuneELevel = FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_E, ptPower->snoPower));
            tACDProxy.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, ptPower->snoPower), nRuneALevel);
            tACDProxy.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, ptPower->snoPower), nRuneBLevel);
            tACDProxy.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, ptPower->snoPower), nRuneCLevel);
            tACDProxy.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, ptPower->snoPower), nRuneDLevel);
            tACDProxy.AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, ptPower->snoPower), nRuneELevel);
        }

    }

    return idACDProxy;
}

#include "../../Common/Nav/NavCellPath.h"
#include "../Network/KnownList.h"
// X/Common/Powers/PowersCommon.cpp:3892 (not declared in a src header; same local declaration as PowerHelper.cpp:54)
BOOL PowerGetCollFlags(AutoPowerDefinitionR pdefPower, PowerPassabilityType ePassability, ActorCollisionFlags & tCollFlags);
// PowerTeleportActorToCursor: TELEPORT_SEARCH_RADIUS_MAX defined on its DWARF line (13873). Inferred: the 13977 scope is a bare block (DWARF lexical block 13978-13999, no condition in the code); FindPath flags 1/0 and uSpecialFlags |= 1 follow PowerHelper.cpp:2215.
#line 13873
const float TELEPORT_SEARCH_RADIUS_MAX = 10.0f;

void PowerTeleportActorToCursor(
    VACDID idActor,
    int32 nCursorID,
    BOOL bDoRollout,
    BOOL bCheckForPath,
    float flCameraSmoothingSeconds,
    BOOL bSnapToGround)
{
    PowerID idPower = sGetCurrentPower();
    Power * ptPower = PowerGet(idPower);

    PowerCursor * ptCursor = sPowerGetCursorAtIndex(ptPower, nCursorID, TRUE);
    if (!ptCursor)
        return;



    ActorCommonData * ptACD = ACDTryToGet(idActor);
    if (!ptACD)
        return;



    if (ptACD->IsGizmo())
        return;



    ActorID idActorOwner = (ActorID)ptACD->idOwner;
    WorldPlace tPlaceWarp = ptCursor->tPlace;

    WorldPlace placeCurr = ptACD->GetPlace();
    if (placeCurr.idSWorld != tPlaceWarp.idSWorld)
        return;



    Actor * ptActor = ActorTryToGet(idActorOwner);

    float flRadius = ptACD->GetRadius();





    SActorMovement * ptMove = ptActor->ptSActorMovement;
    if (!ptMove)
        return;




    if (bDoRollout)
    {
        ActorCollisionFlags tDestFlags;
        BOOL bHasFlags = PowerGetCollFlags(ptPower->snoPower, TRUE, tDestFlags);
        if (!bHasFlags)
            return;





        ActorCollisionFlags tCollFlags;
        ACDCollParams tACDCollParams;
        if (PowerGetCollFlags(ptPower->snoPower, FALSE, tCollFlags))
        {
            tACDCollParams.uObjectNoOverlapFlags = tCollFlags.uDynamicFlagsNoOverlap;
        }
        else
        {

            ACDCollSettings tCollSettings;
            ACDCollGetSettings(ptActor->snoActor, tCollSettings);
            tACDCollParams.uObjectNoOverlapFlags = tCollSettings.uDynamicFlagsNoOverlap;
        }
        tACDCollParams.uSpecialFlags |= 1;
        tACDCollParams.eTeam = ptACD->GetTeam();

        BOOL bDoComputeDest = TRUE;

        if (bCheckForPath)
        {
            PathPoint2DList listPathPoints;
            listPathPoints.SetPool(NavCellPathGetPointPool());

            PathResult ePathResult = ptMove->FindPath(tPlaceWarp, listPathPoints, 1, 0, &tACDCollParams);
            if (ePathResult == PATH_RESULT_FOUND || ePathResult == PATH_RESULT_NOT_NECESSARY)
            {
                bDoComputeDest = FALSE;
            }

        }

        if (bDoComputeDest)
        {
            PowerComputeDestination(ptPower->snoPower, *ptMove, tPlaceWarp, tPlaceWarp, TRUE, TELEPORT_SEARCH_RADIUS_MAX);
        }
    }



    {
        int32 nCameraSmoothingTicks = Math::FloatRoundToInt(flCameraSmoothingSeconds * TICKS_PER_SECOND);
        nCameraSmoothingTicks = Math::Max(nCameraSmoothingTicks, 0);

        ptMove->ActivateTriggersToDestWithPathFind(tPlaceWarp);
        ptMove->SnapToNewPlace(tPlaceWarp, bSnapToGround, FALSE, TRUE, nCameraSmoothingTicks);
        ptMove->StopAndSyncPosition(TRUE, FALSE);


        Angle aFacing = ptCursor->qOrientation.ToYaw();
        ptMove->SetNewFacing(aFacing, TRUE, TRUE);



        Player * ptPlayer = PlayerGetByACD(ptACD->id);
        if (ptPlayer)
        {
            KnownListForceUpdate(*ptPlayer);
        }

        ptACD->PickupNearby();

        PowerAPI_BuffApplyOnTeleportedEffects(ptACD->id);
    }
}
