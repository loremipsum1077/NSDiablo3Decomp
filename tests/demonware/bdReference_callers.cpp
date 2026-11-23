// Small original functions that inline bdReference members which have no out-of-line copy.
// The classes are stand-ins with only the members these functions touch at their original
// offsets (not the real declarations); the function bodies are what the line table implies.
#include "Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"

class bdCommonAddr : public bdReferencable {
public:
    bdBool operator==(const bdCommonAddr& other) const;
};
class bdAddrHandle : public bdReferencable {};
class bdByteBuffer : public bdReferencable {};

// copy constructor (bdConnection.cpp:20), operator=(const bdReference&) (bdConnection.cpp:30),
// bdReferencable() and bdReference() (bdConnection.cpp:86)
class bdConnection : public bdReferencable {
public:
    bdConnection();
    virtual ~bdConnection();
    bdReference<bdCommonAddr> getAddress() const;
    void setAddressHandle(const bdReference<bdAddrHandle>& addr);
    bdReference<bdCommonAddr> m_addr;        // 0x10
    bdReference<bdAddrHandle> m_addrHandle;  // 0x18
    bdReference<bdReferencable> m_refs[3];   // 0x20 (stand-in for three more references)
};

bdConnection::bdConnection()
{
}

bdReference<bdCommonAddr> bdConnection::getAddress() const
{
    return m_addr;
}

void bdConnection::setAddressHandle(const bdReference<bdAddrHandle>& addr)
{
    m_addrHandle = addr;
}

// notNull (bdMessage.cpp:63)
class bdMessage : public bdReferencable {
public:
    bdBool hasPayload() const;
    bdReference<bdByteBuffer> m_payload;  // 0x10
};

bdBool bdMessage::hasPayload() const
{
    return m_payload.notNull();
}

// operator=(const bdReference&) from a by-value argument (bdGameInfo.cpp:73)
class bdGameInfo : public bdReferencable {
public:
    void setHostAddr(bdReference<bdCommonAddr> hostAddr);
    char pad[0x18];
    bdReference<bdCommonAddr> m_hostAddr;  // 0x28
};

void bdGameInfo::setHostAddr(bdReference<bdCommonAddr> hostAddr)
{
    m_hostAddr = hostAddr;
}

// operator=(T*) (bdRemoteTask.cpp:508)
class bdStopwatch {
public:
    void start();
    bdUInt64 m_start;
};
class bdRemoteTask : public bdReferencable {
public:
    void start(bdFloat32 timeout);
    bdStopwatch m_timer;                      // 0x10
    bdFloat32 m_timeout;                      // 0x18
    bdInt m_status;                           // 0x1c
    bdReference<bdByteBuffer> m_byteResults;  // 0x20
};

void bdRemoteTask::start(bdFloat32 timeout)
{
    m_timer.start();
    m_timeout = timeout;
    m_status = 1;
    m_byteResults = NULL;
}

// operator== and operator-> const, operator* (Online.cpp:595)
typedef bdReference<bdCommonAddr> bdCommonAddrRef;
typedef int BOOL;
BOOL IsSameAddress(bdCommonAddrRef ptAddr1, bdCommonAddrRef ptAddr2)
{
    return ptAddr1 == ptAddr2 ||
           (ptAddr1.notNull() && ptAddr2.notNull() && *ptAddr1 == *ptAddr2);
}
