// Original file: X/Client/Targeting/ConsoleTargeting.cpp

#include "ConsoleTargeting.h"
#include "../CPlayer.h"
#include "../CPlayerInput.h"
#include "../Movement/CActorMovement.h"
#include "../Console/ControllerUtils.h"
#include "../RActors/RActors.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/NPC.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../Camera/CameraManager.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Server/Powers/PowerHelper.h"
#include "../../Common/Gizmo.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include <float.h>
#include "../CPlayerUtil.h"
BOOL WorldPointToScreenPointUICUnclipped(const Vector3D & wp, Vector2D & vecUIC, int32 nCameraIndex);  // signature from the call in ReduceWithCone (DWARF-less callee _Z35WorldPointToScreenPointUICUnclippedRK8Vector3DR8Vector2Di, X/Client/Camera/CameraManager.cpp)
BOOL PowerIsCloseEnough(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIgnoreRange);  // signature from the call in SetGizmoTarget (callee _Z18PowerIsCloseEnough5ACDID15AutoDefinitionRI15PowerDefinitionEPK6Targeti, X/Common/Powers/PowersCommon.cpp:2149)
float PowerGetOffsetFromTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // signature from the call in TargetSetToActor (callee _Z24PowerGetOffsetFromTargetRK15ActorCommonData15AutoDefinitionRI15PowerDefinitionE, X/Common/Powers/PowersCommon.cpp:2407)
BOOL ItemIsAutoPickup(const ActorCommonData & tACDItem);  // X/Common/Items/Items.cpp (as declared in CPlayerUtil.cpp:56; callee _Z16ItemIsAutoPickupRK15ActorCommonData)
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD);  // X/Common/Items.cpp:2208
PickupResponse ACDInventoryCanPickup(const ActorCommonData & tACDPickee, const ActorCommonData & tACDItem, BOOL bTryStashFirst, PickupResult * ptResult, BOOL bTryStashOnly, BOOL bAllowAutoEquip);  // X/Common/ActorInventory.cpp:10712
BOOL RActorIsHidden(const RActor * ptRActor);  // X/Client/RActors/RActors.cpp
BOOL PowerCastTargetNecroCorpsesOnly(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp:1188 (no header declares it)
namespace Targeting { namespace Console { void GetTargetingInformation(const TargetingInformationInput & input, TargetingInformationOutput & output); } }  // defined at line 1112, called from ResolveTargetForPlayer (lines 863, 1011)
BOOL PowerPlayerAttackOnlyOncePerClick(AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp:2283 (no header declares it)
BOOL PowerAutoTargets(AutoPowerDefinitionR pdefPower, const Player * ptPlayer);  // X/Common/Powers/PowersCommon.cpp:1775 (no header declares it)
BOOL PowerPrefersRadialTargeting(AutoPowerDefinitionR pDefPower, const Player * ptPlayer);  // X/Common/Powers/PowersCommon.cpp:1813 (no header declares it)
void RActorsPlayerTargetCull(const Sphere & tSphereClose, RActorIDListPooled & listActorsClose, const Sphere & tSphereCone, const Vector2D & wpConeStart, const Vector2D & wpConeDir, Angle aRequiredTheta, float flSearchDist, RActorIDListPooled & listActorsCone, const RActorID idIgnore);  // X/Client/RActors/RActorCollision.cpp:1058
void RActorsIntersectSphere(const Sphere & tSphere, const RActorID idIgnore, RActorIDListPooled & listActors);  // X/Client/RActors/RActorCollision.cpp:1041
#include "../../Sigma/MemoryPools.h"



namespace Targeting { namespace Console {
struct TargetPriority
{
    GizmoType eGizmoType;
    uint32 bNearby : 1;
    uint32 bCone : 1;
    uint32 bCheckForInventoryFull : 1;
    uint32 bCheckForNPCs : 1;
    uint32 bOperableGizmo : 1;
    uint32 bTriggersTrap : 1;
};


static const TargetPriority targetGizmoPrioritiesOverMonsters[] =
{
    { ACTORGIZMO_HEALING_WELL,       1, 0, 0, 0, 1, 0 },
    { ACTORGIZMO_POOL_OF_REFLECTION, 1, 0, 0, 0, 1, 0 },
    { ACTORGIZMO_POWERUP,            1, 0, 0, 0, 1, 0 },
    { ACTORGIZMO_DOOR,               1, 0, 1, 0, 1, 1 },
    { ACTORGIZMO_CHEST,              1, 0, 1, 0, 1, 1 },
};

static const TargetPriority targetGizmoHighPriorities[] =
{
    { ACTORGIZMO_ITEM_INTERNAL,      1, 0, 1, 0, 1, 0 },
    { ACTORGIZMO_UNDEFINED,          1, 0, 0, 1, 1, 0 },
    { ACTORGIZMO_PLACED_LOOT,        1, 0, 1, 0, 1, 0 },
    { ACTORGIZMO_TOWN_PORTAL,        1, 0, 1, 0, 1, 0 },
    { ACTORGIZMO_WAYPOINT,           1, 0, 1, 0, 1, 0 },
    { ACTORGIZMO_DOOR,               1, 0, 1, 0, 1, 0 },
    { ACTORGIZMO_UNDEFINED,          1, 0, 1, 0, 1, 0 },
};

static const TargetPriority targetGizmoLowPriorities[] =
{
    { ACTORGIZMO_BREAKABLEDOOR,      1, 1, 1, 0, 0, 0 },
    { ACTORGIZMO_BREAKABLECHEST,     1, 1, 1, 0, 0, 0 },
    { ACTORGIZMO_UNDEFINED,          0, 1, 0, 1, 1, 0 },
    { ACTORGIZMO_ITEM_INTERNAL,      1, 0, 0, 0, 1, 0 },
};


void ReduceWithCone(
    RActorIDListPooled & listConeRActors,
    const ActorCommonData & tACDPlayer,
    const Player * ptPlayer,
    Angle aRequiredTheta,
    float flSearchDist,
    WorldPlace wpConeStart)
{
    WorldPlace wpPlayer = tACDPlayer.GetPlace();
    float flCosRequiredTheta = Math::Cos(aRequiredTheta);
    const Vector2D vLStickWorldDirection = ptPlayer->tTargetComponent.GetTargetingDirection();

    Rect2D rectViewportUIC;
    CGameGetLetterboxedViewportUIC(rectViewportUIC);
    rectViewportUIC.UniformScale(1.15f);

    POS pos = listConeRActors.GetHeadPosition();
    while (pos != NULL)
    {
        POS oldPos = pos;
        RActorID idRActor = (RActorID)listConeRActors.GetNext(pos);
        RActor * pRActor = RActorGet(idRActor);
        if (pRActor->idACD == ACDID_INVALID)
        {
            listConeRActors.RemoveAt(oldPos);
            continue;
        }

        const ActorCommonData * ptACD = ACDTryToGet(pRActor->idACD);
        if (!ptACD)
        {
            listConeRActors.RemoveAt(oldPos);
            continue;
        }

        WorldPlace wpTarget = ptACD->GetPlace();
        float flDistSq = Vector3D::DistanceSquared(wpTarget.wp, wpPlayer.wp);
        float flRadius = ptACD->GetCollSettings().GetCyl().wdRadius;
        float flCheckDist = flRadius + flSearchDist;

        if (flDistSq > flCheckDist * flCheckDist)
        {
            listConeRActors.RemoveAt(oldPos);
            continue;
        }

        Vector2D vToTargetFromIntersection = wpTarget.wp.As2D() - wpConeStart.wp.As2D();
        Vector2D vToTargetFromPlayer = wpTarget.wp.As2D() - wpPlayer.wp.As2D();
        vToTargetFromIntersection.Normalize();
        vToTargetFromPlayer.Normalize();
        float cosThetaFromIntersection = Vector2D::DotProduct(vLStickWorldDirection, vToTargetFromIntersection);
        float cosThetaFromPlayer = Vector2D::DotProduct(vLStickWorldDirection, vToTargetFromPlayer);

        if (aRequiredTheta != FULL_CIRCLE && (cosThetaFromIntersection < flCosRequiredTheta || cosThetaFromPlayer < FLT_EPSILON))
        {
            listConeRActors.RemoveAt(oldPos);
            continue;
        }


        Vector2D wpUIC;
        if (!WorldPointToScreenPointUICUnclipped(wpTarget.wp, wpUIC, -1))
        {
            listConeRActors.RemoveAt(oldPos);
            continue;
        }

        if (!rectViewportUIC.ContainsPoint(wpUIC))
        {
            listConeRActors.RemoveAt(oldPos);
            continue;
        }
    }
}
// NON_MATCHING: 1936 vs 1940 bytes; same calls and branch structure, but the selection tail (lines 287-350) if-converts the eps path
// and keeps flLowestHealth/flBestDist in stack slots where the original holds them in v10/v11 (6 tail shapes tried).
const ActorCommonData * GetHostileTargetFromList(
    RActorIDListPooled & listRActors,
    const PowerDefinition * pdefPower,
    const ActorCommonData & tACDPlayer,
    const Player * ptPlayer,
    Angle aRequiredTheta,
    Angle aPreferredTheta,
    float flSearchDist,
    const WorldPlace & wpConeStart,
    BOOL bUseTargetLockPriorities)
{
    const ActorCommonData * ptResult = NULL;
    Target tTarget;

    float flBestDist = FLT_MAX;
    float flBestCosTheta = -1.0f;

    float flLowestHealth = FLT_MAX;
    BOOL bFoundElite = FALSE;

    WorldPlace wpPlayer = tACDPlayer.GetPlace();

    float flCosRequiredTheta = Math::Cos(aRequiredTheta);
    float flCosPreferredTheta = Math::Cos(aPreferredTheta);

    BOOL bExcludeOutOfRangeTargets = (BOOL)PowerGetFormulaValue(&tACDPlayer, AutoPowerDefinitionR(pdefPower), TAG_POWER_CONSOLE_LT_FIND_TARGET_IF_LOCKED_TARGET_OUT_OF_RANGE, -1, -1, 0);
    BOOL bInAimMode = ptPlayer->tTargetComponent.GetInAimMode();
    const Vector2D vLStickWorldDirection = ptPlayer->tTargetComponent.GetTargetingDirection();

    Rect2D rectViewportUIC;
    CGameGetLetterboxedViewportUIC(rectViewportUIC);
    rectViewportUIC.UniformScale(1.15f);

    BOOL fPowerCastTargetAllies = PowerCastTargetAllies(AutoPowerDefinitionR(pdefPower));
    BOOL fPowerCastTargetNeutral = PowerCastTargetNeutral(AutoPowerDefinitionR(pdefPower));
    BOOL fPowerCastTargetNecroCorpsesOnly = PowerCastTargetNecroCorpsesOnly(tACDPlayer, AutoPowerDefinitionR(pdefPower));

    POS pos = listRActors.GetHeadPosition();
    while (pos != NULL)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * pRActor = RActorGet(idRActor);
        const ActorCommonData * ptACD = ACDTryToGet(pRActor->idACD);


        if (fPowerCastTargetNecroCorpsesOnly)
        {
            if (!ptACD->IsNecroCorpse())
                continue;
        }
        else
        {
            if (!ACDIsHostile(tACDPlayer, *ptACD, FALSE) && !fPowerCastTargetAllies && !fPowerCastTargetNeutral)
                continue;
        }
        // Inference (line 216): DistanceSquared(target, player) argument order; the reverse order changes how clang shares
        // the subtraction with vToTargetFromPlayer (same as ReduceWithCone line 116).
        // Inference (line 217): radius read through GetCollSettings().GetCyl(), the accessors the DWARF records inlined here.

        WorldPlace wpTarget = ptACD->GetPlace();
        float flDistSq = Vector3D::DistanceSquared(wpTarget.wp, wpPlayer.wp);
        float flRadius = ptACD->GetCollSettings().GetCyl().wdRadius;
        float flCheckDist = flRadius + flSearchDist;

        if (flDistSq > flCheckDist * flCheckDist)
            continue;



        Vector2D vToTargetFromIntersection = wpTarget.wp.As2D() - wpConeStart.wp.As2D();
        Vector2D vToTargetFromPlayer = wpTarget.wp.As2D() - wpPlayer.wp.As2D();
        vToTargetFromIntersection.Normalize();
        vToTargetFromPlayer.Normalize();
        float cosThetaFromIntersection = Vector2D::DotProduct(vLStickWorldDirection, vToTargetFromIntersection);
        float cosThetaFromPlayer = Vector2D::DotProduct(vLStickWorldDirection, vToTargetFromPlayer);

        if (aRequiredTheta != FULL_CIRCLE && (cosThetaFromIntersection < flCosRequiredTheta || cosThetaFromPlayer < FLT_EPSILON))
            continue;



        if (cosThetaFromPlayer < FLT_EPSILON)
        {
            flDistSq *= 2.0f;
        }


        Vector2D wpUIC;
        if (!WorldPointToScreenPointUICUnclipped(wpTarget.wp, wpUIC, -1))
            continue;



        if (!rectViewportUIC.ContainsPoint(wpUIC))
            continue;




        if (aPreferredTheta > 0.0f)
        {
            BOOL bPreferred = cosThetaFromIntersection > flCosPreferredTheta;
            BOOL bBestPreferred = flBestCosTheta > flCosPreferredTheta;

            if (bPreferred && !bBestPreferred)
                flBestDist = FLT_MAX;

            if (bBestPreferred && !bPreferred)
                continue;
        }




        tTarget.InitializeFromACD(ptACD->id, FALSE, FALSE);
        if (!PowerCanTarget(AutoPowerDefinitionR(pdefPower), tACDPlayer.id, &tTarget))
            continue;
        // Inference (lines 276-278): the call sits on line 278 with the two flag tests on 276; PowerIsCloseEnough takes
        // an AutoPowerDefinitionR built from pdefPower->tHeader.snoHandle (an SNO), not from pdefPower itself.

        if (bExcludeOutOfRangeTargets && bInAimMode &&

            !PowerIsCloseEnough(tACDPlayer.id, AutoPowerDefinitionR(pdefPower->tHeader.snoHandle), &tTarget, 0.0f, 0.0f, NULL, FALSE))
            continue;
        // Inference (line 285): ATTRIBUTE_HITPOINTS_CUR / ATTRIBUTE_HITPOINTS_MAX keys from the AttributesGetFloat arguments;
        // the (int32) casts follow the float-to-int-to-float conversions in the original.

        if (bUseTargetLockPriorities)
        {
            float flHealth = (float)(int32)ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_CUR) / (float)(int32)ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX);

            if (ptACD->eMonsterRarity != MR_NORMAL)
            {
                if (!bFoundElite)
                {
                    bFoundElite = TRUE;
                    flLowestHealth = flHealth;
                    ptResult = ptACD;
                    flBestDist = flDistSq;
                    flBestCosTheta = cosThetaFromIntersection;
                }



                else if (flHealth < flLowestHealth)
                {
                    flLowestHealth = flHealth;
                    ptResult = ptACD;
                    flBestDist = flDistSq;
                    flBestCosTheta = cosThetaFromIntersection;
                }
                // Inference (lines 287-350): the original has no line rows for the selection stores, so where they sit
                // is not recorded; this per-branch form was the closest of the six shapes tried.


            }
            else if (!bFoundElite)
            {




                if (flHealth < flLowestHealth)
                {
                    flLowestHealth = flHealth;
                    ptResult = ptACD;
                    flBestDist = flDistSq;
                    flBestCosTheta = cosThetaFromIntersection;
                }

                else if (flHealth - flLowestHealth < EPSILON &&

                         flDistSq <= flBestDist)
                {
                    ptResult = ptACD;
                    flBestDist = flDistSq;
                    flBestCosTheta = cosThetaFromIntersection;
                }

            }
        }
        else
        {




            if (flDistSq <= flBestDist)
            {
                ptResult = ptACD;
                flBestDist = flDistSq;
                flBestCosTheta = cosThetaFromIntersection;
            }
        }
    }


    return ptResult;
}



const ActorCommonData * GetGizmoTargetFromList(
    const ActorCommonData & tACDPlayer,
    TargetPriority tTargetType,
    RActorIDListPooled & tList,
    Angle aSearchAngle,
    const WorldPlace & wpConeStart)
{
    const ActorCommonData * ptResult = NULL;
    Target tTarget;

    float flBestDist = FLT_MAX;
    int32 nHighestPriority = -1;

    WorldPlace wpPlayer = tACDPlayer.GetPlace();

    float flCosSearchAngle = Math::Cos(aSearchAngle);

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);

    POS pos = tList.GetHeadPosition();
    while (pos != NULL)
    {
        RActorID idRActor = (RActorID)tList.GetNext(pos);
        RActor * pRActor = RActorGet(idRActor);
        const ActorCommonData * ptACD = ACDTryToGet(pRActor->idACD);
        if (!ptACD)
            continue;



        GizmoType eType = ptACD->GetGizmoType();
        // Inference (lines 393, 394): Normalize order follows the first sqrt in the code, which works on the wpConeStart-based vector.
        WorldPlace wpTarget = ptACD->GetPlace();
        Vector3D vToTargetFromPlayer = wpTarget.wp - wpPlayer.wp;
        Vector3D vToTargetFromIntersection = wpTarget.wp - wpConeStart.wp;
        vToTargetFromIntersection.Normalize();
        vToTargetFromPlayer.Normalize();
        const Vector2D vLStickWorldDirection = ptPlayer->tTargetComponent.GetTargetingDirection();
        float cosThetaFromIntersection = Vector2D::DotProduct(vLStickWorldDirection, vToTargetFromIntersection.As2D());
        float cosThetaFromPlayer = Vector2D::DotProduct(vLStickWorldDirection, vToTargetFromPlayer.As2D());
        float flDistSq = Vector3D::Distance2DSquared(wpTarget.wp, wpPlayer.wp);

        if (aSearchAngle != FULL_CIRCLE)
        {
            if (cosThetaFromIntersection < flCosSearchAngle || cosThetaFromPlayer < FLT_EPSILON)
                continue;
        }



        {
            ScopedGlobalDefinitionLockR pdefGlobal;
            float minGizmoRange = pdefGlobal->flControllerAutoTargetWreckablesRange;
            if (flDistSq > minGizmoRange * minGizmoRange)
                continue;



            float flHeightDelta = Math::Abs(wpPlayer.wp.z - wpTarget.wp.z);
            if (flHeightDelta > pdefGlobal->flControllerAutoTargetWreckablesRangeHeight)
                continue;
        }





        if (aSearchAngle != FULL_CIRCLE)
        {

            Vector2D wpUIC;
            if (!WorldPointToScreenPointUIC(wpTarget.wp, wpUIC, -1))
                continue;
        }



        if (tTargetType.bCheckForNPCs)
        {

            if (!ptACD->IsOperatableNPC(ptPlayer, TRUE))
                continue;



            int32 nPlayerIndex = PlayerGetByACD(tACDPlayer.id)->nPlayerIndex;
            ConversationIcon eIcon = ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_CONVERSATION_ICON, nPlayerIndex));
            // Inference: icon values 0, 3, 4 come from the 0x19 bit mask at 0x312dac; ConversationIcon has no named values in the headers.
            if (eIcon == 0 || eIcon == 3 || eIcon == 4)
                continue;
        }


        else if (tTargetType.eGizmoType != -1 && eType != tTargetType.eGizmoType)
            continue;



        if (tTargetType.bTriggersTrap)
        {
            ScopedActorDefinitionLockR pdefActor(ptACD->GetActorSNO());

            float flPercentChanceToTriggerPower = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_CHEST_CHANCE_TO_TRIGGER_TRAP);
            if (flPercentChanceToTriggerPower < EPSILON)
                continue;
        }


        // Inference: the gizmo types are the set bits of the 0x6128022000203003 mask at 0x312e2c (value + 7), named from GizmoSwig.h.
        switch (eType)
        {
        case ACTORGIZMO_CHECKPOINT: case ACTORGIZMO_SIGN: case ACTORGIZMO_SCRIPT_PROP:
        case ACTORGIZMO_PORTAL_DESTINATION: case ACTORGIZMO_SPAWNER: case ACTORGIZMO_TRIGGER:
        case ACTORGIZMO_DESTROYABLE_OBJECT: case ACTORGIZMO_PRESSUREPLATE: case ACTORGIZMO_DESTROY_ON_PROXIMITY:
        case ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT: case ACTORGIZMO_SAVEPOINT: case ACTORGIZMO_SAVEPOINT_PORTAL:
            continue;
        }


        if (eType == ACTORGIZMO_CHEST || eType == ACTORGIZMO_LORE_CHEST)
        {
            ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
            if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHEST_OPERATES_ON_PROXIMITY) != 0)
                continue;
        }



        if (eType == ACTORGIZMO_ITEM_INTERNAL)
        {
            if (ItemIsAutoPickup(*ptACD))
                continue;



            if (tTargetType.bCheckForInventoryFull && ACDInventoryCanPickup(tACDPlayer, *ptACD, FALSE, NULL, FALSE, TRUE) != PICKUP_RESPONSE_OK)
                continue;
        }



        if (RActorIsHidden(pRActor) && !GizmoIsPortal(eType))
            continue;

        if (GizmoIsWreckable(eType))
        {
            if (tTargetType.bOperableGizmo)
                continue;
        }
        else if (tTargetType.bOperableGizmo == (ptACD->IsNecroCorpse() != FALSE))
            continue;

        if ((ptACD->IsOperatableGizmo(tACDPlayer.id, FALSE) && !GizmoIsWreckable(eType) && !ptACD->IsNecroCorpse()) || (ptACD->IsOperatableNPC(ptPlayer, TRUE) && tTargetType.bNearby))
        {
            Target tTargetScoped;
            tTargetScoped.InitializeFromACD(ptACD->id, FALSE, FALSE);

            if (!PowerIsCloseEnough(tACDPlayer.id, AutoPowerDefinitionR(GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO)), &tTargetScoped, FALSE))
                continue;
        }





        if (!ptACD->IsTargetable())
            continue;





        if (eType == ACTORGIZMO_ITEM_INTERNAL)
        {
            int32 nPriority = ItemGetDisplayedQualityLevel(*ptACD);
            if (nPriority < 3 &&
                (ItemIsSocketable(*ptACD) ||
                 ItemHasLabel(ptACD->hGB.gbid, 0x26) ||
                 ItemHasLabel(ptACD->hGB.gbid, 0x50) ||
                 ItemHasLabel(ptACD->hGB.gbid, 0x72) ||
                 ItemHasLabel(ptACD->hGB.gbid, 0x4c)))
                nPriority = 3;





            if (ItemIsCosmeticItem(ptACD->hGB.gbid))
                nPriority = 9;



            if (nPriority < nHighestPriority)
                continue;


            if (nPriority > nHighestPriority)
            {
                nHighestPriority = nPriority;
                flBestDist = FLT_MAX;
            }
        }


        if (cosThetaFromPlayer < FLT_EPSILON)
        {
            flDistSq *= 1.2f;
        }

        if (flDistSq > flBestDist)
            continue;

        flBestDist = flDistSq;
        ptResult = ptACD;
    }


    return ptResult;
}



const ActorCommonData * GetGizmoTargetFromLists(
    const ActorCommonData & tACDPlayer,
    TargetPriority tTargetType,
    RActorIDListPooled & tCloseRActors,
    RActorIDListPooled & tConeRActors,
    const TargetingInformationOutput & targetingInfo)
{
    const ActorCommonData * pResult = NULL;
    // Inference (line 591): ternary operand order from the fcsel in SetGizmoTarget (operable-gizmo targeting searches the full circle).
    if (tTargetType.bNearby)
    {
        float aSearchAngle = tTargetType.bOperableGizmo ? FULL_CIRCLE : targetingInfo.flControllerAutoTargetingCloseAngle;
        pResult = GetGizmoTargetFromList(tACDPlayer, tTargetType, tCloseRActors, aSearchAngle, targetingInfo.wpConeStart);
        if (pResult)
            return pResult;
    }

    if (tTargetType.bCone)
    {
        pResult = GetGizmoTargetFromList(tACDPlayer, tTargetType, tConeRActors, targetingInfo.aSearchAngle, targetingInfo.wpConeStart);
    }
    return pResult;
}





BOOL SetGizmoTarget(
    const Player * ptPlayer,
    const ActorCommonData & tACDPlayer,
    const SNO snoPowerToTry,
    TargetPriority tTargetType,
    RActorIDListPooled & tCloseRActors,
    RActorIDListPooled & tConeRActors,
    const TargetingInformationOutput & targetingInfo,
    ResolvedClick & tResolvedClick)
{
    const ActorCommonData * ptACDTarget = GetGizmoTargetFromLists(tACDPlayer, tTargetType, tCloseRActors, tConeRActors, targetingInfo);
    if (!ptACDTarget)
        return FALSE;

    tResolvedClick.tTarget.InitializeFromACD(ptACDTarget->id, FALSE, FALSE);
    tResolvedClick.snoPower = ACDGizmoGetOperationPower(ACDGet(tResolvedClick.tTarget.idACDTarget));
    tResolvedClick.idRActor = (RActorID)ptACDTarget->idOwner;

    if (tResolvedClick.snoPower != SNO_NONE &&
        PowerIsCloseEnough(tACDPlayer.id, AutoPowerDefinitionR(GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO)), &tResolvedClick.tTarget, FALSE))
        tResolvedClick.snoOriginalPower = tResolvedClick.snoPower;
    else
        tResolvedClick.snoPower = snoPowerToTry;
    return TRUE;
}






















void TargetSetToActor(
    const Player * ptPlayer,
    const ActorCommonData & tACDTarget,
    Target & tTarget,
    const PowerDefinition * pdefPower)
{

    const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    float flOffset = PowerGetOffsetFromTarget(tACDPlayer, AutoPowerDefinitionR(pdefPower));
    if (Math::Abs(flOffset) < EPSILON)
    {

        tTarget.InitializeFromACD(tACDTarget.id, FALSE, FALSE);
        return;
    }

    const RActor * ptRActor = RActorGet(ptPlayer->idPlayerRActor);

    WorldPlace tSourcePlace = tACDPlayer.GetPlace();
    WorldPlace tTargetPlace = tACDTarget.GetPlace();

    Vector3D vOffset = tTargetPlace.wp - tSourcePlace.wp;
    float flDistance = vOffset.Normalize();
    vOffset *= Math::Max(flOffset, -flDistance);
    tTargetPlace.wp += vOffset;

    if (!PowerIsAimedAtGround(AutoPowerDefinitionR(pdefPower)))
        PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(&tACDPlayer, pdefPower, tTargetPlace, TRUE);
    else
        ptRActor->ptCActorMovement->SnapPlaceToGround(tTargetPlace);

    tTarget.InitializeFromPlace(tTargetPlace);
}


void TargetSetToGround(
    const Player * ptPlayer,
    Target & tTarget,
    const PowerDefinition * pdefPower)
{
    const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    const RActor * ptRActor = RActorGet(ptPlayer->idPlayerRActor);
    WorldPlace tTargetPlace = tACDPlayer.GetPlace();
    // Inference (lines 704, 709): Vector3D(x, y, z) constructor; the DWARF records no inlined constructor call, the stores (x, y, 0) fit it.
    // Inference (line 712): Math::Max(1.0f, x) operand order; fmax needs this order (Max(x, 1.0f) gives fmaxnm).
    if (ptPlayer->tTargetComponent.GetInReticleMode())
    {
        Vector2D vReticle;
        ptPlayer->tTargetComponent.GetTargetingReticle(vReticle);
        Vector3D vOffset(vReticle.x, vReticle.y, 0.0f);
        tTargetPlace.wp += vOffset;
    }
    else
    {
        Vector3D vOffset(::Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).x,
                         ::Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).y, 0.0f);
        float flAdjustment = ptPlayer->tTargetComponent.GetTargetRangeAdjustment(pdefPower);
        vOffset *= Math::Max(1.0f, flAdjustment);
        tTargetPlace.wp += vOffset;
    }

    if (!PowerIsAimedAtGround(AutoPowerDefinitionR(pdefPower)))
        PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(&tACDPlayer, pdefPower, tTargetPlace, TRUE);
    else
        ptRActor->ptCActorMovement->SnapPlaceToGround(tTargetPlace);

    tTarget.InitializeFromPlace(tTargetPlace);
}


void FindGizmoInListAndRemoveNonTarget(
    const Player * ptPlayer,
    RActorIDListPooled & tGizmos,
    RActorIDListPooled & tRActors)
{
    ACDID idACDPlayer = ptPlayer->idPlayerACD;

    POS pos = tRActors.GetHeadPosition();
    while (pos != NULL)
    {
        POS oldPos = pos;
        RActorID idRActor = (RActorID)tRActors.GetNext(pos);
        RActor * pRActor = RActorGet(idRActor);
        if (pRActor->idACD == ACDID_INVALID)
        {
            tRActors.RemoveAt(oldPos);
            continue;
        }

        const ActorCommonData * ptACD = ACDTryToGet(pRActor->idACD);
        if (!ptACD || ItemIsGold(*ptACD))
        {
            tRActors.RemoveAt(oldPos);
            continue;
        }


        if (!(ptACD->IsNecroCorpse() || ptACD->IsMonster() || ACDIsPlayer(ptACD->id)) || !ptACD->IsAlive() || !ptACD->IsTargetable())
        {
            tRActors.RemoveAt(oldPos);
            oldPos = NULL;
        }


        if (pRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT))
        {
            if (oldPos != NULL)
                tRActors.RemoveAt(oldPos);
            continue;
        }



        if (ptACD->IsOperatableGizmo(idACDPlayer, FALSE) || ptACD->IsOperatableNPC(ptPlayer, TRUE))




            tGizmos.AddTail_Tracked(idRActor);
    }
}


void ResolveTargetForPlayer(
    const Player * ptPlayer,
    const SNO snoPowerToTry,
    ResolvedClick & tResolvedClick,
    BOOL bCheckOncePerClick,
    BOOL bCheckForHostiles,
    BOOL bCheckForOperatableGizmos,
    BOOL bAllowSecondaryTargets,
    BOOL bUseTargetLockPriorities)
{


    tResolvedClick.snoPower = SNO_NONE;
    tResolvedClick.tTarget.Clear();

    if (!ptPlayer || snoPowerToTry == SNO_NONE)
        return;
    // Inference: ptPlayer is const Player * (DWARF 0x02abccdc), but ResolveLockedTarget (Player.h:961), ResolveStickyTarget (:957),
    // SetLastTarget (:958) and GetIsHoldingReticlePowerDown (:981) take a non-const this. The const_cast<Player *> at those
    // uses (lines 823, 839, 956, 971, 1027) only drops const; the DWARF records no mutable member.
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    if (!tACDPlayer.IsAlive())
        return;



    ScopedPowerDefinitionLockR pdefPower(snoPowerToTry);

    if (PowerPlayerAttackOnlyOncePerClick(pdefPower) && bCheckOncePerClick)
        return;



    BOOL bInAimMode = ptPlayer->tTargetComponent.GetInAimMode();


    if (PowerIsChannelled(tACDPlayer, pdefPower) && bInAimMode)
    {
        TargetSetToGround(ptPlayer, tResolvedClick.tTarget, pdefPower);
        tResolvedClick.snoPower = snoPowerToTry;
        return;
    }


    if (ptPlayer->tTargetComponent.GetInReticleMode() &&
        const_cast<Player *>(ptPlayer)->tTargetComponent.GetIsHoldingReticlePowerDown() &&
        ptPlayer->tTargetComponent.GetReticlePower() == snoPowerToTry)
    {
        TargetSetToGround(ptPlayer, tResolvedClick.tTarget, pdefPower);
        tResolvedClick.snoPower = snoPowerToTry;
        return;
    }


    BOOL bTargetsEnemies = PowerCastTargetEnemies(pdefPower);
    BOOL bTargetsNecroCorpsesOnly = PowerCastTargetNecroCorpsesOnly(tACDPlayer, pdefPower);
    BOOL bCheckForTargets = (bCheckForHostiles && bTargetsEnemies) || bTargetsNecroCorpsesOnly;


    if (bCheckForTargets)
    {
        const ActorCommonData * ptACDLockedTarget = const_cast<Player *>(ptPlayer)->tTargetComponent.ResolveLockedTarget();
        if (ptACDLockedTarget)
        {


            if (!bInAimMode && PowerAutoTargets(pdefPower, ptPlayer) && !bTargetsNecroCorpsesOnly)
            {
                TargetSetToActor(ptPlayer, *ptACDLockedTarget, tResolvedClick.tTarget, pdefPower);
                tResolvedClick.snoPower = snoPowerToTry;
                tResolvedClick.idRActor = (RActorID)ptACDLockedTarget->idOwner;
                return;
            }
        }
    }


    if (bInAimMode)
    {
        bCheckForOperatableGizmos = FALSE;
    }


    TargetingInformationInput input(ptPlayer, pdefPower, TARGETING_PRIMARY);
    TargetingInformationOutput output;
    GetTargetingInformation(input, output);


    Sphere wsClose;
    wsClose.wpCenter = tACDPlayer.GetPlace().wp;
    wsClose.wdRadius = output.flControllerAutoTargetingCloseRange;
    RActorIDListPooled listCloseRActors(MemoryPoolsGetDATAIDListPool());

    Sphere wsCone;
    wsCone.wpCenter = wsClose.wpCenter;
    wsCone.wdRadius = output.flSearchRadius;
    RActorIDListPooled listConeRActors(MemoryPoolsGetDATAIDListPool());

    RActorsPlayerTargetCull(wsClose,
                            listCloseRActors,
                            wsCone,
                            output.wpConeStart.wp.As2D(),
                            ptPlayer->tTargetComponent.GetTargetingDirection(),
                            output.aSearchAngle,
                            output.flSearchRadius,
                            listConeRActors,
                            RACTORID_INVALID);
    RActorIDListPooled listCloseGizmo(MemoryPoolsGetDATAIDListPool());
    FindGizmoInListAndRemoveNonTarget(ptPlayer, listCloseGizmo, listCloseRActors);

    RActorIDListPooled listConeGizmo(MemoryPoolsGetDATAIDListPool());
    ReduceWithCone(listConeRActors, tACDPlayer, ptPlayer, output.aSearchAngle, output.flSearchRadius, output.wpConeStart);
    FindGizmoInListAndRemoveNonTarget(ptPlayer, listConeGizmo, listConeRActors);
    // Inference (lines 895, 932, 984): each condition's && sits at the column after "<table>[i].bOperableGizmo"; the
    // second test is inferred from the original, which tries the two non-operable low-priority entries only when bCheckForTargets.
    for (int32 i = 0; i < (int32)(sizeof(targetGizmoPrioritiesOverMonsters) / sizeof(targetGizmoPrioritiesOverMonsters[0])); ++i)
    {
        if (targetGizmoPrioritiesOverMonsters[i].bOperableGizmo && !bCheckForOperatableGizmos)
            continue;

        if (!targetGizmoPrioritiesOverMonsters[i].bOperableGizmo && !bCheckForTargets)
            continue;





        if (SetGizmoTarget(ptPlayer, tACDPlayer, snoPowerToTry, targetGizmoPrioritiesOverMonsters[i], listCloseGizmo, listConeGizmo, output, tResolvedClick))
            return;
    }

    // Inference (lines 915, 921): float argument order from the call setup and from the argument columns 120/164 and 117.


    const ActorCommonData * ptACDRadialTarget = NULL;
    if (bCheckForTargets && (pdefPower->tPower.uFlags & (1 << POWERBIT_CONSOLE_USE_RADIAL_TARGETING)))
    {
        ptACDRadialTarget = GetHostileTargetFromList(listCloseRActors, pdefPower, tACDPlayer, ptPlayer, output.flControllerAutoTargetingCloseAngle, output.aSearchAngle, output.flControllerAutoTargetingCloseRange, output.wpConeStart, bUseTargetLockPriorities);
    }

    const ActorCommonData * ptACDConeTarget = NULL;
    if (bCheckForTargets && (pdefPower->tPower.uFlags & (1 << POWERBIT_CONSOLE_USE_CONE_TARGETING)))
    {
        ptACDConeTarget = GetHostileTargetFromList(listConeRActors, pdefPower, tACDPlayer, ptPlayer, output.aSearchAngle, 0.0f, output.flSearchRadius, output.wpConeStart, bUseTargetLockPriorities);
    }

    const ActorCommonData * ptACDTarget = PowerPrefersRadialTargeting(pdefPower, ptPlayer) ? (ptACDRadialTarget ? ptACDRadialTarget : ptACDConeTarget) : (ptACDConeTarget ? ptACDConeTarget : ptACDRadialTarget);

    // Inference (line 928): operator columns 35/39/97 fit the negated form; ACDIsHostile(radial, player) argument order from the call.

    if (!(bCheckForTargets && ptACDRadialTarget && ptACDRadialTarget == ptACDTarget && ACDIsHostile(*ptACDRadialTarget, tACDPlayer, FALSE)))
    {
        for (int32 i = 0; i < (int32)(sizeof(targetGizmoHighPriorities) / sizeof(targetGizmoHighPriorities[0])); ++i)
        {
            if (targetGizmoHighPriorities[i].bOperableGizmo && !bCheckForOperatableGizmos)
                continue;

            if (!targetGizmoHighPriorities[i].bOperableGizmo && !bCheckForTargets)
                continue;





            if (SetGizmoTarget(ptPlayer, tACDPlayer, snoPowerToTry, targetGizmoHighPriorities[i], listCloseGizmo, listConeGizmo, output, tResolvedClick))
                return;
        }
    }




    if (bCheckForTargets && ptACDTarget)
    {
        if (PowerAutoTargets(pdefPower, ptPlayer))
        {
            TargetSetToActor(ptPlayer, *ptACDTarget, tResolvedClick.tTarget, pdefPower);
            tResolvedClick.idRActor = (RActorID)ptACDTarget->idOwner;
            const_cast<Player *>(ptPlayer)->tTargetComponent.SetLastTarget(ptACDTarget->id, FALSE);
        }
        else
        {
            TargetSetToGround(ptPlayer, tResolvedClick.tTarget, pdefPower);
        }

        tResolvedClick.snoPower = snoPowerToTry;
        return;
    }



    if (bCheckForTargets && PowerRequiresTarget(tACDPlayer, pdefPower) && !ptPlayer->tTargetComponent.GetLastTargetInSecondaryRange())
    {
        const ActorCommonData * ptACDStickyTarget = const_cast<Player *>(ptPlayer)->tTargetComponent.ResolveStickyTarget();
        if (ptACDStickyTarget && ptACDStickyTarget->IsAlive() && ptACDStickyTarget->IsTargetable() && PowerAutoTargets(pdefPower, ptPlayer) && !bTargetsNecroCorpsesOnly)
        {
            TargetSetToActor(ptPlayer, *ptACDStickyTarget, tResolvedClick.tTarget, pdefPower);
            tResolvedClick.snoPower = snoPowerToTry;
            tResolvedClick.idRActor = (RActorID)ptACDStickyTarget->idOwner;
            return;
        }
    }


    for (int32 i = 0; i < (int32)(sizeof(targetGizmoLowPriorities) / sizeof(targetGizmoLowPriorities[0])); ++i)
    {
        if (targetGizmoLowPriorities[i].bOperableGizmo && !bCheckForOperatableGizmos)
            continue;

        if (!targetGizmoLowPriorities[i].bOperableGizmo && !bCheckForTargets)
            continue;





        if (SetGizmoTarget(ptPlayer, tACDPlayer, snoPowerToTry, targetGizmoLowPriorities[i], listCloseGizmo, listConeGizmo, output, tResolvedClick))
            return;
    }



    if (!bCheckForHostiles)
    {
        return;
    }




    if (bCheckForTargets && bAllowSecondaryTargets && (pdefPower->tPower.uFlags & (1 << POWERBIT_CONSOLE_USE_SECONDARY_TARGETING)) && PowerAutoTargets(pdefPower, ptPlayer))
    {
        input.eTargetingType = TARGETING_SECONDARY;
        GetTargetingInformation(input, output);
        // Inference (lines 1013, 1015): the original stores the secondary radius into wsClose (x29-0xdc) and intersects that sphere.
        wsClose.wdRadius = output.flSearchRadius;
        RActorIDListPooled listConeRActorsScoped(MemoryPoolsGetDATAIDListPool());
        RActorsIntersectSphere(wsClose, RACTORID_INVALID, listConeRActorsScoped);

        ReduceWithCone(listConeRActorsScoped, tACDPlayer, ptPlayer, output.aSearchAngle, output.flSearchRadius, output.wpConeStart);

        RActorIDListPooled listConeGizmoScoped(MemoryPoolsGetDATAIDListPool());
        FindGizmoInListAndRemoveNonTarget(ptPlayer, listConeGizmoScoped, listConeRActorsScoped);

        const ActorCommonData * ptACDSecondaryTarget = GetHostileTargetFromList(listConeRActorsScoped, pdefPower, tACDPlayer, ptPlayer, output.aSearchAngle, 0.0f, output.flSearchRadius, output.wpConeStart, bUseTargetLockPriorities);
        if (ptACDSecondaryTarget)
        {
            TargetSetToActor(ptPlayer, *ptACDSecondaryTarget, tResolvedClick.tTarget, pdefPower);
            tResolvedClick.idRActor = (RActorID)ptACDSecondaryTarget->idOwner;
            const_cast<Player *>(ptPlayer)->tTargetComponent.SetLastTarget(ptACDSecondaryTarget->id, TRUE);
            tResolvedClick.snoPower = snoPowerToTry;
            return;
        }
    }


    TargetSetToGround(ptPlayer, tResolvedClick.tTarget, pdefPower);
    tResolvedClick.snoPower = snoPowerToTry;

    WorldPlace wpResolvedClick;
    if (tResolvedClick.tTarget.GetPlace(wpResolvedClick))
    {
        RActorID idRActorSticky = CPlayerUtilGetNearbyStickyTargetedActorFromList(ptPlayer, wpResolvedClick.wp, snoPowerToTry);
        RActor * ptRActorSticky = RActorTryToGet(idRActorSticky);
        if (ptRActorSticky)
        {
            ActorCommonData * ptACDSticky = ACDTryToGet(ptRActorSticky->idACD);
            if (ptACDSticky)
            {
                TargetSetToActor(ptPlayer, *ptACDSticky, tResolvedClick.tTarget, pdefPower);
                tResolvedClick.idRActor = idRActorSticky;
            }
        }
    }


}


void ApplyGrowthForCast(
    const Player * ptPlayer,
    const PowerDefinition * pdefPower,
    float & flSearchRadius,
    Angle & aSearchAngle)
{
    if (!pdefPower)
        return;



    ScopedGlobalDefinitionLockR pdefGlobal;

    if (!(pdefPower->tPower.uFlags & (1 << POWERBIT_CONSOLE_TARGET_AREA_GROWS)))
        return;

    const ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACD)
        return;




    int32 nMaxGrowth = pdefGlobal->nMaxTimesToGrow;
    if (TagMapTagExists(pdefPower->tPower.hGeneralTagMap, TAG_POWER_CONSOLE_TARGET_NUM_GROWS))
    {
        nMaxGrowth = (int32)PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(pdefPower), TAG_POWER_CONSOLE_TARGET_NUM_GROWS, -1, -1, 0);
    }

    int32 nCurrentKills = Math::Min(ptPlayer->tClientInfo.nKillsSinceMoving, nMaxGrowth - 1);

    float flSearchRadiusGrowth = pdefGlobal->flControllerAutoTargetingSearchGrowthRate;
    if (TagMapTagExists(pdefPower->tPower.hGeneralTagMap, TAG_POWER_CONSOLE_TARGET_DIST_GROWTH_PER_KILL))
    {
        flSearchRadiusGrowth = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(pdefPower), TAG_POWER_CONSOLE_TARGET_DIST_GROWTH_PER_KILL, -1, -1, 0);
    }

    float flSearchThetaGrowth = pdefGlobal->flControllerAutoTargetingSearchAngleGrowthRate;
    if (TagMapTagExists(pdefPower->tPower.hGeneralTagMap, TAG_POWER_CONSOLE_TARGET_THETA_GROWTH_PER_KILL))
    {
        flSearchThetaGrowth = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(pdefPower), TAG_POWER_CONSOLE_TARGET_THETA_GROWTH_PER_KILL, -1, -1, 0);
    }

    float flSearchAimModeGrowth = 0.0f;
    if (ptPlayer->tTargetComponent.GetInAimMode() && PlayerGetLocalPlayerCount() == 1)
    {
        flSearchAimModeGrowth = pdefGlobal->flControllerAutoTargetingSearchAimModeGrowth * ::Console::ControllerUtils::GetLTriggerPosition(ptPlayer->nControllerIndex);
    }

    float flScalar = (float)nCurrentKills;

    flSearchRadius += flSearchRadiusGrowth * flScalar + flSearchAimModeGrowth;
    aSearchAngle += flSearchThetaGrowth * flScalar;
}

void GetTargetingInformation(
    const TargetingInformationInput & input,
    TargetingInformationOutput & output)
{
    const ActorCommonData * ptACD = ACDTryToGet(input.ptPlayer->idPlayerACD);
    if (!ptACD)
        return;



    float flNearWidth = 0.0f;

    ScopedGlobalDefinitionLockR pdefGlobal;
    output.flSearchRadius = pdefGlobal->flControllerAutoTargetingSearchRange;
    output.flControllerAutoTargetingCloseRange = pdefGlobal->flControllerAutoTargetingCloseRange;

    if (input.pdefPower)
    {
        if (TagMapTagExists(input.pdefPower->tPower.hGeneralTagMap, TAG_POWER_CONSOLE_AUTO_TARGET_SEARCH_RANGE))
        {
            output.flSearchRadius = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(input.pdefPower), TAG_POWER_CONSOLE_AUTO_TARGET_SEARCH_RANGE, -1, -1, 0);
        }

        if (TagMapTagExists(input.pdefPower->tPower.hGeneralTagMap, TAG_POWER_CONSOLE_AUTO_TARGET_CLOSE_RANGE))
        {
            output.flControllerAutoTargetingCloseRange = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(input.pdefPower), TAG_POWER_CONSOLE_AUTO_TARGET_CLOSE_RANGE, -1, -1, 0);
        }
    }

    BOOL bDeadZone = ::Console::ControllerUtils::LStickIsInDeadZone(input.ptPlayer->nControllerIndex, TRUE);

    output.flControllerAutoTargetingCloseAngle = FULL_CIRCLE;
    if (!bDeadZone && pdefGlobal->flControllerAutoTargetingCloseAngle != 0.0f)
    {
        output.flControllerAutoTargetingCloseAngle = Math::DegToAngle(pdefGlobal->flControllerAutoTargetingCloseAngle) * 0.5f;
    }



    const Vector2D vLStickWorldDirection = input.ptPlayer->tTargetComponent.GetTargetingDirection();

    switch (input.eTargetingType)
    {
    case TARGETING_PRIMARY:
    {
        // Inference (line 1160): "!bDeadZone && dot < 0" with the secondary values first; the branch is b.pl (not-less-than), which a >= test (b.ge) does not give.

        Vector2D vForward = ptACD->GetForward2D();
        if (!bDeadZone && Vector2D::DotProduct(vLStickWorldDirection, vForward) < 0.0f)
        {
            flNearWidth = pdefGlobal->flControllerAutoTargetingSearchNearWidthSecondary;
            output.aSearchAngle = pdefGlobal->flControllerAutoTargetingSearchAngleSecondary;
        }
        else
        {
            flNearWidth = pdefGlobal->flControllerAutoTargetingSearchNearWidth;
            output.aSearchAngle = pdefGlobal->flControllerAutoTargetingSearchAngle;
        }
        flNearWidth *= 0.5f;
        ApplyGrowthForCast(input.ptPlayer, input.pdefPower, output.flSearchRadius, output.aSearchAngle);
        output.aSearchAngle = Math::DegToAngle(output.aSearchAngle) * 0.5f;
        break;
    }
    case TARGETING_STICKY:
        flNearWidth = pdefGlobal->flControllerAutoTargetingStickyNearWidth * 0.5f;
        output.aSearchAngle = Math::DegToAngle(pdefGlobal->flControllerAutoTargetingStickyAngle) * 0.5f;
        break;
    case TARGETING_SECONDARY:
        output.flSearchRadius = pdefGlobal->flControllerAutoTargetingSearchRangeSecondary;
        flNearWidth = pdefGlobal->flControllerAutoTargetingSearchNearWidthSecondary * 0.5f;
        output.aSearchAngle = Math::DegToAngle(pdefGlobal->flControllerAutoTargetingSearchAngleSecondary) * 0.5f;
        break;
    default:
        break;
    }
    WorldPlace wpPlayer = ptACD->GetPlace();
    output.wpConeStart.wp = wpPlayer.wp;

    if (Math::Abs(flNearWidth) < EPSILON || output.aSearchAngle > HALF_CIRCLE)
        return;



    float flTangent = Math::Tan(output.aSearchAngle);
    output.wpConeStart.wp.x -= (vLStickWorldDirection.x * flNearWidth) / flTangent;
    output.wpConeStart.wp.y -= (vLStickWorldDirection.y * flNearWidth) / flTangent;
}

} }
