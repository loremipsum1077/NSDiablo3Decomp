#include "../../../Common/ActorCommonData.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
// X/Common/Definitions/GameBalanceDefinition.h
enum {
    IQL_LEGENDARY = 9,
    IQL_SPECIAL = 10,
    IQL_DISPLAY_AS_SET_ITEM = 11,
};
typedef int32 ItemQualityLevel;
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACDItem);  // X/Common/Items.cpp

namespace UISalvage {

//----------------------------------------------------------------------------

BOOL ShouldWarnOnSalvage(
    const ActorCommonData & tACDItem)
{
    ItemQualityLevel eIQL = ItemGetDisplayedQualityLevel(tACDItem);
    return eIQL == IQL_LEGENDARY || eIQL == IQL_DISPLAY_AS_SET_ITEM;
}

}  // namespace UISalvage
