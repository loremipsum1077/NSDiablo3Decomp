// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdAddrHandle.h

#pragma once

#include "../bdCore/bdReference/bdReferencable.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdSocket/bdAddr.h"
#include "../bdCore/bdSocket/bdEndpoint.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSecurityID;

class bdAddrHandle;

typedef bdReference<bdAddrHandle> bdAddrHandleRef;

class bdAddrHandle : public bdReferencable {  // size 0xb0
public:
    BD_CLASS_ALLOCATOR(bdAddrHandle);
    virtual ~bdAddrHandle() {} // Original implicit inline destructor, line 28.
    enum bdAddrHandleStatus {
        BD_ADDR_NOT_CONSTRUCTED = 0,
        BD_ADDR_NOT_RESOLVED = 1,
        BD_ADDR_ERROR = 2,
        BD_ADDR_RESOLVED = 3,
        BD_ADDR_UNRESOLVED = 4,
    };
    void getSecID(bdSecurityID &) const;
    const bdAddr getRealAddr() const;
    bdBool operator==(const bdAddrHandle & other) const;
    void setRealAddr(const bdAddr & realAddr);
    void setStatus(const bdAddrHandle::bdAddrHandleStatus & status);
    bdAddrHandle::bdAddrHandleStatus getStatus() const;
    bdEndpoint m_endpoint;  // 0x10
protected:
    friend class bdAddressMap; // Original map constructs protected address handles.
    bdAddrHandle::bdAddrHandleStatus m_status;  // 0x20
    bdAddr m_realAddr;  // 0x24
    bdAddrHandle(bdCommonAddrRef, const bdSecurityID &);
};
static_assert(sizeof(bdAddrHandle) == 0xb0, "bdAddrHandle");


// Functions defined in this file (bodies are not in the DWARF):
//   line    28  bdAddrHandle::~bdAddrHandle()  [0x7100b57300 (+1 more), inlined into 1 functions]
//   line    32  void bdAddrHandle::operator delete(void * p)  [inlined into 1 functions]
//   line    32  void * bdAddrHandle::operator new(size_t nbytes)  [inlined into 1 functions]
