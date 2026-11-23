// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_SemaphoreTypes.h

#pragma once

#include "detail/os_InternalConditionVariable.h"
#include "detail/os_InternalCriticalSection.h"
#include "../util/util_TypedStorage.h"
#include <stddef.h>
#include <stdint.h>

namespace nn { namespace os { namespace detail { class MultiWaitObjectList; } } }

namespace nn {
namespace os {
struct SemaphoreType {  // size 0x28
    nn::util::TypedStorage<nn::os::detail::MultiWaitObjectList, 16, 8> _multiWaitObjectList;  // 0x00
    uint8_t _state;  // 0x10
    int _count;  // 0x14
    int _maxCount;  // 0x18
    nn::os::detail::InternalCriticalSectionStorage _csSemaphore;  // 0x1c
    nn::os::detail::InternalConditionVariableStorage _cvNotZero;  // 0x20
};
static_assert(sizeof(SemaphoreType) == 0x28, "SemaphoreType");

}  // namespace os
}  // namespace nn
