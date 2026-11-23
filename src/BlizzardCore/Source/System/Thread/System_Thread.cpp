// Original file: BlizzardCore/Source/System/Thread/System_Thread.cpp
// The original is a unity file that includes Thread.cpp, ThreadAPI.cpp, ThreadLocalStorage.cpp,
// ThreadPools.cpp and Thread_NX64.cpp. Their functions are written here grouped by original file.
#include "ThreadPriv.h"
#include "../../../Include/Blizzard/Memory.h"
#include "../../../Include/Blizzard/Sys/Atomic.h"
#include "../../../Include/Blizzard/Using.h"
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Sys/Allocator_STL.h"
#include "../../../../NintendoSDK/Include/nn/nn_Result.h"

#include <new>
#include <set>
#include <vector>

// NintendoSDK declarations (nn/os/os_ThreadLocalStorageApi.h, os_ThreadApi.h). The functions are
// imports, so nothing in the image checks the prototypes.
namespace nn {
namespace os {
typedef void (*TlsDestructor)(uintptr_t);
nn::Result AllocateTlsSlot(TlsSlot * outTlsSlot, TlsDestructor function);
void FreeTlsSlot(TlsSlot tlsSlot);
uintptr_t GetTlsValue(TlsSlot tlsSlot);
void SetTlsValue(TlsSlot tlsSlot, uintptr_t value);
void DestroyThread(ThreadType * thread);
void WaitThread(ThreadType * thread);
int ChangeThreadPriority(ThreadType * thread, int priority);
void SetThreadCoreMask(ThreadType * thread, int idealCore, uint64_t affinityMask);
nn::Result CreateThread(ThreadType * thread, void (*function)(void *), void * argument, void * stack,
                        size_t stackSize, int priority);
void SetThreadName(ThreadType * thread, const char * threadName);
void StartThread(ThreadType * thread);
ThreadType * GetCurrentThread();
}  // namespace os
}  // namespace nn

namespace Blizzard {
namespace Process {
bool Sleep(Time::Millisecond milliseconds);
}  // namespace Process

namespace Thread {
namespace CreateAttributes {
enum {
    startSuspended = 1,
};
}  // namespace CreateAttributes

// ThreadPools.cpp
struct ThreadPoolRecord {  // size 0x60
    unsigned int initialSize;  // 0x00
    unsigned int maxSize;  // 0x04
    ThreadPoolRecord(unsigned int, unsigned int, Blizzard::Time::Timestamp, const Blizzard::Thread::Affinity &);
    ~ThreadPoolRecord();
    System_Thread::ThreadWorker * GetWorker(int32, int);
    Blizzard::Lock::RMutex lock;  // 0x08
    std::vector<System_Thread::ThreadWorker *> idleWorkers;  // 0x28
    std::vector<System_Thread::ThreadWorker *> workers;  // 0x40
    Blizzard::Thread::Affinity defaultAffinity;  // 0x58
};

// ThreadPriv.h:96
inline ThreadRecord::ThreadRecord()
    : threadId(NULL),
      thread(),
      stack(NULL),
      threadAffinity(Affinity::None()),
      threadParam(NULL),
      threadFunc(NULL),
      start(0),
      refCount(0),
      threadReturn(0)
{
    worker = NULL;
    deadlockStamp = 0;
    debugName[0] = '\0';
    deadlockThreshold = 0;
}
}  // namespace Thread
}  // namespace Blizzard

namespace System_Thread {
// ---- Thread.cpp (lines 34-62) ----
typedef std::set<Blizzard::Thread::ThreadRecord *, std::less<Blizzard::Thread::ThreadRecord *>,
                 Blizzard::Memory::AllocSTL_raw<Blizzard::Thread::ThreadRecord *> >
    ThreadRegistryRecord;

bool s_initialized;
Blizzard::Lock::Mutex s_mutex;
Blizzard::Thread::Thread s_mainThread;
Blizzard::Lock::Mutex s_registryMutex;
ThreadRegistryRecord * s_ThreadRegistry;

// ---- ThreadAPI.cpp (line 37) ----
int s_DefaultStackSize;

// ---- ThreadLocalStorage.cpp (lines 33-37) ----
static Blizzard::Thread::TLSSlot * s_slotList[128];
static unsigned int s_slotListUsed;
Blizzard::Thread::TLSSlot s_threadRecordTLS;
Blizzard::Thread::TLSSlot s_stackTraceEntryPointTLS;

// ---- ThreadPools.cpp (line 390) ----
static Blizzard::Thread::ThreadPool s_SystemThreadPool;

void InitThreadSystem();
void AddToRegistry(Blizzard::Thread::Thread thread);
void RemoveFromRegistry(Blizzard::Thread::Thread thread);
bool StartThread(Blizzard::Thread::Thread thread, int32, int stacksize);
void ThreadStarter(void * parms);
bool AllocateTLSSlot(Blizzard::Thread::TLSSlot * slot, Blizzard::Thread::TLSDestructor destructorFn);
bool AllocateLocalStorage(Blizzard::Thread::TLSSlot * slot, Blizzard::Thread::TLSDestructor destructorFn);
void AddToSlotList(Blizzard::Thread::TLSSlot * slot);
void RemoveFromSlotList(Blizzard::Thread::TLSSlot * slot);
void RunTLSDestructors();

// ---- Thread_NX64.cpp: inline pieces used by the other parts ----
static const int snPriorityLookup[7] = { 31, 30, 17, 16, 15, 1, 0 };

// line 105
inline bool InternalResume(Blizzard::Thread::Thread thread)
{
    thread->start = 1;
    return true;
}

// line 116
inline bool InternalWait(Blizzard::Thread::Thread thread)
{
    nn::os::WaitThread(thread->threadId);
    return true;
}

// line 151
inline bool InternalSetPriority(Blizzard::Thread::Thread thread, Blizzard::Thread::EThreadPriority priorityIn)
{
    if (static_cast<uint>(priorityIn) < sizeof(snPriorityLookup) / sizeof(snPriorityLookup[0]))
    {
        const int priority = snPriorityLookup[priorityIn];
        nn::os::ChangeThreadPriority(thread->threadId, priority);
        return true;
    }
    return false;
}

// line 302
inline void CloseThread(Blizzard::Thread::Thread thread)
{
    if (thread->threadId == &thread->thread)
    {
        nn::os::DestroyThread(thread->threadId);
    }
    if (thread->stack != NULL)
    {
        Blizzard::Memory::Free(thread->stack, 0, "", 0, NULL);
    }
}

// line 319
inline bool InternalAllocateLocalStorage(Blizzard::Thread::TLSSlot * slot, Blizzard::Thread::TLSDestructor destructorFn)
{
    slot->initialized =
        nn::os::AllocateTlsSlot(&slot->tSlot, reinterpret_cast<nn::os::TlsDestructor>(destructorFn)).IsSuccess();
    return slot->initialized;
}

// line 334
inline void InternalSetLocalStorage(const Blizzard::Thread::TLSSlot * slot, const void * valueIn)
{
    const uintptr_t value = reinterpret_cast<uintptr_t>(valueIn);
    nn::os::SetTlsValue(slot->tSlot, value);
}

// line 346
inline void * InternalGetLocalStorage(const Blizzard::Thread::TLSSlot * slot)
{
    const uintptr_t value = nn::os::GetTlsValue(slot->tSlot);
    return reinterpret_cast<void *>(value);
}

// line 357
inline void InternalFreeLocalStorage(Blizzard::Thread::TLSSlot * slot)
{
    nn::os::FreeTlsSlot(slot->tSlot);
    slot->initialized = false;
}

// line 368
inline bool TLSSlotIsAllocated(const Blizzard::Thread::TLSSlot * slot)
{
    return slot->initialized;
}

// ---- ThreadPools.cpp (lines 307-317) ----
inline Blizzard::Thread::Thread WorkerPhysical(ThreadWorker * worker)
{
    return worker->physical;
}

inline Blizzard::Thread::Thread WorkerLogical(ThreadWorker * worker)
{
    return worker->logical;
}

// line 292
inline void InternalInitMainThread(Blizzard::Thread::Thread thread)
{
    thread->threadId = nn::os::GetCurrentThread();
}

// ---- Thread.cpp ----
void ShutdownThreadSystem();
void NewMainThread();

// line 77
struct UsingThread : public Blizzard::Using::ScopedUse {  // size 0x8
    UsingThread() : ScopedUse("Blizzard::Thread", ShutdownThreadSystem)
    {
        Blizzard::Using::DependsOn("Blizzard::Thread", "Blizzard::Memory");
        Blizzard::Using::DependsOn("Blizzard::ThreadLocalStorage", "Blizzard::Thread");
        InitThreadSystem();
    }
};

// line 84
UsingThread s_usingThread;

// line 95
void InitThreadSystem()
{
    if (s_initialized)
    {
        return;
    }
    s_initialized = true;
    s_mutex.Create();
    s_registryMutex.Create();
    Blizzard::Thread::AllocateLocalStorage(&s_threadRecordTLS);
    Blizzard::Thread::AllocateLocalStorage(&s_stackTraceEntryPointTLS);
    NewMainThread();
}

#line 172
inline Blizzard::Thread::Thread NewThread(Blizzard::Thread::ThreadProc entry, void * params, const char * debugName)
{
    if (debugName == NULL)
    {
        debugName = "";
    }

    int debugNameLen = static_cast<int>(Blizzard::String::Length(debugName));
    Blizzard::Thread::Thread th = Blizzard::Memory::RawAlloc(sizeof(Blizzard::Thread::ThreadRecord) + debugNameLen + 1);
    Blizzard::String::MemFill(th, sizeof(Blizzard::Thread::ThreadRecord), 0);

    Blizzard::Thread::Thread thread = new (th) Blizzard::Thread::ThreadRecord();
    thread->threadParam = params;
    thread->threadFunc = entry;
    thread->refCount = 2;
    thread->start = 0;
    Blizzard::String::Copy(thread->debugName, debugName, debugNameLen + 1);


    AddToRegistry(thread);
    return thread;
}

#line 154
inline void NewMainThread()
{
    s_mainThread = NewThread(NULL, NULL, "");
    Blizzard::Thread::SetLocalStorage(&s_threadRecordTLS, s_mainThread);
    InternalInitMainThread(s_mainThread);
    s_mainThread->start = 1;
    s_mainThread->refCount = 1;
}

// line 231
void AddToRegistry(Blizzard::Thread::Thread thread)
{
    Blizzard::Lock::ScopedMutex mutex(s_registryMutex);
    if (s_ThreadRegistry == NULL)
    {
        s_ThreadRegistry = new (static_cast<void *>(Blizzard::Memory::RawAlloc(sizeof(ThreadRegistryRecord)))) ThreadRegistryRecord;
    }
    s_ThreadRegistry->insert(thread);
}

// line 246
void RemoveFromRegistry(Blizzard::Thread::Thread thread)
{
    Blizzard::Lock::ScopedMutex mutex(s_registryMutex);
    if (s_ThreadRegistry != NULL)
    {
        s_ThreadRegistry->erase(thread);
        if (s_ThreadRegistry->empty())
        {
            s_ThreadRegistry->~ThreadRegistryRecord();
            Blizzard::Memory::RawFree(s_ThreadRegistry);
            s_ThreadRegistry = NULL;
        }
    }
}

// line 202
inline void FreeThread(Blizzard::Thread::Thread thread)
{
    RemoveFromRegistry(thread);
    Blizzard::Memory::RawFree(thread);
}

// ThreadPools.cpp:427, 433
inline bool ClosePool(Blizzard::Thread::ThreadPool pool)
{
    delete pool;
    return true;
}

inline bool CloseSystemThreadPool()
{
    if (s_SystemThreadPool != NULL)
    {
        ClosePool(s_SystemThreadPool);
        s_SystemThreadPool = NULL;
    }
    return true;
}

// line 129
void ShutdownThreadSystem()
{
    if (s_mainThread != NULL)
    {
        FreeThread(s_mainThread);
        s_mainThread = NULL;
    }
    CloseSystemThreadPool();
}
}  // namespace System_Thread

// ---- ThreadAPI.cpp ----
namespace Blizzard {
namespace Thread {
// line 62
bool Close(Thread thread)
{
    if (Lock::Atomic::Decrement(&thread->refCount) > 0)
    {
        return true;
    }
    System_Thread::CloseThread(thread);
    System_Thread::FreeThread(thread);
    return true;
}

// line 84
bool Create(ThreadProc entry, void * params, Thread * thread, const char * debugName, int32 attributes, int stacksize)
{
    System_Thread::InitThreadSystem();
    *thread = System_Thread::NewThread(entry, params, debugName);
    (*thread)->start = (attributes & CreateAttributes::startSuspended) ? 0 : 1;
    if (System_Thread::StartThread(*thread, attributes, stacksize))
    {
        return true;
    }
    System_Thread::FreeThread(*thread);
    *thread = NULL;
    return false;
}

// line 128
inline Thread CurrentPhysicalThread()
{
    System_Thread::InitThreadSystem();
    return static_cast<Thread>(GetLocalStorage(&System_Thread::s_threadRecordTLS));
}

// line 139
Thread CurrentThread()
{
    Thread thread = CurrentPhysicalThread();
    if (thread != NULL && thread->worker != NULL)
    {
        thread = System_Thread::WorkerLogical(thread->worker);
    }
    return thread;
}

// line 178
inline bool Exists(Thread thread)
{
    return thread != NULL && thread->start >= 0;
}

// line 193
bool IsMainThread()
{
    return CurrentThread() == System_Thread::s_mainThread;
}

// line 235
bool Resume(Thread thread)
{
    if (thread->worker != NULL)
    {
        if (System_Thread::WorkerLogical(thread->worker) == thread && thread->start == 0)
        {
            thread->start = 1;
            thread->worker->activate.Set();
        }
        return true;
    }
    return System_Thread::InternalResume(thread);
}

// line 285
void SetThreadAffinity(Thread thread, uint32 dwAffinityMask)
{
    nn::os::SetThreadCoreMask(thread->threadId, -1, dwAffinityMask);
}

// line 342
bool SetPriority(Thread thread, EThreadPriority priority)
{
    Thread phy_thread = thread;
    if (thread->worker != NULL && System_Thread::WorkerPhysical(thread->worker) != thread)
    {
        phy_thread = System_Thread::WorkerPhysical(thread->worker);
        if (System_Thread::WorkerLogical(thread->worker) != thread)
        {
            return false;
        }
    }
    return System_Thread::InternalSetPriority(phy_thread, priority);
}

// line 399
bool WaitForever(Thread thread)
{
    if (Exists(thread))
    {
        if (thread->worker == NULL || System_Thread::WorkerPhysical(thread->worker) == thread)
        {
            System_Thread::InternalWait(thread);
        }
        else if (System_Thread::WorkerLogical(thread->worker) == thread)
        {
            thread->worker->idle.WaitForever();
            thread->worker->idle.Set();
        }
    }
    return true;
}

// line 437
void SetDefaultStackSize(int bytes)
{
    System_Thread::s_DefaultStackSize = bytes;
}

// ---- ThreadLocalStorage.cpp ----
// line 60
void * RegisterLocalStorage(TLSSlot * slot, TLSConstructor constructorFn, void * constructorCookie,
                            TLSDestructor destructorFn)
{
    void * value;
    if (!System_Thread::TLSSlotIsAllocated(slot))
    {
        if (!System_Thread::AllocateTLSSlot(slot, destructorFn))
        {
            return NULL;
        }
    }
    else
    {
        value = GetLocalStorage(slot);
        if (value != NULL)
        {
            return value;
        }
    }
    value = constructorFn(constructorCookie);
    SetLocalStorage(slot, value);
    return value;
}

// line 111
bool AllocateLocalStorage(TLSSlot * slot)
{
    return System_Thread::AllocateLocalStorage(slot, NULL);
}

// line 121
void * GetLocalStorage(const TLSSlot * slot)
{
    return System_Thread::InternalGetLocalStorage(slot);
}

// line 133
void SetLocalStorage(const TLSSlot * slot, const void * value)
{
    System_Thread::InternalSetLocalStorage(slot, value);
}

// line 145
void FreeLocalStorage(TLSSlot * slot)
{
    System_Thread::InternalFreeLocalStorage(slot);
    System_Thread::RemoveFromSlotList(slot);
}
}  // namespace Thread
}  // namespace Blizzard

namespace System_Thread {
// line 195
inline bool AllocateLocalStorage(Blizzard::Thread::TLSSlot * slot, Blizzard::Thread::TLSDestructor destructorFn)
{
    InitThreadSystem();
    if (!InternalAllocateLocalStorage(slot, destructorFn))
    {
        return false;
    }
    slot->destructor = destructorFn;
    AddToSlotList(slot);
    return true;
}

// line 222
bool AllocateTLSSlot(Blizzard::Thread::TLSSlot * slot, Blizzard::Thread::TLSDestructor destructorFn)
{
    InitThreadSystem();
    Blizzard::Lock::ScopedMutex mutex(s_mutex);
    if (!TLSSlotIsAllocated(slot))
    {
        if (!AllocateLocalStorage(slot, destructorFn))
        {
            return false;
        }
    }
    return true;
}

// line 252
inline void AddToSlotList(Blizzard::Thread::TLSSlot * slot)
{
    Blizzard::Lock::ScopedMutex mutex(s_mutex);
    s_slotList[s_slotListUsed++] = slot;
}

// line 263
inline void RemoveFromSlotList(Blizzard::Thread::TLSSlot * slot)
{
    Blizzard::Lock::ScopedMutex mutex(s_mutex);
    for (uint32 index = 0; index < s_slotListUsed; index++)
    {
        if (s_slotList[index] == slot)
        {
            s_slotList[index] = s_slotList[s_slotListUsed - 1];
            s_slotList[s_slotListUsed - 1] = NULL;
            s_slotListUsed--;
            break;
        }
    }
}

// line 291
inline void RunTLSDestructors()
{
    Blizzard::Lock::ScopedMutex mutex(s_mutex);
    for (uint32 index = 0; index < s_slotListUsed; index++)
    {
        Blizzard::Thread::TLSDestructor destructorFn = s_slotList[index]->destructor;
        if (destructorFn != NULL)
        {
            void * value = Blizzard::Thread::GetLocalStorage(s_slotList[index]);
            if (value != NULL)
            {
                destructorFn(value);
            }
        }
    }
}

void RunExternalTLSDestructors();

// line 331
struct RunTLSDestructorsOnMainThread : public Blizzard::Using::ScopedUse {  // size 0x8
    RunTLSDestructorsOnMainThread() : ScopedUse("Blizzard::ThreadLocalStorage", RunExternalTLSDestructors) {}
};

// line 338
RunTLSDestructorsOnMainThread s_registerMainThreadTLSDestructors;

// ---- Thread_NX64.cpp (line 379) ----
void RunExternalTLSDestructors()
{
    if (s_initialized)
    {
        RunTLSDestructors();
    }
}

// ---- ThreadPools.cpp ----
// line 114
ThreadWorker::~ThreadWorker()
{
    activate.Set();
    idle.WaitForever();
    if (physical != NULL)
    {
        activate.Set();
        InternalWait(physical);
        Blizzard::Thread::Close(physical);
        physical = NULL;
    }
}
}  // namespace System_Thread

namespace Blizzard {
namespace Thread {
// The constructor, CreatePool and CreateSystemThreadPool were removed by the linker, so nothing
// in the image checks them. They are kept, in the smallest form the declarations allow, because
// they are the only writers of s_SystemThreadPool: without them ShutdownThreadSystem loses its
// pool clean-up. The constructor body and the pool arguments other than the two sizes are guesses.
ThreadPoolRecord::ThreadPoolRecord(unsigned int initial, unsigned int maxSimultaneous, Blizzard::Time::Timestamp,
                                   const Blizzard::Thread::Affinity & affinity)
    : initialSize(initial), maxSize(maxSimultaneous), defaultAffinity(affinity)
{
}

// line 241
ThreadPoolRecord::~ThreadPoolRecord()
{
    {
        Blizzard::Lock::ScopedRMutex mutex(lock);
        initialSize = 0;
        idleWorkers.clear();
    }
    for (std::vector<System_Thread::ThreadWorker *>::iterator it = workers.begin(); it != workers.end(); ++it)
    {
        delete *it;
    }
    Blizzard::Lock::ScopedRMutex mutex(lock);
    workers.clear();
}

// line 402
bool CreatePool(ThreadPool & pool, unsigned int initial, unsigned int maxSimultaneous,
                Blizzard::Time::Timestamp timeToLive, Affinity defaultAffinity)
{
    pool = new ThreadPoolRecord(initial, maxSimultaneous, timeToLive, defaultAffinity);
    return true;
}
}  // namespace Thread
}  // namespace Blizzard

namespace System_Thread {
static const int SYSTEM_THREAD_POOL_INIT_SIZE = 1;
static const int SYSTEM_THREAD_POOL_MAX_SIZE = 0;

// line 451
bool CreateSystemThreadPool()
{
    return Blizzard::Thread::CreatePool(s_SystemThreadPool, SYSTEM_THREAD_POOL_INIT_SIZE, SYSTEM_THREAD_POOL_MAX_SIZE, 0,
                                        Blizzard::Thread::Affinity::None());
}
}  // namespace System_Thread

// ---- Thread_NX64.cpp (lines 29-76) ----
static inline void * AlignMemory(void * const memory, const size_t align)
{
    const uintptr_t pointer = reinterpret_cast<uintptr_t>(memory);
    const uintptr_t pointerAligned = (pointer + align - 1) & ~(align - 1);
    return reinterpret_cast<void *>(pointerAligned);
}

namespace System_Thread {
// line 37
bool StartThread(Blizzard::Thread::Thread thread, int32, int stacksize)
{
    if (s_DefaultStackSize != 0 && stacksize == 0)
    {
        stacksize = s_DefaultStackSize;
    }
    void * stackAligned;
    size_t stacksizeAligned = stacksize;
    void * stack = Blizzard::Memory::Allocate(stacksize + 0xfff, 0, "", 0, NULL);
    stackAligned = AlignMemory(stack, 0x1000);
    nn::os::ThreadType * const threadId = &thread->thread;
    const int priority = 16;
    const nn::Result result =
        nn::os::CreateThread(threadId, ThreadStarter, thread, stackAligned, stacksizeAligned, priority);
    if (result.IsSuccess())
    {
        nn::os::SetThreadName(threadId, thread->debugName);
        thread->threadId = threadId;
        thread->stack = stack;
        nn::os::StartThread(threadId);
        return true;
    }
    Blizzard::Memory::Free(stack, 0, "", 0, NULL);
    return false;
}

// line 76
void ThreadStarter(void * parms)
{
    uint retval;
    Blizzard::Thread::Thread thread = static_cast<Blizzard::Thread::Thread>(parms);
    Blizzard::Thread::SetLocalStorage(&s_threadRecordTLS, thread);
    while (thread->start == 0)
    {
        Blizzard::Process::Sleep(1);
    }
    retval = thread->threadFunc(thread->threadParam);
    thread->start = -1;
    thread->threadReturn = retval;
    Blizzard::Thread::Close(thread);
}
}  // namespace System_Thread
