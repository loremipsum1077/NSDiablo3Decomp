// Original file: X/Server/TownPortal.cpp
#include "./TownPortal.h"
#include "./Actors/Actors.h"
#include "./Map/Minimap.h"
#include "./Powers/Powers.h"
#include "./SWorld.h"
#include "./SPlayer.h"
#include "./SScene.h"
#include "./AI/BrainComponents.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Definitions/MarkerSetDefinition.h"
#include "../Common/GameSwig.h"
#include "../Common/InactiveMarker.h"
#include "../Common/MarkerSet.h"
#include "../Common/Marker/Marker.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/TagMaps/MarkerTags.h"
#include "../Common/TagMaps/ActorTags.h"
#include "./SGame.h"
#include "../Common/TagMaps/Tags.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Common/ActorCommonData.h"
#include "../Common/AnimPlayer.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Attributes/FastAttributeStructs.h"
#include "../Common/Definitions/ActDefinition.h"
#include "../Common/GizmoSwig.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Player.h"
#include "../Common/TagMaps/AnimTagsSwig.h"
#include "../Common/World.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/MemoryPools.h"

void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest);  // X/Common/Act.cpp:260
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // X/Common/World.cpp:605
ACDID SWorldGetPortalDestinationACD(const SWorldID idSWorld, const uint32 uEntranceGUID);  // X/Server/SWorld.cpp:1375
void GizmoSetHearthPortal(Player & tPlayer, const WorldPlace & placeReturn, const BOOL bSendToClient);  // X/Server/SGizmo.cpp:3484
BOOL SPlayerFindGoodSpawnLocation(const WorldPlace & placeStart, WorldPlace & placeResult, float flSearchRadius);  // X/Server/SPlayer.cpp:2289
void ActorChangeWorld(const ActorID idActor, const WorldPlace & tPlace, const Quaternion & q);  // X/Server/Actors/Actors.cpp:6093
void SPlayerWarpedThroughPortal(ActorCommonData & tACDPlayer, const ACDID idACDPortalFrom, const ACDID idACDPortalTo, const PlayerWarpReason eReason, const WorldPlace * ptPlaceBeforeWarp);  // X/Server/SPlayer.cpp:8807
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame);  // X/Server/Actors/ActorPowers.cpp:392
int32 SWorldGetTieredDungeonFinderLevel(SWorldID idSWorld);  // X/Server/SWorld.cpp:3357
void SceneCommonDataActivate(const SSceneID idSScene);  // X/Common/SceneCommon.cpp:729
void SPlayerWorldCreatingBuffApplyToLocalPlayers(ActorCommonData & tACDPlayer);  // X/Server/SPlayer.cpp:8377
void SPlayerWorldCreatingBuffRemove(ActorCommonData & tACDPlayer);  // X/Server/SPlayer.cpp:8460
BOOL BrainAPIResurrectByType(ACDID idACD, RevivalType eRevivalType);  // X/Server/AI/BrainComponents.cpp:2981
void SendPlayerCancelWarp(const ACDID idACDPlayer);  // X/Server/Network/ServerMessageSend.cpp:1403
SNO ACDGetLevelArea(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:4421
int32 MarkerGetACDsLinkedByActor(const ActorID idActor, XPooledDataIDList & listACDs, LPCSTR pszGroupName);  // X/Server/Marker/SMarker.cpp:1231

//@ 110
ACDID TownPortalFindLocationMarkerACD(BOOL bCausedByLocalPlayer)
{
    ResolvedPortalDestination tDest;

    ActGetCurrentStartLocation(tDest);


    SWorldID idSWorld = WorldCommonFindBySNO(tDest.snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
    {
        idSWorld = SWorldCreate(tDest.snoWorld, TRUE, bCausedByLocalPlayer, NULL, ACDID_INVALID, ACDID_INVALID, tDest.uEntranceGUID, 0, FALSE);
    }

    ACDID idACDDest = SWorldGetPortalDestinationACD(idSWorld, tDest.uEntranceGUID);







    return idACDDest;
}

// Only the ActorTryToGet call (line 139) survives in the binary; the rest of this function
// (lines 140-159) is not recoverable and is left out.
//@ 136
static void sTownPortalDeletePair(ActorID idPortal)
{

    Actor * ptPortal = ActorTryToGet(idPortal);
    (void)ptPortal;
}

//@ 162
static void sPortalToPlace(ActorCommonData & tACDOperator, ACDID idACDPortal, ACDID idACDOppositePortal, const WorldPlace & placeDesiredDest, const Quaternion & qDest, PlayerWarpReason eWarpReason)






{

    WorldPlace placeOpenDest;
    SPlayerFindGoodSpawnLocation(placeDesiredDest, placeOpenDest, 50.0f);

    WorldPlace placeOperatorBeforeWarp = tACDOperator.GetPlace();
    ActorID idActorOperator = (ActorID)ACDGetOwner(tACDOperator.id);
    ActorChangeWorld(idActorOperator, placeOpenDest, qDest);


    SPlayerWarpedThroughPortal(tACDOperator, idACDPortal, idACDOppositePortal, eWarpReason, &placeOperatorBeforeWarp);




    ActorCommonData * ptACDOppositePortal = ACDTryToGet(idACDOppositePortal);
    if (ptACDOppositePortal)
    {
        ActorID idActorOppositePortal = (ActorID)ACDGetOwner(idACDOppositePortal);
        Actor * ptActorOppositePortal = ActorTryToGet(idActorOppositePortal);


        if (ptActorOppositePortal)
        {
            SNO snoMarkerSet = ptActorOppositePortal->tMarkerOwner.snoMarkerSet;
            if (snoMarkerSet != SNO_NONE)
            {


                ScopedMarkerSetDefinitionLockR ptMarkerSet(snoMarkerSet);
                if (ptActorOppositePortal->tMarkerOwner.nMarkerIndex >= 0 && ptActorOppositePortal->tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
                {
                    const Marker & tMarker = ptMarkerSet->tMarkerSet[ptActorOppositePortal->tMarkerOwner.nMarkerIndex];

                    ActorAnimationTag eAnimTag = (ActorAnimationTag)TagMapGetInt(tMarker.hTagMap, TAG_MARKER_PORTAL_TAKEN_ANIM_TAG);


                    if (eAnimTag != (ActorAnimationTag)-1)
                    {
                        if (tACDOperator.IsLoading())
                        {
                            tACDOperator.AttributesSetInt(FastAttribKey(ATTRIBUTE_LOADING_ANIM_TAG), eAnimTag);
                        }
                        else
                        {
                            ActorPowersInterruptPrimary(idActorOperator, TRUE);
                            BrainAPIDoPlayAnimation(idActorOperator, eAnimTag, (uint32)-1, TRUE, FALSE, 0.0f, FALSE, 1.0f);
                        }
                    }
                }
            }
        }
    }

    FastAttribSetValueInt(tACDOperator.idFastAttrib, FastAttribKey(ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL), SWorldGetTieredDungeonFinderLevel(placeDesiredDest.idSWorld));
}

//@ 226
static void sPortalWorldCreated(SWorldID idSWorld, ACDID idACDOperator, ACDID idACDPortal, uint32 uEntranceGUID, PlayerWarpReason eWarpReason)





{

    ActorCommonData * ptACDOperator = ACDTryToGet(idACDOperator);
    if (ptACDOperator == NULL)
    {
        return;
    }



    if (eWarpReason != WARP_CONSOLE_PORTAL)
    {

        Player * ptActingPlayer = PlayerGetByACD(idACDOperator);

        if (ptActingPlayer == NULL)
        {
            return;
        }


        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (PlayersAreLocalToEachOther(*ptActingPlayer, *ptPlayer))
            {
                if (PlayerIsHardcoreAndDead(*ptPlayer))
                {
                    SPlayerDisconnect(*ptPlayer, 8, FALSE);
                }
                else if (!ACDGet(ptPlayer->idPlayerACD).IsAlive())
                {
                    BrainAPIResurrectByType(ptPlayer->idPlayerACD, REVIVAL_TYPE_AT_CORPSE_NOW_DO_NOT_WARP);
                }

                sPortalWorldCreated(idSWorld, ptPlayer->idPlayerACD, idACDPortal, uEntranceGUID, WARP_CONSOLE_PORTAL);
            }
        }

        return;
    }


    SPlayerWorldCreatingBuffRemove(*ptACDOperator);

    ACDID idACDOppositePortal = SWorldGetPortalDestinationACD(idSWorld, uEntranceGUID);
    if (idACDOppositePortal == ACDID_INVALID)
    {







        SendPlayerCancelWarp(idACDOperator);
        return;
    }



    ActorCommonData & tACDOppositePortal = ACDGet(idACDOppositePortal);
    Quaternion qDest = tACDOppositePortal.GetOrientation();

    sPortalToPlace(*ptACDOperator, idACDPortal, idACDOppositePortal, tACDOppositePortal.GetPlace(), qDest, WARP_CONSOLE_PORTAL);
}

//@ 296
BOOL PortalUse(ACDID idACDPortal, ActorCommonData & tACDOperator, const PlayerWarpReason eWarpReason)



{

    if (!tACDOperator.IsAlive())
    {
        return FALSE;
    }


    ActorID idActorPortal = (ActorID)ACDGetOwner(idACDPortal);
    Actor * ptActorPortal = ActorGet(idActorPortal);

    ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);
    if (ptACDPortal && ptACDPortal->GetGizmoType() == ACTORGIZMO_RETURN_PORTAL)
    {
        SWorldID idSWorldCurrent = tACDOperator.GetSWorldID();
        SWorld * ptSWorldCurrent = SWorldTryToGet(idSWorldCurrent);
        if (ptSWorldCurrent == NULL)
        {
            return FALSE;
        }




        WorldPlace & placeReturn = ptSWorldCurrent->tReturnPortalLocation.placeReturn;
        SWorld * ptSWorldReturn = SWorldTryToGet(placeReturn.idSWorld);
        if (ptSWorldReturn == NULL)
        {
            return FALSE;
        }





        if (!SWorldIsCreated(*ptSWorldReturn))
        {
            return FALSE;
        }





        if (SSceneTryToGet(ptSWorldCurrent->tReturnPortalLocation.idSScene) == NULL)
        {
            return FALSE;
        }





        SceneCommonDataActivate(ptSWorldCurrent->tReturnPortalLocation.idSScene);
        sPortalToPlace(tACDOperator, idACDPortal, ACDID_INVALID, placeReturn, cgqIdentity, eWarpReason);
        return TRUE;
    }

    ResolvedPortalDestination tResolvedDest;
    ActorResolvePortalDestination(ptActorPortal, tResolvedDest);

    SWorldID idSWorld = SWorldFindBySNO(tResolvedDest.snoWorld);
    if (idSWorld == SWORLDID_INVALID)
    {
        if (!SNOFileExists(WORLD_SNO, tResolvedDest.snoWorld))
        {
            return FALSE;
        }




        SPlayerWorldCreatingBuffApplyToLocalPlayers(tACDOperator);

        SWorldCreate(tResolvedDest.snoWorld, FALSE, FALSE, sPortalWorldCreated, tACDOperator.id, idACDPortal, tResolvedDest.uEntranceGUID, eWarpReason, FALSE);
    }
    else
    {

        SWorld * ptSWorld = SWorldTryToGet(idSWorld);
        if (ptSWorld)
        {
            if (SWorldIsCreated(*ptSWorld))
            {
                sPortalWorldCreated(idSWorld, tACDOperator.id, idACDPortal, tResolvedDest.uEntranceGUID, eWarpReason);
            }
            else
            {

                ptSWorld->AddCreateCallback(sPortalWorldCreated, tACDOperator.id, idACDPortal, tResolvedDest.uEntranceGUID, eWarpReason);
            }
        }
    }


    return TRUE;
}

//@ 394
void HearthPortalUse(const ActorCommonData & tACDPortal, ActorCommonData & tACDPlayer)


{
    ActorCommonData * ptACDPlayer;


    Player * ptOriginalPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptOriginalPlayer == NULL)
    {
        return;
    }

    if (!ptOriginalPlayer->tHearthPortalInfo.IsValid())
    {
        return;
    }





    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayersAreLocalToEachOther(*ptPlayer, *ptOriginalPlayer))
        {



            ptACDPlayer = &ACDGet(ptPlayer->idPlayerACD);


            PowerStartData tPowerStartData;
            PowerStartDataInit(tPowerStartData);
            tPowerStartData.idACD = ptACDPlayer->id;
            tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ENTER_RECALL_PORTAL);
            PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
        }
    }



    PingID idPing = tACDPortal.AttributesGetInt(FastAttribKey(ATTRIBUTE_PING));
    Minimap::RemovePing(idPing);
}

//@ 450
void TownPortalsDeleteForPlayer(Player & tPlayer)
{

    if (tPlayer.idActorTownPortalInUse != ACTORID_INVALID)
    {
        sTownPortalDeletePair(tPlayer.idActorTownPortalInUse);
        tPlayer.idActorTownPortalInUse = ACTORID_INVALID;
    }

    GizmoSetHearthPortal(tPlayer, cgplaceNull, TRUE);
}

//@ 491
void SecretPortalOperate(const ACDID idACDPortal, const ACDID)


{

    ActorCommonData & tACDPortal = ACDGet(idACDPortal);

    int32 nGizmoState = tACDPortal.AttributesGetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE));
    if (nGizmoState == -1)
    {

        tACDPortal.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 0);



        ActorID idActorPortal = (ActorID)ACDGetOwner(tACDPortal.id);
        Actor * ptActorPortal = ActorGet(idActorPortal);

        ResolvedPortalDestination tResolvedDest;
        ActorResolvePortalDestination(ptActorPortal, tResolvedDest);

        *tACDPortal.ptResolvedDest = tResolvedDest;


        tACDPortal.ModifiedMark(ACDMODIFIED_PORTAL_SPECIFIER);

        AnimPlayerPlayAnimation(tACDPortal, TAG_ACTOR_ANIMATION_OPENING, (uint32)-1, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
        AnimPlayerSetIdleAnimation(tACDPortal, TAG_ACTOR_ANIMATION_OPEN);
    }
    else
    {






        // The result of this read (line 527) is unused in the binary; what tested it is not recoverable.
        tACDPortal.AttributesGetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE));








        tACDPortal.AttributesSetInt(FastAttribKey(ATTRIBUTE_GIZMO_STATE), 1);
    }
}

//@ 543
ACDID SPortalGetLinkedReturnLocation(ACDID idACDPortal)
{

    ActorID idActor = (ActorID)ACDGetOwner(idACDPortal);


    XPooledDataIDList listLinkedACDs(MemoryPoolsGetDATAIDListPool());
    MarkerGetACDsLinkedByActor(idActor, listLinkedACDs, NULL);

    POS pos = listLinkedACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listLinkedACDs.GetNext(pos);
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD && ptACD->IsGizmo() && ptACD->GetGizmoType() == ACTORGIZMO_PORTAL_DESTINATION)
        {
            return idACD;
        }
    }

    return ACDID_INVALID;
}

ACDID SPortalGetLinkedReturnLocation(ACDID idACDPortal);

//@ 568
void SPortalResolveDynamicReturnLocation(ACDID idACDPortalOrigin, ResolvedPortalDestination & tPortalReturnLocation)

{
    Actor * ptActor = ActorGet((ActorID)ACDGetOwner(idACDPortalOrigin));
    ActorCommonData * ptACDPortalOrigin = ACDTryToGet(idACDPortalOrigin);




    if (ptACDPortalOrigin && ptACDPortalOrigin->AttributesGetInt(FastAttribKey(ATTRIBUTE_LINKED_DYNAMIC_ENTRANCE_GUID)))
    {
        tPortalReturnLocation.uEntranceGUID = ptACDPortalOrigin->AttributesGetInt(FastAttribKey(ATTRIBUTE_LINKED_DYNAMIC_ENTRANCE_GUID));
        tPortalReturnLocation.snoDestLevelArea = ACDGetLevelArea(*ptACDPortalOrigin);
        tPortalReturnLocation.snoWorld = WorldCommonGetWorldSNO(ptACDPortalOrigin->GetSWorldID());
        return;
    }


    if (ptActor->tMarkerOwner.snoMarkerSet == SNO_NONE)
    {
        return;
    }



    if (!ActorGetInheritedTagMapValueDWORD(ptActor, TAG_ACTOR_GIZMO_PORTAL_SETS_DYNAMIC_RETURN_LOCATION, TAG_MARKER_PORTAL_SETS_DYNAMIC_RETURN_LOCATION))
    {
        return;
    }




    ACDID idACDReturnLocation = SPortalGetLinkedReturnLocation(idACDPortalOrigin);


    if (idACDReturnLocation == ACDID_INVALID)
    {
        idACDReturnLocation = idACDPortalOrigin;
    }


    if (idACDReturnLocation != ACDID_INVALID)
    {
        ActorCommonData & tACDReturnLocation = ACDGet(idACDReturnLocation);


        if (tACDReturnLocation.AttributesGetInt(FastAttribKey(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID)) == 0)
        {

            SWorld * ptSWorldOrigin = SWorldGet(tACDReturnLocation.GetSWorldID());
            uint32 uStartingLocationGUID = SGameGlobalsGet().uNextDynamicEntranceGUID++;
            tACDReturnLocation.AttributesSetInt(FastAttribKey(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID), uStartingLocationGUID);


            Actor * ptActorReturnLocation = ActorGet((ActorID)ACDGetOwner(tACDReturnLocation.id));

            EntranceLocation tEntranceLocation;
            tEntranceLocation.idSScene = tACDReturnLocation.GetWorldLocation().idSScene;
            tEntranceLocation.snoMarker = ptActorReturnLocation->tMarkerOwner.snoMarkerSet;
            tEntranceLocation.tName = ptActorReturnLocation->tName;
            ptSWorldOrigin->SetEntranceLocation(uStartingLocationGUID, tEntranceLocation);
        }


        tPortalReturnLocation.snoDestLevelArea = ACDGetLevelArea(tACDReturnLocation);
        tPortalReturnLocation.uEntranceGUID = tACDReturnLocation.AttributesGetInt(FastAttribKey(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID));
        tPortalReturnLocation.snoWorld = WorldCommonGetWorldSNO(tACDReturnLocation.GetSWorldID());
    }
}
