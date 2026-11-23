// Template checks for blz/unordered_set.h and blz/chained_hash_table.h.
#include "Contrib/Contrib/blz/include/blz/unordered_set.h"

class XVarBase;

typedef blz::unordered_set<int> SetInt;
typedef blz::unordered_set<unsigned long> SetUL;
typedef blz::unordered_set<XVarBase*> SetXVar;

template blz::pair<SetInt::iterator, bool> SetInt::chained_hash_table::insert(int&);
template void SetInt::chained_hash_table::rehash(size_t);

template blz::pair<SetUL::iterator, bool> SetUL::chained_hash_table::insert(unsigned long&);
template void SetUL::chained_hash_table::rehash(size_t);

template blz::pair<SetXVar::iterator, bool> SetXVar::chained_hash_table::insert(XVarBase*&);
template void SetXVar::chained_hash_table::rehash(size_t);
