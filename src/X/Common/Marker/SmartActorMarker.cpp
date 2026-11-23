// Original file: X/Common/Marker/SmartActorMarker.cpp
//
// Complete.

#include "X/Common/Marker/SmartActorMarker.h"
#include "X/Client/CGame.h"
#include "X/Client/RActors/RActors.h"
#include "X/Client/Scene.h"
#include "X/Common/Definitions/ActorDefinition.h"
#include "X/Common/GameCommonData.h"
#include "X/Common/ActorCommonData.h"
#include "X/Common/Definitions/QuestRangeDefinition.h"
#include "X/Common/GameBalance.h"
#include "X/Common/Hyperlink.h"
#include "X/Common/SceneCommon.h"
#include "X/Server/Actors/Actors.h"
#include "X/Server/SGame.h"
#include "X/Common/GizmoSwig.h"
#include "X/Common/Marker/Marker.h"
#include "X/Common/NodePathSwig.h"
#include "X/Common/TagMaps/ActorTags.h"
#include "X/Common/TagMaps/MarkerTags.h"
#include "X/Common/TagMaps/Tags.h"
#include "X/Sigma/Math/Sphere.h"
#include "X/Sigma/Math/Transform.h"
#include "X/Sigma/Random.h"
#include "X/Sigma/RefString.h"
#include "X/Sigma/TypeDescriptor/SymbolicValue.h"

// X/Common/SNO/SymbolicValue.cpp:173 and :5221.
extern const SymbolicEntry gActorGizmoNames[47];
extern const SymbolicEntry gActorTypes[13];

BOOL ACDGroupExists(uint32 uGroupHash);  // X/Common/ActorCommonData.cpp:7998
Team TeamGetForMarker(const Marker & tMarker);  // X/Common/TeamCommon.cpp:370
BOOL FindGBHandleByActor(SNO snoActor, GBHandle & gbHandle);  // X/Common/Definitions/GameBalanceDefinition.cpp:3415
ActorID GBAdd(ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp:6041
ActorID SceneryAdd(const ActorAddStruct & tActorAddOriginal);  // X/Server/Actors/Actors.cpp:5865
ActorID PropAddBySNO(ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp:5847
ActorID GizmoAddBySNO(int32 nLevel, ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp:5803
SNO ScenesCommonLevelAreaFromPlace(SSceneID idSScene, Vector3D wp, BOOL fProcessInactive);  // X/Common/SceneCommon.cpp:1291
typedef int32 BehaviorType;  // DWARF type of ActorCanWalk's local; its header is not in src







void ActorMarker::GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds)
{



    SNO snoActor = tMarker.snoname.snoHandle;
    float flScale = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_SCALE);

    ScopedActorDefinitionLockR ptDefActor(snoActor);
    if (TagMapGetInt(ptDefActor->hTagMap, TAG_ACTOR_USE_ABSOLUTE_ORIENTATION))
    {
        Angle aAngle = TagMapGetFloat(ptDefActor->hTagMap, TAG_ACTOR_ABSOLUTE_ORIENTATION);
        transform.q.FromYaw(aAngle);
    }
    Vector3D wpSphereCenter = ptDefActor->wsBounds.wpCenter;
    WorldDistance wdSphereRadius = ptDefActor->wsBounds.wdRadius;


    wpSphereCenter = wpSphereCenter * flScale;
    Vector3D wpCenter = transform.q.Rotate(wpSphereCenter) + transform.wp;

    wsBounds.Set(wpCenter, wdSphereRadius * flScale);
}

void ActorMarker::GetAABB(const Marker & tMarker, AABB & aabbBounds)
{


    float flScale = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_SCALE);
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;
    GetActorAABB(tMarker.snoname.snoHandle, transform, flScale, aabbBounds);
}

CRefString ActorMarker::GetMarkerTypeString(const Marker * ptMarker) const
{

    if (ptMarker == NULL)
    {
        return MarkerBase::GetMarkerTypeString(ptMarker);
    }

    const SNO snoActor = ptMarker->snoname.snoHandle;
    ScopedActorDefinitionLockR pdefActor(snoActor);
    CRefString sString("Actor-");

    if (pdefActor->eType == ACTORTYPE_GIZMO)
    {
        GizmoType eGizmoType = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
        sString += SymbolToName(eGizmoType, gActorGizmoNames);
    }
    else
    {
        sString += SymbolToName(pdefActor->eType, gActorTypes);
    }
    return sString;
}

BOOL ActorMarker::IsForClient(const Marker & tMarker)
{



    if (TagMapGetInt(tMarker.hTagMap, TAG_MARKER_CLIENT_ONLY))
    {
        return TRUE;
    }


    const SNO snoActor = tMarker.snoname.snoHandle;
    ScopedActorDefinitionLockR pdefActor(snoActor);

    switch (pdefActor->eType)
    {
    case ACTORTYPE_AXE_SYMBOL:
    {
        PathClass nPathClass = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PATH_ID);
        return nPathClass == PATH_INVALID;
    }
    case ACTORTYPE_CLIENT_EFFECT:
    case ACTORTYPE_ENVIRONMENT:
    case ACTORTYPE_CRITTER:
    case ACTORTYPE_ITEM:
        return TRUE;
    }

    return FALSE;







}

BOOL ActorMarker::IsSpecialServerMarker(const Marker & tMarker)
{


    ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
    GizmoType nGizmoType = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);

    switch (nGizmoType)
    {
    case ACTORGIZMO_PORTAL: case ACTORGIZMO_WAYPOINT: case ACTORGIZMO_PORTAL_DESTINATION:
    case ACTORGIZMO_PAGE_OF_FATE_PORTAL: case ACTORGIZMO_BOSS_PORTAL: case ACTORGIZMO_PLACED_LOOT:
    case ACTORGIZMO_SAVEPOINT: case ACTORGIZMO_SAVEPOINT_PORTAL: case ACTORGIZMO_DUNGEON_PORTAL:
        return TRUE;
    }


    return nGizmoType == ACTORGIZMO_HEARTH_PORTAL;
}

BOOL ActorMarker::IsConsideredInMarkerSetBounds(const Marker & tMarker)
{

    ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
    return !TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_NOT_CONSIDERED_IN_MARKERSET_BOUNDS);





}

BOOL ActorMarker::IsPathMarker(const Marker & tMarker)
{

    if (tMarker.snoname.snoHandle != SNO_NONE)
    {
        ScopedActorDefinitionLockR ptActor(tMarker.snoname.snoHandle);

        if (TagMapGetInt(ptActor->hTagMap, TAG_ACTOR_PATH_ID) != PATH_INVALID)
        {
            return TRUE;
        }
    }


    return FALSE;
}

DataID ActorMarker::CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner)





{


    if (idSScene != SSCENEID_INVALID && TagMapTagExists(tMarker.hTagMap, TAG_MARKER_ONLY_SPAWN_IN_LEVEL_AREA))
    {
        SNO snoLevelAreaSpawn = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_ONLY_SPAWN_IN_LEVEL_AREA);

        if (snoLevelAreaSpawn != SNO_NONE)
        {
            SceneCommonData & tSCD = SceneCommonDataGet(idSScene);

            SNO snoLevelAreaScene = ScenesCommonLevelAreaFromPlace(idSScene, placePosition.wp, FALSE);

            if (snoLevelAreaScene != snoLevelAreaSpawn)
            {
                return DATAID_INVALID;
            }
        }
    }










    float flScale = CreateNewActorScale(tMarker, SGameGlobalsGet().rng);
    SNO snoConvList = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_CONVERSATION_LIST);
    uint32 uPrimaryGroup = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_PRIMARY_GROUP);
    uint32 uSecondaryGroup = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_SECONDARY_GROUP);

    if (uPrimaryGroup != (uint32)-1 && !ACDGroupExists(uPrimaryGroup))
    {
        CRefString sMarkerSet = HyperlinkCreateFromSNO(SNOName(MARKERSET_SNO, tMarkerOwner.snoMarkerSet), NULL);


        uPrimaryGroup = (uint32)-1;
    }
    if (uSecondaryGroup != (uint32)-1 && !ACDGroupExists(uSecondaryGroup))
    {
        CRefString sMarkerSet = HyperlinkCreateFromSNO(SNOName(MARKERSET_SNO, tMarkerOwner.snoMarkerSet), NULL);


        uSecondaryGroup = (uint32)-1;
    }
    if (!MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker, CONVERSATIONLIST_SNO, snoConvList))
    {
        snoConvList = SNO_NONE;
    }

    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_IS_FROM_MARKER);




    ActorAddStruct tActorAdd;
    tActorAdd.idMarkersSScene = idSScene;
    tActorAdd.tPlace = placePosition;
    tActorAdd.qOrientation = qOrientation;
    tActorAdd.flScale = flScale;
    tActorAdd.tMarkersName = tMarker.dwHash;

    SNO snoScript = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_SCRIPT);
    if (snoScript != SNO_NONE &&
        MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker, SCRIPT_SNO, snoScript))
    {
        tActorAdd.listSNOScripts.AddTail_Tracked(snoScript);
    }
    for (uint32 i = 0; i < 9; i++)
    {
        snoScript = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_SCRIPT_2 + i);
        if (snoScript != SNO_NONE &&
            MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker, SCRIPT_SNO, snoScript))
        {
            tActorAdd.listSNOScripts.AddTail_Tracked(snoScript);
        }
    }

    tActorAdd.snoConvList = snoConvList;
    tActorAdd.eTeam = TeamGetForMarker(tMarker);
    tActorAdd.tMarkerOwner = tMarkerOwner;
    tActorAdd.uPrimaryGroup = uPrimaryGroup;
    tActorAdd.uSecondaryGroup = uSecondaryGroup;

    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_AI_DISABLED)) dwAddFlags |= (1 << ACTORADD_DISABLE_BRAIN_BIT);
    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_AI_NO_DAMAGE)) dwAddFlags |= (1 << ACTORADD_NO_DAMAGE);
    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_AI_DOES_FAKE_DAMAGE)) dwAddFlags |= (1 << ACTORADD_DOES_FAKE_DAMAGE);
    dwAddFlags |= (1 << ACTORADD_KEEP_MARKER_NAME);
    if (TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_GIZMO_DOOR_OPEN)) dwAddFlags |= (1 << ACTORADD_GIZMO_DOOR_OPEN);


    SNO snoQuestRangeOpen = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_GIZMO_DOOR_OPEN_ON_QUESTRANGE);
    if (snoQuestRangeOpen != SNO_NONE && QuestRangeEvaluate(snoQuestRangeOpen, NULL))
    {
        dwAddFlags |= (1 << ACTORADD_GIZMO_DOOR_OPEN);
    }

    tActorAdd.dwActorAddFlags = dwAddFlags;

    ActorID idActor = ACTORID_INVALID;

    ActorType eActorType = ActorGetType(tMarker.snoname);
    if (eActorType == ACTORTYPE_ITEM)
    {
        GBHandle gbHandle = cghgbNone;
        FindGBHandleByActor(tMarker.snoname.snoHandle, gbHandle);
        if (gbHandle.gbid != GBID_NONE)
        {
            tActorAdd.hGB = gbHandle;
            idActor = GBAdd(tActorAdd);
            return ACDTryFindByActor(idActor);
        }
    }

    tActorAdd.tSNOName = tMarker.snoname;
    tActorAdd.snoActor = tMarker.snoname.snoHandle;

    switch (eActorType)
    {
    case ACTORTYPE_MONSTER: if (TagMapTagExists(tMarker.hTagMap, TAG_MARKER_MONSTER_RARITY))
        {
            tActorAdd.eMonsterRarity = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_MONSTER_RARITY);
        }

        idActor = GBAdd(tActorAdd);
        break;
    case ACTORTYPE_GIZMO:
    {
        ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);
        GizmoType eGizmoType = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
        if (eGizmoType == ACTORGIZMO_PLACED_LOOT)
        {
            return ACDID_INVALID;
        }


        idActor = GizmoAddBySNO(-1, tActorAdd);
    }
        break;
    case ACTORTYPE_SERVER_PROP:
        idActor = PropAddBySNO(tActorAdd);
        break;
    default:

        idActor = SceneryAdd(tActorAdd);
        break;
    }
    ACDID idACD = ACDTryFindByActor(idActor);

    if (idACD != ACDID_INVALID)
    {
        BOOL bStartsDisabled = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_START_DISABLED);
        if (bStartsDisabled)
        {
            ActorCommonData & tACD = ACDGet(idACD);
            tACD.SetEnabled(FALSE);
        }
    }

    return idACD;
}


























































































































DataID ActorMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner)
{






    uint32 dwAddFlags = 4;

    if (!ActorCanWalk(tMarker.snoname.snoHandle))
    {
        dwAddFlags |= 1;
    }

    RNG & tRNG = CGameGlobalsGet().rngLocal;
    float flScale = CreateNewActorScale(tMarker, tRNG);

    DataID idObject = (DataID)RActorCreate(tMarker.snoname.snoHandle, flScale, -1.0f, transform, dwAddFlags, RENDERCONTEXT_DEFAULT,
                                   idSScene);







    if (idObject != DATAID_INVALID)
    {
        RActor * ptRActor = RActorGet((RActorID)idObject);

        ptRActor->idSScene = idSScene;
        ptRActor->tMarkerOwner = tMarkerOwner;
        int32 nFadeGroup = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_FADE_GROUP);
        if (nFadeGroup == 0)
        {
            ptRActor->nFadeGroupSubObjectIndex = 0;
        }
        else
        {
            ptRActor->nFadeGroupSubObjectIndex = SceneGetASubObjectIndexFromFadeGroup(idSScene, nFadeGroup);
        }
        ptRActor->snoAmbientOcclusionOverrideTex = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_AO_OVERRIDE);
    }

    return idObject;
}


BOOL ActorMarker::ActorCanWalk(const SNO & snoActor)
{

    ScopedActorDefinitionLockR pdefActor(snoActor);
    BehaviorType eBehavior = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_BEHAVIOR_TYPE);

    return eBehavior == 2;









}



float ActorMarker::CreateNewActorScale(const Marker & tMarker, RNG & tRNG)
{


    float flScale = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_SCALE);
    if (tMarker.eType == MARKERTYPE_ACTOR)
    {
        ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
        if (flScale < EPSILON || (pdefActor->dwFlags & (1 << 0)))
        {

            float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
            float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);
            flScale = RandRangeFloat_Internal(tRNG, flMinScale, flDeltaScale);
        }
    }

    return flScale;
}
