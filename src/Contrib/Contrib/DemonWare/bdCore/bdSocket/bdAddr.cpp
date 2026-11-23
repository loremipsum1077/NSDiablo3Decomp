#include "bdAddr.h"
#include "../bdUtilities/bdBytePacker.h"

#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

bdAddr::bdAddr() : m_address(), m_port(0) {}
bdAddr::bdAddr(const bdAddr &other) : m_address(other.m_address), m_port(other.m_port) {}
bdAddr::bdAddr(const bdInetAddr &address, const bdPort port) : m_address(address), m_port(port) {}
bdAddr::bdAddr(const bdNChar8 *socketAddress) : m_address() { set(socketAddress); }
void bdAddr::set(const bdNChar8 *socketAddress)
{
    const bdNChar8 *const sep = bdStrchr(socketAddress, ':');
    if (sep) {
        bdNChar8 strAddr[16];
        const bdUInt pos = static_cast<bdUInt>(sep - socketAddress + 1);
        const bdUInt strAddrSize = sizeof(strAddr);
        bdMemcpy(strAddr, socketAddress, pos < strAddrSize ? pos : strAddrSize);
        strAddr[pos > strAddrSize ? strAddrSize - 1 : pos - 1] = 0;
        m_address.set(bdSockAddr(strAddr));
        m_port = static_cast<bdPort>(bdStrtoui32(sep + 1, NULL, 10));
    } else {
        set(bdInetAddr(socketAddress), 0);
    }
}
void bdAddr::set(const bdInetAddr &address, const bdPort port) { m_address.set(address); m_port = port; }
bdBool bdAddr::operator==(const bdAddr &other) const { return m_port == other.m_port && m_address == other.m_address; }
bdBool bdAddr::operator!=(const bdAddr &other) const { return m_port != other.m_port || m_address != other.m_address; }
const bdInetAddr &bdAddr::getAddress() const { return m_address; }
bdInetAddr &bdAddr::getAddress() { return m_address; }
void bdAddr::setPort(const bdPort port) { m_port = port; }
bdPort bdAddr::getPort() const { return m_port; }
bdUWord bdAddr::toString(bdNChar8 *const str, const bdUWord size) const
{
    bdUWord strLength = m_address.toString(str, size);
    bdUWord sizeLeft = size >= strLength ? size - strLength : 0;
    return strLength + bdSnprintf(str + strLength, sizeLeft, ":%u", m_port);
}
bdUInt bdAddr::getHash() const
{
    bdNChar8 val[1288];
    bdUInt len = 0;
    bdUInt hash = 0;
    if (serialize(val, sizeof(val), 0, len)) {
        for (bdUInt i = 0; i < len; ++i) hash = hash * 16777619U ^ static_cast<bdUByte8>(val[i]);
    }
    return hash;
}
bdBool bdAddr::serialize(void *data, const bdUInt size, const bdUInt offset, bdUInt &newOffset) const
{
    newOffset = offset;
    bdBool status = m_address.serialize(data, size, offset, newOffset);
    status = status && bdBytePacker::appendBasicType(data, size, newOffset, newOffset, m_port);
    if (!status) newOffset = offset;
    return status;
}
bdBool bdAddr::deserialize(const void *data, const bdUInt size, const bdUInt offset, bdUInt &newOffset)
{
    newOffset = offset;
    bdBool status = m_address.deserialize(data, size, offset, newOffset);
    status = status && bdBytePacker::removeBasicType(data, size, newOffset, newOffset, m_port);
    if (!status) newOffset = offset;
    return status;
}
