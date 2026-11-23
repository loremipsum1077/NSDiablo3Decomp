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

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "Externals/curl/lib/urldata.h"

/* Prototypes from sendf.h, hostip.h, multiif.h, inet_pton.h,
 * curl_addrinfo.h, timeval.h and curl_threads.h (siglo), kept local like
 * the other curl files; and the nn::os / nn::socket C API this port uses. */
#define failf Curl_failf
#define Curl_tvnow() curlx_tvnow()
#define Curl_tvdiff(x,y) curlx_tvdiff(x,y)
#define connclose(x,y) ((x)->bits.close = TRUE)
#define TRUE 1
#define FALSE 0
#define CURL_ASYNC_SUCCESS 0
#define RESOLVER_ENOMEM ENOMEM
#define SOCKERRNO nnsocketGetLastErrno()
#define SET_ERRNO(x) (errno = (x))
#define AF_INET 2
#define curl_thread_t_null NULL
struct in_addr { uint32_t s_addr; };
void Curl_failf(struct SessionHandle *, const char *fmt, ...);
struct timeval curlx_tvnow(void);
long curlx_tvdiff(struct timeval t1, struct timeval t2);
void Curl_expire(struct SessionHandle *data, long milli);
int Curl_inet_pton(int af, const char *src, void *dst);
Curl_addrinfo *Curl_ip2addr(int af, const void *inaddr, const char *hostname,
                            int port);
void Curl_freeaddrinfo(Curl_addrinfo *cahead);
CURLcode Curl_addrinfo_callback(struct connectdata *conn, int status,
                                Curl_addrinfo *ai);
typedef union nnosMutexType { uint8_t buf[32]; void *alignment_holder; } nnosMutexType;
typedef enum nnsocketResolverOptionKey {
  nnsocketResolverOptionKey_GetCancelHandleInteger = 10001,
  nnsocketResolverOptionKey_SetCancelHandleInteger = 20001,
  nnsocketResolverOptionKey_RequestEnableServiceDiscoveryBoolean = 30001,
  nnsocketResolverOptionKey_RequestCancelHandleInteger = 30002,
  nnsocketResolverOptionKey_RequestEnableDnsCacheBoolean = 30003
} nnsocketResolverOptionKey;
typedef enum nnsocketResolverOptionType {
  nnsocketResolverOptionType_Boolean = 100,
  nnsocketResolverOptionType_Integer = 101
} nnsocketResolverOptionType;
typedef struct nnsocketResolverOption {
  nnsocketResolverOptionKey key;
  nnsocketResolverOptionType type;
  size_t size;
  union {
    _Bool booleanValue; int integerValue; uint32_t unsigned32Value;
    uint64_t unsigned64Value; double doubleValue; const char *pointerValue;
  } data;
} nnsocketResolverOption;
void nnosInitializeMutex(nnosMutexType *mutex, _Bool recursive, int lockLevel);
void nnosFinalizeMutex(nnosMutexType *mutex);
void nnosLockMutex(nnosMutexType *mutex);
void nnosUnlockMutex(nnosMutexType *mutex);
void nnosSleepThread(int64_t nanoSeconds);
int nnsocketGetLastErrno(void);
int nnsocketResolverGetOption(nnsocketResolverOption *option,
                              nnsocketResolverOptionKey key);
int nnsocketResolverSetOption(const nnsocketResolverOption *option);
Curl_addrinfo *Curl_ipv4_resolve_r(const char *hostname, int port,
                                   nnsocketResolverOption *options,
                                   size_t optionCount);
typedef struct Curl_SigloThreadContext Curl_SigloThreadContext;
Curl_SigloThreadContext *Curl_thread_create(unsigned int (*function)(void *),
                                            void *argument);
void Curl_thread_destroy(Curl_SigloThreadContext *context);
int Curl_thread_join(Curl_SigloThreadContext **hnd);
#include "Externals/curl/lib/curl_memory.h"
int Curl_resolver_global_init(void)
{
  return CURLE_OK;
}

/*
 * Curl_resolver_global_cleanup()
 * Called from curl_global_cleanup() to destroy global resolver environment.
 * Does nothing here.
 */
void Curl_resolver_global_cleanup(void)
{
}

/*
 * Curl_resolver_init()
 * Called from curl_easy_init() -> Curl_open() to initialize resolver
 * URL-state specific environment ('resolver' member of the UrlState
 * structure).  Does nothing here.
 */
CURLcode Curl_resolver_init(void **resolver)
{
  (void)resolver;
  return CURLE_OK;
}

/*
 * Curl_resolver_cleanup()
 * Called from curl_easy_cleanup() -> Curl_close() to cleanup resolver
 * URL-state specific environment ('resolver' member of the UrlState
 * structure).  Does nothing here.
 */
void Curl_resolver_cleanup(void *resolver)
{
  (void)resolver;
}

/*
 * Curl_resolver_duphandle()
 * Called from curl_easy_duphandle() to duplicate resolver URL state-specific
 * environment ('resolver' member of the UrlState structure).  Does nothing
 * here.
 */
int Curl_resolver_duphandle(void **to, void *from)
{
  (void)to;
  (void)from;
  return CURLE_OK;
}

static void destroy_async_data(struct Curl_async *);

/* Data for synchronization between resolver thread and its parent */







struct thread_sync_data {
  nnosMutexType * mtx;
  int done;

  char * hostname;        /* hostname to resolve, Curl_async.hostname
                             duplicate */
  int port;
  int sock_error;
  Curl_addrinfo *res;

  nnsocketResolverOption resolverOptions[3]; /* service discovery,
                                                cancel handle and DNS cache
                                                options passed to the
                                                resolver */

  struct thread_data *td; /* for thread-self cleanup */
};

struct thread_data {
  Curl_SigloThreadContext *thread_hnd;
  unsigned int poll_interval;
  long interval_end;
  struct thread_sync_data tsd;
};

const unsigned int NSD_RESOLVER_OPTION_INDEX = 0;
const unsigned int CANCEL_RESOLVER_OPTION_INDEX = 1;
const unsigned int CACHE_RESOLVER_OPTION_INDEX = 2;

/*
 * Curl_resolver_cancel() is called when someone gives up trying to
 * resolve the host name: it cancels the request in the resolver if one is
 * running, and frees the async data.
 */

void Curl_resolver_cancel(struct connectdata *conn)
{
  int cancel_handle;
  struct thread_data *td = (struct thread_data*) conn->async.os_specific;

  if(td) {
    /* the resolver handed out a cancel handle when the thread was set up */


    cancel_handle =
      td->tsd.resolverOptions[CANCEL_RESOLVER_OPTION_INDEX].data.integerValue;
    if(cancel_handle != 0) {
      nnsocketResolverOption option;


      option.key = nnsocketResolverOptionKey_SetCancelHandleInteger;
      option.type = nnsocketResolverOptionType_Integer;
      option.size = sizeof(int);

      option.data.integerValue = cancel_handle;

      /* retry while the resolver is busy */
      while(nnsocketResolverSetOption(&option) == -1 &&
            nnsocketGetLastErrno() == EAGAIN) {
        nnosSleepThread(10 * 1000 * 1000); /* 10 ms */
      }
    }
  }

  destroy_async_data(&conn->async);
}

/* Get resolver thread sync data for the connection */
static
struct thread_sync_data *conn_thread_sync_data(struct connectdata *conn)
{
  return &(((struct thread_data *)conn->async.os_specific)->tsd);
}

/* Destroy resolver thread synchronization data */


static
void destroy_thread_sync_data(struct thread_sync_data * tsd)
{
  if(tsd->mtx) {
    nnosFinalizeMutex(tsd->mtx);
    free(tsd->mtx);
  }

  free(tsd->hostname);

  if(tsd->res)
    Curl_freeaddrinfo(tsd->res);

  memset(tsd, 0, sizeof(*tsd));
}

/* Initialize resolver thread synchronization data */







static
int init_thread_sync_data(struct thread_data * td,
                          const char * hostname,
                          int port,
                          struct connectdata *conn)
{
  unsigned int index;
  struct thread_sync_data *tsd = &td->tsd;

  /* zero out the whole struct, then fill in the parts that need values */


  memset(tsd, 0, sizeof(*tsd));

  tsd->td = td;
  tsd->port = port;

  /* the mutex is allocated: nnosMutexType must not move once initialized */
  tsd->mtx = malloc(sizeof(nnosMutexType));
  if(tsd->mtx == NULL)
    goto err_exit;

  nnosInitializeMutex(tsd->mtx, FALSE, 1);

  tsd->sock_error = CURL_ASYNC_SUCCESS;

  index = NSD_RESOLVER_OPTION_INDEX;
  tsd->resolverOptions[index].key =
    nnsocketResolverOptionKey_RequestEnableServiceDiscoveryBoolean;
  tsd->resolverOptions[index].type = nnsocketResolverOptionType_Boolean;
  tsd->resolverOptions[index].size = sizeof(_Bool);
  tsd->resolverOptions[index].data.booleanValue = FALSE;

  if(conn->data != NULL) {


    tsd->resolverOptions[index].data.booleanValue =
      !conn->data->set.disablensd;
  }

  index = CANCEL_RESOLVER_OPTION_INDEX;
  tsd->resolverOptions[index].key =
    nnsocketResolverOptionKey_GetCancelHandleInteger;
  tsd->resolverOptions[index].type = nnsocketResolverOptionType_Integer;
  tsd->resolverOptions[index].size = sizeof(int);
  nnsocketResolverGetOption(&tsd->resolverOptions[index],
                            nnsocketResolverOptionKey_GetCancelHandleInteger);
  tsd->resolverOptions[index].key =
    nnsocketResolverOptionKey_RequestCancelHandleInteger;

  index = CACHE_RESOLVER_OPTION_INDEX;
  tsd->resolverOptions[index].key =
    nnsocketResolverOptionKey_RequestEnableDnsCacheBoolean;
  tsd->resolverOptions[index].type = nnsocketResolverOptionType_Boolean;
  tsd->resolverOptions[index].size = sizeof(_Bool);
  tsd->resolverOptions[index].data.booleanValue = TRUE;

  if(conn->data != NULL) {


    tsd->resolverOptions[index].data.booleanValue =
      !conn->data->set.disablecache;
  }

  /* Copying hostname string because original can be destroyed by parent
   * thread during gethostbyname execution.
   */


  tsd->hostname = strdup(hostname);
  if(!tsd->hostname)
    goto err_exit;

  return 1;

 err_exit:
  /* Memory allocation failed */
  destroy_thread_sync_data(tsd);
  return 0;
}

static int getaddrinfo_complete(struct connectdata *conn)
{
  struct thread_sync_data *tsd = conn_thread_sync_data(conn);
  int rc;

  rc = Curl_addrinfo_callback(conn, tsd->sock_error, tsd->res);
  /* The tsd->res structure has been copied to async.dns and perhaps the DNS
     cache.  Set our copy to NULL so destroy_thread_sync_data doesn't free it.
  */
  tsd->res = NULL;

  return rc;
}











































/*
 * gethostbyname_thread() resolves a name and then exits.
 */

static unsigned int gethostbyname_thread (void *arg)
{
  struct thread_sync_data *tsd = (struct thread_sync_data *)arg;
  struct thread_data *td = tsd->td;
  nnsocketResolverOption options[3];

  memcpy(options, tsd->resolverOptions, sizeof(options));


  tsd->res = Curl_ipv4_resolve_r(tsd->hostname,
                                 tsd->port, options, 3);





  if(!tsd->res) {
    tsd->sock_error = SOCKERRNO;
    if(tsd->sock_error == 0)
      tsd->sock_error = RESOLVER_ENOMEM;
  }

  nnosLockMutex(tsd->mtx);
  if(tsd->done) {
    /* too late, gotta clean up the mess */
    nnosUnlockMutex(tsd->mtx);
    destroy_thread_sync_data(tsd);
    free(td);
  }
  else {
    tsd->done = 1;
    nnosUnlockMutex(tsd->mtx);
  }

  return 0;
}

/*
 * destroy_async_data() cleans up async resolver data and thread handle.
 */


static void destroy_async_data (struct Curl_async *async)
{
  if(async->os_specific) {
    struct thread_data *td = (struct thread_data*) async->os_specific;
    int done;

    /*
     * if the thread is still blocking in the resolve syscall, detach it and
     * let the thread do the cleanup...
     */
    nnosLockMutex(td->tsd.mtx);
    done = td->tsd.done;
    td->tsd.done = 1;
    nnosUnlockMutex(td->tsd.mtx);

    if(!done) {
      Curl_thread_destroy(td->thread_hnd);
    }
    else {
      if(td->thread_hnd != curl_thread_t_null)
        Curl_thread_join(&td->thread_hnd);

      destroy_thread_sync_data(&td->tsd);

      free(async->os_specific);
    }
  }
  async->os_specific = NULL;

  free(async->hostname);
  async->hostname = NULL;
}

/*
 * init_resolve_thread() starts a new thread that performs the actual
 * resolve. This function returns before the resolve is done.
 *
 * Returns FALSE in case of failure, otherwise TRUE.
 */
static _Bool init_resolve_thread (struct connectdata *conn,
                                 const char *hostname, int port)
{
  struct thread_data *td = calloc(1, sizeof(struct thread_data));
  int err = RESOLVER_ENOMEM;

  conn->async.os_specific = (void*) td;
  if(!td)
    goto err_exit;

  conn->async.port = port;
  conn->async.done = FALSE;
  conn->async.status = 0;
  conn->async.dns = NULL;
  td->thread_hnd = curl_thread_t_null;

  if(!init_thread_sync_data(td, hostname, port, conn))
    goto err_exit;

  free(conn->async.hostname);
  conn->async.hostname = strdup(hostname);
  if(!conn->async.hostname)
    goto err_exit;



  /* the resolver thread works on its own copy of the request */

  td->thread_hnd = Curl_thread_create(gethostbyname_thread, &td->tsd);


  if(!td->thread_hnd) {

    err = errno;

    goto err_exit;
  }

  return TRUE;

 err_exit:
  destroy_async_data(&conn->async);

  SET_ERRNO(err);

  return FALSE;
}

/*
 * resolver_error() calls failf() with the appropriate message after a resolve
 * error
 */





static CURLcode resolver_error(struct connectdata *conn)
{
  const char *host_or_proxy;
  CURLcode result;

  if(conn->bits.httpproxy) {
    host_or_proxy = "proxy";
    result = CURLE_COULDNT_RESOLVE_PROXY;
  }
  else {
    host_or_proxy = "host";
    result = CURLE_COULDNT_RESOLVE_HOST;
  }

  failf(conn->data, "Could not resolve %s: %s", host_or_proxy,
        conn->async.hostname);

  return result;
}

/*
 * Curl_resolver_wait_resolv()
 *
 * waits for a resolve to finish. This function should be avoided since using
 * this risk getting the multi interface to "hang".
 *
 * If 'entry' is non-NULL, make it point to the resolved dns entry
 *
 * This is the version for resolves-in-a-thread.
 */
CURLcode Curl_resolver_wait_resolv(struct connectdata *conn,
                                   struct Curl_dns_entry **entry)
{
  struct thread_data   *td = (struct thread_data*) conn->async.os_specific;
  CURLcode result = CURLE_OK;



  /* wait for the thread to resolve the name */
  if(Curl_thread_join(&td->thread_hnd))
    result = getaddrinfo_complete(conn);



  conn->async.done = TRUE;

  if(entry)
    *entry = conn->async.dns;

  if(!conn->async.dns)
    /* a name was not resolved, report error */
    result = resolver_error(conn);

  destroy_async_data(&conn->async);

  if(!conn->async.dns)
    connclose(conn, "asynch resolve failed");

  return result;
}

/*
 * Curl_resolver_is_resolved() is called repeatedly to check if a previous
 * name resolve request has completed. It should also make sure to time-out if
 * the operation seems to take too long.
 */
CURLcode Curl_resolver_is_resolved(struct connectdata *conn,
                                   struct Curl_dns_entry **entry)
{
  struct SessionHandle *data = conn->data;
  struct thread_data   *td = (struct thread_data*) conn->async.os_specific;
  int done = 0;

  *entry = NULL;

  if(!td) {

    return CURLE_COULDNT_RESOLVE_HOST;
  }

  nnosLockMutex(td->tsd.mtx);
  done = td->tsd.done;
  nnosUnlockMutex(td->tsd.mtx);

  if(done) {
    getaddrinfo_complete(conn);

    if(!conn->async.dns) {
      CURLcode result = resolver_error(conn);
      destroy_async_data(&conn->async);
      return result;
    }
    destroy_async_data(&conn->async);
    *entry = conn->async.dns;
  }
  else {
    /* poll for name lookup done with exponential backoff up to 250ms */
    long elapsed = Curl_tvdiff(Curl_tvnow(), data->progress.t_startsingle);
    if(elapsed < 0)
      elapsed = 0;

    if(td->poll_interval == 0)
      /* Start at 1ms poll interval */
      td->poll_interval = 1;
    else if(elapsed >= td->interval_end)
      /* Back-off exponentially if last interval expired  */
      td->poll_interval *= 2;

    if(td->poll_interval > 250)
      td->poll_interval = 250;

    td->interval_end = elapsed + td->poll_interval;
    Curl_expire(conn->data, td->poll_interval);
  }

  return CURLE_OK;
}

int Curl_resolver_getsock(struct connectdata *conn,
                          curl_socket_t *socks,
                          int numsocks)
{
  (void)conn;
  (void)socks;
  (void)numsocks;
  return 0;
}

/*
 * Curl_getaddrinfo() - for platforms without getaddrinfo
 */

Curl_addrinfo *Curl_resolver_getaddrinfo(struct connectdata *conn,
                                         const char *hostname,
                                         int port,
                                         int *waitp)
{
  struct in_addr in;

  *waitp = 0; /* default to synchronous response */


  if(Curl_inet_pton(AF_INET, hostname, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    return Curl_ip2addr(AF_INET, &in, hostname, port);

  /* fire up a new resolver thread! */
  if(init_resolve_thread(conn, hostname, port)) {
    *waitp = 1; /* expect asynchronous response */
    return NULL;
  }

  /* fall-back to blocking version */
  return Curl_ipv4_resolve_r(hostname, port, NULL, 0);
}





















































































CURLcode Curl_set_dns_servers(struct SessionHandle *data,
                              char *servers)
{
  (void)data;
  (void)servers;
  return CURLE_NOT_BUILT_IN;

}

CURLcode Curl_set_dns_interface(struct SessionHandle *data,
                                const char *interf)
{
  (void)data;
  (void)interf;
  return CURLE_NOT_BUILT_IN;
}

CURLcode Curl_set_dns_local_ip4(struct SessionHandle *data,
                                const char *local_ip4)
{
  (void)data;
  (void)local_ip4;
  return CURLE_NOT_BUILT_IN;
}

CURLcode Curl_set_dns_local_ip6(struct SessionHandle *data,
                                const char *local_ip6)
{
  (void)data;
  (void)local_ip6;
  return CURLE_NOT_BUILT_IN;
}
