// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStoreStreaming.cpp
#include "bdObjectStoreStreaming.h"
#include "bdObjectStoreDownloadInterceptor.h"
#include "bdObjectStoreMetadata.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

// Log format strings are compiled out; the texts are placeholders, only the argument lists are
// evidenced. Only the functions that survive out of line (or inlined into them) are written.

bdObjectStoreStreaming::~bdObjectStoreStreaming()
{
    if (m_http)
    {
        delete m_http;
    }
}

// line 117. Only the transitions used by initHTTP are evidenced.
inline void bdObjectStoreStreaming::setState(bdObjectStoreStreaming::bdStatus newState, bdLobbyErrorCode errorCode)
{
    if (m_overallTask->m_errorCode == BD_NO_ERROR)
    {
        if (errorCode != BD_NO_ERROR)
        {
            m_overallTask->m_errorCode = errorCode;
        }
        else if (m_state == BD_POST_HTTP_OPERATION || m_state == BD_PRE_HTTP_OPERATION)
        {
            m_overallTask->m_errorCode = m_remoteTask->m_errorCode;
        }
    }
    if (newState == BD_FAILED)
    {
        m_overallTask->m_status = bdTask::BD_FAILED;
    }
    m_state = newState;
}

bdHTTP::bdStatus bdObjectStoreStreaming::startContentDownload(const bdNChar8 *const downloadURL,
                                                              bdDownloadInterceptor * downloadHandler,
                                                              bdUInt64 transactionID,
                                                              const bdNChar8 * authorizationHeader)
{
    bdLogInfo("objectstore", "Downloading %s", downloadURL);

    bdBool ok = m_http->initRequest(bdHTTP::BD_GET, downloadURL, 0);
    m_operation = BD_DOWNLOAD_OBJECT;
    if (transactionID != 0)
    {
        bdNChar8 transactionHeader[21];
        bdSnprintf(transactionHeader, sizeof(transactionHeader), "%llu", transactionID);
        ok = ok && m_http->setHeader("DW-Transaction", transactionHeader);
    }
    ok = ok && m_http->setHeader("Authorization", authorizationHeader);
    ok = ok && m_http->setDownloadHandler(downloadHandler);
    if (!ok || m_http->sendRequest() != bdHTTP::BD_PENDING)
    {
        bdLogWarn("objectstore", "Failed to start download of %s", downloadURL);
    }
    return m_http->getStatus();
}

void bdObjectStoreStreaming::initHTTP()
{
    if (m_state == BD_DONE)
    {
        bdLogWarn("objectstore", "Already done");
        return;
    }
    bdHTTP::bdStatus status = bdHTTP::BD_FAILED;

    if (m_operation == BD_DOWNLOAD_OBJECT)
    {
        if (m_downloadHandler)
        {
            status = startContentDownload(m_metadata->getContentURL(),
                                          m_downloadHandler,
                                          m_remoteTask->getTransactionID(),
                                          m_authorizationHeader);
        }
        else
        {
            bdLogWarn("objectstore", "No download handler");
            setState(BD_FAILED, BD_NO_ERROR);
        }
    }

    if (m_state != BD_DONE)
    {
        if (status == bdHTTP::BD_PENDING || status == bdHTTP::BD_DONE)
        {
            setState(BD_HTTP_OPERATION, BD_NO_ERROR);
        }
        else
        {
            setState(BD_FAILED, BD_OBJECTSTORE_FAILED_TO_START_HTTP);
        }
    }
}
