/* Exercise recovered ltable.c against a minimal allocator/GC/error host.
 * This is behavioral evidence, not instruction or VM integration equivalence. */
#include "ltable.h"
#include "lstate.h"
#include "lua.h"
#include <assert.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t live_bytes;
static int barriers;
static jmp_buf error_jump;
static char error_text[100];
const TValue luaO_nilobject_ = {{NULL}, LUA_TNIL};
void *luaM_realloc_(lua_State *L, void *p, size_t old, size_t size) {
  (void)L;
  live_bytes -= old;
  live_bytes += size;
  if (!size) { free(p); return NULL; }
  p = realloc(p, size); assert(p); return p;
}
void *luaM_toobig(lua_State *L) { (void)L; abort(); }
void luaC_link(lua_State *L, GCObject *o, lu_byte tt) {
  (void)L; o->gch.next = NULL; o->gch.tt = tt; o->gch.marked = 0;
}
void luaC_barrierback(lua_State *L, Table *t) {
  (void)L; ++barriers; t->marked &= ~4;
}
void luaG_runerror(lua_State *L, const char *fmt, ...) {
  va_list ap; (void)L; va_start(ap,fmt);
  vsnprintf(error_text,sizeof(error_text),fmt,ap); va_end(ap);
  longjmp(error_jump,1);
}
unsigned int twoto(unsigned int nPower) { return 1u << nPower; }
int luaO_log2(unsigned int x) {
  int n = -1; while (x) { ++n; x >>= 1; } return n;
}
int luaO_rawequalObj(const TValue *a,const TValue *b) {
  if (a->tt != b->tt) return 0;
  switch(a->tt) {
    case LUA_TNIL: return 1;
    case LUA_TNUMBER: return a->value.n == b->value.n;
    case LUA_TBOOLEAN: return a->value.b == b->value.b;
    case LUA_TLIGHTUSERDATA: return a->value.p == b->value.p;
    default: return a->value.gc == b->value.gc;
  }
}
static void put_number(TValue *v, double n) { v->tt=LUA_TNUMBER; v->value.n=n; }
int main(void) {
  lua_State L = {0};
  Table *t = luaH_new(&L,0,0);
  assert(luaH_getn(t)==0);
  for (int i=1;i<=512;++i) put_number(luaH_setnum(&L,t,i),i*3);
  assert(luaH_getn(t)==512);
  for (int i=1;i<=512;++i) assert(luaH_getnum(t,i)->value.n==i*3);
  /* Sparse and negative integers stay in the hash part. */
  for (int i=1;i<=128;++i) put_number(luaH_setnum(&L,t,-i*1024),i);
  /* The internal resize-array API requires sufficient hash capacity for
   * displaced values; Lua normally calls it while constructing tables. */
  Table *shrunk = luaH_new(&L,512,1024);
  for (int i=1;i<=512;++i) put_number(luaH_setnum(&L,shrunk,i),i*3);
  luaH_resizearray(&L,shrunk,16);
  for (int i=1;i<=512;++i) assert(luaH_getnum(shrunk,i)->value.n==i*3);
  luaH_free(&L,shrunk);
  for (int i=1;i<=512;++i) assert(luaH_getnum(t,i)->value.n==i*3);
  for (int i=1;i<=128;++i) assert(luaH_getnum(t,-i*1024)->value.n==i);
  /* Distinct interned strings with the same hash exercise collision relocation. */
  TString keys[64] = {0};
  for (int i=0;i<64;++i) {
    keys[i].tsv.tt=LUA_TSTRING; keys[i].tsv.marked=1; keys[i].tsv.hash=42;
    t->marked=4;
    put_number(luaH_setstr(&L,t,&keys[i]),i+1000);
  }
  assert(barriers==64);
  for (int i=0;i<64;++i) assert(luaH_getstr(t,&keys[i])->value.n==i+1000);
  TValue fractional; put_number(&fractional,0.5);
  put_number(luaH_set(&L,t,&fractional),99);
  assert(luaH_get(t,&fractional)->value.n==99);
  TValue pointer={{NULL},LUA_TLIGHTUSERDATA}; pointer.value.p=&L;
  put_number(luaH_set(&L,t,&pointer),77);
  assert(luaH_get(t,&pointer)->value.n==77);
  TValue boolean={{NULL},LUA_TBOOLEAN}; boolean.value.b=1;
  put_number(luaH_set(&L,t,&boolean),88);
  assert(luaH_get(t,&boolean)->value.n==88);
  /* Deleting a hash value does not invalidate its key for next(). */
  TValue iteration[2] = {{{NULL},LUA_TNIL},{{NULL},LUA_TNIL}};
  int seen=0; while(luaH_next(&L,t,iteration)) ++seen;
  assert(seen==512+128+64+3);
  luaH_setnum(&L,t,512)->tt=LUA_TNIL;
  assert(luaH_getn(t)==511);
  luaH_setstr(&L,t,&keys[20])->tt=LUA_TNIL;
  iteration[0].tt=LUA_TSTRING; iteration[0].value.gc=(GCObject*)&keys[20];
  (void)luaH_next(&L,t,iteration);
  TValue invalid={{NULL},LUA_TNIL};
  if (!setjmp(error_jump)) { luaH_set(&L,t,&invalid); assert(0); }
  assert(!strcmp(error_text,"table index is nil"));
  put_number(&invalid,NAN);
  if (!setjmp(error_jump)) { luaH_set(&L,t,&invalid); assert(0); }
  assert(!strcmp(error_text,"table index is NaN"));
  put_number(&iteration[0],123456789);
  if (!setjmp(error_jump)) { luaH_next(&L,t,iteration); assert(0); }
  assert(!strcmp(error_text,"invalid key to 'next'"));
  luaH_free(&L,t); assert(live_bytes==0);
  puts("Lua tables: rehash, shrink, collisions, iteration, length, errors and allocation balance passed");
}
