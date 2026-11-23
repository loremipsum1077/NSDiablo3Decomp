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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_HTTP_CHUNKS_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_HTTP_CHUNKS_H

#include <stddef.h>
#include <stdint.h>
#include "Programs/Eris/Outputs/Include/curl/curlbuild.h"


/* D:/home/teamcity2/work/sdk/Externals/curl/lib/http_chunks.h:66; DIE 0x7c91a5c */
typedef enum {
  CHUNK_HEX = 0,
  CHUNK_LF = 1,
  CHUNK_DATA = 2,
  CHUNK_POSTLF = 3,
  CHUNK_STOP = 4,
  CHUNK_TRAILER = 5,
  CHUNK_TRAILER_CR = 6,
  CHUNK_TRAILER_POSTCR = 7
} ChunkyState;

_Static_assert(sizeof(ChunkyState) == 4, "ChunkyState size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/http_chunks.h:82; DIE 0x7c91a0b */
struct Curl_chunker {
  char hexbuffer[17]; /* 0x0 */
  int hexindex; /* 0x14 */
  ChunkyState state; /* 0x18 */
  curl_off_t datasize; /* 0x20 */
  size_t dataleft; /* 0x28 */
};

_Static_assert(sizeof(struct Curl_chunker) == 48, "Curl_chunker size");
_Static_assert(offsetof(struct Curl_chunker, hexbuffer) == 0, "Curl_chunker.hexbuffer");
_Static_assert(offsetof(struct Curl_chunker, hexindex) == 20, "Curl_chunker.hexindex");
_Static_assert(offsetof(struct Curl_chunker, state) == 24, "Curl_chunker.state");
_Static_assert(offsetof(struct Curl_chunker, datasize) == 32, "Curl_chunker.datasize");
_Static_assert(offsetof(struct Curl_chunker, dataleft) == 40, "Curl_chunker.dataleft");

/* Added for transfer.c (http_chunks.h:93-105, http.h:91-94). */
typedef enum {
  CHUNKE_STOP = -1,
  CHUNKE_OK = 0,
  CHUNKE_TOO_LONG_HEX = 1,
  CHUNKE_ILLEGAL_HEX,
  CHUNKE_BAD_CHUNK,
  CHUNKE_WRITE_ERROR,
  CHUNKE_BAD_ENCODING,
  CHUNKE_OUT_OF_MEMORY,
  CHUNKE_LAST
} CHUNKcode;

const char *Curl_chunked_strerror(CHUNKcode code);

struct connectdata;
CHUNKcode Curl_httpchunk_read(struct connectdata *conn, char *datap,
                              ssize_t length, ssize_t *wrote);

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_HTTP_CHUNKS_H */
