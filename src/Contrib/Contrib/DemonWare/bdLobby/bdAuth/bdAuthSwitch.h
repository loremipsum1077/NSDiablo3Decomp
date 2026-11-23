// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAuth/bdAuthSwitch.h

#pragma once

#include "../../bdCore/bdSocket/bdPort.h"
#include "../../bdJSON/bdJSONDeserializer.h"
#include "../bdAuth.h"
#include "../bdLobbyCommon/bdEnvironmentString.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyErrorCodes.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>
#include "../../../../../NintendoSDK/Include/nn/account/account_ApiNetworkServiceAccount.h"

class bdJSONSerializer;

class bdAuthSwitch : public bdAuth {  // size 0x3a20
public:
    bdAuthSwitch(const bdTitleID, const bdNChar8 *, const bdPort);
    bdAuthSwitch(const bdTitleID, const bdEnvironmentString &, const bdEnvironment, const bdPort, const bdNChar8 *);
    virtual ~bdAuthSwitch();  // vtable slot 0
    bdBool authorizeSwitch(const nn::account::UserHandle & userHandle, const bdNChar8 * username, const bdUInt32 version);
    virtual bdBool getEnvironment(bdEnvironment & environment) const;  // vtable slot 4
    virtual bdAuth::bdStatus getStatus();  // vtable slot 5
    virtual bdBool isLoginQueueResponse() const;  // vtable slot 6
    bdUInt16 getNSOSubscriptionStatus();
    enum bdAuthSwitchTokenCacheFetchStatus {
        BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_FAILED = 0,
        BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_DONE = 1,
        BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_IN_PROGRESS = 2,
    };
    enum bdAuthSwitchSubTask {
        GET_SWITCH_TOKEN = 0,
        GET_DW_AUTH_TICKET = 1,
    };
private:
    bdBool checkUserIsValid();
    bdBool networkIsAvailable();
    bdAuthSwitch::bdAuthSwitchTokenCacheFetchStatus startFetchingTokenCache();
    bdAuthSwitch::bdAuthSwitchTokenCacheFetchStatus pumpFetchTokenCache();
    virtual bdLobbyErrorCode handleReply();  // vtable slot 7
    virtual bdBool createPlatformDataJson(bdJSONSerializer & platformDataJson);  // vtable slot 11
    virtual bdBool processPlatformData(bdJSONDeserializer & responseJSON);  // vtable slot 10
    bdBool setIssEnvironmentMap(const bdNChar8 * issJSON);
    bdBool getEnvironmentFromISS(bdEnvironment & environment, const bdNChar8 * iss) const;
    bdAuthSwitch::bdAuthSwitchSubTask m_subtask;  // 0x31a0
    nn::account::UserHandle m_userHandle;  // 0x31a8
    const bdNChar8 * m_username;  // 0x31c0
    bdUInt16 m_nsoSubscriptionStatus;  // 0x31c8
    nn::account::AsyncContext m_asyncContext;  // 0x31d0
    size_t m_actualTokenBufferSize;  // 0x31f0
    bdNChar8 m_tokenBuffer[2048];  // 0x31f8
    bdEnvironment m_environment;  // 0x39f8
    bdJSONDeserializer m_issMap;  // 0x3a00
};
static_assert(sizeof(bdAuthSwitch) == 0x3a20, "bdAuthSwitch");


// Variables defined in this file:
//   line    22  static const bdUInt16 BD_NSO_SUBSCRIPTION_NOT_SET
//   line    25  static const bdUInt16 BD_NSO_NO_SUBSCRIPTION
//   line    32  static const bdNChar8 BD_DEFAULT_ISS_ENV_JSON[406]

// DWARF bdAuthSwitch.h:32; original _ZL23BD_DEFAULT_ISS_ENV_JSON data, including final NUL.
#line 32
static const bdNChar8 BD_DEFAULT_ISS_ENV_JSON[406] =
#include "../../../../../X/Common/generated-game-data/issuer_mapping.inc"
;
