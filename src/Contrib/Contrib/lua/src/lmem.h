/* Lua 5.1.1 lmem.h subset, recovered against lmem.c original assembly.
 * Copyright (C) 1994-2006 Lua.org, PUC-Rio; see lmem.c for license. */
#pragma once
#include "llimits.h"

#define MEMERRMSG	"not enough memory"

/* llimits.h MAX_SIZET is ((size_t)(~(size_t)0)-2). Keep this
 * owner-local expansion until the remaining allocation API is adopted. */
#define luaM_reallocv(L,b,on,n,e) \
  ((cast(size_t, (n)+1) <= ((size_t)(~(size_t)0)-2)/(e)) ? \
    luaM_realloc_(L, (b), (on)*(e), (n)*(e)) : luaM_toobig(L))

#define luaM_freemem(L, b, s)	luaM_realloc_(L, (b), (s), 0)
#define luaM_free(L, b)		luaM_realloc_(L, (b), sizeof(*(b)), 0)
#define luaM_freearray(L, b, n, t)   luaM_reallocv(L, (b), n, 0, sizeof(t))

#define luaM_malloc(L,t)	luaM_realloc_(L, NULL, 0, (t))
#define luaM_new(L,t)		cast(t *, luaM_malloc(L, sizeof(t)))
#define luaM_newvector(L,n,t) \
		cast(t *, luaM_reallocv(L, NULL, 0, n, sizeof(t)))

#define luaM_growvector(L,v,nelems,size,t,limit,e) \
          if ((nelems)+1 > (size)) \
            ((v)=cast(t *, luaM_growaux_(L,v,&(size),sizeof(t),limit,e)))

#define luaM_reallocvector(L, v,oldn,n,t) \
   ((v)=cast(t *, luaM_reallocv(L, v, oldn, n, sizeof(t))))


#ifdef __cplusplus
extern "C" {
#endif
void *luaM_realloc_(lua_State *L, void *block, size_t osize, size_t nsize);
void *luaM_toobig(lua_State *L);
void *luaM_growaux_(lua_State *L, void *block, int *size, size_t size_elems,
                    int limit, const char *errormsg);

#ifdef __cplusplus
}
#endif
