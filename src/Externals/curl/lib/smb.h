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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_SMB_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_SMB_H

#include <stddef.h>
#include <stdint.h>


/* D:/home/teamcity2/work/sdk/Externals/curl/lib/smb.h:25; DIE 0x7c8f266 */
enum smb_conn_state {
  SMB_NOT_CONNECTED = 0,
  SMB_CONNECTING = 1,
  SMB_NEGOTIATE = 2,
  SMB_SETUP = 3,
  SMB_CONNECTED = 4
};


/* D:/home/teamcity2/work/sdk/Externals/curl/lib/smb.h:33; DIE 0x7c92639 */
struct smb_conn {
  enum smb_conn_state state; /* 0x0 */
  char *user; /* 0x8 */
  char *domain; /* 0x10 */
  unsigned char challenge[8]; /* 0x18 */
  unsigned int session_key; /* 0x20 */
  unsigned short uid; /* 0x24 */
  char *recv_buf; /* 0x28 */
  size_t upload_size; /* 0x30 */
  size_t send_size; /* 0x38 */
  size_t sent; /* 0x40 */
  size_t got; /* 0x48 */
};

_Static_assert(sizeof(struct smb_conn) == 80, "smb_conn size");
_Static_assert(offsetof(struct smb_conn, state) == 0, "smb_conn.state");
_Static_assert(offsetof(struct smb_conn, user) == 8, "smb_conn.user");
_Static_assert(offsetof(struct smb_conn, domain) == 16, "smb_conn.domain");
_Static_assert(offsetof(struct smb_conn, challenge) == 24, "smb_conn.challenge");
_Static_assert(offsetof(struct smb_conn, session_key) == 32, "smb_conn.session_key");
_Static_assert(offsetof(struct smb_conn, uid) == 36, "smb_conn.uid");
_Static_assert(offsetof(struct smb_conn, recv_buf) == 40, "smb_conn.recv_buf");
_Static_assert(offsetof(struct smb_conn, upload_size) == 48, "smb_conn.upload_size");
_Static_assert(offsetof(struct smb_conn, send_size) == 56, "smb_conn.send_size");
_Static_assert(offsetof(struct smb_conn, sent) == 64, "smb_conn.sent");
_Static_assert(offsetof(struct smb_conn, got) == 72, "smb_conn.got");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_SMB_H */
