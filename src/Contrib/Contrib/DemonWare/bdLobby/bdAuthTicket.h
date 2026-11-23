// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAuthTicket.h

#pragma once

#include "bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include <stddef.h>
#include <stdint.h>

class bdAuthTicket {  // size 0x88
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdAuthTicket();
    bdAuthTicket(bdUByte8, bdTitleID, bdUInt32, bdUInt32, bdUInt64, bdOnlineUserID, bdNChar8 *, bdUByte8 *);
    ~bdAuthTicket();
    void serialize(void *);
    void deserialize(const void * buffer);
    bdUInt32 m_magicNumber;  // 0x00
    bdUByte8 m_type;  // 0x04
    bdTitleID m_titleID;  // 0x08
    bdUInt32 m_timeIssued;  // 0x0c
    bdUInt32 m_timeExpires;  // 0x10
    bdUInt64 m_licenseID;  // 0x18
    bdOnlineUserID m_userID;  // 0x20
    bdNChar8 m_username[64];  // 0x28
    bdUByte8 m_sessionKey[24];  // 0x68
private:
    bdUByte8 m_usingHashMagicNumber[3];  // 0x80
    bdUByte8 m_hash[4];  // 0x83
};
static_assert(sizeof(bdAuthTicket) == 0x88, "bdAuthTicket");

class bdAuthInfo {  // size 0xb0
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    // Line 107; the field order of the zeroing is a guess (clang merges it into one memset).
    bdAuthInfo()
        : m_titleID(0), m_IVSeed(0), m_userID(0), m_authSessionKeyKDF(false)
    {
        bdMemset(m_data, 0, sizeof(m_data));
        bdMemset(m_sessionKey, 0, sizeof(m_sessionKey));
    }
    ~bdAuthInfo() {}
    bdTitleID m_titleID;  // 0x00
    bdUInt32 m_IVSeed;  // 0x04
    bdByte8 m_data[128];  // 0x08
    bdUByte8 m_sessionKey[24];  // 0x88
    bdOnlineUserID m_userID;  // 0xa0
private:
    // Inferred: bdLobbyConnection::prepare2x0ClientAuthAndDeriveCrypto reads this private member
    // directly (no inlined accessor in its DWARF), so the class must befriend it.
    friend class bdLobbyConnection;
    // Inferred: bdAuth::handleAuthReply (bdAuth.cpp:432) writes it directly as well.
    friend class bdAuth;
    bdBool m_authSessionKeyKDF;  // 0xa8
};
static_assert(sizeof(bdAuthInfo) == 0xb0, "bdAuthInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line   107  bdAuthInfo::bdAuthInfo()  [inlined into 3 functions]
