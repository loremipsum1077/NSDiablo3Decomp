// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Networking/XSocketBase.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"

class XNetAddress;
struct XSocketBaseInternal;

typedef uint8 XSocketBaseOpaque[4];

class XSocketBase {  // size 0x10
    // vtable pointer
public:
    XSocketBase();
    XSocketBase(const XSocketBase &);
    virtual ~XSocketBase();  // vtable slot 0
    XSocketBase & operator=(const XSocketBase &);
    void Reset();
    void Shutdown();
    BOOL IsClosed() const;
    void Close();
    XNetAddress SelfAddress() const;
    XNetAddress PeerAddress() const;
    const XSocketBaseInternal & Internal() const;
    XSocketBaseInternal & Internal();
    BOOL CreateStream();
    BOOL CreateDatagram();
    BOOL SetNonBlocking(BOOL);
    BOOL SetReuseAddress(BOOL);
    BOOL SetNoDelay(BOOL);
    BOOL SetWindowSize(uint32);
    BOOL SetReceiveTimeout(uint32);
    BOOL SetBroadcast(BOOL);
    enum Result {
        RESULT_SUCCESS = 0,
        RESULT_FAILURE = 1,
        RESULT_WOULDBLOCK = 2,
        RESULT_INPROGRESS = 3,
    };
    XSocketBase::Result Connect(const XNetAddress &);
    XSocketBase::Result Bind(const XNetAddress &);
    XSocketBase::Result Listen(uint32);
    XSocketBase::Result Accept(XSocketBase &, XNetAddress &);
    XSocketBase::Result Send(const void *, uint32, uint32 &);
    XSocketBase::Result Recv(void *, uint32, uint32 &);
    XSocketBase::Result SendTo(const void * pData, uint32 uBytes, const XNetAddress & tPeerAddress,
                               uint32 & uBytesSent);
    XSocketBase::Result RecvFrom(void *, uint32, XNetAddress &, uint32 &);
private:
    XSocketBaseOpaque m_tOpaque;  // 0x08
};
static_assert(sizeof(XSocketBase) == 0x10, "XSocketBase");

void ReportWinSockError(LPCSTR szMsg, const int32 nReturnVal);

