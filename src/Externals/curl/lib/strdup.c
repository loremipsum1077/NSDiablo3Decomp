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
#include <string.h>

#include "curl_memory.h"

/* LINKER-REMOVED: the original has no copy of Curl_memdup and no DWARF for strdup.c, but its
 * Curl_dupset (url.c, also linker-removed) calls it. Written as upstream curl-7_50_3 strdup.c has
 * it, with the prototype url.c declares. curlx_strdup is left out: this target has strdup. */

/***************************************************************************
 *
 * Curl_memdup(source, length)
 *
 * Copies the 'source' data to a newly allocated buffer (that is
 * returned). Copies 'length' bytes.
 *
 * Returns the new pointer or NULL on failure.
 *
 ***************************************************************************/
char *Curl_memdup(const char *src, size_t length)
{
  char *buffer = malloc(length);
  if(!buffer)
    return NULL; /* fail */

  memcpy(buffer, src, length);

  return buffer;
}
