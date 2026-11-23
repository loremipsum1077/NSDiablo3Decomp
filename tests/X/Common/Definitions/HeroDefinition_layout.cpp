// Layout check for X/Common/Definitions/HeroDefinition.h against the DWARF (byte_size and
// data_member_location of every struct in it). Compiled, not compared: it defines no function
// the binary has.
#include "X/Common/Definitions/HeroDefinition.h"

#include <cstddef>

#define CHECK(T, m, off) static_assert(offsetof(T, m) == off, #T "::" #m)

static_assert(sizeof(PlayerSavedQuest) == 0xa8, "PlayerSavedQuest");
CHECK(PlayerSavedQuest, snoQuest, 0x00);
CHECK(PlayerSavedQuest, nCurrentStepUID, 0x04);
CHECK(PlayerSavedQuest, arObjectiveState, 0x08);
CHECK(PlayerSavedQuest, arFailureConditionState, 0x58);

static_assert(sizeof(PlayerSavedQuestUpdateMessage) == 0x2a1c, "PlayerSavedQuestUpdateMessage");
CHECK(PlayerSavedQuestUpdateMessage, eLastPlayedAct, 0x08);
CHECK(PlayerSavedQuestUpdateMessage, snoLastPlayedQuest, 0x0c);
CHECK(PlayerSavedQuestUpdateMessage, nLastPlayedStepUID, 0x10);
CHECK(PlayerSavedQuestUpdateMessage, nHandicapLevel, 0x14);
CHECK(PlayerSavedQuestUpdateMessage, tSavedQuest, 0x18);
CHECK(PlayerSavedQuestUpdateMessage, dwNumQuests, 0x2a18);

static_assert(sizeof(PlayerQuestHistoryEntry) == 0x8, "PlayerQuestHistoryEntry");
CHECK(PlayerQuestHistoryEntry, nStepUID, 0x04);
static_assert(sizeof(PlayerQuestHistoryUpdateMessage) == 0x64c, "PlayerQuestHistoryUpdateMessage");
CHECK(PlayerQuestHistoryUpdateMessage, tQuestHistory, 0x08);
CHECK(PlayerQuestHistoryUpdateMessage, dwNumEntries, 0x648);

static_assert(sizeof(PlayerQuestRewardHistoryEntry) == 0xc, "PlayerQuestRewardHistoryEntry");
CHECK(PlayerQuestRewardHistoryEntry, nStepUID, 0x04);
CHECK(PlayerQuestRewardHistoryEntry, uDifficultyFlags, 0x08);

static_assert(sizeof(HirelingInfo) == 0x20, "HirelingInfo");
CHECK(HirelingInfo, gbidName, 0x04);
CHECK(HirelingInfo, fDead, 0x08);
CHECK(HirelingInfo, arnPowerKeyParam, 0x0c);
static_assert(sizeof(HirelingSavedData) == 0x88, "HirelingSavedData");
CHECK(HirelingSavedData, eActiveHireling, 0x80);
CHECK(HirelingSavedData, dwAvailableHirelings, 0x84);

static_assert(sizeof(LearnedLore) == 0x804, "LearnedLore");
CHECK(LearnedLore, m_snoLoreLearned, 0x04);

static_assert(sizeof(SavedConversations) == 0x90c, "SavedConversations");
CHECK(SavedConversations, m_arPlayedConv, 0x04);
CHECK(SavedConversations, m_snoSavedConv, 0x4c);
static_assert(sizeof(PlayerSavedConversationsUpdateMessage) == 0x914, "PlayerSavedConversationsUpdateMessage");
CHECK(PlayerSavedConversationsUpdateMessage, tSavedConversations, 0x08);

static_assert(sizeof(SavePointData) == 0x10, "SavePointData");
CHECK(SavePointData, nNum, 0x04);
CHECK(SavePointData, bCreatesPortal, 0x08);
CHECK(SavePointData, idACDReturnPortal, 0x0c);
static_assert(sizeof(SavePointDataMessage) == 0x1c, "SavePointDataMessage");
CHECK(SavePointDataMessage, tData, 0x08);
CHECK(SavePointDataMessage, nPlayerIndex, 0x18);

static_assert(sizeof(ActiveSkillSavedData) == 0x10, "ActiveSkillSavedData");
CHECK(ActiveSkillSavedData, eRuneType, 0x04);
CHECK(ActiveSkillSavedData, dwLastLifeOnHitTick, 0x08);
CHECK(ActiveSkillSavedData, idChargesTimer, 0x0c);

static_assert(sizeof(PlayerSavedData) == 0x12ac, "PlayerSavedData");
CHECK(PlayerSavedData, tPotionBtn, 0x60);
CHECK(PlayerSavedData, bSkillSlotEverAssigned, 0x70);
CHECK(PlayerSavedData, uSecondsPlayed, 0x74);
CHECK(PlayerSavedData, m_ActivatedWaypoints, 0x78);
CHECK(PlayerSavedData, tHirelingSavedData, 0x7c);
CHECK(PlayerSavedData, uTimeLastLevel, 0x104);
CHECK(PlayerSavedData, tLearnedLore, 0x108);
CHECK(PlayerSavedData, tSavedConversations, 0x90c);
CHECK(PlayerSavedData, tActiveSkills, 0x1218);
CHECK(PlayerSavedData, snoTraits, 0x1278);
CHECK(PlayerSavedData, gbidLegendaryPowers, 0x1288);
CHECK(PlayerSavedData, tLastSavePoint, 0x1294);
CHECK(PlayerSavedData, uEventFlags, 0x12a4);
CHECK(PlayerSavedData, uSkillKitVersion, 0x12a8);

static_assert(sizeof(PlayerSavedDataMessage) == 0x12b8, "PlayerSavedDataMessage");
CHECK(PlayerSavedDataMessage, tSavedData, 0x08);
CHECK(PlayerSavedDataMessage, nPlayerIndex, 0x12b4);

static_assert(sizeof(PlayerSkillsMessage) == 0x88, "PlayerSkillsMessage");
CHECK(PlayerSkillsMessage, tActiveSkills, 0x08);
CHECK(PlayerSkillsMessage, snoTraits, 0x68);
CHECK(PlayerSkillsMessage, gbidLegendaryPowers, 0x78);
CHECK(PlayerSkillsMessage, nPlayerIndex, 0x84);

static_assert(sizeof(LastWaypointLevelAreaDataMessage) == 0x10, "LastWaypointLevelAreaDataMessage");
CHECK(LastWaypointLevelAreaDataMessage, nPlayerIndex, 0x0c);
static_assert(sizeof(AssignSkillMessage) == 0x18, "AssignSkillMessage");
CHECK(AssignSkillMessage, nPlayerIndex, 0x14);
static_assert(sizeof(AssignLegendaryPowerMessage) == 0x14, "AssignLegendaryPowerMessage");
CHECK(AssignLegendaryPowerMessage, nPlayerIndex, 0x10);
static_assert(sizeof(UnassignSkillMessage) == 0xc, "UnassignSkillMessage");
static_assert(sizeof(SwapSkillsMessage) == 0x10, "SwapSkillsMessage");
CHECK(SwapSkillsMessage, nSlot1, 0x0c);
static_assert(sizeof(AssignTraitsMessage) == 0x18, "AssignTraitsMessage");
static_assert(sizeof(SpendParagonPointsMessage) == 0x10, "SpendParagonPointsMessage");
CHECK(SpendParagonPointsMessage, nPoints, 0x0c);

static_assert(sizeof(HeroDefinition) == 0x20, "HeroDefinition");
CHECK(HeroDefinition, serProtoData, 0x0c);
CHECK(HeroDefinition, pProtoData, 0x18);
static_assert(sizeof(HeroGroupDef) == 0xa0, "HeroGroupDef");
