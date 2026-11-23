// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Battlenet/Subsystems/ToastManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Gfx/GfxCommon.h"
#include "../../UserInterface/UIID.h"
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

enum {
    TOAST_LOW = 0,
    TOAST_PRIORITY_START = 0,
    TOAST_MEDIUM = 1,
    TOAST_HIGH = 2,
    TOAST_NUM_PRIORITIES = 3,
};

typedef int32 ToastPriority;

typedef int32 ToastId;

class ToastManagerBase {  // size 0x1d8
    // vtable pointer
    typedef OnlineService::Callback<void ()> ToastOnClickCallback;
public:
    virtual void SendToast(CRefString sMessage, const ImageTextureFrame & tIconTexture, ToastPriority ePriority, ToastManagerBase::ToastOnClickCallback cb, const float flToastDurationTime);  // vtable slot 0
    virtual ToastId SendIndefiniteToast(CRefString sMessage, const ImageTextureFrame & tIconTexture, ToastPriority ePriority, ToastManagerBase::ToastOnClickCallback cb);  // vtable slot 1
    void ProcessToasts();
    void ClearToasts();
    void OutroToast(ToastId idToast);
    void ClearToast(ToastId idToast);
    BOOL IsEnabled() const;
    void SetEnabled(BOOL);
protected:
    ToastManagerBase(uint32, float, BOOL);
    virtual ~ToastManagerBase();  // vtable slot 0
    struct Toast {  // size 0x40
        // vtable pointer
        Toast() : nId(-1), flToastDuration(0.0f) {}  // line 90; body from the copy inlined into FanfareManager::SendFanfare
        virtual ~Toast() {}  // vtable slot 0; line 92
        CRefString sMessage;  // 0x08
        ImageTextureFrame tIcon;  // 0x20
        ToastManagerBase::ToastOnClickCallback cbOnClick;  // 0x28
        ToastId nId;  // 0x30
        float flToastDuration;  // 0x34
        UIID idToastWin;  // 0x38
    };
    virtual const UIID CreateToastVisual(int32, ToastManagerBase::Toast *) = 0;  // vtable slot 4
    virtual void DisposeToastVisual(ToastManagerBase::Toast *) = 0;  // vtable slot 5
    virtual void UpdateToastVisual(int32, ToastManagerBase::Toast *, float) {}  // vtable slot 6; line 110
    virtual void OutroToastVisual(ToastManagerBase::Toast * ptToast, float) { ClearToast(ptToast->nId); }  // vtable slot 7; line 113
    ToastManagerBase::Toast * GetCurrentToast(int32) const;
    const ToastManagerBase::Toast * GetQueueHead() const;
    int32 GetSlotForVisual(const UIID &) const;
    float GetDefaultToastDuration() const { return m_flDefaultToastDuration; }  // line 122
    void SetDefaultToastDuration(const float);
    ToastId GetNextToastId() {  // line 134
        if (m_nToastIdGenerator == -1)
        {
            m_nToastIdGenerator = 0;
        }
        return m_nToastIdGenerator++;
    }
    void RemoveToast(int32 nToastSlot);
    void AddToastToQueue(ToastManagerBase::Toast * ptToast, ToastPriority ePriority);
    typedef XGrowableList<ToastManagerBase::Toast *, ToastManagerBase::Toast *> ToastQueue;
    ToastManagerBase::ToastQueue m_toastQueue[3];  // 0x08
    blz::vector<ToastManagerBase::Toast *, blz::allocator<ToastManagerBase::Toast *> > m_currentToast;  // 0x110
private:
    void ProcessToast(int32 nToastSlot, ToastManagerBase::Toast * ptToast);
    void UpdateToastDisplay();
    int32 GetFreeToastSlot() const;
    uint32 m_nMaxVisible;  // 0x128
    float m_flDefaultToastDuration;  // 0x12c
    ToastId m_nToastIdGenerator;  // 0x130
    BOOL m_fKeepToastOnHover;  // 0x134
    BOOL m_fInDestructor;  // 0x138
    BOOL m_fIsEnabled;  // 0x13c
    blz::vector<float, blz::allocator<float> > m_toastTimeElapsed;  // 0x140
    XGrowableMap<UIID, const UIID &, int, int> m_toastVisualsToSlots;  // 0x158
};
static_assert(sizeof(ToastManagerBase) == 0x1d8, "ToastManagerBase");


// Functions defined in this file (bodies are not in the DWARF):
//   line    90  ToastManagerBase::Toast::Toast()  [inlined into 3 functions]
//   line    92  ToastManagerBase::Toast::~Toast()  [0x7100197520 (+1 more), inlined into 2 functions]
//   line   110  void ToastManagerBase::UpdateToastVisual(int32, ToastManagerBase::Toast *, float)  [0x7100197500]
//   line   113  void ToastManagerBase::OutroToastVisual(ToastManagerBase::Toast * ptToast, float)  [0x7100197510]
//   line   122  float ToastManagerBase::GetDefaultToastDuration() const  [inlined into 1 functions]
//   line   134  ToastId ToastManagerBase::GetNextToastId()  [inlined into 1 functions]
