#ifndef HEADER_CURL_MD5_H
#define HEADER_CURL_MD5_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2010, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#ifndef CURL_DISABLE_CRYPTO_AUTH

#define MD5_DIGEST_LEN  16

typedef void (* Curl_MD5_init_func)(void *context);
typedef void (* Curl_MD5_update_func)(void *context,
                                      const unsigned char *data,
                                      unsigned int len);
typedef void (* Curl_MD5_final_func)(unsigned char *result, void *context);

typedef struct {
  Curl_MD5_init_func     md5_init_func;   /* Initialize context procedure */
  Curl_MD5_update_func   md5_update_func; /* Update context with data */
  Curl_MD5_final_func    md5_final_func;  /* Get final result procedure */
  unsigned int           md5_ctxtsize;  /* Context structure size */
  unsigned int           md5_resultlen; /* Result length (bytes) */
} MD5_params;

typedef struct {
  const MD5_params      *md5_hash;      /* Hash function definition */
  void                  *md5_hashctx;   /* Hash function context */
} MD5_context;

extern const MD5_params Curl_DIGEST_MD5[1];
/* Curl_HMAC_MD5 (curl_hmac.h) is left out until curl_hmac.h is written. */

void Curl_md5it(unsigned char *output,
                const unsigned char *input);

MD5_context * Curl_MD5_init(const MD5_params *md5params);
int Curl_MD5_update(MD5_context *context,
                    const unsigned char *data,
                    unsigned int len);
int Curl_MD5_final(MD5_context *context, unsigned char *result);

#endif


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(MD5_params) == 0x20, "MD5_params");
_Static_assert(offsetof(MD5_params, md5_init_func) == 0x0, "MD5_params.md5_init_func");
_Static_assert(offsetof(MD5_params, md5_update_func) == 0x8, "MD5_params.md5_update_func");
_Static_assert(offsetof(MD5_params, md5_final_func) == 0x10, "MD5_params.md5_final_func");
_Static_assert(offsetof(MD5_params, md5_ctxtsize) == 0x18, "MD5_params.md5_ctxtsize");
_Static_assert(offsetof(MD5_params, md5_resultlen) == 0x1c, "MD5_params.md5_resultlen");
_Static_assert(sizeof(MD5_context) == 0x10, "MD5_context");
_Static_assert(offsetof(MD5_context, md5_hash) == 0x0, "MD5_context.md5_hash");
_Static_assert(offsetof(MD5_context, md5_hashctx) == 0x8, "MD5_context.md5_hashctx");

#endif /* HEADER_CURL_MD5_H */
