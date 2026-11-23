// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdCookie.h

#pragma once

#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdCookie;
typedef bdReference<bdCookie> bdCookieRef;

class bdCookie : public bdReferencable {  // size 0x20
public:
    BD_CLASS_ALLOCATOR(bdCookie);
    bdCookie();
    bdCookie(bdUInt32, bdUInt32, bdUInt32, bdUInt32);
    virtual ~bdCookie();  // vtable slot 0
    bdUInt32 getLocalTag() const;
    bdUInt32 getPeerTag() const;
    bdUInt32 getLocalTieTag() const;
    bdUInt32 getPeerTieTag() const;
    bdUInt serialize(bdUByte8 * data, const bdUInt size) const;
    bdBool deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt & offset);
    bdUInt getSerializedSize() const;
protected:
    bdUInt32 m_localTag;  // 0x0c
    bdUInt32 m_peerTag;  // 0x10
    bdUInt32 m_localTieTag;  // 0x14
    bdUInt32 m_peerTieTag;  // 0x18
    static bdUByte8 m_secret[20];
    static bdBool m_secretInitialized;
};
static_assert(sizeof(bdCookie) == 0x20, "bdCookie");


// Functions defined in this file (bodies are not in the DWARF):
//   line    51  void bdCookie::operator delete(void * p)  [inlined into 1 functions]
//   line    51  void * bdCookie::operator new(size_t nbytes)  [inlined into 2 functions]
