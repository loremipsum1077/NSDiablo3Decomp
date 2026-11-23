// Original file: X/Client/Sound/SoundUtils.cpp
// Original file: X/Client/Sound/SoundUtils.cpp (all functions written; SelectSoundPermutation is NON_MATCHING).
#include "../CWorld.h"  // CWorldIsRetro, called at line 410
#include "../../Sigma/Random.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/SNO/SNOName.h"
#include "../Definitions/SoundBankDefinition.h"
#include "../Definitions/SoundDefinition.h"
#include "Sound.h"
#include "SoundPlatform.h"
#include <math.h>
#include <alloca.h>
enum {
    LINEAR_TO_BEL_LOOKUP_ENTRY_COUNT = 256,
    STREAMING_DURATION_CUTOFF = 600,
};

struct SoundUtilGlobals {  // size 0x8
    RNG rng;  // 0x00
};





















static const int32 MAX_BEL_VOLUME = 0;
static const int32 MIN_BEL_VOLUME = -10000;














static SoundUtilGlobals * sgptGlobals;









void SoundUtilsInitialize()

{
    sgptGlobals = (SoundUtilGlobals *)SigmaMemoryNew(sizeof(SoundUtilGlobals), 4, NULL, TRUE);
    RandInit(sgptGlobals->rng);
}






void SoundUtilsDispose()

{
    if (sgptGlobals)
    {
        SigmaMemoryFree(sgptGlobals, NULL); sgptGlobals = NULL;
    }

}



































float SoundBelToLinear(const int32 lVolume)




{
    if (lVolume <= MIN_BEL_VOLUME)
    {
        return 0.0f;
    }
    if (lVolume >= MAX_BEL_VOLUME)
    {
        return 1.0f;
    }






    return powf(10.0f, (float)lVolume / 2000.0f);


}
























float SoundPitchSemitonesToLinear(const float flSemitones)






{
    float flOctaves = Math::Pin(flSemitones / 12.0f, -5.0f, 5.0f);

    return powf(2.0f, flOctaves);
}




StreamMode SoundShouldBeStreamed(const SoundSample & tSample)

{


    if (tSample.ComputeDuration() > STREAMING_DURATION_CUTOFF)
        return 1;




    return 0;
}




StreamMode SoundShouldBeStreamed(const SoundDefinition * pdefSound)

{

    if (SoundIsConversation(pdefSound))
    {
        return 1;
    }


    SNOName snoname(SOUND_SNO, pdefSound->tHeader.snoHandle);
    if (!SNOFileExists(snoname))
    {
        return 0;
    }

    void * lpSoundBank = NULL;

    for (int32 i = 0; i < pdefSound->nPermutationCount; i++)
    {
        const SoundPermutation & tPermutation = pdefSound->ptPermutations[i];

        if (tPermutation.snoValidatedSoundBank == SNO_NONE)
            continue;



        if (lpSoundBank == NULL || ((const SoundBankDefinition *)lpSoundBank)->tHeader.snoHandle != tPermutation.snoValidatedSoundBank)
        {
            if (lpSoundBank)
            {
                gptGroupDefSoundBank->UnlockR(lpSoundBank);
            }
            lpSoundBank = (void *)gptGroupDefSoundBank->LockR(tPermutation.snoValidatedSoundBank, FALSE);
        }
        const SoundBankDefinition * pdefSoundBank = (const SoundBankDefinition *)lpSoundBank;



        if (pdefSoundBank->nSoundSampleCount != 0 &&
            SoundShouldBeStreamed(pdefSoundBank->ptSoundSamples[tPermutation.nSampleIndex]))
        {
            gptGroupDefSoundBank->UnlockR(lpSoundBank);
            return 1;
        }
    }


    if (lpSoundBank)
    {
        gptGroupDefSoundBank->UnlockR(lpSoundBank);
    }

    return 0;
}

































static BOOL fCheckStreamedOnce(
    const SoundDefinition * pdefSound,
    BOOL & fStreamInitialized,
    BOOL & fStreamed)
{
    if (!fStreamInitialized)
    {



        // Store of fStreamInitialized shares the call row 306:17; written on its own line (inferred).
        fStreamed = SoundShouldBeStreamed(pdefSound);
        fStreamInitialized = TRUE;
    }

    return fStreamed;
}
// NON_MATCHING: 1156 vs 1228 bytes. Ours loop-unswitches the line-420 loop on line 429's `nNumWeightedPermutations < 2` (folding it away); the original unswitches on line 435's `> 1` and keeps 429 in both copies (72 bytes). Also fStreamInitialized is kept in a register (original spills it), and the retro BOOL has the opposite polarity. Tried: 429 as ||, swapped ||, bitwise |; 452 as if/else and as a clear-only if.
int32 SelectSoundPermutation(
    const SoundDefinition * pdefSound,
    SoundPlayHistory * ptSoundPlayHistory)
{
    // nPermutationIndex starts at -1 (inferred): the line-543 search leaves -1 when nothing is found.
    int32 nPermutationIndex = -1;

    int32 nNumSoloingPerms = 0;






















    uint32 dwTotalWeight = 0;
    int32 nViablePermCount = 0;
    int32 * pnViablePermList = (int32 *)alloca(pdefSound->nPermutationCount * sizeof(int32));

    int32 nNumWeightedPermutations = 0;

    int32 nHighPriorityCounter = 0;


    BOOL fStreamInitialized = FALSE;
    BOOL fStreamed = FALSE;
    int32 nHighestPermutationToPreload = -1;
    uint32 dwHighestWeightToPreload = 0;
    int32 nHighestPermutationPreloaded = -1;
    uint32 dwHighestWeightPreloaded = 0;


    for (int32 i = 0; i < pdefSound->nPermutationCount; i++)
    {
        const SoundPermutation & tPermutation = pdefSound->ptPermutations[i];
        if (tPermutation.dwWeight > 0)
        {

            if (!SoundPlatform::IsReady(tPermutation.snoValidatedSoundBank, tPermutation.nSampleIndex) && !fCheckStreamedOnce(pdefSound, fStreamInitialized, fStreamed))
            {
                if (!SoundPlatform::IsPreloaded(tPermutation.snoValidatedSoundBank, tPermutation.nSampleIndex))
                {
                    if (tPermutation.dwWeight > dwHighestWeightToPreload)
                    {
                        dwHighestWeightToPreload = tPermutation.dwWeight;
                        nHighestPermutationToPreload = i;
                    }
                }
                else
                {
                    if (tPermutation.dwWeight > dwHighestWeightPreloaded)
                    {
                        dwHighestWeightPreloaded = tPermutation.dwWeight;
                        nHighestPermutationPreloaded = i;
                    }
                }

                continue;
            }






            nNumWeightedPermutations++;
        }
    }


    // Line 403 has no row: inferred from the preloaded index being the one used at line 512 after Preload.
    if (nHighestPermutationPreloaded == -1 && nHighestPermutationToPreload != -1)
    {
        const SoundPermutation & tHighestPermutation = pdefSound->ptPermutations[nHighestPermutationToPreload];
        SoundPlatform::Preload(tHighestPermutation.snoValidatedSoundBank, tHighestPermutation.nSampleIndex);
        nHighestPermutationPreloaded = nHighestPermutationToPreload;
    }



    // Bypass test (row 418:30) folded into this BOOL: the line-524 clear runs only when retro, effect type and no bypass (inferred).
    BOOL fRestrictPermutationsForRetro =
        CWorldIsRetro() &&
        (pdefSound->eType == SOUNDTYPE_EFFECT || pdefSound->eType == SOUNDTYPE_EFFECT_UI) &&
        !(pdefSound->dwFlags & (1 << SOUND_BYPASS_RETRO_BIT));




    // Restricted path compares the count with 1 (cmp #1 / b.le) and uses 2: written as a ternary (inferred; no Math::Min inline in the DWARF).
    int32 nPermutationsToUse = (fRestrictPermutationsForRetro && pdefSound->nPermutationCount > 1) ? 2 : pdefSound->nPermutationCount;

    for (int32 i = 0; i < nPermutationsToUse; i++)
    {
        const SoundPermutation & tPermutation = pdefSound->ptPermutations[i];

        BOOL fPlayable = TRUE;



        if (pdefSound->dwFlags & (1 << SOUND_NO_BACK_TO_BACK_REPEATS_BIT))
            fPlayable = nNumWeightedPermutations < 2 || i != ptSoundPlayHistory->nLastPlayedPerm;




        // nNumWeightedPermutations > 1 inferred from the loop being unswitched on cmp w24, #1 / b.le.
        if (nNumWeightedPermutations > 1 && (pdefSound->dwFlags & ((1 << SOUND_PLAY_EACH_PERMUTATION_ONCE_BIT) | (1 << SOUND_PLAY_IN_ORDER_BIT))))
        {


            int32 nByteIndex = i >> 3;
            int32 nBitIndex = i & 7;










            // Shape of lines 452-453 inferred: played bit, then fPlayable, then the weight test, all branching to the IsReady at line 472.
            if (!(ptSoundPlayHistory->arPlayedFlags[nByteIndex] & (1 << nBitIndex)) && fPlayable &&
                tPermutation.dwWeight > 0)
                fPlayable = TRUE;
            else
                fPlayable = FALSE;
        }











        if (fPlayable)
        {

            if (SoundPlatform::IsReady(tPermutation.snoValidatedSoundBank, tPermutation.nSampleIndex) || fCheckStreamedOnce(pdefSound, fStreamInitialized, fStreamed))
            {

















                if (tPermutation.snoValidatedSoundBank != SNO_NONE)
                {






                    pnViablePermList[nViablePermCount++] = i;
                    dwTotalWeight += tPermutation.dwWeight;
                }
            }
        }
    }




    if (dwTotalWeight == 0)
    {

        if (nHighestPermutationPreloaded != -1)
        {
            nPermutationIndex = nHighestPermutationPreloaded;
            nViablePermCount = 1;
        }
        else
        {
            if (fRestrictPermutationsForRetro)
            {
            // nPermutationIndex = 0 has no row (the mov sits at 524): inferred from w21 = 0 on both paths.

                // 0x10 = sizeof(arPlayedFlags).
                SigmaMemoryClear(ptSoundPlayHistory->arPlayedFlags, sizeof(ptSoundPlayHistory->arPlayedFlags));
            }
            nPermutationIndex = 0;
        }
    }





    // Line 541: Rand_Internal(rng, n - 1) since the original tests (n - 1) & n and divides by n. Line 542: dwTotalWeight reused as the running sum (546:27 is a compound assignment; no other local exists). Line 552: break inferred from the loop exit test.
    else if (pdefSound->dwFlags & (1 << SOUND_PLAY_IN_ORDER_BIT))
    {
        nPermutationIndex = pnViablePermList[0];
    }
    else
    {
        uint32 dwWeight = Rand_Internal(sgptGlobals->rng, dwTotalWeight - 1);
        dwTotalWeight = 0;
        for (int32 i = 0; i < nViablePermCount; i++)
        {
            int32 nIndex = pnViablePermList[i];
            dwTotalWeight += pdefSound->ptPermutations[nIndex].dwWeight;



            if (dwWeight < dwTotalWeight)
            {
                nPermutationIndex = nIndex; break;
            }
        }
    }
    if (pdefSound->dwFlags & ((1 << SOUND_PLAY_EACH_PERMUTATION_ONCE_BIT) | (1 << SOUND_PLAY_IN_ORDER_BIT)))
    {

        if (nViablePermCount == 1)
        {
            SigmaMemoryClear(ptSoundPlayHistory->arPlayedFlags, sizeof(ptSoundPlayHistory->arPlayedFlags));
        }
        else
        {
            int32 nByteIndex = nPermutationIndex >> 3;
            int32 nBitIndex = nPermutationIndex & 7;




            ptSoundPlayHistory->arPlayedFlags[nByteIndex] |= (1 << nBitIndex);
        }
    }
    ptSoundPlayHistory->nLastPlayedPerm = nPermutationIndex;



    return nPermutationIndex;
}




uint32 SelectLoopingSoundStartSample(const BOOL fRandomStartPosition, SoundSample & tSample)

{
    uint32 dwStartSample = 0;


    if (fRandomStartPosition)
    {
        uint32 dwChannelCount, dwFrequency, dwBitRate;

        SoundFormatToCSB(tSample.GetFormat(), dwChannelCount, dwFrequency, dwBitRate);
        uint32 dwBytesPerSample = dwChannelCount * dwBitRate / 8;

        dwStartSample = Rand_Internal(sgptGlobals->rng, tSample.dwDecompressedSize / dwBytesPerSample - 1);
    }

    return dwStartSample;
}




float GetDspParamValue(DspParam & tParam)

{


    float flValue = tParam.flMinValue;

    if (tParam.flDeltaValue > 0.0f)
    {
        flValue = RandRangeFloat_Internal(sgptGlobals->rng, tParam.flMinValue, tParam.flDeltaValue);
    }



    return flValue;
}






























float SoundGetRolloffAtten(const float flDist, const RolloffParams & tRolloffParams)



{




    if (tRolloffParams.eFalloffType == FALLOFF_NONE)
    {
        return 1.0f;
    }
    if (flDist > tRolloffParams.flDistanceMax)
    {
        return 0.0f;
    }
    if (flDist <= tRolloffParams.flDistanceMin)
        return 1.0f;
    switch (tRolloffParams.eFalloffType)
    {
    case FALLOFF_INVERSE_DISTANCE:
        return 1.0f / (1.0f + tRolloffParams.flRolloffFactor * (flDist - tRolloffParams.flDistanceMin) / tRolloffParams.flDistanceMin);
    case FALLOFF_LINEAR:
        return 1.0f - (flDist - tRolloffParams.flDistanceMin) / (tRolloffParams.flDistanceMax - tRolloffParams.flDistanceMin);
    case FALLOFF_LINEAR_LOG:
        {
            float flDB = (flDist - tRolloffParams.flDistanceMin) * -50.0f / (tRolloffParams.flDistanceMax - tRolloffParams.flDistanceMin);
            return SoundBelToLinear(Math::FloatToInt(flDB * 100.0f));
        }
    }
    return 1.0f;
}
