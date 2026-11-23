// Original file: X/Client/Console/ControllerUtils.cpp

#include "ControllerUtils.h"
#include "LocalPlayerClamping.h"
#include "../Input/Input.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"

namespace {

struct ControllerData {  // size 0x20
    Vector2D tRStickWorldDir;  // 0x00
    Vector2D tLStickWorldDir;  // 0x08
    IVector2D tRStickScreenOffset;  // 0x10
    IVector2D tLStickScreenOffset;  // 0x18
};

struct ControllerInputGlobals {  // size 0x80
    ControllerData arControllers[4];  // 0x00
};

static ControllerInputGlobals sgtControllerGlobals;

}  // namespace

namespace Console {
namespace ControllerUtils {

BOOL IsButtonDown(int32 nControllerIndex, InputButton eButton)
{
    return InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[nControllerIndex].tCurrentFrame.IsButtonDown(eButton);
}

BOOL IsButtonRepeated(int32 nControllerIndex, InputButton eButton)
{
    return (Blizzard::Time::Milliseconds() - InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[nControllerIndex].tButtonLastDownTimes[eButton]) < 300;
}

Blizzard::Time::Millisecond GetHeldButtonTime(int32 nControllerIndex, InputButton eButton)
{
    return Blizzard::Time::Milliseconds() - InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[nControllerIndex].tButtonHeldDownTimes[eButton];
}

float GetLTriggerPosition(int32 nControllerIndex)
{
    return (float)InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[nControllerIndex].tCurrentFrame.nLeftTrigger / 255.0f;
}

Vector2D GetRawLStickPosition(int32 nControllerIndex, BOOL bClamped)
{
    if (bClamped)
    {
        return LocalPlayerClamping::GetLStickPosition(nControllerIndex);
    }
    else
    {
        const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
        return tInputState.tControllerState[nControllerIndex].tCurrentFrame.vecLeftStick;
    }
}

BOOL LStickIsInDeadZone(int32 nControllerIndex, BOOL bClamped)
{
    ScopedGlobalDefinitionLockR pdefGlobal;
    return LStickIsInDeadZone(nControllerIndex, bClamped, pdefGlobal->flControllerDeadZone, pdefGlobal->flControllerReboundDeadZone);
}

BOOL LStickIsInDeadZone(int32 nControllerIndex, BOOL bClamped, const float flDeadZone, const float flReboundDeadZone)
{
    Vector2D vLStick = GetRawLStickPosition(nControllerIndex, bClamped);

    if (IsStickRebounding(nControllerIndex, FALSE))
    {
        float flReboundDeadZoneToUse = Math::Max(flDeadZone, flReboundDeadZone);
        if (vLStick.MagnitudeSquared() > flReboundDeadZoneToUse * flReboundDeadZoneToUse)
        {
            return FALSE;
        }
    }
    else if (vLStick.MagnitudeSquared() > flDeadZone * flDeadZone)
    {
        return FALSE;
    }

    return TRUE;
}

Vector2D GetRawRStickPosition(int32 nControllerIndex)
{
    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
    return tInputState.tControllerState[nControllerIndex].tCurrentFrame.vecRightStick;
}

BOOL RStickIsInDeadZone(int32 nControllerIndex)
{
    ScopedGlobalDefinitionLockR pdefGlobal;
    return RStickIsInDeadZone(nControllerIndex, pdefGlobal->flControllerDeadZone, pdefGlobal->flControllerReboundDeadZone);
}

BOOL RStickIsInDeadZone(int32 nControllerIndex, float flDeadZone, float flReboundDeadZone)
{
    Vector2D vRStick = GetRawRStickPosition(nControllerIndex);

    if (IsStickRebounding(nControllerIndex, TRUE))
    {
        float flReboundDeadZoneToUse = Math::Max(flDeadZone, flReboundDeadZone);
        if (vRStick.MagnitudeSquared() > flReboundDeadZoneToUse * flReboundDeadZoneToUse)
        {
            return FALSE;
        }
    }
    else if (vRStick.MagnitudeSquared() > flDeadZone * flDeadZone)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL IsStickRebounding(int32 nControllerIndex, BOOL bRightStick)
{
    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
    Blizzard::Time::Millisecond tLastExtremeTime = bRightStick ? tInputState.tControllerState[nControllerIndex].tRStickLastExtremeTime : tInputState.tControllerState[nControllerIndex].tLStickLastExtremeTime;

    ScopedGlobalDefinitionLockR pdefGlobal;
    return (Blizzard::Time::Milliseconds() - tLastExtremeTime) < pdefGlobal->nControllerReboundDuration;
}

Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bClamped)
{
    if (LStickIsInDeadZone(nControllerIndex, bClamped))
    {
        return Vector2D(0.0f, 0.0f);
    }

    return GetRawLStickPosition(nControllerIndex, bClamped);
}

Vector2D GetRStickPosition(int32 nControllerIndex)
{
    if (RStickIsInDeadZone(nControllerIndex))
    {
        return Vector2D(0.0f, 0.0f);
    }

    return GetRawRStickPosition(nControllerIndex);
}

static const Vector2D & sGetLastFrameLStickPos(int32 nControllerIndex)
{
    return InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[nControllerIndex].tPrevFrame.vecLeftStick;
}

static const Vector2D & sGetLastFrameRStickPos(int32 nControllerIndex)
{
    return InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[nControllerIndex].tPrevFrame.vecRightStick;
}

BOOL DidStickCrossThreshold(int32 nControllerIndex, BOOL bRightStick, float flThreshold)
{
    Vector2D vLastPos = bRightStick ? sGetLastFrameRStickPos(nControllerIndex) : sGetLastFrameLStickPos(nControllerIndex);
    Vector2D vCurPos = bRightStick ? GetRStickPosition(nControllerIndex) : GetLStickPosition(nControllerIndex, FALSE);

    return (vCurPos.MagnitudeSquared() > flThreshold * flThreshold) != (vLastPos.MagnitudeSquared() > flThreshold * flThreshold);
}

void SetLStickWorldDirection(int32 nControllerIndex, float x, float y)
{
    sgtControllerGlobals.arControllers[nControllerIndex].tLStickWorldDir.Set(x, y);
    sgtControllerGlobals.arControllers[nControllerIndex].tLStickWorldDir.Normalize();
}

void SetRStickWorldDirection(int32 nControllerIndex, float x, float y)
{
    sgtControllerGlobals.arControllers[nControllerIndex].tRStickWorldDir.Set(x, y);
    sgtControllerGlobals.arControllers[nControllerIndex].tRStickWorldDir.Normalize();
}

const Vector2D & GetLStickWorldDirection(int32 nControllerIndex)
{
    return sgtControllerGlobals.arControllers[nControllerIndex].tLStickWorldDir;
}

const Vector2D & GetRStickWorldDirection(int32 nControllerIndex)
{
    return sgtControllerGlobals.arControllers[nControllerIndex].tRStickWorldDir;
}

void SetLStickScreenOffset(int32 nControllerIndex, int32 x, int32 y)
{
    sgtControllerGlobals.arControllers[nControllerIndex].tLStickScreenOffset.Set(x, y);
}

void SetRStickScreenOffset(int32 nControllerIndex, int32 x, int32 y)
{
    sgtControllerGlobals.arControllers[nControllerIndex].tRStickScreenOffset.Set(x, y);
}

const IVector2D & GetRStickScreenOffset(int32 nControllerIndex)
{
    return sgtControllerGlobals.arControllers[nControllerIndex].tRStickScreenOffset;
}

const IVector2D & GetLStickScreenOffset(int32 nControllerIndex)
{
    return sgtControllerGlobals.arControllers[nControllerIndex].tLStickScreenOffset;
}

}  // namespace ControllerUtils
}  // namespace Console
