// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformStreamSocket-switch.cpp

#include "bdPlatformStreamSocket.h"
#include "../bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatformMemory/bdPlatformMemory.inl"
#include "../../../../../NintendoSDK/Include/nn/socket/socket_Api.h"

// Invented name: an assert-style log macro whose DWARF leaves a block-scoped `exprValue`.
#define bdAssertMsg(expr, format, ...) \
    { \
        const bdBool exprValue = (expr); \
        if (!exprValue) \
        { \
            bdLogError("assert", format, ##__VA_ARGS__); \
        } \
    }

bdUInt64 bdPlatformStreamSocket::m_totalBytesSent = 0;
bdUInt64 bdPlatformStreamSocket::m_totalBytesRecvd = 0;

bdInt bdPlatformStreamSocket::create(const bdBool blocking)
{
    bdInt handle = nn::socket::Socket(AF_INET, SOCK_STREAM, 0);
    if (handle >= 0)
    {
        const bdInt flags = nn::socket::Fcntl(handle, F_GETFL);
        const bdBool ok = nn::socket::Fcntl(handle, F_SETFL, !blocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) >= 0;
        if (!ok)
        {
            bdLogError("platform socket", "fcntl failed: %s", bdPlatformSocket::platformErrToString(nn::socket::GetLastErrno()));
            close(handle);
        }
    }
    else
    {
        bdLogError("platform socket", "socket failed: %s", bdPlatformSocket::platformErrToString(nn::socket::GetLastErrno()));
        handle = -1;
    }
    return handle;
}



































bdSocketStatusCode bdPlatformStreamSocket::connect(const bdInt handle, const bdInAddr addr, const bdUInt16 port)
{
    if (handle < 0)
    {
        return BD_NET_INVALID_HANDLE;
    }

    sockaddr_in sockAddr;
    bdMemset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_len = sizeof(sockAddr);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = nn::socket::InetHtons(port);
    sockAddr.sin_addr.s_addr = addr.inUn.m_iaddr;

    bdInt result = nn::socket::Connect(handle, reinterpret_cast<sockaddr *>(&sockAddr), sizeof(sockAddr));
    if (result == -1)
    {
        result = nn::socket::GetLastErrno();
        if (result != EINPROGRESS)
        {
            bdLogError("platform socket", "connect failed: %s", bdPlatformSocket::platformErrToString(result));
            close(handle);
        }
    }
    return bdPlatformSocket::platformErrToBDErr(result);
}





































bdInt bdPlatformStreamSocket::send(const bdInt handle, const void * data, const bdUInt length)
{
    if (data == NULL)
    {
        bdAssertMsg(false, "data is NULL");
        return -1;
    }
    if (handle < 0)
    {
        return BD_NET_INVALID_HANDLE;
    }

    bdInt flags = 0;
    bdInt result = nn::socket::Send(handle, data, length, flags);
    if (result >= 0)
    {
        m_totalBytesSent += result;
    }
    else
    {
        result = bdPlatformSocket::platformErrToBDErr(nn::socket::GetLastErrno());
    }
    return result;
}




bdInt bdPlatformStreamSocket::receive(const bdInt handle, void *const data, const bdUInt size)
{
    if (data == NULL)
    {
        bdAssertMsg(false, "data is NULL");
        return -1;
    }
    if (handle < 0)
    {
        return BD_NET_INVALID_HANDLE;
    }

    bdInt flags = 0;
    bdInt result = nn::socket::Recv(handle, data, size, flags);
    if (result >= 0)
    {
        m_totalBytesRecvd += result;
    }
    else
    {
        result = bdPlatformSocket::platformErrToBDErr(nn::socket::GetLastErrno());
    }
    return result;
}




bdBool bdPlatformStreamSocket::close(const bdInt handle)
{
    bdBool result = true;
    if (handle != -1)
    {
        if (nn::socket::Close(handle) != 0)
        {
            bdLogError("platform socket", "close failed: %s", bdPlatformSocket::platformErrToString(nn::socket::GetLastErrno()));
            result = false;
        }
    }
    return result;
}



bdBool bdPlatformStreamSocket::isWritable(const bdInt handle)
{
    bdSocketStatusCode ignored;
    return isWritable(handle, ignored);
}

bdBool bdPlatformStreamSocket::isWritable(const bdInt handle, bdSocketStatusCode & error)
{
    bdBool writable = false;
    error = BD_NET_INVALID_HANDLE;
    if (handle >= 0)
    {
        pollfd pollFd;
        pollFd.fd = handle;
        pollFd.events = POLLOUT | POLLERR;
        pollFd.revents = 0;
        const bdInt timeout = 0;
        const bdInt numFds = 1;
        const bdInt result = nn::socket::Poll(&pollFd, numFds, timeout);
        if (result < 0 || (pollFd.revents & POLLERR))
        {
            const bdInt socketError = nn::socket::GetLastErrno();
            error = bdPlatformSocket::platformErrToBDErr(socketError);
        }
        else
        {
            error = BD_NET_SUCCESS;
        }
        if (result > 0)
        {
            writable = (pollFd.revents & POLLOUT) != 0;
        }
    }
    return writable;
}
