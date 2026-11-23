/* Native behavioral checks for the recovered API; linker dead stripping excludes
 * untested VM/parser/allocation paths. These tests do not claim byte matching. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lua.h"
#include "lstate.h"
#include "lvm.h"
#include "lgc.h"

extern void lua_xmove(lua_State *, lua_State *, int);
extern void lua_insert(lua_State *, int);
extern int lua_checkstack(lua_State *, int);
extern int lua_gc(lua_State *, int, int);
extern const char *lua_getupvalue(lua_State *, int, int);
extern const char *lua_setupvalue(lua_State *, int, int);
const TValue luaO_nilobject_ = {{0}, LUA_TNIL};
static unsigned steps, collections;
/* Conversion failure stub is deliberately unreachable: only numeric inputs are
 * passed to the public numeric conversion functions below. */
const TValue *luaV_tonumber(const TValue *obj, TValue *n) {
  (void)obj; (void)n; abort();
}
void luaD_growstack(lua_State *L, int n) { (void)L; (void)n; abort(); }
void luaC_barrierf(lua_State *L, GCObject *a, GCObject *b) {
  (void)L; (void)a; (void)b; abort();
}
void luaC_step(lua_State *L) {
  ++steps; G(L)->GCthreshold=G(L)->totalbytes+1; G(L)->gcstate=0;
}
void luaC_fullgc(lua_State *L) { (void)L; ++collections; }

static void init(lua_State *L, global_State *g, CallInfo *ci, TValue *stack) {
  memset(L,0,sizeof(*L)); memset(ci,0,sizeof(*ci));
  L->l_G=g; L->stack=L->base=L->top=stack;
  L->stack_last=stack+63; L->ci=L->base_ci=ci; ci->top=stack+63;
}
int main(void) {
  lua_State L, other; global_State g={0}; CallInfo ci, ci2;
  TValue stack[64]={0}, stack2[64]={0}; GCObject closure={0};
  init(&L,&g,&ci,stack); init(&other,&g,&ci2,stack2);
  lua_pushnumber(&L,12.75); lua_pushinteger(&L,-9);
  lua_pushboolean(&L,-4); lua_pushnil(&L);
  assert(lua_gettop(&L)==4 && lua_tonumber(&L,1)==12.75);
  assert(lua_tointeger(&L,1)==12 && lua_tointeger(&L,2)==-9);
  assert(lua_toboolean(&L,3)==1 && lua_toboolean(&L,-1)==0);
  assert(lua_type(&L,5)==LUA_TNONE && lua_type(&L,-2)==LUA_TBOOLEAN);
  assert(lua_toboolean(&L,5)==0);
  lua_pushvalue(&L,2); assert(lua_tointeger(&L,-1)==-9);
  lua_insert(&L,1); assert(lua_tointeger(&L,1)==-9);
  lua_remove(&L,2); assert(lua_gettop(&L)==4);
  lua_settop(&L,6); assert(lua_type(&L,5)==LUA_TNIL && lua_type(&L,6)==LUA_TNIL);
  lua_settop(&L,-3); assert(lua_gettop(&L)==4);
  lua_settop(&L,0); lua_pushinteger(&L,11); lua_pushinteger(&L,22);
  lua_xmove(&L,&L,2); assert(lua_gettop(&L)==2);
  lua_xmove(&L,&other,2); assert(lua_gettop(&L)==0 && lua_gettop(&other)==2);
  assert(lua_tointeger(&other,1)==11 && lua_tointeger(&other,2)==22);
  assert(lua_checkstack(&L,2049)==0 && lua_checkstack(&L,8)==1);
  g.l_registry.tt=LUA_TBOOLEAN; g.l_registry.value.b=1;
  L.l_gt.tt=LUA_TBOOLEAN; L.l_gt.value.b=0;
  assert(lua_toboolean(&L,LUA_REGISTRYINDEX)==1);
  assert(lua_toboolean(&L,LUA_GLOBALSINDEX)==0);
  closure.cl.c.isC=1; closure.cl.c.nupvalues=1;
  closure.cl.c.upvalue[0].tt=LUA_TNUMBER; closure.cl.c.upvalue[0].value.n=17;
  stack[0].tt=LUA_TFUNCTION; stack[0].value.gc=&closure; L.top=stack+1;
  assert(lua_getupvalue(&L,1,0)==NULL && lua_gettop(&L)==1);
  assert(strcmp(lua_getupvalue(&L,1,1),"")==0 && lua_tointeger(&L,-1)==17);
  lua_settop(&L,1); lua_pushinteger(&L,31);
  assert(lua_setupvalue(&L,1,2)==NULL && lua_gettop(&L)==2);
  assert(strcmp(lua_setupvalue(&L,1,1),"")==0 && lua_gettop(&L)==1);
  assert(closure.cl.c.upvalue[0].value.n==31);
  *lua_abortaddress(&L)=1; assert(L.abort==1);
  g.totalbytes=3*1024+7;
  assert(lua_gc(&L,LUA_GCCOUNT,0)==3 && lua_gc(&L,LUA_GCCOUNTB,0)==7);
  lua_gc(&L,LUA_GCSTOP,0); assert(g.GCthreshold==(lu_mem)-3);
  lua_gc(&L,LUA_GCRESTART,0); assert(g.GCthreshold==g.totalbytes);
  assert(lua_gc(&L,LUA_GCSTEP,1)==1 && steps==1);
  lua_gc(&L,LUA_GCCOLLECT,0); assert(collections==1);
  g.gcpause=100; assert(lua_gc(&L,LUA_GCSETPAUSE,200)==100 && g.gcpause==200);
  g.gcstepmul=150; assert(lua_gc(&L,LUA_GCSETSTEPMUL,250)==150 && g.gcstepmul==250);
  assert(lua_gc(&L,-1,0)==-1);
  puts("Lua API native checks passed");
}
