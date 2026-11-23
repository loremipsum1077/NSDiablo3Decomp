// Original file: BlizzardCore/Source/System/Memory/System_Memory.cpp
// The original is a unity file; both functions are from Memory.cpp.
#include "../../../Include/Blizzard/Memory.h"

#include <stdlib.h>

namespace Blizzard {
namespace Memory {
// Memory.cpp:808
voidp_return RawAlloc(size_t size)
{
    return malloc(size);
}

// Memory.cpp:821
void RawFree(void * ptr)
{
    free(ptr);
}
}  // namespace Memory
}  // namespace Blizzard
