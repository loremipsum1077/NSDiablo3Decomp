// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformThread/bdPlatformThread.h

#pragma once

#include "../bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdPlatformThread {  // size 0x1
public:
    static bdBool createThread(bdThreadProc threadProc, void * args, bdThreadHandle & handle, bdInt priority, bdUInt stackSize, void * stackPointer, bdInt coreAffinity, const bdNChar8 *);
    static bdBool startThread(bdThreadHandle & handle, void *, bdUInt);
    static bdBool nameThread(bdThreadHandle &, const bdNChar8 *);
    static void joinThread(bdThreadHandle & handle);
    static void cancelThread(bdThreadHandle &);
    static void deleteThread(bdThreadHandle & handle);
    static void setPriority(bdThreadHandle &, const bdInt);
    static bdInt getPriority(bdThreadHandle &);
    static bdInt getPriority();
    static bdThreadHandle getThreadHandle();
private:
    bdPlatformThread();
};
static_assert(sizeof(bdPlatformThread) == 0x1, "bdPlatformThread");

