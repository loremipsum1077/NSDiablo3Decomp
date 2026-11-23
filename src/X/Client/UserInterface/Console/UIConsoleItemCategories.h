// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleItemCategories.h

#pragma once
#include "../../../Sigma/SigmaTypes.h"

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 UIFilterType;

struct Player;
struct UIID;

namespace UIInventory {
namespace Console {
enum {
    CATEGORY_NONE = -1,
    CATEGORY_TREASURE = 0,
    CATEGORY_COSMETICS = 1,
    CATEGORY_MISC = 2,
    CATEGORY_GEMS = 3,
    CATEGORY_CRAFTING = 4,
    CATEGORY_RUBY = 5,
    CATEGORY_AMETHYST = 6,
    CATEGORY_EMERALD = 7,
    CATEGORY_TOPAZ = 8,
    CATEGORY_DIAMOND = 9,
    CATEGORY_UPGRADEABLE_JEWEL = 10,
    CATEGORY_SHARD = 11,
    CATEGORY_WINGS = 12,
    CATEGORY_PETS = 13,
    CATEGORY_PENNANTS = 14,
    CATEGORY_PORTRAIT_FRAMES = 15,
};

typedef int32 ItemCategory;

class UIInventoryCategoryView;
class UIInventoryViewItemsProvider;
class CosmeticItemsProvider;
class CraftingMatItemsProvider;

BOOL IsGemSubCategory( const ItemCategory eCategory );
BOOL IsCosmeticSubCategory( const ItemCategory eCategory );
UIFilterType GetFilterTypeForCategory( const ItemCategory eCategory, const BOOL bIsRecipeFilter );
void PopulateTopLevelCategories( UIInventoryCategoryView &tCategoryView );
void PopulateGemCategories( UIInventoryCategoryView &tCategoryView );
void PopulateCosmeticCategories( UIInventoryCategoryView &tCategoryView );
BOOL CategoryHasItems( const ItemCategory eCategory, const UIInventoryViewItemsProvider &tItemsProvider,
                       const BOOL bIsBrowsingRecipes );
BOOL CategoryHasCosmeticItems( const ItemCategory eCategory,
                               const UIInventoryViewItemsProvider &tItemsProvider,
                               const CosmeticItemsProvider &tCosmeticItemsProvider );
BOOL CategoryHasCraftingMatItems( const ItemCategory eCategory,
                                  const UIInventoryViewItemsProvider &tItemsProvider,
                                  const CraftingMatItemsProvider &tCraftingMatItemsProvider );
BOOL CategoryHasNewItems( const ItemCategory eCategory, const Player *ptPlayer );
BOOL CategoryHasNewEliteItems( const ItemCategory eCategory, const Player *ptPlayer );
void UpdateCommonCategoryUI( const UIID &idItemUI, const ItemCategory eCategory, const BOOL bEnabled,
                             const BOOL bHasNewItems, const BOOL bHasNewEliteItems );

}  // namespace Console
}  // namespace UIInventory
