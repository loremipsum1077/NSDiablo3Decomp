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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_SMTP_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_SMTP_H

#include <stddef.h>
#include <stdint.h>
#include "Externals/curl/lib/curl_sasl.h"
#include "Externals/curl/lib/pingpong.h"

struct SASL;
struct pingpong;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/smtp.h:48; DIE 0x7c92601 */
typedef enum {
  SMTP_STOP = 0,
  SMTP_SERVERGREET = 1,
  SMTP_EHLO = 2,
  SMTP_HELO = 3,
  SMTP_STARTTLS = 4,
  SMTP_UPGRADETLS = 5,
  SMTP_AUTH = 6,
  SMTP_COMMAND = 7,
  SMTP_MAIL = 8,
  SMTP_RCPT = 9,
  SMTP_DATA = 10,
  SMTP_POSTDATA = 11,
  SMTP_QUIT = 12,
  SMTP_LAST = 13
} smtpstate;

_Static_assert(sizeof(smtpstate) == 4, "smtpstate size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/smtp.h:65; DIE 0x7c92598 */
struct smtp_conn {
  struct pingpong pp; /* 0x0 */
  smtpstate state; /* 0x70 */
  _Bool ssldone; /* 0x74 */
  char *domain; /* 0x78 */
  struct SASL sasl; /* 0x80 */
  _Bool tls_supported; /* 0xa0 */
  _Bool size_supported; /* 0xa1 */
  _Bool auth_supported; /* 0xa2 */
};

_Static_assert(sizeof(struct smtp_conn) == 168, "smtp_conn size");
_Static_assert(offsetof(struct smtp_conn, pp) == 0, "smtp_conn.pp");
_Static_assert(offsetof(struct smtp_conn, state) == 112, "smtp_conn.state");
_Static_assert(offsetof(struct smtp_conn, ssldone) == 116, "smtp_conn.ssldone");
_Static_assert(offsetof(struct smtp_conn, domain) == 120, "smtp_conn.domain");
_Static_assert(offsetof(struct smtp_conn, sasl) == 128, "smtp_conn.sasl");
_Static_assert(offsetof(struct smtp_conn, tls_supported) == 160, "smtp_conn.tls_supported");
_Static_assert(offsetof(struct smtp_conn, size_supported) == 161, "smtp_conn.size_supported");
_Static_assert(offsetof(struct smtp_conn, auth_supported) == 162, "smtp_conn.auth_supported");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_SMTP_H */
