// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSequenceNumberStore.h

#pragma once

#include "bdSequenceNumber.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSequenceNumberStore {  // size 0x8
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdSequenceStatus {
        BD_SN_INVALID_SMALLER = -2,
        BD_SN_VALID_SMALLER = -1,
        BD_SN_INVALID_DUPLICATE = 0,
        BD_SN_VALID_LARGER = 1,
        BD_SN_VALID_MUCH_LARGER = 2,
    };
    bdSequenceNumberStore(const bdSequenceNumber &);
    bdSequenceNumberStore::bdSequenceStatus check(const bdSequenceNumber & thisSeq);
    const bdSequenceNumber & getLastSequenceNumber() const;
    void reset(const bdSequenceNumber &);
protected:
    bdUInt32 m_bitmap;  // 0x00
    bdSequenceNumber m_lastSeq;  // 0x04
};
static_assert(sizeof(bdSequenceNumberStore) == 0x8, "bdSequenceNumberStore");

