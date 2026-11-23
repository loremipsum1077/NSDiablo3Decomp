#ifndef HEADER_CURL_MEMORY_H
#define HEADER_CURL_MEMORY_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
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

/*
 * The SDK build routes libcurl's five memory functions straight to the
 * allocator table in siglo/curl_nintendo_allocator.c: every call site in
 * the binary calls Curl_SigloMalloc and friends directly.
 */

#include <stddef.h>

void *Curl_SigloMalloc(size_t sizeToAllocate);
void Curl_SigloFree(void *pointer);
void *Curl_SigloRealloc(void *ptr, size_t size);
char *Curl_SigloStrdup(const char *string);
void *Curl_SigloCalloc(size_t nmemb, size_t size);

#undef strdup
#define strdup(ptr) Curl_SigloStrdup(ptr)
#undef malloc
#define malloc(size) Curl_SigloMalloc(size)
#undef calloc
#define calloc(nbelem,size) Curl_SigloCalloc(nbelem, size)
#undef realloc
#define realloc(ptr,size) Curl_SigloRealloc(ptr, size)
#undef free
#define free(ptr) Curl_SigloFree(ptr)

#endif /* HEADER_CURL_MEMORY_H */
