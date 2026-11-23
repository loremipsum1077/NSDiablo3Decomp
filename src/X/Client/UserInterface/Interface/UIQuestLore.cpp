// Original file: X/Client/UserInterface/Interface/UIQuestLore.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIItemButtonControl.h"
#include "../Controls/UIScrollableTextControl.h"
#include "../Controls/UIToolTipControl.h"
#include "../UIIDs.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Sound/Sound.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Items.h"
#include "../../../Common/Quests/Quests.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet (UI.cpp, UIQuests.cpp, QuestDefinition.cpp, GameTemplate.cpp, UIQuestLore.h).
extern UIID UIDLG_MAIN_QUEST, UITXT_MAIN_QUEST_NO_QUESTS, UIWND_MAIN_QUEST_ICON, UIDLG_MAIN_QUEST_WRAPPER, UITXT_MAIN_QUEST_NAME, UITXT_MAIN_QUEST_DESCRIPTION, UIWND_MAIN_QUEST_REWARD_WRAPPER;  // 0x71017d8f98..0x71017d8fc8
extern UIID UIWND_MAIN_QUEST_XP_WRAPPER, UITXT_MAIN_QUEST_XP, UIWND_MAIN_QUEST_GOLD_WRAPPER, UITXT_MAIN_QUEST_GOLD, UIWND_MAIN_QUEST_ITEM_WRAPPER, UIBTN_MAIN_QUEST_ITEM, UITXT_MAIN_QUEST_ITEM;  // 0x71017d8fd0..0x71017d9000
UIItemButton & UI_GetItemButtonFromID(const UIID & id); UIScrollableText & UI_GetScrollableTextFromID(const UIID & id);
namespace UIQuests { BOOL CanShowXPReward(const uint64 nXPGranted); SNO GetMainQuest(); }
CRefString QuestsGetUIName(SNO snoQuest); CRefString QuestStepGetDescription(SNO snoQuest, int32 nStepUID, PlayerClass ePlayerClass); BOOL QuestGetCurrentStepReward(const Player & tPlayer, SNO snoQuest, uint64 & nXPGranted, int32 & nGoldGranted, SNO & snoRecipe, SNO & snoTreasureClass);  // Quests.cpp
void QuestGetIcon(const Act eAct, const SNO snoQuest, const BOOL fCompleted, ImageTextureFrame & tFrame); Act GameGetCurrentAct();









struct UIQuestLoreGlobals
{
    SNO snoCurrentRewardRecipe;
    RecipeTooltipData tTipData;
    BOOL bMainQuestIsDirty;
};


static UIQuestLoreGlobals * sgptUIQuestLoreGlobals;

namespace UIQuestLore {

void InitializeForGame()
{

    sgptUIQuestLoreGlobals = new (SigmaMemoryNew(sizeof(UIQuestLoreGlobals), 4, NULL, TRUE)) UIQuestLoreGlobals();
    sgptUIQuestLoreGlobals->snoCurrentRewardRecipe = SNO_NONE;
    sgptUIQuestLoreGlobals->bMainQuestIsDirty = TRUE;
}



void DisposeForGame()
{

    delete sgptUIQuestLoreGlobals;
    sgptUIQuestLoreGlobals = NULL;
}























































































































































































void ShowLoreDisplayForSNO(const SNO snoLore)
{
}





















































void OnSideScreenShowHide(const UIID &, BOOL fWindowOpen)
{








    if (fWindowOpen)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_QUESTS_OPEN), cgtPSODefaults);
    }
    else
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_QUESTS_CLOSE), cgtPSODefaults);
    }
}






















































static void sSetupQuestInfoControls(const SNO snoQuest)
{

    UIText & tTxtName = UI_GetTextFromID(UITXT_MAIN_QUEST_NAME);







    QuestInstance * ptQuestInst = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (ptQuestInst)
    {



        tTxtName.SetText(QuestsGetUIName(ptQuestInst->snoQuest), FALSE);

        if (ptQuestInst->nCurrentStepUID != -1)
        {
            PlayerClass eClass = PlayerGetPlayerClass(LocalPlayerACDGet());
            CRefString sDescription = QuestStepGetDescription(ptQuestInst->snoQuest, ptQuestInst->nCurrentStepUID, eClass);

            UI_GetScrollableTextFromID(UITXT_MAIN_QUEST_DESCRIPTION).SetText(sDescription);
        }
    }
}



static void sSetupRewardControls(const SNO snoQuest)
{

    UIWindow & tWndRewardWrapper = UI_GetWindowFromID(UIWND_MAIN_QUEST_REWARD_WRAPPER);








    const Player * ptPlayer = LocalPlayerGet();
    uint64 nXPGranted; int32 nGoldGranted; SNO snoRecipe; SNO snoTreasureClassIgnored;
    if (ptPlayer &&







        QuestGetCurrentStepReward(*ptPlayer, snoQuest, nXPGranted, nGoldGranted, snoRecipe, snoTreasureClassIgnored))
    {



        const BOOL fCanShowXPReward = UIQuests::CanShowXPReward(nXPGranted);
        if (nGoldGranted == 0 && !fCanShowXPReward && snoRecipe == SNO_NONE)
        {
            tWndRewardWrapper.SetActive(FALSE);
        }
        else
        {



            tWndRewardWrapper.SetActive(TRUE);

            UIWindow & tWndItemWrapper = UI_GetWindowFromID(UIWND_MAIN_QUEST_ITEM_WRAPPER);
            UIWindow & tWndGoldWrapper = UI_GetWindowFromID(UIWND_MAIN_QUEST_GOLD_WRAPPER);
            UIWindow & tWndXPWrapper = UI_GetWindowFromID(UIWND_MAIN_QUEST_XP_WRAPPER);

            if (snoRecipe != SNO_NONE)
            {

                tWndItemWrapper.SetActive(TRUE);
                tWndGoldWrapper.SetActive(FALSE);
                tWndXPWrapper.SetActive(FALSE);


                UIItemButton & tBtnItem = UI_GetItemButtonFromID(UIBTN_MAIN_QUEST_ITEM);
                tBtnItem.SetActive(TRUE);
                tBtnItem.SetUserData((void *)(intptr)snoRecipe);
                tBtnItem.SetFromRecipe(snoRecipe);

                CRefString sLabel = SNOToString(RECIPE_SNO, snoRecipe, FALSE);
                CRefString usItemName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_RECIPES), sLabel);
                UI_GetTextFromID(UITXT_MAIN_QUEST_ITEM).SetText(usItemName, FALSE);
            }
            else
            {
                tWndItemWrapper.SetActive(FALSE);

                if (nGoldGranted > 0)
                {
                    CRefString sGold = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_QUESTLOG), "QuestRewardGold"), UIText_ValueToGameCurrency(nGoldGranted, CURRENCY_GOLD).str(), NULL);

                    UI_GetTextFromID(UITXT_MAIN_QUEST_GOLD).SetText(sGold, FALSE);
                    tWndGoldWrapper.SetActive(TRUE);
                }
                else
                {
                    tWndGoldWrapper.SetActive(FALSE);
                }

                if (fCanShowXPReward)
                {
                    CRefString sXP = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_QUESTLOG), "QuestRewardXP"), UnicodeText::FormatNumber(nXPGranted, TRUE, FNS_NORMAL, 0).str(), NULL);

                    UI_GetTextFromID(UITXT_MAIN_QUEST_XP).SetText(sXP, FALSE);
                    tWndXPWrapper.SetActive(TRUE);
                }
                else
                {
                    tWndXPWrapper.SetActive(FALSE);
                }
            }
        }
    }
}




void MarkMainQuestAsDirty()
{

    sgptUIQuestLoreGlobals->bMainQuestIsDirty = TRUE;
}




void MainQuestRefresh()
{

    if (!UI_TestID(UIDLG_MAIN_QUEST)) { return; }
    if (!UI_GetWindowFromID(UIDLG_MAIN_QUEST).IsActive()) { return; }




    if (!sgptUIQuestLoreGlobals->bMainQuestIsDirty) { return; }




    UIWindow & tWndIcon = UI_GetWindowFromID(UIWND_MAIN_QUEST_ICON);
    UIWindow & tWndWrapper = UI_GetWindowFromID(UIDLG_MAIN_QUEST_WRAPPER);
    UIText & tTextNoQuests = UI_GetTextFromID(UITXT_MAIN_QUEST_NO_QUESTS);

    const SNO snoQuest = UIQuests::GetMainQuest();
    if (snoQuest != SNO_NONE)
    {
        sSetupQuestInfoControls(snoQuest);
        sSetupRewardControls(snoQuest);

        tWndIcon.SetActive(TRUE);
        tWndWrapper.SetActive(TRUE);
        tTextNoQuests.SetActive(FALSE);

        ImageTextureFrame tTexFrame;
        QuestGetIcon(GameGetCurrentAct(), snoQuest, TRUE, tTexFrame);

        tWndIcon.SetTextureFrame(tTexFrame);
    }
    else
    {
        tWndIcon.SetActive(FALSE);
        tWndWrapper.SetActive(FALSE);
        tTextNoQuests.SetActive(TRUE);
    }

    sgptUIQuestLoreGlobals->bMainQuestIsDirty = FALSE;
}
}  // namespace UIQuestLore
