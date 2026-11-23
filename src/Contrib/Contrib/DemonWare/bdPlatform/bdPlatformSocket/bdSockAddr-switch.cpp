#include "bdSockAddr.h"
#include "../../../../../NintendoSDK/Include/nn/socket/socket_Api.h"
#include <string.h>

bdSockAddr::bdSockAddr()
{
    m_family = 2;
    inUn.m_iaddr = 0xff00ff00;
}

bdSockAddr::bdSockAddr(const bdNChar8 * addr)
{
    m_family = 2;
    inUn.m_iaddr = 0xff00ff00;
    fromString(addr);
}

void bdSockAddr::fromString(const bdNChar8 * addr)
{
    nn::socket::InetPton(2, addr, &inUn);
}

bdUWord bdSockAddr::toString(bdNChar8 *const str, const bdUWord size) const
{
    const bdNChar8 * result = nn::socket::InetNtop(2, &inUn, str, size);
    return result ? strlen(result) : 0;
}
