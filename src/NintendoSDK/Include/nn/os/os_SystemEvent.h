// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_SystemEvent.h

#pragma once

#include "os_EventCommon.h"
#include "os_NativeHandleTypes.h"
#include "os_SystemEventTypes.h"

namespace nn { class TimeSpan; }

namespace nn {
namespace os {
class SystemEvent {  // size 0x30
    SystemEvent(const nn::os::SystemEvent &);
    nn::os::SystemEvent & operator=(const nn::os::SystemEvent &);
    SystemEvent(nn::os::SystemEvent &&);
    nn::os::SystemEvent & operator=(nn::os::SystemEvent &&);
public:
    SystemEvent();
    explicit SystemEvent(nn::os::EventClearMode, bool);
    explicit SystemEvent(nn::os::NativeHandle, bool, nn::os::NativeHandle, bool, nn::os::EventClearMode);
    ~SystemEvent();
    void Attach(nn::os::NativeHandle, bool, nn::os::NativeHandle, bool, nn::os::EventClearMode);
    void AttachReadableHandle(nn::os::NativeHandle, bool, nn::os::EventClearMode);
    void AttachWritableHandle(nn::os::NativeHandle, bool, nn::os::EventClearMode);
    nn::os::NativeHandle DetachReadableHandle();
    nn::os::NativeHandle DetachWritableHandle();
    void Wait();
    bool TryWait();
    bool TimedWait(nn::TimeSpan);
    void Signal();
    void Clear();
    nn::os::NativeHandle GetReadableHandle() const;
    nn::os::NativeHandle GetWritableHandle() const;
    operator nn::os::SystemEventType &();
    operator const nn::os::SystemEventType &() const;
    nn::os::SystemEventType * GetBase();
private:
    nn::os::SystemEventType m_SystemEvent;  // 0x00
};
static_assert(sizeof(SystemEvent) == 0x30, "SystemEvent");

}  // namespace os
}  // namespace nn

// Functions defined in this file (bodies are not in the DWARF):
//   line    60  nn::os::SystemEvent::SystemEvent()  [inlined into 3 functions]
//   line   126  nn::os::SystemEvent::~SystemEvent()  [inlined into 2 functions]
//   line   260  bool nn::os::SystemEvent::TryWait()  [inlined into 3 functions]
//   line   307  void nn::os::SystemEvent::Clear()  [inlined into 2 functions]

// Bodies of the inline members NX64NFP.cpp needs (lines 60, 126 and 260 of the original).
namespace nn { namespace os {
bool TryWaitSystemEvent(SystemEventType *);  // nn/os/os_SystemEvent.h
void DestroySystemEvent(SystemEventType *);  // nn/os/os_SystemEvent.h
void ClearSystemEvent(SystemEventType *);  // nn/os/os_SystemEvent.h
} }
inline nn::os::SystemEvent::SystemEvent() { m_SystemEvent._state = nn::os::SystemEventType::State_NotInitialized; }
inline nn::os::SystemEvent::~SystemEvent()
{
    if (m_SystemEvent._state != nn::os::SystemEventType::State_NotInitialized)
    {
        nn::os::DestroySystemEvent(&m_SystemEvent);
    }
}
inline bool nn::os::SystemEvent::TryWait() { return nn::os::TryWaitSystemEvent(&m_SystemEvent); }
// Original line 307; inferred body (inlined into DoStartDetection as a ClearSystemEvent call).
inline void nn::os::SystemEvent::Clear() { nn::os::ClearSystemEvent(&m_SystemEvent); }
inline nn::os::SystemEventType * nn::os::SystemEvent::GetBase() { return &m_SystemEvent; }
