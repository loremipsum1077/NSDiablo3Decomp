// Template checks for blz::function / blz::bind (func_poly.h, func_bind.h).
#include "Contrib/Contrib/blz/include/blz/func_poly.h"
#include "Contrib/Contrib/blz/include/blz/func_bind.h"

struct Player;
class XVarBase;
class XVarCollection {
public:
    void f(XVarBase*);
};

struct ACDID { unsigned int id; };
namespace UIInventory { namespace Console {
struct RemovedItemPredicate { void* m_unk; int operator()(ACDID) const; };
} }

typedef void (*PlayerFn)(Player*, int);
typedef blz::_bind_object<PlayerFn, Player*&, int&> BindA;
typedef blz::_bind_object<PlayerFn, Player*, const int&> BindB;
typedef blz::_bind_object<void (XVarCollection::*)(XVarBase*), XVarCollection*, XVarBase*> BindC;

template class blz::function<void()>::impl<BindA>;
template class blz::function<void()>::impl<BindB>;
template class blz::function<void()>::impl<BindC>;
template class blz::function<int(ACDID)>::impl<UIInventory::Console::RemovedItemPredicate>;

// callers: the inlined function members (mangled names are recorded for them)
void use_function(blz::function<void()>& a, const blz::function<void()>& b, Player* p, int& i, PlayerFn fn) {
    a = b;
    a = nullptr;
    blz::function<void()> c(blz::bind(fn, p, i));
    if (c)
        c();
    blz::function<void()> d(blz::move(c));
    a = d;
}

// UIInventory::Console::CraftingMatItemsProvider::Refresh (UIConsoleInventoryView.cpp:1253)
// sorts a vector of a local struct with a lambda wrapped in a blz::function,
// which instantiates function<bool(const CurrencyItemInfo&, ...)>::impl<$_0>
// and (through blz::sort's by-value comparator) the function copies.
#include "Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "Contrib/Contrib/blz/include/blz/vector.h"

namespace UIInventory { namespace Console {
int GetCurrency(int);
int GetCurrencyCount();
class CraftingMatItemsProvider {
public:
    void Refresh();
};

void CraftingMatItemsProvider::Refresh() {
    struct CurrencyItemInfo {
        int nCurrency;
        int nAmount;
    };
    blz::vector<CurrencyItemInfo, blz::allocator<CurrencyItemInfo> > arCurrencyInfos;
    for (int i = 0; i < GetCurrencyCount(); ++i) {
        CurrencyItemInfo tCurrencyItemInfo = {i, GetCurrency(i)};
        arCurrencyInfos.push_back(tCurrencyItemInfo);
    }
    blz::function<bool(const CurrencyItemInfo&, const CurrencyItemInfo&)> funcSortEntries =
        [](const CurrencyItemInfo& tCurrencyA, const CurrencyItemInfo& tCurrencyB) {
            return tCurrencyA.nAmount < tCurrencyB.nAmount;
        };
    blz::sort(arCurrencyInfos.begin(), arCurrencyInfos.end(), funcSortEntries);
}
} }

namespace UIInventory { namespace Console {
class UnmailedCraftingMatItemsProvider {
public:
    void Refresh();
};

void UnmailedCraftingMatItemsProvider::Refresh() {
    struct CurrencyItemInfo {
        int nCurrency;
        int nAmount;
    };
    blz::vector<CurrencyItemInfo, blz::allocator<CurrencyItemInfo> > arCurrencyInfos;
    for (int i = 0; i < GetCurrencyCount(); ++i) {
        CurrencyItemInfo tCurrencyItemInfo = {i, GetCurrency(i)};
        arCurrencyInfos.push_back(tCurrencyItemInfo);
    }
    blz::function<bool(const CurrencyItemInfo&, const CurrencyItemInfo&)> funcSortEntries =
        [](const CurrencyItemInfo& tCurrencyA, const CurrencyItemInfo& tCurrencyB) {
            return tCurrencyA.nAmount < tCurrencyB.nAmount;
        };
    blz::sort(arCurrencyInfos.begin(), arCurrencyInfos.end(), funcSortEntries);
}
} }
