#pragma once

#include "../Types.h"
#include "../../../../NintendoSDK/Include/nn/os/os_MutexTypes.h"

// MutexType uses its original SDK owner. LightEventType retains its recovered
// DWARF layout below; Semaphore still keeps only its size.
namespace nn {
namespace os {
struct ThreadType;

struct LightEventType {  // size 0xc
    struct {
        int __a_;
    } _signalState;  // 0x00
    bool _isAutoClear;  // 0x04
    bool _isInitialized;  // 0x05
    uint8 _padding1[2];  // 0x06
    uint32 _padding2;  // 0x08
};
static_assert(sizeof(LightEventType) == 0xc, "LightEventType");
}  // namespace os
}  // namespace nn

namespace System_Lock {
struct Event {  // size 0xc
    nn::os::LightEventType tEvent;  // 0x00
};
static_assert(sizeof(Event) == 0xc, "Event");

struct Semaphore {  // size 0x28
    uint64 tSemaphore[5];  // 0x00
};
static_assert(sizeof(Semaphore) == 0x28, "Semaphore");

struct Mutex {  // size 0x20
    nn::os::MutexType tMutex;  // 0x00
};
static_assert(sizeof(Mutex) == 0x20, "Mutex");
}  // namespace System_Lock
