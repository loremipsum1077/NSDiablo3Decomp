// Original owner: X/Client/UserInterface/Console/UIConsoleNewItemIndicators.h.
#pragma once
#include "../../../../Contrib/Contrib/blz/include/blz/func_poly.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"

namespace UIInventory { namespace Console {
template <typename T> class NewItemIndicatorManager {
public:
    typedef blz::function<int(T)> PredicateFunc;
    typedef XGrowableMap<T, T, int, int> ItemMap;

    void MarkAsNew(const T idItem) { m_tNewItemMap.SetAt_Tracked(idItem, TRUE); } // line 30
    void UnMarkAsNew(const T idItem) { // line 36
        if (m_tNewItemMap.LookupPtr(idItem)) m_tNewItemMap.RemoveAt(idItem);
    }
    const ItemMap & GetItemMap() const { return m_tNewItemMap; }
    // The discarded value local is not named in the optimized DWARF; nValue is invented.
    BOOL ItemIsNew(const T idItem) const { int nValue; return m_tNewItemMap.Lookup(idItem, nValue); } // line 52
    // NON_MATCHING: 464 vs 456 bytes; only the inlined blz::vector::push_back grow path differs (csinc vs csel,
    // an extra branch, x22/x24 swap); calls, constants and loops match. Tried `!= 0` on the predicate.
    // line 63; locals on DWARF lines 66 arToBeRemoved, 68 pos, 71 key, 72 bValIgnored, 80 nCount, 81 i
    void RemoveItemsIf(PredicateFunc fnPredicate) {
        blz::vector<T> arToBeRemoved;
        POS pos = m_tNewItemMap.GetStartAssoc();
        while (pos) {
            T key;
            BOOL bValIgnored;
            m_tNewItemMap.GetNextAssoc(pos, key, bValIgnored);
            if (fnPredicate(key)) {
                arToBeRemoved.push_back(key);
            }
        }
        size_t nCount = arToBeRemoved.size();
        for (size_t i = 0; i < nCount; ++i) {
            m_tNewItemMap.RemoveAt(arToBeRemoved[i]);
        }
    }
    void Reset() { m_tNewItemMap.RemoveAll(); } // line 87
private:
    ItemMap m_tNewItemMap;
};
static_assert(sizeof(NewItemIndicatorManager<ACDID>) == 0x80, "NewItemIndicatorManager<ACDID>");
static_assert(sizeof(NewItemIndicatorManager<int>) == 0x80, "NewItemIndicatorManager<int>");
} }
