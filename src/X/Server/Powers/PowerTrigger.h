// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Powers/PowerTrigger.h
// Subset.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Powers/PowersStruct.h"

void AnimationPower_CloseOnTarget(const PowerID idPower);  // PowerTrigger.cpp:755
BOOL SpecialWalkPower_Start(const PowerID idPower, void * pData);  // PowerTrigger.cpp:1388
BOOL SpecialWalkPower_End(const PowerID idPower, void *);  // PowerTrigger.cpp:1497
void LoopingAnimationPower_StartTimer(const PowerID idPower, float flSeconds, BOOL bShowProgressBar, BOOL bInterruptible);  // PowerTrigger.cpp:1687
void LoopingAnimationPower_RemoveBuff(const PowerID idPower);  // PowerTrigger.cpp:1772

typedef int32 TimerID;  // X/Server/Timers.h
TimerID PowerTriggerAddTimerTrigger(PowerID idPower, uint32 dwDuration, PowerTriggerCallback pCallbackFunction, void * pCallbackParameter);  // PowerTrigger.cpp:684
struct Power;
typedef int32 PowerMessage;  // X/Server/Powers/Powers.h
void AnimationPower(Power * ptPower, PowerMessage eMessage, void * pParam);  // PowerTrigger.cpp
void NonAnimationPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam);  // PowerTrigger.cpp
void SpecialWalkPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam);  // PowerTrigger.cpp
void LoopingAnimationPower(Power * ptPower, PowerMessage eMessage, void * pParam);  // PowerTrigger.cpp
struct PreplayedAnimData;
void PowerTriggerPlayTriggerAnimation(PowerID idPower, uint32 dwAnimationTag, uint32 dwWantedDuration, float flSpeedScalar, BOOL bSetAimTarget, const PreplayedAnimData & tPreplayedAnimData, int32 eAnimReason, int32 eContactFrameType, BOOL bVerifyContactTimings);  // PowerTrigger.cpp:300
