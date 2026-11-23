#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleModalNotificationManager.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../Gfx/GfxVB.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "../../../Common/NPC.h"
typedef int32 CrafterType;
enum {
    CRAFTER_TYPE_JEWELER = 1,  // X/Common/Crafting/ItemCraftingSwig.h
};














typedef int32 NPCInteractionType;





typedef int32 NPCMenu;






// X/Common/NPC.cpp
CrafterType NPCGetCrafterType(const ActorCommonData & tACD);
// X/Common/Monsters.cpp
void MonsterGetBaseName(const SNOName & tSNOName, CRefString & sName);
// X/Common/Conversation.cpp
void ConversationGetTitleLabel(SNO snoConversation, CRefString & sLabel, int32 & snoStringList);
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
int32 PlayerGetNumPlayers();
// X/Common/Game.cpp
BOOL GameTypeIsPvP();
BOOL GameIsWeeklyChallenge();
#include "../../../Common/Networking/GameMessage.h"













// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/CPlayer.cpp
Player * LocalPlayerGet();
// X/Client/Sound/Sound.h
typedef int32 SoundID;
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
int32 UI_GetPrimaryUIController();
void UI_SetPrimaryUIController(int32 nControllerIndex);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIWND_CONVERSATION_OPTIONS_MAIN;
extern UIID UIWND_CONVERSATION_OPTIONS_TALK;
extern UIID UIWND_CONVERSATION_OPTIONS_HIRE;
extern UIID UIWND_CONVERSATION_OPTIONS_INVENTORY;
extern UIID UIWND_CONVERSATION_OPTIONS_CRAFT;
extern UIID UITXT_CONVERSATION_OPTIONS_MAIN_NAME;
extern UIID UIWND_CONVERSATION_OPTIONS_TALK_MENU;
extern UIID UILIST_CONVERSATION_OPTIONS_TALK;
// X/Client/UserInterface/Interface/UIConfirmation.cpp
namespace UIConfirmation {
BOOL IsVisible();
}  // namespace UIConfirmation
// X/Client/Console/LocalPlayerClamping.cpp
namespace LocalPlayerClamping {
BOOL IsBeingDragged(int32 nControllerIndex);
}  // namespace LocalPlayerClamping
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
BOOL IsScreenSetOpen();
ConsoleModalNotificationManager & GetModalNotificationManager();
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleNPC.h
namespace UINPC {
namespace Console {
void OnTalkSelect(const UIID &, const int32 nControllerIndex);
void OnUpdate(const UIID &);
void OnTalk(const UIID &, const int32 nControllerIndex);
void OnTalkCancel(const UIID &, const int32 nControllerIndex);
void OnHire(const UIID &, const int32 nControllerIndex);
void OnCraft(const UIID &, const int32 nControllerIndex);
void InteractMenuEnable(const int32 nControllerIndex, ActorCommonData & tACDTalkingTo, const NPCInteraction * ptNPCInteraction, const int32 nNumInteractions, const NPCMenu ePreferredMenu);
void InteractMenuDisable(const int32 nControllerIndex);
BOOL InteractMenuIsActive();
BOOL CachedInteractMenuIsActive();
}  // namespace Console
}  // namespace UINPC

// Invented name: the talk-option flag that greys a row out and that sSwitchToTalkMenu skips.
enum {
    TALK_MENU_FLAG_DISABLED = 0x2,
};

struct MainMenuItem {  // size 0x8
    NPCInteractionType eType;  // 0x00
    uint32 dwFlags;  // 0x04
    MainMenuItem() : eType(NIT_INVALID), dwFlags(0) {}
};

struct MainMenu {  // size 0xf4
    MainMenu() : nNumMenuItems(0) {}
    void BuildMainMenu(const ActorCommonData & tACDTalkingTo, const int32 nNumInteractions, const NPCInteraction * ptNPCInteraction);
    void AddMenuItem(const NPCInteractionType eType, const uint32 dwFlags);
    MainMenuItem tMenuItems[30];  // 0x00
    int32 nNumMenuItems;  // 0xf0
};

struct TalkMenuItem {  // size 0x10c
    SNO snoStringList;  // 0x00
    CHAR uzLabel[256];  // 0x04
    SNO snoConversation;  // 0x104
    uint32 dwFlags;  // 0x108
    TalkMenuItem() : snoStringList(SNO_NONE), snoConversation(SNO_NONE), dwFlags(0) {
        SigmaMemoryClear(uzLabel, sizeof(uzLabel));
    }
};

class ListViewDataSource : public ConsoleListBoxDataSource {  // size 0x1f78
public:
    ListViewDataSource();
    virtual ~ListViewDataSource();
    void BuildTalkSubmenu(const int32 nNumInteractions, const NPCInteraction * ptNPCInteraction);
    void AddMenuItem(const SNO snoStringList, const CHAR * pszLabel, const SNO snoConversation, const uint32 dwFlags);
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    TalkMenuItem tMenuItems[30];  // 0x08
    int32 nNumMenuItems;  // 0x1f70
};

ListViewDataSource::~ListViewDataSource()
{
}

struct NPCMenuGlobals {  // size 0x20f8
    ACDID idACDTalkingTo;  // 0x00
    CHAR uszTalkingTo[128];  // 0x04
    MainMenu tMainMenu;  // 0x84
    ListViewDataSource tTalkSubMenu;  // 0x178
    BOOL bCachedInteractMenuActive;  // 0x20f0
    NPCMenuGlobals() {
        idACDTalkingTo = ACDID_INVALID;
        SigmaMemoryClear(uszTalkingTo, sizeof(uszTalkingTo));
        bCachedInteractMenuActive = FALSE;
    }
};

static NPCMenuGlobals sgtNPCMenuGlobals;

ListViewDataSource::ListViewDataSource()
{
    nNumMenuItems = 0;
}

int32 ListViewDataSource::GetNumItems() const
{
    return nNumMenuItems;
}

UIControlSourceKey ListViewDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse("ConsoleConversationOptions:ConsoleConversationOptions_main.LayoutRoot.Talk.TalkOptionsTemplate", SNO_NONE, NULL, NULL);
    return ITEM_SOURCE_KEY;
}

void ListViewDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

void ListViewDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    UIID idText;
    idText.SetFromParent(idItemUI, "option");

    UIText & tText = UI_GetTextFromID(idText);
    CRefString sText;

    TalkMenuItem & tMenuItem = tMenuItems[nIndex];
    if (tMenuItem.snoStringList != SNO_NONE)
        sText = StringListFetch(tMenuItem.snoStringList, tMenuItem.uzLabel);
    else
        sText = tMenuItem.uzLabel;

    tText.SetText(sText, FALSE);

    tText.SetTextColor(nIndex == nNumMenuItems - 1 ? crgbaRed : (tMenuItem.dwFlags & TALK_MENU_FLAG_DISABLED) ? crgbaGray : crgbaWhite);

    UIID idSelected = UIIDCreateFromParent(idItemUI, ".highlight");
    UI_GetWindowFromID(idSelected).SetActive(tListBox.GetSelectedIndex() == nIndex);
}

inline void ListViewDataSource::AddMenuItem(const SNO snoStringList, const CHAR * pszLabel, const SNO snoConversation, const uint32 dwFlags)
{
    if (nNumMenuItems < 30)
    {
        TalkMenuItem & tItem = tMenuItems[nNumMenuItems];
        tItem.snoStringList = snoStringList;
        Blizzard::String::Copy(tItem.uzLabel, pszLabel, sizeof(tItem.uzLabel));
        tItem.snoConversation = snoConversation;
        tItem.dwFlags = dwFlags;
        nNumMenuItems++;
    }
}

void ListViewDataSource::BuildTalkSubmenu(const int32 nNumInteractions, const NPCInteraction * ptNPCInteraction)
{
    nNumMenuItems = 0;

    for (int32 i = 0; i < nNumInteractions; i++)
    {
        const NPCInteraction & tInteraction = ptNPCInteraction[i];
        if (tInteraction.eType == NIT_TALK_IMPORTANT || tInteraction.eType == NIT_TALK_DIALOG)
        {
            SNO snoConv = tInteraction.dwData;
            SNO snoStringList = SNO_NONE;
            CRefString sLabel;
            ConversationGetTitleLabel(snoConv, sLabel, snoStringList);
            AddMenuItem(snoStringList, sLabel, snoConv, tInteraction.dwFlags);
        }
    }

    AddMenuItem(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "Cancel", SNO_NONE, 0);
}

inline void MainMenu::AddMenuItem(const NPCInteractionType eType, const uint32 dwFlags)
{
    if (nNumMenuItems >= 30)
        return;

    MainMenuItem & tItem = tMenuItems[nNumMenuItems];
    tItem.eType = eType;
    tItem.dwFlags = dwFlags;
    nNumMenuItems++;
}

inline void MainMenu::BuildMainMenu(const ActorCommonData & tACDTalkingTo, const int32 nNumInteractions, const NPCInteraction * ptNPCInteraction)
{
    nNumMenuItems = 0;

    if (sgtNPCMenuGlobals.tTalkSubMenu.nNumMenuItems > 1)
        AddMenuItem(NIT_TALK_DIALOG, 1);

    for (int32 i = 0; i < nNumInteractions; i++)
    {
        if (ptNPCInteraction[i].eType == NIT_CRAFT || ptNPCInteraction[i].eType == NIT_HIRE)
            AddMenuItem(ptNPCInteraction[i].eType, ptNPCInteraction[i].dwFlags);
    }
}

static void sHideMainMenu()
{
    UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_MAIN).SetActive(FALSE);
}

static void sHideTalkMenu()
{
    UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_TALK_MENU).SetActive(FALSE);
}

static BOOL sCanHireHireling(Player * ptPlayer)
{
    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACD && ptACD->AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN))
        return FALSE;

    return PlayerGetNumPlayers() == 1 && !GameTypeIsPvP() && !GameIsWeeklyChallenge();
}

static void sSwitchToMainMenu(int32 nControllerIndex)
{
    sHideTalkMenu();
    UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_MAIN).SetActive(TRUE);

    BOOL fTalk = FALSE;
    BOOL fHire = FALSE;
    BOOL fCraft = FALSE;
    LPCSTR szLabel = "";

    for (int32 i = 0; i < sgtNPCMenuGlobals.tMainMenu.nNumMenuItems; i++)
    {
        switch (sgtNPCMenuGlobals.tMainMenu.tMenuItems[i].eType)
        {
        case NIT_TALK_IMPORTANT:
        case NIT_TALK_DIALOG:
            fTalk = TRUE;
            break;

        case NIT_CRAFT:
            {
                ActorCommonData * ptACDCrafter = ACDTryToGet(sgtNPCMenuGlobals.idACDTalkingTo);
                if (ptACDCrafter)
                {
                    CrafterType eCrafterType = NPCGetCrafterType(*ptACDCrafter);
                    szLabel = eCrafterType == CRAFTER_TYPE_JEWELER ? "UI:Console_Artisan_Gem" : "UI:Console_Artisan_Anvil";
                }
                fCraft = TRUE;
            }
            break;

        case NIT_HIRE:
            if (sCanHireHireling(PlayerGetByControllerIndex(nControllerIndex)))
                fHire = TRUE;
            break;
        }
    }

    UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_TALK).SetActive(fTalk);
    UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_HIRE).SetActive(fHire);
    UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_INVENTORY).SetActive(FALSE);

    UIWindow & tWndCraft = UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_CRAFT);
    tWndCraft.SetActive(fCraft);
    if (fCraft)
    {
        UIID idIcon = UIIDCreateFromParent(tWndCraft.GetID(), ".icon");
        UI_GetWindowFromID(idIcon).SetTextureFrame(szLabel);
    }

    UI_GetTextFromID(UITXT_CONVERSATION_OPTIONS_MAIN_NAME).SetText(sgtNPCMenuGlobals.uszTalkingTo, FALSE);
}

static void sSwitchToTalkMenu()
{
    sHideMainMenu();
    UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_TALK_MENU).SetActive(TRUE);

    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_CONVERSATION_OPTIONS_TALK);
    tListBox.SetDataSource(sgtNPCMenuGlobals.tTalkSubMenu);
    tListBox.ResetAllItems();

    if (tListBox.GetNumItems() != 0)
    {
        int32 nIndex = 0;
        for (int32 i = 0; i < sgtNPCMenuGlobals.tTalkSubMenu.nNumMenuItems; i++)
        {
            if (!(sgtNPCMenuGlobals.tTalkSubMenu.tMenuItems[i].dwFlags & TALK_MENU_FLAG_DISABLED))
                break;
            nIndex++;
        }
        tListBox.SetSelectedIndex(nIndex);
    }

    UI_SetControllerFocus(UILIST_CONVERSATION_OPTIONS_TALK);
}

static void sCloseMenu()
{
    sHideMainMenu();
    sHideTalkMenu();
    UI_SetPrimaryUIController(-1);

    SimpleMessage tMessage;
    MessageSendToServer(CMSG_NPC_INTERACT_CANCEL, &tMessage, sizeof(tMessage), LocalPlayerGet());
}

void UINPC::Console::OnTalkSelect(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
        return;

    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_CONVERSATION_OPTIONS_TALK);
    int32 nSelectedIndex = tListBox.GetSelectedIndex();
    if (nSelectedIndex == -1)
        return;

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    TalkMenuItem & tSelectedItem = sgtNPCMenuGlobals.tTalkSubMenu.tMenuItems[nSelectedIndex];
    if (tSelectedItem.snoConversation == SNO_NONE)
    {
        OnTalkCancel(GetUIIDNone(), nControllerIndex);
    }
    else
    {
        SNODataMessage tMessage;
        tMessage.snoData = tSelectedItem.snoConversation;
        MessageSendToServer(CMSG_NPC_INTERACT_TALK_DIALOG, &tMessage, sizeof(tMessage), ptPlayer);
        sCloseMenu();
    }

    SNO sound = GlobalSNOGet(GLOBALSNO_SOUND_CLICK_GENERIC);
    SoundManager::PlaySound(sound, cgtPSODefaults);
}

void UINPC::Console::OnUpdate(const UIID &)
{
    sgtNPCMenuGlobals.bCachedInteractMenuActive = FALSE;

    UIWindow & tWndTalk = UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_TALK_MENU);
    UIWindow & tWndMain = UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_MAIN);

    if (tWndTalk.IsActive())
    {
        if (::Console::Screens::GetModalNotificationManager().IsNotificationShowing() || UIConfirmation::IsVisible())
            sCloseMenu();

        tWndTalk.CenterOverACD(sgtNPCMenuGlobals.idACDTalkingTo);
        sgtNPCMenuGlobals.bCachedInteractMenuActive = TRUE;
    }
    else if (tWndMain.IsActive())
    {
        tWndMain.CenterOverACD(sgtNPCMenuGlobals.idACDTalkingTo);
        sgtNPCMenuGlobals.bCachedInteractMenuActive = TRUE;
    }

    if (sgtNPCMenuGlobals.bCachedInteractMenuActive && UI_GetPrimaryUIController() != -1 && LocalPlayerClamping::IsBeingDragged(UI_GetPrimaryUIController()))
    {
        sCloseMenu();
        sgtNPCMenuGlobals.bCachedInteractMenuActive = FALSE;
    }
}

void UINPC::Console::OnTalk(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
        return;

    if (!::Console::Screens::IsScreenSetOpen())
        sSwitchToTalkMenu();
}

void UINPC::Console::OnTalkCancel(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
        return;

    sCloseMenu();
}

void UINPC::Console::OnHire(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
        return;

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (sCanHireHireling(ptPlayer))
    {
        SimpleMessage tMessage;
        MessageSendToServer(CMSG_NPC_INTERACT_HIRE, &tMessage, sizeof(tMessage), ptPlayer);

        SNO sound = GlobalSNOGet(GLOBALSNO_SOUND_CLICK_GENERIC);
        SoundManager::PlaySound(sound, cgtPSODefaults);

        sCloseMenu();
    }
}

void UINPC::Console::OnCraft(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
        return;

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    SimpleMessage tMessage;
    MessageSendToServer(CMSG_NPC_INTERACT_CRAFT, &tMessage, sizeof(tMessage), ptPlayer);

    sCloseMenu();
}

void UINPC::Console::InteractMenuEnable(const int32 nControllerIndex, ActorCommonData & tACDTalkingTo, const NPCInteraction * ptNPCInteraction, const int32 nNumInteractions, const NPCMenu ePreferredMenu)
{
    if (::Console::Screens::IsScreenSetOpen())
        return;

    UI_SetPrimaryUIController(nControllerIndex);
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    if (sgtNPCMenuGlobals.idACDTalkingTo != ACDID_INVALID)
        ptPlayer->tClientInfo.fSuppressVendorCloseNotification = TRUE;

    sgtNPCMenuGlobals.tTalkSubMenu.BuildTalkSubmenu(nNumInteractions, ptNPCInteraction);
    sgtNPCMenuGlobals.tMainMenu.BuildMainMenu(tACDTalkingTo, nNumInteractions, ptNPCInteraction);

    CRefString usTalkingTo;
    if (tACDTalkingTo.IsMonster())
        MonsterGetBaseName(tACDTalkingTo.tSNOName, usTalkingTo);

    Blizzard::String::Copy(sgtNPCMenuGlobals.uszTalkingTo, usTalkingTo, sizeof(sgtNPCMenuGlobals.uszTalkingTo));
    sgtNPCMenuGlobals.idACDTalkingTo = tACDTalkingTo.id;

    NPCMenu eMenuToEnable = NM_DEFAULT;
    if (ePreferredMenu == NM_TALK && sgtNPCMenuGlobals.tTalkSubMenu.nNumMenuItems > 1)
        eMenuToEnable = NM_TALK;
    else if (ePreferredMenu == NM_DEFAULT && sgtNPCMenuGlobals.tMainMenu.nNumMenuItems == 1 && sgtNPCMenuGlobals.tTalkSubMenu.nNumMenuItems > 1)
        eMenuToEnable = NM_TALK;

    if (eMenuToEnable == NM_TALK)
        sSwitchToTalkMenu();
    else
        sSwitchToMainMenu(nControllerIndex);

    ptPlayer->tClientInfo.fSuppressVendorCloseNotification = FALSE;
}

void UINPC::Console::InteractMenuDisable(const int32 nControllerIndex)
{
    sCloseMenu();
}

BOOL UINPC::Console::InteractMenuIsActive()
{
    return UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_MAIN).IsActive() || UI_GetWindowFromID(UIWND_CONVERSATION_OPTIONS_TALK_MENU).IsActive();
}

BOOL UINPC::Console::CachedInteractMenuIsActive()
{
    return sgtNPCMenuGlobals.bCachedInteractMenuActive;
}
