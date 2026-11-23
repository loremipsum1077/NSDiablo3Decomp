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

#include "Programs/Eris/Outputs/Include/curl/curlbuild.h"
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/bits/alltypes.h"

/* The SDK reads the time through its shim (siglo/curl_nintendo_shim.c). */
struct timeval nnCurlShim_get_timeval(struct timeval *ptime);























struct timeval curlx_tvnow(void)
{
  /*
  ** The system tick is monotonic; the shim converts it to a timeval.
  */

  return nnCurlShim_get_timeval(NULL);
}
































































/*
 * Make sure that the first argument is the more recent time, as otherwise
 * we'll get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds.
 */
long curlx_tvdiff(struct timeval newer, struct timeval older)
{
  return (newer.tv_sec-older.tv_sec)*1000+
    (long)(newer.tv_usec-older.tv_usec)/1000;
}

/*
 * Same as curlx_tvdiff but with full usec resolution.
 *
 * Returns: the time difference in seconds with subsecond resolution.
 */
double curlx_tvdiff_secs(struct timeval newer, struct timeval older)
{
  if(newer.tv_sec != older.tv_sec)
    return (double)(newer.tv_sec-older.tv_sec)+
      (double)(newer.tv_usec-older.tv_usec)/1000000.0;
  else
    return (double)(newer.tv_usec-older.tv_usec)/1000000.0;
}

/* return the number of seconds in the given input timeval struct */
long Curl_tvlong(struct timeval t1)
{
  return t1.tv_sec;
}
