#include "../../../Common/CommonTypes.h"
#include "../../../Common/Items.h"
#include "../../ImageCollection.h"
#include "../UI.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../../Contrib/Contrib/blz/include/blz/set.h"
#include "UITransmogrifyCommon.h"

// DWARF Items.cpp:1883; declaration only, using canonical GBID and PlayerClass types.
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);






static int32 sCompareTransmogGBIDs(
    const void * lhs,
    const void * rhs)
{
    using namespace UITransmogrify;






    // Inference: the qsort callback arguments have the DWARF DisplayedTransmog pointer types.
    const UITransmogrify::DisplayedTransmog * pTransmogLeft = static_cast<const UITransmogrify::DisplayedTransmog *>(lhs);
    ScopedItemDefinitionLockR pdefTransmogLeft(pTransmogLeft->gbidTransmog);
    // Inference: same typed callback conversion for rhs; no field-offset pointer arithmetic.
    const UITransmogrify::DisplayedTransmog * pTransmogRight = static_cast<const UITransmogrify::DisplayedTransmog *>(rhs);
    ScopedItemDefinitionLockR pdefTransmogRight(pTransmogRight->gbidTransmog);



    BOOL bLeftIsSet = ItemIsSetItem(pTransmogLeft->gbidTransmog);
    BOOL bRightIsSet = ItemIsSetItem(pTransmogRight->gbidTransmog);
    if (bLeftIsSet != bRightIsSet)
    {
        return bLeftIsSet ? 1 : -1;
    }

    BOOL bLeftIsLegendary = ItemIsLegendary(pTransmogLeft->gbidTransmog);
    BOOL bRightIsLegendary = ItemIsLegendary(pTransmogRight->gbidTransmog);
    if (bLeftIsLegendary != bRightIsLegendary)
    {
        return bLeftIsLegendary ? 1 : -1;
    }
    // Inference: ordered level comparisons yield +1, -1, or zero in the original branches.
    return pdefTransmogLeft->nItemLevel > pdefTransmogRight->nItemLevel ? 1 :
           pdefTransmogLeft->nItemLevel < pdefTransmogRight->nItemLevel ? -1 : 0;

}


namespace UITransmogrify {

void EnumDisplayedTransmogs(
    const Player * ptPlayer,
    InventorySlotType eSelectedSlot,
    const ActorCommonData * ptACDItem,
    UITransmogrify::DisplayedTransmogArray & arTransmogs,
    BOOL bAllowUnavailable)
{
    XGrowableMap<int, int, UITransmogrify::DisplayedTransmog, const UITransmogrify::DisplayedTransmog &> mapTransmogs;
    blz::set<unsigned int> setIconsToTransmog;

    GBHandleList listItems(GBGetHandlePool());
    GBEnumerate(GB_ITEMS, listItems);
    // Inference: enumerate the handle list in its original forward order.
    POS pos = listItems.GetHeadPosition();
    while (pos)
    {
        GBID gbidItem = listItems.GetNext(pos).gbid;
        BOOL bIsOriginalItemTransmog = ptACDItem && ItemCrafting_ItemTransmogsMatch(ptACDItem->hGB.gbid, gbidItem);
        // Inference: retain an original-item transmog even when it cannot otherwise be unlocked.
        if (!bIsOriginalItemTransmog && !ItemCrafting_CanUnlockTransmog(gbidItem, NULL, FALSE))
        {
            continue;
        }
        // Inference: SLOT_NONE is rejected for items and means no restriction for the selected slot.
        InventorySlotType eItemSlot = ItemCrafting_GetTransmogSlot(gbidItem);
        if (eItemSlot == SLOT_NONE || (eSelectedSlot != SLOT_NONE && eSelectedSlot != eItemSlot))
        {
            continue;
        }


        ScopedItemDefinitionLockR pdefItem(gbidItem);

        // Inference: initialize the displayed record from this item; original appearance also counts as unlocked.
        UITransmogrify::DisplayedTransmog tDisplayedTransmog;
        tDisplayedTransmog.gbidTransmog = gbidItem;
        tDisplayedTransmog.snoActor = pdefItem->snoActor;
        tDisplayedTransmog.bUnlocked = ItemCrafting_IsTransmogUnlocked(gbidItem, ptPlayer, NULL) || bIsOriginalItemTransmog;
        tDisplayedTransmog.bCanUnlock = !tDisplayedTransmog.bUnlocked && ItemCrafting_CanUnlockTransmog(gbidItem, ptPlayer, TRUE);

        UITransmogrify::DisplayedTransmog tExistingDisplayedTransmog;
        if (mapTransmogs.Lookup(pdefItem->snoActor, tExistingDisplayedTransmog))
        {
            // Inference: existing unlocked state dominates unlockable state in the original selects.
            if (!tDisplayedTransmog.bUnlocked)
            {
                if (tExistingDisplayedTransmog.bUnlocked)
                {
                    tDisplayedTransmog.bUnlocked = TRUE;
                    tDisplayedTransmog.bCanUnlock = FALSE;
                }
                else if (!tDisplayedTransmog.bCanUnlock)
                {
                    tDisplayedTransmog.bCanUnlock = tExistingDisplayedTransmog.bCanUnlock;
                }
            }

            // Inference: positive comparator result retains the existing representative.
            if (ItemCrafting_CompareMatchingTransmogs(gbidItem, tExistingDisplayedTransmog.gbidTransmog) > 0)
            {
                tDisplayedTransmog.gbidTransmog = tExistingDisplayedTransmog.gbidTransmog;
            }
        }

        mapTransmogs.SetAt_Tracked(pdefItem->snoActor, tDisplayedTransmog);
    }

    // Inference: traverse actor-keyed representatives after completing the item list.
    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);
    pos = mapTransmogs.GetStartAssoc();
    while (pos)
    {
        SNO snoActor;
        UITransmogrify::DisplayedTransmog tDisplayedTransmog;
        mapTransmogs.GetNextAssoc(pos, snoActor, tDisplayedTransmog);
        // Inference: original unavailable guard and ordered item/class checks govern both append paths.
        if (bAllowUnavailable ||
            ((tDisplayedTransmog.bUnlocked || tDisplayedTransmog.bCanUnlock) &&
             ptACDItem && ItemCrafting_CanTransmogItem(*ptACDItem, tDisplayedTransmog.gbidTransmog, ptPlayer, TRUE, TRUE) &&
             ItemMeetsOwnerClassRestrictions(tDisplayedTransmog.gbidTransmog, ePlayerClass, FALSE)))
        {
            if (ePlayerClass == PLAYER_CLASS_NECROMANCER)
            {
                // Inference: original offsets select the necromancer default icon, falling back to barbarian.
                ScopedActorDefinitionLockR pdefActor(snoActor);
                ImageHandle hItemIcon = pdefActor->tInvImages[PLAYER_CLASS_NECROMANCER].hDefaultImage;

                if (hItemIcon == 0)
                {

                    hItemIcon = pdefActor->tInvImages[PLAYER_CLASS_BARBARIAN].hDefaultImage;
                }

                if (setIconsToTransmog.find(hItemIcon) == setIconsToTransmog.end())
                {
                    setIconsToTransmog.insert(hItemIcon);
                    arTransmogs.push_back(tDisplayedTransmog);
                }
            }
            else
            {
                arTransmogs.push_back(tDisplayedTransmog);
            }
        }
    }

    // Inference: the original passes the file-local comparator to the existing vector-sort helper.
    blzext::vector_sort(arTransmogs, sCompareTransmogGBIDs);
}




void UpdateTransmogLegendaryBackground(
    const UIID & idBackground,
    GBID gbidTransmog)
{
    UIWindow & tBackground = UI_GetWindowFromID(idBackground);

    if (ItemIsLegendary(gbidTransmog))
    {
        ImageTextureFrame tBackgroundTexFrame;
        if (ItemIsSetItem(gbidTransmog))
        {
            ImageCollectionGetImageInfo("UI:Quality_Large_Inventory_Set", tBackgroundTexFrame);
        }
        else
        {
            ImageCollectionGetImageInfo("UI:Quality_Large_Inventory_Legendary", tBackgroundTexFrame);
        }

        tBackground.SetTextureFrame(tBackgroundTexFrame);
        tBackground.SetActive(TRUE);
    }
    else
    {
        tBackground.SetActive(FALSE);
    }
}
} // namespace UITransmogrify
