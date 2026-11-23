#include "bdAddressMap.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatform/bdPlatformString/bdPlatformString.inl"

bdAddressMap::bdAddressMap() {}

bdBool bdAddressMap::getAddrHandle(const bdCommonAddrRef ca, const bdSecurityID& id, bdAddrHandleRef& addrHandle)
{
    const bdBool found = findAddrHandle(ca, id, addrHandle);
    if (!found) {
        addrHandle = new bdAddrHandle(ca, id);
        m_addrHandles.pushBack(addrHandle);
    }
    return true;
}

// found is declared before handle (DWARF 71/72) and tested at loop-body level (79:7).
bdBool bdAddressMap::findAddrHandle(const bdCommonAddrRef ca, const bdSecurityID& id, bdAddrHandleRef& addrHandle) const
{
    bdBool result = false;
    if (ca.isNull()) bdUseVAArgs();
    else {
        const bdEndpoint endpoint(ca, id);
        const bdUInt numAddrHandles = m_addrHandles.getSize();
        bdUInt numFound = 0;
        for (bdUInt i = 0; i < numAddrHandles; ++i) {
            bdBool found = false;
            const bdAddrHandleRef handle = m_addrHandles[i];
            if (handle.notNull()) {
                found = handle->m_endpoint == endpoint;
            }
            if (found) {
                // Original stores result only on the first find (no store beside the later ++numFound).
                if (numFound == 0) {
                    addrHandle = handle;
                    result = true;
                }
                ++numFound;
            }
        }
        if (numFound > 1) bdUseVAArgs(numFound);
    }
    return result;
}

bdBool bdAddressMap::addrToCommonAddr(const bdAddrHandleRef& addrHandle, bdCommonAddrRef& ca, bdSecurityID& id) const
{
    bdBool result = false;
    if (addrHandle.isNull()) bdUseVAArgs();
    else {
        ca = addrHandle->m_endpoint.getCommonAddr();
        id = addrHandle->m_endpoint.getSecID();
        if (ca.isNull()) bdUseVAArgs();
        result = true;
    }
    return result;
}


bdBool bdAddressMap::unregisterAddr(bdAddrHandleRef& addrHandle)
{
    bdBool result = false;
    if (addrHandle.isNull()) bdUseVAArgs();
    else {
        bdBool found = false;  // inferred order: the original zeroes numFound (w21) before found (w24); DWARF lists numFound first
        bdUInt numFound = 0;
        for (bdUInt i = 0; i < m_addrHandles.getSize(); ++i) {
            const bdAddrHandleRef handle = m_addrHandles[i];
            if (handle.notNull()) found = *handle == *addrHandle;
            if (found) {
                m_addrHandles.removeAt(i);
                --i;
                ++numFound;
                result = true;
            }
        }
        if (numFound > 1) bdUseVAArgs(numFound);
        else if (numFound == 0) bdUseVAArgs();
        // DWARF has no status local here (bdAddressMap.cpp:161 cols 15/25: setStatus with the enumerator as a temporary).
        addrHandle->setStatus(bdAddrHandle::BD_ADDR_UNRESOLVED);
    }
    return result;
}

bdBool bdAddressMap::unregisterRealAddr(const bdAddr realAddr)
{
    bdBool ok = false;
    for (bdUInt i = 0; i < m_addrHandles.getSize(); ++i) {
        const bdAddrHandleRef handle = m_addrHandles[i];
        if (handle.notNull() && handle->getRealAddr() == realAddr) {
            const bdAddrHandle::bdAddrHandleStatus status = bdAddrHandle::BD_ADDR_UNRESOLVED;
            handle->setStatus(status);
            ok = true;
        }
    }
    if (!ok) {
        bdNChar8 addrString[22] = {};
        realAddr.toString(addrString, sizeof(addrString));
        bdUseVAArgs(addrString);
    }
    return ok;
}

bdUWord bdAddressMap::addrToString(const bdAddrHandleRef& addrHandle, bdNChar8* const str, const bdUWord size)
{
    bdUWord strLength;
    if (addrHandle.isNull()) {
        const bdNChar8* const nullRefStr = "Null addr handle ref";
        strLength = bdStrlcpy(str, nullRefStr, size);
    } else if (addrHandle->getStatus() == bdAddrHandle::BD_ADDR_RESOLVED) {
        strLength = addrHandle->getRealAddr().toString(str, size);
    } else if (addrHandle->m_endpoint.getCommonAddr() && addrHandle->m_endpoint.getCommonAddr()->isLoopback()) {
        const bdNChar8* const loopbackStr = "loopback";
        strLength = bdStrlcpy(str, loopbackStr, size);
    } else {
        const bdNChar8* const notResolvedStr = "not resolved";
        strLength = bdStrlcpy(str, notResolvedStr, size);
    }
    return strLength;
}
