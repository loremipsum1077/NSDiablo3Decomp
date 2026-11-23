// Original file: X/Client/UserInterface/Console/UIConsoleMultiplayerDeathMenu.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "UIConsoleMultiplayerDeathMenu.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIGame.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GameTemplate.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// Local declarations stand in for headers not adopted yet; each comment names the defining file.
UIWindow & UI_GetWindowFromID(const UIID & id); UIText & UI_GetTextFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace UIHUD { namespace Console { int32 PlayerGetHUDSlot(const Player * ptPlayer); } }  // UIConsoleHUD.cpp
BOOL UIDeathMenu_Console_CanReviveAtCorpse(ACDID idACDPlayer); void UIDeathMenu_Console_ReviveAtCorpse(const UIID & id, const int32 nControllerIndex); void UIDeathMenu_Console_ReviveAtCheckpoint(const UIID & id, const int32 nControllerIndex); void UIDeathMenu_Console_HandleHardcoreDeath(const int32 nControllerIndex); void UIDeathMenu_Console_ReviveInTown(const UIID & id, const int32 nControllerIndex);  // UIConsoleDeathMenu.cpp

namespace UIMultiplayerDeathMenu {
namespace Console {



static BOOL bOpenForController[4];

void sUpdateResurrectionsInProgress(int32 nControllerIndex)

{
    if (!AllLocalPlayersDead())
    {
        return;
    }

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->nControllerIndex != nControllerIndex)
        {



            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

            if (!ptACDPlayer->IsAlive()

                && ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER)
                && !IsActiveForController(ptPlayer->nControllerIndex))
            {
                OpenForPlayer(ptPlayer->nPlayerIndex, TRUE);
            }
        }
    }
}

void OpenForPlayer(const int32 nPlayerIndex, BOOL bForced)
{


    if (nPlayerIndex == -1)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);





    if (!ptACDPlayer || ptACDPlayer->IsAlive())
    {
        return;
    }

    int32 nControllerIndex = ptPlayer->nControllerIndex;

    int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);

    UIID idHUDRoot = UIGame::GetPlayerHUDID(nHudSlot);

    UIID idEntry = UIIDCreateFromParent(idHUDRoot, ".Local.FastAccessMenu.FastAccessSlot");

    UIWindow & tWndEntry = UI_GetWindowFromID(idEntry);
    tWndEntry.SetFilterControllerIndex(nControllerIndex);
    tWndEntry.SetActive(TRUE);

    UIID idIcon;
    idIcon.SetFromParent(idEntry, "Icon");

    UI_GetWindowFromID(idIcon).SetActive(FALSE);

    UIID idName, idInfo;

    idName.SetFromParent(idEntry, "Item");
    idInfo.SetFromParent(idEntry, "Info");

    UIText & tTextName = UI_GetTextFromID(idName);
    tTextName.SetActive(TRUE);

    BOOL bHardcore = (ptPlayer->tHeroStateData.uPlayerFlags & 1) != 0;
    if (bHardcore && !ptACDPlayer->IsDueling())
    {
        tTextName.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerHardcoreDeath"), FALSE);
    }
    else
    {
        tTextName.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathTitle"), FALSE);
    }

    UIText & tTextInfo = UI_GetTextFromID(idInfo);
    tTextInfo.SetActive(TRUE);

    UIID idLegend;
    idLegend.SetFromParent(idEntry, "FastAccessLegend");
    UIText & tLegendText = UI_GetTextFromID(idLegend);
    tLegendText.SetActive(FALSE);

    if (!bForced)
    {
        sUpdateResurrectionsInProgress(nControllerIndex);
    }

    bOpenForController[nControllerIndex] = TRUE;
}


void CloseForController(const int32 nControllerIndex)
{

    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == -1)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);

    UIID idHUDRoot = UIGame::GetPlayerHUDID(nHudSlot);

    UIID idEntry = UIIDCreateFromParent(idHUDRoot, ".Local.FastAccessMenu.FastAccessSlot");

    UIWindow & tWndEntry = UI_GetWindowFromID(idEntry);
    tWndEntry.SetActive(FALSE);

    bOpenForController[nControllerIndex] = FALSE;
}

BOOL AllLocalPlayersDead()

{
    BOOL bAllLocalPlayersDead = TRUE;

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer))
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

            if (ptACDPlayer && ptACDPlayer->IsAlive())
            {
                bAllLocalPlayersDead = FALSE;
                break;
            }
        }
    }

    return bAllLocalPlayersDead;
}


BOOL CanReviveAtCorpse(const int32 nControllerIndex)

{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (!ptPlayer)
    {
        return FALSE;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

    int32 nServerTime = CGameGetPredictedServerTime();
    int32 nResurrectAllowedTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME);
    int32 nResurrectionTimeLeft = nResurrectAllowedTime - nServerTime;

    if (nResurrectionTimeLeft > 0)
    {
        return FALSE;
    }

    int32 nCorpseDelay = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
    int32 nCorpseTimeLeft = nCorpseDelay - nServerTime;

    if (nCorpseTimeLeft > 0)
    {
        return FALSE;
    }

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (AllLocalPlayersDead() && ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
        {
            return FALSE;
        }
    }

    GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
    if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedData.bTieredLootRunFailed)
    {
        int32 nAllowedResTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
        if (nAllowedResTime != 0)
        {
            int32 nRemainingTime = nAllowedResTime - CGameGetPredictedServerTime();
            if (nRemainingTime > 0)
            {
                return FALSE;
            }
        }
    }

    return UIDeathMenu_Console_CanReviveAtCorpse(ptPlayer->idPlayerACD);
}


BOOL PlayerHasResTimer(int32 nControllerIndex)

{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

    int32 nServerTime = CGameGetPredictedServerTime();

    int32 nResurrectAllowedTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME);

    if (nResurrectAllowedTime > nServerTime)
    {
        return TRUE;
    }

    GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
    if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedData.bTieredLootRunFailed)
    {
        int32 nAllowedResTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
        if (nAllowedResTime != 0)
        {
            int32 nRemainingTime = nAllowedResTime - CGameGetPredictedServerTime();
            if (nRemainingTime > 0)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

void Update()

{
    for (int32 nControllerIndex = 0; nControllerIndex < 4; nControllerIndex++)
    {
        if (bOpenForController[nControllerIndex])
        {
            int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

            if (nPlayerIndex == -1)
            {
                continue;
            }

            Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

            if (ptACDPlayer->IsAlive())
            {
                CloseForController(nControllerIndex);
                continue;
            }

            int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);

            UIID idHUDRoot = UIGame::GetPlayerHUDID(nHudSlot);

            UIID idEntry = UIIDCreateFromParent(idHUDRoot, ".Local.FastAccessMenu.FastAccessSlot");

            UIWindow & tWndEntry = UI_GetWindowFromID(idEntry);
            tWndEntry.SetFilterControllerIndex(nControllerIndex);
            tWndEntry.SetActive(TRUE);

            UIID idInfo;

            idInfo.SetFromParent(idEntry, "Info");

            UIText & tTextInfo = UI_GetTextFromID(idInfo);

            UIID idLegend;
            idLegend.SetFromParent(idEntry, "FastAccessLegend");
            UIText & tTextLegend = UI_GetTextFromID(idLegend);

            if (ptPlayer)
            {
                tTextLegend.SetControllerIndexOverride(ptPlayer->nControllerIndex);
            }

            int32 nServerTime = CGameGetPredictedServerTime();

            int32 nResurrectAllowedTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME);
            int32 nResurrectionTimeLeft = nResurrectAllowedTime - nServerTime;

            if (nResurrectionTimeLeft <= 0)
            {
                int32 nCorpseDelay = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
                int32 nCorpseTimeLeft = nCorpseDelay - nServerTime;
                int32 nRemainingResTimeTieredRift = 0;

                GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
                if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedData.bTieredLootRunFailed)
                {
                    int32 nAllowedResTimeTieredRift = ptACDPlayer->AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
                    int32 nRemainingTime = nAllowedResTimeTieredRift - CGameGetPredictedServerTime();
                    nRemainingResTimeTieredRift = nRemainingTime > 0 ? nRemainingTime : 0;
                }




                if (nCorpseTimeLeft > 0 || nRemainingResTimeTieredRift != 0)
                {
                    int32 nMaxCorpseResTime = Math::Max(nCorpseTimeLeft, nRemainingResTimeTieredRift);
                    int32 nSecondsLeft = ((uint32)nMaxCorpseResTime + 59) / 60;
                    CRefString sCountDownFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "DeathMenuCountdown");
                    CRefString sCountDown = LocalizedSprintf(sCountDownFormat, UnicodeText::FormatNumber(nSecondsLeft, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);

                    tTextInfo.SetText(sCountDown.str(), FALSE);
                    tTextInfo.SetActive(TRUE);
                    tTextLegend.SetActive(FALSE);
                }
                else
                {
                    BOOL bHardcore = (ptPlayer->tHeroStateData.uPlayerFlags & 1) != 0;
                    if (bHardcore && !ptACDPlayer->IsDueling())
                    {
                        tTextInfo.SetActive(FALSE);

                        tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerHardcoreDeathAccept"), FALSE);

                        tTextLegend.SetActive(TRUE);
                    }
                    else
                    {

                        tTextInfo.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MutliplayerResOptionHeader"), FALSE);

                        tTextLegend.SetActive(TRUE);

                        if (CanReviveAtCorpse(nControllerIndex))
                        {
                            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

                            if (AllLocalPlayersDead() && !GameTypeIsPvP() && !tACDPlayer.IsDueling())
                            {
                                if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
                                {
                                    tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathUIBlizzcon2014"), FALSE);
                                }
                                else
                                {
                                    GameSyncedData & tSyncedDataScoped = GameGetActiveGameCommonData().tGameSyncedData;
                                    if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedDataScoped.bTieredLootRunFailed)
                                    {
                                        tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathUITieredRift"), FALSE);
                                    }
                                    else
                                    {
                                        tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathUIDefault"), FALSE);
                                    }
                                }
                            }
                            else if (GameTypeIsPvP() || tACDPlayer.IsDueling())
                            {
                                tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathUIPVPOptions"), FALSE);
                            }
                            else
                            {
                                tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathUICorpseRes"), FALSE);
                            }
                        }
                        else
                        {
                            if (ACDIsInTieredLootRun(*ptACDPlayer))
                            {
                                tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathUITieredRift"), FALSE);
                            }
                            else
                            {
                                tTextLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathUIBoss"), FALSE);
                            }
                        }
                    }
                }
            }
            else
            {
                int32 nSecondsLeft = Math::Max((nResurrectionTimeLeft + 59) / 60, 0);
                CRefString sCountDownFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "DeathMenuCountdown");
                CRefString sCountDown = LocalizedSprintf(sCountDownFormat, UnicodeText::FormatNumber(nSecondsLeft, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);

                tTextInfo.SetText(sCountDown.str(), FALSE);
                tTextInfo.SetActive(TRUE);
                tTextLegend.SetActive(FALSE);
            }
        }
    }
}

void UICallback_MultiplayerDeathMenu_OkClickedResInTown(int32 nControllerIndex, const UIID & id)

{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

    if (AllLocalPlayersDead() || GameTypeIsPvP() || ptACDPlayer->IsDueling())
    {
        for (int32 nControllerIndexIt = 0; nControllerIndexIt < 4; nControllerIndexIt++)
        {
            if (bOpenForController[nControllerIndexIt])
            {
                UIDeathMenu_Console_ReviveInTown(GetUIIDNone(), nControllerIndexIt);
                CloseForController(nControllerIndexIt);
            }
        }
    }
}


void OnButton2(const int32 nControllerIndex)

{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }

    if (!bOpenForController[nControllerIndex])
        return;
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

    if (ptPlayer->tHeroStateData.uPlayerFlags & 1)
    {
        return;
    }

    if (PlayerHasResTimer(nControllerIndex))
    {
        return;
    }

    GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
    if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedData.bTieredLootRunFailed)
    {
        return;
    }

    if (AllLocalPlayersDead() || GameTypeIsPvP() || ptACDPlayer->IsDueling())
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathResTitle");
        tParams.uszMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathResInTown");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&UICallback_MultiplayerDeathMenu_OkClickedResInTown>(nControllerIndex);
        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
}


void UICallback_MultiplayerDeathMenu_OkClickedResAtCheckpoint(const UIID & id)

{
    for (int32 nControllerIndex = 0; nControllerIndex < 4; nControllerIndex++)
    {
        if (bOpenForController[nControllerIndex])
        {
            UIDeathMenu_Console_ReviveAtCheckpoint(GetUIIDNone(), nControllerIndex);
            CloseForController(nControllerIndex);
        }
    }
}

BOOL sHandleHardcoreDeath(const int32 nControllerIndex)

{
    if (!bOpenForController[nControllerIndex])
        return FALSE;

    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == -1)
    {
        return FALSE;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    BOOL bHardcore = (ptPlayer->tHeroStateData.uPlayerFlags & 1) != 0;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

    if (bHardcore && !ptACDPlayer->IsDueling())
    {
        UIDeathMenu_Console_HandleHardcoreDeath(nControllerIndex);
        return TRUE;
    }



    return FALSE;
}

void OnButton3(const int32 nControllerIndex)

{



    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == -1)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    BOOL bHardcore = (ptPlayer->tHeroStateData.uPlayerFlags & 1) != 0;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

    if (bHardcore && ptACDPlayer && !ptACDPlayer->IsDueling())
    {
        return;
    }


    Player * ptPlayerScoped = PlayerGetByControllerIndex(nControllerIndex);
    ActorCommonData * ptACDPlayerScoped = ACDTryToGet(ptPlayerScoped->idPlayerACD);

    if (PlayerHasResTimer(nControllerIndex))
    {
        return;
    }

    if (bOpenForController[nControllerIndex] && AllLocalPlayersDead() && ptACDPlayerScoped && !GameTypeIsPvP() && !ptACDPlayerScoped->IsDueling())
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathResTitle");
        tParams.uszMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "MultiplayerDeathResAtCheckpoint");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &UICallback_MultiplayerDeathMenu_OkClickedResAtCheckpoint>();
        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
}


void OnButton4(const int32 nControllerIndex)

{
    if (bOpenForController[nControllerIndex])
    {
        if (CanReviveAtCorpse(nControllerIndex))
        {
            UIDeathMenu_Console_ReviveAtCorpse(GetUIIDNone(), nControllerIndex);
            CloseForController(nControllerIndex);
        }
    }
}


void OnBackButton(const int32 nControllerIndex)

{

    sHandleHardcoreDeath(nControllerIndex);
}


BOOL IsActiveForController(const int32 nControllerIndex)

{
    return bOpenForController[nControllerIndex];
}

void PlayerLeft(const int32 nControllerIndex)

{
    if (bOpenForController[nControllerIndex])
    {
        CloseForController(nControllerIndex);
    }
}

void RefreshForAllPlayers()

{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer->nPlayerIndex))
        {
            int32 nControllerIndex = ptPlayer->nControllerIndex;

            int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);

            UIID idHUDRoot = UIGame::GetPlayerHUDID(nHudSlot);

            UIID idEntry = UIIDCreateFromParent(idHUDRoot, ".Local.FastAccessMenu.FastAccessSlot");

            UIWindow & tWndEntry = UI_GetWindowFromID(idEntry);
            tWndEntry.SetActive(FALSE);

            bOpenForController[nControllerIndex] = FALSE;

            ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);



            if (ptACD && !ptACD->IsAlive())
            {
                OpenForPlayer(ptPlayer->nPlayerIndex, FALSE);
            }
        }
    }
}

}  // namespace Console
}  // namespace UIMultiplayerDeathMenu
