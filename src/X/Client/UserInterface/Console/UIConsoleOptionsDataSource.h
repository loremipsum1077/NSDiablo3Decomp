// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleOptionsDataSource.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIConsoleSelectorControl.h"
#include "../UIID.h"
#include "../../../Sigma/RefString.h"

struct UIControlSourceKey;

namespace UIOptions {
namespace Console {
enum ScreenActions {
    ACTION_SELECT = 0,
    ACTION_ACCEPT = 1,
    ACTION_BACK = 2,
    ACTION_NETWORK_OPTIONS = 3,
};

enum Menus {
    GAMEPLAY = 0,
    VIDEO = 1,
    AUDIO = 2,
    MULTIPLAYER = 3,
    CONTROLLER = 4,
    BATTLENET = 5,
    SUB_MENU_SELECTOR = 6,
    MENUS_COUNT = 7,
};

enum SelectionType {
    TEXT = 0,
    SELECTOR = 1,
    CHECKBOX = 2,
};

enum OptionID {
    NONE = 0,
    GAMEPLAY_TAB = 1,
    VIDEO_TAB = 2,
    AUDIO_TAB = 3,
    MULTIPLAYER_TAB = 4,
    GAMMA_SETTING = 5,
    MUSIC_VOLUME = 6,
    MASTER_VOLUME = 7,
    EFFECTS_VOLUME = 8,
    DIALOGUE_VOLUME = 9,
    AMBIENT_VOLUME = 10,
    CONTROLS = 11,
    BATTLENET_TAB = 12,
    TEXT_LANGUAGE_OVERRIDE = 13,
    AUDIO_LANGUAGE_OVERRIDE = 14,
    ELECTIVE_MODE = 15,
    DISPLAY_PLAYER_HEALTH_BARS = 16,
    DISPLAY_MONSTER_HEALTH_BARS = 17,
    DISPLAY_PLAYER_NAMES = 18,
    DISPLAY_HEALING_NUMBERS = 19,
    DISPLAY_DAMAGE_NUMBERS = 20,
    DISPLAY_CRITICAL_DAMAGE_NUMBERS = 21,
    DISPLAY_SHORT_FLOATING_NUMBERS = 22,
    DISPLAY_DEFENSIVE_MESSAGES = 23,
    ALWAYS_SHOW_ITEM_LABELS_ON_DROP = 24,
    SHOW_TUTORIALS = 25,
    AUTO_EQUIP_ITEMS = 26,
    SHOW_ADVANCED_TOOLTIPS = 27,
    SMART_MINIMAP = 28,
    ALWAYS_SHOW_CAMPAIGN_OBJECTIVES = 29,
    ALWAYS_SHOW_ADVENTURE_OBJECTIVES = 30,
    ITEM_RARITY_ICONS = 31,
    SAFE_ZONE = 32,
    QUEST_DIALOG_SUBTITLES = 33,
    CINEMATIC_SUBTITLES = 34,
    REGION_OVERRIDE = 35,
    REGION_PREFERENCE = 36,
    LANGUAGE_PREFERENCE = 37,
    EQUIPMENT_MANAGER_DROP_TO_STASH = 38,
    OPTIONID_COUNT = 39,
};

class Selection {  // size 0x48
public:
    UIOptions::Console::SelectionType m_eType;  // 0x00
    CRefString m_sDisplayText;  // 0x08
    CRefString m_sToolTip;  // 0x20
    BOOL m_bSelected;  // 0x38
    UIOptions::Console::OptionID m_eOptionID;  // 0x3c
    BOOL m_bChecked;  // 0x40
    BOOL m_bWrap;  // 0x44
    Selection();
    void Initialize(UIOptions::Console::SelectionType type, CRefString sLabel, CRefString sToolTipLabel, UIOptions::Console::OptionID eOptionID);
};
static_assert(sizeof(Selection) == 0x48, "Selection");

typedef blz::vector<UIOptions::Console::Selection, blz::allocator<UIOptions::Console::Selection> > SelectionArray;

class ConsoleOptionsSelectorDataSource : public ConsoleSelectorDataSource {  // size 0x10
public:
    UIOptions::Console::OptionID m_eCurrentOptionID;  // 0x08
    int32 m_nCurrentListItemIndex;  // 0x0c
    ConsoleOptionsSelectorDataSource();
    int32 GetNumItems(UIOptions::Console::OptionID) const;
    virtual int32 GetNumItems() const;  // vtable slot 0
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 1
    virtual void InitializeItemUI(const UIConsoleSelector & tSelector, const UIID & idItemUI, const int32 nIndex);  // vtable slot 2
    virtual void UpdateItemUI(const UIConsoleSelector & tSelector, const UIID & idItemUI, const int32 nIndex);  // vtable slot 3
    void SetItemIndex(UIConsoleSelector & tSelector);
};
static_assert(sizeof(ConsoleOptionsSelectorDataSource) == 0x10, "ConsoleOptionsSelectorDataSource");

class ConsoleOptionsListDataSource : public ConsoleListBoxDataSource {  // size 0xd0
public:
    UIOptions::Console::ConsoleOptionsSelectorDataSource m_selectorData;  // 0x08
    UIOptions::Console::SelectionArray m_tSubMenuSelector;  // 0x18
    UIOptions::Console::SelectionArray m_tGameplay;  // 0x30
    UIOptions::Console::SelectionArray m_tVideo;  // 0x48
    UIOptions::Console::SelectionArray m_tAudio;  // 0x60
    UIOptions::Console::SelectionArray m_tMultiplayer;  // 0x78
    UIOptions::Console::SelectionArray * m_ptActiveList;  // 0x90
    UIOptions::Console::Menus m_eCurrentMenu;  // 0x98
    CRefString m_sListSourceKey;  // 0xa0
    UIID m_idList;  // 0xb8
    UIID m_idTooltip;  // 0xc0
    UIID m_idRActor;  // 0xc8
    ConsoleOptionsListDataSource();
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    void SetUIControls(CRefString sSourceKey, const UIID & idList, const UIID & idTooltip, const UIID & idRActor);
    void RefreshSelections();
};
static_assert(sizeof(ConsoleOptionsListDataSource) == 0xd0, "ConsoleOptionsListDataSource");

}  // namespace Console
}  // namespace UIOptions
