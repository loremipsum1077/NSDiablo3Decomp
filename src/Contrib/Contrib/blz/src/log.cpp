#include "../include/blz/log.h"

#include <stdarg.h>

namespace blz {

// Defined in blz/src/format.cpp (declared by blz/format.h).
int printf(const char* format, ...);
int vprintf(const char* format, va_list arglist);

// line 14
void _log(const char* category, log_priority pri, const char* file, unsigned int line, const char* fmt, ...) {
    va_list arglist;
    va_start(arglist, fmt);
    printf("[%s/%d] [%s:%d]: ", category, pri, file, line);
    vprintf(fmt, arglist);
    va_end(arglist);
    printf("\n");
}

}  // namespace blz
