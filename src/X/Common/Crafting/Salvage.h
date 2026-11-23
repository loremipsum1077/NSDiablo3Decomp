// Original owner: X/Common/Crafting/Salvage.h. Query subset used by inventory filters.
#pragma once
#include "../ActorCommonData.h"
typedef int32 GameError;
namespace SalvageCommon { GameError ItemCanSalvage(const ActorCommonData & tACDItem, const ActorCommonData & tACDOwner); }

// Salvage.h: the remaining DWARF declarations used by Salvage.cpp.
enum {
    SAT_INVALID = -1,
    SAT_NORMAL = 0,
    SAT_MAGIC = 1,
    SAT_RARE = 2,
    SAT_NUM_TYPES = 3,
};
typedef int32 SalvageAllType;

namespace SalvageCommon {
SNO GetTreasureClass(const ActorCommonData & tACDItem);  // Salvage.cpp:22
SalvageAllType ItemGetSalvageAllType(const ActorCommonData & tACDItem, const ActorCommonData & tACDOwner);  // Salvage.cpp:161
}
