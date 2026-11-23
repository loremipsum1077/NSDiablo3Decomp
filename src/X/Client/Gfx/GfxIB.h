// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/GfxIB.h
// Packed index-buffer structs on the DWARF layouts; nothing here has code.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SubObjectGfx.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XPtr64.h"

typedef int32 ResourceUsage;

enum {
    TRISTRIP_RESTART_INDEX = 65535,
    MAX_INDICES_LOG2 = 21,
};

enum {
    IBID_INVALID = -1,
};

typedef int32 IBID;
void GfxIBSetCurrent(const IBID ibid);

// X/Client/Gfx/GfxIB.cpp:345, 444
void GfxQuadlistIBAlloc(const ResourceUsage eUsage, const int32 nQuadCount, const BOOL fPurgeIfNecessary, int32 & nIndexCount, IBID & ibid);
void GfxIBRelease(const IBID ibid);

struct PackedIB {  // size 0x8
    int32 nIndexCount;  // 0x00
    IBID ibid;  // 0x04
};
static_assert(sizeof(PackedIB) == 0x8, "PackedIB");

typedef XGrowableList<PackedIB, PackedIB &> PackedIBList;

struct PackedIBEntry {  // size 0x10
    PackedIB * ptPIB;  // 0x00
    uint32 dwIndexOffset;  // 0x08
};
static_assert(sizeof(PackedIBEntry) == 0x10, "PackedIBEntry");

struct PackedIBEntry64 {  // size 0x10
    XPtr64<PackedIB> ptPIB;  // 0x00
    uint32 dwIndexOffset;  // 0x08
};
static_assert(sizeof(PackedIBEntry64) == 0x10, "PackedIBEntry64");

