// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAuth.h

#pragma once

#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdCore/bdSocket/bdPort.h"
#include "bdAntiCheat/bdExtendedAuthInfo.h"
#include "bdAuthTicket.h"
#include "bdLobbyCommon/bdAuthCommon.h"
#include "bdLobbyCommon/bdEnvironmentString.h"
#include "bdLobbyCommon/bdHTTPAuthCommon.h"
#include "bdLobbyCommon/bdLobbyServicesCommon.h"
#include "bdLobbyErrorCodes.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAuthUserInfo;
class bdJSONDeserializer;
class bdJSONSerializer;

class bdAuth {  // size 0x31a0
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdAuth);
    enum bdStatus {
        BD_READY = 0,
        BD_CONNECTING = 1,
    };
    bdAuth(const bdTitleID, const bdNChar8 *, const bdPort);
    bdAuth(const bdTitleID, const bdEnvironmentString &, const bdPort);
    virtual ~bdAuth();  // vtable slot 0
    virtual bdBool authorize(const bdAuthUserInfo &);  // vtable slot 2
    virtual bdBool tryCreateAccount(const bdAuthUserInfo &);  // vtable slot 3
    void abortOperation();
    virtual bdBool getEnvironment(bdEnvironment &) const = 0;  // vtable slot 4
    virtual bdAuth::bdStatus getStatus();  // vtable slot 5
    bdLobbyErrorCode getErrorCode() const;
    const bdAuthInfo * getAuthInfo() const;
    const bdAuthTicket * getAuthTicket() const;
    virtual bdBool isLoginQueueResponse() const;  // vtable slot 6
    const bdExtendedAuthInfo * getExtendedAuthInfo() const;
protected:
    virtual bdLobbyErrorCode handleReply() = 0;  // vtable slot 7
    bdLobbyErrorCode handleAuthReply(bdUInt expectedTaskID, bdUInt32 expectedTicketSize);
    virtual bdBool isValidResponse(bdUInt64 expectedTaskID, bdUInt64 responseTaskID) const;  // vtable slot 8
private:
    bdBool validateResponseSignature();
protected:
    virtual bdBool processPlatformClientTicket(bdUInt32 ivSeed, bdByte8 * clientTicketBuffer, bdUInt32 bufferSize);  // vtable slot 9
    virtual bdBool processPlatformData(bdJSONDeserializer & responseJSON);  // vtable slot 10
    bdBool makeAuth();
    virtual bdBool createPlatformDataJson(bdJSONSerializer & platformDataJson);  // vtable slot 11
    virtual bdUInt32 createPlatformExtraHeaders(bdHTTPCurl::bdHTTPHeader *const extraHeaders, const bdUInt32 maxNumExtraHeaders);  // vtable slot 12
    bdBool createRequestBuffer(bdUInt32 size);
    void releaseRequestBuffer();
    bdOnlineUserID m_userID;  // 0x08
    bdTitleID m_titleID;  // 0x10
    bdAuthMsgTypes m_task;  // 0x14
    bdAuthMsgTypes m_responseTask;  // 0x18
    bdUInt32 m_titleVersion;  // 0x1c
    bdAuth::bdStatus m_status;  // 0x20
    bdAuthInfo m_authInfo;  // 0x28
    bdAuthTicket m_authTicket;  // 0xd8
    bdAuthHTTP_t * m_httpInterface;  // 0x160
    bdNChar8 * m_httpAuthRequest;  // 0x168
    bdUInt32 m_httpAuthRequestLength;  // 0x170
    bdNChar8 m_httpAuthReply[5120];  // 0x174
    bdLobbyErrorCode m_errorCode;  // 0x1574
    bdEnvironmentString m_authAddresses;  // 0x1578
    bdPort m_authPort;  // 0x2178
    bdUInt32 m_ivSeed;  // 0x217c
    bdExtendedAuthInfo m_extendedAuthInfo;  // 0x2180
};
static_assert(sizeof(bdAuth) == 0x31a0, "bdAuth");


// Functions defined in this file (bodies are not in the DWARF):
//   line    63  void bdAuth::operator delete(void * p)  [inlined into 1 functions]
//   line    63  void * bdAuth::operator new(size_t nbytes)  [inline]
