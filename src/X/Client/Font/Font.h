// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Font/Font.h
// Subset: TextRenderProperties and the two drawing entry points X/Client/CGame.cpp calls.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Gfx/GfxCommon.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"

class FormattedText;

class FontColorStack {  // size 0x24
public:
    FontColorStack() { m_nColorCount = 0; }  // line 29
    void Empty();
    void Push(const RGBAColor & rgbaColor);
    RGBAColor Pop();
    const RGBAColor & GetTop() const;
private:
    int32 m_nColorCount;  // 0x00
    RGBAColor m_rgbaColor[8];  // 0x04
};
static_assert(sizeof(FontColorStack) == 0x24, "FontColorStack");
extern FontColorStack cgtColorStack;  // Font.cpp:48
struct FontDefinition;
struct FontPointSizeData;

struct TextRenderProperties {  // size 0x1c
    TextRenderProperties(const SNO, const int32, const float, const float);
    SNO snoFont;  // 0x00
    int32 nFontSize;  // 0x04
    float flScale;  // 0x08
    float flAdvanceScalar;  // 0x0c
    BOOL fSnapToPixelCenterX;  // 0x10
    BOOL fSnapToPixelCenterY;  // 0x14
    float flStereoDepth;  // 0x18
};
static_assert(sizeof(TextRenderProperties) == 0x1c, "TextRenderProperties");

// Font.h:52. The body is not in the DWARF; written from the inlined copies (one store per member,
// lines 58-63).
inline TextRenderProperties::TextRenderProperties(const SNO snoFont, const int32 nFontSize, const float flScale, const float flAdvanceScalar)
    : snoFont(snoFont),
      nFontSize(nFontSize),
      flScale(flScale),
      flAdvanceScalar(flAdvanceScalar),
      fSnapToPixelCenterX(TRUE),
      fSnapToPixelCenterY(TRUE),
      flStereoDepth(1.0f) {}

// Defined in X/Client/Font/Font.cpp.
void FontStringDrawUIC(const TextRenderProperties & tProps, const FormattedText & tFormattedText, const Vector2D & vecPointUIC, const RGBAColor & rgbaColor, const RGBAColor * prgbaColorDropShadow, const float flZ, BOOL fAllowColor, const Rect2D * prectClipUIC, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
void FontStringGetRenderedSizeUIC(const TextRenderProperties & tProps, const FormattedText & tFormattedText, Vector2D & vecSizeUIC);
// Defined in X/Client/Font/Font.cpp; the DWARF gives the signatures of these.
float FontGetLineAdvanceUIC(const TextRenderProperties & tProps);
float FontGetLineAdvanceUIC(const TextRenderProperties & tProps, FontPointSizeData * ptPointSize);
float FontGetHeightUIC(const TextRenderProperties & tProps, int32 nLines);
float FontGetAscenderUIC(const TextRenderProperties & tProps, FontPointSizeData * ptPointSize);
float FontGetCursorHeightUIC(const TextRenderProperties & tProps);
float FontGetBulletIndentAdvanceUIC(const TextRenderProperties & tProps, int32 nBulletIndentCount);
int32 FontGetNextSmallestFontSize(SNO snoFont, int32 nFontSize);
void FontStringGetRenderedSizeUIC(const TextRenderProperties & tProps, const FormattedText & tFormattedText, Vector2D & vecSizeUIC, float * pflAscenderGlyphTopGapUIC, float * pflMaxAscenderGlyphTopGapUIC);
int32 FontStringGetPosWithinLine(const TextRenderProperties & tProps, const FormattedText & tFormattedText, const Vector2D & vecMousePixels);
void FontStringGetSelectedRegionUIC(const TextRenderProperties & tProps, const FormattedText & tFormattedText, const int32 nSelectionBegin, const int32 nSelectionEnd, Vector2D & vecSizeUIC, Vector2D & vecSelectionBeginUIC, Vector2D & vecSelectionEndUIC);
void FontSubstringDrawUIC(const TextRenderProperties & tProps, FontPointSizeData * ptPointSize, const FontDefinition * pdefFont, const FormattedText & tFormattedText, const Vector2D & vecPointUIC, const uint8 bAlpha, const RGBAColor * prgbaColorDropShadow, const BOOL bUseDropShadow, const float flZ, BOOL fAllowColor, const Rect2D * prectClipUIC, const RenderLayer eRenderLayerOverride);

// Font.cpp: the app subsystem entry points (X/Common/Game.cpp sgtAppSubsystems).
void FontInitializeForApp();
void FontDisposeForApp();

#line 86 "X/Client/Font/Font.h"
static const int32 FONT_DEFAULT_SIZE = 14;
