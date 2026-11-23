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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_CURL_SASL_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_CURL_SASL_H

#include <stddef.h>
#include <stdint.h>

struct SASLproto;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/curl_sasl.h:91; DIE 0x7c924f5 */
typedef enum {
  SASL_STOP = 0,
  SASL_PLAIN = 1,
  SASL_LOGIN = 2,
  SASL_LOGIN_PASSWD = 3,
  SASL_EXTERNAL = 4,
  SASL_CRAMMD5 = 5,
  SASL_DIGESTMD5 = 6,
  SASL_DIGESTMD5_RESP = 7,
  SASL_NTLM = 8,
  SASL_NTLM_TYPE2MSG = 9,
  SASL_GSSAPI = 10,
  SASL_GSSAPI_TOKEN = 11,
  SASL_GSSAPI_NO_DATA = 12,
  SASL_XOAUTH2 = 13,
  SASL_CANCEL = 14,
  SASL_FINAL = 15
} saslstate;

_Static_assert(sizeof(saslstate) == 4, "saslstate size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/curl_sasl.h:116; DIE 0x7c923e5 */
struct SASL {
  const struct SASLproto *params; /* 0x0 */
  saslstate state; /* 0x8 */
  unsigned int authmechs; /* 0xc */
  unsigned int prefmech; /* 0x10 */
  unsigned int authused; /* 0x14 */
  _Bool resetprefs; /* 0x18 */
  _Bool mutual_auth; /* 0x19 */
  _Bool force_ir; /* 0x1a */
};

_Static_assert(sizeof(struct SASL) == 32, "SASL size");
_Static_assert(offsetof(struct SASL, params) == 0, "SASL.params");
_Static_assert(offsetof(struct SASL, state) == 8, "SASL.state");
_Static_assert(offsetof(struct SASL, authmechs) == 12, "SASL.authmechs");
_Static_assert(offsetof(struct SASL, prefmech) == 16, "SASL.prefmech");
_Static_assert(offsetof(struct SASL, authused) == 20, "SASL.authused");
_Static_assert(offsetof(struct SASL, resetprefs) == 24, "SASL.resetprefs");
_Static_assert(offsetof(struct SASL, mutual_auth) == 25, "SASL.mutual_auth");
_Static_assert(offsetof(struct SASL, force_ir) == 26, "SASL.force_ir");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_CURL_SASL_H */
