// Original file: X/Client/UserInterface/Console/UIConsoleDesignerMenu.cpp
//
// The console designer (debug) menu: character, quest, game, console, quick-option and music pages.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#line 8
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Contrib/Contrib/blz/include/blz/string.h"
#include "ConsoleInputUtilities.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIConsoleSelectorControl.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Game.h"
#include "../../Sound/Sound.h"
#include "../../../Sigma/RefString.h"
#include "ConsoleScreenManager.h"
#include "../../../Common/Player.h"
#include "../../CGame.h"
#include "../../CWorld.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Common/Definitions/ActDefinition.h"
#include "../../../Common/Act.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/SNO/SNOTOC.h"
#include "../../Console/ControllerUtils.h"
#include "../../../../BlizzardCore/Include/Blizzard/String.h"

#include <string.h>

// Declared here because their headers are not adopted yet; each comment names the defining file.
extern UIID UIDLG_CONSOLEDESIGNERMENU;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_MAINMENU;
extern UIID UIDLG_CONSOLEDESIGNERMENU_MAINMENU_MENUOPTIONSLIST;  // X/Client/UserInterface/UIIDs.cpp  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_CHARACTER_BOOSTOPTIONSLIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_QUEST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_QUEST_LIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_GAME_GAMELIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_CONSOLE_CONSOLELIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_TOGGLELIST;  // X/Client/UserInterface/UIIDs.cpp
extern UIID UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_ACTIONSLIST;  // X/Client/UserInterface/UIIDs.cpp
BOOL UI_TestID(const UIID & id);  // X/Client/UserInterface/UI.cpp
const UIID & UI_GetControllerFocus();  // X/Client/UserInterface/UI.cpp
void UI_SetPrimaryUIController(int32 nPrimaryController);  // X/Client/UserInterface/UI.cpp
CRefString QuestsGetUIName(SNO snoQuest);  // X/Common/Quests/Quests.cpp
CRefString SNOFileGetName(const SNOName & snoname);  // X/Common/SNO/SNOFiles.cpp
CRefString DisplayFormattedHandicap(int32 nHandicap);  // X/Client/Battlenet/UI/Util/UIUtil.cpp

























namespace UIConsoleDesignerMenu {
enum MainMenus {
    MENU_CHARACTER = 0,
    MENU_QUEST = 1,
    MENU_GAME = 2,
    MENU_CONSOLE = 3,
    MENU_QUICK = 4,
    MENU_MUSIC = 5,
    NUM_MENUS = 6,
};

enum SelectionType {
    TEXT = 0,
    SELECTOR = 1,
};

enum SelectionID {
    VOICE_CHAT = 0,
    OTHER = 1,
};

class MenuOptionItem {  // size 0x28
public:
    UIID m_menuID;  // 0x00
    MainMenus m_eMenu;  // 0x08
    CRefString m_displayText;  // 0x10
    MenuOptionItem();
};

class MainMenuListDataSource : public ConsoleListBoxDataSource {  // size 0xf8
public:
    MainMenuListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
    MenuOptionItem m_menuOptionItems[6];  // 0x08
};

class ConsoleCommandOption {  // size 0x40
public:
    CRefString m_displayText;  // 0x00
    CRefString m_command;  // 0x18
    BOOL m_bCloseOnSelect;  // 0x30
    BOOL bToggledOn;  // 0x34
    BOOL bHasBeenSelected;  // 0x38
    ConsoleCommandOption();
};

class MultipleTypeOption {  // size 0x48
public:
    CRefString m_displayText;  // 0x00
    CRefString m_command;  // 0x18
    BOOL m_bCloseOnSelect;  // 0x30
    BOOL bToggledOn;  // 0x34
    BOOL bHasBeenSelected;  // 0x38
    SelectionType eType;  // 0x3c
    SelectionID eID;  // 0x40
    MultipleTypeOption();
};

class CharacterResetHeroListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<ConsoleCommandOption, blz::allocator<ConsoleCommandOption> > m_consoleCommandOptionItems;  // 0x08
    CharacterResetHeroListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
};

class CharacterFollowerListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<ConsoleCommandOption, blz::allocator<ConsoleCommandOption> > m_consoleCommandOptionItems;  // 0x08
    CharacterFollowerListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
};

class CharacterBoostOptionsListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<ConsoleCommandOption, blz::allocator<ConsoleCommandOption> > m_consoleCommandOptionItems;  // 0x08
    CharacterBoostOptionsListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
};

class QuestMenuOption {  // size 0x38
public:
    SNO m_sno;  // 0x00
    CRefString m_displayName;  // 0x08
    CRefString m_data;  // 0x20
    QuestMenuOption()
        : m_sno(-1)
    {
    }
};

class QuestListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<QuestMenuOption, blz::allocator<QuestMenuOption> > m_menuOptions;  // 0x08
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
};

class SelectorDataSource {  // size 0x10 (base ConsoleSelectorDataSource is not in this header yet)
public:
    SelectionID m_currentOptionID;  // 0x08
    int32 m_nCurrentListItemIndex;  // 0x0c
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleSelector &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleSelector &, const UIID &, const int32);
};

class ConsoleListDataSource : public ConsoleListBoxDataSource {  // size 0x30
public:
    blz::vector<MultipleTypeOption, blz::allocator<MultipleTypeOption> > m_MultipleTypeItems;  // 0x08
    SelectorDataSource m_SelectorDataSource;  // 0x20
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
};

class GameListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<ConsoleCommandOption, blz::allocator<ConsoleCommandOption> > m_consoleCommandOptionItems;  // 0x08
    GameListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
};

class QuickToggleListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<ConsoleCommandOption, blz::allocator<ConsoleCommandOption> > m_consoleCommandOptionItems;  // 0x08
    QuickToggleListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
};

class QuickActionsListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<ConsoleCommandOption, blz::allocator<ConsoleCommandOption> > m_consoleCommandOptionItems;  // 0x08
    QuickActionsListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
};

class MusicListDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    blz::vector<ConsoleCommandOption, blz::allocator<ConsoleCommandOption> > m_musicOptionItems;  // 0x08
    MusicListDataSource();
    virtual int32 GetNumItems() const;
    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const;
    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32);
    virtual void UpdateItemUI(const UIConsoleListBox &, const UIID &, const int32);
    void Initialize();
};

enum QuestMenus {
    QuestMenus_Main = 0,
    QuestMenus_Quest_Difficulty = 1,
    QuestMenus_Quest_Act = 2,
    QuestMenus_Quest_Quest = 3,
    QuestMenus_Quest_Step = 4,
    QuestMenus_Transport_Act = 5,
    QuestMenus_Transport_Locations = 6,
    QuestMenus_Boss = 7,
};

class DesignerMenuGlobals {  // size 0x2d8
public:
    MainMenuListDataSource listDataMainMenu;  // 0x00
    ConsoleListDataSource listDataConsoleMenu;  // 0xf8
    GameListDataSource listDataGameMenu;  // 0x128
    QuickToggleListDataSource listDataQuickToggleMenu;  // 0x148
    QuickActionsListDataSource listDataQuickActionsMenu;  // 0x168
    CharacterResetHeroListDataSource listDataCharacterResetHeroMenu;  // 0x188
    CharacterFollowerListDataSource listDataCharacterFollowerMenu;  // 0x1a8
    CharacterBoostOptionsListDataSource listDataCharacterBoostOptionsMenu;  // 0x1c8
    QuestListDataSource listDataQuestMenu;  // 0x1e8
    MusicListDataSource listDataMusicMenu;  // 0x208
    MainMenus m_eCurrentMenu;  // 0x228
    UIID m_currentMenu;  // 0x230
    UIID m_selectedMenu;  // 0x238
    int32 m_consoleMenuSelectionId;  // 0x240
    int32 m_gameMenuSelectionId;  // 0x244
    int32 m_quickToggleMenuSelectionId;  // 0x248
    int32 m_quickActionsMenuSelectionId;  // 0x24c
    int32 m_characterResetHeroMenuSelectionId;  // 0x250
    int32 m_characterFollowersMenuSelectionId;  // 0x254
    int32 m_characterBoostOptionsMenuSelectionId;  // 0x258
    int32 m_musicMenuSelectionId;  // 0x25c
    QuestMenus eCurrentQuestMenu;  // 0x260
    int32 questDifficulty;  // 0x264
    int32 questAct;  // 0x268
    CRefString questQuest;  // 0x270
    CRefString questStep;  // 0x288
    BOOL bMusicChoosingCategory;  // 0x2a0
    ::Console::AnalogStickInputFilter tStickFilter;  // 0x2a8
    BOOL bReseed;  // 0x2d0
    DesignerMenuGlobals();
    ~DesignerMenuGlobals();
};
static_assert(sizeof(DesignerMenuGlobals) == 0x2d8, "DesignerMenuGlobals");

void OnListSelectionChanged(const UIID & id, const int32, const int32 nNewVal);
namespace CharacterMenu { void Open(); void OnButton1(const UIID &, const int32); }
namespace QuestMenu { void Open(); void OnButton1(const UIID &, const int32); }
namespace ConsoleMenu { void Open(); void OnButton1(const UIID &, const int32); }
namespace GameMenu { void Open(); void OnButton1(const UIID &, const int32); }
namespace QuickOptionsMenu { void Open(); void OnButton1(const UIID &, const int32); }
namespace MusicMenu { void Open(); void OnButton1(const UIID &, const int32); void PopulateList(int32); }
























































































































































































































































































































































DesignerMenuGlobals * sgptDesignerMenuGlobals;


inline void ProcessCompoundCommands(LPCSTR uszCommands, int32 nControllerIndex)
{
    CHAR uszTempBuffer[256];
    CHAR * puszCurrentCommand;
    int32 nPlayerIndex;

    Blizzard::String::Copy(uszTempBuffer, uszCommands, sizeof(uszTempBuffer));

    nPlayerIndex = -1;
    if (PlayerGetByControllerIndex(nControllerIndex))
    {
        nPlayerIndex = PlayerGetByControllerIndex(nControllerIndex)->nPlayerIndex;
    }

    puszCurrentCommand = uszTempBuffer;
    while (*puszCurrentCommand)
    {

        CHAR * pucBreak = strchr(puszCurrentCommand, ';');
        if (pucBreak)
        {
            *pucBreak = 0;
        }













        puszCurrentCommand += strlen(puszCurrentCommand);
        if (pucBreak) puszCurrentCommand++;
    }
}















































































































inline int32 CharacterResetHeroListDataSource::GetNumItems() const
{
    return m_consoleCommandOptionItems.size();
}










































































































inline int32 CharacterFollowerListDataSource::GetNumItems() const
{
    return m_consoleCommandOptionItems.size();
}













































































inline int32 CharacterBoostOptionsListDataSource::GetNumItems() const
{
    return m_consoleCommandOptionItems.size();
}








































































































































































































































































































inline ConsoleCommandOption::ConsoleCommandOption()
    : m_bCloseOnSelect(FALSE), bToggledOn(FALSE), bHasBeenSelected(FALSE)
{
}













inline int32 ConsoleListDataSource::GetNumItems() const
{
    return m_MultipleTypeItems.size();
}













































































































































inline int32 GameListDataSource::GetNumItems() const
{
    return m_consoleCommandOptionItems.size();
}

























































































































































































































































inline int32 QuickActionsListDataSource::GetNumItems() const
{
    return m_consoleCommandOptionItems.size();
}








































































































































































































void Open(int32 nControllerIndex)

{
    if (UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU).IsActive())
        return;
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_MAINMENU_MENUOPTIONSLIST);
    tListBox.SetOnSelChangeFunction(OnListSelectionChanged);
    tListBox.SetDataSource(sgptDesignerMenuGlobals->listDataMainMenu);
    tListBox.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));

    UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU);
    UI_SetPrimaryUIController(nControllerIndex);

    UI_GetWindowFromID(sgptDesignerMenuGlobals->m_currentMenu).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU).SetActive(TRUE);
    UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU_MAINMENU).SetActive(TRUE);

    sgptDesignerMenuGlobals->m_currentMenu = UIDLG_CONSOLEDESIGNERMENU_MAINMENU;

    UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Close", FALSE);


    for (int32 i = 0; i < sgptDesignerMenuGlobals->listDataConsoleMenu.GetNumItems(); i++)
    {
        sgptDesignerMenuGlobals->listDataConsoleMenu.m_MultipleTypeItems[i].bHasBeenSelected = FALSE;
    }
    for (int32 i = 0; i < sgptDesignerMenuGlobals->listDataGameMenu.GetNumItems(); i++)
    {
        sgptDesignerMenuGlobals->listDataGameMenu.m_consoleCommandOptionItems[i].bHasBeenSelected = FALSE;
    }
    for (int32 i = 0; i < sgptDesignerMenuGlobals->listDataCharacterBoostOptionsMenu.GetNumItems(); i++)
    {
        sgptDesignerMenuGlobals->listDataCharacterBoostOptionsMenu.m_consoleCommandOptionItems[i].bHasBeenSelected = FALSE;
    }
    for (int32 i = 0; i < sgptDesignerMenuGlobals->listDataCharacterFollowerMenu.GetNumItems(); i++)
    {
        sgptDesignerMenuGlobals->listDataCharacterFollowerMenu.m_consoleCommandOptionItems[i].bHasBeenSelected = FALSE;
    }
    for (int32 i = 0; i < sgptDesignerMenuGlobals->listDataCharacterResetHeroMenu.GetNumItems(); i++)
    {
        sgptDesignerMenuGlobals->listDataCharacterResetHeroMenu.m_consoleCommandOptionItems[i].bHasBeenSelected = FALSE;
    }
    for (int32 i = 0; i < sgptDesignerMenuGlobals->listDataQuickActionsMenu.GetNumItems(); i++)
    {
        sgptDesignerMenuGlobals->listDataQuickActionsMenu.m_consoleCommandOptionItems[i].bHasBeenSelected = FALSE;
    }

    sgptDesignerMenuGlobals->bReseed = FALSE;


    GamePause(TRUE);
}



void Close()
{

    if (!UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU).IsActive())
        return;

    UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU_MAINMENU).SetActive(FALSE);







    GamePause(FALSE);
}






void OnListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{


    if (UI_TestID(id))
    {

        sgptDesignerMenuGlobals->m_selectedMenu = sgptDesignerMenuGlobals->listDataMainMenu.m_menuOptionItems[nNewVal].m_menuID;
        sgptDesignerMenuGlobals->m_eCurrentMenu = sgptDesignerMenuGlobals->listDataMainMenu.m_menuOptionItems[nNewVal].m_eMenu;
    }
}
void OnButton1(const UIID & id, const int32 nControllerIndex)


{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);


    if (sgptDesignerMenuGlobals->m_currentMenu == UIDLG_CONSOLEDESIGNERMENU_MAINMENU)
    {
        UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU_MAINMENU).SetActive(FALSE);
        UI_GetWindowFromID(sgptDesignerMenuGlobals->m_selectedMenu).SetActive(TRUE);
        sgptDesignerMenuGlobals->m_currentMenu = sgptDesignerMenuGlobals->m_selectedMenu;

        UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Back", FALSE);


        switch (sgptDesignerMenuGlobals->m_eCurrentMenu) {
        case MENU_CHARACTER:
            CharacterMenu::Open();
            break;

        case MENU_QUEST:
            QuestMenu::Open();
            break;

        case MENU_CONSOLE:
            ConsoleMenu::Open();
            break;

        case MENU_GAME:
            GameMenu::Open();
            break;

        case MENU_QUICK:
            QuickOptionsMenu::Open();
            break;

        case MENU_MUSIC:
            MusicMenu::Open();
            break;
        }
    }
    else
    {
        UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Accept Changes and Back", FALSE);


        switch (sgptDesignerMenuGlobals->m_eCurrentMenu) {
        case MENU_CHARACTER:
            CharacterMenu::OnButton1(id, nControllerIndex);
            break;

        case MENU_QUEST:
            UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Back", FALSE);
            QuestMenu::OnButton1(id, nControllerIndex);
            break;

        case MENU_CONSOLE:
            ConsoleMenu::OnButton1(id, nControllerIndex);
            break;

        case MENU_GAME:
            GameMenu::OnButton1(id, nControllerIndex);
            break;

        case MENU_QUICK:
            QuickOptionsMenu::OnButton1(id, nControllerIndex);
            break;

        case MENU_MUSIC:
            MusicMenu::OnButton1(id, nControllerIndex);
        }
    }
}




void OnButton2(const UIID &, const int32)
{

    if (sgptDesignerMenuGlobals->m_currentMenu == UIDLG_CONSOLEDESIGNERMENU_MAINMENU)
    {
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
    }
    else
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);

        UI_GetWindowFromID(sgptDesignerMenuGlobals->m_currentMenu).SetActive(FALSE);
        UI_GetWindowFromID(UIDLG_CONSOLEDESIGNERMENU_MAINMENU).SetActive(TRUE);

        UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Close", FALSE);

        sgptDesignerMenuGlobals->m_currentMenu = UIDLG_CONSOLEDESIGNERMENU_MAINMENU;
    }
}

inline void QuickMenuSwitchListsLeft()
{


    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

    UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_TOGGLELIST);

    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_ACTIONSLIST).ForceCallToSelectionChange();
    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_TOGGLELIST).ForceCallToSelectionChange();
}
inline void QuickMenuSwitchListsRight()
{


    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);

    UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_ACTIONSLIST);

    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_ACTIONSLIST).ForceCallToSelectionChange();
    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_TOGGLELIST).ForceCallToSelectionChange();
}


void CharacterMenuSwitchListsLeft()
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);


    if (UI_GetControllerFocus() == UIDLG_CONSOLEDESIGNERMENU_CHARACTER_BOOSTOPTIONSLIST)
    {
        UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST);
    }
    else if (UI_GetControllerFocus() == UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST)
    {
        UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST);
    }



    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST).ForceCallToSelectionChange();
    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST).ForceCallToSelectionChange();
    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_BOOSTOPTIONSLIST).ForceCallToSelectionChange();
}

void CharacterMenuSwitchListsRight()
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_GENERIC_FOCUS_SWITCH), cgtPSODefaults);


    if (UI_GetControllerFocus() == UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST)
    {
        UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST);
    }
    else if (UI_GetControllerFocus() == UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST)
    {
        UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_BOOSTOPTIONSLIST);
    }



    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST).ForceCallToSelectionChange();
    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST).ForceCallToSelectionChange();
    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_BOOSTOPTIONSLIST).ForceCallToSelectionChange();
}


void OnLStick(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() == nControllerIndex)
    {





        ::Console::AnalogStickInputFilter::Rule::ID idRule = sgptDesignerMenuGlobals->tStickFilter.HandleInput(::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE), FALSE);

        switch (sgptDesignerMenuGlobals->m_eCurrentMenu)
        {
        case MENU_QUICK:
            switch (idRule)
            {
            case ::Console::RULE_STICK_LEFT:
                QuickMenuSwitchListsLeft();
                break;

            case ::Console::RULE_STICK_RIGHT:
                QuickMenuSwitchListsRight();
                break;
            }
            break;

        case MENU_CHARACTER:
            switch (idRule)
            {
            case ::Console::RULE_STICK_LEFT:
                CharacterMenuSwitchListsLeft();
                break;

            case ::Console::RULE_STICK_RIGHT:
                CharacterMenuSwitchListsRight();
                break;
            }
            break;
        }
    }
}











namespace CharacterMenu {
void OnButton1(const UIID &, const int32 nControllerIndex)
{


    if (UI_GetControllerFocus() == UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST)
    {
        ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataCharacterResetHeroMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_characterResetHeroMenuSelectionId].m_command, nControllerIndex);
        sgptDesignerMenuGlobals->listDataCharacterResetHeroMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_characterResetHeroMenuSelectionId].bHasBeenSelected = TRUE;
    }
    else if (UI_GetControllerFocus() == UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST)
    {
        ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataCharacterFollowerMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_characterFollowersMenuSelectionId].m_command, nControllerIndex);
        sgptDesignerMenuGlobals->listDataCharacterFollowerMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_characterFollowersMenuSelectionId].bHasBeenSelected = TRUE;
    }
    else
    {

        ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataCharacterBoostOptionsMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_characterBoostOptionsMenuSelectionId].m_command, nControllerIndex);
        sgptDesignerMenuGlobals->listDataCharacterBoostOptionsMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_characterBoostOptionsMenuSelectionId].bHasBeenSelected = TRUE;
    }
}






void OnResetHeroListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{


    if (UI_TestID(id))
    {
        sgptDesignerMenuGlobals->m_characterResetHeroMenuSelectionId = nNewVal;
    }
}

void OnFollowersListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{


    if (UI_TestID(id))
    {
        sgptDesignerMenuGlobals->m_characterFollowersMenuSelectionId = nNewVal;
    }
}

void OnBoostOptionsListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{


    if (UI_TestID(id))
    {
        sgptDesignerMenuGlobals->m_characterBoostOptionsMenuSelectionId = nNewVal;
    }
}



void Open()
{

    UIConsoleListBox & tResetHeroList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST);
    tResetHeroList.SetOnSelChangeFunction(OnResetHeroListSelectionChanged);
    tResetHeroList.SetDataSource(sgptDesignerMenuGlobals->listDataCharacterResetHeroMenu);
    tResetHeroList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));

    UIConsoleListBox & tFollowerList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_FOLLOWERLIST);
    tFollowerList.SetOnSelChangeFunction(OnFollowersListSelectionChanged);
    tFollowerList.SetDataSource(sgptDesignerMenuGlobals->listDataCharacterFollowerMenu);
    tFollowerList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));

    UIConsoleListBox & tBoostList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_BOOSTOPTIONSLIST);
    tBoostList.SetOnSelChangeFunction(OnBoostOptionsListSelectionChanged);
    tBoostList.SetDataSource(sgptDesignerMenuGlobals->listDataCharacterBoostOptionsMenu);
    tBoostList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));


    UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_CHARACTER_RESETHEROLIST);
    tResetHeroList.ForceCallToSelectionChange();
    tFollowerList.ForceCallToSelectionChange();
    tBoostList.ForceCallToSelectionChange();
}







void OnDPadLeft(const UIID &, const int32)
{


    CharacterMenuSwitchListsLeft();
}

void OnDPadRight(const UIID &, const int32)
{


    CharacterMenuSwitchListsRight();
}
}  // namespace CharacterMenu









namespace QuestMenu {
void SetupMain()
{
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();

    sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Main;

    QuestMenuOption tOption;


    tOption.m_displayName = "Location";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Game Selection";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Boss Selection";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Quest Advance";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);
}
void SetupBoss()
{
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();
    sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Boss;

    QuestMenuOption tOption;

    tOption.m_displayName = "Greed";
    tOption.m_data = "bossencounter p1_Greed";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Skeleton King";
    tOption.m_data = "bossencounter SkeletonKing";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Spider Queen";
    tOption.m_data = "bossencounter SpiderQueen";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Butcher";
    tOption.m_data = "bossencounter Butcher";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Maghda";
    tOption.m_data = "bossencounter Maghda";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Zoltun Kulle";
    tOption.m_data = "bossencounter A2_BlackSoulstone";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Belial";
    tOption.m_data = "belial";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Ghom";
    tOption.m_data = "bossencounter Gluttony";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Siegebreaker";
    tOption.m_data = "bossencounter Siegebreaker";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Mistress of Pain";
    tOption.m_data = "bossencounter MistressOfPain";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Azmodan";
    tOption.m_data = "bossencounter Azmodan";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Iskatu (1000 Monster Fight)";
    tOption.m_data = "bossencounter A4_1000MonsterFight";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Despair";
    tOption.m_data = "bossencounter Despair";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Izual";
    tOption.m_data = "bossencounter Izual";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Diablo";
    tOption.m_data = "bossencounter Diablo";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Adria";
    tOption.m_data = "bossencounter x1_Adria";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Battering Ram";
    tOption.m_data = "bossencounter X1_BatteringRam";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Urzael";
    tOption.m_data = "bossencounter x1_Urzael";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Malthael";
    tOption.m_data = "bossencounter x1_fortress_MalthaelFight";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Uber Diablo";
    tOption.m_data = "bossencounter Uber_Diablo";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);
}



void SetupTransportAct()
{
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();

    sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Transport_Act;

    QuestMenuOption tOption;

    tOption.m_displayName = "Act I";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Act II";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Act III";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Act IV";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Act V";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

    tOption.m_displayName = "Misc";
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);
}



































































void SetupQuestDifficulty()
{
    sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Quest_Difficulty;

    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();

    QuestMenuOption mo;

    for (int32 nDifficulty = HC_NORMAL; nDifficulty < HC_NUM_HANDICAP_LEVELS; nDifficulty++)
    {
        mo.m_displayName = DisplayFormattedHandicap(nDifficulty);
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
    }
}

void SetupQuestAct()
{
    if (sgptDesignerMenuGlobals->eCurrentQuestMenu == QuestMenus_Quest_Difficulty)
    {



        sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Quest_Act;
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();

        QuestMenuOption tOption;


        tOption.m_displayName = "Act I";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

        tOption.m_displayName = "Act II";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

        tOption.m_displayName = "Act III";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

        tOption.m_displayName = "Act IV";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);

        tOption.m_displayName = "Act V";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(tOption);
    } }
void SetupQuestQuest(int32 actNum)

{
    if (sgptDesignerMenuGlobals->eCurrentQuestMenu == QuestMenus_Quest_Act)
    {


        sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Quest_Quest;

        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();


        Act eAct = (Act)(actNum * 100);
        SNO snoAct = ActSNOFromActEnum(eAct);
        if (snoAct == SNO_NONE)
            return;




        ScopedActDefinitionLockR ptDefAct(snoAct);
        int32 nNumQuests = ActGetNumQuests(ptDefAct);


        for (int32 i = 0; i < nNumQuests; i++)
        {
            SNO snoQuest = ptDefAct->arQuests[i].snoQuest;
            if (snoQuest != SNO_NONE)
            {
                ScopedQuestDefinitionLockR pdefQuest(snoQuest);

                CRefString sQuestName;
                sQuestName.Format("%s  (%s)", SNOToString(QUEST_SNO, snoQuest, FALSE).str(), QuestsGetUIName(snoQuest).str());


                QuestMenuOption mo;

                mo.m_displayName = sQuestName;
                mo.m_sno = pdefQuest->tHeader.snoHandle;
                mo.m_data.Format(SNOToString(QUEST_SNO, snoQuest, FALSE).str());
                sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
            }
        }
    } }

void SetupQuestStep(int32 questNum)
{

    if (sgptDesignerMenuGlobals->eCurrentQuestMenu == QuestMenus_Quest_Quest)
    {



        sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Quest_Step;

        ScopedQuestDefinitionLockR pdefQuest(sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions[questNum].m_sno);

        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();


        for (int32 i = 0; i < pdefQuest->nNumQuestSteps; i++)
        {
            QuestStep & tQuestStep = pdefQuest->arQuestSteps[i];
            if (tQuestStep.dwFlags & 1)
            {
                QuestMenuOption mo;

                mo.m_displayName = tQuestStep.szStepName_VariableArray;
                mo.m_sno = pdefQuest->tHeader.snoHandle;
                mo.m_data.Format("tQuestStep.szStepName");
                sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
            }
        }
    }
}














































































































































void OnListSelectionChanged(const UIID & id, const int32, const int32)
{
}






void Open()
{
    sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Main;
    SetupMain();

    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUEST_LIST);
    tList.SetOnSelChangeFunction(OnListSelectionChanged);
    tList.SetDataSource(sgptDesignerMenuGlobals->listDataQuestMenu);
    tList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));
    tList.ResetAllItems();

    UIID idSubMenuText, idTitleText;
    idSubMenuText.SetFromParent(UIDLG_CONSOLEDESIGNERMENU_QUEST, "QuestSubMenuText");
    idTitleText.SetFromParent(UIDLG_CONSOLEDESIGNERMENU_QUEST, "QuestTitleText");
    if (UI_TestID(idSubMenuText) && UI_TestID(idTitleText))
    {
        UI_GetTextFromID(idTitleText).SetText("Quest Menu", FALSE);
        UI_GetTextFromID(idSubMenuText).SetText("", FALSE);
    }
}
}  // namespace QuestMenu























namespace ConsoleMenu {
void OnButton1(const UIID &, const int32 nControllerIndex)
{


    MultipleTypeOption & tCmd = sgptDesignerMenuGlobals->listDataConsoleMenu.m_MultipleTypeItems[sgptDesignerMenuGlobals->m_consoleMenuSelectionId];
    if (tCmd.m_bCloseOnSelect)
    {
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
    }



    if (strcmp(sgptDesignerMenuGlobals->listDataConsoleMenu.m_MultipleTypeItems[sgptDesignerMenuGlobals->m_consoleMenuSelectionId].m_displayText, "Gamma Lower") == 0)
    {
        tCmd.m_command.Format("gamma %f", Prefs::GetVideoPrefs().GetGamma() - 0.1f);
    }

    else if (strcmp(sgptDesignerMenuGlobals->listDataConsoleMenu.m_MultipleTypeItems[sgptDesignerMenuGlobals->m_consoleMenuSelectionId].m_displayText, "Gamma Higher") == 0)
    {
        tCmd.m_command.Format("gamma %f", Prefs::GetVideoPrefs().GetGamma() + 0.1f);
    }


    if (tCmd.eType == TEXT)
    {
        ProcessCompoundCommands(tCmd.m_command, nControllerIndex);
        tCmd.bHasBeenSelected = TRUE;
    }


    UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CONSOLE_CONSOLELIST).ForceCallToSelectionChange();
}
}  // namespace ConsoleMenu

namespace ConsoleMenu {
void OnListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{


    if (UI_TestID(id))
        sgptDesignerMenuGlobals->m_consoleMenuSelectionId = nNewVal;
}



void Open()
{
    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_CONSOLE_CONSOLELIST);
    tList.SetOnSelChangeFunction(OnListSelectionChanged);
    tList.SetDataSource(sgptDesignerMenuGlobals->listDataConsoleMenu);
    tList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));
    tList.ForceCallToSelectionChange();
    tList.ResetAllItems();
}
}  // namespace ConsoleMenu























namespace GameMenu {
void OnButton1(const UIID &, const int32 nControllerIndex)
{



    ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataGameMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_gameMenuSelectionId].m_command, nControllerIndex);
    sgptDesignerMenuGlobals->listDataGameMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_gameMenuSelectionId].bHasBeenSelected = TRUE;
}






void OnListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{


    if (UI_TestID(id))
    {
        sgptDesignerMenuGlobals->m_gameMenuSelectionId = nNewVal;

        if (nNewVal >= 0)
        {




            if (strcmp(sgptDesignerMenuGlobals->listDataGameMenu.m_consoleCommandOptionItems[nNewVal].m_command, "dropall") == 0)
            {
                GamePause(FALSE);
            }
            else
            {
                GamePause(TRUE);
            }
        } }
}

void Open()
{
    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_GAME_GAMELIST);
    tList.SetOnSelChangeFunction(OnListSelectionChanged);
    tList.SetDataSource(sgptDesignerMenuGlobals->listDataGameMenu);
    tList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));
    tList.ResetAllItems();
}
}  // namespace GameMenu

namespace QuickOptionsMenu {
























void OnButton1(const UIID &, const int32 nControllerIndex)
{

    sgptDesignerMenuGlobals->listDataQuickToggleMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_quickToggleMenuSelectionId].bToggledOn =
        !sgptDesignerMenuGlobals->listDataQuickToggleMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_quickToggleMenuSelectionId].bToggledOn;


    if (UI_GetControllerFocus() == UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_TOGGLELIST)
    {
        ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataQuickToggleMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_quickToggleMenuSelectionId].m_command, nControllerIndex);
    }
    else
    {

        if (strcmp(sgptDesignerMenuGlobals->listDataQuickActionsMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_quickActionsMenuSelectionId].m_displayText.str(), "Reseed DRLG") == 0)
        {

            sgptDesignerMenuGlobals->bReseed = TRUE;
        }
        else
        {

            ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataQuickActionsMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_quickActionsMenuSelectionId].m_command, nControllerIndex);
        }

        sgptDesignerMenuGlobals->listDataQuickActionsMenu.m_consoleCommandOptionItems[sgptDesignerMenuGlobals->m_quickActionsMenuSelectionId].bHasBeenSelected = TRUE;
    }
}



void OnToggleListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{


    if (UI_TestID(id))
    {
        sgptDesignerMenuGlobals->m_quickToggleMenuSelectionId = nNewVal;
    }
}

void OnActionsListSelectionChanged(const UIID & id, const int32, const int32 nNewVal)
{


    if (UI_TestID(id))
    {
        sgptDesignerMenuGlobals->m_quickActionsMenuSelectionId = nNewVal;
    }
}



void Open()
{
    UIConsoleListBox & tToggleList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_TOGGLELIST);
    tToggleList.SetOnSelChangeFunction(OnToggleListSelectionChanged);
    tToggleList.SetDataSource(sgptDesignerMenuGlobals->listDataQuickToggleMenu);
    tToggleList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));

    UIConsoleListBox & tActionsList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_ACTIONSLIST);
    tActionsList.SetOnSelChangeFunction(OnActionsListSelectionChanged);
    tActionsList.SetDataSource(sgptDesignerMenuGlobals->listDataQuickActionsMenu);
    tActionsList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));

    UI_SetControllerFocus(UIDLG_CONSOLEDESIGNERMENU_QUICKOPTIONS_TOGGLELIST);
    tActionsList.ForceCallToSelectionChange();
    tToggleList.ForceCallToSelectionChange();
}







void OnDPadLeft(const UIID &, const int32)
{


    QuickMenuSwitchListsLeft();
}

void OnDPadRight(const UIID &, const int32)
{


    QuickMenuSwitchListsRight();
}
}  // namespace QuickOptionsMenu

namespace MusicMenu {





void PopulateList(Act eAct)
{
    sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems.clear();

    ConsoleCommandOption co;

    PooledSNONameList listSNOFiles(GetSnoNameListPool());
    SNOFileEnumerateSNOsForGroup(MUSIC_SNO, &listSNOFiles);
    POS posIDSno = listSNOFiles.GetHeadPosition();

    while (posIDSno)
    {
        SNOName & snonameFile = listSNOFiles.GetNext(posIDSno);
        CRefString sFileName = SNOFileGetName(snonameFile);
        co.m_displayText.Format(sFileName);
        co.m_command.Format("music %s", sFileName.str());
        BOOL bAddToList;
        blz::string sFileNameString(sFileName.str());
        if (sFileNameString.find("Music_A1") != blz::string::npos)

            bAddToList = (eAct == ACT_1);

        else if (sFileNameString.find("Music_A2") != blz::string::npos)

            bAddToList = (eAct == ACT_2);

        else if (sFileNameString.find("Music_A3") != blz::string::npos)

            bAddToList = (eAct == ACT_3);

        else if (sFileNameString.find("Music_A4") != blz::string::npos)

            bAddToList = (eAct == ACT_4);

        else if (sFileNameString.find("Music_A5") != blz::string::npos)

            bAddToList = (eAct == ACT_5);

        else

            bAddToList = (eAct > ACT_5);


        if (bAddToList)

            sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems.push_back(co);

    }
}


void PopulateCategoryList()
{
    sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems.clear();

    ConsoleCommandOption tOption;

    for (int32 i = 1; i <= 4; i++)
    {
        tOption.m_displayText.Format("Act %i", i);
        sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems.push_back(tOption);
    }
    tOption.m_displayText = "Misc";
    sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems.push_back(tOption);
    tOption.m_displayText = "Restore Scene Music";
    sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems.push_back(tOption);

    sgptDesignerMenuGlobals->bMusicChoosingCategory = TRUE;
}










void OnButton1(const UIID &, const int32 nControllerIndex)
{



    CGame & tCGame = CGameGet();
    if (sgptDesignerMenuGlobals->bMusicChoosingCategory)
    {
        if (sgptDesignerMenuGlobals->m_musicMenuSelectionId == 5)
        {

            tCGame.ptCWorldGlobals->fMusicDisabled = FALSE;
        }
        else
        {

            PopulateList(sgptDesignerMenuGlobals->m_musicMenuSelectionId * 100);
            sgptDesignerMenuGlobals->bMusicChoosingCategory = FALSE;

            UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_GAME_GAMELIST);
            tListBox.ResetAllItems();
            sgptDesignerMenuGlobals->m_musicMenuSelectionId = 0;
        }
    }
    else
    {

        tCGame.ptCWorldGlobals->fMusicDisabled = TRUE;

        ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems[sgptDesignerMenuGlobals->m_musicMenuSelectionId].m_command, nControllerIndex);
        sgptDesignerMenuGlobals->listDataMusicMenu.m_musicOptionItems[sgptDesignerMenuGlobals->m_musicMenuSelectionId].bHasBeenSelected = TRUE;
    }
}




void OnListSelectionChanged(const UIID &, const int32, const int32 nNewVal)
{


    sgptDesignerMenuGlobals->m_musicMenuSelectionId = nNewVal;
}




void Open()
{
    PopulateCategoryList();

    UIConsoleListBox & tList = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_GAME_GAMELIST);
    tList.SetOnSelChangeFunction(OnListSelectionChanged);
    tList.SetDataSource(sgptDesignerMenuGlobals->listDataMusicMenu);
    tList.SetAnalogStickInputFilter(::Console::GetDefaultStickInputFilter(::Console::FILTER_DIRECTIONAL));
    tList.ResetAllItems();
}
}  // namespace MusicMenu

}  // namespace UIConsoleDesignerMenu

#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
// GameBalanceDefinition.h: original DWARF declarations.
const GameBalanceDefinition * HelpCodesTableLockR();
void HelpCodesTableUnlockR(const GameBalanceDefinition * lpDef);
namespace UIConsoleDesignerMenu {
namespace QuestMenu {
// Inference: act selection is expressed with consecutive rejecting filters; DWARF2521
// and the original branch targets converge on the single option construction.
#line 2485 "X/Client/UserInterface/Console/UIConsoleDesignerMenu.cpp"
void SetupTransportLocations(int32 actNum)
{
    sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.clear();
    sgptDesignerMenuGlobals->eCurrentQuestMenu = QuestMenus_Transport_Locations;
    if (actNum * 100 <= ACT_5)
    {
#line 2496
        Act eAct = actNum * 100;
#line 2501
        XGrowableList<UIID, const UIID &> listControls;
        XGrowableList<const char *, const char *> listLabels;
        XGrowableList<const char *, const char *> listCommands;
#line 2505
        const GameBalanceDefinition * ptHelpCodesTableDef = HelpCodesTableLockR();
        const HelpCodeTable & tHelpCodesTable = ptHelpCodesTableDef->tHelpCodesTable;
        int32 nItems = tHelpCodesTable.serHelpCodes.dwSize / sizeof(HelpCode);
#line 2509
        for (int32 i = 0; i < nItems; ++i)
        {
            if (eAct == ACT_1 && strcmp(tHelpCodesTable.ptHelpCodes[i].szControl, "help_combobox_transport_actI") != 0)
                continue;
            if (eAct == ACT_2 && strcmp(tHelpCodesTable.ptHelpCodes[i].szControl, "help_combobox_transport_actII") != 0)
                continue;
            if (eAct == ACT_3 && strcmp(tHelpCodesTable.ptHelpCodes[i].szControl, "help_combobox_transport_actIII") != 0)
                continue;
            if (eAct == ACT_4 && strcmp(tHelpCodesTable.ptHelpCodes[i].szControl, "help_combobox_transport_actIV") != 0)
                continue;
            if (eAct == ACT_5 && strcmp(tHelpCodesTable.ptHelpCodes[i].szControl, "help_combobox_transport_actV") != 0)
                continue;
#line 2521
            QuestMenuOption mo;
            mo.m_displayName = tHelpCodesTable.ptHelpCodes[i].szDisplay;
            mo.m_data = tHelpCodesTable.ptHelpCodes[i].szCommand;
            sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
        }
        HelpCodesTableUnlockR(ptHelpCodesTableDef);
    }
    else
    {
#line 2531
        QuestMenuOption mo;
        mo.m_displayName = "Combat World";
        mo.m_data = "world qa_combatworld";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
        mo.m_displayName = "Flat World";
        mo.m_data = "world \"Axe Flatworld\"";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
        mo.m_displayName = "Boss World";
        mo.m_data = "world bossworldqa";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
        mo.m_displayName = "Trials";
        mo.m_data = "world x1_test_challengetestworld";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
        mo.m_displayName = "Set Dungeons";
        mo.m_data = "world \"Axe FlatWorld_SetDungeons\"";
        sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.push_back(mo);
    }
}
// Inference: early returns spell the original common epilogue paths; bRefreshList
// starts true and only the questadvance case suppresses ResetAllItems.
#line 2681
void OnButton1(const UIID &, const int32 nControllerIndex)
{
    if (!UI_TestID(UIDLG_CONSOLEDESIGNERMENU_QUEST_LIST))
        return;
#line 2686
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UIDLG_CONSOLEDESIGNERMENU_QUEST_LIST);
    int32 nIndex = tListBox.GetSelectedIndex();
    if (tListBox.GetNumItems() == 0 || nIndex >= sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions.size())
        return;
#line 2696
    UIID idHeaderName, idTitleName;
    idHeaderName.SetFromParent(UIDLG_CONSOLEDESIGNERMENU_QUEST, "QuestSubMenuText");
    idTitleName.SetFromParent(UIDLG_CONSOLEDESIGNERMENU_QUEST, "QuestTitleText");
    if (!UI_TestID(idHeaderName) || !UI_TestID(idTitleName))
        return;
#line 2704
    BOOL bRefreshList = TRUE;
    UI_GetTextFromID(idTitleName).SetText(sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions[nIndex].m_displayName, FALSE);
    switch (sgptDesignerMenuGlobals->eCurrentQuestMenu)
    {
    case QuestMenus_Main:
        switch (nIndex)
        {
        case 0:
            UI_GetTextFromID(idTitleName).SetText("Location", FALSE);
            UI_GetTextFromID(idHeaderName).SetText("Act", FALSE);
            SetupTransportAct();
            break;
        case 1:
            UI_GetTextFromID(idTitleName).SetText("Difficulty Selection", FALSE);
            UI_GetTextFromID(idHeaderName).SetText("Difficulty", FALSE);
            SetupQuestDifficulty();
            break;
        case 2:
            UI_GetTextFromID(idTitleName).SetText("Boss Selection", FALSE);
            UI_GetTextFromID(idHeaderName).SetText("", FALSE);
            SetupBoss();
            break;
        case 3:
            ProcessCompoundCommands("questadvance", nControllerIndex);
            bRefreshList = FALSE;
            break;
        }
        break;
    case QuestMenus_Quest_Difficulty:
        sgptDesignerMenuGlobals->questDifficulty = nIndex;
        UI_GetTextFromID(idHeaderName).SetText("Act", FALSE);
        SetupQuestAct();
        break;
    case QuestMenus_Quest_Act:
        sgptDesignerMenuGlobals->questAct = nIndex + 1;
        UI_GetTextFromID(idHeaderName).SetText("Quest", FALSE);
        SetupQuestQuest(nIndex);
        break;
    case QuestMenus_Quest_Quest:
        sgptDesignerMenuGlobals->questQuest = sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions[nIndex].m_data;
        UI_GetTextFromID(idHeaderName).SetText("Quest Step", FALSE);
        SetupQuestStep(nIndex);
        break;
    case QuestMenus_Quest_Step:
        {
#line 2781
            CRefString command;
            command.Format("setdifficulty %i", sgptDesignerMenuGlobals->questDifficulty);
            ProcessCompoundCommands(command, nControllerIndex);
#line 2785
            Act eAct = GameGetCurrentAct();
            if ((sgptDesignerMenuGlobals->questAct - 1) * 100 != eAct)
            {
                command.Format("act A%i", sgptDesignerMenuGlobals->questAct);
                ProcessCompoundCommands(command, nControllerIndex);
            }
            command.Format("questadvance %s %s", sgptDesignerMenuGlobals->questQuest.str(), sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions[nIndex].m_displayName.str());
            ProcessCompoundCommands(command, nControllerIndex);
            SetupMain();
            UI_GetTextFromID(idTitleName).SetText("Quest Menu", FALSE);
            UI_GetTextFromID(idHeaderName).SetText("", FALSE);
            UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Accept Changes and Back", FALSE);
            break;
        }
    case QuestMenus_Transport_Act:
        UI_GetTextFromID(idHeaderName).SetText("Locations", FALSE);
        SetupTransportLocations(nIndex);
        break;
    case QuestMenus_Transport_Locations:
        ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions[nIndex].m_data, nControllerIndex);
        UI_GetTextFromID(idTitleName).SetText("Quest Menu", FALSE);
        UI_GetTextFromID(idHeaderName).SetText("", FALSE);
        UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Accept Changes and Back", FALSE);
        SetupMain();
        break;
    case QuestMenus_Boss:
        ProcessCompoundCommands(sgptDesignerMenuGlobals->listDataQuestMenu.m_menuOptions[nIndex].m_data, nControllerIndex);
        UI_GetTextFromID(idTitleName).SetText("Quest Menu", FALSE);
        UI_GetTextFromID(idHeaderName).SetText("", FALSE);
        UI_GetTextFromID(UIDLG_CONSOLEDESIGNERMENU_BUTTONGUIDE).SetText("{icon:button2, 2.0} Accept Changes and Back", FALSE);
        SetupMain();
        break;
    }
    if (bRefreshList)
        tListBox.ResetAllItems();
}
}
}
