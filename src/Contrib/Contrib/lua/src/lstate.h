// Adopted complete DWARF layouts; forward declarations break reference-header cycles.
// Original file: Contrib/Contrib/lua/src/lstate.h

#pragma once
#include "lobject.h"
#include "lzio.h"
typedef struct lua_longjmp lua_longjmp;
typedef struct global_State global_State;
typedef struct stringtable stringtable;
typedef struct CallInfo CallInfo;

#include <stddef.h>
#include <stdint.h>

struct lua_longjmp;

struct stringtable {  // size 0x10
    GCObject ** hash;  // 0x00
    lu_int32 nuse;  // 0x08
    int size;  // 0x0c
};
LUA_LAYOUT_ASSERT(sizeof(stringtable) == 0x10, "stringtable");


struct CallInfo {  // size 0x28
    StkId base;  // 0x00
    StkId func;  // 0x08
    StkId top;  // 0x10
    const Instruction * savedpc;  // 0x18
    int nresults;  // 0x20
    int tailcalls;  // 0x24
};
LUA_LAYOUT_ASSERT(sizeof(CallInfo) == 0x28, "CallInfo");


struct global_State {  // size 0x1b0
    stringtable strt;  // 0x00
    lua_Alloc frealloc;  // 0x10
    void * ud;  // 0x18
    lu_byte currentwhite;  // 0x20
    lu_byte gcstate;  // 0x21
    int sweepstrgc;  // 0x24
    GCObject * rootgc;  // 0x28
    GCObject ** sweepgc;  // 0x30
    GCObject * gray;  // 0x38
    GCObject * grayagain;  // 0x40
    GCObject * weak;  // 0x48
    GCObject * tmudata;  // 0x50
    Mbuffer buff;  // 0x58
    lu_mem GCthreshold;  // 0x70
    lu_mem totalbytes;  // 0x78
    lu_mem estimate;  // 0x80
    lu_mem gcdept;  // 0x88
    int gcpause;  // 0x90
    int gcstepmul;  // 0x94
    lua_CFunction panic;  // 0x98
    TValue l_registry;  // 0xa0
    lua_State * mainthread;  // 0xb0
    UpVal uvhead;  // 0xb8
    Table * mt[9];  // 0xe0
    TString * tmname[17];  // 0x128
};
LUA_LAYOUT_ASSERT(sizeof(global_State) == 0x1b0, "global_State");


struct lua_State {  // size 0xc0
    GCObject * next;  // 0x00
    lu_byte tt;  // 0x08
    lu_byte marked;  // 0x09
    lu_byte status;  // 0x0a
    StkId top;  // 0x10
    StkId base;  // 0x18
    global_State * l_G;  // 0x20
    CallInfo * ci;  // 0x28
    const Instruction * savedpc;  // 0x30
    StkId stack_last;  // 0x38
    StkId stack;  // 0x40
    CallInfo * end_ci;  // 0x48
    CallInfo * base_ci;  // 0x50
    int stacksize;  // 0x58
    int size_ci;  // 0x5c
    unsigned short nCcalls;  // 0x60
    lu_byte hookmask;  // 0x62
    lu_byte allowhook;  // 0x63
    int basehookcount;  // 0x64
    int hookcount;  // 0x68
    lua_Hook hook;  // 0x70
    TValue l_gt;  // 0x78
    TValue env;  // 0x88
    GCObject * openupval;  // 0x98
    GCObject * gclist;  // 0xa0
    lua_longjmp * errorJmp;  // 0xa8
    ptrdiff_t errfunc;  // 0xb0
    int abort;  // 0xb8
};
LUA_LAYOUT_ASSERT(sizeof(lua_State) == 0xc0, "lua_State");

union GCObject {  // size 0xc0
    GCheader gch;  // 0x00
    TString ts;  // 0x00
    Udata u;  // 0x00
    Closure cl;  // 0x00
    Table h;  // 0x00
    Proto p;  // 0x00
    UpVal uv;  // 0x00
    lua_State th;  // 0x00
};
LUA_LAYOUT_ASSERT(sizeof(GCObject) == 0xc0, "GCObject");


#define G(L) ((L)->l_G)

#define gt(L)	(&L->l_gt)

#define registry(L)	(&G(L)->l_registry)

#define EXTRA_STACK   5

#define BASIC_CI_SIZE           8

#define BASIC_STACK_SIZE        (2*LUA_MINSTACK)

#define curr_func(L)	(clvalue(L->ci->func))

#define ci_func(ci)	(clvalue((ci)->func))

#define f_isLua(ci)	(!ci_func(ci)->c.isC)

#define isLua(ci)	(ttisfunction((ci)->func) && f_isLua(ci))

#define rawgco2ts(o)	check_exp((o)->gch.tt == LUA_TSTRING, &((o)->ts))

#define gco2ts(o)	(&rawgco2ts(o)->tsv)

#define rawgco2u(o)	check_exp((o)->gch.tt == LUA_TUSERDATA, &((o)->u))

#define gco2u(o)	(&rawgco2u(o)->uv)

#define gco2cl(o)	check_exp((o)->gch.tt == LUA_TFUNCTION, &((o)->cl))

#define gco2h(o)	check_exp((o)->gch.tt == LUA_TTABLE, &((o)->h))

#define gco2p(o)	check_exp((o)->gch.tt == LUA_TPROTO, &((o)->p))

#define gco2uv(o)	check_exp((o)->gch.tt == LUA_TUPVAL, &((o)->uv))

#define ngcotouv(o) \
	check_exp((o) == NULL || (o)->gch.tt == LUA_TUPVAL, &((o)->uv))

#define gco2th(o)	check_exp((o)->gch.tt == LUA_TTHREAD, &((o)->th))

#define obj2gco(v)	(cast(GCObject *, (v)))

#ifdef __cplusplus
extern "C" {
#endif
extern lua_State *luaE_newthread (lua_State *L);
extern void luaE_freethread (lua_State *L, lua_State *L1);
#ifdef __cplusplus
}
#endif
