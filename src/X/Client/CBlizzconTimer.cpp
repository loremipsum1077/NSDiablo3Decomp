// Original file: X/Client/CBlizzconTimer.cpp

#include "CBlizzconTimer.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
#include "Console/GamerProfile.h"
#include "Console/Online.h"
#include "UserInterface/Console/ConsoleScreenManager.h"
#include "UserInterface/Controls/UITextControl.h"
#include "UserInterface/Interface/UIBlizzconEnd.h"
#include "UserInterface/Interface/UIConfirmation.h"
#include "UserInterface/Interface/UIGame.h"
#include "UserInterface/Interface/UIQuestComplete.h"
#include "UserInterface/UI.h"
#include "UserInterface/UIID.h"
#include "UserInterface/UIIDs.h"
#include "../Common/Definitions/StringListDefinition.h"
#include "../Common/Game.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/UserInterface/FontDefinition.h"
#include "../Common/UserInterface/UITextControl.h"
#include "../Sigma/RefString.h"
// Declared here because their headers do not carry them yet.
const CmdLineParams * CmdLineGetParams();  // X/Common/Shell/CmdLine.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
namespace Console { namespace Online { void PartyLeave(); } }  // X/Client/Console/Online.cpp
namespace UIFloatingMessage { void Show(LPCSTR uszMessage, uint32 dwDuration); }  // UIGame.cpp
static void sResetTimer();















uint32 s_uGameTerminationTime = 0;
uint32 s_uCurrentTime = 0;
static uint32 s_uTimeBits = 0;

static int32 s_nGoToVictoryScreenTime = 0;


static uint32 s_uShowVictoryPopUpTime = 0;
static const int32 TIME_DELAY_BEFORE_POP_UP = 1800;


static bool s_bDeferredReturnToStartScreen = false;








void CBlizzconTimerInitializeForGame()
{

    sResetTimer();

}



void CBlizzconTimerDisposeForGame()
{

    s_bDeferredReturnToStartScreen = false;
    s_uGameTerminationTime = 0;
}






























void CBlizzconReturnToStartScreen()
{

    Player * ptPlayer = PlayerGetByControllerIndex(::Console::GamerProfile::GetPrimaryProfileUserIndex());
    if (ptPlayer)
    {




        IntDataMessage tMessage;
        tMessage.nData = 0;
        MessageSendToServer(CMSG_START_LOGOUT_TIMER, &tMessage, sizeof(tMessage), ptPlayer);
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);

        if (::Console::Online::PartyIsInParty())
        {
            ::Console::Online::PartyLeave();
        }
    }
}











void CBlizzconDeferReturnToStartScreen()
{

    s_bDeferredReturnToStartScreen = true;
}

static void sVictoryConfirmed(const UIID &);

void CBlizzconTimerUpdate()
{














    if (s_bDeferredReturnToStartScreen)
    {
        s_bDeferredReturnToStartScreen = false;
        CBlizzconReturnToStartScreen();
    }


    if (s_uGameTerminationTime == 0 ||
        UIBlizzconEnd::IsActive())
    {
        return;
    }


    s_uCurrentTime = AppGetTicks();
    int32 nTimeLeft = s_uGameTerminationTime - s_uCurrentTime;
    SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_DEMO_VICTORY);

    if (nTimeLeft <= 0 && !(s_uTimeBits & 1))
    {

        UIFloatingMessage::Show(StringListFetch(snoStringList, "TimeExpired"), 0);
        UIGame::SetSpecialAreaText(StringListFetch(snoStringList, "ThanksForPlaying"), NULL);
        UI_GetTextFromID(UITEXT_NOTIFY_MESSAGE_TITLE).SetDropShadowColor(RGBAColor(0, 0, 0, 0xff));
        s_uGameTerminationTime = s_uCurrentTime;
        nTimeLeft = 0;
        s_uTimeBits |= 1;
    }

    CRefString sTimeLeftMessage("");


    const int32 nThreeMinutesLeft = 3 * 60 * 60;
    if (nTimeLeft <= nThreeMinutesLeft && !(s_uTimeBits & 2))
    {

        if (sTimeLeftMessage.Length() == 0)
        {
            sTimeLeftMessage = StringListFetch(snoStringList, "ThreeMinutesLeft");
        }

        s_uTimeBits |= 2;
    }

    if (sTimeLeftMessage.Length() != 0)
    {
        UIGame::SetSpecialAreaText(sTimeLeftMessage, NULL);

        Set_Text_Color(sTimeLeftMessage, FONTCOLOR_YELLOW);
    }


    if (nTimeLeft < -900 && !(s_uTimeBits & 4))
    {
        s_uTimeBits |= 4;
        sVictoryConfirmed(GetUIIDNone());
    }

    if ((s_uTimeBits & 8) && !(s_uTimeBits & 0x10) && s_uCurrentTime >= s_uShowVictoryPopUpTime)
    {
        s_uTimeBits |= 0x10;
        UIQuestComplete::Close(GetUIIDNone());
        UIConfirmation::Show(StringListFetch("ConsoleDemo:EndGameModal").str(), sVictoryConfirmed, NULL, FALSE, FALSE, TRUE, -1);
    }

}




static void sVictoryConfirmed(const UIID &)
{

    UIBlizzconEnd::Show(0);
    UIFloatingMessage::Show("", 0);
}




void CBlizzconVictory()
{


    s_uShowVictoryPopUpTime = s_uCurrentTime + TIME_DELAY_BEFORE_POP_UP;
    s_uTimeBits |= 8;



}





static void sResetTimer()
{

    s_bDeferredReturnToStartScreen = false; s_uGameTerminationTime = 0;




    s_uTimeBits = 0; s_uShowVictoryPopUpTime = 0; s_nGoToVictoryScreenTime = 0;


    uint32 uGameTimeLimitInMins = CmdLineGetParams()->uBlizzconGameTimeLimit;
    if (!GameIsStartUp() && uGameTimeLimitInMins != 0)
    {





        uint32 uCurrentTime = AppGetTicks();
        s_uGameTerminationTime = uCurrentTime + uGameTimeLimitInMins * 60 * 60;
    }
}
