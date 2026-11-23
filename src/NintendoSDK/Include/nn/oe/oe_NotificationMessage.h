// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/oe/oe_NotificationMessage.h

#pragma once

#include "../nn_BitTypes.h"

namespace nn {
namespace oe {
typedef nn::Bit32 Message;

enum FocusState {
    FocusState_InFocus = 1,
    FocusState_OutOfFocus = 2,
    FocusState_Background = 3,
};

enum FocusHandlingMode {
    FocusHandlingMode_Suspend = 0,
    FocusHandlingMode_Notify = 1,
    FocusHandlingMode_SuspendAndNotify = 2,
    FocusHandlingMode_InFocusOnly = 3,
};

// The message values and the functions below are not in the DWARF; values are taken from
// the binary's compares.
const Message MessageExitRequest = 4;
const Message MessageFocusStateChanged = 15;
const Message MessageResume = 16;
const Message MessageOperationModeChanged = 30;
const Message MessagePerformanceModeChanged = 31;

bool TryPopNotificationMessage(Message * pOutMessage);
FocusState GetCurrentFocusState();
void SetFocusHandlingMode(FocusHandlingMode eMode);
void SetResumeNotificationEnabled(bool bEnabled);
void SetOperationModeChangedNotificationEnabled(bool bEnabled);
void SetPerformanceModeChangedNotificationEnabled(bool bEnabled);
void EnterExitRequestHandlingSection();
void LeaveExitRequestHandlingSection();

}  // namespace oe
}  // namespace nn
