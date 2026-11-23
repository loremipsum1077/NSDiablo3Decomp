#pragma once
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../UIID.h"
#include "UIFunctions.h"
class ActorCommonData;
struct TransmuteRecipeIngredient;
struct TransmuteRecipe;
typedef int32 InventorySlotType;
namespace UITransmuteRecipes {
struct IngredientCount {
    GBID gbidItem;
    int32 nCount;
    IngredientCount();
};
struct CanCompleteCallbackData {
    ActorCommonData* ptACDPlayer;
    const TransmuteRecipeIngredient* ptIngredient;
    blz::vector<IngredientCount> arIngredientCounts;
    int32 nSpecificGBIDCount;
    BOOL bCanComplete;
    CanCompleteCallbackData();
};
static_assert(sizeof(IngredientCount) == 8, "IngredientCount");
static_assert(sizeof(CanCompleteCallbackData) == 0x30, "CanCompleteCallbackData");
void DisplayIngredients(const TransmuteRecipe*, const UIID&, const UIID&, const UIID&, const UIID&, const LPCSTR, UIFunctionPtr, ActorCommonData*);
void CanCompleteCallback(ACDID, InventorySlotType, void*);
}
