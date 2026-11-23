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
#include <stdio.h>
#include <string.h>
#include "Externals/curl/lib/urldata.h"
#include "Externals/curl/lib/formdata.h"
#include "Externals/curl/lib/vtls/vtls.h"
#include "Externals/curl/lib/sendf.h"
/* Declarations from curl_setup.h, curl.h, strequal.h, curl_memory.h, curl_printf.h and sys/stat.h; kept local like the other curl files. */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define ZERO_NULL 0
#define HTTPPOST_FILENAME (1<<0)
#define HTTPPOST_READFILE (1<<1)
#define HTTPPOST_PTRNAME (1<<2)
#define HTTPPOST_PTRCONTENTS (1<<3)
#define HTTPPOST_BUFFER (1<<4)
#define HTTPPOST_PTRBUFFER (1<<5)
#define HTTPPOST_CALLBACK (1<<6)
struct curl_httppost { struct curl_httppost *next; char *name; long namelength; char *contents; long contentslength; char *buffer; long bufferlength; char *contenttype; struct curl_slist *contentheader; struct curl_httppost *more; long flags; char *showfilename; void *userp; };
struct stat { unsigned long st_dev; unsigned long st_ino; unsigned int st_mode; unsigned int st_nlink; unsigned int st_uid; unsigned int st_gid; unsigned long st_rdev; unsigned long __pad; long st_size; int st_blksize; int __pad2; long st_blocks; struct timespec { long tv_sec; long tv_nsec; } st_atim, st_mtim, st_ctim; unsigned int __unused1[2]; };
#define S_ISDIR(m) (((m) & 0xf000) == 0x4000)
#define struct_stat struct stat
#define strequal(a,b) curl_strequal(a,b)
#define aprintf curl_maprintf
#define vsnprintf curl_mvsnprintf
#define malloc(size) Curl_SigloMalloc(size)
#define strdup(ptr) Curl_SigloStrdup(ptr)
#define free(ptr) Curl_SigloFree(ptr)
#define Curl_safefree(ptr) do { free((ptr)); (ptr) = NULL;} while(0)
#define fopen nnCurlShim_fopen
#define fread nnCurlShim_fread
#define fclose nnCurlShim_fclose
#define stat(a,b) nnCurlShim_stat(a,b)
#define fopen_read fopen
int curl_strequal(const char *s1, const char *s2); char *curl_maprintf(const char *format, ...); int curl_mvsnprintf(char *buffer, size_t maxlength, const char *format, va_list args); void *Curl_SigloMalloc(size_t size); char *Curl_SigloStrdup(const char *str); void Curl_SigloFree(void *ptr);
FILE *nnCurlShim_fopen(const char *path, const char *mode); size_t nnCurlShim_fread(void *ptr, size_t size, size_t nmemb, FILE *stream); int nnCurlShim_fclose(FILE *fp); int nnCurlShim_stat(const char *path, struct stat *buf);

/* The SDK libc (musl-derived) declares `extern FILE *const stdin;`; the toolchain image's glibc declares it non-const, which stops the load being hoisted. Bind a const declaration to the same symbol, as include/stdio.h does for sscanf. */
extern FILE *const stdin_nx __asm__("stdin");
#undef stdin
#define stdin stdin_nx

static char *Curl_basename(char *path);
#define basename(x)  Curl_basename((x))

static char *formboundary(struct SessionHandle *data);

/*
 * AddFormData() adds a chunk of data to the FormData linked list.
 *
 * size is incremented by the chunk length, unless it is NULL
 */
static CURLcode AddFormData(struct FormData **formp,
                            enum formtype type,
                            const void *line,
                            size_t length,
                            curl_off_t *size)
{
  struct FormData *newform = malloc(sizeof(struct FormData));
  if(!newform)
    return CURLE_OUT_OF_MEMORY;
  newform->next = NULL;

  if(type <= FORM_CONTENT) {
    /* we make it easier for plain strings: */
    if(!length)
      length = strlen((char *)line);

    newform->line = malloc(length+1);
    if(!newform->line) {
      free(newform);
      return CURLE_OUT_OF_MEMORY;
    }
    memcpy(newform->line, line, length);
    newform->length = length;
    newform->line[length]=0; /* zero terminate for easier debugging */
  }
  else
    /* For callbacks and files we don't have any actual data so we just keep a
       pointer to whatever this points to */
    newform->line = (char *)line;

  newform->type = type;

  if(*formp) {
    (*formp)->next = newform;
    *formp = newform;
  }
  else
    *formp = newform;

  if(size) {
    if(type != FORM_FILE)
      /* for static content as well as callback data we add the size given
         as input argument */
      *size += length;
    else {
      /* Since this is a file to be uploaded here, add the size of the actual
         file */
      if(!strequal("-", newform->line)) {
        struct_stat file;
        if(!stat(newform->line, &file) && !S_ISDIR(file.st_mode))
          *size += file.st_size;
        else
          return CURLE_BAD_FUNCTION_ARGUMENT;
      }
    }
  }
  return CURLE_OK;
}

/*
 * AddFormDataf() adds printf()-style formatted data to the formdata chain.
 */

static CURLcode AddFormDataf(struct FormData **formp,
                             curl_off_t *size,
                             const char *fmt, ...)
{
  char s[4096];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(s, sizeof(s), fmt, ap);
  va_end(ap);

  return AddFormData(formp, FORM_DATA, s, 0, size);
}

/*
 * Curl_formclean() is used from http.c, this cleans a built FormData linked
 * list
 */
void Curl_formclean(struct FormData **form_ptr)
{
  struct FormData *next, *form;

  form = *form_ptr;
  if(!form)
    return;

  do {
    next=form->next;  /* the following form line */
    if(form->type <= FORM_CONTENT)
      free(form->line); /* free the line */
    free(form);       /* free the struct */

  } while((form = next) != NULL); /* continue */

  *form_ptr = NULL;
}

static char *Curl_basename(char *path)
{
  /* Ignore all the details above for now and make a quick and simple
     implementaion here */
  char *s1;
  char *s2;

  s1=strrchr(path, '/');
  s2=strrchr(path, '\\');

  if(s1 && s2) {
    path = (s1 > s2? s1 : s2)+1;
  }
  else if(s1)
    path = s1 + 1;
  else if(s2)
    path = s2 + 1;

  return path;
}

static char *strippath(const char *fullfile)
{
  char *filename;
  char *base;
  filename = strdup(fullfile); /* duplicate since basename() may ruin the
                                  buffer it works on */
  if(!filename)
    return NULL;
  base = strdup(basename(filename));

  free(filename); /* free temporary buffer */

  return base; /* returns an allocated string or NULL ! */
}

static CURLcode formdata_add_filename(const struct curl_httppost *file,
                                      struct FormData **form,
                                      curl_off_t *size)
{
  CURLcode result = CURLE_OK;
  char *filename = file->showfilename;
  char *filebasename = NULL;
  char *filename_escaped = NULL;

  if(!filename) {
    filebasename = strippath(file->contents);
    if(!filebasename)
      return CURLE_OUT_OF_MEMORY;
    filename = filebasename;
  }

  if(strchr(filename, '\\') || strchr(filename, '"')) {
    char *p0, *p1;

    /* filename need be escaped */
    filename_escaped = malloc(strlen(filename)*2+1);
    if(!filename_escaped) {
      Curl_safefree(filebasename);
      return CURLE_OUT_OF_MEMORY;
    }
    p0 = filename_escaped;
    p1 = filename;
    while(*p1) {
      if(*p1 == '\\' || *p1 == '"')
        *p0++ = '\\';
      *p0++ = *p1++;
    }
    *p0 = '\0';
    filename = filename_escaped;
  }
  result = AddFormDataf(form, size,
                        "; filename=\"%s\"",
                        filename);
  Curl_safefree(filename_escaped);
  Curl_safefree(filebasename);
  return result;
}

/*
 * Curl_getformdata() converts a linked list of "meta data" into a complete
 * (possibly huge) multipart formdata. The input list is in 'post', while the
 * output resulting linked lists gets stored in '*finalform'. *sizep will get
 * the total size of the whole POST.
 * A multipart/form_data content-type is built, unless a custom content-type
 * is passed in 'custom_content_type'.
 *
 * This function will not do a failf() for the potential memory failures but
 * should for all other errors it spots. Just note that this function MAY get
 * a NULL pointer in the 'data' argument.
 */

CURLcode Curl_getformdata(struct SessionHandle *data,
                          struct FormData **finalform,
                          struct curl_httppost *post,
                          const char *custom_content_type,
                          curl_off_t *sizep)
{
  struct FormData *form = NULL;
  struct FormData *firstform;
  struct curl_httppost *file;
  CURLcode result = CURLE_OK;

  curl_off_t size = 0; /* support potentially ENORMOUS formposts */
  char *boundary;
  char *fileboundary = NULL;
  struct curl_slist* curList;

  *finalform = NULL; /* default form is empty */

  if(!post)
    return result; /* no input => no output! */

  boundary = formboundary(data);
  if(!boundary)
    return CURLE_OUT_OF_MEMORY;

  /* Make the first line of the output */
  result = AddFormDataf(&form, NULL,
                        "%s; boundary=%s\r\n",
                        custom_content_type?custom_content_type:
                        "Content-Type: multipart/form-data",
                        boundary);

  if(result) {
    Curl_safefree(boundary);
    return result;
  }
  /* we DO NOT include that line in the total size of the POST, since it'll be
     part of the header! */

  firstform = form;

  do {

    if(size) {
      result = AddFormDataf(&form, &size, "\r\n");
      if(result)
        break;
    }

    /* boundary */
    result = AddFormDataf(&form, &size, "--%s\r\n", boundary);
    if(result)
      break;

    /* Maybe later this should be disabled when a custom_content_type is
       passed, since Content-Disposition is not meaningful for all multipart
       types.
    */
    result = AddFormDataf(&form, &size,
                          "Content-Disposition: form-data; name=\"");
    if(result)
      break;

    result = AddFormData(&form, FORM_DATA, post->name, post->namelength,
                         &size);
    if(result)
      break;

    result = AddFormDataf(&form, &size, "\"");
    if(result)
      break;

    if(post->more) {
      /* If used, this is a link to more file names, we must then do
         the magic to include several files with the same field name */

      Curl_safefree(fileboundary);
      fileboundary = formboundary(data);
      if(!fileboundary) {
        result = CURLE_OUT_OF_MEMORY;
        break;
      }

      result = AddFormDataf(&form, &size,
                            "\r\nContent-Type: multipart/mixed;"
                            " boundary=%s\r\n",
                            fileboundary);
      if(result)
        break;
    }

    file = post;

    do {

      /* If 'showfilename' is set, that is a faked name passed on to us
         to use to in the formpost. If that is not set, the actually used
         local file name should be added. */

      if(post->more) {
        /* if multiple-file */
        result = AddFormDataf(&form, &size,
                              "\r\n--%s\r\nContent-Disposition: "
                              "attachment",
                              fileboundary);
        if(result)
          break;
        result = formdata_add_filename(file, &form, &size);
        if(result)
          break;
      }
      else if(post->flags & (HTTPPOST_FILENAME|HTTPPOST_BUFFER|
                             HTTPPOST_CALLBACK)) {
        /* it should be noted that for the HTTPPOST_FILENAME and
           HTTPPOST_CALLBACK cases the ->showfilename struct member is always
           assigned at this point */
        if(post->showfilename || (post->flags & HTTPPOST_FILENAME)) {
          result = formdata_add_filename(post, &form, &size);
        }

        if(result)
          break;
      }

      if(file->contenttype) {
        /* we have a specified type */
        result = AddFormDataf(&form, &size,
                              "\r\nContent-Type: %s",
                              file->contenttype);
        if(result)
          break;
      }

      curList = file->contentheader;
      while(curList) {
        /* Process the additional headers specified for this form */
        result = AddFormDataf( &form, &size, "\r\n%s", curList->data );
        if(result)
          break;
        curList = curList->next;
      }
      if(result)
        break;

      result = AddFormDataf(&form, &size, "\r\n\r\n");
      if(result)
        break;

      if((post->flags & HTTPPOST_FILENAME) ||
         (post->flags & HTTPPOST_READFILE)) {
        /* we should include the contents from the specified file */
        FILE *fileread;

        fileread = strequal("-", file->contents)?
          stdin:fopen(file->contents, "rb"); /* binary read for win32  */

        /*
         * VMS: This only allows for stream files on VMS.  Stream files are
         * OK, as are FIXED & VAR files WITHOUT implied CC For implied CC,
         * every record needs to have a \n appended & 1 added to SIZE
         */

        if(fileread) {
          if(fileread != stdin) {
            /* close the file */
            fclose(fileread);
            /* add the file name only - for later reading from this */
            result = AddFormData(&form, FORM_FILE, file->contents, 0, &size);
          }
          else {
            /* When uploading from stdin, we can't know the size of the file,
             * thus must read the full file as before. We *could* use chunked
             * transfer-encoding, but that only works for HTTP 1.1 and we
             * can't be sure we work with such a server.
             */
            size_t nread;
            char buffer[512];
            while((nread = fread(buffer, 1, sizeof(buffer), fileread)) != 0) {
              result = AddFormData(&form, FORM_CONTENT, buffer, nread, &size);
              if(result)
                break;
            }
          }
        }
        else {
          if(data)
            failf(data, "couldn't open file \"%s\"", file->contents);
          *finalform = NULL;
          result = CURLE_READ_ERROR;
        }
      }
      else if(post->flags & HTTPPOST_BUFFER)
        /* include contents of buffer */
        result = AddFormData(&form, FORM_CONTENT, post->buffer,
                             post->bufferlength, &size);
      else if(post->flags & HTTPPOST_CALLBACK)
        /* the contents should be read with the callback and the size
           is set with the contentslength */
        result = AddFormData(&form, FORM_CALLBACK, post->userp,
                             post->contentslength, &size);
      else
        /* include the contents we got */
        result = AddFormData(&form, FORM_CONTENT, post->contents,
                             post->contentslength, &size);

      file = file->more;
    } while(file && !result); /* for each specified file for this field */

    if(result)
      break;

    if(post->more) {
      /* this was a multiple-file inclusion, make a termination file
         boundary: */
      result = AddFormDataf(&form, &size,
                           "\r\n--%s--",
                           fileboundary);
      if(result)
        break;
    }

  } while((post = post->next) != NULL); /* for each field */

  /* end-boundary for everything */
  if(!result)
    result = AddFormDataf(&form, &size, "\r\n--%s--\r\n", boundary);

  if(result) {
    Curl_formclean(&firstform);
    Curl_safefree(fileboundary);
    Curl_safefree(boundary);
    return result;
  }

  *sizep = size;

  Curl_safefree(fileboundary);
  Curl_safefree(boundary);

  *finalform = firstform;

  return result;
}

/*
 * Curl_FormInit() inits the struct 'form' points to with the 'formdata'
 * and resets the 'sent' counter.
 */
int Curl_FormInit(struct Form *form, struct FormData *formdata )
{
  if(!formdata)
    return 1; /* error */

  form->data = formdata;
  form->sent = 0;
  form->fp = NULL;
  form->fread_func = ZERO_NULL;

  return 0;
}

/*
 * readfromfile()
 *
 * The read callback that this function may use can return a value larger than
 * 'size' (which then this function returns) that indicates a problem and it
 * must be properly dealt with
 */
static size_t readfromfile(struct Form *form, char *buffer,
                           size_t size)
{
  size_t nread;
  bool callback = (form->data->type == FORM_CALLBACK)?TRUE:FALSE;

  if(callback) {
    if(form->fread_func == ZERO_NULL)
      return 0;
    else
      nread = form->fread_func(buffer, 1, size, form->data->line);
  }
  else {
    if(!form->fp) {
      /* this file hasn't yet been opened */
      form->fp = fopen_read(form->data->line, "rb"); /* b is for binary */
      if(!form->fp)
        return (size_t)-1; /* failure */
    }
    nread = fread(buffer, 1, size, form->fp);
  }
  if(!nread) {
    /* this is the last chunk from the file, move on */
    if(form->fp) {
      fclose(form->fp);
      form->fp = NULL;
    }
    form->data = form->data->next;
  }

  return nread;
}

/*
 * Curl_FormReader() is the fread() emulation function that will be used to
 * deliver the formdata to the transfer loop.
 */
size_t Curl_FormReader(char *buffer,
                       size_t size,
                       size_t nitems,
                       FILE *mydata)
{
  struct Form *form;
  size_t wantedsize;
  size_t gotsize = 0;

  form=(struct Form *)mydata;

  wantedsize = size * nitems;

  if(!form->data)
    return 0; /* nothing, error, empty */

  if((form->data->type == FORM_FILE) ||
     (form->data->type == FORM_CALLBACK)) {
    gotsize = readfromfile(form, buffer, wantedsize);

    if(gotsize)
      /* If positive or -1, return. If zero, continue! */
      return gotsize;
  }
  do {

    if((form->data->length - form->sent ) > wantedsize - gotsize) {

      memcpy(buffer + gotsize , form->data->line + form->sent,
             wantedsize - gotsize);

      form->sent += wantedsize-gotsize;

      return wantedsize;
    }

    memcpy(buffer+gotsize,
           form->data->line + form->sent,
           (form->data->length - form->sent) );
    gotsize += form->data->length - form->sent;

    form->sent = 0;

    form->data = form->data->next; /* advance */

  } while(form->data && (form->data->type < FORM_CALLBACK));
  /* If we got an empty line and we have more data, we proceed to the next
     line immediately to avoid returning zero before we've reached the end. */

  return gotsize;
}

/*
 * Curl_formpostheader() returns the first line of the formpost, the
 * request-header part (which is not part of the request-body like the rest of
 * the post).
 */
char *Curl_formpostheader(void *formp, size_t *len)
{
  char *header;
  struct Form *form=(struct Form *)formp;

  if(!form->data)
    return 0; /* nothing, ERROR! */

  header = form->data->line;
  *len = form->data->length;

  form->data = form->data->next; /* advance */

  return header;
}

/*
 * formboundary() creates a suitable boundary string and returns an allocated
 * one.
 */
static char *formboundary(struct SessionHandle *data)
{
  /* 24 dashes and 16 hexadecimal digits makes 64 bit (18446744073709551615)
     combinations */
  return aprintf("------------------------%08x%08x",
                 Curl_rand(data), Curl_rand(data));
}
