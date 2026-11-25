// X/Client/SubObjectGfx.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/p2-render/sog/frag.cpp by build/try/p2-render/gen.py.
#include "Definitions/TextureDefinition.h"
#include <limits.h>
#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "SubObjectGfx.h"
#include "CGame.h"
#include "PostFX.h"
#include "RWindow.h"
#include "Sprite.h"
#include "TexAnimation.h"
#include "Definitions/AppearanceDefinition.h"
#include "../Common/GlobalSNO.h"
#include "../Sigma/Random.h"
#include "Gfx/Gfx.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxMaterial.h"
#include "Gfx/GfxIB.h"
#include "Gfx/GfxVB.h"
#include "Definitions/ShaderDefinition.h"
#include "../Sigma/DataStructures/XArray.h"
#include "../Sigma/DataStructures/XFixedBlockAllocator.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Matrix.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RGBAColor.h"
#include "../Common/Game.h"
struct OcclusionQuery;
struct RActor;
#include <new>
uint32 GfxGetShadowTech();
void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight);  // Gfx/Gfx.cpp:1854
// X/Client/TexAnimation.cpp:275
uint32 TexAnimUpdate(const UberMaterial & tUberMaterial, const ObjectTexAnimParam * ptObjTexAnimParam, TexAnimState * ptTexAnimState, const int32 nTexAnimStateCount, float flDeltaTime, float flClientTime);
#include "Gfx/GfxShader.h"
#include "Camera/CameraManager.h"
#include "Camera/Camera.h"
#include "Gfx/GfxAPI.h"
#include "../Common/TagMaps/Tags.h"
#include "../Common/TagMaps/VertexShaderTags.h"
#include "Definitions/ShaderMapDefinition.h"
#include "../Common/TagMaps/AppearanceTags.h"
// X/Client/Gfx/Gfx.cpp:2053, 3242, 3349, 3414, 3440, 3451 and X/Client/Gfx/GfxShader.cpp:409, 800, 1491, 1553 (src Gfx.h does not declare them)
void GfxWorldMatrixSet( const Matrix4x4 & m );
void GfxSetRenderTargets( const RenderTarget * tRenderTargets );
void GfxSetDepthTarget( const SNO sno );
void GfxSetRenderAndDepthTarget( const SNO snoRenderTarget, const SNO snoDepthTarget, const CubeMapFace eCubeMapFace );
void GfxGetRenderTargets( RenderTarget * tRenderTargets );
SNO GfxGetDepthTarget();
void GfxShaderInit( const ShaderDefinition * pdefShader, const int32 nPassIndex );
void GfxShaderDraw( const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount, const PerfBin ePerfBin );
void GfxShaderSetStream( const StreamIndex eStreamIndex, const uint32 dwStreamOffset, const VBID vbid );
void GfxShaderSetIB( const IBID ibid, const uint32 dwIndexOffset );
struct RenderableHeader;
struct RenderableDetails;
static void sDrawImmediateStencilBounds( const RenderableHeader & tHeader, const RenderableDetails & tDetails, const RenderLayer eRenderLayer );  // line 2924
void SubObjectGfxOcclusionHighlightingAddRef( OcclusionHighlighting * ptManager );  // line 3756 (DWARF: declared in SubObjectGfx.h)
void SubObjectGfxOcclusionHighlightingDecRef( OcclusionHighlighting * ptManager );  // line 3767
void GfxFixedPointPosSet( const Vector3D & vecFixedPointPosScale, const Vector3D & vecFixedPointPosOffset );  // X/Client/Gfx/Gfx.cpp:2018
#include "../Common/Definitions/WorldDefinition.h"

#include "Cloth.h"
#include "../Common/Player.h"
#include "Weather.h"
#include "Definitions/PreferencesDefinition.h"
#include "../Common/Definitions/GlobalDefinition.h"
BOOL GfxFogEnabled();  // X/Client/Gfx/Gfx.cpp:2488
void GfxFogSetEnabled( const BOOL fEnabled );  // X/Client/Gfx/Gfx.cpp:2476
void RopeDraw( const RenderableHeader & tHeader, const RenderableDetails & tDetails );  // X/Client/Rope.cpp:1728
void TrailDraw( const RenderableHeader & tHeader, const RenderableDetails & tDetails );  // X/Client/Trails.cpp:1330
void ParticleDrawBillboards( const RenderableHeader & tHeader, const RenderableDetails & tDetails, const RenderLayer eRenderLayer, const BOOL fSupportsLights );  // X/Client/Particle.cpp:7068
const MaterialTextureEntry * MaterialLookup( const UberMaterial & tUberMaterial, ShaderTex eShaderTex );  // X/Client/Gfx/GfxMaterial.cpp:354
void GfxShaderSetPointLinearLightCount( const int32 nPassIndex, const ShaderDefinition * pdefShader, const int32 nLightCount, const LightState * const * pArrayLights );  // X/Client/Gfx/GfxShader.cpp:1593
ResourceUsage GfxVBGetUsage( const VBID vbid );  // X/Client/Gfx/GfxVB.cpp:635
void GfxVBFormatCopyIntoVB( const VBID vbid, const FatVertex * ptVerts, const uint32 dwCount, uint32 & dwOffset );  // X/Client/Gfx/GfxVB.cpp:854
void GfxShaderSetNumBones( const int32 nNumBones );  // X/Client/Gfx/GfxShader.cpp:1570
void GfxShaderSetTextures( const UberMaterial & tUberMaterial, const TexAnimState * ptTexAnimStates, LPCSTR szDebug, const SNO snoAppearance );  // X/Client/Gfx/GfxShader.cpp:986
BOOL GfxShaderSetDiffuseOverlayTexture( const OverlayType eOverlayType, const OverlayProjectionAxis eAxis, const UberMaterial & tUberMaterial, const TexAnimState * ptTexAnimStates, const int32 nTexAnimStateCount );  // X/Client/Gfx/GfxShader.cpp:1359
void GfxShaderSetTexture( const SNO sno, const ShaderTex eShaderTex );  // X/Client/Gfx/GfxShader.cpp:955
void GfxShaderSetLightmapTexture( const SNO sno, const ShaderTex eShaderTex );  // X/Client/Gfx/GfxShader.cpp:966
void GfxShaderSetLights( const Material & mat, const ShaderDefinition * pdefShader, const int32 nLightCount, const LightState * const * pArrayLights );  // X/Client/Gfx/GfxShader.cpp:1622
RGBAColorValue RActorGetMeshLighting( const RActorID idRActor );  // X/Client/RActors/RActors.cpp:5981
extern VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];  // X/Client/Gfx/GfxVB.cpp
static void sDrawImmediate( const RenderableHeader & tHeader, const RenderableDetails & tDetails, const RenderLayer eRenderLayer, BOOL fSupportsLights );  // line 2528
static void sSetupHardwareSkinned( const RenderableHeader & tHeader, const RenderableDetails & tDetails, BOOL bNeedsBumpBasis, int32 & nStartIndex, int32 & nIndexCount, int32 & nVertexCount );  // line 3001
// X/Client/Gfx/GfxMaterial.cpp:310 and :412, X/Client/TexAnimation.cpp:1048
const MaterialTextureEntry * MaterialLookup( const MaterialTextureEntry * arMaterialTex, int32 nCount, ShaderTex eShaderTex );
int32 MaterialGetTextureCount( const UberMaterial & tUberMaterial );
void TexAnimApply( const TextureBinding * ptTexBinding, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState & tTexAnimState, const TextureDefinition * ptTextureDef );





// X/Client/SubObjectGfx.cpp local types (on their DWARF lines)
typedef float Matrix3x4[12];

struct RenderLayerData {  // size 0x18

    int32 nMaxRenderableHeaderCount;  // 0x00
    RenderableHeader * ptRenderableHeaderList;  // 0x08
    int32 nRenderableHeaderCount;  // 0x10
};
static_assert(sizeof(RenderLayerData) == 0x18, "RenderLayerData");


float s_fOcclusionHighlightingLatency = 0.1f;  // 0x710103949c = 0x3dcccccd

class OcclusionHighlighting {  // size 0x160


    enum {
        SORT_PRIORITY_CHANGE = 32,
        MAX_HIGHLIGHTED_ACTORS = 26,
        MAX_SUBOBJECTS_PER_ACTOR = 38,
    };

public:
    OcclusionHighlighting();
    ~OcclusionHighlighting();

    void QueryBegin();
    void QueryEnd();

    BOOL Occluded();

    void FrameEnd();

    void AddRenderableRef();
    BOOL DecRenderableRef();

    BOOL MarkedForDispose() const;
    void MarkForDispose(BOOL flDisposed) { m_flMarkedForDispose = flDisposed; }


    BOOL HasRenderableRefs() const;

    void ClearQueries();

    void SetEstimatedPixelCount(uint32 nPixelCount);







private:

    int32 m_nRenderableRefCount;  // 0x00
    BOOL m_flMarkedForDispose;  // 0x04

    XArray<OcclusionQuery *, OcclusionQuery *, 38> m_queries;  // 0x08
    Blizzard::Time::Millisecond m_tLastVisible;  // 0x148

    enum State {
        S_BUILDING = 0,
        S_QUERYING = 1,
        S_OVERFLOWED = 2,
    };


    State m_eState;  // 0x14c

    enum OcclusionState {
        OS_UNKNOWN = 0,
        OS_OCCLUDED = 1,
        OS_VISIBLE = 2,
    };


    OcclusionState m_eOcclusionState;  // 0x150

    uint32 m_nTotalVisiblePixels;  // 0x154
    uint32 m_nEstimatedPixelCount;  // 0x158

    OcclusionState FindOcclusionState();
};
static_assert(sizeof(OcclusionHighlighting) == 0x160, "OcclusionHighlighting");
typedef XFixedBlockAllocator<OcclusionHighlighting, 26> OcclusionHighlightingAllocator;



struct SubObjectGlobals {  // size 0x2570

    Matrix3x4 * pmMatrixPalette;  // 0x00

    BOOL fLocalPlayerGhosted;  // 0x08

    int32 nMaxSortedRenderables;  // 0x0c

    int32 nShadowMapTexWidth;  // 0x10

    Matrix4x4 mCachedShadowProj;  // 0x14


    SNO snoTexShadowMapColor;  // 0x54
    SNO snoTexShadowMapColor2;  // 0x58
    BOOL bUseShadowMapColorClearStenciled;  // 0x5c
    SNO snoTexShadowMapDepth;  // 0x60

    BOOL bTexWaterUpdate;  // 0x64
    SNO snoTexWater;  // 0x68
    BOOL bTexInteractiveFogUpdate;  // 0x6c
    SNO snoTexInteractiveFog;  // 0x70

    BlurFilter tFilter;  // 0x74

    SubObjectState tRippleUpdateSubObjectState;  // 0xd8

    SubObjectState tRippleDrawSubObjectState;  // 0x120


    VBID vbidStencilAABB;  // 0x168
    IBID ibidStencilAABB;  // 0x16c

    VBID vbidRippleSprite;  // 0x170
    IBID ibidRippleSprite;  // 0x174


    OcclusionHighlightingAllocator tOcclusionHighlightingPool;  // 0x178
};
static_assert(sizeof(SubObjectGlobals) == 0x2570, "SubObjectGlobals");




// values read from the original's data at 0x7100e3df50 (BOOL[27])
static const BOOL sarRenderLayerSupportsLights[NUM_RENDERLAYERS] = { 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
#include "Prefetch.h"
#include "../Common/GameCommonData.h"
void GfxViewportSet( const IRect2D & rectViewport );  // X/Client/Gfx/Gfx.cpp (as CGame.cpp:1454)
void GfxViewportScaleBiasGet( Vector4D & vScaleBias );  // X/Client/Gfx/Gfx.cpp (as PostFX.cpp:120)
void GfxViewportScaleBiasSet( const Vector4D & vViewportScaleBias );  // X/Client/Gfx/Gfx.cpp (as CGame.cpp:2498)
void GfxRenderClear( BOOL fClearColor, BOOL fClearDepth, BOOL fClearStencil, const RGBAColor & rgbaClear );  // X/Client/Gfx/Gfx.cpp, _Z14GfxRenderCleariiiRK9RGBAColor
void GfxGetRenderTargetDimensions( uint32 & dwWidth, uint32 & dwHeight );  // X/Client/Gfx/Gfx.cpp:3462
void GfxBuild2DUIProjectionMatrix( Matrix4x4 & mProj );  // X/Client/Gfx/GfxPrimitives.cpp (as GfxPrimitives.cpp:36)
void * GfxVBLock( const VBID vbid, const uint32 dwCount, uint32 & dwStartIndex );  // X/Client/Gfx/GfxVB.cpp (as Sprite.cpp:36)
TexAnimMatrixType TexAnimGetTexMatrix( Matrix4x4 & mTex, const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const TexAnimState * ptTexAnimState, const TextureDefinition * ptTextureDef );  // X/Client/TexAnimation.cpp:707
// X/Client/Gfx/GfxVB.h:137 and :595 (src GfxVB.h lacks them; copy of Sprite.cpp:127-145)
struct VertexSprite {  // size 0x2c
    Vector3D wpPos;  // 0x00
    VertexColor vcDiffuse;  // 0x0c
    VertexColor vcAuxColor;  // 0x10
    Vector2D vpUV0;  // 0x14
    Vector2D vpUV1;  // 0x1c
    Vector2D vpUV2;  // 0x24
};
inline void VertexSpriteSet(VertexSprite & vDest, const WorldDistance wdX, const WorldDistance wdY, const WorldDistance wdZ,
                            const VertexColor vcDiffuse, const VertexColor vcAuxColor,
                            const Vector2D & vpUVSet0, const Vector2D & vpUVSet1, const Vector2D & vpUVSet2)
{
    vDest.wpPos.Set(wdX, wdY, wdZ);
    vDest.vcDiffuse = vcDiffuse;
    vDest.vcAuxColor = vcAuxColor;
    vDest.vpUV0 = vpUVSet0;
    vDest.vpUV1 = vpUVSet1;
    vDest.vpUV2 = vpUVSet2;
}
static void sBuildProjectionTextureMatrix( Matrix4x4 & mTex );  // line 1900
BOOL SubObjectWaterTextureGetUpdate();  // line 3693
BOOL SubObjectInteractiveFogTextureGetUpdate();  // line 3714
void GfxSetRenderTarget( const SNO sno, const CubeMapFace eCubeMapFace );  // X/Client/Gfx/Gfx.cpp:3334









enum {
    INITIAL_MAX_RENDERABLES_PER_LAYER = 0,
    INITIAL_MAX_SORTED_RENDERABLES = 1024,
    VERTICES_PER_RIPPLE = 3,
};

enum TransSortOrder {
    SORT0_IN_FRONT = 0,
    SORT_OVERLAP = 1,
    SORT1_IN_FRONT = 2,
};

extern const Vector2D cgvecPMAOffset;
extern const Vector2D cgvecRippleMapOffset;
extern const WorldDistance cwdPMAMapWidth;
extern const WorldDistance cwdRippleMapWidth;
extern const Matrix4x4 cmSceneScaleTweak;

uint32 s_nOcclusionHideVisiblePixelPercent;
uint32 s_nOcclusionShowVisiblePixelPercent;

const Vector2D cgvecPMAOffset( -10.0f, -10.0f );
const Vector2D cgvecRippleMapOffset( -10.0f, -10.0f );

WorldDistance cwdShadowMapWidth = 154.0f;
float cflShadowMapScaleFactor = 1.0f / cwdShadowMapWidth;
const WorldDistance cwdPMAMapWidth = 154.0f;  // value read from 0x7100e3df48
const WorldDistance cwdRippleMapWidth = 154.0f;  // value read from 0x7100e3df4c

static const float cflSceneScaleFactor = 1.0000916f;
const Matrix4x4 cmSceneScaleTweak( cflSceneScaleFactor, 0.0f, 0.0f, 0.0f,
                                   0.0f, cflSceneScaleFactor, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f );

static RenderableHeader * sMergeSortTemps;  // line 269

// X/Client/TexAnimation.cpp:195 and :1071
void TexAnimInitialize(const UberMaterial & tUberMaterial, TexAnimState * ptTexAnimState, RActor * ptRActor);
TexAnimState * MaterialAllocTexAnimState(const UberMaterial & tUberMaterial, int32 & nTexAnimStateCount);

// X/Client/Definitions/AppearanceDefinition.cpp:4244
SubObjectAppearance & AppearanceGetSubObjectAppearance(const AppearanceDefinition * ptDefinition, int32 nSubObjectLook, const int32 nSubObject, const GeoSetType eGeoSetType);

static void sInitializeWaterParams();
void SubObjectWaterTextureSetUpdate(BOOL bUpdate);
void SubObjectInteractiveFogTextureSetUpdate(BOOL bUpdate);
// X/Client/Gfx/GfxShader.cpp (the DWARF places these in GfxShader.h; only the inline is a header body)
struct TextureBinding;
void GfxShaderInit(const SNO sno, const int32 nPassIndex);
TextureBinding * GfxGetTextureBinding(const ShaderTex eShaderTex);
void GfxShaderSetTexture(const TextureBinding * ptTexBinding, const SNO sno);
void GfxShaderSetTextureFactor(const RGBAColor & rgbaFactor);

// X/Client/TexAnimation.cpp:1124 (not declared in src TexAnimation.h)
TexAnimState * TexAnimRenderAsyncCopy( int32 nCount, const TexAnimState * ptTexAnimState, const UberMaterial * ptUberMaterial, ShaderTex eOverlayShaderTex );
void MaterialFreeTexAnimState(TexAnimState *& ptTexAnimState);  // X/Client/TexAnimation.cpp

// Defined below; the DWARF has them at SubObjectGfx.cpp:3290-3547.
static void sMergeSort( RenderableHeader * pBase, size_t nCount, int32 ( * pfCompare )( const void *, const void * ) );
static int32 sOpaqueSort( const void * lpArg0, const void * lpArg1 );
static int32 sEarlyOpaqueSort( const void * lpArg0, const void * lpArg1 );
static int32 sBackToFrontSort( const void * lpArg0, const void * lpArg1 );
static int32 sProjectionSort( const void * lpArg0, const void * lpArg1 );
static int32 sTransparentSort( const void * lpArg0, const void * lpArg1 );

// The line table places the final bitfield clear immediately before the depth-pass check.
inline RenderableHeader::RenderableHeader(
    const ShaderDefinition * pdefShader,
    const Material & mat )
{
    this->pdefShader = pdefShader;
    this->nSortPri = mat.nSortPri;
    fUseHighlight = FALSE;
    fIsWall = FALSE;
    fInPrepass = FALSE;
    fSetShadowLevel = FALSE;

    this->nGroup = (uintptr)-1;
    nRenderableIndex = 0;
    rgbaColor.Set( 0, 0, 0, 0 );
    this->nDist = 0;
    irectDist.left = 0;
    irectDist.top = 0;
    irectDist.right = 0;
    irectDist.bottom = 0;
    this->fEarlyZDisabled = FALSE;
    if ( pdefShader && pdefShader->dwRenderLayers == ( 1 << RENDERLAYER_DEPTHPASS_DEPRECATED ) ) {

        const RenderParams & tRenderParams = ( (RenderPass *)pdefShader->ptPasses )->tRenderParams;
        fEarlyZDisabled = tRenderParams.fAlphaTestEnable && ( tRenderParams.fZBufferWriteEnable || tRenderParams.fStencilBufferEnable );
    }
}




static BOOL sRenderableTypeHasAppearance( RenderableType eType ) {
    return eType == RENDERABLETYPE_SCENE || eType == RENDERABLETYPE_RACTOR;
}





static BOOL sRenderableTypeGetSize( RenderableType eType ) {
    switch ( eType ) {
    case RENDERABLETYPE_ROPE:
        return sizeof( RenderableCommon ) + sizeof( RenderableRope );
    case RENDERABLETYPE_TRAIL:
        return sizeof( RenderableCommon ) + sizeof( RenderableTrail );
    case RENDERABLETYPE_STENCIL_BOUNDS:
        return sizeof( RenderableCommon ) + sizeof( RenderableStencilBound );
    case RENDERABLETYPE_BILLBOARDPARTICLE:
        return sizeof( RenderableCommon ) + sizeof( RenderableParticle );
    default:
        return sizeof( RenderableDetails );
    }
}


RenderableDetails::RenderableDetails( const RenderableType eType, const DataID id, const PerfBin ePerfBin, const Material & mat ) {
    SigmaMemoryClear( this, sRenderableTypeGetSize( eType ) );

    tCommon.eType = eType;
    tCommon.id = id;
    tCommon.ePerfBin = ePerfBin;
    tCommon.mat = mat;
    tCommon.mWorld = cgm4x4Identity;

    if ( sRenderableTypeHasAppearance( eType ) ) {
        tAppearance.snoIrradianceTex = SNO_NONE;
        tAppearance.snoAmbientOcclusionOverrideTex = SNO_NONE;
        for ( int32 i = 0; i < 3; i++ ) {
            tAppearance.rgbavalHitFlashAdd[i] = 0.0f;
            tAppearance.rgbavalHitFlashMul[i] = 1.0f;
        }
    }
}











Renderable::Renderable( const RenderableType eType, const DataID id, const PerfBin ePerfBin, const ShaderDefinition * pdefShader, const Material & mat )
    : header( pdefShader, mat ), details( eType, id, ePerfBin, mat ) {
}












void SubObjectGfxInitializeForGame() {
    SubObjectGlobals *& ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    ptSubObjectGlobals = new ( SigmaMemoryNew( sizeof( SubObjectGlobals ), 4, NULL, TRUE ) ) SubObjectGlobals();

    ptSubObjectGlobals->nMaxSortedRenderables = INITIAL_MAX_SORTED_RENDERABLES;
    sMergeSortTemps = (RenderableHeader *)SigmaMemoryNew( ptSubObjectGlobals->nMaxSortedRenderables * sizeof( RenderableHeader ), 4, NULL, FALSE );

    sInitializeWaterParams();
}

typedef uint16 GfxNdx;  // X/Client/Gfx/GfxAPI.h:56
BOOL GfxIBAllocTracked(const ResourceUsage eUsage, const uint32 dwCount, const BOOL fPurgeIfNecessary, const GfxNdx * lpInitialData, IBID & ibid, LPCSTR szFile, int32 nLine, LPCSTR szDebugString);  // X/Client/Gfx/GfxIB.cpp
// X/Client/Gfx/GfxVB.h:544, written from the copies inlined into sInitializeStencilAABB
inline void VertexNoTexLSet( VertexNoTexL & vDest, const Vector3D & wpPos, const VertexColor vcDiffuse ) {
    vDest.wpPos = wpPos;
    vDest.vcDiffuse = vcDiffuse;
}
static void sInitializeStencilAABB(); static void sDisposeWaterParams(); static void sDisposeStencilAABB();  // lines 493, 548, 563
// Discarded by the linker (DWARF subprogram at address 0, 0x78 bytes, inlined sDisposeWaterParams at 464 and sDisposeStencilAABB at 465). Inference: the two re-initialising calls on 466-467 (order and lines assumed).
void SubObjectGfxResetForAppearanceChange() {


    sDisposeWaterParams();
    sDisposeStencilAABB();
    sInitializeWaterParams();
    sInitializeStencilAABB();
}




static void sInitializeWaterParams() {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    if ( !ptSubObjectGlobals ) {
        return;
    }

    ScopedAppearanceDefinitionLockR pdefAppearance( GlobalSNOGet( GLOBALSNO_WATER_SURFACE_APPEARANCE ) );
    {
        SubObjectAppearance & tAppearance = AppearanceGetSubObjectAppearance( pdefAppearance, 0, 0, 0 );
        ptSubObjectGlobals->tRippleUpdateSubObjectState.arTexAnimStates = MaterialAllocTexAnimState( tAppearance.tCompositeUberMaterial, ptSubObjectGlobals->tRippleUpdateSubObjectState.nTexAnimStateCount );
        TexAnimInitialize( tAppearance.tCompositeUberMaterial, ptSubObjectGlobals->tRippleUpdateSubObjectState.arTexAnimStates, NULL );
    }
}







static void sInitializeStencilAABB() {


    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    if ( !ptSubObjectGlobals ) {
        return;
    }


    VertexNoTexL * pv = (VertexNoTexL *)SigmaMemoryNew( 8 * sizeof( VertexNoTexL ), 4, NULL, FALSE );

    VertexColor colColor( 255, 255, 255, 255 );

    VertexNoTexLSet( pv[0], Vector3D( 1.0f, -1.0f, 1.0f ), colColor );
    VertexNoTexLSet( pv[1], Vector3D( 1.0f, 1.0f, 1.0f ), colColor );
    VertexNoTexLSet( pv[2], Vector3D( 1.0f, 1.0f, -1.0f ), colColor );
    VertexNoTexLSet( pv[3], Vector3D( 1.0f, -1.0f, -1.0f ), colColor );
    VertexNoTexLSet( pv[4], Vector3D( -1.0f, -1.0f, 1.0f ), colColor );
    VertexNoTexLSet( pv[5], Vector3D( -1.0f, 1.0f, 1.0f ), colColor );
    VertexNoTexLSet( pv[6], Vector3D( -1.0f, 1.0f, -1.0f ), colColor );
    VertexNoTexLSet( pv[7], Vector3D( -1.0f, -1.0f, -1.0f ), colColor );

    BOOL fSuccess = GfxVBAllocTracked( RESOURCEUSAGE_IMMUTABLE, VBFORMAT_NOTEX_L, 8, FALSE, pv, ptSubObjectGlobals->vbidStencilAABB, "", 0, "" );

    SigmaMemoryFree( pv, NULL );

    uint16 * pi = (uint16 *)SigmaMemoryNew( 36 * sizeof( uint16 ), 4, NULL, FALSE );
    pi[0] = 2; pi[1] = 1; pi[2] = 0;
    pi[3] = 3; pi[4] = 2; pi[5] = 0;
    pi[6] = 6; pi[7] = 5; pi[8] = 1;
    pi[9] = 2; pi[10] = 6; pi[11] = 1;
    pi[12] = 7; pi[13] = 4; pi[14] = 5;
    pi[15] = 6; pi[16] = 7; pi[17] = 5;
    pi[18] = 3; pi[19] = 0; pi[20] = 4;
    pi[21] = 7; pi[22] = 3; pi[23] = 4;
    pi[24] = 1; pi[25] = 5; pi[26] = 4;
    pi[27] = 0; pi[28] = 1; pi[29] = 4;
    pi[30] = 6; pi[31] = 2; pi[32] = 3;
    pi[33] = 7; pi[34] = 6; pi[35] = 3;

    GfxIBAllocTracked( RESOURCEUSAGE_IMMUTABLE, 36, FALSE, pi, ptSubObjectGlobals->ibidStencilAABB, "", 0, "" );

    SigmaMemoryFree( pi, NULL );

    fSuccess = GfxVBAllocTracked( RESOURCEUSAGE_DYNAMIC, VBFORMAT_SPRITE, VERTICES_PER_RIPPLE, TRUE, NULL, ptSubObjectGlobals->vbidRippleSprite, "", 0, "" );


    int32 nIndexCount;
    GfxQuadlistIBAlloc( RESOURCEUSAGE_IMMUTABLE, 1, FALSE, nIndexCount, ptSubObjectGlobals->ibidRippleSprite );

}




static void sDisposeWaterParams() {


    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    if ( ptSubObjectGlobals ) {




        MaterialFreeTexAnimState( ptSubObjectGlobals->tRippleUpdateSubObjectState.arTexAnimStates );
    }
}



static void sDisposeStencilAABB() {


    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    if ( ptSubObjectGlobals ) {




        GfxVBRelease( ptSubObjectGlobals->vbidStencilAABB );
        ptSubObjectGlobals->vbidStencilAABB = -1;
        GfxIBRelease( ptSubObjectGlobals->ibidStencilAABB );
        ptSubObjectGlobals->ibidStencilAABB = -1;

        GfxVBRelease( ptSubObjectGlobals->vbidRippleSprite );
        ptSubObjectGlobals->vbidRippleSprite = -1;
        GfxIBRelease( ptSubObjectGlobals->ibidRippleSprite );
        ptSubObjectGlobals->ibidRippleSprite = -1;
    }
}



void sComputeAppearanceFXParms( ShaderFloat & tRegTemp, const RenderableDetails & tDetails ) {

    AppearanceFXFunction eAppearanceFXFunc = (AppearanceFXFunction)TagMapGetInt( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_FX_FUNC );
    float flVal0 = TagMapGetFloat( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_FX_VAL0 );
    float flVal1 = TagMapGetFloat( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_FX_VAL1 );
    float flPeriod = TagMapGetFloat( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_FX_PERIOD );

    float flClientTime = GetRWindowMgr()->GetMainRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )].flClientTime;

    if ( flPeriod < 0.0f ) {

        flClientTime -= tDetails.tAppearance.flLookSwitchTime;
        flPeriod = Math::Abs( flPeriod );
    }

    float flTrunc = Math::FloatTrunc( flClientTime / flPeriod );
    float flTime = ( flClientTime - flPeriod * flTrunc ) / flPeriod;
    // Inference: one store pair at 604 writes {1, 0, 0, 0}; the four element stores are placed on 604-607.
    tRegTemp[0] = 1.0f;
    tRegTemp[1] = 0.0f;
    tRegTemp[2] = 0.0f;
    tRegTemp[3] = 0.0f;

    switch ( eAppearanceFXFunc ) {







    case APPEARANCE_FX_FUNC_SINE:

        tRegTemp[0] = flVal1 + flVal0 * ( Math::Sin( flTime * FULL_CIRCLE ) * 0.5f + 0.5f );
        break;


    case APPEARANCE_FX_FUNC_LERP:

        tRegTemp[0] = Math::Lerp( flVal0, flVal1, flTime );
        break;


    case APPEARANCE_FX_FUNC_LOG:

        tRegTemp[0] = flVal0 * ( 1.0f - Math::Pow( flTime, flVal1 ) );
        break;


    case APPEARANCE_FX_FUNC_EXP:

        tRegTemp[0] = flVal0 * Math::Pow( 1.0f - flTime, flVal1 );
        break;


    case APPEARANCE_FX_FUNC_LINEAR_FADE:

        if ( flTime < flVal1 ) {

            tRegTemp[0] = Math::Sin( flTime * FULL_CIRCLE ) * 0.5f + 0.5f;
        } else {


            tRegTemp[0] = Math::Lerp(
                Math::Sin( flVal1 * FULL_CIRCLE ) * 0.5f + 0.5f,
                0.0f,
                flTime - flVal1 );
        }
        tRegTemp[0] *= flVal0;
        break;

    case APPEARANCE_FX_FUNC_RAND:
        {

            RNG & rng = CGameGlobalsGet().rngLocal;
            float flRand = RandFloat_Internal( rng );
            tRegTemp[0] = Math::Lerp( flVal0, flVal1, flRand );
        }
        break;

    case APPEARANCE_FX_FUNC_SET:

        tRegTemp[0] = flVal0; tRegTemp[1] = flVal1;
        break;

    default:
        break;
    }





}




void DisplayList::Initialize( const RenderContext eContext, const BOOL fSimple ) {
    int32 nRenderLayerCount = fSimple ? NUM_SIMPLE_RENDERLAYERS : NUM_RENDERLAYERS;
    m_eContext = eContext;
    m_fSimple = fSimple;

    m_tLock.Create();

    ptRenderLayerData = (RenderLayerData *)SigmaMemoryNew( nRenderLayerCount * sizeof( RenderLayerData ), 4, NULL, TRUE );
    for ( int32 i = 0; i < nRenderLayerCount; i++ ) {
        RenderLayerData & tRenderLayer = ptRenderLayerData[i];
        tRenderLayer.nMaxRenderableHeaderCount = 0;
        tRenderLayer.nRenderableHeaderCount = 0;
        tRenderLayer.ptRenderableHeaderList = NULL;
    }

    nMaxRenderableDetailsCount = 0;
    nRenderableDetailsCount = 0;
    ptRenderableDetailsList = NULL;
}























void DisplayList::Dispose() {
    int32 nRenderLayerCount = m_fSimple ? NUM_SIMPLE_RENDERLAYERS : NUM_RENDERLAYERS;

    for ( int32 i = 0; i < nRenderLayerCount; i++ ) {
        RenderLayerData & tRenderLayer = ptRenderLayerData[i];
        SigmaMemoryFree( tRenderLayer.ptRenderableHeaderList, NULL );
        tRenderLayer.ptRenderableHeaderList = NULL;
    }

    SigmaMemoryFree( ptRenderableDetailsList, NULL );
    ptRenderableDetailsList = NULL;
    SigmaMemoryFree( ptRenderLayerData, NULL );
    ptRenderLayerData = NULL;

    m_tLock.Destroy();
}







void DisplayList::Compact() {
    int32 nRenderLayerCount = m_fSimple ? NUM_SIMPLE_RENDERLAYERS : NUM_RENDERLAYERS;

    for ( int32 i = 0; i < nRenderLayerCount; i++ ) {
        RenderLayerData & tRenderLayer = ptRenderLayerData[i];
        tRenderLayer.nMaxRenderableHeaderCount = 0;
        SigmaMemoryFree( tRenderLayer.ptRenderableHeaderList, NULL );
        tRenderLayer.ptRenderableHeaderList = (RenderableHeader *)SigmaMemoryNew( tRenderLayer.nMaxRenderableHeaderCount * sizeof( RenderableHeader ), 4, NULL, FALSE );
    }

    nMaxRenderableDetailsCount = 0;
    SigmaMemoryFree( ptRenderableDetailsList, NULL );
    ptRenderableDetailsList = (RenderableDetails *)SigmaMemoryNew( nMaxRenderableDetailsCount * sizeof( RenderableDetails ), 4, NULL, FALSE );
}










void DisplayList::RenderBegin() {
    int32 nRenderLayerCount = m_fSimple ? NUM_SIMPLE_RENDERLAYERS : NUM_RENDERLAYERS;
    for ( int32 i = 0; i < nRenderLayerCount; i++ ) {
        RenderLayerData & tRenderLayer = ptRenderLayerData[i];
        tRenderLayer.nRenderableHeaderCount = 0;
    }
    nRenderableDetailsCount = 0;
}









void DisplayList::RenderEnd() {


    BOOL fLockShader = !gptGroupDefShader->NoLockTracking();


    int32 nRenderLayerCount = m_fSimple ? NUM_SIMPLE_RENDERLAYERS : NUM_RENDERLAYERS;
    for ( int32 nRenderLayer = 0; nRenderLayer < nRenderLayerCount; nRenderLayer++ ) {

        RenderLayerData & tRenderLayer = ptRenderLayerData[nRenderLayer];
        int32 nRenderableHeaderCount = tRenderLayer.nRenderableHeaderCount;
        for ( int32 i = 0; i < nRenderableHeaderCount; i++ ) {

            RenderableHeader & tHeader = tRenderLayer.ptRenderableHeaderList[i];
            if ( fLockShader && tHeader.pdefShader ) {
                // Inference: the shader unlock leaves no code (DefinitionUnlockR<ShaderDefinition> is empty); the NULL store is only on the other path.
                DefinitionUnlockR( tHeader.pdefShader );

            } else {

                tHeader.pdefShader = NULL;
            }
        }
        tRenderLayer.nRenderableHeaderCount = 0;
    }

    for ( int32 i = 0; i < nRenderableDetailsCount; i++ ) {

        RenderableDetails & tDetails = ptRenderableDetailsList[i];
        if ( sRenderableTypeHasAppearance( tDetails.tCommon.eType ) ) {


            SubObjectGfxOcclusionHighlightingDecRef( tDetails.tAppearance.ptOcclusionHighlighting );


            if ( tDetails.tAppearance.pdefAppearance ) {

                gptGroupDefAppearance->UnlockR( (void *&)tDetails.tAppearance.pdefAppearance );
            }
        }
    }
    nRenderableDetailsCount = 0;
}




void DisplayList::Sort() {


    int32 nRenderLayerCount = m_fSimple ? NUM_SIMPLE_RENDERLAYERS : NUM_RENDERLAYERS;
    for ( int32 nRenderLayer = 0; nRenderLayer < nRenderLayerCount; nRenderLayer++ ) {

        RenderLayerData & tRenderLayer = ptRenderLayerData[nRenderLayer];
        RenderLayer eRenderLayer = (RenderLayer)nRenderLayer;

        if ( tRenderLayer.nRenderableHeaderCount != 0 ) {

            switch ( eRenderLayer ) {
            case RENDERLAYER_POSTFX:
            case RENDERLAYER_FLOOR:


                sMergeSort( tRenderLayer.ptRenderableHeaderList, tRenderLayer.nRenderableHeaderCount, sOpaqueSort );
                break;
            case RENDERLAYER_PROJECTED_ADDITIVE:
            case RENDERLAYER_TRANSPARENT_MASKED:
                sMergeSort( tRenderLayer.ptRenderableHeaderList, tRenderLayer.nRenderableHeaderCount, sProjectionSort );
                break;
            case RENDERLAYER_SKY: case RENDERLAYER_TRANSPARENT_GROUND:
            case RENDERLAYER_OPAQUE: case RENDERLAYER_BLOOM:
            case RENDERLAYER_AURAS: case RENDERLAYER_GROUND_FX_1:

                sMergeSort( tRenderLayer.ptRenderableHeaderList, tRenderLayer.nRenderableHeaderCount, sBackToFrontSort );
                break;





            case RENDERLAYER_TRANSPARENT:

                sMergeSort( tRenderLayer.ptRenderableHeaderList, tRenderLayer.nRenderableHeaderCount, sTransparentSort );
                break;







            case RENDERLAYER_EARLY_OPAQUE:

                sMergeSort( tRenderLayer.ptRenderableHeaderList, tRenderLayer.nRenderableHeaderCount, sEarlyOpaqueSort );
                break;
            default:
                break;
            }
        }
    }




}




void DisplayList::sResizeVisibleSubObjectDetailsList() {


    nMaxRenderableDetailsCount += 4;
    ptRenderableDetailsList = (RenderableDetails *)SigmaMemoryResize( ptRenderableDetailsList, NULL, nMaxRenderableDetailsCount * sizeof( RenderableDetails ), 4 );
}




void DisplayList::sResizeVisibleSubObjectHeaderList(
    const RenderLayer eRenderLayer ) {

    RenderLayerData & tRenderLayer = ptRenderLayerData[eRenderLayer];
    tRenderLayer.nMaxRenderableHeaderCount += 4;
    tRenderLayer.ptRenderableHeaderList = (RenderableHeader *)SigmaMemoryResize( tRenderLayer.ptRenderableHeaderList, NULL, tRenderLayer.nMaxRenderableHeaderCount * sizeof( RenderableHeader ), 4 );

    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    int32 nMaxSortedRenderables = ptSubObjectGlobals->nMaxSortedRenderables;
    int32 nRenderLayerCount = m_fSimple ? NUM_SIMPLE_RENDERLAYERS : NUM_RENDERLAYERS;
    for ( int32 i = 0; i < nRenderLayerCount; i++ ) {

        RenderLayerData & tScopedRenderLayer = ptRenderLayerData[i];
        nMaxSortedRenderables = Math::Max( nMaxSortedRenderables, tScopedRenderLayer.nMaxRenderableHeaderCount );
    }
    if ( nMaxSortedRenderables > ptSubObjectGlobals->nMaxSortedRenderables ) {

        ptSubObjectGlobals->nMaxSortedRenderables = nMaxSortedRenderables;
        sMergeSortTemps = (RenderableHeader *)SigmaMemoryResize( sMergeSortTemps, NULL, nMaxSortedRenderables * sizeof( RenderableHeader ), 4 );
    }
}



// NON_MATCHING: 368 vs 368 bytes; only the add of &m_tLock.tEvent is scheduled before the dmb of the inlined SpinlockLeave instead of after (SubmitRenderableHeader, same inline, matches).
int32 DisplayList::SubmitRenderableDetails(
    const RenderableDetails & tRenderableDetails ) {










    Blizzard::Lock::ScopedSpinlock lock( m_tLock );


    if ( nRenderableDetailsCount >= nMaxRenderableDetailsCount ) {

        sResizeVisibleSubObjectDetailsList();
    }


    int32 nRenderableIndex = nRenderableDetailsCount;
    RenderableType eType = tRenderableDetails.tCommon.eType;
    const AppearanceDefinition * pdefAppearance = tRenderableDetails.tAppearance.pdefAppearance;
    RenderableDetails & tDetails = ptRenderableDetailsList[nRenderableDetailsCount++];
    SigmaMemoryCopy( &tDetails, &tRenderableDetails, sRenderableTypeGetSize( eType ) );

    if ( sRenderableTypeHasAppearance( eType ) ) {

        if ( pdefAppearance ) {

            gptGroupDefAppearance->LockR( (void *)pdefAppearance );
        }



        SubObjectGfxOcclusionHighlightingAddRef( tRenderableDetails.tAppearance.ptOcclusionHighlighting );
    }


    return nRenderableIndex;
}




void DisplayList::SubmitRenderableHeader(
    const RenderableHeader & tRenderableHeader,
    int32 nRenderableIndex ) {

    if ( nRenderableIndex == -1 ) {

        return;
    }


    Blizzard::Lock::ScopedSpinlock lock( m_tLock );


    const ShaderDefinition * pdefShader = tRenderableHeader.pdefShader;

    BOOL fLockShader = !gptGroupDefShader->NoLockTracking();

    int32 nPassCount = pdefShader->nPassCount;
    RenderPass * ptPasses = pdefShader->ptPasses;
    BOOL fSimple = m_fSimple;
    for ( int32 i = 0; i < nPassCount; i++ ) {

        RenderLayer eRenderLayer = ptPasses[i].eRenderLayer;
        if ( fSimple && eRenderLayer >= NUM_SIMPLE_RENDERLAYERS ) {

            continue;
        }




        RenderLayerData & tRenderLayer = ptRenderLayerData[eRenderLayer];

        if ( tRenderLayer.nRenderableHeaderCount >= tRenderLayer.nMaxRenderableHeaderCount ) {

            sResizeVisibleSubObjectHeaderList( eRenderLayer );
        }


        RenderableHeader & tHeader = tRenderLayer.ptRenderableHeaderList[tRenderLayer.nRenderableHeaderCount++];
        tHeader = tRenderableHeader;
        tHeader.nRenderableIndex = nRenderableIndex;


        if ( fLockShader ) {

            gptGroupDefShader->LockR( (void *)pdefShader );
        }
    }


}




void DisplayList::SubmitRenderable( const Renderable & tRenderable ) {
    int32 nRenderableIndex = SubmitRenderableDetails( tRenderable.details );
    SubmitRenderableHeader( tRenderable.header, nRenderableIndex );
}





void DisplayList::DrawRenderLayer(
    const RenderLayer eRenderLayer) const
{
    BOOL fSupportsLights = sarRenderLayerSupportsLights[eRenderLayer];
    RenderLayerData & tRenderLayer = ptRenderLayerData[eRenderLayer];

    BOOL fDisableFogging = m_eContext != RENDERCONTEXT_DEFAULT;
    // Inference: the FALSE has no row of its own (str wzr at +0x60 carries line 0); fSavedFogState is only read under fDisableFogging at 1098.
    BOOL fSavedFogState = FALSE;
    if ( fDisableFogging ) {

        fSavedFogState = GfxFogEnabled();
        GfxFogSetEnabled( FALSE );
    }






    for ( int32 i = 0; i < tRenderLayer.nRenderableHeaderCount; i++ ) {

        const RenderableHeader & tHeader = tRenderLayer.ptRenderableHeaderList[i];
        const RenderableDetails & tDetails = ptRenderableDetailsList[tHeader.nRenderableIndex];
        switch ( tDetails.tCommon.eType ) {

            case RENDERABLETYPE_ROPE:
                RopeDraw( tHeader, tDetails );
                break;

            case RENDERABLETYPE_TRAIL:
                TrailDraw( tHeader, tDetails );
                break;

            case RENDERABLETYPE_STENCIL_BOUNDS:
                sDrawImmediateStencilBounds( tHeader, tDetails, eRenderLayer );
                break;

            case RENDERABLETYPE_BILLBOARDPARTICLE:
                ParticleDrawBillboards( tHeader, tDetails, eRenderLayer, fSupportsLights );
                break;

            default:
                sDrawImmediate( tHeader, tDetails, eRenderLayer, fSupportsLights );
                break;
        }
    }

    if ( fDisableFogging ) {


        GfxFogSetEnabled( fSavedFogState );
    }





}





void DisplayList::DrawBoundingStencil( const RenderLayer eRenderLayer ) const {



    RenderTarget tRenderTargets[2];
    GfxGetRenderTargets( tRenderTargets );
    SNO snoDepthTarget = GfxGetDepthTarget();


    GfxSetRenderAndDepthTarget( SNO_NONE, snoDepthTarget, CUBEMAPFACE_POSX );



    RenderLayerData & tRenderLayer = ptRenderLayerData[eRenderLayer];
    for ( int32 i = 0; i < tRenderLayer.nRenderableHeaderCount; i++ ) {

        const RenderableHeader & tHeader = tRenderLayer.ptRenderableHeaderList[i];
        const RenderableDetails & tDetails = ptRenderableDetailsList[tHeader.nRenderableIndex];
        if ( tDetails.tCommon.eType == RENDERABLETYPE_STENCIL_BOUNDS ) {


            sDrawImmediateStencilBounds( tHeader, tDetails, eRenderLayer );
        }
    }







    GfxSetRenderTargets( tRenderTargets );
    GfxSetDepthTarget( snoDepthTarget );

}




BOOL DisplayList::IsRenderLayerEmpty( const RenderLayer eRenderLayer ) const {
    const RenderLayerData & tRenderLayer = ptRenderLayerData[eRenderLayer];
    return tRenderLayer.nRenderableHeaderCount == 0;
}







BOOL SubObjectGfxInit() {


    BOOL fRenderTargetsCreated = TRUE;
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    TextureFormat eTextureFormat = TEXTUREFORMAT_ARGB8888;

    uint32 dwScreenWidth, dwScreenHeight;
    GfxGetCurrentDimensions( dwScreenWidth, dwScreenHeight );












    {
        ptSubObjectGlobals->nShadowMapTexWidth = 1024;
        uint32 dwShadowTech = GfxGetShadowTech();


        switch ( dwShadowTech ) {
        case GFXSHADOWTECH_VSM:








            ptSubObjectGlobals->nShadowMapTexWidth = 1024;
            eTextureFormat = TEXTUREFORMAT_A16B16G16R16F;
            break;
        default:
            eTextureFormat = TEXTUREFORMAT_ARGB8888;
            break;
        }







        uint32 dwTargetERAMOffset = 0;
        ptSubObjectGlobals->snoTexShadowMapColor = TextureDefinitionCreateTexture(
            "Shadow RenderTarget", eTextureFormat,
            eTextureFormat,
            ptSubObjectGlobals->nShadowMapTexWidth, ptSubObjectGlobals->nShadowMapTexWidth, RESOURCEUSAGE_IMMUTABLE, 1, 1, dwTargetERAMOffset, -15 );

        fRenderTargetsCreated = ptSubObjectGlobals->snoTexShadowMapColor != SNO_NONE;

        if ( dwShadowTech == GFXSHADOWTECH_VSM ) {

            ptSubObjectGlobals->snoTexShadowMapColor2 = TextureDefinitionCreateTexture(
                "Shadow RenderTarget", eTextureFormat,
                eTextureFormat,
                ptSubObjectGlobals->nShadowMapTexWidth, ptSubObjectGlobals->nShadowMapTexWidth, RESOURCEUSAGE_IMMUTABLE, 1, 1, 0, 0 );

            fRenderTargetsCreated &= ptSubObjectGlobals->snoTexShadowMapColor2 != SNO_NONE;
        }
    }



























    eTextureFormat = TEXTUREFORMAT_D24X8;


    ptSubObjectGlobals->snoTexShadowMapDepth = TextureDefinitionCreateTexture(
        "Shadow DepthTarget", eTextureFormat,
        eTextureFormat,
        ptSubObjectGlobals->nShadowMapTexWidth, ptSubObjectGlobals->nShadowMapTexWidth, RESOURCEUSAGE_IMMUTABLE, 2, 1, 0, 0 );

    fRenderTargetsCreated = fRenderTargetsCreated && ptSubObjectGlobals->snoTexShadowMapDepth != SNO_NONE;


    SigmaMemoryClear( &ptSubObjectGlobals->tFilter, sizeof( BlurFilter ) );




    const float flFilterOffset = 0.5f / (float)ptSubObjectGlobals->nShadowMapTexWidth;

    ptSubObjectGlobals->tFilter.uvSample[0].flU = -flFilterOffset;
    ptSubObjectGlobals->tFilter.uvSample[0].flV = -flFilterOffset;
    ShaderFloatSet( ptSubObjectGlobals->tFilter.tWeight[0], 0.25f );
    ptSubObjectGlobals->tFilter.uvSample[1].flU = -flFilterOffset;
    ptSubObjectGlobals->tFilter.uvSample[1].flV = flFilterOffset;
    ShaderFloatSet( ptSubObjectGlobals->tFilter.tWeight[1], 0.25f );
    ptSubObjectGlobals->tFilter.uvSample[2].flU = flFilterOffset;
    ptSubObjectGlobals->tFilter.uvSample[2].flV = -flFilterOffset;
    ShaderFloatSet( ptSubObjectGlobals->tFilter.tWeight[2], 0.25f );
    ptSubObjectGlobals->tFilter.uvSample[3].flU = flFilterOffset;
    ptSubObjectGlobals->tFilter.uvSample[3].flV = flFilterOffset;
    ShaderFloatSet( ptSubObjectGlobals->tFilter.tWeight[3], 0.25f );



    eTextureFormat = TEXTUREFORMAT_L8;









    {
        int nTexWidth = 768;



        ptSubObjectGlobals->snoTexWater = TextureDefinitionCreateTexture(
            "WaterSimRT", eTextureFormat,
            eTextureFormat,
            nTexWidth, nTexWidth, RESOURCEUSAGE_IMMUTABLE, 1, 1, 0, 0 );


        fRenderTargetsCreated = fRenderTargetsCreated && ptSubObjectGlobals->snoTexWater != SNO_NONE;

        ptSubObjectGlobals->bTexWaterUpdate = TRUE;


        ptSubObjectGlobals->snoTexInteractiveFog = TextureDefinitionCreateTexture(
            "InteractiveFogRT", eTextureFormat,
            eTextureFormat,
            nTexWidth, nTexWidth, RESOURCEUSAGE_IMMUTABLE, 1, 1, 0, 0 );


        fRenderTargetsCreated = fRenderTargetsCreated && ptSubObjectGlobals->snoTexInteractiveFog != SNO_NONE;

        ptSubObjectGlobals->bTexInteractiveFogUpdate = TRUE;
    }












    if ( fRenderTargetsCreated ) {




        sInitializeStencilAABB();
        ptSubObjectGlobals->bUseShadowMapColorClearStenciled = FALSE;
        return TRUE;
    }
    return FALSE;
}



void SubObjectGfxDispose() {


    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    gptGroupDefTexture->Purge( ptSubObjectGlobals->snoTexShadowMapColor, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN );
    gptGroupDefTexture->Purge( ptSubObjectGlobals->snoTexShadowMapColor2, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN );
    gptGroupDefTexture->Purge( ptSubObjectGlobals->snoTexShadowMapDepth, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN );
    gptGroupDefTexture->Purge( ptSubObjectGlobals->snoTexWater, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN );
    gptGroupDefTexture->Purge( ptSubObjectGlobals->snoTexInteractiveFog, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN );

    ptSubObjectGlobals->snoTexShadowMapColor = -1;
    ptSubObjectGlobals->snoTexShadowMapColor2 = -1;
    ptSubObjectGlobals->snoTexShadowMapDepth = -1;
    ptSubObjectGlobals->bTexWaterUpdate = FALSE;
    ptSubObjectGlobals->snoTexWater = -1;
    ptSubObjectGlobals->bTexInteractiveFogUpdate = FALSE;
    ptSubObjectGlobals->snoTexInteractiveFog = -1;

    sDisposeStencilAABB();
}




void SubObjectGfxDisposeForGame() {


    SubObjectGlobals *& ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    sDisposeWaterParams();

    SigmaMemoryFree( sMergeSortTemps, NULL ); sMergeSortTemps = NULL;



    delete ptSubObjectGlobals; ptSubObjectGlobals = NULL;
}




inline void sDrawTriBatch(  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    const int32 nVertMin,
    const int32 nVertCount,
    const uint32 dwBaseIndexOffset,
    const int32 nStartIndex,
    const int32 nIndexCount,
    const BOOL fUnStripified,
    const PerfBin ePerfBin ) {

    if ( fUnStripified ) {

        GfxShaderDraw( DRAWMODE_TRIANGLELIST, dwBaseIndexOffset, nVertMin, nVertCount, nStartIndex, nIndexCount / 3, ePerfBin );

    } else {

        GfxShaderDraw( DRAWMODE_TRIANGLESTRIP, dwBaseIndexOffset, nVertMin, nVertCount, nStartIndex, nIndexCount - 2, ePerfBin );
    }
}




static void sRenderCookieShadows(
    DisplayList & dispList ) {

    if ( AppRenderLayerIsHidden( RENDERLAYER_PROJECTED_SHADOWMAP ) ) {
        return;
    }


    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    CameraManagerSetCurrentCamera( CAMERA_SHADOW );
    sBuildProjectionTextureMatrix( ptSubObjectGlobals->mCachedShadowProj );

    IRect2D rect;
    rect.Set( 0, 0, ptSubObjectGlobals->nShadowMapTexWidth, ptSubObjectGlobals->nShadowMapTexWidth );
    GfxViewportSet( rect );
    dispList.DrawRenderLayer( RENDERLAYER_PROJECTED_SHADOWMAP );


    if ( GfxGetShadowTech() == GFXSHADOWTECH_VSM && ptSubObjectGlobals->snoTexShadowMapColor2 != SNO_NONE ) {



        dispList.DrawBoundingStencil( RENDERLAYER_SHADOWMAP_STENCIL_BOUNDS );
    }




    CameraManagerRestoreCurrentCameraToMainCamera();

    if ( GfxGetShadowTech() == GFXSHADOWTECH_VSM && ptSubObjectGlobals->snoTexShadowMapColor2 != SNO_NONE ) {






        Vector4D tViewportSave;
        GfxViewportScaleBiasGet( tViewportSave );
        GfxViewportScaleBiasSet( Vector4D( 1.0f, 0.0f, 1.0f, 0.0f ) );

        GfxSetRenderAndDepthTarget( ptSubObjectGlobals->snoTexShadowMapColor, ptSubObjectGlobals->snoTexShadowMapDepth, CUBEMAPFACE_POSX );
        GfxRenderClear( TRUE, FALSE, FALSE, RGBAColor( 0, 0, 0, 0 ) );

        float flBlurWeights[4] = { 0.25f, 0.25f, 0.25f, 0.25f };
        BlurFilter tFilter;
        tFilter.uvSample[0].flU = -0.6666666f; tFilter.uvSample[0].flV = -0.6666666f;

        tFilter.uvSample[1].flU = -0.6666666f; tFilter.uvSample[1].flV = 0.6666666f;

        tFilter.uvSample[2].flU = 0.6666666f; tFilter.uvSample[2].flV = -0.6666666f;

        tFilter.uvSample[3].flU = 0.6666666f; tFilter.uvSample[3].flV = 0.6666666f;

        ShaderFloatSet( tFilter.tWeight[0], flBlurWeights[0], flBlurWeights[1], flBlurWeights[2], flBlurWeights[3] );

        PostFXFilterRender( GlobalSNOGet( GLOBALSNO_POSTFX_VSM_CONSOLE_PPC_SHADER ), ptSubObjectGlobals->snoTexShadowMapColor2, tFilter, FALSE );

        GfxViewportScaleBiasSet( tViewportSave );
    }
}






















static void sMatrixFromUberMaterial(
    Matrix4x4 & mTex,
    SNO & snoTex,
    const UberMaterial & tMasterMaterial,
    const ShaderTex eShaderTex,
    const SubObjectState & tState ) {

    const MaterialTextureEntry * ptMatTexEntry = MaterialLookup( tMasterMaterial, eShaderTex );
    if ( ptMatTexEntry ) {

        uint32 dwOffset = (uint32)( (uintptr)ptMatTexEntry - (uintptr)(MaterialTextureEntry *)tMasterMaterial.ptMatTexList );
        int32 nIndex = dwOffset / sizeof( MaterialTextureEntry );


        TexAnimGetTexMatrix( mTex, ptMatTexEntry->tMatTex.pTexAnimCompParams, ptMatTexEntry->tMatTex.serTexAnimCompParams.dwSize, &tState.arTexAnimStates[nIndex], NULL );



        snoTex = ptMatTexEntry->tMatTex.snoTex;

    } else {

        mTex = cgm4x4Identity;

        snoTex = SNO_NONE;
    }
}



static BOOL sDrawRippleBackground(
    const SNO snoAppearance,
    const SubObjectState & tState,
    const Vector3D & wpPlayerPos ) {



    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    VBID vbidRippleSprite = ptSubObjectGlobals->vbidRippleSprite;

    Matrix4x4 mTex0, mTex1;
    SNO snoTex0, snoTex1;
    ScopedAppearanceDefinitionLockR pdefAppearance( snoAppearance );
    SubObjectAppearance & tAppearance = AppearanceGetSubObjectAppearance( pdefAppearance, 0, 0, 0 );
    SNO snoShader = ShaderMapGetDefaultShader( tAppearance.tCompositeUberMaterial.snoShaderMap, TRUE, NULL );
    sMatrixFromUberMaterial( mTex0, snoTex0, tAppearance.tCompositeUberMaterial, SHADERTEX_DIFFUSE, tState );
    sMatrixFromUberMaterial( mTex1, snoTex1, tAppearance.tCompositeUberMaterial, SHADERTEX_ALPHA_LAYER_1, tState );

    // declarator order inferred from the frame slots (flRTWidth at sp+0x194 in the original)
    float flRTWidth, flRTHeight;
    GfxGetRenderTargetFloatDimensions( flRTWidth, flRTHeight );

    BOOL bDrewSomething = FALSE;



    Matrix4x4 mOffset( cgm4x4Identity );
    mOffset._41 = wpPlayerPos.x / cwdRippleMapWidth;
    mOffset._42 = -wpPlayerPos.y / cwdRippleMapWidth;
    mTex0 = mOffset * mTex0;
    mTex1 = mOffset * mTex1;

    float flZ = 0.0f;
    VertexColor vc( crgbaWhite );
    VertexColor vcAux( crgbaBlack );
    Rect2D rect( 0.0f, 0.0f, flRTWidth, flRTHeight );
    // inferred: Set order 0, 2, 1, 3 (DWARF scope order 1569, 1570, 1568, 1571; (1,1) constant built first)
    Vector4D wpUV0[4];
    Vector4D wpUV1[4];
    wpUV0[0].Set( 0.0f, 0.0f, 0.0f, 1.0f );
    wpUV0[2].Set( 1.0f, 1.0f, 0.0f, 1.0f );
    wpUV0[1].Set( 1.0f, 0.0f, 0.0f, 1.0f );
    wpUV0[3].Set( 0.0f, 1.0f, 0.0f, 1.0f );

    for ( int i = 0; i < 4; i++ ) {

        wpUV1[i] = wpUV0[i] * mTex1;
        wpUV0[i] = wpUV0[i] * mTex0;
    }


    Vector2D wvMin0( Math::FloatTrunc( wpUV0[0].x ), Math::FloatTrunc( wpUV0[0].y ) );
    Vector2D wvMin1( Math::FloatTrunc( wpUV1[0].x ), Math::FloatTrunc( wpUV1[0].y ) );
    for ( int i = 0; i < 4; i++ ) {

        wpUV0[i].x -= wvMin0.x;
        wpUV0[i].y -= wvMin0.y;
        wpUV1[i].x -= wvMin1.x;
        wpUV1[i].y -= wvMin1.y;
    }

    uint32 dwStartIndex;
    VertexSprite * ptVertices = (VertexSprite *)GfxVBLock( vbidRippleSprite, VERTICES_PER_RIPPLE, dwStartIndex );
    if ( ptVertices ) {

        Vector2D vpUVNull( 0.0f, 0.0f );
        VertexSpriteSet( ptVertices[0], rect.left, rect.top, flZ, vc, vcAux,
                         Vector2D( wpUV0[0].x, wpUV0[0].y ),
                         Vector2D( wpUV1[0].x, wpUV1[0].y ),
                         vpUVNull );


        VertexSpriteSet( ptVertices[1], rect.left, rect.bottom * 3.0f, flZ, vc, vcAux,
                         Vector2D( wpUV0[3].x, Math::Lerp( wpUV0[0].y, wpUV0[3].y, 3.0f ) ),
                         Vector2D( wpUV1[3].x, Math::Lerp( wpUV1[0].y, wpUV1[3].y, 3.0f ) ),
                         vpUVNull );
        VertexSpriteSet( ptVertices[2], rect.right * 3.0f, rect.top, flZ, vc, vcAux,
                         Vector2D( Math::Lerp( wpUV0[0].x, wpUV0[1].x, 3.0f ), wpUV0[1].y ),
                         Vector2D( Math::Lerp( wpUV1[0].x, wpUV1[1].x, 3.0f ), wpUV1[1].y ),
                         vpUVNull );















        GfxVBUnlock( vbidRippleSprite );

        ScopedShaderDefinitionLockR pdefShader( snoShader );
        GfxWorldMatrixSet( cgm4x4Identity );
        GfxViewMatrixSet( cgm4x4Identity );

        Matrix4x4 mProj;
        GfxBuild2DUIProjectionMatrix( mProj );
        GfxProjectionMatrixSet( mProj );


        IRect2D rectMainViewport;
        rectMainViewport.left = 0; rectMainViewport.top = 0;

        GfxGetRenderTargetDimensions( (uint32 &)rectMainViewport.right, (uint32 &)rectMainViewport.bottom );
        GfxViewportSet( rectMainViewport );



        if ( snoTex0 != SNO_NONE && !Prefetch::AddToQueue( SNOName( TEXTURE_SNO, snoTex0 ), 2 ) ) {

            return FALSE;
        }

        ShaderFloat tTex0Dimensions;
        if ( snoTex0 != SNO_NONE ) {

            ScopedTextureDefinitionLockR pdefTexture( snoTex0 );
            ShaderFloatSet( tTex0Dimensions, 1.0f / pdefTexture->dwWidth, 1.0f / pdefTexture->dwHeight, 0.0f, 0.0f );
        }


        for ( int32 nPassIndex = 0; nPassIndex < pdefShader->nPassCount; nPassIndex++ ) {

            GfxShaderInit( pdefShader, nPassIndex );


            GfxShaderSetIB( ptSubObjectGlobals->ibidRippleSprite, 0 );
            GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, vbidRippleSprite );

            const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding( 0 );
            if ( ptBinding ) {

                GfxShaderSetConstant( ptBinding, tTex0Dimensions, 1 );
            }


            GfxShaderSetTexture( GfxGetTextureBinding( SHADERTEX_DIFFUSE ), snoTex0 );
            GfxShaderSetTexture( GfxGetTextureBinding( SHADERTEX_ALPHA_LAYER_1 ), snoTex1 );
            GfxShaderSetTexture( GfxGetTextureBinding( SHADERTEX_ALPHA_LAYER_2 ), SNO_NONE );




            uint32 dwTriCount = 1;



            GfxShaderDraw( DRAWMODE_TRIANGLELIST, dwStartIndex, 0, VERTICES_PER_RIPPLE, 0, dwTriCount, PERFBIN_BACKGROUND );






            bDrewSomething = TRUE;
        }


        CameraManagerRestoreCurrentCameraToMainCamera();
    }
    return bDrewSomething;
}




void SubObjectGfxDebugDepthDrawDeferred( RWindow & tRWindow ) {
    SNO snoShader = GlobalSNOGet( GLOBALSNO_SUBOBJECT_DEBUG_DEPTH_PMA_SHADER );
    Rect2D rect( 0.0f, 0.0f, 512.0f, 512.0f );
    ImageTextureFrame tImageTexFrame( tRWindow.GetPMATarget(), -1 );
    SpriteDrawDeferred( tImageTexFrame, snoShader, rect, crgbaWhite, crgbaBlack, EPSILON, NULL, -1, NULL, 0, RENDERLAYER_INVALID, TRUE );
}






void DisplayListBuildProjectionLayers(
    RWindow & tRWindow ) {

    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex( 1 )];
    DisplayList & dispListMain = tAsyncRVData.dispList;

    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    int32 nShadowQuality = Prefs::GetVideoPrefs().GetShadowQuality();
    GameMode eMode = CGameGetGameCommonData().GameModeGet();

    if ( nShadowQuality &&
         ( eMode == GAMEMODE_PLAYING || eMode == GAMEMODE_MENU ) &&
         tRWindow.GetRWindowID() == RWindowMainWindow ) {





























        uint32 dwShadowTech = GfxGetShadowTech();


        if ( dwShadowTech == GFXSHADOWTECH_VSM ) {

            GfxSetRenderAndDepthTarget( ptSubObjectGlobals->snoTexShadowMapColor2, ptSubObjectGlobals->snoTexShadowMapDepth, CUBEMAPFACE_POSX );

        } else {

            GfxSetRenderAndDepthTarget( ptSubObjectGlobals->snoTexShadowMapColor, ptSubObjectGlobals->snoTexShadowMapDepth, CUBEMAPFACE_POSX );

        }


        // inferred: branch sense and colours from the original layout (inlined RGBAColor scope at 1781 holds the zero store that comes first)
        if ( dwShadowTech == GFXSHADOWTECH_REFRAST ) {

            GfxRenderClear( TRUE, TRUE, TRUE, RGBAColor( 255, 255, 255, 255 ) );





        } else {

            GfxRenderClear( TRUE, TRUE, TRUE, RGBAColor( 0, 0, 0, 0 ) );
        }


        sRenderCookieShadows( dispListMain );
    }






    GfxSetRenderAndDepthTarget( tRWindow.GetPMATarget(), SNO_NONE, CUBEMAPFACE_POSX );
    GfxRenderClear( TRUE, FALSE, FALSE, RGBAColor( 0, 0, 0, 255 ) );


    BOOL bDebugDepthPMA = sgtAppGlobals.eDebugRenderMode == DRM_PMA_DEPTH_COMPLEXITY;
    if ( bDebugDepthPMA ) {

        sgtAppGlobals.eDebugRenderMode = DRM_DEPTH_COMPLEXITY;
    }

    CameraManagerSetCurrentCamera( tRWindow.GetPMACameraIndex() );
    if ( dispListMain.ptRenderLayerData[RENDERLAYER_PROJECTED_ADDITIVE].nRenderableHeaderCount ) {

        dispListMain.DrawRenderLayer( RENDERLAYER_PROJECTED_ADDITIVE );
    }
    CameraManagerSetCurrentCamera( tRWindow.GetCameraIndex() );

    if ( bDebugDepthPMA ) {

        sgtAppGlobals.eDebugRenderMode = DRM_PMA_DEPTH_COMPLEXITY;
    }



    if ( tRWindow.GetRWindowID() == RWindowMainWindow ) {



        if ( SubObjectWaterTextureGetUpdate() ) {


            // inferred: copy-initialised from cgwpNull and overwritten when valid (one Vector3D::operator= at 1828; select code on 1826 at cols 17/30), as at line 2251
            Vector3D wpPlayerPos( cgwpNull );
            if ( tAsyncRVData.fLocalPlayerValid ) {

                wpPlayerPos = tAsyncRVData.wpLocalPlayerPosition;
            }

            GfxSetRenderAndDepthTarget( ptSubObjectGlobals->snoTexWater, SNO_NONE, CUBEMAPFACE_POSX );






            BOOL bDrewRippleBackground = sDrawRippleBackground( GlobalSNOGet( GLOBALSNO_WATER_SURFACE_APPEARANCE ), ptSubObjectGlobals->tRippleDrawSubObjectState, wpPlayerPos );






            if ( !bDrewRippleBackground ) {


                GfxRenderClear( TRUE, FALSE, FALSE, RGBAColor( 0, 0, 0, 0 ) );
            }




            CameraManagerSetCurrentCamera( CAMERA_RIPPLEMAP );
            if ( dispListMain.ptRenderLayerData[RENDERLAYER_WATER_SIM].nRenderableHeaderCount ) {

                dispListMain.DrawRenderLayer( RENDERLAYER_WATER_SIM );
            }
            CameraManagerSetCurrentCamera( tRWindow.GetCameraIndex() );


            SubObjectWaterTextureSetUpdate( FALSE );
        }



        if ( SubObjectInteractiveFogTextureGetUpdate() ) {





            GfxSetRenderAndDepthTarget( ptSubObjectGlobals->snoTexInteractiveFog, SNO_NONE, CUBEMAPFACE_POSX );
            GfxRenderClear( TRUE, FALSE, FALSE, RGBAColor( 0, 0, 0, 255 ) );
            CameraManagerSetCurrentCamera( CAMERA_RIPPLEMAP );
            if ( dispListMain.ptRenderLayerData[RENDERLAYER_INTERACTIVE_FOG].nRenderableHeaderCount ) {

                dispListMain.DrawRenderLayer( RENDERLAYER_INTERACTIVE_FOG );
            }
            CameraManagerSetCurrentCamera( tRWindow.GetCameraIndex() );


            SubObjectInteractiveFogTextureSetUpdate( FALSE );
        }
    }



    GfxSetRenderTarget( PostFXGetBuffer( EFFECTBUFFER_DEFAULT_SOFTPARTICLE_DEPTH ), CUBEMAPFACE_POSX );
    GfxSetDepthTarget( PostFXGetDepthBuffer( EFFECTBUFFER_DEFAULT_SOFTPARTICLE_DEPTH ) );
    GfxRenderClear( FALSE, TRUE, TRUE, RGBAColor( 0, 0, 0, 0 ) );


    tRWindow.SetRenderAndDepthTargetToDefaults();
}




static void sBuildProjectionTextureMatrix(
    Matrix4x4 & mTex ) {

    const Camera & shadowCam = CameraManagerGetCamera( CAMERA_SHADOW );

    mTex = shadowCam.mView * shadowCam.mProjection;
    mTex = mTex * cgm4x4ProjectionToNDC;
}




BOOL sSetDiffuseTextureForPMA(
    const UberMaterial & tUberMaterial,
    const TexAnimState * ptTexAnimState ) {

    TextureBinding * ptTexBinding = GfxGetTextureBinding( SHADERTEX_DIFFUSE_LAYER_1 );
    if ( !ptTexBinding ) {
        return FALSE;
    }


    SNO snoTex;
    int32 nCount = MaterialGetTextureCount( tUberMaterial );
    const MaterialTextureEntry * ptMatTexEntry = MaterialLookup( tUberMaterial.ptMatTexList, nCount, SHADERTEX_DIFFUSE );
    if ( !ptMatTexEntry ) {
        return FALSE;
    }


    SNO snoShader = ShaderMapGetDefaultShader( tUberMaterial.snoShaderMap, TRUE, NULL );
    ScopedShaderDefinitionLockR pdefShader( snoShader );
    if ( !TagMapGetDWORD( pdefShader->ptPasses->hTagMap, TAG_VS_DIFF_ALPHA_IS_GLOSS ) ) {

        snoTex = ptMatTexEntry->tMatTex.snoTex;

    } else {

        snoTex = GlobalSNOGet( GLOBALSNO_OPAQUE_WHITE_TEXTURE );
    }
    GfxShaderSetTexture( ptTexBinding, snoTex );
    const MaterialTexture & tMatTex = ptMatTexEntry->tMatTex;

    TexAnimApply( ptTexBinding, tMatTex.pTexAnimCompParams, tMatTex.serTexAnimCompParams.dwSize, ptTexAnimState[ptMatTexEntry - tUberMaterial.ptMatTexList], NULL );

    return TRUE;



}




void sSetPMAMaskTextureForPMA(
    const UberMaterial & tUberMaterial,
    TextureBinding * ptPMAMaskTexBinding ) {

    if ( ptPMAMaskTexBinding ) {
        // Inference: snoTex starts as SNO_NONE (no instruction; a missing entry goes straight to the default at 1970).
        SNO snoTex = SNO_NONE;
        int32 nCount = MaterialGetTextureCount( tUberMaterial );
        const MaterialTextureEntry * ptMatTexEntry = MaterialLookup( tUberMaterial.ptMatTexList, nCount, SHADERTEX_PMA_MASK );
        if ( ptMatTexEntry ) {

            const MaterialTexture & tMatTex = ptMatTexEntry->tMatTex;
            snoTex = tMatTex.snoTex;
        }
        if ( snoTex == SNO_NONE ) {

            snoTex = GlobalSNOGet( GLOBALSNO_SUBOBJECT_PMA_MASK_TEXTURE );
        }

        GfxShaderSetTexture( ptPMAMaskTexBinding, snoTex );
    }
}




static void sShaderSetPlayerPos() {


    const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
    const ShaderConstFastBinding * ptFastBinding = GfxShaderGetConstantFastBinding( ptFastBindings, VSC_PLAYERPOS );

    {
        AsyncRWindowData & tAsyncRVData = GetRWindowMgr()->GetCurrentRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( 1 )];
        if ( tAsyncRVData.fLocalPlayerValid ) {

            const Vector3D & wp = tAsyncRVData.wpLocalPlayerPosition;

            float flPeriod = 240.0f;
            Angle a = Math::NormalizeAngle( CGameGetClientTime() * FULL_CIRCLE / flPeriod );
            float flT = Math::Sin( a ) * 0.25f + 0.75f;

            ShaderFloat tPos = { wp.x, wp.y, wp.z, flT };
            GfxShaderSetConstant( ptFastBinding, tPos, 1 );
        } else {


            ShaderFloat tPos = { 0.0f, 1.0f, 0.0f, 0.0f };
            GfxShaderSetConstant( ptFastBinding, tPos, 1 );
        }
    }
}




static TextureBinding * sShaderInitForRedraw( const SNO snoShader, const SNO snoLayerTexture, const Matrix4x4 & mTex ) {






    const int32 nPassIndex = 0;
    GfxShaderInit( snoShader, nPassIndex );
    GfxShaderSetTextureFactor( crgbaBlack );
    sShaderSetPlayerPos();

    TextureBinding * ptDiffuseTexBinding = GfxGetTextureBinding( SHADERTEX_DIFFUSE );
    if ( ptDiffuseTexBinding ) {

        GfxShaderSetTexture( ptDiffuseTexBinding, snoLayerTexture );
        GfxShaderTexMatrixSet( ptDiffuseTexBinding, mTex );
    }
    TextureBinding * ptAlphaTexBinding = GfxGetTextureBinding( SHADERTEX_ALPHA );
    if ( ptAlphaTexBinding ) {

        GfxShaderSetTexture( ptAlphaTexBinding, GlobalSNOGet( GLOBALSNO_SUBOBJECT_SHADOW_MAP_MASK ) );
        GfxShaderTexMatrixSet( ptAlphaTexBinding, mTex );
    }
    TextureBinding * ptPMAMaskTexBinding = GfxGetTextureBinding( SHADERTEX_PMA_MASK );
    if ( ptPMAMaskTexBinding ) {

        GfxShaderTexMatrixSet( ptPMAMaskTexBinding, cgm4x4Identity );
    }
    return ptPMAMaskTexBinding;
}





static void sSetRandFloatBinding(
    const RenderableDetails & tDetails,
    const RenderPass & tPass,
    const ShaderConstFastBinding * ptFastBinding ) {

    float flClientTime = GetRWindowMgr()->GetMainRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( 1 )].flClientTime;

    float flPeriod = TagMapGetFloat( tPass.hTagMap, TAG_VS_TIMER_PERIOD ) + tDetails.tAppearance.flRand;

    float flTrunc = Math::FloatTrunc( flClientTime / flPeriod );

    float normalizedClientTime = ( flClientTime - flPeriod * flTrunc ) / flPeriod;
    float periodicClientTime = normalizedClientTime * 2.0f * PI;

    ShaderFloat tParams = {
        tDetails.tAppearance.flRand,

        tDetails.tAppearance.flRand * 2.0f * PI,
        normalizedClientTime,
        periodicClientTime };


    GfxShaderSetConstant( ptFastBinding, tParams, 1 );
}




static void sRedrawRenderablesWithProjectedAdditive(
    RWindow & tRWindow,
    const SNO snoShader,
    const Vector3D & wp ) {

    const SNO snoLayerTexture = tRWindow.GetPMATarget();
    const DisplayList & dispListProjected = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex( 1 )].dispListProjected;




    Matrix4x4 mTex;
    const Camera & viewCam = CameraManagerGetCamera( tRWindow.GetCameraIndex() );
    const Camera & addCam = CameraManagerGetCamera( tRWindow.GetPMACameraIndex() );

    mTex = cgm4x4Identity;
    mTex._11 = 0.5f;
    mTex._41 = 0.5f;
    mTex._22 = -0.5f;
    mTex._42 = 0.5f;
    mTex = viewCam.mView.Inverse() * addCam.mView * addCam.mProjection * mTex;

    TextureBinding * ptPMAMaskTexBinding = sShaderInitForRedraw( snoShader, snoLayerTexture, mTex );

    BOOL fResetShader = FALSE;
    // Inference: the bound is written != NUM_RENDERLAYERS (with < the loop is rotated differently: csel skip of layer 9 instead of the original re-test after the increment).
    for ( int eRenderLayer = 0; eRenderLayer != NUM_RENDERLAYERS; eRenderLayer++ ) {

        if ( eRenderLayer == RENDERLAYER_PROJECTED_SHADOWMAP ) {

            continue;
        }


        RenderLayerData & tRenderLayer = dispListProjected.ptRenderLayerData[eRenderLayer];

        for ( int32 i = 0; i < tRenderLayer.nRenderableHeaderCount; i++ ) {

            const RenderableHeader & tHeader = tRenderLayer.ptRenderableHeaderList[i];
            const RenderableDetails & tDetails = dispListProjected.ptRenderableDetailsList[tHeader.nRenderableIndex];

            if ( !sRenderableTypeHasAppearance( tDetails.tCommon.eType ) ) {
                continue;
            }
            if ( tDetails.tAppearance.pdefAppearance ) {


                if ( TagMapGetInt( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_NO_RECEIVE_FX ) ) {
                    continue;
                }


                SNO snoFXShader = TagMapGetSNO( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_FX_SHADER );
                if ( snoFXShader != SNO_NONE ) {


                    ptPMAMaskTexBinding = sShaderInitForRedraw( snoFXShader, snoLayerTexture, mTex );

                    ScopedShaderDefinitionLockR pdefShader( snoFXShader );
                    const RenderPass & tPass = *pdefShader->ptPasses;
                    const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
                    const ShaderConstFastBinding * ptFastBinding = GfxShaderGetConstantFastBinding( ptFastBindings, VSC_RAND_FLOAT );

                    fResetShader = TRUE;
                    sSetRandFloatBinding( tDetails, tPass, ptFastBinding );
                } else {

                    if ( fResetShader ) {

                        ptPMAMaskTexBinding = sShaderInitForRedraw( snoShader, snoLayerTexture, mTex );
                    }
                    fResetShader = FALSE;
                }
            }
            const SubObject & tSubObject = *tDetails.tAppearance.ptSubObject;


            if ( tDetails.tCommon.eType == RENDERABLETYPE_SCENE ) {

                Sphere ws;
                tSubObject.aabbBounds.GetMinimalBoundingSphere( ws );
                ws.wpCenter = ws.wpCenter * tDetails.tCommon.mWorld;
                const WorldDistance wdEffectDist = ws.wdRadius + 77.0f;
                if ( Vector3D::Distance2DSquared( ws.wpCenter, wp ) > wdEffectDist * wdEffectDist ) {
                    continue;
                }
            }



            if ( !sSetDiffuseTextureForPMA( tDetails.tAppearance.ptSOA->tCompositeUberMaterial,
                                            tDetails.tAppearance.arTexAnimStates ) ) {
                continue;
            }


            sSetPMAMaskTextureForPMA( tDetails.tAppearance.ptSOA->tCompositeUberMaterial, ptPMAMaskTexBinding );

            GfxWorldMatrixSet( tDetails.tCommon.mWorld );

            if ( !( tSubObject.dwFlags & ( ( 1 << SUBOBJECT_EDGE_GEOMETRY_BIT ) | ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) ) ) ) {


                PackedVBEntry & tEntry = tSubObject.ptPostprocessed->tVBEntry;
                PackedIBEntry & tEntryIB = tSubObject.ptPostprocessed->tIBEntry;



                GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, tEntry.ptPVB->vbid );
                GfxShaderSetIB( tEntryIB.ptPIB->ibid, tEntryIB.dwIndexOffset );
                sDrawTriBatch( 0, tSubObject.nVertCount, tEntry.dwVertexOffset, 0, tSubObject.nIndexCount, tSubObject.dwFlags & ( 1 << SUBOBJECT_UNSTRIPIFIED_BIT ), PERFBIN_BACKGROUND );


            } else if ( ( tSubObject.dwFlags & ( ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) | ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) ) == ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) ) {


                PackedVBEntry & tEntry = tSubObject.ptPostprocessed->tPackVBEntry;
                PackedIBEntry & tEntryIB = tSubObject.ptPostprocessed->tIBEntry;



                GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, tEntry.ptPVB->vbid );
                GfxShaderSetIB( tEntryIB.ptPIB->ibid, tEntryIB.dwIndexOffset );
                Vector3D vecScale( tSubObject.wpFixedPointPosScale.x * 32767.0f, tSubObject.wpFixedPointPosScale.y * 32767.0f, tSubObject.wpFixedPointPosScale.z * 32767.0f );
                GfxFixedPointPosSet( vecScale, tSubObject.wpFixedPointPosOffset );
                sDrawTriBatch( 0, tSubObject.nVertCount, tEntry.dwVertexOffset, 0, tSubObject.nIndexCount, tSubObject.dwFlags & ( 1 << SUBOBJECT_UNSTRIPIFIED_BIT ), PERFBIN_BACKGROUND );
                GfxFixedPointPosSet( cgwvOne, cgwvNull );

            } else if ( tSubObject.dwFlags & ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) {


                Vector3D vecScale( tSubObject.wpFixedPointPosScale.x * 32767.0f, tSubObject.wpFixedPointPosScale.y * 32767.0f, tSubObject.wpFixedPointPosScale.z * 32767.0f );
                GfxFixedPointPosSet( vecScale, tSubObject.wpFixedPointPosOffset );

                int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof( SkinningInfo );
                for ( int32 nSkinningInfo = 0; nSkinningInfo < nSkinningInfoCount; nSkinningInfo++ ) {

                    SkinningInfo & tSkinningInfo = ( (SkinningInfo *)tSubObject.ptSkinningInfo )[nSkinningInfo];

                    PackedVBEntry64 & tEntry = tSkinningInfo.tPackVBEntry;
                    PackedIBEntry64 & tEntryIB = tSkinningInfo.tPackIBEntry;

                    GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, tEntry.ptPVB->vbid );
                    GfxShaderSetIB( tEntryIB.ptPIB->ibid, tEntryIB.dwIndexOffset );
                    sDrawTriBatch( 0, tSkinningInfo.nPackVertCount, tEntry.dwVertexOffset, 0, tSkinningInfo.nPackIndexCount, tSubObject.dwFlags & ( 1 << SUBOBJECT_UNSTRIPIFIED_BIT ), PERFBIN_BACKGROUND );
                }

                GfxFixedPointPosSet( cgwvOne, cgwvNull );
            }
        }
    }
}








void DisplayListDrawProjectionLayers(
    RWindow & tRWindow ) {

    BOOL fUseAlternatePMAShader = FALSE;

    SNO snoWorld = tRWindow.GetCurrentWorld();
    if ( snoWorld != SNO_NONE ) {

        ScopedWorldDefinitionLockR pdefWorld( snoWorld );
        fUseAlternatePMAShader = pdefWorld->uFlags & ( 1 << WDF_ALTERNATE_PMA );
    }
    // Inference: one GlobalSNOGet call in the original (0x91 | 2 when the flag is set); written as a choice between two calls that clang merges.
    SNO snoShader = fUseAlternatePMAShader ?
        GlobalSNOGet( GLOBALSNO_SUBOBJECT_PROJECTED_LAYER__ALTERNATE_SHADER ) :
        GlobalSNOGet( GLOBALSNO_SUBOBJECT_PROJECTED_LAYER_SHADER );

    Vector3D wpPos( cgwpNull );
    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex( 1 )];
    if ( tAsyncRVData.fLocalPlayerValid ) {

        wpPos = tAsyncRVData.wpLocalPlayerPosition;
    }



    sRedrawRenderablesWithProjectedAdditive( tRWindow, snoShader, wpPos );
}




static void sHookupBanner(
    const RenderableDetails & tDetails ) {

    TextureBinding * ptTexBinding = GfxGetTextureBinding( SHADERTEX_BANNER );
    if ( ptTexBinding ) {


        BannerRenderData * ptBanner = tDetails.tAppearance.ptBannerRenderData;
        if ( !ptBanner ) {
            // Inference: the binary calls GameModeIsAnEditMode at 2275:18 (+0xc3c) and never tests its result; the use the original made of it (2276-2281) left no code.
            GameModeIsAnEditMode( CGameGetGameCommonData().GameModeGet() );
            // Inference: the null-banner path falls straight out of the helper after the call above (+0xc40), so the block ends in a return.
            return;
        }



        static ShaderTex shaderTexs[] = {
            SHADERTEX_BANNER,
            SHADERTEX_DIFFUSE_LAYER_1,
            SHADERTEX_DIFFUSE_LAYER_2,
            SHADERTEX_DIFFUSE_LAYER_3 };



        if ( ptBanner->snoTexBanners[0] != SNO_NONE ) {

            for ( int i = 0; i < 4; i++ ) {

                if ( ptBanner->snoTexBanners[i] != SNO_NONE ) {

                    GfxShaderSetTexture( GfxGetTextureBinding( shaderTexs[i] ), ptBanner->snoTexBanners[i] );
                }
            }
        }

        const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding( VSC_BANNER_TINT_RAMP_UV );
        if ( ptBinding ) {

            ShaderFloat tParams =
            {
                ptBanner->flTintU[0], ptBanner->flTintU[1], ptBanner->flTintU[2],
                1.0f };
            GfxShaderSetConstant( ptBinding, tParams, 1 );
        }
    }
}




static void sApplySubObjectConstants(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails,
    const RenderPass & tPass ) {

    const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();


    if ( GfxShaderHasConstant( ptFastBindings, VSC_EDGEALPHAPARAMS ) ) {

        const float flAlphaFalloffPower = TagMapGetFloat( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_EDGEALPHA_FALLOFF_POWER );
        const float flColorFalloffPower = TagMapGetFloat( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_EDGECOLOR_FALLOFF_POWER );
        ShaderFloat tRegTemp;
        sComputeAppearanceFXParms( tRegTemp, tDetails );
        tRegTemp[0] *= flAlphaFalloffPower;
        tRegTemp[1] = flColorFalloffPower;
        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_EDGEALPHAPARAMS ), tRegTemp, 1 );
    }

    sShaderSetPlayerPos();

    if ( GfxShaderHasConstant( ptFastBindings, VSC_MESHLIGHTING ) ) {




        RGBAColorValue meshLightingContribution( 1.0f, 1.0f, 1.0f, 1.0f );
        if ( tDetails.tCommon.eType == RENDERABLETYPE_RACTOR ) {

            meshLightingContribution = RActorGetMeshLighting( (RActorID)tDetails.tCommon.id );
        }

        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_MESHLIGHTING ), &meshLightingContribution, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_SHADOWCOLOR ) ) {
        // Inference: order from the frame slots (flShadowFactor -0xa4, flAOFactor -0xa8, flAOShadowFactor -0xac, passed as x1/x2/x3); the DWARF lists these three in the reverse order.
        float flShadowFactor, flAOFactor, flAOShadowFactor;
        Weather::GetShadowColor( GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData(), flShadowFactor, flAOFactor, flAOShadowFactor );
        float flShadowMapSize = Prefs::GetVideoPrefs().GetShadowMapSize();
        if ( tHeader.fSetShadowLevel == FALSE ) {

            flShadowFactor = 1.0f;
        }


        ShaderFloat tParams =
        {
            flShadowFactor,
            flShadowMapSize,
            flAOShadowFactor,
            flAOFactor };


        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_SHADOWCOLOR ), tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_DIFFUSEOVERLAY ) ) {

        float f = 0.0f;

        if ( tDetails.tAppearance.dwOverlayFadeTimeOriginal ) {


            f = Math::Sqrt( tDetails.tAppearance.dwOverlayFadeTimeCurrent / (float)tDetails.tAppearance.dwOverlayFadeTimeOriginal );
        }

        ShaderFloat tParams =
        { f, f, f, f };


        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_DIFFUSEOVERLAY ), tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_APPEARANCE_FX ) ) {

        ShaderFloat tRegTemp;
        sComputeAppearanceFXParms( tRegTemp, tDetails );
        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_APPEARANCE_FX ), tRegTemp, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_HITFLASHADDCOLOR ) ) {

        float f = TagMapGetFloat( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_HIT_FLASH_POWER );

        ShaderFloat tParams =
        {
            tDetails.tAppearance.rgbavalHitFlashAdd[0],
            tDetails.tAppearance.rgbavalHitFlashAdd[1],
            tDetails.tAppearance.rgbavalHitFlashAdd[2],
            f };

        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_HITFLASHADDCOLOR ), tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_HITFLASHMULCOLOR ) ) {

        ShaderFloat tParams =
        {
            tDetails.tAppearance.rgbavalHitFlashMul[0],
            tDetails.tAppearance.rgbavalHitFlashMul[1],
            tDetails.tAppearance.rgbavalHitFlashMul[2],
            1.0f };

        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_HITFLASHMULCOLOR ), tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_TINT_RAMP_UV ) ) {

        float flV = 0.0f;
        if ( tDetails.tAppearance.eDyeType >= DYE_TYPE_FIRST_COLOR ) {

            flV = ( (float)( tDetails.tAppearance.eDyeType - DYE_TYPE_FIRST_COLOR ) + 0.5f ) / NUM_DYE_COLORS;
        }
        ShaderFloat tParams =
        { flV, flV, flV, flV };


        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_TINT_RAMP_UV ), tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_RAND_FLOAT ) ) {

        sSetRandFloatBinding( tDetails, tPass, GfxShaderGetConstantFastBinding( ptFastBindings, VSC_RAND_FLOAT ) );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_USE_DYE_TYPE ) ) {

        RGBAColorValue rgbaColorValue;
        if ( tDetails.tAppearance.eDyeType == DYE_TYPE_NONE ) {

            rgbaColorValue.Set( 0.0f, 0.0f, 0.0f, 0.0f );
        } else {


            rgbaColorValue.Set( 1.0f, 1.0f, 1.0f, 1.0f );
        }
        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_USE_DYE_TYPE ), &rgbaColorValue, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_NORMALMAPSCALE ) ) {

        float f = TagMapGetFloat( tDetails.tAppearance.ptSOA->hTagMapMasterMaterial, TAG_APPEARANCE_NORMAL_MAP_SCALE );
        if ( TagMapGetInt( tPass.hTagMap, TAG_VS_PERPIXEL_LIGHTING ) ) {

            f *= Weather::GetNormalMapFactor( GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData() );
            f *= sgtAppGlobals.flNormalMapFactor;
        }
        ShaderFloat tParams =
        { f, f, f, f };


        GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_NORMALMAPSCALE ), tParams, 1 );
    }

    if ( GfxShaderHasConstant( ptFastBindings, VSC_DBG_REG0 ) ) {

        switch ( sgtAppGlobals.eDebugRenderMode ) {
            case DRM_GLOSS: {


                const RGBAColorValue & rgbavalMatSpecular = tDetails.tCommon.mat.rgbavalSpecular;
                GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_DBG_REG0 ), &rgbavalMatSpecular, 1 );
                break;
            }
            case DRM_DIFFUSE_LIGHTING: {



                ShaderFloat tParams = { 0.0f, 1.0f, 0.0f, 0.0f };




                GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_DBG_REG0 ), tParams, 1 );
                break;
            }
            case DRM_SPECULAR_LIGHTING: {


                ShaderFloat tParams = { 0.0f, 0.0f, 1.0f, 0.0f };




                GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_DBG_REG0 ), tParams, 1 );
                break;
            }
            default: {



                ShaderFloat tParams = { 1.0f, 0.0f, 0.0f, 0.0f };




                GfxShaderSetConstant( GfxShaderGetConstantFastBinding( ptFastBindings, VSC_DBG_REG0 ), tParams, 1 );
                break;
            }
        }
    }
}









static void sDrawImmediate(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails,
    const RenderLayer eRenderLayer,
    BOOL fSupportsLights)
{



    // Inference: nStartIndex leaves no code; every draw passes a constant 0 as the start index (w4 = wzr at +0xd44, +0xd7c, +0xebc), so it starts at 0 and is never changed.
    int32 nStartIndex = 0;
    int32 nIndexCount;
    uint32 dwIBOffset = 0;
















    // Inference: the FALSE initialiser has no row of its own (stur wzr at +0x158 and +0x1048 carry line 0); QueryEnd at 2916 is skipped on the paths that do not begin a query.
    BOOL fMustEndQuery = FALSE;
    if ( tDetails.tAppearance.ptOcclusionHighlighting && tHeader.fUseHighlight ) {

        if ( eRenderLayer != RENDERLAYER_HIGHLIGHT ) {

            tDetails.tAppearance.ptOcclusionHighlighting->QueryBegin();
            // Inference: the store of 1 to fMustEndQuery is tail-duplicated into both exits of the inlined QueryBegin (3927:9 at +0xfcc and line 0 at +0x1018); QueryBegin returns void.
            fMustEndQuery = TRUE;
        } else {

            if ( !tDetails.tAppearance.ptOcclusionHighlighting->Occluded() ) {
                return;
            }
        }
    }




    GfxWorldMatrixSet( tDetails.tCommon.mWorld );



    const SubObject & tSubObject = *tDetails.tAppearance.ptSubObject;

    nIndexCount = tSubObject.nIndexCount;

    BOOL bIsCloth = tDetails.tAppearance.idCloth != CLOTHID_INVALID;

    VBID vbid;
    uint32 dwStreamOffset;



    BOOL bEdgeGeometryAndCloth = ( tSubObject.dwFlags & ( 1 << SUBOBJECT_EDGE_GEOMETRY_BIT ) ) && bIsCloth && tSubObject.nVertCount > 0;

    BOOL bPackGeometryAndCloth = ( tSubObject.dwFlags & ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) ) && bIsCloth && ( tSubObject.nVertCount > 0 || !( tSubObject.dwFlags & ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) );

    if ( ( !( tSubObject.dwFlags & ( 1 << SUBOBJECT_EDGE_GEOMETRY_BIT ) ) || bEdgeGeometryAndCloth ) &&
         ( !( tSubObject.dwFlags & ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) || bPackGeometryAndCloth ) ) {

        vbid = tDetails.tAppearance.ptEntry->ptPVB->vbid;
        dwStreamOffset = tDetails.tAppearance.ptEntry->dwStreamOffset;
    } else {
        // Inference: the else values have no rows of their own (mov w16, #-1 and stur wzr at +0x250/+0x254 carry line 0).
        vbid = VBID_INVALID;
        // Inference: as above.
        dwStreamOffset = 0;
    }
    if ( sgtAppGlobals.eDebugRenderMode == DRM_AO || sgtAppGlobals.eDebugRenderMode == DRM_AO_SM ) {


        if ( MaterialLookup( tDetails.tAppearance.ptSOA->tCompositeUberMaterial, SHADERTEX_LIGHTMAP ) == NULL &&
             tDetails.tAppearance.snoAmbientOcclusionOverrideTex == SNO_NONE ) {
            return;
        }
    }


    const ShaderDefinition * pdefShader = tHeader.pdefShader;









    uint32 dwFlags = tSubObject.dwFlags;

    BOOL fVSIsHardwareSkinned = pdefShader->ptPasses[0].tPostprocessed.ptVSD->dwFlags & ( 1 << VSPP_DOES_HARDWARE_SKINNING_BIT );

    for ( int32 nPassIndex = 0; nPassIndex < pdefShader->nPassCount; nPassIndex++ ) {

        const RenderPass & tPass = pdefShader->ptPasses[nPassIndex];


        if ( !tPass.tPostprocessed.ptVSD->lpSH || !tPass.tPostprocessed.ptPSD->lpSH ) {
            continue;
        }


        if ( tPass.eRenderLayer != eRenderLayer ) {
            continue;
        }


        BOOL fVSRequiresDynamicVB = ( tPass.eVertexPreprocessStep != VPS_NONE ) || ( tPass.dwFlags & ( 1 << VS_REQUIRES_DYNAMIC_VBS_BIT ) ) || bIsCloth;
        BOOL fVSNeedsTangentBasis = tPass.tPostprocessed.ptVSD->dwFlags & ( 1 << VSPP_NEEDS_TANGENT_BASIS_BIT );


        if ( fVSIsHardwareSkinned != ( tPass.tPostprocessed.ptVSD->dwFlags & ( 1 << VSPP_DOES_HARDWARE_SKINNING_BIT ) ) ) {
            continue;
        }




        if ( fVSIsHardwareSkinned && !tDetails.tAppearance.ptBoneMapping ) {
            continue;
        }
















        if ( fVSRequiresDynamicVB ) {

            vbid = cgtVBFormatInfo[VBFORMAT_2TEX_L].vbidPooled;
            // Inference: the 0 is selected by the csel at +0x4dc (line 2676) and has no row of its own.
            dwStreamOffset = 0;
        }



        if ( ( !( dwFlags & ( 1 << SUBOBJECT_EDGE_GEOMETRY_BIT ) ) || bEdgeGeometryAndCloth ) &&
             ( !( dwFlags & ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) || bPackGeometryAndCloth ) ) {


            if ( !GfxVBIsValid( vbid ) ) {
                continue;
            }
        }



        GfxShaderSetPointLinearLightCount( nPassIndex, pdefShader, tDetails.tCommon.nLightCount, tDetails.tCommon.pArrayLights );

        GfxShaderInit( pdefShader, nPassIndex );
        // Inference: second test of the 2685 condition; clang threads it into the two copies of 2696/2698 (+0x500 and +0x538), so it has no row of its own.
        if ( ( !( dwFlags & ( 1 << SUBOBJECT_EDGE_GEOMETRY_BIT ) ) || bEdgeGeometryAndCloth ) &&
             ( !( dwFlags & ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) || bPackGeometryAndCloth ) ) {
            GfxShaderSetStream( STREAMINDEX_DEFAULT, dwStreamOffset, vbid );
        }


        int32 nVertexCount = tSubObject.nVertCount;

        GfxShaderSetTextureFactor( tHeader.rgbaColor );

        sApplySubObjectConstants( tHeader, tDetails, tPass );








        if ( tPass.eVertexPreprocessStep == VPS_NONE &&
             ( !( dwFlags & ( 1 << SUBOBJECT_EDGE_GEOMETRY_BIT ) ) || bEdgeGeometryAndCloth ) &&
             ( !( dwFlags & ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) ) || bPackGeometryAndCloth ) ) {



            if ( GfxVBGetUsage( vbid ) == RESOURCEUSAGE_DYNAMIC ) {

                if ( bIsCloth ) {

                    if ( fVSIsHardwareSkinned ) {
                        continue;
                    }



                    Cloth * ptCloth = ClothManager::Get( tDetails.tAppearance.idCloth );

                    VBID vbidCloth = cgtVBFormatInfo[VBFORMAT_CLOTH].vbidPooled;

                    ptCloth->CopyAsyncVertices( vbid, vbidCloth, dwIBOffset );



                    GfxShaderSetStream( STREAMINDEX_CLOTH, 0, vbidCloth );
                } else {



                    GfxVBFormatCopyIntoVB( vbid, tSubObject.ptVertList, tSubObject.nVertCount, dwIBOffset );
                }
            }
        }




        if ( fVSIsHardwareSkinned ) {

            sSetupHardwareSkinned( tHeader, tDetails, fVSNeedsTangentBasis, nStartIndex, nIndexCount, nVertexCount );

        } else if ( !( dwFlags & ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) || bPackGeometryAndCloth ) {








            GfxShaderSetIB( tDetails.tAppearance.ptEntryIB->ptPIB->ibid, tDetails.tAppearance.ptEntryIB->dwIndexOffset );
        }




        GfxShaderSetTextures( tDetails.tAppearance.ptSOA->tCompositeUberMaterial, tDetails.tAppearance.arTexAnimStates, "", tDetails.tAppearance.snoAppearance );



        if ( !GfxShaderSetDiffuseOverlayTexture( tDetails.tAppearance.eOverlayType, tDetails.tAppearance.eOverlayProjectionAxis, tDetails.tAppearance.ptSOA->tCompositeUberMaterial, tDetails.tAppearance.arTexAnimStates, tDetails.tAppearance.nTexAnimStateCount ) ) {


            const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
            const ShaderConstFastBinding * ptFastBinding = GfxShaderGetConstantFastBinding( ptFastBindings, VSC_DIFFUSEOVERLAY );
            {

                ShaderFloat tParams = { 0.0f, 0.0f, 0.0f, 0.0f };



                GfxShaderSetConstant( ptFastBinding, tParams, 1 );
            }
        }



        sHookupBanner( tDetails );

        if ( tDetails.tAppearance.snoIrradianceTex != SNO_NONE ) {

            GfxShaderSetTexture( tDetails.tAppearance.snoIrradianceTex, SHADERTEX_IRRADIANCE );
        }
        if ( tDetails.tAppearance.snoAmbientOcclusionOverrideTex != SNO_NONE ) {

            GfxShaderSetLightmapTexture( tDetails.tAppearance.snoAmbientOcclusionOverrideTex, SHADERTEX_LIGHTMAP );
        }



































        if ( fSupportsLights ) {

            GfxShaderSetLights( tDetails.tCommon.mat, pdefShader, tDetails.tCommon.nLightCount, tDetails.tCommon.pArrayLights );
        }

        if ( ( !( dwFlags & ( 1 << SUBOBJECT_EDGE_GEOMETRY_BIT ) ) || bEdgeGeometryAndCloth ) && ( !( dwFlags & ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) ) || bPackGeometryAndCloth ) ) {













            sDrawTriBatch( 0, nVertexCount, dwIBOffset, nStartIndex, nIndexCount, dwFlags & ( 1 << SUBOBJECT_UNSTRIPIFIED_BIT ), tDetails.tCommon.ePerfBin );








        } else if ( ( dwFlags & ( ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) | ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) ) == ( 1 << SUBOBJECT_PACK_GEOMETRY_BIT ) ) {


            Vector3D vecScale( tSubObject.wpFixedPointPosScale.x * 32767.0f, tSubObject.wpFixedPointPosScale.y * 32767.0f, tSubObject.wpFixedPointPosScale.z * 32767.0f );
            GfxFixedPointPosSet( vecScale, tSubObject.wpFixedPointPosOffset );
            sDrawTriBatch( 0, nVertexCount, dwIBOffset, nStartIndex, nIndexCount, dwFlags & ( 1 << SUBOBJECT_UNSTRIPIFIED_BIT ), tDetails.tCommon.ePerfBin );
            GfxFixedPointPosSet( cgwvOne, cgwvNull );

        } else if ( dwFlags & ( 1 << SUBOBJECT_PACK_SKIN_BIT ) ) {

            if ( fVSIsHardwareSkinned ) {

                // Inference: this block (DWARF vecScale at 2882) has no rows; clang tail-merges it with the 2872-2875 block (cbz w26 at +0xccc falls into that code).
                Vector3D vecScale( tSubObject.wpFixedPointPosScale.x * 32767.0f, tSubObject.wpFixedPointPosScale.y * 32767.0f, tSubObject.wpFixedPointPosScale.z * 32767.0f );
                GfxFixedPointPosSet( vecScale, tSubObject.wpFixedPointPosOffset );
                sDrawTriBatch( 0, nVertexCount, dwIBOffset, nStartIndex, nIndexCount, dwFlags & ( 1 << SUBOBJECT_UNSTRIPIFIED_BIT ), tDetails.tCommon.ePerfBin );
                GfxFixedPointPosSet( cgwvOne, cgwvNull );

            } else {


                Vector3D vecScale( tSubObject.wpFixedPointPosScale.x * 32767.0f, tSubObject.wpFixedPointPosScale.y * 32767.0f, tSubObject.wpFixedPointPosScale.z * 32767.0f );
                GfxFixedPointPosSet( vecScale, tSubObject.wpFixedPointPosOffset );

                int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof( SkinningInfo );
                for ( int32 nSkinningInfo = 0; nSkinningInfo < nSkinningInfoCount; nSkinningInfo++ ) {

                    SkinningInfo & tSkinningInfo = ( (SkinningInfo *)tSubObject.ptSkinningInfo )[nSkinningInfo];

                    PackedVBEntry64 & tEntry = tSkinningInfo.tPackVBEntry;
                    PackedIBEntry64 & tEntryIB = tSkinningInfo.tPackIBEntry;

                    GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, tEntry.ptPVB->vbid );
                    GfxShaderSetIB( tEntryIB.ptPIB->ibid, tEntryIB.dwIndexOffset );
                    sDrawTriBatch( 0, tSkinningInfo.nPackVertCount, tEntry.dwVertexOffset, 0, tSkinningInfo.nPackIndexCount, dwFlags & ( 1 << SUBOBJECT_UNSTRIPIFIED_BIT ), PERFBIN_BACKGROUND );
                }

                GfxFixedPointPosSet( cgwvOne, cgwvNull );
            }
        }
    }




    if ( fMustEndQuery ) {

        tDetails.tAppearance.ptOcclusionHighlighting->QueryEnd();
    }
}





static void sDrawImmediateStencilBounds(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails,
    const RenderLayer eRenderLayer ) {



    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    if ( !ptSubObjectGlobals ) {
        return;
    }


    const ShaderDefinition * pdefShader = tHeader.pdefShader;
    // Inference: RenderableStencilBound stores float[3] (DWARF SubObjectGfx.h:88-89) while AABB holds Vector3D and the DWARF shows only Vector3D::operator= inlined at 2940/2941, so the arrays are read as Vector3D.
    AABB aabbBounds;
    aabbBounds.wp = *(const Vector3D *)tDetails.tStencilBound.wp;
    aabbBounds.wvExt = *(const Vector3D *)tDetails.tStencilBound.wvExt;


    Matrix4x4 matTransformAABB( aabbBounds.wvExt.x, 0.0f, 0.0f, 0.0f,
                                0.0f, aabbBounds.wvExt.y, 0.0f, 0.0f,
                                0.0f, 0.0f, aabbBounds.wvExt.z, 0.0f,
                                aabbBounds.wp.x, aabbBounds.wp.y, aabbBounds.wp.z, 1.0f );


    Matrix4x4 matCombine = matTransformAABB * tDetails.tCommon.mWorld;
    GfxWorldMatrixSet( matCombine );

    int32 nPassIndex = 0;
    GfxShaderInit( pdefShader, nPassIndex );
    GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, ptSubObjectGlobals->vbidStencilAABB );

    GfxShaderSetIB( ptSubObjectGlobals->ibidStencilAABB, 0 );
    // Inference: vertex count 12 and index count 36 (12 triangles) from the GfxShaderDraw arguments.
    sDrawTriBatch( 0, 12, 0, 0, 36, TRUE, tDetails.tCommon.ePerfBin );
}








































static void sSetupHardwareSkinned(
    const RenderableHeader & tHeader,
    const RenderableDetails & tDetails,
    BOOL bNeedsBumpBasis,
    int32 & nStartIndex,
    int32 & nIndexCount,
    int32 & nVertexCount ) {


    if ( tDetails.tCommon.eType != RENDERABLETYPE_RACTOR ) {
        return;
    }






    const PackedVBEntry64 & tSkinEntry = tDetails.tAppearance.ptBoneMapping->tPackVBEntry;
    const PackedIBEntry64 & tIBEntry = tDetails.tAppearance.ptBoneMapping->tPackIBEntry;
    int32 nNumIndices = tDetails.tAppearance.ptBoneMapping->nPackIndexCount;
    int32 nNumVertices = tDetails.tAppearance.ptBoneMapping->nPackVertCount;
    int32 nNumBones = tDetails.tAppearance.ptBoneMapping->nNumBones;

    nIndexCount = nNumIndices;
    nVertexCount = nNumVertices;
























    GfxShaderSetStream( STREAMINDEX_DEFAULT, tSkinEntry.dwStreamOffset, tSkinEntry.ptPVB->vbid );






    GfxShaderSetIB( tIBEntry.ptPIB->ibid, tIBEntry.dwIndexOffset );

    GfxShaderSetNumBones( nNumBones );


    const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding( VSC_BONEMATRIXSTARTREGISTER );
    if ( ptBinding ) {

        GfxShaderSetConstant( ptBinding, tDetails.tAppearance.pmMatrixPalette, nNumBones * 3 );
    }
}




























































































void SubObjectGfxUpdate( const float flDeltaTime, const float flClientTime ) {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    SNO snoAppearance = GlobalSNOGet( GLOBALSNO_WATER_SURFACE_APPEARANCE );
    ScopedAppearanceDefinitionLockR pdefAppearance( snoAppearance );
    {
        SubObjectAppearance & tAppearance = AppearanceGetSubObjectAppearance( pdefAppearance, 0, 0, 0 );
        TexAnimUpdate( tAppearance.tCompositeUberMaterial, NULL, ptSubObjectGlobals->tRippleUpdateSubObjectState.arTexAnimStates, ptSubObjectGlobals->tRippleUpdateSubObjectState.nTexAnimStateCount, flDeltaTime, flClientTime );
    }
}































void SubObjectGfxPrepareRender() {


    GetRWindowMgr()->GetMainRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_UPDATE )].flClientTime = CGameGetClientTime();


    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    ptSubObjectGlobals->tRippleDrawSubObjectState = ptSubObjectGlobals->tRippleUpdateSubObjectState;
    if ( ptSubObjectGlobals->tRippleUpdateSubObjectState.nTexAnimStateCount ) {

        ptSubObjectGlobals->tRippleDrawSubObjectState.arTexAnimStates = TexAnimRenderAsyncCopy( ptSubObjectGlobals->tRippleUpdateSubObjectState.nTexAnimStateCount, ptSubObjectGlobals->tRippleUpdateSubObjectState.arTexAnimStates, NULL, SHADERTEX_NONE );
    }

}




void SubObjectStateInitFade( SubObjectState & tSubObjectState ) {
    float flCurrentTime = CGameGetClientTime();
    float flTimeOffset = RandFloat_Internal( CGameGlobalsGet().rngLocal ) * 0.3f + 0.3f;

    tSubObjectState.flLastFadeCheckTime = flCurrentTime > flTimeOffset ? flCurrentTime - flTimeOffset : 0.0f;
    tSubObjectState.fLastFadeCheckResult = FALSE;
}














void SubObjectStateUpdate( const SubObjectAppearance & tSOA, SubObjectState & tSubObjectState, const ObjectTexAnimParam * ptObjTexAnimParam, const float flDeltaTime, const float flClientTime ) {
    if ( flDeltaTime == 0.0f ) {
        return;
    }

    TexAnimUpdate( tSOA.tCompositeUberMaterial, ptObjTexAnimParam, tSubObjectState.arTexAnimStates, tSubObjectState.nTexAnimStateCount, flDeltaTime, flClientTime );
}










































static int32 sOpaqueSort( const void * lpArg0, const void * lpArg1 ) {
    const RenderableHeader * ptSort0 = (const RenderableHeader *)lpArg0;
    const RenderableHeader * ptSort1 = (const RenderableHeader *)lpArg1;

    if ( ptSort0->fInPrepass != ptSort1->fInPrepass ) {
        return ptSort1->fInPrepass - ptSort0->fInPrepass;
    }

    if ( ptSort0->nSortPri != ptSort1->nSortPri ) {
        return ptSort0->nSortPri - ptSort1->nSortPri;
    }

    if ( ptSort0->pdefShader != ptSort1->pdefShader ) {
        return (int32)( (intptr)ptSort0->pdefShader - (intptr)ptSort1->pdefShader );
    }

    return ptSort0->nDist - ptSort1->nDist;
}
















static int32 sEarlyOpaqueSort( const void * lpArg0, const void * lpArg1 ) {
    const RenderableHeader * ptSort0 = (const RenderableHeader *)lpArg0;
    const RenderableHeader * ptSort1 = (const RenderableHeader *)lpArg1;

    if ( ptSort0->fEarlyZDisabled != ptSort1->fEarlyZDisabled ) {
        return ptSort1->fEarlyZDisabled - ptSort0->fEarlyZDisabled;
    }

    if ( ptSort0->pdefShader != ptSort1->pdefShader ) {
        return (int32)( (intptr)ptSort0->pdefShader - (intptr)ptSort1->pdefShader );
    }

    return ptSort0->nDist - ptSort1->nDist;
}















static int32 sBackToFrontSort( const void * lpArg0, const void * lpArg1 ) {
    const RenderableHeader * ptSort0 = (const RenderableHeader *)lpArg0;
    const RenderableHeader * ptSort1 = (const RenderableHeader *)lpArg1;

    if ( ptSort0->fInPrepass != ptSort1->fInPrepass ) {
        return ptSort1->fInPrepass - ptSort0->fInPrepass;
    }

    if ( ptSort0->nSortPri != ptSort1->nSortPri ) {
        return ptSort0->nSortPri - ptSort1->nSortPri;
    }

    return ptSort1->nDist - ptSort0->nDist;
}











static int32 sProjectionSort( const void * lpArg0, const void * lpArg1 ) {
    const RenderableHeader * ptSort0 = (const RenderableHeader *)lpArg0;
    const RenderableHeader * ptSort1 = (const RenderableHeader *)lpArg1;

    if ( ptSort0->nSortPri != ptSort1->nSortPri ) {
        return ptSort0->nSortPri - ptSort1->nSortPri;
    }

    return 0;
}














static const TransSortOrder scnOrderMatrix[3][3] = {
    { SORT0_IN_FRONT, SORT0_IN_FRONT, SORT_OVERLAP },
    { SORT0_IN_FRONT, SORT_OVERLAP, SORT1_IN_FRONT },
    { SORT_OVERLAP, SORT1_IN_FRONT, SORT1_IN_FRONT }
};
static int32 sTransparentSort(
    const void * lpArg0,
    const void * lpArg1)
{
    const RenderableHeader * ptSort0 = (const RenderableHeader *)lpArg0;
    const RenderableHeader * ptSort1 = (const RenderableHeader *)lpArg1;

    if ( ptSort0->nSortPri != ptSort1->nSortPri ) {
        return ptSort0->nSortPri - ptSort1->nSortPri;
    }


    if ( ptSort0->fIsWall || ptSort1->fIsWall ) {



        TransSortOrder nXOrder;
        TransSortOrder nYOrder;

        if ( ptSort0->irectDist.left < ptSort1->irectDist.right ) {



            nXOrder = ptSort1->irectDist.left >= ptSort0->irectDist.right ? SORT1_IN_FRONT : SORT_OVERLAP;
        } else {
            nXOrder = SORT0_IN_FRONT;
        }





        if ( ptSort0->irectDist.top < ptSort1->irectDist.bottom ) {



            nYOrder = ptSort1->irectDist.top >= ptSort0->irectDist.bottom ? SORT1_IN_FRONT : SORT_OVERLAP;
        } else {
            nYOrder = SORT0_IN_FRONT;
        }






        TransSortOrder nResultOrder = scnOrderMatrix[nYOrder][nXOrder];

        if ( nResultOrder == SORT0_IN_FRONT ) {
            return 1;
        } else if ( nResultOrder == SORT1_IN_FRONT ) {
            return -1;
        }







        int32 wdManhattanSort0 = ptSort0->irectDist.left + ptSort0->irectDist.top;
        int32 wdManhattanSort1 = ptSort1->irectDist.left + ptSort1->irectDist.top;

        if ( wdManhattanSort0 > wdManhattanSort1 ) {
            return 1;
        } else if ( wdManhattanSort1 > wdManhattanSort0 ) {
            return -1;
        }








    } else {
        int32 flCmp = ptSort1->nDist - ptSort0->nDist;
        if ( flCmp > 0 ) {
            return 1;
        } else if ( flCmp < 0 ) {
            return -1;
        }
    }







    return ptSort0->nGroup > ptSort1->nGroup ?

        -1 : 1;





}






static void sMerge(
    RenderableHeader * pBase,
    size_t nMiddle,
    size_t nCount,
    int32 ( * pfCompare )( const void *, const void * ) )
{
    SigmaMemoryCopy( sMergeSortTemps, pBase, nMiddle * sizeof( RenderableHeader ) );

    size_t nLeft = 0;
    size_t nRight = nMiddle;
    size_t nDst = 0;

    while ( nLeft < nMiddle && nRight < nCount ) {

        const RenderableHeader & a = sMergeSortTemps[nLeft];
        const RenderableHeader & b = pBase[nRight];

        if ( pfCompare( &a, &b ) > 0 ) {
            pBase[nDst++] = b;
            nRight++;
        } else {


            pBase[nDst++] = a;
            nLeft++;
        }
    }

    if ( nLeft < nMiddle ) {
        SigmaMemoryCopy( pBase + nDst, sMergeSortTemps + nLeft, ( nMiddle - nLeft ) * sizeof( RenderableHeader ) );
    }


}
static void sMergeSort(
    RenderableHeader * pBase,
    size_t nCount,
    int32 ( * pfCompare )( const void *, const void * ) )
{

    if ( nCount > 1 ) {



        size_t nMiddle = nCount / 2;
        sMergeSort( pBase, nMiddle, pfCompare );
        sMergeSort( pBase + nMiddle, nCount - nMiddle, pfCompare );

        sMerge( pBase, nMiddle, nCount, pfCompare );
    }
}



void RenderableHeader::SetDist( float flDist ) {
    nDist = (int32)Math::Pin( flDist * 100.0f, INT_MIN, INT_MAX );
}






void RenderableHeader::SetRect( const Rect2D & rect ) {
    irectDist.left = (int32)Math::Pin( rect.left * 1000.0f, INT_MIN, INT_MAX );
    irectDist.right = (int32)Math::Pin( rect.right * 1000.0f, INT_MIN, INT_MAX );
    irectDist.top = (int32)Math::Pin( rect.top * 1000.0f, INT_MIN, INT_MAX );
    irectDist.bottom = (int32)Math::Pin( rect.bottom * 1000.0f, INT_MIN, INT_MAX );
}






void RenderableHeader::SetRectFromPoint( const Vector3D & wpCenter ) {
    fIsWall = FALSE;
    irectDist.left = (int32)Math::Pin( wpCenter.x * 1000.0f, INT_MIN, INT_MAX );
    irectDist.right = (int32)Math::Pin( wpCenter.x * 1000.0f, INT_MIN, INT_MAX );
    irectDist.top = (int32)Math::Pin( wpCenter.y * 1000.0f, INT_MIN, INT_MAX );
    irectDist.bottom = (int32)Math::Pin( wpCenter.y * 1000.0f, INT_MIN, INT_MAX );
}






SNO SubObjectGfxGetWaterTexture() {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    SubObjectWaterTextureSetUpdate( TRUE );
    return ptSubObjectGlobals->snoTexWater;
}









SNO SubObjectGfxGetInteractiveFogTexture() {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    SubObjectInteractiveFogTextureSetUpdate( TRUE );
    return ptSubObjectGlobals->snoTexInteractiveFog;
}









void SubObjectGfxGetShadowDepthData( SNO & snoTex, Matrix4x4 & mTex ) {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;

    uint32 dwShadowTech = GfxGetShadowTech();
    if ( dwShadowTech == 0 || dwShadowTech == 4 ) {
        snoTex = ptSubObjectGlobals->snoTexShadowMapColor;
    } else {
        snoTex = ptSubObjectGlobals->snoTexShadowMapDepth;
    }

    mTex = ptSubObjectGlobals->mCachedShadowProj;
}









void SubObjectGfxGetShadowColorData( SNO & snoTex ) {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    snoTex = ptSubObjectGlobals->snoTexShadowMapColor;
}






BOOL SubObjectGetLocalPlayerIsGhosted() {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    return ptSubObjectGlobals->fLocalPlayerGhosted;
}






void SubObjectSetLocalPlayerIsGhosted( BOOL fGhosted ) {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    ptSubObjectGlobals->fLocalPlayerGhosted = fGhosted;
}






inline void SubObjectWaterTextureSetUpdate( BOOL bUpdate ) {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    ptSubObjectGlobals->bTexWaterUpdate = bUpdate;
}







inline BOOL SubObjectWaterTextureGetUpdate() {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    return ptSubObjectGlobals->bTexWaterUpdate;
}






inline void SubObjectInteractiveFogTextureSetUpdate( BOOL bUpdate ) {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    ptSubObjectGlobals->bTexInteractiveFogUpdate = bUpdate;
}







inline BOOL SubObjectInteractiveFogTextureGetUpdate() {
    SubObjectGlobals * ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    return ptSubObjectGlobals->bTexInteractiveFogUpdate;
}








BOOL s_bOcclusionHighlightingEnabled = TRUE;
Blizzard::Lock::RMutex s_tOcclusionHighlightingPoolMutex;




BOOL SubObjectGfxOcclusionHighlightingIsEnabled() {
    return s_bOcclusionHighlightingEnabled;
}


void SubObjectGfxOcclusionHighlightingSetEnabled( BOOL bEnabled ) {
    s_bOcclusionHighlightingEnabled = bEnabled;
}





OcclusionHighlighting * SubObjectGfxOcclusionHighlightingAcquire() {

    s_tOcclusionHighlightingPoolMutex.Enter();
    SubObjectGlobals *& ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    OcclusionHighlighting * ptr = ptSubObjectGlobals->tOcclusionHighlightingPool.Alloc_Tracked();
    s_tOcclusionHighlightingPoolMutex.Leave();
    return ptr;
}



void SubObjectGfxOcclusionHighlightingAddRef(
    OcclusionHighlighting * ptManager ) {

    if ( ptManager ) {

        ptManager->AddRenderableRef();
    }
}



void SubObjectGfxOcclusionHighlightingDecRef(
    OcclusionHighlighting * ptManager ) {

    if ( ptManager ) {

        ptManager->DecRenderableRef();
    }
}



void SubObjectGfxOcclusionHighlightingDispose( OcclusionHighlighting * ptManager ) {
    if ( ptManager ) {
        ptManager->MarkForDispose( TRUE );
    }
}








// NON_MATCHING: 564 vs 564 bytes; in the inlined FixedBlockAllocator::GetNextBlock the original keeps the free-list null test inside the block loop, ours hoists it (same as the note on GetNextBlock in XFixedBlockAllocator.h).
void SubObjectGfxOcclusionHighlightingFrameEnd() {

    SubObjectGlobals *& ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    OcclusionHighlightingAllocator & tAllocator = ptSubObjectGlobals->tOcclusionHighlightingPool;

    s_tOcclusionHighlightingPoolMutex.Enter();

    POS pos = tAllocator.GetFirstBlockPOS();

    while ( pos ) {

        OcclusionHighlighting * ptManager = tAllocator.GetNextBlock( pos );
        ptManager->FrameEnd();

        if ( ptManager->MarkedForDispose() && !ptManager->HasRenderableRefs() ) {


            tAllocator.Free( ptManager );
        }
    }

    s_tOcclusionHighlightingPoolMutex.Leave();
}


// NON_MATCHING: 264 vs 264 bytes; same GetNextBlock free-list null test placement as SubObjectGfxOcclusionHighlightingFrameEnd, plus the register choices that follow from it.
void SubObjectGfxOcclusionHighlightingClearQueries() {

    SubObjectGlobals *& ptSubObjectGlobals = CGameGet().ptSubObjectGlobals;
    OcclusionHighlightingAllocator & tAllocator = ptSubObjectGlobals->tOcclusionHighlightingPool;

    s_tOcclusionHighlightingPoolMutex.Enter();

    POS pos = tAllocator.GetFirstBlockPOS();
    while ( pos ) {


        OcclusionHighlighting * ptManager = tAllocator.GetNextBlock( pos );
        ptManager->ClearQueries();
    }

    s_tOcclusionHighlightingPoolMutex.Leave();
}



void SubObjectGfxOcclusionHighlightingSetEstimatedPixelCount( OcclusionHighlighting * ptManager, uint32 nPixelCount ) {
    if ( ptManager ) {
        ptManager->SetEstimatedPixelCount( nPixelCount );
    }
}































BOOL OcclusionHighlighting::Occluded() {

    OcclusionState eOccluded = FindOcclusionState();

    if ( eOccluded == OS_VISIBLE ) {

        m_tLastVisible = Blizzard::Time::Milliseconds();

    } else if ( eOccluded == OS_OCCLUDED ) {


        if ( Blizzard::Time::Milliseconds() - m_tLastVisible > s_fOcclusionHighlightingLatency * 1000.0f ) return TRUE;
    }
    return FALSE;
}







void OcclusionHighlighting::QueryEnd() {

    if ( m_eState == S_OVERFLOWED ) {

        m_eState = S_BUILDING;
        return;
    }

    m_eState = S_BUILDING;


    if ( m_queries.GetArraySize() == 0 ) {

        return;
    }

    OcclusionQuery * ptQuery = m_queries[m_queries.GetArraySize() - 1];

    if ( ptQuery ) {

        gptGFXAPI->OcclusionQueryEnd( ptQuery );
    }
}



void OcclusionHighlighting::QueryBegin() {

    if ( m_queries.GetArraySize() == MAX_SUBOBJECTS_PER_ACTOR ) {

        m_eState = S_OVERFLOWED;
        return;
    }

    m_eState = S_BUILDING;

    OcclusionQuery * ptQuery = gptGFXAPI->OcclusionQueryCreate();

    if ( ptQuery ) {

        gptGFXAPI->OcclusionQueryBegin( ptQuery );
    }

    m_queries.Add_Tracked( ptQuery );
}



void OcclusionHighlighting::FrameEnd() {


    for ( int32 i = 0; i < m_queries.GetArraySize(); ) {

        OcclusionQuery * ptQuery = m_queries[i];
        if ( !ptQuery || m_eState == S_BUILDING || gptGFXAPI->OcclusionQueryCompleted( ptQuery ) ) {

            if ( ptQuery ) {

                gptGFXAPI->OcclusionQueryDispose( ptQuery );
            }
            m_queries.RemoveAtReorder( i );

        } else {

            i++;
        }
    }
    m_eState = S_BUILDING;
}


// Inference: member initialisers on 3969-3974 from the line table of the copy inlined into SubObjectGfxOcclusionHighlightingAcquire (zero stores at 3973 before the Milliseconds call, both enums in one store at 3970, the two counts at 3974).
OcclusionHighlighting::OcclusionHighlighting()
    : m_tLastVisible( Blizzard::Time::Milliseconds() )
    , m_eState( S_BUILDING ), m_eOcclusionState( OS_OCCLUDED )


    , m_nRenderableRefCount( 0 ), m_flMarkedForDispose( FALSE )
    , m_nTotalVisiblePixels( 0 ), m_nEstimatedPixelCount( 0 ) {
}





// NON_MATCHING: 316 vs 316 bytes; only the two zero-initialising movs of bHasValidQuery and eOccluded before the loop are swapped. Tried: bHasValidQuery = TRUE before or after the GetResult block, for ( i = 0; ...; ) instead of while.
OcclusionHighlighting::OcclusionState OcclusionHighlighting::FindOcclusionState() {

    if ( m_eState == S_QUERYING ) {

        return m_eOcclusionState;
    }

    m_eState = S_QUERYING;
    OcclusionState eOccluded = OS_UNKNOWN;
    int32 i = 0;
    BOOL bHasValidQuery = FALSE;

    m_nTotalVisiblePixels = 0;

    while ( i < m_queries.GetArraySize() ) {

        OcclusionQuery * ptQuery = m_queries[i];
        if ( ptQuery ) {


            uint32 nPixels = 0;
            if ( gptGFXAPI->OcclusionQueryGetResult( ptQuery, nPixels ) ) {

                m_nTotalVisiblePixels += nPixels;
                if ( nPixels > 0 ) {

                    eOccluded = OS_VISIBLE;

                } else {

                    if ( eOccluded == OS_UNKNOWN ) {
                        eOccluded = OS_OCCLUDED;
                    }
                }
            }
            bHasValidQuery = TRUE;
        }
        i++;
    }

    uint32 nRatio;
    if ( m_nEstimatedPixelCount > 0 ) {

        nRatio = m_nTotalVisiblePixels * 100 / m_nEstimatedPixelCount;

        if ( eOccluded != OS_UNKNOWN ) {

            if ( nRatio < s_nOcclusionHideVisiblePixelPercent ) {

                eOccluded = OS_OCCLUDED;

            }

            else if ( m_eOcclusionState == OS_OCCLUDED &&
                      nRatio < s_nOcclusionShowVisiblePixelPercent ) {

                eOccluded = OS_OCCLUDED;
            }
        }
    }



    if ( eOccluded != OS_UNKNOWN ) {

        m_eOcclusionState = eOccluded;
    }

    if ( !bHasValidQuery ) {

        m_eOcclusionState = OS_VISIBLE;
    }
    return m_eOcclusionState;

}




void OcclusionHighlighting::AddRenderableRef() {

    Blizzard::Lock::Atomic::Increment( &m_nRenderableRefCount );
}











BOOL OcclusionHighlighting::DecRenderableRef() {

    int32 nNewValue = Blizzard::Lock::Atomic::Decrement( &m_nRenderableRefCount );
    // Inference: the result is unused by the only caller; the comparison is assumed.
    return nNewValue == 0;
}




OcclusionHighlighting::~OcclusionHighlighting() {



    for ( int i = 0; i < m_queries.GetArraySize(); i++ ) {

        OcclusionQuery * ptQuery = m_queries[i];
        if ( ptQuery ) {

            gptGFXAPI->OcclusionQueryDispose( ptQuery );
        }
    }


}



BOOL OcclusionHighlighting::MarkedForDispose() const {

    return m_flMarkedForDispose;
}












BOOL OcclusionHighlighting::HasRenderableRefs() const {

    return m_nRenderableRefCount != 0;
}



void OcclusionHighlighting::ClearQueries() {

    m_queries.EmptyArray();
}



inline void OcclusionHighlighting::SetEstimatedPixelCount( uint32 nPixelCount ) {
    uint32 dwScreenWidth;
    uint32 dwScreenHeight;
    GfxGetCurrentDimensions( dwScreenWidth, dwScreenHeight );
    float flResolutionMultiplier = (float)( dwScreenHeight * dwScreenWidth ) / ( 1280.0f * 720.0f );
    m_nEstimatedPixelCount = (uint32)( flResolutionMultiplier * nPixelCount );
}
