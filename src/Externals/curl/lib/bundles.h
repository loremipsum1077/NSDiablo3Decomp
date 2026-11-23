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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_BUNDLES_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_BUNDLES_H

#include <stddef.h>
#include <stdint.h>

struct curl_llist;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/bundles.h:25; DIE 0x7c926e3 */
struct connectbundle {
  _Bool server_supports_pipelining; /* 0x0 */
  size_t num_connections; /* 0x8 */
  struct curl_llist *conn_list; /* 0x10 */
};

_Static_assert(sizeof(struct connectbundle) == 24, "connectbundle size");
_Static_assert(offsetof(struct connectbundle, server_supports_pipelining) == 0, "connectbundle.server_supports_pipelining");
_Static_assert(offsetof(struct connectbundle, num_connections) == 8, "connectbundle.num_connections");
_Static_assert(offsetof(struct connectbundle, conn_list) == 16, "connectbundle.conn_list");

#include "Programs/Eris/Outputs/Include/curl/curl.h"
struct SessionHandle;
struct connectdata;
CURLcode Curl_bundle_create(struct SessionHandle *data, struct connectbundle **cb_ptr);
void Curl_bundle_destroy(struct connectbundle *cb_ptr);
CURLcode Curl_bundle_add_conn(struct connectbundle *cb_ptr, struct connectdata *conn);
int Curl_bundle_remove_conn(struct connectbundle *cb_ptr, struct connectdata *conn);

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_BUNDLES_H */
