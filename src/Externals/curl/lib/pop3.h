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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_POP3_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_POP3_H

#include <stddef.h>
#include <stdint.h>
#include "Externals/curl/lib/curl_sasl.h"
#include "Externals/curl/lib/pingpong.h"

struct SASL;
struct pingpong;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/pop3.h:46; DIE 0x7c9258d */
typedef enum {
  POP3_STOP = 0,
  POP3_SERVERGREET = 1,
  POP3_CAPA = 2,
  POP3_STARTTLS = 3,
  POP3_UPGRADETLS = 4,
  POP3_AUTH = 5,
  POP3_APOP = 6,
  POP3_USER = 7,
  POP3_PASS = 8,
  POP3_COMMAND = 9,
  POP3_QUIT = 10,
  POP3_LAST = 11
} pop3state;

_Static_assert(sizeof(pop3state) == 4, "pop3state size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/pop3.h:60; DIE 0x7c9250c */
struct pop3_conn {
  struct pingpong pp; /* 0x0 */
  pop3state state; /* 0x70 */
  _Bool ssldone; /* 0x74 */
  size_t eob; /* 0x78 */
  size_t strip; /* 0x80 */
  struct SASL sasl; /* 0x88 */
  unsigned int authtypes; /* 0xa8 */
  unsigned int preftype; /* 0xac */
  char *apoptimestamp; /* 0xb0 */
  _Bool tls_supported; /* 0xb8 */
};

_Static_assert(sizeof(struct pop3_conn) == 192, "pop3_conn size");
_Static_assert(offsetof(struct pop3_conn, pp) == 0, "pop3_conn.pp");
_Static_assert(offsetof(struct pop3_conn, state) == 112, "pop3_conn.state");
_Static_assert(offsetof(struct pop3_conn, ssldone) == 116, "pop3_conn.ssldone");
_Static_assert(offsetof(struct pop3_conn, eob) == 120, "pop3_conn.eob");
_Static_assert(offsetof(struct pop3_conn, strip) == 128, "pop3_conn.strip");
_Static_assert(offsetof(struct pop3_conn, sasl) == 136, "pop3_conn.sasl");
_Static_assert(offsetof(struct pop3_conn, authtypes) == 168, "pop3_conn.authtypes");
_Static_assert(offsetof(struct pop3_conn, preftype) == 172, "pop3_conn.preftype");
_Static_assert(offsetof(struct pop3_conn, apoptimestamp) == 176, "pop3_conn.apoptimestamp");
_Static_assert(offsetof(struct pop3_conn, tls_supported) == 184, "pop3_conn.tls_supported");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_POP3_H */
