/* Behavior tests for recovered lobject.c. VM-dependent formatting is excluded. */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "Contrib/Contrib/lua/src/lua.h"
#include "Contrib/Contrib/lua/src/lstate.h"
#include "Contrib/Contrib/lua/src/lobject.h"

/* Missing VM services must not be reached by these object-only tests. */
TString *luaS_newlstr(lua_State *L, const char *s, size_t n) {
  (void)L; (void)s; (void)n; assert(!"unexpected string allocation"); return NULL;
}
void luaD_growstack(lua_State *L, int n) {
  (void)L; (void)n; assert(!"unexpected stack growth");
}
void luaV_concat(lua_State *L, int total, int last) {
  (void)L; (void)total; (void)last; assert(!"unexpected concatenation");
}
int main(void) {
  for (unsigned int i=0; i<32; ++i) assert(twoto(i) == (1U<<i));
  assert(luaO_log2(0) == -1);
  for (unsigned int i=1; i<65536; ++i) {
    unsigned int x=i; int expected=-1;
    while (x) { ++expected; x >>= 1; }
    assert(luaO_log2(i) == expected);
    int encoded=luaO_int2fb(i), decoded=luaO_fb2int(encoded);
    assert(decoded >= (int)i);
    if (i<16) assert(decoded == (int)i);
    if (encoded>0) assert(luaO_fb2int(encoded-1) < (int)i);
  }
  lua_Number n;
  assert(luaO_str2d("12.5", &n) && n==12.5);
  assert(luaO_str2d("0xFF", &n) && n==255);
  assert(luaO_str2d(" -42 \t\r\n\v\f", &n) && n==-42);
  assert(!luaO_str2d("", &n));
  assert(!luaO_str2d("abc", &n));
  assert(!luaO_str2d("123x!", &n));
  assert(!luaO_str2d("1\001", &n));
  TValue a={{NULL},LUA_TNIL}, b=a;
  assert(luaO_rawequalObj(&a,&b));
  b.tt=LUA_TBOOLEAN; b.value.b=0; assert(!luaO_rawequalObj(&a,&b));
  a=b; assert(luaO_rawequalObj(&a,&b)); b.value.b=1; assert(!luaO_rawequalObj(&a,&b));
  a.tt=b.tt=LUA_TNUMBER; a.value.n=0.0; b.value.n=-0.0;
  assert(luaO_rawequalObj(&a,&b)); a.value.n=b.value.n=NAN; assert(!luaO_rawequalObj(&a,&b));
  a.tt=b.tt=LUA_TLIGHTUSERDATA; a.value.p=&a; b.value.p=&a;
  assert(luaO_rawequalObj(&a,&b)); b.value.p=&b; assert(!luaO_rawequalObj(&a,&b));
  a.tt=b.tt=LUA_TSTRING; a.value.gc=(GCObject *)&a; b.value.gc=a.value.gc;
  assert(luaO_rawequalObj(&a,&b)); b.value.gc=(GCObject *)&b; assert(!luaO_rawequalObj(&a,&b));
  assert(luaO_nilobject_.tt==LUA_TNIL && luaO_nilobject_.value.gc==NULL);
  char out[60];
  luaO_chunkid(out,"=literal",sizeof(out)); assert(!strcmp(out,"literal"));
  luaO_chunkid(out,"=longliteral",5); assert(!strcmp(out,"long"));
  luaO_chunkid(out,"@path/script.lua",sizeof(out)); assert(!strcmp(out,"path/script.lua"));
  luaO_chunkid(out,"@0123456789abcdefghijklmnop",20); assert(!strcmp(out,"...efghijklmnop"));
  luaO_chunkid(out,"return 1",sizeof(out)); assert(!strcmp(out,"[string \"return 1\"]"));
  luaO_chunkid(out,"first\nsecond",sizeof(out)); assert(!strcmp(out,"[string \"first...\"]"));
  puts("Lua object behavior tests passed");
  return 0;
}
