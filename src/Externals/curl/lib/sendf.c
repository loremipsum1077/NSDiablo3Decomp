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

#include <stdarg.h>
#include <string.h>
#include "Externals/curl/lib/urldata.h"
#include "Externals/curl/lib/sendf.h"
/* Declarations from curl_setup.h, curl.h, curl_memory.h, curl_printf.h, strerror.h, multiif.h, non-ascii.h and the siglo socket header (curl_nintendo_socket_posix.h); kept local like the other curl files. */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define DEBUGASSERT(x) do { } while(0)
#define DEBUGF(x) do { } while(0)
#define SOCKERRNO nnsocketGetLastErrno()
#define EINTR 4
#define EAGAIN 11
#define EWOULDBLOCK EAGAIN
#define malloc(size) Curl_SigloMalloc(size)
#define realloc(ptr,size) Curl_SigloRealloc(ptr,size)
#define free(ptr) Curl_SigloFree(ptr)
#define snprintf curl_msnprintf
#define vsnprintf curl_mvsnprintf
#define swrite(x,y,z) (ssize_t)send((x), (y), (z), 0)
#define sread(x,y,z) (ssize_t)recv((x), (y), (z), 0)
#define Curl_convert_from_network(a,b,c) CURLE_OK
#define CURLMIN(x,y) ((x)<(y)?(x):(y))
#define CURL_DO_LINEEND_CONV
#ifndef SECONDARYSOCKET
#define SECONDARYSOCKET 1
#endif
#ifndef BUFSIZE
#define BUFSIZE 16384
#endif
#ifndef CURL_MAX_WRITE_SIZE
#define CURL_MAX_WRITE_SIZE 16384
#endif
#ifndef CURL_ERROR_SIZE
#define CURL_ERROR_SIZE 256
#endif
#ifndef CURL_WRITEFUNC_PAUSE
#define CURL_WRITEFUNC_PAUSE 0x10000001
#endif
#ifndef KEEP_RECV_PAUSE
#define KEEP_RECV_PAUSE (1<<4)
#endif
#ifndef PROTOPT_NONETWORK
#define PROTOPT_NONETWORK (1<<4)
#endif
#ifndef PROTO_FAMILY_FTP
#define PROTO_FAMILY_FTP ((1<<2)|(1<<3))
#endif
void *Curl_SigloMalloc(size_t size); void *Curl_SigloRealloc(void *ptr, size_t size); void Curl_SigloFree(void *ptr); int curl_msnprintf(char *buffer, size_t maxlength, const char *format, ...); int curl_mvsnprintf(char *buffer, size_t maxlength, const char *format, va_list args); const char *Curl_strerror(struct connectdata *conn, int err); bool Curl_multi_pipeline_enabled(const struct Curl_multi *multi); int nnsocketGetLastErrno(void); ssize_t nnsocketSend(int, const void *, size_t, int); ssize_t nnsocketRecv(int, void *, size_t, int);
static inline ssize_t send(int socket, const void *buffer, size_t bufferLength, int flags) { return nnsocketSend(socket, buffer, bufferLength, flags); } static inline ssize_t recv(int socket, void *outBuffer, size_t outBufferLength, int flags) { return nnsocketRecv(socket, outBuffer, outBufferLength, flags); }
int Curl_debug(struct SessionHandle *handle, curl_infotype type, char *data, size_t size, struct connectdata *conn);

#ifdef CURL_DO_LINEEND_CONV
/*
 * convert_lineends() changes CRLF (\r\n) end-of-line markers to a single LF
 * (\n), with special processing for CRLF sequences that are split between two
 * blocks of data.  Remaining, bare CRs are changed to LFs.  The possibly new
 * size of the data is returned.
 */
static size_t convert_lineends(struct SessionHandle *data,
                               char *startPtr, size_t size)
{
  char *inPtr, *outPtr;

  /* sanity check */
  if((startPtr == NULL) || (size < 1)) {
    return size;
  }

  if(data->state.prev_block_had_trailing_cr) {
    /* The previous block of incoming data
       had a trailing CR, which was turned into a LF. */
    if(*startPtr == '\n') {
      /* This block of incoming data starts with the
         previous block's LF so get rid of it */
      memmove(startPtr, startPtr+1, size-1);
      size--;
      /* and it wasn't a bare CR but a CRLF conversion instead */
      data->state.crlf_conversions++;
    }
    data->state.prev_block_had_trailing_cr = FALSE; /* reset the flag */
  }

  /* find 1st CR, if any */
  inPtr = outPtr = memchr(startPtr, '\r', size);
  if(inPtr) {
    /* at least one CR, now look for CRLF */
    while(inPtr < (startPtr+size-1)) {
      /* note that it's size-1, so we'll never look past the last byte */
      if(memcmp(inPtr, "\r\n", 2) == 0) {
        /* CRLF found, bump past the CR and copy the NL */
        inPtr++;
        *outPtr = *inPtr;
        /* keep track of how many CRLFs we converted */
        data->state.crlf_conversions++;
      }
      else {
        if(*inPtr == '\r') {
          /* lone CR, move LF instead */
          *outPtr = '\n';
        }
        else {
          /* not a CRLF nor a CR, just copy whatever it is */
          *outPtr = *inPtr;
        }
      }
      outPtr++;
      inPtr++;
    } /* end of while loop */

    if(inPtr < startPtr+size) {
      /* handle last byte */
      if(*inPtr == '\r') {
        /* deal with a CR at the end of the buffer */
        *outPtr = '\n'; /* copy a NL instead */
        /* note that a CRLF might be split across two blocks */
        data->state.prev_block_had_trailing_cr = TRUE;
      }
      else {
        /* copy last byte */
        *outPtr = *inPtr;
      }
      outPtr++;
    }
    if(outPtr < startPtr+size)
      /* tidy up by null terminating the now shorter data */
      *outPtr = '\0';

    return (outPtr - startPtr);
  }
  return size;
}
#endif /* CURL_DO_LINEEND_CONV */

/* Curl_infof() is for info message along the way */

void Curl_infof(struct SessionHandle *data, const char *fmt, ...)
{
  if(data && data->set.verbose) {
    va_list ap;
    size_t len;
    char print_buffer[2048 + 1];
    va_start(ap, fmt);
    vsnprintf(print_buffer, sizeof(print_buffer), fmt, ap);
    va_end(ap);
    len = strlen(print_buffer);
    Curl_debug(data, CURLINFO_TEXT, print_buffer, len, NULL);
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */

void Curl_failf(struct SessionHandle *data, const char *fmt, ...)
{
  va_list ap;
  size_t len;
  va_start(ap, fmt);

  vsnprintf(data->state.buffer, BUFSIZE, fmt, ap);

  if(data->set.errorbuffer && !data->state.errorbuf) {
    snprintf(data->set.errorbuffer, CURL_ERROR_SIZE, "%s", data->state.buffer);
    data->state.errorbuf = TRUE; /* wrote error string */
  }
  if(data->set.verbose) {
    len = strlen(data->state.buffer);
    if(len < BUFSIZE - 1) {
      data->state.buffer[len] = '\n';
      data->state.buffer[++len] = '\0';
    }
    Curl_debug(data, CURLINFO_TEXT, data->state.buffer, len, NULL);
  }

  va_end(ap);
}

/*
 * Curl_write() is an internal write function that sends data to the
 * server. Works with plain sockets, SCP, SSL or kerberos.
 *
 * If the write would block (CURLE_AGAIN), we return CURLE_OK and
 * (*written == 0). Otherwise we return regular CURLcode value.
 */
CURLcode Curl_write(struct connectdata *conn,
                    curl_socket_t sockfd,
                    const void *mem,
                    size_t len,
                    ssize_t *written)
{
  ssize_t bytes_written;
  CURLcode result = CURLE_OK;
  int num = (sockfd == conn->sock[SECONDARYSOCKET]);

  bytes_written = conn->send[num](conn, num, mem, len, &result);

  *written = bytes_written;
  if(bytes_written >= 0)
    /* we completely ignore the curlcode value when subzero is not returned */
    return CURLE_OK;

  /* handle CURLE_AGAIN or a send failure */
  switch(result) {
  case CURLE_AGAIN:
    *written = 0;
    return CURLE_OK;

  case CURLE_OK:
    /* general send failure */
    return CURLE_SEND_ERROR;

  default:
    /* we got a specific curlcode, forward it */
    return result;
  }
}

ssize_t Curl_send_plain(struct connectdata *conn, int num,
                        const void *mem, size_t len, CURLcode *code)
{
  curl_socket_t sockfd = conn->sock[num];
  ssize_t bytes_written = swrite(sockfd, mem, len);

  *code = CURLE_OK;
  if(-1 == bytes_written) {
    int err = SOCKERRNO;

    if(
      /* errno may be EWOULDBLOCK or on some systems EAGAIN when it returned
         due to its inability to send off data without blocking. We therefor
         treat both error codes the same here */
      (EWOULDBLOCK == err) || (EAGAIN == err) || (EINTR == err)
      ) {
      /* this is just a case of EWOULDBLOCK */
      bytes_written=0;
      *code = CURLE_AGAIN;
    }
    else {
      failf(conn->data, "Send failure: %s",
            Curl_strerror(conn, err));
      conn->data->state.os_errno = err;
      *code = CURLE_SEND_ERROR;
    }
  }
  return bytes_written;
}

/*
 * Curl_write_plain() is an internal write function that sends data to the
 * server using plain sockets only. Otherwise meant to have the exact same
 * proto as Curl_write()
 */
CURLcode Curl_write_plain(struct connectdata *conn,
                          curl_socket_t sockfd,
                          const void *mem,
                          size_t len,
                          ssize_t *written)
{
  ssize_t bytes_written;
  CURLcode result;
  int num = (sockfd == conn->sock[SECONDARYSOCKET]);

  bytes_written = Curl_send_plain(conn, num, mem, len, &result);

  *written = bytes_written;

  return result;
}

ssize_t Curl_recv_plain(struct connectdata *conn, int num, char *buf,
                        size_t len, CURLcode *code)
{
  curl_socket_t sockfd = conn->sock[num];
  ssize_t nread = sread(sockfd, buf, len);

  *code = CURLE_OK;
  if(-1 == nread) {
    int err = SOCKERRNO;

    if(
      /* errno may be EWOULDBLOCK or on some systems EAGAIN when it returned
         due to its inability to send off data without blocking. We therefor
         treat both error codes the same here */
      (EWOULDBLOCK == err) || (EAGAIN == err) || (EINTR == err)
      ) {
      /* this is just a case of EWOULDBLOCK */
      *code = CURLE_AGAIN;
    }
    else {
      failf(conn->data, "Recv failure: %s",
            Curl_strerror(conn, err));
      conn->data->state.os_errno = err;
      *code = CURLE_RECV_ERROR;
    }
  }
  return nread;
}

static CURLcode pausewrite(struct SessionHandle *data,
                           int type, /* what type of data */
                           const char *ptr,
                           size_t len)
{
  /* signalled to pause sending on this connection, but since we have data
     we want to send we need to dup it to save a copy for when the sending
     is again enabled */
  struct SingleRequest *k = &data->req;
  char *dupl = malloc(len);
  if(!dupl)
    return CURLE_OUT_OF_MEMORY;

  memcpy(dupl, ptr, len);

  /* store this information in the state struct for later use */
  data->state.tempwrite = dupl;
  data->state.tempwritesize = len;
  data->state.tempwritetype = type;

  /* mark the connection as RECV paused */
  k->keepon |= KEEP_RECV_PAUSE;

  DEBUGF(infof(data, "Pausing with %zu bytes in buffer for type %02x\n",
               len, type));

  return CURLE_OK;
}


/* Curl_client_chop_write() writes chunks of data not larger than
 * CURL_MAX_WRITE_SIZE via client write callback(s) and
 * takes care of pause requests from the callbacks.
 */
CURLcode Curl_client_chop_write(struct connectdata *conn,
                                int type,
                                char * ptr,
                                size_t len)
{
  struct SessionHandle *data = conn->data;
  curl_write_callback writeheader = NULL;
  curl_write_callback writebody = NULL;

  if(!len)
    return CURLE_OK;

  /* If reading is actually paused, we're forced to append this chunk of data
     to the already held data, but only if it is the same type as otherwise it
     can't work and it'll return error instead. */
  if(data->req.keepon & KEEP_RECV_PAUSE) {
    size_t newlen;
    char *newptr;
    if(type != data->state.tempwritetype)
      /* major internal confusion */
      return CURLE_RECV_ERROR;

    DEBUGASSERT(data->state.tempwrite);

    /* figure out the new size of the data to save */
    newlen = len + data->state.tempwritesize;
    /* allocate the new memory area */
    newptr = realloc(data->state.tempwrite, newlen);
    if(!newptr)
      return CURLE_OUT_OF_MEMORY;
    /* copy the new data to the end of the new area */
    memcpy(newptr + data->state.tempwritesize, ptr, len);
    /* update the pointer and the size */
    data->state.tempwrite = newptr;
    data->state.tempwritesize = newlen;
    return CURLE_OK;
  }

  /* Determine the callback(s) */
  if(type & CLIENTWRITE_BODY)
    writebody = data->set.fwrite_func;
  if((type & CLIENTWRITE_HEADER) &&
     (data->set.fwrite_header || data->set.writeheader)) {
    /*
     * Write headers to the same callback or to the especially setup
     * header callback function (added after version 7.7.1).
     */
    writeheader =
      data->set.fwrite_header? data->set.fwrite_header: data->set.fwrite_func;
  }

  /* Chop data, write chunks. */
  while(len) {
    size_t chunklen = len <= CURL_MAX_WRITE_SIZE? len: CURL_MAX_WRITE_SIZE;

    if(writebody) {
      size_t wrote = writebody(ptr, 1, chunklen, data->set.out);

      if(CURL_WRITEFUNC_PAUSE == wrote) {
        if(conn->handler->flags & PROTOPT_NONETWORK) {
          /* Protocols that work without network cannot be paused. This is
             actually only FILE:// just now, and it can't pause since the
             transfer isn't done using the "normal" procedure. */
          failf(data, "Write callback asked for PAUSE when not supported!");
          return CURLE_WRITE_ERROR;
        }
        else
          return pausewrite(data, type, ptr, len);
      }
      else if(wrote != chunklen) {
        failf(data, "Failed writing body (%zu != %zu)", wrote, chunklen);
        return CURLE_WRITE_ERROR;
      }
    }

    if(writeheader) {
      size_t wrote = writeheader(ptr, 1, chunklen, data->set.writeheader);

      if(CURL_WRITEFUNC_PAUSE == wrote)
        /* here we pass in the HEADER bit only since if this was body as well
           then it was passed already and clearly that didn't trigger the
           pause, so this is saved for later with the HEADER bit only */
        return pausewrite(data, CLIENTWRITE_HEADER, ptr, len);

      if(wrote != chunklen) {
        failf (data, "Failed writing header");
        return CURLE_WRITE_ERROR;
      }
    }

    ptr += chunklen;
    len -= chunklen;
  }

  return CURLE_OK;
}


/* Curl_client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.

   If CURL_DO_LINEEND_CONV is enabled, data is converted IN PLACE to the
   local character encoding.  This is a problem and should be changed in
   the future to leave the original data alone.
 */
CURLcode Curl_client_write(struct connectdata *conn,
                           int type,
                           char *ptr,
                           size_t len)
{
  struct SessionHandle *data = conn->data;

  if(0 == len)
    len = strlen(ptr);

  DEBUGASSERT(type <= 3);

  /* FTP data may need conversion. */
  if((type & CLIENTWRITE_BODY) &&
    (conn->handler->protocol & PROTO_FAMILY_FTP) &&
    conn->proto.ftpc.transfertype == 'A') {
    /* convert from the network encoding */
    CURLcode result = Curl_convert_from_network(data, ptr, len);
    /* Curl_convert_from_network calls failf if unsuccessful */
    if(result)
      return result;

#ifdef CURL_DO_LINEEND_CONV
    /* convert end-of-line markers */
    len = convert_lineends(data, ptr, len);
#endif /* CURL_DO_LINEEND_CONV */
    }

  return Curl_client_chop_write(conn, type, ptr, len);
}

CURLcode Curl_read_plain(curl_socket_t sockfd,
                         char *buf,
                         size_t bytesfromsocket,
                         ssize_t *n)
{
  ssize_t nread = sread(sockfd, buf, bytesfromsocket);

  if(-1 == nread) {
    int err = SOCKERRNO;
    if((EWOULDBLOCK == err) || (EAGAIN == err) || (EINTR == err))
      return CURLE_AGAIN;
    return CURLE_RECV_ERROR;
  }

  /* we only return number of bytes read when we return OK */
  *n = nread;
  return CURLE_OK;
}

/*
 * Internal read-from-socket function. This is meant to deal with plain
 * sockets, SSL sockets and kerberos sockets.
 *
 * Returns a regular CURLcode value.
 */
CURLcode Curl_read(struct connectdata *conn, /* connection data */
                   curl_socket_t sockfd,     /* read from this socket */
                   char *buf,                /* store read data here */
                   size_t sizerequested,     /* max amount to read */
                   ssize_t *n)               /* amount bytes read */
{
  CURLcode result = CURLE_RECV_ERROR;
  ssize_t nread = 0;
  size_t bytesfromsocket = 0;
  char *buffertofill = NULL;
  bool pipelining = Curl_multi_pipeline_enabled(conn->data->multi);

  /* Set 'num' to 0 or 1, depending on which socket that has been sent here.
     If it is the second socket, we set num to 1. Otherwise to 0. This lets
     us use the correct ssl handle. */
  int num = (sockfd == conn->sock[SECONDARYSOCKET]);

  *n=0; /* reset amount to zero */

  /* If session can pipeline, check connection buffer  */
  if(pipelining) {
    size_t bytestocopy = CURLMIN(conn->buf_len - conn->read_pos,
                                 sizerequested);

    /* Copy from our master buffer first if we have some unread data there*/
    if(bytestocopy > 0) {
      memcpy(buf, conn->master_buffer + conn->read_pos, bytestocopy);
      conn->read_pos += bytestocopy;
      conn->bits.stream_was_rewound = FALSE;

      *n = (ssize_t)bytestocopy;
      return CURLE_OK;
    }
    /* If we come here, it means that there is no data to read from the buffer,
     * so we read from the socket */
    bytesfromsocket = CURLMIN(sizerequested, BUFSIZE * sizeof (char));
    buffertofill = conn->master_buffer;
  }
  else {
    bytesfromsocket = CURLMIN((long)sizerequested,
                              conn->data->set.buffer_size ?
                              conn->data->set.buffer_size : BUFSIZE);
    buffertofill = buf;
  }

  nread = conn->recv[num](conn, num, buffertofill, bytesfromsocket, &result);
  if(nread < 0)
    return result;

  if(pipelining) {
    memcpy(buf, conn->master_buffer, nread);
    conn->buf_len = nread;
    conn->read_pos = nread;
  }

  *n += nread;

  return CURLE_OK;
}

/* return 0 on success */
static int showit(struct SessionHandle *data, curl_infotype type,
                  char *ptr, size_t size)
{
  if(data->set.fdebug)
    return (*data->set.fdebug)(data, type, ptr, size,
                               data->set.debugdata);

  switch(type) {
  case CURLINFO_TEXT:
  case CURLINFO_HEADER_OUT:
  case CURLINFO_HEADER_IN:
    {
      /* Nintendo's port replaces the fwrite() pair with a log call that is
         compiled out of this build; only the NUL terminated copy it was
         given remains. */
      char *stringToPrint = malloc(size + 1);
      if(stringToPrint) {
        memcpy(stringToPrint, ptr, size);
        stringToPrint[size] = '\0';
        free(stringToPrint);
      }
    }
    break;
  default: /* nada */
    break;
  }
  return 0;
}

int Curl_debug(struct SessionHandle *data, curl_infotype type,
               char *ptr, size_t size,
               struct connectdata *conn)
{
  int rc;
  if(data->set.printhost && conn && conn->host.dispname) {
    char buffer[160];
    const char *t=NULL;
    const char *w="Data";
    switch (type) {
    case CURLINFO_HEADER_IN:
      w = "Header";
      /* FALLTHROUGH */
    case CURLINFO_DATA_IN:
      t = "from";
      break;
    case CURLINFO_HEADER_OUT:
      w = "Header";
      /* FALLTHROUGH */
    case CURLINFO_DATA_OUT:
      t = "to";
      break;
    default:
      break;
    }

    if(t) {
      snprintf(buffer, sizeof(buffer), "[%s %s %s]", w, t,
               conn->host.dispname);
      rc = showit(data, CURLINFO_TEXT, buffer, strlen(buffer));
      if(rc)
        return rc;
    }
  }
  rc = showit(data, type, ptr, size);
  return rc;
}
