// Original file: X/Server/OnlineService/DataHealUtils.cpp

#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Crafting/ItemCrafting.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/OnlineService/OnlineService.h"







D3::Account::AccountPartition * FindAccountPartition(
    D3::Account::SavedDefinition & tSavedDefinition,
    int partitionId)
{
    for (int i = 0; i < tSavedDefinition.partitions_size(); ++i)
    {
        D3::Account::AccountPartition * pPartition = tSavedDefinition.mutable_partitions(i);
        if (pPartition->partition_id() == partitionId)
        {
            return pPartition;
        }
    }

    D3::Account::AccountPartition * pNewPartition = tSavedDefinition.mutable_partitions()->Add();
    pNewPartition->set_partition_id(partitionId);
    return pNewPartition;
}



void HealTransmogs(
    const D3::Account::SavedDefinition & srcAccountDefinition,
    D3::Account::SavedDefinition & healedAccountDefinition,
    const OnlineService::GameAccountHandle & gameAccount,
    const char * logHeader)
{
    if (!srcAccountDefinition.has_account_wide_transmog_data())
    {
        healedAccountDefinition = srcAccountDefinition;
        return;
    }


    for (const D3::Account::AccountPartition & tOrgPartition : srcAccountDefinition.partitions())
    {
        D3::Account::AccountPartition * pHealedPartition = FindAccountPartition(healedAccountDefinition, tOrgPartition.partition_id());
        D3::ItemCrafting::CrafterSavedData & tCrafterSavedData = *pHealedPartition->mutable_crafter_data();

        CrafterSavedData tempCrafterSavedData;
        tempCrafterSavedData.ImportNoLogs(
            tOrgPartition.has_crafter_data() ? tOrgPartition.crafter_data() : tCrafterSavedData, &srcAccountDefinition);


        for (int index = 0; index < tempCrafterSavedData.tTransmogData.nUnlockedAccountTransmogs; ++index)
        {
            ScopedItemDefinitionLockR pdefTransmog(tempCrafterSavedData.tTransmogData.gbidUnlockedAccountTransmogs[index]);
            if (pdefTransmog == NULL || (pdefTransmog->dwFlags & (1 << ITEM_ACCOUNT_WIDE_TRANSMOG)))
            {
                continue;
            }


            if (tempCrafterSavedData.tTransmogData.nUnlockedTransmogs == MAX_UNLOCKED_PARTITION_TRANSMOGS_PER_ACCOUNT)
            {
                break;
            }



            tempCrafterSavedData.tTransmogData.gbidUnlockedTransmogs[tempCrafterSavedData.tTransmogData.nUnlockedTransmogs++] =
                tempCrafterSavedData.tTransmogData.gbidUnlockedAccountTransmogs[index];
        }

        tempCrafterSavedData.Export(tCrafterSavedData, &healedAccountDefinition);
    }


    CrafterTransmogData oldCrafterTransmogData;
    CrafterTransmogData newCrafterTransmogData;

    D3::GameBalance::BitPackedGbidArray emptyBitPackedGbidArray;

    oldCrafterTransmogData.ImportNoLogs(emptyBitPackedGbidArray, &srcAccountDefinition.account_wide_transmog_data());
    newCrafterTransmogData.SetToDefaults();

    for (int i = 0; i < oldCrafterTransmogData.nUnlockedAccountTransmogs; ++i)
    {

        ScopedItemDefinitionLockR pdefTransmog(oldCrafterTransmogData.gbidUnlockedAccountTransmogs[i]);
        if (pdefTransmog == NULL || (pdefTransmog->dwFlags & (1 << ITEM_ACCOUNT_WIDE_TRANSMOG)))





            newCrafterTransmogData.gbidUnlockedAccountTransmogs[newCrafterTransmogData.nUnlockedAccountTransmogs++] =
                oldCrafterTransmogData.gbidUnlockedAccountTransmogs[i];
    }

    newCrafterTransmogData.Export(emptyBitPackedGbidArray, healedAccountDefinition.mutable_account_wide_transmog_data());
}
