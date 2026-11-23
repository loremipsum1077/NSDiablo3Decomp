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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_WILDCARD_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_WILDCARD_H

#include <stddef.h>

struct curl_llist;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/wildcard.h:38 */
typedef enum {
  CURLWC_INIT = 0,
  CURLWC_MATCHING = 1,
  CURLWC_DOWNLOADING = 2,
  CURLWC_CLEAN = 3,
  CURLWC_SKIP = 4,
  CURLWC_ERROR = 5,
  CURLWC_DONE = 6
} curl_wildcard_states;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/wildcard.h:40 */
typedef void (*curl_wildcard_tmp_dtor)(void *);

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/wildcard.h:43 */
struct WildcardData {
  curl_wildcard_states state; /* 0x0 */
  char *path; /* 0x8 */
  char *pattern; /* 0x10 */
  struct curl_llist *filelist; /* 0x18 */
  void *tmp; /* 0x20 */
  curl_wildcard_tmp_dtor tmp_dtor; /* 0x28 */
  void *customptr; /* 0x30 */
};

_Static_assert(sizeof(struct WildcardData) == 56, "WildcardData size");
_Static_assert(offsetof(struct WildcardData, state) == 0, "WildcardData.state");
_Static_assert(offsetof(struct WildcardData, path) == 8, "WildcardData.path");
_Static_assert(offsetof(struct WildcardData, pattern) == 16, "WildcardData.pattern");
_Static_assert(offsetof(struct WildcardData, filelist) == 24, "WildcardData.filelist");
_Static_assert(offsetof(struct WildcardData, tmp) == 32, "WildcardData.tmp");
_Static_assert(offsetof(struct WildcardData, tmp_dtor) == 40, "WildcardData.tmp_dtor");
_Static_assert(offsetof(struct WildcardData, customptr) == 48, "WildcardData.customptr");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_WILDCARD_H */
