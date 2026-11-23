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

/* Reconstructed from this binary's instructions and DWARF. */
#include "Externals/curl/lib/urldata.h"
#include "Externals/curl/lib/share.h"




































































































































































































CURLSHcode Curl_share_lock(struct SessionHandle *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  struct Curl_share *share = data->share;

  if(!share)
    return CURLSHE_INVALID;

  if(share->specifier & (1U << type)) {
    if(share->lockfunc)
      share->lockfunc(data, type, accesstype, share->clientdata);
  }



  return CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(struct SessionHandle *data, curl_lock_data type)
{
  struct Curl_share *share = data->share;

  if(!share)
    return CURLSHE_INVALID;

  if(share->specifier & (1U << type)) {
    if(share->unlockfunc)
      share->unlockfunc(data, type, share->clientdata);
  }


  return CURLSHE_OK;
}
