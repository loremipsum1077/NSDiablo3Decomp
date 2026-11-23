// Original file: X/Common/AnimCommon.cpp

#include "AnimCommon.h"
#include "Definitions/AnimationDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "../Client/CWorld.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Random.h"

// Declared here because its header does not carry it yet (X/Client/Animation.cpp:834).
float AnimationGetAnimGlobalsSpeedScale();











void AnimationSetChannelPlayParams(AnimChannelPlayParams & tChannelPlayParams,
                                   const AnimationDefinition * pdefAnim,
                                   const int32 nPermutationIndex,
                                   const float flAnimSpeedScale)
{

    tChannelPlayParams.snoAnim = pdefAnim->tHeader.snoHandle;
    tChannelPlayParams.nPermutationIndex = nPermutationIndex;





    tChannelPlayParams.flAnimSpeedScale = (flAnimSpeedScale < 0.000001f) ? 0.001f : flAnimSpeedScale;





    tChannelPlayParams.ePlaybackMode = pdefAnim->ePlaybackMode;
}




void AnimationSetChannelPlayParams(AnimChannelPlayParams & tChannelPlayParams,
                                   const AnimationDefinition * pdefAnim,
                                   RNG & rng)
{





    int32 nPermutationIndex = AnimationSelectPermutation(pdefAnim, rng);


    if (GameActiveGameCommonDataIsClient() && CWorldIsRetro())
    {
        nPermutationIndex = 0;
    }




    const AnimPermutation & tPermutation = pdefAnim->ptPermutations[nPermutationIndex];
    float flAnimSpeedScale = RandRangeFloat_Internal(rng, tPermutation.flSpeedMultMin, tPermutation.flSpeedMultDelta);



    AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, nPermutationIndex, flAnimSpeedScale);
}





















uint32 AnimGetDurationTimeFromPlayParams(AnimChannelPlayParams & tChannelPlayParams,
                                         const AnimationDefinition * pdefAnim)
{
    if (tChannelPlayParams.snoAnim == SNO_NONE)
    {
        return (uint32)-1;
    }



    const AnimPermutation & tPermutation = pdefAnim->ptPermutations[tChannelPlayParams.nPermutationIndex];



    float flSpeedScale = AnimationGetGlobalSpeedScale() * tChannelPlayParams.flAnimSpeedScale;

    float flDuration = (float)(tPermutation.nKeyframeCount - 1) / (flSpeedScale * tPermutation.flFrameRate);





    uint32 dwDuration = Math::FloatToInt(flDuration);
    if (dwDuration == 0)
    {
        dwDuration = 1;
    }

    return dwDuration;
}



uint32 AnimGetContactTimeFromPlayParams(AnimChannelPlayParams & tChannelPlayParams,
                                        int32 nContactFrameIndex,
                                        const AnimationDefinition * pdefAnim)
{
    if (tChannelPlayParams.snoAnim == SNO_NONE)
    {
        return (uint32)-1;
    }




    const AnimPermutation & tPermutation = pdefAnim->ptPermutations[tChannelPlayParams.nPermutationIndex];


    float flSpeedScale = AnimationGetGlobalSpeedScale() * tChannelPlayParams.flAnimSpeedScale;

    float flContactKeyframe = tPermutation.flContactKeyframe[nContactFrameIndex];
    if (flContactKeyframe >= 0.0f)
    {
        return Math::FloatToInt(flContactKeyframe / (flSpeedScale * tPermutation.flFrameRate));
    }

    return (uint32)-1;
}









BOOL AnimCanRagdollFromPlayParams(const AnimChannelPlayParams & tChannelPlayParams,
                                  const AnimationDefinition * pdefAnim)
{
    if (tChannelPlayParams.snoAnim == SNO_NONE)
    {
        return FALSE;
    }


    const AnimPermutation & tPermutation = pdefAnim->ptPermutations[tChannelPlayParams.nPermutationIndex];
    return (tPermutation.dwFlags & (1 << ANIM_PERMUTATION_ENABLES_RAGDOLL_BIT)) != 0;
}










void AnimationGetAvgVelocity(const AnimationDefinition * pdefAnim,
                             const int32 nPermutationIndex,
                             const float flTranslationScale,
                             const BOOL bMultipleBySpeedScale,
                             Vector3D & wvVel)
{
    const AnimPermutation & tPermutation = pdefAnim->ptPermutations[nPermutationIndex];
    float flScale = AnimationGetGlobalSpeedScale() * flTranslationScale;

    if (bMultipleBySpeedScale)
        flScale *= tPermutation.flSpeedMultMin;

    wvVel = tPermutation.wvAvgVel * flScale;
}












uint32 AnimGetInterruptTimeFromPlayParams(AnimChannelPlayParams & tChannelPlayParams,
                                          const AnimationDefinition * pdefAnim)
{
    if (tChannelPlayParams.snoAnim == SNO_NONE)
    {
        return (uint32)-1;
    }




    const AnimPermutation & tPermutation = pdefAnim->ptPermutations[tChannelPlayParams.nPermutationIndex];

    if (tPermutation.flEarliestInterruptKeyFrame >= 0.0f)
    {

        float flSpeedScale = AnimationGetGlobalSpeedScale() * tChannelPlayParams.flAnimSpeedScale;

        return Math::FloatToInt(tPermutation.flEarliestInterruptKeyFrame / (flSpeedScale * tPermutation.flFrameRate));
    }

    return (uint32)-1;
}









int32 AnimationSelectPermutation(const AnimationDefinition * pdefAnim,
                                 RNG & rng)
{


    uint32 dwTotalWeight = 0;

    for (int32 i = 0; i < pdefAnim->nPermutationCount; i++)
    {
        dwTotalWeight += pdefAnim->ptPermutations[i].dwWeight;
    }
    if (dwTotalWeight == 0)
    {
        return 0;
    }


    uint32 dwRoll = Rand_Internal(rng, dwTotalWeight - 1);


    uint32 dwWeight = 0;
    for (int32 i = 0; i < pdefAnim->nPermutationCount; i++)
    {
        dwWeight += pdefAnim->ptPermutations[i].dwWeight;
        if (dwRoll < dwWeight)
        {
            return i;
        }
    }




    return -1;
}





float AnimationGetGlobalSpeedScale()
{






    if (GameGetActiveGameCommonData().GameModeGet() == GAMEMODE_HOLODECK)
    {




        return AnimationGetAnimGlobalsSpeedScale();
    }

    return 1.0f;
}
