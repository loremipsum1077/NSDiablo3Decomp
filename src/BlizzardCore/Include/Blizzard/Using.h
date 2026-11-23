#pragma once

#include "Types.h"

namespace Blizzard {
namespace Using {
typedef void (*ShutdownProc)();

void StartUsing(const char* usingName);
void StopUsing(const char* usingName);
void Register(const char* usingName, ShutdownProc shutdownProc);
void DependsOn(const char* usingName, const char* dependsOnName);

// Using.h:94-108. The bodies are not in the DWARF; they are written from the copies inlined into
// the static initialiser of System_File.cpp and the out-of-line destructor there.
class ScopedUse {  // size 0x8
public:
    ScopedUse(const char* usingName) : m_name(usingName) { StartUsing(usingName); }
    ScopedUse(const char* usingName, ShutdownProc shutdown) : m_name(usingName) {
        StartUsing(usingName);
        Register(m_name, shutdown);
    }
    ~ScopedUse() { StopUsing(m_name); }

protected:
    const char* m_name;  // 0x00

private:
    ScopedUse();
    ScopedUse(const ScopedUse&);
    ScopedUse& operator=(const ScopedUse&);
};
}  // namespace Using
}  // namespace Blizzard

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(Blizzard::Using::ScopedUse) == 0x8, "Blizzard::Using::ScopedUse");
