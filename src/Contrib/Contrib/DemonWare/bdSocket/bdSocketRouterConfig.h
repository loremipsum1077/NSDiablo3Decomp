// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSocketRouterConfig.h

#pragma once

#include "../bdCore/bdCrypto/bdCypherSuites.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSocketRouterConfig {  // size 0x10
public:
    static void operator delete(void *);
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    bdSocketRouterConfig();
    void reset();
    bdBool sanityCheckConfig();
    bdBool m_ignoreConnectionReset;  // 0x00
    bdFloat32 m_DTLSAssociationReceiveTimeout;  // 0x04
    bdUInt m_maxConnectionResets;  // 0x08
    bdCypherSuite m_cypherSuite;  // 0x0c
};
static_assert(sizeof(bdSocketRouterConfig) == 0x10, "bdSocketRouterConfig");

