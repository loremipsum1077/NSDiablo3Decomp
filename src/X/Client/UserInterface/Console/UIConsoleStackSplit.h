// Original file: X/Client/UserInterface/Console/UIConsoleStackSplit.h

#pragma once

#include "../../../Common/CommonTypes.h"

typedef void (*UIStackSplitCallback)(const ACDID, const uint64);

namespace UIStackSplit {
namespace Console {

enum StackSplitType {
    STACKSPLIT_GENERIC = 0,
    STACKSPLIT_GOLD_TRADE = 1,
    STACKSPLIT_GOLD_MAIL = 2,
    STACKSPLIT_CRAFTING_MAT_TRADE = 3,
    STACKSPLIT_CRAFTING_MAT_MAIL = 4,
};

}  // namespace Console
}  // namespace UIStackSplit
