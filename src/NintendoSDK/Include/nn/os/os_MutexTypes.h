// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: NintendoSDK/Include/nn/os/os_MutexTypes.h

#pragma once

#include "detail/os_InternalCriticalSection.h"
#include <stddef.h>
#include <stdint.h>

namespace nn { namespace os { struct ThreadType; } }

namespace nn {
namespace os {
struct MutexType {  // size 0x20
    uint8_t _state;  // 0x00
    bool _isRecursive;  // 0x01
    int _lockLevel;  // 0x04
    int _nestCount;  // 0x08
    nn::os::ThreadType * _ownerThread;  // 0x10
    union {  // size 0x4
        int32_t _mutexImage[1];  // 0x00
        nn::os::detail::InternalCriticalSectionStorage _mutex;  // 0x00
    };  // 0x18
};
static_assert(sizeof(MutexType) == 0x20, "MutexType");

}  // namespace os
}  // namespace nn
