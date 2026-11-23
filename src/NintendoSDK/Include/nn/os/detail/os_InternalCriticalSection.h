// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: NintendoSDK/Include/nn/os/detail/os_InternalCriticalSection.h

#pragma once

#include "os_InternalCriticalSection-os.horizon.h"
#include "../../util/util_TypedStorage.h"

namespace nn {
namespace os {
namespace detail {
class InternalCriticalSection {  // size 0x4
public:
    void Initialize();
    void Finalize();
    void Enter();
    bool TryEnter();
    void Leave();
    bool IsLockedByCurrentThread() const;
    void lock();
    void unlock();
    nn::os::detail::InternalCriticalSectionImpl * Get();
private:
    nn::os::detail::InternalCriticalSectionImpl m_Impl;  // 0x00
};
static_assert(sizeof(InternalCriticalSection) == 0x4, "InternalCriticalSection");

typedef nn::util::TypedStorage<nn::os::detail::InternalCriticalSection, 4, 4> InternalCriticalSectionStorage;

}  // namespace detail
}  // namespace os
}  // namespace nn
