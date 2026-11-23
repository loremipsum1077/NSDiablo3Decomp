// Original file: X/Client/UserInterface/Interface/UIChallenge.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/World.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Quests/Quests.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
// Declared here because their headers are not adopted yet (CGameMessage.cpp, CPlayer.cpp, QuestDefinition.cpp, UIText.cpp).
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer); ACDID LocalPlayerACDGet();
extern LPCSTR gszQuestDescLabel; extern SymbolicEntry gChallengeTypes[]; CRefString UIText_GetTimeString(const uint32 uSeconds);









class UIChallengeGlobals
{
public:
    UIChallengeGlobals()
    {
        bJoinExisting = FALSE;
        dwJoinTimeout = 0;
        snoChallenge = SNO_NONE;
    }


    BOOL bJoinExisting;
    uint32 dwJoinTimeout;
    SNO snoChallenge;
};


static UIChallengeGlobals sgtUIChallengeGlobals;

namespace UIChallenge {
void Close(); BOOL IsVisible();
void OnOK(const UIID & id)
{

    SNODataMessage tMessage;

    tMessage.snoData = sgtUIChallengeGlobals.snoChallenge;
    MessageSendToServer(467, &tMessage, sizeof(tMessage), LocalPlayerGet());

    Close();
}



void OnCancel(const UIID & id)
{



    Close();
}




void OnButtonOK(const UIID & id, const int32 nControllerIndex)
{


    OnOK(id);
}



void OnButtonCancel(const UIID & id, const int32 nControllerIndex)
{


    OnCancel(id);
}




void Show(SNO snoChallenge, BOOL bJoinExisting)
{



    if (IsVisible() && bJoinExisting)
    {
        return;
    }

    ScopedQuestDefinitionLockR pdefQuest(snoChallenge);

    SNO snoStringList = QuestGetStringList(snoChallenge);
    CRefString sName = StringListFetch(snoStringList, gszQuestNameLabel);
    CRefString sDescription = StringListFetch(snoStringList, gszQuestDescLabel);
    CRefString sTypeLabel("ChallengeType");
    sTypeLabel += SymbolToName(pdefQuest->eChallengeType, gChallengeTypes);
    CRefString sType = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CHALLENGE), sTypeLabel.str());

    UI_GetWindowFromID(UIDLG_CHALLENGE).SetActive(TRUE);
    UI_GetTextFromID(UITXT_CHALLENGE_NAME).SetText(sName, FALSE);
    UI_GetTextFromID(UITXT_CHALLENGE_DESCRIPTION).SetText(sDescription, FALSE);
    UI_GetTextFromID(UITXT_CHALLENGE_TYPE).SetText(sType, FALSE);

    sgtUIChallengeGlobals.snoChallenge = snoChallenge;
    sgtUIChallengeGlobals.bJoinExisting = bJoinExisting;


    if (!bJoinExisting)
    {
        CRefString sStatus = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CHALLENGE), "Start");
        UI_GetTextFromID(UITXT_CHALLENGE_STATUS).SetText(sStatus, FALSE);

        UIText & tTxtTimer = UI_GetTextFromID(UITXT_CHALLENGE_TIMER);
        tTxtTimer.SetText("", FALSE);
    }
    else
    {
        CRefString sStatus = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CHALLENGE), "Started");
        UI_GetTextFromID(UITXT_CHALLENGE_STATUS).SetText(sStatus, FALSE);

        const uint32 dwTimeout = 60;
        sgtUIChallengeGlobals.dwJoinTimeout = CGameGetClientTicks() + dwTimeout * 60;

        UIText & tTxtTimer = UI_GetTextFromID(UITXT_CHALLENGE_TIMER);
        tTxtTimer.SetText(UIText_GetTimeString(dwTimeout), FALSE);
    }
}



BOOL IsVisible()
{

    return UI_GetWindowFromID(UIDLG_CHALLENGE).IsActive();
}



void Close()
{

    UI_GetWindowFromID(UIDLG_CHALLENGE).SetActive(FALSE);
    sgtUIChallengeGlobals.snoChallenge = SNO_NONE;
    sgtUIChallengeGlobals.bJoinExisting = FALSE;
    sgtUIChallengeGlobals.dwJoinTimeout = 0;
}



void OnUpdate(
    const UIID & id)
{


    const ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());

    if (IsVisible())
    {

        uint32 dwNow = CGameGetClientTicks();
        if (sgtUIChallengeGlobals.bJoinExisting &&
            dwNow > sgtUIChallengeGlobals.dwJoinTimeout)
        {
            Close();
            return;
        }





        ScopedQuestDefinitionLockR pdefQuest(sgtUIChallengeGlobals.snoChallenge);
        if (pdefQuest->snoChallengeWorld != SNO_NONE)
        {
            if (ptACD)  // inferred: the ptACD test leaves no row of its own (190:24 is the member load; 194 sits at 16 spaces)
            {
                if (WorldCommonGetWorldSNO(ptACD->GetSWorldID()) == pdefQuest->snoChallengeWorld)
                {
                    Close();
                    return;
                }
            }
        }


        if (ptACD && ptACD->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
        {
            Close();
            return;
        }


        if (sgtUIChallengeGlobals.bJoinExisting)
        {
            uint32 dwSecondsRemaining = (sgtUIChallengeGlobals.dwJoinTimeout - dwNow) / 60;
            UIText & tTxtTimer = UI_GetTextFromID(UITXT_CHALLENGE_TIMER);
            tTxtTimer.SetText(UIText_GetTimeString(dwSecondsRemaining), FALSE);
        }
        else
        {
            UIText & tTxtTimer = UI_GetTextFromID(UITXT_CHALLENGE_TIMER);
            tTxtTimer.SetText("", FALSE);
        }


        if (!sgtUIChallengeGlobals.bJoinExisting)
        {
            QuestInstance * ptQuestInstance = QuestsGetQuestInstance(sgtUIChallengeGlobals.snoChallenge, ACDID_INVALID);
            if (ptQuestInstance &&

                ptQuestInstance->eInstanceState != QUEST_INSTANCE_UNASSIGNED)
            {
                CRefString sStatus = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CHALLENGE), "Started");
                UI_GetTextFromID(UITXT_CHALLENGE_STATUS).SetText(sStatus, FALSE);
            }
        }
    }
}

}  // namespace UIChallenge
