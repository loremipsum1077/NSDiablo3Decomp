// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIDyeItemCommon.cpp

#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../../../Contrib/Contrib/blz/include/blz/iter_range.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/GameBalance.h"
#include "../../../Sigma/Random.h"

#include "../../../Common/Items.h"


namespace UIDyeItem { DyeType GetDyeTypeForItemGBID(GBID gbidItem); }


static int32 sGetSortedIndexForDye(
    GBID gbidDye)
{
    static const int32 arIndices[] = {
        -1, 18, 15, 1, 17, 2, 4, 3, 11, 10, 12, 5, 6, 16, 19, 7, 0, 9, 8, 14, 13, 20, 21,
    };























    DyeType eDyeType = UIDyeItem::GetDyeTypeForItemGBID(gbidDye);
    if ((eDyeType < DYE_TYPE_FIRST_COLOR || eDyeType >= MAX_DYE_VALUE) && eDyeType != DYE_TYPE_VANISHING)
    {
        return -1;
    }
    return arIndices[eDyeType];
}

namespace UIDyeItem {


// NON_MATCHING: 1088 vs 1080; the line-48 test (no NONE clause: columns 42/84 fit `(<FIRST || >=MAX) && != VANISHING`, original 43/84) now flattens to cset/cset/cbz/and as in the original, with the `and` operands swapped; the arTemp push_back growth picks csinc vs csel (+8 bytes).
void GetAllDyes(
    GBIDArray & arDyes)
{
    arDyes.clear();

    GBIDArray arTemp;
    GBHandleList listItems(GBGetHandlePool());
    GBEnumerate(GB_ITEMS, listItems);

    POS pos = listItems.GetHeadPosition();
    while (pos)
    {
        GBID gbidItem = listItems.GetNext(pos).gbid;
        if (ItemIsDye(gbidItem))
        {
            arTemp.push_back(gbidItem);
        }
    }


    arDyes.resize(arTemp.size(), GBID_NONE);
    for (GBID gbidDye : arTemp)
    {
        int32 nIndex = sGetSortedIndexForDye(gbidDye);
        if (nIndex != -1)
        {
            arDyes[nIndex] = gbidDye;
        }
    }


    int * iter = blz::remove_if(blz::begin(arDyes), blz::end(arDyes),
        [](GBID gbidDye) { return gbidDye == GBID_NONE || GetDyeTypeForItemGBID(gbidDye) == DYE_TYPE_NONE; });
    arDyes.resize(iter - blz::begin(arDyes));
}




DyeType GetDyeTypeForItemGBID(
    GBID gbidItem)
{
    DyeType eResult = DYE_TYPE_NONE;
    if (ItemIsDye(gbidItem))
    {
        ScopedItemDefinitionLockR ptDyeDef(gbidItem);
        for (int32 i = 0; i < (int32)ptDyeDef->serAttributes_Console.ElementCount(sizeof(AttributeSpecifier)); i++)
        {
            if (ptDyeDef->ptAttributes[i].eAttribute == ATTRIBUTE_DYE_TYPE)
            {
                SavedDyeType eSavedDyeType = (SavedDyeType)ItemGetAttributeSpecifierValue(gbidItem, ptDyeDef->ptAttributes[i], NULL, NULL, 0, 0);
                eResult = DyeTypeTranslateFromSavedDyeType(eSavedDyeType);
                break;
            }
        }
    }
    return eResult;
}




bool ItemHasDye(
    const ActorCommonData & tACDItem,
    GBID gbidDye)
{
    DyeType eDye = tACDItem.GetDyeType();
    GBID gbidAppliedDye = ItemGetGBIDForDyeType(eDye);

    return gbidAppliedDye == gbidDye;
}




bool IsPromoDye(
    GBID gbidDye)
{
    DyeType eDyeType = GetDyeTypeForItemGBID(gbidDye);
    return eDyeType == DYE_TYPE_PROMO_01 || eDyeType == DYE_TYPE_PROMO_02;
}

}  // namespace UIDyeItem

