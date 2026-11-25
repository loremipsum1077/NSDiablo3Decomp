// Original file: X/Server/SItems.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "Loot/Loot.h"
#include "../Common/ItemsSwig.h"
#include "../Common/Game.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorInventory.h"
#include "../Common/Items.h"
#include "../Common/Player.h"
#include "../Common/GameCommonData.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Crafting/ItemCrafting.h"
#include "Crafting/SItemCrafting.h"
#include "Loot/Loot.h"
#include "Actors/Actors.h"
#include "SGame.h"
#include "Timers.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Definitions/StringListDefinition.h"
#include "../Sigma/LanguageRules.h"
#include "../Sigma/Random.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include <float.h>
#include "../Common/Protobuf/GameBalance.pb.h"
#include "../Common/Achievements/AchievementDefines.h"
#include "PlayerList.h"
#include "../Common/VisualInventory.h"
#include "../Common/Networking/GameMessage.h"
typedef int32 FlippyID; void FlippyDestroy(FlippyID idFlippy); void ACDInventoryRemoveToLimbo(const ACDID idACDContainer, ActorCommonData & tACDItem, BOOL bRemoveFromKnownList); BOOL ItemRemoveSocketedItemAttributes(ActorCommonData & tACDItemToSocket, const ActorCommonData & tACDItemSocketedInto); ACDID ActorInventoryGetUltimateOwner(const ActorCommonData * ptACDItem); uint32 ItemGetTradeTimeLeft(const ActorCommonData & tACDItem);
BOOL ItemIsGlyph(const ActorCommonData & tACDItem); void ItemSetBindingLevel(ActorCommonData & tACD, const ItemBindingLevel eBindingType); void ItemSetBoundToACD(ActorCommonData & tACD, const ACDID idACDOwner); uint32 TimerGetTime(const TimerID idTimer); void ItemGeneratorPopulateFromACDID(const ActorCommonData & tACDItem, D3::Items::Generator & tGenerator, BOOL bSetRestoredBit); void VisualInventoryResetSlot(const ACDID idACD, const InventorySlotType eInventorySlot);
ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, LootSpecifier & tSpecifier, const int32 nInherentAffixes); ACDID SItemPlayerReplace(ActorCommonData & tACDOldItem, D3::Items::Generator & tNewItemGenerator, Player & tPlayer, BOOL bResocketGems); void SItemRegisterTradingTimer(ActorCommonData & tACDItem, uint32 uUpdateTicks); void sGlyphAutoPickup(TimerID, DataID idOwner, void *); ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, GBHandle hGBItem, const GBID * ptGBIDAffixes, const int32 nAffixCount, const int32 nBonusRandomAffixes);
void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eItemEffectType, int32 & nEffectLevel, const BOOL bWantOverrides, const BOOL bForTooltip); void VisualInventorySetItemInSlot(const ACDID idACD, const VisualInventorySlot eSlot, const GBID gbidItem, const DyeType eDyeType, const ItemEffectType eItemEffectType, const int32 nEffectLevel, const BOOL bItemIsBrokenOrDisabled);
BOOL ItemIsBroken(const ActorCommonData & tACDItem); BOOL ItemCanExtractLegendaryPower(GBID gbidItem); BOOL ItemPlayerHasExtractedLegendaryPower(const Player & tPlayer, GBID gbidItem); void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);
void ItemsAddItemAffixGBID(ActorCommonData & tACD, GBID gbidAffix, uint32 dwFlags, ItemQualityLevel eIQL, BOOL bIsReplacementAffix, ItemAffixFastAttribMap * arAffixSpecificAttributes); void ItemAddBaseAttributesForItem(ActorCommonData & tACDItem, GBHandle hGBItem, BOOL bIsCrafted, BOOL bSimulateOnly, BOOL bAddJewelUpgrades);

BOOL SItemPlayerIdentify(ActorCommonData & tACDItem, ActorCommonData * ptACDPlayer)
{
    BOOL fUnidentified = ItemIsUnidentified(tACDItem);
    if (fUnidentified)
    {
        Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);

        RNG tItemRNG;
        RandInit(tItemRNG);
        RandSetSeed_Internal(tItemRNG, ptActor->dwItemIdentifySeed);
        tACDItem.ptRNG = &tItemRNG;

        ItemQualityLevel eIQL = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL_IDENTIFIED);
        if (eIQL != -1)
        {
            tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL, eIQL);
        }
        else
        {
            eIQL = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL);
        }

        ItemAddBaseAttributesForItem(tACDItem, tACDItem.hGB, FALSE, TRUE, TRUE);

        if (tACDItem.listUnidentifiedAffixGBIDs.Count() > 0)
        {
            while (tACDItem.listUnidentifiedAffixGBIDs.GetTailPosition())
            {
                GBID gbidAffix = tACDItem.listUnidentifiedAffixGBIDs.GetTail();
                ItemsAddItemAffixGBID(tACDItem, gbidAffix, 1, eIQL, FALSE, NULL);
                tACDItem.listUnidentifiedAffixGBIDs.RemoveTail();
            }
            tACDItem.ModifiedMark(3);
        }

        if (ptACDPlayer)
        {
            SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_IDENTIFY, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());
        }

        tACDItem.ptRNG = NULL;
        tACDItem.AttributesSetInt(ATTRIBUTE_SEED, ptActor->dwItemInitSeed);
        tACDItem.AttributesSetInt(ATTRIBUTE_UNIDENTIFIED, FALSE);

        if (eIQL >= 6 && eIQL <= 8)
        {
            tACDItem.ModifiedMark(6);
        }

        Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
        if (ptPlayer)
        {
            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(tACDItem.id);
            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_IDENTIFY_NOTIFY, &tMessage, sizeof(tMessage));
        }

        SItemCrafting_TryUnlockTransmog(tACDItem.hGB.gbid, *ptPlayer, TRUE, TRUE);
        return TRUE;
    }

    return FALSE;
}


enum { ACTOR_SPHERE_TREE_FLAGS_ALL = 7 }; enum { GB_ITEMTYPES = 1 };  // X/Common/Actors/ActorCollision.h



struct ItemPlayerReplaceUnsocketData {  // size 0x10
    ACDID idGems[3];  // 0x00
    int32 nGemsCount;  // 0x0c
};















static void sDestroyItem(ActorCommonData * ptACDItem)
{
    FlippyID idFlippy = ptACDItem->AttributesGetInt(ATTRIBUTE_FLIPPY_ID);
    if (idFlippy != -1)
    {
        FlippyDestroy(idFlippy);
    }

    const InventoryLocation * ptInvLocation = ptACDItem->TryToGetInventoryLocation();
    if (ptInvLocation && ptInvLocation->idACDContainer != ACDID_INVALID)
    {
        ActorCommonData * ptACDContainer = ACDTryToGet(ptInvLocation->idACDContainer);
        if (ptACDContainer && ptACDContainer->IsItem())
        {
            ptACDItem = ptACDContainer;
        }
    }

    ActorDelete((ActorID)ptACDItem->idOwner);
}



void SItemsDestroyLockedOwned(ActorCommonData & tACDPlayer)
{
    if (SGameIsShuttingDown())
    {
        return;
    }

    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;
        if (ptACD->IsItem() && ItemGetBoundToACD(*ptACD, TRUE) == tACDPlayer.id)
        {
            sDestroyItem(ptACD);
        }
    }
}



BOOL ItemsValidateGeneratorParams(const D3::Items::Generator & tGenerator);  // X/Common/Items.cpp:6349
uint32 ItemSocketCountFromGenerator(const D3::Items::Generator & tGenerator);  // X/Common/Items.cpp:7820
BOOL ItemGenerateCommon(ActorCommonData & tACD, const D3::Items::Generator & tGenerator, ItemAffixFastAttribMap * arAffixSpecificAttributes);  // X/Common/Items.cpp:6942
BOOL WorldPlaceEqual(const WorldPlace & tPlace1, const WorldPlace & tPlace2);  // X/Common/WorldPlace.cpp:114
void ServerACDGarbageCollectOldestItemOnGround();  // X/Server/Network/Server.cpp:2029
struct SceneCommonData;  // X/Common/SceneCommon.h
SceneCommonData * SceneCommonDataTryToGet(SSceneID idSScene);  // X/Common/SceneCommon.h
SSceneID SceneCommonGetSceneIDFromWorldPlace(const WorldPlace & tPlace, BOOL bIncludeInactiveScenes);  // X/Common/SceneCommon.h
BOOL SceneCommonIsActivated(const SSceneID idSScene);  // X/Common/SceneCommon.cpp:2689




static ACDID sCreateItemActor(
	const OnlineService::ItemId & tItemId,
	const D3::Items::Generator & tGenerator,
	const WorldPlace & tPlace,
	const float flScale,
	uint32 tName)
{


	ActorAddStruct tActorAdd;
	tActorAdd.tPlace = tPlace;
	tActorAdd.hGB.Import(tGenerator.gb_handle());
	tActorAdd.flScale = flScale;
	tActorAdd.tMarkersName = tName;

	tActorAdd.snoActor = GameBalanceGetActorSNO(tActorAdd.hGB);
	if (!SNOFileExists(ACTOR_SNO, tActorAdd.snoActor))
	{
		return ACDID_INVALID;
	}


	uint32 uSocketCount = ItemSocketCountFromGenerator(tGenerator);
	tActorAdd.dwActorAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK);
	if (uSocketCount > 0) tActorAdd.dwActorAddFlags |= (1 << ACTORADD_FORCE_INVENTORY_CREATE); else tActorAdd.dwActorAddFlags &= ~(1 << ACTORADD_FORCE_INVENTORY_CREATE);  // inferred: lines 213/214 rows all at column 2 (likely a bit macro)

	tActorAdd.qOrientation = cgqIdentity;

	{
		Angle aYaw = RandAngle(SGameGlobalsGet().rng);
		tActorAdd.qOrientation.FromYaw(aYaw);
	}

	tActorAdd.eFastAttribType = FAT_ITEM;


	// inferred: two ActorAdd attempts (unrolled in the original), garbage-collecting a ground item after each failure
	ActorID idActor = ACTORID_INVALID;
	int32 nTries = 0;
	while (nTries < 2)
	{
		nTries++;
		idActor = ActorAdd(tActorAdd);



		if (idActor == ACTORID_INVALID)
		{
			ServerACDGarbageCollectOldestItemOnGround();
		}
		else
		{
			break;
		}
	}


	// inferred: no instruction of its own (folded into the loop exit)
	if (idActor == ACTORID_INVALID)
	{
		return ACDID_INVALID;
	}



	Actor *ptActor = ActorGet(idActor);
	ActorCommonData &tACD = ACDGet(ptActor->idACD);





    D3::OnlineService::ItemId tD3ItemId;
    ItemComputeConsoleItemId(tACD, tD3ItemId);
    ptActor->tItemId.Import(tD3ItemId);




	return tACD.id;
}



void sGlyphAutoPickup(TimerID, DataID idOwner, void *)
{
    ACDID idACDItem = (ACDID)idOwner;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem || !ptACDItem->IsInWorld())
    {
        return;
    }

    const float GLYPH_AUTOPICKUP_BROADCHECK_RADIUS = 20.0f;
    WorldPlace placeItem = ptACDItem->GetPlace();

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectSphere(placeItem, GLYPH_AUTOPICKUP_BROADCHECK_RADIUS, idACDItem, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);

    float flWinningDistSqr = FLT_MAX;
    ACDID idACDWinner = ACDID_INVALID;
    POS pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listACDs.GetNext(pos);
        if (ACDIsPlayer(idACD))
        {
            ActorCommonData & tACD = ACDGet(idACD);
            WorldPlace placeACD = tACD.GetPlace();
            Vector3D vDelta = placeACD.wp - placeItem.wp;
            float flMagSqr = vDelta.MagnitudeSquared();
            if (flMagSqr < flWinningDistSqr)
            {
                ScopedGlobalDefinitionLockR pdefGlobal;
                float flPickupRadius = pdefGlobal->ptServerData->flBaseGoldPickupRadius + tACD.AttributesGetFloat(ATTRIBUTE_GOLD_PICKUP_RADIUS) + ptACDItem->GetRadius();
                if (flMagSqr <= flPickupRadius * flPickupRadius)
                {
                    flWinningDistSqr = flMagSqr;
                    idACDWinner = idACD;
                }
            }
        }
    }

    if (idACDWinner != ACDID_INVALID)
    {
        SACDInventoryPickup(ACDGetControllingActor(idACDWinner), idACDItem, -1, TRUE, FALSE, NULL, TRUE, FALSE, FALSE, TRUE);
    }
}





void SItemLanded(ActorCommonData & tACDItem)
{
    const int32 GLYPH_AUTOPICKUP_DELAY_TICKS = 90;
    if (ItemIsGlyph(tACDItem))
    {
        TimerRegister(tACDItem.id, TIMER_OWNER_ACD, GLYPH_AUTOPICKUP_DELAY_TICKS, NULL, sGlyphAutoPickup);
    }
}





ACDID SItemGenerate(
	const OnlineService::ItemId & tItemId,
	const D3::Items::Generator & tGenerator,
	const WorldPlace & tPlace,
	const float flScale,
	uint32 tName)
{
	if (!ItemsValidateGeneratorParams(tGenerator))
	{
		return ACDID_INVALID;
	}


    if (!WorldPlaceEqual(tPlace, cgplaceNull))
    {

        SSceneID idSceneToSpawnIn = SceneCommonGetSceneIDFromWorldPlace(tPlace, FALSE);
        SceneCommonData *ptScene = SceneCommonDataTryToGet(idSceneToSpawnIn);
        if (ptScene == NULL)
        {
            return ACDID_INVALID;
        }



        BOOL bActivated = SceneCommonIsActivated(idSceneToSpawnIn);
        if (!bActivated)
        {
            return ACDID_INVALID;
        }
    }



	ACDID idACDItem = sCreateItemActor(tItemId, tGenerator, tPlace, flScale, tName);
    if (idACDItem == ACDID_INVALID)
    {
        return ACDID_INVALID;
    }





	ActorCommonData & tItemACD = ACDGet(idACDItem);
	if (!ItemGenerateCommon(tItemACD, tGenerator, NULL))
	{

		ActorID idActorUseItem = (ActorID)ACDGetOwner(idACDItem);

		ActorDelete(idActorUseItem);
		return ACDID_INVALID;
	}


	return idACDItem;
}





ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, LootSpecifier & tSpecifier, const int32 nInherentAffixes)
{
    return ItemDetermineQualityLevel(szDebugStr, tSpecifier.hGBItem, &tSpecifier.arAffixGBIDs[nInherentAffixes], tSpecifier.nNumAffixes - nInherentAffixes, 0);
}














































































































































































































































































































































































































































BOOL sItemPlayerReplaceUnsocket(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    ItemPlayerReplaceUnsocketData * ptUnsocketData = (ItemPlayerReplaceUnsocketData *)pCallBackData;
    ActorCommonData & tACDHostItem = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDGem = ACDInventoryGetSquareItem(tACDHostItem, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDGem != ACDID_INVALID)
    {
        ActorCommonData * ptACDGem = ACDTryToGet(idACDGem);
        if (ptACDGem && ItemIsSocketable(*ptACDGem))
        {
            ItemRemoveSocketedItemAttributes(*ptACDGem, tACDHostItem);
            ACDInventoryRemoveToLimbo(tInvLoc.idACDContainer, *ptACDGem, FALSE);
            ptUnsocketData->idGems[ptUnsocketData->nGemsCount] = ptACDGem->id;
            ptUnsocketData->nGemsCount++;
        }
    }
    return TRUE;
}

BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax); BOOL ACDInventoryAddToLocation(ActorCommonData & tACDContainer, ActorCommonData & tACDItem, const ActorLocation & tNewLocation, BOOL fSkipRequirements, BOOL bMarkAsDirty, BOOL bOnHeroLoad, BOOL bRemoveExistingItems); BOOL ActorInventorySocketItem(ActorCommonData & tACDItemToSocket, ActorCommonData & tACDItemSocketedInto, const IVector2D & tPoint, BOOL bMarkItemAsDirty);
void SPlayerUpdateLoadoutItemANNs(Player & tPlayer, const ActorCommonData * ptACDItem, ACDNetworkName annItemOld); void LootRollForAncientLegendary(LootSpecifier & tSpecifier, const ACDID idACDLooter); void LootDetermineInherentAffixes(LootSpecifier & tSpecifier);
void LootDetermineRandomAffixes(LootSpecifier & tSpecifier); GBID ItemsChooseLevelAppropriateItemFromBaseItem(GBID gbidBaseItem, int32 nLevel); int32 ItemGetItemLevel(const GBID gbidItem);  // ActorInventory.cpp, SPlayer.cpp, Loot.cpp, Items.cpp





// Laid on its DWARF lines 861-974; rows 876/916/963/971 columns give the ternary, the `!bOldItemWasEquipped ||` test and the unconditional loadout update.
ACDID SItemPlayerReplace(
    ActorCommonData &tACDOldItem,
    D3::Items::Generator &tNewItemGenerator,
    Player &tPlayer,
    BOOL bResocketGems)
{

    ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer || !tACDOldItem.IsItem())
    {
        return ACDID_INVALID;
    }


    const InventoryLocation *ptOldItemLocation = tACDOldItem.TryToGetInventoryLocation();
    ActorCommonData *ptACDOwner = ptOldItemLocation ? ACDTryToGet(ptOldItemLocation->idACDContainer) : NULL; // row 876 cols 35/86: ternary (inferred)
    if (!ptACDOwner ||
        (ptACDOwner->id != ptACDPlayer->id &&
         ACDGetControllingActor(ptACDOwner->id) != ptACDPlayer->id))
    {
        return ACDID_INVALID;
    }



    tNewItemGenerator.clear_contents();


    ACDID idACDNewItem = SItemGenerate(OnlineService::ItemId::INVALID, tNewItemGenerator, cgplaceNull, 0.0f, 0);
    ActorCommonData *ptACDNewItem = ACDTryToGet(idACDNewItem);
    if (!ptACDNewItem)
    {
        return ACDID_INVALID;
    }


    ptACDNewItem->SetAssignedHeroID(&tPlayer);



    ItemPlayerReplaceUnsocketData tUnsocketData;
    tUnsocketData.nGemsCount = 0;
    if (tACDOldItem.HasInventory() &&
        ItemGetNumFilledSockets(tACDOldItem) > 0)
    {
        ACDInventoryIterateSlot(tACDOldItem, SLOT_ITEM_SOCKETS, NULL, sItemPlayerReplaceUnsocket, &tUnsocketData, FALSE, FALSE);
    }


    ActorLocation tLocation;
    tLocation.eActorLocation = AL_INVENTORY;
    tLocation.tInvLocation = *ptOldItemLocation;

    BOOL bAddedToInventory = FALSE;
    BOOL bOldItemWasEquipped = ItemIsEquipped(tACDOldItem);
    if (!bOldItemWasEquipped ||
        ItemOwnerMeetsRequirements(*ptACDNewItem, *ptACDOwner, NULL))
    {


        SACDInventoryAddToDirtyList(tACDOldItem, AIDA_REMOVE, tACDOldItem.GetLocationSlot(), -1);
        ACDInventoryRemoveToLimbo(ptACDOwner->id, tACDOldItem, FALSE);


        bAddedToInventory = ACDInventoryAddToLocation(*ptACDOwner, *ptACDNewItem, tLocation, FALSE, TRUE, FALSE, FALSE);
    }


    ACDID idACDOldItem = tACDOldItem.id;
    SACDInventoryDeleteItem(tACDOldItem, TRUE, TRUE, FALSE);


    if (!bAddedToInventory)
    {
        SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDNewItem, FALSE, NULL, TRUE);



        if (!ptACDNewItem->IsInInventory())
        {
            bResocketGems = FALSE;
        }
    }


    for (int32 i = 0; i < tUnsocketData.nGemsCount; i++)
    {
        ActorCommonData *ptACDGem = ACDTryToGet(tUnsocketData.idGems[i]);


        IVector2D tPoint(0, 0);
        if (!bResocketGems ||
            !ACDInventoryCanHold(*ptACDNewItem, *ptACDGem, SLOT_ITEM_SOCKETS, tPoint, NULL) ||
            !ActorInventorySocketItem(*ptACDGem, *ptACDNewItem, tPoint, TRUE))
        {

            SACDInventoryPickupOrSpillOntoGround(*ptACDPlayer, *ptACDGem, FALSE, NULL, TRUE);
        }
    }



    if (bOldItemWasEquipped && !bAddedToInventory &&

        ItemOwnerMeetsRequirements(*ptACDNewItem, *ptACDOwner, NULL))
    {
        ACDInventoryAddToLocation(*ptACDOwner, *ptACDNewItem, tLocation, FALSE, TRUE, FALSE, FALSE);
    }
    // row 971 col 5 and every path (tbnz 963, cbz 965) reaching it: called unconditionally

    SPlayerUpdateLoadoutItemANNs(tPlayer, ptACDNewItem, ANNFromACD(idACDOldItem));

    return idACDNewItem;
}




ACDID SItemReforge(ActorCommonData & tACDOldItem, Player & tPlayer)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return ACDID_INVALID;
    }

    if (!tACDOldItem.IsItem())
    {
        return ACDID_INVALID;
    }

    ScopedItemDefinitionLockR pdefItem(tACDOldItem.hGB.gbid);
    if (!pdefItem || !ItemIsLegendary(tACDOldItem.hGB.gbid))
    {
        return ACDID_INVALID;
    }

    int32 nItemLevel = ItemGetItemLevel(tACDOldItem.hGB.gbid);
    int32 nLegendaryItemLevelOverride = tACDOldItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
    nLegendaryItemLevelOverride = (nLegendaryItemLevelOverride > 0) ? nLegendaryItemLevelOverride : nItemLevel;

    GBID gbidBaseItem = ItemsChooseLevelAppropriateItemFromBaseItem(pdefItem->gbidBaseItem, nLegendaryItemLevelOverride);
    if (!GBIDExists(gbidBaseItem, GB_ITEMS))
    {
        return ACDID_INVALID;
    }

    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDOldItem, ACDID_INVALID);

    LootSpecifier tSpecifier;
    tSpecifier.tLooteeParams = tLootee.tLooteeParams;
    tSpecifier.hGBItem = tACDOldItem.hGB;
    tSpecifier.eIQL = IQL_LEGENDARY;
    tSpecifier.nAffixLevel = nLegendaryItemLevelOverride;
    tSpecifier.nLegendaryItemLevel = nLegendaryItemLevelOverride;
    tSpecifier.fAutoIdentify = TRUE;
    tSpecifier.fAccountBound = TRUE;
    tSpecifier.tLooteeParams.bForceCrafterAncientLegendaryChance = TRUE;
    LootRollForAncientLegendary(tSpecifier, tPlayer.idPlayerACD);

    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptACDPlayer);
    tSpecifier.FillInTargetedClass(ePlayerClass, NULL, TRUE);
    LootDetermineInherentAffixes(tSpecifier);
    LootDetermineRandomAffixes(tSpecifier);

    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);
    tGenerator.set_seed(RandGetSeed(SGameGetRNG()));
    tGenerator.mutable_gb_handle()->set_gbid(tSpecifier.hGBItem.gbid);
    tGenerator.set_legendary_item_level(tSpecifier.nLegendaryItemLevel);
    tGenerator.set_legendary_base_item_gbid(gbidBaseItem);
    tGenerator.set_item_quality_level(tSpecifier.eIQL);
    for (int32 i = 0; i < tSpecifier.nNumAffixes; i++)
    {
        tGenerator.add_base_affixes(tSpecifier.arAffixGBIDs[i]);
    }

    uint32 uFlags = tGenerator.flags() | (1 << IGF_IDENTIFIED_BIT);
    uFlags = tACDOldItem.AttributesGetInt(ATTRIBUTE_IS_VENDOR_BOUGHT) ? (uFlags | (1 << IGF_VENDOR_BOUGHT_BIT)) : (uFlags & ~(1 << IGF_VENDOR_BOUGHT_BIT));
    uFlags = tACDOldItem.AttributesGetInt(ATTRIBUTE_LOOT_2_0_DROP) ? (uFlags | (1 << IGF_LOOT_2_0_BIT)) : (uFlags & ~(1 << IGF_LOOT_2_0_BIT));
    uFlags = (tSpecifier.eAncientRank == ANCIENT_RANK_ANCIENT) ? (uFlags | (1 << IGF_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_ANCIENT_BIT));
    uFlags = (tSpecifier.eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT) ? (uFlags | (1 << IGF_PRIMAL_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_PRIMAL_ANCIENT_BIT));
    tGenerator.set_flags(uFlags);
    tGenerator.set_item_binding_level(IBL_ACCOUNT_PICKUP);

    if (GameIsSeasonal())
    {
        tGenerator.set_season_created(GameGetCurrentSeason());
    }

    return SItemPlayerReplace(tACDOldItem, tGenerator, tPlayer, FALSE);
}


































































































static const uint32 ITEM_TRADING_TIMER_DELAY = 3600;
void sItemTradingTimerCallback(TimerID idTimer, DataID idOwner, void *)
{
    ACDID idACDItem = (ACDID)idOwner;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
    {
        return;
    }

    ACDID idACDOwner = ActorInventoryGetUltimateOwner(ptACDItem);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (!ptACDOwner)
    {
        return;
    }

    Player * ptPlayer = ptACDOwner->GetPetOwnerPlayer();
    ACDID idACDPlayerOwner = ptPlayer ? ptPlayer->idPlayerACD : idACDOwner;
    if (ACDIsPlayer(idACDPlayerOwner))
    {
        uint32 uTradeEndTime = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_TRADE_END_TIME);
        uint32 uNow = GameGetServerTime();
        if (uNow < uTradeEndTime)
        {
            TimerUnregister(idTimer);
            SItemRegisterTradingTimer(*ptACDItem, ITEM_TRADING_TIMER_DELAY);
            SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_TRADE_TIMER_PERIODIC, ptACDItem->GetLocationSlot(), ptACDItem->GetLocationSlot());
        }
        else
        {
            ItemSetBindingLevel(*ptACDItem, IBL_ACCOUNT_PICKUP);
            ItemSetBoundToACD(*ptACDItem, idACDPlayerOwner);
            SACDInventoryAddToDirtyList(*ptACDItem, AIDA_UPDATE_TRADE_TIMER_COMPLETE, ptACDItem->GetLocationSlot(), ptACDItem->GetLocationSlot());
        }
    }
    else
    {
        TimerUnregister(idTimer);
        ItemSetBindingLevel(*ptACDItem, IBL_ACCOUNT_PICKUP);
    }
}












void SItemRegisterTradingTimer(ActorCommonData & tACDItem, uint32 uUpdateTicks)
{
    uint32 uTimeLeft = ItemGetTradeTimeLeft(tACDItem);
    uint32 uDelayTicks = uTimeLeft > uUpdateTicks ? uUpdateTicks : uTimeLeft;

    TimerID idTimer = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_TRADE_TIMER_ID);
    if (TimerExists(idTimer))
    {
        if (TimerGetTime(idTimer) <= GameGetServerTime() + uDelayTicks)
        {
            return;
        }
        TimerUnregister(idTimer);
    }

    idTimer = TimerRegister(tACDItem.id, TIMER_OWNER_ACD, uDelayTicks, (void *)(intptr_t)tACDItem.id, sItemTradingTimerCallback);
    tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_TRADE_TIMER_ID, idTimer);
}









void SItemBroadcastToClan(const ActorCommonData & tACDItem, const Player & tPlayer)
{
}
























BOOL SItemApplyDye(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, DyeType eDyeType)
{
    if (!ItemIsDyeable(tACDItem))
    {
        return FALSE;
    }

    AttribGroupID idFastAttrib = tACDItem.AttributesGetAttributeGroup();
    FastAttribSetValueInt(idFastAttrib, ATTRIBUTE_DYE_TYPE, eDyeType);
    tACDItem.ModifiedMark(0);
    SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_DYE, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());

    InventoryLocation & tInvLocation = tACDItem.GetInventoryLocation();
    VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(tInvLocation.eSlot);
    if (eVisualSlot != -1)
    {
        GBID gbidActor = tACDItem.GetVisualGBID();
        ItemEffectType eItemEffectType;
        int32 nEffectLevel;
        ACDGetItemEffectTypeAndLevel(tACDItem, eItemEffectType, nEffectLevel, TRUE, FALSE);
        VisualInventorySetItemInSlot(tACDOwner.id, eVisualSlot, gbidActor, eDyeType, eItemEffectType, nEffectLevel, ItemIsBroken(tACDItem));
    }

    Player * ptPlayer = PlayerGetByACD(tACDOwner.id);
    if (ptPlayer)
    {
        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(tACDItem.id);
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_DYE_NOTIFY, &tMessage, sizeof(tMessage));
    }

    return TRUE;
}









void SItemApplyTemporaryTransmog(ActorCommonData & tACDItem, GBID gbidTransmog, ActorCommonData & tACDOwner)
{
    if (!ItemIsEquipped(tACDItem))
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(tACDOwner.id);
    if (!ptPlayer)
    {
        return;
    }

    if (ItemCrafting_CanTransmogItem(tACDItem, gbidTransmog, ptPlayer, FALSE, FALSE))
    {
        tACDItem.AttributesSetInt(ATTRIBUTE_TRANSMOG_GBID_TEMP_OVERRIDE, gbidTransmog);
        InventorySlotType eEquippedSlot = tACDItem.GetLocationSlot();
        VisualInventoryResetSlot(tACDOwner.id, eEquippedSlot);
    }
}














void SItemRemoveTemporaryTransmog(ActorCommonData & tACDItem, ActorCommonData & tACDOwner)
{
    tACDItem.AttributesSetInt(ATTRIBUTE_TRANSMOG_GBID_TEMP_OVERRIDE, -1);
    if (ItemIsEquipped(tACDItem))
    {
        InventorySlotType eEquippedSlot = tACDItem.GetLocationSlot();
        VisualInventoryResetSlot(tACDOwner.id, eEquippedSlot);
    }
}








BOOL SItemConsumableAddSockets(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, const ActorCommonData & tACDItemToUse)
{
    if (IsSocketAddable(tACDItem, tACDItemToUse) != GAMEERROR_NONE)
    {
        return FALSE;
    }

    AttribGroupID idFastAttrib = tACDItem.AttributesGetAttributeGroup();
    FastAttribSetValueInt(idFastAttrib, ATTRIBUTE_CONSUMABLE_ADD_SOCKETS, TRUE);

    Player * ptPlayer = PlayerGetByACD(tACDOwner.id);

    D3::Items::Generator tGenerator;
    ItemGeneratorPopulateFromACDID(tACDItem, tGenerator, TRUE);
    tGenerator.set_item_binding_level(IBL_ACCOUNT_PICKUP);
    return SItemPlayerReplace(tACDItem, tGenerator, *ptPlayer, FALSE);
}

#include "../Sigma/Math/MathGeneral.h"
#include "../Common/Attributes/FastAttributeStructs.h"
#include "../Common/Attributes/AttributeEnums.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
BOOL ItemIsUpgradeableJewel(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:9622
BOOL ItemIsSocketed(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:3522
float ItemGetAttributeSpecifierValue(const GBID gbidItem, const AttributeSpecifier & tAttribute, const ActorCommonData * ptACDOwner, RNG * ptRNGOverride, int32 nLegendaryItemLevelOverride, BOOL bPrimalAncient);  // X/Common/Items.cpp:4889

extern "C" BOOL sub_71004f71e0(const ActorCommonData & tACDItem);  // X/Common/Items.cpp (ItemDefinition flag bit 30)
extern "C" void sub_71007e02c0(ActorCommonData & tACDItem, const AttributeSpecifier * ptSpecifier, const ActorCommonData * ptACDOwner);

typedef blz::pair<int32, ActorCommonData::Unk2c0Value> JewelUpgradeAffix;

struct JewelUpgradeAffixLess
{
    bool operator()(const JewelUpgradeAffix & tLeft, const JewelUpgradeAffix & tRight) const
    {
        if (tLeft.second.unk_0 != tRight.second.unk_0)
            return tLeft.second.unk_0 < tRight.second.unk_0;
        return tLeft.first < tRight.first;
    }
};

// NON_MATCHING: 2344 vs 2316 bytes; same calls and branches as 2.7.7 (affix map copied to a vector, blz::sort by rank then GBID, three sub_71007e02c0 secondary effects, bit-30 SItemPlayerReplace tail). The frame is 0x10 smaller and registers/spill slots differ.
BOOL SItemUpgradeJewel(ActorCommonData & tACDItem, int32 nUpgradeRanks, ActorCommonData & tACDOwner)
{
    if (!ItemIsUpgradeableJewel(tACDItem))
    {
        return FALSE;
    }

    if (ItemIsUnidentified(tACDItem))
    {
        return FALSE;
    }

    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (!pdefItem)
    {
        return FALSE;
    }

    int32 nMaxRank = pdefItem->nJewelMaxRank;
    int32 nJewelRank = tACDItem.AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
    if (nMaxRank >= 0)
    {
        int32 nRanksAvailable = Math::Max(nMaxRank - nJewelRank, 0);
        nUpgradeRanks = Math::Min(nUpgradeRanks, nRanksAvailable);
    }

    if (nUpgradeRanks <= 0 || !tACDItem.IsInInventory())
    {
        return FALSE;
    }

    int32 nJewelRankNew = nJewelRank + nUpgradeRanks;
    Actor * ptActor = ActorGet((ActorID)tACDItem.idOwner);
    RNG tItemRNG;
    RandInit(tItemRNG);
    RandSetSeed_Internal(tItemRNG, ptActor->dwItemIdentifySeed);
    tACDItem.ptRNG = &tItemRNG;
    ItemAddBaseAttributesForItem(tACDItem, tACDItem.hGB, FALSE, TRUE, FALSE);

    blz::vector<JewelUpgradeAffix> arAffixes;
    for (blz::unordered_map<int32, ActorCommonData::Unk2c0Value>::iterator it = tACDItem.unk_2c0.begin(); it != tACDItem.unk_2c0.end(); ++it)
    {
        arAffixes.push_back(JewelUpgradeAffix(it->first, it->second));
    }
    blz::sort(arAffixes.begin(), arAffixes.end(), JewelUpgradeAffixLess());

    for (blz::vector<JewelUpgradeAffix>::iterator it = arAffixes.begin(); it != arAffixes.end(); ++it)
    {
        if (nJewelRank < it->second.unk_0 && nJewelRankNew >= it->second.unk_0)
        {
            D3::Items::Generator tGenerator;
            ItemGeneratorPopulateFromACDID(tACDItem, tGenerator, TRUE);
            ItemsAddItemAffixGBID(tACDItem, it->first, tGenerator.flags() & (1 << IGF_IDENTIFIED_BIT), tGenerator.item_quality_level(), FALSE, NULL);
            it->second.unk_4 = TRUE;
        }
    }
    tACDItem.ptRNG = NULL;

    if (pdefItem->tJewelUpgradeAttribute.eAttribute != -1)
    {
        FastAttribKey tKey = MakeAttribKey(pdefItem->tJewelUpgradeAttribute.eAttribute, pdefItem->tJewelUpgradeAttribute.nParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);
        ActorCommonData * ptACDOwner = ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer);
        float flAttributeValuePerRank = ItemGetAttributeSpecifierValue(tACDItem.hGB.gbid, pdefItem->tJewelUpgradeAttribute, ptACDOwner, &tItemRNG, tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE), FALSE);
        FastAttribValue tValuePerRank;
        if (eAttribType == ATTRIBUTEDT_FLOAT)
        {
            tValuePerRank.flValue = flAttributeValuePerRank;
        }
        else
        {
            tValuePerRank.nValue = (int32)flAttributeValuePerRank;
        }
        FastAttribValue tValue = FastAttribGetValue(tACDItem.idFastAttrib, tKey);
        if (eAttribType == ATTRIBUTEDT_FLOAT)
        {
            tValue.flValue += nUpgradeRanks * tValuePerRank.flValue;
        }
        else
        {
            tValue.nValue += tValuePerRank.nValue * nUpgradeRanks;
        }
        FastAttribSetValue(tACDItem.idFastAttrib, tKey, tValue);
    }

    int32 nUnlockRank = pdefItem->nJewelSecondaryEffectUnlockRank;
    if (nJewelRankNew >= nUnlockRank && nJewelRank < nUnlockRank && nUnlockRank > 0)
    {
        sub_71007e02c0(tACDItem, &pdefItem->tJewelSecondaryEffectAttribute, ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer));
    }

    nUnlockRank = pdefItem->unk_2d0;
    if (nJewelRankNew >= nUnlockRank && nJewelRank < nUnlockRank && nUnlockRank > 0)
    {
        sub_71007e02c0(tACDItem, &pdefItem->unk_2b8, ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer));
    }

    nUnlockRank = pdefItem->unk_2f0;
    if (nJewelRankNew >= nUnlockRank && nJewelRank < nUnlockRank && nUnlockRank > 0)
    {
        sub_71007e02c0(tACDItem, &pdefItem->unk_2d8, ACDTryToGet(tACDItem.tLocation.tInvLocation.idACDContainer));
    }

    tACDItem.AttributesSetInt(ATTRIBUTE_JEWEL_RANK, nJewelRankNew);

    tACDItem.ModifiedMark(0);
    SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_JEWEL_UPGRADE, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());

    if (ItemIsSocketed(tACDItem))
    {
        ActorCommonData * ptACDHostItem = ACDTryToGet(tACDItem.GetLocation().tInvLocation.idACDContainer);
        if (ptACDHostItem)
        {
            ptACDHostItem->ModifiedMark(0);
            SACDInventoryAddToDirtyList(*ptACDHostItem, AIDA_UPDATE_JEWEL_UPGRADE, ptACDHostItem->GetLocationSlot(), ptACDHostItem->GetLocationSlot());
        }
    }

    if (sub_71004f71e0(tACDItem))
    {
        D3::Items::Generator tGenerator;
        ItemGeneratorPopulateFromACDID(tACDItem, tGenerator, TRUE);
        return SItemPlayerReplace(tACDItem, tGenerator, *PlayerGetByACD(tACDOwner.id), FALSE) != ACDID_INVALID;
    }
    return TRUE;
}

extern "C" void sub_71007e02c0(ActorCommonData & tACDItem, const AttributeSpecifier * ptSpecifier, const ActorCommonData * ptACDOwner)
{
    FastAttribKey tKey = MakeAttribKey(ptSpecifier->eAttribute, ptSpecifier->nParam);
    AttributeDataType eAttribType = KeyGetDataType(tKey);
    float flEffectValue = ItemGetAttributeSpecifierValue(tACDItem.hGB.gbid, *ptSpecifier, ptACDOwner, tACDItem.ptRNG, tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE), FALSE);
    FastAttribValue tEffectValue;
    if (eAttribType == ATTRIBUTEDT_FLOAT)
    {
        tEffectValue.flValue = flEffectValue;
    }
    else
    {
        tEffectValue.nValue = (int32)flEffectValue;
    }
    FastAttribValue tValue = FastAttribGetValue(tACDItem.idFastAttrib, tKey);
    if (eAttribType == ATTRIBUTEDT_FLOAT)
    {
        tValue.flValue += tEffectValue.flValue;
    }
    else
    {
        tValue.nValue += tEffectValue.nValue;
    }
    FastAttribSetValue(tACDItem.idFastAttrib, tKey, tValue);
}




BOOL SItemPlayerExtractLegendaryPower(Player & tPlayer, GBID gbidItem, GameError & eGameError)
{
    eGameError = GAMEERROR_NONE;
    if (!ItemCanExtractLegendaryPower(gbidItem))
    {
        eGameError = GAMEERROR_TRANSMUTE_FAILED_CANNOT_EXTRACT_FROM_THIS_ITEM;
        return FALSE;
    }

    if (!ItemPlayerHasExtractedLegendaryPower(tPlayer, gbidItem))
    {
        int32 & nExtractedLegendariesCount = tPlayer.tCrafterSavedData.tExtractedLegendariesData.nExtractedLegendariesCount;
        tPlayer.tCrafterSavedData.tExtractedLegendariesData.gbidExtractedLegendaries[nExtractedLegendariesCount] = gbidItem;
        nExtractedLegendariesCount++;

        D3::GameBalance::BitPackedGbidArray tExtractedLegendariesData;
        tExtractedLegendariesData.add_elements(gbidItem);
        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listPlayers, tPlayer);
        MessageSendToClient(listPlayers, SMSG_EXTRACTED_LEGENDARIES_DATA_PARTIAL, tExtractedLegendariesData);

        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(&tPlayer);
        tEvaluationData.EvaluateEvent(&tPlayer, Achievement::ACH_EVENT_PLAYER_EXTRACTED_POWER, 1, (uint32)sub_71007b5e70());
    }

    return TRUE;
}













































CRefString SItemGetDisplayName(const ActorCommonData & tACD, const BOOL, const BOOL fForceIdentified, const BOOL, const BOOL)
{
    SNO snoItemStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS);
    SNO snoItemTypeNames = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES);

    CRefString usReturn("ERROR GETTING NAME");
    if (tACD.hGB.eType != GB_ITEMS)
    {
        return usReturn;
    }

    BOOL fIdentified = !ItemIsUnidentified(tACD);

    ScopedItemDefinitionLockR ptItem(tACD.hGB.gbid);
    if (!ptItem)
    {
        return CRefString("BAD GBID");
    }

    LPCSTR szItemLabel = ptItem->tHeader.szName;
    GBID gbidItemType = ptItem->gbidItemType;
    CRefString sItemName;
    if (!fForceIdentified && !fIdentified && (ptItem->dwFlags & 3))
    {
        sItemName = StringListFetch(snoItemTypeNames, GBIDToString(gbidItemType, GB_ITEMTYPES, FALSE));
    }
    else
    {
        CRefString sInitialItemName;
        CRefString sName2Ignored;
        uint32 dwGrammaticalForm;
        if (StringListFetch(snoItemStringList, szItemLabel, sInitialItemName))
        {
            LanguageRules::ExtractGrammaticalForm(sInitialItemName, sItemName, sName2Ignored, dwGrammaticalForm);
        }
    }

    return sItemName;
}
