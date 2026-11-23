#include "../../../BlizzardCore/Include/Blizzard/Memory.h"

#include "../Memory/SigmaMemory.h"

namespace Blizzard {
namespace Memory {

voidp_return Allocate(uint32 size) {
    return SigmaMemoryNew(size, 4, NULL, FALSE);
}

voidp_return Allocate(uint32 size, uint32 flags) {
    return SigmaMemoryNew(size, 4, NULL, flags & ZEROFILL);
}

voidp_return Allocate(uint32 size, uint32 flags, const char* file, uint32 line, const char* info) {
    return SigmaMemoryNew(size, 4, NULL, flags & ZEROFILL);
}

void Free(void* ptr) {
    SigmaMemoryFree(ptr, NULL);
}

void Free(void* ptr, uint32, const char* file, uint32 line, const char*) {
    SigmaMemoryFree(ptr, NULL);
}

void Shutdown() {
}

}  // namespace Memory
}  // namespace Blizzard
