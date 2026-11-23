#include "Animation.h"
#include "Attachments.h"
#include "CGame.h"
#include "CWorld.h"
#include "Definitions/AppearanceDefinition.h"
#include "Gfx/GfxVB.h"
#include "RActors/RActorAnimation.h"
#include "RActors/RActorAttachments.h"
#include "RActors/RActors.h"
#include "../Common/Definitions/AnimTreeDefinition.h"
#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include <float.h>
// Declared in X/Common/AnimCommon.h (not yet in src/).
float AnimationGetGlobalSpeedScale(); struct AnimBoneStack;
int32 AnimationSelectPermutation(const AnimationDefinition * pdefAnim, RNG & rng);
BOOL AnimCanRagdollFromPlayParams(const struct AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim);
void AnimationGetAvgVelocity(const AnimationDefinition * pdefAnim, const int32 nPermutationIndex, const float flTranslationScale, const BOOL bMultipleBySpeedScale, Vector3D & wvVel);

#include "../Common/AnimCommon.h"






void AnimationSetChannelPlayParams(AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim, RNG & rng);
// X/Common/Definitions/AnimSetDefinition.h
SNO AnimSetGetAnim(SNO snoAnimSet, const WeaponClass eWeaponClass, const uint32 dwAnimTagId);
void sAnimTreeChannelPlay(RActor * ptRActor, AnimChannel & tChannel, const AnimChannelPlayParams & tChannelPlayParams, const BOOL bDisableBlending);
typedef BOOL (*TreeRecursiveCallback)(AnimTree & tTree, uint32 dwLeafIndex);
void sAnimTreeBlendBones(uint32 dwBlendFlags, uint32 dwRootIndex, const AnimTreeDefinition & tTreeDefinition, const AnimTreeControl & tControl, AnimBoneStack & tBoneStack, AnimTree & tTree);
BOOL sAnimTreeNodeRecursiveAnyTrueCallback(uint32 dwRootIndex, const AnimTreeDefinition & tTreeDefinition, const AnimTreeControl & tControl, AnimTree & tTree, TreeRecursiveCallback ptCallback);

static Blizzard::Lock::RMutex stAnimToStructureMapLock;

static int32 RETRO_FRAMERATE_FACTOR = 2;
static float MIN_FRAMERATE_FOR_RETRO_FILTER = 20.0f;





enum {
    ANIMTREE_BLEND_BONES = 0,
    ANIMTREE_BLEND_WEIGHTS = 1,
};










struct AnimationGlobals {
    PRSTransform * ptTemp0;
    PRSTransform * ptTemp1;
    float flGlobalSpeedScale;
};





















void AnimChannel::Init(int nBoneCount, SNO snoAppearance)
{
    // ePlaybackMode, nFrameCount stay cleared
    flAnimSpeedScale = 1.0f;
    flTimeScale = 1.0f;
    flFrameRate = 0.0f;



    tAnimPermID.snoAnim = -1;
    tAnimPermID.nPermutationIndex = -1;
    flTLast = -1.0f;
    flT = 0.0f;
    flScaleDeltaT = 0.0f;


    arBoneStates = NULL;
    nBoneStateCount = nBoneCount;
    fNeedsInterpolation = FALSE;
    bDisableRetroFiltering = FALSE;

    pnChannelAnimToStructure = NULL;
    snoMyAppearance = snoAppearance;
    snoCurrentAnimToStructure = -1;
    nCurrentAnimPermutation = -1;
    pnCustomAnimToStructure = NULL;
}

void AnimChannel::Dispose()
{
    SigmaMemoryFree(arBoneStates, NULL); arBoneStates = NULL;
    SigmaMemoryFree(pnCustomAnimToStructure, NULL); pnCustomAnimToStructure = NULL;
}


int32 AnimChannel::GetCurrentFrame() const
{


    if (ShouldApplyRetroFilter() && flFrameRate >= MIN_FRAMERATE_FOR_RETRO_FILTER)
    {
        return Math::FloatToInt(flFrameRate * flT / RETRO_FRAMERATE_FACTOR) * RETRO_FRAMERATE_FACTOR;
    }

    return Math::FloatToInt(flT * flFrameRate);
}

int32 AnimChannel::GetNextFrame() const
{

    int32 nNextFrame = GetCurrentFrame();
    if (!IsStopped())
    {





        nNextFrame += ShouldApplyRetroFilter() && flFrameRate >= MIN_FRAMERATE_FOR_RETRO_FILTER ? RETRO_FRAMERATE_FACTOR : 1;

        switch (ePlaybackMode)
        {
        case PLAYBACKMODE_NORMAL:

            if (nNextFrame >= nFrameCount)
            {
                nNextFrame = nFrameCount - 1;
            }
            break;

        case PLAYBACKMODE_LOOP:

            if (nNextFrame >= nFrameCount)
            {
                nNextFrame = 0;
            }
            break;
        default:

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }

    return nNextFrame;
}
BOOL AnimChannel::ShouldApplyRetroFilter() const
{

    return CWorldIsRetro() && !bDisableRetroFiltering;
}

BOOL AnimChannel::Advance(float flDeltaTime)
{

    if (IsStopped())
        return FALSE;





    flTLast = flT;
    flT += flScaleDeltaT * flDeltaTime;

    if (flT > flTMax)
    {
        switch (ePlaybackMode)
        {
        case PLAYBACKMODE_NORMAL:
            flT = flTMax;


            if (flTLast < flTMax)
                return FALSE;




            flScaleDeltaT = 0.0f;
            fNeedsInterpolation = TRUE;
            return TRUE;

        case PLAYBACKMODE_LOOP:

            while (flT > flTMax)
            {
                flT -= flTMax;
            }
            return TRUE;
        default:

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
    return FALSE;
}
// NON_MATCHING: stAnimToStructureMapLock sits at +0x18 of the merged statics, the original at +0x20 (one 4-byte static missing before it)



void AnimChannel::BuildAnimToStructureMap(const AnimationDefinition * ptAnimDef, const AnimPermutation & tAnimPerm, int32 nPermutationIndex)
{



    SNO snoAnim = ptAnimDef->tHeader.snoHandle;

    if (snoCurrentAnimToStructure == snoAnim &&
        nCurrentAnimPermutation == nPermutationIndex &&
        (ptAnimDef->snoAppearance != snoMyAppearance ||
         (tAnimPerm.pnAnimToStructure &&
          tAnimPerm.pnAnimToStructure == pnChannelAnimToStructure)))
        return;
    BOOL bPrevTraceEnable = SigmaMemorySetTraceAllocations(FALSE);

    BOOL _fOneToOne = FALSE;
    if (ptAnimDef->snoAppearance == snoMyAppearance) {

        Blizzard::Lock::ScopedMutex scopedLock(stAnimToStructureMapLock);

        if (!tAnimPerm.pnAnimToStructure)
        {
            ScopedAppearanceDefinitionLockR pdefAppearance(ptAnimDef->snoAppearance);
            const Structure & tStructure = pdefAppearance->tStructure;
            int16 * pnAnimToStructure = (int16 *)SigmaMemoryNew(tAnimPerm.nBoneCount * sizeof(int16), 4, NULL, FALSE);
            AnimationBuildAnimToStructureMapping(tStructure, tAnimPerm.nBoneCount, tAnimPerm.ptBoneNames, pnAnimToStructure, &_fOneToOne);
            const_cast<AnimPermutation &>(tAnimPerm).pnAnimToStructure = pnAnimToStructure;
        }
        else
        {
            _fOneToOne = TRUE;

            for (int32 i = 0; i < tAnimPerm.nBoneCount; i++)
            {
                if (tAnimPerm.pnAnimToStructure[i] != i)
                {
                    _fOneToOne = FALSE;
                    break;
                }
            }
        }

        pnChannelAnimToStructure = tAnimPerm.pnAnimToStructure;
        fOneToOne = _fOneToOne;
    }
    else
    {
        if (!pnCustomAnimToStructure)
        {
            pnCustomAnimToStructure = (int16 *)SigmaMemoryNew(MAX_BONES_PER_ACTOR * sizeof(int16), 4, NULL, FALSE);
        }

        ScopedAppearanceDefinitionLockR pdefAppearance(snoMyAppearance);
        const Structure & tStructure = pdefAppearance->tStructure;
        AnimationBuildAnimToStructureMapping(tStructure, tAnimPerm.nBoneCount, tAnimPerm.ptBoneNames, pnCustomAnimToStructure, &_fOneToOne);

        pnChannelAnimToStructure = pnCustomAnimToStructure;
        fOneToOne = _fOneToOne;
    }

    SigmaMemorySetTraceAllocations(bPrevTraceEnable);


    snoCurrentAnimToStructure = snoAnim; nCurrentAnimPermutation = nPermutationIndex;
}






void AnimationInitializeForGame()
{
    AnimationGlobals *& ptAnimGlobals = CGameGet().ptAnimGlobals;
    ptAnimGlobals = (AnimationGlobals *)SigmaMemoryNew(sizeof(AnimationGlobals), 4, NULL, TRUE);
    ptAnimGlobals->ptTemp0 = (PRSTransform *)SigmaMemoryNew(sizeof(PRSTransform) * MAX_BONES_PER_ACTOR, 4, NULL, FALSE);
    ptAnimGlobals->ptTemp1 = (PRSTransform *)SigmaMemoryNew(sizeof(PRSTransform) * MAX_BONES_PER_ACTOR, 4, NULL, FALSE);






    ptAnimGlobals->flGlobalSpeedScale = 1.0f;











}























































void AnimationDisposeForGame()
{
    CGame & tCGame = CGameGet();









    SigmaMemoryFree(tCGame.ptAnimGlobals->ptTemp0, NULL); tCGame.ptAnimGlobals->ptTemp0 = NULL;
    SigmaMemoryFree(tCGame.ptAnimGlobals->ptTemp1, NULL); tCGame.ptAnimGlobals->ptTemp1 = NULL;
    SigmaMemoryFree(tCGame.ptAnimGlobals, NULL); tCGame.ptAnimGlobals = NULL;


}




void AnimationBoneCacheInit(AnimBoneCache & tCache, int nBoneCount, int nNumBoneCacheEntries, const BoneStructure * ptBoneStructure)
{













    SigmaMemoryClear(&tCache, sizeof(AnimBoneCache));
    tCache.nNumEntries = nNumBoneCacheEntries;
    for (int32 i = 0; i < tCache.nNumEntries; i++)
    {









        tCache.tBoneCache[i].ptBone = (PRSTransform *)SigmaMemoryNew(sizeof(PRSTransform) * nBoneCount, 4, NULL, FALSE);
        for (int32 nBoneIndex = 0; nBoneIndex < nBoneCount; nBoneIndex++)
        {

            PRSTransform & tBone = tCache.tBoneCache[i].ptBone[nBoneIndex];
            {
                const BoneStructure & tStructureBone = ptBoneStructure[nBoneIndex];

                tBone = tStructureBone.transformParentRel;
            }
        }
    }




    tCache.nCacheIndex = -1;
    tCache.nPrevCacheIndex = -1;
    tCache.bCacheIsValid = FALSE;
}




void AnimationBoneCacheDispose(AnimBoneCache & tCache)
{
    for (int32 i = 0; i < tCache.nNumEntries; i++)
    {





        SigmaMemoryFree(tCache.tBoneCache[i].ptBone, NULL); tCache.tBoneCache[i].ptBone = NULL;
    }
}









inline void AnimationBoneCacheBump(AnimBoneCache & tCache, const BOOL fBlend)
{




    if (fBlend && tCache.bCacheIsValid)
    {



        tCache.nPrevCacheIndex = tCache.nCacheIndex++;
        tCache.nCacheIndex = tCache.nCacheIndex >= tCache.nNumEntries ? 0 : tCache.nCacheIndex;
    }
    else
    {

        if (tCache.nCacheIndex == -1)
            tCache.nCacheIndex = 0;
    }


    tCache.bCacheIsValid = FALSE;
}




void AnimationChannelInit(AnimChannel & tChannel, const int32 nBoneCount, const BoneStructure * ptBoneStructure, const float flTranslationScale, SNO snoAppearance)
{







    SigmaMemoryClear(&tChannel, sizeof(AnimChannel));
    tChannel.flTranslationScale = flTranslationScale;
    tChannel.bPaused = FALSE;

    tChannel.Init(nBoneCount, snoAppearance);
}




void AnimationChannelDispose(AnimChannel & tChannel)
{

    tChannel.Dispose();
}



















void AnimationChannelPlay(RActor * ptRActor, const AnimChannelPlayParams & tChannelPlayParams, const BOOL bDisableBlending)
{



    if (ptRActor->ptActorAnim->ptAnimTree)
        return;

    AnimChannel & tChannel = ptRActor->ptActorAnim->tAnimChannel;
    AnimBoneCache & tCache = ptRActor->ptActorAnim->tAnimBoneCache;

    if (tChannelPlayParams.snoAnim == SNO_NONE)
        return;








    AnimPermID tPermIDPrev = tChannel.tAnimPermID;


    const AnimationDefinition * pdefAnim = (const AnimationDefinition *)gptGroupDefAnimation->LockR(tChannelPlayParams.snoAnim, FALSE);
    BOOL bSwitchingAnimations;



    tChannel.ePlaybackMode = tChannelPlayParams.ePlaybackMode;

    tChannel.tAnimPermID.snoAnim = tChannelPlayParams.snoAnim;
    tChannel.tAnimPermID.nPermutationIndex = tChannelPlayParams.nPermutationIndex;
    tChannel.fNeedsInterpolation = TRUE;

    const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[tChannelPlayParams.nPermutationIndex];
    tChannel.flFrameRate = tAnimPerm.flFrameRate * 60.0f;

    tChannel.flTLast = -1.0f; tChannel.flT = 0.0f;
    tChannel.flTMax = (float)(tAnimPerm.nKeyframeCount - 1) / tChannel.flFrameRate;
    tChannel.flScaleDeltaT = 1.0f;

    tChannel.flAnimSpeedScale = tChannelPlayParams.flAnimSpeedScale;
    tChannel.flTimeScale = 1.0f;

    tChannel.nFrameCount = tAnimPerm.nKeyframeCount;

    tChannel.bDisableRetroFiltering = AnimCanRagdollFromPlayParams(tChannelPlayParams, pdefAnim);


    if (tChannel.arBoneStates)
    {
        SigmaMemoryClear(tChannel.arBoneStates, tChannel.nBoneStateCount * sizeof(BoneState));
    }

    gptGroupDefAnimation->UnlockR((void *&)pdefAnim);



    if (bDisableBlending || tChannel.ShouldApplyRetroFilter())
    {
        tChannel.flBlendTime = 0.0f;
        bSwitchingAnimations = TRUE;
    }
    else
    {
        const AnimPermID & tPermID = tChannel.tAnimPermID;
        const AnimationDefinition * _pdefAnim = (const AnimationDefinition *)gptGroupDefAnimation->LockR(tPermID.snoAnim, FALSE);
        const AnimPermutation & _tAnimPerm = _pdefAnim->ptPermutations[tPermID.nPermutationIndex];

        if (tPermID.snoAnim == tPermIDPrev.snoAnim)
        {
            bSwitchingAnimations = FALSE;

            if (tPermIDPrev.nPermutationIndex == tPermID.nPermutationIndex)
                tChannel.flBlendTime = 0.0f;
            else






                tChannel.flBlendTime = (float)_tAnimPerm.dwFromPermBlendTime * (1.0f / 60.0f);
        }
        else
        {
            bSwitchingAnimations = TRUE;


            tChannel.flBlendTime = (float)_tAnimPerm.dwBlendTime * (1.0f / 60.0f);
        }




        tChannel.flBlendTime = Math::Min(tChannel.flBlendTime, tChannel.flTMax);

        gptGroupDefAnimation->UnlockR((void *&)_pdefAnim);
    }
    tChannel.flBlendT = 0.0f;

    if (bSwitchingAnimations)
    {
        RActorSendMsgKey(ptRActor->id, KeyedMessage(MSGKEY_RESERVED_LEAVING_ANIMATION, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_SELF_AND_CHILDREN);
    }

    if (tCache.nCacheIndex == -1)
    {

        tChannel.flBlendTime = 0.0f;
    }



    BOOL fBlend = TRUE;

    AnimationBoneCacheBump(tCache, fBlend);
}





void AnimationChannelSetPercentComplete(AnimChannel & tChannel, float flFraction)
{


    flFraction = Math::Pin(flFraction, 0.0f, 1.0f);
    tChannel.flT = flFraction * tChannel.flTMax;
}



static BOOL sAnimChannelIsAnimating(const AnimChannel & tChannel)
{




    if ((!tChannel.IsStopped() && !tChannel.bPaused) || tChannel.fNeedsInterpolation) return TRUE; return FALSE;
}





BOOL AnimationChannelIsAnimating(const AnimChannel & tChannel, const AnimBoneCache & tCache)
{



    if (tCache.nCacheIndex == -1)
        return FALSE;




    float flGlobalSpeedScale = AnimationGetGlobalSpeedScale();
    if (flGlobalSpeedScale < EPSILON)
        return FALSE;



    return sAnimChannelIsAnimating(tChannel);
}




BOOL AnimationChannelNeedsInterpolation(const AnimChannel & tChannel)
{

    return tChannel.fNeedsInterpolation != FALSE;
}









void AnimationChannelGetAvgVelocityPerTicks(const AnimChannel & tChannel, const AnimBoneCache & tCache, const float flTranslationScale, Vector3D & wvVel)
{




    if (!AnimationChannelIsAnimating(tChannel, tCache))
    {
        wvVel = cgwvNull;
        return;
    }

    ScopedAnimationDefinitionLockR pdefAnim(tChannel.tAnimPermID.snoAnim);
    AnimationGetAvgVelocity(pdefAnim, tChannel.tAnimPermID.nPermutationIndex, flTranslationScale, FALSE, wvVel);


}






void AnimationChannelGetNonlinearOffset(AnimChannel & tChannel, AnimBoneCache & tCache, Vector3D & wvOffset)
{


    const AnimPermID & tAnimPermID = tChannel.tAnimPermID;
    Vector3D & wvOffsetToCache = tCache.tBoneCache[tCache.nCacheIndex].wvNonlinearOffset;

    if (!AnimationChannelIsAnimating(tChannel, tCache))
    {
        wvOffsetToCache = cgwvNull;
        wvOffset = cgwvNull;
    }


    if (tAnimPermID.snoAnim == SNO_NONE)
    {
        wvOffsetToCache = cgwvNull;
    }
    else
    {
        int32 nNextFrame;
        int32 nFrame;
        ScopedAnimationDefinitionLockR pdefAnim(tAnimPermID.snoAnim);
        const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[tAnimPermID.nPermutationIndex];

        nFrame = tChannel.GetCurrentFrame();
        nNextFrame = tChannel.GetNextFrame();

        if (nNextFrame != nFrame)
        {
            float flTFrame = tChannel.flT * tChannel.flFrameRate - (float)nFrame;


            Math::Lerp(tAnimPerm.pwvNonlinearOffset[nFrame], tAnimPerm.pwvNonlinearOffset[nNextFrame], flTFrame, wvOffsetToCache);
        }
        else
        {

            wvOffsetToCache = tAnimPerm.pwvNonlinearOffset[nFrame];
        }
        wvOffsetToCache *= tChannel.flTranslationScale;
    }

    if (tChannel.flBlendTime > EPSILON && tCache.nPrevCacheIndex != -1)
    {

        Math::Lerp(wvOffsetToCache, tCache.tBoneCache[tCache.nPrevCacheIndex].wvNonlinearOffset, tChannel.flBlendT, wvOffset);
    }
    else
    {
        wvOffset = wvOffsetToCache;
    }
}




void AnimationChannelSetSpeedScale(AnimChannel & tChannel, const float flScale)
{


    tChannel.flAnimSpeedScale = flScale;
}





















float AnimationGetAnimGlobalsSpeedScale()
{

    AnimationGlobals * ptAnimGlobals = CGameGet().ptAnimGlobals;
    if (ptAnimGlobals == NULL)
        return 1.0f;



    return ptAnimGlobals->flGlobalSpeedScale;
}






void AnimationChannelScaleToVelocity(AnimChannel & tChannel, const WorldDistance wdMag)
{



    const AnimPermID & tAnimPermID = tChannel.tAnimPermID;
    Vector3D wvVel;
    if (tAnimPermID.snoAnim == SNO_NONE)
    {
        wvVel = cgwvNull;
    }
    else
    {
        ScopedAnimationDefinitionLockR pdefAnim(tAnimPermID.snoAnim);
        AnimationGetAvgVelocity(pdefAnim, tAnimPermID.nPermutationIndex, tChannel.flTranslationScale, FALSE, wvVel);
    }

    WorldDistance wdAnimMag = wvVel.Magnitude();

    if (wdAnimMag > EPSILON)
    {
        float flScale = wdMag / wdAnimMag;

        tChannel.flAnimSpeedScale = flScale;
    }
}





void AnimationChannelUpdate(AnimChannel & tChannel, AnimBoneCache & tCache, float flDeltaTime)
{



    if (tCache.nCacheIndex == -1 ||
        tChannel.bPaused)
        return;




    if (tChannel.flScaleDeltaT > 0.0f)
    {
        float flGlobalSpeedScale = AnimationGetGlobalSpeedScale();
        float flAnimSpeedScale = tChannel.flTimeScale * tChannel.flAnimSpeedScale;
        tChannel.flScaleDeltaT = flGlobalSpeedScale * flAnimSpeedScale;
    }

    BOOL bHitEnd = FALSE;
    if (!tChannel.fDirectlyControlled)
    {

        bHitEnd = tChannel.Advance(flDeltaTime);
    }


    if (bHitEnd && !tChannel.bFullRagdoll)
    {
        AnimPermID & tPermID = tChannel.tAnimPermID;
        ScopedAnimationDefinitionLockR pdefAnim(tPermID.snoAnim);

        RNG & rng = CGameGlobalsGet().rngLocal;

        int32 nLastPermutationIndex = tPermID.nPermutationIndex;
        tPermID.nPermutationIndex = AnimationSelectPermutation(pdefAnim, rng);

        const AnimPermutation * ptAnimPerm = &pdefAnim->ptPermutations[tPermID.nPermutationIndex];

        tChannel.flTMax = (float)(ptAnimPerm->nKeyframeCount - 1) / tChannel.flFrameRate;
        tChannel.nFrameCount = ptAnimPerm->nKeyframeCount;

        BOOL fBlend = TRUE;
        if (nLastPermutationIndex != tPermID.nPermutationIndex)
        {

            tChannel.flTLast = -1.0f;
            tChannel.flT = 0.0f;


            tChannel.flBlendTime = (float)ptAnimPerm->dwFromPermBlendTime * (1.0f / 60.0f);
            tChannel.flBlendT = 0.0f;
        }




        if (tChannel.arBoneStates)
        {
            SigmaMemoryClear(tChannel.arBoneStates, tChannel.nBoneStateCount * sizeof(BoneState));
        }




        AnimationBoneCacheBump(tCache, fBlend);
    }

    if (tChannel.flBlendTime > EPSILON)
    {
        float flGlobalSpeedScale = AnimationGetGlobalSpeedScale();
        float flAnimSpeedScale = tChannel.flTimeScale * tChannel.flAnimSpeedScale;

        tChannel.flBlendT += flGlobalSpeedScale * flAnimSpeedScale / tChannel.flBlendTime * flDeltaTime;
        if (tChannel.flBlendT >= 1.0f)
        {


            tChannel.flBlendTime = 0.0f; tChannel.flBlendT = 0.0f;
        }
    }
}





static uint32 sReadInteger(
    uint8 * pCurrentData,
    uint32 dwSizeOf)
{
    uint32 dwValue;
    if (dwSizeOf == 1)
    {
        dwValue = *pCurrentData;
    }
    else if (dwSizeOf == 2)
    {
        dwValue = *(uint16 *)pCurrentData;
    }
    else
    {
        dwValue = *(uint32 *)pCurrentData;
    }

    return dwValue;
}

// sInitializeTrans/Rot/Scale and sGetTransKeyData/ScaleKeyData are external and not declared inline: the DWARF keeps
// their gc-removed out-of-line copies (low_pc 0; 0xdc, 0x68, 0x90, 0xcc, 0x4c bytes, the sizes ours compile to).
void sInitializeTrans(
    BoneState * ptBoneState,
    const AnimPermutation & tAnimPerm,
    const TranslationCurve * ptTranslationCurve)
{
    uint8* pCurrentData = (uint8*)ptTranslationCurve->ptKeysComp;
    uint32 dwSizeOfFrameCount = tAnimPerm.nKeyframeCount < 256 ? 1 : (tAnimPerm.nKeyframeCount < 65536 ? 2 : 4);
    ptBoneState->dwSizeOfFrameCount = dwSizeOfFrameCount;

    if (pCurrentData == NULL)
        return;



    ptBoneState->nTransKeyCount = sReadInteger(pCurrentData, dwSizeOfFrameCount); pCurrentData += dwSizeOfFrameCount;
    ptBoneState->dwSizeOfTrans = *pCurrentData++;


    if (ptBoneState->nTransKeyCount > 1)
    {
        pCurrentData += Math::Align(dwSizeOfFrameCount + sizeof(uint8), sizeof(uint32)) - (dwSizeOfFrameCount + sizeof(uint8));  // offset written as dwSizeOfFrameCount + 1 byte: the original adds Align(n + 1, 4) - (n + 1) without reloading ptKeysComp; columns 89 / 93 fit sizeof(uint32) / dwSizeOfFrameCount

        ptBoneState->wpBase = *(Vector3D*)pCurrentData; pCurrentData += sizeof(Vector3D);
    }
    else
    {
        pCurrentData += Math::Align(dwSizeOfFrameCount + sizeof(uint8), dwSizeOfFrameCount) - (dwSizeOfFrameCount + sizeof(uint8));

        ptBoneState->wpBase.Set(0.0f, 0.0f, 0.0f);
    }

    ptBoneState->dwFirstTransKeyFrameOffset = (uint32)(pCurrentData - ptTranslationCurve->ptKeysComp);
    ptBoneState->dwFirstTransKeyDataOffset = Math::Align(ptBoneState->dwFirstTransKeyFrameOffset + ptBoneState->nTransKeyCount * dwSizeOfFrameCount, ptBoneState->dwSizeOfTrans);
}




void sInitializeRot(
    BoneState * ptBoneState,
    const AnimPermutation & tAnimPerm,
    const RotationCurve * ptRotCurve)
{
    uint8* pCurrentData = (uint8*)ptRotCurve->ptKeysComp;
    uint32 dwSizeOfFrameCount = tAnimPerm.nKeyframeCount < 256 ? 1 : (tAnimPerm.nKeyframeCount < 65536 ? 2 : 4);
    ptBoneState->dwSizeOfFrameCount = dwSizeOfFrameCount;

    if (pCurrentData == NULL)
        return;



    ptBoneState->nRotKeyCount = sReadInteger(pCurrentData, dwSizeOfFrameCount);
    // Original stores dwSizeOfFrameCount itself here (stp with nRotKeyCount at 1044, no reload of ptKeysComp).
    ptBoneState->dwFirstRotKeyFrameOffset = dwSizeOfFrameCount;
    ptBoneState->dwFirstRotKeyDataOffset = Math::Align(ptBoneState->dwFirstRotKeyFrameOffset + ptBoneState->nRotKeyCount * dwSizeOfFrameCount, 2);
}




void sInitializeScale(
    BoneState * ptBoneState,
    const AnimPermutation & tAnimPerm,
    const ScaleCurve * ptScaleCurve)
{
    uint8* pCurrentData = (uint8*)ptScaleCurve->ptKeysComp;
    uint32 dwSizeOfFrameCount = tAnimPerm.nKeyframeCount < 256 ? 1 : (tAnimPerm.nKeyframeCount < 65536 ? 2 : 4);
    ptBoneState->dwSizeOfFrameCount = dwSizeOfFrameCount;

    if (pCurrentData == NULL)
        return;



    ptBoneState->nScaleKeyCount = sReadInteger(pCurrentData, dwSizeOfFrameCount); pCurrentData += dwSizeOfFrameCount;
    ptBoneState->dwSizeOfScale = *pCurrentData++;

    ptBoneState->dwFirstScaleKeyFrameOffset = Math::Align((uint32)(pCurrentData - ptScaleCurve->ptKeysComp), dwSizeOfFrameCount);
    ptBoneState->dwFirstScaleKeyDataOffset = Math::Align(ptBoneState->dwFirstScaleKeyFrameOffset + ptBoneState->nScaleKeyCount * dwSizeOfFrameCount, ptBoneState->dwSizeOfScale);
}




Vector3D sGetTransKeyData(
    BoneState * ptBoneState,
    uint8 * ptTransData,
    int32 nFrame)
{
    uint32 dwSizeOfTrans = ptBoneState->dwSizeOfTrans;
    Vector3D wp;

    if (dwSizeOfTrans == 1)
    {
        wp.x = ptTransData[nFrame * 3] / 127.0f - 1.0f;
        wp.y = ptTransData[nFrame * 3 + 1] / 127.0f - 1.0f;
        wp.z = ptTransData[nFrame * 3 + 2] / 127.0f - 1.0f;
    }
    else if (dwSizeOfTrans == 2)
    {
        uint16* ptTransData16 = (uint16*)ptTransData;
        wp.x = ptTransData16[nFrame * 3] / 32767.0f - 1.0f;
        wp.y = ptTransData16[nFrame * 3 + 1] / 32767.0f - 1.0f;
        wp.z = ptTransData16[nFrame * 3 + 2] / 32767.0f - 1.0f;
    }
    else
    {
        float* ptTransData32 = (float*)ptTransData;
        wp.x = ptTransData32[nFrame * 3];
        wp.y = ptTransData32[nFrame * 3 + 1];
        wp.z = ptTransData32[nFrame * 3 + 2];
    }

    return wp + ptBoneState->wpBase;
}




float sGetScaleKeyData(
    BoneState * ptBoneState,
    uint8 * ptScaleData,
    int32 nFrame)
{
    uint32 dwSizeOfScale = ptBoneState->dwSizeOfScale;

    if (dwSizeOfScale == 1)
    {
        return ptScaleData[nFrame] / 127.0f;
    }
    else if (dwSizeOfScale == 2)
    {
        uint16* ptScaleData16 = (uint16*)ptScaleData;
        return ptScaleData16[nFrame] / 32767.0f;
    }
    else
    {
        float* ptScaleData32 = (float*)ptScaleData;
        return ptScaleData32[nFrame];
    }
}



// Lines from the original: both nKeyCount tests are on 1150 with case 0 laid out first (switch), stores 1152-1163, switch on 1177.
static BOOL sGetNextFrameIndex(
    uint32 dwSizeOfFrameCount,
    int32 nCurKey,
    int32 nKeyCount,
    int32 nFrameCount,
    uint8 * ptFrameData,
    int32 nFrame,
    int32 & nCurFrameIndex,
    int32 & nNextFrameIndex,
    int32 & nCurFrame,
    int32 & nNextFrame)
{
    switch (nKeyCount) {
    case 0:
        nCurFrameIndex = -1;
        nNextFrameIndex = -1;
        nCurFrame = 0;
        nNextFrame = nFrameCount - 1;
        return FALSE;


    case 1:
        nCurFrameIndex = 0;
        nNextFrameIndex = 0;
        nCurFrame = 0;
        nNextFrame = nFrameCount - 1;
        return TRUE;
    }
    // nTransKeyCount = nKeyCount - 1 is the loop bound (w10 from the subs at 1150); nKey1 starts at nCurKey (csel at 1179).
    int32 nKey1 = nCurKey;
    int32 nTransKeyCount = nKeyCount - 1;








    switch (dwSizeOfFrameCount) {
    case 1: {
        if (ptFrameData[nKey1] > nFrame)
        {
            nKey1 = 0;
        }

        int32 nKey2 = nKey1;
        while (nKey2 + 1 < nTransKeyCount && ptFrameData[nKey2 + 1] < nFrame)
        {
            ++nKey2;
        }

        nCurFrameIndex = nKey2;
        nNextFrameIndex = nKey2 + 1;
        nCurFrame = ptFrameData[nKey2];
        nNextFrame = ptFrameData[nKey2 + 1];
        return TRUE;
    }
    case 2: {
        uint16 * ptFrameData16 = (uint16 *)ptFrameData;
        if (ptFrameData16[nKey1] > nFrame)
        {
            nKey1 = 0;
        }

        int32 nKey2 = nKey1;
        while (nKey2 + 1 < nTransKeyCount && ptFrameData16[nKey2 + 1] < nFrame)
        {
            ++nKey2;
        }

        nCurFrameIndex = nKey2;
        nNextFrameIndex = nKey2 + 1;
        nCurFrame = ptFrameData16[nKey2];
        nNextFrame = ptFrameData16[nKey2 + 1];
        return TRUE;
    }
    default: {
        int32 * ptFrameData32 = (int32 *)ptFrameData;
        if (ptFrameData32[nKey1] > nFrame)
        {
            nKey1 = 0;
        }

        int32 nKey2 = nKey1;
        while (nKey2 + 1 < nTransKeyCount && ptFrameData32[nKey2 + 1] < nFrame)
        {
            ++nKey2;
        }

        nCurFrameIndex = nKey2;
        nNextFrameIndex = nKey2 + 1;
        nCurFrame = ptFrameData32[nKey2];
        nNextFrame = ptFrameData32[nKey2 + 1];
        return TRUE;
    }
    }
}





static void sInterpolateCurve(
    PRSTransform * ptInterpolatedBone,
    BoneState * ptBoneState,
    const AnimPermutation & tAnimPerm,
    uint32 nBoneIndex,
    float flFrame,
    int32 nFrame)
{
    const TranslationCurve * ptTranslationCurve = &tAnimPerm.ptTranslationCurves[nBoneIndex];
    const RotationCurve * ptRotationCurve = &tAnimPerm.ptRotationCurves[nBoneIndex];
    const ScaleCurve * ptScaleCurve = &tAnimPerm.ptScaleCurves[nBoneIndex];

    {

        BOOL fNotInitialized = ptBoneState->nTransSegSpanInv == 0;
        if (fNotInitialized || ptBoneState->nTransSegStart > nFrame || ptBoneState->nTransSegEnd < nFrame)
        {
            if (fNotInitialized)
            {

                sInitializeTrans(ptBoneState, tAnimPerm, ptTranslationCurve);
            }

            int32 nKey1;
            int32 nKey2;
            int32 nCurFrame;
            int32 nNextFrame;
            sGetNextFrameIndex(ptBoneState->dwSizeOfFrameCount, ptBoneState->nTranslationKey, ptBoneState->nTransKeyCount, tAnimPerm.nKeyframeCount, (uint8*)ptTranslationCurve->ptKeysComp + ptBoneState->dwFirstTransKeyFrameOffset, nFrame, nKey1, nKey2, nCurFrame, nNextFrame);

            TranslationKey tKey1;
            tKey1.nFrame = nCurFrame;
            tKey1.wp = sGetTransKeyData(ptBoneState, (uint8*)ptTranslationCurve->ptKeysComp + ptBoneState->dwFirstTransKeyDataOffset, nKey1);

            TranslationKey tKey2;
            tKey2.nFrame = nNextFrame;
            tKey2.wp = sGetTransKeyData(ptBoneState, (uint8*)ptTranslationCurve->ptKeysComp + ptBoneState->dwFirstTransKeyDataOffset, nKey2);

            const TranslationKey * ptKey1 = &tKey1;
            const TranslationKey * ptKey2 = &tKey2;






















            float flSegmentSpan = (float)(ptKey2->nFrame - ptKey1->nFrame);

            ptBoneState->nTranslationKey = nKey1;
            // nTransSegEnd: its store is merged into the stp of nTransSegStart (1306); line 1308 inferred.
            ptBoneState->nTransSegStart = ptKey1->nFrame;
            ptBoneState->flTransSegStart = (float)ptKey1->nFrame;
            ptBoneState->nTransSegEnd = ptKey2->nFrame;
            ptBoneState->flTransSegSpanInv = 1.0f / flSegmentSpan;
            ptBoneState->wpTransKey1 = ptKey1->wp;
            ptBoneState->wpTransKey2 = ptKey2->wp;
        }


        if (ptBoneState->nTransKeyCount == 1)
        {
            ptInterpolatedBone->wp = ptBoneState->wpTransKey1;
        }
        else
        {


            float flAlpha = (flFrame - ptBoneState->flTransSegStart) * ptBoneState->flTransSegSpanInv;
            ptInterpolatedBone->wp = (1.0f - flAlpha) * ptBoneState->wpTransKey1 + flAlpha * ptBoneState->wpTransKey2;
        }
    }

    {

        BOOL fNotInitialized = ptBoneState->nRotSegSpanInv == 0;
        if (fNotInitialized || ptBoneState->nRotSegStart > nFrame || ptBoneState->nRotSegEnd < nFrame)
        {
            if (fNotInitialized)
            {

                sInitializeRot(ptBoneState, tAnimPerm, ptRotationCurve);
            }

            int32 nKey1;
            int32 nKey2;
            int32 nCurFrame;
            int32 nNextFrame;
            BOOL fValidIndex = sGetNextFrameIndex(ptBoneState->dwSizeOfFrameCount, ptBoneState->nRotationKey, ptBoneState->nRotKeyCount, tAnimPerm.nKeyframeCount, (uint8*)ptRotationCurve->ptKeysComp + ptBoneState->dwFirstRotKeyFrameOffset, nFrame, nKey1, nKey2, nCurFrame, nNextFrame);
            static const Quaternion16 ctIdentity = { 0, 0, 0, 32767 };  // w = 32767: the folded constant 0x7fff000000000000

            RotationKey tKey1;
            tKey1.nFrame = nCurFrame;
            tKey1.q16 = fValidIndex ? ((Quaternion16 *)((uint8*)ptRotationCurve->ptKeysComp + ptBoneState->dwFirstRotKeyDataOffset))[nKey1] : ctIdentity;

            RotationKey tKey2;
            tKey2.nFrame = nNextFrame;
            tKey2.q16 = fValidIndex ? ((Quaternion16 *)((uint8*)ptRotationCurve->ptKeysComp + ptBoneState->dwFirstRotKeyDataOffset))[nKey2] : ctIdentity;

            const RotationKey * ptKey1 = &tKey1;
            const RotationKey * ptKey2 = &tKey2;
























            float flSegmentSpan = (float)(ptKey2->nFrame - ptKey1->nFrame);
            // nRotSegEnd: merged into the stp of nRotSegStart (1382); line 1384 inferred.
            ptBoneState->nRotSegStart = ptKey1->nFrame;
            ptBoneState->flRotSegStart = (float)ptKey1->nFrame;
            ptBoneState->nRotSegEnd = ptKey2->nFrame;
            ptBoneState->flRotSegSpanInv = 1.0f / flSegmentSpan;
            ptBoneState->qRotKey1 = ptKey1->q16.Decompress();
            ptBoneState->qRotKey1.Normalize();
            ptBoneState->qRotKey2 = ptKey2->q16.Decompress();
            ptBoneState->qRotKey2.Normalize();
            ptBoneState->nRotationKey = nKey1;
        }


        if (ptBoneState->nRotKeyCount == 1)
        {
            ptInterpolatedBone->q = ptBoneState->qRotKey1;
        }
        else
        {

            float flAlpha = (flFrame - ptBoneState->flRotSegStart) * ptBoneState->flRotSegSpanInv;
            ptInterpolatedBone->q = (1.0f - flAlpha) * ptBoneState->qRotKey1 + flAlpha * ptBoneState->qRotKey2;
            ptInterpolatedBone->q.Normalize();
        }
    }

    {

        BOOL fNotInitialized = ptBoneState->nScaleSegSpanInv == 0;
        if (fNotInitialized || ptBoneState->nScaleSegStart > nFrame || ptBoneState->nScaleSegEnd < nFrame)
        {
            if (fNotInitialized)
            {

                sInitializeScale(ptBoneState, tAnimPerm, ptScaleCurve);
            }

            int32 nKey1;
            int32 nKey2;
            int32 nCurFrame;
            int32 nNextFrame;
            BOOL fValidIndex = sGetNextFrameIndex(ptBoneState->dwSizeOfFrameCount, ptBoneState->nScaleKey, ptBoneState->nScaleKeyCount, tAnimPerm.nKeyframeCount, (uint8*)ptScaleCurve->ptKeysComp + ptBoneState->dwFirstScaleKeyFrameOffset, nFrame, nKey1, nKey2, nCurFrame, nNextFrame);

            ScaleKey tKey1;
            tKey1.nFrame = nCurFrame;
            tKey1.flScale = fValidIndex ? sGetScaleKeyData(ptBoneState, (uint8*)ptScaleCurve->ptKeysComp + ptBoneState->dwFirstScaleKeyDataOffset, nKey1) : 1.0f;

            ScaleKey tKey2;
            tKey2.nFrame = nNextFrame;
            tKey2.flScale = fValidIndex ? sGetScaleKeyData(ptBoneState, (uint8*)ptScaleCurve->ptKeysComp + ptBoneState->dwFirstScaleKeyDataOffset, nKey2) : 1.0f;

            const ScaleKey * ptKey1 = &tKey1;
            const ScaleKey * ptKey2 = &tKey2;























            float flSegmentSpan = (float)(ptKey2->nFrame - ptKey1->nFrame);
            // nScaleSegEnd: merged into the stp of nScaleSegStart (1459); line 1461 inferred.
            ptBoneState->nScaleSegStart = ptKey1->nFrame;
            ptBoneState->flScaleSegStart = (float)ptKey1->nFrame;
            ptBoneState->nScaleSegEnd = ptKey2->nFrame;
            ptBoneState->flScaleSegSpanInv = 1.0f / flSegmentSpan;
            ptBoneState->flScaleKey1 = ptKey1->flScale;
            ptBoneState->flScaleKey2 = ptKey2->flScale;
            ptBoneState->nScaleKey = nKey1;
        }


        if (ptBoneState->nScaleKeyCount == 1)
        {
            ptInterpolatedBone->flScale = ptBoneState->flScaleKey1;
        }
        else
        {

            float flAlpha = (flFrame - ptBoneState->flScaleSegStart) * ptBoneState->flScaleSegSpanInv;
            ptInterpolatedBone->flScale = (1.0f - flAlpha) * ptBoneState->flScaleKey1 + flAlpha * ptBoneState->flScaleKey2;
            // The store above is dead (overwritten here); the original has none at 1477.
            ptInterpolatedBone->flScale = Math::Pin(ptInterpolatedBone->flScale, 0.01f, 100.0f);
        }
    }


}




static void sInterpolateAnimInst(
    AnimChannel & tChannel,
    PRSTransform * ptInterpolatedBones)
{
    const SNO snoAnim = tChannel.tAnimPermID.snoAnim;
    const int32 nPermutationIndex = tChannel.tAnimPermID.nPermutationIndex;

    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);


    const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[nPermutationIndex];

    tChannel.BuildAnimToStructureMap(pdefAnim, tAnimPerm, nPermutationIndex);

    tChannel.fNeedsInterpolation = FALSE;

    if (tChannel.arBoneStates == NULL)
    {
        tChannel.arBoneStates = (BoneState *)SigmaMemoryNew(tChannel.nBoneStateCount * sizeof(BoneState), 4, NULL, FALSE);
        SigmaMemoryClear(tChannel.arBoneStates, tChannel.nBoneStateCount * sizeof(BoneState));
    }





    if (!tAnimPerm.ptTranslationCurves ||
        !tAnimPerm.ptRotationCurves ||
        !tAnimPerm.ptScaleCurves)
    {
        CRefString sMissingCurve;
        if (!tAnimPerm.ptTranslationCurves)
            sMissingCurve = "translation";
        else if (!tAnimPerm.ptRotationCurves)
            sMissingCurve = "rotation";
        else sMissingCurve = "scale";
    }
























    int32 nCurrentFrame = tChannel.GetCurrentFrame();
    float flFrame;

    if (tChannel.ShouldApplyRetroFilter())
    {
        flFrame = (float)nCurrentFrame;
    }
    else
    {
        flFrame = tChannel.flT * tChannel.flFrameRate;
        if (Math::FloatTrunc(flFrame) < flFrame)
            nCurrentFrame++;
    }



    if (tChannel.fOneToOne)
    {
        int32 nCount = Math::Min(tAnimPerm.nBoneCount, tChannel.nBoneStateCount);
        for (int32 nBone = 0; nBone < nCount; ++nBone)
        {
            sInterpolateCurve(ptInterpolatedBones + nBone, tChannel.arBoneStates + nBone, tAnimPerm, nBone, flFrame, nCurrentFrame);
        }
    }
    else
    {
        for (int32 nBone = 0; nBone < tAnimPerm.nBoneCount; ++nBone)
        {

            int32 nStructureBoneIndex = tChannel.pnChannelAnimToStructure[nBone];
            if (nStructureBoneIndex != -1 && nStructureBoneIndex < tChannel.nBoneStateCount)
            {
                sInterpolateCurve(ptInterpolatedBones + nStructureBoneIndex, tChannel.arBoneStates + nStructureBoneIndex, tAnimPerm, nBone, flFrame, nCurrentFrame);
            }
        }
    }



}




static void sExtractBoneBlendWeights(
    AnimChannel & tChannel,
    AnimBoneWeightSet eWeightSet,
    float * ptBoneBlendWeights)
{


    const SNO snoAnim = tChannel.tAnimPermID.snoAnim;
    const int32 nPermutationIndex = tChannel.tAnimPermID.nPermutationIndex;

    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);


    const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[nPermutationIndex];

    tChannel.BuildAnimToStructureMap(pdefAnim, tAnimPerm, nPermutationIndex);

    for (int32 nBone = 0; nBone < tAnimPerm.nBoneCount; ++nBone)
    {
        int32 nStructureBoneIndex = tChannel.pnChannelAnimToStructure[nBone];
        if (nStructureBoneIndex != -1 && nStructureBoneIndex < tChannel.nBoneStateCount)
        {

            ptBoneBlendWeights[nStructureBoneIndex] = tAnimPerm.ptBoneBlendWeights ? tAnimPerm.ptBoneBlendWeights[nBone].aflWeight[eWeightSet] : 1.0f;
        }
    }



}






void AnimationChannelInterpolateBones(
    AnimChannel & tChannel,
    AnimBoneCache & tCache,
    int32 nBoneCount,
    PRSTransform * & ptInterpolatedBones)
{
    if (ptInterpolatedBones == NULL)
    {
        ptInterpolatedBones = tCache.tBoneCache[tCache.nCacheIndex].ptBone;
        tCache.bCacheIsValid = TRUE;
    }

    sInterpolateAnimInst(tChannel, ptInterpolatedBones);









    if (tChannel.flBlendTime > 0.000001f && tCache.nPrevCacheIndex != -1)
    {
        PRSTransform * ptCachedKeyframe = tCache.tBoneCache[tCache.nPrevCacheIndex].ptBone;
        for (int32 i = 0; i < nBoneCount; ++i)
        {
            if (tChannel.flBlendT == 0.0f)
                ptInterpolatedBones[i] = ptCachedKeyframe[i];
            else
                PRSTransform::Lerp(ptCachedKeyframe[i], ptInterpolatedBones[i], tChannel.flBlendT, ptInterpolatedBones[i]);
        }
    }
}










void sAnimTreeLeafUpdateInit(AnimTreeLeafUpdate & tLeafUpdate, AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLeafIndex, const AnimChannelPlayParams & tChannelPlayParams)
{





    tLeafUpdate.bActive = FALSE;
    tLeafUpdate.flInvCycleTime = 0.0f;
    tLeafUpdate.bHitEnd = FALSE;


    SNO snoAnim = tChannelPlayParams.snoAnim;
    int32 nPermutationIndex = tChannelPlayParams.nPermutationIndex;

    const AnimationDefinition * pdefAnim = (const AnimationDefinition *)gptGroupDefAnimation->LockR(snoAnim, FALSE);
    const AnimPermutation * ptAnimPerm = &pdefAnim->ptPermutations[nPermutationIndex];

    const float flCycleTime = (float)(ptAnimPerm->nKeyframeCount - 1) / (ptAnimPerm->flFrameRate * 60.0f) / tChannelPlayParams.flAnimSpeedScale;
    tTree.ptLeafState[dwLeafIndex].tUpdate.flInvCycleTime = 1.0f / flCycleTime;

    if (tTreeDefinition.dwNumSyncGroups)
    {
        int nSyncGroupIndex = tTreeDefinition.ptLeaf[dwLeafIndex].nSyncGroupIndex;
        if (nSyncGroupIndex != -1)
        {

            tTree.ptSyncGroup[nSyncGroupIndex].flCycleTimeScale = 1.0f;
            tTree.ptSyncGroup[nSyncGroupIndex].flCycleTime = flCycleTime;
            tTree.ptSyncGroup[nSyncGroupIndex].flAnimSpeedScale = tTree.ptChannels[dwLeafIndex].flAnimSpeedScale;
            tTree.ptSyncGroup[nSyncGroupIndex].flFrameRate = ptAnimPerm->flFrameRate * 60.0f;
            tTree.ptSyncGroup[nSyncGroupIndex].flT = 0.0f;
            tTree.ptSyncGroup[nSyncGroupIndex].flTMax = tTree.ptChannels[dwLeafIndex].flTMax;
            tTree.ptSyncGroup[nSyncGroupIndex].ePlaybackMode = tTree.ptChannels[dwLeafIndex].ePlaybackMode;
            tTree.ptSyncGroup[nSyncGroupIndex].eUpdateMode = tTreeDefinition.ptLeaf[dwLeafIndex].eUpdateMode;


            tTree.ptChannels[dwLeafIndex].SetDirectlyControlled(TRUE);
        }
    }

    gptGroupDefAnimation->UnlockR((void *&)pdefAnim);
}




inline void sAnimTreeNodeUpdateInitBlend(AnimTreeNodeUpdate & tNodeUpdate, const AnimTreeBlendRamp & tBlendRamp, float flDurationSec, PlaybackMode ePlaybackMode, BOOL bManualRampOut)
{





    tNodeUpdate.flTSec = 0.0f;
    tNodeUpdate.flDurationSec = flDurationSec;
    tNodeUpdate.flBlend = tBlendRamp.flBlendStart;
    tNodeUpdate.ePlaybackMode = ePlaybackMode;
    tNodeUpdate.bManualRampOut = bManualRampOut;
}




void sAnimTreeNodeUpdateAdvance(
    AnimTreeNodeUpdate & tNodeUpdate,
    const AnimTreeBlendRamp & tBlendRamp,
    float flGlobalSpeedScale,
    float flDeltaTime)
{
    tNodeUpdate.flTSec = Math::Pin(tNodeUpdate.flTSec + flGlobalSpeedScale * flDeltaTime, 0.0f, tNodeUpdate.flDurationSec);

    float flDurationSec = tNodeUpdate.flDurationSec;
    float flRampInTimeSec = Math::Min(tBlendRamp.flRampInTimeSec, flDurationSec);
    float flRampOutTimeSec = Math::Min(tBlendRamp.flRampOutTimeSec, flDurationSec);

    if (tNodeUpdate.flTSec < flRampInTimeSec)
    {
        float flLerp = tNodeUpdate.flTSec / flRampInTimeSec;
        tNodeUpdate.flBlend = Math::Lerp(tBlendRamp.flBlendStart, tBlendRamp.flBlendMid, flLerp);
    }
    else if (tNodeUpdate.flTSec < flDurationSec - flRampOutTimeSec || tNodeUpdate.ePlaybackMode == PLAYBACKMODE_LOOP || tNodeUpdate.bManualRampOut)
    {
        tNodeUpdate.flBlend = tBlendRamp.flBlendMid;
    }
    else
    {
        if (flRampOutTimeSec > EPSILON)
        {
            float flLerp = (tNodeUpdate.flTSec - flDurationSec + flRampOutTimeSec) / flRampOutTimeSec;
            tNodeUpdate.flBlend = Math::Lerp(tBlendRamp.flBlendMid, tBlendRamp.flBlendEnd, flLerp);
        }
        else
        {
            tNodeUpdate.flBlend = tBlendRamp.flBlendEnd;
        }
    }
}




inline BOOL sAnimTreeNodeUpdateCanBeRampedOut(AnimTreeNodeUpdate & tNodeUpdate, const AnimTreeBlendRamp & tBlendRamp)
{


    float flDurationSec = tNodeUpdate.flDurationSec;
    float flRampInTimeSec = Math::Min(tBlendRamp.flRampInTimeSec, flDurationSec);
    float flRampOutTimeSec = Math::Min(tBlendRamp.flRampOutTimeSec, flDurationSec);

    if (tNodeUpdate.flTSec < flRampInTimeSec)
        return TRUE;


    return tNodeUpdate.flTSec < flDurationSec - flRampOutTimeSec || tNodeUpdate.ePlaybackMode == PLAYBACKMODE_LOOP || tNodeUpdate.bManualRampOut;
}












inline void sAnimTreeNodeUpdateRampOut(AnimTreeNodeUpdate & tNodeUpdate, const AnimTreeBlendRamp & tBlendRamp)
{


    tNodeUpdate.flTSec = tBlendRamp.flRampInTimeSec;
    tNodeUpdate.flDurationSec = tBlendRamp.flRampInTimeSec + tBlendRamp.flRampOutTimeSec;
    tNodeUpdate.flBlend = tBlendRamp.flBlendMid;
    tNodeUpdate.ePlaybackMode = PLAYBACKMODE_NORMAL;
    tNodeUpdate.bManualRampOut = FALSE;
}




inline float sGetBlendCaseFloat(const AnimTreeNode & tNode, const AnimTreeControl & tControl, uint32 nCaseIndex)
{



    if (tNode.tBlendCase[nCaseIndex].eParam == ACP_NONE)
    {
        return tNode.tBlendCase[nCaseIndex].flLiteral;
    }


    return tControl.tParam[tNode.tBlendCase[nCaseIndex].eParam].flValue;
}





int32 sGetBlendCaseInt(
    const AnimTreeNode & tNode,
    const AnimTreeControl & tControl,
    uint32 nCaseIndex)
{
    if (tNode.tBlendCase[nCaseIndex].eParam == ACP_NONE)
    {
        return tNode.tBlendCase[nCaseIndex].nLiteral;
    }


    return tControl.tParam[tNode.tBlendCase[nCaseIndex].eParam].nValue;
}




// NON_MATCHING: 4200 bytes vs original 4232; everything matches except the two dwCase search loops (1974, 2154): the original folds the break into the loop test (csel/ccmp, index recomputed with umaddl), ours keeps two exits with a strength-reduced pointer, as sAnimTreeUpdate / sAnimTreeNodeRecursiveAnyTrueCallback also do in the original. Tried: continue/break, swapped compare, pre-increment.
void sAnimTreeBlendBones(
    uint32 dwBlendFlags,
    uint32 dwRootIndex,
    const AnimTreeDefinition & tTreeDefinition,
    const AnimTreeControl & tControl,
    AnimBoneStack & tBoneStack,
    AnimTree & tTree)
{

    if (dwRootIndex & 0x80000000)
    {
        uint32 dwLeafIndex = dwRootIndex & 0x7fffffff;
        AnimChannel * ptChannel = &tTree.ptChannels[dwLeafIndex];


        if ((tTree.nBoneCount + tBoneStack.dwTop) > tBoneStack.dwSize)
            return;




        PRSTransform * ptBonePose = &tBoneStack.ptTransform[tBoneStack.dwTop];
        float * pflBoneWeight = &tBoneStack.pflWeight[tBoneStack.dwTop];


        const AnimTreeLeaf * ptLeaf = &tTreeDefinition.ptLeaf[dwLeafIndex];
        if (ptLeaf->eNodeType != ANT_POSE)
        {



            for (int nBone = 0; nBone < tTree.nBoneCount; nBone++)
            {
                ptBonePose[nBone] = cgtPRSTransformIdentity;
                pflBoneWeight[nBone] = 1.0f;
            }

            if (dwBlendFlags & (1 << ANIMTREE_BLEND_BONES))
            {



                sInterpolateAnimInst(*ptChannel, ptBonePose);
            }

            if (dwBlendFlags & (1 << ANIMTREE_BLEND_WEIGHTS))
            {

                AnimBoneWeightSet eBoneWeightSet = tTreeDefinition.ptLeaf[dwLeafIndex].eBoneWeightSet;
                if (eBoneWeightSet != ABW_NONE)
                {
                    sExtractBoneBlendWeights(*ptChannel, eBoneWeightSet, pflBoneWeight);
                }
            }
        }
        else
        {

            if (tTree.ptLeafState[dwLeafIndex].tBoneCache.bDirty)
            {



                for (int nBone = 0; nBone < tTree.nBoneCount; nBone++)
                {
                    tTree.ptLeafState[dwLeafIndex].tBoneCache.ptBonePose[nBone] = cgtPRSTransformIdentity;
                    tTree.ptLeafState[dwLeafIndex].tBoneCache.pflBoneWeight[nBone] = 1.0f;
                }

                if (dwBlendFlags & (1 << ANIMTREE_BLEND_BONES))
                {


                    sInterpolateAnimInst(*ptChannel, tTree.ptLeafState[dwLeafIndex].tBoneCache.ptBonePose);
                }

                if (dwBlendFlags & (1 << ANIMTREE_BLEND_WEIGHTS))
                {

                    AnimBoneWeightSet eBoneWeightSet = tTreeDefinition.ptLeaf[dwLeafIndex].eBoneWeightSet;
                    if (eBoneWeightSet != ABW_NONE)
                    {
                        sExtractBoneBlendWeights(*ptChannel, eBoneWeightSet, tTree.ptLeafState[dwLeafIndex].tBoneCache.pflBoneWeight);
                    }
                }

                tTree.ptLeafState[dwLeafIndex].tBoneCache.bDirty = FALSE;
            }


            SigmaMemoryCopy(ptBonePose, tTree.ptLeafState[dwLeafIndex].tBoneCache.ptBonePose, tTree.nBoneCount * sizeof(PRSTransform));
            SigmaMemoryCopy(pflBoneWeight, tTree.ptLeafState[dwLeafIndex].tBoneCache.pflBoneWeight, tTree.nBoneCount * sizeof(float));
        }


        tBoneStack.dwTop += tTree.nBoneCount;
        return;
    }


    const AnimTreeNode * ptNode = &tTreeDefinition.ptNode[dwRootIndex];


    float flBlendFactor = 0.0f;
    int nBlendFactor = 0;
    if (ptNode->eBlendParam != ACP_NONE)
    {
        flBlendFactor = tControl.tParam[ptNode->eBlendParam].flValue;
        nBlendFactor = tControl.tParam[ptNode->eBlendParam].nValue;
    }
    else
    {
        flBlendFactor = tTree.ptNodeUpdate[dwRootIndex].flBlend;
    }


    switch (ptNode->eNodeType)
    {
    case ANT_PIECEWISE_LINEAR_BLEND:
        {

            uint32 dwCase;
            for (dwCase = 0; dwCase < ptNode->dwNumChildren - 2; ++dwCase)
            {
                float flCaseValue = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);





                if (flBlendFactor <= flCaseValue) break;
            }
            float flBlend = 0.0f;
            if (ptNode->dwNumChildren >= 2)
            {
                float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase);
                float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
                flBlend = Math::Pin(flBlend, 0.0f, 1.0f);
            }

            if (flBlend < EPSILON)
            {
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase], tTreeDefinition, tControl, tBoneStack, tTree);
            }
            else if (flBlend > 1.0f - EPSILON)
            {
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tBoneStack, tTree);
            }
            else
            {
                uint32 dwChild0Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase], tTreeDefinition, tControl, tBoneStack, tTree);
                uint32 dwChild1Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tBoneStack, tTree);

                PRSTransform * ptBones0 = &tBoneStack.ptTransform[dwChild0Top];
                float * pflBoneWeights0 = &tBoneStack.pflWeight[dwChild0Top];
                PRSTransform * ptBones1 = &tBoneStack.ptTransform[dwChild1Top];
                float * pflBoneWeights1 = &tBoneStack.pflWeight[dwChild1Top];

                for (int32 i = 0; i < tTree.nBoneCount; ++i)
                {
                    PRSTransform::Lerp(ptBones0[i], ptBones1[i], flBlend, ptBones0[i]);

                    pflBoneWeights0[i] = Math::Lerp(pflBoneWeights0[i], pflBoneWeights1[i], flBlend);
                }

                tBoneStack.dwTop = dwChild1Top;
            }
        }
        break;



    case ANT_BONE_WEIGHTED_BLEND:
        {
            float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, 0);
            float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, 1);
            float flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
            flBlend = Math::Pin(flBlend, 0.0f, 1.0f);

            if (flBlend < EPSILON)
            {
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[0], tTreeDefinition, tControl, tBoneStack, tTree);
            }
            else
            {
                uint32 dwChild0Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[0], tTreeDefinition, tControl, tBoneStack, tTree);
                uint32 dwChild1Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[1], tTreeDefinition, tControl, tBoneStack, tTree);

                PRSTransform * ptBones0 = &tBoneStack.ptTransform[dwChild0Top];
                float * pflBoneWeights0 = &tBoneStack.pflWeight[dwChild0Top];
                PRSTransform * ptBones1 = &tBoneStack.ptTransform[dwChild1Top];
                float * pflBoneWeights1 = &tBoneStack.pflWeight[dwChild1Top];

                for (int32 i = 0; i < tTree.nBoneCount; ++i)
                {
                    PRSTransform::Lerp(ptBones0[i], ptBones1[i], flBlend * pflBoneWeights1[i], ptBones0[i]);

                    pflBoneWeights0[i] = flBlend * pflBoneWeights1[i];
                }

                tBoneStack.dwTop = dwChild1Top;
            }
        }
        break;



    case ANT_ADDITIVE_BLEND:
        {
            float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, 0);
            float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, 1);
            float flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
            flBlend = Math::Pin(flBlend, 0.0f, 1.0f);

            if (flBlend < EPSILON)
            {
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[0], tTreeDefinition, tControl, tBoneStack, tTree);
            }
            else
            {



                uint32 dwChild0Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[0], tTreeDefinition, tControl, tBoneStack, tTree);
                uint32 dwChild1Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[1], tTreeDefinition, tControl, tBoneStack, tTree);
                uint32 dwChild2Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[2], tTreeDefinition, tControl, tBoneStack, tTree);

                PRSTransform * ptBones0 = &tBoneStack.ptTransform[dwChild0Top];
                float * pflBoneWeights0 = &tBoneStack.pflWeight[dwChild0Top];
                PRSTransform * ptBones1 = &tBoneStack.ptTransform[dwChild1Top];
                float * pflBoneWeights1 = &tBoneStack.pflWeight[dwChild1Top];
                PRSTransform * ptBones2 = &tBoneStack.ptTransform[dwChild2Top];


                for (int32 i = 0; i < tTree.nBoneCount; ++i)
                {
                    PRSTransform tRelative = ptBones2[i].Inverse() * ptBones1[i];
                    PRSTransform tConcat = ptBones0[i] * tRelative;

                    PRSTransform::Lerp(ptBones0[i], tConcat, flBlend * pflBoneWeights1[i], ptBones0[i]);

                    pflBoneWeights0[i] = flBlend * pflBoneWeights1[i];
                }

                tBoneStack.dwTop = dwChild1Top;
            }
        }
        break;
    case ANT_SWITCH_BLEND:
        {

            float flBlend = tTree.ptNodeUpdate[dwRootIndex].flBlend;
            uint32 dwChild = tTree.ptNodeUpdate[dwRootIndex].dwChild;
            uint32 dwNextChild = tTree.ptNodeUpdate[dwRootIndex].dwNextChild;
            if (dwChild == dwNextChild)
            {
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwChild], tTreeDefinition, tControl, tBoneStack, tTree);
            }
            else
            {
                uint32 dwChild0Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwChild], tTreeDefinition, tControl, tBoneStack, tTree);
                uint32 dwChild1Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwNextChild], tTreeDefinition, tControl, tBoneStack, tTree);

                PRSTransform * ptBones0 = &tBoneStack.ptTransform[dwChild0Top];
                float * pflBoneWeights0 = &tBoneStack.pflWeight[dwChild0Top];
                PRSTransform * ptBones1 = &tBoneStack.ptTransform[dwChild1Top];
                float * pflBoneWeights1 = &tBoneStack.pflWeight[dwChild1Top];

                for (int32 i = 0; i < tTree.nBoneCount; ++i)
                {
                    PRSTransform::Lerp(ptBones0[i], ptBones1[i], flBlend, ptBones0[i]);

                    pflBoneWeights0[i] = Math::Lerp(pflBoneWeights0[i], pflBoneWeights1[i], flBlend);
                }

                tBoneStack.dwTop = dwChild1Top;
            }
        }
        break;



    case ANT_BONE_WEIGHTS_LINEAR_BLEND:
        {

            uint32 dwChild0Top = tBoneStack.dwTop;
            sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[0], tTreeDefinition, tControl, tBoneStack, tTree);

            float * pflBoneWeights0 = &tBoneStack.pflWeight[dwChild0Top];


            uint32 dwCase;
            for (dwCase = 0; dwCase < ptNode->dwNumChildren - 3; ++dwCase)
            {
                float flCaseValue = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);





                if (flBlendFactor <= flCaseValue) break;
            }
            float flBlend = 0.0f;
            if (ptNode->dwNumChildren - 1 >= 2)
            {
                float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase);
                float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
                flBlend = Math::Pin(flBlend, 0.0f, 1.0f);
            }

            if (flBlend < EPSILON)
            {
                uint32 dwChild1Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tBoneStack, tTree);

                float * pflBoneWeights1 = &tBoneStack.pflWeight[dwChild1Top];

                for (int32 i = 0; i < tTree.nBoneCount; ++i)
                {
                    pflBoneWeights0[i] = pflBoneWeights1[i];
                }

                tBoneStack.dwTop = dwChild1Top;
            }
            else if (flBlend > 1.0f - EPSILON)
            {
                uint32 dwChild2Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase + 2], tTreeDefinition, tControl, tBoneStack, tTree);

                float * pflBoneWeights2 = &tBoneStack.pflWeight[dwChild2Top];

                for (int32 i = 0; i < tTree.nBoneCount; ++i)
                {
                    pflBoneWeights0[i] = pflBoneWeights2[i];
                }

                tBoneStack.dwTop = dwChild2Top;
            }
            else
            {
                uint32 dwChild1Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tBoneStack, tTree);
                uint32 dwChild2Top = tBoneStack.dwTop;
                sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[dwCase + 2], tTreeDefinition, tControl, tBoneStack, tTree);

                float * pflBoneWeights1 = &tBoneStack.pflWeight[dwChild1Top];
                float * pflBoneWeights2 = &tBoneStack.pflWeight[dwChild2Top];

                for (int32 i = 0; i < tTree.nBoneCount; ++i)
                {
                    pflBoneWeights0[i] = Math::Lerp(pflBoneWeights1[i], pflBoneWeights2[i], flBlend);
                }

                tBoneStack.dwTop = dwChild1Top;
            }
        }
        break;


    case ANT_BONE_WEIGHTS_MULTIPLY_BLEND:
        {

            uint32 dwChild0Top = tBoneStack.dwTop;
            sAnimTreeBlendBones(dwBlendFlags, ptNode->pdwChildren[0], tTreeDefinition, tControl, tBoneStack, tTree);

            float * pflBoneWeights0 = &tBoneStack.pflWeight[dwChild0Top];

            uint32 dwChild1Top = tBoneStack.dwTop;
            sAnimTreeBlendBones((1 << ANIMTREE_BLEND_WEIGHTS), ptNode->pdwChildren[1], tTreeDefinition, tControl, tBoneStack, tTree);

            float * pflBoneWeights1 = &tBoneStack.pflWeight[dwChild1Top];

            for (int32 i = 0; i < tTree.nBoneCount; ++i)
            {
                pflBoneWeights0[i] *= pflBoneWeights1[i];
            }

            tBoneStack.dwTop = dwChild1Top;
        }
        break;
    }
}




void sAnimTreeUpdate(uint32 dwRootIndex, const AnimTreeDefinition & tTreeDefinition, const AnimTreeControl & tControl, AnimTree & tTree, float flBlendWeight, int32 nSyncGroup, float flSyncWeight)
{








    if (dwRootIndex & 0x80000000)
    {
        uint32 dwLeafIndex = dwRootIndex & 0x7fffffff;
        int32 nLeafSyncGroupIndex = tTreeDefinition.ptLeaf[dwLeafIndex].nSyncGroupIndex;


        if (nLeafSyncGroupIndex != -1 && nLeafSyncGroupIndex == nSyncGroup)
        {

            tTree.ptSyncGroup[nSyncGroup].flTotalBlendWeight += flSyncWeight;
            tTree.ptSyncGroup[nSyncGroup].flInvCycleTimeBlend += flSyncWeight * tTree.ptLeafState[dwLeafIndex].tUpdate.flInvCycleTime;
        }

        tTree.ptLeafState[dwLeafIndex].tUpdate.bActive = TRUE;
        tTree.ptLeafState[dwLeafIndex].tUpdate.flBlendWeight = flBlendWeight;
        return;
    }


    const AnimTreeNode * ptNode = &tTreeDefinition.ptNode[dwRootIndex];
    int nBlendFactor = 0;
    float flBlendFactor = 0.0f;


    if (ptNode->eBlendParam != ACP_NONE)
    {
        nBlendFactor = tControl.tParam[ptNode->eBlendParam].nValue;
        flBlendFactor = tControl.tParam[ptNode->eBlendParam].flValue;
    }
    else
    {
        flBlendFactor = tTree.ptNodeUpdate[dwRootIndex].flBlend;
    }


    if (ptNode->nSyncGroupIndex != -1)
    {
        nSyncGroup = ptNode->nSyncGroupIndex;
        flSyncWeight = 1.0f;
    }


    switch (ptNode->eNodeType)
    {
    case ANT_PIECEWISE_LINEAR_BLEND:
        {
            uint32 dwCase;
            float flBlend = 0.0f;
            for (dwCase = 0; dwCase < ptNode->dwNumChildren - 2; dwCase++)
            {
                float flCaseValue = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                if (flBlendFactor <= flCaseValue)
                    break;
            }





            if (ptNode->dwNumChildren >= 2)
            {
                float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase);
                float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
                flBlend = Math::Pin(flBlend, 0.0f, 1.0f);
            }



            if (flBlend < EPSILON)
                sAnimTreeUpdate(ptNode->pdwChildren[dwCase], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
            else if (flBlend > 1.0f - EPSILON)
                sAnimTreeUpdate(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
            else
            {


                sAnimTreeUpdate(ptNode->pdwChildren[dwCase], tTreeDefinition, tControl, tTree, 1.0f - flBlend, nSyncGroup, flSyncWeight * (1.0f - flBlend));
                sAnimTreeUpdate(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, flBlend, nSyncGroup, flSyncWeight * flBlend);
            }
        }
        break;
    case ANT_ADDITIVE_BLEND:
        {



            float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, 0);
            float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, 1);
            float flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
            flBlend = Math::Pin(flBlend, 0.0f, 1.0f);

            if (flBlend < EPSILON)
                sAnimTreeUpdate(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
            else
            {
                sAnimTreeUpdate(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, 1.0f - flBlend, nSyncGroup, flSyncWeight * (1.0f - flBlend));
                sAnimTreeUpdate(ptNode->pdwChildren[1], tTreeDefinition, tControl, tTree, flBlend, nSyncGroup, flSyncWeight * flBlend);
            }
        }
        break;
    case ANT_BONE_WEIGHTED_BLEND:
        {





            float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, 0);
            float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, 1);
            float flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
            flBlend = Math::Pin(flBlend, 0.0f, 1.0f);

            if (flBlend < EPSILON)
                sAnimTreeUpdate(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
            else
            {
                sAnimTreeUpdate(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, 1.0f - flBlend, nSyncGroup, flSyncWeight * (1.0f - flBlend));
                sAnimTreeUpdate(ptNode->pdwChildren[1], tTreeDefinition, tControl, tTree, flBlend, nSyncGroup, flSyncWeight * flBlend);
            }
        }
        break;
    case ANT_SWITCH_BLEND:
        {



            float flBlend = tTree.ptNodeUpdate[dwRootIndex].flBlend;
            uint32 dwChild = tTree.ptNodeUpdate[dwRootIndex].dwChild;
            uint32 dwNextChild = tTree.ptNodeUpdate[dwRootIndex].dwNextChild;
            if (dwChild == dwNextChild)
                sAnimTreeUpdate(ptNode->pdwChildren[dwChild], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
            else
            {


                sAnimTreeUpdate(ptNode->pdwChildren[dwChild], tTreeDefinition, tControl, tTree, 1.0f - flBlendFactor, nSyncGroup, flSyncWeight * (1.0f - flBlend));
                sAnimTreeUpdate(ptNode->pdwChildren[dwNextChild], tTreeDefinition, tControl, tTree, flBlendFactor, nSyncGroup, flSyncWeight * flBlend);
            }
        }
        break;
    case ANT_BONE_WEIGHTS_LINEAR_BLEND:
        {




            sAnimTreeUpdate(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
            uint32 dwCase;
            float flBlend = 0.0f;

            for (dwCase = 0; dwCase < ptNode->dwNumChildren - 3; dwCase++)
            {
                float flCaseValue = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                if (flBlendFactor <= flCaseValue)
                    break;
            }





            if (ptNode->dwNumChildren - 1 >= 2)
            {
                float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase);
                float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
                flBlend = Math::Pin(flBlend, 0.0f, 1.0f);
            }

            if (flBlend < EPSILON)
                sAnimTreeUpdate(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, flBlend, nSyncGroup, flSyncWeight);
            else if (flBlend > 1.0f - EPSILON)
                sAnimTreeUpdate(ptNode->pdwChildren[dwCase + 2], tTreeDefinition, tControl, tTree, flBlend, nSyncGroup, flSyncWeight);
            else
            {




                sAnimTreeUpdate(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, flBlend, nSyncGroup, flSyncWeight * (1.0f - flBlend));
                sAnimTreeUpdate(ptNode->pdwChildren[dwCase + 2], tTreeDefinition, tControl, tTree, flBlend, nSyncGroup, flSyncWeight * flBlend);
            }
        }
        break;
    case ANT_BONE_WEIGHTS_MULTIPLY_BLEND:




        sAnimTreeUpdate(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
        sAnimTreeUpdate(ptNode->pdwChildren[1], tTreeDefinition, tControl, tTree, 1.0f, nSyncGroup, flSyncWeight);
        break;
    }

}




void sAnimTreeChannelPlay(RActor * ptRActor, AnimChannel & tChannel, const AnimChannelPlayParams & tChannelPlayParams, const BOOL bDisableBlending)
{




    if (tChannelPlayParams.snoAnim == SNO_NONE)
        return;





    AnimPermID tPermIDPrev = tChannel.tAnimPermID;


    const AnimationDefinition * pdefAnim = (const AnimationDefinition *)gptGroupDefAnimation->LockR(tChannelPlayParams.snoAnim, FALSE);
    BOOL bSwitchingAnimations;



    tChannel.ePlaybackMode = tChannelPlayParams.ePlaybackMode;

    tChannel.tAnimPermID.snoAnim = tChannelPlayParams.snoAnim;
    tChannel.tAnimPermID.nPermutationIndex = tChannelPlayParams.nPermutationIndex;
    tChannel.fNeedsInterpolation = TRUE;

    const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[tChannelPlayParams.nPermutationIndex];
    tChannel.flFrameRate = tAnimPerm.flFrameRate * 60.0f;

    tChannel.flTLast = -1.0f; tChannel.flT = 0.0f;
    tChannel.flTMax = (float)(tAnimPerm.nKeyframeCount - 1) / tChannel.flFrameRate;
    tChannel.flScaleDeltaT = 1.0f;

    tChannel.flAnimSpeedScale = tChannelPlayParams.flAnimSpeedScale;
    tChannel.flTimeScale = 1.0f;

    tChannel.nFrameCount = tAnimPerm.nKeyframeCount;

    tChannel.bDisableRetroFiltering = AnimCanRagdollFromPlayParams(tChannelPlayParams, pdefAnim);


    if (tChannel.arBoneStates)
    {
        SigmaMemoryClear(tChannel.arBoneStates, tChannel.nBoneStateCount * sizeof(BoneState));
    }

    gptGroupDefAnimation->UnlockR((void *&)pdefAnim);



    if (bDisableBlending || tChannel.ShouldApplyRetroFilter())
    {
        tChannel.flBlendTime = 0.0f;
        bSwitchingAnimations = TRUE;
    }
    else
    {
        const AnimPermID & tPermID = tChannel.tAnimPermID;
        const AnimationDefinition * pdefAnim2 = (const AnimationDefinition *)gptGroupDefAnimation->LockR(tPermID.snoAnim, FALSE);
        const AnimPermutation & tAnimPerm2 = pdefAnim2->ptPermutations[tPermID.nPermutationIndex];

        if (tPermID.snoAnim == tPermIDPrev.snoAnim)
        {


            if (tPermIDPrev.nPermutationIndex == tPermID.nPermutationIndex)
            {
                bSwitchingAnimations = FALSE;
                tChannel.flBlendTime = 0.0f;
            }
            else
            {


                tChannel.flBlendTime = (float)tAnimPerm2.dwFromPermBlendTime * (1.0f / 60.0f);
                bSwitchingAnimations = FALSE;
            }
        }
        else
        {

            tChannel.flBlendTime = (float)tAnimPerm2.dwBlendTime * (1.0f / 60.0f);
            bSwitchingAnimations = TRUE;
        }



        tChannel.flBlendTime = Math::Min(tChannel.flBlendTime, tChannel.flTMax);

        gptGroupDefAnimation->UnlockR((void *&)pdefAnim2);
    }
    tChannel.flBlendT = 0.0f;

    if (ptRActor)
    {
        if (bSwitchingAnimations)
        {
            RActorSendMsgKey(ptRActor->id, KeyedMessage(MSGKEY_RESERVED_LEAVING_ANIMATION, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_SELF_AND_CHILDREN);
        }

        AnimationBoneCacheBump(ptRActor->ptActorAnim->tAnimBoneCache, TRUE);
    }
}




void AnimTreeInit(AnimTree & tTree, int32 nBoneCount, const BoneStructure * ptBoneStructure, const float flTranslationScale, SNO snoAppearance, SNO snoAnimSet, SNO snoAnimTreeDefinition, uint32 dwAnimTagDefault, WeaponClass eWeaponClassDefault)
{









    const AnimTreeDefinition * pdefTree = (const AnimTreeDefinition *)gptGroupDefAnimTree->LockRFast(snoAnimTreeDefinition);


    tTree.dwNumChannels = pdefTree->dwNumLeafs;
    tTree.ptChannels = new AnimChannel[pdefTree->dwNumLeafs];
    tTree.dwNumNodeUpdates = pdefTree->dwNumNodes;
    tTree.ptNodeUpdate = new AnimTreeNodeUpdate[pdefTree->dwNumNodes];
    tTree.dwNumLeafStates = pdefTree->dwNumLeafs;
    tTree.ptLeafState = new AnimTreeLeafState[pdefTree->dwNumLeafs];
    tTree.dwNumSyncGroups = pdefTree->dwNumSyncGroups;
    tTree.ptSyncGroup = NULL;
    if (pdefTree->dwNumSyncGroups)
    {
        tTree.ptSyncGroup = new AnimTreeSyncGroup[pdefTree->dwNumSyncGroups];
    }

    for (uint32 dwNodeIndex = 0; dwNodeIndex < pdefTree->dwNumNodes; dwNodeIndex++)
    {
        AnimTreeNode & tNode = pdefTree->ptNode[dwNodeIndex];
        sAnimTreeNodeUpdateInitBlend(tTree.ptNodeUpdate[dwNodeIndex], tNode.tBlendRamp, tNode.tBlendRamp.flDurationSec, PLAYBACKMODE_NORMAL, FALSE);
        tTree.ptNodeUpdate[dwNodeIndex].dwChild = 0;
        tTree.ptNodeUpdate[dwNodeIndex].dwNextChild = 0;
        tTree.ptNodeUpdate[dwNodeIndex].flTSec = tTree.ptNodeUpdate[dwNodeIndex].flDurationSec;
    }

    for (uint32 dwLeafIndex = 0; dwLeafIndex < pdefTree->dwNumLeafs; dwLeafIndex++)
    {
        AnimationChannelInit(tTree.ptChannels[dwLeafIndex], nBoneCount, ptBoneStructure, flTranslationScale, snoAppearance);






        RNG & rng = CGameGlobalsGet().rngLocal;

        SNO snoAnim = pdefTree->ptLeaf[dwLeafIndex].snoAnim;
        if (snoAnim == SNO_NONE)
        {
            snoAnim = AnimSetGetAnim(snoAnimSet, eWeaponClassDefault, pdefTree->ptLeaf[dwLeafIndex].dwTagAnimation);
            if (snoAnim == SNO_NONE)
            {
                snoAnim = AnimSetGetAnim(snoAnimSet, eWeaponClassDefault, dwAnimTagDefault);
            }
        }
        AnimChannelPlayParams tChannelPlayParams;
        ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
        tChannelPlayParams.flAnimSpeedScale = pdefTree->ptLeaf[dwLeafIndex].flPlaySpeed;
        sAnimTreeChannelPlay(NULL, tTree.ptChannels[dwLeafIndex], tChannelPlayParams, TRUE);

        if (pdefTree->ptLeaf[dwLeafIndex].eNodeType == ANT_POSE)
        {
            tTree.ptChannels[dwLeafIndex].flT = Math::Pin(pdefTree->ptLeaf[dwLeafIndex].flPoseFrame, 0.0f, tTree.ptChannels[dwLeafIndex].flTMax);
            tTree.ptChannels[dwLeafIndex].flTLast = tTree.ptChannels[dwLeafIndex].flT;
            tTree.ptChannels[dwLeafIndex].bPaused = TRUE;
        }

        AnimTreeLeafUpdate & tLeafUpdate = tTree.ptLeafState[dwLeafIndex].tUpdate;
        sAnimTreeLeafUpdateInit(tLeafUpdate, tTree, *pdefTree, dwLeafIndex, tChannelPlayParams);

        if (pdefTree->ptLeaf[dwLeafIndex].eNodeType == ANT_POSE)
        {
            tTree.ptLeafState[dwLeafIndex].tBoneCache.bDirty = TRUE;
            tTree.ptLeafState[dwLeafIndex].tBoneCache.ptBonePose = new PRSTransform[nBoneCount];
            tTree.ptLeafState[dwLeafIndex].tBoneCache.pflBoneWeight = new float[nBoneCount];
        }
        else
        {
            tTree.ptLeafState[dwLeafIndex].tBoneCache.bDirty = FALSE;
            tTree.ptLeafState[dwLeafIndex].tBoneCache.ptBonePose = NULL;
            tTree.ptLeafState[dwLeafIndex].tBoneCache.pflBoneWeight = NULL;
        }
    }

    tTree.nBoneCount = nBoneCount;



    tTree.bPaused = FALSE;
}




void AnimTreeDispose(AnimTree & tTree)
{
    for (uint32 dwChannel = 0; dwChannel < tTree.dwNumChannels; dwChannel++)
    {
        AnimationChannelDispose(tTree.ptChannels[dwChannel]);
    }


    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTree.dwNumLeafStates; dwLeafIndex++)
    {
        delete tTree.ptLeafState[dwLeafIndex].tBoneCache.ptBonePose; tTree.ptLeafState[dwLeafIndex].tBoneCache.ptBonePose = NULL;
        delete tTree.ptLeafState[dwLeafIndex].tBoneCache.pflBoneWeight; tTree.ptLeafState[dwLeafIndex].tBoneCache.pflBoneWeight = NULL;
    }

    delete tTree.ptChannels; tTree.ptChannels = NULL;
    delete tTree.ptNodeUpdate; tTree.ptNodeUpdate = NULL;
    delete tTree.ptLeafState; tTree.ptLeafState = NULL;
    delete tTree.ptSyncGroup; tTree.ptSyncGroup = NULL;
}




void AnimTreeUpdate(
    RActor * ptRActor,
    const AnimTreeDefinition & tTreeDefinition,
    const AnimTreeControl & tControl,
    AnimTree & tTree,
    float flDeltaTime)
{

    if (tTree.bPaused)
        return;




    if (tTree.dwNumSyncGroups)
    {
        for (uint32 dwSyncGroupIndex = 0; dwSyncGroupIndex < tTree.dwNumSyncGroups; dwSyncGroupIndex++)
        {
            tTree.ptSyncGroup[dwSyncGroupIndex].flTotalBlendWeight = 0.0f;
            tTree.ptSyncGroup[dwSyncGroupIndex].flInvCycleTimeBlend = 0.0f;
        }
    }


    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTree.dwNumLeafStates; dwLeafIndex++)
    {
        tTree.ptLeafState[dwLeafIndex].tUpdate.bActive = FALSE;
    }


    for (uint32 dwChannelIndex = 0; dwChannelIndex < tTree.dwNumChannels; dwChannelIndex++)
    {
        AnimChannel& tChannel = tTree.ptChannels[dwChannelIndex];

        if (tChannel.bPaused)
            continue;



        if (!tChannel.fDirectlyControlled && tTree.ptLeafState[dwChannelIndex].tUpdate.bHitEnd && !tChannel.bFullRagdoll)
        {
            RNG & rng = CGameGlobalsGet().rngLocal;
            AnimChannelPlayParams tChannelPlayParams;
            SNO snoAnim = tChannel.tAnimPermID.snoAnim;
            ScopedAnimationDefinitionLockR pdefAnim(snoAnim);

            int32 nLastPermutationIndex = tChannel.tAnimPermID.nPermutationIndex;
            int32 nPermutationIndex = AnimationSelectPermutation(pdefAnim, rng);

            if (nPermutationIndex != nLastPermutationIndex)
            {
                AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, nPermutationIndex, tTreeDefinition.ptLeaf[dwChannelIndex].flPlaySpeed);
                sAnimTreeChannelPlay(NULL, tTree.ptChannels[dwChannelIndex], tChannelPlayParams, TRUE);

                if (tTreeDefinition.ptLeaf[dwChannelIndex].eNodeType == ANT_POSE)
                {
                    tTree.ptChannels[dwChannelIndex].flT = Math::Pin(tTreeDefinition.ptLeaf[dwChannelIndex].flPoseFrame, 0.0f, tTree.ptChannels[dwChannelIndex].flTMax);
                    tTree.ptChannels[dwChannelIndex].flTLast = tTree.ptChannels[dwChannelIndex].flT;
                    tTree.ptChannels[dwChannelIndex].bPaused = TRUE;
                }

                BOOL bActive = tTree.ptLeafState[dwChannelIndex].tUpdate.bActive;
                AnimTreeLeafUpdate & tLeafUpdate = tTree.ptLeafState[dwChannelIndex].tUpdate;
                sAnimTreeLeafUpdateInit(tLeafUpdate, tTree, tTreeDefinition, dwChannelIndex, tChannelPlayParams);
                tTree.ptLeafState[dwChannelIndex].tUpdate.bActive = bActive;

                if (tTreeDefinition.ptLeaf[dwChannelIndex].eNodeType == ANT_POSE)
                {
                    tTree.ptLeafState[dwChannelIndex].tBoneCache.bDirty = TRUE;
                }

                AnimationBoneCacheBump(ptRActor->ptActorAnim->tAnimBoneCache, TRUE);
            }
        }
    }

    float flGlobalSpeedScale = AnimationGetGlobalSpeedScale();


    for (uint32 dwNodeIndex = 0; dwNodeIndex < tTree.dwNumNodeUpdates; dwNodeIndex++)
    {
        const AnimTreeNode * ptNode = &tTreeDefinition.ptNode[dwNodeIndex];
        AnimTreeNodeUpdate * ptNodeUpdate = &tTree.ptNodeUpdate[dwNodeIndex];


        int nBlendFactor = 0;
        if (ptNode->eBlendParam != ACP_NONE)
        {
            nBlendFactor = tControl.tParam[ptNode->eBlendParam].nValue;
        }

        switch(ptNode->eNodeType)
        {
        case ANT_SWITCH_BLEND:
            {

                uint32 dwNextChild;
                for (dwNextChild = 0; dwNextChild < ptNode->dwNumChildren; dwNextChild++)
                {
                    int nCaseValue = sGetBlendCaseInt(*ptNode, tControl, dwNextChild);
                    if (nCaseValue == nBlendFactor)
                        break;
                }


                if (dwNextChild == ptNode->dwNumChildren)
                    dwNextChild = 0;




                if (ptNodeUpdate->dwChild != dwNextChild && ptNodeUpdate->dwNextChild != dwNextChild)
                {
                    sAnimTreeNodeUpdateInitBlend(*ptNodeUpdate, ptNode->tBlendRamp, ptNode->tBlendRamp.flDurationSec, PLAYBACKMODE_NORMAL, FALSE);
                }

                ptNodeUpdate->dwNextChild = dwNextChild;
            }
            break;
        }


        sAnimTreeNodeUpdateAdvance(*ptNodeUpdate, ptNode->tBlendRamp, flGlobalSpeedScale, flDeltaTime);


        switch(ptNode->eNodeType)
        {
        case ANT_SWITCH_BLEND:
            {
                if (ptNodeUpdate->flTSec == ptNodeUpdate->flDurationSec)
                {
                    ptNodeUpdate->dwChild = ptNodeUpdate->dwNextChild;
                }
            }
            break;
        }
    }





    sAnimTreeUpdate(tTreeDefinition.dwRootNode, tTreeDefinition, tControl, tTree, 1.0f, -1, 1.0f);


    if (tTree.dwNumSyncGroups)
    {

        for (uint32 dwSyncGroupIndex = 0; dwSyncGroupIndex < tTree.dwNumSyncGroups; dwSyncGroupIndex++)
        {
            AnimTreeSyncGroup * ptSyncGroup = &tTree.ptSyncGroup[dwSyncGroupIndex];
            if (ptSyncGroup->flTotalBlendWeight > EPSILON)
            {
                float flCycleTimeBlend = 1.0f / ptSyncGroup->flInvCycleTimeBlend;
                float flWeightedCycleTime = flCycleTimeBlend / ptSyncGroup->flTotalBlendWeight;
                ptSyncGroup->flCycleTimeScale = ptSyncGroup->flCycleTime / flWeightedCycleTime;

                float flAnimSpeedScale = ptSyncGroup->flCycleTimeScale * ptSyncGroup->flAnimSpeedScale * flGlobalSpeedScale;
                ptSyncGroup->flT += flAnimSpeedScale * flDeltaTime;
                if (ptSyncGroup->flT >= ptSyncGroup->flTMax)
                {
                    switch (ptSyncGroup->ePlaybackMode)
                    {
                    case PLAYBACKMODE_NORMAL:

                        ptSyncGroup->flT = Math::Min(ptSyncGroup->flT, ptSyncGroup->flTMax);
                        break;

                    case PLAYBACKMODE_LOOP:

                        while (ptSyncGroup->flT >= ptSyncGroup->flTMax)
                        {
                            ptSyncGroup->flT -= ptSyncGroup->flTMax;
                        }
                        break;
                    }
                }
            }
            else
            {
                if (ptSyncGroup->eUpdateMode == 1)
                {

                    ptSyncGroup->flT = 0.0f;
                }
            }
        }


        for (uint32 dwChannelIndex = 0; dwChannelIndex < tTree.dwNumChannels; dwChannelIndex++)
        {
            AnimChannel& tChannel = tTree.ptChannels[dwChannelIndex];
            int32 nSyncGroupIndex = tTreeDefinition.ptLeaf[dwChannelIndex].nSyncGroupIndex;
            if (nSyncGroupIndex != -1)
            {
                if (tTree.ptSyncGroup[nSyncGroupIndex].flTotalBlendWeight > EPSILON)
                {
                    tChannel.flTLast = tChannel.flT;
                    float flInterp = tTree.ptSyncGroup[nSyncGroupIndex].flT / tTree.ptSyncGroup[nSyncGroupIndex].flTMax;
                    tChannel.flT = flInterp * tChannel.flTMax;
                    tChannel.flT = Math::Pin(tChannel.flT, 0.0f, tChannel.flTMax);
                }
                else
                {
                    tChannel.flTLast = tChannel.flT;
                    tChannel.flT = 0.0f;
                }
            }
        }
    }


    for (uint32 dwChannelIndex = 0; dwChannelIndex < tTree.dwNumChannels; dwChannelIndex++)
    {
        AnimChannel& tChannel = tTree.ptChannels[dwChannelIndex];

        if (tChannel.bPaused)
            continue;




        if (tChannel.flScaleDeltaT > 0.0f)
        {
            float flGlobalSpeedScale2 = AnimationGetGlobalSpeedScale();
            float flAnimSpeedScale = tChannel.flTimeScale * tChannel.flAnimSpeedScale * flGlobalSpeedScale2;
            tChannel.flScaleDeltaT = flAnimSpeedScale;
        }


        if (!tChannel.fDirectlyControlled)
        {
            BOOL bHitEnd = FALSE;
            switch (tTreeDefinition.ptLeaf[dwChannelIndex].eUpdateMode)
            {
            case 0:
                bHitEnd = tChannel.Advance(flDeltaTime);
                break;
            case 1:
                if (tTree.ptLeafState[dwChannelIndex].tUpdate.bActive)
                {
                    bHitEnd = tChannel.Advance(flDeltaTime);
                }
                else
                {
                    tChannel.flT = 0.0f;
                    tChannel.flTLast = -1.0f;
                    tChannel.flScaleDeltaT = 0.0f;
                }
                break;
            }

            tTree.ptLeafState[dwChannelIndex].tUpdate.bHitEnd = bHitEnd;
        }
    }
}






void AnimTreeBlendBones(const AnimTreeDefinition & tTreeDefinition, const AnimTreeControl & tControl, AnimBoneStack & tBoneStack, AnimTree & tTree)
{






    sAnimTreeBlendBones((1 << ANIMTREE_BLEND_BONES) | (1 << ANIMTREE_BLEND_WEIGHTS), tTreeDefinition.dwRootNode, tTreeDefinition, tControl, tBoneStack, tTree);
}








void AnimTreeLayerPlay(RActor * ptRActor, uint32 dwLayerHash, const AnimChannelPlayParams & tChannelPlayParams, BOOL bImmediatelyActive, BOOL bManualRampOut)
{





    if (dwLayerHash == 0)
        return;



    if (!ptRActor->ptActorAnim || !ptRActor->ptActorAnim->ptAnimTree)
        return;

    AnimTree * ptAnimTree = ptRActor->ptActorAnim->ptAnimTree;


    ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);

    float flDurationSec = -1.0f;
    for (uint32 dwLeafIndex = 0; dwLeafIndex < pdefAnimTree->dwNumLeafs; dwLeafIndex++)
    {
        if (pdefAnimTree->ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            AnimChannelPlayParams tScaledChannelPlayParams = tChannelPlayParams;

            AnimChannel & tAnimChannel = ptAnimTree->ptChannels[dwLeafIndex];
            tScaledChannelPlayParams.flAnimSpeedScale = pdefAnimTree->ptLeaf[dwLeafIndex].flPlaySpeed * tScaledChannelPlayParams.flAnimSpeedScale;
            sAnimTreeChannelPlay(ptRActor, tAnimChannel, tScaledChannelPlayParams, TRUE);

            if (pdefAnimTree->ptLeaf[dwLeafIndex].eNodeType == ANT_POSE)
            {
                ptAnimTree->ptChannels[dwLeafIndex].flT = Math::Pin(pdefAnimTree->ptLeaf[dwLeafIndex].flPoseFrame, 0.0f, ptAnimTree->ptChannels[dwLeafIndex].flTMax);
                ptAnimTree->ptChannels[dwLeafIndex].flTLast = ptAnimTree->ptChannels[dwLeafIndex].flT;
                ptAnimTree->ptChannels[dwLeafIndex].bPaused = TRUE;
            }

            AnimTreeLeafUpdate & tLeafUpdate = ptAnimTree->ptLeafState[dwLeafIndex].tUpdate;
            sAnimTreeLeafUpdateInit(tLeafUpdate, *ptAnimTree, *pdefAnimTree, dwLeafIndex, tScaledChannelPlayParams);
            if (bImmediatelyActive)
            {
                tLeafUpdate.bActive = TRUE;
            }

            if (pdefAnimTree->ptLeaf[dwLeafIndex].eNodeType != ANT_POSE)
            {
                flDurationSec = 1.0f / tLeafUpdate.flInvCycleTime;
            }
            else
            {

                ptAnimTree->ptLeafState[dwLeafIndex].tBoneCache.bDirty = TRUE;
            }
        }
    }

    for (uint32 dwNode = 0; dwNode < pdefAnimTree->dwNumNodes; dwNode++)
    {
        if (pdefAnimTree->ptNode[dwNode].tLayerName.dwHash == dwLayerHash)
        {
            AnimTreeNodeUpdate & tNodeUpdate = ptAnimTree->ptNodeUpdate[dwNode];


            sAnimTreeNodeUpdateInitBlend(tNodeUpdate, pdefAnimTree->ptNode[dwNode].tBlendRamp,



                flDurationSec > 0.0f ? flDurationSec : pdefAnimTree->ptNode[dwNode].tBlendRamp.flDurationSec, tChannelPlayParams.ePlaybackMode, bManualRampOut);
        }
    }
}



























































































void AnimTreeLayerSetPoseFromLayer(RActor * ptRActor, uint32 dwLayerHash, uint32 dwFromLayerHash)
{



    if (dwLayerHash == 0)
        return;



    if (!ptRActor->ptActorAnim || !ptRActor->ptActorAnim->ptAnimTree)
        return;

    AnimTree * ptAnimTree = ptRActor->ptActorAnim->ptAnimTree;


    ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);


    float flT;
    AnimChannelPlayParams tChannelPlayParams;
    tChannelPlayParams.snoAnim = SNO_NONE; tChannelPlayParams.nPermutationIndex = -1;


    for (uint32 dwLeafIndex = 0; dwLeafIndex < pdefAnimTree->dwNumLeafs; dwLeafIndex++)
    {
        if (pdefAnimTree->ptLeaf[dwLeafIndex].tLayerName.dwHash == dwFromLayerHash)
        {
            AnimChannel & tAnimChannel = ptAnimTree->ptChannels[dwLeafIndex];


            tChannelPlayParams.snoAnim = tAnimChannel.tAnimPermID.snoAnim;
            tChannelPlayParams.nPermutationIndex = tAnimChannel.tAnimPermID.nPermutationIndex;
            tChannelPlayParams.flAnimSpeedScale = tAnimChannel.flAnimSpeedScale;
            tChannelPlayParams.ePlaybackMode = tAnimChannel.ePlaybackMode;
            flT = tAnimChannel.flT;
            break;
        }
    }


    if (tChannelPlayParams.snoAnim == SNO_NONE)
        return;




    AnimTreeLayerPlay(ptRActor, dwLayerHash, tChannelPlayParams, TRUE, FALSE);


    for (uint32 dwLeafIndex = 0; dwLeafIndex < pdefAnimTree->dwNumLeafs; dwLeafIndex++)
    {
        if (pdefAnimTree->ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            AnimChannel & tAnimChannel = ptAnimTree->ptChannels[dwLeafIndex];

            if (pdefAnimTree->ptLeaf[dwLeafIndex].eNodeType == ANT_POSE)
            {
                tAnimChannel.flT = flT;
                tAnimChannel.flTLast = flT;
                ptAnimTree->ptLeafState[dwLeafIndex].tBoneCache.bDirty = TRUE;
            }
        }
    }
}





float AnimTreeGetLayerPercentComplete(const AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{



    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            AnimChannel & tAnimChannel = tTree.ptChannels[dwLeafIndex];
            return tAnimChannel.flT / tAnimChannel.flTMax;
        }
    }

    return 1.0f;
}
























SNO AnimTreeLayerGetSNO(const AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{






    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            AnimChannel & tAnimChannel = tTree.ptChannels[dwLeafIndex];
            return tAnimChannel.tAnimPermID.snoAnim;
        }
    }

    return SNO_NONE;
}




























void AnimTreeLayerEnableFullRagdoll(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{



    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            AnimChannel & tAnimChannel = tTree.ptChannels[dwLeafIndex];
            AnimationChannelEnableFullRagdoll(tAnimChannel);
        }
    }
}






void AnimTreeLayerScaleToVelocity(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash, const WorldDistance wdMag)
{




    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash &&

            tTreeDefinition.ptLeaf[dwLeafIndex].nSyncGroupIndex == -1)
        {
            AnimChannel & tAnimChannel = tTree.ptChannels[dwLeafIndex];

            AnimationChannelScaleToVelocity(tAnimChannel, wdMag);

            const float flCycleTime = (float)(tAnimChannel.nFrameCount - 1) / (tAnimChannel.flFrameRate * 60.0f) / tAnimChannel.flAnimSpeedScale;
            tTree.ptLeafState[dwLeafIndex].tUpdate.flInvCycleTime = 1.0f / flCycleTime;
        }
    }
}






BOOL AnimTreeLayerHasManualBlendRampOut(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{



    if (dwLayerHash == 0)
        return FALSE;



    for (uint32 dwNodeIndex = 0; dwNodeIndex < tTreeDefinition.dwNumNodes; dwNodeIndex++)
    {
        if (tTreeDefinition.ptNode[dwNodeIndex].tLayerName.dwHash == dwLayerHash)
        {
            if (tTree.ptNodeUpdate[dwNodeIndex].bManualRampOut)
                return TRUE;
        }
    }
    return FALSE;
}








void AnimTreeLayerRampOutBlend(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{



    if (dwLayerHash == 0)
        return;



    for (uint32 dwNode = 0; dwNode < tTreeDefinition.dwNumNodes; dwNode++)
    {
        if (tTreeDefinition.ptNode[dwNode].tLayerName.dwHash == dwLayerHash)
        {
            AnimTreeNodeUpdate & tNodeUpdate = tTree.ptNodeUpdate[dwNode];
            if (sAnimTreeNodeUpdateCanBeRampedOut(tNodeUpdate, tTreeDefinition.ptNode[dwNode].tBlendRamp))
            {
                sAnimTreeNodeUpdateRampOut(tNodeUpdate, tTreeDefinition.ptNode[dwNode].tBlendRamp);
            }
        }
    }
}




PlaybackMode AnimTreeLayerGetPlaybackMode(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{






    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            return tTree.ptChannels[dwLeafIndex].ePlaybackMode;
        }
    }

    return PLAYBACKMODE_NORMAL;
}





BOOL AnimTreeLayerIsActive(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{



    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            if (tTree.ptLeafState[dwLeafIndex].tUpdate.bActive)
                return TRUE;
        }
    }



    return FALSE;
}








BOOL sAnimTreeNodeIsAnimating(AnimTree & tTree, uint32 dwLeafIndex)
{


    return sAnimChannelIsAnimating(tTree.ptChannels[dwLeafIndex]);
}

BOOL sAnimTreeNodeNeedsInterpolation(AnimTree & tTree, uint32 dwLeafIndex)
{


    return AnimationChannelNeedsInterpolation(tTree.ptChannels[dwLeafIndex]);
}




BOOL sAnimTreeNodeRecursiveAnyTrueCallback(
    uint32 dwRootIndex,
    const AnimTreeDefinition & tTreeDefinition,
    const AnimTreeControl & tControl,
    AnimTree & tTree,
    TreeRecursiveCallback ptCallback)
{

    if (dwRootIndex & 0x80000000)
    {
        uint32 dwLeafIndex = dwRootIndex & 0x7fffffff;

        return ptCallback(tTree, dwLeafIndex);
    }

    const AnimTreeNode * ptNode = &tTreeDefinition.ptNode[dwRootIndex];


    float flBlendFactor = 0.0f;
    int nBlendFactor = 0;
    if (ptNode->eBlendParam != ACP_NONE)
    {
        flBlendFactor = tControl.tParam[ptNode->eBlendParam].flValue;
        nBlendFactor = tControl.tParam[ptNode->eBlendParam].nValue;
    }
    else
    {
        flBlendFactor = tTree.ptNodeUpdate[dwRootIndex].flBlend;
    }


    switch (ptNode->eNodeType)
    {
    case ANT_PIECEWISE_LINEAR_BLEND:
        {

            uint32 dwCase;
            for (dwCase = 0; dwCase < ptNode->dwNumChildren - 2; dwCase++)
            {
                float flCaseValue = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                if (flBlendFactor <= flCaseValue)
                    break;
            }




            float flBlend = 0.0f;
            if (ptNode->dwNumChildren >= 2)
            {
                float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase);
                float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
                flBlend = Math::Pin(flBlend, 0.0f, 1.0f);
            }
            // returns on 3511/3515/3519 placed from the 3513 "else if" (col 22) and the one-line 3519 "||" (cols 24, 62, 178)
            if (flBlend < EPSILON)
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase], tTreeDefinition, tControl, tTree, ptCallback);
            }
            else if (flBlend > 1.0f - EPSILON)
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, ptCallback);
            }
            else
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase], tTreeDefinition, tControl, tTree, ptCallback) || sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, ptCallback);
            }
        }
    case ANT_BONE_WEIGHTED_BLEND:
        {




            float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, 0);
            float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, 1);
            float flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
            flBlend = Math::Pin(flBlend, 0.0f, 1.0f);

            if (flBlend < EPSILON)
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, ptCallback);
            }
            else
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, ptCallback) || sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[1], tTreeDefinition, tControl, tTree, ptCallback);
            }
        }
    case ANT_ADDITIVE_BLEND:
        {




            float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, 0);
            float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, 1);
            float flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
            flBlend = Math::Pin(flBlend, 0.0f, 1.0f);

            if (flBlend < EPSILON)
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, ptCallback);
            }
            else
            {



                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, ptCallback) || sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[1], tTreeDefinition, tControl, tTree, ptCallback) || sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[2], tTreeDefinition, tControl, tTree, ptCallback);
            }
        }
    case ANT_SWITCH_BLEND:
        {


            uint32 dwChild = tTree.ptNodeUpdate[dwRootIndex].dwChild;
            uint32 dwNextChild = tTree.ptNodeUpdate[dwRootIndex].dwNextChild;
            if (dwChild == dwNextChild)
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwChild], tTreeDefinition, tControl, tTree, ptCallback);
            }
            else
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwChild], tTreeDefinition, tControl, tTree, ptCallback) || sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwNextChild], tTreeDefinition, tControl, tTree, ptCallback);
            }
        }
    case ANT_BONE_WEIGHTS_LINEAR_BLEND:
        {





            if (sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, ptCallback))
            {
                return TRUE;
            }


            uint32 dwCase;
            for (dwCase = 0; dwCase < ptNode->dwNumChildren - 3; dwCase++)
            {
                float flCaseValue = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                if (flBlendFactor <= flCaseValue)
                    break;
            }




            float flBlend = 0.0f;
            if ((ptNode->dwNumChildren - 1) >= 2)
            {
                float flCase0Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase);
                float flCase1Value = sGetBlendCaseFloat(*ptNode, tControl, dwCase + 1);
                flBlend = (flBlendFactor - flCase0Value) / (flCase1Value - flCase0Value);
                flBlend = Math::Pin(flBlend, 0.0f, 1.0f);
            }

            if (flBlend < EPSILON)
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, ptCallback);
            }
            else if (flBlend > 1.0f - EPSILON)
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase + 2], tTreeDefinition, tControl, tTree, ptCallback);
            }
            else
            {
                return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase + 1], tTreeDefinition, tControl, tTree, ptCallback) || sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[dwCase + 2], tTreeDefinition, tControl, tTree, ptCallback);
            }
        }
    case ANT_BONE_WEIGHTS_MULTIPLY_BLEND:



        {
            return sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[0], tTreeDefinition, tControl, tTree, ptCallback) || sAnimTreeNodeRecursiveAnyTrueCallback(ptNode->pdwChildren[1], tTreeDefinition, tControl, tTree, ptCallback);
        }
    }


    return FALSE;
}





BOOL AnimationTreeIsAnimating(AnimTree & tTree, const AnimTreeControl & tControl, const AnimTreeDefinition & tTreeDefinition)
{



    if (tTree.bPaused)
        return FALSE;



    float flGlobalSpeedScale = AnimationGetGlobalSpeedScale();
    if (flGlobalSpeedScale < EPSILON)
        return FALSE;



    return sAnimTreeNodeRecursiveAnyTrueCallback(tTreeDefinition.dwRootNode, tTreeDefinition, tControl, tTree, sAnimTreeNodeIsAnimating);
}





BOOL AnimationTreeNeedsInterpolation(AnimTree & tTree, const AnimTreeControl & tControl, const AnimTreeDefinition & tTreeDefinition)
{



    return sAnimTreeNodeRecursiveAnyTrueCallback(tTreeDefinition.dwRootNode, tTreeDefinition, tControl, tTree, sAnimTreeNodeNeedsInterpolation);
}





BOOL AnimTreeLayerIsAnimating(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{



    float flGlobalSpeedScale = AnimationGetGlobalSpeedScale();
    if (flGlobalSpeedScale < EPSILON)
        return FALSE;



    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            if (sAnimChannelIsAnimating(tTree.ptChannels[dwLeafIndex]))
                return TRUE;
        }
    }
    return FALSE;
}







BOOL AnimTreeDefinitionHasLayer(const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash)
{


    for (uint32 dwLeafIndex = 0; dwLeafIndex < tTreeDefinition.dwNumLeafs; dwLeafIndex++)
    {
        if (tTreeDefinition.ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
            return TRUE;
    }



    for (uint32 dwNodeIndex = 0; dwNodeIndex < tTreeDefinition.dwNumNodes; dwNodeIndex++)
    {
        if (tTreeDefinition.ptNode[dwNodeIndex].tLayerName.dwHash == dwLayerHash)
            return TRUE;
    }
    return FALSE;
}









void ProcessBones(RActor * ptRActor, ProcessedBone * ptProcessedBones, AnimationController ** ptBoneAnimControllers, const PRSTransform * ptInterpolatedBones, const BoneStructure * ptBoneStructures, const int32 nBoneCount, float flDeltaTime)
{











    for (int32 i = 0; i < nBoneCount; i++)
    {
        int32 nParentIndex = ptBoneStructures[i].nParentIndex;
        PRSTransform & modelTransform = ptProcessedBones[i].modelTransform;

        if (ptBoneAnimControllers[i])
        {
            if (nParentIndex == -1)
            {

                ptBoneAnimControllers[i]->ComputeBone(ptRActor, modelTransform, cgtPRSTransformIdentity, ptInterpolatedBones[i], ptBoneStructures[i].transformParentRel, i, flDeltaTime);
            }
            else
            {

                const PRSTransform & parentModelTransform = ptProcessedBones[nParentIndex].modelTransform;
                ptBoneAnimControllers[i]->ComputeBone(ptRActor, modelTransform, parentModelTransform, ptInterpolatedBones[i], ptBoneStructures[i].transformParentRel, i, flDeltaTime);
            }
        }
        else
        {
            if (nParentIndex == -1)
            {

                modelTransform = ptInterpolatedBones[i];
            }
            else
            {

                const PRSTransform & parentModelTransform = ptProcessedBones[nParentIndex].modelTransform;
                modelTransform = parentModelTransform * ptInterpolatedBones[i];
            }
        }
    }
}









void ProcessBoneMatrices(ProcessedBone * ptProcessedBoneList, ProcessedBoneSkin * ptProcessedBoneSkinList, Vector3D & wpMin, Vector3D & wpMax, const BoneStructure * ptBoneStructure, const int32 nBoneCount)
{





    Vector3D wpLocalMin(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3D wpLocalMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);








    for (int32 i = 0; i < nBoneCount; i++)
    {
        ProcessedBone & tProcessedBone = ptProcessedBoneList[i];
        ProcessedBoneSkin & tProcessedBoneSkin = ptProcessedBoneSkinList[i];
        const BoneStructure & tBoneStructure = ptBoneStructure[i];














        tProcessedBone.transformDelta = tProcessedBone.modelTransform * tBoneStructure.transformInv;

        tProcessedBone.skinDelta = tProcessedBone.modelTransform * tBoneStructure.transformBindPoseInv;
        tProcessedBone.skinDelta.ToMatrix(tProcessedBoneSkin.mSkin);

        const PRSTransform & modelTransform = tProcessedBone.modelTransform;
        const Sphere & wsBounds = tBoneStructure.wsBounds;
        float flRadius = wsBounds.wdRadius * modelTransform.flScale;
        Vector3D wp = modelTransform * wsBounds.wpCenter;
        wpLocalMin.x = Math::Min(wpLocalMin.x, wp.x - flRadius); wpLocalMin.y = Math::Min(wpLocalMin.y, wp.y - flRadius); wpLocalMin.z = Math::Min(wpLocalMin.z, wp.z - flRadius);
        wpLocalMax.x = Math::Max(wpLocalMax.x, wp.x + flRadius); wpLocalMax.y = Math::Max(wpLocalMax.y, wp.y + flRadius); wpLocalMax.z = Math::Max(wpLocalMax.z, wp.z + flRadius);
    }

    wpMin = wpLocalMin;
    wpMax = wpLocalMax;
}






























































































































































































































































// NON_MATCHING: 2 instructions, register choice for m._43 in the z line
void AnimationSkinSingleBone(
    const Vector3D * pvSrcList,
    const int32 nVertCount,
    const VertInfluences * ptInfluenceList,
    const ProcessedBoneSkin * ptProcessedBoneSkinList,
    Vector3D * pvDestList)
{



    for (int32 nIndex = 0; nIndex < nVertCount; nIndex++)
    {
        const Vector3D & wp = pvSrcList[nIndex];
        const VertInfluences & tInfluence = ptInfluenceList[nIndex];
        const Matrix4x4 & m = ptProcessedBoneSkinList[tInfluence.tBoneInfluence[0].nBoneIndex].mSkin;
        Vector3D & wpFinal = pvDestList[nIndex];



        wpFinal.x = m._41 + (m._11 * wp.x + m._21 * wp.y + m._31 * wp.z);
        wpFinal.y = m._42 + (m._12 * wp.x + m._22 * wp.y + m._32 * wp.z);
        wpFinal.z = m._43 + (m._13 * wp.x + m._23 * wp.y + m._33 * wp.z);


    }
}




SNO AnimationChannelGetSNO(const AnimChannel & tChannel)
{

    return tChannel.tAnimPermID.snoAnim;
}






















































































































void AnimationChannelEnableFullRagdoll(AnimChannel & tChannel)
{

    tChannel.bFullRagdoll = TRUE;


    tChannel.ePlaybackMode = (PlaybackMode)0;
    tChannel.flBlendTime = 0.0f;
}

