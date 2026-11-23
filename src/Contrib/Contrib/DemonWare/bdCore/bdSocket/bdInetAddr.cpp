#include "bdInetAddr.h"
#include "../bdUtilities/bdBytePacker.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

bdInetAddr bdInetAddr::Broadcast() { return Broadcast(2); }
bdInetAddr bdInetAddr::Any() { return Any(2); }

inline bdInetAddr bdInetAddr::Broadcast(bdUInt16 af_family)
{
    bdInetAddr addr;
    if (af_family == 23) {
        // IPv6 spelling is retained from the sibling source; this branch
        // is not exercised by the surviving Broadcast() caller.
        addr.set("ff02::1");
        addr.m_addr.m_family = 23;
    } else {
        addr.set(0xffffffffu);
        addr.m_addr.m_family = 2;
    }
    return addr;
}

bdInetAddr bdInetAddr::Any(bdUInt16 af_family)
{
    bdInetAddr addr;
    if (af_family == 23) {
        addr.set("::");
        addr.m_addr.m_family = 23;
    } else {
        addr.set(0u);
        addr.m_addr.m_family = 2;
    }
    return addr;
}

bdInetAddr::~bdInetAddr() { m_addr.inUn.m_iaddr = 0xdeadbeef; }
bdInetAddr::bdInetAddr(const bdInAddr inaddr) : m_addr(inaddr) {}
bdInetAddr::bdInetAddr(const bdUInt address) { set(address); }
bdInetAddr::bdInetAddr() {}
bdInetAddr::bdInetAddr(const bdInetAddr & other) { m_addr = other.m_addr; }
bdInetAddr::bdInetAddr(const bdNChar8 * address) { set(address); }

void bdInetAddr::set(const bdInetAddr & other)
{
    if (other.m_addr.m_family != 23) m_addr = other.m_addr;
}
void bdInetAddr::set(const bdUInt address)
{
    m_addr.inUn.m_iaddr = address;
    m_addr.m_family = 2;
}
void bdInetAddr::set(const bdInAddr inaddr) { m_addr = inaddr; }
void bdInetAddr::set(const bdNChar8 * address) { m_addr.fromString(address); }

bdBool bdInetAddr::operator==(const bdInetAddr & other) const
{
    bdBool result = false;
    if (other.m_addr.m_family != 23) result = m_addr.inUn.m_iaddr == other.m_addr.inUn.m_iaddr;
    return result;
}
bdUInt bdInetAddr::toUInt32() const
{
    bdUInt result = 0;
    if (m_addr.m_family == 2) result = m_addr.inUn.m_iaddr;
    return result;
}
bdBool bdInetAddr::operator!=(const bdInetAddr & other) const { return !(*this == other); }
bdBool bdInetAddr::isValid() const { return !(*this == bdInetAddr()); }

bdUWord bdInetAddr::toString(bdNChar8 *const str, const bdUWord size) const
{
    bdUWord strLength = 0;
    bdMemset(str, 0, size);
    if (isValid()) strLength = m_addr.toString(str, size);
    return strLength;
}
bdInAddr bdInetAddr::getInAddr() const { return m_addr; }
bdBool bdInetAddr::serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const
{
    bdBool ok = false;
    if (m_addr.m_family != 23) ok = bdBytePacker::appendBuffer(data, size, offset, newOffset, &m_addr.inUn.m_iaddr, 4);
    return ok;
}
bdBool bdInetAddr::deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset)
{
    bdBool ok = false;
    if (m_addr.m_family != 23) ok = bdBytePacker::removeBuffer(data, size, offset, newOffset, &m_addr.inUn.m_iaddr, 4);
    return ok;
}
