#include "bdNATTypeDiscoveryConfig.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
bdNATTypeDiscoveryConfig::bdNATTypeDiscoveryConfig() : m_NtdcsMaxResends(5), m_NtdcsSendTimeout(0.5f) {}
void bdNATTypeDiscoveryConfig::sanityCheckConfig() {
 if (m_NtdcsMaxResends < 2 || m_NtdcsMaxResends > 10) bdUseVAArgs(m_NtdcsMaxResends);
 if (m_NtdcsSendTimeout > 2.0f || m_NtdcsSendTimeout < 0.05f) bdUseVAArgs(m_NtdcsSendTimeout);
}
