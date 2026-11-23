#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "bdAddrHandleRefWrapper.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdAddrHandleRefWrapper::bdAddrHandleRefWrapper() {}
bdAddrHandleRefWrapper::bdAddrHandleRefWrapper(bdAddrHandleRef handle)
    : m_handle(handle) {}

bdBool bdAddrHandleRefWrapper::operator==(const bdAddrHandleRefWrapper& other) const
{
    bdBool equal = false;
    if (m_handle.notNull() && other.m_handle.notNull())
        equal = *m_handle == *other.m_handle;
    else
        bdUseVAArgs(); // Original disabled diagnostic retains only this call.
    return equal;
}

bdUInt bdAddrHandleRefWrapper::getHash(const bdAddrHandleRefWrapper& addr) const
{
    bdEndpointHashingClass hash;
    const bdUInt hashVal = hash.getHash(addr.m_handle->m_endpoint);
    return hashVal;
}
