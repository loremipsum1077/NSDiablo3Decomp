/* Host behavior checks for recovered lfunc.c/lstring.c. The VM and collector
 * are deliberately absent: allocation and GC linking below are test doubles. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Contrib/Contrib/lua/src/lua.h"
#include "Contrib/Contrib/lua/src/lstate.h"
#include "Contrib/Contrib/lua/src/lfunc.h"
#include "Contrib/Contrib/lua/src/lstring.h"

static unsigned frees, closes;
void *luaM_realloc_(lua_State *L, void *p, size_t oldsize, size_t size) {
  (void)L; (void)oldsize;
  if (!size) { if (p) ++frees; free(p); return NULL; }
  p = realloc(p, size); assert(p); return p;
}
void *luaM_toobig(lua_State *L) { (void)L; abort(); }
void luaC_link(lua_State *L, GCObject *o, lu_byte tt) {
  o->gch.next = G(L)->rootgc; G(L)->rootgc = o;
  o->gch.tt = tt; o->gch.marked = G(L)->currentwhite;
}
void luaC_linkupval(lua_State *L, UpVal *uv) {
  ++closes; luaC_link(L, (GCObject *)uv, LUA_TUPVAL);
}

int main(void) {
  global_State g = {0}; lua_State L = {0}; TValue stack[4] = {0};
  L.l_G = &g; g.mainthread = &L; g.currentwhite = 1;
  g.uvhead.u.l.next = g.uvhead.u.l.prev = &g.uvhead;
  luaS_resize(&L, 2);
  TString *a = luaS_newlstr(&L, "a\0b", 3);
  assert(a->tsv.len == 3 && !memcmp(a+1, "a\0b\0", 4));
  assert(luaS_newlstr(&L, "a\0b", 3) == a);
  a->tsv.marked = 2; assert(luaS_newlstr(&L, "a\0b", 3) == a);
  assert(a->tsv.marked == 1);
  for (int i=0; i<40; ++i) { char s[2] = {(char)i, 'x'}; luaS_newlstr(&L,s,2); }
  assert(g.strt.nuse == 41 && g.strt.size >= 32);
  luaS_resize(&L, 128); assert(luaS_newlstr(&L,"a\0b",3) == a);
  g.gcstate = 2; luaS_resize(&L,256); assert(g.strt.size == 128); g.gcstate=0;
  Table env = {0}; Udata *u = luaS_newudata(&L,17,&env);
  assert(u->uv.len == 17 && u->uv.env == &env && !u->uv.metatable);
  assert(L.next == (GCObject *)u && u->uv.tt == LUA_TUSERDATA);
  free(u);
  Closure *c = luaF_newCclosure(&L,3,&env), *l = luaF_newLclosure(&L,3,&env);
  assert(c->c.isC == 1 && c->c.nupvalues == 3 && c->c.env == &env);
  assert(!l->l.isC && l->l.nupvalues == 3 && !l->l.upvals[0] && !l->l.upvals[2]);
  luaF_freeclosure(&L,c); luaF_freeclosure(&L,l); g.rootgc = NULL;
  UpVal *closed = luaF_newupval(&L);
  assert(closed->v == &closed->u.value && closed->v->tt == LUA_TNIL);
  luaF_freeupval(&L,closed); g.rootgc = NULL;
  stack[1].tt = LUA_TNUMBER; stack[1].value.n = 42;
  UpVal *low=luaF_findupval(&L,stack+1), *high=luaF_findupval(&L,stack+3);
  assert(L.openupval == (GCObject *)high && high->next == (GCObject *)low);
  low->marked=2; assert(luaF_findupval(&L,stack+1) == low && low->marked == 1);
  high->marked=2; unsigned before=frees;
  luaF_close(&L,stack+2); assert(frees == before+1 && L.openupval == (GCObject *)low);
  luaF_close(&L,stack); assert(!L.openupval && closes == 1);
  assert(low->v == &low->u.value && low->v->tt == LUA_TNUMBER && low->v->value.n == 42);
  assert(g.uvhead.u.l.next == &g.uvhead && g.uvhead.u.l.prev == &g.uvhead);
  luaF_freeupval(&L,low); g.rootgc=NULL;
  Proto *p=luaF_newproto(&L);
  assert(!p->k && !p->p && !p->code && !p->source && !p->sizecode && !p->nups);
  p->locvars=calloc(2,sizeof(LocVar)); p->sizelocvars=2;
  p->locvars[0]=(LocVar){a,0,5}; p->locvars[1]=(LocVar){a,2,8};
  assert(luaF_getlocalname(p,2,2) == (char *)(a+1));
  assert(!luaF_getlocalname(p,2,5) && !luaF_getlocalname(p,1,8));
  luaF_freeproto(&L,p);
  for(int i=0;i<g.strt.size;++i) { GCObject *o=g.strt.hash[i]; while(o) {GCObject *n=o->gch.next;free(o);o=n;} }
  free(g.strt.hash);
  return 0;
}
