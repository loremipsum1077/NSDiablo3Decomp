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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_IMAP_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_IMAP_H

#include <stddef.h>
#include <stdint.h>
#include "Externals/curl/lib/curl_sasl.h"
#include "Externals/curl/lib/pingpong.h"

struct SASL;
struct pingpong;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/imap.h:50; DIE 0x7c923da */
typedef enum {
  IMAP_STOP = 0,
  IMAP_SERVERGREET = 1,
  IMAP_CAPABILITY = 2,
  IMAP_STARTTLS = 3,
  IMAP_UPGRADETLS = 4,
  IMAP_AUTHENTICATE = 5,
  IMAP_LOGIN = 6,
  IMAP_LIST = 7,
  IMAP_SELECT = 8,
  IMAP_FETCH = 9,
  IMAP_FETCH_FINAL = 10,
  IMAP_APPEND = 11,
  IMAP_APPEND_FINAL = 12,
  IMAP_SEARCH = 13,
  IMAP_LOGOUT = 14,
  IMAP_LAST = 15
} imapstate;

_Static_assert(sizeof(imapstate) == 4, "imapstate size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/imap.h:70; DIE 0x7c92341 */
struct imap_conn {
  struct pingpong pp; /* 0x0 */
  imapstate state; /* 0x70 */
  _Bool ssldone; /* 0x74 */
  struct SASL sasl; /* 0x78 */
  unsigned int preftype; /* 0x98 */
  int cmdid; /* 0x9c */
  char resptag[5]; /* 0xa0 */
  _Bool tls_supported; /* 0xa5 */
  _Bool login_disabled; /* 0xa6 */
  _Bool ir_supported; /* 0xa7 */
  char *mailbox; /* 0xa8 */
  char *mailbox_uidvalidity; /* 0xb0 */
};

_Static_assert(sizeof(struct imap_conn) == 184, "imap_conn size");
_Static_assert(offsetof(struct imap_conn, pp) == 0, "imap_conn.pp");
_Static_assert(offsetof(struct imap_conn, state) == 112, "imap_conn.state");
_Static_assert(offsetof(struct imap_conn, ssldone) == 116, "imap_conn.ssldone");
_Static_assert(offsetof(struct imap_conn, sasl) == 120, "imap_conn.sasl");
_Static_assert(offsetof(struct imap_conn, preftype) == 152, "imap_conn.preftype");
_Static_assert(offsetof(struct imap_conn, cmdid) == 156, "imap_conn.cmdid");
_Static_assert(offsetof(struct imap_conn, resptag) == 160, "imap_conn.resptag");
_Static_assert(offsetof(struct imap_conn, tls_supported) == 165, "imap_conn.tls_supported");
_Static_assert(offsetof(struct imap_conn, login_disabled) == 166, "imap_conn.login_disabled");
_Static_assert(offsetof(struct imap_conn, ir_supported) == 167, "imap_conn.ir_supported");
_Static_assert(offsetof(struct imap_conn, mailbox) == 168, "imap_conn.mailbox");
_Static_assert(offsetof(struct imap_conn, mailbox_uidvalidity) == 176, "imap_conn.mailbox_uidvalidity");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_IMAP_H */
