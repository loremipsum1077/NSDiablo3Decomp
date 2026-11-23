/*
** $Id: llex.h,v 1.58 2006/03/23 18:23:32 roberto Exp $
** Lexical Analyzer
** See Copyright Notice in lua.h
*/

#ifndef llex_h
#define llex_h

#include "lobject.h"
#include "lzio.h"


#define FIRST_RESERVED	257

/* maximum length of a reserved word */
#define TOKEN_LEN	(sizeof("function")/sizeof(char))


/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER RESERVED"
*/
enum RESERVED {
  /* terminal symbols denoted by reserved words */
  TK_AND = FIRST_RESERVED, TK_BREAK,
  TK_DO, TK_ELSE, TK_ELSEIF, TK_END, TK_FALSE, TK_FOR, TK_FUNCTION,
  TK_IF, TK_IN, TK_LOCAL, TK_NIL, TK_NOT, TK_OR, TK_REPEAT,
  TK_RETURN, TK_THEN, TK_TRUE, TK_UNTIL, TK_WHILE,
  /* other terminal symbols */
  TK_CONCAT, TK_DOTS, TK_EQ, TK_GE, TK_LE, TK_NE, TK_NUMBER,
  TK_NAME, TK_STRING, TK_EOS
};

/* number of reserved words */
#define NUM_RESERVED	(cast(int, TK_WHILE-FIRST_RESERVED+1))


/* array with token `names' */
LUAI_DATA const char *const luaX_tokens [];


typedef union {
  lua_Number r;
  TString *ts;
} SemInfo;  /* semantics information */


typedef struct Token {
  int token;
  SemInfo seminfo;
} Token;


typedef struct LexState {
  int current;  /* current character (charint) */
  int linenumber;  /* input line counter */
  int lastline;  /* line of last token `consumed' */
  Token t;  /* current token */
  Token lookahead;  /* look ahead token */
  struct FuncState *fs;  /* `FuncState' is private to the parser */
  struct lua_State *L;
  ZIO *z;  /* input stream */
  Mbuffer *buff;  /* buffer for tokens */
  TString *source;  /* current source name */
  char decpoint;  /* locale decimal point */
} LexState;


LUAI_FUNC void luaX_init (lua_State *L);
LUAI_FUNC void luaX_setinput (lua_State *L, LexState *ls, ZIO *z,
                              TString *source);
LUAI_FUNC TString *luaX_newstring (LexState *ls, const char *str, size_t l);
LUAI_FUNC void luaX_next (LexState *ls);
LUAI_FUNC void luaX_lookahead (LexState *ls);
LUAI_FUNC void luaX_lexerror (LexState *ls, const char *msg, int token);
LUAI_FUNC void luaX_syntaxerror (LexState *ls, const char *s);
LUAI_FUNC const char *luaX_token2str (LexState *ls, int token);



// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
_Static_assert(sizeof(SemInfo) == 0x8, "SemInfo");
_Static_assert(sizeof(struct Token) == 0x10, "Token");
_Static_assert(offsetof(struct Token, token) == 0x0, "Token.token");
_Static_assert(offsetof(struct Token, seminfo) == 0x8, "Token.seminfo");
_Static_assert(sizeof(struct LexState) == 0x60, "LexState");
_Static_assert(offsetof(struct LexState, current) == 0x0, "LexState.current");
_Static_assert(offsetof(struct LexState, linenumber) == 0x4, "LexState.linenumber");
_Static_assert(offsetof(struct LexState, lastline) == 0x8, "LexState.lastline");
_Static_assert(offsetof(struct LexState, t) == 0x10, "LexState.t");
_Static_assert(offsetof(struct LexState, lookahead) == 0x20, "LexState.lookahead");
_Static_assert(offsetof(struct LexState, fs) == 0x30, "LexState.fs");
_Static_assert(offsetof(struct LexState, L) == 0x38, "LexState.L");
_Static_assert(offsetof(struct LexState, z) == 0x40, "LexState.z");
_Static_assert(offsetof(struct LexState, buff) == 0x48, "LexState.buff");
_Static_assert(offsetof(struct LexState, source) == 0x50, "LexState.source");
_Static_assert(offsetof(struct LexState, decpoint) == 0x58, "LexState.decpoint");

#endif
