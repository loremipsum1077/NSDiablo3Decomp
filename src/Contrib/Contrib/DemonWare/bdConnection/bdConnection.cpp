// Original bdConnection/bdConnection.cpp
#include "bdConnection.h"
#include "bdConnectionListener.h"
#include "bdMessage/bdMessage.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
bdConnection::~bdConnection() {}
bdCommonAddrRef bdConnection::getAddress() const { return m_addr; }
const bdAddrHandleRef & bdConnection::getAddressHandle() const { return m_addrHandle; }
void bdConnection::setAddressHandle(const bdAddrHandleRef & addr) { m_addrHandle = addr; }
bdConnection::Status bdConnection::getStatus() const { return m_status; }
void bdConnection::registerListener(bdConnectionListener *const listener) { m_listeners.pushBack(listener); }
void bdConnection::unregisterListener(bdConnectionListener *const listener) { m_listeners.removeAllKeepOrder(listener); }
bdBool bdConnection::connect() { m_status=BD_CONNECTING; return true; }
void bdConnection::disconnect() { m_status=BD_DISCONNECTING; }
void bdConnection::close() { m_status=BD_DISCONNECTED; }
bdConnection::bdConnection() : m_addr(), m_addrHandle(), m_listeners(0), m_maxTransmissionRate(0), m_status(BD_NOT_CONNECTED) {}
bdConnection::bdConnection(bdCommonAddrRef addr) : m_addr(addr), m_addrHandle(), m_listeners(0), m_maxTransmissionRate(0), m_status(BD_NOT_CONNECTED) {}
