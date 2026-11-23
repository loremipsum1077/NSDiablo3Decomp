/*
** $Id: lzio.c,v 1.31 2005/06/03 20:15:29 roberto Exp $
** a generic input stream interface
** Copyright notice retained below.
*/


#include <string.h>

#define lzio_c
#define LUA_CORE
#define LUA_MINBUFFER 32 /* luaconf.h; original compares n with 31. */

/* Internal C declarations are supplied by llimits.h and lzio.h. */

#include "llimits.h"
#include "lmem.h"
#include "lstate.h"
#include "lzio.h"


// NON_MATCHING: original retains extra parameter copies/spills; see docs/lzio_evidence.md.
int luaZ_fill (ZIO *z) {
  size_t size;
  lua_State *L = z->L;
  const char *buff;
  /* lua_unlock is empty in this build. */
  buff = z->reader(L, z->data, &size);
  /* lua_lock is empty in this build. */
  if (buff == NULL || size == 0) return EOZ;
  z->n = size - 1;
  z->p = buff;
  return char2int(*(z->p++));
}


// NON_MATCHING: original retains extra parameter copies/spills; see docs/lzio_evidence.md.
int luaZ_lookahead (ZIO *z) {
  if (z->n == 0) {
    if (luaZ_fill(z) == EOZ)
      return EOZ;
    else {
      z->n++;  /* luaZ_fill removed first byte; put back it */
      z->p--;
    }
  }
  return char2int(*z->p);
}


// NON_MATCHING: original retains extra parameter copies/spills; see docs/lzio_evidence.md.
void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader, void *data) {
  z->L = L;
  z->reader = reader;
  z->data = data;
  z->n = 0;
  z->p = NULL;
}


/* --------------------------------------------------------------- read --- */
// NON_MATCHING: original retains extra parameter copies/spills; see docs/lzio_evidence.md.
size_t luaZ_read (ZIO *z, void *b, size_t n) {
  while (n) {
    size_t m;
    if (luaZ_lookahead(z) == EOZ)
      return n;  /* return number of missing bytes */
    m = (n <= z->n) ? n : z->n;  /* min. between n and z->n */
    memcpy(b, z->p, m);
    z->n -= m;
    z->p += m;
    b = (char *)b + m;
    n -= m;
  }
  return 0;
}

/* ------------------------------------------------------------------------ */
// NON_MATCHING: original retains extra parameter copies/spills; see docs/lzio_evidence.md.
char *luaZ_openspace (lua_State *L, Mbuffer *buff, size_t n) {
  if (n > buff->buffsize) {
    if (n < LUA_MINBUFFER) n = LUA_MINBUFFER;
    luaZ_resizebuffer(L, buff, n);
  }
  return buff->buffer;
}



/******************************************************************************
* Copyright (C) 1994-2006 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


