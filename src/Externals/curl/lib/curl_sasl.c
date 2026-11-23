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

#include "NintendoSDK/Compilers/NX/nx/aarch64/include/ctype.h"
#include <string.h>
#include "Externals/curl/lib/urldata.h"
#include "Externals/curl/lib/curl_md5.h"
#include "Externals/curl/lib/vtls/vtls.h"
/* Declarations from curl_setup.h, curl_base64.h, strtok.h, rawstr.h, curl_memory.h, curl_printf.h and curl_sasl.h; kept local like the other curl files. */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define ISSPACE(x) (isspace((int) ((unsigned char)x)))
#define CURLDIGESTALGO_MD5 0
#define CURLDIGESTALGO_MD5SESS 1
#define aprintf curl_maprintf
#define snprintf curl_msnprintf
#define malloc(size) Curl_SigloMalloc(size)
#define strdup(ptr) Curl_SigloStrdup(ptr)
#define free(ptr) Curl_SigloFree(ptr)
#define Curl_safefree(ptr) do { free((ptr)); (ptr) = NULL;} while(0)
#define strtok_r Curl_strtok_r
int Curl_raw_equal(const char *first, const char *second); char *curl_maprintf(const char *format, ...); int curl_msnprintf(char *buffer, size_t maxlength, const char *format, ...); void *Curl_SigloMalloc(size_t size); char *Curl_SigloStrdup(const char *str); void Curl_SigloFree(void *ptr);
char *Curl_strtok_r(char *s, const char *delim, char **last); CURLcode Curl_base64_encode(struct SessionHandle *data, const char *inputbuff, size_t insize, char **outptr, size_t *outlen);
CURLcode Curl_sasl_decode_digest_http_message(const char *chlg, struct digestdata *digest); CURLcode Curl_sasl_create_digest_http_message(struct SessionHandle *data, const char *userp, const char *passwdp, const unsigned char *request, const unsigned char *uri, struct digestdata *digest, char **outptr, size_t *outlen); void Curl_sasl_digest_cleanup(struct digestdata *digest);

#define DIGEST_MAX_VALUE_LENGTH           256
#define DIGEST_MAX_CONTENT_LENGTH         1024

/*
 * Returns 0 on success and then the buffers are filled in fine.
 *
 * Non-zero means failure to parse.
 */
static int sasl_digest_get_pair(const char *str, char *value, char *content,
                                const char **endptr)
{
  int c;
  bool starts_with_quote = FALSE;
  bool escape = FALSE;

  for(c = DIGEST_MAX_VALUE_LENGTH - 1; (*str && (*str != '=') && c--); )
    *value++ = *str++;
  *value = 0;

  if('=' != *str++)
    /* eek, no match */
    return 1;

  if('\"' == *str) {
    /* this starts with a quote so it must end with one as well! */
    str++;
    starts_with_quote = TRUE;
  }

  for(c = DIGEST_MAX_CONTENT_LENGTH - 1; *str && c--; str++) {
    switch(*str) {
    case '\\':
      if(!escape) {
        /* possibly the start of an escaped quote */
        escape = TRUE;
        *content++ = '\\'; /* even though this is an escape character, we still
                              store it as-is in the target buffer */
        continue;
      }
      break;
    case ',':
      if(!starts_with_quote) {
        /* this signals the end of the content if we didn't get a starting
           quote and then we do "sloppy" parsing */
        c = 0; /* the end */
        continue;
      }
      break;
    case '\r':
    case '\n':
      /* end of string */
      c = 0;
      continue;
    case '\"':
      if(!escape && starts_with_quote) {
        /* end of string */
        c = 0;
        continue;
      }
      break;
    }
    escape = FALSE;
    *content++ = *str;
  }
  *content = 0;

  *endptr = str;

  return 0; /* all is fine! */
}

/* Convert md5 chunk to RFC2617 (section 3.1.3) -suitable ascii string*/
static void sasl_digest_md5_to_ascii(unsigned char *source, /* 16 bytes */
                                     unsigned char *dest) /* 33 bytes */
{
  int i;
  for(i = 0; i < 16; i++)
    snprintf((char *)&dest[i*2], 3, "%02x", source[i]);
}

/* Perform quoted-string escaping as described in RFC2616 and its errata */
static char *sasl_digest_string_quoted(const char *source)
{
  char *dest, *d;
  const char *s = source;
  size_t n = 1; /* null terminator */

  /* Calculate size needed */
  while(*s) {
    ++n;
    if(*s == '"' || *s == '\\') {
      ++n;
    }
    ++s;
  }

  dest = malloc(n);
  if(dest) {
    s = source;
    d = dest;
    while(*s) {
      if(*s == '"' || *s == '\\') {
        *d++ = '\\';
      }
      *d++ = *s++;
    }
    *d = 0;
  }

  return dest;
}

/*
 * Curl_sasl_decode_digest_http_message()
 *
 * This is used to decode a HTTP DIGEST challenge message into the seperate
 * attributes.
 *
 * Parameters:
 *
 * chlg    [in]     - Pointer to the challenge message.
 * digest  [in/out] - The digest data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_sasl_decode_digest_http_message(const char *chlg,
                                              struct digestdata *digest)
{
  bool before = FALSE; /* got a nonce before */
  bool foundAuth = FALSE;
  bool foundAuthInt = FALSE;
  char *token = NULL;
  char *tmp = NULL;

  /* If we already have received a nonce, keep that in mind */
  if(digest->nonce)
    before = TRUE;

  /* Clean up any former leftovers and initialise to defaults */
  Curl_sasl_digest_cleanup(digest);

  for(;;) {
    char value[DIGEST_MAX_VALUE_LENGTH];
    char content[DIGEST_MAX_CONTENT_LENGTH];

    /* Pass all additional spaces here */
    while(*chlg && ISSPACE(*chlg))
      chlg++;

    /* Extract a value=content pair */
    if(!sasl_digest_get_pair(chlg, value, content, &chlg)) {
      if(Curl_raw_equal(value, "nonce")) {
        digest->nonce = strdup(content);
        if(!digest->nonce)
          return CURLE_OUT_OF_MEMORY;
      }
      else if(Curl_raw_equal(value, "stale")) {
        if(Curl_raw_equal(content, "true")) {
          digest->stale = TRUE;
          digest->nc = 1; /* we make a new nonce now */
        }
      }
      else if(Curl_raw_equal(value, "realm")) {
        digest->realm = strdup(content);
        if(!digest->realm)
          return CURLE_OUT_OF_MEMORY;
      }
      else if(Curl_raw_equal(value, "opaque")) {
        digest->opaque = strdup(content);
        if(!digest->opaque)
          return CURLE_OUT_OF_MEMORY;
      }
      else if(Curl_raw_equal(value, "qop")) {
        char *tok_buf;
        /* Tokenize the list and choose auth if possible, use a temporary
           clone of the buffer since strtok_r() ruins it */
        tmp = strdup(content);
        if(!tmp)
          return CURLE_OUT_OF_MEMORY;

        token = strtok_r(tmp, ",", &tok_buf);
        while(token != NULL) {
          if(Curl_raw_equal(token, "auth")) {
            foundAuth = TRUE;
          }
          else if(Curl_raw_equal(token, "auth-int")) {
            foundAuthInt = TRUE;
          }
          token = strtok_r(NULL, ",", &tok_buf);
        }

        free(tmp);

        /* Select only auth or auth-int. Otherwise, ignore */
        if(foundAuth) {
          digest->qop = strdup("auth");
          if(!digest->qop)
            return CURLE_OUT_OF_MEMORY;
        }
        else if(foundAuthInt) {
          digest->qop = strdup("auth-int");
          if(!digest->qop)
            return CURLE_OUT_OF_MEMORY;
        }
      }
      else if(Curl_raw_equal(value, "algorithm")) {
        digest->algorithm = strdup(content);
        if(!digest->algorithm)
          return CURLE_OUT_OF_MEMORY;

        if(Curl_raw_equal(content, "MD5-sess"))
          digest->algo = CURLDIGESTALGO_MD5SESS;
        else if(Curl_raw_equal(content, "MD5"))
          digest->algo = CURLDIGESTALGO_MD5;
        else
          return CURLE_BAD_CONTENT_ENCODING;
      }
      else {
        /* unknown specifier, ignore it! */
      }
    }
    else
      break; /* we're done here */

    /* Pass all additional spaces here */
    while(*chlg && ISSPACE(*chlg))
      chlg++;

    /* Allow the list to be comma-separated */
    if(',' == *chlg)
      chlg++;
  }

  /* We had a nonce since before, and we got another one now without
     'stale=true'. This means we provided bad credentials in the previous
     request */
  if(before && !digest->stale)
    return CURLE_BAD_CONTENT_ENCODING;

  /* We got this header without a nonce, that's a bad Digest line! */
  if(!digest->nonce)
    return CURLE_BAD_CONTENT_ENCODING;

  return CURLE_OK;
}

/*
 * Curl_sasl_digest_cleanup()
 *
 * This is used to clean up the digest specific data.
 *
 * Parameters:
 *
 * digest    [in/out] - The digest data struct being cleaned up.
 *
 */
void Curl_sasl_digest_cleanup(struct digestdata *digest)
{
  Curl_safefree(digest->nonce);
  Curl_safefree(digest->cnonce);
  Curl_safefree(digest->realm);
  Curl_safefree(digest->opaque);
  Curl_safefree(digest->qop);
  Curl_safefree(digest->algorithm);

  digest->nc = 0;
  digest->algo = CURLDIGESTALGO_MD5; /* default algorithm */
  digest->stale = FALSE; /* default means normal, not stale */
}

/*
 * Curl_sasl_create_digest_http_message()
 *
 * This is used to generate a HTTP DIGEST response message ready for sending
 * to the recipient.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * userp   [in]     - The user name.
 * passdwp [in]     - The user's password.
 * request [in]     - The HTTP request.
 * uripath [in]     - The path of the HTTP uri.
 * digest  [in/out] - The digest data struct being used and modified.
 * outptr  [in/out] - The address where a pointer to newly allocated memory
 *                    holding the result will be stored upon completion.
 * outlen  [out]    - The length of the output message.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_sasl_create_digest_http_message(struct SessionHandle *data,
                                              const char *userp,
                                              const char *passwdp,
                                              const unsigned char *request,
                                              const unsigned char *uripath,
                                              struct digestdata *digest,
                                              char **outptr, size_t *outlen)
{
  CURLcode result;
  unsigned char md5buf[16]; /* 16 bytes/128 bits */
  unsigned char request_digest[33];
  unsigned char *md5this;
  unsigned char ha1[33];/* 32 digits and 1 zero byte */
  unsigned char ha2[33];/* 32 digits and 1 zero byte */
  char cnoncebuf[33];
  char *cnonce = NULL;
  size_t cnonce_sz = 0;
  char *userp_quoted;
  char *response = NULL;
  char *tmp = NULL;

  if(!digest->nc)
    digest->nc = 1;

  if(!digest->cnonce) {
    snprintf(cnoncebuf, sizeof(cnoncebuf), "%08x%08x%08x%08x",
             Curl_rand(data), Curl_rand(data),
             Curl_rand(data), Curl_rand(data));

    result = Curl_base64_encode(data, cnoncebuf, strlen(cnoncebuf),
                                &cnonce, &cnonce_sz);
    if(result)
      return result;

    digest->cnonce = cnonce;
  }

  /*
    if the algorithm is "MD5" or unspecified (which then defaults to MD5):

    A1 = unq(username-value) ":" unq(realm-value) ":" passwd

    if the algorithm is "MD5-sess" then:

    A1 = H( unq(username-value) ":" unq(realm-value) ":" passwd ) ":"
         unq(nonce-value) ":" unq(cnonce-value)
  */

  md5this = (unsigned char *)
    aprintf("%s:%s:%s", userp, digest->realm, passwdp);
  if(!md5this)
    return CURLE_OUT_OF_MEMORY;

  Curl_md5it(md5buf, md5this);
  free(md5this);
  sasl_digest_md5_to_ascii(md5buf, ha1);

  if(digest->algo == CURLDIGESTALGO_MD5SESS) {
    /* nonce and cnonce are OUTSIDE the hash */
    tmp = aprintf("%s:%s:%s", ha1, digest->nonce, digest->cnonce);
    if(!tmp)
      return CURLE_OUT_OF_MEMORY;

    Curl_md5it(md5buf, (unsigned char *)tmp);
    free(tmp);
    sasl_digest_md5_to_ascii(md5buf, ha1);
  }

  /*
    If the "qop" directive's value is "auth", then A2 is:

      A2 = Method ":" digest-uri-value

    If the "qop" value is "auth-int", then A2 is:

      A2 = Method ":" digest-uri-value ":" H(entity-body)

    (The "Method" value is the HTTP request method as specified in section
    5.1.1 of RFC 2616)
  */

  md5this = (unsigned char *)aprintf("%s:%s", request, uripath);

  if(digest->qop && Curl_raw_equal(digest->qop, "auth-int")) {
    /* We don't support auth-int for PUT or POST at the moment.
       TODO: replace md5 of empty string with entity-body for PUT/POST */
    unsigned char *md5this2 = (unsigned char *)
      aprintf("%s:%s", md5this, "d41d8cd98f00b204e9800998ecf8427e");
    free(md5this);
    md5this = md5this2;
  }

  if(!md5this)
    return CURLE_OUT_OF_MEMORY;

  Curl_md5it(md5buf, md5this);
  free(md5this);
  sasl_digest_md5_to_ascii(md5buf, ha2);

  if(digest->qop) {
    md5this = (unsigned char *)aprintf("%s:%s:%08x:%s:%s:%s",
                                       ha1,
                                       digest->nonce,
                                       digest->nc,
                                       digest->cnonce,
                                       digest->qop,
                                       ha2);
  }
  else {
    md5this = (unsigned char *)aprintf("%s:%s:%s",
                                       ha1,
                                       digest->nonce,
                                       ha2);
  }

  if(!md5this)
    return CURLE_OUT_OF_MEMORY;

  Curl_md5it(md5buf, md5this);
  free(md5this);
  sasl_digest_md5_to_ascii(md5buf, request_digest);

  /* for more robustness we escape the userp ... */
  userp_quoted = sasl_digest_string_quoted(userp);
  if(!userp_quoted)
    return CURLE_OUT_OF_MEMORY;

  if(digest->qop) {
    response = aprintf("username=\"%s\", "
                       "realm=\"%s\", "
                       "nonce=\"%s\", "
                       "uri=\"%s\", "
                       "cnonce=\"%s\", "
                       "nc=%08x, "
                       "qop=%s, "
                       "response=\"%s\"",
                       userp_quoted,
                       digest->realm,
                       digest->nonce,
                       uripath,
                       digest->cnonce,
                       digest->nc,
                       digest->qop,
                       request_digest);

    if(Curl_raw_equal(digest->qop, "auth"))
      digest->nc++; /* The nc (from RFC) has to be a 8 hex digit number 0
                       padded which tells to the server how many times you are
                       using the same nonce in the qop=auth mode */
  }
  else {
    response = aprintf("username=\"%s\", "
                       "realm=\"%s\", "
                       "nonce=\"%s\", "
                       "uri=\"%s\", "
                       "response=\"%s\"",
                       userp_quoted,
                       digest->realm,
                       digest->nonce,
                       uripath,
                       request_digest);
  }
  free(userp_quoted);
  if(!response)
    return CURLE_OUT_OF_MEMORY;

  /* Add the optional fields */
  if(digest->opaque) {
    /* Append the opaque */
    tmp = aprintf("%s, opaque=\"%s\"", response, digest->opaque);
    free(response);
    if(!tmp)
      return CURLE_OUT_OF_MEMORY;

    response = tmp;
  }

  if(digest->algorithm) {
    /* Append the algorithm */
    tmp = aprintf("%s, algorithm=\"%s\"", response, digest->algorithm);
    free(response);
    if(!tmp)
      return CURLE_OUT_OF_MEMORY;

    response = tmp;
  }

  /* Return the output */
  *outptr = response;
  *outlen = strlen(response);

  return CURLE_OK;
}
