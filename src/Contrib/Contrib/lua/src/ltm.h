// TMS values adopted from original DWARF.
// Original file: Contrib/Contrib/lua/src/ltm.h

#pragma once

#include "lobject.h"

typedef enum {
    TM_INDEX = 0,
    TM_NEWINDEX = 1,
    TM_GC = 2,
    TM_MODE = 3,
    TM_EQ = 4,
    TM_ADD = 5,
    TM_SUB = 6,
    TM_MUL = 7,
    TM_DIV = 8,
    TM_MOD = 9,
    TM_POW = 10,
    TM_UNM = 11,
    TM_LEN = 12,
    TM_LT = 13,
    TM_LE = 14,
    TM_CONCAT = 15,
    TM_CALL = 16,
    TM_N = 17,
} TMS;

#ifdef __cplusplus
extern "C" {
#endif
LUAI_DATA const char *const luaT_typenames[];
/* Lua 5.1.1 ltm.h declarations (defined in ltm.c) */
LUAI_FUNC const TValue *luaT_gettm (Table *events, TMS event, TString *ename);
LUAI_FUNC const TValue *luaT_gettmbyobj (lua_State *L, const TValue *o,
                                                       TMS event);
LUAI_FUNC void luaT_init (lua_State *L);
#ifdef __cplusplus
}
#endif

/* Lua 5.1.1 ltm.h fast tag-method lookup (G() is in lstate.h) */
#define gfasttm(g,et,e) ((et) == NULL ? NULL : \
  ((et)->flags & (1u<<(e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e)	gfasttm(G(l), et, e)
