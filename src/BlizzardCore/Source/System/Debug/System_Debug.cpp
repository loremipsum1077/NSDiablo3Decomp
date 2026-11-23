// Original file: BlizzardCore/Source/System/Debug/System_Debug.cpp
// The original is a unity file that includes Debug.cpp and ErrorStack.cpp.
#include "../../../Include/Blizzard/Debug.h"
#include "../../../Include/Blizzard/Error.h"
#include "../../../Include/Blizzard/Memory.h"
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Util.h"

#include <new>

// NintendoSDK declaration (nn/diag/diag_Debugger.h).
namespace nn {
namespace diag {
bool IsDebuggerAttached();
}  // namespace diag
}  // namespace nn

namespace Blizzard {
namespace Debug {
// Debug.cpp:270
bool AmIBeingDebugged()
{
    return nn::diag::IsDebuggerAttached();
}

// ErrorStack.cpp:32
bool g_ErrorStackEnabled = true;

// ErrorStack.cpp:155
ErrorStack AddErrorToStack(ErrorStack cur, int errCode, const char * errStr, int category)
{
    if (!g_ErrorStackEnabled)
    {
        return NULL;
    }
    size_t strSize = String::Length(errStr) + 1;
    int level;
    const char * prevFlattened;
    if (cur == NULL)
    {
        level = 0;
        prevFlattened = "";
    }
    else
    {
        prevFlattened = cur->flattened;
        level = cur->level + 1;
    }
    size_t flattenedMax = strSize + String::Length(prevFlattened) + 100;
    if (flattenedMax > 4096)
    {
        flattenedMax = 4096;
    }
    Util::Buffer<2048, char> flattened(flattenedMax);
    String::Format(flattened, static_cast<uint32>(flattenedMax), "[%d] err=%d text=%s\n%s", level, errCode, errStr,
                   prevFlattened);
    size_t flattenedSize = String::Length(flattened) + 1;
    ErrorStackRecord * record = Memory::Allocate(sizeof(ErrorStackRecord));
    record->prev = cur;
    record->level = level;
    record->code_ = errCode;
    record->category_ = category;
    record->str_ = Memory::Allocate(static_cast<uint32>(strSize));
    String::Copy(record->str_, errStr, strSize);
    record->flattened = Memory::Allocate(static_cast<uint32>(flattenedSize));
    String::Copy(record->flattened, flattened, flattenedSize);
    if (cur == NULL)
    {
        cur = record;
    }
    else
    {
        ErrorStackRecord * prev = cur->prev;
        ErrorStackRecord temp;
        String::MemCopy(&temp, cur, sizeof(ErrorStackRecord));
        String::MemCopy(cur, record, sizeof(ErrorStackRecord));
        String::MemCopy(record, &temp, sizeof(ErrorStackRecord));
        cur->prev = record;
        record->prev = prev;
    }
    if (cur->level > 15)
    {
        ErrorStackRecord * last = record;
        while (last->prev->prev != NULL)
        {
            last = last->prev;
        }
        Memory::Free(last->prev->flattened);
        Memory::Free(last->prev->str_);
        Memory::Free(last->prev);
        last->prev = NULL;
    }
    return cur;
}

// ErrorStack.cpp:233
void FreeErrorStack(ErrorStack err)
{
    while (err != NULL)
    {
        ErrorStack prev = err->prev;
        Memory::Free(err->flattened);
        Memory::Free(err->str_);
        Memory::Free(err);
        err = prev;
    }
}
}  // namespace Debug
}  // namespace Blizzard
