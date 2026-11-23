// Adopted from the DWARF skeleton (build/dwarf/headers/X/Client/Sprite.h);
// hand-maintained from here on.
// Original file: X/Client/Sprite.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx/GfxCommon.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/RGBAColor.h"

typedef int32 AsyncRenderDeviceStatus;

class UISprite {  // size 0x8
    // vtable pointer
public:
    virtual void Draw() = 0;  // vtable slot 0
};
static_assert(sizeof(UISprite) == 0x8, "UISprite");

class UISpritePoolInterface {  // size 0x18
    // vtable pointer
public:
    UISpritePoolInterface();
    virtual ~UISpritePoolInterface();  // vtable slot 0
    virtual void Initialize() = 0;  // vtable slot 2
    virtual void Dispose() = 0;  // vtable slot 3
    virtual void DeleteAll() = 0;  // vtable slot 4
    UISpritePoolInterface * GetNext() const;
private:
    UISpritePoolInterface * m_ptNext;  // 0x08
    UISpritePoolInterface * m_ptPrev;  // 0x10
};
static_assert(sizeof(UISpritePoolInterface) == 0x18, "UISpritePoolInterface");

// X/Client/Sprite.cpp:2004
//   _Z18SpriteDrawDeferredRK17ImageTextureFrameS1_iRK6Rect2DRK9RGBAColorS7_ffPS3_11RenderLayeri
void SpriteDrawDeferred( const ImageTextureFrame & tImageTexFrame,
                         const ImageTextureFrame & tImageTexFrameMask,
                         const SNO snoShader,
                         const Rect2D & rectScreen,
                         const RGBAColor & rgbaColor,
                         const RGBAColor & rgbaAuxColor,
                         const float flZ,
                         const float flStereoDepth,
                         const Rect2D *const pwrClipping,
                         const RenderLayer eRenderLayerOverride,
                         BOOL fInRenderDraw );

// X/Client/Sprite.cpp:2167
//   _Z18SpriteDrawDeferredRK17ImageTextureFrameiRK6Rect2DRK9RGBAColorS7_fPS3_iS8_i11RenderLayeri
void SpriteDrawDeferred( const ImageTextureFrame & tImageTexFrame,
                         const SNO snoShader,
                         const Rect2D & rectScreen,
                         const RGBAColor & rgbaColor,
                         const RGBAColor & rgbaAuxColor,
                         const float flZ,
                         const Rect2D *const pwrClipping,
                         const SNO snoTexAlpha,
                         const Rect2D * pwrTexAlphaCoords,
                         const BOOL fFlipV,
                         const RenderLayer eRenderLayerOverride,
                         BOOL fInRenderDraw );

// X/Client/Sprite.cpp:3111
void SpriteDrawUISprite(UISprite * ptSprite, const float flZ, const RenderLayer eRenderLayer);

// X/Client/Sprite.cpp:2411
//   _Z26SpriteDrawDeferredMirroredRK17ImageTextureFrameiRK6Rect2DRK9RGBAColorS7_fPS3_iiiS8_i11RenderLayer
void SpriteDrawDeferredMirrored( const ImageTextureFrame & tImageTexFrame,
                                 const SNO snoShader,
                                 const Rect2D & rectScreen,
                                 const RGBAColor & rgbaColor,
                                 const RGBAColor & rgbaAuxColor,
                                 const float flZ,
                                 const Rect2D *const pwrClipping,
                                 const BOOL bMirrorX, const BOOL bMirrorY,
                                 const SNO snoTexAlpha,
                                 const Rect2D * pwrTexAlphaCoords,
                                 const BOOL fFlipV,
                                 const RenderLayer eRenderLayerOverride );
