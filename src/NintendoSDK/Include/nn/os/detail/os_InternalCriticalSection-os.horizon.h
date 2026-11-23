// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: NintendoSDK/Include/nn/os/detail/os_InternalCriticalSection-os.horizon.h

#pragma once

#include "../../nn_BitTypes.h"

namespace nn {
namespace os {
namespace detail {
class InternalCriticalSectionImplByHorizon {  // size 0x4
public:
    InternalCriticalSectionImplByHorizon();
    void Initialize();
    void Finalize();
    void Enter();
    bool TryEnter();
    void Leave();
    bool IsLockedByCurrentThread() const;
    void lock();
    void unlock();
private:
    nn::Bit32 m_ThreadHandle;  // 0x00
};
static_assert(sizeof(InternalCriticalSectionImplByHorizon) == 0x4, "InternalCriticalSectionImplByHorizon");

typedef nn::os::detail::InternalCriticalSectionImplByHorizon InternalCriticalSectionImpl;

}  // namespace detail
}  // namespace os
}  // namespace nn
