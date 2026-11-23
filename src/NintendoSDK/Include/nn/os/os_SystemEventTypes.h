// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_SystemEventTypes.h

#pragma once

#include "detail/os_InternalConditionVariable.h"
#include "detail/os_InternalCriticalSection.h"
#include "../util/util_TypedStorage.h"

#include "os_EventTypes.h"
#include "os_NativeHandleTypes.h"
#include <stddef.h>
#include <stdint.h>

namespace nn { namespace os { namespace detail { class MultiWaitObjectList; } } }

namespace nn {
namespace os {
namespace detail {
struct InterProcessEventType {  // size 0x20
    nn::util::TypedStorage<nn::os::detail::MultiWaitObjectList, 16, 8> _multiWaitObjectList;  // 0x00
    bool _autoClear;  // 0x10
    uint8_t _state;  // 0x11
    bool _isReadableHandleManaged;  // 0x12
    bool _isWritableHandleManaged;  // 0x13
    nn::os::NativeHandle _readableHandle;  // 0x14
    nn::os::NativeHandle _writableHandle;  // 0x18
};
static_assert(sizeof(InterProcessEventType) == 0x20, "InterProcessEventType");

}  // namespace detail
}  // namespace os
}  // namespace nn
namespace nn {
namespace os {
struct SystemEventType {  // size 0x30
    union {  // size 0x28
        nn::os::EventType _event;  // 0x00
        nn::os::detail::InterProcessEventType _interProcessEvent;  // 0x00
    };  // 0x00
    enum State {
        State_NotInitialized = 0,
        State_InitializedAsEvent = 1,
        State_InitializedAsInterProcessEvent = 2,
    };
    uint8_t _state;  // 0x28
};
static_assert(sizeof(SystemEventType) == 0x30, "SystemEventType");

}  // namespace os
}  // namespace nn
