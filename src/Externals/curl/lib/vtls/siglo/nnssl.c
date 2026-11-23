/* Externals/curl/lib/vtls/siglo/nnssl.c: Nintendo's nn::ssl backend for libcurl's vtls layer.
 * Functions sit on their original line numbers (2.6.2 DWARF, CU 0x07c8a8a6). */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "Externals/curl/lib/urldata.h"
#include "Externals/curl/lib/vtls/siglo/nnssl.h"
#include <fcntl.h>
/* nn_Result.h:23-26 (Programs/Chris/Include/nnc), DWARF; local because no header defines it yet. */
typedef unsigned int nnResultInnerType;
typedef struct nnResult { nnResultInnerType _value; } nnResult;
typedef unsigned long nnsslSslConnectionId;  /* ssl_Types.h:19 */
typedef unsigned long nnsslSslContextId;  /* ssl_Types.h:18 */

/* nn SDK functions. The names are the Ghidra import's (the original binary has no symbols for
   them); the prototypes are inferred from the call sequences, so they are not DWARF facts. */
_Bool nnResultIsFailure(nnResult result);
_Bool nnResultIsSuccess(nnResult result);
int nnResultGetModule(nnResult result);
int nnResultGetDescription(nnResult result);
nnResult nnsslInitialize(void);
void nnsslFinalize(void);
nnResult nnsslConnectionGetConnectionId(const nnsslSslConnectionType *pConnection, nnsslSslConnectionId *pId);
nnResult nnsslConnectionDestroy(nnsslSslConnectionType *pConnection);
nnResult nnsslContextGetContextId(const nnsslSslContextType *pContext, nnsslSslContextId *pId);
nnResult nnsslContextDestroy(nnsslSslContextType *pContext);
nnResult nnsslConnectionPeek(nnsslSslConnectionType *pConnection, void *pBuffer, int *pSize, size_t bufferSize);
nnResult nnsslConnectionPending(const nnsslSslConnectionType *pConnection, int *pSize);
nnResult nnsslConnectionRead(nnsslSslConnectionType *pConnection, void *pBuffer, int *pSize, size_t bufferSize);
nnResult nnsslConnectionWrite(nnsslSslConnectionType *pConnection, const void *pBuffer, int *pSize, size_t bufferSize);

/* Results the code compares against; inferred from the immediates (module 123, description in the
   upper bits): 0x1927b after nnsslInitialize, 0x1987b from the IO calls. */
static const nnResult cnnResult_0x1927b = { 0x1927b };
static const nnResult cnnResult_0x1987b = { 0x1987b };

/* ssl_Connection.h:33-60 enumerations (DWARF, CU 0x07c8a8a6); local because ssl_Connection.h lacks them. */
typedef enum nnsslConnectionVerifyOption { nnsslConnectionVerifyOption_None = 0, nnsslConnectionVerifyOption_PeerCa = 1, nnsslConnectionVerifyOption_HostName = 2, nnsslConnectionVerifyOption_DateCheck = 4, nnsslConnectionVerifyOption_EvCertPartial = 8, nnsslConnectionVerifyOption_All = 7 } nnsslConnectionVerifyOption;
typedef enum nnsslConnectionIoMode { nnsslConnectionIoMode_Blocking = 1, nnsslConnectionIoMode_NonBlocking = 2 } nnsslConnectionIoMode;
typedef enum nnsslConnectionSessionCacheMode { nnsslConnectionSessionCacheMode_None = 0, nnsslConnectionSessionCacheMode_SessionId = 1 } nnsslConnectionSessionCacheMode;
typedef enum nnsslConnectionOptionType { nnsslConnectionOptionType_DoNotCloseSocket = 0, nnsslConnectionOptionType_GetServerCertChain = 1, nnsslConnectionOptionType_SkipDefaultVerify = 2 } nnsslConnectionOptionType;

/* More nn SDK functions: Ghidra import names, prototypes inferred from the call sequences. */
nnResult nnsslConnectionCreate(nnsslSslConnectionType *pConnection, nnsslSslContextType *pContext);
nnResult nnsslConnectionSetOption(nnsslSslConnectionType *pConnection, nnsslConnectionOptionType optionType, _Bool enable);
nnResult nnsslConnectionSetSocketDescriptor(nnsslSslConnectionType *pConnection, int socketDescriptor);
nnResult nnsslConnectionSetHostName(nnsslSslConnectionType *pConnection, const char *pHostName, uint32_t hostNameLength);
nnResult nnsslConnectionSetVerifyOption(nnsslSslConnectionType *pConnection, nnsslConnectionVerifyOption optionValue);
nnResult nnsslConnectionSetSessionCacheMode(nnsslSslConnectionType *pConnection, nnsslConnectionSessionCacheMode mode);
nnResult nnsslConnectionSetIoMode(nnsslSslConnectionType *pConnection, nnsslConnectionIoMode mode);
nnResult nnsslConnectionDoHandshakeWithCertBuffer(nnsslSslConnectionType *pConnection, uint32_t *pOutServerCertSize, uint32_t *pOutServerCertCount);
nnResult nnsslConnectionGetVerifyCertError(nnsslSslConnectionType *pConnection, nnResult *pOutResult);
int nnsocketFcntl(int sockfd, int command, int flags);
/* connect.h (not in src yet). */
long Curl_timeleft(struct SessionHandle *data, struct timeval *nowp, _Bool duringconnect);
/* Forward declarations of the IO callbacks defined at 542 and 678 (inferred: nnssl_connect_common uses them first). */
static ssize_t nnssl_recv(struct connectdata *conn, int sockindex, char *buf, size_t buffersize, CURLcode *curlcode);
static ssize_t nnssl_send(struct connectdata *conn, int sockindex, const void *buf, size_t buffersize, CURLcode *curlcode);

/* More results the code compares against, named by value (immediates, module 123). */
static const nnResult cnnResult_0x19e7b = { 0x19e7b };
static const nnResult cnnResult_0x25a7b = { 0x25a7b };
static const nnResult cnnResult_0x2607b = { 0x2607b };
static const nnResult cnnResult_0x25e7b = { 0x25e7b };
static const nnResult cnnResult_0x2867b = { 0x2867b };
static const nnResult cnnResult_0x2687b = { 0x2687b };
static const nnResult cnnResult_0xbca7b = { 0xbca7b };
static const nnResult cnnResult_0xbd07b = { 0xbd07b };
static const nnResult cnnResult_0xbce7b = { 0xbce7b };






static _Bool nssl_results_equal(nnResult lhs, nnResult rhs)
{
  return nnResultGetModule(lhs) == nnResultGetModule(rhs) &&
         nnResultGetDescription(lhs) == nnResultGetDescription(rhs);
}

static _Bool nssl_is_context_valid(struct ssl_connect_data *connssl)
{
  nnsslSslContextId contextId = 0;

  if(!connssl->pnnssl_context)
    return false;
  {
    nnResult result = nnsslContextGetContextId(connssl->pnnssl_context, &contextId);
    if(nnResultIsFailure(result))
      return false;
  }



  return contextId != 0;
}





static _Bool nssl_is_connection_valid(struct ssl_connect_data *connssl)
{
  nnsslSslConnectionId connectionId = 0;
  nnResult result = nnsslConnectionGetConnectionId(&connssl->nnssl_connection, &connectionId);
  if(nnResultIsFailure(result))
    return false;




  return connectionId != 0;
}





static void nnssl_destroy_ssl_context(struct ssl_connect_data *connssl)
{
  nnResult result;

  if(nssl_is_context_valid(connssl)) {




    result = nnsslContextDestroy(connssl->pnnssl_context);
    nnResultIsFailure(result);
  }
}



static void nnssl_destroy_ssl_connection(struct ssl_connect_data *connssl)
{
  nnResult result;

  if(nssl_is_connection_valid(connssl)) {




    result = nnsslConnectionDestroy(&connssl->nnssl_connection);
    nnResultIsFailure(result);




    connssl->state = ssl_connection_none;
  }
}
static CURLcode nnssl_get_verify_cert_error(struct connectdata *conn, int sockindex)
{
  nnResult result;
  nnResult verifyErrorResult;
  CURLcode curlResult = CURLE_PEER_FAILED_VERIFICATION;
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];



  result = nnsslConnectionGetVerifyCertError(&connssl->nnssl_connection, &verifyErrorResult);
  if(nnResultIsFailure(result)) {

    return curlResult;
  }




  conn->data->set.ssl.certverifyresult = verifyErrorResult._value;

  if(nssl_results_equal(verifyErrorResult, cnnResult_0x25a7b) ||
     nssl_results_equal(verifyErrorResult, cnnResult_0x2607b) ||
     nssl_results_equal(verifyErrorResult, cnnResult_0x25e7b)) {

    curlResult = CURLE_PEER_FAILED_VERIFICATION;
  }


  else if(nssl_results_equal(verifyErrorResult, cnnResult_0x2867b)) {

    curlResult = CURLE_SSL_CERTPROBLEM;
  }


  else if(nssl_results_equal(verifyErrorResult, cnnResult_0x2687b)) {

    curlResult = CURLE_SSL_CACERT;
  }


  else if(nssl_results_equal(verifyErrorResult, cnnResult_0xbca7b) ||
          nssl_results_equal(verifyErrorResult, cnnResult_0xbd07b) ||
          nssl_results_equal(verifyErrorResult, cnnResult_0xbce7b)) {

    curlResult = CURLE_SSL_CERTPROBLEM;
  }
  return curlResult;
}




static CURLcode nnssl_fail_connect(
  struct connectdata *conn,
  int sockindex)
{
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];

  nnssl_destroy_ssl_connection(connssl);
  if(!connssl->using_external_ssl_context)

    nnssl_destroy_ssl_context(connssl);
  /* Inference: the return value is never used (the caller returns its own result). */
  return CURLE_OK;
}


static CURLcode nnssl_set_nonblock_io(struct connectdata *conn, int sockindex)
{
  nnResult result;
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];

  result = nnsslConnectionSetIoMode(&connssl->nnssl_connection,
                                    nnsslConnectionIoMode_NonBlocking);

  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }
  return CURLE_OK;
}


static CURLcode nnssl_setup_connect(struct connectdata *conn, int sockindex)
{
  nnResult result;
  CURLcode curlResult;
  nnsslConnectionVerifyOption verifyOption = nnsslConnectionVerifyOption_None;
  nnsslConnectionSessionCacheMode cacheMode = nnsslConnectionSessionCacheMode_None;
  struct SessionHandle *data = conn->data;
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];
  nnsslSslContextId contextId = 0;

  if(data->set.ssl.psslctx) {

    connssl->pnnssl_context = data->set.ssl.psslctx;
    connssl->using_external_ssl_context = true;
  }
  else {

    connssl->pnnssl_context = &connssl->nnssl_context;
    connssl->using_external_ssl_context = false;




    result = nnsslContextGetContextId(connssl->pnnssl_context, &contextId);
    if(nnResultIsFailure(result)) {

      return CURLE_SSL_CTX_FATAL;
    }



    if(contextId != 0) {

      result = nnsslContextDestroy(connssl->pnnssl_context);
      if(nnResultIsFailure(result)) {

        return CURLE_SSL_CTX_FATAL;
      }
    }



    if(!data->set.ssl.fsslctx && !data->set.ssl.psslctx) {

      return CURLE_SSL_CTX_FUNCTION_NOT_SET;
    }



    curlResult = data->set.ssl.fsslctx(conn->data, connssl->pnnssl_context, conn->data->set.ssl.fsslctxp);
    if(curlResult != CURLE_OK) {

      return CURLE_ABORTED_BY_CALLBACK;
    }
  }



  result = nnsslContextGetContextId(connssl->pnnssl_context, &contextId);
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CTX_FATAL;
  }

  if(!contextId) {

    return CURLE_SSL_CTX_INVALID;
  }



  result = nnsslConnectionCreate(&connssl->nnssl_connection, connssl->pnnssl_context);
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }


  nnsocketFcntl(conn->sock[sockindex], F_SETFL, 0);


  result = nnsslConnectionSetOption(&connssl->nnssl_connection, nnsslConnectionOptionType_DoNotCloseSocket, true);
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }


  result = nnsslConnectionSetSocketDescriptor(&connssl->nnssl_connection, conn->sock[sockindex]);
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }


  result = nnsslConnectionSetHostName(&connssl->nnssl_connection, conn->host.dispname, strlen(conn->host.dispname));
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }




  result = nnsslConnectionSetOption(&connssl->nnssl_connection, nnsslConnectionOptionType_SkipDefaultVerify, conn->data->set.ssl.skipssldefaultverify);
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }


  if(conn->data->set.ssl.verifypeer) {

    verifyOption = nnsslConnectionVerifyOption_PeerCa;
  }

  if(conn->data->set.ssl.verifyhost) {


    verifyOption |= nnsslConnectionVerifyOption_HostName;
  }
  if(conn->data->set.ssl.verifydate) {


    verifyOption |= nnsslConnectionVerifyOption_DateCheck;
  }
  result = nnsslConnectionSetVerifyOption(&connssl->nnssl_connection, verifyOption);
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }

  if(conn->data->set.ssl.sessionid) {

    cacheMode = nnsslConnectionSessionCacheMode_SessionId;
  }


  result = nnsslConnectionSetSessionCacheMode(&connssl->nnssl_connection, cacheMode);
  if(nnResultIsFailure(result)) {

    return CURLE_SSL_CONNECT_ERROR;
  }

  if(conn->data->set.ssl.fsslconnbefore) {

    curlResult = conn->data->set.ssl.fsslconnbefore(conn->data, &connssl->nnssl_connection, conn->data->set.ssl.psslcondata);
    if(curlResult != CURLE_OK) {

      return CURLE_ABORTED_BY_CALLBACK;
    }
  }
  return CURLE_OK;
}



static CURLcode nnssl_do_connect(struct connectdata *conn, int sockindex)
{
  nnResult result;
  CURLcode returnVal = CURLE_OK;
  struct SessionHandle *data = conn->data;
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];


  const long time_left = Curl_timeleft(data, NULL, true);
  if(time_left < 0) {

    return CURLE_OPERATION_TIMEDOUT;
  }


  data->set.ssl.dohandshakeresult = 0;
  data->set.ssl.certverifyresult = 0;

  curl_handshake_info handshakeInfo;
  memset(&handshakeInfo, 0, sizeof(handshakeInfo));

  uint32_t certDataSize = 0;
  uint32_t certCount = 0;


  result = nnsslConnectionDoHandshakeWithCertBuffer(&connssl->nnssl_connection, &certDataSize, &certCount);
  if(nnResultIsFailure(result)) {



    if(nssl_results_equal(result, cnnResult_0x1987b)) {

      return CURLE_AGAIN;
    }





    data->set.ssl.dohandshakeresult = result._value;

    if(nssl_results_equal(result, cnnResult_0x19e7b)) {




      returnVal = CURLE_PEER_FAILED_VERIFICATION;
    }
    else {

      returnVal = CURLE_SSL_CONNECT_ERROR;
    }
  }




  if(data->set.ssl.fsslconnafter) {

    handshakeInfo.certDataSize = certDataSize;
    handshakeInfo.certCount = certCount;
    handshakeInfo.isHandshakeSuccess = (returnVal == CURLE_OK);
    CURLcode curlResult = data->set.ssl.fsslconnafter(conn->data, &connssl->nnssl_connection, conn->data->set.ssl.psslcondata, handshakeInfo);
    if(curlResult != CURLE_OK) {

      returnVal = CURLE_ABORTED_BY_CALLBACK;
    }
  }
  return returnVal;
}



static CURLcode nnssl_connect_common(struct connectdata *conn, int sockindex, _Bool *pIsDone)
{
  CURLcode curlResult = CURLE_OK;
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];









  if(connssl->state == ssl_connection_complete) {

    *pIsDone = true;
    return CURLE_OK;
  }



  if(connssl->connecting_state == ssl_connect_1) {

    curlResult = nnssl_setup_connect(conn, sockindex);
    if(curlResult == CURLE_OK)





      curlResult = nnssl_set_nonblock_io(conn, sockindex);
    if(curlResult == CURLE_OK)




      connssl->connecting_state = ssl_connect_2;
  }
  if(curlResult == CURLE_OK) {
    curlResult = nnssl_do_connect(conn, sockindex);
    if(curlResult == CURLE_OK) {







      *pIsDone = true;
      conn->recv[sockindex] = nnssl_recv;
      conn->send[sockindex] = nnssl_send;
      connssl->state = ssl_connection_complete;

      connssl->connecting_state = ssl_connect_1;
      return CURLE_OK;
    }
  }


  if(curlResult == CURLE_AGAIN) {

    return CURLE_OK;
  }




  if(curlResult == CURLE_PEER_FAILED_VERIFICATION)
    curlResult = nnssl_get_verify_cert_error(conn, sockindex);


  nnssl_fail_connect(conn, sockindex);

  return curlResult;
}
static ssize_t nnssl_recv(struct connectdata *conn,
                          int sockindex,
                          char *buf,
                          size_t buffersize,
                          CURLcode *curlcode)
{

  int bytesRead = 0;
  nnResult result;
  result = nnsslConnectionRead(
    &conn->ssl[sockindex].nnssl_connection, buf, &bytesRead, buffersize);



  if(nnResultIsFailure(result)) {

    *curlcode = nssl_results_equal(result, cnnResult_0x1987b) ? CURLE_AGAIN : CURLE_RECV_ERROR;
    return -1;
  }








  return bytesRead;
}





int Curl_nnssl_init(void)
{
  nnResult result = nnsslInitialize();
  if(nnResultIsFailure(result) &&
     !nssl_results_equal(result, cnnResult_0x1927b))
    return 0;
  return 1;
}







void Curl_nnssl_cleanup(void)
{
  nnsslFinalize();
}
/* LINKER-REMOVED: DWARF line 596, size 0x14, no locals or inline calls. */
CURLcode Curl_nnssl_connect(struct connectdata *conn, int sockindex)
{
  (void)sockindex;
  if(!conn)
    return CURLE_SSL_INVALID_REFERENCE;
  return CURLE_NOT_BUILT_IN; /* inferred: the null test and both codes */
}



CURLcode Curl_nnssl_connect_nonblocking(struct connectdata *conn, int sockindex, _Bool *done)
{
  if(!conn || !done)
    return CURLE_SSL_INVALID_REFERENCE;



  return nnssl_connect_common(conn, sockindex, done);
}


void Curl_nnssl_close(struct connectdata *conn, int sockindex)
{
  if(conn) {




    nnssl_destroy_ssl_connection(&conn->ssl[sockindex]);
    if(!conn->ssl[sockindex].using_external_ssl_context)

      nnssl_destroy_ssl_context(&conn->ssl[sockindex]);
  }
}







int Curl_nnssl_check_cxn(struct connectdata *conn)
{
  char buf;
  int bytesRead = 0;
  nnResult result;

  if(!conn)
    return CURLE_SSL_INVALID_REFERENCE;




  result = nnsslConnectionPeek(
    &conn->ssl[0].nnssl_connection, &buf, &bytesRead, 1);



  if(nnResultIsFailure(result)) {

    return nssl_results_equal(result, cnnResult_0x1987b)
      ? 1
      : -1;
  }





  if(bytesRead > 0)
    return 1;




  return bytesRead == 0 ? 0 : -1;
}





static ssize_t nnssl_send(struct connectdata *conn,
                          int sockindex,
                          const void *buf,
                          size_t buffersize,
                          CURLcode *curlcode)
{

  int bytesWritten = 0;
  nnResult result;

  if(!conn || !buf || !curlcode)
    return CURLE_SSL_INVALID_REFERENCE;




  result = nnsslConnectionWrite(
    &conn->ssl[sockindex].nnssl_connection, buf, &bytesWritten, buffersize);



  if(nnResultIsFailure(result)) {

    *curlcode = nssl_results_equal(result, cnnResult_0x1987b) ? CURLE_AGAIN : CURLE_WRITE_ERROR;
    return -1;
  }









  return bytesWritten;
}

size_t Curl_nnssl_version(char *buffer, size_t size)
{
  if(!buffer)
    return 0;



  return snprintf(buffer, size, "nn::ssl");
}















int Curl_nnssl_random(struct SessionHandle *data, unsigned char *entropy, size_t length)
{
  unsigned int i;


  /* Inference: operand order of the condition is not recoverable; clang emits the original's checks
     only for this spelling. */
  if(data && length && entropy) {




    for(i = 0; i < length; i++)
      entropy[i] = 0;
  }



  return 0;
}
_Bool Curl_nnssl_cert_status_request(void)
{



  return false;
}


_Bool Curl_nnssl_false_start(void)
{



  return false;
}


_Bool Curl_nnssl_data_pending(const struct connectdata *conn, int connindex)
{
  int pendingSizeCourier = 0;

  nnResult result = nnsslConnectionPending(&conn->ssl[connindex].nnssl_connection, &pendingSizeCourier);

  return nnResultIsSuccess(result) && pendingSizeCourier > 0;
}
