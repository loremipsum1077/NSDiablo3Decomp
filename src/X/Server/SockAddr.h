// Original file: X/Server/SockAddr.h
// Layout and member list from the DWARF. Bodies of the constructor, destructor and Clear are
// written from the copies in X/Server/Network/XServerSocket.cpp (static initializer and the
// out-of-line ~SockAddr); the other members are declared only.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/Memory/SigmaMemory.h"

struct SockAddr {  // size 0x18
    SockAddr() { ZeroMemory(this, sizeof(*this)); }  // line 17
    ~SockAddr() { Clear(); }  // line 18
    union {  // size 0x10
        struct {  // size 0x10
            uint8 data[16];  // 0x00
        } ipv6;  // 0x00
        uint32 ipv4;  // 0x00
        char * name;  // 0x00
    };  // 0x00
    enum SockFamily {
        SOCKADDR_NONE = 0,
        SOCKADDR_IPV4 = 1,
        SOCKADDR_IPV6 = 2,
        SOCKADDR_NAMED = 3,
    };
    SockAddr::SockFamily family;  // 0x10
    void Clear() {  // line 34
        if (family == SOCKADDR_NAMED && name != NULL) {
            SigmaMemoryFree(name, NULL);
            name = NULL;
        }
        ZeroMemory(this, sizeof(*this));
    }
    void Normalize();
    SockAddr GetV6Mapped() const;
    bool IsValid() const;
    int compare(const SockAddr &) const;
    bool operator==(const SockAddr &) const;
    bool operator!=(const SockAddr &) const;
    bool operator<(const SockAddr &) const;
    bool FromString(const char *);
    uint32 GetV4();
    uint32 GetV4() const;
    bool IsV4() const;
    bool IsV6() const;
    bool IsNamed() const;
    void SetV4(uint32);
    void SetV6(uint8 *);
    void SetNamed(const char *);
};
static_assert(sizeof(SockAddr) == 0x18, "SockAddr");

static const SockAddr SockNone;  // line 66
