// Original file: X/Client/RActors/AnimationChain.cpp
#include "RActors.h"
#include "AnimationChain.h"
#include "RActorAnimation.h"
#include "../CGame.h"
#include "../../Common/AnimCommon.h"
#include "../../Common/Definitions/AnimationDefinition.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"

void AnimationChannelPlay(RActor * ptRActor, const AnimChannelPlayParams & tChannelPlayParams, const BOOL bDisableBlending);  // X/Client/Animation.cpp

static void sAnimationChainPlayOne(const RActorID idRActor, uint32 dwIndex);

// Removed by the linker (DW_AT_low_pc 0); the DWARF keeps its signature, locals and the inlined
// RActorTryToGet at line 74. It is written because, without a second caller, sAnimationChainPlayOne
// is inlined into AnimationChainUpdate, which the original does not do. Everything after the locals
// (the checks and the call) is a reconstruction.
//@ 71
void AnimationChainRestart(const RActorID idRActor)
{
//@ 74
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor == NULL || ptRActor->ptActorAnim == NULL)
    {
        return;
    }
//@ 81
    ActorAnim & tActorAnim = *ptRActor->ptActorAnim;
    AnimationChain & tActorAnimationChain = tActorAnim.tAnimationChain;
    if (!tActorAnimationChain.listChainEntries.IsEmpty())
    {
        sAnimationChainPlayOne(idRActor, 0);
    }
}

//@ 118
static void sAnimationChainDispose(const RActorID idRActor)
{
//@ 124
    ActorAnim * ptActorAnim = RActorGet(idRActor)->ptActorAnim;
    ptActorAnim->tAnimationChain.listChainEntries.RemoveAll();
}

//@ 152
static const AnimationChainEntry & sAnimationChainGetIndex(const RActorID idRActor, uint32 dwIndex)
{
    ActorAnim * ptActorAnim = RActorGet(idRActor)->ptActorAnim;
    TAnimChainEntries & listChainEntries = ptActorAnim->tAnimationChain.listChainEntries;
    POS pos = listChainEntries.GetHeadPosition();
//@ 162
    dwIndex = Math::Min<int32>(dwIndex, listChainEntries.Count() - 1);

    while (dwIndex > 0)
    {
        listChainEntries.GetNext(pos);
        dwIndex--;
    }
    return listChainEntries.GetAt(pos);
}

//@ 178
static void sAnimationChainPlayOne(const RActorID idRActor, uint32 dwIndex)
{
//@ 181
    RActor * ptRActor = RActorGet(idRActor);

//@ 184
    ActorAnim * ptActorAnim = ptRActor->ptActorAnim;
    AnimationChain & tAnimationChain = ptActorAnim->tAnimationChain;
    const AnimationChainEntry & tEntry = sAnimationChainGetIndex(idRActor, dwIndex);

//@ 189
    const AnimationDefinition * pdefAnim = DefinitionLockR<AnimationDefinition>(tEntry.snoAnim);
    AnimChannelPlayParams tChannelPlayParams;
    AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, CGameGlobalsGet().rngLocal);

//@ 193
    if (tAnimationChain.nLoopMode == ANIMLOOP_PLAYONCE)
    {
        tChannelPlayParams.ePlaybackMode = PLAYBACKMODE_NORMAL;
    }

//@ 199
    BOOL bDisableBlending; if (tChannelPlayParams.ePlaybackMode == PLAYBACKMODE_NORMAL && tAnimationChain.listChainEntries.Count() == 1) { bDisableBlending = TRUE; } else { bDisableBlending = FALSE; }

//@ 211
    if (tEntry.iPermutation >= 0)
    {
        tChannelPlayParams.nPermutationIndex = tEntry.iPermutation;
    }

//@ 216
    AnimationChannelPlay(ptRActor, tChannelPlayParams, bDisableBlending);

    float flLength = AnimGetDurationTimeFromPlayParams(tChannelPlayParams, pdefAnim) * (1.0f / 60.0f);

//@ 222
    tAnimationChain.flAnimationEndTime = CGameGetClientTime() + flLength;
    tAnimationChain.fNextUpdateIsAtEnd = FALSE;
    tAnimationChain.dwCurrentIndex = dwIndex;
    DefinitionUnlockR<AnimationDefinition>(pdefAnim);
}

//@ 230
static void sAnimationChainDurationEvent(RActor * ptRActor)
{
//@ 233
    if (ptRActor->ptActorAnim == NULL)
    {
        return;
    }
    AnimationChain & tAnimationChain = ptRActor->ptActorAnim->tAnimationChain;

//@ 242
    uint32 dwIndex = tAnimationChain.dwCurrentIndex + 1;

    if ((int32)dwIndex >= tAnimationChain.listChainEntries.Count())
    {
        switch (tAnimationChain.nLoopMode)
        {
        case ANIMLOOP_NONE:
        case ANIMLOOP_PLAYONCE:
//@ 250
            sAnimationChainDispose(ptRActor->id);
            return;
        case ANIMLOOP_LAST:
//@ 254
            dwIndex = tAnimationChain.listChainEntries.Count() - 1;
            break;
        case ANIMLOOP_ALL:
            dwIndex = 0;
            break;
        default:
//@ 265
            DisplayInternalError(NULL, "", 265, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            return;
        }
    }
//@ 269
    sAnimationChainPlayOne(ptRActor->id, dwIndex);
}

//@ 455
void AnimationChainUpdate(RActor * ptRActor, float flDeltaTime)
{
//@ 459
    ActorAnim * ptActorAnim = ptRActor->ptActorAnim;
    AnimationChain & tAnimationChain = ptActorAnim->tAnimationChain;

//@ 462
    if (tAnimationChain.listChainEntries.Count() == 0)
    {
        return;
    }

//@ 469
    if (ptActorAnim->tAnimChannel.bPaused)
    {
        tAnimationChain.flAnimationEndTime += flDeltaTime;
    }

//@ 474
    float flClientTime = CGameGetClientTime();

    if (tAnimationChain.fNextUpdateIsAtEnd)
    {
        sAnimationChainDurationEvent(ptRActor);
    }
//@ 480
    else if (flClientTime > tAnimationChain.flAnimationEndTime)
    {
//@ 483
        tAnimationChain.fNextUpdateIsAtEnd = TRUE;
    }
}

//@ 490
void AnimationChainInit(const RActorID idRActor)
{
//@ 498
}

//@ 503
void AnimationChainDispose(const RActorID idRActor)
{
//@ 511
}
