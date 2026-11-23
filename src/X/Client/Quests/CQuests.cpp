// Original file: X/Client/Quests/CQuests.cpp

#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Definitions/QuestDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Quests/Quests.h"
#include "../../Common/Quests/QuestsSwig.h"

// Declared here because their headers do not carry them yet (signatures from the DWARF).
BOOL QuestIsACDObjective(const ActorCommonData & tACD);  // Quests.cpp:2094
BOOL QuestsACDUnlocksObjective(const ActorCommonData & tACD);  // Quests.cpp:2240
void UIMinimap_ShowForUpdate();  // UIMinimap.cpp:5565
namespace UIQuests { void QueueUpdate(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID, const BOOL bDisplayButton, const BOOL bFailed, const int32 nPreviousStepUID, const int32 nObjectiveIndex, const int32 nValue, const BOOL bPassed); }  // UIQuests.cpp:1106
void CQuestsFlagACD(ActorCommonData & tACD); void CQuestsUnFlagACD(ActorCommonData & tACD);

namespace {








int32 nTrackedACDS = 0;


void sUpdateQuestMobFlags()
{

    BOOL bCheckForUnflag = nTrackedACDS != 0;

    nTrackedACDS = 0;

    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;
        if (!ptACD)
        {
            continue;
        }
        if (bCheckForUnflag)
        {
            if ((ptACD->dwFlags & (1 << ACD_QUEST_OBJECTIVE)) &&

                !QuestIsACDObjective(*ptACD))
            {
                CQuestsUnFlagACD(*ptACD);
            }
        }

        if (QuestIsACDObjective(*ptACD))
        {
            CQuestsFlagACD(*ptACD);
        }
    }
}
}


static QuestInstance * sCreateAndAdvanceQuest(SNO snoQuest,
                                              SNO snoLevelArea,
                                              int32 nStepUID,
                                              int32 & nPrevObjID,
                                              BOOL & bCreated)
{
    bCreated = FALSE;

    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);


    if (!ptQuestInstance)
    {


        ScopedQuestDefinitionLockR pdefQuest(snoQuest);
        if (pdefQuest->eQuestType == QUEST_TYPE_MAIN_QUEST && pdefQuest->eAct != ACT_INVALID)
        {






            QuestsActivateForAct(pdefQuest->eAct);

            ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
        }

        if (!ptQuestInstance)
        {






















            GameCommonData & tGCD = GameGetActiveGameCommonData();
            QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;

            QuestInstance tQuestInstance;
            QuestsQuestInstanceInitialize(tQuestInstance, snoQuest, snoLevelArea);
            ptQuestGlobals->tQuestInstanceList.AddTail_Tracked(tQuestInstance);
            ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
            bCreated = TRUE;
        }
    }




    nPrevObjID = ptQuestInstance->nCurrentStepUID;
    QuestsAdvanceToStep(*ptQuestInstance, nStepUID, FALSE);

    sUpdateQuestMobFlags();

    return ptQuestInstance;
}



void CQuestsUpdateLog(const QuestUpdateMessage & tMsg)
{



    int32 nPrevObjID;
    BOOL bCreated;
    QuestInstance * ptQuestInstance = sCreateAndAdvanceQuest(tMsg.snoQuest, tMsg.snoLevelArea, tMsg.nStepUID, nPrevObjID, bCreated);


    if (tMsg.bFailed)
    {
        ptQuestInstance->eInstanceState = QUEST_INSTANCE_FAILED;
    }


    if (!(tMsg.bFailed && bCreated))
    {
        UIQuests::QueueUpdate(tMsg.snoQuest, tMsg.snoLevelArea, tMsg.nStepUID, tMsg.bDisplayButton, tMsg.bFailed, nPrevObjID, -1, -1, FALSE);
    }

    sUpdateQuestMobFlags();


    UIMinimap_ShowForUpdate();

}





void CQuestsOnInventoryUpdate(const ActorCommonData & tACD)
{



    if (QuestsACDUnlocksObjective(tACD))
    {
        sUpdateQuestMobFlags();
    }
}



void CQuestsUpdateCounters(SNO snoQuest,
                           SNO snoLevelArea,
                           int32 nStepUID,
                           int32 nObjectiveIndex,
                           int32 nValue,
                           BOOL bPassed)
{


    int32 nPrevObjID;
    BOOL bCreated;
    QuestInstance * ptQuestInstance = sCreateAndAdvanceQuest(snoQuest, snoLevelArea, nStepUID, nPrevObjID, bCreated);



    ptQuestInstance->arObjectiveState[nObjectiveIndex] = nValue;

    UIQuests::QueueUpdate(snoQuest, snoLevelArea, nStepUID, FALSE, FALSE, nPrevObjID, nObjectiveIndex, nValue, bPassed);

    sUpdateQuestMobFlags();
}




void CQuestsUpdateMeter(const QuestMeterMessage & tMsg)
{
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(tMsg.snoQuest, ACDID_INVALID);
    if (ptQuestInstance)
    {
        ptQuestInstance->idACDMeter = ACDFromANN(tMsg.annMeter);
        ptQuestInstance->flMeter = tMsg.flMeter;
    }

    sUpdateQuestMobFlags();
}





void CQuestsFlagACD(
    ActorCommonData & tACD)
{
    SNO snoEffectGroup;
    if (tACD.IsGizmo())
    {




        if (!tACD.IsOperatableGizmo(ACDID_INVALID, TRUE))
        {
            return;
        }

        snoEffectGroup = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_QUEST_GIZMO);
    }
    else {
        snoEffectGroup = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_QUEST_MOB);
    }


    tACD.AddClientEffect(snoEffectGroup, 0);

    if (!tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_QUEST_MONSTER)))
    {

        tACD.dwFlags |= (1 << ACD_QUEST_OBJECTIVE);
    } nTrackedACDS++; }



void CQuestsUnFlagACD(
    ActorCommonData & tACD)
{
    SNO snoEffectGroup;
    if (tACD.IsGizmo())
    {
        snoEffectGroup = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_QUEST_GIZMO);
    }
    else
    {

        snoEffectGroup = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_QUEST_MOB);
    }

    tACD.RemoveClientEffect(snoEffectGroup);


    tACD.dwFlags &= ~(1 << ACD_QUEST_OBJECTIVE);

    nTrackedACDS--;
}

void CQuestsUpdateAllActors()
{

    sUpdateQuestMobFlags();
}
