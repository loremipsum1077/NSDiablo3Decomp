// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/Gfx.h
//
// Trimmed to what GfxVB.cpp uses. The per-frame stats are kept as padding until something reads
// them.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/SigmaTypes.h"
#include "GfxCommon.h"

enum PerfBin {
    PERFBIN_BACKGROUND = 0,
    PERFBIN_UI = 1,
    PERFBIN_POSTFX = 2,
    PERFBIN_ACTORS = 3,
    PERFBIN_PARTICLES = 4,
    PERFBIN_OTHER = 5,
    NUM_PERFBINS = 6,
};

struct PerfBinStatsFrame {  // size 0xc
    int32 nDrawCalls;  // 0x00
    uint32 dwPolys;  // 0x04
    uint32 dwEstStaticTextureUsage;  // 0x08
};
static_assert(sizeof(PerfBinStatsFrame) == 0xc, "PerfBinStatsFrame");

struct GfxPerfStatsFrame {  // size 0x5c
    int32 nVBSwitches;  // 0x00
    int32 nShaderSwitches;  // 0x04
    int32 nTextureSwitches;  // 0x08
    int32 nBatchedTextureSwitches;  // 0x0c
    uint32 dwVBMemUsage;  // 0x10
    PerfBinStatsFrame tBinStats[6];  // 0x14
};
static_assert(sizeof(GfxPerfStatsFrame) == 0x5c, "GfxPerfStatsFrame");

struct GfxPerfStats {  // size 0xc0
    uint32 dwVBMemUsage;  // 0x00
    uint32 dwIBMemUsage;  // 0x04
    GfxPerfStatsFrame tCurrent;  // 0x08
    GfxPerfStatsFrame tLast;  // 0x64
};
static_assert(sizeof(GfxPerfStats) == 0xc0, "GfxPerfStats");

struct RenderTarget {  // size 0x8
    SNO snoRTTex;  // 0x00
    CubeMapFace eCubeMapFace;  // 0x04
};
static_assert(sizeof(RenderTarget) == 0x8, "RenderTarget");

struct Matrix4x4;
struct Quaternion;

// X/Client/Gfx/Gfx.cpp:1550
int32 GfxGetWorkerIndex();

// Defined in X/Client/Gfx/Gfx.cpp.
void GfxViewMatrixSet(const Matrix4x4 & m);
void GfxProjectionMatrixSet(const Matrix4x4 & m);
void GfxIrradMatrixSet(const Matrix4x4 & m);
void GfxCameraBillboardSet(const Quaternion & q);
// X/Client/Gfx/Gfx.cpp (X/Common/Game.cpp GameGetCustomCrashDescription).
const char * GfxGPUCrashDescription();

// Globals defined in the corresponding original .cpp.
struct Rect2D;
struct Vector2D;
extern const Rect2D cgrectViewportDefault;
extern const Vector2D cgvecViewportScaleDefault;

BOOL GfxStereo3DEnabled();  // Gfx.cpp:3898

struct Matrix4x4;
const Matrix4x4 & GfxViewMatrixGet();  // Gfx.cpp:2183
void GfxGetRenderTargetFloatDimensions(float & flWidth, float & flHeight);  // Gfx.cpp:3476
BOOL GfxCursorMakeResident();  // Gfx.cpp:3813
