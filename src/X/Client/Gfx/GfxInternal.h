// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/GfxInternal.h
//
// Trimmed: members whose types src/ does not have yet are padding with the original member
// names beside them, so every offset stays exact.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx.h"
#include "GfxAPI.h"
#include "GfxCommon.h"
#include "GfxVB.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"

struct IndexBuffer;
struct VertexBuffer;
class FileOutputStream;
struct ShaderConstFastBinding;
struct ShaderData;
struct TextureBinding;
class TriangleList2D;

typedef int32 IBID;
typedef int32 GlobalSNO;  // X/Common/GlobalSNO.h

enum {
    GFXINTERNAL_WINDOW_OPEN_BIT = 0,
    GFXINTERNAL_FORCED_RESET_BIT = 1,
    GFXINTERNAL_HARD_RESET_BIT = 2,
    GFXINTERNAL_FORCE_WIREFRAME_BIT = 3,
    GFXINTERNAL_TEXMEM_DUMP_BIT = 4,
    GFXINTERNAL_LETTERBOX_BIT = 5,
    GFXINTERNAL_PREFS_CHANGED_BIT = 6,
    GFXINTERNAL_SOFTWARE_CURSOR_BIT = 7,
    GFXINTERNAL_RENDERING_DISABLED_BIT = 8,
    GFXINTERNAL_GAMMA_CHANGED_BIT = 9,
    GFXINTERNAL_PRESENT_FAILED_BIT = 10,
};

struct RenderState {  // size 0x1f28
    XPtr64<unsigned char> ptStateBlock;  // 0x00
    BOOL fFogEnabled;  // 0x08
    SNO snoShader;  // 0x0c
    int32 nPassIndex;  // 0x10
    SNO snoTex[24];  // 0x14
    SNO snoLightMapTex;  // 0x74
    IBID ibid;  // 0x78
    uint32 dwIndexOffset;  // 0x7c
    VBID vbid[3];  // 0x80
    uint32 dwStreamOffset[3];  // 0x8c
    RGBAColor rgbaFogColor;  // 0x98
    RGBAColorValue rgbaFogColorValue;  // 0x9c
    float flFogStart;  // 0xac
    float flFogEnd;  // 0xb0
    float flHeightFogStart;  // 0xb4
    float flHeightFogEnd;  // 0xb8
    RGBAColor rgbaAlphaTestRef;  // 0xbc
    Vector4D vecViewportScaleBias;  // 0xc0
    Vector3D vecFixedPointPosScale;  // 0xd0
    Vector3D vecFixedPointPosOffset;  // 0xdc
    Matrix4x4 mWorld;  // 0xe8
    Matrix4x4 mView;  // 0x128
    Matrix4x4 mWorldView;  // 0x168
    Matrix4x4 mProjection;  // 0x1a8
    Matrix4x4 mIrrad;  // 0x1e8
    Matrix4x4 mWorldTranspose;  // 0x228
    Matrix4x4 mViewTranspose;  // 0x268
    Matrix4x4 mWorldViewTranspose;  // 0x2a8
    Matrix4x4 mWorldViewProjectionTranspose;  // 0x2e8
    Matrix4x4 mProjectionTranspose;  // 0x328
    Matrix4x4 mIrradTranspose;  // 0x368
    Matrix4x4 mTex[12];  // 0x3a8
    float flZBias;  // 0x6a8
    Quaternion qBillboard;  // 0x6ac
    BOOL fTexMatrixModified[12];  // 0x6bc
    uint32 dwStageCount;  // 0x6ec
    RGBAColor rgbaTexFactor;  // 0x6f0
    RGBAColorValue rgbaTexFactorValue;  // 0x6f4
    CullMode eCullMode;  // 0x704
    BOOL fHWFogEnable;  // 0x708
    int32 nNumBones;  // 0x70c
    DirectionalLight tDirLightList[16];  // 0x710
    PointLight tPointLightList[16];  // 0xa10
    SpotLight tSpotLightList[16];  // 0xf10
    CylindricalLight tCylLightList[16];  // 0x1510
    PointLinearLight tPointLinearLightList[16];  // 0x1b10
    int32 nNonNullDirLightCount;  // 0x1f10
    int32 nNonNullPointLightCount;  // 0x1f14
    int32 nNonNullSpotLightCount;  // 0x1f18
    int32 nNonNullCylindricalLightCount;  // 0x1f1c
    int32 nNonNullPointLinearLightCount;  // 0x1f20
};
static_assert(sizeof(RenderState) == 0x1f28, "RenderState");

struct Line3DShaderBatch {  // size 0x10
    int32 nStartLineVertexIndex;  // 0x00
    int32 nEndLineVertexIndex;  // 0x04
    GlobalSNO snoShader;  // 0x08
    RenderContext eContext;  // 0x0c
    int32 GetVertexCount() { return nEndLineVertexIndex - nStartLineVertexIndex + 1; }  // line 140
};
static_assert(sizeof(Line3DShaderBatch) == 0x10, "Line3DShaderBatch");

struct Line2DShaderBatch {  // size 0xc
    int32 nStartLineVertexIndex;  // 0x00
    int32 nEndLineVertexIndex;  // 0x04
    RenderContext eContext;  // 0x08
    int32 GetVertexCount() { return nEndLineVertexIndex - nStartLineVertexIndex + 1; }  // line 152
};
static_assert(sizeof(Line2DShaderBatch) == 0xc, "Line2DShaderBatch");

struct TriOverlayShaderBatch {  // size 0x10
    int32 nStart;  // 0x00
    int32 nEnd;  // 0x04
    GlobalSNO snoShader;  // 0x08
    RenderContext eContext;  // 0x0c
    int32 GetVertexCount();
};
static_assert(sizeof(TriOverlayShaderBatch) == 0x10, "TriOverlayShaderBatch");

typedef blz::vector<VertexNoTexL, blz::allocator<VertexNoTexL> > VertexArray;
typedef XGrowableList<Line2DShaderBatch, Line2DShaderBatch &> Line2DShaderBatchList;
typedef XGrowableList<Line3DShaderBatch, Line3DShaderBatch &> Line3DShaderBatchList;
typedef blz::vector<Vertex1TexL, blz::allocator<Vertex1TexL> > TriOverlayVertexArray;
typedef XGrowableList<TriOverlayShaderBatch, TriOverlayShaderBatch &> TriOverlayShaderBatchList;

struct GfxRenderData {  // size 0x168
    VertexArray tLineList2DVertices;  // 0x00
    Line3DShaderBatchList tLine3DShaderBatches;  // 0x18
    Line2DShaderBatchList tLine2DShaderBatches;  // 0x70
    VertexArray tLineList3DVertices;  // 0xc8
    VertexArray tPointList3DVertices;  // 0xe0
    TriOverlayVertexArray tTriOverlayVertices;  // 0xf8
    TriOverlayShaderBatchList tTriOverlayShaderBatches;  // 0x110
};
static_assert(sizeof(GfxRenderData) == 0x168, "GfxRenderData");

struct RenderWorkerData {  // size 0x4010
    RenderState tRenderState;  // 0x00
    RenderState tRenderStateLast;  // 0x1f28
    ShaderTex eShaderTex[24];  // 0x3e50
    TextureBinding * ptTexBinding;  // 0x3eb0
    GfxPerfStats tStats;  // 0x3eb8
    RenderTarget tCurrentRTs[2];  // 0x3f78
    uint32 dwRTHeight;  // 0x3f88
    uint32 dwRTWidth;  // 0x3f8c
    float flRTHeight;  // 0x3f90
    float flRTWidth;  // 0x3f94
    float flRTInvHeight;  // 0x3f98
    float flRTInvWidth;  // 0x3f9c
    SNO snoCurrentDepthTarget;  // 0x3fa0
    uint32 dwNumBatches;  // 0x3fa4
    float flClipPlane[24];  // 0x3fa8
    const ShaderConstFastBinding * ptShaderConstantFastBindings;  // 0x4008
};
static_assert(sizeof(RenderWorkerData) == 0x4010, "RenderWorkerData");

struct GfxInternalData {  // size 0x4838
    uint32 dwFlags;  // 0x00
    DisplayMode * ptModes;  // 0x08
    uint32 dwModeCount;  // 0x10
    BOOL fTextureFormatValid[3][44];  // 0x14
    DisplayMode tCurrentMode;  // 0x224
    DisplayCaps tCaps;  // 0x25c
    GfxRenderData tRenderData[2];  // 0x270
    VBID vbidLineList2D;  // 0x540
    VBID vbidLineList3D;  // 0x544
    VBID vbidPointList3D;  // 0x548
    VBID vbidTriOverlay;  // 0x54c
    VBID vbidTriList2D;  // 0x550
    TriangleList2D * ptTriList2D;  // 0x558
    RenderWorkerData workerData[1];  // 0x560
    GfxPerfStats tGlobalStats;  // 0x4570
    GfxOptions tGfxOptions;  // 0x4630
    uint32 dwPSConstantsSize;  // 0x4638
    uint32 dwVSConstantsSize;  // 0x463c
    TChainedDataArray<IndexBuffer, int> * ptIBList;  // 0x4640
    TChainedDataArray<VertexBuffer, int> * ptVBList;  // 0x4648
    XGrowableList<int, int> tDynIBList;  // 0x4650
    XGrowableList<int, int> tDynVBList;  // 0x46a8
    Blizzard::Lock::Mutex tIBListMutex;  // 0x4700
    Blizzard::Lock::Mutex tVBListMutex;  // 0x4720
    uint32 dwNumPresents;  // 0x4740
    float flModeHeight;  // 0x4744
    float flModeWidth;  // 0x4748
    SNO snoSwapChainRenderTarget;  // 0x474c
    SNO snoTexCursor;  // 0x4750
    uint32 dwTexCursorFrame;  // 0x4754
    GfxAPI eGraphicsSystem;  // 0x4758
    GfxAPI eNewGraphicsSystem;  // 0x475c
    XGrowableList<ShaderData *, ShaderData *> tVSCache;  // 0x4760
    XGrowableList<ShaderData *, ShaderData *> tPSCache;  // 0x47b8
    FileOutputStream * ptLogStream;  // 0x4810
    float flReflectionPlane[8];  // 0x4818
};
static_assert(sizeof(GfxInternalData) == 0x4838, "GfxInternalData");

// X/Client/Gfx/Gfx.cpp:96
extern GfxInternalData * gptGfxData;
