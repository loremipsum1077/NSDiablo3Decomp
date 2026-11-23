#include "bdGetHostByName.h"
#include "../../bdCore/bdThread/bdThread.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

// Reconstructed from original instructions and DWARF.






// Blank lines preserve the original owner statement lines.


bdGetHostByName::bdGetHostByName(bdUInt threadStackSize) :
    m_status(BD_LOOKUP_UNINITIALIZED),
    m_thread(NULL),
    m_numAddresses(0),
    m_threadStackSize(threadStackSize)
{
}

bdGetHostByName::~bdGetHostByName()
{
    quit();
}

bdBool bdGetHostByName::start(const bdNChar8 *hostname,
                            bdGetHostByNameConfig config)
{
    bdBool success = false;
    if (m_status == BD_LOOKUP_UNINITIALIZED)
    {
        m_thread = new bdThread(this, 16, m_threadStackSize, 0);
        m_config = config;
        m_timer.reset();
        m_timer.start();
        config.sanityCheckConfig();
        m_status = BD_LOOKUP_PENDING;

        success = m_thread->start(hostname, bdStrlen(hostname) + 1, NULL);
        if (success)
        {
            bdUseVAArgs(hostname);
        }
        else
        {


            m_status = BD_LOOKUP_ERROR;
            bdUseVAArgs(hostname);
        }
    }
    else
    {
        bdUseVAArgs();
    }
    return success;
}

void bdGetHostByName::pump()
{
    if (m_status == BD_LOOKUP_PENDING)
    {
        if (m_timer.getElapsedTimeInSeconds() > m_config.m_timeout)
        {
            bdUseVAArgs(m_config.m_timeout);
            cancelLookup();
            m_status = BD_LOOKUP_TIMED_OUT;
        }
    }
    else
    {
        bdUseVAArgs();
    }
}

bdGetHostByName::bdStatus bdGetHostByName::getStatus() const
{
    return m_status;
}

bdUInt bdGetHostByName::getNumAddresses() const
{
    return m_numAddresses;
}

bdInAddr bdGetHostByName::getAddressAt(bdUInt index) const
{
    bdInAddr result;
    if (index < m_numAddresses)
    {
        result = m_addresses[index];
    }
    else
    {
        bdUseVAArgs(index);
    }

    return result;
}




void bdGetHostByName::cancelLookup()
{
    if (m_status == BD_LOOKUP_PENDING)
    {
        bdPlatformSocket::abortGetHostByName(m_handle);
        m_status = BD_LOOKUP_CANCELLED;
    }
    else
    {
        bdUseVAArgs();
    }
}
