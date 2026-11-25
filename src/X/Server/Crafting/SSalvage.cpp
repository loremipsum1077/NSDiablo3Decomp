// Original file: X/Server/Crafting/SSalvage.cpp
#include "SItemCrafting.h"
#include "../Loot/Loot.h"
#include "../PlayerList.h"
#include "../Powers/PowerAPI.h"
#include "../SGameStats.h"
#include "../SGameStatsSwig.h"
#include "../Actors/Actors.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Crafting/ItemCrafting.h"
#include "../../Common/Crafting/Salvage.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Definitions/TreasureClassDefinition.h"
#include "../../Common/Items.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/MemoryPools.h"


// Declared here until their headers carry them.
BOOL LootGetLootSpecifiers(const LooteeInfo & tLootee, const ACDID idACDLooter, LootSpecifierArray & arSpecifiers, const TreasureClassDefinition * pdefTreasureClass, const BOOL bIgnoreQuestItems, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot);  // X/Server/Loot/Loot.cpp:3697
ACDID LootCreateItem(LootSpecifier & tSpecifier, const WorldPlace & tPlace, const ActorCommonData * ptACDSource);  // X/Server/Loot/Loot.cpp:4339
void PlayerClearPotionBtnForMissingItem(Player * ptPlayer, const ACDID idACDItem);  // X/Common/Player.cpp:1735
ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD);  // X/Common/Items.cpp:2208
void MessageSendToClient(const PlayerList & listDestPlayers, int32 nOpcode, void * pMessage, uint32 dwSize);

namespace SSalvage { BOOL TrySalvage(Player & tPlayer, ActorCommonData & tACDItem); void TrySalvageAll(Player & tPlayer, SalvageAllType eSalvageType); }

struct SalvageAllCallbackData {  // size 0x48
    ActorCommonData * ptACDPlayer;  // 0x00
    SalvageAllType eSalvageType;  // 0x08
    SalvageResultsMessage tResultsMessage;  // 0x0c
    BOOL bReturnedGems;  // 0x40
};






static void sSalvageTreasureClassInternal(SalvageResultsMessage & tResultsMessage, const LooteeInfo & tLootee, ActorCommonData & tACDOwner, const TreasureClassDefinition * ptSalvageTreasureClass)





{
    LootSpecifierArray arLootSpecifiers;
    if (LootGetLootSpecifiers(tLootee, tACDOwner.id, arLootSpecifiers, ptSalvageTreasureClass, FALSE, 1, NULL))
    {

        int32 nNumItems = arLootSpecifiers.GetArraySize();
        for (int32 nItem = 0; nItem < nNumItems; nItem++)
        {
            arLootSpecifiers[nItem].fAutoIdentify = TRUE;


            ACDID idACDNewItem = LootCreateItem(arLootSpecifiers[nItem], cgplaceNull, NULL);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDNewItem);
            if (ptACDItem)
            {




                BOOL bFound = FALSE;
                for (int32 j = 0; j < tResultsMessage.nMaterialsResults; j++)
                {
                    if (tResultsMessage.gbidMaterials[j] == ptACDItem->hGB.gbid)
                    {
                        tResultsMessage.nMaterialsCounts[j]++;
                        bFound = TRUE;
                        break;
                    }
                }

                if (!bFound && tResultsMessage.nMaterialsResults < 4)
                {
                    tResultsMessage.gbidMaterials[tResultsMessage.nMaterialsResults] = ptACDItem->hGB.gbid;
                    tResultsMessage.nMaterialsCounts[tResultsMessage.nMaterialsResults] = 1;
                    tResultsMessage.nMaterialsResults++;
                }


                ACDID idACDStack = ACDID_INVALID;
                SACDInventoryPickupOrSpillOntoGround(tACDOwner, *ptACDItem, FALSE, &idACDStack, TRUE);






                Player * ptPlayer = PlayerGetByACD(tACDOwner.id);
                if (ptPlayer)
                {
                    ActorCommonData * ptACDStack = ACDTryToGet(idACDStack);
                    if (ptACDStack)
                    {
                        ptACDItem = ptACDStack;
                    }

                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetPlayer(ptPlayer);
                    tEvaluationData.SetACD(ptACDItem);

                    tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_RECOVERED_CRAFTING_MATERIAL,

                        ptACDItem->hGB.gbid, 1, (uint32)sub_71007b5e70());
                }
            }
        }
    }
}



static void sSalvageItemInternal(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, SalvageResultsMessage & tResultsMessage, BOOL & bReturnedGems)




{

    SNO snoTreasureClass = SalvageCommon::GetTreasureClass(tACDItem);
    ScopedTreasureClassDefinitionLockR pdefTreasureClass(snoTreasureClass);
    if (pdefTreasureClass == NULL)
    {
        return;
    }


    if (ItemHasSockets(tACDItem) &&
        ItemGetNumFilledSockets(tACDItem) > 0)
    {
        ItemCrafting_BackEnd::UnsocketCallbackData tCallbackData;
        tCallbackData.ptACDPlayer = &tACDOwner;
        tCallbackData.bTriggerAchievementEvent = FALSE;

        ACDInventoryIterateSlot(tACDItem, SLOT_ITEM_SOCKETS, NULL, ItemCrafting_BackEnd::UnSocket, &tCallbackData, FALSE, FALSE);
        bReturnedGems = TRUE;
    }






    {
        Player * ptPlayer = PlayerGetByACD(tACDOwner.id);
        if (ptPlayer)
        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.SetACD(&tACDItem);

            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_SALVAGE_ITEM,

                ItemGetQualityLevel(tACDItem), 1, (uint32)sub_71007b5e70());

        }
    }


    uint64 uStackSize = Math::Max<uint64>(1, tACDItem.GetStackCount());
    uint64 nNumRolls = uStackSize * (tACDItem.AttributesGetInt(ATTRIBUTE_HIGHLY_SALVAGEABLE) + 1);

    ScopedGlobalDefinitionLockR pdefGlobal;
    switch (ItemGetAncientRank(tACDItem))
    {
    case 1:
        nNumRolls *= pdefGlobal->ptServerData->nAncientSalvageMultiplier;
        break;
    case 2:
        nNumRolls *= pdefGlobal->ptServerData->nPrimalAncientSalvageMultiplier;
        break;
    }




    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDItem, ACDID_INVALID);


    if (tACDItem.IsInInventory())
    {
        InventoryLocation & tInvLocation = tACDItem.GetInventoryLocation();
        InventorySlotType eSlot = tInvLocation.eSlot;
        if ((eSlot >= SLOT_PLAYER_HEAD && eSlot < SLOT_PLAYER_SHARED_STASH) ||
            (eSlot >= SLOT_HIRELING_START && eSlot <= SLOT_HIRELING_END))
        {
            PowerAPI_BuffApplyOnEquipmentChangeEffects(tInvLocation.idACDContainer, eSlot, 1);
        }

        Player * ptPlayer = PlayerGetByACD(tACDOwner.id);
        if (ptPlayer)
        {
            PlayerClearPotionBtnForMissingItem(ptPlayer, tACDItem.id);
            ACDInventoryEquipmentOrSkillChanged(tACDOwner);
        }
    }


    SACDInventoryDeleteItem(tACDItem, uStackSize, TRUE, FALSE);


    for (int32 nRoll = 0; nRoll < nNumRolls; nRoll++)
    {
        sSalvageTreasureClassInternal(tResultsMessage, tLootee, tACDOwner, pdefTreasureClass);
    }
}

static void sSalvageAllCallback(ACDID idACDSquare, InventorySlotType, void * pData)



{
    SalvageAllCallbackData * ptCallbackData = (SalvageAllCallbackData *)pData;


    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem == NULL)
    {
        return;
    }

    SalvageAllType eItemSalvageType = SalvageCommon::ItemGetSalvageAllType(*ptACDItem, *ptCallbackData->ptACDPlayer);
    if (eItemSalvageType != SAT_INVALID &&
        eItemSalvageType == ptCallbackData->eSalvageType)
    {



        sSalvageItemInternal(*ptACDItem, *ptCallbackData->ptACDPlayer, ptCallbackData->tResultsMessage, ptCallbackData->bReturnedGems);
    }
}





BOOL SSalvage::TrySalvage(Player & tPlayer, ActorCommonData & tACDItem)


{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }

    if (SalvageCommon::ItemCanSalvage(tACDItem, *ptACDPlayer) != GAMEERROR_NONE)
    {
        return FALSE;
    }


    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_ITEM_SALVAGED, tACDItem.hGB.gbid, ItemGetStatsDataKey(tACDItem), 1.0f);


    BOOL bReturnedGems = FALSE;
    SalvageResultsMessage tResultsMessage;
    tResultsMessage.gbidOriginalItem = tACDItem.hGB.gbid;
    tResultsMessage.eIQLOriginalItem = ItemGetDisplayedQualityLevel(tACDItem);
    tResultsMessage.nMaterialsResults = 0;


    sSalvageItemInternal(tACDItem, *ptACDPlayer, tResultsMessage, bReturnedGems);


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, ptACDPlayer->id);
    MessageSendToClient(listDestPlayers, SMSG_SALVAGE_RESULT, &tResultsMessage, sizeof(tResultsMessage));


    if (bReturnedGems)
    {
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_RETURNED_GEMS_NOTIFICATION, &tMessage, sizeof(tMessage));
    }

    return TRUE;
}


void SSalvage::TrySalvageAll(Player & tPlayer, SalvageAllType eSalvageType)


{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (eSalvageType < 0 || eSalvageType >= SAT_NUM_TYPES || ptACDPlayer == NULL)
    {
        return;
    }








    SalvageAllCallbackData tCallbackData;
    tCallbackData.ptACDPlayer = ptACDPlayer;
    tCallbackData.eSalvageType = eSalvageType;
    tCallbackData.tResultsMessage.gbidOriginalItem = -1; tCallbackData.tResultsMessage.eIQLOriginalItem = -1;
    tCallbackData.tResultsMessage.nMaterialsResults = 0;
    tCallbackData.bReturnedGems = FALSE;

    ACDInventoryIterateItems(*ptACDPlayer, 0, 0, sSalvageAllCallback, &tCallbackData);


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, ptACDPlayer->id);

    if (tCallbackData.tResultsMessage.nMaterialsResults > 0)
    {
        MessageSendToClient(listDestPlayers, SMSG_SALVAGE_RESULT, &tCallbackData.tResultsMessage, sizeof(tCallbackData.tResultsMessage));
    }


    if (tCallbackData.bReturnedGems)
    {
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_RETURNED_GEMS_NOTIFICATION, &tMessage, sizeof(tMessage));
    }
}
