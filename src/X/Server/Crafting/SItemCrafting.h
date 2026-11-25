// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Crafting/SItemCrafting.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 CrafterType;

typedef int32 ItemLabel;


#include "../../Common/CommonTypes.h"
#include "../../Common/GameBalance.h"

struct Player;
struct InventoryLocation;
class ActorCommonData;
typedef int32 InventorySlotType;  // X/Common/ActorCommonData.h

// Functions defined in SItemCrafting.cpp (declarations added as they are written).
namespace ItemCrafting_BackEnd {
ACDID CraftItem(GBID gbidRecipe, Player & tPlayer);  // line 59
BOOL SpendGold(ActorCommonData & tACD, int32 nGold);  // line 93
void RepairItem(ACDID idACDSquare, InventorySlotType, void * pUserData);  // line 122
BOOL UnSocket(const InventoryLocation & tInvLoc, void * pCallBackData);  // line 151
}  // namespace ItemCrafting_BackEnd
void SItemCrafting_RemoveGems(ActorCommonData & tACDItem, Player & tPlayer);  // line 210
BOOL SItemCrafting_TryUnlockTransmog(GBID gbidTransmog, Player & tPlayer, BOOL bMustBeInInventory, BOOL bSendFullUpdate);  // line 296
void SItemCrafting_SendPlayerInfo(const Player & tPlayer, const CrafterType eType, const BOOL fInitialInfo);  // line 2169
void SItemCrafting_SendPlayerTransmogInfo(const Player & tPlayer, const BOOL bInitialInfo);  // line 2189
namespace D3 { namespace Items { class Generator; } }
struct LootSpecifier;
struct ItemSpecifierData;
BOOL SItemCrafting_GenerateItemLootSpecifierFromRecipe(const ItemSpecifierData & tSpecificItem, LootSpecifier & tSpecifier, const Player * ptPlayer, BOOL bTargetedForPlayerClass);  // line 2205
void SItemCrafting_GenerateItemGeneratorFromRecipe(D3::Items::Generator & tGenerator, const LootSpecifier & tSpecifier, const ItemSpecifierData & tSpecificItem);  // line 2235
ACDID SItemCrafting_GenerateItemFromRecipe(GBID gbidRecipe, const Player * ptPlayer);  // line 2311
BOOL SItemCrafting_TryRepair(ACDID idACDItem, Player & tPlayer);  // line 2404
void SItemCrafting_InitSavedData(Player &);  // line 3010
void sCrafterOnLevelUp(const Player & tPlayer, CrafterType eType, const BOOL fChangedRank, const BOOL fSendToClient);  // line 3016
BOOL SCrafterIncrementLevel(Player & tPlayer, CrafterType eType, uint32 nLevels);  // line 3070
void SCrafterUnlock(Player & tPlayer, const CrafterType eType);  // line 3114

// ---------------------------------------------------------------------------------------------
// Not part of the original SItemCrafting.h: declarations SItemCrafting.cpp needs from headers that
// are not adopted yet. They live here so SItemCrafting.cpp keeps its functions on their original
// lines; move each to its real header once that header is adopted.
#include "../../Common/ActorInventory.h"
#include "../../Common/Crafting/ItemCrafting.h"
#include "../../Common/Definitions/TreasureClassDefinition.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/WorldPlace.h"
#include "../Loot/Loot.h"
#include "../../Common/Game.h"
#include "../../Sigma/Random.h"

// X/Common/Networking/GameMessage.h (not in the adopted subset yet).













BOOL ItemPlayerConsumeRecipeReagents(const Player & tPlayer, GBID gbidRecipe);  // X/Common/Items.cpp
BOOL ItemPlayerConsumeReforgeReagents(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem);  // X/Common/Items.cpp:8411
ACDID SItemReforge(ActorCommonData & tACDOldItem, Player & tPlayer);  // X/Server/SItems.cpp:979
BOOL SItemApplyDye(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, DyeType eDyeType);  // X/Server/SItems.cpp:1260
int32 ItemGetStatsDataKey(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:767
BOOL ItemPlayerChargeTransmogCosts(ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem, GBID gbidTransmog);  // X/Common/Items.cpp
BOOL ItemPlayerChargeDyeItemCosts(ActorCommonData & tACDPlayer, DyeType eDyeType);  // X/Common/Items.cpp:8512
void VisualInventoryResetSlot(const ACDID idACD, const InventorySlotType eInventorySlot);  // X/Common/VisualInventory.cpp:589
int32 ACDGetTieredLootRunLevel(const ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:9307
BOOL SItemUpgradeJewel(ActorCommonData & tACDItem, int32 nUpgradeRanks, ActorCommonData & tACDOwner);  // X/Server/SItems.cpp:1380
namespace NephalemRiftManager {  // X/Server/System/SNephalemRift.cpp
void PlayerSetJewelUpgradesUsed(Player * ptPlayer, int32 nUsed);
void PlayerSetJewelBonusUpgradesLeft(Player * ptPlayer, int32 nRemaining);
BOOL PlayersAreStillChoosingRewards(const Player * ptPlayerToIgnore);
void SendPlayersDoneChoosingRewardsEvent(ACDID idACDPlayer);
}

#include "../Achievements/AchievementModifierEvaluationLogic.h"




















// Declared here from their definitions elsewhere (headers not adopted yet).
void SGameStatsRecordGoldSpent(ActorCommonData & tACDPlayer, uint64 uGoldSpent);  // X/Server/SGameStats.cpp:833
void LootGetSpecifierForSpecifiedItem(LootSpecifier & tSpecifier, const ItemSpecifierData & tSpecificItem, const SharedLooteeParams & tLooteeParams, const Player * ptPlayer, BOOL bTargetRandomClass);  // X/Server/Loot/Loot.cpp:1968
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // X/Common/Player.cpp:2318
BOOL SACDInventoryPickupOrSpillOntoGround(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const BOOL bTryStashFirst, ACDID * pidACDStackDest, const BOOL bAllowAutoEquip);  // X/Common/ActorInventory.cpp:5218
ACDID SItemGenerate(const OnlineService::ItemId & tItemId, const D3::Items::Generator & tGenerator, const WorldPlace & tPlace, const float flScale, uint32 tName);  // X/Server/SItems.cpp:337
void LootDetermineRareName(ActorCommonData & tACD, const LootSpecifier & tSpecifier);  // X/Server/Loot/Loot.cpp:5608
DataID SACDInventoryAddToDirtyList(const ActorCommonData & tACDItem, ActorInventoryDatabaseAction eAction, InventorySlotType eCameFrom, InventorySlotType eDestination);  // X/Common/ActorInventory.cpp:11891
void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:9033
void ItemModifyDurability(const ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, int32 nDurabilityChange);  // X/Common/Items.cpp:8729
BOOL ItemIsLegendary(GBID gbidItem);  // X/Common/Items.cpp:5080

