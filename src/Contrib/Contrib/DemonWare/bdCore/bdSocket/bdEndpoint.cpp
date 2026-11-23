// Recovered from DWARF and instruction evidence.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdEndpoint.cpp
#include "bdEndpoint.h"
#include "bdCommonAddr.h"
#include "../bdContainers/bdHashMap.h"












bdEndpoint::bdEndpoint(const bdCommonAddrRef addr, const bdSecurityID & secID)
    : m_ca(addr), m_secID(secID)
{
}



bdBool bdEndpoint::operator==(const bdEndpoint & other) const
{
    bdBool result = m_ca.isNull() && other.m_ca.isNull();

    if (m_ca.notNull() && other.m_ca.notNull())
    {
        result = m_ca->getHash() == other.m_ca->getHash() && m_secID == other.m_secID;
    }
    return result;
}














bdCommonAddrRef bdEndpoint::getCommonAddr() const
{
    return m_ca;
}

const bdSecurityID & bdEndpoint::getSecID() const
{
    return m_secID;
}

bdUInt bdEndpoint::getHash() const
{
    bdUInt hash = 0;
    if (getCommonAddr().notNull())
    {
        hash = getCommonAddr()->getHash();
        bdHashingClass hasher;
        hash += hasher.getHash(m_secID);
    }
    return hash;
}

bdUInt bdEndpointHashingClass::getHash(const bdEndpoint & other) const
{
    return other.getHash();
}
