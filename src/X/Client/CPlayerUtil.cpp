// X/Client/CPlayerUtil.cpp
// Functions sit on their original line numbers.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CPlayerUtil.h"
#include "CPlayerInput.h"
#include "../Common/Player.h"
#include "../Common/Target.h"
#include "../Common/Game.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Gizmo.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Definitions/PowerDefinition.h"
#include "../Common/Powers/PowersCommon.h"
#include "../Common/TagMaps/PowerTags.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/MemoryPools.h"
#include "RActors/RActors.h"
#include "Gfx/GfxCommon.h"
#include "Camera/CameraManager.h"
#include "../Common/GizmoSwig.h"
#include "../Common/AnimPlayer.h"
#include "../Common/Powers/PowersStruct.h"
#include "CGame.h"
#include "Console/ControllerUtils.h"
#include "Definitions/PreferencesDefinition.h"
#include "Input/Input.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Camera/CameraLocation.h"
struct RActorIntersection {  // size 0x8; X/Client/RActors/RActorCollision.h
    RActorID idRActor;  // 0x00
    float flT;  // 0x04
};

typedef blz::vector<RActorIntersection, blz::allocator<RActorIntersection> > RActorIntersectArray;  // X/Client/RActors/RActorCollision.h

RActorIntersectArray & SelectionGetPickList();  // X/Client/Selection.cpp
RActorIntersectArray & SelectionGetFallbackPickList();  // X/Client/Selection.cpp
RActorID SelectionGetTargetedCorpse();  // X/Client/Selection.cpp
CursorMgr & PlayerGetCursorMgr();  // X/Client/CPlayerInput.cpp
float PowerGetStickyTargetRadius(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp
void RActorsIntersectSphere(const Sphere & tSphere, RActorID idRActorIgnore, RActorIDListPooled & listRActors);  // X/Client/RActors/RActors.cpp
namespace UIConversation { BOOL ACDIsInConversation(const ACDID idACD, BOOL bConvDialogOnly, BOOL bUnusableOnly); }  // X/Client/UserInterface/UIConversation.cpp
namespace UIInventory { ACDID FindBestItemToUse(GBID gbidItem); }  // X/Client/UserInterface/Interface/UIInventory.cpp
BOOL RActorIsHidden(const RActor * ptRActor);  // X/Client/RActors/RActors.cpp
BOOL ItemIsAutoPickup(const ActorCommonData & tACDItem);  // X/Common/Items/Items.cpp
BOOL ItemCanBeSelectedOnGround(const ActorCommonData & tACDItem);  // X/Common/Items/Items.cpp
BOOL PlayersAreHostileToEachOther(const Player * ptPlayer0, const Player * ptPlayer1);  // X/Common/Player.cpp
SNO PowerPlayerFallbackIfNecessary(AutoPowerDefinitionR pdefPower, ACDID idACD, GameError & eError, BOOL bCurrentlyChannelingPower);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerCastTargetNecroCorpsesOnly(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerIsValidCastTarget(AutoPowerDefinitionR pdefPower, ACDID idACDAttacker, ACDID idACDTarget, BOOL bAllowSelfTarget);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerIsCloseEnoughToPlace(ACDID idACD, const WorldPlace & placeSource, AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldDistance wdRange, WorldDistance wdLongReach, float * pflDistance, BOOL bIgnoreRange);  // X/Common/Powers/PowersCommon.cpp
WorldPlace ClientPointToPlace(const Vector3D & wp);  // X/Client/CWorld.cpp
SNO InputChooseFallbackPower(SNO snoPower, const Player * ptPlayer, GameError * peGameError);  // X/Client/CPlayerInput.cpp
namespace Targeting { namespace Console {
void ResolveTargetForPlayer(const Player * ptPlayer, const SNO snoPowerToTry, ResolvedClick & tResolvedClick, BOOL bCheckOncePerClick, BOOL bCheckForHostiles, BOOL bCheckForOperatableGizmos, BOOL bAllowSecondaryTargets, BOOL bUseTargetLockPriorities);  // X/Client/Targeting/ConsoleTargeting.cpp
} }
namespace Controller {
InputButton GetBindingFromSkill(const SNO snoPower, const Player * ptPlayer);  // X/Client/Definitions/ControllerSchemeDefinition.cpp
}
void PowerGetStartRange(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldDistance & wdAttackRange);  // X/Common/Powers/PowersCommon.cpp

BOOL PowerLocksActorsWhileSweeping(AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp:795
BOOL PowerIsTargetWithinValidZRange(const ACDID idACDPlayer, const Target & tTarget);  // X/Common/Powers/PowersCommon.cpp:2217
uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim);  // X/Common/Powers/PowersCommon.cpp:8013
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive);  // X/Common/SceneCommon.cpp:1240
BOOL NavMeshGetHeightAndNormal(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp, const BOOL fPinToSquareBounds, WorldDistance & flHeight, Vector3D & wvNormal);  // X/Common/Nav/NavigationMesh.cpp:783


































































































































































































const float CONTACT_PLANE_NEAR_TARGET_DISTANCE_FUDGE = 20.0f;
const float CONTACT_PLANE_Z_HEIGHT_FUDGE = 0.5f;
const float GROUND_TARGET_NEAR_DISTANCE_SQUARE_FUDGE = 10.0f;
const float GROUND_TARGET_NEAR_DISTANCE_FUDGE = 3.162f;

void CPlayerUtilGetTarget(
    const Player * ptPlayer,
    const RActorID idRActorHit,
    const IVector2D & ptScreenClick,
    const SNO snoPower,
    Target & tTarget,
    ResolvedClick * ptClick)
{


    tTarget.Clear();

    if (ptClick)
    {

        tTarget = ptClick->tTarget;
        return;
    }


    if (PowerLocksActorsWhileSweeping(snoPower))
    {
        if (PowerStickyTargetsActorsWhileSweeping(snoPower))
        {
            if (ptPlayer->tClientInfo.tTargetLock.snoPower == snoPower &&
                ptPlayer->tClientInfo.tTargetLock.tTarget.IsTargetingACD() &&
                ACDTryToGet(ptPlayer->tClientInfo.tTargetLock.tTarget.GetACD()) &&
                PowerIsValidCastTarget(snoPower, ptPlayer->idPlayerACD, ptPlayer->tClientInfo.tTargetLock.tTarget.GetACD(), FALSE))
            {
                tTarget = ptPlayer->tClientInfo.tTargetLock.tTarget;
                return;
            }
        }

        RActor * ptRActorTarget = RActorTryToGet(idRActorHit);
        if (ptRActorTarget)
        {
            tTarget.InitializeFromACD(ptRActorTarget->idACD, FALSE, FALSE);
            return;
        }
    }

	if (snoPower != SNO_NONE &&
		PowerTargetGroundOnly(snoPower))
    {
        Vector3D wpTarget;
        CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_GROUND_PLANE, wpTarget, 0.0f);

        RActor* ptPlayerRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptPlayerRActor)
        {
            ActorCommonData* ptACDPlayer = ACDTryToGet(ptPlayerRActor->idACD);
            if (ptACDPlayer)
            {

                WorldPlace wpPlayer = ptACDPlayer->GetPlace();
                // Inference: v2dPlayerPos is in the DWARF (line 328, no location) but the original subtracts wpPlayer.wp directly (load order at +0x49c), so it is left unused.
                Vector2D v2dPlayerPos = wpPlayer.wp.As2D();
                Vector2D v2DRelativeClickPos = wpTarget.As2D() - wpPlayer.wp.As2D();

                float flClickPosDistSquared = v2DRelativeClickPos.MagnitudeSquared();

                if (flClickPosDistSquared < GROUND_TARGET_NEAR_DISTANCE_SQUARE_FUDGE)
                {
                    v2DRelativeClickPos.Normalize();
                    v2DRelativeClickPos *= GROUND_TARGET_NEAR_DISTANCE_FUDGE;

                    wpTarget.As2D() += v2DRelativeClickPos;
                }
            }
        }

        WorldPlace placeTarget = ClientPointToPlace(wpTarget);
        tTarget.InitializeFromPlace(placeTarget);
        return;
    }

    if (snoPower != SNO_NONE &&
        PowerTargetNavMeshOnly(snoPower))
    {
        Vector3D wpTarget;

        if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK))
        {
            CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_WALKING, wpTarget, 0.0f);
        }
        else
        {
            CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_NAV_MESH, wpTarget, 0.0f);
        }

        WorldPlace placeTarget = ClientPointToPlace(wpTarget);
        tTarget.InitializeFromPlace(placeTarget);

        RActor* ptPlayerRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptPlayerRActor)
        {
            if (!PowerIsTargetWithinValidZRange(ptPlayerRActor->idACD, tTarget))
            {
                CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_GROUND_PLANE, wpTarget, 0.0f);
                WorldPlace placeTargetLoc = ClientPointToPlace(wpTarget);
                tTarget.InitializeFromPlace(placeTargetLoc);
            }
        }
        return;
    }


    if (snoPower != SNO_NONE &&
        PowerTargetsContactPlaneOnly(snoPower))
    {
        Vector3D wpTarget;
        Vector3D wdContactOffset;
        wdContactOffset.Set(0.0f, 0.0f, 0.0f);

        RActor* ptPlayerRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptPlayerRActor)
        {

            ActorCommonData* ptACDPlayer = ACDTryToGet(ptPlayerRActor->idACD);
            if (ptACDPlayer)
            {
                uint32 dwAnimationTag = PowerDetermineAnimation(snoPower, ptPlayerRActor->idACD, cgTargetNone, FALSE);
                WeaponClass eWeaponClass = ptACDPlayer->GetWeaponClassForAnim(NULL);
                SNO snoAnim = ActorGetAnim(ptPlayerRActor->snoActor, eWeaponClass, dwAnimationTag);

                if (snoAnim != SNO_NONE)
                {
                    ptACDPlayer->GetContactOffsetForAnim(snoAnim, wdContactOffset, 0, 0, FALSE);
                    CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_CONTACT_OFFSET_PLANE_FROM_NAV_MESH, wpTarget, wdContactOffset.z);


                    WorldPlace wpPlayer = ptACDPlayer->GetPlace();

                    Vector2D v2DContactOffset = wdContactOffset.As2D();
                    Vector2D v2DRelativeClickPos = wpTarget.As2D() - wpPlayer.wp.As2D();

                    float flContactDistSquared = v2DContactOffset.MagnitudeSquared();
                    float flClickPosDistSquared = v2DRelativeClickPos.MagnitudeSquared();

                    if (flClickPosDistSquared < flContactDistSquared + CONTACT_PLANE_NEAR_TARGET_DISTANCE_FUDGE)
                    {

                        v2DRelativeClickPos.Normalize();
                        v2DRelativeClickPos *= Math::Sqrt(flContactDistSquared + CONTACT_PLANE_NEAR_TARGET_DISTANCE_FUDGE);

                        wpTarget.x = wpPlayer.wp.x + v2DRelativeClickPos.x;
                        wpTarget.y = wpPlayer.wp.y + v2DRelativeClickPos.y;

                        WorldPlace wpAdjustedTarget;
                        wpAdjustedTarget.wp = wpTarget;
                        wpAdjustedTarget.idSWorld = wpPlayer.idSWorld;

                        NavMeshHandle tNavMeshHandle;
                        Vector3D vNormal;
                        float flHeight;
                        ScenesCommonNavMeshNodeFromPlace(wpAdjustedTarget, &tNavMeshHandle, FALSE, FALSE);
                        NavMeshGetHeightAndNormal(tNavMeshHandle, wpTarget, FALSE, flHeight, vNormal);

                        wpTarget.z = flHeight + wdContactOffset.z;
                    }



                    Vector3D wpContactOfset;
                    ptACDPlayer->GetContactOffsetForAnim(snoAnim, wpContactOfset, 0, 0, TRUE);
                    float flZDifference = Math::Abs(wpContactOfset.z - wpTarget.z);

                    if (flZDifference < CONTACT_PLANE_Z_HEIGHT_FUDGE)
                    {
                        wpTarget.z = wpContactOfset.z;
                    }
                }
                else
                {
                    CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_GROUND_PLANE, wpTarget, 0.0f);
                }
            }
        }

        WorldPlace placeTarget = ClientPointToPlace(wpTarget);
        tTarget.InitializeFromPlace(placeTarget);

        if (ptPlayerRActor)
        {
            if (!PowerIsTargetWithinValidZRange(ptPlayerRActor->idACD, tTarget))
            {
                CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_CONTACT_OFFSET_PLANE_FROM_GROUND_PLANE, wpTarget, wdContactOffset.z);
                WorldPlace placeTargetLoc = ClientPointToPlace(wpTarget);
                tTarget.InitializeFromPlace(placeTargetLoc);
            }
        }
        return;
    }

    RActor * ptRActorTarget = RActorTryToGet(idRActorHit);
    if (ptRActorTarget)
    {
        tTarget.InitializeFromACD(ptRActorTarget->idACD, FALSE, FALSE);
        return;
    }


	if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO))
    {
        return;
    }

    Vector3D wpTarget;

    CPlayerUtilPlayerClick(ptPlayer, ptScreenClick, PLAYER_CLICK_INTERSECT_APPEARANCE, wpTarget, 0.0f);

	if (snoPower != SNO_NONE && !PowerIsAimedAtGround(snoPower))
    {

		ACDID idACDPlayer = ptPlayer->idPlayerACD;
		ScopedActorDefinitionLockR pdefActor(ACDGet(idACDPlayer).snoActor);
		WorldDistance wdHeightOffset = pdefActor->wpLocationPowerSrc.z;

		const PRTransform& transform = CameraManagerGetCameraState(CAMERA_CURRENT).transform;
        Vector3D wvBackVector = transform.q.Rotate(cgwvZAxis);

        if (Math::Abs(wvBackVector.z) > 0.1f)
        {
            float flScale = wdHeightOffset / wvBackVector.z;
            wpTarget += wvBackVector * flScale * 0.7f;
        }

        wpTarget += cgwvZAxis * wdHeightOffset * 0.3f;
    }

    WorldPlace placeTarget = ClientPointToPlace(wpTarget);
    tTarget.InitializeFromPlace(placeTarget);
}






static BOOL sACDIsIgnored(const ActorCommonData & tACD, const ACDID idACDPlayer, const BOOL fIgnoreGizmos, const SNO snoPower)




{
    if (fIgnoreGizmos &&
        tACD.IsOperatableGizmo(ACDID_INVALID, FALSE))
    {

        if (!tACD.IsWreckableObject())
        {
            return TRUE;
        }
    }

    if (tACD.IsGizmo())
    {

        GizmoType eGizmoType = tACD.GetGizmoType();
        if (eGizmoType == ACTORGIZMO_DESTROYABLE_OBJECT || eGizmoType == ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT)
        {
            return TRUE;
        }


        if (!tACD.IsAlive())
        {
            return TRUE;
        }
    }

    RActorID idTargetRActor = (RActorID)ACDGetOwner(tACD.id);
    const RActor * ptTargetRActor = RActorGet(idTargetRActor);
    BOOL bRActorIsVisible = ptTargetRActor->dwVisibilityFlags & 1;
    if (!bRActorIsVisible)
    {
        return TRUE;
    }

    if (!PowerIsValidCastTarget(AutoPowerDefinitionR(snoPower), idACDPlayer, tACD.id, FALSE)) { return TRUE; } return FALSE;





}



static RActorID sFindFirstRActor(RActorIntersectArray & listRActorsHit, const RActorID idPlayerRActor, const BOOL fIgnoreGizmos, const SNO snoPowerToUse)




{
    ACDID idACDRActor = RActorGet(idPlayerRActor)->idACD;
    for (uint32 i = 0; i < listRActorsHit.size(); ++i)
    {
        RActorID idRActor = listRActorsHit[i].idRActor;

        if (idRActor == idPlayerRActor)
        {
            continue;
        }

        const RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor)
        {
            continue;
        }

        ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
        if (!ptACD)
        {
            continue;
        }

        if (!sACDIsIgnored(*ptACD, idACDRActor, fIgnoreGizmos, snoPowerToUse))
        {
            return idRActor;
        }
    }




    return RACTORID_INVALID;
}


// NON_MATCHING: 1452 vs 1472 bytes, register allocation only: original keeps ptACD in x22 (copied to x26 around FindBestItemToUse) and tResolvedClick in x20, rematerialising the 1/0xffff ChainedDataArray constants after calls; ours spills ptACD and tResolvedClick. f19 fixed (from line-table columns) the operand order at 617 (col 29 is ptPlayer->idPlayerRActor, gives the original ccmp), store order 715-717 (col 29 rows: snoPower at 716, idRActor at 717), the flat !fIgnoreGizmos && conditions at 646/655 and the if (!sACDIsIgnored) block closing at 751.
static void sGetTargetedActorFromList(
    const RActorIntersectArray & listRActorsHit,
    const Player * ptPlayer,
    const SNO snoPowerOnButton,
    const BOOL fResolvingForHighlight,
    ResolvedClick & tResolvedClick)
{
    BOOL fIgnoreGizmos = CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, FALSE, FALSE);
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    for (uint32 i = 0; i < listRActorsHit.size(); i++)
    {
        RActorID idRActor = listRActorsHit[i].idRActor;

        if (idRActor == ptPlayer->idPlayerRActor || idRActor == RACTORID_INVALID)
            continue;



        const RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor || ptRActor->idACD == ACDID_INVALID)
            continue;



        ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
        if (!ptACD)
            continue;



        if (PowerCastTargetNecroCorpsesOnly(tACDPlayer, AutoPowerDefinitionR(snoPowerOnButton)) != (ptACD->IsNecroCorpse() != FALSE))
            continue;



        if (ptACD->IsHelper())
        {
            tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_HELPER_ATTACH);
            tResolvedClick.idRActor = idRActor;
            return;
        }

        if (!fIgnoreGizmos &&
            ptACD->IsNPC() &&
            (ptACD->IsOperatableNPC(ptPlayer, TRUE) || (UIConversation::ACDIsInConversation(ptACD->id, TRUE, FALSE) && !RActorIsHidden(ptRActor))))
        {
            tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_NPC);
            tResolvedClick.idRActor = idRActor;
            return;
        }

        if (!fIgnoreGizmos &&
            ptACD->IsOperatableGizmo(tACDPlayer.id, FALSE))
        {
            GizmoType nGizmoType = ptACD->GetGizmoType();

            switch (nGizmoType)
            {
                case ACTORGIZMO_ITEM_INTERNAL:
                    if (fResolvingForHighlight)
                    {
                        if (!ItemCanBeSelectedOnGround(*ptACD))
                            continue;
                    }
                    else
                    {


                        if (ItemIsAutoPickup(*ptACD))
                            continue;





                        if (PlayerGetCursorMgr().GetCursorState() != CURSORSTATE_NORMAL)
                            continue;
                    }
                    break;


                case ACTORGIZMO_BANNER:
                {
                    int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, ptACD->snoActor);
                    Player * ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);
                    if (ptPlayerDest &&

                        PlayersAreHostileToEachOther(ptPlayerDest, ptPlayer))
                        continue;
                    break;
                }
            }


            GBID gbidItemToUse = GizmoGetItemToUse(ptACD->snoActor);
            if (gbidItemToUse != GLOBALGBID_INVALID)
            {
                ACDID idACDItemToUse = UIInventory::FindBestItemToUse(gbidItemToUse);
                if (idACDItemToUse != ACDID_INVALID)
                {
                    tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = ACDGet(idACDItemToUse).AttributesGetPower(0);
                    tResolvedClick.idACDItemToUse = idACDItemToUse;
                    tResolvedClick.idRActor = (RActorID)ACDGetOwner(idACDItemToUse);
                    return;
                }
            }

            SNO snoGizmoPower = ACDGizmoGetOperationPower(*ptACD);
            if (snoGizmoPower != SNO_NONE &&
                ptACD->IsTargetable())
            {
                tResolvedClick.snoOriginalPower = snoGizmoPower;
                tResolvedClick.snoPower = snoGizmoPower;
                tResolvedClick.idRActor = idRActor;


                if (nGizmoType != ACTORGIZMO_PORTAL && nGizmoType != ACTORGIZMO_PAGE_OF_FATE_PORTAL && nGizmoType != ACTORGIZMO_BOSS_PORTAL && nGizmoType != ACTORGIZMO_DUNGEON_PORTAL && nGizmoType != ACTORGIZMO_RETURN_PORTAL)
                    return;
            }
        }











        if (!sACDIsIgnored(*ptACD, ptPlayer->idPlayerACD, fIgnoreGizmos, snoPowerOnButton))
        {

            if (ptACD->IsWreckableObject() &&
                !PowerIsOffensive(AutoPowerDefinitionR(snoPowerOnButton)))
            {
                tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = tACDPlayer.AttributeGetBasicAttack();
                tResolvedClick.idRActor = idRActor;
                return;
            }

            tResolvedClick.snoPower = snoPowerOnButton;
            tResolvedClick.idRActor = idRActor;
            return;
        }
    }
}




void CPlayerUtilResolveClick(const Player * ptPlayer, const SNO snoPowerOnButton, const BOOL fResolvingForHighlight, ResolvedClick & tResolvedClick)




{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_FAKE_INPUT_BIT))
    {
        tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_WALK);
        return;
    }

    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    if (tCursorMgr.GetCursorState() == CURSORSTATE_USE_ITEM)
    {
        ActorCommonData * ptACDItemToUse = tCursorMgr.GetItemToUse();
        if (ptACDItemToUse)
        {


            tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = ptACDItemToUse->AttributesGetPower(0);
            tResolvedClick.idACDItemToUse = ptACDItemToUse->id;
            tResolvedClick.idRActor = (RActorID)ACDGetOwner(tResolvedClick.idACDItemToUse);
            tResolvedClick.bPlayItemSound = TRUE;
        }
        return;
    }
    tResolvedClick.snoOriginalPower = snoPowerOnButton;

    SNO snoPowerToTry = snoPowerOnButton;
    if (snoPowerToTry != SNO_NONE)
    {
        GameError eError;
        snoPowerToTry = PowerPlayerFallbackIfNecessary(AutoPowerDefinitionR(snoPowerOnButton), ptPlayer->idPlayerACD, eError, FALSE);

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && PowerCastTargetNecroCorpsesOnly(*ptACDPlayer, AutoPowerDefinitionR(snoPowerToTry)))
        {
            tResolvedClick.snoPower = snoPowerOnButton;
            tResolvedClick.idRActor = SelectionGetTargetedCorpse();
        }
        else
        {
            RActorIntersectArray & listRActorsHit = SelectionGetPickList();
            sGetTargetedActorFromList(listRActorsHit, ptPlayer, snoPowerToTry, fResolvingForHighlight, tResolvedClick);
        }
    }

    if (tResolvedClick.snoPower == SNO_NONE)
    {
        RActorIntersectArray & listRActorsHitFallback = SelectionGetFallbackPickList();
        sGetTargetedActorFromList(listRActorsHitFallback, ptPlayer, snoPowerToTry, fResolvingForHighlight, tResolvedClick);
    }

    if (tResolvedClick.idRActor == RACTORID_INVALID)
    {
        if (!CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, FALSE, FALSE))
        {





            tResolvedClick.snoOriginalPower = snoPowerToTry = GlobalSNOGet(GLOBALSNO_POWERS_WALK);
        }
        tResolvedClick.snoPower = snoPowerToTry;
    }
}




GameError CPlayerUtilResolveConsoleButton(const SNO snoPowerOnButton, ResolvedClick & tResolvedClick, const Player * ptPlayer, BOOL bHeldButton, BOOL bCheckForHostiles, BOOL bCheckForOperatableGizmos)






{
    tResolvedClick.snoOriginalPower = snoPowerOnButton;

    GameError eError = GAMEERROR_NONE;
    if (snoPowerOnButton != SNO_NONE)
    {
        SNO snoPowerToTry = InputChooseFallbackPower(snoPowerOnButton, ptPlayer, &eError);
        Targeting::Console::ResolveTargetForPlayer(ptPlayer, snoPowerToTry, tResolvedClick, bHeldButton, bCheckForHostiles, bCheckForOperatableGizmos, TRUE, FALSE);
    }

    return eError;
}






RActorID CPlayerUtilPickRActorForPower(const Player * ptPlayer, const SNO snoPowerToUse)


{
    BOOL fIgnoreGizmos = CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, FALSE, FALSE);


    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer && PowerCastTargetNecroCorpsesOnly(*ptACDPlayer, AutoPowerDefinitionR(snoPowerToUse)))
    {
        return SelectionGetTargetedCorpse();
    }

    RActorIntersectArray & listRActorsHit = SelectionGetPickList();

    RActorID idRActorHit = sFindFirstRActor(listRActorsHit, ptPlayer->idPlayerRActor, fIgnoreGizmos, snoPowerToUse);
    if (idRActorHit == RACTORID_INVALID)
    {




        RActorIntersectArray & listRActorsHitFallback = SelectionGetFallbackPickList();
        idRActorHit = sFindFirstRActor(listRActorsHitFallback, ptPlayer->idPlayerRActor, fIgnoreGizmos, snoPowerToUse);
    }
    if (idRActorHit == RACTORID_INVALID)
    {
        Vector3D vClickPos;
        CPlayerUtilPlayerClick(ptPlayer, InputGetCurrentState(INPUTCONTEXT_DEFAULT).tMouseState.p, PLAYER_CLICK_INTERSECT_GROUND_PLANE, vClickPos, 0.0f);


        idRActorHit = CPlayerUtilGetNearbyStickyTargetedActorFromList(ptPlayer, vClickPos, snoPowerToUse);
    }
    return idRActorHit;
}












RActorID CPlayerUtilChooseBestAutoTarget(
    const Player * ptPlayer,
    SNO snoPower,
    float flSearchRadius,
    Angle aSearchAngle,
    float flCloseRange,
    const Vector2D * pwpFacing)
{
    ActorCommonData& tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    RActor* ptPlayerRActor = RActorGet(ptPlayer->idPlayerRActor);

    Sphere ws;
    ws.wpCenter = tACDPlayer.GetPlace().wp;
    ws.wdRadius = flSearchRadius;

    RActorIDListPooled listRActors(MemoryPoolsGetDATAIDListPool());
    RActorsIntersectSphere(ws, RACTORID_INVALID, listRActors);

    RActorID idRActorTarget = RACTORID_INVALID;

    float flCosSearchAngle = Math::Cos(aSearchAngle);
    POS pos = listRActors.GetHeadPosition();
    WorldDistance wdClosest = MAX_WORLDDISTANCE;

    WorldDistance wdCloseRangeClosest = flCloseRange * flCloseRange;
    RActorID idRActorCloseRangeTarget = RACTORID_INVALID;

    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorGet(idRActor);

        ActorCommonData* ptACD = ACDTryToGet(ptRActor->idACD);
        if (!ptACD ||
            !ptACD->IsAlive())
        {
            continue;
        }

        if (!ACDIsHostile(tACDPlayer, *ptACD, FALSE) &&
            !PowerCastTargetAllies(snoPower) &&
            !PowerCastTargetNeutral(snoPower))
        {
            continue;
        }

        Target tTestTarget;
        tTestTarget.InitializeFromACD(ptACD->id, FALSE, FALSE);

		WorldPlace placeCurrent = tACDPlayer.GetPlace();
        WorldPlace placeDest;
        tTestTarget.GetPlace(placeDest);


        if (!CPlayerUtilNeedsToGetCloser(ptPlayerRActor, snoPower, tTestTarget, 0.0f, 0.0f) &&
            PowerCanTarget(snoPower, ptPlayerRActor->idACD, &tTestTarget))
        {

            WorldDistance wdDistance = Vector3D::DistanceSquared(placeCurrent.wp, placeDest.wp);
            if (wdDistance < wdCloseRangeClosest)
            {
                wdCloseRangeClosest = wdDistance;
                idRActorCloseRangeTarget = idRActor;
            }
        }


		if (aSearchAngle < FULL_CIRCLE)
        {
			Vector2D vecToTarget(placeDest.wp.x - placeCurrent.wp.x, placeDest.wp.y - placeCurrent.wp.y);
            vecToTarget.Normalize();

            Vector2D vecFacing;
			if (pwpFacing)
            {

                vecFacing = *pwpFacing;
            }
            else
            {
				Angle aFacing = tACDPlayer.GetOrientation().ToYaw();
                vecFacing.x = Math::Cos(aFacing); vecFacing.y = Math::Sin(aFacing);
            }

            if (Vector2D::DotProduct(vecToTarget, vecFacing) < flCosSearchAngle)
            {
                continue;
            }
        }

        if (!CPlayerUtilNeedsToGetCloser(ptPlayerRActor, snoPower, tTestTarget, 0.0f, 0.0f) &&
            PowerCanTarget(snoPower, ptPlayerRActor->idACD, &tTestTarget))
        {

            WorldDistance wdDistance = Vector3D::DistanceSquared(placeCurrent.wp, placeDest.wp);
            if (wdDistance < wdClosest)
            {
                wdClosest = wdDistance;
                idRActorTarget = idRActor;
            }
        }
    }


    if (idRActorTarget == RACTORID_INVALID)
    {
        idRActorTarget = idRActorCloseRangeTarget;
    }

    return idRActorTarget;
}




RActorID CPlayerUtilGetNearbyStickyTargetedActorFromList(const Player * ptPlayer, const Vector3D & vBaseTargetPos, const SNO snoPower)



{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
    {
        return RACTORID_INVALID;
    }

    float flStickyTargetRadius = PowerGetStickyTargetRadius(ptACDPlayer, AutoPowerDefinitionR(snoPower));
    if (flStickyTargetRadius < 1e-6f)
    {
        return RACTORID_INVALID;
    }

    RActor * ptPlayerRActor = RActorGet(ptPlayer->idPlayerRActor);

    Sphere tSphere;
    tSphere.wpCenter = vBaseTargetPos;
    tSphere.wdRadius = flStickyTargetRadius;

    RActorIDListPooled listRActors(MemoryPoolsGetDATAIDListPool());
    RActorsIntersectSphere(tSphere, ptPlayer->idPlayerRActor, listRActors);

    RActorID idRActorClosest = RACTORID_INVALID;
    float flClosestDistSq = MAX_WORLDDISTANCE;

    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);

        const RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor || ptRActor->idACD == ACDID_INVALID)
        {
            continue;
        }

        const ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
        if (!ptACD)
        {
            continue;
        }

        if (sACDIsIgnored(*ptACD, ptPlayer->idPlayerACD, TRUE, snoPower))
        {
            continue;
        }

        Target tTestTarget;
        tTestTarget.InitializeFromACD(ptACD->id, 0, 0);
        if (CPlayerUtilNeedsToGetCloser(ptPlayerRActor, snoPower, tTestTarget, 0.0f, 0.0f))
        {
            continue;
        }

        float flDistSq = Vector3D::DistanceSquared(ptACD->GetPlace().wp, vBaseTargetPos);
        if (flDistSq < flClosestDistSq)
        {
            idRActorClosest = idRActor;
            flClosestDistSq = flDistSq;
        }
    }

    return idRActorClosest;
}


















static const float WALKING_ATTACK_RANGE_HYSTERESIS_PERCENT = 0.75f;

WorldDistance CPlayerUtilGetRangeFromDest(const ACDID idACDPlayer, const SNO snoPower, const Target & tActorTarget)



{
    WorldDistance wdAttackRange;
    PowerGetStartRange(idACDPlayer, AutoPowerDefinitionR(snoPower), tActorTarget, wdAttackRange);



    return wdAttackRange * WALKING_ATTACK_RANGE_HYSTERESIS_PERCENT;

}




BOOL CPlayerUtilNeedsToGetCloser(const RActor * ptPlayerRActor, SNO snoPower, const Target & tActorTarget, WorldDistance wdRange, WorldDistance wdLongReach)





{
    WorldPlace placeSource = ClientPointToPlace(ptPlayerRActor->tLocation.tWorldLocation.transform.wp);

    BOOL bCloseEnough = PowerIsCloseEnoughToPlace(ptPlayerRActor->idACD, placeSource, AutoPowerDefinitionR(snoPower), tActorTarget, wdRange, wdLongReach, NULL, FALSE);

    return !bCloseEnough;
}



BOOL CPlayerUtilPlayerSpecifiedDontMove(const Player * ptPlayer, BOOL bPrimaryActionKey, BOOL bHasActorTarget)



{
    if (Prefs::CheckAction(ACTION_UI_HOLD_POSITION, FALSE) ||









        ptPlayer->tTargetComponent.GetInAimMode() == TRUE || (bPrimaryActionKey && !bHasActorTarget) ||




        ptPlayer->tTargetComponent.GetTargetRangeAdjustment(SNO_NONE) > 1.0f)
    {
        return TRUE;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }




    SNO snoPower = SNO_NONE;
    {
        SNO snoAnimPower = SNO_NONE;
        AnimReason eAnimReason = AnimPlayerGetAnimReason(*ptACDPlayer);
        if (eAnimReason == ANIM_REASON_POWER || eAnimReason == ANIM_REASON_PREPLAY_POWER || eAnimReason == ANIM_REASON_PREPLAY_CHANNELLED)
        {



            snoAnimPower = ptPlayer->tClientInfo.snoCurrentOriginalAnimPower;
        }

        if (snoAnimPower == SNO_NONE && CGameGetClientTicks() <= ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime)
        {
            snoAnimPower = ptPlayer->tClientInfo.snoLastUsedPower;
        }





        if (snoAnimPower != SNO_NONE)
        {
            InputButton eLastSkillBinding = Controller::GetBindingFromSkill(snoAnimPower, ptPlayer);
            if (Console::ControllerUtils::IsButtonDown(ptPlayer->nControllerIndex, eLastSkillBinding) ||
                Console::ControllerUtils::IsButtonRepeated(ptPlayer->nControllerIndex, eLastSkillBinding))
            {
                snoPower = snoAnimPower;
            }
        }
    }


    if (snoPower == SNO_NONE)
    {
        snoPower = ptPlayer->tClientInfo.tCachedTargetLock.snoOriginalPower;
    }

    if (snoPower == SNO_NONE || PowerShouldUseClientWalking(*ptACDPlayer, AutoPowerDefinitionR(snoPower))) return FALSE;

    BOOL bDontMove = TRUE;
    {
        ScopedPowerDefinitionLockR pdefPower(snoPower);
        if (pdefPower->tPower.uFlags & (1 << POWERBIT_CONSOLE_ALLOW_WALK_BETWEEN_CASTS))
        {

            const RActor * ptPlayerRActor = RActorGet(ptPlayer->idPlayerRActor);
            ResolvedClick tResolvedClick;
            CPlayerUtilResolveConsoleButton(snoPower, tResolvedClick, ptPlayer, TRUE, TRUE, FALSE);

            bDontMove = tResolvedClick.tTarget.IsTargetingACD() && !CPlayerUtilNeedsToGetCloser(ptPlayerRActor, snoPower, tResolvedClick.tTarget, 0.0f, 1.0f);
        }
    }

    return bDontMove;

}



BOOL CPlayerUtilCanInterruptComboNow()
{


    return FALSE;
}




void CPlayerUtilSpawnClickEffect(const Target & tTarget)

{
    SNO snoActorEffect = GlobalSNOGet(GLOBALSNO_ACTOR_PLAYER_CLICKED);
    if (snoActorEffect != SNO_NONE &&

        AppDrawFlagIsSet(APP_DRAW_CLICK_EFFECT_BIT))
    {

        WorldPlace placeClick;
        tTarget.GetPlace(placeClick);
        RActorCreate(GlobalSNOGet(GLOBALSNO_ACTOR_PLAYER_CLICKED), 1.0f, 0.0f, PRTransform(cgqIdentity, placeClick.wp), 0, RENDERCONTEXT_DEFAULT, SSCENEID_INVALID);
    }





}



void CPlayerUtilSpawnPowerClickEffect(SNO snoPower, const Target & tTarget)


{
    SNO snoActorEffect = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_CAST_TARGET_CLIENT_EFFECT_ACTOR);
    if (snoActorEffect != SNO_NONE &&

        AppDrawFlagIsSet(APP_DRAW_CLICK_EFFECT_BIT))
    {

        WorldPlace placeClick;
        tTarget.GetPlace(placeClick);
        RActorCreate(snoActorEffect, 1.0f, 0.0f, PRTransform(cgqIdentity, placeClick.wp), 0, RENDERCONTEXT_DEFAULT, SSCENEID_INVALID);
    }





}

#include "../Common/Nav/NavZoneIntersect.h"
#include "../Common/Nav/NavZone.h"
#include "../Common/Nav/NavigationMesh.h"
#include "../Sigma/Math/Ray.h"
#include "../Sigma/Math/Plane.h"
#include "../Sigma/Math/Intersect.h"
#include "StructureOctree.h"
// DWARF Selection.cpp:1063, CameraManager.cpp:1036, SceneCommon.cpp:1340, NavigationMesh.cpp:786, NavZoneIntersect.cpp:1506.
BOOL ScreenPickGround(const IVector2D &, Vector3D &, uint32, SSceneID, SSceneID *, SNO *, int32 *, WorldDistance *);
void ScreenPointToWorldLineSeg(const IVector2D &, LineSeg3D &, int32);
BOOL ScenesCommonLineIntersectNavMeshNode(SWorldID, const LineSeg3D &, NavMeshHandle *, WorldPlace *);
BOOL NavMeshGetHeightAndNormal(const NavMeshHandle &, const Vector3D &, BOOL, WorldDistance &, Vector3D &);
BOOL NavZoneCollideSweptPoint(const NavCellHandle &, const Vector2D &, const Vector2D &, uint16, uint16, uint32, NavCellCollideResult &);
// Inference: append with original logical lines because existing declarations occupy the original 48-65 span.
#line 47
const float WALL_COSANG = 0.5f;
#line 47
// NON_MATCHING: 1380/1380 bytes; five multiply/register operand differences in inherited Plane::Set; tried collision-place then current-intersection destination initialization.
void CPlayerUtilPlayerClick(
    const Player * ptPlayer,
    const IVector2D & p,
    const PlayerClickIntersectType eIntersectType,
    Vector3D & wpIntersect,
    const float flPlaneOffsetDist)
{
    switch (eIntersectType)
    {
        case PLAYER_CLICK_INTERSECT_APPEARANCE:
        { // Inference: DWARF mask69 is the three StructureOctree bits0,2,6.
            uint32 dwIntersectFlags = (1 << INTERSECTFLAGS_IGNORE_NOT_WALKABLE) | (1 << INTERSECTFLAGS_NO_CULL) | (1 << INTERSECTFLAGS_IGNORE_FADE_SURFACES_BIT);



            Vector3D wpGround;

            BOOL bHitGround = ScreenPickGround(p, wpGround, dwIntersectFlags, -1, NULL, NULL, NULL, NULL);

            wpIntersect = wpGround;
            // Inference: the shared recursive fallback follows a failed pick (original +0x88).
            if (!bHitGround)
                CPlayerUtilPlayerClick(ptPlayer, p, PLAYER_CLICK_INTERSECT_GROUND_PLANE, wpIntersect, 0.0f);
            break;
        }



        case PLAYER_CLICK_INTERSECT_NAV_MESH:
        {
            LineSeg3D wline;
            ScreenPointToWorldLineSeg(p, wline, -1);
            // Inference: both handle fields are initialized to the original -1 sentinel.
            NavMeshHandle tNavMeshHandle = {-1, -1};
            SWorldID idSWorld = CGameGetCurrentSWorldID();

            WorldPlace placeCollision;
            if (ScenesCommonLineIntersectNavMeshNode(idSWorld, wline, &tNavMeshHandle, &placeCollision))
            {
                wpIntersect = placeCollision.wp;

                if (NavMeshHandleGetStaticFlag(tNavMeshHandle, CELL_STATIC_ALLOW_WALK))
                    break;
            }
            // Inference: the failed intersection and non-walkable paths share the original recursive fallback.
            CPlayerUtilPlayerClick(ptPlayer, p, PLAYER_CLICK_INTERSECT_GROUND_PLANE, wpIntersect, 0.0f);
            break;
        }





        case PLAYER_CLICK_INTERSECT_WALKING:
        {
            LineSeg3D wline;
            ScreenPointToWorldLineSeg(p, wline, -1);
            // Inference: both handle fields use the original -1 sentinel.
            NavMeshHandle tNavMeshHandle = {-1, -1};
            SWorldID idSWorld = CGameGetCurrentSWorldID();

            WorldPlace placeCollision;
            if (ScenesCommonLineIntersectNavMeshNode(idSWorld, wline, &tNavMeshHandle, &placeCollision))
            {
                wpIntersect = placeCollision.wp;

                if (!NavMeshHandleGetStaticFlag(tNavMeshHandle, CELL_STATIC_ALLOW_WALK))
                {
                    Vector3D vNormal;
                    float flZ;
                    NavMeshGetHeightAndNormal(tNavMeshHandle, placeCollision.wp, FALSE, flZ, vNormal);
                    float flCosAng = Vector3D::DotProduct(vNormal, cgwvZAxis);
                    if (flCosAng <= WALL_COSANG)
                    {

                        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                        WorldPlace placeCurr = tACDPlayer.GetPlace();



                        NavCellHandle hNavCell;
                        NavCellHandleFromWorldPlace(placeCurr, hNavCell);

                        NavCellCollideResult tStaticResult;
                        Vector2D vDir2D = wpIntersect.As2D() - placeCurr.wp.As2D();
                        NavZoneCollideSweptPoint(hNavCell, placeCurr.wp.As2D(), vDir2D, (1 << CELL_STATIC_ALLOW_WALK), 0, (1 << NAV_ZONE_COLL_ROUNDED_CORNERS) | (1 << NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY), tStaticResult);






                        // Inference: optimized-out WorldPlace takes current intersection and world, preserving z while replacing collision xy.
                        WorldPlace placeDest = {wpIntersect, placeCurr.idSWorld};

                        placeDest.wp.As2D() = tStaticResult.wpEnd;



                        wpIntersect = placeDest.wp;
                        break;
                    }
                }
                else
                    break;
            }
            // Inference: non-wall failures share the ground-plane recursive fallback.
            CPlayerUtilPlayerClick(ptPlayer, p, PLAYER_CLICK_INTERSECT_GROUND_PLANE, wpIntersect, 0.0f);
            break;
        }





        case PLAYER_CLICK_INTERSECT_GROUND_PLANE:
        {
            const RActor * ptPlayerRActor = RActorGet(ptPlayer->idPlayerRActor);
            Vector3D wpActor = ptPlayerRActor->tLocation.tWorldLocation.transform.wp;




            LineSeg3D wline;
            ScreenPointToWorldLineSeg(p, wline, -1);

            Plane wplane;
            wplane.Set(wpActor, cgwvZAxis);

            Vector3D wpPlane;
            BOOL bHitPlane = Intersect::Ray3DAndPlane(wline.wr, wplane, wpPlane);

            if (bHitPlane)
            {
                wpIntersect = wpPlane;
            }
            else
            {
                wpIntersect = wpActor;
            }
            break;
        }
        case PLAYER_CLICK_INTERSECT_CONTACT_OFFSET_PLANE_FROM_NAV_MESH:
        {
            LineSeg3D wline;
            ScreenPointToWorldLineSeg(p, wline, -1);
            // Inference: both handle fields use the original -1 sentinel.
            NavMeshHandle tNavMeshHandle = {-1, -1};
            SWorldID idSWorld = CGameGetCurrentSWorldID();

            WorldPlace placeCollision;

            if (ScenesCommonLineIntersectNavMeshNode(idSWorld, wline, &tNavMeshHandle, &placeCollision))
            {


                placeCollision.wp.z += flPlaneOffsetDist;


                Plane wplane;
                wplane.Set(placeCollision.wp, cgwvZAxis);


                Vector3D wpPlane;
                BOOL bHitPlane = Intersect::Ray3DAndPlane(wline.wr, wplane, wpPlane);

                if (bHitPlane)
                {
                    wpIntersect = wpPlane;
                }
                else
                {
                    const RActor * ptPlayerRActor = RActorGet(ptPlayer->idPlayerRActor);
                    Vector3D wpActor = ptPlayerRActor->tLocation.tWorldLocation.transform.wp;
                    wpIntersect = wpActor;
                }
            }
            else
                CPlayerUtilPlayerClick(ptPlayer, p, PLAYER_CLICK_INTERSECT_GROUND_PLANE, wpIntersect, 0.0f);
            break;
        }


        case PLAYER_CLICK_INTERSECT_CONTACT_OFFSET_PLANE_FROM_GROUND_PLANE:
        {
            const RActor * ptPlayerRActor = RActorGet(ptPlayer->idPlayerRActor);


            Vector3D wpActor = ptPlayerRActor->tLocation.tWorldLocation.transform.wp;
            wpActor.z += flPlaneOffsetDist;

            LineSeg3D wline;
            ScreenPointToWorldLineSeg(p, wline, -1);

            Plane wplane;
            wplane.Set(wpActor, cgwvZAxis);

            Vector3D wpPlane;
            BOOL bHitPlane = Intersect::Ray3DAndPlane(wline.wr, wplane, wpPlane);

            if (bHitPlane)
            {
                wpIntersect = wpPlane;
            }
            else
            {

                wpIntersect = wpActor;
            }
            break;
        }
    }


}
