// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_EventTypes.h

#pragma once

#include "detail/os_InternalConditionVariable.h"
#include "detail/os_InternalCriticalSection.h"
#include "../util/util_TypedStorage.h"

#include <stddef.h>
#include <stdint.h>
// #include <nn/os/detail/os_InternalConditionVariable.h>  // Nintendo SDK
// #include <nn/os/detail/os_InternalCriticalSection.h>  // Nintendo SDK

namespace nn { namespace os { namespace detail { class MultiWaitObjectList; } } }

namespace nn {
namespace os {
struct EventType {  // size 0x28
    nn::util::TypedStorage<nn::os::detail::MultiWaitObjectList, 16, 8> _multiWaitObjectList;  // 0x00
    bool _signalState;  // 0x10
    bool _initiallySignaled;  // 0x11
    uint8_t _clearMode;  // 0x12
    uint8_t _state;  // 0x13
    uint32_t _broadcastCounterLower;  // 0x14
    uint32_t _broadcastCounterUpper;  // 0x18
    nn::os::detail::InternalCriticalSectionStorage _csEvent;  // 0x1c
    nn::os::detail::InternalConditionVariableStorage _cvSignaled;  // 0x20
};
static_assert(sizeof(EventType) == 0x28, "EventType");

}  // namespace os
}  // namespace nn
