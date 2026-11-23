#include "UI.h"

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../CGame.h"
#include "../CWorld.h"
#include "../Cutscene.h"
#include "../Input/Input.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/ThreadGlobals.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../Sigma/DataStructures/XCommandBuffer.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../Definitions/UIDialogDefinition.h"
#include "../../Common/Game.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RefString.h"
#include "Console/ConsoleScreenManager.h"
#include "Controls/UIActorTagControl.h"
#include "Controls/UIAutoScrollingTextControl.h"
#include "Controls/UIBlinkerControl.h"
#include "Controls/UIButtonControl.h"
#include "Controls/UICheckBoxControl.h"
#include "Controls/UIConsoleBuffIndicatorControl.h"
#include "Controls/UIConsoleListboxControl.h"
#include "Controls/UIConsoleScrollBoxControl.h"
#include "Controls/UIConsoleScrollIndicatorControl.h"
#include "Controls/UIConsoleSelectorControl.h"
#include "Controls/UIConsoleSkillsTimerControl.h"
#include "Controls/UIControlControl.h"
#include "Controls/UIControlGroup.h"
#include "Controls/UICutsceneControl.h"
#include "Controls/UIEditControl.h"
#include "Controls/UIFloatBubbleControl.h"
#include "Controls/UIHotKeyControl.h"
#include "Controls/UIHotbarButtonControl.h"
#include "Controls/UIItemButtonControl.h"
#include "Controls/UIItemTagControl.h"
#include "Controls/UIProgressBarControl.h"
#include "Controls/UIRActorControl.h"
#include "Controls/UIRadioButtonControl.h"
#include "Controls/UIRadioButtonGroupControl.h"
#include "Controls/UIScrollableTextControl.h"
#include "Controls/UIScrollbarControl.h"
#include "Controls/UIStackPanelControl.h"
#include "Controls/UITextControl.h"
#include "Controls/UIThumbButtonControl.h"
#include "Controls/UITimerControl.h"
#include "Controls/UIToolTipControl.h"
#include "Controls/UITreeControl.h"
#include "Controls/UITutorialControl.h"
#include "Controls/UIWindowControl.h"
#include "Controls/UIWrapPanelControl.h"
#include "Interface/UIMinimap.h"
#include "UIAnimation.h"
#include "UIID.h"
#include "UIMessage.h"
#include "UIStoryboard.h"
#include "UITypes.h"
#include "UIAttributes.h"
#include "../Definitions/TextureDefinition.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Common/GameTemplate.h"
#include "../../Sigma/Random.h"
#include "../Battlenet/Hub.h"
#include "../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Battlenet/Subsystems/ScreenManager.h"
#include "../CPlayerInput.h"
#include "../../Common/Player.h"
#include "UIHeroData.h"
#include "Interface/UIConfirmation.h"
#include "Interface/UIGame.h"
#include "Interface/UIWaypointMapCommon.h"
#include "Interface/UIConversations.h"
#include "Interface/UIQuestComplete.h"
// GfxVB.h and the nn/ldn headers supply the header statics of _GLOBAL__sub_I_UI.cpp (SNONAME_NONE, cgplaceNull, four RGBAColor, the ldn
// addresses).
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

// SafeZone, UI, the control map typedefs and UIGlobals are in UI.h. The XHashKey specialization below stays here: this is
// the one file that instantiates CControlOnlyMap.
// UI_Input (DWARF UI.cpp:3338) and sUpdateActivePopupControlVisibility (3052) need these.
#include "../Camera/CameraLocation.h"
#include "../Camera/CameraManager.h"
#include "Controls/UIContextMenuControl.h"







// The generic XHashKey in XBaseMap.h casts the key straight to uint32, which does not compile
// for a pointer key; the binary hashes the low four bytes of the pointer the same way.
template <>
inline uint32 XHashKey<UIControl *>(UIControl * key) {
    uint32 dwKey = (uint32)(uintptr)key;
    uint32 hash = 0x811C9DC5;
    hash ^= dwKey & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 8) & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 16) & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 24) & 0xFF;
    hash *= 0x01000193;
    return hash;
}


























typedef blz::vector<UIID, blz::allocator<UIID> > UIIDArray;
typedef BOOL (*FocusableControlPredicate)(const UIID &);








































struct SpriteThreadData {  // size 0x38
    Blizzard::Lock::Event tWorkPendingEvent;  // 0x00
    Blizzard::Lock::Event tWorkCompleteEvent;  // 0x0c
    Blizzard::Thread::Thread tThread;  // 0x18
    XCommandBuffer * pSpriteThreadBuffer;  // 0x20
    DataIDMemoryPool * pDataIDMemoryPool;  // 0x28
    bool bActive;  // 0x30
};
static_assert(sizeof(SpriteThreadData) == 0x38, "SpriteThreadData");

// X/Client/Gfx/Gfx.cpp
BOOL GfxIsLetterboxed();
// X/Client/UserInterface/Controls/UIConsoleEditControl.cpp
namespace UIConsole {
void OutputString(const CHAR * puzString);
}

void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);
// X/Client/CGame.cpp
extern const Rect2D cgrectViewportDefault;
void UI_PopDialog(UIIDList & listMenuStack, const BOOL fActivatePrev);
void UI_SetCapture(const UIID & idCapture);

static float sgflCurrentAspectRatio = 4.0f / 3.0f;
static SpriteThreadData stSpriteThreadData;

// X/Client/Cutscene.cpp
CutsceneID CutsceneGetFullScreenCutscene();
// X/Client/UserInterface/Interface/UICredits.cpp, UIVictory.cpp
namespace UICredits {
BOOL IsActive();
}
namespace UIVictory {
BOOL IsActive();
}
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
void InitializeForWorld();
void DisposeForWorld();
}  // namespace Screens
}  // namespace Console

BOOL UI_SendMessage(const UIID & idRecipient, const UIMsg & msg, const uintptr dwParam);

inline int32 UI_GetXMLInt32Constant(const UIControlSourceKey & tSrcKey, int32 nDefaultValue);
static BOOL sKeyboardFocusPred(const UIID & idControl);
static UIID sIncrementFocus(const UIID & idCurrentFocus, const BOOL fReverseTabOrder, FocusableControlPredicate pfnPredicate);

// X/Client/UserInterface/Console/ConsoleScreenManager.h:83 (only the screen set this file opens)
namespace Console {
namespace Screens {
enum { MAIN_MENU_SCREEN_SET = 20 };
void Update();  // ConsoleScreenManager.cpp
BOOL OpenToScreen(const ScreenSets eScreenSet, const int32 idScreen, int32 nControllerOpening, BOOL fSkipTransition);  // ConsoleScreenManager.cpp:1104
}  // namespace Screens
namespace Notifications {
void EnableHostMigrationLoadscreenTip(BOOL bEnable);  // ConsoleScreenManager.cpp
}
namespace WebBrowser {
void Update();  // X/Client/Console/WebBrowserNX64.cpp
}
namespace Touch {
void Update();  // X/Client/UserInterface/Console/UIConsoleTouch.cpp
}
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleStartupFlow.cpp
namespace UIStartupFlow {
namespace Console {
void PerformStartupFlow();
}
}  // namespace UIStartupFlow
// X/Client/UserInterface/Interface/UIGame.cpp, UIConversations.cpp, UIQuests.cpp
namespace UIGame {
void Update(const float flDeltaTime);
}
namespace UIConversation {
void Update();
}
namespace UIQuests {
void Update();
}
// X/Client/UserInterface/Interface/UITutorials.h (only the member this file calls)
#include "Interface/UITutorials.h"
BOOL InputIsKeyboard(const BufferedInput & tInput);  // X/Client/Input/Input.cpp:989
BOOL InputIsController(const BufferedInput & tInput);  // X/Client/Input/Input.cpp:1015
void CPlayerInputClearHeldButton(const BufferedInput & tInput);  // X/Client/CPlayerInput.cpp
namespace UIConsole { BOOL IsActive(); }  // X/Client/UserInterface/Controls/UIConsoleEditControl.cpp
extern UIID UITEXT_CHATENTRY_EDITLINE;  // X/Client/UserInterface/UIIDs.h:128
extern UITutorialManager gtTutorialMgr;
UIEdit & UI_GetEditFromID(const UIID & id);  // defined below (DWARF UI.cpp:5667)
struct UILoadingScreenData {  // size 0x48
    UILoadingScreenData();
    virtual ~UILoadingScreenData();  // vtable slot 0
    ImageTextureFrame m_tLoadingScreenMain;  // 0x08
    ImageTextureFrame m_tLoadingScreenLeftCap;  // 0x10
    ImageTextureFrame m_tLoadingScreenRightCap;  // 0x18
    ImageTextureFrame m_tLoadingScreenDiabloIcon;  // 0x20
    ImageTextureFrame m_tLoadingScreenStatusIcon;  // 0x28
    ImageTextureFrame m_tLoadingScreenHourGlass;  // 0x30
    UIAnimation<unsigned char> * m_ptLoadingScreenIconFade;  // 0x38
    UIText * m_ptText;  // 0x40
};
static_assert(sizeof(UILoadingScreenData) == 0x48, "UILoadingScreenData");

// Declarations for the functions added by agentB (2026-11-13); each names the file that defines it.
Hub & HubGet();  // X/Client/Battlenet/Hub.cpp:93
CursorMgr & PlayerGetCursorMgr();  // X/Client/CPlayerInput.cpp:6603
BOOL CutsceneCanBeCancelled(const CutsceneID idCutscene);  // X/Client/Cutscene.cpp:2138
void UISideScreenCloseAll(uint32 dwSideScreenAlterMask);  // X/Client/UserInterface/Interface/UISideScreen.cpp:363
namespace UIJoinRift {
BOOL IsActive();  // X/Client/UserInterface/Console/UIConsoleJoinRift.cpp:402
}
namespace UIOptionsConfirmation {
BOOL IsVisible();  // :61
void CancelAndClose();  // :71
}
namespace UIFollowerSwap {
BOOL IsVisible();  // :90
void Close();  // :100
}
namespace UICountDown {
void PlayerMoved();  // :196
}
namespace UIQuests {
void ResetUI();  // X/Client/UserInterface/Interface/UIQuests.cpp:1035
}
void UIGlobalsInit();  // X/Client/UserInterface/UIGlobals.cpp:9006
void FormattedTextInitializeForApp();  // :642
void FormattedTextDisposeForApp();  // :681
namespace OnlineService {
void AchievementsUtilInitializeForApp();  // :1392
void AchievementsUtilDisposeForApp();  // :1401
}
namespace UICredits {
void InitializeForApp();  // X/Client/UserInterface/Interface/UICredits.cpp:116
}
namespace Console {
namespace Screens {
void InitializeForApp();  // ConsoleScreenManager.cpp:745
void DisposeForApp();  // ConsoleScreenManager.cpp:759
}
namespace UISaveIcon {
void InitializeForApp();  // :212
void DisposeForApp();  // :224
}
}
namespace UIInventory {
namespace Console {
void ItemFiltersInitializeForApp();  // :110
void ItemFiltersDisposeForApp();  // :120
}
}

// Declarations for the loading-screen functions (agentB, 2026-11-13); each names the file that defines it.
namespace Console {
namespace GamerProfile {
int32 GetPrimaryProfileUserIndex();  // X/Client/Console/ConsoleGamerProfile.cpp:1130
}
namespace Online {
int32 PartyGetOnlineState();  // returns Console::Online::PartyOnlineState  // X/Client/Console/ConsoleOnline.cpp:4628
}
}

// X/Client/UserInterface/UI.cpp (DWARF struct, declared in this file)
struct LoadscreenTipList {  // size 0x8
    SNO snoList;  // 0x00
    uint32 nNumStrings;  // 0x04
};
static_assert(sizeof(LoadscreenTipList) == 0x8, "LoadscreenTipList");

void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot);  // line 6196

UI & UIGet();

// DWARF UI.cpp:243 (const int32 MAX_CONTROLS = 8192); used by UI::Initialize.
#line 243
const int32 MAX_CONTROLS = 8192;
#line 245
static BOOL sgfTraceTargetControl = FALSE;

#line 251
inline UILoadingScreenData::UILoadingScreenData()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    : m_tLoadingScreenMain(cgtImageTextureFrameNull)
    , m_tLoadingScreenLeftCap(cgtImageTextureFrameNull)
    , m_tLoadingScreenRightCap(cgtImageTextureFrameNull)
    , m_tLoadingScreenDiabloIcon(cgtImageTextureFrameNull)
    , m_tLoadingScreenStatusIcon(cgtImageTextureFrameNull)
    , m_tLoadingScreenHourGlass(cgtImageTextureFrameNull)
    , m_ptLoadingScreenIconFade(NULL)
    , m_ptText(NULL)
{
}

#line 263
UILoadingScreenData::~UILoadingScreenData()
{
    if (m_ptLoadingScreenIconFade)
    {
        ::delete m_ptLoadingScreenIconFade;
        m_ptLoadingScreenIconFade = NULL;
    }
}

#line 282
static UILoadingScreenData * s_ptUILoadingScreenData = NULL;

static const float scnCapWidth = 160.20001220703125f;
static const uint32 LOADING_SCREEN_ICON_FADE_LENGTH = 160;
static const uint32 LOADING_SCREEN_ICON_FADE_OFFSET = 19;

#line 419
Rect2D SafeZone::GetMargins() const
{
    const VideoPrefs & tVideoPrefs = Prefs::GetVideoPrefs();
    return Rect2D(tVideoPrefs.flSafeZoneLeft, tVideoPrefs.flSafeZoneTop, 1.0f - tVideoPrefs.flSafeZoneRight, 1.0f - tVideoPrefs.flSafeZoneBottom);
}

#line 440
void SafeZone::SetMargins(const float flLeft, const float flTop, const float flRight, const float flBottom)
{
    VideoPrefs & tVideoPrefs = Prefs::GetMutableVideoPrefs();
    tVideoPrefs.flSafeZoneLeft = flLeft;
    tVideoPrefs.flSafeZoneTop = flTop;
    tVideoPrefs.flSafeZoneRight = 1.0f - flRight;
    tVideoPrefs.flSafeZoneBottom = 1.0f - flBottom;

    if (UI_TestID(cgUIIDRoot))
    {
        UI_GetWindowFromID(cgUIIDRoot).UpdateAbsolutePositions();
    }
}

#line 464
void SafeZone::ApplyToRect(Rect2D & rect)
{
    Rect2D rectMargins = GetMargins();
    float flUnsafeLeft = rectMargins.left * (rect.right - rect.left);
    float flUnsafeRight = rectMargins.right * (rect.right - rect.left);
    float flUnsafeTop = rectMargins.top * (rect.bottom - rect.top);
    float flUnsafeBottom = rectMargins.bottom * (rect.bottom - rect.top);
    rect.left += flUnsafeLeft;
    rect.top += flUnsafeTop;
    rect.right -= flUnsafeRight;
    rect.bottom -= flUnsafeBottom;
}

#line 501
void UI_SetCapture(const UIID & idCapture)
{
    UI & UI = UIGet();
    UI.idCapture = idCapture;

    if (idCapture == GetUIIDNone())
    {
        const UIID & idHit = UI_GetMousedOverControl();
        if (idHit != GetUIIDNone())
        {
            UI_GetControlFromID(idHit).EnteredHitRegion();
        }
    }
}

#line 526
const UIID & UI_GetCapture()
{
    UI & UI = UIGet();
    return UI.idCapture;
}

#line 538
int32 UI_GetPrimaryUIController()
{
    UI & UI = UIGet();
    return UI.nPrimaryController;
}

#line 548
void UI_SetPrimaryUIController(int32 nPrimaryController)
{
    UI & UI = UIGet();
    UI.nPrimaryController = nPrimaryController;
}

#line 556
BOOL UI_ValidateController(int32 nController)
{
    UI & UI = UIGet();
    return UI.nPrimaryController == -1 || UI.nPrimaryController == nController;
}

#line 573
void UI_SetKeyboardFocus(const UIID & idFocus)
{
    UI & UI = UIGet();
    if (idFocus == UI.idKeyboardFocus)
    {
        return;
    }

    UIID idOldFocus(UI.idKeyboardFocus);
    UI.idKeyboardFocus = idFocus;

    if (UI_TestID(idOldFocus))
    {
        UI_GetControlFromID(idOldFocus).LostKeyboardFocus();
    }
    if (UI_TestID(UI.idKeyboardFocus))
    {
        UI_GetControlFromID(UI.idKeyboardFocus).GainedKeyboardFocus();
    }
}

#line 599
void UI_SetControllerFocus(const UIID & idFocus)
{
    UI & UI = UIGet();
    if (idFocus == UI.idControllerFocus)
    {
        return;
    }

    UIID idOldFocus(UI.idControllerFocus);
    UI.idControllerFocus = idFocus;

    if (UI_TestID(idOldFocus))
    {
        UI_GetControlFromID(idOldFocus).LostControllerFocus();
    }
    if (UI_TestID(UI.idControllerFocus))
    {
        UI_GetControlFromID(UI.idControllerFocus).GainedControllerFocus();
    }
}

#line 626
void UI_HandleKeyboardFocusLost()
{
    if (UI_TestID(cgUIIDRoot))
    {
        const UIID & idNewFocus = UI_GetWindowFromID(cgUIIDRoot).GetInitialKeyboardFocus();
        UI_SetKeyboardFocus(idNewFocus);
    }
}
// LINKER-REMOVED: DWARF UI.cpp:637, concrete DIE at low_pc 0 with size 0x14 (prologue row 639, call rows 649:29
// GetUIIDNone and 649:9 UI_SetKeyboardFocus, the same two calls UI_Input inlines at 3433). Nothing else is inferred.
#line 637
void UI_HandleControllerFocusLost()

{









        UI_SetKeyboardFocus(GetUIIDNone());
}


#line 656
const UIID & UI_GetKeyboardFocus()
{
    UI & UI = UIGet();

    if (UI.idKeyboardFocus != GetUIIDNone())
    {
        if (!UI_TestID(UI.idKeyboardFocus))
        {
            UI.idKeyboardFocus = GetUIIDNone();
        }
    }
    return UI.idKeyboardFocus;
}

#line 674
const UIID & UI_GetControllerFocus()
{
    UI & UI = UIGet();

    if (UI.idControllerFocus != GetUIIDNone())
    {
        if (!UI_TestID(UI.idControllerFocus))
        {
            UI.idControllerFocus = GetUIIDNone();
        }
    }
    return UI.idControllerFocus;
}

#line 737
const UIID & UI_GetActivePopupControl()
{
    UI & UI = UIGet();

    if (UI.idActivePopupControl != GetUIIDNone())
    {
        if (!UI_TestID(UI.idActivePopupControl))
        {
            UI.idActivePopupControl = GetUIIDNone();
        }
    }
    return UI.idActivePopupControl;
}

#line 757
const UIID & UI_GetMousedOverControl()
{
    return UIGet().GetMousedOverControl();
}

#line 857
void UI_CloseActivePopupControl()
{
    UI & UI = UIGet();

    if (UI.idActivePopupControl != GetUIIDNone())
    {
        UIControl & tControl = UI_GetControlFromID(UI.idActivePopupControl);
        if (tControl.IsType(UIT_CONTEXTMENU))
        {
            tControl.SetActive(FALSE);
            UI_DeleteControl(UI.idActivePopupControl, TRUE);
            UI.idActivePopupControl = GetUIIDNone();
        }
    }
}

#line 884
BOOL UI_CloseAllTabs()
{
    BOOL fAllowMainMenuToBeShown = TRUE;
#line 897
    PlayerGetCursorMgr().ResetCursorState(FALSE);

    CutsceneID idCutscene = CutsceneGetFullScreenCutscene();
    if (idCutscene != CUTSCENEID_INVALID)
    {
#line 904
        if (CutsceneCanBeCancelled(idCutscene))
        {
            CutsceneStop(idCutscene, FALSE);
        }
        return FALSE;
    }
#line 921
    if (UIConfirmation::IsVisible())
    {
        UIConfirmation::Close();
        return FALSE;
    }
#line 936
    ModalNotificationManager & tManager = HubGet().ModalNotificationManagerGet();
    if (tManager.IsNotificationShowing())
    {
        tManager.CancelCurrentlyShowingNotification();
        return FALSE;
    }

    if (CGameInScriptedSequence())
    {
#line 948
        CGameGlobals & tCGameGlobals = CGameGlobalsGet();
        if (!tCGameGlobals.bScriptedSequenceCancelled)
        {
            UIGame::ConfirmSkipEvent();
        }
        return FALSE;
    }
#line 958
    if (UIWaypointMap::IsOpenForTutorial())
    {
        return FALSE;
    }
#line 965
    if (UIJoinRift::IsActive())
    {
        return FALSE;
    }
#line 971
    if (UIOptionsConfirmation::IsVisible())
    {
        UIOptionsConfirmation::CancelAndClose();
        return FALSE;
    }
#line 978
    if (UIFollowerSwap::IsVisible())
    {
        UIFollowerSwap::Close();
        return FALSE;
    }
#line 985
    UI_CloseActivePopupControl();
#line 1049
    if (UIConversation::CanCancel())
    {
#line 1052
        UIConversation::ConsoleCancel(GetUIIDNone(), -1);
        fAllowMainMenuToBeShown = FALSE;
    }
#line 1070
    if (UIRewards::IsAnyVisible())
    {
        UIRewards::CloseAll();
        fAllowMainMenuToBeShown = FALSE;
    }
#line 1077
    if (Console::Screens::IsScreenSetOpen())
    {
        Console::Screens::CloseAllScreenSets(FALSE);
        return FALSE;
    }
#line 1096
    return fAllowMainMenuToBeShown;
}

// Matches once UI_FindControl (5247, linker-removed) is present: its out-of-line push_back<UIID> call makes the
// inliner inline reserve into push_back, so push_back stays out of line here, as in the original.

#line 1103
static void sGetFocusableControls(
    const UIID & idInputFocusGroup,
    FocusableControlPredicate pfnPredicate,
    UIIDArray & arFocusableControls)
{
    UIControl & tControl = UI_GetControlFromID(idInputFocusGroup);
    if (tControl.IsType(UIT_CONTROLGROUP))
    {
        const UIControlItemArray & arChildList = UI_GetControlGroupFromID(idInputFocusGroup).GetChildList();
        for (uint32 i = 0; i < arChildList.size(); i++)
        {
            if (!arChildList[i].ptControl->IsType(UIT_WINDOW) || !UI_GetWindowFromID(arChildList[i].id).IsInputFocusGroup())
            {
                if (pfnPredicate(arChildList[i].id))
                {
                    arFocusableControls.push_back(arChildList[i].id);
                }
                sGetFocusableControls(arChildList[i].id, pfnPredicate, arFocusableControls);
            }
        }
    }
}

#line 1128
static int32 sFocusableControlsSortPred(const void * pDataA, const void * pDataB)
{


    const UIID & idA = *(const UIID *)pDataA;
    const UIID & idB = *(const UIID *)pDataB;


    uint32 nOrderA = (UI_TestID(idA) && UI_GetControlFromID(idA).IsType(UIT_WINDOW)) ? UI_GetWindowFromID(idA).GetInputFocusOrder() : 0xffffffff;
    uint32 nOrderB = (UI_TestID(idB) && UI_GetControlFromID(idB).IsType(UIT_WINDOW)) ? UI_GetWindowFromID(idB).GetInputFocusOrder() : 0xffffffff;
    if (nOrderA > nOrderB)
    {
        return 1;
    }
    else if (nOrderB > nOrderA)
    {

        return -1;
    }
    return 0;
}

#line 1162
static BOOL sKeyboardFocusPred(
    const UIID & idControl)
{
    if (UI_TestID(idControl))
        return UI_GetControlFromID(idControl).CanAcceptKeyboardFocus();
    return FALSE;
}

// DWARF UI.cpp:1171 (concrete DIE at low_pc 0, linker-dropped); mirror of sKeyboardFocusPred.
// Inlined calls UI_TestID at 1174 and UI_GetControlFromID at 1175; CanAcceptControllerFocus
// is UIControl's controller counterpart of CanAcceptKeyboardFocus (inferred from the mirror).
#line 1171
static BOOL sControllerFocusPred(
    const UIID & idControl)
{
    if (UI_TestID(idControl))
        return UI_GetControlFromID(idControl).CanAcceptControllerFocus();
    return FALSE;
}




#line 1180
static UIID sIncrementFocus(
    const UIID &idCurrentFocus,
    const BOOL fReverseTabOrder,
    FocusableControlPredicate pfnPredicate)
{
    if (!pfnPredicate || !UI_TestID(idCurrentFocus))
    {

        return GetUIIDNone();
    }



    UIID idParent = UI_GetControlFromID(idCurrentFocus).GetParent();
    while (UI_TestID(idParent) && !(UI_GetControlFromID(idParent).IsType(UIT_WINDOW) && UI_GetWindowFromID(idParent).IsInputFocusGroup()) && idParent != cgUIIDRoot)
    {

        idParent = UI_GetControlFromID(idParent).GetParent();
        if (idParent == GetUIIDNone())
        {

            return GetUIIDNone();
        }
    }



    UIIDArray arFocusableControls;
    sGetFocusableControls(idParent, pfnPredicate, arFocusableControls);
    blzext::vector_sort(arFocusableControls, sFocusableControlsSortPred);
    int32 posLastFocus = blzext::vector_find(arFocusableControls, idCurrentFocus);
    if (posLastFocus < 0)
    {


        return GetUIIDNone();
    }


    if (fReverseTabOrder)
    {

        int32 posNextFocus = Math::PinWrap(posLastFocus - 1, 0, (int32)arFocusableControls.size());
        return arFocusableControls[posNextFocus];
    }
    else
    {

        int32 posNextFocus = Math::PinWrap(posLastFocus + 1, 0, (int32)arFocusableControls.size());
        return arFocusableControls[posNextFocus];
    }


}

#line 1226
void UI_IncrementKeyboardFocus(
    const BOOL fReverseTabOrder)
{
    UIID idCurrentFocus = UI_GetKeyboardFocus();
    if (idCurrentFocus == GetUIIDNone())
    {
        return;
    }

    UIID idNextFocus = sIncrementFocus(idCurrentFocus, fReverseTabOrder, sKeyboardFocusPred);
    if (idNextFocus == GetUIIDNone())
    {
        return;
    }

    UI_SetKeyboardFocus(idNextFocus);
}

// DWARF UI.cpp:1246 (concrete DIE at low_pc 0, linker-dropped); locals idCurrentFocus:1249,
// idNextFocus:1255, UIID compares at 1250/1256: the controller mirror of UI_IncrementKeyboardFocus.
#line 1246
void UI_IncrementControllerFocus(
    const BOOL fReverseTabOrder)
{
    UIID idCurrentFocus = UI_GetControllerFocus();
    if (idCurrentFocus == GetUIIDNone())
    {
        return;
    }

    UIID idNextFocus = sIncrementFocus(idCurrentFocus, fReverseTabOrder, sControllerFocusPred);
    if (idNextFocus == GetUIIDNone())
    {
        return;
    }

    UI_SetControllerFocus(idNextFocus);
}

#line 1267
UIID UI_HitTest(const IVector2D & pt)
{
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        return GetUIIDNone();
    }

    UIControl & tControl = UI_GetControlFromID(cgUIIDRoot);
    return tControl.HitTest(pt);
}

#line 1296
void UIInitializeForApp()
{

    UIAppGlobals * ptUIAppGlobals = (UIAppGlobals *)SigmaMemoryNew(sizeof(UIAppGlobals), 4, NULL, TRUE);

    UIGlobalsInit();

    ptUIAppGlobals->mapUIState.Initialize(16, FALSE, NULL);
    ptUIAppGlobals->listStartupDlgStack.Initialize(NULL);
    ptUIAppGlobals->listInGameDlgStack.Initialize(NULL);
    ptUIAppGlobals->ptHeroData = new UIHeroData();
    ptUIAppGlobals->ptHeroData->Set("Default", OnlineService::INVALID_HERO_ID);

    sgtAppGlobals.ptUIAppGlobals = ptUIAppGlobals;

#line 1312
    s_ptUILoadingScreenData = new UILoadingScreenData();

    FormattedTextInitializeForApp();

    OnlineService::AchievementsUtilInitializeForApp();

    UICredits::InitializeForApp();
#line 1340
    Console::Screens::InitializeForApp();
    Console::UISaveIcon::InitializeForApp();
    UIInventory::Console::ItemFiltersInitializeForApp();
}

#line 1349
void UIDisposeForApp()
{
#line 1373
    UIInventory::Console::ItemFiltersDisposeForApp();
    Console::UISaveIcon::DisposeForApp();
    Console::Screens::DisposeForApp();

    OnlineService::AchievementsUtilDisposeForApp();
    FormattedTextDisposeForApp();

    UIAppGlobals *& ptUIAppGlobals = sgtAppGlobals.ptUIAppGlobals;

    ptUIAppGlobals->mapUIState.RemoveAll();
    ptUIAppGlobals->mapUIState.Dispose();

    ptUIAppGlobals->listStartupDlgStack.RemoveAll();
    ptUIAppGlobals->listStartupDlgStack.Dispose();

    ptUIAppGlobals->listInGameDlgStack.RemoveAll();
    ptUIAppGlobals->listInGameDlgStack.Dispose();

    delete ptUIAppGlobals->ptHeroData; ptUIAppGlobals->ptHeroData = NULL;
    ::delete s_ptUILoadingScreenData; s_ptUILoadingScreenData = NULL;
    SigmaMemoryFree(ptUIAppGlobals, NULL); ptUIAppGlobals = NULL;
}

#line 1399
UIIDList & UI_GetStartupDlgStack()
{
    return sgtAppGlobals.ptUIAppGlobals->listStartupDlgStack;
}

#line 1408
UIIDList & UI_GetInGameDlgStack()
{
    return sgtAppGlobals.ptUIAppGlobals->listInGameDlgStack;
}

#line 1417
void UI_ClearDialogStack(UIIDList & listMenuStack)
{
    while (listMenuStack.Count())
    {
        UI_PopDialog(listMenuStack, FALSE);
    }
}

#line 1429
void UI_PushDialog(UIIDList & listMenuStack, const UIID & idNewMenu, const BOOL fCloseCurrent, const BOOL fSetFocusToNewDialog)
{







    if (fCloseCurrent && listMenuStack.Count() > 0)
    {
        UIID idCurrentMenu = listMenuStack.GetHead();

        UIControl & tControlCurrent = UI_GetControlFromID(idCurrentMenu);
        tControlCurrent.SetActive(FALSE);
    }



    UIWindow & tDialogNewMenu = UI_GetWindowFromID(idNewMenu);
    tDialogNewMenu.SetActive(TRUE);

    if (fSetFocusToNewDialog)
    {
        UI_SetKeyboardFocus(tDialogNewMenu.GetInitialKeyboardFocus());
        tDialogNewMenu.SetInitialEditText();
        UI_SetCapture(GetUIIDNone());
    }



    listMenuStack.AddHead_Tracked(idNewMenu);
}

#line 1464
void UI_PopDialog(UIIDList & listMenuStack, const BOOL fActivatePrev)
{



    UIID idCurrentMenu = listMenuStack.GetHead();





    if (UI_TestID(idCurrentMenu))
    {


        UIControl & tControlCurrent = UI_GetControlFromID(idCurrentMenu);
        tControlCurrent.SetActive(FALSE);
    }







    listMenuStack.RemoveHead();
    if (fActivatePrev && listMenuStack.Count() > 0)
    {


        UIID idNewMenu = listMenuStack.GetHead();
        UI_GetControlFromID(idNewMenu).SetActive(TRUE);
    }
}

// Declarations for UI_InitializeForGame (the owning headers are not adopted yet).
#include "../../Sigma/Stopwatch.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../Prefetch.h"
#include "../SNOPreload.h"
#include "UIInit.h"
struct SNOAndTextureLoad {  // size 0xc
    SNO snoHandle;  // 0x00
    BOOL fLoadChildren;  // 0x04
    BOOL fKeepUI;  // 0x08
};
BOOL InputRegisterListener(const InputMsg eInputMsg, const InputListenerFunc pfunc, InputContext eContext, int32 nIndex);  // X/Client/Input/Input.cpp:217
BOOL UI_Input(const BufferedInput & tInput);
float GfxGetAspectRatio();  // X/Client/Gfx/Gfx.cpp
void ConfigureUISpriteThread(Blizzard::Thread::Thread tThread);  // X/Common/Shell/MainNX64.cpp
void UIFunction_InitializeMap();
void UIConsoleFunction_InitializeMap();
void UITextControlInitializeForGame();
void UI_StoryboardsInitializeForGame();
void UIStartup_InitializeForGame();
void UIMinimap_InitializeForGame();
void CPlayerUIInitializeForGame();
namespace UIOptions { namespace Console { void InitializeForGame(); } }
namespace UIFirstTimeFlow { namespace Console { void InitializeForGame(); } }
namespace UIGammaAdjust { namespace Console { void InitializeForGame(); } }
namespace UIControllerLayout { namespace Console { void InitializeForGame(); } }
namespace UISafeZoneSettings { void InitializeForGame(); }
namespace UIPartyDisplay { namespace Console { void InitializeForGame(); } }
namespace UIMainMenu { namespace Console { void InitializeForGame(); } }
namespace UIConsoleRiftEndScreen { void InitializeForGame(); }
namespace UILeaderboardInspect { namespace Console { void InitializeForGame(); } }
namespace UIGame { void InitializeForGame(); }
namespace UIConversation { void InitializeForGame(); }
namespace UIQuests { void InitializeForGame(); }
namespace UIQuestLore { void InitializeForGame(); }
namespace ControllerDisconnectedHandler { namespace Console { void InitializeForGame(); } }
static void sSpriteThreadInitialize();
static void sAspectRatioInit();
static uint sSpriteThreadProc(void * pDataIn);

// Declarations for UI_DisposeForGame (the owning headers are not adopted yet).
void UI_StoryboardsDisposeForGame();
void CPlayerUIDisposeForGame();
namespace UIQuestLore { void DisposeForGame(); }
namespace UIQuests { void DisposeForGame(); }
namespace UIConversation { void DisposeForGame(); }
namespace UILeaderboardInspect { namespace Console { void DisposeForGame(); } }
namespace UIControllerLayout { namespace Console { void DisposeForGame(); } }
namespace UIGammaAdjust { namespace Console { void DisposeForGame(); } }
namespace UIFirstTimeFlow { namespace Console { void DisposeForGame(); } }
namespace UIOptions { namespace Console { void DisposeForGame(); } }
namespace UISafeZoneSettings { void DisposeForGame(); }
namespace UIPartyDisplay { namespace Console { void DisposeForGame(); } }
namespace UIMainMenu { namespace Console { void DisposeForGame(); } }
namespace UIConsoleRiftEndScreen { void DisposeForGame(); }
void UIMinimap_DisposeForGame();
void UIQuit_DisposeForGame();
namespace UIGame { void DisposeForGame(); }
void UIStartup_DisposeForGame();
void UITextControlDisposeForGame();
void InputUnregisterListener(const InputMsg eInputMsg, const InputListenerFunc pfunc, InputContext eContext);  // X/Client/Input/Input.cpp
static void sSpriteThreadDispose();

// Lines 1552-1568 from the original line table; the UIID members are set in the order of
// their stores (0x28, 0x30, 0x50, 0x20, 0x38, 0x48, 0x40).
#line 1549
void UI::Initialize()

{
    pmapElements = new CControlMap(16, NULL);
    pmapElements->SetHashTableSize_Tracked(MAX_CONTROLS);
    fRootIsSet = FALSE;
    idKeyboardFocus = GetUIIDNone();
    idVisibleFocus = GetUIIDNone();
    idControllerFocus = GetUIIDNone();
    idCapture = GetUIIDNone();
    idActivePopupControl = GetUIIDNone();
    idMousedOverControl = GetUIIDNone();
    idLastClickedControl = GetUIIDNone();
    bLeftTriggerBounceForUI = FALSE;
    bRightTriggerBounceForUI = FALSE;

    fInputListenerArrayIsDirty = TRUE;
    pInputListeners = new CControlArray;
    pmapInputListeners = new CControlOnlyMap(16, NULL);
    nPrimaryController = -1;

}

#line 1600
void UI_InitializeForGame()
{
#line 1603
    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (ptUIGlobals)
    {
        return;
    }
    ptUIGlobals = (UIGlobals *)SigmaMemoryNew(sizeof(UIGlobals), 4, NULL, TRUE);
#line 1616
    Stopwatch tStopwatch;
#line 1618
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ptUIGlobals->tUIFunctionMap.Initialize(16, FALSE, ptMemoryInterface);
    ptUIGlobals->tUIFunctionMap.SetHashTableSize_Tracked(1024);
#line 1622
    UIFunction_InitializeMap();
#line 1624
    UIConsoleFunction_InitializeMap();
    UIOptions::Console::InitializeForGame();
    UIFirstTimeFlow::Console::InitializeForGame();
    UIGammaAdjust::Console::InitializeForGame();
    UIControllerLayout::Console::InitializeForGame();
    UISafeZoneSettings::InitializeForGame();
    UIPartyDisplay::Console::InitializeForGame();
    UIMainMenu::Console::InitializeForGame();
    UIConsoleRiftEndScreen::InitializeForGame();
    UILeaderboardInspect::Console::InitializeForGame();
#line 1635
    UITextControlInitializeForGame();
#line 1649
    UI_StoryboardsInitializeForGame();
#line 1652
    sSpriteThreadInitialize();
#line 1656
    sgflCurrentAspectRatio = 4.0f / 3.0f;
#line 1658
    ptUIGlobals->ui.Initialize();
    UIInit_CreateRoot();
#line 1665
    GameCommonData & tGCD = CGameGetGameCommonData();
    int32 nInsertionIndex = (tGCD.GameModeGet() == GAMEMODE_PLAYING) ? 0 : -1;
#line 1675
    InputRegisterListener(INPUTMSG_LEFTMOUSEDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_LEFTMOUSESTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_LEFTMOUSEUP, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_LEFTMOUSEDOUBLECLICK, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_MIDDLEMOUSEDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_MIDDLEMOUSESTILLDOWN_NO_DELAY, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_MIDDLEMOUSESTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_MIDDLEMOUSEUP, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_MIDDLEMOUSEDOUBLECLICK, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_RIGHTMOUSEDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_RIGHTMOUSEUP, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_RIGHTMOUSEDOUBLECLICK, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_MOUSEMOVE, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_KEYDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_KEYUP, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_CHARACTER, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_KEYDOWNONCE, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_KEYSTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_MOUSEWHEEL, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_LEFTSTICK, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_RIGHTSTICK, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_LEFTTRIGGER, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_RIGHTTRIGGER, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_GESTUREFLICK, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONSTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONHELDDOWN, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONREPEAT, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONUP, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_START_COMPOSITION, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_END_COMPOSITION, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_SHOW_COMPOSITION, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_INSERT_RESULT, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_LANGUAGE_CHANGE, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_OPEN_CANDIDATES, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_CHANGE_CANDIDATES, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_CLOSE_CANDIDATES, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_SHOWREADING, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
    InputRegisterListener(INPUTMSG_IME_HIDEREADING, UI_Input, INPUTCONTEXT_DEFAULT, nInsertionIndex);
#line 1719
    ptUIGlobals->idACDMonsterHPBar = ACDID_INVALID;
    ptUIGlobals->snoLastClickedMonster = SNO_NONE;
    ptUIGlobals->dwRiftClosingTick = 0;
    ptUIGlobals->uszLastClickedMonsterNamePretty[0] = 0;
    ptUIGlobals->nLastClickedMonsterLevel = 0;
    ptUIGlobals->idACDBossHPBar = ACDID_INVALID;
#line 1727
    ptUIGlobals->tMouseLocation.x = 0;
    ptUIGlobals->tMouseLocation.y = 0;
#line 1730
    ptUIGlobals->fInitialized = TRUE;
#line 1736
    const GameParams & tParams = CGameGet().tGameStartParams;
#line 1738
    ControlCreationParams::s_fDuringInitialization = TRUE;
    if (GameIsStartUp())
    {
        if (CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
        {
#line 1792
            SNOAndTextureLoad snoUI[] =
            {
                { SNOFileLookupGUID(UI_SNO, "Dialogs"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleStartScreen"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleMainMenu"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHeroSelect"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHeroCreate"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleGameSettings"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleOptions"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleUIRActorControls"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePlayerOptions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTutorial"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleButtonLegend"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleOrnateWarning"), FALSE, FALSE },
#line 1807
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewNX64"), FALSE, FALSE },
#line 1810
                { SNOFileLookupGUID(UI_SNO, "ConsoleEULA"), FALSE, FALSE },
#line 1812
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewPatches"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewSeasons"), FALSE, FALSE },
            };
#line 1850
            SNOAndTextureLoad snoTopLayerUI[] =
            {
                { SNOFileLookupGUID(UI_SNO, "ConsoleModalNotifications"), FALSE, FALSE },
#line 1854
                { SNOFileLookupGUID(UI_SNO, "ConsoleAmiiboTutorial"), FALSE, FALSE },
#line 1856
                { SNOFileLookupGUID(UI_SNO, "ConsoleCutscene"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleControllerDisconnect"), FALSE, FALSE },
            };
#line 1865
            for (int32 i = 0; i < sizeof(snoUI) / sizeof(snoUI[0]); i++)
            {
                SNOPreload::AddClientUI(snoUI[i].snoHandle);
                if (snoUI[i].fLoadChildren)
                {
                    SNOPreload::AddClientUIChildren(snoUI[i].snoHandle);
                }
            }
#line 1874
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                SNOPreload::AddClientUI(snoTopLayerUI[i].snoHandle);
                if (snoTopLayerUI[i].fLoadChildren)
                {
                    SNOPreload::AddClientUIChildren(snoTopLayerUI[i].snoHandle);
                }
            }
#line 1883
            SNOPreload::WaitForAllUIRoots();
#line 1885
            for (int32 i = 0; i < sizeof(snoUI) / sizeof(snoUI[0]); i++)
            {
                UI_CreateDlg(cgUIIDNormalLayer, snoUI[i].snoHandle);
            }
#line 1894
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                UI_CreateDlg(cgUIIDTopLayer, snoTopLayerUI[i].snoHandle);
            }
        }
        else
        {
#line 1959
            SNOAndTextureLoad snoUI[] =
            {
                { SNOFileLookupGUID(UI_SNO, "ConsoleStartScreen"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleMainMenu"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHeroSelect"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHeroCreate"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleOptions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleUIRActorControls"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePartyDisplay"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePlayerOptions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleGameSettings"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePublicGamesList"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleBannerCustomization"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleAchievements"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSeasonJourney"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleLeaderboards"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleParty"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleProfileSummary"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleProfileHallOfFallen"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCinematics"), FALSE, FALSE },
#line 1984
                { SNOFileLookupGUID(UI_SNO, "ConsoleControllerLayout_NX64"), FALSE, FALSE },
#line 1986
                { SNOFileLookupGUID(UI_SNO, "ConsoleGammaAdjustScreen"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleBattlenetAccountLinkScreen"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSafeZone"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleAutosaveWarning"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleRatingsScene"), TRUE, FALSE },
#line 1992
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewNX64"), FALSE, FALSE },
#line 1995
                { SNOFileLookupGUID(UI_SNO, "ConsoleEULA"), FALSE, FALSE },
#line 1997
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewPatches"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewSeasons"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleButtonLegend"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleOrnateWarning"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleNetworkOptions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleGamePrivacyOptions"), FALSE, FALSE },
#line 2013
                { SNOFileLookupGUID(UI_SNO, "Dialogs"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "Fade"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTutorial"), FALSE, FALSE },
            };
#line 2047
            SNOAndTextureLoad snoTopLayerUI[] =
            {
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_X1"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_2017"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_Eternal"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCreditsSound"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleModalNotifications"), FALSE, FALSE },
#line 2056
                { SNOFileLookupGUID(UI_SNO, "ConsoleAmiiboTutorial"), FALSE, FALSE },
#line 2058
                { SNOFileLookupGUID(UI_SNO, "ConsoleToasts"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleAchievementFanfare"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCutscene"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleControllerDisconnect"), FALSE, FALSE },
            };
#line 2069
            Prefetch::BeginAdd();
            for (int32 i = 0; i < sizeof(snoUI) / sizeof(snoUI[0]); i++)
            {
                SNOPreload::AddClientUI(snoUI[i].snoHandle);
                if (snoUI[i].fLoadChildren)
                {
                    SNOPreload::AddClientUIChildren(snoUI[i].snoHandle);
                }
            }
#line 2079
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                SNOPreload::AddClientUI(snoTopLayerUI[i].snoHandle);
                if (snoTopLayerUI[i].fLoadChildren)
                {
                    SNOPreload::AddClientUIChildren(snoTopLayerUI[i].snoHandle);
                }
            }
#line 2088
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsolePortraits"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsolePartyDisplay"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "BattleNetCampaignHeroScales"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleMenuNav"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "BattleNetStyleButtons"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleHUD"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleHUD_SinglePlayerFrame"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleGameDifficulty"));
#line 2097
            Prefetch::EndAdd();
#line 2099
            SNOPreload::WaitForAllUIRoots();
#line 2101
            for (int32 i = 0; i < sizeof(snoUI) / sizeof(snoUI[0]); i++)
            {
                UI_CreateDlg(cgUIIDNormalLayer, snoUI[i].snoHandle);
            }
#line 2110
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                UI_CreateDlg(cgUIIDTopLayer, snoTopLayerUI[i].snoHandle);
            }
        }
    }
    else
    {
#line 2120
        if (tParams.eMode == GAMEMODE_PLAYING)
        {
            SNOAndTextureLoad snoUI[] =
            {
                { SNOFileLookupGUID(UI_SNO, "ItemTags"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "Game"), FALSE, FALSE },
#line 2221
                { SNOFileLookupGUID(UI_SNO, "ConsoleMinimap"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "QuestUpdate"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleGameHUDMisc"), TRUE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleGameNotify"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleActTransitions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePlayingHotkeys"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleQuestReward"), TRUE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleBountyReward"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTieredRifts_Reward"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTieredRifts_BloodshardReward"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTieredRifts_Scorecard"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleConversationOptions"), TRUE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleConversationDialog"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleMap"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleWaypointMap"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleEventText"), TRUE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "CountDown"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleInventoryRing"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleInventoryList"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleInventoryItem"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCharacterDetails"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleFollowerRing"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleWaypointList"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleStash"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleEquipmentManager"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleMail"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleFriendsList"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSkillSelection"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleParagon"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleParty"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTrade"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleQuests"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleJournal"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTieredRifts_Entry"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTieredRifts_Urshi"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTieredRifts_GemUpgrade"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleKanais_Cube"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleKanais_Collection"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleEmoteMenu"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHUD_SinglePlayerFrame"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHUD"), TRUE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleBossJoinParty"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleBossEnter"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSetDungeonJoinParty"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSetDungeonCompletion"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHeroSelect"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHeroCreate"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleOrnateWarning"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleChallenge"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleUIRActorControls"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleTutorial"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePause"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleOptions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePlayerOptions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsolePartyDisplay"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleBannerCustomization"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleGameSettings"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleProfileSummary"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleAchievements"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSeasonJourney"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleLeaderboards"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "Console_RiftsVictoryScreen"), FALSE, FALSE },
#line 2288
                { SNOFileLookupGUID(UI_SNO, "ConsoleControllerLayout_NX64"), FALSE, FALSE },
#line 2290
                { SNOFileLookupGUID(UI_SNO, "ConsoleGammaAdjustScreen"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleBattlenetAccountLinkScreen"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSafeZone"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleDesignerMenu"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "Dialogs"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleDeathMenu"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleHardcoreDeath"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "Fade"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleDebugController"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleLoreNotify"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleLorePlayer"), FALSE, FALSE },
#line 2302
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewNX64"), FALSE, FALSE },
#line 2305
                { SNOFileLookupGUID(UI_SNO, "ConsoleEULA"), FALSE, FALSE },
#line 2307
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewPatches"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleWhatsNewSeasons"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleButtonLegend"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleNetworkOptions"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleGamePrivacyOptions"), FALSE, FALSE },
            };
#line 2342
            SNOAndTextureLoad snoTopLayerUI[] =
            {
                { SNOFileLookupGUID(UI_SNO, "VictoryScreen_X1"), FALSE, FALSE },
#line 2352
                { SNOFileLookupGUID(UI_SNO, "BlizzconReward"), FALSE, FALSE },
#line 2357
                { SNOFileLookupGUID(UI_SNO, "ConsoleVictoryScreen"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_X1"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_2017"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_Eternal"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCreditsSound"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleToasts"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleAchievementFanfare"), FALSE, TRUE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleFollowerSwap"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleConfirmation"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleSplitstack"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCutscene"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleScriptedSequence"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleModalTieredRiftsJoin"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleErrorNotify"), FALSE, FALSE },
#line 2373
                { SNOFileLookupGUID(UI_SNO, "ConsoleAmiiboTutorial"), FALSE, FALSE },
#line 2375
                { SNOFileLookupGUID(UI_SNO, "ConsoleModalNotifications"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleControllerDisconnect"), FALSE, FALSE },
            };
#line 2384
            Prefetch::BeginAdd();
            for (int32 i = 0; i < sizeof(snoUI) / sizeof(snoUI[0]); i++)
            {
                SNOPreload::AddClientUI(snoUI[i].snoHandle);
                if (snoUI[i].fLoadChildren)
                {
                    SNOPreload::AddClientUIChildren(snoUI[i].snoHandle);
                }
            }
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                SNOPreload::AddClientUI(snoTopLayerUI[i].snoHandle);
                if (snoTopLayerUI[i].fLoadChildren)
                {
                    SNOPreload::AddClientUIChildren(snoTopLayerUI[i].snoHandle);
                }
            }
#line 2403
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleSkillSelectionLoadoutTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleSkillSelectionListTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryRingLoadoutTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryListEquipmentTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryListMysticTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryListRecipeTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryListCubeTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryListCollectionTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryListCategoryTemplate"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleWaypointMap_Act"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleMailList"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleMailFriendList"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsolePortraits"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsolePartyDisplay"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleMenuNav"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "BattleNetStyleButtons"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleHUD_SinglePlayerFrame"));
            SNOPreload::AddClientUIChildren(SNOFileLookupGUID(UI_SNO, "ConsoleHUD_SinglePlayerFrame"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleInventoryItem"));
            SNOPreload::AddClientUI(SNOFileLookupGUID(UI_SNO, "ConsoleGameDifficulty"));
#line 2426
            Prefetch::EndAdd();
#line 2428
            SNOPreload::WaitForAllUIRoots();
#line 2430
            for (int32 i = 0; i < sizeof(snoUI) / sizeof(snoUI[0]); i++)
            {
                UI_CreateDlg(cgUIIDNormalLayer, snoUI[i].snoHandle);
            }
#line 2439
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                UI_CreateDlg(cgUIIDTopLayer, snoTopLayerUI[i].snoHandle);
            }
        }
        else
        {
#line 2483
            SNOAndTextureLoad snoTopLayerUI[] =
            {
                { SNOFileLookupGUID(UI_SNO, "ConsoleCutscene"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_X1"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_2017"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCredits_Eternal"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleCreditsSound"), FALSE, FALSE },
                { SNOFileLookupGUID(UI_SNO, "ConsoleControllerDisconnect"), FALSE, FALSE },
            };
#line 2498
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                SNOPreload::AddClientUI(snoTopLayerUI[i].snoHandle);
                if (snoTopLayerUI[i].fLoadChildren)
                {
                    SNOPreload::AddClientUIChildren(snoTopLayerUI[i].snoHandle);
                }
            }
            SNOPreload::WaitForAllUIRoots();
#line 2508
            for (int32 i = 0; i < sizeof(snoTopLayerUI) / sizeof(snoTopLayerUI[0]); i++)
            {
                UI_CreateDlg(cgUIIDTopLayer, snoTopLayerUI[i].snoHandle);
            }
        }
    }
#line 2518
    sAspectRatioInit();
#line 2522
    gtTutorialMgr.InitializeForGame();
    UIStartup_InitializeForGame();
#line 2544
    if (tParams.eMode == GAMEMODE_PLAYING
        && !GameIsStartUp())
    {
        UIGame::InitializeForGame();
#line 2554
        UIMinimap_InitializeForGame();
        UIConversation::InitializeForGame();
        UIQuests::InitializeForGame();
        UIQuestLore::InitializeForGame();
        CPlayerUIInitializeForGame();
    }
#line 2562
    ControllerDisconnectedHandler::Console::InitializeForGame();
#line 2564
    ControlCreationParams::s_fDuringInitialization = FALSE;
#line 2566
    UI_SendMessage(cgUIIDRoot, UI_INIT_FOR_GAME, 0);
}

#line 2574
void UI_GameInitialized()
{

    if (GameIsStartUp())
    {





        Console::Notifications::EnableHostMigrationLoadscreenTip(FALSE);
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            UIStartupFlow::Console::PerformStartupFlow();
        }
        else
        {

            Console::Screens::OpenToScreen(Console::Screens::MAIN_MENU_SCREEN_SET, 0, -1, FALSE);
        }
    }
}

#line 2602
static void sAspectRatioInit()
{

    if (GfxIsLetterboxed())
    {
        sgflCurrentAspectRatio = 16.0f / 9.0f;
    }
    else
    {
        sgflCurrentAspectRatio = GfxGetAspectRatio();
    }
}

#line 2618
void UI_AspectRatioChanged()
{
    float flPrevAspectRatio = sgflCurrentAspectRatio;
    Rect2D rectScreenNDC = cgrectViewportDefault;
    float flNewAspectRatio = CGameGetLetterboxedViewportNDC(rectScreenNDC);

    if (Math::Abs(flPrevAspectRatio - flNewAspectRatio) > 0.01f || GfxIsLetterboxed())
    {
        UI_GetWindowFromID(cgUIIDRoot).UpdateAbsolutePositions();
    }
    sgflCurrentAspectRatio = flNewAspectRatio;
}

#line 2640
void UI_SaveState()
{
#line 2644
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        return;
    }

    UIAppGlobals * ptUIAppGlobals = sgtAppGlobals.ptUIAppGlobals;
#line 2657
    ptUIAppGlobals->mapUIState.RemoveAll();

#line 2660
    UI & UI = UIGet();
    POS pos = UI.pmapElements->GetStartAssoc();
    while (pos)
    {
        uint64 idControl;
        UIControl * pValue;
        UI.pmapElements->GetNextAssoc(pos, idControl, pValue);

        UI_GetControlFromID(UIID(idControl)).SaveUIState(ptUIAppGlobals->mapUIState);
    }
}

#line 2675
void UI_RestoreState()
{
#line 2679
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (!ptUIGlobals)
    {
        return;
    }
    UI & UI = ptUIGlobals->ui;

    UIAppGlobals * ptUIAppGlobals = sgtAppGlobals.ptUIAppGlobals;

#line 2690
    {
        POS pos = UI.pmapElements->GetStartAssoc();
        while (pos)
        {
            uint64 idControl;
            UIControl * pValue;

            UI.pmapElements->GetNextAssoc(pos, idControl, pValue);

            if (pValue)
            {
                UI_GetControlFromID(UIID(idControl)).RestoreUIState(ptUIAppGlobals->mapUIState);
            }
        }
    }
#line 2706
    UI_GetControlFromID(cgUIIDRoot).UpdateCachedActiveState();

#line 2709
    ptUIAppGlobals->mapUIState.RemoveAll();
}

#line 2715
void UI_DisposeForGame()
#line 2717
{
    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;
    if (ptUIGlobals)
    {
        UI & UI = ptUIGlobals->ui;
#line 2723
        UI_SendMessage(cgUIIDRoot, UI_DISPOSE_FOR_GAME, 0);
#line 2743
        UI_StoryboardsDisposeForGame();
        CPlayerUIDisposeForGame();
        UIQuestLore::DisposeForGame();
        UIQuests::DisposeForGame();
        UIConversation::DisposeForGame();
#line 2758
        UILeaderboardInspect::Console::DisposeForGame();
        UIControllerLayout::Console::DisposeForGame();
        UIGammaAdjust::Console::DisposeForGame();
        UIFirstTimeFlow::Console::DisposeForGame();
        UIOptions::Console::DisposeForGame();
        UISafeZoneSettings::DisposeForGame();
        UIPartyDisplay::Console::DisposeForGame();
        UIMainMenu::Console::DisposeForGame();
        UIConsoleRiftEndScreen::DisposeForGame();
#line 2769
        UIMinimap_DisposeForGame();
        UIQuit_DisposeForGame();
        UIGame::DisposeForGame();
#line 2779
        gtTutorialMgr.DisposeForGame();
#line 2784
        UIStartup_DisposeForGame();
#line 2787
        UI_GetInGameDlgStack().RemoveAll();
#line 2790
        UIID idRoot(cgUIIDRoot);
        UI_DeleteControl(idRoot, FALSE);
        delete UI.pmapElements; UI.pmapElements = NULL;
#line 2795
        delete UI.pInputListeners; UI.pInputListeners = NULL;
        delete UI.pmapInputListeners; UI.pmapInputListeners = NULL;
#line 2804
        ptUIGlobals->tUIFunctionMap.Dispose();
#line 2810
        sSpriteThreadDispose();
    }
#line 2814
    InputUnregisterListener(INPUTMSG_LEFTMOUSEDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSESTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSEUP, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSEDOUBLECLICK, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MIDDLEMOUSEDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MIDDLEMOUSESTILLDOWN_NO_DELAY, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MIDDLEMOUSESTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MIDDLEMOUSEUP, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MIDDLEMOUSEDOUBLECLICK, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSEDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSEUP, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSEDOUBLECLICK, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MOUSEMOVE, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_KEYDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_KEYUP, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CHARACTER, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_KEYDOWNONCE, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_KEYSTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MOUSEWHEEL, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLER_START, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTSTICK, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTTRIGGER, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTTRIGGER, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_GESTURE_START, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONSTILLDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONHELDDOWN, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONREPEAT, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONUP, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_START_COMPOSITION, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_END_COMPOSITION, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_SHOW_COMPOSITION, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_INSERT_RESULT, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_LANGUAGE_CHANGE, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_OPEN_CANDIDATES, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_CHANGE_CANDIDATES, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_CLOSE_CANDIDATES, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_SHOWREADING, UI_Input, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_IME_HIDEREADING, UI_Input, INPUTCONTEXT_DEFAULT);
#line 2864
    UITextControlDisposeForGame();
#line 2866
    SigmaMemoryFree(ptUIGlobals, NULL); ptUIGlobals = NULL;
#line 2867
}
#line 2872
static void sHandleFocusSwitching(
    UIID & idRecipient)
{
    UIID idKeyboardFocus = UI_GetKeyboardFocus();
    BOOL fChatEditFocused = UI_TestID(idKeyboardFocus) && UI_GetControlFromID(idKeyboardFocus).IsType(UIT_CHATEDIT);

    // INFERRED: the DWARF gives bIsIMEButton the constant 0 from 2876 to the end; where it is tested is inferred
    // (it folds away). The two identical None branches below are merged by the compiler (their rows are line 0).
    BOOL bIsIMEButton = FALSE;


    if (idRecipient == GetUIIDNone())
    {
        if (!fChatEditFocused && !bIsIMEButton)
        {
            UI_SetKeyboardFocus(GetUIIDNone());
        }
        return;
    }



    UIControl & tControl = UI_GetControlFromID(idRecipient);
    if (tControl.CanAcceptKeyboardFocus())
    {
        UI_SetKeyboardFocus(idRecipient);
    }
    else if (!fChatEditFocused && !bIsIMEButton)
    {
        UI_SetKeyboardFocus(GetUIIDNone());
    }
}


#line 2938
void UI::SetMousedOverControl(const UIID & idMousedOverControlNew)
{
    if (idMousedOverControl == idMousedOverControlNew)
    {
        return;
    }

    UIID idMousedOverControlOld(idMousedOverControl);
    idMousedOverControl = idMousedOverControlNew;

    if (idMousedOverControlOld != GetUIIDNone())
    {
        UI_GetControlFromID(idMousedOverControlOld).LeftHitRegion();
    }
    if (idMousedOverControlNew != GetUIIDNone())
    {
        UI_GetControlFromID(idMousedOverControlNew).EnteredHitRegion();
    }
}
#line 3052
static void sUpdateActivePopupControlVisibility()
{
    // The original reads UIControl::m_bCachedActiveState directly (rows 3061:14, 3068:80; no IsActive inline in this
    // unit) but the member is private and the DWARF records no friend: IsActive() is the same single load.
    UIID idActivePopupControl = UI_GetActivePopupControl();
    if (idActivePopupControl != GetUIIDNone())
    {
        BOOL fCloseActivePopupControl = FALSE;
        UIControl & tControl = UI_GetControlFromID(idActivePopupControl);
        if (!tControl.IsActive())
        {
            fCloseActivePopupControl = TRUE;
        }
        else if (tControl.IsType(UIT_CONTEXTMENU))
        {
            UIContextMenu & menu = static_cast<UIContextMenu &>(tControl);
            BOOL fIsCallerMenuActive = UI_TestID(menu.GetCallerControlID()) && UI_GetControlFromID(menu.GetCallerControlID()).IsActive();
            if (menu.GetCallerControlID() == GetUIIDNone() || !fIsCallerMenuActive)
            {
                fCloseActivePopupControl = TRUE;
            }
        }
        if (fCloseActivePopupControl)
        {
            UI_CloseActivePopupControl();
        }
    }
}

#line 3126
static BOOL sLetActivePopupHandleKey(
    const UIID &,
    BufferedInput & tInputToUse)
{

    if (tInputToUse.eInputMsg == INPUTMSG_KEYDOWNONCE && tInputToUse.tBufKeyData.eKey == KEY_ESCAPE)
    {

        UI_CloseActivePopupControl();
        return TRUE;
    }
    return FALSE;
}

#line 3338
BOOL UI_Input(
    const BufferedInput & tInput)
{












    if (CGameLoadingScreenIsEnabled())
    {
        return FALSE;
    }

    if (!CGameIsValid() || !GameActiveGameCommonDataIsClient())
    {
        return FALSE;
    }




























	UI & UI = UIGet();

	if (UI.pmapElements == NULL)
	{
		return FALSE;
	}




	if (CGameGet().ptCameraManager)
	{



		const CameraState& cms = CameraManagerGetCameraState(CAMERA_CURRENT);
		if (Math::Abs(cms.flFarPlane - cms.flNearPlane) < 0.000001f)
		{
			return FALSE;
		}
	}





	sUpdateActivePopupControlVisibility();


	{
		UIID idKeyboardFocus = UI_GetKeyboardFocus();
		if (idKeyboardFocus != GetUIIDNone() &&
			!UI_GetControlFromID(idKeyboardFocus).CanAcceptKeyboardFocus())
		{
			UI_HandleKeyboardFocusLost();
		}
	}

    // m_bCachedActiveState is read directly here in the original (row 3431:59); IsActive() is the same load.
    UIID idControllerFocus = UI_GetControllerFocus();
    if (idControllerFocus != GetUIIDNone() &&
        !UI_GetControlFromID(idControllerFocus).IsActive())
    {
        UI_HandleControllerFocusLost();
    }


    UIID idRecipient = cgUIIDRoot;
	UIID idFocus = GetUIIDNone();

    BufferedInput tInputToUse = tInput;





































































































    if (InputIsKeyboard(tInputToUse))
    {












        UIID idActivePopupControl = UI_GetActivePopupControl();
        if (idActivePopupControl != GetUIIDNone() &&
            sLetActivePopupHandleKey(idActivePopupControl, tInputToUse))
        {
            return TRUE;
        }


        idFocus = UI_GetKeyboardFocus();
    }

    else if (InputIsController(tInputToUse))
    {
        // INFERRED: the threshold 5 (b.gt 5 at 3570:13 and 3576:13) is written as a literal; no named constant found.
        if (tInputToUse.eInputMsg == INPUTMSG_RIGHTTRIGGER && tInputToUse.tBufControllerData.nRightTriggerPos <= 5)
        {
            UI.bRightTriggerBounceForUI = FALSE;
            return FALSE;
        }

        if (tInputToUse.eInputMsg == INPUTMSG_LEFTTRIGGER && tInputToUse.tBufControllerData.nLeftTriggerPos <= 5)
        {
            UI.bLeftTriggerBounceForUI = FALSE;
            return FALSE;
        }












        idFocus = UI_GetControllerFocus();
    }


	if (!UI_TestID(idRecipient) ||
		idRecipient == cgUIIDMainScreen)
	{

		if (UIGame::ModalIsActive())
		{
			goto InputHandled;
		}
		return FALSE;
	}


	// INFERRED: UI_GetEditFromID (DWARF call line 3615) is called and its result unused; written as the if-body on
	// the condition's last line.

	if (idRecipient == cgUIIDRoot &&
		InputIsKeyboard(tInputToUse) &&
		!UIConsole::IsActive() &&
		UI_TestID(UITEXT_CHATENTRY_EDITLINE)) { UI_GetEditFromID(UITEXT_CHATENTRY_EDITLINE); }







	if (sgfTraceTargetControl)
	{
		UIID idKeyboardFocus = UI_GetKeyboardFocus();
		UIID idCapture = UI_GetCapture();
	}



    if (idFocus != GetUIIDNone())
    {

        if (UI_SendMessage(idFocus, UI_INPUT, (uintptr)&tInputToUse))
        {
            goto InputHandled;
        }
    }


    if (UI.fInputListenerArrayIsDirty)
    {
        UI.pInputListeners->clear();
        UI.pmapInputListeners->RemoveAll();

        UI_SendMessage(cgUIIDRoot, UI_REGISTER_INPUT_LISTENER, 0);

        UI.fInputListenerArrayIsDirty = FALSE;
    }

    if (idRecipient == cgUIIDRoot)
    {
        CControlArray& tInputListeners = *UI.pInputListeners;
        uint32 nCount = tInputListeners.size();
        for (uint32 i = 0; i < nCount; i++)
        {
            if (tInputListeners[i]->CanHandleMessage(UI_INPUT, (uintptr)&tInputToUse) && tInputListeners[i]->HandleMessage(UI_INPUT, (uintptr)&tInputToUse))
            {
                goto InputHandled;
            }
        }
    }
    else
    {
        if (UI_SendMessage(idRecipient, UI_INPUT, (uintptr)&tInputToUse))
        {
            if (tInputToUse.eInputMsg == INPUTMSG_LEFTMOUSEDOWN && UI_TestID(idRecipient))
            {
                sHandleFocusSwitching(idRecipient);
            }
            goto InputHandled;
        }
    }






    if (Console::Screens::IsScreenSetOpen() == FALSE)
    {
        return FALSE;
    }


    // INFERRED: label name. Paths from 3601, 3634, 3657, 3671 and 3680 all jump to the 3691 rows (clang 5 emits no
    // DW_TAG_label, so the name is not recorded); there is no flag local in the DWARF to carry the state instead.

InputHandled:

    if (PlayerGetByControllerIndex(tInputToUse.tBufControllerData.nControllerIndex))
    {

        CPlayerInputClearHeldButton(tInputToUse);
    }
    return TRUE;

}


#line 3704
void UI_RegisterInputListener(UIControl * pControl)
{





    CControlOnlyMap * pmapInputListeners = UIGet().pmapInputListeners;
    CControlArray * pInputListeners = UIGet().pInputListeners;

    if (pmapInputListeners->LookupPtr(pControl) == NULL)
    {
        pInputListeners->push_back(pControl);
        pmapInputListeners->SetAt_Tracked(pControl, TRUE);
    }
}

#line 3723
void UI_FlagInputListenerArrayDirty()
{
    UIGet().fInputListenerArrayIsDirty = TRUE;
}

#line 3733
void UI_Update(const float flDeltaTime)
{







    if (!InputIsCursorInWindow() && !InputAppHasCapture())
    {
        UIGet().SetMousedOverControl(GetUIIDNone());
    }

    Console::WebBrowser::Update();
    Console::Screens::Update();
    Console::Touch::Update();

    UI_SendMessage(cgUIIDRoot, UI_UPDATE, 0);
    UIGame::Update(flDeltaTime);
    gtTutorialMgr.Update();

    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() == GAMEMODE_PLAYING && !GameIsStartUp())
    {
        UIConversation::Update();
        UIQuests::Update();
    }
}

#line 3860
void UI_Print(LPCSTR szText, const BOOL fEchoToConsole)
{
    if (!CGameIsValid())
    {
        return;
    }

    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (fEchoToConsole && ptUIGlobals)
    {
        UIConsole::OutputString(szText);
    }
}

#line 3897
void UI_PreDraw()
{
    Blizzard::Lock::EventSet(stSpriteThreadData.tWorkPendingEvent);
}

#line 3908
uint32 s_controlDrawCount;

#line 3913
void UI_Draw()
{
    s_controlDrawCount = 0;







    stSpriteThreadData.tWorkCompleteEvent.WaitForever();



    stSpriteThreadData.pSpriteThreadBuffer->Flush();
}

#line 3936
UI & UIGet()
{
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    return ptUIGlobals->ui;
}

#line 3953
LoadscreenTipList GetTipListForGlobalSNO(GlobalSNO snoStringList, int32 & nTotalStrings)
{

    LoadscreenTipList tTipList;
    SNO snoTipList = GlobalSNOGet(snoStringList);
    ScopedStringListDefinitionLockR pdefStringList(snoTipList);
    tTipList.snoList = snoTipList;
    tTipList.nNumStrings = pdefStringList->dwStringCount;
    nTotalStrings += tTipList.nNumStrings;

    return tTipList;
}
#line 3966
void UI_DisplayLoadingScreenTooltip(BOOL bChangeTip)

{
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    LoadingScreenData & tLSD = tCGameGlobals.tLoadingScreenData;


    s_ptUILoadingScreenData->m_ptText->SetControllerIndexOverride(Console::GamerProfile::GetPrimaryProfileUserIndex());



    if (tLSD.bTipOverridden)
    {
        s_ptUILoadingScreenData->m_ptText->SetText(tLSD.sOverrideTip, FALSE);
        return;
    }


    if (tLSD.bHostMigrationTips)
    {

        if (tLSD.bHostMigrationDisplayFirstTip)
        {

            if (Console::Online::PartyGetOnlineState() == 1)
            {
                s_ptUILoadingScreenData->m_ptText->SetText(
                    StringListFetch("ConsoleErrors:GameMigrationLoadScreenFromAdHoc"), FALSE);
            }
            else
            {

                s_ptUILoadingScreenData->m_ptText->SetText(
                    StringListFetch("ConsoleErrors:GameMigrationLoadScreen"), FALSE);
            }
        }
        else
        {
            s_ptUILoadingScreenData->m_ptText->SetText(
                StringListFetch("ConsoleErrors:GameMigrationLoadScreen2"), FALSE);
        }
        tLSD.bHostMigrationDisplayFirstTip = !tLSD.bHostMigrationDisplayFirstTip;
        return;
    }

#line 4013
    blz::vector<LoadscreenTipList, blz::allocator<LoadscreenTipList> > arTipLists;
    int32 nTotalStrings = 0;


    if (GameTypeIsPvP(CGameGet().tGameStartParams.eGameType))
    {
        arTipLists.push_back(GetTipListForGlobalSNO(GLOBALSNO_STRINGLIST_LOADSCREEN_TIPS_PVP, nTotalStrings));
    }
    else
    {
#line 4074
        int32 nGameLevel = GameGetActiveGameCommonData().tGameSyncedData.nMonsterLevel;
        if (nGameLevel <= 0)
        {





            static int32 nMaxLocalHeroLevel = 0;
            if (tLSD.nMaxLocalHeroLevel > 0)
            {
                nMaxLocalHeroLevel = tLSD.nMaxLocalHeroLevel;
            }
            nGameLevel = nMaxLocalHeroLevel;
        }
        BOOL bGameIsMaxLevel = nGameLevel >= 70;

        arTipLists.push_back(GetTipListForGlobalSNO(GLOBALSNO_STRINGLIST_LOADSCREEN_TIPS_CONSOLE_ALWAYS, nTotalStrings));
        if (bGameIsMaxLevel)
        {
            arTipLists.push_back(GetTipListForGlobalSNO(GLOBALSNO_STRINGLIST_LOADSCREEN_TIPS_CONSOLE_MAX_LEVEL, nTotalStrings));
        }
        else
        {
            arTipLists.push_back(GetTipListForGlobalSNO(GLOBALSNO_STRINGLIST_LOADSCREEN_TIPS_CONSOLE_LOW_LEVEL, nTotalStrings));
        }
    }

#line 4108
    uint32 nTipNumber = tLSD.nCurrentTip;
    if (bChangeTip && nTotalStrings > 0)
    {
        AppGlobals & tAppGlobals = sgtAppGlobals;
        BOOL bRandomizeTips = !(tAppGlobals.dwDebugFlags2 & (1 << 30));

        if (bRandomizeTips)
        {


            RNG tRNG;
            RandInit(tRNG);

            nTipNumber = RandGetDWord(tRNG) % nTotalStrings;


            if (nTotalStrings != 1 && nTipNumber == tLSD.nCurrentTip)
            {
                nTipNumber = (nTipNumber + 1) % nTotalStrings;
            }
        }
        else { nTipNumber = (nTipNumber + 1) % nTotalStrings; }
        tLSD.nCurrentTip = nTipNumber;
    }

#line 4131
    uint32 nListNumber = 0;
    BOOL bTipDisplayed = FALSE;
    for (; nListNumber < arTipLists.size(); nListNumber++)
    {
        uint32 nNumStringsInCurrentList = arTipLists[nListNumber].nNumStrings;
        if (nTipNumber < nNumStringsInCurrentList)
        {

            {
                ScopedStringListDefinitionLockR pdefStringList(arTipLists[nListNumber].snoList);
                s_ptUILoadingScreenData->m_ptText->SetText(pdefStringList->arStrings[nTipNumber].szText.Get(), FALSE);
                bTipDisplayed = TRUE;
            }
            break;
        }
        nTipNumber -= nNumStringsInCurrentList;
    }

#line 4151
    if (!bTipDisplayed)
    {
        s_ptUILoadingScreenData->m_ptText->SetText("", FALSE);
    }
}

#line 4337
void UI_EndDrawLoadingScreen()
{

    ScopedTextureDefinitionLockR pdefTextureLeft(s_ptUILoadingScreenData->m_tLoadingScreenLeftCap.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureLeft, FALSE);
    ScopedTextureDefinitionLockR pdefTextureRight(s_ptUILoadingScreenData->m_tLoadingScreenRightCap.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureRight, FALSE);
    ScopedTextureDefinitionLockR pdefTextureMain(s_ptUILoadingScreenData->m_tLoadingScreenMain.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureMain, FALSE);
    ScopedTextureDefinitionLockR pdefTextureDiabloIcon(s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureDiabloIcon, FALSE);
    ScopedTextureDefinitionLockR pdefTextureStatus(s_ptUILoadingScreenData->m_tLoadingScreenStatusIcon.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureStatus, FALSE);
    ScopedTextureDefinitionLockR pdefTextureHourGlass(s_ptUILoadingScreenData->m_tLoadingScreenHourGlass.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureHourGlass, FALSE);

    UI_DeleteControl(s_ptUILoadingScreenData->m_ptText->GetID(), TRUE);
    ::delete s_ptUILoadingScreenData->m_ptLoadingScreenIconFade; s_ptUILoadingScreenData->m_ptLoadingScreenIconFade = NULL;
}

#line 4612
BOOL UI_SendMessage(const UIID & idRecipient, const UIMsg & msg, const uintptr dwParam)
{









































    if (!UI_TestID(idRecipient))
    {
        return FALSE;
    }
    UIControl & tRecipient = UI_GetControlFromID(idRecipient);















































    if (UI_TestID(tRecipient.GetParent()))
    {
        UI_GetControlFromID(tRecipient.GetParent()).PeekChildMessage(msg, dwParam);
    }

    if (tRecipient.CanHandleMessage(msg, dwParam))
    {
        return tRecipient.HandleMessage(msg, dwParam);
    }
    return FALSE;
}

#line 4723
BOOL UI_GetControlData(const UIControlSourceKey & tSrcKey, const UIDialogDefinition * pdefUI, UIControlData *& pControlData)
{
    BOOL bRetval = FALSE;



    if (tSrcKey.snoUI != SNO_NONE)
    {




        if (pdefUI->pmapControlData)
        {
            bRetval = pdefUI->pmapControlData->Lookup(tSrcKey.nKey, pControlData);
        }
    }



    return bRetval;
}

#line 4745
UIStyle * UI_GetStyleDataClone(LPCSTR szStyleSrc, SNO snoImpliedFile, const UIStyleMap * pmapStylesFromImpliedSno)
{


    if (!szStyleSrc || !szStyleSrc[0])
    {
        return NULL;
    }

    UIControlSourceKey tSrcKey = UIControlSourceKey::Parse(szStyleSrc, snoImpliedFile, NULL, NULL);

    if (tSrcKey.snoUI == SNO_NONE)
    {
        return NULL;
    }

#line 4763
    UIStyle * pStyle = NULL;

    if (pmapStylesFromImpliedSno && tSrcKey.snoUI == snoImpliedFile)
    {
        if (pmapStylesFromImpliedSno->Lookup(tSrcKey.nKey, pStyle))
        {
            pStyle = pStyle->Clone();
        }
    }
    else
    {
#line 4774
        ScopedUIDialogDefinitionLockR pdefUI(tSrcKey.snoUI);
        if (pdefUI->pmapStyles && pdefUI->pmapStyles->Lookup(tSrcKey.nKey, pStyle))
        {
            pStyle = pStyle->Clone();
        }
    }

#line 4786
    return pStyle;
}

#line 4791
UIID UI_DeriveFullUIID(const UIID & idControl, LPCSTR szIncompletePath, BOOL bUseCaretForTemplateInstances)




{
    LPCSTR szRootPrefix = "Root.";
    UIID idFinal;
    if (szIncompletePath[0] == '.')
    {

        idFinal.SetFromParent(idControl, szIncompletePath + 1);
    }
    else if (szIncompletePath[0] == '^') {
#line 4805
        if (bUseCaretForTemplateInstances)
        {

            UIID idCurrent(idControl);
            UIID idParent(UI_GetControlFromID(idControl).GetParent());
            while (UI_TestID(idParent) &&
                   idParent != cgUIIDNormalLayer &&
                   idParent != cgUIIDTopLayer &&
                   idParent != cgUIIDWindowLayer &&
                   idParent != cgUIIDRoot &&
                   !UI_GetControlFromID(idParent).m_bIsInstantiatedTemplate)
            {
                idCurrent = idParent;
                idParent = UI_GetControlFromID(idCurrent).GetParent();
            }

            if (UI_TestID(idParent) &&
                UI_GetControlFromID(idParent).m_bIsInstantiatedTemplate)
            {


                LPCSTR uszPeriod = strchr(szIncompletePath, '.');
                if (uszPeriod)
                {
                    idFinal.SetFromParent(idParent, uszPeriod + 1); return idFinal;
                }
            }
            idFinal.SetFromParent(idParent, szIncompletePath + 1);
        }
        else
        {
#line 4844
            UIID idCurrent(idControl);
            UIID idParent(UI_GetControlFromID(idControl).GetParent());
            while (UI_TestID(idParent) &&
                   idParent != cgUIIDNormalLayer &&
                   idParent != cgUIIDTopLayer &&
                   idParent != cgUIIDWindowLayer &&
                   idParent != cgUIIDRoot)
            {
                idCurrent = idParent;
                idParent = UI_GetControlFromID(idCurrent).GetParent();
            }
            idFinal.SetFromParent(idParent, szIncompletePath + 1);
        }
    }
    else {
#line 4859
        if (strncmp(szIncompletePath, szRootPrefix, strlen(szRootPrefix)) == 0)
        {

            idFinal.FromLabel(szIncompletePath);
        }
        else
        {


            idFinal.SetFromParent(UI_GetControlFromID(idControl).GetParent(), szIncompletePath);
        }
    }

    return idFinal;
}

#line 4877
UIID UIIDCreateFromControlAttribute(const UIControlSourceKey & tSrcKey, const UIID & id, const UIAttributeType eAttributeType)



{
    if (tSrcKey.snoUI == SNO_NONE)
    {
        return GetUIIDNone();
    }

    ScopedUIDialogDefinitionLockR pdefUI(tSrcKey.snoUI);
    UIControlData * pControlData;
    BOOL fSuccess = UI_GetControlData(tSrcKey, pdefUI, pControlData);
    if (!fSuccess)
    {
        return GetUIIDNone();
    }

    if (!pControlData->AttributeExists(eAttributeType))
    {
        return GetUIIDNone();
    }

#line 4900
    LPCSTR pszLabel = pControlData->GetAttributeValue(eAttributeType);
    UIID idWithinTemplate = UI_DeriveFullUIID(id, pszLabel, TRUE);
    if (UI_TestID(idWithinTemplate))
    {
        return idWithinTemplate;
    }

#line 4908
    return UI_DeriveFullUIID(id, pszLabel, FALSE);
}

#line 4948
int32 UI_GetXMLInt32Constant(LPCSTR szConstantPath, int32 nDefaultValue)
{
    if (szConstantPath && szConstantPath[0])
    {
        UIControlSourceKey tSrcKey = UIControlSourceKey::Parse(szConstantPath, SNO_NONE, NULL, NULL);
        return UI_GetXMLInt32Constant(tSrcKey, nDefaultValue);
    }
    return nDefaultValue;
}

#line 4963
inline int32 UI_GetXMLInt32Constant(const UIControlSourceKey & tSrcKey, int32 nDefaultValue)
{
    int32 nValue = nDefaultValue;
    if (tSrcKey.snoUI != SNO_NONE)
    {
        ScopedUIDialogDefinitionLockR pdefUI(tSrcKey.snoUI);
        if (pdefUI->tConstantsMaps.pmapInt32s)
        {
            pdefUI->tConstantsMaps.pmapInt32s->Lookup(tSrcKey.nKey, nValue);
        }
    }
    return nValue;
}

#line 4982
uint32 UI_GetXMLUint32Constant(LPCSTR szConstantPath, uint32 nDefaultValue)
{
    int32 nValue = UI_GetXMLInt32Constant(szConstantPath, nDefaultValue);
    return nValue;
}

#line 4992
uint32 UI_GetXMLUint32Constant(const UIControlSourceKey & tSrcKey, uint32 nDefaultValue)
{
    int32 nValue = UI_GetXMLInt32Constant(tSrcKey, nDefaultValue);
    return nValue;
}

#line 5018
float UI_GetXMLFloat32Constant(const UIControlSourceKey & tSrcKey, float flDefaultValue)
{
    float flValue = flDefaultValue;
    if (tSrcKey.snoUI != SNO_NONE)
    {
        ScopedUIDialogDefinitionLockR pdefUI(tSrcKey.snoUI);
        BOOL bFound = FALSE;
        if (pdefUI->tConstantsMaps.pmapFloat32s)
        {
            bFound = pdefUI->tConstantsMaps.pmapFloat32s->Lookup(tSrcKey.nKey, flValue);
        }
    }
    return flValue;
}

#line 5044
BOOL UI_TestID(const UIID & id)
{
    if (id == GetUIIDNone() || !CGameIsValid())
    {
        return FALSE;
    }

    UIGlobals * ptUIGlobals = sgtAppGlobals.ptCGameActive->ptUIGlobals;
    if (!ptUIGlobals)
    {
        return FALSE;
    }

    UI & UI = ptUIGlobals->ui;
    UIControl * pControl = NULL;
    UI.pmapElements->Lookup(id, pControl);
    return pControl != NULL;
}

#line 5077
static UIControl * sLookUpControlByID(const UIID & id)
{
    UI & UI = UIGet();
    UIControl * pControl = NULL;
    UI.pmapElements->Lookup(id, pControl);
    return pControl;
}

// LINKER-REMOVED: DWARF UI.cpp:5247 (concrete DIE low_pc 0, 0x288 bytes) with params/locals on 5248-5263 and its
// line rows: arMatches 5250, UI_TestID inlined at 5251 (row 5251:5, the tested id and the early exit are inferred),
// strlen 5256:27, tRootControl 5257 (UI_GetControlGroupFromID inlined), arChildren copy 5258, loop 5259:5 on size(),
// front() 5261, GetIDAsString 5262:41, strstr 5263:31/49, test 5265:13/26/29 (`strstrResult[dwNameLength]` inferred
// from the three-instruction row), out-of-line push_back 5267:23, IsType test 5269, vector_append 5271:13,
// vector_erase_reorder 5273. Needed because it is the second caller of blz::vector<UIID>::push_back in this CU, which
// decides how push_back is inlined into sGetFocusableControls.
#line 5247
UIIDArray UI_FindControl(
    LPCSTR szControlName)
{
    UIIDArray arMatches;
    if (!UI_TestID(cgUIIDRoot))
    {
        return arMatches;
    }

    uint32 dwNameLength = strlen(szControlName);
    UIControlGroup & tRootControl = UI_GetControlGroupFromID(cgUIIDRoot);
    UIControlItemArray arChildren = tRootControl.GetChildList();
    while (arChildren.size() > 0)
    {
        const UIID & idChild = arChildren.front().id;
        CRefString sChildName = idChild.GetIDAsString();
        LPCSTR strstrResult = strstr(sChildName.str(), szControlName);

        if (strstrResult && strstrResult[dwNameLength] == '\0')
        {
            arMatches.push_back(idChild);
        }
        if (UI_GetControlFromID(idChild).IsType(UIT_CONTROLGROUP))
        {
            blzext::vector_append(arChildren, UI_GetControlGroupFromID(idChild).GetChildList());
        }
        blzext::vector_erase_reorder(arChildren, 0);
    }

    return arMatches;
}

#line 5283
template <class T, class TCreationParams> T & sGetControlFromID(const UIID & id, const UIType eType)
{

    UIControl * pControl = sLookUpControlByID(id);

    if (!pControl)
    {
        CRefString sError = CRefString().Format("UI ERROR: Failed to find UIID %s\n", id.GetIDAsString().str());












        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    return *(T *)pControl;
}

#line 5314
UIControl & UI_GetControlFromID(const UIID & id)
{
    return sGetControlFromID<UIControl, ControlCreationParams>(id, UIT_CONTROL);
}

UIControlGroup & UI_GetControlGroupFromID(const UIID & id)
{
    return sGetControlFromID<UIControlGroup, ControlCreationParams>(id, UIT_CONTROLGROUP);
}

UIHotkey & UI_GetHotkeyFromID(const UIID & id)
{
    return sGetControlFromID<UIHotkey, HotkeyCreationParams>(id, UIT_HOTKEY);
}

UIWindow & UI_GetWindowFromID(const UIID & id)
{
    return sGetControlFromID<UIWindow, WindowCreationParams>(id, UIT_WINDOW);
}

UIText & UI_GetTextFromID(const UIID & id)
{
    return sGetControlFromID<UIText, TextCreationParams>(id, UIT_TEXT);
}

UIButton & UI_GetButtonFromID(const UIID & id)
{
    return sGetControlFromID<UIButton, ButtonCreationParams>(id, UIT_BUTTON);
}

UIItemTag & UI_GetItemTagFromID(const UIID & id)
{
    return sGetControlFromID<UIItemTag, ButtonCreationParams>(id, UIT_ITEMTAG);
}

UIActorTag & UI_GetActorTagFromID(const UIID & id)
{
    return sGetControlFromID<UIActorTag, TextCreationParams>(id, UIT_ACTORTAG);
}

UIItemButton & UI_GetItemButtonFromID(const UIID & id)
{
    return sGetControlFromID<UIItemButton, ItemButtonCreationParams>(id, UIT_ITEMBUTTON);
}

UIThumbButton & UI_GetThumbButtonFromID(const UIID & id)
{
    return sGetControlFromID<UIThumbButton, ButtonCreationParams>(id, UIT_THUMBBUTTON);
}

UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id)
{
    return sGetControlFromID<UIHotbarButton, HotbarButtonCreationParams>(id, UIT_HOTBARBUTTON);
}

UIAutoScrollingText & UI_GetAutoScrollingTextFromID(const UIID & id)
{
    return sGetControlFromID<UIAutoScrollingText, AutoScrollingTextCreationParams>(id, UIT_AUTOSCROLLINGTEXT);
}

UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id)
{
    return sGetControlFromID<UIConsoleScrollBox, ConsoleScrollBoxCreationParams>(id, UIT_CONSOLESCROLLBOX);
}

UIConsoleScrollIndicator & UI_GetConsoleScrollIndicatorFromID(const UIID & id)
{
    return sGetControlFromID<UIConsoleScrollIndicator, ConsoleScrollIndicatorCreationParams>(id, UIT_CONSOLESCROLLINDICATOR);
}

UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id)
{
    return sGetControlFromID<UIConsoleListBox, ConsoleListBoxCreationParams>(id, UIT_CONSOLELISTBOX);
}

UIConsoleBuffIndicator & UI_GetConsoleBuffIndicatorFromID(const UIID & id)
{
    return sGetControlFromID<UIConsoleBuffIndicator, ConsoleBuffIndicatorCreationParams>(id, UIT_CONSOLEBUFFINDICATOR);
}

UIConsoleSelector & UI_GetConsoleSelectorFromID(const UIID & id)
{
    return sGetControlFromID<UIConsoleSelector, ConsoleSelectorCreationParams>(id, UIT_CONSOLESELECTOR);
}

UIConsoleSkillsTimer & UI_GetConsoleSkillsTimerFromID(const UIID & id)
{
    return sGetControlFromID<UIConsoleSkillsTimer, TimerCreationParams>(id, UIT_TIMER);
}

UICheckBox & UI_GetCheckBoxFromID(const UIID & id)
{
    return sGetControlFromID<UICheckBox, CheckBoxCreationParams>(id, UIT_CHECKBOX);
}

UIToolTip & UI_GetToolTipFromID(const UIID & id)
{
    return sGetControlFromID<UIToolTip, WindowCreationParams>(id, UIT_TOOLTIP);
}

UITutorial & UI_GetTutorialFromID(const UIID & id)
{
    return sGetControlFromID<UITutorial, WindowCreationParams>(id, UIT_TUTORIAL);
}

UIEdit & UI_GetEditFromID(const UIID & id)
{
    return sGetControlFromID<UIEdit, EditBoxCreationParams>(id, UIT_EDIT);
}

UIScrollableText & UI_GetScrollableTextFromID(const UIID & id)
{
    return sGetControlFromID<UIScrollableText, ScrollableTextCreationParams>(id, UIT_SCROLLABLETEXT);
}

UIScrollbar & UI_GetScrollbarFromID(const UIID & id)
{
    return sGetControlFromID<UIScrollbar, ScrollbarCreationParams>(id, UIT_SCROLLBAR);
}

UIProgressBar & UI_GetProgressBarFromID(const UIID & id)
{
    return sGetControlFromID<UIProgressBar, ProgressBarCreationParams>(id, UIT_PROGRESSBAR);
}

UIRadioButton & UI_GetRadioButtonFromID(const UIID & id)
{
    return sGetControlFromID<UIRadioButton, RadioButtonCreationParams>(id, UIT_RADIOBUTTON);
}

UIRadioButtonGroup & UI_GetRadioButtonGroupFromID(const UIID & id)
{
    return sGetControlFromID<UIRadioButtonGroup, RadioButtonGroupCreationParams>(id, UIT_RADIOBUTTONGROUP);
}

UIFloatBubble & UI_GetFloatBubbleFromID(const UIID & id)
{
    return sGetControlFromID<UIFloatBubble, TextCreationParams>(id, UIT_FLOATBUBBLE);
}

UITimer & UI_GetTimerFromID(const UIID & id)
{
    return sGetControlFromID<UITimer, TimerCreationParams>(id, UIT_TIMER);
}

UIBlinker & UI_GetBlinkerFromID(const UIID & id)
{
    return sGetControlFromID<UIBlinker, BlinkerCreationParams>(id, UIT_BLINKER);
}

UIMinimap & UI_GetMinimapFromID(const UIID & id)
{
    return sGetControlFromID<UIMinimap, WindowCreationParams>(id, UIT_MINIMAP);
}

UIRActor & UI_GetRActorFromID(const UIID & id)
{
    return sGetControlFromID<UIRActor, RActorCreationParams>(id, UIT_RACTOR);
}

UICutscene & UI_GetCutsceneFromID(const UIID & id)
{
    return sGetControlFromID<UICutscene, WindowCreationParams>(id, UIT_CUTSCENE);
}

UIStackPanel & UI_GetStackPanelFromID(const UIID & id)
{
    return sGetControlFromID<UIStackPanel, StackPanelCreationParams>(id, UIT_STACKPANEL);
}

UIWrapPanel & UI_GetWrapPanelFromID(const UIID & id)
{
    return sGetControlFromID<UIWrapPanel, WrapPanelCreationParams>(id, UIT_WRAPPANEL);
}

UIStoryboard & UI_GetStoryboardFromID(const UIID & id)
{
    return sGetControlFromID<UIStoryboard, StoryboardCreationParams>(id, UIT_STORYBOARD);
}

UITreeFolder & UI_GetTreeFolderFromID(const UIID & id)
{
    return sGetControlFromID<UITreeFolder, TreeFolderCreationParams>(id, UIT_TREEFOLDER);
}

#line 5932
void UI_AddControl(const UIID & id, UIControl * pControl)
{







    UI & UI = UIGet();









    UIControl * pExistingControl = NULL;
    BOOL bExists = UI.pmapElements->Lookup(id, pExistingControl);

    if (bExists)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    UI.pmapElements->SetAt_Tracked(id, pControl);
}

#line 6019
static UIControlData * sUIControl_FindChild(const UIControlDataArray * pChildren, LPCSTR szName, size_t * nNameLength)
{


#line 6024
    for (uint32 i = 0; i < pChildren->size(); i++)
    {
        UIControlData * pChild = (*pChildren)[i];

        if (nNameLength)
        {
            if (strncmp(szName, "*", *nNameLength) == 0 ||
                (strncmp(szName, pChild->szName, *nNameLength) == 0 && strlen(pChild->szName) == *nNameLength))
            {
                return pChild;
            }
        }
        else
        {
#line 6037
            if (strcmp(szName, "*") == 0 ||
                strcmp(szName, pChild->szName) == 0)
            {
                return pChild;
            }
        }
    }

    return NULL;
}

#line 6050
static UIControlData * sUIControl_FindDescendant(LPCSTR szPathToChild, const UIControlDataArray * pChildren, BOOL fDirectChildOnly)



{
    if (!szPathToChild ||
        !pChildren ||
        !szPathToChild[0])
    {
        return NULL;
    }

#line 6062
    const char * dot = strchr(szPathToChild, '.');

    if (dot)
    {
        size_t nNameLength = dot - szPathToChild;
        UIControlData * pChild = sUIControl_FindChild(pChildren, szPathToChild, &nNameLength);
        if (pChild)
        {
            return sUIControl_FindDescendant(dot + 1, pChild->plistChildren, TRUE);
        }
    }

#line 6074
    UIControlData * pDescendant = sUIControl_FindChild(pChildren, szPathToChild, NULL);

    if (pDescendant)
    {
        return pDescendant;
    }

    if (!fDirectChildOnly)
    {
#line 6085
        for (uint32 i = 0; i < pChildren->size(); i++)
        {
            UIControlData * pChild = (*pChildren)[i];
            pDescendant = sUIControl_FindDescendant(szPathToChild, pChild->plistChildren, FALSE);
            if (pDescendant)
            {
                return pDescendant;
            }
        }
    }

    return NULL;
}

#line 6101
void UI_SetupStyles(UIControlData & pControl, const UIControlDataArray * pChildren, SNO snoImpliedFile, const UIStyleMap * pmapStylesFromImpliedSno)




{
    if (pControl.AttributeExists(ATT_XML_STYLE))
    {
        LPCSTR szStyle = pControl.GetAttributeValue(ATT_XML_STYLE);
        if (szStyle && szStyle[0])
        {
            UIStyle * pStyle = UI_GetStyleDataClone(szStyle, snoImpliedFile, pmapStylesFromImpliedSno);
            if (pStyle)
            {
                if (pControl.pBaseStyle)
                {

                    UIStyle::DeleteClone(pControl.pBaseStyle, FALSE);
                    SigmaMemoryFree(pControl.pBaseStyle, NULL); pControl.pBaseStyle = NULL;
                }
                pControl.pBaseStyle = pStyle;

#line 6124
                const UIStyle * pCurStyle = pStyle;
                while (pCurStyle)
                {
#line 6130
                    for (int32 i = (int32)pCurStyle->plistChildSpecs->size() - 1; i >= 0; i--)
                    {

                        UIStyleChildSpec * pSpec = (*pCurStyle->plistChildSpecs)[i];
                        LPCSTR szPathToChild = pSpec->szPathToChild;
                        BOOL fDirectChildOnly = TRUE;
                        if (strncmp(szPathToChild, "//", 2) == 0)
                        {
                            szPathToChild += 2;
                            fDirectChildOnly = FALSE;
                        }
                        UIControlData * pDescendant = sUIControl_FindDescendant(szPathToChild, pChildren, fDirectChildOnly);
                        if (pDescendant)
                        {
                            if (!pDescendant->plistStylesFromAncestors)
                            {
                                pDescendant->plistStylesFromAncestors = new blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> >();
                            }
                            pDescendant->plistStylesFromAncestors->insert(pDescendant->plistStylesFromAncestors->begin(), pSpec->Clone());
                        }
                    }
                    pCurStyle = pCurStyle->pBaseStyle;
                }
            }
        }
    }

#line 6157
    if (pControl.tInlineStyle.plistChildSpecs)
    {
        for (uint32 i = 0; i < pControl.tInlineStyle.plistChildSpecs->size(); i++)
        {

            UIStyleChildSpec * pSpec = (*pControl.tInlineStyle.plistChildSpecs)[i];
            LPCSTR szPathToChild = pSpec->szPathToChild;
            BOOL fDirectChildOnly = TRUE;
            if (strncmp(szPathToChild, "//", 2) == 0)
            {
                szPathToChild += 2;
                fDirectChildOnly = FALSE;
            }
            UIControlData * pDescendant = sUIControl_FindDescendant(szPathToChild, pChildren, fDirectChildOnly);
            if (pDescendant)
            {
                if (!pDescendant->plistStylesFromAncestors)
                {
                    pDescendant->plistStylesFromAncestors = new blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> >();
                }
                pDescendant->plistStylesFromAncestors->push_back(pSpec->Clone());
            }
        }
    }

#line 6182
    if (pChildren)
    {

        for (uint32 i = 0; i < pChildren->size(); i++)
        {
            UIControlData * pChild = (*pChildren)[i];
            UI_SetupStyles(*pChild, pChild->plistChildren, snoImpliedFile, pmapStylesFromImpliedSno);
        }
    }
}

#line 6196
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot)
{
    UIID idControlCopy(idControl);
    UI_DeleteControl(idControlCopy, fAssertIfDeletingRoot);
}

#line 6205
void UI_DeleteControl(
    UIID & idControl,
    const BOOL fAssertIfDeletingRoot)
{
    UIGlobals * ptUIGlobals;






    if (idControl == cgUIIDRoot && fAssertIfDeletingRoot)
    {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }









    if (CGameIsValid())
    {





        ptUIGlobals = CGameGet().ptUIGlobals;


        if (ptUIGlobals)
        {
            UI & UI = ptUIGlobals->ui;
            UIControl * pControl = NULL;
            if (UI.pmapElements->Lookup(idControl.nIDHash, pControl))
            {


                const UIID & idParent = UI_GetControlFromID(idControl).GetParent();

                if (UI_TestID(idParent))
                {

                    if (UI_GetControlFromID(idParent).IsType(UIT_CONTROLGROUP))
                    {
                        UI_GetControlGroupFromID(idParent).RemoveChild(idControl);
                    }
                }

                if (pControl->IsType(UIT_RADIOBUTTON))
                {

                    UIID idRadioButtonGroup = ((UIRadioButton *)pControl)->GetRadioButtonGroup();
                    if (UI_TestID(idRadioButtonGroup))
                    {

                        UIRadioButtonGroup & tGroup = UI_GetRadioButtonGroupFromID(idRadioButtonGroup);
                        tGroup.RemoveChild(idControl);
                    }
                }



                UI.pmapElements->RemoveAt(idControl);
                ::delete pControl;


                if (UI.idMousedOverControl == idControl)
                {

                    UI.idMousedOverControl = GetUIIDNone();
                }
            }
        }
    }

    idControl = GetUIIDNone();
}

#line 6282
InvGlobals * UI_GetInventoryData()
{
    if (CGameGet().ptUIGlobals)
    {
        return CGameGet().ptUIGlobals->ptInventory;
    }
    return NULL;
}

#line 6296
void UI_ResetForPlayerMovement(Player * ptPlayer)
{
#line 6309
    UIConfirmation::PlayerMoved(ptPlayer->nControllerIndex);
#line 6316
    UICountDown::PlayerMoved();

    PlayerGetCursorMgr().ResetCursorState(FALSE);

    ptPlayer->tClientInfo.nKillsSinceMoving = 0;
#line 6323
    if (ptPlayer->tTargetComponent.GetLockedTargetType() == ConsoleTargetComponent::TARGETLOCK_PERSISTENT || ptPlayer->tTargetComponent.GetLockedTargetType() == ConsoleTargetComponent::TARGETLOCK_STICKY)
    {

        ptPlayer->tTargetComponent.ClearLockedTarget();
    }
}

#line 6333
void UI_SetIsResetting(BOOL isResettingUI)
{
}

#line 6343
BOOL UI_IsResetting()
{
    return FALSE;
}

#line 6354
SNO UI_GetDefaultHeroLight()
{
    return GlobalSNOGet(CWorldIsRetro() ? GLOBALSNO_LIGHT_3DCHARACTER_RETRO : GLOBALSNO_LIGHT_3DCHARACTER);
}

#line 6362
void UI_Reset(bool bExpectUIRestore)
{
#line 6373
    UI_ClearDialogStack(UI_GetInGameDlgStack());

    if (bExpectUIRestore)
    {
        HubGet().ScreenManagerGet().HideCurrentOverlay();
    }
    else
    {

        HubGet().ScreenManagerGet().CloseOverlay();
    }
#line 6387
    if (GameIsStartUp())
    {
        return;
    }
#line 6392
    UIQuests::ResetUI();
    UISideScreenCloseAll(0xffffffff);
#line 6397
    PlayerGetCursorMgr().ResetCursorState(FALSE);
#line 6401
    Console::Screens::CloseScreenSet(1, FALSE);
}
// LINKER-REMOVED: DWARF UI.cpp:6407, concrete DIE at low_pc 0 with size 0x18; its rows (6409, 6410:27 load, 6410:24
// store, 6411) fit one negating store. It is the only writer of sgfTraceTargetControl, whose load UI_Input keeps
// (3623); without a writer the compiler folds the flag to 0. The negation is inferred from the size and columns.
#line 6407
void UI_ToggleTraceTargetControl()

{
	sgfTraceTargetControl = !sgfTraceTargetControl;
}


#line 6417
void UI_GfxInit()
{
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (ptUIGlobals)
    {
        UI_SendMessage(cgUIIDRoot, UI_GFX_INIT, 0);
    }
}

#line 6436
void UI_GfxDispose()
{
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (ptUIGlobals)
    {
        UI_SendMessage(cgUIIDRoot, UI_GFX_DISPOSE, 0);
    }
}

#line 6475
void UI_InitializeForWorld()
{
    UI_SendMessage(cgUIIDRoot, UI_INIT_FOR_WORLD, 0);
    Console::Screens::InitializeForWorld();
}

#line 6490
void UI_DisposeForWorld()
{
    UI_SendMessage(cgUIIDRoot, UI_DISPOSE_FOR_WORLD, 0);
    Console::Screens::DisposeForWorld();
}

#line 6507
UIFunctionMap & UI_GetUIFunctionMap()
{
    return CGameGet().ptUIGlobals->tUIFunctionMap;
}

#line 6519
BOOL UI_DrawingFullscreenUI()
{
    if (CutsceneGetFullScreenCutscene() != CUTSCENEID_INVALID || UICredits::IsActive() || UIVictory::IsActive())
    {
        return TRUE;
    }
    return FALSE;
}

#line 6775
static void sSpriteThreadInitialize()
{

    Blizzard::Lock::EventCreate(stSpriteThreadData.tWorkPendingEvent);
    Blizzard::Lock::EventCreate(stSpriteThreadData.tWorkCompleteEvent);
    stSpriteThreadData.pSpriteThreadBuffer = new XCommandBuffer("SpriteThreadBuffer", 0x40000);
    stSpriteThreadData.pDataIDMemoryPool = new (SigmaMemoryNew(sizeof(DataIDMemoryPool), 4, NULL, FALSE)) DataIDMemoryPool;
    stSpriteThreadData.bActive = true;

    Blizzard::Thread::Create(sSpriteThreadProc, &stSpriteThreadData, &stSpriteThreadData.tThread, "UISpriteThread", 0, 0x10000);

    ConfigureUISpriteThread(stSpriteThreadData.tThread);
    Blizzard::Thread::Resume(stSpriteThreadData.tThread);
}


#line 6790
static void sSpriteThreadDispose()
{
#line 6793
    stSpriteThreadData.bActive = false;
    stSpriteThreadData.tWorkPendingEvent.Set();
#line 6796
    Blizzard::Thread::WaitForever(stSpriteThreadData.tThread);
    Blizzard::Thread::Close(stSpriteThreadData.tThread);
#line 6799
    Blizzard::Lock::EventDestroy(stSpriteThreadData.tWorkPendingEvent);
    Blizzard::Lock::EventDestroy(stSpriteThreadData.tWorkCompleteEvent);
    delete stSpriteThreadData.pSpriteThreadBuffer;
    stSpriteThreadData.pDataIDMemoryPool->~DataIDMemoryPool(); SigmaMemoryFree(stSpriteThreadData.pDataIDMemoryPool, NULL); stSpriteThreadData.pDataIDMemoryPool = NULL;
}


#line 6805
static uint sSpriteThreadProc(void * pDataIn)
{
    SpriteThreadData * pData = (SpriteThreadData *)pDataIn;

    ThreadGlobals::InitializeForThread();
    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_MAIN;
    ThreadGlobals::Get().m_pSpriteThreadBuffer = pData->pSpriteThreadBuffer;
    ThreadGlobals::Get().m_pDataIDMemoryPool = pData->pDataIDMemoryPool;

    for (;;)
    {
        pData->tWorkPendingEvent.WaitForever();
        if (!pData->bActive)
        {
            break;
        }

        GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);

        float flZSlice = 0.0f;
        UI_GetWindowFromID(cgUIIDRoot).Draw(0xff, flZSlice);

        pData->tWorkCompleteEvent.Set();
    }
    return 0;
}


#include "../../Common/Definitions/LevelAreaDefinition.h"
#include "../CPlayer.h"
// Raw cached DWARF has only CWorld.cpp:1075 (formal1076), no header declaration.
SNO WorldGetLevelAreaSNOFromRActor(RActorID idRActor);
#line 4159
void UI_BeginDrawLoadingScreen()

{
    Act eLoadingScreenAct = ACT_5;
    if (GameCommonDataExists())
    {

        CGame & tClientData = CGameGet();
        if (tClientData.tQueuedGameStartParams.bValid)
        {
            eLoadingScreenAct = tClientData.tQueuedGameStartParams.eAct;
        }
        else
        {
            eLoadingScreenAct = GameGetCurrentAct();
        }
    }

    if (eLoadingScreenAct == ACT_OPEN_WORLD)
    {
        SNO snoLevelArea = WorldGetLevelAreaSNOFromRActor(LocalPlayerRActorGet());
        if (snoLevelArea != SNO_NONE)
        {
            Act eLevelAreaAct = LevelAreaGetAct(snoLevelArea);
            if (eLevelAreaAct != ACT_INVALID)
            {
                eLoadingScreenAct = eLevelAreaAct;
            }
        }
    }

    switch (eLoadingScreenAct)
    {
        case ACT_1:

            s_ptUILoadingScreenData->m_tLoadingScreenLeftCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act1_EndCapLeft");
            s_ptUILoadingScreenData->m_tLoadingScreenRightCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act1_EndCapRight");
            s_ptUILoadingScreenData->m_tLoadingScreenMain = ImageTextureFrame("UI:BattleNetLoadingScreen_Act1_4x3");
            break;

        case ACT_2:

            s_ptUILoadingScreenData->m_tLoadingScreenLeftCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act2_EndCapLeft");
            s_ptUILoadingScreenData->m_tLoadingScreenRightCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act2_EndCapRight");
            s_ptUILoadingScreenData->m_tLoadingScreenMain = ImageTextureFrame("UI:BattleNetLoadingScreen_Act2_4x3");
            break;

        case ACT_3:

            s_ptUILoadingScreenData->m_tLoadingScreenLeftCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act3_EndCapLeft");
            s_ptUILoadingScreenData->m_tLoadingScreenRightCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act3_EndCapRight");
            s_ptUILoadingScreenData->m_tLoadingScreenMain = ImageTextureFrame("UI:BattleNetLoadingScreen_Act3_4x3");
            break;

        case ACT_4:

            s_ptUILoadingScreenData->m_tLoadingScreenLeftCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act4_EndCapLeft");
            s_ptUILoadingScreenData->m_tLoadingScreenRightCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act4_EndCapRight");
            s_ptUILoadingScreenData->m_tLoadingScreenMain = ImageTextureFrame("UI:BattleNetLoadingScreen_Act4_4x3");
            break;

        default:

            s_ptUILoadingScreenData->m_tLoadingScreenLeftCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act5_EndCapLeft");
            s_ptUILoadingScreenData->m_tLoadingScreenRightCap = ImageTextureFrame("UI:BattleNetLoadingScreen_Act5_EndCapRight");
            s_ptUILoadingScreenData->m_tLoadingScreenMain = ImageTextureFrame("UI:BattleNetLoadingScreen_Act5_4x3");
            break;
    }








    switch (UnicodeText::GetCurrentLocale())
    {
    case XLOCALE_CHINESE_TAIWAN:
    {
        if (eLoadingScreenAct >= ACT_5)
        {
            s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon = ImageTextureFrame("UI:BattleNetLoadingScreen_Logo_zhTW_x1");
        }
        else
        {
            s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon = ImageTextureFrame("UI:BattleNetLoadingScreen_Logo_zhTW");
        }
        break;
    }
    case XLOCALE_CHINESE_CHINA:
    {

        if (eLoadingScreenAct >= ACT_5)
        {
            s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon = ImageTextureFrame("UI:BattleNetLoadingScreen_Logo_zhCN_x1");
        }
        else
        {
            s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon = ImageTextureFrame("UI:BattleNetLoadingScreen_Logo_zhCN");
        }
        break;
    }
    default:
    {
        if (eLoadingScreenAct >= ACT_5)
        {
            s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon = ImageTextureFrame("UI:BattleNetLoadingScreen_Logo_enUS_x1");
        }
        else
        {
            s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon = ImageTextureFrame("UI:BattleNetLoadingScreen_Logo_enUS");
        }
        break;
    }
    }

    s_ptUILoadingScreenData->m_tLoadingScreenStatusIcon = ImageTextureFrame("UI:BattleNetIconStatus_Loading");
    s_ptUILoadingScreenData->m_tLoadingScreenHourGlass = ImageTextureFrame(GlobalSNOGet(GLOBALSNO_TEXTURE_LOADING_HOURGLASS), 0);


    s_ptUILoadingScreenData->m_ptLoadingScreenIconFade = ::new UIAnimation<unsigned char>;
    s_ptUILoadingScreenData->m_ptLoadingScreenIconFade->AddNode(0.0f, 0, 2);
    s_ptUILoadingScreenData->m_ptLoadingScreenIconFade->AddNode(0.25f, 255, 2);
    s_ptUILoadingScreenData->m_ptLoadingScreenIconFade->AddNode(0.5625f, 255, 2);
    s_ptUILoadingScreenData->m_ptLoadingScreenIconFade->AddNode(1.0f, 0, 2);


    TextCreationParams tParams(UIT_TEXT, "LoadScreenText", cgUIIDTopLayer);
    tParams.fActive = TRUE;

    tParams.snoFont = GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF);
    tParams.nFontSize = 16;




    tParams.eJustifyHorizontal = UIJH_CENTER;
    tParams.eJustifyVertical = UIJV_MIDDLE;
    tParams.rgbaText = crgbaWhite;
    tParams.eRenderLayerOverride = RENDERLAYER_UI_OVERLAY;




    const float FOUR_BY_THREE = 4.0f / 3.0f;
    const float THREE_BY_FOUR = 3.0f / 4.0f;
    float fLeft = 0.0f;
    float fWidth = 960.0f;

    if (GfxGetAspectRatio() - FOUR_BY_THREE <= EPSILON)
    {
        fWidth *= THREE_BY_FOUR;
        fLeft = (960.0f - fWidth) * 0.5f;
    }
    s_ptUILoadingScreenData->m_ptText = ::new UIText(tParams, Vector2D(fLeft, 553.800048828125f), Vector2D(fWidth, 43.2f));
    s_ptUILoadingScreenData->m_ptText->SetWordWrap(TRUE);

    s_ptUILoadingScreenData->m_ptText->SetActive(FALSE);



    ScopedTextureDefinitionLockR pdefTextureLeft(s_ptUILoadingScreenData->m_tLoadingScreenLeftCap.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureLeft, TRUE);
    ScopedTextureDefinitionLockR pdefTextureRight(s_ptUILoadingScreenData->m_tLoadingScreenRightCap.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureRight, TRUE);
    ScopedTextureDefinitionLockR pdefTextureMain(s_ptUILoadingScreenData->m_tLoadingScreenMain.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureMain, TRUE);
    ScopedTextureDefinitionLockR pdefTextureDiabloIcon(s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureDiabloIcon, TRUE);
    ScopedTextureDefinitionLockR pdefTextureStatus(s_ptUILoadingScreenData->m_tLoadingScreenStatusIcon.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureStatus, TRUE);
    ScopedTextureDefinitionLockR pdefTextureHourGlass(s_ptUILoadingScreenData->m_tLoadingScreenHourGlass.snoTexture);
    gptGroupDefTexture->SetRequired((void *)(const TextureDefinition *)pdefTextureHourGlass, TRUE);
}

#include "../Sprite.h"
#include "UIUtils.h"
#line 4360
void UI_DrawLoadingScreen(
    uint32 dwTotalElapsedTicks)
{
    float flZSlice = 0.0f;
    Rect2D rectFullScreenUIC;
    CGameGetLetterboxedViewportUIC(rectFullScreenUIC);

    BOOL fNeedLeftCap = rectFullScreenUIC.left < 0.0f;
    BOOL fNeedRightCap = rectFullScreenUIC.right > 960.0f;
    if (fNeedLeftCap)
    {
        Rect2D rectLeftCapUIC(-scnCapWidth, 0.0f, 0.0f, 720.0f);
        Rect2D rectLeftCapPixels;
        ConvertUICToPixels(rectLeftCapUIC, rectLeftCapPixels);

        Rect2D rectLeftCapClippingUIC(rectFullScreenUIC.left, 0.0f, 0.0f, 720.0f);
        Rect2D rectLeftCapClippingPixels;
        ConvertUICToPixels(rectLeftCapClippingUIC, rectLeftCapClippingPixels);

        SpriteDrawDeferred(
            s_ptUILoadingScreenData->m_tLoadingScreenLeftCap,
            SNO_NONE,
            rectLeftCapPixels,
            crgbaWhite,
            crgbaBlack,
            UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice,
            &rectLeftCapClippingPixels,
            SNO_NONE,
            NULL,
            FALSE,
            RENDERLAYER_UI_OVERLAY,
            FALSE);
        flZSlice += 1.0f;
    }

    if (fNeedRightCap)
    {
        Rect2D rectRightCapUIC(960.0f, 0.0f, 960.0f + scnCapWidth, 720.0f);
        Rect2D rectRightCapPixels;
        ConvertUICToPixels(rectRightCapUIC, rectRightCapPixels);

        Rect2D rectRightCapClippingUIC(960.0f, 0.0f, rectFullScreenUIC.right, 720.0f);
        Rect2D rectRightCapClippingPixels;
        ConvertUICToPixels(rectRightCapClippingUIC, rectRightCapClippingPixels);

        SpriteDrawDeferred(
            s_ptUILoadingScreenData->m_tLoadingScreenRightCap,
            SNO_NONE,
            rectRightCapPixels,
            crgbaWhite,
            crgbaBlack,
            UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice,
            &rectRightCapClippingPixels,
            SNO_NONE,
            NULL,
            FALSE,
            RENDERLAYER_UI_OVERLAY,
            FALSE);
        flZSlice += 1.0f;
    }

    Rect2D rectLoadingScreenMainUIC(0.0f, 0.0f, 960.0f, 720.0f);
    Rect2D rectLoadingScreenMainPixels;
    ConvertUICToPixels(rectLoadingScreenMainUIC, rectLoadingScreenMainPixels);

    SpriteDrawDeferred(
        s_ptUILoadingScreenData->m_tLoadingScreenMain,
        SNO_NONE,
        rectLoadingScreenMainPixels,
        crgbaWhite,
        crgbaBlack,
        UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice,
        NULL,
        SNO_NONE,
        NULL,
        FALSE,
        RENDERLAYER_UI_OVERLAY,
        FALSE);
    flZSlice += 1.0f;


    Vector2D vecDiabloIconTL(0.0f, 0.0f), vecDiabloIconDim(960.0f, 720.0f);

    Rect2D rectDiabloIconUIC;
    rectDiabloIconUIC.left = vecDiabloIconTL.x;
    rectDiabloIconUIC.top = vecDiabloIconTL.y;
    rectDiabloIconUIC.bottom = rectDiabloIconUIC.top + vecDiabloIconDim.y;
    rectDiabloIconUIC.right = rectDiabloIconUIC.left + vecDiabloIconDim.x;

    Rect2D rectDiabloIconPixels;
    ConvertUICToPixels(rectDiabloIconUIC, rectDiabloIconPixels);







    SpriteDrawDeferred(
        s_ptUILoadingScreenData->m_tLoadingScreenDiabloIcon,
        SNO_NONE,
        rectDiabloIconPixels,
        crgbaWhite,
        crgbaBlack,
        UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice,
        NULL,
        SNO_NONE,
        NULL,
        FALSE,
        RENDERLAYER_UI_OVERLAY,
        FALSE);
    flZSlice += 1.0f;


    s_ptUILoadingScreenData->m_ptText->Draw(255, flZSlice);



    Vector2D vecIconTL(668.0f * 0.6f, 1090.0f * 0.6f), vecIconDim(58.0f * 0.6f, 57.0f * 0.6f);
    float flIconHorizontalOffset = 52.0f * 0.6f;




    for (int32 i = 0; i < 5; i++)
    {
        Rect2D rectIconUIC;
        rectIconUIC.left = vecIconTL.x + flIconHorizontalOffset * i;
        rectIconUIC.top = vecIconTL.y;
        rectIconUIC.bottom = rectIconUIC.top + vecIconDim.y;
        rectIconUIC.right = rectIconUIC.left + vecIconDim.x;

        Rect2D rectIconPixels;
        ConvertUICToPixels(rectIconUIC, rectIconPixels);

        RGBAColor rgbaIconColor = crgbaWhite;
        uint32 dwCurrentTicks = dwTotalElapsedTicks < i * LOADING_SCREEN_ICON_FADE_OFFSET ? 0 : (dwTotalElapsedTicks - i * LOADING_SCREEN_ICON_FADE_OFFSET) % LOADING_SCREEN_ICON_FADE_LENGTH;
        float flT = (float)dwCurrentTicks / LOADING_SCREEN_ICON_FADE_LENGTH;
        s_ptUILoadingScreenData->m_ptLoadingScreenIconFade->Evaluate(flT, rgbaIconColor.a);

        SpriteDrawDeferred(
            s_ptUILoadingScreenData->m_tLoadingScreenStatusIcon,
            SNO_NONE,
            rectIconPixels,
            rgbaIconColor,
            crgbaBlack,
            UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice,
            NULL,
            SNO_NONE,
            NULL,
            FALSE,
            RENDERLAYER_UI_OVERLAY,
            FALSE);
        flZSlice += 1.0f;
    }




























































































}
