// Original file: X/Server/Definitions/AIDefinition.cpp

#include "AIDefinition.h"

// For SNONAME_NONE and cgplaceNull, whose initializers the original runs in this file's static
// initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp and X/Common/SNO/SymbolicValue.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_SNO; extern const XTypeDescriptor * DT_TIME_SHORT;
extern const SymbolicEntry gAIStateConditionTypes[81]; extern const SymbolicEntry gAIStateTargetTypes[48]; extern const SymbolicEntry gAIStateTypes[10]; extern const SymbolicEntry gAIStateCooldowns[10];

enum {
    AISTATE_XML_VERSION = 5,
    AISTATE_SNO_COMMON_VERSION = 33,
    AISTATE_SNO_PC_VERSION = 33,
    AISTATE_SNO_CONSOLE_VERSION = 34,
    AISTATE_SNO_AXE_VERSION = 34,
    AIBEHAVIOR_SNO_COMMON_VERSION = 19,
    AIBEHAVIOR_XML_VERSION = 2,
    AIBEHAVIOR_SNO_PC_VERSION = 19,
    AIBEHAVIOR_SNO_CONSOLE_VERSION = 20,
    AIBEHAVIOR_SNO_AXE_VERSION = 20,
};



















static const TypeDescriptorField AICondition_Fields[5] = {
    EnumTypeDescriptorField("", offsetof(AICondition, eCondition), NULL, gAIStateConditionTypes), EnumTypeDescriptorField("", offsetof(AICondition, eConditionTargetType), NULL, gAIStateTargetTypes),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(AICondition, flConditionParm), NULL, 1, AISTATE_MAX_CONDITION_PARAMETERS, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(AICondition, snoPower), "", POWER_SNO, 0, -1), TypeDescriptorField(NULL, DT_NULL, sizeof(AICondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AICondition("AICondition", 0, AICondition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AI_CONDITION_DEFINITION = &TypeDescriptor_AICondition;

static const TypeDescriptorField AIEntry_Fields[13] = {
    TypeDescriptorField("", DT_INT, offsetof(AIEntry, dwPriority), NULL, 1),
    TypeDescriptorField("", DT_TIME_SHORT, offsetof(AIEntry, dwCoolUpMin), NULL, 1),
    TypeDescriptorField("", DT_TIME_SHORT, offsetof(AIEntry, dwCoolUpDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AIEntry, dwWeight), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_AI_CONDITION_DEFINITION, offsetof(AIEntry, arCondition), NULL, 1, AISTATE_MAX_CONDITIONS_PER_ENTRY, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(AIEntry, snoPower), "", POWER_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(AIEntry, gbidMovementStyle), "", 10, 0, -1),
    EnumTypeDescriptorField("", offsetof(AIEntry, ePowerTargetType), NULL, gAIStateTargetTypes),
    TypeDescriptorField("", DT_TIME_SHORT, offsetof(AIEntry, dwCoolDownMin), NULL, 1),
    TypeDescriptorField("", DT_TIME_SHORT, offsetof(AIEntry, dwCoolDownDelta), NULL, 1),
    EnumTypeDescriptorField("", offsetof(AIEntry, eCoolDown), (const void *)-1, gAIStateCooldowns), EnumTypeDescriptorField("", offsetof(AIEntry, eStateSwitch), NULL, gAIStateTypes), TypeDescriptorField(NULL, DT_NULL, sizeof(AIEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AIEntry("AIEntry", 0, AIEntry_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AI_ENTRY_DEFINITION = &TypeDescriptor_AIEntry;

static const TypeDescriptorField AIDecision_Fields[4] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AIDecision, serEntries), NULL, 0), VariableArrayTypeDescriptorField("", DT_AI_ENTRY_DEFINITION, offsetof(AIDecision, arEntries), 0x21, (int32)offsetof(AIDecision, serEntries) - (int32)offsetof(AIDecision, arEntries)), VariableArrayCountTypeDescriptorField("", offsetof(AIDecision, nEntryCount), 0x10000, DT_AI_ENTRY_DEFINITION, (int32)offsetof(AIDecision, serEntries) - (int32)offsetof(AIDecision, nEntryCount)), TypeDescriptorField(NULL, DT_NULL, sizeof(AIDecision), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AIDecision("AIDecision", 0, AIDecision_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AI_DECISION_DEFINITION = &TypeDescriptor_AIDecision;

static const TypeDescriptorField AIStateDefinition_Fields[2] = {
    FixedArrayTypeDescriptorField("", DT_AI_DECISION_DEFINITION, offsetof(AIStateDefinition, arDecision), NULL, 1, 2, 0), TypeDescriptorField(NULL, DT_NULL, sizeof(AIStateDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AIStateDefinition("AIState", AISTATE_XML_VERSION, AIStateDefinition_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AI_STATE_DEFINITION = &TypeDescriptor_AIStateDefinition;

static const TypeDescriptorField AIBehaviorDefinition_Fields[3] = {
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(AIBehaviorDefinition, arAIState), NULL, 0, 8, AISTATE_SNO, 0),
    TypeDescriptorField("", DT_INT, offsetof(AIBehaviorDefinition, dwPad), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(AIBehaviorDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AIBehaviorDefinition("AIBehavior", AIBEHAVIOR_XML_VERSION, AIBehaviorDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AI_BEHAVIOR_DEFINITION = &TypeDescriptor_AIBehaviorDefinition;




AIBehaviorGroupDef * gptGroupDefAIBehavior;
AIStateGroupDef * gptGroupDefAIState;




AIBehaviorGroupDef::AIBehaviorGroupDef()
    : GroupDefinition("AIBehavior", AIBEHAVIOR_SNO,
                      SNOVersions(AIBEHAVIOR_SNO_PC_VERSION,
                                  AIBEHAVIOR_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  AIBEHAVIOR_SNO_CONSOLE_VERSION,
                                  AIBEHAVIOR_SNO_CONSOLE_VERSION,
                                  AIBEHAVIOR_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(AIBehaviorDefinition), DT_AI_BEHAVIOR_DEFINITION, 0x8005, 0xe) {
}









AIStateGroupDef::AIStateGroupDef()
    : GroupDefinition("AIState", AISTATE_SNO,
                      SNOVersions(AISTATE_SNO_PC_VERSION,
                                  AISTATE_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  AISTATE_SNO_CONSOLE_VERSION,
                                  AISTATE_SNO_CONSOLE_VERSION,
                                  AISTATE_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(AIStateDefinition), DT_AI_STATE_DEFINITION, 0x8241, 0x500e) {
}
















uint32 AIBehaviorGroupDef::GetMaxCount() {
    return 1024;
}


uint32 AIStateGroupDef::GetMaxCount() {
    return 1024;
}
