// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/QuestDefinition.h
// Subset: QuestDefinition's step, completion and bounty data and the read lock. The other members are padding
// (invented names) until someone needs them; the full reference header pulls in a lot more.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../GameBalance.h"
#include "../GameSwig.h"
#include "../Quests/QuestsSwig.h"
#include "../SNO/SNOFiles.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

enum {
    MAX_QUEST_NAME_LENGTH = 64,
    MAX_QUEST_ICON_NAME_LENGTH = 64,
    MAX_STRING_PARAMETER_LENGTH = 256,
    NUM_WAYPOINT_LEVEL_AREAS = 2,
    NUM_QUEST_ADDITIONAL_LEVEL_AREAS = 18,
    CHALLENGE_TIMEOUT_IN_SECONDS = 60,
};

enum QuestObjectiveState {
    OBJECTIVESTATE_INCOMPLETE = 0,
    OBJECTIVESTATE_PASSED = 1,
    OBJECTIVESTATE_FAILED = 2,
};

enum {
    QSF_RESUMABLE_STEP = 0,
    QSF_SAVE_PARTIAL_COMPLETION = 1,
    QSF_TRACK_PER_PLAYER = 2,
    QSF_SHARED_LOG_TEXT = 3,
    QSF_SHOW_METER = 4,
    QSF_ORPHANED_STEP = 5,
    QSF_SELECTABLE_STEP = 6,
    QSF_PREVENT_MATCHMAKING = 7,
    QSF_USE_PREVIOUS_LOG_TEXT = 8,
    QSF_GRANT_NV_ON_COMPLETION = 9,
    QSF_HIDE_BONUS_TEXT = 10,
};

enum QuestFlags {
    QUEST_FLAG_AUTO_GENERATE_MAP_ICON = 0,
    QUEST_FLAG_AUTO_MARK_OBJECTIVE_NPCS = 1,
    QUEST_FLAG_SHOW_NEW_QUEST_BLINKER = 2,
    QUEST_FLAG_GLOBAL_EVENT = 3,
    QUEST_FLAG_SHARED_DESCRIPTION_TEXT = 4,
    NUM_QUEST_FLAGS = 5,
};

struct QuestStepObjective {  // size 0x58
    int32 nUID;  // 0x00
    QuestStepObjectiveType eObjectiveType;  // 0x04
    int32 nParam1;  // 0x08
    int32 nParam2;  // 0x0c
    SNOName snoname1;  // 0x10
    SNOName snoname2;  // 0x18
    GBID gbidParam1;  // 0x20
    GBID gbidParam2;  // 0x24
    XPtr64<char> szParam1_VariableArray;  // 0x28
    SerializeData serParam1;  // 0x30
    XPtr64<char> szParam2_VariableArray;  // 0x38
    SerializeData serParam2;  // 0x40
    BOOL bHiddenUntilComplete;  // 0x48
    int32 dwFlags;  // 0x4c
    GBID gbidItemToShow;  // 0x50
    void GenerateVariableArray();
};
static_assert(sizeof(QuestStepObjective) == 0x58, "QuestStepObjective");

struct QuestStepObjectiveSet {  // size 0x18
    int32 nNextStepUID;  // 0x00
    XPtr64<QuestStepObjective> arStepObjectives;  // 0x08
    SerializeData serStepObjectives;  // 0x10
    void GenerateVariableArray();
};
static_assert(sizeof(QuestStepObjectiveSet) == 0x18, "QuestStepObjectiveSet");

struct QuestStepBonusObjectiveSet {  // size 0x28
    int32 nRewardXPTier;  // 0x00
    int32 nRewardGoldTier;  // 0x04
    int32 nRewardDivisor;  // 0x08
    BOOL bRepeatable;  // 0x0c
    BOOL bInverse;  // 0x10
    XPtr64<QuestStepObjective> arStepBonusObjectives;  // 0x18
    SerializeData serStepBonusObjectives;  // 0x20
    void GenerateVariableArray();
};
static_assert(sizeof(QuestStepBonusObjectiveSet) == 0x28, "QuestStepBonusObjectiveSet");

struct QuestStepFailureConditionSet;

enum QuestStepFailureConditionType {
    FAILURE_CONDITION_MONSTER_DIED = 0,
    FAILURE_CONDITION_PLAYER_DIED = 1,
    FAILURE_CONDITION_ACTOR_DIED = 2,
    FAILURE_CONDITION_TIMED_EVENT_EXPIRED = 3,
    FAILURE_CONDITION_ITEM_USED = 4,
    FAILURE_CONDITION_GAME_FLAG_SET = 5,
    FAILURE_CONDITION_PLAYER_FLAG_SET = 6,
    FAILURE_CONDITION_EVENT_RECEIVED = 7,
    NUM_QUEST_STEP_FAILURE_CONDITION_TYPES = 8,
};

enum {
    QUSF_SELECTABLE_STEP = 0,
    QUSF_PREVENT_MATCHMAKING = 1,
};

struct QuestStepFailureCondition {  // size 0x48
    QuestStepFailureConditionType eFailureConditionType;  // 0x00
    int32 nParam1;  // 0x04
    int32 nParam2;  // 0x08
    SNOName snoname1;  // 0x0c
    SNOName snoname2;  // 0x14
    GBID gbidParam1;  // 0x1c
    GBID gbidParam2;  // 0x20
    XPtr64<char> szParam1_VariableArray;  // 0x28
    SerializeData serParam1;  // 0x30
    XPtr64<char> szParam2_VariableArray;  // 0x38
    SerializeData serParam2;  // 0x40
    void GenerateVariableArray();
};
static_assert(sizeof(QuestStepFailureCondition) == 0x48, "QuestStepFailureCondition");

struct QuestStepFailureConditionSet {  // size 0x10
    XPtr64<QuestStepFailureCondition> arStepFailureConditions;  // 0x00
    SerializeData serStepFailureConditions;  // 0x08
    void GenerateVariableArray();
};
static_assert(sizeof(QuestStepFailureConditionSet) == 0x10, "QuestStepFailureConditionSet");

struct QuestUnassignedStep {  // size 0x28
    int32 nUID_deprecated;  // 0x00
    uint32 dwFlags;  // 0x04
    XPtr64<QuestStepObjectiveSet> arStepObjectiveSets;  // 0x08
    SerializeData serStepObjectiveSets;  // 0x10
    XPtr64<QuestStepFailureConditionSet> arStepFailureConditionSets;  // 0x18
    SerializeData serStepFailureConditionSets;  // 0x20
    void GenerateVariableArray();
};
static_assert(sizeof(QuestUnassignedStep) == 0x28, "QuestUnassignedStep");

enum QuestRewardType {
    QUEST_REWARD_NO_ITEM = 0,
    QUEST_REWARD_SHARED_RECIPE = 1,
    QUEST_REWARD_INDIVIDUAL_RECIPE = 2,
    QUEST_REWARD_TREASURE_CLASS = 3,
    NUM_QUEST_REWARD_TYPES = 4,
};

struct QuestStep {  // size 0xb0
    XPtr64<char> szStepName_VariableArray;  // 0x00
    SerializeData serStepName;  // 0x08
    int32 nUID;  // 0x10
    uint32 dwFlags;  // 0x14
    QuestRewardType eRewardType;  // 0x18
    SNO snoRewardRecipe[7];  // 0x1c
    SNO snoRewardTreasureClass;  // 0x38
    int32 nRewardXPTier;  // 0x3c
    int32 nRewardGoldTier;  // 0x40
    QuestRewardType eReplayRewardType;  // 0x44
    SNO snoReplayRewardRecipe[7];  // 0x48
    SNO snoReplayRewardTreasureClass;  // 0x64
    int32 nReplayRewardXPTier;  // 0x68
    int32 nReplayRewardGoldTier;  // 0x6c
    SNO snoPowerGranted;  // 0x70
    SNO snoWaypointLevelAreas[NUM_WAYPOINT_LEVEL_AREAS];  // 0x74
    XPtr64<QuestStepObjectiveSet> arStepObjectiveSets;  // 0x80
    SerializeData serStepObjectiveSets;  // 0x88
    XPtr64<QuestStepBonusObjectiveSet> arStepBonusObjectiveSets;  // 0x90
    SerializeData serStepBonusObjectiveSets;  // 0x98
    XPtr64<QuestStepFailureConditionSet> arStepFailureConditionSets;  // 0xa0
    SerializeData serStepFailureConditionSets;  // 0xa8
    void GenerateVariableArray();
};
static_assert(sizeof(QuestStep) == 0xb0, "QuestStep");

struct QuestCompletionStep {  // size 0x18
    XPtr64<char> szStepName_VariableArray;  // 0x00
    SerializeData serStepName;  // 0x08
    int32 nUID;  // 0x10
    BOOL bSharedLogText;  // 0x14
    void GenerateVariableArray();
};
static_assert(sizeof(QuestCompletionStep) == 0x18, "QuestCompletionStep");

struct BountyData {  // size 0x5c
    Act eAct;  // 0x00
    BountyType eBountyType;  // 0x04
    int32 nRewardTier;  // 0x08
    SNO snoLevelArea;  // 0x0c
    SNO snoWorld;  // 0x10
    SNO snoRequiredQuest;  // 0x14
    SNO snoRequiredQuestWorld;  // 0x18
    SNO snoRequiredActor;  // 0x1c
    SNO snoRequiredActorWorld;  // 0x20
    SNO snoRequiredActorLevelArea;  // 0x24
    SNO snoRequiredScene;  // 0x28
    SNO snoRequiredSceneWorld;  // 0x2c
    GBID gbidRequiredLabel;  // 0x30
    SNO snoRequiredAdventure;  // 0x34
    SNO snoRequiredAdventureWorld;  // 0x38
    SNO snoRequiredAdventureLevelArea;  // 0x3c
    SNO snoRequiredEncounter;  // 0x40
    SNO snoRequiredScene2;  // 0x44
    SNO snoRequiredSceneWorld2;  // 0x48
    GBID gbidRequiredLabel2;  // 0x4c
    SNO snoGlobalConductor;  // 0x50
    SNO snoBossEncounter;  // 0x54
    float flAutoPingTime;  // 0x58
};
static_assert(sizeof(BountyData) == 0x5c, "BountyData");

struct QuestDefinition {  // size 0x120
    RequiredDefinitionHeader tHeader;  // 0x00
    QuestType eQuestType;  // 0x0c
    int32 nNumQuestSteps;  // 0x10
    int32 nNumQuestCompletionSteps;  // 0x14
    int32 nQuestUID;  // 0x18
    uint32 dwFlags;  // 0x1c
    uint32 hZoneLabel;  // 0x20 (StringLabelHandle)
    Act eAct;  // 0x24
    QuestUnassignedStep tQuestUnassignedStep;  // 0x28
    XPtr64<QuestStep> arQuestSteps;  // 0x50
    SerializeData serQuestSteps;  // 0x58
    XPtr64<QuestCompletionStep> arQuestCompletionSteps;  // 0x60
    SerializeData serQuestCompletionSteps;  // 0x68
    SNO snoAdditionalLevelAreas[18];  // 0x70
    SNO snoChallengeWorld;  // 0xb8
    ChallengeType eChallengeType;  // 0xbc
    BountyData tBountyData;  // 0xc0
    uint8 _unadopted1[0x120 - 0x11c];  // (invented name: 0x11c .. 0x120; 8-byte aligned in the original)
    void GenerateVariableArray();
};
static_assert(sizeof(QuestDefinition) == 0x120, "QuestDefinition");

class QuestGroupDef : public GroupDefinition {  // size 0xa0
public:
    QuestGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 397; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 400
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_COREDATA; }  // vtable slot 7, line 405
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
};
static_assert(sizeof(QuestGroupDef) == 0xa0, "QuestGroupDef");

extern QuestGroupDef * gptGroupDefQuest;  // QuestDefinition.cpp:827

typedef ScopedDefinitionLockR<QuestDefinition> ScopedQuestDefinitionLockR;

// Declared here from their definitions in QuestDefinition.cpp.
extern LPCSTR gszQuestNameLabel;  // QuestDefinition.cpp:1500
SNO QuestGetStringList(SNO snoQuest);  // QuestDefinition.cpp:1563

// Line 453. Its callers call LockRFast and have nothing from an unlock, so the unlock is taken
// to be empty, as for ObserverDefinition.h.
template <> inline const QuestDefinition * DefinitionLockR<QuestDefinition>(SNO sno) {
    return (const QuestDefinition *)gptGroupDefQuest->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<QuestDefinition>(const QuestDefinition *) {}

// Query and label functions defined in QuestDefinition.cpp:1514-1977.
int32 QuestGetNumSteps(SNO snoQuest);
CRefString QuestGetName(SNO snoQuest);
CRefString QuestGetStepDescriptionLabel(const QuestDefinition * ptQuestDef, int32 nStepUID, PlayerClass ePlayerClass);
CRefString QuestGetStepHeaderLabel(const QuestDefinition * ptQuestDef, int32 nStepUID);
CRefString QuestGetStepSplashLabel(const QuestDefinition * ptQuestDef, int32 nStepUID);
CRefString QuestGetStepMeterLabel(const QuestDefinition * ptQuestDef, int32 nStepUID);
CRefString QuestGetObjectiveDescriptionLabel(const QuestStepObjective & tObjective, PlayerClass ePlayerClass);
uint32 QuestStepUIDToIndex(const QuestDefinition * ptQuestDef, int32 nStepUID);
BOOL QuestStepIsCompletionStep(SNO snoQuest, int32 nStepUID);
BOOL QuestStepIsCompletionStep(const QuestDefinition * ptQuestDef, int32 nStepUID);
QuestType QuestGetQuestType(SNO snoQuest);
Act QuestGetBountyAct(SNO snoQuest);
Act QuestGetAct(SNO snoQuest);
