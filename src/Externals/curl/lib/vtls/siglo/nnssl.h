#ifndef HEADER_CURL_NNSSL_H
#define HEADER_CURL_NNSSL_H
/*
 * Nintendo's nn::ssl backend for libcurl's vtls layer (siglo/nnssl.h).
 * No public source; the prototypes are the DWARF's, the curlssl_* mapping
 * is what vtls.c compiles to in the binary.
 */

#include <stddef.h>
#include "Externals/curl/lib/urldata.h"

int Curl_nnssl_init(void);
void Curl_nnssl_cleanup(void);
CURLcode Curl_nnssl_connect_nonblocking(struct connectdata *conn,
                                        int sockindex, _Bool *done);
void Curl_nnssl_close(struct connectdata *conn, int sockindex);
int Curl_nnssl_check_cxn(struct connectdata *conn);
size_t Curl_nnssl_version(char *buffer, size_t size);
int Curl_nnssl_random(struct SessionHandle *data, unsigned char *entropy,
                      size_t length);
_Bool Curl_nnssl_cert_status_request(void);
_Bool Curl_nnssl_false_start(void);
_Bool Curl_nnssl_data_pending(const struct connectdata *conn,
                              int connindex);
/* Linker-removed with its only caller, Curl_ssl_connect (both have DWARF). */
CURLcode Curl_nnssl_connect(struct connectdata *conn, int sockindex);

/* Set the API backend definition to nn::ssl (no curl_sslbackend value of
   its own; CURLSSLBACKEND_NONE is used) */
#define CURL_SSL_BACKEND CURLSSLBACKEND_NONE

/* API setup for nn::ssl */
#define curlssl_init Curl_nnssl_init
#define curlssl_cleanup Curl_nnssl_cleanup
#define curlssl_connect Curl_nnssl_connect
/* No Curl_nnssl_shutdown exists: no unit has DWARF for one, and the original Curl_ssl_shutdown
   (linker-removed) is 8 bytes, a constant return, so the shutdown test is a nonzero constant.
   Inferred: the value 1. */
#define curlssl_shutdown(x,y) ((void)(x), (void)(y), 1)
#define curlssl_connect_nonblocking Curl_nnssl_connect_nonblocking
#define curlssl_session_free(x) ((void)(x))
#define curlssl_close_all(x) ((void)(x))
#define curlssl_close Curl_nnssl_close
#define curlssl_set_engine(x,y) ((void)(x), (void)(y), CURLE_NOT_BUILT_IN)
#define curlssl_set_engine_default(x) ((void)(x), CURLE_NOT_BUILT_IN)
#define curlssl_engines_list(x) ((void)(x), (struct curl_slist *)NULL)
#define curlssl_version Curl_nnssl_version
#define curlssl_check_cxn Curl_nnssl_check_cxn
#define curlssl_data_pending Curl_nnssl_data_pending
#define curlssl_random(x,y,z) Curl_nnssl_random(x,y,z)
#define curlssl_cert_status_request() Curl_nnssl_cert_status_request()
#define curlssl_false_start() Curl_nnssl_false_start()

#endif /* HEADER_CURL_NNSSL_H */
