// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/detail/os_InternalConditionVariable.h

#pragma once

#include "os_InternalConditionVariable-os.horizon.h"
#include "../os_ConditionVariableCommon.h"
#include "../../util/util_TypedStorage.h"

namespace nn { namespace os { namespace detail { class InternalCriticalSection; } } }
namespace nn { namespace os { namespace detail { class TimeoutHelper; } } }

namespace nn {
namespace os {
namespace detail {
class InternalConditionVariable {  // size 0x4
public:
    void Initialize();
    void Signal();
    void Broadcast();
    void Wait(nn::os::detail::InternalCriticalSection *);
    nn::os::ConditionVariableStatus TimedWait(nn::os::detail::InternalCriticalSection *, const nn::os::detail::TimeoutHelper &);
private:
    nn::os::detail::InternalConditionVariableImpl m_Impl;  // 0x00
};
static_assert(sizeof(InternalConditionVariable) == 0x4, "InternalConditionVariable");

typedef nn::util::TypedStorage<nn::os::detail::InternalConditionVariable, 4, 4> InternalConditionVariableStorage;

}  // namespace detail
}  // namespace os
}  // namespace nn
