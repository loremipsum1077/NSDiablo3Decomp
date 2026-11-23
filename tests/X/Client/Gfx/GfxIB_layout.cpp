// Layout check for X/Client/Gfx/GfxIB.h against the DWARF. Compiled, not compared.
#include "X/Client/Gfx/GfxIB.h"

#include <cstddef>

static_assert(sizeof(PackedIB) == 0x8, "PackedIB");
static_assert(offsetof(PackedIB, ibid) == 0x04, "PackedIB::ibid");
static_assert(sizeof(PackedIBEntry) == 0x10, "PackedIBEntry");
static_assert(offsetof(PackedIBEntry, dwIndexOffset) == 0x08, "PackedIBEntry::dwIndexOffset");
static_assert(sizeof(PackedIBEntry64) == 0x10, "PackedIBEntry64");
static_assert(offsetof(PackedIBEntry64, dwIndexOffset) == 0x08, "PackedIBEntry64::dwIndexOffset");
