// X/Client/CPlayer.cpp
// Functions sit on their original line numbers.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CPlayer.h"
#include "CGame.h"
#include "Attachments.h"
#include "Gfx/GfxVB.h"
#include "UserInterface/UIID.h"
#include "../Common/ActorCommonData.h"
#include "../Common/CommonTypes.h"
#include "../Common/GameBalance.h"
#include "../Common/ImageHandle.h"
#include "../Common/ItemMailCommon.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/DataStructures/XRingBuffer.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/RefString.h"
#include "../Sigma/UnicodeText.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../Common/GlobalSNO.h"
#include "../Common/GameTemplate.h"
#include "../Common/Monsters.h"
#include "Sound/Sound.h"
#include "../Common/AnimPlayer.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Definitions/StringListDefinition.h"
#include "../Common/UserInterface/UITextControl.h"
#include "../Common/UserInterface/FontDefinition.h"
#include "RWindow.h"
#include "UserInterface/Interface/UIFade.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/Protobuf/Account.pb.h"
#include "Console/GamerProfile.h"
#include "Console/Online.h"
#include "Definitions/PreferencesDefinition.h"
#include "RActors/RActors.h"
#include "UserInterface/UI.h"
#include "UserInterface/Console/ConsoleScreenManager.h"
#include "../Common/VisualInventory.h"
#include "Gfx/GfxCommon.h"
#include "UserInterface/Controls/UIItemTagControl.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "Font/Font.h"
class UIItemTag;
namespace D3 { namespace GameMessage {  // X/Common/Protobuf/GameMessage.pb.h
class MailContents;
class MailOperationResponse;
} }
#include "Font/FormattedText.h"
struct ItemPickupText {  // size 0x48
    RActorID idRActor;  // 0x00
    ImageHandle hImage;  // 0x04
    RGBAColor rgbaQualityColor;  // 0x08
    BOOL bHighPriority;  // 0x0c
    int32 nPlayerOwnerIndex;  // 0x10
    float flUnitAlpha;  // 0x14
    Rect2D rectUICIcon;  // 0x18
    Rect2D rectUICText;  // 0x28
    BOOL fDraw;  // 0x38
    UIItemTag * ptItemTag;  // 0x40
};
static_assert(sizeof(ItemPickupText) == 0x48, "ItemPickupText");

typedef ItemPickupText ItemPickupTextList[128];

struct ItemPickupGlobals {  // size 0x2410
    ItemPickupTextList listItemText;  // 0x00
    int32 nItemTextCount;  // 0x2400
    UIID idHighlightedTag;  // 0x2408
};
static_assert(sizeof(ItemPickupGlobals) == 0x2410, "ItemPickupGlobals");

struct FloatingTextBase {  // size 0x58
    FloatingNumberType eType;  // 0x00
    ParticleSystemID idParticleSystem;  // 0x04
    WorldPlace wpPlace;  // 0x08
    CRefString sUnicodeText;  // 0x18
    Vector3D vecUIC;  // 0x30
    uint32 dwBornTime;  // 0x3c
    uint32 dwEnteredTime;  // 0x40
    RGBAColor rgbaColor;  // 0x44
    float flUnitAlpha;  // 0x48
    float flScale;  // 0x4c
    BOOL fVisible;  // 0x50
};
static_assert(sizeof(FloatingTextBase) == 0x58, "FloatingTextBase");

class FloatingText : public FloatingTextBase {  // size 0x68
public:
    int32 nControllerIndex;  // 0x54
    int32 nAmount;  // 0x58
    int32 nAmount2;  // 0x5c
    GBID gbidItem;  // 0x60
};

typedef XGrowableList<FloatingText, const FloatingText &> FloatingTextList;

class FloatingDamageNumber : public FloatingTextBase {  // size 0x60
public:
    RActorID idRActor;  // 0x54
    float flDamageAmount;  // 0x58
};

typedef XGrowableList<FloatingDamageNumber, const FloatingDamageNumber &> FloatingDamageNumbersList;

struct PingInfo {  // size 0x40
    uint32 uPingSentTime;  // 0x00
    uint32 uPingSequence;  // 0x04
    XRingBuffer<unsigned int, unsigned int, 5> tLatency;  // 0x08
};
static_assert(sizeof(PingInfo) == 0x40, "PingInfo");

class LargeDamageTracker {  // size 0x14
public:
    LargeDamageTracker();
    void Reset();
    bool ProcessDamage(float flDamageAmount);
private:
    float flLargestDamageSeen;  // 0x00
    uint32 dwLargeDamageTimestamp;  // 0x04
    uint32 dwAnyDamageTimestamp;  // 0x08
    uint32 dwTicksSinceLastDecay;  // 0x0c
    int32 nNumDamageNumbersToIgnore;  // 0x10
    static const float MIN_LARGE_DAMAGE_AMOUNT;
    static const float DECAY_PERCENTAGE;
    static const float DAMAGE_INCREASE_PERCENTAGE;
    static const uint32 RESET_TIME;
    static const int32 NUM_DAMAGE_NUMBER_THRESHOLD;
};
static_assert(sizeof(LargeDamageTracker) == 0x14, "LargeDamageTracker");

const int32 LargeDamageTracker::NUM_DAMAGE_NUMBER_THRESHOLD = 10;

struct CPlayerGlobals {  // size 0x2678
    int32 nLocalPlayers[4];  // 0x00
    uint32 uNumLocalPlayers;  // 0x10
    ItemPickupGlobals tItemPickup;  // 0x18
    FloatingDamageNumbersList listFloatingDamageNumbers;  // 0x2428
    FloatingTextList listFloatingText;  // 0x2480
    CalloutEventList listCallouts;  // 0x24d8
    uint32 dwNextTimeCanSayError;  // 0x2530
    uint32 dwNextTimeCanSayResourceError;  // 0x2534
    uint32 dwResourceErrorCount;  // 0x2538
    uint32 dwGameTickOfLastPaneOpen;  // 0x253c
    RActorID idRActorLastUsedWaypoint;  // 0x2540
    uint32 dwNextTimeCanUsePortals;  // 0x2544
    uint32 dwFramesTillCanUsePortals;  // 0x2548
    PingInfo tPingInfo;  // 0x2550
    uint32 uLastPingTime;  // 0x2590
    uint32 dwShowItemsOnGroundStart;  // 0x2594
    uint8 bBusy;  // 0x2598
    MailRecordMap tMailMap;  // 0x25a0
    FriendGiftData tPlayerGiftInfo[4];  // 0x25c0
    FriendGiftData tAvengerGiftInfo;  // 0x2640
    LargeDamageTracker tLargeDamageTracker;  // 0x2660
    uint32 dwLastDelayedFloatingNumber;  // 0x2674
};
static_assert(sizeof(CPlayerGlobals) == 0x2678, "CPlayerGlobals");
// X/Common/Networking/GameMessage.h (also declared locally by X/Server/LuaScript/ScriptUtil.cpp)
#include "ImageCollection.h"
// Inference: local placement; signature Font.cpp:889 (declared at 171), original declaration header unavailable.
const uint32 PING_INTERVAL_MS = 5000;  // DWARF CPlayer.cpp:170 const_value 5000
void FontStringDrawCenteredUIC(const TextRenderProperties &, const FormattedText &, const Vector2D &, const RGBAColor &, const RGBAColor *, const float, const RenderLayer, BOOL);
// Inference: local declaration placement; const RGBAColor definition is Font.cpp:47 in DWARF.
extern const RGBAColor crgbaTextShadow;

// X/Common/Player.h:1058
struct LogoutContextMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    LogoutContext eLogoutContext;  // 0x08
};
static_assert(sizeof(LogoutContextMessage) == 0xc, "LogoutContextMessage");

typedef int32 PlayerWarpReason;  // X/Common/World.h
typedef int32 ActorAnimationTag;  // X/Common/TagMaps/AnimTagsSwig.h

// Functions other files define; their headers are not in src/ yet or do not declare them.
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, int32 nControllerIndex);  // X/Client/Network/CGameMessage.cpp
CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound);  // X/Common/Player.cpp
void PlayerSetDefaults(Player & tPlayer);  // X/Common/Player.cpp
void InputSetPlayerController(Player * ptPlayer, int32 nControllerIndex);  // X/Client/Input
namespace Console { namespace Online { void LeaderboardPerformHealing(int32 nControllerIndex); } }  // X/Client/Console
void ItemSetIsBeingManipulated(ActorCommonData & tACDItem, BOOL bManipulated, uint32 dwTicks);  // X/Common/Items.cpp
void VisualInventoryApply(RActor * ptRActor, VisualInventory & tVisualInventory, VisualInventoryType eVisualInventoryType, PlayerClass ePlayerClass, BOOL fFemale);  // X/Client/CVisualInventory.cpp
void PlayersSetMainPlayerRActor(Player * ptPlayer, RActorID idRActor);  // X/Client/CPlayerInput.cpp:6776
void CPlayerInputSetAnimSeed(Player * ptPlayer, const uint32 dwSyncedAnimSeed);  // X/Client/CPlayerInput.cpp:6857
SNO WorldGetLevelAreaSNOFromRActor(RActorID idRActor);  // X/Client/CWorld.cpp:1075
BOOL CPlayerUtilPlayerSpecifiedDontMove(const Player * ptPlayer, BOOL bPrimaryActionKey, BOOL bHasActorTarget);  // X/Client/CPlayerUtil.cpp:1137
namespace UICallouts { void NewCalloutEvent(const CalloutEvent & tEvent); }  // X/Client/UserInterface/Interface/UICallouts.cpp
namespace UIWaypointMap { void CancelTutorialWasInterrupted(); }  // X/Client/UserInterface/Console/UIConsoleWaypointMap.cpp:1301
namespace Console { namespace Online {
void PartyRemovePlayer(int32 nControllerIndex);  // X/Client/Console/Online.cpp:3766
BOOL PresenceHasAnyFriends(int32 nUserIndex);  // X/Client/Console/PresenceManager.cpp:4121
BOOL PresenceGetRandomFriend(int32 nUserIndex, Friend & tOutFriend, unsigned long idBlacklistPlayer);  // X/Client/Console/PresenceManager.cpp:4160 (bdOnlineUserID)
} }
namespace Console { namespace GamerProfile { void SignOut(int32 nUserIdx); } }  // X/Client/Console/GamerProfileNX64.cpp:630
namespace Console { namespace DLCManager { uint32 GetLicenses(); } }  // X/Client/Console/DLCManager.cpp:385
namespace UIUtils { namespace Console { RGBAColor GetPlayerColor(const int32 nPlayerIndex); } }  // X/Client/UserInterface/Console/UIConsoleUtils.cpp:370
void UI_Print(LPCSTR szText, BOOL bChat);  // X/Client/UserInterface/UI.cpp
namespace UIFastEquip { namespace Console { void Close(); } }  // X/Client/UserInterface/Console/UIConsoleFastEquip.cpp:1335
void UIMinimapUpdateZoom(const BOOL bSnapZoomLevel);  // X/Client/UserInterface/Interface/UIMinimap.cpp:4578
namespace Weather { void Warped(RWindow & tRWindow); }  // X/Client/Weather.cpp:1600
void CGameModeResetCameraToFollowPlayer();  // X/Client/CGameMode.cpp
void RActorRemoveEffectParticle(RActor & tRActor, FastAttribKey tKey);  // X/Client/RActors/RActors.cpp:3983
void CTrickleRemoveAll();  // X/Client/Network/CTrickle.cpp:347
namespace UIEventQueue { namespace Console {  // X/Client/UserInterface/Console/UIEventQueueConsole.cpp
void DisplayMessageWithIconForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tIcon, CRefString * psDetail, int32 nArg4, int32 nArg5);
} }
// Declared in X/Client/UserInterface/Interface/UITutorials.h (not adopted yet); only the
// member this file calls.
class UITutorial;
#include "UserInterface/Interface/UITutorials.h"
#include "../Sigma/Math/Ray.h"
#include "Particle.h"
#include "Camera/CameraManager.h"
extern UITutorialManager gtTutorialMgr;
enum { CMSG_PLAYER_GIFT = 522 };  // X/Common/Networking/GameMessage.h (invented name; id from the call)
// Inference: local declaration placement; UIGlobals.cpp:4220 supplies this DWARF-owned UIID.
extern UIID UIDLG_ITEMTAGS;
// Inference: declaration placement/header ownership; signature from GfxPrimitives.cpp:966.
void GfxDrawSolidWedgeDeferred(LineSeg3D, Angle, int, Vector3D, RGBAColor, RGBAColor, GlobalSNO, const RenderContext);
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/TagMaps/Tags.h"
#include "UserInterface/Interface/UIConversations.h"
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);  // X/Client/UserInterface/UI.cpp:2866; the template calls _Z16UI_DeleteControlR4UIIDi (0x710006874c)
// X/Client/UserInterface/UI.h:550; src/'s UI.h does not have this template yet.
template <typename T>
inline void UI_DeleteControl(T *& ptControl)
{
    if (ptControl)
    {
        UIID id(ptControl->GetID());
        UI_DeleteControl(id, TRUE);
        ptControl = NULL;
    }
}
struct ClientTrickleEntry {  // size 0x68; X/Client/Network/CTrickle.h:22
    ACDNetworkName ann;  // 0x00
    SNO snoActor;  // 0x04
    WorldPlace tPlace;  // 0x08
    int32 nPlayerIndexOwner;  // 0x18
    SNO snoLevelArea;  // 0x1c
    float flHealthPercent;  // 0x20
    int32 eType;  // 0x24 (TrickleInfoType)
    uint32 dwTrickleFlags;  // 0x28
    ImageTextureFrame tIcon;  // 0x2c
    SNO snoMinimapStringlist;  // 0x34
    uint32 hStringListLabel;  // 0x38
    float fMinMinimapDistSq;  // 0x3c
    float fMaxMinimapDistSq;  // 0x40
    float flLastInjuredTime;  // 0x44
    int32 dwHeadstoneCorpseReviveTime;  // 0x48
    uint32 uEngagedWithRareTime;  // 0x4c
    uint32 uEngagedWithGoblinTime;  // 0x50
    BOOL bInCombat;  // 0x54
    int32 nTieredRiftRank;  // 0x58
    BOOL bBrawling;  // 0x5c
    BOOL bInUberFight;  // 0x60
    int32 gbidPortraitFrame;  // 0x64
};
static_assert(sizeof(ClientTrickleEntry) == 0x68, "ClientTrickleEntry");
typedef XPooledList<ClientTrickleEntry, const ClientTrickleEntry &> ClientTrickleList;  // X/Client/Network/CTrickle.h:52
ClientTrickleList & CTrickleGetList();  // X/Client/Network/CTrickle.cpp
// Reset and ProcessDamage restored to DWARF decl lines; declaration-only material is below.
inline void LargeDamageTracker::Reset()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    flLargestDamageSeen = 0.0f;
    // Inference: original merged zero stores clear all three timer fields at line279.
    dwLargeDamageTimestamp = dwAnyDamageTimestamp = dwTicksSinceLastDecay = 0;


    nNumDamageNumbersToIgnore = NUM_DAMAGE_NUMBER_THRESHOLD;
}

inline bool LargeDamageTracker::ProcessDamage(  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    float flDamageAmount)
{
    uint32 dwNow = CGameGetClientTicks();
    uint32 dwTicksSinceLastLargeDamage = dwNow - dwLargeDamageTimestamp;

    if (dwTicksSinceLastLargeDamage > RESET_TIME)
    {


        Reset();
    }
    else
    {
        dwTicksSinceLastDecay += dwNow - dwAnyDamageTimestamp;


        if (dwTicksSinceLastDecay)
        {
            float flSecondsSinceLastDecay = float(dwTicksSinceLastDecay) / 60.0f;
            int32 nSeconds = Math::FloatFloorToInt(flSecondsSinceLastDecay);
            for (int32 i = 0; i < nSeconds; ++i)
            {
                flLargestDamageSeen *= DECAY_PERCENTAGE;
            }

            dwTicksSinceLastDecay -= nSeconds * 60;
        }
    }


    dwAnyDamageTimestamp = dwNow;
    // Inference: original initializes false, only threshold/new-record branch replaces it.
    bool bNewLargestDamageAmount = false;
    if (flDamageAmount > MIN_LARGE_DAMAGE_AMOUNT && flDamageAmount > flLargestDamageSeen)
    {

        bNewLargestDamageAmount = flDamageAmount > flLargestDamageSeen * DAMAGE_INCREASE_PERCENTAGE;


        flLargestDamageSeen = flDamageAmount;
        dwLargeDamageTimestamp = dwNow;


        dwTicksSinceLastDecay = 0;
    }

    if (nNumDamageNumbersToIgnore)
    {
        --nNumDamageNumbersToIgnore;
        return false;
    }

    return bNewLargestDamageAmount;
}

// Inference: forward-declaration placement; these file statics are defined at their DWARF
// lines 3564, 3629, 3710 and 3801, after their caller CPlayersUpdate (1057).
static void sFloatingDamageNumbersUpdate();
static void sFloatingTextUpdate();
static void sCalloutEventsUpdate();
static void sUpdateBusyStatus();
static void sItemPickupTextUpdate();  // Inference: same placement reason; defined at DWARF line 4711, called at 1053.

// Inference: definitions placed at DWARF member declaration lines; values from original comparisons/constant loads.
const float LargeDamageTracker::MIN_LARGE_DAMAGE_AMOUNT = 10000.0f;
const float LargeDamageTracker::DECAY_PERCENTAGE = 0.97f;
const float LargeDamageTracker::DAMAGE_INCREASE_PERCENTAGE = 1.1f;
const uint32 LargeDamageTracker::RESET_TIME = 600;

#include "Definitions/CAttachmentDefinition.h"
// Inference: declaration placement; exact signature is RActorAttachments.cpp2248.
void RActorGetHardpointTransformByIndex(const RActor &, const int32, PRTransform &);

#include "Definitions/AppearanceDefinition.h"
// Inference: local declaration placement; signatures from RActors.cpp3095, RActorAttachments.cpp2325/2315, Attachments.cpp897/106.
void RActorChangeLookIndex(RActor &, const int32, const BOOL);
AttachmentID RActorAddAttachment(RActor &, const AttachmentType, const SNO, const uint32, HardpointHash, BOOL);
void RActorClearActorAttachment(RActor &, const uint32);
Attachment * AttachmentFind(XPooledDataIDList &, const uint32);
Attachment * AttachmentTryToGet(const AttachmentID);
#include "ParticleInternal.h"
#include "UserInterface/Interface/UIGame.h"
BOOL UISideScreenAnyAreVisible();  // Inference: local declaration placement; X/Client/UserInterface/UIMasterIdList.cpp:352
void ParticleSystemDelete(const ParticleSystemID idParticleSystem);  // Inference: local declaration placement; X/Client/Particle.cpp:2191
BOOL PlayerIsEngagedWithRare(uint32 uEngagedTime);  // X/Common/Player.cpp
BOOL PlayerIsEngagedWithTreasureGoblin(uint32 uEngagedTime);  // X/Common/Player.cpp
void PlayerPetInitialize(PlayerPet & tPet);  // X/Common/Player.cpp:2547
struct SceneCommonData;
SceneCommonData * SceneCommonDataTryToGet(SSceneID idSScene);  // X/Common/SceneCommon.cpp
SNO SceneCommonGetSceneSNO(SSceneID idSScene);  // X/Common/SceneCommon.cpp:1881
SNO WorldCommonGetWorldSNO(SWorldID idSWorld);  // X/Common/WorldCommon.cpp
Act GameGetCurrentAct();  // X/Common/GameTemplate.cpp
#include "../Common/Quests/Quests.h"
#include "../Common/UserInterface/UIInventory.h"
#include "UserInterface/Interface/UIToolTipManager.h"
#include "SNOPreload.h"
#include "../Common/ChallengeRift.h"
#include "../Common/Shell/CmdLine.h"
#include "UserInterface/Console/ConsoleModalNotificationManager.h"
QuestInstance * GetLeadingMainLineQuest();  // X/Common/Quests/Quests.cpp
namespace UIParagonBonuses { namespace Console { void OnPlayerLeveled(const Player * ptPlayer); } }  // X/Client/UserInterface/Console/UIConsoleParagon.cpp:400
// Inference: local declaration placement; signatures from the indicated DWARF definitions.
void PlayerSetACD(Player & tPlayer, const ACDID idACD); // X/Common/Player.cpp:1438
void RActorUpdateOcclusionManager(RActor * ptOwnerRActor); // RActors/RActors.cpp:849
void ACDUpdateAppearance(ACDID idACD); // CActorCommonData.cpp:81
void ResetTradeSessionData(Player * ptPlayer); // X/Common/Trade.cpp:225
namespace Console { namespace Notifications {
BOOL HostMigrationLoadscreenTipEnabled(); // ConsoleScreenManager.cpp:2985
void EnableHostMigrationLoadscreenTip(BOOL bFlag); // ConsoleScreenManager.cpp:2977
} }
#include "../Common/Items.h"
// Inference: local declaration placement; DWARF CItems.cpp:90 and CActorInventory.cpp:79 signatures.
RGBAColor ItemGetCraftingMatDisplayNameColor(GBID gbidItem);
ImageHandle ACDInventoryGetImageHandleFromGBID(const GBID gbidItem, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale);
#include "Targeting/ConsoleTargeting.h"
#include "../Common/Definitions/PowerDefinition.h"
// Inference: declaration placement; signatures ConsoleTargeting.cpp1112 and GfxPrimitives.cpp920.
namespace Targeting { namespace Console { void GetTargetingInformation(const TargetingInformationInput &, TargetingInformationOutput &); } }
void GfxSolidCircleDrawDeferred(Vector3D, Vector3D, float, int, RGBAColor, GlobalSNO, const RenderContext);
// Inference: declaration placement; signature from UIUtils.cpp845.
CRefString FormatTruncatedNumber(const float, const BOOL, const BOOL);
#include "CWorld.h"
// Inference: local declaration placement; signature is DWARF RActors.cpp:12229.
BOOL RActorIsVisibleToLocalPlayer(const RActor * ptRActor);



void CPlayerInitializeForApp()
{


    CPlayerGlobals *& ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    ptCPlayerGlobals = (CPlayerGlobals *)SigmaMemoryNew(sizeof(CPlayerGlobals), 4, NULL, TRUE);

    for (uint32 i = 0; i < 4; i++)
    {
        ptCPlayerGlobals->nLocalPlayers[i] = -1;

        ptCPlayerGlobals->tPlayerGiftInfo[i].idRecipient = 0;
        ptCPlayerGlobals->tPlayerGiftInfo[i].sNameRecipient = CRefString("NONE");
    }

    ptCPlayerGlobals->uNumLocalPlayers = 1;

    ptCPlayerGlobals->listFloatingDamageNumbers.Initialize(NULL);
    ptCPlayerGlobals->listFloatingText.Initialize(NULL);
    ptCPlayerGlobals->listCallouts.Initialize(NULL);
    ptCPlayerGlobals->tMailMap = MailRecordMap();


    ptCPlayerGlobals->tAvengerGiftInfo.idRecipient = 0;
    ptCPlayerGlobals->tAvengerGiftInfo.sNameRecipient = CRefString("NONE");

    new (&ptCPlayerGlobals->tPingInfo.tLatency) XRingBuffer<unsigned int, unsigned int, 5>(TRUE);
}



void CPlayerDisposeForApp()
{


    CPlayerGlobals *& ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    ptCPlayerGlobals->listCallouts.Dispose();
    ptCPlayerGlobals->listFloatingText.Dispose();
    ptCPlayerGlobals->listFloatingDamageNumbers.Dispose();
    ptCPlayerGlobals->tMailMap.~MailRecordMap();

    XFREE(ptCPlayerGlobals, NULL);
}



void CPlayerInitializeForGame()
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;



    for (int32 i = 0; i < 4; i++)
        ptCPlayerGlobals->nLocalPlayers[i] = -1;

    ptCPlayerGlobals->uNumLocalPlayers = 0;

    ptCPlayerGlobals->dwNextTimeCanSayError = 0;
    ptCPlayerGlobals->dwNextTimeCanSayResourceError = 0;
    ptCPlayerGlobals->dwResourceErrorCount = 0;
    ptCPlayerGlobals->dwGameTickOfLastPaneOpen = 0;
    ptCPlayerGlobals->bBusy = FALSE;


    ptCPlayerGlobals->idRActorLastUsedWaypoint = RACTORID_INVALID;
    ptCPlayerGlobals->dwNextTimeCanUsePortals = 0;

    ptCPlayerGlobals->dwFramesTillCanUsePortals = 0;
    ptCPlayerGlobals->dwShowItemsOnGroundStart = 0;

    ptCPlayerGlobals->uLastPingTime = Blizzard::Time::Milliseconds();

    ptCPlayerGlobals->tPingInfo.tLatency.Empty();
    ptCPlayerGlobals->tPingInfo.uPingSentTime = 0;
    ptCPlayerGlobals->tPingInfo.uPingSequence = 0;

    ptCPlayerGlobals->tMailMap.clear();

    ptCPlayerGlobals->dwLastDelayedFloatingNumber = 0;

    ptCPlayerGlobals->tLargeDamageTracker.Reset();
}


void CPlayerUIInitializeForGame()
{
    // Inference: original allocates 128 inactive tags, at (0,0) with dimensions (10,10).
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    UIID idParent = UIDLG_ITEMTAGS;

    ItemPickupTextList & listItemText = ptCPlayerGlobals->tItemPickup.listItemText;
    for (int32 i = 0; i < 128; i++)
    {
        ButtonCreationParams tParams(UIT_ITEMTAG, CRefString().Format("ItemTag_%d", i), idParent);
        tParams.fActive = FALSE;
        tParams.vecTextButtonPressOffsetUIC.Set(0.0f, 0.0f);

        UIItemTag & tItemTag = *new UIItemTag(tParams, Vector2D(0.0f, 0.0f), Vector2D(10.0f, 10.0f));
        listItemText[i].ptItemTag = &tItemTag;
    }
    // Inference: highlighted tag uses original name, white text/highlight and black drop shadows.
    ButtonCreationParams tParams(UIT_ITEMTAG, "HighlightedItemTag", idParent);
    tParams.fActive = FALSE;
    tParams.vecTextButtonPressOffsetUIC.Set(0.0f, 0.0f);
    tParams.rgbaText = crgbaWhite;
    tParams.rgbaDropShadow = crgbaBlack;
    tParams.rgbaTextHighlight = crgbaWhite;
    tParams.rgbaTextHighlightDropShadow = crgbaBlack;
    UIItemTag & tItemTag = *new UIItemTag(tParams, Vector2D(0.0f, 0.0f), Vector2D(10.0f, 10.0f));
    ptCPlayerGlobals->tItemPickup.idHighlightedTag = tItemTag.GetID();
}




void CPlayerDisposeForGame()
{
}








void CPlayerUIDisposeForGame()
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    ItemPickupTextList & listItemText = ptCPlayerGlobals->tItemPickup.listItemText;

    for (int32 i = 0; i < 128; i++)
    {
        UI_DeleteControl(listItemText[i].ptItemTag);
    }
}




void CPlayerReset()
{


    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;

    ptCPlayerGlobals->tItemPickup.nItemTextCount = 0;
    ptCPlayerGlobals->listFloatingDamageNumbers.RemoveAll();
    ptCPlayerGlobals->listFloatingText.RemoveAll();
    ptCPlayerGlobals->listCallouts.RemoveAll();
    ptCPlayerGlobals->tLargeDamageTracker.Reset();
}


static void sUpdatePing()
{





    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    uint32 uNow = Blizzard::Time::Milliseconds();

    if (ptCPlayerGlobals->uLastPingTime == 0 || uNow - ptCPlayerGlobals->uLastPingTime >= PING_INTERVAL_MS)
    {
        DWordDataMessage tMessage;
        tMessage.dwData = ++ptCPlayerGlobals->tPingInfo.uPingSequence;
        MessageSendToServer(CMSG_PING, &tMessage, sizeof(tMessage), LocalPlayerGet());

        ptCPlayerGlobals->uLastPingTime = uNow;
        ptCPlayerGlobals->tPingInfo.uPingSentTime = uNow;
    }
}















void CPlayerHandlePong(uint32 uPingSequence)
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    PingInfo & tPingInfo = ptCPlayerGlobals->tPingInfo;

    if (tPingInfo.uPingSequence == uPingSequence) {
        uint32 uNow = Blizzard::Time::Milliseconds();
        uint32 uDeltaMS = uNow - tPingInfo.uPingSentTime;




        tPingInfo.tLatency.Enqueue(uDeltaMS);

        float flCurrMS = (float)CPlayerGetLatencyMsec();
        CGameGlobalsGet().flLagCompensationTicks = flCurrMS * 60.0f / 1000.0f * 0.5f;
    }
}





uint32 CPlayerGetLatencyMsec()
{
    uint32 uTotal = 0;
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    PingInfo & tPingInfo = ptCPlayerGlobals->tPingInfo;

    size_t count = tPingInfo.tLatency.Count();
    if (count == 0)
        return 0;

    for (size_t i = 0; i < count; i++) {
        uTotal += tPingInfo.tLatency[i];
    }

    return uTotal / (uint32)count;
}



































void sSendItemMoveRequests(
    Player & tPlayer)
{
    uint32 dwNow = CGameGetClientTicks();
    if (!GameIsSoftPaused() && dwNow - tPlayer.tClientInfo.dwLastItemMoveProcessed < 30)
    {
        return;
    }
    // Inference: original batches all queued messages after 30 ticks or while soft-paused.
    POS pos = tPlayer.tClientInfo.listQueuedItemMoves.GetHeadPosition();
    while (pos)
    {
        InventoryRequestMoveMessage tMessage = tPlayer.tClientInfo.listQueuedItemMoves.GetNext(pos);
        MessageSendToServer(CMSG_TRY_INVENTORY_MOVE, &tMessage, sizeof(tMessage), &tPlayer);
    }

    tPlayer.tClientInfo.listQueuedItemMoves.RemoveAll();
    tPlayer.tClientInfo.dwLastItemMoveProcessed = dwNow;
}





























































































int32 PlayerIndexToLocalPlayerIndex(const int32 nPlayerIndex)
{
    int32 nLocalPlayerIndex = -1;
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->nPlayerIndex == nPlayerIndex) {
            nLocalPlayerIndex = ptPlayer->nControllerIndex;
            break;
        }
    }
    return nLocalPlayerIndex;
}

















int32 PlayerLocalIndexToPlayerIndex(const int32 nLocalPlayerIndex)
{
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->nControllerIndex == nLocalPlayerIndex)
            return ptPlayer->nPlayerIndex;
    }
    return -1;
}

















ACDID LocalPlayersACDGetByIndex(const int32 nLocalPlayerIndex)
{
    int32 nCount = 0;
    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->nControllerIndex == nLocalPlayerIndex)
            return ptPlayer->idPlayerACD;
    }
    return ACDID_INVALID;
}




























































































































































void CPlayersAddNecroCorpse(ActorCommonData * ptACDCorpse)

{
    if (!ptACDCorpse->IsNecroCorpse())
    {
        return;
    }

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->ePlayerClassPortrait == PLAYER_CLASS_NECROMANCER)
        {
            ptPlayer->listNecroCorpses.push_back(ptACDCorpse->id);
        }
    }
}


void CPlayersRemoveNecroCorpse(ActorCommonData * ptACDCorpse)

{
    if (!ptACDCorpse->IsNecroCorpse())
    {
        return;
    }

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->ePlayerClassPortrait == PLAYER_CLASS_NECROMANCER)
        {
            ACDID * iter = blz::find(ptPlayer->listNecroCorpses.begin(), ptPlayer->listNecroCorpses.end(), ptACDCorpse->id);
            if (iter != ptPlayer->listNecroCorpses.end())
            {
                ptPlayer->listNecroCorpses.erase(iter);
            }
        }
    }
}


void CPlayersFixedUpdate()
// Inference: prologue rows are at 1051, so the opening brace sits there.
{

    sItemPickupTextUpdate();
}


void CPlayersUpdate()
{
    // Inference (1061): only the CmdLineGetParams call survives; the test-type read follows InputNX64.cpp:607.
    GameCommonData & tGCD = CGameGetGameCommonData();
    const bool bIsTesting = CmdLineGetParams()->eTestType != 0;

    if (tGCD.GameModeGet() != GAMEMODE_PLAYING ||
        GameIsStartUp())
        return;



    RActorID idLocalPlayerRActor = LocalPlayerRActorGet();
    const RActor * ptPlayerRActor = RActorTryToGet(idLocalPlayerRActor);
    if (!ptPlayerRActor)
        return;



    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {

        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD)
        {
            float flCurrentHP = ptACD->GetHitPointsCurrent();
            float flMaxHP = ptACD->GetHitPointsMaxTotal();
            if (flMaxHP < -EPSILON || flMaxHP > EPSILON)
            {
                ptPlayer->flHealthPercent = flCurrentHP / flMaxHP;
            }
        }


        if (ptPlayer->tClientInfo.listQueuedItemMoves.Count())
        {
            sSendItemMoveRequests(*ptPlayer);
        }
    }


    AppGlobals & tAppGlobals = sgtAppGlobals;
    sUpdatePing();


    if (!tAppGlobals.GetTicksPerSec())
        return;
    sFloatingDamageNumbersUpdate();
    sFloatingTextUpdate();
    sCalloutEventsUpdate();
    sUpdateBusyStatus();



}




void CPlayerNewPlayer(const NewPlayerMessage & tNewPlayerMsg, int32 nControllerIndex)


{


    Player & tPlayer = *PlayerGetByPlayerIndex(tNewPlayerMsg.nPlayerIndex);
    PlayerSetDefaults(tPlayer);


    tPlayer.uHeroId = tNewPlayerMsg.uHeroId;
    tPlayer.tAccount.tHandle = tNewPlayerMsg.tAccountId;
    Blizzard::String::Copy(tPlayer.usHeroName, tNewPlayerMsg.usHeroName, sizeof(tPlayer.usHeroName));
    tPlayer.nPlayerIndex = tNewPlayerMsg.nPlayerIndex;
    tPlayer.eTeam = tNewPlayerMsg.eTeam;
    tPlayer.ePlayerClassPortrait = tNewPlayerMsg.eClass;

    tPlayer.snoActorPortrait = tNewPlayerMsg.snoActorPortrait;
    tPlayer.nCurrentLevel = tNewPlayerMsg.nCurrentLevel;
    tPlayer.nCurrentAltLevel = tNewPlayerMsg.nCurrentAltLevel;
    tPlayer.nCurrentHighestHeroSoloRiftLevel = tNewPlayerMsg.nCurrentHighestHeroSoloRiftLevel;
    tPlayer.tClientInfo.ann = tNewPlayerMsg.ann;
    tPlayer.tHeroStateData = tNewPlayerMsg.tHeroStateData;
    tPlayer.idGameConnection = tNewPlayerMsg.idGameConnection;



    Blizzard::String::Copy(tPlayer.tAccount.usName, tNewPlayerMsg.usGamerTag, sizeof(tPlayer.tAccount.usName));
    tPlayer.uAcceptedLicenseBitsHero = tNewPlayerMsg.uAcceptedLicenseBitsHero;
    tPlayer.tClientInfo.bUpscaled = tNewPlayerMsg.bUpscaled;
    tPlayer.tAccount.uPartitionFlags = tNewPlayerMsg.uPartitionFlags;
    tPlayer.uSeasonCreated = tNewPlayerMsg.uSeasonCreated;

    if (PlayerIsLocal(tNewPlayerMsg.nPlayerIndex))
    {
        InputSetPlayerController(&tPlayer, nControllerIndex);





        Console::Online::LeaderboardPerformHealing(nControllerIndex);
    }

    tPlayer.uLastPlayedTimeUpdate = GameGetServerTime();



    tPlayer.tNFPDefaultScanTime = tNewPlayerMsg.nNFPDefaultScanTime;
    tPlayer.tNFPGoblinScanTime = tNewPlayerMsg.nNFPGoblinScanTime;











































}



void CPlayerRemovePlayer(const Player & tPlayer)
{
}









CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound)
{
    return PlayerGetHeroDisplayName(tPlayer, fDisplayPound);
}


















CRefString CPlayerGetIndicatorString(const Player & tPlayer)
{
    CRefString sPlayerIndicator = CPlayerGetIndicatorString(tPlayer.nPlayerIndex);
    return sPlayerIndicator;
}




CRefString CPlayerGetIndicatorString(int32 nPlayerIndex)
{
    CRefString sPlayerIndicator;


    if (nPlayerIndex >= 0 && nPlayerIndex < 4) {

        sPlayerIndicator = LocalizedSprintf(StringListFetch("Console:PlayerLabelShort").str(),
                                            UnicodeText::FormatNumber(nPlayerIndex + 1, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        Set_Text_Color(sPlayerIndicator, UIUtils::Console::GetPlayerColor(nPlayerIndex));
    }


    return sPlayerIndicator;
}




void CPlayerAppearanceUpdate(ACDID idACD)
{



    VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
    if (ptVisualInventory)
    {
        Player * ptPlayer = ACDGetControllingPlayer(idACD);
        if (ptPlayer)
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
            BOOL fFemale = (ptPlayer->tHeroStateData.uPlayerFlags & 0x2) != 0;

            const RActorID idRActor = (RActorID)ACDGetOwner(idACD);
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor)
            {
                VisualInventoryApply(ptRActor, *ptVisualInventory, VIT_PLAYER, ePlayerClass, fFemale);
            }
        }
    }
}



void CPlayerPetAppearanceUpdate(ACDID idACDPet)
{



    ActorCommonData & tACDPet = ACDGet(idACDPet);
    if (!tACDPet.HasInventory())
        return;




    VisualInventory * ptVisualInventory = VisualInventoryGet(idACDPet);
    if (ptVisualInventory)
    {
        VisualInventoryType eVisualInventoryType = VIT_HIRELING;
        const RActorID idRActor = (RActorID)ACDGetOwner(idACDPet);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {

            VisualInventoryApply(ptRActor, *ptVisualInventory, eVisualInventoryType, -1, FALSE);
        }
    }
}


void PlayersSetLocalPlayer(const int32 nControllerIndex, const int32 nPlayerIndex, const uint32 dwSyncedAnimSeed)
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;

    ptCPlayerGlobals->nLocalPlayers[nControllerIndex] = nPlayerIndex;
    ptCPlayerGlobals->uNumLocalPlayers = 1;

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    ptPlayer->tClientInfo.bPlayerIsLocal = TRUE;

    ptPlayer->nControllerIndex = nControllerIndex;

    CPlayerUpdateServerSidePrefs(ptPlayer);

    CPlayerInputSetAnimSeed(ptPlayer, dwSyncedAnimSeed);
}













void PlayersAddLocalPlayer(const int32 nControllerIndex, const int32 nPlayerIndex, const uint32 dwSyncedAnimSeed)
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;

    ptCPlayerGlobals->nLocalPlayers[nControllerIndex] = nPlayerIndex;
    ptCPlayerGlobals->uNumLocalPlayers++;

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    ptPlayer->tClientInfo.bPlayerIsLocal = TRUE;

    ptPlayer->nControllerIndex = nControllerIndex;

    if (ptCPlayerGlobals->uNumLocalPlayers > 1) {
        Prefs::GetMutableVideoPrefs().SetUseLowFXMode(TRUE);
    }

    CPlayerUpdateServerSidePrefs(ptPlayer);
    CPlayerSendPrefsToServer(ptPlayer);

    CPlayerInputSetAnimSeed(ptPlayer, dwSyncedAnimSeed);

    SimpleMessage tMessage;
    MessageSendToServer(CMSG_FINISHED_GAME_SETUP, &tMessage, sizeof(tMessage), ptPlayer);
}













uint32 PlayerGetLocalPlayerCount()
{
    return CGameGet().ptCPlayerGlobals->uNumLocalPlayers;
}





BOOL CPlayerIsValid(const Player * ptPlayer)
{
    if (ptPlayer == NULL)
        return FALSE;
    if (ptPlayer->idPlayerACD == ACDID_INVALID)
        return FALSE;
    return ptPlayer->nControllerIndex != -1;
}






























BOOL CPlayerIsEngagedWithRare(const Player * ptPlayer)
{

    if (!ptPlayer)
        return FALSE;


    uint32 uEngagedTime = 0;

    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACD)
    {
        uEngagedTime = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_ENGAGED_WITH_RARE_TIME));
    }
    else
    {
        ClientTrickleList & listTrickle = CTrickleGetList();

        for (POS posTrickle = listTrickle.GetHeadPosition(); posTrickle; )
        {
            const ClientTrickleEntry & tTrickleEntry = listTrickle.GetNext(posTrickle);
            if (tTrickleEntry.nPlayerIndexOwner == ptPlayer->nPlayerIndex)
            {
                uEngagedTime = tTrickleEntry.uEngagedWithRareTime;
                break;
            }
        }
    }

    return PlayerIsEngagedWithRare(uEngagedTime);
}




BOOL CPlayerIsEngagedWithTreasureGoblin(const Player * ptPlayer)
{

    if (!ptPlayer)
        return FALSE;


    uint32 uEngagedTime = 0;

    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACD)
    {
        uEngagedTime = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_ENGAGED_WITH_TREASURE_GOBLIN_TIME));
    }
    else
    {
        ClientTrickleList & listTrickle = CTrickleGetList();

        for (POS posTrickle = listTrickle.GetHeadPosition(); posTrickle; )
        {
            const ClientTrickleEntry & tTrickleEntry = listTrickle.GetNext(posTrickle);
            if (tTrickleEntry.nPlayerIndexOwner == ptPlayer->nPlayerIndex)
            {
                uEngagedTime = tTrickleEntry.uEngagedWithGoblinTime;
                break;
            }
        }
    }

    return PlayerIsEngagedWithTreasureGoblin(uEngagedTime);
}




































































void CPlayerGetPortraitImage(const Player * ptPlayer, ImageTextureFrame & tFrame, BOOL bUseReversedPortrait)
{
    ActorGetPortraitTexFrame(tFrame, ptPlayer->snoActorPortrait, bUseReversedPortrait);
}







Team PlayerGetLocalTeam()
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    int32 nLocalPlayer = Console::GamerProfile::GetPrimaryProfileUserIndex();
    if (nLocalPlayer == -1)
        return -1;
    if (ptCPlayerGlobals->nLocalPlayers[nLocalPlayer] == -1)
        return -1;
    PlayerGlobals * ptPlayerGlobals = CGameGetGameCommonData().ptPlayerGlobals;
    Player & tPlayer = ptPlayerGlobals->tPlayerArray[ptCPlayerGlobals->nLocalPlayers[nLocalPlayer]];
    return tPlayer.eTeam;
}













Player * LocalPlayerGet()
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    if (ptCPlayerGlobals == NULL)
        return NULL;
    PlayerGlobals * ptPlayerGlobals = CGameGetGameCommonData().ptPlayerGlobals;
    if (ptPlayerGlobals == NULL)
        return NULL;

    int32 nIndex = -1;
    for (int32 i = 0; i < 4; i++) {
        int32 nPlayerIndex = ptCPlayerGlobals->nLocalPlayers[i];
        if (nPlayerIndex == -1)
            continue;
        nIndex = i;
        if (ptCPlayerGlobals->uNumLocalPlayers == 1)
            break;
        if (ptPlayerGlobals->tPlayerArray[nPlayerIndex].nControllerIndex == Console::GamerProfile::GetPrimaryProfileUserIndex())
            break;
    }
    if (nIndex == -1)
        return NULL;

    Player & tPlayer = ptPlayerGlobals->tPlayerArray[ptCPlayerGlobals->nLocalPlayers[nIndex]];
    return &tPlayer;
}






























RActorID LocalPlayerRActorGet()
{
    const Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer)
        return ptPlayer->idPlayerRActor;
    return RACTORID_INVALID;
}











ACDID LocalPlayerACDGet()
{
    const Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer)
        return ptPlayer->idPlayerACD;
    return ACDID_INVALID;
}




































































static CRefString sFormatDamageOrHealingNumber(
    const float flDamageAmount)
{
    // Inference: original enables millions/special suffix, or grouped float formatting with zero fractional digits/locale.
    if (!Prefs::GetGameplayPrefs().GetDisableShortFloatingNumbers())
    {
        return FormatTruncatedNumber(flDamageAmount, TRUE, TRUE);
    }
    else
    {
        return UnicodeText::FormatNumber(flDamageAmount, TRUE, FNS_NORMAL, 0, 0, 0);
    }
}



// Inference: numeric cases lacking adopted names follow original jump table and exact StringList labels.
CRefString sGenerateDamageNumberText(
    FloatingNumberType eType,
    const float flDamageAmount)
{
    CRefString sUnicodeText;

    switch (eType)
    {
    case FLOATING_NUMBER_TYPE_HEAL:
        sUnicodeText.Format("+%s", sFormatDamageOrHealingNumber(flDamageAmount).str());
        break;
    case FLOATING_NUMBER_TYPE_DAMAGE_DONE:
    case FLOATING_NUMBER_TYPE_DOT_DAMAGE_DONE:
    case FLOATING_NUMBER_TYPE_CRITICAL_HIT_DONE:
    case FLOATING_NUMBER_TYPE_DAMAGE_TAKEN:
    case FLOATING_NUMBER_TYPE_DOT_DAMAGE_TAKEN:
    case FLOATING_NUMBER_TYPE_CRITICAL_HIT_TAKEN:
        sUnicodeText.Format("%s", sFormatDamageOrHealingNumber(flDamageAmount).str());
        break;
    case 6:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Dodge").str());
        break;
    case 7:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Dodged").str());
        break;
    case 8:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Block").str());
        break;
    case 9:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Parry").str());
        break;
    case 12:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Root").str());
        break;
    case 13:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Stun").str());
        break;
    case 15:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Frozen").str());
        break;
    case 14:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Blind").str());
        break;
    case 16:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Fear").str());
        break;
    case 17:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Charm").str());
        break;
    case 18:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Taunt").str());
        break;
    case 19:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Snare").str());
        break;
    case 20:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "AttackSpeedSlow").str());
        break;
    case 23:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "BrokeStun").str());
        break;
    case 22:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "BrokeBlind").str());
        break;
    case 21:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "BrokeFreeze").str());
        break;
    case 24:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "BrokeRoot").str());
        break;
    case 25:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "BrokeSnare").str());
        break;
    case 26:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "BrokeFear").str());
        break;
    case 27:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Immune").str());
        break;
    case 11:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "Absorb").str());
        break;
    case 28:
        sUnicodeText.Format("%s", StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "CrushingBlow").str());
        break;
    case 35:
        sUnicodeText.Format(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "PVPKill").str(), flDamageAmount);
        break;
    case 36:
        sUnicodeText.Format(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "PVPAssist").str(), flDamageAmount);
        break;
    case 37:
        sUnicodeText.Format("%s", StringListFetch("X1_ConsoleCallouts:NephalemGlory_Damage").str());
        break;
    case 38:
        sUnicodeText.Format(StringListFetch("X1_ConsoleCallouts:NephalemGlory_ExtraTime").str(), Math::FloatRoundToInt(flDamageAmount));
        break;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return sUnicodeText;
}



// Inference: numeric cases 32-34 are original jump-table entries; no adopted symbolic names.
CRefString sGenerateFloatingText(
    FloatingNumberType eType,
    int32 nAmount,
    int32 nAmount2,
    GBID gbidItem)
{
    CRefString sAmount = UnicodeText::FormatNumber(nAmount, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    CRefString sText;

    switch (eType)
    {
    case 32:
        if (gbidItem != -1)
        {
            CRefString sItemName = ItemGetDisplayName(gbidItem);
            RGBAColor rgbaColor = ItemGetCraftingMatDisplayNameColor(gbidItem);
            Set_Text_Color(sItemName, rgbaColor);

            CRefString sIcon;
            ImageHandle hImage = ACDInventoryGetImageHandleFromGBID(gbidItem, 0, 1, TRUE);
            if (hImage != 0)
            {
                sIcon.Format("{item:%08x,1.5}", hImage);
            }

            sText = LocalizedSprintf(StringListFetch("H2OLayout:CraftingMatPickup"), sItemName.str(), sAmount.str(), sIcon.str(), NULL);
        }
        break;
    case 34:
        {
            CRefString sAmount2 = UnicodeText::FormatNumber(nAmount2, TRUE, FNS_NORMAL, XLOCALE_INVALID);
            sText = LocalizedSprintf(StringListFetch("H2OLayout:QuestBonusReward"), sAmount.str(), sAmount2.str(), NULL);
        }
        break;

    case 33:
        sText = LocalizedSprintf(StringListFetch("H2OLayout:XPPickup"), sAmount.str(), NULL);
        break;

    case 29:
        sText = LocalizedSprintf(StringListFetch("H2OLayout:GoldPickup"), sAmount.str(), NULL);
        break;

    case 30:
        sText = LocalizedSprintf(StringListFetch("H2OLayout:CurrencyPickup"), sAmount.str(), NULL);
        break;

    case 31:
        sText = LocalizedSprintf(StringListFetch("H2OLayout:PlatinumPickup"), sAmount.str(), NULL);
        break;
    }

    return sText;
}





static SNO sGetParticleSystemForFloatingNumber(
    const FloatingNumberType & eType)
{
    switch (eType)
    {
    // Inference: case grouping from original table 0x7100e3cf10; absent enum names retain their numeric values.
    case 2: return GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_CRIT_DONE);
    case 3: case 4: return GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_TAKEN);
    case 5: return GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_CRIT_TAKEN);
    case 10: return GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_HEAL);
    case 29: case 30: case 31: case 32: return GlobalSNOGet(GLOBALSNO_PARTICLE_GOLD_PICKUP);
    case 33: case 34: return GlobalSNOGet(GLOBALSNO_PARTICLE_XP_PICKUP);
    case 35: case 36: return GlobalSNOGet(GLOBALSNO_PARTICLE_PVP_XP_PICKUP);
    case 37: case 38: return GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_NEPHALEM_GLORY);
    // Inference: original table also covers types 0 and 1 with the default damage-done particle.
    case FLOATING_NUMBER_TYPE_DAMAGE_DONE: case FLOATING_NUMBER_TYPE_DOT_DAMAGE_DONE: return GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_DONE);



















    default: return GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_DONE);
    }
}


CRefString sGenerateDamageNumberText(FloatingNumberType, const float); // Genuine definition at1778.

static ParticleSystemID sAddParticleSystem(
    const RActorID idRActor,
    const SNO snoParticle)
{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return PARTICLESYSTEMID_INVALID;



    int32 nHardpointIndex = HardpointIndexFromName(ptRActor->snoAppearance, AURA_HARDPOINT_NAME);
    PRTransform transform;
    RActorGetHardpointTransformByIndex(*ptRActor, nHardpointIndex, transform);

    ParticleSystemAddParams tParams(snoParticle, transform);
    ParticleSystemID idParticleSystem = ParticleSystemAdd(tParams);

    return idParticleSystem;
}



static void sGenerateDamageNumberParticleSystem(
    FloatingDamageNumber & tDamageNumber)
{
    tDamageNumber.dwBornTime = CGameGetClientTicks();
    tDamageNumber.sUnicodeText = sGenerateDamageNumberText(tDamageNumber.eType, tDamageNumber.flDamageAmount);
    SNO snoParticle = sGetParticleSystemForFloatingNumber(tDamageNumber.eType);
    // Inference: original unsigned comparison admits damage types0,1,2 for large-damage tracking.
    if (uint32(tDamageNumber.eType) <= 2)


    {
        CPlayerGlobals *& ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
        if (ptCPlayerGlobals->tLargeDamageTracker.ProcessDamage(tDamageNumber.flDamageAmount))
        {
            snoParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_DAMAGE_NUMBERS_VERY_LARGE);
        }
    }

    tDamageNumber.idParticleSystem = sAddParticleSystem(tDamageNumber.idRActor, snoParticle);
}




static void sGenerateParticleSystem(
    FloatingText & tText)
{
    tText.dwBornTime = CGameGetClientTicks();
    tText.sUnicodeText = sGenerateFloatingText(tText.eType, tText.nAmount, tText.nAmount2, tText.gbidItem);
    SNO snoParticle = sGetParticleSystemForFloatingNumber(tText.eType);

    ParticleSystemAddParams tParams(snoParticle, PRTransform(cgqIdentity, tText.wpPlace.wp));
    tText.idParticleSystem = ParticleSystemAdd(tParams);
}




static uint32 sGetAggregationTime(
    FloatingNumberType eType)
{
    switch (eType)
    {
    // Inference: cases from mask 0x61781ffc12/table 0x7100e3ce70; table holes hold 30, so the 30 group comes first.
    case 1: case 4: case 10: case 11: case 12: case 13: case 14:
    case 15: case 16: case 17: case 18: case 19: case 20:
    case 27: case 28: case 37: case 38: return 30;
    case 29: case 30: return 60;
    case 32: return 6;
    default: return 0;
    }
}




























static BOOL sDontDisplayUntilEndOfAggregationWindow(
    FloatingNumberType eType)
{
    switch (eType)
    {
    // Inference: original helper mask 0x412 selects DOT types 1/4 and healing 10.
    case 1: case 4: case 10: return TRUE;
    default: return FALSE;
    }
}




















// Inference: DWARF CPlayer.cpp:2120 const float 120.0; the original adds it to the tick count as an integer.
static const float FLOATING_NUMBER_HEAL_STALE_TICKS = 120.0f;
void CPlayerAddFloatingDamageNumber(
    const ActorCommonData & tACD,
    float flDamage,
    FloatingNumberType eType)
{

    if (CWorldIsRetro())

        return;


    RActorID idRActor = static_cast<RActorID>(ACDGetOwner(tACD.id));
    RActor * pRActor = RActorTryToGet(idRActor);
    if (!pRActor)

        return;



    if (tACD.IsGizmo())

        return;



    ActorCommonData * ptACDVictim = ACDTryToGet(pRActor->idACD);
    if (!RActorIsVisibleToLocalPlayer(pRActor) && (!ptACDVictim || !ptACDVictim->AttributesGetInt(FastAttribKey(ATTRIBUTE_ALWAYS_SHOW_FLOATING_NUMBERS))))

        return;

    // Inference: original switch table 0x7100e3cd54 groups numeric floating-number types as below.
    switch (eType)
    {
    case 0:
    case 3:
    case 1:
    case 4:
        if (!CGameDisplayDamageNumbers())

            return;

        break;

    case 2:
    case 5:
    case 28:
        if (!CGameDisplayCriticals())

            return;

        break;
    case 10:
        if (!CGameDisplayHealNumbers())

            return;

        break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:

        if (!GameTypeIsPvP() && !CGameDisplayDefensiveMessages())

            return;

        break;
    }

    uint32 dwNow = CGameGetClientTicks();
    if (eType == 10)
    {
        if (dwNow > pRActor->uNextFloatingNumberHealTime)
        {


            pRActor->flFloatingNumberHealAccumulator = 0.0f;
        }

        pRActor->uNextFloatingNumberHealTime = dwNow + (uint32)FLOATING_NUMBER_HEAL_STALE_TICKS;
        pRActor->flFloatingNumberHealAccumulator += flDamage;
        flDamage = Math::FloatFloorToInt(pRActor->flFloatingNumberHealAccumulator);
        pRActor->flFloatingNumberHealAccumulator = Math::Max(0.0f, pRActor->flFloatingNumberHealAccumulator - flDamage);

        if (flDamage < 1.0f)

            return;
    }


    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    FloatingDamageNumbersList & listNumbers = ptCPlayerGlobals->listFloatingDamageNumbers;


    uint32 dwAggregationTime = sGetAggregationTime(eType);
    if (dwAggregationTime)
    {
        POS pos = listNumbers.GetHeadPosition();
        while (pos)
        {
            FloatingDamageNumber & tDamageNumber = listNumbers.GetNext(pos);
            if (sDontDisplayUntilEndOfAggregationWindow(eType))
            {
                if (tDamageNumber.idRActor == idRActor &&
                    tDamageNumber.eType == eType &&
                    tDamageNumber.idParticleSystem == -1)
                {

                    tDamageNumber.flDamageAmount += flDamage;
                    return;
                }
            }
            else
            {
                if (tDamageNumber.idRActor == idRActor &&
                    tDamageNumber.eType == eType)
                {

                    return;
                }
            }
        }
    }

    // Inference: original unsigned type range 0..5 suppresses damage below one, preserving NaNs.
    if (static_cast<uint32>(eType) <= 5 && flDamage < 1.0f)

        return;










    FloatingDamageNumber tNewItem;
    tNewItem.idRActor = idRActor;
    tNewItem.dwBornTime = 0;
    tNewItem.dwEnteredTime = dwNow;

    tNewItem.flDamageAmount = flDamage;
    tNewItem.eType = eType;
    tNewItem.rgbaColor = crgbaWhite;
    tNewItem.flUnitAlpha = tNewItem.flScale = 1.0f;
    tNewItem.wpPlace = tACD.GetPlace();
    tNewItem.wpPlace.wp.z += tACD.GetHeight();

    BOOL bResult = WorldPointToScreenPoint3DUIC(tNewItem.wpPlace.wp, tNewItem.vecUIC, -1);
    if (bResult)
    {



        if (dwAggregationTime && sDontDisplayUntilEndOfAggregationWindow(eType))
        {

            tNewItem.idParticleSystem = static_cast<ParticleSystemID>(-1);
        }
        else
        {
            sGenerateDamageNumberParticleSystem(tNewItem);
        }
        tNewItem.fVisible = TRUE;

        listNumbers.AddTail_Tracked(tNewItem);
    }
}



static void sAddFloatingAmountNumber(
    const int32 nControllerIndex,
    const WorldPlace & wpPlace,
    const int32 nAmount,
    const int32 nAmount2,
    const GBID gbidItem,
    const FloatingNumberType eType)
{
    uint32 dwNow = CGameGetClientTicks();

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    FloatingTextList & listNumbers = ptCPlayerGlobals->listFloatingText;


    uint32 dwAggregationTime = sGetAggregationTime(eType);
    if (dwAggregationTime) {
        // Inference: DWARF fResetTimer has constant value 1; the original timer store is unconditional.
        BOOL fResetTimer = TRUE;
        POS pos = listNumbers.GetHeadPosition();
        while (pos) {

            FloatingText & tText = listNumbers.GetNext(pos);

            if (tText.nControllerIndex == nControllerIndex &&
                tText.eType == eType &&
                tText.gbidItem == gbidItem &&
                tText.idParticleSystem == -1)
            {

                tText.nAmount += nAmount;
                // Inference: this conditional folds away because DWARF records fResetTimer as true.
                if (fResetTimer)
                {
                    tText.dwEnteredTime = dwNow;
                }
                return;
            }
        }
    }

    FloatingText tNewItem;
    tNewItem.nControllerIndex = nControllerIndex;
    tNewItem.dwBornTime = 0;
    tNewItem.dwEnteredTime = dwNow;

    tNewItem.nAmount = nAmount;
    tNewItem.nAmount2 = nAmount2;
    tNewItem.gbidItem = gbidItem;
    tNewItem.eType = eType;
    tNewItem.rgbaColor = crgbaWhite;
    tNewItem.flUnitAlpha = tNewItem.flScale = 1.0f;
    tNewItem.wpPlace = wpPlace;

    BOOL bResult = WorldPointToScreenPoint3DUIC(tNewItem.wpPlace.wp, tNewItem.vecUIC, -1);
    if (!bResult)
    {
        return;
    }

    if (dwAggregationTime)
    {

        tNewItem.idParticleSystem = -1;
    }
    else
    {
        sGenerateParticleSystem(tNewItem);
    }

    tNewItem.fVisible = TRUE;

    listNumbers.AddTail_Tracked(tNewItem);
}




void CPlayerAddFloatingGBIDAmountNumber(
    const int32 nControllerIndex,
    const WorldPlace & wpPlace,
    const int32 nAmount,
    const GBID gbidItem)
{
    // Inference: original arguments are zero secondary amount and floating-number type 32.
    sAddFloatingAmountNumber(nControllerIndex, wpPlace, nAmount, 0, gbidItem, 32);
}



void CPlayerAddFloatingAmountNumber(
    const int32 nControllerIndex,
    const WorldPlace & wpPlace,
    const int32 nAmount,
    const int32 nAmount2,
    const FloatingNumberType eType)
{
    // Inference: original non-item path passes the invalid GBID sentinel -1.
    sAddFloatingAmountNumber(nControllerIndex, wpPlace, nAmount, nAmount2, -1, eType);
}



CalloutEvent::CalloutEvent(const CalloutEventType eType, const uint32 uXPRewarded)
{
    this->eType = eType;
    uTime = CGameGetClientTicks();
    uCount = 0;
    this->uXPRewarded = uXPRewarded;
    bNewRecord = FALSE;
    idACDSavedPlayer = ACDID_INVALID;
    flHealthPercent = 0.0f;
    flXPMultiplier = 0.0f;
    uTotalTime = 0;
}






void CPlayerAddCalloutEvent(const CalloutEvent & tEvent)
{
    UICallouts::NewCalloutEvent(tEvent);
}








































































































void CPlayerEnterKnown(
    const int32 nPlayerIndex,
    const ACDID idACDPlayer,
    RActorID idRActorPlayer)
{








    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);




    PlayerSetACD(*ptPlayer, idACDPlayer);


    ptPlayer->idPlayerRActor = idRActorPlayer;

    RActorUpdateOcclusionManager(RActorTryToGet(idRActorPlayer));

    SNOPreload::UpdateSkills();
    SNOPreload::UpdateEquipped();

    if (ptPlayer->idPlayerACD == idACDPlayer && GameIsWeeklyChallenge())
    {
        ChallengeRift::SnapshotPlayerSkills(*ptPlayer);
    }














    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    AnimPlayerPlayIdle(tACDPlayer);

    if (GameActiveGameCommonDataIsClient())
    {
        ACDUpdateAppearance(tACDPlayer.id);


        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            if (PlayerIsLocal(ptPlayer))
            {

                CGameLoadingPartRequired(LSC_PLAYER_PRE_WARPED);
            }
        }

        if (CGameLoadingScreenIsEnabled())
        {
            if (Console::Notifications::HostMigrationLoadscreenTipEnabled())
            {


                NotificationCreationParams tParams;

                if (Console::Online::PartyGetOnlineState() == Console::Online::PARTY_ADHOC)
                {
                    tParams.uszMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_ERRORS), "GameMigrationErrorFromAdHoc");
                }
                else
                {

                    tParams.uszMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_ERRORS), "GameMigrationError");
                }

                Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            }
            Console::Notifications::EnableHostMigrationLoadscreenTip(FALSE);
            CGameLoadingPartCompleted(LSC_PLAYER_ENTER_KNOWN);
        }
        else
        {
            CGameSetLoadComplete(ptPlayer);
        }

















        ResetTradeSessionData(ptPlayer);
    }
}



static void sPrintGainedValue(int32 nVal, LPCSTR szLabel)
{

    if (nVal != 0) {

        CRefString sVal = UnicodeText::FormatNumber(nVal, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), szLabel), sVal.str(), NULL);
        Set_Text_Color(sMsg, FONTCOLOR_SYSTEM);
        UI_Print(sMsg, FALSE);
    }
}






void CPlayerLevelMessage(int32 nPlayerIndex, int32 nNewLevel)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    if (PlayerIsLocal(ptPlayer) && !GameTypeIsPvP()) {
        {
            CRefString sVal = UnicodeText::FormatNumber(nNewLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
            CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "LevelUpCongrats"), sVal.str(), NULL);
            Set_Text_Color(sMsg, FONTCOLOR_SYSTEM);
            UI_Print(sMsg, FALSE);
        }

        if (ptPlayer->idPlayerACD != ACDID_INVALID) {
            PlayerClass eClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
            ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
            const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
            const AttributeAwardDefinition & tAttribAwardDef = ((ExperienceLevel *)tExpTable.ptExperienceLevels)[nNewLevel - 1].tAttributeAward[eClass];
            sPrintGainedValue(tAttribAwardDef.nStrength, "LevelUpStrengthGained");
            sPrintGainedValue(tAttribAwardDef.nDexterity, "LevelUpDexterityGained");
            sPrintGainedValue(tAttribAwardDef.nIntelligence, "LevelUpIntelligenceGained");
            sPrintGainedValue(tAttribAwardDef.nVitality, "LevelUpVitalityGained");

            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer) {
            }
        }
    }

    ptPlayer->nCurrentLevel = nNewLevel;
}







































void CPlayerAltLevelMessage(int32 nPlayerIndex, int32 nNewAltLevel)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    if (PlayerIsLocal(ptPlayer)) {
        UIParagonBonuses::Console::OnPlayerLeveled(ptPlayer);

        if (!GameTypeIsPvP()) {
            {
                CRefString sVal = UnicodeText::FormatNumber(nNewAltLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
                CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "LevelUpAltCongrats"), sVal.str(), NULL);
                Set_Text_Color(sMsg, FONTCOLOR_SYSTEM);
                UI_Print(sMsg, FALSE);

                sPrintGainedValue(3, "LevelUpMagicFindGained");
                sPrintGainedValue(3, "LevelUpGoldFindGained");
            }

            if (ptPlayer->idPlayerACD != ACDID_INVALID) {
                PlayerClass eClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
                ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT));
                const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;
                const AttributeAwardDefinition & tAttribAwardDef = ((ExperienceLevelAlt *)tExpTableAlt.ptExperienceLevelsAlt)[Math::Min(nNewAltLevel - 1, 19999)].tAttributeAward[eClass];
                sPrintGainedValue(tAttribAwardDef.nStrength, "LevelUpStrengthGained");
                sPrintGainedValue(tAttribAwardDef.nDexterity, "LevelUpDexterityGained");
                sPrintGainedValue(tAttribAwardDef.nIntelligence, "LevelUpIntelligenceGained");
                sPrintGainedValue(tAttribAwardDef.nVitality, "LevelUpVitalityGained");
            }
        }
    }

    ptPlayer->nCurrentAltLevel = nNewAltLevel;
}



























void CPlayerHighestHeroSoloRiftLevelMessage(int32 nPlayerIndex, int32 nNewRiftLevel)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    ptPlayer->nCurrentHighestHeroSoloRiftLevel = nNewRiftLevel;
}











void CPlayerAddPetAt(Player & tPlayer, int32 nIndex, ACDID idACDPet, PetType eType)
{
    PlayerPet & tPet = tPlayer.tPets[nIndex];
    PlayerPetInitialize(tPet);
    tPet.idACD = idACDPet;
    tPet.eType = eType;

    if (nIndex > tPlayer.nMaxUsedPetIndex)
        tPlayer.nMaxUsedPetIndex = nIndex;

    BOOL bIsLocal = PlayerIsLocal(&tPlayer);
    if (bIsLocal && idACDPet != ACDID_INVALID && eType == 24) {
        CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "FollowerActivated"),
                                           MonsterGetName(ACDTryToGet(idACDPet), FALSE).str(), NULL);
        Set_Text_Color(sMsg, FONTCOLOR_SYSTEM);
        UI_Print(sMsg, FALSE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_INTERACTION_HIRE_FOLLOWER), cgtPSODefaults);
    }
}





























void CPlayerMainRActorTeleported()
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    ptCPlayerGlobals->dwFramesTillCanUsePortals = 2;
}












BOOL CPlayerCanUsePortal()
{
    // Portals stay usable for a couple of frames after the main RActor teleports, and
    // otherwise until dwNextTimeCanUsePortals.




    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;

    uint32 dwNow = CGameGetClientTicks();

    if (ptCPlayerGlobals->dwFramesTillCanUsePortals)
        return TRUE;

    // the wait can never be longer than 180 ticks

    if (ptCPlayerGlobals->dwNextTimeCanUsePortals > dwNow + 180) {

        //
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }




    return dwNow < ptCPlayerGlobals->dwNextTimeCanUsePortals;
}







void DrawCone(Player * ptPlayer, RGBAColor fillColor, RGBAColor edgeColor, Angle angle, float radius, Vector3D startLoc)
{   // Inference: tTransparent is DWARF-only; no initializer or use survives, so none is invented.
    RGBAColor tTransparent;

    // Inference: original targets XY and rotates about the referenced global cgwvZAxis by -angle.
    Vector2D tTargetingDir = ptPlayer->tTargetComponent.GetTargetingDirection();
    Vector3D tTargeting3dVector(tTargetingDir.x, tTargetingDir.y, 0.0f);

    Quaternion qRot;
    qRot.SetRotation(cgwvZAxis, -angle);

    Vector3D vStartDir = startLoc + qRot.Rotate(tTargeting3dVector) * radius;
    LineSeg3D lineseg;
    lineseg.Set(startLoc, vStartDir);
    float flNumSlices = angle * 64.0f / PI;

    // Inference: original shader4 is GLOBALSNO_SHADER_3D_PRIMS_NO_Z, default render context.
    GfxDrawSolidWedgeDeferred(lineseg, angle * 2.0f, Math::FloatToInt(flNumSlices), cgwvZAxis, fillColor, edgeColor, GLOBALSNO_SHADER_3D_PRIMS_NO_Z, RENDERCONTEXT_DEFAULT);
}

static void sPlayerRenderControllerAutoTargetingRange()
{
    if (AppDrawFlagIsSet(APP_DRAW_CONTROLLER_AUTO_TARGETING))
    {
        GameCommonData & tGCD = CGameGetGameCommonData();
        if (tGCD.GameModeGet() != GAMEMODE_PLAYING ||
            GameIsStartUp())
            return;



        ACDID idACDPlayer = LocalPlayerACDGet();
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (!ptACDPlayer)
            return;



        Vector3D wp = ptACDPlayer->GetPlace().wp;
        // Inference: original uses crgbaRed/crgbaBlue references, with alpha16/0 and filler blue64/alpha64.
        RGBAColor tSolid = crgbaRed;
        tSolid.a = 16;


        RGBAColor tFiller = crgbaBlue;
        tFiller.b = 64; tFiller.a = 64;

        // Inference: DWARF-only transparent local has no surviving initializer/use, so none is invented.
        RGBAColor tTransparent;


        RGBAColor tEdge = crgbaBlue;
        tEdge.a = 0;



        // Inference: original immediate0x60ff8080 supplies the sticky-target fill color.
        RGBAColor tFiller3 = RGBAColor(128, 128, 255, 96);

        // Inference: the original reloads the id from the ACD (ldr w0, [x19]) for this call.
        Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);

        SNO snoPower = ptPlayer->tClientInfo.snoLastUsedPower;
        if (snoPower == SNO_NONE)
        {
            snoPower = ptACDPlayer->AttributeGetBasicAttack();
        }


        if (snoPower == SNO_NONE)
            return;



        ScopedPowerDefinitionLockR pdefPower(snoPower);
        if (!pdefPower)
            return;


        // Inference: initial targeting type has no surviving store before SECONDARY; use canonical PRIMARY default.
        Targeting::Console::TargetingInformationInput input(ptPlayer, pdefPower, Targeting::Console::TARGETING_PRIMARY);
        Targeting::Console::TargetingInformationOutput output;

        float distBetweenConeAndPlayer;


        input.eTargetingType = Targeting::Console::TARGETING_SECONDARY;
        Targeting::Console::GetTargetingInformation(input, output);
        distBetweenConeAndPlayer = Vector3D::Distance(wp, output.wpConeStart.wp);
        DrawCone(ptPlayer, tSolid, tEdge, output.aSearchAngle, distBetweenConeAndPlayer + output.flSearchRadius, output.wpConeStart.wp);


        input.eTargetingType = Targeting::Console::TARGETING_STICKY;
        Targeting::Console::GetTargetingInformation(input, output);
        DrawCone(ptPlayer, tEdge, tFiller3, output.aSearchAngle, output.flSearchRadius, output.wpConeStart.wp);


        input.eTargetingType = Targeting::Console::TARGETING_PRIMARY;
        Targeting::Console::GetTargetingInformation(input, output);
        distBetweenConeAndPlayer = Vector3D::Distance(wp, output.wpConeStart.wp);
        DrawCone(ptPlayer, tFiller, tEdge, output.aSearchAngle, distBetweenConeAndPlayer + output.flSearchRadius, output.wpConeStart.wp);


        // Inference: full-circle close angle uses32-point circle, original shader4/default context; otherwise uses cone.
        if (output.flControllerAutoTargetingCloseAngle == FULL_CIRCLE)
        {
            GfxSolidCircleDrawDeferred(wp, cgwvZAxis, output.flControllerAutoTargetingCloseRange, 32, tFiller, GLOBALSNO_SHADER_3D_PRIMS_NO_Z, RENDERCONTEXT_DEFAULT);
        }
        else
        {
            DrawCone(ptPlayer, tFiller, tEdge, output.flControllerAutoTargetingCloseAngle, output.flControllerAutoTargetingCloseRange, wp);
        }
    }
}



// NON_MATCHING: 936 vs 936 bytes, ~8 instructions differ: the tFiller bit insert (0x40400000 | crgbaBlue.rg) sinks below the second Distance, which shifts the color registers (w25/w26/w27).
void CPlayerRenderUpdate()
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    if (ptCPlayerGlobals->dwFramesTillCanUsePortals)
    {
        --ptCPlayerGlobals->dwFramesTillCanUsePortals;

        if (!ptCPlayerGlobals->dwFramesTillCanUsePortals)
        {

            // Inference: original expiry adds20 client ticks after the last countdown frame.
            uint32 dwNow = CGameGetClientTicks();

            ptCPlayerGlobals->dwNextTimeCanUsePortals = dwNow + 20;
        }
    }


    sPlayerRenderControllerAutoTargetingRange();


}



void CPlayerBeginInspect(int32 nPlayerIndex, Player * ptPlayer)
{
    Player * ptInspect = ptPlayer;
    if (ptInspect == NULL)
        ptInspect = LocalPlayerGet();
    if (ptInspect == NULL)
        return;
    if (PlayerGetByPlayerIndex(nPlayerIndex) == NULL)
        return;
    PlayerIndexMessage tInspect;
    SigmaMemoryClear(&tInspect, sizeof(tInspect));
    tInspect.nPlayerIndex = nPlayerIndex;
    MessageSendToServer(CMSG_INSPECT_START, &tInspect, sizeof(tInspect), ptInspect);
}






























































































CRefString CPlayerGetLevelString(const int32 nLevel, const int32 nAltLevel)
{
    CRefString usLevel;

    if (nAltLevel > 0) {

        usLevel = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "AltLevelFormat"),
                                   UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), UnicodeText::FormatNumber(nAltLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    }
    else {

        usLevel = UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID);
    }


    return usLevel;
}





void CPlayerWarped(const PlayerWarpReason eReason, float flWarpFadeInSecs)
{
    // eReason 14 only matters while the loading screen is up
    if (eReason != 14 ||
        CGameLoadingScreenIsEnabled()) {





        UIFastEquip::Console::Close();
        CGameLoadingPartCompleted(LSC_PLAYER_PRE_WARPED);
        CGameLoadingPartCompleted(LSC_PLAYER_WARPED);






        Weather::Warped(GetRWindowMgr()->GetCurrentRWindow());
        CGameModeResetCameraToFollowPlayer();







        UIMinimapUpdateZoom(TRUE);


        BOOL bInScriptedSequence = CGameInScriptedSequence();
        if (eReason != 15 && !bInScriptedSequence) {

            float flDuration = flWarpFadeInSecs < 1e-6f ? 1.0f / 6.0f : flWarpFadeInSecs;



            CGameLoadingScreenScheduleFade(flDuration, eReason == 8 ? UIFade::FADE_COLOR_WHITE : UIFade::FADE_COLOR_BLACK);
        }
    }
}











void CPlayerChangedTeams(RActorID idRActor)
{
    Player * ptPlayer = PlayerGetByRActor(idRActor);

    ptPlayer->eTeam = ACDGet(ptPlayer->idPlayerACD).AttributesGetInt(FastAttribKey(ATTRIBUTE_TEAM));

    RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
    if (ptRActor) {
        RActorRemoveEffectParticle(*ptRActor, FastAttribKey(ATTRIBUTE_DISPLAYS_TEAM_EFFECT));
    }

    CTrickleRemoveAll();
}






static void sSayResourceError(
    uint32 dwSoundTag,
    const RActor * ptRActor)
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    uint32 dwCooldownTime;
    if (ptCPlayerGlobals->dwResourceErrorCount < 5)
    {
        dwCooldownTime = 600;
    }
    else if (ptCPlayerGlobals->dwResourceErrorCount < 10)
    {
        dwCooldownTime = 1800;
    }
    else if (ptCPlayerGlobals->dwResourceErrorCount < 15)
    {
        dwCooldownTime = 7200;
    }
    else
    {
        return;
    }

    uint32 dwNow = CGameGetClientTicks();
    if (dwNow <= ptCPlayerGlobals->dwNextTimeCanSayResourceError)
    {
        return;
    }

    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    SNO snoWarningSound = TagMapGetSNO(pdefActor->hTagMap, dwSoundTag);
    if (snoWarningSound == SNO_NONE)
    {
        return;
    }

    PlaySoundOptions tOptions;
    tOptions.fForce2D = TRUE;
    SoundManager::PlaySound(snoWarningSound, tOptions);

    ++ptCPlayerGlobals->dwResourceErrorCount;
    ptCPlayerGlobals->dwNextTimeCanSayResourceError = dwNow + dwCooldownTime;
}




void CPlayerSayError(
    uint32 dwSoundTag,
    RActorID idRActor,
    BOOL bIgnoreCooldown)
{
    if (dwSoundTag == uint32(-1))
    {
        return;
    }

    if (idRActor == RACTORID_INVALID)
    {
        idRActor = LocalPlayerRActorGet();
    }

    const RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
    {
        return;
    }



    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD && !ptACD->IsAlive())
    {
        return;
    }


    if (UIConversation::IsImportantConversationPlaying())
    {
        return;
    }

    if (dwSoundTag == 0x10225 || dwSoundTag == 0x10226)
    {


        sSayResourceError(dwSoundTag, ptRActor);
        return;
    }

    ScopedGlobalDefinitionLockR pdefGlobal;
    uint32 dwCooldownTime = pdefGlobal->flPlayerErrorCooldownTime * 60.0f;

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    uint32 dwNow = CGameGetClientTicks();
    if (!bIgnoreCooldown &&
        dwNow <= ptCPlayerGlobals->dwNextTimeCanSayError)
    {
        return;
    }

    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    SNO snoWarningSound = TagMapGetSNO(pdefActor->hTagMap, dwSoundTag);
    if (snoWarningSound == SNO_NONE)
    {
        return;
    }

    PlaySoundOptions tOptions;
    tOptions.fForce2D = TRUE;
    SoundManager::PlaySound(snoWarningSound, tOptions);

    ptCPlayerGlobals->dwNextTimeCanSayError = dwNow + dwCooldownTime;
}




void CPlayerFillDebugGameState(DebugGameState & tGameState)
{
    SigmaMemoryFill(&tGameState, 0xcd, sizeof(tGameState));

    tGameState.bPlayerValid = FALSE;

    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer) {
        RActorID idPlayerRActor = LocalPlayerRActorGet();
        const RActor * ptRActor = RActorTryToGet(idPlayerRActor);






        if (ptRActor) {

            tGameState.bPlayerValid = TRUE;

            SSceneID idSScene = ptRActor->tLocation.tWorldLocation.idSScene;
            if (idSScene != SSCENEID_INVALID && SceneCommonDataTryToGet(idSScene)) {
                tGameState.snoScene = SceneCommonGetSceneSNO(idSScene);
            }
            tGameState.eAct = GameGetCurrentAct();



            tGameState.snoPlayerActor = ptRActor->snoActor;

            WorldPlace tPlace = ptACDPlayer->GetPlace();
            tGameState.snoWorld = WorldCommonGetWorldSNO(tPlace.idSWorld);


            tGameState.tWorldPos = tPlace.wp;



            tGameState.dwSeed = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_WORLD_SEED));
        }
    }

    QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
    if (ptQuestInstance) {
        tGameState.snoQuest = ptQuestInstance->snoQuest;
        tGameState.nQuestStepUID = ptQuestInstance->nCurrentStepUID;
    }

    tGameState.nLocalPlayers = PlayerGetLocalPlayerCount();
    tGameState.nPlayers = PlayerGetNumPlayers();
    tGameState.bGameIsStartUp = GameIsStartUp();
}

CRefString CPlayerGetScreenshotComment(BOOL bForInspectorField)
{
    CRefString sJPGComment;
    return sJPGComment;
}































































































void CPlayerTakeScreenshot()
{
}








void CPlayerSetLastUsedWaypointRActor(RActorID idRActorWaypoint)
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    ptCPlayerGlobals->idRActorLastUsedWaypoint = idRActorWaypoint;
}




RActor * CPlayerGetLastUsedWaypointRActor()
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    RActor * ptRActor = RActorTryToGet(ptCPlayerGlobals->idRActorLastUsedWaypoint);
    return ptRActor;
}






static void sCalloutEventsUpdate()
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    CalloutEventList & listCallouts = ptCPlayerGlobals->listCallouts;

    CalloutEvent * ptEventToDisplay = NULL;

    uint32 dwNow = CGameGetClientTicks();
    POS pos = listCallouts.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        CalloutEvent & tEvent = listCallouts.GetNext(pos);
        // Inference: original expires callouts 180 ticks after uTime.
        if (dwNow > tEvent.uTime + 180)
        {
            listCallouts.RemoveAt(posLast);
            continue;
        }


        if (!ptEventToDisplay) ptEventToDisplay = &tEvent;

    }
    // Inference: original fades over 60 ticks after a 120-tick hold; no event shows fully opaque.



    uint8 bOpacity = 255;
    if (ptEventToDisplay)
    {
        uint32 dwTicksElapsed = dwNow - ptEventToDisplay->uTime;
        if (dwTicksElapsed > 120)
        {
            float flFrac = (float)(dwTicksElapsed - 120) / 60.0f;
            bOpacity = (uint8)Math::Pin((1.0f - flFrac) * 255.0f, 0.0f, 255.0f);
        }
    }
    UIGame::UpdateCalloutDisplay(ptEventToDisplay, bOpacity);
}


void CPlayerResetLastShownUITime()
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    ptCPlayerGlobals->dwGameTickOfLastPaneOpen = CGameGetClientTicks();
    ptCPlayerGlobals->bBusy = FALSE;
}















static void sUpdateBusyStatus()
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    BOOL bPrevState = ptCPlayerGlobals->bBusy;

    ScopedGlobalDefinitionLockR pdefGlobal;
    uint32 nUIBusyTime = pdefGlobal->nUIBusyTime * 60;

    uint32 dwNow = CGameGetClientTicks();


    if (UISideScreenAnyAreVisible() || UIGame::ModalIsActive())
    {
        ptCPlayerGlobals->bBusy = dwNow - ptCPlayerGlobals->dwGameTickOfLastPaneOpen > nUIBusyTime;
    }
    else
    {
        CPlayerResetLastShownUITime();
    }


    if (ptCPlayerGlobals->bBusy != bPrevState)
    {

        BoolDataMessage tMessage;
        tMessage.bVal = ptCPlayerGlobals->bBusy;
        MessageSendToServer(CMSG_PLAYER_BUSY, &tMessage, sizeof(tMessage), LocalPlayerGet());
    }
}





static BOOL sUpdateFloatingNumber(
    const ParticleSystemID idParticleSystem,
    WorldPlace & wpPlace,
    const SWorldID idSWorldPlayer,
    RGBAColor & rgbaColor,
    float & flUnitAlpha,
    float & flScale,
    Vector3D & vecUIC,
    BOOL & fVisible)
{
    if (wpPlace.idSWorld != idSWorldPlayer)
    {
        return FALSE;
    }

    ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(idParticleSystem);
    if (!ptParticleSystem)
    {
        return FALSE;
    }

    Particle * ptParticle = ptParticleSystem->TryToGetFirstParticle();
    if (!ptParticle)
    {
        return FALSE;
    }

    wpPlace.wp = ptParticle->wp;
    flScale = ptParticle->tParams.flScale;
    flUnitAlpha = ptParticle->tParams.flAlpha;
    rgbaColor = ptParticle->tParams.rgbaColor;

    BOOL bResult = WorldPointToScreenPoint3DUIC(wpPlace.wp, vecUIC, -1);

    fVisible = bResult ? TRUE : FALSE;
    return TRUE;
}










static void sFloatingDamageNumbersUpdate()
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    FloatingDamageNumbersList & listNumbers = ptCPlayerGlobals->listFloatingDamageNumbers;

    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
    {
        listNumbers.RemoveAll();
        return;
    }

    WorldPlace wpPlayer = ptACDPlayer->GetPlace();
    uint32 dwNow = CGameGetClientTicks();

    POS pos = listNumbers.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        FloatingDamageNumber & tDamageNumber = listNumbers.GetNext(pos);

        uint32 dwAggregationTime = sGetAggregationTime(tDamageNumber.eType);
        if (dwAggregationTime && sDontDisplayUntilEndOfAggregationWindow(tDamageNumber.eType))
        {
            if (dwNow < tDamageNumber.dwEnteredTime + dwAggregationTime)
            {
                continue;
            }

            if (tDamageNumber.idParticleSystem == PARTICLESYSTEMID_INVALID)
            {
                sGenerateDamageNumberParticleSystem(tDamageNumber);

                ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(tDamageNumber.idParticleSystem);
                if (ptParticleSystem)
                {
                    if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_ALWAYS_SHOW_FLOATING_NUMBERS)) ptParticleSystem->dwFlags |= (1 << PARTICLE_SYSTEM_DONT_GET_ALPHA_FROM_PARENT); else ptParticleSystem->dwFlags &= ~(1 << PARTICLE_SYSTEM_DONT_GET_ALPHA_FROM_PARENT);
                }
            }
        }
        // Inference: the original passes the update result on as the delete flag.
        BOOL fDelete = !sUpdateFloatingNumber(
            tDamageNumber.idParticleSystem,
            tDamageNumber.wpPlace,
            wpPlayer.idSWorld,
            tDamageNumber.rgbaColor,
            tDamageNumber.flUnitAlpha,
            tDamageNumber.flScale,
            tDamageNumber.vecUIC,
            tDamageNumber.fVisible);


        RActor * ptRActor = RActorTryToGet(tDamageNumber.idRActor);
        ActorCommonData * ptACD = ptRActor ? ACDTryToGet(ptRActor->idACD) : NULL;
        if (!RActorIsVisibleToLocalPlayer(ptRActor) && !(ptACD && ptACD->AttributesGetInt(ATTRIBUTE_ALWAYS_SHOW_FLOATING_NUMBERS)))
        {
            fDelete = TRUE;
        }

        if (fDelete)
        {
            ParticleSystemDelete(tDamageNumber.idParticleSystem);
            listNumbers.RemoveAt(posLast);
        }
    }
}





static BOOL sFloatingTextTypeInstancesAreDelayed(
    FloatingNumberType eType)
{
    switch (eType)
    {
    // Inference: original unsigned range check eType-29 < 4 selects types 29..32.
    case FLOATING_NUMBER_TYPE_GOLD: case FLOATING_NUMBER_TYPE_BLOODSHARDS: case FLOATING_NUMBER_TYPE_PLATINUM: case 32: return TRUE;
    default: return FALSE;
    }
}








static void sFloatingTextUpdate()
{

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    FloatingTextList & listFloatingText = ptCPlayerGlobals->listFloatingText;

    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
    {
        listFloatingText.RemoveAll();
        return;
    }

    WorldPlace wpPlayer = ptACDPlayer->GetPlace();
    uint32 dwNow = CGameGetClientTicks();

    POS pos = listFloatingText.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        FloatingText & tFloatingText = listFloatingText.GetNext(pos);

        uint32 dwAggregationTime = sGetAggregationTime(tFloatingText.eType);
        if (dwAggregationTime)
        {
            if (dwNow < tFloatingText.dwEnteredTime + dwAggregationTime)
            {
                continue;
            }


            if (tFloatingText.idParticleSystem == PARTICLESYSTEMID_INVALID)
            {
                BOOL fDelayInstances = sFloatingTextTypeInstancesAreDelayed(tFloatingText.eType);
                if (fDelayInstances && dwNow <
                    ptCPlayerGlobals->dwLastDelayedFloatingNumber + 30)  // Inference: original constant 0x1e spaces delayed instances.
                {
                    continue;
                }


                RActor * ptRActor = NULL;



                Player * ptPlayer = PlayerGetByControllerIndex(tFloatingText.nControllerIndex);
                if (ptPlayer)
                {
                    ptRActor = RActorTryToGet((RActorID)ACDGetOwner(ptPlayer->idPlayerACD));
                }
                if (ptRActor)
                {

                    int32 nHPName = HardpointIndexFromName(ptRActor->snoAppearance, AURA_HARDPOINT_NAME);
                    if (nHPName != -1)
                    {
                        PRTransform transformHP;
                        RActorGetHardpointTransformByIndex(*ptRActor, nHPName, transformHP);
                        tFloatingText.wpPlace.wp = transformHP.wp;
                    }
                }

                sGenerateParticleSystem(tFloatingText);
                if (fDelayInstances)
                {
                    ptCPlayerGlobals->dwLastDelayedFloatingNumber = dwNow;
                }
            }
        }
        // Inference: the original passes the update result on as the delete flag.
        BOOL fDelete = !sUpdateFloatingNumber(
            tFloatingText.idParticleSystem,
            tFloatingText.wpPlace,
            wpPlayer.idSWorld,
            tFloatingText.rgbaColor,
            tFloatingText.flUnitAlpha,
            tFloatingText.flScale,
            tFloatingText.vecUIC,
            tFloatingText.fVisible);

        if (fDelete)
        {
            listFloatingText.RemoveAt(posLast);
        }
    }
}






static SNO sGetFloatingNumberFont(
    FloatingNumberType eType,
    int32 & nFontSize)
{
    switch (eType)  // Inference: original compares 5 then 2 and branches (no csel); a switch with default first gives that.
    {
    default:
        nFontSize = 14;
        break;
    case FLOATING_NUMBER_TYPE_CRITICAL_HIT_TAKEN: case FLOATING_NUMBER_TYPE_CRITICAL_HIT_DONE:
        nFontSize = 22;
        break;
    }
    return GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF);  // Inference: both font sizes use GLOBALSNO_FONT_SANS_SERIF in surviving Switch code.
}





void FloatingDamageNumbersDraw(
    float & flZSlice)
{
    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice++;

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    FloatingDamageNumbersList & listNumbers = ptCPlayerGlobals->listFloatingDamageNumbers;


    RGBAColor rgbaShadow = crgbaTextShadow;
    POS pos = listNumbers.GetHeadPosition();
    while (pos)
    {
        FloatingDamageNumber & tDamageNumber = listNumbers.GetNext(pos);
        if (!tDamageNumber.fVisible)
        {
            continue;
        }
        // Inference: original suppresses sub-half-point ordinary/DOT damage, preserving critical and special types.
        if (tDamageNumber.flDamageAmount >= 0.5f ||
            (tDamageNumber.eType != FLOATING_NUMBER_TYPE_DAMAGE_DONE && tDamageNumber.eType != FLOATING_NUMBER_TYPE_DOT_DAMAGE_DONE && tDamageNumber.eType != FLOATING_NUMBER_TYPE_DAMAGE_TAKEN && tDamageNumber.eType != FLOATING_NUMBER_TYPE_DOT_DAMAGE_TAKEN))
        {

            uint8 uAlpha = Math::Pin(Math::FloatToInt(tDamageNumber.flUnitAlpha * 255.0f), 0, 255);
            RGBAColor rgbaColor = tDamageNumber.rgbaColor;
            rgbaColor.a = uAlpha;
            rgbaShadow.a = uAlpha;
            int32 nFontSize;
            SNO snoFont = sGetFloatingNumberFont(tDamageNumber.eType, nFontSize);

            TextRenderProperties tProps(snoFont, nFontSize, tDamageNumber.flScale, 1.0f);
            Vector2D vecUIC(tDamageNumber.vecUIC.x, tDamageNumber.vecUIC.y);
            FontStringDrawCenteredUIC(tProps, FormattedText(tDamageNumber.sUnicodeText, PARSE_DEFAULT), vecUIC, rgbaColor, &rgbaShadow, flZ, RENDERLAYER_INVALID, FALSE);
        }
    }
}




static SNO sGetFloatingTextFont(
    FloatingNumberType eType,
    int32 & nFontSize)
{
    // Inference: surviving Switch code uses size14 and GLOBALSNO_FONT_SANS_SERIF for every type.
    nFontSize = 14;
    return GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF);
}




void FloatingTextDraw(
    float & flZSlice)
{
    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice++;

    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    FloatingTextList & listNumbers = ptCPlayerGlobals->listFloatingText;

    RGBAColor rgbaShadow = crgbaTextShadow;
    POS pos = listNumbers.GetHeadPosition();
    while (pos)
    {
        FloatingText & tNumber = listNumbers.GetNext(pos);

        if (!tNumber.fVisible)
        {
            continue;
        }
        // Inference: original clamps signed alpha to [0,255] and selects the gold color.
        uint8 uAlpha = Math::Pin(Math::FloatToInt(tNumber.flUnitAlpha * 255.0f), 0, 255);
        RGBAColor rgbaColor = crgbaGold;
        rgbaColor.a = uAlpha;
        rgbaShadow.a = uAlpha;
        int32 nFontSize;
        SNO snoFont = sGetFloatingTextFont(tNumber.eType, nFontSize);

        TextRenderProperties tProps(snoFont, nFontSize, tNumber.flScale, 1.0f);
        Vector2D vecUIC(tNumber.vecUIC.x, tNumber.vecUIC.y);
        FontStringDrawCenteredUIC(tProps, FormattedText(tNumber.sUnicodeText, PARSE_DEFAULT), vecUIC, rgbaColor, &rgbaShadow, flZ, RENDERLAYER_INVALID, FALSE);
    }
}












































































































































#include "UserInterface/Controls/UIToolTipControl.h"
// Inference: DWARF records this enum at CPlayer.cpp:152 and OO_TICKS_TO_FADE_ITEM_NAME_TEXT at :174;
// those lines hold the reconstructed CPlayerGlobals layout, so the definitions sit here.
enum
{
    MAX_DISPLAYED_ITEM_TEXTS = 128,
    MAX_OVERLAP_TRIALS = 12,
    TICKS_TO_AUTODISPLAY_DROPPED_ITEM_NAMES = 900,
    TICKS_TO_SHOW_NAMES_AFTER_HOTKEY = 600,
    TICKS_TO_FADE_ITEM_NAME_TEXT = 60,
    CALLOUT_EVENT_DISPLAY_DURATION = 180,
    CALLOUT_EVENT_FADE_START = 120,
    ITEM_MOVE_REQUEST_BATCH_DELAY = 30,
    FLOATING_INSTANCE_DELAY = 30,
};
const float OO_TICKS_TO_FADE_ITEM_NAME_TEXT = 1.0f / TICKS_TO_FADE_ITEM_NAME_TEXT;  // DWARF const_value 0x3c888889

// Inference: local declaration placement; signatures from the DWARF definitions named.
namespace UIEventQueue { namespace Console { ImageHandle GetImageForItem(const ActorCommonData & tACDItem); } }  // UIConsoleEventQueue.cpp:200
UIItemTag & UI_GetItemTagFromID(const UIID & id);  // UI.cpp:2493
RActorIDListPooled & RActorGetMainList();  // RActors.cpp:11792
int32 ItemGetPlayerRecipient(const ActorCommonData & tACD);  // Items.cpp
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD);  // Items.cpp:2208
BOOL ItemIsGlyph(const ActorCommonData & tACDItem);  // Items.cpp:4252
BOOL ItemHasLabel(const GBID gbidItem, const ItemLabel eItemLabel);  // Items.cpp:2977
namespace UIInventory {
void GetItemTooltipParameters(ItemTooltipParameterBlock & tParams, const ActorCommonData & tACDItem, const ItemTooltipPlayerData & tTooltipPlayerData, const BOOL bConsoleInventory, const BOOL bShowAsRepaired);  // UIInventory.cpp:5260
void InitItemTooltip(ItemTooltipData & tData, const ACDID idACD, const GBID gbid, const ItemTooltipParameterBlock & tParams, const BOOL bCalculateComparisonData, ActorCommonData * ptACDPlayer, const BOOL bDisableSocketText);  // UIInventory.cpp:5705
void ShowAdditionalTooltips(const UIID & idParent, ActorCommonData & tACDOwner, const GBID gbidItem, const InventorySlotType eSlotToIgnore, const BOOL bResetScrollPos);  // UIInventory.cpp:6645
BOOL IsOpen();  // UIInventory.cpp:7242
}


static void sSubmitItemText(
    const RActorID idRActor,
    const float flUnitAlpha,
    BOOL & bFoundHighPriorityItem)
{
    ItemPickupGlobals & tItemPickup = CGameGet().ptCPlayerGlobals->tItemPickup;
    float flAlphaToUse = flUnitAlpha;

    if (idRActor == RACTORID_INVALID || tItemPickup.nItemTextCount >= MAX_DISPLAYED_ITEM_TEXTS)
        return;



    const RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;



    ACDID idACD = ptRActor->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return;



    ItemPickupText & tNewItem = tItemPickup.listItemText[tItemPickup.nItemTextCount];
    BOOL fShow = FALSE;
    if (ptACD->IsItem() &&
        !ItemIsGlyph(*ptACD))
    {


        if (!ItemHasLabel(ptACD->hGB.gbid, ITL_GOLD))
        {
            fShow = TRUE;

            tNewItem.hImage = UIEventQueue::Console::GetImageForItem(*ptACD);


            // Inference: the cset and csinc both carry line 4214, so the flag update stays on that line.
            tNewItem.rgbaQualityColor = ItemGetDisplayNameColor(*ptACD);
            if (ItemGetDisplayedQualityLevel(*ptACD) >= IQL_LEGENDARY) { tNewItem.bHighPriority = TRUE; bFoundHighPriorityItem = TRUE; } else { tNewItem.bHighPriority = FALSE; }


        }

























        tNewItem.nPlayerOwnerIndex = -1;
        int32 nPlayerIndex = ItemGetPlayerRecipient(*ptACD);

        if (nPlayerIndex != -1)
        {
            Player * ptOwnerPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
            if (ptOwnerPlayer)
            {
                tNewItem.nPlayerOwnerIndex = ptOwnerPlayer->nPlayerIndex;
            }
        }
    }

    if (fShow)
    {



        Vector2D wpUIC;
        Vector3D wpCenter = ptRActor->tLocation.tWorldLocation.transform.wp;
        BOOL bResult = WorldPointToScreenPointUIC(wpCenter, wpUIC, -1);
        if (bResult)
        {



            tNewItem.idRActor = idRActor;


            const int32 ITEM_TEXT_Y_OFFSET = -25;
            wpUIC.y += ITEM_TEXT_Y_OFFSET;


            Vector2D vecDimUICIcon;
            const float ICON_SIZE = 40.000202f;  // DWARF const_value 0x42200035
            // Inference: no instruction; the original subtracts 20.000101 (ICON_SIZE / 2) from wpUIC.
            vecDimUICIcon.x = vecDimUICIcon.y = ICON_SIZE * 0.5f;
            Vector2D vecTL = wpUIC - vecDimUICIcon;
            tNewItem.rectUICIcon.Set(vecTL.x, vecTL.y, vecTL.x + ICON_SIZE, vecTL.y + ICON_SIZE);




























            tNewItem.flUnitAlpha = flAlphaToUse;

            tItemPickup.nItemTextCount++;
        }
    }
}


BOOL sCheckCollideWithList(
    Rect2D & rectUIC,
    const int32 nCompare,
    const int32 nTrials,
    const Rect2D & rectBoundsUIC,
    const float flHorPad,
    const float flVertPad,
    BOOL bHighPriority)
{
    ItemPickupGlobals & tItemPickup = CGameGet().ptCPlayerGlobals->tItemPickup;
    ItemPickupTextList & listItemText = tItemPickup.listItemText;
    // Inference: priority tags compare only earlier priority tags; others also compare later priority tags; trial>6 moves X, else Y.
    int32 nItemsToCheck = bHighPriority ? nCompare : tItemPickup.nItemTextCount;
    for (int32 i = 0; i < nItemsToCheck; i++)
    {
        ItemPickupText & tItem = listItemText[i];
        Rect2D * ptRectToCompare = &tItem.rectUICText;
        if (bHighPriority)
        {
            if (!tItem.bHighPriority)
                continue;
        }
        else
        {
            if (i >= nCompare && !tItem.bHighPriority)
                continue;
        }


        if (Rect2D::Intersect(rectUIC, *ptRectToCompare, NULL))
        {
            if (nTrials > 6)
            {
                float flDeltaX;

                if (rectUIC.left < rectBoundsUIC.right)
                {
                    flDeltaX = ptRectToCompare->right - rectUIC.left + flHorPad;
                }
                else
                {
                    flDeltaX = ptRectToCompare->left - rectUIC.right - flHorPad;
                }
                rectUIC.left += flDeltaX;
                rectUIC.right += flDeltaX;
            }
            else
            {
                float flDeltaY;

                if (rectUIC.top > rectBoundsUIC.bottom)
                {
                    flDeltaY = ptRectToCompare->bottom - rectUIC.top + flVertPad;
                }
                else
                {
                    flDeltaY = ptRectToCompare->top - rectUIC.bottom - flVertPad;
                }
                rectUIC.top += flDeltaY;
                rectUIC.bottom += flDeltaY;
            }
            return FALSE;
        }
    }

    return TRUE;
}



// Inference: original bounds use epsilon 0.000001, bottom1000, pads10/2, and at most12 collision trials.
static void sFixOverlap(
    BOOL bHighPriority)
{
    ItemPickupGlobals & tItemPickup = CGameGet().ptCPlayerGlobals->tItemPickup;

    Rect2D rectViewportUIC;
    CGameGetSafeViewportUIC(rectViewportUIC);

    Rect2D rectMainViewportUIC = rectViewportUIC;
    rectMainViewportUIC.bottom = 1000.0f;

    BOOL fItemTagsAsIcons = Prefs::GetGameplayPrefs().GetItemTagsAsIcons();


    for (int32 i = 0; i < tItemPickup.nItemTextCount; ++i)
    {
        ItemPickupText & tItem = tItemPickup.listItemText[i];
        if (tItem.bHighPriority != bHighPriority)
            continue;



        tItem.fDraw = TRUE;

        if (!fItemTagsAsIcons)
        {
            BOOL fOk = FALSE;
            int32 nTrials = 0;

            while (!fOk)
            {
                fOk = sCheckCollideWithList(tItem.rectUICText, i, nTrials, rectMainViewportUIC, 10.0f, 2.0f, bHighPriority);



                if (!fOk && ++nTrials >= MAX_OVERLAP_TRIALS)
                {

                    tItem.fDraw = FALSE;
                    break;
                }
            }
        }

        if ((tItem.rectUICText.left < rectMainViewportUIC.left - 0.000001f ||
             tItem.rectUICText.top < rectMainViewportUIC.top - 0.000001f ||
             tItem.rectUICText.right > rectMainViewportUIC.right + 0.000001f ||
             tItem.rectUICText.bottom > rectMainViewportUIC.bottom) &&
            (tItem.rectUICIcon.left < rectMainViewportUIC.left - 0.000001f ||
             tItem.rectUICIcon.top < rectMainViewportUIC.top - 0.000001f ||
             tItem.rectUICIcon.right > rectMainViewportUIC.right + 0.000001f ||
             tItem.rectUICIcon.bottom > rectMainViewportUIC.bottom))
        {
            tItem.fDraw = FALSE;
        }


    }
}





static BOOL sShowActualTooltip(
    RActor * ptRActor,
    ActorCommonData & tACDPlayer,
    BOOL bResetScrollPos)
{

    ActorCommonData * ptACDItem = ACDTryToGet(ptRActor->idACD);

    if (ptACDItem &&
        ptACDItem->IsItem())
    {
        if (!ItemHasLabel(ptACDItem->hGB.gbid, ITL_GOLD))
        {



            ItemTooltipParameterBlock tParams;
            UIInventory::GetItemTooltipParameters(tParams, *ptACDItem, ItemTooltipPlayerData(&tACDPlayer, NULL), FALSE, FALSE);

            ItemTooltipData tData;
            UIInventory::InitItemTooltip(tData, ptACDItem->id, ptACDItem->hGB.gbid, tParams, TRUE, &tACDPlayer, FALSE);

            Vector2D wpUIC;
            Vector3D wpCenter = ptRActor->tLocation.tWorldLocation.transform.wp;
            BOOL bResult = WorldPointToScreenPointUIC(wpCenter, wpUIC, -1);
            if (bResult)
            {
                gtTooltipMgr.SetItemToolTip(UIDLG_ITEMTAGS, tData, TRUE, bResetScrollPos, &wpUIC, FALSE);

                if (UIInventory::IsOpen())
                {
                    UIInventory::ShowAdditionalTooltips(UIDLG_ITEMTAGS, tACDPlayer, ptACDItem->hGB.gbid, -1, TRUE);
                }


                ItemPickupGlobals & tItemPickup = CGameGet().ptCPlayerGlobals->tItemPickup;
                for (int32 i = 0; i < MAX_DISPLAYED_ITEM_TEXTS; i++)
                {
                    ItemPickupText & tItem = tItemPickup.listItemText[i];
                    tItem.ptItemTag->SetActive(FALSE);
                }
            }

        }
    }
    // Inference: every return path of the inlined copy leaves TRUE in the caller's bShowedTooltip.
    return TRUE;
}






static void sUpdateItemTag(
    UIItemTag & tItemTag,
    int32 nItemIndex)
{
    ItemPickupText & tItem = CGameGet().ptCPlayerGlobals->tItemPickup.listItemText[nItemIndex];
    tItemTag.SetActive(tItem.fDraw);

    if (tItem.fDraw)
    {
        ImageTextureFrame tFrame;
        ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, tItem.hImage, tFrame);
        tItemTag.SetAllTextureFrames(tFrame);

        tItemTag.SetWindowRect(tItem.rectUICIcon);

        tItemTag.SetItemQualityColor(tItem.rgbaQualityColor);

        tItemTag.SetItemOwnerIndex(tItem.nPlayerOwnerIndex);

        // Inference: rgbaColor (DWARF :4527, no location) starts as GetWindowColor(); only the alpha store survives.
        RGBAColor rgbaColor = tItemTag.GetWindowColor();
        rgbaColor.a = (uint8)(int32)(tItem.flUnitAlpha * 255.0f);
        tItemTag.SetWindowColor(rgbaColor);

        tItemTag.SetRActor(tItem.idRActor);
    }
}




static void sUpdateItemTags()
{

    int32 nHighlightedIndex = -1;


    ItemPickupGlobals & tItemPickup = CGameGet().ptCPlayerGlobals->tItemPickup;
    for (int32 i = 0; i < MAX_DISPLAYED_ITEM_TEXTS; i++)
    {
        ItemPickupText & tItem = tItemPickup.listItemText[i];
        UIItemTag & tItemTag = *tItem.ptItemTag;

        if (i < tItemPickup.nItemTextCount)
        {
            RActor * ptRActor = RActorTryToGet(tItem.idRActor);
            if (ptRActor && ((ptRActor->dwFlags >> RACTOR_HIGHLIGHT_BIT) & 1))
            {
                nHighlightedIndex = i;
            }

            if (i == nHighlightedIndex) tItemTag.SetActive(FALSE);
            else
                sUpdateItemTag(tItemTag, i);
        }
        else
        {
            tItemTag.SetActive(FALSE);
        }
    }






    if (nHighlightedIndex == -1)
    {
        UIItemTag & tHighlightedItemTag = UI_GetItemTagFromID(tItemPickup.idHighlightedTag);
        tHighlightedItemTag.SetActive(FALSE);
    }
    else
    {
        UIItemTag & tHighlightedItemTag = UI_GetItemTagFromID(tItemPickup.idHighlightedTag);
        sUpdateItemTag(tHighlightedItemTag, nHighlightedIndex);
    }
}































































































































static void sItemPickupTextUpdate()
{

    static ACDID s_idACDHighlightedRActor = ACDID_INVALID;

    CGame & tCGame = CGameGet();
    CPlayerGlobals * ptCPlayerGlobals = tCGame.ptCPlayerGlobals;

    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
        return;



    BOOL fCtrlDown = InputCtrlKeyIsDown(INPUTCONTEXT_DEFAULT);
    BOOL bShowDroppedItemTags = Prefs::GetGameplayPrefs().GetShowItemDroppedTooltips();

    float flAlpha = 1.0f;
    switch (Prefs::GetGameplayPrefs().GetItemsOnGroundSetting())
    {
    case ITEMS_ON_GROUND_SHOW_WHILE_HELD:
        flAlpha = Prefs::CheckAction(ACTION_UI_TOGGLE_ITEMS_ON_GROUND, FALSE) ? 1.0f : 0.0f;
        break;
    case ITEMS_ON_GROUND_TOGGLE:

        flAlpha = Prefs::GetGameplayPrefs().GetShowItemsOnGround() ? 1.0f : 0.0f;
        break;
    case ITEMS_ON_GROUND_SHOW_FOR_DURATION:
        {

            uint32 dwAge = CGameGetPredictedServerTime() - ptCPlayerGlobals->dwShowItemsOnGroundStart;
            if (dwAge <= TICKS_TO_SHOW_NAMES_AFTER_HOTKEY)
            {
                if (dwAge >= TICKS_TO_SHOW_NAMES_AFTER_HOTKEY - TICKS_TO_FADE_ITEM_NAME_TEXT)
                {
                    flAlpha = (dwAge - (TICKS_TO_SHOW_NAMES_AFTER_HOTKEY - TICKS_TO_FADE_ITEM_NAME_TEXT)) * OO_TICKS_TO_FADE_ITEM_NAME_TEXT;
                    flAlpha = Math::Pin(flAlpha, 0.0f, 1.0f);
                    flAlpha = 1.0f - flAlpha;
                }
            }
            else
            {
                flAlpha = 0.0f;
            }
        }
        break;
    }





    ptCPlayerGlobals->tItemPickup.nItemTextCount = 0;

    uint32 dwGameServerTime = CGameGetPredictedServerTime();


    BOOL bShowedTooltip = FALSE;
    BOOL bFoundHighPriorityItem = FALSE;
    RActorIDListPooled & listRActors = RActorGetMainList();
    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor)
            continue;



        if ((ptRActor->dwVisibilityFlags >> RACTOR_VISIBLE_BIT) & 1)
        {
            BOOL bResetTooltipScrollPos = FALSE;

            BOOL fHighlight = (ptRActor->dwFlags >> RACTOR_HIGHLIGHT_BIT) & 1;
            if (fHighlight && ptRActor->idACD != s_idACDHighlightedRActor)
            {
                bResetTooltipScrollPos = TRUE;
                s_idACDHighlightedRActor = ptRActor->idACD;
            }


            if (fHighlight && fCtrlDown)
            {

                bShowedTooltip = sShowActualTooltip(ptRActor, *ptACDPlayer, bResetTooltipScrollPos);
            }

            float flUnitAlpha = flAlpha;







            if (bShowDroppedItemTags)
            {
                ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
                if (ptACD && ptACD->IsItem())
                {
                    if (!ItemIsGlyph(*ptACD))
                    {
                        uint32 dwAge = dwGameServerTime - ptACD->GetHitGroundTime();
                        if (dwAge <= TICKS_TO_AUTODISPLAY_DROPPED_ITEM_NAMES)
                        {
                            float flAlphaFromDrop = 1.0f;
                            if (dwAge >= TICKS_TO_AUTODISPLAY_DROPPED_ITEM_NAMES - TICKS_TO_FADE_ITEM_NAME_TEXT)
                            {
                                flAlphaFromDrop = (dwAge - (TICKS_TO_AUTODISPLAY_DROPPED_ITEM_NAMES - TICKS_TO_FADE_ITEM_NAME_TEXT)) * OO_TICKS_TO_FADE_ITEM_NAME_TEXT;
                                flAlphaFromDrop = Math::Pin(flAlphaFromDrop, 0.0f, 1.0f);
                                flAlphaFromDrop = 1.0f - flAlphaFromDrop;
                            }
                            flUnitAlpha = Math::Max(flAlphaFromDrop, flUnitAlpha);
                        }
                    }
                    else if (ItemHasLabel(ptACD->hGB.gbid, ITL_SHRINE_GLYPH))
                    {
                        gtTutorialMgr.ShowTutorial(idRActor, GlobalSNOGet(GLOBALSNO_TUTORIAL_NEPHALEM_GLORY_1), NULL, TRUE);
                    }
                }
            }

            if (flUnitAlpha > EPSILON)
            {

                sSubmitItemText(ptRActor->id, flUnitAlpha, bFoundHighPriorityItem);
            }
        }
    }

    if (!bShowedTooltip)
    {

        s_idACDHighlightedRActor = ACDID_INVALID;
    }

    if (bFoundHighPriorityItem)
    {

        sFixOverlap(TRUE);
    }
    sFixOverlap(FALSE);

    sUpdateItemTags();
}




BOOL CPlayerCanHighlightItem(const Player * ptPlayer)
{
    return !CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, FALSE, FALSE);
}




void CPlayerResetShownItemsOnGroundDuration()
{
    CPlayerGlobals *& ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    ptCPlayerGlobals->dwShowItemsOnGroundStart = CGameGetPredictedServerTime();
}





// Inference: caller line evidence attributes raw size division here, with no ElementCount inline DIE; unsigned modulo follows udiv.
static SNO sGetBannerSNO(
    const SNO * ptArray,
    const SerializeData & serArray,
    const int32 nIndex)
{
    int32 nCount = serArray.dwSize / sizeof(SNO);
    if (!nCount)
        return SNO_NONE;



    int32 nActualIndex = uint32(nIndex) % uint32(nCount);
    return ptArray[nActualIndex];
}

static SNO sGetBannerSNO(
    const BannerTexturePair * ptArray,
    const SerializeData & serArray,
    const int32 nIndex)
{
    int32 nCount = serArray.dwSize / sizeof(BannerTexturePair);
    if (!nCount)
        return SNO_NONE;



    int32 nActualIndex = uint32(nIndex) % uint32(nCount);
    return ptArray[nActualIndex].snoTexture;
}




void CPlayerUpdateBanner(
    const PlayerAccountBanner & tBannerConfiguration,
    BannerRenderData & tBannerRenderData,
    BOOL fIsNamePlate)
{
    ScopedGlobalDefinitionLockR pdefGlobals;
    const BannerParams & tParams = pdefGlobals->tBannerParams;
    int32 nNumEpicBanners = tParams.serEpicBannerDescriptions.ElementCount(sizeof(EpicBannerDescription));

    if (fIsNamePlate)
    {
        tBannerRenderData.snoTexBanners[BANNER_BACKGROUND] = GlobalSNOGet(GLOBALSNO_TEXTURE_BANNER_NAMEPLATE_SHAPE);
        tBannerRenderData.snoTexBanners[BANNER_PATTERN] = GlobalSNOGet(GLOBALSNO_TEXTURE_BANNER_NAMEPLATE_PATTERN);
    }
    else
    {
        if (tBannerConfiguration.nEpicBanner == -1 || tBannerConfiguration.nEpicBanner >= nNumEpicBanners)
        {
            tBannerRenderData.snoTexBanners[BANNER_BACKGROUND] = sGetBannerSNO(tParams.arTexBackgrounds, tParams.serTexBackgrounds, tBannerConfiguration.uBannerIndex);
        }
        else
        {
            const EpicBannerDescription & tEpicBanner = tParams.arEpicBannerDescriptions[tBannerConfiguration.nEpicBanner];
            tBannerRenderData.snoTexBanners[BANNER_BACKGROUND] = tEpicBanner.snoBannerShape;
        }
        tBannerRenderData.snoTexBanners[BANNER_PATTERN] = sGetBannerSNO(tParams.arTexPatterns, tParams.serTexPatterns, tBannerConfiguration.nPattern);
    }

    if (tBannerConfiguration.bUseSigilVariant)
    {
        tBannerRenderData.snoTexBanners[BANNER_SIGIL_MAIN] = sGetBannerSNO(tParams.arTexVariantSigils, tParams.serTexVariantSigils, tBannerConfiguration.nSigilMain);
    }
    else
    {
        tBannerRenderData.snoTexBanners[BANNER_SIGIL_MAIN] = sGetBannerSNO(tParams.arTexMainSigils, tParams.serTexMainSigils, tBannerConfiguration.nSigilMain);
    }
    tBannerRenderData.snoTexBanners[BANNER_SIGIL_ACCENT] = sGetBannerSNO(tParams.arTexSigilAccents, tParams.serTexSigilAccents, tBannerConfiguration.nSigilAccent);

    SNO snoBannerAppearance = GlobalSNOGet(GLOBALSNO_PLAYER_BANNER_APPEARANCE);
    ScopedAppearanceDefinitionLockR pBannerAppearance(snoBannerAppearance);
    const int32 NUM_BANNER_LOOKS = pBannerAppearance->nNumLooks;
    tBannerRenderData.nLook = uint32(tBannerConfiguration.nPlacementIndex) % uint32(NUM_BANNER_LOOKS);
    // Inference: original Rand_Internal has nDelta2147483645 and nMax2147483646; shared draw selects every non-epic attachment.
    int32 nNum = Rand_Internal(CGameGlobalsGet().rngLocal, 2147483645u);
    tBannerRenderData.snoActorAttachments[BANNERATTACHMENT_RIBBON] = sGetBannerSNO(tParams.arActorRibbons, tParams.serActorRibbons, nNum);
    tBannerRenderData.snoActorAttachments[BANNERATTACHMENT_CAP] = sGetBannerSNO(tParams.arActorCaps, tParams.serActorCaps, nNum);
    if (tBannerConfiguration.nEpicBanner == -1 || tBannerConfiguration.nEpicBanner >= nNumEpicBanners)
    {
        tBannerRenderData.snoActorAttachments[BANNERATTACHMENT_BASE] = sGetBannerSNO(tParams.arActorBases, tParams.serActorBases, nNum);
        tBannerRenderData.snoActorAttachments[BANNERATTACHMENT_POLE] = sGetBannerSNO(tParams.arActorPoles, tParams.serActorPoles, nNum);
    }
    else
    {
        const EpicBannerDescription & tEpicBanner = tParams.arEpicBannerDescriptions[tBannerConfiguration.nEpicBanner];
        tBannerRenderData.snoActorAttachments[BANNERATTACHMENT_BASE] = tEpicBanner.snoBannerBase;
        tBannerRenderData.snoActorAttachments[BANNERATTACHMENT_POLE] = tEpicBanner.snoBannerPole;
    }

    int32 nCount = tParams.serColorSets.ElementCount(sizeof(BannerColorSet));

    // Inference: original unrolled loads select background, pattern, sigil and center each palette texel by0.5.
    for (int32 i = 0; i < NUM_BANNER_COLORS; ++i)
    {
        int32 nActualIndex;

        switch (i)
        {
        case BANNERCOLOR_BACKGROUND:
            nActualIndex = tBannerConfiguration.nBackgroundColorIndex;
            break;


        case BANNERCOLOR_PATTERN:
            nActualIndex = tBannerConfiguration.nPatternColorIndex;
            break;


        case BANNERCOLOR_SIGIL:
            nActualIndex = tBannerConfiguration.nSigilColorIndex;
            break;






        }

        nActualIndex = uint32(nActualIndex) % uint32(nCount);
        tBannerRenderData.flTintU[i] = (float(nActualIndex) + 0.5f) / float(nCount);
    }
}



void CPlayerUpdateBannerAttachments(
    RActor & tRActor,
    const BannerRenderData & tBannerRenderData)
{
    RActorChangeLookIndex(tRActor, tBannerRenderData.nLook, FALSE);
    // Inference: four slots use IDs674+i and original HP_base/cap/pole/ribbon strings; new child inherits irradiance texture.
    for (int32 i = 0; i < NUM_BANNER_ATTACHMENTS; ++i)
    {
        SNO snoActor = tBannerRenderData.snoActorAttachments[i];
        HardpointHash szHardpoint;
        szHardpoint = uint32(0);
        uint32 dwID = 674 + i;

        switch (i)
        {
        case BANNERATTACHMENT_BASE:
            szHardpoint = "HP_base";
            break;
        case BANNERATTACHMENT_CAP:
            szHardpoint = "HP_cap";
            break;
        case BANNERATTACHMENT_POLE:
            szHardpoint = "HP_pole";
            break;
        case BANNERATTACHMENT_RIBBON:
            szHardpoint = "HP_ribbon";
            break;
        }



        Attachment * ptAttachment = AttachmentFind(tRActor.tAttachmentList, dwID);
        if (ptAttachment)
        {

            if (ptAttachment->snoname.snoHandle == snoActor)
                continue;


            RActorClearActorAttachment(tRActor, dwID);
        }

        if (snoActor != SNO_NONE)
        {
            AttachmentID idAttachment = RActorAddAttachment(tRActor, ATTACHMENTTYPE_ACTOR, snoActor, dwID, szHardpoint, FALSE);

            Attachment * ptNewAttachment = AttachmentTryToGet(idAttachment);
            if (ptNewAttachment)
            {
                RActorID idRActorAttachment = static_cast<RActorID>(ptNewAttachment->idAttachedObject);
                RActor * ptRActor = RActorGet(idRActorAttachment);
                ptRActor->snoIrradianceTex = tRActor.snoIrradianceTex;
            }
        }
    }
}









SNO CPlayerGetLevelArea(const Player & tPlayer)
{
    SNO snoLevelArea;
    if (PlayerIsLocal(&tPlayer))
        snoLevelArea = WorldGetLevelAreaSNOFromRActor(LocalPlayerRActorGet());
    else
        snoLevelArea = tPlayer.snoAreaLocation;
    return snoLevelArea;
}


























void CPlayerUpdateServerSidePrefs(Player * ptPlayer)
{
    if (ptPlayer == NULL)
        return;

#define SET_PREF_FLAG(flag, cond) if (cond) ptPlayer->dwServerSidePrefFlags |= (flag); else ptPlayer->dwServerSidePrefFlags &= ~(flag)
    SET_PREF_FLAG(0x1, Prefs::GetGameplayPrefs().GetAutoEquipItems());
    SET_PREF_FLAG(0x2, Prefs::GetGameplayPrefs().GetShowDamageNumbers());
    SET_PREF_FLAG(0x4, Prefs::GetGameplayPrefs().GetShowHealNumbers());
    SET_PREF_FLAG(0x8, Prefs::GetGameplayPrefs().GetShowCriticals());
    SET_PREF_FLAG(0x10, Prefs::GetGameplayPrefs().GetInventoryZoom());
}






void CPlayerSendPrefsToServer(Player * ptPlayer)
{
    if (ptPlayer == NULL)
        return;

    DWordDataMessage tMessage;
    tMessage.dwData = ptPlayer->dwServerSidePrefFlags;
    MessageSendToServer(CMSG_SEND_PLAYER_PREFS, &tMessage, sizeof(tMessage), ptPlayer);
}






void CPlayerPlayLoadingAnim()
{
    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer) {
        ActorAnimationTag eAnimTag = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_LOADING_ANIM_TAG));
        if (eAnimTag != -1) {
            AnimationTiming tAnimationTiming;
            AnimPlayerGetAnimationTiming(*ptACDPlayer, eAnimTag, (uint32)-1, 1.0f, tAnimationTiming);
        }
    }
}







void CPlayerHandleItemMove(ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, Player & tPlayer)



{
    InventoryRequestMoveMessage tMessage;
    tMessage.annItem = ANNFromACD(tACDItem.id);
    tMessage.tLocation.annOwner = ANNFromACD(tInvLocDest.idACDContainer);
    tMessage.tLocation.eSlot = tInvLocDest.eSlot;
    tMessage.tLocation.nSlotX = tInvLocDest.pt.x;
    tMessage.tLocation.nSlotY = tInvLocDest.pt.y;



    if (tPlayer.tClientInfo.listQueuedItemMoves.Find(tMessage))
    {
        return;
    }
    ItemSetIsBeingManipulated(tACDItem, TRUE, 120);
    tPlayer.tClientInfo.listQueuedItemMoves.AddTail_Tracked(tMessage);
}



BOOL CPlayerIsGhosted()
{
    if (PlayerGetLocalPlayerCount() > 1)
        return FALSE;

    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (ptACDLocalPlayer == NULL)
        return FALSE;
    return ptACDLocalPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_GHOSTED)) != 0;
}









BOOL PlayerIsLocal(const Player * ptPlayer)
{
    if (ptPlayer == NULL)
        return FALSE;

    return PlayerIsLocal(ptPlayer->nPlayerIndex);
}





BOOL PlayerIsLocal(const ACDID idACD)
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    PlayerGlobals * ptPlayerGlobals = CGameGetGameCommonData().ptPlayerGlobals;
    if (ptCPlayerGlobals == NULL || ptPlayerGlobals == NULL)
        return FALSE;
    for (uint32 i = 0; i < 4; i++) {
        if (ptCPlayerGlobals->nLocalPlayers[i] != -1) {
            Player & tPlayer = ptPlayerGlobals->tPlayerArray[ptCPlayerGlobals->nLocalPlayers[i]];
            if (tPlayer.idPlayerACD == idACD)
                return TRUE;
        }
    }
    return FALSE;
}










BOOL PlayerIsLocal(RActorID idRActor)
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    PlayerGlobals * ptPlayerGlobals = CGameGetGameCommonData().ptPlayerGlobals;
    if (ptCPlayerGlobals == NULL || ptPlayerGlobals == NULL)
        return FALSE;
    for (uint32 i = 0; i < 4; i++) {
        if (ptCPlayerGlobals->nLocalPlayers[i] != -1) {
            Player & tPlayer = ptPlayerGlobals->tPlayerArray[ptCPlayerGlobals->nLocalPlayers[i]];
            if (tPlayer.idPlayerRActor == idRActor)
                return TRUE;
        }
    }
    return FALSE;
}














BOOL PlayerIsLocal(int32 nPlayerIndex)
{
    BOOL bFound = FALSE;
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;
    if (nPlayerIndex != -1 && ptCPlayerGlobals) {
        for (uint32 i = 0; i < 4; i++) {
            if (ptCPlayerGlobals->nLocalPlayers[i] == nPlayerIndex) {
                bFound = TRUE;
                break;
            }
        }
    }
    return bFound;
}









Player * LocalPlayerGetPrimary()
{
    return LocalPlayerGet();
}









Player * LocalPlayerGetValidRActor()
{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer)) {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->idPlayerRActor != RACTORID_INVALID)
            return ptPlayer;
    }
    return NULL;
}











void CPlayerSetFriendStatus(Player * ptPlayer)
{
    BoolDataMessage tSourceMessage;
    tSourceMessage.bVal = Console::Online::PresenceHasAnyFriends(ptPlayer->nControllerIndex);
    MessageSendToServer(CMSG_SET_FRIEND_STATUS, &tSourceMessage, sizeof(tSourceMessage), ptPlayer);
}





void CPlayerGetNewFriendGiftRecipient(Player * ptPlayer)
{

    CPlayerGlobals * ptPlayerGlobals = CGameGet().ptCPlayerGlobals;
    const int32 nPlayerIndex = ptPlayer->nPlayerIndex;

    Console::Online::Friend tOutFriend;
    if (Console::Online::PresenceGetRandomFriend(ptPlayer->nControllerIndex, tOutFriend, ptPlayerGlobals->tPlayerGiftInfo[nPlayerIndex].idRecipient))
    {
        ptPlayerGlobals->tPlayerGiftInfo[nPlayerIndex].sNameRecipient = CRefString(tOutFriend.m_szUsername);
        ptPlayerGlobals->tPlayerGiftInfo[nPlayerIndex].idRecipient = tOutFriend.m_idBDUser;
    }
    else
    {


        CPlayerSetFriendStatus(ptPlayer);
    }
}

void CPlayerSetAvengerGiftRecipient(CRefString sNameRecipient, uint64 idRecipient)
{
    CGameGet().ptCPlayerGlobals->tAvengerGiftInfo.sNameRecipient = sNameRecipient;
    CGameGet().ptCPlayerGlobals->tAvengerGiftInfo.idRecipient = idRecipient;
}



inline CRefString PlayerGetFriendGiftRecipientName(int32 nPlayerIndex)
{

    return CGameGet().ptCPlayerGlobals->tPlayerGiftInfo[nPlayerIndex].sNameRecipient;
}

inline uint64 PlayerGetFriendGiftRecipientID(int32 nPlayerIndex)
{

    return CGameGet().ptCPlayerGlobals->tPlayerGiftInfo[nPlayerIndex].idRecipient;
}

CRefString PlayerGetAvengerGiftRecipientName()
{
    return CGameGet().ptCPlayerGlobals->tAvengerGiftInfo.sNameRecipient;
}

inline uint64 PlayerGetAvengerGiftRecipientID()
{

    return CGameGet().ptCPlayerGlobals->tAvengerGiftInfo.idRecipient;
}

void PlayerRegisterFriendGift(Player * ptPlayer, ACDID idACD, int32 nOwnerIndex, const BOOL bAvenger)
{





    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {


        if ((uint32)nOwnerIndex >= 4)
        {
            nOwnerIndex = ptPlayer->nPlayerIndex;
        }

        Player * ptPlayerToUse = PlayerGetByPlayerIndex(nOwnerIndex);

        if (!ptPlayerToUse)
            ptPlayerToUse = ptPlayer;




        if (ptPlayerToUse)
        {




            if (!ptACD->ptFriendGiftData)
            {
                ptACD->ptFriendGiftData = new FriendGiftData;
            }


            if (bAvenger)
            {

                ptACD->ptFriendGiftData->idRecipient = PlayerGetAvengerGiftRecipientID();
                ptACD->ptFriendGiftData->sNameRecipient = PlayerGetAvengerGiftRecipientName();
            }
            else if (ptACD->ptFriendGiftData->idRecipient == 0)
            {


                ptACD->ptFriendGiftData->idRecipient = PlayerGetFriendGiftRecipientID(nOwnerIndex);
                ptACD->ptFriendGiftData->sNameRecipient = PlayerGetFriendGiftRecipientName(nOwnerIndex);
            }













            ACDNetworkName annItem = ANNFromACD(idACD);
            PlayerGiftMessage tMessage;
            tMessage.ann = annItem;
            tMessage.nId = ptACD->ptFriendGiftData->idRecipient;
            tMessage.nOwnerIndex = nOwnerIndex;
            Blizzard::String::Copy(tMessage.szPlayerName, ptACD->ptFriendGiftData->sNameRecipient.str(), 32);

            MessageSendToServer(CMSG_PLAYER_GIFT, &tMessage, sizeof(tMessage), ptPlayerToUse->nControllerIndex);

            CPlayerGetNewFriendGiftRecipient(ptPlayerToUse);
        }
    }
}

BOOL PlayersAreAllLocal()
{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer)) {
        if (!PlayerIsLocal(ptPlayer))
            return FALSE;
    }
    return TRUE;
}

// NON_MATCHING: final OR takes GetLicenses() as the first operand (orr w8, w0, w19 vs w19, w0)

void CPlayerLocalPlayerJoin(
    int32 nControllerIndex)
{
    GamePause(FALSE);
    GameSoftPause(FALSE);

    JoinConsoleGameMessage tMessage;

    tMessage.eXLocale = UnicodeText::GetCurrentLocale();
    tMessage.uControllerIndex = (uint8)nControllerIndex;

    D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(nControllerIndex);

    tMessage.uLicenses = tAccountDefinition.console_data().legacy_license_bits();
    tMessage.uLicenses |= 0x50;
    if (tAccountDefinition.console_data().has_bnet_account())
        tMessage.uLicenses |= 0x2000000;
    else
        tMessage.uLicenses &= ~0x3000000;
    tMessage.uLicenses |= Console::DLCManager::GetLicenses();
    tMessage.uLicenses |= 0x1000000;
    MessageSendToServer(CMSG_JOIN_CONSOLE_GAME, &tMessage, sizeof(tMessage), (const Player *)NULL);
}




void CPlayerLocalPlayerLeave(Player * ptPlayer)
{
    if (PlayerIsLocal(ptPlayer)) {
        GamePause(FALSE);
        GameSoftPause(FALSE);
        LogoutContextMessage tMessage;
        tMessage.eLogoutContext = LOGOUT_CONTEXT_SOLO;
        MessageSendToServer(CMSG_START_LOGOUT_TIMER, &tMessage, sizeof(tMessage), ptPlayer);
    }
}









void CPlayerLocalPlayerLeft(int32 nControllerIndex)
{
    CPlayerGlobals * ptCPlayerGlobals = CGameGet().ptCPlayerGlobals;

    ptCPlayerGlobals->uNumLocalPlayers--;

    ptCPlayerGlobals->nLocalPlayers[nControllerIndex] = -1;

    Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer) {

        PlayersSetMainPlayerRActor(ptPlayer, ptPlayer->idPlayerRActor);
    }

    if (ptCPlayerGlobals->uNumLocalPlayers < 2) {

        Prefs::GetMutableVideoPrefs().SetUseLowFXMode(FALSE);
    }

    if (Console::GamerProfile::GetPrimaryProfileUserIndex() != nControllerIndex) {

        if (Console::Screens::IsScreenSetOpen() && UI_GetPrimaryUIController() == nControllerIndex) {

            Console::Screens::CloseScreenSet(TRUE, FALSE);
        }

        if (Console::Online::PartyIsInParty(nControllerIndex)) {

            Console::Online::PartyRemovePlayer(nControllerIndex);
        }

        Console::GamerProfile::SignOut(nControllerIndex);
    }

    UIWaypointMap::CancelTutorialWasInterrupted();
}

























void CPlayerHandleFetchMail(const D3::GameMessage::MailContents & tMessage)
{
}






























void CPlayerHandleMailOperationResponse(const D3::GameMessage::MailOperationResponse & tResponse)
{
}







































void CPlayerHandleSentMail(uint64 nMailId, int errCode)
{
}












































































































void CPlayerUpdateUpscaleNotification(Player * ptPlayer, int32 nUpscaleLevel)
{


    if (ptPlayer)
    {
        if (nUpscaleLevel > 0)
        {

            CRefString sTitle = StringListFetch("Console:ApprenticeModeTitle");
            CRefString sDetail = StringListFetch("Console:ApprenticeModeDetail");
            CRefString sTextureName = CRefString().Format("UI:Buff_Sidekick");

            UIEventQueue::Console::DisplayMessageWithIconForPlayer(sTitle, ptPlayer, ImageTextureFrame(sTextureName), &sDetail, 0, 0);
            if (PlayerIsLocal(ptPlayer->nPlayerIndex))
            {
                gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_X1_SIDEKICK), ptPlayer, TRUE);
            }
        }

        ptPlayer->tClientInfo.bUpscaled = nUpscaleLevel > 0;
    }

}


















































int32 CPlayerGetStashLookIndex(const Player & tPlayer)
{
    return 1;
}

