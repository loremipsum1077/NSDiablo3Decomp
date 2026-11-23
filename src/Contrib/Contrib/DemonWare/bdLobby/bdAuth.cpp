// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAuth.cpp
#include "bdAuth.h"
#include "../bdJSON/bdJSONSerializer.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdSocket/bdSocketRouterAdHoc-switch.h"
#include "../bdCore/bdContainers/bdSingleton.h"
#include "../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../bdCore/bdUtilities/bdBase64.h"
#include "../bdCore/bdUtilities/bdBytePacker.h"
#include "../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../bdJSON/bdJSONDeserializer.h"
#include "../bdPlatform/bdPlatformString/bdPlatformString.inl"

// Defined in Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdAuthCommon.cpp:22 (not in src yet);
// referenced through the GOT by handleAuthReply. Declaration placement inferred.
extern const bdUInt32 BD_MAGIC_NUMBER;
// Defined in Contrib/Contrib/DemonWare/bdLobby/bdLobbyService.cpp:4035; declaration placement inferred.
const bdNChar8 * bdLobbyErrorCodeToString(const bdLobbyErrorCode code);






















bdAuth::bdAuth(const bdTitleID titleID, const bdEnvironmentString & authAddresses, const bdPort authPort)
    :

      m_titleID(titleID),
      m_httpAuthRequest(NULL),
      m_httpAuthRequestLength(0),
      m_errorCode(BD_NO_ERROR),
      m_authAddresses(authAddresses),
      m_authPort(authPort),
      m_ivSeed(0)
{



    m_httpInterface = new bdAuthHTTP_t();

    m_status = BD_READY;
    m_task = m_responseTask = BD_AUTH_INVALID_REQUEST;

}

bdAuth::~bdAuth()
{
    releaseRequestBuffer();
    if (m_httpInterface) delete m_httpInterface;
}

bdBool bdAuth::authorize(const bdAuthUserInfo &)
{
    return false;
}





bdBool bdAuth::tryCreateAccount(const bdAuthUserInfo &)
{
    bdLogWarn("auth", "Account creation is not supported");
    return false;
}








bdLobbyErrorCode bdAuth::getErrorCode() const
{
    return m_errorCode;
}

const bdAuthInfo * bdAuth::getAuthInfo() const
{
    return &m_authInfo;
}

const bdAuthTicket * bdAuth::getAuthTicket() const
{
    return &m_authTicket;
}

bdBool bdAuth::isLoginQueueResponse() const
{
    return false;
}






bdAuth::bdStatus bdAuth::getStatus()
{
    if (m_status == BD_CONNECTING)
    {
        const bdHTTP::bdStatus httpStatus = m_httpInterface->getStatus();
        if (httpStatus != bdHTTP::BD_EMPTY && httpStatus != bdHTTP::BD_PENDING)
        {




            if (httpStatus == bdHTTP::BD_DONE)
            {
                m_errorCode = handleReply();
                if (m_errorCode != BD_AUTH_CONTINUING)
                {
                    m_status = BD_READY;
                    releaseRequestBuffer();
                }
            }
            else
            {
                const bdInt32 internalError = m_httpInterface->getInternalError();
                bdLogWarn("auth", "HTTP request failed: %s, HTTP status %u, internal error %d (%s)", bdAuthHTTPUtility::getStatusCodeString(httpStatus), m_httpInterface->getLastHTTPStatus(), internalError, bdAuthHTTPUtility::getInternalErrorCodeString(internalError));



                m_errorCode = bdAuthHTTPUtility::mapHTTPErrorCode(*m_httpInterface);
                m_status = BD_READY;
                releaseRequestBuffer();
            }
        }
    }
    return m_status;
}

bdBool bdAuth::makeAuth()
{
	bdBool ok = true;

	bdEnvironment environment;
	if (!getEnvironment(environment))
	{
		bdLogError("auth", "Could not get the auth environment");
		m_errorCode = BD_AUTH_ENVIRONMENT_ERROR;
		ok = false;
	}

	bdNChar8 serializedJSON[5120] = {0};
	if (ok)
	{
		m_ivSeed = bdCryptoUtils::getNewIVSeed();
		bdMemset(m_httpAuthReply, 0, sizeof(m_httpAuthReply));


		bdJSONSerializer requestJSON(serializedJSON, sizeof(serializedJSON));
		const bdBool setAsString = true;
		ok = ok && requestJSON.writeBeginObject();
		ok = ok && requestJSON.writeUInt64("auth_task", m_task, setAsString);
		ok = ok && requestJSON.writeUInt64("iv_seed", m_ivSeed, setAsString);
		ok = ok && requestJSON.writeUInt64("title_id", m_titleID, setAsString);

		ok = ok && requestJSON.writeString("identity", "356c4bc3");



		bdNChar8 serializedPlatformDataJson[5120] = {0};
		{
			bdJSONSerializer platformDataJson(serializedPlatformDataJson, sizeof(serializedPlatformDataJson));
			ok = ok && platformDataJson.writeBeginObject();
			ok = ok && createPlatformDataJson(platformDataJson);
			ok = ok && platformDataJson.writeEndObject();
			ok = ok && platformDataJson.validate();
		}


		ok = ok && requestJSON.writeString("extra_data", serializedPlatformDataJson);
		ok = ok && requestJSON.writeEndObject();
		ok = ok && requestJSON.validate();
		if (!ok)
		{
			m_errorCode = BD_AUTH_JSON_FORMAT_ERROR;
		}
	}

	if (ok)
	{

		createRequestBuffer(static_cast<bdUInt32>(bdStrlen(serializedJSON)));
		bdMemcpy(m_httpAuthRequest, serializedJSON, m_httpAuthRequestLength);


		bdNChar8 urlString[512] = {0};









		bdSnprintf(urlString, sizeof(urlString), "https://%s:%d/auth/",
							m_authAddresses.get(environment), m_authPort);

		bdLogInfo("auth", "Auth URL: %s", urlString);


		bdUInt64 authTransactionId = bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUInt64();


		const bdUInt32 maxNumExtraHeaders = 1;
		bdHTTPCurl::bdHTTPHeader extraHeaders[maxNumExtraHeaders];
		bdUInt32 numExtraHeaders = createPlatformExtraHeaders(extraHeaders, maxNumExtraHeaders);




		bdNChar8 authTransactionIdString[24];
		bdSnprintf(authTransactionIdString, sizeof(authTransactionIdString), "%llu", authTransactionId);

		ok = ok && m_httpInterface->initRequest(bdHTTP::BD_POST, urlString, 15000);
		ok = ok && m_httpInterface->setHeader("Content-Type", "application/json");
		ok = ok && m_httpInterface->setHeader("X-TransactionID", authTransactionIdString);
		for (bdUInt i = 0; i < numExtraHeaders; ++i)
		{
			ok = ok && m_httpInterface->setHeader(extraHeaders[i].key, extraHeaders[i].value);
		}
		ok = ok && m_httpInterface->setUploadData(m_httpAuthRequest, m_httpAuthRequestLength);
		ok = ok && m_httpInterface->setDownloadBuffer(m_httpAuthReply, sizeof(m_httpAuthReply));
		ok = ok && (m_httpInterface->sendRequest() == bdHTTP::BD_PENDING);

		const bdHTTP::bdStatus httpStatus = m_httpInterface->getStatus();











		if (ok)
		{
			if (httpStatus == bdHTTP::BD_PENDING)
			{
				m_status = BD_CONNECTING;
			}
			else
			{
				bdLogWarn("auth", "Could not start the auth request, HTTP status %d", httpStatus);
				m_errorCode = BD_START_TASK_FAILED;
				ok = false;
			}
		}
	}
	return ok;
}

bdBool bdAuth::createPlatformDataJson(bdJSONSerializer & platformDataJson)
{
    return platformDataJson.writeUInt64("version", m_titleVersion, true);
}

bdUInt32 bdAuth::createPlatformExtraHeaders(bdHTTPCurl::bdHTTPHeader *const extraHeaders, const bdUInt32 maxNumExtraHeaders)
{
    return 0;
}




bdBool bdAuth::createRequestBuffer(bdUInt32 size)
{
	releaseRequestBuffer();
	m_httpAuthRequestLength = size;
	m_httpAuthRequest = static_cast<bdNChar8 *>(bdMemory::allocate(m_httpAuthRequestLength));

	// Inferred: the DWARF has the local `result` (line 295) but makeAuth ignores the return value.
	bdBool result = (m_httpAuthRequest != NULL);
	return result;
}
inline void bdAuth::releaseRequestBuffer()
{
    if (m_httpAuthRequest)
    {
        bdMemory::deallocate(m_httpAuthRequest);
        m_httpAuthRequest = NULL;
        m_httpAuthRequestLength = 0;
    }
}

bdLobbyErrorCode bdAuth::handleAuthReply(bdUInt expectedTaskID,
	bdUInt32 expectedTicketSize)
{
	bdLogInfo("auth", "Handling auth reply");
	bdLobbyErrorCode authTaskResult = BD_NO_ERROR;
	const bdUInt32 httpAuthReplyCode = m_httpInterface->getLastHTTPStatus();

	if (httpAuthReplyCode == 200)
	{

		bdUInt64 ivSeed = 0;
		bdUInt64 taskID = 0;
		bdUInt64 errorCode = 0;
		bdByte8 clientTicketBuffer[204] = {0};


		bdJSONDeserializer responseJSON;
		const bdBool parsingResult = responseJSON.parse(m_httpAuthReply);
		if (!parsingResult || !responseJSON.isObject())
		{
			authTaskResult = BD_AUTH_JSON_FORMAT_ERROR;
		}
		else
		{
			bdBool ok = true;

			ok = ok && responseJSON.getUInt64("auth_task", taskID);
			if (!ok || !isValidResponse(expectedTaskID, taskID))
			{
				bdLogWarn("auth", "Unexpected auth task %llu", taskID);
				authTaskResult = BD_AUTH_REPLY_CONTENT_ERROR;
			}
			else
			{



				m_responseTask = static_cast<bdAuthMsgTypes>(taskID);

				ok = ok && responseJSON.getUInt64("code", errorCode);
				if (ok)
				{
					if (errorCode == BD_AUTH_NO_ERROR)
					{

						ok = ok && validateResponseSignature();








						ok = ok && responseJSON.getUInt64("iv_seed", ivSeed);


						bdNChar8 clientTicketStr[205] = {0};
						ok = ok && responseJSON.getString("client_ticket", clientTicketStr, sizeof(clientTicketStr));
						if (ok)
						{






							const bdUInt decodedClientTicketSize = bdBase64::decode(clientTicketStr,
								bdStrlen(clientTicketStr), clientTicketBuffer, expectedTicketSize);


							if (decodedClientTicketSize != expectedTicketSize)
							{
								bdLogWarn("auth", "Client ticket has unexpected size %u", decodedClientTicketSize);
								ok = false;
							}
						}


						bdNChar8 serverTicketStr[205] = {0};
						ok = ok && responseJSON.getString("server_ticket", serverTicketStr, sizeof(serverTicketStr));
						if (ok)
						{

							const bdUInt decodedServerTicketSize = bdBase64::decode(serverTicketStr,
								bdStrlen(serverTicketStr),
								m_authInfo.m_data, sizeof(m_authInfo.m_data));

							if (decodedServerTicketSize != sizeof(m_authInfo.m_data))
							{
								bdLogWarn("auth", "Server ticket has unexpected size %u", decodedServerTicketSize);
								ok = false;
							}
						}


						ok = ok && processPlatformData(responseJSON);

						bdUInt32 magicNumberCheck = 0;
						bdUInt offset = 0;
						ok = ok && bdBytePacker::removeBasicType(clientTicketBuffer, sizeof(magicNumberCheck), offset, offset, magicNumberCheck);

						if (magicNumberCheck != BD_MAGIC_NUMBER)
						{



							ok = ok && processPlatformClientTicket(static_cast<bdUInt32>(ivSeed), clientTicketBuffer, expectedTicketSize);
						}


						if (ok)
						{

							m_authTicket.deserialize(clientTicketBuffer);

							if (m_authTicket.m_magicNumber == BD_MAGIC_NUMBER)
							{
								bdMemcpy(m_authInfo.m_sessionKey, m_authTicket.m_sessionKey, sizeof(m_authInfo.m_sessionKey));
								m_authInfo.m_titleID = m_authTicket.m_titleID;
								m_authInfo.m_IVSeed = static_cast<bdUInt32>(ivSeed);
								m_authInfo.m_userID = m_authTicket.m_userID;

								m_authInfo.m_authSessionKeyKDF = responseJSON.hasKey("identity");

								bdLogInfo("auth", "Authenticated user %llu", m_authInfo.m_userID);
								authTaskResult = BD_AUTH_NO_ERROR;
							}
							else
							{

								bdLogWarn("auth", "Could not decrypt the auth ticket");
								if (taskID == BD_AUTH_ACCOUNT_FOR_MMP_REPLY || taskID == BD_AUTH_HOST_FOR_MMP_REPLY)
								{
									authTaskResult = BD_AUTH_INCORRECT_PASSWORD;
								}
								else
								{
									authTaskResult = BD_AUTH_TICKET_DECRYPTION_ERROR;
								}
							}
						}
						else
						{
							bdLogWarn("auth", "Invalid auth reply content");
							authTaskResult = BD_AUTH_REPLY_CONTENT_ERROR;
						}
					}
					else
					{
						authTaskResult = static_cast<bdLobbyErrorCode>(errorCode);

						bdLogWarn("auth", "Auth failed with error %llu (%s)", errorCode, bdLobbyErrorCodeToString(authTaskResult));

					}
				}
				else
				{
					bdLogWarn("auth", "Invalid auth reply content");
					authTaskResult = BD_AUTH_REPLY_CONTENT_ERROR;
				}
			}
		}
	}
	else
	{
		bdLogWarn("auth", "Auth request failed with HTTP status %u", httpAuthReplyCode);
		authTaskResult = bdAuthHTTPUtility::mapHTTPErrorCode(*m_httpInterface);
	}

	return authTaskResult;
}

bdBool bdAuth::processPlatformClientTicket(bdUInt32 ivSeed, bdByte8 * clientTicketBuffer, bdUInt32 bufferSize)
{
    return true;
}







bdBool bdAuth::processPlatformData(bdJSONDeserializer & responseJSON)
{
    return true;
}



bdBool bdAuth::isValidResponse(bdUInt64 expectedTaskID, bdUInt64 responseTaskID) const
{
    return expectedTaskID == responseTaskID;
}


bdBool bdAuth::validateResponseSignature()
{



























	// Inferred: only this log call (line 535) survives inlining; the result folds to true.
	bdLogInfo("auth", "Response signature validation is not enabled");
	return true;
}
