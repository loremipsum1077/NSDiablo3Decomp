#pragma once

// blz logging. _log is out of line in blz/src/log.cpp: it prints
// "[category/pri] [file:line]: ", the formatted message and a newline.

#include "config.h"

namespace blz {

// line 14
enum log_priority {
    log_normal = 0,
    log_debug = 1,
    log_spam = 2,
};

void _log(const char* category, log_priority pri, const char* file, unsigned int line, const char* fmt, ...);

}  // namespace blz
