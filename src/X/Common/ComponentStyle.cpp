// Original file: X/Common/ComponentStyle.cpp

#include "ActorCommonData.h"
#include "ComponentStyle.h"
#include "ItemsSwig.h"
#include "../Sigma/Unicode.h"

struct InventorySlotTable {  // size 0x28
    InventorySlotType eSlot;  // 0x00
    CHAR szName[4];  // 0x04
    CHAR szDesc[32];  // 0x08
};
static_assert(sizeof(InventorySlotTable) == 0x28, "InventorySlotTable");

struct ComponentStyleTable {  // size 0x34
    ComponentStyle eStyle;  // 0x00
    CHAR szName[16];  // 0x04
    CHAR szDesc[32];  // 0x14
};
static_assert(sizeof(ComponentStyleTable) == 0x34, "ComponentStyleTable");

static const InventorySlotTable sgtComponentSlotTable[6] = {
    { SLOT_PLAYER_TORSO,      "TRS", "" },
    { SLOT_PLAYER_HANDS,      "GLV", "" },
    { SLOT_PLAYER_FEET,       "BTS", "" },
    { SLOT_PLAYER_RIGHT_HAND, "RH1", "" },
    { SLOT_PLAYER_LEFT_HAND,  "LH1", "" },
    { SLOT_PLAYER_LEGS,       "LEG", "" },
};

LPCSTR InventorySlotToComponentString(InventorySlotType eSlot)
{
    int nNumArrayEntries = sizeof(sgtComponentSlotTable) / sizeof(sgtComponentSlotTable[0]);
    for (int i = 0; i < nNumArrayEntries; ++i)
    {
        const InventorySlotTable &tEntry = sgtComponentSlotTable[i];
        if (tEntry.eSlot == eSlot)
            return tEntry.szName;
    }
    return NULL;
}
