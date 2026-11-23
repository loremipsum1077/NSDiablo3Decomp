// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCore.h

#pragma once

#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdCore {  // size 0x1
public:
    static void init(const bdBool defaultMemoryFunctions);
    static void quit();
    static bdBool m_initialized;
protected:
    bdCore();
};
static_assert(sizeof(bdCore) == 0x1, "bdCore");

