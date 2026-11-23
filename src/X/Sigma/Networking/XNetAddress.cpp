#include "XNetAddress.h"

#include <stdlib.h>
#include <string.h>

#include "../../../BlizzardCore/Include/Blizzard/Config/Platform/NX64.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Memory/SigmaMemory.h"
#include "../RefString.h"
#include "XNetAddressPrivate.h"

XNetAddress::XNetAddress() {
    XNetAddressInternal& tInternal = *reinterpret_cast<XNetAddressInternal*>(m_tOpaque);

    tInternal.m_sockAddr.sin_family = AF_INET;
    ZeroMemory(tInternal.m_sockAddr.sin_zero, sizeof(tInternal.m_sockAddr.sin_zero));
    SetAny(0);
}

XNetAddress::XNetAddress(XNetNode tNode, XNetPort tPort) {
    XNetAddressInternal& tInternal = *reinterpret_cast<XNetAddressInternal*>(m_tOpaque);

    tInternal.m_sockAddr.sin_family = AF_INET;
    ZeroMemory(tInternal.m_sockAddr.sin_zero, sizeof(tInternal.m_sockAddr.sin_zero));
    SetNodeAndPort(tNode, tPort);
}

XNetAddress& XNetAddress::operator=(const XNetAddress& tAddress) {
    memcpy(m_tOpaque, tAddress.m_tOpaque, sizeof(m_tOpaque));
    return *this;
}

XNetAddress XNetAddress::FromString(const char* pszAddress, XNetPort tDefaultPort) {
    char szTempString[128];
    Blizzard::String::Copy(szTempString, pszAddress, sizeof(szTempString));

    XNetPort tPort = tDefaultPort;
    char* pszPort = strchr(szTempString, ':');
    if (pszPort) {
        *pszPort = '\0';
        tPort = (XNetPort)atoi(pszPort + 1);
    }

    XNetNode tNode = ntohl(inet_addr(szTempString));
    if (tNode == INADDR_NONE) {
        hostent* ptHostEntry = gethostbyname(szTempString);
        if (ptHostEntry == NULL || ptHostEntry->h_addrtype != AF_INET || ptHostEntry->h_length != 4) {
            return XNetAddress();
        }

        char** hostAddr = ptHostEntry->h_addr_list;
        tNode = ntohl(*(uint32*)hostAddr[0]);
        if (tNode == INADDR_NONE) {
            return XNetAddress();
        }
    }

    return XNetAddress(tNode, tPort);
}

CRefString XNetAddress::ToString(BOOL bIncludePort) const {
    CRefString s;

    XNetNode tNode = Node();
    XNetPort tPort = Port();
    if (bIncludePort) {
        s.Format("%d.%d.%d.%d:%d", tNode >> 24, (tNode >> 16) & 0xff, (tNode >> 8) & 0xff, tNode & 0xff, tPort);
    } else {
        s.Format("%d.%d.%d.%d", tNode >> 24, (tNode >> 16) & 0xff, (tNode >> 8) & 0xff, tNode & 0xff);
    }

    return s;
}

XNetNode XNetAddress::Node() const {
    const XNetAddressInternal& tInternal = *reinterpret_cast<const XNetAddressInternal*>(m_tOpaque);

    return ntohl(tInternal.m_sockAddr.sin_addr.s_addr);
}

XNetPort XNetAddress::Port() const {
    const XNetAddressInternal& tInternal = *reinterpret_cast<const XNetAddressInternal*>(m_tOpaque);

    return ntohs(tInternal.m_sockAddr.sin_port);
}

void XNetAddress::SetNodeAndPort(XNetNode tNode, XNetPort tPort) {
    XNetAddressInternal& tInternal = *reinterpret_cast<XNetAddressInternal*>(m_tOpaque);

    tInternal.m_sockAddr.sin_port = htons(tPort);

    tInternal.m_sockAddr.sin_addr.s_addr = htonl(tNode);
}

void XNetAddress::SetBroadcast(XNetPort tPort) {
    SetNodeAndPort(INADDR_BROADCAST, tPort);
}

void XNetAddress::SetLoopback(XNetPort tPort) {
    SetNodeAndPort(INADDR_LOOPBACK, tPort);
}

void XNetAddress::SetAny(XNetPort tPort) {
    SetNodeAndPort(INADDR_ANY, tPort);
}

BOOL XNetAddress::IsAny() const {
    return Node() == INADDR_ANY;
}
