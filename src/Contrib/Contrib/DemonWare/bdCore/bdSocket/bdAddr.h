// Adopted from DWARF; hand-maintained.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdAddr.h

#pragma once

#include "bdInetAddr.h"
#include "bdPort.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr {  // size 0x88
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    ~bdAddr() {} // Original implicit destructor, bdAddr.h:26.
    bdAddr();
    bdAddr(const bdAddr &);
    bdAddr(const bdInetAddr &, const bdPort);
    bdAddr(const bdNChar8 *);
    void set(const bdNChar8 * socketAddress);
    void set(const bdInetAddr & address, const bdPort port);
    bdBool operator==(const bdAddr & other) const;
    bdBool operator!=(const bdAddr & other) const;
    bdBool operator<(const bdAddr &) const;
    const bdInetAddr & getAddress() const;
    bdInetAddr & getAddress();
    void setPort(const bdPort port);
    bdPort getPort() const;
    bdUWord toString(bdNChar8 *const str, const bdUWord size) const;
    bdUInt getHash() const;
    bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;
    bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);
    bdUInt getSerializedSize() const;
protected:
    bdInetAddr m_address;  // 0x00
    bdPort m_port;  // 0x84
    static bdUInt serializedSize;
};
static_assert(sizeof(bdAddr) == 0x88, "bdAddr");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  bdAddr::~bdAddr()  [inlined into 90 functions]
