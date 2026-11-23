// Original file: X/Client/UserInterface/Console/UIConsoleDeathMenu.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Console/GamerProfile.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "UIConsoleDeathMenuHardcore.h"
#include "UIConsoleMultiplayerDeathMenu.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GameTemplate.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
extern UIID UIDLG_CONSOLE_DEATHMENU, UITEXT_CONSOLE_DEATHMENU_TITLE, UITEXT_CONSOLE_DEATHMENU_TIMER, UITEXT_CONSOLE_DEATHMENU_LEGEND, UIWND_CONSOLE_DEATHMENU_TITLE_ENGLISH, UITXT_CONSOLE_DEATHMENU_TITLE_LOCALIZED, UITXT_CONSOLE_DEATHMENU_REST_XP_MESSAGE, UIDLG_CONSOLE_HARDCORE_DEATHMENU; BOOL PlayerCanReviveAtCorpse(const ActorCommonData & tACDPlayer); BOOL PlayerAffectedByDurability(const ActorCommonData & tACDPlayer);  // X/Common/Player.cpp:5001, :5096
namespace UIMultiplayerDeathMenu { namespace Console { void OpenForPlayer(const int32 nPlayerIndex, BOOL bForced); BOOL AllLocalPlayersDead(); } } namespace UIHUD { namespace Console { BOOL CheckHadBonusXPFlag(); } } namespace UIUtils { namespace Console { BOOL IsJapaneseVersion(); } }  // UIConsoleHUD.cpp, UIConsoleUtils.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer); /* X/Client/Network/CGameMessage.cpp */ UIIDList & UI_GetInGameDlgStack(); void UI_PushDialog(UIIDList & listMenuStack, const UIID & idNewMenu, const BOOL fCloseCurrent, const BOOL fSetFocusToNewDialog); void UI_PopDialog(UIIDList & listDialogStack, BOOL fShowPrevious); const UIID & UI_GetCapture(); void UI_SetCapture(const UIID & idCapture); void UIStartup_CleanupForResurrect();  // X/Client/UserInterface/UI.cpp; UIStartup.cpp:455; local declarations stand in for headers not adopted yet

void sOnReviveAtCorpse(Player * ptPlayer)
{

    IntDataMessage tMessage;
    tMessage.nData = 2;
    MessageSendToServer(CMSG_ACCEPTED_DEATH, &tMessage, sizeof(tMessage), ptPlayer);
}



BOOL UIDeathMenu_Console_CanReviveAtCorpse(ACDID idACDPlayer)

{
    const ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
    {
        return FALSE;
    }

    return PlayerCanReviveAtCorpse(*ptACDPlayer);
}



BOOL UIDeathMenu_Console_ValidateDeadPlayer(const Player * ptPlayer)
{
    if (ptPlayer)
    {

        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD && !ptACD->IsAlive())
        {
            return TRUE;
        }
    }
    return FALSE;
}





BOOL sAllPlayersDead()

{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->flHealthPercent > 0.000001f)
        {
            return FALSE;
        }
    }
    return TRUE;
}




void sConsoleOnReviveAtCheckpoint(const UIID &, int32 nControllerIndex)


{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }

    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == -1)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    IntDataMessage tMessage;
    tMessage.nData = 1;
    MessageSendToServer(CMSG_ACCEPTED_DEATH, &tMessage, sizeof(tMessage), ptPlayer);
}


void sConsoleOnReviveInTown(const UIID &, int32 nControllerIndex)


{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }

    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);
    if (nPlayerIndex == -1)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
    {
        return;
    }


    if (!UIMultiplayerDeathMenu::Console::AllLocalPlayersDead() && !ptACDPlayer->IsDueling())
    {
        return;
    }

    GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
    if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedData.bTieredLootRunFailed)
    {
        return;
    }

    IntDataMessage tMessage;
    tMessage.nData = 0;
    MessageSendToServer(CMSG_ACCEPTED_DEATH, &tMessage, sizeof(tMessage), ptPlayer);
}




void UIDeathMenu_Console_ReviveAtCorpse(const UIID &, const int32 nControllerIndex)


{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (!UIDeathMenu_Console_ValidateDeadPlayer(ptPlayer))
        return;
    if (UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_LEGEND).IsActive() || UIMultiplayerDeathMenu::Console::IsActiveForController(nControllerIndex))
    {
        const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer
            && PlayerCanReviveAtCorpse(*ptACDPlayer))
        {
            sOnReviveAtCorpse(ptPlayer);
        }
    }
}




void UIDeathMenu_Console_ReviveAtCheckpoint(const UIID & id, const int32 nControllerIndex)


{

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    if (!UIDeathMenu_Console_ValidateDeadPlayer(ptPlayer))
        return;
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    if (tACDPlayer.IsDueling())
        return;
    if (UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_LEGEND).IsActive() || UIMultiplayerDeathMenu::Console::IsActiveForController(nControllerIndex))
    {
        sConsoleOnReviveAtCheckpoint(id, ptPlayer->nControllerIndex);
    }
}




void sOnWarningAccepted(int32 nControllerIndex, const UIID &)
{
    const Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        IntDataMessage tMessage;
        tMessage.nData = 1;
        MessageSendToServer(CMSG_ACCEPTED_DEATH, &tMessage, sizeof(tMessage), ptPlayer);
    }
}







void LeaveGameWarning(int32 nControllerIndex)

{
    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;

    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
    {
        tParams.uszMessage = StringListFetch("Console:MultiplayerHardcoreDeathAcceptPrimaryUser");
    }
    else
    {
        tParams.uszMessage = StringListFetch("Console:MultiplayerHardcoreDeathAcceptNonPrimaryUser");
    }

    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&sOnWarningAccepted>(nControllerIndex);
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}



void UIDeathMenu_Console_HandleHardcoreDeath(const int32 nControllerIndex)

{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    if (UIDeathMenu_Console_ValidateDeadPlayer(ptPlayer) && (ptPlayer->tHeroStateData.uPlayerFlags & 1))
    {
        LeaveGameWarning(nControllerIndex);
    }
}




void UIDeathMenu_Console_ReviveInTown(const UIID & id, const int32 nControllerIndex)


{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);

    if (!UIDeathMenu_Console_ValidateDeadPlayer(ptPlayer))
        return;
    if (UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_LEGEND).IsActive() || UIMultiplayerDeathMenu::Console::IsActiveForController(nControllerIndex))
    {
        sConsoleOnReviveInTown(id, ptPlayer->nControllerIndex);
    }
}




void UIDeathMenu_Console_Init(int32 nPlayerIndex)

{

    if (PlayerGetLocalPlayerCount() > 1)
    {
        UIMultiplayerDeathMenu::Console::OpenForPlayer(nPlayerIndex, FALSE);
        return;
    }


    Player * ptPlayer = LocalPlayerGet();

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    BOOL bHardcore = (ptPlayer->tHeroStateData.uPlayerFlags & 1) != 0;
    if (bHardcore && !tACDPlayer.IsDueling())
    {
        UIDeathMenuHardcore::Console::OpenForPlayer(nPlayerIndex);
        return;
    }

    UI_PushDialog(UI_GetInGameDlgStack(), UIDLG_CONSOLE_DEATHMENU, TRUE, TRUE);

    CRefString sStr;
    if (PlayerAffectedByDurability(tACDPlayer) && !tACDPlayer.IsDueling())
    {
        int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);

        ScopedGameBalanceDefinitionLockR ptExpTableDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;

        float flDurabilityPctLoss = tExpTable.ptExperienceLevels[nLevel - 1].flDurabilityLossPct;

        CRefString sLabel("DeathDurabilityLoss");
        int32 nCost = ItemCrafting_GetRepairAllCost(ptPlayer, TRUE) * 2;
        if (nCost > 0
            && !tACDPlayer.CanAffordCurrencyCost(nCost, 0))
        {

            sLabel = "DeathDurabilityLossLowGold";
        }

        CRefString sFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), sLabel);
        sStr = LocalizedSprintf(sFormat, UnicodeText::FormatNumber(flDurabilityPctLoss, TRUE, FNS_PERCENT, 0, 0, (XLocale)0).str(), NULL);

        UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_TITLE).SetText(sStr.str(), FALSE);
        UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_TITLE).SetActive(TRUE);
    }
    else
    {
        UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_TITLE).SetActive(FALSE);
    }
    BOOL bShowLocalizedTitle = !UIUtils::Console::IsJapaneseVersion();
    UI_GetTextFromID(UITXT_CONSOLE_DEATHMENU_TITLE_LOCALIZED).SetActive(bShowLocalizedTitle);
    UI_GetWindowFromID(UIWND_CONSOLE_DEATHMENU_TITLE_ENGLISH).SetActive(!bShowLocalizedTitle);


    UI_GetWindowFromID(UITXT_CONSOLE_DEATHMENU_REST_XP_MESSAGE).SetActive(UIHUD::Console::CheckHadBonusXPFlag());
}



void UIDeathMenu_Console_Display()

{
    UIWindow & tDeathMenu = UI_GetWindowFromID(UIDLG_CONSOLE_DEATHMENU);
    if (tDeathMenu.IsActive())
    {
        ACDID idACDPlayer = LocalPlayerACDGet();
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (!ptACDPlayer || ptACDPlayer->IsAlive())
        {
            UI_PopDialog(UI_GetInGameDlgStack(), TRUE);
            UIStartup_CleanupForResurrect();
        }
        else if (!GameTypeIsPvP())
        {




            if (UI_GetCapture() == cgUIIDMainScreen)
            {
                UI_SetCapture(GetUIIDNone());
            }

            UIText & tTimer = UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_TIMER);
            UIText & tLegend = UI_GetTextFromID(UITEXT_CONSOLE_DEATHMENU_LEGEND);
            tLegend.SetControllerIndexOverride(::Console::GamerProfile::GetPrimaryProfileUserIndex());

            int32 nServerTime = CGameGetPredictedServerTime();

            int32 nResurrectAllowedTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME);
            int32 nResurrectionTimeLeft = nResurrectAllowedTime - nServerTime;

            if (nResurrectionTimeLeft <= 0)
            {
                nResurrectAllowedTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
                nResurrectionTimeLeft = nResurrectAllowedTime - nServerTime;

                int32 nRemainingResTimeTieredRift = 0;
                BOOL bIsSoloPlayerInTieredRift = FALSE;
                GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;

                if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedData.bTieredLootRunFailed)
                {
                    nRemainingResTimeTieredRift = ptACDPlayer->AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME)
                        - CGameGetPredictedServerTime();
                    nRemainingResTimeTieredRift = Math::Max(nRemainingResTimeTieredRift, 0);




                    bIsSoloPlayerInTieredRift = CGameGlobalsGet().nDungeonFinderParticipatingPlayerCount == 1;
                }


                if (nResurrectionTimeLeft > 0 || (!bIsSoloPlayerInTieredRift && nRemainingResTimeTieredRift != 0))
                {

                    uint32 nRemainingTime = ((uint32)Math::Max(nResurrectionTimeLeft, nRemainingResTimeTieredRift) + 59) / 60;
                    CRefString sCountDownFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "DeathMenuCountdown");
                    CRefString sCountDown = LocalizedSprintf(sCountDownFormat, UnicodeText::FormatNumber((int32)nRemainingTime, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);
                    tTimer.SetText(sCountDown.str(), FALSE);
                    tTimer.SetActive(TRUE);
                    tLegend.SetActive(FALSE);
                }
                else
                {

                    tTimer.SetActive(FALSE);
                    tLegend.SetActive(TRUE);

                    if (PlayerCanReviveAtCorpse(*ptACDPlayer))
                    {
                        if (ptACDPlayer->IsDueling())
                        {
                            tLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "DeathUIDueling"), FALSE);
                        }


                        else if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
                        {
                            tLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "DeathUIBlizzcon2014"), FALSE);
                        }


                        else if (ACDIsInTieredLootRun(*ptACDPlayer) && !tSyncedData.bTieredLootRunFailed)
                        {

                            if (nRemainingResTimeTieredRift == 0)
                            {
                                tLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "DeathUITieredRift"), FALSE);
                            }
                            else
                            {
                                uint32 nSecondsLeft = ((uint32)nRemainingResTimeTieredRift + 59) / 60;
                                CRefString sLegendFormat = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "DeathUITieredRiftCountdown");
                                CRefString sLegend = LocalizedSprintf(sLegendFormat, UnicodeText::FormatNumber((int32)nSecondsLeft, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);
                                tLegend.SetText(sLegend.str(), FALSE);
                            }
                        }
                        else
                        {

                            tLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "DeathUIDefault"), FALSE);
                        }
                    }
                    else
                    {


                        tLegend.SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), "DeathUIBoss"), FALSE);
                    }
                }
            }
            else
            {

                int32 nRemainingTime = Math::Max((nResurrectionTimeLeft + 59) / 60, 0);
                CRefString sCountDownFormat = (sAllPlayersDead() && PlayersGetCount() > 1) ? StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_UI), "DeathMenuAllDead") : StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), "DeathMenuCountdown");
                CRefString sCountDown = LocalizedSprintf(sCountDownFormat, UnicodeText::FormatNumber(nRemainingTime, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);
                tTimer.SetText(sCountDown.str(), FALSE);
                tTimer.SetActive(TRUE);
                tLegend.SetActive(FALSE);
            }
        }
    }
}

BOOL UIDeathMenu_Console_IsADeathScreenActive()

{
    return UI_GetWindowFromID(UIDLG_CONSOLE_DEATHMENU).IsActive()
        || UI_GetWindowFromID(UIDLG_CONSOLE_HARDCORE_DEATHMENU).IsActive();
}
