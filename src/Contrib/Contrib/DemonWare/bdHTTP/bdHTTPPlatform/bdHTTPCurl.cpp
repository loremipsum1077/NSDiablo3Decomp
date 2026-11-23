// Original file: Contrib/Contrib/DemonWare/bdHTTP/bdHTTPPlatform/bdHTTPCurl.cpp
#include "bdHTTPCurl.h"
#include "../../bdCore/bdThread/bdSemaphore.h"
#include "../../bdCore/bdThread/bdThread.h"
#include "../../bdLobby/bdContentStreaming/bdDownloadInterceptor.h"
#include "../../bdLobby/bdContentStreaming/bdUploadInterceptor.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "../../bdPlatform/bdPlatformTiming/bdPlatformTiming.h"
#include "../../../../../NintendoSDK/Include/nn/nn_Result.h"
#define _Bool bool  // the recovered curl.h is a C header
#include "../../../../../Programs/Eris/Outputs/Include/curl/curl.h"
#include "../../../../../Programs/Eris/Outputs/Include/curl/multi.h"
#undef _Bool

// curl API used here. The recovered curl.h carries types and option values only, so the
// prototypes (upstream) and the few missing constants are declared here.
typedef void *(*curl_malloc_callback)(size_t size); typedef void (*curl_free_callback)(void * ptr);
typedef void *(*curl_realloc_callback)(void * ptr, size_t size); typedef char *(*curl_strdup_callback)(const char * str);
typedef void *(*curl_calloc_callback)(size_t nmemb, size_t size);
struct curl_version_info_data; enum { CURL_GLOBAL_SSL = 1 }; enum CURLMcode { CURLM_CALL_MULTI_PERFORM = -1, CURLM_OK = 0 };
enum CURLversion { CURLVERSION_FIRST, CURLVERSION_SECOND, CURLVERSION_THIRD, CURLVERSION_FOURTH, CURLVERSION_NOW = CURLVERSION_FOURTH };
enum CURLINFO { CURLINFO_RESPONSE_CODE = 0x200000 + 2, CURLINFO_CONTENT_LENGTH_DOWNLOAD = 0x300000 + 15 };
extern "C" {
CURLcode curl_global_init_mem(long flags, curl_malloc_callback m, curl_free_callback f, curl_realloc_callback r, curl_strdup_callback s, curl_calloc_callback c); void curl_global_cleanup(void);
curl_version_info_data * curl_version_info(CURLversion);
CURL * curl_easy_init(void); void curl_easy_cleanup(CURL * curl);
CURLcode curl_easy_setopt(CURL * curl, CURLoption option, ...);
CURLcode curl_easy_getinfo(CURL * curl, CURLINFO info, ...);
const char * curl_easy_strerror(CURLcode); CURLM * curl_multi_init(void); CURLMcode curl_multi_cleanup(CURLM * multi);
CURLMcode curl_multi_add_handle(CURLM * multi, CURL * curl); CURLMcode curl_multi_remove_handle(CURLM * multi, CURL * curl);
CURLMcode curl_multi_perform(CURLM * multi, int * running_handles); CURLMsg * curl_multi_info_read(CURLM * multi, int * msgs_in_queue);
struct curl_slist * curl_slist_append(struct curl_slist *, const char *); void curl_slist_free_all(struct curl_slist *);
}

// nn::ssl::Context (SDK import _ZN2nn3ssl7Context6CreateENS1_10SslVersionE); no DWARF for it.
namespace nn { namespace ssl {
class Context {
public:
    enum SslVersion { SslVersion_Auto = 1 };
    nn::Result Create(SslVersion version);
};
} }

char * bdStrdup(const char * string); void * bdCallocate(size_t nmemb, size_t size);


const bdNChar8 * httpThreadName = "bdHTTPCurl";

bdHTTPCurl::bdHTTPCurl() : bdHTTP(), bdRunnable(),
    m_threadExiting(false),
    m_thread(NULL),
    m_curl(NULL),
    m_headerList(NULL),
    m_responseHeaderList(NULL),
    m_uploadBuffer(NULL),
    m_uploadBufferSize(0),
    m_uploadHandler(NULL),
    m_downloadBuffer(NULL),
    m_downloadBufferSize(0),
    m_downloadHandler(NULL),
    m_internalErrorCode(0),
    m_stopWatch(),
    m_bytesTransferedUpload(0),
    m_bytesTransferedDownload(0), m_downloadHandlerGotSize(false),
    m_caPath(NULL),
    m_userAgentHeader(NULL),
    m_httpErrorCode(0)
{



    m_httpSemaphore = new bdSemaphore(0, 2);
    init();
}

bdHTTPCurl::~bdHTTPCurl()
{
    if (m_caPath)
    {
        bdMemory::deallocate(m_caPath);
        m_caPath = NULL;
    }

    if (m_userAgentHeader)
    {
        bdMemory::deallocate(m_userAgentHeader);
        m_userAgentHeader = NULL;
    }


    abortOperation();
    finalize();

    if (m_curl)
    {
        curl_easy_cleanup(m_curl);
    }

    if (m_headerList)
    {
        curl_slist_free_all(m_headerList);
        m_headerList = NULL;
    }

    if (m_responseHeaderList)
    {
        curl_slist_free_all(m_responseHeaderList);
        m_responseHeaderList = NULL;
    }

    curl_global_cleanup();







    m_httpSemaphore->destroy();

}



int debug_callback(CURL *, curl_infotype type, char * data, size_t, void *)
{
    switch (type) { case CURLINFO_TEXT: bdLogInfo("http", "%s", data); break; case CURLINFO_HEADER_IN: bdLogInfo("http", "%s", data); break; case CURLINFO_HEADER_OUT: bdLogInfo("http", "%s", data); break; default: break; }






















    return 0;
}


inline void bdHTTPCurl::init()
{
    m_errorBuffer[0] = '\0';

    curl_global_init_mem(CURL_GLOBAL_SSL,
                         bdMemory::getAllocateFunc(),
                         bdMemory::getDeallocateFunc(),
                         bdMemory::getReallocateFunc(), bdStrdup, bdCallocate);



    curl_version_info(CURLVERSION_NOW);



    m_status = BD_EMPTY;
}





inline void bdHTTPCurl::reset()
{

    m_bytesTransferedUpload = 0; m_bytesTransferedDownload = 0; m_downloadHandlerGotSize = false;
    m_downloadHandler = NULL;

    m_uploadHandler = NULL; m_downloadBuffer = NULL; m_downloadBufferSize = 0;
    m_uploadBufferSize = 0;


    m_uploadBuffer = NULL;
    m_httpErrorCode = 0;
    m_internalErrorCode = 0;

    m_status = BD_EMPTY;

    if (m_curl)
    {
        curl_easy_cleanup(m_curl);
    }

    m_curl = curl_easy_init();

    if (m_headerList)
    {
        curl_slist_free_all(m_headerList);
        m_headerList = NULL;
    }

    if (m_responseHeaderList)
    {
        curl_slist_free_all(m_responseHeaderList);
        m_responseHeaderList = NULL;
    }
}

bdBool bdHTTPCurl::initRequest(bdHTTP::bdOperation operation,
                             const bdNChar8 *const url,
                             const bdUInt32 timeout)
{
    if (!isState(BD_FINISHED) && !isState(BD_IDLE))
    {
        return false;
    }
    reset();

    bdBool result = false;
    if (m_curl)
    {
        bdBool ok = true;



        ok = curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 0) == CURLE_OK;

        ok = ok && curl_easy_setopt(m_curl, CURLOPT_DEBUGFUNCTION, debug_callback) == CURLE_OK;






        ok = ok && curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, m_errorBuffer) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_URL, url) == CURLE_OK;

        switch (operation)
        {
        case BD_GET:

            ok = ok && curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1) == CURLE_OK;
            break;

        case BD_DELETE:

            ok = ok && curl_easy_setopt(m_curl, CURLOPT_NOBODY, 1) == CURLE_OK;
            ok = ok && curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "DELETE") == CURLE_OK;
            break;

        case BD_COPY:
        {
            ok = ok && curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "COPY") == CURLE_OK;

            bdNChar8 * startRequestPath = NULL;
            bdNChar8 serverName[512];
            bdUInt32 port;

            const bdBool parsed = parseURL(url, serverName, &startRequestPath, port);

            bdNChar8 hostHeader[525];



            if (port == 443 || port == 80)
            {

                bdSnprintf(hostHeader, sizeof(hostHeader), "Host: %s", serverName);
            }
            else
            {
                bdSnprintf(hostHeader, sizeof(hostHeader), "Host: %s:%d", serverName, port);
            }
            m_headerList = curl_slist_append(m_headerList, hostHeader);
            m_headerList = curl_slist_append(m_headerList, "Overwrite: T");

            if (!parsed) { ok = false; }
            break;
        }
        case BD_PUT:
            ok = ok && curl_easy_setopt(m_curl, CURLOPT_PUT, 1) == CURLE_OK;
            m_headerList = curl_slist_append(m_headerList, "Expect:");
            break;

        default:

            ok = ok && curl_easy_setopt(m_curl, CURLOPT_POST, 1) == CURLE_OK;
            break;
        }

        m_status = BD_INITIALIZING;
        result = ok && setTimeout(timeout);
        if (!result)
        {

            bdLogWarn("http", "Failed to initialize request");
            reset();
        }
    }

    return result;
}
bdBool bdHTTPCurl::setHeader(const bdNChar8 * key,
                           const bdNChar8 * value)
{
    bdBool ok = false;
    if (isState(BD_CREATING))
    {
        bdNChar8 header[5120];

        const bdUInt length = bdSnprintf(header, sizeof(header), "%s: %s", key, value);

        if (length < sizeof(header))
        {
            m_headerList = curl_slist_append(m_headerList, header); ok = true;
        }
        else
        {

            bdLogWarn("http", "Header too long, max %u", sizeof(header));
        }
    }
    else
    {

        bdLogWarn("http", "setHeader called in the wrong state");
    }

    return ok;
}
bdBool bdHTTPCurl::getHeader(const bdNChar8 * key,
                           bdNChar8 * outBuffer,
                           const bdUInt32 outBufferSize)
{
    bdBool found = false;
    if (isState(BD_FINISHED))
    {
        bdNChar8 prefix[5120];
        const bdInt length = bdSnprintf(prefix, sizeof(prefix), "%s: ", key);
        if (length < 5120)
        {

            for (curl_slist * header = m_responseHeaderList; header != NULL; header = header->next)
            {
                if (bdStrncmp(prefix, header->data, length) == 0)
                {
                    bdStrlcpy(outBuffer, header->data + length, outBufferSize);
                    found = true;
                    break;
                }
            }
            if (!found)
            {



                bdLogWarn("http", "Header %s not found", key);
            }
        }
        else
        {
            bdLogWarn("http", "Header name too long: %u, max %u", length, sizeof(prefix));
        }
    }
    else
    {


        bdLogWarn("http", "getHeader called in the wrong state");
    }

    return found;
}

int bdHTTPCurl::curlSilentResponseHandler(char *, size_t size, size_t nItems, void *)
{
    return static_cast<int>(size * nItems);
}


size_t bdHTTPCurl::curlSslContextCallback(CURL * curl, void * sslContext, void * userData)
{





    const nn::Result result = static_cast<nn::ssl::Context *>(sslContext)->Create(nn::ssl::Context::SslVersion_Auto);

    if (result.IsFailure())
    {
        return static_cast<size_t>(-1);
    }


    return 0;
}

bdBool bdHTTPCurl::setUploadData(const bdByte8 * buffer, bdUInt32 size)
{
    if (!isState(BD_CREATING))
    {
        return false;
    }

    bdBool ok = true;

    if (size != 0)
    {
        bdNChar8 contentLength[11];
        bdSnprintf(contentLength, sizeof(contentLength), "%u", size);

        ok = setHeader("Content-Length", contentLength);
    }




    ok = ok && curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlSilentResponseHandler) == CURLE_OK;
    ok = ok && curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this) == CURLE_OK;

    ok = ok && curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, curlUploadCallback) == CURLE_OK;
    ok = ok && curl_easy_setopt(m_curl, CURLOPT_READDATA, this) == CURLE_OK;

    if (ok && buffer != NULL && size != 0)
    {
        m_uploadBuffer = buffer;
        m_uploadBufferSize = size;
    }

    if (size != 0)
    {
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(m_uploadBufferSize)) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_FAILONERROR, 0) == CURLE_OK;
    }

    if (!ok)
    {
        reset();
    }


    return ok;
}
bdBool bdHTTPCurl::setUploadHandler(bdUploadInterceptor * handler, bdUInt32 size)
{
    if (!isState(BD_CREATING))
    {
        return false;
    }

    bdBool ok = true;

    if (size != 0)
    {
        bdNChar8 contentLength[11];
        bdSnprintf(contentLength, sizeof(contentLength), "%u", size);

        ok = setHeader("Content-Length", contentLength);

        m_uploadBufferSize = size;
    }




    ok = ok && curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlSilentResponseHandler) == CURLE_OK;
    ok = ok && curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this) == CURLE_OK;

    ok = ok && curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, curlUploadCallback) == CURLE_OK;
    ok = ok && curl_easy_setopt(m_curl, CURLOPT_READDATA, this) == CURLE_OK;

    if (handler == NULL || size != 0)
    {
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(m_uploadBufferSize)) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_FAILONERROR, 0) == CURLE_OK;
    }
    else
    {
        ok = ok && setHeader("Transfer-Encoding", "chunked");
    }

    if (ok)
    {
        m_uploadHandler = handler;
    }
    else
    {
        reset();
    }


    return ok;
}
bdBool bdHTTPCurl::setDownloadBuffer(bdByte8 * buffer, bdUInt32 size)
{
    if (!isState(BD_CREATING))
    {
        return false;
    }
    bdBool ok = true;
    if (buffer != NULL && size != 0)
    {
        m_downloadBuffer = buffer;
        m_downloadBufferSize = size;

        ok = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlDownloadCallback) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, curlHeaderCallback) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this) == CURLE_OK;
    }


    return ok;
}
bdBool bdHTTPCurl::setDownloadHandler(bdDownloadInterceptor * handler)
{
    if (!isState(BD_CREATING))
    {
        return false;
    }
    bdBool ok = true;

    if (handler != NULL)
    {
        ok = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlDownloadCallback) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, curlHeaderCallback) == CURLE_OK;
        ok = ok && curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this) == CURLE_OK;
        m_downloadHandler = handler;
    }


    return ok;
}
bdBool bdHTTPCurl::setTimeout(const bdUInt32 timeout)
{
    bdBool ok = false;
    if (isState(BD_CREATING))
    {
        m_timeout = timeout; ok = true;
    }
    else
    {
        bdLogWarn("http", "setTimeout called in the wrong state: %u", m_status);







        m_status = BD_FAILED;
        abortOperation();
    }
    return ok;
}

bdHTTP::bdStatus bdHTTPCurl::sendRequest()
{
    if (!isState(BD_CREATING))
    {
        m_status = BD_FAILED;
    }
    else
    {




    bdBool ok = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerList) == CURLE_OK;

    if (m_timeout != 0)
    {

        ok = ok && curl_easy_setopt(m_curl, CURLOPT_TIMEOUT_MS, m_timeout) == CURLE_OK;
        if (!ok)
        {
            bdLogWarn("http", "Failed to set the timeout");
        }
    }
    ok = ok && curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 60) == CURLE_OK;
    ok = ok && curl_easy_setopt(m_curl, CURLOPT_DNS_CACHE_TIMEOUT, -1) == CURLE_OK;

    ok = ok && initCurlSSL();



    ok = ok && curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1) == CURLE_OK;
    ok = ok && curl_easy_setopt(m_curl, CURLOPT_FAILONERROR, 0) == CURLE_OK;

    if (ok)
    {
        m_bytesTransferedUpload = 0; m_bytesTransferedDownload = 0;

        m_stopWatch.reset();
        m_stopWatch.start();
        startAsyncOperation();
    }
    else
    {

        curl_easy_cleanup(m_curl);
        m_curl = NULL;
    }
    }

    return m_status;
}

bdHTTP::bdStatus bdHTTPCurl::getStatus()
{
    return m_status;
}

bdBool bdHTTPCurl::getResponseLength(bdUInt32 & responseLength)
{
    responseLength = 0;
    return isState(BD_RUNNING);
}






bdUInt32 bdHTTPCurl::getLastHTTPStatus() const
{
    return m_httpErrorCode;
}

bdInt32 bdHTTPCurl::getInternalError() const
{
    if (isState(BD_RUNNING))
    {
        return BD_HTTP_NO_ERROR;
    }
    return m_internalErrorCode;
}

bdBool bdHTTPCurl::clearResult()
{
    return true;
}

void bdHTTPCurl::finalize()
{
    if (m_thread != NULL && !m_threadExiting)
    {
        m_threadExiting = true;

        m_httpSemaphore->release();
        m_thread->join();

        m_thread->cleanup();
        m_thread = NULL;
        m_threadExiting = false;
    }
}

void bdHTTPCurl::abortOperation()
{
    m_status = BD_ABORTING;
}

bdHTTP::bdStatus bdHTTPCurl::startAsyncOperation()
{
    if (!isState(BD_CREATING))
    {
        return BD_FAILED;
    }


    bdBool started = true;
    if (m_threadExiting) { started = false; }
    else
    {
        if (m_thread == NULL)
        {
            m_thread = new bdThread(this, 16, 0x8000, 0);
            started = m_thread->start(NULL, 0, httpThreadName);
        }
    }
    if (started)
    {
        m_status = BD_PENDING;
        m_httpSemaphore->release();
    }
    else
    {
        bdLogError("http", "Failed to start the HTTP thread");
        m_status = BD_FAILED;
    }

    return m_status;
}

bdUInt bdHTTPCurl::run(void *)
{
    bdLogInfo("http", "HTTP thread started");

    do
    {
        m_httpSemaphore->wait();
        if (m_curl != NULL && m_status == BD_PENDING)
        {
            performOperation();
        }
    } while (!m_threadExiting);
    bdLogInfo("http", "HTTP thread exiting");
    return 0;
}

void bdHTTPCurl::performOperation()
{



    m_downloadHandlerGotSize = false;




    CURLM * multi = curl_multi_init();
    CURLMcode multiResult = curl_multi_add_handle(multi, m_curl);

    bdInt stillRunning = 1;
    CURLcode result = CURLE_FAILED_INIT;


    while (stillRunning > 0 && (multiResult == CURLM_OK || multiResult == CURLM_CALL_MULTI_PERFORM))
    {
        const bdUInt32 transferedBefore = m_bytesTransferedUpload + m_bytesTransferedDownload;


        multiResult = curl_multi_perform(multi, &stillRunning);

        if (m_status == BD_ABORTING)
        {
            result = CURLE_ABORTED_BY_CALLBACK;
            break;
        }

        if (multiResult == CURLM_OK && transferedBefore == m_bytesTransferedUpload + m_bytesTransferedDownload)
        {



            bdPlatformTiming::sleep(30);
        }
    }

    if ((multiResult == CURLM_OK || multiResult == CURLM_CALL_MULTI_PERFORM) && m_status != BD_ABORTING)
    {

        bdInt msgsInQueue = 0;
        do
        {
            CURLMsg * msg = curl_multi_info_read(multi, &msgsInQueue);
            if (msg != NULL && msg->easy_handle == m_curl && msg->msg == CURLMSG_DONE)
            {

                result = msg->data.result;
            }
        } while (msgsInQueue > 0);
    }

    if (m_status == BD_ABORTING)
    {
        m_status = BD_ABORTED;
    }


    curl_multi_remove_handle(multi, m_curl);
    curl_multi_cleanup(multi);

    bdHTTP::bdStatus newStatus = BD_FAILED;
    if (result != CURLE_OK)
    {


        const char * error = curl_easy_strerror(result);
        bdLogWarn("http", "Request failed: %s", error);
        switch (result)
        {
        case CURLE_COULDNT_CONNECT:
            m_internalErrorCode = BD_HTTP_CONNECT_ERROR;
            break;
        case CURLE_SSL_CONNECT_ERROR:
            m_internalErrorCode = BD_HTTP_SSL_CERTIFICATE_ERROR;
            break;
        case CURLE_COULDNT_RESOLVE_HOST:
            m_internalErrorCode = BD_HTTP_DNS_LOOKUP_FAILED_ERROR;
            break;
        case CURLE_OPERATION_TIMEDOUT:
            m_internalErrorCode = BD_HTTP_TIMEOUT;
            break;
        default:
            m_internalErrorCode = result;
            break;
        }
    }
    else
    {
        m_httpErrorCode = 0;





        bdUInt32 responseCode = 0;
        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &responseCode);
        m_httpErrorCode = responseCode;

        if (responseCode < 200 || responseCode >= 300)
        {


            const bdUWord errorLength = bdStrlen(m_errorBuffer);
            if (errorLength != 0)
            {
                bdLogWarn("http", "HTTP status %u: %s", responseCode, m_errorBuffer);

                if (m_downloadBuffer)
                {
                    bdMemcpy(m_downloadBuffer, m_errorBuffer, errorLength > m_downloadBufferSize ? m_downloadBufferSize : errorLength);
                }
                m_errorBuffer[0] = '\0';
            }
            else
            {

                bdLogWarn("http", "HTTP status %u", responseCode);
            }
        }
        else
        {
            bdLogInfo("http", "Transferred %u bytes at %f bytes/s", m_bytesTransferedUpload + m_bytesTransferedDownload, getUploadRate());
            newStatus = BD_DONE;
        }
    }
























    if (m_headerList)
    {
        curl_slist_free_all(m_headerList);
        m_headerList = NULL;
    }

    curl_easy_cleanup(m_curl);
    m_curl = NULL;
    m_status = newStatus;
}






bdUInt32 bdHTTPCurl::getUploadProgress() const
{
    return m_bytesTransferedUpload;
}

bdUInt32 bdHTTPCurl::getDownloadProgress() const
{
    return m_bytesTransferedDownload;
}

bdFloat32 bdHTTPCurl::getUploadRate() const
{
    if (isState(BD_RUNNING))
    {
        if (m_stopWatch.getElapsedTimeInSeconds() >= 0.1f)
        {
            return static_cast<bdFloat32>(m_bytesTransferedUpload) / m_stopWatch.getElapsedTimeInSeconds();
        }
    }

    return 0.0f;
}

bdFloat32 bdHTTPCurl::getDownloadRate() const
{
    if (isState(BD_RUNNING))
    {
        if (m_stopWatch.getElapsedTimeInSeconds() >= 0.1f)
        {
            return static_cast<bdFloat32>(m_bytesTransferedDownload) / m_stopWatch.getElapsedTimeInSeconds();
        }
    }

    return 0.0f;
}

inline bdBool bdHTTPCurl::initCurlSSL()
{
    bdBool ok = true;


















    ok = ok && curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 2) == CURLE_OK;

    ok = ok && curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 1) == CURLE_OK;

    ok = ok && curl_easy_setopt(m_curl, CURLOPT_SSL_CTX_FUNCTION, curlSslContextCallback) == CURLE_OK;
    return ok;
}







































































bdBool bdHTTPCurl::parseURL(const bdNChar8 *const url,
                          bdNChar8 * serverName,
                          bdNChar8 ** startRequestPath,
                          bdUInt32 & port)
{

    if (url == NULL)
    {
        bdLogError("http", "No URL");
        return false;
    }











    const bdNChar8 * startServerName = url;
    if (bdStrncmp(url, "http", 4) == 0)
    {









        const bdNChar8 * colon = bdStrchr(url, ':');
        if (colon == NULL)
        {
            bdLogError("http", "Malformed URL %s", url);
            return false;
        }

        startServerName = colon + 3;
    }


    *startRequestPath = const_cast<bdNChar8 *>(bdStrchr(startServerName, '/'));
    if (*startRequestPath == NULL)
    {
        bdLogError("http", "Malformed URL %s", url);
        return false;
    }
    const bdNChar8 * endServerName;

    const bdNChar8 * portColon = bdStrchr(startServerName, ':');
    if (portColon == NULL)
    {
        port = (bdStrncmp(url, "https://", 8) == 0) ? 443 : 80;
        endServerName = *startRequestPath - 1;
    }
    else
    {

        port = atoi(portColon + 1);

        endServerName = portColon - 1;

        if (startServerName > endServerName || endServerName > *startRequestPath)
        {
            bdLogError("http", "Malformed URL %s", url);
            return false;
        }

        if (port == 0)
        {
            bdLogError("http", "Malformed URL %s", url);
            return false;
        }
    }




    const bdInt serverNameLength = static_cast<bdInt>(endServerName - startServerName) + 2;

    if (serverNameLength > 512u)
    {
        bdLogError("http", "Malformed URL %s", url);
        return false;
    }


    bdStrlcpy(serverName, startServerName, serverNameLength);















    return true;
}




int bdHTTPCurl::curlDownloadCallback(char * curlBuffer, size_t size, size_t nItems, void * httpWrapper)
{
    bdHTTPCurl * http = static_cast<bdHTTPCurl *>(httpWrapper);

    if (http->m_downloadHandler)
    {

        if (!http->m_downloadHandlerGotSize)
        {
            bdBool unknownLength = false; bdUInt32 length = 0;
            double contentLength;


            const CURLcode result = curl_easy_getinfo(http->m_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLength);

            if (result != CURLE_OK)
            {
                bdLogWarn("http", "Failed to get the content length");
                return result;
            }
            if (contentLength < 0.0)
            {
                contentLength = 0.0; unknownLength = true;
            }
            else
            {

                length = static_cast<bdUInt32>(contentLength + 0.5);
            }




            if (!http->m_downloadHandler->handleContentLength(length, unknownLength))
            {

                http->abortOperation();
                bdLogWarn("http", "Download handler rejected content length %u (unknown: %s)", length, unknownLength ? "true" : "false");
                return -1;
            }
        }



        return http->userDownloadCallback(curlBuffer, size, nItems);
    }
    return http->defaultDownloadCallback(curlBuffer, size, nItems);








}





int bdHTTPCurl::curlUploadCallback(char * curlBuffer, size_t size, size_t nItems, void * httpWrapper)
{
    bdHTTPCurl * http = static_cast<bdHTTPCurl *>(httpWrapper);
    return http->m_uploadHandler ? http->userUploadCallback(curlBuffer, size, nItems) : http->defaultUploadCallback(curlBuffer, size, nItems);
}



inline int bdHTTPCurl::defaultDownloadCallback(char * curlBuffer, size_t size, size_t nItems)
{
    if (isState(BD_STOPPING) || curlBuffer == NULL)
    {
        return 0;
    }

    const bdUInt bytes = static_cast<bdUInt>(size * nItems);
    if (m_downloadBufferSize >= m_bytesTransferedDownload + bytes)
    {
        bdMemcpy(m_downloadBuffer + m_bytesTransferedDownload, curlBuffer, bytes);
        m_bytesTransferedDownload += bytes;
        return bytes;
    }

    bdLogWarn("http", "Download buffer too small");
    return 0;
}




inline int bdHTTPCurl::defaultUploadCallback(char * curlBuffer, size_t size, size_t nItems)
{
    if (isState(BD_STOPPING) || curlBuffer == NULL)
    {
        return 0;
    }

    bdUInt bytes = 0;

    if (m_bytesTransferedUpload < m_uploadBufferSize)
    {
        const bdUInt remaining = m_uploadBufferSize - m_bytesTransferedUpload; bytes = remaining < bdUInt(size) * bdUInt(nItems) ? remaining : bdUInt(size) * bdUInt(nItems);
        bdMemcpy(curlBuffer, m_uploadBuffer + m_bytesTransferedUpload, bytes);
        m_bytesTransferedUpload += bytes;
    }
    return bytes;
}



inline int bdHTTPCurl::userDownloadCallback(char * data, size_t size, size_t nItems)
{
    if (isState(BD_STOPPING))
    {
        return 0;
    }

    const bdUInt bytes = bdUInt(size) * bdUInt(nItems);
    const bdInt handled = m_downloadHandler->handleDownload(data, bytes);
    m_bytesTransferedDownload += bytes;
    return handled;
}


inline int bdHTTPCurl::userUploadCallback(char * data, size_t size, size_t nItems)
{
    if (isState(BD_STOPPING))
    {
        return 0;
    }

    const bdUInt bytes = bdUInt(size) * bdUInt(nItems);
    const bdInt uploaded = m_uploadHandler->handleUpload(data, bytes, m_bytesTransferedUpload);

    if (uploaded > 0)
    {
        m_bytesTransferedUpload += uploaded;
    }
    return uploaded;
}


int bdHTTPCurl::curlHeaderCallback(char * curlBuffer, size_t size, size_t nItems, void * httpWrapper)
{
    return static_cast<bdHTTPCurl *>(httpWrapper)->defaultHeaderCallback(curlBuffer, size, nItems);
}


inline int bdHTTPCurl::defaultHeaderCallback(char * curlBuffer, size_t size, size_t nItems)
{
    if (isState(BD_STOPPING) || curlBuffer == NULL)
    {
        return 0;
    }

    const bdUInt bytes = bdUInt(size) * bdUInt(nItems);
    m_responseHeaderList = curl_slist_append(m_responseHeaderList, curlBuffer);
    return bytes;
}
















































void * bdCallocate(size_t nmemb, size_t size)
{
    const size_t total = nmemb * size;
    void * const p = bdMemory::allocate(total);
    bdMemset(p, 0, total);

    return p;
}


char * bdStrdup(const char * string)
{
    char * copy = NULL;


    if (string != NULL)
    {

        const bdUWord size = bdStrlen(string) + 1;
        copy = static_cast<char *>(bdMemory::allocate(size));
        if (copy != NULL)
        {
            bdStrlcpy(copy, string, size);
        }
    }


    return copy;
}
