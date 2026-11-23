/* Original file: Contrib/Contrib/PCRE/pcre_globals.c (PCRE 8.35). The allocation hooks the
image keeps; pcre_stack_malloc and pcre_stack_free (lines 80-81) were dropped by the linker. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pcre_internal.h"





































































PCRE_EXP_DATA_DEFN void *(*PUBL(malloc))(size_t) = malloc;
PCRE_EXP_DATA_DEFN void  (*PUBL(free))(void *) = free;


PCRE_EXP_DATA_DEFN int   (*PUBL(callout))(PUBL(callout_block) *) = NULL;
PCRE_EXP_DATA_DEFN int   (*PUBL(stack_guard))(void) = NULL;
