// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/PostFX.h
// Subset: the reflection-plane entry points X/Client/Camera/CameraManager.cpp calls.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxVB.h"
#include "../Sigma/RGBAColor.h"

struct Plane;

// Defined in X/Client/PostFX.cpp:2347 and :2357.
int32 PostFXGetNumReflectionPlanes();
Plane & PostFXGetReflectionPlane(int32 nPlaneIndex);

enum EffectBuffer {
    EFFECTBUFFER_NONE = -1,
    EFFECTBUFFER_SCREEN_PING = 0,
    EFFECTBUFFER_SCREEN_PONG = 1,
    EFFECTBUFFER_BLUR_DISTORTION = 2,
    EFFECTBUFFER_SCREEN_HALF_PING = 3,
    EFFECTBUFFER_REFLECTION_0 = 4,
    EFFECTBUFFER_REFLECTION_1 = 5,
    EFFECTBUFFER_REFLECTION_DEPTH = 6,
    EFFECTBUFFER_REFRACTION = 7,
    EFFECTBUFFER_HDR = 8,
    EFFECTBUFFER_BACKBUFFER = 9,
    EFFECTBUFFER_DEPTH = 10,
    EFFECTBUFFER_DEPTH_PREV = 11,
    EFFECTBUFFER_BLOOM_PONG = 12,
    EFFECTBUFFER_SCREEN_PONG_X360_MINUS1 = 13,
    EFFECTBUFFER_SCREEN_PONG_X360_MINUS2 = 14,
    EFFECTBUFFER_COLOR = 15,
    EFFECTBUFFER_BACKBUFFER_DEPTH = 16,
    EFFECTBUFFER_HIGHLIGHT = 17,
    EFFECTBUFFER_MSAA_COLOR = 18,
    EFFECTBUFFER_MSAA_DEPTH = 19,
    EFFECTBUFFER_MSAA_COLOR_FMASK = 20,
    EFFECTBUFFER_SCREEN_PING_LDR = 21,
    EFFECTBUFFER_SCREEN_PONG_LDR = 22,
    EFFECTBUFFER_DEFAULT_SOFTPARTICLE_DEPTH = 23,
    EFFECTBUFFER_SSAO_NORMAL = 24,
    EFFECTBUFFER_SSAO = 25,
    EFFECTBUFFER_SSAO_PING = 26,
    EFFECTBUFFER_SSAO_PONG = 27,
    NUM_EFFECT_BUFFERS = 28,
};

enum {
    POSTFX_FRAME_HAS_SOMETHING_DISTORTING = 0,
    POSTFX_FRAME_GHOSTED = 1,
    POSTFX_FRAME_DISABLE_TEXT_HIGHLIGHT = 2,
};

struct BlurFilter {  // size 0x60
    UVSet uvSample[4];  // 0x00
    ShaderFloat tWeight[4];  // 0x20
};
static_assert(sizeof(BlurFilter) == 0x60, "BlurFilter");

typedef void (*FadeCallback)(float);

struct PostFXFadeParams {  // size 0x20
    float flFadeTimeInSeconds;  // 0x00
    RGBAColor rgbaColor;  // 0x04
    float flStartAlpha;  // 0x08
    float flEndAlpha;  // 0x0c
    float flZ;  // 0x10
    BOOL fKeepOn;  // 0x14
    FadeCallback pfnFadeCallback;  // 0x18
};
static_assert(sizeof(PostFXFadeParams) == 0x20, "PostFXFadeParams");

LPCSTR PostFXGetEffectRTDebugName(int32 nType);
void PostFXFilterRender(const SNO snoShader, const SNO snoTexture);
void PostFXFilterRender(const SNO snoShader, const SNO snoTexture, const BlurFilter & tFilter, BOOL bNullPixelShader);
void PostFXFilterRender(const SNO snoShader, const SNO snoTexture0, const SNO snoTexture1, const SNO snoTexture2, const SNO snoTexture3);
SNO PostFXGetBuffer(const EffectBuffer eBuffer);
SNO PostFXGetDepthBuffer(const EffectBuffer eBuffer);
BOOL PostFXGetFrameFlag(const uint32 dwBit);
void PostFXSetFrameFlag(const uint32 dwBit, const BOOL fValue);
void PostFXFadeStart(const PostFXFadeParams & tFadeParams);
