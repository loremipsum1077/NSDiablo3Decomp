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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_LLIST_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_LLIST_H

#include <stddef.h>
#include <stdint.h>

struct curl_llist_element;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/llist.h:28; DIE 0x7c8fad1 */
typedef void (*curl_llist_dtor)(void *, void *);


/* D:/home/teamcity2/work/sdk/Externals/curl/lib/llist.h:30; DIE 0x7c8faa4 */
struct curl_llist_element {
  void *ptr; /* 0x0 */
  struct curl_llist_element *prev; /* 0x8 */
  struct curl_llist_element *next; /* 0x10 */
};

_Static_assert(sizeof(struct curl_llist_element) == 24, "curl_llist_element size");
_Static_assert(offsetof(struct curl_llist_element, ptr) == 0, "curl_llist_element.ptr");
_Static_assert(offsetof(struct curl_llist_element, prev) == 8, "curl_llist_element.prev");
_Static_assert(offsetof(struct curl_llist_element, next) == 16, "curl_llist_element.next");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/llist.h:37; DIE 0x7c8fa66 */
struct curl_llist {
  struct curl_llist_element *head; /* 0x0 */
  struct curl_llist_element *tail; /* 0x8 */
  curl_llist_dtor dtor; /* 0x10 */
  size_t size; /* 0x18 */
};

_Static_assert(sizeof(struct curl_llist) == 32, "curl_llist size");
_Static_assert(offsetof(struct curl_llist, head) == 0, "curl_llist.head");
_Static_assert(offsetof(struct curl_llist, tail) == 8, "curl_llist.tail");
_Static_assert(offsetof(struct curl_llist, dtor) == 16, "curl_llist.dtor");
_Static_assert(offsetof(struct curl_llist, size) == 24, "curl_llist.size");

struct curl_llist *Curl_llist_alloc(curl_llist_dtor dtor);
void Curl_llist_destroy(struct curl_llist *list, void *user);
int Curl_llist_insert_next(struct curl_llist *list, struct curl_llist_element *e, const void *p);
int Curl_llist_remove(struct curl_llist *list, struct curl_llist_element *e, void *user);

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_LLIST_H */
