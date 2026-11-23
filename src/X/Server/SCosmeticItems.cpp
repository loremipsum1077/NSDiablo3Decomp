// X/Server/SCosmeticItems.cpp -- the server learns cosmetic items (wings, pennants, portrait
// frames, pets) for a player, tells the client, and equips them. Every function sits on its
// original line.

#include "SCosmeticItems.h"

#include "../Common/ActorCommonData.h"
#include "../Common/CosmeticItems.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Game.h"
#include "../Common/GameBalance.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/Protobuf/CosmeticItems.pb.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "Actors/ActorEffect.h"
#include "OnlineService/OnlineServiceServer.h"
#include "PlayerList.h"
#include "SPlayer.h"

Player * PlayerGetByACD(ACDID idACD);  // X/Common/Player.cpp








BOOL SCosmeticItems_LearnCosmeticItem(
    const ACDID idACDPlayer,
    const GBID gbidItem,
    CosmeticItemType eType,
    bool bPurchased)
{
    ScopedItemDefinitionLockR pdefItemEntry(gbidItem);

    if (pdefItemEntry == NULL)
    {
        return FALSE;
    }

    switch (eType)
    {
    case COSMETIC_ITEM_WINGS:
        if (pdefItemEntry->gbidItemType != GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_WINGS) ||
            pdefItemEntry->snoMainEffect == SNO_NONE)
        {
            return FALSE;
        }
        break;
    case COSMETIC_ITEM_PENNANT:
        if (pdefItemEntry->gbidItemType != GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PENNANT) ||
            pdefItemEntry->snoMainEffect == SNO_NONE)
        {
            return FALSE;
        }
        break;
    case COSMETIC_ITEM_FRAME:
        if (pdefItemEntry->gbidItemType != GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PORTRAIT_FRAME) ||
            pdefItemEntry->hPortraitFrame == 0)
        {
            return FALSE;
        }
        break;

    default:
        return FALSE;
    }
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
    {
        CosmeticItemData & tCosmeticItemData = ptPlayer->tCosmeticItemSavedData.tCosmeticItemData;
        if (bPurchased)
        {
            for (int32 i = 0; i < tCosmeticItemData.nNumPurchasedCosmeticItems; i++)
            {
                if (tCosmeticItemData.gbidPurchasedCosmeticItems[i] == gbidItem)
                {
                    return FALSE;
                }
            }
        }

        for (int32 i = 0; i < tCosmeticItemData.nNumCosmeticItems; i++)
        {
            if (tCosmeticItemData.gbidCosmeticItems[i] == gbidItem)
            {
                return FALSE;
            }
        }

        if (bPurchased)
        {
            tCosmeticItemData.AddPurchasedCosmeticItem(gbidItem);
        }
        else
        {
            tCosmeticItemData.AddCosmeticItem(gbidItem);
        }

        D3::CosmeticItems::CosmeticItems tData;
        tCosmeticItemData.Export(tData);

        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listPlayers, *ptPlayer);
        MessageSendToClient(listPlayers, SMSG_UPDATED_COSMETIC_ITEM_DATA, tData);

        return TRUE;
    }

    return FALSE;
}

























































BOOL SCosmeticItems_LearnPet(
    const ACDID idACDPlayer,
    const GBID gbidItem,
    bool bPurchased)
{
    ScopedItemDefinitionLockR pdefItemEntry(gbidItem);
    if (pdefItemEntry == NULL ||
        pdefItemEntry->gbidItemType != GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PET))
    {
        return FALSE;
    }

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
    {
        CosmeticItemData & tCosmeticItemData = ptPlayer->tCosmeticItemSavedData.tCosmeticItemData;
        if (bPurchased)
        {
            for (int32 i = 0; i < tCosmeticItemData.nNumPurchasedCosmeticItems; i++)
            {
                if (tCosmeticItemData.gbidPurchasedCosmeticItems[i] == gbidItem)
                {
                    return FALSE;
                }
            }
        }

        for (int32 i = 0; i < tCosmeticItemData.nNumCosmeticItems; i++)
        {
            if (tCosmeticItemData.gbidCosmeticItems[i] == gbidItem)
            {
                return FALSE;
            }
        }

        BOOL bLimitedDuration = FALSE;

        for (int32 i = 0; i < (int32)(pdefItemEntry->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
        {
            const AttributeSpecifier & tSpecifier = pdefItemEntry->ptAttributes[i];

            if (tSpecifier.eAttribute == ATTRIBUTE_COSMETIC_PET_EXPIRATION && tSpecifier.nParam > 0)
            {
                for (int32 j = 0; j < tCosmeticItemData.nNumLimitedDurationPets; j++)
                {
                    if (tCosmeticItemData.tLimitedDurationPets[j].gbidPet == gbidItem)
                    {
                        return FALSE;
                    }
                }

                tCosmeticItemData.tLimitedDurationPets[tCosmeticItemData.nNumLimitedDurationPets].gbidPet =
                    gbidItem;

                // The attribute parameter is the pet duration in days.
                uint32 nPetDuration = tSpecifier.nParam * SECONDS_PER_DAY;

                uint32 nNow = Blizzard::Time::ToUnixTime(OnlineServiceServer::GetCurrentTime());
                Blizzard::Time::Timestamp nPetExpiration = Blizzard::Time::FromUnixTime(nNow + nPetDuration);

                tCosmeticItemData.tLimitedDurationPets[tCosmeticItemData.nNumLimitedDurationPets].nExpirationTime = nPetExpiration;
                tCosmeticItemData.nNumLimitedDurationPets++;
                tCosmeticItemData.UpdateLimitedDurationPets(ACDGet(idACDPlayer));
                bLimitedDuration = TRUE;
                break;
            }
        }

        if (!bLimitedDuration)
        {
            if (bPurchased)
            {
                tCosmeticItemData.AddPurchasedCosmeticItem(gbidItem);
            }
            else
            {
                tCosmeticItemData.AddCosmeticItem(gbidItem);
            }
        }

        D3::CosmeticItems::CosmeticItems tData;
        tCosmeticItemData.Export(tData);

        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listPlayers, *ptPlayer);
        MessageSendToClient(listPlayers, SMSG_UPDATED_COSMETIC_ITEM_DATA, tData);

        return TRUE;
    }

    return FALSE;
}




















































void SCosmeticItems_SendPlayerCosmeticItemInfo(
    const Player & tPlayer)
{
    D3::CosmeticItems::CosmeticItems tData;
    tPlayer.tCosmeticItemSavedData.tCosmeticItemData.Export(tData);
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);
    MessageSendToClient(listPlayers, SMSG_UPDATED_COSMETIC_ITEM_DATA, tData);
}



static BOOL sEquipCosmeticItem(
    Player & tPlayer,
    const GBID gbidItem,
    CosmeticItemType eType)
{
    switch (eType)
    {
    case COSMETIC_ITEM_WINGS:
    case COSMETIC_ITEM_PENNANT:
        ACDActivateCosmeticEffect(tPlayer.idPlayerACD, gbidItem, eType);
        break;

    case COSMETIC_ITEM_PET:
        SPlayerActivatePet(tPlayer, gbidItem);
        break;

    case COSMETIC_ITEM_FRAME:
        SPlayerSetCosmeticPortrait(tPlayer, gbidItem);
        break;

    default:
        return FALSE;
    }

    return TRUE;
}


BOOL SCosmeticItems_EquipCosmeticItem(
    Player & tPlayer,
    const GBID gbidItem)
{
    ActorCommonData * ptPlayerACD = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptPlayerACD == NULL)
    {
        return FALSE;
    }
    CosmeticItemType eType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbidItem);
    if (!CosmeticItems_ValidateItem(*ptPlayerACD, gbidItem, eType))
    {
        return FALSE;
    }
    return sEquipCosmeticItem(tPlayer, gbidItem, eType);
}



BOOL SCosmeticItems_UnequipCosmeticItem(
    Player & tPlayer,
    CosmeticItemType eType)
{
    return sEquipCosmeticItem(tPlayer, GBID_NONE, eType);
}
