// Template checks for blz::vector<blz::string>::emplace_back (separate TU: it
// would share reserve with the other blz::string instantiations).
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "vector_types.h"

template blz::string& blz::vector<blz::string>::emplace_back(const char*&);
