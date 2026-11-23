// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Error.h

#pragma once

namespace Blizzard {
namespace Debug {
enum ErrorCategory {
    ERR_BLIZZARD_FILE = 1,
    ERR_BLIZZARD_LOCK = 2,
    ERR_BLIZZARD_MEMORY = 3,
    ERR_BLIZZARD_MOPAQ = 4,
    ERR_STREAMING = 20,
    ERR_HTTP = 21,
    ERR_STORAGE = 22,
    ERR_OS_WIN32 = 30,
    ERR_OS_MAC = 31,
};

class ErrorStackRecord;
typedef Blizzard::Debug::ErrorStackRecord * ErrorStack;

typedef bool (*ErrorStackCallback)(Blizzard::Debug::ErrorStack, void *);

class ErrorStackRecord {  // size 0x28
public:
    Blizzard::Debug::ErrorStackRecord * prev;  // 0x00
    int level;  // 0x08
    char * flattened;  // 0x10
    char * str_;  // 0x18
    int code_;  // 0x20
    int category_;  // 0x24
    int GetLevel();
    const char * GetFlattenedString();
    int GetCategory();
    int GetCode();
    const char * GetString();
    bool FindCategoryCode(int category, int code, Blizzard::Debug::ErrorStackCallback cb, void * data);
    bool FindCategoryCode(int, int);
    bool FindCategory(int category, Blizzard::Debug::ErrorStackCallback cb, void * data);
    bool FindCategory(int);
    bool FindCode(int code, Blizzard::Debug::ErrorStackCallback cb, void * data);
    bool FindCode(int);
    Blizzard::Debug::ErrorStack AddErrorToStack(int, const char *, int);
};
static_assert(sizeof(ErrorStackRecord) == 0x28, "ErrorStackRecord");

// BlizzardCore/Source/System/Debug/Error.cpp:155, 233.
ErrorStack AddErrorToStack(ErrorStack cur, int errCode, const char * errStr, int category);
void FreeErrorStack(ErrorStack err);

}  // namespace Debug
}  // namespace Blizzard
