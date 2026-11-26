// Original file: X/Common/Player.cpp

#include "Player.h"
#include "PlayerSwig.h"
#include "GameSwig.h"
#include "GlobalSNO.h"
#include "Powers/PowerAPISwig.h"
#include "../Sigma/RefString.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "ActorCommonData.h"
#include "HotbarButton.h"
#include "Items.h"
#include "Definitions/GameBalanceDefinition.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Server/Timers.h"
#include "Game.h"
#include "GameCommonData.h"
#include "World.h"
#include "Definitions/FlagSetDefinition.h"
#include "Attributes/FastAttribute.h"
#include "Shell/CmdLine.h"
#include "../Client/CPlayer.h"
#include "../Client/CGame.h"
#include "../Client/Console/GamerProfile.h"
#include "../Sigma/Math/MathGeneral.h"
#include "Definitions/QuestDefinition.h"
#include "Definitions/GlobalDefinition.h"
#include "Quests/QuestsSwig.h"
#include "GameTemplate.h"
#include "ActorInventory.h"
#include "ItemsSwig.h"
#include "Crafting/ItemCraftingSwig.h"
#include "../Server/SGame.h"
#include "../Sigma/Random.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "Powers/PowersCommon.h"
#include <float.h>
#include "../Client/Console/ControllerUtils.h"
#include "../Server/Actors/Actors.h"
#include "Definitions/SkillKitDefinition.h"
#include "../Sigma/Math/Intersect.h"
#include "Definitions/StringListDefinition.h"
#include "Networking/GameMessage.h"
#include "NephalemRiftCommon.h"
#include "../Server/PlayerList.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/Tags.h"
#include "../Client/Definitions/PreferencesDefinition.h"
struct GameHandicapLevelLimitation {  // size 0x10
    HandicapLevelLimitation m_privateGameLimitation;  // 0x00
    HandicapLevelLimitation m_publicGameLimitation;  // 0x08
};
#include "../Client/UserInterface/Console/UIConsoleNewItemIndicators.h"
namespace NephalemRiftManager {  // X/Server/System/SNephalemRift.h
BOOL IsTieredRiftActive();
BOOL PlayerPaidForRift(const Player * ptPlayer);
}
// Declaration placement inferred; signature from DWARF PowersCommon.cpp:4470.
int32 CountSetBits(const uint8 * lpb, const int32 nLen);  // X/Sigma/BitManip.cpp:87
Player * ACDGetControllingPlayer(const ACDID idACD);  // Player.cpp:1394
void PlayerRemovePet(Player & tPlayer, ACDID idACDPet, BOOL bDisplayChatMessage);  // Player.cpp:2393
BOOL PlayerAssignBaseHealthPotion(Player * ptPlayer);  // Player.cpp:1703
void PlayerCrafterInitialize(CrafterData & tCrafterData);  // Player.cpp:3323
void PlayerSavedQuestInitialize(PlayerSavedQuest & tSavedQuest);  // Player.cpp:3291
void PlayerQuestHistoryEntryInitialize(PlayerQuestHistoryEntry & tEntry);  // Player.cpp:3302
void PlayerQuestRewardHistoryEntryInitialize(PlayerQuestRewardHistoryEntry & tEntry);  // Player.cpp:3312
void QuestsVersionForPlayer(HeroStateData & tStateData);  // X/Common/Quests/Quests.cpp:1528
ACDID PlayerGetNextPet(ACDID idACDPlayer, PetType eType, int32 & nPos);  // Player.cpp:2600
SNO PowerGetNormalAttackReplacement(ACDID idACD);  // X/Common/Powers/PowersCommon.cpp:2206
void PowerAPI_SetNormalAttackReplacement(ACDID idACD, SNO snoPower);  // X/Server/Powers/PowerAPI.cpp:12663
BOOL ItemCanBeAssignedToUsableItemButton(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
void PlayerSetPotionBtnAssignment(Player * ptPlayer, const HotbarButtonData & tButtonData);  // Player.cpp:1761
// Declaration placement inferred; signature from DWARF CPlayerInput.cpp:6603.
uint64 ItemGenerateConsoleEquipmentManagerID(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:11269
struct BufferedInput;
namespace Controller {  // X/Client/Definitions/ControllerSchemeDefinition.h
BOOL InputIsTownPortal(const BufferedInput & tInput);
int32 GetSlotFromInput(const BufferedInput & tInput);
}
typedef BOOL (*SlotSearchCallback)(const InventoryLocation &, void *);  // X/Common/ActorInventory.h:189
void ACDInventoryIterateSlot(const ActorCommonData & tACD, InventorySlotType eSlot, const IRect2D * ptRect, SlotSearchCallback pfnCallBack, void * pCallBackData, BOOL fRightLeft, BOOL fBottomUp);  // X/Common/ActorInventory.cpp:2490
SNO GetTargetLockSNOForPlayer(const Player * ptPlayer);  // X/Client/CPlayerInput.cpp
Player * TradeGetTradingTarget(const Player & tPlayer);  // X/Common/Trade.cpp
void STradeTriggerUpdate(Player & tPlayer, BOOL bUpdate);  // X/Server/STrade.cpp
BOOL ActorFlagIsSet(const ActorID idActor, const ActorFlag eFlag);  // X/Server/Actors/Actors.cpp
BOOL HeroIsRanged(ACDID idACDPlayer);  // X/Common/Hero.cpp
// Declaration placement inferred; signature from DWARF ActorCommonData.cpp:8206.
BOOL ACDIsFollower(const ActorCommonData & tACD);
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower);
// X/Common/Player.h (DWARF layouts); not in src's Player.h subset yet, kept local.
#include "../Client/CPlayerInput.h"
// Declaration placement inferred; signatures: PowersCommon.cpp:1775 and CPlayerUtil.cpp:829.
BOOL PowerAutoTargets(AutoPowerDefinitionR pdefPower, const Player * ptPlayer);
GameError CPlayerUtilResolveConsoleButton(const SNO snoPowerOnButton, ResolvedClick & tResolvedClick, const Player * ptPlayer, BOOL bHeldButton, BOOL bCheckForHostiles, BOOL bCheckForOperatableGizmos);
extern const InventorySlotType cgarPlayerLoadoutSocketableSlotsOrder[9];  // Player.cpp:173; data at 0x7100e49988.
#include "../Client/Targeting/ConsoleTargeting.h"
// Declaration placement inferred from DWARF ConsoleTargeting.cpp:1112; original header declaration line unknown.
namespace Targeting { namespace Console { void GetTargetingInformation(const TargetingInformationInput & input, TargetingInformationOutput & output); } }
#include "../Client/Network/CTrickle.h"
struct StashIconStateMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    uint8 bStashIcons[13];  // 0x08
};
struct LogoutContextMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    int32 eLogoutContext;  // 0x08 (LogoutContext)
};
extern const XTypeDescriptor * DT_HERO_STATE_DATA;  // line 242 below
extern const XTypeDescriptor * DT_PLAYER_SAVED_DATA;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_QUEST_REWARD_HISTORY_ENTRY;
CursorMgr & PlayerGetCursorMgr();
struct PlayerLoadoutEquipmentUpdateMessage { RequiredMessageHeader tHeader; int32 nLoadoutIndex; int32 nEquipmentIndex; LoadoutItemData tNewItem; };  static_assert(sizeof(PlayerLoadoutEquipmentUpdateMessage) == 0x40, "PlayerLoadoutEquipmentUpdateMessage");
struct PlayerLoadoutSaveMessage { RequiredMessageHeader tHeader; int32 nLoadoutIndex; CHAR usName[53]; };  static_assert(sizeof(PlayerLoadoutSaveMessage) == 0x44, "PlayerLoadoutSaveMessage");
struct PlayerLoadoutSetSlotMessage { RequiredMessageHeader tHeader; int32 nLoadoutIndex; InventorySlotType eSlot; ACDNetworkName annItem; };  static_assert(sizeof(PlayerLoadoutSetSlotMessage) == 0x14, "PlayerLoadoutSetSlotMessage");
// Declaration placement inferred; signature from DWARF Tutorials.cpp:181.
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); }
namespace OnlineService { extern const XTypeDescriptor * DT_GAME_ACCOUNT_HANDLE; }  // X/Common/OnlineService/OnlineService.cpp
#include "Protobuf/GameMessage.pb.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
BOOL ItemRemoveSocketedItemAttributes(ActorCommonData & tACDItemToSocket, const ActorCommonData & tACDItemSocketedInto);  // Items.cpp:4681
void ACDInventoryRemoveToLimbo(const ACDID idACDContainer, ActorCommonData & tACDItem, BOOL bRemoveFromKnownList);  // ActorInventory.cpp:3228
DataID SACDInventoryAddToDirtyList(const ActorCommonData & tACDItem, ActorInventoryDatabaseAction eAction, InventorySlotType eCameFrom, InventorySlotType eDestination);  // ActorInventory.cpp:11891
int32 ItemCrafting_GetGemQuality(const GBID gbidItem);  // Crafting/ItemCrafting.cpp:2454
BOOL PlayerLoadoutSocketablesMatch(GBID gbidItem1, GBID gbidItem2);  // Player.cpp:4832
// Same-file declarations for genuine later definitions at 4092 and 4177.
BOOL PlayerCanChangeLoadoutSkillsAndTraits(const Player & tPlayer, int32 nLoadoutIndex); BOOL PlayerLoadoutMatchesCurrentEquipment(const Player & tPlayer, int32 nLoadoutIndex);
static const GameHandicapLevelLimitation scgarGameHandicapLevelLimitations[17] =
{
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 0, 0 }, { 0, 0 } },
    { { 70, 0 }, { 70, 30 } },
    { { 70, 0 }, { 70, 35 } },
    { { 70, 0 }, { 70, 40 } },
    { { 70, 0 }, { 70, 45 } },
    { { 70, 0 }, { 70, 50 } },
    { { 70, 0 }, { 70, 55 } },
    { { 70, 0 }, { 70, 60 } },
};
// Inference: optimized-away array values follow the 15 calls at original Player.cpp:4433.
static const InventorySlotType scarLoadoutEquipmentSlots[15] = {
    SLOT_PLAYER_BACKPACK, SLOT_PLAYER_SHARED_STASH, SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO,
    SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS, SLOT_PLAYER_WAIST,
    SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS,
    SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_NECK
};


// Lines 189-340: type descriptors (_GLOBAL__sub_I_Player.cpp), generated with build/try/solo/desc.py; field names are "" in the binary.
static const TypeDescriptorField GameErrorData_Fields[3] = {
    BoundedTypeDescriptorField("", DT_INT, offsetof(GameErrorData, eError), NULL, NULL, (const void *)0xc3),
    TypeDescriptorField("", DT_INT, offsetof(GameErrorData, dwVal), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameErrorData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GameErrorData("GameErrorData", 0, GameErrorData_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GAME_ERROR_DATA = &TypeDescriptor_GameErrorData;

static const TypeDescriptorField NewPlayerMessage_Fields[23] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(NewPlayerMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, nPlayerIndex), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT64, offsetof(NewPlayerMessage, uHeroId), NULL, 1),
    TypeDescriptorField("", OnlineService::DT_GAME_ACCOUNT_HANDLE, offsetof(NewPlayerMessage, tAccountId), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(NewPlayerMessage, usHeroName), "", 1, 0x31, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, eTeam), NULL, (const void *)-1, (const void *)0x16),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, eClass), NULL, (const void *)-1, (const void *)0x6),
    FilterTypeDescriptorField("", DT_SNO, offsetof(NewPlayerMessage, snoActorPortrait), "", ACTOR_SNO, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, nCurrentLevel), NULL, NULL, (const void *)0x46),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, nCurrentAltLevel), NULL, NULL, (const void *)0x4e20),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, nCurrentHighestHeroSoloRiftLevel), NULL, NULL, (const void *)0x1ff),
    TypeDescriptorField("", DT_HERO_STATE_DATA, offsetof(NewPlayerMessage, tHeroStateData), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, bJustJoined), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, idGameConnection), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, ann), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(NewPlayerMessage, usGamerTag), "", 1, 0x80, -1),
    TypeDescriptorField("", DT_INT64, offsetof(NewPlayerMessage, uAcceptedLicenseBitsHero), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, bUpscaled), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, uPartitionFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(NewPlayerMessage, uSeasonCreated), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(NewPlayerMessage, nNFPDefaultScanTime), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(NewPlayerMessage, nNFPGoblinScanTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(NewPlayerMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_NewPlayerMessage("NewPlayerMessage", 0, NewPlayerMessage_Fields, 23, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_NEW_PLAYER_MESSAGE = &TypeDescriptor_NewPlayerMessage;

static const TypeDescriptorField PlayerKickTimerMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerKickTimerMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerKickTimerMessage, dwLastKickAttempt), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerKickTimerMessage, dwLastSuccessfulKick), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerKickTimerMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerKickTimerMessage("PlayerKickTimerMessage", 0, PlayerKickTimerMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAYER_KICK_TIMER_MESSAGE = &TypeDescriptor_PlayerKickTimerMessage;

static const TypeDescriptorField HeroStateData_Fields[8] = {
    TypeDescriptorField("", DT_INT, offsetof(HeroStateData, eLastPlayedAct), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HeroStateData, eHighestUnlockedAct), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HeroStateData, uPlayerFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HeroStateData, uPlayerFlags2), NULL, 1),
    TypeDescriptorField("", DT_PLAYER_SAVED_DATA, offsetof(HeroStateData, tSavedData), NULL, 1),
    FixedArrayTypeDescriptorField("tQuestRewardHistory", DT_QUEST_REWARD_HISTORY_ENTRY, offsetof(HeroStateData, tQuestRewardHistory), NULL, 8, 100, -4),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(HeroStateData, uBossKillFlags), NULL, 1, 6, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HeroStateData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HeroStateData("HeroStateData", 0, HeroStateData_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HERO_STATE_DATA = &TypeDescriptor_HeroStateData;

static const TypeDescriptorField HeroStateMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HeroStateMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_HERO_STATE_DATA, offsetof(HeroStateMessage, tStateData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HeroStateMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HeroStateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HeroStateMessage("HeroStateMessage", 0, HeroStateMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HERO_STATE_MESSAGE = &TypeDescriptor_HeroStateMessage;

static const TypeDescriptorField PlayerQuestRewardHistoryUpdateMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerQuestRewardHistoryUpdateMessage, tHeader), NULL, 1),
    FixedArrayTypeDescriptorField("tQuestRewardHistory", DT_QUEST_REWARD_HISTORY_ENTRY, offsetof(PlayerQuestRewardHistoryUpdateMessage, tQuestRewardHistory), NULL, 8, 100, -4),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerQuestRewardHistoryUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerQuestRewardHistoryUpdateMessage("PlayerQuestRewardHistoryUpdateMessage", 0, PlayerQuestRewardHistoryUpdateMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_QUEST_REWARD_HISTORY_UPDATE_MESSAGE = &TypeDescriptor_PlayerQuestRewardHistoryUpdateMessage;

static const TypeDescriptorField StashIconStateMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(StashIconStateMessage, tHeader), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_BYTE, offsetof(StashIconStateMessage, bStashIcons), NULL, 1, 0xd, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(StashIconStateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_StashIconStateMessage("StashIconStateMessage", 0, StashIconStateMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_STASH_ICON_STATE_MESSAGE = &TypeDescriptor_StashIconStateMessage;

static const TypeDescriptorField LogoutContextMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(LogoutContextMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(LogoutContextMessage, eLogoutContext), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LogoutContextMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LogoutContextMessage("LogoutContextMessage", 0, LogoutContextMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOGOUT_CONTEXT_MESSAGE = &TypeDescriptor_LogoutContextMessage;

static const TypeDescriptorField LoadoutSkillData_Fields[3] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(LoadoutSkillData, snoPower), "", POWER_SNO, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(LoadoutSkillData, eRune), NULL, (const void *)-1, (const void *)4),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LoadoutSkillData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LoadoutSkillData("LoadoutSkillData", 0, LoadoutSkillData_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOADOUT_SKILL_DATA = &TypeDescriptor_LoadoutSkillData;

static const TypeDescriptorField LoadoutItemData_Fields[6] = {
    TypeDescriptorField("", DT_INT, offsetof(LoadoutItemData, annItem), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(LoadoutItemData, uOriginalItemIdLow), NULL, 1),
    FixedArrayTypeDescriptorField("gbidSockets", DT_INT, offsetof(LoadoutItemData, gbidSockets), NULL, 8, 3, 0xc),
    TypeDescriptorField("", DT_INT, offsetof(LoadoutItemData, unk_20), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(LoadoutItemData, uSoulShardIdLow), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LoadoutItemData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LoadoutItemData("LoadoutItemData", 0, LoadoutItemData_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOADOUT_ITEM_DATA = &TypeDescriptor_LoadoutItemData;

static const TypeDescriptorField LoadoutPotionData_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(LoadoutPotionData, annPotion), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(LoadoutPotionData, uOriginalItemIdLow), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LoadoutPotionData, gbidPotion), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LoadoutPotionData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LoadoutPotionData("LoadoutPotionData", 0, LoadoutPotionData_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LOADOUT_POTION_DATA = &TypeDescriptor_LoadoutPotionData;

static const TypeDescriptorField PlayerLoadoutDataMessage_Fields[10] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerLoadoutDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutDataMessage, nLoadoutIndex), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_LOADOUT_ITEM_DATA, offsetof(PlayerLoadoutDataMessage, tEquipment), NULL, 1, 0xd, 0),
    TypeDescriptorField("", DT_LOADOUT_POTION_DATA, offsetof(PlayerLoadoutDataMessage, tEquippedPotion), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_LOADOUT_SKILL_DATA, offsetof(PlayerLoadoutDataMessage, tActiveSkills), NULL, 1, 6, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(PlayerLoadoutDataMessage, snoTraits), NULL, 0, 4, POWER_SNO, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(PlayerLoadoutDataMessage, gbidLegendaryPowers), "", 0, 4, 2, 0),
    CharArrayTypeDescriptorField("", offsetof(PlayerLoadoutDataMessage, usName), "", 1, 0x35, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutDataMessage, nTabIcon), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerLoadoutDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerLoadoutDataMessage("PlayerLoadoutDataMessage", 0, PlayerLoadoutDataMessage_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAYER_LOADOUT_DATA_MESSAGE = &TypeDescriptor_PlayerLoadoutDataMessage;

static const TypeDescriptorField PlayerLoadoutEquipmentUpdateMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerLoadoutEquipmentUpdateMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutEquipmentUpdateMessage, nLoadoutIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutEquipmentUpdateMessage, nEquipmentIndex), NULL, 1),
    TypeDescriptorField("", DT_LOADOUT_ITEM_DATA, offsetof(PlayerLoadoutEquipmentUpdateMessage, tNewItem), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerLoadoutEquipmentUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerLoadoutEquipmentUpdateMessage("PlayerLoadoutEquipmentUpdateMessage", 0, PlayerLoadoutEquipmentUpdateMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAYER_LOADOUT_EQUIPMENT_UPDATE_MESSAGE = &TypeDescriptor_PlayerLoadoutEquipmentUpdateMessage;

static const TypeDescriptorField PlayerLoadoutSaveMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerLoadoutSaveMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutSaveMessage, nLoadoutIndex), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(PlayerLoadoutSaveMessage, usName), "", 1, 0x35, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerLoadoutSaveMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerLoadoutSaveMessage("PlayerLoadoutSaveMessage", 0, PlayerLoadoutSaveMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAYER_LOADOUT_SAVE_MESSAGE = &TypeDescriptor_PlayerLoadoutSaveMessage;

static const TypeDescriptorField PlayerLoadoutSetSlotMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerLoadoutSetSlotMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutSetSlotMessage, nLoadoutIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutSetSlotMessage, eSlot), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutSetSlotMessage, annItem), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerLoadoutSetSlotMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerLoadoutSetSlotMessage("PlayerLoadoutSetSlotMessage", 0, PlayerLoadoutSetSlotMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAYER_LOADOUT_SET_SLOT_MESSAGE = &TypeDescriptor_PlayerLoadoutSetSlotMessage;

static const TypeDescriptorField PlayerLoadoutTabIconMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerLoadoutTabIconMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutTabIconMessage, nLoadoutIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutTabIconMessage, nTabIcon), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerLoadoutTabIconMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerLoadoutTabIconMessage("PlayerLoadoutTabIconMessage", 0, PlayerLoadoutTabIconMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAYER_LOADOUT_TAB_ICON_MESSAGE = &TypeDescriptor_PlayerLoadoutTabIconMessage;

static const TypeDescriptorField PlayerLoadoutEquipResultsMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerLoadoutEquipResultsMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerLoadoutEquipResultsMessage, bSocketsChanged), NULL, NULL, (const void *)0x1),
    FixedArrayTypeDescriptorField("arRemovedItemANNs", DT_INT, offsetof(PlayerLoadoutEquipResultsMessage, arRemovedItemANNs), NULL, 8, 0x23, 0x8c),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerLoadoutEquipResultsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerLoadoutEquipResultsMessage("PlayerLoadoutEquipResultsMessage", 0, PlayerLoadoutEquipResultsMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAYER_LOADOUT_EQUIP_RESULTS_MESSAGE = &TypeDescriptor_PlayerLoadoutEquipResultsMessage;

static XVarScope unk_1916900("Paragon", NULL);
XVarUint32 unk_19167f8(unk_1916900, "MaxParagonLevelForDistributedPointAllocation_Capped", 800);


















extern const int32 cgnActiveSkillSlotLevelReq[6] = { 1, 2, 4, 9, 14, 19 };
extern const int32 cgnTraitSlotLevelReq[4] = { 10, 20, 30, 70 };







void KillCounter::Reset()
{
    uTotalCount = 0;
    uCountGrantingXP = 0;
    uLastTick = 0;
    bNewRecord = FALSE;
    uStartTime = 0;
    bExpired = FALSE;
    nTotalXP = 0;
    uLastMessageSentTime = 0;
    uLastMessageCount = 0;
    uMassacreTier = 0;
    bStartingTimerActive = FALSE;
    uPreviousLastTick = 0;
    uLastUpdate = 0;
}



void CurrencyCounter::Reset()
{
    uCount = 0;
    uLastCount = 0;
    uLastTick = 0;
    uLastUpdate = 0;
}







// Original include position unknown; PlayerSetDefaults below uses TEAM_INVALID, GAMECONNECTIONID_INVALID and HC_NONE; PlayersInitializeGameCommonData uses MemoryPoolsGetDATAIDListPool.
#include "../Sigma/MemoryPools.h"
#include "ActorCommonDataSwig.h"
#include "Networking/Network.h"
#include "HirelingSwig.h"


// PlayerStats::SetDefaults: DWARF Player.cpp:398, uNow at :401, loop counters at :410 and :416, KillCounter::Reset
// inlined at :418 (cu_04439fcf). Only its PlayerSetDefaults inline copy survives (memsets at :412 and :357).
// The gold and platinum CurrencyCounters keep their values: no store reaches them in the original.

void PlayerStats::SetDefaults()
{

    uint32 uNow = GameGetServerTime();

    m_uStartTickForPlaySession = uNow;
    m_uGoldSpent = 0;
    m_uGoldObtainedThroughSelling = 0;
    m_flTotalHealthPickedUp = 0.0f;
    m_flMaxLumpOfHealthPickedUp = 0.0f;
    m_uTownPortalsUsed = 0;

    for (int32 i = 0; i < NUM_CURRENCY_TYPES; i++)
    {
        m_uTotalCurrencyPickedUp[i] = 0;
        m_uMaxLumpOfCurrencyPickedUp[i] = 0;
    }

    for (int32 i = 0; i < KILLCOUNTER_COUNT; i++)
    {
        m_arKillCounter[i].Reset();
    }
}



BOOL HearthPortalData::IsValid() const
{
    return WorldTryGetCommonData(place.idSWorld) != NULL;
}





template <typename T>
static bool sGenerateItemIDLow(
    const ActorCommonData * ptACDItem,
    T & tSavedLoadoutItem)
{

    uint64 uFakeItemId = ItemGenerateConsoleEquipmentManagerID(*ptACDItem);
    tSavedLoadoutItem.set_item_id_low(uFakeItemId);
    // Inference: success has no surviving comparison in either caller; both continue unconditionally after setting the ID.
    return true;
}











// Same-file declaration of the original later definition at4393.
BOOL PlayerLoadoutItemIsInValidInventorySlot(const ActorCommonData & tACDItem, ACDID idACDPlayer);

void LoadoutSavedData::Export(
    D3::Hero::Loadout & tSavedLoadout,
    ACDID idACDPlayer) const
{
    tSavedLoadout.set_tab_icon(nTabIcon);

    if (bEmpty)
    {
        return;
    }

    if (strlen(usName) > 0)
    {
        tSavedLoadout.set_name(usName);
    }
    // Inference: all thirteen original equipment entries are exported, even when the item is no longer available.
    for (int32 i = 0; i < sizeof(tEquipment) / sizeof(tEquipment[0]); ++i)
    {
        D3::Hero::LoadoutItem & tSavedLoadoutItem = *tSavedLoadout.add_equipped_items();

        tSavedLoadoutItem.set_item_id_low(tEquipment[i].uOriginalItemIdLow);

        const ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(tEquipment[i].annItem));
        if (!ptACDItem)
        {
            continue;
        }

        if (!PlayerLoadoutItemIsInValidInventorySlot(*ptACDItem, idACDPlayer))
        {
            continue;
        }

        sGenerateItemIDLow(ptACDItem, tSavedLoadoutItem);



        // Inference: only present valid-inventory items contribute their remembered sockets, following the original guarded loop.
        for (int32 j = 0; j < tEquipment[i].nSocketsCount; ++j)
        {
            tSavedLoadoutItem.add_gbid_sockets(tEquipment[i].gbidSockets[j]);
        }
        if (tEquipment[i].uSoulShardIdLow != 0)
            tSavedLoadoutItem.set_soul_shard_id_low(tEquipment[i].uSoulShardIdLow);
    }
    // Inference: six skills, four traits and four legendary powers follow DWARF extents and unrolled original calls.
    for (int32 i = 0; i < 6; ++i)
    {
        D3::Hero::SkillWithRune & tSavedActiveSkill = *tSavedLoadout.add_active_skills();

        tSavedActiveSkill.set_sno_skill(tActiveSkills[i].snoPower);
        if (tActiveSkills[i].eRune != -1)
        {
            tSavedActiveSkill.set_rune_type(tActiveSkills[i].eRune);
        }
    }

    for (int32 i = 0; i < 4; ++i)
    {
        tSavedLoadout.add_sno_traits(snoTraits[i]);
    }

    for (int32 i = 0; i < 4; ++i)
    {
        tSavedLoadout.add_gbid_legendary_powers(gbidLegendaryPowers[i]);
    }
    // Inference: potion export requires a valid stored network name and live ACD; its current GBID replaces the saved value.
    if (tEquippedPotion.annPotion != ANN_INVALID)
    {
        const ActorCommonData * ptACDPotion = ACDTryToGet(ACDFromANN(tEquippedPotion.annPotion));
        if (ptACDPotion)
        {
            D3::Hero::LoadoutPotion * ptEquippedPotion = tSavedLoadout.mutable_equipped_potion();
            ptEquippedPotion->set_item_id_low(tEquippedPotion.uOriginalItemIdLow);
            sGenerateItemIDLow(ptACDPotion, *ptEquippedPotion);
            ptEquippedPotion->set_gbid_potion(ptACDPotion->hGB.gbid);
        }
    }
}



void LoadoutSavedData::Import(
    const D3::Hero::Loadout & tSavedLoadout)
{
    if (tSavedLoadout.has_tab_icon())
    {
        nTabIcon = tSavedLoadout.tab_icon();
    }


    if (tSavedLoadout.equipped_items_size() != 13 && !tSavedLoadout.has_equipped_potion())
    {
        bEmpty = TRUE;
        return;
    }

    bEmpty = FALSE;

    if (tSavedLoadout.has_name())
    {
        Blizzard::String::Copy(usName, tSavedLoadout.name().c_str(), sizeof(usName));
    }
    // Inferred array-size bound: original +0xd4/+0xd8 compares the incremented index unsigned against 13; tEquipment has 13 entries.
    for (int32 i = 0; i < sizeof(tEquipment) / sizeof(tEquipment[0]) && i < tSavedLoadout.equipped_items_size(); ++i)
    {
        const D3::Hero::LoadoutItem & tSavedLoadoutItem = tSavedLoadout.equipped_items(i);
        LoadoutItemData & tLoadoutItem = tEquipment[i];

        tLoadoutItem.uOriginalItemIdLow = tSavedLoadoutItem.item_id_low();

        tLoadoutItem.nSocketsCount = tSavedLoadoutItem.gbid_sockets_size();
        for (int32 nSocket = 0; nSocket < tLoadoutItem.nSocketsCount; ++nSocket)
        {
            tLoadoutItem.gbidSockets[nSocket] = tSavedLoadoutItem.gbid_sockets(nSocket);
        }
        tLoadoutItem.unk_20 = -1;
        tLoadoutItem.uSoulShardIdLow = 0;
        if (tSavedLoadoutItem.has_soul_shard_id_low())
            tLoadoutItem.uSoulShardIdLow = tSavedLoadoutItem.soul_shard_id_low();
    }

    for (int32 i = 0; i < 6 && i < tSavedLoadout.active_skills_size(); ++i)
    {
        tActiveSkills[i].snoPower = tSavedLoadout.active_skills(i).sno_skill();
        if (tSavedLoadout.active_skills(i).has_rune_type())
        {
            tActiveSkills[i].eRune = tSavedLoadout.active_skills(i).rune_type();
        }
    }

    for (int32 i = 0; i < 4 && i < tSavedLoadout.sno_traits_size(); ++i)
    {
        snoTraits[i] = tSavedLoadout.sno_traits(i);
    }

    for (int32 i = 0; i < 4 && i < tSavedLoadout.gbid_legendary_powers_size(); ++i)
    {

        GBID gbidItem = tSavedLoadout.gbid_legendary_powers(i);
        ScopedItemDefinitionLockR ptItemDef(gbidItem);

        if (ptItemDef)
        {


            GBID gbidLegendaryReplacement = ptItemDef->gbidLegendaryPowerItemReplacement;
            if (gbidLegendaryReplacement != GBID_NONE)
            {
                gbidItem = gbidLegendaryReplacement;
            }
        }

        gbidLegendaryPowers[i] = gbidItem;
    }

    if (tSavedLoadout.has_equipped_potion())
    {
        tEquippedPotion.uOriginalItemIdLow = tSavedLoadout.equipped_potion().item_id_low();
        tEquippedPotion.gbidPotion = tSavedLoadout.equipped_potion().gbid_potion();
    }
}
























CalloutEventType CalloutEventFromKillCounter(const KillCounterType eKillCounterType)
{
    CalloutEventType eCalloutEventType;

    switch (eKillCounterType)
    {
    case KILLCOUNTER_MONSTER:
        return CET_KILL_MONSTER;
    case KILLCOUNTER_OBJECT_DESTRUCTION:
        return CET_KILL_OBJECT_DESTRUCTION;
    case KILLCOUNTER_SINGLE_ATTACK:
        return CET_KILL_SINGLE_ATTACK;
    case KILLCOUNTER_ENVIRONMENT_ATTACK:
        return CET_KILL_ENVIRONMENT_ATTACK;
    default:
        break;
    }


    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}







BOOL PlayerIsMale(const Player & tPlayer)
{
    return !((tPlayer.tHeroStateData.uPlayerFlags >> PLAYERFLAG_FEMALE) & 1);
}




CRefString PlayerGetHeroName(const Player & tPlayer)
{
    CRefString usHeroName(tPlayer.usHeroName);
    return usHeroName;
}




CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound)
{
    if (PlayerIsLocal(&tPlayer) && !GameIsWeeklyChallenge())
    {
        return CRefString(tPlayer.usHeroName);
    }
    return CRefString(tPlayer.tAccount.usName);
}















CRefString PlayerGetAccountName(const Player & tPlayer)
{
    CRefString usAccountName(tPlayer.tAccount.usName);
    return usAccountName;
}




void PlayerTalkData::Reset()
{
    idACDTalkingTo = ACDID_INVALID;
}

// NON_MATCHING: the original keeps the next player index in a separate 32-bit register (smaddl); same loop, different induction




void PlayerClientInfo::SetDefaults()
{
    Reset();

    nComboLevel = 0;
    nKillsSinceMoving = 0;
    ann = ANN_INVALID;



    dwNameDisplayEnd = 0;
    dwFadeOutTime = 0;
    dwLastItemMoveProcessed = 0;
    dwTimeLastComboAttack = 0;
    dwTimeLastNonPreplayedAnimatingPower = 0;
    bWantToCancelChanneledPower = FALSE;
    fSuppressVendorCloseNotification = FALSE;

    RandInit(rngSyncedAnim);
    RandSetSeed_Internal(rngSyncedAnim, 0xdeadbeef);

    dwTimeInputWasCached = 0;
    tCachedTargetLock.Clear();

    dwNextTimeCanSayImFull = 0;

    SigmaMemoryClear(dwGameTickOfLastAction, sizeof(dwGameTickOfLastAction));
    SigmaMemoryClear(dwGameTickOfLastHeldAction, sizeof(dwGameTickOfLastHeldAction));
    SigmaMemoryClear(dwGameTickOfLastActionSecondaryPowerCast, sizeof(dwGameTickOfLastActionSecondaryPowerCast));
    SigmaMemoryClear(dwGameTickOfLastActionOtherListener, sizeof(dwGameTickOfLastActionOtherListener));

    dwGameTickOfLastHeldEvent = 0;

    dwClosingAbortTime = 0;

    bQueuedHighlightForceUpdate = bPlayerIsLocal = bJoinRequested = bUpscaled = FALSE;

    dwTimeInputWasCached = 0;
    dwNextTimeCanSayImFull = 0;




}


void PlayerClientInfo::Reset()
{
    dwInterruptedAnimCompleteTime = 0;
    dwMinChanneledDurationTime = 0;


    snoCurrentPower = snoCurrentAnimPower = snoCurrentOriginalAnimPower = snoCurrentChannelledPower = snoCurrentClientWalkPower = snoCurrentLoopingPower = snoLastUsedPower = snoLastCursorQueuedPower = SNO_NONE;





    bWantToCancelChanneledPower = FALSE;


    nUIAction = -1;
    nInputMode = INPUT_MODE_NORMAL;

    idACDCurrentTarget = ACDID_INVALID;
    bClosing = FALSE;
    dwClosingAbortTime = 0;
    bQueuedHighlightForceUpdate = FALSE;
    dwDownEventBits = 0;
    tTargetLock.Clear();
}
enum { GB_INVALID = -1 };  // X/Common/Definitions/GameBalanceDefinition.h:34 (kept local)
void PlayerSetACD(Player & tPlayer, const ACDID idACD);  // Player.cpp:1438
void PlayerClearQuestData(Player & tPlayer);  // Player.cpp:3029
void PlayerPetInitialize(PlayerPet & tPet);  // Player.cpp:2547
void PlayerQuestDataInit(PlayerQuestData & tPlayerQuestData);  // Quests.cpp:987

void PlayerSetDefaults(
    Player & tPlayer)
{



    strcpy(tPlayer.szHeroFilename, "Default");
    new (&tPlayer.tAccount) PlayerAccount;

    uint32 dwNow = GameGetServerTime();
    tPlayer.bDidFirstTimeSetup = FALSE;
    tPlayer.bIsBeingDeleted = FALSE;
    tPlayer.bParticipatingInVoteKick = FALSE;
    tPlayer.bForceHirelingItemSave = FALSE;
    tPlayer.bNewX1Player = FALSE;

    tPlayer.snoActorPortrait = SNO_NONE;
    tPlayer.snoLastWaypointLevelArea = SNO_NONE; tPlayer.ePlayerClassPortrait = PLAYER_CLASS_NONE;
    tPlayer.nPortraitAction = 0;
    PlayerSetACD(tPlayer, ACDID_INVALID);
    tPlayer.idPlayerRActor = RACTORID_INVALID;

    tPlayer.tClientInfo.SetDefaults();

    tPlayer.nControllerIndex = -1;
    tPlayer.bIsPrimaryPlayerServerOnly = FALSE;
    tPlayer.bIsGuestPlayerServerOnly = FALSE;
    tPlayer.bIsLocalToServerServerOnly = FALSE;
    tPlayer.snoControllerBinding = SNO_NONE;
    tPlayer.tTargetComponent.SetOwner(&tPlayer);

    tPlayer.uConsoleSyncFlags = 0;
    tPlayer.uLastPlayedTimeUpdate = 0;
    tPlayer.tConsoleCounters.uMonstersKilled = 0;
    tPlayer.tConsoleCounters.uElitesKilled = 0;
    tPlayer.tConsoleCounters.uGoldPickedUp = 0;
    tPlayer.bHasPerformedFinalSaveForQuit = FALSE;
    tPlayer.tMapVendorSeeds.clear();
    tPlayer.tMapPartitionWideCubeSeeds.clear();
    tPlayer.uCubeBaseSeed = 0;
    tPlayer.tChallengeRiftRewards.clear();

    tPlayer.tNFPDefaultScanTime = 0;
    tPlayer.tNFPGoblinScanTime = 0;




    tPlayer.tBannerRenderData.Initialize();
    tPlayer.eTeam = TEAM_INVALID;
    tPlayer.eJoinTeam = TEAM_INVALID;
    tPlayer.idGameConnection = GAMECONNECTIONID_INVALID;
    for (int32 i = 0; i < LOGOUT_CONTEXT_COUNT; i++) tPlayer.auLogoutTimerData[i].Clear();

    tPlayer.nPlayerKills = 0;
    tPlayer.nDeaths = 0;
    tPlayer.nAssists = 0;
    tPlayer.nTeamWins = 0;
    tPlayer.fOptedIntoActTransition = FALSE;
    tPlayer.idACDActiveVendor = ACDID_INVALID;
    tPlayer.eActiveCrafterType = CRAFTER_TYPE_INVALID;
    tPlayer.bCompletedSoloRift = FALSE;

    // pdefGlobals (DWARF :847) keeps only its lock call (GroupDefinition::LockRFast); no later instruction reads it.
    {
        ScopedGlobalDefinitionLockR pdefGlobals;

        tPlayer.dwLastKickAttempt = dwNow;
        tPlayer.dwLastSuccessfulKick = 0;
    }


    tPlayer.dwLastInputTime = dwNow;


    for (int32 i = 0; i < NUM_VALIDATION_ERROR_TYPES; i++)
    {
        tPlayer.nValidationErrors[i] = 0;
    }
    tPlayer.idActorTownPortalInUse = ACTORID_INVALID;


    PlayerInitializeWaypoints(&tPlayer);

    tPlayer.snoLastResidualFootstep = SNO_NONE;
    tPlayer.dwLastResidualFootstepTime = 0;

    tPlayer.placeCheckedForActivatingScenes = cgplaceNull;



    tPlayer.tTradeData.idACDPlayerTradingWith = ACDID_INVALID;
    tPlayer.tTradeData.idACDPlayerWantToTradeWith = ACDID_INVALID;
    tPlayer.tTradeData.uMyGoldOffer = 0;
    tPlayer.tTradeData.bTradeAccepted = FALSE;

    PlayerQuestDataInit(tPlayer.tQuestData);


    tPlayer.idACDPlayerToInspect = ACDID_INVALID;

    tPlayer.tLastCheckpoint.Initialize();

    SigmaMemoryClear(tPlayer.arGameFlags, sizeof(tPlayer.arGameFlags));


    PlayerClearQuestData(tPlayer);

    tPlayer.tHeroStateData.SetDefaults();
    CrafterSavedDataSetDefaults(tPlayer.tCrafterSavedData);
    tPlayer.tCurrencySavedData.SetToDefaults();

    tPlayer.tTalkData.Reset();


    for (int32 i = 0; i < MAX_PETS; i++)
    {
        PlayerPet & tPet = tPlayer.tPets[i];
        PlayerPetInitialize(tPet);
    }
    tPlayer.nMaxUsedPetIndex = -1;


    HirelingStableInit(tPlayer.tHirelingStable);


    tPlayer.idRequestedHirelingLocation = ACDID_INVALID;
    tPlayer.eRequestedHirelingClass = HC_NONE;
    tPlayer.dwRequestedHirelingTimeStamp = 0;

    for (int32 i = 0; i < 10; i++)
    {
        tPlayer.tLoadouts[i].Initialize();
        tPlayer.tLoadouts[i].nTabIcon = i;
    }

    tPlayer.tStats.SetDefaults();


    tPlayer.uServerFlags = 0;
    tPlayer.tAVCameraTransform = cgtPRTransformIdentity;
    tPlayer.tQueuedPower.SetDefaults();

    tPlayer.tPlayerResumeStateUponJoining.eLastPlayedAct = ACT_1;
    tPlayer.tPlayerResumeStateUponJoining.snoLastPlayedQuest = SNO_NONE;
    tPlayer.tPlayerResumeStateUponJoining.nLastPlayedQuestStep = -1;
    tPlayer.tPlayerResumeStateUponJoining.uActivatedWaypoints = 0;
    tPlayer.tPlayerResumeStateUponJoining.tLastSavePoint.Reset();
    tPlayer.tPlayerResumeStateUponJoining.nHandicapLevelSnapshot = -1;

    tPlayer.nKicksInitiated = 0;
    tPlayer.nKicksParticipatedIn = 0;
    tPlayer.nGamesWithoutKicks = 0;
    tPlayer.nTimesVoteKicked = 0;

    tPlayer.dwBossesKilledFlags = 0;
    tPlayer.dwNephalemRiftFlags = 0;
    tPlayer.uEntitledRewardBits = 0;
    tPlayer.uOutstandingRewardBits = 0;
    tPlayer.dwTimeStampJoined = 0;

    tPlayer.hGBKiller.Set(GB_INVALID, GBID_NONE);
    tPlayer.snoKiller = SNO_NONE;
    tPlayer.eKillerRarity = MR_NONE;
    tPlayer.killerRareNameGBIDs[0] = GBID_NONE;
    tPlayer.killerRareNameGBIDs[1] = GBID_NONE;
    tPlayer.snoKilledIn = SNO_NONE;
    tPlayer.idTimerForcedResurrect = TIMERID_INVALID;
    tPlayer.idTimerCorpseResurrect = TIMERID_INVALID;
    tPlayer.idTimerCorpseResurrectGrantCharge = TIMERID_INVALID;




    tPlayer.dwTickLastResurrection = 0;
    tPlayer.uAcceptedLicenseBitsHero = 0;

    tPlayer.tVendorData.Initialize();

    tPlayer.bOptedInToOpenWorldModeChange = FALSE;
    tPlayer.bPlayOpenWorldTutorial = FALSE;
    tPlayer.bCosmeticPetMissing = FALSE;

    tPlayer.idTimerLoadoutEquipApplyPassives = TIMERID_INVALID;



    SigmaMemoryClear(tPlayer.tSeenTutorials.GetRawBuffer(), tPlayer.tSeenTutorials.GetLength());
    for (int32 i = 0; i < 13; i++)
    {
        tPlayer.bStashIcons[i] = i % MAX_STASH_TABS_CONSOLE;
    }

    tPlayer.dwServerSidePrefFlags = 0;

    tPlayer.eLocale = 0;
    tPlayer.eWarpReason = WARP_UNKNOWN;

    tPlayer.fRefreshWaypointDestination = FALSE;
    tPlayer.flWarpFadeInSecs = 0.0f;
    tPlayer.eOpenWorldStatus = OPENWORLD_STATUS_INVALID;
    tPlayer.bForceKnownListUpdate = TRUE;

    for (int32 i = 0; i < 16; i++)
    {
        tPlayer.arMoveValidationLog[i].eType = MOVE_LOG_INVALID;
    }
    tPlayer.uMoveLogIdx = 0;

    tPlayer.bSetDungeonActive = FALSE;
}




void CrafterSavedDataSetDefaults(CrafterSavedData & tData)
{
    for (int32 i = 0; i < CRAFTER_TYPE_COUNT; i++)
    {
        PlayerCrafterInitialize(tData.tCrafterData[i]);
    }
    tData.tCrafterData[CRAFTER_TYPE_JEWEL_UPGRADE].nLevel = 1;

    tData.tTransmogData.SetToDefaults();
    tData.tExtractedLegendariesData.SetToDefaults();
}










// Lines from the original line table (Player.cpp:1022-1087); locals from the cu_04439fcf abstract DIEs
// (ptPlayerGlobals 1023, i 1035, tPlayer 1037, nPlayer 1052).
void PlayersInitializeGameCommonData(
    GameCommonData & tGCD)
{
    PlayerGlobals *& ptPlayerGlobals = tGCD.ptPlayerGlobals;
    ptPlayerGlobals = new (SigmaMemoryNew(sizeof(PlayerGlobals), 4, AppGetCurrentMemoryInterface(), TRUE)) PlayerGlobals;


    ptPlayerGlobals->poolTimeMap.Initialize();
    ptPlayerGlobals->poolTimeMap.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptPlayerGlobals->poolTimeMap.SetBlockCount_Tracked(1024);

    ptPlayerGlobals->poolSNOName.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptPlayerGlobals->poolSNOName.SetBlockCount_Tracked(64);
    ptPlayerGlobals->poolSNOName.SetGrow(XALLOC_GROW_LINEAR, 64);

    for (int32 i = 0; i < MAX_PLAYERS; ++i)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];


        tPlayer.nPlayerIndex = i;
        tPlayer.listVisibleACDs.SetPool(MemoryPoolsGetDATAIDListPool());
        tPlayer.listPrefetchableACDs.SetPool(MemoryPoolsGetDATAIDListPool());
        tPlayer.listDestinationsToPrefetch.SetPool(MemoryPoolsGetDATAIDListPool());
        tPlayer.fRefreshWaypointDestination = FALSE;
        tPlayer.bCosmeticPetMissing = FALSE;


        tPlayer.tLoresUsed.SetPool(MemoryPoolsGetDATAIDListPool());
        tPlayer.listOnGroundCraftingPlans.SetPool(MemoryPoolsGetDATAIDListPool());
        tPlayer.nBusyMessagesSinceNPCInteract = 0;

        for (int32 nPlayer = 0; nPlayer < MAX_PLAYERS; nPlayer++)
        {
            tPlayer.flPVPPlayerDamageTaken[nPlayer] = 0.0f;
        }

        tPlayer.idACDActiveChallengePortal = ACDID_INVALID;
        tPlayer.bChallengeAccepted = FALSE;
        tPlayer.idACDStashLastOperated = ACDID_INVALID;
        tPlayer.dwTickLastVerification = 0;

        tPlayer.tPlaceLastVerification = cgplaceNull;

        tPlayer.flMaxSpeedThisVerificationInterval = 0.0f;
        tPlayer.dwTickLastSpamCheck = 0;
        tPlayer.nMessageCostLast300 = 0;
        tPlayer.nMessageCostLast600 = 0;
        tPlayer.nMessageCostLast3600 = 0;
        tPlayer.nMessageCostLastUpdateTick = 0;
        tPlayer.nMessageCostMax300 = 0;
        tPlayer.nMessageCostMax600 = 0;
        tPlayer.nMessageCostMax3600 = 0;



        tPlayer.listConversations.SetPool(&ptPlayerGlobals->poolSNOName);


        PlayerSetDefaults(tPlayer);

        tPlayer.ptNewItemManager = new UIInventory::Console::NewItemIndicatorManager<ACDID>;
        tPlayer.ptNewEliteItemManager = new UIInventory::Console::NewItemIndicatorManager<ACDID>;
        tPlayer.ptNewRecipeManager = new UIInventory::Console::NewItemIndicatorManager<GBID>;
        tPlayer.ptNewCosmeticManager = new UIInventory::Console::NewItemIndicatorManager<GBID>;
    }

}



// implicit _ZN6PlayerD2Ev (UITutorials.cpp owner) is emitted here too; with list.h clear storing next (:801) then prev (:802) it equals the original except the final tail-call target, which is local in this object.
void PlayersDisposeGameCommonData(
    GameCommonData & tGCD)
{


    PlayerGlobals *& ptPlayerGlobals = tGCD.ptPlayerGlobals;
    if (!ptPlayerGlobals)
    {
        return;
    }


    for (int32 i = 0; i < MAX_PLAYERS; ++i)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];


        HirelingStableDispose(tPlayer.tHirelingStable);


        delete tPlayer.ptNewCosmeticManager; tPlayer.ptNewCosmeticManager = NULL;
        delete tPlayer.ptNewRecipeManager; tPlayer.ptNewRecipeManager = NULL;
        delete tPlayer.ptNewEliteItemManager; tPlayer.ptNewEliteItemManager = NULL;
        delete tPlayer.ptNewItemManager; tPlayer.ptNewItemManager = NULL;
    }



    ptPlayerGlobals->~PlayerGlobals(); SigmaMemoryFree(ptPlayerGlobals, AppGetCurrentMemoryInterface()); ptPlayerGlobals = NULL;
}



ActorCommonData * ConsoleTargetComponent::ResolveStickyTarget()
{
    ActorCommonData * ptACDTarget = ACDTryToGet(GetLastTarget());
    if (ptACDTarget == NULL)
    {
        SetLastTarget(ACDID_INVALID, FALSE);
        return NULL;
    }
    // inferred: the NULL exit above tail-merges with 1168-1169 (one inlined SetLastTarget survives); no lexical blocks in DWARF, so the locals below are function-scope
    const ActorCommonData & tACDPlayer = ACDGet(ptOwner->idPlayerACD);
    WorldPlace tPlacePlayer = tACDPlayer.GetPlace();
    WorldPlace tPlaceTarget = ptACDTarget->GetPlace();


    Vector2D vForward = GetTargetingDirection();

    SNO snoPower = ptOwner->tClientInfo.snoLastUsedPower;
    const PowerDefinition * pdefPower = NULL;
    if (snoPower != SNO_NONE)
    {
        pdefPower = static_cast<const PowerDefinition *>(gptGroupDefPower->LockR(snoPower, FALSE));
    }

    Targeting::Console::TargetingInformationInput input(ptOwner, pdefPower, Targeting::Console::TARGETING_STICKY);
    Targeting::Console::TargetingInformationOutput output;
    Targeting::Console::GetTargetingInformation(input, output);
    // Inference: original group unlock takes pdefPower by reference; no inline lock/unlock helper DIE.
    if (pdefPower)
    {
        gptGroupDefPower->UnlockR((void *&)pdefPower);
    }

    Vector2D vToTargetFromIntersection = tPlaceTarget.wp.As2D() - output.wpConeStart.wp.As2D();
    Vector2D vToTargetFromPlayer = tPlaceTarget.wp.As2D() - tPlacePlayer.wp.As2D();
    vToTargetFromIntersection.Normalize();
    vToTargetFromPlayer.Normalize();
    float cosThetaFromIntersection = Vector2D::DotProduct(vForward, vToTargetFromIntersection);
    float cosThetaFromPlayer = Vector2D::DotProduct(vForward, vToTargetFromPlayer);
    if (cosThetaFromIntersection >= Math::Cos(output.aSearchAngle) && cosThetaFromPlayer >= FLT_EPSILON)
    {
        return ptACDTarget;
    }

    SetLastTarget(ACDID_INVALID, FALSE);
    return NULL;
}


ActorCommonData * ConsoleTargetComponent::ResolveLockedTarget()
{
    if (eTargetLockType == TARGETLOCK_HARD || eTargetLockType == TARGETLOCK_STICKY || eTargetLockType == TARGETLOCK_PERSISTENT)
    {
        ActorCommonData * ptACDTarget = ACDTryToGet(m_lockedTarget);
        if (!ptACDTarget)
        {
            ClearLockedTarget();
            return NULL;
        }


        if (!ptACDTarget->IsAlive() || !ptACDTarget->IsTargetable())
        {
            ClearLockedTarget();
            return NULL;
        }


        return ptACDTarget;
    }


    return NULL;
}


void ConsoleTargetComponent::ClearLockedTarget()
{
    if (eTargetLockType == TARGETLOCK_HARD)
    {
        SNO snoEffect = GetTargetLockSNOForPlayer(ptOwner);
        ActorCommonData * ptACDTarget = ACDTryToGet(m_lockedTarget);
        if (ptACDTarget)
        {
            ptACDTarget->RemoveClientEffect(snoEffect);
        }
    }

    m_lockedTarget = ACDID_INVALID;
    eTargetLockType = TARGETLOCK_NONE;
}



static const float MIN_TARGETING_RANGE = 4.0f;

float ConsoleTargetComponent::GetTargetRangeAdjustment(SNO snoPower) const
{
    if (snoPower == SNO_NONE)
    {
        return flTargetRangeAdjustment;
    }

    ScopedPowerDefinitionLockR pdefPower(snoPower);
    return GetTargetRangeAdjustment(pdefPower);
}

float ConsoleTargetComponent::GetTargetRangeAdjustment(const PowerDefinition * pdefPower) const
{
    float flAdjust;
    const ActorCommonData & tACD = ACDGet(ptOwner->idPlayerACD);
    if (flTargetRangeAdjustment > FLT_EPSILON)
    {
        flAdjust = flTargetRangeAdjustment + PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CONTROLLER_MIN_RANGE_HELD, -1, -1, 0);
    }
    else
    {
        flAdjust = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CONTROLLER_MIN_RANGE, -1, -1, 0);
    }

    return Math::Pin(flAdjust, MIN_TARGETING_RANGE, PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CONTROLLER_MAX_RANGE, -1, -1, 0));
}















void ConsoleTargetComponent::ResetTargetingReticle(const Player * ptPlayer, SNO snoPower, BOOL bMaxDistance)
{
    if (PowerAutoTargets(snoPower, ptPlayer))
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD)
        {
            ResolvedClick tResolvedClick;
            CPlayerUtilResolveConsoleButton(snoPower, tResolvedClick, ptPlayer, FALSE, TRUE, FALSE);

            WorldPlace tPlace;
            tResolvedClick.tTarget.GetPlace(tPlace);

            vTargetingReticle = tPlace.wp.As2D() - ptACD->GetPlace().wp.As2D();
            return;
        }
    }

    float flAdjustment = bMaxDistance ? GetTargetRangeAdjustment(snoPower) : MIN_TARGETING_RANGE;
    vTargetingReticle = Console::ControllerUtils::GetLStickWorldDirection(ptOwner->nControllerIndex) * flAdjustment;
}


void ConsoleTargetComponent::UpdateReticleDistanceRatio(SNO snoPower)
{
    ScopedGlobalDefinitionLockR pdefGlobals;
    float flRange = pdefGlobals->flTargetRangeMaxGrowth;
    if (flRange > FLT_EPSILON)
    {
        flReticleDistanceRatio = (vTargetingReticle.Magnitude() - MIN_TARGETING_RANGE) / flRange;
    }
}

void ConsoleTargetComponent::GetTargetingReticle(Vector2D & vOutput) const
{
    if (vTargetingReticle.MagnitudeSquared() > MIN_TARGETING_RANGE * MIN_TARGETING_RANGE)
    {
        vOutput = vTargetingReticle;
    }
    else
    {
        const ActorCommonData & tACD = ACDGet(ptOwner->idPlayerACD);
        vOutput = tACD.GetForward2D();
        vOutput *= MIN_TARGETING_RANGE;
    }
}









void ConsoleTargetComponent::AdjustTargetingReticle(float flScalar)
{
    if (Console::ControllerUtils::LStickIsInDeadZone(ptOwner->nControllerIndex, TRUE))
    {
        ScopedGlobalDefinitionLockR pdefGlobals;
        float flAdjust = CGameGetClientDeltaTime() * pdefGlobals->flTargetRangeGrowthPerSecond * flScalar;
        Vector2D vLStick = Console::ControllerUtils::GetLStickWorldDirection(ptOwner->nControllerIndex);
        vTargetingReticle = flAdjust * vLStick + vTargetingReticle;
        float flDistance = vTargetingReticle.Magnitude();
        flTargetRangeAdjustment = Math::Pin(flDistance, MIN_TARGETING_RANGE, pdefGlobals->flTargetRangeMaxGrowth);
        if (flDistance > MIN_TARGETING_RANGE && flTargetRangeAdjustment != flDistance)
        {
            vTargetingReticle *= flTargetRangeAdjustment / flDistance;
        }
    }
}

















Vector2D ConsoleTargetComponent::GetTargetingDirection() const
{
    if (bInReticleMode)
    {
        Vector2D vReticle;
        GetTargetingReticle(vReticle);
        vReticle.Normalize();
        return vReticle;
    }

    return Console::ControllerUtils::GetLStickWorldDirection(ptOwner->nControllerIndex);
}










uint32 PlayersGetCount(
    void)
{
    uint32 dwCount = 0;
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        dwCount++;
    }
    return dwCount;
}



static BOOL sPlayerIsValid(const Player & tPlayer)
{
    return tPlayer.eTeam != -1;  // TEAM_INVALID (X/Common/ActorCommonDataSwig.h)
}






Player * ACDGetControllingPlayer(const ACDID idACD)
{
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return NULL;
    }

    ACDID idACDController = ACDGetControllingActor(idACD);
    if (!ACDIsPlayer(idACDController))
    {
        idACDController = idACD;
    }

    return PlayerGetByACD(idACDController);
}





ACDID ACDGetPlayerACDOnSameTeam(const ACDID idACD)
{
    ActorCommonData * pACD = ACDTryToGet(idACD);
    if (!pACD)
    {
        return ACDID_INVALID;
    }

    Player * ptPlayer = ACDGetControllingPlayer(idACD);
    if (!ptPlayer)
    {
        return ACDID_INVALID;
    }

    return ptPlayer->idPlayerACD;
}







void PlayerSetACD(
    Player & tPlayer,
    const ACDID idACD)
{
    tPlayer.idPlayerACD = idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        tPlayer.ePlayerClassPortrait = PlayerGetPlayerClass(idACD);
        tPlayer.snoActorPortrait = ptACD->snoActor;



        if (GameActiveGameCommonDataIsClient())
        {
            tPlayer.tClientInfo.ann = ANNFromACD(idACD);
        }
        else
        {


            ptACD->SetTeam(tPlayer.eTeam);
        }
    }
}




Player * PlayerGetByACD(const ACDID idACD)
{
    if (idACD == ACDID_INVALID)
    {
        return NULL;
    }

    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        if (tPlayer.idPlayerACD == idACD)
        {
            return &tPlayer;
        }
    }
    return NULL;
}





Player * PlayerGetByRActor(RActorID idRActor)
{
    if (idRActor == RACTORID_INVALID)
    {
        return NULL;
    }

    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        if (tPlayer.idPlayerRActor == idRActor)
        {
            return &tPlayer;
        }
    }
    return NULL;
}











Player * PlayerGetByANN(const ACDNetworkName ann)
{
    if (ann == ANN_INVALID)
    {
        return NULL;
    }

    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        if (tPlayer.tClientInfo.ann == ann)
        {
            return &tPlayer;
        }
    }
    return NULL;
}









Player * PlayerGetByHeroId(OnlineService::HeroId uHeroId)
{
    if (uHeroId == OnlineService::INVALID_HERO_ID)
    {
        return NULL;
    }

    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        if (tPlayer.uHeroId == uHeroId)
        {
            return &tPlayer;
        }
    }
    return NULL;
}







Player * PlayerGetByGameAccount(const OnlineService::GameAccountHandle & tGameAccount)
{
    if (tGameAccount.IsInvalid())
    {
        return NULL;
    }

    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];
        if (tPlayer.tAccount.tHandle == tGameAccount)
        {
            return &tPlayer;
        }
    }
    return NULL;
}









Player * PlayerGetByControllerIndex(int32 nControllerIndex)
{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->nControllerIndex == nControllerIndex)
        {
            return ptPlayer;
        }
    }
    return NULL;
}







Player * PlayerGetByControllerIndexAlways(int32 nControllerIndex)
{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer) && ptPlayer->nControllerIndex == nControllerIndex)
        {
            return ptPlayer;
        }
    }
    return NULL;
}







PlayerAvengerData * GetPlayerAvengerData(Player * ptPlayer, AvengerSource eAvengerSource)
{
    PlayerAvengerData * ptPlayerAvengerData = NULL;

    switch (eAvengerSource)
    {
    case AVENGER_SOURCE_SOLO:
        ptPlayerAvengerData = &ptPlayer->tSoloAvenger;
        break;
    case AVENGER_SOURCE_HARDCORE:
        ptPlayerAvengerData = &ptPlayer->tHardcoreAvenger;
        break;
    case AVENGER_SOURCE_FRIEND:
        ptPlayerAvengerData = &ptPlayer->tFriendAvenger;
        break;
    default:
        ptPlayerAvengerData = &ptPlayer->tSoloAvenger;
        break;
    }
    return ptPlayerAvengerData;
}




BOOL ACDIsPlayer(const ACDID idACD)
{
    return PlayerGetByACD(idACD) != NULL;
}



BOOL ACDIsPet(const ACDID idACD)
{
    BOOL bRet = FALSE;
    const ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        bRet = ptACD->IsPet();
    }
    return bRet;
}





// Inferred case spelling from the original four-pointer table at 0x710104ad30, range 2..5, strings 0x7100dc1c35/79/bd and 0x7100dc1d01.
static const char * sGetAnchorName(
    SkillButton eButton)
{
    switch (eButton)
    {
#include "generated-game-data/hotbar_anchors.inc"
#line 1711
    default: return NULL;
    }
}









BOOL PlayerAssignBaseHealthPotion(Player * ptPlayer)
{
    if (!ptPlayer)
    {
        return FALSE;
    }

    InventoryLocation tInvLoc;
    tInvLoc.idACDContainer = ptPlayer->idPlayerACD;
    tInvLoc.eSlot = SLOT_PLAYER_BASE_HEALTHPOTION;
    tInvLoc.pt.Set(0, 0);

    ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLoc);

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItemInSlot);
    if (ptACDItem && ItemCanBeAssignedToUsableItemButton(*ptACDItem))
    {
        ptPlayer->tHeroStateData.tSavedData.tPotionBtn.SetItemANN(ANNFromACD(idACDItemInSlot));
        return TRUE;
    }
    return FALSE;
}










void PlayerClearPotionBtnForMissingItem(Player * ptPlayer, const ACDID idACDItem)
{
    HotbarButtonData & tSavedButtonData = ptPlayer->tHeroStateData.tSavedData.tPotionBtn;
    if (idACDItem == ACDID_INVALID)
    {
        return;
    }

    ACDNetworkName annItem = ANNFromACD(idACDItem);
    if (tSavedButtonData.m_annItem == annItem && !PlayerAssignBaseHealthPotion(ptPlayer))
    {
        HotbarButtonData tEmptyButtonData;
        PlayerSetPotionBtnAssignment(ptPlayer, tEmptyButtonData);
    }
}











void PlayerSetPotionBtnAssignment(Player * ptPlayer, const HotbarButtonData & tButtonData)
{
    HotbarButtonData & tSavedButtonData = ptPlayer->tHeroStateData.tSavedData.tPotionBtn;
    if (tButtonData.m_snoPower == SNO_NONE)
    {
        tSavedButtonData = tButtonData;
    }
}

// NON_MATCHING: the original addresses the guard and tOverrideAssignments from one .bss anchor (+0x18/+0x30); file statics not all written yet






// Condition order from the original line table columns: 1823:44 (&& after the server test), 1824:21 (m_snoPower member) before the button tests.
void PlayerSetHotbarBtnAssignment(
    Player * ptPlayer,
    SkillButton eButton,
    const HotbarButtonData & tButtonData)
{
    if (eButton < 0 || eButton >= 6)
    {
        return;
    }



    if (tButtonData.m_gbidItem != GBID_NONE ||
        tButtonData.m_annItem != ANN_INVALID)
    {
        return;
    }


    if (GameActiveGameCommonDataIsClient())
    {

        CursorMgr & tCursorMgr = PlayerGetCursorMgr();
        if (tCursorMgr.GetHotBarOverridden())
        {
            return;
        }
    }


    HotbarButtonData & tSavedButtonData = ptPlayer->tHeroStateData.tSavedData.tHotbarBtnAssignments[eButton];
    tSavedButtonData = tButtonData;


    if (tSavedButtonData.m_snoPower != SNO_NONE)
    {

        if (!SNOFileExists(POWER_SNO, tSavedButtonData.m_snoPower))
        {

            tSavedButtonData.m_snoPower = SNO_NONE;
        }
    }


    if (GameActiveGameCommonDataIsServer() &&
        tButtonData.m_snoPower != SNO_NONE && eButton != SKILL_LMB && eButton != SKILL_RMB)
    {

        SNO snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_HOTBAR_SKILL_READY);
        if (snoTutorial != SNO_NONE && !Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial))
        {
            D3::GameMessage::TutorialMessage tMessage;
            tMessage.set_tutorial_sno(snoTutorial);
            tMessage.set_ui_anchor_name(sGetAnchorName(eButton), strlen(sGetAnchorName(eButton)));

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, ptPlayer->idPlayerACD);

            MessageSendToClient(listDestPlayers, SMSG_TUTORIAL, tMessage);
        }
    }

}




const HotbarButtonData & PlayerGetHotbarBtnAssignment(const Player * ptPlayer, const SkillButton eButton)
{
    static HotbarButtonData tOverrideAssignments[6];

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer && ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_SKILL_OVERRIDE_ACTIVE)) && GameActiveGameCommonDataIsClient() && PlayerIsLocal(ptPlayer))
    {
        SNO snoPower = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, eButton));
        HotbarButtonData & tOverrideButton = tOverrideAssignments[eButton];
        tOverrideButton.SetSkill(snoPower, -1);
        return tOverrideButton;
    }

    return ptPlayer->tHeroStateData.tSavedData.tHotbarBtnAssignments[eButton];
}















SNO PlayerGetLastWaypointLevelArea(const Player * ptPlayer)
{
    return ptPlayer->snoLastWaypointLevelArea;
}





SNO PlayerGetPowerSNOFromBufferedInput(const Player & tPlayer, const BufferedInput & tInput)
{
    if (Controller::InputIsTownPortal(tInput))
    {
        return GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL);
    }

    int32 nSkillSlot = Controller::GetSlotFromInput(tInput);
    if (nSkillSlot == -1)
    {
        return SNO_NONE;
    }

    const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(&tPlayer, nSkillSlot);
    return tButtonData.m_snoPower;
}







const HotbarButtonData & PlayerGetPotionBtnAssignment(const Player * ptPlayer)
{
    return ptPlayer->tHeroStateData.tSavedData.tPotionBtn;
}




void PlayerDetermineNextDualWieldHand(const ACDID idACDPlayer)
{
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    DualWieldAttackType eAttackType = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND)) == DUALWIELD_ATTACK_MAIN ? DUALWIELD_ATTACK_OFFHAND : DUALWIELD_ATTACK_MAIN;
    tACDPlayer.AttributesSetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND_NEXT), eAttackType);

    if (GameActiveGameCommonDataIsServer() && PowerGetNormalAttackReplacement(idACDPlayer) == SNO_NONE)
    {
        float flBothAttackChance = tACDPlayer.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DUAL_WIELD_BOTH_ATTACK_CHANCE));
        if (flBothAttackChance > 1e-6f)
        {
            float flBothAttackRoll = RandFloat_Internal(SGameGlobalsGet().rng);
            if (flBothAttackRoll <= flBothAttackChance)
            {
                PowerAPI_SetNormalAttackReplacement(idACDPlayer, GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT_BOTHHAND));
            }
        }
    }
}








void PlayerResetDualWield(const ACDID idACDPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer && ACDIsPlayer(idACDPlayer))
    {
        ptACDPlayer->AttributesSetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), DUALWIELD_ATTACK_MAIN);
        PlayerDetermineNextDualWieldHand(idACDPlayer);
    }
}




Player * PlayerGetByPlayerIndex(int32 nPlayerIndex)
{
    if (nPlayerIndex == -1)
    {
        return NULL;
    }




    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    return &ptPlayerGlobals->tPlayerArray[nPlayerIndex];
}





Player * PlayerGetFirstInGame()
{
    Player * ptPlayer = PlayerGetFirstAll();
    while (ptPlayer)
    {
        if (PlayerIsInWorld(*ptPlayer))
        {
            return ptPlayer;
        }
        ptPlayer = PlayerGetNextAll(ptPlayer);
    }
    return NULL;
}






Player * PlayerGetNextInGame(const Player * ptPlayer)
{
    Player * ptPlayerNext = PlayerGetNextAll(ptPlayer);
    while (ptPlayerNext)
    {
        if (PlayerIsInWorld(*ptPlayerNext))
        {
            return ptPlayerNext;
        }
        ptPlayerNext = PlayerGetNextAll(ptPlayerNext);
    }
    return NULL;
}






Player * PlayerGetFirstInTieredRift()
{
    if (!NephalemRiftManager::IsTieredRiftActive())
    {
        return NULL;
    }

    Player * ptPlayer = PlayerGetFirstInGame();
    while (ptPlayer && !NephalemRiftManager::PlayerPaidForRift(ptPlayer))
    {
        ptPlayer = PlayerGetNextInGame(ptPlayer);
    }
    return ptPlayer;
}























Player * PlayerGetNextInTieredRift(const Player * ptPlayer)
{
    Player * ptNextPlayer = PlayerGetNextInGame(ptPlayer);
    while (ptNextPlayer && !NephalemRiftManager::PlayerPaidForRift(ptNextPlayer))
    {
        ptNextPlayer = PlayerGetNextInGame(ptNextPlayer);
    }
    return ptNextPlayer;
}






















BOOL PlayerPendingRiftJoin(const Player * ptPlayer)
{
    uint32 dwRiftFlags = ptPlayer->dwNephalemRiftFlags;
    return (dwRiftFlags & 0x1) || (dwRiftFlags & 0x2) || (dwRiftFlags & 0x10);
}











Player * sPlayerGetNext(
    const int32 nStartingIndex)
{
    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;



    for (int32 i = nStartingIndex; i < MAX_PLAYERS; i++)
    {
        Player * ptPlayer = &ptPlayerGlobals->tPlayerArray[i];
        if (ptPlayer->eTeam != TEAM_INVALID)
        {
            return ptPlayer;
        }
    }

    return NULL;
}



Player * PlayerGetFirstAll(
    void)
{
    return sPlayerGetNext(0);
}




Player * PlayerGetNextAll(
    const Player * ptPlayer)
{

    return sPlayerGetNext(ptPlayer->nPlayerIndex + 1);
}



uint32 PlayerCalculatePlayerCountOnConnection(const GameConnectionID & tGameConnection)
{
    int32 nPlayerCount = 0;
    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player * ptPlayer = &ptPlayerGlobals->tPlayerArray[i];
        if (sPlayerIsValid(*ptPlayer) && ptPlayer->idGameConnection == tGameConnection)
        {
            nPlayerCount++;
        }
    }
    return nPlayerCount;
}










int32 PlayerGetNumPlayersForDifficulty()
{
    int32 nPlayerCountOverride = GameGetActiveGameCommonData().nNumPlayers;
    if (nPlayerCountOverride > 0)
    {
        return nPlayerCountOverride;
    }

    if (GameTypeIsPvP())
    {
        return 1;
    }

    return PlayerGetNumPlayers();
}







int32 PlayerGetNumPlayers()
{
    int32 nPlayerCount = 0;
    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player * ptPlayer = &ptPlayerGlobals->tPlayerArray[i];
        if (sPlayerIsValid(*ptPlayer))
        {
            nPlayerCount++;
        }
    }
    return nPlayerCount;
}








int32 PlayerGetNumValidInGamePlayers()
{
    int32 nPlayerCount = 0;
    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player * ptPlayer = &ptPlayerGlobals->tPlayerArray[i];
        if (sPlayerIsValid(*ptPlayer) && (!PlayerIsLocal(ptPlayer) || ptPlayer->idPlayerACD != ACDID_INVALID))
        {
            nPlayerCount++;
        }
    }
    return nPlayerCount;
}









void PlayerInitializeWaypoints(Player * ptPlayer)
{
    ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints = 0;
}

struct PlayerClassLookup {  // size 0x8
    PlayerClass ePlayerClass;  // 0x00
    GlobalGBID eGlobalGBID;  // 0x04
};





static PlayerClassLookup sgtPlayerClassLookupTable[] =
{
    { PLAYER_CLASS_DEMONHUNTER, GLOBALGBID_PLAYER_DEMONHUNTER },
    { PLAYER_CLASS_BARBARIAN, GLOBALGBID_PLAYER_BARBARIAN },
    { PLAYER_CLASS_WIZARD, GLOBALGBID_PLAYER_WIZARD },
    { PLAYER_CLASS_WITCHDOCTOR, GLOBALGBID_PLAYER_WITCHDOCTOR },
    { PLAYER_CLASS_MONK, GLOBALGBID_PLAYER_MONK },
    { PLAYER_CLASS_CRUSADER, GLOBALGBID_PLAYER_CRUSADER },
    { PLAYER_CLASS_NECROMANCER, GLOBALGBID_PLAYER_NECROMANCER },
};
static const int32 sgnNumPlayerClassLookupEntries = 7;




PlayerClass PlayerClassFromGBID(const GBID gbid)
{
    if (gbid == GBID_NONE)
    {
        return PLAYER_CLASS_NONE;
    }

    for (int32 i = 0; i < sgnNumPlayerClassLookupEntries; i++)
    {
        if (GlobalGBIDGet(sgtPlayerClassLookupTable[i].eGlobalGBID) == gbid)
        {
            return sgtPlayerClassLookupTable[i].ePlayerClass;
        }
    }
    return PLAYER_CLASS_NONE;
}




GBID PlayerClassToGBID(const PlayerClass ePlayerClass)
{
    for (int32 i = 0; i < sgnNumPlayerClassLookupEntries; i++)
    {
        if (sgtPlayerClassLookupTable[i].ePlayerClass == ePlayerClass)
        {
            return GlobalGBIDGet(sgtPlayerClassLookupTable[i].eGlobalGBID);
        }
    }
    return GBID_NONE;
}






PlayerClass PlayerGetPlayerClass(const ACDID idACD)
{
    return PlayerGetPlayerClass(ACDTryToGet(idACD));
}




PlayerClass PlayerGetPlayerClass(const ActorCommonData * ptACD)
{
    if (ptACD && ptACD->hGB.eType == GB_HEROS)
    {
        return PlayerClassFromGBID(ptACD->hGB.gbid);
    }
    return PLAYER_CLASS_NONE;
}
#include "Monsters.h"
#include "UserInterface/UITextControl.h"
#include "../Sigma/TypeDescriptor/SymbolicValue.h"
// Inferred declaration placement; signatures from UI.cpp:3860, UIHireling.cpp:420, STrickle.cpp:305 and GameCommonData.cpp:466.
void UI_Print(LPCSTR szText, const BOOL fEchoToConsole);
namespace UIHireling { void OnFollowerDismissed(ACDID idACDHireling); }
void STrickleUpdateActor(const ActorCommonData & tACD);
BOOL ActEnumIsValid(const Act eAct, BOOL bActTestIsLegal);
extern const SymbolicEntry gActs[9]; // DWARF SymbolicValue.cpp:2591; declaration placement inferred.

PlayerClass PlayerGetPlayerClass(const Player * ptPlayer)
{
    if (ptPlayer)
    {
        return ptPlayer->ePlayerClassPortrait;
    }
    return PLAYER_CLASS_NONE;
}






BOOL PlayerClassCanDualWield(const PlayerClass ePlayerClass)
{
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
    case PLAYER_CLASS_BARBARIAN:
    case PLAYER_CLASS_MONK:
        return TRUE;
    case PLAYER_CLASS_WIZARD:
    case PLAYER_CLASS_WITCHDOCTOR:
        return FALSE;
    }
    return FALSE;
}

static void sSendDetachPetMessage(
    Player & tPlayer,
    ACDID idACDPet,
    BOOL bDisplayChatMessage)
{






    PetDetachMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.annPet = ANNFromACD(idACDPet);
    tMessage.bDisplayChatMessage = bDisplayChatMessage;


    PlayerList listDestPlayer(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayer, tPlayer);


    MessageSendToClient(listDestPlayer, SMSG_DETACH_PET, &tMessage, sizeof(tMessage));

}













void PlayerRemovePet(ACDID idACDPlayer, ACDID idACDPet, BOOL bDisplayChatMessage)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {



        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    PlayerRemovePet(*ptPlayer, idACDPet, bDisplayChatMessage);
}







void PlayerRemovePet(
    Player & tPlayer,
    ACDID idACDPet,
    BOOL bDisplayChatMessage)
{

    if (idACDPet == ACDID_INVALID)
    {
        return;
    }


    if (tPlayer.idPlayerACD == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDPet = ACDGet(idACDPet);

    // Inference: zero-based scan through the inclusive highest occupied pet slot follows original loop bounds.
    for (int32 i = 0; i <= tPlayer.nMaxUsedPetIndex; ++i)
    {
        if (tPlayer.tPets[i].idACD == idACDPet)
        {
            if (i == tPlayer.nMaxUsedPetIndex)
            {
                for (int32 j = i - 1; j >= 0; --j)
                {
                    if (tPlayer.tPets[j].idACD != ACDID_INVALID)
                    {
                        tPlayer.nMaxUsedPetIndex = j;
                        break;
                    }
                }

                // Inference: original compares unchanged highest slot to i after the scoped reverse scan (DWARF j ends before this test).
                if (tPlayer.nMaxUsedPetIndex == i)
                {
                    tPlayer.nMaxUsedPetIndex = -1;
                }
            }

            PetType ePetType = tPlayer.tPets[i].eType;


            // Inference: detached slots clear both the ID and type to their authenticated -1 sentinels.
            tPlayer.tPets[i].idACD = ACDID_INVALID;
            tPlayer.tPets[i].eType = PETTYPE_NONE;





            if (GameActiveGameCommonDataIsServer())
            {

                tACDPet.AttributesSetInt(ATTRIBUTE_PET_OWNER, -1);


                sSendDetachPetMessage(tPlayer, idACDPet, bDisplayChatMessage);





                if (ePetType == PETTYPE_FOLLOWER)
                {
                    STrickleUpdateActor(tACDPet);
                }
            }
            else
            {



                // Inference: client follower/hireling/proxy cases follow original dispatch and string references.
                switch (ePetType)
                { case PETTYPE_FOLLOWER:
                    if (bDisplayChatMessage)
                    {
                        CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "FollowerDeactivated"),
                            MonsterGetName(ACDTryToGet(idACDPet), FALSE).str(), NULL);

                        Set_Text_Color(sMsg, FONTCOLOR_SYSTEM);





                        UI_Print(sMsg, FALSE);

                    }
                    break;
                case PETTYPE_HIRELING:

                    if (bDisplayChatMessage)
                    {
                        Act eAct = GameGetCurrentAct();
                        CRefString sMsg;

                        if (ActEnumIsValid(eAct, FALSE))
                        {
                            CRefString sActName(SymbolToName(eAct, gActs));
                            sMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), CRefString().Format("HirelingDismissed_%s", sActName.str()));
                        }
                        else
                        {
                            sMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "HirelingDismissed");
                        }

                        Set_Text_Color(sMsg, FONTCOLOR_SYSTEM);





                        UI_Print(sMsg, FALSE);

                    }


                    UIHireling::OnFollowerDismissed(idACDPet);
                    break;

                case PETTYPE_HIRELING_PROXY:

                    UIHireling::OnFollowerDismissed(idACDPet);
                    break;
                }
            }

















            return;
        }
    }
}



void PlayerPetInitialize(PlayerPet & tPet)
{
    tPet.idACD = ACDID_INVALID;
    tPet.eType = PETTYPE_NONE;
}



















int32 PlayerGetPetIndex(const Player & tPlayer, ACDID idACDPet)
{
    for (int32 i = 0; i <= tPlayer.nMaxUsedPetIndex; i++)
    {
        if (tPlayer.tPets[i].idACD == idACDPet)
        {
            return i;
        }
    }
    return -1;
}







ACDID PlayerGetFirstPet(
    ACDID idACDPlayer,
    PetType eType,
    int32 & nPos)
{
    nPos = -1;
    return PlayerGetNextPet(idACDPlayer, eType, nPos);
}



ACDID PlayerGetNextPet(
    ACDID idACDPlayer,
    PetType eType,
    int32 & nPos)
{

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);

    if (!ptPlayer)
    {
        return ACDID_INVALID;
    }

    if (eType == PETTYPE_NONE)
    {
        for (++nPos; nPos <= ptPlayer->nMaxUsedPetIndex; nPos++)
        {
            if (ptPlayer->tPets[nPos].idACD != ACDID_INVALID)
            {
                return ptPlayer->tPets[nPos].idACD;
            }
        }
    }

    else
    {
        for (++nPos; nPos <= ptPlayer->nMaxUsedPetIndex; nPos++)
        {
            if (ptPlayer->tPets[nPos].idACD != ACDID_INVALID &&
                ptPlayer->tPets[nPos].eType == eType)
            {
                return ptPlayer->tPets[nPos].idACD;
            }
        }
    }

    return ACDID_INVALID;
}



ACDID PlayerGetHireling(ACDID idACDPlayer)
{
    int32 nPos;



    return PlayerGetFirstPet(idACDPlayer, PETTYPE_HIRELING, nPos);
}


ACDID PlayerGetHirelingProxy(
    ACDID idACDPlayer,
    HirelingClass eClass)
{
    int32 nPos;
    ACDID idACDPet = PlayerGetFirstPet(idACDPlayer, PETTYPE_HIRELING_PROXY, nPos);
    while (idACDPet != ACDID_INVALID)
    {
        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
        if (ptACDPet && ACDGetHirelingClass(*ptACDPet) == eClass)
        {
            return idACDPet;
        }
        idACDPet = PlayerGetNextPet(idACDPlayer, PETTYPE_HIRELING_PROXY, nPos);
    }
    return ACDID_INVALID;
}




int32 PlayerGetNumPetsOfType(ACDID idACDPlayer, PetType eType)
{

    int32 nCount = 0;

    if (idACDPlayer == ACDID_INVALID || eType < PETTYPE_FIRST || eType >= NUM_PET_TYPES)
    {
        return nCount;
    }



    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {
        return 0;
    }


    for (int32 i = 0; i <= ptPlayer->nMaxUsedPetIndex; i++)
    {
        ACDID idACDPet = ptPlayer->tPets[i].idACD;
        if (idACDPet == ACDID_INVALID)
        {
            continue;
        }



        if (GameActiveGameCommonDataIsServer())
        {
            ActorID idActorPet = (ActorID)ACDGetOwner(idACDPet);
            if (ActorFlagIsSet(idActorPet, ACTOR_DYING))
            {
                continue;
            }
        }


        if (ptPlayer->tPets[i].eType == eType)
        {
            nCount++;
        }
    }


    return nCount;
}

ACDID PlayerGetPet(ACDID idACDPlayer, int32 nIndex)
{



    if (nIndex < 0 || nIndex >= MAX_PETS)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }




    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {
        return ACDID_INVALID;
    }

    return ptPlayer->tPets[nIndex].idACD;
}


PetType PlayerGetPetType(ACDID idACDPlayer, int32 nIndex)
{



    if (nIndex < 0 || nIndex >= MAX_PETS)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }




    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {
        return PETTYPE_NONE;
    }

    return ptPlayer->tPets[nIndex].eType;
}


int32 PlayerGetPetCount(ACDID idACDPlayer)
{

    int32 nPetCount = 0;
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
    {
        return 0;
    }

    for (int32 i = 0; i <= ptPlayer->nMaxUsedPetIndex; i++)
    {
        ACDID idACDPet = ptPlayer->tPets[i].idACD;
        if (idACDPet == ACDID_INVALID)
        {
            continue;
        }


        if (GameActiveGameCommonDataIsServer())
        {
            ActorID idActorPet = (ActorID)ACDGetOwner(idACDPet);

            if (!ActorTryToGet(idActorPet))
            {
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            if (ActorFlagIsSet(idActorPet, ACTOR_DYING))
            {
                continue;
            }
        }


        nPetCount++;
    }
    return nPetCount;
}




BOOL PlayerPetShowPortrait(PetType eType)
{
    switch (eType)
    {
    case PETTYPE_WIZARDMIRRORIMAGE:
    case PETTYPE_WITCHDOCTOR_DEATH_STATUE_TOTEM:
    case PETTYPE_WITCHDOCTOR_FIRE_STATUE_TOTEM:
    case PETTYPE_WITCHDOCTOR_ZOMBIE:
    case PETTYPE_GENERIC_STATIC_PET_0:
    case PETTYPE_GENERIC_STATIC_PET_1:
    case PETTYPE_GENERIC_STATIC_PET_2:
    case PETTYPE_GENERIC_STATIC_PET_3:
    case PETTYPE_GENERIC_STATIC_PET_4:
    case PETTYPE_GENERIC_STATIC_PET_5:
    case PETTYPE_GENERIC_STATIC_PET_6:
    case PETTYPE_COMPANION:
    case PETTYPE_HIRELING_PROXY:
        return FALSE;
    default:
        break;
    }
    return TRUE;
}


BOOL PlayerPetIsDismissableByPlayer(const ActorCommonData & tACDPet)
{
    PetType ePetType = tACDPet.AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE));
    switch (ePetType)
    {
    case PETTYPE_WIZARDMIRRORIMAGE:
    case PETTYPE_WITCHDOCTOR_DEATH_STATUE_TOTEM:
    case PETTYPE_WITCHDOCTOR_FIRE_STATUE_TOTEM:
    case PETTYPE_WITCHDOCTOR_ZOMBIE:
    case PETTYPE_HELPER:
    case PETTYPE_GENERIC_STATIC_PET_0:
    case PETTYPE_GENERIC_STATIC_PET_1:
    case PETTYPE_GENERIC_STATIC_PET_2:
    case PETTYPE_GENERIC_STATIC_PET_3:
    case PETTYPE_GENERIC_STATIC_PET_4:
    case PETTYPE_GENERIC_STATIC_PET_5:
    case PETTYPE_GENERIC_STATIC_PET_6:
    case PETTYPE_FOLLOWER:
    case PETTYPE_COMPANION:
    case PETTYPE_HIRELING_PROXY:
    case PETTYPE_FOLLOWER_PROXY:
        return FALSE;
    default:
        break;
    }
    return tACDPet.AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_CANNOT_BE_DISMISSED)) == 0;
}



BOOL PlayerGetSavedQuest(const D3::Hero::SavedDefinition & tHeroDefinition, SNO snoQuest, PlayerSavedQuest & tPlayerSavedQuest)
{



    for (int32 i = 0; i < tHeroDefinition.saved_quest_size(); i++)
    {
        const D3::Hero::SavedQuest & tSavedQuest = tHeroDefinition.saved_quest(i);
        if (tSavedQuest.sno_quest() == snoQuest)
        {
            tPlayerSavedQuest.Import(tSavedQuest);
            return TRUE;
        }
    }
    return FALSE;
}




PlayerSavedQuest * PlayerGetSavedQuest(Player & tPlayer, SNO snoQuest)
{
    PlayerSavedQuest * ptSavedQuest = NULL;
    for (int32 i = 0; i < MAX_NUM_SAVED_QUESTS; i++)
    {
        PlayerSavedQuest & tSavedQuest = tPlayer.tHeroStateData.tSavedQuest[i];
        if (tSavedQuest.snoQuest == snoQuest)
        {
            ptSavedQuest = &tSavedQuest;
            break;
        }
    }
    return ptSavedQuest;
}







PlayerSavedQuest * PlayerAddSavedQuest(Player & tPlayer, SNO snoQuest)
{
    PlayerSavedQuest * ptSavedQuest = NULL;
    for (int32 i = 0; i < MAX_NUM_SAVED_QUESTS; i++)
    {
        PlayerSavedQuest & tSavedQuest = tPlayer.tHeroStateData.tSavedQuest[i];
        if (tSavedQuest.snoQuest == snoQuest)
        {
            return &tSavedQuest;
        }
        if (tSavedQuest.snoQuest == SNO_NONE)
        {
            tPlayer.uConsoleSyncFlags |= (1 << PCSF_SAVED_QUESTS);
            tSavedQuest.snoQuest = snoQuest;
            ptSavedQuest = &tSavedQuest;
            break;
        }
    }
    return ptSavedQuest;
}






































































BOOL PlayerHasReceivedQuestStepReward(const Player & tPlayer, SNO snoQuest, int32 nStepUID)
{
    DifficultyLevel eDifficulty = GameGetDifficultyLevel();
    if (eDifficulty == -1)
    {
        return FALSE;
    }

    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    if (pdefQuest->eQuestType != QUEST_TYPE_MAIN_QUEST)
    {
        return TRUE;
    }

    for (int32 i = 0; i < tPlayer.tHeroStateData.nNumQuestRewardEntries; i++)
    {
        const PlayerQuestRewardHistoryEntry & tEntry = tPlayer.tHeroStateData.tQuestRewardHistory[i];
        if (tEntry.snoQuest == snoQuest && tEntry.nStepUID == nStepUID)
        {
            return (tEntry.uDifficultyFlags >> eDifficulty) & 1;
        }
    }
    return FALSE;
}





void PlayerSavedQuestInitialize(PlayerSavedQuest & tSavedQuest);  // Player.cpp:3291
void PlayerQuestHistoryEntryInitialize(PlayerQuestHistoryEntry & tEntry);  // Player.cpp:3302
void PlayerQuestRewardHistoryEntryInitialize(PlayerQuestRewardHistoryEntry & tEntry);  // Player.cpp:3312

void PlayerClearQuestData(
    Player & tPlayer)
{

    for (int32 i = 0; i < MAX_NUM_SAVED_QUESTS; i++)
    {
        PlayerSavedQuestInitialize(tPlayer.tHeroStateData.tSavedQuest[i]);
    }

    for (int32 i = 0; i < NUM_QUEST_HISTORY_ENTRIES; i++)
    {
        PlayerQuestHistoryEntryInitialize(tPlayer.tHeroStateData.tQuestHistory[i]);
    }

    tPlayer.tHeroStateData.nNumQuestRewardEntries = 0;
    for (int32 i = 0; i < NUM_QUEST_REWARD_ENTRIES; i++)
    {
        PlayerQuestRewardHistoryEntryInitialize(tPlayer.tHeroStateData.tQuestRewardHistory[i]);
    }
}



Player * PlayerFindClosest(const WorldPlace & tPlace, const WorldDistance wdRadius)
{

    WorldDistance wdPlayerClosestDistSq = 0.0f;
    WorldDistance wdRadiusSq = wdRadius * wdRadius;
    Player * ptPlayerClosest = NULL;

    for (Player * ptPlayer = PlayerGetFirstInGame();
         ptPlayer;
         ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        WorldPlace tPlacePlayer = tACDPlayer.GetPlace();


        if (!WorldPlaceInSameWorld(tPlace, tPlacePlayer))
        {
            continue;
        }


        WorldDistance wdDistSq = WorldPlaceDistanceSquared(tPlace, tPlacePlayer);


        if (wdDistSq >= wdRadiusSq)
        {
            continue;
        }



        if (ptPlayerClosest == NULL)
        {
            ptPlayerClosest = ptPlayer;
            wdPlayerClosestDistSq = wdDistSq;
        }
        else if (wdDistSq < wdPlayerClosestDistSq)
        {
            ptPlayerClosest = ptPlayer;
            wdPlayerClosestDistSq = wdDistSq;
        }
    }

    return ptPlayerClosest;
}











void PlayersFindInRadius(const WorldPlace & tPlace, const WorldDistance wdRadius, XPooledDataIDList & tListPlayerACDs)
{


    const WorldDistance wdRadiusSquared = wdRadius * wdRadius;


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        if (!tACDPlayer.IsInWorld())
        {
            continue;
        }

        const WorldPlace tPlacePlayer = tACDPlayer.GetPlace();


        if (!WorldPlaceInSameWorld(tPlace, tPlacePlayer))
        {
            continue;
        }


        if (Vector3D::DistanceSquared(tPlace.wp, tPlacePlayer.wp) <= wdRadiusSquared)
        {
            tListPlayerACDs.AddTail_Tracked(ptPlayer->idPlayerACD);
        }
    }
}



void PlayersFindInOBB(SWorldID idSWorld, const OBB & tOBB, XPooledDataIDList & tListPlayerACDs)
{



    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        const ACDID idACDPlayer = ptPlayer->idPlayerACD;
        ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
        if (!tACDPlayer.IsInWorld())
        {
            continue;
        }


        const WorldLocation & tWorldLocation = tACDPlayer.GetWorldLocation();


        if (tWorldLocation.idSWorld != idSWorld)
        {
            continue;
        }



        Sphere ws;
        ws.wpCenter = tWorldLocation.transform.wp;
        ws.wdRadius = 0.25f;

        if (Intersect::SphereAndOBB(ws, tOBB, NULL))
        {
            tListPlayerACDs.AddTail_Tracked(idACDPlayer);
        }
    }
}




























BOOL PlayerIsInLevelArea(const ACDID idACDPlayer, const SNO snoLevelArea)
{
    SNO snoLevelAreaPlayer = ACDGetLevelArea(ACDGet(idACDPlayer));
    return snoLevelAreaPlayer == snoLevelArea;
}




BOOL PlayerHasActivatedWaypoint(const Player & tPlayer, const int32 nWaypointNumber)
{
    if (GameIsOpenWorld())
    {
        return TRUE;
    }

    if (nWaypointNumber >= 0 && nWaypointNumber < 32)
    {
        int32 nActivatedWaypoints = CountSetBits((const uint8 *)&tPlayer.tHeroStateData.tSavedData.m_ActivatedWaypoints, sizeof(tPlayer.tHeroStateData.tSavedData.m_ActivatedWaypoints));
        if (nActivatedWaypoints < 2)
        {
            return FALSE;
        }

        return (tPlayer.tHeroStateData.tSavedData.m_ActivatedWaypoints >> nWaypointNumber) & 1;
    }
    return FALSE;
}





















static const int32 sgnNumAllowedPortalWorldTags = 8;

BOOL EscortCanBePulledThroughWarp(const ActorCommonData & tACDEscort, const ActorCommonData & tACDPlayer)
{


    if (tACDEscort.IsLoading())
    {
        return FALSE;
    }

    ScopedActorDefinitionLockR pdefActor(tACDEscort.snoActor);
    BOOL bRestrictAllowedWorlds = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_RESTRICT_ALLOWED_WORLDS);
    if (!bRestrictAllowedWorlds)
    {
        return TRUE;
    }

    SWorldID idSWorld = tACDPlayer.GetWorldLocation().idSWorld;
    WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);
    if (!ptWCD)
    {
        return FALSE;
    }

    for (int32 i = 0; i < sgnNumAllowedPortalWorldTags; i++)
    {
        SNO snoWorld = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_ALLOWED_PORTAL_WORLD_0 + i * (TAG_ACTOR_ALLOWED_PORTAL_WORLD_1 - TAG_ACTOR_ALLOWED_PORTAL_WORLD_0));
        if (snoWorld == ptWCD->snoWorld)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void PlayerSavedQuestInitialize(PlayerSavedQuest & tSavedQuest)
{
    SigmaMemoryClear(&tSavedQuest, sizeof(tSavedQuest));
    tSavedQuest.snoQuest = SNO_NONE;
    tSavedQuest.nCurrentStepUID = -1;
}





void PlayerQuestHistoryEntryInitialize(PlayerQuestHistoryEntry & tEntry)
{
    SigmaMemoryClear(&tEntry, sizeof(tEntry));
    tEntry.nStepUID = -1;
    tEntry.snoQuest = SNO_NONE;
}




void PlayerQuestRewardHistoryEntryInitialize(PlayerQuestRewardHistoryEntry & tEntry)
{
    SigmaMemoryClear(&tEntry, sizeof(tEntry));
    tEntry.snoQuest = SNO_NONE;
    tEntry.nStepUID = -1;
    tEntry.uDifficultyFlags = 0;
}




void PlayerCrafterInitialize(CrafterData & tCrafterData)
{
    tCrafterData.nNumRecipes = 0;
    memset(tCrafterData.gbidRecipes, 0xff, sizeof(tCrafterData.gbidRecipes));
    tCrafterData.nLevel = 0;
}









PlayerAccount PlayerGetAccount(const ACDID idACDPlayer)
{
    const Player * ptPlayer = PlayerGetByACD(idACDPlayer);

    return ptPlayer->tAccount;
}





CRefString PlayerGetClassUIName(const PlayerClass ePlayerClass, const BOOL fIsMale)
{


    CRefString sLabel;
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        sLabel = "PlayerClassDemonHunter";
        break;
    case PLAYER_CLASS_BARBARIAN:
        sLabel = "PlayerClassBarbarian";
        break;
    case PLAYER_CLASS_WIZARD:
        sLabel = "PlayerClassWizard";
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        sLabel = "PlayerClassWitchDoctor";
        break;
    case PLAYER_CLASS_MONK:
        sLabel = "PlayerClassMonk";
        break;
    case PLAYER_CLASS_CRUSADER:
        sLabel = "PlayerClassCrusader";
        break;
    case PLAYER_CLASS_NECROMANCER:
        sLabel = "PlayerClassNecromancer";
        break;
    default:
        sLabel = "PlayerClassUnknown";
        break;
    }



    if (fIsMale)
    {
        sLabel += "Male";
    }
    else
    {
        sLabel += "Female";
    }


    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), sLabel);
}





















LPCSTR PlayerGetShortClassName(PlayerClass ePlayerClass)
{
    LPCSTR szPlayerClass;
    switch (ePlayerClass)
    {
    case PLAYER_CLASS_DEMONHUNTER: szPlayerClass = "DHunter"; break;
    case PLAYER_CLASS_BARBARIAN: szPlayerClass = "Barb"; break;
    case PLAYER_CLASS_WIZARD: szPlayerClass = "Wiz"; break;
    case PLAYER_CLASS_WITCHDOCTOR: szPlayerClass = "WitchDoc"; break;
    case PLAYER_CLASS_MONK: szPlayerClass = "Monk"; break;
    case PLAYER_CLASS_CRUSADER: szPlayerClass = "Crusader"; break;
    case PLAYER_CLASS_NECROMANCER: szPlayerClass = "Necromancer"; break;
    default: szPlayerClass = "UNKNOWN PLAYER CLASS"; break;
    }
    return szPlayerClass;
}


















BOOL PlayerIsPlayerGameFlagSet(const Player * ptPlayer, int32 nPlayerFlagIndex)
{
    return FlagSetGetFlag(ptPlayer->arGameFlags, sizeof(ptPlayer->arGameFlags), nPlayerFlagIndex);
}






BOOL PlayerIsPlayerGameFlagSet(const Player * ptPlayer, LPCSTR szLabel)
{
    int32 nFlagIndex = FlagSetLookup(GlobalSNOGet(GLOBALSNO_FLAGSET_PLAYER_FLAGS), szLabel);
    return PlayerIsPlayerGameFlagSet(ptPlayer, nFlagIndex);
}









void PlayerDisplayErrorMessage(ACDID idACDPlayer, GameError eError)
{





    IntDataMessage tMsg;
    tMsg.nData = eError;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);

    MessageSendToClient(listDestPlayers, SMSG_DISPLAY_ERROR, &tMsg, sizeof(tMsg));
}






TrickleInfoType PlayerGetTrickleInfoType(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACD)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    Player * ptTrickledPlayer = PlayerGetByACD(tACD.id);

    if (ptTrickledPlayer)
    {

        if (PlayersAreLocalToEachOther(*ptPlayer, *ptTrickledPlayer))
        {
            return TRICKLE_INFO_LOCAL_PLAYER;
        }

        if (ACDIsFriendly(tACDPlayer, tACD, FALSE))
        {
            return TRICKLE_INFO_FRIENDLY_PLAYER;
        }


        return TRICKLE_INFO_HOSTILE_PLAYER;
    }

    else if (tACD.IsPet())
    {
        if (ACDIsFriendly(tACDPlayer, tACD, FALSE))
        {
            if (ACDGetPetOwner(tACD.id) == tACDPlayer.id)
            {
                return TRICKLE_INFO_MY_PET;
            }
            else if (ACDIsFollower(tACD))
            {
                return TRICKLE_INFO_FOLLOWER_PET;
            }
        }




        else
        {
            return TRICKLE_INFO_HOSTILE_PLAYER_PET;
        }
    }

    else if (tACD.IsNPC())
    {
        return TRICKLE_INFO_NPC;
    }
    else if (tACD.IsMonster())
    {
        if (GameTypeIsPvP())
        {
            if (tACD.GetTeam() == 1)
            {
                return TRICKLE_INFO_PVP_NEUTRAL;
            }
            return ACDIsFriendly(tACDPlayer, tACD, FALSE) ? TRICKLE_INFO_PVP_FRIENDLY : TRICKLE_INFO_PVP_HOSTILE;
        }








        else
        {
            return ACDIsFriendly(tACDPlayer, tACD, FALSE) ? TRICKLE_INFO_FRIENDLY_MONSTER : TRICKLE_INFO_HOSTILE_MONSTER;
        }






    }
    else if (tACD.IsGizmo())
    {
        if (GameTypeIsPvP())
        {
            return TRICKLE_INFO_PVP_NEUTRAL;
        }
        return TRICKLE_INFO_GIZMO;
    }




    return TRICKLE_INFO_INVALID;
}



uint32 PlayerGetPlayedSeconds(const Player & tPlayer)
{
    return tPlayer.tHeroStateData.tSavedData.uSecondsPlayed;
}





void PlayerRemoveItemFromTradeOffer(Player * ptPlayer, const ActorCommonData & tACDItem)
{

    if (!ptPlayer || !TradeGetTradingTarget(*ptPlayer))
    {
        return;
    }

    ACDNetworkName annItem = ANNFromACD(tACDItem.id);
    for (int32 i = 0; i < 4; i++)
    {
        if (ptPlayer->tTradeData.arrItems[i] == annItem)
        {
            ptPlayer->tTradeData.arrItems[i] = ANN_INVALID;

            if (GameActiveGameCommonDataIsServer())
            {
                STradeTriggerUpdate(*ptPlayer, TRUE);
            }
        }
    }
}








































int32 PlayerGetNumAvailableSkillSlots(const ActorCommonData & tACDPlayer)
{
    int32 nLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    for (int32 i = 0; i < 6; i++)
    {
        if (nLevel < cgnActiveSkillSlotLevelReq[i])
        {
            return i;
        }
    }
    return 6;
}






















void PlayerAssignToActiveSkillSlot(Player & tPlayer, const SNO snoPower, const RuneType eRune, const int32 nSlot)
{
    ActiveSkillSavedData & tSavedData = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot];
    tSavedData.snoSkill = snoPower;
    tSavedData.eRuneType = eRune;
    tSavedData.dwLastLifeOnHitTick = 0;
    tSavedData.idChargesTimer = TIMERID_INVALID;
}







SNO PlayerGetPowerInActiveSkillSlot(const Player & tPlayer, const int32 nSlot, RuneType * peRuneType)
{
    SNO snoPower = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot].snoSkill;
    if (peRuneType && snoPower != SNO_NONE)
    {
        *peRuneType = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot].eRuneType;
    }
    return snoPower;
}







void PlayerAssignToTraitSlot(Player & tPlayer, const SNO snoPower, const int32 nSlot)
{
    tPlayer.tHeroStateData.tSavedData.snoTraits[nSlot] = snoPower;
}







SNO PlayerGetPowerInTraitSlot(const Player & tPlayer, const int32 nSlot)
{
    return tPlayer.tHeroStateData.tSavedData.snoTraits[nSlot];
}






void PlayerAssignToLegendaryPowerSlot(Player & tPlayer, const GBID gbidLegendaryItem, const int32 nSlot)
{
    tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[nSlot] = gbidLegendaryItem;
}







GBID PlayerGetLegendaryPowerInSlot(const Player & tPlayer, const int32 nSlot)
{
    return tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[nSlot];
}







BOOL PlayerAlreadyPurchasedActiveSkill(
    const Player & tPlayer,
    const SNO snoPower,
    int32 & nSlotOfExistingSkill)
{
    PowerHotbarExclusiveType eHotbarExclusiveType = PowerGetHotbarExclusiveType(snoPower);

    nSlotOfExistingSkill = -1;
    for (int32 nSlot = 0; nSlot < 6; ++nSlot)
    {
        SNO snoPowerCmp = tPlayer.tHeroStateData.tSavedData.tActiveSkills[nSlot].snoSkill;
        if (snoPowerCmp != SNO_NONE &&
            (snoPowerCmp == snoPower ||
             (eHotbarExclusiveType != 0 &&
              eHotbarExclusiveType == PowerGetHotbarExclusiveType(snoPowerCmp))))
        {
            nSlotOfExistingSkill = nSlot;
            return TRUE;
        }
    }

    return FALSE;
}






















void HeroStateData::SetDefaults()
{
    eLastPlayedAct = 0;
    eHighestUnlockedAct = 0;
    uPlayerFlags = 0;
    uPlayerFlags2 = 0;






    for (int32 i = 0; i < MAX_NUM_SAVED_QUESTS; i++)
    {
        PlayerSavedQuestInitialize(tSavedQuest[i]);
    }

    for (int32 i = 0; i < 200; i++)
    {
        PlayerQuestHistoryEntryInitialize(tQuestHistory[i]);
    }

    for (int32 i = 0; i < 6; i++)
    {
        uBossKillFlags[i] = 0;
    }

    nNumQuestRewardEntries = 0;
    for (int32 i = 0; i < 100; i++)
    {
        PlayerQuestRewardHistoryEntryInitialize(tQuestRewardHistory[i]);
    }

    QuestsVersionForPlayer(*this);
    tSavedData.SetDefaults();
}

void CurrencySavedData::SetToDefaults()
{
    for (int32 i = 0; i < NUM_CURRENCY_TYPES; i++)
    {
        arCurrency[i].eType = i;
        arCurrency[i].nCount = 0;
        arCurrency[i].uFlags = 0;
    }
}




void CurrencySavedData::Export(
    D3::Items::CurrencySavedData & tCurrencySavedData,
    ExportPartitionScope ePartitionScope,
    ExportDirtyScope eDirtyScope,
    BOOL bExportFlags,
    uint32 uFlagsToClear)
{
    uint32 uClearMask = ~uFlagsToClear;

    for (int32 i = 0; i < NUM_CURRENCY_TYPES; ++i)
    {
        // Inference: partition 2 is account-wide; flags 1/2 are persistent/client dirty bits, from original tests.
        if ((ePartitionScope == EXPORT_PARTITION_INDIVIDUAL && i == CURRENCY_PLATINUM) ||
            (ePartitionScope == EXPORT_PARTITION_ACCOUNT_WIDE && i != CURRENCY_PLATINUM))
        {
            continue;
        }



        if ((eDirtyScope == EXPORT_DIRTY_PERSIST && !(arCurrency[i].uFlags & 1)) ||
            (eDirtyScope == EXPORT_DIRTY_CLIENTSYNC && !(arCurrency[i].uFlags & 2)))
        {

            if (i < tCurrencySavedData.currency_size())
            {
                continue;
            }
        }



        D3::Items::CurrencyData * ptCurrencyData = tCurrencySavedData.add_currency();
        ptCurrencyData->set_id(arCurrency[i].eType);
        ptCurrencyData->set_count(arCurrency[i].nCount);








        arCurrency[i].uFlags &= uClearMask;

        ptCurrencyData->set_gbid_deprecated(GBID_NONE);
    }

}



CurrencySavedData::CurrencySavedData()
{

    SetToDefaults();
}


void CurrencySavedData::Import(
    const D3::Items::CurrencySavedData & tCurrencySavedData,
    BOOL bResetCurrency)
{
    if (bResetCurrency)
    {
        SetToDefaults();
    }

    int32 nCurrencyDataCount = tCurrencySavedData.currency_size();
    for (int32 i = 0; i < nCurrencyDataCount; ++i)
    {
        const D3::Items::CurrencyData & tData = tCurrencySavedData.currency(i);
        Import(tData);
    }
}



void CurrencySavedData::Import(
    const D3::Items::CurrencyData & tCurrencyData)
{
    CurrencyType eType = tCurrencyData.id();


    arCurrency[eType].nCount = tCurrencyData.count();
    if (tCurrencyData.has_data_persist_flags())
    {
        arCurrency[eType].uFlags = tCurrencyData.data_persist_flags();
    }
}


void PlayerQueuedPower::SetDefaults()
{

    dwPowerLastStartTime = 0;
    dwPowerLastInterruptTime = 0;
    dwPowerLastEndTime = 0;

    Clear();
}


void PlayerQueuedPower::Clear()
{
    snoQueuedPower = SNO_NONE;
    dwQueuedTime = 0;
    tQueuedTarget.Clear();
    nQueuedComboLevel = 0;
    idACDQueuedItemBeingUsed = ACDID_INVALID;
}



void PlayerVendorData::Initialize()
{

    idACDEnchantAffixPendingItem = ACDID_INVALID;
    gbidEnchantAffixPendingRemoved = GBID_NONE;
    nEnchantAffixChoicesCount = 0;
}


void PlayerVendorData::VendorReset()
{
    idACDEnchantAffixPendingItem = ACDID_INVALID;
    gbidEnchantAffixPendingRemoved = GBID_NONE;
    nEnchantAffixChoicesCount = 0;
}




BOOL PlayerIsInWorld(const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer && ptACDPlayer->GetLocationType() == AL_WORLD)
    {
        return TRUE;
    }
    return FALSE;
}





BOOL PlayerCanUsePortal(const ActorCommonData & tACDPlayer)
{
    uint32 dwNextPortalTime = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_PORTAL_NEXT_TIME));
    return GameGetServerTime() >= dwNextPortalTime;
}










BOOL PlayerCanChangeSkillorTrait(const ActorCommonData & tACDPlayer, BOOL bForHotbarSwapOnly)
{
    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_GOD)) || tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALLOW_SKILL_CHANGES)))
    {
        return TRUE;
    }

    if ((CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_PVP_BIT) || CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT)) && !bForHotbarSwapOnly)
    {
        return FALSE;
    }

    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_BOSS_ENCOUNTER)))
    {
        return FALSE;
    }

    if (ACDIsParticipatingInTieredLootRun(tACDPlayer))
    {
        return FALSE;
    }

    return !ACDIsParticipatingInSetDungeon(tACDPlayer);
}




















BOOL PlayerCanChangeParagon(const ActorCommonData & tACDPlayer)
{
    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_GOD)))
    {
        return TRUE;
    }

    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALLOW_SKILL_CHANGES)))
    {
        return TRUE;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_PVP_BIT))
    {
        return FALSE;
    }

    return !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
}

BOOL PlayerCanChangeLoadoutSkillsAndTraits(
    const Player & tPlayer,
    int32 nLoadoutIndex)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }

    const LoadoutSavedData * ptLoadout = NULL;
    if (nLoadoutIndex >= 0 && nLoadoutIndex < 10)
    {
        ptLoadout = &tPlayer.tLoadouts[nLoadoutIndex];
    }

    for (int32 i = 0; i < 6; ++i)
    {
        SNO snoSkillInSlot = PlayerGetPowerInActiveSkillSlot(tPlayer, i, NULL);
        if (!ptLoadout || ptLoadout->tActiveSkills[i].snoPower != snoSkillInSlot)
        {
            if (!PowerIsCoolDownDone(*ptACDPlayer, snoSkillInSlot))
            {
                return FALSE;
            }

            if (ptLoadout && !PowerIsCoolDownDone(*ptACDPlayer, ptLoadout->tActiveSkills[i].snoPower))
            {
                return FALSE;
            }
        }
    }



    for (int32 i = 0; i < 4; ++i)
    {
        SNO snoTraitInSlot = PlayerGetPowerInTraitSlot(tPlayer, i);
        if (!ptLoadout || ptLoadout->snoTraits[i] != snoTraitInSlot)
        {
            if (!PowerIsCoolDownDone(*ptACDPlayer, snoTraitInSlot))
            {
                return FALSE;
            }

            if (ptLoadout && !PowerIsCoolDownDone(*ptACDPlayer, ptLoadout->snoTraits[i]))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}




extern "C" BOOL sub_7100506d10(const Player & tPlayer, GBID gbidItem, LegendaryPowerSlotType eSlotType);  // Items.cpp, 2.7.7 only (unnamed)
// NON_MATCHING: 2 of 101 instructions: the original sign-extends nLoadoutIndex (sxtw) before the first madd, ours folds it into smaddl.
BOOL PlayerCanChangeLoadoutEquipment(const Player & tPlayer, int32 nLoadoutIndex)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }
    if (nLoadoutIndex < 0 || nLoadoutIndex >= 10)
    {
        return TRUE;
    }
    for (int32 i = 0; i < 4; i++)
    {
        if (!sub_7100506d10(tPlayer, tPlayer.tLoadouts[nLoadoutIndex].gbidLegendaryPowers[i], (LegendaryPowerSlotType)i))
        {
            return FALSE;
        }
    }
    return TRUE;
}















// NON_MATCHING: Clang branches around rune reads and uses a different final potion comparison.
BOOL PlayerLoadoutMatchesCurrentEquipment(
    const Player & tPlayer,
    int32 nLoadoutIndex)
{
    if (nLoadoutIndex < 0 || nLoadoutIndex >= 10)
    {
        return FALSE;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }

    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nLoadoutIndex];
    if (tLoadout.bEmpty)
    {
        return FALSE;
    }
    // Inference: original unsigned iteration covers each of the thirteen equipped body slots.
    for (int32 nBodySlotIndex = 0; nBodySlotIndex < NUM_PLAYER_BODYSLOTS; ++nBodySlotIndex)
    {
        const LoadoutItemData & tLoadoutItem = tLoadout.tEquipment[nBodySlotIndex];
        InventorySlotType eInvSlot = SLOT_PLAYER_HEAD + nBodySlotIndex;

        ACDID idACDCurrentItem = ACDInventoryGetSquareItem(*ptACDPlayer, eInvSlot, cgptNull);
        ActorCommonData * ptACDCurrentItem = ACDTryToGet(idACDCurrentItem);
        if (!ptACDCurrentItem)
        {
            if (tLoadoutItem.annItem != ANN_INVALID)
            {
                return FALSE;
            }
            continue;
        }


        if (tLoadoutItem.annItem != ANNFromACD(idACDCurrentItem))
        {
            return FALSE;
        }

        if (!ItemHasSockets(*ptACDCurrentItem))
        {
            if (tLoadoutItem.nSocketsCount > 0)
            {
                return FALSE;
            }
            continue;
        }


        int32 nFilledSockets = ptACDCurrentItem->AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
        if (nFilledSockets != tLoadoutItem.nSocketsCount)
        {
            return FALSE;
        }

        for (int32 nSocket = 0; nSocket < tLoadoutItem.nSocketsCount; ++nSocket)
        {
            IVector2D tPoint;
            tPoint.Set(0, nSocket);
            ACDID idACDSocketedItem = ACDInventoryGetSquareItem(*ptACDCurrentItem, SLOT_ITEM_SOCKETS, tPoint);
            ActorCommonData * ptACDSocketedItem = ACDTryToGet(idACDSocketedItem);
            if (!ptACDSocketedItem)
            {
                return FALSE;
            }

            if (!PlayerLoadoutSocketablesMatch(ptACDSocketedItem->hGB.gbid, tLoadoutItem.gbidSockets[nSocket]))
            {
                return FALSE;
            }
        }
    }
    RuneType eRuneType;
    for (int32 i = 0; i < 6; ++i)
    {
        SNO snoCurrentPower = PlayerGetPowerInActiveSkillSlot(tPlayer, i, &eRuneType);
        if (snoCurrentPower != tLoadout.tActiveSkills[i].snoPower)
        {
            return FALSE;
        }

        if (tLoadout.tActiveSkills[i].snoPower != SNO_NONE && eRuneType != tLoadout.tActiveSkills[i].eRune)
        {
            return FALSE;
        }
    }

    for (int32 i = 0; i < 4; ++i)
    {
        SNO snoCurrentTrait = PlayerGetPowerInTraitSlot(tPlayer, i);
        if (snoCurrentTrait != tLoadout.snoTraits[i])
        {
            return FALSE;
        }
    }

    for (int32 i = 0; i < 4; ++i)
    {
        GBID gbidCurrentLegendaryPower = tPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[i];
        if (gbidCurrentLegendaryPower != tLoadout.gbidLegendaryPowers[i])
        {
            return FALSE;
        }
    }

    ACDNetworkName annLoadoutPotion = tLoadout.tEquippedPotion.annPotion;
    if (annLoadoutPotion == ANN_INVALID)
    {
        if (PlayerGetPotionBtnAssignment(&tPlayer).m_gbidItem == GlobalGBIDGet(GLOBALGBID_ITEM_HEALTHPOTION_BOTTOMLESS))
            return TRUE;

        InventoryLocation tInvLoc;
        ACDInventoryLocationSet(tInvLoc, tPlayer.idPlayerACD, SLOT_PLAYER_BASE_HEALTHPOTION, 0, 0);
        ACDID idPotion = ACDInventoryGetItemInSlot(tInvLoc);
        annLoadoutPotion = ANNFromACD(idPotion);
    }

    if (!ACDTryToGet(ACDFromANN(annLoadoutPotion)))
    {
        return FALSE;
    }

    const HotbarButtonData & tButtonData = PlayerGetPotionBtnAssignment(&tPlayer);
    if (tButtonData.m_annItem != annLoadoutPotion)
        return FALSE;
    return TRUE;





}



BOOL PlayerCanChangeLoadout(
    const Player & tPlayer,
    int32 nLoadoutIndex,
    GameError * peGameError)
{
    if (peGameError)
    {
        *peGameError = GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }

    if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
    {
        if (peGameError)
        {
            if (ACDIsParticipatingInTieredLootRun(*ptACDPlayer))
            {
                *peGameError = GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_IS_IN_GRIFT;
            }
        }
        return FALSE;
    }

    if (!PlayerCanChangeLoadoutSkillsAndTraits(tPlayer, nLoadoutIndex))
    {
        if (peGameError)
        {
            *peGameError = GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_SKILL_ON_COOLDOWN;
        }
        return FALSE;
    }

    if (!PlayerCanChangeLoadoutEquipment(tPlayer, nLoadoutIndex))
    {
        return FALSE;
    }

    if (PlayerLoadoutMatchesCurrentEquipment(tPlayer, nLoadoutIndex))
    {
        if (peGameError)
        {
            *peGameError = GAMEERROR_EQUIPMENT_MANAGER_CANNOT_CHANGE_LOADOUT_CURRENTLY_EQUIPPED;
        }
        return FALSE;
    }

    return TRUE;
}



BOOL PlayerCanViewSkillorTrait(const ActorCommonData & tACDPlayer)
{
    if (PlayerCanChangeSkillorTrait(tACDPlayer, FALSE))
    {
        return TRUE;
    }

    if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !tACDPlayer.IsInCombat() && GameIsWeeklyChallenge())
    {
        return TRUE;
    }

    return FALSE;
}









BOOL PlayerLoadoutItemIsInValidInventorySlot(const ActorCommonData & tACDItem, ACDID idACDPlayer)
{
    if (!tACDItem.IsItem())
    {
        return FALSE;
    }

    if (tACDItem.GetLocationType() != AL_INVENTORY)
    {
        return FALSE;
    }

    if (tACDItem.GetInventoryContainerOwner() != idACDPlayer)
    {
        return FALSE;
    }

    InventorySlotType eSlot = tACDItem.GetLocationSlot();
    if (eSlot == SLOT_PLAYER_BACKPACK || eSlot == SLOT_PLAYER_SHARED_STASH)
    {
        return TRUE;
    }
    for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; i++)
    {
        if (eSlot == i)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void PlayerInventoryIterateLoadoutEquipmentSlots(
    const ActorCommonData & tACD,
    SlotSearchCallback pfnCallBack,
    void * pCallBackData)
{
    for (int32 i = 0; i < 15; ++i)
    {
        ACDInventoryIterateSlot(tACD, scarLoadoutEquipmentSlots[i], NULL, pfnCallBack, pCallBackData, FALSE, FALSE);
    }
}




extern "C" BOOL sub_71004f8950(GBID gbidItem);  // Items.cpp

static BOOL sPlayerLoadoutItemUpdateSockets(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    LoadoutItemData * ptLoadoutItem = (LoadoutItemData *)pCallBackData;
    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);
    ActorCommonData * ptACDItem = ACDTryToGet(ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt));
    if (ptACDItem)
    {
        ptLoadoutItem->gbidSockets[ptLoadoutItem->nSocketsCount] = ptACDItem->hGB.gbid;
        ptLoadoutItem->nSocketsCount++;
        if (sub_71004f8950(ptACDItem->hGB.gbid) && GameActiveGameCommonDataIsServer())
            ptLoadoutItem->uSoulShardIdLow = ItemGenerateConsoleEquipmentManagerID(*ptACDItem);
    }

    return TRUE;
}







void PlayerLoadoutItemUpdate(LoadoutItemData & tLoadoutItem, const ActorCommonData & tACDItem)
{
    tLoadoutItem.annItem = ANNFromACD(tACDItem.id);

    if (GameActiveGameCommonDataIsServer())
    {
        uint64 uFakeItemId = ItemGenerateConsoleEquipmentManagerID(tACDItem);
        tLoadoutItem.uOriginalItemIdLow = uFakeItemId;
    }

    tLoadoutItem.nSocketsCount = 0;

    if (ItemHasSockets(tACDItem))
    {
        ACDInventoryIterateSlot(tACDItem, SLOT_ITEM_SOCKETS, NULL, sPlayerLoadoutItemUpdateSockets, &tLoadoutItem, FALSE, FALSE);
    }
}














void PlayerLoadoutPotionUpdate(LoadoutPotionData & tLoadoutPotion, const ActorCommonData & tACDItem)
{
    tLoadoutPotion.annPotion = ANNFromACD(tACDItem.id);

    if (GameActiveGameCommonDataIsServer())
    {
        uint64 uFakeItemId = ItemGenerateConsoleEquipmentManagerID(tACDItem);
        tLoadoutPotion.uOriginalItemIdLow = uFakeItemId;
    }

    tLoadoutPotion.gbidPotion = tACDItem.hGB.gbid;
}












void PlayerLoadoutGetDesiredSocketables(
    GBIDArray & arGBIDSocketables,
    const Player & tPlayer,
    int32 nLoadoutIndex)
{
    if (nLoadoutIndex < 0 || nLoadoutIndex >= 10)
    {
        return;
    }

    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nLoadoutIndex];
    if (tLoadout.bEmpty)
    {
        return;
    }

    for (int32 nOrderedSlot = 0; cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] != SLOT_NONE; ++nOrderedSlot)
    {
        int32 nEquipmentSlotIndex = cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] - 1;
        const LoadoutItemData & tLoadoutItem = tLoadout.tEquipment[nEquipmentSlotIndex];

        if (!ACDTryToGet(ACDFromANN(tLoadoutItem.annItem)))
        {
            continue;
        }

        for (int32 nSocket = 0; nSocket < tLoadoutItem.nSocketsCount; ++nSocket)
        {
            GBID gbidSocketable = tLoadoutItem.gbidSockets[nSocket];
            // Inference: bExists starts FALSE and becomes TRUE on the original matching early exit.
            BOOL bExists = FALSE;
            for (auto it = arGBIDSocketables.begin(); it != arGBIDSocketables.end(); ++it)
            {
                if (PlayerLoadoutSocketablesMatch(gbidSocketable, *it))
                {
                    bExists = TRUE;
                    break;
                }
            }

            if (!bExists)
            {
                arGBIDSocketables.push_back(gbidSocketable);
            }
        }
    }
}




enum LoadoutSocketablePriority
{
    LSP_LIMBO,
    LSP_INVENTORY_STACK,
    LSP_INVENTORY_SOCKET,
    LSP_EQUIPPED_ITEM_SOCKET,
    LSP_STASH_STACK,
    LSP_STASH_SOCKET,
    LSP_OTHER,
    LSP_NUM_TYPES
};


struct FindLoadoutSocketablesCallbackData
{
    XPooledDataIDList & listACDIDSocketables;
    const GBIDArray * arGBIDSocketables;

    FindLoadoutSocketablesCallbackData(XPooledDataIDList & _listACDIDSocketables, const GBIDArray * _arGBIDSocketables) : listACDIDSocketables(_listACDIDSocketables), arGBIDSocketables(_arGBIDSocketables) {}

private:
    FindLoadoutSocketablesCallbackData(FindLoadoutSocketablesCallbackData &);
    FindLoadoutSocketablesCallbackData & operator=(FindLoadoutSocketablesCallbackData &);
};

static LoadoutSocketablePriority sGetPlayerLoadoutSocketablePriority(
    const ActorCommonData & tACDSocketable)
{
    if (!tACDSocketable.IsInInventory())
    {
        return LSP_LIMBO;
    }

    const InventoryLocation & tInvLocation = tACDSocketable.GetInventoryLocation();

    if (tInvLocation.eSlot == SLOT_PLAYER_BACKPACK)
    {
        return LSP_INVENTORY_STACK;
    }
    else if (tInvLocation.eSlot == SLOT_PLAYER_SHARED_STASH)
    {
        return LSP_STASH_STACK;
    }
    else if (tInvLocation.eSlot == SLOT_ITEM_SOCKETS)
    {
        const ActorCommonData * ptACDContainer = ACDTryToGet(tInvLocation.idACDContainer);
        if (!ptACDContainer->IsInInventory())
        {
            return LSP_LIMBO;
        }

        const InventoryLocation & tInvLocationContainer = ptACDContainer->GetInventoryLocation();

        if (tInvLocationContainer.eSlot == SLOT_PLAYER_BACKPACK)
        {
            return LSP_INVENTORY_SOCKET;
        }
        else if (tInvLocationContainer.eSlot == SLOT_PLAYER_SHARED_STASH)
        {
            return LSP_STASH_SOCKET;
        }
        else if (tInvLocationContainer.eSlot >= SLOT_PLAYER_HEAD && tInvLocationContainer.eSlot <= SLOT_PLAYER_NECK)
        {
            return LSP_EQUIPPED_ITEM_SOCKET;
        }
    }

    return LSP_OTHER;
}

static int32 sGetPlayerLoadoutSocketableRank(
    const ActorCommonData & tACDSocketable)
{
    if (ItemIsUpgradeableJewel(tACDSocketable.hGB.gbid))
    {
        int32 nJewelRank = tACDSocketable.AttributesGetInt(FastAttribKey(ATTRIBUTE_JEWEL_RANK));
        return nJewelRank;
    }

    int32 nGemLevel = ItemCrafting_GetGemQuality(tACDSocketable.hGB.gbid);
    return nGemLevel;
}

static void sPlayerLoadoutTrackSocketable(
    FindLoadoutSocketablesCallbackData & tSocketableData,
    const ActorCommonData & tACDSocketable)
{
    if (tSocketableData.arGBIDSocketables)
    {
        BOOL bValidGBID = FALSE;
        for (const int * it = tSocketableData.arGBIDSocketables->begin(); it != tSocketableData.arGBIDSocketables->end(); ++it)
        {
            if (PlayerLoadoutSocketablesMatch(tACDSocketable.hGB.gbid, *it))
            {
                bValidGBID = TRUE;
                break;
            }
        }
        if (!bValidGBID)
        {
            return;
        }
    }


    POS posInsert = NULL;
    BOOL bFoundMatch = FALSE;
    POS posSocketables = tSocketableData.listACDIDSocketables.GetHeadPosition();
    while (posSocketables)
    {
        POS posPrevious = posSocketables;
        ACDID idACDSocketable = ACDID(tSocketableData.listACDIDSocketables.GetNext(posSocketables));

        ActorCommonData * ptACDExistingSocketable = ACDTryToGet(idACDSocketable);
        if (PlayerLoadoutSocketablesMatch(ptACDExistingSocketable->hGB.gbid, tACDSocketable.hGB.gbid))
        {

            if (idACDSocketable == tACDSocketable.id)
            {
                return;
            }


            if (posInsert)
            {
                continue;
            }

            bFoundMatch = TRUE;
            int32 nSocketableRank = sGetPlayerLoadoutSocketableRank(tACDSocketable);
            int32 nExistingSocketableRank = sGetPlayerLoadoutSocketableRank(*ptACDExistingSocketable);

            if (nSocketableRank > nExistingSocketableRank)
            {
                posInsert = posPrevious;
            }
            else if (nSocketableRank == nExistingSocketableRank)
            {
                LoadoutSocketablePriority eSocketablePriority = sGetPlayerLoadoutSocketablePriority(tACDSocketable);
                LoadoutSocketablePriority eExistingSocketablePriority = sGetPlayerLoadoutSocketablePriority(*ptACDExistingSocketable);

                if (eSocketablePriority <= eExistingSocketablePriority)
                {
                    posInsert = posPrevious;
                }
            }
        }
        else if (bFoundMatch)
        {
            if (!posInsert)
                posInsert = posPrevious;
            break;
        }

    }


    if (posInsert)
    {
        tSocketableData.listACDIDSocketables.InsertBefore_Tracked(posInsert, tACDSocketable.id);
    }
    else
    {
        tSocketableData.listACDIDSocketables.AddTail_Tracked(tACDSocketable.id);
    }
}

static void sPlayerLoadoutFindSocketablesCallback(
    ACDID idACDSquare,
    InventorySlotType eSlot,
    void * pUserData)
{


    ActorCommonData * ptACDSocketable = ACDTryToGet(idACDSquare);
    if (!ptACDSocketable || !ItemIsSocketable(*ptACDSocketable))
    {
        return;
    }

    FindLoadoutSocketablesCallbackData * ptData = static_cast<FindLoadoutSocketablesCallbackData *>(pUserData);
    sPlayerLoadoutTrackSocketable(*ptData, *ptACDSocketable);
}

void PlayerLoadoutFindSocketables(
    XPooledDataIDList & listACDIDSocketables,
    const ActorCommonData & tACDPlayer,
    const GBIDArray * arGBIDSocketables)
{
    if (arGBIDSocketables && arGBIDSocketables->size() == 0)
    {
        return;
    }

    FindLoadoutSocketablesCallbackData tData(listACDIDSocketables, arGBIDSocketables);

    for (int32 i = SLOT_PLAYER_HEAD; i <= SLOT_PLAYER_NECK; ++i)
    {
        ACDInventoryIterateItems(tACDPlayer, i, ACDII_IGNORE_SOCKET_SLOTS, sPlayerLoadoutFindSocketablesCallback, &tData);
    }

    ACDInventoryIterateItems(tACDPlayer, SLOT_PLAYER_BACKPACK, ACDII_IGNORE_SOCKET_SLOTS, sPlayerLoadoutFindSocketablesCallback, &tData);
    ACDInventoryIterateItems(tACDPlayer, SLOT_PLAYER_SHARED_STASH, ACDII_IGNORE_SOCKET_SLOTS, sPlayerLoadoutFindSocketablesCallback, &tData);
}







static BOOL sPlayerLoadoutUnsocketItemToLimbo(
    const InventoryLocation & tInvLoc,
    void * pUserData)
{
    ActorCommonData & tACD = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACD, tInvLoc.eSlot, tInvLoc.pt);

    ActorCommonData * ptACDSocketable = ACDTryToGet(idACDItem);
    if (!ptACDSocketable || !ItemIsSocketable(*ptACDSocketable))
    {
        return TRUE;
    }

    ItemRemoveSocketedItemAttributes(*ptACDSocketable, tACD);
    SACDInventoryAddToDirtyList(*ptACDSocketable, AIDA_REMOVE, ptACDSocketable->GetLocationSlot(), SLOT_NONE);
    ACDInventoryRemoveToLimbo(tInvLoc.idACDContainer, *ptACDSocketable, TRUE);




    if (pUserData)
    {
        FindLoadoutSocketablesCallbackData * ptData = static_cast<FindLoadoutSocketablesCallbackData *>(pUserData);
        sPlayerLoadoutTrackSocketable(*ptData, *ptACDSocketable);
    }

    return TRUE;
}



void PlayerLoadoutUnsocketItemToLimbo(
    const ActorCommonData & tACDItem,
    XPooledDataIDList * plistACDIDSocketables)
{
    if (!ItemHasSockets(tACDItem))
    {
        return;
    }

    if (plistACDIDSocketables)
    {
        FindLoadoutSocketablesCallbackData tData(*plistACDIDSocketables, NULL);
        ACDInventoryIterateSlot(tACDItem, SLOT_ITEM_SOCKETS, NULL, sPlayerLoadoutUnsocketItemToLimbo, &tData, FALSE, FALSE);
    }
    else
    {
        ACDInventoryIterateSlot(tACDItem, SLOT_ITEM_SOCKETS, NULL, sPlayerLoadoutUnsocketItemToLimbo, NULL, FALSE, FALSE);
    }
}





BOOL PlayerLoadoutSocketablesMatch(
    GBID gbidItem1,
    GBID gbidItem2)
{
    if (ItemIsUpgradeableJewel(gbidItem1))
    {
        if (gbidItem1 == gbidItem2)
        {
            return TRUE;
        }
    }
    else
    {
        ScopedItemDefinitionLockR pdefItem1(gbidItem1);
        ScopedItemDefinitionLockR pdefItem2(gbidItem2);

        if (pdefItem1 && pdefItem2 && pdefItem1->eGemType == pdefItem2->eGemType)
        {
            return TRUE;
        }
    }

    return FALSE;
}



BOOL PlayerCanOpenInventory(const ActorCommonData & tACDPlayer)
{
    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_GOD)) || tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALLOW_SKILL_CHANGES)))
    {
        return TRUE;
    }

    return !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
}









// NON_MATCHING: 448 vs 396; the original shares one "return FALSE" block for every failing check and keeps the TRUE returns duplicated; ours duplicates the FALSE exits. Relaid out on DWARF lines (4878-4923), still differs
BOOL PlayerCanManipulateInventory(
    const ActorCommonData & tACDPlayer)
{
    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_GOD) ||
        tACDPlayer.AttributesGetInt(ATTRIBUTE_ALLOW_SKILL_CHANGES))
    {
        return TRUE;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }

    if (GameIsWeeklyChallenge())
    {
        return FALSE;
    }

    if (NephalemRift::IsTieredRiftActive())
    {
        BOOL bParticipating = FastAttribGetValueInt(tACDPlayer.idFastAttrib, FastAttribKey(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN));
        if (bParticipating)
        {
            GameSyncedData & tGameSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
            int32 nRiftTier = tGameSyncedData.nOpenLootRunLevel;
            if (nRiftTier >= 0 && !tGameSyncedData.bLootRunBossDead)
            {
                return FALSE;
            }
        }
    }

    if (ACDIsParticipatingInSetDungeon(tACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
        if (!ptPlayer || ptPlayer->bSetDungeonActive)
        {
            return FALSE;
        }
    }



    return TRUE;
}



BOOL PlayerCanOpenQuests()
{
    if (!GameIsParamFlagSet(GPF_QUEST_UI))
    {
        return FALSE;
    }

    return !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
}










BOOL PlayerCanAttemptVoteKick(
    const Player & tPlayer)
{
    ScopedGlobalDefinitionLockR pdefGlobals;
    uint32 dwNow = GameGetServerTime();

    uint32 dwAttemptDelayTicks = (uint32)(pdefGlobals->flVoteKickAttemptDelay * 3600.0f);
    if ((tPlayer.dwLastKickAttempt + dwAttemptDelayTicks) > dwNow)
    {
        return FALSE;
    }

    // inferred: dwSuccessfulDelayTicks declared here (DWARF decl line 4959), tests from columns 4960:9/17/42/75
    uint32 dwSuccessfulDelayTicks = (uint32)(pdefGlobals->flVoteKickSuccessfulDelay * 3600.0f);
    if (tPlayer.dwLastSuccessfulKick > 0 && (tPlayer.dwLastSuccessfulKick + dwSuccessfulDelayTicks) > dwNow)
    {
        return FALSE;
    }

    return TRUE;
}



int32 PlayerGetTieredLootRunCorpseResurrectionTimeRemaining(
    const ActorCommonData & tACDPlayer)
{
    int32 nAllowedResTime = tACDPlayer.AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
    if (nAllowedResTime)
    {
        int32 nRemainingTime = 0;


        if (GameActiveGameCommonDataIsServer())
        {
            nRemainingTime = nAllowedResTime - GameGetServerTime();
        }



        if (GameActiveGameCommonDataIsClient())
        {
            nRemainingTime = nAllowedResTime - CGameGetPredictedServerTime();
        }

        return nRemainingTime;
    }

    return 0;
}





BOOL PlayerCanReviveAtCorpse(
    const ActorCommonData & tACDPlayer)
{

    if (GameCommonDataExists() && GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx >= 0)
    {
        return FALSE;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return TRUE;
    }


    int32 nCorpseDelay = tACDPlayer.AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
    int32 nResurrectionCharges = tACDPlayer.AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_CHARGES);
    if (nCorpseDelay > 0 || nResurrectionCharges < 1)
    {
        return FALSE;
    }


    GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;
    if (ACDIsInTieredLootRun(tACDPlayer) && !tSyncedData.bTieredLootRunFailed && NephalemRift::GetParticipatingPlayerCount() > 1)
    {
        int32 nRemainingTime = PlayerGetTieredLootRunCorpseResurrectionTimeRemaining(tACDPlayer);



        if (nRemainingTime > 0) return FALSE;
    }


    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);

    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
    {


        for (Player * ptCheckPlayer = PlayerGetFirstAll(); ptCheckPlayer; ptCheckPlayer = PlayerGetNextAll(ptCheckPlayer))
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptCheckPlayer->idPlayerACD);
            if (ptACDPlayer && ptACDPlayer->AttributesGetFloat(ATTRIBUTE_HITPOINTS_CUR) > 0.000001f &&
                (PlayersAreLocalToEachOther(*ptPlayer, *ptCheckPlayer) || tACDPlayer.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER)))
            {
                return TRUE;
            }
        }

        return FALSE;
    }










    if (ptPlayer && ((ptPlayer->tHeroStateData.uPlayerFlags >> PLAYERFLAG_HARDCORE) & 1) &&
        !tACDPlayer.IsDueling() &&
        !GameIsWeeklyChallenge())
    {
        return FALSE;
    }

    return !tACDPlayer.AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_DISABLED);
}













BOOL PlayerCanReviveAtCheckpoint(const ActorCommonData & tACDPlayer)
{
    return TRUE;
}







BOOL PlayerAffectedByDurability(const ActorCommonData & tACDPlayer)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }

    if (GameIsParamFlagSet(GPF_IGNORE_DURABILITY_LOSS))
    {
        return FALSE;
    }

    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_PLAYER_ALL_ITEMS_INDESTRUCTIBLE)) > 0)
    {
        return FALSE;
    }

    return tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL)) >= 10;
}






static BOOL sGameIsPastServerTime(uint32 uServerTime)
{
    uint32 uCurrentServerTime = 0;
    if (GameActiveGameCommonDataIsServer())
    {
        uCurrentServerTime = GameGetServerTime();
    }
    if (GameActiveGameCommonDataIsClient())
    {
        uCurrentServerTime = CGameGetPredictedServerTime();
    }
    return uCurrentServerTime > uServerTime;
}











BOOL PlayerIsEngagedWithRare(uint32 uEngagedWithRareTime)
{
    return !sGameIsPastServerTime(uEngagedWithRareTime);
}




BOOL PlayerIsEngagedWithTreasureGoblin(uint32 uEngagedWithGoblinTime)
{
    return !sGameIsPastServerTime(uEngagedWithGoblinTime);
}




BOOL PlayerIsHardcoreAndDead(const Player & tPlayer)
{
    if (((tPlayer.tHeroStateData.uPlayerFlags >> PLAYERFLAG_HARDCORE) & 1) && !GameTypeIsPvP())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer && !ptACDPlayer->IsAlive() && !ptACDPlayer->IsDueling())
        {
            return TRUE;
        }
    }
    return FALSE;
}






BOOL PlayersAreLocalToEachOther(const Player & tPlayer0, const Player & tPlayer1)
{
    return tPlayer0.idGameConnection == tPlayer1.idGameConnection;
}





BOOL PlayersAreHostileToEachOther(const Player * ptPlayer0, const Player * ptPlayer1)
{
    return FALSE;
}























































SNO PlayerGetPowerFallbackPower(const Player & tPlayer)
{

    const ActorCommonData * ptACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACD)
    {
        return SNO_NONE;
    }

    SNO snoSkillKit = ptACD->GetSkillKit();
    if (snoSkillKit == SNO_NONE || snoSkillKit == INVALID_SNO)
    {
        return SNO_NONE;
    }

    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);

    int32 nCount = pdefSkillKit->serPowerFallbacks.dwSize / sizeof(SNO);
    for (int x = 0; x < nCount; x++)
    {
        SNO snoFallbackEntry = pdefSkillKit->arPowerFallbacks[x];
        if (ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoFallbackEntry)) > 0)
        {
            return snoFallbackEntry;
        }
    }

    return SNO_NONE;
}




int32 PlayerGetParagonPointsTotalForCategory(const ActorCommonData & tACDPlayer, int32 nCategory)
{
    int32 nAltLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL));
    int32 nPointsTotal = (3 - nCategory + Math::Min(800, nAltLevel)) / 4;
    if (nCategory == 0 && nAltLevel > 800)
    {
        nPointsTotal += nAltLevel - 800;
    }
    return nPointsTotal;
}












extern "C" int32 sub_71005272f0(const ActorCommonData & tACDPlayer, GBID gbidParagonBonus)
{
    ScopedParagonBonusLockR pdefBonus(gbidParagonBonus);
    return tACDPlayer.AttributesGetInt(ATTRIBUTE_PARAGON_CAP_ENABLED) ? pdefBonus->unk_1c : pdefBonus->nPointsMax;
}

extern "C" BOOL sub_7100527370(const ActorCommonData & tACDPlayer)
{
    return tACDPlayer.AttributesGetInt(ATTRIBUTE_PARAGON_CAP_ENABLED);
}

extern "C" const AttributeSpecifier * sub_7100527380(const ActorCommonData & tACDPlayer, GBID gbidBonus, uint32 nIndex)
{
    ScopedParagonBonusLockR pdefBonus(gbidBonus);
    return sub_7100527370(tACDPlayer) ? &pdefBonus->unk_80[nIndex] : &pdefBonus->ptAttributes[nIndex];
}

extern "C" const AttributeSpecifier * sub_7100527410(const ActorCommonData & tACDPlayer, GBID gbidBonus, uint32 nIndex)
{
    ScopedParagonBonusLockR pdefBonus(gbidBonus);
    return sub_7100527370(tACDPlayer) ? &pdefBonus->unk_80[nIndex] : &pdefBonus->ptAttributes[nIndex];
}

extern "C" int32 sub_71005274a0(const ActorCommonData & tACDPlayer)
{
    return sub_7100527370(tACDPlayer) ? unk_19167f8.Get() / 4 : 0x1fffffff;
}

extern "C" void sub_7100527880(const ActorCommonData & tACDPlayer, int32 * pnCaps);

// NON_MATCHING: Overflow guards and category cap loads and stores are scheduled differently.
extern "C" void sub_7100527730(const ActorCommonData & tACDPlayer, int32 * pnPoints)
{
    int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
    int32 arCaps[4] = {};
    sub_7100527880(tACDPlayer, arCaps);
    int32 nBase = nLevel / 4;
    int32 nRemainder = nLevel % 4;
    int32 nPrimary = nBase + (nRemainder > 0);
    if (nPrimary < arCaps[0])
    {
        for (int32 i = 1; i < 4; ++i)
        {
            int32 nExtra = nBase + (nRemainder > i) - arCaps[i];
            if (nExtra > 0 && nExtra < 0x7fffffff - nPrimary)
                nPrimary = Math::Min(nPrimary + nExtra, arCaps[0]);
        }
    }
    else
        nPrimary = arCaps[0];

    pnPoints[0] = nPrimary;
    for (int32 i = 1; i < 4; ++i)
        pnPoints[i] = Math::Min(nBase + (nRemainder > i), arCaps[i]);
}

extern "C" void sub_7100527880(const ActorCommonData & tACDPlayer, int32 * pnCaps)
{
    int32 nCategoryCap = sub_71005274a0(tACDPlayer);
    GBHandleList listBonuses(GBGetHandlePool());
    GBEnumerate(GB_PARAGON_BONUSES, listBonuses);
    POS pos = listBonuses.GetHeadPosition();
    while (pos)
    {
        const GBHandle & hBonus = listBonuses.GetNext(pos);
        ScopedParagonBonusLockR pdefBonus(hBonus.gbid);
        int32 nCategory = pdefBonus->nCategory;
        int32 nMax = sub_71005272f0(tACDPlayer, hBonus.gbid);
        int32 nCurrent = pnCaps[nCategory];
        if (0x7fffffff - nCurrent > nMax)
            nCurrent += nMax;
        pnCaps[nCategory] = Math::Min(nCurrent, nCategoryCap);
    }
}

extern "C" BOOL sub_71005274e0(const ActorCommonData & tACDPlayer)
{
    int32 arPoints[4];
    sub_7100527730(tACDPlayer, arPoints);
    int32 arSpent[4] = {};
    GBHandleList listBonuses(GBGetHandlePool());
    GBEnumerate(GB_PARAGON_BONUSES, listBonuses);
    POS pos = listBonuses.GetHeadPosition();
    while (pos)
    {
        const GBHandle & hBonus = listBonuses.GetNext(pos);
        ScopedParagonBonusLockR pdefBonus(hBonus.gbid);
        int32 nSpent = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_PARAGON_BONUS, hBonus.gbid));
        arSpent[pdefBonus->nCategory] += nSpent;
    }
    for (int32 i = 0; i < 4; ++i)
    {
        if (arSpent[i] < arPoints[i])
            return FALSE;
    }
    return TRUE;
}

Player * PlayerGetPrimaryPlayerForGameConnection(GameConnectionID idGameConnection)
{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->idGameConnection == idGameConnection && ptPlayer->bIsPrimaryPlayerServerOnly)
        {
            return ptPlayer;
        }
    }
    return NULL;
}






int32 PlayerGetNumberPlayersSharingGameConnection(const Player * ptPlayer)
{
    int32 nCount = 0;
    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
    {
        if (ptPlayer->idGameConnection == ptOtherPlayer->idGameConnection)
        {
            nCount++;
        }
    }
    return nCount;
}






extern "C" int32 sub_71004b8980(const ActorCommonData * ptACDPlayer, int32 nTicks);
float PlayerGetStreakTimerTolerance(const Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    ScopedGlobalDefinitionLockR pdefGlobals;
    float flTolerance;
    if (PlayerGetNumPlayers() > 1)
        flTolerance = pdefGlobals->flMassacreStreakTimerCoop;
    else if (ptACDPlayer)
        flTolerance = HeroIsRanged(ptACDPlayer->id) ? pdefGlobals->flMassacreStreakTimerRanged : pdefGlobals->flMassacreStreakTimerMelee;
    else
        return pdefGlobals->flMassacreStreakTimerMelee;
    if (ptACDPlayer)
        flTolerance = sub_71004b8980(ptACDPlayer, (int32)(flTolerance * 60.0f)) / 60.0f;
    return flTolerance;



}




BOOL PlayerIsHardcore(const Player * ptPlayer)
{
    return (ptPlayer->tHeroStateData.uPlayerFlags >> PLAYERFLAG_HARDCORE) & 1;
}





FloatingNumberType FloatingNumberTypeFromCurrency(const CurrencyType eCurrency)
{
    switch (eCurrency)
    {
    case CURRENCY_GOLD:
    default:
        return FLOATING_NUMBER_TYPE_GOLD;
    case CURRENCY_BLOODSHARDS:
        return FLOATING_NUMBER_TYPE_BLOODSHARDS;
    case CURRENCY_PLATINUM:
        return FLOATING_NUMBER_TYPE_PLATINUM;
    }
}





BOOL PlayerEligibleForDailyFirst(const Player & tPlayer, FastAttribKey tKey)
{
    if (GameGetDailyPlatinumLimit() == 0)
    {
        return FALSE;
    }

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    const uint32 SECONDS_IN_A_DAY = 86400;
    uint32 nLastTime = tACDPlayer.AttributesGetInt(tKey);
    uint32 nNow = Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp());
    uint32 nUnixTimeOfPrevReset = (nNow / SECONDS_IN_A_DAY) * SECONDS_IN_A_DAY + GameGetDailyPlatinumResetTime();
    if (nUnixTimeOfPrevReset > nNow)
    {
        nUnixTimeOfPrevReset -= SECONDS_IN_A_DAY;
    }
    return nLastTime < nUnixTimeOfPrevReset;
}



















static blz::pair<int, int> arParagonPortraits[19] =
{
    blz::pair<int, int>(0, GLOBALGBID_PORTRAIT_PARAGON_0),
    blz::pair<int, int>(1, GLOBALGBID_PORTRAIT_PARAGON_1),
    blz::pair<int, int>(10, GLOBALGBID_PORTRAIT_PARAGON_10),
    blz::pair<int, int>(20, GLOBALGBID_PORTRAIT_PARAGON_20),
    blz::pair<int, int>(30, GLOBALGBID_PORTRAIT_PARAGON_30),
    blz::pair<int, int>(40, GLOBALGBID_PORTRAIT_PARAGON_40),
    blz::pair<int, int>(50, GLOBALGBID_PORTRAIT_PARAGON_50),
    blz::pair<int, int>(60, GLOBALGBID_PORTRAIT_PARAGON_60),
    blz::pair<int, int>(70, GLOBALGBID_PORTRAIT_PARAGON_70),
    blz::pair<int, int>(80, GLOBALGBID_PORTRAIT_PARAGON_80),
    blz::pair<int, int>(90, GLOBALGBID_PORTRAIT_PARAGON_90),
    blz::pair<int, int>(100, GLOBALGBID_PORTRAIT_PARAGON_100),
    blz::pair<int, int>(200, GLOBALGBID_PORTRAIT_PARAGON_200),
    blz::pair<int, int>(300, GLOBALGBID_PORTRAIT_PARAGON_300),
    blz::pair<int, int>(400, GLOBALGBID_PORTRAIT_PARAGON_400),
    blz::pair<int, int>(500, GLOBALGBID_PORTRAIT_PARAGON_500),
    blz::pair<int, int>(600, GLOBALGBID_PORTRAIT_PARAGON_600),
    blz::pair<int, int>(700, GLOBALGBID_PORTRAIT_PARAGON_700),
    blz::pair<int, int>(800, GLOBALGBID_PORTRAIT_PARAGON_800),
};



int32 PlayerGetParagonLevelFromPortraitGBID(GBID gbidPortrait)
{
    for (int i = 0; i < (int)(sizeof(arParagonPortraits) / sizeof(arParagonPortraits[0])); i++)
    {
        if (GlobalGBIDGet(arParagonPortraits[i].second) == gbidPortrait)
        {
            return arParagonPortraits[i].first;
        }
    }

    return -1;
}






GBID PlayerGetPortraitGBIDFromParagonLevel(int32 nLevel)
{
    GBID gbidPortrait = GBID_NONE;

    for (int32 i = 0; i < (int32)(sizeof(arParagonPortraits) / sizeof(arParagonPortraits[0])); i++)
    {

        if (nLevel < arParagonPortraits[i].first)
        {
            break;
        }
        gbidPortrait = GlobalGBIDGet(arParagonPortraits[i].second);
    }

    return gbidPortrait;
}







GBIDArray PlayerGetUnlockedPortraitsForParagonLevel(int32 nLevel)
{

    GBIDArray arPortraits;

    for (int32 i = 0; i < (int32)(sizeof(arParagonPortraits) / sizeof(arParagonPortraits[0])); i++)
    {
        if (arParagonPortraits[i].first <= nLevel)
        {
            arPortraits.push_back(GlobalGBIDGet(arParagonPortraits[i].second));
        }
    }

    return arPortraits;
}



bool PlayerCanPlayHandicap(int32 nLevel, int32 nHighestRiftLevel, X1HandicapLevel eHandicap, bool bPublicGame)
{
    if (bPublicGame)
    {
        return nLevel >= scgarGameHandicapLevelLimitations[eHandicap].m_publicGameLimitation.m_nRequiredLevel &&
            nHighestRiftLevel >= scgarGameHandicapLevelLimitations[eHandicap].m_publicGameLimitation.m_nRequiredHighestSoloRiftLevel;
    }
    else
    {
        return nLevel >= scgarGameHandicapLevelLimitations[eHandicap].m_privateGameLimitation.m_nRequiredLevel &&
            nHighestRiftLevel >= scgarGameHandicapLevelLimitations[eHandicap].m_privateGameLimitation.m_nRequiredHighestSoloRiftLevel;
    }
}






















BOOL PlayerHasUnlockedChallengeMode(int32 nControllerIndex)
{
    const D3::Account::SavedDefinition & tAccountDefinition = Console::GamerProfile::GetAccount(nControllerIndex);
    return tAccountDefinition.digest().completed_solo_rift();
}








void PlayerAddOnGroundCraftingPlan(Player & tPlayer, GBID gbidPlan)
{
    if (!tPlayer.listOnGroundCraftingPlans.Find(gbidPlan))
    {
        tPlayer.listOnGroundCraftingPlans.AddTail_Tracked(gbidPlan);
    }
}





BOOL PlayerHasOnGroundCraftingPlan(const Player & tPlayer, GBID gbidPlan)
{
    return tPlayer.listOnGroundCraftingPlans.Find(gbidPlan) != NULL;
}





void PlayerRemoveOnGroundCraftingPlan(Player & tPlayer, GBID gbidPlan)
{

    POS posGBID = tPlayer.listOnGroundCraftingPlans.Find(gbidPlan);
    if (posGBID)
    {
        tPlayer.listOnGroundCraftingPlans.RemoveAt(posGBID);
    }
}

// Probably: loadout socketable helper (2.7.7; caller UIEquipmentManager::AssignLoadoutItemSocketables): when the
// loadout item's recorded socketable (LoadoutItemData::unk_20, an ACDNetworkName) is in listACDIDSocketables,
// move that entry to the head of the list so it is assigned first.
extern "C" void sub_71005285b0(const LoadoutItemData & tLoadoutItem, XPooledDataIDList & listACDIDSocketables)
{
    if ((ACDNetworkName)tLoadoutItem.unk_20 == ANN_INVALID)
        return;

    const ActorCommonData * ptACDSocketable = ACDTryToGet(ACDFromANN((ACDNetworkName)tLoadoutItem.unk_20));
    if (ptACDSocketable == NULL)
        return;

    const ACDNetworkName annSocketable = ptACDSocketable->ann;
    POS pos = listACDIDSocketables.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        const ActorCommonData * ptACD = ACDTryToGet((ACDID)listACDIDSocketables.GetNext(pos));
        if (ptACD != NULL && ptACD->ann == annSocketable)
        {
            listACDIDSocketables.MoveToHead(posCurrent);
            return;
        }
    }
}

#line 173 "X/Common/Player.cpp"
const InventorySlotType cgarPlayerLoadoutSocketableSlotsOrder[9] = { SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_LEGS, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_NECK, SLOT_NONE };
#line 5590 "X/Common/Player.cpp"

extern "C" BOOL sub_710072e0b0(uint32 uSeasonCreated, uint32 uFlags);

extern "C" BOOL sub_7100527d30(const Player *ptPlayer)
{
    return sub_710072e0b0(ptPlayer->uSeasonCreated, ptPlayer->tHeroStateData.uPlayerFlags2);
}

struct FindMatchingPotionData {
    GBID gbidPotionToFind;
    ACDID idACDFoundPotion;
    InventoryLocation tInvLoc;

    uint64 unk_18;

    FindMatchingPotionData()
        : idACDFoundPotion(ACDID_INVALID)
    {
        tInvLoc.idACDContainer = (ACDID)0;
        tInvLoc.eSlot = 0;
    }
};
// NON_MATCHING: Clang merges the GBID comparisons and reorders the failed lookups.
static BOOL sFindMatchingPotionCallback(
    const InventoryLocation & tInvLoc,
    void * pCallBackData)
{
    FindMatchingPotionData * ptPotionData = (FindMatchingPotionData *)pCallBackData;
    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDItem == ACDID_INVALID)
        return TRUE;

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
        return TRUE;

    const uint64 uItemId = ptPotionData->unk_18;
    BOOL bMatch;
    if (uItemId)
    {
        if (!ActorTryToGet((ActorID)ptACDItem->idOwner))
            return TRUE;
        bMatch = uItemId == ItemGenerateConsoleEquipmentManagerID(*ptACDItem) &&
            ptACDItem->hGB.gbid == ptPotionData->gbidPotionToFind;
    }
    else
    {
        bMatch = ptACDItem->hGB.gbid == ptPotionData->gbidPotionToFind;
    }

    if (bMatch)
    {
        ptPotionData->idACDFoundPotion = idACDItem;
        ptPotionData->tInvLoc = tInvLoc;
        return FALSE;
    }
    return TRUE;
}

extern "C" BOOL sub_71005265f0(
    const ActorCommonData & tACDPlayer,
    GBID gbidPotion,
    uint64 uItemId,
    ActorCommonData *& ptACDMatchingPotion,
    InventoryLocation & tMatchingPotionLoc)
{
    FindMatchingPotionData tFindPotionCallbackData;
    tFindPotionCallbackData.gbidPotionToFind = gbidPotion;
    tFindPotionCallbackData.unk_18 = uItemId;
    ACDInventoryIterateSlot(tACDPlayer, SLOT_PLAYER_BACKPACK, NULL, sFindMatchingPotionCallback, &tFindPotionCallbackData, FALSE, FALSE);
    if (tFindPotionCallbackData.idACDFoundPotion == ACDID_INVALID)
        ACDInventoryIterateSlot(tACDPlayer, SLOT_PLAYER_SHARED_STASH, NULL, sFindMatchingPotionCallback, &tFindPotionCallbackData, FALSE, FALSE);

    if (tFindPotionCallbackData.idACDFoundPotion == ACDID_INVALID)
        return FALSE;

    ptACDMatchingPotion = ACDTryToGet(tFindPotionCallbackData.idACDFoundPotion);
    tMatchingPotionLoc = tFindPotionCallbackData.tInvLoc;
    return TRUE;
}

BOOL SPlayerFindMatchingPotion(
    const ActorCommonData & tACDPlayer,
    GBID gbidPotion,
    ActorCommonData *& ptACDMatchingPotion,
    InventoryLocation & tMatchingPotionLoc)
{
    return sub_71005265f0(tACDPlayer, gbidPotion, 0, ptACDMatchingPotion, tMatchingPotionLoc);
}
