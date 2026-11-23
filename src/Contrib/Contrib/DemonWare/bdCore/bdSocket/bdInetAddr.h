// Adopted from the DWARF; hand-maintained declarations.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdInetAddr.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdPlatform/bdPlatformSocket/bdSockAddr.h"
#include <stddef.h>
#include <stdint.h>

class bdInetAddr {  // size 0x84
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    static bdInetAddr Loopback();
    static bdInetAddr Broadcast();
    static bdInetAddr Any();
    static bdInetAddr Loopback(bdUInt16 af_family);
    static bdInetAddr Broadcast(bdUInt16 af_family);
    static bdInetAddr Any(bdUInt16 af_family);
    bdInetAddr();
    ~bdInetAddr();
    bdInetAddr(const bdInetAddr &);
    bdInetAddr(const bdUInt);
    bdInetAddr(bdUByte8 *);
    bdInetAddr(const bdNChar8 *);
    bdInetAddr(const bdInAddr);
    void set(const bdNChar8 * address);
    void set(const bdUInt address);
    void set(const bdInetAddr & other);
    void set(const bdInAddr inaddr);
    void set(bdUByte8 * addr);
    bdBool operator==(const bdInetAddr & other) const;
    bdBool operator!=(const bdInetAddr & other) const;
    bdBool operator<(const bdInetAddr &) const;
    bdBool isValid() const;
    bdBool isLoopback() const;
    bdBool isBroadcast() const;
    bdUInt toUInt32() const;
    bdUWord toString(bdNChar8 *const str, const bdUWord size) const;
    bdInAddr getInAddr() const;
    bdInAddr getSockAddr() const;
    bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;
    bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);
protected:
    bdInAddr m_addr;  // 0x00
};
static_assert(sizeof(bdInetAddr) == 0x84, "bdInetAddr");

