// Original file: Contrib/Contrib/DemonWare/bdCore/bdThread/bdThread.h
// Blank lines keep the original line numbers (bdThread allocator at line 41).

#pragma once

#include "../../bdPlatform/bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"

class bdRunnable;
class bdThread;

struct bdThreadArgs {  // size 0x18
    // line numbers of the allocator macro in this struct are not recorded (invented position)
    BD_CLASS_ALLOCATOR(bdThreadArgs);
    bdThreadArgs() : m_args(0), m_runnable(0), m_thread(0) {}
    void * m_args;  // 0x00
    bdRunnable * m_runnable;  // 0x08
    bdThread * m_thread;  // 0x10
};
static_assert(sizeof(bdThreadArgs) == 0x18, "bdThreadArgs");

class bdThread {  // size 0x1f8
public:
















    BD_CLASS_ALLOCATOR(bdThread);
    bdThread(bdRunnable * runnable, bdInt priority, bdUInt stackSize, bdInt coreAffinity);
    bdBool start(const void * args, const bdUWord size, const bdNChar8 * threadName);
    bool getThreadName(bdNChar8 *);
    void stop();
    void join();
    void cancel();
    void cleanup();
    void setPriority(bdInt);
    bdInt getPriority();
    bdBool isRunning() const;
    bdRunnable * getRunnable();
    bdThreadHandle getThreadHandle();
    void wakeupJoiningThread();
protected:
    ~bdThread();
    bdRunnable * m_runnable;  // 0x00
    bdThreadArgs m_threadArgs;  // 0x08
    bdThreadHandle m_handle;  // 0x20
    bdBool m_isRunning;  // 0x1e0
    bdUInt m_stackSize;  // 0x1e4
    bdUInt m_startPriority;  // 0x1e8
    bdInt m_coreAffinity;  // 0x1ec
    static bdInt threadProc(bdThreadArgs * args);
    void * m_stackPointer;  // 0x1f0
};
static_assert(sizeof(bdThread) == 0x1f8, "bdThread");
