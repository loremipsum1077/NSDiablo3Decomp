// Original file: X/Client/UserInterface/Console/UIConsoleHUD.cpp
// Console HUD. Generated from build/try/r5c-hud/frag.cpp by gen.py (pads functions onto their original lines).
// Local declarations stand in for headers not adopted yet; each names its defining file.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../Console/ControllerUtils.h"
#include "../UIID.h"
#include "../UI.h"
#include "../UIIDs.h"
#include "../../CPlayer.h"
#include "../../../Common/Player.h"
#include "../../../Common/Game.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/ImageHandle.h"
#include "../../../Common/OnlineService/SubscriptionEvent.h"
#include "../../../Sigma/RefString.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIBuffs.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../RActors/RActors.h"
#include "../../CGame.h"
#include "ConsoleScreenManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../Interface/UIConfirmation.h"
#include "../Controls/UIConsoleBuffIndicatorControl.h"
#include "../Controls/UIConsoleSkillsTimerControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UIHotbarButtonControl.h"
#include "../Controls/UITimerControl.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../Battlenet/Subsystems/FanfareManager.h"
#include "../UIStoryboard.h"
#include "UIConsoleHUD.h"
#include "../../../Common/TagMaps/ActorTags.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Powers/PowerAPISwig.h"
namespace Console { namespace Online { PartyMemberVoiceChatTalkingEvent & GetPartyMemberVoiceChatTalkingEvent(); BOOL IsVoiceChatRestricted(); int32 PartyGetHostIndex(); } }  // X/Client/Console/Online.cpp
extern UIID UIWND_CONSOLE_HUD_PLAYER_SLOTS;  // X/Client/UserInterface/UIIDs.h
BOOL RActorIsHidden(const RActor * ptRActor);  // X/Client/RActors/RActors.cpp:12137
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);  // UI.cpp:5411
UIConsoleBuffIndicator & UI_GetConsoleBuffIndicatorFromID(const UIID & id);  // UI.cpp:5594
UIConsoleSkillsTimer & UI_GetConsoleSkillsTimerFromID(const UIID & id);  // UI.cpp:5622
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);  // UI.cpp:6205
const HotbarButtonData & PlayerGetHotbarBtnAssignment(const Player * ptPlayer, const SkillButton eButton);  // X/Common/Player.cpp:1846
namespace UIGame { RActorID GetRActorHighlight(int32 nControllerIndex); }  // X/Client/UserInterface/Interface/UIGame.cpp:6417
namespace UIFastEquip { namespace Console { void CloseForPlayer(const int32 nPlayerIndex); } }  // UIConsoleFastEquip.cpp:1321
BOOL PlayerPetShowPortrait(PetType eType);  // X/Common/Player.cpp:2810
int32 PlayerGetNumValidInGamePlayers();  // X/Common/Player.cpp:2203
namespace UIEventQueue { namespace Console { void UpdateForPlayer(const Player * ptPlayer); void InvalidateForPlayer(int32 nPlayerIndex); } }  // UIConsoleEventQueue.cpp
void PowerGetBuffAttributes(int32, Attrib &, Attrib &, Attrib &);  // X/Common/Powers/PowersCommon.h
class UITimer; UITimer & UI_GetTimerFromID(const UIID & id);  // UI.cpp
int32 PlayerGetNumPetsOfType(ACDID idACDPlayer, PetType eType);  // X/Common/Player.cpp:2672
ACDID PlayerGetFirstPet(ACDID idACDPlayer, PetType eType, int32 & nPos);  // X/Common/Player.cpp:2589
namespace UIMultiplayerDeathMenu { namespace Console { void RefreshForAllPlayers(); } }  // UIConsoleMultiplayerDeathMenu.cpp:613
inline void UIProgressBar::SetForegroundRActor(const SNO snoActor, const RenderContext eContext) { sSetRActor(m_idForegroundChild, snoActor, eContext); }  // UIProgressBarControl.h:109
#include "../Interface/UIGame.h"
static uint64 sgarLastKnownRestXP[4];
extern UIID UISTORYBOARD_CONSOLE_FREE_CAST_TIMING;  // X/Client/UserInterface/UIIDs.h; original relocation.
namespace UIHUD {
namespace Console {


static const int32 HIRELING_HUD_SLOT = 2;
static const int32 BUFF_SLOTS_PER_ROW = 7;

static const int32 BUFF_SLOT_COUNT = 14;
static const float BUFF_WIDTH_OFFSET = 8.0f;
static const float BUFF_HEIGHT_OFFSET = 8.0f;

static bool sgbLostBonusXP;

enum eBuffType {
    BUFF_TYPE_NONE = 0,
    BUFF_TYPE_OTHER_BUFF = 1,
    BUFF_TYPE_DEBUFF = 2,
    BUFF_TYPE_OWN_BUFF = 3,
    BUFF_TYPE_PET = 4,
};


static const int32 FLASHER_STEPS = 8;


static float ftStepInfo[8][2] = {
    { 0.0f, 0.0f },
    { 0.15f, 0.85f },
    { 0.2f, 0.85f },
    { 0.3f, 0.45f },
    { 0.35f, 0.45f },
    { 0.45f, 0.85f },
    { 0.5f, 0.85f },
    { 0.65f, 0.0f },
};

class SkillCooldownFlasher {  // size 0x18
public:



    SkillCooldownFlasher()
        : m_ftAlpha(0.0f)
        , m_nTime(0)
        , m_bActive(FALSE)
        , m_nCurrentStep(0)
        , m_idTarget(GetUIIDNone())
    {
    }

    BOOL IsActive() { return m_bActive; }
    void Update();
    void Trigger();






    void SetTarget(UIID & idTarget) { m_idTarget = idTarget; }

private:
    float m_ftAlpha;  // 0x00
    uint32 m_nTime;  // 0x04
    BOOL m_bActive;  // 0x08
    uint32 m_nCurrentStep;  // 0x0c
    UIID m_idTarget;  // 0x10
};

struct DisplayInfo {  // size 0x4d8
    static const int32 SKILL_BUTTON_COUNT = 6;
    struct SkillButtonInfo {  // size 0x30
        InputButton eControllerButton;  // 0x00
        UIID idButton;  // 0x08
        BOOL bDirty;  // 0x10
        SkillCooldownFlasher tFlasher;  // 0x18










        SkillButtonInfo()
            : eControllerButton(-1)
            , idButton(GetUIIDNone())
            , bDirty(FALSE)
        {
        }
    };
    struct DisplayBuff {  // size 0x28
        eBuffType eType;  // 0x00
        SNO snoPower;  // 0x04
        int32 nPowerBuffIndex;  // 0x08
        PetType ePetType;  // 0x0c
        UIID idControl;  // 0x10
        UIID idBuffIndicator;  // 0x18
        int32 nStackCount;  // 0x20
        BOOL bShowSingleStack;  // 0x24
    };
    struct LocalHUD {  // size 0x470
        UIID idMain;  // 0x00
        UIID idXPBar;  // 0x08
        UIID idAltXPBar;  // 0x10
        UIID idPoRXPMarker;  // 0x18
        UIID idLevel;  // 0x20
        UIID idHealthBar;  // 0x28
        UIID idLowHealthBar;  // 0x30
        UIID idLowHealthGlow;  // 0x38
        UIID idHealthGlobe;  // 0x40
        UIID idHealthGlobeTarget;  // 0x48
        UIID idResourceBar1;  // 0x50
        UIID idResourceBar2;  // 0x58
        UIID idResourceBar3;  // 0x60
        UIID idResourceBarDivider;  // 0x68
        UIID idBuffRegion;  // 0x70
        UIID idBuffPositive;  // 0x78
        UIID idBuffNegative;  // 0x80
        UIID idPetRegion;  // 0x88
        UIID idNameRegion;  // 0x90
        UIID idPotionIcon;  // 0x98
        UIID idPotionButtonText;  // 0xa0
        UIID idPotionCount;  // 0xa8
        UIID idPotionCountShadow;  // 0xb0
        UIID idPotionCooldownCover;  // 0xb8
        UIID idPotionCooldownSpark;  // 0xc0
        UIID idDeath;  // 0xc8
        UIID idDurability;  // 0xd0
        UIID idApprenticeIndicator;  // 0xd8
        UIID idMailIndicator;  // 0xe0
        UIID idPartyLeaderIndicator;  // 0xe8
        UIID idVoiceChatIndicator;  // 0xf0
        UIID idCacheIndicator;  // 0xf8
        SkillButtonInfo arSkills[6];  // 0x100
        SkillCooldownFlasher tPotionFlasher;  // 0x220
        DisplayBuff arCurrentBuffs[14];  // 0x238
        BOOL bLowHealthActive;  // 0x468









LocalHUD()
    : idMain(GetUIIDNone())
    , idXPBar(GetUIIDNone())
    , idAltXPBar(GetUIIDNone())
    , idPoRXPMarker(GetUIIDNone())
    , idLevel(GetUIIDNone())
    , idHealthBar(GetUIIDNone())
    , idLowHealthBar(GetUIIDNone())
    , idLowHealthGlow(GetUIIDNone())
    , idHealthGlobe(GetUIIDNone())
    , idHealthGlobeTarget(GetUIIDNone())
    , idResourceBar1(GetUIIDNone())
    , idResourceBar2(GetUIIDNone())
    , idResourceBar3(GetUIIDNone())
    , idResourceBarDivider(GetUIIDNone())
    , idBuffRegion(GetUIIDNone())
    , idBuffPositive(GetUIIDNone())
    , idBuffNegative(GetUIIDNone())
    , idPetRegion(GetUIIDNone())
    , idNameRegion(GetUIIDNone())
    , idPotionIcon(GetUIIDNone())
    , idPotionButtonText(GetUIIDNone())
    , idPotionCount(GetUIIDNone())
    , idPotionCountShadow(GetUIIDNone())
    , idPotionCooldownCover(GetUIIDNone())
    , idPotionCooldownSpark(GetUIIDNone())
    , idDeath(GetUIIDNone())
    , idDurability(GetUIIDNone())
    , idApprenticeIndicator(GetUIIDNone())
    , idMailIndicator(GetUIIDNone())
    , idPartyLeaderIndicator(GetUIIDNone())
    , idVoiceChatIndicator(GetUIIDNone())
    , idCacheIndicator(GetUIIDNone())
{
    bLowHealthActive = FALSE;
    arSkills[0].eControllerButton = 0;
    arSkills[1].eControllerButton = 2;
    arSkills[2].eControllerButton = 3;
    arSkills[3].eControllerButton = 1;
    arSkills[4].eControllerButton = 6;
    arSkills[5].eControllerButton = 7;
    for (uint32 n = 0; n < BUFF_SLOT_COUNT; n++) {
        arCurrentBuffs[n].eType = BUFF_TYPE_NONE;
    }
}

        void FillDisplayInfo(const UIID & idRoot);
    };
    struct OnlineHUD {  // size 0x60
        UIID idMain;  // 0x00
        UIID idNameRegion;  // 0x08
        UIID idPortrait;  // 0x10
        UIID unk_18;  // 0x18: .OnlinePortraitFrame.CheatDeath
        UIID idHirelingPortrait;  // 0x20
        UIID idHirelingHealthBar;  // 0x28
        UIID idHirelingName;  // 0x30
        UIID idLevel;  // 0x38
        UIID idHealthBar;  // 0x40
        UIID idLabelRegion;  // 0x48
        UIID idLevelRegion;  // 0x50
        uint32 nTimeLastSpoke;  // 0x58











OnlineHUD()
    : idMain(GetUIIDNone())
    , idNameRegion(GetUIIDNone())
    , idPortrait(GetUIIDNone())
    , idHirelingPortrait(GetUIIDNone())
    , idHirelingHealthBar(GetUIIDNone())
    , idHirelingName(GetUIIDNone())
    , idLevel(GetUIIDNone())
    , idHealthBar(GetUIIDNone())
    , idLabelRegion(GetUIIDNone())
    , idLevelRegion(GetUIIDNone())
    , nTimeLastSpoke(0)
{
}

        void FillDisplayInfo(const UIID & idRoot);
    };



DisplayInfo()
    : idMain(GetUIIDNone())
{
}

    UIID idMain;  // 0x00
    LocalHUD tLocal;  // 0x08
    OnlineHUD tOnline;  // 0x478
};

static_assert(sizeof(DisplayInfo::LocalHUD) == 0x470, "LocalHUD");
static_assert(sizeof(DisplayInfo::OnlineHUD) == 0x60, "OnlineHUD");
static_assert(__builtin_offsetof(DisplayInfo::OnlineHUD, unk_18) == 0x18, "OnlineHUD cheat-death control");
static_assert(__builtin_offsetof(DisplayInfo::OnlineHUD, nTimeLastSpoke) == 0x58, "OnlineHUD timestamp");
static_assert(sizeof(DisplayInfo) == 0x4d8, "DisplayInfo");

struct BuffInfo {  // size 0x10
    SNO snoPower;  // 0x00
    PetType ePetType;  // 0x04
    ImageHandle hImage;  // 0x08
    int32 nBuffIdx;  // 0x0c
};

struct HUDGlobals {  // size 0x1360







HUDGlobals()
    : fInitialized(FALSE)
    , fPlayerJoinedOrLeft(FALSE)
    , nPlayers(0)
{
    for (uint32 n = 0; n < 4; n++) {
        nLastPotionCount[n] = -1;
    }
}

    BOOL fInitialized;  // 0x00
    BOOL fPlayerJoinedOrLeft;  // 0x04
    int32 nPlayers;  // 0x08
    DisplayInfo tMultiPlayers[4];  // 0x10
    int32 nLastPotionCount[4];  // 0x1350
};





static HUDGlobals sgtHUDGlobals;

void DisplayInfo::LocalHUD::FillDisplayInfo(const UIID & idRoot) {
    idMain = idRoot;
    idNameRegion = UIIDCreateFromParent(idRoot, ".Name");
    idXPBar = UIIDCreateFromParent(idRoot, ".XP");

    idAltXPBar = UIIDCreateFromParent(idRoot, ".ParagonXP");
    idPoRXPMarker = UIIDCreateFromParent(idRoot, ".PoolsOfReflectionXPMarker");
    idLevel = UIIDCreateFromParent(idRoot, ".Level");
    idResourceBar1 = UIIDCreateFromParent(idRoot, ".Resource1");
    idResourceBar2 = UIIDCreateFromParent(idRoot, ".Resource2");
    idResourceBar3 = UIIDCreateFromParent(idRoot, ".Resource3");
    idResourceBarDivider = UIIDCreateFromParent(idRoot, ".Divider");
    idHealthBar = UIIDCreateFromParent(idRoot, ".Health");
    idLowHealthBar = UIIDCreateFromParent(idRoot, ".LowHealth");
    idLowHealthGlow = UIIDCreateFromParent(idRoot, ".LowHP_Glow");
    idHealthGlobe = UIIDCreateFromParent(idRoot, ".Health.HP_Globe");
    idHealthGlobeTarget = UIIDCreateFromParent(idRoot, ".Health.HP_Forecast");
    idPotionIcon = UIIDCreateFromParent(idRoot, ".PotionIcon");
    idPotionButtonText = UIIDCreateFromParent(idRoot, ".PotionButton");
    idPotionCount = UIIDCreateFromParent(idRoot, ".PotionCount");
    idPotionCountShadow = UIIDCreateFromParent(idRoot, ".PotionCountShadow");
    idPotionCooldownCover = UIIDCreateFromParent(idRoot, ".PotionCooldownCover");
    idPotionCooldownSpark = UIIDCreateFromParent(idRoot, ".PotionCooldownSpark");
    idDeath = UIIDCreateFromParent(idRoot, ".Death");
    idDurability = UIIDCreateFromParent(idRoot, ".Durability");
    idBuffRegion = UIIDCreateFromParent(idRoot, ".Buffs");
    idApprenticeIndicator = UIIDCreateFromParent(idRoot, ".ApprenticeWings");
    idMailIndicator = UIIDCreateFromParent(idRoot, ".HUDNotify.MailNotify");
    idPartyLeaderIndicator = UIIDCreateFromParent(idRoot, ".Crown");
    idVoiceChatIndicator = UIIDCreateFromParent(idRoot, ".HUDNotify.Chat");
    idCacheIndicator = UIIDCreateFromParent(idRoot, ".HUDNotify.CacheNotify");
    if (!UI_TestID(idBuffRegion)) {
        idBuffRegion = GetUIIDNone();
    }
    idBuffPositive = UIIDCreateFromParent(idRoot, ".Buffs.buff_area_positive");
    if (!UI_TestID(idBuffPositive)) {
        idBuffPositive = GetUIIDNone();
    }
    idBuffNegative = UIIDCreateFromParent(idRoot, ".Buffs.buff_area_negative");
    if (!UI_TestID(idBuffNegative)) {
        idBuffNegative = GetUIIDNone();
    }

    for (int32 i = 0; i < SKILL_BUTTON_COUNT; i++) {
        arSkills[i].idButton = UIIDCreateFromParent(idRoot, CRefString().Format(".SkillsParent.Skills.Slot%d", i + 1));
    }

    UIID idStoryboard = UIIDCreate("Root.NormalLayer.ConsoleHUD_SinglePlayerFrame_main.SkillSlotPulse");
    if (UI_TestID(idStoryboard)) {
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        CRefString sAnimationId;
        for (int32 i = 0; i < SKILL_BUTTON_COUNT; i++) {
            UIID idSelected = UIIDCreateFromParent(arSkills[i].idButton, ".Selected");
            if (UI_TestID(idSelected)) {
                tStoryboard.BindAnimation(idSelected, sAnimationId.Format("%s.SelectedA", idStoryboard.GetIDAsString().str()), (UIAnimationProperty)6, 1.0f);
            }
        }
    }

    for (int32 i = 0; i < SKILL_BUTTON_COUNT; i++) {
        UIID idFlasher = UIIDCreateFromParent(arSkills[i].idButton, ".anim_CooldownWhiteout");
        arSkills[i].tFlasher.SetTarget(idFlasher);
    }
    UIID idPotionFlasher = UIIDCreateFromParent(idRoot, ".anim_CooldownWhiteout");
    tPotionFlasher.SetTarget(idPotionFlasher);
}

int32 GetBuffIndex(const Player * ptPlayer, int32 nRefValue, int32 nPowerBuffIndex);
void SetDisplayBuffForPlayer(const Player * ptPlayer, int32 nBuffIndex, eBuffType eType, const BuffInfo & tBuffInfo);
void UpdateForPlayer(const Player * ptPlayer, const BOOL fLocal);
void DisplayBuff(const Player * ptPlayer, const UIBuffs::SortedBuff & tBuff, eBuffType eType, int32 nStackCount);
void UpdateForHireling(Player * ptPlayer, const ActorCommonData & tACDHireling);

} }
// X/Common/Tutorials.cpp, X/Client/UserInterface/Interface/UITutorials.h (not adopted yet) Also DWARF Monsters.cpp:982, Hireling.cpp:710, Definitions/ActorDefinition.cpp:1195.
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); void Shown(Player & tPlayer, const SNO snoTutorial); void Reset(Player & tPlayer, const SNO snoTutorial); } namespace Controller { CRefString GetPotionButtonIconName(const Player * ptPlayer); } CRefString MonsterGetName(const ActorCommonData * ptACD, const BOOL bSetTextColor); BOOL HirelingHasUnspentSkillPoints(const ActorCommonData & tACDHireling); void ActorGetPortraitTexFrame(ImageTextureFrame & tTexFrame, const SNO snoActor, const BOOL bUseAlt); // ControllerSchemeDefinition.cpp:279
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr; void ConvertPixelsToUIC(const Vector2D & vecPixels, Vector2D & vecUIC); // UIUtils.h
extern UIID UIDLG_QUESTREWARD; void RActorChangeLook(const RActorID idRActor, LPCSTR szLookName); // UIIDs.h / RActors.cpp:3168
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
extern UIID UISTORYBOARD_CONSOLE_REST_EXP_MARKER_FADEIN; extern UIID UISTORYBOARD_CONSOLE_REST_EXP_MARKER_DESTROY;  // X/Client/UserInterface/UIIDs.h
#include "../../../Common/ActorInventory.h"
namespace UIHUD { namespace Console {
void DisplayInfo::OnlineHUD::FillDisplayInfo(const UIID & idRoot) {
    idMain = idRoot;
    idNameRegion = UIIDCreateFromParent(idRoot, ".Name");
    idPortrait = UIIDCreateFromParent(idRoot, ".OnlinePortraitFrame");
    unk_18 = UIIDCreateFromParent(idPortrait, ".CheatDeath");
    idHirelingPortrait = UIIDCreateFromParent(idRoot, ".Portrait");
    idHirelingHealthBar = UIIDCreateFromParent(idRoot, ".FollowerHealth");
    idHirelingName = UIIDCreateFromParent(idRoot, ".FollowerName");
    idLevel = UIIDCreateFromParent(idRoot, ".Level");
    idHealthBar = UIIDCreateFromParent(idRoot, ".Health");
    idLabelRegion = UIIDCreateFromParent(idRoot, ".PlayerLabel");
    idLevelRegion = UIIDCreateFromParent(idRoot, ".RemotePlayerLevel");
}




static void sFillDisplayInfo(const UIID & idHUDRoot, DisplayInfo & tInfo) {
    UIID idLocal = UIIDCreateFromParent(idHUDRoot, ".Local");
    tInfo.tLocal.FillDisplayInfo(idLocal);
    UIID idOnline = UIIDCreateFromParent(idHUDRoot, ".Online");
    tInfo.tOnline.FillDisplayInfo(idOnline);
    tInfo.idMain = idHUDRoot;
}








int32 PlayerGetHUDSlot(const Player * ptPlayer) {
    if (ptPlayer) {
        if (PlayerIsLocal(ptPlayer)) {
            uint32 nLocalSlot = 0;
            for (int8 i = 0; i < ptPlayer->nPlayerIndex; i++) {
                Player * ptLocalPlayer = PlayerGetByPlayerIndex(i);
                if (ptLocalPlayer && ptLocalPlayer->idPlayerACD != ACDID_INVALID && PlayerIsLocal(ptLocalPlayer)) {
                    nLocalSlot++;
                }
            }
            return nLocalSlot;
        } else {
            int32 nOnlineSlot = 0;
            int32 nLocalPlayerCount = PlayerGetLocalPlayerCount();
            Player * ptPlayerToCheck = PlayerGetFirstAll();
            while (ptPlayerToCheck && ptPlayerToCheck != ptPlayer) {
                if (!PlayerIsLocal(ptPlayerToCheck)) {
                    nOnlineSlot++;
                }
                ptPlayerToCheck = PlayerGetNextAll(ptPlayerToCheck);
            }
            return nOnlineSlot + nLocalPlayerCount;
        }
    }
    return 0;
}
}  // namespace Console
}  // namespace UIHUD
#include "../../Network/CTrickle.h"
#include "../../Console/GamerProfile.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../../Common/GameBalance.h"
const ClientTrickleEntry * CTrickleEntryFindForANN(const ACDNetworkName ann);  // X/Client/Network/CTrickle.cpp:160
GBID PlayerGetPortraitGBIDFromParagonLevel(int32 nLevel);  // X/Common/Player.cpp:5467
namespace UIDurabilityIndicator {
void UpdateDurabilityIndicator(const ActorCommonData & tACDPlayer, const UIID & idIndicator);  // X/Client/UserInterface/Interface/UIDurabilityIndicator.cpp:78
}
namespace UIUtils {
namespace Console {
void DisplayPortrait(PlayerClass eClass, BOOL bFemale, int32 nAltLevel, const UIID & idPortrait, GBID gbidPortraitFrame);  // X/Client/UserInterface/Console/UIConsoleUtils.cpp:1043
}
}
namespace UIHUD {
namespace Console {
UIID PlayerGetResourceUIControl(const Player * ptPlayer) {
    int32 nResourceSlot = ptPlayer->ePlayerClassPortrait ? 1 : 3;
    return UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.Resource%d", PlayerGetHUDSlot(ptPlayer) + 1, nResourceSlot));
}













DisplayInfo & GetDisplayInfoForPlayer(
    const Player * ptPlayer)
{
    int32 nSlot = PlayerGetHUDSlot(ptPlayer);
    return sgtHUDGlobals.tMultiPlayers[nSlot];
}



void SetBuffDisplayOffset(const Player * ptPlayer, int32 nBuffIndex, int32 nOffset) {
    int32 nSlot = PlayerGetHUDSlot(ptPlayer);
    DisplayInfo & tMPSlot = sgtHUDGlobals.tMultiPlayers[nSlot];
    UIID idWindowBase = tMPSlot.tLocal.idBuffPositive;
    int32 nRow = nOffset / BUFF_SLOTS_PER_ROW;
    int32 nOffsetInRow = nOffset % BUFF_SLOTS_PER_ROW;
    if (UI_TestID(idWindowBase)) {
        UIWindow & tWindow = UI_GetWindowFromID(idWindowBase);
        Rect2D rectBase;
        tWindow.GetWindowRect(rectBase);
        if (UI_TestID(tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idControl) && UI_TestID(tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idBuffIndicator)) {
            UIWindow & tControl = UI_GetWindowFromID(tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idControl);
            UIWindow & Buff = UI_GetWindowFromID(tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idBuffIndicator);
            Rect2D recBuff;
            Buff.GetClippingRect(recBuff);
            if (nSlot % 2 == 1) {
                nOffsetInRow = BUFF_SLOTS_PER_ROW - 1 - nOffsetInRow;
            }
            float flOffsetX = nOffsetInRow * (recBuff.Width() + BUFF_WIDTH_OFFSET);
            float flOffsetY = 0.0f;
            if (nRow > 0) {
                if (nSlot < 2) {
                    flOffsetY = -(recBuff.Height() + BUFF_HEIGHT_OFFSET);
                } else {
                    flOffsetY = recBuff.Height() + BUFF_HEIGHT_OFFSET;
                }
            }
            Vector2D vecTL;
            vecTL.x = rectBase.left + flOffsetX;
            vecTL.y = rectBase.top + flOffsetY;
            vecTL.SnapToIntegerCoords();
            tControl.MoveTo(vecTL);
        }
    }
}



























void UpdateStackCount(const Player * ptPlayer, int32 nBuffIndex, int32 nStackCount) {
    int32 nSlot = PlayerGetHUDSlot(ptPlayer);
    DisplayInfo & tMPSlot = sgtHUDGlobals.tMultiPlayers[nSlot];
    BOOL bNeedsUpdate = tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].nStackCount != nStackCount;
    tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].nStackCount = nStackCount;
    if (bNeedsUpdate && UI_TestID(tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idControl)) {
        UIID idText = UIIDCreateFromParent(tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idControl, ".StackCount");
        if (idText != GetUIIDNone()) {
            UIText & tText = UI_GetTextFromID(idText);
            if (nStackCount > 1 || tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].bShowSingleStack) {
                tText.SetText(CRefString().Format("%d", nStackCount), FALSE);
            } else {
                tText.SetText(CRefString().Format(""), FALSE);
            }
        }
    }
}




} }
#include "../UIInit.h"
#include "../UICreation.h"
#include "../../ImageCollection.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../Controls/UIBlinkerControl.h"
extern UIID UIWND_CONSOLE_HUD; BOOL PowerBuffIconDisplayDuration(AutoPowerDefinitionR pdefPower, int32 nBuffIndex); // UIIDs.h / PowersCommon.cpp:1035
namespace UIHUD { namespace Console {







void SetDisplayBuffForPlayer(
    const Player * ptPlayer,
    int32 nBuffIndex,
    eBuffType eType,
    const BuffInfo & tBuffInfo)
{


    int32 nSlot = PlayerGetHUDSlot(ptPlayer);
    DisplayInfo & tMPSlot = sgtHUDGlobals.tMultiPlayers[nSlot];

    tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].eType = eType;
    tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].snoPower = tBuffInfo.snoPower;
    tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].nPowerBuffIndex = tBuffInfo.nBuffIdx;
    tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].ePetType = tBuffInfo.ePetType;

    CRefString szRootId;
    szRootId.Format("%s.Player%d", UIWND_CONSOLE_HUD_PLAYER_SLOTS.GetIDAsString().str(), nSlot + 1);

    SNOName snoname(UI_SNO, "ConsoleHUD_SinglePlayerFrame");

    UIWindow * pControl;

    if (nSlot % 2 == 0)
    {
        UIControlSourceKey tTemplateSrcKey(snoname.snoHandle, "ConsoleHUD_SinglePlayerFrame_main.LayoutRoot.BuffTemplate");
        pControl = (UIWindow *)UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("Player%dbuffEntry%d", nSlot, nBuffIndex), UIWND_CONSOLE_HUD), NULL);
    }
    else
    {
        UIControlSourceKey tTemplateSrcKey(snoname.snoHandle, "ConsoleHUD_SinglePlayerFrame_main.LayoutRoot.BuffTemplateFlipped");
        pControl = (UIWindow *)UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("Player%dbuffEntry%d", nSlot, nBuffIndex), UIWND_CONSOLE_HUD), NULL);
    }

    tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idControl = pControl->GetID();
    pControl->SetActive(TRUE);




    UIID idBuffIndicator = UIIDCreateFromParent(pControl->GetID(), ".BuffIcon");
    tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].idBuffIndicator = idBuffIndicator;

    UIConsoleBuffIndicator & tHIndicator = UI_GetConsoleBuffIndicatorFromID(idBuffIndicator);

    ImageHandle hImage = tBuffInfo.hImage;
    if (hImage)
    {
        ImageTextureFrame tImageTexFrame;
        ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tImageTexFrame);
        tHIndicator.SetTextureFrame(tImageTexFrame);

        Rect2D clipRect;
        tHIndicator.GetClippingRect(clipRect);

        if (eType != BUFF_TYPE_PET && PowerBuffIconDisplayDuration(AutoPowerDefinitionR(tBuffInfo.snoPower), tBuffInfo.nBuffIdx))
        {
            TimerCreationParams tTimerParams(UIT_TIMER, "timer", idBuffIndicator);
            UIID idTimer = CreateControl<UITimer>(tTimerParams, clipRect);

            UI_GetTimerFromID(idTimer).SetReverseColor(TRUE);
            tHIndicator.SetTimerId(idTimer);
        }
    }
    tHIndicator.SetBuffData(tMPSlot.tLocal.arCurrentBuffs[nBuffIndex].snoPower, 1);

    if (eType == BUFF_TYPE_DEBUFF)
    {

        UIID idDebuff = UIIDCreateFromParent(pControl->GetID(), ".Debuff");
        UIBlinker & Debuff = UI_GetBlinkerFromID(idDebuff);
        Debuff.SetActive(TRUE);
    }
    else if (eType != BUFF_TYPE_PET)
    {


        UIID idBuff = UIIDCreateFromParent(pControl->GetID(), ".Buff");
        UIWindow & Buff = UI_GetWindowFromID(idBuff);
        Buff.SetActive(TRUE);
    }


}



inline void ClearDisplayBuffForSlot(uint32 nSlot, uint32 index) {
    DisplayInfo::DisplayBuff & tBuff = sgtHUDGlobals.tMultiPlayers[nSlot].tLocal.arCurrentBuffs[index];
    tBuff.eType = BUFF_TYPE_NONE;
    tBuff.snoPower = SNO_NONE;
    tBuff.nPowerBuffIndex = -1;
    tBuff.ePetType = -1;
    tBuff.idControl = GetUIIDNone();
    tBuff.idBuffIndicator = GetUIIDNone();
    tBuff.nStackCount = 0;
    tBuff.bShowSingleStack = FALSE;
}






inline void ClearDisplayBuffForPlayer(const Player * ptPlayer, uint32 index) {
    int32 nSlot = PlayerGetHUDSlot(ptPlayer);
    ClearDisplayBuffForSlot(nSlot, index);
}






void GetBuffDisplayInfoForPlayer(const Player * ptPlayer, UIID & idBuffRegion, UIID & idBuffPositive, UIID & idBuffNegative) {
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    idBuffRegion = tInfo.tLocal.idBuffRegion;
    idBuffPositive = tInfo.tLocal.idBuffPositive;
    idBuffNegative = tInfo.tLocal.idBuffNegative;
}








float GetAdjustedHPFrac(
    const float flFrac)
{
    const Vector2D cvecInflection1(0.5f, 0.45f);
    const Vector2D cvecInflection2(0.9f, 0.75f);
    if (flFrac < cvecInflection1.x)
    {
        float flSlope = cvecInflection1.y / cvecInflection1.x;
        return flFrac * flSlope;
    }
    else if (flFrac < cvecInflection2.x)
    {
        float flSlope = (cvecInflection2.y - cvecInflection1.y) / (cvecInflection2.x - cvecInflection1.x);
        return cvecInflection1.y + (flFrac - cvecInflection1.x) * flSlope;
    }
    else
    {
        float flSlope = (1.0f - cvecInflection2.y) / (1.0f - cvecInflection2.x);
        return cvecInflection2.y + (flFrac - cvecInflection2.x) * flSlope;
    }
}


static void sGetHealthFracForACD(
    const ActorCommonData & tACD,
    float & flAdjustedFrac,
    float & flAdjustTargetFrac)
{
    float flHPMax = tACD.GetHitPointsMaxTotal();
    float flHPCur = tACD.GetHitPointsCurrent();
    float flHPTarget = Math::Max(flHPCur, tACD.AttributesGetFloat(ATTRIBUTE_HITPOINTS_HEALED_TARGET));
    float flFrac = flHPCur / flHPMax;
    float flTargetFrac = flHPTarget / flHPMax;
    flAdjustedFrac = GetAdjustedHPFrac(flFrac);
    flAdjustTargetFrac = GetAdjustedHPFrac(flTargetFrac);
}


void UpdateHealthGlobe(
    DisplayInfo::LocalHUD & tHUD,
    const ActorCommonData & tACD)
{
    UIRActor & tHealthBall = UI_GetRActorFromID(tHUD.idHealthGlobe);
    UIRActor & tHealthBallTarget = UI_GetRActorFromID(tHUD.idHealthGlobeTarget);
    RActorChangeLook(tHealthBall.GetRActor(), "Blood_liquid");
    RActorChangeLook(tHealthBallTarget.GetRActor(), "Blood_forecast");
#line 822
    float flAdjustedFrac;
#line 822
    float flAdjustTargetFrac;
    sGetHealthFracForACD(tACD, flAdjustedFrac, flAdjustTargetFrac);
#line 825
    UIProgressBar & tProgressBarHealth = UI_GetProgressBarFromID(tHUD.idHealthBar);
    tProgressBarHealth.SetExtentsAndPosition(0.0f, 1.0f, flAdjustedFrac, flAdjustTargetFrac);
#line 828
    UIProgressBar & tProgressBarLowHealth = UI_GetProgressBarFromID(tHUD.idLowHealthBar);
    tProgressBarLowHealth.SetExtentsAndPosition(0.0f, 1.0f, flAdjustedFrac, flAdjustTargetFrac);
#line 831
    UIRActor & tLowHealthGlow = UI_GetRActorFromID(tHUD.idLowHealthGlow);
    if (flAdjustedFrac <= 0.33f)
    {
        tProgressBarLowHealth.SetActive(TRUE);
        tLowHealthGlow.SetActive(TRUE);
        if (tACD.IsAlive())
        {
            // Inferred const removal: DWARF const tACD calls the original non-const effect method.
            const_cast<ActorCommonData &>(tACD).AddClientEffect(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_LOW_HEALTH), 0);
        }
        tHUD.bLowHealthActive = TRUE;
    }
    else if (tHUD.bLowHealthActive)
    {
        tProgressBarLowHealth.SetActive(FALSE);
        tLowHealthGlow.SetActive(FALSE);
        // Inferred const removal: DWARF const tACD calls the original non-const effect method.
        const_cast<ActorCommonData &>(tACD).RemoveClientEffect(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_LOW_HEALTH));
        tHUD.bLowHealthActive = FALSE;
    }
#line 856
    SNO snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_HEALTHPOTIONUSE);
    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (flAdjustedFrac <= 0.75f && !Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial) && !tACD.IsInCombat() && tACD.IsAlive())
    {
#line 862
        const HotbarButtonData & tPotionData = PlayerGetPotionBtnAssignment(ptPlayer);
        int32 nPotionCount = ActorInventoryCountItemsOfTypeOnPlayer(tACD.id, tPotionData.m_gbidItem, NULL);
        if (nPotionCount > 0)
        {
            gtTutorialMgr.ShowTutorial(tHUD.idHealthBar, snoTutorial, ptPlayer, TRUE);
            Tutorial::Shown(*ptPlayer, GlobalSNOGet(GLOBALSNO_TUTORIAL_DAMAGE_TAKEN));
        }
        else
        {
            gtTutorialMgr.ShowTutorial(tHUD.idHealthBar, GlobalSNOGet(GLOBALSNO_TUTORIAL_DAMAGE_TAKEN), ptPlayer, TRUE);
        }
    }
}







#line 880
// LINKER-REMOVED: DWARF 881 (low_pc 0, 0x54 bytes), one inlined copy in the CU; body from DWARF locals and line columns, needed as an out-of-line caller; exact expressions inferred.
void UpdatePlayerDeathIndicator(
    const UIID & idDeath,
    const ActorCommonData & tACD)
{
    UIWindow & tWindow = UI_GetWindowFromID(idDeath);

    if (!tACD.IsAlive())
    {
        tWindow.SetActive(TRUE);
    }
    else
    {
        tWindow.SetActive(FALSE);
    }
}


static void sOnRestXPFadeInDone(const UIID & id) {
    int32 nPlayerIndex = (int32)(intptr_t)UI_GetStoryboardFromID(id).GetUserData();
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer) {
        DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
        UI_GetRActorFromID(UIIDCreateFromParent(tInfo.tLocal.idPoRXPMarker, ".GrantedFX")).SetActive(FALSE);
    }
}





static void sOnRestXPDestroyDone(const UIID & id) {
    int32 nPlayerIndex = (int32)(intptr_t)UI_GetStoryboardFromID(id).GetUserData();
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer) {
        DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
        UI_GetRActorFromID(UIIDCreateFromParent(tInfo.tLocal.idMain, ".LostFX")).SetActive(FALSE);
        UI_GetRActorFromID(UIIDCreateFromParent(tInfo.tLocal.idMain, ".ExpiredFX")).SetActive(FALSE);
    }
}





void UpdateXPBar(const UIID & idMain, const UIID & idBar, const UIID & idAltBar, const UIID & idPoRXPMarker, const ActorCommonData & tACD, const Player * ptPlayer)
{
    uint64 nExpDiff = 0;
    uint64 nExpRemaining = 0;
    int32 nLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
    Vector2D vecBarTL;
    Vector2D vecBarDim;









    if (!tACD.ShouldUseAltLevels())
    {
        UI_GetProgressBarFromID(idAltBar).SetActive(FALSE);

        UIProgressBar & tProgressBar = UI_GetProgressBarFromID(idBar);
        tProgressBar.SetActive(TRUE);

        tProgressBar.GetRelativePosition(vecBarTL, vecBarDim);

        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));

        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;

        nExpDiff = tExpTable.ptExperienceLevels[nLevel - 1].nDeltaXP;

        // a level of 0 means the data is not there yet

        if (nLevel == 0)
        {
            if (UI_TestID(idPoRXPMarker))
            {
                UI_GetProgressBarFromID(idPoRXPMarker).SetActive(FALSE);
            }
            return;
        }

        nExpRemaining = Math::Min(tACD.GetExperienceNext(), nExpDiff);

        tProgressBar.SetExtentsAndPosition(0.0f, (float)nExpDiff, (float)(nExpDiff - nExpRemaining), -1.0f);
    }
    else
    {
        UIProgressBar & tProgressBar = UI_GetProgressBarFromID(idAltBar);

        tProgressBar.SetActive(TRUE);

        tProgressBar.GetRelativePosition(vecBarTL, vecBarDim);

        UI_GetProgressBarFromID(idBar).SetActive(FALSE);

        int32 nLevelScoped = tACD.AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
        if (nLevelScoped >= 0 && nLevelScoped <= 20000)
        {
            ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT));
            const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;

            nExpDiff = tExpTableAlt.ptExperienceLevelsAlt[Math::Min(nLevelScoped, 19999)].nDeltaXP;

            nExpRemaining = Math::Min(tACD.GetAltExperienceNext(), nExpDiff);
        }

        tProgressBar.SetExtentsAndPosition(0.0f, (float)nExpDiff, (float)(nExpDiff - nExpRemaining), -1.0f);
    }


    if (!UI_TestID(idPoRXPMarker))
        return;

    ScopedGlobalDefinitionLockR pdefGlobal;

    UIWindow & tRestXPMarker = UI_GetWindowFromID(idPoRXPMarker);

    uint64 nRestXP = tACD.GetRestExperience();
    uint64 nRestXPExpiration = nRestXP + Math::FloatRoundToInt((float)nRestXP / pdefGlobal->flRestXPBonusPercent);
    if (nRestXPExpiration)
    {
        float flXPBarScalar = 1.0f;
        if (nRestXPExpiration < nExpRemaining)
            flXPBarScalar = (float)(nExpDiff - nExpRemaining + nRestXPExpiration) / (float)nExpDiff;

        Vector2D vecTL;
        Vector2D vecDim;
        tRestXPMarker.GetRelativePosition(vecTL, vecDim);

        // the marker sits where the bar would end with the rest XP added; odd slots fill right to left
        vecTL.x = vecBarTL.x + (((PlayerGetHUDSlot(ptPlayer) & 1) ? vecBarDim.x - vecBarDim.x * flXPBarScalar : vecBarDim.x * flXPBarScalar) - vecDim.x * 0.5f);
        vecTL.y = vecBarTL.y - (vecDim.y - vecBarDim.y) * 0.5f;

        tRestXPMarker.SetRelativePosition(vecTL, vecDim);

        gtTutorialMgr.ShowTutorial(idPoRXPMarker, GlobalSNOGet(GLOBALSNO_TUTORIAL_REST_XP), ptPlayer, TRUE);
    }

    tRestXPMarker.SetActive(nRestXPExpiration != 0);

    if (!sgarLastKnownRestXP[ptPlayer->nPlayerIndex] && nRestXP)
    {
        UIStoryboard & tFadeIn = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_REST_EXP_MARKER_FADEIN);
        tFadeIn.SetUserData((void *)(intptr_t)ptPlayer->nPlayerIndex);
        tFadeIn.SetOnEndCallback(sOnRestXPFadeInDone);
        tFadeIn.Reset();
        tFadeIn.Play(FALSE, 0.0f);

        UIRActor & tFX = UI_GetRActorFromID(UIIDCreateFromParent(idPoRXPMarker, ".GrantedFX"));
        tFX.SetActive(FALSE);
        tFX.SetActive(TRUE);
    }
    else if (!nRestXP && sgarLastKnownRestXP[ptPlayer->nPlayerIndex])
    {
        UIStoryboard & tFadeIn = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_REST_EXP_MARKER_DESTROY);
        tFadeIn.SetUserData((void *)(intptr_t)ptPlayer->nPlayerIndex);
        tFadeIn.SetOnEndCallback(sOnRestXPDestroyDone);
        tFadeIn.Reset();
        tFadeIn.Play(FALSE, 0.0f);

        if (tACD.IsAlive())
        {
            UIRActor & tExpiredFX = UI_GetRActorFromID(UIIDCreateFromParent(idMain, ".ExpiredFX"));
            Vector2D vecBulletDim;
            Vector2D vecBulletTL;
            Vector2D vecFXTL;
            Vector2D vecFXDim;
            tRestXPMarker.GetRelativePosition(vecBulletTL, vecBulletDim);
            tExpiredFX.GetRelativePosition(vecFXTL, vecFXDim);

            vecFXTL.x = vecBulletTL.x + (vecBulletDim.x - vecFXDim.x) * 0.5f;
            vecFXTL.y = vecBulletTL.y + (vecBulletDim.y - vecFXDim.y) * 0.5f;

            tExpiredFX.SetRelativePosition(vecFXTL, vecFXDim);
            tExpiredFX.SetActive(FALSE);
            tExpiredFX.SetActive(TRUE);
        }
        else
        {
            UIRActor & tLostFX = UI_GetRActorFromID(UIIDCreateFromParent(idMain, ".LostFX"));
            Vector2D vecBulletDim;
            Vector2D vecBulletTL;
            Vector2D vecFXTL;
            Vector2D vecFXDim;
            tRestXPMarker.GetRelativePosition(vecBulletTL, vecBulletDim);
            tLostFX.GetRelativePosition(vecFXTL, vecFXDim);

            vecFXTL.x = vecBulletTL.x + (vecBulletDim.x - vecFXDim.x) * 0.5f;
            vecFXTL.y = vecBulletTL.y + (vecBulletDim.y - vecFXDim.y) * 0.5f;

            tLostFX.SetRelativePosition(vecFXTL, vecFXDim);
            tLostFX.SetActive(FALSE);
            tLostFX.SetActive(TRUE);

            if (PlayerGetLocalPlayerCount() == 1 && ptPlayer->nControllerIndex == 0)
            {
                sgbLostBonusXP = true;
            }
        }
    }

    sgarLastKnownRestXP[ptPlayer->nPlayerIndex] = nRestXP;
}
















void UpdateTutorials(const Player * ptPlayer, const ActorCommonData & tACDPlayer) {
    SNO snoTutorial;
    if (GameIsSeasonal()) {
        int32 & nSeenSeasonTutorials = Prefs::GetSeasonTutorialsSeen();
        if ((uint32)nSeenSeasonTutorials < OnlineService::GetSeasonNum()) {
            Player * ptNonConstPlayer = PlayerGetByACD(tACDPlayer.id);
            if (ptNonConstPlayer) {
                SNO snoSeasonJourneyTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_SEASON_JOURNEY);
                SNO snoLeaderboardTutorial1 = GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_LEADERBOARDS_1);
                SNO snoLeaderboardTutorial2 = GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_LEADERBOARDS_2);
                Tutorial::Reset(*ptNonConstPlayer, snoSeasonJourneyTutorial);
                Tutorial::Reset(*ptNonConstPlayer, snoLeaderboardTutorial1);
                Tutorial::Reset(*ptNonConstPlayer, snoLeaderboardTutorial2);
                nSeenSeasonTutorials = OnlineService::GetSeasonNum();
            }
        }
        SNO snoSeasonJourneyTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_SEASON_JOURNEY);
        if (!Tutorial::PlayerHasSeen(*ptPlayer, snoSeasonJourneyTutorial) && !gtTutorialMgr.IsAnyShowing(0) && (!UI_TestID(UIDLG_QUESTREWARD) || !UI_GetWindowFromID(UIDLG_QUESTREWARD).IsActive())) {
            gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoSeasonJourneyTutorial, ptPlayer, TRUE);
        }
        if (!::Console::Screens::GetFanfareManager().IsActive()) {
            if (ptPlayer->nCurrentLevel >= 10) {
                SNO snoLeaderboardTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_CONSOLE_LEADERBOARDS_1);
                if (!Tutorial::PlayerHasSeen(*ptPlayer, snoLeaderboardTutorial) && !gtTutorialMgr.IsAnyShowing(0) && (!UI_TestID(UIDLG_QUESTREWARD) || !UI_GetWindowFromID(UIDLG_QUESTREWARD).IsActive())) {
                    gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoLeaderboardTutorial, ptPlayer, TRUE);
                }
            }
            if (!Tutorial::PlayerHasSeen(*ptPlayer, snoSeasonJourneyTutorial) && !gtTutorialMgr.IsAnyShowing(0) && (!UI_TestID(UIDLG_QUESTREWARD) || !UI_GetWindowFromID(UIDLG_QUESTREWARD).IsActive())) {
                gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoSeasonJourneyTutorial, ptPlayer, TRUE);
            }
        }
    }
    snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_HEARTH_UNLOCKED);
    if (!Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial) && !gtTutorialMgr.IsAnyShowing(0) && (!UI_TestID(UIDLG_QUESTREWARD) || !UI_GetWindowFromID(UIDLG_QUESTREWARD).IsActive())) {
        SNO snoStoneOfRecallPower = GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL);
        BOOL fStoneOfRecallUnlocked = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoStoneOfRecallPower));
        if (fStoneOfRecallUnlocked) {
            gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoTutorial, ptPlayer, TRUE);
        }
    }
}

// 1208-1214 statement shapes (in-place *= and the 1213 local) from the original columns 1210:35, 1213:47, 1214:58/81/95.





































inline void ShowControl(const UIID & id, const BOOL bShow) {
    if (UI_TestID(id)) {
        UI_GetControlFromID(id).SetActive(bShow);
    }
}




void UpdateResourceBar(const UIID & idBar, ResourceType eResourceType, const ActorCommonData & tACD) {
    if (UI_TestID(idBar)) {
        float flEffectiveResourceMax = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_EFFECTIVE_MAX, eResourceType));
        float flResourceMax = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResourceType));
        float flResource = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_CUR, eResourceType));
        float flReservedPercent = (flResourceMax - flEffectiveResourceMax) / flResourceMax;
        if (flEffectiveResourceMax > 1e-06f) {
            flReservedPercent *= 0.8f;
        }
        float flNonReservedPercent = 1.0f - flReservedPercent;
        float flResourcePosition = flReservedPercent + flNonReservedPercent * (flResource / flEffectiveResourceMax);
        UIProgressBar & tProgressBarResource = UI_GetProgressBarFromID(idBar);
        tProgressBarResource.SetExtentsAndPosition(0.0f, 1.0f, flResourcePosition, -1.0f);
    }
}












// LINKER-REMOVED: DWARF 1222 (low_pc 0, 0x78 bytes), one inlined copy in the CU; body from DWARF locals and line columns, needed as an out-of-line caller; exact expressions inferred.
void UpdateResourceBars(
    const DisplayInfo::LocalHUD & tHUD,
    const ActorCommonData & tACDPlayer)
{
    UpdateResourceBar(tHUD.idResourceBar1, tACDPlayer.GetPrimaryResourceType(), tACDPlayer);
    UpdateResourceBar(tHUD.idResourceBar2, tACDPlayer.GetPrimaryResourceType(), tACDPlayer);
    UpdateResourceBar(tHUD.idResourceBar3, tACDPlayer.GetSecondaryResourceType(), tACDPlayer);
}

// DWARF DW_INL_inlined only: preserve natural standalone emission; original discarded body is not checkable.
SkillButton CPlayerUtilSkillButtonFromInputButton(
    InputButton eButton)
{
    switch (eButton)
    {
    case 0: return 0;
    case 1: return 4;
    case 2: return 2;
    case 3: return 3;
    case 4:
    case 5:
    case 6: return 5;
    case 7: return 1;
    default: return -1;
    }
}




} }
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../../Contrib/Contrib/blz/include/blz/func_poly.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
BOOL PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR, int32); BOOL CPlayerInputIsSkillDisplayedAsUnusable(const Player *, const SNO); BOOL PowerIsBasicAttack(AutoPowerDefinitionR); int32 PowerActorGetChargeCost(const ActorCommonData &, SNO); // DWARF CPlayerInput.cpp:6613, PowersCommon.cpp:735/4339
namespace UIBuffs { void EnumBuffsOnSkillButton(SNO snoPower, blz::function<void(SortedBuff &)> fnCallback); int32 GetStackCountForSkillButton(SNO, const Player *); } namespace Controller { CRefString GetActionButtonIconName(const int32, const Player *, const BOOL); int32 GetSlotFromButton(InputButton, const Player *); } // DWARF UIBuffs.cpp:2341, ControllerSchemeDefinition.cpp:424/434
namespace UIHUD { namespace Console {

void sUpdateSkillButtonProgressBar(const UIID & idButton, const SNO snoPower, const Player * ptPlayer)
{
    UIID idProgressFrame = UIIDCreateFromParent(idButton, ".Duration");
    UIWindow & tProgressFrame = UI_GetWindowFromID(idProgressFrame);
    tProgressFrame.SetActive(FALSE);
    if (snoPower == SNO_NONE)
    {
        return;
    }





    blz::vector<UIBuffs::SortedBuff *> vecBuffsFound;
    UIBuffs::EnumBuffsOnSkillButton(snoPower, [ptPlayer, &vecBuffsFound](UIBuffs::SortedBuff & tBuff) {
        if (tBuff.ptPlayer == ptPlayer &&
            PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR(tBuff.snoPower), tBuff.nBuffIdx))
        {
            vecBuffsFound.push_back(&tBuff);
        }
    });

    float flLargestPos = 0.0f;

    for (UIBuffs::SortedBuff * ptBuff : vecBuffsFound)
    {
        Attrib eAttribStart;
        Attrib eAttribEnd;
        Attrib eAttribCount;
        PowerGetBuffAttributes(ptBuff->nBuffIdx, eAttribStart, eAttribEnd, eAttribCount);

        ActorCommonData * ptACDAffected = ptBuff->ptPlayer ? ACDTryToGet(ptBuff->ptPlayer->idPlayerACD) : NULL;
        if (ptACDAffected)
        {
            uint32 dwStartTick = ptACDAffected->AttributesGetInt(MakeAttribKey(eAttribStart, ptBuff->snoPower));
            uint32 dwEndTick = ptACDAffected->AttributesGetInt(MakeAttribKey(eAttribEnd, ptBuff->snoPower));
            uint32 dwNow = CGameGetPredictedServerTime();
            float flPos;

            if (dwNow > dwEndTick)
            {
                ptBuff->nDuration = 0;
                flPos = 0.0f;
            }
            else
            {
                int32 nDelta = dwEndTick - dwStartTick;
                int32 nTimeLeft = dwEndTick - dwNow;
                if (ptBuff->nDuration < 0)
                {
                    ptBuff->nDuration = nDelta;
                }
                else if (nTimeLeft > ptBuff->nDuration)
                {
                    ptBuff->nDuration = nTimeLeft;
                }
                dwStartTick = dwEndTick - ptBuff->nDuration;
                flPos = 1.0f - (float)(dwNow - dwStartTick) / (float)(dwEndTick - dwStartTick);
            }

            if (flPos > flLargestPos)
                flLargestPos = flPos;
        }
    }








    tProgressFrame.SetActive(flLargestPos >= 0.000001f);

    UIID idGreenBar = UIIDCreateFromParent(idProgressFrame, ".ProgressBar");
    UIID idYellowBar = UIIDCreateFromParent(idProgressFrame, ".ProgressBarYellow");

    UIBuffs::UpdateBuffProgressBar(idGreenBar, idYellowBar, flLargestPos);
}


void UpdateSkills(
    const DisplayInfo::LocalHUD & tHUD,
    const Player * ptPlayer)
{
    UIID idStoryboard = UIIDCreate("Root.NormalLayer.ConsoleHUD_SinglePlayerFrame_main.SkillSlotPulse");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    // Inference: the flag accumulates any dirty slot over the six-button loop.
    BOOL bPlayNewSkillAnim = FALSE;

    for (int32 i = 0; i < DisplayInfo::SKILL_BUTTON_COUNT; i++)
    {
        const DisplayInfo::SkillButtonInfo & tSkillInfo = tHUD.arSkills[i];

        if (!UI_TestID(tSkillInfo.idButton))
        {
            continue;
        }

        SkillButton eSkillButton = CPlayerUtilSkillButtonFromInputButton(tSkillInfo.eControllerButton); // Inference: invalid input hides the button before continuing.
        if (eSkillButton == -1)
        {
            UI_GetControlFromID(tSkillInfo.idButton).SetActive(FALSE);
            continue;
        }

        const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, eSkillButton);
        BOOL fDisable;

        if (tButtonData.m_snoPower != SNO_NONE &&
            !PowerIsBasicAttack(AutoPowerDefinitionR(tButtonData.m_snoPower)))
        {
            fDisable = CPlayerInputIsSkillDisplayedAsUnusable(ptPlayer, tButtonData.m_snoPower);
        }
        else
        {
            UI_GetControlFromID(tSkillInfo.idButton).SetActive(FALSE);
            continue;
        }


        UI_GetControlFromID(tSkillInfo.idButton).SetActive(TRUE);

        UIID idHotbarButton = UIIDCreateFromParent(tSkillInfo.idButton, ".Skill");
        UIHotbarButton & tButton = UI_GetHotbarButtonFromID(idHotbarButton);
        tButton.SetPlayer(ptPlayer);
        tButton.UpdateSkillButtonState(tButtonData, fDisable, TRUE, TRUE, TRUE, FALSE);


        UIGame::ApplyActiveBuffButtonEffect(ptPlayer, i, tButtonData);


        sUpdateSkillButtonProgressBar(tSkillInfo.idButton, tButtonData.m_snoPower, ptPlayer);

        UIID idCooldownCover = UIIDCreateFromParent(tSkillInfo.idButton, ".SkillCooldownCover");
        UIWindow & tCooldownCover = UI_GetWindowFromID(idCooldownCover);

        UIID idSpark = UIIDCreateFromParent(tSkillInfo.idButton, ".CooldownSpark");
        UIWindow & tCooldownSpark = UI_GetWindowFromID(idSpark);

        int32 nTimerDuration = tButton.GetDurationRemaining();
        if (nTimerDuration > 0)
        {
            tCooldownCover.SetActive(TRUE);

            UIID idTimer = tButton.GetTimerID();

            if (UI_TestID(idTimer))
            {
                UIConsoleSkillsTimer * ptTimer = &UI_GetConsoleSkillsTimerFromID(idTimer); // Inference: DWARF nested block preserves the null-timer guard.
                if (ptTimer)
                {
                    Vector2D vecUIC;
                    ConvertPixelsToUIC(ptTimer->GetLeadingEdgePos(), vecUIC);
                    vecUIC.x -= tCooldownSpark.GetWindowRect().Width() * 0.5f;
                    vecUIC.y -= tCooldownSpark.GetWindowRect().Height() * 0.5f;
                    tCooldownSpark.MoveTo(vecUIC);
                    tCooldownSpark.SetActive(TRUE);
                    tCooldownSpark.SetNoSnap(TRUE);
                }
            }
        }
        else
        {
            if (tCooldownSpark.IsActive())
            { // Inference: DWARF const tSkillInfo invokes the non-const flasher method.
                const_cast<SkillCooldownFlasher &>(tSkillInfo.tFlasher).Trigger();
            }

            tCooldownCover.SetActive(FALSE);
            tCooldownSpark.SetActive(FALSE);
        }

        // Inference: DWARF const tSkillInfo invokes the original non-const Update method.
        const_cast<SkillCooldownFlasher &>(tSkillInfo.tFlasher).Update();

        UIID idChargesWrapper = UIIDCreateFromParent(tSkillInfo.idButton, ".ChargesWrapper");
        UIID idCharges = UIIDCreateFromParent(idChargesWrapper, ".ChargeCount");
        UIWindow & tChargesWrapper = UI_GetWindowFromID(idChargesWrapper);
        UIText & tCharges = UI_GetTextFromID(idCharges);

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        { // Inference: use the canonical GetData accessor for original button data loads.
            int32 nChargeCost = PowerActorGetChargeCost(*ptACDPlayer, tButton.GetData().m_snoPower);
            if (nChargeCost > 0)
            {
                int32 nCount = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, tButton.GetData().m_snoPower));
                tChargesWrapper.SetActive(TRUE);
                tCharges.SetText(CRefString().Format("%d", nCount), FALSE);
            }
            else
            {
                tChargesWrapper.SetActive(FALSE);
            }
        }

        UIID idStackCountWrapper = UIIDCreateFromParent(tSkillInfo.idButton, ".StackCountWrapper");
        UIID idStackCount = UIIDCreateFromParent(idStackCountWrapper, ".StackCount");
        UIWindow & tStackCountWrapper = UI_GetWindowFromID(idStackCountWrapper);
        UIText & tStackCount = UI_GetTextFromID(idStackCount);
        // Inference: same canonical GetData accessor for the original power-SNO load.
        int32 nStackCount = UIBuffs::GetStackCountForSkillButton(tButton.GetData().m_snoPower, ptPlayer);
        if (nStackCount > 1)
        {
            tStackCount.SetText(UnicodeText::FormatNumber(nStackCount, 1, FNS_NORMAL, 0), FALSE);
        }
        tStackCountWrapper.SetActive(nStackCount > 1);

        UIID idBinding = UIIDCreateFromParent(tSkillInfo.idButton, ".KeyBinding");
        UIText & tBinding = UI_GetTextFromID(idBinding);

        tBinding.SetControllerIndexOverride(ptPlayer->nControllerIndex);
        tBinding.SetText(Controller::GetActionButtonIconName(Controller::GetSlotFromButton(tSkillInfo.eControllerButton, ptPlayer), ptPlayer, TRUE).str(), FALSE);


        UIID idSelected = UIIDCreateFromParent(tSkillInfo.idButton, ".Selected");
        UIWindow & tSelected = UI_GetWindowFromID(idSelected);
        if (tSkillInfo.bDirty)
        {
            tSelected.SetActive(TRUE);
            bPlayNewSkillAnim = TRUE; // Inference: original clears bDirty through const tSkillInfo.
            const_cast<DisplayInfo::SkillButtonInfo &>(tSkillInfo).bDirty = FALSE;
        }
        else
        if (!tStoryboard.IsPlaying())
        {

            tSelected.SetActive(FALSE);
        }
    }

    if (bPlayNewSkillAnim)
    {

        tStoryboard.Play(0, 0.0f);
    }
}


void UpdatePotion(
    const DisplayInfo::LocalHUD & tHUD,
    const Player * ptPlayer)
{
    if (!UI_TestID(tHUD.idPotionIcon)) return;
    if (!UI_TestID(tHUD.idPotionButtonText)) return;
    if (!UI_TestID(tHUD.idPotionCount)) return;
    if (!UI_TestID(tHUD.idPotionCountShadow)) return;
    if (!UI_TestID(tHUD.idPotionCooldownCover)) return;
    if (!UI_TestID(tHUD.idPotionCooldownSpark)) return;

    const HotbarButtonData & tPotionData = PlayerGetPotionBtnAssignment(ptPlayer);
    GBID gbidItem;

    ACDID idItem = ACDFromANN(tPotionData.m_annItem);
    if (idItem != ACDID_INVALID)
    {
        ActorCommonData & tACDItem = ACDGet(idItem);
        gbidItem = tACDItem.hGB.gbid;
    }
    else
    {
        gbidItem = tPotionData.m_gbidItem;
    }

    int32 nPotionCount = ActorInventoryCountItemsOfTypeOnPlayer(ptPlayer->idPlayerACD, gbidItem, &idItem);

    UI_GetWindowFromID(tHUD.idPotionCountShadow).SetActive(FALSE);
    UI_GetWindowFromID(tHUD.idPotionCount).SetActive(FALSE);

    UIHotbarButton & tPotionButton = UI_GetHotbarButtonFromID(tHUD.idPotionIcon);
    BOOL bUnusable = nPotionCount < 1;
    tPotionButton.UpdateSkillButtonState(tPotionData, bUnusable, TRUE, TRUE, FALSE, FALSE);

    UIText & tPotionButtonText = UI_GetTextFromID(tHUD.idPotionButtonText);
    CRefString sIconName = Controller::GetPotionButtonIconName(ptPlayer);
    if (sIconName.Length())
    {
        tPotionButtonText.SetControllerIndexOverride(ptPlayer->nControllerIndex);
        tPotionButtonText.SetText(sIconName.str(), FALSE);
    }

    if (nPotionCount != sgtHUDGlobals.nLastPotionCount[ptPlayer->nPlayerIndex])
    {
        UI_GetTextFromID(tHUD.idPotionCount).SetText(UnicodeText::FormatNumber(nPotionCount, TRUE, FNS_NORMAL, FALSE), FALSE);
        sgtHUDGlobals.nLastPotionCount[ptPlayer->nPlayerIndex] = nPotionCount;
    }


    int32 nTimerDuration = tPotionButton.GetDurationRemaining();
    UIWindow & tPotionCooldownCover = UI_GetWindowFromID(tHUD.idPotionCooldownCover);
    UIWindow & tCooldownSpark = UI_GetWindowFromID(tHUD.idPotionCooldownSpark);
    if (nTimerDuration > 0)
    {
        tPotionCooldownCover.SetActive(TRUE);
        tCooldownSpark.SetActive(TRUE);
        UIID idTimer = tPotionButton.GetTimerID();
        if (UI_TestID(idTimer))
        {
            UIConsoleSkillsTimer * ptTimer = &UI_GetConsoleSkillsTimerFromID(idTimer);


            Vector2D vecUIC;
            ConvertPixelsToUIC(ptTimer->GetLeadingEdgePos(), vecUIC);
            vecUIC.x -= tCooldownSpark.GetWindowRect().Width() * 0.5f;
            vecUIC.y -= tCooldownSpark.GetWindowRect().Height() * 0.5f;
            tCooldownSpark.MoveTo(vecUIC);
            tCooldownSpark.SetNoSnap(TRUE);
        }
    }
    else
    {

        if (tCooldownSpark.IsActive())
        { // Inferred const removal: original const HUD calls non-const Trigger.
            const_cast<SkillCooldownFlasher &>(tHUD.tPotionFlasher).Trigger();
        }

        tPotionCooldownCover.SetActive(FALSE);
        tCooldownSpark.SetActive(FALSE);
    }
    // Inferred const removal: original const HUD calls non-const Update.
    const_cast<SkillCooldownFlasher &>(tHUD.tPotionFlasher).Update();
}


GlobalSNO indicators[4] = {
    GLOBALSNO_EFFECTGROUP_PLAYER_1,
    GLOBALSNO_EFFECTGROUP_PLAYER_2,
    GLOBALSNO_EFFECTGROUP_PLAYER_3,
    GLOBALSNO_EFFECTGROUP_PLAYER_4,
};

GlobalSNO controllerLabels[4] = {
    GLOBALSNO_EFFECTGROUP_PLAYER_1_NUM,
    GLOBALSNO_EFFECTGROUP_PLAYER_2_NUM,
    GLOBALSNO_EFFECTGROUP_PLAYER_3_NUM,
    GLOBALSNO_EFFECTGROUP_PLAYER_4_NUM,
};

void UpdatePlayerIndicator(const Player * ptPlayer, BOOL bReapply) {
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer) {
        int32 nIndex = ptPlayer->nPlayerIndex;
        if (bReapply) {
            ptACDPlayer->RemoveClientEffect(GlobalSNOGet(indicators[nIndex]));
            ptACDPlayer->RemoveClientEffect(GlobalSNOGet(controllerLabels[nIndex]));
        }
        if (PlayersGetCount() <= 1) {
            return;
        }
        ptACDPlayer->AddClientEffect(GlobalSNOGet(indicators[nIndex]), 0);
        ptACDPlayer->AddClientEffect(GlobalSNOGet(controllerLabels[nIndex]), 0);
    }
}

















GlobalSNO targetIndicators[4] = {
    GLOBALSNO_EFFECTGROUP_PLAYERTARGET_1,
    GLOBALSNO_EFFECTGROUP_PLAYERTARGET_2,
    GLOBALSNO_EFFECTGROUP_PLAYERTARGET_3,
    GLOBALSNO_EFFECTGROUP_PLAYERTARGET_4,
};

void UpdatePlayerTargetingIndicator(const Player * ptPlayer, const RActor * ptRActor, BOOL bApply) {
    if (bApply && (PlayerGetLocalPlayerCount() < 2 || RActorIsHidden(ptRActor))) {
        return;
    }
    if (ptPlayer->nControllerIndex == -1) {
        return;
    }
    int32 nIndex = ptPlayer->nPlayerIndex;
    ActorCommonData * ptACDTarget = ACDTryToGet(ptRActor->idACD);
    if (ptACDTarget && (ptACDTarget->IsMonster() || ACDIsPlayer(ptACDTarget->id))) {
        SNO snoEffect = GlobalSNOGet(targetIndicators[nIndex]);
        if (bApply) {
            ptACDTarget->AddClientEffect(snoEffect, 0);
        } else {
            ptACDTarget->RemoveClientEffect(snoEffect);
        }
    }
}



















// LINKER-REMOVED: DWARF 1678 (low_pc 0, 0x74 bytes), one inlined copy in the CU; body from DWARF locals and line columns, needed as an out-of-line caller; exact expressions inferred.
void UpdateLocalNamePlate(
    const Player * ptPlayer,
    const UIID & idNameRegion)
{
    if (UI_TestID(idNameRegion))
    {
        UIText & tText = UI_GetTextFromID(idNameRegion);
        CRefString sIndicator = CPlayerGetIndicatorString(*ptPlayer);
        tText.SetText(sIndicator, FALSE);
    }
}

CRefString GetPlayerLevelString(const Player * ptPlayer) {
    if (ptPlayer->nCurrentLevel == 70 && ptPlayer->nCurrentAltLevel > 0) {
        return CRefString("{c:ff5789ff}") + UnicodeText::FormatNumber(ptPlayer->nCurrentAltLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID) + CRefString("{/c}");
    }
    return CRefString("{c_white}") + UnicodeText::FormatNumber(ptPlayer->nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID) + CRefString("{/c}");
}









void UpdateRemoteNamePlate(const Player * ptPlayer, const UIID & idNameRegion, const UIID & idLabelRegion, const UIID & idLevelRegion) {
    if (UI_TestID(idNameRegion)) {
        UIText & tNameText = UI_GetTextFromID(idNameRegion);
        tNameText.SetActive(TRUE);
        CRefString sName(ptPlayer->tAccount.usName);
        tNameText.SetText(sName, FALSE);
        if (UI_TestID(idLevelRegion)) {
            CRefString sLevel = GetPlayerLevelString(ptPlayer);
            if (ptPlayer->tClientInfo.bUpscaled) {
                sLevel = CRefString("{icon:Console_ApprenticeWingLeft,2.0}") + sLevel + CRefString("{icon:Console_ApprenticeWingRight,2.0}");
            }
            UIText & tLevelText = UI_GetTextFromID(idLevelRegion);
            tLevelText.SetActive(TRUE);
            tLevelText.SetText(sLevel, FALSE);
            if (UI_TestID(idLabelRegion)) {
                UIText & tIndicatorText = UI_GetTextFromID(idLabelRegion);
                CRefString sIndicator = CPlayerGetIndicatorString(*ptPlayer);
                if (::Console::Online::PartyGetHostIndex() == ptPlayer->nPlayerIndex) {
                    sIndicator = CRefString("{icon:Console_PartyLeaderIcon} ") + sIndicator;
                }
                uint32 nLastSpoke = sgtHUDGlobals.tMultiPlayers[ptPlayer->nPlayerIndex].tOnline.nTimeLastSpoke;
                if (nLastSpoke != 0 && AppGetTicks() - nLastSpoke <= 60) {
                    if (PlayerGetHUDSlot(ptPlayer) % 2 == 0) {
                        sIndicator = sIndicator + CRefString(" {icon:Console_VoiceChat_Speaking,2}");
                    } else {
                        sIndicator = CRefString("{icon:Console_VoiceChat_Speaking,2} ") + sIndicator;
                    }
                }
                tIndicatorText.SetText(sIndicator, FALSE);
                tIndicatorText.SetActive(TRUE);
            }
        }
    }
}








































GlobalSNO playerIndicatorLabels[4] = {
    GLOBALSNO_EFFECT_GROUP_INDICATOR_PLAYER1,
    GLOBALSNO_EFFECT_GROUP_INDICATOR_PLAYER2,
    GLOBALSNO_EFFECT_GROUP_INDICATOR_PLAYER3,
    GLOBALSNO_EFFECT_GROUP_INDICATOR_PLAYER4,
};


void UpdatePlayerIdentifier(const Player * ptPlayer) {
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer) {
        RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptRActor) {
            if (::Console::Screens::IsScreenSetOpen() || CGameInScriptedSequence() || ::Console::Screens::GetModalNotificationManager().IsNotificationShowing() || UIConfirmation::IsVisible()) {
                ptRActor->bShowLocatorBeam = FALSE;
            }
            if (ptRActor->bShowLocatorBeam) {
                ptACDPlayer->AddClientEffect(GlobalSNOGet(playerIndicatorLabels[ptPlayer->nPlayerIndex]), 0);
            } else {
                ptACDPlayer->RemoveClientEffect(GlobalSNOGet(playerIndicatorLabels[ptPlayer->nPlayerIndex]));
            }
        }
    }
}
















void UpdateBuffRenderPositions(
    const Player * ptPlayer)
{
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);

    int32 nTotalBuffs = 0;

    int32 nBuffOffsetLeft = 0;

    for (int32 n = 0; n < BUFF_SLOT_COUNT; n++) {
        int32 newOffset = n - nBuffOffsetLeft;

        if (tInfo.tLocal.arCurrentBuffs[n].eType == BUFF_TYPE_NONE) {

            nBuffOffsetLeft++;
        } else if (tInfo.tLocal.arCurrentBuffs[n].eType != BUFF_TYPE_DEBUFF) {


            SetBuffDisplayOffset(ptPlayer, n, newOffset);
            nTotalBuffs++;
        }
    }

    int32 nDebuffOffset = nTotalBuffs >= BUFF_SLOTS_PER_ROW ? BUFF_SLOT_COUNT - 1 : BUFF_SLOTS_PER_ROW - 1;

    for (int32 n = BUFF_SLOT_COUNT - 1; n >= 0; n--) {
        if (tInfo.tLocal.arCurrentBuffs[n].eType == BUFF_TYPE_DEBUFF) {

            SetBuffDisplayOffset(ptPlayer, n, nDebuffOffset);
            nDebuffOffset--;
        }
    }
}

int32 GetBuffIndex(const Player * ptPlayer, int32 nRefValue, int32 nPowerBuffIndex) {
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    for (int32 n = 0; n < BUFF_SLOT_COUNT; n++) {
        const DisplayInfo::DisplayBuff & tDisplayBuff = tInfo.tLocal.arCurrentBuffs[n];
        if (tDisplayBuff.eType == BUFF_TYPE_PET) {
            if (tDisplayBuff.ePetType == nRefValue) {
                return n;
            }
        } else if (tDisplayBuff.snoPower == nRefValue && tDisplayBuff.nPowerBuffIndex == nPowerBuffIndex) {
            return n;
        }
    }
    return -1;
}








void ClearBuffSlot(const Player * ptPlayer, int32 nBuffIndex) {
    int32 nSlot = PlayerGetHUDSlot(ptPlayer);
    DisplayInfo::DisplayBuff & tBuff = sgtHUDGlobals.tMultiPlayers[nSlot].tLocal.arCurrentBuffs[nBuffIndex];
    if (tBuff.eType != BUFF_TYPE_NONE) {
        if (UI_TestID(tBuff.idBuffIndicator)) {
            UIConsoleBuffIndicator & tHIndicator = UI_GetConsoleBuffIndicatorFromID(tBuff.idBuffIndicator);
            UIID idTimer = tHIndicator.m_idTimer;
            if (idTimer != GetUIIDNone()) {
                UI_DeleteControl(idTimer, TRUE);
            }
            UIID idText = tHIndicator.m_idText;
            if (idText != GetUIIDNone()) {
                UI_DeleteControl(idText, TRUE);
            }
            UI_DeleteControl(tBuff.idBuffIndicator, TRUE);
            UI_DeleteControl(tBuff.idControl, TRUE);
        }
        ClearDisplayBuffForPlayer(ptPlayer, nBuffIndex);
    }
}













void OnRemoveBuff(SNO snoPower, int32 nBuffIndex, const Player * ptPlayer) {
    int32 index = GetBuffIndex(ptPlayer, snoPower, nBuffIndex);
    if (index != -1) {
        ClearBuffSlot(ptPlayer, index);
    }
}






int32 FindClearBuffSlot(const Player * ptPlayer, eBuffType eTypeNeeded) {
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    for (int32 n = 0; n < BUFF_SLOT_COUNT; n++) {
        if (tInfo.tLocal.arCurrentBuffs[n].eType == BUFF_TYPE_NONE || tInfo.tLocal.arCurrentBuffs[n].eType < eTypeNeeded) {
            ClearBuffSlot(ptPlayer, n);
            return n;
        }
    }
    return -1;
}









void DisplayBuff(const Player * ptPlayer, const UIBuffs::SortedBuff & tBuff, eBuffType eType, int32 nStackCount) {
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    int32 nDisplayIndex = GetBuffIndex(ptPlayer, tBuff.snoPower, tBuff.nBuffIdx);
    if (nDisplayIndex != -1) {
        if (tInfo.tLocal.arCurrentBuffs[nDisplayIndex].nStackCount != nStackCount) {
            UpdateStackCount(ptPlayer, nDisplayIndex, nStackCount);
        }
        return;
    }
    int32 n;
    if (eType == BUFF_TYPE_DEBUFF) {
        for (n = BUFF_SLOT_COUNT - 1; n >= 0; n--) {
            if (tInfo.tLocal.arCurrentBuffs[n].eType < eType) {
                break;
            }
        }
        if (n < 0) {
            n = FindClearBuffSlot(ptPlayer, eType);
            if (n < 0) {
                return;
            }
        }
    } else {
        for (n = 0; n < BUFF_SLOT_COUNT; n++) {
            if (tInfo.tLocal.arCurrentBuffs[n].eType < eType) {
                break;
            }
        }
        if (n == BUFF_SLOT_COUNT) {
            n = FindClearBuffSlot(ptPlayer, eType);
            if (n < 0) {
                return;
            }
        }
    }
    if (tInfo.tLocal.arCurrentBuffs[n].eType != BUFF_TYPE_NONE) {
        ClearBuffSlot(ptPlayer, n);
    }
    BuffInfo tBuffInfo;
    tBuffInfo.snoPower = tBuff.snoPower;
    tBuffInfo.ePetType = -1;
    tBuffInfo.hImage = tBuff.GetImage();
    tBuffInfo.nBuffIdx = tBuff.nBuffIdx;
    SetDisplayBuffForPlayer(ptPlayer, n, eType, tBuffInfo);
}
































BOOL BuffIsOwnType(const UIBuffs::SortedBuff & tBuff, eBuffType eType) {
    ActorCommonData * ptACDPlayer = ACDTryToGet(tBuff.ptPlayer->idPlayerACD);
    if (ptACDPlayer) {
        BOOL bIsOwnBuff = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, tBuff.snoPower));
        if (bIsOwnBuff) {
            return eType == BUFF_TYPE_OWN_BUFF;
        }
        return eType != BUFF_TYPE_OWN_BUFF;
    }
    return FALSE;
}











void UpdateBuffsAffectingPlayer(const Player * ptPlayer, UIBuffs::XPooledSortedBuffList & BuffList, eBuffType eType) {
    POS pos = BuffList.GetHeadPosition();
    while (pos) {
        UIBuffs::SortedBuff & tBuff = BuffList.GetNext(pos);
        if (!tBuff.ptPlayer || ptPlayer->nPlayerIndex != tBuff.ptPlayer->nPlayerIndex) {
            continue;
        }
        ActorCommonData * ptACDAffected = ACDTryToGet(tBuff.ptPlayer->idPlayerACD);
        if (!ptACDAffected) {
            continue;
        }
        Attrib eAttribStart, eAttribEnd, eAttribCount;
        PowerGetBuffAttributes(tBuff.nBuffIdx, eAttribStart, eAttribEnd, eAttribCount);
        uint32 dwStartTick = ptACDAffected->AttributesGetInt(MakeAttribKey(eAttribStart, tBuff.snoPower));
        uint32 dwEndTick = ptACDAffected->AttributesGetInt(MakeAttribKey(eAttribEnd, tBuff.snoPower));
        uint32 dwNow = CGameGetPredictedServerTime();
        int32 nStackCount = ptACDAffected->AttributesGetInt(MakeAttribKey(eAttribCount, tBuff.snoPower));
        BOOL bInfinite = !ptACDAffected->AttributesAttributeExists(MakeAttribKey(eAttribEnd, tBuff.snoPower));
        if (nStackCount > 0 && (dwEndTick >= dwNow || bInfinite) && (eType == BUFF_TYPE_DEBUFF || BuffIsOwnType(tBuff, eType)) && PlayerIsLocal(ptPlayer)) {
        DisplayBuff(ptPlayer, tBuff, eType, nStackCount);
        int32 nDisplayIndex = GetBuffIndex(ptPlayer, tBuff.snoPower, tBuff.nBuffIdx);
        if (nDisplayIndex != -1) {
            int32 nTimeLeft = dwEndTick - dwNow;
            if (tBuff.nDuration < 0) {
                tBuff.nDuration = dwEndTick - dwStartTick;
            } else if (nTimeLeft > tBuff.nDuration) {
                tBuff.nDuration = nTimeLeft;
            }
            int32 nDelta = dwEndTick - tBuff.nDuration;
            DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
            if (UI_TestID(tInfo.tLocal.arCurrentBuffs[nDisplayIndex].idBuffIndicator)) {
                UIConsoleBuffIndicator & tHIndicator = UI_GetConsoleBuffIndicatorFromID(tInfo.tLocal.arCurrentBuffs[nDisplayIndex].idBuffIndicator);
                UIID timerID = tHIndicator.m_idTimer;
                if (timerID != GetUIIDNone()) {
                    UITimer & tTimer = UI_GetTimerFromID(tHIndicator.m_idTimer);
                    tTimer.SetTimer(nDelta, dwEndTick);
                }
            }
        }
        }
    }
}












































void DisplayPetBuff(const Player * ptPlayer, const ActorCommonData * ptACDPet, PetType ePetType, int32 nCount) {
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    int32 n;
    for (n = 0; n < BUFF_SLOT_COUNT; n++) {
        if (tInfo.tLocal.arCurrentBuffs[n].eType < BUFF_TYPE_PET) {
            break;
        }
    }
    if (n == BUFF_SLOT_COUNT) {
        n = FindClearBuffSlot(ptPlayer, BUFF_TYPE_PET);
        if (n < 0) {
            return;
        }
    }
    if (tInfo.tLocal.arCurrentBuffs[n].eType != BUFF_TYPE_NONE) {
        ClearBuffSlot(ptPlayer, n);
    }
    BuffInfo tBuffInfo;
    tBuffInfo.snoPower = SNO_NONE;
    tBuffInfo.ePetType = ePetType;
    tBuffInfo.hImage = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_PORTRAIT_IMAGE, ptACDPet->snoActor);
    tBuffInfo.nBuffIdx = -1;
    SetDisplayBuffForPlayer(ptPlayer, n, BUFF_TYPE_PET, tBuffInfo);
    tInfo.tLocal.arCurrentBuffs[n].bShowSingleStack = TRUE;
}



















inline BOOL PetTypeNeedsBuffDisplayed(PetType eType) {
    switch (eType) {
    case PETTYPE_HIRELING:
    case PETTYPE_FOLLOWER:
    case PETTYPE_FOLLOWER_PROXY:
        return FALSE;
    default:
        return PlayerPetShowPortrait(eType);
    }
}




void UpdatePetBuffsForPlayer(const Player * ptPlayer) {
    for (int32 nPetType = 0; nPetType < NUM_PET_TYPES; nPetType++) {
        if (!PetTypeNeedsBuffDisplayed(nPetType)) {
            continue;
        }
        int32 nDisplayIndex = GetBuffIndex(ptPlayer, nPetType, -1);
        int32 nCount = PlayerGetNumPetsOfType(ptPlayer->idPlayerACD, nPetType);
        if (nCount > 0) {
            if (nDisplayIndex == -1) {
                int32 nPos;
                ACDID idACDPet = PlayerGetFirstPet(ptPlayer->idPlayerACD, nPetType, nPos);
                ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
                if (ptACDPet && ActorDefinitionGetTagMapValueInt(TAG_ACTOR_PORTRAIT_IMAGE, ptACDPet->snoActor)) {
                    DisplayPetBuff(ptPlayer, ptACDPet, nPetType, nCount);
                }
            } else {
                UpdateStackCount(ptPlayer, nDisplayIndex, nCount);
            }
        } else if (nDisplayIndex != -1) {
            ClearBuffSlot(ptPlayer, nDisplayIndex);
        }
    }
}

















// LINKER-REMOVED: DWARF 2223 (low_pc 0, 0x80 bytes), one inlined copy in the CU; body from DWARF locals and line columns, needed as an out-of-line caller; exact expressions inferred.
void UpdateBuffs(
    const Player * ptPlayer)
{
    UIBuffs::UIBuffGlobals * pBuffGlobals = UIBuffs::GetBuffGlobals();

    if (pBuffGlobals)
    {



        UpdatePetBuffsForPlayer(ptPlayer);
        UpdateBuffsAffectingPlayer(ptPlayer, pBuffGlobals->listBeneficialBuffs, BUFF_TYPE_OWN_BUFF);
        UpdateBuffsAffectingPlayer(ptPlayer, pBuffGlobals->listHarmfulBuffs, BUFF_TYPE_DEBUFF);
        UpdateBuffsAffectingPlayer(ptPlayer, pBuffGlobals->listBeneficialBuffs, BUFF_TYPE_OTHER_BUFF);

        UpdateBuffRenderPositions(ptPlayer);
    }
}
void ClearAllBuffs() {
    for (uint32 nController = 0; nController < 4; nController++) {
        DisplayInfo & tCurrentInfo = sgtHUDGlobals.tMultiPlayers[nController];
        for (int32 nBuffIndex = 0; nBuffIndex < BUFF_SLOT_COUNT; nBuffIndex++) {
            DisplayInfo::DisplayBuff & tCurrentBuff = tCurrentInfo.tLocal.arCurrentBuffs[nBuffIndex];
            if (tCurrentBuff.eType != BUFF_TYPE_NONE) {
                if (UI_TestID(tCurrentBuff.idBuffIndicator)) {
                    UIConsoleBuffIndicator & tHIndicator = UI_GetConsoleBuffIndicatorFromID(tCurrentBuff.idBuffIndicator);
                    UIID idTimer = tHIndicator.m_idTimer;
                    if (idTimer != GetUIIDNone()) {
                        UI_DeleteControl(idTimer, TRUE);
                    }
                    UIID idText = tHIndicator.m_idText;
                    if (idText != GetUIIDNone()) {
                        UI_DeleteControl(idText, TRUE);
                    }
                    UI_DeleteControl(tCurrentBuff.idBuffIndicator, TRUE);
                    UI_DeleteControl(tCurrentBuff.idControl, TRUE);
                }
                ClearDisplayBuffForSlot(nController, nBuffIndex);
            }
        }
    }
}

















void InitializeForPlayer(const Player * ptPlayer) {
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer) {
        return;
    }
    SNO snoResourceActor = SNO_NONE;
    SNO snoSecondaryResourceActor = SNO_NONE;
    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptACDPlayer->id);
    switch (ePlayerClass) {
    case PLAYER_CLASS_DEMONHUNTER:
        snoResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_HATE_BAR);
        snoSecondaryResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_DISCIPLINE_BAR);
        break;
    case PLAYER_CLASS_BARBARIAN:
        snoResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_FURY_BAR);
        break;
    case PLAYER_CLASS_WIZARD:
        snoResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_ARCANUM_BAR);
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        snoResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_MANA_BAR);
        break;
    case PLAYER_CLASS_MONK:
        snoResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_SPIRIT_BAR);
        break;
    case PLAYER_CLASS_CRUSADER:
        snoResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_FAITH_BAR);
        break;
    case PLAYER_CLASS_NECROMANCER:
        snoResourceActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_ESSENCE_BAR);
        break;
    }
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    UIID idPrimaryResource = snoSecondaryResourceActor == SNO_NONE ? tInfo.tLocal.idResourceBar1 : tInfo.tLocal.idResourceBar2;
    UIID idUnusedResource = snoSecondaryResourceActor == SNO_NONE ? tInfo.tLocal.idResourceBar2 : tInfo.tLocal.idResourceBar1;
    UIID idSecondaryResource = tInfo.tLocal.idResourceBar3;
    UI_GetProgressBarFromID(idPrimaryResource).SetForegroundRActor(snoResourceActor, RENDERCONTEXT_HUD_0);
    UI_GetProgressBarFromID(idUnusedResource).SetForegroundRActor(SNO_NONE, RENDERCONTEXT_HUD_0);
    UI_GetProgressBarFromID(idSecondaryResource).SetForegroundRActor(snoSecondaryResourceActor, RENDERCONTEXT_HUD_0);
    BOOL bHasSecondaryResource = ptACDPlayer->GetSecondaryResourceType() != -1;
    ShowControl(tInfo.tLocal.idResourceBar1, !bHasSecondaryResource);
    ShowControl(tInfo.tLocal.idResourceBar2, bHasSecondaryResource);
    ShowControl(tInfo.tLocal.idResourceBar3, bHasSecondaryResource);
    if (UI_TestID(tInfo.tLocal.idResourceBarDivider)) {
        UI_GetWindowFromID(tInfo.tLocal.idResourceBarDivider).SetActive(bHasSecondaryResource);
    }
    UIMultiplayerDeathMenu::Console::RefreshForAllPlayers();
    UIFastEquip::Console::CloseForPlayer(ptPlayer->nPlayerIndex);
}






















void SetPlayerLevelString(const UIID & idLevelText, const Player * ptPlayer) {
    UIText & tText = UI_GetTextFromID(idLevelText);
    if (ptPlayer->nCurrentLevel == 70 && ptPlayer->nCurrentAltLevel > 0) {
        tText.SetTextColor(RGBAColor(0x57, 0x89, 0xff, 0xff));
        tText.SetText(UnicodeText::FormatNumber(ptPlayer->nCurrentAltLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID), FALSE);
    } else {
        tText.SetTextColor(RGBAColor(0xff, 0xff, 0xff, 0xff));
        tText.SetText(UnicodeText::FormatNumber(ptPlayer->nCurrentLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID), FALSE);
    }
}










void UpdateForPlayer(
    const Player * ptPlayer,
    const BOOL fLocal)
{
            if (ptPlayer == NULL)
            {
                return;
            }


            BOOL bHudIsActive = UI_GetWindowFromID(UIWND_CONSOLE_HUD).IsActive();
            if (bHudIsActive)
            {
                DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);

                BOOL bNewLocalHUD = !UI_GetControlFromID(tInfo.tLocal.idMain).IsItselfActive() && fLocal;


                UI_GetControlFromID(tInfo.tLocal.idMain).SetActive(fLocal);
                UI_GetControlFromID(tInfo.tOnline.idMain).SetActive(!fLocal);

                if (fLocal)
                {
                    if (ptPlayer->idPlayerACD == ACDID_INVALID)
                    {
                        return;
                    }

                    const ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                    DisplayInfo::LocalHUD & tHUD = tInfo.tLocal;
                    if (UI_TestID(tHUD.idPotionIcon))
                    {
                        UIHotbarButton & tPotionButton = UI_GetHotbarButtonFromID(tHUD.idPotionIcon);
                        tPotionButton.SetPlayer(ptPlayer);
                        tPotionButton.HideStackCount(TRUE);
                    }

                    if (UI_TestID(tHUD.idLevel))
                    {
                        SetPlayerLevelString(tHUD.idLevel, ptPlayer);
                    }

                   if (UI_TestID(tHUD.idApprenticeIndicator))
                    {
                        ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
                        int32 nUpscaleLevel = tACD.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL);
                        UI_GetWindowFromID(tHUD.idApprenticeIndicator).SetActive(nUpscaleLevel > 0);
                    }

                   if (UI_TestID(tHUD.idMailIndicator))
                    {
                        if (GameIsSeasonal())
                        {
                            UI_GetWindowFromID(tHUD.idMailIndicator).SetActive(FALSE);
                        }
                        else
                        {
                            BOOL bHasMail = ::Console::Online::MailHasMail(ptPlayer->nControllerIndex, ::Console::Online::MAIL_TYPE_MESSAGE);
                            BOOL bAlreadyActive = UI_GetWindowFromID(tHUD.idMailIndicator).IsActive();
                            UI_GetWindowFromID(tHUD.idMailIndicator).SetActive(bHasMail);
                            if (bHasMail && !bAlreadyActive)
                            {
                                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAIL_NEW_MAIL_ARRIVED), cgtPSODefaults);
                            }
                        }
                    }

                    if (UI_TestID(tHUD.idCacheIndicator) && !GameIsWeeklyChallenge() && GameIsOpenWorld())
                    {
                        D3::Account::SavedDefinition & tAccountDefinition = ::Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);

                        uint64 uNow = Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp());

                        BOOL bRewardAvailable = FALSE;

                        for (int32 n = 0; n < tAccountDefinition.console_data().challenge_rift_reward_size() && !bRewardAvailable; n++)
                        {  // Inference: `auto` from the copy-constructor column 2450:44; DWARF type ConsoleChallengeRiftReward by value.
                            auto tReward = tAccountDefinition.console_data().challenge_rift_reward(n);
                            // Inference: one || condition; the original keeps both TRUE paths in one block (line 0:37) and branches instead of cset.
                            if (!GameIsSeasonal() ||

                                tReward.season_earned() == OnlineService::GetSeasonNum())
                            {
                                bRewardAvailable = TRUE;
                            }



                        }

                        UI_GetWindowFromID(tHUD.idCacheIndicator).SetActive(bRewardAvailable);
                    }

                   if (UI_TestID(tHUD.idPartyLeaderIndicator))
                    {
                        UI_GetWindowFromID(tHUD.idPartyLeaderIndicator).SetActive((::Console::Online::PartyGetHostIndex() == ptPlayer->nPlayerIndex) && PlayerGetNumPlayers() > 1);
                    }

                   if (UI_TestID(tHUD.idVoiceChatIndicator))
                    {
                        uint32 nLastSpoke = sgtHUDGlobals.tMultiPlayers[ptPlayer->nPlayerIndex].tOnline.nTimeLastSpoke;

                       if (nLastSpoke != 0 && AppGetTicks() - nLastSpoke <= 60)
                        {
                            UI_GetWindowFromID(tHUD.idVoiceChatIndicator).SetActive(TRUE);
                        }
                        else
                        {
                            UI_GetWindowFromID(tHUD.idVoiceChatIndicator).SetActive(FALSE);
                        }
                    }



                    UpdateResourceBars(tHUD, tACDPlayer);
                    UpdateHealthGlobe(tHUD, tACDPlayer);
                    UpdateXPBar(tHUD.idMain, tHUD.idXPBar, tHUD.idAltXPBar, tHUD.idPoRXPMarker, tACDPlayer, ptPlayer);
                    UpdatePotion(tHUD, ptPlayer);
                    UpdateSkills(tHUD, ptPlayer);
                    UpdateLocalNamePlate(ptPlayer, tHUD.idNameRegion);
                    UpdateBuffs(ptPlayer);
                    UpdatePlayerDeathIndicator(tHUD.idDeath, tACDPlayer);
                    UIDurabilityIndicator::UpdateDurabilityIndicator(tACDPlayer, tHUD.idDurability);
                    UpdateTutorials(ptPlayer, tACDPlayer);

                    if (bNewLocalHUD)
                    {
                        UI_GetRActorFromID(UIIDCreateFromParent(tHUD.idPoRXPMarker, ".GrantedFX")).SetActive(FALSE);
                    }
                }
                else
                {
                    DisplayInfo::OnlineHUD & tHUD = tInfo.tOnline;
                    UpdateRemoteNamePlate(ptPlayer, tHUD.idNameRegion, tHUD.idLabelRegion, tHUD.idLevelRegion);

                   if (UI_TestID(tHUD.idHirelingPortrait))
                    {
                        UI_GetWindowFromID(tHUD.idHirelingPortrait).SetActive(FALSE);
                    }

                    if (UI_TestID(tHUD.idHirelingName))
                    {
                        UI_GetWindowFromID(tHUD.idHirelingName).SetActive(FALSE);
                    }

                    if (UI_TestID(tHUD.idHirelingHealthBar))
                    {
                        UI_GetWindowFromID(tHUD.idHirelingHealthBar).SetActive(FALSE);
                    }

                    if (UI_TestID(tHUD.idPortrait))
                    {
                        UI_GetWindowFromID(tHUD.idPortrait).SetActive(TRUE);
                        GBID gbidPortraitFrame = GBID_NONE;








                       const ClientTrickleEntry * ptTrickleEntry = CTrickleEntryFindForANN(ptPlayer->tClientInfo.ann);
                        if (ptTrickleEntry)
                        {
                            gbidPortraitFrame = ptTrickleEntry->gbidPortraitFrame;
                        }

                        if (gbidPortraitFrame == GBID_NONE)
                        {
                            gbidPortraitFrame = PlayerGetPortraitGBIDFromParagonLevel(ptPlayer->nCurrentAltLevel);
                        }

                        UIUtils::Console::DisplayPortrait(ptPlayer->ePlayerClassPortrait, (ptPlayer->tHeroStateData.uPlayerFlags >> PLAYERFLAG_FEMALE) & 1, ptPlayer->nCurrentAltLevel, tHUD.idPortrait, gbidPortraitFrame);
                    }

                    if (UI_TestID(tHUD.idLevel))
                    {
                        UI_GetWindowFromID(tHUD.idLevel).SetActive(TRUE);
                        SetPlayerLevelString(tHUD.idLevel, ptPlayer);
                    }

                    if (UI_TestID(tHUD.idHealthBar))
                    {
                        UI_GetWindowFromID(tHUD.idHealthBar).SetActive(TRUE);
                        float flAdjustedFrac = ptPlayer->flHealthPercent;
                        UIProgressBar & tProgressBarHealth = UI_GetProgressBarFromID(tHUD.idHealthBar);
                        tProgressBarHealth.SetExtentsAndPosition(0.0f, 1.0f, flAdjustedFrac, flAdjustedFrac);
                    }
                }
            }


            UpdatePlayerIdentifier(ptPlayer);
}


void UpdateForHireling(
    Player * ptPlayer,
    const ActorCommonData & tACDHireling)
{
    DisplayInfo & tInfo = sgtHUDGlobals.tMultiPlayers[HIRELING_HUD_SLOT];

    UI_GetControlFromID(tInfo.tLocal.idMain).SetActive(FALSE);
    UI_GetControlFromID(tInfo.tOnline.idMain).SetActive(TRUE);

    {
        DisplayInfo::OnlineHUD & tHUD = tInfo.tOnline;

        if (UI_TestID(tHUD.idPortrait))
        {
            UI_GetWindowFromID(tHUD.idPortrait).SetActive(FALSE);
        }

        if (UI_TestID(tHUD.idHealthBar))
        {
            UI_GetWindowFromID(tHUD.idHealthBar).SetActive(FALSE);
        }

        if (UI_TestID(tHUD.idNameRegion))
        {
            UI_GetWindowFromID(tHUD.idNameRegion).SetActive(FALSE);
        }

        if (UI_TestID(tHUD.idHirelingName))
        {
            UI_GetWindowFromID(tHUD.idHirelingName).SetActive(TRUE);
            CRefString sName = MonsterGetName(&tACDHireling, TRUE);
            UI_GetTextFromID(tHUD.idHirelingName).SetText(sName, FALSE);
        }

        if (UI_TestID(tHUD.idHirelingPortrait))
        {
            ImageTextureFrame tFrame;
            ActorGetPortraitTexFrame(tFrame, tACDHireling.snoActor, FALSE);
            UIWindow & tPortrait = UI_GetWindowFromID(tHUD.idHirelingPortrait);
            tPortrait.SetTextureFrame(tFrame);
            tPortrait.SetActive(TRUE);
        }

        if (UI_TestID(tHUD.idLevel))
        {
            int32 nLevel = tACDHireling.AttributesGetInt(ATTRIBUTE_LEVEL);
            UI_GetTextFromID(tHUD.idLevel).SetText(UnicodeText::FormatNumber(nLevel, 1, FNS_NORMAL, 0), FALSE);
        }

        if (UI_TestID(tHUD.idLevelRegion))
        {
            UI_GetWindowFromID(tHUD.idLevelRegion).SetActive(FALSE);
        }

        if (UI_TestID(tHUD.idLabelRegion))
        {
            UI_GetWindowFromID(tHUD.idLabelRegion).SetActive(FALSE);
        }

        if (UI_TestID(tHUD.idHirelingHealthBar))
        {
            UI_GetWindowFromID(tHUD.idHirelingHealthBar).SetActive(TRUE);
            float flAdjustedFrac;
#line 2632
            float flAdjustTargetFrac;
            sGetHealthFracForACD(tACDHireling, flAdjustedFrac, flAdjustTargetFrac);
#line 2635
            UIProgressBar & tProgressBarHealth = UI_GetProgressBarFromID(tHUD.idHirelingHealthBar);
            tProgressBarHealth.SetExtentsAndPosition(0.0f, 1.0f, flAdjustedFrac, flAdjustedFrac);
#line 2639
            if (gtTutorialMgr.ShowTutorial(tHUD.idHirelingHealthBar, GlobalSNOGet(GLOBALSNO_TUTORIAL_FOLLOWER_REINFORCED), NULL, TRUE))
            {
#line 2643
                if (HirelingHasUnspentSkillPoints(tACDHireling))
                {
                    Tutorial::Shown(*ptPlayer, GlobalSNOGet(GLOBALSNO_TUTORIAL_HIRELING_TRAINING));
                }
            }


            SNO snoTrainingTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_HIRELING_TRAINING);
            if (!gtTutorialMgr.IsAnyShowing(0) && !Tutorial::PlayerHasSeen(*ptPlayer, snoTrainingTutorial))
            {
                if (HirelingHasUnspentSkillPoints(tACDHireling))
                {
                    gtTutorialMgr.ShowTutorial(tHUD.idHirelingHealthBar, snoTrainingTutorial, NULL, TRUE);
                }
            }
        }
    }
}


void RefreshHUDs() {
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer)) {
        if (PlayerIsLocal(ptPlayer)) {
            InitializeForPlayer(ptPlayer);
        } else {
            UpdatePlayerIndicator(ptPlayer, TRUE);
        }
        sgtHUDGlobals.nLastPotionCount[ptPlayer->nPlayerIndex] = -1;
        UIEventQueue::Console::InvalidateForPlayer(ptPlayer->nPlayerIndex);
    }
    ClearAllBuffs();
    sgtHUDGlobals.fPlayerJoinedOrLeft = FALSE;
}












void Update() {
    uint32 dwFrameCount = sgtAppGlobals.dwFrameCounter;
    int32 nNumPlayers = PlayerGetNumValidInGamePlayers();
    ActorCommonData * ptACDHireling = NULL;
    if (nNumPlayers != sgtHUDGlobals.nPlayers) {
        sgtHUDGlobals.fPlayerJoinedOrLeft = TRUE;
        sgtHUDGlobals.nPlayers = nNumPlayers;
    }
    if (sgtHUDGlobals.fPlayerJoinedOrLeft) {
        RefreshHUDs();
    }
    for (const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer)) {
        if (ptPlayer->idPlayerRActor != RACTORID_INVALID) {
            UpdatePlayerIndicator(ptPlayer, FALSE);
        }
        BOOL fLocal = PlayerIsLocal(ptPlayer);
        if (!fLocal || dwFrameCount % 4 == (uint32)ptPlayer->nPlayerIndex) {
            UpdateForPlayer(ptPlayer, fLocal);
            UIEventQueue::Console::UpdateForPlayer(ptPlayer);
        }
    }
    if (PlayerGetNumPlayers() > 0) {
        UI_GetTextFromID(sgtHUDGlobals.tMultiPlayers[0].tLocal.idNameRegion).SetActive(PlayerGetNumPlayers() > 1);
    }
    if (nNumPlayers == 1) {
        Player * ptPlayer = PlayerGetFirstAll();
        ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
        ptACDHireling = ACDTryToGet(idACDHireling);
        if (ptACDHireling) {
            UpdateForHireling(ptPlayer, *ptACDHireling);
        }
    }
    for (int32 i = 0; i < 4; i++) {
        BOOL fShow = i < nNumPlayers;
        if (i == HIRELING_HUD_SLOT && ptACDHireling) {
            fShow = TRUE;
        }
        ShowControl(sgtHUDGlobals.tMultiPlayers[i].idMain, fShow);
    }
}



























void PlayerChangedWorld(Player * ptPlayer) {
    ::Console::Screens::CloseScreenSet(TRUE, FALSE);
    UpdatePlayerIndicator(ptPlayer, TRUE);
    if (PlayerIsLocal(ptPlayer)) {
        DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
        DisplayInfo::LocalHUD & tHUD = tInfo.tLocal;
        if (tHUD.bLowHealthActive) {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer) {
                ptACDPlayer->RemoveClientEffect(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_LOW_HEALTH));
            }
        }
    }
    InitializeForPlayer(ptPlayer);
    sgtHUDGlobals.fPlayerJoinedOrLeft = TRUE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_LOADING)) {
        CGameSetLoadComplete(ptPlayer);
    }
}













void OnEventRemotePlayerSpoke(int32 nPlayerIndex) {
    if (::Console::Online::IsVoiceChatRestricted()) {
        return;
    }
    sgtHUDGlobals.tMultiPlayers[nPlayerIndex].tOnline.nTimeLastSpoke = AppGetTicks();
}




void PlayerLeft() {
    Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer) {
        if (PlayersGetCount() < 2) {
            UpdatePlayerIndicator(ptPlayer, TRUE);
        }
        if (PlayerGetLocalPlayerCount() < 2) {
            RActorID idRActor = UIGame::GetRActorHighlight(ptPlayer->nControllerIndex);
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor) {
                UpdatePlayerTargetingIndicator(ptPlayer, ptRActor, FALSE);
            }
        }
    }
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer)) {
        if (ptPlayer->idPlayerRActor != RACTORID_INVALID) {
            InitializeForPlayer(ptPlayer);
            BOOL bRemotePlayer = !PlayerIsLocal(ptPlayer);
            (void)bRemotePlayer;
        }
    }
    sgtHUDGlobals.fPlayerJoinedOrLeft = TRUE;
}



















void MarkSkillSlotAsChanged(const Player * ptPlayer, SkillButton eSkillButton) {
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    for (int32 i = 0; i < DisplayInfo::SKILL_BUTTON_COUNT; i++) {
        if (CPlayerUtilSkillButtonFromInputButton(tInfo.tLocal.arSkills[i].eControllerButton) == eSkillButton) {
            tInfo.tLocal.arSkills[i].bDirty = TRUE;
            return;
        }
    }
}








static void sOnFreeCastFXDone(
    const UIID & id)
{
    int32 nPlayerIndex = (int32)(intptr_t)UI_GetStoryboardFromID(id).GetUserData();
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer)
    {


        DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
        for (int32 i = 0; i < DisplayInfo::SKILL_BUTTON_COUNT; i++)
        {
            UI_GetRActorFromID(UIIDCreateFromParent(tInfo.tLocal.arSkills[i].idButton, ".RevengeGlow")).SetActive(FALSE);
        }
    }
}


void ApplyFreeCastEffects(
    const Player * ptPlayer,
    SkillButton eSkillButton)
{
    if (!ptPlayer || !PlayerIsLocal(ptPlayer))
    {
        return;
    }


    int32 nHudSlot = PlayerGetHUDSlot(ptPlayer);
    UIID idHud = UIGame::GetPlayerHUDID(nHudSlot);

    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    for (int32 i = 0; i < DisplayInfo::SKILL_BUTTON_COUNT; i++)
    {
        if (CPlayerUtilSkillButtonFromInputButton(tInfo.tLocal.arSkills[i].eControllerButton) == eSkillButton)
        {
            UIStoryboard & tTiming = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_FREE_CAST_TIMING);
            tTiming.SetUserData((void *)(intptr_t)ptPlayer->nPlayerIndex);
            tTiming.SetOnEndCallback(sOnFreeCastFXDone);
            tTiming.Reset();
            tTiming.Play(FALSE, 0.0f);

            UIID idFreeCaseFX = UIIDCreateFromParent(tInfo.tLocal.arSkills[i].idButton, ".RevengeGlow");
            UI_GetRActorFromID(idFreeCaseFX).SetActive(FALSE);
            UI_GetRActorFromID(idFreeCaseFX).SetActive(TRUE);
        }
    }
}


void ReportPowerOnCooldownError(int32 nPlayerIndex, SNO snoPower) {
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer && PlayerIsLocal(ptPlayer)) {
        DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
        for (int32 i = 0; i < DisplayInfo::SKILL_BUTTON_COUNT; i++) {
            SkillButton eSkillButton = CPlayerUtilSkillButtonFromInputButton(tInfo.tLocal.arSkills[i].eControllerButton);
            if (eSkillButton != -1) {
                const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, eSkillButton);
                if (tButtonData.m_snoPower == snoPower) {
                    const DisplayInfo::SkillButtonInfo & tSkillInfo = tInfo.tLocal.arSkills[i];
                    UIID idHotbarButton = UIIDCreateFromParent(tSkillInfo.idButton, ".Skill");
                    UIHotbarButton & tButton = UI_GetHotbarButtonFromID(idHotbarButton);
                    UIID idTimer = tButton.m_idTimer;
                    if (UI_TestID(idTimer)) {
                        UIConsoleSkillsTimer * ptTimer = &UI_GetConsoleSkillsTimerFromID(idTimer);
                        ptTimer->Flash();
                    }
                    return;
                }
            }
        }
    }
}



















void FlashPotionIcon(int32 nPlayerIndex) {
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    DisplayInfo & tInfo = GetDisplayInfoForPlayer(ptPlayer);
    DisplayInfo::LocalHUD & tHUD = tInfo.tLocal;
    UIHotbarButton & tPotionButton = UI_GetHotbarButtonFromID(tHUD.idPotionIcon);
    UIID idTimer = tPotionButton.m_idTimer;
    if (UI_TestID(idTimer)) {
        UIConsoleSkillsTimer & ptTimer = UI_GetConsoleSkillsTimerFromID(idTimer);
        ptTimer.Flash();
    }
}










BOOL CheckHadBonusXPFlag() {
    return sgbLostBonusXP;
}




void ResetHadBonusXPFlag() {
    sgbLostBonusXP = false;
}




void InitializeForGame() {
    if (!sgtHUDGlobals.fInitialized) {
        for (int32 i = 0; i < 4; i++) {
            UIID idHUDRoot = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d", i + 1));
            sFillDisplayInfo(idHUDRoot, sgtHUDGlobals.tMultiPlayers[i]);
            sgtHUDGlobals.nLastPotionCount[i] = -1;
        }
        ::Console::Online::GetPartyMemberVoiceChatTalkingEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventRemotePlayerSpoke>());
    }
    sgbLostBonusXP = false;
    for (uint64 & xp : sgarLastKnownRestXP) {
        xp = 0;
    }
}











void DisposeForGame() {
    if (!sgtHUDGlobals.fInitialized) {
        sgtHUDGlobals.fInitialized = FALSE;
        ::Console::Online::GetPartyMemberVoiceChatTalkingEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, int, &OnEventRemotePlayerSpoke>());
    }
}








void SkillCooldownFlasher::Trigger()
{

    if (m_bActive || m_idTarget == GetUIIDNone())
    {
        return;
    }

    m_bActive = TRUE;
    m_nCurrentStep = 1;
    m_nTime = AppGetTicks();
}

void SkillCooldownFlasher::Update() {
    if (!m_bActive) {
        return;
    }
    float ftElapsedTime = (AppGetTicks() - m_nTime) / 60.0f;

    if (!(ftElapsedTime < ftStepInfo[m_nCurrentStep][0])) {




        while (m_nCurrentStep < FLASHER_STEPS && ftElapsedTime >= ftStepInfo[m_nCurrentStep][0]) {

            m_nCurrentStep++;
        }
    }

    if (m_nCurrentStep < FLASHER_STEPS) {

        m_ftAlpha = ftStepInfo[m_nCurrentStep - 1][1] + (ftElapsedTime - ftStepInfo[m_nCurrentStep - 1][0]) * (ftStepInfo[m_nCurrentStep][1] - ftStepInfo[m_nCurrentStep - 1][1]) / (ftStepInfo[m_nCurrentStep][0] - ftStepInfo[m_nCurrentStep - 1][0]);
    } else {

        m_bActive = FALSE;
        m_ftAlpha = 0.0f;
    }


    UI_GetWindowFromID(m_idTarget).SetTransparency((uint8)(m_ftAlpha * 255.0f));
}

}  // namespace Console
}  // namespace UIHUD
