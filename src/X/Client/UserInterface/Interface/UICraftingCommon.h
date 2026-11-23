// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UICraftingCommon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"

typedef int32 CrafterType;
struct Player;
struct RecipeEntry;

namespace UICrafting {
typedef BOOL (*RecipeDefPredicate)(const RecipeEntry *);

void BuildRecipeListForCrafter(GBIDArray & arrRecipes, const Player * ptPlayer, CrafterType eCrafterType, RecipeDefPredicate fnPredicate);  // UICraftingCommon.cpp:21
ACDID GenerateItemFromRecipe(ACDNetworkName ann, GBID gbidRecipe);  // UICraftingCommon.cpp:79

}  // namespace UICrafting
