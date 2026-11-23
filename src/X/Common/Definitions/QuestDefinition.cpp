// Original file: X/Common/Definitions/QuestDefinition.cpp
// All indexed functions are written; helper-body gaps are recorded below.
// Existing matching definitions retain their original positions.
#include "QuestDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Player.h"
#include "../SNO/SymbolicValue.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp
extern const XTypeDescriptor * DT_SNO_NAME;
extern const SymbolicEntry gQuestStepObjectiveTypes[21];  // X/Common/SNO/SymbolicValue.cpp:5101
extern const SymbolicEntry gQuestStepFailureConditionTypes[9];  // :5127
extern const SymbolicEntry gQuestTypes[10];  // :5151
extern const SymbolicEntry gBountyTypes[9];  // :5172
extern const SymbolicEntry gChallengeTypes[7];  // :5188
extern const SymbolicEntry gQuestRewardTypes[5];  // :5211
extern const XTypeDescriptor * DT_QUEST_STEP_OBJECTIVE_SET;
extern const XTypeDescriptor * DT_QUEST_STEP_BONUS_OBJECTIVE_SET;
extern const XTypeDescriptor * DT_QUEST_STEP_FAILURE_CONDITION_SET;
extern const XTypeDescriptor * DT_QUEST_UNASSIGNED_STEP;
extern const XTypeDescriptor * DT_QUEST_STEP;
extern const XTypeDescriptor * DT_QUEST_COMPLETION_STEP;
extern const XTypeDescriptor * DT_BOUNTY_DATA;
extern const XTypeDescriptor * DT_QUEST_STEP_OBJECTIVE;
extern const XTypeDescriptor * DT_QUEST_STEP_FAILURE_CONDITION;

// X/Common/Definitions/QuestDefinition.h; the adopted header does not carry it yet.
struct QuestLevelRange {  // size 0x8
    QuestLevelRange();
    int32 nMin;  // 0x00
    int32 nMax;  // 0x04
};
static_assert(sizeof(QuestLevelRange) == 0x8, "QuestLevelRange");

// Lines 91-224. The field names are "" in the binary.
static const TypeDescriptorField QuestStepObjective_Fields[16] = {
    TypeDescriptorField("", DT_INT, offsetof(QuestStepObjective, nUID), NULL, 1),
    EnumTypeDescriptorField("", offsetof(QuestStepObjective, eObjectiveType), NULL, gQuestStepObjectiveTypes),
    TypeDescriptorField("", DT_INT, offsetof(QuestStepObjective, nParam1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStepObjective, nParam2), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(QuestStepObjective, snoname1), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(QuestStepObjective, snoname2), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(QuestStepObjective, gbidParam1), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(QuestStepObjective, gbidParam2), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStepObjective, serParam1), NULL, 0),
    CStringTypeDescriptorField("", offsetof(QuestStepObjective, szParam1_VariableArray), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStepObjective, serParam2), NULL, 0),
    CStringTypeDescriptorField("", offsetof(QuestStepObjective, szParam2_VariableArray), 0x21, 8),
    BoundedTypeDescriptorField("", DT_INT, offsetof(QuestStepObjective, bHiddenUntilComplete), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStepObjective, dwFlags), NULL, 0x80001),
    FilterTypeDescriptorField("", DT_GBID, offsetof(QuestStepObjective, gbidItemToShow), "", 2, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestStepObjective), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestStepObjective("QuestStepObjective", 3, QuestStepObjective_Fields, 16, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_STEP_OBJECTIVE = &TypeDescriptor_QuestStepObjective;

static const TypeDescriptorField QuestStepObjectiveSet_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(QuestStepObjectiveSet, nNextStepUID), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStepObjectiveSet, serStepObjectives), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_OBJECTIVE, offsetof(QuestStepObjectiveSet, arStepObjectives), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestStepObjectiveSet), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestStepObjectiveSet("QuestStepObjectiveSet", 0, QuestStepObjectiveSet_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_STEP_OBJECTIVE_SET = &TypeDescriptor_QuestStepObjectiveSet;

static const TypeDescriptorField QuestStepBonusObjectiveSet_Fields[8] = {
    TypeDescriptorField("", DT_INT, offsetof(QuestStepBonusObjectiveSet, nRewardXPTier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStepBonusObjectiveSet, nRewardGoldTier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStepBonusObjectiveSet, nRewardDivisor), (const void *)0x1, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(QuestStepBonusObjectiveSet, bRepeatable), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(QuestStepBonusObjectiveSet, bInverse), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStepBonusObjectiveSet, serStepBonusObjectives), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_OBJECTIVE, offsetof(QuestStepBonusObjectiveSet, arStepBonusObjectives), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestStepBonusObjectiveSet), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestStepBonusObjectiveSet("QuestStepBonusObjectiveSet", 1, QuestStepBonusObjectiveSet_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_STEP_BONUS_OBJECTIVE_SET = &TypeDescriptor_QuestStepBonusObjectiveSet;

static const TypeDescriptorField QuestStepFailureCondition_Fields[12] = {
    EnumTypeDescriptorField("", offsetof(QuestStepFailureCondition, eFailureConditionType), NULL, gQuestStepFailureConditionTypes),
    TypeDescriptorField("", DT_INT, offsetof(QuestStepFailureCondition, nParam1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStepFailureCondition, nParam2), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(QuestStepFailureCondition, snoname1), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(QuestStepFailureCondition, snoname2), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(QuestStepFailureCondition, gbidParam1), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(QuestStepFailureCondition, gbidParam2), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStepFailureCondition, serParam1), NULL, 0),
    CStringTypeDescriptorField("", offsetof(QuestStepFailureCondition, szParam1_VariableArray), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStepFailureCondition, serParam2), NULL, 0),
    CStringTypeDescriptorField("", offsetof(QuestStepFailureCondition, szParam2_VariableArray), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestStepFailureCondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestStepFailureCondition("QuestStepFailureCondition", 1, QuestStepFailureCondition_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_STEP_FAILURE_CONDITION = &TypeDescriptor_QuestStepFailureCondition;

static const TypeDescriptorField QuestStepFailureConditionSet_Fields[3] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStepFailureConditionSet, serStepFailureConditions), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_FAILURE_CONDITION, offsetof(QuestStepFailureConditionSet, arStepFailureConditions), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestStepFailureConditionSet), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestStepFailureConditionSet("QuestStepFailureConditionSet", 0, QuestStepFailureConditionSet_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_STEP_FAILURE_CONDITION_SET = &TypeDescriptor_QuestStepFailureConditionSet;

static const TypeDescriptorField QuestUnassignedStep_Fields[7] = {
    TypeDescriptorField("", DT_INT, offsetof(QuestUnassignedStep, nUID_deprecated), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestUnassignedStep, dwFlags), (const void *)0x1, 0x80001),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestUnassignedStep, serStepObjectiveSets), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_OBJECTIVE_SET, offsetof(QuestUnassignedStep, arStepObjectiveSets), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestUnassignedStep, serStepFailureConditionSets), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_FAILURE_CONDITION_SET, offsetof(QuestUnassignedStep, arStepFailureConditionSets), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestUnassignedStep), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestUnassignedStep("QuestUnassignedStep", 1, QuestUnassignedStep_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_UNASSIGNED_STEP = &TypeDescriptor_QuestUnassignedStep;

static const TypeDescriptorField QuestStep_Fields[23] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStep, serStepName), NULL, 0),
    CStringTypeDescriptorField("", offsetof(QuestStep, szStepName_VariableArray), 0x21, 8),
    TypeDescriptorField("", DT_INT, offsetof(QuestStep, nUID), NULL, 1),
    EnumTypeDescriptorField("", offsetof(QuestStep, eRewardType), NULL, gQuestRewardTypes),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(QuestStep, snoRewardRecipe), NULL, 0, 7, 0x31, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestStep, snoRewardTreasureClass), "", 0x34, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStep, nRewardXPTier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStep, nRewardGoldTier), NULL, 1),
    EnumTypeDescriptorField("", offsetof(QuestStep, eReplayRewardType), NULL, gQuestRewardTypes),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(QuestStep, snoReplayRewardRecipe), NULL, 0, 7, 0x31, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestStep, snoReplayRewardTreasureClass), "", 0x34, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStep, nReplayRewardXPTier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestStep, nReplayRewardGoldTier), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStep, serStepObjectiveSets), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_OBJECTIVE_SET, offsetof(QuestStep, arStepObjectiveSets), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStep, serStepBonusObjectiveSets), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_BONUS_OBJECTIVE_SET, offsetof(QuestStep, arStepBonusObjectiveSets), 0x21, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestStep, serStepFailureConditionSets), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP_FAILURE_CONDITION_SET, offsetof(QuestStep, arStepFailureConditionSets), 0x21, 8),
    TypeDescriptorField("", DT_INT, offsetof(QuestStep, dwFlags), NULL, 0x80001),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestStep, snoPowerGranted), "", 0x1d, 0, -1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(QuestStep, snoWaypointLevelAreas), NULL, 0, 2, 0x16, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestStep), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestStep("QuestStep", 6, QuestStep_Fields, 23, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_STEP = &TypeDescriptor_QuestStep;

static const TypeDescriptorField QuestCompletionStep_Fields[5] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestCompletionStep, serStepName), NULL, 0),
    CStringTypeDescriptorField("", offsetof(QuestCompletionStep, szStepName_VariableArray), 0x21, 8),
    TypeDescriptorField("", DT_INT, offsetof(QuestCompletionStep, nUID), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(QuestCompletionStep, bSharedLogText), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestCompletionStep), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestCompletionStep("QuestCompletionStep", 1, QuestCompletionStep_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_COMPLETION_STEP = &TypeDescriptor_QuestCompletionStep;

static const TypeDescriptorField QuestLevelRange_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(QuestLevelRange, nMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestLevelRange, nMax), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestLevelRange), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestLevelRange("QuestLevelRange", 0, QuestLevelRange_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_LEVEL_RANGE = &TypeDescriptor_QuestLevelRange;

static const TypeDescriptorField BountyData_Fields[24] = {
    EnumTypeDescriptorField("", offsetof(BountyData, eAct), (const void *)-1, gActs),
    EnumTypeDescriptorField("", offsetof(BountyData, eBountyType), (const void *)-1, gBountyTypes),
    TypeDescriptorField("", DT_INT, offsetof(BountyData, nRewardTier), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoLevelArea), "", 0x16, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoWorld), "", 0x30, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredQuest), "", 0x1f, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredQuestWorld), "", 0x30, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredActor), "", 1, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredActorWorld), "", 0x30, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredActorLevelArea), "", 0x16, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredScene), "", 0x21, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredSceneWorld), "", 0x30, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredAdventure), "", 2, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredAdventureWorld), "", 0x30, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredAdventureLevelArea), "", 0x16, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredEncounter), "", 0xf, 0, -1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(BountyData, gbidRequiredLabel), "", 0xb, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredScene2), "", 0x21, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoRequiredSceneWorld2), "", 0x30, 0, -1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(BountyData, gbidRequiredLabel2), "", 0xb, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoGlobalConductor), "", 0x36, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BountyData, snoBossEncounter), "", 0x40, 0, -1),
    TypeDescriptorField("", DT_FLOAT, offsetof(BountyData, flAutoPingTime), (const void *)0x42700000, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BountyData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BountyData("BountyData", 0, BountyData_Fields, 24, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BOUNTY_DATA = &TypeDescriptor_BountyData;

static const TypeDescriptorField QuestDefinition_Fields[17] = {
    EnumTypeDescriptorField("", offsetof(QuestDefinition, eQuestType), NULL, gQuestTypes),
    TypeDescriptorField("", DT_INT, offsetof(QuestDefinition, nQuestUID), NULL, 1),
    TypeDescriptorField("", DT_QUEST_UNASSIGNED_STEP, offsetof(QuestDefinition, tQuestUnassignedStep), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestDefinition, serQuestSteps), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_STEP, offsetof(QuestDefinition, arQuestSteps), 0x21, 8),
    VariableArrayCountTypeDescriptorField("", offsetof(QuestDefinition, nNumQuestSteps), 0x10000, DT_QUEST_STEP, 0x48),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(QuestDefinition, serQuestCompletionSteps), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_QUEST_COMPLETION_STEP, offsetof(QuestDefinition, arQuestCompletionSteps), 0x21, 8),
    VariableArrayCountTypeDescriptorField("", offsetof(QuestDefinition, nNumQuestCompletionSteps), 0x10000, DT_QUEST_COMPLETION_STEP, 0x54),
    TypeDescriptorField("", DT_INT, offsetof(QuestDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(QuestDefinition, hZoneLabel), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestDefinition, eAct), (const void *)-1, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(QuestDefinition, snoAdditionalLevelAreas), NULL, 0, 0x12, 0x16, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestDefinition, snoChallengeWorld), "", 0x30, 0, -1),
    EnumTypeDescriptorField("", offsetof(QuestDefinition, eChallengeType), NULL, gChallengeTypes),
    TypeDescriptorField("", DT_BOUNTY_DATA, offsetof(QuestDefinition, tBountyData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestDefinition("Quest", 31, QuestDefinition_Fields, 17, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST = &TypeDescriptor_QuestDefinition;


QuestGroupDef * gptGroupDefQuest;  // line 827

// line 833
QuestGroupDef::QuestGroupDef()
    : GroupDefinition("Quest", QUEST_SNO,
                      SNOVersions(135, 137, 135, 135, 136),
                      SNODATATYPE_TITLE, sizeof(QuestDefinition), DT_QUEST, 0x8005, 14)
{
}

// line 854
uint32 QuestGroupDef::GetMaxCount()
{
    return 80;
}

// line 866
void QuestGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    QuestDefinition * pdef = (QuestDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(QuestDefinition));

    pdef->arQuestSteps = NULL;
    pdef->serQuestSteps.dwSize = 0;

    pdef->hZoneLabel = (uint32)-1;


    pdef->tQuestUnassignedStep.dwFlags = 0;
}

// Original data at 0x71010601c0-0x7101060220 (tools/data.py --ptr).
LPCSTR gszQuestStringListPrefix = "Quest_";  // line 1499
LPCSTR gszQuestNameLabel = "Name";
LPCSTR gszQuestDescLabel = "Description";
LPCSTR gszQuestStepSplashLabel = "Step%dSplash";
LPCSTR gszQuestStepMeterLabel = "Step%dMeter";
LPCSTR gszQuestStepDescLabel = "Step%dDesc_%s";
LPCSTR gszQuestStepHeaderLabel = "Step%dHeader";
LPCSTR gszQuestObjectiveLabel = "Objective%dDesc_%s";
LPCSTR gszQuestObjectiveLabelOld = "Objective%dDesc";
LPCSTR gszQuestStepBnetTextLabel = "Step%dBNetText";
LPCSTR gszQuestStepBnetTitleLabel = "Step%dBNetTitle";
LPCSTR gszUnassignedQuestStepBnetTextLabel = "UnassignedStepBNetText";
LPCSTR gszUnassignedQuestStepBnetTitleLabel = "UnassignedStepBNetTitle";  // line 1511

// Helpers defined later in this original source file.
int32 QuestStepIndexToUID(const QuestDefinition * ptQuestDef, uint32 uIndex);
uint32 QuestCompletionStepUIDToIndex(const QuestDefinition * ptQuestDef, int32 nCompletionStepUID);

int32 QuestGetNumSteps(SNO snoQuest) {  // line 1514
    if (snoQuest == SNO_NONE)
        return 0;
    ScopedQuestDefinitionLockR ptQuest(snoQuest);
    return ptQuest->nNumQuestSteps;
}

CRefString QuestGetName(SNO snoQuest) {  // line 1528
    return SNOToString(QUEST_SNO, snoQuest, FALSE);
}

inline CRefString QuestGetStringListName(SNO snoQuest) {  // line 1551
    CRefString sQuest = QuestGetName(snoQuest);
    CRefString sStringList(gszQuestStringListPrefix);
    sStringList += sQuest;
    return sStringList;
}

SNO QuestGetStringList(SNO snoQuest) {  // line 1563
    CRefString sStringList = QuestGetStringListName(snoQuest);
    return SNOFileLookupGUID(STRINGLIST_SNO, sStringList);
}

CRefString QuestGetStepDescriptionLabel(const QuestDefinition * ptQuestDef, int32 nStepUID, PlayerClass ePlayerClass) {  // line 1572
    BOOL bSharedLogText = FALSE;
    uint32 nStepIndex = QuestStepUIDToIndex(ptQuestDef, nStepUID);
    if (nStepIndex != uint32(-1)) {
        while (nStepIndex > 0) {
            if (!(ptQuestDef->arQuestSteps[nStepIndex].dwFlags &
                  ((1 << QSF_ORPHANED_STEP) | (1 << QSF_USE_PREVIOUS_LOG_TEXT))))
                break;
            --nStepIndex;
        }
        nStepUID = QuestStepIndexToUID(ptQuestDef, nStepIndex);
        bSharedLogText = (ptQuestDef->arQuestSteps[nStepIndex].dwFlags & (1 << QSF_SHARED_LOG_TEXT)) != 0;
    } else {
        nStepIndex = QuestCompletionStepUIDToIndex(ptQuestDef, nStepUID);
        if (nStepIndex != uint32(-1))
            bSharedLogText = ptQuestDef->arQuestCompletionSteps[nStepIndex].bSharedLogText;
    }
    CRefString sLabel;
    sLabel.Format(gszQuestStepDescLabel, nStepUID,
                  PlayerGetShortClassName(bSharedLogText ? PLAYER_CLASS_BARBARIAN : ePlayerClass));
    return sLabel;
}

CRefString QuestGetStepHeaderLabel(const QuestDefinition * ptQuestDef, int32 nStepUID) {  // line 1653
    CRefString sLabel;
    sLabel.Format(gszQuestStepHeaderLabel, nStepUID);
    return sLabel;
}

CRefString QuestGetStepSplashLabel(const QuestDefinition * ptQuestDef, int32 nStepUID) {  // line 1703
    CRefString sLabel;
    sLabel.Format(gszQuestStepSplashLabel, nStepUID);
    return sLabel;
}

CRefString QuestGetStepMeterLabel(const QuestDefinition * ptQuestDef, int32 nStepUID) {  // line 1714
    CRefString sLabel;
    sLabel.Format(gszQuestStepMeterLabel, nStepUID);
    return sLabel;
}

CRefString QuestGetObjectiveDescriptionLabel(const QuestStepObjective & tObjective, PlayerClass ePlayerClass) {  // line 1725
    // The objective's shared-text flag is bit 4 in the original instructions.
    if (tObjective.dwFlags & 0x10)
        ePlayerClass = PLAYER_CLASS_BARBARIAN;
    CRefString sLabel;
    if (ePlayerClass == PLAYER_CLASS_BARBARIAN)
        sLabel.Format(gszQuestObjectiveLabelOld, tObjective.nUID);
    else
        sLabel.Format(gszQuestObjectiveLabel, tObjective.nUID, PlayerGetShortClassName(ePlayerClass));
    return sLabel;
}

uint32 QuestStepUIDToIndex(const QuestDefinition * ptQuestDef, int32 nStepUID) {  // line 1761
    uint32 uIndex = uint32(-1);
    if (nStepUID >= 0) {
        for (int32 i = 0; i < ptQuestDef->nNumQuestSteps; ++i) {
            QuestStep * ptQuestStep = &ptQuestDef->arQuestSteps[i];
            if (ptQuestStep->nUID == nStepUID) {
                uIndex = i;
                break;
            }
        }
    }
    return uIndex;
}

inline int32 QuestStepIndexToUID(const QuestDefinition * ptQuestDef, uint32 uIndex) {  // line 1785
    return ptQuestDef->arQuestSteps[uIndex].nUID;
}

inline uint32 QuestCompletionStepUIDToIndex(const QuestDefinition * ptQuestDef, int32 nCompletionStepUID) {  // line 1793
    uint32 uIndex = uint32(-1);
    if (nCompletionStepUID >= 0) {
        for (int32 i = 0; i < ptQuestDef->nNumQuestCompletionSteps; ++i) {
            QuestCompletionStep * ptQuestCompletionStep = &ptQuestDef->arQuestCompletionSteps[i];
            if (ptQuestCompletionStep->nUID == nCompletionStepUID) {
                uIndex = i;
                break;
            }
        }
    }
    return uIndex;
}

BOOL QuestStepIsCompletionStep(SNO snoQuest, int32 nStepUID) {  // line 1924
    ScopedQuestDefinitionLockR ptQuestDef(snoQuest);
    return QuestStepIsCompletionStep(ptQuestDef, nStepUID);
}

BOOL QuestStepIsCompletionStep(const QuestDefinition * ptQuestDef, int32 nStepUID) {  // line 1933
    if (nStepUID == QUEST_UNASSIGNED_STEP_UID)
        return FALSE;
    if (nStepUID == QUEST_ANY_COMPLETION_STEP_UID)
        return TRUE;
    for (int32 i = 0; i < ptQuestDef->nNumQuestCompletionSteps; ++i) {
        QuestCompletionStep * ptQuestCompletionStep = &ptQuestDef->arQuestCompletionSteps[i];
        if (ptQuestCompletionStep->nUID == nStepUID)
            return TRUE;
    }
    return FALSE;
}

QuestType QuestGetQuestType(SNO snoQuest) {  // line 1961
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    return pdefQuest->eQuestType;
}

Act QuestGetBountyAct(SNO snoQuest) {  // line 1969
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    return pdefQuest->tBountyData.eAct;
}

Act QuestGetAct(SNO snoQuest) {  // line 1977
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    return pdefQuest->eAct;
}

#include <string.h>
#include "ActDefinition.h"
#include "../../Client/ImageCollection.h"
// Original GameCommonData.cpp:415 and Quests declarations.
SNO ActSNOFromActEnum(const Act eAct);
BOOL QuestStepIsReachableByStep(const QuestDefinition*, int32, int32);

#line 884 "X/Common/Definitions/QuestDefinition.cpp"
void QuestStepObjective::GenerateVariableArray()
{
    SigmaMemoryFree(szParam1_VariableArray, NULL);
    szParam1_VariableArray = NULL;
    SigmaMemoryFree(szParam2_VariableArray, NULL);
    szParam2_VariableArray = NULL;
    serParam1.dwSize = strlen(szParam1_VariableArray) + 1;
    szParam1_VariableArray = static_cast<char*>(SigmaMemoryNew(serParam1.dwSize, 4, NULL, FALSE));
    SigmaMemoryCopy(szParam1_VariableArray, szParam1_VariableArray, serParam1.dwSize);
    if (szParam2_VariableArray[0]) {
        serParam2.dwSize = strlen(szParam2_VariableArray) + 1;
        szParam2_VariableArray = static_cast<char*>(SigmaMemoryNew(serParam2.dwSize, 4, NULL, FALSE));
        SigmaMemoryCopy(szParam2_VariableArray, szParam2_VariableArray, serParam2.dwSize);
    }
    else {
        serParam2.dwSize = 0;
        szParam2_VariableArray = NULL;
    }
}

#line 939 "X/Common/Definitions/QuestDefinition.cpp"
void QuestStepFailureCondition::GenerateVariableArray()
{
    SigmaMemoryFree(szParam1_VariableArray, NULL);
    szParam1_VariableArray = NULL;
    SigmaMemoryFree(szParam2_VariableArray, NULL);
    szParam2_VariableArray = NULL;
    serParam1.dwSize = strlen(szParam1_VariableArray) + 1;
    szParam1_VariableArray = static_cast<char*>(SigmaMemoryNew(serParam1.dwSize, 4, NULL, FALSE));
    SigmaMemoryCopy(szParam1_VariableArray, szParam1_VariableArray, serParam1.dwSize);
    if (szParam2_VariableArray[0]) {
        serParam2.dwSize = strlen(szParam2_VariableArray) + 1;
        szParam2_VariableArray = static_cast<char*>(SigmaMemoryNew(serParam2.dwSize, 4, NULL, FALSE));
        SigmaMemoryCopy(szParam2_VariableArray, szParam2_VariableArray, serParam2.dwSize);
    }
    else {
        serParam2.dwSize = 0;
        szParam2_VariableArray = NULL;
    }
}

#line 1072 "X/Common/Definitions/QuestDefinition.cpp"
BOOL QuestGroupDef::Write(void* lpDefinition, FileReference& tRef) const
{
    QuestDefinition* pdefQuest = static_cast<QuestDefinition*>(lpDefinition);
    for (int32 i = 0; i < pdefQuest->nNumQuestSteps; ++i) {
        QuestStep& tQuestStep = pdefQuest->arQuestSteps[i];
        BOOL bOrphaned = !QuestStepIsReachableByStep(pdefQuest, QUEST_UNASSIGNED_STEP_UID, tQuestStep.nUID);
        if (bOrphaned) tQuestStep.dwFlags |= 1 << QSF_ORPHANED_STEP;
        else tQuestStep.dwFlags &= ~(1 << QSF_ORPHANED_STEP);
    }
    pdefQuest->GenerateVariableArray();
    return GroupDefinition::Write(lpDefinition, tRef);
}
#line 1985 "X/Common/Definitions/QuestDefinition.cpp"
void QuestGetIcon(const Act eAct, const SNO snoQuest, const BOOL fCompleted, ImageTextureFrame& tFrame)
{
    tFrame = cgtImageTextureFrameNull;
    SNO snoAct = ActSNOFromActEnum(eAct);
    int32 nQuest = 0;
    ScopedActDefinitionLockR ptDefAct(snoAct);
    for (int32 i = 0; i < ActGetNumQuests(ptDefAct); ++i) {
        SNO snoCurrentQuest = ptDefAct->arQuests[i].snoQuest;
        if (snoCurrentQuest == SNO_NONE && QuestGetQuestType(snoCurrentQuest) != QUEST_TYPE_MAIN_QUEST) continue;
        if (snoCurrentQuest == snoQuest) {
            int32 nActNumber = 0;
            switch (eAct) {
            case ACT_1: nActNumber = 1; break;
            case ACT_2: nActNumber = 2; break;
            case ACT_3: nActNumber = 3; break;
            case ACT_4: nActNumber = 4; break;
            case ACT_5: nActNumber = 5; break;
            }
            CRefString sFrame = CRefString().Format("BattleNetQuestIcons_Act_%d_Quest_%d_", nActNumber, nQuest + 1);
            sFrame += fCompleted ? "Completed" : "Incomplete";
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, sFrame, tFrame);
            return;
        }
        ++nQuest;
    }
}

#line 917 "X/Common/Definitions/QuestDefinition.cpp"
void QuestStepObjectiveSet::GenerateVariableArray()
{
    for (uint32 i = 0; i < serStepObjectives.dwSize / sizeof(QuestStepObjective); ++i)
    {
        arStepObjectives[i].GenerateVariableArray();
    }
}

#line 928 "X/Common/Definitions/QuestDefinition.cpp"
// Original concrete copy is linker-removed (DWARF low_pc 0); loop shape follows QuestStepObjectiveSet (917).
void QuestStepBonusObjectiveSet::GenerateVariableArray()
{
    for (uint32 i = 0; i < serStepBonusObjectives.dwSize / sizeof(QuestStepObjective); ++i)
    {
        arStepBonusObjectives[i].GenerateVariableArray();
    }
}

#line 972 "X/Common/Definitions/QuestDefinition.cpp"
void QuestStepFailureConditionSet::GenerateVariableArray()
{
    for (uint32 i = 0; i < serStepFailureConditions.dwSize / sizeof(QuestStepFailureCondition); ++i)
    {
        arStepFailureConditions[i].GenerateVariableArray();
    }
}

#line 983 "X/Common/Definitions/QuestDefinition.cpp"
void QuestUnassignedStep::GenerateVariableArray()
{
    for (uint32 i = 0; i < serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet); ++i)
    {
        arStepObjectiveSets[i].GenerateVariableArray();
    }

    for (uint32 i = 0; i < serStepFailureConditionSets.dwSize / sizeof(QuestStepFailureConditionSet); ++i)
    {
        arStepFailureConditionSets[i].GenerateVariableArray();
    }
}

#line 999 "X/Common/Definitions/QuestDefinition.cpp"
void QuestStep::GenerateVariableArray()
{
    SigmaMemoryFree(szStepName_VariableArray, NULL); szStepName_VariableArray = NULL;
    // Inference: the original traps right after the NULL store (0x63d588), so the string is read through
    // while NULL; written like the szParam2 copy of QuestStepObjective (884). Nothing after it survives.
    if (szStepName_VariableArray[0]) {
        serStepName.dwSize = strlen(szStepName_VariableArray) + 1;
        szStepName_VariableArray = static_cast<char*>(SigmaMemoryNew(serStepName.dwSize, 4, NULL, FALSE));
        SigmaMemoryCopy(szStepName_VariableArray, szStepName_VariableArray, serStepName.dwSize);
    }
    else {
        serStepName.dwSize = 0;
        szStepName_VariableArray = NULL;
    }
#line 1015 "X/Common/Definitions/QuestDefinition.cpp"
    for (uint32 i = 0; i < serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet); ++i)
    {
        arStepObjectiveSets[i].GenerateVariableArray();
    }
#line 1020 "X/Common/Definitions/QuestDefinition.cpp"
    for (uint32 i = 0; i < serStepBonusObjectiveSets.dwSize / sizeof(QuestStepBonusObjectiveSet); ++i)
    {
        arStepBonusObjectiveSets[i].GenerateVariableArray();
    }
#line 1025 "X/Common/Definitions/QuestDefinition.cpp"
    for (uint32 i = 0; i < serStepFailureConditionSets.dwSize / sizeof(QuestStepFailureConditionSet); ++i)
    {
        arStepFailureConditionSets[i].GenerateVariableArray();
    }
}

#line 1034 "X/Common/Definitions/QuestDefinition.cpp"
// Original concrete copy is linker-removed (DWARF low_pc 0); its inlined use in QuestDefinition (1065)
// frees the name and traps like QuestStep (999).
void QuestCompletionStep::GenerateVariableArray()
{
    SigmaMemoryFree(szStepName_VariableArray, NULL); szStepName_VariableArray = NULL;
    if (szStepName_VariableArray[0]) {
        serStepName.dwSize = strlen(szStepName_VariableArray) + 1;
        szStepName_VariableArray = static_cast<char*>(SigmaMemoryNew(serStepName.dwSize, 4, NULL, FALSE));
        SigmaMemoryCopy(szStepName_VariableArray, szStepName_VariableArray, serStepName.dwSize);
    }
    else {
        serStepName.dwSize = 0;
        szStepName_VariableArray = NULL;
    }
}

#line 1054 "X/Common/Definitions/QuestDefinition.cpp"
void QuestDefinition::GenerateVariableArray()
{
    tQuestUnassignedStep.GenerateVariableArray();

    for (int32 i = 0; i < nNumQuestSteps; ++i)
    {
        arQuestSteps[i].GenerateVariableArray();
    }

    for (int32 i = 0; i < nNumQuestCompletionSteps; ++i)
    {
        arQuestCompletionSteps[i].GenerateVariableArray();
    }
}
#include "ConversationDefinition.h"
#include "StringListDefinition.h"
#include "LevelAreaDefinition.h"
#include "../Monsters.h"
#include "../GlobalSNO.h"
#include "../../Sigma/UnicodeText.h"
// Complete ConversationDefinition layout from ConversationDefinition.h DWARF.
typedef int32 ConversationType;
typedef int32 ConversationIcon;
struct ConversationTreeNode;
struct ConversationDefinition {  // size 0x110
    RequiredDefinitionHeader tHeader;  // 0x00
    ConversationType eConvType;  // 0x0c
    ConversationIcon eConvIcon;  // 0x10
    SNO snoConvPiggyback;  // 0x14
    SNO snoConvUnlocks[3];  // 0x18
    uint32 dwFlags;  // 0x24
    CHAR szSetPlayerFlag[128];  // 0x28
    SNO snoPrimaryNpc;  // 0xa8
    SNO snoAltNpc1;  // 0xac
    SNO snoAltNpc2;  // 0xb0
    SNO snoAltNpc3;  // 0xb4
    SNO snoAltNpc4;  // 0xb8
    uint32 dwLineUID;  // 0xbc
    XPtr64<ConversationTreeNode> arRootTreeNodes;  // 0xc0
    SerializeData serRootTreeNodes;  // 0xc8
    XPtr64<unsigned char> pCompiledScript;  // 0xd0
    SerializeData serCompiledScript;  // 0xd8
    SNO snoScript;  // 0xe0
    BOOL fSpeakerUsed[9];  // 0xe4
    SNO snoBossEncounter;  // 0x108
    uint32 dwPad;  // 0x10c
};
static_assert(sizeof(ConversationDefinition) == 0x110, "ConversationDefinition");


typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;
// ConversationDefinition.h:462-463; original caller has LockR/UnlockR, unlike fast-lock types.
template <> inline const ConversationDefinition* DefinitionLockR<ConversationDefinition>(SNO sno)
{
    return static_cast<const ConversationDefinition*>(gptGroupDefConversation->LockR(sno, FALSE));
}
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition* pdef)
{
    // Inferred local name; DWARF UnlockR takes mutable void*& while pdef is const.
    void* lpDef = const_cast<ConversationDefinition*>(pdef);
    gptGroupDefConversation->UnlockR(lpDef);
}
extern const SymbolicEntry gMonsterFamily[];
BOOL SymbolIsValid(int32 nSymbol, const SymbolicEntry* pTable);
#line 2036 "X/Common/Definitions/QuestDefinition.cpp"
static CRefString sMonsterFamilyGetName(MonsterFamily eFamily)
{
    CRefString sFamily;
    if (SymbolIsValid(eFamily, gMonsterFamily)) {
        LPCSTR szLabel = SymbolToName(eFamily, gMonsterFamily);
        sFamily = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTER_FAMILIES), szLabel);
    }
    return sFamily;
}
#line 2050
// Line 2097: the KillLastMonstersIn branch comes first (rows 2099-2101 before 2106-2107).
CRefString QuestGetDynamicObjectiveDescription(
    const QuestStepObjective& tObjective,
    int32 nState)
{
    SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_OPENWORLD_QUEST_OBJECTIVES);

    switch (tObjective.eObjectiveType)
    {
    case OBJECTIVE_KILL_MONSTER_FROM_FAMILY:
        {
            CRefString sDescription = LocalizedSprintf(StringListFetch(snoStringList, "KillMonsterFromFamily"),
                sMonsterFamilyGetName(tObjective.nParam1).str(), NULL);
            return sDescription;
        }

    case OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA:
        {

            CRefString sDescription;
            if (tObjective.nParam2 <= 1)
            {
                sDescription = LocalizedSprintf(StringListFetch(snoStringList, "KillElitePack"),
                    LevelAreaGetLocalizedName(tObjective.snoname1.snoHandle).str(), NULL);
            }
            else
            {

                sDescription = LocalizedSprintf(StringListFetch(snoStringList, "KillMultipleElitePacks"),
                    UnicodeText::FormatNumber(tObjective.nParam2, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                    LevelAreaGetLocalizedName(tObjective.snoname1.snoHandle).str(), NULL);
            }
            return sDescription;
        }

    case OBJECTIVE_KILL_ANY_MONSTER_IN_LEVEL_AREA:
        {

            CRefString sDescription = LocalizedSprintf(StringListFetch(snoStringList, "KillAnyMonster"),
                UnicodeText::FormatNumber(tObjective.nParam2, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                LevelAreaGetLocalizedName(tObjective.snoname1.snoHandle).str(), NULL);
            return sDescription;
        }

    case OBJECTIVE_KILL_ALL_MONSTERS_IN_WORLD:
        {
            CRefString sDescription;
            if (nState != 0 && nState != -1 && nState != 0x7fffffff)
            {
                sDescription = LocalizedSprintf(StringListFetch(snoStringList, "KillLastMonstersIn"),
                    UnicodeText::FormatNumber(nState, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                    LevelAreaGetLocalizedName(tObjective.snoname2.snoHandle).str(), NULL);
            }
            else
            {

                sDescription = LocalizedSprintf(StringListFetch(snoStringList, "KillAllMonstersIn"),
                    LevelAreaGetLocalizedName(tObjective.snoname2.snoHandle).str(), NULL);
            }

            return sDescription;
        }

    case OBJECTIVE_HAD_CONVERSATION:
        {
            CRefString sName("NPC");
            ScopedConversationDefinitionLockR pdefConv(tObjective.snoname1.snoHandle);
            if (pdefConv->snoPrimaryNpc != SNO_NONE)
            {
                SNOName snoNameNPC(ACTOR_SNO, pdefConv->snoPrimaryNpc);
                MonsterGetBaseName(snoNameNPC, sName);
            }

            CRefString sDescription = LocalizedSprintf(StringListFetch(snoStringList, "HadConversation"),
                sName.str(), NULL);

            return sDescription;

        }
    default:
        return CRefString("dynamic objective type missing description text");
    }
}
