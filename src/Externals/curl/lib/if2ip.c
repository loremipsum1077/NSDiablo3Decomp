/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the permission notice below.
 * The terms are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the permission notice.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* The original Switch owner selects curl's no-IPv6/no-interface-lookup
 * implementations. The signatures and enum values are present in its DWARF;
 * these upstream platform fallbacks perform no address or buffer access.
 * Reference: https://github.com/curl/curl/blob/curl-7_50_3/lib/if2ip.c
 */
struct sockaddr;

#define IPV6_SCOPE_GLOBAL 0

typedef enum {
  IF2IP_NOT_FOUND = 0,
  IF2IP_AF_NOT_SUPPORTED = 1,
  IF2IP_FOUND = 2
} if2ip_result_t;

/* Return the scope of the given address. */
unsigned int Curl_ipv6_scope(const struct sockaddr *sa)
{
  (void) sa;
  return IPV6_SCOPE_GLOBAL;
}

if2ip_result_t Curl_if2ip(int af, unsigned int remote_scope,
                        unsigned int remote_scope_id, const char *interf,
                        char *buf, int buf_size)
{
  (void) af;
  (void) remote_scope;
  (void) remote_scope_id;
  (void) interf;
  (void) buf;
  (void) buf_size;
  return IF2IP_NOT_FOUND;
}

/* COPYRIGHT AND PERMISSION NOTICE
 *
 * Copyright (c) 1996 - 2016, Daniel Stenberg, <daniel@haxx.se>, and many
 * contributors, see the THANKS file.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 * Except as contained in this notice, the name of a copyright holder shall not
 * be used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization of the copyright holder.
 */
