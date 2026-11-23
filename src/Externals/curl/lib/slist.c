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

#include <stddef.h>

/* curl/curl.h:2083 in the original DWARF; local to this isolated owner. */
struct curl_slist {
  char *data;
  struct curl_slist *next;
};
_Static_assert(sizeof(struct curl_slist) == 16, "curl_slist size");
_Static_assert(offsetof(struct curl_slist, data) == 0, "curl_slist data");
_Static_assert(offsetof(struct curl_slist, next) == 8, "curl_slist next");

/* curl_nintendo_allocator.c:80,85,95: authenticated SDK allocator wrappers. */
extern void *Curl_SigloMalloc(size_t sizeToAllocate);
extern void Curl_SigloFree(void *pointer);
extern char *Curl_SigloStrdup(const char *string);

/* returns last node in linked list */
static struct curl_slist *slist_get_last(struct curl_slist *list)
{
  struct curl_slist     *item;

  /* if caller passed us a NULL, return now */
  if(!list)
    return NULL;

  /* loop through to find the last item */
  item = list;
  while(item->next) {
    item = item->next;
  }
  return item;
}

/*
 * Curl_slist_append_nodup() appends a string to the linked list. Rather than
 * copying the string in dynamic storage, it takes its ownership. The string
 * should have been malloc()ated. Curl_slist_append_nodup always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function.
 * If an error occurs, NULL is returned and the string argument is NOT
 * released.
 */
struct curl_slist *Curl_slist_append_nodup(struct curl_slist *list, char *data)
{
  struct curl_slist     *last;
  struct curl_slist     *new_item;

  /* DEBUGASSERT(data) is absent in the original release build. */

  new_item = Curl_SigloMalloc(sizeof(struct curl_slist));
  if(!new_item)
    return NULL;

  new_item->next = NULL;
  new_item->data = data;

  /* if this is the first item, then new_item *is* the list */
  if(!list)
    return new_item;

  last = slist_get_last(list);
  last->next = new_item;
  return list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then simply create a separate _init function and call it
 * appropriately from within the program.
 */
struct curl_slist *curl_slist_append(struct curl_slist *list,
                                     const char *data)
{
  char *dupdata = Curl_SigloStrdup(data);

  if(!dupdata)
    return NULL;

  list = Curl_slist_append_nodup(list, dupdata);
  if(!list)
    Curl_SigloFree(dupdata);

  return list;
}

/* Curl_slist_duplicate has no original emitted identity in this owner. */

/* be nice and clean up resources */
void curl_slist_free_all(struct curl_slist *list)
{
  struct curl_slist     *next;
  struct curl_slist     *item;

  if(!list)
    return;

  item = list;
  do {
    next = item->next;
    Curl_SigloFree(item->data);
    item->data = NULL;
    Curl_SigloFree(item);
    item = next;
  } while(next);
}


/*
 * COPYRIGHT AND PERMISSION NOTICE
 *
 * Copyright (c) 1996 - 2016, Daniel Stenberg, <daniel@haxx.se>, and many
 * contributors, see the THANKS file.
 *
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall not
 * be used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization of the copyright holder.
 */
