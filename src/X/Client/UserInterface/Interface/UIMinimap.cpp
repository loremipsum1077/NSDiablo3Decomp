// Original file: X/Client/UserInterface/Interface/UIMinimap.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "UIMinimap.h"

#include <float.h>

#include "../UI.h"
#include "../UIID.h"
#include "../UIGlobals.h"
#include "../../CGame.h"
#include "../../CPlayerInput.h"
#include "../../ImageCollection.h"
#include "../../Prefetch.h"
#include "../../Definitions/ShaderMapDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/ActorCommonDataSwig.h"
#include "../../Input/Input.h"
#include "../../Console/ControllerUtils.h"
#include "../../CPlayer.h"
#include "../../RActors/RActors.h"
#include "../../../Common/Gizmo.h"
#include "../../../Common/GizmoSwig.h"
#include "../../../Common/Player.h"
#include "../../../Common/GameConstants.h"
#include "../../../Common/TeamCommon.h"
#include "../../../Common/World.h"
#include "../../../Common/TagMaps/Tags.h"
#include "../../../Common/TagMaps/ActorTags.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Definitions/WorldDefinition.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/Act.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GameTemplate.h"
#include "../../../Sigma/MemoryPools.h"
#include "../../../Common/ActorCommon.h"
#include "../../../Common/ActorCommonData.h"
#include "../../Definitions/TextureDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../../Sigma/DataStructures/XFixedBlockAllocator.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../../BlizzardCore/Include/Blizzard/Debug.h"
typedef uint32 StringLabelHandle;  // X/Common/Labels.h

// Defined in X/Client/UserInterface/UIGlobals.cpp (lines 3961-3964).
extern UIID UIDLG_MINIMAP_PVE;
extern UIID UIMAP_MINIMAP_PVE;
extern UIID UIWND_MINIMAP_PVE_ARROW_REGION;
extern UIID UIDLG_MAP_LOCALMAP;
extern UIID UIWND_MINIMAP_TUTORIAL_ANCHOR;
// Declared in X/Client/UserInterface/Interface/UITutorials.h (not adopted yet); only the
// member this file calls.
class UITutorial;
#include "UITutorials.h"
// Sprite.cpp and UIUtils.cpp:304; the adopted Sprite.h/UIUtils.h lack these signatures.
void SpriteDrawDeferredRotated(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const Angle aYaw, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride);
void ConvertUICToPixels(const Rect2D & rectUIC, Rect2D & rectPixels, int nXOffset, int nYOffset);
extern UITutorialManager gtTutorialMgr;
// Declared in X/Client/UserInterface/Interface/UIQuests.h (not adopted yet).
namespace UIQuests {
void MakeCurrentObjectivesReappear();
}
// Declared in X/Common/Player.h (Player.cpp:1519).
Player * PlayerGetByANN(const ACDNetworkName ann);
// Declared in X/Client/UserInterface/Console/UIDeathMenu_Console.h (not adopted yet).
BOOL UIDeathMenu_Console_IsADeathScreenActive();
// Declared in X/Client/RActors/RActors.h (RActors.cpp) and X/Common/Quest.h.
SNO RActorGetLevelArea(RActorID idRActor);
BOOL QuestHasAdditionalLevelArea(SNO snoQuest, SNO snoLevelArea);
// Declared in X/Client/CPlayerInput.h (CPlayerInput.cpp:6603).
CursorMgr & PlayerGetCursorMgr();
// Declared in X/Client/UserInterface/UI.h (UI.cpp:5748).
UIMinimap & UI_GetMinimapFromID(const UIID & id);

enum {
    EXPLORED_SEEN_BIT = 0,
    EXPLORED_GHOST_BIT = 1,
};

struct MapSceneExploredGrid {  // size 0x100
    uint8 pBits[256];  // 0x00
};

typedef XFixedBlockAllocator<MapSceneExploredGrid, 1024> MapSceneExploredDataListPooled;
#include "../../Network/CTrickle.h"
// Defined in X/Client/UserInterface/UIGlobals.cpp (lines 9-10).
extern UIID UIDLG_MINIMAP_PVE_ANCHOR_SP;
extern UIID UIDLG_MINIMAP_PVE_ANCHOR_MP;
// Declared in X/Client/Network/CTrickle.h (CTrickle.cpp:309); not in the adopted header yet.
ClientTrickleEntry * CTrickleEntryFindForANN(const ACDNetworkName ann);
#include "../../../Common/Definitions/LevelAreaDefinition.h"
#include "../../../Common/SceneCommon.h"
// Declared in X/Common/Quests/Quests.h (Quests.cpp:3140); not in an adopted header yet.
uint32 QuestGetAutoPingTimeTicks(const SNO snoQuest);
#include "../../Sprite.h"

static const uint32 MINIMAP_SHOW_UPDATE_TIMEOUT = 600;

#include "../../../Common/Definitions/QuestRangeDefinition.h"
#include "../../../Common/TagMaps/MarkerTags.h"
static const LPCSTR cszPlayerMarkers[4] = {"MarkerFriendlyPlayer", "Marker_Player_Combat", "Marker_NPC_CombatElite", "Marker_NPC_TreasureGoblin"};
BOOL ACDIsPortal(const ActorCommonData &);
BOOL ACDIsLootRunBoss(const ActorCommonData &);
// DWARF X/Client/Map/Minimap.cpp:31.
namespace Minimap { ImageTextureFrame ResolveIconForRActor(const RActor *); }
BOOL WorldIsRotated(const SNO snoWorld);
static void sRevealMapAtLocation(const WorldPlace & tPlace, const float flRadius);  // defined below (DWARF line 5536)
// DWARF X/Common/SceneCommon.cpp:243 and Definitions/SceneDefinition.cpp:216.
static const LPCSTR cszPlayerGuideMarkers[4] = {"Marker_Guide", "Marker_Guide_Combat", "Marker_Guide_CombatElite", "Marker_Guide_TreasureGoblin"};
CRefString SceneDefinitionGetMinimapName(const SNO);
#include "../../../Common/Sno/SNOFiles.h"
// DWARF X/Client/UserInterface/UIUtils.cpp:347; X/Client/Sprite.cpp:2853.
void ConvertUICToPixelsDoNotSnap(const Vector2D &, Vector2D &);
void SpriteDrawDeferredMinimapTile(SNO, SNO, const Vector2D *, const RGBAColor &, const RGBAColor &, float, SNO, const Vector2D *, SNO, const Vector2D *);
// DWARF UIMinimap.cpp:3926; original internal helper is used by both real callers.
struct MapScene;
static void sCreateExploredInfo(MapScene &, const uint8 *, const BOOL &);
#include "../../../Sigma/GfxColorFormats.h"
#include "../../../Common/Powers/PowerAPISwig.h"
// DWARF X/Common/Player.cpp:2721/2744; declarations absent from adopted Player.h.
ACDID PlayerGetPet(ACDID idACDPlayer, int32 nIndex);
PetType PlayerGetPetType(ACDID idACDPlayer, int32 nIndex);
// DWARF X/Common/SceneCommon.cpp:243; declaration moved from line120 for original marker data.
void sSceneCalculate2DLogicalWorldBounds(Rect2D &, const SNO &, const PRTransform &);
// DWARF CPlayer.cpp:1442/1477 and GameCommonData.cpp:589.
BOOL CPlayerIsEngagedWithRare(const Player *);
BOOL CPlayerIsEngagedWithTreasureGoblin(const Player *);
BOOL GameCommonPlayerIsPartyGuide(const Player &);
// DWARF X/Client/Network/CTrickle.cpp:342.
ClientTrickleList & CTrickleGetList();
static const uint32 EXPLORED_BITS_WIDTH = 32;
static const uint32 EXPLORED_BITS_HEIGHT = 32;
static const uint32 EXPLORED_BITS_DEPTH = 2;
static const uint32 EXPLORED_BITS_BYTE_SIZE = 256;


static const uint32 MAX_EXPLORED_DATA_COUNT = 1024;






























struct MapScene {  // size 0x48
    SNO snoScene;  // 0x00
    SSceneID idSScene;  // 0x04
    SWorldID idSWorld;  // 0x08
    SNO snoMapTex;  // 0x0c
    Rect2D rectSceneWorld;  // 0x10
    SNO snoExploredTexture;  // 0x20
    MapSceneExploredGrid * pExploredData;  // 0x28
    int32 nExploredDataWidth;  // 0x30
    int32 nExploredDataHeight;  // 0x34
    uint32 dwLastDrawn;  // 0x38
    BOOL bRevealed;  // 0x3c
    BOOL fDirty;  // 0x40
};


















struct MarkerInfo {  // size 0x4c
    WorldPlace tPlace;  // 0x00
    ImageTextureFrame m_tTexture;  // 0x10
    StringLabelHandle m_hLabel;  // 0x18
    SNO m_snoStringList;  // 0x1c
    SNO m_snoKnownActorOverride;  // 0x20
    SNO m_snoQuestSource;  // 0x24
    BOOL m_bCanBecomeArrow;  // 0x28
    BOOL m_bRespectsFoW;  // 0x2c
    BOOL m_bIsPing;  // 0x30
    BOOL m_bIsKeyWarden;  // 0x34
    float m_fMaxDisplayRangeSq;  // 0x38
    float m_fMinDisplayRangeSq;  // 0x3c
    float m_fDiscoveryRangeSq;  // 0x40
    uint32 m_nPlayerUseFlags;  // 0x44
    float m_flDiscoveryTime;  // 0x48
};

typedef XPooledDataIDList MinimapMarkerList;

typedef XGrowableList<MapScene, const MapScene &> MapSceneList;

typedef XGrowableMap<unsigned int, unsigned int, MarkerInfo, const MarkerInfo &> MarkerInfoList;

const TextureFormat EXPLORED_TEXTURE_FORMAT = TEXTUREFORMAT_LIN_L8;












static const float ZOOM_APPROACH_RATE = 0.03f;



static const int32 NUM_OBJECTIVE_CONES = 3;
static const float OBJECTIVE_CONE_ANGLE = 45.0f;







enum LevelAreaState {
    LAS_INVALID = -1,
    LAS_HIDING = 0,
    LAS_SHOWING = 1,
    LAS_SHOWN = 2,
    LAS_NUM_STATES = 3,
};

enum MapMarkerState {
    MMS_NORMAL = 0,
    MMS_COMBAT = 1,
    MMS_COMBAT_WITH_ELITE = 2,
    MMS_TREASURE_GOBLIN = 3,
    MMS_NUM_STATES = 4,
};







struct ObjectiveCone {  // size 0x14
    WorldPlace tPlace;  // 0x00
    Angle aPlayerToTarget;  // 0x10
    ObjectiveCone()
    {
        tPlace.idSWorld = SWORLDID_INVALID;
        aPlayerToTarget = 0.0f;
    }
};





struct MinimapGlobals {  // size 0x40188
    MinimapGlobals()
        : ptListMarkers(NULL)
        , ptListServerMarkers(NULL)
        , ptListPingMarkers(NULL)
        , dwLastClearTick(0)
        , flAreaNameAlpha(1.0f)
        , snoLevelArea(SNO_NONE)
        , snoLevelAreaNext(SNO_NONE)
        , eLevelAreaState(LAS_INVALID)
        , eAct(ACT_INVALID)
        , fEnabled(TRUE)
        , fAnyDirty(FALSE)
        , dwDisabledLayers(0)
        , dwTraceLayers(0)
        , nCurrentHandicapLevel(0)
        , idMinimapDlg(GetUIIDNone())
        , idMinimap(GetUIIDNone())
        , idMinimapArrowRegion(GetUIIDNone())
        , nCurrentObjectiveCone(0)
    {
        alphaClearColor = 0xff;
        alphaSharedVisionClearColor = 0x73;
        nLastShownTime = 0;
    }
    MinimapMarkerList * ptListMarkers;  // 0x00
    MarkerInfoList * ptListServerMarkers;  // 0x08
    MarkerInfoList * ptListPingMarkers;  // 0x10
    uint32 dwLastClearTick;  // 0x18
    MapSceneList tListMapScenes;  // 0x20
    float flAreaNameAlpha;  // 0x78
    SNO snoLevelArea;  // 0x7c
    SNO snoLevelAreaNext;  // 0x80
    LevelAreaState eLevelAreaState;  // 0x84
    uint8 alphaClearColor;  // 0x88
    uint8 alphaSharedVisionClearColor;  // 0x89
    Act eAct;  // 0x8c
    BOOL fEnabled;  // 0x90
    BOOL fAnyDirty;  // 0x94
    uint16 dwDisabledLayers;  // 0x98
    uint16 dwTraceLayers;  // 0x9a
    int32 nCurrentHandicapLevel;  // 0x9c
    MapSceneExploredDataListPooled tMapSceneExploredDataList;  // 0xa0
    uint32 nLastShownTime;  // 0x400d8
    UIID idMinimapDlg;  // 0x400e0
    UIID idMinimap;  // 0x400e8
    UIID idMinimapArrowRegion;  // 0x400f0
    MarkerInfo tObjectiveMarker;  // 0x400f8
    ObjectiveCone arObjectiveCones[NUM_OBJECTIVE_CONES];  // 0x40144
    int32 nCurrentObjectiveCone;  // 0x40180
};
static_assert(sizeof(MinimapGlobals) == 0x40188, "MinimapGlobals");


























static MinimapGlobals * sgtMinimapGlobals;

BOOL MinimapShouldDisplay(const MarkerInfoList * ptListMarker, const UIMinimap & tMiniMap);
































































MapScene * sFindSceneByID(const SSceneID idSScene)
{

    if (idSScene == SSCENEID_INVALID)
        return NULL;
    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    POS pos = tListMapScenes.GetHeadPosition();
    while (pos)
    {
        MapScene & tMapScene = tListMapScenes.GetNext(pos);
        if (tMapScene.idSScene == idSScene)
            return &tMapScene;
    }

    return NULL;
}





MapScene * sFindScene(const WorldPlace & tPlace)
{

    SSceneID idSScene = SceneCommonGetSceneIDFromWorldPlace(tPlace, FALSE);
    MapScene * pRet = sFindSceneByID(idSScene);
    if (pRet == NULL)
    {
        MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
        POS pos = tListMapScenes.GetHeadPosition();
        while (pos)
        {
            MapScene & tMapScene = tListMapScenes.GetNext(pos);
            if (tMapScene.idSWorld == tPlace.idSWorld &&
                tMapScene.rectSceneWorld.ContainsPoint(tPlace.wp.As2D()))
            {
                pRet = &tMapScene;
                break;
            }
        }
    }
    return pRet;
}





inline void UIMinimap::sInit()
{
    sResetScrollOffset();

    m_bAlpha = 0xff;
    SetEatsClicks(EC_IGNORE);

    m_tNameDrawInfo.Initialize();
    m_dwLastTooltipTime = (uint32)-1;





    m_dwFlags = 0;
    m_flFeetPerPixel = m_flFeetPerPixelTarget = 1.6666666f;

    m_flZoomScaleFactor = 1.0f;





    m_idArrowRegion = GetUIIDNone();
    m_rectMap = cgrectNull;
    m_rectArrowRegion = cgrectNull;


    m_nPrevPingCount = 0;


    m_fRound = FALSE;
    m_nPingCount = 0;
    m_snoTextureMask = SNO_NONE;
}
















UIMinimap::UIMinimap(const WindowCreationParams & tParams, const Rect2D * pRect) :
    UIWindow(tParams, pRect)
{


    sInit();
}




inline void UIMinimap::sResetScrollOffset()
{


    m_pointScrollOffset.Set(0.0f, 0.0f);
    m_pointScrollOffsetPrev = m_pointScrollOffset;
}



inline void UIMinimap::sResetZoom()
{

    m_flZoomScaleFactor = 1.0f;
}




























void UIMinimap::sRecalculateRegions()
{

    m_rectMap = m_rect;

    if (!UI_TestID(m_idArrowRegion))
    {
        const uint32 dwInsetAmount = 20;

        m_rectArrowRegion = m_rectMap;
        m_rectArrowRegion.left += dwInsetAmount;
        m_rectArrowRegion.top += dwInsetAmount;
        m_rectArrowRegion.right -= dwInsetAmount;
        m_rectArrowRegion.bottom -= dwInsetAmount;
    }
    else
    {
        UI_GetWindowFromID(m_idArrowRegion).GetWindowRect(m_rectArrowRegion);
    }
}




void UIMinimap::Initialize(const BOOL fFullScreen, const UIID & idArrowRegion)
{










    m_flFeetPerPixel = m_flFeetPerPixelTarget = sGetZoomDistance();
    m_flZoomScaleFactor = 1.0f;
    if (fFullScreen)
        sResetScrollOffset();
    m_dwFlags = fFullScreen ? ((1 << UIM_DRAW_ARROWS_BIT) | (1 << UIM_ALLOW_SCROLLING_BIT)) : (1 << UIM_DRAW_ARROWS_BIT);
    m_fRound = !fFullScreen;

    m_snoTextureMask = GlobalSNOGet(m_fRound ? GLOBALSNO_MINIMAP_TEXTURE_SIDEBAR_MASK_ROUND : GLOBALSNO_MINIMAP_TEXTURE_SIDEBAR_MASK);

    m_idArrowRegion = idArrowRegion;

    m_bInputBleedOverCleared = FALSE;
}


























static void sGetTextureMaskUV(
    Rect2D & rectTextureMask,
    const Rect2D & rectDraw,
    const Rect2D & rectMapUIC)
{
    float wdWidth = rectMapUIC.Width();
    float wdHeight = rectMapUIC.Height();

    rectTextureMask.left = (rectDraw.left - rectMapUIC.left) / wdWidth;
    rectTextureMask.right = (rectDraw.right - rectMapUIC.left) / wdWidth;
    rectTextureMask.top = (rectDraw.top - rectMapUIC.top) / wdHeight;
    rectTextureMask.bottom = (rectDraw.bottom - rectMapUIC.top) / wdHeight;
}
























































































































































































BOOL UIMinimap::MarkerVisibleOnMinimap(const Vector2D & vMarkerPlace) const
{
    Vector2D vecMarkerUIC;
    sWorldToMapUIC(vMarkerPlace, vecMarkerUIC);

    return sMarkerIsInArrowRegion(vecMarkerUIC);
}








enum
{
    ARROW_IMAGE_TL,
    ARROW_IMAGE_TOP,
    ARROW_IMAGE_TR,
    ARROW_IMAGE_RIGHT,
    ARROW_IMAGE_BR,
    ARROW_IMAGE_BOTTOM,
    ARROW_IMAGE_BL,
    ARROW_IMAGE_LEFT,
    NUM_ARROW_IMAGES
};

LPCSTR cgszArrowImages[NUM_ARROW_IMAGES] =
{
    "Map_Arrow_UpperLeft",
    "Map_Arrow_Up",
    "Map_Arrow_UpperRight",
    "Map_Arrow_Right",
    "Map_Arrow_LowerRight",
    "Map_Arrow_Down",
    "Map_Arrow_LowerLeft",
    "Map_Arrow_Left",
};

LPCSTR cgszPartyGuideArrowImages[NUM_ARROW_IMAGES] =
{
    "Map_PartyArrow_UpperLeft",
    "Map_PartyArrow_Up",
    "Map_PartyArrow_UpperRight",
    "Map_PartyArrow_Right",
    "Map_PartyArrow_LowerRight",
    "Map_PartyArrow_Down",
    "Map_PartyArrow_LowerLeft",
    "Map_PartyArrow_Left",
};

LPCSTR cgszDestinationArrowImages[NUM_ARROW_IMAGES] =
{
    "Map_DestinationArrow_UpperLeft",
    "Map_DestinationArrow_Up",
    "Map_DestinationArrow_UpperRight",
    "Map_DestinationArrow_Right",
    "Map_DestinationArrow_LowerRight",
    "Map_DestinationArrow_Down",
    "Map_DestinationArrow_LowerLeft",
    "Map_DestinationArrow_Left",
};

LPCSTR cgszKeyWardenArrowImages[NUM_ARROW_IMAGES] =
{
    "Map_WardenArrow_UpperLeft",
    "Map_WardenArrow_Up",
    "Map_WardenArrow_UpperRight",
    "Map_WardenArrow_Right",
    "Map_WardenArrow_LowerRight",
    "Map_WardenArrow_Down",
    "Map_WardenArrow_LowerLeft",
    "Map_WardenArrow_Left",
};

static void sGetArrowTextureFrame(
    ImageTextureFrame & tImageTexFrameArrow,
    LPCSTR * pszArrowImages,
    const Vector2D & vecUIC,
    const Rect2D & rectArrowRegion)
{
    int nArrowImageIndex;
    if (vecUIC.x < rectArrowRegion.left)
    {
        if (vecUIC.y <= rectArrowRegion.top)
        {
            nArrowImageIndex = ARROW_IMAGE_TL;
        }
        else if (vecUIC.y >= rectArrowRegion.bottom)
        {
            nArrowImageIndex = ARROW_IMAGE_BL;
        }
        else
        {
            nArrowImageIndex = ARROW_IMAGE_LEFT;
        }
    }
    else if (vecUIC.x > rectArrowRegion.right)
    {
        if (vecUIC.y <= rectArrowRegion.top)
        {
            nArrowImageIndex = ARROW_IMAGE_TR;
        }
        else if (vecUIC.y >= rectArrowRegion.bottom)
        {
            nArrowImageIndex = ARROW_IMAGE_BR;
        }
        else
        {
            nArrowImageIndex = ARROW_IMAGE_RIGHT;
        }
    }
    else
    {

        if (vecUIC.y <= rectArrowRegion.top)
            nArrowImageIndex = ARROW_IMAGE_TOP;
        else
            nArrowImageIndex = ARROW_IMAGE_BOTTOM;
    }

    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, pszArrowImages[nArrowImageIndex], tImageTexFrameArrow);
}




inline BOOL UIMinimap::sMarkerIsInArrowRegion(const Vector2D & vecUIC) const
{
    if (m_fRound)
    {
        float flRadius;
        Vector2D vecCenter = m_rectArrowRegion.Center();
        flRadius = m_rectArrowRegion.Height() * 0.5f;
        if (Vector2D::DistanceSquared(vecUIC, vecCenter) > flRadius * flRadius)
            return FALSE;
        return TRUE;
    }










    return m_rectArrowRegion.ContainsPoint(vecUIC.x, vecUIC.y);
}




void UIMinimap::sDrawArrow(
    const Vector2D & vecUIC,
    const float flZ,
    const uint32 dwFlags,



    ACDID idACDMap,
    uint8 nNameAlpha)
{
    if (!(dwFlags & m_dwFlags & (1 << DMF_CAN_TURN_INTO_ARROW_BIT)))
        return;








    BOOL bHasBackground = FALSE;
    ImageTextureFrame tImageTexFrameBackground;

    float flArrowScale = 1.0f;
    LPCSTR * pszArrowImages;

    if (dwFlags & (1 << DMF_KEY_WARDEN))
    {
        pszArrowImages = cgszKeyWardenArrowImages;
    }
    else if (dwFlags & (1 << DMF_USE_PLAYER_ARROWS))
    {
        if (dwFlags & (1 << DMF_PARTY_GUIDE))
        {
            pszArrowImages = cgszPartyGuideArrowImages;
        }
        else
        {
            pszArrowImages = cgszArrowImages;
        }

        if (dwFlags & (1 << DMF_TREASURE_GOBLIN))
        {
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "Marker_Arrow_TreasureGoblin", tImageTexFrameBackground);
            bHasBackground = TRUE;
            flArrowScale = 0.8f;
        }

        else if (dwFlags & ((1 << DMF_COMBAT) | (1 << DMF_COMBAT_WITH_ELITE)))
        {
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "Marker_Arrow_CombatElite", tImageTexFrameBackground);
            bHasBackground = TRUE;
            flArrowScale = 0.8f;
        }
    }
    else
        pszArrowImages = cgszDestinationArrowImages;
    ImageTextureFrame tImageTexFrameArrow;
    Vector2D vecPointToUseUIC;
    Angle aRot = 0.0f;


    if (m_fRound)
    {

        ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, pszArrowImages[ARROW_IMAGE_RIGHT], tImageTexFrameArrow);

        Vector2D vecCenter = m_rectArrowRegion.Center();
        float flRadius = m_rectArrowRegion.Height() * 0.5f;
        Vector2D vecFromCenter = vecUIC - vecCenter;

        vecFromCenter.Normalize();
        vecPointToUseUIC = vecCenter + vecFromCenter * flRadius;

        aRot = Math::ATan(vecFromCenter.y, vecFromCenter.x);
    }
    else
    {
        sGetArrowTextureFrame(tImageTexFrameArrow, pszArrowImages, vecUIC, m_rectArrowRegion);

        vecPointToUseUIC.x = Math::Pin(vecUIC.x, m_rectArrowRegion.left, m_rectArrowRegion.right);
        vecPointToUseUIC.y = Math::Pin(vecUIC.y, m_rectArrowRegion.top, m_rectArrowRegion.bottom);
    }


    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrameArrow.snoTexture), SNOFilesAsyncPriority_High))
        return;



    const float DEFAULT_ICON_SIZE = 38.4f;
    Rect2D rectUIC;
    rectUIC.left = vecPointToUseUIC.x - DEFAULT_ICON_SIZE * 0.5f;
    rectUIC.top = vecPointToUseUIC.y - DEFAULT_ICON_SIZE * 0.5f;
    rectUIC.right = rectUIC.left + DEFAULT_ICON_SIZE;
    rectUIC.bottom = rectUIC.top + DEFAULT_ICON_SIZE;

    Rect2D rectPixel;
    ConvertUICToPixels(rectUIC, rectPixel, 0, 0);

    float flOffset = UIDC_DIALOG_Z_INCREMENT / 3.0f;
    float flCurZ = Math::Max(flZ, flOffset * 2.0f);


    flCurZ -= flOffset;

    if (bHasBackground)
    {
        SpriteDrawDeferred(tImageTexFrameBackground, SNO_NONE, rectPixel, crgbaWhite, crgbaBlack, flCurZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
        flCurZ -= flOffset;
    }

    Rect2D rectArrow = rectPixel;
    if (!Math::FloatEqual(flArrowScale, 1.0f))
    {
        Vector2D vHalfSize = rectArrow.Size() * 0.5f * (flArrowScale - 1.0f);
        rectArrow.left -= vHalfSize.x;
        rectArrow.top -= vHalfSize.y;
        rectArrow.right += vHalfSize.x;
        rectArrow.bottom += vHalfSize.y;
    }

    if (m_fRound)
    {
        SpriteDrawDeferredRotated(tImageTexFrameArrow, SNO_NONE, rectArrow, crgbaWhite, crgbaBlack, flCurZ, NULL, aRot, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID);
    }
    else
    {
        SpriteDrawDeferred(tImageTexFrameArrow, SNO_NONE, rectArrow, crgbaWhite, crgbaBlack, flCurZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
    }




}




void UIMinimap::sDrawCone(
    Vector2D vecUIC,
    Angle aRot,
    const float flZ )
{
    ImageTextureFrame tImageTexFrameBackground;
    (void)tImageTexFrameBackground;
    float flConeScale = 3.0f;
    const float CONE_IMAGE_SIZE = 100.8f * flConeScale;
    ImageTextureFrame tImageTexFrameArrow;


    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, "PH_cone", tImageTexFrameArrow);


    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrameArrow.snoTexture), SNOFilesAsyncPriority_High))
        return;







    vecUIC.x += Math::Cos(aRot) * CONE_IMAGE_SIZE * 0.5f * flConeScale;
    vecUIC.y += Math::Sin(aRot) * CONE_IMAGE_SIZE * 0.5f * flConeScale;

    Rect2D rectUIC;
    rectUIC.left = vecUIC.x - CONE_IMAGE_SIZE * 0.5f;
    rectUIC.top = vecUIC.y - CONE_IMAGE_SIZE * 0.5f;
    rectUIC.right = rectUIC.left + CONE_IMAGE_SIZE;
    rectUIC.bottom = rectUIC.top + CONE_IMAGE_SIZE;

    Rect2D rectPixel;
    ConvertUICToPixels(rectUIC, rectPixel, 0, 0);

    float flOffset = UIDC_DIALOG_Z_INCREMENT / flConeScale;
    float flCurZ = Math::Max(flZ, flOffset * 2.0f);


    flCurZ -= flOffset;

    Rect2D rectArrow = rectPixel;
    {

        Vector2D vHalfSize = rectArrow.Size() * 0.5f * (flConeScale - 1.0f);
        rectArrow.left -= vHalfSize.x;
        rectArrow.top -= vHalfSize.y;
        rectArrow.right += vHalfSize.x;
        rectArrow.bottom += vHalfSize.y;
    }


    aRot += PI * 0.5f;
    SpriteDrawDeferredRotated(tImageTexFrameArrow, SNO_NONE, rectArrow, crgbaWhite, crgbaBlack, flCurZ, NULL, aRot, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID);
}



void UIMinimap::sDrawMarker(
    const SNO snoShaderMarker,
    const ImageTextureFrame & tImageTexFrame,
    const Vector2D & vecUIC,
    const float flZ,
    const uint32 dwFlags,



    ACDID idACDMap,
    uint8 nNameAlpha,
    float flYawOffset)
{
    if (tImageTexFrame.snoTexture == SNO_NONE)
        return;




    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), SNOFilesAsyncPriority_High))
        return;



    BOOL fIsDestination = tImageTexFrame.snoTexture == GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_DEST_ICON) || tImageTexFrame.snoTexture == GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_QUEST_RING);
    if (fIsDestination && !Prefs::GetGameplayPrefs().GetShowBreadCrumb() && !(dwFlags & (1 << DMF_FORCE_DEST_PING)))
        return;



    if (!sMarkerIsInArrowRegion(vecUIC))
    {
        sDrawArrow(vecUIC, flZ, dwFlags, idACDMap, nNameAlpha);
    }
    else
    {




        RGBAColor rgbaColor = crgbaWhite;
        const float DEFAULT_ICON_SIZE = 38.4f;





        float flScale = 1.0f;
        uint8 bAlpha = 0xff;
        if (fIsDestination)
        {
            m_nPingCount++;

            const uint32 dwCycleDuration = TICKS_PER_SECOND;
            float flFrac = (float)(CGameGetRealTicks() % dwCycleDuration) / (float)dwCycleDuration;

            flScale = Math::Lerp(0.0f, 1.5f, flFrac);
            bAlpha = (uint8)(Math::Lerp(1.0f, 0.0f, flFrac) * 255.0f);
        }

        const float flIconSize = flScale * DEFAULT_ICON_SIZE;
        uint8 bAlphaToUse = MultiplyAlpha(bAlpha, m_bAlpha);

        rgbaColor.a = bAlphaToUse;

        Rect2D rectUIC;
        rectUIC.left = vecUIC.x - flIconSize * 0.5f;
        rectUIC.top = vecUIC.y - flIconSize * 0.5f;
        rectUIC.right = rectUIC.left + flIconSize;
        rectUIC.bottom = rectUIC.top + flIconSize;

        Rect2D * pRectTextureMask = NULL;
        Rect2D rectTextureMask;
        if (m_snoTextureMask != SNO_NONE)
        {

            sGetTextureMaskUV(rectTextureMask, rectUIC, m_rectMap);
            pRectTextureMask = &rectTextureMask;
        }




        RActor * ptRActor = NULL;

        if ((dwFlags & (1 << DMF_USE_ACTOR_ORIENTATION_BIT)) && idACDMap != ACDID_INVALID)
        {
            RActorID idRActor = (RActorID)ACDGetOwner(idACDMap);
            ptRActor = RActorTryToGet(idRActor);
        }

        Rect2D rectPixel;
        ConvertUICToPixels(rectUIC, rectPixel, 0, 0);

        if (ptRActor)
        {

            float flModifiedYaw = -(ptRActor->aYaw + flYawOffset);
            SpriteDrawDeferredRotated(tImageTexFrame, snoShaderMarker, rectPixel, rgbaColor, crgbaBlack, flZ, NULL, flModifiedYaw, m_snoTextureMask, pRectTextureMask, FALSE, RENDERLAYER_INVALID);
        }
        else
        {










            SpriteDrawDeferred(tImageTexFrame, snoShaderMarker, rectPixel, rgbaColor, crgbaBlack, flZ, NULL, m_snoTextureMask, pRectTextureMask, FALSE, RENDERLAYER_INVALID, FALSE);
        }
    }




}



BOOL sShouldHide(
    const ActorCommonData & tACD,
    const RActor * ptRActor)
{
    BOOL bRet = FALSE;

    if (!((!(ptRActor->dwFlags & (1 << RACTOR_HIDE_BIT)) && tACD.IsEnabled())
                || ACDIsPortal(tACD)))
    {
        // Inference: hidden path returns true; original caller skips the marker on this path.
        bRet = TRUE;
    }
    return bRet;
}

inline void UIMinimap::sWorldToMapUIC(const Vector2D & pointWorld, Vector2D & pointUIC) const
{


    Vector2D vec = pointWorld * m_mWorldToUIC;

    pointUIC.Set(vec.x + m_rectMap.left + m_rectMap.Width() * 0.5f,
                 vec.y + m_rectMap.top + m_rectMap.Height() * 0.5f);
}



// NON_MATCHING: 312 vs 316; emitted through the real caller (sUpdateSceneVisibility) our build argument-promotes tMapScene (callers load pExploredData/nExploredDataWidth/nExploredDataHeight and pass them) while the original keeps the MapScene & and only drops the 3 constant-0 floats; with promotion blocked by a probe the body was 3 insns off (sign tests hoisted together; pTextureRow add operand order). Tried: 0.0f call arguments (DWARF const_value 0 on the float params), condition forms listed in build/try/draft/UIMinimap.txt
static void sClearVisibilityLine(
    MapScene & tMapScene,
    Pixel8 * pTextureSurface,
    uint32 dwSurfacePitch,
    int32 nLineStartX,
    int32 nLineEndX,
    int32 nLineY,

    uint32 dwFlags,



    float flCenterCircleX,
    float flCenterCircleY,
    float flRadius)
{


    if (nLineY == -1)
        nLineY = 0;


    if (nLineY == tMapScene.nExploredDataHeight)

        nLineY = tMapScene.nExploredDataHeight - 1;

    if (nLineStartX == tMapScene.nExploredDataWidth)

        nLineStartX = tMapScene.nExploredDataWidth - 1;

    if (nLineEndX == -1)
        nLineEndX = 0;




    if (nLineY < 0 || nLineY >= tMapScene.nExploredDataHeight)
        return;
    if (nLineEndX < 0 || nLineStartX >= tMapScene.nExploredDataWidth)
        return;





    int32 nLineStartXClipped;
    float flCircleX;
    int32 nLineEndXClipped;
    if (nLineStartX >= 0) nLineStartXClipped = nLineStartX; else nLineStartXClipped = 0;




    nLineEndXClipped = Math::Min(nLineEndX, tMapScene.nExploredDataWidth - 1);

    Pixel8 * pTextureRow = NULL;
    if (pTextureSurface)

        pTextureRow = pTextureSurface + nLineY * dwSurfacePitch + nLineStartXClipped;



    uint8 * pDataRow = tMapScene.pExploredData->pBits + (nLineY * tMapScene.nExploredDataWidth * EXPLORED_BITS_DEPTH) / 8;




    int32 nWidthToClear = nLineEndXClipped - nLineStartXClipped;

    Pixel8 dwColor;


















    for (int32 x = 0; x <= nWidthToClear; x++)
    {


        uint32 dwOffset = (nLineStartXClipped + x) * EXPLORED_BITS_DEPTH;
        uint32 dwByte = dwOffset / 8;
        uint32 dwFraction = 8 - EXPLORED_BITS_DEPTH - (dwOffset - dwByte * 8);


        pDataRow[dwByte] |= dwFlags << dwFraction;
        if (pTextureRow)
        {
            uint32 dwCurFlags = (pDataRow[dwByte] >> dwFraction) & 3;
            if (dwCurFlags & (1 << EXPLORED_SEEN_BIT))
                dwColor = 0xff;
            else
                dwColor = (dwCurFlags & (1 << EXPLORED_GHOST_BIT)) << 6;
            pTextureRow[x] = dwColor;
        }
    }






































}
static void sFreeMapSceneData(MapScene & tMapScene);  // defined below (DWARF line 4665)
void * TextureDefinitionLockTexture(const SNO sno, uint32 & dwPitch, uint32 & dwHeight, const uint32 dwMipLevel, const uint32 dwFlags);  // X/Client/Definitions/TextureDefinition.cpp:1081
void TextureDefinitionUnlockTexture(const SNO sno, void * lpBuf, const uint32 dwMipLevel);  // X/Client/Definitions/TextureDefinition.cpp:1241
static Pixel8 * sLockTexture(
    SNO snoTexture,
    uint32 & dwPitch,
    uint32 & dwHeight)
{



    if (!gptGroupDefTexture->WasDynamicallyCreated(snoTexture))
    {

        SigmaMemoryOverflowCheck();
        if (Blizzard::Debug::AmIBeingDebugged())
        {
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }


    return (Pixel8 *)TextureDefinitionLockTexture(snoTexture, dwPitch, dwHeight, 0, 0);
}

























































static void sClearVisibilityCircle(
    MapScene & tMapScene,
    float flCenterX,
    float flCenterY,
    float flRadius,

    uint32 dwFlags)
{



    int32 nCenterX = Math::FloatToInt(flCenterX);
    int32 nCenterY = Math::FloatToInt(flCenterY);
    int32 nRadius = Math::FloatToInt(flRadius);


    if (nRadius == 0)
        return;




    int32 nX = 0;
    int32 nY = nRadius;
    int32 nD = 3 - 2 * nRadius;


    uint32 dwPitch = 0;
    Pixel8 * pTextureSurface = NULL;

    if (tMapScene.snoExploredTexture != SNO_NONE)
    {
        uint32 dwHeight = 0;
        pTextureSurface = sLockTexture(tMapScene.snoExploredTexture, dwPitch, dwHeight);
    }




    int32 nLineStartX;
    int32 nLineEndX;
    int32 nLineY;
    while (nX <= nY)
    {
        // the 0.0f arguments: DWARF gives sClearVisibilityLine's flCenterCircleX/flCenterCircleY/flRadius const_value 0 (every call passes 0)

        nLineStartX = nCenterX - nX;
        nLineEndX = nCenterX + nX; nLineY = nCenterY + nY;
        sClearVisibilityLine(tMapScene, pTextureSurface, dwPitch, nLineStartX, nLineEndX, nLineY, dwFlags, 0.0f, 0.0f, 0.0f);

















        nLineY = nCenterY - nY;
        sClearVisibilityLine(tMapScene, pTextureSurface, dwPitch, nLineStartX, nLineEndX, nLineY, dwFlags, 0.0f, 0.0f, 0.0f);















        nLineStartX = nCenterX - nY;
        nLineEndX = nCenterX + nY;
        nLineY = nCenterY + nX;
        sClearVisibilityLine(tMapScene, pTextureSurface, dwPitch, nLineStartX, nLineEndX, nLineY, dwFlags, 0.0f, 0.0f, 0.0f);


















        nLineY = nCenterY - nX;
        sClearVisibilityLine(tMapScene, pTextureSurface, dwPitch, nLineStartX, nLineEndX, nLineY, dwFlags, 0.0f, 0.0f, 0.0f);













        if (nD < 0)
        {

            nD += 4 * nX + 6;
        }
        else
        {
            nD += 4 * (nX - nY) + 10;
            nY--;
        }
        nX++;
    }



    if (pTextureSurface)

        TextureDefinitionUnlockTexture(tMapScene.snoExploredTexture, pTextureSurface, 0);
}












static uint32 sGetFlagsFromLocation(
    const MapScene & tMapScene,
    IVector2D & pointPixel)
{

    uint32 dwOffset = (pointPixel.x * tMapScene.nExploredDataWidth + pointPixel.y) * EXPLORED_BITS_DEPTH;
    uint32 dwByte = dwOffset / 8;
    uint32 dwFraction = 8 - EXPLORED_BITS_DEPTH - (dwOffset - dwByte * 8);
    uint8 dwExploredByte = tMapScene.pExploredData->pBits[dwByte];
    return dwExploredByte >> dwFraction;
}



















static void sWorldToVisibilityPixel(
    const MapScene & tMapScene,
    const Vector3D & wpPos,
    IVector2D & pointPixel)
{
    float xDelta = wpPos.x - tMapScene.rectSceneWorld.left;
    float xRatio = xDelta / (tMapScene.rectSceneWorld.right - tMapScene.rectSceneWorld.left);
    pointPixel.x = Math::FloatToInt(xRatio * tMapScene.nExploredDataHeight);

    float yDelta = wpPos.y - tMapScene.rectSceneWorld.top;
    float yRatio = yDelta / (tMapScene.rectSceneWorld.bottom - tMapScene.rectSceneWorld.top);
    pointPixel.y = Math::FloatToInt(yRatio * tMapScene.nExploredDataWidth);
}



static void sWorldToVisibilityPixelFloat(
    const MapScene & tMapScene,
    float wdX,
    float wdY,
    Vector2D & pointPixel)
{

    float xDelta = wdX - tMapScene.rectSceneWorld.left;
    float xRatio = xDelta / (tMapScene.rectSceneWorld.right - tMapScene.rectSceneWorld.left);
    pointPixel.x = xRatio * tMapScene.nExploredDataHeight;

    float yDelta = wdY - tMapScene.rectSceneWorld.top;
    float yRatio = yDelta / (tMapScene.rectSceneWorld.bottom - tMapScene.rectSceneWorld.top);
    pointPixel.y = yRatio * tMapScene.nExploredDataWidth;
}




static float sGetVisitedRatio(
    MapScene & tMapScene)
{
    int32 nExploredDataWidth = tMapScene.nExploredDataWidth;
    int32 nExploredDataHeight = tMapScene.nExploredDataHeight;
    int32 nSeenCount = 0;


    MapSceneExploredGrid * ptGrid = tMapScene.pExploredData;




    for (int32 y = 0; y < nExploredDataHeight; y++)
    {
        for (int32 x = 0; x < nExploredDataWidth; x++)
        {

            uint32 dwOffset = (x + y * nExploredDataWidth) * EXPLORED_BITS_DEPTH;
            uint32 dwByte = dwOffset / 8;
            uint32 dwFraction = 8 - EXPLORED_BITS_DEPTH - (dwOffset - dwByte * 8);
            uint8 dwExploredByte = ptGrid->pBits[dwByte];
            uint32 dwFlags = dwExploredByte >> dwFraction;
            if (dwFlags & (1 << EXPLORED_SEEN_BIT))
                nSeenCount++;
        }
    }









    return (float)nSeenCount / ((float)nExploredDataWidth * (float)nExploredDataHeight);
}


// NON_MATCHING: 1188 vs 1176; control flow and call order agree, but the inlined sClearVisibilityCircle passes argument-promoted MapScene fields to sClearVisibilityLine (see the note on that function) instead of the MapScene &, which changes register allocation and the frame (0xd0 vs 0xc0)
static void sUpdateSceneVisibility(
    MapScene & tMapScene,
    const Vector2D & wpLocClear,

    uint32 dwFlags,



    const float flRevealRadius)
{
    if (tMapScene.bRevealed)
        return;




    if (tMapScene.pExploredData == NULL)
        return;




    Rect2D rectSight;

    rectSight.left = wpLocClear.x - flRevealRadius;
    rectSight.top = wpLocClear.y - flRevealRadius;
    rectSight.right = wpLocClear.x + flRevealRadius;
    rectSight.bottom = wpLocClear.y + flRevealRadius;




    Rect2D rectSightEnlarged;
    float flWidthMargin = (tMapScene.rectSceneWorld.right - tMapScene.rectSceneWorld.left) * 0.03125f;
    float flHeightMargin = (tMapScene.rectSceneWorld.bottom - tMapScene.rectSceneWorld.top) * 0.03125f;
    rectSightEnlarged.Set(
        rectSight.left - flWidthMargin,
        rectSight.top - flHeightMargin,
        rectSight.right + flWidthMargin,
        rectSight.bottom + flHeightMargin);
    if (!Rect2D::Intersect(tMapScene.rectSceneWorld, rectSightEnlarged, NULL))
        return;










    Vector2D pointPlayerTexture;
    sWorldToVisibilityPixelFloat(tMapScene, wpLocClear.x, wpLocClear.y, pointPlayerTexture);


    Vector2D pointSightEdgeTexture;
    sWorldToVisibilityPixelFloat(tMapScene, rectSight.left, wpLocClear.y, pointSightEdgeTexture);



    float flSightRadiusPixel = Math::Abs(pointSightEdgeTexture.x - pointPlayerTexture.x);


    sClearVisibilityCircle(tMapScene, pointPlayerTexture.y, pointPlayerTexture.x, flSightRadiusPixel, dwFlags);










    if (sGetVisitedRatio(tMapScene) == 1.0f)
    {
        tMapScene.bRevealed = TRUE;
        sFreeMapSceneData(tMapScene);
    }
}



static MapScene * sFindMapSceneByServerSceneID(const SSceneID idSScene)
{




    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    POS pos = tListMapScenes.GetHeadPosition();
    while (pos)
    {
        MapScene & tMapScene = tListMapScenes.GetNext(pos);
        if (tMapScene.idSScene == idSScene)
        {
            return &tMapScene;
        }
    }

    return NULL;
}











static MapScene * sFindRActorMapScene(const RActor * ptRActor)
{
    SSceneID idSScene = ptRActor->idSScene;
    if (idSScene == SSCENEID_INVALID)
        return NULL;

    return sFindMapSceneByServerSceneID(idSScene);
}


static BOOL sPositionInExploredArea(
    const Vector3D & wpPos,
    const MapScene * ptMapScene)
{
    if (ptMapScene == NULL)
        return FALSE;




    if (ptMapScene->pExploredData == NULL)
        return TRUE;




    IVector2D pointPixel;
    sWorldToVisibilityPixel(*ptMapScene, wpPos, pointPixel);


    if (pointPixel.x < 0 || pointPixel.y < 0 || pointPixel.y >= ptMapScene->nExploredDataWidth || pointPixel.x >= ptMapScene->nExploredDataHeight)
        return FALSE;




    BOOL bVisible = FALSE;



    uint32 dwFlags = sGetFlagsFromLocation(*ptMapScene, pointPixel);
    if (dwFlags & ((1 << EXPLORED_SEEN_BIT) | (1 << EXPLORED_GHOST_BIT)))
        bVisible = TRUE;

    return bVisible;
}















static void sBlitPixel32(
    Pixel8 * pDst,
    const uint32 dwWidth,
    const uint32 dwHeight,
    const uint32 dwPitch,

    const MapSceneExploredGrid * pGrid)
{




    uint32 mod = dwPitch - dwWidth;


    const uint8 *pSrc = pGrid->pBits;


    for (uint32 y = 0; y < dwHeight; ++y)
    {
        for (uint32 x = 0; x < dwWidth; ++x)
        {

            uint32 dwOffset = (x + y * dwWidth) * EXPLORED_BITS_DEPTH;
            uint32 dwByte = dwOffset / 8;
            uint32 dwFraction = 8 - EXPLORED_BITS_DEPTH - (dwOffset - dwByte * 8);
            uint8 dwExploredByte = pSrc[dwByte];
            uint32 dwFlags = dwExploredByte >> dwFraction;
            if (dwFlags & (1 << EXPLORED_SEEN_BIT))
                // Inference: the original pixel alpha is255 when seen,128 when ghosted, otherwise0.
                *pDst = 0xff;
            else // Inference: original ghost alpha is128, otherwise0.
                *pDst = (dwFlags & (1 << EXPLORED_GHOST_BIT)) ? 0x80 : 0;
            ++pDst;
        }









        pDst += mod;
    }
}



static void sCreateExploredTexture(
    MapScene & tMapScene)
{







    if (tMapScene.pExploredData == NULL)
    {
        tMapScene.snoExploredTexture = GlobalSNOGet(GLOBALSNO_OPAQUE_WHITE_TEXTURE);
        return;
    }



    TextureFormat eExploredTextureFormat = EXPLORED_TEXTURE_FORMAT;
    tMapScene.snoExploredTexture = TextureDefinitionCreateTexture("ExploredMinimap", EXPLORED_TEXTURE_FORMAT, eExploredTextureFormat, tMapScene.nExploredDataWidth, tMapScene.nExploredDataHeight, RESOURCEUSAGE_STATIC, 0, 1, 0, 0);






    uint32 dwPitch = 0;
    uint32 dwHeight = 0;
    Pixel8 *pSurface = sLockTexture(tMapScene.snoExploredTexture, dwPitch, dwHeight);







    sBlitPixel32(pSurface, tMapScene.nExploredDataWidth, tMapScene.nExploredDataHeight,
        dwPitch, tMapScene.pExploredData);


    TextureDefinitionUnlockTexture(tMapScene.snoExploredTexture, pSurface, 0);
}



static void sDestroyExploredTexture(
    MapScene & tMapScene,
    const BOOL fPurge)
{


    if (tMapScene.snoExploredTexture != SNO_NONE)
    {






        if (fPurge && tMapScene.snoExploredTexture != GlobalSNOGet(GLOBALSNO_OPAQUE_WHITE_TEXTURE))
        {
            gptGroupDefTexture->Purge(tMapScene.snoExploredTexture, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
        }
        tMapScene.snoExploredTexture = SNO_NONE;
    }
}


static const RActor * sFindPetRActor(
    ACDID idACDPet)
{
    ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
    // Inference: the missing-ACD branch merges with the dead-pet return.
    if (!ptACDPet ||
        !ptACDPet->IsAlive())
        return NULL;


    // Inference: client ACD ownership is an RActorID (also used by UIMinimap_EnterKnown).
    RActorID idRActor = (RActorID)ACDGetOwner(idACDPet);
    return RActorTryToGet(idRActor);
}




void UIMinimap::sDrawMapMarkers(float & flZSlice)
{




    m_nPingCount = 0;

    SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();
    float flZ;





    if (!GameTypeIsPvP())
    {
        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawKnownListMarkers(flZ, idSWorldCurrent);

        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawTrickleMarkers(flZ, idSWorldCurrent);

        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawPetMarkers(flZ);

        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawServerMarkers(flZ, idSWorldCurrent);

        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawPlayerMarkers(flZ, idSWorldCurrent);
        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawLocalPlayerMarker(flZ);
    }
    else
    {
        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawKnownListMarkers(flZ, idSWorldCurrent);

        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawTrickleMarkers(flZ, idSWorldCurrent);

        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawPlayerMarkers(flZ, idSWorldCurrent);
        flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;
        DrawLocalPlayerMarker(flZ);
    }




    flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice; flZSlice += 1.0f;







    if (m_nPingCount > 0 && m_nPrevPingCount == 0)
    {
        UIQuests::MakeCurrentObjectivesReappear();
    }
    m_nPrevPingCount = m_nPingCount;
}


void ConvertUICToPixels(const Vector2D & vecUIC, Vector2D & vecPixels, int nXOffset, int nYOffset);


void UIMinimap::sDrawMapObjectiveCones(
    float & flZSlice )
{

    if (sgtMinimapGlobals->tObjectiveMarker.tPlace.idSWorld == SWORLDID_INVALID)
        return;



    MarkerInfo & tObjectiveMarker = sgtMinimapGlobals->tObjectiveMarker;


    SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();
    if (tObjectiveMarker.tPlace.idSWorld != idSWorldCurrent)
        return;




    Vector2D vecObjectiveUIC;
    sWorldToMapUIC(tObjectiveMarker.tPlace.wp.As2D(), vecObjectiveUIC);


    for (int32 nIndex = 0; nIndex < NUM_OBJECTIVE_CONES; ++nIndex)
    {
        ObjectiveCone tCone = sgtMinimapGlobals->arObjectiveCones[nIndex];


        if (tCone.tPlace.idSWorld != idSWorldCurrent)
            continue;



        Vector2D vecConeUIC;
        sWorldToMapUIC(tCone.tPlace.wp.As2D(), vecConeUIC);

        Vector2D vecConePixels;
        ConvertUICToPixels(vecConeUIC, vecConePixels, 0, 0);

        float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
        flZSlice += 1.0f;

        sDrawCone(vecConeUIC, tCone.aPlayerToTarget, flZ);
    }
}


BOOL sIsQuestActor(
    const ActorCommonData * ptACD)
{
    return (ptACD->dwFlags & (1 << ACD_QUEST_OBJECTIVE)) ||
           ptACD->AttributesGetInt(ATTRIBUTE_QUEST_MONSTER);
}


BOOL sHasConversation(
    const ActorCommonData * ptACD,
    int32 nPlayerIndex)
{
    return ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_CONVERSATION_ICON, nPlayerIndex)) > 0;
}

// LINKER-REMOVED: DWARF size 0x8c; address-0 rows show a 2-instruction test at 2307:9 before the lock (folded where the caller already tests snoActor != SNO_NONE); inferred `snoActor == SNO_NONE` early return.
SNO sGetPVPIcon(
    TrickleInfoType eType,
    SNO snoActor)
{



    if (snoActor == SNO_NONE)
        return SNO_NONE;



    ScopedActorDefinitionLockR pdefActor(snoActor);

    switch (eType)
    {
    case TRICKLE_INFO_PVP_FRIENDLY:
        return TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON_PVP_FRIENDLY);
    case TRICKLE_INFO_PVP_HOSTILE:
        return TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON_PVP_HOSTILE);
    case TRICKLE_INFO_PVP_NEUTRAL:
        return TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON);
    }
    // Inference: default switch path carries SNO_NONE through the unlock.
    return SNO_NONE;
}


SNO sGetPVPIconForACD(const ActorCommonData & tACD)
{
    Team eTeam;
    ScopedActorDefinitionLockR pdefActor(tACD.GetActorSNO());
    eTeam = tACD.GetTeam();

    if (eTeam == TEAM_COLOR_1)
    {
        return TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON);
    }
    else if (TeamsAreFriendly(eTeam, LocalPlayerGetPrimary()->eTeam))
    {
        return TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON_PVP_FRIENDLY);
    }
    else
    {
        return TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_ICON_PVP_HOSTILE);
    }
}

// NON_MATCHING:1932 vs1932 bytes,22 words differ in existing visibility/world-to-UI helper scheduling; tested distance/comparison operand order and arrow-veto branches.
void UIMinimap::DrawKnownListMarkers(
    float & flZ,
    SWorldID idSWorldCurrent)
{
    if (sgtMinimapGlobals->dwDisabledLayers & (1 << Minimap::LAYER_KNOWN_LIST))
        return;




    SNO snoShaderMarker = ShaderMapGetDefaultShader( GlobalSNOGet(GLOBALSNO_MINIMAP_SHADERMAP_MARKER), TRUE, NULL);

    const ActorCommonData *ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet()); // Inference: failed local-player lookup joins the shared return.
    if (!ptACDLocalPlayer)
        return;



    Vector3D wpPlayer = ptACDLocalPlayer->GetPlace().wp;

    int32 nPlayerIndex = LocalPlayerGet()->nPlayerIndex;

    POS posMarker = sgtMinimapGlobals->ptListMarkers->GetHeadPosition();
    while (posMarker)
    {
        ACDNetworkName ANN = (ACDNetworkName)sgtMinimapGlobals->ptListMarkers->GetNext(posMarker);
        ACDID idACD = ACDFromANN(ANN);
        const ActorCommonData *ptACD = ACDTryToGet(idACD);
        // Inference: original missing-ACD path rejoins the loop without a distinct line row.
        if (!ptACD)
            continue;



        if (ptACD->GetLocationType() != AL_WORLD)
            continue;


















        WorldPlace tPlaceACD = ptACD->GetPlace();
        if (tPlaceACD.idSWorld != idSWorldCurrent)
            continue;



        if (ptACD->AttributesGetInt(ATTRIBUTE_MINIMAP_ICON_ACTIVE) == 0 && !sIsQuestActor(ptACD) && !sHasConversation(ptACD, nPlayerIndex))
            continue;



        if (ACDIsPlayer(ptACD->id) || ptACD->IsPet())
            continue;



        ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
        SNO snoQuestRange = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_MINIMAP_QUEST_RANGE);
        if (!QuestRangeEvaluate(snoQuestRange, NULL))
            continue;


        // Inference: client idOwner is an RActorID, as at EnterKnown5102 and original offset0x14.
        const RActor *ptRActor = RActorTryToGet((RActorID)ptACD->idOwner);
        // Inference: original null render-actor check joins the definition-unlock tail.
        if (!ptRActor)
            continue;


        if (sShouldHide(*ptACD, ptRActor))
            continue;



        float flDisplayRangeMin = ActorGetInheritedTagMapValueFloat(ptRActor->snoActor, ptRActor->tMinimapInfo, TAG_ACTOR_MINIMAP_MIN_RANGE, TAG_MARKER_MIN_MINIMAP_RANGE);
        float flDisplayRangeMax = ActorGetInheritedTagMapValueFloat(ptRActor->snoActor, ptRActor->tMinimapInfo, TAG_ACTOR_MINIMAP_MAX_RANGE, TAG_MARKER_MAX_MINIMAP_RANGE);
        float flDistToMarkerSq = Vector3D::Distance2DSquared(tPlaceACD.wp, wpPlayer);
        if (flDisplayRangeMin > FLT_EPSILON && flDisplayRangeMin * flDisplayRangeMin > flDistToMarkerSq)
            continue;




        if (flDisplayRangeMax > FLT_EPSILON && flDisplayRangeMax * flDisplayRangeMax < flDistToMarkerSq)
            continue;




        BOOL bRespectsFoW = ActorGetInheritedTagMapValueInt(ptRActor->snoActor, ptRActor->tMinimapInfo, TAG_ACTOR_MINIMAP_RESPECTS_FOW, TAG_MARKER_MINIMAP_HIDE_UNTIL_EXPLORED);
        if (bRespectsFoW)
        {
            MapScene *ptScene = sFindScene(tPlaceACD);
            if (!sPositionInExploredArea(tPlaceACD.wp, ptScene))
            {
                float flDiscoveryRadius = ActorGetInheritedTagMapValueFloat(ptRActor->snoActor, ptRActor->tMinimapInfo, TAG_ACTOR_MINIMAP_DISCOVERY_RADIUS, TAG_MARKER_MINIMAP_DISCOVERY_RADIUS);
                if (ptScene == NULL || !(flDistToMarkerSq < flDiscoveryRadius * flDiscoveryRadius))
                    continue;
                sRevealMapAtLocation(tPlaceACD, 20.0f);
            }
        }






        ImageTextureFrame tImageTexFrame = Minimap::ResolveIconForRActor(ptRActor);
        if (tImageTexFrame.snoTexture == SNO_NONE)
            continue;



        if (GameTypeIsPvP())
        {
            tImageTexFrame.snoTexture = sGetPVPIconForACD(*ptACD);
        }

        if (sIsQuestActor(ptACD) && ptACD->IsMonster())
        {
            gtTutorialMgr.ShowTutorial(UIWND_MINIMAP_TUTORIAL_ANCHOR, GlobalSNOGet(GLOBALSNO_TUTORIAL_MINIMAP_QUEST_MOB), NULL, TRUE);
        }





        BOOL bBecomesArrow = !ActorGetInheritedTagMapValueInt(ptRActor->snoActor, ptRActor->tMinimapInfo, TAG_ACTOR_MINIMAP_NO_ARROW, TAG_MARKER_MINIMAP_DOES_NOT_BECOME_ARROW);
        if (ptACD->AttributesGetInt(ATTRIBUTE_MINIMAP_ICON_DISABLE_ARROW))
            // Inference: the original merges the attribute and tag vetoes before testing zero.
            bBecomesArrow = FALSE;


        if (ACDIsLootRunBoss(*ptACD))
        {
            // Inference: original ORs the loot-run-boss result into the arrow eligibility.
            bBecomesArrow = TRUE;
        }


        uint32 dwFlags = 0;
        if (bBecomesArrow) dwFlags |= (1 << DMF_CAN_TURN_INTO_ARROW_BIT);


        Vector2D vecMarkerUIC;
        sWorldToMapUIC(tPlaceACD.wp.As2D(), vecMarkerUIC);
        sDrawMarker(snoShaderMarker, tImageTexFrame, vecMarkerUIC, flZ, dwFlags, idACD, 0, 0.0f);




    }
}

// NON_MATCHING: same size; the inlined sWorldToMapUIC multiplies (line 2577) are scheduled differently. The w6 difference went away once sDrawArrow/sDrawMarker were written.
#line 2518
void UIMinimap::DrawLocalPlayerMarker(
    float & flZ )
{
    if (sgtMinimapGlobals->dwDisabledLayers & (1 << Minimap::LAYER_LOCAL_PLAYER))
        return;



    SNO snoShaderMarker = ShaderMapGetDefaultShader(GlobalSNOGet(GLOBALSNO_MINIMAP_SHADERMAP_MARKER), TRUE, NULL);

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!PlayerIsLocal(ptPlayer))
            continue;




        const RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptRActorPlayer != NULL)
        {
            ActorCommonData & tACDPlayer = ACDGet(ptRActorPlayer->idACD);


            if (!tACDPlayer.IsAlive())
                continue;



            float fFacingOffset = -3.9551096f;

            if (WorldIsRotated(CGameGetCurrentWorld()))
            {
                fFacingOffset = -3.16971135f;
            }

            if (GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1))
            {
                Team eTeam = tACDPlayer.GetTeam();
                if (eTeam == TEAM_PLAYER_1)
                    fFacingOffset = -0.813516855f;
            }



            ImageTextureFrame tImageTexFrame;
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "MarkerLocalPlayer", tImageTexFrame);

            const WorldLocation & wlPlayerLoc = ptRActorPlayer->tLocation.tWorldLocation;
            Vector2D vecPlayerUIC;
            sWorldToMapUIC(wlPlayerLoc.transform.wp.As2D(), vecPlayerUIC);



            CRefString sMapName;
            uint32 dwFlags = (1 << DMF_CAN_TURN_INTO_ARROW_BIT) | (1 << DMF_USE_ACTOR_ORIENTATION_BIT);
            sDrawMarker(snoShaderMarker, tImageTexFrame, vecPlayerUIC, flZ,
                dwFlags,


                ptRActorPlayer->idACD, 0, fFacingOffset);
        }
    }

}
#line 2585
void UIMinimap::DrawPetMarkers(float & flZ)
{


    if (sgtMinimapGlobals->dwDisabledLayers & (1 << Minimap::LAYER_PETS))
        return;






    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        if (PlayerIsLocal(ptPlayer))
        {

            DrawPetMarkersForPlayer(ptPlayer, flZ);
        }
    }
}
// NON_MATCHING: 824/824 bytes, 14 words differ in sWorldToMapUIC scheduling; first faithful body, no variants.
void UIMinimap::DrawPetMarkersForPlayer(
    Player * ptPlayer,
    float & flZ)
{
    SNO snoShaderMarker = ShaderMapGetDefaultShader(GlobalSNOGet(GLOBALSNO_MINIMAP_SHADERMAP_MARKER), TRUE, NULL);

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    for (int32 i = 0; i < MAX_PETS; ++i)
    {

        const RActor * ptRActor = sFindPetRActor(PlayerGetPet(ptPlayer->idPlayerACD, i)); // Inference: null pet skips the marker (merged loop branch).
        if (ptRActor)
        {
            PetType eType = PlayerGetPetType(ptPlayer->idPlayerACD, i);
            if (eType == PETTYPE_WIZARDMIRRORIMAGE ||
                eType == PETTYPE_WITCHDOCTOR_DEATH_STATUE_TOTEM ||
                eType == PETTYPE_WITCHDOCTOR_FIRE_STATUE_TOTEM ||
                eType == PETTYPE_WITCHDOCTOR_ZOMBIE ||
                eType == PETTYPE_NECROMANCER_SKELETON_MAGE_DECAY ||
                eType == PETTYPE_GENERIC_STATIC_PET_0 ||
                eType == PETTYPE_GENERIC_STATIC_PET_1 ||
                eType == PETTYPE_GENERIC_STATIC_PET_2 ||
                eType == PETTYPE_GENERIC_STATIC_PET_3 ||
                eType == PETTYPE_GENERIC_STATIC_PET_4 ||
                eType == PETTYPE_GENERIC_STATIC_PET_5 ||
                eType == PETTYPE_GENERIC_STATIC_PET_6 ||
                eType == PETTYPE_COMPANION ||
                eType == PETTYPE_HIRELING_PROXY ||
                eType == PETTYPE_FOLLOWER_PROXY)
            {
                continue;
            }

            if (tACDPlayer.IsDueling())
            {

                if (eType == PETTYPE_HIRELING || eType == PETTYPE_FOLLOWER)
                    continue;
            }






            ImageTextureFrame tImageTexFrame;
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "MarkerLocalPlayerPet", tImageTexFrame);

            const WorldLocation & wlPetLoc = ptRActor->tLocation.tWorldLocation;
            Vector2D vecPlayerUIC;
            sWorldToMapUIC(wlPetLoc.transform.wp.As2D(), vecPlayerUIC);






            sDrawMarker(snoShaderMarker, tImageTexFrame, vecPlayerUIC, flZ,
                        0,


                        ptRActor->idACD, 0, 0.0f);
        }
    }
}

// NON_MATCHING: 1152/1152 bytes, 24 words differ in world-to-UI/combat-flag scheduling; tried reversing combat flag/state assignments.
void UIMinimap::DrawPlayerMarkers(
    float & flZ,
    SWorldID idSWorldCurrent)
{
    if (sgtMinimapGlobals->dwDisabledLayers & (1 << Minimap::LAYER_PLAYERS))
        return;



    const ActorCommonData * ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet()); // Inference: absent local ACD skips IsDueling (merged null branch).
    if (ptACDLocalPlayer && ptACDLocalPlayer->IsDueling())
        return;




    uint32 dwNow = GameGetServerTime();
    SNO snoShaderMarker = ShaderMapGetDefaultShader(GlobalSNOGet(GLOBALSNO_MINIMAP_SHADERMAP_MARKER), TRUE, NULL);
    // DWARF location is constant zero.
    BOOL bHideHostilePlayers = FALSE;

    for (const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (!PlayerIsLocal(ptPlayer))
        {


            // Inference: no trickle entry retains zero flags, as in original missing-entry branch.
            uint32 dwTrickleFlags = 0;
            WorldPlace tPlace;
            const ClientTrickleEntry * ptTrickleEntry = CTrickleEntryFindForANN(ptPlayer->tClientInfo.ann);
            if (ptTrickleEntry)
            {
                tPlace = ptTrickleEntry->tPlace;
                dwTrickleFlags = ptTrickleEntry->dwTrickleFlags;
            }

            if (dwTrickleFlags & (1 << TRICKLE_FLAG_OBSERVER_BIT))
                continue;



            if ((dwTrickleFlags & (1 << TRICKLE_FLAG_GHOSTED_BIT)) && !TeamsAreFriendly(ptPlayer->eTeam, PlayerGetLocalTeam()))
                continue;



            const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD); // Inference: only a present ACD overrides the trickle position.
            if (ptACDPlayer)
            {
                tPlace = ptACDPlayer->GetPlace();

                if (ptACDPlayer->IsObserver())
                    continue;

            }


            if (tPlace.idSWorld != idSWorldCurrent)
                continue;



            // Inference: named bit6 supplies the revealed-on-map branches at2802.
            BOOL bRevealedOnMap = dwTrickleFlags & (1 << TRICKLE_FLAG_PVP_PLAYER_REVEALED_ON_MAP);
            BOOL bHostilePlayer = !TeamsAreFriendly(ptPlayer->eTeam, LocalPlayerGetPrimary()->eTeam);
























            ImageTextureFrame tImageTexFrame;
            uint32 dwFlags = (1 << DMF_CAN_TURN_INTO_ARROW_BIT) | (1 << DMF_USE_PLAYER_ARROWS);
            // Inference: normal marker state is the fallthrough of the three combat checks.
            MapMarkerState eMarkerState = MMS_NORMAL;
            if (CPlayerIsEngagedWithTreasureGoblin(ptPlayer))
            {
                dwFlags |= (1 << DMF_TREASURE_GOBLIN);
                eMarkerState = MMS_TREASURE_GOBLIN;
            }
            else if (CPlayerIsEngagedWithRare(ptPlayer))
            {
                dwFlags |= (1 << DMF_COMBAT_WITH_ELITE);
                eMarkerState = MMS_COMBAT_WITH_ELITE;
            }
            else if (ptACDPlayer && ptACDPlayer->IsInCombat())
            {
                dwFlags |= (1 << DMF_COMBAT);
                eMarkerState = MMS_COMBAT;
            }

            if (ptPlayer->flHealthPercent < FLT_EPSILON || (dwTrickleFlags & (1 << TRICKLE_FLAG_DEAD_BIT)) ||
                (ptACDPlayer && !ptACDPlayer->IsAlive() && !ptACDPlayer->IsDueling()))
            {

                if (!(ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) || GameTypeIsPvP())
                {
                    ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "MarkerCorpse", tImageTexFrame);
                }
            }
            else if (GameCommonPlayerIsPartyGuide(*ptPlayer))
            {
                ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, cszPlayerGuideMarkers[eMarkerState], tImageTexFrame);
                dwFlags |= (1 << DMF_PARTY_GUIDE);
            }
            else
            {
                if (!bRevealedOnMap)
                {
                    if (!bHostilePlayer)
                    {
                        ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, cszPlayerMarkers[eMarkerState], tImageTexFrame);
                    }
                    else
                    {
                        ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "MarkerHostilePlayer", tImageTexFrame);
                    }
                }
                else
                {
                    if (!bHostilePlayer)
                    {
                        ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "MarkerExit", tImageTexFrame);
                    }
                    else
                    {
                        ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, "MarkerDungeonEntranceBoss", tImageTexFrame);
                    }
                }
            }


            Vector2D vecPlayerUIC;
            sWorldToMapUIC(tPlace.wp.As2D(), vecPlayerUIC);
            // Inference: no location or emitted use survives for this DWARF local; retain its declaration.
            BOOL fForceShowName; // DWARF nNameAlpha location is constant -1 (uint8=255).
            uint8 nNameAlpha = 255;















            sDrawMarker(snoShaderMarker, tImageTexFrame, vecPlayerUIC, flZ,
                        dwFlags,


                        ptPlayer->idPlayerACD, nNameAlpha, 0.0f); }
    }
}

// NON_MATCHING: 1212/1212 bytes, 7 words differ in world-to-UI/visibility helper scheduling; reversed distance operands (9 to 7).
void UIMinimap::DrawTrickleMarkers(
    float & flZ,
    SWorldID idSWorldCurrent)
{
    if (sgtMinimapGlobals->dwDisabledLayers & (1 << Minimap::LAYER_TRICKLE))
        return;




    SNO snoShaderMarker = ShaderMapGetDefaultShader(GlobalSNOGet(GLOBALSNO_MINIMAP_SHADERMAP_MARKER), TRUE, NULL);
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet()); // Inference: null ACD follows the merged exit.
    if (!ptACDPlayer)
        return;



    Vector3D wpPlayer = ptACDPlayer->GetPlace().wp;
    ClientTrickleList & tTrickleList = CTrickleGetList();
    POS pos = tTrickleList.GetHeadPosition();

    while (pos)
    {
        ClientTrickleEntry & tTrickleEntry = tTrickleList.GetNext(pos);
        if (PlayerGetByANN(tTrickleEntry.ann))
            continue;



        ACDID idACDTrickled = ACDFromANN(tTrickleEntry.ann);
        if (ACDTryToGet(idACDTrickled))
            continue;



        if (tTrickleEntry.tIcon.snoTexture == SNO_NONE)
            continue;



        if (tTrickleEntry.eType == TRICKLE_INFO_LOCAL_PLAYER ||
            tTrickleEntry.eType == TRICKLE_INFO_MY_PET ||
            tTrickleEntry.eType == TRICKLE_INFO_FRIENDLY_PLAYER ||
            tTrickleEntry.eType == TRICKLE_INFO_FRIENDLY_PLAYER_PET ||
            tTrickleEntry.eType == TRICKLE_INFO_FOLLOWER_PET ||
            tTrickleEntry.eType == TRICKLE_INFO_HOSTILE_PLAYER ||
            tTrickleEntry.eType == TRICKLE_INFO_HOSTILE_PLAYER_PET ||
            tTrickleEntry.eType == TRICKLE_INFO_FRIENDLY_MONSTER)
        {
            continue;
        }


        if (tTrickleEntry.tPlace.idSWorld != idSWorldCurrent)
            continue;



        if (tTrickleEntry.dwTrickleFlags & ((1 << TRICKLE_FLAG_OBSERVER_BIT) | (1 << TRICKLE_FLAG_NOT_ON_MAP) | (1 << TRICKLE_FLAG_HIDDEN)))
            continue;





        float flDistToMarkerSq = Vector3D::Distance2DSquared(tTrickleEntry.tPlace.wp, wpPlayer);
        if (tTrickleEntry.fMinMinimapDistSq > FLT_EPSILON && tTrickleEntry.fMinMinimapDistSq > flDistToMarkerSq)
            continue;




        if (tTrickleEntry.fMaxMinimapDistSq > FLT_EPSILON && tTrickleEntry.fMaxMinimapDistSq < flDistToMarkerSq)
            continue;





        if ((tTrickleEntry.dwTrickleFlags & (1 << TRICKLE_FLAG_RESPECTS_FOW)) && !(tTrickleEntry.dwTrickleFlags & (1 << TRICKLE_FLAG_LOOT_RUN_BOSS)))
        {
            if (!sPositionInExploredArea(tTrickleEntry.tPlace.wp, sFindScene(tTrickleEntry.tPlace)))
                continue;
        }


        // Inference: default marker flags zero; the sole assignment enables arrows.
        uint32 dwFlags = 0;
        if (tTrickleEntry.dwTrickleFlags & (1 << TRICKLE_FLAG_BECOMES_ARROW)) dwFlags |= (1 << DMF_CAN_TURN_INTO_ARROW_BIT);

        Vector2D vecUIC;
        sWorldToMapUIC(tTrickleEntry.tPlace.wp.As2D(), vecUIC);














        ImageTextureFrame tIconToUse = tTrickleEntry.tIcon;

        if (GameTypeIsPvP() && tTrickleEntry.snoActor != SNO_NONE)
        {
            tIconToUse.snoTexture = sGetPVPIcon(tTrickleEntry.eType, tTrickleEntry.snoActor);
        }

        sDrawMarker(snoShaderMarker, tIconToUse, vecUIC, flZ,
                    dwFlags,


                    ACDFromANN(tTrickleEntry.ann), 0, 0.0f);
    }
}

// NON_MATCHING: 5/391 insns; in the inlined sGetFlagsFromLocation the original computes dwOffset>>2 before the constant 6 for dwFraction (orr/lsr order); 7 variants of the shift/fraction expressions tried.
void UIMinimap::sDrawListMarkers(
    float flZ,
    SWorldID idSCurrentWorld,
    BOOL bUsePingList)
{
    MarkerInfoList * ptListMarkers;
    if (bUsePingList)
    {
        ptListMarkers = sgtMinimapGlobals->ptListPingMarkers;
    }
    else
    {
        ptListMarkers = sgtMinimapGlobals->ptListServerMarkers;
    }

    if (ptListMarkers == NULL)
        return;



    POS pos;
    uint32 nKey;
    MarkerInfo tMarker;
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (ptACDPlayer == NULL)
        return;



    ActorCommonData & tACDPlayer = *ptACDPlayer;
    RActorID idPlayerRActor = LocalPlayerRActorGet();
    SNO snoLevelArea = RActorGetLevelArea(idPlayerRActor);
    WorldPlace tPlayerLoc = tACDPlayer.GetPlace();
    Vector2D vPlayerPlace(tPlayerLoc.wp.x, tPlayerLoc.wp.y);
    SNO snoShaderMarker = ShaderMapGetDefaultShader(GlobalSNOGet(GLOBALSNO_MINIMAP_SHADERMAP_MARKER), TRUE, NULL);

    pos = ptListMarkers->GetStartAssoc();
    while (pos)
    {
        ptListMarkers->GetNextAssoc(pos, nKey, tMarker);

        if (tMarker.tPlace.idSWorld != idSCurrentWorld)
            continue;



        Vector2D vecMarkerUIC;
        Vector2D vMarkerPlace(tMarker.tPlace.wp.x, tMarker.tPlace.wp.y);

        float flDistSq = Vector2D::DistanceSquared(vMarkerPlace, vPlayerPlace);
        if (tMarker.m_fMaxDisplayRangeSq > FLT_EPSILON && flDistSq > tMarker.m_fMaxDisplayRangeSq)
        {

            BOOL bAutoDiscover = FALSE;
            if (tMarker.m_snoQuestSource != SNO_NONE)
            {
                ScopedQuestDefinitionLockR pdefQuest(tMarker.m_snoQuestSource);
                if (snoLevelArea == pdefQuest->tBountyData.snoLevelArea ||
                    QuestHasAdditionalLevelArea(tMarker.m_snoQuestSource, snoLevelArea))
                {
                    uint32 uAutoDiscoveryDicks = QuestGetAutoPingTimeTicks(tMarker.m_snoQuestSource);
                    if (uAutoDiscoveryDicks == (uint32)-1)
                    {
                        bAutoDiscover = tMarker.m_flDiscoveryTime > 60.0f;
                    }
                    else
                    {
                        bAutoDiscover = tMarker.m_flDiscoveryTime >= uAutoDiscoveryDicks * OO_TICKS_PER_SECOND;
                    }
                }
            }

            if (!bAutoDiscover)
                continue;
        }











        if (tMarker.m_fMinDisplayRangeSq > FLT_EPSILON &&
            flDistSq < tMarker.m_fMinDisplayRangeSq)
            continue;



        if (tMarker.m_bRespectsFoW)
        {

            MapScene * ptScene = sFindScene(tMarker.tPlace);
            if (!sPositionInExploredArea(tMarker.tPlace.wp, ptScene))
            {
                if (ptScene == NULL || !(flDistSq < tMarker.m_fDiscoveryRangeSq))
                    continue;
                sRevealMapAtLocation(tMarker.tPlace, 20.0f);
            }
        }






        if (tMarker.m_snoKnownActorOverride != SNO_NONE)
        {
            POS posMarker = sgtMinimapGlobals->ptListMarkers->GetHeadPosition();
            while (posMarker)
            {
                ACDNetworkName ANN = (ACDNetworkName)sgtMinimapGlobals->ptListMarkers->GetNext(posMarker);
                ACDID idACD = ACDFromANN(ANN);
                const ActorCommonData * ptACD = ACDTryToGet(idACD);
                // whatever used ptACD here left no instructions (only ACDTryToGet's TLS call survives in the original)

            }
        }





























        sWorldToMapUIC(vMarkerPlace, vecMarkerUIC);
        uint32 dwFlags = 0;
        if (tMarker.m_bCanBecomeArrow)
            dwFlags |= (1 << DMF_CAN_TURN_INTO_ARROW_BIT);
        if (tMarker.m_bIsKeyWarden)
            dwFlags |= (1 << DMF_KEY_WARDEN);




        sDrawMarker(snoShaderMarker, tMarker.m_tTexture, vecMarkerUIC, flZ, dwFlags, ACDID_INVALID, 0, 0.0f);





        if (tMarker.m_nPlayerUseFlags)
        {
            BOOL fUsedByNonLocalPlayer = FALSE;
            CRefString sMapName;
            for (const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
            {
                if (!PlayerIsLocal(ptPlayer))
                {



                    if (tMarker.m_nPlayerUseFlags & (1 << ptPlayer->nPlayerIndex))
                        fUsedByNonLocalPlayer = TRUE;
                }
            }











            if (fUsedByNonLocalPlayer)
            {
                ImageTextureFrame tImage;
                tImage.snoTexture = GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_QUEST_RING); tImage.nFrame = 0;


                uint32 dwFlagsScoped = (1 << DMF_CAN_TURN_INTO_ARROW_BIT) | (1 << DMF_FORCE_DEST_PING) | (1 << DMF_USE_PLAYER_ARROWS);
                sDrawMarker(snoShaderMarker, tImage, vecMarkerUIC, flZ + FLT_EPSILON, dwFlagsScoped, ACDID_INVALID, 0, 0.0f);
            }




        }
    }
}


void UIMinimap::DrawServerMarkers(float flZ, SWorldID idSCurrentWorld)
{


    if (sgtMinimapGlobals->dwDisabledLayers & (1 << Minimap::LAYER_SERVER))
        return;





    sDrawListMarkers(flZ, idSCurrentWorld, FALSE);
    sDrawListMarkers(flZ, idSCurrentWorld, TRUE);
}


void UIMinimap::sDrawMap(
    float & flZSlice)
{
    sRecalculateRegions();


    RGBAColor colorImage(255, 255, 255, m_bAlpha);


    uint32 dwNow = CGameGetRealTicks();

    SNO snoMinimapShader = ShaderMapGetDefaultShader(GlobalSNOGet(GLOBALSNO_MINIMAP_SHADERMAP), TRUE, NULL);


    Vector2D wpExploredMaskCorners[4];
    Rect2D rectExploredTextureMask;
    rectExploredTextureMask.Set(0.0f, 0.0f, 1.0f, 1.0f);
    rectExploredTextureMask.ToPoints(wpExploredMaskCorners, FALSE);


    SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();
    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    POS pos = tListMapScenes.GetHeadPosition();
    while (pos)
    {
        MapScene & tMapScene = tListMapScenes.GetNext(pos);


        if (tMapScene.idSWorld != idSWorldCurrent)
            continue;





        if (tMapScene.snoMapTex == SNO_NONE)
            continue;




        Vector2D vecCornersUIC[4];
        sWorldToMapUIC(Vector2D(tMapScene.rectSceneWorld.left, tMapScene.rectSceneWorld.top), vecCornersUIC[0]);
        sWorldToMapUIC(Vector2D(tMapScene.rectSceneWorld.right, tMapScene.rectSceneWorld.top), vecCornersUIC[3]);
        sWorldToMapUIC(Vector2D(tMapScene.rectSceneWorld.right, tMapScene.rectSceneWorld.bottom), vecCornersUIC[2]);
        sWorldToMapUIC(Vector2D(tMapScene.rectSceneWorld.left, tMapScene.rectSceneWorld.bottom), vecCornersUIC[1]);


        Vector2D vecMin = vecCornersUIC[0];
        Vector2D vecMax = vecCornersUIC[0];
        for (int32 i = 1; i < 4; ++i)
        {
            vecMin.x = Math::Min(vecMin.x, vecCornersUIC[i].x);
            vecMin.y = Math::Min(vecMin.y, vecCornersUIC[i].y);
            vecMax.x = Math::Max(vecMax.x, vecCornersUIC[i].x);
            vecMax.y = Math::Max(vecMax.y, vecCornersUIC[i].y);
        }
        Rect2D rectBounding;
        rectBounding.Set(vecMin.x, vecMin.y, vecMax.x, vecMax.y);
        if (!Rect2D::Intersect(rectBounding, m_rectMap, NULL))
            continue;





        if (tMapScene.snoExploredTexture == SNO_NONE)
        {
            if (tMapScene.pExploredData == NULL && !tMapScene.bRevealed)
            {
                sCreateExploredInfo(tMapScene, NULL, FALSE);
            }


            sCreateExploredTexture(tMapScene);
            if (tMapScene.snoExploredTexture == SNO_NONE)
                continue;
        }





        tMapScene.dwLastDrawn = dwNow;

        float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
        flZSlice += 1.0f;


        Vector2D vecMaskCorners[4];
        Vector2D vecCornersPixel[4];
        for (int i = 0; i < 4; ++i)
        {
            vecMaskCorners[i].x = (vecCornersUIC[i].x - m_rectMap.left) / m_rectMap.Width();
            vecMaskCorners[i].y = (vecCornersUIC[i].y - m_rectMap.top) / m_rectMap.Height();

            ConvertUICToPixelsDoNotSnap(vecCornersUIC[i], vecCornersPixel[i]);
        }

        SNO snoShader = snoMinimapShader;















        SpriteDrawDeferredMinimapTile(
            tMapScene.snoMapTex,
            snoShader,
            vecCornersPixel,
            colorImage,
            crgbaBlack,
            flZ,
            m_snoTextureMask,
            vecMaskCorners,
            tMapScene.snoExploredTexture,
            wpExploredMaskCorners);
    }

    sDrawMapMarkers(flZSlice);

    if (GetID() == UIDLG_MAP_LOCALMAP)
    {
        sDrawMapObjectiveCones(flZSlice);
    }
}



static Angle sGetMapRotation()
{
    // Inference: -135 degree default; the folded constants are -3/8 and -1/4 circle (0x7100e3cc10).
    Angle aMapRotation = -3.0f * EIGHTH_CIRCLE;

    if (WorldIsRotated(CGameGetCurrentWorld()))
        aMapRotation += EIGHTH_CIRCLE;




    if (GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1) &&
        LocalPlayerGet()->eTeam == TEAM_PLAYER_1)
    {
        aMapRotation += HALF_CIRCLE;
    }
    return aMapRotation;
}























inline void UIMinimap::sHandleControllerScrolling(const BufferedInput & tInput)
{
    const float MAX_SPEED = 5.0f;

    if (Console::ControllerUtils::LStickIsInDeadZone(tInput.tBufControllerData.nControllerIndex, FALSE))
        return;
    if (Console::ControllerUtils::IsButtonDown(tInput.tBufControllerData.nControllerIndex, 14))
        return;





    Vector2D vecStick = Console::ControllerUtils::GetLStickPosition(tInput.tBufControllerData.nControllerIndex, FALSE);

    vecStick.x = vecStick.x * MAX_SPEED * 960.0f / 720.0f;
    vecStick.y *= -MAX_SPEED;
    vecStick = Matrix3x3::Transform2x2(vecStick, m_mUICToWorld);

    m_pointScrollOffset += vecStick;
}




inline void UIMinimap::sHandleControllerZoom(const BufferedInput & tInput)
{
    const float MAX_SPEED = 0.05f;

    switch (tInput.tBufControllerData.eInputButton)
    {
    case 8: m_flZoomScaleFactor += -MAX_SPEED; break;
    case 9: m_flZoomScaleFactor += MAX_SPEED; break;
    }







    m_flZoomScaleFactor = Math::Pin(m_flZoomScaleFactor, 0.5f, 1.5f);
}

























































BOOL UIMinimap::sInput(const BufferedInput & tInput)
{


    if (tInput.tBufControllerData.nControllerIndex != UI_GetPrimaryUIController())
        return FALSE;





    switch (tInput.eInputMsg)
    {






































































    case INPUTMSG_LEFTSTICK:
        if (m_dwFlags & (1 << UIM_ALLOW_SCROLLING_BIT))
        {
            sHandleControllerScrolling(tInput);
        }
        break;

    case INPUTMSG_RIGHTSTICK:
        if (m_dwFlags & (1 << UIM_ALLOW_SCROLLING_BIT))
        {
            sHandleControllerZoom(tInput);
        }
        break;

    case INPUTMSG_CONTROLLERBUTTONDOWN:
        if (m_dwFlags & (1 << UIM_ALLOW_SCROLLING_BIT))
        {

            switch (tInput.tBufControllerData.eInputButton)
            {
            case 11:
                sResetZoom();
                return TRUE;


            case 10:
                sResetScrollOffset();
                return TRUE;



            case 9:
                m_bInputBleedOverCleared = TRUE;
                break;
            }
        }
        break;











    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
        if (m_dwFlags & (1 << UIM_ALLOW_SCROLLING_BIT))
        {
            if (tInput.tBufControllerData.eInputButton == 8)
            {
                sHandleControllerZoom(tInput);
                return TRUE;
            }
            else if (tInput.tBufControllerData.eInputButton == 9 && m_bInputBleedOverCleared)
            {
                sHandleControllerZoom(tInput);
                return TRUE;
            }
        }
        break;
    }





    return FALSE;
}



void UIMinimap::Draw(const uint8 bLayerAlpha, float & flZSlice)
{
    if (sgtMinimapGlobals == NULL || !sgtMinimapGlobals->fEnabled)
        return;

    UIWindow::Draw(bLayerAlpha, flZSlice);
    if (UpdateTransform())
        sDrawMap(flZSlice);
}

















// NON_MATCHING: 9/431 insns, same size; only the s8-s11 assignment of the four position accumulators differs (tried chained and explicit zero inits)
BOOL UIMinimap::UpdateTransform()

{
    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    Vector2D pointMapCenterWorld;
    // Inference: accumulators start at zero (fmov wzr; no Vector2D constructor is inlined here).
    pointMapCenterWorld.x = 0.0f;
    pointMapCenterWorld.y = 0.0f;
    // Inference: lines 3673-3694 hold no code in the original line table; pointMapCenterAliveWorld copies the zeroed pointMapCenterWorld (no Vector2D ctor inlined).


















    int32 nLocalPlayers = 0;
    int32 nLocalAlivePlayers = 0;
    Vector2D pointMapCenterAliveWorld = pointMapCenterWorld;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!PlayerIsLocal(ptPlayer))
            continue;




        const ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        const RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptRActorPlayer)
        {
            const WorldLocation & wlPlayerLoc = ptRActorPlayer->tLocation.tWorldLocation;
            pointMapCenterWorld.x += wlPlayerLoc.transform.wp.x;
            pointMapCenterWorld.y += wlPlayerLoc.transform.wp.y;
            ++nLocalPlayers;

            if (ptACD && ptACD->IsAlive())
            {
                pointMapCenterAliveWorld.x += wlPlayerLoc.transform.wp.x;
                pointMapCenterAliveWorld.y += wlPlayerLoc.transform.wp.y;
                ++nLocalAlivePlayers;
            }
        }
    }
    if (nLocalAlivePlayers > 0)
    {
        pointMapCenterWorld.x = pointMapCenterAliveWorld.x / nLocalAlivePlayers;
        pointMapCenterWorld.y = pointMapCenterAliveWorld.y / nLocalAlivePlayers;
    }
    else if (nLocalPlayers > 0)
    {
        pointMapCenterWorld.x /= nLocalPlayers;
        pointMapCenterWorld.y /= nLocalPlayers;
    }
    else
    {
        return FALSE;
    }



    pointMapCenterWorld += m_pointScrollOffset;

    GameType eGameType = GameGetGameType();
    if (eGameType >= GT_PVP_GAME_FIRST && eGameType <= GT_PVP_GAME_LAST)
    {



        if (!tListMapScenes.IsEmpty())
        {

            MapScene & tMapScene = tListMapScenes.GetHead();
            int32 nScenes = tListMapScenes.Count();


            float flLength = Math::Sqrt((float)nScenes);

            pointMapCenterWorld.x = tMapScene.rectSceneWorld.Width() * flLength * 0.5f;
            pointMapCenterWorld.y = tMapScene.rectSceneWorld.Height() * flLength * 0.5f;
        }
    }

    {
        Matrix3x3 mTrans = cgm3x3Identity;
        mTrans._31 = -pointMapCenterWorld.x;
        mTrans._32 = -pointMapCenterWorld.y;

        Angle aMapRotation = sGetMapRotation();
        Matrix3x3 mRotate;
        mRotate.SetRotationZ(aMapRotation);

        Matrix3x3 mScale;
        mScale.SetScale(1.0f / (m_flZoomScaleFactor * m_flFeetPerPixel));

        m_mWorldToUIC = mTrans * mRotate * mScale * cgm3x3FlipHandedness;
        m_mUICToWorld = m_mWorldToUIC.Inverse();
    }

    return TRUE;
}




UIID UIMinimap::HitTest(const IVector2D & pt)
{
    if (IsActive() && EatsClicks() != EC_IGNORE && IsInside(pt))
    {
        return GetID();
    }
    return GetUIIDNone();
}

















UIID UIMinimap::HitTestForTooltips(const IVector2D & pt, BOOL &)
{
    if (IsActive() && IsInside(pt))
    {
        return GetID();
    }
    return GetUIIDNone();
}











BOOL UIMinimap::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    switch (msg)
    {
    case UI_UPDATE:
        {


            sUpdate();
        }
        break;

    case UI_INPUT:
        {
            BOOL bRetval = sInput(*(const BufferedInput *)dwParam);
            if (bRetval)
                return bRetval;
        }
        break;

    case UI_REGISTER_INPUT_LISTENER:
        UI_RegisterInputListener(this);
        break;
    }

    return UIWindow::HandleMessage(msg, dwParam);
}










BOOL UIMinimap::CanShowBuff(SNO snoPower, int32 nBuffIndex) {
    if ((snoPower == GlobalSNOGet(GLOBALSNO_POWERS_MULTIPLAYER_BUFF) && nBuffIndex == 0) ||
        snoPower == GlobalSNOGet(GLOBALSNO_POWERS_EASTER_EGG_WORLD) ||
        snoPower == GlobalSNOGet(GLOBALSNO_POWERS_ANNIVERSARY_XPMF) ||
        snoPower == GlobalSNOGet(GLOBALSNO_POWERS_COMMUNITY_EVENT_XPMF)) {
        BOOL bMergesTooltip = PowerBuffMergesTooltip(AutoPowerDefinitionR(snoPower), nBuffIndex);
        return !bMergesTooltip;
    }
    return FALSE;
}











void UIMinimap::HandleBuffCountAttributeChange(const FastAttribValue tValue, const SNO snoPower, const int32 nBuffIndex, const ActorCommonData & tACD) {
}






































static void sCreateExploredInfo(
    MapScene & tMapScene,

    const uint8 * pInitialExploredData,



    const BOOL & bRevealed)
{





    if (!pInitialExploredData && tMapScene.pExploredData)
        return;




    tMapScene.bRevealed = bRevealed || GameIsParamFlagSet(GPF_REVEAL_MINIMAP);



    if (sgtMinimapGlobals->tMapSceneExploredDataList.GetUsedBlockCount() == MAX_EXPLORED_DATA_COUNT)
    {




        uint32 dwNow = CGameGetRealTicks();
        uint32 dwOldest = 0;
        MapScene *ptOldestMapScene = NULL;
        MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
        POS pos = tListMapScenes.GetHeadPosition();
        while (pos)
        {
            MapScene & tMyMapScene = tListMapScenes.GetNext(pos);
            if (!tMyMapScene.pExploredData)
                continue;


            if (dwNow - tMyMapScene.dwLastDrawn > dwOldest || ptOldestMapScene == NULL)
            {
                // Inference: original csel updates both oldest scene and age together.
                dwOldest = dwNow - tMyMapScene.dwLastDrawn; ptOldestMapScene = &tMyMapScene;
            }
        }


        if (ptOldestMapScene->pExploredData != NULL && !ptOldestMapScene->bRevealed)
        {
            float fVisitedRatio = sGetVisitedRatio(*ptOldestMapScene);


            if (fVisitedRatio > 0.0f)
            {
                ptOldestMapScene->bRevealed = TRUE;
            }
        }
        sFreeMapSceneData(*ptOldestMapScene);
    }



    float wdSceneWidth = tMapScene.rectSceneWorld.Height();
    float wdSceneHeight = tMapScene.rectSceneWorld.Width();
    if (wdSceneWidth > wdSceneHeight)
    {
        tMapScene.nExploredDataWidth = EXPLORED_BITS_WIDTH;
        tMapScene.nExploredDataHeight = Math::FloatToInt(wdSceneHeight / wdSceneWidth * EXPLORED_BITS_HEIGHT);
    }
    else
    {
        tMapScene.nExploredDataWidth = Math::FloatToInt(wdSceneWidth / wdSceneHeight * EXPLORED_BITS_WIDTH); tMapScene.nExploredDataHeight = EXPLORED_BITS_HEIGHT;
    }

    // Inference: DWARF nDataSize is constant256, the declared explored-grid byte size.
    int32 nDataSize = EXPLORED_BITS_BYTE_SIZE;
    tMapScene.pExploredData = tMapScene.bRevealed ? NULL : sgtMinimapGlobals->tMapSceneExploredDataList.Alloc_Tracked();





















    if (!tMapScene.bRevealed)
    {

        if (!pInitialExploredData)
        {
            SigmaMemoryClear(tMapScene.pExploredData, nDataSize);
        }
        else
        {
            SigmaMemoryCopy(tMapScene.pExploredData, pInitialExploredData, nDataSize);
        }
    }
}



void UIMinimap_GfxInit() {
}









void UIMinimap_GfxDispose()
{


    if (sgtMinimapGlobals == NULL)
        return;





    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    POS pos = tListMapScenes.GetHeadPosition();
    while (pos)
    {

        MapScene & tMapScene = tListMapScenes.GetNext(pos);
        sDestroyExploredTexture(tMapScene, FALSE);
        tMapScene.snoExploredTexture = SNO_NONE;
    }
}
static void sMapSceneInitialize(
    MapScene & tMapScene)
{

    tMapScene.snoScene = tMapScene.idSScene = tMapScene.idSWorld = tMapScene.snoMapTex = SNO_NONE;


    tMapScene.rectSceneWorld.Set(0.0f, 0.0f, 0.0f, 0.0f);
    tMapScene.snoExploredTexture = SNO_NONE;
    tMapScene.pExploredData = NULL;
    tMapScene.nExploredDataWidth = tMapScene.nExploredDataHeight = 0;

    tMapScene.dwLastDrawn = CGameGetRealTicks();
    tMapScene.bRevealed = FALSE;
}


// NON_MATCHING:604 vs604 bytes,5 words differ in paired scene-ID stores and x21/x22 allocation; tried three evidence-based assignment orders.
void UIMinimapRevealScene(
    const SSceneID idSScene,
    const SNO snoScene,
    const PRTransform & transform,

    const uint8 * pInitialExploredData,



    const SWorldID idSWorld,
    const BOOL bRevealed)
{
    if (!sgtMinimapGlobals)
        return;




    MapScene *ptMapScene = sFindSceneByID(idSScene);
    if (ptMapScene)
    {
        if (bRevealed && !ptMapScene->bRevealed)
        {
            ptMapScene->bRevealed = TRUE;
            sFreeMapSceneData(*ptMapScene);
        }
        return;
    }


    MapScene tMapSceneNew;
    sMapSceneInitialize(tMapSceneNew);


    sSceneCalculate2DLogicalWorldBounds(tMapSceneNew.rectSceneWorld, snoScene, transform);


    if (tMapSceneNew.rectSceneWorld.Height() == 0.0f ||
        tMapSceneNew.rectSceneWorld.Width() == 0.0f)
        return;




    tMapSceneNew.dwLastDrawn = CGameGetRealTicks();
    tMapSceneNew.idSWorld = idSWorld; // rows: the paired idSWorld/idSScene store is on 4138:24, snoScene (reloaded) on 4139:24/26
    tMapSceneNew.snoScene = snoScene;
    tMapSceneNew.idSScene = idSScene;
    tMapSceneNew.snoExploredTexture = SNO_NONE;
    tMapSceneNew.fDirty = FALSE;
    sCreateExploredInfo(tMapSceneNew, pInitialExploredData, bRevealed);


    CRefString sSceneMapName = SceneDefinitionGetMinimapName(snoScene);
    tMapSceneNew.snoMapTex = SNOFileLookupGUID(TEXTURE_SNO, sSceneMapName.str());


    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    tListMapScenes.AddTail_Tracked(tMapSceneNew);








}



static void sUpdateLevelArea()
{


    ACDID idACDLocalPlayer = LocalPlayerACDGet();
    if (idACDLocalPlayer != ACDID_INVALID)
    {
        RActorID idPlayerRActor = LocalPlayerRActorGet();
        SNO snoLevelArea = RActorGetLevelArea(idPlayerRActor);

        if (GameIsOpenWorld() && snoLevelArea != SNO_NONE)
        {


            ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
            if (pdefLevelArea->snoOpenWorldDisplayLevelArea != SNO_NONE)
                snoLevelArea = pdefLevelArea->snoOpenWorldDisplayLevelArea;


        }





        if (sgtMinimapGlobals->snoLevelArea != snoLevelArea &&
            sgtMinimapGlobals->snoLevelAreaNext != snoLevelArea)
        {
            sgtMinimapGlobals->snoLevelAreaNext = snoLevelArea;

            CRefString sNameLabelNew = LevelAreaGetLocalizedName(snoLevelArea);
            CRefString sNameLabelPrev = LevelAreaGetLocalizedName(sgtMinimapGlobals->snoLevelArea);

            if (sNameLabelNew != sNameLabelPrev)
            {
                sgtMinimapGlobals->eLevelAreaState = LAS_HIDING;
            }
            else
            {
                sgtMinimapGlobals->snoLevelArea = snoLevelArea;
            }
        }
    }


    switch (sgtMinimapGlobals->eLevelAreaState)
    {
    case LAS_HIDING:
        {

            float flDeltaTime = CGameGetRealDeltaTime();

            sgtMinimapGlobals->flAreaNameAlpha += flDeltaTime * -2.5f;
            if (sgtMinimapGlobals->flAreaNameAlpha <= 0.0f)
            {
                sgtMinimapGlobals->flAreaNameAlpha = 0.0f;
                sgtMinimapGlobals->snoLevelArea = sgtMinimapGlobals->snoLevelAreaNext;
                sgtMinimapGlobals->snoLevelAreaNext = SNO_NONE;
                sgtMinimapGlobals->eLevelAreaState = LAS_SHOWING;
            }
        }
        break;


    case LAS_SHOWING:
        {
            float flDeltaTime = CGameGetRealDeltaTime();

            sgtMinimapGlobals->flAreaNameAlpha += flDeltaTime * 2.5f;
            if (sgtMinimapGlobals->flAreaNameAlpha >= 1.0f)
            {
                sgtMinimapGlobals->flAreaNameAlpha = 1.0f;
                sgtMinimapGlobals->eLevelAreaState = LAS_SHOWN;
            }
        }
        break;

    default:
        break;
    }


    ACDID idACDPlayerLocal = LocalPlayerACDGet();
    if (idACDPlayerLocal != ACDID_INVALID)
    {
        CRefString usAreaName;

        SNO snoLevelArea = sgtMinimapGlobals->snoLevelArea;
        if (snoLevelArea != SNO_NONE)
        {
            usAreaName = LevelAreaGetLocalizedName(snoLevelArea);
        }










































































































































































    // usAreaName's destructor is at line 4427 in the line table; nothing between 4255 and 4427 emits code.
    }
}




void sUpdatePingDiscoveryTime(float flTime)
{
    MarkerInfoList * ptListMarkers = sgtMinimapGlobals->ptListPingMarkers;
    if (ptListMarkers == NULL)
        return;

    RActorID idPlayerRActor = LocalPlayerRActorGet();
    SNO snoLevelArea = RActorGetLevelArea(idPlayerRActor);
    if (snoLevelArea == SNO_NONE)
        return;


    uint32 nKey;
    POS pos;
    MarkerInfo tMarker;


    pos = ptListMarkers->GetStartAssoc();
    while (pos)
    {
        ptListMarkers->GetNextAssoc(pos, nKey, tMarker);
        if (tMarker.m_snoQuestSource != SNO_NONE)
        {
            ScopedQuestDefinitionLockR pdefQuest(tMarker.m_snoQuestSource);
            if (snoLevelArea == pdefQuest->tBountyData.snoLevelArea || QuestHasAdditionalLevelArea(tMarker.m_snoQuestSource, snoLevelArea))
            {


                tMarker.m_flDiscoveryTime += flTime;
                ptListMarkers->SetAt_Tracked(nKey, tMarker);
            }
        }
    }
}







static BOOL sUseMPMapPosition()
{
    int32 nNumPlayers = PlayerGetNumPlayers();
    return nNumPlayers > 1;
}






// line table: the UIDeathMenu call is on line 4492 here, while UIMinimap_Update inlines sUseMPMapPosition (4477) without it
BOOL MinimapShouldDisplay(const MarkerInfoList * ptListMarker, const UIMinimap & tMiniMap)


{


    if (sUseMPMapPosition() && UIDeathMenu_Console_IsADeathScreenActive())
        return FALSE;


    if (PlayerGetLocalPlayerCount() == 1)
        return TRUE;



    if (!Prefs::GetGameplayPrefs().GetEnableSmartCoopMinimap())
        return TRUE;



    uint32 nCurrentTime = CGameGetRealTicks();

    if (ptListMarker == NULL)
        return FALSE;



    POS pos;
    uint32 nKey;
    MarkerInfo tMarker;
    ACDID idACD = LocalPlayerACDGet();
    if (idACD == ACDID_INVALID)
        return FALSE;



    ActorCommonData & tACDPlayer = ACDGet(idACD);
    WorldPlace tPlayerLoc = tACDPlayer.GetPlace();
    Vector2D vPlayerPlace(tPlayerLoc.wp.x, tPlayerLoc.wp.y);
    SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();

    pos = ptListMarker->GetStartAssoc();

    while (pos)
    {
        ptListMarker->GetNextAssoc(pos, nKey, tMarker);

        if (tMarker.tPlace.idSWorld == idSWorldCurrent && tMarker.m_bCanBecomeArrow)
        {








            if (nCurrentTime - sgtMinimapGlobals->nLastShownTime < MINIMAP_SHOW_UPDATE_TIMEOUT)
                return TRUE;



            Vector2D vMarkerPlace(tMarker.tPlace.wp.x, tMarker.tPlace.wp.y);

            if (tMiniMap.MarkerVisibleOnMinimap(vMarkerPlace))
            {



                float flDistSq = Vector2D::DistanceSquared(vMarkerPlace, vPlayerPlace);
                if (!(tMarker.m_fMaxDisplayRangeSq > FLT_EPSILON && flDistSq > tMarker.m_fMaxDisplayRangeSq) && !(tMarker.m_fMinDisplayRangeSq > FLT_EPSILON && flDistSq < tMarker.m_fMinDisplayRangeSq))
                    return TRUE;
            }
        }
    }









    return FALSE;

}





void UIMinimapUpdateZoom(const BOOL bSnapZoomLevel)
{
    if (sgtMinimapGlobals == NULL)
        return;

    if (UI_TestID(sgtMinimapGlobals->idMinimapDlg) && UI_TestID(sgtMinimapGlobals->idMinimap) && UI_TestID(UIDLG_MAP_LOCALMAP))
    {
        UIMinimap & tMiniMap = UI_GetMinimapFromID(sgtMinimapGlobals->idMinimap);
        UIMinimap & tLocalMap = UI_GetMinimapFromID(UIDLG_MAP_LOCALMAP);






        BOOL fShowMinimap = !tLocalMap.IsActive();
        if (!MinimapShouldDisplay(sgtMinimapGlobals->ptListPingMarkers, tMiniMap) &&
            !MinimapShouldDisplay(sgtMinimapGlobals->ptListServerMarkers, tMiniMap))
        {
            tMiniMap.UpdateTransform();
            fShowMinimap = FALSE;
        }

        UI_GetWindowFromID(sgtMinimapGlobals->idMinimapDlg).SetActive(fShowMinimap);

        tMiniMap.UpdateZoom();
        tLocalMap.UpdateZoom();
    }
}





float UIMinimap::sGetZoomDistance() const
{
    float flScale = GameGetMinimapZoomDist();
    if (GameTypeIsPvP())
    {
        ACDID idACD = LocalPlayerACDGet();
        if (idACD != ACDID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD)
            {
                SNO snoWorld = WorldCommonGetWorldSNO(ptACD->GetSWorldID());
                if (snoWorld != SNO_NONE)
                {
                    ScopedWorldDefinitionLockR pdefWorld(snoWorld);
                    if (pdefWorld->flPVPMinimapFeetPerPixel > 1e-6f)
                        flScale = pdefWorld->flPVPMinimapFeetPerPixel;
                }
            }
        }
    }

    if (m_dwFlags & (1 << UIM_ALLOW_SCROLLING_BIT))
        flScale *= 0.8f;

    return flScale;
}









inline void UIMinimap::UpdateZoom()
{
    m_flFeetPerPixelTarget = sGetZoomDistance();


    if (Math::Abs(m_flFeetPerPixelTarget - m_flFeetPerPixel) > 0.001f)
    {
        m_flFeetPerPixel = Math::Lerp(m_flFeetPerPixel, m_flFeetPerPixelTarget, ZOOM_APPROACH_RATE);
    }
    else
    {
        m_flFeetPerPixel = m_flFeetPerPixelTarget;
    }
}



static void sFreeMapSceneData(MapScene & tMapScene)
{


    if (tMapScene.snoExploredTexture != SNO_NONE)
    {
        sDestroyExploredTexture(tMapScene, TRUE);
    }



    if (tMapScene.pExploredData != NULL)
    {
        sgtMinimapGlobals->tMapSceneExploredDataList.Free(tMapScene.pExploredData);
        tMapScene.pExploredData = NULL;
    }
}








inline void UIMinimap::sUpdate()
{
    if (m_dwFlags & (1 << UIM_ALLOW_SCROLLING_BIT))
    {


        CursorMgr & tCursorMgr = PlayerGetCursorMgr();
        if (!(UI_GetMousedOverControl() == GetID()))
        {
            tCursorMgr.ResetCursorState(FALSE);
        }
    }









    uint32 dwCurTime = CGameGetRealTicks();
    if (m_dwLastTooltipTime != (uint32)-1 && dwCurTime > m_dwLastTooltipTime + 1)
    {


        m_tNameDrawInfo.Initialize();
        m_dwLastTooltipTime = (uint32)-1;
    }
}

void UIMinimap_Update()
{

    {



        UIWindow & tMinimapWrapper = UI_GetWindowFromID(UIDLG_MINIMAP_PVE);
        BOOL fShowMinimap;

        UIMinimap & tMiniMap = UI_GetMinimapFromID(sgtMinimapGlobals->idMinimap);
        fShowMinimap = MinimapShouldDisplay(sgtMinimapGlobals->ptListPingMarkers, tMiniMap) || MinimapShouldDisplay(sgtMinimapGlobals->ptListServerMarkers, tMiniMap);



        tMinimapWrapper.SetActive(fShowMinimap);


        const UIID & idAnchor = sUseMPMapPosition() ? UIDLG_MINIMAP_PVE_ANCHOR_MP : UIDLG_MINIMAP_PVE_ANCHOR_SP;
        UIWindow & tWndAnchor = UI_GetWindowFromID(idAnchor);





        const Rect2D & rectAnchor = tWndAnchor.GetWindowRect();
        Vector2D vecTLAnchor(rectAnchor.left, rectAnchor.top);

        Vector2D vecTL, vecDim;
        tMinimapWrapper.GetRelativePosition(vecTL, vecDim);
        tMinimapWrapper.SetRelativePosition(vecTLAnchor, vecDim);
    }






    sUpdateLevelArea();
    sUpdatePingDiscoveryTime(CGameGetRealDeltaTime());
    UIMinimapUpdateZoom(FALSE);






    uint32 dwNow = CGameGetRealTicks();
    if (dwNow - sgtMinimapGlobals->dwLastClearTick > 20)
    {
        sgtMinimapGlobals->dwLastClearTick = dwNow;

        ScopedGlobalDefinitionLockR pdefGlobal;
        float flRevealRadius = pdefGlobal->flMinimapRevealRadius;

        SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();


        MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
        POS pos = tListMapScenes.GetHeadPosition();
        while (pos)
        {
            MapScene & tMapScene = tListMapScenes.GetNext(pos);


            for (const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
            {
                if (PlayerIsLocal(ptPlayer))
                {
                    const RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);
                    if (ptRActorPlayer == NULL)
                        continue;



                    const PRTransform & transform = ptRActorPlayer->tLocation.tWorldLocation.transform;

                    Vector2D wpLocClear;
                    wpLocClear.x = transform.wp.x; wpLocClear.y = transform.wp.y;






                    if (idSWorldCurrent == tMapScene.idSWorld)
                    {

                        sUpdateSceneVisibility(tMapScene, wpLocClear, 1, flRevealRadius);
                    }
                }
                else
                {



                    if (!TeamsAreFriendly(ptPlayer->eTeam, LocalPlayerGetPrimary()->eTeam))
                        continue;



                    ClientTrickleEntry * ptTrickleEntry = CTrickleEntryFindForANN(ptPlayer->tClientInfo.ann);
                    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptTrickleEntry == NULL && ptACD == NULL)
                        continue;



                    WorldPlace tACDPlace;
                    if (ptTrickleEntry)
                    {
                        tACDPlace = ptTrickleEntry->tPlace;
                    }

                    if (ptACD)
                    {
                        tACDPlace = ptACD->GetPlace();
                    }


                    if (tACDPlace.idSWorld != tMapScene.idSWorld)
                        continue;




                    Vector2D wpLocClear;
                    wpLocClear.x = tACDPlace.wp.x; wpLocClear.y = tACDPlace.wp.y;


                    sUpdateSceneVisibility(tMapScene, wpLocClear, 2, flRevealRadius);
                }
            }





            if (tMapScene.snoExploredTexture != SNO_NONE)
            {









                if (dwNow - tMapScene.dwLastDrawn > 300)
                {
                    sDestroyExploredTexture(tMapScene, TRUE);
                }
            }
        }
    }










}



void UIMinimap_InitializeForGame()
{
    sgtMinimapGlobals = new MinimapGlobals;







    sgtMinimapGlobals->ptListMarkers = new MinimapMarkerList(MemoryPoolsGetDATAIDListPool());
    sgtMinimapGlobals->ptListServerMarkers = new MarkerInfoList(16, NULL);
    sgtMinimapGlobals->ptListPingMarkers = new MarkerInfoList(16, NULL);
    sgtMinimapGlobals->fEnabled = TRUE;
















    sgtMinimapGlobals->idMinimapDlg = UIDLG_MINIMAP_PVE;
    sgtMinimapGlobals->idMinimap = UIMAP_MINIMAP_PVE;
    sgtMinimapGlobals->idMinimapArrowRegion = UIWND_MINIMAP_PVE_ARROW_REGION;

    if (UI_TestID(sgtMinimapGlobals->idMinimap) && UI_TestID(sgtMinimapGlobals->idMinimapArrowRegion))
    {
        UI_GetMinimapFromID(sgtMinimapGlobals->idMinimap).Initialize(FALSE, sgtMinimapGlobals->idMinimapArrowRegion);
    }
}



void sRemoveMarkersForWorld(SWorldID idSWorld, MarkerInfoList * ptListMarkers)
{
    if (ptListMarkers == NULL)
        return;

    {
        uint32 nKey;
        POS pos;
        MarkerInfo tMarker;

        pos = ptListMarkers->GetStartAssoc();
        while (pos)
        {
            ptListMarkers->GetNextAssoc(pos, nKey, tMarker);


            if (tMarker.tPlace.idSWorld == idSWorld)
            {
                ptListMarkers->RemoveAt(nKey);
            }
        }
    }
}

void UIMinimap_DisposeForWorld(const SWorldID idSWorld)
{

    sRemoveMarkersForWorld(idSWorld, sgtMinimapGlobals->ptListServerMarkers);
    sRemoveMarkersForWorld(idSWorld, sgtMinimapGlobals->ptListPingMarkers);




    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    POS pos = tListMapScenes.GetHeadPosition();
    while (pos)
    {


        POS checkPos = pos;
        MapScene & tMapScene = tListMapScenes.GetNext(pos);
        if (tMapScene.idSWorld == idSWorld)
        {


            sFreeMapSceneData(tMapScene);
            tListMapScenes.RemoveAt(checkPos);
        }
    }
}


void UIMinimap_DisposeForGame()
{

    if (sgtMinimapGlobals == NULL)
        return;




    if (sgtAppGlobals.nGameGfxInitCount == 1)
    {

        UIMinimap_GfxDispose();
    }


    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    POS pos = tListMapScenes.GetHeadPosition();
    while (pos)
    {
        MapScene & tMapScene = tListMapScenes.GetNext(pos);


        sFreeMapSceneData(tMapScene);
    }








    sgtMinimapGlobals->tListMapScenes.RemoveAll();
    sgtMinimapGlobals->dwLastClearTick = 0;

    delete sgtMinimapGlobals->ptListMarkers; sgtMinimapGlobals->ptListMarkers = NULL;
    delete sgtMinimapGlobals->ptListServerMarkers; sgtMinimapGlobals->ptListServerMarkers = NULL;
    delete sgtMinimapGlobals->ptListPingMarkers; sgtMinimapGlobals->ptListPingMarkers = NULL;

    delete sgtMinimapGlobals; sgtMinimapGlobals = NULL;
}
void UIMinimap_EnterKnown(const ACDID idACD)
{
    if (sgtMinimapGlobals == NULL)
        return;






    ACDNetworkName ann = ANNFromACD(idACD);
    if (sgtMinimapGlobals->ptListMarkers->Find(ann))
        return;




    const ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return;





    ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
    switch (pdefActor->eType)
    {
    case ACTORTYPE_GIZMO:
        if (pdefActor == NULL || !TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_FORCE_MINIMAP_ICON))
        {



            GizmoType eGizmoType = ptACD->GetGizmoType();
            if (GizmoUsesStaticMinimapPing(eGizmoType))
                return;




            switch (eGizmoType)
            {
            case ACTORGIZMO_PORTAL:
                if (pdefActor == NULL || !TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_PORTAL_SETS_DYNAMIC_RETURN_LOCATION))
                    return;
                break;

            case ACTORGIZMO_UNDEFINED:
            case ACTORGIZMO_WAYPOINT:
            case ACTORGIZMO_CHECKPOINT:
            case ACTORGIZMO_TOWN_PORTAL:
            case ACTORGIZMO_HEARTH_PORTAL:
            case ACTORGIZMO_PORTAL_DESTINATION:
            case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
            case ACTORGIZMO_SECRET_PORTAL:
            case ACTORGIZMO_BOSS_PORTAL:
            case ACTORGIZMO_PLACED_LOOT:
            case ACTORGIZMO_SAVEPOINT:
            case ACTORGIZMO_DUNGEON_PORTAL:
            case ACTORGIZMO_RETURN_PORTAL:
                return;
            }
        }
        break;

    case ACTORTYPE_CLIENT_EFFECT:
    case ACTORTYPE_CRITTER:
    case ACTORTYPE_PLAYER:
    case ACTORTYPE_ITEM:
    case ACTORTYPE_AXE_SYMBOL:
    case ACTORTYPE_PROJECTILE:
        return;
    }

    RActorID idRActor = (RActorID)ptACD->idOwner;
    const RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor == NULL)
        return;



    MapScene * pMapScene = sFindRActorMapScene(ptRActor);
    if (pMapScene == NULL)
        return;




    sgtMinimapGlobals->ptListMarkers->AddTail_Tracked(ann);
}

static void sDeleteMarkerForANN(const ACDNetworkName ann)
{
    if (sgtMinimapGlobals == NULL)
        return;





    POS pos = sgtMinimapGlobals->ptListMarkers->Find(ann);
    if (pos)
    {
        sgtMinimapGlobals->ptListMarkers->RemoveAt(pos);
    }
}



void UIMinimap_ExitKnown(const ACDID idACD)
{
    if (sgtMinimapGlobals == NULL)
        return;






    sDeleteMarkerForANN(ANNFromACD(idACD));
}


























MarkerInfo MarkerInfoFromMessage(const MapMarkerInfoMessage * ptMsg)
{
    MarkerInfo tInfo;
    tInfo.tPlace = ptMsg->m_Place;
    tInfo.m_tTexture.snoTexture = ptMsg->m_ImageInfo;
    tInfo.m_tTexture.nFrame = 0;
    tInfo.m_bCanBecomeArrow = ptMsg->m_bCanBecomeArrow;
    tInfo.m_fMaxDisplayRangeSq = ptMsg->m_fMaxDisplayRangeSq;
    tInfo.m_fMinDisplayRangeSq = ptMsg->m_fMinDisplayRangeSq;
    tInfo.m_fDiscoveryRangeSq = ptMsg->m_fDiscoveryRangeSq;
    tInfo.m_bRespectsFoW = ptMsg->m_bRespectsFoW;
    tInfo.m_nPlayerUseFlags = ptMsg->m_nPlayerUseFlags;
    tInfo.m_flDiscoveryTime = 0.0f;
    tInfo.m_bIsPing = ptMsg->m_bIsPing;
    tInfo.m_snoQuestSource = ptMsg->m_snoQuestSource;

    if (tInfo.m_tTexture.snoTexture == SNO_NONE && ptMsg->m_hImage != 0)
    {
        if (!ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, ptMsg->m_hImage, tInfo.m_tTexture))
        {
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, ptMsg->m_hImage, tInfo.m_tTexture);
        }
    }

    if (tInfo.m_tTexture.snoTexture == GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_DEST_ICON) && !tInfo.m_bIsPing)
    {
        const float DESTINATION_MIN_DIST = 15.0f;
        tInfo.m_fMinDisplayRangeSq = DESTINATION_MIN_DIST * DESTINATION_MIN_DIST;
    }

    tInfo.m_snoStringList = ptMsg->m_snoStringList;
    tInfo.m_hLabel = ptMsg->m_hLabel;
    tInfo.m_snoKnownActorOverride = ptMsg->m_snoKnownActorOverride;
    tInfo.m_snoQuestSource = ptMsg->m_snoQuestSource;
    tInfo.m_bIsKeyWarden = FALSE;
    if (ActorIsKeywarden(tInfo.m_snoKnownActorOverride))
    {
        tInfo.m_bIsKeyWarden = TRUE;
        tInfo.m_fMaxDisplayRangeSq = 1250.0f * 1250.0f;
    }

    return tInfo;
}







void sAddMarker(MarkerInfo & tMarker, uint32 nKey)
{
    if (tMarker.m_bIsPing)
    {



        MarkerInfo tMarkerExisting;
        if (sgtMinimapGlobals->ptListPingMarkers->Lookup(nKey, tMarkerExisting))
        {
            tMarker.m_flDiscoveryTime = tMarkerExisting.m_flDiscoveryTime;
        }
        else
        {
            bool bQuestMarkerTutorial = tMarker.m_tTexture.snoTexture == GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_DEST_ICON) ||  // DWARF types this BOOL; only bool gives the original bytes
                                        tMarker.m_tTexture.snoTexture == GlobalSNOGet(GLOBALSNO_TEXURE_MARKER_QUEST_RING);
            if (Prefs::GetGameplayPrefs().GetShowBreadCrumb() && bQuestMarkerTutorial)
            {
                gtTutorialMgr.ShowTutorial(UIWND_MINIMAP_TUTORIAL_ANCHOR, GlobalSNOGet(GLOBALSNO_TUTORIAL_MINIMAP_PING), NULL, TRUE);
            }
        }





        sgtMinimapGlobals->ptListPingMarkers->SetAt_Tracked(nKey, tMarker);
    }
    else
    {
        sgtMinimapGlobals->ptListServerMarkers->SetAt_Tracked(nKey, tMarker);
    }
}




void sRemoveMarker(uint32 nKey, BOOL bIsPing)
{
    MarkerInfo tMarker;
    if (!bIsPing)
    {
        if (sgtMinimapGlobals->ptListServerMarkers->Lookup(nKey, tMarker))
        {
            sgtMinimapGlobals->ptListServerMarkers->RemoveAt(nKey);
        }
    }
    else
    {
        if (sgtMinimapGlobals->ptListPingMarkers->Lookup(nKey, tMarker))
        {
            sgtMinimapGlobals->ptListPingMarkers->RemoveAt(nKey);
        }
    }
}





void UIMinimap_AddMarker(const MapMarkerInfoMessage * ptMsg)
{
    if (sgtMinimapGlobals == NULL)
        return;

    MarkerInfo tInfo = MarkerInfoFromMessage(ptMsg);
    if (ptMsg->m_bActive)
    {
        sAddMarker(tInfo, ptMsg->m_nHashedName);
    }
    else
    {
        sRemoveMarker(ptMsg->m_nHashedName, tInfo.m_bIsPing);
    }
}





void UIMinimap_SetObjective(const MapMarkerInfoMessage * ptMsg)
{
    if (sgtMinimapGlobals == NULL)
        return;




    sgtMinimapGlobals->tObjectiveMarker = MarkerInfoFromMessage(ptMsg);
}


void UIMinimap_SetObjectiveCone(const ActorCommonData * ptACD)
{
    if (ptACD == NULL)
        return;

    MarkerInfo & tObjectiveMarker = sgtMinimapGlobals->tObjectiveMarker;
    if (tObjectiveMarker.tPlace.idSWorld == SWORLDID_INVALID)
        return;







    SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();
    if (tObjectiveMarker.tPlace.idSWorld != idSWorldCurrent)
        return;




    UIMinimap & tLocalMap = UI_GetMinimapFromID(UIDLG_MAP_LOCALMAP);
    tLocalMap.UpdateTransform();

    Vector2D vecObjectiveUIC, vecPlayerUIC;

    tLocalMap.sWorldToMapUIC(ptACD->GetPlace().wp.As2D(), vecPlayerUIC);
    tLocalMap.sWorldToMapUIC(tObjectiveMarker.tPlace.wp.As2D(), vecObjectiveUIC);

    Vector2D vecPlayerToTarget = vecObjectiveUIC - vecPlayerUIC;

    Angle aPlayerToTarget = vecPlayerToTarget.ToYaw();




    Angle aConeAngle = aPlayerToTarget + (RandFloat_Internal(CGameGlobalsGet().rngLocal) - 0.5f) * Math::DegToAngle(OBJECTIVE_CONE_ANGLE);

    sgtMinimapGlobals->arObjectiveCones[sgtMinimapGlobals->nCurrentObjectiveCone].tPlace = ptACD->GetPlace();
    sgtMinimapGlobals->arObjectiveCones[sgtMinimapGlobals->nCurrentObjectiveCone].aPlayerToTarget = aConeAngle;
    sgtMinimapGlobals->nCurrentObjectiveCone = (sgtMinimapGlobals->nCurrentObjectiveCone + 1) % NUM_OBJECTIVE_CONES;
}
















































































































































void UIMinimapToggle(const UIID &)
{
    if (sgtMinimapGlobals != NULL)
        sgtMinimapGlobals->fEnabled = !sgtMinimapGlobals->fEnabled;
}






void UIMinimap_DisplayANNName(ACDNetworkName ann, float flSeconds)
{


    if (sgtMinimapGlobals == NULL)
        return;



    Player * ptPlayer = PlayerGetByANN(ann);
    if (ptPlayer == NULL)
        return;



    uint32 dwNow = GameGetServerTime();
    ptPlayer->tClientInfo.dwNameDisplayEnd = dwNow + (uint32)(flSeconds * TICKS_PER_SECOND);
    ptPlayer->tClientInfo.dwFadeOutTime = ptPlayer->tClientInfo.dwNameDisplayEnd + 2 * TICKS_PER_SECOND;
}


static void sRevealMapAtLocation(
    const WorldPlace & tPlace,
    const float flRadius)
{
    Vector2D wpLocClear = tPlace.wp.As2D();


    MapSceneList & tListMapScenes = sgtMinimapGlobals->tListMapScenes;
    POS pos = tListMapScenes.GetHeadPosition();
    while (pos)
    {
        MapScene & tMapScene = tListMapScenes.GetNext(pos);




        if (tPlace.idSWorld == tMapScene.idSWorld)
        {

            sUpdateSceneVisibility(tMapScene, wpLocClear, 1 << EXPLORED_SEEN_BIT, flRadius);
        }
    }
}






void UIMinimap_ShowForUpdate()
{
    sgtMinimapGlobals->nLastShownTime = CGameGetRealTicks();
}
