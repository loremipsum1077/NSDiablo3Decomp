#include "Quests.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../GlobalSNO.h"
#include "../Player.h"
#include "../Shell/CmdLine.h"
#include "../Definitions/ActorDefinition.h"
#include "../Definitions/ActDefinition.h"
#include "../Act.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../TagMaps/ActorTags.h"
#include "../TagMaps/Tags.h"
#include "../Monsters.h"
#include "../World.h"
#include "../Definitions/GlobalDefinition.h"
#include "../Networking/GameMessage.h"
#include "../../Server/PlayerList.h"
#include "../../Server/Quests/SQuests.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Definitions/StringListDefinition.h"
#include "../../Server/SGame.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/LanguageRules.h"
#include "../../Sigma/MemoryPools.h"

struct QuestAppGlobals {  // size 0x10
    int32 nNumQuests;  // 0x00
    SNO * arQuestSNOs;  // 0x08
};

#include "../NephalemRiftCommon.h"
// DWARF X/Common/Player.cpp:2996; declaration of the existing reward-history query.
BOOL PlayerHasReceivedQuestStepReward(const Player & tPlayer, SNO snoQuest, int32 nStepUID);
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
// DWARF GfxVB.h:351-354 supplies the original four normal-color initializers.
#include "../../Client/Gfx/GfxVB.h"

































extern const uint32 gdwQuestUpdateRate = 120;
extern const uint32 gdwBountySceneActivationUpdateRate = 1;














// Inference: tables below recover original constructor arguments using DWARF fields; empty strings and scalar bounds are confirmed in the original initializer.
static const TypeDescriptorField QuestUpdateMessage_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(QuestUpdateMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestUpdateMessage, snoQuest), "", QUEST_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestUpdateMessage, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(QuestUpdateMessage, nStepUID), NULL, 1), // Inference: the next field encodes its upper bound 1 as const void*, as the original constructor does.
    BoundedTypeDescriptorField("", DT_INT, offsetof(QuestUpdateMessage, bDisplayButton), NULL, NULL, (const void *)0x1), // Inference: the next field encodes its upper bound 1 as const void*, as the original constructor does.
    BoundedTypeDescriptorField("", DT_INT, offsetof(QuestUpdateMessage, bFailed), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestUpdateMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_QuestUpdateMessage("QuestUpdateMessage", 0, QuestUpdateMessage_Fields, 7, NULL, 1u << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUEST_UPDATE_MESSAGE = &TypeDescriptor_QuestUpdateMessage;

static const TypeDescriptorField QuestMeterMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(QuestMeterMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestMeterMessage, snoQuest), "", QUEST_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(QuestMeterMessage, annMeter), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(QuestMeterMessage, flMeter), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestMeterMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_QuestMeterMessage("QuestMeterMessage", 0, QuestMeterMessage_Fields, 5, NULL, 1u << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUEST_METER_MESSAGE = &TypeDescriptor_QuestMeterMessage;

static const TypeDescriptorField QuestFlagsMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(QuestFlagsMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestFlagsMessage, snoQuest), "", QUEST_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(QuestFlagsMessage, dwFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestFlagsMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_QuestFlagsMessage("QuestFlagsMessage", 0, QuestFlagsMessage_Fields, 4, NULL, 1u << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUEST_FLAGS_MESSAGE = &TypeDescriptor_QuestFlagsMessage;

static const TypeDescriptorField QuestCounterMessage_Fields[8] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(QuestCounterMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestCounterMessage, snoQuest), "", QUEST_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestCounterMessage, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(QuestCounterMessage, nStepUID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestCounterMessage, nObjectiveIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestCounterMessage, nValue), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestCounterMessage, bCompleted), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestCounterMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_QuestCounterMessage("QuestCounterMessage", 0, QuestCounterMessage_Fields, 8, NULL, 1u << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUEST_COUNTER_MESSAGE = &TypeDescriptor_QuestCounterMessage;

static const TypeDescriptorField DynamicQuestObjectiveMessage_Fields[16] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DynamicQuestObjectiveMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, nUID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, eObjectiveType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, nParam1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, nParam2), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(DynamicQuestObjectiveMessage, snoname1), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(DynamicQuestObjectiveMessage, snoname2), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(DynamicQuestObjectiveMessage, gbidParam1), NULL, 1), // Inference: the next field encodes its upper bound 1 as const void*, as the original constructor does.
    TypeDescriptorField("", DT_GBID, offsetof(DynamicQuestObjectiveMessage, gbidParam2), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, bHiddenUntilComplete), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, dwFlags), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(DynamicQuestObjectiveMessage, gbidItemToShow), "", GB_ITEMS, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, nState), NULL, 1), // Inference: the next field encodes its upper bound 1 as const void*, as the original constructor does.
    BoundedTypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, bCompleted), NULL, NULL, (const void *)0x1), // Inference: the next field encodes its upper bound 1 as const void*, as the original constructor does.
    BoundedTypeDescriptorField("", DT_INT, offsetof(DynamicQuestObjectiveMessage, bHidden), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DynamicQuestObjectiveMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_DynamicQuestObjectiveMessage("DynamicQuestObjectiveMessage", 0, DynamicQuestObjectiveMessage_Fields, 16, NULL, 1u << TD_IS_COMPLEX_TYPE, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DYNAMIC_QUEST_OBJECTIVE_MESSAGE = &TypeDescriptor_DynamicQuestObjectiveMessage;







QuestGlobals * QuestsGetQuestGlobals()
{

    return GameGetActiveGameCommonData().ptQuestGlobals;
}



void QuestsInitializeForApp()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;


    tAppGlobals.ptQuestAppGlobals = (QuestAppGlobals *)SigmaMemoryNew(sizeof(QuestAppGlobals), 4, NULL, TRUE);

    PooledSNONameList listSNOFiles(GetSnoNameListPool());
    SNOFileEnumerateSNOsForGroup(QUEST_SNO, &listSNOFiles);

    POS pos = listSNOFiles.GetHeadPosition();


    tAppGlobals.ptQuestAppGlobals->nNumQuests = listSNOFiles.Count();
    tAppGlobals.ptQuestAppGlobals->arQuestSNOs = (SNO *)SigmaMemoryNew(sizeof(SNO) * listSNOFiles.Count(), 4, NULL, FALSE);
    int32 nQuest = 0;
    while (pos)
    {
        SNOName & snonameItem = listSNOFiles.GetNext(pos);

        tAppGlobals.ptQuestAppGlobals->arQuestSNOs[nQuest++] = snonameItem.snoHandle;
    }


}



void QuestsDisposeForApp()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;

    SigmaMemoryFree(tAppGlobals.ptQuestAppGlobals->arQuestSNOs, NULL); tAppGlobals.ptQuestAppGlobals->arQuestSNOs = NULL;
    SigmaMemoryFree(tAppGlobals.ptQuestAppGlobals, NULL); tAppGlobals.ptQuestAppGlobals = NULL;
}
BOOL GameCommonGameFlagIsSet(LPCSTR szFlagLabel);  // X/Common/GameCommonData.cpp:500 (declared here: no src header declares it)
Act ActEnumFromActIndex(const int32 nIndex);  // X/Common/GameCommonData.cpp:380 (declared here: no src header declares it)
void SQuestsSendUpdateToAllPlayers(SNO snoQuest);  // X/Server/Quests/SQuests.cpp:5177 (declared here: no src header declares it)
void QuestsLoad(SNO snoQuest, SNO snoLevelArea);  // defined below (Quests.cpp:701); forward declaration for sQuestsPickBounties




static void sQuestsRemoveIneligibleBounties(
    PooledSNONameList & listSNOFiles)
{

    if (!GameCommonGameFlagIsSet("RetroAnniversaryEvent_Dungeon") && !GameCommonGameFlagIsSet("RetroAnniversaryEvent_Precursor"))
    {
        return;
    }

    SNO snoQuestAnniversaryEventDisabledBounty = GlobalSNOGet(GLOBALSNO_QUEST_ANNIVERSARY_EVENT_DISABLED_BOUNTY);

    POS posSNOs = listSNOFiles.GetHeadPosition();
    while (posSNOs)
    {
        POS posPrevious = posSNOs;
        SNO snoBountyQuest = listSNOFiles.GetNext(posSNOs).snoHandle;

        if (snoBountyQuest == snoQuestAnniversaryEventDisabledBounty)
        {
            listSNOFiles.RemoveAt(posPrevious);
            // Inference: the original leaves the loop right after the removal (no line row; the RemoveAt tail jumps to the end).
            break;
        }
    }
}
static void sQuestsRemoveConflictingBounties(
    PooledSNONameList & listSNOFiles,
    const QuestDefinition * ptChosenBounty)
{
    POS posSNOs = listSNOFiles.GetHeadPosition();
    while (posSNOs)
    {
        POS posPrevious = posSNOs;
        SNO snoQuest = listSNOFiles.GetNext(posSNOs).snoHandle;
        ScopedQuestDefinitionLockR pdefQuest(snoQuest);

        if (snoQuest == ptChosenBounty->tHeader.snoHandle)
        {
            listSNOFiles.RemoveAt(posPrevious);
        }
        else if (QuestIsAnyBountyType(pdefQuest))
        {
            if (pdefQuest->tBountyData.snoLevelArea == ptChosenBounty->tBountyData.snoLevelArea)
            {
                listSNOFiles.RemoveAt(posPrevious);
            }
        }
    }
}
// NON_MATCHING: 3364 vs 3444 bytes. Calls, constants and control flow agree; differences: the inlined blz::vector<int>::push_back grow (original keeps separate size/capacity induction values: lsr+add+sub, csel vs our add-lsr, csinc; header vector.h), placement of the shared pick-order store block (original after the push loops, ours at the loop top), and register allocation/spill slots (frame 0x130 vs 0x120). Tried switch case orders, an if/else-if chain (line 313 column 27 is the || of it; matches the compare chain), loop-condition order at 377.
void sQuestsPickBounties()

{




















    // Inference: nHQBountyCounts (DWARF line 271) has no location, constant or use; its initializer (if any) is unknown.
    int32 nHQBountyCounts[NUM_HQ_BOUNTY_TYPES];





    PooledSNONameList listSNOFiles(GetSnoNameListPool());
    SNOFileEnumerateSNOsForGroup(QUEST_SNO, &listSNOFiles);

    sQuestsRemoveIneligibleBounties(listSNOFiles);

    ScopedGlobalDefinitionLockR pdefGlobals;
    GlobalServerData * ptServerData = pdefGlobals->ptServerData;
    for (int32 nAct = 0; nAct < 5; nAct++)
    {
        Act eAct = ActEnumFromActIndex(nAct);

        int32 nTotalBounties = RandRange_Internal(SGameGlobalsGet().rng, ptServerData->nOpenWorldBountiesTotalMin, ptServerData->nOpenWorldBountiesTotalDelta);

        int32 arrBountyCounts[NUM_NORMAL_BOUNTY_TYPES] =
        {
            ptServerData->nOpenWorldBountiesKillBossMin,
            ptServerData->nOpenWorldBountiesClearDungeonMin,
            ptServerData->nOpenWorldBountiesCompleteEventMin,
            ptServerData->nOpenWorldBountiesKillUniqueMin,
            ptServerData->nOpenWorldBountyCampMin,
        };

        int32 arrBountyDeltas[NUM_NORMAL_BOUNTY_TYPES] =
        {
            ptServerData->nOpenWorldBountiesKillBossDelta,
            ptServerData->nOpenWorldBountiesClearDungeonDelta,
            ptServerData->nOpenWorldBountiesCompleteEventDelta,
            ptServerData->nOpenWorldBountiesKillUniqueDelta,
            ptServerData->nOpenWorldBountyCampDelta,
        };

        int32 arrBountyPickOrder[NUM_NORMAL_BOUNTY_TYPES];



        // Inference: the per-act orders below come from the merged stores at +0x64c (no line rows of their own); no default case (other acts store nothing).
        if (eAct == ACT_1 || eAct == ACT_2 || eAct == ACT_3)
        {
            arrBountyPickOrder[0] = BOUNTY_TYPE_CAMPS;
            arrBountyPickOrder[1] = BOUNTY_TYPE_KILL_BOSS;
            arrBountyPickOrder[2] = BOUNTY_TYPE_CLEAR_DUNGEON;
            arrBountyPickOrder[3] = BOUNTY_TYPE_COMPLETE_EVENT;
            arrBountyPickOrder[4] = BOUNTY_TYPE_KILL_UNIQUE;
        }
        else if (eAct == ACT_4)
        {
            arrBountyPickOrder[0] = BOUNTY_TYPE_CLEAR_DUNGEON;
            arrBountyPickOrder[1] = BOUNTY_TYPE_KILL_BOSS;
            arrBountyPickOrder[2] = BOUNTY_TYPE_COMPLETE_EVENT;
            arrBountyPickOrder[3] = BOUNTY_TYPE_CAMPS;
            arrBountyPickOrder[4] = BOUNTY_TYPE_KILL_UNIQUE;
        }
        else if (eAct == ACT_5)
        {
            arrBountyPickOrder[0] = BOUNTY_TYPE_KILL_BOSS;
            arrBountyPickOrder[1] = BOUNTY_TYPE_CLEAR_DUNGEON;
            arrBountyPickOrder[2] = BOUNTY_TYPE_COMPLETE_EVENT;
            arrBountyPickOrder[3] = BOUNTY_TYPE_CAMPS;
            arrBountyPickOrder[4] = BOUNTY_TYPE_KILL_UNIQUE;
        }




        int32 nNumMinBountiesInData = 0;
        for (int32 i = 0; i < NUM_NORMAL_BOUNTY_TYPES; i++)
        {
            nNumMinBountiesInData += arrBountyCounts[i];
        }
        // Inference: nNumDeltaBountiesInData is described as constant 0 throughout and has no other use.
        int32 nNumDeltaBountiesInData = 0;















        int32 nAdditionalBounties = Math::Max(nTotalBounties - nNumMinBountiesInData, 0);

        blz::vector<int> listBountySelection;
        for (int32 i = 0; i < NUM_NORMAL_BOUNTY_TYPES; i++)
        {
            while (arrBountyDeltas[i] > 0)
            {
                listBountySelection.push_back((BountyType)i);
                arrBountyDeltas[i]--;
            }
        }

        blz::random_shuffle(listBountySelection.begin(), listBountySelection.end());

        for (int32 i = 0; i < nAdditionalBounties && !listBountySelection.empty(); i++)
        {





            BountyType eAdditionalBounty = (BountyType)listBountySelection.back();
            arrBountyCounts[eAdditionalBounty]++;
            // Inference: pop_back has no line row; the post-decrement walk of back() at +0x750 needs it.
            listBountySelection.pop_back();
        }

        for (int32 i = 0; i < NUM_NORMAL_BOUNTY_TYPES; i++)
        {

            int32 nOrderedIndex = arrBountyPickOrder[i];

            while (arrBountyCounts[nOrderedIndex] > 0)
            {
                arrBountyCounts[nOrderedIndex]--;

                BountyType eBountyType = (BountyType)nOrderedIndex;

                PooledSNONameList listEligibleQuestSNOs(GetSnoNameListPool());

                POS posSNOs = listSNOFiles.GetHeadPosition();
                while (posSNOs)
                {
                    SNOName & snonameFile = listSNOFiles.GetNext(posSNOs);
                    ScopedQuestDefinitionLockR pdefQuest( snonameFile.snoHandle );
                    if (pdefQuest->eQuestType != QUEST_TYPE_BOUNTY ||
                        pdefQuest->tBountyData.eAct != eAct)
                    {
                        continue;
                    }
























                    if (pdefQuest->tBountyData.eBountyType == eBountyType)
                    {
                        listEligibleQuestSNOs.AddTail_Tracked(snonameFile);
                    }
                }

                if (listEligibleQuestSNOs.IsEmpty())
                {
                    continue;
                }





                int32 nRandomIndex = Rand_Internal(SGameGlobalsGet().rng, listEligibleQuestSNOs.Count() - 1);
                POS posQuestSNOs = listEligibleQuestSNOs.GetHeadPosition();
                while (nRandomIndex)
                {
                    listEligibleQuestSNOs.GetNext(posQuestSNOs);
                    nRandomIndex--;
                }

                SNOName & snonameChosenQuest = listEligibleQuestSNOs.GetAt(posQuestSNOs);
                ScopedQuestDefinitionLockR pdefQuest(snonameChosenQuest.snoHandle);

                QuestsLoad(snonameChosenQuest.snoHandle, pdefQuest->tBountyData.snoLevelArea);
                SQuestsSendUpdateToAllPlayers(snonameChosenQuest.snoHandle);

                sQuestsRemoveConflictingBounties(listSNOFiles, pdefQuest);
            }
        }
    }
}




void QuestsInitializeForGame()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    tGCD.ptQuestGlobals = (QuestGlobals *)SigmaMemoryNew(sizeof(QuestGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);
    QuestGlobals * ptQuestGlobals = tGCD.ptQuestGlobals;












    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();

    ptQuestGlobals->tQuestInstancePool.Initialize();
    ptQuestGlobals->tQuestInstancePool.SetMemoryInterface(ptMemoryInterface);

    ptQuestGlobals->tQuestInstanceList.SetPool(&ptQuestGlobals->tQuestInstancePool);
    ptQuestGlobals->tQuestPerPlayerInstanceList.SetPool(&ptQuestGlobals->tQuestInstancePool);




    ptQuestGlobals->bGameOver = FALSE;
    ptQuestGlobals->eCurrentAct = ACT_INVALID;


    SigmaMemoryClear(ptQuestGlobals->tDynamicQuestObjectives, sizeof(ptQuestGlobals->tDynamicQuestObjectives));
    for (int32 i = 0; i < 10; i++)
    {
        ptQuestGlobals->tDynamicQuestObjectives[i].eObjectiveType = OBJECTIVE_INVALID;
    }
    for (int32 i = 0; i < 10; i++)
    {
        ptQuestGlobals->arDynamicQuestObjectiveState[i] = 0;
        ptQuestGlobals->arDynamicQuestObjectiveCompleted[i] = FALSE;
        ptQuestGlobals->arDynamicQuestObjectiveHidden[i] = FALSE;
    }

    QuestsActivateForAct(GameGetCurrentAct());

    ptQuestGlobals->listOpenWorldTrackedSNOs.Initialize(MemoryPoolsGetDATAIDListPool());


    if (!GameActiveGameCommonDataIsClient())
    {
        UpdateFunctionInitialize(UPDATE_QUESTS, -1, ptQuestGlobals->tQuestsUpdateData);
        ServerQueueUpdateFunction(120, ptQuestGlobals->tQuestsUpdateData);

        UpdateFunctionInitialize(UPDATE_BOUNTY_SCENE_ACTIVATION, -1, ptQuestGlobals->tBountySceneActivationUpdate);


        ptQuestGlobals->bForcePlayerLocationTriggerEventAfterActivationUpdate = FALSE;
        ptQuestGlobals->bStartup = FALSE;
        ptQuestGlobals->bResendEnterEvents = FALSE;
        memset(ptQuestGlobals->arDynamicQuestObjectiveDirty, 0, sizeof(ptQuestGlobals->arDynamicQuestObjectiveDirty));
        ptQuestGlobals->snoCurrentMainLineQuest = SNO_NONE;




        ptQuestGlobals->nCurrentMainLineQuestStepUID = QUEST_UNASSIGNED_STEP_UID;
        ptQuestGlobals->nCurrentMainLineQuestHandicapLevel = GameGetHandicapLevel();

        if (GameIsOpenWorld())
        {





            sQuestsPickBounties();
        }

        ptQuestGlobals->tBountyActivationRecordList.Initialize(ptMemoryInterface);
        ptQuestGlobals->dwUpdatesSinceLastDeactivation = 0;
    }
}







void QuestsDisposeForGame()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;



    if (!GameActiveGameCommonDataIsClient())
    {
        ServerRemoveUpdateFunction(ptQuestGlobals->tQuestsUpdateData);
        ServerRemoveUpdateFunction(ptQuestGlobals->tBountySceneActivationUpdate);

        ptQuestGlobals->tBountyActivationRecordList.RemoveAll();
        ptQuestGlobals->tBountyActivationRecordList.~BountyActivationRecordList();
    }


    ptQuestGlobals->tQuestInstanceList.Dispose();
    ptQuestGlobals->tQuestPerPlayerInstanceList.Dispose();
    ptQuestGlobals->tQuestInstancePool.Dispose();

    ptQuestGlobals->listOpenWorldTrackedSNOs.Dispose();

    // Inference: the line-591 cleanup expands to a free followed by clearing the owning pointer.
    SigmaMemoryFree(ptQuestGlobals, AppGetCurrentMemoryInterface()); ptQuestGlobals = NULL;
}



void QuestsForceSystemUpdate()
{

    if (!GameActiveGameCommonDataIsClient())
    {
        GameCommonData & tGCD = GameGetActiveGameCommonData();
        QuestGlobals * ptQuestGlobals = tGCD.ptQuestGlobals;

        ServerQueueUpdateFunction(0, ptQuestGlobals->tQuestsUpdateData);
    }
}





void QuestsActivateForAct(Act eAct)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;


    if (ptQuestGlobals->eCurrentAct == eAct)
        return;



    ptQuestGlobals->eCurrentAct = eAct;
    ptQuestGlobals->tQuestInstanceList.RemoveAll();

    SNO snoAct = ActSNOFromActEnum(eAct);
    ScopedActDefinitionLockR pdefAct(snoAct);


    int32 nNumActQuests = ActGetNumQuests(pdefAct);
    int nCount = 0;




















    for (int32 i = 0; i < nNumActQuests; i++)
    {
        ActQuestInfo & tInfo = pdefAct->arQuests[i];









        ScopedQuestDefinitionLockR pdefQuest(tInfo.snoQuest);
        if (pdefQuest->eQuestType == QUEST_TYPE_MAIN_QUEST)
        {



            QuestInstance tQuestInstance;
            QuestsQuestInstanceInitialize(tQuestInstance, tInfo.snoQuest, SNO_NONE);
            ptQuestGlobals->tQuestInstanceList.AddTail_Tracked(tQuestInstance);
        }
    }

    ptQuestGlobals->nNumMainQuests = nCount;


    for (int32 i = 0; i < nNumActQuests; i++)
    {
        ActQuestInfo & tInfo = pdefAct->arQuests[i];

        ScopedQuestDefinitionLockR pdefQuest(tInfo.snoQuest);
        if (pdefQuest->eQuestType == QUEST_TYPE_SIDE_QUEST)
        {




            QuestInstance tQuestInstance;
            QuestsQuestInstanceInitialize(tQuestInstance, tInfo.snoQuest, SNO_NONE);
            ptQuestGlobals->tQuestInstanceList.AddTail_Tracked(tQuestInstance);
        }
    }
    ptQuestGlobals->nNumSideQuests = nCount - ptQuestGlobals->nNumMainQuests;
}



void QuestsLoad(SNO snoQuest, SNO snoLevelArea)
{



    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (ptQuestInstance)
        return;



    GameCommonData & tGCD = GameGetActiveGameCommonData();
    QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;

    QuestInstance tQuestInstance;
    QuestsQuestInstanceInitialize(tQuestInstance, snoQuest, snoLevelArea);
    ptQuestGlobals->tQuestInstanceList.AddTail_Tracked(tQuestInstance);
}



void QuestsQuestInstanceInitialize(QuestInstance & tQuestInstance, SNO snoQuest, SNO snoLevelArea)
{




    if (!GameActiveGameCommonDataIsClient())
    {
        if (GameIsOpenWorld() &&
            QuestGetQuestType(snoQuest) == QUEST_TYPE_MAIN_QUEST &&
            GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC) != snoQuest)
        {

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }


    tQuestInstance.snoQuest = snoQuest;
    tQuestInstance.dwStarted = GameGetServerTime();
    tQuestInstance.dwFlags = 0;
    tQuestInstance.nCurrentStepUID = QUEST_UNASSIGNED_STEP_UID;
    tQuestInstance.nResumableStepUID = QUEST_UNASSIGNED_STEP_UID;
    tQuestInstance.eInstanceState = QUEST_INSTANCE_UNASSIGNED;
    tQuestInstance.nPlayerIndex = -1;
    tQuestInstance.snoLevelArea = snoLevelArea;
    tQuestInstance.idACDMeter = ACDID_INVALID;
    tQuestInstance.flMeter = -1.0f;

    for (int32 i = 0; i < 20; i++)
    {
        tQuestInstance.arObjectiveState[i] = 0;
        tQuestInstance.arObjectiveCompleted[i] = FALSE;
        tQuestInstance.arFailureConditionState[i] = 0;
        tQuestInstance.arBonusRewards[i] = 0;
    }
}













void QuestsAdvanceToStep(QuestInstance & tQuestInstance, int32 nStepUID, BOOL bRollback)
{


    if (tQuestInstance.nCurrentStepUID == nStepUID)
        return;




    if (GameActiveGameCommonDataIsServer())
    {
        if (!bRollback)
        {
            if (!QuestStepIsReachableByStep(tQuestInstance.snoQuest, tQuestInstance.nCurrentStepUID, nStepUID))
                return;








            if ((nStepUID == QUEST_UNASSIGNED_STEP_UID || nStepUID == QUEST_NULL_STEP_UID) && tQuestInstance.eInstanceState != QUEST_INSTANCE_UNASSIGNED)
                return;
        }







        if (QuestIsGameOver())
        {
            tQuestInstance.nResumableStepUID = QUEST_UNASSIGNED_STEP_UID;
            tQuestInstance.eInstanceState = QUEST_INSTANCE_UNASSIGNED;
            tQuestInstance.nCurrentStepUID = QUEST_UNASSIGNED_STEP_UID;
            return;
        }
    }












    tQuestInstance.nCurrentStepUID = nStepUID;
    tQuestInstance.dwStarted = GameGetServerTime();

    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
    {
        tQuestInstance.eInstanceState = QUEST_INSTANCE_UNASSIGNED;
    }
    else if (QuestStepIsCompletionStep(tQuestInstance.snoQuest, nStepUID))
    {
        tQuestInstance.eInstanceState = QUEST_INSTANCE_COMPLETED;
    }
    else
    {
        if (GameActiveGameCommonDataIsServer() &&
            tQuestInstance.eInstanceState != QUEST_INSTANCE_ACTIVE)
        {
            SQuestsOnQuestActivated(tQuestInstance);
        }
        tQuestInstance.eInstanceState = QUEST_INSTANCE_ACTIVE;
    }




    if (tQuestInstance.eInstanceState != QUEST_INSTANCE_ACTIVE || QuestStepIsResumable(tQuestInstance.snoQuest, nStepUID))
    {
        tQuestInstance.nResumableStepUID = nStepUID;
    }




    for (int32 i = 0; i < 20; i++)
    {
        tQuestInstance.arObjectiveState[i] = 0;
        tQuestInstance.arObjectiveCompleted[i] = FALSE;
        tQuestInstance.arFailureConditionState[i] = 0;
    }
}







QuestInstance * QuestsGetQuestInstance(
    SNO snoQuest,
    ACDID idACDPlayer)
{
    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();


    if (idACDPlayer != ACDID_INVALID)
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer)
        {
            POS pos = ptQuestGlobals->tQuestPerPlayerInstanceList.GetHeadPosition();
            while (pos)
            {
                QuestInstance & tQuestInstance = ptQuestGlobals->tQuestPerPlayerInstanceList.GetNext(pos);
                if (tQuestInstance.snoQuest == snoQuest && tQuestInstance.nPlayerIndex == ptPlayer->nPlayerIndex)
                    return &tQuestInstance;
            }
        }
    }





    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (tQuestInstance.snoQuest == snoQuest)
            return &tQuestInstance;
    }



    return NULL;
}





















CRefString QuestsGetUIName(SNO snoQuest)
{

    if (snoQuest != SNO_NONE)
    {
        SNO snoStringList = QuestGetStringList(snoQuest);

        return StringListFetch(snoStringList, gszQuestNameLabel);
    }

    return CRefString("Invalid Quest SNO passed to QuestsGetUIName");
}



void QuestsIterate(QuestIterateCallback pfnCallback, void * pUserData)
{


    QuestGlobals * ptQuestGlobals = GameGetActiveGameCommonData().ptQuestGlobals;
    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuest = ptQuestGlobals->tQuestInstanceList.GetNext(pos);


        if (tQuest.snoQuest == SNO_NONE)
            continue;

        if (pfnCallback)

            pfnCallback(tQuest, pUserData);
    }
}






















void PlayerQuestDataInit(PlayerQuestData & tPlayerQuestData)
{

    tPlayerQuestData.idSSceneLast = (SSceneID)-1;
}



BOOL QuestIsComplete(const QuestInstance & tQuest)
{

    return tQuest.eInstanceState == QUEST_INSTANCE_COMPLETED;
}



BOOL QuestDungeonFinderIsComplete()

{

    SNO snoQuest = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (!ptQuestInstance ||
        QuestIsComplete(*ptQuestInstance))
    {



        // return line 1016 per line table; the FALSE return has no row of its own (merged into the inlined QuestIsComplete compare at 998)
        return TRUE;
    }
    return FALSE;
}
QuestInstance * GetLeadingMainLineQuest()
{

    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
    if (!ptQuestGlobals)
        return NULL;


    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuest = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestGetQuestType(tQuest.snoQuest) == QUEST_TYPE_MAIN_QUEST && tQuest.eInstanceState != QUEST_INSTANCE_COMPLETED)
            return &tQuest;
    }





    return NULL;
}




CRefString QuestStepGetDescription(SNO snoQuest, int32 nStepUID, PlayerClass ePlayerClass)
{


    ScopedQuestDefinitionLockR pdefQuest(snoQuest);


    CRefString sHeaderLabel = QuestGetStepHeaderLabel(pdefQuest, nStepUID);
    CRefString sDescLabel = QuestGetStepDescriptionLabel(pdefQuest, nStepUID, ePlayerClass);

    SNO snoStringList = QuestGetStringList(snoQuest);


    sHeaderLabel = SNOToString(STRINGLIST_SNO, snoStringList, FALSE) + ":" + sHeaderLabel;
    sDescLabel = SNOToString(STRINGLIST_SNO, snoStringList, FALSE) + ":" + sDescLabel;


    BOOL bHeaderSuccess = FALSE;
    CRefString sHeaderText = StringListFetch(sHeaderLabel, bHeaderSuccess);

    BOOL bDescSuccess = FALSE;
    CRefString sDescText = StringListFetch(sDescLabel, bDescSuccess);
    CRefString sDescAfterLanguageRules;
    if (bDescSuccess)
    {
        sDescAfterLanguageRules = LanguageRules::ApplyLanguageRules(sDescText.str());

        sDescAfterLanguageRules.Format("{c_gold}%s{/c_gold}", sDescAfterLanguageRules.str());
    }

    if (bHeaderSuccess && bDescSuccess)
    {

        return sHeaderText + "\n\n" + sDescAfterLanguageRules;
    }
    else if (bHeaderSuccess)
    {
        return sHeaderText;
    }
    else if (bDescSuccess)
    {
        return sDescAfterLanguageRules;
    }


    return "";
}




BOOL QuestStepGetSplashText(SNO snoQuest, int32 nStepUID, CRefString & strOut)
{

    CRefString sLabel;
    {
        ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
        sLabel = QuestGetStepSplashLabel(ptQuestDef, nStepUID);
    }


    SNO snoStringList = QuestGetStringList(snoQuest);
    BOOL bSuccess = StringListFetch(snoStringList, sLabel, strOut);

    return bSuccess;
}




BOOL QuestStepGetMeterText(SNO snoQuest, int32 nStepUID, CRefString & strOut)
{

    CRefString sLabel;
    {
        ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
        sLabel = QuestGetStepMeterLabel(ptQuestDef, nStepUID);
    }


    SNO snoStringList = QuestGetStringList(snoQuest);
    BOOL bSuccess = StringListFetch(snoStringList, sLabel, strOut);

    return bSuccess;
}




BOOL QuestStepIsResumable(SNO snoQuest, int32 nStepUID)
{


    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
        return TRUE;

    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);


    if (ptQuestDef->eQuestType >= QUEST_TYPE_EVENT && ptQuestDef->eQuestType <= QUEST_TYPE_CHALLENGE)
        return FALSE;

    if (QuestIsAnyBountyType(ptQuestDef))
        return FALSE;



    for (int32 i = 0; i < ptQuestDef->nNumQuestSteps; i++)
    {
        if (ptQuestDef->arQuestSteps[i].nUID == nStepUID)
        {
            return ((ptQuestDef->arQuestSteps[i].dwFlags >> QSF_RESUMABLE_STEP) | (ptQuestDef->arQuestSteps[i].dwFlags >> QSF_SELECTABLE_STEP)) & 1;
        }
    }



    return FALSE;
}


BOOL QuestStepIsSelectable(SNO snoQuest, int32 nStepUID)
{

    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);

    if (ptQuestDef->eQuestType != QUEST_TYPE_MAIN_QUEST)
        return FALSE;

    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
        return (ptQuestDef->tQuestUnassignedStep.dwFlags & (1 << QUSF_SELECTABLE_STEP)) != 0;

    for (int32 i = 0; i < ptQuestDef->nNumQuestSteps; i++)
    {
        if (ptQuestDef->arQuestSteps[i].nUID == nStepUID)
        {
            return (ptQuestDef->arQuestSteps[i].dwFlags & (1 << QSF_SELECTABLE_STEP)) != 0;
        }
    }






    return FALSE;
}


BOOL QuestStepSavePartialCompletion(SNO snoQuest, int32 nStepUID)
{


    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
        return FALSE;

    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);


    if (ptQuestDef->eQuestType >= QUEST_TYPE_EVENT && ptQuestDef->eQuestType <= QUEST_TYPE_CHALLENGE)
        return FALSE;

    if (QuestIsAnyBountyType(ptQuestDef))
        return FALSE;



    for (int32 i = 0; i < ptQuestDef->nNumQuestSteps; i++)
    {
        if (ptQuestDef->arQuestSteps[i].nUID == nStepUID)
        {
            return (ptQuestDef->arQuestSteps[i].dwFlags & (1 << QSF_SAVE_PARTIAL_COMPLETION)) != 0;
        }
    }


    return FALSE;
}


BOOL QuestStepTrackPerPlayer(SNO snoQuest, int32 nStepUID)
{


    if (nStepUID == QUEST_NULL_STEP_UID)
        return FALSE;



    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    switch (pdefQuest->eQuestType)
    {
    case QUEST_TYPE_EVENT:
    case QUEST_TYPE_PAGE_OF_FATE:
    case QUEST_TYPE_CHALLENGE:
    case QUEST_TYPE_BOUNTY:
    case QUEST_TYPE_SET_DUNGEON:
    case QUEST_TYPE_SET_DUNGEON_BONUS:
    case QUEST_TYPE_SET_DUNGEON_MASTERY:
    case QUEST_TYPE_SET_DUNGEON_TRACKER:
        return FALSE;
    case QUEST_TYPE_HORADRIC_QUEST:




        if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
            return FALSE;
        for (int32 i = 0; i < pdefQuest->nNumQuestSteps; i++)
        {
            if (pdefQuest->arQuestSteps[i].nUID == nStepUID)
            {
                int32 nNumBonusObjectiveSets = pdefQuest->arQuestSteps[i].serStepBonusObjectiveSets.ElementCount(sizeof(QuestStepBonusObjectiveSet));
                for (int32 s = 0; s < nNumBonusObjectiveSets; s++)
                {
                    QuestStepBonusObjectiveSet & tBonusObjectiveSet = pdefQuest->arQuestSteps[i].arStepBonusObjectiveSets[s];
                    int32 nNumBonusObjectives = tBonusObjectiveSet.serStepBonusObjectives.ElementCount(sizeof(QuestStepObjective));
                    for (int32 k = 0; k < nNumBonusObjectives; k++)
                    {
                        const QuestStepObjective & tObjective = tBonusObjectiveSet.arStepBonusObjectives[k];
                        if (tObjective.eObjectiveType == OBJECTIVE_HQ_ACT_REAGENTS_COLLECTED)
                            return TRUE;
                    }
                }

                return FALSE;
            }
        }

        return FALSE;
    case QUEST_TYPE_SIDE_QUEST:
        return TRUE;
    default:
        break;
    }
    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
        return FALSE;


    for (int32 i = 0; i < pdefQuest->nNumQuestSteps; i++)
    {
        if (pdefQuest->arQuestSteps[i].nUID == nStepUID)
        {
            return (pdefQuest->arQuestSteps[i].dwFlags & (1 << QSF_TRACK_PER_PLAYER)) != 0;
        }
    }


    return FALSE;
}




CRefString QuestStepGetName(SNO snoQuest, int32 nStepUID)
{

    CRefString sName;
    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);

    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
    {
        sName = "Unassigned";
    }
    else
    {
        for (int32 i = 0; i < ptQuestDef->nNumQuestSteps; i++)
        {
            if (ptQuestDef->arQuestSteps[i].nUID == nStepUID)
                sName = ptQuestDef->arQuestSteps[i].szStepName_VariableArray;
        }


        for (int32 i = 0; i < ptQuestDef->nNumQuestCompletionSteps; i++)
        {
            if (ptQuestDef->arQuestCompletionSteps[i].nUID == nStepUID)
                sName = ptQuestDef->arQuestCompletionSteps[i].szStepName_VariableArray;
        }
    }




    return sName;
}




int32 QuestStepGetUIDByName(SNO snoQuest, LPCSTR szStepName_VariableArray)
{

    int32 nStepUID = QUEST_UNASSIGNED_STEP_UID;
    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    for (int32 i = 0; i < ptQuestDef->nNumQuestSteps; i++)
    {
        if (strcmp(ptQuestDef->arQuestSteps[i].szStepName_VariableArray, szStepName_VariableArray) == 0)
        {
            nStepUID = ptQuestDef->arQuestSteps[i].nUID;
            break;
        }
    }


    return nStepUID;
}





















int32 QuestStepGetNumObjectives(const QuestStep & tQuestStep)
{

    int32 nNumObjectives = 0;
    int32 nNumObjectiveSets = tQuestStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
    for (int32 i = 0; i < nNumObjectiveSets; i++)
    {
        QuestStepObjectiveSet & tObjectiveSet = tQuestStep.arStepObjectiveSets[i];
        nNumObjectives += tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
    }



    return nNumObjectives;
}


int32 QuestStepGetNumBonusObjectives(const QuestStep & tQuestStep)
{

    int32 nNumBonusObjectives = 0;
    int32 nNumBonusObjectiveSets = tQuestStep.serStepBonusObjectiveSets.dwSize / sizeof(QuestStepBonusObjectiveSet);
    for (int32 i = 0; i < nNumBonusObjectiveSets; i++)
    {
        QuestStepBonusObjectiveSet & tBonusObjectiveSet = tQuestStep.arStepBonusObjectiveSets[i];
        nNumBonusObjectives += tBonusObjectiveSet.serStepBonusObjectives.dwSize / sizeof(QuestStepObjective);
    }



    return nNumBonusObjectives;
}


int32 QuestStepGetNumFailureConditions(const QuestStep & tQuestStep)
{

    int32 nNumConditions = 0;
    int32 nNumFailureConditionSets = tQuestStep.serStepFailureConditionSets.dwSize / sizeof(QuestStepFailureConditionSet);
    for (int32 i = 0; i < nNumFailureConditionSets; i++)
    {
        QuestStepFailureConditionSet & tFailureConditionSet = tQuestStep.arStepFailureConditionSets[i];
        nNumConditions += tFailureConditionSet.serStepFailureConditions.dwSize / sizeof(QuestStepFailureCondition);
    }



    return nNumConditions;
}


int32 QuestGetNumObjectivesToSave(SNO snoQuest, int32 nStepUID)
{

    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    int32 nStep = QuestStepUIDToIndex(ptQuestDef, nStepUID);

    if (nStep >= 0)
    {
        const QuestStep & tQuestStep = ptQuestDef->arQuestSteps[nStep];

        return QuestStepGetNumObjectives(tQuestStep) + QuestStepGetNumBonusObjectives(tQuestStep);
    }

    return 0;
}



int32 QuestGetNumFailureConditionsToSave(SNO snoQuest, int32 nStepUID)
{

    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    int32 nStep = QuestStepUIDToIndex(ptQuestDef, nStepUID);

    if (nStep >= 0)
    {
        const QuestStep & tQuestStep = ptQuestDef->arQuestSteps[nStep];

        return QuestStepGetNumFailureConditions(tQuestStep);
    }

    return 0;
}
// Case order and grouping inferred (no case rows): COMPLETED first, then UNASSIGNED/FAILED sharing a return gives the original's 4-entry jump table.


static BOOL sQuestHasStepBeenProgressed(
    const QuestInstance & tQuest,
    int32 nStepUID,
    BOOL bPassedOnly)
{
    switch (tQuest.eInstanceState)
    {
    case QUEST_INSTANCE_COMPLETED:
        return TRUE;
    case QUEST_INSTANCE_UNASSIGNED:
    case QUEST_INSTANCE_FAILED:
        return FALSE;
    case QUEST_INSTANCE_ACTIVE:









        if (bPassedOnly && tQuest.nCurrentStepUID == nStepUID)
            return FALSE;






        return QuestStepIsReachableByStep(tQuest.snoQuest, nStepUID, tQuest.nCurrentStepUID);
    }


    return FALSE;
}




BOOL QuestHasStepBeenReached(const QuestInstance & tQuest, int32 nStepUID)
{
    return sQuestHasStepBeenProgressed(tQuest, nStepUID, FALSE);
}





BOOL QuestHasStepBeenPassed(const QuestInstance & tQuest, int32 nStepUID)
{
    return sQuestHasStepBeenProgressed(tQuest, nStepUID, TRUE);
}















void QuestsVersionForPlayer(HeroStateData & tStateData)
{











































}
















































BOOL QuestStepIsReachableByStep(SNO snoQuest, int32 nStepUIDSource, int32 nStepUIDDest)
{



    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    return QuestStepIsReachableByStep(ptQuestDef, nStepUIDSource, nStepUIDDest);
}


// NON_MATCHING: 352 B both; the unassigned-step block is placed after the loop (original: before it), and the 1669 test duplicated after the 1662 call checks bSource before nStepUIDDest. Tried swapping 1669 operands (no change).
BOOL QuestStepIsReachableByStep(
    const QuestDefinition * ptQuestDef,
    int32 nStepUIDSource,
    int32 nStepUIDDest)
{


    if (nStepUIDSource == QUEST_NULL_STEP_UID || nStepUIDDest == QUEST_NULL_STEP_UID)
        return FALSE;





    if (nStepUIDDest == nStepUIDSource)
        return TRUE;



    BOOL bSourceIsCompletionStep = QuestStepIsCompletionStep(ptQuestDef, nStepUIDSource);
    BOOL bDestIsCompletionStep = QuestStepIsCompletionStep(ptQuestDef, nStepUIDDest);


    if (bSourceIsCompletionStep && bDestIsCompletionStep)
        return TRUE;




    if (!bDestIsCompletionStep && QuestStepUIDToIndex(ptQuestDef, nStepUIDDest) == (uint32)-1)
        return FALSE;





    if (nStepUIDDest == QUEST_UNASSIGNED_STEP_UID || bSourceIsCompletionStep)
        return FALSE;










    if (nStepUIDDest == QUEST_ANY_COMPLETION_STEP_UID)
        return TRUE;



    int32 nNumObjectiveSets;
    XPtr64<QuestStepObjectiveSet> arStepObjectiveSets;

    if (nStepUIDSource == QUEST_UNASSIGNED_STEP_UID)
    {
        nNumObjectiveSets = ptQuestDef->tQuestUnassignedStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
        arStepObjectiveSets = ptQuestDef->tQuestUnassignedStep.arStepObjectiveSets;
    }
    else
    {
        int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, nStepUIDSource);
        if (nStepIndex >= 0)  // no else: the original falls through with nNumObjectiveSets unset (signed loop compare b.lt, tbnz to the loop guard's FALSE return)
        {
            QuestStep * ptQuestStep = &ptQuestDef->arQuestSteps[nStepIndex];
            nNumObjectiveSets = ptQuestStep->serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
            arStepObjectiveSets = ptQuestStep->arStepObjectiveSets;
        }
    }

    for (int32 j = 0; j < nNumObjectiveSets; j++)
    {
        QuestStepObjectiveSet & tObjectiveSet = arStepObjectiveSets[j];


        if (tObjectiveSet.nNextStepUID == QUEST_NULL_STEP_UID)
            continue;



        BOOL bReachable = QuestStepIsReachableByStep(ptQuestDef, tObjectiveSet.nNextStepUID, nStepUIDDest);
        if (bReachable)
            return TRUE;
    }


    return FALSE;

}



BOOL QuestStepCanReachReward(SNO snoQuest, int32 nStepUID)
{


    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    return QuestStepCanReachReward(ptQuestDef, nStepUID);
}



BOOL QuestStepCanReachReward(
    const QuestDefinition * ptQuestDef,
    int32 nStepUID)
{


    if (nStepUID == QUEST_NULL_STEP_UID)
        return FALSE;





    if (QuestStepIsCompletionStep(ptQuestDef, nStepUID))
        return FALSE;



    int32 nNumObjectiveSets;
    XPtr64<QuestStepObjectiveSet> arStepObjectiveSets;

    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
    {
        nNumObjectiveSets = ptQuestDef->tQuestUnassignedStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
        arStepObjectiveSets = ptQuestDef->tQuestUnassignedStep.arStepObjectiveSets;
    }
    else
    {
        int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, nStepUID);
        if (nStepIndex >= 0)  // no else: the original falls through with nNumObjectiveSets unset (signed loop compare b.lt, tbnz to the shared FALSE return)
        {
            QuestStep * ptQuestStep = &ptQuestDef->arQuestSteps[nStepIndex];
            nNumObjectiveSets = ptQuestStep->serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
            arStepObjectiveSets = ptQuestStep->arStepObjectiveSets;





            if (ptQuestStep->nRewardXPTier > 0 || ptQuestStep->nReplayRewardXPTier > 0 ||
                ptQuestStep->nRewardGoldTier > 0 || ptQuestStep->nReplayRewardGoldTier > 0)
                return TRUE;
        }
    }



    for (int32 j = 0; j < nNumObjectiveSets; j++)
    {
        QuestStepObjectiveSet & tObjectiveSet = arStepObjectiveSets[j];


        if (tObjectiveSet.nNextStepUID == QUEST_NULL_STEP_UID)
            continue;



        BOOL bCanReachReward = QuestStepCanReachReward(ptQuestDef, tObjectiveSet.nNextStepUID);
        if (bCanReachReward)
            return TRUE;
    }


    return FALSE;

}


BOOL sTestUnlockingPrimaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest)
{


    int32 nObjectiveIndex = 0;
    int32 nNumObjectiveSets = tStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));

    for (int32 j = 0; j < nNumObjectiveSets; j++)
    {
        const QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[j];
        int32 nNumObjectives = tObjectiveSet.serStepObjectives.ElementCount(sizeof(QuestStepObjective));

        for (int32 k = 0; k < nNumObjectives; k++)
        {
            const QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];

            if (!(tObjective.dwFlags & 2))
                continue;



            if (QuestObjectivePassed(tObjective, tQuest.arObjectiveState[nObjectiveIndex + k]))
                continue;



            if (tObjective.eObjectiveType == OBJECTIVE_INTERACT_WITH_GIZMO)
            {
                SNO snoGizmo = tObjective.snoname1.snoHandle;
                ScopedActorDefinitionLockR pdefActor(snoGizmo);
                GBID gbidItemRequired = TagMapGetGBID(pdefActor->hTagMap, TAG_ACTOR_GIZMO_ITEM_REQUIRED_TO_OPERATE);

                if (gbidItemRequired == tACD.hGB.gbid)
                    return TRUE;
            }
        }



        nObjectiveIndex += nNumObjectives;
    }


    return FALSE;
}


BOOL sTestPrimaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest)
{


    int32 nObjectiveIndex = 0;
    int32 nNumObjectiveSets = tStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));

    for (int32 j = 0; j < nNumObjectiveSets; j++)
    {
        const QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[j];
        int32 nNumObjectives = tObjectiveSet.serStepObjectives.ElementCount(sizeof(QuestStepObjective));

        for (int32 k = 0; k < nNumObjectives; k++)
        {
            const QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];

            if (!(tObjective.dwFlags & 2))
                continue;



            if (QuestObjectivePassed(tObjective, tQuest.arObjectiveState[nObjectiveIndex + k]))
                continue;



            switch (tObjective.eObjectiveType)
            {
            case OBJECTIVE_KILL_GROUP:
                {
                    uint32 uHash = ActorGroupNameToHash(tObjective.szParam1_VariableArray);
                    if (uHash == tACD.uGroupPrimary || uHash == tACD.uGroupSecondary)
                        return TRUE;
                }
                break;
            case OBJECTIVE_KILL_MONSTER:
                if (tObjective.snoname1.snoHandle == tACD.tSNOName.snoHandle)
                    return TRUE;
                break;
            }
            switch (tObjective.eObjectiveType)
            {
            case OBJECTIVE_KILL_MONSTER_FROM_GROUP:
                if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID)) == -1)
                {
                    uint32 uHash = ActorGroupNameToHash(tObjective.szParam1_VariableArray);
                    if (uHash == tACD.uGroupPrimary || uHash == tACD.uGroupSecondary)
                        return TRUE;
                }
                break;
            case OBJECTIVE_KILL_MONSTER_FROM_FAMILY:
                if (tObjective.nParam1 != 0 && tACD.IsMonster() && MonsterGetMonsterFamily(tACD.tSNOName) == tObjective.nParam1)
                    return TRUE;
                break;
            case OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA:
                if (tACD.eMonsterRarity == MR_CHAMPION || tACD.eMonsterRarity == MR_RARE)
                {
                    if (ACDGetLevelArea(tACD) == tObjective.snoname1.snoHandle)
                        return TRUE;
                }
                break;
            case OBJECTIVE_INTERACT_WITH_GIZMO:
                if (tObjective.snoname1.snoHandle == tACD.snoActor &&
                    tACD.IsOperatableGizmo(ACDID_INVALID, FALSE))
                    return TRUE;
                break;
            case OBJECTIVE_KILL_ALL_MONSTERS_IN_WORLD:
                if (tACD.GetSWorldID() != SWORLDID_INVALID &&
                    tObjective.snoname1.snoHandle == WorldCommonGetWorldSNO(tACD.GetSWorldID()) &&
                    tACD.IsMonsterTeam() &&
                    tACD.IsAlive() &&
                    tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_RESURRECTED)) == 0 &&
                    !ActorSNOIsUntargetable(tACD.GetActorSNO()))
                {
                    if (tQuest.arObjectiveState[nObjectiveIndex + k] > 0)
                        return TRUE;
                }
                break;
            default:
                break;
            }
        }
        nObjectiveIndex += nNumObjectives;
    }
    return FALSE;
}
BOOL sTestUnlockingSecondaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest)
{



















    int32 nObjectiveIndex = 0;
    int32 nNumBaseObjectives = QuestStepGetNumObjectives(tStep);
    int32 nNumObjectiveSets = tStep.serStepBonusObjectiveSets.ElementCount(sizeof(QuestStepBonusObjectiveSet));

    for (int32 j = 0; j < nNumObjectiveSets; j++)
    {
        const QuestStepBonusObjectiveSet & tObjectiveSet = tStep.arStepBonusObjectiveSets[j];
        int32 nNumObjectives = tObjectiveSet.serStepBonusObjectives.ElementCount(sizeof(QuestStepObjective));

        for (int32 k = 0; k < nNumObjectives; k++)
        {
            const QuestStepObjective & tObjective = tObjectiveSet.arStepBonusObjectives[k];

            if (!(tObjective.dwFlags & 2))
                continue;




            if (QuestObjectivePassed(tObjective, tQuest.arObjectiveState[nNumBaseObjectives + nObjectiveIndex + k]))
                continue;



            if (tObjective.eObjectiveType == OBJECTIVE_INTERACT_WITH_GIZMO)
            {
                SNO snoGizmo = tObjective.snoname1.snoHandle;
                ScopedActorDefinitionLockR pdefActor(snoGizmo);
                GBID gbidItemRequired = TagMapGetGBID(pdefActor->hTagMap, TAG_ACTOR_GIZMO_ITEM_REQUIRED_TO_OPERATE);

                if (gbidItemRequired == tACD.hGB.gbid)
                    return TRUE;
            }
        }



        nObjectiveIndex += nNumObjectives;
    }


    return FALSE;
}


BOOL sTestSecondaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest)
{

    int32 nObjectiveIndex = 0;
    int32 nNumBaseObjectives = QuestStepGetNumObjectives(tStep);
    int32 nNumObjectiveSets = tStep.serStepBonusObjectiveSets.ElementCount(sizeof(QuestStepBonusObjectiveSet));


    for (int32 j = 0; j < nNumObjectiveSets; j++)
    {
        const QuestStepBonusObjectiveSet & tObjectiveSet = tStep.arStepBonusObjectiveSets[j];
        int32 nNumObjectives = tObjectiveSet.serStepBonusObjectives.ElementCount(sizeof(QuestStepObjective));
        int32 nCheckPassed = !tObjectiveSet.bRepeatable || tObjectiveSet.bInverse;

        for (int32 k = 0; k < nNumObjectives; k++)
        {
            const QuestStepObjective & tObjective = tObjectiveSet.arStepBonusObjectives[k];

            if (!(tObjective.dwFlags & 2))
                continue;



            if (nCheckPassed && QuestObjectivePassed(tObjective, tQuest.arObjectiveState[nNumBaseObjectives + nObjectiveIndex + k]))
                continue;



            switch (tObjective.eObjectiveType)
            {
            case OBJECTIVE_KILL_GROUP:
                {
                    uint32 uHash = ActorGroupNameToHash(tObjective.szParam1_VariableArray);
                    if (uHash == tACD.uGroupPrimary || uHash == tACD.uGroupSecondary)
                        return TRUE;
                }
                break;
            case OBJECTIVE_KILL_MONSTER:
                if (tObjective.snoname1.snoHandle == tACD.tSNOName.snoHandle)
                    return TRUE;
                break;
            }
            switch (tObjective.eObjectiveType)
            {
            case OBJECTIVE_KILL_MONSTER_FROM_GROUP:
                if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID)) == -1)
                {
                    uint32 uHash = ActorGroupNameToHash(tObjective.szParam1_VariableArray);
                    if (uHash == tACD.uGroupPrimary || uHash == tACD.uGroupSecondary)
                        return TRUE;
                }
                break;
            case OBJECTIVE_KILL_MONSTER_FROM_FAMILY:
                if (tObjective.nParam1 != -1 && tACD.IsMonster() && MonsterGetMonsterFamily(tACD.tSNOName) == tObjective.nParam1)
                    return TRUE;
                break;
            case OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA:
                if (tACD.eMonsterRarity == MR_CHAMPION || tACD.eMonsterRarity == MR_RARE)
                {
                    if (ACDGetLevelArea(tACD) == tObjective.snoname1.snoHandle)
                        return TRUE;
                }
                break;
            case OBJECTIVE_INTERACT_WITH_GIZMO:
                if (tObjective.snoname1.snoHandle == tACD.tSNOName.snoHandle &&
                    tACD.IsOperatableGizmo(ACDID_INVALID, FALSE))
                    return TRUE;
                break;
            default:
                break;
            }
        }
        nObjectiveIndex += nNumObjectives;
    }
    return FALSE;
}
BOOL QuestIsACDObjective(const ActorCommonData & tACD)
{
    QuestGlobals * pGlobals = QuestsGetQuestGlobals();
    if (!pGlobals)
        return FALSE;


















    POS pos = pGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuest = pGlobals->tQuestInstanceList.GetNext(pos);

        if (tQuest.eInstanceState != QUEST_INSTANCE_ACTIVE)
            continue;




        ScopedQuestDefinitionLockR pdefQuest(tQuest.snoQuest);

        int32 nStepIndex = QuestStepUIDToIndex(pdefQuest,
            tQuest.nCurrentStepUID);


        if (nStepIndex < 0)
            continue;



        const QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];

        if (sTestPrimaryObjectives(tACD, tStep, tQuest) || sTestSecondaryObjectives(tACD, tStep, tQuest))
            return TRUE;
    }



    if (GameIsOpenWorld())
    {
        GameCommonData & tGCD = GameGetActiveGameCommonData();
        QuestGlobals * ptQuestGlobals = tGCD.ptQuestGlobals;


        for (int32 i = 0; i < 10; i++)
        {
            if (ptQuestGlobals->tDynamicQuestObjectives[i].eObjectiveType == OBJECTIVE_KILL_MONSTER && ptQuestGlobals->tDynamicQuestObjectives[i].snoname1.snoHandle == tACD.snoActor)
                return TRUE;
            if (ptQuestGlobals->tDynamicQuestObjectives[i].eObjectiveType == OBJECTIVE_INTERACT_WITH_GIZMO && ptQuestGlobals->tDynamicQuestObjectives[i].snoname1.snoHandle == tACD.snoActor)
                return TRUE;
        }
    }










    return FALSE;
}









































SNO QuestGetActiveBountyForLevelArea(SNO snoLevelArea, BOOL bCheckAdditionalLevelAreas)
{


    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
    POS pos;
    pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (QuestIsAnyBountyType(tQuestInstance.snoQuest))
        {
            ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
            if (pdefQuest->tBountyData.snoLevelArea == snoLevelArea ||
                (bCheckAdditionalLevelAreas && QuestHasAdditionalLevelArea(tQuestInstance.snoQuest, snoLevelArea)))
                return tQuestInstance.snoQuest;
        }
    }



    pos = ptQuestGlobals->tQuestPerPlayerInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestPerPlayerInstanceList.GetNext(pos);
        if (QuestIsAnyBountyType(tQuestInstance.snoQuest))
        {
            ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);
            if (pdefQuest->tBountyData.snoLevelArea == snoLevelArea ||
                (bCheckAdditionalLevelAreas && QuestHasAdditionalLevelArea(tQuestInstance.snoQuest, snoLevelArea)))
                return tQuestInstance.snoQuest;
        }
    }




    return SNO_NONE;
}

BOOL QuestsACDUnlocksObjective(const ActorCommonData & tACD)
{

    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
    if (!ptQuestGlobals)
        return FALSE;



    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);

        if (tQuestInstance.eInstanceState != QUEST_INSTANCE_ACTIVE)
            continue;




        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);

        int32 nStepIndex = QuestStepUIDToIndex(pdefQuest,
            tQuestInstance.nCurrentStepUID);


        if (nStepIndex < 0)
            continue;



        const QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];

        if (sTestUnlockingPrimaryObjectives(tACD, tStep, tQuestInstance) ||
            sTestUnlockingSecondaryObjectives(tACD, tStep, tQuestInstance))
            return TRUE;
    }



    return FALSE;
}


BOOL QuestIsAnyCurrentKillQuest()
{

    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
    if (!ptQuestGlobals)
        return FALSE;



    POS pos = ptQuestGlobals->tQuestInstanceList.GetHeadPosition();
    while (pos)
    {
        QuestInstance & tQuestInstance = ptQuestGlobals->tQuestInstanceList.GetNext(pos);
        if (tQuestInstance.eInstanceState != QUEST_INSTANCE_ACTIVE)
            continue;




        ScopedQuestDefinitionLockR pdefQuest(tQuestInstance.snoQuest);

        int32 nStepIndex = QuestStepUIDToIndex(pdefQuest,
            tQuestInstance.nCurrentStepUID);


        if (nStepIndex < 0)
            continue;



        const QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];


    int32 nNumObjectiveSets = tStep.serStepObjectiveSets.ElementCount(sizeof(QuestStepObjectiveSet));
    for (int32 j = 0; j < nNumObjectiveSets; j++)
    {
        const QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[j];
        int32 nNumObjectives = tObjectiveSet.serStepObjectives.ElementCount(sizeof(QuestStepObjective));

        for (int32 k = 0; k < nNumObjectives; k++)
        {
            const QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[k];

            switch (tObjective.eObjectiveType)
            {
            case OBJECTIVE_KILL_MONSTER:
            case OBJECTIVE_INTERACT_WITH_GIZMO:
            case OBJECTIVE_KILL_GROUP:
            case OBJECTIVE_KILL_MONSTER_FROM_GROUP:
            case OBJECTIVE_KILL_MONSTER_FROM_FAMILY:
            case OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA:
                return TRUE;
            default:
                break;
            }
        }
    }
    int32 nNumBonusObjectiveSets = tStep.serStepBonusObjectiveSets.ElementCount(sizeof(QuestStepBonusObjectiveSet));
    for (int32 j = 0; j < nNumBonusObjectiveSets; j++)
    {
        const QuestStepBonusObjectiveSet & tBonusObjectiveSet = tStep.arStepBonusObjectiveSets[j];
        int32 nNumBonusObjectives = tBonusObjectiveSet.serStepBonusObjectives.ElementCount(sizeof(QuestStepObjective));
        for (int32 k = 0; k < nNumBonusObjectives; k++)
        {
            const QuestStepObjective & tBonusObjective = tBonusObjectiveSet.arStepBonusObjectives[k];
            switch (tBonusObjective.eObjectiveType)
            {
            case OBJECTIVE_KILL_MONSTER:
            case OBJECTIVE_INTERACT_WITH_GIZMO:
            case OBJECTIVE_KILL_GROUP:
            case OBJECTIVE_KILL_MONSTER_FROM_GROUP:
            case OBJECTIVE_KILL_MONSTER_FROM_FAMILY:
            case OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA:
                return TRUE;
            default:
                break;
            }
        }
    }
    }
    return FALSE;
}
// NON_MATCHING: 28 vs 32 B; ours if-converts to cset/cset/and, the original branches to two returns (2378, 2384). Tried assign-only, nested if, return-in-block; 2373 col 27 (load) puts nParam2 first.
BOOL QuestObjectivePassed(
    const QuestStepObjective & tObjectiveToCheck,
    int32 nObjectiveCounter)
{
    BOOL bPassed = FALSE;

    if (tObjectiveToCheck.nParam2 <= nObjectiveCounter)
    {



        bPassed = (nObjectiveCounter != 0);
        return bPassed;
    }



    return bPassed;
}



BOOL QuestGetCurrentStepReward(const Player & tPlayer, SNO snoQuest, uint64 & nXPGranted, int32 & nGoldGranted, SNO & snoRecipe, SNO & snoTreasureClass)
{






    QuestInstance * ptQuestInst = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (!ptQuestInst)
        return FALSE;



    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    int32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, ptQuestInst->nCurrentStepUID);
    if (nStepIndex < 0)
        return FALSE;



    QuestStep & tQuestStep = ptQuestDef->arQuestSteps[nStepIndex];
    int32 nTreasureLevel;
    BOOL bReplayReward;
    BOOL bAddNVStack;
    return QuestStepGetReward(tPlayer, snoQuest, tQuestStep, nXPGranted, nGoldGranted, snoRecipe, snoTreasureClass, nTreasureLevel, bReplayReward, bAddNVStack);
}




BOOL QuestGetFirstStepReward(const Player & tPlayer, SNO snoQuest, uint64 & nXPGranted, int32 & nGoldGranted, SNO & snoRecipe, SNO & snoTreasureClass)
{






    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    if (ptQuestDef->nNumQuestSteps == 0)
        return FALSE;


    QuestStep & tQuestStep = ptQuestDef->arQuestSteps[0];
    int32 nTreasureLevel;
    BOOL bReplayReward;
    BOOL bAddNVStack;

    return QuestStepGetReward(tPlayer, snoQuest, tQuestStep, nXPGranted, nGoldGranted, snoRecipe, snoTreasureClass, nTreasureLevel, bReplayReward, bAddNVStack);
}




BOOL QuestStepGetReward(
    const Player & tPlayer,
    SNO snoQuest,
    const QuestStep & tQuestStep,
    uint64 & nXPGranted,
    int32 & nGoldGranted,
    SNO & snoRecipe,
    SNO & snoTreasureClass,
    int32 & nTreasureLevel,
    BOOL & bReplayReward,
    BOOL & bAddNVStack)
{
    if (tQuestStep.nRewardXPTier < 0 || tQuestStep.nRewardXPTier > MAX_QUEST_REWARD_XP_TIER ||
        tQuestStep.nRewardGoldTier < 0 || tQuestStep.nRewardGoldTier > MAX_QUEST_REWARD_GOLD_TIER)
    {
        // Inference: the unsigned range branches share the false-return tail.
        return FALSE;
    }


    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    // Inference: merged ACD lookup failure returns FALSE before reward writes.
    if (!ptACDPlayer)
        return FALSE;


    ScopedQuestDefinitionLockR pdefQuest(snoQuest);




    bReplayReward = PlayerHasReceivedQuestStepReward(tPlayer, snoQuest, tQuestStep.nUID);

    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;

    nTreasureLevel = GameGetMonsterLevel();

    int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    nTreasureLevel = Math::Min(nTreasureLevel, nPlayerLevel);
    nTreasureLevel = Math::Max(nTreasureLevel, 1);
    // Inference: recipe and default branches share SNO_NONE assignments in the original.
    if (!bReplayReward)
    {
        if (tQuestStep.eRewardType == QUEST_REWARD_SHARED_RECIPE)
        {
            snoRecipe = tQuestStep.snoRewardRecipe[PLAYER_CLASS_BARBARIAN];
            snoTreasureClass = SNO_NONE;
        }
        else if (tQuestStep.eRewardType == QUEST_REWARD_INDIVIDUAL_RECIPE)
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(tPlayer.idPlayerACD);

            snoRecipe = tQuestStep.snoRewardRecipe[ePlayerClass];
            snoTreasureClass = SNO_NONE;
        }
        else if (tQuestStep.eRewardType == QUEST_REWARD_TREASURE_CLASS)
        {
            snoRecipe = SNO_NONE;
            snoTreasureClass = tQuestStep.snoRewardTreasureClass;
        }
        else
        {
            snoRecipe = SNO_NONE;
            snoTreasureClass = SNO_NONE;
        }

        nXPGranted = (tQuestStep.nRewardXPTier > 0) ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nQuestTierXP[tQuestStep.nRewardXPTier - 1] : 0;
        nGoldGranted = (tQuestStep.nRewardGoldTier > 0) ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nQuestTierGold[tQuestStep.nRewardGoldTier - 1] : 0;
    }
    else
    {
        if (tQuestStep.eReplayRewardType == QUEST_REWARD_SHARED_RECIPE)
        {
            snoRecipe = tQuestStep.snoReplayRewardRecipe[PLAYER_CLASS_BARBARIAN];
            snoTreasureClass = SNO_NONE;
        }
        else if (tQuestStep.eReplayRewardType == QUEST_REWARD_INDIVIDUAL_RECIPE)
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(tPlayer.idPlayerACD);

            snoRecipe = tQuestStep.snoReplayRewardRecipe[ePlayerClass];
            snoTreasureClass = SNO_NONE;
        }
        else if (tQuestStep.eReplayRewardType == QUEST_REWARD_TREASURE_CLASS)
        {
            snoRecipe = SNO_NONE;
            snoTreasureClass = tQuestStep.snoReplayRewardTreasureClass;
        }
        else
        {
            snoRecipe = SNO_NONE;
            snoTreasureClass = SNO_NONE;
        }

        QuestType eQuestType = QuestGetQuestType(snoQuest);
        if (eQuestType == QUEST_TYPE_BOUNTY)
        {
            if (pdefQuest->tBountyData.eBountyType >= 0 && pdefQuest->tBountyData.eBountyType < NUM_NORMAL_BOUNTY_TYPES)
            {
                // Inference: the original clamps the reward tier to the five bounty entries.
                int32 nRewardTier = Math::Pin(pdefQuest->tBountyData.nRewardTier, 0, 4);
                nXPGranted = (tQuestStep.nReplayRewardXPTier > 0) ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nBountyXP[nRewardTier] : 0;
                nGoldGranted = (tQuestStep.nReplayRewardGoldTier > 0) ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nBountyGold[nRewardTier] : 0;
            }
            else
            {
                // Inference: non-normal bounty types share the zero reward tail with Horadric quests.
                nXPGranted = 0;
                nGoldGranted = 0;
            }
        }
        else if (eQuestType == QUEST_TYPE_HORADRIC_QUEST)
        {
            nXPGranted = 0;
            nGoldGranted = 0;
        }
        else
        {


        nXPGranted = (tQuestStep.nReplayRewardXPTier > 0) ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nQuestTierXP[tQuestStep.nReplayRewardXPTier - 1] : 0;
        nGoldGranted = (tQuestStep.nReplayRewardGoldTier > 0) ? tExpTable.ptExperienceLevels[nTreasureLevel - 1].nQuestTierGold[tQuestStep.nReplayRewardGoldTier - 1] : 0;
        }
    }


    bAddNVStack = (tQuestStep.dwFlags >> QSF_GRANT_NV_ON_COMPLETION) & 1;
    BOOL bQuestIsTieredRiftChallenge = snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_TIERED_RIFT_CHALLENGE);
    BOOL bQuestIsDungeonfinder = snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER);
    // Inference: the original branches select rift scalars, ordinary handicap, or challenge-only no-op.
    if ((bQuestIsDungeonfinder && NephalemRift::IsTieredRiftActive()) || bQuestIsTieredRiftChallenge)
    {
        if (bQuestIsDungeonfinder)
        {

            ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
            // Inference: 2585 is a two-sided range check (column 40 = its &&) on a signed count; the original folds it to one unsigned compare (without the int32 cast ours is 1332 B).
            int32 nGameBalanceIndex = Math::Max(0, NephalemRift::GetActiveTieredRiftLevel());
            nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);

            if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
            {
                const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];
                float flDungeonFinderGoldScalar = tTieredLootRunLevel.tLootRunValues.flGoldFind;
                float flDungeonFinderXPScalar = tTieredLootRunLevel.tLootRunValues.flXPBonus;

                nXPGranted = (uint64)((float)nXPGranted * (1.0f + flDungeonFinderXPScalar));
                nGoldGranted = Math::FloatToInt((float)nGoldGranted * (1.0f + flDungeonFinderGoldScalar));
            }

        }
    }
    else
    {




        int32 nQuestHandicapLevel = GameGetHandicapLevel();


        if (GameActiveGameCommonDataIsServer())
        {
            QuestType eQuestType = QuestGetQuestType(snoQuest);
            if (eQuestType == QUEST_TYPE_MAIN_QUEST)
            {


                QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
                if (ptQuestGlobals)
                {
                    nQuestHandicapLevel = Math::Min(nQuestHandicapLevel, ptQuestGlobals->nCurrentMainLineQuestHandicapLevel);
                }
            }
        }


        float flMagicFindFromHandicap;
        float flGoldFindFromHandicap;
        float flXPBonusFromHandicap;
        GameGetPlayerHandicapValues(nQuestHandicapLevel, flMagicFindFromHandicap, flGoldFindFromHandicap, flXPBonusFromHandicap);
        nXPGranted = (uint64)((float)nXPGranted * (1.0f + flXPBonusFromHandicap));
        nGoldGranted = Math::FloatToInt((float)nGoldGranted * (1.0f + flGoldFindFromHandicap));
    }
    // Inference: successful reward paths share the original TRUE-return tail.
    return TRUE;
}



void QuestsRefreshQuestDialogs(SNO snoQuest)
{








































}



void QuestSetGameOver()
{

    QuestGlobals * pGlobals = QuestsGetQuestGlobals();
    if (pGlobals)
        pGlobals->bGameOver = TRUE;
}





BOOL QuestIsGameOver()
{

    QuestGlobals * pGlobals = QuestsGetQuestGlobals();
    if (pGlobals)
        return pGlobals->bGameOver;


    return FALSE;
}




BOOL QuestStepIsEligibleForMatchmaking(SNO snoQuest, int32 nStepUID)
{


    if (GameIsOpenWorld())
        return TRUE;
    if (snoQuest == SNO_NONE)
        return FALSE;



    ScopedQuestDefinitionLockR pdefQuest(snoQuest);

    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
    {
        const QuestUnassignedStep & tQuestUnassignedStep = pdefQuest->tQuestUnassignedStep;
        return !(tQuestUnassignedStep.dwFlags & (1 << QUSF_PREVENT_MATCHMAKING));
    }

    int32 nUIDPreventStepSetByDesigner = QUEST_NULL_STEP_UID;
    int32 nUIDPreventStepImplied = QUEST_NULL_STEP_UID;
    int32 nHalfwayIndex = pdefQuest->nNumQuestSteps / 2;
    for (int32 i = 0; i < pdefQuest->nNumQuestSteps; i++)
    {
        const QuestStep & tCurrStep = pdefQuest->arQuestSteps[i];
        if (tCurrStep.dwFlags & (1 << QSF_PREVENT_MATCHMAKING))
        {
            nUIDPreventStepSetByDesigner = tCurrStep.nUID;
            break;
        }

        if (nUIDPreventStepImplied == QUEST_NULL_STEP_UID && i >= nHalfwayIndex)
        {

            nUIDPreventStepImplied = tCurrStep.nUID;
        }
    }








    int32 nUIDPreventStep = (nUIDPreventStepSetByDesigner != QUEST_NULL_STEP_UID) ? nUIDPreventStepSetByDesigner : nUIDPreventStepImplied;




    if (nUIDPreventStep == nStepUID)
        return FALSE;





    return QuestStepIsReachableByStep(snoQuest, nStepUID, nUIDPreventStep);
}



BOOL QuestIsEligibleForMatchmaking()
{

    if (QuestIsGameOver())
        return FALSE;






    QuestInstance * ptCurrentQuest = GetLeadingMainLineQuest();
    if (ptCurrentQuest)
        return QuestStepIsEligibleForMatchmaking(ptCurrentQuest->snoQuest, ptCurrentQuest->nCurrentStepUID);


    return FALSE;
}




















































































void QuestGetBonusAward(const Player * ptPlayer, SNO snoQuest, const QuestStepBonusObjectiveSet & tBonusObjectiveSet, int32 & nBonusXPGranted, int32 & nBonusGoldGranted, int32 nTimesCompleted)
{





    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;




    int32 nDivisor = Math::Max(tBonusObjectiveSet.nRewardDivisor, 1);

    int32 nRewardXPLevel = GameGetMonsterLevel();
    if (ptPlayer && ptPlayer->idPlayerACD != ACDID_INVALID)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            nRewardXPLevel = Math::Min(nRewardXPLevel, ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL)));
        }
    }
    nRewardXPLevel = Math::Max(nRewardXPLevel, 1);
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);


    if (pdefQuest->eQuestType != QUEST_TYPE_CHALLENGE)
    {
        nBonusXPGranted += ((tBonusObjectiveSet.nRewardXPTier > 0) ? tExpTable.ptExperienceLevels[nRewardXPLevel - 1].nQuestTierXP[tBonusObjectiveSet.nRewardXPTier - 1] : 0) * nTimesCompleted / nDivisor;
        nBonusGoldGranted += ((tBonusObjectiveSet.nRewardGoldTier > 0) ? tExpTable.ptExperienceLevels[nRewardXPLevel - 1].nQuestTierGold[tBonusObjectiveSet.nRewardGoldTier - 1] : 0) * nTimesCompleted / nDivisor;
    }
    else
    {

        nBonusXPGranted += tExpTable.ptExperienceLevels[nRewardXPLevel - 1].nChallengeBonusXP[pdefQuest->eChallengeType] * nTimesCompleted / nDivisor;
        nBonusGoldGranted += tExpTable.ptExperienceLevels[nRewardXPLevel - 1].nChallengeBonusGold[pdefQuest->eChallengeType] * nTimesCompleted / nDivisor;
    }


    float flMagicFindFromHandicap;
    float flGoldFindFromHandicap;
    float flXPBonusFromHandicap;
    GameGetPlayerHandicapValues(GameGetHandicapLevel(), flMagicFindFromHandicap, flGoldFindFromHandicap, flXPBonusFromHandicap);
    nBonusXPGranted = Math::FloatToInt(nBonusXPGranted * (1.0f + flXPBonusFromHandicap));
    nBonusGoldGranted = Math::FloatToInt(nBonusGoldGranted * (1.0f + flGoldFindFromHandicap));
}



BOOL QuestHasAdditionalLevelArea(SNO snoQuest, SNO snoLevelArea)
{


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return TRUE;



    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    if (!pdefQuest)
        return FALSE;
    for (int32 i = 0; i < NUM_QUEST_ADDITIONAL_LEVEL_AREAS; i++)
    {
        if (pdefQuest->snoAdditionalLevelAreas[i] == snoLevelArea)
            return TRUE;
    }
    return FALSE;
}








static const GlobalSNO sctKeyWardenBossEncounters[4] = { GLOBALSNO_BOSS_ENCOUNTER_KEYWARDEN_A1, GLOBALSNO_BOSS_ENCOUNTER_KEYWARDEN_A2, GLOBALSNO_BOSS_ENCOUNTER_KEYWARDEN_A3, GLOBALSNO_BOSS_ENCOUNTER_KEYWARDEN_A4 };






SNO QuestsOpenWorldGetKeyWardenBossEncounter(int32 nAct)
{

    if (nAct < 1 || nAct > 4)
        return SNO_NONE;

    return GlobalSNOGet(sctKeyWardenBossEncounters[nAct - 1]);
}





void QuestsAddOpenWorldTrackedSNO(SNO sno)
{

    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
    if (ptQuestGlobals->listOpenWorldTrackedSNOs.Find(sno))
        return;
    ptQuestGlobals->listOpenWorldTrackedSNOs.AddTail_Tracked(sno);


    if (GameActiveGameCommonDataIsServer())
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

        SNODataMessage tMessage;
        tMessage.snoData = sno;
        MessageSendToClient(listDestPlayers, SMSG_OPEN_WORLD_TRACKED_SNO, &tMessage, sizeof(tMessage));
    }
}





void QuestsSyncOpenWorldTrackedSNOToPlayer(Player * ptPlayer)
{



    if (!ptPlayer)
        return;



    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);

    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
    POS pos = ptQuestGlobals->listOpenWorldTrackedSNOs.GetHeadPosition();
    while (pos)
    {
        SNODataMessage tMessage;
        tMessage.snoData = ptQuestGlobals->listOpenWorldTrackedSNOs.GetNext(pos);
        MessageSendToClient(listDestPlayers, SMSG_OPEN_WORLD_TRACKED_SNO, &tMessage, sizeof(tMessage));
    }
}




BOOL QuestsOpenWorldTrackedSNOIsComplete(SNO sno)
{

    QuestGlobals * ptQuestGlobals = QuestsGetQuestGlobals();
    return ptQuestGlobals->listOpenWorldTrackedSNOs.Find(sno) != NULL;
}








BOOL QuestIsAnyBountyType(const QuestDefinition * ptQuestDef)
{

    return ptQuestDef && (ptQuestDef->eQuestType == QUEST_TYPE_BOUNTY || ptQuestDef->eQuestType == QUEST_TYPE_HORADRIC_QUEST);
}





BOOL QuestIsAnyBountyType(SNO snoQuest)
{

    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    return QuestIsAnyBountyType(pdefQuest);
}



BOOL QuestIsAnySetDungeonType(SNO snoQuest)
{

    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    return pdefQuest->eQuestType >= QUEST_TYPE_SET_DUNGEON && pdefQuest->eQuestType <= QUEST_TYPE_SET_DUNGEON_TRACKER;
}











SNO QuestGetBountyTurninQuestForAct(Act eAct)
{

    if (!GameIsOpenWorld())
        return SNO_NONE;

    GlobalSNO eGlobalSNO;

    switch (eAct)
    {
    case ACT_1: eGlobalSNO = GLOBALSNO_QUEST_BOUNTY_TURNIN_A1; break;
    case ACT_2: eGlobalSNO = GLOBALSNO_QUEST_BOUNTY_TURNIN_A2; break;
    case ACT_3: eGlobalSNO = GLOBALSNO_QUEST_BOUNTY_TURNIN_A3; break;
    case ACT_4: eGlobalSNO = GLOBALSNO_QUEST_BOUNTY_TURNIN_A4; break;
    case ACT_5: eGlobalSNO = GLOBALSNO_QUEST_BOUNTY_TURNIN_A5; break;
    default: return SNO_NONE;
    }
    return GlobalSNOGet(eGlobalSNO);
}










BOOL QuestAreAllBountiesCompleteForAct(Act eAct)
{

    if (!GameIsOpenWorld())
        return FALSE;



    SNO snoQuest = QuestGetBountyTurninQuestForAct(eAct);

    if (snoQuest != SNO_NONE)
    {
        QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
        if (ptQuestInstance && QuestIsComplete(*ptQuestInstance))
            return TRUE;
    }


    return FALSE;
}




float QuestGetAutoPingTimeSeconds(SNO snoQuest)
{

    if (!GameIsOpenWorld())
        return -1.0f;



    if (!QuestIsAnyBountyType(snoQuest))
        return -1.0f;

    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    return ptQuestDef->tBountyData.flAutoPingTime;
}




uint32 QuestGetAutoPingTimeTicks(const SNO snoQuest)
{

    float flAutoPingSeconds = QuestGetAutoPingTimeSeconds(snoQuest);
    if (flAutoPingSeconds < 0.0f)
        return (uint32)-1;




    return Math::FloatToInt(flAutoPingSeconds * 60.0f);
}
