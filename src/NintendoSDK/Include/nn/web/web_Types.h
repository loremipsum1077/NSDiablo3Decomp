// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/web/web_Types.h

#pragma once

namespace nn {
namespace web {
enum WebBackgroundKind {
    WebBackgroundKind_Normal = 0,
    WebBackgroundKind_ApplicationCapture = 1,
    WebBackgroundKind_ApplicationCaptureBlur = 2,
    WebBackgroundKind_Max = 3,
};

enum WebLeftStickMode {
    WebLeftStickMode_StickPointer = 0,
    WebLeftStickMode_SpatialNavi = 1,
    WebLeftStickMode_Max = 2,
};

enum WebBootDisplayKind {
    WebBootDisplayKind_Default = 0,
    WebBootDisplayKind_White = 1,
    WebBootDisplayKind_Black = 2,
    WebBootDisplayKind_CallerCapture = 3,
    WebBootDisplayKind_CallerCaptureBlur = 4,
    WebBootDisplayKind_Max = 5,
};

enum WebFooterFixedKind {
    WebFooterFixedKind_Auto = 0,
    WebFooterFixedKind_Shown = 1,
    WebFooterFixedKind_Max = 2,
};

enum WebDisplayUrlKind {
    WebDisplayUrlKind_FullPath = 0,
    WebDisplayUrlKind_Domain = 1,
};

enum WebExitReason {
    WebExitReason_EndButtonPressed = 0,
    WebExitReason_BackButtonPressed = 1,
    WebExitReason_ExitMessage = 2,
    WebExitReason_CallbackUrlReached = 3,
    WebExitReason_LastWindowDeleted = 4,
    WebExitReason_LocalNetworkDisconnectionDenied = 5,
    WebExitReason_MediaPlayerClosed = 6,
    WebExitReason_CausedByWebPage = 7,
    WebExitReason_UserSelectionCanceled = 8,
    WebExitReason_NetworkConnectionFailed = 9,
    WebExitReason_SystemUpdateRequired = 10,
    WebExitReason_Unexpected = 20,
};

}  // namespace web
}  // namespace nn
