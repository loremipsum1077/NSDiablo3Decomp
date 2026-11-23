// X/Common/Crafting/Salvage.cpp

#include "Salvage.h"
#include "../ActorCommonData.h"
#include "../ActorInventory.h"
#include "../Attributes/FastAttribConstantsSwig.h"
#include "../Attributes/FastAttributeStructs.h"
#include "../Definitions/Definitions.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../GlobalSNO.h"
#include "../HotbarButton.h"
#include "../Items.h"
#include "../ItemsSwig.h"
#include "../Player.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/Math/MathGeneral.h"

// Original line 22.
SNO SalvageCommon::GetTreasureClass(const ActorCommonData & tACDItem)
{
    const GameBalanceDefinition * pdefGBItems;
    SNO snoTreasureClass = SNO_NONE;
    ItemDefinition * pdefItem = (ItemDefinition *)GBLockR(tACDItem.hGB, pdefGBItems, TRUE);

    ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_ITEM_SALVAGE_LEVELS));
    if (pdefGB)
    {
        int32 nMaxSalvageLevel = pdefGB->tItemSalvageLevelTable.serItemSalvageLevels.dwSize / sizeof(ItemSalvageLevel);
        int32 nItemSalvageLevel = Math::Max(ItemGetLevelRequirement(tACDItem.hGB.gbid, &tACDItem, TRUE), 1);

        if (nMaxSalvageLevel > 0 && nItemSalvageLevel > 0)
        {
            ItemQualityLevel eIQL = ItemGetQualityLevel(tACDItem);
            int32 nTableIndex = Math::Min(nItemSalvageLevel - 1, nMaxSalvageLevel - 1);
            ItemSalvageLevel & tLevel = pdefGB->tItemSalvageLevelTable.ptItemSalvageLevels[nTableIndex];

            if (eIQL <= IQL_SUPERIOR)
            {
                snoTreasureClass = pdefItem->snoSalvageTreasureClassNormal;
                if (snoTreasureClass == SNO_NONE)
                {
                    snoTreasureClass = tLevel.snoSalvageTreasureClassNormal;
                }
            }
            else if (eIQL < IQL_RARE_AFFIX_START || eIQL == IQL_SPECIAL)
            {
                snoTreasureClass = pdefItem->snoSalvageTreasureClassMagic;
                if (snoTreasureClass == SNO_NONE)
                {
                    snoTreasureClass = tLevel.snoSalvageTreasureClassMagic;
                }
            }
            else if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
            {
                snoTreasureClass = pdefItem->snoSalvageTreasureClassRare;
                if (snoTreasureClass == SNO_NONE)
                {
                    snoTreasureClass = tLevel.snoSalvageTreasureClassRare;
                }
            }
            else if (eIQL == IQL_LEGENDARY)
            {
                snoTreasureClass = pdefItem->snoSalvageTreasureClassNormal;
                if (snoTreasureClass == SNO_NONE)
                {
                    snoTreasureClass = tLevel.snoSalvageTreasureClassLegendary;
                }
            }
        }
    }

    GBUnlockR(pdefGBItems);

    return snoTreasureClass;
}

// Original line 83.
GameError SalvageCommon::ItemCanSalvage(const ActorCommonData & tACDItem, const ActorCommonData & tACDOwner)
{
    if (!tACDItem.IsItem())
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE;
    }

    {
        ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
        if (!pdefItem || !(pdefItem->dwFlags & (1 << ITEM_CAN_SALVAGE)))
        {
            return GAMEERROR_IARESULT_CANNOT_SALVAGE;
        }
    }

    if (GetTreasureClass(tACDItem) == SNO_NONE)
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE;
    }

    if (tACDOwner.IsItem() && ItemHasSockets(tACDOwner) && ItemIsSocketable(tACDItem))
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE_GEMMED_ITEMS;
    }

    const ActorLocation & tLocation = tACDItem.GetLocation();
    InventorySlotType eSlotType = tLocation.tInvLocation.eSlot;
    if (!ACDInventoryIsSlotPlayer(eSlotType))
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE;
    }

    if (eSlotType == SLOT_PLAYER_BUYBACK || eSlotType == SLOT_PLAYER_BASE_HEALTHPOTION)
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE;
    }

    if (ACDInventoryGetItemInSlot(tLocation.tInvLocation) != tACDItem.id)
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE;
    }

    if (tLocation.tInvLocation.idACDContainer != tACDOwner.id)
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE;
    }

    if (!ACDIsPlayer(tACDOwner.id) && !tACDOwner.IsPet())
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE;
    }

    if (tACDItem.AttributesGetInt(ATTRIBUTE_IS_VENDOR_BOUGHT))
    {
        return GAMEERROR_IARESULT_CANNOT_SALVAGE_VENDOR_ITEMS;
    }

    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_HEALTHPOTION) && ACDIsPlayer(tACDOwner.id))
    {
        Player * ptPlayer = PlayerGetByACD(tACDOwner.id);
        if (ptPlayer)
        {
            const HotbarButtonData & tPotionData = PlayerGetPotionBtnAssignment(ptPlayer);
            if (ACDFromANN(tPotionData.m_annItem) == tACDItem.id)
            {
                return GAMEERROR_IARESULT_CANNOT_SALVAGE;
            }
        }
    }

    return GAMEERROR_NONE;
}

// Original line 161.
SalvageAllType SalvageCommon::ItemGetSalvageAllType(const ActorCommonData & tACDItem, const ActorCommonData & tACDOwner)
{
    if (ItemCanSalvage(tACDItem, tACDOwner) != GAMEERROR_NONE)
    {
        return SAT_INVALID;
    }

    if (ItemGetBoundToACD(tACDItem, TRUE) != ACDID_INVALID)
    {
        return SAT_INVALID;
    }

    GBID gbidTransmog = tACDItem.AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
    if (gbidTransmog != GBID_NONE)
    {
        return SAT_INVALID;
    }

    if (ItemGetNumFilledSockets(tACDItem) > 0)
    {
        return SAT_INVALID;
    }

    ItemQualityLevel eIQL = ItemGetQualityLevel(tACDItem);
    if (eIQL == IQL_INVALID)
    {
        return SAT_INVALID;
    }

    if (eIQL < IQL_MAGIC_AFFIX_START)
    {
        return SAT_NORMAL;
    }

    if (eIQL < IQL_RARE_AFFIX_START)
    {
        return SAT_MAGIC;
    }

    if (eIQL < IQL_LEGENDARY)
    {
        return SAT_RARE;
    }

    return SAT_INVALID;
}
