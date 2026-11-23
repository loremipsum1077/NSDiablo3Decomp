#include "bdNATTravClientData.h"
#include "bdNATTravListener.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
bdNATTravClientData::bdNATTravClientData() : m_state(BD_NTCDS_INITIAL),m_listener(0),m_secondaryListener(0),m_tries(0),m_throttled(false) {}
bdNATTravClientData::bdNATTravClientData(bdCommonAddrRef local,bdCommonAddrRef remote,bdNATTravListener *listener)
 : m_state(BD_NTCDS_STAGE_1),m_local(local),m_remote(remote),m_listener(listener),m_secondaryListener(0),m_tries(0),m_throttled(false) {}
void bdNATTravClientData::callOnNATAddrDiscovery(bdCommonAddrRef remote,const bdAddr &realAddr) {
 m_listener->onNATAddrDiscovery(remote,realAddr);
 if (m_secondaryListener) m_secondaryListener->onNATAddrDiscovery(remote,realAddr);
}
void bdNATTravClientData::callOnNATAddrDiscoveryFailed(bdCommonAddrRef remote) {
 m_listener->onNATAddrDiscoveryFailed(remote);
 if (m_secondaryListener) m_secondaryListener->onNATAddrDiscoveryFailed(remote);
}
