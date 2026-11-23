// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_ThreadTypes.h

#pragma once

#include "detail/os_InternalConditionVariable.h"
#include "detail/os_InternalCriticalSection.h"
#include "detail/os_ThreadTypes-os.horizon.h"
#include "os_ThreadCommon.h"
#include "../util/util_TypedStorage.h"
#include <stddef.h>
#include <stdint.h>

namespace nn { namespace os { struct FiberType; } }
namespace nn { namespace os { namespace detail { class MultiWaitObjectList; } } }
namespace nn { namespace util { struct IntrusiveListNode; } }

namespace nn {
namespace os {
struct ThreadType {  // size 0x1c0
    nn::util::TypedStorage<nn::util::IntrusiveListNode, 16, 8> _allThreadsListNode;  // 0x00
    nn::util::TypedStorage<nn::os::detail::MultiWaitObjectList, 16, 8> _multiWaitObjectList;  // 0x10
    uint8_t _gap20[0x20];  // 0x20 (members absent from the DWARF; placeholder)
    uint8_t _state;  // 0x40
    bool _stackIsAliased;  // 0x41
    bool _autoRegistered;  // 0x42
    uint8_t _suspendCount;  // 0x43
    int _basePriority;  // 0x44
    void * _originalStack;  // 0x48
    void * _stack;  // 0x50
    size_t _stackSize;  // 0x58
    void * _argument;  // 0x60
    nn::os::ThreadFunction _threadFunction;  // 0x68
    nn::os::FiberType * _currentFiber;  // 0x70
    nn::os::FiberType * _initialFiber;  // 0x78
    uint32_t _lockHistory;  // 0x80
    uintptr_t _tlsValueArray[32];  // 0x88
    char _threadNameBuffer[32];  // 0x188
    const char * _namePointer;  // 0x1a8
    nn::os::detail::InternalCriticalSectionStorage _csThread;  // 0x1b0
    nn::os::detail::InternalConditionVariableStorage _cvThread;  // 0x1b4
    nn::os::detail::InternalThreadHandle _handle;  // 0x1b8
};
static_assert(sizeof(ThreadType) == 0x1c0, "ThreadType");

}  // namespace os
}  // namespace nn

// Variables defined in this file:
//   line   142  static const int nn::os::IdealCoreDontCare
