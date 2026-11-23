// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.cpp
#line 3 "C:/Project/Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.cpp" // the original build path: NN_LOG in publish() compiles __FILE__ in
#include "bdPlatformLog.h"
#include "../bdPlatformMemory/bdPlatformMemory.inl"
#include "../bdPlatformString/bdPlatformString.inl"
#include "../../../../../NintendoSDK/Include/nn/diag/diag_LogTypes.h"






















bdLogSubscriber * g_logSubscriberList = NULL;

bdLogSubscriber::bdLogSubscriber()
    : bdLinkable()
{
    bdMemset(m_channels, 0, sizeof(m_channels));
}




bdLogSubscriber::~bdLogSubscriber()
{
}

bdBool bdLogSubscriber::addChannel(const bdNChar8 *const channel)
{
    bdBool added = false;

    for (bdUInt i = 0; !added && i < 5; ++i)
    {
        if (m_channels[i] == NULL)
        {
            m_channels[i] = channel;
            added = true;
        }
    }

    if (added)
    {
        if (!bdLinkable::find(g_logSubscriberList, this))
        {
            link(reinterpret_cast<bdLinkable **>(&g_logSubscriberList));
        }
    }

    return added;
}
































void bdLogSubscriber::publish(const bdLogMessageType type,
                              const bdNChar8 *const,
                              const bdNChar8 *const file,
                              const bdNChar8 *const,
                              const bdUInt line,
                              const bdNChar8 *const msg) const
{

    const bdNChar8 * level = (type == BD_LOG_WARNING) ? "WARN" : ((type == BD_LOG_ERROR) ? "ERR " : "INFO");

    const bdNChar8 * filename = file;










    const bdNChar8 * fnameOld = bdStrrchr(file, '\\');
    if (fnameOld == NULL)
    {
        fnameOld = bdStrrchr(file, '/');
        if (fnameOld != NULL)
        {
            filename = fnameOld + 1;
        }
    }
    else
    {
        filename = fnameOld + 1;
    }









    fnameOld = bdStrrchr(filename, '/');
    if (fnameOld != NULL)
    {
        filename = fnameOld + 1;
    }
























    NN_LOG("%s %s(%3u): %s\n", level, filename, line, msg);
}































































void bdUseVAArgs(...)
{

}
