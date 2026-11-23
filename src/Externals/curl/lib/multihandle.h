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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_MULTIHANDLE_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_MULTIHANDLE_H

#include <stddef.h>
#include "Programs/Eris/Outputs/Include/curl/multi.h"

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/multihandle.h:55 */
typedef enum {
  CURLM_STATE_INIT = 0,
  CURLM_STATE_CONNECT_PEND = 1,
  CURLM_STATE_CONNECT = 2,
  CURLM_STATE_WAITRESOLVE = 3,
  CURLM_STATE_WAITCONNECT = 4,
  CURLM_STATE_WAITPROXYCONNECT = 5,
  CURLM_STATE_SENDPROTOCONNECT = 6,
  CURLM_STATE_PROTOCONNECT = 7,
  CURLM_STATE_WAITDO = 8,
  CURLM_STATE_DO = 9,
  CURLM_STATE_DOING = 10,
  CURLM_STATE_DO_MORE = 11,
  CURLM_STATE_DO_DONE = 12,
  CURLM_STATE_WAITPERFORM = 13,
  CURLM_STATE_PERFORM = 14,
  CURLM_STATE_TOOFAST = 15,
  CURLM_STATE_DONE = 16,
  CURLM_STATE_COMPLETED = 17,
  CURLM_STATE_MSGSENT = 18,
  CURLM_STATE_LAST = 19
} CURLMstate;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/multihandle.h:25 */
struct Curl_message {
  struct CURLMsg extmsg; /* 0x0 */
};

_Static_assert(sizeof(struct Curl_message) == 24, "Curl_message size");
_Static_assert(offsetof(struct Curl_message, extmsg) == 0, "Curl_message.extmsg");

/* Added for url.c (ConnectionDone reads maxconnects/num_easy):
 * D:/home/teamcity2/work/sdk/Externals/curl/lib/multihandle.h:64 */
struct SessionHandle;
struct curl_llist;
struct curl_hash;
struct Curl_tree;
struct conncache;
struct Curl_multi {
  long type; /* 0x0 */
  struct SessionHandle *easyp; /* 0x8 */
  struct SessionHandle *easylp; /* 0x10 */
  int num_easy; /* 0x18 */
  int num_alive; /* 0x1c */
  struct curl_llist *msglist; /* 0x20 */
  struct curl_llist *pending; /* 0x28 */
  int (*socket_cb)(CURL *, curl_socket_t, int, void *, void *); /* 0x30 */
  void *socket_userp; /* 0x38 */
  struct curl_hash *hostcache; /* 0x40 */
  struct Curl_tree *timetree; /* 0x48 */
  struct curl_hash *sockhash; /* 0x50 */
  _Bool pipelining_enabled; /* 0x58 */
  struct conncache *conn_cache; /* 0x60 */
  struct SessionHandle *closure_handle; /* 0x68 */
  long maxconnects; /* 0x70 */
  long max_host_connections; /* 0x78 */
  long max_total_connections; /* 0x80 */
  long max_pipeline_length; /* 0x88 */
  long content_length_penalty_size; /* 0x90 */
  long chunk_length_penalty_size; /* 0x98 */
  struct curl_llist *pipelining_site_bl; /* 0xa0 */
  struct curl_llist *pipelining_server_bl; /* 0xa8 */
  int (*timer_cb)(void *, long, void *); /* 0xb0 */
  void *timer_userp; /* 0xb8 */
  struct timeval timer_lastcall; /* 0xc0 */
};

_Static_assert(sizeof(struct Curl_multi) == 208, "Curl_multi size");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_MULTIHANDLE_H */
