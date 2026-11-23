// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdHTTP/bdHTTPPlatform/bdHTTPCurl.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdThread/bdRunnable.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../bdHTTP.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>
// #include <curl/curl.h>  // Nintendo SDK
// From curl/curl.h (the mangled bdHTTPCurl::curlSslContextCallback takes void *).
typedef void CURL;

class bdDownloadInterceptor;
class bdSemaphore;
class bdThread;
class bdUploadInterceptor;
struct curl_slist;

class bdHTTPCurl : public bdHTTP, public bdRunnable {  // size 0x4b0
public:
    BD_CLASS_ALLOCATOR(bdHTTPCurl);
    struct bdHTTPHeader {  // size 0x10; DWARF bdHTTPCurl.h:47 (layout from the bdAuth.cpp CU)
        const bdNChar8 * key;  // 0x00, line 49
        const bdNChar8 * value;  // 0x08, line 50
    };
    bdHTTPCurl();
    virtual ~bdHTTPCurl();  // vtable slot 0
    enum bdHTTPWrapperInternalErrors {
        BD_HTTP_NO_ERROR = 0,
        BD_HTTP_SSL_CERTIFICATE_ERROR = 1,
        BD_HTTP_DNS_LOOKUP_FAILED_ERROR = 2,
        BD_HTTP_THIRD_PARTY_TOKEN_RETRIEVAL_ERROR = 3,
        BD_HTTP_CONNECT_ERROR = 4,
        BD_HTTP_ABORTED = 5,
        BD_HTTP_TIMEOUT = 6,
    };
    virtual bdBool initRequest(bdHTTP::bdOperation operation, const bdNChar8 *const url, const bdUInt32 timeout);  // vtable slot 2
    virtual bdBool setHeader(const bdNChar8 * key, const bdNChar8 * value);  // vtable slot 3
    virtual bdBool getHeader(const bdNChar8 * key, bdNChar8 * outBuffer, const bdUInt32 outBufferSize);  // vtable slot 5
    // Inferred (not in the DWARF): bdAuth::makeAuth (bdAuth.cpp:243-244) calls the bdNChar8 overloads
    // of bdHTTP through a bdHTTPCurl pointer, so the base overloads must not be hidden here.
    using bdHTTP::setUploadData;
    using bdHTTP::setDownloadBuffer;
    virtual bdBool setUploadData(const bdByte8 * buffer, bdUInt32 size);  // vtable slot 6
    virtual bdBool setUploadHandler(bdUploadInterceptor * handler, bdUInt32 size);  // vtable slot 7
    virtual bdBool setDownloadBuffer(bdByte8 * buffer, bdUInt32 size);  // vtable slot 8
    virtual bdBool setDownloadHandler(bdDownloadInterceptor * handler);  // vtable slot 9
    virtual bdBool setTimeout(const bdUInt32 timeout);  // vtable slot 10
    virtual bdHTTP::bdStatus sendRequest();  // vtable slot 11
    virtual bdHTTP::bdStatus getStatus();  // vtable slot 12
    virtual bdBool getResponseLength(bdUInt32 & responseLength);  // vtable slot 13
    virtual bdUInt32 getLastHTTPStatus() const;  // vtable slot 14
    virtual bdInt32 getInternalError() const;  // vtable slot 15
    virtual void abortOperation();  // vtable slot 16
    virtual void finalize();  // vtable slot 17
    virtual bdUInt32 getUploadProgress() const;  // vtable slot 18
    virtual bdUInt32 getDownloadProgress() const;  // vtable slot 19
    virtual bdFloat32 getUploadRate() const;  // vtable slot 20
    virtual bdFloat32 getDownloadRate() const;  // vtable slot 21
    virtual bdBool clearResult();  // vtable slot 22
    void init();
    void setUserAgent(const bdNChar8 *const);
    void setCAPath(const bdNChar8 *);
    virtual bdUInt run(void *);  // vtable slot 24
    int defaultDownloadCallback(char * curlBuffer, size_t size, size_t nItems);
    int defaultUploadCallback(char * curlBuffer, size_t size, size_t nItems);
    int defaultHeaderCallback(char * curlBuffer, size_t size, size_t nItems);
    int userDownloadCallback(char * data, size_t size, size_t nItems);
    int userUploadCallback(char * data, size_t size, size_t nItems);
    static int curlDownloadCallback(char * curlBuffer, size_t size, size_t nItems, void * httpWrapper);
    static int curlUploadCallback(char * curlBuffer, size_t size, size_t nItems, void * httpWrapper);
    static int curlHeaderCallback(char * curlBuffer, size_t size, size_t nItems, void * httpWrapper);
    static int curlSilentResponseHandler(char *, size_t size, size_t nItems, void *);
    CURL * getCurl();
    static size_t curlSslContextCallback(CURL * curl, void * sslContext, void * userData);
protected:
    void reset();
    void cleanup();
    bdBool initCurlSSL();
    bdBool parseURL(const bdNChar8 *const url, bdNChar8 * serverName, bdNChar8 ** startRequestPath, bdUInt32 & port);
    bdHTTP::bdStatus startAsyncOperation();
    void performOperation();
    bdBool getStatusCode();
    bdBool m_threadExiting;  // 0x19
    bdSemaphore * m_httpSemaphore;  // 0x20
    bdThread * m_thread;  // 0x28
    CURL * m_curl;  // 0x30
    curl_slist * m_headerList;  // 0x38
    curl_slist * m_responseHeaderList;  // 0x40
    const bdByte8 * m_uploadBuffer;  // 0x48
    bdUInt32 m_uploadBufferSize;  // 0x50
    bdUploadInterceptor * m_uploadHandler;  // 0x58
    bdByte8 * m_downloadBuffer;  // 0x60
    bdUInt32 m_downloadBufferSize;  // 0x68
    bdDownloadInterceptor * m_downloadHandler;  // 0x70
    bdNChar8 m_errorBuffer[1024];  // 0x78
    bdInt m_internalErrorCode;  // 0x478
    bdStopwatch m_stopWatch;  // 0x480
    bdUInt32 m_bytesTransferedUpload;  // 0x488
    bdUInt32 m_bytesTransferedDownload;  // 0x48c
    bdBool m_downloadHandlerGotSize;  // 0x490
    bdNChar8 * m_caPath;  // 0x498
    bdNChar8 * m_userAgentHeader;  // 0x4a0
    bdInt m_httpErrorCode;  // 0x4a8
};
static_assert(sizeof(bdHTTPCurl) == 0x4b0, "bdHTTPCurl");


// Functions defined in this file (bodies are not in the DWARF):
//   line    45  void bdHTTPCurl::operator delete(void * p)  [inlined into 1 functions]
//   line    45  void * bdHTTPCurl::operator new(size_t nbytes)  [inlined into 1 functions]
