// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMarketplace/bdMarketplace.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdApplyConversionRuleRequest;
class bdApplyConversionRuleResponse;
class bdExchangeRevokeResult;
class bdExchangeRevokeResultV2;
class bdExchangeTransactionMultipleResults;
class bdExchangeTransactionResult;
class bdExchangeTransactionResultV2;
class bdGetConversionRulesByIDsRequest;
class bdGetConversionRulesByKindRequest;
class bdGetConversionRulesRequest;
class bdGetConversionRulesResponse;
class bdGetPlayersBalancesRequest;
class bdGetPlayersBalancesResponse;
class bdMarketplaceAuditLogResult;
class bdMarketplaceCouponInventory;
class bdMarketplaceCouponInventoryV2;
class bdMarketplaceCouponInventoryV3;
class bdMarketplaceCouponInventoryV4;
class bdMarketplaceCurrency;
class bdMarketplaceDepositResult;
class bdMarketplaceDiscount;
class bdMarketplaceDiscountInfo;
class bdMarketplaceEntitlement;
class bdMarketplaceFPSku;
class bdMarketplaceInventory;
class bdMarketplaceInventoryItemData;
class bdMarketplaceInventoryItemIdResult;
class bdMarketplaceInventoryItemInfo;
class bdMarketplaceInventoryItemQuantity;
class bdMarketplaceItem;
class bdMarketplaceItemQuantity;
class bdMarketplaceItemToPawn;
class bdMarketplaceItemToPawnV2;
class bdMarketplaceItemTombstone;
class bdMarketplaceLimitedSkuQuantity;
class bdMarketplacePawnChoice;
class bdMarketplacePlayerCurrency;
class bdMarketplacePlayerData;
class bdMarketplaceProduct;
class bdMarketplaceProductTombstone;
class bdMarketplaceProductV2;
class bdMarketplacePurchaseSkusCouponOrder;
class bdMarketplacePurchaseSkusCouponOrderV2;
class bdMarketplacePurchaseSkusResult;
class bdMarketplacePurchaseSkusResultV2;
class bdMarketplacePurchaseSkusResultV3;
class bdMarketplacePurchaseSkusResultV4;
class bdMarketplacePurchaseSkusSkuOrder;
class bdMarketplacePurchaseSkusSkuOrderV2;
class bdMarketplaceReceiptStatus;
class bdMarketplaceReward;
class bdMarketplaceSku;
class bdMarketplaceSkuTombstone;
class bdMarketplaceSkuV2;
class bdMarketplaceSkuV3;
class bdMarketplaceUsageTime;
class bdMarketplaceVersion;
class bdPurchaseWiiUInitV2Request;
class bdPurchaseWiiUInitV2Response;
class bdRemoteTaskManager;
class bdMarketplaceCouponInventoryResult;  // their headers are not adopted yet
class bdMarketplaceCouponInventoryResultV4;
class bdStructBufferTask;
typedef bdReference<bdStructBufferTask> bdStructBufferTaskRef;
class bdSetPlayersBalancesRequest;
class bdSetPlayersBalancesResponse;
class bdSteamOrderIdResult;
class bdSteamOrderIdResultV2;
class bdStringResult;
class bdStructBufferSerializable;
class bdTaskParams;
class bdValidationTokenResult;
class bdWiiUPrePurchaseResult;
class bdWiiUProcessDurableV2Request;
class bdWiiUProcessDurableV2Response;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdMarketplace {  // size 0x18
public:
    bdRemoteTaskRef getProducts(bdMarketplaceProduct *, const bdUInt16);
    bdRemoteTaskRef getProducts(bdMarketplaceProduct *, const bdUInt16, const bdUInt32 *, const bdUByte8);
    bdRemoteTaskRef getProductsPaginated(bdMarketplaceProduct *, const bdUInt32, const bdUInt32, const bdUInt32 *, const bdUByte8, const bdNChar8 *);
    bdRemoteTaskRef getSkus(bdMarketplaceSku *, const bdUInt16, const bdBool, const bdUInt32 *, const bdUByte8);
    bdRemoteTaskRef getSkusPaginated(bdMarketplaceSku *, const bdUInt32, const bdUInt32, const bdBool, const bdUInt32 *, const bdUInt32);
    bdRemoteTaskRef getSkusPaginated(bdMarketplaceSkuV2 *, const bdUInt32, const bdUInt32, const bdBool, const bdUInt32 *, const bdUInt32, const bdUByte8 *, const bdUInt32, const bdNChar8 *);
    bdRemoteTaskRef getItemsPaginated(bdMarketplaceItem *, const bdUInt32, const bdUInt32, const bdBool, const bdUInt32 *, const bdUInt32);
    bdRemoteTaskRef deposit(bdMarketplacePlayerCurrency *, bdUByte8);
    bdRemoteTaskRef getBalance(bdMarketplaceCurrency *, const bdUInt32);
    bdRemoteTaskRef getPlayersBalances(bdUserID *, bdUInt32, bdMarketplacePlayerCurrency *, const bdUInt32);
    bdRemoteTaskRef getPlayersBalancesV2(bdUserID *, bdUInt32, bdMarketplacePlayerCurrency *, const bdUInt32);
    bdRemoteTaskRef setPlayersBalances(bdMarketplacePlayerCurrency *, bdUByte8);
    bdRemoteTaskRef setPlayersBalancesV2(bdMarketplacePlayerCurrency *, const bdUInt32, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef getInventory(bdMarketplaceInventory *, const bdUInt16);
    bdRemoteTaskRef getInventoryPaginated(bdMarketplaceInventory *, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef putInventoryItemData(const bdMarketplaceInventoryItemData &);
    bdRemoteTaskRef consumeInventoryItem(const bdUserID, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef consumeInventoryItems(const bdUInt32 *, const bdUInt32 *, const bdUInt16);
    BD_CLASS_ALLOCATOR(bdMarketplace);
    bdMarketplace(bdRemoteTaskManager *const, const bdNChar8 *);
    ~bdMarketplace();
    bdRemoteTaskRef putInventoryItem(const bdMarketplaceInventory &);
    bdRemoteTaskRef getProductsPaginated(bdMarketplaceProductV2 *, const bdUInt32, const bdUInt32, const bdUInt32 *, const bdUByte8, const bdNChar8 *);
    bdRemoteTaskRef consumeInventoryItems(const bdMarketplaceInventoryItemQuantity *, const bdUInt16);
    bdRemoteTaskRef putInventoryItemsData(const bdMarketplaceInventoryItemData *, const bdUByte8);
    bdRemoteTaskRef getDeletedProducts(bdMarketplaceProductTombstone *, const bdUInt32, const bdUInt32, const bdNChar8 *);
    bdRemoteTaskRef putPlayersInventoryItems(const bdMarketplaceInventory *, const bdUByte8, const bdBool);
    bdRemoteTaskRef getSkusPaginated(bdMarketplaceSkuV3 *, const bdUInt32, const bdUInt32, const bdBool, const bdUInt32 *, const bdUInt32, const bdUByte8 *, const bdUInt32, const bdNChar8 *);
    bdRemoteTaskRef getCouponInventory(bdMarketplaceCouponInventory *, const bdUInt16);
    bdRemoteTaskRef getCouponInventoryV2(bdMarketplaceCouponInventoryV3 *, const bdUInt16);
    bdRemoteTaskRef getLimitedSkuQuantites(bdMarketplaceLimitedSkuQuantity *, const bdUInt32 *, const bdUInt32);
    bdRemoteTaskRef getPendingCouponInventory(bdMarketplaceCouponInventory *, const bdUInt16);
    bdRemoteTaskRef getLimitedSkuQuantities(bdMarketplaceLimitedSkuQuantity *, const bdUInt32 *, const bdUInt32);
    bdRemoteTaskRef claimCoupon(const bdNChar8 (*)[25], const bdUByte8, bdMarketplaceCouponInventory *, const bdUInt32);
    bdRemoteTaskRef transferCoupon(const bdUserID *, const bdNChar8 (*)[25], const bdUByte8);
    bdRemoteTaskRef getDeletedSkus(bdMarketplaceSkuTombstone *, const bdUInt32, const bdUInt32, const bdNChar8 *);
    bdRemoteTaskRef redeemCoupon(bdNChar8 (*)[25], bdUByte8);
    bdRemoteTaskRef deleteSpecifiedCoupons(bdNChar8 (*)[25], bdUByte8);
    bdRemoteTaskRef getItemsPaginated(bdMarketplaceItem *, const bdUInt32, const bdUInt32, const bdBool, const bdUInt32 *, const bdUInt32, const bdNChar8 *);
    bdRemoteTaskRef purchaseSkus(const bdUInt32 *, const bdUInt32 *, const bdUByte8);
    bdRemoteTaskRef getDeletedItems(bdMarketplaceItemTombstone *, const bdUInt32, const bdUInt32, const bdNChar8 *);
    bdRemoteTaskRef purchaseSkusForPlayer(const bdUserID, const bdUInt32 *, const bdUInt32 *, const bdUByte8);
    bdRemoteTaskRef getDiscountsForSkuIDs(bdMarketplaceDiscount *, bdUInt32, bdUInt32, const bdUInt32 *, bdUInt32, bdUInt32, bdUInt32);
    bdRemoteTaskRef purchaseSkus(const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdBool);
    bdRemoteTaskRef getDiscountsForPlayer(bdMarketplaceDiscountInfo *, bdUInt32, bdUInt32, bdUInt32, bdUInt32);
    bdRemoteTaskRef purchaseSkusForPlayer(const bdUserID, const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdBool);
    bdRemoteTaskRef getRewards(const bdUInt32 *, bdUInt32, bdMarketplaceReward *, bdUInt32);
    bdRemoteTaskRef purchaseSkus(const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdBool, const bdUserID *, bdMarketplaceCouponInventory *, const bdUInt32, const bdUByte8 **, const bdUInt32 *, const bdUByte8, const bdUInt32 *);
    bdRemoteTaskRef getBalanceV2(bdMarketplaceCurrency *, const bdUInt32);
    bdRemoteTaskRef getPlayersBalancesV3(bdGetPlayersBalancesRequest *, bdGetPlayersBalancesResponse *);
    bdRemoteTaskRef purchaseSkusForPlayer(const bdUserID, const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdBool, const bdUserID *, bdMarketplaceCouponInventory *, const bdUInt32, const bdUByte8 **, const bdUInt32 *, const bdUByte8, const bdUInt32 *);
    bdRemoteTaskRef setPlayersBalancesV3(bdSetPlayersBalancesRequest *, bdSetPlayersBalancesResponse *);
    bdRemoteTaskRef setPlayersBalancesV2(bdMarketplacePlayerCurrency *, const bdUInt32, const bdUInt32, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef depositV3(const bdNChar8 *, bdMarketplacePlayerCurrency *, const bdUInt32, bdMarketplaceDepositResult *);
    bdRemoteTaskRef purchaseSkus(const bdNChar8 *, const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdBool, const bdUserID *, const bdUInt32, const bdUByte8 **, const bdUInt32 *, const bdUByte8, bdMarketplaceCouponInventoryResult *);
    bdRemoteTaskRef depositV3(const bdNChar8 *, bdMarketplacePlayerCurrency *, const bdUInt32, const bdUInt32, bdMarketplaceDepositResult *);
    bdRemoteTaskRef getInventoryPaginated(bdMarketplaceInventory *, const bdUInt32, const bdUInt32, const bdUInt32 *, const bdUInt16);
    bdRemoteTaskRef purchaseSkus(const bdNChar8 *, const bdUByte8, const bdUInt32 *, const bdUInt32 *, const bdUInt32, bdMarketplacePurchaseSkusResult *, const bdUInt32 *, const bdUserID *, const bdUByte8 **, const bdUInt32 *, const bdUByte8, const bdBool);
    bdRemoteTaskRef getInventoryItemIDs(bdMarketplaceInventoryItemIdResult *, const bdUInt16);
    bdRemoteTaskRef getInventoryByItemIDs(const bdUInt32 *, const bdUInt32, bdMarketplaceInventory *, const bdUInt16);
    bdRemoteTaskRef purchaseSkus(const bdNChar8 *, const bdUserID, const bdUByte8, const bdMarketplacePurchaseSkusSkuOrder *, const bdUByte8, const bdMarketplacePurchaseSkusCouponOrder *, bdMarketplacePurchaseSkusResultV2 *, const bdUByte8, const bdBool);
    bdRemoteTaskRef validateInventoryItemsToken(const bdNChar8 *const, const bdValidationTokenResult &, bdValidationTokenResult *);
    bdRemoteTaskRef getInventoryItemInfo(bdMarketplaceInventoryItemInfo *, const bdUInt32, const bdUInt32, const bdUInt32 *, const bdUInt32);
    bdRemoteTaskRef purchaseSkus(const bdNChar8 *, const bdUserID, const bdUByte8, const bdMarketplacePurchaseSkusSkuOrderV2 *, const bdUByte8, const bdMarketplacePurchaseSkusCouponOrderV2 *, bdMarketplacePurchaseSkusResultV2 *, const bdUByte8, const bdBool);
    bdRemoteTaskRef putInventoryItemsData(const bdNChar8 *, const bdMarketplaceInventoryItemData *, const bdUByte8, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef giftSkuTransaction(const bdUByte8 *, const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdOnlineUserID);
    bdRemoteTaskRef putPlayersInventoryItems(const bdNChar8 *, const bdMarketplaceInventory *, const bdUByte8, bdMarketplaceAuditLogResult *, const bdBool);
    bdRemoteTaskRef pawnItem(const bdUInt32);
    bdRemoteTaskRef consumeInventoryItems(const bdNChar8 *, const bdUInt32 *, const bdUInt32 *, const bdUInt16, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef pawnItem(const bdUInt32, const bdMarketplacePawnChoice *, const bdUByte8);
    bdRemoteTaskRef getPlayersInventories(const bdUserID *, const bdUByte8, bdMarketplaceInventory *, const bdUInt16);
    bdRemoteTaskRef deleteInventory();
    bdRemoteTaskRef pawnItem(const bdNChar8 *, const bdUInt32, const bdMarketplacePawnChoice *, const bdUByte8);
    bdRemoteTaskRef getExpiredInventoryItems();
    bdRemoteTaskRef deleteExpiredInventoryItems(const bdUInt32 *, const bdUInt16);
    bdRemoteTaskRef pawnItems(const bdNChar8 *, const bdMarketplaceItemToPawn *, const bdUInt32, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef donatePlayerInventory(const bdUserID, const bdMarketplaceItemQuantity *, const bdUInt32);
    bdRemoteTaskRef purchaseOnSteamInitialize(const bdNChar8 *, bdSteamOrderIdResult *);
    bdRemoteTaskRef donatePlayerInventory(const bdNChar8 *, const bdUserID, const bdMarketplaceItemQuantity *, const bdUInt32);
    bdRemoteTaskRef purchaseOnSteamInitialize(const bdNChar8 *, const bdUInt32, bdSteamOrderIdResult *);
    bdRemoteTaskRef putPlayersEntitlements(const bdMarketplaceEntitlement *, const bdUByte8);
    bdRemoteTaskRef purchaseOnSteamInitialize(const bdNChar8 *, const bdUInt32, const bdUserID, bdSteamOrderIdResult *);
    bdRemoteTaskRef getPlayersEntitlements(const bdUserID *, const bdUByte8, bdMarketplaceEntitlement *, const bdUInt16);
    bdRemoteTaskRef purchaseOnSteamFinalize(const bdNChar8 *, bdExchangeTransactionResult *);
    bdRemoteTaskRef getEntitlements(bdMarketplaceEntitlement *);
    bdRemoteTaskRef purchaseOnSteamFinalize(const bdNChar8 *, const bdUInt64, const bdUInt32, const bdBool, bdExchangeTransactionResult *);
    bdRemoteTaskRef deletePlayersEntitlements(const bdMarketplaceEntitlement *, const bdUInt32);
    enum bdCouponType {
        BD_NEW_COUPONS = 0,
        BD_CLAIMED_COUPONS = 1,
        BD_ALL_COUPONS = 2,
    };
    bdRemoteTaskRef steamProcessDurable(const bdNChar8 *, const bdUInt32, bdExchangeTransactionResult *);
    bdRemoteTaskRef getCouponsByType(bdMarketplaceCouponInventoryV2 *, const bdMarketplace::bdCouponType, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef steamProcessDurable(const bdNChar8 *, const bdUInt32, const bdUserID, bdExchangeTransactionResult *);
    bdRemoteTaskRef getCouponsByTypeV2(bdMarketplaceCouponInventoryV4 *, const bdMarketplace::bdCouponType, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef steamRevokeProduct(const bdNChar8 *, const bdUInt32, bdExchangeRevokeResult *);
    bdRemoteTaskRef getPendingCouponInventoryV2(bdMarketplaceCouponInventory *, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef psnProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResult *);
    bdRemoteTaskRef getPendingCouponInventoryV3(bdMarketplaceCouponInventoryV3 *, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef getPendingCouponInventoryV4(bdMarketplaceCouponInventoryV4 *, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef psnProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResult *);
    bdRemoteTaskRef claimCoupon(const bdNChar8 *, const bdNChar8 (*)[25], const bdUByte8, const bdUInt32, bdMarketplaceCouponInventoryResult *);
    bdRemoteTaskRef psnRevokeProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeRevokeResult *);
    bdRemoteTaskRef claimCoupon(const bdNChar8 *, const bdNChar8 (*)[25], const bdUByte8, const bdUInt32, bdMarketplaceCouponInventoryResultV4 *);
    bdRemoteTaskRef xbox360ProcessProduct(const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResult *);
    bdRemoteTaskRef transferCoupon(const bdNChar8 *, const bdUserID *, const bdNChar8 (*)[25], const bdUByte8, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef xbox360ProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResult *);
    bdRemoteTaskRef redeemCoupon(const bdNChar8 *, const bdNChar8 (*)[25], const bdUByte8, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef xbox360RevokeProduct(const bdNChar8 *, const bdNChar8 *, bdExchangeRevokeResult *);
    bdRemoteTaskRef deleteSpecifiedCoupons(const bdNChar8 *, const bdNChar8 (*)[25], const bdUByte8, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef xboxOneTransferConsumable(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResult *);
    bdRemoteTaskRef giftCoupon(const bdUserAccountID *, const bdUInt32 *, const bdUByte8, const bdUByte8 **, const bdUInt32 *, const bdUByte8 *, const bdUInt32 *, const bdUInt32 *);
    bdRemoteTaskRef xboxOneTransferConsumable(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResult *);
    bdRemoteTaskRef xboxOneProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResult *);
    bdRemoteTaskRef giftCouponV2(const bdUserAccountID *, const bdUInt32 *, const bdUByte8, const bdUByte8 **, const bdUInt32 *, const bdUInt32 *, const bdUInt32 *, const bdUInt32 *);
    bdRemoteTaskRef xboxOneProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResult *);
    bdRemoteTaskRef xboxOneRevokeProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeRevokeResult *);
    bdRemoteTaskRef purchaseSkus(const bdNChar8 *, const bdUserID, const bdUByte8, const bdMarketplacePurchaseSkusSkuOrderV2 *, const bdUByte8, const bdMarketplacePurchaseSkusCouponOrderV2 *, bdMarketplacePurchaseSkusResultV3 *, const bdUByte8, const bdBool);
    bdRemoteTaskRef purchaseOnWiiUInitialize(const bdNChar8 *, const bdUInt64, const bdUInt32, const bdNChar8 *, bdWiiUPrePurchaseResult *);
    bdRemoteTaskRef wiiUProcessDurable(const bdNChar8 *, const bdUInt64, const bdUInt32, const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef purchaseSkus(const bdNChar8 *, const bdUserID, const bdUByte8, const bdMarketplacePurchaseSkusSkuOrderV2 *, const bdUByte8, const bdMarketplacePurchaseSkusCouponOrderV2 *, bdMarketplacePurchaseSkusResultV4 *, const bdUInt32, const bdBool);
    bdRemoteTaskRef getFPSkusPaginated(bdMarketplaceFPSku *, const bdUInt32, const bdUInt32, const bdBool, const bdNChar8 *);
    bdRemoteTaskRef giftSkuTransaction(const bdUByte8 *, const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdOnlineUserID, const bdUByte8);
    bdRemoteTaskRef getFPSkusPaginated(bdMarketplaceFPSku *, const bdUInt32, const bdUInt32, const bdBool);
    bdRemoteTaskRef giftSkuTransactionV2(const bdUByte8 *, const bdUInt32 *, const bdUInt32 *, const bdUByte8, const bdOnlineUserID, const bdUInt32);
    bdRemoteTaskRef validateReceipt(const void *, const bdUInt32, bdMarketplaceReceiptStatus *const, const bdUByte8, const void *, const bdUInt32);
    bdRemoteTaskRef pawnItems(const bdNChar8 *, const bdMarketplaceItemToPawnV2 *, const bdUInt32, bdMarketplaceAuditLogResult *);
    bdRemoteTaskRef getPlayerData(bdMarketplacePlayerData *);
    bdRemoteTaskRef getAllPlayerData(const bdUserID *, const bdUInt32, bdMarketplacePlayerData *, const bdUInt32);
    bdRemoteTaskRef getUsageTime(bdMarketplaceUsageTime *);
    bdRemoteTaskRef getPlayersUsageTimes(bdMarketplaceUsageTime *, const bdUInt16);
    bdRemoteTaskRef reportUsageTime(const bdUInt32);
    bdRemoteTaskRef reportPlayersUsageTimes(const bdMarketplaceUsageTime *, const bdUInt16);
    bdRemoteTaskRef startExchangeTransaction(bdStringResult *);
    bdRemoteTaskRef purchaseOnSteamInitialize(const bdNChar8 *, bdSteamOrderIdResultV2 *);
    bdRemoteTaskRef purchaseOnSteamInitialize(const bdNChar8 *, const bdUInt32, bdSteamOrderIdResultV2 *);
    bdRemoteTaskRef purchaseOnSteamInitialize(const bdNChar8 *, const bdUInt32, const bdUserID, bdSteamOrderIdResultV2 *);
    bdRemoteTaskRef purchaseOnSteamFinalize(const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef purchaseOnSteamFinalize(const bdNChar8 *, const bdUInt64, const bdUInt32, const bdBool, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef steamProcessDurable(const bdNChar8 *, const bdUInt32, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef steamProcessDurable(const bdNChar8 *, const bdUInt32, const bdUserID, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef steamRevokeProduct(const bdNChar8 *, const bdUInt32, bdExchangeRevokeResultV2 *);
    bdRemoteTaskRef psnProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef psnProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef psnRevokeProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeRevokeResultV2 *);
    bdRemoteTaskRef xbox360ProcessProduct(const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef xbox360ProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef xbox360RevokeProduct(const bdNChar8 *, const bdNChar8 *, bdExchangeRevokeResultV2 *);
    bdRemoteTaskRef xboxOneTransferConsumable(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef xboxOneTransferConsumable(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef xboxOneProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef xboxOneProcessProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, const bdUserID, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef xboxOneRevokeProduct(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *, bdExchangeRevokeResultV2 *);
    bdRemoteTaskRef purchaseOnWiiUInitializeV2(bdPurchaseWiiUInitV2Request *, bdPurchaseWiiUInitV2Response *);
    bdRemoteTaskRef purchaseOnWiiUFinalize(const bdNChar8 *, const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef wiiUProcessDurableV2(bdWiiUProcessDurableV2Request *, bdWiiUProcessDurableV2Response *);
    bdRemoteTaskRef wiiUProcessConsumable(const bdNChar8 *, const bdUInt64, const bdUInt32, const bdNChar8 *, bdExchangeTransactionResultV2 *);
    bdRemoteTaskRef iTunesProcessProduct(const bdNChar8 *, const void *, const bdUInt32, bdExchangeTransactionMultipleResults *);
    bdRemoteTaskRef getFPSkusPaginated(bdMarketplaceFPSku *, const bdUInt32, const bdUInt32, const bdBool, const bdNChar8 *, const bdUInt32 *, const bdUInt32);
    bdRemoteTaskRef verifyReceiptV2(const bdUByte8, const bdNChar8 *, const bdNChar8 *, bdMarketplaceReceiptStatus *const);
    bdRemoteTaskRef getConversionRules(bdGetConversionRulesRequest *, bdGetConversionRulesResponse *);
    bdRemoteTaskRef getConversionRules(bdGetConversionRulesByKindRequest *, bdGetConversionRulesResponse *);
    bdRemoteTaskRef getConversionRules(bdGetConversionRulesByIDsRequest *, bdGetConversionRulesResponse *);
    bdRemoteTaskRef applyConversionRule(bdApplyConversionRuleRequest *, bdApplyConversionRuleResponse *);
    bdRemoteTaskRef getMarketplaceVersion(bdMarketplaceVersion *);
private:
    bdRemoteTaskRef getCouponInventory(bdMarketplaceCouponInventory *, const bdUInt16, const bdUByte8);
protected:
    void startTask(bdRemoteTaskRef & remoteTask, bdTaskParams & params);
    void startStructTask(bdStructBufferTaskRef & remoteTask, const bdUByte8 taskID, bdStructBufferSerializable * request, bdStructBufferSerializable * response);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdNChar8 m_context[16];  // 0x08
};
static_assert(sizeof(bdMarketplace) == 0x18, "bdMarketplace");


// Functions defined in this file (bodies are not in the DWARF):
//   line   375  void bdMarketplace::operator delete(void * p)  [inlined into 1 functions]
//   line   375  void * bdMarketplace::operator new(size_t nbytes)  [inline]
