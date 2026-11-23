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

#ifndef D3_RECOVERED_CURL_SHARE_H
#define D3_RECOVERED_CURL_SHARE_H

#include <stddef.h>
#include "Programs/Eris/Outputs/Include/curl/curl.h"

/* Callback declarations: original curl.h:2266 and :2270. */
typedef void (*curl_lock_function)(CURL *, curl_lock_data, curl_lock_access, void *);
typedef void (*curl_unlock_function)(CURL *, curl_lock_data, void *);

struct curl_hash;
struct CookieInfo;
struct curl_ssl_session;
struct SessionHandle;

/* Original share.h:39; NSS DIE 0x07c1273d. */
struct Curl_share {
  unsigned int specifier;
  volatile unsigned int dirty;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  void *clientdata;
  struct curl_hash *hostcache;
  struct CookieInfo *cookies;
  struct curl_ssl_session *sslsession;
  size_t max_ssl_sessions;
  long sessionage;
};
_Static_assert(sizeof(struct Curl_share) == 72, "Curl_share size");
_Static_assert(offsetof(struct Curl_share, lockfunc) == 8, "Curl_share lockfunc");
_Static_assert(offsetof(struct Curl_share, unlockfunc) == 16, "Curl_share unlockfunc");
_Static_assert(offsetof(struct Curl_share, clientdata) == 24, "Curl_share clientdata");

CURLSHcode Curl_share_lock(struct SessionHandle *, curl_lock_data, curl_lock_access);
CURLSHcode Curl_share_unlock(struct SessionHandle *, curl_lock_data);
#endif
