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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_CONNCACHE_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_CONNCACHE_H

#include <stddef.h>
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/bits/alltypes.h"
#include "Programs/Eris/Outputs/Include/curl/curl.h"

struct curl_hash;
struct connectbundle;
struct connectdata;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/conncache.h:26 */
struct conncache {
  struct curl_hash *hash; /* 0x0 */
  size_t num_connections; /* 0x8 */
  long int next_connection_id; /* 0x10 */
  struct timeval last_cleanup; /* 0x18 */
};

_Static_assert(sizeof(struct conncache) == 40, "conncache size");
_Static_assert(offsetof(struct conncache, hash) == 0, "conncache.hash");
_Static_assert(offsetof(struct conncache, num_connections) == 8, "conncache.num_connections");
_Static_assert(offsetof(struct conncache, next_connection_id) == 16, "conncache.next_connection_id");
_Static_assert(offsetof(struct conncache, last_cleanup) == 24, "conncache.last_cleanup");

struct conncache *Curl_conncache_init(int size);
void Curl_conncache_destroy(struct conncache *connc);
struct connectbundle *Curl_conncache_find_bundle(struct connectdata *conn,
                                                 struct conncache *connc);
CURLcode Curl_conncache_add_conn(struct conncache *connc,
                                 struct connectdata *conn);
void Curl_conncache_remove_conn(struct conncache *connc,
                                struct connectdata *conn);
void Curl_conncache_foreach(struct conncache *connc,
                            void *param,
                            int (*func)(struct connectdata *conn,
                                        void *param));
struct connectdata *
Curl_conncache_find_first_connection(struct conncache *connc);

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_CONNCACHE_H */
