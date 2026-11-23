// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/UserInterface/Interface/UIInventory.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/SigmaTypes.h"

namespace D3 { namespace Items { class Generator; } }

typedef int32 TooltipType;

typedef int32 ItemQualityLevel;

typedef int32 PlayerClass;

enum {
    ICT_DEFAULT = 0,
    ICT_TRADING_PLAYER = 1,
};

typedef int32 InventoryControlType;

enum {
    INVENTORY_MAIN = 0,
    INVENTORY_INSPECT = 1,
    INVENTORY_EQUIPMENT_MANAGER = 2,
    INVENTORY_GENERIC_PLAYER_INSPECT = 3,
    INVENTORY_COUNT = 4,
};

typedef int32 UIInventoryControl;

namespace UIInventory {
class GenerateItemsAndGems {  // size 0x10
public:
    GenerateItemsAndGems();
    ~GenerateItemsAndGems();
    GenerateItemsAndGems(UIInventory::GenerateItemsAndGems &&);
    UIInventory::GenerateItemsAndGems & operator=(UIInventory::GenerateItemsAndGems && rhs);
    GenerateItemsAndGems(const UIInventory::GenerateItemsAndGems &);
    UIInventory::GenerateItemsAndGems & operator=(const UIInventory::GenerateItemsAndGems &);
    BOOL Init(const D3::Items::Generator &, const ACDNetworkName);
    void Destroy();
    ACDID GetItemACDID();
private:
    ACDID m_idACDItem;  // 0x00
    ACDID m_idACDSocketed[3];  // 0x04
};
static_assert(sizeof(GenerateItemsAndGems) == 0x10, "GenerateItemsAndGems");

}  // namespace UIInventory
