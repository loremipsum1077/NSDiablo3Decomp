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

/*
 * COPYRIGHT AND PERMISSION NOTICE (curl COPYING)
 *
 * Copyright (c) 1996 - 2016, Daniel Stenberg, <daniel@haxx.se>, and many
 * contributors, see the THANKS file.
 *
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall
 * not be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization of the
 * copyright holder.
 */

/*
 * Recovered owner: Externals/curl/lib/curl_addrinfo.c (SRC-8a8141e44156).
 * Recovery-only source; no strict certification is claimed. Original
 * identities and the limited import ABI evidence are recorded below.
 *
 *   Curl_freeaddrinfo  0x7100b8d710   68 bytes  (curl_addrinfo.c:77)
 *   Curl_he2ai         0x7100b8d760  316 bytes  (curl_addrinfo.c:266)
 *   Curl_ip2addr       0x7100b8d8a0  192 bytes  (curl_addrinfo.c:383)
 *   Curl_str2addr      0x7100b8d960  184 bytes  (curl_addrinfo.c:458)
 *
 * Bodies follow public curl-7_50_3:
 *   https://raw.githubusercontent.com/curl/curl/curl-7_50_3/lib/curl_addrinfo.c
 *   https://raw.githubusercontent.com/curl/curl/curl-7_50_3/lib/curl_addrinfo.h
 * That tag is a body reference only, not a claim about the exact Nintendo
 * fork. Where they differ, the original DWARF (CU DIE 0x07c6b027, unit header
 * 0x07c6b01c) and original instructions control.
 *
 * Types are taken from the original CU DIEs noted on each declaration. The
 * target uses Nintendo/BSD socket layouts (one-byte sa_family at offset 1),
 * not host layouts, so they are declared locally here.
 *
 * IPv4 only: namebuff's addrentry union holds only ina4 (DIE 0x07c6b670,
 * 4 bytes) and the original code only handles AF_INET (2). This is upstream
 * built without ENABLE_IPV6. Release DEBUGASSERTs are absent in the original
 * instructions and are omitted.
 *
 * htons boundary: the original inlines a wrapper `uint16_t htons(uint16_t
 * hostValue)` (DIE 0x07c6b315/0x07c6b322, curl_nintendo_socket_posix.h:348)
 * whose call at 0xb8d81c goes to PLT 0xc1efe0, authenticated as the
 * undefined global function nnsocketInetHtons (JUMP_SLOT 1026, addend 0).
 * There is NO DWARF declaration for nnsocketInetHtons itself. Its prototype
 * below is a narrow call-site ABI inference (port moved to w0, low 16 bits of
 * w0 stored to sin_port) and is not a proven SDK import prototype.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Programs/Chris/Include/nn/socket/sys/socket.h:51, DIE 0x07c6b2c6 */
typedef unsigned int socklen_t;
/* Programs/Eris/Outputs/Include/curl/curlbuild.h:164, DIE 0x07c6b2bb */
typedef socklen_t curl_socklen_t;
/* Programs/Chris/Include/nn/socket/sys/socket.h:50, DIE 0x07c6b42d */
typedef uint8_t sa_family_t;
/* Programs/Chris/Include/nn/socket/netinet/in.h:53, DIE 0x07c6b83c */
typedef uint16_t in_port_t;
/* Programs/Chris/Include/nn/socket/netinet/in.h:52, DIE 0x07c6b6f0 */
typedef uint32_t in_addr_t;

#define AF_INET     2
#define SOCK_STREAM 1

/* Programs/Eris/Outputs/Include/curl/curl.h:414/553, DIEs 0x07c6b046 and
   0x07c6b7e6 */
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

/* Programs/Chris/Include/nn/socket/sys/socket.h:263, DIE 0x07c6b3f5 */
struct sockaddr {
  unsigned char sa_len;
  sa_family_t   sa_family;
  char          sa_data[14];
};

/* Programs/Chris/Include/nn/socket/netinet/in.h:56, DIE 0x07c6b6db */
struct in_addr {
  in_addr_t s_addr;
};

/* Programs/Chris/Include/nn/socket/netinet/in.h:62, DIE 0x07c6b7f7 */
struct sockaddr_in {
  uint8_t        sin_len;
  sa_family_t    sin_family;
  in_port_t      sin_port;
  struct in_addr sin_addr;
  char           sin_zero[8];
};

/* Programs/Chris/Include/nn/socket/netdb.h:96, DIE 0x07c6b691 */
struct hostent {
  char  *h_name;
  char **h_aliases;
  int    h_addrtype;
  int    h_length;
  char **h_addr_list;
};

/* Externals/curl/lib/curl_addrinfo.h:51/61, DIEs 0x07c6b374 and 0x07c6b369 */
struct Curl_addrinfo {
  int                   ai_flags;
  int                   ai_family;
  int                   ai_socktype;
  int                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  char                 *ai_canonname;
  struct sockaddr      *ai_addr;
  struct Curl_addrinfo *ai_next;
};
typedef struct Curl_addrinfo Curl_addrinfo;

_Static_assert(sizeof(struct sockaddr) == 16, "sockaddr size");
_Static_assert(offsetof(struct sockaddr, sa_family) == 1, "sa_family");
_Static_assert(offsetof(struct sockaddr, sa_data) == 2, "sa_data");
_Static_assert(sizeof(struct in_addr) == 4, "in_addr size");
_Static_assert(sizeof(struct sockaddr_in) == 16, "sockaddr_in size");
_Static_assert(offsetof(struct sockaddr_in, sin_family) == 1, "sin_family");
_Static_assert(offsetof(struct sockaddr_in, sin_port) == 2, "sin_port");
_Static_assert(offsetof(struct sockaddr_in, sin_addr) == 4, "sin_addr");
_Static_assert(offsetof(struct sockaddr_in, sin_zero) == 8, "sin_zero");
_Static_assert(sizeof(struct hostent) == 32, "hostent size");
_Static_assert(offsetof(struct hostent, h_addrtype) == 16, "h_addrtype");
_Static_assert(offsetof(struct hostent, h_length) == 20, "h_length");
_Static_assert(offsetof(struct hostent, h_addr_list) == 24, "h_addr_list");
_Static_assert(sizeof(Curl_addrinfo) == 48, "Curl_addrinfo size");
_Static_assert(offsetof(Curl_addrinfo, ai_addrlen) == 16, "ai_addrlen");
_Static_assert(offsetof(Curl_addrinfo, ai_canonname) == 24, "ai_canonname");
_Static_assert(offsetof(Curl_addrinfo, ai_addr) == 32, "ai_addr");
_Static_assert(offsetof(Curl_addrinfo, ai_next) == 40, "ai_next");

/* Externals/curl/siglo/curl_nintendo_allocator.c (original DWARF) */
extern void *Curl_SigloMalloc(size_t sizeToAllocate);
extern void *Curl_SigloCalloc(size_t nmemb, size_t size);
extern void Curl_SigloFree(void *pointer);
extern char *Curl_SigloStrdup(const char *string);

/* Externals/curl/lib/inet_pton.c:66 (original DWARF) */
extern int Curl_inet_pton(int af, const char *src, void *dst);

/* Nintendo SDK import. Call-site ABI inference only; see header comment. */
extern uint16_t nnsocketInetHtons(uint16_t hostValue);

/* Externals/curl/siglo/curl_nintendo_socket_posix.h:348, DIE 0x07c6b315 */
static inline uint16_t htons(uint16_t hostValue)
{
  return nnsocketInetHtons(hostValue);
}

Curl_addrinfo *Curl_he2ai(const struct hostent *he, int port);
Curl_addrinfo *Curl_ip2addr(int af, const void *inaddr, const char *hostname,
                            int port);
Curl_addrinfo *Curl_str2addr(char *address, int port);

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

void
Curl_freeaddrinfo(Curl_addrinfo *cahead)
{
  Curl_addrinfo *ca;
  Curl_addrinfo *canext;

  for(ca = cahead; ca != NULL; ca = canext) {
    Curl_SigloFree(ca->ai_addr);
    Curl_SigloFree(ca->ai_canonname);
    canext = ca->ai_next;

    Curl_SigloFree(ca);
  }
}

/*
 * Curl_he2ai()
 *
 * This function returns a pointer to the first element of a newly allocated
 * Curl_addrinfo struct linked list filled with the data of a given hostent.
 * Curl_addrinfo is meant to work like the addrinfo struct does for a IPv6
 * stack, but usable also for IPv4, all hosts and environments.
 */

Curl_addrinfo *
Curl_he2ai(const struct hostent *he, int port)
{
  Curl_addrinfo *ai;
  Curl_addrinfo *prevai = NULL;
  Curl_addrinfo *firstai = NULL;
  struct sockaddr_in *addr;
  CURLcode result = CURLE_OK;
  int i;
  char *curr;

  if(!he)
    /* no input == no output! */
    return NULL;

  for(i=0; (curr = he->h_addr_list[i]) != NULL; i++) {

    size_t ss_size;
    ss_size = sizeof (struct sockaddr_in);

    if((ai = Curl_SigloCalloc(1, sizeof(Curl_addrinfo))) == NULL) {
      result = CURLE_OUT_OF_MEMORY;
      break;
    }
    if((ai->ai_canonname = Curl_SigloStrdup(he->h_name)) == NULL) {
      result = CURLE_OUT_OF_MEMORY;
      Curl_SigloFree(ai);
      break;
    }
    if((ai->ai_addr = Curl_SigloCalloc(1, ss_size)) == NULL) {
      result = CURLE_OUT_OF_MEMORY;
      Curl_SigloFree(ai->ai_canonname);
      Curl_SigloFree(ai);
      break;
    }

    if(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    if(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    switch (ai->ai_family) {
    case AF_INET:
      addr = (void *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, sizeof(struct in_addr));
      addr->sin_family = (unsigned short)(he->h_addrtype);
      addr->sin_port = htons((unsigned short)port);
      break;
    }

    prevai = ai;
  }

  if(result) {
    Curl_freeaddrinfo(firstai);
    firstai = NULL;
  }

  return firstai;
}


/* Externals/curl/lib/curl_addrinfo.c:361, DIE 0x07c6b64d */
struct namebuff {
  struct hostent hostentry;
  union {
    struct in_addr  ina4;
  } addrentry;
  char *h_addr_list[2];
};

_Static_assert(sizeof(struct namebuff) == 56, "namebuff size");
_Static_assert(offsetof(struct namebuff, addrentry) == 32, "addrentry");
_Static_assert(sizeof(((struct namebuff *)0)->addrentry) == 4,
               "addrentry size");
_Static_assert(offsetof(struct namebuff, h_addr_list) == 40, "h_addr_list");


/*
 * Curl_ip2addr()
 *
 * This function takes an internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */

Curl_addrinfo *
Curl_ip2addr(int af, const void *inaddr, const char *hostname, int port)
{
  Curl_addrinfo *ai;
  struct hostent  *h;
  struct namebuff *buf;
  char  *addrentry;
  char  *hoststr;
  size_t addrsize;

  buf = Curl_SigloMalloc(sizeof(struct namebuff));
  if(!buf)
    return NULL;

  hoststr = Curl_SigloStrdup(hostname);
  if(!hoststr) {
    Curl_SigloFree(buf);
    return NULL;
  }

  switch(af) {
  case AF_INET:
    addrsize = sizeof(struct in_addr);
    addrentry = (void *)&buf->addrentry.ina4;
    memcpy(addrentry, inaddr, sizeof(struct in_addr));
    break;
  default:
    Curl_SigloFree(hoststr);
    Curl_SigloFree(buf);
    return NULL;
  }

  h = &buf->hostentry;
  h->h_name = hoststr;
  h->h_aliases = NULL;
  h->h_addrtype = (short)af;
  h->h_length = (short)addrsize;
  h->h_addr_list = &buf->h_addr_list[0];
  h->h_addr_list[0] = addrentry;
  h->h_addr_list[1] = NULL; /* terminate list of entries */

  ai = Curl_he2ai(h, port);

  Curl_SigloFree(hoststr);
  Curl_SigloFree(buf);

  return ai;
}

/*
 * Given an IPv4 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
Curl_addrinfo *Curl_str2addr(char *address, int port)
{
  struct in_addr in;
  if(Curl_inet_pton(AF_INET, address, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    return Curl_ip2addr(AF_INET, &in, address, port);
  return NULL; /* bad input format */
}
