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


/* Original DWARF declarations consumed by conncache.c (struct SessionHandle
 * closure). Complete definitions for every by-value type; pointer-only types
 * are forward declarations. No synthetic padding, hidden body or API stub.
 */
#ifndef D3_RECOVERED_PROGRAMS_ERIS_OUTPUTS_INCLUDE_CURL_MULTI_H
#define D3_RECOVERED_PROGRAMS_ERIS_OUTPUTS_INCLUDE_CURL_MULTI_H

#include <stddef.h>
#include "Programs/Eris/Outputs/Include/curl/curl.h"

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/multi.h:82 */
typedef enum {
  CURLMSG_NONE = 0,
  CURLMSG_DONE = 1,
  CURLMSG_LAST = 2
} CURLMSG;

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/multi.h:84 */
struct CURLMsg {
  CURLMSG msg; /* 0x0 */
  CURL *easy_handle; /* 0x8 */
  union {
    void *whatever; /* 0x0 */
    CURLcode result; /* 0x0 */
  } data; /* 0x10 */
};

_Static_assert(sizeof(struct CURLMsg) == 24, "CURLMsg size");
_Static_assert(offsetof(struct CURLMsg, msg) == 0, "CURLMsg.msg");
_Static_assert(offsetof(struct CURLMsg, easy_handle) == 8, "CURLMsg.easy_handle");
_Static_assert(offsetof(struct CURLMsg, data) == 16, "CURLMsg.data");

/* D:/home/teamcity2/work/sdk/Programs/Eris/Outputs/Include/curl/multi.h:92 */
typedef struct CURLMsg CURLMsg;

/* Added for multi.c. */
typedef void CURLM;
typedef int (*curl_socket_callback)(CURL *easy, curl_socket_t s, int what,
                                    void *userp, void *socketp);
typedef int (*curl_multi_timer_callback)(CURLM *multi, long timeout_ms,
                                         void *userp);

#endif /* D3_RECOVERED_PROGRAMS_ERIS_OUTPUTS_INCLUDE_CURL_MULTI_H */
