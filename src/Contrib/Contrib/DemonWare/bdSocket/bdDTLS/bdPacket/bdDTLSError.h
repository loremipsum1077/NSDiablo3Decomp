// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdPacket/bdDTLSError.h

#pragma once

#include "../../../bdCore/bdSocket/bdSecurityID.h"
#include "../../../bdCore/bdMemory/bdMemory-std.h"
#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdDTLSHeader.h"
#include <stddef.h>
#include <stdint.h>

class bdDTLSError : public bdDTLSHeader {  // size 0x20
public:
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    enum bdDTLSErrorType {
        BD_DTLS_ERROR_BAD_SECID = 0,
        BD_DTLS_INVALID_STATE = 1,
    };
    bdDTLSError();
    bdDTLSError(bdUInt16, bdDTLSError::bdDTLSErrorType, const bdSecurityID &);
    virtual ~bdDTLSError();  // vtable slot 0
    virtual bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;  // vtable slot 2
    virtual bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);  // vtable slot 3
    bdDTLSError::bdDTLSErrorType getEtype() const;
    void getSecID(bdSecurityID & secID) const;
protected:
    bdDTLSError::bdDTLSErrorType m_etype;  // 0x10
    bdSecurityID m_secID;  // 0x14
};
static_assert(sizeof(bdDTLSError) == 0x20, "bdDTLSError");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  void bdDTLSError::operator delete(void * p)  [inlined into 1 functions]
