// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Debug.h

#pragma once

namespace Blizzard {
namespace Debug {
enum TraceFlags {
    c_stackTraceNoSymbols = 1,
    c_stackTraceWithSymbols = 0,
    c_stackTraceManual = 2,
    c_stackTraceDbgHelp = 0,
    c_stackTraceDefault = 0,
};

// BlizzardCore/Source/System/Debug/Debug.cpp:271.
bool AmIBeingDebugged();

}  // namespace Debug
}  // namespace Blizzard
