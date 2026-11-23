#include "Contrib/Contrib/DemonWare/bdCore/bdSocket/bdStreamSocket.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdSocket/bdAddr.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformStreamSocket.h"

// Function definitions sit on their DWARF lines (16..84); blank lines pad the gaps.









bdStreamSocket::bdStreamSocket()
    : m_handle(-1)
{
}

bdStreamSocket::~bdStreamSocket()
{
}

bdBool bdStreamSocket::create(const bdBool blocking)
{



    m_handle = bdPlatformStreamSocket::create(blocking);
    const bdBool ok = m_handle != -1;
    return ok;
}

















bdSocketStatusCode bdStreamSocket::connect(const bdAddr addr) const
{
    const bdSocketStatusCode status = bdPlatformStreamSocket::connect(m_handle,
        addr.getAddress().getInAddr(),
        addr.getPort());
    return status;
}

bdBool bdStreamSocket::isConnected() const
{


    return bdPlatformStreamSocket::isWritable(m_handle);
}

bdBool bdStreamSocket::isWritable(bdSocketStatusCode & error) const
{
    error = BD_NET_SUCCESS;
    return bdPlatformStreamSocket::isWritable(m_handle, error);
}

bdInt bdStreamSocket::send(const void * data, const bdUInt length) const
{

    return bdPlatformStreamSocket::send(m_handle, data, length);
}

bdInt bdStreamSocket::recv(void * data, const bdUInt size) const
{

    return bdPlatformStreamSocket::receive(m_handle, data, size);
}

void bdStreamSocket::close()
{
    const bdBool ok = bdPlatformStreamSocket::close(m_handle);
    m_handle = -1;

    if (!ok)
    {
        bdLogWarn("bdSocket/stream", "Failed to close socket");
    }
}
