// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/detail/os_InternalConditionVariable-os.horizon.h

#pragma once

#include "../os_ConditionVariableCommon.h"
#include <stddef.h>
#include <stdint.h>

namespace nn { namespace os { namespace detail { class InternalCriticalSection; } } }
namespace nn { namespace os { namespace detail { class TimeoutHelper; } } }

namespace nn {
namespace os {
namespace detail {
class InternalConditionVariableImplByHorizon {  // size 0x4
public:
    InternalConditionVariableImplByHorizon();
    void Initialize();
    void Signal();
    void Broadcast();
    void Wait(nn::os::detail::InternalCriticalSection *);
    nn::os::ConditionVariableStatus TimedWait(nn::os::detail::InternalCriticalSection *, const nn::os::detail::TimeoutHelper &);
private:
    uint32_t m_Value;  // 0x00
};
static_assert(sizeof(InternalConditionVariableImplByHorizon) == 0x4, "InternalConditionVariableImplByHorizon");

typedef nn::os::detail::InternalConditionVariableImplByHorizon InternalConditionVariableImpl;

}  // namespace detail
}  // namespace os
}  // namespace nn
