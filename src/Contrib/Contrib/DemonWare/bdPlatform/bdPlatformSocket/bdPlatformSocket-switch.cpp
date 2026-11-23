// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdPlatformSocket-switch.cpp

#include "bdPlatformSocket.h"
#include "../bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatformMemory/bdPlatformMemory.inl"
#include "../../../../../NintendoSDK/Include/nn/socket/socket_Api.h"
#include "../../../../../NintendoSDK/Include/nn/socket/netdb.h"

// Invented names: an assert-style log macro whose DWARF leaves a block-scoped `exprValue`,
// and one switch case per errno value that returns the errno's name.
#define bdAssertMsg(expr, format, ...) { const bdBool exprValue = (expr); if (!exprValue) { bdLogError("assert", format, ##__VA_ARGS__); } }
#define BD_ERRNO_CASE(value, name) case value: return #name;

bdUInt64 bdPlatformSocket::m_totalBytesSent = 0;
bdUInt64 bdPlatformSocket::m_totalPacketsSent = 0;
bdUInt64 bdPlatformSocket::m_totalBytesRecvd = 0;
bdUInt64 bdPlatformSocket::m_totalPacketsRecvd = 0;
bdBool bdPlatformSocket::m_adHoc = false;
bdInt bdPlatformSocket::create(const bdBool blocking, const bdBool broadcast)
{
    bdInt handle = nn::socket::Socket(AF_INET, SOCK_DGRAM, 0);
    if (handle >= 0)
    {
        const bdInt flags = nn::socket::Fcntl(handle, F_GETFL);
        const bdBool ok = nn::socket::Fcntl(handle, F_SETFL, !blocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) == 0;
        if (ok)
        {
            bdInt broadcastEnabled = broadcast;
            if (nn::socket::SetSockOpt(handle, SOL_SOCKET, SO_BROADCAST, &broadcastEnabled, sizeof(broadcastEnabled)) == 0)
            {
                return handle;
            }
        }
        bdLogError("platform socket", "socket setup failed: %s", platformErrToString(nn::socket::GetLastErrno()));
        close(handle);
    }
    else
    {
        bdLogError("platform socket", "socket failed: %s", platformErrToString(nn::socket::GetLastErrno()));
    }
    return -1;
}













bdSocketStatusCode bdPlatformSocket::bind(bdInt & handle, const bdInAddr addr, const bdUInt16 port)
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

    bdInt result = nn::socket::Bind(handle, reinterpret_cast<sockaddr *>(&sockAddr), sizeof(sockAddr));
    if (result != 0)
    {
        result = nn::socket::GetLastErrno();
        close(handle);
        bdLogError("platform socket", "bind failed: %s", platformErrToString(result));
    }
    return platformErrToBDErr(result);
}




bdInt bdPlatformSocket::sendTo(const bdInt handle, const bdInAddr addr, const bdUInt16 port, const void * data, const bdUInt length)
{
    bdInt result;
    if (data == NULL)
    {
        bdAssertMsg(false, "data is NULL");
        result = BD_NET_ERROR;
    }
    else if (handle < 0)
    {
        result = BD_NET_INVALID_HANDLE;
    }
    else
    {
        sockaddr_in sockAddr;
        bdMemset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_len = sizeof(sockAddr);
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = nn::socket::InetHtons(port);
        sockAddr.sin_addr.s_addr = addr.inUn.m_iaddr;
        bdInt flags = 0;
        result = nn::socket::SendTo(handle, data, length, flags, reinterpret_cast<sockaddr *>(&sockAddr), sizeof(sockAddr));
        if (result >= 0)
        {
            m_totalBytesSent += result;
            ++m_totalPacketsSent;
        }
        else
        {
            result = platformErrToBDErr(nn::socket::GetLastErrno());
        }
    }
    return result;
}





bdInt bdPlatformSocket::receiveFrom(const bdInt handle, bdInAddr & addr, bdUInt16 & port, void * data, const bdUInt size)
{
    bdInt result;
    if (data == NULL)
    {
        bdAssertMsg(false, "data is NULL");
        result = BD_NET_ERROR;
    }
    else if (handle < 0)
    {
        result = BD_NET_INVALID_HANDLE;
    }
    else
    {
        bdInt flags = 0;
        socklen_t sock_length = sizeof(sockaddr_in);
        sockaddr_in sockAddr;
        bdMemset(&sockAddr, 0, sizeof(sockAddr));
        result = nn::socket::RecvFrom(handle, data, size, flags, reinterpret_cast<sockaddr *>(&sockAddr), &sock_length);
        if (result >= 0)
        {
            addr.inUn.m_iaddr = sockAddr.sin_addr.s_addr;
            port = nn::socket::InetNtohs(sockAddr.sin_port);
            m_totalBytesRecvd += result;
            ++m_totalPacketsRecvd;
        }
        else
        {
            result = platformErrToBDErr(nn::socket::GetLastErrno());
        }
    }
    return result;
}





bdBool bdPlatformSocket::close(bdInt & handle)
{
    bdBool result = true;
    if (handle != -1)
    {
        if (nn::socket::Close(handle) != 0)
        {
            bdLogError("platform socket", "close failed: %s", platformErrToString(nn::socket::GetLastErrno()));
            result = false;
        }
        handle = -1;
    }
    return result;
}



bdUInt bdPlatformSocket::getHostByName(const bdNChar8 *const name, bdInAddr * addresses, bdUInt numAddresses, const bdDNSHandle)
{
    bdUInt numResolvedIps = 0;
    if (!m_adHoc)
    {
        addrinfo * queryResult = NULL;
        addrinfo hints;
        bdMemset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        const bdInt error = nn::socket::GetAddrInfo(name, NULL, &hints, &queryResult);
        switch (error)
        {
        case 0:
            if (queryResult != NULL)
            {
                for (addrinfo * addr = queryResult; addr != NULL && numResolvedIps < numAddresses; addr = addr->ai_next)
                {
                    sockaddr_in inAddr;
                    bdMemset(&inAddr, 0, sizeof(inAddr));
                    bdMemcpy(&inAddr, addr->ai_addr, addr->ai_addrlen);
                    addresses[numResolvedIps++].inUn.m_iaddr = inAddr.sin_addr.s_addr;
                }
                nn::socket::FreeAddrInfo(queryResult);
            }
            break;
        case -1:
        {
            const bdInt internalError = nn::socket::GetLastErrno();
            bdLogError("platform socket", "getaddrinfo failed: %d", internalError);
            break;
        }
        case 1:
            bdLogWarn("platform socket", "host not found: %s", name);
            break;
        default:
            bdLogError("platform socket", "getaddrinfo failed: %d", error);
            break;
        }
    }
    else
    {
        bdLogWarn("platform socket", "DNS not available in ad hoc mode");
    }
    return numResolvedIps;
}









void bdPlatformSocket::abortGetHostByName(bdDNSHandle)
{
}

bdSocketStatusCode bdPlatformSocket::platformErrToBDErr(const bdInt error)
{
    bdSocketStatusCode socketStatusCode;
    switch (error)
    {
    case 0:
        socketStatusCode = BD_NET_SUCCESS;
        break;
    case EINVAL:
    case EOPNOTSUPP:
    case EAFNOSUPPORT:
        socketStatusCode = BD_NET_ADDRESS_INVALID;
        break;
    case EBADF:
    case ENOTSOCK:
        socketStatusCode = BD_NET_INVALID_HANDLE;
        break;
    case EAGAIN:
    case EINPROGRESS:
        socketStatusCode = BD_NET_WOULD_BLOCK;
        break;
    case ECONNABORTED:
    case ECONNRESET:
        socketStatusCode = BD_NET_CONNECTION_RESET;
        break;
    case EMFILE:
        socketStatusCode = BD_NET_SUBSYTEM_ERROR;
        break;
    case EADDRINUSE:
        socketStatusCode = BD_NET_ADDRESS_IN_USE;
        break;
    case ENOTCONN:
        socketStatusCode = BD_NET_NOT_CONNECTED;
        break;
    default:
        socketStatusCode = BD_NET_ERROR;
        break;
    }
    return socketStatusCode;
}


const bdNChar8 *const bdPlatformSocket::platformErrToString(const bdInt error)
{
    switch (error)
    {
    BD_ERRNO_CASE(1, EPERM)
    BD_ERRNO_CASE(2, ENOENT)
    BD_ERRNO_CASE(3, ESRCH)
    BD_ERRNO_CASE(4, EINTR)
    BD_ERRNO_CASE(5, EIO)
    BD_ERRNO_CASE(6, ENXIO)
    BD_ERRNO_CASE(7, E2BIG)
    BD_ERRNO_CASE(8, ENOEXEC)
    BD_ERRNO_CASE(9, EBADF)
    BD_ERRNO_CASE(10, ECHILD)
    BD_ERRNO_CASE(11, EWOULDBLOCK)
    BD_ERRNO_CASE(12, ENOMEM)
    BD_ERRNO_CASE(13, EACCES)
    BD_ERRNO_CASE(14, EFAULT)
    BD_ERRNO_CASE(15, ENOTBLK)
    BD_ERRNO_CASE(16, EBUSY)
    BD_ERRNO_CASE(17, EEXIST)
    BD_ERRNO_CASE(18, EXDEV)
    BD_ERRNO_CASE(19, ENODEV)
    BD_ERRNO_CASE(20, ENOTDIR)
    BD_ERRNO_CASE(21, EISDIR)
    BD_ERRNO_CASE(22, EINVAL)
    BD_ERRNO_CASE(23, ENFILE)
    BD_ERRNO_CASE(24, EMFILE)
    BD_ERRNO_CASE(25, ENOTTY)
    BD_ERRNO_CASE(26, ETXTBSY)
    BD_ERRNO_CASE(27, EFBIG)
    BD_ERRNO_CASE(28, ENOSPC)
    BD_ERRNO_CASE(29, ESPIPE)
    BD_ERRNO_CASE(30, EROFS)
    BD_ERRNO_CASE(31, EMLINK)
    BD_ERRNO_CASE(32, EPIPE)
    BD_ERRNO_CASE(33, EDOM)
    BD_ERRNO_CASE(34, ERANGE)
    BD_ERRNO_CASE(35, EDEADLK)
    BD_ERRNO_CASE(36, ENAMETOOLONG)
    BD_ERRNO_CASE(37, ENOLCK)
    BD_ERRNO_CASE(38, ENOSYS)
    BD_ERRNO_CASE(39, ENOTEMPTY)
    BD_ERRNO_CASE(40, ELOOP)
    BD_ERRNO_CASE(42, ENOMSG)
    BD_ERRNO_CASE(43, EIDRM)
    BD_ERRNO_CASE(44, ECHRNG)
    BD_ERRNO_CASE(45, EL2NSYNC)
    BD_ERRNO_CASE(46, EL3HLT)
    BD_ERRNO_CASE(47, EL3RST)
    BD_ERRNO_CASE(48, ELNRNG)
    BD_ERRNO_CASE(49, EUNATCH)
    BD_ERRNO_CASE(50, ENOCSI)
    BD_ERRNO_CASE(51, EL2HLT)
    BD_ERRNO_CASE(52, EBADE)
    BD_ERRNO_CASE(53, EBADR)
    BD_ERRNO_CASE(54, EXFULL)
    BD_ERRNO_CASE(55, ENOANO)
    BD_ERRNO_CASE(56, EBADRQC)
    BD_ERRNO_CASE(57, EBADSLT)
    BD_ERRNO_CASE(59, EBFONT)
    BD_ERRNO_CASE(60, ENOSTR)
    BD_ERRNO_CASE(61, ENODATA)
    BD_ERRNO_CASE(62, ETIME)
    BD_ERRNO_CASE(63, ENOSR)
    BD_ERRNO_CASE(64, ENONET)
    BD_ERRNO_CASE(65, ENOPKG)
    BD_ERRNO_CASE(66, EREMOTE)
    BD_ERRNO_CASE(67, ENOLINK)
    BD_ERRNO_CASE(68, EADV)
    BD_ERRNO_CASE(69, ESRMNT)
    BD_ERRNO_CASE(70, ECOMM)
    BD_ERRNO_CASE(71, EPROTO)
    BD_ERRNO_CASE(72, EMULTIHOP)
    BD_ERRNO_CASE(73, EDOTDOT)
    BD_ERRNO_CASE(74, EBADMSG)
    BD_ERRNO_CASE(75, EOVERFLOW)
    BD_ERRNO_CASE(76, ENOTUNIQ)
    BD_ERRNO_CASE(77, EBADFD)
    BD_ERRNO_CASE(78, EREMCHG)
    BD_ERRNO_CASE(79, ELIBACC)
    BD_ERRNO_CASE(80, ELIBBAD)
    BD_ERRNO_CASE(81, ELIBSCN)
    BD_ERRNO_CASE(82, ELIBMAX)
    BD_ERRNO_CASE(83, ELIBEXEC)
    BD_ERRNO_CASE(84, EILSEQ)
    BD_ERRNO_CASE(85, ERESTART)
    BD_ERRNO_CASE(86, ESTRPIPE)
    BD_ERRNO_CASE(87, EUSERS)
    BD_ERRNO_CASE(88, ENOTSOCK)
    BD_ERRNO_CASE(89, EDESTADDRREQ)
    BD_ERRNO_CASE(90, EMSGSIZE)
    BD_ERRNO_CASE(91, EPROTOTYPE)
    BD_ERRNO_CASE(92, ENOPROTOOPT)
    BD_ERRNO_CASE(93, EPROTONOSUPPORT)
    BD_ERRNO_CASE(94, ESOCKTNOSUPPORT)
    BD_ERRNO_CASE(95, EOPNOTSUPP)
    BD_ERRNO_CASE(96, EPFNOSUPPORT)
    BD_ERRNO_CASE(97, EAFNOSUPPORT)
    BD_ERRNO_CASE(98, EADDRINUSE)
    BD_ERRNO_CASE(99, EADDRNOTAVAIL)
    BD_ERRNO_CASE(100, ENETDOWN)
    BD_ERRNO_CASE(101, ENETUNREACH)
    BD_ERRNO_CASE(102, ENETRESET)
    BD_ERRNO_CASE(103, ECONNABORTED)
    BD_ERRNO_CASE(104, ECONNRESET)
    BD_ERRNO_CASE(105, ENOBUFS)
    BD_ERRNO_CASE(106, EISCONN)
    BD_ERRNO_CASE(107, ENOTCONN)
    BD_ERRNO_CASE(108, ESHUTDOWN)
    BD_ERRNO_CASE(109, ETOOMANYREFS)
    BD_ERRNO_CASE(110, ETIMEDOUT)
    BD_ERRNO_CASE(111, ECONNREFUSED)
    BD_ERRNO_CASE(112, EHOSTDOWN)
    BD_ERRNO_CASE(113, EHOSTUNREACH)
    BD_ERRNO_CASE(114, EALREADY)
    BD_ERRNO_CASE(115, EINPROGRESS)
    BD_ERRNO_CASE(116, ESTALE)
    BD_ERRNO_CASE(117, EUCLEAN)
    BD_ERRNO_CASE(118, ENOTNAM)
    BD_ERRNO_CASE(119, ENAVAIL)
    BD_ERRNO_CASE(120, EISNAM)
    BD_ERRNO_CASE(121, EREMOTEIO)
    BD_ERRNO_CASE(122, EDQUOT)
    BD_ERRNO_CASE(123, ENOMEDIUM)
    BD_ERRNO_CASE(124, EMEDIUMTYPE)
    BD_ERRNO_CASE(125, ECANCELED)
    BD_ERRNO_CASE(126, ENOKEY)
    BD_ERRNO_CASE(127, EKEYEXPIRED)
    BD_ERRNO_CASE(128, EKEYREVOKED)
    BD_ERRNO_CASE(129, EKEYREJECTED)
    BD_ERRNO_CASE(130, EOWNERDEAD)
    BD_ERRNO_CASE(131, ENOTRECOVERABLE)
    BD_ERRNO_CASE(132, ERFKILL)
    BD_ERRNO_CASE(133, EHWPOISON)
    }

    return "Unknown error in bdPlatformSocket::platformErrToString's lookup table";
}
