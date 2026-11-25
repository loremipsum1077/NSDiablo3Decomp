#pragma once

#include "SigmaTypes.h"

struct Vector3D;
struct RGBAColorValue;

typedef int32 TextureFormat;

typedef uint8 Pixel8;

typedef uint16 Pixel16;

typedef uint32 Pixel32;

typedef uint32 UIColor;

struct RGBAColor {
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;

    RGBAColor() {}
    RGBAColor(const RGBAColor& rgba) { Set(rgba); }  // line 38
    RGBAColor(uint8 r, uint8 g, uint8 b) : r(r), g(g), b(b), a(0xff) {}  // line 39
    RGBAColor(uint8 r, uint8 g, uint8 b, uint8 a) : r(r), g(g), b(b), a(a) {}  // line 40

    BOOL operator==(const RGBAColor& rgba) const;
    BOOL operator!=(const RGBAColor& rgba) const;
    void Set(const uint8 _r, const uint8 _g, const uint8 _b, const uint8 _a);
    void Set(const RGBAColorValue& rgbaval);
    void Set(const RGBAColor& rgba);
    void Set(const UIColor& uicolor);
    UIColor ToUIColor() const;  // line 91
    RGBAColor& operator=(const RGBAColor& rgba) {  // line 58
        Set(rgba);
        return *this;
    }
    RGBAColor operator*(const RGBAColor& rgba) const;  // RGBAColor.inl:175
    void FromTexturePixel(const void* pColor, const TextureFormat eTextureFormat);
    Pixel32 ToTexturePixel(const TextureFormat eTextureFormat) const;
};

struct RGBAColorValue {
    float r;
    float g;
    float b;
    float a;
    float AsGrayScale() const;  // RGBAColor.inl:413
    RGBAColorValue() {}  // line 123
    RGBAColorValue(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}  // line 124
    void Set(const float _r, const float _g, const float _b, const float _a);  // RGBAColor.inl:359
    void Set(const RGBAColor & rgba);  // RGBAColor.inl:375
    RGBAColorValue operator+(const RGBAColorValue & l) const;  // RGBAColor.inl:220
    RGBAColorValue & operator+=(const RGBAColorValue & l);  // RGBAColor.inl:232
    RGBAColorValue operator-(const RGBAColorValue & l) const;  // RGBAColor.inl:244
    RGBAColorValue operator*(const RGBAColorValue & l) const;  // RGBAColor.inl:268
    RGBAColorValue operator*(const float flScale) const;  // RGBAColor.inl:292
    RGBAColorValue Scale(const float flScale) const;  // RGBAColor.inl:401
};

extern const RGBAColor crgbaWhite;
extern const RGBAColor crgbaGray;
extern const RGBAColor crgbaRed;
extern const RGBAColor crgbaGreen;
extern const RGBAColor crgbaBlue;
extern const RGBAColor crgbaYellow;
extern const RGBAColor crgbaOrange;
extern const RGBAColor crgbaPurple;
extern const RGBAColor crgbaGold;
extern const RGBAColor crgbaUIGold;
extern const RGBAColor crgbaBlack;
extern const RGBAColor crgbaUIOrange;
extern const RGBAColor unk_7100ed3648;
extern const RGBAColor crgbaDisabled;

extern const RGBAColorValue crgbavalNull;
extern const RGBAColorValue crgbavalOne;

#include "RGBAColor.inl"

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(RGBAColor) == 0x4, "RGBAColor");
static_assert(offsetof(RGBAColor, r) == 0x0, "RGBAColor");
static_assert(offsetof(RGBAColor, g) == 0x1, "RGBAColor");
static_assert(offsetof(RGBAColor, b) == 0x2, "RGBAColor");
static_assert(offsetof(RGBAColor, a) == 0x3, "RGBAColor");
static_assert(sizeof(RGBAColorValue) == 0x10, "RGBAColorValue");
static_assert(offsetof(RGBAColorValue, r) == 0x0, "RGBAColorValue");
static_assert(offsetof(RGBAColorValue, g) == 0x4, "RGBAColorValue");
static_assert(offsetof(RGBAColorValue, b) == 0x8, "RGBAColorValue");
static_assert(offsetof(RGBAColorValue, a) == 0xc, "RGBAColorValue");
