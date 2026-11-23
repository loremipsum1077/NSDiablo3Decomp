#include "bdGetHostByName.h"
#include "../../bdCore/bdThread/bdThread.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
// Blank lines keep the original line numbers









bdUInt bdGetHostByName::run(void* args)
{
    const bdUInt result = 0;
    if (m_status == BD_LOOKUP_PENDING)
    {
        m_numAddresses = bdPlatformSocket::getHostByName(static_cast<const bdNChar8*>(args), m_addresses, 4, m_handle);
        m_status = (m_numAddresses == 0) ? BD_LOOKUP_FAILED : BD_LOOKUP_SUCCEEDED;
    }
    else
    {







        bdUseVAArgs();
    }
    return result;
}

void bdGetHostByName::quit()
{
    if (m_status != BD_LOOKUP_UNINITIALIZED)
    {
        if (m_status == BD_LOOKUP_PENDING)
        {
            cancelLookup();
        }
        if (m_thread != NULL)
        {
            m_thread->join();
            m_thread->cleanup();
            m_thread = NULL;
        }
        m_status = BD_LOOKUP_UNINITIALIZED;
    }
}
