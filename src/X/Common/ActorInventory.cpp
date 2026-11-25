// Original file: X/Common/ActorInventory.cpp
//
// Partly written. Functions sit on their original line numbers (asserts embed __LINE__);
// blank lines stand in for functions nobody has written yet.
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "ActorCommonData.h"
#include "ActorInventory.h"
#include "ItemsSwig.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Items.h"
#include "Game.h"
#include "GameCommonData.h"
#include "Player.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/MonsterDefinition.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/Tags.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/DataStructures/XPooledMap.h"
#include "HirelingSwig.h"
#include "VisualInventory.h"
#include "GlobalSNO.h"
#include "../Client/CWorld.h"
#include "../Sigma/RefString.h"
#include "ItemMailCommon.h"
#include "Protobuf/Items.pb.h"
#include "../Server/Actors/Actors.h"
#include "Definitions/PowerDefinition.h"
#include "TagMaps/PowerTags.h"
#include "Powers/PowersCommon.h"
#include "Powers/PowerAPISwig.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Server/Powers/Powers.h"
#include "OnlineService/OnlineService.h"
#include "../Server/SGame.h"
#include "GameConstants.h"
#include "Definitions/GlobalDefinition.h"
#include "Monsters.h"
#include "../Server/PlayerList.h"
#include "Networking/GameMessage.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Server/Achievements/AchievementModifierEvaluationLogic.h"
#include "Achievements/AchievementDefines.h"
#include "../Server/SGameStats.h"
#include "../Server/LuaScript/ScriptEvent.h"
BOOL ItemIsLargeSlotItem(GBID gbidItem);  // Items.cpp:1843
BOOL PlayerClassCanDualWield(const PlayerClass ePlayerClass);  // Player.cpp:2332
BOOL ItemIsAllowedInSlot(const InventorySlotType eSlot, const ItemTypeDefinition * pItemTypeDef);  // Items.cpp:8832
ItemBindingLevel ItemGetBindingLevel(const ActorCommonData & tACD);  // Items.cpp:5534
void STradeCheckForModified(const ActorCommonData & tACDItem, BOOL bRemoveItem);  // STrade.cpp:844
enum { GB_ITEMTYPES = 1 };
#include "../Server/Powers/PowerAPI.h"
GBID ItemGetItemTypeGBID(GBID gbidItem);  // Items.cpp:1717
VisualInventorySlot VisualInventorySlotFromInventorySlot(const InventorySlotType eInventorySlot);  // VisualInventory.cpp:268
CurrencyType CurrencyFromGBID(GBID gbid);  // Items.cpp:10444
HirelingClass HirelingGetClass(const ActorCommonData & tACDHireling);  // Hireling.cpp:222
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);  // ActorInventory.cpp:9743
WeaponClass GetWeaponClass(const GBID gbidItemLeft, const GBID gbidItemRight);  // ActorInventory.cpp:11233
Attrib ActorInventoryGetVariableSizedSlotAttrib(const InventorySlotType eSlot);  // ActorInventory.cpp:12416
SNO ACDGetInventory(const ActorCommonData * ptACD, BOOL bSecondary);  // ActorInventory.cpp:12508
BOOL ACDInventoryValidateSlotForACD(const ActorCommonData & tACD, const InventorySlotType eSlot);  // ActorInventory.cpp:2156
void ACDInventoryGetSlotTypeInfo(InventoryType eType, int32 & nStart, int32 & nEnd, int32 & nSlots);  // ActorInventory.cpp:9771
void SACDInventoryDeleteItem(ActorCommonData & tACDItem, uint64 uCount, BOOL bMarkAsDirty, BOOL bFlagForDeletion);  // ActorInventory.cpp:6800
DataID SACDInventoryAddToDirtyList(const ActorCommonData & tACDItem, ActorInventoryDatabaseAction eAction, InventorySlotType eCameFrom, InventorySlotType eDestination);  // ActorInventory.cpp:11891
void ACDInventoryGetSlotPageSize(const InventorySlotType eSlot, IVector2D & tSize);  // ActorInventory.cpp:2081
BOOL ACDInventorySlotContainsOneHandedWeapon(const ActorCommonData & tACD, const InventorySlotType eSlot, BOOL bUseVisualInventory);  // ActorInventory.cpp:11438
BOOL ACDCanDualWield(const ActorCommonData & tACD);  // ActorInventory.cpp:11419
ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD);  // ActorInventory.cpp:5373
BOOL ACDInventorySlotIsMultiItemSlot(InventorySlotType eSlot);  // ActorInventory.cpp:2141
IVector2D ACDInventoryGetItemSize(const ActorCommonData & tACDItem, InventorySlotType eSlot);  // ActorInventory.cpp:7355
BOOL ActorInventoryIsVariableSizedSlot(const InventorySlotType eSlot);  // ActorInventory.cpp:12401
BOOL ActorInventoryIsVariableSizedSlotValid(const InventoryLocation & tInvLoc);  // ActorInventory.cpp:685
inline IRect2D ACDInventoryGetItemExtents(const ActorCommonData & tACDItem, const IVector2D & vecBase);  // ActorInventory.cpp:7372 (never emitted out of line)
void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD);  // ActorInventory.cpp:9033
inline void SACDInventorySetItemUnequipped(ActorCommonData & tACDItem);  // ActorInventory.cpp:12740 (never emitted out of line)
void SScenePutACDInScene(SSceneID idSScene, ACDID idACD, BOOL bAdd);  // SScene.cpp:513
void ActorCollisionDispose(ActorCommonData & tACD);  // ActorCollision.cpp:205
void ACDCollDisposeSphereNode(ActorCommonData & tACD);  // ActorCommonCollision.cpp:172
BOOL ItemIsBroken(const ActorCommonData & tACDItem);  // Items.cpp:8672
BOOL ItemIsEquippedButDisabled(const ActorCommonData & tACDItem);  // Items.cpp:8690
void ClearUpscaledAttributes(ActorCommonData & tACDItem);  // Items.cpp:6929
AttribGroupID SItemGetAttributeGroup(const ActorCommonData & tACDItem);  // Items.cpp:5470
void VisualInventorySetItemInSlot(const ACDID idACD, const VisualInventorySlot eSlot, const GBID gbidItem, const DyeType eDyeType, const ItemEffectType eItemEffectType, const int32 nEffectLevel, const BOOL bItemIsBrokenOrDisabled);  // VisualInventory.cpp:464
void PlayerResetDualWield(const ACDID idACDPlayer);  // Player.cpp:1943
void STradeRemoveItemFromTrade(const ActorCommonData & tACDItem);  // STrade.cpp:888
void ScriptEventOnPlayerDroppedItem(ACDID idACDPlayer, ACDID idACDItem);  // ScriptEvent.cpp:3248
enum KnownEntryType {
    KET_ACD = 8,
};
void KnownListItemDeleted(KnownEntryType eType, DataID idObject);  // KnownList.cpp:1166
void FlippyDropCreateOnActor(const ACDID idACDDropper, const ACDID idACDItem, const WorldPlace & tPlace);  // Flippies.cpp:1180
ACDID ACDInventoryGetUltimateContainer(const ActorCommonData & tACDItem);  // ActorInventory.cpp:4224
void PlayerClearPotionBtnForMissingItem(Player * ptPlayer, const ACDID idACDItem);  // Player.cpp:1735
void PowerAPI_BuffApplyOnEquipmentChangeEffects(ACDID idACDActor, InventorySlotType eSlot, int32 eReason);  // PowerAPI.cpp:11840
BOOL FlippyFindLandingLocation(const ActorCommonData & tACDLaunching, const SNO snoActorFlippy, WorldPlace & tPlaceLanding, const Player * ptAssignedPlayer, const Vector3D * pvBias, ACDID idACDKiller);  // Flippies.cpp:831
BOOL TradeIsTrading(const ACDID idACDPlayer);  // Trade.cpp:270
GameError ItemCanDrop(const ActorCommonData & tACDItem);  // Items.cpp:9360
void PlayerRemoveItemFromTradeOffer(Player * ptPlayer, const ActorCommonData & tACDItem);  // Player.cpp:3602
SNO ItemGetUsePower(const GBID gbidItem);  // Items.cpp:2434
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);  // Items.cpp:1883
int32 ItemGetSocketedUniqueEquipped(const ActorCommonData & tACDItem, GBID * gbidUniqueEquippedSocketables, int32 nMaxUniqueEquippedSocketables);  // Items.cpp:4865
void ScriptEventOnActorRerolledInventory(ACDID idACDActor);  // ScriptEvent.cpp:3101
void ActorInventorySetSeed(ActorInventory * ptInventory, uint32 dwSeed);  // ActorInventory.cpp:12876
BOOL ACDInventoryInitialize(ActorCommonData & tACD, BOOL fForceInventoryCreate);  // ActorInventory.cpp:1455
void ACDVisualInventoryInitialize(ActorCommonData & tACD);  // VisualInventory.cpp:224
void SACDCreateRandomItems(ActorCommonData & tACDContainer, BOOL fAutoIdentify, InventorySlotType eSlot, SNO snoTreasureClass, BOOL bPlayEffects);  // ActorInventory.cpp:9519
BOOL ItemIsGlyph(const ActorCommonData & tACDItem);  // Items.cpp:4252
VisualInventorySlot VisualInventorySlotFromInventorySlot(const InventorySlotType eInventorySlot);  // VisualInventory.cpp:268
void PowerStartUseItem(ACDID idACDPlayer, ACDID idACDItemToUse, const Target * ptTarget);  // Power_ItemUse.cpp:2086
BOOL ACDInventoryCanUseItem(const ActorCommonData & tACDUser, const ActorCommonData & tACDItem, GameError * peOutError);  // ActorInventory.cpp:11276
CostType ACDInventoryGetCostType(const ACDID idACDItem, const ACDID idACDContainer, const ACDID idACDPlayer);  // ActorInventory.cpp:11791
struct PickupResult;
PickupResponse ACDInventoryCanPickup(const ActorCommonData & tACDPickee, const ActorCommonData & tACDItem, BOOL bTryStashFirst, PickupResult * ptResult, BOOL bTryStashOnly, BOOL bAllowAutoEquip);  // ActorInventory.cpp:10712
void ItemGeneratorPopulateFromACDID(const ActorCommonData & tACDItem, D3::Items::Generator & tGenerator, BOOL bSetRestoredBit);  // Items.cpp:7599
ACDID SItemGenerate(const OnlineService::ItemId & tItemId, const D3::Items::Generator & tGenerator, const WorldPlace & tPlace, const float flScale, uint32 tName);  // SItems.cpp:337
void ItemSetBindingLevel(ActorCommonData & tACD, const ItemBindingLevel eBindingType);  // Items.cpp:5525
void ItemSetBoundToACD(ActorCommonData & tACD, const ACDID idACDOwner);  // Items.cpp:5565
BOOL SACDInventorySell(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem);  // ActorInventory.cpp:10361
PowerID ActorPowersGetPrimary(ActorID idActor);  // ActorPowers.cpp:216
void ActorPowersForceStopPrimary(ActorID idActor, BOOL bIgnoreContactFrame);  // ActorPowers.cpp:375
BOOL SPlayerHasTradeRestrictions(const Player & tPlayer);  // SPlayer.cpp:10716
BOOL ItemIsAutoPickup(const ActorCommonData & tACDItem);  // Items.cpp:4183
BOOL ItemIsLegendary(GBID gbidItem);  // Items.cpp:5080
SNO ActorInventoryGetIdentifyPower(const ActorCommonData & tACDItem);  // ActorInventory.cpp:12894
BOOL ACDInventoryDelete(ActorCommonData & tACDItem, BOOL bMarkItemAsDirty);  // ActorInventory.cpp:11163
BOOL ItemUsesQuiver(const ActorCommonData & tACDItem);  // Items.cpp:3467
SNO ItemUseGetScrollPower(const ActorCommonData & tACDItem);  // Items.cpp:2543
BOOL ItemOwnerMeetsRequirements(const ActorCommonData & tACDItem, const ActorCommonData & tACDOwner, GameError * pOutError);  // Items.cpp:2109
AttribGroupID ItemGenerateUpscaledAttribForPlayer(ActorCommonData & tACDOwner, ActorCommonData & tACDItem);  // Items.cpp:6706
BOOL ACDInventorySlotIsEquippable(InventorySlotType eSlot);  // ActorInventory.cpp:10928
void MoveItemToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, BOOL bMarkItemAsDirty, BOOL bOnHeroLoad);  // ActorInventory.cpp:3513
static void sACDAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tNewLocation, BOOL bMarkItemAsDirty, InventorySlotType eCameFrom, BOOL bOnHeroLoad); static void sOnPlayerPickedUpItem(ACDID idACDPlayer, ActorCommonData & tACDItem);
inline void SACDInventoryEvaluateAcquireItemAchievementEvent(const Player & tPlayer, const ActorCommonData & tACDItem);  // ActorInventory.cpp:12551
BOOL ItemsPreEquip(ActorCommonData & tACDItem, ActorCommonData & tACDContainer, const InventorySlotType);  /* Items.cpp:5011 */  void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eItemEffectType, int32 & nEffectLevel, const BOOL bWantOverrides, const BOOL bForTooltip);  // VisualInventory.cpp:877
void VisualInventoryResetSlot(const ACDID idACD, const InventorySlotType eInventorySlot);  /* VisualInventory.cpp:589 */  void ACDUpdateAppearance(ACDID idACD);  // ActorAppearance.cpp:81
void ActorInventoryMergeItemAttributes(ActorCommonData & tACDOwner, ActorCommonData & tACDItem);  /* ActorInventory.cpp:12177 */  void SACDInventorySetItemEquipped(ActorCommonData & tACDItem, BOOL bOnHeroLoad);  void SACDInventoryEvaluateItemDyeAchievements(Achievement::EvaluationData * ptEvaluationData);  // ActorInventory.cpp:12620
SNO ItemGetLoreSNO(const ActorCommonData & tACDItem);  /* Items.cpp:3549 */  BOOL ItemUnlocksTransmogOnPickup(const ActorCommonData & tACDItem);  // Items.cpp:4338
void SPlayerLearnLore(const ACDID idACDPlayer, const SNO snoLore);  /* SPlayer.cpp:7260 */  void SItemRegisterTradingTimer(ActorCommonData & tACDItem, uint32 uUpdateTicks);  // SItems.cpp:1206
BOOL SItemCrafting_TryUnlockTransmog(GBID gbidTransmog, Player & tPlayer, BOOL bMustBeInInventory, BOOL bSendFullUpdate);  // SItemCrafting.cpp:296
void SPlayerRecordCurrencyPickedUp(const ACDID idACDPlayer, const uint64 uCurrencyPickedUp, const CurrencyType eCurrencyType);  /* SPlayer.cpp:4714 */  void SPlayerIncreaseCurrencyCounter(Player & tPlayer, CurrencyType eCurrencyType, uint32 nAmountPickedUp);  /* SPlayer.cpp:9912 */  void ACDPlayEffectForPlayer(const ACDID idACDPlayer, const ACDID idACD, const int32 eEffectType, const int32 nEffectParameter);  /* line 137; EffectType */  GBID CurrencyGetGBID(const CurrencyType eCurrencyType);  /* Items.cpp:10429 */  FloatingNumberType FloatingNumberTypeFromCurrency(const CurrencyType eCurrency);  /* Player.cpp:5369 */  ItemSellError ItemCanBeSold(const ActorCommonData & tACDItem, const ActorCommonData * ptACDVendor);  /* Items.cpp:3963 */  GBID ItemGetCosmeticFromItem(const ActorCommonData & tACDItem);  /* Items.cpp:11127 */  int32 ItemGetStatsDataKey(const ActorCommonData & tACDItem);  /* Items.cpp:2250 */  void SGameStatsRecordGoldObtainedThroughSelling(ActorCommonData & tACDPlayer, uint64 uGoldObtained);  /* SGameStats.cpp:847 */  void ACDInventoryEquippedItemsChanged(ActorCommonData & tACD);  /* ActorInventory.cpp:9042 */  GameError ACDInventoryCanMove(const ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, ACDID * pidACDItemInSlot);  /* ActorInventory.cpp:10573 */  BOOL ACDInventoryIsSlotMerchant(InventorySlotType eSlot);  /* ActorInventory.cpp:10888 */  static BOOL sDoesItemFitAtThisLocation(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc);  BOOL ItemIsQuest(const ActorCommonData & tACDItem);  /* Items.cpp:4271 */  BOOL ItemUsableByHireling(const GBID gbidItem, const HirelingClass eHirelingClass);  /* Items.cpp:2074 */  BOOL ItemIsUniqueEquipped(const ActorCommonData & tACDItem);  /* Items.cpp:5219 */  SocketReason ItemCanSocketInto(const ActorCommonData & tACDItemToSocket, ActorCommonData & tACDItemSocketedInto);  /* Items.cpp:4416 */  BOOL ActorInventoryHasUniqueEquippedSocketablesEquipped(const ActorCommonData & tACDOwner, const GBID * gbidUniqueEquippedSocketables, int32 nUniqueEquippedSocketables, ACDID idACDSkipped, InventorySlotType eSkippedSlot);  /* ActorInventory.cpp:13205 */  BOOL ItemDeletedAfterUse(const ActorCommonData & tACDItem);  // Items.cpp:3734
BOOL ItemIsTwoHanded(const ActorCommonData & tACDItem);  /* Items.cpp:3403 */  BOOL ItemIsUsable(const ActorCommonData & tACDItem);  // Items.cpp:3499
struct ItemTypeCountCallbackData {  // size 0x18
    uint64 uCount;  // 0x00
    GBHandle gbHandleToMatch;  // 0x08
    ACDID idACDItem;  // 0x10
}; static_assert(sizeof(ItemTypeCountCallbackData) == 0x18, "ItemTypeCountCallbackData");
struct SNOActorItemCountCallbackData {  // size 0x10
    uint64 uCount;  // 0x00
    SNO snoActorToMatch;  // 0x08
    ACDID idACDItem;  // 0x0c
}; static_assert(sizeof(SNOActorItemCountCallbackData) == 0x10, "SNOActorItemCountCallbackData");
struct CountItems {  // size 0x18
    uint64 uItemCount;  // 0x00
    GBHandle hGBItem;  // 0x08
    BOOL bWhiteEquippableOnly;  // 0x10
    BOOL bCheckSockets;  // 0x14
}; static_assert(sizeof(CountItems) == 0x18, "CountItems");
struct FindItem {  // size 0x20
    GBHandle hGBItem;  // 0x00
    ACDID idACDItem;  // 0x08
    uint64 uCount;  // 0x10
    BOOL bWhiteEquippableOnly;  // 0x18
}; static_assert(sizeof(FindItem) == 0x20, "FindItem");
struct SavedInventorySlotDef {  // size 0x8
    SavedInventorySlotType eSavedSlotType;  // 0x00
    InventorySlotType eNormalSlotType;  // 0x04
}; static_assert(sizeof(SavedInventorySlotDef) == 0x8, "SavedInventorySlotDef");
static BOOL sStackItems(ActorCommonData & tACDSrcItem, ActorCommonData & tACDDestItem, uint64 uAmount, BOOL bMarkAsDirty);
typedef XGrowableList<ACDID, ACDID> InventoryItemList;
struct InventoryCallbackItemCountType {  // size 0x70
    InventoryCallbackItemCountType(const BOOL fExactMatch, const ActorCommonData * ptACDSearchItem) : m_fExactMatch(fExactMatch), m_uTotalItemCount(0), m_ptACDSearchItem(ptACDSearchItem) {}
    BOOL m_fExactMatch;  // 0x00
    uint64 m_uTotalItemCount;  // 0x08
    const ActorCommonData * m_ptACDSearchItem;  // 0x10
    InventoryItemList m_tItemList;  // 0x18
}; static_assert(sizeof(InventoryCallbackItemCountType) == 0x70, "InventoryCallbackItemCountType");
struct OverlapTestData {  // size 0x20
    ACDID idACDItemToTest;  // 0x00
    InventoryLocation tSrcInvLoc;  // 0x04
    InventoryOverlapList * ptList;  // 0x18
}; static_assert(sizeof(OverlapTestData) == 0x20, "OverlapTestData");
struct SocketUpdateInfo {  // size 0x28
    InventoryId tInventoryId;  // 0x00
    InventorySlotType eCameFrom;  // 0x18
    InventorySlotType eTo;  // 0x1c
    BOOL bRepairSocketsFilledCount;  // 0x20
}; static_assert(sizeof(SocketUpdateInfo) == 0x28, "SocketUpdateInfo");
static BOOL sTestSlotOverlap(const InventoryLocation & tInvLoc, void * pCallBackData);
struct InventorySquare {  // size 0x4
    ACDID idACD;  // 0x00
}; static_assert(sizeof(InventorySquare) == 0x4, "InventorySquare");
struct InventorySlot {  // size 0x18
    blz::vector<InventorySquare, blz::allocator<InventorySquare> > arSquares;  // 0x00
}; static_assert(sizeof(InventorySlot) == 0x18, "InventorySlot");
typedef XPooledMap<unsigned int, unsigned int, InventorySlot *, InventorySlot *, 32> InventoryMap;
typedef XMapMemoryPool<unsigned int, InventorySlot *> InventoryMapPool;
struct ActorInventory {  // size 0x138
    InventoryType eType;  // 0x00
    InventoryMap mapInventorySlots;  // 0x08
    uint32 dwSeed;  // 0x130
}; static_assert(sizeof(ActorInventory) == 0x138, "ActorInventory");

const InventorySlotType cgtDefaultPlayerInventorySlots[15] = { SLOT_PLAYER_BACKPACK, SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS, SLOT_PLAYER_WAIST, SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_NECK, SLOT_NONE };  // line 104
const InventorySlotType cgtPlayerInventorySlotsPreferEquipped[15] = { SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS, SLOT_PLAYER_WAIST, SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_NECK, SLOT_PLAYER_BACKPACK, SLOT_NONE };  // line 143
const InventorySlotType cgtPlayerInventorySlotsEquippedOnly[14] = { SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS, SLOT_PLAYER_WAIST, SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_NECK, SLOT_NONE };  // line 163
const InventorySlotType cgtDefaultPlayerInventorySlotsIncludingStash[16] = { SLOT_PLAYER_BACKPACK, SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS, SLOT_PLAYER_WAIST, SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_NECK, SLOT_PLAYER_SHARED_STASH, SLOT_NONE };  // line 182
const InventorySlotType cgtPlayerInventorySlotsPreferEquippedIncludingStash[16] = { SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS, SLOT_PLAYER_WAIST, SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_NECK, SLOT_PLAYER_BACKPACK, SLOT_PLAYER_SHARED_STASH, SLOT_NONE };  // line 204






struct ActorInventoryGlobals {  // size 0xa8
    ChainedFixedBlockAllocator poolActorInventory; XChainedFixedBlockAllocator<InventorySlot> poolInventorySlot; InventoryMapPool poolInventoryMap;  // 0x00, 0x30, 0x60
    DirtyInventoryRecordMap * ptItemMoveLog;  // 0x90
    uint32 uDirtyUpdateCounter;  // 0x98
    MailRecordMap * ptMailMap;  // 0xa0
}; static_assert(sizeof(ActorInventoryGlobals) == 0xa8, "ActorInventoryGlobals");
#include "Networking/Network.h"
#include "Actors/ActorEffectCommon.h"
#include "Shell/CmdLine.h"
#include "../Server/SPlayer.h"
static const GameError cgtPowerScrollBuffExclusiveTypeErrors[5] = {
    GAMEERROR_NONE,
    GAMEERROR_ITEM_NOT_READY,
    GAMEERROR_ITEM_NOT_READY,
    GAMEERROR_ITEM_NOT_READY,
    GAMEERROR_ITEM_NOT_READY,
};
#include "../Server/Map/Minimap.h"


enum { INITIAL_INVENTORY_POOL_COUNT = 64, MAX_INVENTORY_MAP_PAIRS = 1024, MAX_INVENTORY_SLOTS = 256, };
void ActorTriggerAttributeUpdate(const ActorID idActor);  // X/Server/Actors/Actors.cpp:3770

#include "../Server/SGameManager.h"
#include "ActorAttributes.h"
#include "HotbarButton.h"
#include "../Sigma/MemoryPools.h"
BOOL SItemCrafting_TryUnlockSecondaryTransmogs(GBID gbidTransmog, Player & tPlayer);  // X/Server/Crafting/SItemCrafting.cpp:386
ACDID LootQuickCreateItem(const GBHandle & hGB, const BOOL fAutoIdentify);  // X/Server/Loot/Loot.cpp:4301
SavedDyeType DyeTypeTranslateToSavedDyeType(const DyeType eDye);  // Definitions/GlobalDefinition.cpp:740
BOOL ReplayIsReplayGame();  // X/Common/Replay.cpp
#include "Marker/Marker.h"










































#include "../Client/Gfx/GfxVB.h"
#include "../Sigma/DataStructures/XVar.h"




static XVarScope s_scopeAuctionHouse("AuctionHouse", NULL);
XVarUint64 s_varAuctionLockoutTime(s_scopeAuctionHouse, "ItemLockCooldownMinutes", 0);




















































































static const SavedInventorySlotDef cgarSavedInventorySlotTypeDefs[36] = {
    { SAVED_SLOT_NONE, SLOT_NONE },
    { SAVED_SLOT_PLAYER_BACKPACK, SLOT_PLAYER_BACKPACK },
    { SAVED_SLOT_PLAYER_HEAD, SLOT_PLAYER_HEAD },
    { SAVED_SLOT_PLAYER_TORSO, SLOT_PLAYER_TORSO },
    { SAVED_SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_RIGHT_HAND },
    { SAVED_SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_LEFT_HAND },
    { SAVED_SLOT_PLAYER_HANDS, SLOT_PLAYER_HANDS },
    { SAVED_SLOT_PLAYER_WAIST, SLOT_PLAYER_WAIST },
    { SAVED_SLOT_PLAYER_FEET, SLOT_PLAYER_FEET },
    { SAVED_SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_SHOULDERS },
    { SAVED_SLOT_PLAYER_LEGS, SLOT_PLAYER_LEGS },
    { SAVED_SLOT_PLAYER_BRACERS, SLOT_PLAYER_BRACERS },
    { SAVED_SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_RIGHT_FINGER },
    { SAVED_SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_LEFT_FINGER },
    { SAVED_SLOT_PLAYER_NECK, SLOT_PLAYER_NECK },
    { SAVED_SLOT_PLAYER_BUYBACK, SLOT_PLAYER_BUYBACK },
    { SAVED_SLOT_PLAYER_SHARED_STASH, SLOT_PLAYER_SHARED_STASH },
    { SAVED_SLOT_PLAYER_BASE_HEALTHPOTION, SLOT_PLAYER_BASE_HEALTHPOTION },
    { SAVED_SLOT_UNK_250, SLOT_UNK_11 },
    { SAVED_SLOT_MERCHANT_DISPLAY, SLOT_MERCHANT_DISPLAY },
    { SAVED_SLOT_MERCHANT_SHOPLIST, SLOT_MERCHANT_SHOPLIST },
    { SAVED_SLOT_ITEM_SOCKETS, SLOT_ITEM_SOCKETS },
    { SAVED_SLOT_HIRELING_RIGHT_HAND, SLOT_HIRELING_RIGHT_HAND },
    { SAVED_SLOT_HIRELING_LEFT_HAND, SLOT_HIRELING_LEFT_HAND },
    { SAVED_SLOT_HIRELING_SPECIAL, SLOT_HIRELING_SPECIAL },
    { SAVED_SLOT_HIRELING_NECK, SLOT_HIRELING_NECK },
    { SAVED_SLOT_HIRELING_RIGHT_FINGER, SLOT_HIRELING_RIGHT_FINGER },
    { SAVED_SLOT_HIRELING_LEFT_FINGER, SLOT_HIRELING_LEFT_FINGER },
    { SAVED_SLOT_UNK_570, SLOT_UNK_1b },
    { SAVED_SLOT_UNK_580, SLOT_UNK_1c },
    { SAVED_SLOT_UNK_590, SLOT_UNK_1d },
    { SAVED_SLOT_UNK_5a0, SLOT_UNK_1e },
    { SAVED_SLOT_UNK_5b0, SLOT_UNK_1f },
    { SAVED_SLOT_UNK_5c0, SLOT_UNK_20 },
    { SAVED_SLOT_UNK_5d0, SLOT_UNK_21 },
    { SAVED_SLOT_UNK_5e0, SLOT_UNK_22 },

};








InventorySlotType ActorInventoryTranslateFromSavedSlot(int32 eSavedSlotType)
{
    for (int32 i = 0; i < (int32)(sizeof(cgarSavedInventorySlotTypeDefs) / sizeof(cgarSavedInventorySlotTypeDefs[0])); i++)
    {
        const SavedInventorySlotDef & tEntry = cgarSavedInventorySlotTypeDefs[i];
        if (tEntry.eSavedSlotType == eSavedSlotType)
            return tEntry.eNormalSlotType;
    }
    return SLOT_NONE;
}




SavedInventorySlotType ActorInventoryTranslateToSavedSlot(InventorySlotType eSlotType)
{
    for (int32 i = 0; i < (int32)(sizeof(cgarSavedInventorySlotTypeDefs) / sizeof(cgarSavedInventorySlotTypeDefs[0])); i++)
    {
        const SavedInventorySlotDef & tEntry = cgarSavedInventorySlotTypeDefs[i];
        if (tEntry.eNormalSlotType == eSlotType)
        {
            return tEntry.eSavedSlotType;
        }
    }
    return SAVED_SLOT_NONE;
}





ACDID ActorInventoryGetUltimateOwner(const ActorCommonData * ptACDItem)
{
    while (ptACDItem && ptACDItem->GetLocationType() == AL_INVENTORY)
    {
        ACDID idACDOwner = ptACDItem->GetInventoryContainerOwner();
        if (idACDOwner == ptACDItem->id)
            return idACDOwner;

        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
        if (ACDIsPlayer(idACDOwner))
            return idACDOwner;
        if (ptACDOwner == NULL)
            return ACDID_INVALID;
        if (!ptACDOwner->IsItem())
            return idACDOwner;
        ptACDItem = ptACDOwner;
    }
    return ACDID_INVALID;
}








static BOOL sIsInStash(const ActorCommonData * ptACDItem)
{
    while (ptACDItem->GetLocationType() == AL_INVENTORY)
    {
        ACDID idACDOwner = ptACDItem->tLocation.tInvLocation.idACDContainer;
        if (idACDOwner == ptACDItem->id)
            return FALSE;

        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
        if (ptACDOwner == NULL)
            return FALSE;
        if (ptACDOwner->GetLocationSlot() == SLOT_PLAYER_SHARED_STASH)
            return TRUE;
        ptACDItem = ptACDOwner;
    }
    return FALSE;
}













ActorInventoryStorageBucket GetInventoryBucket(const ActorCommonData & tACDItem, InventorySlotType eSlot)
{
    switch (eSlot)
    {
    case SLOT_NONE:
        return AISB_NONE;

    case SLOT_PLAYER_SHARED_STASH:
        return AISB_STASH;



    case SLOT_ITEM_SOCKETS:
        return sIsInStash(&tACDItem) ? AISB_STASH : AISB_HERO;



    default:
        return AISB_HERO;

    }
}



InventorySlotType ACDInventoryGetOppositeTwoHandedSlot(InventorySlotType eSlot)
{
    switch (eSlot)
    {
    case SLOT_PLAYER_LEFT_HAND:
        return SLOT_PLAYER_RIGHT_HAND;
    case SLOT_PLAYER_RIGHT_HAND:
        return SLOT_PLAYER_LEFT_HAND;
    case SLOT_HIRELING_LEFT_HAND:
        return SLOT_HIRELING_RIGHT_HAND;
    case SLOT_HIRELING_RIGHT_HAND:
        return SLOT_HIRELING_LEFT_HAND;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}



BOOL ACDInventoryIsSlotValidForHireling(const ActorCommonData & tACDHireling, const int32 eClass, const InventorySlotType eSlot)
{
    switch (eClass)
    {
    case 1:
    case 2:
    case 3:
        if (eSlot < SLOT_HIRELING_START || eSlot >= SLOT_HIRELING_END)
            return FALSE;
        if (eSlot == SLOT_HIRELING_SPECIAL && !ACDInventoryHirelingSlotUnlocked(tACDHireling))
            return FALSE;
        break;
    }
    return TRUE;
}















BOOL ACDInventoryHirelingSlotUnlocked(const ActorCommonData & tACDHireling)
{
    return tACDHireling.AttributesGetInt(ATTRIBUTE_LEVEL) >= HIRELING_SPECIAL_SLOT_UNLOCK_LEVEL;
}




static void sTestSquareFilledCount(ACDID idACDSquare, InventorySlotType, void * pCallBackData)
{
    const ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem)
    {
        int32 * pCount = (int32 *)pCallBackData;
        *pCount += ItemIsLargeSlotItem(ptACDItem->hGB.gbid) ? 2 : 1;
    }
}






static BOOL sACDInventorySlotIsEmpty(const InventoryLocation & tInvLoc)
{

    return ACDInventoryGetItemInSlot(tInvLoc) == ACDID_INVALID;
}




ACDID ACDInventoryGetItemInSlot(const InventoryLocation & tInvLoc)
{

    InventoryOverlapList tList;
    OverlapTestData tData;
    tData.idACDItemToTest = ACDID_INVALID;
    tData.tSrcInvLoc = tInvLoc;
    tData.ptList = &tList;

    IRect2D rectSearch;

    rectSearch.Set(tInvLoc.pt.x, tInvLoc.pt.y - 1, tInvLoc.pt.x + 1, tInvLoc.pt.y + 1);

    ActorCommonData * ptACD = ACDTryToGet(tInvLoc.idACDContainer);
    if (!ptACD)
        return ACDID_INVALID;



    ACDInventoryIterateSlot(*ptACD, tInvLoc.eSlot, &rectSearch, sTestSlotOverlap, &tData, FALSE, FALSE);

    if (tData.ptList->Count() == 0)
        return ACDID_INVALID;




    return tData.ptList->GetHead();
}






BOOL ActorInventoryIsVariableSizedSlotValidForItem(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc)
{
    int32 nBackpackSlots;
    IVector2D vecItemSize;
    FastAttribKey eKeySlots = ActorInventoryGetVariableSizedSlotAttrib(tInvLoc.eSlot);
    IVector2D tSize;

    ACDInventorySlotGetSize(tInvLoc.eSlot, tSize);

    nBackpackSlots = ACDGet(tInvLoc.idACDContainer).AttributesGetInt(eKeySlots);
    int32 nSlotIndexMax = Math::Min(tSize.x * tSize.y, nBackpackSlots);

    vecItemSize = ACDInventoryGetItemSize(tACDItem, tInvLoc.eSlot);
    if (tInvLoc.pt.x + (tInvLoc.pt.y + (vecItemSize.y - 1)) * tSize.x
        >= nSlotIndexMax)
        return FALSE;




    if (vecItemSize.y > 1)
    {
        IVector2D tPageSize;
        ACDInventoryGetSlotPageSize(tInvLoc.eSlot, tPageSize);
        if ((tInvLoc.pt.y + 1) % tPageSize.y == 0)
            return FALSE;
    }




    return TRUE;
}



BOOL ActorInventoryIsVariableSizedSlotValid(const InventoryLocation & tInvLoc)
{
    int32 nBackpackSlots;
    FastAttribKey eKeySlots = ActorInventoryGetVariableSizedSlotAttrib(tInvLoc.eSlot);
    IVector2D tSize;

    ACDInventorySlotGetSize(tInvLoc.eSlot, tSize);

    nBackpackSlots = ACDGet(tInvLoc.idACDContainer).AttributesGetInt(eKeySlots);
    int32 nSlotIndexMax = Math::Min(tSize.x * tSize.y, nBackpackSlots);
    return tInvLoc.pt.x + tInvLoc.pt.y * tSize.x < nSlotIndexMax;
}
// NON_MATCHING: ours inlines ~XList<ACDID,ACDID,8> (see ACDInventoryGetItemInSlot)










static BOOL sACDInventoryFindBestLocationForLargeItem(const ActorCommonData & tACDItem, const IVector2D & tSizeSlot, InventoryLocation & tInvLoc, const int32 * pnSlotYMinMax)
{




    BOOL fVariableSizedSlot = ActorInventoryIsVariableSizedSlot(tInvLoc.eSlot);

    int32 nSlotYMin = 0;
    int32 nSlotYMax = tSizeSlot.y;

    if (pnSlotYMinMax)
    {
        nSlotYMin = pnSlotYMinMax[0];
        nSlotYMax = pnSlotYMinMax[1];
    }


    for (int32 y = nSlotYMin; y != nSlotYMax; y++)
    {
        for (int32 x = 0; x != tSizeSlot.x; x++)
        {
            tInvLoc.pt.Set(x, y);


            if (fVariableSizedSlot && !ActorInventoryIsVariableSizedSlotValidForItem(tACDItem, tInvLoc))
                continue;



            InventoryOverlapList tList;
            ACDInventoryTestOverlap(tACDItem, tInvLoc, tList);
            if (tList.Count() == 0)
            {


                return TRUE;
            }
        }
    }
    return FALSE;
}



static BOOL sACDInventoryFindBestLocationForSmallItem(const ActorCommonData &, const IVector2D & tSizeSlot, InventoryLocation & tInvLoc, const int32 * pnSlotYMinMax)
{




    BOOL fVariableSizedSlot = ActorInventoryIsVariableSizedSlot(tInvLoc.eSlot);



    int32 nSlotYMin = 0;
    int32 nSlotYMax = tSizeSlot.y;
    if (pnSlotYMinMax)
    {
        nSlotYMin = pnSlotYMinMax[0];
        nSlotYMax = pnSlotYMinMax[1];
    }

    for (int32 x = 0; x != tSizeSlot.x; x++)
    {
        for (int32 y = nSlotYMin; y != nSlotYMax; y++)
        {
            tInvLoc.pt.Set(x, y);


            if (fVariableSizedSlot && !ActorInventoryIsVariableSizedSlotValid(tInvLoc))
                continue;



            if (sACDInventorySlotIsEmpty(tInvLoc))
                return TRUE;
        }
    }




    return FALSE;
}



static BOOL sACDInventoryFindBestLocation(const ActorCommonData & tACDItem, InventoryLocation & tInvLoc, const int32 * pnSlotYMinMax)
{


    IVector2D tSizeSlot;

    ACDInventorySlotGetSize(tInvLoc.eSlot, tSizeSlot);



    if (tInvLoc.eSlot == SLOT_ITEM_SOCKETS)
    {
        tSizeSlot.y = ACDGet(tInvLoc.idACDContainer).AttributesGetInt(ATTRIBUTE_SOCKETS);
    }


    if (tSizeSlot.x == 1 && tSizeSlot.y == 1)
    {
        return sACDInventorySlotIsEmpty(tInvLoc);
    }


    if (!pnSlotYMinMax && tInvLoc.eSlot == SLOT_PLAYER_SHARED_STASH)
    {

        IVector2D tSlotPageSize;
        ACDInventoryGetSlotPageSize(tInvLoc.eSlot, tSlotPageSize);
        int32 nSlotsPerPage = tSlotPageSize.x * tSlotPageSize.y;

        FastAttribKey eKeySlots = ActorInventoryGetVariableSizedSlotAttrib(tInvLoc.eSlot);
        int32 nBackpackSlots = ACDGet(tInvLoc.idACDContainer).AttributesGetInt(eKeySlots);
        int32 nPages = nBackpackSlots / nSlotsPerPage + ((nBackpackSlots % nSlotsPerPage > 0) ? 1 : 0);

        int32 nSlotYMinMaxTemp[2];
        for (int32 i = 0; i < nPages; i++)
        {

            nSlotYMinMaxTemp[0] = tSlotPageSize.y * i;
            nSlotYMinMaxTemp[1] = tSlotPageSize.y * (i + 1);

            if (ItemIsLargeSlotItem(tACDItem.hGB.gbid))
            {
                if (sACDInventoryFindBestLocationForLargeItem(tACDItem, tSizeSlot, tInvLoc, nSlotYMinMaxTemp))
                    return TRUE;
            }
            else
            {


                if (sACDInventoryFindBestLocationForSmallItem(tACDItem, tSizeSlot, tInvLoc, nSlotYMinMaxTemp))
                    return TRUE;
            }
        }
        return FALSE;
    }





    if (ItemIsLargeSlotItem(tACDItem.hGB.gbid))
    {
        return sACDInventoryFindBestLocationForLargeItem(tACDItem, tSizeSlot, tInvLoc, pnSlotYMinMax);
    }
    else
    {
        return sACDInventoryFindBestLocationForSmallItem(tACDItem, tSizeSlot, tInvLoc, pnSlotYMinMax);
    }

}



BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax)
{





    tWhereCanHold = cgptNull;
    InventoryLocation tInvLoc;
    ACDInventoryLocationSet(tInvLoc, tACD.id, eSlot, tWhereCanHold.x, tWhereCanHold.y);

    if (!ACDInventoryValidateSlotForACD(tACD, eSlot))
        return FALSE;



    if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLoc, FALSE, FALSE) != GAMEERROR_NONE)
        return FALSE;



    if (ACDInventorySlotIsMultiItemSlot(eSlot))
    {
        BOOL fResult = sACDInventoryFindBestLocation(tACDItem, tInvLoc, pnSlotYMinMax);
        if (!fResult)
            return FALSE;
        tWhereCanHold = tInvLoc.pt;
    }
    else
    {






        ACDID idACDItem = ACDInventoryGetSquareItem(tACD, eSlot, cgptNull);
        if (idACDItem != ACDID_INVALID)
            return FALSE;
        tWhereCanHold = cgptNull;
    }




    return TRUE;
}


static ACDID sGetItemInSlot(ActorCommonData & tACD, InventorySlotType eSlot)
{


    return ACDInventoryGetSquareItem(tACD, eSlot, cgptNull);
}



EInvItemClass GetInventoryItemClass(
    const GBID gbidItem)
{
    if (ItemHasLabel(gbidItem, ITL_SHIELD))
        return IIC_SHIELD;



    if (ItemHasLabel(gbidItem, ITL_QUIVER))
        return IIC_QUIVER;



    if (ItemHasLabel(gbidItem, ITL_OFFHAND))
        return IIC_OFFHAND;



    if (ItemHasLabel(gbidItem, ITL_WEAPON))
    {
        if (ItemHasLabel(gbidItem, ITL_2_HAND))
            return IIC_TWOHANDED;
        return IIC_ONEHANDED;
    }

    return IIC_NONE;
}






static void sGetPlayerPetAndOwner(ACDID idACD, InventorySlotType eSlot1, InventorySlotType eSlot2, ACDID & idACDPlayer, ACDID & idACDHireling, ACDID & idACDSlotOwner)
{







    if (ACDIsPlayer(idACD))
    {
        idACDPlayer = idACD;
        idACDHireling = PlayerGetHireling(idACD);
    }
    else
    {
        idACDHireling = idACD;
        idACDPlayer = ACDGetPetOwner(idACD);
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);


    if (ptACDPlayer &&
        ACDInventoryValidateSlotForACD(*ptACDPlayer, eSlot1) &&
        ACDInventoryValidateSlotForACD(*ptACDPlayer, eSlot2))
    {
        idACDSlotOwner = idACDPlayer;
        return;
    }

    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling &&
        ACDInventoryValidateSlotForACD(*ptACDHireling, eSlot1) &&
        ACDInventoryValidateSlotForACD(*ptACDHireling, eSlot2))
    {
        idACDSlotOwner = idACDHireling;
        return;
    }


    idACDSlotOwner = ACDID_INVALID;
}




BOOL ACDInventoryDoItemsConflict(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItemMain,
    const ActorCommonData & tACDItemOffHand)
{
    EInvItemClass eClassMain = GetInventoryItemClass(tACDItemMain.hGB.gbid);
    EInvItemClass eClassOffHand = GetInventoryItemClass(tACDItemOffHand.hGB.gbid);

    if (ItemUsesQuiver(tACDItemMain) && eClassOffHand == IIC_QUIVER)
        return FALSE;




    if (eClassMain == IIC_ONEHANDED &&
        (eClassOffHand == IIC_OFFHAND || eClassOffHand == IIC_SHIELD))
        return FALSE;



    if (eClassMain == IIC_ONEHANDED && eClassOffHand == IIC_ONEHANDED)
    {

        if (PlayerGetPlayerClass(tACDPlayer.id) == PLAYER_CLASS_DEMONHUNTER)
        {

            if (ItemHasLabel(tACDItemMain.hGB.gbid, ITL_HAND_CROSSBOW) &&
                ItemHasLabel(tACDItemOffHand.hGB.gbid, ITL_HAND_CROSSBOW))
                return FALSE;
        }


        else if (ACDCanDualWield(tACDPlayer))
        {
            return FALSE;
        }
    }

    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD))
    {
        if (eClassMain == IIC_TWOHANDED && eClassOffHand == IIC_SHIELD)
            return FALSE;
    }




    return TRUE;
}


static GameError sCheckForLeftArmConflict(const ActorCommonData & tACD, const ActorCommonData & tACDItem, InventorySlotType eLeftSlot, InventorySlotType eRightSlot, const ItemTypeDefinition * pItemTypeDef)
{






    if (!ItemIsAllowedInSlot(eLeftSlot, pItemTypeDef))
        return GAMEERROR_IARESULT_ITEM_TYPE_DOESNT_MATCH_SLOT;

    ACDID idACDPlayer;
    ACDID idACDHireling;
    ACDID idACDOwner;

    sGetPlayerPetAndOwner(tACD.id, eLeftSlot, eRightSlot, idACDPlayer, idACDHireling, idACDOwner);
    if (idACDOwner == ACDID_INVALID)
        return GAMEERROR_IARESULT_UNKNOWN;




    ActorCommonData & tACDOwner = ACDGet(idACDOwner);
    ACDID idACDLeft = sGetItemInSlot(tACDOwner, eLeftSlot);
    ACDID idACDRight = sGetItemInSlot(tACDOwner, eRightSlot);


    if (idACDLeft == ACDID_INVALID || idACDRight == ACDID_INVALID)
        return GAMEERROR_NONE;



    if (idACDPlayer == ACDID_INVALID)
        return GAMEERROR_IARESULT_UNKNOWN;



    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    ActorCommonData & tACDRight = ACDGet(idACDRight);


    if (!ItemIsTwoHanded(tACDItem))
    {
        if (!ACDInventoryDoItemsConflict(tACDPlayer, tACDItem, tACDRight))
            return GAMEERROR_NONE;

        IVector2D tPoint;
        if (!ACDInventoryCanHold(tACDPlayer, tACDRight, SLOT_PLAYER_BACKPACK, tPoint, NULL))
            return GAMEERROR_IARESULT_NOT_ENOUGH_ROOM;

        return GAMEERROR_NONE;
    }




    if (ItemUsesQuiver(tACDItem) &&
        GetInventoryItemClass(tACDRight.hGB.gbid) == IIC_QUIVER)
    {
        return GAMEERROR_NONE;
    }


    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD) &&
        GetInventoryItemClass(tACDRight.hGB.gbid) == IIC_SHIELD)
    {
        return GAMEERROR_NONE;
    }


    IVector2D tPoint;
    if (!ACDInventoryCanHold(tACDPlayer, tACDRight, SLOT_PLAYER_BACKPACK, tPoint, NULL))
        return GAMEERROR_IARESULT_NOT_ENOUGH_ROOM;



    return GAMEERROR_NONE;
}



static GameError sCheckForRightArmConflict(const ActorCommonData & tACD, const ActorCommonData & tACDItem, InventorySlotType eRightSlot, InventorySlotType eLeftSlot, const ItemTypeDefinition * pItemTypeDef)
{





    EInvItemClass eHand = GetInventoryItemClass(tACDItem.hGB.gbid);

    if (eHand == IIC_TWOHANDED)
    {
        return sCheckForLeftArmConflict(tACD, tACDItem, eLeftSlot, eRightSlot, pItemTypeDef);
    }


    if (!ItemIsAllowedInSlot(eRightSlot, pItemTypeDef))
        return GAMEERROR_IARESULT_ITEM_TYPE_DOESNT_MATCH_SLOT;

    ACDID idACDPlayer;
    ACDID idACDHireling;
    ACDID idACDOwner;

    sGetPlayerPetAndOwner(tACD.id, eLeftSlot, eRightSlot, idACDPlayer, idACDHireling, idACDOwner);
    if (idACDOwner == ACDID_INVALID)
        return GAMEERROR_IARESULT_UNKNOWN;




    if (eHand == IIC_ONEHANDED &&
        !ACDCanDualWield(tACD))
    {
        return GAMEERROR_IARESULT_UNKNOWN;
    }




    if (eHand == IIC_ONEHANDED && PlayerGetPlayerClass(tACD.id) == PLAYER_CLASS_DEMONHUNTER)
    {

        if (!ItemHasLabel(tACDItem.hGB.gbid, ITL_HAND_CROSSBOW))
            return GAMEERROR_IARESULT_UNKNOWN;




        InventoryLocation tInvLocLeftHand;
        ACDInventoryLocationSet(tInvLocLeftHand, tACD.id, SLOT_PLAYER_LEFT_HAND, 0, 0);
        ACDID idACDItemLeftHand = ACDInventoryGetItemInSlot(tInvLocLeftHand);
        ActorCommonData * ptACDItemLeftHand = ACDTryToGet(idACDItemLeftHand);

        if (ptACDItemLeftHand && !ItemHasLabel(ptACDItemLeftHand->hGB.gbid, ITL_HAND_CROSSBOW))
            return GAMEERROR_IARESULT_ITEM_REQUIRES_XBOW;
    }




    ActorCommonData & tACDOwner = ACDGet(idACDOwner);
    ACDID idACDLeft = sGetItemInSlot(tACDOwner, eLeftSlot);


    if (idACDLeft == ACDID_INVALID)
        return GAMEERROR_NONE;


    if (idACDPlayer == ACDID_INVALID)
        return GAMEERROR_IARESULT_UNKNOWN;



    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    ActorCommonData & tACDLeft = ACDGet(idACDLeft);
    if (ACDInventoryDoItemsConflict(tACDPlayer, tACDLeft, tACDItem))
    {


        ACDID idACDRight = sGetItemInSlot(tACDOwner, eRightSlot);
        if (idACDRight != ACDID_INVALID)
        {
            IVector2D tPoint;
            if (!ACDInventoryCanHold(tACDPlayer, tACDLeft, SLOT_PLAYER_BACKPACK, tPoint, NULL))
                return GAMEERROR_IARESULT_NOT_ENOUGH_ROOM;
        }
    }



    return GAMEERROR_NONE;
}



static void sGetInventorySlotInfo(const ActorInventory * ptInventory, int32 & nStart, int32 & nEnd, int32 & nSlots)
{






    ACDInventoryGetSlotTypeInfo(ptInventory->eType, nStart, nEnd, nSlots);
}


static InventoryType sGetInitialInventoryType(const ActorCommonData & tACD)
{

    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
    if (pdefActor->eType == ACTORTYPE_PLAYER)
        return INVENTORY_PLAYER;



    BOOL bNPC = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_STARTS_AS_NPC, tACD.snoActor);
    BOOL bMerchant = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_TRADE, tACD.snoActor);
    if (bNPC && bMerchant)
        return INVENTORY_MERCHANT;



    if (ACDIsHireling(tACD))
        return INVENTORY_HIRELING;



    if (tACD.IsItem() && ItemHasSockets(tACD))
        return INVENTORY_SOCKETS;



    return INVENTORY_NONE;
}



static InventoryType sGetForcedInventoryType(const ActorCommonData & tACD)
{

    if (tACD.IsItem())
        return INVENTORY_SOCKETS;



    return INVENTORY_GENERIC;
}


















void ActorInventoryInitializeForGame()
{

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    ptActorInventoryGlobals = (ActorInventoryGlobals *)SigmaMemoryNew(sizeof(ActorInventoryGlobals), 4, ptMemoryInterface, TRUE);

    ptActorInventoryGlobals->poolActorInventory.Initialize(sizeof(ActorInventory), INITIAL_INVENTORY_POOL_COUNT, ptMemoryInterface);
    ptActorInventoryGlobals->poolActorInventory.SetBlockCount_Tracked(INITIAL_INVENTORY_POOL_COUNT);
    ptActorInventoryGlobals->poolActorInventory.SetGrow(XALLOC_GROW_LINEAR, INITIAL_INVENTORY_POOL_COUNT);

    ptActorInventoryGlobals->poolInventorySlot.Initialize();
    ptActorInventoryGlobals->poolInventorySlot.SetMemoryInterface(ptMemoryInterface);
    ptActorInventoryGlobals->poolInventorySlot.SetBlockCount_Tracked(MAX_INVENTORY_SLOTS);

    ptActorInventoryGlobals->poolInventoryMap.Initialize();
    ptActorInventoryGlobals->poolInventoryMap.SetMemoryInterface(ptMemoryInterface);
    ptActorInventoryGlobals->poolInventoryMap.SetBlockCount_Tracked(MAX_INVENTORY_MAP_PAIRS);

    ptActorInventoryGlobals->ptItemMoveLog = new DirtyInventoryRecordMap;
    ptActorInventoryGlobals->ptMailMap = new MailRecordMap;

}



// NON_MATCHING: size 644 != 640; ours reloads the bucket count before the inlined clear() at 1366 (hoisting end() into a second for-init local matches, but the DWARF has only tIt)
void ActorInventoryDisposeForGame()

{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    if (ptActorInventoryGlobals)
    {
        ptActorInventoryGlobals->poolActorInventory.Dispose();
        ptActorInventoryGlobals->poolInventorySlot.Dispose();
        ptActorInventoryGlobals->poolInventoryMap.Dispose();

        if (ptActorInventoryGlobals->ptItemMoveLog)
        {

            if (!GameActiveGameCommonDataIsClient() &&
                !(SGameGlobalsGet().dwFlags & (1 << 14)))
            {



                BOOL fLeakFound = FALSE;
                for (DirtyInventoryRecordMap::iterator tIt = ptActorInventoryGlobals->ptItemMoveLog->begin(); tIt != ptActorInventoryGlobals->ptItemMoveLog->end(); ++tIt)
                {
                    fLeakFound = TRUE;
                }










                ptActorInventoryGlobals->ptItemMoveLog->clear();
            }

            delete ptActorInventoryGlobals->ptItemMoveLog;

            ptActorInventoryGlobals->ptMailMap->clear();
            delete ptActorInventoryGlobals->ptMailMap;
        }
        SigmaMemoryFree(ptActorInventoryGlobals, ptMemoryInterface); ptActorInventoryGlobals = NULL;
    }
}














static InventoryMapPool * sGetInventoryMapPool()
{

    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;

    return &ptActorInventoryGlobals->poolInventoryMap;
}




static ActorInventory * sAllocActorInventory()
{

    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;

    ActorInventory * ptActorInventory = (ActorInventory *)ptActorInventoryGlobals->poolActorInventory.Alloc_Tracked();

    SigmaMemoryClear(ptActorInventory, sizeof(ActorInventory));
    return ptActorInventory;
}





static void sFreeActorInventory(ActorInventory *& ptInventory)
{

    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;

    ptActorInventoryGlobals->poolActorInventory.Free(ptInventory);

    ptInventory = NULL;
}




static InventorySlot * sAllocInventorySlot()
{

    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;

    return ptActorInventoryGlobals->poolInventorySlot.Alloc_Tracked();
}






static void sFreeInventorySlot(InventorySlot *& ptInventorySlot)
{

    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;

    ptActorInventoryGlobals->poolInventorySlot.Free(ptInventorySlot);

    ptInventorySlot = NULL;
}



BOOL ACDInventoryInitialize(ActorCommonData & tACD, BOOL fForceInventoryCreate)
{



    InventoryType eInventoryType = sGetInitialInventoryType(tACD);
    if (eInventoryType == INVENTORY_NONE)
    {
        if (!fForceInventoryCreate)
            return FALSE;
        eInventoryType = sGetForcedInventoryType(tACD);
    }











    tACD.ptInventory = sAllocActorInventory();
    ActorInventory & tInventory = *tACD.ptInventory;
    tInventory.eType = eInventoryType;


    if (!GameActiveGameCommonDataIsClient())
    {
        tInventory.dwSeed = RandGetSeed(SGameGlobalsGet().rng);

        switch (eInventoryType)
        {
        case INVENTORY_MERCHANT:
        {   ScopedGlobalDefinitionLockR pdefGlobal;

            tACD.AttributesSetInt(ATTRIBUTE_INVENTORY_REROLL_TIME, GameGetServerTime() + pdefGlobal->ptServerData->nMinutesToRerollVendorInventory * TICKS_PER_MINUTE);
            ActorTriggerAttributeUpdate((ActorID)tACD.idOwner);
        }
            break;
        }
    }


    int32 nStart, nEnd, nSlots;
    ACDInventoryGetSlotTypeInfo(eInventoryType, nStart, nEnd, nSlots);
    tInventory.mapInventorySlots.Initialize(sGetInventoryMapPool());

    for (int32 i = nStart; i < nEnd; i++)
    {   IVector2D tSizeSlot;
        ACDInventorySlotGetSize(i, tSizeSlot);
        int32 nSquares = tSizeSlot.x * tSizeSlot.y;


        InventorySlot * ptSlot = sAllocInventorySlot();
        ptSlot->arSquares.resize(nSquares);
        for (int32 j = 0; j < nSquares; j++)
        {
            ptSlot->arSquares[j].idACD = ACDID_INVALID;
        }
        tInventory.mapInventorySlots.SetAt_Tracked(i, ptSlot);
    }

    return TRUE;
}




void ACDInventoryDispose(ActorInventory *& ptInventory)
{



    if (!ptInventory)
        return;



    POS posSlotMap = ptInventory->mapInventorySlots.GetStartAssoc();
    while (posSlotMap)
    {
        InventorySlot * ptSlot;
        uint32 dwSlot;
        ptInventory->mapInventorySlots.GetNextAssoc(posSlotMap, dwSlot, ptSlot);

        if (ptSlot)
        {
            IVector2D tSizeSlot;

            if (!GameActiveGameCommonDataIsClient())
            {
                for (uint32 j = 0; j < ptSlot->arSquares.size(); j++)
                {
                    InventorySquare & tSquare = ptSlot->arSquares[j];
                    if (tSquare.idACD != ACDID_INVALID)
                    {
                        ActorGlobals * ptActorGlobals = SGameManager::GameGet().ptActorGlobals;
                        if (ptActorGlobals)
                        {
                            ActorDelete((ActorID)ACDGetOwner(tSquare.idACD));
                        }
                    }
                }
            }



            sFreeInventorySlot(ptSlot);
        }
    }

    ptInventory->mapInventorySlots.Dispose();

    sFreeActorInventory(ptInventory);
}



BOOL ACDInventoryItemAllowedInThisSlotType(const GBID gbidItem, const InventorySlotType eSlot)
{
    ScopedItemDefinitionLockR pdefItem(gbidItem);
    if (!pdefItem)
        return FALSE;

    ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);
    if (!pItemTypeDef)
        return FALSE;

    return ItemIsAllowedInSlot(eSlot, pItemTypeDef);
}









GameError ACDInventoryItemAllowedInSlot(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc, const BOOL fSkipRequirements, const BOOL fSwapping)
{




    if (!tACDItem.IsItem())
        return GAMEERROR_IARESULT_UNKNOWN;



    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);
    if (ACDInventoryIsSlotMerchant(tInvLoc.eSlot))
    {
        if (!tACDContainer.IsMerchant())
            return GAMEERROR_IARESULT_UNKNOWN;
    }



    if (tInvLoc.eSlot == SLOT_PLAYER_BUYBACK)
    {

        if (ItemIsQuest(tACDItem))
            return GAMEERROR_IARESULT_CANT_TRADE_QUEST_ITEMS;
        return GAMEERROR_NONE;
    }







    if (ItemIsSocketable(tACDItem))
    {
        if (tInvLoc.eSlot == SLOT_ITEM_SOCKETS && ItemHasSockets(tACDContainer))
            return GAMEERROR_NONE;


        if (ACDInventorySlotIsEquippable(tInvLoc.eSlot) && !fSwapping)
        {



            ACDID idACDItemToReceiveGem = sGetItemInSlot(tACDContainer, tInvLoc.eSlot);
            ActorCommonData * ptACDItemToReceiveGem = ACDTryToGet(idACDItemToReceiveGem);
            if (ptACDItemToReceiveGem)
            {
                InventoryLocation tSocketLoc;
                IVector2D tSocketPt;
                if (!ACDInventoryCanHold(*ptACDItemToReceiveGem, tACDItem, SLOT_ITEM_SOCKETS, tSocketPt, NULL))
                    return GAMEERROR_IARESULT_NO_FREE_SOCKETS;



                if (ItemCanSocketInto(tACDItem, *ptACDItemToReceiveGem) != SOCKET_REASON_SUCCESS)
                    return GAMEERROR_ITEM_SOCKET_TYPE_MISMATCH;




                tSocketLoc.idACDContainer = ptACDItemToReceiveGem->id;
                tSocketLoc.eSlot = SLOT_ITEM_SOCKETS;
                tSocketLoc.pt = tSocketPt;

                return ACDInventoryItemAllowedInSlot(tACDItem, tSocketLoc, FALSE, FALSE);
            }
        }
    }



    HirelingClass eHireling = HirelingGetClass(tACDContainer);
    if (eHireling != HC_NONE)
    {
        if (!ACDInventoryIsSlotValidForHireling(tACDContainer, eHireling, tInvLoc.eSlot))
            return GAMEERROR_IARESULT_UNKNOWN;
    }



    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (!pdefItem)
        return GAMEERROR_IARESULT_UNKNOWN;



    ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);
    if (!pItemTypeDef)
        return GAMEERROR_IARESULT_UNKNOWN;



    if (!sDoesItemFitAtThisLocation(tACDItem, tInvLoc))
        return GAMEERROR_IARESULT_UNKNOWN;




    switch (tInvLoc.eSlot)
    {
    case SLOT_PLAYER_BACKPACK:
        return GAMEERROR_NONE;


    case SLOT_PLAYER_BASE_HEALTHPOTION:
        if (tACDItem.hGB.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_HEALTHPOTION_BOTTOMLESS))
            return GAMEERROR_NONE;
        return GAMEERROR_IARESULT_UNKNOWN;









    case SLOT_PLAYER_SHARED_STASH:
        return ItemHasLabel(tACDItem.hGB.gbid, ITL_QUEST) ? GAMEERROR_IARESULT_UNKNOWN : GAMEERROR_NONE;











    case SLOT_MERCHANT_DISPLAY:
    case SLOT_MERCHANT_SHOPLIST:
        return ItemIsCrafted(tACDItem) ? GAMEERROR_IARESULT_UNKNOWN : GAMEERROR_NONE;
    }











    if (!ItemIsAllowedInSlot(tInvLoc.eSlot, pItemTypeDef))
        return GAMEERROR_IARESULT_ITEM_TYPE_DOESNT_MATCH_SLOT;



    if (!ItemUsableByHireling(tACDItem.hGB.gbid, eHireling))
    {
        BOOL fSpecialItem = tInvLoc.eSlot == SLOT_HIRELING_SPECIAL;
        switch (eHireling)
        {
        case HC_TEMPLAR:
            return fSpecialItem ? GAMEERROR_IARESULT_TEMPLAR_CANT_EQUIP_SPECIAL_ITEM : GAMEERROR_IARESULT_TEMPLAR_CANT_EQUIP_ITEM;
        case HC_SCOUNDREL:
            return fSpecialItem ? GAMEERROR_IARESULT_SCOUNDREL_CANT_EQUIP_SPECIAL_ITEM : GAMEERROR_IARESULT_SCOUNDREL_CANT_EQUIP_ITEM;
        case HC_ENCHANTRESS:
            return fSpecialItem ? GAMEERROR_IARESULT_ENCHANTRESS_CANT_EQUIP_SPECIAL_ITEM : GAMEERROR_IARESULT_ENCHANTRESS_CANT_EQUIP_ITEM;
        }
    }

    GameError eRetVal = GAMEERROR_NONE;
    switch (tInvLoc.eSlot)
    {
    case SLOT_PLAYER_RIGHT_HAND:

        eRetVal = sCheckForRightArmConflict(tACDContainer, tACDItem, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, pItemTypeDef);
        break;
    case SLOT_PLAYER_LEFT_HAND:
        eRetVal = sCheckForLeftArmConflict(tACDContainer, tACDItem, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_RIGHT_HAND, pItemTypeDef);
        break;


    case SLOT_HIRELING_RIGHT_HAND:
        eRetVal = sCheckForRightArmConflict(tACDContainer, tACDItem, SLOT_HIRELING_RIGHT_HAND, SLOT_HIRELING_LEFT_HAND, pItemTypeDef);
        break;
    case SLOT_HIRELING_LEFT_HAND:
        eRetVal = sCheckForLeftArmConflict(tACDContainer, tACDItem, SLOT_HIRELING_LEFT_HAND, SLOT_HIRELING_RIGHT_HAND, pItemTypeDef);
        break;
    }

    if (eRetVal != GAMEERROR_NONE)
        return eRetVal;




    if (tInvLoc.eSlot != SLOT_PLAYER_BACKPACK &&
        tInvLoc.eSlot != SLOT_PLAYER_SHARED_STASH &&
        !ACDInventoryIsSlotMerchant(tInvLoc.eSlot))
    {

    if (ItemIsUnidentified(tACDItem))
        return GAMEERROR_IARESULT_CANT_EQUIP_UNIDENTIFIED_ITEM;



    eRetVal = GAMEERROR_IARESULT_UNKNOWN;
    if (!fSkipRequirements && !ItemOwnerMeetsRequirements(tACDItem, tACDContainer, &eRetVal))
    {

        return eRetVal;
    }


    if (ItemIsUniqueEquipped(tACDItem))
    {


        int32 slotStart = SLOT_PLAYER_BODYSLOT_START; int32 slotEnd = SLOT_PLAYER_BODYSLOT_END; if (eHireling != HC_NONE) { slotStart = SLOT_HIRELING_START; slotEnd = SLOT_HIRELING_END; }






        for (int32 i = slotStart; i < slotEnd; i++)
        {
            InventoryLocation tInvLocSlot;
            ACDInventoryLocationSet(tInvLocSlot, tACDContainer.id, i, 0, 0);
            ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
            ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
            if (ptACDItemInSlot &&
                idACDItemInSlot != tACDItem.id &&
                i != tInvLoc.eSlot)
            {
                if (tACDItem.hGB.gbid == ptACDItemInSlot->hGB.gbid)
                    return GAMEERROR_ITEM_UNIQUE_EQUIPPED;



                if (pdefItem->gbidLegendaryFamily != GBID_NONE)
                {
                    ScopedItemDefinitionLockR pdefItemInSlot(ptACDItemInSlot->hGB.gbid);
                    if (pdefItemInSlot &&
                        pdefItemInSlot->gbidLegendaryFamily != GBID_NONE &&
                        pdefItem->gbidLegendaryFamily == pdefItemInSlot->gbidLegendaryFamily)
                    {
                        return GAMEERROR_ITEM_UNIQUE_EQUIPPED;
                    }
                }
            }
        }
    }



    if (ItemHasSockets(tACDItem))
    {

        GBID gbidUniqueEquippedSocketables[3];
        int32 nUniqueEquippedSocketables = ItemGetSocketedUniqueEquipped(tACDItem, gbidUniqueEquippedSocketables, 3);


        if (ActorInventoryHasUniqueEquippedSocketablesEquipped(tACDContainer, gbidUniqueEquippedSocketables,
                                                               nUniqueEquippedSocketables,
                                                               tACDItem.id,
                                                               tInvLoc.eSlot))
        {
            return GAMEERROR_EQUIP_ITEM_SOCKET_UNIQUE_EQUIPPED;
        }
    }


    }
    return GAMEERROR_NONE;
}
















int32 ACDInventoryStashTabsBuyableWithGold(ActorCommonData & tACDPlayer)
{
    return MAX_STASH_TABS_CONSOLE;
}






























































































































int32 ACDInventoryGetNumUnlockedStashTabs(ActorCommonData & tACDPlayer)
{
    return MAX_STASH_TABS_CONSOLE;
}

















































int32 ACDInventoryGetMaxStashSlots(ActorCommonData & tACDPlayer)
{
    IVector2D tSlotPageSize;
    ACDInventoryGetSlotPageSize(SLOT_PLAYER_SHARED_STASH, tSlotPageSize);
    int32 nMaxSlots = ACDInventoryGetNumUnlockedStashTabs(tACDPlayer) * tSlotPageSize.x * tSlotPageSize.y;
    return nMaxSlots;
}







void ACDInventoryGetSlotPageSize(const InventorySlotType eSlot, IVector2D & tSize)
{


    switch (eSlot)
    {
    case SLOT_PLAYER_SHARED_STASH:
        tSize.Set(7, 10);
        break;
    default:
        ACDInventorySlotGetSize(eSlot, tSize);
    }
}



void ACDInventorySlotGetSize(const InventorySlotType eSlot, IVector2D & tSize)
{
    switch (eSlot)
    {
    case SLOT_PLAYER_BACKPACK: tSize.Set(10, 6); break;
    case SLOT_PLAYER_SHARED_STASH: tSize.Set(7, 10 * MAX_STASH_TABS); break;
    case SLOT_PLAYER_BUYBACK: tSize.Set(12, 1); break;
    case SLOT_MERCHANT_DISPLAY:
    case SLOT_MERCHANT_SHOPLIST:
        tSize.Set(48, 1);
        break;
    case SLOT_ITEM_SOCKETS:
        tSize.Set(1, 3);
        break;
    default:
        tSize.Set(1, 1);
        break;
    }
}



int32 ACDInventorySlotGetUpgradeIncrement(const InventorySlotType eSlot)
{
    if (eSlot == SLOT_PLAYER_SHARED_STASH)
        return 20;
    return 0;
}
















BOOL ACDInventorySlotIsMultiItemSlot(InventorySlotType eSlot)
{
    IVector2D size;

    ACDInventorySlotGetSize(eSlot, size);
    if (size.x > 1 || size.y > 1)
        return TRUE;
    return FALSE;
}






BOOL ACDInventoryValidateSlotForACD(const ActorCommonData & tACD, const InventorySlotType eSlot)
{

    if (!tACD.HasInventory())
        return FALSE;




    InventoryType eInventoryType = ACDInventoryGetType(tACD);
    switch (eInventoryType)
    {
    case INVENTORY_NONE:
        return FALSE;

    case INVENTORY_PLAYER:
        break;



    case INVENTORY_MERCHANT:
        if (!tACD.IsMerchant())
            return FALSE;
        break;






    case INVENTORY_SOCKETS:
        if (!ItemHasSockets(tACD) &&



            !ItemIsUnidentified(tACD))
            return FALSE;
        break;









    case INVENTORY_HIRELING:
        if (HirelingGetClass(tACD) == HC_NONE)
            return FALSE;
        break;





    case INVENTORY_GENERIC:
        if (!tACD.IsNPC() && !ACDGetInventory(&tACD, FALSE) && !ACDGetInventory(&tACD, TRUE))
            return FALSE;
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    int32 nStartSlot;
    int32 nEndSlot;
    int32 nNumSlots;



    ACDInventoryGetSlotTypeInfo(eInventoryType, nStartSlot, nEndSlot, nNumSlots);






    return eSlot >= nStartSlot && eSlot < nEndSlot;
}






















































BOOL ACDInventoryFindNonFullStackInSlot(const ActorCommonData & tACDItem, const ActorCommonData & tACDContainer, const InventorySlotType eCheckSlot, InventoryLocation & tNewLocation, const int32 * pnSlotYMinMax)
{






    if (ItemIsUnidentified(tACDItem) || ItemGetBindingLevel(tACDItem) == IBL_BOUND_TO_GAME)
        return FALSE;



    uint64 uItemStackSize = Math::Max<uint64>(1, tACDItem.GetStackCount());

    IVector2D tSize;
    ACDInventorySlotGetSize(eCheckSlot, tSize);


    int32 nSlotYMin = 0;
    int32 nSlotYMax = tSize.y;
    if (pnSlotYMinMax)
    {
        nSlotYMin = pnSlotYMinMax[0];
        nSlotYMax = pnSlotYMinMax[1];
    }



    IVector2D tPoint;
    for (tPoint.y = nSlotYMin; tPoint.y < nSlotYMax; tPoint.y++)
    {
        for (tPoint.x = 0; tPoint.x < tSize.x; tPoint.x++)
        {

            ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDContainer, eCheckSlot, tPoint);
            ActorCommonData * ptACDSquare = ACDTryToGet(idACDSquareItem);
            if (!ptACDSquare)
                continue;



            if (ptACDSquare->hGB.gbid != tACDItem.hGB.gbid)
                continue;




            if (ItemIsUnidentified(*ptACDSquare) || ItemGetBindingLevel(*ptACDSquare) == IBL_BOUND_TO_GAME)
                continue;



            uint64 uStackSize = Math::Max<uint64>(1, ptACDSquare->GetStackCount());
            if (uStackSize + uItemStackSize <= ItemGetMaxStackSize(tACDItem.hGB, &tACDContainer, NULL))
            {
                tNewLocation.idACDContainer = tACDContainer.id;
                tNewLocation.eSlot = eCheckSlot;
                tNewLocation.pt = tPoint;
                return TRUE;
            }
        }
    }

    return FALSE;
}
BOOL ACDInventoryStackItems(ActorCommonData & tACDSrcItem, ActorCommonData & tACDDestItem)
{
    return sStackItems(tACDSrcItem, tACDDestItem, tACDSrcItem.GetStackCount(), TRUE);
}





static InventorySlot * sACDInventoryGetSlot(const ActorCommonData & tACD, const InventorySlotType eSlot)
{
    InventorySlot * ptSlot;
    tACD.ptInventory->mapInventorySlots.Lookup(eSlot, ptSlot);
    return ptSlot;
}

































ACDID ACDInventoryGetSquareItem(const ActorCommonData & tACD, const InventorySlotType eSlot, const IVector2D & pt)
{
    BOOL fValid = ACDInventoryValidateSlotForACD(tACD, eSlot);





    if (!fValid)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    InventorySlot * ptSlot = sACDInventoryGetSlot(tACD, eSlot);



    IVector2D tSlotSize;

    ACDInventorySlotGetSize(eSlot, tSlotSize);



    int32 nDesiredIndex = pt.x + pt.y * tSlotSize.x;



    InventorySquare & tSquare = ptSlot->arSquares[nDesiredIndex];
    return tSquare.idACD;
}
InventorySquare * sGetSquare(const ActorCommonData & tACD, InventorySlotType eSlot, const IVector2D & pt)
{
    BOOL fValid = ACDInventoryValidateSlotForACD(tACD, eSlot);
    if (!fValid)
        return NULL;

    IVector2D tSlotSize;
    ACDInventorySlotGetSize(eSlot, tSlotSize);
    InventorySlot * ptSlot = sACDInventoryGetSlot(tACD, eSlot);
    if (pt.x >= 0 && pt.x < tSlotSize.x && pt.y >= 0 && pt.y < tSlotSize.y)
        return &ptSlot->arSquares[pt.x + pt.y * tSlotSize.x];
    return NULL;
}





















BOOL ACDInventoryCallbackSpecificSlotPosition(const InventoryLocation & tInvLoc, SlotSearchCallback pfnCallBack, void * pCallBackData)
{
    return pfnCallBack(tInvLoc, pCallBackData);
}

// NON_MATCHING: 568 vs 568 bytes, 19 instructions differ by register only (tStart.x/tStart.y clamp results swap w10/w11 and the loop counters w19/w26). Fixed: nEnd/nStart decl order (2504/2505), Math::Min(tSize, rect) operand order, layout on DWARF lines. Tried with no change: Max(0, x), if/else defaults, default-store order, swapped RightLeft statements.















void ACDInventoryIterateSlot(
    const ActorCommonData & tACD,
    InventorySlotType eSlot,
    const IRect2D * ptRect,
    SlotSearchCallback pfnCallBack,
    void * pCallBackData,
    BOOL fRightLeft,
    BOOL fBottomUp)
{





    int32 nEnd;
    int32 nStart;


    if (eSlot == SLOT_NONE)
    {
        int32 nSlots;
        sGetInventorySlotInfo(tACD.ptInventory, nStart, nEnd, nSlots);
    }
    else
    {

        nStart = eSlot;
        nEnd = eSlot + 1;
    }


    for (int32 i = nStart; i < nEnd; i++)
    {

        InventorySlotType eCurrentSlot = i;

        if (!ACDInventorySlotIsMultiItemSlot(eCurrentSlot))
        {
            InventoryLocation tInvLoc;
            ACDInventoryLocationSet(tInvLoc, tACD.id, eCurrentSlot, 0, 0);
            if (!ACDInventoryCallbackSpecificSlotPosition(tInvLoc, pfnCallBack, pCallBackData))
                return;
        }
        else
        {




            IVector2D tStart, tStop;




            IVector2D tSize;
            ACDInventorySlotGetSize(eCurrentSlot, tSize);
            tStart.x = 0; tStart.y = 0;
            tStop = tSize;
            if (ptRect)
            {
                // Inference: orig rows 2555:24 / 2556:23 hold the ptRect->left/top and right/bottom loads; the clamps sit on
                // their Max/Min call lines 2559-2562. The GetSize call (2545-2547 here) has no DIE of its own: it merged with the IsMultiItemSlot copy.







                tStart.x = Math::Max(ptRect->left, 0);
                tStart.y = Math::Max(ptRect->top, 0);
                tStop.x = Math::Min(tSize.x, ptRect->right);
                tStop.y = Math::Min(tSize.y, ptRect->bottom);
            }


            IVector2D tSaveStart = tStart;
            IVector2D tSaveStop = tStop;
            if (fRightLeft)
            {
                tStart.x = 1 - tSaveStop.x;
                tStop.x = tSaveStart.x + 1;
            }
            if (fBottomUp)
            {
                tStart.y = 1 - tSaveStop.y;
                tStop.y = tSaveStart.y + 1;
            }


            IVector2D tPoint;
            for (tPoint.y = tStart.y; tPoint.y < tStop.y; tPoint.y++)
            {
                for (tPoint.x = tStart.x; tPoint.x < tStop.x; tPoint.x++)
                {

                    IVector2D tNewPoint;
                    tNewPoint.x = abs(tPoint.x);
                    tNewPoint.y = abs(tPoint.y);







                    InventoryLocation tInvLoc;
                    ACDInventoryLocationSet(tInvLoc, tACD.id, eCurrentSlot, tNewPoint.x, tNewPoint.y);


                    if (!pfnCallBack(tInvLoc, pCallBackData))
                        return;
                }
            }
        }
    }




}


// inline: the DWARF has only the abstract DW_INL_inlined DIE (2614), no concrete out-of-line DIE, so no copy was emitted (an external non-inline function always is). Measured (only includer is this file): IterateItems 852 -> 828 (orig 832), extra out-of-line emission gone, every other function unchanged
inline void sACDInventoryCallbackSquare(
    InventorySlotType eSlot,
    InventorySquare & tSquare,
    uint32 dwACDIIFlags,
    InventorySearchCallback pfnCallback,
    void * pCallbackUserData)
{



    pfnCallback(tSquare.idACD, eSlot, pCallbackUserData);

    if (dwACDIIFlags & (1 << ACDII_CONTAINED_ITEMS))
    {



        ActorCommonData * ptACDSquare = ACDTryToGet(tSquare.idACD);

        if (ptACDSquare && ptACDSquare->HasInventory())
        {
            ACDInventoryIterateItems(*ptACDSquare, SLOT_NONE, dwACDIIFlags, pfnCallback, pCallbackUserData);
        }
    }
}

// NON_MATCHING: 828 vs 832 bytes; sACDInventoryCallbackSquare now inlined (inline keyword, see 2613) and the loop shape matches. Left: the original stores nEnd to its stack slot in each branch (2668 and the SLOT_NONE table path, loading nEnd before nStart); ours stores once at the join (spill placement)





void ACDInventoryIterateItems(
    const ActorCommonData & tACD,
    InventorySlotType eSlot,
    uint32 dwACDIIFlags,
    InventorySearchCallback pfnCallback,
    void * pCallbackUserData)
{


    int32 nEnd = 0;  // = 0 inferred: DWARF location of nEnd/nStart is constu 0 from 0x407604, before either is assigned
    int32 nStart = 0;


    if (eSlot == SLOT_NONE)
    {
        int32 nSlots;
        sGetInventorySlotInfo(tACD.ptInventory, nStart, nEnd, nSlots);
    }
    else
    {

        nStart = eSlot;
        nEnd = eSlot + 1;
    }


    for (int32 i = nStart; i < nEnd; i++)
    {

        InventorySlotType eCurrentSlot = i;


        if ((dwACDIIFlags & (1 << ACDII_IGNORE_SOCKET_SLOTS)) && eCurrentSlot >= SLOT_SOCKETS_START && eCurrentSlot <= SLOT_SOCKETS_END)
            continue;




        InventorySlot * ptSlot = sACDInventoryGetSlot(tACD, eCurrentSlot);

        for (uint32 nIndex = 0; nIndex < ptSlot->arSquares.size(); nIndex++)
        {
            InventorySquare & tSquare = ptSlot->arSquares[nIndex];
            if (ACDTryToGet(tSquare.idACD))
            {
                sACDInventoryCallbackSquare(eCurrentSlot, tSquare, dwACDIIFlags, pfnCallback, pCallbackUserData);
            }
        }
    }
}





static BOOL sTestTwoHandedSlotConflict(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem, InventorySlotType eSlot)
{


    InventorySlotType eOtherHand;
    switch (eSlot)
    {
    case SLOT_PLAYER_RIGHT_HAND:
        eOtherHand = SLOT_PLAYER_LEFT_HAND;
        break;
    case SLOT_PLAYER_LEFT_HAND:
        eOtherHand = SLOT_PLAYER_RIGHT_HAND;
        break;
    case SLOT_HIRELING_RIGHT_HAND:
        eOtherHand = SLOT_HIRELING_LEFT_HAND;
        break;
    case SLOT_HIRELING_LEFT_HAND:
        eOtherHand = SLOT_HIRELING_RIGHT_HAND;
        break;
    default:
        return FALSE;
    }

    ACDID idACDOther = ACDInventoryGetSquareItem(tACDPlayer, eOtherHand, cgptNull);
    ActorCommonData * ptACDOther = ACDTryToGet(idACDOther);
    if (!ptACDOther)
        return FALSE;
    if (eSlot == SLOT_PLAYER_LEFT_HAND || eSlot == SLOT_HIRELING_LEFT_HAND)
    {


        return ACDInventoryDoItemsConflict(tACDPlayer, tACDItem, *ptACDOther);
    }






    return ACDInventoryDoItemsConflict(tACDPlayer, *ptACDOther, tACDItem);
}










static BOOL sFindNonFullStackInSlot(const ActorCommonData & tACDItem, InventorySlotType eCheckSlot, InventoryLocation & tNewLocation)



{

    if (ItemIsUnidentified(tACDItem) || ItemGetBindingLevel(tACDItem) == IBL_BOUND_TO_GAME)
        return FALSE;



    const ActorCommonData & tACDContainer = ACDGet(tNewLocation.idACDContainer);
    IVector2D tSize;

    ACDInventorySlotGetSize(eCheckSlot, tSize);
    IVector2D tPoint;
    for (tPoint.y = 0; tPoint.y < tSize.y; tPoint.y++)
    {
        for (tPoint.x = 0; tPoint.x < tSize.x; tPoint.x++)
        {

            ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDContainer, eCheckSlot, tPoint);
            if (idACDSquareItem == ACDID_INVALID)
                continue;



            ActorCommonData * ptACDSquare = ACDTryToGet(idACDSquareItem);
            if (!ptACDSquare)
                continue;



            if (ptACDSquare->hGB.gbid != tACDItem.hGB.gbid)
                continue;




            if (ItemIsUnidentified(*ptACDSquare) || ItemGetBindingLevel(*ptACDSquare) == IBL_BOUND_TO_GAME)
                continue;



            uint64 uMaxStackSize = ItemGetMaxStackSize(ptACDSquare->hGB, &tACDContainer, NULL);
            uint64 uStackDest = Math::Max<uint64>(1, ptACDSquare->GetStackCount());
            uint64 uStackSrc = Math::Max<uint64>(1, tACDItem.GetStackCount());



            if (uStackSrc == 0x8000000000000000ULL)
                uStackSrc = 1;



            if (uStackDest == 0x8000000000000000ULL)
                uStackDest = 1;



            if (uStackSrc + uStackDest <= uMaxStackSize)
            {
                tNewLocation.eSlot = eCheckSlot;
                tNewLocation.pt = tPoint;
                return TRUE;
            }
        }
    }
    return FALSE;
}


static BOOL sFindNonFullStack(const ActorCommonData & tACDItem, InventoryLocation & tNewLocation)


{
    if (ItemGetMaxStackSize(tACDItem.hGB, NULL, NULL) < 2)
        return FALSE;


    InventorySlotType eCheckSlot[1];
    int32 nNumCheckSlots;
    if (ItemIsUnidentified(tACDItem) || ItemGetBindingLevel(tACDItem) == IBL_BOUND_TO_GAME)
        return FALSE;



    const ActorCommonData & tACD = ACDGet(tNewLocation.idACDContainer);
    if (!tACD.ptInventory)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    switch (tACD.ptInventory->eType)
    {
    case INVENTORY_MERCHANT:
        if (sFindNonFullStackInSlot(tACDItem, SLOT_MERCHANT_DISPLAY, tNewLocation)) return TRUE;
        if (sFindNonFullStackInSlot(tACDItem, SLOT_MERCHANT_SHOPLIST, tNewLocation)) return TRUE;
        return FALSE;
    case INVENTORY_HIRELING:
    case INVENTORY_GENERIC:
        return FALSE;
    default:
        eCheckSlot[0] = SLOT_PLAYER_BACKPACK;
        nNumCheckSlots = 1;
        break;
    }




    for (int32 i = 0; i < nNumCheckSlots; i++)
    {
        if (sFindNonFullStackInSlot(tACDItem, eCheckSlot[i], tNewLocation))
            return TRUE;
    }
    return FALSE;
}





BOOL ItemIsLockedFromTrade(const ActorCommonData & tACDItem); ACDID ItemGetLockedToACD(const ActorCommonData & tACDItem); BOOL ItemIsUnique(const ActorCommonData & tACDItem);
static void sACDInventoryChooseBestSlot(PickupResult & tResult,
                                        const ActorCommonData & tACDItem,
                                        BOOL bTryStashFirst,
                                        BOOL bTryStashOnly,
                                        BOOL bAllowAutoEquip)
{


    const ActorCommonData & tACDContainer = ACDGet(tResult.tNewLocation.idACDContainer);
    tResult.tNewLocation.pt = cgptNull;


    InventoryType eInvType = tACDContainer.ptInventory->eType;
    if (eInvType != INVENTORY_PLAYER)
        bTryStashFirst = FALSE;




    if (ItemGetMaxStackSize(tACDItem.hGB, NULL, NULL) > 1 && !bTryStashFirst)
    {
        if (sFindNonFullStack(tACDItem, tResult.tNewLocation))
        {
            tResult.ePickupAction = PICKUP_ACTION_STACK;
            return;
        }
    }


    if (eInvType == INVENTORY_PLAYER)
    {
        Player * ptPlayer = PlayerGetByACD(tACDContainer.id);


        for (int32 nSlot = SLOT_PLAYER_END - 1; nSlot >= SLOT_PLAYER_START; nSlot--)
        {
            InventorySlotType eSlot = (InventorySlotType)nSlot;

            if (bTryStashOnly && eSlot != SLOT_PLAYER_SHARED_STASH)
            {
                continue;
            }



            switch (eSlot)
            {
            case SLOT_PLAYER_BACKPACK:
                if (bTryStashFirst)
                {

                    if (sFindNonFullStackInSlot(tACDItem, SLOT_PLAYER_BACKPACK, tResult.tNewLocation))
                    {
                        tResult.ePickupAction = PICKUP_ACTION_STACK;
                        return;
                    }
                }
                break;
            case SLOT_PLAYER_BUYBACK:
            case SLOT_PLAYER_BASE_HEALTHPOTION:
                continue;

            case SLOT_PLAYER_SHARED_STASH:
                if (!bTryStashFirst)
                    continue;


                if (sFindNonFullStackInSlot(tACDItem, SLOT_PLAYER_SHARED_STASH, tResult.tNewLocation))
                {
                    tResult.ePickupAction = PICKUP_ACTION_STACK;
                    return;
                }
                break;
            }








            int32 bIgnoreBoE = FALSE;

            if (GameActiveGameCommonDataIsServer())
            {
                SGameGlobals & tSGameGlobals = SGameGlobalsGet();
                bIgnoreBoE = (tSGameGlobals.dwFlags2 & (1 << 1)) != 0;
            }

            if (!bAllowAutoEquip || !(ptPlayer->dwServerSidePrefFlags & (1 << SSPREF_AUTOEQUIP_ITEMS)) ||

                (!bIgnoreBoE &&
                 ItemGetBindingLevel(tACDItem) == IBL_ACCOUNT_EQUIP && ItemGetBoundToACD(tACDItem, FALSE) == ACDID_INVALID))
            {
                if (eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END)
                {
                    continue;
                }
            }


            if (ACDInventoryCanHold(tACDContainer, tACDItem, eSlot, tResult.tNewLocation.pt, NULL) &&
                !sTestTwoHandedSlotConflict(tACDContainer, tACDItem, eSlot))
            {

                if (!ACDInventorySlotIsEquippable(eSlot) ||

                    ItemOwnerMeetsRequirements(tACDItem, tACDContainer, NULL))
                {
                    tResult.tNewLocation.eSlot = eSlot;
                    tResult.ePickupAction = PICKUP_ACTION_MOVE_TO_LOCATION;
                    return;
                }
            }
        }
    }





    else if (eInvType == INVENTORY_MERCHANT)
    {
        if (ACDInventoryCanHold(tACDContainer, tACDItem, SLOT_MERCHANT_SHOPLIST, tResult.tNewLocation.pt, NULL))
        {
            tResult.tNewLocation.eSlot = SLOT_MERCHANT_SHOPLIST;
            tResult.ePickupAction = PICKUP_ACTION_MOVE_TO_LOCATION;
            return;
        }
    }

    else if (eInvType == INVENTORY_HIRELING || eInvType == INVENTORY_GENERIC)
    {

        for (int32 nSlot = SLOT_HIRELING_END - 1; nSlot >= SLOT_HIRELING_START; nSlot--)
        {
            InventorySlotType eSlot = (InventorySlotType)nSlot;

            if (ACDInventoryCanHold(tACDContainer, tACDItem, eSlot, tResult.tNewLocation.pt, NULL) &&
                !sTestTwoHandedSlotConflict(tACDContainer, tACDItem, eSlot))
            {



                if (ItemOwnerMeetsRequirements(tACDItem, tACDContainer, NULL))
                {
                    tResult.tNewLocation.eSlot = eSlot;


                    tResult.ePickupAction = PICKUP_ACTION_MOVE_TO_LOCATION;
                    return;
                }
            }
        }
    }

    tResult.tNewLocation.pt = cgptNull;
    tResult.tNewLocation.eSlot = SLOT_NONE;
}



static void sACDRemoveFromCurrentPosition(
    ActorCommonData & tACDItem)
{
    if (tACDItem.IsInWorld())
    {

        if (!GameActiveGameCommonDataIsClient())
        {


            SSceneID idSSceneNew = ACDGetCurrentSSceneID(tACDItem);
            SScenePutACDInScene(idSSceneNew, tACDItem.id, FALSE);
            ActorCollisionDispose(tACDItem);
            tACDItem.ChangeLocation(AL_NONE);
        }










        ACDCollDisposeSphereNode(tACDItem);
        return;
    }


    if (!tACDItem.IsInInventory())
        return;



    InventoryLocation & tInvLocation = tACDItem.GetInventoryLocation();
    ACDID idACDOwner = tInvLocation.idACDContainer;
    InventorySlotType eSlot = tInvLocation.eSlot;
    if (eSlot == SLOT_NONE)
        return;



    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);



    if (GameActiveGameCommonDataIsServer())
    {

        tACDItem.ModifiedMark(ACDMODIFIED_INVENTORY_POSITION);




        if (ptACDOwner->HasVisualInventory())
        {
            VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eSlot);
            if (eVisualSlot != VIS_INVALID)
            {
                BOOL bBrokenOrDisabled = ItemIsBroken(tACDItem) || ItemIsEquippedButDisabled(tACDItem);
                VisualInventorySetItemInSlot(idACDOwner, eVisualSlot, GBID_NONE, 0, 0, 0, bBrokenOrDisabled);
            }
        }

        BOOL fEquipped = ItemIsEquipped(tACDItem);
        if (fEquipped)
        {
            ACDInventoryEquipmentOrSkillChanged(*ptACDOwner);
        }

        if ((eSlot != SLOT_PLAYER_BACKPACK) && (eSlot != SLOT_PLAYER_BUYBACK) &&
            (eSlot != SLOT_PLAYER_SHARED_STASH) &&
            (eSlot != SLOT_MERCHANT_DISPLAY) &&
            (eSlot != SLOT_MERCHANT_SHOPLIST)
            && (fEquipped || eSlot == SLOT_ITEM_SOCKETS))
        {
            if (!ItemIsBroken(tACDItem) && !ItemIsEquippedButDisabled(tACDItem))
            {


                if (ptACDOwner->AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) > 0)
                {
                    ptACDOwner->AttributesUnmergeAttributeGroup(tACDItem.idUpscaledAttribGroup);
                    ClearUpscaledAttributes(tACDItem);
                }
                else
                {

                    AttribGroupID idFastAttribItem = SItemGetAttributeGroup(tACDItem);
                    ptACDOwner->AttributesUnmergeAttributeGroup(idFastAttribItem);
                }
            }

            tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED, 0);

            SACDInventorySetItemUnequipped(tACDItem);

            ptACDOwner->AttributesSetInt(MakeAttribKey(ATTRIBUTE_SLOT_UNEQUIPPED_TIME, eSlot), GameGetServerTime());


            if (eSlot == SLOT_PLAYER_RIGHT_HAND &&
                ItemHasLabel(tACDItem.hGB.gbid, ITL_WEAPON))
            {
                tACDItem.AttributesSetInt(ATTRIBUTE_HELD_IN_OFFHAND, 0);
                PlayerResetDualWield(idACDOwner);
            }
        }

        if (eSlot == SLOT_ITEM_SOCKETS)
        {
            int32 nSockets = ptACDOwner->AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
            ptACDOwner->AttributesSetInt(ATTRIBUTE_SOCKETS_FILLED, nSockets - 1);

            SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_SOCKET_TO_INVENTORY, SLOT_ITEM_SOCKETS, SLOT_NONE);
        }

        STradeRemoveItemFromTrade(tACDItem);
    }



    if (ptACDOwner)
    {

        InventorySlot * ptSlot = sACDInventoryGetSlot(*ptACDOwner, eSlot);
        for (uint32 i = 0; i < ptSlot->arSquares.size(); i++)
        {
            InventorySquare & tSquare = ptSlot->arSquares[i];
            if (tSquare.idACD == tACDItem.id)
            {
                tSquare.idACD = ACDID_INVALID;
                break;
            }
        }
    }



    InventoryLocation & tInvLocation2 = tACDItem.GetInventoryLocation();
    tInvLocation2.idACDContainer = ACDID_INVALID;
    tInvLocation2.eSlot = SLOT_NONE;
    tInvLocation2.pt = cgptNull;

}




void ACDRemoveFromCurrentPosition(ActorCommonData & tACDItem)
{


    if (GameActiveGameCommonDataIsServer() &&
        tACDItem.IsInInventory())
    {
        InventoryLocation & tInvLocation = tACDItem.GetInventoryLocation();
        ACDID idACDOwner = tInvLocation.idACDContainer;

        if (ACDIsPlayer(idACDOwner))
        {
            ScriptEventOnPlayerDroppedItem(idACDOwner, tACDItem.id);
        }
    }


    sACDRemoveFromCurrentPosition(tACDItem);
}




void sSocketLimboIterateCB(ACDID idACDSquare, InventorySlotType, void * pUserData)
{




    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem)
    {
        KnownListItemDeleted(KET_ACD, ptACDItem->id);
    }
}





void ACDInventoryRemoveToLimbo(const ACDID, ActorCommonData & tACDItem, BOOL bRemoveFromKnownList)
{















    ACDRemoveFromCurrentPosition(tACDItem);






    tACDItem.ChangeLocation(AL_NONE);


    if (GameActiveGameCommonDataIsServer() && bRemoveFromKnownList)
    {

        KnownListItemDeleted(KET_ACD, tACDItem.id);

        uint32 dwSocketsFilled = tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
        if (dwSocketsFilled)
        {

            ACDInventoryIterateItems(tACDItem, SLOT_ITEM_SOCKETS, 0, sSocketLimboIterateCB, NULL);
        }
    }

}




void ACDRemoveToLimbo(ActorCommonData & tACDItem)
{


    ACDRemoveFromCurrentPosition(tACDItem);



    tACDItem.ChangeLocation(AL_NONE);
}




void ACDInventoryDropFromLimbo(ActorCommonData & tACDContainer, ActorCommonData & tACDItem, const WorldPlace * ptPlace)
{

















    ACDID idACDUltimateContainer = ACDInventoryGetUltimateContainer(tACDContainer);


    if (idACDUltimateContainer == ACDID_INVALID)
    {
        idACDUltimateContainer = tACDContainer.id;
    }

    tACDItem.ModifiedMark(ACDMODIFIED_ATTRIBUTES);

    Player * ptPlayer = PlayerGetByACD(idACDUltimateContainer);
    if (ptPlayer)
    {
        tACDItem.SetItemSourcePlayerID(ptPlayer->tAccount.tHandle.m_id);
    }

    FlippyDropCreateOnActor(tACDContainer.id, tACDItem.id, *ptPlace);
}






void sSocketDropItemsIterateCB(ACDID idACDSquare, InventorySlotType, void * pUserData)
{



    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem)
    {
        InventorySlotType * eOriginalSlot = (InventorySlotType *)pUserData;

        SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_SOCKET_DROP, *eOriginalSlot, SLOT_NONE);
    }
}





void ACDInventoryDrop(ActorCommonData & tACDWantingToDrop, ActorCommonData & tACDItem, const WorldPlace * ptPlace)
{








    if (GameActiveGameCommonDataIsServer())
    {

        InventorySlotType eOriginalSlot = tACDItem.GetLocationSlot();

        SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_DROP, tACDItem.GetLocationSlot(), -1);


        uint32 dwSocketsFilled = tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
        if (dwSocketsFilled)
        {

            ACDInventoryIterateItems(tACDItem, SLOT_ITEM_SOCKETS, 0, sSocketDropItemsIterateCB, &eOriginalSlot);
        }
    }




    const InventoryLocation * ptInvLocation = tACDItem.TryToGetInventoryLocation();
    if (ptInvLocation)
    {
        ACDInventoryRemoveToLimbo(ptInvLocation->idACDContainer, tACDItem, TRUE);
    }





    if (GameActiveGameCommonDataIsServer())
    {

        ActorID idActorDropper = (ActorID)ACDGetOwner(tACDWantingToDrop.id);
        Actor * ptActorDropper = ActorTryToGet(idActorDropper);
        if (ptActorDropper)
        {
            PowerID idPowerPrimary = ActorPowersGetPrimary(idActorDropper);
            Power * ptPower = PowerTryToGet(idPowerPrimary);
            SNO snoPowerIdentify = ActorInventoryGetIdentifyPower(tACDWantingToDrop);
            if (ptPower && snoPowerIdentify != SNO_NONE &&

                ptPower->snoPower == snoPowerIdentify)
            {
                ACDID idACDTarget = ptPower->tTarget.GetACD();
                if (idACDTarget == tACDItem.id)
                {
                    ActorPowersForceStopPrimary(idActorDropper, TRUE);
                }
            }
        }



        Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);
        ptActor->tItemInventoryId.SetInvalid();

        Player * ptPlayer = PlayerGetByACD(tACDWantingToDrop.id);
        if (ptPlayer)
        {
            if (SPlayerHasTradeRestrictions(*ptPlayer))
            {
                tACDItem.SetAssignedHeroID(ptPlayer);
            }


            if (GameIsSeasonal())
            {
                tACDItem.SetAssignedHeroID(ptPlayer);
            }
        }




        if (ItemIsAutoPickup(tACDItem))
        {
            tACDItem.AttributesSetInt(ATTRIBUTE_NO_AUTO_PICKUP, TRUE);
        }


        ACDInventoryDropFromLimbo(tACDWantingToDrop, tACDItem, ptPlace);
        ACDInventoryEquipmentOrSkillChanged(tACDWantingToDrop);





    }
}


BOOL ACDInventoryCanMergeAttributes(const ActorCommonData & tACDContainer, const ActorCommonData & tACDItem, InventorySlotType eSlot)
{





    if (GameActiveGameCommonDataIsClient())
        return FALSE;





    if (ACDInventorySlotIsEquippable(eSlot))
    {
        if (ACDIsPlayer(tACDContainer.id))
            return TRUE;


        if (tACDContainer.IsPet())
            return TRUE;
    }





    if (eSlot == SLOT_ITEM_SOCKETS &&
        tACDContainer.AttributesGetInt(ATTRIBUTE_SOCKETS) > 0)
        return TRUE;

    return FALSE;
}



static void sShiftBuybackItemsDown(const ACDID idACDPlayer, const int32 nIndexToShiftTo)
{




    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    IVector2D tSlotSize; ACDInventorySlotGetSize(SLOT_PLAYER_BUYBACK, tSlotSize);
    for (int32 i = nIndexToShiftTo; i + 1 < tSlotSize.x; i++)
    {
        ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDPlayer, SLOT_PLAYER_BUYBACK, IVector2D(i + 1, 0));
        ActorCommonData * ptACDLaterItem = ACDTryToGet(idACDSquareItem);
        if (ptACDLaterItem)
        {

            InventoryLocation tNewLocation;
            ACDInventoryLocationSet(tNewLocation, tACDPlayer.id, SLOT_PLAYER_BUYBACK, i, 0);

            sACDRemoveFromCurrentPosition(*ptACDLaterItem);
            sACDAddToInventory(*ptACDLaterItem, tNewLocation, TRUE, SLOT_PLAYER_BUYBACK, FALSE);
        }
    }
}




void MoveItemToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, BOOL bMarkItemAsDirty, BOOL bOnHeroLoad)
{




    BOOL fOldLocationIsValid = FALSE;
    InventoryLocation tOldLocation;
    if (GameActiveGameCommonDataIsServer() &&

        tACDItem.IsInInventory())
    {
        tOldLocation = tACDItem.GetInventoryLocation();
        fOldLocationIsValid = TRUE;

        if (ACDIsPlayer(tOldLocation.idACDContainer) && tLocation.idACDContainer != tOldLocation.idACDContainer)
        {
            ScriptEventOnPlayerDroppedItem(tOldLocation.idACDContainer, tACDItem.id);
        }
    }



    InventorySlotType eCameFrom = tACDItem.GetLocationSlot();
    sACDRemoveFromCurrentPosition(tACDItem);
    sACDAddToInventory(tACDItem, tLocation, bMarkItemAsDirty, eCameFrom, bOnHeroLoad);


    if (tOldLocation.eSlot == SLOT_PLAYER_BUYBACK && fOldLocationIsValid)
    {


        sShiftBuybackItemsDown(tOldLocation.idACDContainer, tOldLocation.pt.x);
    }




    if (GameActiveGameCommonDataIsServer())
    {
        BOOL bNewLocationIsOnPlayer = ACDIsPlayer(tLocation.idACDContainer);
        if (!fOldLocationIsValid || tLocation.idACDContainer != tOldLocation.idACDContainer)
        {
            if (bNewLocationIsOnPlayer)
            {
                sOnPlayerPickedUpItem(tLocation.idACDContainer, tACDItem);
            }
            else
            {

                ActorCommonData & tACDContainer = ACDGet(tLocation.idACDContainer);
                if (ACDIsHireling(tACDContainer) && fOldLocationIsValid && ACDIsPlayer(tOldLocation.idACDContainer))
                {
                    ScriptEventOnHirelingEquippedItem(tACDContainer.id, tACDItem.id);
                }
            }
        }



        if (bNewLocationIsOnPlayer && ACDInventorySlotIsEquippable(tLocation.eSlot))
        {
            PowerAPI_BuffApplyOnEquipmentChangeEffects(tLocation.idACDContainer, tLocation.eSlot, 0);
        }
    }
}














void sSocketCountItemsIterateCB(
    ACDID idACDSquare,
    InventorySlotType,
    void * pUserData)
{
    uint32 * ptCount = (uint32 *)pUserData;


    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem)
    {
        (*ptCount)++;
    }
}

void sSocketUpdateItemsIterateCB(ACDID idACDSquare, InventorySlotType, void * pUserData)
{



    if (idACDSquare != ACDID_INVALID)
    {
        SocketUpdateInfo * info = (SocketUpdateInfo *)pUserData;
        ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
        if (ptACDItem)
        {

            Actor * ptGem = ActorGet((ActorID)ptACDItem->idOwner);
            InventoryId & tInventoryId = ptGem->tItemInventoryId;



            tInventoryId = info->tInventoryId;
            SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_SOCKET_MOVED, info->eCameFrom, info->eTo);
        }
        else
        {



            info->bRepairSocketsFilledCount = TRUE;
        }
    }
}




static void sACDAddToInventory(ActorCommonData & tACDItem,
                               const InventoryLocation & tNewLocation,
                               BOOL bMarkItemAsDirty,
                               InventorySlotType eCameFrom,
                               BOOL bOnHeroLoad)

{




    tACDItem.ChangeLocation(AL_INVENTORY);
    InventoryLocation & tCurrentInvLocation = tACDItem.GetInventoryLocation();
    IVector2D pt;


    if (GameActiveGameCommonDataIsServer()) { }














    tCurrentInvLocation = tNewLocation;

    ActorCommonData & tACDContainer = ACDGet(tNewLocation.idACDContainer);


    if (GameActiveGameCommonDataIsServer())
    {
        ACDID idACDUltOwner = ActorInventoryGetUltimateOwner(&tACDItem);
        ActorCommonData * ptACDUltOwner = ACDTryToGet(idACDUltOwner);
        if (ptACDUltOwner && ptACDUltOwner->HasInventory())
        {




            VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(tNewLocation.eSlot);
            if (eVisualSlot != VIS_INVALID)
            {
                DyeType eDyeType = tACDItem.GetDyeType();
                ItemEffectType eItemEffectType;
                int32 nEffectLevel;
                ACDGetItemEffectTypeAndLevel(tACDItem, eItemEffectType, nEffectLevel, TRUE, FALSE);

                BOOL bBrokenOrDisabled = ItemIsBroken(tACDItem) || ItemIsEquippedButDisabled(tACDItem);
                VisualInventorySetItemInSlot(tACDContainer.id, eVisualSlot, tACDItem.GetVisualGBID(), eDyeType, eItemEffectType, nEffectLevel, bBrokenOrDisabled);
            }



            InventoryId tInventoryId;
            if (ACDIsPlayer(ptACDUltOwner->id))
            {
                Player * pPlayer = PlayerGetByACD(ptACDUltOwner->id);
                if (pPlayer)
                {
                    InventoryLocation tOwnerLocation = tNewLocation;



                    if (tACDContainer.IsItem())
                    {
                        tOwnerLocation = tACDContainer.GetInventoryLocation();
                    }


                    if (tOwnerLocation.eSlot == SLOT_PLAYER_SHARED_STASH)
                    {
                        tInventoryId = pPlayer->tAccount.tHandle;
                    }
                    else
                    {
                        tInventoryId = pPlayer->uHeroId;
                    }






                    if (eCameFrom == SLOT_PLAYER_SHARED_STASH)
                    {
                        SACDInventoryEvaluateAcquireItemAchievementEvent(*pPlayer, tACDItem);
                    }
                }
            }
            else if (ptACDUltOwner->IsPet())
            {
                Player * ptOwner = ptACDUltOwner->GetPetOwnerPlayer();
                if (ptOwner)
                {
                    tInventoryId = ptOwner->uHeroId;
                }
            }





            if (tInventoryId.IsInvalid()) { }




            Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);
            ptActor->tItemInventoryId = tInventoryId;


            if (bMarkItemAsDirty && tNewLocation.eSlot != SLOT_ITEM_SOCKETS)
            {
                SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_MOVE, eCameFrom, tNewLocation.eSlot);

                uint32 dwSocketsFilled = tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
                if (dwSocketsFilled)
                {

                    SocketUpdateInfo info;
                    info.tInventoryId = tInventoryId;
                    info.eCameFrom = eCameFrom;
                    info.eTo = tNewLocation.eSlot;
                    info.bRepairSocketsFilledCount = FALSE;
                    ACDInventoryIterateItems(tACDItem, SLOT_NONE, 0, sSocketUpdateItemsIterateCB, &info);

                    if (info.bRepairSocketsFilledCount)
                    {
                        uint32 uSocketsFilledCount = 0;
                        ACDInventoryIterateItems(tACDItem, SLOT_NONE, 0, sSocketCountItemsIterateCB, &uSocketsFilledCount);
                        tACDItem.AttributesSetInt(ATTRIBUTE_SOCKETS_FILLED, uSocketsFilledCount);
                    }
                }
            }
        }
    }



    pt = cgptNull;
    if (ACDInventorySlotIsMultiItemSlot(tNewLocation.eSlot))
    {
        pt.Set(tNewLocation.pt.x, tNewLocation.pt.y);
    }

    InventorySquare * ptSquare = sGetSquare(tACDContainer, tNewLocation.eSlot, pt);
    if (ptSquare)
    {








        if (ACDTryToGet(ptSquare->idACD))
        {









            ACDRemoveFromCurrentPosition(ACDGet(ptSquare->idACD));
        }

        ptSquare->idACD = tACDItem.id;


        if (GameActiveGameCommonDataIsServer())
        {

            ItemsPreEquip(tACDItem, tACDContainer, tNewLocation.eSlot);




            if (bMarkItemAsDirty && tNewLocation.eSlot == SLOT_ITEM_SOCKETS)
            {

                ActorInventoryDatabaseAction eGemAction = eCameFrom == SLOT_PLAYER_SHARED_STASH ? AIDA_UPDATE_STASH_TO_SOCKET : AIDA_UPDATE_INVENTORY_TO_SOCKET;




                SACDInventoryAddToDirtyList(tACDItem, eGemAction, eCameFrom, SLOT_ITEM_SOCKETS);
            }

            if (ACDInventoryCanMergeAttributes(tACDContainer, tACDItem, tNewLocation.eSlot))
            {
                if (tNewLocation.eSlot != SLOT_ITEM_SOCKETS)
                {
                    SACDInventorySetItemEquipped(tACDItem, bOnHeroLoad);

                    tACDContainer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SLOT_UNEQUIPPED_TIME, tNewLocation.eSlot), 0);
                    tACDItem.AttributesSetInt(ATTRIBUTE_PICKED_UP_TIME, 0);


                    if (ACDIsPlayer(tACDContainer.id))
                    {
                        Player * ptPlayer = PlayerGetByACD(tACDContainer.id);
                        POS pos = tACDItem.listIdentifiedAffixGBIDs.GetHeadPosition();
                        while (pos)
                        {
                            GBID gbidAffix = tACDItem.listIdentifiedAffixGBIDs.GetNext(pos);
                            if (gbidAffix != GBID_NONE)
                            {
                                SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_ITEM_AFFIX_EQUIPPED, gbidAffix, 0, 1.0f);
                            }
                        }

                        if (tACDContainer.IsInCombat())
                        {
                            SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_ITEM_EQUIPPED_IN_COMBAT, 0, 0, 1.0f);
                        }
                    }


                    if (tNewLocation.eSlot == SLOT_PLAYER_RIGHT_HAND &&
                        ItemHasLabel(tACDItem.hGB.gbid, ITL_WEAPON))
                    {
                        AttribGroupID idFastAttribItem = SItemGetAttributeGroup(tACDItem);
                        FastAttribSetValue(idFastAttribItem, ATTRIBUTE_HELD_IN_OFFHAND, FastAttribValue(TRUE));
                    }
                }

                if (!ItemIsBroken(tACDItem) && !ItemIsEquippedButDisabled(tACDItem))
                {
                    ActorInventoryMergeItemAttributes(tACDContainer, tACDItem);
                }
            }

            if (ItemGetBoundToACD(tACDItem, FALSE) == ACDID_INVALID)
            {
                ItemBindingLevel eBindLevel = ItemGetBindingLevel(tACDItem);

                if ((tNewLocation.eSlot >= SLOT_PLAYER_START && tNewLocation.eSlot < SLOT_PLAYER_END && eBindLevel == IBL_ACCOUNT_PICKUP) ||

                    (tNewLocation.eSlot >= SLOT_PLAYER_BODYSLOT_START && tNewLocation.eSlot < SLOT_PLAYER_BODYSLOT_END && eBindLevel == IBL_ACCOUNT_EQUIP) ||

                    (tNewLocation.eSlot >= SLOT_HIRELING_START && tNewLocation.eSlot < SLOT_HIRELING_END && eBindLevel == IBL_ACCOUNT_EQUIP))
                {


                    Player * ptPlayer = tACDContainer.GetPetOwnerPlayer();
                    ACDID idACDUltimateOwner = ptPlayer ? ptPlayer->idPlayerACD : tACDContainer.id;
                    ItemSetBoundToACD(tACDItem, idACDUltimateOwner);
                }
            }

            tACDItem.ModifiedMark(8);
        }
        else
        {






            ACDID idACDUltOwner = ActorInventoryGetUltimateOwner(&tACDItem);
            ActorCommonData * ptACDUltOwner = ACDTryToGet(idACDUltOwner);

            if (tNewLocation.eSlot == SLOT_PLAYER_LEFT_HAND && ptACDUltOwner &&
                ptACDUltOwner->AttributesAttributeExists(ATTRIBUTE_WEAPON_EFFECT_OVERRIDE))
            {
                VisualInventoryResetSlot(idACDUltOwner, SLOT_PLAYER_LEFT_HAND);
            }
            ACDUpdateAppearance(tACDContainer.id);
        }
    }

}





static void sOnPlayerPickedUpItem(ACDID idACDPlayer, ActorCommonData & tACDItem)


{
    ScriptEventOnPlayerPickedUpItem(idACDPlayer, tACDItem.id);


    SNO snoLore = ItemGetLoreSNO(tACDItem);
    if (snoLore != SNO_NONE)
    {
        SPlayerLearnLore(idACDPlayer, snoLore);
    }

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        if (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_BOOSTED))
        {
            Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);
            if (ptActor)
                ptActor->dwActorFlags |= (1 << ACTOR_CHEATED);
        }

        if (ItemUnlocksTransmogOnPickup(tACDItem))
        {
            SItemCrafting_TryUnlockTransmog(tACDItem.hGB.gbid, *ptPlayer, TRUE, TRUE);
        }

        if (ItemGetBindingLevel(tACDItem) == IBL_BOUND_TO_GAME)
        {
            SItemRegisterTradingTimer(tACDItem, 0);
        }
    }
}





void ACDAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, const BOOL bMarkItemAsDirty, InventorySlotType eCameFrom)
{




    sACDAddToInventory(tACDItem, tLocation, bMarkItemAsDirty, eCameFrom, FALSE);


    if (GameActiveGameCommonDataIsServer() &&

        ACDIsPlayer(tLocation.idACDContainer))
    {

        sOnPlayerPickedUpItem(tLocation.idACDContainer, tACDItem);
    }
}





void ACDInventoryRestoreFromLimbo(
    ActorCommonData & tACDItem,
    InventoryLocation & tLocation)
{







    ACDAddToInventory(tACDItem, tLocation, FALSE, SLOT_NONE);


    ActorCommonData & tACDContainer = ACDGet(tLocation.idACDContainer);
    ACDID idACDOwner = ACDInventoryGetUltimateContainer(tACDContainer);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner)
    {
        ACDInventoryEquipmentOrSkillChanged(*ptACDOwner);
    }



}


BOOL ACDInventoryMove(ActorCommonData & tACDItem, const InventoryLocation & tDstLocation)
{



    if (!tACDItem.IsInInventory())
    {







        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    if (!GameActiveGameCommonDataIsClient())
    {
        if (ACDInventoryItemAllowedInSlot(tACDItem, tDstLocation, FALSE, FALSE) != GAMEERROR_NONE)
        {
            return FALSE;
        }
    }



    InventoryLocation tSrcLocation = tACDItem.GetInventoryLocation();


    MoveItemToInventory(tACDItem, tDstLocation, TRUE, FALSE);


    return TRUE;
}










static void sCountItemsMatchingGBHandle(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    ItemTypeCountCallbackData & tCallbackData = *(ItemTypeCountCallbackData *)pUserData;
    const ActorCommonData * ptACD = ACDTryToGet(idACDSquare);
    if (ptACD && ptACD->hGB == tCallbackData.gbHandleToMatch)
    {
        tCallbackData.uCount += Math::Max<uint64>(1, ptACD->GetStackCount());
        tCallbackData.idACDItem = ptACD->id;
    }
}






int32 ActorInventoryCountItemsOfTypeOnPlayer(const ACDID idACDContainer, const GBID gbidItem, ACDID * ptidACDItem)
{
    ActorCommonData & tACDContainer = ACDGet(idACDContainer);
    GBHandle gbHandle(GB_ITEMS, gbidItem);
    InventorySlotType tSlots[3] = { SLOT_PLAYER_BACKPACK, SLOT_PLAYER_BASE_HEALTHPOTION, SLOT_NONE };
    return ActorInventoryCountItemsOfType(tACDContainer, gbHandle, tSlots, ptidACDItem);
}
















int32 ActorInventoryCountItemsOfType(const ActorCommonData & tACDContainer, const GBHandle & gbHandleToMatch, InventorySlotType * peSlotsToSearch, ACDID * ptidACDItem)
{
    ItemTypeCountCallbackData tCallbackData;
    tCallbackData.uCount = 0;
    tCallbackData.gbHandleToMatch = gbHandleToMatch;
    tCallbackData.idACDItem = ACDID_INVALID;
    if (peSlotsToSearch)
    {
        for (int32 i = 0; peSlotsToSearch[i] != SLOT_NONE; i++)
        {
            ACDInventoryIterateItems(tACDContainer, peSlotsToSearch[i], (1 << ACDII_CONTAINED_ITEMS) | (1 << ACDII_IGNORE_SOCKET_SLOTS), sCountItemsMatchingGBHandle, &tCallbackData);
        }
    }
    else
    {
        ACDInventoryIterateItems(tACDContainer, SLOT_NONE, (1 << ACDII_CONTAINED_ITEMS) | (1 << ACDII_IGNORE_SOCKET_SLOTS), sCountItemsMatchingGBHandle, &tCallbackData);
    }
    if (ptidACDItem)
        *ptidACDItem = tCallbackData.idACDItem;
    if (tCallbackData.uCount > 0x7fffffff)
        tCallbackData.uCount = 0x7fffffff;
    return (int32)tCallbackData.uCount;
}











































static void sCountItemsMatchingActorSNO(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    SNOActorItemCountCallbackData & tCallbackData = *(SNOActorItemCountCallbackData *)pUserData;
    const ActorCommonData * ptACD = ACDTryToGet(idACDSquare);
    if (ptACD && ptACD->snoActor == tCallbackData.snoActorToMatch)
    {
        tCallbackData.uCount += Math::Max<uint64>(1, ptACD->GetStackCount());
        tCallbackData.idACDItem = ptACD->id;
    }
}






int32 ActorInventoryCountSNOActorItemsOnPlayer(const ACDID idACDContainer, const SNO snoItemActor, ACDID * ptidACDItem)
{
    ActorCommonData & tACDContainer = ACDGet(idACDContainer);
    SNOActorItemCountCallbackData tCallbackData;
    tCallbackData.uCount = 0;
    tCallbackData.snoActorToMatch = snoItemActor;
    tCallbackData.idACDItem = ACDID_INVALID;
    ACDInventoryIterateItems(tACDContainer, SLOT_PLAYER_BACKPACK, (1 << ACDII_CONTAINED_ITEMS) | (1 << ACDII_IGNORE_SOCKET_SLOTS), sCountItemsMatchingActorSNO, &tCallbackData);
    if (ptidACDItem)
        *ptidACDItem = tCallbackData.idACDItem;
    if (tCallbackData.uCount > 0x7fffffff)
        tCallbackData.uCount = 0x7fffffff;
    return (int32)tCallbackData.uCount;
}
























ACDID ACDInventoryGetUltimateContainer(const ActorCommonData & tACDItem)
{
    ACDID idACDUltimateContainer = ACDID_INVALID;
    const InventoryLocation * ptInvLocation = tACDItem.TryToGetInventoryLocation();
    while (ptInvLocation)
    {
        idACDUltimateContainer = ptInvLocation->idACDContainer;
        ptInvLocation = ACDGet(idACDUltimateContainer).TryToGetInventoryLocation();
    }
    return idACDUltimateContainer;
}










static BOOL sDoesItemFitAtThisLocation(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc)
{



    if (!ACDInventorySlotIsMultiItemSlot(tInvLoc.eSlot))
    {
        if (tInvLoc.pt.x != 0 || tInvLoc.pt.y != 0)
            return FALSE;
        return TRUE;
    }





    IVector2D tSlotSize;
    ACDInventorySlotGetSize(tInvLoc.eSlot, tSlotSize);


    if (tInvLoc.pt.x < 0 || tInvLoc.pt.x >= tSlotSize.x ||
        tInvLoc.pt.y < 0 || tInvLoc.pt.y >= tSlotSize.y)
    {
        return FALSE;
    }

    if (ActorInventoryIsVariableSizedSlot(tInvLoc.eSlot))
    {
        if (!ActorInventoryIsVariableSizedSlotValidForItem(tACDItem, tInvLoc))
            return FALSE;
    }


    return TRUE;
}





static void sAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tInvLoc, InventorySlotType eCameFrom)
{



    ACDAddToInventory(tACDItem, tInvLoc, TRUE, eCameFrom);
}




static int32 * sGetSlotMinMax(InventoryLocation & tInvLoc, int32 * nSlotYMinMax)
{


    int32 * pnSlotYMinMax = NULL;

    if (tInvLoc.eSlot == SLOT_PLAYER_SHARED_STASH)
    {

        IVector2D tSize;
        ACDInventoryGetSlotPageSize(tInvLoc.eSlot, tSize);

        int32 nPage = tInvLoc.pt.y / tSize.y;
        nSlotYMinMax[0] = nPage * tSize.y;
        nSlotYMinMax[1] = (nPage + 1) * tSize.y;
        pnSlotYMinMax = nSlotYMinMax;
    }

    return pnSlotYMinMax;
}




static BOOL sACDInventorySwap(ActorCommonData & tACDItem0, InventoryLocation & tInvLoc0, ActorCommonData & tACDItem1)
{






    InventoryLocation tInvLocOld0 = tACDItem0.GetInventoryLocation();
    InventoryLocation tInvLocOld1 = tACDItem1.GetInventoryLocation();


    InventoryLocation tInvLoc1 = tInvLocOld0;

    BOOL fItem0IsLarge = ItemIsLargeSlotItem(tACDItem0.hGB.gbid);
    BOOL fItem1IsLarge = ItemIsLargeSlotItem(tACDItem1.hGB.gbid);


    if (tInvLocOld0.eSlot == tInvLocOld1.eSlot && fItem0IsLarge != fItem1IsLarge &&
        tInvLocOld0.pt.x == tInvLocOld1.pt.x)
    {


        if (tInvLocOld0.pt.y + (fItem0IsLarge ? 2 : 1) == tInvLocOld1.pt.y)
        {

            tInvLoc0.pt.y = tInvLocOld0.pt.y + (fItem1IsLarge ? 2 : 1);
            tInvLoc1.pt.y = tInvLocOld0.pt.y; }

        else if (tInvLocOld1.pt.y + (fItem1IsLarge ? 2 : 1) == tInvLocOld0.pt.y)
        {

            tInvLoc0.pt.y = tInvLocOld1.pt.y;
            tInvLoc1.pt.y = tInvLocOld1.pt.y + (fItem0IsLarge ? 2 : 1);
        }
    }


    if (ACDInventoryItemAllowedInSlot(tACDItem0, tInvLoc0, FALSE, TRUE) != GAMEERROR_NONE)
        return FALSE;





    int32 nSlotYMinMax[2];
    int32 * pnSlotYMinMax = sGetSlotMinMax(tInvLoc1, nSlotYMinMax);

    BOOL fAdjusted = FALSE;
    GameError eResultItemInSlot = ACDInventoryItemAllowedInSlot(tACDItem1, tInvLoc1, FALSE, TRUE);
    if (eResultItemInSlot != GAMEERROR_NONE)
    {



        if (!fItem0IsLarge && fItem1IsLarge &&
            ActorInventoryIsVariableSizedSlot(tInvLoc1.eSlot))
        {

            if (!pnSlotYMinMax ||
                tInvLoc1.pt.y > pnSlotYMinMax[0])
            {
                tInvLoc1.pt.y--;


                eResultItemInSlot = ACDInventoryItemAllowedInSlot(tACDItem1, tInvLoc1, FALSE, TRUE);
                fAdjusted = TRUE;
            }
        }
        if (eResultItemInSlot != GAMEERROR_NONE)
            return FALSE;
    }



    ACDRemoveFromCurrentPosition(tACDItem0);
    ACDRemoveFromCurrentPosition(tACDItem1);


    sAddToInventory(tACDItem0, tInvLoc0, tInvLocOld0.eSlot);


    ACDID idACDItemInSlot;
    GameError eResultCanMove = ACDInventoryCanMove(tACDItem1, tInvLoc1, &idACDItemInSlot);
    if (eResultCanMove != GAMEERROR_NONE ||
        idACDItemInSlot != ACDID_INVALID)
    {


        // Original line 4410 puts its first branch at column 46, the first && of this order (the other terms' order is from the permuter match).
        if (idACDItemInSlot != ACDID_INVALID && !fAdjusted && fItem1IsLarge && !fItem0IsLarge &&


            ActorInventoryIsVariableSizedSlot(tInvLoc1.eSlot))
        {

            if (!pnSlotYMinMax ||
                tInvLoc1.pt.y > pnSlotYMinMax[0])
            {
                tInvLoc1.pt.y--;



                if (ACDInventoryItemAllowedInSlot(tACDItem1, tInvLoc1, FALSE, TRUE) == GAMEERROR_NONE)
                {
                    eResultCanMove = ACDInventoryCanMove(tACDItem1, tInvLoc1, &idACDItemInSlot);
                }
            }
        }

        if (eResultCanMove != GAMEERROR_NONE ||
            idACDItemInSlot != ACDID_INVALID)
        {

            if (!sACDInventoryFindBestLocation(tACDItem1, tInvLoc1, pnSlotYMinMax))
            {






















                InventorySlotType eCameFrom = tACDItem0.GetLocationSlot();
                ACDRemoveFromCurrentPosition(tACDItem0);


                ACDAddToInventory(tACDItem1, tInvLocOld1, TRUE, eCameFrom);


                ACDAddToInventory(tACDItem0, tInvLocOld0, TRUE, SLOT_NONE);


                ACDID idACDOwner = ACDInventoryGetUltimateContainer(tACDItem0);
                if (ACDIsPlayer(idACDOwner))
                {
                    PlayerDisplayErrorMessage(idACDOwner, GAMEERROR_IARESULT_NOT_ENOUGH_ROOM);
                }
                return TRUE;
            }
        }
    }



    sAddToInventory(tACDItem1, tInvLoc1, tInvLocOld1.eSlot);





    if (tInvLoc0.eSlot >= SLOT_PLAYER_START && tInvLoc0.eSlot < SLOT_PLAYER_END &&
        tInvLoc1.eSlot >= SLOT_HIRELING_START && tInvLoc1.eSlot < SLOT_HIRELING_END)
    {
        ScriptEventOnHirelingEquippedItem(tInvLoc1.idACDContainer, tACDItem1.id);
    }
    else if (tInvLoc1.eSlot >= SLOT_PLAYER_START && tInvLoc1.eSlot < SLOT_PLAYER_END &&
             tInvLoc0.eSlot >= SLOT_HIRELING_START && tInvLoc0.eSlot < SLOT_HIRELING_END)
    {
        ScriptEventOnHirelingEquippedItem(tInvLoc0.idACDContainer, tACDItem0.id);
    }

    return TRUE;
}





BOOL ACDInventoryAddToLocation(ActorCommonData & tACDContainer, ActorCommonData & tACDItem, const ActorLocation & tNewLocation, BOOL fSkipRequirements, BOOL bMarkAsDirty, BOOL bOnHeroLoad, BOOL bRemoveExistingItems)
{






    const IVector2D & pt = tNewLocation.tInvLocation.pt;



    const InventorySlotType eSlot = tNewLocation.tInvLocation.eSlot;


    if (!ACDInventoryValidateSlotForACD(tACDContainer, eSlot))
        return FALSE;






    if (ACDInventoryItemAllowedInSlot(tACDItem, tNewLocation.tInvLocation, fSkipRequirements, FALSE) != GAMEERROR_NONE)
        return FALSE;







    InventoryLocation tInvLoc;
    ACDInventoryLocationSet(tInvLoc, tACDContainer.id, eSlot, pt.x, pt.y);


    if (!sACDInventorySlotIsEmpty(tInvLoc))
    {
        ACDID tItemInSlot = ACDInventoryGetItemInSlot(tInvLoc);
        ActorCommonData * ptItemACD = ACDTryToGet(tItemInSlot);
        Actor * ptActorInSlot = ActorGet((ActorID)ptItemACD->idOwner);


        Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);


        if (!bRemoveExistingItems)
            return FALSE;
        ACDInventoryDelete(*ptItemACD, TRUE);
    }













    MoveItemToInventory(tACDItem, tNewLocation.tInvLocation, bMarkAsDirty, bOnHeroLoad);


    return TRUE;
}




































































typedef int32 ItemUseResult;  // X/Server/Powers/Power_ItemUse.h
ItemUseResult ItemUse(const PowerID idPower, const ACDID idACDUser, const ACDID idACDItemToUse, const Target * ptTarget);  // Power_ItemUse.cpp:1836
BOOL GizmoDoVacuumPickupAttractorFX(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, const SNO snoAttractorEffectGroup);  // SGizmo.cpp:1322
void SQuestsOnPlayerAcquireItem(ACDID idACDPlayer, ACDID idACDItem);  // SQuests.cpp:4685
void ItemSetPlayerRecipient(ActorCommonData & tACDItem, int32 nPlayerIndex);  // Items.cpp:5621
void PlayerRemoveOnGroundCraftingPlan(Player & tPlayer, GBID gbidPlan);  // Player.cpp:5577
PickupResponse sHandleCurrencyPickup(ActorCommonData & tACDItem, ActorCommonData & tACDPickee, BOOL bPlayEffects, BOOL bPickedUpViaCompanion);  // ActorInventory.cpp:4851







static void sOnAcquiredItem(ActorCommonData & tACDPickee, ActorCommonData & tACDItem)


{
    SQuestsOnPlayerAcquireItem(tACDPickee.id, tACDItem.id);

    Player * ptPlayer = PlayerGetByACD(tACDPickee.id);
    if (ptPlayer)
    {
        SACDInventoryEvaluateAcquireItemAchievementEvent(*ptPlayer, tACDItem);

        if (ItemHasLabel(tACDItem.hGB.gbid, ITL_CRAFTING_PLAN))
        {
            PlayerRemoveOnGroundCraftingPlan(*ptPlayer, tACDItem.hGB.gbid);
        }



        ItemQualityLevel eIQL = (ItemQualityLevel)tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL);
        if (eIQL >= IQL_RARE_AFFIX_START)
        {
            SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_RareItem, PLAYER_SAVE_MODE_WITHIN_5_SECONDS);
        }
    }


}







static void sSendCurrencyFloatingAmount(Player * ptPlayer, const WorldPlace & wplace, const int32 nAmount, const CurrencyType eCurrencyType)
{




    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);

    if (eCurrencyType >= CURRENCY_CRAFTING_MAT_START)
    {
        FloatingGBIDAmountMessage tMessage;
        tMessage.wpPlace = wplace;
        tMessage.nAmount = nAmount;
        tMessage.gbidItem = CurrencyGetGBID(eCurrencyType);
        MessageSendToClient(listDestPlayers, SMSG_FLOATING_GBID_AMOUNT, &tMessage, sizeof(tMessage));
    }
    else
    {
        FloatingAmountMessage tMessage;
        tMessage.wpPlace = wplace;
        tMessage.nAmount = nAmount;
        tMessage.eType = FloatingNumberTypeFromCurrency(eCurrencyType);
        MessageSendToClient(listDestPlayers, SMSG_FLOATING_AMOUNT, &tMessage, sizeof(tMessage));
    }
}



int64 sPlayerCurrencyPickup(ActorCommonData & tACDItem, ActorCommonData & tACDPickee, Player * ptCollectingPlayer, CurrencyType eCurrencyType, int64 nAmount, int64 nCurrencyCap, BOOL bPlayEffects, BOOL)
{










    int64 nCurrentAmount = ItemPlayerGetCurrencyCount(tACDPickee, eCurrencyType);
    int64 nAmountToAdd = nAmount;
    int64 nRemainder = 0;

    if (nCurrencyCap > 0 &&
        nCurrentAmount + nAmount > nCurrencyCap)
    {
        if (nCurrentAmount >= nCurrencyCap)
            return nAmount;

        nRemainder = nCurrentAmount + nAmount - nCurrencyCap;
        nAmountToAdd = nAmount - nRemainder;
    }






    if (nAmountToAdd > 0)
    {
        if (eCurrencyType == CURRENCY_PLATINUM &&

            GameGetDailyPlatinumLimit() != 0)
        {
            int64 nOriginalAmount = tACDPickee.GetCurrencyAmount(CURRENCY_PLATINUM);

            PlatinumAwardedMessage tMessage;
            tMessage.uCurrentPlatinum = nOriginalAmount;
            tMessage.uPlatinumIncrement = nAmountToAdd;

            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptCollectingPlayer);
            MessageSendToClient(listDestPlayers, SMSG_PLATINUM_AWARDED, &tMessage, sizeof(tMessage));
        }


        tACDPickee.ModifyCurrencyAmount(nAmountToAdd, eCurrencyType, GMR_PICKUP, FALSE);
        SGameStatsPlayerCounterIncrement(ptCollectingPlayer, STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP, eCurrencyType, 0, (float)nAmountToAdd);

        if (ptCollectingPlayer)
        {
            WorldPlace wplace;
            if (tACDItem.IsInWorld())
            {
                wplace = tACDItem.GetWorldPlaceCenter();
            }
            else
            {
                wplace = tACDPickee.GetWorldPlaceCenter();
            }


            sSendCurrencyFloatingAmount(ptCollectingPlayer, wplace, (int32)nAmountToAdd, eCurrencyType);


            if (tACDItem.tLocation.eActorLocation == AL_WORLD)
            {
                SGameStatsPlayerCounterIncrement(ptCollectingPlayer, STAT_COUNTER_PLAYER_CURRENCY_PICKED_UP_FROM_GROUND, eCurrencyType, 0, (float)nAmountToAdd);





                SPlayerRecordCurrencyPickedUp(tACDPickee.id, nAmountToAdd, eCurrencyType);


                if (eCurrencyType == CURRENCY_GOLD)
                {
                    if (bPlayEffects && tACDPickee.IsAlive())
                    {
                        ACDPlayEffectForPlayer(tACDPickee.id, tACDPickee.id, EFFECT_PICKUP_GOLD, 0);
                    }





                    SPlayerIncreaseCurrencyCounter(*ptCollectingPlayer, CURRENCY_GOLD, (uint32)nAmountToAdd);


                    ptCollectingPlayer->tConsoleCounters.uGoldPickedUp += (uint32)nAmountToAdd;



                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetPlayer(ptCollectingPlayer);

                    tEvaluationData.EvaluateEvent(ptCollectingPlayer, Achievement::ACH_EVENT_PICKUP_GOLD, (uint32)nAmount);
                }
















                else if (eCurrencyType == CURRENCY_PLATINUM &&

                         GameGetDailyPlatinumLimit() != 0)
                {
                    SPlayerIncreaseCurrencyCounter(*ptCollectingPlayer, CURRENCY_PLATINUM, (uint32)nAmountToAdd);
                }
            }
        }
    }

    return nRemainder;
}




PickupResponse sHandleCurrencyPickup(ActorCommonData & tACDItem, ActorCommonData & tACDPickee, BOOL bPlayEffects, BOOL bPickedUpViaCompanion)




{
    CurrencyType eCurrencyType = CurrencyFromGBID(tACDItem.hGB.gbid);


    ScopedItemDefinitionLockR pdefCurrency(tACDItem.hGB.gbid);


    int64 nCurrencyCap = ItemGetMaxStackSize(tACDItem.hGB, &tACDPickee, NULL);
    int64 nAmountFromItem = tACDItem.GetStackCount();
    int64 nRemainder = 0;


    if (eCurrencyType == CURRENCY_GOLD)
    {
        int64 nInherentGoldOnItem = tACDItem.AttributesGetInt(ATTRIBUTE_GOLD);
        if (nInherentGoldOnItem > 0)
        {
            nAmountFromItem = Math::Max(nAmountFromItem, nInherentGoldOnItem);
        }
    }



    Player * ptCollectingPlayer = ACDGetControllingPlayer(tACDPickee.id);

    if (!tACDPickee.IsMerchant())
    {
        GameConnectionID idPrimary = GAMECONNECTIONID_INVALID;




        if (ptCollectingPlayer)
        {
            int64 nCurrentAmount = ItemPlayerGetCurrencyCount(tACDPickee, eCurrencyType);

            if (nCurrencyCap > 0 &&
                nCurrentAmount + nAmountFromItem > nCurrencyCap)
            {
                return PICKUP_RESPONSE_NO_SUITABLE_SLOT_FOUND;
            }

            idPrimary = ptCollectingPlayer->idGameConnection;
        }


        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (idPrimary == GAMECONNECTIONID_INVALID || idPrimary != ptPlayer->idGameConnection)
            {
                continue;
            }

            if (eCurrencyType >= CURRENCY_CRAFTING_MAT_START && ptPlayer->idPlayerACD != tACDPickee.id)
            {
                continue;
            }

            ActorCommonData & tACDPlayerPickee = ACDGet(ptPlayer->idPlayerACD);
            int64 nPersonalCurrencyCap = ItemGetMaxStackSize(tACDItem.hGB, &tACDPlayerPickee, NULL);


            nRemainder = sPlayerCurrencyPickup(tACDItem, tACDPlayerPickee, ptPlayer, eCurrencyType, nAmountFromItem, nPersonalCurrencyCap, bPlayEffects, bPickedUpViaCompanion);
        }
    }






    {
        ActorID idActorItem = (ActorID)ACDGetOwner(tACDItem.id);

        ActorDelete(idActorItem);
    }
































    return PICKUP_RESPONSE_OK;
}






static void sOnItemCountChanged(Player * ptPlayer, ActorCommonData & tACDPickee, ActorCommonData & tACDItem, uint64 uCount, BOOL bPickedUpFromGround)





{

    if (ItemIsGem(tACDItem.hGB.gbid) && bPickedUpFromGround)
    {
        WorldPlace wplace;
        if (tACDItem.IsInWorld())
        {
            wplace = tACDItem.GetWorldPlaceCenter();
        }
        else
        {
            wplace = tACDPickee.GetWorldPlaceCenter();
        }
        FloatingGBIDAmountMessage tMessage;

        tMessage.wpPlace = wplace;
        tMessage.nAmount = (int32)uCount;
        tMessage.gbidItem = tACDItem.hGB.gbid;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_FLOATING_GBID_AMOUNT, &tMessage, sizeof(tMessage));
    }
}



PickupResponse SACDInventoryPickup(const ACDID idACDPickee, const ACDID idACDItem, const EffectType ePickupEffect, const BOOL fAddToDirtyItemList, const BOOL bTryStashFirst, ACDID * idACDStackDest, BOOL bPlayEffects, BOOL bTryStashOnly, BOOL bPickedUpViaCompanion, BOOL bAllowAutoEquip)










{














    ActorCommonData & tACDItem = ACDGet(idACDItem);
    ActorCommonData & tACDPickee = ACDGet(idACDPickee);

    Player * ptPlayer = PlayerGetByACD(tACDPickee.id);
    if (ptPlayer &&

        !tACDItem.DoesPlayerMatchAssignedHeroID(ptPlayer))
    {
        return PICKUP_RESPONSE_OWNED_BY_OTHER;
    }



    BOOL bPickedUpFromGround = tACDItem.tLocation.eActorLocation == AL_WORLD;

    if (!CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT) &&

        ACDIsParticipatingInTieredLootRun(tACDPickee))
    {
        bAllowAutoEquip = FALSE;
    }


    PickupResult tResult;
    PickupResponse ePickupResponse = ACDInventoryCanPickup(tACDPickee, tACDItem, bTryStashFirst, &tResult, bTryStashOnly, bAllowAutoEquip);
    if (ePickupResponse != PICKUP_RESPONSE_OK)
    {
        return ePickupResponse;
    }


    if (ptPlayer && ptPlayer->bIsBeingDeleted && tResult.ePickupAction != PICKUP_ACTION_QUEST)
    {
        return PICKUP_RESPONSE_INVALID;
    }

    if (ePickupEffect != EFFECT_INVALID)
    {
        ACDPlayEffectForPlayer(tACDPickee.id, tACDPickee.id, ePickupEffect, 0);

        if (ePickupEffect == EFFECT_BLOOD_SHARD_ON_PICKUP)
        {
            GizmoDoVacuumPickupAttractorFX(tACDItem, tACDPickee, GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_PICKUP_BLOODSHARD_ATTRACTOR));
        }
    }

    if (ItemGetBindingLevel(tACDItem) == IBL_ACCOUNT_PICKUP &&
        ItemGetBoundToACD(tACDItem, FALSE) == ACDID_INVALID)
    {
        Player * ptOwner = tACDPickee.GetPetOwnerPlayer();
        ACDID idACDUltimateOwner = ptOwner ? ptOwner->idPlayerACD : tACDPickee.id;
        ItemSetBoundToACD(tACDItem, idACDUltimateOwner);
    }



    if (!ItemIsCurrency(tACDItem))
    {

        tACDItem.ClearAssignedHero();
    }


    ItemSetPlayerRecipient(tACDItem, -1);


    PowerAPI_BuffApplyOnItemPickUpEffects(tACDPickee.id, tACDItem.id, tResult.ePickupAction);


    if (tACDItem.id == ACDID_INVALID)
    {
        return PICKUP_RESPONSE_INVALID;
    }


    tACDItem.AttributesSetInt(ATTRIBUTE_PICKED_UP_TIME, GameGetServerTime());


    PingID idPing = tACDItem.AttributesGetInt(ATTRIBUTE_PING);
    Minimap::RemovePing(idPing);
    tACDItem.AttributesSetInt(ATTRIBUTE_PING, -1);


    switch (tResult.ePickupAction)
    {
    case PICKUP_ACTION_GLYPH:
    {
        PowerStartUseItem(tACDPickee.id, tACDItem.id, NULL);


        Player * ptPlayerPickee = PlayerGetByACD(tACDPickee.id);
        if (ptPlayerPickee)
            SACDInventoryEvaluateAcquireItemAchievementEvent(*ptPlayerPickee, tACDItem);
        break;
    }




    case PICKUP_ACTION_USE_ON_PICKUP:
    {
        ItemUse(POWERID_INVALID, tACDPickee.id, tACDItem.id, NULL);
        Player * ptPlayerPickee = PlayerGetByACD(tACDPickee.id);
        if (ptPlayerPickee)
            SACDInventoryEvaluateAcquireItemAchievementEvent(*ptPlayerPickee, tACDItem);
        break;
    }






    case PICKUP_ACTION_QUEST:
    {
        SQuestsOnPlayerAcquireItem(tACDPickee.id, tACDItem.id);


        sOnPlayerPickedUpItem(tACDPickee.id, tACDItem);
        sOnAcquiredItem(tACDPickee, tACDItem);
        sOnItemCountChanged(ptPlayer, tACDPickee, tACDItem, tACDItem.GetStackCount(), bPickedUpFromGround);


        ActorID idActorItem = (ActorID)ACDGetOwner(tACDItem.id);

        ActorDelete(idActorItem);
        break;
    }


    case PICKUP_ACTION_MOVE_TO_LOCATION:
    {
        tACDItem.ModifiedMark(ACDMODIFIED_INVENTORY_ACTOR_SNO);
        MoveItemToInventory(tACDItem, tResult.tNewLocation, fAddToDirtyItemList, FALSE);
        ACDInventoryEquipmentOrSkillChanged(tACDPickee);
        sOnAcquiredItem(tACDPickee, tACDItem);
        sOnItemCountChanged(ptPlayer, tACDPickee, tACDItem, tACDItem.GetStackCount(), bPickedUpFromGround);
        break;
    }


    case PICKUP_ACTION_STACK:
    {
        ActorCommonData & tACDContainer = ACDGet(tResult.tNewLocation.idACDContainer);
        ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDContainer, tResult.tNewLocation.eSlot, tResult.tNewLocation.pt);
        ActorCommonData & tACDDestItem = ACDGet(idACDSquareItem);
        uint64 uStackCountBefore = tACDDestItem.GetStackCount();

        if (!sStackItems(tACDItem, tACDDestItem, tACDItem.GetStackCount(), fAddToDirtyItemList))
        {
            return PICKUP_RESPONSE_NO_SUITABLE_SLOT_FOUND;
        }

        uint64 uStackCountDelta = tACDDestItem.GetStackCount() - uStackCountBefore;
        if (uStackCountDelta != 0)
        {
            sOnItemCountChanged(ptPlayer, tACDPickee, tACDDestItem, uStackCountDelta, bPickedUpFromGround);
        }


        if (tACDItem.id != ACDID_INVALID)
        {
            sOnAcquiredItem(tACDPickee, tACDItem);
        }

        if (idACDStackDest)
        {
            *idACDStackDest = tACDDestItem.id;
        }
        break;
    }


    case PICKUP_ACTION_CURRENCY:
    {
        return sHandleCurrencyPickup(tACDItem, tACDPickee, bPlayEffects, bPickedUpViaCompanion);
    }
    default:

        sOnAcquiredItem(tACDPickee, tACDItem);
        break;
    }
    return PICKUP_RESPONSE_OK;
}



BOOL SACDInventoryPickupOrSpillOntoGround(ActorCommonData & tACDPlayer,
                                          ActorCommonData & tACDItem,
                                          const BOOL bTryStashFirst,
                                          ACDID * pidACDStackDest,
                                          const BOOL bAllowAutoEquip)
{










    PickupResponse eResponse = SACDInventoryPickup(tACDPlayer.id, tACDItem.id, EFFECT_PLAY_PICKUP_SOUND, TRUE, bTryStashFirst, pidACDStackDest, FALSE, FALSE, FALSE, bAllowAutoEquip);
    if (eResponse != PICKUP_RESPONSE_OK)
    {





        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
        if (ptPlayer && ptPlayer->bIsBeingDeleted)
        {

            ActorFlagForDeletion((ActorID)ACDGetOwner(tACDItem.id));
            return FALSE;
        }

        WorldPlace tPlace;
        if (FlippyFindLandingLocation(tACDPlayer, tACDItem.snoActor, tPlace, ptPlayer, NULL, ACDID_INVALID))
        {
            if (ptPlayer)
            {
                tACDItem.SetAssignedHeroID(ptPlayer);
            }
            else
            {
                tACDItem.ClearAssignedHero();
            }

            FlippyDropCreateOnActor(tACDPlayer.id, tACDItem.id, tPlace);
        }
        else
        {




            ActorFlagForDeletion((ActorID)ACDGetOwner(tACDItem.id));
            return FALSE;
        }
    }


    return TRUE;
}




void ACDInventoryGetActiveWeaponSlots(ACDID idACD, InventorySlotType * peSlotPrimary, InventorySlotType * peSlotSecondary)
{
    *peSlotPrimary = SLOT_NONE;
    *peSlotSecondary = SLOT_NONE;












    ActorCommonData * ptACD = ACDTryToGet(idACD);






    if (!ptACD || !ptACD->ptInventory)
        return;



    switch (ptACD->ptInventory->eType)
    {
    case INVENTORY_PLAYER:
        *peSlotPrimary = SLOT_PLAYER_LEFT_HAND;
        *peSlotSecondary = SLOT_PLAYER_RIGHT_HAND;
        break;
    case INVENTORY_HIRELING:
    case INVENTORY_GENERIC:







        HirelingGetClass(*ptACD);
        *peSlotPrimary = SLOT_HIRELING_LEFT_HAND;
        *peSlotSecondary = SLOT_HIRELING_RIGHT_HAND;
        break;
    }
}

















ACDID ACDInventoryGetPrimaryWeapon(ACDID idACD)
{
    InventorySlotType eSlot;
    InventorySlotType eSlotSecondary;

    ACDInventoryGetActiveWeaponSlots(idACD, &eSlot, &eSlotSecondary);
    if (eSlot == SLOT_NONE)
        return ACDID_INVALID;




    ActorCommonData & tACD = ACDGet(idACD);
    if (!ACDInventoryValidateSlotForACD(tACD, eSlot))
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    return ACDInventoryGetSquareItem(tACD, eSlot, cgptNull);
}



ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD)
{
    InventorySlotType eSlotPrimary;
    InventorySlotType eSlot;

    ACDInventoryGetActiveWeaponSlots(idACD, &eSlotPrimary, &eSlot);
    if (eSlot == SLOT_NONE)
        return ACDID_INVALID;




    ActorCommonData & tACD = ACDGet(idACD);
    if (!ACDInventoryValidateSlotForACD(tACD, eSlot))
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    return ACDInventoryGetSquareItem(tACD, eSlot, cgptNull);
}



ACDID ACDInventoryGetActiveWeapon(const ActorCommonData & tACD)
{

    ACDID idACDActiveWeapon = ACDInventoryGetPrimaryWeapon(tACD.id);

    if (ACDIsDualWielding(tACD, TRUE))
    {
        DualWieldAttackType eAttackType = tACD.AttributesGetInt(ATTRIBUTE_DUALWIELD_HAND);

        if (eAttackType == DUALWIELD_ATTACK_OFFHAND)
        {
            idACDActiveWeapon = ACDInventoryGetSecondaryWeaponOrShield(tACD.id);
        }
    }



    else if (ACDCanDualWield(tACD) && idACDActiveWeapon == ACDID_INVALID)
    {


        ACDID idACDOffhandWeapon = ACDInventoryGetSecondaryWeaponOrShield(tACD.id);
        if (idACDOffhandWeapon != ACDID_INVALID)
        {
            ActorCommonData * ptACDOffhandWeapon = ACDTryToGet(idACDOffhandWeapon);
            if (ptACDOffhandWeapon && ItemHasLabel(ptACDOffhandWeapon->hGB.gbid, ITL_WEAPON))
                idACDActiveWeapon = idACDOffhandWeapon;
        }
    }





    return idACDActiveWeapon;
}





































SNO ACDInventoryGetPrimaryWeaponSNO(ACDID idACD)
{
    ACDID idACDItem = ACDInventoryGetPrimaryWeapon(idACD);
    if (idACDItem != ACDID_INVALID)
        idACD = idACDItem;
    ActorCommonData & tACD = ACDGet(idACD);
    return tACD.snoActor;
}





























void ACDInventorySaveItem(
    const ActorCommonData & tItemACD,
    D3::Items::SavedItem & tSavedItem,
    HirelingClass eHirelingClass,
    InventorySlotType eHirelingSlot)
{

    tSavedItem.Clear();

    const InventoryLocation & tInvLoc = tItemACD.tLocation.tInvLocation;


    IVector2D tSlotSize;
    ACDInventorySlotGetSize(tInvLoc.eSlot, tSlotSize);


    if (GameActiveGameCommonDataIsClient())
    {
        ItemComputeConsoleItemId(tItemACD, *tSavedItem.mutable_id());
    }
    else
    {

        const Actor * ptActor = ActorGet((ActorID)tItemACD.idOwner);
        ptActor->tItemId.Export(*tSavedItem.mutable_id());
        ptActor->tItemInventoryId.Export(*tSavedItem.mutable_owner_entity_id());
    }




    tSavedItem.set_used_socket_count(0);

    if (tItemACD.tLocation.eActorLocation == AL_INVENTORY && tInvLoc.eSlot != SLOT_NONE)
    {
        if (tInvLoc.eSlot == SLOT_ITEM_SOCKETS)
        {



            if (GameActiveGameCommonDataIsClient())
            {
                const ActorCommonData & tParentItemACD = ACDGet(tInvLoc.idACDContainer);
                ItemComputeConsoleItemId(tParentItemACD, *tSavedItem.mutable_socket_id());
            }
            else
            {



                const ActorCommonData & tParentItemACD = ACDGet(tInvLoc.idACDContainer);
                const Actor * ptParentActor = ActorGet((ActorID)tParentItemACD.idOwner);
                ptParentActor->tItemId.Export(*tSavedItem.mutable_socket_id());
            }
        }
        else if (eHirelingClass == HC_NONE)
        {

            ACDID idRootContainer = ACDInventoryGetUltimateContainer(tItemACD);
            ActorCommonData & tACDRootContainer = ACDGet(idRootContainer);
            if (!tACDRootContainer.IsItem() && !ACDIsPlayer(idRootContainer))
            {
                eHirelingClass = HirelingGetClass(tACDRootContainer);
                if (eHirelingClass == HC_NONE)
                {
                    return;
                }
            }
        }
    }






    tSavedItem.set_hireling_class(eHirelingClass);

    SavedInventorySlotType eSavedSlot = ActorInventoryTranslateToSavedSlot(eHirelingClass != HC_NONE ? eHirelingSlot : tInvLoc.eSlot);
    tSavedItem.set_item_slot(eSavedSlot);

    tSavedItem.set_square_index(tInvLoc.pt.y * tSlotSize.x + tInvLoc.pt.x);


    if (tItemACD.ptFriendGiftData)
    {
        uint64 nGiftRecipientID = tItemACD.ptFriendGiftData->idRecipient;
        if (nGiftRecipientID != 0)
        {
            D3::Items::FriendGiftInfo & tFriendGiftInfo = *tSavedItem.mutable_friend_gift_info();
            tFriendGiftInfo.set_id_friend_recipient(nGiftRecipientID);
            CRefString strRecipientName = tItemACD.ptFriendGiftData->sNameRecipient;
            tFriendGiftInfo.set_recipient_name(strRecipientName.str());
        }
    }



    D3::Items::Generator & tGenerator = *tSavedItem.mutable_generator();
    ItemGeneratorPopulateFromACDID(tItemACD, tGenerator, TRUE);






}



/* :5508 */  int32 sACDInventorySavedStateStoreRecursive(const ActorCommonData & tACD, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems, uint32 uSlotsToSaveBits, bool bDirtyOnly);  /* :5725 */

static void sSaveItem(
    ACDID idACDItem,
    InventorySlotType eSlot,
    int32 nInventorySquareIndex,
    HirelingClass eClass,
    google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems,
    bool bDirtyOnly,
    bool bSaveSocketedItems,
    int32 & nNumAttachments)
{









    D3::Items::SavedItem & tSavedItem = *tSavedItems.Add();
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);

    ACDInventorySaveItem(*ptACDItem, tSavedItem, nInventorySquareIndex, eSlot);

    nNumAttachments++;

    uint32 usedSocketCount = ptACDItem->AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED);
    tSavedItem.set_used_socket_count(usedSocketCount);


    if (bSaveSocketedItems && ptACDItem->HasInventory())
    {
        sACDInventorySavedStateStoreRecursive(*ptACDItem, tSavedItems, 0xffffffff, bDirtyOnly);
    }
}






















static void sDeleteItem(ACDID idACDSquare, InventorySlotType, void *)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem)
    {
        uint64 uStackSize = ptACDItem->GetStackCount();
        SACDInventoryDeleteItem(*ptACDItem, uStackSize, TRUE, FALSE);
    }
}











void ACDInventoryDeleteAll(ActorCommonData & tACD)
{
    if (tACD.HasInventory())
        ACDInventoryIterateItems(tACD, SLOT_NONE, 0, sDeleteItem, NULL);
}









void ACDInventoryDeleteSharedStash(ActorCommonData & tACD)
{
    if (tACD.HasInventory())
        ACDInventoryIterateItems(tACD, SLOT_PLAYER_SHARED_STASH, 0, sDeleteItem, NULL);
}









int32 sACDInventorySavedStateStoreRecursive(
    const ActorCommonData & tACD,
    google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems,
    uint32 uSlotsToSaveBits,
    bool bDirtyOnly)
{
    IVector2D tSizeSlot;
    int32 nNumAttachments = 0;

    if (!tACD.HasInventory())
        return nNumAttachments;



    int32 nEnd;
    int32 nStart;
    int32 nSlots;
    sGetInventorySlotInfo(tACD.ptInventory, nStart, nEnd, nSlots);

    for (int32 nSlot = nStart; nSlot < nEnd; nSlot++)
    {

        if (nSlot == SLOT_PLAYER_BUYBACK || nSlot == SLOT_PLAYER_BASE_HEALTHPOTION)
            continue;




        if (!(uSlotsToSaveBits & (1 << nSlot)))
            continue;



        InventorySlot * ptSlot = NULL;
        tACD.ptInventory->mapInventorySlots.Lookup(nSlot, ptSlot);






        for (uint32 nSquareIndex = 0; nSquareIndex < ptSlot->arSquares.size(); nSquareIndex++)
        {
            InventorySquare & tSquare = ptSlot->arSquares[nSquareIndex];


            if (tSquare.idACD != ACDID_INVALID)
            {

                ActorCommonData * ptACDItem = ACDTryToGet(tSquare.idACD);
                if (!ptACDItem)
                {
                    Player * ptPlayerContainer = PlayerGetByACD(tACD.id);
























                    continue;
                }
                bool bSaveSocketedItems = (uSlotsToSaveBits & (1 << SLOT_ITEM_SOCKETS)) != 0;
                sSaveItem(tSquare.idACD, nSlot, 0, HC_NONE, tSavedItems, bDirtyOnly, bSaveSocketedItems,
                          nNumAttachments);
            }
        }
    }









    return nNumAttachments;
}

int32 sACDInventorySavedStateStoreRecursive(const ActorCommonData & tACD, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems, uint32 uSlotsToSaveBits, bool bDirtyOnly);  // line 5725
void ACDInventorySavedStateStore(const ActorCommonData & tACD, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems, uint32 uSlotsToSaveBits, bool bDirtyOnly)
{








    if (!tACD.HasInventory())
        return;



    sACDInventorySavedStateStoreRecursive(tACD, tSavedItems, uSlotsToSaveBits, bDirtyOnly);





}




void sHirelingSavedStateStoreRecursive(
    const HirelingStable & tStable,
    google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems,
    bool bDirtyOnly,
    bool bSaveSocketedItems)
{
    for (int32 nHireling = 0; nHireling < 4; nHireling++)
    {
        const HirelingItemStore * ptItems = &tStable.tHireling[nHireling].tItems;


        for (int32 nSlot = SLOT_HIRELING_START; nSlot < SLOT_HIRELING_END; nSlot++)
        {
            ACDID idACDItem = ptItems->idACDItems[nSlot - SLOT_HIRELING_START];
            if (idACDItem != ACDID_INVALID)
            {

























                int32 nNumAttachments = 0;
                sSaveItem(idACDItem, nSlot, nHireling, nHireling, tSavedItems, bDirtyOnly, bSaveSocketedItems, nNumAttachments);
            }
        }
    }








}


void sHirelingSavedStateStoreRecursive(const HirelingStable & tStable, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems, bool bDirtyOnly, bool bSaveSocketedItems);  // line 5849
void HirelingCheatItemsForActiveHirelingAndProxies(Player & tPlayer, BOOL bCheat);  // X/Common/Hireling.cpp (parameter name invented)
void ACDHirelingInventorySavedStateStore(Player & tPlayer, google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems, bool bDirtyOnly, bool bSaveSocketedItems)
{





    HirelingCheatItemsForActiveHirelingAndProxies(tPlayer, TRUE);

    sHirelingSavedStateStoreRecursive(tPlayer.tHirelingStable, tSavedItems, bDirtyOnly, bSaveSocketedItems);


    HirelingCheatItemsForActiveHirelingAndProxies(tPlayer, FALSE);
}






static BOOL sIgnoreDeletedGBID(
    GBID gbidItem)
{

    return OnlineService::IsPTR() && gbidItem == (GBID)0xa5bbf08e;
}
















































































































BOOL SACDInventoryRestoreItemToLocation(ActorCommonData & tACDRestoreTo, ActorCommonData & tACDItem, ActorLocation & tNewLocation); // ActorInventory.cpp:6672 (raw DWARF has no recovered header declaration).
BOOL ActorInventorySocketItem(ActorCommonData & tACDItemToSocket, ActorCommonData & tACDItemSocketedInto, const IVector2D & tPoint, BOOL bMarkItemAsDirty); // ActorInventory.cpp:7825 (raw DWARF has no recovered header declaration).


static ACDID sRestoreItem(
    ActorCommonData & tACDRestoreTo,
    ACDID idACDPlayer,
    const D3::Items::SavedItem & tSavedItem,
    BOOL bResocketItem)
{
    InventorySlotType eSlot = ActorInventoryTranslateFromSavedSlot(tSavedItem.item_slot());

    IVector2D tSizeSlot;
    ACDInventorySlotGetSize(eSlot, tSizeSlot);

    D3::Items::Generator tGenerator(tSavedItem.generator());

    ItemUpdateLegacyItem(tGenerator);
    ItemCapJewelRank(tGenerator);


    OnlineService::ItemId tItemId(tSavedItem.id());
    ACDID idACDItem = SItemGenerate(tItemId, tGenerator, cgplaceNull, 0.0f, 0);
    if (idACDItem == ACDID_INVALID)
        return ACDID_INVALID;




    if (GameIsSeasonal() &&

        !GameIsUtility() &&
        tSavedItem.generator().season_created() != GameGetCurrentSeason())
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    }






    ActorCommonData & tACDItem = ACDGet(idACDItem);
    Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);


    ptActor->tItemInventoryId.Import(tSavedItem.owner_entity_id());
    ptActor->dwActorFlags |= (1 << ACTOR_DB_PERSISTED);

    if (tGenerator.flags() & (1 << IGF_CHEATED))
        ptActor->dwActorFlags |= (1 << ACTOR_CHEATED);

    if (tSavedItem.hireling_class() == HC_NONE)
    {

        ActorLocation tLocation;
        tLocation.eActorLocation = AL_INVENTORY;
        tLocation.tInvLocation.eSlot = eSlot;
        tLocation.tInvLocation.pt.x = tSavedItem.square_index() % tSizeSlot.x;
        tLocation.tInvLocation.pt.y = tSavedItem.square_index() / tSizeSlot.x;
        tLocation.tInvLocation.idACDContainer = tACDRestoreTo.id;

        BOOL bRestored = FALSE;
        BOOL bFailedDueToStashSpace = FALSE;
        BOOL bFailedDueToSlotOccupied = FALSE;

        if (bResocketItem)
        {
            bRestored = ActorInventorySocketItem(tACDItem, tACDRestoreTo, tLocation.tInvLocation.pt, FALSE);
        }
        else
        {
            if (eSlot == SLOT_PLAYER_SHARED_STASH &&

                !sDoesItemFitAtThisLocation(tACDItem, tLocation.tInvLocation))
            {
                bFailedDueToStashSpace = TRUE;
            }
            else
            {



                bRestored = SACDInventoryRestoreItemToLocation(tACDRestoreTo, tACDItem, tLocation);
            }

            if (!bRestored)
            {
                bFailedDueToSlotOccupied = ACDInventoryValidateSlotForACD(tACDRestoreTo, eSlot) && !sACDInventorySlotIsEmpty(tLocation.tInvLocation);
            }
        }

        if (!bRestored)
        {
            if (bFailedDueToSlotOccupied || bFailedDueToStashSpace)
            {



                SACDInventoryDeleteItem(tACDItem, tACDItem.GetStackCount(), TRUE, TRUE);

                if (GameIsBattleNetServer())
                {

                    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
                    DirtyInventoryRecordMap * ptItemMoveLog = ptActorInventoryGlobals->ptItemMoveLog;

                    DirtyInventoryRecordMap::iterator it = ptItemMoveLog->find(ptActor->tItemId.GetLow());
                    if (it != ptItemMoveLog->end())
                        it->second.eAction = AIDA_REMOVE;
                    else
                    {

                        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
                        if (ptPlayer)
                        {
                            ActorInventoryDatabaseRecord tmp;
                            tmp.uActionMask = AIDR_NONE;
                            tmp.firstAccountId = ptPlayer->tAccount.tHandle.m_id;
                            tmp.bNewItem = false;
                            tmp.eAction = AIDA_REMOVE;
                            (*ptItemMoveLog)[ptActor->tItemId.GetLow()] = tmp;
                        }
                    }
                }






                if (!bFailedDueToStashSpace)
                {
                    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
                }
                return ACDID_INVALID;
            }
            else
            {










                ACDID idACDOwner = ACDInventoryGetUltimateContainer(tACDRestoreTo);
                if (idACDOwner == ACDID_INVALID)
                {
                    idACDOwner = tACDRestoreTo.id;
                }



                if (ItemIsGold(tACDItem))
                {
                    ActorCommonData & tACDOwner = ACDGet(idACDOwner);


                    uint64 uGoldToPickup = tACDItem.GetStackCount();
                    tACDOwner.ModifyCurrencyAmount(uGoldToPickup, CURRENCY_GOLD, GMR_UNKNOWN, TRUE);



                    ActorID idActor;
                    SACDInventoryAddToDirtyList(tACDItem, AIDA_REMOVE, tACDItem.GetLocationSlot(), SLOT_NONE);
                    ActorDelete((ActorID)tACDItem.idOwner);
                    return ACDID_INVALID;
                }
                else
                {

                    if (SACDInventoryPickup(idACDOwner, tACDItem.id, EFFECT_INVALID, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE) != PICKUP_RESPONSE_OK)
                    {
                        SACDInventoryAddToDirtyList(tACDItem, AIDA_REMOVE, tACDItem.GetLocationSlot(), SLOT_NONE);
                        ActorDelete((ActorID)tACDItem.idOwner);
                        return ACDID_INVALID;
                    }
                }











                if (!ACDTryToGet(idACDItem))
                    return ACDID_INVALID;
            }
        }
    }
    else
    {



        if (!bResocketItem)
        {

            Player * ptPlayer = PlayerGetByACD(tACDRestoreTo.id);
            HirelingStable & tStable = ptPlayer->tHirelingStable;
            HirelingItemStore * ptStore = &tStable.tHireling[tSavedItem.hireling_class()].tItems;
            int32 nSlot = ActorInventoryTranslateFromSavedSlot(tSavedItem.item_slot());

            ptStore->idACDItems[nSlot - SLOT_HIRELING_START] = tACDItem.id;
        }
        else
        {
            ActorLocation tLocation;
            tLocation.eActorLocation = AL_INVENTORY;
            tLocation.tInvLocation.eSlot = ActorInventoryTranslateFromSavedSlot(tSavedItem.item_slot());
            tLocation.tInvLocation.pt.x = tSavedItem.square_index() % tSizeSlot.x;
            tLocation.tInvLocation.pt.y = tSavedItem.square_index() / tSizeSlot.x;
            tLocation.tInvLocation.idACDContainer = tACDRestoreTo.id;

            ActorInventorySocketItem(tACDItem, tACDRestoreTo, tLocation.tInvLocation.pt, FALSE);
        }
    }

    if (tGenerator.flags() & (1 << IGF_BOUND_BIT))
    {


        ItemSetBoundToACD(tACDItem, idACDPlayer);
    }

    if (ItemIsLockedFromTrade(tACDItem))
    {


        ItemSetLockedToACD(tACDItem, idACDPlayer);
    }

    if (tGenerator.flags() & (1 << IGF_MARK_DIRTY_ON_CREATE))
    {
        if (tSavedItem.hireling_class() != HC_NONE)
        {
            Player * ptPlayer = NULL;
            if (ACDIsPlayer(tACDRestoreTo.id))
            {
                ptPlayer = PlayerGetByACD(tACDRestoreTo.id);
            }
            else
            {
                ACDID idOwner = (ACDID)tACDRestoreTo.idOwner;

                while (idOwner != ACDID_INVALID && !ptPlayer)
                {
                    if (ACDIsPlayer(idOwner))
                    {
                        ptPlayer = PlayerGetByACD(idOwner);
                    }
                    else
                    {
                        idOwner = (ACDID)ACDGetOwner(idOwner);
                    }
                }
            }

            if (ptPlayer)
            {
                ptPlayer->bForceHirelingItemSave = true;
            }
        }
        else
        {








            SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_STACK, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());
        }
    }


    int nSockets = 0;
    if (tACDItem.ptInventory)
    {
        for (int i = 0; i < tGenerator.contents_size(); i++)
        {
            const D3::Items::EmbeddedGenerator & tEmbedded = tGenerator.contents(i);





            D3::Items::SavedItem tSavedGem;
            *tSavedGem.mutable_id() = tEmbedded.id();
            *tSavedGem.mutable_generator() = tEmbedded.generator();
            *tSavedGem.mutable_owner_entity_id() = tSavedItem.owner_entity_id();
            tSavedGem.set_hireling_class(tSavedItem.hireling_class());
            tSavedGem.set_item_slot(SAVED_SLOT_ITEM_SOCKETS);
            tSavedGem.set_square_index(i);

            if (sIgnoreDeletedGBID(tSavedGem.generator().gb_handle().gbid()))
                continue;



            ACDID idGem = sRestoreItem(tACDItem, idACDPlayer, tSavedGem, TRUE);
            if (idGem != ACDID_INVALID)
                nSockets++;




        }
    }
    tACDItem.AttributesSetInt(ATTRIBUTE_SOCKETS_FILLED, nSockets);


    if (tSavedItem.has_friend_gift_info())
    {
        const D3::Items::FriendGiftInfo & tFriendGiftInfo = tSavedItem.friend_gift_info();
        tACDItem.ptFriendGiftData = new FriendGiftData;
        tACDItem.ptFriendGiftData->sNameRecipient = tFriendGiftInfo.recipient_name().c_str();
        tACDItem.ptFriendGiftData->idRecipient = tFriendGiftInfo.id_friend_recipient();
    }


    return idACDItem;
}




static ACDID sCreateBottomlessHealthPotion()
{


    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);
    tGenerator.set_flags(tGenerator.flags() | (1 << IGF_IDENTIFIED_BIT));
    tGenerator.mutable_gb_handle()->set_gbid(GlobalGBIDGet(GLOBALGBID_ITEM_HEALTHPOTION_BOTTOMLESS));

    return SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);



}




void ACDInventoryCreateBaseHealthPotion(ActorCommonData & tACDPlayer)
{




    ACDID idPotion = sCreateBottomlessHealthPotion();
    if (idPotion != ACDID_INVALID)
    {
        ActorLocation tLocation;
        ActorCommonData * ptACDItem = ACDTryToGet(idPotion);



        tLocation.eActorLocation = AL_INVENTORY;
        tLocation.tInvLocation.eSlot = SLOT_PLAYER_BASE_HEALTHPOTION;

        tLocation.tInvLocation.pt.x = 0;
        tLocation.tInvLocation.pt.y = 0;
        tLocation.tInvLocation.idACDContainer = tACDPlayer.id;
        BOOL bSuccess = ACDInventoryAddToLocation(tACDPlayer, *ptACDItem, tLocation, TRUE, FALSE, TRUE, FALSE);
    }









}




void ACDInventorySavedStateRestore(
    ActorCommonData & tACDPlayer,
    const google::protobuf::RepeatedPtrField<D3::Items::SavedItem> & tSavedItems)
{





    for (int32 nItemIndex = 0; nItemIndex < tSavedItems.size(); nItemIndex++)
    {

        const D3::Items::SavedItem & tSavedItem = tSavedItems.Get(nItemIndex);
        if (ActorInventoryTranslateFromSavedSlot(tSavedItem.item_slot()) == SLOT_NONE)
            continue;

        if (tSavedItem.socket_id().id_high() == 0 || tSavedItem.socket_id().id_low() == 0)
        {

            if (sIgnoreDeletedGBID(tSavedItem.generator().gb_handle().gbid()))
                continue;



            ACDID idItem = sRestoreItem(tACDPlayer, tACDPlayer.id, tSavedItem, FALSE);
            GBID gbidItem = tSavedItem.generator().gb_handle().gbid();
            if (idItem != ACDID_INVALID)
            {
                if (ItemIsConsumablePotion(gbidItem))
                { removeObsoleteItem: // Label spelling inferred from the shared potion/dye deletion branch.
                    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
                    if (ptACDItem)
                    {
                        PlayerGetByACD(tACDPlayer.id);
                        SACDInventoryDeleteItem(*ptACDItem, ptACDItem->GetStackCount(), TRUE, FALSE);
                    }
                }






                else if (ItemIsCurrency(gbidItem))
                {

                    CurrencyType eCurrencyType = CurrencyFromGBID(gbidItem);
                    if (eCurrencyType != CURRENCY_INVALID)
                    {
                        ActorCommonData * ptACDItem = ACDTryToGet(idItem);
                        if (ptACDItem)
                        {
                            Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
                            uint64 nStackCount = ptACDItem->GetStackCount();





                            SACDInventoryDeleteItem(*ptACDItem, nStackCount, TRUE, FALSE);
                            tACDPlayer.ModifyCurrencyAmount(nStackCount, eCurrencyType, GMR_CURRENCY_CONVERSION, TRUE);
                        }
                    }
                }




                else if (ItemIsDye(gbidItem))
                {
                    goto removeObsoleteItem;






                }
            }
            else
            {




                CRefString sItemName = GBIDToString(gbidItem);

            }
        }
    }

}




















































































void ACDInventoryItemRestoreSocketsFromGenerator(
    ACDID idACDOwner,
    ActorCommonData & tACDItem,
    const D3::Items::Generator & tGenerator,
    XPooledDataIDList * plistACDs,
    int32 nNetworkNameBase)
{
    int nSockets = 0;
    for (int i = 0; i < tGenerator.contents_size(); i++)
    {
        const D3::Items::EmbeddedGenerator & tEmbedded = tGenerator.contents(i);


        D3::Items::SavedItem tSavedGem;
        *tSavedGem.mutable_id() = tEmbedded.id();
        *tSavedGem.mutable_generator() = tEmbedded.generator();
        tSavedGem.set_item_slot(SAVED_SLOT_ITEM_SOCKETS);
        tSavedGem.set_square_index(i);

        ACDID idGem;
        if (GameActiveGameCommonDataIsClient())
        {

            idGem = CItemGenerate((ACDNetworkName)(nNetworkNameBase + 10 + i), tSavedGem.generator(), NULL);
        }
        else if (!sIgnoreDeletedGBID(tSavedGem.generator().gb_handle().gbid()))
        {





            idGem = sRestoreItem(tACDItem, idACDOwner, tSavedGem, TRUE);
        }
        else
            continue;


        if (idGem != ACDID_INVALID)
        {

            nSockets++;

            if (plistACDs)
            {
                plistACDs->AddTail_Tracked(idGem);
            }
        }




    }
    tACDItem.AttributesSetInt(ATTRIBUTE_SOCKETS_FILLED, nSockets);
}




BOOL SACDInventoryRestoreItemToLocation(
    ActorCommonData & tACDRestoreTo,
    ActorCommonData & tACDItem,
    ActorLocation & tNewLocation)
{




    if (!ACDInventoryAddToLocation(tACDRestoreTo, tACDItem, tNewLocation, TRUE, FALSE, TRUE, FALSE))
        return FALSE;




    tACDItem.AttributesSetInt(ATTRIBUTE_KNOWN_BY_OWNER, tACDRestoreTo.id);
    return TRUE;
}





static void sUpdateItemACD(ActorCommonData & tACDItem, const InventoryLocation & tInvLoc)
{

    InventoryLocation & tInvLocation = tACDItem.GetInventoryLocation();
    tInvLocation = tInvLoc;
}






static BOOL sSUseItem(ACDID idACDPlayer, ACDID idACDItemToUse, const Target * ptTarget)
{




    PowerStartUseItem(idACDPlayer, idACDItemToUse, ptTarget);
    return TRUE;
}












static void sNotifyPlayerOfItemDeletion(ActorCommonData & tACDItem)
{

    ACDID idACDOwner = tACDItem.GetInventoryContainerOwner();
    Player * ptPlayer = PlayerGetByACD(idACDOwner);
    if (ptPlayer)
    {



        PlayerRemoveItemFromTradeOffer(ptPlayer, tACDItem);

        SimpleMessage tMessage;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_ITEM_DELETE_NOTIFY, &tMessage, sizeof(tMessage));
    }
}





static BOOL sDeleteSocketedGem(const InventoryLocation & tInvLoc, void * pCallBackData)
{


    ActorCommonData * ptACD = ACDTryToGet(tInvLoc.idACDContainer);
    if (!ptACD)
        return TRUE;



    ACDID idACDItem = ACDInventoryGetSquareItem(*ptACD, tInvLoc.eSlot, tInvLoc.pt);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem || !ItemIsSocketable(*ptACDItem))
        return TRUE;



    sACDRemoveFromCurrentPosition(*ptACDItem);

    ActorID idActorItem = (ActorID)ACDGetOwner(ptACDItem->id);

    BOOL bFlagForDeletion = *(BOOL *)pCallBackData;
    if (bFlagForDeletion)
    {
        ActorFlagForDeletion(idActorItem);
    }
    else
    {
        ActorDelete(idActorItem);
    }


    return TRUE;
}
static void sDeleteSocketedGems(ActorCommonData & tACDItem, BOOL bFlagForDeletion)
{


    if (ItemHasSockets(tACDItem) && ItemGetNumFilledSockets(tACDItem) > 0)
    {

        ACDInventoryIterateSlot(tACDItem, SLOT_ITEM_SOCKETS, NULL, sDeleteSocketedGem, &bFlagForDeletion, FALSE, FALSE);
    }
}





void SACDInventoryDeleteItem(ActorCommonData & tACDItem, uint64 uCount, BOOL bMarkAsDirty, BOOL bFlagForDeletion)
{











    sNotifyPlayerOfItemDeletion(tACDItem);


    uint64 uStackSize = tACDItem.GetStackCount();
    if (uCount < uStackSize)
    {
        tACDItem.SubtractFromStackCount(uCount);
        if (bMarkAsDirty)
        {
            SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_STACK, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());
        }
    }
    else
    {



        if (bMarkAsDirty)
        {
            SACDInventoryAddToDirtyList(tACDItem, AIDA_REMOVE, tACDItem.GetLocationSlot(), SLOT_NONE);
        }

        sDeleteSocketedGems(tACDItem, bFlagForDeletion);

        ACDRemoveFromCurrentPosition(tACDItem);


        ActorID idActorUseItem = (ActorID)ACDGetOwner(tACDItem.id);


        if (bFlagForDeletion)
        {
            ActorFlagForDeletion(idActorUseItem);
        }
        else
        {
            ActorDelete(idActorUseItem);
        }
    }

}



















































































































































































































































































































































BOOL SACDInventoryDeleteItemAfterUse(ActorCommonData & tACDItem)
{



    if (ItemDeletedAfterUse(tACDItem))
    {



        if (!ItemIsUsable(tACDItem))
        {
            return FALSE;
        }

        SACDInventoryDeleteItem(tACDItem, 1, TRUE, FALSE);
    }


    return TRUE;
}



static BOOL sSPlaceItem(ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest)
{






    if (!ACDInventoryMove(tACDItem, tInvLocDest))
        return FALSE;




    sUpdateItemACD(tACDItem, tACDItem.GetInventoryLocation());

    return TRUE;
}





static BOOL sStackItems(ActorCommonData & tACDSrcItem, ActorCommonData & tACDDestItem, uint64 uAmount, BOOL bMarkAsDirty)
{
    if (tACDDestItem.hGB != tACDSrcItem.hGB)
        return FALSE;
    if (ItemIsUnidentified(tACDSrcItem) || ItemIsUnidentified(tACDDestItem))
        return FALSE;
    if (ItemGetBindingLevel(tACDSrcItem) == IBL_BOUND_TO_GAME || ItemGetBindingLevel(tACDDestItem) == IBL_BOUND_TO_GAME)
        return FALSE;

    uint64 uMaxStackSize = ItemGetMaxStackSize(tACDDestItem.hGB, NULL, &tACDDestItem);
    uint64 uStackSrc = Math::Max<uint64>(1, uAmount);
    uint64 uAdded = tACDDestItem.AddToStackCount(uStackSrc, uMaxStackSize);
    if (uAdded == 0)
        return FALSE;

    STradeCheckForModified(tACDSrcItem, TRUE);
    SACDInventoryDeleteItem(tACDSrcItem, uAdded, bMarkAsDirty, FALSE);
    STradeCheckForModified(tACDDestItem, FALSE);
    if (bMarkAsDirty)
        SACDInventoryAddToDirtyList(tACDDestItem, AIDA_UPDATE_STACK, tACDSrcItem.GetLocationSlot(), tACDDestItem.GetLocationSlot());
    return TRUE;
}




































// NON_MATCHING: 2 insns in the inlined sACDInventorySwap: at line 4338 the original compares the two eSlot values before the two fItemNIsLarge flags, ours the other way round; tried nested vs one three-term condition and both != operand orders.
static BOOL sSSwapItem(
    ActorCommonData & tACDSrc,
    ActorCommonData & tACDDest,
    const IVector2D & pt)
{








    uint64 uMaxStackSize = ItemGetMaxStackSize(tACDDest.hGB, NULL, &tACDDest);
    if (tACDDest.hGB.gbid == tACDSrc.hGB.gbid && uMaxStackSize > 1 &&

        tACDDest.GetStackCount() < uMaxStackSize &&
        !ItemIsUnidentified(tACDSrc) &&
        !ItemIsUnidentified(tACDDest) &&
        ItemGetBindingLevel(tACDSrc) != IBL_BOUND_TO_GAME &&
        ItemGetBindingLevel(tACDDest) != IBL_BOUND_TO_GAME)
    {
        ACDID idACDSlotOwner = tACDDest.GetLocation().tInvLocation.idACDContainer;
        ActorCommonData & tACDOwner = ACDGet(idACDSlotOwner);
        return sStackItems(tACDSrc, tACDDest, tACDSrc.GetStackCount(), !tACDOwner.IsMerchant());
    }

    if (tACDSrc.id == tACDDest.id)
        return FALSE;





    InventoryLocation tInvLocSrc_New = tACDDest.GetLocation().tInvLocation;
    tInvLocSrc_New.pt = pt;


    return sACDInventorySwap(tACDSrc, tInvLocSrc_New, tACDDest);
}








BOOL sSlotIsAlwaysSingleItem(InventorySlotType eSlot)
{
    return eSlot == SLOT_PLAYER_BUYBACK;
}




IVector2D ACDInventoryGetItemSize(const ActorCommonData & tACDItem, InventorySlotType eSlot)
{
    if (ItemIsLargeSlotItem(tACDItem.hGB.gbid) && !sSlotIsAlwaysSingleItem(eSlot))
        return IVector2D(1, 2);
    return IVector2D(1, 1);
}











IRect2D ACDInventoryGetItemExtents(const ActorCommonData & tACDItem, const IVector2D & vecBase)
{


    IVector2D vecSize = ACDInventoryGetItemSize(tACDItem, SLOT_NONE);


    return IRect2D(vecBase.x, vecBase.y, vecBase.x + vecSize.x, vecBase.y + vecSize.y);
}






static BOOL sTestSlotOverlap(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    OverlapTestData * ptData = (OverlapTestData *)pCallBackData;

    if (!ptData)
        return FALSE;





    ActorCommonData * ptACDContainer = ACDTryToGet(tInvLoc.idACDContainer);
    if (!ptACDContainer)
        return FALSE;



    ACDID idACDSquare = ACDInventoryGetSquareItem(*ptACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);






    if (ptACDItem && idACDSquare != ptData->idACDItemToTest)
    {



        IRect2D rectItemExtents = ACDInventoryGetItemExtents(*ptACDItem, tInvLoc.pt);
        if (rectItemExtents.ContainsPoint(ptData->tSrcInvLoc.pt))
        {




            if (!ptData->ptList->Find(idACDSquare))
            {



                ptData->ptList->AddTail_Tracked(idACDSquare);
            }
        }
    }
    return TRUE;
}


void ACDInventoryTestOverlap(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc, InventoryOverlapList & tList)
{



    tList.RemoveAll();

    OverlapTestData tData;
    tData.idACDItemToTest = tACDItem.id;
    tData.tSrcInvLoc = tInvLoc;
    tData.ptList = &tList;

    IRect2D rectItemExtents = ACDInventoryGetItemExtents(tACDItem, tInvLoc.pt);
    ActorCommonData & tACD = ACDGet(tInvLoc.idACDContainer);

    for (int32 nSlotY = rectItemExtents.top; nSlotY < rectItemExtents.bottom; nSlotY++)
    {
        for (int32 nSlotX = rectItemExtents.left; nSlotX < rectItemExtents.right; nSlotX++)
        {
            tData.tSrcInvLoc.pt.Set(nSlotX, nSlotY);



            IRect2D rectSearch;
            rectSearch.Set(nSlotX, nSlotY - 1, nSlotX + 1, nSlotY + 1);
            ACDInventoryIterateSlot(tACD, tInvLoc.eSlot, &rectSearch, sTestSlotOverlap, &tData, FALSE, FALSE);
        }
    }
}



static BOOL sProcessMoveStandard(ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest)
{

    ACDID idACDItemInSlot;

    if (ACDInventoryCanMove(tACDItem, tInvLocDest, &idACDItemInSlot) != GAMEERROR_NONE)
        return FALSE;






    if (idACDItemInSlot == tACDItem.id)
    {
        sSPlaceItem(tACDItem, tInvLocDest);
        return TRUE;
    }

    ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
    if (!ptACDItemInSlot)
    {
        sSPlaceItem(tACDItem, tInvLocDest);
        return TRUE;
    }


    if (ptACDItemInSlot->hGB.gbid == tACDItem.hGB.gbid &&
        ItemGetMaxStackSize(tACDItem.hGB, NULL, NULL) > 1)
    {
        if (ACDInventoryStackItems(tACDItem, *ptACDItemInSlot))
            return TRUE;
    }



    ActorLocation tLocCur = tACDItem.GetLocation();
    Player * ptPlayer = PlayerGetByACD(tLocCur.tInvLocation.idACDContainer);

    if (tInvLocDest.eSlot == SLOT_PLAYER_BACKPACK &&
        (tLocCur.eActorLocation != AL_INVENTORY || tLocCur.tInvLocation.eSlot != SLOT_PLAYER_BACKPACK))
    {

        if (ptPlayer)
        {
            PlayerClearPotionBtnForMissingItem(ptPlayer, ptACDItemInSlot->id);
        }
    }

    sSSwapItem(tACDItem, *ptACDItemInSlot, tInvLocDest.pt);
    if (ptPlayer)
    {

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            ptACDPlayer->dwFlags |= (1 << ACD_QUEUE_INVENTORY_EQUIPMENT_OR_SKILL_CHANGED_BIT);
        }
    }
    return TRUE;
}





static BOOL sPlaceOrSwapToArm(ActorCommonData & tACDOwner, ActorCommonData & tACDItem, const ACDID idACDArm, InventorySlotType eSlot)
{




    if (tACDItem.id == idACDArm)
        return TRUE;







    ActorCommonData * ptACDArm = ACDTryToGet(idACDArm);
    if (!ptACDArm)
    {
        InventoryLocation tInvLocDest;
        ACDInventoryLocationSet(tInvLocDest, tACDOwner.id, eSlot, 0, 0);

        return sSPlaceItem(tACDItem, tInvLocDest);
    }

    return sSSwapItem(tACDItem, *ptACDArm, IVector2D(0, 0));
}




static BOOL sPlaceOrSwapMultiLeft(ActorCommonData & tACDPlayer, ActorCommonData & tACDOwner, ActorCommonData & tACDItem, ACDID idACDLeft, ActorCommonData & tACDRight, InventorySlotType eSlotLeft)
{






    InventoryLocation tInvLocDest;
    ActorCommonData * ptACDLeft = ACDTryToGet(idACDLeft);

    if (!ptACDLeft)
    {

        InventoryLocation tInvLocSwappedItem = tACDItem.GetInventoryLocation();





        if (ACDInventoryItemAllowedInSlot(tACDRight, tInvLocSwappedItem, FALSE, FALSE) != GAMEERROR_NONE)
            return FALSE;




        ACDInventoryLocationSet(tInvLocDest, tACDOwner.id, eSlotLeft, 0, 0);
        if (!sSPlaceItem(tACDItem, tInvLocDest))
            return FALSE;




        if (!sSPlaceItem(tACDRight, tInvLocSwappedItem))
        {

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }

        return TRUE;
    }


    InventorySlotType eSlotTypeDest = SLOT_PLAYER_BACKPACK;


    IVector2D tPoint;
    if (!ACDInventoryCanHold(tACDPlayer, tACDRight, eSlotTypeDest, tPoint, NULL))
        return FALSE;




    ACDInventoryLocationSet(tInvLocDest, tACDPlayer.id, eSlotTypeDest, tPoint.x, tPoint.y);
    if (!sSPlaceItem(tACDRight, tInvLocDest))
        return FALSE;




    if (!sSSwapItem(tACDItem, *ptACDLeft, IVector2D(0, 0)))
        return FALSE;

    return TRUE;
}






static BOOL sPlaceOrSwapMultiRight(ActorCommonData & tACDPlayer, ActorCommonData & tACDOwner, ActorCommonData & tACDItem, ActorCommonData & tACDLeft, ACDID idACDRight, InventorySlotType eSlotRight)
{






    InventoryLocation tInvLocDest;
    ActorCommonData * ptACDRight = ACDTryToGet(idACDRight);

    if (!ptACDRight)
    {

        InventoryLocation tInvLocSwappedItem = tACDItem.GetInventoryLocation();





        if (ACDInventoryItemAllowedInSlot(tACDLeft, tInvLocSwappedItem, FALSE, FALSE) != GAMEERROR_NONE)
            return FALSE;




        ACDInventoryLocationSet(tInvLocDest, tACDOwner.id, eSlotRight, 0, 0);
        if (!sSPlaceItem(tACDItem, tInvLocDest))
            return FALSE;




        if (!sSPlaceItem(tACDLeft, tInvLocSwappedItem))
        {

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }

        return TRUE;
    }


    InventorySlotType eSlotTypeDest = SLOT_PLAYER_BACKPACK;


    IVector2D tPoint;
    if (!ACDInventoryCanHold(tACDPlayer, tACDLeft, eSlotTypeDest, tPoint, NULL))
        return FALSE;




    ACDInventoryLocationSet(tInvLocDest, tACDPlayer.id, eSlotTypeDest, tPoint.x, tPoint.y);
    if (!sSPlaceItem(tACDLeft, tInvLocDest))
        return FALSE;




    if (!sSSwapItem(tACDItem, *ptACDRight, IVector2D(0, 0)))
        return FALSE;

    return TRUE;
}





static BOOL sProcessMoveArmLeft(ActorCommonData & tACDPlayer, ActorCommonData & tACDOwner, ActorCommonData & tACDItem, InventorySlotType eSlotLeft, InventorySlotType eSlotRight)
{






    ACDID idACDLeft = sGetItemInSlot(tACDOwner, eSlotLeft);
    ACDID idACDRight = sGetItemInSlot(tACDOwner, eSlotRight);

    ActorCommonData * ptACDRight = ACDTryToGet(idACDRight);

    if (!ptACDRight)
    {
        return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDLeft, eSlotLeft);
    }


    EInvItemClass eRight = GetInventoryItemClass(ptACDRight->hGB.gbid);
    if (ItemUsesQuiver(tACDItem) && eRight == IIC_QUIVER)
    {
        return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDLeft, eSlotLeft);
    }

    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD) && eRight == IIC_SHIELD)
    {
        return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDLeft, eSlotLeft);
    }


    EInvItemClass eHandItemClass = GetInventoryItemClass(tACDItem.hGB.gbid);
    if (eHandItemClass == IIC_TWOHANDED) return sPlaceOrSwapMultiLeft(tACDPlayer, tACDOwner, tACDItem, idACDLeft, *ptACDRight, eSlotLeft);

    if (eHandItemClass != IIC_ONEHANDED)
        return FALSE;



    if (eRight == IIC_QUIVER) return sPlaceOrSwapMultiLeft(tACDPlayer, tACDOwner, tACDItem, idACDLeft, *ptACDRight, eSlotLeft);
    if (!ItemHasLabel(tACDItem.hGB.gbid, ITL_HAND_CROSSBOW) && ItemHasLabel(ptACDRight->hGB.gbid, ITL_HAND_CROSSBOW))
    {
        return sPlaceOrSwapMultiLeft(tACDPlayer, tACDOwner, tACDItem, idACDLeft, *ptACDRight, eSlotLeft);
    }


    return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDLeft, eSlotLeft);
}













static BOOL sProcessMoveArmRight(ActorCommonData & tACDPlayer, ActorCommonData & tACDOwner, ActorCommonData & tACDItem, InventorySlotType eSlotRight, InventorySlotType eSlotLeft)
{





    EInvItemClass eItem = GetInventoryItemClass(tACDItem.hGB.gbid);


    if (eItem == IIC_TWOHANDED)
    {
        return sProcessMoveArmLeft(tACDPlayer, tACDOwner, tACDItem, eSlotLeft, eSlotRight);
    }


    if (eItem == IIC_ONEHANDED &&
        !ACDCanDualWield(tACDOwner))
    {
        return FALSE;
    }

    ACDID idACDLeft = sGetItemInSlot(tACDOwner, eSlotLeft);
    ACDID idACDRight = sGetItemInSlot(tACDOwner, eSlotRight);

    ActorCommonData * ptACDLeft = ACDTryToGet(idACDLeft);


    if (!ptACDLeft)
    {
        return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDRight, eSlotRight);
    }

    EInvItemClass eLeft = GetInventoryItemClass(ptACDLeft->hGB.gbid);

    if (eItem != IIC_QUIVER && eLeft != IIC_TWOHANDED)
    {
        return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDRight, eSlotRight);
    }


    if (ItemUsesQuiver(*ptACDLeft) && eItem == IIC_QUIVER)
    {
        return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDRight, eSlotRight);
    }

    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD) && eItem == IIC_SHIELD)
    {
        return sPlaceOrSwapToArm(tACDOwner, tACDItem, idACDRight, eSlotRight);
    }



    return sPlaceOrSwapMultiRight(tACDPlayer, tACDOwner, tACDItem, *ptACDLeft, idACDRight, eSlotRight);
}



BOOL ActorInventorySocketItem(
    ActorCommonData & tACDItemToSocket,
    ActorCommonData & tACDItemSocketedInto,
    const IVector2D & tPoint,
    BOOL bMarkItemAsDirty)
{






    if (GameActiveGameCommonDataIsClient())
        return FALSE;



    ActorCommonData * ptACDItemToSocket = &tACDItemToSocket;
    SocketReason socketReason = ItemCanSocketInto(tACDItemToSocket, tACDItemSocketedInto);
    if (socketReason == SOCKET_REASON_SUCCESS)
    {

        InventorySlotType eCameFrom = tACDItemToSocket.GetLocationSlot();
        int64 nStackSize = tACDItemToSocket.GetStackCount();
        if (nStackSize > 1)
        {
            SACDInventoryDeleteItem(tACDItemToSocket, 1, TRUE, FALSE);

            ACDID idACDItemToSocketNew = LootQuickCreateItem(tACDItemToSocket.hGB, TRUE);

            ptACDItemToSocket = &ACDGet(idACDItemToSocketNew);



            ItemSetBindingLevel(*ptACDItemToSocket, ItemGetBindingLevel(tACDItemToSocket));
            ItemSetBoundToACD(*ptACDItemToSocket, ItemGetBoundToACD(tACDItemToSocket, FALSE));
            bMarkItemAsDirty = TRUE;
        }
        else
        {   ACDRemoveFromCurrentPosition(*ptACDItemToSocket);
        }

        ItemAddSocketedItemAttributes(*ptACDItemToSocket, tACDItemSocketedInto);

        InventoryLocation tLocation;
        tLocation.idACDContainer = tACDItemSocketedInto.id;
        tLocation.eSlot = SLOT_ITEM_SOCKETS;
        tLocation.pt = tPoint;

        ACDAddToInventory(*ptACDItemToSocket, tLocation, bMarkItemAsDirty, eCameFrom);
        tACDItemSocketedInto.AttributesSetInt(ATTRIBUTE_SOCKETS_FILLED, tACDItemSocketedInto.AttributesGetInt(ATTRIBUTE_SOCKETS_FILLED) + 1);

        if (bMarkItemAsDirty)
        {








            SACDInventoryAddToDirtyList(tACDItemSocketedInto, AIDA_UPDATE_GEM_ADDED, tACDItemSocketedInto.GetLocationSlot(), tACDItemSocketedInto.GetLocationSlot());
        }

        tACDItemSocketedInto.ModifiedMark(ACDMODIFIED_ATTRIBUTES);
        ptACDItemToSocket->ModifiedMark(ACDMODIFIED_INVENTORY_POSITION);
        return TRUE;
    }


    OnlineService::ItemId tItemIDToSocket = ActorGet((ActorID)tACDItemToSocket.idOwner)->tItemId;
    OnlineService::ItemId tItemIDSocketedInto = ActorGet((ActorID)tACDItemSocketedInto.idOwner)->tItemId;









    return FALSE;
}


static BOOL sIsHandSlot(InventorySlotType eSlot)
{

    switch (eSlot)
    {
    case SLOT_PLAYER_RIGHT_HAND:
    case SLOT_PLAYER_LEFT_HAND:
    case SLOT_HIRELING_RIGHT_HAND:
    case SLOT_HIRELING_LEFT_HAND:
        return TRUE;
    }
    return FALSE;
}



BOOL SACDInventoryProcessMoveRequest(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest)
{










    if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLocDest, FALSE, FALSE) != GAMEERROR_NONE)
        return FALSE;



    if (tInvLocDest.idACDContainer != tACDPlayer.id)
    {

        PlayerRemoveItemFromTradeOffer(PlayerGetByACD(tACDPlayer.id), tACDItem);
    }


    if (tACDItem.IsInInventory() && (!ItemIsTwoHanded(tACDItem) || ItemUsesQuiver(tACDItem)))
    {
        InventoryLocation & tInvLocCurrent = tACDItem.GetInventoryLocation();
        BOOL fSrcIsHandSlot = sIsHandSlot(tInvLocCurrent.eSlot);
        BOOL fDestIsHandSlot = sIsHandSlot(tInvLocDest.eSlot);
        if ((fSrcIsHandSlot && !fDestIsHandSlot) ||
            (fSrcIsHandSlot && fDestIsHandSlot && tInvLocCurrent.idACDContainer != tInvLocDest.idACDContainer))
        {
            ACDID idDestItem = ACDInventoryGetItemInSlot(tInvLocDest);
            ActorCommonData * ptACDDestItem = ACDTryToGet(idDestItem);

            InventoryOverlapList tList;
            ACDInventoryTestOverlap(tACDItem, tInvLocDest, tList);
            if (!ptACDDestItem && tList.Count() > 0)
            {

                idDestItem = tList.GetHead();
                ptACDDestItem = ACDTryToGet(idDestItem);
            }

            if (ptACDDestItem)
            {

                if (ItemIsTwoHanded(*ptACDDestItem) && !ItemIsTwoHanded(tACDItem))
                {
                    return SACDInventoryProcessMoveRequest(tACDPlayer, *ptACDDestItem, tInvLocCurrent);
                }


                if (GetInventoryItemClass(ptACDDestItem->hGB.gbid) != IIC_QUIVER &&
                    GetInventoryItemClass(tACDItem.hGB.gbid) == IIC_QUIVER)
                {
                    return SACDInventoryProcessMoveRequest(tACDPlayer, *ptACDDestItem, tInvLocCurrent);
                }

                if (PlayerGetPlayerClass(tACDPlayer.id) == PLAYER_CLASS_DEMONHUNTER &&
                    (tInvLocDest.idACDContainer == tACDPlayer.id || tInvLocCurrent.idACDContainer == tACDPlayer.id))
                {
                    WeaponClass eWeaponClassDestItem = GetWeaponClass(ptACDDestItem->hGB.gbid, GBID_NONE);
                    WeaponClass eWeaponClassPlayer = GetWeaponClass(tACDItem.hGB.gbid, GBID_NONE);


                    if (eWeaponClassDestItem != WEAPONCLASS_NONE)
                    {
                        if (eWeaponClassDestItem == WEAPONCLASS_HANDXBOW)
                        {
                            if (eWeaponClassPlayer != WEAPONCLASS_DUALWIELD && eWeaponClassPlayer != WEAPONCLASS_HANDXBOW)
                                return SACDInventoryProcessMoveRequest(tACDPlayer, *ptACDDestItem, tInvLocCurrent);
                        }



                        else if (eWeaponClassPlayer == WEAPONCLASS_DUALWIELD || eWeaponClassPlayer == WEAPONCLASS_HANDXBOW)
                        {
                            return SACDInventoryProcessMoveRequest(tACDPlayer, *ptACDDestItem, tInvLocCurrent);
                        }
                    }
                }
            }
        }
    }
    if (tACDItem.IsInInventory())
    {
        InventoryLocation tInvLocCur = tACDItem.GetInventoryLocation();
        InventorySlotType eSlotCurrent = tInvLocCur.eSlot;

        if ((eSlotCurrent >= SLOT_PLAYER_BODYSLOT_START && eSlotCurrent <= SLOT_PLAYER_BODYSLOT_END) ||
            (eSlotCurrent >= SLOT_HIRELING_START && eSlotCurrent <= SLOT_HIRELING_END))
        {
            PowerAPI_BuffApplyOnEquipmentChangeEffects(tInvLocCur.idACDContainer, eSlotCurrent, 1);
        }

        if (tInvLocDest.eSlot != SLOT_PLAYER_BACKPACK)
        {
            Player * ptPlayer = PlayerGetByACD(tInvLocCur.idACDContainer);
            if (ptPlayer)
            {
                PlayerClearPotionBtnForMissingItem(ptPlayer, tACDItem.id);
            }
        }
    }

    if ((tInvLocDest.eSlot >= SLOT_PLAYER_BODYSLOT_START && tInvLocDest.eSlot <= SLOT_PLAYER_BODYSLOT_END) ||
        (tInvLocDest.eSlot >= SLOT_HIRELING_START && tInvLocDest.eSlot <= SLOT_HIRELING_END))
    {

        ACDID idACDItemInSlot;
        if (ACDInventoryCanMove(tACDItem, tInvLocDest, &idACDItemInSlot) == GAMEERROR_NONE)
        {
            PowerAPI_BuffApplyOnEquipmentChangeEffects(tInvLocDest.idACDContainer, tInvLocDest.eSlot, 1);
        }

        PowerAPI_BuffApplyOnEquipmentChangeEffects(tInvLocDest.idACDContainer, tInvLocDest.eSlot, 0);
        tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK, 0);
    }


    ACDID idACDOwnerPrev = ACDInventoryGetUltimateContainer(tACDItem);

    switch (tInvLocDest.eSlot)
    {
    case SLOT_PLAYER_LEFT_HAND:

        if (!sProcessMoveArmLeft(tACDPlayer, tACDPlayer, tACDItem, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_RIGHT_HAND))
            return FALSE;
        break;



    case SLOT_HIRELING_LEFT_HAND:
        {
            ActorCommonData & tACDOwner = ACDGet(tInvLocDest.idACDContainer);
            if (!sProcessMoveArmLeft(tACDPlayer, tACDOwner, tACDItem, tInvLocDest.eSlot, SLOT_HIRELING_RIGHT_HAND))
                return FALSE;
        }
        break;



    case SLOT_PLAYER_RIGHT_HAND:

        if (!sProcessMoveArmRight(tACDPlayer, tACDPlayer, tACDItem, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND))
            return FALSE;
        break;



    case SLOT_HIRELING_RIGHT_HAND:
        {
            ActorCommonData & tACDOwner = ACDGet(tInvLocDest.idACDContainer);
            if (!sProcessMoveArmRight(tACDPlayer, tACDOwner, tACDItem, tInvLocDest.eSlot, SLOT_HIRELING_LEFT_HAND))
                return FALSE;
        }
        break;



    default:

        if (!sProcessMoveStandard(tACDItem, tInvLocDest))
            return FALSE;
        break;
    }



    if ((tInvLocDest.eSlot >= SLOT_PLAYER_BODYSLOT_START && tInvLocDest.eSlot <= SLOT_PLAYER_BODYSLOT_END) ||
        (tInvLocDest.eSlot >= SLOT_HIRELING_START && tInvLocDest.eSlot <= SLOT_HIRELING_END))
    {
        ACDInventoryEquipmentOrSkillChanged(tACDPlayer);
        ACDInventoryEquippedItemsChanged(tACDPlayer);
    }

    tACDPlayer.MarkHeroStateSavedDataChanged();


    if (ActorFlaggedForDeletion((ActorID)ACDGetOwner(tACDItem.id)))
        return TRUE;


    ACDID idACDOwnerCur = ACDInventoryGetUltimateContainer(tACDItem);

    if (idACDOwnerPrev != ACDID_INVALID && idACDOwnerPrev != tACDPlayer.id)
    {
        ActorCommonData * ptACDOwnerPrev = ACDTryToGet(idACDOwnerPrev);
        if (ptACDOwnerPrev)
        {
            ACDInventoryEquipmentOrSkillChanged(*ptACDOwnerPrev);
        }
    }
    else if (idACDOwnerCur != ACDID_INVALID && idACDOwnerCur != tACDPlayer.id)
    {
        ActorCommonData * ptACDOwnerCur = ACDTryToGet(idACDOwnerCur);
        if (ptACDOwnerCur)
        {
            ACDInventoryEquipmentOrSkillChanged(*ptACDOwnerCur);
        }
    }




    return TRUE;
}



BOOL SACDInventoryProcessSocketRequest(
    ActorCommonData & tACDPlayer,
    ActorCommonData & tACDGem,
    ActorCommonData & tACDItemToReceiveGem)
{
    IVector2D tPoint;
    if (!ACDInventoryCanHold(tACDItemToReceiveGem, tACDGem, SLOT_ITEM_SOCKETS, tPoint, NULL))
        return FALSE;



    if (!ActorInventorySocketItem(tACDGem, tACDItemToReceiveGem, tPoint, TRUE))
        return FALSE;




    ACDID idACDOwner = tACDItemToReceiveGem.GetInventoryContainerOwner();
    ActorCommonData & tACDOwner = ACDGet(idACDOwner);

    tACDOwner.ModifiedMark(ACDMODIFIED_ATTRIBUTES);
    ACDInventoryEquipmentOrSkillChanged(tACDOwner);






    {
        Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);

        if (ptPlayer)
        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.SetACD(&tACDGem);

            ScopedItemDefinitionLockR ptDefGem(tACDGem.hGB.gbid);

            tEvaluationData.EvaluateEvent(ptPlayer,
                Achievement::ACH_EVENT_SOCKET_ITEM,

                ptDefGem->eGemType,
                1);


            SItemCrafting_TryUnlockSecondaryTransmogs(tACDGem.hGB.gbid, *ptPlayer);
        }
    }





    return TRUE;
}



BOOL SACDInventoryProcessUseRequest(ACDID idPlayerACD, ACDID idACDItemToUse, const Target * ptTarget)
{







    ActorCommonData & tACDPlayer = ACDGet(idPlayerACD);
    ActorCommonData & tACDItem = ACDGet(idACDItemToUse);

    if (!ACDInventoryCanUseItem(tACDPlayer, tACDItem, NULL))
        return FALSE;




    if (!sSUseItem(tACDPlayer.id, tACDItem.id, ptTarget))
        return FALSE;








    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);



    return TRUE;
}




static BOOL sDropItem(ActorCommonData & tACDWantingToDrop, ActorCommonData & tACDItem, const BOOL fVisibleOnlyByOwner)
{






    WorldPlace tPlace;








    if (tACDItem.IsInInventory())
    {
        InventoryLocation tInvLocation = tACDItem.GetInventoryLocation();
        Player * ptPlayer = PlayerGetByACD(tInvLocation.idACDContainer);
        if (ptPlayer)
        {
            PlayerClearPotionBtnForMissingItem(ptPlayer, tACDItem.id);
        }

        InventorySlotType eSlot = tInvLocation.eSlot;
        if ((eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot <= SLOT_PLAYER_BODYSLOT_END) ||
            (eSlot >= SLOT_HIRELING_START && eSlot <= SLOT_HIRELING_END))
        {
            PowerAPI_BuffApplyOnEquipmentChangeEffects(tInvLocation.idACDContainer, eSlot, 1);
        }
    }
    tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK, 0);

    Player * ptAssignedPlayer = NULL;

    BOOL bLocationFound = FlippyFindLandingLocation(tACDWantingToDrop, tACDItem.snoActor, tPlace, ptAssignedPlayer, NULL, ACDID_INVALID);
    if (bLocationFound)
    {
        sNotifyPlayerOfItemDeletion(tACDItem);

        ACDInventoryDrop(tACDWantingToDrop, tACDItem, &tPlace); return TRUE;
    }
    else
    {

        tPlace = tACDWantingToDrop.GetPlace();
    }















    return FALSE;
}


BOOL SACDInventoryProcessDropRequest(const ACDID idACDWantingToDrop, const ACDID idACDItem, const BOOL bCheckAlive)
{



    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
        return FALSE;




    if (TradeIsTrading(idACDWantingToDrop))
        return FALSE;




    if (ItemCanDrop(*ptACDItem) != GAMEERROR_NONE)
        return FALSE;



    ActorCommonData & tACDWantingToDrop = ACDGet(idACDWantingToDrop);
    if (tACDWantingToDrop.IsObserver())
        return FALSE;




    if (bCheckAlive && !tACDWantingToDrop.IsAlive())
        return FALSE;




    if (!ptACDItem->IsInInventory())
        return FALSE;




    return sDropItem(tACDWantingToDrop, *ptACDItem, bCheckAlive);
}



void SACDInventoryProcessDropToPortraitRequest(ACDID idACDPlayer, ACDID idACDItem)
{


    if (!ACDIsPlayer(idACDPlayer))
        return;




    ACDID idACDHireling = PlayerGetHireling(idACDPlayer);
    if (idACDHireling == ACDID_INVALID)
        return;



    ActorCommonData & tACDItem = ACDGet(idACDItem);
    if (ItemGetUseType(tACDItem) == ITEM_USE_HEALTHPOTION)
    {

        Target tTarget = cgTargetNone;
        if (!SACDInventoryProcessUseRequest(idACDHireling, tACDItem.id, &tTarget))
        {
            PlayerDisplayErrorMessage(idACDPlayer, GAMEERROR_IARESULT_HIRELING_CANT_USE_ITEM);
        }
    }
}



























static ACDID sSplitStack(ActorCommonData & tACDItem, const uint64 uAmount)
{
    // NON_MATCHING: cgplaceNull is reached as merged-globals+0xc in the original (another file-static constant sits before it; not referenced yet here)


    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);
    ItemGeneratorPopulateFromACDID(tACDItem, tGenerator, TRUE);

    ACDID idACDSplitItem = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);






    tACDItem.SubtractFromStackCount(uAmount);
    SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_STACK, tACDItem.tLocation.tInvLocation.eSlot, tACDItem.tLocation.tInvLocation.eSlot);


    ActorCommonData & tACDNewItem = ACDGet(idACDSplitItem);
    tACDNewItem.SetStackCount(uAmount);

    ItemSetBindingLevel(tACDNewItem, ItemGetBindingLevel(tACDItem));
    ItemSetBoundToACD(tACDNewItem, ItemGetBoundToACD(tACDItem, FALSE));

    return idACDSplitItem;
}





BOOL SACDInventoryProcessDropStackPortionRequest(const ACDID idACDWantingToDrop, ActorCommonData & tACDItem, const uint64 uAmount)
{









    if (ItemCanDrop(tACDItem) != GAMEERROR_NONE)
        return FALSE;



    ActorCommonData & tACDWantingToDrop = ACDGet(idACDWantingToDrop);
    if (tACDWantingToDrop.IsObserver())
        return FALSE;




    if (!tACDWantingToDrop.IsAlive())
        return FALSE;





    if (TradeIsTrading(tACDWantingToDrop.id))
        return FALSE;



    if (!tACDItem.IsInInventory())
        return FALSE;





    ACDID idACDSplitItem = sSplitStack(tACDItem, uAmount);

    ActorCommonData & tACDNewItem = ACDGet(idACDSplitItem);

    return sDropItem(tACDWantingToDrop, tACDNewItem, TRUE);



}




void SACDInventoryProcessSplitStack(ActorCommonData & tACDItem, const uint64 uAmount, const InventoryLocation & tInvLoc)
{









    ACDID idACDSplitItem = sSplitStack(tACDItem, uAmount);

    ActorCommonData & tACDNewItem = ACDGet(idACDSplitItem);
    tACDNewItem.ModifiedMark(9);
    MoveItemToInventory(tACDNewItem, tInvLoc, TRUE, FALSE);
}





void SACDInventoryProcessStackTransfer(ActorCommonData & tACDItemSrc, ActorCommonData & tACDItemDest, const uint64 uAmount)
{
    sStackItems(tACDItemSrc, tACDItemDest, uAmount, TRUE);
}







void SACDInventoryProcessGoldTransfer(ActorCommonData & tACDPlayerSrc, ActorCommonData & tACDPlayerDest, const uint64 uAmount)
{


    int64 nGoldOffer = (int64)uAmount;
    CurrencyType eCurrencyType = CURRENCY_GOLD;

    uint64 uOrigAmountSrc = tACDPlayerSrc.GetCurrencyAmount(eCurrencyType);
    uint64 uOrigAmountDst = tACDPlayerDest.GetCurrencyAmount(eCurrencyType);


    BOOL bGoldTradeSuccess = tACDPlayerSrc.ModifyCurrencyAmount(-nGoldOffer, eCurrencyType, GMR_TRADE, FALSE) &&
        tACDPlayerDest.ModifyCurrencyAmount(nGoldOffer, eCurrencyType, GMR_TRADE, FALSE);




    if (!bGoldTradeSuccess)
    {

        tACDPlayerSrc.SetCurrencyAmount(uOrigAmountSrc, eCurrencyType, GMR_TRADE_FAILED);
        tACDPlayerDest.SetCurrencyAmount(uOrigAmountDst, eCurrencyType, GMR_TRADE_FAILED);
    }

}

































































































static void sRMBSkillsFallbackToAttackPower(ActorCommonData & tACD, Player * ptPlayer)
{


    uint32 dwSkillFlags = (1 << SKILL_QUERY_EXCLUDE_NON_USABLE) | (1 << SKILL_QUERY_EXCLUDE_TOGGLEABLES);
    SNO snoAttackPower;

    SNOPooledList listSkills(MemoryPoolsGetDATAIDListPool());
    tACD.AttributesGetPowers(dwSkillFlags, listSkills, &snoAttackPower);

    HotbarButtonData tNewButtonData;
    tNewButtonData.SetSkill(snoAttackPower, RUNE_TYPE_INVALID);

    const HotbarButtonData & tButtonDataRMB = PlayerGetHotbarBtnAssignment(ptPlayer, SKILL_RMB);
    if (!listSkills.Find(tButtonDataRMB.m_snoPower))
    {
        PlayerSetHotbarBtnAssignment(ptPlayer, SKILL_RMB, tNewButtonData);
    }
}



static void sLMBSkillsFallbackToAttackPower(ActorCommonData & tACD, Player * ptPlayer)
{


    uint32 dwSkillFlags = (1 << SKILL_QUERY_EXCLUDE_NON_USABLE) | (1 << SKILL_QUERY_EXCLUDE_NON_LMB) | (1 << SKILL_QUERY_EXCLUDE_TOGGLEABLES);
    SNO snoAttackPower;

    SNOPooledList listSkills(MemoryPoolsGetDATAIDListPool());
    tACD.AttributesGetPowers(dwSkillFlags, listSkills, &snoAttackPower);

    const HotbarButtonData & tButtonDataLMB = PlayerGetHotbarBtnAssignment(ptPlayer, SKILL_LMB);
    if (!listSkills.Find(tButtonDataLMB.m_snoPower))
    {
        HotbarButtonData tNewButtonData;
        tNewButtonData.SetSkill(snoAttackPower, RUNE_TYPE_INVALID);

        PlayerSetHotbarBtnAssignment(ptPlayer, SKILL_LMB, tNewButtonData);
    }
}




void ACDInventoryUpdateSkillButtons(ActorCommonData & tACD)
{



    Player * ptPlayer = PlayerGetByACD(tACD.id);


    sRMBSkillsFallbackToAttackPower(tACD, ptPlayer);
    sLMBSkillsFallbackToAttackPower(tACD, ptPlayer);
}



static void sUpdateItemUseability(
    ACDID idACDSquare,
    InventorySlotType eSlot,
    void * pUserData)
{


    ACDID idACDOwner = *(ACDID *)pUserData;

    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (!ptACDItem || !ptACDOwner)
        return;



    if (!ACDInventorySlotIsEquippable(eSlot))
        return;



    if (!ACDIsPlayer(idACDOwner))
        return;



    Player * ptPlayer = PlayerGetByACD(idACDOwner);


    BOOL fMeetsRequirements = ItemOwnerMeetsRequirements(*ptACDItem, *ptACDOwner, NULL);
    if (!GameActiveGameCommonDataIsClient())
    {
        VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eSlot);
        if (eVisualSlot != VIS_INVALID)
        {
            GBID gbidActor = GBID_NONE;
            DyeType eDyeType = ptACDItem->GetDyeType();
            ItemEffectType eItemEffectType = ITEM_EFFECT_TYPE_NONE;
            int32 nEffectLevel = -1;
            if (fMeetsRequirements && !ItemIsBroken(*ptACDItem) && !ItemIsEquippedButDisabled(*ptACDItem))
            {
                gbidActor = ptACDItem->GetVisualGBID();
                ACDGetItemEffectTypeAndLevel(*ptACDItem, eItemEffectType, nEffectLevel, TRUE, FALSE);
            }

            BOOL bBrokenOrDisabled = ItemIsBroken(*ptACDItem) || ItemIsEquippedButDisabled(*ptACDItem);
            VisualInventorySetItemInSlot(idACDOwner, eVisualSlot, gbidActor, eDyeType, eItemEffectType, nEffectLevel, bBrokenOrDisabled);
        }
        AttribGroupID idFastAttribItem = SItemGetAttributeGroup(*ptACDItem);

        if (ACDInventoryCanMergeAttributes(*ptACDOwner, *ptACDItem, eSlot))
        {
            if (fMeetsRequirements)
            {
                if (!ItemIsEquipped(*ptACDItem))
                {
                    SACDInventorySetItemEquipped(*ptACDItem, FALSE);

                    ptACDItem->AttributesSetInt(ATTRIBUTE_PICKED_UP_TIME, 0);
                    if (!ItemIsBroken(*ptACDItem) && !ItemIsEquippedButDisabled(*ptACDItem))
                    {
                        ActorInventoryMergeItemAttributes(*ptACDOwner, *ptACDItem);
                    }

                    ptACDOwner->AttributesSetInt(MakeAttribKey(ATTRIBUTE_SLOT_UNEQUIPPED_TIME, eSlot), 0);
                }
            }
            else
            {
                if (ItemIsEquipped(*ptACDItem) && !ItemIsBroken(*ptACDItem) && !ItemIsEquippedButDisabled(*ptACDItem))
                {
                    SACDInventorySetItemUnequipped(*ptACDItem);


                    if (ptPlayer && ptACDOwner->AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) > 0)
                    {
                        if (ptACDItem->idUpscaledAttribGroup != ATTRIBGROUPID_INVALID)
                        {
                            ptACDOwner->AttributesUnmergeAttributeGroup(ptACDItem->idUpscaledAttribGroup);
                            ClearUpscaledAttributes(*ptACDItem);
                        }
                    }

                    else
                    {
                        ptACDOwner->AttributesUnmergeAttributeGroup(idFastAttribItem);
                    }
                }
            }
        }
    }
}



static void sUpdateSetItemBonuses(
    ACDID idACDSquare,
    InventorySlotType eSlot,
    void * pUserData)
{




    ACDID idACDOwner = *(ACDID *)pUserData;

    if (!ACDInventorySlotIsEquippable(eSlot))
        return;



    ActorCommonData & tACDOwner = ACDGet(idACDOwner);
    Player * ptPlayer = PlayerGetByACD(tACDOwner.id);



    if (!GameActiveGameCommonDataIsClient())
    {
        ActorCommonData & tACDItem = ACDGet(idACDSquare);

        const GameBalanceDefinition * pdefGBItems;
        GBHandle hGBItem(GB_ITEMS, tACDItem.hGB.gbid);
        const ItemDefinition * pItemDef = (const ItemDefinition *)GBLockR(hGBItem, pdefGBItems, TRUE);
        if (pItemDef)
        {



            if (pItemDef->gbidSet != GBID_NONE)
            {
                SNO snoSetItemBonusPower = GlobalSNOGet(GLOBALSNO_POWERS_SET_ITEM_BONUS_BUFF);


                int32 nCount = tACDOwner.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SET_ITEM_COUNT, pItemDef->gbidSet));

                if (nCount > 1)
                {
                    nCount += tACDOwner.AttributesGetInt(ATTRIBUTE_SET_ITEM_DISCOUNT);
                }


                POS posBuff = tACDOwner.listBuffs.GetHeadPosition();

                while (posBuff)
                {
                    BuffID idBuff = tACDOwner.listBuffs.GetNext(posBuff);

                    PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                    if (ptBuff &&
                        !ptBuff->bDeleted)
                    {
                        if (ptBuff->snoPower == snoSetItemBonusPower &&
                            ptBuff->nBuffData == pItemDef->gbidSet)
                        {
                            if (nCount == ptBuff->nBuffLevel)
                            {
                                GBUnlockR(pdefGBItems);
                                return;
                            }
                        }
                    }
                }


                PowerAPIBuffStartData arSetTierBuffs[13];

                GBHandleList listBonuses(GBGetHandlePool());
                GBEnumerate(GB_SET_ITEM_BONUSES, listBonuses);


                POS pos = listBonuses.GetHeadPosition();
                while (pos)
                {
                    const GBHandle & hGBBonus = listBonuses.GetNext(pos);
                    const GameBalanceDefinition * pdefGBBonus;
                    SetItemBonusTableEntry * pBonus = (SetItemBonusTableEntry *)GBLockR(hGBBonus, pdefGBBonus, TRUE);
                    if (pBonus)
                    {




                        if (pBonus->gbidSet == pItemDef->gbidSet &&

                            pBonus->nRequired <= nCount)
                        {

                            for (int32 i = 0; i < MAX_SET_ITEM_BONUS_ATTRIBUTES; i++)
                            {
                                if (pBonus->ptSetItemBonusAttributes[i].eAttribute != ATTRIBUTE_NONE)
                                {




                                    PowerAPIBuffStartData & tBuffStartData = arSetTierBuffs[pBonus->nRequired];
                                    if (!tBuffStartData.IsInitialized())
                                    {
                                        tBuffStartData.Initialize();
                                        tBuffStartData.snoPower = snoSetItemBonusPower;
                                        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
                                        tBuffStartData.nMaxStackCount = 1000;
                                        tBuffStartData.bInfinite = TRUE;
                                        tBuffStartData.nBuffLevel = nCount;
                                        tBuffStartData.nBuffData = pItemDef->gbidSet;
                                        tBuffStartData.bPositiveEffect = TRUE;
                                        tBuffStartData.bRemoveOnDeath = FALSE;
                                        tBuffStartData.bKeepForProfileSnapshot = TRUE;
                                        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_SET_BONUS_COUNT, 1);
                                    }

                                    float flAttributeValue = AffixGetFormulaValue(
                                        pBonus->ptSetItemBonusAttributes[i].pbAttributeFormula,
                                        pBonus->ptSetItemBonusAttributes[i].serAttributeFormula.dwSize,
                                        &tACDOwner, GBID_NONE, NULL, 0, 0);



                                    FastAttribKey tKey = MakeAttribKey(pBonus->ptSetItemBonusAttributes[i].eAttribute, pBonus->ptSetItemBonusAttributes[i].nParam);
                                    if (KeyGetDataType(tKey) == 0)
                                    {
                                        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, tKey, flAttributeValue);
                                    }
                                    else
                                    {
                                        FastAttribSetValueInt(tBuffStartData.idFastAttrib, tKey, Math::FloatToInt(flAttributeValue));
                                    }
                                }
                            }
                        }
                    }
                    GBUnlockR(pdefGBBonus);
                }

                for (int32 nTier = 0;
                     nTier < 13; nTier++)
                {   PowerAPIBuffStartData & tBuffStartData = arSetTierBuffs[nTier];
                    if (tBuffStartData.IsInitialized())
                    {
                        PowerAPI_BuffAddToACD(tBuffStartData, tACDOwner);
                    }
                }
            }
            GBUnlockR(pdefGBItems);
        }
    }
}




static BOOL sACDSlotIsBareHanded(ActorCommonData & tACD, const InventorySlotType eSlot)
{


    VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eSlot);
    const VisualInventory * ptVisualInventory = VisualInventoryGet(tACD.id);
    if (!ptVisualInventory)
        return FALSE;
    {
        GBID gbidItem = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlot].gbidItem;
        return gbidItem == GBID_NONE;
    }
}








static BOOL sACDIsBareHanded(ActorCommonData & tACD, InventorySlotType eSlotType)
{


    if (!sACDSlotIsBareHanded(tACD, eSlotType))
        return FALSE; {
        WeaponClass eWeaponClass = ACDGetCurrentWeaponClass(tACD);
        switch (eWeaponClass) { case WEAPONCLASS_NONE: case WEAPONCLASS_HTH: case WEAPONCLASS_HTH_WITH_ORB: return TRUE; default: return FALSE; }
    }
}











static void sUpdateBareHandedPassive(ActorCommonData & tACD, BOOL bBareHanded)
{


    SNO snoPassive = GlobalSNOGet(GLOBALSNO_POWERS_BAREHANDED_PASSIVE);
    if (snoPassive == SNO_NONE)
        return;
    if (bBareHanded)
    {
        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = tACD.id;
        tPowerStartData.snoPower = snoPassive;
        PowerStartEx(tPowerStartData, -1, TRUE, FALSE);

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(PowerAPI_BuffFindOnACD(snoPassive, tACD, ACDID_INVALID));

        if (ptBuff)

            ptBuff->bKeepForProfileSnapshot = TRUE;
    }
    else
    {

        PowerAPI_BuffFlagForDeletion(PowerAPI_BuffFindOnACD(snoPassive, tACD, ACDID_INVALID), FALSE, TRUE, 0, TRUE);
    }
}







void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD)
{
    tACD.dwFlags |= (1 << ACD_QUEUE_INVENTORY_EQUIPMENT_OR_SKILL_CHANGED_BIT);
    tACD.dwFlags |= (1 << ACD_QUEUE_SKILL_CHANGED_BIT);
}




void ACDInventoryEquippedItemsChanged(ActorCommonData & tACD)
{
    tACD.dwFlags |= 0x20;
}

void ACDInventoryUpdateSkillButtons(ActorCommonData & tACD);  /* ActorInventory.cpp:8692 */  void SACDInventoryRemoveIllegalItems(ActorCommonData & tACD);  /* :12937 */  void SACDInventoryRestoreIllegalItems(ActorCommonData & tACD);  /* :13065 */


static void sUpdateSetItems(ActorCommonData & tACD)
{



    POS pos = tACD.listBuffs.GetHeadPosition();

    while (pos)
    {
        BuffID idBuff = tACD.listBuffs.GetNext(pos);

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff &&
            !ptBuff->bDeleted)
        {
            if (ptBuff->snoPower == GlobalSNOGet(GLOBALSNO_POWERS_SET_ITEM_BONUS_BUFF))
            {

                int32 nCount = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SET_ITEM_COUNT, ptBuff->nBuffData));
                if (nCount > 1)
                {
                    nCount += tACD.AttributesGetInt(ATTRIBUTE_SET_ITEM_DISCOUNT);
                }

                if (nCount != ptBuff->nBuffLevel)
                {
                    PowerAPI_BuffFlagForDeletion(ptBuff->id, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
                }
            }
        }
    }


    ACDInventoryIterateItems(tACD, SLOT_NONE, 0, sUpdateSetItemBonuses, &tACD);
}




static GBID sGetSlotItemGBID(const ActorCommonData & tACD, const InventorySlotType eSlot)
{


    VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eSlot);
    if (eVisualSlot != VIS_INVALID)
    {
        const VisualInventory * ptVisualInventory = tACD.ptVisualInventory;
        if (ptVisualInventory)
        {

            return ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlot].gbidItem;
        }
    }
    return GBID_NONE;
}





void sACDInventoryHandleSkillChanges(ActorCommonData & tACD)
{








    if (!tACD.IsInWorld())
        return;



    tACD.dwFlags &= ~(1 << ACD_QUEUE_SKILL_CHANGED_BIT);

    sUpdateBareHandedPassive(tACD, sACDIsBareHanded(tACD, SLOT_PLAYER_LEFT_HAND));

    tACD.ApplyAllPassives(FALSE);
}




static void sACDInventoryHandleEquipmentOrSkillChanges(ActorCommonData & tACD)
{

    if (!tACD.IsInWorld())
        return;



    tACD.dwFlags &= ~(1 << ACD_QUEUE_INVENTORY_EQUIPMENT_OR_SKILL_CHANGED_BIT);

    if (ACDIsPlayer(tACD.id))
    {
        Player * ptPlayer;

        sACDInventoryHandleSkillChanges(tACD);

        sUpdateSetItems(tACD);
        ACDInventoryIterateItems(tACD, SLOT_NONE, 0, sUpdateItemUseability, &tACD);

        tACD.AttributesSetInt(ATTRIBUTE_DUALWIELD_HANDS_SWAPPED, FALSE);



        if (!ACDInventorySlotContainsOneHandedWeapon(tACD, SLOT_PLAYER_LEFT_HAND, TRUE) && ACDInventorySlotContainsOneHandedWeapon(tACD, SLOT_PLAYER_RIGHT_HAND, TRUE))
        {
            tACD.AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, 1);
        }
        else if (ACDInventorySlotContainsOneHandedWeapon(tACD, SLOT_PLAYER_LEFT_HAND, TRUE) && !ACDInventorySlotContainsOneHandedWeapon(tACD, SLOT_PLAYER_RIGHT_HAND, TRUE))
        {
            tACD.AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, 0);
        }
        else
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(&tACD);
            GBID gbidLeft = sGetSlotItemGBID(tACD, SLOT_PLAYER_LEFT_HAND);
            GBID gbidRight = sGetSlotItemGBID(tACD, SLOT_PLAYER_RIGHT_HAND);
            if (VisualInventoryShouldSwapWeapons(ePlayerClass, gbidLeft, gbidRight))
            {
                tACD.AttributesSetInt(ATTRIBUTE_DUALWIELD_HANDS_SWAPPED, TRUE);
            }
        }

        ACDInventoryUpdateSkillButtons(tACD);

        ptPlayer = PlayerGetByACD(tACD.id);


        sACDInventoryHandleSkillChanges(tACD);


        SACDInventoryRemoveIllegalItems(tACD);

        SACDInventoryRestoreIllegalItems(tACD);


        if (ptPlayer->dwJoinTime == GameGetServerTime())
        {
            tACD.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
        }

        tACD.MarkHeroStateSavedDataChanged();


        SPlayerUpdateSidekickStatsBoost(tACD);
    }













    tACD.dwFlags &= ~(1 << ACD_QUEUE_EQUIPPED_ITEMS_CHANGED_BIT);

    if (HirelingGetClass(tACD) != 0)
    {
        sACDInventoryHandleSkillChanges(tACD);
    }
}


void ACDInventoryHandleEquipmentOrSkillChanges()
{



    GameCommonData & tGCD = GameGetActiveGameCommonData();
    PlayerGlobals * ptPlayerGlobals = tGCD.ptPlayerGlobals;

    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        Player & tPlayer = ptPlayerGlobals->tPlayerArray[i];

        ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (!ptACDPlayer)
            continue;




        if ((tPlayer.tHeroStateData.uPlayerFlags & ((1 << PLAYERFLAG_HARDCORE) | (1 << PLAYERFLAG_DECEASED))) == ((1 << PLAYERFLAG_HARDCORE) | (1 << PLAYERFLAG_DECEASED)))
            continue;





        if (ptACDPlayer->dwFlags & (1 << ACD_QUEUE_INVENTORY_EQUIPMENT_OR_SKILL_CHANGED_BIT))
        {
            sACDInventoryHandleEquipmentOrSkillChanges(*ptACDPlayer);
        }


        if (ptACDPlayer->dwFlags & (1 << ACD_QUEUE_SKILL_CHANGED_BIT))
        {
            sACDInventoryHandleSkillChanges(*ptACDPlayer);
        }

        ACDID idACDHireling = PlayerGetHireling(tPlayer.idPlayerACD);
        ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
        if (ptACDHireling)
        {
            if (ptACDHireling->dwFlags & (1 << ACD_QUEUE_INVENTORY_EQUIPMENT_OR_SKILL_CHANGED_BIT))
            {
                sACDInventoryHandleEquipmentOrSkillChanges(*ptACDHireling);
            }


            if (ptACDHireling->dwFlags & (1 << ACD_QUEUE_SKILL_CHANGED_BIT))
            {
                sACDInventoryHandleSkillChanges(*ptACDHireling);
            }
        }
    }
}






































































































static void sItemCountTypeCallBack(ACDID idACDSquare, InventorySlotType, void * pUserData)
{


    InventoryCallbackItemCountType * ptItemCount = (InventoryCallbackItemCountType *)pUserData;





    const ActorCommonData * ptSrcACD = ptItemCount->m_ptACDSearchItem;



    const ActorCommonData & tACD = ACDGet(idACDSquare);

    if (tACD.hGB.gbid == ptSrcACD->hGB.gbid ||
        (!ptItemCount->m_fExactMatch && tACD.hGB.eType == ptSrcACD->hGB.eType))
    {
        ptItemCount->m_tItemList.AddTail_Tracked(tACD.id);
        ptItemCount->m_uTotalItemCount += Math::Max<uint64>(1, tACD.GetStackCount());
    }
}



#include "../Server/Loot/Loot.h"
static ActorCommonData * sAddItemToMerchant(ActorCommonData & tACDMerchant,
                                            BOOL fAutoIdentify,
                                            LootSpecifier & tLootSpecifier,
                                            BOOL bInfiniteQuantity,
                                            BOOL bPlayEffects)
{
    if (!tACDMerchant.ptInventory)
    {
        return NULL;
    }







    tLootSpecifier.fAutoIdentify = fAutoIdentify;

    SNO snoInventory = ACDGetInventory(&tACDMerchant, FALSE);


    ACDID idACDItem = LootCreateItem(tLootSpecifier, cgplaceNull, &tACDMerchant);
    if (idACDItem == ACDID_INVALID)
    {
        return NULL;
    }

    ActorCommonData & tACD = ACDGet(idACDItem);
    if (bInfiniteQuantity)
    {
        tACD.SetStackCount(0x8000000000000000ULL);
    }




    InventoryLocation tLoc;
    tLoc.idACDContainer = tACDMerchant.id;
    if (sFindNonFullStack(tACD, tLoc))
    {
        if (bInfiniteQuantity)
        {
            ActorID idActor = (ActorID)ACDGetOwner(idACDItem);
            ActorDelete(idActor);
            return NULL;
        }



        ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDMerchant, tLoc.eSlot, tLoc.pt);

        ActorCommonData * ptACD = ACDTryToGet(idACDSquareItem);

        if (ptACD && ptACD->GetStackCount() == 0x8000000000000000ULL)
        {
            ActorDelete((ActorID)ACDGetOwner(idACDItem));
            return NULL;
        }
    }



    if (tACDMerchant.IsMerchant())
    {
        tACD.AttributesSetInt(ATTRIBUTE_STORE_SNO, snoInventory);
    }

    BOOL bFlagDirty = ACDIsHireling(tACDMerchant) || ACDIsPlayer(tACDMerchant.id);

    BOOL fPickedUp = SACDInventoryPickup(tACDMerchant.id, idACDItem, EFFECT_INVALID, bFlagDirty, FALSE, NULL, bPlayEffects, FALSE, FALSE, TRUE) == PICKUP_RESPONSE_OK;
    if (!fPickedUp)
    {

        ActorID idActor = (ActorID)ACDGetOwner(idACDItem);
        ActorDelete(idActor);
        return NULL;
    }


    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem)
    {
        ptACDItem->AttributesSetInt(ATTRIBUTE_KNOWN_BY_OWNER, tACDMerchant.id);
    }
    return ptACDItem;
}



int32 ACDInventoryGetNumEmptySlots(const ActorCommonData & tACD, InventorySlotType eSlot)
{
    int32 nFilledSpaces = ACDInventoryGetNumFilledSlots(tACD, eSlot);
    IVector2D tSize;
    ACDInventorySlotGetSize(eSlot, tSize);
    return tSize.x * tSize.y - nFilledSpaces;
}






int32 ACDInventoryGetNumFilledSlots(const ActorCommonData & tACD, InventorySlotType eSlot)
{
    int32 nFilledSpaces = 0;
    ACDInventoryIterateItems(tACD, eSlot, 0, sTestSquareFilledCount, &nFilledSpaces);
    return nFilledSpaces;
}





void SACDCreateRandomItems(ActorCommonData & tACDContainer, BOOL fAutoIdentify, InventorySlotType eSlot, SNO snoTreasureClass, BOOL bPlayEffects)





{







    BOOL bInfinite = FALSE;
    BOOL bIsMerchant = tACDContainer.IsMerchant();
    if (bIsMerchant)
    {
        if (eSlot != SLOT_NONE)
        {

            if (ACDInventoryGetNumEmptySlots(tACDContainer, eSlot) < 40)
            {
                return;
            }
        }

        bInfinite = ACDGetInventory(&tACDContainer, FALSE) == snoTreasureClass;
    }





    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDContainer, ACDID_INVALID);
    if (snoTreasureClass != SNO_NONE)
    {
        tLootee.snoTreasureClass = snoTreasureClass;
    }
    LootSpecifierArray arLootSpecifiers;
    BOOL bSuccess = LootGetLootSpecifiers(tLootee, tACDContainer.id, arLootSpecifiers, NULL, FALSE, 1, NULL);

    if (bSuccess)
    {



        uint32 dwSoldSpecialItemFlags = 0;
        int32 nNumItems = arLootSpecifiers.GetArraySize();


        if (bIsMerchant && !bInfinite)
        {
            dwSoldSpecialItemFlags = tACDContainer.AttributesGetInt(ATTRIBUTE_SPECIAL_INVENTORY_HAS_SOLD);
        }

        for (int32 i = 0; i < nNumItems; i++)
        {

            if (!(dwSoldSpecialItemFlags & (1 << i)))
            {
                ActorCommonData * ptACDItem = sAddItemToMerchant(tACDContainer, fAutoIdentify, arLootSpecifiers[i], bInfinite, bPlayEffects);
                if (ptACDItem && !bInfinite)
                {
                    ptACDItem->AttributesSetInt(ATTRIBUTE_SPECIAL_INVENTORY_INDEX, i);
                }
            }
        }
    }
}




void SACDInventoryLoad(ACDID idACDContainer, SNO snoInventory)
{






    ActorCommonData * ptACDContainer = ACDTryToGet(idACDContainer);
    if (snoInventory == SNO_NONE || !ptACDContainer)
        return;




    if (!ptACDContainer->HasInventory())
    {
        ACDInventoryInitialize(*ptACDContainer, TRUE);
        ACDVisualInventoryInitialize(*ptACDContainer);
    }

    if (ptACDContainer->ptInventory)
    {
        SGameGlobals & tGlobals = SGameGlobalsGet();
        uint32 dwPrevSeed = RandGetSeed(tGlobals.rng);
        RandSetSeed_Internal(tGlobals.rng, ptACDContainer->ptInventory->dwSeed);
        SACDCreateRandomItems(*ptACDContainer, TRUE, -1, snoInventory, FALSE);
        RandSetSeed_Internal(tGlobals.rng, dwPrevSeed);
    }






}




void SACDInventoryReload(
    ACDID idACDContainer,
    SNO snoInventoryClass)
{







    ActorCommonData & tACDContainer = ACDGet(idACDContainer);
    InventoryType eType = ACDInventoryGetType(tACDContainer);


    int32 nFirst, nLast, nNumSlots;
    ACDInventoryGetSlotTypeInfo(eType, nFirst, nLast, nNumSlots);


    for (int32 nSlot = nFirst; nSlot < nLast; nSlot++)
    {



        IVector2D tSlotSize;
        ACDInventorySlotGetSize(nSlot, tSlotSize);

        for (int32 y = 0; y < tSlotSize.y; y++)
        {
            for (int32 x = 0; x < tSlotSize.x; x++)
            {
                IVector2D tPoint;
                tPoint.Set(x, y);


                ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDContainer, nSlot, tPoint);
                if (idACDSquareItem != ACDID_INVALID)
                {

                    ActorCommonData & tACDToDelete = ACDGet(idACDSquareItem);


                    ACDRemoveFromCurrentPosition(tACDToDelete);


                    ActorID idActor = (ActorID)ACDGetOwner(tACDToDelete.id);

                    ActorDelete(idActor);
                }
            }
        }
    }


    SACDInventoryLoad(idACDContainer, snoInventoryClass);
}







void SACDInventoryReroll(ActorCommonData & tACDContainer)
{





    ScopedGlobalDefinitionLockR pdefGlobal;

    tACDContainer.AttributesSetInt(ATTRIBUTE_INVENTORY_REROLL_TIME, GameGetServerTime() + pdefGlobal->ptServerData->nMinutesToRerollVendorInventory * TICKS_PER_MINUTE);


    tACDContainer.AttributesSetInt(ATTRIBUTE_SPECIAL_INVENTORY_HAS_SOLD, 0);


    ACDInventoryDeleteAll(tACDContainer);


    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    ActorInventorySetSeed(tACDContainer.ptInventory, Rand_Internal(tSGameGlobals.rng, 0xFFFFFFFD));


    int32 nNewLevel = MonsterGetLevel(tACDContainer.tSNOName);
    tACDContainer.AttributesSetInt(ATTRIBUTE_LEVEL, nNewLevel);


    SNO snoInventory = ACDGetInventory(&tACDContainer, FALSE);
    if (snoInventory != SNO_NONE)
    {
        SACDInventoryLoad(tACDContainer.id, snoInventory);
    }

    snoInventory = ACDGetInventory(&tACDContainer, TRUE);
    if (snoInventory != SNO_NONE)
    {
        SACDInventoryLoad(tACDContainer.id, snoInventory);
    }


    ScriptEventOnActorRerolledInventory(tACDContainer.id);
}





void ACDInventoryLocationSet(
    InventoryLocation & tInvLoc,
    const ACDID idACDContainer,
    const InventorySlotType eSlot,
    const int32 nSlotX,
    const int32 nSlotY)
{
    tInvLoc.idACDContainer = idACDContainer;
    tInvLoc.eSlot = eSlot;
    tInvLoc.pt.Set(nSlotX, nSlotY);
}



InventoryType ACDInventoryGetType(const ActorCommonData & tACD)
{
    if (tACD.ptInventory)
        return tACD.ptInventory->eType;
    return INVENTORY_NONE;
}








void ACDInventoryGetSlotTypeInfo(InventoryType eType, int32 & nStart, int32 & nEnd, int32 & nSlots)
{
    switch (eType)
    {
    case INVENTORY_PLAYER:
        nStart = SLOT_PLAYER_START;
        nEnd = SLOT_PLAYER_END;
        nSlots = NUM_PLAYER_SLOTS;
        break;
    case INVENTORY_MERCHANT:
        nStart = SLOT_MERCHANT_START;
        nEnd = SLOT_MERCHANT_END;
        nSlots = NUM_MERCHANT_SLOTS;
        break;
    case INVENTORY_SOCKETS:
        nStart = SLOT_SOCKETS_START;
        nEnd = SLOT_SOCKETS_END;
        nSlots = NUM_SOCKETS_SLOTS;
        break;
    case INVENTORY_HIRELING:
        nStart = SLOT_HIRELING_START;
        nEnd = SLOT_HIRELING_END;
        nSlots = NUM_HIRELING_SLOTS;
        break;
    case INVENTORY_GENERIC:
        nStart = SLOT_HIRELING_START;
        nEnd = SLOT_HIRELING_END;
        nSlots = NUM_HIRELING_SLOTS;
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




static void sResetForNewSlot(
    const ActorCommonData & tACD,
    const int32 nCurrentSlot,
    int32 & nSquareCount,
    InventorySlot *& ptSlot)
{

    IVector2D tSize;
    ACDInventorySlotGetSize(nCurrentSlot, tSize);
    nSquareCount = tSize.x * tSize.y;

    tACD.ptInventory->mapInventorySlots.Lookup(nCurrentSlot, ptSlot);
}




// NON_MATCHING: 22/206 insns, register allocation only (x11/x12 swapped between the slot-size value and the map/jump-table temporaries); leaving ptSlot uninitialised fixes the registers but loses the `mov x23, xzr` on the first failed lookup
BOOL ACDInventoryIterate(
    InventoryIterator & tIt)
{
    const ActorCommonData & tACD = *tIt.m_ptACD;

    if (!tACD.ptInventory)
        return FALSE;



    int32 nStartSlot;
    int32 nEndSlot;
    int32 nSlotCount;
    sGetInventorySlotInfo(tACD.ptInventory, nStartSlot, nEndSlot, nSlotCount);

    if (tIt.m_nCurrentPosition == -1)
    {

        tIt.m_nCurrentSlot = nStartSlot;
        tIt.m_nCurrentPosition = -1;
    }

    int32 nSquareCount;
    InventorySlot * ptSlot = NULL;
    sResetForNewSlot(tACD, tIt.m_nCurrentSlot, nSquareCount, ptSlot);

    while (TRUE)
    {
        tIt.m_nCurrentPosition++;

        if (tIt.m_nCurrentPosition >= nSquareCount)
        {
            tIt.m_nCurrentSlot++;
            sResetForNewSlot(tACD, tIt.m_nCurrentSlot, nSquareCount, ptSlot);

            tIt.m_nCurrentPosition = 0;

            if (tIt.m_nCurrentSlot >= nEndSlot)
            {
                tIt.m_nCurrentSlot = -1;
                tIt.m_nCurrentPosition = -1;
                tIt.m_idItem = ACDID_INVALID;
                return FALSE;
            }
        }

        const InventorySquare & tSquare = ptSlot->arSquares[tIt.m_nCurrentPosition];
        if (ACDTryToGet(tSquare.idACD))
        {
            tIt.m_idItem = tSquare.idACD;
            return TRUE;
        }
    }
}




#include "../Server/ConversationMgr.h"
#include "../../Contrib/Contrib/blz/include/blz/func_bind.h"
void SItemBroadcastToClan(const ActorCommonData & tACDItem, const Player & tPlayer);  // X/Server/SItems.cpp:1233

static BOOL sChargePlayerForItem(
    Player * ptPlayer,
    ActorCommonData & tACDPlayer,
    ActorCommonData & tACDItem,
    ActorCommonData * ptACDVendor,
    CostType eCostType)
{
    CurrencyType eCurrencyType = ptACDVendor ? ptACDVendor->GetCurrencyType() : CURRENCY_GOLD;







    uint64 uCost = ItemGetCost(tACDItem, eCostType, eCurrencyType, ptACDVendor, TRUE, FALSE);
    if (tACDPlayer.ModifyCurrencyAmount(-(int64)uCost, eCurrencyType, GMR_PURCHASE, FALSE))
    {



        if (ptACDVendor && eCurrencyType == CURRENCY_BLOODSHARDS)
        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);

            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_SPEND_BLOODSHARDS, (uint32)uCost);



        }

        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_BUYING_ITEMS, eCurrencyType, 0, (float)uCost);
        return TRUE;
    }
    return FALSE;
}

blz::pair<uint32, uint32> sGetOverrideSeedForItem(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItem)
{

    uint32 nRetVal = 0;
    uint32 nCarry = 420;

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer)
    {
        ptPlayer->uConsoleSyncFlags |= (1 << PCSF_RANDOM_SEEDS);

        blz::unordered_map<int32, blz::pair<uint32, uint32> >::iterator it = ptPlayer->tMapVendorSeeds.find(tACDItem.hGB.gbid);
        if (it != ptPlayer->tMapVendorSeeds.end())
        {
            return it->second;
        }

        nRetVal = (uint32)ptPlayer->uHeroId;
    }

    return blz::make_pair(nRetVal, nCarry);
}










void sConsoleRNGOutOfScope(
    Player * ptPlayer,
    GBID gbidSlot)
{



    RNG tRNG = SGameGlobalsGet().rng;
    ptPlayer->tMapVendorSeeds[gbidSlot] = blz::pair<uint32, uint32>(tRNG.dwSeed, tRNG.dwCarry);

}

BOOL SACDInventoryBuy(
    ActorCommonData & tACDPlayer,
    ActorCommonData & tACDItem)
{




    if (ACDInventoryCanBuy(tACDPlayer, tACDItem) != BUY_RESPONSE_OKAY)
    {
        return FALSE;
    }




    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);


    ActorCommonData * ptACDVendor = ACDTryToGet(ptPlayer->idACDActiveVendor);
    CostType eCostType = ACDInventoryGetCostType(tACDItem.id, ptPlayer->idACDActiveVendor, ACDID_INVALID);

    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (pdefItem->snoRandomItemTreasureClass != SNO_NONE)
    {


        LooteeInfo tLootee;
        tLootee.InitializeFromACD(*ptACDVendor, ACDID_INVALID);
        tLootee.snoTreasureClass = pdefItem->snoRandomItemTreasureClass;
        tLootee.tLooteeParams.nLevel = Math::Max(tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL), pdefItem->nNormalRequiredLevel);
        tLootee.tLooteeParams.bDisableTormentDrops = FALSE;
        tLootee.tLooteeParams.bDisableTormentCheck = TRUE;
        tLootee.tLooteeParams.bSuppressMagicAndGoldFind = TRUE;
        tLootee.tLooteeParams.bSuppressUnluckyBonus = TRUE;
        tLootee.tLooteeParams.bIgnoreClassLegendaryDropWeight = TRUE;
        tLootee.tLooteeParams.bForceCrafterAncientLegendaryChance = TRUE;

        XPooledDataIDList listLootACDs(MemoryPoolsGetDATAIDListPool());
        {

            blz::pair<uint32, uint32> tSeed = sGetOverrideSeedForItem(tACDPlayer, tACDItem);
            ConsoleScopedOverrideRNG overrideRNG(tSeed.first, tSeed.second,
                blz::bind(&sConsoleRNGOutOfScope, ptPlayer, tACDItem.hGB.gbid));

            LootGiveTreasureToPlayer(tLootee, tACDPlayer, &listLootACDs, LC_RANDOMITEMVENDOR, FALSE, FALSE);
        }
        if (listLootACDs.Count() == 0)
        {
            return FALSE;
        }


        if (!sChargePlayerForItem(ptPlayer, tACDPlayer, tACDItem, ptACDVendor, eCostType))
        {
            return FALSE;
        }


        ItemQualityLevel eHighestIQL = IQL_NORMAL;
        POS pos = listLootACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACDLootItem = (ACDID)listLootACDs.GetNext(pos);
            ActorCommonData * ptACDLootItem = ACDTryToGet(idACDLootItem);
            if (ptACDLootItem)
            {
                SItemCrafting_TryUnlockTransmog(ptACDLootItem->hGB.gbid, *ptPlayer, TRUE, TRUE);


                PlayerList listDestPlayers(PlayerListGetElementPool());
                listDestPlayers.AddTail_Tracked(ptPlayer);

                ANNDataMessage tMessage;
                tMessage.ann = ANNFromACD(idACDLootItem);
                MessageSendToClient(listDestPlayers, SMSG_MYSTERY_ITEM_RESULTS, &tMessage, sizeof(tMessage));


                SItemBroadcastToClan(*ptACDLootItem, *ptPlayer);

                ItemQualityLevel eIQL = ItemGetQualityLevel(*ptACDLootItem);
                eHighestIQL = (eIQL > eHighestIQL) ? eIQL : eHighestIQL;




                if (eIQL == IQL_LEGENDARY || eIQL == IQL_DISPLAY_AS_SET_ITEM)
                {




                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetPlayer(ptPlayer);
                    tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_LEGENDARY_FROM_KADALA, 1);
                }

            }
        }


        SNO snoConversation = SNO_NONE;
        if (eHighestIQL == IQL_LEGENDARY || eHighestIQL == IQL_DISPLAY_AS_SET_ITEM)
        {
            snoConversation = GlobalSNOGet(GLOBALSNO_CONVERSATION_BOUGHT_RANDOM_ITEM_LEGENDARY);
        }
        else if (eHighestIQL >= IQL_RARE_AFFIX_START && eHighestIQL <= IQL_RARE_AFFIX_END)
        {
            snoConversation = GlobalSNOGet(GLOBALSNO_CONVERSATION_BOUGHT_RANDOM_ITEM_RARE);
        }
        else
        {
            snoConversation = GlobalSNOGet(GLOBALSNO_CONVERSATION_BOUGHT_RANDOM_ITEM_BAD);
        }
        ConversationMgr::StartConversation(ptPlayer->nPlayerIndex, snoConversation, ptACDVendor->id, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, 0x7fffffff, FALSE, FALSE);

        return TRUE;
    }

    ActorCommonData * ptACDItemToPickup = &tACDItem;



    if (tACDItem.tLocation.tInvLocation.eSlot != SLOT_PLAYER_BUYBACK)
    {
        uint64 uStackSize = tACDItem.GetStackCount();
        if (uStackSize > 1)
        {
            D3::Items::Generator tGenerator;
            ItemGeneratorToDefaults(tGenerator);
            ItemGeneratorPopulateFromACDID(tACDItem, tGenerator, TRUE);

            ACDID idACDSplitItem = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);






            tACDItem.SubtractFromStackCount(1);


            ptACDItemToPickup = ACDTryToGet(idACDSplitItem);
            if (ptACDItemToPickup)
            {
                ptACDItemToPickup->SetStackCount(1);


                if (ItemGetUseType(*ptACDItemToPickup) == ITEM_USE_TREASURE_BAG)
                {
                    ptACDItemToPickup->AttributesSetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE, tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL));
                }
            }
        }
        else if (ptACDVendor && ptACDVendor->IsMerchant())
        {



            int32 nSpecialInventoryIndex = tACDItem.AttributesGetInt(ATTRIBUTE_SPECIAL_INVENTORY_INDEX);
            if (nSpecialInventoryIndex >= 0)
            {
                uint32 dwSoldSpecialItemFlags = ptACDVendor->AttributesGetInt(ATTRIBUTE_SPECIAL_INVENTORY_HAS_SOLD);







                dwSoldSpecialItemFlags |= (1 << nSpecialInventoryIndex);
                ptACDVendor->AttributesSetInt(ATTRIBUTE_SPECIAL_INVENTORY_HAS_SOLD, dwSoldSpecialItemFlags);
                tACDItem.AttributesSetInt(ATTRIBUTE_SPECIAL_INVENTORY_INDEX, -1);
            }
        }



        if (ptACDVendor && ptACDVendor->IsMerchant())
        {
            if (ItemGetBoundToACD(tACDItem, TRUE) == ptACDVendor->id)
            {
                ItemSetBoundToACD(tACDItem, ACDID_INVALID);
            }


            ptACDItemToPickup->AttributesSetInt(ATTRIBUTE_IS_VENDOR_BOUGHT, 1);
        }
    }

    if (!sChargePlayerForItem(ptPlayer, tACDPlayer, *ptACDItemToPickup, ptACDVendor, eCostType))
    {
        return FALSE;
    }

    SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_ITEM_BOUGHT, ptACDItemToPickup->hGB.gbid, ItemGetStatsDataKey(*ptACDItemToPickup), 1.0f);



    if (eCostType != 2)
    {




        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);
        tEvaluationData.SetACD(ptACDItemToPickup);

        tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_ITEM_PURCHASED,


            ptACDItemToPickup->hGB.gbid, 1);

    }


    PickupResponse ePickupResponse = SACDInventoryPickup(tACDPlayer.id, ptACDItemToPickup->id, EFFECT_INVALID, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE);








    return TRUE;
}



void sMoveItemToBuybackSlot(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem)
{



    if (GameActiveGameCommonDataIsServer())
    {
        uint32 dwNow = GameGetServerTime();
        tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_SOLD_TICK, dwNow);
    }


    InventorySlotType eCameFrom = tACDItem.tLocation.tInvLocation.eSlot;
    sACDRemoveFromCurrentPosition(tACDItem);

    IVector2D tMaxSize;
    ACDInventorySlotGetSize(SLOT_PLAYER_BUYBACK, tMaxSize);


    IVector2D tInsertionPoint(0, 0);
    BOOL fFoundEmptySquare = FALSE;
    for (int32 i = 0; i < tMaxSize.x; i++)
    {
        tInsertionPoint.x = i;
        ACDID idACDSquare = ACDInventoryGetSquareItem(tACDPlayer, SLOT_PLAYER_BUYBACK, tInsertionPoint);
        if (idACDSquare == ACDID_INVALID)
        {
            fFoundEmptySquare = TRUE;
            break;
        }
    }

    if (!fFoundEmptySquare)
    {



        ACDID idACDSquare = ACDInventoryGetSquareItem(tACDPlayer, SLOT_PLAYER_BUYBACK, IVector2D(0, 0));
        ActorCommonData * ptACDItemInSquare = ACDTryToGet(idACDSquare);
        if (ptACDItemInSquare)
        {
            ACDInventoryDelete(*ptACDItemInSquare, TRUE);
        }
        sShiftBuybackItemsDown(tACDPlayer.id, 0);
        tInsertionPoint.x = tMaxSize.x - 1;
    }

    InventoryLocation tLocation;
    tLocation.eSlot = SLOT_PLAYER_BUYBACK;
    tLocation.idACDContainer = tACDPlayer.id;
    tLocation.pt = tInsertionPoint;
    sACDAddToInventory(tACDItem, tLocation, TRUE, eCameFrom, FALSE);
}





static BOOL sACDInventorySellShared(ActorCommonData * ptACDVendor, ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const BOOL fAllowBuyback)
{









    if (ItemCanBeSold(tACDItem, ptACDVendor) != ISE_NONE)
        return FALSE;



    CurrencyType eCurrencyType = ptACDVendor->GetCurrencyType();
    uint64 uCost = ItemGetCost(tACDItem, 0, eCurrencyType, ptACDVendor, TRUE, FALSE);
    if (!tACDPlayer.ModifyCurrencyAmount(uCost, eCurrencyType, GMR_SOLD, FALSE))
        return FALSE;



    if (tACDItem.IsInInventory())
    {
        InventoryLocation tInvLocCur = tACDItem.GetInventoryLocation();
        Player * ptPlayer = PlayerGetByACD(tInvLocCur.idACDContainer);
        if (ptPlayer)
        {
            PlayerClearPotionBtnForMissingItem(ptPlayer, tACDItem.id);
        }

        InventorySlotType eSlotCurrent = tInvLocCur.eSlot;

        if ((eSlotCurrent >= SLOT_PLAYER_BODYSLOT_START && eSlotCurrent <= SLOT_PLAYER_BODYSLOT_END) ||
            (eSlotCurrent >= SLOT_HIRELING_START && eSlotCurrent <= SLOT_HIRELING_END))
        {
            PowerAPI_BuffApplyOnEquipmentChangeEffects(tInvLocCur.idACDContainer, eSlotCurrent, 1);
        }



        if (ItemGetUseType(tACDItem) == ITEM_USE_COLLECTORS_EDITION_BUFF)
        {
            GBID gbidCosmetic = ItemGetCosmeticFromItem(tACDItem);
            if (gbidCosmetic != GBID_NONE)
            {
                VisualInventory * ptVisualInventory = VisualInventoryGet(tInvLocCur.idACDContainer);
                if (ptVisualInventory)
                {
                    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++)
                    {
                        if (ptVisualInventory->tEquipmentCurrent.tCosmeticItems[i].gbidItem == gbidCosmetic)
                        {
                            VisualInventorySetCosmeticItemInSlot(tInvLocCur.idACDContainer, (VisualCosmeticItemSlot)i, GBID_NONE);
                        }
                    }
                }
            }
        }
    }


    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    PlayerRemoveItemFromTradeOffer(ptPlayer, tACDItem);

    SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_ITEM_VENDORED, tACDItem.hGB.gbid, ItemGetStatsDataKey(tACDItem), 1.0f);

    SGameStatsRecordGoldObtainedThroughSelling(tACDPlayer, uCost);





    if (fAllowBuyback)
    {
        sMoveItemToBuybackSlot(tACDPlayer, tACDItem);
        tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK, 0);
    }
    PowerAPI_BuffApplyOnItemSoldEffect(tACDPlayer.id, tACDItem);






    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);

    SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_CURRENCY_RECEIVED_FROM_VENDORING_ITEMS, eCurrencyType, 0, (float)uCost);


    sSendCurrencyFloatingAmount(ptPlayer, tACDPlayer.GetWorldPlaceCenter(), (int32)uCost, eCurrencyType);

    return TRUE;
}





BOOL SACDInventorySell(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem)
{


    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);

    ActorCommonData * ptACDVendor = ACDTryToGet(ptPlayer->idACDActiveVendor);
    if (!ptACDVendor)
        return FALSE;











    return sACDInventorySellShared(ptACDVendor, tACDPlayer, tACDItem, TRUE);
}




BOOL SACDInventorySellStackPortion(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const uint64 uAmount)
{





    ACDID idACDSplitItem = sSplitStack(tACDItem, uAmount);

    ActorCommonData & tACDNewItem = ACDGet(idACDSplitItem);
    return SACDInventorySell(tACDPlayer, tACDNewItem);
}










BOOL SACDInventoryBuyStashSlots(const ACDID idACDPlayerActor)
{




    ActorCommonData & tACDPlayer = ACDGet(idACDPlayerActor);















    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);


    int32 nCost = ACDInventoryGetStashUpgradeCost(tACDPlayer);

    if (nCost == 0)
        return FALSE;



    CurrencyType eCurrencyType = CURRENCY_GOLD;
    if (!tACDPlayer.CanAffordCurrencyCost(nCost, eCurrencyType))
        return FALSE;



    if (!tACDPlayer.ModifyCurrencyAmount(-nCost, eCurrencyType, GMR_BUY_STASH, FALSE))
        return FALSE;




    SGameStatsRecordGoldSpent(tACDPlayer, nCost);

    int32 nSlotCount = tACDPlayer.AttributesGetInt(ATTRIBUTE_SHARED_STASH_SLOTS);
    int32 nMaxSlots = 7 * 10 * MAX_STASH_TABS_GOLD_PURCHASABLE;














    int32 nUpgradeAmount = 20;

    if (nMaxSlots - nSlotCount < nUpgradeAmount)
    {
        nSlotCount = nMaxSlots;
    }
    else
    {
        nSlotCount += nUpgradeAmount;
    }

    if (nSlotCount > nMaxSlots - 20)
    {
        nSlotCount = nMaxSlots;
    }







    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);

        tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_STASH_SIZE_INCREASED, 1);



    }

    tACDPlayer.AttributesSetInt(ATTRIBUTE_SHARED_STASH_SLOTS, nSlotCount);





    return TRUE;
}


BOOL SACDInventoryRewardStashTab(const ACDID idACDPlayerActor)
{




















































    return TRUE;
}




GameError ACDInventoryCanMove(const ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, ACDID * pidACDItemInSlot)
{


    ACDID idTemp;
    if (!pidACDItemInSlot)
        pidACDItemInSlot = &idTemp;




    GameError eResult = ACDInventoryItemAllowedInSlot(tACDItem, tInvLocDest, FALSE, FALSE);
    if (eResult != GAMEERROR_NONE)
        return eResult;




    InventoryOverlapList tList;
    ACDInventoryTestOverlap(tACDItem, tInvLocDest, tList);


    int32 nCount = tList.Count();
    if (nCount == 0)
    {
        *pidACDItemInSlot = ACDID_INVALID;
    }
    else if (nCount == 1)
    {

        ACDID idACDInSlot = tList.GetHead();
        *pidACDItemInSlot = idACDInSlot;


        ActorCommonData * ptACDInSlot = ACDTryToGet(idACDInSlot);
        if (ptACDInSlot)
        {
            InventoryLocation tInvLocSource = tACDItem.GetInventoryLocation();
            if (tInvLocSource.idACDContainer != ACDID_INVALID)
            {



                GameError eResultItemInSlot = ACDInventoryItemAllowedInSlot(*ptACDInSlot, tInvLocSource, FALSE, FALSE);
                if (eResultItemInSlot != GAMEERROR_NONE &&







                    ActorInventoryIsVariableSizedSlot(tInvLocSource.eSlot) &&
                    ItemIsLargeSlotItem(ptACDInSlot->hGB.gbid) &&
                    !ItemIsLargeSlotItem(tACDItem.hGB.gbid))
                {
                    tInvLocSource.pt.y--;


                    ACDInventoryTestOverlap(*ptACDInSlot, tInvLocSource, tList);
                    if (tList.Count() <= 1)

                        eResultItemInSlot = ACDInventoryItemAllowedInSlot(*ptACDInSlot, tInvLocSource, FALSE, FALSE);
                }
                eResult = eResultItemInSlot;
            }
        }
    }
    else
    {
        eResult = (GameError)25;  // GAMEERROR_IARESULT_UNKNOWN (Items.cpp; not in the adopted Player.h subset)
    }

    return eResult;
}



BuyResponse ACDInventoryCanBuy(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem)
{


    const Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);


    if (tACDItem.tLocation.eActorLocation != AL_INVENTORY)
        return BUY_RESPONSE_ITEM_NOT_AT_VENDOR;

    const InventoryLocation & tLocation = tACDItem.tLocation.tInvLocation;

    const ActorCommonData * ptACDVendor = ACDTryToGet(ptPlayer->idACDActiveVendor);


    BOOL fInBuybackSlot = tLocation.idACDContainer == tACDPlayer.id && tLocation.eSlot == SLOT_PLAYER_BUYBACK;
    if (tLocation.idACDContainer != ptPlayer->idACDActiveVendor && !fInBuybackSlot)
        return BUY_RESPONSE_ITEM_NOT_AT_VENDOR;




    CurrencyType eCurrencyType = ptACDVendor ? ptACDVendor->GetCurrencyType() : CURRENCY_GOLD;

    {
        BOOL fConsiderStackSize = fInBuybackSlot;

        CostType eCostType = ACDInventoryGetCostType(tACDItem.id, tLocation.idACDContainer, ACDID_INVALID);



        uint64 uCost = ItemGetCost(tACDItem, eCostType, eCurrencyType, ptACDVendor, fConsiderStackSize, FALSE);
        if (!tACDPlayer.CanAffordCurrencyCost(uCost, eCurrencyType))
            return BUY_RESPONSE_CANT_AFFORD;
    }















    if (ACDInventoryCanPickup(tACDPlayer, tACDItem, FALSE, NULL, FALSE, TRUE) != PICKUP_RESPONSE_OK)
        return BUY_RESPONSE_CANT_PICKUP;



    return BUY_RESPONSE_OKAY;
}

// NON_MATCHING: 4396 vs 4180 bytes. Top half (up to the unique-item check) matches. In the inlined sACDInventoryChooseBestSlot ours merges the
// sTestTwoHandedSlotConflict switch with the ACDInventorySlotIsEquippable switch into one jump table (the original keeps two), and tests the autoequip pref with ldrb (original ldr w + tbz).
static void sACDInventoryGetItemsOfTypeItem(const ActorCommonData & tACDPickee, InventorySlotType * peSlots, InventoryCallbackItemCountType * ptItemCount); BOOL ActorInventoryAllowedToPickupItem(const ActorCommonData & tACDPickee, const ActorCommonData & tACDItem);
PickupResponse ACDInventoryCanPickup(
    const ActorCommonData & tACDPickee,
    const ActorCommonData & tACDItem,
    BOOL bTryStashFirst,
    PickupResult * ptResult,
    BOOL bTryStashOnly,
    BOOL bAllowAutoEquip)
{
    ActorLocationType eItemLocation = tACDItem.GetLocationType();
    PickupResponse ePickupResponse = PICKUP_RESPONSE_OK;
    PickupResult tResult;
    tResult.ePickupAction = PICKUP_ACTION_NONE;


    if (TradeIsTrading(tACDPickee.id) && eItemLocation == AL_WORLD)
    {
        ePickupResponse = PICKUP_RESPONSE_TRADING;
        goto done;
    }


    if (ItemIsCurrency(tACDItem))
    {
        tResult.ePickupAction = PICKUP_ACTION_CURRENCY;
        goto done;
    }

    if (ItemIsGlyph(tACDItem))
    {
        tResult.ePickupAction = PICKUP_ACTION_GLYPH;
        goto done;
    }

    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_USE_ON_PICKUP))
    {
        tResult.ePickupAction = PICKUP_ACTION_USE_ON_PICKUP;
        goto done;
    }

    if (ItemIsQuest(tACDItem))
    {
        tResult.ePickupAction = PICKUP_ACTION_QUEST;
        goto done;
    }


    {
        Player * ptPetOwner = tACDPickee.GetPetOwnerPlayer();
        ACDID idACDUltimateOwner = ptPetOwner ? ptPetOwner->idPlayerACD : tACDPickee.id;


        {
            ACDID idACDBoundTo = ItemGetBoundToACD(tACDItem, TRUE);
            BOOL bIsNPC = FALSE;
            ActorCommonData * ptACDBoundTo = ACDTryToGet(idACDBoundTo);
            if (ptACDBoundTo)
            {
                bIsNPC = ptACDBoundTo->IsNPC();
            }

            if (idACDBoundTo != ACDID_INVALID && idACDUltimateOwner != idACDBoundTo && !bIsNPC)
            {
                ePickupResponse = PICKUP_RESPONSE_OWNED_BY_OTHER;
                goto done;
            }





            else if (idACDBoundTo == ACDID_INVALID &&
                     GameActiveGameCommonDataIsClient() &&
                     tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_BOUND_TO_ACD) != ACDID_INVALID)
            {
                ePickupResponse = PICKUP_RESPONSE_OWNED_BY_OTHER;
                goto done;
            }
        }




        if (ItemIsLockedFromTrade(tACDItem))
        {
            ACDID idACDLockedTo = ItemGetLockedToACD(tACDItem);
            if (idACDLockedTo != ACDID_INVALID && idACDUltimateOwner != idACDLockedTo)
            {
                ePickupResponse = PICKUP_RESPONSE_OWNED_BY_OTHER;
                goto done;
            }




            else if (idACDLockedTo == ACDID_INVALID &&
                     GameActiveGameCommonDataIsClient() &&
                     tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LOCKED_TO_ACD) != ACDID_INVALID)
            {
                ePickupResponse = PICKUP_RESPONSE_OWNED_BY_OTHER;
                goto done;
            }
        }
    }


    if (!ActorInventoryAllowedToPickupItem(tACDPickee, tACDItem))
        ePickupResponse = PICKUP_RESPONSE_OWNED_BY_OTHER;

    if (ePickupResponse != PICKUP_RESPONSE_OK)
        goto done;


    if (ItemIsUnique(tACDItem) &&

        ACDIsPlayer(tACDPickee.id))
    {
        InventorySlotType eSlots[24];
        int32 nCurrIndex = 0;
        for (int32 i = SLOT_PLAYER_START; i < SLOT_PLAYER_END; i++)
        {
            eSlots[nCurrIndex++] = (InventorySlotType)i;
        }
        eSlots[nCurrIndex] = SLOT_NONE;

        InventoryCallbackItemCountType tItemCount(TRUE, &tACDItem);

        sACDInventoryGetItemsOfTypeItem(tACDPickee, eSlots, &tItemCount);
        if (tItemCount.m_tItemList.Count() > 0 &&
            tItemCount.m_tItemList.GetHead() != tACDItem.id)
        {
            ePickupResponse = PICKUP_RESPONSE_SINGULAR;
            goto done;
        }
    }



    tResult.tNewLocation.idACDContainer = tACDPickee.id;


    sACDInventoryChooseBestSlot(tResult, tACDItem, bTryStashFirst, bTryStashOnly, bAllowAutoEquip);
    if (tResult.tNewLocation.eSlot == SLOT_NONE)
    {
        ePickupResponse = PICKUP_RESPONSE_NO_SUITABLE_SLOT_FOUND;
    }


    if (tACDItem.AttributesGetInt(ATTRIBUTE_FLIPPY_ID) != -1)
    {
        ePickupResponse = PICKUP_RESPONSE_NOT_RIGHT_NOW;
    }


done:
    if (ptResult)
    {
        *ptResult = tResult;
    }
    return ePickupResponse;
}



BOOL ACDInventoryIsSlotHireling(InventorySlotType eSlot)
{
    return eSlot >= SLOT_HIRELING_START && eSlot < SLOT_HIRELING_END;
}









BOOL ACDInventoryIsSlotMerchant(InventorySlotType eSlot)
{
    return eSlot >= SLOT_MERCHANT_START && eSlot < SLOT_MERCHANT_END;
}









BOOL ACDInventoryIsSlotPlayer(InventorySlotType eSlot)
{
    return eSlot >= SLOT_PLAYER_START && eSlot < SLOT_PLAYER_END;
}









BOOL ACDInventoryIsSlotSocket(InventorySlotType eSlot)
{
    return eSlot == SLOT_ITEM_SOCKETS;
}










BOOL ACDInventorySlotIsEquippable(InventorySlotType eSlot)
{
    switch (eSlot)
    {
    case SLOT_PLAYER_HEAD:
    case SLOT_PLAYER_TORSO:
    case SLOT_PLAYER_RIGHT_HAND:
    case SLOT_PLAYER_LEFT_HAND:
    case SLOT_PLAYER_HANDS:
    case SLOT_PLAYER_WAIST:
    case SLOT_PLAYER_FEET:
    case SLOT_PLAYER_SHOULDERS:
    case SLOT_PLAYER_LEGS:
    case SLOT_PLAYER_BRACERS:
    case SLOT_PLAYER_RIGHT_FINGER:
    case SLOT_PLAYER_LEFT_FINGER:
    case SLOT_PLAYER_NECK:
    case SLOT_HIRELING_RIGHT_HAND:
    case SLOT_HIRELING_LEFT_HAND:
    case SLOT_HIRELING_SPECIAL:
    case SLOT_HIRELING_NECK:
    case SLOT_HIRELING_RIGHT_FINGER:
    case SLOT_HIRELING_LEFT_FINGER:
        return TRUE;
    }
    return FALSE;
}





static void sACDInventoryGetItemsOfTypeItem(const ActorCommonData & tACDPickee,
                                            InventorySlotType * peSlots,
                                            InventoryCallbackItemCountType * ptItemCount)
{
    int32 nIndex = 0;
    while (peSlots[nIndex] != SLOT_NONE)
    {
        ACDInventoryIterateItems(tACDPickee, peSlots[nIndex], 0, sItemCountTypeCallBack, ptItemCount);
        nIndex++;
    }
}














static void sCountItemsCallBack(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    CountItems * ptCountItems = (CountItems *)pUserData;
    const ActorCommonData & tACD = ACDGet(idACDSquare);


    if (tACD.hGB == ptCountItems->hGBItem)
    {
        ItemQualityLevel eIQL = ItemGetQualityLevel(tACD);
        if (!ItemIsEquippable(tACD.hGB.gbid) || !ptCountItems->bWhiteEquippableOnly || eIQL == IQL_NORMAL || eIQL == IQL_SUPERIOR)
        {
            ptCountItems->uItemCount += Math::Max<uint64>(1, tACD.GetStackCount());
        }
    }
    if (ptCountItems->bCheckSockets && ItemHasSockets(tACD))
        ACDInventoryIterateItems(tACD, SLOT_ITEM_SOCKETS, 0, sCountItemsCallBack, pUserData);
}
















int32 ACDInventoryGetItemCount(const ActorCommonData & tACD, const GBHandle & hGBItem, const InventorySlotType * peSlots, BOOL bWhiteEquippableOnly, BOOL bCheckSockets)
{
    if (ItemIsCurrency(hGBItem.gbid))
    {
        CurrencyType eCurrency = CurrencyFromGBID(hGBItem.gbid);
        return (int32)tACD.GetCurrencyAmount(eCurrency);
    }

    CountItems tCountItems;
    tCountItems.uItemCount = 0;
    tCountItems.hGBItem = hGBItem;
    tCountItems.bWhiteEquippableOnly = bWhiteEquippableOnly;
    tCountItems.bCheckSockets = bCheckSockets;
    for (int32 nIndex = 0; peSlots[nIndex] != SLOT_NONE; nIndex++)
    {
        ACDInventoryIterateItems(tACD, peSlots[nIndex], 0, sCountItemsCallBack, &tCountItems);
    }
    if (tCountItems.uItemCount > 0x7fffffff)
        tCountItems.uItemCount = 0x7fffffff;
    return (int32)tCountItems.uItemCount;
}


























static BOOL sFindBestItemCallBack(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    if (pCallBackData == NULL)
        return FALSE;
    ActorCommonData * ptACDContainer = ACDTryToGet(tInvLoc.idACDContainer);
    if (ptACDContainer == NULL)
        return FALSE;

    ACDID idACDSquareItem = ACDInventoryGetSquareItem(*ptACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    FindItem * ptBestItem = (FindItem *)pCallBackData;
    if (idACDSquareItem != ACDID_INVALID)
    {
        const ActorCommonData * ptACD = ACDTryToGet(idACDSquareItem);
        if (ptACD && ptACD->hGB == ptBestItem->hGBItem)
        {
            ItemQualityLevel eIQL = ItemGetQualityLevel(*ptACD);
            if (!ItemIsEquippable(ptACD->hGB.gbid) || !ptBestItem->bWhiteEquippableOnly || eIQL == IQL_NORMAL || eIQL == IQL_SUPERIOR)
            {
                uint64 uCount = Math::Max<uint64>(1, ptACD->GetStackCount());
                if (uCount < ptBestItem->uCount)
                {
                    ptBestItem->idACDItem = idACDSquareItem;
                    ptBestItem->uCount = uCount;
                }
            }
        }
    }
    return TRUE;
}
























void ACDInventoryGetItem(const ActorCommonData & tACDPlayer, const GBHandle & hGBItem, const InventorySlotType * peSlots, ACDID & idACDItem, int32 & nCount, BOOL bWhiteEquippableOnly)
{
    FindItem tBestItem;
    tBestItem.hGBItem = hGBItem;
    tBestItem.idACDItem = ACDID_INVALID;
    tBestItem.uCount = 0x8000000000000000ULL;
    tBestItem.bWhiteEquippableOnly = bWhiteEquippableOnly;
    for (int32 nIndex = 0; peSlots[nIndex] != SLOT_NONE; nIndex++)
    {
        ACDInventoryIterateSlot(tACDPlayer, peSlots[nIndex], NULL, sFindBestItemCallBack, &tBestItem, FALSE, FALSE);
    }
    idACDItem = tBestItem.idACDItem;
    if (idACDItem == ACDID_INVALID)
    {
        nCount = 0;
    }
    else
    {
        if (tBestItem.uCount > 0x7fffffff)
            tBestItem.uCount = 0x7fffffff;
        nCount = (int32)tBestItem.uCount;
    }
}






















BOOL ACDInventoryDelete(ActorCommonData & tACDItem, BOOL bMarkItemAsDirty)
{






    if (GameActiveGameCommonDataIsClient())
        return TRUE;
    if (bMarkItemAsDirty)
    {
        SACDInventoryAddToDirtyList(tACDItem, AIDA_REMOVE, tACDItem.GetLocationSlot(), SLOT_NONE);
        const Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);
        (void)ptActor;
    }




    sDeleteSocketedGems(tACDItem, FALSE);


    ACDRemoveFromCurrentPosition(tACDItem);

    ActorID idActorItem = (ActorID)ACDGetOwner(tACDItem.id);

    ActorDelete(idActorItem);




    return TRUE;
}



WeaponClass ACDGetCurrentWeaponClass(const ActorCommonData & tACD)
{
    if (ACDIsPlayer(tACD.id))
    {
        VisualInventorySlot eVisualSlotLeftHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_LEFT_HAND);
        VisualInventorySlot eVisualSlotRightHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_RIGHT_HAND);
        GBID gbidItemRight = GBID_NONE;
        GBID gbidItemLeft = GBID_NONE;
        const VisualInventory * ptVisualInventory = VisualInventoryGet(tACD.id);
        if (ptVisualInventory)
        {
            const VisualItem & tVisualItemLeftHand = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotLeftHand];
            const VisualItem & tVisualItemRightHand = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotRightHand];
            gbidItemLeft = tVisualItemLeftHand.gbidItem;
            gbidItemRight = tVisualItemRightHand.gbidItem;
        }
        WeaponClass eWeaponClass = GetWeaponClass(gbidItemLeft, gbidItemRight);
        return eWeaponClass;
    }
    return (WeaponClass)tACD.AttributesGetInt(ATTRIBUTE_CURRENT_WEAPONCLASS);
}












WeaponClass GetWeaponClass(const GBID gbidItemLeft, const GBID gbidItemRight)
{
    GBID gbidItem = gbidItemLeft != GBID_NONE ? gbidItemLeft : gbidItemRight;
    if (gbidItem == GBID_NONE)
        return WEAPONCLASS_NONE;
    GBHandle hGB(GB_ITEMS, gbidItem);
    SNO snoActorItem = GameBalanceGetActorSNO(hGB);
    ScopedActorDefinitionLockR pItemDef(snoActorItem);
    WeaponClass eWeaponClass = (WeaponClass)TagMapGetInt(pItemDef->hTagMap, TAG_ACTOR_WEAPON_CLASS);
    return eWeaponClass;
}
































BOOL ACDInventoryCanUseItem(
    const ActorCommonData & tACDUser,
    const ActorCommonData & tACDItem,
    GameError * peOutError)
{
    if (peOutError)
    {
        *peOutError = GAMEERROR_NONE;
    }

    GBID gbidItem = tACDItem.hGB.gbid;


    if (!tACDUser.IsAlive())
        return FALSE;



    PlayerClass ePlayerClass = PlayerGetPlayerClass(tACDUser.id);
    if (!ItemMeetsOwnerClassRestrictions(gbidItem, ePlayerClass, FALSE))
        return FALSE;



    int32 nRequiredLevel = ItemGetLevelRequirement(gbidItem, &tACDItem, FALSE);

    int32 nOwnerLevel = tACDUser.AttributesGetInt(ATTRIBUTE_LEVEL);
    if (nOwnerLevel < nRequiredLevel)
        return FALSE;



    uint32 dwFailureReasons = PowerACDCanStartPowerFailureReasons(tACDUser, AutoPowerDefinitionR(ItemGetUsePower(gbidItem)), NULL, FALSE, FALSE);
    if (dwFailureReasons)
        return FALSE;












    if (ItemHasLabel(gbidItem, ITL_TOWNPORTALSTONE))
    {
        if (!GameIsParamFlagSet(GPF_ALLOW_TOWN_PORTALS))
            return FALSE;




        if (tACDUser.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
            return FALSE;
    }



    switch (ItemGetUseType(tACDItem))
    {
    case ITEM_USE_POWER_SCROLL:
    case ITEM_USE_SCROLL:
        {
            SNO snoPower = ItemUseGetScrollPower(tACDItem);
            if (snoPower == SNO_NONE)
                return FALSE;



            ScopedPowerDefinitionLockR pdefPower(snoPower);
            if (!pdefPower)
                return FALSE;



            BuffExclusiveType eStartBuffType = PowerGeneralTagGetInt(pdefPower, TAG_POWER_SCROLL_BUFF_EXCLUSIVE_TYPE);
            if (eStartBuffType != BUFF_EXCLUSIVE_TYPE_NONE &&
                tACDUser.AttributesGetInt(MakeAttribKey(ATTRIBUTE_BUFF_EXCLUSIVE_TYPE_ACTIVE, eStartBuffType)))
            {


                if (peOutError)
                    *peOutError = cgtPowerScrollBuffExclusiveTypeErrors[eStartBuffType];
                return FALSE;
            }
            break;
        }
    default:
        break;
    }




    return TRUE;
}



BOOL ACDInventoryItemShouldBeGhosted(const ActorCommonData & tACDUser, const ActorCommonData & tACDItem)
{

    ItemUseType eUseType = ItemGetUseType(tACDItem);
    if (eUseType == ITEM_USE_POWER_SCROLL || eUseType == ITEM_USE_SCROLL)
    {



        SNO snoPower = ItemUseGetScrollPower(tACDItem);
        if (snoPower == SNO_NONE)
            return FALSE;



        ScopedPowerDefinitionLockR pdefPower(snoPower);
        if (pdefPower)
        {


            BuffExclusiveType eStartBuffType = PowerGeneralTagGetInt(pdefPower, TAG_POWER_SCROLL_BUFF_EXCLUSIVE_TYPE);
            if (eStartBuffType != BUFF_EXCLUSIVE_TYPE_NONE &&
                tACDUser.AttributesGetInt(MakeAttribKey(ATTRIBUTE_BUFF_EXCLUSIVE_TYPE_ACTIVE, eStartBuffType)))
                return TRUE;
        }



    }






    return FALSE;
}




BOOL ACDCanDualWield(
    const ActorCommonData & tACD)
{
    if (ACDIsPlayer(tACD.id))
    {
        PlayerClass ePlayerClass = PlayerGetPlayerClass(tACD.id);
        return PlayerClassCanDualWield(ePlayerClass);
    }
    return !ACDIsHireling(tACD);
}









BOOL ACDInventorySlotContainsOneHandedWeapon(const ActorCommonData & tACD, const InventorySlotType eSlot, BOOL bUseVisualInventory)
{



    VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eSlot);
    if (eVisualSlot == VIS_INVALID)
        return FALSE;

    GBID gbidItem = GBID_NONE;


    if (bUseVisualInventory)
    {
        const VisualInventory * ptVisualInventory = tACD.ptVisualInventory;
        if (!ptVisualInventory)
            return FALSE;
        {
            const VisualItem & tVisualItem = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlot];
            gbidItem = tVisualItem.gbidItem;
        }
    }
    else {
        ACDID idACDItemInSlot = ACDInventoryGetSquareItem(tACD, eSlot, cgptNull);
        ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
        if (!ptACDItemInSlot)
            return FALSE;
        gbidItem = ptACDItemInSlot->hGB.gbid;
    }


    WeaponClass eClass = GetWeaponClass(gbidItem, GBID_NONE);
    if (eClass == WEAPONCLASS_1HSWING || eClass == WEAPONCLASS_1HTHRUST || eClass == WEAPONCLASS_1HFIST || eClass == WEAPONCLASS_HANDXBOW)
        return TRUE;







    return FALSE;
}






























void ACDInventoryRestoreMail(
    const ActorCommonData & tACD,
    const D3::Items::Mails & tMails)
{

    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    MailRecordMap & tMailMap = *ptActorInventoryGlobals->ptMailMap;

    for (int i = 0; i < tMails.mails_size(); i++)
    {
        const D3::Items::Mail & tMail = tMails.mails(i);

        MailRecordMap::iterator iter = tMailMap.find(tMail.mail_id());
        ActorInventoryMailRecord * ptMailRec;
        if (iter == tMailMap.end())
        {
            ActorInventoryMailRecord tmp;
            blz::pair<MailRecordMap::iterator, bool> insert_pair = tMailMap.insert(blz::make_pair(tMail.mail_id(), tmp));

            ptMailRec = &insert_pair.first->second;
            ptMailRec->bDirty = false;
        }
        else
        {
            ptMailRec = &iter->second;
        }
        ptMailRec->Import(tMail);
    }




}





void ACDInventoryDeleteMail(ActorCommonData & tACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return;




    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    MailRecordMap * ptMailMap = ptActorInventoryGlobals->ptMailMap;

    for (MailRecordMap::iterator iter = ptMailMap->begin(); iter != ptMailMap->end(); )
    {
        const ActorInventoryMailRecord & tMailRecord = iter->second;
        if (tMailRecord.tAccountTo == ptPlayer->tAccount.tHandle.m_id)
        {
            iter = ptMailMap->erase(iter);
        }
        else
        {
            iter++;
        }
    }
}





BOOL ACDIsDualWielding(const ActorCommonData & tACD, BOOL bUseVisualInventory)
{
    if (!ACDCanDualWield(tACD))
        return FALSE;
    return ACDInventorySlotContainsOneHandedWeapon(tACD, SLOT_PLAYER_RIGHT_HAND, bUseVisualInventory) &&
           ACDInventorySlotContainsOneHandedWeapon(tACD, SLOT_PLAYER_LEFT_HAND, bUseVisualInventory);
}


















void ACDInventoryWhereCanThisGo(const ActorCommonData & tACDPlayer, const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots)
{
    static const InventorySlotType sgePaperDollSlots[] =
    {
        SLOT_PLAYER_HEAD,
        SLOT_PLAYER_TORSO,
        SLOT_PLAYER_LEFT_HAND,
        SLOT_PLAYER_RIGHT_HAND,
        SLOT_PLAYER_HANDS,
        SLOT_PLAYER_WAIST,
        SLOT_PLAYER_FEET,
        SLOT_PLAYER_SHOULDERS,
        SLOT_PLAYER_LEGS,
        SLOT_PLAYER_BRACERS,
        SLOT_PLAYER_NECK,
        SLOT_PLAYER_LEFT_FINGER,
        SLOT_PLAYER_RIGHT_FINGER,
    };





    int32 nNumPaperDollSlots = sizeof(sgePaperDollSlots) / sizeof(sgePaperDollSlots[0]);

    EInvItemClass eHand = GetInventoryItemClass(gbidItem);


    for (int32 i = 0; i < nNumPaperDollSlots; i++)
    {
        InventorySlotType eSlot = sgePaperDollSlots[i];


        if (ItemIsUniqueEquipped(gbidItem))
        {
            InventoryLocation tInvLocSlot;
            ACDInventoryLocationSet(tInvLocSlot, tACDPlayer.id, eSlot, 0, 0);
            ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
            ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
            if (ptACDItemInSlot &&
                ptACDItemInSlot->hGB.gbid == gbidItem)
            {
                nNumSlots = 1;
                eValidSlots[0] = eSlot;
                return;
            }
        }

        if (!ACDInventoryItemAllowedInThisSlotType(gbidItem, eSlot))
            continue;

        if (eSlot == SLOT_PLAYER_RIGHT_HAND)
        {
            if (eHand == IIC_ONEHANDED)
            {


                PlayerClass ePlayerClass = PlayerGetPlayerClass(tACDPlayer.id);
                if (ePlayerClass != PLAYER_CLASS_NONE)
                {
                    if (!PlayerClassCanDualWield(ePlayerClass))
                        continue;
                }
                else
                {
                    if (!ACDCanDualWield(tACDPlayer))
                        continue;
                }
            }



            if (PlayerGetPlayerClass(tACDPlayer.id) == PLAYER_CLASS_DEMONHUNTER &&

                !ItemHasLabel(gbidItem, ITL_2_HAND))
            {
                VisualInventory * ptVisualInventorySource = VisualInventoryGet(tACDPlayer.id);
                VisualItem & tVisualItemLeftHand = ptVisualInventorySource->tEquipmentCurrent.tVisualItem[VIS_LEFT_HAND];
                WeaponClass eWeaponClassLeft = GetWeaponClass(tVisualItemLeftHand.gbidItem, GBID_NONE);

                if (!ItemHasLabel(gbidItem, ITL_SHIELD) &&
                    !ItemHasLabel(gbidItem, ITL_QUIVER))
                {


                    if (!ItemHasLabel(gbidItem, ITL_HAND_CROSSBOW))
                        continue;



                    if (eWeaponClassLeft != WEAPONCLASS_HANDXBOW && eWeaponClassLeft != WEAPONCLASS_NONE)
                        continue;
                }
            }
        }





        eValidSlots[nNumSlots++] = eSlot;
    }

}




void ACDInventoryWhereCanThisGoHireling(const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots)
{
    static const InventorySlotType sgePaperDollSlots[] =
    {
        SLOT_HIRELING_LEFT_HAND,
        SLOT_HIRELING_RIGHT_HAND,
        SLOT_HIRELING_SPECIAL,
        SLOT_HIRELING_LEFT_FINGER,
        SLOT_HIRELING_RIGHT_FINGER,
        SLOT_HIRELING_NECK,
    };




    for (int32 i = 0; i < 6; i++)
    {
        if (ACDInventoryItemAllowedInThisSlotType(gbidItem, sgePaperDollSlots[i]))
        {

            eValidSlots[nNumSlots++] = sgePaperDollSlots[i];
        }
    }
}























































CostType ACDInventoryGetCostType(const ACDID idACDItem, const ACDID idACDContainer, const ACDID idACDPlayer)
{



    if (idACDItem == ACDID_INVALID || idACDContainer == ACDID_INVALID)
        return -1;  // CostType values have no names in the DWARF



    ActorCommonData * ptACDContainer = ACDTryToGet(idACDContainer);
    if (!ptACDContainer)
        return -1;




    if (ptACDContainer->IsPet())
        return -1;





    if (idACDPlayer != ACDID_INVALID)
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (!ptPlayer || ptPlayer->idACDActiveVendor == ACDID_INVALID)
            return -1;
    }




    const InventoryLocation * ptLocation = ACDGet(idACDItem).TryToGetInventoryLocation();
    if (ptLocation && ptLocation->eSlot == SLOT_PLAYER_BUYBACK)
        return 2;





    return ptACDContainer->IsMerchant() ? 1 : 0;
}









static void sACDAddReactionFlag(ActorInventoryDatabaseRecord & tRecord, const uint32 newFlag)
{
    tRecord.uActionMask = (ActorInventoryDatabaseReaction)(tRecord.uActionMask | newFlag);
}










































// NON_MATCHING: same 365 instructions; only the layout of three cold switch blocks differs (original order: AIDA_CREATE, STASH_TO_SOCKET, INVENTORY_TO_SOCKET; ours puts the AIDA_CREATE block last). The three have equal branch weight; case order, explicit no-op cases and if/else shape do not change it. The 8 AIDR_LOCATION case labels are packed on lines 12088-12089 because the call sits on line 12090.
DataID SACDInventoryAddToDirtyList(
    const ActorCommonData & tACDItem,
    ActorInventoryDatabaseAction eAction,
    InventorySlotType eCameFrom,
    InventorySlotType eDestination)
{



    DataID tRetId = DATAID_INVALID;



    if (GameIsParamFlagSet(GPF_SAVE_HEROS))
    {




        if (ReplayIsReplayGame())
        {
            return tRetId;
        }

        if (GameIsBattleNetServer())
        {

            if (CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
            {
                return tRetId;
            }


            Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);


            DirtyInventoryRecordMap * ptItemMoveLog = NULL;
            ACDID idACDUltOwner;



            idACDUltOwner = ActorInventoryGetUltimateOwner(&tACDItem);
            if (idACDUltOwner != ACDID_INVALID)
            {


                ActorCommonData & ptACDOwner = ACDGet(idACDUltOwner);
                if (ptACDOwner.IsPet())
                {   Player * ptPlayer = ptACDOwner.GetPetOwnerPlayer();
                    idACDUltOwner = ptPlayer->idPlayerACD;
                }

                Player * ptPlayer = PlayerGetByACD(idACDUltOwner);
                if (ptPlayer != NULL && !SPlayerInSaveList(*ptPlayer))
                {
                    SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_Item, PLAYER_SAVE_MODE_LAZY);
                }

                ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
                ptItemMoveLog = ptActorInventoryGlobals->ptItemMoveLog;
            }

            if (ptItemMoveLog == NULL)
            {
                return tRetId;
            }


            DirtyInventoryRecordMap::iterator it = ptItemMoveLog->find(ptActor->tItemId.GetLow());
            if (it != ptItemMoveLog->end())
            {   it->second.eAction = eAction;
            }
            else
            {



                if (eAction == AIDA_REMOVE && ptActor->tItemId.IsInvalid())
                {
                    return tRetId;
                }

                bool bNewItemId = false;
                bool bNotPersisted = false;






















                ActorInventoryDatabaseRecord tmp;
                tmp.uActionMask = AIDR_NONE;

                Player * ptPlayer = PlayerGetByACD(idACDUltOwner);
                if (ptPlayer != NULL)
                    tmp.firstAccountId = ptPlayer->tAccount.tHandle.m_id;













                tmp.bNewItem = bNewItemId; tmp.eAction = eAction;


                (*ptItemMoveLog)[ptActor->tItemId.GetLow()] = tmp;
                it = ptItemMoveLog->find(ptActor->tItemId.GetLow());
            }

            ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
            it->second.uUpdateCounter = ptActorInventoryGlobals->uDirtyUpdateCounter++;

            it->second.idACD = tACDItem.id;


            it->second.eDestination = (eAction == AIDA_UPDATE_INVENTORY_TO_STASH || eAction == AIDA_UPDATE_STASH_TO_STASH) ? SLOT_PLAYER_SHARED_STASH : eDestination;





            switch (it->second.eAction)
            {
            case AIDA_CREATE:


                sACDAddReactionFlag(it->second, AIDR_GENERATOR | AIDR_OWNER);


                if (eDestination != SLOT_ITEM_SOCKETS)
                    sACDAddReactionFlag(it->second, AIDR_LOCATION);
                break;

            case AIDA_UPDATE_STACK:
            case AIDA_UPDATE_IDENTIFY:
            case AIDA_UPDATE_JEWEL_UPGRADE:
            case AIDA_UPDATE_CRAFT:
            case AIDA_UPDATE_DYE:
            case AIDA_UPDATE_TRANSMOG:
            case AIDA_UPDATE_ENCHANT_START:
            case AIDA_UPDATE_TRADE_TIMER_PERIODIC:
            case AIDA_UPDATE_TRADE_TIMER_COMPLETE:
            case AIDA_UPDATE_DURABILITY:
            case AIDA_UPDATE_GEM_ADDED:
            case AIDA_UPDATE_GEM_REMOVED:








                sACDAddReactionFlag(it->second, AIDR_GENERATOR);
                break;












            case AIDA_UPDATE_STASH_TO_SOCKET:
                sACDAddReactionFlag(it->second, AIDR_FROM_STASH);
                break;

            case AIDA_UPDATE_INVENTORY_TO_SOCKET:
                sACDAddReactionFlag(it->second, AIDR_FROM_INVENTORY);
                break;
            case AIDA_REMOVE: case AIDA_UPDATE_MOVE: case AIDA_UPDATE_DROP: case AIDA_UPDATE_STASH_TO_INVENTORY:
            case AIDA_UPDATE_INVENTORY_TO_STASH: case AIDA_UPDATE_STASH_TO_STASH: case AIDA_UPDATE_SOCKET_TO_INVENTORY: case AIDA_UPDATE_LIMBO:
                sACDAddReactionFlag(it->second, AIDR_LOCATION);
                break;
            }






            Player * ptPlayer = PlayerGetByACD(idACDUltOwner);
            if (ptPlayer != NULL)
                it->second.lastAccountId = ptPlayer->tAccount.tHandle.m_id;

            ACDInventorySaveItem(tACDItem, it->second.tSavedItem, 0, tACDItem.tLocation.tInvLocation.eSlot);


            ActorInventoryStorageBucket eBucketCameFrom = GetInventoryBucket(tACDItem, eCameFrom);
            ActorInventoryStorageBucket eBucketDestination = GetInventoryBucket(tACDItem, it->second.eDestination);
            if (eBucketCameFrom != eBucketDestination) {
                sACDAddReactionFlag(it->second, AIDR_OWNER);

                if (ptPlayer != NULL) {
                    InventoryId tOwnerId;
                    if (eBucketDestination == AISB_STASH)
                        tOwnerId = ptPlayer->tAccount.tHandle;
                    else
                        tOwnerId = ptPlayer->uHeroId;

                    tOwnerId.Export(*it->second.tSavedItem.mutable_owner_entity_id());
                }
            }



            char szSuffix[256];
            szSuffix[0] = 0;
            if (tACDItem.tLocation.tInvLocation.eSlot == SLOT_ITEM_SOCKETS) {
                ActorCommonData * ptACDOwner = ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer);
                if (ptACDOwner != NULL) {
                    Actor * ptOwner = ActorGet((ActorID)ptACDOwner->idOwner);
                    Blizzard::String::Format(szSuffix, sizeof(szSuffix), " container:%s/%s ", MarkerNameGetString(ptACDOwner->tName), ptOwner->tItemId.ToString().str());
                }
            }
            if (eAction == AIDA_UPDATE_STACK)
                Blizzard::String::Format(szSuffix, sizeof(szSuffix), " stack %llu ", tACDItem.GetStackCount());
        }
        else
        {













            Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);

            if (ptActor->tItemId.IsInvalid())
            {






                D3::OnlineService::ItemId tItemId;
                ItemComputeConsoleItemId(tACDItem, tItemId);
                ptActor->tItemId.Import(tItemId);

            }
        }
    }

    return tRetId;
}






void ActorInventoryMergeItemAttributes(
    ActorCommonData & tACDOwner,
    ActorCommonData & tACDItem)
{
    AttribGroupID idFastAttribItem = SItemGetAttributeGroup(tACDItem);
    BOOL bHasAltAttrib = FALSE;
    if (tACDOwner.IsItem())
    {







        AttribGroupID idFastAttribOwnerAlt = SItemGetAttributeGroup(tACDOwner);
        bHasAltAttrib = idFastAttribOwnerAlt != tACDOwner.idFastAttrib;
    }






    Player * ptPlayerOwner = PlayerGetByACD(tACDOwner.id);
    if (ptPlayerOwner && tACDOwner.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) > 0)
    {
        AttribGroupID idUpscaledAttribItem = ItemGenerateUpscaledAttribForPlayer(tACDOwner, tACDItem);
        tACDOwner.AttributesMergeAttributeGroup(idUpscaledAttribItem);
    }
    else
    {


        tACDOwner.AttributesMergeAttributeGroup(idFastAttribItem);
    }



    if (bHasAltAttrib)
    {
        InventoryLocation tInvLoc = tACDOwner.GetInventoryLocation();
        ACDRemoveFromCurrentPosition(tACDOwner);
        sAddToInventory(tACDOwner, tInvLoc, tInvLoc.eSlot);
    }
}
































void ActorInventoryIterateGetItemPassivePowers(ACDID idACDSquare, InventorySlotType, void * plistPassiveSNOs)
{



    if (idACDSquare == ACDID_INVALID || !plistPassiveSNOs)
        return;


    XPooledDataIDList & listPassiveSNOs = *(XPooledDataIDList *)plistPassiveSNOs;


    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);





    if (!ptACDItem || ItemIsBroken(*ptACDItem) || ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED) == TRUE)
        return;





    FastAttribIterator tIt(ptACDItem->idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        Attrib eAttrib = KeyGetAttrib(tIt.tEntryBuffer.tKey);
        if (eAttrib == ATTRIBUTE_ITEM_POWER_PASSIVE)
        {
            SNO snoHandle = PowerSNOFromAttributeKey(tIt.tEntryBuffer.tKey);

            if (snoHandle != SNO_NONE)
            {



                listPassiveSNOs.AddTail_Tracked(snoHandle);
            }
        }
    }
}



static SNO sGetItemSoundFromGBID(GBID gbidItem)
{
    GBID gbidItemType = ItemGetItemTypeGBID(gbidItem);
    CRefString sSound = CRefString().Format("INV_%s", GBIDToString(gbidItemType, GB_ITEMTYPES, FALSE).str());
    if (CWorldIsRetro())
    {
        CRefString sSoundRetro = sSound;
        sSoundRetro += "_retro";
        SNO snoSoundRetro = SNOFileLookupGUID(SOUND_SNO, sSoundRetro.str());
        if (snoSoundRetro != SNO_NONE)
            return snoSoundRetro;
    }
    return SNOFileLookupGUID(SOUND_SNO, sSound.str());
}












SNO ActorInventory_GetItemLandingSound(const ActorCommonData & tACDItem)
{
    SNO snoInventorySound;
    ScopedActorDefinitionLockR pdefActor(tACDItem.snoActor);




    snoInventorySound = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_LANDING_SOUND);
    if (snoInventorySound == SNO_NONE)
        snoInventorySound = sGetItemSoundFromGBID(tACDItem.hGB.gbid);
    return snoInventorySound;
}








SNO ActorInventory_GetItemUseSound(const ActorCommonData * ptACDItem)
{
    if (ptACDItem)
    {
        SNO snoInventorySound;
        ScopedActorDefinitionLockR pdefActor(ptACDItem->snoActor);


        snoInventorySound = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_INVENTORY_SOUND);
        if (snoInventorySound == SNO_NONE)
            snoInventorySound = sGetItemSoundFromGBID(ptACDItem->hGB.gbid);
        return snoInventorySound;
    }
    return GlobalSNOGet(GLOBALSNO_SOUND_PICKUP);
}






































BOOL ActorInventoryIsVariableSizedSlot(const InventorySlotType eSlot)
{
    switch (eSlot)
    {
    case SLOT_PLAYER_BACKPACK:
    case SLOT_PLAYER_SHARED_STASH:
        return TRUE;
    }
    return FALSE;
}





Attrib ActorInventoryGetVariableSizedSlotAttrib(const InventorySlotType eSlot)
{
    switch (eSlot)
    {
    case SLOT_PLAYER_BACKPACK:
        return ATTRIBUTE_BACKPACK_SLOTS;
    case SLOT_PLAYER_SHARED_STASH:
        return ATTRIBUTE_SHARED_STASH_SLOTS;
    }
    return ATTRIBUTE_NONE;
}







BOOL ActorInventoryIsLargeSlot(const InventorySlotType eSlot)
{
    switch (eSlot)
    {
    case SLOT_PLAYER_RIGHT_FINGER:
    case SLOT_PLAYER_LEFT_FINGER:
    case SLOT_PLAYER_NECK:
    case SLOT_HIRELING_SPECIAL:
        return FALSE;
    }
    return TRUE;
}









int32 ACDInventoryGetStashUpgradeCost(ActorCommonData & tACDPlayer)
{
    int32 nMaxSlots = 7 * 10 * MAX_STASH_TABS_GOLD_PURCHASABLE;
    int32 nSlotCount = tACDPlayer.AttributesGetInt(ATTRIBUTE_SHARED_STASH_SLOTS);

    if (nSlotCount < nMaxSlots)
    {
        const int32 nSlotsPerTier = 40;














        ScopedGlobalDefinitionLockR pdefGlobals;


        if (nSlotCount < 60)
            return pdefGlobals->nStashSlotsCost;


        if (nSlotCount < 60 + nSlotsPerTier)
            return pdefGlobals->nStashTabCost[0];


        if (nSlotCount < 60 + 2 * nSlotsPerTier)
            return pdefGlobals->nStashTabCost[1];


        if (nSlotCount < 60 + 3 * nSlotsPerTier)
            return pdefGlobals->nStashTabCost[2];




        if (nSlotCount < 60 + 4 * nSlotsPerTier)
            return pdefGlobals->nStashTabCost[3];
        return pdefGlobals->nStashTabCost[4];
    }


    return 0;
}


SNO ACDGetInventory(const ActorCommonData * ptACD, BOOL bSecondary)
{

    SNO snoRetVal = SNO_NONE;
    if (!ptACD)
        return snoRetVal;




    if (ptACD->IsMonster())
    {
        ScopedActorDefinitionLockR pdefActor(ptACD->tSNOName.snoHandle);
        ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
        snoRetVal = !bSecondary ? pdefMonster->snoInventory : pdefMonster->snoSecondaryInventory;
    }
    else
    {
        GBHandle hGB = ptACD->hGB;
        if (hGB.gbid != GBID_NONE)
        {

            if (hGB.eType == GB_HEROS) {
                const GameBalanceDefinition * pdefGB;
                const HeroData * ptHero = (const HeroData *)GBLockR(hGB, pdefGB, TRUE);
                if (ptHero)
                {
                    snoRetVal = ptHero->snoInventory;
                }
                GBUnlockR(pdefGB);
            }
            else
            {
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
    }


    return snoRetVal;
}


inline void SACDInventoryEvaluateAcquireItemAchievementEvent(const Player & tPlayer, const ActorCommonData & tACDItem)
{











    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetACD(&tACDItem);

        tEvaluationData.EvaluateEvent(&tPlayer,
                                      Achievement::ACH_EVENT_ACQUIRE_ITEM,

                                      tACDItem.hGB.gbid,
                                      1);
    }
}




void SACDInventoryEvaluateItemDyeAchievements(Achievement::EvaluationData * ptEvaluationData)
{



    if (ptEvaluationData->GetEquipmentInfo()->IsAllEquipmentDyedDifferently())
    {

        ptEvaluationData->EvaluateEvent(
            ptEvaluationData->GetPlayer(),
            Achievement::ACH_EVENT_EQUIP_ALL_ITEMS_WITH_DIFFERENT_DYES,
            1);
    }
    else if (ptEvaluationData->GetEquipmentInfo()->IsAllEquipmentDyed())
    {
        if (ptEvaluationData->GetEquipmentInfo()->GetDyeUsedCount() == 1)
        {
            DyeType eDyeType = DYE_TYPE_NONE;

            for (int32 i = 0; i < MAX_DYE_VALUE; i++)
            {
                if (ptEvaluationData->GetEquipmentInfo()->IsDyeUsed(i))
                {
                    eDyeType = i;
                    break;
                }
            }

            ptEvaluationData->EvaluateEvent(
                ptEvaluationData->GetPlayer(),
                Achievement::ACH_EVENT_EQUIP_ALL_ITEMS_WITH_SPECIFIC_DYE,
                DyeTypeTranslateToSavedDyeType(eDyeType),
                1);
        }
    }
}




void SACDInventorySetItemEquipped(ActorCommonData & tACDItem, BOOL bOnHeroLoad)


{
    tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED, TRUE);
    {














        ACDID idACDOwner = ACDInventoryGetUltimateContainer(tACDItem);

        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetACD(&tACDItem);
        tEvaluationData.SetEquipmentInfo(idACDOwner);


        Player * ptPlayerOwner = PlayerGetByACD(idACDOwner);
        if (ptPlayerOwner)
        {
            tEvaluationData.SetPlayer(ptPlayerOwner);


            tEvaluationData.EvaluateEvent(
                tEvaluationData.GetPlayer(),
                Achievement::ACH_EVENT_EQUIP_ITEM_PLAYER,
                tACDItem.hGB.gbid,
                1);

            if (tEvaluationData.GetEquipmentInfo()->IsFullyEquipped())
            {

                tEvaluationData.EvaluateEvent(
                    tEvaluationData.GetPlayer(),
                    Achievement::ACH_EVENT_PLAYER_FULLY_EQUIPPED,
                    1);
            }


            if (tEvaluationData.GetEquipmentInfo()->IsAllEquipmentEnchanted())
            {
                tEvaluationData.EvaluateEvent(
                    tEvaluationData.GetPlayer(),
                    Achievement::ACH_EVENT_EQUIP_ALL_ITEMS_ENCHANTED,
                    1);
            }


            if (tEvaluationData.GetEquipmentInfo()->IsAllEquipmentTransmogrifiedToLegendary())
            {
                tEvaluationData.EvaluateEvent(
                    tEvaluationData.GetPlayer(),
                    Achievement::ACH_EVENT_EQUIP_ALL_ITEMS_TRANSMOG_LEGENDARY,
                    1);
            }


            if (tEvaluationData.GetEquipmentInfo()->IsAllEquipmentMaxLevelLegendary())
            {
                tEvaluationData.EvaluateEvent(
                    tEvaluationData.GetPlayer(),
                    Achievement::ACH_EVENT_EQUIP_ALL_ITEMS_MAX_LEVEL_LEGENDARY,
                    1);
            }

            SACDInventoryEvaluateItemDyeAchievements(&tEvaluationData);
        }
        else
        {

            ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
            if (ptACDOwner && ACDIsHireling(*ptACDOwner))
            {
                tEvaluationData.SetPlayer(ptACDOwner->GetPetOwnerPlayer());

                if (tEvaluationData.GetPlayer())
                {








                    if (tEvaluationData.GetEquipmentInfo()->IsFullyEquipped())
                    {

                        tEvaluationData.EvaluateEvent(
                            tEvaluationData.GetPlayer(),
                            Achievement::ACH_EVENT_HIRELING_FULLY_EQUIPPED,
                            1);
                    }

                    if (tEvaluationData.GetEquipmentInfo()->IsAllHirelingEquipmentMaxLevelLegendary())
                    {
                        tEvaluationData.EvaluateEvent(
                            tEvaluationData.GetPlayer(),
                            Achievement::ACH_EVENT_EQUIP_HIRELING_ALL_ITEMS_MAX_LEVEL_LEGENDARY,
                            1);
                    }
                }
            }
        }
    }

}



void SACDInventorySetItemUnequipped(ActorCommonData & tACDItem)
{

    tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED, FALSE);

    tACDItem.AttributesSetInt(ATTRIBUTE_TRANSMOG_GBID_TEMP_OVERRIDE, GBID_NONE);
}





















































MailRecordMap & ACDMailMapGet()
{
    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    return *ptActorInventoryGlobals->ptMailMap;
}





int32 ACDDirtyListGetSize()
{
    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;
    if (ptActorInventoryGlobals == NULL)
        return 0;

    return (int32)ptActorInventoryGlobals->ptItemMoveLog->size();
}



void ACDPurgeGlobalItemLists(DirtyInventoryRecordMap * ptMapToCopyTo)
{
    ActorInventoryGlobals *& ptActorInventoryGlobals = GameGetActiveGameCommonData().ptActorInventoryGlobals;

    if (!ptActorInventoryGlobals)
        return;

    if (ptMapToCopyTo)
    {
        ptMapToCopyTo->swap(*ptActorInventoryGlobals->ptItemMoveLog);
    }

    ptActorInventoryGlobals->ptItemMoveLog->clear();
}




void ACDUpdateActorPersistState(const DirtyInventoryRecordMap & tDirtyMap)
{



    for (DirtyInventoryRecordMap::const_iterator it = tDirtyMap.begin(); it != tDirtyMap.end(); ++it)
    {

        if (it->second.eAction == AIDA_REMOVE || it->second.eDestination == SLOT_PLAYER_BUYBACK || it->second.eDestination == SLOT_NONE)
        {

            if (!it->second.bNewItem)
            {
                ActorCommonData * ptACDItem = ACDTryToGet(it->second.idACD);
                if (ptACDItem)
                {   Actor * ptActor = ActorGet((ActorID)ptACDItem->idOwner);
                    if (ptActor && ptActor->tItemId.GetLow() == it->first)
                        ptActor->dwActorFlags &= ~(1 << ACTOR_DB_PERSISTED);
                }
            }
        }
        else {
            if (it->second.bNewItem)
            {
                ActorCommonData * ptACDItem = ACDTryToGet(it->second.idACD);
                if (ptACDItem)
                {   Actor * ptActor = ActorGet((ActorID)ptACDItem->idOwner);
                    if (ptActor && ptActor->tItemId.GetLow() == it->first)
                        ptActor->dwActorFlags |= (1 << ACTOR_DB_PERSISTED);
                }
            }
        }
    }
}



void ActorInventorySetSeed(ActorInventory * ptInventory, uint32 dwSeed)
{


    if (ptInventory)
    {
        ptInventory->dwSeed = dwSeed;
    }
}


uint32 ActorInventoryGetSeed(const ActorInventory * pInv)
{
    if (pInv)
        return pInv->dwSeed;
    return 0;
}

SNO ActorInventoryGetIdentifyPower(const ActorCommonData & tACDItem)
{


    SNO snoPower = SNO_NONE;
    if (tACDItem.IsItem())
    {
        GlobalSNO eGlobalSNO = ItemIsLegendary(tACDItem.hGB.gbid) ? GLOBALSNO_POWERS_IDENTIFY_WITH_CAST_LEGENDARY : GLOBALSNO_POWERS_IDENTIFY_WITH_CAST;

        snoPower = GlobalSNOGet(eGlobalSNO);
    }
    return snoPower;
}







BOOL FastAttribHasChild(AttribGroupID idParent, AttribGroupID idChild); void VisualInventoryUpdateItem(ActorCommonData & tACDItem);
static BOOL sCheckBothHandItemLabels(
    const ActorCommonData & tACDItemRight,
    const ActorCommonData & tACDItemLeft,
    const ItemLabel eItemLabel0,
    const ItemLabel eItemLabel1)
{
    if (ItemHasLabel(tACDItemRight.hGB.gbid, eItemLabel0) &&
        ItemHasLabel(tACDItemLeft.hGB.gbid, eItemLabel1))
    {
        return TRUE;
    }

    if (ItemHasLabel(tACDItemLeft.hGB.gbid, eItemLabel0) &&
        ItemHasLabel(tACDItemRight.hGB.gbid, eItemLabel1))
    {
        return TRUE;
    }
    return FALSE;
}


// NON_MATCHING: 1776 vs 1776 bytes, 72 instructions differ. Fixed: the 13033 condition is `id != id &&` on 13033 with the gbid-invalid test (|| at col 56) on 13035 (orig rows 13033:40/68 and 13035:56). Left: the 13038/13039 gbid compares; the original tests hGB.gbid == gbidLinkedPowerInner first and places the other two out of line, ours tests Outter == Inner first. Tried: A || (B || C), swapped == operands (no change). Orig 13038 is indented 20 (cols 52/81), so something with no code may wrap that if.
void SACDInventoryRemoveIllegalItems(
    ActorCommonData & tACD)
{


    if (!ACDIsPlayer(tACD.id))
        return;



    ACDID idACDItemRight = ACDInventoryGetSquareItem(tACD, SLOT_PLAYER_RIGHT_HAND, IVector2D(0, 0));
    ActorCommonData * ptACDItemRight = ACDTryToGet(idACDItemRight);
    if (ptACDItemRight && ptACDItemRight->IsItem() && !ItemIsEquippedButDisabled(*ptACDItemRight))
    {
        ACDID idACDItemLeft = ACDInventoryGetSquareItem(tACD, SLOT_PLAYER_LEFT_HAND, IVector2D(0, 0));
        ActorCommonData * ptACDItemLeft = ACDTryToGet(idACDItemLeft);
        if (ptACDItemLeft && ptACDItemLeft->IsItem())
        {
            if ((sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_2_HAND, ITL_SHIELD) &&
                 !tACD.AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD)) ||
                sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_2_HAND, ITL_2_HAND) ||
                sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_2_HAND, ITL_1_HAND) ||
                sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_2_HAND, ITL_ORB) ||
                sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_2_HAND, ITL_MOJO) ||
                sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_2_HAND, ITL_OFFHAND_NECROMANCER) ||
                sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_MELEE, ITL_BOWCLASS))
            {








                ptACDItemRight->AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED, 1);
                ptACDItemRight->AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED_DUPLICATE_LEGENDARY, 0);

                AttribGroupID idFastAttribItem = SItemGetAttributeGroup(*ptACDItemRight);


                if (FastAttribHasChild(tACD.idFastAttrib, idFastAttribItem))
                {
                    tACD.AttributesUnmergeAttributeGroup(idFastAttribItem);
                    ACDInventoryEquipmentOrSkillChanged(tACD);
                    VisualInventoryUpdateItem(*ptACDItemRight);
                }
            }
        }
    }






    int32 slotStart = SLOT_PLAYER_BODYSLOT_START;
    int32 slotEnd = SLOT_PLAYER_BODYSLOT_END;
    for (int32 nIndexOutter = slotStart; nIndexOutter < slotEnd; nIndexOutter++)
    {
        InventoryLocation tInvLocSlotOutter;
        ACDInventoryLocationSet(tInvLocSlotOutter, tACD.id, nIndexOutter, 0, 0);
        ACDID idACDItemInSlotOutter = ACDInventoryGetItemInSlot(tInvLocSlotOutter);
        ActorCommonData * ptACDItemInSlotOutter = ACDTryToGet(idACDItemInSlotOutter);
        if (!ptACDItemInSlotOutter)
        {
            continue;
        }

        ScopedItemDefinitionLockR pdefItemInSlotOutter(ptACDItemInSlotOutter->hGB.gbid);
        if (!pdefItemInSlotOutter)
        {
            continue;
        }

        GBID gbidLinkedPowerOutter = pdefItemInSlotOutter->gbidLegendaryPowerItemReplacement;

        for (int32 nIndexInner = nIndexOutter + 1; nIndexInner < slotEnd; nIndexInner++)
        {
            InventoryLocation tInvLocSlotInner;
            ACDInventoryLocationSet(tInvLocSlotInner, tACD.id, nIndexInner, 0, 0);
            ACDID idACDItemInSlotInner = ACDInventoryGetItemInSlot(tInvLocSlotInner);
            ActorCommonData * ptACDItemInSlotInner = ACDTryToGet(idACDItemInSlotInner);
            if (!ptACDItemInSlotInner)
            {
                continue;
            }

            ScopedItemDefinitionLockR pdefItemInSlotInner(ptACDItemInSlotInner->hGB.gbid);
            if (!pdefItemInSlotInner)
            {
                continue;
            }

            GBID gbidLinkedPowerInner = pdefItemInSlotInner->gbidLegendaryPowerItemReplacement;
            // Inference: orig rows 13033:17 (branch), 13033:40/68 (->id loads) and 13035:56 (|| of the gbid test) put the condition over 13033-13035
            if (ptACDItemInSlotOutter->id != ptACDItemInSlotInner->id &&

                (gbidLinkedPowerOutter != GBID_NONE || gbidLinkedPowerInner != GBID_NONE))
            {

                if (ptACDItemInSlotOutter->hGB.gbid == gbidLinkedPowerInner ||
                    gbidLinkedPowerOutter == gbidLinkedPowerInner || gbidLinkedPowerOutter == ptACDItemInSlotInner->hGB.gbid)
            {

                ptACDItemInSlotOutter->AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED, 1);
                ptACDItemInSlotOutter->AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED_DUPLICATE_LEGENDARY, 1);

                AttribGroupID idFastAttribItem = SItemGetAttributeGroup(*ptACDItemInSlotOutter);


                if (FastAttribHasChild(tACD.idFastAttrib, idFastAttribItem))
                {
                    tACD.AttributesUnmergeAttributeGroup(idFastAttribItem);
                    ACDInventoryEquipmentOrSkillChanged(tACD);
                    VisualInventoryUpdateItem(*ptACDItemInSlotOutter);
                    break;
                }
            } }
        }
    }




}

void VisualInventoryResetAllSlots(const ACDID idACD);  // VisualInventory.cpp
void SACDInventoryRestoreIllegalItems(ActorCommonData & tACD)
{



    if (!ACDIsPlayer(tACD.id))
        return;





    ACDID idACDItemRight = ACDInventoryGetSquareItem(tACD, SLOT_PLAYER_RIGHT_HAND, IVector2D(0, 0));
    ActorCommonData * ptACDItemRight = ACDTryToGet(idACDItemRight);
    if (ptACDItemRight && ptACDItemRight->IsItem() && ItemIsEquippedButDisabled(*ptACDItemRight))
    {
        BOOL bRestoreRight = TRUE;

        ACDID idACDItemLeft = ACDInventoryGetSquareItem(tACD, SLOT_PLAYER_LEFT_HAND, IVector2D(0, 0));
        ActorCommonData * ptACDItemLeft = ACDTryToGet(idACDItemLeft);
        if (ptACDItemLeft)
        {
            if (!sCheckBothHandItemLabels(*ptACDItemRight, *ptACDItemLeft, ITL_1_HAND, ITL_SHIELD))
            {
                bRestoreRight = FALSE;
            }
        }





        if (bRestoreRight)
        {
            ptACDItemRight->AttributesSetInt(ATTRIBUTE_ITEM_EQUIPPED_BUT_DISABLED, 0);

            AttribGroupID idFastAttribItem = SItemGetAttributeGroup(*ptACDItemRight);
            if (!ItemIsBroken(*ptACDItemRight) &&
                !ItemIsEquippedButDisabled(*ptACDItemRight) &&
                !FastAttribHasChild(tACD.idFastAttrib, idFastAttribItem))
            {
                tACD.AttributesMergeAttributeGroup(idFastAttribItem);
                ACDInventoryEquipmentOrSkillChanged(tACD);
                VisualInventoryResetAllSlots(tACD.id);
            }
        }
    }


}


BOOL ActorInventoryAllowedToSeeItem(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItem)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return TRUE;




    if (tACDItem.HasAssignedHeroID())
    {
        if (!tACDItem.DoesPlayerMatchAssignedHeroID(ptPlayer))
            return FALSE;
    }

    return TRUE;
}




BOOL ActorInventoryAllowedToPickupItem(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItem)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return TRUE;




    if (tACDItem.HasAssignedHeroID() &&
        tACDItem.DoesPlayerMatchAssignedHeroID(ptPlayer))
        return TRUE;



    if (ActorInventoryAllowedToSeeItem(tACDPlayer, tACDItem))
    {
        if (ItemIsGlyph(tACDItem))
            return TRUE;



        ItemBindingLevel eBindingLevel = ItemGetBindingLevel(tACDItem);

        if (eBindingLevel == IBL_BOUND_TO_GAME)
        {
            BOOL bTradableToThisPlayer = FALSE;
            for (int32 i = 0; i < 4; i++)
            {
                if (ptPlayer->tAccount.tHandle.m_id == tACDItem.GetItemTradePlayerID(i))
                {
                    bTradableToThisPlayer = TRUE;
                    break;
                }
            }

            if (!bTradableToThisPlayer)
                return FALSE;
        }


        else if (eBindingLevel == IBL_ACCOUNT_PICKUP || eBindingLevel == IBL_ACCOUNT_EQUIP)
        {
            ACDID idACDBoundTo = ItemGetBoundToACD(tACDItem, TRUE);
            if (idACDBoundTo != ACDID_INVALID &&
                idACDBoundTo != ptPlayer->idPlayerACD &&
                idACDBoundTo != ptPlayer->idACDActiveVendor)
                return FALSE;
        }



        if (tACDItem.GetLocationType() != AL_WORLD ||
            !tACDPlayer.AccountIsTradeRestricted())
            return TRUE;
    }



    return FALSE;
}


BOOL ActorInventoryHasUniqueEquippedSocketablesEquipped(const ActorCommonData & tACDOwner, const GBID * gbidUniqueEquippedSocketables, int32 nUniqueEquippedSocketables, ACDID idACDSkipped, InventorySlotType eSkippedSlot)
{





    if (nUniqueEquippedSocketables <= 0)
        return FALSE;


    int32 nSlotStart = SLOT_PLAYER_BODYSLOT_START;
    int32 nSlotEnd = SLOT_PLAYER_BODYSLOT_END;

    if (HirelingGetClass(tACDOwner) != HC_NONE)
    {
        nSlotStart = SLOT_HIRELING_START;
        nSlotEnd = SLOT_HIRELING_END;
    }

    for (int32 i = nSlotStart; i < nSlotEnd; i++)
    {
        InventoryLocation tInvLocSlot;  GBID gbidSlotUniqueEquippedSocketables[3];
        ACDInventoryLocationSet(tInvLocSlot, tACDOwner.id, i, 0, 0);

        ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
        ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
        if (ptACDItemInSlot && idACDItemInSlot != idACDSkipped && i != eSkippedSlot &&


            ItemHasSockets(*ptACDItemInSlot))
        {




        int32 nSlotUniqueEquippedSocketables = ItemGetSocketedUniqueEquipped(*ptACDItemInSlot, gbidSlotUniqueEquippedSocketables, 3);


        for (int32 s = 0; s < nSlotUniqueEquippedSocketables; s++)
        {
            for (int32 t = 0; t < nUniqueEquippedSocketables; t++)
            {
                if (gbidUniqueEquippedSocketables[t] == gbidSlotUniqueEquippedSocketables[s])
                    return TRUE;
            }
        }
        }
    }




    return FALSE;
}






























































void SACDInventoryRegisterItemGiftRecipient(ActorCommonData & tACDItem, uint64 nId, char * szPlayerName)
{



    if (!tACDItem.ptFriendGiftData)
    {
        tACDItem.ptFriendGiftData = new FriendGiftData();
    }

    tACDItem.ptFriendGiftData->idRecipient = nId;
    tACDItem.ptFriendGiftData->sNameRecipient = CRefString(szPlayerName);
}




static void sUpdateUpscaledItem(ACDID idACDSquare, InventorySlotType eSlot, void * pUserData)
{







    ACDID idACDOwner = *(ACDID *)pUserData;

    if (!ACDInventorySlotIsEquippable(eSlot))
        return;



    ActorCommonData & tACDOwner = ACDGet(idACDOwner);
    Player * ptPlayer = PlayerGetByACD(tACDOwner.id);



    if (!GameActiveGameCommonDataIsClient())
    {
        ActorCommonData & tACDItem = ACDGet(idACDSquare);
        GameError eError;
        if (ItemIsBroken(tACDItem))
            return;




        if (!ItemOwnerMeetsRequirements(tACDItem, tACDOwner, &eError))
            return;





        if (tACDItem.idUpscaledAttribGroup != ATTRIBGROUPID_INVALID)
        {
            tACDOwner.AttributesUnmergeAttributeGroup(tACDItem.idUpscaledAttribGroup);
            ClearUpscaledAttributes(tACDItem);
        }
        else
        {

            AttribGroupID idFastAttribItem = SItemGetAttributeGroup(tACDItem);
            tACDOwner.AttributesUnmergeAttributeGroup(idFastAttribItem);
        }


        Player * ptPlayerOwner = PlayerGetByACD(tACDOwner.id);
        if (ptPlayerOwner && tACDOwner.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL) > 0)
        {
            AttribGroupID idUpscaledAttribItem = ItemGenerateUpscaledAttribForPlayer(tACDOwner, tACDItem);
            tACDOwner.AttributesMergeAttributeGroup(idUpscaledAttribItem);
        }
        else
        {

            AttribGroupID idFastAttribItem = SItemGetAttributeGroup(tACDItem);
            tACDOwner.AttributesMergeAttributeGroup(idFastAttribItem);
        }
    }
}


void ACDInventoryUpdateOnUpscalingChanged(ActorCommonData & tACDOwner)
{

    ACDInventoryIterateItems(tACDOwner, SLOT_NONE, 0, sUpdateUpscaledItem, &tACDOwner);
}













int32 GetSeasonalRewardStashTabsCountFromAttributeValue(int32 nAttribute)
{
    return nAttribute & STASH_TABS_REWARDED_FROM_SEASONS_ATTRIBUTE_COUNT_MASK;
}
