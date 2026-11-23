#include "X/Common/CosmeticItems.h"
#include "X/Common/ActorCommonData.h"
#include "X/Common/Attributes/FastAttributeStructs.h"
#include "X/Common/Definitions/GameBalanceDefinition.h"
#include "X/Common/Definitions/GlobalDefinition.h"
#include "X/Common/GameBalance.h"
#include "X/Common/GameCommonData.h"
#include "X/Common/GlobalSNO.h"
#include "X/Common/Items.h"
#include "X/Common/Player.h"
#include "X/Common/VisualInventory.h"
#include "X/Common/Protobuf/Account.pb.h"
#include "X/Common/Protobuf/CosmeticItems.pb.h"
#include "X/Common/Protobuf/Items.pb.h"

typedef google::protobuf::RepeatedPtrField<D3::CosmeticItems::CosmeticItem>::const_iterator CosmeticItemsIter;



void CosmeticItemSavedData::Export(D3::Account::SavedDefinition * ptAccountData) const
{
    // The account-wide cosmetic list lives in the account save.
    tCosmeticItemData.Export(*ptAccountData->mutable_account_wide_cosmetic_item_data());
}



void CosmeticItemSavedData::Import(const D3::Account::SavedDefinition * ptAccountData)
{
    if (ptAccountData != NULL &&
        ptAccountData->has_account_wide_cosmetic_item_data())
    {
        tCosmeticItemData.Import(ptAccountData->account_wide_cosmetic_item_data());
    }
    else
    {
        tCosmeticItemData.SetToDefaults();
    }

}



void CosmeticItemData::Export(D3::CosmeticItems::CosmeticItemSavedData & tData) const
{

    tData.Clear();

    // Only learned items are saved; purchased items and rented pets come from the store.

    PackAssetsIntoBitPackedGbidArray(
        *tData.mutable_cosmetic_items(),
        ASSET_LIST_COSMETIC_ITEM_GBIDS,
        (int32 *)gbidCosmeticItems,
        nNumCosmeticItems,
        MAX_COSMETIC_ITEMS);
}



void CosmeticItemData::Import(const D3::CosmeticItems::CosmeticItemSavedData & tData)
{

    nNumCosmeticItems = UnpackAssetsFromBitPackedGbidArray(
        tData.cosmetic_items(),
        ASSET_LIST_COSMETIC_ITEM_GBIDS,
        gbidCosmeticItems,
        MAX_COSMETIC_ITEMS);
    nNumPurchasedCosmeticItems = 0;
    nNumLimitedDurationPets = 0;
}




void CosmeticItemData::Import(const D3::CosmeticItems::CosmeticItems & tData)
{
    SetToDefaults();



    for (CosmeticItemsIter iter = tData.cosmetic_items().begin(), end = tData.cosmetic_items().end(); iter != end; ++iter)
    {
        GBID gbidItem = iter->gbid();
        if (iter->has_expiration_time())
        {
            AddLimitedDurationPet(gbidItem, iter->expiration_time());
        }
        else
        {
            AddCosmeticItem(gbidItem);
        }
    }
}





void CosmeticItemData::Export(D3::CosmeticItems::CosmeticItems & tData) const
{

    for (int32 i = 0; i < nNumCosmeticItems; ++i)
    {
        D3::CosmeticItems::CosmeticItem & tNewItem = *tData.mutable_cosmetic_items()->Add();
        tNewItem.set_gbid(gbidCosmeticItems[i]);
    }

    for (int32 i = 0; i < nNumPurchasedCosmeticItems; ++i)
    {
        D3::CosmeticItems::CosmeticItem & tNewItem = *tData.mutable_cosmetic_items()->Add();
        tNewItem.set_gbid(gbidPurchasedCosmeticItems[i]);
    }

    for (int32 i = 0; i < nNumLimitedDurationPets; ++i)
    {
        D3::CosmeticItems::CosmeticItem & tNewItem = *tData.mutable_cosmetic_items()->Add();
        tNewItem.set_gbid(tLimitedDurationPets[i].gbidPet);
        tNewItem.set_expiration_time(tLimitedDurationPets[i].nExpirationTime);
    }

}



void CosmeticItemData::SetToDefaults()
{

    nNumCosmeticItems = 0;
    nNumLimitedDurationPets = 0;
    nNumPurchasedCosmeticItems = 0;

    for (int32 i = 0; i < MAX_COSMETIC_ITEMS; ++i)
    {
        gbidCosmeticItems[i] = GBID_NONE;
        gbidPurchasedCosmeticItems[i] = GBID_NONE;
    }

    for (int32 i = 0; i < MAX_COSMETIC_PETS; ++i)
    {
        tLimitedDurationPets[i].gbidPet = GBID_NONE;
        tLimitedDurationPets[i].nExpirationTime = 0;
    }

}



Blizzard::Time::Timestamp CosmeticItemData::GetPetExpiration(GBID gbidPet)
{

    for (int32 i = 0; i < MAX_COSMETIC_PETS; ++i)
    {
        if (tLimitedDurationPets[i].gbidPet == gbidPet)
        {
            return tLimitedDurationPets[i].nExpirationTime;
        }
    }

    return 0;
}



BOOL CosmeticItemData::UpdateLimitedDurationPets(const ActorCommonData & tACDPlayer)
{
    // Rented pets are not expired from here on this platform.










































    return FALSE;
}




bool CosmeticItemData::HasItem(GBID gbidItem, uint32 dwFlags, Blizzard::Time::Timestamp nNowBlizzardTimestamp) const
{



    if (dwFlags & (1 << HI_LEARNED_ITEMS))
    {
        for (int i = 0; i < nNumCosmeticItems; ++i)
        {
            if (gbidCosmeticItems[i] == gbidItem)
                return true;
        }
    }

    if (dwFlags & (1 << HI_PURCHASED_ITEMS))
    {
        for (int i = 0; i < nNumPurchasedCosmeticItems; ++i)
        {
            if (gbidPurchasedCosmeticItems[i] == gbidItem)
                return true;
        }
    }

    if (dwFlags & (1 << HI_RENTED_ITEMS))
    {
        for (int i = 0; i < nNumLimitedDurationPets; ++i)
        {
            if (tLimitedDurationPets[i].gbidPet == gbidItem &&
                tLimitedDurationPets[i].nExpirationTime > nNowBlizzardTimestamp)
                return true;
        }
    }

    return false;
}



void CosmeticItemData::AddCosmeticItem(GBID gbidItem)
{


    gbidCosmeticItems[nNumCosmeticItems++] = gbidItem;
}



void CosmeticItemData::AddPurchasedCosmeticItem(GBID gbidItem)
{


    gbidPurchasedCosmeticItems[nNumPurchasedCosmeticItems++] = gbidItem;
}



void CosmeticItemData::AddLimitedDurationPet(GBID gbidPet, Blizzard::Time::Timestamp nExpirationTime)
{



    LimitedDurationPet & tPet = tLimitedDurationPets[nNumLimitedDurationPets++];
    tPet.gbidPet = gbidPet;
    tPet.nExpirationTime = nExpirationTime;
}



CosmeticItemType CosmeticItems_GetCosmeticItemTypeFromGBID(GBID gbidItem)
{
    if (!GBIDExists(gbidItem, GB_ITEMS))
    {
        return COSMETIC_ITEM_INVALID;
    }

    ScopedItemDefinitionLockR pdefItemEntry(gbidItem);
    if (pdefItemEntry == NULL)
    {
        return COSMETIC_ITEM_INVALID;
    }

    if (pdefItemEntry->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PENNANT))
    {
        return COSMETIC_ITEM_PENNANT;
    }

    if (pdefItemEntry->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_WINGS))
    {
        return COSMETIC_ITEM_WINGS;
    }

    if (pdefItemEntry->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PORTRAIT_FRAME))
    {
        return COSMETIC_ITEM_FRAME;
    }

    if (pdefItemEntry->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PET))
    {
        return COSMETIC_ITEM_PET;
    }

    return COSMETIC_ITEM_INVALID;
}





























BOOL CosmeticItems_IsParagonFrame(ActorCommonData & tACDPlayer, GBID gbidItem, CosmeticItemType eType)
{



    if (eType == COSMETIC_ITEM_FRAME)
    {
        int32 nParagonPortraitLevel = PlayerGetParagonLevelFromPortraitGBID(gbidItem);

        if (nParagonPortraitLevel != -1 && nParagonPortraitLevel <= tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL)))
        {
            return TRUE;
        }
    }

    return FALSE;
}



BOOL CosmeticItems_ValidateItem(ActorCommonData & tACDPlayer, GBID gbidItem, CosmeticItemType eType)
{



    if (gbidItem == GBID_NONE)
    {
        return FALSE;
    }

    ScopedItemDefinitionLockR pdefItemEntry(gbidItem);

    if (pdefItemEntry == NULL)
    {
        return FALSE;
    }

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);

    if (ptPlayer == NULL)
    {
        return FALSE;
    }

    if (CosmeticItems_GetCosmeticItemTypeFromGBID(gbidItem) != eType)
    {
        return FALSE;
    }

    // Everything is unlocked in a weekly challenge.
    if (GameIsWeeklyChallenge())
    {
        return TRUE;
    }

    if (eType == COSMETIC_ITEM_FRAME)
    {
        int32 nParagonPortraitLevel = PlayerGetParagonLevelFromPortraitGBID(gbidItem);

        if (nParagonPortraitLevel != -1 && nParagonPortraitLevel <= tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL)))
        {
            return TRUE;
        }
    }

    // Cosmetic ownership is not tracked per player on this platform, so any
    // cosmetic of the right type is accepted.


    Blizzard::Time::Timestamp dwNow = Blizzard::Time::GetTimestamp();
    (void)dwNow;








    return TRUE;

}










































ACDID CosmeticItems_GenerateItemFromGBID(ACDNetworkName ann, GBID gbidCosmetic)
{


    ScopedItemDefinitionLockR pdefRecipe(gbidCosmetic);


    GBHandle hgbItem(GB_ITEMS, gbidCosmetic);

    D3::Items::Generator tItem;
    ItemGeneratorToDefaults(tItem);
    hgbItem.Export(*tItem.mutable_gb_handle());

    tItem.set_seed(0);
    uint32 dwFlags = tItem.flags() | (1 << IGF_IDENTIFIED_BIT);
    tItem.set_flags(dwFlags);

    return CItemGenerate(ann, tItem, NULL);
}


BOOL CosmeticItems_KnownByPlayer(const Player & tPlayer, GBID gbidCosmetic)
{

    const CosmeticItemData & tCosmeticItemData = tPlayer.tCosmeticItemSavedData.tCosmeticItemData;

    for (int32 i = 0; i < tCosmeticItemData.nNumCosmeticItems; ++i)
    {
        if (tCosmeticItemData.gbidCosmeticItems[i] == gbidCosmetic)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CosmeticItems_IsEquipped(const Player & tPlayer, GBID gbidItem)
{

    VisualCosmeticItemSlot eSlot;
    CosmeticItemType eType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbidItem);



    switch (eType)
    {
    case COSMETIC_ITEM_WINGS:
        eSlot = VCIS_WINGS;
        break;

    case COSMETIC_ITEM_PENNANT:
        eSlot = VCIS_PENNANT;
        break;

    case COSMETIC_ITEM_PET:
        eSlot = VCIS_PET;
        break;

    case COSMETIC_ITEM_FRAME:
        eSlot = VCIS_PORTRAIT;
        break;

    default:
        return FALSE;
    }

    return VisualInventoryGetCosmeticItemInSlot(tPlayer.idPlayerACD, eSlot) == gbidItem;



}
