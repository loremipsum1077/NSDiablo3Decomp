// Adopted from the DWARF; hand-maintained declarations.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformSocket/bdSockAddr.h

#pragma once

#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdSockAddr {  // size 0x84
public:
    bdSockAddr();
    bdSockAddr(bdUInt32);
    bdSockAddr(const bdNChar8 *);
    void fromString(const bdNChar8 * addr);
    bdUWord toString(bdNChar8 *const str, const bdUWord size) const;
    void fromString(bdUInt16, const bdNChar8 *);
    bdUWord toString(bdUInt16, bdNChar8 *const, const bdUWord) const;
    union {  // size 0x80
        struct {  // size 0x4
            bdUByte8 m_b1;  // 0x00
            bdUByte8 m_b2;  // 0x01
            bdUByte8 m_b3;  // 0x02
            bdUByte8 m_b4;  // 0x03
        } m_caddr;  // 0x00
        bdUInt32 m_iaddr;  // 0x00
        struct {  // size 0x10
            bdUInt16 m_w1;  // 0x00
            bdUInt16 m_w2;  // 0x02
            bdUInt16 m_w3;  // 0x04
            bdUInt16 m_w4;  // 0x06
            bdUInt16 m_w5;  // 0x08
            bdUInt16 m_w6;  // 0x0a
            bdUInt16 m_w7;  // 0x0c
            bdUInt16 m_w8;  // 0x0e
        } m_caddr6;  // 0x00
        bdUByte8 m_iaddr6[16];  // 0x00
        bdUByte8 m_sockaddr_storage[128];  // 0x00
    } inUn;  // 0x00
    bdUInt16 m_family;  // 0x80
};
static_assert(sizeof(bdSockAddr) == 0x84, "bdSockAddr");

typedef bdSockAddr bdInAddr;

