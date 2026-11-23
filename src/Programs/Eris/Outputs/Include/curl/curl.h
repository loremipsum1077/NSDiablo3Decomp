/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/


/* Original DWARF declarations consumed by bundles.c, CU DIE 0x07c8ece6.
 * Complete definitions for every by-value type; pointer-only types are forward
 * declarations. No synthetic padding, hidden implementation body or API stub.
 */
#ifndef D3_RECOVERED_PROGRAMS_ERIS_OUTPUTS_INCLUDE_CURL_CURL_H
#define D3_RECOVERED_PROGRAMS_ERIS_OUTPUTS_INCLUDE_CURL_CURL_H

#include <stddef.h>
#include <stdint.h>
#include "Programs/Eris/Outputs/Include/curl/curlbuild.h"


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:104; DIE 0x7c8f997 */
typedef void CURL;


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:130; DIE 0x7c8f9ab */
typedef int curl_socket_t;


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:308; DIE 0x7c908b9 */
typedef int (*curl_seek_callback)(void *, curl_off_t, int);


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:319; DIE 0x7c90910 */
typedef size_t (*curl_read_callback)(char *, size_t, size_t, void *);


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:553; DIE 0x7c8f905 */
typedef enum {
  CURLE_OK = 0,
  CURLE_UNSUPPORTED_PROTOCOL = 1,
  CURLE_FAILED_INIT = 2,
  CURLE_URL_MALFORMAT = 3,
  CURLE_NOT_BUILT_IN = 4,
  CURLE_COULDNT_RESOLVE_PROXY = 5,
  CURLE_COULDNT_RESOLVE_HOST = 6,
  CURLE_COULDNT_CONNECT = 7,
  CURLE_FTP_WEIRD_SERVER_REPLY = 8,
  CURLE_REMOTE_ACCESS_DENIED = 9,
  CURLE_FTP_ACCEPT_FAILED = 10,
  CURLE_FTP_WEIRD_PASS_REPLY = 11,
  CURLE_FTP_ACCEPT_TIMEOUT = 12,
  CURLE_FTP_WEIRD_PASV_REPLY = 13,
  CURLE_FTP_WEIRD_227_FORMAT = 14,
  CURLE_FTP_CANT_GET_HOST = 15,
  CURLE_HTTP2 = 16,
  CURLE_FTP_COULDNT_SET_TYPE = 17,
  CURLE_PARTIAL_FILE = 18,
  CURLE_FTP_COULDNT_RETR_FILE = 19,
  CURLE_OBSOLETE20 = 20,
  CURLE_QUOTE_ERROR = 21,
  CURLE_HTTP_RETURNED_ERROR = 22,
  CURLE_WRITE_ERROR = 23,
  CURLE_OBSOLETE24 = 24,
  CURLE_UPLOAD_FAILED = 25,
  CURLE_READ_ERROR = 26,
  CURLE_OUT_OF_MEMORY = 27,
  CURLE_OPERATION_TIMEDOUT = 28,
  CURLE_OBSOLETE29 = 29,
  CURLE_FTP_PORT_FAILED = 30,
  CURLE_FTP_COULDNT_USE_REST = 31,
  CURLE_OBSOLETE32 = 32,
  CURLE_RANGE_ERROR = 33,
  CURLE_HTTP_POST_ERROR = 34,
  CURLE_SSL_CONNECT_ERROR = 35,
  CURLE_BAD_DOWNLOAD_RESUME = 36,
  CURLE_FILE_COULDNT_READ_FILE = 37,
  CURLE_LDAP_CANNOT_BIND = 38,
  CURLE_LDAP_SEARCH_FAILED = 39,
  CURLE_OBSOLETE40 = 40,
  CURLE_FUNCTION_NOT_FOUND = 41,
  CURLE_ABORTED_BY_CALLBACK = 42,
  CURLE_BAD_FUNCTION_ARGUMENT = 43,
  CURLE_OBSOLETE44 = 44,
  CURLE_INTERFACE_FAILED = 45,
  CURLE_OBSOLETE46 = 46,
  CURLE_TOO_MANY_REDIRECTS = 47,
  CURLE_UNKNOWN_OPTION = 48,
  CURLE_TELNET_OPTION_SYNTAX = 49,
  CURLE_OBSOLETE50 = 50,
  CURLE_PEER_FAILED_VERIFICATION = 51,
  CURLE_GOT_NOTHING = 52,
  CURLE_SSL_ENGINE_NOTFOUND = 53,
  CURLE_SSL_ENGINE_SETFAILED = 54,
  CURLE_SEND_ERROR = 55,
  CURLE_RECV_ERROR = 56,
  CURLE_OBSOLETE57 = 57,
  CURLE_SSL_CERTPROBLEM = 58,
  CURLE_SSL_CIPHER = 59,
  CURLE_SSL_CACERT = 60,
  CURLE_BAD_CONTENT_ENCODING = 61,
  CURLE_LDAP_INVALID_URL = 62,
  CURLE_FILESIZE_EXCEEDED = 63,
  CURLE_USE_SSL_FAILED = 64,
  CURLE_SEND_FAIL_REWIND = 65,
  CURLE_SSL_ENGINE_INITFAILED = 66,
  CURLE_LOGIN_DENIED = 67,
  CURLE_TFTP_NOTFOUND = 68,
  CURLE_TFTP_PERM = 69,
  CURLE_REMOTE_DISK_FULL = 70,
  CURLE_TFTP_ILLEGAL = 71,
  CURLE_TFTP_UNKNOWNID = 72,
  CURLE_REMOTE_FILE_EXISTS = 73,
  CURLE_TFTP_NOSUCHUSER = 74,
  CURLE_CONV_FAILED = 75,
  CURLE_CONV_REQD = 76,
  CURLE_SSL_CACERT_BADFILE = 77,
  CURLE_REMOTE_FILE_NOT_FOUND = 78,
  CURLE_SSH = 79,
  CURLE_SSL_SHUTDOWN_FAILED = 80,
  CURLE_AGAIN = 81,
  CURLE_SSL_CRL_BADFILE = 82,
  CURLE_SSL_ISSUER_ERROR = 83,
  CURLE_FTP_PRET_FAILED = 84,
  CURLE_RTSP_CSEQ_ERROR = 85,
  CURLE_RTSP_SESSION_ERROR = 86,
  CURLE_FTP_BAD_FILE_LIST = 87,
  CURLE_CHUNK_FAILED = 88,
  CURLE_NO_CONNECTION_AVAILABLE = 89,
  CURLE_SSL_PINNEDPUBKEYNOTMATCH = 90,
  CURLE_SSL_INVALIDCERTSTATUS = 91,
  CURLE_SSL_CTX_FUNCTION_NOT_SET = 92,
  CURLE_SSL_CTX_INVALID = 93,
  CURLE_SSL_CTX_FATAL = 94,
  CURLE_SSL_ALREADY_CONNECTED = 95,
  CURLE_SSL_INVALID_REFERENCE = 96,
  CURL_LAST = 97
} CURLcode;

_Static_assert(sizeof(CURLcode) == 4, "CURLcode size");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:642; DIE 0x7c90e8b */
typedef struct {
  _Bool isHandshakeSuccess; /* 0x0 */
  size_t certDataSize; /* 0x8 */
  size_t certCount; /* 0x10 */
} curl_handshake_info;

_Static_assert(sizeof(curl_handshake_info) == 24, "curl_handshake_info size");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:669; DIE 0x7c90eda */
typedef enum {
  CURLPROXY_HTTP = 0,
  CURLPROXY_HTTP_1_0 = 1,
  CURLPROXY_SOCKS4 = 4,
  CURLPROXY_SOCKS5 = 5,
  CURLPROXY_SOCKS4A = 6,
  CURLPROXY_SOCKS5_HOSTNAME = 7
} curl_proxytype;

_Static_assert(sizeof(curl_proxytype) == 4, "curl_proxytype size");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:357; DIE 0x7c90b64 */
typedef int (*curl_closesocket_callback)(void *, curl_socket_t);


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:630; DIE 0x7c90ece */
typedef CURLcode (*curl_ssl_ctx_callback)(CURL *, void *, void *);


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:645; DIE 0x7c90e3a */
typedef CURLcode (*curl_ssl_conn_callback_before)(CURL *, void *, void *);


/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:650; DIE 0x7c90e60 */
typedef CURLcode (*curl_ssl_conn_callback_after)(CURL *, void *, void *, curl_handshake_info);


/* Added for conncache.c (struct SessionHandle closure). */
struct curl_khkey;
struct curl_slist;
struct curl_sockaddr;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:207 */
typedef size_t (*curl_write_callback)(char *, size_t, size_t, void *);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:172 */
typedef int (*curl_progress_callback)(void *, double, double, double, double);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:180 */
typedef int (*curl_xferinfo_callback)(void *, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:398 */
typedef enum {
  CURLINFO_TEXT = 0,
  CURLINFO_HEADER_IN = 1,
  CURLINFO_HEADER_OUT = 2,
  CURLINFO_DATA_IN = 3,
  CURLINFO_DATA_OUT = 4,
  CURLINFO_SSL_DATA_IN = 5,
  CURLINFO_SSL_DATA_OUT = 6,
  CURLINFO_END = 7
} curl_infotype;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:400 */
typedef int (*curl_debug_callback)(CURL *, curl_infotype, char *, size_t, void *);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:364 */
typedef enum {
  CURLIOE_OK = 0,
  CURLIOE_UNKNOWNCMD = 1,
  CURLIOE_FAILRESTART = 2,
  CURLIOE_LAST = 3
} curlioerr;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:372 */
typedef curlioerr (*curl_ioctl_callback)(CURL *, int, void *);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:328 */
typedef enum {
  CURLSOCKTYPE_IPCXN = 0,
  CURLSOCKTYPE_ACCEPT = 1,
  CURLSOCKTYPE_LAST = 2
} curlsocktype;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:337 */
typedef int (*curl_sockopt_callback)(void *, curl_socket_t, curlsocktype);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:352 */
typedef curl_socket_t (*curl_opensocket_callback)(void *, curlsocktype, struct curl_sockaddr *);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:628 */
typedef CURLcode (*curl_conv_callback)(char *, size_t);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:1840 */
typedef enum {
  CURL_TIMECOND_NONE = 0,
  CURL_TIMECOND_IFMODSINCE = 1,
  CURL_TIMECOND_IFUNMODSINCE = 2,
  CURL_TIMECOND_LASTMOD = 3,
  CURL_TIMECOND_LAST = 4
} curl_TimeCond;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:743 */
enum curl_khmatch {
  CURLKHMATCH_OK = 0,
  CURLKHMATCH_MISMATCH = 1,
  CURLKHMATCH_MISSING = 2,
  CURLKHMATCH_LAST = 3
};

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:751 */
typedef int (*curl_sshkeycallback)(CURL *, const struct curl_khkey *, const struct curl_khkey *, enum curl_khmatch, void *);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:1791 */
enum CURL_NETRC_OPTION {
  CURL_NETRC_IGNORED = 0,
  CURL_NETRC_OPTIONAL = 1,
  CURL_NETRC_REQUIRED = 2,
  CURL_NETRC_LAST = 3
};

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:764 */
typedef enum {
  CURLUSESSL_NONE = 0,
  CURLUSESSL_TRY = 1,
  CURLUSESSL_CONTROL = 2,
  CURLUSESSL_ALL = 3,
  CURLUSESSL_LAST = 4
} curl_usessl;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:803 */
typedef enum {
  CURLFTPAUTH_DEFAULT = 0,
  CURLFTPAUTH_SSL = 1,
  CURLFTPAUTH_TLS = 2,
  CURLFTPAUTH_LAST = 3
} curl_ftpauth;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:795 */
typedef enum {
  CURLFTPSSL_CCC_NONE = 0,
  CURLFTPSSL_CCC_PASSIVE = 1,
  CURLFTPSSL_CCC_ACTIVE = 2,
  CURLFTPSSL_CCC_LAST = 3
} curl_ftpccc;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:276 */
typedef long int (*curl_chunk_bgn_callback)(const void *, void *, int);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:290 */
typedef long int (*curl_chunk_end_callback)(void *);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:299 */
typedef int (*curl_fnmatch_callback)(void *, const char *, const char *);

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:2121 */
struct curl_certinfo {
  int num_of_certs; /* 0x0 */
  struct curl_slist **certinfo; /* 0x8 */
};

_Static_assert(sizeof(struct curl_certinfo) == 16, "curl_certinfo size");
_Static_assert(offsetof(struct curl_certinfo, num_of_certs) == 0, "curl_certinfo.num_of_certs");
_Static_assert(offsetof(struct curl_certinfo, certinfo) == 8, "curl_certinfo.certinfo");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:2142 */
typedef enum {
  CURLSSLBACKEND_NONE = 0,
  CURLSSLBACKEND_OPENSSL = 1,
  CURLSSLBACKEND_GNUTLS = 2,
  CURLSSLBACKEND_NSS = 3,
  CURLSSLBACKEND_OBSOLETE4 = 4,
  CURLSSLBACKEND_GSKIT = 5,
  CURLSSLBACKEND_POLARSSL = 6,
  CURLSSLBACKEND_CYASSL = 7,
  CURLSSLBACKEND_SCHANNEL = 8,
  CURLSSLBACKEND_DARWINSSL = 9,
  CURLSSLBACKEND_AXTLS = 10,
  CURLSSLBACKEND_NNSSL = 11
} curl_sslbackend;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:2147 */
struct curl_tlssessioninfo {
  curl_sslbackend backend; /* 0x0 */
  void *internals; /* 0x8 */
};

_Static_assert(sizeof(struct curl_tlssessioninfo) == 16, "curl_tlssessioninfo size");
_Static_assert(offsetof(struct curl_tlssessioninfo, backend) == 0, "curl_tlssessioninfo.backend");
_Static_assert(offsetof(struct curl_tlssessioninfo, internals) == 8, "curl_tlssessioninfo.internals");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:2244; DIE 0x7c7074f */
typedef enum {
  CURL_LOCK_DATA_NONE = 0,
  CURL_LOCK_DATA_SHARE = 1,
  CURL_LOCK_DATA_COOKIE = 2,
  CURL_LOCK_DATA_DNS = 3,
  CURL_LOCK_DATA_SSL_SESSION = 4,
  CURL_LOCK_DATA_CONNECT = 5,
  CURL_LOCK_DATA_LAST = 6
} curl_lock_data;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:2259; DIE 0x7c7077f */
typedef enum {
  CURL_LOCK_ACCESS_NONE = 0,
  CURL_LOCK_ACCESS_SHARED = 1,
  CURL_LOCK_ACCESS_SINGLE = 2,
  CURL_LOCK_ACCESS_LAST = 3
} curl_lock_access;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/curl.h:2276; DIE 0x7c35e8a */
typedef enum {
  CURLSHE_OK = 0,
  CURLSHE_BAD_OPTION = 1,
  CURLSHE_IN_USE = 2,
  CURLSHE_INVALID = 3,
  CURLSHE_NOMEM = 4,
  CURLSHE_NOT_BUILT_IN = 5,
  CURLSHE_LAST = 6
} CURLSHcode;

/* Added for http.c (curl.h:136, 230, 335, 368, 641-649, 1710). */
struct curl_slist {
  char *data;
  struct curl_slist *next;
};

#define CURL_MAX_WRITE_SIZE 16384
#define CURL_MAX_HTTP_HEADER (100*1024)

#define CURL_SEEKFUNC_OK       0
#define CURL_SEEKFUNC_FAIL     1
#define CURL_SEEKFUNC_CANTSEEK 2

#define CURLAUTH_NONE         ((unsigned long)0)
#define CURLAUTH_BASIC        (((unsigned long)1)<<0)
#define CURLAUTH_DIGEST       (((unsigned long)1)<<1)
#define CURLAUTH_NEGOTIATE    (((unsigned long)1)<<2)
#define CURLAUTH_NTLM         (((unsigned long)1)<<3)
#define CURLAUTH_DIGEST_IE    (((unsigned long)1)<<4)
#define CURLAUTH_NTLM_WB      (((unsigned long)1)<<5)
#define CURLAUTH_ONLY         (((unsigned long)1)<<31)
#define CURLAUTH_ANY          (~CURLAUTH_DIGEST_IE)
#define CURLAUTH_BASIC_ONLY   (CURLAUTH_BASIC|CURLAUTH_ONLY)

#define CURL_HTTP_VERSION_NONE 0
#define CURL_HTTP_VERSION_1_0 1
#define CURL_HTTP_VERSION_1_1 2
#define CURL_HTTP_VERSION_2_0 3

/* Added for url.c (Curl_setopt): curl.h:891-1718, values from the DWARF. */
typedef enum {
  CURLOPT_WRITEDATA = 10001,
  CURLOPT_URL = 10002,
  CURLOPT_PORT = 3,
  CURLOPT_PROXY = 10004,
  CURLOPT_USERPWD = 10005,
  CURLOPT_PROXYUSERPWD = 10006,
  CURLOPT_RANGE = 10007,
  CURLOPT_READDATA = 10009,
  CURLOPT_ERRORBUFFER = 10010,
  CURLOPT_WRITEFUNCTION = 20011,
  CURLOPT_READFUNCTION = 20012,
  CURLOPT_TIMEOUT = 13,
  CURLOPT_INFILESIZE = 14,
  CURLOPT_POSTFIELDS = 10015,
  CURLOPT_REFERER = 10016,
  CURLOPT_FTPPORT = 10017,
  CURLOPT_USERAGENT = 10018,
  CURLOPT_LOW_SPEED_LIMIT = 19,
  CURLOPT_LOW_SPEED_TIME = 20,
  CURLOPT_RESUME_FROM = 21,
  CURLOPT_COOKIE = 10022,
  CURLOPT_HTTPHEADER = 10023,
  CURLOPT_HTTPPOST = 10024,
  CURLOPT_SSLCERT = 10025,
  CURLOPT_KEYPASSWD = 10026,
  CURLOPT_CRLF = 27,
  CURLOPT_QUOTE = 10028,
  CURLOPT_HEADERDATA = 10029,
  CURLOPT_COOKIEFILE = 10031,
  CURLOPT_SSLVERSION = 32,
  CURLOPT_TIMECONDITION = 33,
  CURLOPT_TIMEVALUE = 34,
  CURLOPT_CUSTOMREQUEST = 10036,
  CURLOPT_STDERR = 10037,
  CURLOPT_POSTQUOTE = 10039,
  CURLOPT_OBSOLETE40 = 10040,
  CURLOPT_VERBOSE = 41,
  CURLOPT_HEADER = 42,
  CURLOPT_NOPROGRESS = 43,
  CURLOPT_NOBODY = 44,
  CURLOPT_FAILONERROR = 45,
  CURLOPT_UPLOAD = 46,
  CURLOPT_POST = 47,
  CURLOPT_DIRLISTONLY = 48,
  CURLOPT_APPEND = 50,
  CURLOPT_NETRC = 51,
  CURLOPT_FOLLOWLOCATION = 52,
  CURLOPT_TRANSFERTEXT = 53,
  CURLOPT_PUT = 54,
  CURLOPT_PROGRESSFUNCTION = 20056,
  CURLOPT_PROGRESSDATA = 10057,
  CURLOPT_AUTOREFERER = 58,
  CURLOPT_PROXYPORT = 59,
  CURLOPT_POSTFIELDSIZE = 60,
  CURLOPT_HTTPPROXYTUNNEL = 61,
  CURLOPT_INTERFACE = 10062,
  CURLOPT_KRBLEVEL = 10063,
  CURLOPT_SSL_VERIFYPEER = 64,
  CURLOPT_CAINFO = 10065,
  CURLOPT_MAXREDIRS = 68,
  CURLOPT_FILETIME = 69,
  CURLOPT_TELNETOPTIONS = 10070,
  CURLOPT_MAXCONNECTS = 71,
  CURLOPT_OBSOLETE72 = 72,
  CURLOPT_FRESH_CONNECT = 74,
  CURLOPT_FORBID_REUSE = 75,
  CURLOPT_RANDOM_FILE = 10076,
  CURLOPT_EGDSOCKET = 10077,
  CURLOPT_CONNECTTIMEOUT = 78,
  CURLOPT_HEADERFUNCTION = 20079,
  CURLOPT_HTTPGET = 80,
  CURLOPT_SSL_VERIFYHOST = 81,
  CURLOPT_COOKIEJAR = 10082,
  CURLOPT_SSL_CIPHER_LIST = 10083,
  CURLOPT_HTTP_VERSION = 84,
  CURLOPT_FTP_USE_EPSV = 85,
  CURLOPT_SSLCERTTYPE = 10086,
  CURLOPT_SSLKEY = 10087,
  CURLOPT_SSLKEYTYPE = 10088,
  CURLOPT_SSLENGINE = 10089,
  CURLOPT_SSLENGINE_DEFAULT = 90,
  CURLOPT_DNS_USE_GLOBAL_CACHE = 91,
  CURLOPT_DNS_CACHE_TIMEOUT = 92,
  CURLOPT_PREQUOTE = 10093,
  CURLOPT_DEBUGFUNCTION = 20094,
  CURLOPT_DEBUGDATA = 10095,
  CURLOPT_COOKIESESSION = 96,
  CURLOPT_CAPATH = 10097,
  CURLOPT_BUFFERSIZE = 98,
  CURLOPT_NOSIGNAL = 99,
  CURLOPT_SHARE = 10100,
  CURLOPT_PROXYTYPE = 101,
  CURLOPT_ACCEPT_ENCODING = 10102,
  CURLOPT_PRIVATE = 10103,
  CURLOPT_HTTP200ALIASES = 10104,
  CURLOPT_UNRESTRICTED_AUTH = 105,
  CURLOPT_FTP_USE_EPRT = 106,
  CURLOPT_HTTPAUTH = 107,
  CURLOPT_SSL_CTX_FUNCTION = 20108,
  CURLOPT_SSL_CTX_DATA = 10109,
  CURLOPT_FTP_CREATE_MISSING_DIRS = 110,
  CURLOPT_PROXYAUTH = 111,
  CURLOPT_FTP_RESPONSE_TIMEOUT = 112,
  CURLOPT_IPRESOLVE = 113,
  CURLOPT_MAXFILESIZE = 114,
  CURLOPT_INFILESIZE_LARGE = 30115,
  CURLOPT_RESUME_FROM_LARGE = 30116,
  CURLOPT_MAXFILESIZE_LARGE = 30117,
  CURLOPT_NETRC_FILE = 10118,
  CURLOPT_USE_SSL = 119,
  CURLOPT_POSTFIELDSIZE_LARGE = 30120,
  CURLOPT_TCP_NODELAY = 121,
  CURLOPT_FTPSSLAUTH = 129,
  CURLOPT_IOCTLFUNCTION = 20130,
  CURLOPT_IOCTLDATA = 10131,
  CURLOPT_FTP_ACCOUNT = 10134,
  CURLOPT_COOKIELIST = 10135,
  CURLOPT_IGNORE_CONTENT_LENGTH = 136,
  CURLOPT_FTP_SKIP_PASV_IP = 137,
  CURLOPT_FTP_FILEMETHOD = 138,
  CURLOPT_LOCALPORT = 139,
  CURLOPT_LOCALPORTRANGE = 140,
  CURLOPT_CONNECT_ONLY = 141,
  CURLOPT_CONV_FROM_NETWORK_FUNCTION = 20142,
  CURLOPT_CONV_TO_NETWORK_FUNCTION = 20143,
  CURLOPT_CONV_FROM_UTF8_FUNCTION = 20144,
  CURLOPT_MAX_SEND_SPEED_LARGE = 30145,
  CURLOPT_MAX_RECV_SPEED_LARGE = 30146,
  CURLOPT_FTP_ALTERNATIVE_TO_USER = 10147,
  CURLOPT_SOCKOPTFUNCTION = 20148,
  CURLOPT_SOCKOPTDATA = 10149,
  CURLOPT_SSL_SESSIONID_CACHE = 150,
  CURLOPT_SSH_AUTH_TYPES = 151,
  CURLOPT_SSH_PUBLIC_KEYFILE = 10152,
  CURLOPT_SSH_PRIVATE_KEYFILE = 10153,
  CURLOPT_FTP_SSL_CCC = 154,
  CURLOPT_TIMEOUT_MS = 155,
  CURLOPT_CONNECTTIMEOUT_MS = 156,
  CURLOPT_HTTP_TRANSFER_DECODING = 157,
  CURLOPT_HTTP_CONTENT_DECODING = 158,
  CURLOPT_NEW_FILE_PERMS = 159,
  CURLOPT_NEW_DIRECTORY_PERMS = 160,
  CURLOPT_POSTREDIR = 161,
  CURLOPT_SSH_HOST_PUBLIC_KEY_MD5 = 10162,
  CURLOPT_OPENSOCKETFUNCTION = 20163,
  CURLOPT_OPENSOCKETDATA = 10164,
  CURLOPT_COPYPOSTFIELDS = 10165,
  CURLOPT_PROXY_TRANSFER_MODE = 166,
  CURLOPT_SEEKFUNCTION = 20167,
  CURLOPT_SEEKDATA = 10168,
  CURLOPT_CRLFILE = 10169,
  CURLOPT_ISSUERCERT = 10170,
  CURLOPT_ADDRESS_SCOPE = 171,
  CURLOPT_CERTINFO = 172,
  CURLOPT_USERNAME = 10173,
  CURLOPT_PASSWORD = 10174,
  CURLOPT_PROXYUSERNAME = 10175,
  CURLOPT_PROXYPASSWORD = 10176,
  CURLOPT_NOPROXY = 10177,
  CURLOPT_TFTP_BLKSIZE = 178,
  CURLOPT_SOCKS5_GSSAPI_SERVICE = 10179,
  CURLOPT_SOCKS5_GSSAPI_NEC = 180,
  CURLOPT_PROTOCOLS = 181,
  CURLOPT_REDIR_PROTOCOLS = 182,
  CURLOPT_SSH_KNOWNHOSTS = 10183,
  CURLOPT_SSH_KEYFUNCTION = 20184,
  CURLOPT_SSH_KEYDATA = 10185,
  CURLOPT_MAIL_FROM = 10186,
  CURLOPT_MAIL_RCPT = 10187,
  CURLOPT_FTP_USE_PRET = 188,
  CURLOPT_RTSP_REQUEST = 189,
  CURLOPT_RTSP_SESSION_ID = 10190,
  CURLOPT_RTSP_STREAM_URI = 10191,
  CURLOPT_RTSP_TRANSPORT = 10192,
  CURLOPT_RTSP_CLIENT_CSEQ = 193,
  CURLOPT_RTSP_SERVER_CSEQ = 194,
  CURLOPT_INTERLEAVEDATA = 10195,
  CURLOPT_INTERLEAVEFUNCTION = 20196,
  CURLOPT_WILDCARDMATCH = 197,
  CURLOPT_CHUNK_BGN_FUNCTION = 20198,
  CURLOPT_CHUNK_END_FUNCTION = 20199,
  CURLOPT_FNMATCH_FUNCTION = 20200,
  CURLOPT_CHUNK_DATA = 10201,
  CURLOPT_FNMATCH_DATA = 10202,
  CURLOPT_RESOLVE = 10203,
  CURLOPT_TLSAUTH_USERNAME = 10204,
  CURLOPT_TLSAUTH_PASSWORD = 10205,
  CURLOPT_TLSAUTH_TYPE = 10206,
  CURLOPT_TRANSFER_ENCODING = 207,
  CURLOPT_CLOSESOCKETFUNCTION = 20208,
  CURLOPT_CLOSESOCKETDATA = 10209,
  CURLOPT_GSSAPI_DELEGATION = 210,
  CURLOPT_DNS_SERVERS = 10211,
  CURLOPT_ACCEPTTIMEOUT_MS = 212,
  CURLOPT_TCP_KEEPALIVE = 213,
  CURLOPT_TCP_KEEPIDLE = 214,
  CURLOPT_TCP_KEEPINTVL = 215,
  CURLOPT_SSL_OPTIONS = 216,
  CURLOPT_MAIL_AUTH = 10217,
  CURLOPT_SASL_IR = 218,
  CURLOPT_XFERINFOFUNCTION = 20219,
  CURLOPT_XOAUTH2_BEARER = 10220,
  CURLOPT_DNS_INTERFACE = 10221,
  CURLOPT_DNS_LOCAL_IP4 = 10222,
  CURLOPT_DNS_LOCAL_IP6 = 10223,
  CURLOPT_LOGIN_OPTIONS = 10224,
  CURLOPT_SSL_ENABLE_NPN = 225,
  CURLOPT_SSL_ENABLE_ALPN = 226,
  CURLOPT_EXPECT_100_TIMEOUT_MS = 227,
  CURLOPT_PROXYHEADER = 10228,
  CURLOPT_HEADEROPT = 229,
  CURLOPT_PINNEDPUBLICKEY = 10230,
  CURLOPT_UNIX_SOCKET_PATH = 10231,
  CURLOPT_SSL_VERIFYSTATUS = 232,
  CURLOPT_SSL_FALSESTART = 233,
  CURLOPT_PATH_AS_IS = 234,
  CURLOPT_PROXYAUTOCONFIG = 242,
  CURLOPT_SSL_VERIFYDATE = 243,
  CURLOPT_SSL_CONTEXT = 10244,
  CURLOPT_SSL_CONN_FUNCTION_BEFORE = 20245,
  CURLOPT_SSL_CONN_FUNCTION_AFTER = 20246,
  CURLOPT_SSL_CONN_DATA = 10247,
  CURLOPT_DISABLE_NSD = 248,
  CURLOPT_DISABLE_CACHE = 249,
  CURLOPT_SSL_SKIP_DEFAULT_VERIFY = 250,
  CURLOPT_LASTENTRY = 251,
} CURLoption;

#define CURLOPT_SERVER_RESPONSE_TIMEOUT CURLOPT_FTP_RESPONSE_TIMEOUT

#define CURL_REDIR_POST_301 1
#define CURL_REDIR_POST_302 2
#define CURL_REDIR_POST_303 4
#define CURL_REDIR_POST_ALL \
    (CURL_REDIR_POST_301|CURL_REDIR_POST_302|CURL_REDIR_POST_303)
#define CURLHEADER_UNIFIED  0
#define CURLHEADER_SEPARATE (1<<0)
#define CURLSSLOPT_ALLOW_BEAST (1<<0)

enum {
  CURL_RTSPREQ_NONE, CURL_RTSPREQ_OPTIONS, CURL_RTSPREQ_DESCRIBE,
  CURL_RTSPREQ_ANNOUNCE, CURL_RTSPREQ_SETUP, CURL_RTSPREQ_PLAY,
  CURL_RTSPREQ_PAUSE, CURL_RTSPREQ_TEARDOWN, CURL_RTSPREQ_GET_PARAMETER,
  CURL_RTSPREQ_SET_PARAMETER, CURL_RTSPREQ_RECORD, CURL_RTSPREQ_RECEIVE,
  CURL_RTSPREQ_LAST
};

#endif /* D3_RECOVERED_PROGRAMS_ERIS_OUTPUTS_INCLUDE_CURL_CURL_H */
