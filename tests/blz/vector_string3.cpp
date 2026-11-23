// Template checks for blz::vector<blz::string>::push_back(T&&) (separate TU: it
// would share reserve with the other blz::string instantiations).
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "vector_types.h"

template void blz::vector<blz::string>::push_back(blz::string&&);
