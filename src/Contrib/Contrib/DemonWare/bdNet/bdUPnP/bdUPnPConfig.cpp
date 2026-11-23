#include "bdUPnPConfig.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

// Blank lines keep the original line numbers


































bdUPnPConfig::bdUPnPConfig()
    : m_discoveryTimeout(3.0f), m_responseTimeout(10.0f), m_connectTimeout(5.0f), m_runMode(BD_UPNP_DO_PORT_MAPPING),
      m_discoveryRetries(1), m_mappingRetries(4), m_portSelectionMode(BD_UPNP_PORT_SELECTION_EXPANDING_RANDOM),
      m_portPoolSize(500), m_disabled(false), m_gatewayAddr(), m_onlyUseGateway(false)
{
}








void bdUPnPConfig::reset()
{
    m_discoveryTimeout = 3.0f;
    m_responseTimeout = 10.0f;
    m_connectTimeout = 5.0f;
    m_runMode = BD_UPNP_DO_PORT_MAPPING;
    m_discoveryRetries = 1;
    m_mappingRetries = 4;
    m_gatewayAddr = bdInetAddr();
    m_portSelectionMode = BD_UPNP_PORT_SELECTION_EXPANDING_RANDOM;
    m_portPoolSize = 500;
    m_disabled = false;
    m_onlyUseGateway = false;
}

void bdUPnPConfig::sanityCheckConfig()
{




    if (m_discoveryTimeout > 12.0f || m_discoveryTimeout < 0.5f)


        bdUseVAArgs((double)m_discoveryTimeout);


    if (m_responseTimeout > 40.0f || m_responseTimeout < 1.0f)


        bdUseVAArgs((double)m_responseTimeout);


    if (m_connectTimeout > 20.0f || m_connectTimeout < 1.0f)


        bdUseVAArgs((double)m_connectTimeout);


    if (m_runMode != BD_UPNP_DO_PORT_MAPPING && m_runMode != BD_UPNP_EXTERNAL_IP_ONLY)


        bdUseVAArgs();


    if (m_discoveryRetries >= 5)

        bdUseVAArgs(m_discoveryRetries);


    if (m_mappingRetries > 16)

        bdUseVAArgs(m_mappingRetries, 16);


}
