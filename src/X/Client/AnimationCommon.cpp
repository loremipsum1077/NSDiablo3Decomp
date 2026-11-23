// Reconstructed source; see docs/source_provenance.md.
// Diablo III - X/Client/AnimationCommon.cpp
// Animation state helpers shared by the client animation code.

#include "AnimationCommon.h"
#include "CGame.h"
#include "Prefetch.h"
#include "Definitions/TextureDefinition.h"
#include "Gfx/GfxCommon.h"
#include "../Common/Definitions/Definitions.h"
#include "../Common/SNO/SNOName.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Random.h"

// ANIM_EPSILON is the tolerance subtracted from a frame animation's frame count when
// its last time is computed, so that an animation never lands exactly on its last frame
// boundary and can always wrap (see AnimStateInitialize and AnimationReset).
//
static const float ANIM_EPSILON = 0.0001f;

// Declared up front; the body is at the bottom of this file (AnimationCommon.cpp:213).
BOOL AnimStateIsStopped( const AnimState & tAnimState );

int32 AnimationComputeCurrentFrame( const AnimState & tState )
{
    // The frame index is kept current by AnimationAdvance and AnimStateInitialize;
    // callers only ever need to read it back.
    return tState.nFrame;
}

//---------------------------------------------------------------------------------------------------
// AnimationAdvance
//---------------------------------------------------------------------------------------------------
// Advances one animation state by flDeltaTime seconds and reports where the animation ended up.
//
// The result is a mask of the ANIMATION_*_BIT flags declared in AnimationCommon.h:
//
//   ANIMATION_AT_END_BIT   - the animation reached (or passed) its last frame;
//   ANIMATION_FINISHED_BIT - a non-looping animation was stopped on its last frame.
//
// ePlaybackMode selects the end-of-animation behaviour:
//
//   PLAYBACKMODE_NORMAL - clamp on the last frame and stop the animation;
//   PLAYBACKMODE_LOOP   - wrap around and keep playing.
//
// A state that is pending (its frame count is not known yet) or stopped (no velocity)
// cannot change, so it is returned from with result 0.
//
// flT is the current time inside the animation (in frames), flTMax the last valid time
// (frameCount - ANIM_EPSILON), flTVel the playback rate in frames per second and flDeltaTime
// the elapsed time in seconds.
//
// Callers compare the remembered flTLast with flT, or test the returned flags, to detect
// an end-of-animation or a wrap exactly once.
//
//---------------------------------------------------------------------------------------------------
// AnimStateInitialize / AnimationReset
//---------------------------------------------------------------------------------------------------
// Initialises a state for the frame animation of a texture SNO: the frame count is taken
// from the texture definition (prefetching the texture first, deferring to the pending
// path when it is not resident yet), a random start frame is rolled inside the animation
// range [dwStartFrameMin, dwStartFrameMin + dwStartFrameDelta] and a random playback rate
// is rolled inside [flFrameRateMin, flFrameRateMin + flFrameRateDelta].
//
// When there is no texture SNO (== -1) or no frame animation, the state is left at time 0
// on frame 0 with no velocity; such a state never advances. AnimationReset performs the
// same work for an already known frame count, which is why it takes nFrameCount directly.
//
//---------------------------------------------------------------------------------------------------
// AnimStateIsStopped
//---------------------------------------------------------------------------------------------------
// An animation is stopped when its playback rate is (almost) zero; the epsilon keeps a rate
// that has decayed through arithmetic noise from being treated as moving.
//
//---------------------------------------------------------------------------------------------------
// Shared helpers
//---------------------------------------------------------------------------------------------------
// AnimationCommon.h owns AnimState; this file owns the frame and time bookkeeping for it.
uint32 AnimationAdvance( AnimState & tState, const PlaybackMode ePlaybackMode, const float flDeltaTime )
{
    uint32 dwResultFlags = 0;

    // A pending state has no frame count yet: it cannot advance.
    //
    if ( tState.fPending )
        return dwResultFlags;

    // A stopped state has no velocity to integrate.
    //
    //
    if ( AnimStateIsStopped( tState ) )
        return dwResultFlags;

    // Integrate the elapsed time, remembering the previous time first.
    //
    //
    //
    tState.flTLast = tState.flT;
    tState.flT += tState.flTVel * flDeltaTime;

    // Resolve the end-of-animation behaviour of the selected playback mode.
    //
    switch ( ePlaybackMode )
    {
    case PLAYBACKMODE_NORMAL:
        // Clamp on the last frame and stop the animation.
        if ( tState.flT > tState.flTMax )
        {
            tState.flTVel = 0.0f; dwResultFlags |= (1 << ANIMATION_AT_END_BIT) | (1 << ANIMATION_FINISHED_BIT);
            tState.flT = Math::Min( tState.flT, tState.flTMax );
        }
        break;

    case PLAYBACKMODE_LOOP:
        // Wrap the time around until it fits in one cycle, flagging the wrap at least once.
        //
        //
        for ( ; tState.flT > tState.flTMax; dwResultFlags |= (1 << ANIMATION_AT_END_BIT) )
        {
            tState.flT -= tState.flTMax;
        }
        break;

    default:
        //
        DisplayInternalError( 0, "", 127, ERROR_INTERNAL_FATAL );
        ExitProgram( ERROR_INTERNAL_FATAL );
    }

    tState.nFrame = Math::FloatToInt( tState.flT );

    // Only the ANIMATION_*_BIT flags are ever observed by callers.
    //
    return dwResultFlags;
}

//---------------------------------------------------------------------------------------------------
void AnimStateInitialize( AnimState & tAnimState, const SNO snoTexture,
                         const FrameAnim * ptFrameAnim )
// Initialises the state; the comment block above describes the rolled ranges.
//
{
    if ( snoTexture != -1 && ptFrameAnim )
    {
        uint32 dwFrameCount;

        // The frame count lives in the texture definition; prefetch the texture first and
        // fall back to a single frame, pending animation until the texture is resident.
        if ( Prefetch::AddToQueue( SNOName( TEXTURE_SNO, snoTexture ), SNOFilesAsyncPriority_High ) )
        {
            ScopedTextureDefinitionLockR pdefTexture( snoTexture );
            dwFrameCount = pdefTexture->dwFrameCount;
            tAnimState.fPending = false;
        }
        else
        {
            dwFrameCount = 1; tAnimState.fPending = true;
        }


        // Roll the start frame and the playback rate from the animation's ranges.
        //
        CGameGlobals & tCGameGlobals = CGameGlobalsGet();

        int32 nFrame = RandRange_Internal( tCGameGlobals.rngLocal, ptFrameAnim->dwStartFrameMin, ptFrameAnim->dwStartFrameDelta );
        nFrame = Math::Min( nFrame, (int32)dwFrameCount - 1 );

        tAnimState.nFrame = nFrame;

        tAnimState.flT = (float)nFrame;
        tAnimState.flTMax = (float)dwFrameCount - ANIM_EPSILON;

        tAnimState.flTVel = RandRangeFloat_Internal( tCGameGlobals.rngLocal, ptFrameAnim->flFrameRateMin, ptFrameAnim->flFrameRateDelta ) * 60.0f;
    }
    else
    {
        //
        //
        //
        tAnimState.flT = 0.0f; tAnimState.nFrame = 0;
        tAnimState.flTMax = 0.0f;
        tAnimState.flTVel = 0.0f;
    }
}

//---------------------------------------------------------------------------------------------------
// AnimationReset
//---------------------------------------------------------------------------------------------------
void AnimationReset( AnimState & tAnimState, const FrameAnim & tFrameAnim,
                     const int32 nFrameCount )
// Re-initialises the state for an already known frame count (see the block above).
//
{
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    tAnimState.flTLast = -1.0f;
    int32 nFrame = RandRange_Internal( tCGameGlobals.rngLocal, tFrameAnim.dwStartFrameMin, tFrameAnim.dwStartFrameDelta );

    tAnimState.nFrame = nFrame;

    tAnimState.flT = (float)nFrame;
    tAnimState.flTMax = (float)nFrameCount - ANIM_EPSILON;

    tAnimState.flTVel = RandRangeFloat_Internal( tCGameGlobals.rngLocal, tFrameAnim.flFrameRateMin, tFrameAnim.flFrameRateDelta ) * 60.0f;

    // flTLast is deliberately left at -1 so the next AnimationAdvance reports a change.
}

//---------------------------------------------------------------------------------------------------
// AnimStateIsStopped
//---------------------------------------------------------------------------------------------------
BOOL AnimStateIsStopped( const AnimState & tAnimState )
{
    // A playback rate that has decayed through arithmetic noise still counts as stopped.
    return Math::Abs( tAnimState.flTVel ) < 1e-6f;
}

//---------------------------------------------------------------------------------------------------
// Provenance (all four functions above byte-match; the facts below were confirmed against the DWARF
// and the original bytes, not assumed).
//---------------------------------------------------------------------------------------------------
// FrameAnim is owned by X/Client/Gfx/GfxCommon.h (DWARF build/dwarf/headers/X/Client/Gfx/GfxCommon.h:387);
// its members map to flFrameRateMin (+0x4), flFrameRateDelta (+0x8), dwStartFrameMin (+0xc) and
// dwStartFrameDelta (+0x10). TextureDefinition::dwFrameCount is at 0x210 (DWARF TextureDefinition.h:55).
// TEXTURE_SNO (44) is X/Common/SNO/SNOFiles.h:150; SNOFilesAsyncPriority_High (2) is SNOFiles.h:24
// (SNOGroup is typedef int32, SNOFiles.h:178).
// Pool constants in the original: 1e-6f = s_intTypeLookup+0x60 (0x358637bd), 60.0f =
// s_intTypeLookup+0x78 (0x42700000), ANIM_EPSILON = 0.0001f (0x38d1b717, declared above at line 20).
