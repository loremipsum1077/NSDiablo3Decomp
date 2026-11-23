// Reconstructed source; see docs/source_provenance.md.

#include "../../Common/ActorCommonData.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameTemplate.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Player.h"
#include "../../Common/UserInterface/UITextControl.h"
#include "../../Sigma/RefString.h"
#include "../CPlayer.h"
#include "../Sound/Sound.h"

// Declarations for functions whose owning headers are not adopted yet.
void UIStartup_ClientDisconnected(const char * pszTitle, const char * pszText);  // UIStartup.cpp:314
void UIStartup_ClientDied(int32 nPlayerIndex);  // UIStartup.cpp:390
void UIStartup_CleanupForResurrect();
void CPlayerTakeScreenshot();  // CPlayer.cpp
CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound);  // CPlayer.cpp
CRefString CHyperlinkCreateFromGameAccountHandle(const OnlineService::GameAccountHandle & tGameAccount,
                                                 CRefString sDisplayName);  // Hyperlink.cpp:2417
void PlayerSetDefaults(Player & tPlayer);  // Player.cpp
void UI_Print(LPCSTR szText, BOOL bChat);  // UI.cpp
void ClientInitializeForGame();  // Client.cpp
void ClientDisposeForGame();  // Client.cpp
void ClientReadUpdate();  // Client.cpp
void ClientWriteUpdate();  // Client.cpp


















void CNetworkClientDisconnected() {



    UIStartup_ClientDisconnected(
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "DisconnectTitle"),
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "DisconnectText"));
}





void CNetworkSeasonEnded() {



    UIStartup_ClientDisconnected(
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "SeasonEndTitle"),
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "SeasonEndText"));
}





void CNetworkChallengeRiftsDisabled() {



    UIStartup_ClientDisconnected(
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "ChallengeRiftsDisabledTitle"),
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "ChallengeRiftsDisabledText"));
}





void CNetworkClientDied(int32 nPlayerIndex) {


    const Player * ptLocalPlayer = LocalPlayerGet();
    if (ptLocalPlayer != NULL) {

        ActorCommonData * ptACDLocalPlayer = ACDTryToGet(ptLocalPlayer->idPlayerACD);
        if ((ptLocalPlayer->tHeroStateData.uPlayerFlags & 1) && !GameTypeIsPvP() &&

            (ptACDLocalPlayer == NULL || !ptACDLocalPlayer->IsDueling())) {

            CPlayerTakeScreenshot();
        }
    }



    UIStartup_ClientDied(nPlayerIndex);
}

void CNetworkServerAckedResurrect() {


    UIStartup_CleanupForResurrect();
}




void CNetworkPlayerDisconnected(int32 nPlayerIndex) {


    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    if (ptPlayer != NULL) {








        {
            CRefString usMessage;
            CRefString strName = CPlayerGetServiceName(*ptPlayer, FALSE);
            if (strName.Length() != 0) {
                CRefString sHyperlinkName = CHyperlinkCreateFromGameAccountHandle(
                    ptPlayer->tAccount.tHandle, CRefString(CRefString().Format("[%s]", strName.str())));

                Set_Text_Color(sHyperlinkName, 0x16);
                if (!GameTypeIsPvP()) {

                    usMessage = LocalizedSprintf(
                        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerLeft"),
                        sHyperlinkName.str(), NULL);
                } else {

                    usMessage = LocalizedSprintf(
                        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerLeftPVP"),
                        sHyperlinkName.str(), NULL);
                }
            } else {

                if (!GameTypeIsPvP()) {

                    usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerLeftUnknown");
                } else {


                    usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerLeftPVPUnknown");
                }
            }

            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PLAYER_LEFT), cgtPSODefaults);

            Set_Text_Color(usMessage, 0x15);















        UI_Print(usMessage, TRUE);

        }

        PlayerSetDefaults(*ptPlayer);
    }
}



void CNetworkInitializeForGame() {


    if (GameIsNetworked()) {




        ClientInitializeForGame();
    }
}



void CNetworkDisposeForGame() {


    if (GameIsNetworked()) {




        ClientDisposeForGame();
    }
}








void CNetworkReadUpdate() {







    NetworkGlobals * ptNetworkGlobals = sgtAppGlobals.ptNetworkGlobals;

    if (ptNetworkGlobals != NULL) {










        ClientReadUpdate();
    }
}



void CNetworkWriteUpdate() {
    NetworkGlobals * ptNetworkGlobals = sgtAppGlobals.ptNetworkGlobals;
    if (ptNetworkGlobals != NULL) {
        ClientWriteUpdate();
    }
}
