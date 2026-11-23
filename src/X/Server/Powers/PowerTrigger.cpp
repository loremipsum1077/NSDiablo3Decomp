// Original file: X/Server/Powers/PowerTrigger.cpp

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "PowerTrigger.h"
#include "PowerAPI.h"
#include "Powers.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Powers/SpellEffectCommon.h"
#include "../../Common/Target.h"
#include "../SGame.h"
#include "../Actors/Actors.h"
#include "../Movement/SActorMovement.h"
#include "../PlayerList.h"
#include "../Timers.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../Actors/ActorEffect.h"
#include "PowerHelper.h"
#include "SpellEffect.h"
#include "../AI/AIStates.h"
#include "../AI/ActorAI.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/GameConstants.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Actors/ActorPowers.h"
#include "../../Common/Player.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Actors/ActorEffectCommon.h"
uint32 TimerGetTime(const TimerID idTimer);  // X/Server/Timers.cpp:458
void PlayerBrainTryQueuedPower(ACDID idACDPlayer);  // X/Server/AI/PlayerBrain.cpp:407
BOOL PowerIsScripted(AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp:1768
void PowerTargetCriteriaInit(PowerID idPower);  // Powers.cpp:3878
SNO PowerDetermineContactFrameEffect(PowerID idPower, ActorCommonData * ptACD);  // Powers.cpp:2489
void PowerUpdateFacing(const PowerID idPower, void * pData);  // Powers.cpp:3511
uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim);  // X/Common/Powers/PowersCommon.cpp:8013
uint32 AnimPlayerGetWalkCoolDownTag(ActorCommonData & tACD, uint32 dwAnimTag);  // X/Common/AnimPlayer.cpp
void SpellEffectRun(PowerID idPower, SpellEffect eSpellEffect, void * pData);  // SpellEffect.cpp:19358
void SpellEffectRunNextEffect(PowerID idPower, void * pData);  // SpellEffect.cpp:19380
const int32 MAX_PREPLAY_BACKDATING_TICKS = 30;
void PowerTriggerUpdateTimersAddDelay(const PowerID idPower, float flDelay);  // DWARF PowerTrigger.cpp:1663 (external, linker-removed)
static void sAnimationPower_Contact(const PowerID idPower, void * pData);

// X/Common/Networking/GameMessage.h (not in our subset yet)
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
// DWARF: X/Server/LuaScript/LuaScript.cpp:642.
BOOL IsTriggerEventTargetingActor(ActorID idActor);
void SpellEffectRunEndEffect(PowerID idPower, void * pData);  // SpellEffect.cpp:19413
// DWARF: AnimPlayer.cpp:731 and Powers.cpp:5481; declarations only.
void AnimPlayerGetScaledAnimationTimingForMovingPower(SNO, ActorCommonData &, uint32, uint32, WorldPlace, float, RNG &, AnimationTiming &);
BOOL PowerComputeDestination(SNO, const ActorMovement &, const WorldPlace &, WorldPlace &, BOOL, float);
#include "../../Common/TagMaps/AnimTags.h"
void SpellEffectIncrement(PowerID idPower);  // DWARF: X/Server/Powers/SpellEffect.cpp:19441
// MAX_NUM_CONTACT_FRAMES is declared in Common/Powers/PowersStruct.h (DWARF PowersStruct.h:24).
static void sPowerOnContact(TimerID idTimer, DataID idOwner, void * lpData)



{
    PowerID idPower = idOwner;
    uint32 nContactFrame = (uint32)(uintptr_t)lpData;
    Power * ptPower = PowerGet(idPower);

    if (nContactFrame == 0)
    {
        ptPower->idTimerContact = TIMERID_INVALID;
    }
    else
    {
        POS posTimer = ptPower->listTimers.GetHeadPosition();
        while (posTimer)
        {
            POS posCurrent = posTimer;
            const PowerTimer & tPowerTimer = ptPower->listTimers.GetNext(posTimer);
            if (tPowerTimer.idTimer == idTimer)
            {
                ptPower->listTimers.RemoveAt(posCurrent);
                break;
            }
        }
    }

    ptPower->dwCurrentContactFrame = nContactFrame;

    const PowerDefinition * pdefPower = (const PowerDefinition *)gptGroupDefPower->LockR(ptPower->snoPower, FALSE);
    BOOL bContactFrameEnabled = PowerIsContactFrameEnabled(pdefPower, nContactFrame);
    gptGroupDefPower->UnlockR((void *&)pdefPower);

    if ((ptPower->dwFlags & (1 << PF_INTERRUPTED)) == 0 && bContactFrameEnabled)
    {
        if (!PowerIsScripted(AutoPowerDefinitionR(ptPower->snoPower)))
            PowerTargetCriteriaInit(idPower);

        ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
        SNO snoContactFrameEffect = PowerDetermineContactFrameEffect(idPower, ptACD);
        if (snoContactFrameEffect != SNO_NONE)
            ACDPlayEffect(ptPower->idACD, EFFECT_PLAY_EFFECT_GROUP, snoContactFrameEffect, ptPower->idACD);

        PowerUpdateFacing(idPower, NULL);
        SpecificPowerSendMessage(idPower, POWER_CONTACT, (void *)-1);
    }

    PowerCheckForDone(idPower);
    ptPower->dwLastContactTick = GameGetServerTime();
}

// Declarations for sSpecialWalk_* below (w87). PowerAPIChooseOpenGround belongs in X/Server/Powers/PowerAPI.h:366
// (DWARF); declared locally like PowerHelper.cpp/SpellEffect.cpp do, since PowerAPI.h does not include NavigationMesh.h.
#include "../../Common/Nav/NavigationMesh.h"
struct PowerAPIChooseOpenGround {  // size 0x20
    NavPlace tNavPlace;  // 0x00
    float flSkipDistance;  // 0x18
    BOOL bUseActorPassability;  // 0x1c
};
// DWARF X/Server/AI/BrainStance.h:73 (same local declaration as BrainStance.cpp).
struct KnockbackPowerData {
    float flHeight;
    float flGravity;
};
BOOL PowerAPI_DoChooseOpenGround(ActorCommonData & tACD, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & placeOpenSpot, NavMeshHandle & tNavMeshHandleResult);  // X/Server/Powers/PowerAPI.cpp:6235
void PowerHelper_ChooseOpenGroundInit(PowerID idPower, const NavPlace & tNavPlace, PowerAPIChooseOpenGround & tOpenGroundInfo, float flSkipDistance);  // X/Server/Powers/PowerHelper.cpp:833
BOOL PowerShouldRollToDestination(AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp:759
void PowerAddTargettedACD(PowerID idPower, ACDID idACD);  // X/Server/Powers/Powers.cpp:4899














static void sPowerTriggerRegisterContact(
    PowerID idPower,
    uint32 dwDuration,
    uint32 nContactFrame)
{
    Power * ptPower = PowerGet(idPower);


    // Inference: frame zero replaces the primary timer; other indices are pointer-sized callback payloads.
    if (nContactFrame == 0)
    {
        if (TimerExists(ptPower->idTimerContact))
        {
            TimerUnregister(ptPower->idTimerContact);
        }

        ptPower->idTimerContact = TimerRegister(ptPower->id, TIMER_OWNER_POWER, dwDuration, NULL, sPowerOnContact);
    }
    else
    {
        PowerTimer tPowerTimer;
        tPowerTimer.idTimer = TimerRegister(ptPower->id, TIMER_OWNER_POWER, dwDuration, (void *)(uintptr_t)nContactFrame, sPowerOnContact);
        tPowerTimer.pCallbackParameter = (void *)(uintptr_t)nContactFrame;
        ptPower->listTimers.AddTail_Tracked(tPowerTimer);
    }
}




static void sPowerOnInterrupt(
    TimerID idTimer,
    DataID idOwner,
    void * lpData)
{


    // Inference: missing and manually finished powers return before interruptability/cancel processing.
    PowerID idPower = idOwner;
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;


    ptPower->idTimerInterrupt = TIMERID_INVALID;

    if (ptPower->dwFlags & (1 << PF_MANUALLY_FINISH))
        return;



    ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);

    if (ptPower->dwFlags & (1 << PF_CANCEL))
    {
        PowerDetachFromActor(idPower, FALSE);
    }








    PowerCheckForDone(idPower);



    PlayerBrainTryQueuedPower(ptPower->idACD);
}

static void sPowerTriggerRegisterInterrupt(
    PowerID idPower,
    uint32 dwDuration)
{
    Power * ptPower = PowerGet(idPower);
    // Inference: the DWARF ActorID local converts ACDGetOwner's DataID result.
    ActorID idActor = (ActorID)ACDGetOwner(ptPower->idACD);
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor && ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->AddTickDebt(dwDuration);
    }

    if (TimerExists(ptPower->idTimerInterrupt))
    {
        TimerUnregister(ptPower->idTimerInterrupt);
    } // Inference: original ldrsw passes the signed power SNO as a pointer-sized callback payload.
    ptPower->idTimerInterrupt = TimerRegister(ptPower->id, TIMER_OWNER_POWER, dwDuration, (void *)(intptr_t)ptPower->snoPower, sPowerOnInterrupt);
}




static void sPowerOnActorDone(
    TimerID idTimer,
    DataID idOwner,
    void * lpData)
{

    // Inference: the original callback clears its timer, detaches, sends POWER_END with sentinel -1, then checks completion.
    PowerID idPower = idOwner;
    Power * ptPower = PowerGet(idPower);
    ptPower->idTimerActorDone = TIMERID_INVALID;

    PowerDetachFromActor(idPower, FALSE);
    SpecificPowerSendMessage(idPower, POWER_END, (void *)-1);

    PowerCheckForDone(idPower);



    PlayerBrainTryQueuedPower(ptPower->idACD);
}
// DWARF inline helper; body not independently checkable (no standalone binary function). Inference: PF_MANUALLY_FINISH skips registration.
static void sPowerTriggerRegisterActorDone(
    PowerID idPower,
    uint32 dwDuration)
{
    Power * ptPower = PowerGet(idPower);

    if (ptPower->dwFlags & (1 << PF_MANUALLY_FINISH))
    {
        return;
    }

    if (TimerExists(ptPower->idTimerActorDone))
    {
        TimerUnregister(ptPower->idTimerActorDone);
    }
    ptPower->idTimerActorDone = TimerRegister(ptPower->id, TIMER_OWNER_POWER, dwDuration, NULL, sPowerOnActorDone);
}





void sServerMessageSetAimTarget(const ACDID idACD, const Target & tAimTarget)
{
    AimTargetMessage tMessage;
    tMessage.ann = ANNFromACD(idACD);
    tMessage.eType = tAimTarget.eType;
    tMessage.annTarget = ANNFromACD(tAimTarget.GetACD());
    tMessage.tPlace = tAimTarget.tPlace;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, idACD, FALSE);
    MessageSendToClient(listDestPlayers, SMSG_AIM_TARGET, &tMessage, sizeof(tMessage));
}







void PowerTriggerPlayTriggerAnimation(
    PowerID idPower,
    uint32 dwAnimationTag,
    uint32 dwWantedDuration,
    float flSpeedScalar,
    BOOL bSetAimTarget,
    const PreplayedAnimData & tPreplayedAnimData,
    AnimReason eAnimReason,
    ContactFrameType eContactFrameType,
    BOOL bVerifyContactTimings)
{
    Power * ptPower = PowerGet(idPower);
    // Inference: DWARF ActorID local uses the canonical DataID owner result with an enum conversion.
    ACDID idACDMainActor = ptPower->idACD;
    ActorCommonData & tACDMainActor = ACDGet(idACDMainActor);
    ActorID idActor = (ActorID)ACDGetOwner(idACDMainActor);
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
        return;

    // Inference: the original uses the synced RNG only for PREPLAY_ANIM_FLAGS_PREPLAYED_ANIM (bit 0).

    Player * ptPlayer = PlayerGetByACD(idACDMainActor);

    RNG * prng;
    if (tPreplayedAnimData.dwPrePlayedAnimFlags & (1 << 0))
    {



        RandSetSeed_Internal(SGameGlobalsGet().rngSyncedAnim, tPreplayedAnimData.dwSyncedSeed);

        prng = &SGameGlobalsGet().rngSyncedAnim;
    }
    else
    {
        prng = &SGameGetRNG();
    }


    // Inference: ordinary timing is the negative branch; scaled timing computes and faces its destination.
    AnimationTiming tAnimationTiming;
    if (!PowerHasScaledAnimationTiming(ptPower->snoPower))
    {
        AnimPlayerGetAnimationTiming(tACDMainActor, dwAnimationTag, dwWantedDuration, flSpeedScalar, *prng, tAnimationTiming);
    }
    else
    {





        WorldPlace wpDestination;
        ptPower->tTarget.GetPlace(wpDestination);

        if (ptActor->ptSActorMovement)
        {
            if (PowerGetChargeSteeringType(ptPower->snoPower) == CHARGE_STEERING_NONE)
            {
                PowerComputeDestination(ptPower->snoPower, *ptActor->ptSActorMovement, wpDestination, wpDestination, TRUE, 10.0f);
            }

            ptActor->ptSActorMovement->FaceTarget(wpDestination.wp, FALSE);
        }

        AnimPlayerGetScaledAnimationTimingForMovingPower(
            ptPower->snoPower,
            tACDMainActor, dwAnimationTag, dwWantedDuration,
            wpDestination, flSpeedScalar, *prng,
            tAnimationTiming);
    }






    // Inference: contact counters and the last accepted contact start at zero; four is MAX_NUM_CONTACT_FRAMES.
    uint32 dwStartDuration = tAnimationTiming.tAnimSpec[ANIMOFFSET_START].dwDuration;
    uint32 dwEndDuration = tAnimationTiming.tAnimSpec[ANIMOFFSET_END].dwDuration;
    uint32 dwExecuteTime[4];

    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
    int32 nContactCountStart = 0;
    int32 nContactCountNormal = 0;
    int32 nContactCountEnd = 0;
    uint32 dwLastContactTime = 0;
    BOOL bContactFrameEnabled[4];

    ptPower->nContactAnimationPermutation[ANIMOFFSET_CONTINUE] = tAnimationTiming.tAnimSpec[ANIMOFFSET_CONTINUE].nPermutationIndex;
    ptPower->snoContactAnimation[ANIMOFFSET_CONTINUE] = tAnimationTiming.tAnimSpec[ANIMOFFSET_CONTINUE].snoAnim;
    ptPower->nContactAnimationPermutation[ANIMOFFSET_START] = tAnimationTiming.tAnimSpec[ANIMOFFSET_START].nPermutationIndex;
    ptPower->snoContactAnimation[ANIMOFFSET_START] = tAnimationTiming.tAnimSpec[ANIMOFFSET_START].snoAnim;
    ptPower->nContactAnimationPermutation[ANIMOFFSET_END] = tAnimationTiming.tAnimSpec[ANIMOFFSET_END].nPermutationIndex;
    ptPower->snoContactAnimation[ANIMOFFSET_END] = tAnimationTiming.tAnimSpec[ANIMOFFSET_END].snoAnim;

    for (int32 i = 0; i < MAX_NUM_CONTACT_FRAMES; i++)
    {
        bContactFrameEnabled[i] = PowerIsContactFrameEnabled(pdefPower, i);
        if (eContactFrameType == CONTACT_FRAME_NONE)
        {
            bContactFrameEnabled[i] = FALSE;
            continue;
        }
        // Inference: the original switch handles start/end contacts, with all other types using normal timing.
        switch (PowerGetContactFrameType(ptPower->snoPower, i))
        {
        case CONTACT_FRAME_IN_START:
            // Inference: normal playback consumes start timing only when a start animation exists.
            if (eContactFrameType == CONTACT_FRAME_NORMAL)
            {
                if (dwStartDuration == 0)
                {
                    dwExecuteTime[i] = 0;
                }
                else
                {




                    dwExecuteTime[i] = tAnimationTiming.dwStartAnimExecuteTime[nContactCountStart++];
                }
            }
            else if (eContactFrameType == CONTACT_FRAME_IN_START)
            {
                dwExecuteTime[i] = tAnimationTiming.dwExecuteTime[nContactCountNormal++];
            }
            else
            {
                dwExecuteTime[i] = 0;
                bContactFrameEnabled[i] = FALSE;
            }
            break;

        case CONTACT_FRAME_IN_END:
            // Inference: end contacts include both preceding animation durations during normal playback.


            if (eContactFrameType == CONTACT_FRAME_NORMAL)
            {
                if (dwEndDuration == 0)
                {
                    dwExecuteTime[i] = tAnimationTiming.tAnimSpec[ANIMOFFSET_CONTINUE].dwDuration + dwStartDuration;
                }
                else
                {




                    dwExecuteTime[i] = tAnimationTiming.tAnimSpec[ANIMOFFSET_CONTINUE].dwDuration + dwStartDuration + tAnimationTiming.dwEndAnimExecuteTime[nContactCountEnd++];
                }
            }
            else if (eContactFrameType == CONTACT_FRAME_IN_END)
            {
                dwExecuteTime[i] = tAnimationTiming.dwExecuteTime[nContactCountNormal++];
            }
            else
            {
                dwExecuteTime[i] = 0;
                bContactFrameEnabled[i] = FALSE;
            }
            break;

        default:




            dwExecuteTime[i] = dwStartDuration + tAnimationTiming.dwExecuteTime[nContactCountNormal++];
            break;
        }

        // Inference: verify mode retains the last time on decreasing contacts; combo powers stop after the first contact.
        if (bContactFrameEnabled[i])
        {
            if ((dwExecuteTime[i] >= dwLastContactTime) || !bVerifyContactTimings)
                dwLastContactTime = dwExecuteTime[i];
        }









        if (PowerIsComboPower(AutoPowerDefinitionR(ptPower->snoPower)))
            break;
    }


    // Inference: the interrupt cannot precede the last enabled contact; cycle time includes start/end durations.
    uint32 dwNow = GameGetServerTime();
    uint32 dwStartTime = dwNow;
    uint32 dwInterruptTime = Math::Max(tAnimationTiming.dwInterruptTime + dwStartDuration, dwLastContactTime);
    uint32 dwCycleTime = tAnimationTiming.dwCycleTime + dwStartDuration + dwEndDuration;

    if (PowerIsChannelled(tACDMainActor, ptPower->snoPower))
    {
        // Inference: DWARF POWER_MIN_CHANNELING_TICKS (10) minus POWER_MIN_CHANNELING_TICKS_SLOP (2).
        dwInterruptTime = 10 - 2;
        dwCycleTime = 10 - 2;
    }



    PowerTemplate eTemplate = PowerGetTemplate(ptPower->snoPower, ptPower->idACD);
    if (eTemplate == LOOPING_ANIMATION_POWER || eTemplate == GIZMO_POWER)
    {
        dwInterruptTime = 0;
        dwCycleTime = 0;
    }
    // Inference: PREPLAY_ANIM_FLAGS_PREPLAYED_ANIM is bit 0; backdating honors the player's prior cast times.
    if (tPreplayedAnimData.dwPrePlayedAnimFlags & (1 << 0))
    {











        uint32 dwServerTimeAnimStarted = tPreplayedAnimData.dwServerTimeAnimStarted;
        if (ptPlayer)
        {
            uint32 dwEarliestCastAllowed;
            if (PowerCancelsOtherPowers(ptPower->snoPower))
            {
                if (PowerCancelsOnInterruptFrame(ptPower->snoPower))
                {
                    dwEarliestCastAllowed = ptPlayer->tQueuedPower.dwPowerLastInterruptTime;
                }
                else
                {


                    dwEarliestCastAllowed = ptPlayer->tQueuedPower.dwPowerLastStartTime;
                }
            }
            else
            {
                dwEarliestCastAllowed = ptPlayer->tQueuedPower.dwPowerLastEndTime;
            }
            dwServerTimeAnimStarted = Math::Max(dwServerTimeAnimStarted, dwEarliestCastAllowed);
        }
        // Inference: signed differences are clamped to [-30, 0], then each timer is clamped nonnegative.
        int32 nTimeOffset = Math::Pin((int32)(dwServerTimeAnimStarted - dwNow), -MAX_PREPLAY_BACKDATING_TICKS, 0);
        for (int32 i = 0; i < 4; ++i)
        {
            dwExecuteTime[i] = Math::Max(0, (int32)dwExecuteTime[i] + nTimeOffset);
        }
        dwStartTime = dwNow + nTimeOffset;
        dwInterruptTime = Math::Max(0, (int32)dwInterruptTime + nTimeOffset);
        dwCycleTime = Math::Max(0, (int32)dwCycleTime + nTimeOffset);
    }



    if (ptPlayer)
    {
        ptPlayer->tQueuedPower.dwPowerLastStartTime = dwStartTime;
        ptPlayer->tQueuedPower.dwPowerLastInterruptTime = dwInterruptTime + dwNow;
        ptPlayer->tQueuedPower.dwPowerLastEndTime = dwCycleTime + dwNow;
    }
    // Inference: non-combo powers register enabled frames; combo powers reuse their current contact index.
    if (!PowerIsComboPower(AutoPowerDefinitionR(ptPower->snoPower)))
    {
        for (int32 i = 0; i < 4; ++i)
        {
            if (bContactFrameEnabled[i])
            {
                sPowerTriggerRegisterContact(idPower, dwExecuteTime[i], i);
            }
        }
    }
    else
    {


        sPowerTriggerRegisterContact(idPower, dwExecuteTime[0], ptPower->dwCurrentContactFrame);
    }


    ptPower->dwFirstContactTick = dwNow + dwExecuteTime[0];
    // Inference: infinite animation and no-interrupt powers omit only the interrupt timer.
    if (dwWantedDuration != ANIM_PLAY_FOREVER &&
        !PowerNoInterruptTimer(ptPower->snoPower))
    {
        sPowerTriggerRegisterInterrupt(idPower, dwInterruptTime);
    }
    sPowerTriggerRegisterActorDone(idPower, dwCycleTime);

    if (bSetAimTarget && !ptPower->tTarget.IsNone())
    {
        sServerMessageSetAimTarget(tACDMainActor.id, ptPower->tTarget);
    }

    BOOL bTranslate = PowerIsTranslate(ptPower->snoPower);
    BOOL bPreplayed = (tPreplayedAnimData.dwPrePlayedAnimFlags & (1 << 0)) != 0;

    if (ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->StopAndSyncPosition(FALSE, bPreplayed);
    }
    // Inference: PREPLAY_ANIM_FLAGS_PREPLAYED_BY_ALL_PLAYERS (bit 2) suppresses sending.
    AnimPlayerPlayAnimationsWithTiming(
        tACDMainActor,
        &tAnimationTiming,
        1,
        bTranslate,
        eAnimReason,
        bPreplayed,
        (tPreplayedAnimData.dwPrePlayedAnimFlags & (1 << 2)) != 0);
}




void sPowerTriggerCallback(TimerID idTimer, DataID idOwner, void * pCallbackFunction)
{
    PowerID idPower = idOwner;
    Power * ptPower = PowerGet(idPower);
    void * pParam = NULL;

    POS posTimer = ptPower->listTimers.GetHeadPosition();
    while (posTimer)
    {
        POS posCurrent = posTimer;
        const PowerTimer & tPowerTimer = ptPower->listTimers.GetNext(posTimer);
        if (tPowerTimer.idTimer == idTimer)
        {
            pParam = tPowerTimer.pCallbackParameter;
            ptPower->listTimers.RemoveAt(posCurrent);
            break;
        }
    }

    ((PowerTriggerCallback)pCallbackFunction)(idPower, pParam);
    PowerCheckForDone(idPower);
}




































TimerID PowerTriggerAddTimerTrigger(PowerID idPower, uint32 dwDuration, PowerTriggerCallback pCallbackFunction, void * pCallbackParameter)
{
    Power * ptPower = PowerGet(idPower);
    TimerID idTimer = TimerRegister(ptPower->id, TIMER_OWNER_POWER, dwDuration, (void *)pCallbackFunction, sPowerTriggerCallback);

    PowerTimer tPowerTimer;
    tPowerTimer.idTimer = idTimer;
    tPowerTimer.pCallbackParameter = pCallbackParameter;
    ptPower->listTimers.AddTail_Tracked(tPowerTimer);
    return idTimer;
}








void PowerTriggerRemoveTimerTrigger(PowerID idPower, TimerID idTimer)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        POS pos = ptPower->listTimers.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            PowerTimer & tPowerTimer = ptPower->listTimers.GetNext(pos);
            if (tPowerTimer.idTimer == idTimer)
            {
                ptPower->listTimers.RemoveAt(posLast);
                TimerUnregister(idTimer);
                return;
            }
        }
    }
}







static void sAnimationPower_PopulateAttributes(const PowerID idPower, AttribGroupID idFastAttrib)
{


    PowerHelper_PopulateCommonAttributes(idPower, idFastAttrib);
}



static void sAnimationPower_AttackPayloadInit(const PowerID idPower, PowerAPIAttackPayload & tPayload)
{


    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
}

static void sAnimationPower_UI_Damage_Numbers(const PowerID idPower, PowerAPIAttackPayload * pPayload)
{


    sAnimationPower_AttackPayloadInit(idPower, *pPayload);
}




void AnimationPower_CloseOnTarget(
    const PowerID idPower)
{




    // Inference: the original returns for absent/dead owners, then immobilized/rooted owners.
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }



    if (ptACD->IsImmobilized() || ptACD->IsRooted())
    {
        return;
    }



    ACDID idACDTarget = ptPower->tTarget.GetACD();
    float wdDistance;
    if (PowerIsCloseEnough(ptACD->id, ptPower->snoPower, &ptPower->tTarget, 0.0f, 0.000001f, &wdDistance, FALSE))
    {
        if (idACDTarget != ACDID_INVALID)
            return;
    }
    // Inference: the failed close-enough test shares the original nonpositive-distance exit.
    else
    {
        if (wdDistance <= 0.0f)
            return;
    }



    // Inference: the shuffle bonus is clamped to zero before scaling the walking speed.
    ScopedGlobalDefinitionLockR pdefGlobal;
    WorldDistance wdSpeed = ptACD->GetWalkingSpeed(TAG_ACTOR_ANIMATION_RUN);
    float flSpeedMultiplier = PowerGetFormulaValue(idPower, TAG_POWER_WALKING_SPEED_MULTIPLIER);
    wdSpeed *= flSpeedMultiplier * (1.0f + Math::Max(0.0f, pdefGlobal->flShuffleMovementSpeedBonus));


    int32 nTicksUntilContactFrame = ptPower->dwFirstContactTick - GameGetServerTime();
    WorldDistance wdMaxDistance = wdSpeed * nTicksUntilContactFrame;

    // Inference: actor targets cap travel; otherwise the minimum shuffle distance may raise speed.
    if (idACDTarget != ACDID_INVALID && wdMaxDistance > wdDistance)
    {
        wdMaxDistance = wdDistance;
    }
    else if (wdMaxDistance < pdefGlobal->flShuffleMinMoveDistance)
    {
        wdMaxDistance = pdefGlobal->flShuffleMinMoveDistance;

        if (idACDTarget != ACDID_INVALID)
        {
            wdMaxDistance = Math::Min(wdDistance, wdMaxDistance);
        }




        wdSpeed = wdMaxDistance / Math::Max(nTicksUntilContactFrame, 1);
    }
    // Inference: DWARF ActorID local converts the canonical DataID owner field.
    ActorID idActor = (ActorID)ptACD->idOwner;
    Actor * ptActor = ActorGet(idActor);


    WorldPlace wpTarget;
    if (ptPower->tTarget.GetPlace(wpTarget))
    {
        ptActor->ptSActorMovement->SnapPlaceToGround(wpTarget);

        Vector3D wvToTarget = wpTarget.wp - ptACD->GetPlace().wp;
        WorldDistance wdOriginalDistance = wvToTarget.Normalize();
        wvToTarget *= wdMaxDistance;
        wpTarget.wp = wvToTarget + ptACD->GetPlace().wp;


        // Inference: the original only retargets an overshoot when no actor is targeted.
        if (!ptPower->tTarget.IsTargetingACD() && wdMaxDistance > wdOriginalDistance)
        {

            ptPower->tTarget.InitializeFromPlace(wpTarget);
        }
    }

    // Inference: original mask 0x801a06 combines these six DWARF movement bits.
    uint32 uMoveFlags = (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING) |
        (1 << MOVEBIT_FROM_POWER) | (1 << MOVEBIT_DONT_AUTOPLAY_IDLE) |
        (1 << MOVEBIT_NO_ACCEL) | (1 << MOVEBIT_USE_CHARGE_STEERING);



    // Inference: players and trigger-targeted actors add MOVEBIT_ACTIVATE_TRIGGERS.
    if (ACDIsPlayer(ptACD->id) ||
        IsTriggerEventTargetingActor(idActor))
    {
        uMoveFlags |= (1 << MOVEBIT_ACTIVATE_TRIGGERS);
    }


    // Inference: retain the original half-tick bias before FloatRoundToInt and the contact-time lower bound.
    int32 nMaxTicksToWalk = Math::Max(nTicksUntilContactFrame, Math::FloatRoundToInt(wdMaxDistance / wdSpeed + 0.5f));
    // Inference: the original destination tolerance is speed * 0.2f; MoveTo uses no animation/power/callback.
    float flMaxRangeFromDest = wdSpeed * 0.2f;
    ptActor->ptSActorMovement->MoveTo(wpTarget, wdSpeed, 0.0f, TAG_ACTOR_ANIMATION_INVALID, uMoveFlags, flMaxRangeFromDest, idACDTarget, SNO_NONE, 0, nMaxTicksToWalk, 100.0f, NULL);
}





static void sAnimationPower_Start(const PowerID idPower)
{

    Power * ptPower = PowerGet(idPower);


    if (ptPower->bDoesntAnimate)
    {
        sAnimationPower_Contact(idPower, NULL);
        return;
    }



    PowerAPIAnimation tAnimation;
    PowerHelper_AnimationInit(idPower, tAnimation);
    tAnimation.tPreplayedAnimData = ptPower->tPreplayedAnimData;

    ActorCommonData & tACDMainActor = ACDGet(ptPower->idACD);
    BOOL bInfAnimTiming = PowerHasInfiniteAnimationTiming(tACDMainActor, AutoPowerDefinitionR(ptPower->snoPower));
    if (bInfAnimTiming || PowerIsChannelled(tACDMainActor, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        ptPower->dwFlags |= (1 << PF_MANUALLY_FINISH);
    }
    if (bInfAnimTiming)
    {
        tAnimation.dwDuration = (uint32)-2;
    }


    PowerAPI_DoAnimation(tAnimation);



    if (PowerCanMoveForwardBeforeContactFrame(AutoPowerDefinitionR(ptPower->snoPower)) &&
        (ptPower->dwFlags & (1 << PF_CONSOLE_MOVE_TOWARDS_TARGET)))
    {
        AnimationPower_CloseOnTarget(idPower);
    }
}



static void sPower_Create(const Power * ptPower, void * pData)
{




    ActorCommonData * ptACDowner = PowerGetOwner(ptPower);
    if (!ptACDowner)
        return;




    SpellEffect eCreateEffect = (SpellEffect)PowerGeneralTagGetInt(
        AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_SPELL_FUNC_CREATE);

    if (eCreateEffect != SPELL_INVALID && eCreateEffect != SPELL_NONE)
    {

        SpellEffectRun(ptPower->id, eCreateEffect, pData);
    }
}


static void sAnimationPower_Contact(const PowerID idPower, void * pData)
{


    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACDowner = PowerGetOwner(idPower);

    if (!ptACDowner)
        return;



    if (!ptPower->tTarget.IsNone())
    {
        ACDID idACDTarget = ptPower->tTarget.GetACD();
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        BOOL bIsAnItem = ptACDTarget && ptACDTarget->IsItem();

        if (!bIsAnItem && ptPower->tTarget.IsInvalidForSource(PowerGetOwner(idPower)))
            return;
    }





    if (!PowerHasInfiniteAnimationTiming(*ptACDowner, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        AIQueueCheckActionCompleted((ActorID)ptACDowner->idOwner, ptPower->snoPower, QUEUED_ACTION_POWER);
    }


    SpellEffectRunNextEffect(idPower, pData);

    AIStateActivateCooldown(*ptPower);
}

static void sPowerTrigger_Impact(const PowerID idPower, void * pData)
{
    ImpactData * tImpact = (ImpactData *)pData;




    if (!SpellEffect_ProjectileTryReflect(idPower, tImpact))
    {
        SpellEffectRun(idPower, tImpact->eSpellEffect, pData);
    }
}



static void sPowerTrigger_EndMessage(const PowerID idPower, void * pData)
{



    SpellEffectRunEndEffect(idPower, pData);
}




void AnimationPower(Power * ptPower, PowerMessage eMessage, void * pParam)
{












    switch (eMessage)
    {
    case POWER_CREATE:
        sPower_Create(ptPower, pParam);
        break;
    case POWER_POPULATE_ATTRIBUTES:
        sAnimationPower_PopulateAttributes(ptPower->id, (AttribGroupID)(intptr_t)pParam);
        break;
    case POWER_UI_DAMAGE_NUMBERS:
        sAnimationPower_UI_Damage_Numbers(ptPower->id, (PowerAPIAttackPayload *)pParam);
        break;

    case POWER_START:
        SpellEffectRunBeginEffect(ptPower->id, pParam);
        if (ptPower && !(ptPower->dwFlags & (1 << PF_STOPPING)))
        {
            sAnimationPower_Start(ptPower->id);
        }
        break;


    case POWER_CONTACT:
        sAnimationPower_Contact(ptPower->id, pParam);
        break;
    case POWER_IMPACT:
        sPowerTrigger_Impact(ptPower->id, pParam);
        break;
    case POWER_INTERRUPT:
        if (PowerGetInterruptedEffect(ptPower->id) != SPELL_INVALID)
        {
            SpellEffectRunInterruptedEffect(ptPower->id, NULL);
        }
        AIStateActivateCooldown(*ptPower);
        break;
    case POWER_UPDATE_TARGET:
        PowerHelper_UpdateTargetStandard(ptPower->id, (const UpdateTargetData *)pParam);
        break;
    case POWER_END:
        sPowerTrigger_EndMessage(ptPower->id, pParam);
        break;
    default:
        break;
    }
}






void NonAnimationPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam)
{



    switch (eMessage)
    {
    case POWER_CREATE:
        sPower_Create(ptPower, pParam);
        break;
    case POWER_POPULATE_ATTRIBUTES:
        PowerHelper_PopulateCommonAttributes(ptPower->id, (AttribGroupID)(intptr_t)pParam);
        break;
    case POWER_START:
        SpellEffectRunBeginEffect(ptPower->id, pParam);
        SpellEffectRunNextEffect(ptPower->id, pParam);
        break;
    case POWER_CONTACT:
        SpellEffectRunNextEffect(ptPower->id, pParam);
        AIStateActivateCooldown(*ptPower);
        break;
    case POWER_IMPACT:
        sPowerTrigger_Impact(ptPower->id, pParam);
        break;
    case POWER_INTERRUPT:
        if (PowerGetInterruptedEffect(ptPower->id) != SPELL_INVALID)
        {
            SpellEffectRunInterruptedEffect(ptPower->id, NULL);
        }
        AIStateActivateCooldown(*ptPower);
        break;
    case POWER_END:
        sPowerTrigger_EndMessage(ptPower->id, pParam);
        break;
    default:
        break;
    }
}


static void sSpecialWalk_SetDestination(
    const PowerID idPower,
    WorldPlace & tTargetPlace)
{
    Power *ptPower = PowerTryToGet(idPower);
    ActorCommonData *ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }

    ptPower->tTarget.GetPlace(tTargetPlace);

    WorldPlace tCurrentPlace = ptACD->GetPlace();
    if (tCurrentPlace.idSWorld != tTargetPlace.idSWorld)
    {
        tTargetPlace = tCurrentPlace;
        return;
    }

    if (PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_PERTURB_DESTINATION) != 0.0f)
    {
        NavPlace tNavPlace;
        ptPower->tTarget.GetNavPlace(tNavPlace);
        PowerAPIChooseOpenGround tOpenGroundInfo;
        PowerHelper_ChooseOpenGroundInit(idPower, tNavPlace, tOpenGroundInfo, 1.0f);
        WorldPlace placeResult;
        NavMeshHandle tNavMeshResult;
        if (PowerAPI_DoChooseOpenGround(*ptACD, tOpenGroundInfo, placeResult, tNavMeshResult))
        {
            tTargetPlace = placeResult;
        }
    }






    float flDistanceMin = PowerGetFormulaValue(idPower, TAG_POWER_WALKING_DISTANCE_MIN);
    if (flDistanceMin >= EPSILON)
    {
        float flDistanceMinSqr = flDistanceMin * flDistanceMin;


        Vector3D vDiff = tTargetPlace.wp - tCurrentPlace.wp;
        float flMagSqr = vDiff.MagnitudeSquared();
        if (flMagSqr > (EPSILON * EPSILON) && flMagSqr < flDistanceMinSqr)
        {

            vDiff.Normalize();
            vDiff *= flDistanceMin;
            tTargetPlace.wp += vDiff;
        }
    }

    if (PowerShouldRollToDestination(ptPower->snoPower))
    {
        ActorID idActor = (ActorID)ptACD->idOwner;
        Actor *ptActor = ActorGet(idActor);
        if (!ptActor->ptSActorMovement)
        {
            return;
        }



        ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
        BOOL bActivateTriggers = (pdefPower->tPower.uFlags & (1 << POWERBIT_REQUIRES_WALKING_PATH_TO_TARGET)) != 0;
        PowerComputeDestination(ptPower->snoPower, *ptActor->ptSActorMovement, tTargetPlace, tTargetPlace, bActivateTriggers, 10.0f);
    }
}



static void sSpecialWalk_SetTarget(
    const PowerID idPower,
    WorldPlace & tTargetPlace)
{
    Power *ptPower = PowerTryToGet(idPower);


    ACDID idACDTarget = ptPower->tTarget.GetACD();
    if (idACDTarget != ACDID_INVALID)
    {
        PowerAddTargettedACD(idPower, idACDTarget);
    }


    sSpecialWalk_SetDestination(idPower, tTargetPlace);
    ptPower->tTarget.InitializeFromPlace(tTargetPlace);
}



static void sSpecialWalk_InitTarget(const PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    WorldPlace tTargetPlace;
    ptPower->tTarget.GetPlace(tTargetPlace);
    ptPower->tTarget.InitializeFromPlace(tTargetPlace);
}





static void sSpecialWalk_StartMove(
    const PowerID idPower,
    void * pData)
{
    Power *ptPower = PowerTryToGet(idPower);



    ActorCommonData *ptACD = PowerGetOwner(ptPower);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }

    ActorID idActor = (ActorID)ptACD->idOwner;
    Actor *ptActor = ActorGet(idActor);



    PowerID idPrimaryPower = ActorPowersGetPrimary(idActor);
    if (idPrimaryPower != idPower)
    {
        return;
    }


    WorldPlace tTargetPlace;
    sSpecialWalk_SetTarget(idPower, tTargetPlace);



    WorldDistance wdSpeed = ptACD->GetWalkingSpeed(TAG_ACTOR_ANIMATION_RUN);
    float flSpeedMultiplier = PowerGetFormulaValue(idPower, TAG_POWER_WALKING_SPEED_MULTIPLIER);
    wdSpeed *= flSpeedMultiplier;


    KnockbackPowerData * ptKnockbackData = NULL;
    if (PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_IS_KNOCKBACK) != 0.0f)
    {
        ptKnockbackData = (KnockbackPowerData *)pData;
    }


    float flHeight;
    if (ptKnockbackData)
    {
        flHeight = ptKnockbackData->flHeight;
    }
    else
    {
        flHeight = PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_TRAJECTORY_HEIGHT);
    }


    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
    if (flHeight > EPSILON)
    {
        float flGravity;
        if (ptKnockbackData)
        {
            flGravity = ptKnockbackData->flGravity;
        }
        else
        {
            flGravity = PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_TRAJECTORY_GRAVITY);
        }
        if (flGravity >= EPSILON)
        {
            flGravity = 0.0f;
        }




        uint32 uMoveFlags = (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING) | (1 << MOVEBIT_AUTO_UPDATE_YAW);
        if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_START_WALK_AFTER_INTRO))
        {
            uMoveFlags |= (1 << MOVEBIT_START_WALK_AFTER_INTRO);
        }

        if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_ARC_MOVE_UNTIL_DEST_HEIGHT))
        {
            uMoveFlags |= (1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT);
        }

        if (ACDIsPlayer(ptACD->id) ||
            IsTriggerEventTargetingActor(idActor))
        {

            // Inference: no row of its own; folded into the select on line 1306.
            if (!(pdefPower->tPower.uFlags & (1 << POWERBIT_REQUIRES_WALKING_PATH_TO_TARGET)))
            {
                uMoveFlags |= (1 << MOVEBIT_ACTIVATE_TRIGGERS);
            }
        }

        if (PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_TRAJECTORY_HEIGHT_IS_RELATIVE_TO_MAX) != 0.0f)
        {
            WorldPlace placeCurr = ptACD->GetPlace();
            float flHighest = Math::Max(tTargetPlace.wp.z, placeCurr.wp.z);
            flHeight += flHighest - placeCurr.wp.z;
        }

        ActorAnimationTag eAnimTagLand = (ActorAnimationTag)PowerGeneralTagGetInt(pdefPower, TAG_POWER_ANIMATION_TAG_2);
        BOOL bSuccess = ptActor->ptSActorMovement->MoveWithArc(tTargetPlace, (ActorAnimationTag)ptPower->dwAnimationTag, eAnimTagLand, uMoveFlags, flGravity, flHeight, ptPower->snoPower, 0.0f);
        if (!bSuccess)
        {
            PowerForceStop(idPower, TRUE);
        }
    }
    else
    {
        uint32 uMoveFlags = (1 << MOVEBIT_FROM_POWER) | (1 << MOVEBIT_NO_ACCEL);
        if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_START_WALK_AFTER_INTRO))
        {
            uMoveFlags |= (1 << MOVEBIT_START_WALK_AFTER_INTRO);
        }

        BOOL bNeverUpdatesFacingDuring = PowerGeneralTagGetBool(pdefPower, TAG_POWER_NEVER_UPDATES_FACING_DURING);
        if (!bNeverUpdatesFacingDuring)
            uMoveFlags |= (1 << MOVEBIT_AUTO_UPDATE_YAW);
        if (!PowerCanPathDuringWalk(ptPower->snoPower))
        {
            uMoveFlags |= (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING);
        }

        int32 nMaxTicksToWalk = -1;
        if (PowerGetChargeSteeringType(ptPower->snoPower) != CHARGE_STEERING_NONE)
        {
            // Inference: DWARF rows put both updates on line 1346; |= before &= is the order that reproduces the codegen.
            uMoveFlags |= (1 << MOVEBIT_USE_CHARGE_STEERING);
            uMoveFlags &= ~(1 << MOVEBIT_NO_STEERING);
            if (PowerGetChargeSteeringType(ptPower->snoPower) == CHARGE_STEERING_WITH_PATHING)
            {
                // Inference: as above, both updates sit on DWARF line 1351.
                uMoveFlags |= (1 << MOVEBIT_DYNAMIC_PATHING);
                uMoveFlags &= ~(1 << MOVEBIT_NO_PATHFINDING);
            }
            if (wdSpeed > EPSILON)
            {
                const float CHARGE_STEERING_DISTANCE_CAP = 60.0f;
                nMaxTicksToWalk = Math::FloatRoundToInt((CHARGE_STEERING_DISTANCE_CAP / wdSpeed) + 0.5f);
            }
            // Inference: CHARGE_STEERING_DISTANCE_CAP (DWARF line 1356, no const_value) = 60.0f from the literal pool.
            ActorSteer &tSteer = ptActor->ptSActorMovement->GetSteer();
            tSteer.Reset180Steering(Vector2D(0.0f, 0.0f));
        }

        if (ACDIsPlayer(ptACD->id) ||
            IsTriggerEventTargetingActor(idActor))
        {
            uMoveFlags |= (1 << MOVEBIT_ACTIVATE_TRIGGERS);
        }


        uint32 uWalkInPlaceTicks = 0;
        float flMinWalkDuration = PowerGetFormulaValue(idPower, TAG_POWER_WALKING_DURATION_MIN);
        if (flMinWalkDuration > 0.0f)
        {
            uWalkInPlaceTicks = (uint32)(flMinWalkDuration * TICKS_PER_SECOND);
        }




        float flMaxRangeFromDest = wdSpeed * 0.2f;
        ptActor->ptSActorMovement->MoveTo(tTargetPlace, wdSpeed, 0.0f, (ActorAnimationTag)ptPower->dwAnimationTag, uMoveFlags, flMaxRangeFromDest, ACDID_INVALID, ptPower->snoPower, uWalkInPlaceTicks, nMaxTicksToWalk, 100.0f, NULL);
    }
}



BOOL SpecialWalkPower_Start(
    const PowerID idPower,
    void * pData)
{
    Power *ptPower = PowerTryToGet(idPower);
    ActorCommonData *ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return FALSE;
    }

    ActorID idActor = (ActorID)ptACD->idOwner;
    Actor *ptActor = ActorGet(idActor);
    if (!ptActor->ptSActorMovement)
    {
        return FALSE;
    }





    ptPower->dwFlags |= (1 << PF_MANUALLY_FINISH);

    if (PowerShouldUseClientWalking(*ptACD, ptPower->snoPower))
    {
        ptPower->dwFlags |= (1 << PF_USING_CLIENT_AUTO_WALK);
    }
    else
    {


        ptActor->ptSActorMovement->StopWalk();
        ptActor->ptSActorMovement->StopNow();


        BOOL bStartNow = TRUE;

        ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
        if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_START_WALK_AFTER_INTRO))
        {

            uint32 dwStartTag;
            uint32 dwEndTag;
            AnimTagGetStartAndEndTags(ptPower->dwAnimationTag, dwStartTag, dwEndTag);
            if (AnimPlayerAnimExists(*ptACD, dwStartTag))
            {

                AnimationTiming tAnimationTiming;
                AnimPlayerGetAnimationTiming(*ptACD, ptPower->dwAnimationTag, ANIM_PLAY_FOREVER, 1.0f, tAnimationTiming);
                uint32 uDelayBeforeMoving = tAnimationTiming.tAnimSpec[ANIMOFFSET_START].dwDuration;

                // Inference: no row of its own; StartMove is not called directly on this path (timer instead).
                bStartNow = FALSE;

                PowerTriggerAddTimerTrigger(idPower, uDelayBeforeMoving, sSpecialWalk_StartMove, pData);




                PreplayedAnimData tPreplayedAnimData;
                tPreplayedAnimData.dwServerTimeAnimStarted = GameGetServerTime();
                tPreplayedAnimData.dwSyncedSeed = 0;
                tPreplayedAnimData.dwPrePlayedAnimFlags = 0;


                PowerTriggerPlayTriggerAnimation(
                    idPower,
                    dwStartTag,
                    (uint32)-1,
                    1.0f,
                    FALSE,
                    tPreplayedAnimData,
                    ANIM_REASON_POWER,
                    CONTACT_FRAME_IN_START,
                    FALSE);


                float flTargetingDelay = PowerGetFormulaValue(idPower, TAG_POWER_DELAY_BEFORE_SETTING_TARGET);
                if (flTargetingDelay > 0.0f)
                {
                    uint32 dwDelayBeforeTargeting = Math::Min((uint32)(flTargetingDelay * TICKS_PER_SECOND), uDelayBeforeMoving);
                    PowerTriggerAddTimerTrigger(idPower, dwDelayBeforeTargeting, sSpecialWalk_InitTarget, NULL);
                }
                else if (!PowerGeneralTagGetBool(pdefPower, TAG_POWER_SET_TARGET_AFTER_INTRO))
                {

                    WorldPlace tTargetPlace;
                    ptPower->tTarget.GetPlace(tTargetPlace);
                    ptPower->tTarget.InitializeFromPlace(tTargetPlace);
                }
            }
        }



        if (bStartNow)
            sSpecialWalk_StartMove(idPower, pData);

    }

    SpellEffectRunNextEffect(idPower, pData);
    SpellEffectIncrement(idPower);
    return TRUE;

}



BOOL SpecialWalkPower_End(const PowerID idPower, void *)
{


    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
        return TRUE;



    ActorCommonData & tACD = ACDGet(ptPower->idACD);


    ptPower->dwFlags &= ~(1 << PF_MANUALLY_FINISH);


    uint32 dwAnimTag = PowerDetermineAnimation(AutoPowerDefinitionR(ptPower->snoPower), tACD.id, cgTargetNone, FALSE);
    uint32 dwCoolDownTag = AnimPlayerGetWalkCoolDownTag(tACD, dwAnimTag);
    if (dwCoolDownTag != (uint32)-1)
    {
        PreplayedAnimData tPreplayedAnimData;
        tPreplayedAnimData.dwServerTimeAnimStarted = GameGetServerTime();
        tPreplayedAnimData.dwSyncedSeed = 0;
        tPreplayedAnimData.dwPrePlayedAnimFlags = 5;


        uint32 uNow = GameGetServerTime();
        if (ptPower->dwStartTick + 1 >= uNow)
        {





            tPreplayedAnimData.dwPrePlayedAnimFlags = 0;
            ActorMovement * ptMove = tACD.GetActorMovement();
            if (ptMove)
            {
                ptMove->StopNow();
            }
        }


        float flScalar = ACDIsPlayer(tACD.id) ?

            PowerGetFormulaValue(idPower, TAG_POWER_SPECIALWALK_PLAYER_END_ANIM_SCALAR) : 1.0f;


        PowerTriggerPlayTriggerAnimation(idPower, dwCoolDownTag, (uint32)-1, flScalar, FALSE, tPreplayedAnimData, ANIM_REASON_POWER, CONTACT_FRAME_IN_END, FALSE);
    }










    SNO snoEndingEffect = PowerGetEffect(idPower, TAG_POWER_ENDING_EFFECT_GROUP);
    if (snoEndingEffect != SNO_NONE)
    {
        ACDPlayEffect(tACD.id, EFFECT_PLAY_EFFECT_GROUP, snoEndingEffect, tACD.id);
    }


    BOOL bInterrupted = FALSE;
    if (dwCoolDownTag == (uint32)-1)
    {
        PowerInterrupt(idPower, FALSE);
        bInterrupted = TRUE;
    }
    if (PowerIsKnockbackMovementPower(AutoPowerDefinitionR(ptPower->snoPower)))
    {
        PowerAPI_BuffApplyOnKnockbackLandEffects(tACD.id);
    }

    AIStateActivateCooldown(*ptPower);


    return bInterrupted;
}
void SpecialWalkPowerMessageHandler(Power * ptPower, PowerMessage eMessage, void * pParam)
{



    switch (eMessage)
    {

    case POWER_CREATE:
        sPower_Create(ptPower, pParam);
        break;
    case POWER_POPULATE_ATTRIBUTES:
        PowerHelper_PopulateCommonAttributes(ptPower->id, (AttribGroupID)(intptr_t)pParam);
        break;
    case POWER_UI_DAMAGE_NUMBERS:
        PowerHelper_AttackPayloadInit_Standard(ptPower->id, *(PowerAPIAttackPayload *)pParam);
        break;
    case POWER_START:
    {
        ActorCommonData * ptACD = PowerGetOwner(ptPower->id);
        ActorID idActor = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;

        PowerID idPrimaryPower = ActorPowersGetPrimary(idActor);






        if (SpecialWalkPower_Start(ptPower->id, pParam))
        {
            idPrimaryPower = ActorPowersGetPrimary(idActor);
            if (ptPower->id == idPrimaryPower)
            {







                SpellEffectRunBeginEffect(ptPower->id, pParam);
                break;
            }
        }


        PowerInterrupt(ptPower->id, FALSE);
        break;
    }

    case POWER_IMPACT:
        sPowerTrigger_Impact(ptPower->id, pParam);
        break;


    case POWER_ARRIVED:
        if (!SpecialWalkPower_End(ptPower->id, NULL))
            break;
    case POWER_END:
        sPowerTrigger_EndMessage(ptPower->id, pParam);
        break;

    case POWER_UPDATE_TARGET:
        PowerHelper_UpdateTargetStandard(ptPower->id, (const UpdateTargetData *)pParam);
        break;
    case POWER_INTERRUPT:
        if (PowerGetInterruptedEffect(ptPower->id) != SPELL_INVALID)
        {
            SpellEffectRunInterruptedEffect(ptPower->id, NULL);
        }
        AIStateActivateCooldown(*ptPower);
        break;
    case POWER_CONTACT:
        SpellEffectRunNextEffect(ptPower->id, pParam);
        break;
    default:
        break;
    }
}
// LINKER-REMOVED: DWARF DIE 0x06ff4a66 (low_pc 0, 0x160 bytes; params idPower 1664, flDelay 1665; ptPower 1667;
// dwLeft 1670/1675/1680; Contact and ActorDone helpers inlined at 1671/1676) and its rows at address 0 (cols 9/18/25/55-67).
// Inferred: `!= TIMERID_INVALID` (two col-9 insns), PowerGet (no null test), the subtrahend GameGetServerTime() (PowerAPI.cpp:5656 idiom).
void PowerTriggerUpdateTimersAddDelay(
    const PowerID idPower,
    float flDelay)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower->idTimerContact != TIMERID_INVALID)
    {
        uint32 dwLeft = TimerGetTime(ptPower->idTimerContact) - GameGetServerTime();
        sPowerTriggerRegisterContact(idPower, (uint32)(dwLeft + flDelay), 0);
    }
    if (ptPower->idTimerActorDone != TIMERID_INVALID)
    {
        uint32 dwLeft = TimerGetTime(ptPower->idTimerActorDone) - GameGetServerTime();
        sPowerTriggerRegisterActorDone(idPower, (uint32)(dwLeft + flDelay));
    }
    if (ptPower->idTimerInterrupt != TIMERID_INVALID)
    {
        uint32 dwLeft = TimerGetTime(ptPower->idTimerInterrupt) - GameGetServerTime();
        sPowerTriggerRegisterInterrupt(idPower, (uint32)(dwLeft + flDelay));
    }
}



void LoopingAnimationPower_StartTimer(const PowerID idPower, float flSeconds, BOOL bShowProgressBar, BOOL bInterruptible)
{




    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptPower || !ptACD)
        return;




    ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);
    ptPower->dwFlags |= (1 << PF_MANUALLY_FINISH);

    PowerAPIAnimation tAnimation;
    PowerHelper_AnimationInit(idPower, tAnimation);

    tAnimation.dwDuration = (uint32)-2;

    AnimReason eAnimReason = ANIM_REASON_INTERRUPTABLE_ANIMATION;
    if (ptPower->snoPower != SNO_NONE)
    {
        ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
        if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_LOOKSWITCH_DOESNT_INTERRUPT))
        {
            eAnimReason = ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH;
        }
    }

    PowerTriggerPlayTriggerAnimation(
        tAnimation.idPower,
        tAnimation.dwAnimationTag,
        tAnimation.dwDuration,
        tAnimation.flSpeedScalar,
        tAnimation.bSetAimTarget,
        tAnimation.tPreplayedAnimData, eAnimReason, CONTACT_FRAME_NONE, TRUE);



    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.nBuffIndex = 0;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);

    if (flSeconds > 0.0f)
    {
        int32 nTicks = Math::FloatToInt(flSeconds * TICKS_PER_SECOND);
        sPowerTriggerRegisterContact(idPower, nTicks, 0);

        if (bShowProgressBar)
        {
            uint32 uNow = GameGetServerTime();
            uint32 uEndTime = uNow + nTicks;
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_LOOPING_ANIMATION_START_TIME, uNow);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_LOOPING_ANIMATION_END_TIME, uEndTime);

            if (PowerGeneralTagGetBool(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_LOOPING_SUPPRESS_ITEM_TOOLTIPS))
            {
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_LOOPING_ANIMATION_SUPPRESS_ITEM_TOOLTIPS, TRUE);
            }
        }
    }


    if (bInterruptible)
    {
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ALWAYS_PLAYS_GETHIT, TRUE);
    }


    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}
static void sLoopingAnimationPower_Start(const PowerID idPower)
{


    float flSeconds = PowerGetFormulaValue(idPower, TAG_POWER_LOOPING_ANIMATION_TIME);
    LoopingAnimationPower_StartTimer(idPower, flSeconds, TRUE, TRUE);
}


void LoopingAnimationPower_RemoveBuff(const PowerID idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = PowerGetOwner(idPower);
        if (ptACD)
        {
            BuffID idBuff = PowerAPI_BuffFindOnACDByPowerID(ptPower->snoPower, ptACD, idPower);
            if (idBuff != BUFFID_INVALID)
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, FALSE, TRUE);
        }
    }
}






void LoopingAnimationPower(Power * ptPower, PowerMessage eMessage, void * pParam)
{




    switch (eMessage)
    {

    case POWER_CREATE:
        sPower_Create(ptPower, pParam);
        break;
    case POWER_POPULATE_ATTRIBUTES:
        sAnimationPower_PopulateAttributes(ptPower->id, (AttribGroupID)(intptr_t)pParam);
        break;
    case POWER_UI_DAMAGE_NUMBERS:
        sAnimationPower_UI_Damage_Numbers(ptPower->id, (PowerAPIAttackPayload *)pParam);
        break;

    case POWER_START:
        SpellEffectRunBeginEffect(ptPower->id, pParam);
        if (ptPower && !(ptPower->dwFlags & (1 << PF_STOPPING)))
        {
            sLoopingAnimationPower_Start(ptPower->id);
        }
        break;


    case POWER_CONTACT:
        sAnimationPower_Contact(ptPower->id, pParam);
        PowerForceStop(ptPower->id, TRUE);
        break;

    case POWER_IMPACT:
        sPowerTrigger_Impact(ptPower->id, pParam);
        break;
    case POWER_INTERRUPT:
        LoopingAnimationPower_RemoveBuff(ptPower->id);
        if (PowerGetInterruptedEffect(ptPower->id) != SPELL_INVALID)
        {
            SpellEffectRunInterruptedEffect(ptPower->id, NULL);
        }
        AIStateActivateCooldown(*ptPower);
        break;
    case POWER_UPDATE_TARGET:
        PowerHelper_UpdateTargetStandard(ptPower->id, (const UpdateTargetData *)pParam);
        break;
    case POWER_END:
        LoopingAnimationPower_RemoveBuff(ptPower->id);
        ptPower->dwFlags &= ~(1 << PF_MANUALLY_FINISH);
        sPowerTrigger_EndMessage(ptPower->id, pParam);
        break;
    default:
        break;
    }
}
