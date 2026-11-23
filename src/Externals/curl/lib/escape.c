/*
 * COPYRIGHT AND PERMISSION NOTICE
 *
 * Copyright (c) 1996 - 2016, Daniel Stenberg, <daniel@haxx.se>, and many
 * contributors, see the THANKS file.
 *
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall not
 * be used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization of the copyright holder.
 */

/* Reconstructed from original instructions and DWARF; only retained functions.
 * Local declarations preserve the original Nintendo CURLcode values. */
#include <stddef.h>
#include <stdbool.h>

typedef void CURL;
struct SessionHandle;

typedef enum {
  CURLE_OK = 0, CURLE_UNSUPPORTED_PROTOCOL = 1,
  CURLE_FAILED_INIT = 2, CURLE_URL_MALFORMAT = 3,
  CURLE_NOT_BUILT_IN = 4, CURLE_COULDNT_RESOLVE_PROXY = 5,
  CURLE_COULDNT_RESOLVE_HOST = 6, CURLE_COULDNT_CONNECT = 7,
  CURLE_FTP_WEIRD_SERVER_REPLY = 8, CURLE_REMOTE_ACCESS_DENIED = 9,
  CURLE_FTP_ACCEPT_FAILED = 10, CURLE_FTP_WEIRD_PASS_REPLY = 11,
  CURLE_FTP_ACCEPT_TIMEOUT = 12, CURLE_FTP_WEIRD_PASV_REPLY = 13,
  CURLE_FTP_WEIRD_227_FORMAT = 14, CURLE_FTP_CANT_GET_HOST = 15,
  CURLE_HTTP2 = 16, CURLE_FTP_COULDNT_SET_TYPE = 17,
  CURLE_PARTIAL_FILE = 18, CURLE_FTP_COULDNT_RETR_FILE = 19,
  CURLE_OBSOLETE20 = 20, CURLE_QUOTE_ERROR = 21,
  CURLE_HTTP_RETURNED_ERROR = 22, CURLE_WRITE_ERROR = 23,
  CURLE_OBSOLETE24 = 24, CURLE_UPLOAD_FAILED = 25,
  CURLE_READ_ERROR = 26, CURLE_OUT_OF_MEMORY = 27,
  CURLE_OPERATION_TIMEDOUT = 28, CURLE_OBSOLETE29 = 29,
  CURLE_FTP_PORT_FAILED = 30, CURLE_FTP_COULDNT_USE_REST = 31,
  CURLE_OBSOLETE32 = 32, CURLE_RANGE_ERROR = 33,
  CURLE_HTTP_POST_ERROR = 34, CURLE_SSL_CONNECT_ERROR = 35,
  CURLE_BAD_DOWNLOAD_RESUME = 36, CURLE_FILE_COULDNT_READ_FILE = 37,
  CURLE_LDAP_CANNOT_BIND = 38, CURLE_LDAP_SEARCH_FAILED = 39,
  CURLE_OBSOLETE40 = 40, CURLE_FUNCTION_NOT_FOUND = 41,
  CURLE_ABORTED_BY_CALLBACK = 42, CURLE_BAD_FUNCTION_ARGUMENT = 43,
  CURLE_OBSOLETE44 = 44, CURLE_INTERFACE_FAILED = 45,
  CURLE_OBSOLETE46 = 46, CURLE_TOO_MANY_REDIRECTS = 47,
  CURLE_UNKNOWN_OPTION = 48, CURLE_TELNET_OPTION_SYNTAX = 49,
  CURLE_OBSOLETE50 = 50, CURLE_PEER_FAILED_VERIFICATION = 51,
  CURLE_GOT_NOTHING = 52, CURLE_SSL_ENGINE_NOTFOUND = 53,
  CURLE_SSL_ENGINE_SETFAILED = 54, CURLE_SEND_ERROR = 55,
  CURLE_RECV_ERROR = 56, CURLE_OBSOLETE57 = 57,
  CURLE_SSL_CERTPROBLEM = 58, CURLE_SSL_CIPHER = 59,
  CURLE_SSL_CACERT = 60, CURLE_BAD_CONTENT_ENCODING = 61,
  CURLE_LDAP_INVALID_URL = 62, CURLE_FILESIZE_EXCEEDED = 63,
  CURLE_USE_SSL_FAILED = 64, CURLE_SEND_FAIL_REWIND = 65,
  CURLE_SSL_ENGINE_INITFAILED = 66, CURLE_LOGIN_DENIED = 67,
  CURLE_TFTP_NOTFOUND = 68, CURLE_TFTP_PERM = 69,
  CURLE_REMOTE_DISK_FULL = 70, CURLE_TFTP_ILLEGAL = 71,
  CURLE_TFTP_UNKNOWNID = 72, CURLE_REMOTE_FILE_EXISTS = 73,
  CURLE_TFTP_NOSUCHUSER = 74, CURLE_CONV_FAILED = 75,
  CURLE_CONV_REQD = 76, CURLE_SSL_CACERT_BADFILE = 77,
  CURLE_REMOTE_FILE_NOT_FOUND = 78, CURLE_SSH = 79,
  CURLE_SSL_SHUTDOWN_FAILED = 80, CURLE_AGAIN = 81,
  CURLE_SSL_CRL_BADFILE = 82, CURLE_SSL_ISSUER_ERROR = 83,
  CURLE_FTP_PRET_FAILED = 84, CURLE_RTSP_CSEQ_ERROR = 85,
  CURLE_RTSP_SESSION_ERROR = 86, CURLE_FTP_BAD_FILE_LIST = 87,
  CURLE_CHUNK_FAILED = 88, CURLE_NO_CONNECTION_AVAILABLE = 89,
  CURLE_SSL_PINNEDPUBKEYNOTMATCH = 90, CURLE_SSL_INVALIDCERTSTATUS = 91,
  CURLE_SSL_CTX_FUNCTION_NOT_SET = 92, CURLE_SSL_CTX_INVALID = 93,
  CURLE_SSL_CTX_FATAL = 94, CURLE_SSL_ALREADY_CONNECTED = 95,
  CURLE_SSL_INVALID_REFERENCE = 96, CURL_LAST = 97
} CURLcode;

extern void *Curl_SigloMalloc(size_t sizeToAllocate);
extern void Curl_SigloFree(void *pointer);
extern unsigned char curlx_ultouc(unsigned long ulnum);
extern int curlx_uztosi(size_t uznum);

/* Local externs retain the original libc calls, without host ctype macros. */
extern size_t strlen(const char *);
extern int isxdigit(int);
extern unsigned long strtoul(const char *, char **, int);
















































CURLcode Curl_urldecode(struct SessionHandle *data,
                       const char *string, size_t length,
                       char **ostring, size_t *olen,
                       _Bool reject_ctrl)
{
  size_t alloc = (length ? length : strlen(string)) + 1;
  char *ns = Curl_SigloMalloc(alloc);
  unsigned char in;
  size_t strindex = 0;
  unsigned long hex;
  CURLcode result = CURLE_OK;

  if(!ns)
    return CURLE_OUT_OF_MEMORY;

  while(--alloc > 0) {
    in = (unsigned char)*string;
    if(('%' == in) && (alloc > 2) &&
       isxdigit((unsigned char)string[1]) && isxdigit((unsigned char)string[2])) {

      char hexstr[3];
      char *ptr;
      hexstr[0] = string[1];
      hexstr[1] = string[2];
      hexstr[2] = 0;

      hex = strtoul(hexstr, &ptr, 16);

      in = curlx_ultouc(hex);








      string += 2;
      alloc -= 2;
    }

    if(reject_ctrl && (in < 0x20)) {
      Curl_SigloFree(ns);
      return CURLE_URL_MALFORMAT;
    }

    ns[strindex++] = in;
    string++;
  }
  ns[strindex] = 0;

  if(olen) {

    *olen = strindex;
  }

  *ostring = ns;

  return result;
}







char *curl_easy_unescape(CURL *handle, const char *string, int length,
                         int *olen)
{
  char *str = NULL;
  size_t inputlen = (size_t)length;
  size_t outputlen;
  CURLcode res = Curl_urldecode(handle, string, inputlen, &str, &outputlen, false);

  if(res)
    return NULL;
  if(olen)
    *olen = curlx_uztosi(outputlen);
  return str;
}
