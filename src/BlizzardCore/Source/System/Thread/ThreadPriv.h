// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Source/System/Thread/ThreadPriv.h

#pragma once

#include "../../../Include/Blizzard/Lock.h"
#include "../../../Include/Blizzard/Thread.h"
#include "../../../Include/Blizzard/Time.h"
#include "../../../Include/Blizzard/Types.h"
#include "../../../../NintendoSDK/Include/nn/os/os_ThreadTypes.h"
#include <stddef.h>
#include <stdint.h>

namespace System_Thread {
class ThreadWorker {  // size 0x50
public:
    ThreadWorker();
    ~ThreadWorker();
    Blizzard::Thread::Thread physical;  // 0x00
    Blizzard::Thread::ThreadPool pool;  // 0x08
    Blizzard::Lock::REvent activate;  // 0x10
    Blizzard::Lock::REvent idle;  // 0x1c
    Blizzard::Thread::Thread logical;  // 0x28
    Blizzard::Thread::ThreadProc taskFunction;  // 0x30
    void * taskParam;  // 0x38
    Blizzard::Thread::EThreadPriority priority_saved;  // 0x40
    Blizzard::Thread::Affinity affinity_saved;  // 0x44
    bool Init(Blizzard::Thread::ThreadPool, int32, int, const Blizzard::Thread::Affinity &);
    static uint Executive(void *);

private:
    ThreadWorker(const System_Thread::ThreadWorker &);
    System_Thread::ThreadWorker & operator=(const System_Thread::ThreadWorker &);
};
static_assert(sizeof(ThreadWorker) == 0x50, "ThreadWorker");
}  // namespace System_Thread

namespace Blizzard {
namespace Thread {
struct ThreadRecord {  // size 0x218
    nn::os::ThreadType * threadId;  // 0x00
    nn::os::ThreadType thread;  // 0x08
    void * stack;  // 0x1c8
    Blizzard::Thread::Affinity threadAffinity;  // 0x1d0
    void * threadParam;  // 0x1d8
    Blizzard::Thread::ThreadProc threadFunc;  // 0x1e0
    int32 start;  // 0x1e8
    int32 refCount;  // 0x1ec
    uint threadReturn;  // 0x1f0
    System_Thread::ThreadWorker * worker;  // 0x1f8
    Blizzard::Time::Timestamp deadlockStamp;  // 0x200
    Blizzard::Time::Timestamp deadlockThreshold;  // 0x208
    char debugName[1];  // 0x210
    ThreadRecord();
    void printInfo(char *, size_t) const;
};
static_assert(sizeof(ThreadRecord) == 0x218, "ThreadRecord");
}  // namespace Thread
}  // namespace Blizzard
