// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Quests/Quests.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../Definitions/QuestDefinition.h"
#include "../GameBalance.h"
#include "QuestsSwig.h"
#include "../SNO/SNOName.h"
#include "../../Server/SGame.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XPooledList.h"

enum QuestInstanceState {
    QUEST_INSTANCE_UNASSIGNED = 0,
    QUEST_INSTANCE_ACTIVE = 1,
    QUEST_INSTANCE_COMPLETED = 2,
    QUEST_INSTANCE_FAILED = 3,
};

enum QuestInstanceFlags {
    QUEST_FLAG_COLOR_ORBS_RED = 0,
    QUEST_FLAG_COLOR_ORBS_GREEN = 1,
    QUEST_FLAG_COLOR_ORBS_BLUE = 2,
};

struct QuestInstance;
typedef void (*QuestIterateCallback)(QuestInstance &, void *);

struct QuestInstance {  // size 0x168
    SNO snoQuest;  // 0x00
    uint32 dwStarted;  // 0x04
    uint32 dwFlags;  // 0x08
    SNO snoLevelArea;  // 0x0c
    ACDID idACDMeter;  // 0x10
    float flMeter;  // 0x14
    QuestInstanceState eInstanceState;  // 0x18
    int32 nCurrentStepUID;  // 0x1c
    int32 nResumableStepUID;  // 0x20
    int32 arObjectiveState[20];  // 0x24
    BOOL arObjectiveCompleted[20];  // 0x74
    int32 arFailureConditionState[20];  // 0xc4
    int32 arBonusRewards[20];  // 0x114
    int32 nPlayerIndex;  // 0x164
};
static_assert(sizeof(QuestInstance) == 0x168, "QuestInstance");

enum BountyActivationType {
    BOUNTY_ACTIVATION_ACTOR = 0,
    BOUNTY_ACTIVATION_LEVEL_AREA = 1,
};

struct BountyActivationRecord {  // size 0x18
    BountyActivationType eType;  // 0x00
    SNO snoActor;  // 0x04
    SNO snoWorld;  // 0x08
    SNO snoLevelArea;  // 0x0c
    SNO snoQuest;  // 0x10
    BOOL bFlaggedForDeletion;  // 0x14
};
static_assert(sizeof(BountyActivationRecord) == 0x18, "BountyActivationRecord");

typedef XGrowableList<BountyActivationRecord, const BountyActivationRecord &> BountyActivationRecordList;

struct QuestGlobals {  // size 0x580
    XListMemoryPool<QuestInstance> tQuestInstancePool;  // 0x00
    XPooledList<QuestInstance, QuestInstance> tQuestInstanceList;  // 0x30
    int32 nNumMainQuests;  // 0x58
    int32 nNumSideQuests;  // 0x5c
    Act eCurrentAct;  // 0x60
    UpdateData tQuestsUpdateData;  // 0x68
    BOOL bForcePlayerLocationTriggerEventAfterActivationUpdate;  // 0x88
    BOOL bStartup;  // 0x8c
    BOOL bResendEnterEvents;  // 0x90
    BOOL arDynamicQuestObjectiveDirty[10];  // 0x94
    SNO snoCurrentMainLineQuest;  // 0xbc
    int32 nCurrentMainLineQuestStepUID;  // 0xc0
    int32 nCurrentMainLineQuestHandicapLevel;  // 0xc4
    BountyActivationRecordList tBountyActivationRecordList;  // 0xc8
    UpdateData tBountySceneActivationUpdate;  // 0x120
    uint32 dwUpdatesSinceLastDeactivation;  // 0x140
    BOOL bGameOver;  // 0x144
    QuestStepObjective tDynamicQuestObjectives[10];  // 0x148
    int32 arDynamicQuestObjectiveState[10];  // 0x4b8
    BOOL arDynamicQuestObjectiveCompleted[10];  // 0x4e0
    BOOL arDynamicQuestObjectiveHidden[10];  // 0x508
    XPooledList<QuestInstance, QuestInstance> tQuestPerPlayerInstanceList;  // 0x530
    XPooledDataIDList listOpenWorldTrackedSNOs;  // 0x558
};
static_assert(sizeof(QuestGlobals) == 0x580, "QuestGlobals");

struct QuestUpdateMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoQuest;  // 0x08
    SNO snoLevelArea;  // 0x0c
    int32 nStepUID;  // 0x10
    BOOL bFailed;  // 0x14
    BOOL bDisplayButton;  // 0x18
};
static_assert(sizeof(QuestUpdateMessage) == 0x1c, "QuestUpdateMessage");

struct QuestMeterMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoQuest;  // 0x08
    ACDNetworkName annMeter;  // 0x0c
    float flMeter;  // 0x10
};
static_assert(sizeof(QuestMeterMessage) == 0x14, "QuestMeterMessage");

struct QuestFlagsMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoQuest;  // 0x08
    uint32 dwFlags;  // 0x0c
};
static_assert(sizeof(QuestFlagsMessage) == 0x10, "QuestFlagsMessage");

struct QuestCounterMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoQuest;  // 0x08
    SNO snoLevelArea;  // 0x0c
    int32 nStepUID;  // 0x10
    int32 nObjectiveIndex;  // 0x14
    int32 nValue;  // 0x18
    BOOL bCompleted;  // 0x1c
};
static_assert(sizeof(QuestCounterMessage) == 0x20, "QuestCounterMessage");

struct DynamicQuestObjectiveMessage {  // size 0x48
    RequiredMessageHeader tHeader;  // 0x00
    int32 nUID;  // 0x08
    QuestStepObjectiveType eObjectiveType;  // 0x0c
    int32 nParam1;  // 0x10
    int32 nParam2;  // 0x14
    SNOName snoname1;  // 0x18
    SNOName snoname2;  // 0x20
    GBID gbidParam1;  // 0x28
    GBID gbidParam2;  // 0x2c
    BOOL bHiddenUntilComplete;  // 0x30
    int32 dwFlags;  // 0x34
    GBID gbidItemToShow;  // 0x38
    int32 nState;  // 0x3c
    BOOL bCompleted;  // 0x40
    BOOL bHidden;  // 0x44
};
static_assert(sizeof(DynamicQuestObjectiveMessage) == 0x48, "DynamicQuestObjectiveMessage");


// Defined in Quests.cpp.
QuestGlobals * QuestsGetQuestGlobals();
QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACDPlayer);
BOOL QuestStepIsReachableByStep(SNO snoQuest, int32 nStepUIDSource, int32 nStepUIDDest);
BOOL QuestStepIsReachableByStep(const QuestDefinition * ptQuestDef, int32 nStepUIDSource, int32 nStepUIDDest);
BOOL QuestStepCanReachReward(SNO snoQuest, int32 nStepUID);
BOOL QuestStepCanReachReward(const QuestDefinition * ptQuestDef, int32 nStepUID);
BOOL QuestStepIsEligibleForMatchmaking(SNO snoQuest, int32 nStepUID);
BOOL QuestIsAnyBountyType(const QuestDefinition * ptQuestDef);
BOOL QuestIsAnyBountyType(SNO snoQuest);
BOOL QuestStepGetReward(const Player & tPlayer, SNO snoQuest, const QuestStep & tQuestStep, uint64 & nXPGranted, int32 & nGoldGranted, SNO & snoRecipe, SNO & snoTreasureClass, int32 & nTreasureLevel, BOOL & bReplayReward, BOOL & bAddNVStack);
class ActorCommonData;
BOOL sTestUnlockingPrimaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest);
BOOL sTestUnlockingSecondaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest);
void QuestsAdvanceToStep(QuestInstance & tQuestInstance, int32 nStepUID, BOOL bRollback);
BOOL QuestStepIsResumable(SNO snoQuest, int32 nStepUID);
BOOL QuestIsGameOver();
BOOL QuestHasAdditionalLevelArea(SNO snoQuest, SNO snoLevelArea);
BOOL QuestObjectivePassed(const QuestStepObjective & tObjectiveToCheck, int32 nObjectiveCounter);
BOOL sTestPrimaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest);
BOOL sTestSecondaryObjectives(const ActorCommonData & tACD, const QuestStep & tStep, const QuestInstance & tQuest);
void QuestsQuestInstanceInitialize(QuestInstance & tQuestInstance, SNO snoQuest, SNO snoLevelArea);
void sQuestsPickBounties();
void QuestsActivateForAct(Act eAct);

SNO QuestsOpenWorldGetKeyWardenBossEncounter(int32 nAct);
void QuestsAddOpenWorldTrackedSNO(SNO sno);
