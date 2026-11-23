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

/* Reconstructed from this binary's instructions and DWARF. */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Externals/curl/lib/urldata.h"

/* Original SDK declarations, used only through pointers here. */
typedef struct nnsocketResolverOption nnsocketResolverOption;
struct hostent;
struct in_addr { uint32_t s_addr; };
#define AF_INET 2
#define CURL_IPRESOLVE_V6 2
int Curl_inet_pton(int af, const char *src, void *dst);
Curl_addrinfo *Curl_ip2addr(int af, const void *inaddr, const char *hostname, int port);
Curl_addrinfo *Curl_he2ai(const struct hostent *he, int port);
struct hostent *nnCurlShim_gethostbyname(const char *name, nnsocketResolverOption *options, size_t resolverOptionCount);



























bool Curl_ipvalid(struct connectdata *conn)
{
  return conn->ip_version != CURL_IPRESOLVE_V6;
}























































Curl_addrinfo *Curl_ipv4_resolve_r(const char *hostname,
                                   int port,
                                   nnsocketResolverOption *options,
                                   size_t optionCount)
{







  Curl_addrinfo *ai;
  struct hostent *h;
  struct in_addr in;
  struct hostent *buf;

  if(Curl_inet_pton(AF_INET, hostname, &in) > 0)

    return Curl_ip2addr(AF_INET, &in, hostname, port);































































































































































  h = nnCurlShim_gethostbyname(hostname, options, optionCount);




  if(h)
    ai = Curl_he2ai(h, port);
  else
    ai = NULL;



  return ai;
}
