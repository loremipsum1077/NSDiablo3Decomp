// Original file: X/Server/Network/ClientValidation.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "ClientValidation.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/GlobalSNO.h"
#include "../SGame.h"
#include "../SPlayer.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Definitions/ConversationDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Hyperlink.h"
#include "../../Common/Items.h"
#include "../../Common/Crafting/ItemCrafting.h"
#include "../../Common/NPC.h"
#include "../../Common/Target.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersStruct.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/DataStructures/XPooledMap.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/Game.h"
#include "../SGameManager.h"
#include "../SWorld.h"
#include "../Actors/Actors.h"
#include "../../Common/Networking/GameMessage.h"
// Declared here from their definitions elsewhere (headers not adopted yet).
BOOL PowerAlternatesAnims(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
uint32 SPlayerGetConsoleLicenseFlags(const Player & tPlayer);
Act SPlayerGetMaxAct(const Player & tPlayer);
SNO ConversationGetEndGameConversation(uint32 uContentFlags);
BOOL CValidateItemIsInPlayerInventory(const Player & tPlayer, const ACDID idACDItem, BOOL bExcludeHirelingSlots);
BOOL CValidatePlayerNearStash(const Player & tPlayer);
ItemSellError ItemCanBeSold(const ActorCommonData & tACDItem, const ActorCommonData * ptACDVendor);
BOOL ItemIsSocketed(const ActorCommonData & tACDItem);
CrafterType NPCGetCrafterType(const ActorCommonData & tACD);
void ACDInventoryGetSlotTypeInfo(InventoryType eType, int32 & nStart, int32 & nStop, int32 & nSlots);
void PowerGetStartRange(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldDistance & wdRange);
BOOL ItemIsBroken(const ActorCommonData & tACDItem);  // X/Common/Items.cpp (header not adopted yet)
ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD);  // X/Common/ActorInventory.cpp:5373
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Definitions/SkillKitDefinition.h"




static const WorldDistance sgwdReasonablePickupItemDistanceSquared = 100.0f;


static const int32 MODERATE_FAILURES_MIN_TO_DISCONNECT = 3;




static const int32 MODERATE_FAILURES_MAX_PER_HOUR = 6;



static const int32 TEN_SECOND_SPAM_THRESHOLD = 1000;

static XVarScope s_scopeClientValidation("ClientValidation", NULL);
XVarUint32 s_varModerateFailuresMaxPerHour(s_scopeClientValidation, "ModerateFailuresMaxPerHour", MODERATE_FAILURES_MAX_PER_HOUR);
XVarUint32 s_varModerateFailuresMinToDisconnect(s_scopeClientValidation, "ModerateFailuresMinToDisconnect", MODERATE_FAILURES_MIN_TO_DISCONNECT);
XVarBool s_varDisconnectOnValidationErrors(s_scopeClientValidation, "DisconnectOnValidationErrors", true);
XVarUint32 s_varTenSecondSpamThreshold(s_scopeClientValidation, "TenSecondSpamThreshold", TEN_SECOND_SPAM_THRESHOLD);










struct ItemKey {
    GBID gbid;
    uint32 dwSeed;

    ItemKey() {}
    ItemKey(const GBID _gbid, const uint32 _dwSeed) : gbid(_gbid), dwSeed(_dwSeed) {}

    BOOL operator==(const ItemKey & tKey) const
    {
        return dwSeed == tKey.dwSeed && gbid == tKey.gbid;
    }
};

// Invented: the binary hashes an ItemKey by its seed alone and the DWARF records no inline for it.
template <>
inline uint32 XHashKey<ItemKey>(ItemKey key) { return key.dwSeed; }

typedef XPooledMap<ItemKey, ItemKey, ACDID, ACDID, 1024> ItemDupeMap;

struct ClientValidationGlobals {
    XMapMemoryPool<ItemKey, ACDID> tItemDupeMapPool;
    ItemDupeMap tItemDupeMap;
};
static_assert(sizeof(ClientValidationGlobals) == 0x2058, "ClientValidationGlobals");






static const int32 ITEM_DUPE_MAP_VALUES_COUNT = 512;

void ClientValidationInitializeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    ClientValidationGlobals *& ptClientValidationGlobals = tSGame.ptClientValidationGlobals;

    ptClientValidationGlobals = (ClientValidationGlobals *)SigmaMemoryNew(sizeof(ClientValidationGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    ptClientValidationGlobals->tItemDupeMapPool.Initialize();
    ptClientValidationGlobals->tItemDupeMapPool.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptClientValidationGlobals->tItemDupeMapPool.SetBlockCount_Tracked(ITEM_DUPE_MAP_VALUES_COUNT);
    ptClientValidationGlobals->tItemDupeMap.Initialize(&ptClientValidationGlobals->tItemDupeMapPool);
}

void ClientValidationDisposeForGame()
{

    SGame & tSGame = SGameManager::GameGet();
    ClientValidationGlobals *& ptClientValidationGlobals = tSGame.ptClientValidationGlobals;

    ptClientValidationGlobals->tItemDupeMap.Dispose();
    ptClientValidationGlobals->tItemDupeMapPool.Dispose();

    SigmaMemoryFree(ptClientValidationGlobals, tSGame.ptMemoryInterface);
    ptClientValidationGlobals = NULL;
}























































































































// NON_MATCHING: 528/528 bytes. The compiler places the target-message block before the shared tail. The original places it after the return.
BOOL CValidateSpamCheck(
    Player & tPlayer,
    const GameMessageType eMessageType,
    void * pMessage)
{





    uint32 dwNow = GameGetServerTime();
    if (dwNow != tPlayer.dwTickLastSpamCheck)
    {
        uint32 dwDiff = dwNow - tPlayer.dwTickLastSpamCheck;

        tPlayer.dwTickLastSpamCheck = dwNow;


        tPlayer.nMessageCostLast3600 -= Math::Min(tPlayer.nMessageCostLast3600 * dwDiff / 3600, tPlayer.nMessageCostLast3600);
        tPlayer.nMessageCostLast600 -= Math::Min(tPlayer.nMessageCostLast600 * dwDiff / 600, tPlayer.nMessageCostLast600);
        tPlayer.nMessageCostLast300 -= Math::Min(tPlayer.nMessageCostLast300 * dwDiff / 300, tPlayer.nMessageCostLast300);
    }

    uint32 nAllowedPerTick = 1;


    uint32 nCost = 0;
    // Case lists come from the jump table. The source position of the 585 case is inferred.
    switch (eMessageType)
    {
    // 595 is also MSG_RANGE_INGAME_END; the message name is taken to be the real one.
    case XMSG_SAVE_BANNER_COMPLETE:
        break;

    case CMSG_VENDOR_CLOSE:
    case CMSG_SHOP_TRY_BUY:
    case CMSG_SECONDARY_ANIMATION_POWER:
    case CMSG_LOOPING_ANIMATION_POWER:
    case CMSG_EXECUTE_NON_PRIMARY_POWER:
    case CMSG_TRY_INVENTORY_USE:
    case CMSG_OPERATE_GIZMO:
    case CMSG_ACCEPT_RESURRECT:
    case CMSG_NPC_TALK_REQUEST:
    case CMSG_SHARED_STASH_TRY_BUY:
    case CMSG_TRY_SALVAGE:
    case CMSG_TRY_SALVAGE_ALL:
    case CMSG_TRY_REPAIR:
    case CMSG_TRY_REPAIR_ALL:
    case CMSG_TRY_REPAIR_EQUIPPED:
    case CMSG_VENDOR_TRY_TRAIN:
    case CMSG_TRY_CRAFT_ITEM:
    case CMSG_TRY_CRAFT_ITEMS:
    case CMSG_TRY_IDENTIFY_WITH_CAST:
    case CMSG_TRY_IDENTIFY_ALL_WITH_CAST:
    case CMSG_REQUEST_BUFF_CANCEL:
    case CMSG_SEND_PLAYER_PREFS:
    case CMSG_TRIED_SALVAGE:
    case CMSG_TRY_REDEEM_PVP_TOKENS:
    case CMSG_DARK_ALCHEMY_ACTIVATE_NODE:
        nCost = 3;
        break;










    case CMSG_TARGET:
    {
        TargetMessage * ptTargetMsg = (TargetMessage *)pMessage;
        PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(ptTargetMsg->snoPower), ACDID_INVALID);



        nCost = (eTemplate == SPECIALWALK_POWER) ? 1 : 3;
        break;
    }

    case CMSG_ACD_TRANSLATE_NORMAL:
    case CMSG_ACD_TRANSLATE_FACING:
    case CMSG_ACD_TRANSLATE_ACK:
    case CMSG_PLAYER_BUSY:
    case CMSG_PLAYER_LORE_PLAYING:
    case CMSG_TRADE:
    case CMSG_REQUEST_CONV_ADVANCE:
    case CMSG_UPDATE_CONV_AUTO_ADVANCE:
    case CMSG_REQUEST_CONV_CANCEL:
    case CMSG_PING:
    case CMSG_NPC_INTERACT_CANCEL:
    case CMSG_REQUEST_PAUSE_STATE_CHANGE:
        nCost = 1;
        break;

    case CMSG_POLL_MAIL:
    case CMSG_FETCH_MAIL:
    case CMSG_SEND_MAIL:
    case CMSG_MAIL_OPERATION:
        nCost = 2;
        break;






    default:
        nCost = 10;
        break;
    }



























    tPlayer.nMessageCostLast3600 += nCost;
    tPlayer.nMessageCostLast600 += nCost;
    tPlayer.nMessageCostLast300 += nCost;

    if (tPlayer.nMessageCostLast300 > tPlayer.nMessageCostMax300)
    {
        tPlayer.nMessageCostMax300 = tPlayer.nMessageCostLast300;
    }
    if (tPlayer.nMessageCostLast600 > tPlayer.nMessageCostMax600)
    {
        tPlayer.nMessageCostMax600 = tPlayer.nMessageCostLast600;
    }
    if (tPlayer.nMessageCostLast3600 > tPlayer.nMessageCostMax3600)
    {
        tPlayer.nMessageCostMax3600 = tPlayer.nMessageCostLast3600;
    }



    return TRUE;
}












static BOOL CValidateACDDistance(const ActorCommonData & tACD0, const ActorCommonData & tACD1, const WorldDistance wdDistanceSquaredToCheck)
{





    const ActorLocation & tLocation0 = tACD0.GetLocation();
    const ActorLocation & tLocation1 = tACD1.GetLocation();


    if (tLocation0.eActorLocation != 0)
    {

        return FALSE;
    }
    if (tLocation1.eActorLocation != 0)
    {

        return FALSE;
    }



    if (tLocation0.tWorldLocation.idSWorld != tLocation1.tWorldLocation.idSWorld)
    {

        return FALSE;
    }




    WorldDistance wd = Vector3D::DistanceSquared(tLocation0.tWorldLocation.transform.wp, tLocation1.tWorldLocation.transform.wp);
    return !(wd > wdDistanceSquaredToCheck);
}



BOOL CValidatePlayerCloseEnoughToPickupItem(const ActorCommonData & tACD0, const ActorCommonData & tACD1)
{


    return CValidateACDDistance(tACD0, tACD1, sgwdReasonablePickupItemDistanceSquared);
}



BOOL CValidatePlayerNearCrafter(const Player & tPlayer, const CrafterType eCrafterType)
{


    const ActorCommonData * ptACDNPC = ACDTryToGet(tPlayer.idACDActiveVendor);
    if (!ptACDNPC)
    {

        return FALSE;
    }


    const ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    const WorldPlace & placePlayer = tACDPlayer.GetPlace();
    const WorldPlace & placeNPC = ptACDNPC->GetPlace();
    if (!WorldPlaceInSameWorld(placePlayer, placeNPC))
    {

        return FALSE;
    }

    float flDistSqr = WorldPlaceDistanceSquared(placePlayer, placeNPC);
    if (flDistSqr >= MAX_CONVERSATION_DISTANCE * MAX_CONVERSATION_DISTANCE)
    {

        return FALSE;
    }

    if (eCrafterType != -1)
    {
        CrafterType eNPCCrafterType = NPCGetCrafterType(*ptACDNPC);
        if (eNPCCrafterType != eCrafterType)
        {

            return FALSE;
        }

        int32 nCrafterLevel = GetCraftLevel(eCrafterType, tPlayer);
        if (nCrafterLevel <= 0)
        {

            return FALSE;
        }
    }

    return TRUE;
}




BOOL CValidatePlayerNearTradeNPC(const Player & tPlayer)
{

    const ActorCommonData * ptACDNPC = ACDTryToGet(tPlayer.idACDActiveVendor);
    if (!ptACDNPC)
    {
        return FALSE;
    }




    if (!CValidatePlayerNearCrafter(tPlayer, -1))
    {

        return FALSE;
    }
    if (!NPCCanTrade(*ptACDNPC))
    {



        return FALSE;
    }
    return TRUE;
}
BOOL CValidatePlayerNearInteractNPC(const Player & tPlayer)
{

    const ActorCommonData * ptACDNPC = ACDTryToGet(tPlayer.tTalkData.idACDTalkingTo);
    if (!ptACDNPC)
    {
        return FALSE;
    }



    const ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    const WorldPlace & placePlayer = tACDPlayer.GetPlace();
    const WorldPlace & placeNPC = ptACDNPC->GetPlace();
    if (!WorldPlaceInSameWorld(placePlayer, placeNPC))
    {

        return FALSE;
    }

    float flDistSqr = WorldPlaceDistanceSquared(placePlayer, placeNPC);
    if (flDistSqr >= MAX_CONVERSATION_DISTANCE * MAX_CONVERSATION_DISTANCE)
    {

        return FALSE;
    }
    return TRUE;
}
inline BOOL CValidateIsACDValidItem(const ACDID idACDItem)
{


    const ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
    {

        return FALSE;
    }
    if (!ptACDItem->IsItem())
    {

        return FALSE;
    }
    return TRUE;
}





BOOL CValidateDoesItemUltimatelyBelongToPlayer(const Player & tPlayer, const ACDID idACDOwner)
{

    if (tPlayer.idPlayerACD == idACDOwner)
    {
        return TRUE;
    }





    ACDID idACDPetOwner = ACDGetPetOwner(idACDOwner);
    if (idACDPetOwner == tPlayer.idPlayerACD)
    {

        const ActorCommonData * ptACDInventory = ACDTryToGet(idACDOwner);
        if (ptACDInventory)
        {

            PetType ePetType = ptACDInventory->AttributesGetInt(ATTRIBUTE_PET_TYPE);
            if (ePetType == 0 || ePetType == 26)
            {
                return TRUE;
            }
        }
    }



    return FALSE;
}


BOOL CValidateItemCanBeSold(const Player & tPlayer, const ACDID idACDItem)
{


    if (!CValidateItemIsInPlayerInventory(tPlayer, idACDItem, FALSE))
    {

        return FALSE;
    }




    ActorCommonData & tACDItem = ACDGet(idACDItem);
    const ActorCommonData * ptACDNPC = ACDTryToGet(tPlayer.idACDActiveVendor);
    if (ItemCanBeSold(tACDItem, ptACDNPC) != ISE_NONE)
    {

        return FALSE;
    }


    if (ItemIsSocketed(tACDItem))
    {

        return FALSE;
    }


    const ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    if (!tACDPlayer.IsAlive())
    {

        return FALSE;
    }
    return TRUE;
}
static BOOL sCValidatePlayerNearACD(const ActorCommonData & tACDPlayer, const ACDID idACDTarget, const float flMaxDist)
{



    const ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {

        return FALSE;
    }
    if (!ptACDTarget->IsInWorld())
    {

        return FALSE;
    }
    WorldPlace placePlayer = tACDPlayer.GetPlace();
    WorldPlace placeTarget = ptACDTarget->GetPlace();

    if (placePlayer.idSWorld != placeTarget.idSWorld)
    {

        return FALSE;
    }


    if (Vector3D::DistanceSquared(placePlayer.wp, placeTarget.wp) > flMaxDist * flMaxDist)
        return FALSE;
    return TRUE;
}





BOOL CValidatePlayerNearStash(const Player & tPlayer)
{

    const float OPERATE_DISTANCE = 15.0f;

    const ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    return sCValidatePlayerNearACD(tACDPlayer, tPlayer.idACDStashLastOperated, OPERATE_DISTANCE);
}


BOOL CValidatePlayerNearGizmo(const Player & tPlayer, const ActorCommonData & tACDGizmo)
{


    const ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    if (tACDPlayer.GetSWorldID() != tACDGizmo.GetSWorldID())
    {



        return FALSE;
    }
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    Target tTarget;
    tTarget.InitializeFromACD(tACDGizmo.id, FALSE, FALSE);
    WorldDistance wdAttackRange;
    PowerGetStartRange(tPlayer.idPlayerACD, AutoPowerDefinitionR(snoPower), tTarget, wdAttackRange);

    const float OPERATION_RADIUS_SLOP = 10.0f;
    wdAttackRange += OPERATION_RADIUS_SLOP;

    float flDistanceSquared = Vector3D::DistanceSquared(tACDPlayer.GetPlace().wp, tACDGizmo.GetPlace().wp);
    if (flDistanceSquared > wdAttackRange * wdAttackRange)
        return FALSE;
    return TRUE;
}


































































void CValidateDupeScan()
{


    SGame & tSGame = SGameManager::GameGet();
    ClientValidationGlobals *& ptClientValidationGlobals = tSGame.ptClientValidationGlobals;

    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        Actor * ptActor = tIt.ptElement;
        if (ptActor)
        {
            ActorCommonData * ptACD = ACDTryToGet(ptActor->idACD);
            if (ptACD && ptACD->IsItem())
            {
                ItemQualityLevel eQualityLevel = ItemGetQualityLevel(*ptACD);



                if (eQualityLevel >= 6 && eQualityLevel <= 9)
                {
                    ACDID idACDExisting;
                    ItemKey tKey(ptACD->hGB.gbid, ptActor->dwItemInitSeed);

                    if (ptClientValidationGlobals->tItemDupeMap.Lookup(tKey, idACDExisting))
                    {
                        ActorCommonData & tACDExisting = ACDGet(idACDExisting);











                    }
                    else
                    {
                        ptClientValidationGlobals->tItemDupeMap.SetAt_Tracked(tKey, ptACD->id);
                    }
                }
            }
        }
    }

    ptClientValidationGlobals->tItemDupeMap.RemoveAll();
}



BOOL CValidateItemIsInPlayerInventory(const Player & tPlayer, const ACDID idACDItem, BOOL bExcludeHirelingSlots)
{


    if (!CValidateIsACDValidItem(idACDItem))
    {

        return FALSE;
    }


    const ActorCommonData & tACDItem = ACDGet(idACDItem);


    const ActorLocation & tLocation = tACDItem.GetLocation();
    if (tLocation.eActorLocation != 1)
    {

        return FALSE;
    }




    if (tLocation.tInvLocation.eSlot == 14)
    {

        return FALSE;
    }




    if (tLocation.tInvLocation.eSlot == 15 && !CValidatePlayerNearStash(tPlayer))
    {

        return FALSE;
    }




    if (bExcludeHirelingSlots && tLocation.tInvLocation.eSlot >= 21 && tLocation.tInvLocation.eSlot <= 34)
    {

        return FALSE;
    }






    if (ItemIsGold(tACDItem))
    {

        return FALSE;
    }

    ACDID idACDOwner = tACDItem.GetInventoryContainerOwner();
    if (!CValidateDoesItemUltimatelyBelongToPlayer(tPlayer, idACDOwner))
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CValidateItemIsInPlayerInventoryForSocketing(const Player & tPlayer, const ACDID idACDItem)
{


    if (!CValidateIsACDValidItem(idACDItem))
    {

        return FALSE;
    }


    const ActorCommonData & tACDItem = ACDGet(idACDItem);


    const ActorLocation & tLocation = tACDItem.GetLocation();
    if (tLocation.eActorLocation != 1)
    {

        return FALSE;
    }




    if (tLocation.tInvLocation.eSlot == 14)
    {

        return FALSE;
    }




    if (tLocation.tInvLocation.eSlot == 15 && !CValidatePlayerNearCrafter(tPlayer, 1))
    {

        return FALSE;
    }




    if (ItemIsGold(tACDItem))
    {

        return FALSE;
    }



    ACDID idACDOwner = tACDItem.GetInventoryContainerOwner();
    if (!CValidateDoesItemUltimatelyBelongToPlayer(tPlayer, idACDOwner))
    {

        return FALSE;
    }
    return TRUE;
}




BOOL CValidateItemIsInPlayerStash(const Player & tPlayer, const ACDID idACDItem)
{


    if (!CValidateIsACDValidItem(idACDItem))
    {

        return FALSE;
    }


    const ActorCommonData & tACDItem = ACDGet(idACDItem);


    const ActorLocation & tLocation = tACDItem.GetLocation();
    if (tLocation.eActorLocation != 1)
    {

        return FALSE;
    }




    if (tLocation.tInvLocation.eSlot != 15)
    {

        return FALSE;
    }



    if (ItemIsGold(tACDItem))
    {
        return FALSE;
    }
    ACDID idACDOwner = tACDItem.GetInventoryContainerOwner();
    if (!CValidateDoesItemUltimatelyBelongToPlayer(tPlayer, idACDOwner))
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CValidateIsACDValidItemInTheWorld(const ACDID idACDItem)
{


    if (!CValidateIsACDValidItem(idACDItem))
    {


        return FALSE;
    }


    const ActorCommonData & tACDItem = ACDGet(idACDItem);
    const ActorLocation & tLocation = tACDItem.GetLocation();
    if (tLocation.eActorLocation != 0)
    {


        return FALSE;
    }
    return TRUE;
}
BOOL CValidateIsInventoryLocationValidToPlayer(const Player & tPlayer, const InvLoc & tLocation)
{


    if (tLocation.eSlot < 0 || tLocation.eSlot >= 35)
    {

        return FALSE;
    }






    IVector2D tSize;
    ACDInventorySlotGetSize(tLocation.eSlot, tSize);
    if (tLocation.nSlotX < 0 || tLocation.nSlotX >= tSize.x ||
        tLocation.nSlotY < 0 || tLocation.nSlotY >= tSize.y)
    {




        return FALSE;
    }

    ACDID idACDLocationOwner = ACDFromANN(tLocation.annOwner);
    const ActorCommonData * ptACDLocationOwner = ACDTryToGet(idACDLocationOwner);
    if (!ptACDLocationOwner)
    {


        return FALSE;
    }
    ACDID idACDOwner = ptACDLocationOwner->GetInventoryContainerOwner();
    if (!CValidateDoesItemUltimatelyBelongToPlayer(tPlayer, idACDOwner))
    {




        return FALSE;
    }
    InventoryType eType = ACDInventoryGetType(*ptACDLocationOwner);
    if (eType == INVENTORY_NONE)
    {




        return FALSE;
    }
    int32 nStart, nStop, nSlots;
    ACDInventoryGetSlotTypeInfo(eType, nStart, nStop, nSlots);
    if (tLocation.eSlot < nStart || tLocation.eSlot >= nStop)
    {




        return FALSE;
    }
    if (tLocation.eSlot == 14 || tLocation.eSlot == 16)
    {







        return FALSE;
    }
    if (tLocation.eSlot == 15 && !CValidatePlayerNearStash(tPlayer))
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CValidateIsInventoryLocationValidForQuickMove(const ActorCommonData & tACDItem, const InvLoc & tLoc)
{


    const ActorLocation & tLocation = tACDItem.GetLocation();
    InventorySlotType eSlotSrc = tLocation.tInvLocation.eSlot;

    BOOL bFromHireling = eSlotSrc >= 21 && eSlotSrc <= 34;




    if (eSlotSrc == 14 && !bFromHireling)
    {



        return FALSE;
    }





    if (tLoc.eSlot != 15 && tLoc.eSlot != 0)
    {

        return FALSE;
    }

    if (tLoc.eSlot != 0 && bFromHireling)
    {

        return FALSE;
    }


    if (eSlotSrc == tLoc.eSlot)
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CValidateTradeContentsStillInInventory(const Player & tPlayer)
{

    for (int32 i = 0; i < 4; i++)
    {

        if (tPlayer.tTradeData.arrItems[i] != ANN_INVALID && !CValidateItemIsInPlayerInventory(tPlayer, ACDFromANN(tPlayer.tTradeData.arrItems[i]), TRUE))
        {

            return FALSE;
        }
    }

    return TRUE;
}





BOOL CValidateANN(const ACDNetworkName ann)
{
    const ACDID idACD = ACDFromANN(ann);

    if (!ACDTryToGet(idACD))
    {




        return FALSE;
    }
    return TRUE;
}

BOOL CValidateANNAllowInvalid(const ACDNetworkName ann)
{
    if (ann == ANN_INVALID)
    {
        return TRUE;
    }

    const ACDID idACD = ACDFromANN(ann);
    if (!ACDTryToGet(idACD))
    {
        return FALSE;
    }
    return TRUE;
}





BOOL CValidateANNInWorldAllowInvalid(const ACDNetworkName ann)
{
    if (ann == ANN_INVALID)
        return TRUE;




    const ACDID idACD = ACDFromANN(ann);
    const ActorCommonData * ptACD = ACDTryToGet(idACD);





    if (!ptACD)
        return TRUE;
    if (!ptACD->IsInWorld())
    {



        return FALSE;
    }
    return TRUE;
}
BOOL CValidateWorldPlace(const WorldPlace & tWorldPlace)
{

    if (tWorldPlace.idSWorld == 0 &&

        tWorldPlace.wp.x == 0.0f &&
        tWorldPlace.wp.y == 0.0f &&
        tWorldPlace.wp.z == 0.0f)
    {

        return TRUE;
    }




    if (!isfinite(tWorldPlace.wp.x) ||
        !isfinite(tWorldPlace.wp.y) ||
        !isfinite(tWorldPlace.wp.z))
    {


        return FALSE;
    }



    if (tWorldPlace.idSWorld != SWORLDID_INVALID)
    {


        SWorld * ptSWorld = SWorldTryToGet(tWorldPlace.idSWorld);
        if (!ptSWorld)
        {



            return FALSE;
        }
    }


    if (tWorldPlace.wp.x < -100000.0f || tWorldPlace.wp.x > 100000.0f ||
        tWorldPlace.wp.y < -100000.0f || tWorldPlace.wp.y > 100000.0f ||
        tWorldPlace.wp.z < -100000.0f || tWorldPlace.wp.z > 100000.0f)
        return FALSE;
    return TRUE;
}
BOOL CValidateTargetPlace(const WorldPlace & tWorldPlace, const ActorCommonData & tACDCaster)
{


    if (!CValidateWorldPlace(tWorldPlace))
        return FALSE;





    if (tWorldPlace.idSWorld == 0)
        return TRUE;

    return tWorldPlace.idSWorld == tACDCaster.GetPlace().idSWorld;
}






BOOL CValidateTargetType(const TargetType eType)
{


    if (eType < 0 || eType >= 4)
    {




        return FALSE;
    }
    return TRUE;
}
BOOL CValidateGBID(const GameBalanceType eType, const GBID gbid)
{


    if (!GBIDExists(gbid, eType))
    {





        return FALSE;
    }
    return TRUE;
}

BOOL CValidateGBIDAllowNone(const GameBalanceType eType, const GBID gbid)
{


    if (gbid == GBID_NONE)
    {

        return TRUE;
    }
    if (!GBIDExists(gbid, eType))
    {





        return FALSE;
    }
    return TRUE;
}



BOOL CValidateSNO(const SNOGroup snoGroup, const SNO sno)
{


    SNOName snoname(snoGroup, sno);
    if (!SNOFileExists(snoname))
    {





        return FALSE;
    }
    return TRUE;
}
























BOOL CValidatePlayerIndex(const int32 nPlayerIndex)
{


    if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYERS)
    {




        return FALSE;
    }
    const Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (!ptPlayer)
    {

        return FALSE;
    }

    if (ptPlayer->idPlayerACD == ACDID_INVALID)
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CValidateUniString(const CHAR * puzString, const int32 nMaxLength)
{

    const CHAR * puzEndOfString = puzString + nMaxLength;



    while (puzString < puzEndOfString)
    {

        const CHAR * pszStringBeforeAdvance = puzString;
        uint32 nCodepoint = Blizzard::Unicode::GetCodepointFromUTF8(&puzString);

        if (nCodepoint == 0)
        {

            return TRUE;
        }



        if (nCodepoint == '|' && *puzString == 'H')
        {


            puzString = pszStringBeforeAdvance;
            if (HyperlinkValidate(puzString) == HLT_NONE)
            {




                return FALSE;
            }
        }
    }








    return FALSE;
}
BOOL CValidatePreplayAnimDataFlags(ActorCommonData & tACD, const PreplayedAnimData & tAnimData, const SNO snoPower)
{



    if (tAnimData.dwPrePlayedAnimFlags & 2)
    {

        if (!PowerAlternatesAnims(tACD, AutoPowerDefinitionR(snoPower)))
        {






            return FALSE;
        }
    }


    if (!(tAnimData.dwPrePlayedAnimFlags & 1))
        return FALSE;
    return TRUE;
}
BOOL CValidatePreplayAnimDataTimeStarted(const PreplayedAnimData & tAnimData)
{
    const uint32 dwPastSlopTicks = 300;
    const uint32 dwFutureSlopTicks = 60;



    uint32 dwNow = GameGetServerTime();

    if (tAnimData.dwServerTimeAnimStarted > dwNow && tAnimData.dwServerTimeAnimStarted - dwNow > dwFutureSlopTicks)
    {



        return FALSE;
    }
    if (dwNow > tAnimData.dwServerTimeAnimStarted && dwNow - tAnimData.dwServerTimeAnimStarted > dwPastSlopTicks)
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CValidatePowerUsableByPlayer(
    const ActorCommonData & tACD,
    const SNO snoPower)
{

    if (PowerIsPassive(AutoPowerDefinitionR(snoPower)))
    {
        return FALSE;
    }



    SNO snoUtilityKit = GlobalSNOGet(GLOBALSNO_CONSOLE_UTILITY_KIT);
    ScopedSkillKitDefinitionLockR pdefKit(snoUtilityKit);
    int32 nCount = pdefKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    for (int32 i = 0; i < nCount; i++)
    {
        ActiveSkillEntry & tEntry = pdefKit->arActiveSkillEntries[i];
        if (tEntry.snoPower == snoPower)
        {
            return TRUE;
        }
    }


    BOOL bSkillOverridesPresent =
        tACD.AttributesGetInt(ATTRIBUTE_SKILL_OVERRIDE_ACTIVE) &&
        GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL) != snoPower;




    if (PowerIsEmote(AutoPowerDefinitionR(snoPower)))
    {
        return TRUE;
    }

    if (GlobalSNOGet(GLOBALSNO_POWERS_SELECTING_SKILL) == snoPower)
    {
        return TRUE;
    }

    if (bSkillOverridesPresent)
    {
        return tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB1)) == snoPower ||
               tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB2)) == snoPower ||
               tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB3)) == snoPower ||
               tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_HB4)) == snoPower ||
               tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_LMB)) == snoPower ||
               tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE, SKILL_RMB)) == snoPower;
    }



    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower)) > 0)
    {
        return TRUE;
    }

    if (GlobalSNOGet(GLOBALSNO_POWERS_PUNCH) == snoPower)
    {
        return TRUE;
    }

    ACDID idACDWeapon = ACDInventoryGetPrimaryWeapon(tACD.id);
    if (idACDWeapon != ACDID_INVALID)
    {
        const ActorCommonData & tACDWeapon = ACDGet(idACDWeapon);
        if (!ItemIsBroken(tACDWeapon))
        {
            SNO snoWeaponPower = tACDWeapon.AttributesGetPower(0);
            if (snoWeaponPower == snoPower)
                return TRUE;
        }
    }



    ACDID idACDSecondaryWeapon = ACDInventoryGetSecondaryWeaponOrShield(tACD.id);
    if (idACDSecondaryWeapon != ACDID_INVALID)
    {
        const ActorCommonData & tACDWeapon = ACDGet(idACDSecondaryWeapon);
        if (!ItemIsBroken(tACDWeapon))
        {
            SNO snoWeaponPower = tACDWeapon.AttributesGetPower(0);
            if (snoWeaponPower == snoPower)
                return TRUE;
        }
    }



    return FALSE;
}


BOOL CValidateEndGameRequest(const SNO snoSentinel, const Player & tPlayer)
{


    Act eMaxAct = SPlayerGetMaxAct(tPlayer);
    if (tPlayer.tHeroStateData.eLastPlayedAct != eMaxAct)
        return FALSE;




    uint32 uContentFlags = SPlayerGetContentFlags(tPlayer);

    SNO snoEndConversation = ConversationGetEndGameConversation(uContentFlags);
    if (snoEndConversation != snoSentinel)
        return FALSE;










    uint32 uEndGameFlag;





    if (!(SGameGlobalsGet().dwFlags & (uEndGameFlag = eMaxAct == ACT_5 ? 0x80000 : 0x40000)))
        return FALSE;

    SNO snoEndGameLevelArea = GlobalSNOGet(GLOBALSNO_LEVELAREA_END_GAME_VANILLA);
    if (uContentFlags & 0x10)
        snoEndGameLevelArea = GlobalSNOGet(GLOBALSNO_LEVELAREA_END_GAME_X1);


    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
    SNO snoLevelArea = ACDGetLevelArea(tACDPlayer);
    if (snoLevelArea != snoEndGameLevelArea)
        return FALSE;
    return TRUE;
}
// NON_MATCHING: the original hoists one "return FALSE" value above the first compare and keeps a second one for the conversation check (4 bytes longer)
BOOL CValidateActTransitionRequest(
    const SNO snoSentinel,
    const Player & tPlayer)
{
    Act eMaxAct = SPlayerGetMaxAct(tPlayer);
    Act eCurrAct = GameGetCurrentAct();
    if (eCurrAct >= eMaxAct || tPlayer.tHeroStateData.eLastPlayedAct != eCurrAct)
    {




        return FALSE;
    }


    OnlineService::ContentLicenseId eContentLicenseId = OnlineService::CONTENT_LICENSE_ADVMODE_ACTV;
    uint32 uEndGameFlag = 19;
    if (eCurrAct != ACT_5)
    {
        eContentLicenseId = OnlineService::CONTENT_LICENSE_ACT1_ACT4;
        uEndGameFlag = 18;
    }

    SNO snoEndConversation = ConversationGetEndGameConversation(1 << eContentLicenseId);
    if (snoEndConversation != snoSentinel)
    {
        return FALSE;
    }

    if ((SGameGlobalsGet().dwFlags & (1 << uEndGameFlag)) == 0)
    {
        return FALSE;
    }

    return TRUE;
}


BOOL CValidatePlayerHasPromoDyes(const Player & tPlayer)
{

    uint32 uLicenses = SPlayerGetConsoleLicenseFlags(tPlayer);

    if (uLicenses & 1)
        return TRUE;





























    return FALSE;
}
