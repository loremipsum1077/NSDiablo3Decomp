// The two blzext allocation hooks. They are declared in
// Contrib/Contrib/blz/include/blz/mem_alloc.h, which every blz container
// allocates through; the game defines them out-of-line here.

#include "../../../Contrib/Contrib/blz/include/blz/mem_alloc.h"
#include "../Memory/SigmaMemory.h"

namespace blzext {

// line 244
void* alloc(size_t n) {
    return SigmaMemoryNew(n, 4, NULL, 0);
}

// line 250
void free(void* p) {
    SigmaMemoryFree(p, NULL);
}

}  // namespace blzext
