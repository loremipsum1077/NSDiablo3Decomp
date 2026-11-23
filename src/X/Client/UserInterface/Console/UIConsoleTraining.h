// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleTraining.h (inline members sit on their original lines, 2.6.2 DWARF)
#pragma once
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryView.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Crafting/ItemCraftingSwig.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/DataStructures/XDataStructuresShared.h"
#include "../../../Sigma/SigmaTypes.h"
class CRefString;
struct CrafterData;
namespace UIInventory {
namespace Console {
class TrainingItems : public UIInventory::Console::RecipeItemsProvider {  // size 0x50
public:
    TrainingItems() {}  // line 20
    virtual ~TrainingItems();  // vtable slot 0




    virtual const GBIDArray & GetRecipes() const  // vtable slot 15
    {

        return m_arrRecipes;
    }
    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType) const;  // vtable slot 6



    virtual UIInventory::Console::UIInventoryViewItemsProvider::ProviderType GetProviderType() const  // vtable slot 7
    {

        if (GetPlayer())
        {
            switch (GetPlayer()->eActiveCrafterType)
            {
            case CRAFTER_TYPE_JEWEL:
                return ITEM_PROVIDER_JEWELER_RECIPES;
            case CRAFTER_TYPE_MYSTIC:
                return ITEM_PROVIDER_MYSTIC_TRANSMOGS;
            default:
                break;
            }
        }

        return ITEM_PROVIDER_BLACKSMITH_RECIPES;
    }
    virtual CRefString GetItemName(const int32 nIndex) const;  // vtable slot 2
    virtual int32 GetIndexOfItem(const ACDID idItem) const;  // vtable slot 16
    virtual void Refresh();  // vtable slot 17
private:
    ACDIDArray m_arrItems;  // 0x20
    GBIDArray m_arrRecipes;  // 0x38
    void CleanupACDs();
    static void RefreshTrainingRewards(const Player & tPlayer, CrafterType eCrafterType, const CrafterData & tCrafterData, ACDIDArray & arrItems, GBIDArray & arrRecipes);
    static void RefreshMysticTrainingRewards(const CrafterData & tCrafterData, ACDIDArray & arrItems);
    static PFN_COMPARE GetSortFuncForCrafter(CrafterType eCrafterType);
};
static_assert(sizeof(TrainingItems) == 0x50, "TrainingItems");



















class UIInventoryTrainingView : public UIInventory::Console::UIInventoryView {  // size 0x60, line 84
public:
    UIInventoryTrainingView(const UIID & idRoot);
protected:
    virtual void OnShow(const BOOL bShow);  // vtable slot 6
    virtual void OnUpdate();  // vtable slot 12
    virtual void OnSelectedItemChanged();  // vtable slot 10
private:
    void UpdateUI();
};
static_assert(sizeof(UIInventoryTrainingView) == 0x60, "UIInventoryTrainingView");

}  // namespace Console
}  // namespace UIInventory
