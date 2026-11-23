#include "Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSocket.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdSocket/bdAddr.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdSocket/bdInetAddr.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformSocket.h"

// Blank lines keep the original line numbers (DWARF `defined at` lines and the line table).








bdSocket::bdSocket()
    : m_handle(-1)
{

}

bdSocket::~bdSocket()
{
    close();
}

bdBool bdSocket::create(const bdBool blocking, const bdBool broadcast, const bdBool)
{









    return create(2, blocking, broadcast, false);  // AF_INET
}



bdBool bdSocket::create(const bdUInt16 af_family, const bdBool blocking, const bdBool broadcast, const bdBool)









{
    bdBool success = false;
    if (m_handle == -1)
    {







        const bdInt createRes = bdPlatformSocket::create(blocking, broadcast);



        if (createRes < 0)
        {
            bdLogWarn("socket", "Failed to create socket: %d.", createRes);
        }
        else
        {
            m_handle = createRes;
            success = true;
        }
    }
    else
    {
        bdLogWarn("socket", "Socket already created.");
    }
    return success;
}

bdSocketStatusCode bdSocket::bind(const bdPort port)
{




    return bind(2, port);  // AF_INET
}

bdSocketStatusCode bdSocket::bind(const bdUInt16 af_family, const bdPort port)
{







    bdAddr address(bdInetAddr::Any(af_family), port);
    return bdSocket::bind(address);
}

bdSocketStatusCode bdSocket::bind(const bdAddr &addr)
{
    return bdPlatformSocket::bind(m_handle,
        addr.getAddress().getInAddr(),
        addr.getPort());
}

bdInt bdSocket::sendTo(const bdAddr &addr,
    const void *data,
    const bdUInt length)
{
    return bdPlatformSocket::sendTo(m_handle,
        addr.getAddress().getInAddr(),
        addr.getPort(), data, length);
}



bdInt bdSocket::receiveFrom(bdAddr &addr,
    void *data,
    const bdUInt size)
{
    bdInAddr inaddr;
    bdPort port;
    bdInt status = bdPlatformSocket::receiveFrom(m_handle, inaddr, port, data, size);
    if (status >= 0 || status == BD_NET_CONNECTION_RESET)
    {
        addr.set(bdInetAddr(inaddr), port);
    }

    return status;
}

bdBool bdSocket::close()
{
    return bdPlatformSocket::close(m_handle);
}
