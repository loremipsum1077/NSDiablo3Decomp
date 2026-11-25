#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleModalNotificationManager.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../CGame.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/UnicodeText.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/RActors/RActors.h:278 and :596. Only the members this file touches are declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    uint8 _unadopted0[0x410 - 0x04];  // 0x04 (invented name: idSScene .. end)
};
static_assert(sizeof(RActor) == 0x410, "RActor");

inline RActor * RActorTryToGet(const RActorID idRActor) {  // line 596
    if (idRActor == RACTORID_INVALID)
        return NULL;
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return NULL;
    return tCGame.ptRActors->ElementTryToGet(idRActor);
}
RActorID RActorCreate(SNO snoActor, float flScale, float flUnused, const PRTransform & tTransform, uint32 dwFlags, RenderContext eContext, int32 nUnused);
// X/Common/VisualInventory.h:83 and :96. The item layout is not adopted yet.
struct VisualEquipment {  // size 0x90
    uint8 _unadopted0[0x90];  // 0x00 (invented name: tVisualItem .. tCosmeticItems)
};
struct VisualInventory {  // size 0x120
    VisualEquipment tEquipmentCurrent;  // 0x00
    VisualEquipment tEquipmentLast;  // 0x90
};
static_assert(sizeof(VisualInventory) == 0x120, "VisualInventory");
VisualInventory * VisualInventoryGet(ACDID idACD);
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
BOOL PlayerIsMale(const Player & tPlayer);
BOOL PlayerLoadoutMatchesCurrentEquipment(const Player & tPlayer, int32 nLoadout);
// X/Client/EffectGroup.cpp
typedef int32 EffectGroupID;
void EffectGroupOutro(EffectGroupID idEffectGroup);
EffectGroupID EffectGroupCreateTracked(SNO snoEffectGroup, RActorID idRActorSource, RActorID idRActorTarget, int32 nUnused0, int32 nUnused1);
// X/Client/Lights.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGame.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/Input/Input.cpp
void InputClearRemappings();
void InputRemapSingleJoyconButton(int32 eButton, int32 eRemappedButton);
// X/Client/UserInterface/Interface/UITutorials.h:34
#include "../Interface/UITutorials.h"




extern UITutorialManager gtTutorialMgr;
// X/Client/UserInterface/Interface/UITextInputPrompt.cpp
namespace UITextInputPrompt {
void Close();
}  // namespace UITextInputPrompt
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tVisualEquipment, int32 nUnused, PlayerClass eClass, BOOL bIsFemale, const PRTransform & tTransform, RenderContext eContext, uint32 dwFlags, float flScale, int32 nUnused2);
// X/Client/UserInterface/Console/UIConsoleHeroSelect.cpp
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL bIsFemale);
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/Console/ControllerUtils.cpp
namespace Console {
namespace ControllerUtils {
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bUnused);
}  // namespace ControllerUtils
}  // namespace Console
#include "ControllerActions.h"
namespace Console {
typedef int32 ControllerInput;
typedef int32 ControllerActionId;








}  // namespace Console
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
ConsoleModalNotificationManager & GetModalNotificationManager();
void UpdateButtonLegendText();
void OpenToScreen(int32 nScreenSet, int32 nScreen, int32 nControllerIndex, BOOL bUnused);
void CloseScreenSet(int32 nUnused0, int32 nUnused1);
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride);
void CloseModalDialog(const UIID & idDialog);
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Interface/UIEquipmentManagerCommon.h/.cpp
namespace UIEquipmentManager {
const int32 cgnLoadoutsPerPage = 10;
int32 GetSelectedLoadout();
void SetSelectedLoadout(int32 nLoadout);
BOOL LoadoutGetLoadoutEquipError(const Player & tPlayer, int32 nLoadout, CRefString & sError, BOOL bUnused);
void EquipLoadoutInternal(const Player * ptPlayer);
void PromptForLoadoutName(const Player & tPlayer, CRefString sTitle, CRefString sDefault, void (*pfnCallback)(BOOL, CRefString, Player *));
void SaveLoadoutInternal(BOOL bSuccessful, CRefString sInput, Player * ptPlayer);
void RenameLoadoutInternal(BOOL bSuccessful, CRefString sInput, Player * ptPlayer);
void UpdateLoadoutPreview(const Player & tPlayer, const LoadoutSavedData & tLoadout, UIRActor & tRActor, bool bUnused);
void ShowErrorMsg(const Player * ptPlayer, CRefString sMessage);
void OnPlayerLoadoutDataMessageReceived(int32 nLoadoutIndex);
}  // namespace UIEquipmentManager
// X/Client/UserInterface/Console/UIConsoleEquipmentManagerDetails.cpp
namespace UIEquipmentManagerDetails {
BOOL IsActive();
void Close();
void OnUpdate();
void OnLeftStick(const UIID & id, const int32 nControllerIndex);
void OnButton1(const UIID & id, const int32 nControllerIndex);
void OnButton2(const UIID & id, const int32 nControllerIndex);
void OnButtonL2(const UIID & id, const int32 nControllerIndex);
void OnButtonR2(const UIID & id, const int32 nControllerIndex);
}  // namespace UIEquipmentManagerDetails
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_EQUIPMENT_MANAGER_CONSOLE;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_VIEWPORT;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SCROLL_ARROW_LEFT;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SCROLL_ARROW_RIGHT;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_CURRENTLY_EQUIPPED_FRAME;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_ERROR;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_ERROR_INFO;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_NOT_SAVED;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_CURRENTLY_EQUIPPED_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_ERROR_MESSAGE;
extern UIID UITXT_CONSOLE_BUTTON_LEGEND_TEXT;

static const int32 ACTION_SAVE = 0;  // line 43
static const int32 ACTION_CLOSE = 1;  // line 44
static const int32 ACTION_EQUIP = 2;  // line 45
static const int32 ACTION_VIEW_DETAILS = 3;  // line 46
static const int32 ACTION_RENAME = 4;  // line 47

enum class NameInputState : int {
    INACTIVE = 0,
    ACTIVE = 1,
};

struct EquipManGlobals {  // size 0x88
    Player * ptPlayer;  // 0x00
    int32 nControllerIndex;  // 0x08
    NameInputState eNameInputState;  // 0x0c
    uint32 dwLastKnownEquippedIndexMap;  // 0x10
    bool bCurrentHeroDirty;  // 0x14
    bool bDirtySlots[UIEquipmentManager::cgnLoadoutsPerPage];  // 0x15
    bool bIsScrolling;  // 0x1f
    Vector2D vecScrollDest;  // 0x20
    const float SCROLL_ANIM_SPEED;  // 0x28
    const float unk_2c;
    EffectGroupID idHightlightEffect;  // 0x30
    EffectGroupID idEquippedArmorSetEffect[UIEquipmentManager::cgnLoadoutsPerPage];  // 0x34
    Console::AnalogStickInputFilter tStickFilter;  // 0x60
    EquipManGlobals();
    void CleanupHighlightEffect();
    void CleanupEquippedEffects();
    void CleanupEquippedEffect(int32 nIndex);
};
static_assert(sizeof(EquipManGlobals) == 0x88, "EquipManGlobals");

static EquipManGlobals * sgptEquipManGlobals;  // line 89

static void sOnSelectionChanged();
static void sUpdateLoadoutPreview(int32 nIndex);
static void sUpdateEquippedStatus();
static void sUpdateHighlight();
static void sRefreshEquippedStatus();
static void sUpdateScrollArrows();
static void sUpdateButtonLegend();
static void sUpdateCurrentHero();
static void sUpdateLoadoutName();
static void sUpdateScrolling();
static void sScrollToSelectedLoadout();
static uint32 sGetEquippedLoadoutIndexes();
static bool sIsLoadoutEquipped(int32 nIndex);

EquipManGlobals::EquipManGlobals()
    : ptPlayer(NULL), nControllerIndex(-1), eNameInputState(NameInputState::INACTIVE), dwLastKnownEquippedIndexMap(0), bCurrentHeroDirty(false), bIsScrolling(false), vecScrollDest(0.0f, 0.0f), SCROLL_ANIM_SPEED(0.035f), unk_2c(0.1f), idHightlightEffect(-1)
{
    SigmaMemoryFill(bDirtySlots, 0, sizeof(bDirtySlots));
    SigmaMemoryFill(idEquippedArmorSetEffect, 0, sizeof(idEquippedArmorSetEffect));
    tStickFilter = Console::GetDefaultStickInputFilter(Console::FILTER_HORIZONTAL);
    tStickFilter.SetMinimumInputDelay(350);
}

void EquipManGlobals::CleanupHighlightEffect()
{
    EffectGroupOutro(idHightlightEffect);
    idHightlightEffect = -1;
}

void EquipManGlobals::CleanupEquippedEffects()
{
    for (int32 i = 0; i < UIEquipmentManager::cgnLoadoutsPerPage; i++) {
        CleanupEquippedEffect(i);
    }
}

void EquipManGlobals::CleanupEquippedEffect(int32 nIndex)
{
    EffectGroupOutro(idEquippedArmorSetEffect[nIndex]);
    idEquippedArmorSetEffect[nIndex] = -1;
}

static bool sIsActive()
{
    return UI_GetWindowFromID(UIDLG_EQUIPMENT_MANAGER_CONSOLE).IsActive();
}

namespace UIEquipmentManager {
namespace Console {

void InitializeForGame()
{
    if (sgptEquipManGlobals != NULL) {
        return;
    }
    sgptEquipManGlobals = new EquipManGlobals();
}

void DisposeForGame()
{
    delete sgptEquipManGlobals;
    sgptEquipManGlobals = NULL;
}

void OpenForController(int32 nControllerIndex)
{
    if (nControllerIndex == -1 || sgptEquipManGlobals == NULL || sIsActive()) {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer == NULL) {
        return;
    }

    sgptEquipManGlobals->ptPlayer = ptPlayer;
    sgptEquipManGlobals->nControllerIndex = nControllerIndex;
    sgptEquipManGlobals->bCurrentHeroDirty = true;
    for (int32 i = 0; i < cgnLoadoutsPerPage; i++) {
        sgptEquipManGlobals->bDirtySlots[i] = true;
    }

    ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_EQUIPMENT_MANAGER_CONSOLE, FALSE, GetUIIDNone());
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT).SetActive(TRUE);

    UIWindow & tContainer = UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER);
    Vector2D vecTL;
    Vector2D vecDim;
    tContainer.GetRelativePosition(vecTL, vecDim);
    vecTL.x = 0.0f;
    tContainer.SetRelativePosition(vecTL, vecDim);

    gtTutorialMgr.ShowTutorial(UITXT_CONSOLE_BUTTON_LEGEND_TEXT, GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIPMENT_MANAGER_SAVE), NULL, TRUE);

    SetSelectedLoadout(0);
    sOnSelectionChanged();

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_EQUIPMENT_MANAGER_OPEN), cgtPSODefaults);
}

void Close()
{
    if (!sIsActive()) {
        return;
    }

    InputClearRemappings();

    if (UIEquipmentManagerDetails::IsActive()) {
        UIEquipmentManagerDetails::Close();
    }

    ::Console::Screens::CloseModalDialog(UIDLG_EQUIPMENT_MANAGER_CONSOLE);

    sgptEquipManGlobals->CleanupHighlightEffect();
    sgptEquipManGlobals->CleanupEquippedEffects();

    UITextInputPrompt::Close();

    sgptEquipManGlobals->eNameInputState = NameInputState::INACTIVE;
}

}  // namespace Console
}  // namespace UIEquipmentManager

static bool sShouldHandleInput(int32 nControllerIndex)
{
    return sgptEquipManGlobals != NULL && sIsActive() && sgptEquipManGlobals->nControllerIndex == nControllerIndex && sgptEquipManGlobals->eNameInputState == NameInputState::INACTIVE;
}

namespace UIEquipmentManager {
namespace Console {

void OnLeftStick(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex)) {
        return;
    }

    if (UIEquipmentManagerDetails::IsActive()) {
        UIEquipmentManagerDetails::OnLeftStick(id, nControllerIndex);
        return;
    }

    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    int idRule = sgptEquipManGlobals->tStickFilter.HandleInput(vecStick, FALSE);

    switch (idRule) {
    case ::Console::RULE_STICK_LEFT:
    {
        int32 nIndex = GetSelectedLoadout();
        if (nIndex > 0) {
            SetSelectedLoadout(nIndex - 1);
            sOnSelectionChanged();
        }
        break;
    }

    case ::Console::RULE_STICK_RIGHT:
    {
        int32 nIndex = GetSelectedLoadout();
        if (nIndex < cgnLoadoutsPerPage - 1) {
            SetSelectedLoadout(nIndex + 1);
            sOnSelectionChanged();
        }
        break;
    }
    }
}

void OnDPadLeft(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex) || UIEquipmentManagerDetails::IsActive()) {
        return;
    }

    int32 nIndex = GetSelectedLoadout();
    if (nIndex > 0) {
        SetSelectedLoadout(nIndex - 1);
        sOnSelectionChanged();
    }
}

void OnDPadRight(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex) || UIEquipmentManagerDetails::IsActive()) {
        return;
    }

    int32 nIndex = GetSelectedLoadout();
    if (nIndex < cgnLoadoutsPerPage - 1) {
        SetSelectedLoadout(nIndex + 1);
        sOnSelectionChanged();
    }
}

}  // namespace Console
}  // namespace UIEquipmentManager

void sEquipLoadoutConfirmation(int32 nControllerIndex, const UIID &)
{
    UIEquipmentManager::EquipLoadoutInternal(PlayerGetByControllerIndex(nControllerIndex));
}

namespace UIEquipmentManager {
namespace Console {

void OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex)) {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer == NULL) {
        return;
    }

    if (UIEquipmentManagerDetails::IsActive()) {
        UIEquipmentManagerDetails::OnButton1(id, nControllerIndex);
        return;
    }

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.GetActionForInput(4) != ACTION_EQUIP) {
        return;
    }

    int32 nSelectedLoadout = GetSelectedLoadout();

    if (sIsLoadoutEquipped(nSelectedLoadout)) {
        ShowErrorMsg(ptPlayer, StringListFetch("ConsoleEquipmentManager:ErrorAlreadyEquipped"));
        return;
    }

    CRefString sErrorUnused;
    if (LoadoutGetLoadoutEquipError(*ptPlayer, nSelectedLoadout, sErrorUnused, TRUE)) {
        NotificationCreationParams tParams;
        tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle");
        tParams.uszMessage = StringListFetch("Hero:EquipmentManager_EquipErrorConfirm");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &sEquipLoadoutConfirmation>(nControllerIndex);
        tParams.nControllerIndex = nControllerIndex;
        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    } else {
        EquipLoadoutInternal(ptPlayer);
    }
}

void OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex)) {
        return;
    }

    if (UIEquipmentManagerDetails::IsActive()) {
        UIEquipmentManagerDetails::OnButton2(id, nControllerIndex);
        return;
    }

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.GetActionForInput(7) != ACTION_CLOSE) {
        return;
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_EQUIPMENT_MANAGER_CLOSE), cgtPSODefaults);
    ::Console::Screens::CloseScreenSet(1, 0);
}

}  // namespace Console
}  // namespace UIEquipmentManager

void sSaveLoadoutNameCallback(BOOL bSuccessful, CRefString sInput, Player * ptPlayer)
{
    if (sgptEquipManGlobals != NULL) {
        sgptEquipManGlobals->eNameInputState = NameInputState::INACTIVE;
    }

    if (bSuccessful && ptPlayer != NULL) {
        UIEquipmentManager::SaveLoadoutInternal(bSuccessful, sInput, ptPlayer);
    }
}

void sLoadoutOverwriteConfirmation(int32 nControllerIndex, const UIID &)
{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer == NULL) {
        return;
    }

    sgptEquipManGlobals->eNameInputState = NameInputState::ACTIVE;
    UIEquipmentManager::PromptForLoadoutName(*ptPlayer, StringListFetch("ConsoleEquipmentManager:SaveLoadoutDialogTitle"), CRefString(""), sSaveLoadoutNameCallback);
}

namespace UIEquipmentManager {
namespace Console {

void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex) || UIEquipmentManagerDetails::IsActive()) {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer == NULL) {
        return;
    }

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.GetActionForInput(5) != ACTION_SAVE) {
        return;
    }

    int32 nSelectedLoadoutIndex = GetSelectedLoadout();
    if (!ptPlayer->tLoadouts[nSelectedLoadoutIndex].bEmpty) {
        NotificationCreationParams tParams;
        tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle");
        tParams.uszMessage = StringListFetch("Hero:EquipmentManager_SaveConfirm");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_1_1<void, int, const UIID &, &sLoadoutOverwriteConfirmation>(nControllerIndex);
        tParams.nControllerIndex = nControllerIndex;
        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    } else {
        sLoadoutOverwriteConfirmation(nControllerIndex, GetUIIDNone());
    }
}

void OnButton4(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex) || UIEquipmentManagerDetails::IsActive()) {
        return;
    }

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.GetActionForInput(6) != ACTION_VIEW_DETAILS) {
        return;
    }

    gtTutorialMgr.CloseTutorialIfOpen(GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIPMENT_MANAGER_DETAILS));
    ::Console::Screens::OpenToScreen(19, 0, nControllerIndex, TRUE);
}

void OnButtonL2(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex)) {
        return;
    }

    if (UIEquipmentManagerDetails::IsActive()) {
        UIEquipmentManagerDetails::OnButtonL2(id, nControllerIndex);
    }
}

void OnButtonR2(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex)) {
        return;
    }

    if (UIEquipmentManagerDetails::IsActive()) {
        UIEquipmentManagerDetails::OnButtonR2(id, nControllerIndex);
    }
}

}  // namespace Console
}  // namespace UIEquipmentManager

void sRenameLoadoutNameCallback(BOOL bSuccessful, CRefString sInput, Player * ptPlayer)
{
    if (sgptEquipManGlobals != NULL) {
        sgptEquipManGlobals->eNameInputState = NameInputState::INACTIVE;
    }

    if (bSuccessful && ptPlayer != NULL) {
        UIEquipmentManager::RenameLoadoutInternal(bSuccessful, sInput, ptPlayer);
    }
}

namespace UIEquipmentManager {
namespace Console {

void OnButtonL3(const UIID & id, const int32 nControllerIndex)
{
    if (!sShouldHandleInput(nControllerIndex) || UIEquipmentManagerDetails::IsActive()) {
        return;
    }

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.GetActionForInput(21) != ACTION_RENAME) {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer == NULL) {
        return;
    }

    sgptEquipManGlobals->eNameInputState = NameInputState::ACTIVE;
    PromptForLoadoutName(*ptPlayer, StringListFetch("ConsoleEquipmentManager:RenameDialogTitle"), CRefString(""), sRenameLoadoutNameCallback);
}

void OnUpdate()
{
    if (sgptEquipManGlobals == NULL || !sIsActive()) {
        return;
    }

    if (UIEquipmentManagerDetails::IsActive()) {
        UIEquipmentManagerDetails::OnUpdate();
        return;
    }

    uint32 dwEquippedIndexMap = sGetEquippedLoadoutIndexes();
    if (dwEquippedIndexMap != sgptEquipManGlobals->dwLastKnownEquippedIndexMap) {
        sgptEquipManGlobals->dwLastKnownEquippedIndexMap = dwEquippedIndexMap;
        sRefreshEquippedStatus();
    }

    sUpdateCurrentHero();

    for (int32 i = 0; i < cgnLoadoutsPerPage; i++) {
        sUpdateLoadoutPreview(i);
    }

    sUpdateLoadoutName();
    sUpdateEquippedStatus();
    sUpdateScrollArrows();
    sUpdateButtonLegend();

    if (sgptEquipManGlobals->idHightlightEffect == -1) {
        sUpdateHighlight();
    }

    sUpdateScrolling();
}

}  // namespace Console

void OnPlayerLoadoutDataMessageReceived(int32 nLoadoutIndex)
{
    if (sgptEquipManGlobals == NULL || sgptEquipManGlobals->ptPlayer == NULL) {
        return;
    }

    if (sgptEquipManGlobals->ptPlayer->tLoadouts[nLoadoutIndex].bEmpty) {
        return;
    }

    gtTutorialMgr.ShowTutorial(UITXT_CONSOLE_BUTTON_LEGEND_TEXT, GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIPMENT_MANAGER_DETAILS), NULL, TRUE);
}

}  // namespace UIEquipmentManager

static void sRefreshEquippedStatus()
{
    if (sgptEquipManGlobals == NULL || !sIsActive()) {
        return;
    }

    sgptEquipManGlobals->bCurrentHeroDirty = true;
    for (int32 i = 0; i < UIEquipmentManager::cgnLoadoutsPerPage; i++) {
        sgptEquipManGlobals->bDirtySlots[i] = true;
        sUpdateLoadoutPreview(i);
    }

    sUpdateEquippedStatus();

    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

static void sUpdateScrollArrows()
{
    UIWindow & tViewport = UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_VIEWPORT);
    Vector2D vecViewportTL;
    Vector2D vecViewportDim;
    tViewport.GetRelativePosition(vecViewportTL, vecViewportDim);

    UIWindow & tContainer = UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER);
    Vector2D vecContainerTL;
    Vector2D vecContainerDim;
    tContainer.GetRelativePosition(vecContainerTL, vecContainerDim);

    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SCROLL_ARROW_LEFT).SetActive(vecContainerTL.x < 0.0f);
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SCROLL_ARROW_RIGHT).SetActive(vecContainerTL.x + vecContainerDim.x > vecViewportDim.x);
}

static void sUpdateButtonLegend()
{
    if (Console::Screens::GetActionManager().AreActionsDirty()) {
        InputClearRemappings();

        Console::ControllerActionManager & tActionManager = Console::Screens::GetActionManager();
        tActionManager.ClearAllActions();

        int32 nIndex = UIEquipmentManager::GetSelectedLoadout();
        if (!sgptEquipManGlobals->ptPlayer->tLoadouts[nIndex].bEmpty) {
            tActionManager.SetAction(4, ACTION_EQUIP, StringListFetch("ConsoleEquipmentManager:EquipCommand"));
            tActionManager.SetAction(6, ACTION_VIEW_DETAILS, StringListFetch("ConsoleEquipmentManager:DetailsCommand"));
            InputRemapSingleJoyconButton(8, 14);
            tActionManager.SetAction(21, ACTION_RENAME, StringListFetch("ConsoleEquipmentManager:RenameCommand"));
        }

        tActionManager.SetAction(7, ACTION_CLOSE, StringListFetch("ConsoleUI:Close"));
        tActionManager.SetAction(5, ACTION_SAVE, StringListFetch("ConsoleEquipmentManager:SaveCommand"));

        Console::Screens::UpdateButtonLegendText();
        Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    }
}

static void sAttachLightToModel(UIRActor & tModel)
{
    LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tModel.GetRenderContext());
    tModel.SetLight(idLight);
}

static void sUpdateHeroRActor(const UIID & idRActorControl, VisualInventory & tVisualInventory, const Player & tPlayer, bool bAddLight)
{
    PlayerClass eClass = PlayerGetPlayerClass(&tPlayer);
    BOOL bIsFemale = !PlayerIsMale(tPlayer);
    SNO snoActor = GetFrontendCharacterSNO(eClass, bIsFemale);
    const SNO snoIrradianceTex = GlobalSNOGet(GLOBALSNO_RING_IRRADIANCE_TEXTURE);

    UIRActor & tHero = UI_GetRActorFromID(idRActorControl);
    tHero.Cleanup();

    PRTransform transform(cgqIdentity, cgwpNull);
    tHero.DeleteRActor();
    RActorID idRActor = UI_CreateHeroUIRActor(snoActor, snoIrradianceTex, tVisualInventory.tEquipmentCurrent, 0, eClass, bIsFemale, transform, tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), 0x68700, 1.0f, 0);
    tHero.SetRActor(idRActor);

    if (bAddLight) {
        sAttachLightToModel(tHero);
    }
}

static SNO sGetPlaceholderHeroSNO(PlayerClass eClass, BOOL bIsFemale)
{
    SNO snoActor = -1;
    GlobalSNO gsnoActor;

    switch (eClass) {
    case PLAYER_CLASS_BARBARIAN:
        gsnoActor = bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_BARBARIAN_FEMALE : GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_BARBARIAN_MALE;
        break;
    case PLAYER_CLASS_CRUSADER:
        gsnoActor = bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_CRUSADER_FEMALE : GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_CRUSADER_MALE;
        break;
    case PLAYER_CLASS_DEMONHUNTER:
        gsnoActor = bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_DEMONHUNTER_FEMALE : GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_DEMONHUNTER_MALE;
        break;
    case PLAYER_CLASS_MONK:
        gsnoActor = bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_MONK_FEMALE : GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_MONK_MALE;
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        gsnoActor = bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_WITCHDOCTOR_FEMALE : GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_WITCHDOCTOR_MALE;
        break;
    case PLAYER_CLASS_WIZARD:
        gsnoActor = bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_WIZARD_FEMALE : GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_WIZARD_MALE;
        break;
    case PLAYER_CLASS_NECROMANCER:
        gsnoActor = bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_NECROMANCER_FEMALE : GLOBALSNO_ACTOR_CONSOLE_ARMORY_PLACEHOLDER_NECROMANCER_MALE;
        break;
    default:
        return snoActor;
    }

    snoActor = GlobalSNOGet(gsnoActor);
    return snoActor;
}

static void sUpdatePlaceholderHeroRActor(const UIID & idPlaceholderRActor)
{
    PlayerClass eClass = PlayerGetPlayerClass(sgptEquipManGlobals->ptPlayer);
    BOOL bIsFemale = !PlayerIsMale(*sgptEquipManGlobals->ptPlayer);
    SNO snoActor = sGetPlaceholderHeroSNO(eClass, bIsFemale);
    GlobalSNOGet(GLOBALSNO_RING_IRRADIANCE_TEXTURE);

    UIRActor & tHero = UI_GetRActorFromID(idPlaceholderRActor);
    tHero.Cleanup();

    PRTransform transform(cgqIdentity, cgwpNull);
    tHero.DeleteRActor();
    RActorID idRActor = RActorCreate(snoActor, 1.0f, -1.0f, transform, 0, tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
    tHero.SetRActor(idRActor);

    tHero.SetObserver(GlobalSNOGet(!bIsFemale && eClass == PLAYER_CLASS_BARBARIAN ? GLOBALSNO_OBSERVER_CONSOLE_EQUIPMENT_MANAGER_EMPTY_LOADOUT : GLOBALSNO_OBSERVER_CONSOLE_EQUIPMENT_MANAGER_LOADOUT));

    sAttachLightToModel(tHero);
}

static void sUpdateEquippedStatus()
{
    if (sgptEquipManGlobals->ptPlayer == NULL) {
        return;
    }

    uint32 dwEquippedLoadoutIndexMap = sGetEquippedLoadoutIndexes();

    UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_NOT_SAVED).SetActive(dwEquippedLoadoutIndexMap == 0);
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_CURRENTLY_EQUIPPED_FRAME).SetActive(dwEquippedLoadoutIndexMap != 0);

    if (dwEquippedLoadoutIndexMap != 0) {
        int32 nEquippedLoadoutIndex = 0;
        while ((dwEquippedLoadoutIndexMap & 1) == 0) {
            dwEquippedLoadoutIndexMap >>= 1;
            nEquippedLoadoutIndex++;
        }

        UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_CURRENTLY_EQUIPPED_NAME).SetText(sgptEquipManGlobals->ptPlayer->tLoadouts[nEquippedLoadoutIndex].usName, FALSE);
    }
}

static void sUpdateCurrentHero()
{
    if (!sgptEquipManGlobals->bCurrentHeroDirty || sgptEquipManGlobals->ptPlayer == NULL) {
        return;
    }

    VisualInventory * ptVisualInventory = VisualInventoryGet(sgptEquipManGlobals->ptPlayer->idPlayerACD);
    if (ptVisualInventory != NULL) {
        UIID idCurrentHeroRActor = UIIDCreateFromParent(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT, ".CurrentLoadout.Model.Hero3DModel");
        UIRActor & tHeroRActor = UI_GetRActorFromID(idCurrentHeroRActor);
        tHeroRActor.Cleanup();

        sUpdateHeroRActor(idCurrentHeroRActor, *ptVisualInventory, *sgptEquipManGlobals->ptPlayer, true);

        sUpdateEquippedStatus();
    }

    sgptEquipManGlobals->bCurrentHeroDirty = false;
}

static void sUpdateLoadoutPreview(int32 nIndex)
{
    if (!sgptEquipManGlobals->bDirtySlots[nIndex] || sgptEquipManGlobals->ptPlayer == NULL) {
        return;
    }

    UIID idLoadoutSlot = UIIDCreateFromParent(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER, CRefString().Format(".LoadoutSlot%d", nIndex));
    UIID idEmptyLoadout = UIIDCreateFromParent(idLoadoutSlot, ".Empty");
    UIID idLoadout = UIIDCreateFromParent(idLoadoutSlot, ".Loadout");
    UIID idEmptyPreviewRActor = UIIDCreateFromParent(idEmptyLoadout, ".Model.Hero3DModel");
    UIID idPreviewRActor = UIIDCreateFromParent(idLoadout, ".Model.Hero3DModel");
    UIID idEquippedStatus = UIIDCreateFromParent(idLoadout, ".EquippedStatus");

    const LoadoutSavedData & tLoadout = sgptEquipManGlobals->ptPlayer->tLoadouts[nIndex];

    if (tLoadout.bEmpty) {
        sUpdatePlaceholderHeroRActor(idEmptyPreviewRActor);
        sgptEquipManGlobals->CleanupEquippedEffect(nIndex);
    } else {
        UIRActor & tRActor = UI_GetRActorFromID(idPreviewRActor);
        tRActor.Cleanup();
        UIEquipmentManager::UpdateLoadoutPreview(*sgptEquipManGlobals->ptPlayer, tLoadout, tRActor, false);

        if (PlayerLoadoutMatchesCurrentEquipment(*sgptEquipManGlobals->ptPlayer, nIndex)) {
            RActorID idRActor = tRActor.GetRActor();
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL) {
                sgptEquipManGlobals->idEquippedArmorSetEffect[nIndex] = EffectGroupCreateTracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_CONSOLE_ARMORY_EQUIPPED_ARMOR_SET), idRActor, RACTORID_INVALID, 0, 0);
            }
            UI_GetWindowFromID(idEquippedStatus).SetActive(TRUE);
        } else {
            sgptEquipManGlobals->CleanupEquippedEffect(nIndex);
            UI_GetWindowFromID(idEquippedStatus).SetActive(FALSE);
        }
    }

    if (UIEquipmentManager::GetSelectedLoadout() == nIndex) {
        sUpdateHighlight();
    }

    UI_GetWindowFromID(idEmptyLoadout).SetActive(tLoadout.bEmpty);
    UI_GetWindowFromID(idLoadout).SetActive(!tLoadout.bEmpty);

    sgptEquipManGlobals->bDirtySlots[nIndex] = false;
}

static void sUpdateLoadoutName()
{
    if (sgptEquipManGlobals->ptPlayer == NULL) {
        return;
    }

    int32 nIndex = UIEquipmentManager::GetSelectedLoadout();
    const LoadoutSavedData & tLoadout = sgptEquipManGlobals->ptPlayer->tLoadouts[nIndex];

    CRefString sLoadoutName;
    if (tLoadout.bEmpty) {
        sLoadoutName = LocalizedSprintf(StringListFetch("ConsoleEquipmentManager:EmptyLoadoutName"), UnicodeText::FormatNumber(nIndex + 1, TRUE, FNS_NORMAL, 0).str(), NULL);
    } else {
        sLoadoutName = tLoadout.usName;
    }

    UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_NAME).SetText(sLoadoutName, FALSE);

    CRefString sErrorMsg;
    BOOL bHasError = UIEquipmentManager::LoadoutGetLoadoutEquipError(*sgptEquipManGlobals->ptPlayer, nIndex, sErrorMsg, FALSE);

    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_ERROR).SetActive(bHasError);
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_ERROR_INFO).SetActive(bHasError);

    if (bHasError) {
        UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_ERROR_MESSAGE).SetText(sErrorMsg, FALSE);
    }
}

static void sUpdateHighlight()
{
    sgptEquipManGlobals->CleanupHighlightEffect();

    int32 nIndex = UIEquipmentManager::GetSelectedLoadout();
    const LoadoutSavedData & tLoadout = sgptEquipManGlobals->ptPlayer->tLoadouts[nIndex];

    UIID idModel = GetUIIDNone();
    if (tLoadout.bEmpty) {
        idModel = UIIDCreateFromParent(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER, CRefString().Format(".LoadoutSlot%d.Empty.Model.Hero3DModel", nIndex));
    } else {
        idModel = UIIDCreateFromParent(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER, CRefString().Format(".LoadoutSlot%d.Loadout.Model.Hero3DModel", nIndex));
    }

    RActorID idRActor = UI_GetRActorFromID(idModel).GetRActor();
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor != NULL) {
        SNO snoSelectEffect = GlobalSNOGet(PlayerLoadoutMatchesCurrentEquipment(*sgptEquipManGlobals->ptPlayer, nIndex) ? GLOBALSNO_EFFECTGROUP_CONSOLE_ARMORY_CURRENT_SELECTION_EQUIPPED : GLOBALSNO_EFFECTGROUP_CONSOLE_ARMORY_CURRENT_SELECTION);
        sgptEquipManGlobals->idHightlightEffect = EffectGroupCreateTracked(snoSelectEffect, idRActor, RACTORID_INVALID, 0, 0);
    }
}

static void sUpdateScrolling()
{
    if (sgptEquipManGlobals->bIsScrolling) {
        UIWindow & tContainer = UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER);
        Vector2D vecTL;
        Vector2D vecDim;
        tContainer.GetRelativePosition(vecTL, vecDim);

        if (Math::Abs(vecTL.x - sgptEquipManGlobals->vecScrollDest.x) <= sgptEquipManGlobals->unk_2c) {
            vecTL.x = sgptEquipManGlobals->vecScrollDest.x;
            tContainer.SetRelativePosition(vecTL, vecDim);
            sgptEquipManGlobals->bIsScrolling = false;
        } else {
            float flDeltaTime = CGameGet().flRealDeltaTime;
            float flAmount = 1.0f - powf(sgptEquipManGlobals->SCROLL_ANIM_SPEED, flDeltaTime);
            float flX = vecTL.x + flAmount * (sgptEquipManGlobals->vecScrollDest.x - vecTL.x);
            tContainer.SetRelativePosition(Vector2D(flX, vecTL.y), vecDim);
        }
    }
}

static void sScrollToSelectedLoadout()
{
    UIWindow & tContainer = UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER);
    int32 nIndex = UIEquipmentManager::GetSelectedLoadout();
    UIID idLoadoutSlot = UIIDCreateFromParent(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_CONTAINER, CRefString().Format(".LoadoutSlot%d", nIndex));
    UIWindow & tSelectedLoadoutWnd = UI_GetWindowFromID(idLoadoutSlot);

    Vector2D vecTL;
    Vector2D vecDim;
    tSelectedLoadoutWnd.GetRelativePosition(vecTL, vecDim);
    float flDestX = -(vecTL.x + vecDim.x * 0.5f);

    tContainer.GetRelativePosition(vecTL, vecDim);

    UIWindow & tViewport = UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT_VIEWPORT);
    Vector2D vecViewportTL;
    Vector2D vecViewportDim;
    tViewport.GetRelativePosition(vecViewportTL, vecViewportDim);
    flDestX += vecViewportDim.x * 0.5f;

    if (Math::Abs(vecTL.x - flDestX) > sgptEquipManGlobals->unk_2c) {
        sgptEquipManGlobals->bIsScrolling = true;
        sgptEquipManGlobals->vecScrollDest.Set(flDestX, 0.0f);
    }
}

static void sOnSelectionChanged()
{
    sUpdateHighlight();
    sScrollToSelectedLoadout();

    int32 nSelectedLoadout = UIEquipmentManager::GetSelectedLoadout();
    if (!sgptEquipManGlobals->ptPlayer->tLoadouts[nSelectedLoadout].bEmpty && !PlayerLoadoutMatchesCurrentEquipment(*sgptEquipManGlobals->ptPlayer, nSelectedLoadout)) {
        gtTutorialMgr.ShowTutorial(UITXT_CONSOLE_BUTTON_LEGEND_TEXT, GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIPMENT_MANAGER_EQUIP), NULL, TRUE);
    }

    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);
}

static uint32 sGetEquippedLoadoutIndexes()
{
    uint32 dwEquippedIndexMap = 0;
    for (int32 i = 0; i < UIEquipmentManager::cgnLoadoutsPerPage; i++) {
        if (PlayerLoadoutMatchesCurrentEquipment(*sgptEquipManGlobals->ptPlayer, i)) {
            dwEquippedIndexMap |= 1 << i;
        }
    }
    return dwEquippedIndexMap;
}

static bool sIsLoadoutEquipped(int32 nIndex)
{
    if (nIndex < 0 || nIndex >= UIEquipmentManager::cgnLoadoutsPerPage) {
        return false;
    }

    uint32 dwEquippedIndexMap = sGetEquippedLoadoutIndexes();
    return (dwEquippedIndexMap & (1 << nIndex)) != 0;
}

namespace UIEquipmentManager {

void ShowErrorMsg(const Player * ptPlayer, CRefString sMessage)
{
    NotificationCreationParams tParams;
    tParams.uszTitle = StringListFetch("H2OLayout:EquipmentManagerError");
    tParams.uszMessage = sMessage.str();
    tParams.nControllerIndex = ptPlayer != NULL ? ptPlayer->nControllerIndex : 0;
    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}

}  // namespace UIEquipmentManager
