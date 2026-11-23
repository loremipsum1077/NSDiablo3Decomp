// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAuth/bdAuthSwitch.cpp
#include "bdAuthSwitch.h"
#include "../../bdJSON/bdJSONSerializer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "../../bdUtility/bdJOSE/bdJWS.h"
#include "../../../../../NintendoSDK/Include/nn/nifm/nifm_Api.h"
#include "../../bdSocket/bdSocketRouterAdHoc-switch.h"

// Log format strings are compiled out; the texts below are placeholders, only the argument lists
// are evidenced. BD_NSO_NO_SUBSCRIPTION (line 25) and BD_DEFAULT_ISS_ENV_JSON (line 32) are unused
// in the surviving code and their values are not in the image.
static const bdUInt16 BD_NSO_SUBSCRIPTION_NOT_SET = 0;

bdAuthSwitch::bdAuthSwitch(const bdTitleID titleID, const bdEnvironmentString & authAddresses, const bdEnvironment env,
                           const bdPort authPort, const bdNChar8 * issJSON)
    : bdAuth(titleID, authAddresses, authPort)
    , m_nsoSubscriptionStatus(BD_NSO_SUBSCRIPTION_NOT_SET)
    , m_environment(env)
{
    bdMemset(m_tokenBuffer, 0, sizeof(m_tokenBuffer));
    setIssEnvironmentMap(issJSON);
}

bdAuthSwitch::~bdAuthSwitch() {}

bdUInt16 bdAuthSwitch::getNSOSubscriptionStatus()
{
    return m_nsoSubscriptionStatus;
}

bdBool bdAuthSwitch::authorizeSwitch(const nn::account::UserHandle & userHandle, const bdNChar8 * username,
                                     const bdUInt32 version)
{
    if (m_status != BD_READY)
    {
        bdLogWarn("auth", "Not ready");
        return false;
    }
    bdLogInfo("auth", "Authorizing");
    m_subtask = GET_SWITCH_TOKEN;
    m_errorCode = BD_AUTH_NO_ERROR;
    m_nsoSubscriptionStatus = BD_NSO_SUBSCRIPTION_NOT_SET;

    m_userHandle = userHandle;
    m_username = username;
    m_titleVersion = version;
    m_task = BD_AUTH_SWITCH_FOR_MMP_REQUEST;

    if (checkUserIsValid() && networkIsAvailable())
    {
        const bdAuthSwitchTokenCacheFetchStatus cacheFetchResult = startFetchingTokenCache();
        if (cacheFetchResult == BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_IN_PROGRESS)
        {
            m_status = BD_CONNECTING;
            return true;
        }
    }
    return false;
}

bdBool bdAuthSwitch::checkUserIsValid()
{
    nn::Result result = nn::account::EnsureNetworkServiceAccountAvailable(m_userHandle);
    if (result.IsFailure())
    {
        bdLogWarn("auth", "EnsureNetworkServiceAccountAvailable failed: %d", result.GetDescription());
        return false;
    }
    bdLogInfo("auth", "Network service account available");
    bdBool hasNSA;
    result = nn::account::IsNetworkServiceAccountAvailable(&hasNSA, m_userHandle);
    if (result.IsFailure())
    {
        bdLogWarn("auth", "IsNetworkServiceAccountAvailable failed: %d", result.GetDescription());
        return false;
    }
    if (!hasNSA)
    {
        bdLogWarn("auth", "No network service account");
        return false;
    }
    bdLogInfo("auth", "Has network service account");
    nn::account::NetworkServiceAccountId nsaid;
    result = nn::account::GetNetworkServiceAccountId(&nsaid, m_userHandle);
    if (result.IsFailure())
    {
        bdLogWarn("auth", "GetNetworkServiceAccountId failed: %d", result.GetDescription());
        return false;
    }
    bdLogInfo("auth", "NSA id %llu", nsaid.id);
    return true;
}

bdBool bdAuthSwitch::networkIsAvailable()
{
    nn::nifm::SubmitNetworkRequestAndWait();
    if (!nn::nifm::IsNetworkAvailable())
    {
        bdLogWarn("auth", "Network is not available");
        return false;
    }
    return true;
}

bdAuthSwitch::bdAuthSwitchTokenCacheFetchStatus bdAuthSwitch::startFetchingTokenCache()
{
    nn::Result result = nn::account::EnsureNetworkServiceAccountIdTokenCacheAsync(&m_asyncContext, m_userHandle);
    if (result.IsFailure())
    {
        bdLogWarn("auth", "EnsureNetworkServiceAccountIdTokenCacheAsync failed: %d", result.GetDescription());
        return BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_FAILED;
    }
    return BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_IN_PROGRESS;
}

bdAuthSwitch::bdAuthSwitchTokenCacheFetchStatus bdAuthSwitch::pumpFetchTokenCache()
{
    bdBool asyncRequestDone = false;
    nn::Result result = m_asyncContext.HasDone(&asyncRequestDone);
    if (result.IsFailure())
    {
        bdLogWarn("auth", "HasDone failed: %d", result.GetDescription());
        return BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_FAILED;
    }

    if (asyncRequestDone)
    {
        nn::Result tokenContextResult = m_asyncContext.GetResult();
        if (tokenContextResult.IsFailure())
        {
            nn::err::ShowError(tokenContextResult);
            bdLogWarn("auth", "Token cache request failed: %d", tokenContextResult.GetDescription());
            return BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_FAILED;
        }

        nn::Result result = nn::account::LoadNetworkServiceAccountIdTokenCache(&m_actualTokenBufferSize,
                                                                               m_tokenBuffer,
                                                                               sizeof(m_tokenBuffer),
                                                                               m_userHandle);
        if (result.IsFailure())
        {
            nn::err::ShowError(result);
            bdLogWarn("auth", "LoadNetworkServiceAccountIdTokenCache failed: %d", result.GetDescription());
            return BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_FAILED;
        }

        bdLogInfo("auth", "Token (%zu): %s", m_actualTokenBufferSize, m_tokenBuffer);
        return BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_DONE;
    }
    return BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_IN_PROGRESS;
}

bdAuth::bdStatus bdAuthSwitch::getStatus()
{
    if (m_status == BD_CONNECTING)
    {
        if (m_subtask != GET_SWITCH_TOKEN)
        {
            return bdAuth::getStatus();
        }
        const bdAuthSwitchTokenCacheFetchStatus cacheFetchResult = pumpFetchTokenCache();
        switch (cacheFetchResult)
        {
        case BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_DONE:
            bdLogInfo("auth", "Token fetched");
            if (makeAuth())
            {
                nn::Result result = m_asyncContext.GetResult();
                if (result.IsFailure())
                {
                    bdLogWarn("auth", "GetResult failed: %d", result.GetDescription());
                    m_errorCode = BD_AUTH_PLATFORM_TOKEN_RETRIEVAL_ERROR;
                    m_status = BD_READY;
                }
                else
                {
                    m_subtask = GET_DW_AUTH_TICKET;
                }
            }
            else
            {
                m_errorCode = BD_AUTH_PLATFORM_TOKEN_RETRIEVAL_ERROR;
                m_status = BD_READY;
            }
            break;
        case BD_AUTH_SWITCH_TOKEN_CACHE_FETCH_FAILED:
            bdLogWarn("auth", "Token fetch failed");
            m_errorCode = BD_AUTH_PLATFORM_TOKEN_RETRIEVAL_ERROR;
            m_status = BD_READY;
            break;
        default:
            bdLogInfo("auth", "Token fetch in progress");
            break;
        }
    }
    return m_status;
}

bdLobbyErrorCode bdAuthSwitch::handleReply()
{
    if (m_task == BD_AUTH_SWITCH_FOR_MMP_REQUEST)
    {
        if (m_subtask == GET_DW_AUTH_TICKET)
        {
            return handleAuthReply(BD_AUTH_SWITCH_FOR_MMP_REPLY, 128);
        }
        bdLogWarn("auth", "Unexpected subtask %u", m_subtask);
    }
    else
    {
        bdLogWarn("auth", "Unexpected task %u", m_task);
    }
    return BD_AUTH_UNKNOWN_ERROR;
}

bdBool bdAuthSwitch::getEnvironment(bdEnvironment & environment) const
{
    static const bdJOSE::bdJWSSigAlgorithm allowedAlgorithms[1] = {bdJOSE::RS256};
    if (bdStrlen(m_tokenBuffer) == 0)
    {
        bdLogInfo("auth", "No token, using configured environment");
        environment = m_environment;
        return true;
    }

    bdUInt tokenSize = 2048;
    bdUByte8 validationToken[2049];
    bdUByte8 key[5] = "none";
    const bdUInt32 tokenStrLen = static_cast<bdUInt32>(bdStrlen(m_tokenBuffer));
    bdJWS::verify(m_tokenBuffer, tokenStrLen, validationToken, tokenSize, allowedAlgorithms, 1, key, sizeof(key));

    const bdNChar8 * jwtData = reinterpret_cast<const bdNChar8 *>(validationToken);
    bdJSONDeserializer jwtJSON;
    bdBool ok = jwtJSON.parse(jwtData);
    if (!ok)
    {
        bdLogWarn("auth", "Failed to get environment from token");
        return false;
    }

    bdNChar8 issData[80];
    ok = jwtJSON.getString("iss", issData, sizeof(issData));
    if (!ok)
    {
        bdLogWarn("auth", "Failed to get environment from token");
        return false;
    }
    return getEnvironmentFromISS(environment, issData);
}

bdBool bdAuthSwitch::getEnvironmentFromISS(bdEnvironment & environment, const bdNChar8 * iss) const
{
    bdInt32 issEnv;
    if (!m_issMap.getInt32(iss, issEnv))
    {
        bdLogWarn("auth", "Failed to get environment from token");
        return false;
    }
    environment = static_cast<bdEnvironment>(issEnv);
    bdLogInfo("auth", "Environment %d from iss %s", environment, iss);
    return true;
}

bdBool bdAuthSwitch::createPlatformDataJson(bdJSONSerializer & platformDataJson)
{
    bdBool ok = bdAuth::createPlatformDataJson(platformDataJson);

    ok = ok && platformDataJson.writeString("token", m_tokenBuffer);
    ok = ok && platformDataJson.writeString("username", m_username);
    ok = ok && platformDataJson.writeBoolean("extended_data", true);

    return ok;
}

bdBool bdAuthSwitch::processPlatformData(bdJSONDeserializer & responseJSON)
{
    bdJSONDeserializer extraData;
    bdNChar8 extraDataStr[5120];
    bdMemset(extraDataStr, 0, sizeof(extraDataStr));
    bdNChar8 extendedData[4097];
    bdMemset(extendedData, 0, sizeof(extendedData));

    bdBool ok = responseJSON.hasKey("extra_data");
    ok = ok && responseJSON.getString("extra_data", extraDataStr, sizeof(extraDataStr));
    ok = ok && extraData.parse(extraDataStr);
    if (!ok)
    {
        bdLogWarn("auth", "No extra data");
    }
    else
    {
        ok = extraData.hasKey("extended_data");
        ok = ok && extraData.getString("extended_data", extendedData, sizeof(extendedData));
    }
    if (ok)
    {
        bdLogInfo("auth", "Extended data length %zu", bdStrlen(extendedData));
        ok = m_extendedAuthInfo.setData(extendedData);
    }
    else
    {
        bdLogWarn("auth", "No extended data");
    }

    if (extraData.hasKey("nso_subscription_status"))
    {
        ok = extraData.getUInt16("nso_subscription_status", m_nsoSubscriptionStatus);
        if (ok)
        {
            bdLogInfo("auth", "NSO subscription status %u", m_nsoSubscriptionStatus);
        }
        else
        {
            bdLogWarn("auth", "Failed to read NSO subscription status");
        }
    }
    return ok;
}

bdBool bdAuthSwitch::setIssEnvironmentMap(const bdNChar8 * issJSON)
{
    bdBool ok = m_issMap.parse(issJSON);
    ok = ok && m_issMap.isObject();
    return ok;
}

bdBool bdAuthSwitch::isLoginQueueResponse() const
{
    return m_responseTask == BD_AUTH_SWITCH_FOR_LOGINQUEUE_REPLY;
}
