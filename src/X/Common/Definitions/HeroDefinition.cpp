// Original file: X/Common/Definitions/HeroDefinition.cpp

#include "HeroDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../SNO/SymbolicValue.h"
#include "../ActorCommonData.h"
#include "../Player.h"
#include "../Hireling.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../Quests/Quests.h"
#include "../VisualInventory.h"
#include "SkillKitDefinition.h"
#include "../../Server/SGameManager.h"
#include "../GlobalSNO.h"
#include "GlobalDefinition.h"
#include "../Attributes/AttributeSerializer.h"
#include "../Attributes/FastAttribute.h"
#include "../Protobuf/AttributeSerializer.pb.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp
extern const XTypeDescriptor * DT_HOTBARBUTTONDATA;  // X/Common/HotbarButton.cpp
extern const XTypeDescriptor * DT_SAVED_CONVERSATIONS;
extern const XTypeDescriptor * DT_HIRELING_INFO;
extern const XTypeDescriptor * DT_LEARNED_LORE;
extern const XTypeDescriptor * DT_ACTIVE_SKILL_SAVED_DATA;
extern const XTypeDescriptor * DT_SAVEDQUEST;
extern const XTypeDescriptor * DT_QUEST_HISTORY_ENTRY;
extern const XTypeDescriptor * DT_HIRELING_SAVED_DATA;
extern const XTypeDescriptor * DT_SAVE_POINT_DATA;
extern const XTypeDescriptor * DT_PLAYER_SAVED_DATA;

// Not written: HeroDefinitionCompose/HeroDefinitionParse (inline, no copy in the image).
const uint32 HERO_DEFINITION_VERSION_1_0_5 = 903;  // line 43
const uint32 HERO_DEFINITION_VERSION_2_0_0 = 905;  // line 44

// line 53. All seven entries are zero in the original image (0x7100e54ce8).
static const uint32 scgdwSkillVersion[7] = { 0, 0, 0, 0, 0, 0, 0 };

// The field names are "" in the binary.
static const TypeDescriptorField PlayerSavedQuest_Fields[5] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayerSavedQuest, snoQuest), "", 0x1f, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedQuest, nCurrentStepUID), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(PlayerSavedQuest, arObjectiveState), NULL, 1, 0x14, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(PlayerSavedQuest, arFailureConditionState), NULL, 1, 0x14, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSavedQuest), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSavedQuest("PlayerSavedQuest", 0, PlayerSavedQuest_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVEDQUEST = &TypeDescriptor_PlayerSavedQuest;

static const TypeDescriptorField PlayerSavedQuestUpdateMessage_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerSavedQuestUpdateMessage, tHeader), NULL, 1),
    EnumTypeDescriptorField("", offsetof(PlayerSavedQuestUpdateMessage, eLastPlayedAct), NULL, gActs),
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayerSavedQuestUpdateMessage, snoLastPlayedQuest), "", 0x1f, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedQuestUpdateMessage, nLastPlayedStepUID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedQuestUpdateMessage, nHandicapLevel), NULL, 1),
    FixedArrayTypeDescriptorField("tSavedQuest", DT_SAVEDQUEST, offsetof(PlayerSavedQuestUpdateMessage, tSavedQuest), NULL, 8, 0x40, 0x2a00),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSavedQuestUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSavedQuestUpdateMessage("PlayerSavedQuestUpdateMessage", 0, PlayerSavedQuestUpdateMessage_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVED_QUEST_UPDATE_MESSAGE = &TypeDescriptor_PlayerSavedQuestUpdateMessage;

static const TypeDescriptorField PlayerQuestHistoryEntry_Fields[3] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayerQuestHistoryEntry, snoQuest), "", 0x1f, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerQuestHistoryEntry, nStepUID), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerQuestHistoryEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerQuestHistoryEntry("PlayerQuestHistoryEntry", 0, PlayerQuestHistoryEntry_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_HISTORY_ENTRY = &TypeDescriptor_PlayerQuestHistoryEntry;

static const TypeDescriptorField PlayerQuestHistoryUpdateMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerQuestHistoryUpdateMessage, tHeader), NULL, 1),
    FixedArrayTypeDescriptorField("tQuestHistory", DT_QUEST_HISTORY_ENTRY, offsetof(PlayerQuestHistoryUpdateMessage, tQuestHistory), NULL, 8, 200, 0x640),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerQuestHistoryUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerQuestHistoryUpdateMessage("PlayerQuestHistoryUpdateMessage", 0, PlayerQuestHistoryUpdateMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_HISTORY_UPDATE_MESSAGE = &TypeDescriptor_PlayerQuestHistoryUpdateMessage;

static const TypeDescriptorField PlayerQuestRewardHistoryEntry_Fields[4] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayerQuestRewardHistoryEntry, snoQuest), "", 0x1f, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerQuestRewardHistoryEntry, nStepUID), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(PlayerQuestRewardHistoryEntry, uDifficultyFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerQuestRewardHistoryEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerQuestRewardHistoryEntry("PlayerQuestRewardHistoryEntry", 0, PlayerQuestRewardHistoryEntry_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUEST_REWARD_HISTORY_ENTRY = &TypeDescriptor_PlayerQuestRewardHistoryEntry;

static const TypeDescriptorField HirelingInfo_Fields[9] = {
    BoundedTypeDescriptorField("", DT_INT, offsetof(HirelingInfo, eClass), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfo, gbidName), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(HirelingInfo, fDead), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfo, arnPowerKeyParam), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfo, arnPowerKeyParam) + 1 * sizeof(int32), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfo, arnPowerKeyParam) + 2 * sizeof(int32), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfo, arnPowerKeyParam) + 3 * sizeof(int32), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfo, arnPowerKeyParam) + 4 * sizeof(int32), (const void *)-1, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HirelingInfo), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HirelingInfo("HirelingInfo", 0, HirelingInfo_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HIRELING_INFO = &TypeDescriptor_HirelingInfo;

static const TypeDescriptorField HirelingSavedData_Fields[4] = {
    FixedArrayTypeDescriptorField("", DT_HIRELING_INFO, offsetof(HirelingSavedData, tHireling), NULL, 1, 4, 0),
    BoundedTypeDescriptorField("", DT_INT, offsetof(HirelingSavedData, eActiveHireling), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(HirelingSavedData, dwAvailableHirelings), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HirelingSavedData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HirelingSavedData("HirelingSavedData", 0, HirelingSavedData_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HIRELING_SAVED_DATA = &TypeDescriptor_HirelingSavedData;

static const TypeDescriptorField LearnedLore_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(LearnedLore, m_nLearnedCount), NULL, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(LearnedLore, m_snoLoreLearned), NULL, 0, 0x200, 0x3b, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LearnedLore), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LearnedLore("LearnedLore", 0, LearnedLore_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LEARNED_LORE = &TypeDescriptor_LearnedLore;

static const TypeDescriptorField SavedConversations_Fields[3] = {
    FixedArrayTypeDescriptorField("", DT_BYTE, offsetof(SavedConversations, m_arPlayedConv), NULL, 1, 0x46, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(SavedConversations, m_snoSavedConv), NULL, 8, 0x230, 0xc, -0x4c),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SavedConversations), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SavedConversations("SavedConversations", 0, SavedConversations_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVED_CONVERSATIONS = &TypeDescriptor_SavedConversations;

static const TypeDescriptorField PlayerSavedConversationsUpdateMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerSavedConversationsUpdateMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SAVED_CONVERSATIONS, offsetof(PlayerSavedConversationsUpdateMessage, tSavedConversations), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSavedConversationsUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSavedConversationsUpdateMessage("PlayerSavedConversationsUpdateMessage", 0, PlayerSavedConversationsUpdateMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVED_CONVERSATIONS_UPDATE_MESSAGE = &TypeDescriptor_PlayerSavedConversationsUpdateMessage;

static const TypeDescriptorField SavePointData_Fields[3] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(SavePointData, snoWorld), "", 0x30, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SavePointData, nNum), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SavePointData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SavePointData("SavePointData", 0, SavePointData_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVE_POINT_DATA = &TypeDescriptor_SavePointData;

static const TypeDescriptorField SavePointDataMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SavePointDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SAVE_POINT_DATA, offsetof(SavePointDataMessage, tData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SavePointDataMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SavePointDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SavePointDataMessage("SavePointDataMessage", 0, SavePointDataMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVE_POINT_DATA_MESSAGE = &TypeDescriptor_SavePointDataMessage;

static const TypeDescriptorField LastWaypointLevelAreaDataMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(LastWaypointLevelAreaDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(LastWaypointLevelAreaDataMessage, snoLevelArea), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LastWaypointLevelAreaDataMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LastWaypointLevelAreaDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LastWaypointLevelAreaDataMessage("LastWaypointLevelAreaDataMessage", 0, LastWaypointLevelAreaDataMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LAST_WAYPOINT_LEVELAREA_DATA_MESSAGE = &TypeDescriptor_LastWaypointLevelAreaDataMessage;

static const TypeDescriptorField ActiveSkillSavedData_Fields[3] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(ActiveSkillSavedData, snoSkill), "", 0x1d, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ActiveSkillSavedData, eRuneType), NULL, (const void *)-1, (const void *)4),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActiveSkillSavedData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActiveSkillSavedData("ActiveSkillSavedData", 0, ActiveSkillSavedData_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACTIVE_SKILL_SAVED_DATA = &TypeDescriptor_ActiveSkillSavedData;

static const TypeDescriptorField PlayerSavedData_Fields[14] = {
    FixedArrayTypeDescriptorField("", DT_HOTBARBUTTONDATA, offsetof(PlayerSavedData, tHotbarBtnAssignments), NULL, 1, 6, 0),
    TypeDescriptorField("", DT_BYTE, offsetof(PlayerSavedData, bSkillSlotEverAssigned), NULL, 1),
    TypeDescriptorField("", DT_HOTBARBUTTONDATA, offsetof(PlayerSavedData, tPotionBtn), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedData, uSecondsPlayed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedData, m_ActivatedWaypoints), NULL, 1),
    TypeDescriptorField("", DT_HIRELING_SAVED_DATA, offsetof(PlayerSavedData, tHirelingSavedData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedData, uTimeLastLevel), NULL, 1),
    TypeDescriptorField("", DT_LEARNED_LORE, offsetof(PlayerSavedData, tLearnedLore), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_ACTIVE_SKILL_SAVED_DATA, offsetof(PlayerSavedData, tActiveSkills), NULL, 1, 6, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(PlayerSavedData, snoTraits), NULL, 0, 4, 0x1d, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(PlayerSavedData, gbidLegendaryPowers), "", 0, 3, 2, 0),
    TypeDescriptorField("", DT_SAVE_POINT_DATA, offsetof(PlayerSavedData, tLastSavePoint), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedData, uEventFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSavedData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSavedData("PlayerSavedData", 0, PlayerSavedData_Fields, 14, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_SAVED_DATA = &TypeDescriptor_PlayerSavedData;

static const TypeDescriptorField PlayerSavedDataMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerSavedDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_PLAYER_SAVED_DATA, offsetof(PlayerSavedDataMessage, tSavedData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSavedDataMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSavedDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSavedDataMessage("PlayerSavedDataMessage", 0, PlayerSavedDataMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_SAVED_DATA_MESSAGE = &TypeDescriptor_PlayerSavedDataMessage;

static const TypeDescriptorField PlayerSkillsMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerSkillsMessage, tHeader), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_ACTIVE_SKILL_SAVED_DATA, offsetof(PlayerSkillsMessage, tActiveSkills), NULL, 1, 6, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(PlayerSkillsMessage, snoTraits), NULL, 0, 4, 0x1d, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(PlayerSkillsMessage, gbidLegendaryPowers), "", 0, 3, 2, 0),
    TypeDescriptorField("", DT_INT, offsetof(PlayerSkillsMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSkillsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSkillsMessage("PlayerSkillsMessage", 0, PlayerSkillsMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_SKILLS_MESSAGE = &TypeDescriptor_PlayerSkillsMessage;

static const TypeDescriptorField AssignSkillMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(AssignSkillMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(AssignSkillMessage, snoPower), "", 0x1d, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(AssignSkillMessage, nRuneType), NULL, (const void *)-1, (const void *)4),
    BoundedTypeDescriptorField("", DT_INT, offsetof(AssignSkillMessage, nSlot), NULL, NULL, (const void *)0x5),
    TypeDescriptorField("", DT_INT, offsetof(AssignSkillMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AssignSkillMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AssignSkillMessage("AssignSkillMessage", 0, AssignSkillMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ASSIGN_SKILL_MESSAGE = &TypeDescriptor_AssignSkillMessage;

static const TypeDescriptorField AssignLegendaryPowerMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(AssignLegendaryPowerMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(AssignLegendaryPowerMessage, gbidLegendaryItem), "", 2, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(AssignLegendaryPowerMessage, nSlot), NULL, NULL, (const void *)0x2),
    TypeDescriptorField("", DT_INT, offsetof(AssignLegendaryPowerMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AssignLegendaryPowerMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AssignLegendaryPowerMessage("AssignLegendaryPowerMessage", 0, AssignLegendaryPowerMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ASSIGN_LEGENDARY_POWER_MESSAGE = &TypeDescriptor_AssignLegendaryPowerMessage;

static const TypeDescriptorField UnassignSkillMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(UnassignSkillMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(UnassignSkillMessage, nSlot), NULL, NULL, (const void *)0x5),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UnassignSkillMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UnassignSkillMessage("UnassignSkillMessage", 0, UnassignSkillMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_UNASSIGN_SKILL_MESSAGE = &TypeDescriptor_UnassignSkillMessage;

static const TypeDescriptorField SwapSkillsMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SwapSkillsMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SwapSkillsMessage, nSlot0), NULL, NULL, (const void *)0x5),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SwapSkillsMessage, nSlot1), NULL, NULL, (const void *)0x5),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SwapSkillsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SwapSkillsMessage("SwapSkillsMessage", 0, SwapSkillsMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SWAP_SKILLS_MESSAGE = &TypeDescriptor_SwapSkillsMessage;

static const TypeDescriptorField AssignTraitsMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(AssignTraitsMessage, tHeader), NULL, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(AssignTraitsMessage, snoPower), NULL, 0, 4, 0x1d, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AssignTraitsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AssignTraitsMessage("AssignTraitsMessage", 0, AssignTraitsMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ASSIGN_TRAITS_MESSAGE = &TypeDescriptor_AssignTraitsMessage;

static const TypeDescriptorField SpendParagonPointsMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SpendParagonPointsMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(SpendParagonPointsMessage, gbidParagonBonus), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SpendParagonPointsMessage, nPoints), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SpendParagonPointsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SpendParagonPointsMessage("SpendParagonPointsMessage", 0, SpendParagonPointsMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SPEND_PARAGON_POINTS_MESSAGE = &TypeDescriptor_SpendParagonPointsMessage;

static const TypeDescriptorField HeroDefinition_Fields[3] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(HeroDefinition, serProtoData), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(HeroDefinition, pProtoData), 0x20, -0xc),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HeroDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HeroDefinition("Hero", 474, HeroDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HERO = &TypeDescriptor_HeroDefinition;


HeroGroupDef * gptGroupDefHero;  // line 247

// line 253
HeroGroupDef::HeroGroupDef()
    : GroupDefinition("Hero", HERO_SNO,
                      SNOVersions(905, 905, 905, 905, 905),
                      SNODATATYPE_USERDATA, sizeof(HeroDefinition), DT_HERO, 0, 0x142e)
{
}

// line 274
uint32 HeroGroupDef::GetMaxCount()
{
    return 16;
}

// line 297
void HeroGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    SigmaMemoryClear(ptDefaultDef, sizeof(HeroDefinition));
}

// line 314
BOOL HeroGroupDef::Write(void * lpDefinition, FileReference & tRef) const
{
    return GroupDefinition::Write(lpDefinition, tRef);
}

// line 366
void HeroInitializeForApp()
{
    POS pos;
    PooledSNONameList listSNOFiles(GetSnoNameListPool());
    SNOFileEnumerateSNOsForGroup(HERO_SNO, &listSNOFiles);

    pos = listSNOFiles.GetHeadPosition();
    while (pos)
    {
        SNOName & snonameFile = listSNOFiles.GetNext(pos);

        gptGroupDefHero->DeleteIfVersionIsWrong(snonameFile.snoHandle);
    }
}

// line 384
void HeroDisposeForApp()
{
}

// line 480 (the DWARF does not record its line; it sits before the helpers that use it).
// Rows read from the original image (0x7100e54c10).
static const SavedAttribDef cgarHeroSaveAttribsDefs[27] = {
#include "../generated-game-data/hero_attribs.inc"
#line 360
};

// line 511
void HeroDefinitionSaveAttributes(ActorCommonData & tACDPlayer, D3::AttributeSerializer::SavedAttributes & tSavedAttributes)
{
    AttributeSerializer_Save(cgarHeroSaveAttribsDefs, sizeof(cgarHeroSaveAttribsDefs) / sizeof(cgarHeroSaveAttribsDefs[0]), tACDPlayer, tSavedAttributes);
}

// line 524
void HeroDefinitionRestoreAttributes(ActorCommonData & tACDPlayer, const D3::AttributeSerializer::SavedAttributes & tSavedAttributes)
{
    AttributeSerializer_Restore(cgarHeroSaveAttribsDefs, sizeof(cgarHeroSaveAttribsDefs) / sizeof(cgarHeroSaveAttribsDefs[0]), tSavedAttributes, tACDPlayer);

    SNO snoStoneOfRecallPower = GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL);
    BOOL fStoneOfRecallUnlocked = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoStoneOfRecallPower));
    tACDPlayer.SetStoneOfRecallUnlocked(fStoneOfRecallUnlocked);

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer && !(ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_RECEIVED_LOOT_FIND_BOOST)))
    {
        float flUnluckySecs = tACDPlayer.AttributesGetFloat(ATTRIBUTE_ITEM_UNLUCKY_BONUS_SECS);

        ScopedGlobalDefinitionLockR pdefGlobal;
        float flHoursMax = pdefGlobal->ptServerData->flUnluckyBonusMinHours;
        float flSecondsMax = flHoursMax * 3600.0f;
        float flMinSecs = flSecondsMax * 0.9f;
        flUnluckySecs = Math::Max(flUnluckySecs, flMinSecs);
        tACDPlayer.AttributesSetFloat(ATTRIBUTE_ITEM_UNLUCKY_BONUS_SECS, flUnluckySecs);

        ptPlayer->tHeroStateData.uPlayerFlags |= (1 << PLAYERFLAG_RECEIVED_LOOT_FIND_BOOST);
        tACDPlayer.MarkHeroStatePlayerFlagsChanged();
    }
}

// line 557
int32 HeroDefinitionSavedAttributeGetInt(const D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey, int32 nDefaultValue)
{
    return AttributeSerializer_GetInt(cgarHeroSaveAttribsDefs, sizeof(cgarHeroSaveAttribsDefs) / sizeof(cgarHeroSaveAttribsDefs[0]), tSavedAttributes, tKey, nDefaultValue);
}

// line 603
BOOL HeroDefinitionSavedAttributeOverwriteInt(D3::AttributeSerializer::SavedAttributes & tSavedAttributes, const FastAttribKey & tKey, const int32 nValue)
{
    return AttributeSerializer_OverwriteInt(cgarHeroSaveAttribsDefs, sizeof(cgarHeroSaveAttribsDefs) / sizeof(cgarHeroSaveAttribsDefs[0]), tSavedAttributes, tKey, nValue);
}

// line 619
void ActiveSkillSavedData::Export(D3::Hero::SkillWithRune & tSavedData) const
{

    tSavedData.set_sno_skill(snoSkill);
    tSavedData.set_rune_type(eRuneType);
}

// line 629
void ActiveSkillSavedData::Import(const D3::Hero::SkillWithRune & tSavedData)
{

    snoSkill = tSavedData.sno_skill();
    eRuneType = tSavedData.rune_type();
    dwLastLifeOnHitTick = 0;
    idChargesTimer = -1;
}

// line 640
void PlayerSavedData::SetDefaults()
{
    tPotionBtn.Empty();


    HirelingSavedDataSetDefaults(tHirelingSavedData);

    bSkillSlotEverAssigned = FALSE;
    uTimeLastLevel = 0;
    uEventFlags = 0;

    tLearnedLore.m_nLearnedCount = 0;
    tSavedConversations.Initialize();
    tLastSavePoint.Reset();


    for (int32 i = 0; i < 6; i++)
    {
        tActiveSkills[i].snoSkill = SNO_NONE;
        tActiveSkills[i].eRuneType = -1;
        tActiveSkills[i].dwLastLifeOnHitTick = 0;

        tActiveSkills[i].idChargesTimer = -1;



        tHotbarBtnAssignments[i].Empty();
    }


    for (int32 i = 0; i < 4; i++)
    {
        snoTraits[i] = SNO_NONE;
    }

    // The DWARF records one loop variable here; how the three legendary-power slots were reset is not
    // recoverable (their stores are merged with the trait stores), so they are plain assignments.
    gbidLegendaryPowers[0] = -1;
    gbidLegendaryPowers[1] = -1;
    gbidLegendaryPowers[2] = -1;
}

// line 681
static uint32 sGetClassSkillVersion(const GBID gbidHero)
{

    PlayerClass eClass = PlayerClassFromGBID(gbidHero);

    return scgdwSkillVersion[eClass];
}

// line 692
BOOL HeroSkillsAreOutOfDate(const GBID gbidHero, const D3::Hero::SavedData & tSavedData)
{
    uint32 nSkillVersionExpected = sGetClassSkillVersion(gbidHero);
    uint32 nSkillVersion = tSavedData.skill_version();

    return nSkillVersion != nSkillVersionExpected;
}

BOOL SPlayerIsEligibleForQuestState(const Player & tPlayer);  // X/Server/SPlayer.cpp; declared here to avoid pulling Server/SPlayer.h into this file

// line 704
void PlayerSavedData::Export(const Player & tPlayer, const GBID gbidHero, D3::Hero::SavedData & tSavedData, BOOL bConsoleClient) const
{
    int32 nSkillVersion = sGetClassSkillVersion(gbidHero);
    tSavedData.set_skill_version(nSkillVersion);

    tSavedData.set_gbid_potion_button(tPotionBtn.m_gbidItem);


    tSavedData.set_skill_slot_ever_assigned(&bSkillSlotEverAssigned, sizeof(bSkillSlotEverAssigned));

    tSavedData.set_time_played(uSecondsPlayed);
    tSavedData.set_event_flags(uEventFlags);

    if (GameActiveGameCommonDataIsServer())
    {
        SGame & tSGame = SGameManager::GameGet();
        if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY))
        {
            tSavedData.set_skill_kit_version(uSkillKitVersion);
        }
        else
        {
            tSavedData.set_skill_kit_version(gptGroupDefSkillKit->GetCurrentVersion(geSigmaPlatform));
        }
    }
    else
    {
        if (bConsoleClient)
        {
            tSavedData.set_skill_kit_version(gptGroupDefSkillKit->GetCurrentVersion(geSigmaPlatform));
        }
        else
        {
            tSavedData.set_skill_kit_version(uSkillKitVersion);
        }
    }

    if (QuestIsGameOver())
    {

        tSavedData.set_activated_waypoints(0);

        SavePointData tSavePoint;
        tSavePoint.Reset();
        tSavePoint.Export(*tSavedData.mutable_save_point());
    }
    else if (bConsoleClient &&

             GameIsOpenWorld())
    {


        if (!tSavedData.has_activated_waypoints())
        {
            tSavedData.set_activated_waypoints(D3::Hero::SavedData::default_instance().activated_waypoints());
        }
        if (!tSavedData.has_save_point())
        {
            SavePointData tSavePoint;
            tSavePoint.Reset();
            tSavePoint.Export(*tSavedData.mutable_save_point());
        }
    }
    else
    {
        if (!bConsoleClient && !SPlayerIsEligibleForQuestState(tPlayer))
        {


            tSavedData.set_activated_waypoints(tPlayer.tPlayerResumeStateUponJoining.uActivatedWaypoints);
            tPlayer.tPlayerResumeStateUponJoining.tLastSavePoint.Export(*tSavedData.mutable_save_point());
        }
        else
        {

            tSavedData.set_activated_waypoints(m_ActivatedWaypoints);
            tLastSavePoint.Export(*tSavedData.mutable_save_point());
        }
    }

    tHirelingSavedData.Export(*tSavedData.mutable_hireling_saved_data());

    tSavedData.set_last_level_time(uTimeLastLevel);

    tLearnedLore.Export(*tSavedData.mutable_learned_lore());

    if (!bConsoleClient)
    {

        tSavedConversations.Export(*tSavedData.mutable_saved_conversations());
    }
    else if (!tSavedData.has_saved_conversations())
    {

        tSavedData.mutable_saved_conversations()->set_played_conversations_bitfield("");
    }

    tSavedData.clear_active_skills();
    for (int32 i = 0; i < 6; i++)
    {
        tActiveSkills[i].Export(*tSavedData.add_active_skills());
    }

    tSavedData.clear_sno_traits();
    for (int32 i = 0; i < 4; i++)
    {
        tSavedData.add_sno_traits(snoTraits[i]);
    }

    tSavedData.clear_gbid_legendary_powers();
    for (int32 i = 0; i < 3; i++)
    {
        tSavedData.add_gbid_legendary_powers(gbidLegendaryPowers[i]);
    }
}

// line 831
void PlayerSavedData::Import(const D3::Hero::SavedData & tSavedData)
{


    const blz::string & strData = tSavedData.skill_slot_ever_assigned();

    if (strData.length() > 0)
    {
        SigmaMemoryCopy(&bSkillSlotEverAssigned, strData.data(), sizeof(bSkillSlotEverAssigned));
    }

    uSecondsPlayed = tSavedData.time_played();
    uEventFlags = tSavedData.event_flags();

    m_ActivatedWaypoints = tSavedData.activated_waypoints();

    tHirelingSavedData.Import(tSavedData.hireling_saved_data());

    uTimeLastLevel = tSavedData.last_level_time();
    uSkillKitVersion = tSavedData.skill_kit_version();

    tLearnedLore.Import(tSavedData.learned_lore());
    tSavedConversations.Import(tSavedData.saved_conversations());

    for (int32 i = 0; i < 6 && i < tSavedData.active_skills_size(); i++)
    {
        tActiveSkills[i].Import(tSavedData.active_skills(i));
        tHotbarBtnAssignments[i].SetSkill(tActiveSkills[i].snoSkill, tActiveSkills[i].eRuneType);
    }

    for (int32 i = 0; i < 4 && i < tSavedData.sno_traits_size(); i++)
    {
        snoTraits[i] = tSavedData.sno_traits(i);
    }

    for (int32 i = 0; i < 3 && i < tSavedData.gbid_legendary_powers_size(); i++)
    {
        gbidLegendaryPowers[i] = tSavedData.gbid_legendary_powers(i);
    }

    tLastSavePoint.Import(tSavedData.save_point());
}

// line 1061
void HeroDefinition_Init(D3::Hero::SavedDefinition & tSavedDefinition)
{
    tSavedDefinition.Clear();

    D3::Hero::Digest & tDigest = *tSavedDefinition.mutable_digest();
    tDigest.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));
    tDigest.set_hero_name("Default");
    tDigest.set_gbid_class(-1);
    tDigest.set_level(1);
    tDigest.set_highest_unlocked_act(0);
    tDigest.set_last_played_quest(-1);
    tDigest.set_last_played_quest_step(-1);
    tDigest.set_player_flags(0);
    VisualEquipment tVisEquip;
    tVisEquip.Init();
    tVisEquip.Export(*tDigest.mutable_visual_equipment());
    tDigest.set_time_played(0);
    tDigest.set_last_played_act(0);

    tDigest.mutable_hero_id()->set_id_high(0);
    tDigest.mutable_hero_id()->set_id_low(0);

    tSavedDefinition.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));
    tSavedDefinition.mutable_saved_attributes()->Clear();

    GBHandle gbHandle(7, -1);  // the two stored values (7, -1) are observed; how the local was initialised is not recorded
    gbHandle.Export(*tSavedDefinition.mutable_saved_attributes()->mutable_gb_handle());
}
