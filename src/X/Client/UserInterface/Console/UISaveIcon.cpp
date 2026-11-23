// Original file: X/Client/UserInterface/Console/UISaveIcon.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../UIUtils.h"
#include "../../CGame.h"
#include "../../Console/GamerProfileInternal.h"
#include "../../Definitions/TextureDefinition.h"
#include "../../Gfx/GfxCommon.h"
#include "../../Sprite.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"

// Declared here until Sprite.h carries it (signature from the DWARF, Sprite.cpp:2289).
void SpriteDrawDeferredRotated( const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D *const pwrClipping, const Angle aYaw, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride );

namespace Console { namespace UISaveIcon {
void InitializeForApp(); void DisposeForApp(); const Vector2D & GetSaveIconDim(); void ForceDraw( const Vector2D * ptVecCenter ); void StopForceDraw(); void Update();
} }
namespace {

static const int32 MINIMUM_DISPLAY_MS = 3000;
static const int32 PULSE_RATE_MS = 1000;
static const float ROTATION_SPEED_DEGREES = -360.0f;

struct SaveIconState
{
    BOOL bVisible;  // 0x00
    BOOL bSaveInProgress;  // 0x04
    BOOL bForceDraw;  // 0x08
    Blizzard::Time::Millisecond tBeginSaveTime;  // 0x0c
    ImageTextureFrame tIcon;  // 0x10
    ImageTextureFrame tBackground;  // 0x18
    Vector2D vecIconCenterUIC;  // 0x20
    Vector2D vecIconDimUIC;  // 0x28
    Vector2D vecBackgroundDimUIC;  // 0x30

    SaveIconState()
        : bVisible( FALSE ), bSaveInProgress( FALSE ), bForceDraw( FALSE ), tBeginSaveTime( 0 )
        , vecIconCenterUIC( 0.0f, 0.0f ), vecIconDimUIC( 0.0f, 0.0f ), vecBackgroundDimUIC( 0.0f, 0.0f )
    {
    }
};
static SaveIconState sgtSaveIconState;
}  // namespace




void sSetTexture( const SNO snoTexture, const int32 nFrame, ImageTextureFrame & tOutTexture, Vector2D & vecOutDimUIC )
{




    tOutTexture = ImageTextureFrame( snoTexture, nFrame );
    const TextureDefinition * ptTexDef = DefinitionLockR<TextureDefinition>( tOutTexture.snoTexture );

    const TexFrame tFrame = ptTexDef->ptFrame[tOutTexture.nFrame];
    const int32 nWidth = Math::FloatToInt( ptTexDef->dwWidth * ( tFrame.flU1 - tFrame.flU0 ) ) - 1;
    const int32 nHeight = Math::FloatToInt( ptTexDef->dwHeight * ( tFrame.flV1 - tFrame.flV0 ) ) - 1;

    vecOutDimUIC.Set( (float)nWidth, (float)nHeight );

    gptGroupDefTexture->SetRequired( (void *)ptTexDef, TRUE );
    DefinitionUnlockR<TextureDefinition>( ptTexDef );
}


void sEnsureTextureIsSet()
{
    if ( sgtSaveIconState.tIcon.snoTexture == SNO_NONE )
    {
        sSetTexture( GlobalSNOGet( GLOBALSNO_TEXTURE_SAVING_ICON ), 0, sgtSaveIconState.tIcon, sgtSaveIconState.vecIconDimUIC );

        sSetTexture( GlobalSNOGet( GLOBALSNO_TEXTURE_SAVING_BACKGROUND ), 0, sgtSaveIconState.tBackground, sgtSaveIconState.vecBackgroundDimUIC );
    }
}



void sResetToDefaultPosition()
{
    Rect2D rectSafeUIC;
    CGameGetSafeViewportUIC( rectSafeUIC );

    if ( PlayerGetNumPlayers() < 4 )
    {
        sgtSaveIconState.vecIconCenterUIC.y = rectSafeUIC.top + 10.0f + sgtSaveIconState.vecBackgroundDimUIC.y * 0.5f;
    }
    else
    {
        sgtSaveIconState.vecIconCenterUIC.y = rectSafeUIC.Height() * 0.5f - sgtSaveIconState.vecBackgroundDimUIC.y * 0.5f;
    }
    sgtSaveIconState.vecIconCenterUIC.x = rectSafeUIC.right - sgtSaveIconState.vecBackgroundDimUIC.x * 0.5f;
}
void sStartIconDraw()
{
    sgtSaveIconState.bVisible = TRUE;
    sgtSaveIconState.tBeginSaveTime = Blizzard::Time::Milliseconds();

    sEnsureTextureIsSet();
}







void sOnBeginSave()
{
    sgtSaveIconState.bVisible = TRUE;
    sgtSaveIconState.bSaveInProgress = TRUE;
    sgtSaveIconState.tBeginSaveTime = Blizzard::Time::Milliseconds();
    sEnsureTextureIsSet();
    sResetToDefaultPosition();
}


void sOnEndSave()
{
    sgtSaveIconState.bSaveInProgress = FALSE;
}







void sDraw()
{
    const int32 nElapsedMS = Blizzard::Time::Milliseconds() - sgtSaveIconState.tBeginSaveTime;
    Rect2D rectSafeUIC;
    CGameGetSafeViewportUIC( rectSafeUIC );








    Rect2D rectBackgroundUIC;
    rectBackgroundUIC.left = sgtSaveIconState.vecIconCenterUIC.x - sgtSaveIconState.vecBackgroundDimUIC.x * 0.5f;
    rectBackgroundUIC.top = sgtSaveIconState.vecIconCenterUIC.y - sgtSaveIconState.vecBackgroundDimUIC.y * 0.5f;
    rectBackgroundUIC.bottom = rectBackgroundUIC.top + sgtSaveIconState.vecBackgroundDimUIC.y;
    rectBackgroundUIC.right = rectBackgroundUIC.left + sgtSaveIconState.vecBackgroundDimUIC.x;

    Rect2D rectBackgroundPixels;
    ConvertUICToPixels( rectBackgroundUIC, rectBackgroundPixels );


    const Angle aRotation = Math::PinWrap( ( nElapsedMS * Math::DegToAngle( ROTATION_SPEED_DEGREES ) ) / 1000.0f, 0.0f, FULL_CIRCLE );
    SpriteDrawDeferredRotated( sgtSaveIconState.tBackground, SNO_NONE, rectBackgroundPixels, crgbaWhite, crgbaBlack, 0.01f, NULL, aRotation, SNO_NONE, NULL, FALSE, RENDERLAYER_UI_OVERLAY );













    Rect2D rectIconUIC;
    rectIconUIC.left = sgtSaveIconState.vecIconCenterUIC.x - sgtSaveIconState.vecIconDimUIC.x * 0.5f;
    rectIconUIC.top = sgtSaveIconState.vecIconCenterUIC.y - sgtSaveIconState.vecIconDimUIC.y * 0.5f;
    rectIconUIC.bottom = rectIconUIC.top + sgtSaveIconState.vecIconDimUIC.y;
    rectIconUIC.right = rectIconUIC.left + sgtSaveIconState.vecIconDimUIC.x;

    Rect2D rectIconPixels;
    ConvertUICToPixels( rectIconUIC, rectIconPixels );










    RGBAColor rgbaIcon = crgbaWhite; const float flPulse = Math::Sin( ( nElapsedMS * FULL_CIRCLE ) / PULSE_RATE_MS - QUARTER_CIRCLE ) * 0.4f + 0.6f;
    rgbaIcon.a = (uint8)Math::FloatToInt( flPulse * 255.0f );


    SpriteDrawDeferred( sgtSaveIconState.tIcon, SNO_NONE, rectIconPixels, rgbaIcon, crgbaBlack, 0.0f, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_UI_OVERLAY, FALSE );
}















void Console::UISaveIcon::InitializeForApp()
{
    Console::GamerProfile::RegisterSaveTaskCallbacks(
        OnlineService::internal::function_bind_0_0<void, &sOnBeginSave>(),
        OnlineService::internal::function_bind_0_0<void, &sOnEndSave>() );
}






void Console::UISaveIcon::DisposeForApp()
{
    Console::GamerProfile::RegisterSaveTaskCallbacks( Console::GamerProfile::BeginSaveTaskCallback(), Console::GamerProfile::EndSaveTaskCallback() );
}







const Vector2D & Console::UISaveIcon::GetSaveIconDim()
{
    sEnsureTextureIsSet();
    return sgtSaveIconState.vecBackgroundDimUIC;
}




void Console::UISaveIcon::ForceDraw( const Vector2D * ptVecCenter )
{
    if ( ptVecCenter != NULL )
    {
        sgtSaveIconState.vecIconCenterUIC = *ptVecCenter;
    }
    else
    {
        sResetToDefaultPosition();
    }

    if ( !sgtSaveIconState.bVisible )
    {
        sgtSaveIconState.bVisible = TRUE;
        sEnsureTextureIsSet();
    }

    sgtSaveIconState.tBeginSaveTime = Blizzard::Time::Milliseconds();
    sgtSaveIconState.bForceDraw = TRUE;
}




void Console::UISaveIcon::StopForceDraw()
{
    sgtSaveIconState.bVisible = sgtSaveIconState.bSaveInProgress;
    sgtSaveIconState.bForceDraw = FALSE;
}




void Console::UISaveIcon::Update()
{
    if ( sgtSaveIconState.bVisible && !sgtSaveIconState.bSaveInProgress && !sgtSaveIconState.bForceDraw
        && Blizzard::Time::Milliseconds() - sgtSaveIconState.tBeginSaveTime >= MINIMUM_DISPLAY_MS )
    {
        sgtSaveIconState.bVisible = FALSE;
    }

    if ( sgtSaveIconState.bVisible )
    {
        sDraw();
    }
}
