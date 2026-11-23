// Layout check for X/Common/VisualInventory.h against the DWARF, and a replay of
// VisualEquipment::Init (X/Common/VisualInventory.cpp, 0x71005c9f20), which inlines
// VisualCosmeticItem::SetDefaults from the header (VisualItem::SetDefaults is the .cpp's own,
// VisualInventory.cpp:1145; its body here is read from the same disassembly).
#include "X/Common/VisualInventory.h"

#include <cstddef>

#define CHECK(T, m, off) static_assert(offsetof(T, m) == off, #T "::" #m)

static_assert(sizeof(VisualItem) == 0x10, "VisualItem");
CHECK(VisualItem, eDyeType, 0x04);
CHECK(VisualItem, eItemEffectType, 0x08);
CHECK(VisualItem, nEffectLevel, 0x0c);
static_assert(sizeof(VisualCosmeticItem) == 0x4, "VisualCosmeticItem");
static_assert(sizeof(VisualEquipment) == 0x90, "VisualEquipment");
CHECK(VisualEquipment, tCosmeticItems, 0x80);
static_assert(sizeof(VisualInventory) == 0x120, "VisualInventory");
CHECK(VisualInventory, tEquipmentLast, 0x90);
static_assert(sizeof(VisualInventoryMessage) == 0x9c, "VisualInventoryMessage");
CHECK(VisualInventoryMessage, ann, 0x08);
CHECK(VisualInventoryMessage, tEquipment, 0x0c);

void VisualItem::SetDefaults()
{
    gbidItem = INVALID_GBID;
    eDyeType = 0;
    eItemEffectType = 0;
    nEffectLevel = 0;
}

void VisualEquipment::Init()
{
    for (int32 i = 0; i < VIS_NUM_SLOTS; i++)
    {
        tVisualItem[i].SetDefaults();
    }
    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++)
    {
        tCosmeticItems[i].SetDefaults();
    }
}
