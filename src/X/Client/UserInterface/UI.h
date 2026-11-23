// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UI.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "UIID.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

struct DisplayMode;
struct FormattedTextGlobals;
struct InvGlobals;
struct Rect2D;
struct UIHelpGlobals;
struct UIHeroData;
struct UITextControlGlobals;

typedef int32 GlobalSNO;

enum {
    UIRESULT_UNHANDLED = 0,
    UIRESULT_HANDLED = 1,
};

typedef int32 UIResult;

class UIControl;
class UIControlGroup;
class UIWindow;
class UIText;
class UIButton;
class UIConsoleListBox;
class UIRadioButtonGroup;
class UIScrollbar;
struct UIControlData;
struct UIControlSourceKey;
struct UIDialogDefinition;
struct UIStyle;

typedef blz::vector<UIControlData *, blz::allocator<UIControlData *> > UIControlDataArray;
typedef XGrowableMap<unsigned long, unsigned long, UIStyle *, UIStyle *> UIStyleMap;

// Declarations of the UI.cpp functions this repo's files call; the DWARF gives their
// signatures (UI.cpp) but not where they were declared.
BOOL UI_GetControlData(const UIControlSourceKey & tSrcKey, const UIDialogDefinition * pdefUI, UIControlData *& pControlData);
UIControl & UI_GetControlFromID(const UIID & id);
UIButton & UI_GetButtonFromID(const UIID & id);  // UI.cpp
BOOL UI_TestID(const UIID & id);  // UI.cpp
UIControlGroup & UI_GetControlGroupFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);  // UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);  // UI.cpp:5339
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);  // UI.cpp
UIRadioButtonGroup & UI_GetRadioButtonGroupFromID(const UIID & id);  // UI.cpp
class UIBlinker;
UIBlinker & UI_GetBlinkerFromID(const UIID & id);  // UI.cpp:5739
void UI_SetControllerFocus(const UIID & idFocus);  // UI.cpp:599
int32 UI_GetPrimaryUIController();  // UI.cpp:538
void UI_SetupStyles(UIControlData & pControl, const UIControlDataArray * pChildren, SNO snoImpliedFile, const UIStyleMap * pmapStylesFromImpliedSno);
void UI_InitializeForGame();
void UI_InitializeForWorld();
void UI_SaveState();
void UI_RestoreState();
void UI_DisposeForGame();
void UI_DisposeForWorld();
void UI_Reset(bool bExpectUIRestore);
void UI_SetIsResetting(BOOL isResettingUI);
void UI_RegisterInputListener(UIControl * pControl);  // UI.cpp

class UIProgressBar;
UIProgressBar & UI_GetProgressBarFromID(const UIID &);
UIScrollbar & UI_GetScrollbarFromID(const UIID & id);  // UI.cpp:5685

// UI.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
void UIInitializeForApp();
void UIDisposeForApp();

const UIID & UI_GetMousedOverControl();  // UI.cpp:757

class SafeZone {  // size 0x1
public:
    Rect2D GetMargins() const;
    void SetMargins(const float flLeft, const float flTop, const float flRight, const float flBottom);
    void ApplyToRect(Rect2D & rect);
};

typedef XGrowableMap<unsigned long, const unsigned long &, UIControl *, UIControl *> CControlMap;
typedef blz::vector<UIControl *, blz::allocator<UIControl *> > CControlArray;
// UI only holds a pointer to this map. The XHashKey<UIControl *> specialization it needs when
// instantiated is in UI.cpp, the one file that uses the map.
typedef XGrowableMap<UIControl *, UIControl *, int, int> CControlOnlyMap;

class UI {  // size 0x60
public:
    void Initialize();
    const UIID & GetMousedOverControl() const { return idMousedOverControl; }  // UI.h:250
    void SetMousedOverControl(const UIID & idMousedOverControlNew);
    CControlMap * pmapElements;  // 0x00
    CControlOnlyMap * pmapInputListeners;  // 0x08
    CControlArray * pInputListeners;  // 0x10
    BOOL fInputListenerArrayIsDirty;  // 0x18
    SafeZone tSafeZone;  // 0x1c
    UIID idCapture;  // 0x20
    UIID idKeyboardFocus;  // 0x28
    UIID idVisibleFocus;  // 0x30
    UIID idActivePopupControl;  // 0x38
    UIID idLastClickedControl;  // 0x40
    UIID idMousedOverControl;  // 0x48
    UIID idControllerFocus;  // 0x50
    int32 nPrimaryController;  // 0x58
    uint32 fRootIsSet : 1;  // 0x5c
    uint32 bLeftTriggerBounceForUI : 1;  // 0x5c
    uint32 bRightTriggerBounceForUI : 1;  // 0x5c
};

// CUIStateMap repeats the typedef in Controls/UIControlControl.h.
typedef XGrowableMap<unsigned long, const unsigned long &, int, int> CUIStateMap;
typedef XGrowableList<UIID, const UIID &> UIIDList;
typedef XGrowableMap<unsigned int, unsigned int, int, int> UIFunctionMap;

struct UIAppGlobals {  // size 0x140
    CUIStateMap mapUIState;  // 0x00
    UIIDList listStartupDlgStack;  // 0x80
    UIIDList listInGameDlgStack;  // 0xd8
    UIHeroData * ptHeroData;  // 0x130
    FormattedTextGlobals * ptFormattedTextGlobals;  // 0x138
};
static_assert(sizeof(UIAppGlobals) == 0x140, "UIAppGlobals");

struct UIGlobals {  // size 0xd58
    UI ui;  // 0x00
    UIHelpGlobals * ptHelp;  // 0x60
    UITextControlGlobals * ptTextControlGlobals;  // 0x68
    ACDID idACDMonsterHPBar;  // 0x70
    SNO snoLastClickedMonster;  // 0x74
    int32 nLastClickedMonsterLevel;  // 0x78
    CHAR uszLastClickedMonsterNamePretty[1024];  // 0x7c
    InvGlobals * ptInventory;  // 0x480
    IVector2D tMouseLocation;  // 0x488
    uint32 dwUIQuitDisplayUntil;  // 0x490
    BOOL bUIQuitDialogVisible;  // 0x494
    BOOL bGameSpecialTextPresent;  // 0x498
    BOOL bSkipPreloadChange;  // 0x49c
    CHAR uszGameSpecialText[1024];  // 0x4a0
    CHAR uszGameSpecialTextSubTitle[1024];  // 0x8a0
    int32 nModeCount;  // 0xca0
    DisplayMode * ptModes;  // 0xca8
    const UIID * ptCurrentHotkeyToAssign;  // 0xcb0
    BOOL fInvPageZoomed;  // 0xcb8
    BOOL fConvPageZoomed;  // 0xcbc
    BOOL fManualZoomed;  // 0xcc0
    BOOL fInitialized;  // 0xcc4
    ACDID idACDBossHPBar;  // 0xcc8
    UIFunctionMap tUIFunctionMap;  // 0xcd0
    uint32 dwRiftClosingTick;  // 0xd50
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(SafeZone) == 0x1, "SafeZone");
static_assert(sizeof(UI) == 0x60, "UI");
static_assert(offsetof(UI, pmapElements) == 0x0, "UI");
static_assert(offsetof(UI, pmapInputListeners) == 0x8, "UI");
static_assert(offsetof(UI, pInputListeners) == 0x10, "UI");
static_assert(offsetof(UI, fInputListenerArrayIsDirty) == 0x18, "UI");
static_assert(offsetof(UI, tSafeZone) == 0x1c, "UI");
static_assert(offsetof(UI, idCapture) == 0x20, "UI");
static_assert(offsetof(UI, idKeyboardFocus) == 0x28, "UI");
static_assert(offsetof(UI, idVisibleFocus) == 0x30, "UI");
static_assert(offsetof(UI, idActivePopupControl) == 0x38, "UI");
static_assert(offsetof(UI, idLastClickedControl) == 0x40, "UI");
static_assert(offsetof(UI, idMousedOverControl) == 0x48, "UI");
static_assert(offsetof(UI, idControllerFocus) == 0x50, "UI");
static_assert(offsetof(UI, nPrimaryController) == 0x58, "UI");
static_assert(sizeof(UIAppGlobals) == 0x140, "UIAppGlobals");
static_assert(offsetof(UIAppGlobals, mapUIState) == 0x0, "UIAppGlobals");
static_assert(offsetof(UIAppGlobals, listStartupDlgStack) == 0x80, "UIAppGlobals");
static_assert(offsetof(UIAppGlobals, listInGameDlgStack) == 0xd8, "UIAppGlobals");
static_assert(offsetof(UIAppGlobals, ptHeroData) == 0x130, "UIAppGlobals");
static_assert(offsetof(UIAppGlobals, ptFormattedTextGlobals) == 0x138, "UIAppGlobals");
static_assert(sizeof(UIGlobals) == 0xd58, "UIGlobals");
static_assert(offsetof(UIGlobals, ui) == 0x0, "UIGlobals");
static_assert(offsetof(UIGlobals, ptHelp) == 0x60, "UIGlobals");
static_assert(offsetof(UIGlobals, ptTextControlGlobals) == 0x68, "UIGlobals");
static_assert(offsetof(UIGlobals, idACDMonsterHPBar) == 0x70, "UIGlobals");
static_assert(offsetof(UIGlobals, snoLastClickedMonster) == 0x74, "UIGlobals");
static_assert(offsetof(UIGlobals, nLastClickedMonsterLevel) == 0x78, "UIGlobals");
static_assert(offsetof(UIGlobals, uszLastClickedMonsterNamePretty) == 0x7c, "UIGlobals");
static_assert(offsetof(UIGlobals, ptInventory) == 0x480, "UIGlobals");
static_assert(offsetof(UIGlobals, tMouseLocation) == 0x488, "UIGlobals");
static_assert(offsetof(UIGlobals, dwUIQuitDisplayUntil) == 0x490, "UIGlobals");
static_assert(offsetof(UIGlobals, bUIQuitDialogVisible) == 0x494, "UIGlobals");
static_assert(offsetof(UIGlobals, bGameSpecialTextPresent) == 0x498, "UIGlobals");
static_assert(offsetof(UIGlobals, bSkipPreloadChange) == 0x49c, "UIGlobals");
static_assert(offsetof(UIGlobals, uszGameSpecialText) == 0x4a0, "UIGlobals");
static_assert(offsetof(UIGlobals, uszGameSpecialTextSubTitle) == 0x8a0, "UIGlobals");
static_assert(offsetof(UIGlobals, nModeCount) == 0xca0, "UIGlobals");
static_assert(offsetof(UIGlobals, ptModes) == 0xca8, "UIGlobals");
static_assert(offsetof(UIGlobals, ptCurrentHotkeyToAssign) == 0xcb0, "UIGlobals");
static_assert(offsetof(UIGlobals, fInvPageZoomed) == 0xcb8, "UIGlobals");
static_assert(offsetof(UIGlobals, fConvPageZoomed) == 0xcbc, "UIGlobals");
static_assert(offsetof(UIGlobals, fManualZoomed) == 0xcc0, "UIGlobals");
static_assert(offsetof(UIGlobals, fInitialized) == 0xcc4, "UIGlobals");
static_assert(offsetof(UIGlobals, idACDBossHPBar) == 0xcc8, "UIGlobals");
static_assert(offsetof(UIGlobals, tUIFunctionMap) == 0xcd0, "UIGlobals");
static_assert(offsetof(UIGlobals, dwRiftClosingTick) == 0xd50, "UIGlobals");
