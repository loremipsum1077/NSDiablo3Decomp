#include "Math/MathGeneral.h"

// Line 18. The body is not in the DWARF; its only local is dwC.
inline uint8 MultiplyAlpha(const uint8 byA, const uint8 byB) {
    uint32 dwC = (uint32)byA * (uint32)byB;
    return (uint8)((dwC * 0x10203) >> 24);
}

// Lines 30 and 42. The bodies are not in the DWARF; written as a per-channel compare.
inline BOOL RGBAColor::operator==(const RGBAColor& rgba) const {
    return r == rgba.r && g == rgba.g && b == rgba.b && a == rgba.a;
}

inline BOOL RGBAColor::operator!=(const RGBAColor& rgba) const {
    return r != rgba.r || g != rgba.g || b != rgba.b || a != rgba.a;
}

inline void RGBAColor::Set(const uint8 _r, const uint8 _g, const uint8 _b, const uint8 _a) {
    r = _r;
    g = _g;
    b = _b;
    a = _a;
}

// Line 91. The body is not in the DWARF; written from the copy inlined into FontGetColorString
// (X/Common/UserInterface/Font.cpp) and the console callers: the packed 0xAARRGGBB word.
inline UIColor RGBAColor::ToUIColor() const {
    return ((uint32)a << 24) | ((uint32)r << 16) | ((uint32)g << 8) | (uint32)b;
}

// Line 98. Written from the copy inlined into sParseText (X/Client/Font/FormattedText.cpp): the
// UIColor is 0xAARRGGBB.
inline void RGBAColor::Set(const UIColor& uicolor) {
    a = (uint8)(uicolor >> 24);
    r = (uint8)(uicolor >> 16);
    g = (uint8)(uicolor >> 8);
    b = (uint8)uicolor;
}

inline void RGBAColor::Set(const RGBAColorValue& rgbaval) {
    r = (uint8)Math::FloatRoundToInt(Math::Pin(rgbaval.r, 0.0f, 1.0f) * 255.0f);
    g = (uint8)Math::FloatRoundToInt(Math::Pin(rgbaval.g, 0.0f, 1.0f) * 255.0f);
    b = (uint8)Math::FloatRoundToInt(Math::Pin(rgbaval.b, 0.0f, 1.0f) * 255.0f);
    a = (uint8)Math::FloatRoundToInt(Math::Pin(rgbaval.a, 0.0f, 1.0f) * 255.0f);
}

// One 32-bit copy, not four byte copies: GfxVBFormatCopy's per-vertex copy loops only match
// with word-wide colour copies.
inline void RGBAColor::Set(const RGBAColor& rgba) {  // line 124
    *(uint32*)this = *(const uint32*)&rgba;
}

// Line 175. Body reconstructed from the two operator* copies inlined into
// X/Client/Animation2D.cpp (lines 147 and 158): four per-channel MultiplyAlpha calls.
inline RGBAColor RGBAColor::operator*(const RGBAColor& rgba) const {
    return RGBAColor(
        MultiplyAlpha(r, rgba.r),
        MultiplyAlpha(g, rgba.g),
        MultiplyAlpha(b, rgba.b),
        MultiplyAlpha(a, rgba.a));
}

// Line 189. Fixed-point lerp of each channel with 8 fractional bits (lines 193-213).
template <>
inline void Math::Lerp<RGBAColor>(const RGBAColor& rgba1, const RGBAColor& rgba2, const float flT, RGBAColor& rgbaResult) {
    int32 nT = (int32)(flT * 256.0f);
    int32 nBase;
    int32 nRange;

    nBase = rgba1.r;
    nRange = rgba2.r - nBase;
    rgbaResult.r = (uint8)(nBase + ((nRange * nT) >> 8));

    nBase = rgba1.g;
    nRange = rgba2.g - nBase;
    rgbaResult.g = (uint8)(nBase + ((nRange * nT) >> 8));

    nBase = rgba1.b;
    nRange = rgba2.b - nBase;
    rgbaResult.b = (uint8)(nBase + ((nRange * nT) >> 8));

    nBase = rgba1.a;
    nRange = rgba2.a - nBase;
    rgbaResult.a = (uint8)(nBase + ((nRange * nT) >> 8));
}

// Line 232 (statements at 235-238). Body written from the copy inlined into GfxShaderSetLights
// (X/Client/Gfx/GfxShader.cpp), its only inline site.
inline RGBAColorValue & RGBAColorValue::operator+=(const RGBAColorValue & l) {
    r += l.r;
    g += l.g;
    b += l.b;
    a += l.a;
    return *this;
}

// Line 268 (statements at 272-275). Body written from the copies inlined into GfxShaderSetLights.
inline RGBAColorValue RGBAColorValue::operator*(const RGBAColorValue & l) const {
    RGBAColorValue rgbavalResult;

    rgbavalResult.r = r * l.r;
    rgbavalResult.g = g * l.g;
    rgbavalResult.b = b * l.b;
    rgbavalResult.a = a * l.a;

    return rgbavalResult;
}

// Line 359. The body is not in the DWARF; written from TypeDescriptorClass_DT_RGBACOLORVALUE::
// LoadFromDefaultData (X/Sigma/TypeDescriptor/DescribedType.cpp).
inline void RGBAColorValue::Set(const float _r, const float _g, const float _b, const float _a) {
    r = _r;
    g = _g;
    b = _b;
    a = _a;
}

// Line 375. Body written from GfxFogSetParams (X/Client/Gfx/Gfx.cpp).
inline void RGBAColorValue::Set(const RGBAColor & rgba) {
    r = rgba.r * (1.0f / 255.0f);
    g = rgba.g * (1.0f / 255.0f);
    b = rgba.b * (1.0f / 255.0f);
    a = rgba.a * (1.0f / 255.0f);
}

// Lines 220, 244 and 292. Bodies written from the copies inlined into sEvaluateLightPaths
// (X/Client/Lights.cpp) through Math::Lerp<RGBAColorValue>.
inline RGBAColorValue RGBAColorValue::operator+(const RGBAColorValue & l) const {
    RGBAColorValue rgbavalResult;

    rgbavalResult.r = r + l.r;
    rgbavalResult.g = g + l.g;
    rgbavalResult.b = b + l.b;
    rgbavalResult.a = a + l.a;

    return rgbavalResult;
}

inline RGBAColorValue RGBAColorValue::operator-(const RGBAColorValue & l) const {
    RGBAColorValue rgbavalResult;

    rgbavalResult.r = r - l.r;
    rgbavalResult.g = g - l.g;
    rgbavalResult.b = b - l.b;
    rgbavalResult.a = a - l.a;

    return rgbavalResult;
}

inline RGBAColorValue RGBAColorValue::operator*(const float flScale) const {
    RGBAColorValue rgbavalResult = *this;

    rgbavalResult.r *= flScale;
    rgbavalResult.g *= flScale;
    rgbavalResult.b *= flScale;
    rgbavalResult.a *= flScale;

    return rgbavalResult;
}

// Line 401. Body written from sEvaluateInterpState (X/Client/Lights.cpp), its only inline site:
// the colour channels are scaled and alpha is kept.
inline RGBAColorValue RGBAColorValue::Scale(const float flScale) const {
    RGBAColorValue rgbavalResult = *this;

    rgbavalResult.r *= flScale;
    rgbavalResult.g *= flScale;
    rgbavalResult.b *= flScale;

    return rgbavalResult;
}

// Line 413. Body written from sGetHightlightColor (X/Client/RActors/RActorGfx.cpp:619), its
// inline site: the weights are the original's constants 0.222, 0.707 and 0.071.
inline float RGBAColorValue::AsGrayScale() const {
    return r * 0.222f + g * 0.707f + b * 0.071f;
}
