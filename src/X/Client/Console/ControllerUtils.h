// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/ControllerUtils.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/Math/Vector.h"

typedef int32 InputButton;

namespace Console {
namespace ControllerUtils {

BOOL IsButtonDown(int32 nControllerIndex, InputButton eButton);
BOOL IsButtonRepeated(int32 nControllerIndex, InputButton eButton);
Blizzard::Time::Millisecond GetHeldButtonTime(int32 nControllerIndex, InputButton eButton);
float GetLTriggerPosition(int32 nControllerIndex);
Vector2D GetRawLStickPosition(int32 nControllerIndex, BOOL bClamped);
BOOL LStickIsInDeadZone(int32 nControllerIndex, BOOL bClamped);
BOOL LStickIsInDeadZone(int32 nControllerIndex, BOOL bClamped, const float flDeadZone, const float flReboundDeadZone);
Vector2D GetRawRStickPosition(int32 nControllerIndex);
BOOL RStickIsInDeadZone(int32 nControllerIndex);
BOOL RStickIsInDeadZone(int32 nControllerIndex, float flDeadZone, float flReboundDeadZone);
BOOL IsStickRebounding(int32 nControllerIndex, BOOL bRightStick);
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bClamped);
Vector2D GetRStickPosition(int32 nControllerIndex);
BOOL DidStickCrossThreshold(int32 nControllerIndex, BOOL bRightStick, float flThreshold);
void SetLStickWorldDirection(int32 nControllerIndex, float x, float y);
void SetRStickWorldDirection(int32 nControllerIndex, float x, float y);
const Vector2D & GetLStickWorldDirection(int32 nControllerIndex);
const Vector2D & GetRStickWorldDirection(int32 nControllerIndex);
void SetLStickScreenOffset(int32 nControllerIndex, int32 x, int32 y);
void SetRStickScreenOffset(int32 nControllerIndex, int32 x, int32 y);
const IVector2D & GetRStickScreenOffset(int32 nControllerIndex);
const IVector2D & GetLStickScreenOffset(int32 nControllerIndex);

}  // namespace ControllerUtils
}  // namespace Console
