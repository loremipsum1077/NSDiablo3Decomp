// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Networking/XNetAddress.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"

class CRefString;
struct XNetAddressInternal;

typedef uint32 XNetNode;

typedef uint16 XNetPort;

typedef uint8 XNetAddressOpaque[16];

class XNetAddress {  // size 0x10
public:
    XNetAddress();
    XNetAddress(XNetNode, XNetPort);
    XNetAddress(const XNetAddress &);
    XNetAddress & operator=(const XNetAddress & tAddress);
    static XNetAddress FromString(const char * pszAddress, XNetPort tDefaultPort);
    CRefString ToString(BOOL bIncludePort) const;
    XNetNode Node() const;
    XNetPort Port() const;
    void NodeAndPort(XNetNode &, XNetPort &) const;
    void SetNodeAndPort(XNetNode tNode, XNetPort tPort);
    void SetBroadcast(XNetPort tPort);
    void SetLoopback(XNetPort tPort);
    void SetAny(XNetPort tPort);
    BOOL IsBroadcast() const;
    BOOL IsLoopback() const;
    BOOL IsAny() const;
    const XNetAddressInternal & Internal() const;
    XNetAddressInternal & Internal();
private:
    XNetAddressOpaque m_tOpaque;  // 0x00
};
static_assert(sizeof(XNetAddress) == 0x10, "XNetAddress");

