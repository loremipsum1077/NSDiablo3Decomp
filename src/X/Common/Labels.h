// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Labels.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "GameBalance.h"
#include "../Sigma/DataStructures/XPtr64.h"
#include "../Sigma/Files/XSerializeData.h"
#include "../Sigma/Unicode.h"

enum {
    LABEL_CONDITION_ALWAYS = 0,
    LABEL_CONDITION_GAME_DIFFICULTY_DEPRECATED = 1,
    LABEL_CONDITION_LABEL_ALREADY_SET = 2,
    LABEL_CONDITION_GAME_IS_OPEN_WORLD = 3,
    LABEL_CONDITION_COUNT = 4,
};

typedef int32 LabelConditionType;

enum {
    LABEL_ENTRY_FLAG_APPLY_TO_ALL_SCENES = 0,
    LABEL_ENTRY_FLAG_APPLY_TO_FARTHEST = 1,
    LABEL_ENTRY_FLAG_MUST_SPAWN = 2,
    LABEL_ENTRY_FLAG_EXCLUDE_SCENES_ALREADY_CHOSEN = 3,
    LABEL_ENTRY_FLAG_COUNT = 4,
};

enum {
    LABEL_CONDITION_FLAG_NOT = 0,
    LABEL_CONDITION_FLAG_COUNT = 1,
};

typedef int32 LabelSetInPlace;

struct LabelCondition {  // size 0x18
    LabelConditionType eType;  // 0x00
    uint32 uFlags;  // 0x04
    int32 nConditionParm[4];  // 0x08
    void Initialize();
};
static_assert(sizeof(LabelCondition) == 0x18, "LabelCondition");

struct LabelEntry {  // size 0x14
    GBID gbidLabel;  // 0x00
    uint32 uFlags;  // 0x04
    float flWeight;  // 0x08
    int32 nApplyCountMin;  // 0x0c
    int32 nApplyCountMax;  // 0x10
    void Initialize();
};
static_assert(sizeof(LabelEntry) == 0x14, "LabelEntry");

struct LabelRule {  // size 0xb0
    CHAR szRuleName[128];  // 0x00
    LabelCondition tCondition;  // 0x80
    int32 nNumToChoose;  // 0x98
    int32 nNumEntries;  // 0x9c
    XPtr64<LabelEntry> arEntries;  // 0xa0
    SerializeData serEntries;  // 0xa8
    void Initialize();
};
static_assert(sizeof(LabelRule) == 0xb0, "LabelRule");

struct LabelRuleSet {  // size 0x18
    int32 nNumRules;  // 0x00
    XPtr64<LabelRule> arRules;  // 0x08
    SerializeData serRules;  // 0x10
};
static_assert(sizeof(LabelRuleSet) == 0x18, "LabelRuleSet");

