// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddrInfo.h

#pragma once

#include "../bdReference/bdReference.h"
#include "bdEndpoint.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCommonAddr;

typedef bdReference<bdCommonAddr> bdCommonAddrRef;

class bdCommonAddrInfo {  // size 0x1
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    static bdUInt getInfo(const bdCommonAddr & addr, bdNChar8 * buf, const bdUInt length);
    static bdUInt getInfo(const bdCommonAddrRef addr, bdNChar8 * buf, const bdUInt length);
    static bdUInt getBriefInfo(const bdCommonAddr & addr, bdNChar8 * buf, const bdUInt length);
    static bdUInt getBriefInfo(const bdCommonAddrRef addr, bdNChar8 * buf, const bdUInt length);
protected:
    bdCommonAddrInfo();
};
static_assert(sizeof(bdCommonAddrInfo) == 0x1, "bdCommonAddrInfo");

