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
 * The SDK's easy.c: upstream 7.42.1 with Nintendo's changes. Only the
 * functions the binary keeps are written; each sits on the line the
 * original DWARF gives it. The SDK build has no win32, Amiga, NetWare,
 * libidn or libssh2 support, and no CURLDEBUG event-based transfers.
 */

#include <stdarg.h>
#include "Externals/curl/lib/urldata.h"

/* Prototypes from url.h, getinfo.h, hostip.h, vtls.h, connect.h and the
 * siglo glue; kept local like the other curl files. */
#define bool _Bool
#define TRUE 1
#define FALSE 0
#define CURL_GLOBAL_SSL (1<<0)
#define CURL_GLOBAL_WIN32 (1<<1)
#define CURL_GLOBAL_DEFAULT (CURL_GLOBAL_SSL|CURL_GLOBAL_WIN32)
#define CURL_GLOBAL_ACK_EINTR (1<<2)
/* CURLoption: the enum in curl.h; only passed through here */
typedef int CURLINFO;   /* enum in curl.h; only passed through here */
typedef void *(*curl_malloc_callback)(size_t size);
typedef void (*curl_free_callback)(void *ptr);
typedef void *(*curl_realloc_callback)(void *ptr, size_t size);
typedef char *(*curl_strdup_callback)(const char *str);
typedef void *(*curl_calloc_callback)(size_t nmemb, size_t size);
extern int Curl_ack_eintr;
CURLcode Curl_open(struct SessionHandle **curl);
CURLcode Curl_close(struct SessionHandle *data);
CURLcode Curl_setopt(struct SessionHandle *data, CURLoption option,
                     va_list param);
CURLcode Curl_getinfo(struct SessionHandle *data, CURLINFO info, ...);
void Curl_global_host_cache_dtor(void);
int Curl_ssl_init(void);
void Curl_ssl_cleanup(void);
int Curl_resolver_global_init(void);
void Curl_resolver_global_cleanup(void);
int Curl_SigloAllocatorInitializeDefaults(void);
int Curl_SigloAllocatorInitialize(curl_malloc_callback m, curl_free_callback f,
                                  curl_realloc_callback r,
                                  curl_strdup_callback s,
                                  curl_calloc_callback c);
void Curl_SigloMiddlewareInfo(void);
struct Curl_SigloThreadGC *Curl_SigloThreadGCInitialize(void);
void Curl_SigloThreadGCFinalize(void);











































































































static unsigned int  initialized;
static long          init_flags;

static CURLcode curl_global_init_internal(bool calledFromInitMem, long flags);

/**
 * curl_global_init() globally initializes cURL given a bitwise set of the
 * different features of what to initialize.
 */
















































CURLcode curl_global_init(long flags)
{
  if(initialized++)
    return CURLE_OK;

  /* Setup the default memory functions here (again) */









  Curl_SigloAllocatorInitializeDefaults();
  return curl_global_init_internal(FALSE, flags);
}

static CURLcode curl_global_init_internal(bool calledFromInitMem, long flags)
{
  Curl_SigloMiddlewareInfo();

  if(calledFromInitMem && initialized++)
    return CURLE_OK;

  /* the thread garbage collector must run before any resolver thread */
  if(!Curl_SigloThreadGCInitialize())
    return CURLE_FAILED_INIT;




  if(flags & CURL_GLOBAL_SSL)
    if(!Curl_ssl_init()) {
      return CURLE_FAILED_INIT;
    }


























  if(Curl_resolver_global_init()) {
    return CURLE_FAILED_INIT;
  }









  if(flags & CURL_GLOBAL_ACK_EINTR)
    Curl_ack_eintr = 1;

  init_flags  = flags;

  return CURLE_OK;
}

/*
 * curl_global_init_mem() globally initializes cURL and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(long flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode result = CURLE_OK;

  /* Invalid input, return immediately */
  if(!m || !f || !r || !s || !c)
    return CURLE_FAILED_INIT;

  if(initialized) {
    /* Already initialized, don't do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    return CURLE_OK;
  }

  /* Install the callbacks before anything allocates */
  Curl_SigloAllocatorInitialize(m, f, r, s, c);

  /* Call the actual init function first */

  result = curl_global_init_internal(TRUE, flags);

  return result;
}

/**
 * curl_global_cleanup() globally cleanups cURL, uses the value of
 * "init_flags" to determine what needs to be cleaned up and what doesn't.
 */









void curl_global_cleanup(void)
{
  if(!initialized)
    return;

  if(--initialized)
    return;

  /* stop the thread garbage collector first */
  Curl_SigloThreadGCFinalize();


  Curl_global_host_cache_dtor();

  if(init_flags & CURL_GLOBAL_SSL)
    Curl_ssl_cleanup();

  Curl_resolver_global_cleanup();










  init_flags  = 0;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(void)
{
  CURLcode result;
  struct SessionHandle *data;

  /* Make sure we inited the global SSL stuff */
  if(!initialized) {
    result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if(result) {
      /* something in the global init failed, return nothing */
      return NULL;
    }
  }

  /* We use curl_open() with undefined URL so far */

  result = Curl_open(&data);
  if(result) {
    return NULL;
  }

  return data;
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 */

#undef curl_easy_setopt

CURLcode curl_easy_setopt(CURL *curl, CURLoption tag, ...)
{
  va_list arg;
  struct SessionHandle *data = curl;
  CURLcode result;

  if(!curl)
    return CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, tag);

  result = Curl_setopt(data, tag, arg);

  va_end(arg);
  return result;
}

















































































































































































































































































































































































































































/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
void curl_easy_cleanup(CURL *curl)
{
  struct SessionHandle *data = (struct SessionHandle *)curl;

  if(!data)
    return;


  Curl_close(data);

}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#undef curl_easy_getinfo

CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  va_list arg;
  void *paramp;
  CURLcode result;
  struct SessionHandle *data = (struct SessionHandle *)curl;

  va_start(arg, info);
  paramp = va_arg(arg, void *);

  result = Curl_getinfo(data, info, paramp);

  va_end(arg);
  return result;
}
