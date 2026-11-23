// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdCommerce/bdCommerce.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCommerceCurrency;
class bdCommerceGiftItem;
class bdCommerceItem;
class bdCommerceItemQuantity;
class bdCommerceReceiptStatus;
class bdCommerceWriter;
class bdRemoteTaskManager;

typedef bdUInt32 bdGiftID;  // bdCommerceGiftItem.h, not adopted yet
typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdCommerce {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdCommerce);
    bdCommerce(bdRemoteTaskManager *const);
    ~bdCommerce();
    bdRemoteTaskRef getBalances(bdCommerceCurrency *, const bdUByte8);
    bdRemoteTaskRef deposit(const bdCommerceCurrency &, const void *, const bdUInt32, bdUInt64 &, const bdUInt64, const bdUByte8);
    bdRemoteTaskRef modifyBalances(const bdCommerceCurrency *, const bdUInt32, bdUInt64 &, const bdUInt64);
    bdRemoteTaskRef setBalances(const bdCommerceCurrency *, const bdUInt16, const bdUInt64);
    bdRemoteTaskRef migrateBalances(const bdOnlineUserID, const bdUInt64);
    bdRemoteTaskRef setWriter(const bdCommerceWriter);
    bdRemoteTaskRef getWriter(const bdUInt64, bdCommerceWriter &);
    bdRemoteTaskRef getWriters(bdCommerceWriter *, const bdUByte8);
    bdRemoteTaskRef getLastWriter(bdCommerceWriter &);
    bdRemoteTaskRef validateReceipt(const void *, const bdUInt32, bdCommerceReceiptStatus *const, const bdUByte8, const void *, const bdUInt32);
    bdRemoteTaskRef getItems(const bdUInt64, bdCommerceItem *, const bdUInt16, const bdUInt16);
    bdRemoteTaskRef getGiftsOfferedToUser(const bdUInt64, bdCommerceGiftItem *, const bdUInt16, const bdUInt16);
    bdRemoteTaskRef getGiftsOfferedByUser(const bdUInt64, bdCommerceGiftItem *, const bdUInt16, const bdUInt16);
    bdRemoteTaskRef retractGiftOffers(const bdGiftID *, const bdUInt16, bdCommerceItem *, bdUInt64, const bdUInt64);
    bdRemoteTaskRef acceptGifts(const bdGiftID *, const bdUInt16, bdCommerceItem *, const bdUInt16, bdUInt64, const bdUInt64);
    bdRemoteTaskRef rejectGifts(const bdGiftID *, const bdUInt16, bdUInt64, const bdUInt64);
    bdRemoteTaskRef purchaseItems(const bdCommerceCurrency *, const bdUInt16, bdCommerceItem *, const bdUInt16, bdUInt64 &, const bdUInt64);
    bdRemoteTaskRef consumeItems(bdCommerceItemQuantity *, const bdUInt16, bdUInt64 &, const bdUInt64);
    bdRemoteTaskRef giftItems(const bdCommerceItemQuantity *, const bdUInt16, const bdOnlineUserID, bdUInt64 &, const bdUInt64);
    bdRemoteTaskRef setInventory(bdCommerceItem *, const bdUInt16, const bdUInt64);
    bdRemoteTaskRef setItems(bdCommerceItem *, const bdUInt16, const bdUInt64);
    bdRemoteTaskRef setItemQuantities(bdCommerceItemQuantity *, const bdUInt16, const bdUInt64);
    bdRemoteTaskRef transferInventory(const bdOnlineUserID, const bdUInt64);
    bdRemoteTaskRef consolidateItems(bdUInt64 *, const bdUInt16, bdCommerceItemQuantity *, const bdUInt64);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdCommerce) == 0x8, "bdCommerce");


// Functions defined in this file (bodies are not in the DWARF):
//   line    64  void bdCommerce::operator delete(void * p)  [inlined into 1 functions]
//   line    64  void * bdCommerce::operator new(size_t nbytes)  [inline]
