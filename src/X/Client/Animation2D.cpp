// Reconstructed source; see docs/source_provenance.md.
// Diablo III - X/Client/Animation2D.cpp
// 2D animation playback: an AnimState (X/Client/AnimationCommon.h) is driven by an
// Animation2DDefinition (X/Client/Definitions/Animation2DDefinition.h) and drawn, either
// as the definition's sprite frame or, when the frame is missing, as a plain rectangle.

#include "AnimationCommon.h"
#include "Definitions/Animation2DDefinition.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxPrimitives.h"
#include "ImageCollection.h"
#include "Sound/Sound.h"
#include "Sprite.h"
#include "../Common/Definitions/Definitions.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XPtr64.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/RGBAColor.h"

// Animation2DReset is defined below in this file (line 72); its caller in this
// translation unit needs the declaration before the definition.
void Animation2DReset( AnimState & tAnimState, const SNO snoAnim2D );

static void sAnimationGetCurrentFrame( const SNO snoAnim2D, const AnimState & tAnimState, RGBAColor & rgbaColor, ImageTextureFrame & tImageTexFrame )
{
    // Walks the definition's frame table (Anim2DFrame: name + colour) at the state's
    // current frame and hands the frame back to the caller.
    //
    if ( snoAnim2D == SNO_NONE )
    {
        rgbaColor = RGBAColor( 0, 0, 0, 0 );
        tImageTexFrame = cgtImageTextureFrameNull;
    }
    else
    {
        const Animation2DDefinition * ptDef = (const Animation2DDefinition *)gptGroupDefAnimation2D->LockRFast( snoAnim2D );

        int32 nCurrentFrame = AnimationComputeCurrentFrame( tAnimState );

        nCurrentFrame = Math::Pin( nCurrentFrame, 0, ptDef->nFrameCount - 1 );

        // The frame name selects the image; the frame colour rides along with it.
        LPCSTR szFrameName = ptDef->ptFrames[nCurrentFrame].szName;
        ImageCollectionGetImageInfo( ptDef->eCollection, szFrameName, tImageTexFrame );
        rgbaColor = ptDef->ptFrames[nCurrentFrame].rgbaColor;
    }
}

//---------------------------------------------------------------------------------------------------
// Animation2DSet
//---------------------------------------------------------------------------------------------------
// Stores snoAnim2DNew on the state, restarting it when it changed or a reset was forced.
void Animation2DSet( AnimState & tAnimState, SNO & snoAnim2D, const SNO snoAnim2DNew, const BOOL fForceReset )
// fForceReset restarts even when the SNO is unchanged (a spawn or a re-entry), and a changed SNO
// is likewise stored and re-initialised. The reset itself lives in Animation2DReset; the DWARF
// records that body inlined here at line 65.
{
    if ( fForceReset
         || snoAnim2D != snoAnim2DNew )
    {
        snoAnim2D = snoAnim2DNew;
        Animation2DReset( tAnimState, snoAnim2DNew );
    }
}

//---------------------------------------------------------------------------------------------------
// Animation2DReset
//---------------------------------------------------------------------------------------------------
void Animation2DReset( AnimState & tAnimState, const SNO snoAnim2D )
// Locks the animation definition and re-initialises the state from its FrameAnim (and the known
// frame count), then plays the definition's one-shot sound if it has one.
{
    if ( snoAnim2D != SNO_NONE )
    {
        // The definition begins with RequiredDefinitionHeader; +0x10 is the ImageCollection,
        // +0x14 the FrameAnim, +0x28 the sound and +0x2c the frame count (Animation2DDefinition.h).
        const Animation2DDefinition * ptDef = (const Animation2DDefinition *)gptGroupDefAnimation2D->LockRFast( snoAnim2D );

        AnimationReset( tAnimState,
                        ptDef->tFrameAnim,
                        ptDef->nFrameCount );

        if ( ptDef->snoSound != SNO_NONE )
        {
            SoundManager::PlaySound( ptDef->snoSound, cgtPSODefaults );
        }
    }

}

//---------------------------------------------------------------------------------------------------
// Animation2DUpdate
//---------------------------------------------------------------------------------------------------
void Animation2DUpdate( AnimState & tAnimState, const SNO snoAnim2D, const float flDeltaTime )
// Advances the state using the definition's playback mode; a state with no animation cannot move.
{
    if ( snoAnim2D != SNO_NONE )
    {
        const Animation2DDefinition * ptDef = (const Animation2DDefinition *)gptGroupDefAnimation2D->LockRFast( snoAnim2D );

        AnimationAdvance( tAnimState, ptDef->tFrameAnim.ePlaybackMode, flDeltaTime );
    }
}

//---------------------------------------------------------------------------------------------------
// Animation2DDraw
//---------------------------------------------------------------------------------------------------
// UNVERIFIED (possible NON_MATCHING): the two colour branches, the frame resolution, the inlined
// sAnimationGetCurrentFrame / Math::Pin / operator* / MultiplyAlpha and both deferred draws follow
// the DWARF call sites. The stack-slot order now follows DWARF decl lines 147/149 (rgbaColorResult
// before wrResult). The only unmeasured point is the SNO_NONE fallback's zero-initialisation order
// in sAnimationGetCurrentFrame (original a,b,g,r; a different clang schedule could still emit
// r,g,b,a). This cannot be checked without try_file.
void Animation2DDraw( const SNO snoAnim2D, const AnimState & tAnimState, const Rect2D & wrRect, const Rect2D & wrRectClipping, const uint8 bAlpha, const float flZ, const RGBAColor & rgbaColor, const SNO snoShader, const RenderLayer eRenderLayerOverride )
// Resolves the current frame and draws it. A fully transparent draw is dropped; a frame that is
// missing falls back to a solid rectangle in the frame's colour, otherwise the sprite is queued.
// All drawing is deferred (SpriteDrawDeferred / GfxRectDrawDeferred).
//
// The two branches below are the two documented fallbacks:
//   * the rectangle branch rebuilds the colour as frame * rgbaColor with the alpha first
//     folded with bAlpha, then intersects the rect and fills it;
//   * the sprite branch rebuilds the colour as frame * rgbaColor with the alpha replaced
//     by bAlpha, then queues the sprite.
{
    if ( bAlpha == 0 )
        return;

    // sAnimationGetCurrentFrame fills the texture frame and the frame colour; the fallback
    // colour when the animation is unset is fully transparent.
    ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
    RGBAColor rgbaColorRect;
    sAnimationGetCurrentFrame( snoAnim2D, tAnimState, rgbaColorRect, tImageTexFrame );

    if ( tImageTexFrame.snoTexture == SNO_NONE
         || tImageTexFrame.nFrame == -1 )
    {
        // No sprite to draw: fold bAlpha into the frame colour and fill the intersection.
        rgbaColorRect.a = MultiplyAlpha( bAlpha, rgbaColorRect.a );
        if ( rgbaColorRect.a != 0 )
        {
            RGBAColor rgbaColorResult = rgbaColor * rgbaColorRect;
            Rect2D wrResult;

            // The rectangle is the intersection of the draw rect and the clip rect.
            if ( Rect2D::Intersect( wrRect, wrRectClipping, &wrResult ) )
            {
                GfxRectDrawDeferred( wrResult, rgbaColorResult, flZ, eRenderLayerOverride, 0 );
            }
        }
    }
    else
    {
        RGBAColor rgbaColorResult = rgbaColor * rgbaColorRect;
        rgbaColorResult.a = bAlpha;

        SpriteDrawDeferred( tImageTexFrame, snoShader, wrRect, rgbaColorResult, crgbaBlack, flZ, &wrRectClipping, SNO_NONE, NULL, FALSE, eRenderLayerOverride, FALSE );
    }
}

// Provenance (evidence: DWARF build/dwarf/headers/X/Client/Animation2D.cpp and the original bytes
// of 0x710033eda0..0x710033f140, not assumptions):
//   Animation2DDefinition is X/Client/Definitions/Animation2DDefinition.h:31 (size 0x40):
//     eCollection +0x10, tFrameAnim +0x14, snoSound +0x28, nFrameCount +0x2c, ptFrames +0x38.
//   Anim2DFrame is that header:25 (size 0x44): szName +0x00, rgbaColor +0x40.
//   gptGroupDefAnimation2D is Animation2DDefinition.h:57 (Animation2DDefinition.cpp:52).
//   LockRFast is X/Common/Definitions/Definitions.h:267; its result is the definition.
//   AnimationComputeCurrentFrame / AnimationReset / AnimationAdvance are AnimationCommon.h.
//   PlaySound and cgtPSODefaults are X/Client/Sound/Sound.h:223 resp. :218.
//   Rect2D::Intersect is X/Sigma/Math/Rect.h:81; MultiplyAlpha and RGBAColor::operator* are
//   X/Sigma/RGBAColor.inl:18 resp. :175; crgbaBlack is X/Sigma/RGBAColor.h:62.
//   cgtImageTextureFrameNull is X/Client/Gfx/GfxCommon.h:567.
//   SpriteDrawDeferred is declared in X/Client/Sprite.h (Sprite.cpp:2167); GfxRectDrawDeferred
//   in X/Client/Gfx/GfxPrimitives.h (GfxPrimitives.cpp:171); ImageCollectionGetImageInfo in
//   X/Client/ImageCollection.h (ImageCollection.cpp:979).
