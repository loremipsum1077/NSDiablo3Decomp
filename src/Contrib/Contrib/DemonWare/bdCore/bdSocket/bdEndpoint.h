// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdEndpoint.h

#pragma once

#include "../bdReference/bdReference.h"
#include "bdSecurityID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCommonAddr;

typedef bdReference<bdCommonAddr> bdCommonAddrRef;

class bdEndpoint {  // size 0x10
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdEndpoint();
    bdEndpoint(const bdCommonAddrRef, const bdSecurityID &);
    bdBool operator==(const bdEndpoint & other) const;
    bdCommonAddrRef getCommonAddr() const;
    const bdSecurityID & getSecID() const;
    bdUInt getSerializedLength() const;
    bdBool serialize(void *, const bdUInt, const bdUInt, bdUInt &) const;
    bdBool deserialize(bdCommonAddrRef, const void *, const bdUInt, const bdUInt, bdUInt &);
    bdUInt getHash() const;
protected:
    bdCommonAddrRef m_ca;  // 0x00
    bdSecurityID m_secID;  // 0x08
};
static_assert(sizeof(bdEndpoint) == 0x10, "bdEndpoint");

class bdEndpointHashingClass {  // size 0x1
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdUInt getHash(const bdEndpoint & other) const;
};
static_assert(sizeof(bdEndpointHashingClass) == 0x1, "bdEndpointHashingClass");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  bdEndpoint::bdEndpoint(const bdEndpoint &)  [inlined into 7 functions]
//   line    22  bdEndpoint::~bdEndpoint()  [inlined into 20 functions]
