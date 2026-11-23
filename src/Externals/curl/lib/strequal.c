/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2013, Daniel Stenberg, <daniel@haxx.se>, et al.
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

#include <stddef.h> /* curl_setup.h is not in this tree; it supplied size_t */

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "NintendoSDK/Compilers/NX/nx/aarch64/include/ctype.h" /* SDK toupper */

/*
 * @unittest: 1301 */

int curl_strequal(const char *first, const char *second)
{
#if defined(HAVE_STRCASECMP)
  return !(strcasecmp)(first, second);
#elif defined(HAVE_STRCMPI)
  return !(strcmpi)(first, second);
#elif defined(HAVE_STRICMP)
  return !(stricmp)(first, second);
#else
  /* The shipped copy has two more lines above this loop than upstream
     curl-7_47_1 (line table: loop at 45, curl_strnequal at 59). */
  while(*first && *second) {
    if(toupper(*first) != toupper(*second)) {
      break;
    }
    first++;
    second++;
  }
  return toupper(*first) == toupper(*second);
#endif
}

/*
 * @unittest: 1301
 */
int curl_strnequal(const char *first, const char *second, size_t max)
{
#if defined(HAVE_STRNCASECMP)
  return !strncasecmp(first, second, max);
#elif defined(HAVE_STRNCMPI)
  return !strncmpi(first, second, max);
#elif defined(HAVE_STRNICMP)
  return !strnicmp(first, second, max);
#else
  while(*first && *second && max) {
    if(toupper(*first) != toupper(*second)) {
      break;
    }
    max--;
    first++;
    second++;
  }
  if(0 == max)
    return 1; /* they are equal this far */

  return toupper(*first) == toupper(*second);
#endif
}
