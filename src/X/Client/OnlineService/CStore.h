// Types adopted from Hyperlink CU 0x0427bd30; original X/Client/OnlineService/CStore.h.
#pragma once
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Common/GameBalance.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/OnlineService/Store.h"
namespace D3 { namespace Store { class Product; class ProductEntitlement; } }
namespace OnlineService {






















typedef uint32 StoreProductId;

typedef int64 StoreItemPriceType;



typedef int32 StoreProductAttributes;
enum {
    STORE_ATTRIBUTE_INVALID = -1,
    STORE_ATTRIBUTE_FEATURED_BANNER = 0,
    STORE_ATTRIBUTE_FEATURED_SQUARE_1 = 1,
    STORE_ATTRIBUTE_FEATURED_SQUARE_2 = 2,
    STORE_ATTRIBUTE_FEATURED_SQUARE_3 = 3,
    STORE_ATTRIBUTE_FEATURED_LIST_1 = 4,
    STORE_ATTRIBUTE_FEATURED_LIST_2 = 5,
    STORE_ATTRIBUTE_FEATURED_LIST_3 = 6,
    STORE_ATTRIBUTE_FEATURED_LIST_4 = 7,
    STORE_ATTRIBUTE_FEATURED_LIST_5 = 8,
    STORE_ATTRIBUTE_FEATURED_LIST_6 = 9,
    STORE_ATTRIBUTE_FEATURED_START = 0,
    STORE_ATTRIBUTE_FEATURED_END = 9,
    STORE_ATTRIBUTE_FEATURED_LIST_MAX = 6,
};



typedef int32 StoreItemEntitlementType;
enum {
    STORE_ENTITLEMENT_INVALID = -1,
    STORE_ENTITLEMENT_ITEM = 0,
    STORE_ENTITLEMENT_LICENSE = 1,
    STORE_ENTITLEMENT_PLATINUM = 2,
    STORE_ENTITLEMENT_COUNT = 3,
};

























struct StoreItemPrice
{
    StoreCurrency m_eCurrency;
    StoreItemPriceType m_dwCurrentFixedPointPrice;
    StoreItemPriceType m_dwFullFixedPointPrice;
};


struct StoreItemEntitlement
{
    StoreItemEntitlement()
    // Inline body: abstract-only C2 DIE, no concrete DIE in any CU; its inlined copies in sParseStoreItem store 8 zero bytes at offset 0 on CStore.h:106.
    // Inferred: the member written (m_platinumAmount is the 8-byte one); m_eType/m_nMaxStackCount defaults, if any, are overwritten by the caller and unknown.
    {
        m_data.m_platinumAmount = 0;
    }
    void Import(const D3::Store::ProductEntitlement &);

    union
    {
        GBID m_gbidItem;
        ContentLicenseId m_contentLicense;
        uint64 m_platinumAmount;
    } m_data;

    StoreItemEntitlementType m_eType;
    uint32 m_nMaxStackCount;
};

class StoreItemPurchaseRestriction
{
public:
    StoreItemPurchaseRestriction();







    ContentLicenseId m_idLicense;
    int32 m_nMinRequired;
    int32 m_nMaxAllowed;
};

struct StoreItem
{
    StoreItem() : m_eCategory(STORE_CATEGORY_INVALID), m_timeReleaseDate(0), m_dwDurationSeconds(0), m_productId(0) {}
    // Inference: body from the copy inlined into HyperlinkValidate (Hyperlink.cpp:298): category -1, zeroed vectors/time/duration/product id, empty string.






    void Import(const D3::Store::Product &);

    StoreCategories m_eCategory;
    blz::vector<StoreItemEntitlement> m_arEntitlements;
    blz::vector<StoreItemPrice> m_arPrices;
    blz::vector<int> m_arAttributes;
    blz::vector<StoreItemPurchaseRestriction> m_arPurchaseRestrictions;
    Blizzard::Time::Timestamp m_timeReleaseDate;
    uint64 m_dwDurationSeconds;
    blz::string m_bundleLabel;

    StoreProductId m_productId;
};

static_assert(sizeof(StoreItemPrice) == 0x18, "StoreItemPrice");
static_assert(sizeof(StoreItemEntitlement) == 0x10, "StoreItemEntitlement");
static_assert(sizeof(StoreItemPurchaseRestriction) == 0xc, "StoreItemPurchaseRestriction");
static_assert(sizeof(StoreItem) == 0xa8, "StoreItem");
} // namespace OnlineService
