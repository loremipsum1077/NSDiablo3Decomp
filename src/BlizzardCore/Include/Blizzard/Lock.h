#pragma once

#include "Sys/Atomic.h"
#include "Sys/Lock_NX64.h"
#include "Time.h"
#include "Types.h"

namespace Blizzard {
namespace Lock {
struct Event;
struct Semaphore;
struct Mutex;
struct Spinlock;

void EventCreate(Event& event);
void EventDestroy(Event& event);
void EventSet(Event& event);
void EventWaitForever(Event& event);
Event* EventWait(Time::Millisecond waitMs, Event& event);

void SemaphoreCreate(Semaphore& sem);
void SemaphoreDestroy(Semaphore& sem);
void SemaphoreSet(Semaphore& sem);
void SemaphoreWaitForever(Semaphore& sem);
Semaphore* SemaphoreWait(Time::Millisecond waitMs, Semaphore& sem);

void MutexCreate(Mutex& mutex);
void MutexEnter(Mutex& mutex);
void MutexLeave(Mutex& mutex);
void MutexDestroy(Mutex& mutex);

struct Event : public System_Lock::Event {  // size 0xc
    void Create();
    void Destroy();
    void Set();
    void WaitForever();
    Event* Wait(Time::Millisecond waitMs);
};
static_assert(sizeof(Event) == 0xc, "Event");

struct Semaphore : public System_Lock::Semaphore {  // size 0x28
    void Create();
    void Destroy();
    void Set();
    void WaitForever();
    Semaphore* Wait(Time::Millisecond waitMs);
};
static_assert(sizeof(Semaphore) == 0x28, "Semaphore");

struct Mutex : public System_Lock::Mutex {  // size 0x20
    void Create();
    void Destroy();
    void Enter();
    void Leave();
};
static_assert(sizeof(Mutex) == 0x20, "Mutex");

struct Spinlock {  // size 0x10
    void Create();
    void Destroy();
    void Enter();
    void Leave();
    volatile int nLock;  // 0x00
    Event tEvent;  // 0x04
};
static_assert(sizeof(Spinlock) == 0x10, "Spinlock");

struct RSpinlock : public Spinlock {  // size 0x10
    RSpinlock();
    ~RSpinlock();

private:
    void Create();
    void Destroy();
    RSpinlock(const RSpinlock&);
    RSpinlock& operator=(const RSpinlock&);
};
static_assert(sizeof(RSpinlock) == 0x10, "RSpinlock");

struct REvent : public Event {  // size 0xc
    REvent();
    ~REvent();

private:
    void Create();
    void Destroy();
    REvent(const REvent&);
    REvent& operator=(const REvent&);
};
static_assert(sizeof(REvent) == 0xc, "REvent");

struct RSemaphore : public Semaphore {  // size 0x28
    RSemaphore();
    ~RSemaphore();

private:
    void Create();
    void Destroy();
    RSemaphore(const RSemaphore&);
    RSemaphore& operator=(const RSemaphore&);
};
static_assert(sizeof(RSemaphore) == 0x28, "RSemaphore");

struct RMutex : public Mutex {  // size 0x20
    RMutex();
    ~RMutex();

private:
    void Create();
    void Destroy();
    RMutex(const RMutex&);
    RMutex& operator=(const RMutex&);
};
static_assert(sizeof(RMutex) == 0x20, "RMutex");

// Lock.h:95 (DWARF). Ctor and dtor are inline at lines 248-249; their bodies are written from the
// copies inlined into DisplayList::SubmitRenderableDetails/SubmitRenderableHeader (SubObjectGfx.cpp),
// which inline SpinlockEnter/SpinlockLeave directly.
struct ScopedSpinlock {  // size 0x8
    ScopedSpinlock(Spinlock& mutex);
    ~ScopedSpinlock();

protected:
    Spinlock* m_spinlock;  // 0x00
};
static_assert(sizeof(ScopedSpinlock) == 0x8, "ScopedSpinlock");

struct ScopedMutex {  // size 0x8
    ScopedMutex(Mutex& mutex);
    ~ScopedMutex();

protected:
    Mutex* m_mutex;  // 0x00
};
static_assert(sizeof(ScopedMutex) == 0x8, "ScopedMutex");

typedef ScopedMutex ScopedRMutex;

inline void SpinlockCreate(Spinlock& spinlock);
inline void SpinlockEnter(Spinlock& spinlock);
inline void SpinlockLeave(Spinlock& spinlock);
inline void SpinlockDestroy(Spinlock& spinlock);

inline void Event::Create() { EventCreate(*this); }
inline void Event::Destroy() { EventDestroy(*this); }
inline void Event::Set() { EventSet(*this); }
inline void Event::WaitForever() { EventWaitForever(*this); }
inline Event* Event::Wait(Time::Millisecond waitMs) { return EventWait(waitMs, *this); }

inline void Semaphore::Create() { SemaphoreCreate(*this); }
inline void Semaphore::Destroy() { SemaphoreDestroy(*this); }
inline void Semaphore::Set() { SemaphoreSet(*this); }
inline void Semaphore::WaitForever() { SemaphoreWaitForever(*this); }
inline Semaphore* Semaphore::Wait(Time::Millisecond waitMs) { return SemaphoreWait(waitMs, *this); }

inline void Mutex::Create() { MutexCreate(*this); }
inline void Mutex::Destroy() { MutexDestroy(*this); }
inline void Mutex::Enter() { MutexEnter(*this); }
inline void Mutex::Leave() { MutexLeave(*this); }

inline void Spinlock::Create() { SpinlockCreate(*this); }
inline void Spinlock::Destroy() { SpinlockDestroy(*this); }
inline void Spinlock::Enter() { SpinlockEnter(*this); }
inline void Spinlock::Leave() { SpinlockLeave(*this); }

inline RSpinlock::RSpinlock() { Spinlock::Create(); }
inline RSpinlock::~RSpinlock() { Spinlock::Destroy(); }

inline REvent::REvent() { Event::Create(); }
inline REvent::~REvent() { Event::Destroy(); }

inline RSemaphore::RSemaphore() { Semaphore::Create(); }
inline RSemaphore::~RSemaphore() { Semaphore::Destroy(); }

inline RMutex::RMutex() { Mutex::Create(); }
inline RMutex::~RMutex() { Mutex::Destroy(); }

inline ScopedMutex::ScopedMutex(Mutex& mutex) : m_mutex(&mutex) { m_mutex->Enter(); }
inline ScopedMutex::~ScopedMutex() { m_mutex->Leave(); }

inline ScopedSpinlock::ScopedSpinlock(Spinlock& mutex) : m_spinlock(&mutex) { SpinlockEnter(*m_spinlock); }  // line 248
inline ScopedSpinlock::~ScopedSpinlock() { SpinlockLeave(*m_spinlock); }  // line 249

// Lock.h:432-459. The bodies are not in the DWARF; they are written from the inlined copies in
// X/Client/CGame.cpp (CGameAsyncRenderGetMemory and the static initialiser).
inline void SpinlockCreate(Spinlock& spinlock) {
    spinlock.nLock = 0;
    spinlock.tEvent.Create();
}

inline void SpinlockEnter(Spinlock& spinlock) {
    uint32 dwSpins = 0;  // invented name
    while (!Atomic::CompareAndSwap(0, 1, (volatile int32*)&spinlock.nLock)) {
        if (++dwSpins >= 100) {
            spinlock.tEvent.WaitForever();
            dwSpins = 0;
        }
    }
}

inline void SpinlockLeave(Spinlock& spinlock) {
    Atomic::ReadWriteBarrier();
    spinlock.nLock = 0;
    spinlock.tEvent.Set();
}

inline void SpinlockDestroy(Spinlock& spinlock) {
    spinlock.tEvent.Destroy();
}
// Lock.h:113 (RefCounted). Constructor and destructor are inline (lines 417-418); the zeroed count
// is from Console::GamerProfile::SaveTask::SaveTask (GamerProfileNX64.cpp).
class RefCounted {  // size 0x10
public:
    int Acquire() { return Atomic::Increment(&m_RefCount); }  // Lock.h:401
    int Release() { int32 nRefCount = Atomic::Decrement(&m_RefCount); if (nRefCount == 0) delete this; return nRefCount; }  // Lock.h:402
    int GetRefCountForDebugging();
    RefCounted() : m_RefCount(0) {}
    // The original body is not empty (clang keeps the RefCounted vptr store in every derived
    // destructor only then), but it compiles to nothing; what it held is unknown.
    virtual ~RefCounted() { ; }
private:
    RefCounted(const Blizzard::Lock::RefCounted &);
    Blizzard::Lock::RefCounted & operator=(const Blizzard::Lock::RefCounted &);
    Blizzard::Lock::RefCounted * operator&();
    int32 m_RefCount;  // 0x08
};
}  // namespace Lock
}  // namespace Blizzard
