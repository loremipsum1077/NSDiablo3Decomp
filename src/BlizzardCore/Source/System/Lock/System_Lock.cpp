// Original file: BlizzardCore/Source/System/Lock/System_Lock.cpp
// The original is a unity file that includes Event_NX64.cpp, Lock.cpp, Mutex_NX64.cpp and
// Semaphore_NX64.cpp (the DWARF decl_file of every function). Their bodies are written here in
// that order, each with a comment giving its original file and line. The linker-stripped
// EventCreate(Event&, const char*) and DoOnce(DoOnceData&, ...) are not written.
#include "../../../Include/Blizzard/Lock.h"
#include "../../../Include/Blizzard/Time.h"
#include <limits.h>

// NintendoSDK declarations (nn/nn_TimeSpan.h, nn/os/os_LightEvent.h, os_Mutex.h, os_Semaphore.h).
namespace nn {
struct TimeSpanType {  // size 0x8
    static TimeSpanType FromMilliSeconds(int64_t milliSeconds) {
        TimeSpanType timeSpan = {milliSeconds * 1000 * 1000};
        return timeSpan;
    }
    int64_t _nanoSeconds;  // 0x00
};

class TimeSpan {  // size 0x8
public:
    TimeSpan(const TimeSpanType& timeSpan) : m_TimeSpan(timeSpan) {}
    static TimeSpan FromMilliSeconds(int64_t milliSeconds) {
        return TimeSpan(TimeSpanType::FromMilliSeconds(milliSeconds));
    }

private:
    TimeSpanType m_TimeSpan;  // 0x00
};

namespace os {
struct SemaphoreType;

enum EventClearMode {
    EventClearMode_ManualClear = 0,
    EventClearMode_AutoClear = 1,
};

const int MutexLockLevelInitial = 0;

void InitializeLightEvent(LightEventType* event, bool isSignaled, EventClearMode clearMode);
void FinalizeLightEvent(LightEventType* event);
void SignalLightEvent(LightEventType* event);
void WaitLightEvent(LightEventType* event);
bool TimedWaitLightEvent(LightEventType* event, TimeSpan timeout);

void InitializeMutex(MutexType* mutex, bool isRecursive, int lockLevel);
void FinalizeMutex(MutexType* mutex);
void LockMutex(MutexType* mutex);
bool TryLockMutex(MutexType* mutex);
void UnlockMutex(MutexType* mutex);

void InitializeSemaphore(SemaphoreType* semaphore, int initialCount, int maxCount);
void FinalizeSemaphore(SemaphoreType* semaphore);
void AcquireSemaphore(SemaphoreType* semaphore);
bool TimedAcquireSemaphore(SemaphoreType* semaphore, TimeSpan timeout);
void ReleaseSemaphore(SemaphoreType* semaphore);
}  // namespace os
}  // namespace nn

namespace Blizzard {
namespace Process {
void Sleep(Time::Millisecond ms);
}  // namespace Process
}  // namespace Blizzard

namespace System_Lock {
Blizzard::Lock::Mutex* MutexTryEnter(Blizzard::Lock::Mutex& mutex);
}  // namespace System_Lock

// ---- Event_NX64.cpp
namespace Blizzard {
namespace Lock {
// Event_NX64.cpp:21
void EventCreate(Event& event)
{
    const bool signaled = false;
    const nn::os::EventClearMode clearMode = nn::os::EventClearMode_AutoClear;
    nn::os::InitializeLightEvent(&event.tEvent, signaled, clearMode);
}

// Event_NX64.cpp:29
void EventDestroy(Event& event)
{
    nn::os::FinalizeLightEvent(&event.tEvent);
}

// Event_NX64.cpp:35
void EventSet(Event& event)
{
    nn::os::SignalLightEvent(&event.tEvent);
}

// Event_NX64.cpp:41
void EventWaitForever(Event& event)
{
    nn::os::WaitLightEvent(&event.tEvent);
}

// Event_NX64.cpp:47
Event* EventWait(Time::Millisecond waitMs, Event& event)
{
    const nn::TimeSpan timeout = nn::TimeSpan::FromMilliSeconds(waitMs);
    const bool result = nn::os::TimedWaitLightEvent(&event.tEvent, timeout);
    return result ? &event : nullptr;
}

// ---- Lock.cpp

// Lock.cpp:50
Mutex* MutexEnter(Time::Millisecond waitMs, Mutex& mutex)
{
    if (waitMs == 0)
        return System_Lock::MutexTryEnter(mutex);

    Time::Millisecond start = Time::Milliseconds();
    for (;;)
    {
        if (System_Lock::MutexTryEnter(mutex))
            return &mutex;

        Time::Millisecond now = Time::Milliseconds();
        if (now - start >= waitMs)
            return nullptr;

        Process::Sleep(1);
    }
}

// ---- Mutex_NX64.cpp

// Mutex_NX64.cpp:18
void MutexCreate(Mutex& mutex)
{
    const bool recursive = true;
    const int lockLevel = nn::os::MutexLockLevelInitial;
    nn::os::InitializeMutex(&mutex.tMutex, recursive, lockLevel);
}

// Mutex_NX64.cpp:29
void MutexEnter(Mutex& mutex)
{
    nn::os::LockMutex(&mutex.tMutex);
}
}  // namespace Lock
}  // namespace Blizzard

// Mutex_NX64.cpp:39
Blizzard::Lock::Mutex* System_Lock::MutexTryEnter(Blizzard::Lock::Mutex& mutex)
{
    if (nn::os::TryLockMutex(&mutex.tMutex))
        return &mutex;
    return nullptr;
}

namespace Blizzard {
namespace Lock {
// Mutex_NX64.cpp:50
void MutexLeave(Mutex& mutex)
{
    nn::os::UnlockMutex(&mutex.tMutex);
}

// Mutex_NX64.cpp:59
void MutexDestroy(Mutex& mutex)
{
    nn::os::FinalizeMutex(&mutex.tMutex);
}

// ---- Semaphore_NX64.cpp
// Lock_NX64.h keeps only the size of System_Lock::Semaphore (the DWARF member is
// nn::os::SemaphoreType tSemaphore), so the SDK calls take its address as a SemaphoreType.

// Semaphore_NX64.cpp:15
void SemaphoreCreate(Semaphore& sem)
{
    const int initialCount = 0;
    const int maxCount = INT_MAX;
    nn::os::InitializeSemaphore(reinterpret_cast<nn::os::SemaphoreType*>(&sem.tSemaphore), initialCount, maxCount);
}

// Semaphore_NX64.cpp:23
void SemaphoreDestroy(Semaphore& sem)
{
    nn::os::FinalizeSemaphore(reinterpret_cast<nn::os::SemaphoreType*>(&sem.tSemaphore));
}

// Semaphore_NX64.cpp:29
void SemaphoreSet(Semaphore& sem)
{
    nn::os::ReleaseSemaphore(reinterpret_cast<nn::os::SemaphoreType*>(&sem.tSemaphore));
}

// Semaphore_NX64.cpp:35
void SemaphoreWaitForever(Semaphore& sem)
{
    nn::os::AcquireSemaphore(reinterpret_cast<nn::os::SemaphoreType*>(&sem.tSemaphore));
}

// Semaphore_NX64.cpp:41
Semaphore* SemaphoreWait(Time::Millisecond waitMs, Semaphore& sem)
{
    const nn::TimeSpan timeout = nn::TimeSpan::FromMilliSeconds(waitMs);
    const bool result = nn::os::TimedAcquireSemaphore(reinterpret_cast<nn::os::SemaphoreType*>(&sem.tSemaphore), timeout);
    return result ? &sem : nullptr;
}
}  // namespace Lock
}  // namespace Blizzard
