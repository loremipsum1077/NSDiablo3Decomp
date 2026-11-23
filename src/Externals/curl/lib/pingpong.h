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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_PINGPONG_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_PINGPONG_H

#include <stddef.h>
#include <stdint.h>
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/bits/alltypes.h"
#include "Programs/Eris/Outputs/Include/curl/curl.h"

struct connectdata;
struct timeval;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/pingpong.h:48; DIE 0x7c92239 */
struct pingpong {
  char *cache; /* 0x0 */
  size_t cache_size; /* 0x8 */
  size_t nread_resp; /* 0x10 */
  char *linestart_resp; /* 0x18 */
  _Bool pending_resp; /* 0x20 */
  char *sendthis; /* 0x28 */
  size_t sendleft; /* 0x30 */
  size_t sendsize; /* 0x38 */
  struct timeval response; /* 0x40 */
  long response_time; /* 0x50 */
  struct connectdata *conn; /* 0x58 */
  CURLcode (*statemach_act)(struct connectdata *); /* 0x60 */
  _Bool (*endofresp)(struct connectdata *, char *, size_t, int *); /* 0x68 */
};

_Static_assert(sizeof(struct pingpong) == 112, "pingpong size");
_Static_assert(offsetof(struct pingpong, cache) == 0, "pingpong.cache");
_Static_assert(offsetof(struct pingpong, cache_size) == 8, "pingpong.cache_size");
_Static_assert(offsetof(struct pingpong, nread_resp) == 16, "pingpong.nread_resp");
_Static_assert(offsetof(struct pingpong, linestart_resp) == 24, "pingpong.linestart_resp");
_Static_assert(offsetof(struct pingpong, pending_resp) == 32, "pingpong.pending_resp");
_Static_assert(offsetof(struct pingpong, sendthis) == 40, "pingpong.sendthis");
_Static_assert(offsetof(struct pingpong, sendleft) == 48, "pingpong.sendleft");
_Static_assert(offsetof(struct pingpong, sendsize) == 56, "pingpong.sendsize");
_Static_assert(offsetof(struct pingpong, response) == 64, "pingpong.response");
_Static_assert(offsetof(struct pingpong, response_time) == 80, "pingpong.response_time");
_Static_assert(offsetof(struct pingpong, conn) == 88, "pingpong.conn");
_Static_assert(offsetof(struct pingpong, statemach_act) == 96, "pingpong.statemach_act");
_Static_assert(offsetof(struct pingpong, endofresp) == 104, "pingpong.endofresp");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_PINGPONG_H */
