// Original bdSocket/bdAddrHandle.cpp
#include "bdAddrHandle.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
bdAddrHandle::bdAddrHandle(bdCommonAddrRef ca, const bdSecurityID & secID)
 : m_endpoint(ca, secID), m_status(BD_ADDR_NOT_RESOLVED), m_realAddr() {}
bdBool bdAddrHandle::operator==(const bdAddrHandle & other) const { bdBool equal = m_endpoint == other.m_endpoint; return equal; }
void bdAddrHandle::setRealAddr(const bdAddr & realAddr) { m_realAddr = realAddr; m_status = BD_ADDR_RESOLVED; }
const bdAddr bdAddrHandle::getRealAddr() const { return m_realAddr; }
void bdAddrHandle::setStatus(const bdAddrHandleStatus & status) { m_status = status; }
bdAddrHandle::bdAddrHandleStatus bdAddrHandle::getStatus() const { return m_status; }
