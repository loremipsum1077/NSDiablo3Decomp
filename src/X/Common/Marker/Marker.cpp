#include "Marker.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../Gizmo.h"
#include "../GizmoSwig.h"
#include "../Definitions/ActorDefinition.h"
#include "../Nav/NavigationMesh.h"
#include "../TagMaps/ActorTags.h"
#include "../TagMaps/Tags.h"
#include "../ThreadGlobals.h"
#include "SmartMarker.h"
#include "../InactiveMarker.h"
#include "../Definitions/MarkerSetDefinition.h"
#include "../Definitions/SceneDefinition.h"
#include "../Definitions/SceneGroupDefinition.h"
#include "../SceneCommon.h"
#include "../../Server/SGame.h"
#include "../../Sigma/Random.h"
#include "../World.h"
#include "../DRLGTypes.h"
#include "../Definitions/WorldDefinition.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Client/Gfx/GfxVB.h"  // was reached through SmartMarker.h:18 (the original SmartMarker.h does not include it)
// X/Common/SNO/SNOFiles.cpp:10239
BOOL SNOCheckSNOReference(SNOGroup snoGroupSrc, SNO snoHandleSrc, SNOGroup snoGroupDst, SNO snoHandleDst, LPCSTR szExtraMsg);
// X/Common/Nav/NavigationMesh.cpp:3201
PassabilityType NavMeshDeterminePassabilityType(const ActorDefinition * pdefActor);
BOOL MarkerKeepActivatedWithScene(const ActorDefinition * pdefActor);
struct MarkerSetTranslation;
// X/Common/SceneCommon.cpp
SWorldID SceneCommonGetSWorldID(SSceneID idSScene);
void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transformSceneToWorld);
// X/Server/SWorld.cpp:2086
int32 SWorldGetRandSeedFromSSceneID(SWorldID idSWorld, SSceneID idSScene);
void MarkersInstantiateForDistribution(SSceneID idSScene);
// X/Client/CMarker.cpp:298
DataID MarkerCreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);
void MarkersCreateInstancesFromMarkerSetSNO(SWorldID idSWorld, const PRTransform & transformSceneToWorld, SSceneID idSScene, SNO snoMarkerSet, const MarkerSetTranslation * ptTranslate, uint32 dwMarkersetCreateOptions, uint16 nAdventureParentIndex);
void MarkerFillPotentialSceneLabels(SNO snoScene, LabelListPooled & listPotentialLabels);
























































#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
extern const XTypeDescriptor * DT_SERIALIZEDATA;
// DWARF Marker.cpp:124-141; field names are empty in the executable.
static const TypeDescriptorField MarkerLink_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(MarkerLink, dwHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(MarkerLink, dwGroupHash), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MarkerLink), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MarkerLink("MarkerLink", 0, MarkerLink_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_MARKERLINK = &TypeDescriptor_MarkerLink;
static const TypeDescriptorField Marker_Fields[11] = {
    TypeDescriptorField("", DT_INT, offsetof(Marker, dwHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(Marker, dwHashPadding), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(Marker, eType), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(Marker, transform), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(Marker, snoname), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Marker, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(Marker, hTagMap), 0x20021, -8, 0x80000),
    TypeDescriptorField("", DT_INT, offsetof(Marker, nLinkCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Marker, serMarkerLinks), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MARKERLINK, offsetof(Marker, ptMarkerLinks), 0x21, -12),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Marker), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Marker("Marker", 13, Marker_Fields, 11, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_MARKER = &TypeDescriptor_Marker;
























LPCSTR LOCAL_MARKER_SCOPE_TOKEN = "<>:";





inline void sGenerateStringHash(CHAR * szOutput, uint32 dwHash)
{
    for (uint32 i = 0; i < 8; i++)
    {
        uint32 nDigit = (dwHash >> (i * 4)) & 0xf;
        CHAR cDigit = '0' + nDigit;
        if (nDigit >= 10)
            cDigit += 'a' - '0' - 10;
        szOutput[11 - i] = cDigit;
    }
}








const CHAR * MarkerNameGetString(uint32 dwHash)
{
    static CHAR szServerMarkerName[128] = "#[0xXXXXXXXX]";
    static CHAR szMainMarkerName[128] = "#[0x00000000]";
    static CHAR szUnsafeMarkerName[128] = "#[0x00000000]";

    if (ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_SERVERWORKER)
    {
        sGenerateStringHash(szServerMarkerName, dwHash);
        return szServerMarkerName;
    }
    else if (ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_MAIN)
    {
        sGenerateStringHash(szMainMarkerName, dwHash);
        return szMainMarkerName;
    }
    else
    {
        sGenerateStringHash(szUnsafeMarkerName, dwHash);
        return szUnsafeMarkerName;
    }
}






uint32 MarkerStringHash(LPCSTR pszName)
{
    uint32 nHash = 0;
    if (!pszName[0])
        return nHash;

    if (pszName[0] == '#' && (uint32)strlen(pszName) == 13 && pszName[1] == '[' && pszName[2] == '0' && pszName[3] == 'x' && pszName[12] == ']')
    {
        for (uint32 i = 4; i < 12; i++)
        {
            CHAR cCharValue = pszName[i];
            if (cCharValue >= 'a')
                cCharValue = cCharValue - 'a' + 10;
            else
                cCharValue = cCharValue - '0';
            nHash = nHash * 16 + cCharValue;
        }
        return nHash;
    }

    while (*pszName)
    {
        char cChar = *pszName;
        if (cChar >= 'A' && cChar <= 'Z')
            cChar += 'a' - 'A';
        nHash = nHash * 33 + cChar;
        if (cChar == ':')
            break;
        pszName++;
    }

    if (*pszName == ':')
        return nHash + MarkerStringHash(pszName + 1);
    return nHash;
}























































































































































































































































































void MarkersInitializeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    MarkerGlobals *& ptMarkerGlobals = tGCD.ptMarkerGlobals;

    ptMarkerGlobals = (MarkerGlobals *)SigmaMemoryNew(sizeof(MarkerGlobals), 4, ptMemoryInterface, TRUE);
    ptMarkerGlobals->nMarkerSetInstances = 0;
}
















void MarkersDisposeForGame()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    MarkerGlobals *& ptMarkerGlobals = tGCD.ptMarkerGlobals;

    if (ptMarkerGlobals->ptMarkerInstances)
        TChainedDataArrayDispose(ptMarkerGlobals->ptMarkerInstances);

    if (ptMarkerGlobals->ptMarkerInstancesBySet)
    {
        sFreeAllRelatedMarkerInstances(ptMarkerGlobals->ptMarkerInstancesBySet);
        delete ptMarkerGlobals->ptMarkerInstancesBySet;
    }

    SigmaMemoryFree(ptMarkerGlobals, ptMemoryInterface);
    ptMarkerGlobals = NULL;
}











void sFreeAllRelatedMarkerInstances(TMarkerSetToMarkerInstanceMap * ptMarkerInstancesBySet)
{
    POS pos = ptMarkerInstancesBySet->GetStartAssoc();
    while (pos)
    {
        TMarkerIntancePtrArray * pAllRelatedInstances;
        SNO sno;
        ptMarkerInstancesBySet->GetNextAssoc(pos, sno, pAllRelatedInstances);
        delete pAllRelatedInstances;
    }
    ptMarkerInstancesBySet->RemoveAll();
}








static MarkerInstance & sMarkerInstanceGet(GameCommonData & tGCD, const MarkerID idMarker)
{
    MarkerGlobals * ptMarkerGlobals = tGCD.ptMarkerGlobals;
    return *ptMarkerGlobals->ptMarkerInstances->ElementGet(idMarker);
}











BOOL MarkerCheckReference(SNO snoMarkerSet, int nMarkerIndex, const Marker & tMarker, SNOGroup snoGroup, SNO snoHandle)
{
    if (snoHandle == SNO_NONE)
        return FALSE;

    CRefString sExtraMsg;
    sExtraMsg.Format("Marker %d: %s", nMarkerIndex, MarkerNameGetString(tMarker.dwHash));
    return SNOCheckSNOReference(MARKERSET_SNO, snoMarkerSet, snoGroup, snoHandle, sExtraMsg);
}














const MarkerInstance & MarkerInstanceGet(GameCommonData & tGCD, const MarkerID idMarker)
{
    return sMarkerInstanceGet(tGCD, idMarker);
}

































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































BOOL MarkerKeepActivatedWithScene(SNO snoActor)
{
    ScopedActorDefinitionLockR pdefActor(snoActor);
    return MarkerKeepActivatedWithScene(pdefActor);
}





BOOL MarkerKeepActivatedWithScene(
    const ActorDefinition * pdefActor)
{
    if (GameIsStartUp())
        return TRUE;




    GizmoType nGizmoType = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
    BOOL bKeepActiveWithScene = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_ACTIVATE_WITH_SCENE);
    BOOL bKeepKnownWithScene = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_KNOWN_WITH_SCENE);
    PassabilityType ePassabilityType = NavMeshDeterminePassabilityType(pdefActor);
    ActorType eType = pdefActor->eType;

    if (bKeepKnownWithScene || bKeepActiveWithScene)
        return TRUE;








    if (ePassabilityType == PASSABILITY_NONMOVER || eType == ACTORTYPE_SERVER_PROP)
        return TRUE;




    if (GizmoIsNonMoving(nGizmoType))
        return TRUE;





    switch (nGizmoType) {
    case ACTORGIZMO_PORTAL:
    case ACTORGIZMO_WAYPOINT:
    case ACTORGIZMO_CHECKPOINT:
    case ACTORGIZMO_TOWN_PORTAL:
    case ACTORGIZMO_PORTAL_DESTINATION:
    case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
    case ACTORGIZMO_BOSS_PORTAL:
    case ACTORGIZMO_SAVEPOINT:
    case ACTORGIZMO_SAVEPOINT_PORTAL:
    case ACTORGIZMO_DUNGEON_PORTAL:
    case ACTORGIZMO_RETURN_PORTAL:
        return TRUE;
    default:
        return FALSE;
    }
}




DataID MarkerCreate(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const InactiveMarkerOwner & tMarkerOwner)
{
    DataID idObject = DATAID_INVALID;

    ScopedMarkerSetDefinitionLockR ptMarkerSet(tMarkerOwner.snoMarkerSet);
    if (tMarkerOwner.nMarkerIndex >= 0 && tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
    {
        const SmartMarker sMarker(ptMarkerSet, tMarkerOwner.nMarkerIndex);
        if (GameActiveGameCommonDataIsClient())
        {
            idObject = MarkerCreateForClientGame(idSWorld, idSScene, transform, MarkerGet(sMarker), tMarkerOwner);
        }
        else if (idSScene != SSCENEID_INVALID)
        {
            WorldPlace placePosition;
            placePosition.wp = transform.wp;
            placePosition.idSWorld = idSWorld;
            idObject = sMarker.CreateForServerGame(idSScene, placePosition, transform.q, tMarkerOwner);
        }
    }
    return idObject;
}






















































void MarkerIterateSnoReferences(const Marker & tMarker, SNOIterateFunc pfuncIterate, void * pData)
{
    if (tMarker.snoname.snoGroup != SNOGROUP_NONE)
        pfuncIterate(tMarker.snoname, 0, pData);

    if (tMarker.hTagMap)
        TagMapIterateSnoReferences(tMarker.hTagMap, pfuncIterate, pData);
}











void MarkerChangeSNOReference(Marker & tMarker, const SNOName & snonameOld, const SNO snoNew)
{
    if (tMarker.snoname.snoGroup != SNOGROUP_NONE && tMarker.snoname.snoHandle != SNO_NONE)
    {
        if (snonameOld == tMarker.snoname)
            tMarker.snoname.snoHandle = snoNew;
    }

    if (tMarker.hTagMap)
        TagMapChangeSNOReference(tMarker.hTagMap, snonameOld, snoNew);
}
#include "../OnlineService/OnlineService.h"
#include "../GameTemplate.h"
#include "../NodePath.h"
#include "../Definitions/QuestRangeDefinition.h"
#include "../../Server/Map/Minimap.h"
#include "../../Sigma/Math/Sphere.h"
#include "../TagMaps/MarkerTags.h"



static BOOL sMarkerHasPerPolyCollision(
    const Marker & tMarker)
{
    if (tMarker.eType != MARKERTYPE_ACTOR)
        return FALSE;
    ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
    return pdefActor->snoPhysMesh != SNO_NONE;
}


// DWARF WorldPlace.cpp:152, TeamCommon.cpp:370, NodePath.cpp:214/425.
BOOL WorldPlaceSnapToNavMesh(WorldPlace & tPlace, const BOOL fCanActivateScenes);
Team TeamGetForMarker(const Marker & tMarker);
PathID NodePathCreate(int32 nMaxPathNodeCount, PathClass ePathClass, SWorldID idSWorld);
BOOL NodePathAddNode(PathID idPath, uint32 tName, const Vector3D & wpPos, float flRadius, uint32 dwFlags, Team eTeam);



static void sPathMarkerAdd(
    const Marker & tMarker,
    const PRTransform & transform,
    const SWorldID idSWorld)
{


    PathClass ePathClass;
    float flRadius;
    uint32 dwFlags;
    Team eTeam;

    {
        ScopedActorDefinitionLockR ptActor(tMarker.snoname.snoHandle);
        float flScale = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_SCALE);

        ePathClass = TagMapGetInt(ptActor->hTagMap, TAG_ACTOR_PATH_ID);


        dwFlags = TagMapGetInt(ptActor->hTagMap, TAG_ACTOR_PATH_NODE_END_POINT) ? (1 << PATH_NODE_FLAG_PATH_END) : 0;


        flRadius = flScale;

        eTeam = TeamGetForMarker(tMarker);
    }

    PathID idPath = NodePathFind(ePathClass, idSWorld);

    if (idPath == -1)
    {
        const int32 MAX_PATH_NODES = 32;
        idPath = NodePathCreate(MAX_PATH_NODES, ePathClass, idSWorld);
    }

    if (idPath != -1)

        NodePathAddNode(idPath, tMarker.dwHash, transform.wp, flRadius, dwFlags, eTeam);
}



























































































static void sBuildMarkerWorldTransform(
    const MarkerSetTranslation * ptTranslate,
    const SWorldID idSWorld,
    const PRTransform & transformSceneToWorld,
    PRTransform & transform)
{



    if (ptTranslate)
    {

        transform.wp += ptTranslate->wpShift;


        transform.wp += ptTranslate->wpAnchor;


        if (ptTranslate->aPivot != 0.0f)
        {
            Vector3D wvAnchorToMarkerInstance = transform.wp - ptTranslate->wpAnchor;
            Vector3D::Rotate(cgwvZAxis, ptTranslate->aPivot, wvAnchorToMarkerInstance);


            transform.wp = wvAnchorToMarkerInstance + ptTranslate->wpAnchor;


            Angle aYaw = transform.q.ToYaw() + ptTranslate->aPivot;
            transform.q.FromYaw(aYaw);
        }
    }



    transform = transformSceneToWorld * transform;



    if (ptTranslate)
    {


        WorldPlace tPlace;
        tPlace.idSWorld = idSWorld;
        tPlace.wp = transform.wp;

        WorldPlaceSnapToNavMesh(tPlace, TRUE);


        transform.wp = tPlace.wp;
    }
}





















































static void sCreateInstancesFromMarkerSetSNO(
    const SWorldID idSWorld,
    const PRTransform & transformSceneToWorld,
    const SSceneID idSScene,
    const SNO snoMarkerSet,
    const MarkerSetTranslation * ptTranslate,
    const uint32 dwMarkerSetCreateOptions,
    const uint16 nAdventureParentIndex,
    const uint32 nMarkerSetInstanceIndex)
{
    if (snoMarkerSet == SNO_NONE)
        return;





    InactiveMarkerList * ptMarkerScene = NULL;

    SceneCommonData * ptSCD = NULL;
    if (idSScene != -1)
    {
        ptSCD = &SceneCommonDataGet(idSScene);
        ptMarkerScene = ptSCD->ptMarkerScene;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    GameMode nGameMode = tGCD.GameModeGet();
    // bForEdit (BOOL, DWARF line 2222) has no location or value in the original; its initialiser is unknown.

    BOOL bRunningClient = GameActiveGameCommonDataIsClient();










    BOOL bIsPTR = OnlineService::IsPTR();

    ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
    int32 nCountMarkers = pdefMarkerSet->tMarkerSet.GetArraySize();
    for (int32 nMarkerIndex = 0; nMarkerIndex < nCountMarkers; ++nMarkerIndex)
    {
        SmartMarker sMarker(pdefMarkerSet, nMarkerIndex);

        if (nGameMode == GAMEMODE_PLAYING && sMarker.IsForClient() != bRunningClient)
            continue;



        if (!bIsPTR && sMarker.IsForPTR())
            continue;








        if (GameIsOpenWorld() &&
            TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_DISABLED_IN_OPEN_WORLD))
            continue;



        if (GameIsStoryMode() &&
            TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_DISABLED_IN_STORY_MODE))
            continue;




        if (TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_ENABLED_IN_DUNGEON_FINDER) &&
            !WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)) &&
            GameGetActiveGameCommonData().GameModeGet() == GAMEMODE_PLAYING)
            continue;



        if (TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_DISABLED_IN_DUNGEON_FINDER) &&
            WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)))
            continue;




        SNOName snoname(sMarker.GetSNOGroup(), sMarker.GetSNOHandle());

        if (snoname.snoGroup != SNOGROUP_NONE &&
            (snoname.snoHandle == SNO_NONE || !SNOFileExists(snoname)) && nGameMode == GAMEMODE_PLAYING)
            continue;














        if (sMarker.GetMarkerType() == MARKERTYPE_ENCOUNTER)
        {
            if (dwMarkerSetCreateOptions & (1 << MCO_NO_ENCOUNTERS_BIT))
                continue;
        }



        else
        {
            if (dwMarkerSetCreateOptions & (1 << MCO_ENCOUNTERS_ONLY_BIT))
                continue;
        }



        PRTransform transform = gtransformMarkerToScene * sMarker.GetTransform();



























        sBuildMarkerWorldTransform(ptTranslate, idSWorld, transformSceneToWorld, transform);








































        Sphere wsBounds;
        sMarker.GetMarkerBounds(transform, wsBounds);




        BOOL bActivateNow = FALSE;
        if (!TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_START_INACTIVE))
        {
            if (sMarker.GetMarkerType() == MARKERTYPE_ENCOUNTER || sMarker.GetMarkerType() == MARKERTYPE_EVENT)
                bActivateNow = TRUE;
            else
            {



                if (sMarker.GetMarkerType() == MARKERTYPE_ACTOR)

                    bActivateNow = MarkerKeepActivatedWithScene(sMarker.GetSNOHandle());







                if (TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_KNOWN_WITH_SCENE) || (dwMarkerSetCreateOptions & (1 << MCO_INSTANCE_IMMEDIATELY_BIT)))
                    bActivateNow = TRUE;



                SNO snoQuestRange = TagMapGetSNO(sMarker.GetTagMap(), TAG_MARKER_QUESTRANGE);
                if (snoQuestRange != SNO_NONE && !QuestRangeEvaluate(snoQuestRange, NULL))
                    bActivateNow = FALSE;
            }
        }





        if (ptSCD != NULL && (dwMarkerSetCreateOptions & (1 << MCO_FOR_ADVENTURE_BIT)) &&

            !sMarker.IsForClient())
        {
            const Marker & tMarker = MarkerGet(sMarker);
            if (TagMapTagExists(tMarker.hTagMap, TAG_MARKER_MINIMAP_ICON))

                Minimap::AddMarker(tMarker, ptSCD, &transform);

            else if (tMarker.snoname.snoGroup == ACTOR_SNO)
            {
                GizmoType eGizmoType = ActorGetGizmoType(tMarker.snoname.snoHandle);
                if (eGizmoType == ACTORGIZMO_PORTAL ||
                    eGizmoType == ACTORGIZMO_PAGE_OF_FATE_PORTAL ||
                    eGizmoType == ACTORGIZMO_SECRET_PORTAL ||
                    eGizmoType == ACTORGIZMO_BOSS_PORTAL ||
                    eGizmoType == ACTORGIZMO_DUNGEON_PORTAL ||
                    eGizmoType == ACTORGIZMO_RETURN_PORTAL)


                    Minimap::AddPortalMarker(snoMarkerSet, tMarker, ptSCD, &transform);
            }
        }






        InactiveMarkerOwner tMarkerOwner;
        tMarkerOwner.snoMarkerSet = snoMarkerSet;
        tMarkerOwner.nMarkerIndex = nMarkerIndex;
        tMarkerOwner.nAdventureParentIndex = nAdventureParentIndex;
        tMarkerOwner.nMarkerSetInstanceIndex = nMarkerSetInstanceIndex;


        if (sMarker.IsPathMarker())
        {
            if (!bRunningClient)

                sPathMarkerAdd(MarkerGet(sMarker), transform, idSWorld);
        }

        else if (idSScene == -1 || bActivateNow)

            MarkerCreate(idSWorld, idSScene, transform, tMarkerOwner);

        else

            InactiveMarkerCreateFromMarkerSet(ptMarkerScene, tMarkerOwner, transform, wsBounds);
    }

}




void MarkersCreateInstancesFromMarkerSetSNO(
    SWorldID idSWorld,
    const PRTransform & transformSceneToWorld,
    SSceneID idSScene,
    SNO snoMarkerSet,
    const MarkerSetTranslation * ptTranslate,
    uint32 dwMarkersetCreateOptions,
    uint16 nAdventureParentIndex)
{
    if (GameActiveGameCommonDataIsServer())
    {

        GameCommonData & tGCD = GameGetActiveGameCommonData();
        MarkerGlobals *& ptMarkerGlobals = tGCD.ptMarkerGlobals;
        ++ptMarkerGlobals->nMarkerSetInstances;


        uint32 dwFlags = dwMarkersetCreateOptions |
            (1 << MCO_NO_ENCOUNTERS_BIT) | (1 << MCO_INSTANCE_IMMEDIATELY_BIT);

        sCreateInstancesFromMarkerSetSNO(
            idSWorld,
            transformSceneToWorld,
            idSScene,
            snoMarkerSet,
            ptTranslate,
            dwFlags,
            nAdventureParentIndex,
            ptMarkerGlobals->nMarkerSetInstances);



        dwFlags = dwMarkersetCreateOptions | (1 << MCO_ENCOUNTERS_ONLY_BIT) | (1 << MCO_INSTANCE_IMMEDIATELY_BIT);

        sCreateInstancesFromMarkerSetSNO(
            idSWorld,
            transformSceneToWorld,
            idSScene,
            snoMarkerSet,
            ptTranslate,
            dwFlags,
            nAdventureParentIndex,
            ptMarkerGlobals->nMarkerSetInstances);
    }
    else

        sCreateInstancesFromMarkerSetSNO(idSWorld, transformSceneToWorld, idSScene, snoMarkerSet, ptTranslate, dwMarkersetCreateOptions, nAdventureParentIndex, 0);
}





// DWARF SmartMarker.h:81/92/93/96: original inlined bodies, scoped here without an inline hint.
inline void SmartMarker::GetMarkerBounds(PRTransform & transform, Sphere & wsBounds) const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    m_pInterface->GetMarkerBounds(*m_pMarker, transform, wsBounds);
}
inline BOOL SmartMarker::IsForClient() const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return m_pInterface->IsForClient(*m_pMarker);
}
inline BOOL SmartMarker::IsForPTR() const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return m_pInterface->IsForPTR(*m_pMarker);
}
inline BOOL SmartMarker::IsPathMarker() const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return m_pInterface->IsPathMarker(*m_pMarker);
}


































static void sMarkersCreateInstancesFromSceneMarkerSets(SSceneID idSScene)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    SWorldID idSWorld = SceneCommonGetSWorldID(idSScene);
    PRTransform transformSceneToWorld;
    SceneGetSceneToWorldTransform(idSScene, transformSceneToWorld);

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    GameMode eGameMode = tGCD.GameModeGet();

    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
    int32 nCount = pdefScene->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nCount; i++)
    {
        SNO snoMarkerSet = pdefScene->arMarkerSets[i];
        GBID gbidLabel;
        MarkerSetTranslation * ptTranslate = NULL;

        ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
        if (eGameMode == GAMEMODE_PLAYING)
        {
            gbidLabel = pdefMarkerSet->nLabel;
            if (gbidLabel != GBID_NONE && !tSCD.listAppliedLabels.Find(gbidLabel))
                continue;
        }

        MarkersCreateInstancesFromMarkerSetSNO(idSWorld, transformSceneToWorld, idSScene, snoMarkerSet, ptTranslate, 0, 0xffff);
    }
}































void MarkersCreateInstancesFromScene(SSceneID idSScene)
{
    int32 nOldSeed;
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    if (!GameActiveGameCommonDataIsClient())
    {
        nOldSeed = RandGetSeed(SGameGlobalsGet().rng);
        int32 nSeed = SWorldGetRandSeedFromSSceneID(tSCD.idSWorld, idSScene);
        RandSetSeed_Internal(SGameGlobalsGet().rng, nSeed);
    }
    else
    {
        nOldSeed = 0;
    }

    sMarkersCreateInstancesFromSceneMarkerSets(idSScene);
    MarkersInstantiateForDistribution(idSScene);

    if (!GameActiveGameCommonDataIsClient())
        RandSetSeed_Internal(SGameGlobalsGet().rng, nOldSeed);
}













void MarkersCreateInstancesFromWorld(SWorldID idSWorld, SSceneID idSScene)
{
    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
    IVector2D pointWorldOrigin = WorldGetCommonData(idSWorld).pointWorldOrigin;
    Vector3D wpSceneChunk((float)-pointWorldOrigin.x, (float)-pointWorldOrigin.y, 0.0f);
    PRTransform transformSceneToWorld(cgqIdentity, wpSceneChunk);

    ScopedWorldDefinitionLockR pdefWorld(snoWorld);
    int32 nWorldCount = pdefWorld->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nWorldCount; i++)
    {
        SNO snoMarkerSet = pdefWorld->arMarkerSets[i];
        MarkersCreateInstancesFromMarkerSetSNO(idSWorld, transformSceneToWorld, idSScene, snoMarkerSet, NULL, 0, 0xffff);
    }
}






























































// Identity spelled as in Transform.cpp:26; DWARF shows only copy-ctors inlined at 2763/2764 and the original folds the values.
PRTransform gtransformMarkerToScene(Quaternion(Vector3D(0.0f, 0.0f, 0.0f), 1.0f), Vector3D(0.0f, 0.0f, 0.0f));
PRTransform gtransformSceneToMarker(Quaternion(Vector3D(0.0f, 0.0f, 0.0f), 1.0f), Vector3D(0.0f, 0.0f, 0.0f));

void SetMarkerToSceneTransform(const PRTransform & transform)
{
    gtransformMarkerToScene = transform;
    gtransformSceneToMarker = gtransformMarkerToScene.Inverse();
}




void MarkerFillPotentialSceneLabels(DRLGPlacedScene & tPlacedScene)
{
    if (tPlacedScene.snoname.snoGroup == SCENE_SNO)
        MarkerFillPotentialSceneLabels(tPlacedScene.snoname.snoHandle, tPlacedScene.listPotentialLabels);
}











void MarkerFillPotentialSceneLabels(SNO snoScene, LabelListPooled & listPotentialLabels)
{
    ScopedSceneDefinitionLockR pdefScene(snoScene);

    int32 nCount = pdefScene->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nCount; i++)
    {
        SNO snoMarkerSet = pdefScene->arMarkerSets[i];
        if (snoMarkerSet == SNO_NONE)
            continue;

        ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
        if (!pdefMarkerSet)
            continue;

        if (!listPotentialLabels.Find(pdefMarkerSet->nLabel))
            listPotentialLabels.AddTail_Tracked(pdefMarkerSet->nLabel);

        for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; j++)
        {
            int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
            if (tMarker.eType == MARKERTYPE_SCENEGROUP)
            {
                ScopedSceneGroupDefinitionLockR ptSceneGroupDef(tMarker.snoname.snoHandle);
                for (int32 nScene = 0; nScene < ptSceneGroupDef->nSceneCnt; nScene++)
                {
                    SceneGroupItem & tSceneItem = ptSceneGroupDef->arScenes[nScene];
                    if (tSceneItem.gbidLabel != GBID_NONE && !listPotentialLabels.Find(tSceneItem.gbidLabel))
                        listPotentialLabels.AddTail_Tracked(tSceneItem.gbidLabel);
                }
            }
        }
    }
}

















#include "../PortalCommon.h"
#include "../ActorCommon.h"
#include "../GlobalSNO.h"
#include "../Definitions/BossEncounterDefinition.h"
#include "../../Server/SWorld.h"
#include "../../Server/SGameManager.h"
// DWARF World.cpp:605, Act.cpp:260, GlobalDefinition.cpp:1511.
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);
void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest);
BOOL StartingLocationGUIDIsValid(const uint32 uGUID);




PortalResolveMethod MarkerResolvePortalDestination(
    const Marker & tMarker,
    const SceneCommonData & tSCD,
    ResolvedPortalDestination & tResolvedDest)
{
    GizmoType eType = ActorGetGizmoType(tMarker.snoname.snoHandle);
    if (!GizmoIsPortal(eType))
    {
        tResolvedDest.snoWorld = SNO_NONE;
        return PRM_INVALID;
    }

    if (eType == ACTORGIZMO_BOSS_PORTAL)
    {
        SNO snoBossEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, tMarker.snoname.snoHandle);
        if (snoBossEncounter != SNO_NONE)
        {
            ScopedBossEncounterDefinitionLockR pdefBoss(snoBossEncounter);
            tResolvedDest.snoWorld = pdefBoss->arrWorlds[0];
            tResolvedDest.uEntranceGUID = pdefBoss->uEntranceGUID;
            tResolvedDest.snoDestLevelArea = ActorGetInheritedTagMapValueSNO(tMarker.snoname.snoHandle, tMarker, TAG_ACTOR_GIZMO_PORTAL_DEST_LEVEL_AREA, TAG_MARKER_PORTAL_DEST_LEVEL_AREA);
            return PRM_BOSS_ENCOUNTER;
        }
    }
    if (eType == ACTORGIZMO_PAGE_OF_FATE_PORTAL)
    {
        DisplayInternalError(NULL, "", 2883, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);  // original fatal assertion; expression and file strings stripped
    }


    if (eType == ACTORGIZMO_DUNGEON_PORTAL &&
        WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(tSCD.idSWorld)))
    {

        SWorldID idSWorldFirstFloor = WorldCommonFindBySNO(GlobalSNOGet(GLOBALSNO_WORLD_DUNGEON_FINDER_LEVEL_01), TRUE);
        SWorld * ptSWorldFirstFloor = SWorldTryToGet(idSWorldFirstFloor);
        if (ptSWorldFirstFloor)
        {
            tResolvedDest = ptSWorldFirstFloor->tDynamicReturnLocation;
            return PRM_DYNAMIC_RETURN_LOCATION;
        }
    }

    PortalResolveMethod eMethod = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_PORTAL_RESOLVE_METHOD);

    if (eMethod == PRM_PREV_WORLD &&
        WorldGetDungeonFinderLevel(WorldCommonGetWorldSNO(tSCD.idSWorld)) == 1)

        eMethod = PRM_DYNAMIC_RETURN_LOCATION;



    switch (eMethod)
    {
    case PRM_SPECIFIC_WORLD:
        tResolvedDest.snoWorld = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_PORTAL_SPECIFIED_WORLD);
        tResolvedDest.snoDestLevelArea = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_PORTAL_DEST_LEVEL_AREA);
        tResolvedDest.uEntranceGUID = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_PORTAL_ENTRANCE_NUMBER);
        break;

    case PRM_CURRENT_WORLD:
        tResolvedDest.snoWorld = WorldCommonGetWorldSNO(tSCD.idSWorld);
        tResolvedDest.snoDestLevelArea = tSCD.tSceneSpec.snoNextLevelArea != SNO_NONE ? tSCD.tSceneSpec.snoNextLevelArea : TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_PORTAL_DEST_LEVEL_AREA);
        tResolvedDest.uEntranceGUID = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_PORTAL_ENTRANCE_NUMBER);
        break;

    case PRM_PREV_WORLD:
        tResolvedDest.snoWorld = tSCD.tSceneSpec.snoPrevWorld;
        tResolvedDest.uEntranceGUID = tSCD.tSceneSpec.uPrevEntranceGUID;
        tResolvedDest.snoDestLevelArea = tSCD.tSceneSpec.snoPrevLevelArea;
        break;

    case PRM_NEXT_WORLD:
        tResolvedDest.snoWorld = tSCD.tSceneSpec.snoNextWorld;
        tResolvedDest.uEntranceGUID = tSCD.tSceneSpec.uNextEntranceGUID;
        tResolvedDest.snoDestLevelArea = tSCD.tSceneSpec.snoNextLevelArea;
        break;

    case PRM_SAVE_POINT_PORTAL:
    {
        ActGetCurrentStartLocation(tResolvedDest);


        uint32 uEntranceGUID = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_PORTAL_ENTRANCE_NUMBER);
        if (uEntranceGUID != (uint32)-1)
        {
            tResolvedDest.uEntranceGUID = uEntranceGUID;
        }
        break;
    }










    case PRM_DYNAMIC_RETURN_LOCATION:

        if (GameActiveGameCommonDataIsServer())
        {
            SWorld * ptSWorld = SWorldTryToGet(tSCD.idSWorld);
            if (ptSWorld && ptSWorld->tDynamicReturnLocation.IsValid())
            {
                tResolvedDest.snoWorld = ptSWorld->tDynamicReturnLocation.snoWorld;
                tResolvedDest.snoDestLevelArea = ptSWorld->tDynamicReturnLocation.snoDestLevelArea;
                tResolvedDest.uEntranceGUID = ptSWorld->tDynamicReturnLocation.uEntranceGUID;
            }
        }
        break;
    }







    if (eType == ACTORGIZMO_PORTAL)
    {

        SNO snoChallenge = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_CHALLENGE_SNO, tMarker.snoname.snoHandle);
        if (snoChallenge != SNO_NONE)
            return eMethod;
    }








    StartingLocationGUIDIsValid(tResolvedDest.uEntranceGUID);









    return eMethod;
}




static void sAddEnvAppsFromMarkerSet(
    SceneCommonData & tSCD,
    const MarkerSetDefinition & tdefMarkerSet,
    const PRTransform & transformSceneToWorld)
{
    int32 nCountMarkers = tdefMarkerSet.tMarkerSet.GetArraySize();
    for (int32 nMarkerIndex = 0; nMarkerIndex < nCountMarkers; ++nMarkerIndex)
    {
        const Marker & tMarker = tdefMarkerSet.tMarkerSet[nMarkerIndex];
        if (!sMarkerHasPerPolyCollision(tMarker))
            continue;

        ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
        if (pdefActor->eType == ACTORTYPE_ENVIRONMENT)
        {
            EnvAppData tAppData;
            tAppData.snoAppearance = pdefActor->snoAppearance;
            tAppData.snoPhysMesh = pdefActor->snoPhysMesh;
            tAppData.transform = transformSceneToWorld * tMarker.transform;
            tAppData.flScale = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_SCALE);
            tAppData.bodyId = dm_nullBodyId;
            tAppData.idSScene = tSCD.idSScene;
            tSCD.listEnvApps.AddTail_Tracked(tAppData);
        }
    }
}





void MarkersCreateEnvAppList(
    SceneCommonData & tSCD)
{


    PRTransform transformSceneToWorld;
    SceneGetSceneToWorldTransform(tSCD.idSScene, transformSceneToWorld);

    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
    int32 nMarkerSetCount = pdefScene->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nMarkerSetCount; ++i)
    {
        SNO snoMarkerSet = pdefScene->arMarkerSets[i];
        ScopedMarkerSetDefinitionLockR ptMarkerSetDef(snoMarkerSet);
        GBID gbidLabel = ptMarkerSetDef->nLabel;
        if (gbidLabel == -1 || tSCD.listAppliedLabels.Find(gbidLabel))
            sAddEnvAppsFromMarkerSet(tSCD, *ptMarkerSetDef, transformSceneToWorld);
    }



}
