// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/GameBalance.h"
#include "../../../Sigma/RefString.h"

typedef int32 UIFilterType;

typedef int32 VisualInventoryType;

namespace UIInventory {
namespace Console {
enum DetailsViewMode {
    DETAILS_VIEW = 0,
    GEM_COMBINING_VIEW = 1,
    SOCKETING_VIEW = 2,
    GEM_REMOVAL_VIEW = 3,
    ITEM_CATEGORY_VIEW = 4,
    TRAINING_VIEW = 5,
    SALVAGE_VIEW = 6,
    ENCHANT_VIEW = 7,
    TRANSMOG_VIEW = 8,
    DYE_ITEM_VIEW = 9,
};

enum InfoPanelType {
    NO_INFO_PANEL = -1,
    CHARACTER_INFO_PANEL = 0,
    STASH_UPGRADE_INFO_PANEL = 1,
    REPAIR_SUMMARY_INFO_PANEL = 2,
    CRAFTING_SUMMARY_INFO_PANEL = 3,
    JEWELER_COMBINE_INFO_PANEL = 4,
    JEWELER_SOCKETING_INFO_PANEL = 5,
    JEWELER_REMOVE_INFO_PANEL = 6,
    BLACKSMITH_SALVAGE_INFO_PANEL = 7,
    BLACKSMITH_SALVAGE_RESULTS_PANEL = 8,
    VENDOR_INFO_PANEL = 9,
    MAIL_INBOX_PANEL = 10,
    MYSTIC_ENCHANT_LOCKED_PANEL = 11,
    SEASON_STASH_INFO_PANEL = 12,
};

struct SocketEffectInfo {  // size 0x38
    CRefString sItemTypeName;  // 0x00
    CRefString sDescription;  // 0x18
    GBHandle hgbItemType;  // 0x30
};
static_assert(sizeof(SocketEffectInfo) == 0x38, "SocketEffectInfo");

}  // namespace Console
}  // namespace UIInventory

// Functions defined in this file (bodies are not in the DWARF):
//   line   217  UIInventory::Console::SocketEffectInfo::SocketEffectInfo()  [inlined into 1 functions]
//   line   217  UIInventory::Console::SocketEffectInfo::~SocketEffectInfo()  [inlined into 1 functions]

// Variables defined in this file:
//   line   214  static const int32 UIInventory::Console::MAX_SOCKET_EFFECTS
