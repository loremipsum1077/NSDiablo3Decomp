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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_RTSP_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_RTSP_H

#include <stddef.h>
#include <stdint.h>
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/bits/alltypes.h"


/* D:/home/teamcity2/work/sdk/Externals/curl/lib/rtsp.h:43; DIE 0x7c9260c */
struct rtsp_conn {
  char *rtp_buf; /* 0x0 */
  ssize_t rtp_bufsize; /* 0x8 */
  int rtp_channel; /* 0x10 */
};

_Static_assert(sizeof(struct rtsp_conn) == 24, "rtsp_conn size");
_Static_assert(offsetof(struct rtsp_conn, rtp_buf) == 0, "rtsp_conn.rtp_buf");
_Static_assert(offsetof(struct rtsp_conn, rtp_bufsize) == 8, "rtsp_conn.rtp_bufsize");
_Static_assert(offsetof(struct rtsp_conn, rtp_channel) == 16, "rtsp_conn.rtp_channel");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_RTSP_H */
