// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/VisualInventory.h
// Types and members sit on their DWARF decl_line. Inline bodies are written from the copies
// inlined into VisualEquipment::operator== / Init (VisualInventory.cpp),
// VisualInventorySetCosmeticItemInSlot and UIConsoleMystic's preview updates (llvm-objdump -l).
// The two Import helpers are declared only: their single caller, VisualEquipment::Import, calls
// DyeTypeTranslateFromSavedDyeType, which no header in src/ declares yet.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Client/RActors/RActors.h"
#include "ActorCommonData.h"
#include "CommonTypes.h"
#include "GameBalance.h"
#include "../Sigma/SigmaTypes.h"
namespace D3 { namespace Hero { class VisualCosmeticItem; } }
namespace D3 { namespace Hero { class VisualEquipment; } }
namespace D3 { namespace Hero { class VisualItem; } }
typedef int32 PlayerClass;
typedef int32 ItemEffectType;

















typedef int32 VisualInventorySlot; enum {
    VIS_INVALID = -1,
    VIS_HEAD = 0,
    VIS_TORSO = 1,
    VIS_FEET = 2,
    VIS_HANDS = 3,
    VIS_LEFT_HAND = 4,
    VIS_RIGHT_HAND = 5,
    VIS_SHOULDERS = 6,
    VIS_LEGS = 7,
    VIS_NUM_SLOTS = 8,
};




enum {
    VCIS_INVALID = -1,
    VCIS_WINGS = 0,
    VCIS_PENNANT = 1,
    VCIS_PET = 2,
    VCIS_PORTRAIT = 3,
    VCIS_FIRST = 0,
    VCIS_LAST = 3,
    VCIS_NUM_SLOTS = 4,
};





typedef int32 HairStyle; enum {
    HAIR_NONE = -1,
    HAIR_DEFAULT = 0,
    HAIR_HELMET_1 = 1,
    HAIR_HELMET_2 = 2,
    HAIR_BALD = 3,
    HAIR_COUNT = 4,
};







struct VisualItem {  // size 0x10

    void Export(D3::Hero::VisualItem & tItem) const;


    void Import(const D3::Hero::VisualItem & tItem);








    BOOL operator==(const VisualItem & tVisualItem) const
    {
        return eDyeType == tVisualItem.eDyeType
            && eItemEffectType == tVisualItem.eItemEffectType
            && gbidItem == tVisualItem.gbidItem
            && nEffectLevel == tVisualItem.nEffectLevel;
    }

    BOOL operator!=(const VisualItem & tVisualItem) const
    {
        return !(*this == tVisualItem);
    }

    void SetDefaults();  // VisualInventory.cpp:1145


    void Set(const GBID _gbidItem, const DyeType _eDyeType, const ItemEffectType _eItemEffectType, const int32 _nEffectLevel);





    GBID gbidItem;  // 0x00
    DyeType eDyeType;  // 0x04
    ItemEffectType eItemEffectType;  // 0x08
    int32 nEffectLevel;  // 0x0c
};
static_assert(sizeof(VisualItem) == 0x10, "VisualItem");

struct VisualCosmeticItem {  // size 0x4

    void Export(D3::Hero::VisualCosmeticItem & tItem) const;


    void Import(const D3::Hero::VisualCosmeticItem & tItem);





    BOOL operator==(const VisualCosmeticItem & tVisualItem) const
    {
        return gbidItem == tVisualItem.gbidItem;
    }

    BOOL operator!=(const VisualCosmeticItem & tVisualItem) const
    {
        return !(*this == tVisualItem);
    }

    void SetDefaults()
    {

        gbidItem = GBID_NONE;
    }

    void Set(const GBID _gbidItem)
    {

        gbidItem = _gbidItem;
    }

    GBID gbidItem;  // 0x00
};
static_assert(sizeof(VisualCosmeticItem) == 0x4, "VisualCosmeticItem");

struct VisualEquipment {  // size 0x90

    void Export(D3::Hero::VisualEquipment & tEquipmentList) const;


    void Import(const D3::Hero::VisualEquipment & tEquipmentList);


    BOOL operator==(const VisualEquipment & tVisualEquipment) const;

    BOOL operator!=(const VisualEquipment & tVisualEquipment) const
    {
        return !(*this == tVisualEquipment);
    }

    void Init();


    void LimitEffects();


    void FixOffHandWeapons();


    VisualItem tVisualItem[8];  // 0x00
    VisualCosmeticItem tCosmeticItems[4];  // 0x80
};
static_assert(sizeof(VisualEquipment) == 0x90, "VisualEquipment");

struct VisualInventory {  // size 0x120

    void Init();


    void InitializeLast();


    VisualEquipment tEquipmentCurrent;  // 0x00
    VisualEquipment tEquipmentLast;  // 0x90
};
static_assert(sizeof(VisualInventory) == 0x120, "VisualInventory");

typedef int32 VisualInventoryType; enum {
    VIT_INVALID = -1,
    VIT_PLAYER = 0,
    VIT_HIRELING = 1,
    VIT_NUM_TYPES = 2,
};





struct VisualInventoryMessage {  // size 0x9c

    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    VisualEquipment tEquipment;  // 0x0c
};
static_assert(sizeof(VisualInventoryMessage) == 0x9c, "VisualInventoryMessage");

VisualInventory* VisualInventoryGet(const ACDID idACD);  // VisualInventory.cpp:332
VisualInventorySlot VisualInventorySlotFromInventorySlot(const InventorySlotType eInventorySlot);  // VisualInventory.cpp:268
BOOL VisualInventoryShouldSwapWeapons(PlayerClass ePlayerClass, GBID gbidItemLeft, GBID gbidItemRight);  // VisualInventory.cpp:1262
typedef int32 VisualCosmeticItemSlot;  // X/Common/CosmeticItems.h
GBID VisualInventoryGetCosmeticItemInSlot(const ACDID idACD, const VisualCosmeticItemSlot eSlot);  // VisualInventory.cpp:569
void VisualInventorySetCosmeticItemInSlot(const ACDID idACD, const VisualCosmeticItemSlot eSlot, const GBID gbidItem);  // VisualInventory.cpp

// Owning-header placement inferred; raw DWARF has only VisualInventory.cpp:224, no header declaration.
void ACDVisualInventoryInitialize(ActorCommonData & tACD);
