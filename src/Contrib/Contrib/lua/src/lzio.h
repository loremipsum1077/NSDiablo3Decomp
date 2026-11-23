// Mbuffer layout adopted from lzio.h DWARF (size 0x18).
#pragma once
#include <stddef.h>
typedef struct Mbuffer { char *buffer; size_t n; size_t buffsize; } Mbuffer;

// Zio and stream macros recovered from Lua 5.1.1 and original DWARF.
#include "lmem.h"
// lua.h-owned callback declaration; its current public subset requires C++.
typedef const char *(*lua_Reader)(lua_State *L, void *data, size_t *size);
typedef struct Zio ZIO;
struct Zio {
    size_t n;
    const char *p;
    lua_Reader reader;
    void *data;
    lua_State *L;
};
LUA_LAYOUT_ASSERT(sizeof(ZIO) == 0x28, "Zio");
#define EOZ (-1)
#define char2int(c) cast(int, cast(unsigned char, (c)))
// Expand the existing lmem.h allocation API for a char vector.
#define luaZ_resizebuffer(L, buff, size) \
    ((buff)->buffer = cast(char *, luaM_reallocv(L, (buff)->buffer, \
        (buff)->buffsize, size, sizeof(char))), (buff)->buffsize = (size))
int luaZ_fill(ZIO *z);
int luaZ_lookahead(ZIO *z);
void luaZ_init(lua_State *L, ZIO *z, lua_Reader reader, void *data);
size_t luaZ_read(ZIO *z, void *b, size_t n);
char *luaZ_openspace(lua_State *L, Mbuffer *buff, size_t n);
/* Lua 5.1.1 lzio.h buffer macros */
#define luaZ_initbuffer(L, buff) ((buff)->buffer = NULL, (buff)->buffsize = 0)
#define luaZ_freebuffer(L, buff)	luaZ_resizebuffer(L, buff, 0)

/* Lua 5.1.1 buffer capacity accessor. */
#define luaZ_sizebuffer(buff) ((buff)->buffsize)

/* Lua 5.1.1 lzio.h stream and buffer access macros */
#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))
#define luaZ_buffer(buff)	((buff)->buffer)
#define luaZ_bufflen(buff)	((buff)->n)
#define luaZ_resetbuffer(buff) ((buff)->n = 0)
