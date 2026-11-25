// X/Common/ActorInventory.h -- provisional owning header.
//
// ACDInventoryGetType is defined in X/Common/ActorInventory.cpp
// (_Z19ACDInventoryGetTypeRK15ActorCommonData). The full skeleton has not been
// adopted; this carries the one declaration X/Client/CActorCommonData.cpp:90 needs,
// plus the INVENTORY_* values the switch at :97 uses.

#pragma once

#include <stddef.h>
#include "ActorCommonData.h"
#include "../Sigma/DataStructures/XList.h"
#include "../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "ItemMailCommon.h"
#include "Protobuf/Items.pb.h"

typedef int32 InventoryType;

typedef int32 PlayerClass;  // X/Common/GameSwig.h

enum {
    INVENTORY_NONE = 0,
    INVENTORY_PLAYER = 1,
    INVENTORY_MERCHANT = 5,
    INVENTORY_SOCKETS = 6,
    INVENTORY_HIRELING = 7,
    INVENTORY_GENERIC = 8,
};

enum {
    ITEMDRAW_NORMAL = 0,
    ITEMDRAW_SOCKETED = 1,
    ITEMDRAW_COUNT = 2,
};

typedef int32 ItemDrawType;

struct ImageTextureFrame;

typedef XList<ACDID, ACDID, 8> InventoryOverlapList;  // X/Common/ActorInventory.h:128
typedef int32 GameError;  // X/Common/Player.h

// X/Common/ActorInventory.cpp
void ACDInventoryTestOverlap(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc, InventoryOverlapList & tList);  // ActorInventory.cpp:7437
BOOL ActorInventoryIsVariableSizedSlotValidForItem(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc);  // ActorInventory.cpp:649
GameError ACDInventoryItemAllowedInSlot(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc, const BOOL fSkipRequirements, const BOOL fSwapping);  // ActorInventory.cpp:1596
InventoryType ACDInventoryGetType(const ActorCommonData& tACD);
int32 ACDInventoryGetNumEmptySlots(const ActorCommonData & tACD, InventorySlotType eSlot);  // ActorInventory.cpp:9495
BOOL ACDInventoryItemAllowedInThisSlotType(const GBID gbidItem, const InventorySlotType eSlot);  // ActorInventory.cpp:1575
ACDID ACDInventoryGetSquareItem(const ActorCommonData & tACD, const InventorySlotType eSlot, const IVector2D & pt);  // ActorInventory.cpp:2403
int32 ActorInventoryCountItemsOfType(const ActorCommonData & tACDContainer, const GBHandle & gbHandleToMatch, InventorySlotType * peSlotsToSearch, ACDID * ptidACDItem);  // ActorInventory.cpp:4104
struct IRect2D;
typedef void (*InventorySearchCallback)(ACDID idACD, InventorySlotType eSlot, void * pUserData);
void ACDInventoryIterateItems(const ActorCommonData & tACD, InventorySlotType eSlot, uint32 dwACDIIFlags, InventorySearchCallback pfnCallback, void * pCallbackUserData);  // ActorInventory.cpp:2646
int32 ACDInventoryGetItemCount(const ActorCommonData & tACD, const GBHandle & hGBItem, const InventorySlotType * peSlots, BOOL bWhiteEquippableOnly, BOOL bCheckSockets);  // ActorInventory.cpp:11018
typedef BOOL (*SlotSearchCallback)(const InventoryLocation & tInvLoc, void * pCallBackData);
void ACDInventoryIterateSlot(const ActorCommonData & tACD, InventorySlotType eSlot, const IRect2D * ptRect, SlotSearchCallback pfnCallBack, void * pCallBackData, BOOL fRightLeft, BOOL fBottomUp);  // ActorInventory.cpp:2490

// X/Client/CActorInventory.cpp
void ACDInventoryGetItemDrawInfo(const ActorCommonData & tACDItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale);  // CActorInventory.cpp:39

int32 ACDInventoryGetNumFilledSlots(const ActorCommonData & tACD, const InventorySlotType eSlot);
void ACDInventorySlotGetSize(const InventorySlotType eSlot, IVector2D & tSize);

ACDID ACDInventoryGetItemInSlot(const InventoryLocation & tInvLoc);  // ActorInventory.cpp:614
BOOL ACDInventoryIsSlotPlayer(InventorySlotType eSlot);  // ActorInventory.cpp:10901

// X/Common/ActorInventory.cpp (called from X/Common/ActorAttributes.cpp).
ACDID ACDInventoryGetPrimaryWeapon(ACDID idACD);  // ActorInventory.cpp:5348
WeaponClass ACDGetCurrentWeaponClass(const ActorCommonData & tACD);  // ActorInventory.cpp:11200

// X/Common/ActorInventory.cpp (called from X/Common/NephalemRiftCommon.cpp).
void ACDInventoryGetItem(const ActorCommonData & tACDPlayer, const GBHandle & hGBItem, const InventorySlotType * peSlots, ACDID & idACDItem, int32 & nCount, BOOL bWhiteEquippableOnly);  // ActorInventory.cpp:11118
void SACDInventoryDeleteItem(ActorCommonData & tACDItem, uint64 uCount, BOOL bMarkAsDirty, BOOL bFlagForDeletion);  // ActorInventory.cpp:6800
void ACDInventoryDeleteMail(ActorCommonData & tACDPlayer);  // ActorInventory.cpp:11547

int32 ActorInventoryCountItemsOfTypeOnPlayer(const ACDID idACDContainer, const GBID gbidItem, ACDID * ptidACDItem);  // ActorInventory.cpp:4081
BOOL ACDInventoryIsSlotSocket(InventorySlotType eSlot);  // ActorInventory.cpp:10914

// X/Common/ActorInventory.cpp (called from X/Server/Powers/Powers.cpp).
SNO ACDInventoryGetPrimaryWeaponSNO(ACDID idACD);  // ActorInventory.cpp
typedef int32 EffectType;  // X/Common/Actors/ActorEffectCommon.h
typedef int32 PickupResponse;  // X/Common/ActorCommonData.h
enum {
    PICKUP_RESPONSE_INVALID = 0,
    PICKUP_RESPONSE_OK = 1,
    PICKUP_RESPONSE_TRADING = 2,
    PICKUP_RESPONSE_OWNED_BY_OTHER = 3,
    PICKUP_RESPONSE_SINGULAR = 4,
    PICKUP_RESPONSE_NO_SUITABLE_SLOT_FOUND = 5,
    PICKUP_RESPONSE_UNUSABLE_GLYPH = 6,
    PICKUP_RESPONSE_NOT_RIGHT_NOW = 7,
    NUM_PICKUP_RESPONSES = 8,
};
PickupResponse SACDInventoryPickup(const ACDID idACDPickee, const ACDID idACDItem, const EffectType ePickupEffect, const BOOL fAddToDirtyItemList, const BOOL bTryStashFirst, ACDID * idACDStackDest, BOOL bPlayEffects, BOOL bTryStashOnly, BOOL bPickedUpViaCompanion, BOOL bAllowAutoEquip);  // ActorInventory.cpp:5005

// ActorInventory.cpp:104.
extern const InventorySlotType cgtDefaultPlayerInventorySlots[15];
// ActorInventory.cpp:143, 163, 182, 204.
extern const InventorySlotType cgtPlayerInventorySlotsPreferEquipped[15];
extern const InventorySlotType cgtPlayerInventorySlotsEquippedOnly[14];
extern const InventorySlotType cgtDefaultPlayerInventorySlotsIncludingStash[16];
extern const InventorySlotType cgtPlayerInventorySlotsPreferEquippedIncludingStash[16];
BOOL ACDIsDualWielding(const ActorCommonData & tACD, BOOL bUseVisualInventory);  // ActorInventory.cpp:11579

// Adopted from the reference X/Common/ActorInventory.h (for X/Common/ActorInventory.cpp).
enum SavedInventorySlotType {
    SAVED_SLOT_NONE = 256,
    SAVED_SLOT_PLAYER_BACKPACK = 272,
    SAVED_SLOT_PLAYER_HEAD = 288,
    SAVED_SLOT_PLAYER_TORSO = 304,
    SAVED_SLOT_PLAYER_RIGHT_HAND = 320,
    SAVED_SLOT_PLAYER_LEFT_HAND = 336,
    SAVED_SLOT_PLAYER_HANDS = 352,
    SAVED_SLOT_PLAYER_WAIST = 368,
    SAVED_SLOT_PLAYER_FEET = 384,
    SAVED_SLOT_PLAYER_SHOULDERS = 400,
    SAVED_SLOT_PLAYER_LEGS = 416,
    SAVED_SLOT_PLAYER_BRACERS = 432,
    SAVED_SLOT_PLAYER_RIGHT_FINGER = 448,
    SAVED_SLOT_PLAYER_LEFT_FINGER = 464,
    SAVED_SLOT_PLAYER_NECK = 480,
    SAVED_SLOT_PLAYER_BUYBACK = 512,
    SAVED_SLOT_PLAYER_SHARED_STASH = 544,
    SAVED_SLOT_PLAYER_GOLD_DEPRECATED = 560,
    SAVED_SLOT_PLAYER_BASE_HEALTHPOTION = 576,
    SAVED_SLOT_UNK_250 = 592,
    SAVED_SLOT_MERCHANT_DISPLAY = 768,
    SAVED_SLOT_MERCHANT_SHOPLIST = 784,
    SAVED_SLOT_MERCHANT_GOLD_DEPRECATED = 800,
    SAVED_SLOT_ITEM_SOCKETS = 1024,
    SAVED_SLOT_HIRELING_RIGHT_HAND = 1296,
    SAVED_SLOT_HIRELING_LEFT_HAND = 1312,
    SAVED_SLOT_HIRELING_SPECIAL = 1328,
    SAVED_SLOT_HIRELING_NECK = 1344,
    SAVED_SLOT_HIRELING_RIGHT_FINGER = 1360,
    SAVED_SLOT_HIRELING_LEFT_FINGER = 1376,
    SAVED_SLOT_UNK_570 = 1392,
    SAVED_SLOT_UNK_580 = 1408,
    SAVED_SLOT_UNK_590 = 1424,
    SAVED_SLOT_UNK_5a0 = 1440,
    SAVED_SLOT_UNK_5b0 = 1456,
    SAVED_SLOT_UNK_5c0 = 1472,
    SAVED_SLOT_UNK_5d0 = 1488,
    SAVED_SLOT_UNK_5e0 = 1504,

};

enum {
    MAX_VALID_DESTINATION_SLOTS = 4,
    MAX_STASH_TABS_CONSOLE = 5,
    MAX_STASH_TABS_VANILLA = 3,
    MAX_STASH_TABS_ROS = 6,
    MAX_STASH_TABS_GOLD_PURCHASABLE = 5,
    MAX_STASH_TABS_PURCHASABLE_A_LA_CARTE = 5,
    MAX_STASH_TABS_ACHIEVEMENT_REWARDS = 5,
    MAX_STASH_TABS_2_2 = 12,
    MAX_STASH_TABS = 13,
    HIRELING_SPECIAL_SLOT_UNLOCK_LEVEL = 18,
};

enum {
    PICKUP_ACTION_NONE = 0,
    PICKUP_ACTION_GLYPH = 1,
    PICKUP_ACTION_QUEST = 2,
    PICKUP_ACTION_MOVE_TO_LOCATION = 3,
    PICKUP_ACTION_STACK = 4,
    PICKUP_ACTION_USE_ON_PICKUP = 5,
    PICKUP_ACTION_CURRENCY = 6,
};

// PickupResult and PickupAction are in X/Common/ActorCommonData.h (their DWARF home).

enum {
    BUY_RESPONSE_INVALID = -1,
    BUY_RESPONSE_OKAY = 0,
    BUY_RESPONSE_CANT_AFFORD = 1,
    BUY_RESPONSE_CANT_PICKUP = 2,
    BUY_RESPONSE_ITEM_NOT_AT_VENDOR = 3,
    BUY_RESPONSE_COUNT = 4,
};

typedef int32 BuyResponse;

enum EInvItemClass {
    IIC_NONE = 0,
    IIC_ONEHANDED = 1,
    IIC_TWOHANDED = 2,
    IIC_SHIELD = 3,
    IIC_QUIVER = 4,
    IIC_OFFHAND = 5,
};

enum ActorInventoryDatabaseAction {
    AIDA_CREATE = 0,
    AIDA_REMOVE = 1,
    AIDA_UPDATE_MOVE = 2,
    AIDA_UPDATE_DROP = 3,
    AIDA_UPDATE_STACK = 4,
    AIDA_UPDATE_IDENTIFY = 5,
    AIDA_UPDATE_JEWEL_UPGRADE = 6,
    AIDA_UPDATE_CRAFT = 7,
    AIDA_UPDATE_DYE = 8,
    AIDA_UPDATE_TRANSMOG = 9,
    AIDA_UPDATE_ENCHANT_START = 10,
    AIDA_UPDATE_TRADE_TIMER_PERIODIC = 11,
    AIDA_UPDATE_TRADE_TIMER_COMPLETE = 12,
    AIDA_UPDATE_DURABILITY = 13,
    AIDA_UPDATE_STASH_TO_INVENTORY = 14,
    AIDA_UPDATE_INVENTORY_TO_STASH = 15,
    AIDA_UPDATE_STASH_TO_STASH = 16,
    AIDA_UPDATE_GEM_ADDED = 17,
    AIDA_UPDATE_GEM_REMOVED = 18,
    AIDA_UPDATE_STASH_TO_SOCKET = 19,
    AIDA_UPDATE_INVENTORY_TO_SOCKET = 20,
    AIDA_UPDATE_SOCKET_TO_INVENTORY = 21,
    AIDA_UPDATE_SOCKET_DROP = 22,
    AIDA_UPDATE_SOCKET_MOVED = 23,
    AIDA_UPDATE_LIMBO = 24,
    AIDA_LAST = 25,
};

enum ActorInventoryDatabaseReaction {
    AIDR_NONE = 0,
    AIDR_GENERATOR = 1,
    AIDR_OWNER = 2,
    AIDR_LOCATION = 4,
    AIDR_FROM_INVENTORY = 16,
    AIDR_FROM_STASH = 32,
};

struct ActorInventoryDatabaseRecord {  // size 0x80; line 259
    ACDID idACD;  // 0x00
    ActorInventoryDatabaseAction eAction;  // 0x04
    ActorInventoryDatabaseReaction uActionMask;  // 0x08
    InventorySlotType eDestination;  // 0x0c
    D3::Items::SavedItem tSavedItem;  // 0x10
    bool bNewItem;  // 0x60
    uint64 firstAccountId;  // 0x68
    uint64 lastAccountId;  // 0x70
    uint32 uUpdateCounter;  // 0x78
};
static_assert(sizeof(ActorInventoryDatabaseRecord) == 0x80, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, eAction) == 0x4, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, uActionMask) == 0x8, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, eDestination) == 0xc, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, tSavedItem) == 0x10, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, bNewItem) == 0x60, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, firstAccountId) == 0x68, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, lastAccountId) == 0x70, "ActorInventoryDatabaseRecord");
static_assert(offsetof(ActorInventoryDatabaseRecord, uUpdateCounter) == 0x78, "ActorInventoryDatabaseRecord");

typedef blz::unordered_map<unsigned long, ActorInventoryDatabaseRecord, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, ActorInventoryDatabaseRecord> > > DirtyInventoryRecordMap;

MailRecordMap & ACDMailMapGet();  // ActorInventory.cpp:12800
int32 ACDDirtyListGetSize();  // ActorInventory.cpp:12810
void ACDPurgeGlobalItemLists(DirtyInventoryRecordMap * ptMapToCopyTo);  // ActorInventory.cpp:12821
void ACDUpdateActorPersistState(const DirtyInventoryRecordMap & tDirtyMap);  // ActorInventory.cpp:12839

enum ActorInventoryStorageBucket {
    AISB_NONE = 0,
    AISB_HERO = 1,
    AISB_STASH = 2,
};

enum ACDInventoryIterateFlags {
    ACDII_NONE = 0,
    ACDII_CONTAINED_ITEMS = 1,
    ACDII_IGNORE_SOCKET_SLOTS = 2,
};

enum StashTabsRewardedFromSeasons {
    STASH_TABS_REWARDED_FROM_SEASONS_ATTRIBUTE_COUNT_MASK = 65535,
    STASH_TABS_REWARDED_FROM_SEASONS_ATTRIBUTE_LAST_SEASON_OFFSET = 16,
};

BuyResponse ACDInventoryCanBuy(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);  // ActorInventory.cpp:10651
BOOL ACDInventoryHirelingSlotUnlocked(const ActorCommonData & tACDHireling);  // ActorInventory.cpp:582
int32 ACDInventoryGetStashUpgradeCost(ActorCommonData & tACDPlayer);  // ActorInventory.cpp:12455

struct InventoryIterator {  // size 0x18
    inline InventoryIterator(ActorCommonData * ptACD)
    {
        m_ptACD = ptACD;
        m_idItem = ACDID_INVALID;
        m_nCurrentSlot = -1;
        m_nCurrentPosition = -1;
    }
    ActorCommonData * m_ptACD;  // 0x00
    ACDID m_idItem;  // 0x08
    int32 m_nCurrentSlot;  // 0x0c
    int32 m_nCurrentPosition;  // 0x10
};
BOOL ACDInventoryIterate(InventoryIterator & tIt);  // ActorInventory.cpp:9826

// Inventory location as clients send it (X/Server/Network/ClientValidation.cpp).
struct InvLoc {  // size 0x10
    ACDNetworkName annOwner;  // 0x00
    InventorySlotType eSlot;  // 0x04
    int32 nSlotX;  // 0x08
    int32 nSlotY;  // 0x0c
};
static_assert(sizeof(InvLoc) == 0x10, "InvLoc");

// X/Common/ActorInventory.h:704
struct InventoryRequestMoveMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    InvLoc tLocation;  // 0x0c
    BOOL operator==(const InventoryRequestMoveMessage & tOther) const { return annItem == tOther.annItem; }  // line 710
};
static_assert(sizeof(InventoryRequestMoveMessage) == 0x1c, "InventoryRequestMoveMessage");

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(InventoryIterator) == 0x18, "InventoryIterator");
static_assert(offsetof(InventoryIterator, m_ptACD) == 0x0, "InventoryIterator");
static_assert(offsetof(InventoryIterator, m_idItem) == 0x8, "InventoryIterator");
static_assert(offsetof(InventoryIterator, m_nCurrentSlot) == 0xc, "InventoryIterator");
static_assert(offsetof(InventoryIterator, m_nCurrentPosition) == 0x10, "InventoryIterator");
static_assert(sizeof(InvLoc) == 0x10, "InvLoc");
static_assert(offsetof(InvLoc, annOwner) == 0x0, "InvLoc");
static_assert(offsetof(InvLoc, eSlot) == 0x4, "InvLoc");
static_assert(offsetof(InvLoc, nSlotX) == 0x8, "InvLoc");
static_assert(offsetof(InvLoc, nSlotY) == 0xc, "InvLoc");
static_assert(sizeof(InventoryRequestMoveMessage) == 0x1c, "InventoryRequestMoveMessage");
static_assert(offsetof(InventoryRequestMoveMessage, tHeader) == 0x0, "InventoryRequestMoveMessage");
static_assert(offsetof(InventoryRequestMoveMessage, annItem) == 0x8, "InventoryRequestMoveMessage");
static_assert(offsetof(InventoryRequestMoveMessage, tLocation) == 0xc, "InventoryRequestMoveMessage");

BOOL SACDInventoryProcessSocketRequest(ActorCommonData & tACDPlayer, ActorCommonData & tACDGem, ActorCommonData & tACDItemToReceiveGem);

BOOL SACDInventoryDeleteItemAfterUse(ActorCommonData & tACDItem);

BOOL SACDInventoryPickupOrSpillOntoGround(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const BOOL bTryStashFirst, ACDID * pidACDStackDest, const BOOL bAllowAutoEquip);  // ActorInventory.cpp:5218

ACDID ActorInventoryGetUltimateOwner(const ActorCommonData * ptACDItem);  // ActorInventory.cpp:450

void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);  // ActorInventory.cpp:9743

BOOL ACDInventoryMove(ActorCommonData & tACDItem, const InventoryLocation & tDstLocation);  // ActorInventory.cpp:4018
void ACDInventoryDrop(ActorCommonData & tACDWantingToDrop, ActorCommonData & tACDItem, const WorldPlace * ptPlace);  // ActorInventory.cpp:3348

// Owning-header placement inferred; raw DWARF has only ActorInventory.cpp:1455, no header declaration.
BOOL ACDInventoryInitialize(ActorCommonData & tACD, BOOL fForceInventoryCreate);
