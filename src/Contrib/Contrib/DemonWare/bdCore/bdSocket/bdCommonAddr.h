// Adopted from the DWARF reference; hand-maintained.
// Complete field layout; only declarations are needed by bdReference destruction.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h

#pragma once

#include "../bdContainers/bdArray.h"
#include "../bdReference/bdReferencable.h"
#include "../bdReference/bdReference.h"
#include "bdAddr.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdCommonAddr;
class bdBitBuffer;
typedef bdReference<bdCommonAddr> bdCommonAddrRef;

typedef bdReference<bdBitBuffer> bdBitBufferRef;

enum bdNATType {
    BD_NAT_UNKNOWN = 0,
    BD_NAT_OPEN = 1,
    BD_NAT_MODERATE = 2,
    BD_NAT_STRICT = 3,
    BD_NAT_COUNT = 4,
};

class bdCommonAddr : public bdReferencable {  // size 0xb8
public:
    static void * operator new(size_t nbytes);
    static void operator delete(void * p);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdCommonAddr();
    virtual ~bdCommonAddr();  // vtable slot 0
    bdCommonAddr(const bdArray<bdAddr> &, const bdAddr &, const bdNATType);
    bdCommonAddr(bdCommonAddrRef, const bdArray<bdAddr> &, const bdAddr &, const bdNATType);
    bdCommonAddr(const bdAddr &);
    bdNATType getNATType() const;
    const bdAddr & getPublicAddr() const;
    const bdArray<bdAddr> & getLocalAddrs() const;
    const bdAddr & getLocalAddrByIndex(const bdUInt index) const;
    bdUInt getHash() const;
    bdBool isLoopback() const;
    void serialize(bdUByte8 * buffer) const;
    void serialize(bdBitBufferRef buffer) const;
    bdBool deserialize(bdCommonAddrRef me, const bdUByte8 * buffer);
    bdBool deserialize(bdCommonAddrRef me, bdBitBufferRef buffer);
    bdBool operator==(const bdCommonAddr & other) const;
    bdBool operator!=(const bdCommonAddr & other) const;
    bdBool operator<(const bdCommonAddr &) const;
protected:
    void calculateHash();
    bdArray<bdAddr> m_localAddrs;  // 0x10
    bdAddr m_publicAddr;  // 0x20
    bdNATType m_natType;  // 0xa8
    bdUInt m_hash;  // 0xac
    bdBool m_isLoopback;  // 0xb0
};
static_assert(sizeof(bdCommonAddr) == 0xb8, "bdCommonAddr");

class bdCommonAddrHash {  // size 0x1
public:
    bdUInt getHash(const bdCommonAddrRef & addr) const { return addr->getHash(); }
};
static_assert(sizeof(bdCommonAddrHash) == 0x1, "bdCommonAddrHash");


// Functions defined in this file (bodies are not in the DWARF):
//   line    61  void bdCommonAddr::operator delete(void * p)  [inlined into 1 functions]
//   line    61  void * bdCommonAddr::operator new(size_t nbytes)  [inlined into 9 functions]
//   line   250  bdUInt bdCommonAddrHash::getHash(const bdCommonAddrRef & addr) const  [inlined into 4 functions]

// bdCommonAddr.h:61 allocator body recovered from bdGameInfo.cpp:131.
inline void* bdCommonAddr::operator new(size_t nbytes)
{
    return bdMemory::allocate(nbytes);
}

inline void bdCommonAddr::operator delete(void * p) { bdMemory::deallocate(p); }
