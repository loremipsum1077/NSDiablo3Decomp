#include "UIConsoleDeathMenuHardcore.h"

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Gfx/GfxCommon.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/RefString.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"

// Declared here because their headers are not adopted yet. Each comment names
// the file that defines the function or object.
// X/Client/UserInterface/UI.cpp
XGrowableList<UIID, const UIID &> & UI_GetInGameDlgStack();
void UI_PushDialog(XGrowableList<UIID, const UIID &> & listMenuStack, const UIID & idNewMenu, const BOOL fCloseCurrent, const BOOL fSetFocusToNewDialog);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
// X/Client/CPlayer.cpp
Player * LocalPlayerGet();
// X/Common/Player.cpp
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound);
// X/Client/UserInterface/Console/UIConsoleDeathMenu.cpp
void UIDeathMenu_Console_HandleHardcoreDeath(const int32 nControllerIndex);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_BARBARIAN;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_CRUSADER;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_DEMONHUNTER;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_MONK;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_NECROMANCER;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_WITCHDOCTOR;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_WIZARD;
extern UIID UIDLG_CONSOLE_HARDCORE_DEATHMENU_PORTRAIT;

namespace UIDeathMenuHardcore {
namespace Console {

void OpenForPlayer(int32 nPlayerIndex)
{
    UI_PushDialog(UI_GetInGameDlgStack(), UIDLG_CONSOLE_HARDCORE_DEATHMENU, TRUE, TRUE);

    Player * ptPlayer = LocalPlayerGet();

    // The default `ImageTextureFrame` is the all-`-1` pair; clang folds the two
    // `-1` members into one 64-bit store.
    ImageTextureFrame tFrame;
    ActorGetPortraitTexFrame(tFrame, ptPlayer->snoActorPortrait, FALSE);

    UIWindow & tWindow = UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_PORTRAIT);
    tWindow.SetTextureFrame(tFrame);
    tWindow.SetActive(TRUE);

    UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_BARBARIAN).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_CRUSADER).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_DEMONHUNTER).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_MONK).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_WITCHDOCTOR).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_WIZARD).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU_NECROMANCER).SetActive(FALSE);

    PlayerClass eClass = PlayerGetPlayerClass(ptPlayer);

    // Left uninitialised on purpose: the jump table has no default edge, so an
    // out-of-range class falls straight through to the `UI_GetWindowFromID`
    // below with whatever the slot held (0x3c5178 `b.hi` -> 0x3c51d4).
    UIID idMenu;

    switch (eClass)
    {
        case PLAYER_CLASS_BARBARIAN:
            idMenu = UIDLG_CONSOLE_HARDCORE_DEATHMENU_BARBARIAN;
            break;
        case PLAYER_CLASS_CRUSADER:
            idMenu = UIDLG_CONSOLE_HARDCORE_DEATHMENU_CRUSADER;
            break;
        case PLAYER_CLASS_DEMONHUNTER:
            idMenu = UIDLG_CONSOLE_HARDCORE_DEATHMENU_DEMONHUNTER;
            break;
        case PLAYER_CLASS_MONK:
            idMenu = UIDLG_CONSOLE_HARDCORE_DEATHMENU_MONK;
            break;
        case PLAYER_CLASS_NECROMANCER:
            idMenu = UIDLG_CONSOLE_HARDCORE_DEATHMENU_NECROMANCER;
            break;
        case PLAYER_CLASS_WITCHDOCTOR:
            idMenu = UIDLG_CONSOLE_HARDCORE_DEATHMENU_WITCHDOCTOR;
            break;
        case PLAYER_CLASS_WIZARD:
            idMenu = UIDLG_CONSOLE_HARDCORE_DEATHMENU_WIZARD;
            break;
    }

    UI_GetWindowFromID(idMenu).SetActive(TRUE);

    UIID idText = UIIDCreateFromParent(idMenu, ".PlayerName");
    UI_GetTextFromID(idText).SetText(PlayerGetHeroDisplayName(*ptPlayer, FALSE).str(), FALSE);
}

void OnAccept(const UIID & id, const int32 nControllerIndex)
{
    UIDeathMenu_Console_HandleHardcoreDeath(nControllerIndex);
}

}  // namespace Console
}  // namespace UIDeathMenuHardcore
