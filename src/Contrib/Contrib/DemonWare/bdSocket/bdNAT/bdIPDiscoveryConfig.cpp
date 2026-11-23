#include "bdIPDiscoveryConfig.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
bdIPDiscoveryConfig::bdIPDiscoveryConfig() : m_retries(5), m_requestTimeout(0.5f), m_version(3) {}
void bdIPDiscoveryConfig::sanityCheckConfig() {
 if (m_retries < 2 || m_retries > 10) bdUseVAArgs(m_retries);
 if (m_requestTimeout > 2.0f || m_requestTimeout < 0.05f) bdUseVAArgs(m_requestTimeout);
}
