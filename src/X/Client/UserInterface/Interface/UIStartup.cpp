// Original file: X/Client/UserInterface/Interface/UIStartup.cpp

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
#include "../../Battlenet/Hub.h"
#include "../../Battlenet/Subsystems/ScreenManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/Online.h"
#include "../../Particle.h"
#include "../../RActors/RActors.h"
#include "../../RWindow.h"
#include "../Console/ConsoleScreenManager.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UIHeroData.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GameTemplate.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers do not carry them yet.
UIIDList & UI_GetStartupDlgStack();  // X/Client/UserInterface/UI.cpp:1399
void UI_PopDialog(UIIDList & listMenuStack, const BOOL fActivatePrev);  // UI.cpp:1464
void UI_PushDialog(UIIDList & listMenuStack, const UIID & idNewMenu, const BOOL fCloseCurrent, const BOOL fSetFocusToNewDialog);  // UI.cpp:1429
void UI_ClearDialogStack(UIIDList & listMenuStack);  // UI.cpp:1417
UIID UIIDCreateFromControlAttribute(const UIControlSourceKey & tSrcKey, const UIID & id, const UIAttributeType eAttributeType);  // UI.cpp:4877
Hub & HubGet();  // X/Client/Battlenet/Hub.cpp:93
void ClientWorldDispose(CWorldID idCWorld);  // X/Client/CWorld.cpp:373
CWorldID ClientWorldCreate(SNO snoWorld, SWorldID idSWorld, GameMode eMode, int32 nOriginX, int32 nOriginY, int32 nStitchSizeInFeetX, int32 nStitchSizeInFeetY, int32 nWorldSizeInFeetX, int32 nWorldSizeInFeetY, SNO snoDungeonFinderSourceWorld);  // CWorld.cpp:184
void UIFunction_CloseAllTabs(const UIID &);  // Interface/UIFunctions.cpp:291
void UIDeathMenu_Console_Init(int32 nPlayerIndex);  // Console/UIConsoleDeathMenu.cpp:275
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp:45
void ParticleSystemDelete(const ParticleSystemID idParticleSystem);  // X/Client/Particle.cpp:2191
Player * LocalPlayerGet();  // X/Client/CPlayer.cpp:1612
namespace Console { namespace Online { void PartyEndGame(); } }  // X/Client/Console/Online.cpp
namespace Console { namespace Notifications { void ShowLeaveGameDialog(int32 nControllerIndex, BOOL fShow, uint32 uMinTickDuration, uint32 uMaxTickDuration, uint32 uKickTickDuration); } }  // ConsoleScreenManager.cpp
namespace UIHUD { namespace Console { void ResetHadBonusXPFlag(); } }  // Console/UIConsoleHUD.cpp

struct UIStartupGlobals {  // size 0x8
    ParticleSystemID idDeathParticle;  // 0x00
    int32 dwLastPVPResurrectAttempt;  // 0x04
};



















static UIStartupGlobals * sgptUIStartupGlobals;




void UIStartup_InitializeForGame()
{

    sgptUIStartupGlobals = (UIStartupGlobals *)SigmaMemoryNew(sizeof(UIStartupGlobals), 4, NULL, TRUE);

    sgptUIStartupGlobals->idDeathParticle = -1;
}




void UIStartup_DisposeForGame()
{

    SigmaMemoryFree(sgptUIStartupGlobals, NULL); sgptUIStartupGlobals = NULL;
}

void UIStartup_PushDialog(const UIID & idNewMenu, const BOOL fCloseCurrent);
void UIStartup_PopDialog(const BOOL fActivatePrev);



















void UIStartup_SetMenu(const UIID & id)

{



    UIControl & tControl = UI_GetControlFromID(id);
    UIID idNewMenu = UIIDCreateFromControlAttribute(tControl.GetUISrcKey(), id, ATT_ID_NEXT);


    if (idNewMenu == GetUIIDNone())
    {
        UIStartup_PopDialog(TRUE);
    }
    else
    {
        UIStartup_PushDialog(idNewMenu, TRUE);
    }
}



void UIStartup_ClearMenuStack()
{

    UI_ClearDialogStack(UI_GetStartupDlgStack());
}












void UIStartup_PushDialog(const UIID & idNewMenu, const BOOL fCloseCurrent)
{


    UI_PushDialog(UI_GetStartupDlgStack(), idNewMenu, fCloseCurrent, TRUE);
}




void UIStartup_PopDialog(const BOOL fActivatePrev)
{

    UIIDList & listDialogStack = UI_GetStartupDlgStack();
    UI_PopDialog(listDialogStack, fActivatePrev);


    if (listDialogStack.Count() == 0)
    {
        HubGet().ScreenManagerGet().ShowCurrentScreen();
    }
}


























































SNO UIStartup_GetInitialWorld()
{
    return GlobalSNOGet(GLOBALSNO_WORLD_BATTLENET_MAINSCREEN);
}




































void UIStartup_SetBackgroundWorld(SNO snoWorld)

{
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    if (tCGameGlobals.snoCurrentWorld != snoWorld)
    {
        CWorldID cWorldIDPrevious = tCGameGlobals.idCurrentCWorld;
        ClientWorldDispose(cWorldIDPrevious);

        GetRWindowMgr()->GetRWindow(0).DisplayListsEnd();

        ClientWorldCreate(snoWorld, SWORLDID_INVALID, GAMEMODE_MENU, 0, 0, 0, 0, 0, 0, SNO_NONE);


        CGameLoadingScreenDisable(LSC_ALL);
        Console::Screens::Update();
    }
}














void UIStartup_ClientDisconnected(const char * pszTitle, const char * pszText)


{




    if (GameIsStartUp()) return;

    Console::Online::PartyEndGame();


    Console::Notifications::ShowLeaveGameDialog(
        Console::GamerProfile::GetPrimaryProfileUserIndex(), FALSE, 0, 0, 0);


    GameStartMenu(GameGetCurrentAct());
}

























































void UIStartup_ClientDied(int32 nPlayerIndex)

{




    UIFunction_CloseAllTabs(GetUIIDNone());





    Console::Screens::CloseScreenSet(TRUE, FALSE);


    if (GameTypeIsPvP())
    {
        if (GameGetGameType() == GT_PVP_ARENA)
        {






            SimpleMessage tMessage;
            MessageSendToServer(CMSG_ACCEPTED_DEATH, &tMessage, sizeof(tMessage), LocalPlayerGet());
        }
    }
    else
    {


        UIDeathMenu_Console_Init(nPlayerIndex);
    }






    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    ptPlayer->tTargetComponent.ClearLockedTarget();




    RActor * ptRActor = RActorTryToGet(LocalPlayerRActorGet());
    if (ptRActor && PlayerGetLocalPlayerCount() == 1)
    {
        if (sgptUIStartupGlobals->idDeathParticle == -1)
        {
            SNO snoFullScreenDeath = GlobalSNOGet(GLOBALSNO_PARTICLE_FULLSCREEN_DEATH);

            ParticleSystemAddParams tParams(snoFullScreenDeath, PRTransform(cgqIdentity, ptRActor->tLocation.tWorldLocation.transform.wp));
            sgptUIStartupGlobals->idDeathParticle = ParticleSystemAdd(tParams);
        }
    }
}




void UIStartup_CleanupForResurrect()

{

    if (sgptUIStartupGlobals->idDeathParticle != -1)
    {
        ParticleSystemDelete(sgptUIStartupGlobals->idDeathParticle);
        sgptUIStartupGlobals->idDeathParticle = -1;
    }

    UI_GetWindowFromID(UIDLG_NOTIFY_MESSAGE).SetActive(FALSE);



    UIHUD::Console::ResetHadBonusXPFlag();
}







void UIStartup_GetHeroData(UIHeroData & tData)
{

    UIAppGlobals * ptUIAppGlobals = sgtAppGlobals.ptUIAppGlobals;
    SigmaMemoryCopy(&tData, ptUIAppGlobals->ptHeroData, sizeof(UIHeroData));
}
































void UIStartup_DeathTimer(const UIID &)

{
    if (GameIsParamFlagSet(GPF_RESURRECTION_DELAY))
    {
        ACDID idACDPlayer = LocalPlayerACDGet();

        if (idACDPlayer != ACDID_INVALID)
        {
            int32 nRespawnTime = ACDGet(idACDPlayer).AttributesGetInt(FastAttribKey(ATTRIBUTE_RESPAWN_GAMETIME));

            if (nRespawnTime == -1) {
                CRefString usTimer("Waiting for round to end!");
                Set_Text_Color(usTimer, 15);
                UI_GetTextFromID(UITEXT_NOTIFY_MESSAGE_TEXT).SetText(usTimer, FALSE);
            }
            else
            {


                int32 nServerTime = CGameGetPredictedServerTime();

                int32 nSeconds = (nRespawnTime - nServerTime) / 60;





                if ((nSeconds < 0 ? -1 : nSeconds) == -1)
                {
                    if (sgptUIStartupGlobals->dwLastPVPResurrectAttempt + 60 < nServerTime)
                    {
                        sgptUIStartupGlobals->dwLastPVPResurrectAttempt = nServerTime;


                        SimpleMessage tMessage;
                        MessageSendToServer(CMSG_ACCEPTED_DEATH, &tMessage, sizeof(tMessage), LocalPlayerGet());
                    }
                }
            }
        }
    }
}
