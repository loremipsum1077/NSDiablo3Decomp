#include "Contrib/Contrib/DemonWare/bdNet/bdGetHostByName/bdGetHostByNameConfig.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"


bdGetHostByNameConfig::bdGetHostByNameConfig() { m_timeout = 10.0f; }

// Original line 36.
void bdGetHostByNameConfig::sanityCheckConfig()
{
    if (m_timeout > 40.0f || m_timeout < 1.0f)
    {
        bdLogWarn("bdNet/getHostByNameConfig", "Timeout %f is outside the expected range", m_timeout);
    }
}
