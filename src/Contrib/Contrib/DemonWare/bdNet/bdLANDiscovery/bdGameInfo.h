// Adopted from the DWARF reference; hand-maintained.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdGameInfo.h

#pragma once

#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdSocket/bdEndpoint.h"
#include "../../bdCore/bdSocket/bdSecurityID.h"
#include "../../bdCore/bdSocket/bdSecurityKey.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdBitBuffer;
class bdGameInfo;

typedef bdReference<bdGameInfo> bdGameInfoRef;

class bdGameInfo : public bdReferencable {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdGameInfo);
    bdGameInfo();
    bdGameInfo(bdUInt32, const bdSecurityID &, const bdSecurityKey &, bdCommonAddrRef);
    virtual ~bdGameInfo();  // vtable slot 0
    bdUInt32 getTitleID() const;
    bdCommonAddrRef getHostAddr() const;
    const bdSecurityID & getSecurityID() const;
    const bdSecurityKey & getSecurityKey() const;
    void setTitleID(const bdUInt32 titleID);
    void setHostAddr(const bdCommonAddrRef hostAddr);
    void setSecurityID(const bdSecurityID & secID);
    void setSecurityKey(const bdSecurityKey & secKey);
    virtual void serialize(bdBitBuffer & bitBuffer) const;  // vtable slot 2
    virtual bdBool deserialize(const bdCommonAddrRef localAddr, bdBitBuffer & bitBuffer);  // vtable slot 3
protected:
    bdUInt32 m_titleId;  // 0x0c
    bdSecurityID m_secID;  // 0x10
    bdSecurityKey m_secKey;  // 0x18
    bdCommonAddrRef m_hostAddr;  // 0x28
};
static_assert(sizeof(bdGameInfo) == 0x30, "bdGameInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  void bdGameInfo::operator delete(void * p)  [inlined into 2 functions]
//   line    23  void * bdGameInfo::operator new(size_t nbytes)  [inlined into 1 functions]
