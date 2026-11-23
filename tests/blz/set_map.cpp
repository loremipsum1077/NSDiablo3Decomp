// Whole-class instantiations of blz::set and blz::map (compile check of every
// member, plus the mangled names the DWARF records for set<int>).
#include "Contrib/Contrib/blz/include/blz/set.h"
#include "Contrib/Contrib/blz/include/blz/map.h"
#include "Contrib/Contrib/blz/include/blz/iter_op.h"

template class blz::set<int, blz::less<int>, blz::allocator<int> >;
template class blz::map<int, int, blz::less<int>, blz::allocator<blz::pair<const int, int> > >;
template void blz::set<int, blz::less<int>, blz::allocator<int> >::insert<int*>(int*, int*);
template blz::enable_if<true, blz::pair<blz::rb_tree_const_iterator<int>, bool> >::type blz::set<int, blz::less<int>, blz::allocator<int> >::insert<int>(int&&);
template blz::enable_if<blz::is_convertible<blz::pair<int, int>, blz::pair<const int, int> >::value, blz::pair<blz::rb_tree_iterator<blz::pair<const int, int> >, bool> >::type blz::map<int, int, blz::less<int>, blz::allocator<blz::pair<const int, int> > >::insert<blz::pair<int, int> >(blz::pair<int, int>&&);
template class blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >;
template blz::enable_if<!blz::is_integral<int*>::value>::type blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >::_insert_uniq<int*>(int*, int*);
template blz::rb_node_base* blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >::_lower_bound<int>(blz::rb_node_base*, blz::rb_node_base*, const int&) const;
template blz::rb_node_base* blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >::_upper_bound<int>(blz::rb_node_base*, blz::rb_node_base*, const int&) const;
template blz::pair<blz::rb_node_base*, blz::rb_node_base*> blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >::_equal_range<int>(const int&) const;

// the iterators work with blz::distance (vector's range constructor)
template blz::iterator_traits<blz::rb_tree_const_iterator<unsigned long> >::difference_type blz::_distance<blz::rb_tree_const_iterator<unsigned long>, blz::bidirectional_iterator_tag>(blz::rb_tree_const_iterator<unsigned long>, blz::rb_tree_const_iterator<unsigned long>, blz::bidirectional_iterator_tag);
template class blz::set<unsigned long, blz::less<unsigned long>, blz::allocator<unsigned long> >;
static_assert(sizeof(blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >) == 0x28, "rb_tree");
static_assert(sizeof(blz::set<int>) == 0x28, "set");
static_assert(sizeof(blz::map<int, int>) == 0x28, "map");
static_assert(sizeof(blz::rb_node_base) == 0x20, "rb_node_base");
static_assert(sizeof(blz::rb_node<unsigned long>) == 0x28, "rb_node");
static_assert(sizeof(blz::rb_tree_const_iterator<int>) == 8 && sizeof(blz::rb_tree_iterator<int>) == 8, "iterators");
