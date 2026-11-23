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


#include "bundles.h"
#include "llist.h"
#include "urldata.h"

/* Nintendo allocator signatures authenticated from their original CU. */
extern void *Curl_SigloMalloc(size_t sizeToAllocate);
extern void Curl_SigloFree(void *pointer);

_Static_assert(sizeof(void *) == 8, "original pointers");
_Static_assert(sizeof(long) == 8, "original long");
_Static_assert(sizeof(int) == 4, "original int");
_Static_assert(sizeof(_Bool) == 1, "original bool");

/* Recovered from original instructions and DWARF; see packet provenance. */


static void conn_llist_dtor(void *user, void *element)
{
  struct connectdata *data = element;
  (void)user;

  data->bundle = NULL;
}

CURLcode Curl_bundle_create(struct SessionHandle *data,
                            struct connectbundle **cb_ptr)
{
  (void)data;

  *cb_ptr = Curl_SigloMalloc(sizeof(struct connectbundle));
  if(!*cb_ptr)
    return CURLE_OUT_OF_MEMORY;

  (*cb_ptr)->num_connections = 0;
  (*cb_ptr)->server_supports_pipelining = 0;

  (*cb_ptr)->conn_list = Curl_llist_alloc(conn_llist_dtor);
  if(!(*cb_ptr)->conn_list) {
    Curl_SigloFree(*cb_ptr); *cb_ptr = NULL;
    return CURLE_OUT_OF_MEMORY;
  }
  return CURLE_OK;
}

void Curl_bundle_destroy(struct connectbundle *cb_ptr)
{
  if(!cb_ptr)
    return;

  if(cb_ptr->conn_list) {
    Curl_llist_destroy(cb_ptr->conn_list, NULL);
    cb_ptr->conn_list = NULL;
  }
  Curl_SigloFree(cb_ptr);
}


CURLcode Curl_bundle_add_conn(struct connectbundle *cb_ptr,
                             struct connectdata *conn)
{
  if(!Curl_llist_insert_next(cb_ptr->conn_list, cb_ptr->conn_list->tail, conn))
    return CURLE_OUT_OF_MEMORY;

  conn->bundle = cb_ptr;

  cb_ptr->num_connections++;
  return CURLE_OK;
}


int Curl_bundle_remove_conn(struct connectbundle *cb_ptr,
                            struct connectdata *conn)
{
  struct curl_llist_element *curr;

  curr = cb_ptr->conn_list->head;
  while(curr) {
    if(curr->ptr == conn) {
      Curl_llist_remove(cb_ptr->conn_list, curr, NULL);
      cb_ptr->num_connections--;
      conn->bundle = NULL;
      return 1;
    }
    curr = curr->next;
  }
  return 0;
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
