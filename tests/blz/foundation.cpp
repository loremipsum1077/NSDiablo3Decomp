// Template checks for the blz foundation headers.
#include "Contrib/Contrib/blz/include/blz/config.h"
#include "Contrib/Contrib/blz/include/blz/type_traits.h"
#include "Contrib/Contrib/blz/include/blz/limits.h"
#include "Contrib/Contrib/blz/include/blz/mem_alloc.h"
#include "Contrib/Contrib/blz/include/blz/char_traits.h"
#include "Contrib/Contrib/blz/include/blz/func_op.h"
#include "Contrib/Contrib/blz/include/blz/func_hash.h"
#include "Contrib/Contrib/blz/include/blz/func_invoke.h"
#include "Contrib/Contrib/blz/include/blz/util_pair.h"
#include "Contrib/Contrib/blz/include/blz/util_swap.h"
#include "Contrib/Contrib/blz/include/blz/util_int_seq.h"
#include "Contrib/Contrib/blz/include/blz/iter_types.h"
#include "Contrib/Contrib/blz/include/blz/iter_op.h"
#include "Contrib/Contrib/blz/include/blz/iter_move.h"
#include "Contrib/Contrib/blz/include/blz/iter_reverse.h"
#include "Contrib/Contrib/blz/include/blz/iter_insert.h"
#include "Contrib/Contrib/blz/include/blz/iter_range.h"
#include "Contrib/Contrib/blz/include/blz/algo_memory.h"
#include "Contrib/Contrib/blz/include/blz/algo_minmax.h"
#include "Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "Contrib/Contrib/blz/include/blz/algo_search.h"
#include "Contrib/Contrib/blz/include/blz/tuple.h"
#include "Contrib/Contrib/blz/include/blz/array.h"
#include "Contrib/Contrib/blz/include/blz/object_from_member.h"
#include "Contrib/Contrib/blz/include/blz/temp_buffer.h"
#include "Contrib/Contrib/blz/include/blz/float.h"
#include "Contrib/Contrib/blz/include/blz/math.h"
#include "Contrib/Contrib/blz/include/blz/log.h"

class XVarBase;
class XVarCollection {
public:
    void f(XVarBase*);
};
struct Player;
namespace blz { struct list_link { void* a; void* b; }; }
struct XVarScopeInternal { int pad[20]; blz::list_link m_tParentScopeLink; };
struct SNOName { int a, b; };

// Mangled-name checks: explicit instantiation definitions emit each helper
// so its mangled name can be compared with the one the DWARF records.
template blz::enable_if<blz::is_fundamental<int>::value>::type blz::_construct<int>(int*, int*);
template blz::enable_if<!blz::is_fundamental<SNOName>::value>::type blz::_construct<SNOName>(SNOName*, SNOName*);
template void blz::_copy_construct<int>(int*, const int&, blz::true_type);
template void blz::_destroy<SNOName*>(SNOName*, SNOName*, blz::false_type);
template unsigned long* blz::_uninitialized_copy<unsigned long>(unsigned long*, unsigned long*, unsigned long*, blz::true_type);
template unsigned long* blz::_uninitialized_copy<const unsigned long*, unsigned long*>(const unsigned long*, const unsigned long*, unsigned long*, blz::true_type);
template unsigned int* blz::_uninitialized_move<unsigned int>(unsigned int*, unsigned int*, unsigned int*, blz::true_type);
template const unsigned int& blz::min<unsigned int>(const unsigned int&, const unsigned int&);
template const int& blz::max<int>(const int&, const int&);
template const int& blz::clamp<int>(const int&, const int&, const int&);
template const int& blz::clamp<int, blz::less<int> >(const int&, const int&, const int&, blz::less<int>);
template bool blz::binary_search<int*, int>(int*, int*, const int&);
template bool blz::binary_search<int*, int, blz::less<void> >(int*, int*, const int&, blz::less<void>);
template int* blz::fill_n<int*, long, int>(int*, long, const int&);
template void blz::fill<int*, int>(int*, int*, const int&);
template void blz::_fill<int*, int>(int*, int*, const int&, blz::random_access_iterator_tag);
template void blz::random_shuffle<int*>(int*, int*);
template char* blz::swap_ranges<char*, char*>(char*, char*, char*);
template const unsigned long* blz::find<const unsigned long*, unsigned long>(const unsigned long*, const unsigned long*, const unsigned long&);
template void blz::advance<int*, long>(int*&, long);
template void blz::_advance<int*, long>(int*&, long, blz::random_access_iterator_tag);
template blz::iterator_traits<unsigned long*>::difference_type blz::distance<unsigned long*>(unsigned long*, unsigned long*);
template blz::iterator_traits<unsigned long*>::difference_type blz::_distance<unsigned long*>(unsigned long*, unsigned long*, blz::random_access_iterator_tag);
template void blz::swap<char*>(char*&, char*&);
template XVarScopeInternal* blz::object_from_member<XVarScopeInternal, blz::list_link>(blz::list_link*, const blz::list_link XVarScopeInternal::*);
template void blz::invoke(void (XVarCollection::*)(XVarBase*), XVarCollection*&, XVarBase*&);
template blz::pair<int, int> blz::make_pair<int&, int&>(int&, int&);
template bool blz::operator==<int, int>(const blz::pair<int, int>&, const blz::pair<int, int>&);
template bool blz::operator!=<SNOName*, SNOName*>(const blz::move_iterator<SNOName*>&, const blz::move_iterator<SNOName*>&);
template long blz::operator-<SNOName*, SNOName*>(const blz::move_iterator<SNOName*>&, const blz::move_iterator<SNOName*>&);
template blz::pair<int, int>::pair(int&, int&, void*);
template blz::pair<unsigned int, unsigned int>& blz::pair<unsigned int, unsigned int>::operator=<int, int>(blz::pair<int, int>&&);
template blz::tuple<Player*, int>::tuple(Player*&&, const int&);
template bool blz::less<void>::operator()<int&, const int&>(int&, const int&) const;

template struct blz::pair<int, int>;
template class blz::tuple<Player*, int>;
template struct blz::array<int, 10>;
template class blz::allocator<int>;
template struct blz::hash<int>;
template struct blz::hash<XVarBase*>;
template struct blz::equal_to<int>;
template struct blz::less<unsigned int>;
template class blz::move_iterator<SNOName*>;
template class blz::reverse_iterator<SNOName*>;
template class blz::temp_buffer<SNOName>;

bool lessv(int& a, const int& b) { return blz::less<void>()(a, b); }
size_t hd(const void* p, size_t n) { return blz::hash_data(p, n); }

void pmove(blz::pair<const SNOName, unsigned int>* d, blz::pair<const SNOName, unsigned int>&& s) {
    new (d) blz::pair<const SNOName, unsigned int>(blz::move(s));
}
int lim() { return blz::numeric_limits<int>::max() + (int)blz::numeric_limits<double>::max(); }
int tget(blz::tuple<Player*, int>& t) { return blz::get<1>(t) + (blz::get<0>(t) != 0); }
void ttie(int& a, Player*& p, blz::tuple<Player*, int>& t) { blz::tie(p, a) = t; }
int arr(const blz::array<int, 10>& a, size_t n) { return a[n]; }
