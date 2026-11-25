// X/Server/Crafting/SItemCrafting.cpp -- the server half of item crafting: repairing, transmogs,
// dyes, the Kanai cube, enchanting, and crafter levels. Every function sits on its original line.

#include "SItemCrafting.h"

#include "../../Common/ActorCommonData.h"
#include "../../Common/Crafting/ItemCrafting.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/Protobuf/GameBalance.pb.h"
#include "../../Common/Protobuf/ItemCrafting.pb.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../../Common/Items.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Definitions/TreasureClassDefinition.h"
#include "../../Common/Definitions/RecipeDefinition.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/Protobuf/Items.pb.h"
#include "../../Common/WorldPlace.h"
#include "../Loot/Loot.h"
#include "../PlayerList.h"
#include "../SGameStats.h"
#include "../Actors/Actors.h"
#include "../Actors/ActorEffect.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Common/Crafting/ItemCraftingSwig.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/Actors/ActorEffectCommon.h"

// Message structs from X/Common/Crafting/ItemCrafting.h and X/Common/Networking/GameMessage.h (not in the adopted subsets there).
struct CraftingResultsMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; GBID gbidItem; ItemQualityLevel eIQL; };  // size 0x14
struct DyeItemMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; DyeType eDyeType; };  // size 0x10
  // size 0xc
struct TransmogItemMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; GBID gbidTransmog; };  // size 0x10
  // size 0x10
CrafterType NPCGetCrafterType(const ActorCommonData & tACD);  // X/Common/NPC.cpp
void ACDInventoryRemoveToLimbo(const ACDID idACDContainer, ActorCommonData & tACDItem, BOOL bRemoveFromKnownList);  // X/Common/ActorInventory.cpp:1515
BOOL ItemRemoveSocketedItemAttributes(ActorCommonData & tACDItemToSocket, const ActorCommonData & tACDItemSocketedInto);  // X/Common/Items.cpp
void SACDInventoryEvaluateItemDyeAchievements(Achievement::EvaluationData * ptEvaluationData);  // ActorInventory.cpp:12580
#include "../../Common/Marker/Marker.h"
#include "../LuaScript/ScriptEvent.h"
#include "../ConversationMgr.h"
SNO ItemGetLegendaryPower(GBID gbidItem);  // X/Common/Items.cpp:10556
BOOL ItemCanExtractLegendaryPower(GBID gbidItem);  // X/Common/Items.cpp:10656
static BOOL sgfRepairSuccess;  // line 47
BOOL ItemPlayerHasExtractedLegendaryPower(const Player & tPlayer, GBID gbidItem);  // X/Common/Items.cpp:10795
const LPCSTR EVENT_PLAYER_USED_KANAI_CUBE = "PlayerUsedKanaiCube";  // line 49 (DWARF: const LPCSTR; value = the string passed to ScriptEventOnUserEvent at 1932)
BOOL SItemPlayerExtractLegendaryPower(Player & tPlayer, GBID gbidItem, GameError & eGameError);  // X/Server/SItems.cpp:1508
BOOL ItemIsEquippableByPlayerOrHireling(GBID gbidItem);  // X/Common/Items.cpp:9460
BOOL ItemIsEquippableByPlayerOrHireling(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:9448
GBID LootPickLegendaryOfItemType(const Player & tPlayer, GBID gbidItemType, int32 nItemLevel);  // X/Server/Loot/Loot.cpp:7262
void LootRollForAncientLegendary(LootSpecifier & tSpecifier, const ACDID idACDLooter);  // X/Server/Loot/Loot.cpp:7140
void SItemBroadcastToClan(const ActorCommonData & tACDItem, const Player & tPlayer);  // X/Server/SItems.cpp:1233
int32 ItemGetNumberOfItemsInSameSet(const GBID gbid);  // X/Common/Items.cpp:9404
GBID ItemGetSet(const GBID gbid);  // X/Common/Items.cpp:9391
namespace ItemCrafting_BackEnd {
ACDID CraftItem(GBID gbidRecipe, Player & tPlayer)
{

    ACDID idACDItem = SItemCrafting_GenerateItemFromRecipe(gbidRecipe, &tPlayer);
    if (idACDItem == ACDID_INVALID) return ACDID_INVALID;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
        return ACDID_INVALID;



    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
        return ACDID_INVALID;



    ptACDItem->SetAssignedHeroID(&tPlayer);

    ACDID idACDStack = ACDID_INVALID;
    BOOL fSuccess = SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDItem, FALSE, &idACDStack, TRUE);
    if (!fSuccess)
        return ACDID_INVALID;



    SItemCrafting_TryUnlockTransmog(ptACDItem->hGB.gbid, tPlayer, TRUE, TRUE);

    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_RECIPE_CRAFTED, gbidRecipe, 0, 1.0f);

    return (idACDStack != ACDID_INVALID) ? idACDStack : idACDItem;
}


inline BOOL SpendGold(ActorCommonData & tACD, int32 nGold)
{
    BOOL bSuccess = tACD.ModifyCurrencyAmount(-nGold, CURRENCY_GOLD, GMR_CRAFTING, FALSE);
    if (bSuccess)
    {
        SGameStatsRecordGoldSpent(tACD, nGold);
    }
    return bSuccess;
}



inline void SendResultToClient(ACDID idACDItem, Player & tPlayer, GameMessageType eMsg)
{
    CraftingResultsMessage tMessage;



    tMessage.annItem = ANNFromACD(idACDItem);
    ActorCommonData & tACDItem = ACDGet(idACDItem);
    tMessage.gbidItem = tACDItem.hGB.gbid;
    tMessage.eIQL = ItemGetQualityLevel(tACDItem);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, eMsg, &tMessage, sizeof(tMessage));
}



void RepairItem(ACDID idACDSquare, InventorySlotType, void * pUserData)
{



    Player & tPlayer = *(Player *)pUserData;

    if (SItemCrafting_TryRepair(idACDSquare, tPlayer))
    {

        sgfRepairSuccess = TRUE;
    }
}
















BOOL UnSocket(const InventoryLocation & tInvLoc, void * pCallBackData)
{

    ItemCrafting_BackEnd::UnsocketCallbackData * tUnsocketCallbackData = (ItemCrafting_BackEnd::UnsocketCallbackData *)pCallBackData;
    ActorCommonData & tACD = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACD, tInvLoc.eSlot, tInvLoc.pt);

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem || !ItemIsSocketable(*ptACDItem))
    {
        return TRUE;
    }


    ItemRemoveSocketedItemAttributes(*ptACDItem, tACD);




    ACDInventoryRemoveToLimbo(tInvLoc.idACDContainer, *ptACDItem, TRUE);

    ACDID idACDStack = ACDID_INVALID;
    SACDInventoryPickupOrSpillOntoGround(*tUnsocketCallbackData->ptACDPlayer, *ptACDItem, FALSE, &idACDStack, TRUE);

    if (tUnsocketCallbackData->bTriggerAchievementEvent)
    {




        const Player * ptPlayer = PlayerGetByACD(tUnsocketCallbackData->ptACDPlayer->id);

        if (ptPlayer)
        {
            ActorCommonData * ptACDStack = ACDTryToGet(idACDStack);
            if (ptACDStack)
                ptACDItem = ptACDStack;



            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.SetACD(ptACDItem);

            ScopedItemDefinitionLockR ptDefGem(ptACDItem->hGB.gbid);

            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_UNSOCKET_ITEM, ptDefGem->eGemType, 1, (uint32)sub_71007b5e70());




        }
    }


    return TRUE;
}
}  // namespace ItemCrafting_BackEnd
GBID LootPickItemFromSet(const Player & tPlayer, GBID gbidSet, GBID gbidItemSkip, int32 nItemLevel);  // X/Server/Loot/Loot.cpp:7428
void SItemCrafting_RemoveGems(ActorCommonData & tACDItem, Player & tPlayer)
{

    if (!ItemHasSockets(tACDItem))
        return;




    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    int32 nCost = ItemCrafting_GetGemRemovalCost(tACDItem);
    if (nCost > 0)
    {
        if (!tACDPlayer.CanAffordCurrencyCost(nCost, CURRENCY_GOLD))
            return;



        if (!ItemCrafting_BackEnd::SpendGold(tACDPlayer, nCost))
            return;



        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_UNSOCKETING, 0, 0, (float)nCost);
    }


    ItemCrafting_BackEnd::UnsocketCallbackData tCallbackData;
    tCallbackData.ptACDPlayer = &tACDPlayer;
    tCallbackData.bTriggerAchievementEvent = TRUE;
    ACDInventoryIterateSlot(tACDItem, SLOT_ITEM_SOCKETS, NULL, ItemCrafting_BackEnd::UnSocket, &tCallbackData, FALSE, FALSE);

    SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_CRAFT, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());

    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);

    tACDPlayer.ModifiedMark(ACDMODIFIED_ATTRIBUTES);
}
ACDID SItemPlayerReplace(ActorCommonData & tACDOldItem, D3::Items::Generator & tNewItemGenerator, Player & tPlayer, BOOL bResocketGems);  // X/Server/SItems.cpp:861
BOOL ItemIsGemConversionReagent(GBID gbidItem);  // X/Common/Items.cpp:9736
ItemEnchantAffixError ItemCanEnchantAffix(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:3558
BOOL ItemIsAffixInherent(const ActorCommonData & tACDItem, const GBID gbidAffix, BOOL bIgnoreLegendaryAffixFamilies);  // X/Common/Items.cpp:3101
int32 LootBuildValidAffixList(GBHandleList & lstAffixes, const LootSpecifier & tSpecifier, const AffixType eLastAffixType, const uint32 dwFlags);  // X/Server/Loot/Loot.cpp:4744
int32 LootGetAffixFrequencyWeight(const AffixTableEntry * ptAffix, const LootSpecifier * ptSpecifier);  // X/Server/Loot/Loot.cpp:4970
BOOL ItemAffixAddsSockets(GBID gbidAffix);  // X/Common/Items.cpp:6071
GBID LootChooseAffixFromList(const LootSpecifier & tSpecifier, AffixType & eLastAffixType, const GBHandleList & lstAffixes, const int32 nMaxWeight);  // X/Server/Loot/Loot.cpp:4910
BOOL ItemPlayerConsumeEnchantAffixReagents(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem);  // X/Common/Items.cpp:8289
void ItemSetBoundToACD(ActorCommonData & tACD, const ACDID idACDOwner);  // X/Common/Items.cpp:5565
struct EnchantAffixChooseMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; EnchantAffixChoice tEnchantAffixChoices[6]; int32 nEnchantAffixChoicesCount; };  // size 0x40; X/Common/Crafting/ItemCrafting.h:130 (not in the adopted subset there)
static_assert(sizeof(EnchantAffixChooseMessage) == 0x40, "EnchantAffixChooseMessage");
static const int32 MAX_LEGENDARY_AFFIX_FAMILIES = 6;  // X/Common/Definitions/GameBalanceDefinition.h:458 (not adopted there yet)
enum { AFFIXGROUP_INVALID = -1, AFFIXGROUP_MAJOR = 0, AFFIXGROUP_MINOR = 1, NUM_AFFIXGROUPS = 2 };  // X/Common/Definitions/GameBalanceDefinition.h:316 (not adopted there yet)
enum { VAF_IGNORE_QUALITY_LEVEL = 0, VAF_IGNORE_LEVEL_REQUIREMENT = 1, VAF_PICK_AFFIXES_AS_LEGENDARY = 2, VAF_USE_LEGENDARY_ALLOWED_TYPES = 3, VAF_IGNORE_ZERO_FREQUENCY = 4, VAF_IS_CRAFTER = 5, VAF_IS_VENDOR = 6, VAF_PICK_MAJOR_AFFIXES = 7, VAF_PICK_MINOR_AFFIXES = 8, VAF_CANT_ADD_SOCKETS = 9, VAF_PRIMAL_PRIORITIZE_SOCKETS = 10, VAF_PRIMAL_DISALLOW_SOCKETS = 11 };  // X/Common/Items.h:262 (not adopted there yet)
enum { GB_AFFIXES = 8 };  // X/Common/Definitions/GameBalanceDefinition.h:49 (not in the adopted GB_ subset there)































BOOL SItemCrafting_TryUnlockTransmog(GBID gbidTransmog, Player & tPlayer, BOOL bMustBeInInventory, BOOL bSendFullUpdate)




{
    if (!ItemCrafting_CanUnlockTransmog(gbidTransmog, bMustBeInInventory ? &tPlayer : NULL, TRUE))
    {
        return FALSE;
    }

    GBID gbidUnlockedTransmog = GBID_NONE;
    if (ItemCrafting_IsTransmogUnlocked(gbidTransmog, &tPlayer, &gbidUnlockedTransmog))
    {

        ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
        ScopedItemDefinitionLockR pdefUnlockedTransmog(gbidUnlockedTransmog);
        if (pdefTransmog && pdefUnlockedTransmog &&
            (pdefTransmog->dwFlags & (1 << ITEM_ACCOUNT_WIDE_TRANSMOG)) && !(pdefUnlockedTransmog->dwFlags & (1 << ITEM_ACCOUNT_WIDE_TRANSMOG)))
        {
            CrafterTransmogData & tTransmogData = tPlayer.tCrafterSavedData.tTransmogData;

            if (tTransmogData.nUnlockedAccountTransmogs < 64)
            {



                for (int32 i = 0; i < tTransmogData.nUnlockedTransmogs; i++)
                {
                    if (tTransmogData.gbidUnlockedTransmogs[i] == gbidUnlockedTransmog)
                    {
                        tTransmogData.gbidUnlockedTransmogs[i] = tTransmogData.gbidUnlockedTransmogs[tTransmogData.nUnlockedTransmogs - 1];
                        tTransmogData.nUnlockedTransmogs--;

                        tTransmogData.gbidUnlockedAccountTransmogs[tTransmogData.nUnlockedAccountTransmogs] = gbidTransmog;
                        tTransmogData.nUnlockedAccountTransmogs++;
                        return TRUE;
                    }
                }
            }
        }


        return FALSE;
    }
    ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
    if (pdefTransmog && (pdefTransmog->dwFlags & (1 << ITEM_ACCOUNT_WIDE_TRANSMOG)))
    {
        int32 & nUnlockedAccountTransmogs = tPlayer.tCrafterSavedData.tTransmogData.nUnlockedAccountTransmogs;
        if (nUnlockedAccountTransmogs >= MAX_UNLOCKED_ACCOUNT_TRANSMOGS_PER_ACCOUNT)
        {
            return FALSE;
        }

        tPlayer.tCrafterSavedData.tTransmogData.gbidUnlockedAccountTransmogs[nUnlockedAccountTransmogs] = gbidTransmog; nUnlockedAccountTransmogs++;
    }
    else
    {

        int32 & nUnlockedTransmogs = tPlayer.tCrafterSavedData.tTransmogData.nUnlockedTransmogs;
        if (nUnlockedTransmogs >= MAX_UNLOCKED_PARTITION_TRANSMOGS_PER_ACCOUNT)
        {
            return FALSE;
        }

        tPlayer.tCrafterSavedData.tTransmogData.gbidUnlockedTransmogs[nUnlockedTransmogs] = gbidTransmog; nUnlockedTransmogs++;
    }




    uint32 dwNow = GameGetServerTime();
    if (dwNow > tPlayer.dwJoinTime)
    {
        if (bSendFullUpdate)
        {
            SItemCrafting_SendPlayerTransmogInfo(tPlayer, FALSE);
        }
        GBIDDataMessage tMessage;

        tMessage.gbidData = gbidTransmog;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
        MessageSendToClient(listDestPlayers, SMSG_UNLOCK_TRANSMOG_RESULTS, &tMessage, sizeof(tMessage));
    }
    return TRUE;

}
GBID LootPickGem(int32 nGemQuality, GemType eGemType);  // X/Server/Loot/Loot.cpp:7537
BOOL SpellEffect_DeathPortalCreate(const SNO snoActor, const WorldPlace tPlace, Power * ptPower);  // X/Server/Powers/SpellEffect.cpp:16681
BOOL SItemCrafting_TryUnlockSecondaryTransmogs(GBID gbidTransmog, Player & tPlayer)


{
    ScopedItemDefinitionLockR pdefItem(gbidTransmog);
    if (!pdefItem)
        return FALSE;
    BOOL bTransmogUnlocked = FALSE;
    for (int32 i = 0; i < 8; i++)
    {



        if (pdefItem->gbidTransmogsToGrant[i] != GBID_NONE)
        {
            if (SItemCrafting_TryUnlockTransmog(pdefItem->gbidTransmogsToGrant[i], tPlayer, FALSE, FALSE))
                bTransmogUnlocked = TRUE;
        }
    }



    if (bTransmogUnlocked)
    {
        SItemCrafting_SendPlayerTransmogInfo(tPlayer, FALSE);
    }
    return bTransmogUnlocked;

}
uint32 ItemGetRequiredAugmentJewelRankByGBID(GBID gbidAncientItem);  // X/Common/Items.cpp:11015
CurrencyType CurrencyFromGBID(GBID gbid);  // X/Common/Items.cpp:10444
void SItemCrafting_TryTransmog(ACDID idACDItem, GBID gbidTransmog, Player & tPlayer)
{


    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
        return;





    if (gbidTransmog == GBID_NONE)
    {
        gbidTransmog = ptACDItem->hGB.gbid;
    }


    if (ItemCrafting_ItemTransmogsMatch(*ptACDItem, gbidTransmog))
    {
        return;
    }


    if (!ItemCrafting_CanTransmogItem(*ptACDItem, gbidTransmog, &tPlayer, TRUE, TRUE))
    {
        return;
    }


    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!ItemPlayerChargeTransmogCosts(tACDPlayer, *ptACDItem, gbidTransmog))
    {
        return;
    }


    if (ItemCrafting_ItemTransmogsMatch(gbidTransmog, ptACDItem->hGB.gbid))
    {
        ptACDItem->AttributesSetInt(ATTRIBUTE_TRANSMOG_GBID, GBID_NONE);
    }
    else
    {

        ptACDItem->AttributesSetInt(ATTRIBUTE_TRANSMOG_GBID, gbidTransmog);
    }

    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_TRANSMOGGED_ITEM, ptACDItem->hGB.gbid, ItemGetStatsDataKey(*ptACDItem), 1.0f);
    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_TRANSMOGGED_ITEM_TO_ITEM, ptACDItem->hGB.gbid, gbidTransmog, 1.0f);


    SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_TRANSMOG, ptACDItem->GetLocationSlot(), ptACDItem->GetLocationSlot());


    InventorySlotType eEquippedSlot = ptACDItem->GetLocationSlot();
    VisualInventoryResetSlot(tPlayer.idPlayerACD, eEquippedSlot);
    TransmogItemMessage tMessage;

    tMessage.annItem = ANNFromACD(idACDItem);
    tMessage.gbidTransmog = ptACDItem->AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, SMSG_TRANSMOG_RESULTS, &tMessage, sizeof(tMessage));
    {





        ActorCommonData & tACDItem = ACDGet(idACDItem);
        Achievement::EvaluationData tEvaluationData;


        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetACD(&tACDItem);
        tEvaluationData.SetEquipmentInfo(tPlayer.idPlayerACD);

        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_TRANSMOG_ITEM, gbidTransmog, 1, (uint32)sub_71007b5e70());






        if (tEvaluationData.GetEquipmentInfo()->IsAllEquipmentTransmogrifiedToLegendary())
        {
            tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_EQUIP_ALL_ITEMS_TRANSMOG_LEGENDARY, 1, (uint32)sub_71007b5e70());
        }



    }
}
BOOL ItemCrafting_TransmuteEvaluate(const TransmuteInventory & tInventory, const TransmuteRecipeIngredient *const arIngredients, const int32 nIngredientsCount, TransmuteInventory & tTransmuteInventoryFinal, const int32 nIngredientToEvaluate);  // X/Common/Crafting/ItemCrafting.cpp:957
ACDID ACDFindBySNOName(const SNOName & tSNOName);  // X/Common/ActorCommonData.cpp:7398
static void sNotifyDyeResults(const Player & tPlayer, ACDID idACDItem, DyeType eDyeType)
{
    DyeItemMessage tMessage;



    tMessage.annItem = ANNFromACD(idACDItem);
    tMessage.eDyeType = eDyeType;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, SMSG_DYE_ITEM_RESULTS, &tMessage, sizeof(tMessage));
}


void SItemCrafting_TryDyeItem(ACDID idACDItem, DyeType eDyeType, Player & tPlayer)



{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
        return;




    DyeType eOldDyeType = ptACDItem->GetDyeType();
    if (eOldDyeType == eDyeType)
        return;




    if (!ItemIsDyeable(*ptACDItem))
        return;




    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!ItemPlayerChargeDyeItemCosts(tACDPlayer, eDyeType))
        return;



    if (!SItemApplyDye(*ptACDItem, tACDPlayer, eDyeType))
        return;




    SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_DYE, ptACDItem->GetLocationSlot(), ptACDItem->GetLocationSlot());


    InventorySlotType eEquippedSlot = ptACDItem->GetLocationSlot();
    VisualInventoryResetSlot(tPlayer.idPlayerACD, eEquippedSlot);
    {





        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);

        if (tEvaluationData.GetPlayer())
        {
            tEvaluationData.SetACD(ptACDItem);

            tEvaluationData.EvaluateEvent(
                tEvaluationData.GetPlayer(),
                Achievement::ACH_EVENT_DYE_ITEM,
                ItemGetGBIDForDyeType(eDyeType),
                1, (uint32)sub_71007b5e70());

            SACDInventoryEvaluateItemDyeAchievements(&tEvaluationData);
        }
    }

    sNotifyDyeResults(tPlayer, idACDItem, eDyeType);
}



static void sTransmuteConsumeItems(
    const Player & tPlayer,
    TransmuteInventory & tTransmuteInventoryFinal,
    ACDID idACDItemToIgnore)
{
    ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    // 602-605: no row of its own; ACDTryToGet's failure branches (+0x34/+0x54/+0x68/+0xa4) go straight to the epilogue
    if (ptACDPlayer == NULL)
        return;


    int32 nInputs = tTransmuteInventoryFinal.arInputs.size();

    for (int32 i = 0; i < nInputs; ++i)
    {
        TransmuteInput & tInput = tTransmuteInventoryFinal.arInputs[i];

        if (tInput.gbidCurrency != GBID_NONE)
        {
            CurrencyType eCurrency = CurrencyFromGBID(tInput.gbidCurrency);
            if (eCurrency != CURRENCY_INVALID)
            {
                int64 nStartCount = ptACDPlayer->GetCurrencyAmount(eCurrency);
                int64 nEndCount = tInput.nStackCount;
                int64 nDeleteCount = nStartCount - nEndCount;
                ptACDPlayer->ModifyCurrencyAmount(-nDeleteCount, eCurrency, GMR_CUBE_RECIPE, FALSE);
            }
        }
        else
        {
            ACDID idACDItem = tInput.ptACDItem->id;
            ActorCommonData * ptACDItem = tInput.ptACDItem;
            // idACDItemToIgnore == ACDID_INVALID: the original unswitches the loop on it (609: cmn w22, #0x1 / b.eq +0x1fc); without it there is one loop copy (544 vs 856 B)
            if (idACDItem != ACDID_INVALID && ptACDItem != NULL && (idACDItemToIgnore == ACDID_INVALID || idACDItem != idACDItemToIgnore))
            {






                if (ItemGetNumFilledSockets(*ptACDItem) > 0)
                {
                    ActorCommonData &tACDPlayer = ACDGet(tPlayer.idPlayerACD);
                    ItemCrafting_BackEnd::UnsocketCallbackData tCallbackData;
                    tCallbackData.ptACDPlayer = &tACDPlayer;
                    tCallbackData.bTriggerAchievementEvent = FALSE;
                    ACDInventoryIterateSlot(*ptACDItem, SLOT_ITEM_SOCKETS, NULL, ItemCrafting_BackEnd::UnSocket, &tCallbackData, FALSE, FALSE);
                }


                int64 nStartCount = Math::Max<int64>(ptACDItem->GetStackCount(), 1);
                int64 nEndCount = tInput.nStackCount;
                int64 nDeleteCount = nStartCount - nEndCount;

                if (nDeleteCount > 0)
                {
                    SACDInventoryDeleteItem(*ptACDItem, nDeleteCount, TRUE, FALSE);
                }
            }
        }
    }
}

static BOOL sFindKanaiPortalSpawnLocation(
    const Player & tPlayer,
    TransmuteRecipeAction eAction,
    WorldPlace & tWorldPlace)
{
    if (eAction != TRA_OPEN_PORTAL_GREED && eAction != TRA_OPEN_PORTAL_COW)  // inferred, no row: inlined, eAction 12 jumps straight to the caller's failure path
    {
        return FALSE;
    }

    uint32 tPortalMarkerName;

    if (eAction == TRA_OPEN_PORTAL_COW)
    {
        tPortalMarkerName = MarkerStringHash("CubeCowPortal");
    }
    else
    {
        tPortalMarkerName = MarkerStringHash("CubeGreedPortal");
    }

    ActorCommonData &tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        Actor *ptActor = tIt.ptElement;
        if (!ptActor)
            continue;



        ActorCommonData *ptACD = ACDTryToGet(ptActor->idACD);
        if (!ptACD)
            continue;



        if (!ptACD->IsInSWorldID(tACDPlayer.GetSWorldID()))
            continue;



        if (ptActor->tName == tPortalMarkerName)
        {

            tWorldPlace = ptACD->GetPlace();
            return TRUE;
        }
    }

    return FALSE;
}

static int32 s_nStoredSeed;  // line 714
static int32 s_nStoredCarry;  // line 715


static void sConsoleRNGOutOfScope(
    Player * ptPlayer,
    GBID gbidItemType)
{
    // written from the original (0x7d5e40, 68 bytes): unordered_map operator[] then the inlined make_pair / pair assignment at 725


    ptPlayer->tMapPartitionWideCubeSeeds[gbidItemType] = blz::make_pair(s_nStoredSeed, s_nStoredCarry);
}


blz::pair<uint32, uint32> sGetOverrideSeedForItem(
    Player & tPlayer,
    GBID gbidItemType)
{

    uint32 nRetVal = 0;
    uint32 nCarry = 420;


    tPlayer.uConsoleSyncFlags |= (1 << PCSF_PARTITION_WIDE_CUBE_SEEDS);

    blz::unordered_map<int32, blz::pair<uint32, uint32> >::iterator it = tPlayer.tMapPartitionWideCubeSeeds.find(gbidItemType);
    if (it != tPlayer.tMapPartitionWideCubeSeeds.end())  // no row of its own: _find_node_link's b.eq goes straight to 743
    {
        return it->second;
    }

    nRetVal = tPlayer.uCubeBaseSeed;
    nRetVal ^= gbidItemType;


    if (GameIsSeasonal())
    {
        nRetVal += OnlineService::GetSeasonNum();
    }

    if (tPlayer.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE))
    {
        nRetVal = ~nRetVal;  // no row of its own: ldr/sbfx/eor are all 755:9 (if-converted)
    }

    return blz::make_pair(nRetVal, nCarry);
}






static BOOL sTransmuteExecuteAction(
    TransmuteResultsMessage & tResultsMessage,
    GameError & eFailureError,
    TransmuteRecipeAction eAction,
    blz::vector<ACDID, blz::allocator<ACDID> > & aridACDItems,
    int32 nItemsCount,
    TransmuteInventory & tTransmuteInventoryFinal,
    Player & tPlayer,
    const TransmuteRecipeIngredient *const arIngredients)
{
    eFailureError = GAMEERROR_NONE;

    switch (eAction)
    {
    case TRA_EXTRACT_LEGENDARY_POWER:  // jump table entry 0 -> +0x8b8 (line 787)
    {

        GBID gbidLegendaryWithPower = GBID_NONE;
        ActorCommonData *ptACDItemWithLegendaryPower = NULL;
        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            if (ptACDItem && ItemGetLegendaryPower(ptACDItem->hGB.gbid) != SNO_NONE)
            {
                gbidLegendaryWithPower = ptACDItem->hGB.gbid;
                ptACDItemWithLegendaryPower = ptACDItem;  // no code: DWARF locates it only where ptACDItem lives (w10, +0x930..+0x944)
                break;  // no code: the found path branches from 792 straight to +0xf58 (line 800)
            }
        }

        // no code of its own: pdefItem (800) lives in a nested block 800-801; its destructor rows are on line 805
        {
            ScopedItemDefinitionLockR pdefItem(gbidLegendaryWithPower);
            if (pdefItem && pdefItem->gbidLegendaryPowerItemReplacement != GBID_NONE)
            {
                gbidLegendaryWithPower = pdefItem->gbidLegendaryPowerItemReplacement;  // no row of its own: csel at 801:17
            }
        }


        ScopedItemDefinitionLockR pdefItem(gbidLegendaryWithPower);
        if (pdefItem && (pdefItem->dwFlags & (1 << ITEM_CANNOT_EXTRACT_POWER)))
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_LEGENDARY_POWER_BLACKLISTED;
            return FALSE;  // no row of its own: w23 = 0 at line 0, then pdefItem's destructor on line 850
        }


        if (gbidLegendaryWithPower == GBID_NONE ||
            !ItemCanExtractLegendaryPower(gbidLegendaryWithPower))
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_EXTRACT_FROM_THIS_ITEM;
            return FALSE;  // no row of its own: branches to the line-811 exit (w23 = 0)
        }


        if (ItemPlayerHasExtractedLegendaryPower(tPlayer, gbidLegendaryWithPower))
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_LEGENDARY_POWER_ALREADY_EXTRACTED;
            return FALSE;  // no row of its own: branches to the line-811 exit (w23 = 0)
        }


        if (!SItemPlayerExtractLegendaryPower(tPlayer, gbidLegendaryWithPower, eFailureError))
        {
            return FALSE;  // no row of its own: cbz to the shared w23 = 0 tail at +0x174c
        }

        tResultsMessage.gbidPower = gbidLegendaryWithPower;


        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);









        return TRUE;  // no row of its own: w23 = 1 is attributed to 839; pdefItem's destructor is on 850
    }

    case TRA_REFORGE_LEGENDARY:  // jump table entry 1 -> +0x970 (line 857)
    {

        ACDID idACDLegendaryItem = ACDID_INVALID;
        ActorCommonData *ptACDLegendaryItem = NULL;
        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            if (ptACDItem && ItemIsLegendary(ptACDItem->hGB.gbid) && ItemIsEquippableByPlayerOrHireling(ptACDItem->hGB.gbid))
            {
                ptACDLegendaryItem = ptACDItem;  // no code: x24 already holds ptACDItem (ItemIsCrafted at 876 takes x24)
                idACDLegendaryItem = ptACDItem->id;
                break;  // no row of its own: block 859-863 ends at 863, found path continues at 876
            }
        }

        // no code of its own: the not-found exit at +0xa28 (line 0) stores 0xa1 at line 871
        if (ptACDLegendaryItem == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;  // no row of its own: b to the shared exit +0x24dc
        }


        if(ItemIsCrafted(*ptACDLegendaryItem))
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_USE_CRAFTED_ITEM;
            return FALSE;  // no row of its own: b to the shared exit +0x24dc
        }


        ConsoleScopedOverrideRNG overrideRNG(ptACDLegendaryItem->AttributesGetInt(ATTRIBUTE_SEED), 666, RNGOutOfScopeCallback());
        ACDID idACDNewItem = SItemReforge(*ptACDLegendaryItem, tPlayer, FALSE, FALSE);

        tResultsMessage.annItem = ANNFromACD(idACDNewItem);


        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, idACDLegendaryItem);




        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(&tPlayer);
            tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_REFORGED_LEGENDARY, 1, (uint32)sub_71007b5e70());
        }

        return TRUE;  // no row of its own: overrideRNG's destructor is on 901, then b +0x2860 (success)
    }

    case TRA_UPGRADE_RARE_ITEM:  // jump table entry 2 -> +0xa3c (line 907)
    {

        ActorCommonData *ptACDRareItem = NULL;
        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            if (ptACDItem == NULL)  // no row of its own: ACDTryToGet's failure branches go straight to the loop increment (+0xb0c)
            {
                continue;
            }

            ItemQualityLevel eIQL = ItemGetQualityLevel(*ptACDItem);
            if (ItemIsEquippableByPlayerOrHireling(*ptACDItem) && eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
            {

                int32 nLevelReq = ItemGetLevelRequirement(ptACDItem->hGB.gbid, ptACDItem, TRUE);
                if (nLevelReq >= HERO_LEVEL_MAX)  // no row of its own: cmp #0x45 / b.gt at line 0 (attributed to 919)
                {
                    ptACDRareItem = ptACDItem;  // no code: x24 already holds ptACDItem
                    break;  // no code: b.gt goes straight to +0x1784 (line 935)
                }
            }
        }
        // no code of its own: the loop exit at +0xb18 (line 0) stores 0xa1 and leaves through +0x10b8
        if (ptACDRareItem == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;
        }


        ScopedItemDefinitionLockR pdefRareItem(ptACDRareItem->hGB.gbid);
        if (!pdefRareItem)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;  // no row of its own: w23 = 0 is attributed to 938
        }




        blz::pair<uint32, uint32> tSeed = sGetOverrideSeedForItem(tPlayer, pdefRareItem->gbidItemType);
        ConsoleScopedOverrideRNG overrideRNG(tSeed.first, tSeed.second,
            blz::bind(&sConsoleRNGOutOfScope, &tPlayer, pdefRareItem->gbidItemType));

        RNG &tRNG = SGameGlobalsGet().rng;



        int32 nRoll = Rand_Internal(tRNG, 99);
        s_nStoredSeed = tRNG.dwSeed;
        s_nStoredCarry = tRNG.dwCarry;



        int32 nItemLevel = HERO_LEVEL_MAX;
        GBID gbidLegendaryItem = LootPickLegendaryOfItemType(tPlayer, pdefRareItem->gbidItemType, nItemLevel);
        if (gbidLegendaryItem == GBID_NONE)
        {


            ScopedItemTypeDefinitonLockR pdefItemType(pdefRareItem->gbidItemType);
            if (pdefItemType->gbidParent != GBID_NONE)
            {
                gbidLegendaryItem = LootPickLegendaryOfItemType(tPlayer, pdefItemType->gbidParent, nItemLevel);
            }

            if (gbidLegendaryItem == GBID_NONE)
            {
                eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
                return FALSE;  // no row of its own: w23 = 0 at line 0 after the line-976 destructors
            }
        }


        LootSpecifier tSpecifier;
        tSpecifier.hGBItem.eType = GB_ITEMS;
        tSpecifier.hGBItem.gbid = gbidLegendaryItem;
        tSpecifier.tLooteeParams.nLevel = nItemLevel;
        tSpecifier.nLegendaryItemLevel = nItemLevel;
        tSpecifier.nAffixLevel = nItemLevel;  // no row of its own: merged into the 64-bit store at 985 (0x46 << 32 | 9)
        tSpecifier.eIQL = IQL_LEGENDARY;
        tSpecifier.fAutoIdentify = TRUE;
        tSpecifier.fAccountBound = TRUE;
        tSpecifier.tLooteeParams.bForceCrafterAncientLegendaryChance = TRUE;
        LootRollForAncientLegendary(tSpecifier, tPlayer.idPlayerACD);

        PlayerClass ePlayerClass = PlayerGetPlayerClass(&tPlayer);
        tSpecifier.FillInTargetedClass(ePlayerClass, NULL, TRUE);


        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer == NULL)  // no row of its own: ACDTryToGet's failure branches go straight to 998
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;  // no row of its own: w23 = 0 is attributed to 998
        }

        ACDID idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
        ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem == NULL)  // no row of its own: ACDTryToGet's failure branches go straight to 1006
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;  // no row of its own: w23 = 0 is attributed to 1006
        }

        tResultsMessage.annItem = ANNFromACD(idACDItem);



        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);

        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDItem, FALSE, NULL, TRUE);

        SItemCrafting_TryUnlockTransmog(ptACDItem->hGB.gbid, tPlayer, TRUE, TRUE);




        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(&tPlayer);
            tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_PLAYER_UPGRADED_RARE, 1, (uint32)sub_71007b5e70());
        }

        SItemBroadcastToClan(*ptACDItem, tPlayer);

        return TRUE;  // no row of its own: w23 = 1 is attributed to 1016; overrideRNG and pdefRareItem are destroyed on 1032
    }

    case TRA_CONVERT_SET_ITEM:  // jump table entry 3 -> +0xb2c
    {


        ActorCommonData *ptACDSetItem = NULL;
        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            if (ptACDItem == NULL)  // no row of its own: folded into ElementTryToGet's ccmp/b.eq at +0xbac
            {
                continue;
            }

            if (ItemIsSetItem(ptACDItem->hGB.gbid))
            {
                ptACDSetItem = ptACDItem;
                break;
            }
        }
        // No rows of its own: the not-found exit shares the 161 store at +0xe0c (H02g line 1205).
        if (ptACDSetItem == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;
        }



        if(ItemIsCrafted(*ptACDSetItem))
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_USE_CRAFTED_ITEM;  // w8=159 at +0x1668, store shared with H02g at +0xe10
            return FALSE;
        }


        if (ItemGetNumberOfItemsInSameSet(ptACDSetItem->hGB.gbid) < 3)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_NOT_ENOUGH_ITEMS_IN_SET;
            return FALSE;
        }

        int32 nItemLevel = ItemGetItemLevel(ptACDSetItem->hGB.gbid);
        if(ptACDSetItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE) > 0)
        {
            nItemLevel = ptACDSetItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
        }

        ConsoleScopedOverrideRNG overrideRNG(ptACDSetItem->AttributesGetInt(ATTRIBUTE_SEED), 666, RNGOutOfScopeCallback());
        GBID gbidSetItem = LootPickItemFromSet(tPlayer, ItemGetSet(ptACDSetItem->hGB.gbid), ptACDSetItem->hGB.gbid, nItemLevel);
        if (gbidSetItem == GBID_NONE)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;
        }


        LootSpecifier tSpecifier;
        tSpecifier.hGBItem.eType = GB_ITEMS;
        tSpecifier.hGBItem.gbid = gbidSetItem;
        tSpecifier.tLooteeParams.nLevel = nItemLevel;
        tSpecifier.nLegendaryItemLevel = nItemLevel;
        tSpecifier.nAffixLevel = nItemLevel;
        tSpecifier.eIQL = IQL_LEGENDARY;
        tSpecifier.fAutoIdentify = TRUE;
        tSpecifier.fAccountBound = TRUE;
        tSpecifier.tLooteeParams.bForceCrafterAncientLegendaryChance = TRUE;
        tSpecifier.eAncientRank = ANCIENT_RANK_NONE;

        PlayerClass ePlayerClass = PlayerGetPlayerClass(&tPlayer);
        tSpecifier.FillInTargetedClass(ePlayerClass, NULL, TRUE);


        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        ACDID idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
        ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        tResultsMessage.annItem = ANNFromACD(idACDItem);



        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);

        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDItem, FALSE, NULL, TRUE);

        SItemCrafting_TryUnlockTransmog(ptACDItem->hGB.gbid, tPlayer, TRUE, TRUE);




        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(&tPlayer);
            tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_PLAYER_CONVERTED_SET_ITEM, 1, (uint32)sub_71007b5e70());
        }

        return TRUE;  // no row of its own: success b at line 1138 reaches the exit through the overrideRNG cleanup
    }

    case TRA_REMOVE_LEVEL_REQ:  // jump table entry 4 -> +0xbd4
    {

        ActorCommonData *ptACDEquippableItem = NULL;
        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            if (ptACDItem == NULL)  // no row of its own: folded into ElementTryToGet's ccmp/b.eq at +0xc54
            {
                continue;
            }

            if (ItemIsEquippableByPlayerOrHireling(ptACDItem->hGB.gbid))
            {
                ptACDEquippableItem = ptACDItem;
                break;
            }
        }
        // No rows of its own: loop exit sets w8=160 at +0xc78 and shares the store at +0xe10 (H02g).
        if (ptACDEquippableItem == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_EQUIP_ITEM;
            return FALSE;
        }


        AttribGroupID idFastAttrib = ptACDEquippableItem->AttributesGetAttributeGroup();

        if (FastAttribGetValueInt(idFastAttrib, ATTRIBUTE_REMOVE_LEVEL_REQUIREMENT) > 0)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_ALREADY_REMOVED_LEVEL_REQ;  // w8=163 at +0x1694, store shared at +0x10b4
            return FALSE;
        }

        FastAttribSetValueInt(idFastAttrib, ATTRIBUTE_REMOVE_LEVEL_REQUIREMENT, TRUE);


        D3::Items::Generator tGenerator;
        ItemGeneratorPopulateFromACDID(*ptACDEquippableItem, tGenerator, TRUE);
        tGenerator.set_item_binding_level(IBL_ACCOUNT_PICKUP);


        ACDID idNewItem = SItemPlayerReplace(*ptACDEquippableItem, tGenerator, tPlayer, FALSE);

        ActorCommonData *ptACDNewItem = ACDTryToGet(idNewItem);
        if (ptACDNewItem != NULL)  // no row of its own: line-0 b.eq at +0x1c10
        {
            tResultsMessage.annItem = ANNFromACD(idNewItem);
        }



        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ptACDEquippableItem->id);

        return TRUE;  // no row of its own: the b at 1198 after ~Generator goes to the success exit
    }

    case TRA_UPGRADE_ITEM_TO_PLAYER_LEVEL:  // jump table entry 9 -> +0xd68
    {

        ACDID idACDItemToUpgrade = ACDID_INVALID;
        ActorCommonData *ptACDItemToUpgrade = NULL;
        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            if (ptACDItem && ItemIsLegendary(ptACDItem->hGB.gbid))
            {
                ptACDItemToUpgrade = ptACDItem;
                idACDItemToUpgrade = ptACDItem->id;
                break;
            }
        }
        // No rows of its own: loop exit falls into the 161 store at +0xe0c (line 0 of 1205).
        if (ptACDItemToUpgrade == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;
        }



        if (ItemIsCrafted(*ptACDItemToUpgrade))
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_USE_CRAFTED_ITEM;  // w8=159 at +0x16ac, store shared at +0x1cbc
            return FALSE;
        }

        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer == NULL)  // no row of its own: line-0 b.ne at +0x1cb0
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;  // 155 at +0x1cb4 (line 0)
            return FALSE;
        }


        int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
        int32 nItemLevel = ItemGetItemLevel(ptACDItemToUpgrade->hGB.gbid);
        if (ptACDItemToUpgrade->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE) > 0)
        {
            nItemLevel = ptACDItemToUpgrade->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
        }


        if (nItemLevel >= nPlayerLevel)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_ITEM_LEVEL_TOO_LOW;  // 162 at +0x276c, shared with H02d
            return FALSE;
        }


        ptACDItemToUpgrade->AttributesSetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE, nPlayerLevel);
        ACDID idNewItem = SItemReforge(*ptACDItemToUpgrade, tPlayer, FALSE, FALSE);


        if (idNewItem == ACDID_INVALID)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;  // 155 at +0x2608 (H02h), shared
            return FALSE;
        }

        tResultsMessage.annItem = ANNFromACD(idNewItem);


        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, idACDItemToUpgrade);

        return TRUE;  // no row of its own: the shared sTransmuteConsumeItems tail (+0x25e4) continues to the success exit
    }

    case TRA_CONVERT_GEMS:  // jump table entry 8 -> +0xc80
    {
        ActorCommonData *ptACDGem = NULL;
        ActorCommonData *ptACDReagent = NULL;


        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            if (ptACDItem == NULL)  // no row of its own: folded into ElementTryToGet's b.eq to the increment at +0xd38
            {
                continue;
            }

            if (ItemIsGem(ptACDItem->hGB.gbid))
            {
                ptACDGem = ptACDItem;
            }
            else if (ItemIsGemConversionReagent(ptACDItem->hGB.gbid))
            {
                ptACDReagent = ptACDItem;
            }
        }


        if (ptACDGem == NULL)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;  // 161 at +0xf3c (H02l), shared
            return FALSE;
        }

        // No row of its own: GEM_NONE is the cset's false value at line 1321.
        GemType eGemType = GEM_NONE;
        GBID gbidReagent = ptACDReagent->hGB.gbid;
        if (gbidReagent == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_RUBY))
        {
            eGemType = GEM_RUBY;
        }
        else if (gbidReagent == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_DIAMOND))
        {
            eGemType = GEM_DIAMOND;
        }
        else if (gbidReagent == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_TOPAZ))
        {
            eGemType = GEM_TOPAZ;
        }
        else if (gbidReagent == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_EMERALD))
        {
            eGemType = GEM_EMERALD;
        }
        else if (gbidReagent == GlobalGBIDGet(GLOBALGBID_ITEM_GEM_CONVERT_AMETHYST))
        {
            eGemType = GEM_AMETHYST;
        }


        ScopedItemDefinitionLockR pdefGem(ptACDGem->hGB.gbid);
        if (pdefGem->eGemType == eGemType)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;  // 161 at +0x1af8 (line 0)
            return FALSE;
        }


        int32 nGemQuality = ptACDGem->AttributesGetInt(ATTRIBUTE_GEM_QUALITY);


        GBID gbidGem = LootPickGem(nGemQuality, eGemType);
        if (gbidGem == GBID_NONE)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;  // shares the 161 path at +0x1af4
            return FALSE;
        }

        uint64 uStackSize = 0;
        for (int32 i = 0; i < 8; ++i)
        {
            if (arIngredients[i].eType == TRIT_GEM)
            {
                uStackSize = arIngredients[i].nQuantity;
                break;
            }
        }


        LootSpecifier tSpecifier;
        tSpecifier.hGBItem.eType = GB_ITEMS;
        tSpecifier.hGBItem.gbid = gbidGem;
        tSpecifier.tLooteeParams.nLevel = ItemGetItemLevel(ptACDGem->hGB.gbid);
        tSpecifier.eIQL = IQL_NORMAL;
        tSpecifier.bTryStack = TRUE;
        tSpecifier.uDesiredStackCount = uStackSize;


        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (ptACDPlayer == NULL)  // no row of its own: line-0 b.ne at +0x2838
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        ACDID idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
        ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem == NULL)  // no row of its own: line-0 b.ne/cbz at +0x2a20/+0x2a24
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }


        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);

        ACDID idACDStack = ACDID_INVALID;
        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDItem, FALSE, &idACDStack, TRUE);

        if (idACDStack != ACDID_INVALID)
        {
            tResultsMessage.annItem = ANNFromACD(idACDStack);  // merged with line 1392's call (csel at 1386, store at line 0)
        }
        else
        {
            tResultsMessage.annItem = ANNFromACD(idACDItem);
        }

        return TRUE;
    }

    case TRA_CONVERT_CRAFTING_MATS_FROM_NORMAL:
    case TRA_CONVERT_CRAFTING_MATS_FROM_MAGIC:
    case TRA_CONVERT_CRAFTING_MATS_FROM_RARE:
    {

        ActorCommonData *ptACDEquippableItem = NULL;

        GBID gbidCraftingNormal = GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_NORMAL);
        GBID gbidCraftingMagic = GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_MAGIC);
        GBID gbidCraftingRare = GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_RARE);


        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            // inferred: no row of its own; ACDTryToGet's failure exits (+0x718, +0x738, +0x74c) branch straight to ++i
            if (!ptACDItem)
                continue;


            GBID gbidItem = ptACDItem->hGB.gbid;

            if (ItemIsEquippableByPlayerOrHireling(gbidItem) && !ItemIsLegendary(gbidItem))
            {
                ptACDEquippableItem = ptACDItem;
            }
        }


        if (!ptACDEquippableItem)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;
        }


        GBID gbidNewCraftingMat = GBID_NONE;
        ItemQualityLevel eIQL = ItemGetQualityLevel(*ptACDEquippableItem);
        if (eIQL >= IQL_INFERIOR && eIQL <= IQL_SUPERIOR)
        {
            gbidNewCraftingMat = GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_NORMAL);
        }
        else if (eIQL >= IQL_MAGIC_AFFIX_START && eIQL <= IQL_MAGIC_AFFIX_END)
        {
            gbidNewCraftingMat = GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_MAGIC);
        }
        else if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
        {
            gbidNewCraftingMat = GlobalGBIDGet(GLOBALGBID_ITEM_CRAFTING_RARE);
        }

        GBID gbidOldCraftingMat = GBID_NONE;
        switch (eAction)
        {
        case TRA_CONVERT_CRAFTING_MATS_FROM_NORMAL:
            gbidOldCraftingMat = gbidCraftingNormal;
            break;
        case TRA_CONVERT_CRAFTING_MATS_FROM_MAGIC:
            gbidOldCraftingMat = gbidCraftingMagic;
            break;
        case TRA_CONVERT_CRAFTING_MATS_FROM_RARE:
            gbidOldCraftingMat = gbidCraftingRare;
            break;
        }




        if (gbidNewCraftingMat == GBID_NONE || gbidNewCraftingMat == gbidOldCraftingMat)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_FIND_ITEM_TO_UPGRADE;
            return FALSE;
        }


        uint64 uStackSize = 0;
        for (int32 i = 0; i < 8; ++i)
        {
            if (arIngredients[i].eType == TRIT_SPECIFIC_GBID && arIngredients[i].nTypeValue == gbidOldCraftingMat)
            {
                uStackSize = arIngredients[i].nQuantity;
                break;
            }
        }


        LootSpecifier tSpecifier;
        tSpecifier.hGBItem.eType = GB_ITEMS;
        tSpecifier.hGBItem.gbid = gbidNewCraftingMat;
        tSpecifier.eIQL = IQL_NORMAL;
        tSpecifier.bTryStack = TRUE;
        tSpecifier.uDesiredStackCount = uStackSize;


        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (!ptACDPlayer)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        ACDID idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
        ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
        if (!ptACDItem)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        tResultsMessage.gbidFakeItem = gbidNewCraftingMat;
        tResultsMessage.uFakeItemStackCount = uStackSize;


        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);

        ACDID idACDStack = ACDID_INVALID;
        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDItem, FALSE, &idACDStack, TRUE);

        return TRUE;
    }

    case TRA_OPEN_PORTAL_GREED:
    case TRA_OPEN_PORTAL_COW:
    case TRA_OPEN_PORTAL_WHIMSEYSHIRE:
    {

        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (!ptACDPlayer)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        SNO snoActor = SNO_NONE;
        switch (eAction)
        {
        case TRA_OPEN_PORTAL_GREED:
            snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_PORTAL_GREED);
            break;
        case TRA_OPEN_PORTAL_COW:
            snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_PORTAL_COW);
            break;
        case TRA_OPEN_PORTAL_WHIMSEYSHIRE:
            snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_PORTAL_WHIMSEYWHIRE);
            break;
        default:
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;  // no row of its own: 155 at the shared +0x1558 tail
            return FALSE;
        }



        WorldPlace tPlaceResult;
        if (!sFindKanaiPortalSpawnLocation(tPlayer, eAction, tPlaceResult))
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        BOOL bSuccess = SpellEffect_DeathPortalCreate(snoActor, tPlaceResult, NULL);
        if (!bSuccess)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);

        return TRUE;  // no row of its own: the shared sTransmuteConsumeItems tail (+0x25e4) continues to the success exit
    }

    case TRA_AUGMENT_ARMOR:
    {
        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (!ptACDPlayer)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;  // 155 at +0xe94, line 0
            return FALSE;
        }


        ActorCommonData *ptACDLegendaryGem = NULL;
        ActorCommonData *ptACDNormalGem = NULL;
        ActorCommonData *ptACDAncient = NULL;

        for (int32 i = 0; i < nItemsCount; ++i)
        {
            ActorCommonData *ptACDItem = ACDTryToGet(aridACDItems[i]);
            // inferred: no row of its own; ACDTryToGet's failure exits (+0x1218, +0x1238) branch straight to ++i
            if (!ptACDItem)
                continue;


            GBID gbidItem = ptACDItem->hGB.gbid;

            if (ItemIsAncientRank(*ptACDItem))
            {
                ptACDAncient = ptACDItem;
            }
            else if (ItemIsUpgradeableJewel(gbidItem))
            {
                ptACDLegendaryGem = ptACDItem;
            }
            else if (ItemIsGem(gbidItem))
            {
                ptACDNormalGem = ptACDItem;
            }
        }


        if (!ptACDAncient || !ptACDLegendaryGem || !ptACDNormalGem)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }


        GemType eGemType = GEM_NONE;

        ScopedItemDefinitionLockR pdefNormalGem(ptACDNormalGem->hGB.gbid);
        if (pdefNormalGem)
        {
            eGemType = pdefNormalGem->eGemType;
        }
        int32 nOldGemType = ptACDAncient->AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE);
        ptACDAncient->AttributesSetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE, eGemType);


        int32 nJewelRank = ptACDLegendaryGem->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);


        int32 nRequiredRank = ItemGetRequiredAugmentJewelRankByGBID(ptACDAncient->hGB.gbid);

        if (nJewelRank < nRequiredRank)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED_LOW_JEWEL_RANK;
            ptACDAncient->AttributesSetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE, nOldGemType);
            return FALSE;
        }


        int32 nOldJewelRank = ptACDAncient->AttributesGetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK);
        ptACDAncient->AttributesSetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK, nJewelRank);


        D3::Items::Generator tGenerator;
        ItemGeneratorPopulateFromACDID(*ptACDAncient, tGenerator, TRUE);
        tGenerator.set_item_binding_level(IBL_ACCOUNT_PICKUP);


        ACDID idNewItem = SItemPlayerReplace(*ptACDAncient, tGenerator, tPlayer, TRUE);
        if (idNewItem == ACDID_INVALID)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            ptACDAncient->AttributesSetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_RANK, nOldJewelRank);
            ptACDAncient->AttributesSetInt(ATTRIBUTE_CUBE_ENCHANTED_GEM_TYPE, nOldGemType);
            return FALSE;
        }

        tResultsMessage.annItem = ANNFromACD(idNewItem);
        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);




        {
            if (nJewelRank >= 30)
            {
                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.SetPlayer(&tPlayer);
                tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_AUGMENT_ANCIENT_30_PLUS, 1, (uint32)sub_71007b5e70());
                if (nJewelRank >= 40)
                {
                    tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_AUGMENT_ANCIENT_40_PLUS, 1, (uint32)sub_71007b5e70());
                }
                if (nJewelRank >= 50)
                {
                    tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_AUGMENT_ANCIENT_50_PLUS, 1, (uint32)sub_71007b5e70());
                }
            }
        }


        return TRUE;
    }

    case TRA_CONVERT_PAGES:
    {



        if (nItemsCount != 1)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;  // 155 at +0x1cb4 (H02g line 1230), shared
            return FALSE;
        }

        ActorCommonData *ptACDPage = ACDTryToGet(aridACDItems[0]);
        if (!ptACDPage)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        GBID gbidStranglethorn = GBID_NONE;
        GBID gbidBlackrock = ptACDPage->hGB.gbid;
        if (gbidBlackrock == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_1))
        {
            gbidStranglethorn = GlobalGBIDGet(GLOBALGBID_ITEM_STRANGLETHORN_1);
        }
        else if (gbidBlackrock == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_2))
        {
            gbidStranglethorn = GlobalGBIDGet(GLOBALGBID_ITEM_STRANGLETHORN_2);
        }
        else if (gbidBlackrock == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_3))
        {
            gbidStranglethorn = GlobalGBIDGet(GLOBALGBID_ITEM_STRANGLETHORN_3);
        }
        else if (gbidBlackrock == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_4))
        {
            gbidStranglethorn = GlobalGBIDGet(GLOBALGBID_ITEM_STRANGLETHORN_4);
        }
        else if (gbidBlackrock == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_5))
        {
            gbidStranglethorn = GlobalGBIDGet(GLOBALGBID_ITEM_STRANGLETHORN_5);
        }
        else if (gbidBlackrock == GlobalGBIDGet(GLOBALGBID_ITEM_BLACKROCK_LEDGER_6))
        {
            gbidStranglethorn = GlobalGBIDGet(GLOBALGBID_ITEM_STRANGLETHORN_6);
        }
        else
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }


        LootSpecifier tSpecifier;
        tSpecifier.hGBItem.eType = GB_ITEMS;
        tSpecifier.hGBItem.gbid = gbidStranglethorn;
        tSpecifier.eIQL = IQL_NORMAL;


        ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
        if (!ptACDPlayer)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        ACDID idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
        ActorCommonData *ptACDItem = ACDTryToGet(idACDItem);
        if (!ptACDItem)
        {
            eFailureError = GAMEERROR_TRANSMUTE_FAILED;
            return FALSE;
        }

        tResultsMessage.annItem = ANNFromACD(idACDItem);


        sTransmuteConsumeItems(tPlayer, tTransmuteInventoryFinal, ACDID_INVALID);

        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDItem, FALSE, NULL, TRUE);

        return TRUE;
    }

    default:
        eFailureError = GAMEERROR_TRANSMUTE_FAILED;  // no row of its own: the default jump (+0x6b0) lands on the shared 155 store at +0x1cb4
        return FALSE;
    }
}










struct FindSameItemsCallbackData
{
    ActorCommonData * ptACDPlayer;
    blz::vector<ACDID, blz::allocator<ACDID> > * ptarIngredients;
    GBID gbidItem;
    ACDID idOriginalItem;

    FindSameItemsCallbackData()
    {

        ptarIngredients = NULL;  // no code: dead store, the caller sets it on 1846 before any use
        gbidItem = GBID_NONE;  // no code: dead store, the caller sets it on 1845 before any use
        idOriginalItem = ACDID_INVALID;
        ptACDPlayer = NULL;
    }
};


static void sFindSameItemsCallback(
    ACDID idACDSquare,
    InventorySlotType,
    void * pData)
{
    FindSameItemsCallbackData * ptCallbackData = (FindSameItemsCallbackData *)pData;


    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);





    if (ptACDItem && ptACDItem->hGB.gbid == ptCallbackData->gbidItem && idACDSquare != ptCallbackData->idOriginalItem)
    {

        ptCallbackData->ptarIngredients->push_back(idACDSquare);
    }
}


void SItemCrafting_TryTransmute(
    TransmuteItemsMessage * ptMsg,
    Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)  // no row of its own: the b.eq to the epilogue at +0xa4 is the ccmp inside ACDTryToGet (line 1825)
    {
        return;
    }

    blz::vector<ACDID, blz::allocator<ACDID> > aridACDItems;
    for (int32 i = 0; i < ptMsg->nItemsCount; i++)
    {
        ACDID idACDItem = ACDFromANN(ptMsg->annItems[i]);
        aridACDItems.push_back(idACDItem);


        if (ptMsg->nStackedItemsBitField & (1 << i))
        {



            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            FindSameItemsCallbackData tCallbackData;
            tCallbackData.gbidItem = ptACDItem->hGB.gbid;
            tCallbackData.ptarIngredients = &aridACDItems;
            tCallbackData.idOriginalItem = ptACDItem->id;
            ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_BACKPACK, 4, sFindSameItemsCallback, &tCallbackData);
            ACDInventoryIterateItems(*ptACDPlayer, SLOT_PLAYER_SHARED_STASH, 4, sFindSameItemsCallback, &tCallbackData);
        }
    }


    int32 nItemsCount = aridACDItems.size();


    TransmuteInventory tTransmuteInventory;
    for (int32 i = 0; i < nItemsCount; i++)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(aridACDItems[i]);
        if (ptACDItem)  // no row of its own: the cbz/b.eq at +0x2c4/+0x30c are line 0 inside ACDTryToGet (line 1860)
        {
            TransmuteInput tInput;
            tInput.gbidCurrency = GBID_NONE;
            tInput.ptACDItem = ptACDItem;
            tInput.bUsedInRecipe = FALSE;
            tInput.eIngredientType = TRIT_INVALID;  // no row of its own: merged into line 1866's 64-bit store of 0xffffffff00000000 to [tInput+0x18]
            tInput.nStackCount = Math::Max<int32>(ptACDItem->GetStackCount(), 1);

            tTransmuteInventory.arInputs.push_back(tInput);
        }
    }

    for (int32 i = 0; i < ptMsg->nCurrenciesCount; i++)
    {
        GBID gbidItem = ptMsg->gbidCurrencies[i];
        if (gbidItem != GBID_NONE)
        {
            CurrencyType eCurrency = CurrencyFromGBID(gbidItem);

            if (eCurrency != CURRENCY_INVALID)
            {
                TransmuteInput tInput;
                tInput.gbidCurrency = gbidItem;
                tInput.ptACDItem = NULL;
                tInput.bUsedInRecipe = FALSE;
                tInput.eIngredientType = TRIT_SPECIFIC_GBID;  // no row of its own: merged into line 1886's 64-bit store of 0x100000000 to [tInput+0x18]
                tInput.nStackCount = ptACDPlayer->GetCurrencyAmount(eCurrency);

                tTransmuteInventory.arInputs.push_back(tInput);
            }
        }
    }


    GBHandleList listGBHandles(GBGetHandlePool());
    GBEnumerate(GB_TRANSMUTE_RECIPES, listGBHandles);

    POS pos = listGBHandles.GetHeadPosition();
    while (pos)
    {
        GBHandle hGB = listGBHandles.GetNext(pos);
        ScopedTransmuteRecipeEntryLockR pdefTransmuteRecipe(hGB.gbid);
        if (!pdefTransmuteRecipe)
        {
            continue;
        }


        TransmuteInventory tTransmuteInventoryFinal(tTransmuteInventory);
        if (!ItemCrafting_TransmuteEvaluate(tTransmuteInventory, pdefTransmuteRecipe->tIngredients, pdefTransmuteRecipe->nIngredientsCount, tTransmuteInventoryFinal, 0))
        {
            continue;
        }


        TransmuteResultsMessage tMessage;
        tMessage.annItem = ANN_INVALID;
        tMessage.gbidPower = GBID_NONE;  // no row of its own: merged into line 1918's 64-bit store of -1 to [tMessage+0x08]
        tMessage.eType = pdefTransmuteRecipe->eAction;
        tMessage.gbidFakeItem = GBID_NONE;
        tMessage.uFakeItemStackCount = 0;

        GameError eFailureError = GAMEERROR_TRANSMUTE_FAILED;

        if (sTransmuteExecuteAction(tMessage, eFailureError, pdefTransmuteRecipe->eAction, aridACDItems, nItemsCount, tTransmuteInventoryFinal, tPlayer, pdefTransmuteRecipe->tIngredients))
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, tPlayer);
            MessageSendToClient(listDestPlayers, SMSG_TRANSMUTE_RESULTS, &tMessage, sizeof(tMessage));

            ScriptEventOnUserEvent(EVENT_PLAYER_USED_KANAI_CUBE, tPlayer.idPlayerACD, 0);


            SNOName tVendorSNOName(ACTOR_SNO, GlobalSNOGet(GLOBALSNO_ACTOR_ZOLTUN_KULLE_ARTISAN));
            ACDID idACDVendor = ACDFindBySNOName(tVendorSNOName);
            ActorCommonData * ptACDVendor = ACDTryToGet(idACDVendor);
            if (ptACDVendor)  // no row of its own: the b.eq at +0x292c is line 0 (attributed to 1937)
            {
                SNO snoConversation = GlobalSNOGet(GLOBALSNO_CONVERSATION_ZOLTUN_KULLE_CUBE);
                ConversationMgr::StartConversation(tPlayer.nPlayerIndex, snoConversation, ptACDVendor->id, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, 0x7fffffff, FALSE, FALSE);
            }

            SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CUBE_RECIPE_USED, pdefTransmuteRecipe->tHeader.gbid, 0, 1.0f);
        }
        else
        {
            IntDataMessage tIntMessage;
            tIntMessage.nData = eFailureError;

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, tPlayer);
            MessageSendToClient(listDestPlayers, SMSG_TRANSMUTE_FAILED, &tIntMessage, sizeof(tIntMessage));
        }

        return;  // no row of its own: cleanup-destination 1 (exit) is set at 1945:9/1954:9 (orr w19, #1) and tested at +0x2544
    }


    IntDataMessage tMessage;
    tMessage.nData = GAMEERROR_TRANSMUTE_FAILED;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_TRANSMUTE_FAILED, &tMessage, sizeof(tMessage));
}




































void SItemCrafting_TryTrain(Player & tPlayer)

{

    if (!ItemCrafting_CanTrain(tPlayer))
        return;




    GBID gbidRecipe = ItemCrafting_GetTrainingRecipe(tPlayer.eActiveCrafterType, &tPlayer);

    if (gbidRecipe == GBID_NONE)
        return;
    BOOL fChangedRank = FALSE;


    if (ItemPlayerConsumeRecipeReagents(tPlayer, gbidRecipe))
    {

        fChangedRank = SCrafterIncrementLevel(tPlayer, tPlayer.eActiveCrafterType, 1);



        ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);

        int32 nCost = ItemCrafting_GetRecipeCost(gbidRecipe);
        if (pdefRecipeEntry->eCrafterType == 0 /* CRAFTER_BLACKSMITH */)
        {
            SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_LEVELING_BLACKSMITH, 0, 0, (float)nCost);
        }
        else if (pdefRecipeEntry->eCrafterType == 1 /* CRAFTER_JEWELER */)
        {
            SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_LEVELING_JEWELER, 0, 0, (float)nCost);
        }
    }
    IntDataMessage tMessage;

    tMessage.nData = fChangedRank;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, SMSG_VENDOR_TRAIN_RESULTS, &tMessage, sizeof(tMessage));
}


void SItemCrafting_LearnRecipe(const ACDID idACDPlayer, const GBID gbidRecipe)
{


    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    if (!pdefRecipeEntry)
        return;
    if (pdefRecipeEntry->eCrafterType < CRAFTER_TYPE_BLACKSMITH || pdefRecipeEntry->eCrafterType > CRAFTER_TYPE_LAST_IMPROVEABLE)
        return;



    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return; {
        CrafterData & tCrafterSavedData = ptPlayer->tCrafterSavedData.tCrafterData[pdefRecipeEntry->eCrafterType];
        for (int32 i = 0; i < tCrafterSavedData.nNumRecipes; i++)
        {
            if (tCrafterSavedData.gbidRecipes[i] == gbidRecipe)
                return;
        }




        tCrafterSavedData.gbidRecipes[tCrafterSavedData.nNumRecipes] = gbidRecipe;
        tCrafterSavedData.nNumRecipes++;

        SItemCrafting_SendPlayerInfo(*ptPlayer, pdefRecipeEntry->eCrafterType, FALSE);
    }
    if (pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT))
    {
        {




            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.SetCrafterType(pdefRecipeEntry->eCrafterType);

            tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_TEACH_RECIPE, gbidRecipe, 1, (uint32)sub_71007b5e70());




        }

        ActorCommonData * ptACDTalkingTo = ACDTryToGet(ptPlayer->tTalkData.idACDTalkingTo);
        if (ptACDTalkingTo && NPCGetCrafterType(*ptACDTalkingTo) != CRAFTER_TYPE_INVALID)
        {
            ACDID idACDEffectTarget = ptACDTalkingTo->id;
            if (!ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_IS_MAIN_CRAFTER_NPC, ptACDTalkingTo->snoActor))
            {

                XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());

                ActorIterator tIt;
                while (ActorDataArrayIterate(tIt))
                {
                    Actor * ptActor = tIt.ptElement;
                    if (!ptActor)
                        continue;
                    ActorCommonData & tACD = ACDGet(ptActor->idACD);
                    if (!tACD.IsInWorld())
                        continue;



                    if (ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_IS_MAIN_CRAFTER_NPC, tACD.snoActor) && NPCGetCrafterType(tACD) == NPCGetCrafterType(*ptACDTalkingTo))
                    {
                        idACDEffectTarget = tACD.id;
                        break;
                    }
                }
            }


            ACDPlayEffectForPlayer(idACDPlayer, idACDEffectTarget, EFFECT_CRAFTER_LEARN_RECIPE, 0);
        }
    }

}






































void SItemCrafting_SendPlayerInfo(const Player & tPlayer, const CrafterType eType, const BOOL fInitialInfo)
{


    GameMessageType eMsg = ItemCrafting_BackEnd::MessageTypeFromCrafterType(eType, fInitialInfo);
    if (eMsg == 0)
        return;




    const CrafterData & tCrafterSavedData = tPlayer.tCrafterSavedData.tCrafterData[eType];
    D3::ItemCrafting::CrafterData tData;
    tCrafterSavedData.Export(tData);
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);
    MessageSendToClient(listPlayers, eMsg, tData);
}


void SItemCrafting_SendPlayerTransmogInfo(const Player & tPlayer, const BOOL bInitialInfo)
{


    GameMessageType eMsg = bInitialInfo ? SMSG_CRAFTING_DATA_TRANSMOG_INITIAL : SMSG_CRAFTING_DATA_TRANSMOG;

    D3::GameBalance::BitPackedGbidArray tData;
    tPlayer.tCrafterSavedData.tTransmogData.Export(tData, NULL);
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listPlayers, tPlayer);
    MessageSendToClient(listPlayers, eMsg, tData);
}




BOOL SItemCrafting_GenerateItemLootSpecifierFromRecipe(const ItemSpecifierData & tSpecificItem, LootSpecifier & tSpecifier, const Player * ptPlayer, BOOL bTargetedForPlayerClass)
{




    ScopedItemDefinitionLockR pdefItem(tSpecificItem.gbidItem);
    if (pdefItem == NULL)
        return FALSE;



    SharedLooteeParams tLooteeParams;
    tLooteeParams.nLevel = pdefItem->nCrafterRequiredLevel ? pdefItem->nCrafterRequiredLevel : pdefItem->nItemLevel;
    tLooteeParams.fIsCrafter = TRUE;
    tLooteeParams.bForceCrafterAncientLegendaryChance = TRUE;


    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);
    tSpecifier.hGBItem = GBHandle(GB_ITEMS, tSpecificItem.gbidItem);
    tSpecifier.FillInTargetedClass(bTargetedForPlayerClass ? ePlayerClass : PLAYER_CLASS_NONE, NULL, FALSE);

    LootGetSpecifierForSpecifiedItem(tSpecifier, tSpecificItem, tLooteeParams, ptPlayer, TRUE);
    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    tSpecifier.fAutoIdentify = TRUE;


    return TRUE;
}

void SItemCrafting_GenerateItemGeneratorFromRecipe(D3::Items::Generator & tGenerator, const LootSpecifier & tSpecifier, const ItemSpecifierData & tSpecificItem)
{


    ItemGeneratorToDefaults(tGenerator);
    const D3::Items::Generator & tDefault = D3::Items::Generator::default_instance();


    tGenerator.mutable_gb_handle()->set_game_balance_type(GB_ITEMS);
    tGenerator.mutable_gb_handle()->set_gbid(tSpecificItem.gbidItem);
    tGenerator.set_seed(RandGetSeed(SGameGlobalsGet().rng));


    int32 nMaxAffixCount = Math::Min(tSpecifier.nNumAffixes, 6);
    for (int32 j = 0; j < nMaxAffixCount; j++)
    {
        tGenerator.add_base_affixes(tSpecifier.arAffixGBIDs[j]);
    }

    uint32 uFlags = tGenerator.flags() | (1 << IGF_IDENTIFIED_BIT) | (1 << IGF_CRAFTED_BIT);


    uFlags = (tSpecifier.eAncientRank == ANCIENT_RANK_ANCIENT) ? (uFlags | (1 << IGF_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_ANCIENT_BIT));
    uFlags = (tSpecifier.eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT) ? (uFlags | (1 << IGF_PRIMAL_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_PRIMAL_ANCIENT_BIT));
    tGenerator.set_flags(uFlags);
    tGenerator.set_stack_size(1);
    if (tSpecifier.eIQL != tDefault.item_quality_level())
    {
        tGenerator.set_item_quality_level(tSpecifier.eIQL);
    }

    if (tSpecifier.fAccountBound || ItemIsLegendary(tSpecificItem.gbidItem))
    {
        tGenerator.set_item_binding_level(IBL_ACCOUNT_PICKUP);
    }


    if (GameIsSeasonal())
    {
        tGenerator.set_season_created(GameGetCurrentSeason());
    }

    if (tSpecifier.nLegendaryItemLevel > 0 &&

        ItemIsLegendary(tSpecificItem.gbidItem))
    {

        tGenerator.set_legendary_item_level(tSpecifier.nLegendaryItemLevel);
    }

}


inline ACDID SItemCrafting_GenerateItemFromRecipe(const LootSpecifier & tSpecifier, const D3::Items::Generator & tGenerator)
{


    ACDID idACD = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, cgplaceNull, 0.0f, 0);
    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACDItem = ACDGet(idACD);
        if (tGenerator.item_quality_level() >= IQL_RARE_4_AFFIX && tGenerator.item_quality_level() <= IQL_RARE_6_AFFIX)

            LootDetermineRareName(tACDItem, tSpecifier);
    }
    return idACD;
}









ACDID SItemCrafting_GenerateItemFromRecipe(GBID gbidRecipe, const Player * ptPlayer)
{


    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    if (pdefRecipeEntry == NULL)
        return ACDID_INVALID;



    ScopedRecipeDefinitionLockR pdefRecipe(pdefRecipeEntry->snoRecipe);
    if (pdefRecipe == NULL)
        return ACDID_INVALID;



    ItemSpecifierData tSpecificItem = pdefRecipe->tSpecificItem;
    if (!tSpecificItem.IsValid())
        return ACDID_INVALID;



    LootSpecifier tSpecifier;
    if (!SItemCrafting_GenerateItemLootSpecifierFromRecipe(tSpecificItem, tSpecifier, ptPlayer, pdefRecipeEntry->nLevel))
        return ACDID_INVALID;



    D3::Items::Generator tGenerator;
    SItemCrafting_GenerateItemGeneratorFromRecipe(tGenerator, tSpecifier, tSpecificItem);

    return SItemCrafting_GenerateItemFromRecipe(tSpecifier, tGenerator);
}



ACDID SItemCrafting_TryCraftItem(Player & tPlayer, GBID gbidRecipe)


{
    if (!ItemCrafting_CanCraftItem(gbidRecipe, tPlayer.eActiveCrafterType, &tPlayer))
    {
        return ACDID_INVALID;
    }

    if (!ItemPlayerConsumeRecipeReagents(tPlayer, gbidRecipe))
    {
        return ACDID_INVALID;
    }

    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);

    int32 nCost = ItemCrafting_GetRecipeCost(gbidRecipe);
    switch (pdefRecipeEntry->eCrafterType) {
    case 0 /* CRAFTER_BLACKSMITH */:
        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_CRAFTING_BLACKSMITH, 0, 0, (float)nCost);
        break;
    case 1 /* CRAFTER_JEWELER */:

        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_GEM_COMBINING, 0, 0, (float)nCost);
        break;
    }
    ACDID idACDItem = ItemCrafting_BackEnd::CraftItem(gbidRecipe, tPlayer);
    if (idACDItem != ACDID_INVALID)
    {



        ItemCrafting_BackEnd::SendResultToClient(idACDItem, tPlayer, SMSG_CRAFTING_RESULT);






        ActorCommonData & tACDItem = ACDGet(idACDItem);

        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetACD(&tACDItem);
        tEvaluationData.SetCrafterType(tPlayer.eActiveCrafterType);

        tEvaluationData.EvaluateEvent(&tPlayer,
                                      Achievement::ACH_EVENT_CRAFT_ITEM,
                                      tACDItem.hGB.gbid,
                                      1, (uint32)sub_71007b5e70());

    }


    return idACDItem;
}

BOOL SItemCrafting_TryRepair(ACDID idACDItem, Player & tPlayer)
{


    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
        return FALSE;



    if (!ItemCanBeRepaired(*ptACDItem))
        return FALSE;



    int32 nCost = ItemCrafting_GetRepairCost(idACDItem, tPlayer.idPlayerACD);
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!tACDPlayer.CanAffordCurrencyCost(nCost, CURRENCY_GOLD))
    {
        nCost = (int32)tACDPlayer.GetCurrencyAmount(CURRENCY_GOLD);
    }

    if (ptACDItem->GetInventoryLocation().idACDContainer != tACDPlayer.id)
        return FALSE;



    if (!ItemCrafting_BackEnd::SpendGold(tACDPlayer, nCost))
        return FALSE;



    SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_CURRENCY_SPENT_REPAIRING, 0, 0, (float)nCost);






    {   Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetACD(ptACDItem);

        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_REPAIR_ITEM,


                                      ItemGetQualityLevel(*ptACDItem), (uint32)sub_71007b5e70());
    }

    ItemModifyDurability(tACDPlayer, *ptACDItem, ptACDItem->AttributesGetInt(ATTRIBUTE_DURABILITY_MAX));


    return TRUE;
}

void SItemCrafting_TryRepairAll(Player & tPlayer, const BOOL fEquippedOnly)
{


    int32 nCost = ItemCrafting_GetRepairAllCost(&tPlayer, fEquippedOnly);
    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!tACDPlayer.CanAffordCurrencyCost(nCost, CURRENCY_GOLD))
    {
        nCost = (int32)tACDPlayer.GetCurrencyAmount(CURRENCY_GOLD);
    }
























    sgfRepairSuccess = FALSE;

    if (fEquippedOnly)
    {

        for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; i++) ACDInventoryIterateItems(tACDPlayer, i, 0, ItemCrafting_BackEnd::RepairItem, &tPlayer);

    }
    else
    {

        for (int32 nIndex = 0; cgtDefaultPlayerInventorySlots[nIndex] != SLOT_NONE; nIndex++)
        {

            ACDInventoryIterateItems(tACDPlayer, cgtDefaultPlayerInventorySlots[nIndex], 0, ItemCrafting_BackEnd::RepairItem, &tPlayer);
        }
    }

    if (sgfRepairSuccess == TRUE)
    {

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);
        SimpleMessage tMessage;
        MessageSendToClient(listDestPlayers, SMSG_REPAIR_ACK, &tMessage, sizeof(tMessage));
    }
}

void SItemCrafting_Reforge(ACDID idACDItem, Player & tPlayer)


{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
        return;



    if (ItemCanBeReforged(*ptACDItem) != IRE_NONE)
        return;



    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!ItemPlayerConsumeReforgeReagents(tACDPlayer, *ptACDItem))
        return;



    ACDID idACDReforgedItem = SItemReforge(*ptACDItem, tPlayer, FALSE, FALSE);
    if (idACDReforgedItem == ACDID_INVALID)
        return;
    ANNDataMessage tMessage;



    tMessage.ann = ANNFromACD(idACDReforgedItem);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, SMSG_REFORGE_RESULTS, &tMessage, sizeof(tMessage));
}

GBID ItemGetEnchantedAffixOld(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:5289



void SItemCrafting_EnchantItemAffixStart(
    ACDID idACDItem,
    GBID gbidAffixToReroll,
    Player & tPlayer)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem || !ptACDItem->IsItem())
    {
        return;
    }


    if (ItemCanEnchantAffix(*ptACDItem) != IEAE_NONE)
    {
        return;
    }

    // no row of its own: its cmn/b.eq carry 2571:9 (merged ahead of the ItemCanEnchantAffix result test)
    if (gbidAffixToReroll == GBID_NONE)
    {
        return;
    }


    if (ItemIsAffixInherent(*ptACDItem, gbidAffixToReroll, TRUE))
    {
        return;
    }


    if (ItemHasEnchantedAffix(*ptACDItem) && ItemGetEnchantedAffixOld(*ptACDItem) != gbidAffixToReroll)
    {
        return;
    }


    int32 nLegendaryItemLevel = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);


    LootSpecifier tLootSpecifier;
    tLootSpecifier.hGBItem = ptACDItem->hGB;
    tLootSpecifier.tLooteeParams.nLevel = nLegendaryItemLevel > 0 ? nLegendaryItemLevel : ItemGetItemLevel(ptACDItem->hGB.gbid);
    tLootSpecifier.nAffixLevel = tLootSpecifier.tLooteeParams.nLevel;
    tLootSpecifier.eIQL = ItemGetQualityLevel(*ptACDItem);
    tLootSpecifier.eAncientRank = (AncientRank)ptACDItem->AttributesGetInt(ATTRIBUTE_ANCIENT_RANK);
    if (tLootSpecifier.eIQL == IQL_LEGENDARY)
    {
        tLootSpecifier.nLegendaryItemLevel = tLootSpecifier.nAffixLevel;
    }


    RNG tRNGRandomClass;
    RandInit(tRNGRandomClass);
    if (ItemHasEnchantedAffix(*ptACDItem) &&
        ptACDItem->AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_SEED) != 0)
    {
        tRNGRandomClass.dwSeed = ptACDItem->AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_SEED);
    }
    else
    {
        tRNGRandomClass.dwSeed = ptACDItem->AttributesGetInt(ATTRIBUTE_SEED);
    }

    PlayerClass eClass = PlayerGetPlayerClass(&tPlayer);
    tLootSpecifier.FillInTargetedClass(eClass, &tRNGRandomClass, FALSE);



    BOOL bAffixValid = FALSE;
    POS pos = ptACDItem->listIdentifiedAffixGBIDs.GetHeadPosition();
    while (pos)
    {
        GBID gbidAffix = ptACDItem->listIdentifiedAffixGBIDs.GetNext(pos);
        if (gbidAffix == gbidAffixToReroll)
        {
            bAffixValid = TRUE;
        }
        else
        {
            tLootSpecifier.AddAffix(gbidAffix);
        }
    }

    if (!bAffixValid)
    {
        return;
    }





    ScopedItemAffixLockR pdefAffix(gbidAffixToReroll);
    ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
    if (pdefItem)
    {
        for (int32 i = 0; i < MAX_LEGENDARY_AFFIX_FAMILIES; i++)
        {
            GBID gbidLegendaryAffixFamily = pdefItem->gbidLegendaryAffixFamily[i];
            if (gbidLegendaryAffixFamily != GBID_NONE &&
                pdefItem->bUseLegendaryAllowedTypes[i] &&
                (pdefAffix->gbidAffixFamily0 == gbidLegendaryAffixFamily ||
                pdefAffix->gbidAffixFamily1 == gbidLegendaryAffixFamily))
            {
                tLootSpecifier.gbidAllowedLegendaryAffixFamily = gbidLegendaryAffixFamily;
                break;
            }
        }
    }

    EnchantAffixChooseMessage tMessage;
    tMessage.annItem = ANNFromACD(idACDItem);
    int32 & nCount = tMessage.nEnchantAffixChoicesCount;
    // no row: dead store (the increment below stores 1); the choice-0 store goes to [sp+0xc4], index 0
    nCount = 0;

    tMessage.tEnchantAffixChoices[nCount].gbidAffix = GBID_NONE;
    tMessage.tEnchantAffixChoices[nCount].dwSeed = 0;
    ++nCount;


    uint32 dwFlags = 0;
    if (pdefAffix->eAffixGroup == AFFIXGROUP_MAJOR)
    {
        dwFlags |= (1 << VAF_PICK_MAJOR_AFFIXES);
    }
    else if (pdefAffix->eAffixGroup == AFFIXGROUP_MINOR)
    {
        dwFlags |= (1 << VAF_PICK_MINOR_AFFIXES);
    }

    if (ptACDItem->AttributesGetInt(ATTRIBUTE_CONSUMABLE_ADD_SOCKETS))
    {
        dwFlags |= (1 << VAF_CANT_ADD_SOCKETS);
    }

    GBHandleList lstReplacementAffixes(GBGetHandlePool());
    int32 nReplacementAffixesMaxWeight = LootBuildValidAffixList(lstReplacementAffixes, tLootSpecifier, -1, dwFlags);


    if (nReplacementAffixesMaxWeight <= 0)
    {
        return;
    }


    BOOL bAddOriginalAffixToList = FALSE;
    int32 nOriginalAffixFrequencyWeight = LootGetAffixFrequencyWeight(pdefAffix, &tLootSpecifier);
    if (nOriginalAffixFrequencyWeight > 0 &&
        (!(dwFlags & (1 << VAF_CANT_ADD_SOCKETS)) || !ItemAffixAddsSockets(pdefAffix->tHeader.gbid)))
    {
        bAddOriginalAffixToList = TRUE;
        POS scopedPos = lstReplacementAffixes.GetHeadPosition();
        while (scopedPos)
        {
            GBHandle hGBAffix = lstReplacementAffixes.GetNext(scopedPos);
            if (hGBAffix.gbid == gbidAffixToReroll)
            {
                bAddOriginalAffixToList = FALSE;
                break;
            }
        }

        if (bAddOriginalAffixToList)
        {
            GBHandle hGBOriginalAffix(GB_AFFIXES, gbidAffixToReroll);
            lstReplacementAffixes.AddTail_Tracked(hGBOriginalAffix);
            nReplacementAffixesMaxWeight += nOriginalAffixFrequencyWeight;
        }
    }


    ConsoleScopedOverrideRNG overrideRNG(ptACDItem->AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_COUNT) + (ItemGetEnchantedAffixNew(*ptACDItem) != GBID_NONE ? ItemGetEnchantedAffixNew(*ptACDItem) : ptACDItem->AttributesGetInt(ATTRIBUTE_SEED)), 666, RNGOutOfScopeCallback());

    int32 nRandomStartCount = nCount;
    while (nCount < 3)
    {


        int32 nUniqueReplacementAffixesMaxWeight = 0;
        GBHandleList lstUniqueReplacementAffixes(GBGetHandlePool());
        if (nCount > nRandomStartCount)
        {
            POS scopedPos = lstReplacementAffixes.GetHeadPosition();
            while (scopedPos)
            {
                GBHandle hGBAffix = lstReplacementAffixes.GetNext(scopedPos);
                ScopedItemAffixLockR pdefReplacementAffix(hGBAffix.gbid);

                BOOL bUniqueAffix = TRUE;
                for (int32 i = nRandomStartCount; i < nCount; ++i)
                {
                    ScopedItemAffixLockR pdefExistingAffix(tMessage.tEnchantAffixChoices[i].gbidAffix);
                    if (pdefExistingAffix->gbidAffixFamily0 == pdefReplacementAffix->gbidAffixFamily0 &&
                        pdefExistingAffix->gbidAffixFamily1 == pdefReplacementAffix->gbidAffixFamily1)
                    {
                        bUniqueAffix = FALSE;
                        break;
                    }
                }

                if (bUniqueAffix)
                {
                    lstUniqueReplacementAffixes.AddTail_Tracked(hGBAffix);

                    int32 nFrequencyWeight = LootGetAffixFrequencyWeight(pdefReplacementAffix, &tLootSpecifier);

                    nUniqueReplacementAffixesMaxWeight += nFrequencyWeight;
                }
            }
        }


        AffixType eLastAffixType = -1;
        GBID gbidReplacementAffix;
        if (nUniqueReplacementAffixesMaxWeight > 0)
        {
            gbidReplacementAffix = LootChooseAffixFromList(tLootSpecifier, eLastAffixType, lstUniqueReplacementAffixes, nUniqueReplacementAffixesMaxWeight);
        }
        else
        {
            gbidReplacementAffix = LootChooseAffixFromList(tLootSpecifier, eLastAffixType, lstReplacementAffixes, nReplacementAffixesMaxWeight);
        }

        if (gbidReplacementAffix == GBID_NONE)
        {
            break;
        }


        if (bAddOriginalAffixToList &&
            tLootSpecifier.eIQL == IQL_LEGENDARY &&
            pdefAffix->gbidLegendaryUprankAffix == gbidReplacementAffix)
        {
            gbidReplacementAffix = gbidAffixToReroll;
        }


        tMessage.tEnchantAffixChoices[nCount].gbidAffix = gbidReplacementAffix;
        tMessage.tEnchantAffixChoices[nCount].dwSeed = RandGetDWord(SGameGlobalsGet().rng);
        ++nCount;
    }


    if (nCount < 2)
    {
        return;
    }



    ActorCommonData &tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!ItemPlayerConsumeEnchantAffixReagents(tACDPlayer, *ptACDItem))
    {
        return;
    }




    if (!ItemHasEnchantedAffix(*ptACDItem))
    {
        ptACDItem->AttributesSetInt(ATTRIBUTE_ENCHANTED_AFFIX_OLD, gbidAffixToReroll);
        ptACDItem->AttributesSetInt(ATTRIBUTE_ENCHANTED_AFFIX_NEW, gbidAffixToReroll);
        ptACDItem->AttributesSetInt(ATTRIBUTE_ENCHANTED_AFFIX_SEED, 0);
        ptACDItem->AttributesSetInt(ATTRIBUTE_ENCHANTED_AFFIX_COUNT, 1);
        ItemSetBoundToACD(*ptACDItem, tPlayer.idPlayerACD);
    }
    else
    {

        int32 nNewEnchantedAffixCount = ptACDItem->AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_COUNT) + 1;
        ptACDItem->AttributesSetInt(ATTRIBUTE_ENCHANTED_AFFIX_COUNT, nNewEnchantedAffixCount);
    }


    SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_ENCHANT_START, ptACDItem->GetLocationSlot(), ptACDItem->GetLocationSlot());


    tPlayer.tVendorData.idACDEnchantAffixPendingItem = idACDItem;
    tPlayer.tVendorData.gbidEnchantAffixPendingRemoved = gbidAffixToReroll;
    SigmaMemoryCopy(tPlayer.tVendorData.tEnchantAffixChoices, tMessage.tEnchantAffixChoices, sizeof(tMessage.tEnchantAffixChoices));
    tPlayer.tVendorData.nEnchantAffixChoicesCount = tMessage.nEnchantAffixChoicesCount;


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, SMSG_CHOOSE_ENCHANT_AFFIX, &tMessage, sizeof(tMessage));


    tPlayer.tHeroStateData.uPlayerFlags &= ~(1u << PLAYERFLAG_FREE_ENCHANT);
    tACDPlayer.MarkHeroStatePlayerFlagsChanged();
}


void SItemCrafting_EnchantItemAffixChoose(
    ACDID idACDItem,
    int32 nAffixChoice,
    Player & tPlayer)
{

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem || !ptACDItem->IsItem())
    {
        return;
    }

    if (idACDItem != tPlayer.tVendorData.idACDEnchantAffixPendingItem)
    {
        return;
    }


    BOOL bValidReplacedAffix = FALSE;
    GBID gbidRemovedBaseAffix = tPlayer.tVendorData.gbidEnchantAffixPendingRemoved;
    POS pos = ptACDItem->listIdentifiedAffixGBIDs.GetHeadPosition();
    while (pos)
    {
        GBID gbidAffix = ptACDItem->listIdentifiedAffixGBIDs.GetNext(pos);
        if (gbidAffix == gbidRemovedBaseAffix)
        {
            bValidReplacedAffix = TRUE;
            break;
        }
    }

    if (!bValidReplacedAffix)
    {
        return;
    }


    if (nAffixChoice < 0 || nAffixChoice >= tPlayer.tVendorData.nEnchantAffixChoicesCount)
    {
        return;
    }

    GBID gbidAffixChoice = tPlayer.tVendorData.tEnchantAffixChoices[nAffixChoice].gbidAffix;
    uint32 dwAffixSeed = tPlayer.tVendorData.tEnchantAffixChoices[nAffixChoice].dwSeed;

    // no row: SItemPlayerReplace's result or, without a new affix, the item itself (Ghidra: ANNFromACD(idACDItem) on that path)
    ACDID idACDNewItem = idACDItem;
    if (gbidAffixChoice != GBID_NONE)
    {

        int32 nSocketsFilled = ItemGetNumFilledSockets(*ptACDItem);
        if (nSocketsFilled > 0)
        {




            ActorCommonData &tACDPlayer = ACDGet(tPlayer.idPlayerACD);

            GBID gbidAffixToBeReplaced = gbidRemovedBaseAffix;
            if (ItemHasEnchantedAffix(*ptACDItem) &&
                gbidRemovedBaseAffix == ItemGetEnchantedAffixOld(*ptACDItem))
            {
                gbidAffixToBeReplaced = ItemGetEnchantedAffixNew(*ptACDItem);
            }

            int32 nSocketsDelta = ItemAffixGetNumSockets(ptACDItem->hGB.gbid, gbidAffixChoice) -
                                  ItemAffixGetNumSockets(ptACDItem->hGB.gbid, gbidAffixToBeReplaced);

            int32 nSocketsTotal = ptACDItem->AttributesGetInt(ATTRIBUTE_SOCKETS);
            int32 nNewSocketsCount = nSocketsDelta + nSocketsTotal;

            int32 nUnsocketCost = ItemCrafting_GetGemRemovalCostForNewSocketCount(*ptACDItem, nNewSocketsCount);
            CurrencyType eCurrencyType = CURRENCY_GOLD;
            if (nUnsocketCost > 0 &&
                (!tACDPlayer.CanAffordCurrencyCost(nUnsocketCost, eCurrencyType) ||
                !tACDPlayer.ModifyCurrencyAmount(-nUnsocketCost, eCurrencyType, GMR_CRAFTING, FALSE)))
            {
                return;
            }
        }

        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_ENCHANTED_ITEM, ptACDItem->hGB.gbid, ItemGetStatsDataKey(*ptACDItem), 1.0f);
        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_ENCHANTED_ITEM_AFFIX_REMOVED, ptACDItem->hGB.gbid, gbidRemovedBaseAffix, 1.0f);
        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_ENCHANTED_ITEM_AFFIX_ADDED, ptACDItem->hGB.gbid, gbidAffixChoice, 1.0f);


        D3::Items::Generator tGenerator;
        ItemGeneratorPopulateFromACDID(*ptACDItem, tGenerator, TRUE);

        tGenerator.set_enchanted_affix_old(gbidRemovedBaseAffix);
        tGenerator.set_enchanted_affix_new(gbidAffixChoice);
        tGenerator.set_enchanted_affix_seed(dwAffixSeed);
        tGenerator.set_enchanted_affix_count(ptACDItem->AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_COUNT));
        tGenerator.set_item_binding_level(IBL_ACCOUNT_PICKUP);
        tGenerator.set_max_durability(ptACDItem->AttributesGetInt(ATTRIBUTE_DURABILITY_MAX));
        tGenerator.set_durability(ptACDItem->AttributesGetInt(ATTRIBUTE_DURABILITY_CUR));


        idACDNewItem = SItemPlayerReplace(*ptACDItem, tGenerator, tPlayer, TRUE);
        if (idACDNewItem == ACDID_INVALID)
        {
            return;
        }
    }


    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(idACDNewItem);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, SMSG_ENCHANT_AFFIX_RESULTS, &tMessage, sizeof(tMessage));



    {

        ActorCommonData &tACDNewItem = ACDGet(idACDNewItem);


        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetACD(&tACDNewItem);
        tEvaluationData.SetEquipmentInfo(tPlayer.idPlayerACD);

        tEvaluationData.EvaluateEvent(
            &tPlayer,
            Achievement::ACH_EVENT_ENCHANT_ITEM,
            ItemGetQualityLevel(tACDNewItem),
            1, (uint32)sub_71007b5e70());


        if(tEvaluationData.GetEquipmentInfo()->IsAllEquipmentEnchanted())
        {
            tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_EQUIP_ALL_ITEMS_ENCHANTED, 1, (uint32)sub_71007b5e70());
        }




        if (gbidAffixChoice != GBID_NONE && ItemAffixGetNumSockets(ptACDItem->hGB.gbid, gbidAffixChoice) > 0)
        {
            tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_REROLL_AFFIX_TO_GET_SOCKET, 1, (uint32)sub_71007b5e70());
        }



    }


    tPlayer.tVendorData.idACDEnchantAffixPendingItem = ACDID_INVALID;
    tPlayer.tVendorData.gbidEnchantAffixPendingRemoved = GBID_NONE;
    tPlayer.tVendorData.nEnchantAffixChoicesCount = 0;
}
struct CrafterLevelUpMessage { RequiredMessageHeader tHeader; CrafterType eType; uint32 dwAnimTag; uint32 dwNewIdle; int32 nLevel; };  // size 0x18; X/Common/Crafting/ItemCrafting.h:50 (not in the adopted subset there)
static_assert(sizeof(CrafterLevelUpMessage) == 0x18, "CrafterLevelUpMessage");
void SItemCrafting_InitSavedData(Player &)
{


}

void sCrafterOnLevelUp(const Player & tPlayer, CrafterType eType, const BOOL fChangedRank, const BOOL fSendToClient)




{
    int32 nCrafterLevel = GetCraftLevel(eType, tPlayer);
    if (nCrafterLevel < 0)
        return;





    SItemCrafting_SendPlayerInfo(tPlayer, eType, FALSE);

    uint32 nRank = CrafterGetRank(eType, nCrafterLevel);


    if (fSendToClient)
    {
        CrafterLevelUpMessage tMessage;
        tMessage.eType = eType;

        tMessage.dwAnimTag = CrafterGetLevelUpAnim(nRank);
        tMessage.dwNewIdle = CrafterGetIdleAnim(nRank);
        tMessage.nLevel = nCrafterLevel;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
        MessageSendToClient(listDestPlayers, SMSG_VENDOR_LEVEL_UP, &tMessage, sizeof(tMessage));
    }

    if (fChangedRank)
    {





        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetCrafterType(eType);

        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_LEVELUP_CRAFTER, nRank, 1, (uint32)sub_71007b5e70());




    }

}


BOOL SCrafterIncrementLevel(Player & tPlayer, CrafterType eType, uint32 nLevels)
{


    if (nLevels == 0)
        return FALSE;




    int32 nCurrentLevel = GetCraftLevel(eType, tPlayer);
    int32 nMaxLevel = CrafterGetMaxLevel(eType);
    if (nCurrentLevel < nMaxLevel)
    {
        int32 nNewLevel = (nCurrentLevel + (int32)nLevels < nMaxLevel) ? nCurrentLevel + (int32)nLevels : nMaxLevel;
        uint32 nTargetCrafterRank = CrafterGetRank(eType, nNewLevel);
        BOOL fChangedRank = nTargetCrafterRank != CrafterGetRank(eType, nCurrentLevel);

        BOOL fSendToClient = fChangedRank;


        for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer != NULL; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
        {
            if (PlayersAreLocalToEachOther(tPlayer, *ptOtherPlayer))
            {
                uint32 otherCrafterLevel = CrafterGetRank(eType, GetCraftLevel(eType, *ptOtherPlayer));
                if (otherCrafterLevel >= nTargetCrafterRank)
                {
                    fSendToClient = FALSE;
                    break;
                }
            }
        }

        SetCraftLevel(eType, tPlayer, nNewLevel);
        sCrafterOnLevelUp(tPlayer, eType, fChangedRank, fSendToClient);
        return fChangedRank;
    }



    return FALSE;
}

void SCrafterUnlock(Player & tPlayer, const CrafterType eType)
{


    int32 nCurrentLevel = GetCraftLevel(eType, tPlayer);
    if (nCurrentLevel == 0)
    {
        SCrafterIncrementLevel(tPlayer, eType, 1);







        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.SetCrafterType(eType);

        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_ACQUIRE_CRAFTER, eType, 1, (uint32)sub_71007b5e70());
    }














}





void SItemCrafting_UpgradeJewel(Player & tPlayer, ActorCommonData & tACDItem)


{

    if (!ItemIsUpgradeableJewel(tACDItem))
        return;




    ActorCommonData * ptACDVendor = ACDTryToGet(tPlayer.idACDActiveVendor);
    if (!ptACDVendor)
        return;



    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return;




    if (!ACDIsInTieredLootRun(*ptACDVendor))
        return;








    int32 nJewelUpgradesUsed = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_USED);
    int32 nJewelUpgradesBonus = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_BONUS);
    int32 nJewelUpgradesMax = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_MAX);
    int32 nUpgradeRanks = 0;
    if (nJewelUpgradesUsed < nJewelUpgradesMax)
    {













        int32 nTieredLootRunLevel = ACDGetTieredLootRunLevel(*ptACDVendor);
        float flUpgradeChance = ItemCrafting_GetJewelUpgradeChance(tACDItem, nTieredLootRunLevel);
        if (flUpgradeChance >= EPSILON)
        {
            float flRandChance = RandFloat_Internal(SGameGlobalsGet().rng);
            nUpgradeRanks = (flRandChance <= flUpgradeChance) ? 1 : 0;
        }

        SGameStatsPlayerCounterIncrement(&tPlayer, STAT_COUNTER_PLAYER_ITEM_LEGENDARY_GEM_UPGRADED, tACDItem.hGB.gbid, tACDItem.AttributesGetInt(ATTRIBUTE_JEWEL_RANK), (float)nUpgradeRanks);


        SItemUpgradeJewel(tACDItem, nUpgradeRanks, *ptACDPlayer);

        if (nJewelUpgradesBonus > 0)
        {
            NephalemRiftManager::PlayerSetJewelBonusUpgradesLeft(&tPlayer, nJewelUpgradesBonus - 1);
        }
        else
        {
            NephalemRiftManager::PlayerSetJewelUpgradesUsed(&tPlayer, nJewelUpgradesUsed + 1);
        }

        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_LEGENDARY_GEM_UPGRADED, 0, 1, (uint32)sub_71007b5e70());



    }

    if (!NephalemRiftManager::PlayersAreStillChoosingRewards(NULL))
    {
        NephalemRiftManager::SendPlayersDoneChoosingRewardsEvent(tPlayer.idPlayerACD);
    }


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    JewelUpgradeResultsMessage tMessage;

    tMessage.ann = ANNFromACD(tACDItem.id);
    tMessage.nNumRanksGained = nUpgradeRanks;
    MessageSendToClient(listDestPlayers, SMSG_UPGRADE_JEWEL_RESULTS, &tMessage, sizeof(tMessage));
}

extern "C" ACDID sub_7100887500(ActorCommonData & tACDItem, Player & tPlayer, BOOL bUnk3, BOOL bForcePrimal)
{
    ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
    ConsoleScopedOverrideRNG overrideRNG(tACDItem.AttributesGetInt(ATTRIBUTE_SEED), 666, RNGOutOfScopeCallback());
    return SItemReforge(tACDItem, tPlayer, bUnk3, bForcePrimal);
}
