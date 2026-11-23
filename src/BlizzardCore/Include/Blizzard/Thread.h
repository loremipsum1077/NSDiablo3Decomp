#pragma once

#include <stddef.h>
#include <new>

#include "Types.h"

// NintendoSDK/Include/nn/os/os_ThreadLocalStorageCommon.h
namespace nn {
namespace os {
struct TlsSlot {
    uint32_t _innerValue;
};
}  // namespace os
}  // namespace nn

namespace Blizzard {
namespace Thread {
struct ThreadRecord;

enum EThreadPriority {
    PRIORITY_IDLE = 0,
    PRIORITY_LOWEST = 1,
    PRIORITY_BELOW_NORMAL = 2,
    PRIORITY_NORMAL = 3,
    PRIORITY_ABOVE_NORMAL = 4,
    PRIORITY_HIGHEST = 5,
    PRIORITY_TIME_CRITICAL = 6,
};

typedef ThreadRecord* Thread;

// Thread/Extras.h
struct ThreadPoolRecord;
typedef ThreadPoolRecord* ThreadPool;

class Affinity {  // size 0x8
    enum Policy {
        SingleHWThread = 0,
        AllHWThreads = 1,
        NumPolicies = 2,
    };

public:
    Policy m_policy;  // 0x00
    int m_id;  // 0x04

private:
    Affinity(Policy, int);
    // Not in the DWARF: ThreadRecord's constructor leaves the affinity all zero.
    Affinity() : m_policy(SingleHWThread), m_id(0) {}

public:
    static Affinity HWThread(int);
    static Affinity None() { return Affinity(); }
};

typedef uint (*ThreadProc)(void*);

typedef void (*EnumCallback)(Thread, void*);

typedef void* (*TLSConstructor)(void*);

typedef void (*TLSDestructor)(void*);

struct TLSSlot {
    TLSSlot() {}
    void Prepare() {
        tSlot._innerValue = 0;
        initialized = false;
    }
    TLSDestructor destructor;
    bool initialized;
    nn::os::TlsSlot tSlot;

private:
    TLSSlot(const TLSSlot&);
    TLSSlot& operator=(const TLSSlot&);
};

Thread CurrentThread();
bool Create(ThreadProc entry, void* params, Thread* thread, const char* debugName, int32 attributes, int stacksize);
bool Resume(Thread thread);
bool SetPriority(Thread thread, EThreadPriority priority);
bool WaitForever(Thread thread);
bool Close(Thread thread);
bool AllocateLocalStorage(TLSSlot* slot);
void* GetLocalStorage(const TLSSlot* slot);
void SetLocalStorage(const TLSSlot* slot, const void* value);
void FreeLocalStorage(TLSSlot* slot);
void* RegisterLocalStorage(TLSSlot* slot, TLSConstructor constructor, void* userData, TLSDestructor destructor);

// Thread/Extras.h:115-136. The bodies are not in the DWARF; they are written from the
// LocalObject<System_File::FileError> copies in System_File.cpp.
template <typename T>
class LocalObject {  // size 0x10
public:
    LocalObject() {}
    operator T&() { return *static_cast<T*>(RegisterLocalStorage(&m_slot, New, NULL, Delete)); }

protected:
    static void* New(void*) { return new T; }
    static void Delete(void* ptr) { delete static_cast<T*>(ptr); }
    TLSSlot m_slot;  // 0x00

private:
    LocalObject(const LocalObject&);
    T& operator=(const LocalObject&);
};
}  // namespace Thread
}  // namespace Blizzard
