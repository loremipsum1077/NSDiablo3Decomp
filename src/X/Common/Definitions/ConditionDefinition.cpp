// Original file: X/Common/Definitions/ConditionDefinition.cpp

#include "../../Sigma/SigmaTypes.h"
typedef int32 GBID;  // X/Common/GameBalance.h
#include "ConditionDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../Act.h"
#include "../ActorCommonData.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp
extern const SymbolicEntry gHirelingClass[5];  // X/Common/SNO/SymbolicValue.cpp:5389

// The definition structs below belong to X/Common/Definitions/ConditionDefinition.h, which only
// carries the group definition class so far.
enum {
    MAX_LORE_SUBCONDITIONS = 3,
    MAX_QUEST_SUBCONDITIONS = 3,
    MAX_ITEM_SUBCONDITIONS = 3,
    MAX_LABEL_SUBCONDITIONS = 3,
    MAX_SKILL_SUBCONDITIONS = 3,
    MAX_MONSTER_SUBCONDITIONS = 15,
    MAX_GAMEFLAG_SUBCONDITIONS = 3,
    MAX_PLAYERFLAG_SUBCONDITIONS = 3,
    MAX_BUFF_SUBCONDITIONS = 3,
};

struct LoreSubcondition {  // size 0x8
    SNO snoLore;  // 0x00
    Ternary eLearned;  // 0x04
};
static_assert(sizeof(LoreSubcondition) == 0x8, "LoreSubcondition");

struct QuestSubcondition {  // size 0x10
    SNO snoQuest;  // 0x00
    int32 nStepUID;  // 0x04
    Ternary eCompleted;  // 0x08
    Ternary eAssigned;  // 0x0c
};
static_assert(sizeof(QuestSubcondition) == 0x10, "QuestSubcondition");

struct ItemSubcondition {  // size 0x10
    GBID gbidItem;  // 0x00
    BOOL fPossessed;  // 0x04
    int32 nCountMin;  // 0x08
    int32 nCountMax;  // 0x0c
};
static_assert(sizeof(ItemSubcondition) == 0x10, "ItemSubcondition");

struct LabelSubcondition {  // size 0x8
    GBID gbidLabel;  // 0x00
    BOOL fActive;  // 0x04
};
static_assert(sizeof(LabelSubcondition) == 0x8, "LabelSubcondition");

struct SkillSubcondition {  // size 0xc
    SNO snoSkillPower;  // 0x00
    int32 nLevelMin;  // 0x04
    int32 nLevelMax;  // 0x08
};
static_assert(sizeof(SkillSubcondition) == 0xc, "SkillSubcondition");

struct MonsterSubcondition {  // size 0x4
    SNO snoMonsterActor;  // 0x00
};
static_assert(sizeof(MonsterSubcondition) == 0x4, "MonsterSubcondition");

struct GameFlagSubcondition {  // size 0x80
    CHAR szLabel[128];  // 0x00
};
static_assert(sizeof(GameFlagSubcondition) == 0x80, "GameFlagSubcondition");

struct PlayerFlagSubcondition {  // size 0x80
    CHAR szLabel[128];  // 0x00
};
static_assert(sizeof(PlayerFlagSubcondition) == 0x80, "PlayerFlagSubcondition");

struct FollowerSubcondition {  // size 0x8
    HirelingClass eHirelingClass;  // 0x00
    Ternary eCurrent;  // 0x04
};
static_assert(sizeof(FollowerSubcondition) == 0x8, "FollowerSubcondition");

struct BuffSubcondition {  // size 0xc
    SNO snoBuffPower;  // 0x00
    int32 nIndex;  // 0x04
    int32 nNumStacks;  // 0x08
};
static_assert(sizeof(BuffSubcondition) == 0xc, "BuffSubcondition");

struct ConditionDefinition {  // size 0x4d8
    RequiredDefinitionHeader tHeader;  // 0x00
    BOOL fMale;  // 0x0c
    BOOL fFemale;  // 0x10
    BOOL fPlayerClass[7];  // 0x14
    BOOL fIsStoryMode;  // 0x30
    BOOL fIsAdventureMode;  // 0x34
    BOOL fPlayerIsMaxLevel;  // 0x38
    BOOL fPlayerIsNotMaxLevel;  // 0x3c
    LoreSubcondition tLoreSubconditions[3];  // 0x40
    QuestSubcondition tQuestSubconditions[3];  // 0x58
    Act eQuestRewardAct;  // 0x88
    BOOL bQuestRewardRequireBonusAct;  // 0x8c
    BOOL bQuestRewardRequireAllActsComplete;  // 0x90
    ItemSubcondition tItemSubconditions[3];  // 0x94
    int32 nPlayerLevelMin;  // 0xc4
    int32 nPlayerLevelMax;  // 0xc8
    int32 nLooteeLevelMin;  // 0xcc
    int32 nLooteeLevelMax;  // 0xd0
    int32 nHandicapMin;  // 0xd4
    int32 nHandicapMax;  // 0xd8
    int32 nTieredLootrunMin;  // 0xdc
    int32 nTieredLootrunMax;  // 0xe0
    SNO snoLore;  // 0xe4
    Act eActMin;  // 0xe8
    Act eActMax;  // 0xec
    int32 nPlayersMin;  // 0xf0
    int32 nPlayersMax;  // 0xf4
    SNO snoCurrentWorld;  // 0xf8
    SNO snoCurrentLevelArea;  // 0xfc
    SNO snoQuestRange;  // 0x100
    FollowerSubcondition tFollowerSubcondition;  // 0x104
    LabelSubcondition tLabelSubconditions[3];  // 0x10c
    SkillSubcondition tSkillSubconditions[3];  // 0x124
    BOOL bNormal;  // 0x148
    BOOL bChampion;  // 0x14c
    BOOL bRare;  // 0x150
    BOOL bUnique;  // 0x154
    BOOL bBoss;  // 0x158
    BOOL bGameIsPlatinum;  // 0x15c
    BOOL bPlatinumCapReached;  // 0x160
    int32 nPlatinumCapRemaining;  // 0x164
    BOOL bDoubledGoblinsActive;  // 0x168
    MonsterSubcondition tMonsterSubconditions[15];  // 0x16c
    GameFlagSubcondition tGameFlagSubconditions[3];  // 0x1a8
    PlayerFlagSubcondition tPlayerFlagSubconditions[3];  // 0x328
    BuffSubcondition tBuffSubconditions[3];  // 0x4a8
    int32 nAccountPartitionFlag;  // 0x4cc
    BOOL bLearnedAllRecipesBlacksmith;  // 0x4d0
    BOOL bLearnedAllRecipesJeweler;  // 0x4d4
};
static_assert(sizeof(ConditionDefinition) == 0x4d8, "ConditionDefinition");




// The field names are "" in the binary.
static const TypeDescriptorField QuestSubcondition_Fields[5] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(QuestSubcondition, snoQuest), "", 0x1f, 0x200, -1),
    TypeDescriptorField("", DT_INT, offsetof(QuestSubcondition, nStepUID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestSubcondition, eCompleted), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(QuestSubcondition, eAssigned), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuestSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuestSubcondition("QuestSubcondition", 0, QuestSubcondition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUESTSUBCONDITION = &TypeDescriptor_QuestSubcondition;

static const TypeDescriptorField LoreSubcondition_Fields[3] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(LoreSubcondition, snoLore), "", 0x3b, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(LoreSubcondition, eLearned), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LoreSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LoreSubcondition("LoreSubcondition", 0, LoreSubcondition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LORESUBCONDITION = &TypeDescriptor_LoreSubcondition;

static const TypeDescriptorField ItemSubcondition_Fields[5] = {
    FilterTypeDescriptorField("", DT_GBID, offsetof(ItemSubcondition, gbidItem), "", 2, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSubcondition, fPossessed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSubcondition, nCountMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemSubcondition, nCountMax), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ItemSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ItemSubcondition("ItemSubcondition", 0, ItemSubcondition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ITEMSUBCONDITION = &TypeDescriptor_ItemSubcondition;

static const TypeDescriptorField LabelSubcondition_Fields[3] = {
    FilterTypeDescriptorField("", DT_GBID, offsetof(LabelSubcondition, gbidLabel), "", 0xb, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(LabelSubcondition, fActive), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LabelSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LabelSubcondition("LabelSubcondition", 0, LabelSubcondition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LABELSUBCONDITION = &TypeDescriptor_LabelSubcondition;

static const TypeDescriptorField SkillSubcondition_Fields[4] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(SkillSubcondition, snoSkillPower), "", 0x1d, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SkillSubcondition, nLevelMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SkillSubcondition, nLevelMax), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SkillSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SkillSubcondition("SkillSubcondition", 0, SkillSubcondition_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SKILLSUBCONDITION = &TypeDescriptor_SkillSubcondition;

static const TypeDescriptorField MonsterSubcondition_Fields[2] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterSubcondition, snoMonsterActor), "", 1, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterSubcondition("MonsterSubcondition", 0, MonsterSubcondition_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_MONSTERSUBCONDITION = &TypeDescriptor_MonsterSubcondition;

static const TypeDescriptorField GameFlagSubcondition_Fields[2] = {
    CharArrayTypeDescriptorField("", offsetof(GameFlagSubcondition, szLabel), "", 1, 0x80, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameFlagSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GameFlagSubcondition("GameFlagSubcondition", 0, GameFlagSubcondition_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GAMEFLAGSUBCONDITION = &TypeDescriptor_GameFlagSubcondition;

static const TypeDescriptorField PlayerFlagSubcondition_Fields[2] = {
    CharArrayTypeDescriptorField("", offsetof(PlayerFlagSubcondition, szLabel), "", 1, 0x80, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerFlagSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerFlagSubcondition("PlayerFlagSubcondition", 0, PlayerFlagSubcondition_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYERFLAGSUBCONDITION = &TypeDescriptor_PlayerFlagSubcondition;

static const TypeDescriptorField FollowerSubcondition_Fields[3] = {
    EnumTypeDescriptorField("", offsetof(FollowerSubcondition, eHirelingClass), NULL, gHirelingClass),
    TypeDescriptorField("", DT_INT, offsetof(FollowerSubcondition, eCurrent), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FollowerSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FollowerSubcondition("FollowerSubcondition", 0, FollowerSubcondition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_FOLLOWERSUBCONDITION = &TypeDescriptor_FollowerSubcondition;

static const TypeDescriptorField BuffSubcondition_Fields[4] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(BuffSubcondition, snoBuffPower), "", 0x1d, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(BuffSubcondition, nIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BuffSubcondition, nNumStacks), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BuffSubcondition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BuffSubcondition("BuffSubcondition", 0, BuffSubcondition_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BUFFSUBCONDITION = &TypeDescriptor_BuffSubcondition;

static const TypeDescriptorField ConditionDefinition_Fields[49] = {
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fMale), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fFemale), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fPlayerClass), NULL, 1, 7, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fPlayerClass), NULL, 1, 7, 0),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fIsStoryMode), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fIsAdventureMode), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fPlayerIsMaxLevel), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, fPlayerIsNotMaxLevel), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_LORESUBCONDITION, offsetof(ConditionDefinition, tLoreSubconditions), NULL, 1, 3, 0),
    FixedArrayTypeDescriptorField("", DT_QUESTSUBCONDITION, offsetof(ConditionDefinition, tQuestSubconditions), NULL, 1, 3, 0),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, eQuestRewardAct), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bQuestRewardRequireBonusAct), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bQuestRewardRequireAllActsComplete), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_ITEMSUBCONDITION, offsetof(ConditionDefinition, tItemSubconditions), NULL, 1, 3, 0),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nPlayerLevelMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nPlayerLevelMax), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nLooteeLevelMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nLooteeLevelMax), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nHandicapMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nHandicapMax), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nTieredLootrunMin), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nTieredLootrunMax), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, eActMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, eActMax), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nPlayersMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nPlayersMax), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ConditionDefinition, snoCurrentWorld), "", 0x30, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ConditionDefinition, snoCurrentLevelArea), "", 0x16, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ConditionDefinition, snoQuestRange), "", 0x3a, 0, -1),
    TypeDescriptorField("", DT_FOLLOWERSUBCONDITION, offsetof(ConditionDefinition, tFollowerSubcondition), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_LABELSUBCONDITION, offsetof(ConditionDefinition, tLabelSubconditions), NULL, 1, 3, 0),
    FixedArrayTypeDescriptorField("", DT_SKILLSUBCONDITION, offsetof(ConditionDefinition, tSkillSubconditions), NULL, 1, 3, 0),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bNormal), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bChampion), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bRare), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bUnique), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bBoss), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bGameIsPlatinum), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bPlatinumCapReached), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nPlatinumCapRemaining), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bDoubledGoblinsActive), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_MONSTERSUBCONDITION, offsetof(ConditionDefinition, tMonsterSubconditions), NULL, 1, 0xf, 0),
    FixedArrayTypeDescriptorField("", DT_GAMEFLAGSUBCONDITION, offsetof(ConditionDefinition, tGameFlagSubconditions), NULL, 1, 3, 0),
    FixedArrayTypeDescriptorField("", DT_PLAYERFLAGSUBCONDITION, offsetof(ConditionDefinition, tPlayerFlagSubconditions), NULL, 1, 3, 0),
    FixedArrayTypeDescriptorField("", DT_BUFFSUBCONDITION, offsetof(ConditionDefinition, tBuffSubconditions), NULL, 1, 3, 0),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, nAccountPartitionFlag), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bLearnedAllRecipesBlacksmith), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConditionDefinition, bLearnedAllRecipesJeweler), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConditionDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConditionDefinition("Condition", 4, ConditionDefinition_Fields, 49, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CONDITION = &TypeDescriptor_ConditionDefinition;


ConditionGroupDef * gptGroupDefCondition;  // line 163

// line 169
ConditionGroupDef::ConditionGroupDef()
    : GroupDefinition("Condition", CONDITION_SNO,
                      SNOVersions(45, 47, 45, 45, 46),
                      SNODATATYPE_TITLE, sizeof(ConditionDefinition), DT_CONDITION, 0x8241, 0x500e)
{
}

// line 195
uint32 ConditionGroupDef::GetMaxCount()
{
    return 128;
}

// line 207
void ConditionGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    ConditionDefinition * pdefCondition = (ConditionDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdefCondition, sizeof(ConditionDefinition));

    for (int32 i = 0; i < MAX_LORE_SUBCONDITIONS; i++)
    {
        pdefCondition->tLoreSubconditions[i].snoLore = SNO_NONE;
        pdefCondition->tLoreSubconditions[i].eLearned = TERNARY_NA;
    }

    for (int32 i = 0; i < MAX_QUEST_SUBCONDITIONS; i++)
    {
        QuestSubcondition & tQuestSubcondition = pdefCondition->tQuestSubconditions[i];
        tQuestSubcondition.snoQuest = SNO_NONE;
        tQuestSubcondition.nStepUID = -2;
        tQuestSubcondition.eCompleted = TERNARY_NA;
        tQuestSubcondition.eAssigned = TERNARY_NA;
    }
    pdefCondition->eQuestRewardAct = -1;
    pdefCondition->bQuestRewardRequireBonusAct = FALSE;
    pdefCondition->bQuestRewardRequireAllActsComplete = FALSE;

    for (int32 i = 0; i < MAX_ITEM_SUBCONDITIONS; i++)
    {
        ItemSubcondition & tItemSubcondition = pdefCondition->tItemSubconditions[i];
        tItemSubcondition.gbidItem = -1;
    }
    pdefCondition->snoCurrentWorld = SNO_NONE;
    pdefCondition->snoCurrentLevelArea = SNO_NONE;
    pdefCondition->snoQuestRange = SNO_NONE;
    pdefCondition->eActMin = -1;
    pdefCondition->eActMax = -1;

    pdefCondition->tFollowerSubcondition.eHirelingClass = 0;
    pdefCondition->tFollowerSubcondition.eCurrent = TERNARY_NA;

    for (int32 i = 0; i < MAX_LABEL_SUBCONDITIONS; i++)
    {
        LabelSubcondition & tLabelSubcondition = pdefCondition->tLabelSubconditions[i];
        tLabelSubcondition.fActive = FALSE;
    }

    for (int32 i = 0; i < MAX_SKILL_SUBCONDITIONS; i++)
    {
        SkillSubcondition & tSkillSubcondition = pdefCondition->tSkillSubconditions[i];
        tSkillSubcondition.snoSkillPower = SNO_NONE;
    }

    pdefCondition->bNormal = FALSE;
    pdefCondition->bChampion = FALSE;
    pdefCondition->bRare = FALSE;
    pdefCondition->bUnique = FALSE;
    pdefCondition->bBoss = FALSE;

    for (int32 i = 0; i < MAX_MONSTER_SUBCONDITIONS; i++)
    {
        MonsterSubcondition & tMonsterSubcondition = pdefCondition->tMonsterSubconditions[i];
        tMonsterSubcondition.snoMonsterActor = SNO_NONE;
    }

    for (int32 i = 0; i < MAX_GAMEFLAG_SUBCONDITIONS; i++)
    {
        GameFlagSubcondition & tGameFlagSubcondition = pdefCondition->tGameFlagSubconditions[i];
        tGameFlagSubcondition.szLabel[0] = 0;
    }

    for (int32 i = 0; i < MAX_PLAYERFLAG_SUBCONDITIONS; i++)
    {
        PlayerFlagSubcondition & tPlayerFlagSubcondition = pdefCondition->tPlayerFlagSubconditions[i];
        tPlayerFlagSubcondition.szLabel[0] = 0;
    }

    for (int32 i = 0; i < MAX_BUFF_SUBCONDITIONS; i++)
    {
        BuffSubcondition & tBuffSubcondition = pdefCondition->tBuffSubconditions[i];
        tBuffSubcondition.snoBuffPower = SNO_NONE;
        tBuffSubcondition.nIndex = 0;
        tBuffSubcondition.nNumStacks = 0;
    }

    pdefCondition->nHandicapMin = -1;
    pdefCondition->nHandicapMax = -1;
    pdefCondition->nTieredLootrunMin = -1;
    pdefCondition->nTieredLootrunMax = -1;
    pdefCondition->nAccountPartitionFlag = -1;

    pdefCondition->bLearnedAllRecipesBlacksmith = FALSE;
    pdefCondition->bLearnedAllRecipesJeweler = FALSE;
}

// ConditionEvaluate: includes, declarations and helper forward declarations.
#include "HeroDefinition.h"
#include "LevelAreaDefinition.h"
#include "QuestRangeDefinition.h"
#include "../ActorInventory.h"
#include "../GameCommonData.h"
#include "../Hireling.h"
#include "../Player.h"
#include "../SceneCommon.h"
#include "../World.h"
#include "../Crafting/ItemCrafting.h"
#include "../Crafting/ItemCraftingSwig.h"
#include "../HirelingSwig.h"
#include "../Nav/NavigationMesh.h"
#include "../Quests/Quests.h"
#include "../../Server/SGame.h"
#include "../../Server/Loot/Loot.h"
#include "../../Server/Powers/PowerAPI.h"

class DRLGGenerator;
BOOL LabelAlreadySetInGame(GBID gbidLabel, DRLGGenerator * ptDRLGGenerator);  // X/Common/Labels.cpp:809
BOOL GameCommonGameFlagIsSet(LPCSTR szFlagLabel);  // X/Common/GameCommonData.cpp:500
BOOL PlayerIsPlayerGameFlagSet(const Player * ptPlayer, LPCSTR szLabel);  // X/Common/Player.cpp:3461
BOOL ActAllBountiesComplete();  // X/Common/Act.cpp:599

// X/Common/Definitions/ConditionDefinition.h:205-210 (the header does not see ConditionDefinition yet).
template <> inline const ConditionDefinition * DefinitionLockR<ConditionDefinition>(SNO sno) { return (const ConditionDefinition *)gptGroupDefCondition->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<ConditionDefinition>(const ConditionDefinition * pdef) { gptGroupDefCondition->UnlockR((void *&)pdef); }
typedef ScopedDefinitionLockR<ConditionDefinition> ScopedConditionDefinitionLockR;

// Defined after ConditionEvaluate (DWARF lines 298-638 are taken by the code above).
static BOOL sHandleLoreSubcondition(const LoreSubcondition & tLoreSubcondition, const Player * ptPlayer);
static BOOL sHandleFollowerSubcondition(const FollowerSubcondition & tFollowerSubcondition, const Player * ptPlayer);
static BOOL sHandleQuestSubcondition(const QuestSubcondition & tQuestSubcondition);
static BOOL sHandleItemSubcondition(ActorCommonData & tACDPlayer, const ItemSubcondition & tItemSubcondition);
static BOOL sHandleWorldSubcondition(const ActorCommonData * ptACD, const LooteeInfo * ptLooteeInfo, const ConditionDefinition * pdefCondition);
static BOOL sHandleGameFlagSubcondition(const GameFlagSubcondition & tGameFlagSubcondition);
static BOOL sHandlePlayerFlagSubcondition(const Player * ptPlayer, const PlayerFlagSubcondition & tPlayerFlagSubcondition);








































































































































































































BOOL ConditionEvaluate(
	const SNO snoCondition,
	const Player * ptPlayer,
	const ACDID idACD,
	const LooteeInfo * ptLooteeInfo)
{
	ScopedConditionDefinitionLockR pdefCondition(snoCondition);
	if (!pdefCondition)
	{
		return FALSE;
	}

	ActorCommonData * ptACD = ACDTryToGet(idACD);
	if (ptPlayer &&
		ptACD)
	{
		// gender
		BOOL fMale = PlayerIsMale(*ptPlayer);
		if (pdefCondition->fMale && !fMale)
		{
			return FALSE;
		}

		if (pdefCondition->fFemale && fMale)
		{
			return FALSE;
		}


		// player class
		PlayerClass ePlayerClass = PlayerGetPlayerClass(idACD);
		BOOL bPlayerClassMatchFound = FALSE;
		BOOL bPlayerClassRequirementFound = FALSE;
		for (PlayerClass eClassCmp = 0; eClassCmp < NUM_PLAYER_CLASSES; eClassCmp++)
		{
			if (pdefCondition->fPlayerClass[eClassCmp])
			{
				bPlayerClassRequirementFound = TRUE;

				if (ePlayerClass == eClassCmp)
				{
					bPlayerClassMatchFound = TRUE;
					break;
				}
			}
		}

		if (bPlayerClassRequirementFound && !bPlayerClassMatchFound)
		{
			return FALSE;
		}

		// level
		int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
		int32 nLevelCap = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL_CAP);
		if (pdefCondition->fPlayerIsMaxLevel && nLevel < nLevelCap)
		{
			return FALSE;
		}
		if (pdefCondition->fPlayerIsNotMaxLevel && nLevel >= nLevelCap)
		{
			return FALSE;
		}

		// lore
		for (int32 i = 0; i < MAX_LORE_SUBCONDITIONS; i++)
		{
			const LoreSubcondition & tLoreSubcondition = pdefCondition->tLoreSubconditions[i];
			if (!sHandleLoreSubcondition(tLoreSubcondition, ptPlayer))
			{
				return FALSE;
			}
		}

		// follower
		if (!sHandleFollowerSubcondition(pdefCondition->tFollowerSubcondition, ptPlayer))
		{
			return FALSE;
		}

		// loot run
		if (pdefCondition->nTieredLootrunMin == -1 &&
			pdefCondition->nTieredLootrunMax != -1)
		{
			if (ACDIsInLootRun(*ptACD) ||
				ACDIsInTieredLootRun(*ptACD))
			{
				return FALSE;
			}
		}
	}

	// game mode
	if (pdefCondition->fIsStoryMode)
	{
		if (!GameIsStoryMode())
		{
			return FALSE;
		}
	}
	if (pdefCondition->fIsAdventureMode)
	{
		if (!GameIsAdventureMode())
		{
			return FALSE;
		}
	}
	// handicap
	int32 nHandicapLevel = GameGetHandicapLevel();
	if (pdefCondition->nHandicapMin >= 0 && nHandicapLevel < pdefCondition->nHandicapMin)
	{
		return FALSE;
	}


	if (pdefCondition->nHandicapMax >= 0 && nHandicapLevel > pdefCondition->nHandicapMax)
	{
		return FALSE;
	}

	// quests
	for (int32 i = 0; i < MAX_QUEST_SUBCONDITIONS; i++)
	{
		const QuestSubcondition & tQuestSubcondition = pdefCondition->tQuestSubconditions[i];
		if (!sHandleQuestSubcondition(tQuestSubcondition))
		{
			return FALSE;
		}
	}

	// loot
	if (ptLooteeInfo)
	{
		if (pdefCondition->eQuestRewardAct != ACT_INVALID &&
			ptLooteeInfo->eQuestAct != pdefCondition->eQuestRewardAct)
		{
			return FALSE;
		}

		if (pdefCondition->nTieredLootrunMin >= 0 &&
			ptLooteeInfo->tLooteeParams.nTieredLootRunLevel + 1 < pdefCondition->nTieredLootrunMin)
		{
			return FALSE;
		}

		if (pdefCondition->nTieredLootrunMax >= 0 &&
			ptLooteeInfo->tLooteeParams.nTieredLootRunLevel >= pdefCondition->nTieredLootrunMax)
		{
			return FALSE;
		}
	}

	// "== TRUE": the original compares the BOOL with 1 (cmp #0x1 at line 793).
	if (pdefCondition->bQuestRewardRequireAllActsComplete == TRUE &&
		!ActAllBountiesComplete())
	{
		return FALSE;
	}

	// world
	if (!sHandleWorldSubcondition(ptACD, ptLooteeInfo, pdefCondition))
	{
		return FALSE;
	}

	if (pdefCondition->bGameIsPlatinum && !GameGetDailyPlatinumLimit())
	{
		return FALSE;
	}


	if (pdefCondition->bDoubledGoblinsActive && !SGameCommunityBuffDoubleGoblinsActive())
	{
		return FALSE;
	}

	if (ptACD)
	{
		if (ptPlayer)
		{
			// items
			for (int32 i = 0; i < MAX_ITEM_SUBCONDITIONS; i++)
			{
				const ItemSubcondition & tItemSubcondition = pdefCondition->tItemSubconditions[i];
				if (!sHandleItemSubcondition(*ptACD, tItemSubcondition))
				{
					return FALSE;
				}
			}
			// Inferred scope: DWARF nests the skill loop and the 865/886 locals in one block that does not contain the item loop.
			{
				// skills
				for (int32 i = 0; i < MAX_SKILL_SUBCONDITIONS; i++)
				{
					const SkillSubcondition & tSkillSubcondition = pdefCondition->tSkillSubconditions[i];
					if (tSkillSubcondition.snoSkillPower != SNO_NONE)
					{
						int32 nSkillLevel = ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, tSkillSubcondition.snoSkillPower));
						if (nSkillLevel < tSkillSubcondition.nLevelMin)

						{
							return FALSE;
						}


						if (tSkillSubcondition.nLevelMax && nSkillLevel > tSkillSubcondition.nLevelMax)
						{
							return FALSE;
						}
					}
				}

				// platinum
				if (pdefCondition->bPlatinumCapReached && ptACD->AttributesGetInt(ATTRIBUTE_PLATINUM_CAP_REMAINING) > 0)
				{
					return FALSE;
				}

				if (pdefCondition->nPlatinumCapRemaining > ptACD->AttributesGetInt(ATTRIBUTE_PLATINUM_CAP_REMAINING))
				{
					return FALSE;
				}


				// monsters
				BOOL bMonsterSpecified = FALSE;
				BOOL bMonsterFound = FALSE;
				for (int32 i = 0; i < MAX_MONSTER_SUBCONDITIONS; i++)
				{
					const MonsterSubcondition & tMonsterSubcondition = pdefCondition->tMonsterSubconditions[i];
					if (tMonsterSubcondition.snoMonsterActor != SNO_NONE)
					{
						bMonsterSpecified = TRUE;
						if (ptLooteeInfo && ptLooteeInfo->snoActor == tMonsterSubcondition.snoMonsterActor)
						{
							bMonsterFound = TRUE;
						}
					}
				}
				if (bMonsterSpecified && !bMonsterFound)
				{
					return FALSE;
				}


				// monster rarity
				BOOL bMonsterRaritySpecified = FALSE;
				BOOL bMonsterRarityFound = FALSE;

				if (pdefCondition->bNormal ||
					pdefCondition->bChampion ||
					pdefCondition->bRare ||
					pdefCondition->bUnique ||
					pdefCondition->bBoss)
				{
					bMonsterRaritySpecified = TRUE;

					if ((pdefCondition->bNormal && ptLooteeInfo->eMonsterRarity == MR_NORMAL) ||



						(pdefCondition->bChampion && ptLooteeInfo->eMonsterRarity == MR_CHAMPION) ||



						(pdefCondition->bRare && ptLooteeInfo->eMonsterRarity == MR_RARE) ||



						(pdefCondition->bUnique && ptLooteeInfo->eMonsterRarity == MR_UNIQUE) ||



						(pdefCondition->bBoss && ptLooteeInfo->eMonsterRarity == MR_BOSS))
					{
						bMonsterRarityFound = TRUE;
					}
				}

				if (bMonsterRaritySpecified && !bMonsterRarityFound)
				{
					return FALSE;
				}
			}
		}
		else
		{
			// monsters
			BOOL bMonsterSpecified = FALSE;
			BOOL bMonsterFound = FALSE;
			for (int32 i = 0; i < MAX_MONSTER_SUBCONDITIONS; i++)
			{
				const MonsterSubcondition & tMonsterSubcondition = pdefCondition->tMonsterSubconditions[i];
				if (tMonsterSubcondition.snoMonsterActor != SNO_NONE)
				{
					bMonsterSpecified = TRUE;
					if (ptACD->IsMonster() && ptACD->tSNOName.snoHandle == tMonsterSubcondition.snoMonsterActor)
					{
						bMonsterFound = TRUE;
					}
				}
			}
			if (bMonsterSpecified && !bMonsterFound)
			{
				return FALSE;
			}
		}

		// buffs


		if (GameActiveGameCommonDataIsServer())
		{
			for (int32 i = 0; i < MAX_BUFF_SUBCONDITIONS; i++)
			{
				const BuffSubcondition & tBuffSubcondition = pdefCondition->tBuffSubconditions[i];
				if (tBuffSubcondition.snoBuffPower != SNO_NONE &&
					tBuffSubcondition.nNumStacks >= 0)
				{
					int32 nIndex = tBuffSubcondition.nIndex;
					if (nIndex >= 0 && nIndex < 32)  // 32: no named constant found for the buff index range
					{
						int32 nCount = PowerAPI_BuffCountOnACD(tBuffSubcondition.snoBuffPower, nIndex, *ptACD, ACDID_INVALID);
						if (nCount < tBuffSubcondition.nNumStacks)
						{
							return FALSE;
						}
					}
				}
			}
		}
	}

	// game flags
	for (int32 i = 0; i < MAX_GAMEFLAG_SUBCONDITIONS; i++)
	{
		const GameFlagSubcondition & tGameFlagSubcondition = pdefCondition->tGameFlagSubconditions[i];
		if (!sHandleGameFlagSubcondition(tGameFlagSubcondition))
		{
			return FALSE;
		}
	}

	if (ptPlayer)
	{
		for (int32 i = 0; i < MAX_PLAYERFLAG_SUBCONDITIONS; i++)
		{
			const PlayerFlagSubcondition & tPlayerFlagSubcondition = pdefCondition->tPlayerFlagSubconditions[i];
			if (!sHandlePlayerFlagSubcondition(ptPlayer, tPlayerFlagSubcondition))
			{
				return FALSE;
			}
		}
	}
	// "ptPlayer &&" on 995/1003/1011: inferred from their columns (ptPlayer is known non-null on that path, so it emits nothing).
	if (ptPlayer && pdefCondition->nAccountPartitionFlag >= 0)
	{
		if (!(ptPlayer->tAccount.uPartitionFlags & (1 << pdefCondition->nAccountPartitionFlag)))
		{
			return FALSE;
		}
	}

	if (ptPlayer && pdefCondition->bLearnedAllRecipesBlacksmith)
	{
		if (!ItemCrafting_PlayerKnowsAllRecipes(*ptPlayer, CRAFTER_TYPE_BLACKSMITH))
		{
			return FALSE;
		}
	}

	if (ptPlayer && pdefCondition->bLearnedAllRecipesJeweler)
	{
		if (!ItemCrafting_PlayerKnowsAllRecipes(*ptPlayer, CRAFTER_TYPE_JEWEL))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// DWARF line 298
static BOOL sHandleLoreSubcondition(
	const LoreSubcondition & tLoreSubcondition,
	const Player * ptPlayer)
{
	if (tLoreSubcondition.snoLore != SNO_NONE)
	{
		BOOL fLearned = ptPlayer->tHeroStateData.tSavedData.tLearnedLore.Find(tLoreSubcondition.snoLore) != -1;
		if (GameIsWeeklyChallenge())
		{
			fLearned = TRUE;
		}
		if (tLoreSubcondition.eLearned == TERNARY_TRUE && !fLearned)
		{
			return FALSE;
		}
		if (tLoreSubcondition.eLearned == TERNARY_FALSE && fLearned)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// DWARF line 331
static BOOL sHandleFollowerSubcondition(
	const FollowerSubcondition & tFollowerSubcondition,
	const Player * ptPlayer)
{
	if (tFollowerSubcondition.eCurrent != TERNARY_NA)
	{
		ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
		ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
		HirelingClass eClass = ptACDHireling ? HirelingGetClass(*ptACDHireling) : HC_NONE;
		BOOL bPassed = (tFollowerSubcondition.eHirelingClass == eClass) == (tFollowerSubcondition.eCurrent == TERNARY_TRUE);
		return bPassed;
	}
	return TRUE;
}

// DWARF line 351
static BOOL sHandleQuestSubcondition(
	const QuestSubcondition & tQuestSubcondition)
{
	if (tQuestSubcondition.snoQuest != SNO_NONE)
	{
		QuestInstance * ptQuestInstance = QuestsGetQuestInstance(tQuestSubcondition.snoQuest, ACDID_INVALID);
		if (!ptQuestInstance)
		{
			return tQuestSubcondition.eAssigned == TERNARY_FALSE;
		}
		if (tQuestSubcondition.nStepUID != QUEST_NULL_STEP_UID)
		{
			if (tQuestSubcondition.nStepUID != ptQuestInstance->nCurrentStepUID)
			{
				return FALSE;
			}
		}
		else
		{
			if (tQuestSubcondition.eAssigned == TERNARY_TRUE &&
				ptQuestInstance->eInstanceState != QUEST_INSTANCE_ACTIVE &&
				!QuestIsAnyBountyType(ptQuestInstance->snoLevelArea))
			{
				return FALSE;
			}
			if (tQuestSubcondition.eAssigned == TERNARY_FALSE &&
				ptQuestInstance->eInstanceState == QUEST_INSTANCE_ACTIVE)
			{
				return FALSE;
			}
			if (tQuestSubcondition.eCompleted == TERNARY_TRUE &&
				ptQuestInstance->eInstanceState != QUEST_INSTANCE_COMPLETED)
			{
				return FALSE;
			}
			if (tQuestSubcondition.eCompleted == TERNARY_FALSE &&
				ptQuestInstance->eInstanceState == QUEST_INSTANCE_COMPLETED)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

// DWARF line 412
static BOOL sHandleItemSubcondition(
	ActorCommonData & tACDPlayer,
	const ItemSubcondition & tItemSubcondition)
{
	if (tItemSubcondition.gbidItem != GBID_NONE)
	{
		GBHandle hGBItem;
		hGBItem.Set(GB_ITEMS, tItemSubcondition.gbidItem);
		int32 nItemCount = ACDInventoryGetItemCount(tACDPlayer, hGBItem, cgtDefaultPlayerInventorySlots, FALSE, FALSE);
		if (tItemSubcondition.fPossessed)
		{
			if (nItemCount < tItemSubcondition.nCountMin)
			{
				return FALSE;
			}
			if (tItemSubcondition.nCountMax && nItemCount > tItemSubcondition.nCountMax)
			{
				return FALSE;
			}
		}
		else
		{
			if (nItemCount)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

// DWARF line 452
static BOOL sHandleLabelSubcondition(
	const LabelSubcondition & tLabelSubcondition)
{
	if (tLabelSubcondition.gbidLabel != GBID_NONE)
	{
		BOOL fSet = LabelAlreadySetInGame(tLabelSubcondition.gbidLabel, NULL);
		if ((tLabelSubcondition.fActive == FALSE) ==
			(fSet != FALSE))
		{
			return FALSE;
		}
	}
	return TRUE;
}

// DWARF line 476
static BOOL sHandleWorldSubcondition(
	const ActorCommonData * ptACD,
	const LooteeInfo * ptLooteeInfo,
	const ConditionDefinition * pdefCondition)
{
	if (ptACD)
	{
		int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
		if (nLevel < pdefCondition->nPlayerLevelMin)
		{
			return FALSE;
		}
		if (pdefCondition->nPlayerLevelMax && nLevel > pdefCondition->nPlayerLevelMax)
		{
			return FALSE;
		}
		const WorldPlace & tPlace = ptACD->GetPlace();
		if (pdefCondition->snoCurrentWorld != SNO_NONE)
		{
			SNO snoWorld = WorldCommonGetWorldSNO(tPlace.idSWorld);
			if (snoWorld != pdefCondition->snoCurrentWorld)
			{
				return FALSE;
			}
		}
		if (pdefCondition->snoCurrentLevelArea != SNO_NONE)
		{
			SNO snoLevelArea = SNO_NONE;
			NavMeshHandle tNavMeshHandle;
			ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
			if (!NavMeshHandleIsNone(tNavMeshHandle))
			{
				snoLevelArea = NavMeshHandleGetLevelArea(tNavMeshHandle);
			}
			if (snoLevelArea != pdefCondition->snoCurrentLevelArea)
			{
				return FALSE;
			}
		}
	}
	else
	{
		int32 nLevel = GameGetMonsterLevel();
		if (nLevel < pdefCondition->nPlayerLevelMin)
		{
			return FALSE;
		}
		if (pdefCondition->nPlayerLevelMax && nLevel > pdefCondition->nPlayerLevelMax)
		{
			return FALSE;
		}
	}
	if (ptLooteeInfo)
	{
		int32 nLevel = ptLooteeInfo->tLooteeParams.nLevel;
		if (nLevel < pdefCondition->nLooteeLevelMin)
		{
			return FALSE;
		}
		if (pdefCondition->nLooteeLevelMax && nLevel > pdefCondition->nLooteeLevelMax)
		{
			return FALSE;
		}
	}
	if (pdefCondition->snoQuestRange != SNO_NONE)
	{
		if (!QuestRangeEvaluate(pdefCondition->snoQuestRange, NULL))
		{
			return FALSE;
		}
	}
	Act eAct = GameGetCurrentAct();
	if (GameIsOpenWorld() && ptLooteeInfo)
	{
		if (ptLooteeInfo->snoLevelArea != SNO_NONE)
		{
			eAct = LevelAreaGetAct(ptLooteeInfo->snoLevelArea);
		}
	}
	if (pdefCondition->eActMin != ACT_INVALID && eAct < pdefCondition->eActMin)
	{
		return FALSE;
	}
	if (pdefCondition->eActMax != ACT_INVALID && eAct > pdefCondition->eActMax)
	{
		return FALSE;
	}
	int32 nPlayersInGame = PlayerGetNumPlayers();
	if (nPlayersInGame < pdefCondition->nPlayersMin)
	{
		return FALSE;
	}
	if (pdefCondition->nPlayersMax && nPlayersInGame > pdefCondition->nPlayersMax)
	{
		return FALSE;
	}
	for (int32 i = 0; i < MAX_LABEL_SUBCONDITIONS; i++)
	{
		const LabelSubcondition & tLabelSubconditions = pdefCondition->tLabelSubconditions[i];
		if (!sHandleLabelSubcondition(tLabelSubconditions))
		{
			return FALSE;
		}
	}
	return TRUE;
}

// DWARF line 612
static BOOL sHandleGameFlagSubcondition(
	const GameFlagSubcondition & tGameFlagSubcondition)
{
	if (strlen(tGameFlagSubcondition.szLabel) == 0)
	{
		return TRUE;
	}
	return GameCommonGameFlagIsSet(tGameFlagSubcondition.szLabel);
}

// DWARF line 625
static BOOL sHandlePlayerFlagSubcondition(
	const Player * ptPlayer,
	const PlayerFlagSubcondition & tPlayerFlagSubcondition)
{
	if (strlen(tPlayerFlagSubcondition.szLabel) == 0)
	{
		return TRUE;
	}
	return PlayerIsPlayerGameFlagSet(ptPlayer, tPlayerFlagSubcondition.szLabel);
}
