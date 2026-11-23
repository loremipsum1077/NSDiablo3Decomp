// X/Server/Map/Minimap.cpp -- the server's minimap markers and pings, kept per SGame in a
// MiniMapMarkerGlobals block. Every function sits on its original line.

#include "Minimap.h"

#include "../../Client/Gfx/GfxCommon.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/ActDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/Definitions/QuestDefinition.h"
#include "../../Common/GameSwig.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/ImageHandle.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Common/ActorCommon.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Player.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/TagMaps/Tags.h"
#include "../Actors/Actors.h"
#include "../PlayerList.h"
#include "../SGameManager.h"
#include "../SScene.h"
#include "../SWorld.h"
#include "../System/LevelArea.h"
#include "../Timers.h"

// Declared locally until their headers are adopted; the comment names the defining file.
typedef uint32 StringLabelHandle;  // X/Common/Labels.h
BOOL QuestIsAnyBountyType(SNO snoQuest);  // X/Common/Quests/Quests.cpp:3043
void SQuestsRegisterBountyLevelArea(SNO snoWorld, SNO snoLevelArea, SNO snoQuest);  // X/Server/Quests/SQuests.cpp:5791
SNO SQuestGetBountyQuestSNOWithLevelAreaObjective(SNO snoLevelArea);  // X/Server/Quests/SQuests.cpp:5639
void SQuestsRemoveBountyLevelArea(SNO snoLevelArea, SNO snoQuest);  // X/Server/Quests/SQuests.cpp:5822
void STrickleUpdateActor(const ActorCommonData & tACD);  // X/Server/Network/STrickle.cpp:305
void RegisterQuestActor(ACDID idActor, BOOL bEnable);  // X/Server/LuaScript/ScriptUtil.cpp:10705
CRefString MonsterGetSNOName(const SNOName & tActorSNOName);  // X/Common/Monsters.cpp:85
void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transformSceneToWorld);  // X/Common/ScenesCommon.cpp:1620
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive);  // X/Common/ScenesCommon.cpp:1240
SNO NavMeshHandleGetLevelArea(const SceneCommonData * ptSCD, const NavMeshHandle & tNavMeshHandle);  // X/Common/Nav/NavigationMesh.cpp:1773
int32 NavMeshHandleGetLevelAreaIndex(const NavMeshHandle & tNavMeshHandle);  // X/Common/Nav/NavigationMesh.cpp:1760
int32 ActGetWaypointByLevelArea(const SNO snoLevelArea, BOOL bCheckActivationState);  // X/Common/Act.cpp:156
StringLabelHandle StringListGetLabelHandle(LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp:1580
struct QuestRangeResult;  // X/Common/Definitions/QuestRangeDefinition.h
BOOL QuestRangeEvaluate(const SNO snoQuestRange, QuestRangeResult * pResult);  // X/Common/Definitions/QuestRangeDefinition.cpp:280
namespace Minimap { void ResolveIconForActor(const ActorCommonData & tACD, const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner, const int32 nPlayerIndex, SNO & snoRet, CRefString & szRet); }  // X/Common/Map/MinimapCommon.cpp:28

static const float BOUNTY_LEVEL_AREA_AUTOPING_DISTANCE = 200.0f;
static const float BOUNTY_ACTOR_AUTOPING_DISTANCE = 200.0f;
static const float BOUNTY_CHALLENGE_AUTOPING_DISTANCE = 200.0f;
static const float BOUNTY_EVENT_AUTOPING_DISTANCE = 200.0f;
static const float BOUNTY_BOSS_PORTAL_AUTOPING_DISTANCE = 4294967296.0f;

namespace Minimap {

namespace {

static const uint32 sch_MinimapPortalDefaultLabel = 0x526a3318;
struct MinimapMarkerInfo {  // size 0x68
    WorldPlace m_Place;  // 0x00
    ImageTextureFrame m_ImageInfo;  // 0x10
    ImageHandle m_hImage;  // 0x18
    StringLabelHandle m_hLabel;  // 0x1c
    SNO m_snoStringList;  // 0x20
    SNO m_snoQuestRange;  // 0x24
    SNO m_snoDestLevelArea;  // 0x28
    SNO m_snoKnownActorOverride;  // 0x2c
    uint32 m_nHashedName;  // 0x30
    float m_fMaxDisplayRangeSq;  // 0x34
    float m_fMinDisplayRangeSq;  // 0x38
    float m_fDiscoveryRangeSq;  // 0x3c
    int32 m_nWaypointNumber;  // 0x40
    SNO m_snoQuestSource;  // 0x44
    uint32 m_bActive : 1;  // 0x48
    uint32 m_bCanBecomeArrow : 1;  // 0x48
    uint32 m_bRespectsFoW : 1;  // 0x48
    uint32 m_bIsWaypoint : 1;  // 0x48
    uint32 m_bIsPlaceholder : 1;  // 0x48
    uint32 m_bHasBeenDiscovered : 1;  // 0x48
    uint32 m_bIsPing : 1;  // 0x48
    uint32 m_bDeactivatedByScript : 1;  // 0x48
    uint32 m_nPlayerUseFlags : 4;  // 0x48
    uint32 m_nPad : 20;  // 0x48
    OnlineService::HeroId m_idAssignedHero;  // 0x50
    SNOName m_snoNameData;  // 0x58
    BOOL m_bIsSNOLocationReference;  // 0x60
    SSceneID m_idSScene;  // 0x64





    MinimapMarkerInfo()
    {
        SigmaMemoryClear(this, sizeof(*this));
        m_ImageInfo.snoTexture = SNO_NONE;
        m_snoQuestRange = SNO_NONE;


        m_hLabel = (StringLabelHandle)-1;
        m_snoStringList = SNO_NONE;

        m_snoKnownActorOverride = SNO_NONE;

        m_nWaypointNumber = -1;
        m_snoQuestSource = SNO_NONE;
        m_bIsPing = FALSE;
        m_bHasBeenDiscovered = FALSE;
        m_bIsPlaceholder = FALSE;
        m_idAssignedHero = OnlineService::INVALID_HERO_ID;
        m_snoNameData = SNOName(SNOGROUP_NONE, SNO_NONE);
        m_bIsSNOLocationReference = FALSE;
        m_idSScene = SSCENEID_INVALID;
    }
};
static_assert(sizeof(MinimapMarkerInfo) == 0x68, "MinimapMarkerInfo");

typedef XGrowableMap<uint32, uint32, MinimapMarkerInfo, MinimapMarkerInfo &> MarkerList;

}  // namespace

struct SNOAutoPingData {  // size 0x8
    SNO sno;  // 0x00
    PingID idPing;  // 0x04
};

typedef XGrowableMap<int, int, int, int> SNOAutoPingRefMap;
typedef XGrowableList<SNOAutoPingData, SNOAutoPingData> SNOAutoPingDataList;

static PingID sGetNewPingID();



struct MiniMapMarkerGlobals {  // size 0x28
    MarkerList * ptList;  // 0x00
    MarkerList * ptPingList;  // 0x08
    uint32 nGuid;  // 0x10
    uint32 idPingNext;  // 0x14
    SNOAutoPingRefMap * ptSNOAutoPingRefs;  // 0x18
    SNOAutoPingDataList * ptSNOAutoPingData;  // 0x20
    MiniMapMarkerGlobals();
    ~MiniMapMarkerGlobals();
};

inline MiniMapMarkerGlobals::MiniMapMarkerGlobals()
{
    ptList = new MarkerList(16, NULL);
    ptPingList = new MarkerList(16, NULL);
    nGuid = 0;
    idPingNext = 0;

    ptSNOAutoPingRefs = new SNOAutoPingRefMap(16, NULL);
    ptSNOAutoPingData = new SNOAutoPingDataList;
}

MiniMapMarkerGlobals::~MiniMapMarkerGlobals()
{
    delete ptPingList; ptPingList = NULL;
    delete ptList; ptList = NULL;
    delete ptSNOAutoPingRefs; ptSNOAutoPingRefs = NULL;
    delete ptSNOAutoPingData; ptSNOAutoPingData = NULL;
}






void CreateMarkerListForGame()
{
    MiniMapMarkerGlobals *& pGameList = SGameManager::GameGet().ptSMiniMapGlobals;
    pGameList = new MiniMapMarkerGlobals;
}



void DisposeMarkerListForGame()
{
    MiniMapMarkerGlobals *& pGameList = SGameManager::GameGet().ptSMiniMapGlobals;

    if (pGameList)
    {
        delete pGameList; pGameList = NULL;
    }
}

namespace {





MarkerList * GetActiveGameList()
{
    return SGameManager::GameGet().ptSMiniMapGlobals->ptList;
}



MarkerList * GetPingList()
{
    return SGameManager::GameGet().ptSMiniMapGlobals->ptPingList;
}



void MarkerToMessage(const MinimapMarkerInfo & tInfo, MapMarkerInfoMessage & tMsg)
{


    tMsg.m_bActive = tInfo.m_bActive;
    tMsg.m_Place = tInfo.m_Place;
    tMsg.m_ImageInfo = tInfo.m_ImageInfo.snoTexture;
    tMsg.m_nHashedName = tInfo.m_nHashedName;
    tMsg.m_bCanBecomeArrow = tInfo.m_bCanBecomeArrow;
    tMsg.m_bIsPing = tInfo.m_bIsPing;
    tMsg.m_fMaxDisplayRangeSq = tInfo.m_fMaxDisplayRangeSq;
    tMsg.m_fMinDisplayRangeSq = tInfo.m_fMinDisplayRangeSq;
    tMsg.m_fDiscoveryRangeSq = tInfo.m_fDiscoveryRangeSq;
    tMsg.m_bRespectsFoW = tInfo.m_bRespectsFoW;
    tMsg.m_hImage = tInfo.m_hImage;
    tMsg.m_nPlayerUseFlags = tInfo.m_nPlayerUseFlags;

    tMsg.m_hLabel = tInfo.m_hLabel;
    tMsg.m_snoStringList = tInfo.m_snoStringList;
    tMsg.m_snoKnownActorOverride = tInfo.m_snoKnownActorOverride;
    tMsg.m_snoQuestSource = tInfo.m_snoQuestSource;
}


void SendMarkerInfoToClients(MinimapMarkerInfo & tInfo)
{


    if (tInfo.m_bIsPlaceholder)
    {
        return;
    }


    if (tInfo.m_bIsSNOLocationReference)
    {
        return;
    }

    MapMarkerInfoMessage tMsg;
    MarkerToMessage(tInfo, tMsg);

    PlayerList listPlayers(PlayerListGetElementPool());
    if (tInfo.m_idAssignedHero == OnlineService::INVALID_HERO_ID)
    {
        PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    }
    else
    {
        Player * ptPlayer = PlayerGetByHeroId(tInfo.m_idAssignedHero);
        if (ptPlayer)
        {
            listPlayers.AddTail_Tracked(ptPlayer);
        }
    }
    MessageSendToClient(listPlayers, SMSG_MAP_MARKER, &tMsg, sizeof(tMsg));
}



BOOL FindMarker(MinimapMarkerInfo & tInfo, uint32 nHashName)
{
    return GetActiveGameList()->Lookup(nHashName, tInfo);
}





BOOL FindMarker(MinimapMarkerInfo & tInfo, const char * pszMarker)
{
    return FindMarker(tInfo, MarkerStringHash(pszMarker));
}




void SendMarkerInfoToSpecificClient(const Player & tPlayer, const MinimapMarkerInfo & tInfo)
{
    MapMarkerInfoMessage tMsg;


    MarkerToMessage(tInfo, tMsg);

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);
    MessageSendToClient(listPlayers, SMSG_MAP_MARKER, &tMsg, sizeof(tMsg));
}


void UpdateMarker(MinimapMarkerInfo & tMarker)
{
    if (tMarker.m_bIsPing)
    {
        GetPingList()->SetAt_Tracked(tMarker.m_nHashedName, tMarker);
    }
    else
    {
        GetActiveGameList()->SetAt_Tracked(tMarker.m_nHashedName, tMarker);
    }
}



void GetMarkerTransform(Vector3D & wpFinal, const SceneCommonData * ptSCD, const Marker & tMarker)
{
    PRTransform transformSceneToWorld;
    SceneGetSceneToWorldTransform(ptSCD->idSScene, transformSceneToWorld);





    wpFinal = (transformSceneToWorld * tMarker.transform)

                  .wp;
}


CRefString LevelAreaToWayPointName(const SNO snoLevelArea)
{
    CRefString sMinimapName;
    sMinimapName.Format("%s.WaypointMarker", SNOToString(LEVELAREA_SNO, snoLevelArea, FALSE).str());
    return sMinimapName;
}

}  // namespace




void ChangeActs()
{
    MarkerList * pList = GetActiveGameList();
    if (pList)
    {
        pList->RemoveAll();
    }

    pList = GetPingList();
    if (pList)
    {
        pList->RemoveAll();
    }
}



inline CRefString sMarkerGetUniqueMarkerName()
{
    uint32 nGuid = SGameManager::GameGet().ptSMiniMapGlobals->nGuid++;
    CRefString sRet;
    sRet.Format("%d", nGuid);
    return sRet;
}





inline void sAddDiscoveryInfoToWorld(const MinimapMarkerInfo & tMarker)
{
    SWorld * ptSWorld = SWorldTryToGet(tMarker.m_Place.idSWorld);
    if (!ptSWorld)
    {
        return;
    }

    DiscoveryInfo tInfo;
    tInfo.m_nHashedName = tMarker.m_nHashedName;
    tInfo.m_Place = tMarker.m_Place;
    tInfo.m_fDiscoveryRangeSq = tMarker.m_fDiscoveryRangeSq;


    ptSWorld->mapUndiscoveredMinimapMarkers.SetAt_Tracked(tMarker.m_nHashedName, tInfo);
}


inline void sRemoveDiscoveryInfoFromWorld(const MinimapMarkerInfo & tMarker)
{
    SWorld * ptSWorld = SWorldTryToGet(tMarker.m_Place.idSWorld);
    if (!ptSWorld)
    {
        return;
    }

    DiscoveryInfo tInfo;

    if (ptSWorld->mapUndiscoveredMinimapMarkers.Lookup(tMarker.m_nHashedName, tInfo))
    {
        ptSWorld->mapUndiscoveredMinimapMarkers.RemoveAt(tMarker.m_nHashedName);
    }
}


void sCheckMinimapMarkerDiscovery(MinimapMarkerInfo tMarker)
{
    if (tMarker.m_bHasBeenDiscovered)
    {
        if (tMarker.m_bActive)
        {
            SendMarkerInfoToClients(tMarker);
        }
    }
    else
    {





        sAddDiscoveryInfoToWorld(tMarker);
    }
}

void AddMarker(const Marker & tMarker, const SceneCommonData * ptSCD, const PRTransform * ptTransform)
{


    static uint32 nInvalidHashedName = MarkerStringHash("Invalid");
    static uint32 nNoneHashedName = MarkerStringHash("None");
    uint32 nHashedName = tMarker.dwHash;

    if (nHashedName == nInvalidHashedName || nHashedName == nNoneHashedName)
    {

        uint32 tName = MarkerStringHash(sMarkerGetUniqueMarkerName().str());
        nHashedName = tName;
    }

    MinimapMarkerInfo tExistingInfo;
    if (FindMarker(tExistingInfo, nHashedName))
    {
        return;
    }

    MinimapMarkerInfo tInfo;
    tInfo.m_nHashedName = nHashedName;
    tInfo.m_Place.idSWorld = ptSCD->idSWorld;
    tInfo.m_ImageInfo.snoTexture = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_MINIMAP_ICON);
    tInfo.m_snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_MINIMAP);
    tInfo.m_hLabel = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_NAME);
    tInfo.m_ImageInfo.nFrame = 0;
    tInfo.m_snoQuestRange = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_QUESTRANGE);
    tInfo.m_snoKnownActorOverride = SNO_NONE;


    tInfo.m_bCanBecomeArrow = !TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_DOES_NOT_BECOME_ARROW);
    tInfo.m_bRespectsFoW = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_HIDE_UNTIL_EXPLORED);
    tInfo.m_bHasBeenDiscovered = !tInfo.m_bRespectsFoW;

    float fMaxRange = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_MAX_MINIMAP_RANGE);
    tInfo.m_fMaxDisplayRangeSq = fMaxRange * fMaxRange;

    float fMinRange = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_MIN_MINIMAP_RANGE);
    tInfo.m_fMinDisplayRangeSq = fMinRange * fMinRange;

    float fDiscoveryRange = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_MINIMAP_DISCOVERY_RADIUS);
    tInfo.m_fDiscoveryRangeSq = fDiscoveryRange * fDiscoveryRange;

    if (tInfo.m_snoQuestRange != SNO_NONE)
    {
        if (TagMapTagExists(tMarker.hTagMap, TAG_MARKER_MINIMAP_ACTIVE_BY_DEFAULT) &&
            TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_ACTIVE_BY_DEFAULT) == 0)
        {
            tInfo.m_bActive = FALSE;
        }
        else
        {
            tInfo.m_bActive = QuestRangeEvaluate(tInfo.m_snoQuestRange, NULL);
        }
    }
    else
    {
        tInfo.m_bActive = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_ACTIVE_BY_DEFAULT);
    }

    if (ptTransform)
    {
        tInfo.m_Place.idSWorld = ptSCD->idSWorld;
        tInfo.m_Place.wp = ptTransform->wp;
    }
    else
    {
        GetMarkerTransform(tInfo.m_Place.wp, ptSCD, tMarker);
    }

    MinimapMarkerInfo tExistingEntry;
    if (GetActiveGameList()->Lookup(tInfo.m_nHashedName, tExistingEntry))
    {



        if (tExistingEntry.m_bIsPlaceholder)
        {
            tInfo.m_bActive = tExistingEntry.m_bActive;
            tInfo.m_bDeactivatedByScript = tExistingEntry.m_bDeactivatedByScript;
        }
    }





























    GetActiveGameList()->SetAt_Tracked(tInfo.m_nHashedName, tInfo);


    sCheckMinimapMarkerDiscovery(tInfo);
}


inline void AddPlaceholderActivationMarker(const char * pszMarker, BOOL bActivate)
{
    MinimapMarkerInfo tInfo;
    tInfo.m_nHashedName = MarkerStringHash(pszMarker);
    tInfo.m_bActive = bActivate;
    tInfo.m_bIsPlaceholder = TRUE;







    GetActiveGameList()->SetAt_Tracked(tInfo.m_nHashedName, tInfo);
}


void AddSNOLocationMarker(const SNOName snoName, const WorldPlace tPlace, SSceneID idSScene)
{
    MinimapMarkerInfo tInfo;
    tInfo.m_Place = tPlace;
    tInfo.m_snoNameData = snoName;
    tInfo.m_bIsSNOLocationReference = TRUE;
    tInfo.m_bActive = FALSE;
    tInfo.m_idSScene = idSScene;
    tInfo.m_nHashedName = MarkerStringHash(CRefString().Format("SNOLocationMarker:%d:%s", snoName.snoGroup,
                                                               SNOToString(snoName, FALSE).str()));



    GetActiveGameList()->SetAt_Tracked(tInfo.m_nHashedName, tInfo);
}


inline CRefString GetMarkerNameFromPortal(const SceneCommonData * ptSCD, const ResolvedPortalDestination & tDest,
                                          const SNO snoActor, const SNO snoMarkerSet, const BOOL bDisabledInStoryMode)
{
    if (!ptSCD)
    {
        return CRefString("Invalid Portal Name");
    }
    else
    {
        CRefString sPortalName;
        sPortalName.Format("%d-%d-%d-%d-%d-%d", snoMarkerSet, snoActor, tDest.snoWorld, tDest.uEntranceGUID,
                     ptSCD->snoScene, bDisabledInStoryMode);
        return sPortalName;
    }
}





CRefString GetMarkerNameFromPortal(const Actor * ptPortal)
{
    const ActorCommonData * ptACDPortal = ACDTryToGet(ptPortal->idACD);
    if (!ptACDPortal)
    {
        return CRefString("Invalid Portal Name");
    }


    ResolvedPortalDestination tDest;
    ActorResolvePortalDestination(ptPortal, tDest);
    const SceneCommonData * ptSCD = SceneCommonDataTryToGet(ptPortal->idCurrentScene);
    if (!ptSCD)
    {
        return CRefString("Invalid Portal Name");
    }

    return GetMarkerNameFromPortal(ptSCD, tDest, ptPortal->snoActor, ptPortal->tMarkerOwner.snoMarkerSet, FALSE);
}


void AddPortalMarker(const SNO snoMarkerSet, const Marker & tMarker, const SceneCommonData * ptSCD,
                     const PRTransform * ptTransform)
{



    ResolvedPortalDestination tDest;
    MarkerResolvePortalDestination(tMarker, *ptSCD, tDest);
    BOOL bDisabledInStoryMode = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_DISABLED_IN_STORY_MODE);
    CRefString sPortalName = GetMarkerNameFromPortal(ptSCD, tDest, tMarker.snoname.snoHandle, snoMarkerSet,
                                                     bDisabledInStoryMode);
    MinimapMarkerInfo tInfo;
    if (FindMarker(tInfo, sPortalName.str()))
    {
        return;
    }
    tInfo.m_nHashedName = MarkerStringHash(sPortalName.str());
    tInfo.m_Place.idSWorld = ptSCD->idSWorld;
    tInfo.m_ImageInfo.snoTexture = ActorGetInheritedTagMapValueSNO(tMarker.snoname.snoHandle, tMarker,
                                                                   TAG_ACTOR_MINIMAP_ICON, TAG_MARKER_MINIMAP_ICON);
    tInfo.m_hLabel = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_NAME);
    tInfo.m_ImageInfo.nFrame = 0;
    tInfo.m_snoQuestRange = ActorGetInheritedTagMapValueSNO(tMarker.snoname.snoHandle, tMarker,
                                                            TAG_ACTOR_MINIMAP_QUEST_RANGE, TAG_MARKER_QUESTRANGE);
    tInfo.m_bCanBecomeArrow = !ActorGetInheritedTagMapValueInt(tMarker.snoname.snoHandle, tMarker,
                                                               TAG_ACTOR_MINIMAP_NO_ARROW,
                                                               TAG_MARKER_MINIMAP_DOES_NOT_BECOME_ARROW);
    tInfo.m_bRespectsFoW = ActorGetInheritedTagMapValueInt(tMarker.snoname.snoHandle, tMarker,
                                                           TAG_ACTOR_MINIMAP_RESPECTS_FOW,
                                                           TAG_MARKER_MINIMAP_HIDE_UNTIL_EXPLORED);
    tInfo.m_bHasBeenDiscovered = !tInfo.m_bRespectsFoW;
    float nMaxRange = ActorGetInheritedTagMapValueFloat(tMarker.snoname.snoHandle, tMarker,
                                                        TAG_ACTOR_MINIMAP_MAX_RANGE, TAG_MARKER_MAX_MINIMAP_RANGE);
    tInfo.m_fMaxDisplayRangeSq = nMaxRange * nMaxRange;

    float nMinRange = ActorGetInheritedTagMapValueFloat(tMarker.snoname.snoHandle, tMarker,
                                                        TAG_ACTOR_MINIMAP_MIN_RANGE, TAG_MARKER_MIN_MINIMAP_RANGE);
    tInfo.m_fMinDisplayRangeSq = nMinRange * nMinRange;

    float fDiscoveryRange = ActorGetInheritedTagMapValueFloat(tMarker.snoname.snoHandle, tMarker,
                                                              TAG_ACTOR_MINIMAP_DISCOVERY_RADIUS,
                                                              TAG_MARKER_MINIMAP_DISCOVERY_RADIUS);
    tInfo.m_fDiscoveryRangeSq = fDiscoveryRange * fDiscoveryRange;
    if (tInfo.m_snoQuestRange == SNO_NONE)
    {
        tInfo.m_bActive = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_START_DISABLED) == 0;
    }
    else
    {
        tInfo.m_bActive = QuestRangeEvaluate(tInfo.m_snoQuestRange, NULL);
    }
    if (ptTransform)
    {
        tInfo.m_Place.idSWorld = ptSCD->idSWorld;
        tInfo.m_Place.wp = ptTransform->wp;
    }
    else
    {
        GetMarkerTransform(tInfo.m_Place.wp, ptSCD, tMarker);
    }
    if (tDest.snoDestLevelArea == SNO_NONE)
    {
        tInfo.m_hLabel = sch_MinimapPortalDefaultLabel;
        tInfo.m_snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_MINIMAP);
    }
    else
    {
        tInfo.m_hLabel = XHashKey<const char *>(SNOToString(LEVELAREA_SNO, tDest.snoDestLevelArea, FALSE));
        tInfo.m_snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_LEVEL_AREAS);

        if (SNOHasAutoPing(tDest.snoDestLevelArea))
        {
            SNO snoQuestSource = SQuestGetBountyQuestSNOWithLevelAreaObjective(tDest.snoDestLevelArea);

            PingID idPing = PingLocation(tInfo.m_Place, 0, OnlineService::INVALID_HERO_ID, SNO_NONE, TRUE,
                                         tInfo.m_hLabel, tInfo.m_snoStringList, SNO_NONE,
                                         BOUNTY_LEVEL_AREA_AUTOPING_DISTANCE, snoQuestSource);



            SNOAddTrackedPing(tDest.snoDestLevelArea, idPing);

            SQuestsRemoveBountyLevelArea(tDest.snoDestLevelArea, snoQuestSource);
        }
    }


    tInfo.m_snoDestLevelArea = tDest.snoDestLevelArea;

    GetActiveGameList()->SetAt_Tracked(tInfo.m_nHashedName, tInfo);

    sCheckMinimapMarkerDiscovery(tInfo);
}




void RevealPortalMarker(const Actor * ptPortal)
{
    if (!ptPortal)
    {
        return;
    }


    BOOL bReveal = ActorGetInheritedTagMapValueInt(ptPortal, TAG_ACTOR_MINIMAP_REVEAL_TO_ALL,
                                                   TAG_MARKER_MINIMAP_REVEAL_TO_ALL);
    if (!bReveal)
    {
        return;
    }
    CRefString sMarker = GetMarkerNameFromPortal(ptPortal);
    MinimapMarkerInfo tMarker;
    if (FindMarker(tMarker, sMarker.str()))
    {
        tMarker.m_bActive = TRUE;
        tMarker.m_bDeactivatedByScript = FALSE;
        tMarker.m_fMinDisplayRangeSq = 0.0f;
        tMarker.m_bRespectsFoW = FALSE;
        tMarker.m_bHasBeenDiscovered = TRUE;

        UpdateMarker(tMarker);

        SendMarkerInfoToClients(tMarker);

        sRemoveDiscoveryInfoFromWorld(tMarker);
    }
}
// On its DWARF lines 761-867 (param 762, brace 763, snoLevelArea 778, sMarkerName 853); the earlier
// csel-versus-branch difference on idSSceneWaypoint went away with the line layout.
void AddWaypointMarker(
    const SScene & tSScene)
{
    MinimapMarkerInfo tInfo;
    tInfo.m_Place.idSWorld = tSScene.tSCD.idSWorld;
    tInfo.m_Place.wp = tSScene.transWaypoint.wp;

    NavMeshHandle tNavMeshHandle;
    if (!ScenesCommonNavMeshNodeFromPlace(tInfo.m_Place, &tNavMeshHandle, FALSE, TRUE))
    {
        return;
    }





    SNO snoLevelArea = NavMeshHandleGetLevelArea(&tSScene.tSCD, tNavMeshHandle);
    int32 nWaypoint = ActGetWaypointByLevelArea(snoLevelArea, FALSE);
    if (nWaypoint == -1)
    {
        return;
    }





    tInfo.m_bIsWaypoint = TRUE;

    if (tSScene.snoWaypointTexture != SNO_NONE)
    {
        tInfo.m_ImageInfo.snoTexture = tSScene.snoWaypointTexture;
    }
    else
    {
        tInfo.m_ImageInfo.snoTexture = GlobalSNOGet(GLOBALSNO_TEXTURE_MARKER_WAYPOINT_LOCATION);
    }

    tInfo.m_ImageInfo.nFrame = 0;

    tInfo.m_snoQuestRange = SNO_NONE;

    if (tSScene.fWaypointMaxRange > 0.0f)
    {
        tInfo.m_fMaxDisplayRangeSq = tSScene.fWaypointMaxRange * tSScene.fWaypointMaxRange;
    }
    else
    {
        tInfo.m_fMaxDisplayRangeSq = 0.0f;
    }

    if (tSScene.fWaypointMinRange > 0.0f)
    {
        tInfo.m_fMinDisplayRangeSq = tSScene.fWaypointMinRange * tSScene.fWaypointMinRange;
    }
    else
    {
        tInfo.m_fMinDisplayRangeSq = 0.0f;
    }

    if (tSScene.fWaypointDiscoveryRange > 0.0f)
    {
        tInfo.m_fDiscoveryRangeSq = tSScene.fWaypointDiscoveryRange * tSScene.fWaypointDiscoveryRange;
    }
    else
    {
        tInfo.m_fDiscoveryRangeSq = 0.0f;
    }
    tInfo.m_bCanBecomeArrow = FALSE;
    tInfo.m_bRespectsFoW = FALSE;
    tInfo.m_bHasBeenDiscovered = TRUE;


    int32 nLevelAreaIdx = NavMeshHandleGetLevelAreaIndex(tNavMeshHandle);
    if (nLevelAreaIdx >= 0 && nLevelAreaIdx < 4)
    {
        LevelAreaID idLevelArea = tSScene.arLevelAreaIds[nLevelAreaIdx];
        LevelArea * ptLevelArea = LevelAreaTryToGet(idLevelArea);
        if (ptLevelArea)
        {
            if (tSScene.tSCD.idSSceneParent != SSCENEID_INVALID)
            {
                ptLevelArea->idSSceneWaypoint = tSScene.tSCD.idSSceneParent;
            }
            else
            {
                ptLevelArea->idSSceneWaypoint = tSScene.id;
            }
        }
    }

    CRefString sMarkerName = LevelAreaToWayPointName(snoLevelArea);
    tInfo.m_nHashedName = MarkerStringHash(sMarkerName.str());

    tInfo.m_snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_MINIMAP);
    tInfo.m_hLabel = StringListGetLabelHandle("Waypoint");


    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tInfo.m_nWaypointNumber = nWaypoint;
    tInfo.m_bActive = tSGameGlobals.WaypointIsActive(nWaypoint);

    GetActiveGameList()->SetAt_Tracked(tInfo.m_nHashedName, tInfo);

    sCheckMinimapMarkerDiscovery(tInfo);
}



void AddQuestMarker(const Marker & tMarker, const SceneCommonData * ptSCD)
{


    CRefString sMarkerName;
    sMarkerName.Format("%s.QuestMarker", SNOToString(tMarker.snoname, FALSE).str());

    uint32 nHashedName = MarkerStringHash(sMarkerName.str());


    MinimapMarkerInfo tInfo;
    tInfo.m_nHashedName = nHashedName;
    tInfo.m_Place.idSWorld = ptSCD->idSWorld;
    tInfo.m_ImageInfo.snoTexture = GlobalSNOGet(GLOBALSNO_TEXTURE_MARKER_EVENT_LOCATION); tInfo.m_ImageInfo.nFrame = 0;
    tInfo.m_bActive = FALSE;

    tInfo.m_snoQuestRange = SNO_NONE;

    tInfo.m_fMinDisplayRangeSq = 3600.0f;
    tInfo.m_bCanBecomeArrow = TRUE;
    tInfo.m_bRespectsFoW = FALSE; tInfo.m_bHasBeenDiscovered = TRUE;

    SNO snoOverride = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_MINIMAP_ICON);
    if (SNOFileExists(TEXTURE_SNO, snoOverride) && snoOverride != GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_DEST_ICON))
    {
        tInfo.m_ImageInfo.snoTexture = snoOverride;
    }

    tInfo.m_snoStringList = QuestGetStringList(tMarker.snoname.snoHandle);
    tInfo.m_hLabel = StringListGetLabelHandle(gszQuestNameLabel);


    float fMaxRange = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_MAX_MINIMAP_RANGE);
    if (fMaxRange != 0.0f)
    {
        tInfo.m_fMaxDisplayRangeSq = fMaxRange * fMaxRange;
    }


    float fMinRange = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_MIN_MINIMAP_RANGE);
    if (fMinRange != 0.0f)
    {
        tInfo.m_fMinDisplayRangeSq = fMinRange * fMinRange;
    }

    float fDiscoveryRange = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_MINIMAP_DISCOVERY_RADIUS);
    if (fDiscoveryRange != 0.0f)
    {
        tInfo.m_fDiscoveryRangeSq = fDiscoveryRange * fDiscoveryRange;
    }

    GetMarkerTransform(tInfo.m_Place.wp, ptSCD, tMarker);












    GetActiveGameList()->SetAt_Tracked(tInfo.m_nHashedName, tInfo);
}


void AddStaticGizmoMarker(ActorCommonData & tACD)
{

    MinimapMarkerSetInfo tMarkerSetInfo;
    SNO snoIcon;
    CRefString sRet;
    ResolveIconForActor(tACD, tACD.snoActor, tMarkerSetInfo, -1, snoIcon, sRet);

    MinimapMarkerInfo tInfo;



    PingID idPing = tACD.AttributesGetInt(ATTRIBUTE_PING);
    if (idPing == PINGID_INVALID)
    {
        idPing = sGetNewPingID();
        tACD.AttributesSetInt(ATTRIBUTE_PING, idPing);
    }
    else if (GetPingList()->Lookup(idPing, tInfo))
    {


        tInfo.m_ImageInfo.snoTexture = snoIcon;
        SendMarkerInfoToClients(tInfo);
        return;
    }

    tInfo.m_Place = tACD.GetPlace();
    tInfo.m_bIsPing = TRUE;
    tInfo.m_nHashedName = idPing;
    tInfo.m_ImageInfo.snoTexture = snoIcon;

    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
    tInfo.m_snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS);
    tInfo.m_hLabel = StringListGetLabelHandle(SNOToString(ACTOR_SNO, tACD.snoActor, FALSE).str());
    tInfo.m_ImageInfo.nFrame = 0;
    tInfo.m_snoQuestRange = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_QUEST_RANGE);
    tInfo.m_snoKnownActorOverride = SNO_NONE;

    tInfo.m_bCanBecomeArrow = !TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_NO_ARROW);
    tInfo.m_bRespectsFoW = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_RESPECTS_FOW);
    tInfo.m_bHasBeenDiscovered = !tInfo.m_bRespectsFoW;

    float fMaxRange = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_MAX_RANGE);
    tInfo.m_fMaxDisplayRangeSq = fMaxRange * fMaxRange;

    float fMinRange = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_MIN_RANGE);
    tInfo.m_fMinDisplayRangeSq = fMinRange * fMinRange;

    float fDiscoveryRange = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_DISCOVERY_RADIUS);
    tInfo.m_fDiscoveryRangeSq = fDiscoveryRange * fDiscoveryRange;

    if (tInfo.m_snoQuestRange != SNO_NONE)
    {
        if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ACTIVE_BY_DEFAULT) &&
            TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ACTIVE_BY_DEFAULT) == 0)
        {
            tInfo.m_bActive = FALSE;
        }
        else
        {
            tInfo.m_bActive = QuestRangeEvaluate(tInfo.m_snoQuestRange, NULL);
        }
    }
    else
    {
        tInfo.m_bActive = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ACTIVE_BY_DEFAULT);
    }

    UpdateMarker(tInfo);

    SendMarkerInfoToClients(tInfo);
}



void ResolveIconForActor(const ActorCommonData & tACD, const Actor * ptActor, const Player * ptPlayer, SNO & snoRet,
                         CRefString & szRet)
{
    MinimapMarkerSetInfo tInfo(ptActor->tMarkerOwner);
    ResolveIconForActor(tACD, tACD.snoActor, tInfo, ptPlayer->nPlayerIndex, snoRet, szRet);
}












void ResolveMouseOverNameForActor(const Actor * ptActor, uint32 & hStringListHandle, SNO & snoStringList)
{
    snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_MINIMAP);
    hStringListHandle = (uint32)-1;

    if (ptActor->tMarkerOwner.snoMarkerSet != SNO_NONE)
    {




        ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
        const Marker & tMarker = pdefMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];
        if (TagMapTagExists(tMarker.hTagMap, TAG_MARKER_MINIMAP_NAME))
        {
            hStringListHandle = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_MINIMAP_NAME);
        }
    }


    if (hStringListHandle == (uint32)-1 || hStringListHandle == 0)
    {
        hStringListHandle = XHashKey<const char *>(SNOToString(ACTOR_SNO, ptActor->snoActor, FALSE));
    }
}






void DiscoverMinimapMarkers(const ActorCommonData & tACDPlayer)
{

    ScopedGlobalDefinitionLockR pdefGlobal;
    float flRevealRadiusSq = pdefGlobal->flMinimapRevealRadius * pdefGlobal->flMinimapRevealRadius;

    WorldPlace tPlayerLoc = tACDPlayer.GetPlace();
    Vector2D vPlayerPlace(tPlayerLoc.wp.x, tPlayerLoc.wp.y);

    SWorld * ptSWorld = SWorldTryToGet(tPlayerLoc.idSWorld);
    if (!ptSWorld)
    {
        return;
    }

    uint32 nKey;
    DiscoveryInfo tDiscoveryInfo;
    POS pos = ptSWorld->mapUndiscoveredMinimapMarkers.GetStartAssoc();
    while (pos)
    {

        ptSWorld->mapUndiscoveredMinimapMarkers.GetNextAssoc(pos, nKey, tDiscoveryInfo);
        Vector2D vMarkerPlace(tDiscoveryInfo.m_Place.wp.x, tDiscoveryInfo.m_Place.wp.y);

        float flDistSq = Vector2D::DistanceSquared(vMarkerPlace, vPlayerPlace);

        if (flDistSq < flRevealRadiusSq || flDistSq < tDiscoveryInfo.m_fDiscoveryRangeSq)
        {
            ptSWorld->mapUndiscoveredMinimapMarkers.RemoveAt(nKey);

            MinimapMarkerInfo tMarker;
            if (GetActiveGameList()->Lookup(tDiscoveryInfo.m_nHashedName, tMarker))
            {
                tMarker.m_bHasBeenDiscovered = TRUE;
                UpdateMarker(tMarker);

                if (tMarker.m_bActive)
                {
                    SendMarkerInfoToClients(tMarker);
                }
            }
        }
    }
}


void OnNewPlayer(const Player & tPlayer)
{


    MinimapMarkerInfo tMarker;
    POS pos;
    uint32 nKey;
    pos = GetActiveGameList()->GetStartAssoc();
    while (pos)
    {
        GetActiveGameList()->GetNextAssoc(pos, nKey, tMarker);

        if (tMarker.m_bActive && tMarker.m_bHasBeenDiscovered)
        {



            SendMarkerInfoToSpecificClient(tPlayer, tMarker);
        }
    }

    pos = GetPingList()->GetStartAssoc();
    while (pos)
    {
        GetPingList()->GetNextAssoc(pos, nKey, tMarker);
        if (tMarker.m_idAssignedHero == OnlineService::INVALID_HERO_ID)
        {
            SendMarkerInfoToSpecificClient(tPlayer, tMarker);
        }
        else if (tMarker.m_idAssignedHero == tPlayer.uHeroId)
        {
            SendMarkerInfoToSpecificClient(tPlayer, tMarker);
        }
    }
}
void OnRemovedPlayer(const Player & tPlayer)
{
    uint32 dwPrevPortalHash = tPlayer.dwPrevPortalHash;
    MinimapMarkerInfo tMarker;
    if (FindMarker(tMarker, dwPrevPortalHash))
    {
        tMarker.m_nPlayerUseFlags &= ~(1 << tPlayer.nPlayerIndex);
        UpdateMarker(tMarker);

        if (tMarker.m_bActive && tMarker.m_bHasBeenDiscovered)
        {
            SendMarkerInfoToClients(tMarker);
        }
    }
}



inline CRefString ActorGetMarkerName(const Actor * ptActor)
{
    CRefString sRet;
    sRet.Format("%d-%d-%d-%d-%d", ptActor->tMarkerOwner.snoMarkerSet, ptActor->tMarkerOwner.nMarkerIndex,
                ptActor->tMarkerOwner.nMarkerSetInstanceIndex, ptActor->tMarkerOwner.nAdventureParentIndex,
                ptActor->snoActor);
    return sRet;
}







static void ActivateMarker(MinimapMarkerInfo & tInfo)
{
    if (tInfo.m_bActive)
    {
        return;
    }

    tInfo.m_bDeactivatedByScript = FALSE;
    tInfo.m_bActive = TRUE;


    UpdateMarker(tInfo);

    if (tInfo.m_bHasBeenDiscovered)
    {
        SendMarkerInfoToClients(tInfo);
    }
}


void ActivateMarker(const char * pszMarker)
{
    MinimapMarkerInfo tInfo;

    if (!FindMarker(tInfo, pszMarker))
    {
        AddPlaceholderActivationMarker(pszMarker, TRUE);
        return;
    }




    ActivateMarker(tInfo);
}


void ActivateMarker(const ACDID idACD)
{
    ActorCommonData * ptACDMarker = ACDTryToGet(idACD);
    if (!ptACDMarker)
    {
        return;
    }

    const Actor * ptActor = ActorTryToGet((ActorID)ptACDMarker->idOwner);
    if (!ptActor)
    {
        return;
    }

    ptACDMarker->AttributesSetInt(ATTRIBUTE_MINIMAP_ICON_ACTIVE, TRUE);



    MinimapMarkerInfo tInfo;
    if (FindMarker(tInfo, ActorGetMarkerName(ptActor).str()))
    {
        ActivateMarker(tInfo);
    }

    if (FindMarker(tInfo, GetMarkerNameFromPortal(ptActor).str()))
    {
        ActivateMarker(tInfo);
    }

    STrickleUpdateActor(*ptACDMarker);
}


void ActivateWaypointMarker(const SNO snoLevelArea)
{
    CRefString sMinimapName = LevelAreaToWayPointName(snoLevelArea);

    ActivateMarker(sMinimapName.str());



    MarkerList * ptList = GetActiveGameList();
    POS pos = ptList->GetStartAssoc();
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    while (pos)
    {
        MinimapMarkerInfo tMarker;
        uint32 nKey;
        ptList->GetNextAssoc(pos, nKey, tMarker);

        if (tMarker.m_bIsWaypoint)
        {
            uint8 bPrevState = tMarker.m_bActive;
            tMarker.m_bActive = tSGameGlobals.WaypointIsActive(tMarker.m_nWaypointNumber);
            if (tMarker.m_bActive)
            {
                tMarker.m_bDeactivatedByScript = FALSE;
            }

            if (tMarker.m_bActive != bPrevState)
            {
                UpdateMarker(tMarker);
                SendMarkerInfoToClients(tMarker);
            }
        }
    }
}


static void DeactivateMarker(MinimapMarkerInfo & tInfo, BOOL bFromScript)
{
    if (!tInfo.m_bActive)
    {
        return;
    }



    tInfo.m_bActive = FALSE;
    if (bFromScript)
    {
        tInfo.m_bDeactivatedByScript = TRUE;
    }

    UpdateMarker(tInfo);

    if (tInfo.m_bHasBeenDiscovered)
    {
        SendMarkerInfoToClients(tInfo);
    }
}


void DeactivateMarker(const ACDID idACD, BOOL bFromScript)
{
    ActorCommonData * ptACDMarker = ACDTryToGet(idACD);
    if (!ptACDMarker)
    {
        return;
    }

    ptACDMarker->AttributesSetInt(ATTRIBUTE_MINIMAP_ICON_ACTIVE, FALSE);
    RegisterQuestActor(idACD, FALSE);

    const Actor * ptActor = ActorTryToGet((ActorID)ptACDMarker->idOwner);
    if (!ptActor)
    {
        return;
    }




    MinimapMarkerInfo tInfo;
    if (FindMarker(tInfo, ActorGetMarkerName(ptActor).str()))
    {
        DeactivateMarker(tInfo, bFromScript);
    }

    if (FindMarker(tInfo, GetMarkerNameFromPortal(ptActor).str()))
    {
        DeactivateMarker(tInfo, bFromScript);
    }

    STrickleUpdateActor(*ptACDMarker);
}


void DeactivateMarker(const char * pszMarker, BOOL bFromScript)
{
    MinimapMarkerInfo tInfo;

    if (!FindMarker(tInfo, pszMarker))
    {







        AddPlaceholderActivationMarker(pszMarker, FALSE);
        return;
    }

    DeactivateMarker(tInfo, bFromScript);
}


void MarkerSetIcon(const SNO snoIcon, const ACDID idACD)
{


    if (!SNOFileExists(TEXTURE_SNO, snoIcon) || snoIcon == SNO_NONE || idACD == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData * ptACDMarker = ACDTryToGet(idACD);
    if (!ptACDMarker)
    {
        return;
    }

    const Actor * ptActor = ActorTryToGet((ActorID)ptACDMarker->idOwner);
    if (!ptActor)
    {
        return;
    }

    ptACDMarker->AttributesSetInt(ATTRIBUTE_MINIMAP_ICON_OVERRIDE, snoIcon);


    MarkerSetIcon(snoIcon, ActorGetMarkerName(ptActor).str());
    MarkerSetIcon(snoIcon, GetMarkerNameFromPortal(ptActor).str());

    STrickleUpdateActor(*ptACDMarker);
}


void MarkerSetIcon(const SNO snoIcon, const char * pszMarker)
{
    MinimapMarkerInfo tInfo;
    if (!FindMarker(tInfo, pszMarker))
    {
        return;
    }

    tInfo.m_ImageInfo.snoTexture = snoIcon;
    UpdateMarker(tInfo);

    if (tInfo.m_bActive && tInfo.m_bHasBeenDiscovered)
    {
        SendMarkerInfoToClients(tInfo);
    }
}




void EnableMarkerArrow(const ACDID idACD, const BOOL bEnable)
{
    ActorCommonData * ptACDMarker = ACDTryToGet(idACD);
    if (!ptACDMarker)
    {
        return;
    }

    const Actor * ptActor = ActorTryToGet((ActorID)ptACDMarker->idOwner);
    if (!ptActor)
    {
        return;
    }

    ptACDMarker->AttributesSetInt(ATTRIBUTE_MINIMAP_ICON_DISABLE_ARROW, bEnable);




    EnableMarkerArrow(ActorGetMarkerName(ptActor).str(), bEnable);
    EnableMarkerArrow(GetMarkerNameFromPortal(ptActor).str(), bEnable);

    STrickleUpdateActor(*ptACDMarker);
}


void EnableMarkerArrow(const char * pszMarker, const BOOL bEnable)
{
    MinimapMarkerInfo tInfo;
    if (!FindMarker(tInfo, pszMarker))
    {
        return;
    }

    tInfo.m_bCanBecomeArrow = bEnable;
    UpdateMarker(tInfo);

    if (tInfo.m_bActive && tInfo.m_bHasBeenDiscovered)
    {
        SendMarkerInfoToClients(tInfo);
    }
}




void UpdateMarkersForQuestState()
{
    POS pos = GetActiveGameList()->GetStartAssoc();
    while (pos)
    {


        MinimapMarkerInfo tMarker;
        uint32 nKey;
        GetActiveGameList()->GetNextAssoc(pos, nKey, tMarker);

        if (tMarker.m_snoQuestRange != SNO_NONE && !tMarker.m_bDeactivatedByScript)
        {
            BOOL bInRange = QuestRangeEvaluate(tMarker.m_snoQuestRange, NULL);
            if (bInRange)
            {
                ActivateMarker(tMarker);
            }
            else
            {
                DeactivateMarker(tMarker, FALSE);
            }
        }
    }
}


void UpdateWaypointMarkersFromPlayer(const Player & tPlayer)
{
    POS pos = GetActiveGameList()->GetStartAssoc();
    while (pos)
    {


        MinimapMarkerInfo tMarker;
        uint32 nKey;
        GetActiveGameList()->GetNextAssoc(pos, nKey, tMarker);
        if (tMarker.m_nWaypointNumber != -1 && !tMarker.m_bActive)
        {
            BOOL bState = PlayerHasActivatedWaypoint(tPlayer, tMarker.m_nWaypointNumber);
            if (bState)
            {

                ActivateMarker(tMarker);
            }
        }
    }
}


void OnPlayerWarped(const ActorCommonData & tACDPlayer, PlayerWarpReason eWarpReason, const ActorCommonData * ptACDPortal)
{



    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
    {
        return;
    }

    uint32 dwPrevPortalHash = ptPlayer->dwPrevPortalHash;
    MinimapMarkerInfo tMarker;
    if (FindMarker(tMarker, dwPrevPortalHash))
    {
        tMarker.m_nPlayerUseFlags &= ~(1 << ptPlayer->nPlayerIndex);
        UpdateMarker(tMarker);

        if (tMarker.m_bActive && tMarker.m_bHasBeenDiscovered)
        {
            SendMarkerInfoToClients(tMarker);
        }
    }

    if (eWarpReason != WARP_CONSOLE_PORTAL && eWarpReason != WARP_PORTAL)
    {
        return;
    }

    if (!ptACDPortal)
    {
        return;
    }

    const Actor * ptPortal = ActorTryToGet((ActorID)ptACDPortal->idOwner);
    if (!ptPortal)
    {
        return;
    }

    CRefString sMarkerName = GetMarkerNameFromPortal(ptPortal);
    if (FindMarker(tMarker, sMarkerName))
    {
        ptPlayer->dwPrevPortalHash = tMarker.m_nHashedName;
        tMarker.m_nPlayerUseFlags |= (1 << ptPlayer->nPlayerIndex);
        UpdateMarker(tMarker);

        if (tMarker.m_bActive && tMarker.m_bHasBeenDiscovered)
        {
            SendMarkerInfoToClients(tMarker);
        }
    }
}


static void sRemoveAllForWorld(SWorldID idSWorld, MarkerList * ptList)
{
    if (ptList)
    {
        POS pos = ptList->GetStartAssoc();
        while (pos)
        {
            MinimapMarkerInfo tMarker;
            uint32 nKey;
            ptList->GetNextAssoc(pos, nKey, tMarker);
            if (tMarker.m_Place.idSWorld == idSWorld)
            {
                ptList->RemoveAt(nKey);
            }
        }
    }
}





void RemoveAllForWorld(SWorldID idSWorld)
{
    sRemoveAllForWorld(idSWorld, GetActiveGameList());
    sRemoveAllForWorld(idSWorld, GetPingList());
}



static PingID sGetNewPingID()
{
    uint32 uPingID = SGameManager::GameGet().ptSMiniMapGlobals->idPingNext++;
    return uPingID;
}




void sPingDeleteFromTimer(TimerID, DataID idOwner, void *)
{
    PingID idPing = idOwner;
    RemovePing(idPing);
}





PingID PingLocation(const WorldPlace & placePing,
                    int32 nDuration,
                    OnlineService::HeroId idAssignedHero,
                    SNO snoTexture,
                    BOOL bCanBecomeArrow,
                    StringLabelHandle hLabel,
                    SNO snoStringList,
                    SNO snoKnownActorOverride,
                    float flMaxDisplayRange,
                    SNO snoQuestSource)
{
    {



        MinimapMarkerInfo tInfo;
        tInfo.m_Place = placePing;
        tInfo.m_bIsPing = TRUE;
        tInfo.m_nHashedName = sGetNewPingID();

        tInfo.m_idAssignedHero = idAssignedHero;
        tInfo.m_fMinDisplayRangeSq = 0.0f;
        tInfo.m_bActive = TRUE;
        tInfo.m_bCanBecomeArrow = bCanBecomeArrow;
        tInfo.m_hLabel = hLabel;
        tInfo.m_snoStringList = snoStringList;
        tInfo.m_snoKnownActorOverride = snoKnownActorOverride;
        tInfo.m_fMaxDisplayRangeSq = flMaxDisplayRange * flMaxDisplayRange;
        tInfo.m_snoQuestSource = snoQuestSource;
        if (snoTexture == SNO_NONE)
        {




            snoTexture = GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_DEST_ICON);
        }

        tInfo.m_ImageInfo.snoTexture = snoTexture;
        if (nDuration > 0)
        {
            TimerRegister(tInfo.m_nHashedName, TIMER_OWNER_NONE, nDuration, NULL, sPingDeleteFromTimer);
        }
        GetPingList()->SetAt_Tracked(tInfo.m_nHashedName, tInfo);

        if (snoQuestSource != SNO_NONE && QuestIsAnyBountyType(snoQuestSource))
        {
            MapMarkerInfoMessage tMsg;

            MarkerToMessage(tInfo, tMsg);

            PlayerList listPlayers(PlayerListGetElementPool());
            if (tInfo.m_idAssignedHero == OnlineService::INVALID_HERO_ID)
            {
                PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
            }
            else
            {
                Player * ptPlayer = PlayerGetByHeroId(tInfo.m_idAssignedHero);
                if (ptPlayer)
                {
                    listPlayers.AddTail_Tracked(ptPlayer);
                }
            }

            MessageSendToClient(listPlayers, SMSG_MAP_OBJECTIVE_MARKER, &tMsg, sizeof(tMsg));
        }

        SendMarkerInfoToClients(tInfo);

        return tInfo.m_nHashedName;
    }
}




void UpdatePingLocation(PingID idPing, WorldPlace tPlace)
{
    MinimapMarkerInfo tInfo;
    if (GetPingList()->Lookup(idPing, tInfo))
    {
        tInfo.m_Place = tPlace;
        SendMarkerInfoToClients(tInfo);
    }
}




void RemovePing(PingID idPing)
{
    MinimapMarkerInfo tInfo;
    if (GetPingList()->Lookup(idPing, tInfo))
    {
        GetPingList()->RemoveAt(idPing);
        tInfo.m_bActive = FALSE;
        SendMarkerInfoToClients(tInfo);
    }
}

}  // namespace Minimap




void MinimapDeactivateMarker(const char * pszMarker)
{
    Minimap::DeactivateMarker(pszMarker, TRUE);
}



void MinimapActivateMarker(const char * pszMarker)
{
    Minimap::ActivateMarker(pszMarker);
}



void MinimapDeactivateMarker(const ACDID idACD)
{
    Minimap::DeactivateMarker(idACD, TRUE);
}



void MinimapActivateMarker(const ACDID idACD)
{
    Minimap::ActivateMarker(idACD);
}



void MinimapSetIcon(const ACDID idACD, const char * szIconName)
{
    if (!szIconName)
    {
        return;
    }

    SNO snoIcon = SNOFileLookupGUID(TEXTURE_SNO, szIconName);
    if (snoIcon == SNO_NONE)
    {
        return;
    }

    Minimap::MarkerSetIcon(snoIcon, idACD);
}





void MinimapSetIcon(const char * pszMarker, const char * szIconName)
{
    if (!pszMarker || !szIconName)
    {
        return;
    }

    SNO snoIcon = SNOFileLookupGUID(TEXTURE_SNO, szIconName);
    if (snoIcon == SNO_NONE)
    {
        return;
    }

    Minimap::MarkerSetIcon(snoIcon, szIconName);
}





void MinimapEnableMarkerArrow(const ACDID idACD, BOOL bEnable)
{
    Minimap::EnableMarkerArrow(idACD, bEnable);
}




void MinimapEnableMarkerArrow(const char * pszMarker, BOOL bEnable)
{
    if (!pszMarker)
    {
        return;
    }

    Minimap::EnableMarkerArrow(pszMarker, bEnable);
}

namespace Minimap {


BOOL LevelAreaAutoPing(SNO snoLevelArea, SNO snoQuestSource, float flMaxDisplayRange)
{
    BOOL bFound = FALSE;



    MinimapMarkerInfo tMarker;

    POS pos;
    uint32 nKey;

    for (pos = GetActiveGameList()->GetStartAssoc(); pos != NULL;)
    {

        GetActiveGameList()->GetNextAssoc(pos, nKey, tMarker);
        if (tMarker.m_snoDestLevelArea == snoLevelArea)
        {

            PingID idPing = PingLocation(tMarker.m_Place, 0, OnlineService::INVALID_HERO_ID, SNO_NONE, TRUE,
                                         XHashKey<const char *>(SNOToString(LEVELAREA_SNO, snoLevelArea, FALSE)),
                                         GlobalSNOGet(GLOBALSNO_STRINGLIST_LEVEL_AREAS), SNO_NONE,
                                         flMaxDisplayRange, snoQuestSource);






            SNOAddTrackedPing(snoLevelArea, idPing);
            bFound = TRUE;
        }
    }

    return bFound;
}


void ChallengeAutoPing(SNO snoChallenge, SNO snoQuestSource)
{




    MinimapMarkerInfo tMarker;
    POS pos;
    uint32 nKey;
    pos = GetActiveGameList()->GetStartAssoc();
    while (pos)
    {
        GetActiveGameList()->GetNextAssoc(pos, nKey, tMarker);
        if (tMarker.m_snoNameData.snoGroup == ACTOR_SNO)
        {
            ScopedActorDefinitionLockR pdefActor(tMarker.m_snoNameData.snoHandle);
            if (TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHALLENGE_SNO) == snoChallenge)
            {
                PingID idPing = PingLocation(tMarker.m_Place, 0, OnlineService::INVALID_HERO_ID, SNO_NONE, TRUE,
                                             XHashKey<const char *>(gszQuestNameLabel),
                                             QuestGetStringList(snoChallenge), SNO_NONE,
                                             BOUNTY_CHALLENGE_AUTOPING_DISTANCE, snoQuestSource);






                SNOAddTrackedPing(snoChallenge, idPing);
            }
        }
    }
}


void ActorAutoPing(ACDID idACD, SNO snoQuestSource)
{


    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(idACD);

        CRefString sStringLabel;
        SNO snoStringList;
        ScopedActorDefinitionLockR pdefActor(tACD.snoActor);

        if (pdefActor->eType == ACTORTYPE_MONSTER)
        {
            sStringLabel = MonsterGetSNOName(SNOName(ACTOR_SNO, tACD.snoActor));
            snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTERS);
        }
        else
        {
            sStringLabel = SNOToString(ACTOR_SNO, tACD.snoActor, FALSE);
            snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS);
        }

        tACD.AttributesSetInt(ATTRIBUTE_BOUNTY_OBJECTIVE, TRUE);

        if (SNOGetAutoPing(tACD.snoActor) == PINGID_INVALID)
        {
            PingID idPing = PingLocation(tACD.GetWorldPlaceCenter(), 0, OnlineService::INVALID_HERO_ID, SNO_NONE, TRUE,
                                         XHashKey<const char *>(sStringLabel.str()), snoStringList, tACD.snoActor,
                                         BOUNTY_ACTOR_AUTOPING_DISTANCE, snoQuestSource);








            SNOAddTrackedPing(tACD.snoActor, idPing);
        }
        else
        {
            PingID idPing = SNOGetAutoPing(tACD.snoActor);
            UpdatePingLocation(idPing, tACD.GetWorldPlaceCenter());
        }
    }
}


void EventAutoPing(SNO snoEvent, SNO snoQuestSource)
{



    MinimapMarkerInfo tMarker;
    POS pos;
    uint32 nKey;
    pos = GetActiveGameList()->GetStartAssoc();
    while (pos)
    {
        GetActiveGameList()->GetNextAssoc(pos, nKey, tMarker);
        if (tMarker.m_bIsSNOLocationReference && tMarker.m_snoNameData.snoHandle == snoEvent)
        {
            PingID idPing = PingLocation(tMarker.m_Place, 0, OnlineService::INVALID_HERO_ID, SNO_NONE, TRUE,
                                         XHashKey<const char *>(gszQuestNameLabel), QuestGetStringList(snoEvent),
                                         SNO_NONE, BOUNTY_EVENT_AUTOPING_DISTANCE, snoQuestSource);








            SNOAddTrackedPing(snoEvent, idPing);
        }
    }
}

























void SNOAddAutoPingRef(SNO sno, SNOGroup snoGroup, SNO snoQuestSource, BOOL bIsBossBounty)
{
    SNOAutoPingRefMap & tSNOAutoPingRefMap = *SGameManager::GameGet().ptSMiniMapGlobals->ptSNOAutoPingRefs;

    int32 nRefs;
    if (!tSNOAutoPingRefMap.Lookup(sno, nRefs))
    {
        if (snoGroup == LEVELAREA_SNO)
        {
            if (!LevelAreaAutoPing(sno, snoQuestSource,
                                   bIsBossBounty ? BOUNTY_BOSS_PORTAL_AUTOPING_DISTANCE : BOUNTY_LEVEL_AREA_AUTOPING_DISTANCE))
            {
                ScopedQuestDefinitionLockR pdefQuest(snoQuestSource);
                SNO snoWorld = pdefQuest->tBountyData.snoWorld;
                SQuestsRegisterBountyLevelArea(snoWorld, sno, snoQuestSource);
            }
        }
        else
        {









            DisplayInternalError(NULL, "", 2013, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        nRefs = 1;
    }
    else
    {
        nRefs++;
    }

    tSNOAutoPingRefMap.SetAt_Tracked(sno, nRefs);
}

void SNORemoveAutoPingRef(SNO sno)
{
    SNOAutoPingRefMap & tSNOAutoPingRefMap = *SGameManager::GameGet().ptSMiniMapGlobals->ptSNOAutoPingRefs;

    int32 nRefs = 0;

    if (tSNOAutoPingRefMap.Lookup(sno, nRefs) && nRefs != 0)
    {
        nRefs--;

        if (nRefs == 0)
        {





            SNOAutoPingDataList & tSNOAutoPingDataList = *SGameManager::GameGet().ptSMiniMapGlobals->ptSNOAutoPingData;
            POS pos = tSNOAutoPingDataList.GetHeadPosition();
            while (pos)
            {
                POS posCurrent = pos;
                SNOAutoPingData & tData = tSNOAutoPingDataList.GetNext(pos);
                if (tData.sno == sno)
                {
                    RemovePing(tData.idPing);
                    tSNOAutoPingDataList.RemoveAt(posCurrent);
                }
            }
        }
    }

    tSNOAutoPingRefMap.SetAt_Tracked(sno, nRefs);
}



inline void SNOAddTrackedPing(SNO sno, PingID idPing)
{
    SNOAutoPingData tData;
    tData.sno = sno;
    tData.idPing = idPing;

    SNOAutoPingDataList & tSNOAutoPingDataList = *SGameManager::GameGet().ptSMiniMapGlobals->ptSNOAutoPingData;
    tSNOAutoPingDataList.AddTail_Tracked(tData);
}























inline BOOL SNOHasAutoPing(SNO sno)
{
    SNOAutoPingRefMap & tSNOAutoPingRefMap = *SGameManager::GameGet().ptSMiniMapGlobals->ptSNOAutoPingRefs;

    int32 nRefs;
    if (tSNOAutoPingRefMap.Lookup(sno, nRefs))
    {
        return nRefs > 0;
    }

    return FALSE;
}




PingID SNOGetAutoPing(SNO sno)
{
    SNOAutoPingDataList & tSNOAutoPingDataList = *SGameManager::GameGet().ptSMiniMapGlobals->ptSNOAutoPingData;
    POS pos = tSNOAutoPingDataList.GetHeadPosition();
    while (pos)
    {
        SNOAutoPingData & tData = tSNOAutoPingDataList.GetNext(pos);
        if (tData.sno == sno)
        {
            return tData.idPing;
        }
    }

    return PINGID_INVALID;
}




BOOL IsValid(PingID idPing)
{
    MinimapMarkerInfo tInfo;
    return GetPingList()->Lookup(idPing, tInfo);
}

}  // namespace Minimap
