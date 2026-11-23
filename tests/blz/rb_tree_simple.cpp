// Template checks for blz::rb_tree with plain key types.
#include "Contrib/Contrib/blz/include/blz/rb_tree.h"
#include "Contrib/Contrib/blz/include/blz/set.h"
#include "Contrib/Contrib/blz/include/blz/map.h"

template class blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >;
template class blz::rb_tree<blz::rb_map_traits<int, int>, blz::less<int>, blz::allocator<blz::pair<const int, int> > >;
