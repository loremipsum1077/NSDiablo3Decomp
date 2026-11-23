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
#include "Externals/curl/lib/urldata.h"
/* Declarations from curl_setup.h, select.h, timeval.h and the siglo socket header (curl_nintendo_socket_posix.h); kept local like the other curl files. */
#define CURL_SOCKET_BAD -1
#define CURL_CSELECT_IN   0x01
#define CURL_CSELECT_OUT  0x02
#define CURL_CSELECT_ERR  0x04
#define CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)
#define SOCKERRNO nnsocketGetLastErrno()
#define SET_SOCKERRNO(x) nnsocketSetLastErrno(x)
#define EINTR 4
#define EINVAL 22
#define HAVE_POLL_FINE
#define POLLIN     0x0001
#define POLLPRI    0x0002
#define POLLOUT    0x0004
#define POLLERR    0x0008
#define POLLHUP    0x0010
#define POLLNVAL   0x0020
#define POLLRDNORM 0x0040
#define POLLRDBAND 0x0080
#define POLLWRNORM 0x0100
struct pollfd { int fd; short events; short revents; };
struct timeval curlx_tvnow(void); long curlx_tvdiff(struct timeval t1, struct timeval t2); int nnsocketGetLastErrno(void); void nnsocketSetLastErrno(int); typedef unsigned long nfds_t; int nnsocketPoll(struct pollfd *, nfds_t, int);
static inline int poll(struct pollfd *pSocketDescriptors, nfds_t numberOfDescriptors, int timeoutMilliseconds) { return nnsocketPoll(pSocketDescriptors, numberOfDescriptors, timeoutMilliseconds); }
static int Curl_wait_ms(int timeout_ms);
int Curl_socket_check(curl_socket_t readfd0, curl_socket_t readfd1, curl_socket_t writefd, long timeout_ms);

#define elapsed_ms  (int)curlx_tvdiff(curlx_tvnow(), initial_tv)

int Curl_ack_eintr = 0;
#define error_not_EINTR (Curl_ack_eintr || error != EINTR)

/*
 * Internal function used for waiting a specific amount of ms
 * in Curl_socket_ready() and Curl_poll() when no file descriptor
 * is provided to wait on, just being used to delay execution.
 * WinSock select() and poll() timeout mechanisms need a valid
 * socket descriptor in a not null file descriptor set to work.
 * Waiting indefinitely with this function is not allowed, a
 * zero or negative timeout value will return immediately.
 * Timeout resolution, accuracy, as well as maximum supported
 * value is system dependent, neither factor is a citical issue
 * for the intended use of this function in the library.
 *
 * Return values:
 *   -1 = system call error, invalid timeout value, or interrupted
 *    0 = specified timeout has elapsed
 */
/* Upstream gives this function external linkage. Here it is static: the image
 * has no out-of-line copy and Curl_socket_check only matches with it inlined. */
static int Curl_wait_ms(int timeout_ms)
{
  struct timeval initial_tv;
  int pending_ms;
  int error;
  int r = 0;

  if(!timeout_ms)
    return 0;
  if(timeout_ms < 0) {
    SET_SOCKERRNO(EINVAL);
    return -1;
  }
  pending_ms = timeout_ms;
  initial_tv = curlx_tvnow();
  do {
    r = poll(NULL, 0, pending_ms);
    if(r != -1)
      break;
    error = SOCKERRNO;
    if(error && error_not_EINTR)
      break;
    pending_ms = timeout_ms - elapsed_ms;
    if(pending_ms <= 0) {
      r = 0;  /* Simulate a "call timed out" case */
      break;
    }
  } while(r == -1);
  if(r)
    r = -1;
  return r;
}

/*
 * Wait for read or write events on a set of file descriptors. It uses poll()
 * when a fine poll() is available, in order to avoid limits with FD_SETSIZE,
 * otherwise select() is used.  An error is returned if select() is being used
 * and a file descriptor is too large for FD_SETSIZE.
 *
 * A negative timeout value makes this function wait indefinitely,
 * unles no valid file descriptor is given, when this happens the
 * negative timeout is ignored and the function times out immediately.
 *
 * Return values:
 *   -1 = system call error or fd >= FD_SETSIZE
 *    0 = timeout
 *    [bitmask] = action as described below
 *
 * CURL_CSELECT_IN - first socket is readable
 * CURL_CSELECT_IN2 - second socket is readable
 * CURL_CSELECT_OUT - write socket is writable
 * CURL_CSELECT_ERR - an error condition occurred
 */
int Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      long timeout_ms)       /* milliseconds to wait */
{
  struct pollfd pfd[3];
  int num;
  struct timeval initial_tv = {0, 0};
  int pending_ms = 0;
  int error;
  int r;
  int ret;

  if((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, just wait */
    r = Curl_wait_ms((int)timeout_ms);
    return r;
  }

  /* Avoid initial timestamp, avoid curlx_tvnow() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  if(timeout_ms > 0) {
    pending_ms = (int)timeout_ms;
    initial_tv = curlx_tvnow();
  }

  num = 0;
  if(readfd0 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd0;
    pfd[num].events = POLLRDNORM|POLLIN|POLLRDBAND|POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  if(readfd1 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd1;
    pfd[num].events = POLLRDNORM|POLLIN|POLLRDBAND|POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  if(writefd != CURL_SOCKET_BAD) {
    pfd[num].fd = writefd;
    pfd[num].events = POLLWRNORM|POLLOUT;
    pfd[num].revents = 0;
    num++;
  }

  do {
    if(timeout_ms < 0)
      pending_ms = -1;
    else if(!timeout_ms)
      pending_ms = 0;
    r = poll(pfd, num, pending_ms);
    if(r != -1)
      break;
    error = SOCKERRNO;
    if(error && error_not_EINTR)
      break;
    if(timeout_ms > 0) {
      pending_ms = (int)(timeout_ms - elapsed_ms);
      if(pending_ms <= 0) {
        r = 0;  /* Simulate a "call timed out" case */
        break;
      }
    }
  } while(r == -1);

  if(r < 0)
    return -1;
  if(r == 0)
    return 0;

  ret = 0;
  num = 0;
  if(readfd0 != CURL_SOCKET_BAD) {
    if(pfd[num].revents & (POLLRDNORM|POLLIN|POLLERR|POLLHUP))
      ret |= CURL_CSELECT_IN;
    if(pfd[num].revents & (POLLRDBAND|POLLPRI|POLLNVAL))
      ret |= CURL_CSELECT_ERR;
    num++;
  }
  if(readfd1 != CURL_SOCKET_BAD) {
    if(pfd[num].revents & (POLLRDNORM|POLLIN|POLLERR|POLLHUP))
      ret |= CURL_CSELECT_IN2;
    if(pfd[num].revents & (POLLRDBAND|POLLPRI|POLLNVAL))
      ret |= CURL_CSELECT_ERR;
    num++;
  }
  if(writefd != CURL_SOCKET_BAD) {
    if(pfd[num].revents & (POLLWRNORM|POLLOUT))
      ret |= CURL_CSELECT_OUT;
    if(pfd[num].revents & (POLLERR|POLLHUP|POLLNVAL))
      ret |= CURL_CSELECT_ERR;
  }

  return ret;
}
