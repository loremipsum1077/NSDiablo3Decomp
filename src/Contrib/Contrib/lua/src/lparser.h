/*
** $Id: lparser.h,v 1.57 2006/03/09 18:14:31 roberto Exp $
** Lua Parser
** See Copyright Notice in lua.h
*/

#ifndef lparser_h
#define lparser_h

#include "llimits.h"
#include "lobject.h"
#include "lzio.h"


/*
** Expression descriptor
*/

typedef enum {
  VVOID,	/* no value */
  VNIL,
  VTRUE,
  VFALSE,
  VK,		/* info = index of constant in `k' */
  VKNUM,	/* nval = numerical value */
  VLOCAL,	/* info = local register */
  VUPVAL,       /* info = index of upvalue in `upvalues' */
  VGLOBAL,	/* info = index of table; aux = index of global name in `k' */
  VINDEXED,	/* info = table register; aux = index register (or `k') */
  VJMP,		/* info = instruction pc */
  VRELOCABLE,	/* info = instruction pc */
  VNONRELOC,	/* info = result register */
  VCALL,	/* info = instruction pc */
  VVARARG	/* info = instruction pc */
} expkind;

typedef struct expdesc {
  expkind k;
  union {
    struct { int info, aux; } s;
    lua_Number nval;
  } u;
  int t;  /* patch list of `exit when true' */
  int f;  /* patch list of `exit when false' */
} expdesc;


typedef struct upvaldesc {
  lu_byte k;
  lu_byte info;
} upvaldesc;


struct BlockCnt;  /* defined in lparser.c */


/* state needed to generate code for a given function */
typedef struct FuncState {
  Proto *f;  /* current function header */
  Table *h;  /* table to find (and reuse) elements in `k' */
  struct FuncState *prev;  /* enclosing function */
  struct LexState *ls;  /* lexical state */
  struct lua_State *L;  /* copy of the Lua state */
  struct BlockCnt *bl;  /* chain of current blocks */
  int pc;  /* next position to code (equivalent to `ncode') */
  int lasttarget;   /* `pc' of last `jump target' */
  int jpc;  /* list of pending jumps to `pc' */
  int freereg;  /* first free register */
  int nk;  /* number of elements in `k' */
  int np;  /* number of elements in `p' */
  short nlocvars;  /* number of elements in `locvars' */
  lu_byte nactvar;  /* number of active local variables */
  upvaldesc upvalues[LUAI_MAXUPVALUES];  /* upvalues */
  unsigned short actvar[LUAI_MAXVARS];  /* declared-variable stack */
} FuncState;


LUAI_FUNC Proto *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff,
                                            const char *name);



// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(struct expdesc) == 0x18, "expdesc");
_Static_assert(offsetof(struct expdesc, k) == 0x0, "expdesc.k");
_Static_assert(offsetof(struct expdesc, t) == 0x10, "expdesc.t");
_Static_assert(offsetof(struct expdesc, f) == 0x14, "expdesc.f");
_Static_assert(sizeof(struct upvaldesc) == 0x2, "upvaldesc");
_Static_assert(offsetof(struct upvaldesc, k) == 0x0, "upvaldesc.k");
_Static_assert(offsetof(struct upvaldesc, info) == 0x1, "upvaldesc.info");
_Static_assert(sizeof(struct FuncState) == 0x258, "FuncState");
_Static_assert(offsetof(struct FuncState, f) == 0x0, "FuncState.f");
_Static_assert(offsetof(struct FuncState, h) == 0x8, "FuncState.h");
_Static_assert(offsetof(struct FuncState, prev) == 0x10, "FuncState.prev");
_Static_assert(offsetof(struct FuncState, ls) == 0x18, "FuncState.ls");
_Static_assert(offsetof(struct FuncState, L) == 0x20, "FuncState.L");
_Static_assert(offsetof(struct FuncState, bl) == 0x28, "FuncState.bl");
_Static_assert(offsetof(struct FuncState, pc) == 0x30, "FuncState.pc");
_Static_assert(offsetof(struct FuncState, lasttarget) == 0x34, "FuncState.lasttarget");
_Static_assert(offsetof(struct FuncState, jpc) == 0x38, "FuncState.jpc");
_Static_assert(offsetof(struct FuncState, freereg) == 0x3c, "FuncState.freereg");
_Static_assert(offsetof(struct FuncState, nk) == 0x40, "FuncState.nk");
_Static_assert(offsetof(struct FuncState, np) == 0x44, "FuncState.np");
_Static_assert(offsetof(struct FuncState, nlocvars) == 0x48, "FuncState.nlocvars");
_Static_assert(offsetof(struct FuncState, nactvar) == 0x4a, "FuncState.nactvar");
_Static_assert(offsetof(struct FuncState, upvalues) == 0x4b, "FuncState.upvalues");
_Static_assert(offsetof(struct FuncState, actvar) == 0xc4, "FuncState.actvar");

#endif
