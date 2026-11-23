// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/SubObjectGfx.h
// Subset: the Renderable types, DisplayList with the members X/Client/CGame.cpp calls, and the
// SubObjectGfx entry points.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx/Gfx.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxMaterial.h"
#include "../Sigma/Math/Matrix.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/Math/Plane.h"
#include "../Sigma/SigmaTypes.h"

struct TexAnimState;
typedef int32 ClothID;
typedef int32 DyeType;

struct SubObjectState {  // size 0x48
    uint32 dwFlags;  // 0x00
    int32 nSubObjectLook;  // 0x04
    int32 nSubObjectLoadedLook;  // 0x08
    float flLookSwitchTime;  // 0x0c
    float flLastFadeCheckTime;  // 0x10
    BOOL fLastFadeCheckResult;  // 0x14
    SNO snoDefaultShader;  // 0x18
    SNO snoFadeShader;  // 0x1c
    Plane wpReflectionPlane;  // 0x20
    int32 nTexAnimStateCount;  // 0x30
    TexAnimState * arTexAnimStates;  // 0x38
    ClothID idCloth;  // 0x40
    DyeType eDyeType;  // 0x44
};
static_assert(sizeof(SubObjectState) == 0x48, "SubObjectState");

struct AppearanceDefinition;
struct BannerRenderData;
struct BillboardBucket;
class OcclusionHighlighting;
struct PackedVBEntry;
struct PackedIBEntry;
struct RopeCurvePoint;
struct ShaderDefinition;
struct SkinningInfo;
struct SubObject;
struct SubObjectAppearance;
struct TrailCurvePoint;
struct LightState;

typedef int32 OverlayType;
typedef int32 OverlayProjectionAxis;
typedef int32 TrailType;
typedef int32 IBID;
typedef int32 DataID;

enum RenderableType {
    RENDERABLETYPE_UNKNOWN = 0,
    RENDERABLETYPE_SCENE = 1,
    RENDERABLETYPE_RACTOR = 2,
    RENDERABLETYPE_ROPE = 3,
    RENDERABLETYPE_TRAIL = 4,
    RENDERABLETYPE_STENCIL_BOUNDS = 5,
    RENDERABLETYPE_BILLBOARDPARTICLE = 6,
};

struct RenderableStencilBound {  // size 0x18
    float wp[3];  // 0x00
    float wvExt[3];  // 0x0c
};
static_assert(sizeof(RenderableStencilBound) == 0x18, "RenderableStencilBound");

struct RenderableParticle {  // size 0x30
    BillboardBucket * ptBucket;  // 0x00
    uint32 dwBillboardFlags;  // 0x08
    uint32 dwVertexStartIndex;  // 0x0c
    uint32 dwVertexCount;  // 0x10
    SNO snoTex[4];  // 0x14
    int32 nNumInstances;  // 0x24
    const Vector3D * pwvInstanceOffsets;  // 0x28
};
static_assert(sizeof(RenderableParticle) == 0x30, "RenderableParticle");

struct RenderableRope {  // size 0x20
    SNO snoRope;  // 0x00
    int32 nVertCount;  // 0x04
    int32 nCurvePoints;  // 0x08
    RopeCurvePoint * ptCurvePoints;  // 0x10
    TexAnimState * arTexAnimStates;  // 0x18
};
static_assert(sizeof(RenderableRope) == 0x20, "RenderableRope");

struct RenderableTrail {  // size 0x40
    SNO snoTrail;  // 0x00
    IBID ibid;  // 0x04
    int32 nCurvePoints;  // 0x08
    int32 nCircleSegments;  // 0x0c
    BOOL fEdgeAlpha;  // 0x10
    TrailType nTrailType;  // 0x14
    TrailCurvePoint * ptCurvePoints;  // 0x18
    Vector3D * wvCurvePointPerpendicular;  // 0x20
    Vector3D * wvCurvePointAlong;  // 0x28
    Vector3D * wvCurvePointForward;  // 0x30
    TexAnimState * arTexAnimStates;  // 0x38
};
static_assert(sizeof(RenderableTrail) == 0x40, "RenderableTrail");

struct RenderableAppearance {  // size 0xb8
    float flRand;  // 0x00
    SNO snoIrradianceTex;  // 0x04
    SNO snoAmbientOcclusionOverrideTex;  // 0x08
    OverlayType eOverlayType;  // 0x0c
    OverlayProjectionAxis eOverlayProjectionAxis;  // 0x10
    uint32 dwOverlayFadeTimeCurrent;  // 0x14
    uint32 dwOverlayFadeTimeOriginal;  // 0x18
    float rgbavalHitFlashAdd[3];  // 0x1c
    float rgbavalHitFlashMul[3];  // 0x28
    int32 nSubObjectCount;  // 0x34
    int32 nSubObjectIndex;  // 0x38
    const SubObjectAppearance * ptSOA;  // 0x40
    const SubObject * ptSubObject;  // 0x48
    float flLookSwitchTime;  // 0x50
    TexAnimState * arTexAnimStates;  // 0x58
    ClothID idCloth;  // 0x60
    DyeType eDyeType;  // 0x64
    int32 nTexAnimStateCount;  // 0x68
    PackedVBEntry * ptEntry;  // 0x70
    PackedIBEntry * ptEntryIB;  // 0x78
    SkinningInfo * ptBoneMapping;  // 0x80
    int32 nNumMatrices;  // 0x88
    float * pmMatrixPalette;  // 0x90
    SNO snoAppearance;  // 0x98
    const AppearanceDefinition * pdefAppearance;  // 0xa0
    BannerRenderData * ptBannerRenderData;  // 0xa8
    OcclusionHighlighting * ptOcclusionHighlighting;  // 0xb0
};
static_assert(sizeof(RenderableAppearance) == 0xb8, "RenderableAppearance");

struct RenderableCommon {  // size 0x118
    RenderableType eType;  // 0x00
    DataID id;  // 0x04
    PerfBin ePerfBin;  // 0x08
    Material mat;  // 0x0c
    Matrix4x4 mWorld;  // 0x54
    int32 nLightCount;  // 0x94
    const LightState * pArrayLights[16];  // 0x98
};
static_assert(sizeof(RenderableCommon) == 0x118, "RenderableCommon");

class RenderableDetails {  // size 0x1d0
public:
    RenderableDetails(const RenderableType eType, const DataID id, const PerfBin ePerfBin, const Material & mat);
    RenderableCommon tCommon;  // 0x00
    union {  // size 0xb8
        RenderableParticle tParticle;  // 0x00
        RenderableAppearance tAppearance;  // 0x00
        RenderableStencilBound tStencilBound;  // 0x00
        RenderableRope tRope;  // 0x00
        RenderableTrail tTrail;  // 0x00
    };  // 0x118
};
static_assert(sizeof(RenderableDetails) == 0x1d0, "RenderableDetails");

class RenderableHeader {  // size 0x30
public:
    RenderableHeader(const ShaderDefinition * pdefShader, const Material & mat);
    const ShaderDefinition * pdefShader;  // 0x00
    int16 nSortPri : 11;  // 0x08
    uint16 fEarlyZDisabled : 1;  // 0x08
    uint16 fUseHighlight : 1;  // 0x08
    uint16 fIsWall : 1;  // 0x08
    uint16 fInPrepass : 1;  // 0x08
    uint16 fSetShadowLevel : 1;  // 0x08
    int16 nRenderableIndex;  // 0x0a
    RGBAColor rgbaColor;  // 0x0c
    uintptr nGroup;  // 0x10
    void SetDist(float flDist);
    void SetRect(const Rect2D & rect);
    void SetRectFromPoint(const Vector3D & wpCenter);
    int32 nDist;  // 0x18
    IRect2D irectDist;  // 0x1c
};
static_assert(sizeof(RenderableHeader) == 0x30, "RenderableHeader");

class Renderable {  // size 0x200
public:
    Renderable(const RenderableType eType, const DataID id, const PerfBin ePerfBin, const ShaderDefinition * pdefShader, const Material & mat);
    RenderableHeader header;  // 0x00
    RenderableDetails details;  // 0x30
};
static_assert(sizeof(Renderable) == 0x200, "Renderable");

struct RenderLayerData;
class RWindow;

class DisplayList {  // size 0x30
public:
    void Initialize(const RenderContext eContext, const BOOL fSimple);
    void Dispose();
    void Compact();
    void SetRenderContext(const RenderContext eContext);
    RenderContext GetRenderContext() const;
    void RenderBegin();
    void SubmitRenderable(const Renderable & tRenderable);
    int32 SubmitRenderableDetails(const RenderableDetails & tRenderableDetails);
    void SubmitRenderableHeader(const RenderableHeader & tRenderableHeader, int32 nRenderableIndex);
    void Sort();
    void RenderEnd();
    void DrawRenderLayer(const RenderLayer eRenderLayer) const;
    void DrawBoundingStencil(const RenderLayer eRenderLayer) const;
    BOOL IsRenderLayerEmpty(const RenderLayer eRenderLayer) const;
    RenderLayerData * ptRenderLayerData;  // 0x00
    RenderableDetails * ptRenderableDetailsList;  // 0x08
private:
    void sResizeVisibleSubObjectHeaderList(const RenderLayer eRenderLayer);
    void sResizeVisibleSubObjectDetailsList();
    RenderContext m_eContext;  // 0x10
    BOOL m_fSimple;  // 0x14
    Blizzard::Lock::Spinlock m_tLock;  // 0x18
    int32 nMaxRenderableDetailsCount;  // 0x28
    int32 nRenderableDetailsCount;  // 0x2c
};
static_assert(sizeof(DisplayList) == 0x30, "DisplayList");

inline void DisplayList::SetRenderContext(const RenderContext eContext) {  // line 308
    m_eContext = eContext;
}

inline RenderContext DisplayList::GetRenderContext() const {  // line 313
    return m_eContext;
}

// Defined in X/Client/SubObjectGfx.cpp.
BOOL SubObjectGfxInit();
void SubObjectGfxDispose();
void SubObjectGfxUpdate(const float flDeltaTime, const float flClientTime);
void SubObjectGfxPrepareRender();
void SubObjectGfxDebugDepthDrawDeferred(RWindow & tRWindow);
void SubObjectSetLocalPlayerIsGhosted(BOOL fGhosted);
BOOL SubObjectGetLocalPlayerIsGhosted();
void SubObjectGfxOcclusionHighlightingSetEnabled(BOOL bEnabled);
void SubObjectGfxOcclusionHighlightingFrameEnd();
void SubObjectGfxOcclusionHighlightingClearQueries();
void DisplayListBuildProjectionLayers(RWindow & tRWindow);
void DisplayListDrawProjectionLayers(RWindow & tRWindow);
void SubObjectGfxGetShadowDepthData(SNO & snoShadowTex, Matrix4x4 & shadowMat);

// Globals defined in the corresponding original .cpp.
extern const Vector2D cgvecPMAOffset;
extern const WorldDistance cwdPMAMapWidth;
extern const WorldDistance cwdRippleMapWidth;
extern const Matrix4x4 cmSceneScaleTweak;

enum { SUBOBJECTSTATE_RENDER_BIT = 1 };  // SubObjectGfx.h:54

enum { SUBOBJECTSTATE_CAN_BE_RENDERED_BIT = 0 };  // SubObjectGfx.h:53

enum { SUBOBJECTSTATE_TEXURES_LOADED_BIT = 4 };  // SubObjectGfx.h:57

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(SubObjectState) == 0x48, "SubObjectState");
static_assert(offsetof(SubObjectState, dwFlags) == 0x0, "SubObjectState");
static_assert(offsetof(SubObjectState, nSubObjectLook) == 0x4, "SubObjectState");
static_assert(offsetof(SubObjectState, nSubObjectLoadedLook) == 0x8, "SubObjectState");
static_assert(offsetof(SubObjectState, flLookSwitchTime) == 0xc, "SubObjectState");
static_assert(offsetof(SubObjectState, flLastFadeCheckTime) == 0x10, "SubObjectState");
static_assert(offsetof(SubObjectState, fLastFadeCheckResult) == 0x14, "SubObjectState");
static_assert(offsetof(SubObjectState, snoDefaultShader) == 0x18, "SubObjectState");
static_assert(offsetof(SubObjectState, snoFadeShader) == 0x1c, "SubObjectState");
static_assert(offsetof(SubObjectState, wpReflectionPlane) == 0x20, "SubObjectState");
static_assert(offsetof(SubObjectState, nTexAnimStateCount) == 0x30, "SubObjectState");
static_assert(offsetof(SubObjectState, arTexAnimStates) == 0x38, "SubObjectState");
static_assert(offsetof(SubObjectState, idCloth) == 0x40, "SubObjectState");
static_assert(offsetof(SubObjectState, eDyeType) == 0x44, "SubObjectState");
static_assert(sizeof(RenderableStencilBound) == 0x18, "RenderableStencilBound");
static_assert(offsetof(RenderableStencilBound, wp) == 0x0, "RenderableStencilBound");
static_assert(offsetof(RenderableStencilBound, wvExt) == 0xc, "RenderableStencilBound");
static_assert(sizeof(RenderableParticle) == 0x30, "RenderableParticle");
static_assert(offsetof(RenderableParticle, ptBucket) == 0x0, "RenderableParticle");
static_assert(offsetof(RenderableParticle, dwBillboardFlags) == 0x8, "RenderableParticle");
static_assert(offsetof(RenderableParticle, dwVertexStartIndex) == 0xc, "RenderableParticle");
static_assert(offsetof(RenderableParticle, dwVertexCount) == 0x10, "RenderableParticle");
static_assert(offsetof(RenderableParticle, snoTex) == 0x14, "RenderableParticle");
static_assert(offsetof(RenderableParticle, nNumInstances) == 0x24, "RenderableParticle");
static_assert(offsetof(RenderableParticle, pwvInstanceOffsets) == 0x28, "RenderableParticle");
static_assert(sizeof(RenderableRope) == 0x20, "RenderableRope");
static_assert(offsetof(RenderableRope, snoRope) == 0x0, "RenderableRope");
static_assert(offsetof(RenderableRope, nVertCount) == 0x4, "RenderableRope");
static_assert(offsetof(RenderableRope, nCurvePoints) == 0x8, "RenderableRope");
static_assert(offsetof(RenderableRope, ptCurvePoints) == 0x10, "RenderableRope");
static_assert(offsetof(RenderableRope, arTexAnimStates) == 0x18, "RenderableRope");
static_assert(sizeof(RenderableTrail) == 0x40, "RenderableTrail");
static_assert(offsetof(RenderableTrail, snoTrail) == 0x0, "RenderableTrail");
static_assert(offsetof(RenderableTrail, ibid) == 0x4, "RenderableTrail");
static_assert(offsetof(RenderableTrail, nCurvePoints) == 0x8, "RenderableTrail");
static_assert(offsetof(RenderableTrail, nCircleSegments) == 0xc, "RenderableTrail");
static_assert(offsetof(RenderableTrail, fEdgeAlpha) == 0x10, "RenderableTrail");
static_assert(offsetof(RenderableTrail, nTrailType) == 0x14, "RenderableTrail");
static_assert(offsetof(RenderableTrail, ptCurvePoints) == 0x18, "RenderableTrail");
static_assert(offsetof(RenderableTrail, wvCurvePointPerpendicular) == 0x20, "RenderableTrail");
static_assert(offsetof(RenderableTrail, wvCurvePointAlong) == 0x28, "RenderableTrail");
static_assert(offsetof(RenderableTrail, wvCurvePointForward) == 0x30, "RenderableTrail");
static_assert(offsetof(RenderableTrail, arTexAnimStates) == 0x38, "RenderableTrail");
static_assert(sizeof(RenderableAppearance) == 0xb8, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, flRand) == 0x0, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, snoIrradianceTex) == 0x4, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, snoAmbientOcclusionOverrideTex) == 0x8, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, eOverlayType) == 0xc, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, eOverlayProjectionAxis) == 0x10, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, dwOverlayFadeTimeCurrent) == 0x14, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, dwOverlayFadeTimeOriginal) == 0x18, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, rgbavalHitFlashAdd) == 0x1c, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, rgbavalHitFlashMul) == 0x28, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, nSubObjectCount) == 0x34, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, nSubObjectIndex) == 0x38, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, ptSOA) == 0x40, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, ptSubObject) == 0x48, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, flLookSwitchTime) == 0x50, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, arTexAnimStates) == 0x58, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, idCloth) == 0x60, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, eDyeType) == 0x64, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, nTexAnimStateCount) == 0x68, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, ptEntry) == 0x70, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, ptEntryIB) == 0x78, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, ptBoneMapping) == 0x80, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, nNumMatrices) == 0x88, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, pmMatrixPalette) == 0x90, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, snoAppearance) == 0x98, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, pdefAppearance) == 0xa0, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, ptBannerRenderData) == 0xa8, "RenderableAppearance");
static_assert(offsetof(RenderableAppearance, ptOcclusionHighlighting) == 0xb0, "RenderableAppearance");
static_assert(sizeof(RenderableCommon) == 0x118, "RenderableCommon");
static_assert(offsetof(RenderableCommon, eType) == 0x0, "RenderableCommon");
static_assert(offsetof(RenderableCommon, id) == 0x4, "RenderableCommon");
static_assert(offsetof(RenderableCommon, ePerfBin) == 0x8, "RenderableCommon");
static_assert(offsetof(RenderableCommon, mat) == 0xc, "RenderableCommon");
static_assert(offsetof(RenderableCommon, mWorld) == 0x54, "RenderableCommon");
static_assert(offsetof(RenderableCommon, nLightCount) == 0x94, "RenderableCommon");
static_assert(offsetof(RenderableCommon, pArrayLights) == 0x98, "RenderableCommon");
static_assert(sizeof(RenderableDetails) == 0x1d0, "RenderableDetails");
static_assert(offsetof(RenderableDetails, tCommon) == 0x0, "RenderableDetails");
static_assert(sizeof(RenderableHeader) == 0x30, "RenderableHeader");
static_assert(offsetof(RenderableHeader, pdefShader) == 0x0, "RenderableHeader");
static_assert(offsetof(RenderableHeader, nRenderableIndex) == 0xa, "RenderableHeader");
static_assert(offsetof(RenderableHeader, rgbaColor) == 0xc, "RenderableHeader");
static_assert(offsetof(RenderableHeader, nGroup) == 0x10, "RenderableHeader");
static_assert(offsetof(RenderableHeader, nDist) == 0x18, "RenderableHeader");
static_assert(offsetof(RenderableHeader, irectDist) == 0x1c, "RenderableHeader");
static_assert(sizeof(Renderable) == 0x200, "Renderable");
static_assert(offsetof(Renderable, header) == 0x0, "Renderable");
static_assert(offsetof(Renderable, details) == 0x30, "Renderable");
static_assert(sizeof(DisplayList) == 0x30, "DisplayList");
