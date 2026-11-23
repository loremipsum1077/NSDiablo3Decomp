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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_SPLAY_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_SPLAY_H

#include <stddef.h>
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/bits/alltypes.h"

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/splay.h:26 */
struct Curl_tree {
  struct Curl_tree *smaller; /* 0x0 */
  struct Curl_tree *larger; /* 0x8 */
  struct Curl_tree *same; /* 0x10 */
  struct timeval key; /* 0x18 */
  void *payload; /* 0x28 */
};

_Static_assert(sizeof(struct Curl_tree) == 48, "Curl_tree size");
_Static_assert(offsetof(struct Curl_tree, smaller) == 0, "Curl_tree.smaller");
_Static_assert(offsetof(struct Curl_tree, larger) == 8, "Curl_tree.larger");
_Static_assert(offsetof(struct Curl_tree, same) == 16, "Curl_tree.same");
_Static_assert(offsetof(struct Curl_tree, key) == 24, "Curl_tree.key");
_Static_assert(offsetof(struct Curl_tree, payload) == 40, "Curl_tree.payload");

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_SPLAY_H */
