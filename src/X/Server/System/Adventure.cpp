// Original file: X/Server/System/Adventure.cpp
// Complete. AdventureAddParentName (line 218) is external but only exists inlined into AdventureSpawn in
// the original image; the copy this file emits out of line has no original to compare with.
#include "./Adventure.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SScene.h"
#include "../SWorld.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/AdventureDefinition.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/Hyperlink.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Sigma/Random.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GameTemplate.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/Marker/MarkerType.h"
#include "../../Common/MarkerSet.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/World.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include <new>

BOOL SSceneIterateMarkers(const SSceneID idSScene, MarkerIterateCb pfuncIterate, void * ptUser, BOOL bIncludeSubscenes);  // X/Server/SScene.cpp:616
BOOL WorldIsDungeonFinderWorld(const SNO snoWorld);  // X/Common/World.cpp:886
void MarkersCreateInstancesFromMarkerSetSNO(SWorldID idSWorld, const PRTransform & transformSceneToWorld, SSceneID idSScene, SNO snoMarkerSet, const MarkerSetTranslation * ptTranslate, uint32 dwMarkersetCreateOptions, uint16 nAdventureParentIndex);  // X/Common/Marker/Marker.cpp
int32 SWorldGetRandSeedFromHash(SWorldID idSWorld, const int32 nHash);  // X/Server/SWorld.cpp:2069
void SceneClearSpawnArea(SceneCommonData & tSCD, int nSquareIndex, float flRadius, BOOL bUseEliteMask);  // X/Common/SceneCommon.cpp
BOOL AdventureSpawn(SNO snoAdventure, LPCSTR szAdventureName, const PRTransform & transformScene, SceneCommonData & tSCD, const PRTransform & transformSceneToWorld);

typedef blz::vector<CRefString, blz::allocator<CRefString> > XStringArray;  // X/Sigma/RefString.h:65

struct AdventureGlobals {  // size 0x18
    XStringArray arAdventureParentNames;  // 0x00
};

struct AdventureCbData {  // size 0x8
    SScene * ptSScene;  // 0x00
};

//@ 50
static AdventureGlobals & sAdventureGlobalsGet()
{
    SGame & tSGame = SGameManager::GameGet();


    return *tSGame.ptAdventureGlobals;
}

//@ 60
void AdventureInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    AdventureGlobals *& ptAdventureGlobals = tSGame.ptAdventureGlobals;



    ptAdventureGlobals = (AdventureGlobals *)SigmaMemoryNew(sizeof(AdventureGlobals), 4, tSGame.ptMemoryInterface, TRUE); new (ptAdventureGlobals) AdventureGlobals;
}

//@ 74
void AdventureDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    AdventureGlobals *& ptAdventureGlobals = tSGame.ptAdventureGlobals;


    ptAdventureGlobals->arAdventureParentNames.clear();
    ptAdventureGlobals->~AdventureGlobals();

    SigmaMemoryFree(ptAdventureGlobals, tSGame.ptMemoryInterface); ptAdventureGlobals = NULL;
}

//@ 88
static WorldDistance sAdventureGetNoSpawnRadius(const SNO snoAdventure)
{

    WorldDistance wdRadius;



    const AdventureDefinition * ptDefAdventure = (const AdventureDefinition *)gptGroupDefAdventure->LockRFast(snoAdventure);

    if (ptDefAdventure->wdNoSpawnRadius > EPSILON)
    {
        wdRadius = ptDefAdventure->wdNoSpawnRadius;
    }
    else if (ptDefAdventure->snoSymbolActor != SNO_NONE)
    {
        ScopedActorDefinitionLockR ptActorDef(ptDefAdventure->snoSymbolActor);
        float flScale = TagMapGetFloat(ptActorDef->hTagMap, TAG_ACTOR_SCALE_MIN);
        if (ptActorDef->wcylBounds.wdRadius > 0.0f)
        {
            wdRadius = flScale * ptActorDef->wcylBounds.wdRadius;
        }
        else
        {
            wdRadius = flScale * (ptActorDef->wsBounds.wdRadius - BOUNDING_SPHERE_FUDGE_FACTOR);
        }
    }
    else
    {
        CRefString sAdventure = HyperlinkCreateFromSNO(SNOName(ADVENTURE_SNO, snoAdventure), NULL);


        wdRadius = 5.0f;
    }

    return wdRadius;
}

//@ 128
static void sAdventurePaintScene(SWorld & tSWorld, SceneCommonData * ptSCD, const Vector3D & wpLocationScene, const SNO snoAdventure, const SNO snoMarkerSet)





{

    WorldPlace tPlaceAdventure;
    tPlaceAdventure.idSWorld = ptSCD->idSWorld;
    tPlaceAdventure.wp = ptSCD->transformWorld * wpLocationScene;

    NavMeshHandle tNavMeshHandleAdventure;
    ScenesCommonNavMeshNodeFromPlace(tPlaceAdventure, &tNavMeshHandleAdventure, FALSE, FALSE);

    if (!NavMeshHandleIsNone(tNavMeshHandleAdventure))
    {
        WorldDistance wdRange = sAdventureGetNoSpawnRadius(snoAdventure);

        SceneClearSpawnArea(*ptSCD, tNavMeshHandleAdventure.nSquareIndex, wdRange, FALSE);
    }












    ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
    for (int32 nCircle = 0; nCircle < pdefMarkerSet->tNoSpawnSet.GetArraySize(); nCircle++)
    {
        const Circle & tLocalCircle = pdefMarkerSet->tNoSpawnSet[nCircle];
        Circle tWorldCircle = tLocalCircle;
        Vector3D vPos(tWorldCircle.wpCenter.x, tWorldCircle.wpCenter.y, 0.0f);
        vPos = ptSCD->transformWorld * vPos;
        vPos += wpLocationScene;
        tWorldCircle.wpCenter = Vector2D(vPos.x, vPos.y);
        tSWorld.tNoSpawnTree.AddCircle(tWorldCircle);
    }
}

//@ 176
static Angle sAdventurePickRandomPivot(const AdventureDefinition * ptDefAdventure, Angle aInitial, RNG & tRng)



{
    Angle aPivot = aInitial;


    if (!(ptDefAdventure->aRotationMin == 0.0f && ptDefAdventure->aRotationMax == 0.0f) &&
        ptDefAdventure->aRotationMax >= ptDefAdventure->aRotationMin)
    {

        if (ptDefAdventure->aRotationStep > 0.0f)
        {
            float aDeltaRotation = ptDefAdventure->aRotationMax - ptDefAdventure->aRotationMin;

            int32 nStepCount = (int32)(aDeltaRotation / ptDefAdventure->aRotationStep);

            if (nStepCount < 1)
            {
                nStepCount = 1;
            }

            int32 nStep = RandRange_Internal(tRng, 0, nStepCount);

            aPivot += nStep * ptDefAdventure->aRotationStep;
        }
        else
        {

            aPivot += RandRangeFloat_Internal(tRng, ptDefAdventure->aRotationMin, ptDefAdventure->aRotationMax);
        }

        aPivot = Math::NormalizeAngle(aPivot);
    }

    return aPivot;
}

//@ 218
uint16 AdventureAddParentName(LPCSTR szParentName)
{

    AdventureGlobals & tAdventureGlobals = sAdventureGlobalsGet();
    tAdventureGlobals.arAdventureParentNames.push_back(CRefString(szParentName));

    uint32 nArraySize = tAdventureGlobals.arAdventureParentNames.size();

    return (uint16)(nArraySize - 1);
}

//@ 232
CRefString AdventureGetParentName(const uint16 nAdventureParentIndex)
{
    AdventureGlobals & tAdventureGlobals = sAdventureGlobalsGet();



    return tAdventureGlobals.arAdventureParentNames[nAdventureParentIndex];
}

//@ 243
static BOOL sActivateAdventure(const Marker * ptMarker, void * ptUser)


{
    if (ptMarker->eType != MARKERTYPE_ADVENTURE)
    {
        return TRUE;
    }

    AdventureCbData * ptCBData = (AdventureCbData *)ptUser;
    SceneCommonData * ptSCD = &ptCBData->ptSScene->tSCD;
    SNO snoAdventure = ptMarker->snoname.snoHandle;
    PRTransform transformScene = gtransformMarkerToScene * ptMarker->transform;










    if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_ENABLED_IN_DUNGEON_FINDER) &&
        !WorldIsDungeonFinderWorld(WorldCommonGetWorldSNO(ptSCD->idSWorld)))
    {
        return FALSE;
    }

    if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_DISABLED_IN_DUNGEON_FINDER) &&
        WorldIsDungeonFinderWorld(WorldCommonGetWorldSNO(ptSCD->idSWorld)))
    {
        return FALSE;
    }

    if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_DISABLED_IN_OPEN_WORLD) &&
        GameIsOpenWorld())
    {
        return FALSE;
    }

    if (TagMapGetInt(ptMarker->hTagMap, TAG_MARKER_DISABLED_IN_STORY_MODE) &&
        GameIsStoryMode())
    {
        return FALSE;
    }



    BOOL bSuccess = AdventureSpawn(snoAdventure, MarkerNameGetString(ptMarker->dwHash), transformScene, *ptSCD, ptSCD->transformWorld);

    return bSuccess;
}

//@ 297
BOOL AdventureSpawn(SNO snoAdventure, LPCSTR szAdventureName, const PRTransform & transformScene, SceneCommonData & tSCD, const PRTransform & transformSceneToWorld)





{
    SceneCommonData * ptSCD = &tSCD;
    if (tSCD.ptNavMeshInst == NULL)
    {
        SceneCommonData * ptSCDParent = SceneCommonDataTryToGet(tSCD.idSSceneParent);
        if (ptSCDParent != NULL)
        {
            ptSCD = ptSCDParent;
        }
    }







    ScopedAdventureDefinitionLockR pdefAdventure(snoAdventure);

    uint16 nAdventureParentIndex = AdventureAddParentName(szAdventureName);
    SNO snoMarkerSet = pdefAdventure->snoMarkerSet;
    if (snoMarkerSet == SNO_NONE)
    {
        return FALSE;
    }



    RNG tRNG;
    int32 nSeed = SWorldGetRandSeedFromHash(ptSCD->idSWorld, XHashKey<LPCSTR>(szAdventureName));
    RandSetSeed_Internal(tRNG, nSeed);







    MarkerSetTranslation tTranslate;
    tTranslate.wpAnchor = transformScene.wp;
    tTranslate.wpShift = cgwpNull;
    tTranslate.aPivot = sAdventurePickRandomPivot(pdefAdventure, transformScene.q.ToYaw(), tRNG);


    MarkersCreateInstancesFromMarkerSetSNO(
        ptSCD->idSWorld,
        transformSceneToWorld,
        tSCD.idSScene,
        snoMarkerSet, &tTranslate, (1 << MCO_FOR_ADVENTURE_BIT), nAdventureParentIndex);






    SWorld * ptSWorld = SWorldGet(ptSCD->idSWorld);
    sAdventurePaintScene(*ptSWorld, ptSCD, transformScene.wp, snoAdventure, snoMarkerSet);

    return TRUE;
}

//@ 364
void AdventureSceneInitialActivation(const SSceneID idSScene)
{
    SScene * ptSScene = SSceneGet(idSScene);

    AdventureCbData tUserData;
    tUserData.ptSScene = ptSScene;

    SSceneIterateMarkers(ptSScene->id, sActivateAdventure, &tUserData, FALSE);
}
