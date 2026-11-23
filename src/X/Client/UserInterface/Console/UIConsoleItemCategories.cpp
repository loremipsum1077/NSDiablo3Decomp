// Original file: X/Client/UserInterface/Console/UIConsoleItemCategories.cpp
#include "UIConsoleItemCategories.h"
#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryView.h"
#include "ConsoleScreenManager.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"

namespace UIInventory {
namespace Console {
BOOL IsGemSubCategory( const ItemCategory eCategory )
{
    return eCategory >= CATEGORY_RUBY && eCategory <= CATEGORY_UPGRADEABLE_JEWEL;
}

BOOL IsCosmeticSubCategory( const ItemCategory eCategory )
{
    return eCategory >= CATEGORY_WINGS && eCategory <= CATEGORY_PORTRAIT_FRAMES;
}

UIFilterType GetFilterTypeForCategory( const ItemCategory eCategory, const BOOL bIsRecipeFilter )
{
    // Original constant tables recovered with tools/data.py.
    static const UIFilterType GEM_FILTERS[] = {FILTER_RUBY, FILTER_AMETHYST, FILTER_EMERALD, FILTER_TOPAZ, FILTER_DIAMOND, FILTER_UPGRADEABLE_JEWEL};
    static const UIFilterType GEM_RECIPE_FILTERS[] = {FILTER_JEWELER_RECIPES_RUBY, FILTER_JEWELER_RECIPES_AMETHYST, FILTER_JEWELER_RECIPES_EMERALD, FILTER_JEWELER_RECIPES_TOPAZ, FILTER_JEWELER_RECIPES_DIAMOND, FILTER_NONE};
    static const UIFilterType COSMETIC_FILTERS[] = {FILTER_WINGS, FILTER_PETS, FILTER_PENNANTS, FILTER_FRAMES};
    switch( eCategory )
    {
    case CATEGORY_GEMS:      return FILTER_GEMS;
    case CATEGORY_CRAFTING:  return FILTER_CRAFTING_MATERIALS;
    case CATEGORY_TREASURE:  return FILTER_TREASURE;
    case CATEGORY_COSMETICS: return FILTER_COSMETICS;
    case CATEGORY_MISC:      return FILTER_MISCELLANEOUS_ITEMS;
    case CATEGORY_SHARD:     return FILTER_SHARD;
    }

    if( IsGemSubCategory( eCategory ) )
    {
        return ( bIsRecipeFilter == 0 ? GEM_FILTERS : GEM_RECIPE_FILTERS )[eCategory - CATEGORY_RUBY];
    }
    else if( IsCosmeticSubCategory( eCategory ) )
    {
        return COSMETIC_FILTERS[eCategory - CATEGORY_WINGS];
    }

    return FILTER_NONE;
}

void PopulateTopLevelCategories( UIInventoryCategoryView &tCategoryView )
{
    tCategoryView.ClearCategories();
    tCategoryView.AddCategory( CATEGORY_TREASURE );
    tCategoryView.AddCategory( CATEGORY_COSMETICS );
    tCategoryView.AddCategory( CATEGORY_MISC );
    tCategoryView.AddCategory( CATEGORY_GEMS );

    if( !::Console::Screens::IsScreenSetOpen( ::Console::Screens::STASH_SCREEN_SET ) )
    {
        tCategoryView.AddCategory( CATEGORY_CRAFTING );
    }
}

void PopulateGemCategories( UIInventoryCategoryView &tCategoryView )
{
    tCategoryView.ClearCategories();

    tCategoryView.AddCategory( CATEGORY_UPGRADEABLE_JEWEL );
    tCategoryView.AddCategory( CATEGORY_DIAMOND );
    tCategoryView.AddCategory( CATEGORY_RUBY );
    tCategoryView.AddCategory( CATEGORY_TOPAZ );
    tCategoryView.AddCategory( CATEGORY_EMERALD );
    tCategoryView.AddCategory( CATEGORY_AMETHYST );
}

void PopulateCosmeticCategories( UIInventoryCategoryView &tCategoryView )
{
    tCategoryView.ClearCategories();

    tCategoryView.AddCategory( CATEGORY_WINGS );
    tCategoryView.AddCategory( CATEGORY_PETS );
    tCategoryView.AddCategory( CATEGORY_PENNANTS );
    tCategoryView.AddCategory( CATEGORY_PORTRAIT_FRAMES );
}

BOOL CategoryHasItems( const ItemCategory eCategory, const UIInventoryViewItemsProvider &tItemsProvider,
                       const BOOL bIsBrowsingRecipes )
{
    if( eCategory == CATEGORY_GEMS )
    {
        return CategoryHasItems( CATEGORY_RUBY, tItemsProvider, bIsBrowsingRecipes ) ||
               CategoryHasItems( CATEGORY_EMERALD, tItemsProvider, bIsBrowsingRecipes ) ||
               CategoryHasItems( CATEGORY_TOPAZ, tItemsProvider, bIsBrowsingRecipes ) ||
               CategoryHasItems( CATEGORY_AMETHYST, tItemsProvider, bIsBrowsingRecipes ) ||
               CategoryHasItems( CATEGORY_DIAMOND, tItemsProvider, bIsBrowsingRecipes ) ||
               CategoryHasItems( CATEGORY_UPGRADEABLE_JEWEL, tItemsProvider, bIsBrowsingRecipes );
    }

    const UIFilterType eFilterType = GetFilterTypeForCategory( eCategory, bIsBrowsingRecipes );

    return tItemsProvider.GetItemsWithFilter( eFilterType ).size() != 0;
}

BOOL CategoryHasCosmeticItems( const ItemCategory eCategory,
                               const UIInventoryViewItemsProvider &tItemsProvider,
                               const CosmeticItemsProvider &tCosmeticItemsProvider )
{
    const UIFilterType eFilterType = GetFilterTypeForCategory( eCategory, 0 );

    size_t nCosmetics = tItemsProvider.GetItemsWithFilter(eFilterType).size();
    nCosmetics += tCosmeticItemsProvider.GetItemsWithFilter(eFilterType).size();
    return nCosmetics != 0;
}

BOOL CategoryHasCraftingMatItems( const ItemCategory eCategory,
                                  const UIInventoryViewItemsProvider &tItemsProvider,
                                  const CraftingMatItemsProvider &tCraftingMatItemsProvider )
{
    const UIFilterType eFilterType = GetFilterTypeForCategory( eCategory, 0 );

    size_t nCraftingMats = tCraftingMatItemsProvider.GetItemsWithFilter(eFilterType).size();
    return nCraftingMats != 0;
}

static BOOL sCheckForNewItems( const ItemCategory eCategory, const ACDID idPlayerACD,
                               const NewItemIndicatorManager<ACDID> *ptNewItemManager )
{
    static UIFilterType arFilters[] = {FILTER_TREASURE, FILTER_COSMETICS, FILTER_MISCELLANEOUS_ITEMS, FILTER_GEMS, FILTER_CRAFTING_MATERIALS, FILTER_RUBY, FILTER_AMETHYST, FILTER_EMERALD, FILTER_TOPAZ, FILTER_DIAMOND, FILTER_UPGRADEABLE_JEWEL, FILTER_SHARD, FILTER_WINGS, FILTER_PETS, FILTER_PENNANTS, FILTER_FRAMES};
    if( eCategory == CATEGORY_NONE || ptNewItemManager == NULL )
    {
        return FALSE;
    }

    switch( eCategory )
    {
    case CATEGORY_GEMS:
        return sCheckForNewItems( CATEGORY_RUBY, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_TOPAZ, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_EMERALD, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_AMETHYST, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_DIAMOND, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_SHARD, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_UPGRADEABLE_JEWEL, idPlayerACD, ptNewItemManager );

    case CATEGORY_COSMETICS:
        return sCheckForNewItems( CATEGORY_WINGS, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_PETS, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_PENNANTS, idPlayerACD, ptNewItemManager ) ||
               sCheckForNewItems( CATEGORY_PORTRAIT_FRAMES, idPlayerACD, ptNewItemManager );
    }

    return FilterHasNewItems( arFilters[eCategory], idPlayerACD, *ptNewItemManager, TRUE );
}

BOOL CategoryHasNewItems( const ItemCategory eCategory, const Player *ptPlayer )
{
    return sCheckForNewItems( eCategory, ptPlayer->idPlayerACD, ptPlayer->ptNewItemManager );
}

BOOL CategoryHasNewEliteItems( const ItemCategory eCategory, const Player *ptPlayer )
{
    return sCheckForNewItems( eCategory, ptPlayer->idPlayerACD, ptPlayer->ptNewEliteItemManager );
}

void UpdateCommonCategoryUI( const UIID &idItemUI, const ItemCategory eCategory, const BOOL bEnabled,
                             const BOOL bHasNewItems, const BOOL bHasNewEliteItems )
{
    static LPCSTR arszBackgrounds[] = {
        "Treasure",
        "Cosmetics",
        "Misc",
        "Gems",
        "Crafting",
        "Ruby",
        "Amethyst",
        "Emerald",
        "Topaz",
        "Diamond",
        "Legendary",
        "Shard",
        "Wings",
        "Pets",
        "Pennants",
        "Portraits",
    };

    const UIID idBackgrounds = UIIDCreateFromParent( idItemUI, ".LayoutRoot.Backgrounds" );

    UIID idTest;
    idTest.SetFromParent( idBackgrounds, arszBackgrounds[eCategory] );

    const UIControlItemArray &aridChildren = UI_GetWindowFromID(idBackgrounds).GetChildList();
    for( uint32 i = 0; i < aridChildren.size(); i++ )
    {
        UIWindow &tWndCategory = UI_GetWindowFromID( aridChildren[i].id );
        tWndCategory.SetActive( aridChildren[i].id == idTest );
        tWndCategory.SetShader( bEnabled ? -1 : GlobalSNOGet( GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE ) );
    }

    const UIID idNewStar = UIIDCreateFromParent( idItemUI, ".LayoutRoot.ItemNew" );
    UI_GetWindowFromID( idNewStar ).SetActive( bHasNewItems && !bHasNewEliteItems );

    const UIID idNewEliteStar = UIIDCreateFromParent( idItemUI, ".LayoutRoot.ItemNewElite" );
    UI_GetWindowFromID( idNewEliteStar ).SetActive( bHasNewEliteItems );
}

}  // namespace Console
}  // namespace UIInventory
