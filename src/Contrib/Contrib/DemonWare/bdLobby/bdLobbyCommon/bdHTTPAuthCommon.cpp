// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdHTTPAuthCommon.cpp
#include "bdHTTPAuthCommon.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"











bdLobbyErrorCode bdAuthHTTPUtility::mapHTTPErrorCode(const bdAuthHTTP_t & http)
{
    const bdUInt32 httpStatus = http.getLastHTTPStatus();
    const bdUInt32 internalError = http.getInternalError();

    switch (httpStatus)
    {
    case 200:
        return BD_AUTH_NO_ERROR;
    case 400:
    case 403:
    case 404:
        return BD_AUTH_BAD_REQUEST;
    case 0:
        return mapBdHTTPInternalErrorCode(BD_AUTH_UNKNOWN_ERROR, internalError);
    case 503:
        return mapBdHTTPInternalErrorCode(BD_AUTH_SERVER_UNAVAILABLE_ERROR, internalError);
    default:
        break;
    }




    bdLogWarn("auth", "Unexpected HTTP status %u", httpStatus);
    return BD_AUTH_UNKNOWN_ERROR;
}



inline bdLobbyErrorCode bdAuthHTTPUtility::mapBdHTTPInternalErrorCode(const bdLobbyErrorCode defaultError,
                                                               const bdUInt32 internalErrorCode)
{

    switch (internalErrorCode)
    {
    case bdHTTPCurl::BD_HTTP_NO_ERROR:
        return defaultError;
    case bdHTTPCurl::BD_HTTP_SSL_CERTIFICATE_ERROR:
        return BD_AUTH_SSL_CERTIFICATE_ERROR;
    case bdHTTPCurl::BD_HTTP_DNS_LOOKUP_FAILED_ERROR:
        return BD_AUTH_DNS_RESOLUTION_ERROR;
    case bdHTTPCurl::BD_HTTP_THIRD_PARTY_TOKEN_RETRIEVAL_ERROR:
        return BD_AUTH_PLATFORM_TOKEN_RETRIEVAL_ERROR;
    case bdHTTPCurl::BD_HTTP_CONNECT_ERROR:
        return BD_AUTH_SERVER_UNAVAILABLE_ERROR;
    case bdHTTPCurl::BD_HTTP_ABORTED:
        return BD_AUTH_REQUEST_ABORTED_ERROR;
    case bdHTTPCurl::BD_HTTP_TIMEOUT:
        return BD_AUTH_REQUEST_TIMEOUT_ERROR;
    default:
        break;
    }





    bdLogWarn("auth", "Unknown internal HTTP error %u", internalErrorCode);
    return defaultError;
}
















const bdNChar8 * bdAuthHTTPUtility::getStatusCodeString(const bdHTTP::bdStatus status)
{

    switch (status)
    {
    case bdHTTP::BD_UNINITIALIZED: return "BD_UNINITIALIZED";
    case bdHTTP::BD_EMPTY: return "BD_EMPTY";
    case bdHTTP::BD_INITIALIZING: return "BD_INITIALIZING";
    case bdHTTP::BD_PENDING: return "BD_PENDING";
    case bdHTTP::BD_ABORTING: return "BD_ABORTING";
    case bdHTTP::BD_DONE: return "BD_DONE";
    case bdHTTP::BD_FAILED: return "BD_FAILED";
    case bdHTTP::BD_ABORTED: return "BD_ABORTED";
    default: break;
    }

    return "Unknown";
}

const bdNChar8 * bdAuthHTTPUtility::getInternalErrorCodeString(const bdInt32 error)
{

    switch (error)
    {
    case bdHTTPCurl::BD_HTTP_NO_ERROR: return "BD_HTTP_NO_ERROR";
    case bdHTTPCurl::BD_HTTP_SSL_CERTIFICATE_ERROR: return "BD_HTTP_SSL_CERTIFICATE_ERROR";
    case bdHTTPCurl::BD_HTTP_DNS_LOOKUP_FAILED_ERROR: return "BD_HTTP_DNS_LOOKUP_FAILED_ERROR";
    case bdHTTPCurl::BD_HTTP_THIRD_PARTY_TOKEN_RETRIEVAL_ERROR: return "BD_HTTP_THIRD_PARTY_TOKEN_RETRIEVAL_ERROR";
    case bdHTTPCurl::BD_HTTP_CONNECT_ERROR: return "BD_HTTP_CONNECT_ERROR";
    case bdHTTPCurl::BD_HTTP_ABORTED: return "BD_HTTP_ABORTED";
    case bdHTTPCurl::BD_HTTP_TIMEOUT: return "BD_HTTP_TIMEOUT";
    default: break;
    }
    return "Unknown";
}
