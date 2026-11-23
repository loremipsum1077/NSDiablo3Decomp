#include "../../../Common/Hireling.h" // Original file: X/Client/UserInterface/Console/UIConsoleInventoryFilters.cpp.
#include "UIConsoleInventoryFilters.h"
#include "../../../Common/Items.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Crafting/Salvage.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../../../Common/CosmeticItems.h"
#include "../../../Common/Player.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include <string.h>
/* DWARF ActorInventory.cpp:927 and :11419. */ EInvItemClass GetInventoryItemClass(const GBID gbidItem); BOOL ACDCanDualWield(const ActorCommonData &tACD); /* DWARF Items.cpp:2074. */ BOOL ItemUsableByHireling(const GBID gbidItem, const HirelingClass eHirelingClass); /* DWARF Items.cpp:9191 and :2208. */ BOOL ItemIsDye(const ActorCommonData &tACDItem); ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData &tACD);
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Common/GlobalSNO.h"
// DWARF Items.cpp:3558 and UIConsoleTrade.cpp:709 external functions.
ItemEnchantAffixError ItemCanEnchantAffix(const ActorCommonData &tACDItem); namespace UITrade { namespace Console { BOOL ItemIsBeingOffered(const ACDID idACDItem); } }

#include "../../../Sigma/Sort.h"

#include "../../../Common/Definitions/RecipeDefinition.h"
#include "UIConsoleInventoryView.h"

namespace UIInventory { namespace Console {






struct ACDIDInfo {

    ACDID idACD;
    ItemQualityLevel eIQL;
    int32 eISC;
    GemType eGemType;
    int32 nGemQuality;
    int32 nNormalRequiredLevel;
    CHAR szDisplayName[36];
    BOOL bIdentified;

    static ACDIDInfo CreateFromACD(ActorCommonData &tACD);
};
static_assert(sizeof(ACDIDInfo) == 0x40, "ACDIDInfo");
struct FilterGlobals {

    ACDID idJewel;

    FilterGlobals() : idJewel(ACDID_INVALID) {}

};




typedef blz::vector<ACDIDInfo> ACDIDInfoArray;

static Sort::StableIndexSorter<ACDIDInfo> *sgptStableSorter;
FilterGlobals * gptFilterGlobals;




class IteratorCallbackData {
public:

    IteratorCallbackData(
        ACDID idOwnerACD,
        ACDIDArray &arItems,
        ItemQualityLevel eMinimumItemQuality)
        : m_idOwnerACD(idOwnerACD),
          m_arItems(arItems),
          m_eMinimumItemQuality(eMinimumItemQuality)
    {}



    ACDID m_idOwnerACD;
    ACDIDArray &m_arItems;
    ItemQualityLevel m_eMinimumItemQuality;

private:

    IteratorCallbackData &operator=(const IteratorCallbackData &);
};


BOOL sItemIteratorCallback(
    const InventoryLocation &tInvLoc,
    void *pCallBackData)
{
    const IteratorCallbackData *ptData = static_cast<const IteratorCallbackData *>(pCallBackData);

    const ActorCommonData &tACDOwner = ACDGet(ptData->m_idOwnerACD);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDOwner, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDItem != ACDID_INVALID)
    {
        ActorCommonData &tACDItem = ACDGet(idACDItem);
        if (ItemGetQualityLevel(tACDItem) >= ptData->m_eMinimumItemQuality)
        {
            ptData->m_arItems.push_back(idACDItem);
        }
    }
    // Inference: iterator always continues; original epilogue returns one.
    return TRUE;
}




void ItemFiltersInitializeForApp()

{
    sgptStableSorter = new Sort::StableIndexSorter<ACDIDInfo>;
    gptFilterGlobals = new FilterGlobals;
}




void ItemFiltersDisposeForApp()

{
    delete sgptStableSorter; sgptStableSorter = NULL;
    delete gptFilterGlobals; gptFilterGlobals = NULL;
}





static const LPCSTR sgarFilterTypeIconNames[FILTER_COUNT] = {
    "WeaponMainHand",
    "WeaponOffHand",
    "Ring",
    "Neck",
    "Ring",
    "Hands",
    "Shoulder",
    "Chest",
    "Helm",
    "Belt",
    "Legs",
    "Feet",
    "Wrist",
    "Bag",
    "WeaponMainHand",
    "Bag",
    "Gems",
    "Bag",
    "Bag",
    "Bag",
    "Gems",
    "Gems",
    "Gems",
    "Gems",
    "Gems",
    "Gems",
    "LegendaryGems",
    "Gems",
    "Junk",
    "Salvage",
    "MassSalvage",
    "Crafting",
    "Misc",
    "Treasure",
    "RiftKeys",
    "Cosmetics",
    "Cosmetics",
    "Cosmetics",
    "Cosmetics",
    "Cosmetics",
    "WeaponMainHand",
    "WeaponOffHand",
    "Ring",
    "Ring",
    "Neck",
    "Special",
    "Skill1",
    "Skill2",
    "Skill3",
    "Skill4",
    "Gems",
    "Gems",
    "Gems",
    "Gems",
    "Gems",
    "Sword",
    "Crossbow",
    "Dagger",
    "Wand",
    "Mace",
    "WeaponOffHand",
    "MightyWeapon",
    "Staff",
    "Axe",
    "FistWeapon",
    "Bow",
    "Flail",
    "Scythe",
    "Ring",
    "Ring",
    "Neck",
    "Hands",
    "Shoulder",
    "Chest",
    "Helm",
    "Belt",
    "Legs",
    "Feet",
    "Wrist",
    "Bag",
};









LPCSTR GetIconNameFromFilterType(
    const UIFilterType eFilterType) {
    if (eFilterType == FILTER_NONE) return "none";
    if (eFilterType == FILTER_ALL) return "all";
    return sgarFilterTypeIconNames[eFilterType];
}












typedef XGrowableMap<int, int, int, int> FilterToSlotMap;
FilterToSlotMap sgtFilterToSlotMap(16, NULL);


void LazyInitSlotMap() {
    if (!sgtFilterToSlotMap.IsEmpty()) return;
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_HEAD, SLOT_PLAYER_HEAD);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_CHEST, SLOT_PLAYER_TORSO);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_OFF_HAND, SLOT_PLAYER_RIGHT_HAND);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_MAIN_HAND, SLOT_PLAYER_LEFT_HAND);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_HANDS, SLOT_PLAYER_HANDS);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_BELT, SLOT_PLAYER_WAIST);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_FEET, SLOT_PLAYER_FEET);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_SHOULDERS, SLOT_PLAYER_SHOULDERS);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_LEGS, SLOT_PLAYER_LEGS);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_WRIST, SLOT_PLAYER_BRACERS);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_RING_RIGHT, SLOT_PLAYER_RIGHT_FINGER);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_RING_LEFT, SLOT_PLAYER_LEFT_FINGER);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_NECK, SLOT_PLAYER_NECK);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_FOLLOWER_MAIN_HAND, SLOT_HIRELING_LEFT_HAND);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_FOLLOWER_OFF_HAND, SLOT_HIRELING_RIGHT_HAND);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_FOLLOWER_NECK, SLOT_HIRELING_NECK);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_FOLLOWER_RING_RIGHT, SLOT_HIRELING_RIGHT_FINGER);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_FOLLOWER_RING_LEFT, SLOT_HIRELING_LEFT_FINGER);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_FOLLOWER_SPECIAL, SLOT_HIRELING_SPECIAL);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_RING_LEFT, SLOT_PLAYER_LEFT_FINGER);
    sgtFilterToSlotMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_RING_RIGHT, SLOT_PLAYER_RIGHT_FINGER);
}











InventorySlotType GetSlotForFilterType(
    const UIFilterType eFilterType) {
    LazyInitSlotMap();

    InventorySlotType eSlot = SLOT_NONE;
    sgtFilterToSlotMap.Lookup(eFilterType, eSlot);
    return eSlot;
}




BOOL FilterIsSlotBased(
    const UIFilterType eFilterType) {

    InventorySlotType eSlot = GetSlotForFilterType(eFilterType);
    return eSlot != SLOT_NONE && !(eSlot >= SLOT_HIRELING_START && eSlot <= SLOT_HIRELING_END);
}




BOOL FilterIsFollowerSlotBased(
    const UIFilterType eFilterType) {

    InventorySlotType eSlot = GetSlotForFilterType(eFilterType);
    if (eSlot == SLOT_NONE) return FALSE;
    return eSlot >= SLOT_HIRELING_START && eSlot <= SLOT_HIRELING_END;
}



BOOL FilterUsesAltSlotForComparisons(
    const UIFilterType eFilter) {
    if (eFilter == FILTER_OFF_HAND || eFilter == FILTER_RING_RIGHT ||
        eFilter == FILTER_FOLLOWER_OFF_HAND || eFilter == FILTER_FOLLOWER_RING_RIGHT) return TRUE;
    return FALSE;
}









BOOL ItemCanBeWornByThisPlayer(
    const ActorCommonData & tACDOwner,
    const ActorCommonData & tACDItem) {
    if (!ItemIsEquippable(tACDItem)) {
        return FALSE;
    }




    GameError eError; // The existing inferred call-result local below has no recorded DWARF DIE.
    BOOL bMeetsRequirements = ItemOwnerMeetsRequirements(tACDItem, tACDOwner, &eError);
    return bMeetsRequirements || eError != GAMEERROR_IARESULT_PLAYER_IS_WRONG_CLASS_FOR_ITEM;
}











void SetFilterJewel(
    const ACDID idJewel) {
 if (gptFilterGlobals) gptFilterGlobals->idJewel = idJewel; }









static BOOL sCanBeWornByThisFollower(
    const ActorCommonData &tACDOwner,
    const ActorCommonData &tACDItem)
{

    GameError eError;
    if (ItemOwnerMeetsRequirements(tACDItem, tACDOwner, &eError)) return TRUE;
    // Inference: requirement failure is accepted unless the error is wrong class (original cmp30).
    return eError != GAMEERROR_IARESULT_PLAYER_IS_WRONG_CLASS_FOR_ITEM;
}











BOOL ShouldIncludeSlotBasedItem(
    const ActorCommonData &tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType)
{
    if (eFilterType == FILTER_NONE) return FALSE;
    // Inference: FILTER_ALL shares the success tail; original entry compares -2.
    if (eFilterType == FILTER_ALL) return TRUE;






    InventorySlotType eSlot = GetSlotForFilterType(eFilterType);
    if (idACDItem == ACDID_INVALID || eSlot == SLOT_NONE) return FALSE;



    // Inference: failed ACD lookup branches to the common false return.
    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem) return FALSE;




    if (!ACDInventoryItemAllowedInThisSlotType(ptACDItem->hGB.gbid, eSlot)) return FALSE;




    if (eFilterType == FILTER_OFF_HAND)
    {

        EInvItemClass eItemClass = GetInventoryItemClass(ptACDItem->hGB.gbid);
        if (eItemClass == IIC_TWOHANDED) return FALSE;
        if (eItemClass == IIC_ONEHANDED) {
            if (!ACDCanDualWield(tACDOwner)) return FALSE;




            if (PlayerGetPlayerClass(tACDOwner.id) == PLAYER_CLASS_DEMONHUNTER &&
                !ItemHasLabel(ptACDItem->hGB.gbid, ITL_HAND_CROSSBOW)) return FALSE;
        }
    }
    // Inference: all remaining paths merge into the original true-return tail.
    return TRUE;









}





BOOL ShouldIncludeFollowerSlotBasedItem(
    const ActorCommonData &tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType)
{
    if (eFilterType == FILTER_NONE) return FALSE;
    // Inference: FILTER_ALL shares the success tail; original entry compares -2.
    if (eFilterType == FILTER_ALL) return TRUE;






    InventorySlotType eSlot = GetSlotForFilterType(eFilterType);
    if (idACDItem == ACDID_INVALID || eSlot < SLOT_HIRELING_START || eSlot > SLOT_HIRELING_END) return FALSE;



    // Inference: failed ACD lookup branches to the common false return.
    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem) return FALSE;




    if (!sCanBeWornByThisFollower(tACDOwner, *ptACDItem)) return FALSE;




    if (ItemIsSocketable(*ptACDItem)) return FALSE;





    InventoryLocation tInvLoc;
    tInvLoc.idACDContainer = tACDOwner.id;
    tInvLoc.eSlot = eSlot;
    tInvLoc.pt = cgptNull;
    GameError err = ACDInventoryItemAllowedInSlot(*ptACDItem, tInvLoc, FALSE, FALSE);
    if (err != GAMEERROR_NONE && err != GAMEERROR_IARESULT_HIRELING_DOESNT_MEET_ITEM_LEVEL)
    {



        if (eSlot == SLOT_HIRELING_SPECIAL && err == GAMEERROR_IARESULT_UNKNOWN && !ACDInventoryHirelingSlotUnlocked(tACDOwner))
        {
            if (!ItemHasLabel(ptACDItem->hGB.gbid, ITL_FOLLOWER_SPECIAL) || !ItemUsableByHireling(ptACDItem->hGB.gbid, HirelingGetClass(tACDOwner))) return FALSE;
        }
        // Inference: all other inventory errors return false before off-hand checks.
        else return FALSE;
    }






    if (eFilterType == FILTER_FOLLOWER_OFF_HAND)
    {

        EInvItemClass eItemClass = GetInventoryItemClass(ptACDItem->hGB.gbid);
        if (eItemClass == IIC_TWOHANDED) return FALSE;
        if (eItemClass == IIC_ONEHANDED) {
            if (!ACDCanDualWield(tACDOwner)) return FALSE;
        }
    }
    // Inference: remaining paths merge into original true-return tail.
    return TRUE;








}






BOOL ShouldIncludeLabelBasedItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType eFilterType) {
    if (eFilterType == FILTER_NONE) {
        return FALSE;
    }
    if (eFilterType == FILTER_ALL) {
        return TRUE;
    }




    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    return ptACDItem != 0;
}





BOOL ShouldIncludeDamagedItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType ) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    return ItemCanBeRepaired(*ptACDItem);
}







BOOL ShouldIncludeAvailableSocketItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType ) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    if (!ItemHasSockets(*ptACDItem)) {
        return FALSE;
    }




    const int32 nNumSocketsFilled = ItemGetNumFilledSockets(*ptACDItem);
    const int32 nTotalSockets = ptACDItem->AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS));
    return nTotalSockets > nNumSocketsFilled;
}





BOOL ShouldIncludeSocketedItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType ) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    if (!ItemHasSockets(*ptACDItem)) return FALSE;






    int32 nNumSocketsFilled = ItemGetNumFilledSockets(*ptACDItem);
    return nNumSocketsFilled > 0;
}




BOOL ShouldIncludeCanSocketUpgradeableJewelItem(
    const ActorCommonData &,
    ACDID idACDItem,
    const UIFilterType)
{
    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    // Inference: line-zero branch rejects an absent item before ItemHasSockets.
    if (!ptACDItem)
    {
        return FALSE;
    }

    if (!ItemHasSockets(*ptACDItem))
    {
        return FALSE;
    }

    int32 nNumSocketsFilled = ItemGetNumFilledSockets(*ptACDItem);
    int32 nTotalSockets = ptACDItem->AttributesGetInt(ATTRIBUTE_SOCKETS);
    if (nTotalSockets <= nNumSocketsFilled)
    {
        return FALSE;
    }

    if (!gptFilterGlobals)
    {
        return FALSE;
    }
    // Inference: the line-zero lookup exit also rejects an absent selected jewel.
    ActorCommonData *ptACDUpgradeableJewel = ACDTryToGet(gptFilterGlobals->idJewel);
    if (!ptACDUpgradeableJewel || !ItemIsUpgradeableJewel(*ptACDUpgradeableJewel))
    {
        return FALSE;
    }

    return ItemCanSocketInto(*ptACDUpgradeableJewel, *ptACDItem) == SOCKET_REASON_SUCCESS;
}





BOOL ShouldIncludeDyeableItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType ) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    return ItemIsDyeable(*ptACDItem);
}








BOOL ShouldIncludeCanHaveSocketsAddedItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType ) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    return IsSocketAddable(*ptACDItem, ATTRIBUTE_ADD_SOCKETS_TYPE_WEAPON) == -1;
}







BOOL ShouldIncludeGemItem(
    const ActorCommonData &,
    ACDID idACDItem,
    const UIFilterType eFilterType)
{
    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    // Inference: the ACD lookup's line-zero exit is a failed-item return.
    if (!ptACDItem)
        return FALSE;


    if (ItemIsGem(*ptACDItem))
    {
        if (eFilterType == FILTER_GEMS)
        {
            return TRUE;
        }
        else
        {
            ScopedItemDefinitionLockR pdefGem(ptACDItem->hGB.gbid);
            switch (eFilterType)
            {
            case FILTER_RUBY:
                return pdefGem->eGemType == GEM_RUBY;
            case FILTER_TOPAZ:
                return pdefGem->eGemType == GEM_TOPAZ;
            case FILTER_EMERALD:
                return pdefGem->eGemType == GEM_EMERALD;
            case FILTER_AMETHYST:
                return pdefGem->eGemType == GEM_AMETHYST;
            case FILTER_DIAMOND:
                return pdefGem->eGemType == GEM_DIAMOND;
            default:
                return FALSE;
            }





        }
    }

    return FALSE;
}





BOOL ShouldIncludeShardItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    return ItemIsShard(*ptACDItem);
}







BOOL ShouldIncludeJunkItem(
    const ActorCommonData & ,
    ACDID idACDItem,
    const UIFilterType ) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    return ItemIsJunk(*ptACDItem);
}








BOOL ShouldIncludeSalvageableItem(
    const ActorCommonData & tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    return SalvageCommon::ItemCanSalvage(*ptACDItem, tACDOwner) == -1;
}








BOOL ShouldIncludeMassSalvageableItem(
    const ActorCommonData & tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    if (SalvageCommon::ItemCanSalvage(*ptACDItem, tACDOwner) != GAMEERROR_NONE) return FALSE;






    ItemQualityLevel eIQL = ItemGetQualityLevel(*ptACDItem);
    return eIQL < IQL_LEGENDARY;
}











BOOL ShouldIncludeCraftingMaterialsItem(
    const ActorCommonData & tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    if (ItemIsCraftingReagent(*ptACDItem) && ItemIsCurrency(*ptACDItem)) {
        return TRUE;
    }
    return FALSE;
}








BOOL ShouldIncludeMiscellaneousItem(
    const ActorCommonData & tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) { return FALSE; }
    if (ACDInventoryItemAllowedInThisSlotType(ptACDItem->hGB.gbid, SLOT_HIRELING_SPECIAL)) { return TRUE; }
    if (ItemIsEquippable(*ptACDItem)) { return FALSE; }
    if (ItemIsGold(*ptACDItem)) { return FALSE; }
    if (ItemIsGem(*ptACDItem)) { return FALSE; }
    if (ItemIsUpgradeableJewel(*ptACDItem)) { return FALSE; }
    if (ItemIsShard(*ptACDItem)) { return FALSE; }
    if (ItemIsGift(ptACDItem->hGB.gbid)) { return FALSE; }
    if (ItemGetUseType(*ptACDItem) == ITEM_USE_TREASURE_BAG) { return FALSE; }
    if (ItemIsCosmeticItem(ptACDItem->hGB.gbid)) { return FALSE; }
    return !ItemHasLabel(ptACDItem->hGB.gbid, ITL_KNOWLEDGE_UTILITY);
}



















BOOL ShouldIncludeRiftKeyItem(
    const ActorCommonData & tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }
    return NephalemRift::IsRiftKey(idACDItem);
}






BOOL ShouldIncludeTreasureItem(
    const ActorCommonData &tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }

    if (!ItemIsEquippable(*ptACDItem) &&
        ItemGetUseType(*ptACDItem) == ITEM_USE_TREASURE_BAG) {
        return TRUE;
    }

    if (!ItemIsEquippable(*ptACDItem) &&
        ItemGetUseType(*ptACDItem) == ITEM_USE_PLAYER_GIFT) {
        return TRUE;
    }

    return FALSE;
}







BOOL ShouldIncludeCosmeticsItem(
    const ActorCommonData & tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem && ItemIsCosmeticItem(ptACDItem->hGB.gbid)) {
        if (eFilterType == FILTER_COSMETICS) return TRUE;










        CosmeticItemType eType = CosmeticItems_GetCosmeticItemTypeFromGBID(ptACDItem->hGB.gbid);
        if (eFilterType == FILTER_WINGS && eType == COSMETIC_ITEM_WINGS) return TRUE;
        if (eFilterType == FILTER_PETS && eType == COSMETIC_ITEM_PET) return TRUE;
        if (eFilterType == FILTER_PENNANTS && eType == COSMETIC_ITEM_PENNANT) return TRUE;
        if (eFilterType == FILTER_FRAMES && eType == COSMETIC_ITEM_FRAME) return TRUE;
    }
    return FALSE;
}


















BOOL ShouldIncludeUpgradeableJewelItem(
    const ActorCommonData & tACDOwner,
    ACDID idACDItem,
    const UIFilterType eFilterType) {



    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }

    return ItemIsUpgradeableJewel(*ptACDItem);
}






BOOL ShouldIncludeBagItem(
    const ActorCommonData &tACDOwner,
    ACDID idACDItem,
    const UIFilterType) {

    ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == 0) {
        return FALSE;
    }

    if (ACDInventoryItemAllowedInThisSlotType(ptACDItem->hGB.gbid, SLOT_HIRELING_SPECIAL)) {
        return TRUE;
    }

    if (ItemIsEquippable(*ptACDItem)) {
        return FALSE;
    }

    return !ItemIsGold(*ptACDItem);
}








void FilteredInventoryView::ClearItems() { m_arAllItems.clear(); }










void FilteredInventoryView::AddItems(
    const ACDID idACDInvSource,
    const ACDID idACDFilterTarget,
    const InventorySlotType eSlotType,
    const uint32 dwFlags,
    const ItemQualityLevel eMinimumItemQuality)
{


    m_dwFlags = dwFlags;

    IteratorCallbackData tCallbackData(idACDInvSource, m_arAllItems, eMinimumItemQuality);
    ActorCommonData &tACDInvSource = ACDGet(idACDInvSource);
    ACDInventoryIterateSlot(tACDInvSource, eSlotType, NULL, sItemIteratorCallback, &tCallbackData, FALSE, FALSE);

    if (dwFlags & (1u << FIV_INCLUDE_EQUIPPED_BIT))
    {

        for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; ++i)
        {
            InventorySlotType eBodySlot = i;
            ACDInventoryIterateSlot(tACDInvSource, eBodySlot, NULL, sItemIteratorCallback, &tCallbackData, FALSE, FALSE);
        }
    }
    if (dwFlags & (1u << FIV_INCLUDE_EQUIPPED_FOLLOWER_BIT))
    {
        ACDID idFollowerACD = PlayerGetHireling(tACDInvSource.id);
        const ActorCommonData *ptACDFollower = ACDTryToGet(idFollowerACD);
        // Inference: missing follower skips iteration, per original null branch.
        if (ptACDFollower)
        {
            tCallbackData.m_idOwnerACD = idFollowerACD;

            for (int32 i = SLOT_HIRELING_START; i < SLOT_HIRELING_END; ++i)
            {
                InventorySlotType eBodySlot = i;
                ACDInventoryIterateSlot(*ptACDFollower, eBodySlot, NULL, sItemIteratorCallback, &tCallbackData, FALSE, FALSE);
            }
        }
    }

    if ((dwFlags & ((1u << FIV_INCLUDE_UNIDENTIFIED_BIT) | (1u << FIV_ONLY_REPAIRABLE_BIT) |
                    (1u << FIV_EXCLUDE_OFFERED_ITEMS_BIT) | (1u << FIV_INCLUDE_JUNK_BIT) |
                    (1u << FIV_INCLUDE_PLAYER_GIFTS_BIT) | (1u << FIV_EXCLUDE_NON_ENCHANTABLE_BIT) |
                    (1u << FIV_EXCLUDE_ACCOUNT_BOUND_BIT) | (1u << FIV_EXCLUDE_TIERED_RIFT_KEYS_BIT))) !=
        ((1u << FIV_INCLUDE_UNIDENTIFIED_BIT) | (1u << FIV_INCLUDE_JUNK_BIT) | (1u << FIV_INCLUDE_PLAYER_GIFTS_BIT)))
    {




        for (int32 i = (int32)m_arAllItems.size() - 1; i >= 0; --i)
        {
            ActorCommonData &tACDItem = ACDGet(m_arAllItems[i]);
            if ((dwFlags & (1u << FIV_ONLY_REPAIRABLE_BIT)) && !ItemCanBeRepaired(tACDItem))
            { // Inference: original shared erase tail continues with the next item.
                blzext::vector_erase_reorder(m_arAllItems, i); continue;
            }

            if (!(dwFlags & (1u << FIV_INCLUDE_UNIDENTIFIED_BIT)) && ItemIsUnidentified(tACDItem))
            { // Inference: original shared erase tail continues with the next item.
                blzext::vector_erase_reorder(m_arAllItems, i); continue;
            }

            if (dwFlags & (1u << FIV_EXCLUDE_OFFERED_ITEMS_BIT))
            {
                if (UITrade::Console::ItemIsBeingOffered(tACDItem.id))
                { // Inference: original shared erase tail continues with the next item.
                    blzext::vector_erase_reorder(m_arAllItems, i); continue;
                }
            }

            if (!(dwFlags & (1u << FIV_INCLUDE_JUNK_BIT)) && ItemIsJunk(tACDItem))
            { // Inference: original shared erase tail continues with the next item.
                blzext::vector_erase_reorder(m_arAllItems, i); continue;
            }

            if (!(dwFlags & (1u << FIV_INCLUDE_PLAYER_GIFTS_BIT)) && ItemGetUseType(tACDItem) == ITEM_USE_PLAYER_GIFT)
            { // Inference: original shared erase tail continues with the next item.
                blzext::vector_erase_reorder(m_arAllItems, i); continue;
            }

            if ((dwFlags & (1u << FIV_EXCLUDE_NON_ENCHANTABLE_BIT)) && (ItemCanEnchantAffix(tACDItem) != IEAE_NONE || ItemHasLabel(tACDItem.hGB.gbid, ITL_POTION)))
            { // Inference: original shared erase tail continues with the next item.
                blzext::vector_erase_reorder(m_arAllItems, i); continue;
            }

            if ((dwFlags & (1u << FIV_EXCLUDE_ACCOUNT_BOUND_BIT)) && (tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_BOUND_TO_ACD)) !=
                ACDID_INVALID || ItemGetBoundToACD(tACDItem, TRUE) !=
                ACDID_INVALID || tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_CONSOLE_PROMO_ITEM)) !=
                0 || ItemIsOfType(tACDItem.hGB, GlobalGBIDGet(GLOBALGBID_ITEMTYPE_TREASURE_BAG))))
            { // Inference: original shared erase tail continues with the next item.
                blzext::vector_erase_reorder(m_arAllItems, i); continue;
            }


            if ((dwFlags & (1u << FIV_EXCLUDE_TIERED_RIFT_KEYS_BIT)) && NephalemRift::IsRiftKey(tACDItem.id))
            { // Inference: original shared erase tail continues with the next item.
                blzext::vector_erase_reorder(m_arAllItems, i); continue;
            }
        }
    }


    ActorCommonData &tACDFilterTarget = ACDGet(idACDFilterTarget);
    UpdateFilteredItems(tACDFilterTarget);
}




const ACDIDArray & FilteredInventoryView::GetItems(
    const UIFilterType eFilterType) const {
    if (eFilterType == FILTER_NONE) return m_arNoItems;
    if (eFilterType == FILTER_ALL) return m_arAllItems;
    return m_arFilteredItems[eFilterType];
}



BOOL ShouldIncludeFollowerSlotBasedItem(const ActorCommonData &, ACDID, UIFilterType);

ItemFilterPredicate FilteredInventoryView::m_fnSlotFilter = ShouldIncludeSlotBasedItem;
ItemFilterPredicate FilteredInventoryView::m_fnFollowerSlotFilter = ShouldIncludeFollowerSlotBasedItem;
ItemFilterPredicate FilteredInventoryView::m_fnLabelFilter = ShouldIncludeLabelBasedItem;
ItemFilterPredicate FilteredInventoryView::m_fnBagFilter = ShouldIncludeBagItem;
ItemFilterPredicate FilteredInventoryView::m_fnDamagedFilter = ShouldIncludeDamagedItem;
ItemFilterPredicate FilteredInventoryView::m_fnAvailableSocketsFilter = ShouldIncludeAvailableSocketItem;
ItemFilterPredicate FilteredInventoryView::m_fnSocketedFilter = ShouldIncludeSocketedItem;
ItemFilterPredicate FilteredInventoryView::m_fnCanSocketUpgradeableJewelFilter = ShouldIncludeCanSocketUpgradeableJewelItem;
ItemFilterPredicate FilteredInventoryView::m_fnDyeableFilter = ShouldIncludeDyeableItem;
ItemFilterPredicate FilteredInventoryView::m_fnCanAddSocketsFilter = ShouldIncludeCanHaveSocketsAddedItem;
ItemFilterPredicate FilteredInventoryView::m_fnGemsFilter = ShouldIncludeGemItem;
ItemFilterPredicate FilteredInventoryView::m_fnShardFilter = ShouldIncludeShardItem;
ItemFilterPredicate FilteredInventoryView::m_fnJunkItemFilter = ShouldIncludeJunkItem;
ItemFilterPredicate FilteredInventoryView::m_fnSalvageableFilter = ShouldIncludeSalvageableItem;
ItemFilterPredicate FilteredInventoryView::m_fnMassSalvageableFilter = ShouldIncludeMassSalvageableItem;
ItemFilterPredicate FilteredInventoryView::m_fnCraftingMaterialsFilter = ShouldIncludeCraftingMaterialsItem;
ItemFilterPredicate FilteredInventoryView::m_fnMiscellaneousItemsFilter = ShouldIncludeMiscellaneousItem;
ItemFilterPredicate FilteredInventoryView::m_fnTreasureFilter = ShouldIncludeTreasureItem;
ItemFilterPredicate FilteredInventoryView::m_fnRiftKeysFilter = ShouldIncludeRiftKeyItem;
ItemFilterPredicate FilteredInventoryView::m_fnUpgradeableJewelFilter = ShouldIncludeUpgradeableJewelItem;
ItemFilterPredicate FilteredInventoryView::m_fnCosmeticsFilter = ShouldIncludeCosmeticsItem;








ItemFilterPredicate FilteredInventoryView::GetPredicateForType(
    const UIFilterType eFilterType) {
    if (FilterIsSlotBased(eFilterType)) return m_fnSlotFilter;
    if (FilterIsFollowerSlotBased(eFilterType)) return m_fnFollowerSlotFilter;
    switch (eFilterType) {
    case FILTER_BAG: return m_fnBagFilter;
    case FILTER_DAMAGED: return m_fnDamagedFilter;
    case FILTER_AVAILABLE_SOCKETS: return m_fnAvailableSocketsFilter;
    case FILTER_SOCKETED: return m_fnSocketedFilter;
    case FILTER_CAN_SOCKET_UPGRADEABLE_JEWEL: return m_fnCanSocketUpgradeableJewelFilter;
    case FILTER_DYEABLE: return m_fnDyeableFilter;
    case FILTER_CAN_ADD_SOCKETS_WEAPONS: return m_fnCanAddSocketsFilter;
    case FILTER_GEMS: return m_fnGemsFilter;
    case FILTER_RUBY: return m_fnGemsFilter;
    case FILTER_TOPAZ: return m_fnGemsFilter;
    case FILTER_EMERALD: return m_fnGemsFilter;
    case FILTER_AMETHYST: return m_fnGemsFilter;
    case FILTER_DIAMOND: return m_fnGemsFilter;
    case FILTER_UPGRADEABLE_JEWEL: return m_fnUpgradeableJewelFilter;
    case FILTER_SHARD: return m_fnShardFilter;
    case FILTER_JUNK: return m_fnJunkItemFilter;
    case FILTER_SALVAGEABLE: return m_fnSalvageableFilter;
    case FILTER_MASS_SALVAGEABLE: return m_fnMassSalvageableFilter;
    case FILTER_CRAFTING_MATERIALS: return m_fnCraftingMaterialsFilter;
    case FILTER_MISCELLANEOUS_ITEMS: return m_fnMiscellaneousItemsFilter;
    case FILTER_TREASURE: return m_fnTreasureFilter;
    case FILTER_RIFT_KEYS: return m_fnRiftKeysFilter;
    case FILTER_COSMETICS: return m_fnCosmeticsFilter;
    case FILTER_WINGS: return m_fnCosmeticsFilter;
    case FILTER_PETS: return m_fnCosmeticsFilter;
    case FILTER_PENNANTS: return m_fnCosmeticsFilter;
    case FILTER_FRAMES: return m_fnCosmeticsFilter;
    default: return m_fnLabelFilter;
    }
}




















































BOOL sIsGemFilter(
    const UIFilterType eFilterType) { return eFilterType >= FILTER_RUBY && eFilterType <= FILTER_DIAMOND; }








ItemPartitioner FilteredInventoryView::GetPartitionerForType(
    const UIFilterType eFilterType,
    uint32 dwFlags) {
    if (eFilterType == FILTER_BAG || eFilterType == FILTER_MISCELLANEOUS_ITEMS) return m_fnBagPartitioner;
    if (sIsGemFilter(eFilterType)) return m_fnGemsPartitioner;
    if (eFilterType == FILTER_AVAILABLE_SOCKETS) return m_fnAvailableSocketsPartitioner;
    if ((dwFlags & ((1u << FIV_INCLUDE_EQUIPPED_BIT) | (1u << FIV_SHOW_EQUIPPED_LAST_BIT))) ==
        ((1u << FIV_INCLUDE_EQUIPPED_BIT) | (1u << FIV_SHOW_EQUIPPED_LAST_BIT))) return m_fnEquippedLastItemPartitioner;
    return m_fnItemPartitioner;
}



void PartitionEquippableItems(ACDIDArray & arItems, NewItemIndicatorManager<ACDID> * ptNewItemMgr, const BOOL bSortEquippedToLast);
void PartitionBagItems(ACDIDArray &, NewItemIndicatorManager<ACDID> *);
void PartitionGems(ACDIDArray &, NewItemIndicatorManager<ACDID> *);
void PartitionItemsWithSockets(ACDIDArray &, NewItemIndicatorManager<ACDID> *);
void PartitionEquippableItems(ACDIDArray &, NewItemIndicatorManager<ACDID> *);
void PartitionEquippedItemsLast(ACDIDArray &, NewItemIndicatorManager<ACDID> *);

ItemPartitioner FilteredInventoryView::m_fnBagPartitioner = PartitionBagItems;
ItemPartitioner FilteredInventoryView::m_fnGemsPartitioner = PartitionGems;
ItemPartitioner FilteredInventoryView::m_fnAvailableSocketsPartitioner = PartitionItemsWithSockets;
ItemPartitioner FilteredInventoryView::m_fnEquippedLastItemPartitioner = PartitionEquippedItemsLast;
ItemPartitioner FilteredInventoryView::m_fnItemPartitioner = PartitionEquippableItems;







void FilteredInventoryView::sUpdateItemsForFilter(
    const UIFilterType eFilterType,
    ACDIDArray &arFilteredItems,
    const ActorCommonData &tACDOwner) {
    arFilteredItems.clear();


    ItemFilterPredicate fnFilterPredicate = GetPredicateForType(eFilterType);
    for (uint32 j = 0; j < m_arAllItems.size(); ++j) {

        BOOL bIncludeItem = fnFilterPredicate ? fnFilterPredicate(tACDOwner, m_arAllItems[j], eFilterType) : TRUE;
        if (bIncludeItem) {
            arFilteredItems.push_back(m_arAllItems[j]);
        }
    }


    ItemPartitioner fnPartitioner = GetPartitionerForType(eFilterType, m_dwFlags);
    if (fnPartitioner) {
        fnPartitioner(arFilteredItems, m_ptNewItemMgr);
    }
}




void FilteredInventoryView::UpdateFilteredItems(
    const ActorCommonData &tACDOwner) {


    for (int32 i = 0; i < FILTER_FOLLOWER_START; ++i) {
        sUpdateItemsForFilter(i, m_arFilteredItems[i], tACDOwner);
    }



    ACDID idFollowerACD = PlayerGetHireling((ACDID)tACDOwner.id);
    const ActorCommonData *ptItemOwner = ACDTryToGet(idFollowerACD);
    if (ptItemOwner) {

        for (int32 i = FILTER_FOLLOWER_START; i < (FILTER_FOLLOWER_END + 1); ++i) {
            sUpdateItemsForFilter(i, m_arFilteredItems[i], *ptItemOwner);
        }
    }
}





static NewItemIndicatorManager<ACDID> *sgptNewItemMgr;

enum ItemSortClass {
    ISC_POTION, ISC_GEM, ISC_RIFT_KEY, ISC_DYE,
    ISC_TEMPLAR_ITEM, ISC_SCOUNDREL_ITEM, ISC_ENCHANTRESS_ITEM, ISC_OTHER
};








ItemSortClass sGetItemSortClass(
    ActorCommonData &tACD)
{
    if (ItemIsGem(tACD) || ItemIsUpgradeableJewel(tACD))
    {
        return ISC_GEM;
    }
    else if (NephalemRift::IsRiftKey(tACD.id))
    {
        return ISC_RIFT_KEY;
    }
    else if (ItemIsDye(tACD))
    {
        return ISC_DYE;
    }
    else if (ItemHasLabel(tACD.hGB.gbid, ITL_TEMPLAR_SPECIAL))
    {
        return ISC_TEMPLAR_ITEM;
    }
    else if (ItemHasLabel(tACD.hGB.gbid, ITL_SCOUNDREL_SPECIAL))
    {
        return ISC_SCOUNDREL_ITEM;
    }
    else if (ItemHasLabel(tACD.hGB.gbid, ITL_ENCHANTRESS_SPECIAL))
    {
        return ISC_ENCHANTRESS_ITEM;
    }
    else if (ItemHasLabel(tACD.hGB.gbid, ITL_POTION))
    {
        return ISC_POTION;
    }
    else
    {
        return ISC_OTHER;
    }
}

ACDIDInfo ACDIDInfo::CreateFromACD(
    ActorCommonData &tACD)
{
    ACDIDInfo info;
    info.idACD = tACD.id;
    info.eIQL = ItemGetDisplayedQualityLevel(tACD);
    Blizzard::String::Copy(info.szDisplayName, ItemGetDisplayName(tACD, FALSE, FALSE, FALSE, TRUE), sizeof(info.szDisplayName));
    info.eISC = sGetItemSortClass(tACD);
    info.nNormalRequiredLevel = 0;
    info.eGemType = 0;
    info.nGemQuality = 0;
    info.bIdentified = !ItemIsUnidentified(tACD);

    switch (info.eISC)
    {
    case ISC_GEM:
        {
            ScopedItemDefinitionLockR pdefItem(tACD.hGB.gbid);
            info.eGemType = pdefItem->eGemType;
            info.nGemQuality = tACD.AttributesGetInt(ATTRIBUTE_GEM_QUALITY);
        }
        break;
    case ISC_POTION:
        {

            ScopedItemDefinitionLockR pdefItem(tACD.hGB.gbid);
            info.nNormalRequiredLevel = pdefItem->nNormalRequiredLevel;
        }
        break;
    }


    return info;
}



int32 sSortEquippableItems(
    const ACDIDInfo tACDOne,
    const ACDIDInfo tACDTwo)
{

    if (tACDOne.bIdentified != tACDTwo.bIdentified)
    {
        return tACDOne.bIdentified ? 1 : -1;
    }


    ItemQualityLevel eIQL1 = tACDOne.eIQL;
    ItemQualityLevel eIQL2 = tACDTwo.eIQL;

    if (eIQL1 != eIQL2)
    {
        return eIQL2 - eIQL1;
    }


    return strcmp(tACDOne.szDisplayName, tACDTwo.szDisplayName);
}

int32 sSortBagItems(
    const ACDIDInfo tACDOne,
    const ACDIDInfo tACDTwo)
{

    ItemSortClass eISC1 = (ItemSortClass)tACDOne.eISC;
    ItemSortClass eISC2 = (ItemSortClass)tACDTwo.eISC;
    if (eISC1 != eISC2)
    {
        return eISC1 - eISC2;
    }

    switch (eISC1)
    {
    case ISC_GEM:
        {
            if (tACDOne.eGemType != tACDTwo.eGemType)
            {
                return tACDTwo.eGemType - tACDOne.eGemType;
            }

            int32 nGemQualityOne = tACDOne.nGemQuality;
            int32 nGemQualityTwo = tACDTwo.nGemQuality;
            // Inference: the line-zero branch returns unequal gem qualities, otherwise falls through to name comparison.
            if (nGemQualityOne != nGemQualityTwo)
            {
                return nGemQualityTwo - nGemQualityOne;
            }
        }
        break;
    case ISC_POTION:

        // Inference: line-zero subtraction returns unequal required levels.
        if (tACDOne.nNormalRequiredLevel != tACDTwo.nNormalRequiredLevel) {
            return tACDTwo.nNormalRequiredLevel - tACDOne.nNormalRequiredLevel;
        }
        break;
    case ISC_DYE:
        break;










    default:
        {
            ItemQualityLevel eIQL1 = tACDOne.eIQL;
            ItemQualityLevel eIQL2 = tACDTwo.eIQL;
            // Inference: the line-zero quality subtraction branches to name comparison on equality.
            if (eIQL1 != eIQL2)
            {
                return eIQL2 - eIQL1;
            }
        }
        break;
    }
    return strcmp(tACDOne.szDisplayName, tACDTwo.szDisplayName);
}

int32 sSortGemsOfSameType(
    const ACDIDInfo tACDOne,
    const ACDIDInfo tACDTwo)
{

    int32 nGemQualityOne = tACDOne.nGemQuality;
    int32 nGemQualityTwo = tACDTwo.nGemQuality;

    if (nGemQualityOne == nGemQualityTwo)
        return 0;
    else
        return nGemQualityTwo - nGemQualityOne;


}




void PartitionEquippableItems(
    ACDIDArray &arItems,
    NewItemIndicatorManager<ACDID> *ptNewItemMgr,
    const BOOL bSortEquippedToLast)
{
    sgptNewItemMgr = ptNewItemMgr;

    ACDIDInfoArray arEquippedItems, arNewItems, arJunkItems, arOtherItems;

    for (uint32 i = 0; i < arItems.size(); ++i)
    {
        ACDID idACDItem = arItems[i];
        ActorCommonData *pItemACD = ACDTryToGet(idACDItem);
        // Inference: lookup failure skips the item, per original null branch.
        if (pItemACD) {
            if (ItemIsEquipped(*pItemACD))
            {
                arEquippedItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
            else if (sgptNewItemMgr && sgptNewItemMgr->ItemIsNew(idACDItem))
            {
                arNewItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
            else if (ItemIsJunk(*pItemACD))
            {
                arJunkItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
            else
            {
                arOtherItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
        }
    }

    sgptStableSorter->Sort(&arNewItems[0], arNewItems.size(), sSortEquippableItems);
    sgptStableSorter->Sort(&arOtherItems[0], arOtherItems.size(), sSortEquippableItems);
    sgptStableSorter->Sort(&arJunkItems[0], arJunkItems.size(), sSortEquippableItems);

    arItems.clear();

    for (int32 i = 0; i < arNewItems.size(); ++i)
    {
        arItems.push_back(arNewItems[i].idACD);
    }

    if (!bSortEquippedToLast)
    {
        for (int32 i = 0; i < arEquippedItems.size(); ++i)
        {
            arItems.push_back(arEquippedItems[i].idACD);
        }
    }

    for (int32 i = 0; i < arOtherItems.size(); ++i)
    {
        arItems.push_back(arOtherItems[i].idACD);
    }
    for (int32 i = 0; i < arJunkItems.size(); ++i)
    {
        arItems.push_back(arJunkItems[i].idACD);
    }

    if (bSortEquippedToLast)
    {
        for (int32 i = 0; i < arEquippedItems.size(); ++i)
        {
            arItems.push_back(arEquippedItems[i].idACD);
        }
    }
}






void PartitionEquippableItems(
    ACDIDArray &arItems,
    NewItemIndicatorManager<ACDID> *ptNewItemMgr) { PartitionEquippableItems(arItems, ptNewItemMgr, FALSE); }









void PartitionEquippedItemsLast(
    ACDIDArray &arItems,
    NewItemIndicatorManager<ACDID> *ptNewItemMgr) { PartitionEquippableItems(arItems, ptNewItemMgr, TRUE); }







void PartitionBagItems(
    ACDIDArray &arItems,
    NewItemIndicatorManager<ACDID> *ptNewItemMgr)
{
    sgptNewItemMgr = ptNewItemMgr;

    ACDIDInfoArray arNonJunkItems, arNewItems, arJunkItems;

    for (uint32 i = 0; i < arItems.size(); ++i)
    {
        ACDID idACDItem = arItems[i];
        ActorCommonData *pItemACD = ACDTryToGet(idACDItem);
        // Inference: lookup failure skips the item, per original null branch.
        if (pItemACD) {
            if (sgptNewItemMgr && sgptNewItemMgr->ItemIsNew(idACDItem))
            {
                arNewItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
            else if (ItemIsJunk(*pItemACD))
            {
                arJunkItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
            else
            {
                arNonJunkItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
        }
    }

    sgptStableSorter->Sort(&arNewItems[0], arNewItems.size(), sSortBagItems);
    sgptStableSorter->Sort(&arNonJunkItems[0], arNonJunkItems.size(), sSortBagItems);
    sgptStableSorter->Sort(&arJunkItems[0], arJunkItems.size(), sSortBagItems);

    arItems.clear();
    for (int32 i = 0; i < arNewItems.size(); ++i)
    {
        arItems.push_back(arNewItems[i].idACD);
    }
    for (int32 i = 0; i < arNonJunkItems.size(); ++i)
    {
        arItems.push_back(arNonJunkItems[i].idACD);
    }
    for (int32 i = 0; i < arJunkItems.size(); ++i)
    {
        arItems.push_back(arJunkItems[i].idACD);
    }
}




void PartitionGems(
    ACDIDArray &arItems,
    NewItemIndicatorManager<ACDID> *ptNewItemMgr)
{


    ACDIDInfoArray arSortedItems;
    for (uint32 i = 0; i < arItems.size(); ++i)
    {
        ACDID idACDItem = arItems[i];
        ActorCommonData *pItemACD = ACDTryToGet(idACDItem);
        // Inference: absent ACDs are skipped by the original lookup failure branches.
        if (pItemACD)
            arSortedItems.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
    }


    sgptStableSorter->Sort(&arSortedItems[0], arSortedItems.size(), sSortGemsOfSameType);
    arItems.clear();
    for (int32 i = 0; i < arSortedItems.size(); ++i)
    {
        arItems.push_back(arSortedItems[i].idACD);
    }
}





void PartitionItemsWithSockets(
    ACDIDArray &arItems,
    NewItemIndicatorManager<ACDID> *ptNewItemMgr)
{
    sgptNewItemMgr = ptNewItemMgr;

    ACDIDInfoArray arEquipped, arOther;

    for (uint32 i = 0; i < arItems.size(); ++i)
    {
        ACDID idACDItem = arItems[i];
        ActorCommonData *pItemACD = ACDTryToGet(idACDItem);
        // Inference: lookup failure skips the item, per original null branch.
        if (pItemACD) {
            if (ItemIsEquipped(*pItemACD))
            {
                arEquipped.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
            else
            {
                arOther.push_back(ACDIDInfo::CreateFromACD(*pItemACD));
            }
        }
    }

    sgptStableSorter->Sort(&arEquipped[0], arEquipped.size(), sSortEquippableItems);
    sgptStableSorter->Sort(&arOther[0], arOther.size(), sSortEquippableItems);

    arItems.clear();
    for (int32 i = 0; i < arEquipped.size(); ++i)
    {
        arItems.push_back(arEquipped[i].idACD);
    }
    for (int32 i = 0; i < arOther.size(); ++i)
    {
        arItems.push_back(arOther[i].idACD);
    }
}




BOOL RemovedItemPredicate::operator()(
    const ACDID idItemACD)
{
    // Inference: line-zero ACD lookup failure shares the removed-item TRUE return.
    ActorCommonData *ptACDItem = ACDTryToGet(idItemACD);
    if (!ptACDItem || !ptACDItem->TryToGetInventoryLocation())
    {
        return TRUE;
    }

    InventorySlotType eSlot = ptACDItem->GetLocationSlot();
    if (eSlot < m_eBeginSlot || eSlot > m_eEndSlot)
    {
        return TRUE;
    }

    return FALSE;
}



// NON_MATCHING: 820 bytes versus 824; registers shifted by one and, in the !bIncludeEquipped loop copy, the ItemIsEquipped branch is inverted (ours jumps to the fnFilter block, the original falls into it). Re-laid on DWARF lines (s08), no byte change.
BOOL FilterHasNewItems(
    const UIFilterType eFilterType,
    const ACDID idPlayerACD,
    const UIInventory::Console::NewItemIndicatorManager<ACDID> & tNewItemsMgr,
    const BOOL bIncludeEquipped)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }

    ActorCommonData * ptACDOwner = ptACDPlayer;
    if (eFilterType >= FILTER_FOLLOWER_START && eFilterType <= FILTER_FOLLOWER_END)
    {
        ACDID idFollowerACD = PlayerGetHireling(idPlayerACD);
        if (idFollowerACD == ACDID_INVALID)  // inferred: the original compares the hireling id with -1 on this line (F0:1826:21), before ACDTryToGet at 1829
            return FALSE;

        ptACDOwner = ACDTryToGet(idFollowerACD);
        if (!ptACDOwner)
        {
            return FALSE;
        }
    }







    ItemFilterPredicate fnFilter = FilteredInventoryView::GetPredicateForType(eFilterType);
    const NewItemIndicatorManager<ACDID>::ItemMap & tItemMap = tNewItemsMgr.GetItemMap();
    POS pos;
    for (pos = tItemMap.GetStartAssoc(); pos; )
    {
        ACDID idItem;
        BOOL bValIgnored;
        tItemMap.GetNextAssoc(pos, idItem, bValIgnored);

        if (!bIncludeEquipped)
        {

            ActorCommonData * ptACDItem = ACDTryToGet(idItem);
            if (ptACDItem && ItemIsEquipped(*ptACDItem))
            {
                continue;
            }
        }


        if (fnFilter(*ptACDOwner, idItem, eFilterType))
        {
            return TRUE;
        }
    }



    return FALSE;
}



BOOL FilterHasNewItems(
    const UIFilterType eFilterType,
    const ACDID idPlayerACD,
    const NewItemIndicatorManager<GBID> &tNewItemsMgr)
{
    ActorCommonData *ptACDPlayer = ACDTryToGet(idPlayerACD);
    // Inference: absent player follows the line-zero FALSE exit before map traversal.
    if (!ptACDPlayer)
        return FALSE;


    const NewItemIndicatorManager<GBID>::ItemMap &tItemMap = tNewItemsMgr.GetItemMap();
    POS pos = tItemMap.GetStartAssoc();
    while (pos)
    {
        GBID gbidRecipe;
        BOOL bValIgnored;
        tItemMap.GetNextAssoc(pos, gbidRecipe, bValIgnored);

        ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
        if (!pdefRecipeEntry || !SNOFileExists(RECIPE_SNO, pdefRecipeEntry->snoRecipe))
        {
            continue;
        }


        ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntry->snoRecipe);
        if (!GBIDExists(pdefRecipe->tSpecificItem.gbidItem, GB_ITEMS))
        {
            continue;
        }

        ScopedItemDefinitionLockR pdefItem(pdefRecipe->tSpecificItem.gbidItem);
        if (!pdefItem)
        {
            continue;
        }


        GBID gbidItem = pdefRecipe->tSpecificItem.gbidItem;
        if (CrafterRecipes::FilterIncludesItem(gbidItem, eFilterType))
        {
            return TRUE;
        }

    }

    return FALSE;
}

} }
