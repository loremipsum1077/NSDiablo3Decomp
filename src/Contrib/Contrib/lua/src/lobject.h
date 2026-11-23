#include <stdarg.h>
// Adopted complete DWARF layouts; forward declarations break reference-header cycles.
// Original file: Contrib/Contrib/lua/src/lobject.h

#pragma once

#include "llimits.h"
typedef union TString TString;
typedef union Udata Udata;
typedef struct Proto Proto;
typedef struct CClosure CClosure;
typedef struct LClosure LClosure;
typedef union Closure Closure;
typedef union TKey TKey;

typedef union GCObject GCObject;
typedef struct Table Table;
typedef struct LocVar LocVar;
typedef struct UpVal UpVal;
typedef struct Node Node;
typedef struct GCheader GCheader;
typedef struct lua_TValue lua_TValue;


#include <stddef.h>
#include <stdint.h>



struct GCheader {  // size 0x10
    GCObject * next;  // 0x00
    lu_byte tt;  // 0x08
    lu_byte marked;  // 0x09
};
LUA_LAYOUT_ASSERT(sizeof(GCheader) == 0x10, "GCheader");


typedef union {  // size 0x8
    GCObject * gc;  // 0x00
    void * p;  // 0x00
    lua_Number n;  // 0x00
    int b;  // 0x00
} Value;

struct lua_TValue {  // size 0x10
    Value value;  // 0x00
    int tt;  // 0x08
};
LUA_LAYOUT_ASSERT(sizeof(lua_TValue) == 0x10, "lua_TValue");

typedef lua_TValue TValue;

typedef TValue * StkId;

union TString {  // size 0x18
    L_Umaxalign dummy;  // 0x00
    struct {  // size 0x18
        GCObject * next;  // 0x00
        lu_byte tt;  // 0x08
        lu_byte marked;  // 0x09
        lu_byte reserved;  // 0x0a
        unsigned int hash;  // 0x0c
        size_t len;  // 0x10
    } tsv;  // 0x00
};
LUA_LAYOUT_ASSERT(sizeof(TString) == 0x18, "TString");


union Udata {  // size 0x28
    L_Umaxalign dummy;  // 0x00
    struct {  // size 0x28
        GCObject * next;  // 0x00
        lu_byte tt;  // 0x08
        lu_byte marked;  // 0x09
        Table * metatable;  // 0x10
        Table * env;  // 0x18
        size_t len;  // 0x20
    } uv;  // 0x00
};
LUA_LAYOUT_ASSERT(sizeof(Udata) == 0x28, "Udata");


struct Proto {  // size 0x78
    GCObject * next;  // 0x00
    lu_byte tt;  // 0x08
    lu_byte marked;  // 0x09
    TValue * k;  // 0x10
    Instruction * code;  // 0x18
    Proto ** p;  // 0x20
    int * lineinfo;  // 0x28
    LocVar * locvars;  // 0x30
    TString ** upvalues;  // 0x38
    TString * source;  // 0x40
    int sizeupvalues;  // 0x48
    int sizek;  // 0x4c
    int sizecode;  // 0x50
    int sizelineinfo;  // 0x54
    int sizep;  // 0x58
    int sizelocvars;  // 0x5c
    int linedefined;  // 0x60
    int lastlinedefined;  // 0x64
    GCObject * gclist;  // 0x68
    lu_byte nups;  // 0x70
    lu_byte numparams;  // 0x71
    lu_byte is_vararg;  // 0x72
    lu_byte maxstacksize;  // 0x73
};
LUA_LAYOUT_ASSERT(sizeof(Proto) == 0x78, "Proto");


struct LocVar {  // size 0x10
    TString * varname;  // 0x00
    int startpc;  // 0x08
    int endpc;  // 0x0c
};
LUA_LAYOUT_ASSERT(sizeof(LocVar) == 0x10, "LocVar");


struct UpVal {  // size 0x28
    GCObject * next;  // 0x00
    lu_byte tt;  // 0x08
    lu_byte marked;  // 0x09
    TValue * v;  // 0x10
    union {  // size 0x10
        TValue value;  // 0x00
        struct {  // size 0x10
            UpVal * prev;  // 0x00
            UpVal * next;  // 0x08
        } l;  // 0x00
    } u;  // 0x18
};
LUA_LAYOUT_ASSERT(sizeof(UpVal) == 0x28, "UpVal");


struct CClosure {  // size 0x38
    GCObject * next;  // 0x00
    lu_byte tt;  // 0x08
    lu_byte marked;  // 0x09
    lu_byte isC;  // 0x0a
    lu_byte nupvalues;  // 0x0b
    GCObject * gclist;  // 0x10
    Table * env;  // 0x18
    lua_CFunction f;  // 0x20
    TValue upvalue[1];  // 0x28
};
LUA_LAYOUT_ASSERT(sizeof(CClosure) == 0x38, "CClosure");


struct LClosure {  // size 0x30
    GCObject * next;  // 0x00
    lu_byte tt;  // 0x08
    lu_byte marked;  // 0x09
    lu_byte isC;  // 0x0a
    lu_byte nupvalues;  // 0x0b
    GCObject * gclist;  // 0x10
    Table * env;  // 0x18
    Proto * p;  // 0x20
    UpVal * upvals[1];  // 0x28
};
LUA_LAYOUT_ASSERT(sizeof(LClosure) == 0x30, "LClosure");


union Closure {  // size 0x38
    CClosure c;  // 0x00
    LClosure l;  // 0x00
};
LUA_LAYOUT_ASSERT(sizeof(Closure) == 0x38, "Closure");


union TKey {  // size 0x18
    struct {  // size 0x18
        Value value;  // 0x00
        int tt;  // 0x08
        Node * next;  // 0x10
    } nk;  // 0x00
    TValue tvk;  // 0x00
};
LUA_LAYOUT_ASSERT(sizeof(TKey) == 0x18, "TKey");


struct Node {  // size 0x28
    TValue i_val;  // 0x00
    TKey i_key;  // 0x10
};
LUA_LAYOUT_ASSERT(sizeof(Node) == 0x28, "Node");


struct Table {  // size 0x40
    GCObject * next;  // 0x00
    lu_byte tt;  // 0x08
    lu_byte marked;  // 0x09
    lu_byte flags;  // 0x0a
    lu_byte lsizenode;  // 0x0b
    Table * metatable;  // 0x10
    TValue * array;  // 0x18
    Node * node;  // 0x20
    Node * lastfree;  // 0x28
    GCObject * gclist;  // 0x30
    int sizearray;  // 0x38
};
LUA_LAYOUT_ASSERT(sizeof(Table) == 0x40, "Table");



#define ttype(o) ((o)->tt)
#define ttisnil(o) (ttype(o) == 0)
#define hvalue(o) (&(o)->value.gc->h)
#define uvalue(o) (&(o)->value.gc->u.uv)
#ifdef __cplusplus
extern "C" {
#endif
LUAI_DATA const TValue luaO_nilobject_;
#ifdef __cplusplus
}
#endif
#define luaO_nilobject (&luaO_nilobject_)

/* Macros to test type */
#define ttisnumber(o)	(ttype(o) == LUA_TNUMBER)
#define ttisstring(o)	(ttype(o) == LUA_TSTRING)
#define ttistable(o)	(ttype(o) == LUA_TTABLE)
#define ttisfunction(o)	(ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o)	(ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o)	(ttype(o) == LUA_TUSERDATA)
#define ttisthread(o)	(ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o)	(ttype(o) == LUA_TLIGHTUSERDATA)

/* Macros to access values */
#define gcvalue(o)	check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o)	check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o)	check_exp(ttisnumber(o), (o)->value.n)
#define rawtsvalue(o)	check_exp(ttisstring(o), &(o)->value.gc->ts)
#define tsvalue(o)	(&rawtsvalue(o)->tsv)
#define rawuvalue(o)	check_exp(ttisuserdata(o), &(o)->value.gc->u)
#define clvalue(o)	check_exp(ttisfunction(o), &(o)->value.gc->cl)
#define bvalue(o)	check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o)	check_exp(ttisthread(o), &(o)->value.gc->th)

#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

/*
** for internal debug only
*/
#define checkconsistency(obj) \
  lua_assert(!iscollectable(obj) || (ttype(obj) == (obj)->value.gc->gch.tt))

#define checkliveness(g,obj) \
  lua_assert(!iscollectable(obj) || \
  ((ttype(obj) == (obj)->value.gc->gch.tt) && !isdead(g, (obj)->value.gc)))


/* Macros to set values */
#define setnilvalue(obj) ((obj)->tt=LUA_TNIL)

#define setnvalue(obj,x) \
  { TValue *i_o=(obj); i_o->value.n=(x); i_o->tt=LUA_TNUMBER; }

#define setpvalue(obj,x) \
  { TValue *i_o=(obj); i_o->value.p=(x); i_o->tt=LUA_TLIGHTUSERDATA; }

#define setbvalue(obj,x) \
  { TValue *i_o=(obj); i_o->value.b=(x); i_o->tt=LUA_TBOOLEAN; }

#define setsvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TSTRING; \
    checkliveness(G(L),i_o); }

#define setuvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TUSERDATA; \
    checkliveness(G(L),i_o); }

#define setthvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TTHREAD; \
    checkliveness(G(L),i_o); }

#define setclvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TFUNCTION; \
    checkliveness(G(L),i_o); }

#define sethvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TTABLE; \
    checkliveness(G(L),i_o); }

#define setptvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TPROTO; \
    checkliveness(G(L),i_o); }




#define setobj(L,obj1,obj2) \
  { const TValue *o2=(obj2); TValue *o1=(obj1); \
    o1->value = o2->value; o1->tt=o2->tt; \
    checkliveness(G(L),o1); }


/*
** different types of sets, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s	setobj
/* to stack (not from same stack) */
#define setobj2s	setobj
#define setsvalue2s	setsvalue
#define sethvalue2s	sethvalue
#define setptvalue2s	setptvalue
/* from table to same table */
#define setobjt2t	setobj
/* to table */
#define setobj2t	setobj
/* to new object */
#define setobj2n	setobj
#define setsvalue2n	setsvalue

#define setttype(obj, tt) (ttype(obj) = (tt))


#define iscollectable(o)	(ttype(o) >= LUA_TSTRING)




#define LAST_TAG	LUA_TTHREAD

#define NUM_TAGS	(LAST_TAG+1)

#define LUA_TPROTO	(LAST_TAG+1)

#define LUA_TUPVAL	(LAST_TAG+2)

#define LUA_TDEADKEY	(LAST_TAG+3)

#define getstr(ts)	cast(const char *, (ts) + 1)

#define svalue(o)       getstr(tsvalue(o))

#define VARARG_HASARG		1

#define VARARG_ISVARARG		2

#define VARARG_NEEDSARG		4

#define iscfunction(o)	(ttype(o) == LUA_TFUNCTION && clvalue(o)->c.isC)

#define isLfunction(o)	(ttype(o) == LUA_TFUNCTION && !clvalue(o)->c.isC)

#define lmod(s,size) \
	(check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1)))))



#define sizenode(t)	(twoto((t)->lsizenode))

#define ceillog2(x)	(luaO_log2((x)-1) + 1)

#ifdef __cplusplus
extern "C" {
#endif
LUAI_DATA const TValue luaO_nilobject_;
extern unsigned int twoto(unsigned int nPower);
extern int luaO_log2 (unsigned int x);
extern int luaO_int2fb (unsigned int x);
extern int luaO_fb2int (int x);
extern int luaO_rawequalObj (const TValue *t1, const TValue *t2);
extern int luaO_str2d (const char *s, lua_Number *result);
extern const char *luaO_pushvfstring (lua_State *L, const char *fmt,
                                                       va_list argp);
extern const char *luaO_pushfstring (lua_State *L, const char *fmt, ...);
extern void luaO_chunkid (char *out, const char *source, size_t len);
#ifdef __cplusplus
}
#endif
