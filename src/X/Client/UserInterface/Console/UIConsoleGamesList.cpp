// Original file: X/Client/UserInterface/Console/UIConsoleGamesList.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Controls/UIConsoleListboxControl.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser runs them ahead of Online.h's nn::ldn constants.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../Console/GamerProfile.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "ConsoleScreenManager.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "ControllerActions.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
#include "UIConsolePartyDisplay.h"
#include <stdlib.h>
#include <string.h>
// Declared here because their headers are not adopted yet (Online.cpp, OnlineActions.cpp, UIConsoleMainMenu.cpp, CHero/Quests/Difficulty .cpp).
namespace Console { namespace Online { uint32 PartyFind(int32 nControllerIndex, PartyInfo * ptParties, uint32 uMaxParties); void PartyLeaveAndRemake(uint32 bOffline); } }
namespace Console { namespace UIOnlineActions { BOOL IsJoinPartyInProgress(); void JoinParty(::Console::Online::PartyConnectionInfo & tConnectionInfo, ::Console::Online::PartyJoinCallback pfnCallback); } }
namespace UIMainMenu { namespace Console { void Refresh3DBackgroundHeroes(); } }
CRefString DisplayFormattedHandicap(int32 nHandicap); CRefString DisplayActName(int32 eAct); CRefString QuestsGetUIName(SNO snoQuest);
namespace UIGamesList { namespace Console { BOOL IsVisible();

enum {
    MAX_JOINABLE_GAMES = 20,
};



class JoinableGamesDataSource : public ConsoleListBoxDataSource {
public:

    JoinableGamesDataSource() : uFoundParties(0) {}



    virtual UIControlSourceKey GetSourceKeyForItem(const int32) const
    {

        return UIControlSourceKey::Parse("ConsolePublicGamesList:ConsolePublicGames_main.PublicGamesListItemTemplate", -1, NULL, NULL);
    }


    virtual int32 GetNumItems() const
    {

        return uFoundParties;
    }



    virtual void InitializeItemUI(const UIConsoleListBox &, const UIID &, const int32)
    {



    }

    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);







    void Initialize(int32 nControllerIndex)
    {

        uFoundParties = ::Console::Online::PartyFind(nControllerIndex, tJoinableParties, MAX_JOINABLE_GAMES);
        if (uFoundParties > 1)
        {
            qsort(tJoinableParties, uFoundParties, sizeof(::Console::Online::PartyInfo), PartyInfoSortPred);
        }
    }


    static int32 PartyInfoSortPred(const void * pDataA, const void * pDataB)
    {
        ::Console::Online::PartyInfo * tInfoA = (::Console::Online::PartyInfo *)pDataA;
        ::Console::Online::PartyInfo * tInfoB = (::Console::Online::PartyInfo *)pDataB;

        return strcasecmp(tInfoA->m_szName, tInfoB->m_szName);
    }

    ::Console::Online::PartyInfo tJoinableParties[MAX_JOINABLE_GAMES];
    uint32 uFoundParties;
};


struct GameListGlobals {
    JoinableGamesDataSource tGamesListSource;
    RGBAColor rgbaNotSelectedColor;
    BOOL bColorSet;


    GameListGlobals() : bColorSet(FALSE) {}
};
GameListGlobals sgtGameListGlobals;


void JoinableGamesDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{



    UIID idLevel = UIIDCreateFromParent(idItemUI, ".Level");
    UIID idHost = UIIDCreateFromParent(idItemUI, ".Host");
    UIID idDifficulty = UIIDCreateFromParent(idItemUI, ".Difficulty");
    UIID idQuest = UIIDCreateFromParent(idItemUI, ".Quest");
    UIID idParty = UIIDCreateFromParent(idItemUI, ".Party");
    UIID idPing = UIIDCreateFromParent(idItemUI, ".Ping");
    UIID idSelected = UIIDCreateFromParent(idItemUI, ".Selected");
    if (!sgtGameListGlobals.bColorSet)
    {
        sgtGameListGlobals.rgbaNotSelectedColor = UI_GetTextFromID(idLevel).GetTextColor();
        sgtGameListGlobals.bColorSet = TRUE;
    }
    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex;

    ::Console::Online::PartyInfo & tParty = tJoinableParties[nIndex];
    ::Console::Online::GameInfo::Player & tHost = tParty.m_tGameInfo.m_tPlayers[0];
    UI_GetTextFromID(idHost).SetText(tParty.m_szName, FALSE);
    UI_GetTextFromID(idHost).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);
    if (tParty.m_tGameInfo.m_snoQuest != SNO_NONE && tParty.m_tGameInfo.m_snoQuest != INVALID_SNO)
    {







        UI_GetTextFromID(idLevel).SetText(UnicodeText::FormatNumber((int32)tHost.m_tUserPresence.m_uLevel, 1, FNS_NORMAL, 0), FALSE);
        UI_GetTextFromID(idLevel).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);

        CRefString szDifficulty = DisplayFormattedHandicap(tParty.m_tGameInfo.m_nHandicap);
        UI_GetTextFromID(idDifficulty).SetText(szDifficulty, FALSE);
        UI_GetTextFromID(idDifficulty).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);

        CRefString sFinalString = LocalizedSprintf(
            StringListFetch("Bnet_CallToArms:GameDetails").str(),
            DisplayActName(tParty.m_tGameInfo.m_eAct).str(),
            QuestsGetUIName(tParty.m_tGameInfo.m_snoQuest).str(), NULL);
        UI_GetTextFromID(idQuest).SetText(sFinalString, FALSE);
        UI_GetTextFromID(idQuest).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);

        CRefString sParty = LocalizedSprintf(
            StringListFetch("ConsoleGamesList:PartyDisplay").str(),
            UnicodeText::FormatNumber((int32)tParty.m_tGameInfo.m_uNumPlayers, 1, FNS_NORMAL, 0).str(),
            UnicodeText::FormatNumber(4, 1, FNS_NORMAL, 0).str(), NULL);
        UI_GetTextFromID(idParty).SetText(sParty, FALSE);
        UI_GetTextFromID(idParty).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);
    }
    else
    {
        UI_GetTextFromID(idLevel).SetText("-", FALSE);
        UI_GetTextFromID(idLevel).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);
        UI_GetTextFromID(idDifficulty).SetText("-", FALSE);
        UI_GetTextFromID(idDifficulty).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);
        UI_GetTextFromID(idQuest).SetText("-", FALSE);
        UI_GetTextFromID(idQuest).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);
        UI_GetTextFromID(idParty).SetText("-", FALSE);
        UI_GetTextFromID(idParty).SetTextColor(bSelected ? crgbaWhite : sgtGameListGlobals.rgbaNotSelectedColor);
    }

    UI_GetWindowFromID(idPing).SetActive(FALSE);
    UI_GetWindowFromID(idSelected).SetActive(bSelected);
}


void RefreshGamesDisplay(int32 nControllerIndex)
{

    sgtGameListGlobals.tGamesListSource.Initialize(nControllerIndex);
    UIConsoleListBox & tGamesList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SELECTOR);
    tGamesList.SetDataSource(sgtGameListGlobals.tGamesListSource);
    tGamesList.RefreshVisibleItems();

    CRefString sMessage = LocalizedSprintf(
        StringListFetch("ConsoleGamesList:GamesFound").str(),
        UnicodeText::FormatNumber(sgtGameListGlobals.tGamesListSource.GetNumItems(), 1, FNS_NORMAL, 0).str(), NULL);

    UI_GetTextFromID(UITEXT_CONSOLE_GAME_SELECT_NUMGAMES).SetText(sMessage, FALSE);
}


void OpenForController(int32 nControllerIndex)
{

    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_GAME_SELECT, TRUE, GetUIIDNone());

    RefreshGamesDisplay(nControllerIndex);
    UIConsoleListBox & tGamesList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SELECTOR);
    if (sgtGameListGlobals.tGamesListSource.GetNumItems() == 0)
    {
        tGamesList.SetSelectedIndex(-1);
    }
    else
    {
        tGamesList.SetSelectedIndex(0);
    }
    UI_SetControllerFocus(UILIST_CONSOLE_GAME_SELECTOR);

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("Console:Accept"));
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("Console:Back"));
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, 2, StringListFetch("ConsoleGamesList:RefreshGames"));
}


void Close()
{

    if (IsVisible())
    {
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_GAME_SELECT);
    }
}


BOOL IsVisible()
{

    return UI_GetWindowFromID(UIWND_CONSOLE_GAME_SELECT).IsActive();
}


inline void OnPartyJoined(::Console::Online::JoinResult eResult)
{

    UIPartyDisplay::Console::RefreshPartyDisplay();
    UIMainMenu::Console::Refresh3DBackgroundHeroes();

    if (eResult == ::Console::Online::JOIN_SUCCESS)
    {
        Close();
    } else {
        if (::Console::GamerProfile::GetPrimaryProfileUserIndex() != -1)
        {
            RefreshGamesDisplay(::Console::GamerProfile::GetPrimaryProfileUserIndex());
        }
    }
}

void OnAccept(const UIID &, const int32 nControllerIndex)
{


    if (!::Console::UIOnlineActions::IsJoinPartyInProgress() &&
        ::Console::Online::PartyIsInParty())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);


        UIConsoleListBox & tGamesList = UI_GetConsoleListBoxFromID(UILIST_CONSOLE_GAME_SELECTOR);
        if (tGamesList.GetSelectedIndex() != -1)
        {


            if (::Console::Online::PartyIsInParty() &&
                !::Console::Online::PartyIsHost())
            {
                ::Console::Online::PartyLeaveAndRemake(FALSE);
            }

            ::Console::Online::PartyInfo & tPartyInfo = sgtGameListGlobals.tGamesListSource.tJoinableParties[tGamesList.GetSelectedIndex()];

            ::Console::UIOnlineActions::JoinParty(tPartyInfo, OnlineService::internal::function_bind_0_1<void, ::Console::Online::JoinResult, &OnPartyJoined>());
        }
        else
        {
            Close(); } }
}


void OnCancel(const UIID &, const int32 nControllerIndex)
{


    if (!::Console::UIOnlineActions::IsJoinPartyInProgress())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);

        Close();
    }
}
void OnRefresh(const UIID & id, const int32 nControllerIndex)
{


    if (!::Console::UIOnlineActions::IsJoinPartyInProgress())
    {
        RefreshGamesDisplay(nControllerIndex); }
}

} }  // namespace UIGamesList::Console
