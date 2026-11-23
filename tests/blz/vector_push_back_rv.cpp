// Template checks for blz::vector: push_back(T&&).
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "vector_types.h"

template void blz::vector<SNOName>::push_back(SNOName&&);
template void blz::vector<BuffColor>::push_back(BuffColor&&);
template void blz::vector<CRefString>::push_back(CRefString&&);
