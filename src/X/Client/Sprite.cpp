// X/Client/Sprite.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r6-render/sprite/frag.cpp by build/try/r6-render/gen.py.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Sprite.h"
#include "CGame.h"
#include "SubObjectGfx.h"
#include "RWindow.h"
#include "../Common/ThreadGlobals.h"
#include "../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Sigma/DataStructures/XCommandBuffer.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxIB.h"
#include "Gfx/GfxVB.h"
#include "../Common/ActorCommonData.h"
#include "../Common/CommonTypes.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/DataStructures/XPooledAllocator.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Prefetch.h"
#include "Definitions/TextureDefinition.h"
#include "Definitions/ShaderDefinition.h"
#include "../Common/GlobalSNO.h"

class DisplayList;

typedef uint16 GfxNdx;  // X/Client/Gfx/GfxAPI.h:56
// X/Client/Gfx/GfxVB.cpp and GfxIB.cpp; not declared in src/ headers yet.
void * GfxVBLock(const VBID vbid, const uint32 dwCount, uint32 & dwStartIndex);
void GfxVBRestart(VBID vbid);
BOOL GfxIBAllocTracked(const ResourceUsage eUsage, const uint32 dwCount, const BOOL fPurgeIfNecessary, const GfxNdx * lpInitialData, IBID & ibid, LPCSTR szFile, int32 nLine, LPCSTR szDebugString);

void SpritesGfxInit();
void SpritesGfxDispose();

enum {
    INDICES_PER_SPRITE = 6,
    VERTICES_PER_SPRITE = 4,
    MAX_SPRITES_PER_BATCH = 768,
    MAX_RACTOR_SPRITES = 96,
    NUM_SPRITE_BUFFERS = 4,
    MAX_SPRITE_VERTICES = 24576,
    MAX_SPRITE_RENDER_DRAW_VERTICES = 2048,
};

struct Sprite {  // size 0x18
    SNO snoShader;  // 0x00
    SNO snoTexDiffuse;  // 0x04
    SNO snoTexAlpha;  // 0x08
    SNO snoTexAlpha2;  // 0x0c
    float flZ;  // 0x10
    uint32 dwVertexIndex;  // 0x14
};

typedef XListMemoryPool<void *> SpritePointerPool;
typedef XPooledList<void *, void *> SpritePointerList;
typedef XMemoryPool<XPooledList<void *, void *> > SpriteListPool;
typedef XChainedFixedBlockAllocator<Sprite> SpriteAllocator;

enum SpriteType {
    SPRITE_NORMAL = 0,
    SPRITE_RACTOR = 1,
    SPRITE_UI = 2,
};

struct SpriteBucket {  // size 0x28
    uint32 dwKey;  // 0x00
    float flZ;  // 0x04
    SpriteType eType;  // 0x08
    RenderContext eContext;  // 0x0c
    SpritePointerList * ptSpritePtrs;  // 0x10
    uint32 dwStartIndex;  // 0x18
    uint32 dwMinVertexIndex;  // 0x1c
    uint32 dwMaxVertexIndex;  // 0x20
};

class TSpriteBucketArray : public blz::vector<SpriteBucket, blz::allocator<SpriteBucket> > {  // size 0x18
};

struct RenderLayerBucket {  // size 0x20
    int32 nCount;  // 0x00
    int32 nLastBucket;  // 0x04
    TSpriteBucketArray tSpriteBuckets;  // 0x08
};

struct RActorSprite {  // size 0xa8
    Rect2D wrBounds;  // 0x00
    Rect2D wrViewBounds;  // 0x10
    float flZ;  // 0x20
    DisplayList * ptDisplayList;  // 0x28
    SNO snoAppearance;  // 0x30
    WorldLocation tActorLoc;  // 0x38
    SNO snoObserver;  // 0x90
    SNO snoRenderTarget;  // 0x94
    float flAspectRatio;  // 0x98
    BOOL fIsFullscreenUIElement;  // 0x9c
    BOOL fClearZBuffer;  // 0xa0
};
static_assert(sizeof(RActorSprite) == 0xa8, "RActorSprite");

struct SpriteRenderData {  // size 0x408
    RenderLayerBucket tRenderLayerBuckets[27];  // 0x00
    SpriteListPool tSpriteListPool;  // 0x360
    SpritePointerPool tSpritePointerPool;  // 0x390
    int32 nSpriteCount;  // 0x3c0
    SpriteAllocator tSpriteAllocator;  // 0x3c8
    int32 nRActorSpriteCount;  // 0x3f8
    RActorSprite * ptRActorSprites;  // 0x400
};
static_assert(sizeof(SpriteRenderData) == 0x408, "SpriteRenderData");

struct SpriteDrawUISpriteData {  // size 0x10
    UISprite * pSprite;  // 0x00
    float flZ;  // 0x08
    RenderLayer eRenderLayer;  // 0x0c
};

static void sBufferSpriteDrawUISprite(XCommandBuffer * pBuffer, UISprite * ptSprite, const float flZ, const RenderLayer eRenderLayer);

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
struct SpriteDrawDeferredMinimapTileData {
    SNO snoMapTex;  // 0x00
    SNO snoShader;  // 0x04
    Vector2D wpCorners[4];  // 0x08
    RGBAColor rgbaColor;  // 0x28
    RGBAColor rgbaAuxColor;  // 0x2c
    float flZ;  // 0x30
    SNO snoTexMask;  // 0x34
    Vector2D wpMaskCorners[4];  // 0x38
    SNO snoTexExploredMask;  // 0x58
    Vector2D wpExploredMaskCorners[4];  // 0x5c
};
static void sBufferSpriteDrawDeferredMinimapTile(XCommandBuffer * pBuffer, const SNO snoMapTex, const SNO snoShader, const Vector2D * wpCorners, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const SNO snoTexMask, const Vector2D * wpMaskCorners, const SNO snoTexExploredMask, const Vector2D * wpExploredMaskCorners);
struct SpriteDrawSimpleData {
    SNO snoTexture;  // 0x00
    SNO snoShader;  // 0x04
    Rect2D wrScreen;  // 0x08
    Vector2D wpUVs[4];  // 0x18
    RGBAColor rgbaColor;  // 0x38
    RGBAColor rgbaAuxColor;  // 0x3c
    float flZ;  // 0x40
    float flStereoDepth;  // 0x44
    RenderLayer eRenderLayerOverride;  // 0x48
};
static void sBufferSpriteDrawSimple(XCommandBuffer * pBuffer, const SNO snoTexture, const SNO snoShader, const Rect2D & wrScreen, const Vector2D * wpUVs, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const RenderLayer eRenderLayerOverride);
void SpriteDrawSimple(const SNO snoTexture, const SNO snoShader, const Rect2D & wrScreen, const Vector2D * wpUVs, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const RenderLayer eRenderLayerOverride);
struct SpriteDrawDeferredData0 {  // size 0x40
    ImageTextureFrame tImageTexFrame;  // 0x00
    SNO snoShader;  // 0x08
    Vector2D pCoords;  // 0x0c
    float flScaleX;  // 0x14
    float flScaleY;  // 0x18
    RGBAColor rgbaColor;  // 0x1c
    RGBAColor rgbaAuxColor;  // 0x20
    float flZ;  // 0x24
    Rect2D wrClipping;  // 0x28
    RenderLayer eRenderLayerOverride;  // 0x38
    uint8 fInRenderDraw : 1;  // 0x3c
    uint8 bHasClipping : 1;
};
static void sBufferSpriteDrawDeferred(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
void SpriteDrawDeferred(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
void SpriteDrawDeferredMinimapTile(const SNO snoMapTex, const SNO snoShader, const Vector2D * wpCorners, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const SNO snoTexMask, const Vector2D * wpMaskCorners, const SNO snoTexExploredMask, const Vector2D * wpExploredMaskCorners);
void SpriteDrawDeferred(const TextureDefinition * pdefTexture, const float flWidth, const float flHeight, const uint32 nFrame, const ShaderDefinition * pdefShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
static void sBufferSpriteDrawDeferred(XCommandBuffer * pBuffer, const TextureDefinition * pdefTexture, const float flTexWidth, const float flTexHeight, const uint32 dwFrameIndex, const ShaderDefinition * pdefShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
void TextureGetFrameRectUVs(Rect2D & rectTextureUVs, const TextureDefinition * pdefTexture, const uint32 dwFrameIndex);  // Definitions/TextureDefinition.cpp
struct SpriteGlobals {  // size 0x848
    SpriteRenderData tRenderData[2];  // 0x00
    VBID vbidSprite[NUM_SPRITE_BUFFERS];  // 0x810
    IBID ibidSprite;  // 0x820
    IBID ibidQuadSprite;  // 0x824
    uint32 dwRenderUpdateVertIndex;  // 0x828
    uint32 dwRenderDrawVertIndex;  // 0x82c
    VertexSprite * ptRenderUpdateVBuffer;  // 0x830
    BOOL bRenderUpdateVBLocked;  // 0x838
    uint32 dwRenderDrawIndex;  // 0x83c
    uint32 dwSpriteMaxVertices;  // 0x840
    uint32 dwRenderUpdateIndex;  // 0x844
};
static_assert(sizeof(SpriteGlobals) == 0x848, "SpriteGlobals");

inline UISpritePoolInterface * UISpritePoolInterface::GetNext() const { return m_ptNext; }  // Sprite.h
#include "RActors/RActors.h"
#include "Camera/CameraManager.h"
// DWARF RActorGfx.cpp:2777.
void RActorGfxRenderRActorInDisplayList(const Camera &, DisplayList &, DisplayList *, DisplayList *, RActorID, const BOOL, const float, const BOOL);
// DWARF Sprite.cpp:1607 and 3845; declarations precede callers.
static SpriteBucket * sFindComplexSpriteBucket(const RenderLayer, const SpriteType, const float);
static void sBufferSpriteDrawFullscreenRActorSprite(XCommandBuffer *, int32, DisplayList *, const RActorID, float, const float, const SNO, const RenderLayer);
#include "Gfx/Gfx.h"
#include "Gfx/GfxShader.h"
#include "Camera/CameraLocation.h"
#include "UserInterface/UIUtils.h"
#include "Definitions/AppearanceDefinition.h"
#include "../Common/Definitions/ObserverDefinition.h"
// Declarations src headers lack (DWARF function DIEs of the named files); all are called by SpritesDraw's closure.
void GfxWorldMatrixSet(const Matrix4x4 & m);  // Gfx.cpp:2053
void GfxBuild2DUIProjectionMatrix(Matrix4x4 & mProj);  // Gfx.cpp
void GfxViewportSet(const IRect2D & rectViewport);  // Gfx.cpp
void GfxRenderClear(BOOL fClearTarget, BOOL fClearZBuffer, BOOL fClearStencilBuffer, const RGBAColor & rgbaTargetColor);  // Gfx.cpp
void GfxGetRenderTargetDimensions(uint32 & dwWidth, uint32 & dwHeight);  // Gfx.cpp
void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight);  // Gfx.cpp
void GfxSetRenderAndDepthTarget(const SNO snoRenderTarget, const SNO snoDepthTarget, const CubeMapFace eCubeMapFace);  // Gfx.cpp
BOOL GfxIsLetterboxed();  // Gfx.cpp
float GfxGetAspectRatio();  // Gfx.cpp
GfxNdx * GfxIBLock(const IBID ibid, const uint32 dwCount, uint32 & dwStartIndex);  // GfxIB.cpp:561
void GfxIBUnlock(const IBID ibid);  // GfxIB.cpp:588
void GfxShaderInit(const ShaderDefinition * pdefShader, const int32 nPassIndex);  // GfxShader.cpp:409
void GfxShaderSetIB(const IBID ibid, const uint32 dwIndexOffset);  // GfxShader.cpp:1553
void GfxShaderSetStream(const StreamIndex eStream, const uint32 dwOffset, const VBID vbid);  // GfxShader.cpp:1491
TextureBinding * GfxGetTextureBinding(const ShaderTex eShaderTex);  // GfxShader.cpp:868
void GfxShaderSetTexture(const TextureBinding * ptBinding, const SNO snoTexture);  // GfxShader.cpp:917
void GfxShaderDraw(const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimitiveCount, const PerfBin ePerfBin);  // GfxShader.cpp:800
void ConvertUICToPixels(const Rect2D & rectUIC, IRect2D & rectPixels);  // UIUtils.cpp:174
void ConvertPixelsToNDC(const Rect2D & rectPixels, Rect2D & rectNDC);  // UIUtils.cpp:93
// Redeclared with a default last argument: the fullscreen call at Sprite.cpp:1323 passes eight
// arguments and the DWARF shows an inlined Vector2D constructor at that call line.
void CameraStateSetOrthographic(CameraState & cmState, const PRTransform & transform, const float flHeight, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const Rect2D & rectViewport, const Vector2D & vecScale, const Vector2D & vecProjectionTranslate = Vector2D(0.0f, 0.0f));
// DWARF Sprite.cpp:879, 1420, 1484; defined after their caller SpritesDraw.
static void sFillSpriteIndices(TSpriteBucketArray & tSpriteBucket, RenderContext eContext, int32 nStartBucket, int32 nEndBucket, GfxNdx * pwIndices, uint32 dwStartIndex);
static void sDrawRActorSprite(RActorSprite & tSprite);
static void sDrawUISprite(UISprite & tSprite);
#line 80
const float scflFullscreenUIAspectRatio = 1.3333333333333333f;
const float scflFullscreenUIHeight = 12.0f;
#line 219
// DWARF Sprite.cpp:3797, forward declaration for the actual thread-buffer caller.
static void sBufferSpriteDrawRActorSprite(XCommandBuffer *, int32, DisplayList *, const RActorID, const Rect2D &, const Rect2D &, float, const float, const SNO, const SNO, const float, const BOOL);






































































































































































UISpritePoolInterface * s_ptUISpritePoolsHead;





void SpritesInitialize()

{
    SpriteGlobals *& ptSpriteGlobals = CGameGet().ptSpriteGlobals;
    ptSpriteGlobals = (SpriteGlobals *)SigmaMemoryNew(sizeof(SpriteGlobals), 4, NULL, TRUE);
    // Inference: original loop visits both asynchronous render data slots.
    for (int32 nAsyncData = 0; nAsyncData < 2; nAsyncData++)
    {
        SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[nAsyncData];

        tRenderData.tSpritePointerPool.Initialize(); // Inference: count 768 replaces Initialize's DWARF default 10.
        tRenderData.tSpritePointerPool.SetBlockCount_Tracked(MAX_SPRITES_PER_BATCH);

        tRenderData.tSpriteListPool.Initialize(); // Inference: count 768 replaces Initialize's DWARF default 10.
        tRenderData.tSpriteListPool.SetBlockCount_Tracked(MAX_SPRITES_PER_BATCH);

        tRenderData.tSpriteAllocator.Initialize(); // Inference: count 768 replaces Initialize's DWARF default 10.
        tRenderData.tSpriteAllocator.SetBlockCount_Tracked(MAX_SPRITES_PER_BATCH);
        tRenderData.tSpriteAllocator.SetGrow(XALLOC_GROW_LINEAR, MAX_SPRITES_PER_BATCH);


        for (int32 i = 0; i < NUM_RENDERLAYERS; i++)
        {
            RenderLayerBucket & tRenderLayerBucket = tRenderData.tRenderLayerBuckets[i];
            new (&tRenderLayerBucket.tSpriteBuckets) TSpriteBucketArray();
            tRenderLayerBucket.nLastBucket = -1;
        }

        tRenderData.ptRActorSprites = (RActorSprite *)SigmaMemoryNew(sizeof(RActorSprite) * MAX_RACTOR_SPRITES, 4, NULL, TRUE);

        tRenderData.nSpriteCount = 0;

        tRenderData.nRActorSpriteCount = 0;
    }

    UISpritePoolInterface * ptSpritePool;
    for (ptSpritePool = s_ptUISpritePoolsHead; ptSpritePool; ptSpritePool = ptSpritePool->GetNext())
    {
        ptSpritePool->Initialize();
    }



}




void SpritesDispose()
{
    UISpritePoolInterface * ptSpritePool;
    SpriteGlobals *& ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    for (int32 nAsyncData = 0; nAsyncData < 2; nAsyncData++)
    {
        SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[nAsyncData];
        for (int32 i = 0; i < NUM_RENDERLAYERS; i++)
        {
            RenderLayerBucket & tRenderLayerBucket = tRenderData.tRenderLayerBuckets[i];

            tRenderLayerBucket.tSpriteBuckets.~TSpriteBucketArray();
            tRenderLayerBucket.nLastBucket = -1;
        }

        tRenderData.tSpriteAllocator.Dispose();
        tRenderData.tSpriteListPool.Dispose();
        tRenderData.tSpritePointerPool.Dispose();
        SigmaMemoryFree(tRenderData.ptRActorSprites, NULL);
        tRenderData.ptRActorSprites = NULL;
    }

    for (ptSpritePool = s_ptUISpritePoolsHead; ptSpritePool; ptSpritePool = ptSpritePool->GetNext())
        ptSpritePool->Dispose();

    SigmaMemoryFree(ptSpriteGlobals, NULL);
    ptSpriteGlobals = NULL;
}










void sSpritesGfxLockRenderVB()
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    VBID vbid = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderUpdateIndex];
    if (!GfxVBIsValid(vbid))
    {
        SpritesGfxDispose();
        SpritesGfxInit();
    }
    vbid = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderUpdateIndex];

    uint32 dwStartIndex;
    ptSpriteGlobals->ptRenderUpdateVBuffer = (VertexSprite *)GfxVBLock(vbid, ptSpriteGlobals->dwSpriteMaxVertices - MAX_SPRITE_RENDER_DRAW_VERTICES, dwStartIndex);
    ptSpriteGlobals->bRenderUpdateVBLocked = TRUE;
}




BOOL sSpritesGfxUnlockRenderVB()
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    if (ptSpriteGlobals->bRenderUpdateVBLocked)
    {
        VBID vbid = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderUpdateIndex];
        GfxVBUnlock(vbid);
        ptSpriteGlobals->ptRenderUpdateVBuffer = NULL;
        ptSpriteGlobals->bRenderUpdateVBLocked = FALSE;
        return TRUE;
    }
    return FALSE;
}








void sSpritesGfxFlush()
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;
    VBID vbid = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderDrawIndex];
    GfxVBRestart(vbid);
}













inline VertexSprite * sSpritesGfxVBBeginAlloc(BOOL fInRenderDraw, uint32 dwVertexCount, uint32 & dwStartIndex)
{



    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;






















    if (!fInRenderDraw)
    {
        if (ptSpriteGlobals->dwRenderUpdateVertIndex + dwVertexCount >= ptSpriteGlobals->dwSpriteMaxVertices - MAX_SPRITE_RENDER_DRAW_VERTICES)
            { dwStartIndex = 0; return NULL; }
        dwStartIndex = ptSpriteGlobals->dwRenderUpdateVertIndex;
        if (ptSpriteGlobals->ptRenderUpdateVBuffer == NULL)
            { dwStartIndex = 0; return NULL; }




        VertexSprite * ptVertex = ptSpriteGlobals->ptRenderUpdateVBuffer + dwStartIndex;


        ptSpriteGlobals->dwRenderUpdateVertIndex += dwVertexCount;
        return ptVertex;
    }
    else
    {














        if (ptSpriteGlobals->dwRenderDrawVertIndex + dwVertexCount >= ptSpriteGlobals->dwSpriteMaxVertices)
            { dwStartIndex = 0; return NULL; }
        dwStartIndex = ptSpriteGlobals->dwRenderDrawVertIndex;

        VBID vbid = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderDrawIndex];
        VertexSprite * ptVertex = (VertexSprite *)GfxVBLock(vbid, dwVertexCount, dwStartIndex);


        ptSpriteGlobals->dwRenderDrawVertIndex += dwVertexCount;
        return ptVertex;
    }
}












inline void sSpritesGfxVBEndAlloc(BOOL fInRenderDraw)
{

    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;







    if (fInRenderDraw)
    {
        VBID vbid = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderDrawIndex];
        GfxVBUnlock(vbid);
    }
}







void SpritesGfxInit()
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;
    int32 nIndexCount;
    IBID ibid;
    BOOL fSuccess;

    for (int i = 0; i < NUM_SPRITE_BUFFERS; i++)
    {
        VBID vbid;




        BOOL fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_STATIC, VBFORMAT_SPRITE, MAX_SPRITE_VERTICES, TRUE, NULL, vbid, "", 0, "");




        ptSpriteGlobals->vbidSprite[i] = vbid;
    }
    ptSpriteGlobals->dwSpriteMaxVertices = MAX_SPRITE_VERTICES;
    ptSpriteGlobals->dwRenderUpdateIndex = 0;





    ptSpriteGlobals->dwRenderUpdateVertIndex = 0;

    ptSpriteGlobals->bRenderUpdateVBLocked = FALSE;
    ptSpriteGlobals->dwRenderDrawIndex = 0;


    sSpritesGfxLockRenderVB();



    GfxQuadlistIBAlloc(RESOURCEUSAGE_IMMUTABLE, MAX_SPRITES_PER_BATCH, FALSE, nIndexCount, ptSpriteGlobals->ibidQuadSprite);


    fSuccess = GfxIBAllocTracked(RESOURCEUSAGE_DYNAMIC, ptSpriteGlobals->dwSpriteMaxVertices * INDICES_PER_SPRITE, TRUE, NULL, ibid, "", 0, "");

    ptSpriteGlobals->ibidSprite = ibid;
}






void SpritesGfxDispose()
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    sSpritesGfxUnlockRenderVB();

    for (int i = 0; i < NUM_SPRITE_BUFFERS; i++)
    {
        GfxVBRelease(ptSpriteGlobals->vbidSprite[i]);
        ptSpriteGlobals->vbidSprite[i] = -1;
    }
    GfxIBRelease(ptSpriteGlobals->ibidSprite);
    GfxIBRelease(ptSpriteGlobals->ibidQuadSprite);
}










void SpritesGfxAsyncSwapRender(AsyncRenderDeviceStatus eStatus)
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    BOOL bRenderUpdateVBWasUnlocked = sSpritesGfxUnlockRenderVB();

    ptSpriteGlobals->dwRenderDrawIndex = ptSpriteGlobals->dwRenderUpdateIndex;
    ptSpriteGlobals->dwRenderDrawVertIndex = bRenderUpdateVBWasUnlocked ? ptSpriteGlobals->dwSpriteMaxVertices - MAX_SPRITE_RENDER_DRAW_VERTICES : 0;
    ptSpriteGlobals->dwRenderUpdateIndex = (ptSpriteGlobals->dwRenderUpdateIndex + 1) % NUM_SPRITE_BUFFERS;
    ptSpriteGlobals->dwRenderUpdateVertIndex = 0;

    BOOL bDeviceAvailable = eStatus == ASYNC_RENDER_DEVICE_AVAILABLE;
    if (bDeviceAvailable)
        sSpritesGfxLockRenderVB();
}













static void sDrawBatch(
    const ShaderDefinition * pdefShader,
    const SNO snoTexDiffuse,
    const SNO snoTexAlpha,
    const SNO snoTexAlpha2,
    const IBID & ibid,
    const VBID & vbid,
    const uint32 dwBaseIndexOffset,
    const uint32 dwMinVertexIndex,
    const uint32 dwNumVertices,
    const uint32 dwIBStartIndex,
    const int32 nRunLength,
    const BOOL fPrevWasNormal,
    const BOOL fSameShader)
{
    if (!fPrevWasNormal)
    {
        GfxWorldMatrixSet(cgm4x4Identity);
        GfxViewMatrixSet(cgm4x4Identity);

        Matrix4x4 mProj;
        GfxBuild2DUIProjectionMatrix(mProj);
        GfxProjectionMatrixSet(mProj);


        IRect2D rectMainViewport;
        rectMainViewport.left = 0;
        rectMainViewport.top = 0;
        GfxGetRenderTargetDimensions((uint32 &)rectMainViewport.right, (uint32 &)rectMainViewport.bottom);
        GfxViewportSet(rectMainViewport);
    }



    if ((snoTexDiffuse != SNO_NONE) && !Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoTexDiffuse), 2))
    {
        return;
    }

    BOOL fTex0DimensionsInitialized = FALSE;
    ShaderFloat tTex0Dimensions;

    // Inference: DWARF local with no location; the original duplicates the pass loop on it.
    BOOL fPrepareShader = !fSameShader || pdefShader->nPassCount != 1;
    for (int32 nPassIndex = 0; nPassIndex < pdefShader->nPassCount; nPassIndex++)
    {
        if (fPrepareShader)
        {
            GfxShaderInit(pdefShader, nPassIndex);


            GfxShaderSetIB(ibid, 0);
            GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, vbid);
        }
        // Inference: constant index 0 (literal in the original call, mov w0, wzr).
        const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding(0);
        if (ptBinding)
        {
            if ((snoTexDiffuse != SNO_NONE) && !fTex0DimensionsInitialized)
            {
                ScopedTextureDefinitionLockR pdefTexture(snoTexDiffuse);
                ShaderFloatSet(tTex0Dimensions, 1.0f / pdefTexture->dwWidth, 1.0f / pdefTexture->dwHeight, 0.0f, 0.0f);
                fTex0DimensionsInitialized = TRUE;
            }
            GfxShaderSetConstant(ptBinding, &tTex0Dimensions, 1);
        }


        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoTexDiffuse);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA_LAYER_1), snoTexAlpha);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA_LAYER_2), snoTexAlpha2);


        uint32 dwTriCount = nRunLength * 2;
        GfxShaderDraw(DRAWMODE_TRIANGLELIST, dwBaseIndexOffset, dwMinVertexIndex, dwNumVertices, dwIBStartIndex, dwTriCount, PERFBIN_UI);
    }






}




static int32 sSpriteBucketSort(
    const void * lpArg0,
    const void * lpArg1)
{
    const SpriteBucket * ptSort0 = (const SpriteBucket *)lpArg0;
    const SpriteBucket * ptSort1 = (const SpriteBucket *)lpArg1;

    float flCmp = ptSort1->flZ - ptSort0->flZ;
    if (flCmp > 1e-6f)
    {
        return 1;
    }
    else if (flCmp < -1e-6f)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


















static void sFillSpriteIndices(
    TSpriteBucketArray & tSpriteBucket,
    RenderContext eContext,
    int32 nStartBucket,
    int32 nEndBucket,
    GfxNdx * pwIndices,
    uint32 dwStartIndex)
{
    uint32 * pdwIndices = (uint32 *)pwIndices;
    uint32 dwIndex = 0;
    for (int32 nBucket = nStartBucket; nBucket < nEndBucket; nBucket++)
    {
        if (tSpriteBucket[nBucket].eType == SPRITE_NORMAL &&
            tSpriteBucket[nBucket].eContext == eContext)
        {



            SpritePointerList & tSpritePtrs = *tSpriteBucket[nBucket].ptSpritePtrs;
            int32 nCount = tSpritePtrs.Count();  // Inference: DWARF nCount has no location and no inlined Count here (unused); initializer assumed.




            tSpriteBucket[nBucket].dwStartIndex = dwStartIndex + dwIndex * 2;
            uint32 dwMinVertexIndex = 0xffff;
            uint32 dwMaxVertexIndex = 0;

            POS pos = tSpritePtrs.GetHeadPosition();





            while (pos)
            {
                Sprite * ptSprite = (Sprite *)tSpritePtrs.GetNext(pos);
                GfxNdx dwVertexIndex = (GfxNdx)ptSprite->dwVertexIndex;
















                pdwIndices[dwIndex + 0] = (dwVertexIndex + 0) | ((dwVertexIndex + 1) << 16);
                pdwIndices[dwIndex + 1] = (dwVertexIndex + 2) | ((dwVertexIndex + 2) << 16);
                pdwIndices[dwIndex + 2] = (dwVertexIndex + 1) | ((dwVertexIndex + 3) << 16);
                dwIndex += 3;

                dwMinVertexIndex = Math::Min<uint32>(dwMinVertexIndex, dwVertexIndex);
                dwMaxVertexIndex = Math::Max<uint32>(dwMaxVertexIndex, dwVertexIndex + 3);
            }









            tSpriteBucket[nBucket].dwMinVertexIndex = dwMinVertexIndex;
            tSpriteBucket[nBucket].dwMaxVertexIndex = dwMaxVertexIndex;
        }
    }
}



void SpritesDraw(
    const RenderLayer eRenderLayer)
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];


    if (tRenderData.nSpriteCount == 0)
    {
        return;
    }

    RenderLayerBucket & tRenderLayerBucket = tRenderData.tRenderLayerBuckets[eRenderLayer];
    TSpriteBucketArray & tSpriteBucket = tRenderLayerBucket.tSpriteBuckets;
    uint32 nBucketCount = tSpriteBucket.size();
    if (nBucketCount == 0)
    {
        return;
    }





    IBID ibidSprite = ptSpriteGlobals->ibidSprite;



    VBID vbidSprite = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderDrawIndex];

    if (!GfxVBIsValid(vbidSprite))
    {
        SpritesGfxDispose();
        SpritesGfxInit();
    }
    // Inference: operand order of the three layers (the original tests one bit mask 0xa0400).
    if (eRenderLayer == RENDERLAYER_UI || eRenderLayer == RENDERLAYER_HIGHLIGHT || eRenderLayer == RENDERLAYER_UI_OVERLAY)
    {
        SpriteBucket * ptSpriteBuckets = tSpriteBucket.data();
        qsort(ptSpriteBuckets, nBucketCount, sizeof(SpriteBucket), sSpriteBucketSort);
    }

    RWindowMgr * ptRWindowMgr = GetRWindowMgr();
    RenderContext eContext = ptRWindowMgr ? ptRWindowMgr->GetCurrentRWindow().tAsyncData[0].dispList.GetRenderContext() : RENDERCONTEXT_DEFAULT;

    BOOL fPrevWasNormal = FALSE;
    BOOL fSameShader = FALSE;
    const ShaderDefinition * pdefLastSpriteShader = NULL;


    uint32 dwMaxIndexCount = MAX_SPRITES_PER_BATCH * INDICES_PER_SPRITE;
    uint32 nStartBucket = 0;
    while (nStartBucket < nBucketCount)
    {


        uint32 nEndBucket = nBucketCount;
        uint32 dwIndicesTotal = 0;
        for (uint32 nBucket = nStartBucket; nBucket < nBucketCount; nBucket++)
        {
            SpriteBucket & tBucket = tSpriteBucket[nBucket];
            if (tBucket.eType == SPRITE_NORMAL &&
                tBucket.eContext == eContext)
            {




                uint32 dwIndicesInBucket = tBucket.ptSpritePtrs->Count() * INDICES_PER_SPRITE;

                if (dwIndicesTotal + dwIndicesInBucket > dwMaxIndexCount)
                {
                    nEndBucket = nBucket;
                    break;
                }

                dwIndicesTotal += dwIndicesInBucket;
            }
        }
        if (dwIndicesTotal > 0)
        {




            uint32 dwStartIndex;
            GfxNdx * pwIndices = GfxIBLock(ibidSprite, dwIndicesTotal, dwStartIndex);
            if (pwIndices)
            {
                sFillSpriteIndices(tSpriteBucket, eContext, nStartBucket, nEndBucket, pwIndices, dwStartIndex);
                GfxIBUnlock(ibidSprite);
            }
        }


        for (uint32 nBucket = nStartBucket; nBucket < nEndBucket; nBucket++)
        {
            SpritePointerList & tSpritePtrs = *tSpriteBucket[nBucket].ptSpritePtrs;

            if (tSpriteBucket[nBucket].eContext == eContext)
            {
                switch (tSpriteBucket[nBucket].eType)
                {
                case SPRITE_RACTOR:
                    CameraManagerRestoreCurrentCameraToMainCamera();
                    sDrawRActorSprite(*((RActorSprite *)tSpritePtrs.GetHead()));
                    fPrevWasNormal = FALSE;
                    fSameShader = FALSE;
                    break;

                case SPRITE_UI:
                    CameraManagerRestoreCurrentCameraToMainCamera();
                    sDrawUISprite(*((UISprite *)tSpritePtrs.GetHead()));
                    fPrevWasNormal = FALSE;
                    fSameShader = FALSE;
                    break;

                case SPRITE_NORMAL:
                    {
                        if (dwIndicesTotal > 0)
                        {
                            Sprite * ptSprite = (Sprite *)tSpritePtrs.GetHead();
                            int32 nSpriteCount = tSpritePtrs.Count();
                            uint32 dwStartIndex = tSpriteBucket[nBucket].dwStartIndex;
                            uint32 dwMinVertexIndex = tSpriteBucket[nBucket].dwMinVertexIndex;
                            uint32 dwMaxVertexIndex = tSpriteBucket[nBucket].dwMaxVertexIndex;

                            if (pdefLastSpriteShader == NULL || ptSprite->snoShader != pdefLastSpriteShader->tHeader.snoHandle)
                            {




                                pdefLastSpriteShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(ptSprite->snoShader);
                                fSameShader = FALSE;
                            }


                            if (pdefLastSpriteShader->nPassCount == 1)
                            {
                                uint32 dwKey = tSpriteBucket[nBucket].dwKey;
                                while ((nBucket + 1 < nEndBucket) && (tSpriteBucket[nBucket + 1].eType == SPRITE_NORMAL) && (tSpriteBucket[nBucket + 1].eContext == eContext) && (tSpriteBucket[nBucket + 1].dwKey == dwKey))
                                {
                                    SpritePointerList & tScopedSpritePtrs = *tSpriteBucket[nBucket + 1].ptSpritePtrs;


                                    Sprite * ptSameKeySprite = (Sprite *)tScopedSpritePtrs.GetHead();
                                    if (ptSameKeySprite->snoShader != ptSprite->snoShader ||
                                        ptSameKeySprite->snoTexDiffuse != ptSprite->snoTexDiffuse ||
                                        ptSameKeySprite->snoTexAlpha != ptSprite->snoTexAlpha ||
                                        ptSameKeySprite->snoTexAlpha2 != ptSprite->snoTexAlpha2)
                                    {
                                        break;
                                    }

                                    dwMinVertexIndex = Math::Min(dwMinVertexIndex, tSpriteBucket[nBucket + 1].dwMinVertexIndex);
                                    dwMaxVertexIndex = Math::Max(dwMaxVertexIndex, tSpriteBucket[nBucket + 1].dwMaxVertexIndex);

                                    nBucket++;
                                    nSpriteCount += tScopedSpritePtrs.Count();
                                }
                            }

                            sDrawBatch(pdefLastSpriteShader, ptSprite->snoTexDiffuse, ptSprite->snoTexAlpha, ptSprite->snoTexAlpha2, ibidSprite, vbidSprite, 0, dwMinVertexIndex, dwMaxVertexIndex - dwMinVertexIndex + 1, dwStartIndex, nSpriteCount, fPrevWasNormal, fSameShader);
                            fPrevWasNormal = TRUE;
                            fSameShader = TRUE;
                        }
                    }
                    break;
                }
            }
        }

        nStartBucket = nEndBucket;
    }






    CameraManagerRestoreCurrentCameraToMainCamera();
}

// LINKER-REMOVED: DebugVisualizeDepth: concrete DWARF DIE (low_pc 0, 0x1d8 bytes; linker-dropped). Locals, inline tree and
// const values from the DWARF; the GfxRenderClear call (1164), the loop bound, flZ and the sDrawBatch arguments are inferred.
// vbidSprite lives in a stack slot (address taken) and dwStartIndex sits in w6 at the end: the out-of-line sDrawBatch call.
void DebugVisualizeDepth()
{

    const BOOL fInRenderDraw = TRUE;
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    VBID vbidSprite = ptSpriteGlobals->vbidSprite[ptSpriteGlobals->dwRenderDrawIndex];







    float flRTWidth, flRTHeight;
    GfxGetRenderTargetFloatDimensions(flRTWidth, flRTHeight);

    GfxRenderClear(TRUE, FALSE, FALSE, RGBAColor(0, 0, 0, 0));

    VertexColor vc(1, 1, 1, 255);
    VertexColor vcAux(0, 0, 0, 255);
    Rect2D rect;

    uint32 dwStartIndex;
    VertexSprite * ptVertices = sSpritesGfxVBBeginAlloc(fInRenderDraw, 1024, dwStartIndex);
    if (ptVertices)
    {
        for (int32 i = 0; i < 256; i++)
        {
            float flZ = i / 256.0f;

            Vector2D vpUVNull(0.0f, 0.0f);
            VertexSpriteSet(ptVertices[0], 0.0f, 0.0f, flZ, vc, vcAux, vpUVNull, vpUVNull, vpUVNull);
            VertexSpriteSet(ptVertices[1], 0.0f, flRTHeight, flZ, vc, vcAux, vpUVNull, vpUVNull, vpUVNull);
            VertexSpriteSet(ptVertices[2], flRTWidth, 0.0f, flZ, vc, vcAux, vpUVNull, vpUVNull, vpUVNull);
            VertexSpriteSet(ptVertices[3], flRTWidth, flRTHeight, flZ, vc, vcAux, vpUVNull, vpUVNull, vpUVNull);
            ptVertices += VERTICES_PER_SPRITE;
        }
        sSpritesGfxVBEndAlloc(fInRenderDraw);

        ScopedShaderDefinitionLockR pdefShader(GlobalSNOGet(GLOBALSNO_SHADER_DEBUG_DEPTH_VISUALIZATION));
        sDrawBatch(pdefShader, SNO_NONE, SNO_NONE, SNO_NONE, ptSpriteGlobals->ibidQuadSprite, vbidSprite, dwStartIndex, 0, 1024, 0, 256, FALSE, FALSE);
    }
}







static void sSpriteSubmitRActorSprite(
    DisplayList & tDisplayList,
    const RActorID idRActor,
    const Rect2D & wrBounds,
    const Rect2D & wrViewBounds,
    const float flZ,
    const SNO snoObserver,
    const SNO snoRenderTarget,
    const float flAspectRatio,
    const BOOL fClearZBuffer)
{
    if (idRActor == RACTORID_INVALID)
    {
        return;
    }

    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];


    if (tRenderData.nRActorSpriteCount >= MAX_RACTOR_SPRITES)
    {
        return;
    }

    RActor * ptRActor = RActorTryToGet(idRActor);
    // Inference: the inlined lookup's null result branches to the shared return.
    if (!ptRActor)
    {
        return;
    }
    RActorSprite * ptSprite = &tRenderData.ptRActorSprites[tRenderData.nRActorSpriteCount++];


    ptSprite->fIsFullscreenUIElement = FALSE;
    ptSprite->wrBounds = wrBounds;
    Rect2D::Intersect(wrBounds, wrViewBounds, &ptSprite->wrViewBounds);
    ptSprite->flZ = flZ;
    ptSprite->ptDisplayList = &tDisplayList;
    ptSprite->snoAppearance = ptRActor->snoAppearance;
    ptSprite->tActorLoc = ptRActor->tLocation.tWorldLocation;
    ptSprite->snoObserver = snoObserver;
    ptSprite->snoRenderTarget = snoRenderTarget;
    ptSprite->flAspectRatio = flAspectRatio;
    ptSprite->fClearZBuffer = fClearZBuffer;

    RenderLayer eRenderLayer = RENDERLAYER_UI;
    SpriteBucket * ptSpriteBucket = sFindComplexSpriteBucket(eRenderLayer, SPRITE_RACTOR, flZ);


    ptSpriteBucket->ptSpritePtrs->AddTail_Tracked(ptSprite);
}




static void sSpriteSubmitFullscreenRActorSprite(
    DisplayList & tDisplayList,
    const RActorID idRActor,
    const float flZ,
    const SNO snoRenderTarget,
    const RenderLayer eRenderLayer)
{
    if (idRActor == RACTORID_INVALID)
    {
        return;
    }

    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];


    if (tRenderData.nRActorSpriteCount >= MAX_RACTOR_SPRITES)
    {
        return;
    }

    RActor * ptRActor = RActorTryToGet(idRActor);
    // Inference: the inlined lookup's null result branches to the shared return.
    if (!ptRActor)
    {
        return;
    }
    RActorSprite * ptSprite = &tRenderData.ptRActorSprites[tRenderData.nRActorSpriteCount++];

    ptSprite->fIsFullscreenUIElement = TRUE; // Inference: following bounds use original float immediates 0, 0, 960, 720.
    ptSprite->wrBounds.Set(0.0f, 0.0f, 960.0f, 720.0f);
    ptSprite->wrViewBounds = ptSprite->wrBounds;
    ptSprite->flZ = flZ;
    ptSprite->ptDisplayList = &tDisplayList;
    ptSprite->snoAppearance = ptRActor->snoAppearance;
    ptSprite->tActorLoc = ptRActor->tLocation.tWorldLocation;
    ptSprite->snoObserver = GlobalSNOGet(GLOBALSNO_OBSERVER_HUD_ACTOR);
    ptSprite->snoRenderTarget = snoRenderTarget;
    ptSprite->flAspectRatio = scflFullscreenUIAspectRatio;
    ptSprite->fClearZBuffer = FALSE;

    SpriteBucket * ptSpriteBucket = sFindComplexSpriteBucket(eRenderLayer, SPRITE_RACTOR, flZ);


    ptSpriteBucket->ptSpritePtrs->AddTail_Tracked(ptSprite);
}




static void sSetCameraForRActorSprite(
    SNO snoAppearance,
    const WorldLocation & loc,
    RActorSprite & tSprite)
{
    const ObserverDefinition * pdefObserver = (const ObserverDefinition *)gptGroupDefObserver->LockRFast(tSprite.snoObserver);

    Vector3D wpLookFrom = loc.transform.wp + pdefObserver->wvCameraOffsetVector;
    Vector3D wpLookTo = loc.transform.wp + pdefObserver->wvLookAtOffsetVector;

    PRTransform transform;
    transform.Set(wpLookFrom, wpLookTo, cgwvZAxis);

    CameraState cms;
    if (tSprite.fIsFullscreenUIElement)
    {
        float flAspectRatioForCamera = GfxIsLetterboxed() ? (16.0f / 9.0f) : GfxGetAspectRatio();
        // Inference: last argument comes from a default argument (inlined Vector2D at the call line 1323).
        CameraStateSetOrthographic(
            cms,
            transform,
            scflFullscreenUIHeight,
            flAspectRatioForCamera,
            pdefObserver->flZNear,
            pdefObserver->flZFar, cgrectViewportDefault, cgvecViewportScaleDefault);
    }
    else
    {
        Rect2D rectPixels;
        Rect2D rectNDC;
        ConvertUICToPixels(tSprite.wrViewBounds, rectPixels);
        ConvertPixelsToNDC(rectPixels, rectNDC);

        Vector2D vecScale(1.0f, 1.0f);
        Vector2D vecTranslate(0.0f, 0.0f);
        if (tSprite.wrBounds != tSprite.wrViewBounds)
        {
            float flXOffset = (tSprite.wrBounds.right - tSprite.wrViewBounds.right) + (tSprite.wrBounds.left - tSprite.wrViewBounds.left);
            float flYOffset = (tSprite.wrBounds.bottom - tSprite.wrViewBounds.bottom) + (tSprite.wrBounds.top - tSprite.wrViewBounds.top);
            vecTranslate = Vector2D(flXOffset / tSprite.wrViewBounds.Width(), -flYOffset / tSprite.wrViewBounds.Height());
        }

        if (pdefObserver->fOrthographic)
        {
            float flAspectRatio = tSprite.wrBounds.Width() / tSprite.wrBounds.Height();

            float flHeight;
            {
                ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
                const AABB & aabb = pdefAppearance->tStructure.aabbBounds;

                flHeight = Math::Max(aabb.wp.z + aabb.wvExt.z, Math::Abs(aabb.wp.z - aabb.wvExt.z)) * loc.flScale;
                flHeight *= 2.0f;
            }

            if (tSprite.wrBounds != tSprite.wrViewBounds)
            {
                vecScale.x = tSprite.wrBounds.Width() / tSprite.wrViewBounds.Width();
                vecScale.y = tSprite.wrBounds.Height() / tSprite.wrViewBounds.Height();
            }

            CameraStateSetOrthographic(
                cms,
                transform,
                flHeight,
                flAspectRatio,
                pdefObserver->flZNear,
                pdefObserver->flZFar,
                cgrectViewportDefault,
                vecScale,
                vecTranslate);
        }
        else
        {


            Rect2D rectScreenNDC;
            CGameGetLetterboxedViewportNDC(rectScreenNDC);

            float flViewportWidth = rectNDC.Width();
            if (flViewportWidth >= 1e-6f)
            {
                vecScale.x = (vecScale.x * rectScreenNDC.Width()) / flViewportWidth;
            }
            float flViewportHeight = rectNDC.Height();
            if (flViewportHeight >= 1e-6f)
            {
                vecScale.y = (vecScale.y * rectScreenNDC.Height()) / flViewportHeight;
            }

            CameraStateSetPerspective(
                cms,
                transform,
                pdefObserver->aFOV,
                tSprite.flAspectRatio,
                pdefObserver->flZNear,
                pdefObserver->flZFar,
                pdefObserver->wvCameraOffsetVector,
                pdefObserver->wvLookAtOffsetVector,
                wpLookFrom,
                wpLookTo,
                rectNDC,
                vecScale,
                vecTranslate);
        }
    }


    CameraManagerSetCameraState(CAMERA_RACTOR_SPRITE, cms);
    CameraManagerSetCurrentCamera(CAMERA_RACTOR_SPRITE);
}




static void sDrawRActorSprite(
    RActorSprite & tSprite)
{
    sSetCameraForRActorSprite(tSprite.snoAppearance, tSprite.tActorLoc, tSprite);

    if (tSprite.snoRenderTarget != SNO_NONE)
    {
        GfxSetRenderAndDepthTarget(tSprite.snoRenderTarget, SNO_NONE, CUBEMAPFACE_POSX);
        GfxRenderClear(TRUE, TRUE, TRUE, RGBAColor(0, 0, 0, 255));
    }
    else
    {

        IRect2D rectMainViewport;
        ConvertUICToPixels(tSprite.wrViewBounds, rectMainViewport);

        uint32 dwRTWidth, dwRTHeight;
        GfxGetRenderTargetDimensions(dwRTWidth, dwRTHeight);
        if (tSprite.fIsFullscreenUIElement)
        {
            Rect2D rectViewportNDC;
            CGameGetLetterboxedViewportNDC(rectViewportNDC);
            rectMainViewport.left = (int)(rectViewportNDC.left * dwRTWidth);
            rectMainViewport.top = (int)(rectViewportNDC.top * dwRTHeight);
            rectMainViewport.right = (int)(rectViewportNDC.right * dwRTWidth);
            rectMainViewport.bottom = (int)(rectViewportNDC.bottom * dwRTHeight);
        }

        rectMainViewport.left = Math::Max(rectMainViewport.left, 0);
        rectMainViewport.top = Math::Max(rectMainViewport.top, 0);
        rectMainViewport.right = Math::Min(rectMainViewport.right, (int32)dwRTWidth);
        rectMainViewport.bottom = Math::Min(rectMainViewport.bottom, (int32)dwRTHeight);

        GfxViewportSet(rectMainViewport);
        if (tSprite.fClearZBuffer)
        {
            GfxRenderClear(FALSE, TRUE, FALSE, crgbaBlack);
        }
    }


    tSprite.ptDisplayList->DrawRenderLayer(RENDERLAYER_EARLY_OPAQUE);

    tSprite.ptDisplayList->DrawRenderLayer(RENDERLAYER_OPAQUE);
    tSprite.ptDisplayList->DrawRenderLayer(RENDERLAYER_TRANSPARENT);

    if (tSprite.snoRenderTarget != SNO_NONE)
    {
        GetRWindowMgr()->GetRWindow(RWindowMainWindow).SetRenderAndDepthTargetToDefaults();
    }
    else
    {

        IRect2D rectViewportPixels;
        CGameGetLetterboxedViewportPixels(rectViewportPixels);
        GfxViewportSet(rectViewportPixels);
    }

    CameraManagerRestoreCurrentCameraToMainCamera();
}




static void sDrawUISprite(
    UISprite & tSprite)
{
    IRect2D rectMainViewport;
    rectMainViewport.left = 0;
    rectMainViewport.top = 0;
    GfxGetCurrentDimensions((uint32 &)rectMainViewport.right, (uint32 &)rectMainViewport.bottom);
    GfxViewportSet(rectMainViewport);

    tSprite.Draw();
}




static void sSpritesDeleteAll()
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;
    SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];

    for (int32 eRenderLayer = 0; eRenderLayer < NUM_RENDERLAYERS; eRenderLayer++)
    {
        RenderLayerBucket & tRenderLayerBucket = tRenderData.tRenderLayerBuckets[eRenderLayer];
        tRenderLayerBucket.nCount = 0;
        tRenderLayerBucket.nLastBucket = -1;

        uint32 nCount = tRenderLayerBucket.tSpriteBuckets.size();
        for (uint32 i = 0; i < nCount; i++)
        {
            SpritePointerList * ptSpritePtrs = tRenderLayerBucket.tSpriteBuckets[i].ptSpritePtrs;
            if (tRenderLayerBucket.tSpriteBuckets[i].eType == SPRITE_RACTOR)
            {
                RActorSprite * ptSprite = (RActorSprite *)ptSpritePtrs->GetHead();
                ptSprite->ptDisplayList->RenderEnd();
            }
            tRenderData.tSpriteListPool.Free(ptSpritePtrs);
        }
        tRenderLayerBucket.tSpriteBuckets.clear();
    }

    tRenderData.tSpriteAllocator.Reset();
    tRenderData.nSpriteCount = 0;
    tRenderData.nRActorSpriteCount = 0;

    for (UISpritePoolInterface * ptSpritePool = s_ptUISpritePoolsHead; ptSpritePool; ptSpritePool = ptSpritePool->GetNext())
        ptSpritePool->DeleteAll();
}














void SpritesFlush()
{
    sSpritesGfxFlush();
    sSpritesDeleteAll();
}





static SpriteBucket * sFindSpriteBucket(RenderLayer eRenderLayer, uint32 dwKey, float flZ, BOOL fInRenderDraw)
{




    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[CGameAsyncGetRenderDataIndex(fInRenderDraw ? ASYNC_RENDER_DATA_DRAW : ASYNC_RENDER_DATA_UPDATE)];

    RenderLayerBucket & tRenderLayerBucket = tRenderData.tRenderLayerBuckets[eRenderLayer];
    RWindowMgr * ptRWindowMgr = GetRWindowMgr();
    RenderContext eContext = ptRWindowMgr ? ptRWindowMgr->GetCurrentRWindow().tAsyncData[0].dispList.GetRenderContext() : RENDERCONTEXT_DEFAULT;

    tRenderLayerBucket.nCount++;

    TSpriteBucketArray & tSpriteBuckets = tRenderLayerBucket.tSpriteBuckets;
    uint32 nCount = tSpriteBuckets.size();

    if (tRenderLayerBucket.nLastBucket != -1)
    {
        SpriteBucket & tLastBucket = tSpriteBuckets[tRenderLayerBucket.nLastBucket];

        if (tLastBucket.dwKey == dwKey &&
            tLastBucket.eContext == eContext &&
            tLastBucket.ptSpritePtrs->Count() < MAX_SPRITES_PER_BATCH &&
            Math::Abs(flZ - tLastBucket.flZ) < 1e-6f)
        {
            return &tLastBucket;
        }
    }

    SpriteBucket tBucket;
    tBucket.dwKey = dwKey;
    tBucket.flZ = flZ;
    tBucket.eType = SPRITE_NORMAL;
    tBucket.eContext = eContext;
    tBucket.dwStartIndex = 0;
    tBucket.dwMinVertexIndex = 0;
    tBucket.dwMaxVertexIndex = 0;
    tBucket.ptSpritePtrs = tRenderData.tSpriteListPool.Alloc_Tracked();
    tBucket.ptSpritePtrs->SetPool(&tRenderData.tSpritePointerPool);
    blzext::vector_set_at_grow(tSpriteBuckets, nCount, tBucket);

    tRenderLayerBucket.nLastBucket = nCount;
    return &tSpriteBuckets[nCount];
}





static SpriteBucket * sFindComplexSpriteBucket(
    const RenderLayer eRenderLayer,
    const SpriteType eType,
    const float flZ)
{
    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];

    RenderLayerBucket & tRenderLayerBucket = tRenderData.tRenderLayerBuckets[eRenderLayer];

    tRenderLayerBucket.nCount++;

    RWindowMgr * ptRWindowMgr = GetRWindowMgr();
    RenderContext eContext = ptRWindowMgr ? ptRWindowMgr->GetCurrentRWindow().tAsyncData[0].dispList.GetRenderContext() : RENDERCONTEXT_DEFAULT;

    TSpriteBucketArray & tSpriteBuckets = tRenderLayerBucket.tSpriteBuckets;
    uint32 nCount = tSpriteBuckets.size();

    SpriteBucket tBucket;
    tBucket.dwKey = 0;
    tBucket.flZ = flZ;
    tBucket.eType = eType;
    tBucket.eContext = eContext;
    tBucket.dwStartIndex = 0;
    tBucket.dwMinVertexIndex = 0;
    tBucket.dwMaxVertexIndex = 0;
    tBucket.ptSpritePtrs = tRenderData.tSpriteListPool.Alloc_Tracked();
    tBucket.ptSpritePtrs->SetPool(&tRenderData.tSpritePointerPool);
    blzext::vector_set_at_grow(tSpriteBuckets, nCount, tBucket);

    return &tSpriteBuckets[nCount];
}




static Sprite * sSpriteCreate(const ShaderDefinition * pdefShader, const SNO snoTexDiffuse, const SNO snoTexAlpha, const SNO snoTexAlpha2, const float flZ, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw, SpriteBucket ** pptSpriteBucket)








{


    SpriteGlobals * ptSpriteGlobals = CGameGet().ptSpriteGlobals;

    SpriteRenderData & tRenderData = ptSpriteGlobals->tRenderData[CGameAsyncGetRenderDataIndex(fInRenderDraw ? ASYNC_RENDER_DATA_DRAW : ASYNC_RENDER_DATA_UPDATE)];


    Sprite * ptSprite = tRenderData.tSpriteAllocator.Alloc_Tracked();
    ptSprite->snoShader = pdefShader->tHeader.snoHandle;
    ptSprite->snoTexDiffuse = snoTexDiffuse;
    ptSprite->snoTexAlpha = snoTexAlpha;
    ptSprite->snoTexAlpha2 = snoTexAlpha2;
    ptSprite->flZ = flZ;
    tRenderData.nSpriteCount++;


    RenderLayer eRenderLayer = eRenderLayerOverride;
    if (eRenderLayer == RENDERLAYER_INVALID)
        eRenderLayer = ((RenderPass *)pdefShader->ptPasses)->eRenderLayer;



    uint32 dwKey = pdefShader->tHeader.snoHandle + (snoTexDiffuse << 3) + (snoTexAlpha << 6) + (snoTexAlpha2 << 9);

















    SpriteBucket * ptSpriteBucket = sFindSpriteBucket(eRenderLayer, dwKey, flZ, fInRenderDraw);
    if (pptSpriteBucket)
        *pptSpriteBucket = ptSpriteBucket;

    ptSpriteBucket->ptSpritePtrs->AddTail_Tracked(ptSprite);

    return ptSprite;
}




static void sSpriteInitVertices(VertexSprite * ptVertex, const Vector2D * wpCorners, const Vector2D * wpUV, const Vector2D * wpMaskCorners, const Vector2D * wpMask2Corners, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flTexelOffset, const float flZ)
{
    const int32 aeCorners[4] = { CORNER_TL, CORNER_BL, CORNER_TR, CORNER_BR };
    VertexColor vc = rgbaColor;
    VertexColor vcAux = rgbaAuxColor;

    for (int32 i = 0; i < 4; i++)
    {
        int32 eCorner = aeCorners[i];














        VertexSpriteSet(ptVertex[i],
                        wpCorners[eCorner].x + flTexelOffset,
                        wpCorners[eCorner].y + flTexelOffset,
                        flZ, vc, vcAux, wpUV[eCorner], wpMaskCorners[eCorner], wpMask2Corners[eCorner]);
    }
}

















static void sSpriteInitVertices(VertexSprite * ptVertex, const Vector2D * wpCorners, const Vector2D * wpUV, const Vector2D * wpMaskCorners, const Vector2D * wpMask2Corners, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flTexelOffset, const float flZ, const Angle aYaw)
{














    VertexColor vc = rgbaColor;
    VertexColor vcAux = rgbaAuxColor;



    float flSinTheta = Math::Sin(aYaw);
    float flCosTheta = Math::Cos(aYaw);
    float flHalfWidth = fabsf(wpCorners[CORNER_TL].x - wpCorners[CORNER_TR].x) * 0.5f;
    float flHalfHeight = fabsf(wpCorners[CORNER_BR].y - wpCorners[CORNER_TR].y) * 0.5f;
    Vector2D wpCenter(wpCorners[CORNER_TL].x + flHalfWidth, wpCorners[CORNER_TL].y + flHalfHeight);

    for (int32 i = 0; i < 4; i++)
    {
        float flCornerOffsetX, flCornerOffsetY;
        int32 eCorner;
        switch (i)
        {
        case 0: eCorner = CORNER_TL; flCornerOffsetX = -flHalfWidth; flCornerOffsetY = -flHalfHeight; break;
        case 1: eCorner = CORNER_BL; flCornerOffsetX = -flHalfWidth; flCornerOffsetY = flHalfHeight; break;
        case 2: eCorner = CORNER_TR; flCornerOffsetX = flHalfWidth; flCornerOffsetY = -flHalfHeight; break;
        case 3: eCorner = CORNER_BR; flCornerOffsetX = flHalfWidth; flCornerOffsetY = flHalfHeight; break;
        }

        float flCornerX = flCornerOffsetX * flCosTheta - flCornerOffsetY * flSinTheta;
        float flCornerY = flCornerOffsetX * flSinTheta + flCornerOffsetY * flCosTheta;

        VertexSpriteSet(ptVertex[i],

                        wpCenter.x + flCornerX + flTexelOffset,
                        wpCenter.y + flCornerY + flTexelOffset,
                        flZ, vc, vcAux, wpUV[eCorner], wpMaskCorners[eCorner],
                        Vector2D(wpCenter.x + flCornerX + flTexelOffset, wpCenter.y + flCornerY + flTexelOffset));
    }
}















static void sSpriteInitVertices(VertexSprite * ptVertex, const Vector2D * wpCorners, const Vector2D * wpUV, const Vector2D * wpMaskCorners, const Vector2D * wpMask2Corners, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flTexelOffset, const float flZ, const BOOL bMirrorX, const BOOL bMirrorY)
{















    VertexColor vc = rgbaColor;
    VertexColor vcAux = rgbaAuxColor;



    BOOL bReverseOrder = (bMirrorX != bMirrorY);
    float flMirrorX = bMirrorX ? -1.0f : 1.0f;
    float flMirrorY = bMirrorY ? -1.0f : 1.0f;

    float flHalfWidth = fabsf(wpCorners[CORNER_TL].x - wpCorners[CORNER_TR].x) * 0.5f;
    float flHalfHeight = fabsf(wpCorners[CORNER_BR].y - wpCorners[CORNER_TR].y) * 0.5f;
    Vector2D wpCenter(wpCorners[CORNER_TL].x + flHalfWidth, wpCorners[CORNER_TL].y + flHalfHeight);

    if (bReverseOrder)
    {
        for (int32 i = 0; i < 4; i++)
        {
            float flCornerOffsetX, flCornerOffsetY;
            int32 eCorner;
            switch (i)
            {
            case 0: eCorner = CORNER_TR; flCornerOffsetX = flHalfWidth; flCornerOffsetY = -flHalfHeight; break;
            case 1: eCorner = CORNER_BR; flCornerOffsetX = flHalfWidth; flCornerOffsetY = flHalfHeight; break;
            case 2: eCorner = CORNER_TL; flCornerOffsetX = -flHalfWidth; flCornerOffsetY = -flHalfHeight; break;
            case 3: eCorner = CORNER_BL; flCornerOffsetX = -flHalfWidth; flCornerOffsetY = flHalfHeight; break;
            }

            float flCornerX = flCornerOffsetX * flMirrorX;
            float flCornerY = flCornerOffsetY * flMirrorY;

            VertexSpriteSet(ptVertex[i],

                            wpCenter.x + flCornerX + flTexelOffset,
                            wpCenter.y + flCornerY + flTexelOffset,
                            flZ, vc, vcAux, wpUV[eCorner], wpMaskCorners[eCorner],
                            Vector2D(wpCenter.x + flCornerX + flTexelOffset, wpCenter.y + flCornerY + flTexelOffset));
        }
    }
    else
    {



        for (int32 i = 0; i < 4; i++)
        {
            float flCornerOffsetX, flCornerOffsetY;
            int32 eCorner;
            switch (i)
            {
            case 0: eCorner = CORNER_TL; flCornerOffsetX = -flHalfWidth; flCornerOffsetY = -flHalfHeight; break;
            case 1: eCorner = CORNER_BL; flCornerOffsetX = -flHalfWidth; flCornerOffsetY = flHalfHeight; break;
            case 2: eCorner = CORNER_TR; flCornerOffsetX = flHalfWidth; flCornerOffsetY = -flHalfHeight; break;
            case 3: eCorner = CORNER_BR; flCornerOffsetX = flHalfWidth; flCornerOffsetY = flHalfHeight; break;
            }

            float flCornerX = flCornerOffsetX * flMirrorX;
            float flCornerY = flCornerOffsetY * flMirrorY;

            VertexSpriteSet(ptVertex[i],

                            wpCenter.x + flCornerX + flTexelOffset,
                            wpCenter.y + flCornerY + flTexelOffset,
                            flZ, vc, vcAux, wpUV[eCorner], wpMaskCorners[eCorner],
                            Vector2D(wpCenter.x + flCornerX + flTexelOffset, wpCenter.y + flCornerY + flTexelOffset));
        }
    }
}









































static BOOL sClipSprite(const TextureDefinition * pdefTexture, const uint32 dwFrameIndex, const Rect2D & wrScreen, const Rect2D * const pwrClipping, Rect2D & wrScreenClipped, Rect2D & wrTextureClipped, Rect2D * pwrPreCalculatedTextureUVs)
{






    Rect2D wrTextureUVs;

    if (pwrPreCalculatedTextureUVs)
    {
        wrTextureUVs = *pwrPreCalculatedTextureUVs;
    }
    else if (pdefTexture == NULL || dwFrameIndex == (uint32)-1)
    {
        wrTextureUVs.Set(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        TextureGetFrameRectUVs(wrTextureUVs, pdefTexture, dwFrameIndex);
    }

    if (pwrClipping)
    {

        BOOL bVisible = Rect2D::Intersect(wrScreen, *pwrClipping, &wrScreenClipped);
        if (!bVisible)
            return FALSE;



        Rect2D wrFrameSpace;

        wrScreenClipped.ToUnitCoords(wrScreen, wrFrameSpace);
        wrFrameSpace.FromUnitCoords(wrTextureUVs, wrTextureClipped);
    }
    else
    {
        wrScreenClipped = wrScreen;
        wrTextureClipped = wrTextureUVs;
    }

    return TRUE;
}

struct SpriteDrawDeferredData2;
static void sBufferSpriteDrawDeferred(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);

static void sInitStereo3DCorners(const float flStereoDepth, Vector2D * wpCorners)
{
    for (int32 i = 0; i < 4; i++)
    {


        wpCorners[i] = Vector2D(flStereoDepth, flStereoDepth);
    }
}

// DWARF Sprite.cpp:3691, forward declaration for its real caller.
static void sBufferSpriteDrawDeferred(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const ImageTextureFrame & tImageTexFrameMask, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D *const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
// NON_MATCHING: 1752/1780 bytes; seven vertex/UV store pairs merge in the no-stereo branch; both same-line local orders tried.
void SpriteDrawDeferred(
    const ImageTextureFrame & tImageTexFrame,
    const ImageTextureFrame & tImageTexFrameMask,
    const SNO snoShader,
    const Rect2D & rectScreen,
    const RGBAColor & rgbaColor,
    const RGBAColor & rgbaAuxColor,
    const float flZ,
    const float flStereoDepth,
    const Rect2D *const pwrClipping,
    const RenderLayer eRenderLayerOverride,
    BOOL fInRenderDraw)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferred(pSpriteThreadBuffer, tImageTexFrame, tImageTexFrameMask, snoShader, rectScreen, rgbaColor, rgbaAuxColor, flZ, flStereoDepth, pwrClipping, eRenderLayerOverride, fInRenderDraw); // Inference: this branch reaches the original shared epilogue.
        return;
    }


    const TextureDefinition * pdefTexture = NULL;
    if (tImageTexFrame.snoTexture != SNO_NONE)
    {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
        { // Inference: this branch reaches the original shared epilogue.
            return;
        }
 // Inference: the DWARF pointer type identifies the object returned by this void-pointer API.
        pdefTexture = static_cast<const TextureDefinition *>(gptGroupDefTexture->LockR(tImageTexFrame.snoTexture, FALSE));
    }




#line 2043
    Rect2D wrScreenClipped;
#line 2043
    Rect2D wrTextureClipped;
#line 2044
    if (!sClipSprite(pdefTexture, tImageTexFrame.nFrame, rectScreen, pwrClipping, wrScreenClipped, wrTextureClipped, NULL))
    {
        // Inference: UnlockR requires void*& although DWARF records a const definition pointer.
        if (pdefTexture) gptGroupDefTexture->UnlockR((void *&)pdefTexture); // Inference: this branch reaches the original shared epilogue.
        return;
    }
    const TextureDefinition * pdefMaskTexture = NULL;
    if (tImageTexFrameMask.snoTexture != SNO_NONE)
    {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrameMask.snoTexture), 2))
        {
            // Inference: original UnlockR clears the DWARF const definition pointer through void*&.
            if (pdefTexture) gptGroupDefTexture->UnlockR((void *&)pdefTexture); // Inference: this branch reaches the original shared epilogue.
            return;
        } // Inference: the DWARF pointer type identifies the object returned by this void-pointer API.
        pdefMaskTexture = static_cast<const TextureDefinition *>(gptGroupDefTexture->LockR(tImageTexFrameMask.snoTexture, FALSE));
    }









    // Inference: absent override uses the original immediate 9, GLOBALSNO_SHADER_SPRITE_DEFAULT.
    SNO snoShaderToUse = snoShader;
    if (snoShaderToUse == SNO_NONE)
    {
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_DEFAULT);
    }
    ScopedShaderDefinitionLockR pdefShader(snoShaderToUse);

    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL)
    {
        // Inference: both original cleanup calls use void*& for the DWARF const definition pointers.
        if (pdefTexture) gptGroupDefTexture->UnlockR((void *&)pdefTexture); // Inference: UnlockR accepts void*& for this DWARF const definition pointer.
        if (pdefMaskTexture) gptGroupDefTexture->UnlockR((void *&)pdefMaskTexture); // Inference: this branch reaches the original shared epilogue.
        return;
    }
    Sprite * ptSprite = sSpriteCreate(pdefShader, tImageTexFrame.snoTexture, tImageTexFrameMask.snoTexture, SNO_NONE, flZ, eRenderLayerOverride, fInRenderDraw, NULL);
    // Inference: original failure exits unlock both definitions before ending vertex allocation.
    if (!ptSprite)
    { // Inference: UnlockR accepts void*& for this DWARF const definition pointer.
        if (pdefTexture) gptGroupDefTexture->UnlockR((void *&)pdefTexture); // Inference: UnlockR accepts void*& for this DWARF const definition pointer.
        if (pdefMaskTexture) gptGroupDefTexture->UnlockR((void *&)pdefMaskTexture);
        sSpritesGfxVBEndAlloc(fInRenderDraw); // Inference: this branch reaches the original shared epilogue.
        return;
    }

    ptSprite->dwVertexIndex = dwStartIndex;

    Vector2D wpCorners[4];
    Vector2D wpUV[4];
    Vector2D wpUVMask[4];

    wrScreenClipped.ToPoints(wpCorners, 0);
    wrTextureClipped.ToPoints(wpUV, 0);

    if (pdefMaskTexture)
    {
    Rect2D wrMaskScreenClipped;
#line 2108
    Rect2D wrMaskTextureClipped;
#line 2109
        if (!sClipSprite(pdefMaskTexture, tImageTexFrameMask.nFrame, rectScreen, pwrClipping, wrMaskScreenClipped, wrMaskTextureClipped, NULL))
        {
            // Inference: this failed clip unlocks definitions, with no VBEndAlloc call in the original.
            if (pdefTexture) gptGroupDefTexture->UnlockR((void *&)pdefTexture); // Inference: UnlockR accepts void*& for this DWARF const definition pointer.
            if (pdefMaskTexture) gptGroupDefTexture->UnlockR((void *&)pdefMaskTexture); // Inference: this branch reaches the original shared epilogue.
            return;
        }
        wrMaskTextureClipped.ToPoints(wpUVMask, 0);
    }
    else
    {
        SigmaMemoryCopy(wpUVMask, wpUV, sizeof(wpUVMask));
    }
#line 2124
    const float flTexelCenterOffset = 0.0f;
#line 2129
    if (flStereoDepth == 1.0f)
    {
        sSpriteInitVertices(ptVertex, wpCorners, wpUV, wpUVMask, wpCorners, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }
    else
    {









#line 2144
        Vector2D wpStereoDepth[4];

        sInitStereo3DCorners(flStereoDepth, wpStereoDepth);
        sSpriteInitVertices(ptVertex, wpCorners, wpUV, wpUVMask, wpStereoDepth, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }







    // Inference: original UnlockR calls require void*& casts of the DWARF const definition pointers.
#line 2159 // Inference: UnlockR accepts void*& for this DWARF const definition pointer.
    if (pdefTexture) gptGroupDefTexture->UnlockR((void *&)pdefTexture); // Inference: UnlockR accepts void*& for this DWARF const definition pointer.
    if (pdefMaskTexture) gptGroupDefTexture->UnlockR((void *&)pdefMaskTexture);
    sSpritesGfxVBEndAlloc(fInRenderDraw);
}
#line 2163


// NON_MATCHING: 2 instructions swapped after the inlined sSpritesGfxVBBeginAlloc GfxVBLock call (original: ldr, add, mov x26,x0, str;
// ours: ldr, mov, add, str). The fInRenderDraw path of that helper is not proven by any matched caller yet.
void SpriteDrawDeferred(
    const ImageTextureFrame & tImageTexFrame,
    const SNO snoShader,
    const Rect2D & rectScreen,
    const RGBAColor & rgbaColor,
    const RGBAColor & rgbaAuxColor,
    const float flZ,
    const Rect2D * const pwrClipping,
    const SNO snoTexAlpha,
    const Rect2D * pwrTexAlphaCoords,
    const BOOL fFlipV,
    const RenderLayer eRenderLayerOverride,
    BOOL fInRenderDraw)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferred(pSpriteThreadBuffer, tImageTexFrame, snoShader, rectScreen, rgbaColor, rgbaAuxColor, flZ, pwrClipping, snoTexAlpha, pwrTexAlphaCoords, fFlipV, eRenderLayerOverride, fInRenderDraw);
        return;
    }


    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
    {
        return;
    }

    const TextureDefinition * pdefTexture = NULL;
    if (tImageTexFrame.snoTexture != SNO_NONE)
    {
        pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR(tImageTexFrame.snoTexture, FALSE);
    }










    Rect2D wrScreenClipped, wrTextureClipped;
    if (!sClipSprite(pdefTexture, tImageTexFrame.nFrame, rectScreen, pwrClipping, wrScreenClipped, wrTextureClipped, NULL))
    {
        if (pdefTexture)
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        return;
    }

    SNO snoShaderToUse = snoShader;
    if (snoShaderToUse == SNO_NONE)
    {
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_DEFAULT);
    }
    ScopedShaderDefinitionLockR pdefShader(snoShaderToUse);

    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL)
    {
        if (pdefTexture)
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        return;
    }
    Sprite * ptSprite = sSpriteCreate(pdefShader, tImageTexFrame.snoTexture, snoTexAlpha, SNO_NONE, flZ, eRenderLayerOverride, fInRenderDraw, NULL);

    if (ptSprite == NULL)
    {
        if (pdefTexture)
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        sSpritesGfxVBEndAlloc(fInRenderDraw);
        return;
    }
    ptSprite->dwVertexIndex = dwStartIndex;

    Vector2D wpCorners[4];
    wrScreenClipped.ToPoints(wpCorners, 0);

    Vector2D wpMaskCorners[4];
    Vector2D * pwpMaskCorners = wpCorners;
    if (pwrTexAlphaCoords)
    {
        pwrTexAlphaCoords->ToPoints(wpMaskCorners, 0);
        pwpMaskCorners = wpMaskCorners;
    }

    Vector2D wpUV[4];
    wrTextureClipped.ToPoints(wpUV, 0);

    if (fFlipV)
    {
        for (int i = 0; i < 4; i++)
        {
            wpUV[i].y = 1.0f - wpUV[i].y;
        }
    }




    const float flTexelCenterOffset = 0.0f;



    sSpriteInitVertices(ptVertex, wpCorners, wpUV, pwpMaskCorners, wpCorners, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);







    if (pdefTexture)
        gptGroupDefTexture->UnlockR((void *&)pdefTexture);


    sSpritesGfxVBEndAlloc(fInRenderDraw);
}

static void sBufferSpriteDrawDeferredRotated(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const Angle aYaw, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride);

void SpriteDrawDeferredRotated(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const Angle aYaw, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferredRotated(pSpriteThreadBuffer, tImageTexFrame, snoShader, rectScreen, rgbaColor, rgbaAuxColor, flZ, pwrClipping, aYaw, snoTexAlpha, pwrTexAlphaCoords, fFlipV, eRenderLayerOverride);
        return;
    }

    BOOL fInRenderDraw = FALSE;
    const TextureDefinition * pdefTexture = NULL;
    if (tImageTexFrame.snoTexture != SNO_NONE)
    {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
        {
            return;
        }

        pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR(tImageTexFrame.snoTexture, FALSE);
    }

    Rect2D wrScreenClipped, wrTextureClipped;
    if (!sClipSprite(pdefTexture, tImageTexFrame.nFrame, rectScreen, pwrClipping, wrScreenClipped, wrTextureClipped, NULL))
    {
        if (pdefTexture)
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        return;
    }

    SNO snoShaderToUse = snoShader;
    if (snoShaderToUse == SNO_NONE)
    {
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_DEFAULT);
    }
    ScopedShaderDefinitionLockR pdefShader(snoShaderToUse);

    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL)
    {
        if (pdefTexture)
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        return;
    }

    Sprite * ptSprite = sSpriteCreate(pdefShader, tImageTexFrame.snoTexture, snoTexAlpha, SNO_NONE, flZ, eRenderLayerOverride, fInRenderDraw, NULL);

    if (ptSprite)
    {
        ptSprite->dwVertexIndex = dwStartIndex;

        Vector2D wpCorners[4];
        wrScreenClipped.ToPoints(wpCorners, 0);

        Vector2D wpMaskCorners[4];
        Vector2D * pwpMaskCorners = wpCorners;
        if (pwrTexAlphaCoords)
        {
            pwrTexAlphaCoords->ToPoints(wpMaskCorners, 0);
            pwpMaskCorners = wpMaskCorners;
        }

        Vector2D wpUV[4];
        wrTextureClipped.ToPoints(wpUV, 0);

        if (fFlipV)
        {
            for (int i = 0; i < 4; i++)
            {
                wpUV[i].y = 1.0f - wpUV[i].y;
            }
        }

        const float flTexelCenterOffset = 0.0f;

        sSpriteInitVertices(ptVertex, wpCorners, wpUV, pwpMaskCorners, wpCorners, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ, aYaw);
    }

    if (pdefTexture)
        gptGroupDefTexture->UnlockR((void *&)pdefTexture);

    sSpritesGfxVBEndAlloc(fInRenderDraw);
}





































static void sBufferSpriteDrawDeferredMirrored(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const BOOL bMirrorX, const BOOL bMirrorY, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride);

void SpriteDrawDeferredMirrored(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const BOOL bMirrorX, const BOOL bMirrorY, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferredMirrored(pSpriteThreadBuffer, tImageTexFrame, snoShader, rectScreen, rgbaColor, rgbaAuxColor, flZ, pwrClipping, bMirrorX, bMirrorY, snoTexAlpha, pwrTexAlphaCoords, fFlipV, eRenderLayerOverride);
        return;
    }

    BOOL fInRenderDraw = FALSE;
    const TextureDefinition * pdefTexture = NULL;
    if (tImageTexFrame.snoTexture != SNO_NONE)
    {
        if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
        {
            return;
        }

        pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR(tImageTexFrame.snoTexture, FALSE);
    }

    Rect2D wrScreenClipped, wrTextureClipped;
    if (!sClipSprite(pdefTexture, tImageTexFrame.nFrame, rectScreen, pwrClipping, wrScreenClipped, wrTextureClipped, NULL))
    {
        if (pdefTexture)
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        return;
    }

    SNO snoShaderToUse = snoShader;
    if (snoShaderToUse == SNO_NONE)
    {
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_DEFAULT);
    }
    ScopedShaderDefinitionLockR pdefShader(snoShaderToUse);

    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL)
    {
        if (pdefTexture)
            gptGroupDefTexture->UnlockR((void *&)pdefTexture);
        return;
    }

    Sprite * ptSprite = sSpriteCreate(pdefShader, tImageTexFrame.snoTexture, snoTexAlpha, SNO_NONE, flZ, eRenderLayerOverride, fInRenderDraw, NULL);

    if (ptSprite)
    {
        ptSprite->dwVertexIndex = dwStartIndex;

        Vector2D wpCorners[4];
        wrScreenClipped.ToPoints(wpCorners, 0);

        Vector2D wpMaskCorners[4];
        Vector2D * pwpMaskCorners = wpCorners;
        if (pwrTexAlphaCoords)
        {
            pwrTexAlphaCoords->ToPoints(wpMaskCorners, 0);
            pwpMaskCorners = wpMaskCorners;
        }

        Vector2D wpUV[4];
        wrTextureClipped.ToPoints(wpUV, 0);

        if (fFlipV)
        {
            for (int i = 0; i < 4; i++)
            {
                wpUV[i].y = 1.0f - wpUV[i].y;
            }
        }

        const float flTexelCenterOffset = 0.0f;

        sSpriteInitVertices(ptVertex, wpCorners, wpUV, pwpMaskCorners, wpCorners, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ, bMirrorX, bMirrorY);
    }

    if (pdefTexture)
        gptGroupDefTexture->UnlockR((void *&)pdefTexture);

    sSpritesGfxVBEndAlloc(fInRenderDraw);
}












































void SpriteDrawDeferred(const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw)











{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferred(pSpriteThreadBuffer, tImageTexFrame, snoShader, pCoords, flScaleX, flScaleY, rgbaColor, rgbaAuxColor, flZ, pwrClipping, eRenderLayerOverride, fInRenderDraw);
        return;
    }


    const TextureDefinition * pdefTexture = NULL;
    if (tImageTexFrame.snoTexture == SNO_NONE)
        return;

    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
        return;


    pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR(tImageTexFrame.snoTexture, FALSE);







    SNO snoShaderToUse = snoShader;
    if (snoShaderToUse == SNO_NONE)
    {
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_DEFAULT);
    }

    ScopedShaderDefinitionLockR pdefShader(snoShaderToUse);

    SpriteDrawDeferred(pdefTexture, (float)pdefTexture->dwWidth, (float)pdefTexture->dwHeight, tImageTexFrame.nFrame, pdefShader, pCoords, flScaleX, flScaleY, rgbaColor, rgbaAuxColor, flZ, 1.0f, pwrClipping, eRenderLayerOverride, fInRenderDraw);

    if (pdefTexture)
        gptGroupDefTexture->UnlockR((void *&)pdefTexture);
}

// NON_MATCHING: 8 bytes short; the sprite-NULL exit shares the GfxVBUnlock tail block, the original gives it its own copy

void SpriteDrawDeferred(
    const TextureDefinition * pdefTexture,
    const float flTexWidth,
    const float flTexHeight,
    const uint32 dwFrameIndex,
    const ShaderDefinition * pdefShader,
    const Vector2D & pCoords,
    const float flScaleX,
    const float flScaleY,
    const RGBAColor & rgbaColor,
    const RGBAColor & rgbaAuxColor,
    const float flZ,
    const float flStereoDepth,
    const Rect2D * const pwrClipping,
    const RenderLayer eRenderLayerOverride,
    BOOL fInRenderDraw)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferred(pSpriteThreadBuffer, pdefTexture, flTexWidth, flTexHeight, dwFrameIndex, pdefShader, pCoords, flScaleX, flScaleY, rgbaColor, rgbaAuxColor, flZ, flStereoDepth, pwrClipping, eRenderLayerOverride, fInRenderDraw);
        return;
    }









    if (pdefTexture == NULL)
        return;



    Rect2D wrTextureUVs;
    if (dwFrameIndex == (uint32)-1)
    {
        wrTextureUVs.Set(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        TextureGetFrameRectUVs(wrTextureUVs, pdefTexture, dwFrameIndex);
    }

    Rect2D rectScreen;
    {

        float flWidthPixel = wrTextureUVs.Width() * flTexWidth;
        float flHeightPixel = wrTextureUVs.Height() * flTexHeight;

        rectScreen.left = pCoords.x;
        rectScreen.top = pCoords.y;
        rectScreen.right = flWidthPixel * flScaleX + rectScreen.left;
        rectScreen.bottom = flHeightPixel * flScaleY + rectScreen.top;
    }

    Rect2D wrScreenClipped, wrTextureClipped;
    if (!sClipSprite(pdefTexture, dwFrameIndex, rectScreen, pwrClipping, wrScreenClipped, wrTextureClipped, &wrTextureUVs))
        return;



    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL)
        return;



    Sprite * ptSprite = sSpriteCreate(pdefShader, pdefTexture->tHeader.snoHandle, SNO_NONE, SNO_NONE, flZ, eRenderLayerOverride, fInRenderDraw, NULL);
    if (ptSprite == NULL)
    {
        sSpritesGfxVBEndAlloc(fInRenderDraw);
        return;
    }

    ptSprite->dwVertexIndex = dwStartIndex;

    Vector2D wpCorners[4];
    wrScreenClipped.ToPoints(wpCorners, 0);

    Vector2D wpUV[4];
    wrTextureClipped.ToPoints(wpUV, 0);



    const float flTexelCenterOffset = 0.0f;




    if (flStereoDepth == 1.0f)
    {
        sSpriteInitVertices(ptVertex, wpCorners, wpUV, wpCorners, wpCorners, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }
    else
    {









        Vector2D wpStereoDepth[4];
        sInitStereo3DCorners(flStereoDepth, wpStereoDepth);
        sSpriteInitVertices(ptVertex, wpCorners, wpUV, wpCorners, wpStereoDepth, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }











    sSpritesGfxVBEndAlloc(fInRenderDraw);
}
// Two clipped rectangles share DWARF line 2780; line directives keep each declaration separate.
// DWARF Sprite.cpp:3346; declaration for the real thread-buffer caller.
static void sBufferSpriteDrawDeferredGlyph(XCommandBuffer * pBuffer, const TextureDefinition * pdefTexture, const float flTexWidth, const float flTexHeight, const uint32 dwFrameIndex, const ShaderDefinition * pdefShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D *const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);
// NON_MATCHING: 1456/1464 bytes; allocation-helper null-return/shared-tail merging differs; faithful sibling spelling tried.
void SpriteDrawDeferredGlyph(
    const TextureDefinition * pdefTexture,
    const float flTexWidth,
    const float flTexHeight,
    const uint32 dwFrameIndex,
    const ShaderDefinition * pdefShader,
    const Vector2D & pCoords,
    const float flScaleX,
    const float flScaleY,
    const RGBAColor & rgbaColor,
    const RGBAColor & rgbaAuxColor,
    const float flZ,
    const float flStereoDepth,
    const Rect2D *const pwrClipping,
    const RenderLayer eRenderLayerOverride,
    BOOL fInRenderDraw)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferredGlyph(pSpriteThreadBuffer, pdefTexture, flTexWidth, flTexHeight, dwFrameIndex, pdefShader, pCoords, flScaleX, flScaleY, rgbaColor, rgbaAuxColor, flZ, flStereoDepth, pwrClipping, eRenderLayerOverride, fInRenderDraw); // Inference: this branch reaches the original shared epilogue.
        return;
    }








    // Inference: the null-texture branch returns to the shared epilogue.
    if (pdefTexture == NULL) // Inference: this branch reaches the original shared epilogue.
        return;



    Rect2D wrTextureUVs;
    if (dwFrameIndex == (uint32)-1)
    {
        wrTextureUVs.Set(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        TextureGetFrameRectUVs(wrTextureUVs, pdefTexture, dwFrameIndex);
    }

    Rect2D rectScreen;
    {

        float flWidthPixel = wrTextureUVs.Width() * flTexWidth;
        float flHeightPixel = wrTextureUVs.Height() * flTexHeight;

        rectScreen.left = pCoords.x;
        rectScreen.top = pCoords.y;
        rectScreen.right = rectScreen.left + flWidthPixel * flScaleX;
        rectScreen.bottom = rectScreen.top + flHeightPixel * flScaleY;
    }
#line 2780
    Rect2D wrScreenClipped;
#line 2780
    Rect2D wrTextureClipped;
#line 2781
    if (!sClipSprite(pdefTexture, dwFrameIndex, rectScreen, pwrClipping, wrScreenClipped, wrTextureClipped, &wrTextureUVs)) // Inference: this branch reaches the original shared epilogue.
        return;
#line 2786
    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL) // Inference: this branch reaches the original shared epilogue.
        return;

#line 2793
    Sprite * ptSprite = sSpriteCreate(pdefShader, pdefTexture->tHeader.snoHandle, SNO_NONE, SNO_NONE, flZ, eRenderLayerOverride, fInRenderDraw, NULL);
    if (ptSprite == NULL)
    {
        sSpritesGfxVBEndAlloc(fInRenderDraw);
        return;
    }

    ptSprite->dwVertexIndex = dwStartIndex;

    Vector2D wpCorners[4];
    wrScreenClipped.ToPoints(wpCorners, 0);

    Vector2D wpUV[4];
    wrTextureClipped.ToPoints(wpUV, 0);

    const Vector2D wpUV1[4] = { Vector2D(-1.0f, -1.0f), Vector2D(1.0f, -1.0f), Vector2D(1.0f, 1.0f), Vector2D(-1.0f, 1.0f) };



    const float flTexelCenterOffset = 0.0f;




    if (flStereoDepth == 1.0f)
    {
        sSpriteInitVertices(ptVertex, wpCorners, wpUV, wpUV1, wpCorners, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }
    else
    {









        Vector2D wpStereoDepth[4];

        sInitStereo3DCorners(flStereoDepth, wpStereoDepth);
        sSpriteInitVertices(ptVertex, wpCorners, wpUV, wpUV1, wpStereoDepth, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }










    sSpritesGfxVBEndAlloc(fInRenderDraw);
}




void SpriteDrawDeferredMinimapTile(const SNO snoMapTex, const SNO snoShader, const Vector2D * wpCorners, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const SNO snoTexMask, const Vector2D * wpMaskCorners, const SNO snoTexExploredMask, const Vector2D * wpExploredMaskCorners)










{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawDeferredMinimapTile(pSpriteThreadBuffer, snoMapTex, snoShader, wpCorners, rgbaColor, rgbaAuxColor, flZ, snoTexMask, wpMaskCorners, snoTexExploredMask, wpExploredMaskCorners);
        return;
    }

    BOOL fInRenderDraw = FALSE;


    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoMapTex), 2))
        return;



    ScopedTextureDefinitionLockR pdefTexture(snoMapTex);

    Rect2D wrTextureUVs;

    if (pdefTexture->dwFrameCount)
    {











        const float flTexelCenterOffset = 0.5f;
        Vector2D vecHalfTexel;
        vecHalfTexel.x = flTexelCenterOffset / (float)pdefTexture->dwWidth;
        vecHalfTexel.y = flTexelCenterOffset / (float)pdefTexture->dwHeight;




        wrTextureUVs.Set(((TexFrame *)pdefTexture->ptFrame)[0].flU0 + vecHalfTexel.x,
                         ((TexFrame *)pdefTexture->ptFrame)[0].flV0 + vecHalfTexel.y,
                         ((TexFrame *)pdefTexture->ptFrame)[0].flU1 - vecHalfTexel.x,
                         ((TexFrame *)pdefTexture->ptFrame)[0].flV1 - vecHalfTexel.y);
    }
    else
        wrTextureUVs.Set(0.0f, 0.0f, 1.0f, 1.0f);


    SNO snoShaderToUse = snoShader;
    if (snoShaderToUse == SNO_NONE)
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_DEFAULT);

    ScopedShaderDefinitionLockR pdefShader(snoShaderToUse);

    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL)
        return;



    Sprite * ptSprite = sSpriteCreate(pdefShader, snoMapTex, snoTexMask, snoTexExploredMask, flZ, RENDERLAYER_INVALID, fInRenderDraw, NULL);
    if (ptSprite == NULL)
        return;




    ptSprite->dwVertexIndex = dwStartIndex;

    Vector2D wpUV[4];
    wrTextureUVs.ToPoints(wpUV, 0);

    sSpriteInitVertices(ptVertex, wpCorners, wpUV, wpMaskCorners, wpExploredMaskCorners, rgbaColor, rgbaAuxColor, 0.0f, flZ);
}















void SpriteDrawSimple(const SNO snoTexture, const SNO snoShader, const Rect2D & wrScreen, const Vector2D * wpUVs, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const RenderLayer eRenderLayerOverride)









{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawSimple(pSpriteThreadBuffer, snoTexture, snoShader, wrScreen, wpUVs, rgbaColor, rgbaAuxColor, flZ, flStereoDepth, eRenderLayerOverride);
        return;
    }

    BOOL fInRenderDraw = FALSE;


    if (snoTexture == SNO_NONE)
        return;





    SNO snoShaderToUse = snoShader;
    if (snoShaderToUse == SNO_NONE)
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_DEFAULT);

    ScopedShaderDefinitionLockR pdefShader(snoShaderToUse);

    uint32 dwStartIndex;
    VertexSprite * ptVertex = sSpritesGfxVBBeginAlloc(fInRenderDraw, VERTICES_PER_SPRITE, dwStartIndex);
    if (ptVertex == NULL)
        return;



    Sprite * ptSprite = sSpriteCreate(pdefShader, snoTexture, SNO_NONE, SNO_NONE, flZ, eRenderLayerOverride, fInRenderDraw, NULL);
    if (ptSprite == NULL)
        return;




    ptSprite->dwVertexIndex = dwStartIndex;

    Vector2D wpCorners[4];
    wrScreen.ToPoints(wpCorners, 0);






    const float flTexelCenterOffset = 0.0f;

    if (flStereoDepth == 1.0f)
    {
        sSpriteInitVertices(ptVertex, wpCorners, wpUVs, wpCorners, wpCorners, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }
    else
    {










        Vector2D wpStereoDepth[4];
        sInitStereo3DCorners(flStereoDepth, wpStereoDepth);
        sSpriteInitVertices(ptVertex, wpCorners, wpUVs, wpCorners, wpStereoDepth, rgbaColor, rgbaAuxColor, flTexelCenterOffset, flZ);
    }
}















void SpriteDrawRActorSprite(
    int32 nCameraIndex,
    DisplayList * pDisplayList,
    const RActorID idRActor,
    const Rect2D & wrBounds,
    const Rect2D & wrViewBounds,
    float flDeltaTime,
    const float flZ,
    const SNO snoObserver,
    const SNO snoRenderTarget,
    const float flAspectRatio,
    const BOOL fClearZBuffer)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawRActorSprite(pSpriteThreadBuffer, nCameraIndex, pDisplayList, idRActor, wrBounds, wrViewBounds, flDeltaTime, flZ, snoObserver, snoRenderTarget, flAspectRatio, fClearZBuffer);
        return;
    }

    const Camera & cam = CameraManagerGetCamera(nCameraIndex);
    pDisplayList->RenderBegin();
    RActorGfxRenderRActorInDisplayList(cam, *pDisplayList, NULL, NULL, idRActor, FALSE, flDeltaTime, FALSE);
    pDisplayList->Sort();

    sSpriteSubmitRActorSprite(*pDisplayList, idRActor, wrBounds, wrViewBounds, flZ, snoObserver, snoRenderTarget, flAspectRatio, fClearZBuffer);

}




void SpriteDrawFullscreenRActorSprite(
    int32 nCameraIndex,
    DisplayList * pDisplayList,
    const RActorID idRActor,
    float flDeltaTime,
    const float flZ,
    const SNO snoRenderTarget,
    const RenderLayer eRenderLayer)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawFullscreenRActorSprite(pSpriteThreadBuffer, nCameraIndex, pDisplayList, idRActor, flDeltaTime, flZ, snoRenderTarget, eRenderLayer);
        return;
    }

    pDisplayList->RenderBegin();
    const Camera & cam = CameraManagerGetCamera(nCameraIndex);
    RActorGfxRenderRActorInDisplayList(cam, *pDisplayList, NULL, NULL, idRActor, FALSE, flDeltaTime, FALSE);
    pDisplayList->Sort();

    sSpriteSubmitFullscreenRActorSprite(*pDisplayList, idRActor, flZ, snoRenderTarget, eRenderLayer);
}




void SpriteDrawUISprite(UISprite * ptSprite, const float flZ, const RenderLayer eRenderLayer)
{
    XCommandBuffer * pSpriteThreadBuffer = ThreadGlobals::Get().m_pSpriteThreadBuffer;
    if (pSpriteThreadBuffer)
    {
        sBufferSpriteDrawUISprite(pSpriteThreadBuffer, ptSprite, flZ, eRenderLayer);
        return;
    }

    SpriteBucket * ptSpriteBucket = sFindComplexSpriteBucket(eRenderLayer, SPRITE_UI, flZ);
    ptSpriteBucket->ptSpritePtrs->AddTail_Tracked(ptSprite);
}








UISpritePoolInterface::UISpritePoolInterface()
{
    m_ptNext = s_ptUISpritePoolsHead;
    m_ptPrev = NULL;
    s_ptUISpritePoolsHead = this;
    if (m_ptNext)
        m_ptNext->m_ptPrev = this;
}








UISpritePoolInterface::~UISpritePoolInterface()
{
    if (m_ptNext)
        m_ptNext->m_ptPrev = m_ptPrev;
    if (m_ptPrev)
        m_ptPrev->m_ptNext = m_ptNext;
    else
        s_ptUISpritePoolsHead = m_ptNext;
}





































static void sSpriteDrawDeferredData0Func(const void * pDataIn)
{
    const SpriteDrawDeferredData0 * pData = (const SpriteDrawDeferredData0 *)pDataIn;
    SpriteDrawDeferred(pData->tImageTexFrame, pData->snoShader, pData->pCoords, pData->flScaleX, pData->flScaleY,
                       pData->rgbaColor, pData->rgbaAuxColor, pData->flZ, pData->bHasClipping ? &pData->wrClipping : NULL,
                       pData->eRenderLayerOverride, pData->fInRenderDraw);
}


static void sBufferSpriteDrawDeferred(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw)












{
    SpriteDrawDeferredData0 * pData = (SpriteDrawDeferredData0 *)pBuffer->Reserve(sSpriteDrawDeferredData0Func, sizeof(SpriteDrawDeferredData0));

    pData->tImageTexFrame = tImageTexFrame;
    pData->snoShader = snoShader;
    pData->pCoords = pCoords;
    pData->flScaleX = flScaleX;
    pData->flScaleY = flScaleY;
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    if (pwrClipping)
    {
        pData->bHasClipping = TRUE;
        pData->wrClipping = *pwrClipping;
    }
    else
    {
        pData->bHasClipping = FALSE;
    }
    pData->eRenderLayerOverride = eRenderLayerOverride;
    pData->fInRenderDraw = fInRenderDraw;
}

struct SpriteDrawDeferredData1 {  // size 0x4c
    SNO snoTexture;  // 0x00
    float flTexWidth;  // 0x04
    float flTexHeight;  // 0x08
    uint32 dwFrameIndex;  // 0x0c
    SNO snoShader;  // 0x10
    Vector2D pCoords;  // 0x14
    float flScaleX;  // 0x1c
    float flScaleY;  // 0x20
    RGBAColor rgbaColor;  // 0x24
    RGBAColor rgbaAuxColor;  // 0x28
    float flZ;  // 0x2c
    float flStereoDepth;  // 0x30
    Rect2D wrClipping;  // 0x34
    RenderLayer eRenderLayerOverride;  // 0x44
    uint32 fInRenderDraw : 1;  // 0x48
    uint32 bHasClipping : 1;
};


static void sSpriteDrawDeferredData1Func(const void * pDataIn)
{
    const SpriteDrawDeferredData1 * pData = (const SpriteDrawDeferredData1 *)pDataIn;
    ScopedTextureDefinitionLockR pdefTexture(pData->snoTexture);
    ScopedShaderDefinitionLockR pdefShader(pData->snoShader);
    SpriteDrawDeferred(pdefTexture, pData->flTexWidth, pData->flTexHeight, pData->dwFrameIndex, pdefShader, pData->pCoords, pData->flScaleX, pData->flScaleY,
                       pData->rgbaColor, pData->rgbaAuxColor, pData->flZ, pData->flStereoDepth,
                       pData->bHasClipping ? &pData->wrClipping : NULL, pData->eRenderLayerOverride, pData->fInRenderDraw);
}


static void sBufferSpriteDrawDeferred(XCommandBuffer * pBuffer, const TextureDefinition * pdefTexture, const float flTexWidth, const float flTexHeight, const uint32 dwFrameIndex, const ShaderDefinition * pdefShader, const Vector2D & pCoords, const float flScaleX, const float flScaleY, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const Rect2D * const pwrClipping, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw)
















{
    SpriteDrawDeferredData1 * pData = (SpriteDrawDeferredData1 *)pBuffer->Reserve(sSpriteDrawDeferredData1Func, sizeof(SpriteDrawDeferredData1));

    pData->snoTexture = pdefTexture->tHeader.snoHandle;
    pData->flTexWidth = flTexWidth;
    pData->flTexHeight = flTexHeight;
    pData->dwFrameIndex = dwFrameIndex;
    pData->snoShader = pdefShader->tHeader.snoHandle;
    pData->pCoords = pCoords;
    pData->flScaleX = flScaleX;
    pData->flScaleY = flScaleY;
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    pData->flStereoDepth = flStereoDepth;
    if (pwrClipping)
    {
        pData->wrClipping = *pwrClipping;
        pData->bHasClipping = TRUE;
    }
    else
    {
        pData->bHasClipping = FALSE;
    }
    pData->eRenderLayerOverride = eRenderLayerOverride;
    pData->fInRenderDraw = fInRenderDraw;
}

struct SpriteDrawDeferredGlyphData
{
    SNO snoTexture;
    float flTexWidth;
    float flTexHeight;
    uint32 dwFrameIndex;
    SNO snoShader;
    Vector2D pCoords;
    float flScaleX;
    float flScaleY;
    RGBAColor rgbaColor;
    RGBAColor rgbaAuxColor;
    float flZ;
    float flStereoDepth;
    Rect2D wrClipping;
    RenderLayer eRenderLayerOverride;
    uint32 fInRenderDraw : 1;
    uint32 bHasClipping : 1;
};

static void sSpriteDrawDeferredGlyphDataFunc(
    const void * pDataIn)
{ // Inference: the DWARF pointer type identifies the object returned by this void-pointer API.
    const SpriteDrawDeferredGlyphData * pData = static_cast<const SpriteDrawDeferredGlyphData *>(pDataIn);
    ScopedTextureDefinitionLockR pdefTexture(pData->snoTexture);
    ScopedShaderDefinitionLockR pdefShader(pData->snoShader);
    SpriteDrawDeferredGlyph(pdefTexture, pData->flTexWidth, pData->flTexHeight, pData->dwFrameIndex, pdefShader, pData->pCoords,
        pData->flScaleX, pData->flScaleY, pData->rgbaColor, pData->rgbaAuxColor, pData->flZ, pData->flStereoDepth,
        pData->bHasClipping ? &pData->wrClipping : NULL, pData->eRenderLayerOverride, pData->fInRenderDraw);
}
// DWARF inlined helper needed by Glyph; no standalone original body is checkable.
static void sBufferSpriteDrawDeferredGlyph(
    XCommandBuffer * pBuffer,
    const TextureDefinition * pdefTexture,
    const float flTexWidth,
    const float flTexHeight,
    const uint32 dwFrameIndex,
    const ShaderDefinition * pdefShader,
    const Vector2D & pCoords,
    const float flScaleX,
    const float flScaleY,
    const RGBAColor & rgbaColor,
    const RGBAColor & rgbaAuxColor,
    const float flZ,
    const float flStereoDepth,
    const Rect2D *const pwrClipping,
    const RenderLayer eRenderLayerOverride,
    BOOL fInRenderDraw)
{
    void *const pMemory = pBuffer->Reserve(sSpriteDrawDeferredGlyphDataFunc, sizeof(SpriteDrawDeferredGlyphData)); // Inference: DWARF typed pData views the reserved command storage, as in sibling buffers.
    SpriteDrawDeferredGlyphData *const pData = static_cast<SpriteDrawDeferredGlyphData *>(pMemory);
    pData->snoTexture = pdefTexture->tHeader.snoHandle;
    pData->flTexWidth = flTexWidth;
    pData->flTexHeight = flTexHeight;
    pData->dwFrameIndex = dwFrameIndex;
    pData->snoShader = pdefShader->tHeader.snoHandle;
    pData->pCoords = pCoords;
    pData->flScaleX = flScaleX;
    pData->flScaleY = flScaleY;
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    pData->flStereoDepth = flStereoDepth;
    if (pwrClipping)
    {
        pData->wrClipping = *pwrClipping;
        pData->bHasClipping = TRUE;
    }
    else
    {
        pData->bHasClipping = FALSE;
    }
    pData->eRenderLayerOverride = eRenderLayerOverride;
    pData->fInRenderDraw = fInRenderDraw;
}
struct SpriteDrawDeferredData2 {  // size 0x54
    ImageTextureFrame tImageTexFrame;  // 0x00
    SNO snoShader;  // 0x08
    Rect2D rectScreen;  // 0x0c
    RGBAColor rgbaColor;  // 0x1c
    RGBAColor rgbaAuxColor;  // 0x20
    float flZ;  // 0x24
    Rect2D wrClipping;  // 0x28
    SNO snoTexAlpha;  // 0x38
    Rect2D wrTexAlphaCoords;  // 0x3c
    RenderLayer eRenderLayerOverride;  // 0x4c
    uint32 fFlipV : 1;  // 0x50
    uint32 fInRenderDraw : 1;
    uint32 bHasClipping : 1;
    uint32 bHasTexAlphaCoords : 1;
};



static void sSpriteDrawDeferredData2Func(const void * pDataIn)
{
    const SpriteDrawDeferredData2 * pData = (const SpriteDrawDeferredData2 *)pDataIn;
    SpriteDrawDeferred(pData->tImageTexFrame, pData->snoShader, pData->rectScreen, pData->rgbaColor, pData->rgbaAuxColor, pData->flZ,
                       pData->bHasClipping ? &pData->wrClipping : NULL, pData->snoTexAlpha,
                       pData->bHasTexAlphaCoords ? &pData->wrTexAlphaCoords : NULL, pData->fFlipV,
                       pData->eRenderLayerOverride, pData->fInRenderDraw);
}


static void sBufferSpriteDrawDeferred(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw)
{
    SpriteDrawDeferredData2 * pData = (SpriteDrawDeferredData2 *)pBuffer->Reserve(sSpriteDrawDeferredData2Func, sizeof(SpriteDrawDeferredData2));

    pData->tImageTexFrame = tImageTexFrame;
    pData->snoShader = snoShader;
    pData->rectScreen = rectScreen;
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    if (pwrClipping)
    {
        pData->bHasClipping = TRUE;
        pData->wrClipping = *pwrClipping;
    }
    else
    {
        pData->bHasClipping = FALSE;
    }

    pData->snoTexAlpha = snoTexAlpha;
    if (pwrTexAlphaCoords)
    {
        pData->bHasTexAlphaCoords = TRUE;
        pData->wrTexAlphaCoords = *pwrTexAlphaCoords;
    }
    else
    {
        pData->bHasTexAlphaCoords = FALSE;
    }

    pData->fFlipV = fFlipV;
    pData->eRenderLayerOverride = eRenderLayerOverride;
    pData->fInRenderDraw = fInRenderDraw;
}












struct SpriteDrawDeferredRotatedData {  // size 0x58
    ImageTextureFrame tImageTexFrame;  // 0x00
    SNO snoShader;  // 0x08
    Rect2D rectScreen;  // 0x0c
    RGBAColor rgbaColor;  // 0x1c
    RGBAColor rgbaAuxColor;  // 0x20
    float flZ;  // 0x24
    Rect2D wrClipping;  // 0x28
    Angle aYaw;  // 0x38
    SNO snoTexAlpha;  // 0x3c
    Rect2D wrTexAlphaCoords;  // 0x40
    RenderLayer eRenderLayerOverride;  // 0x50
    uint32 fFlipV : 1;  // 0x54
    uint32 bHasClipping : 1;
    uint32 bHasTexAlphaCoords : 1;
};


static void sSpriteDrawDeferredRotatedDataFunc(const void * pDataIn)
{
    const SpriteDrawDeferredRotatedData * pData = (const SpriteDrawDeferredRotatedData *)pDataIn;
    SpriteDrawDeferredRotated(pData->tImageTexFrame, pData->snoShader, pData->rectScreen, pData->rgbaColor, pData->rgbaAuxColor, pData->flZ,
                              pData->bHasClipping ? &pData->wrClipping : NULL, pData->aYaw, pData->snoTexAlpha,
                              pData->bHasTexAlphaCoords ? &pData->wrTexAlphaCoords : NULL, pData->fFlipV, pData->eRenderLayerOverride);
}


static void sBufferSpriteDrawDeferredRotated(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const Angle aYaw, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride)
{
    SpriteDrawDeferredRotatedData * pData = (SpriteDrawDeferredRotatedData *)pBuffer->Reserve(sSpriteDrawDeferredRotatedDataFunc, sizeof(SpriteDrawDeferredRotatedData));

    pData->tImageTexFrame = tImageTexFrame;
    pData->snoShader = snoShader;
    pData->rectScreen = rectScreen;
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    if (pwrClipping)
    {
        pData->bHasClipping = TRUE;
        pData->wrClipping = *pwrClipping;
    }
    else
    {
        pData->bHasClipping = FALSE;
    }

    pData->aYaw = aYaw;
    pData->snoTexAlpha = snoTexAlpha;
    if (pwrTexAlphaCoords)
    {
        pData->bHasTexAlphaCoords = TRUE;
        pData->wrTexAlphaCoords = *pwrTexAlphaCoords;
    }
    else
    {
        pData->bHasTexAlphaCoords = FALSE;
    }

    pData->fFlipV = fFlipV;
    pData->eRenderLayerOverride = eRenderLayerOverride;
}












struct SpriteDrawDeferredMirroredData {  // size 0x5c
    ImageTextureFrame tImageTexFrame;  // 0x00
    SNO snoShader;  // 0x08
    Rect2D rectScreen;  // 0x0c
    RGBAColor rgbaColor;  // 0x1c
    RGBAColor rgbaAuxColor;  // 0x20
    float flZ;  // 0x24
    Rect2D wrClipping;  // 0x28
    BOOL bMirrorX;  // 0x38
    BOOL bMirrorY;  // 0x3c
    SNO snoTexAlpha;  // 0x40
    Rect2D wrTexAlphaCoords;  // 0x44
    RenderLayer eRenderLayerOverride;  // 0x54
    uint32 fFlipV : 1;  // 0x58
    uint32 bHasClipping : 1;
    uint32 bHasTexAlphaCoords : 1;
};


static void sSpriteDrawDeferredMirroredDataFunc(const void * pDataIn)
{
    const SpriteDrawDeferredMirroredData * pData = (const SpriteDrawDeferredMirroredData *)pDataIn;
    SpriteDrawDeferredMirrored(pData->tImageTexFrame, pData->snoShader, pData->rectScreen, pData->rgbaColor, pData->rgbaAuxColor, pData->flZ,
                              pData->bHasClipping ? &pData->wrClipping : NULL, pData->bMirrorX, pData->bMirrorY, pData->snoTexAlpha,
                              pData->bHasTexAlphaCoords ? &pData->wrTexAlphaCoords : NULL, pData->fFlipV, pData->eRenderLayerOverride);
}


static void sBufferSpriteDrawDeferredMirrored(XCommandBuffer * pBuffer, const ImageTextureFrame & tImageTexFrame, const SNO snoShader, const Rect2D & rectScreen, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const Rect2D * const pwrClipping, const BOOL bMirrorX, const BOOL bMirrorY, const SNO snoTexAlpha, const Rect2D * pwrTexAlphaCoords, const BOOL fFlipV, const RenderLayer eRenderLayerOverride)
{
    SpriteDrawDeferredMirroredData * pData = (SpriteDrawDeferredMirroredData *)pBuffer->Reserve(sSpriteDrawDeferredMirroredDataFunc, sizeof(SpriteDrawDeferredMirroredData));

    pData->tImageTexFrame = tImageTexFrame;
    pData->snoShader = snoShader;
    pData->rectScreen = rectScreen;
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    if (pwrClipping)
    {
        pData->bHasClipping = TRUE;
        pData->wrClipping = *pwrClipping;
    }
    else
    {
        pData->bHasClipping = FALSE;
    }

    pData->bMirrorX = bMirrorX;
    pData->bMirrorY = bMirrorY;
    pData->snoTexAlpha = snoTexAlpha;
    if (pwrTexAlphaCoords)
    {
        pData->bHasTexAlphaCoords = TRUE;
        pData->wrTexAlphaCoords = *pwrTexAlphaCoords;
    }
    else
    {
        pData->bHasTexAlphaCoords = FALSE;
    }

    pData->fFlipV = fFlipV;
    pData->eRenderLayerOverride = eRenderLayerOverride;
}



























static void sSpriteDrawDeferredMinimapTileDataFunc(const void * pDataIn)
{
    const SpriteDrawDeferredMinimapTileData * pData = (const SpriteDrawDeferredMinimapTileData *)pDataIn;
    SpriteDrawDeferredMinimapTile(pData->snoMapTex, pData->snoShader, pData->wpCorners, pData->rgbaColor, pData->rgbaAuxColor,
                                  pData->flZ, pData->snoTexMask, pData->wpMaskCorners, pData->snoTexExploredMask, pData->wpExploredMaskCorners);
}


static void sBufferSpriteDrawDeferredMinimapTile(XCommandBuffer * pBuffer, const SNO snoMapTex, const SNO snoShader, const Vector2D * wpCorners, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const SNO snoTexMask, const Vector2D * wpMaskCorners, const SNO snoTexExploredMask, const Vector2D * wpExploredMaskCorners)











{
    SpriteDrawDeferredMinimapTileData * pData = (SpriteDrawDeferredMinimapTileData *)pBuffer->Reserve(sSpriteDrawDeferredMinimapTileDataFunc, sizeof(SpriteDrawDeferredMinimapTileData));

    pData->snoMapTex = snoMapTex;
    pData->snoShader = snoShader;
    SigmaMemoryCopy(pData->wpCorners, wpCorners, sizeof(pData->wpCorners));
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    pData->snoTexMask = snoTexMask;
    SigmaMemoryCopy(pData->wpMaskCorners, wpMaskCorners, sizeof(pData->wpMaskCorners));
    pData->snoTexExploredMask = snoTexExploredMask;
    SigmaMemoryCopy(pData->wpExploredMaskCorners, wpExploredMaskCorners, sizeof(pData->wpExploredMaskCorners));
}

struct SpriteDrawDeferredData3
{
    ImageTextureFrame tImageTexFrame;
    ImageTextureFrame tImageTexFrameMask;
    SNO snoShader;
    Rect2D rectScreen;
    RGBAColor rgbaColor;
    RGBAColor rgbaAuxColor;
    float flZ;
    float flStereoDepth;
    Rect2D wrClipping;
    RenderLayer eRenderLayerOverride;
    uint32 fInRenderDraw : 1;
    uint32 bHasClipping : 1;
};

static void sSpriteDrawDeferredData3Func(
    const void * pDataIn)
{ // Inference: the DWARF pointer type identifies the object returned by this void-pointer API.
    const SpriteDrawDeferredData3 * pData = static_cast<const SpriteDrawDeferredData3 *>(pDataIn);
    SpriteDrawDeferred(pData->tImageTexFrame, pData->tImageTexFrameMask, pData->snoShader, pData->rectScreen,
        pData->rgbaColor, pData->rgbaAuxColor, pData->flZ, pData->flStereoDepth, pData->bHasClipping ? &pData->wrClipping : NULL,
        pData->eRenderLayerOverride, pData->fInRenderDraw);
}
// DWARF inlined mask helper needed by its caller; no standalone original body is checkable.
static void sBufferSpriteDrawDeferred(
    XCommandBuffer * pBuffer,
    const ImageTextureFrame & tImageTexFrame,
    const ImageTextureFrame & tImageTexFrameMask,
    const SNO snoShader,
    const Rect2D & rectScreen,
    const RGBAColor & rgbaColor,
    const RGBAColor & rgbaAuxColor,
    const float flZ,
    const float flStereoDepth,
    const Rect2D *const pwrClipping,
    const RenderLayer eRenderLayerOverride,
    BOOL fInRenderDraw)
{
    void *const pMemory = pBuffer->Reserve(sSpriteDrawDeferredData3Func, sizeof(SpriteDrawDeferredData3)); // Inference: DWARF typed pData views the reserved command storage, as in sibling buffers.
    SpriteDrawDeferredData3 *const pData = static_cast<SpriteDrawDeferredData3 *>(pMemory);
    pData->tImageTexFrame = tImageTexFrame;
    pData->tImageTexFrameMask = tImageTexFrameMask;
    pData->snoShader = snoShader;
    pData->rectScreen = rectScreen;
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    pData->flStereoDepth = flStereoDepth;
    if (pwrClipping)
    {
        pData->bHasClipping = TRUE;
        pData->wrClipping = *pwrClipping;
    }
    else
    {
        pData->bHasClipping = FALSE;
    }
    pData->eRenderLayerOverride = eRenderLayerOverride;
    pData->fInRenderDraw = fInRenderDraw;
}














static void sSpriteDrawSimpleDataFunc(const void * pDataIn)
{
    const SpriteDrawSimpleData * pData = (const SpriteDrawSimpleData *)pDataIn;

    SpriteDrawSimple(pData->snoTexture, pData->snoShader, pData->wrScreen, pData->wpUVs, pData->rgbaColor,
                     pData->rgbaAuxColor, pData->flZ, pData->flStereoDepth, pData->eRenderLayerOverride);
}

static void sBufferSpriteDrawSimple(XCommandBuffer * pBuffer, const SNO snoTexture, const SNO snoShader, const Rect2D & wrScreen, const Vector2D * wpUVs, const RGBAColor & rgbaColor, const RGBAColor & rgbaAuxColor, const float flZ, const float flStereoDepth, const RenderLayer eRenderLayerOverride)










{
    SpriteDrawSimpleData * pData = (SpriteDrawSimpleData *)pBuffer->Reserve(sSpriteDrawSimpleDataFunc, sizeof(SpriteDrawSimpleData));

    pData->snoTexture = snoTexture;
    pData->snoShader = snoShader;
    pData->wrScreen = wrScreen;
    SigmaMemoryCopy(pData->wpUVs, wpUVs, sizeof(pData->wpUVs));
    pData->rgbaColor = rgbaColor;
    pData->rgbaAuxColor = rgbaAuxColor;
    pData->flZ = flZ;
    pData->flStereoDepth = flStereoDepth;
    pData->eRenderLayerOverride = eRenderLayerOverride;
}

struct SpriteDrawRActorSpriteData {
    int32 nCameraIndex;
    DisplayList * pDisplayList;
    RActorID idRActor;
    Rect2D wrBounds;
    Rect2D wrViewBounds;
    float flDeltaTime;
    float flZ;
    SNO snoObserver;
    SNO snoRenderTarget;
    float flAspectRatio;
    BOOL fClearZBuffer;
};


static void sSpriteDrawRActorSpriteDataFunc(
    const void * pDataIn)
{
    const SpriteDrawRActorSpriteData * pData = (const SpriteDrawRActorSpriteData *)pDataIn;
    SpriteDrawRActorSprite(pData->nCameraIndex, pData->pDisplayList, pData->idRActor, pData->wrBounds, pData->wrViewBounds,
        pData->flDeltaTime, pData->flZ, pData->snoObserver, pData->snoRenderTarget, pData->flAspectRatio, pData->fClearZBuffer);
}

static void sBufferSpriteDrawRActorSprite(
    XCommandBuffer * pBuffer,
    int32 nCameraIndex,
    DisplayList * pDisplayList,
    const RActorID idRActor,
    const Rect2D & wrBounds,
    const Rect2D & wrViewBounds,
    float flDeltaTime,
    const float flZ,
    const SNO snoObserver,
    const SNO snoRenderTarget,
    const float flAspectRatio,
    const BOOL fClearZBuffer)
{
    void * const pMemory = pBuffer->Reserve(sSpriteDrawRActorSpriteDataFunc, sizeof(SpriteDrawRActorSpriteData));
    SpriteDrawRActorSpriteData * const pData = (SpriteDrawRActorSpriteData *)pMemory;
    pData->nCameraIndex = nCameraIndex;
    pData->pDisplayList = pDisplayList;
    pData->idRActor = idRActor;
    pData->wrBounds = wrBounds;
    pData->wrViewBounds = wrViewBounds;
    pData->flDeltaTime = flDeltaTime;
    pData->flZ = flZ;
    pData->snoObserver = snoObserver;
    pData->snoRenderTarget = snoRenderTarget;
    pData->flAspectRatio = flAspectRatio;
    pData->fClearZBuffer = fClearZBuffer;
}

struct SpriteDrawFullscreenRActorSpriteData {
    int32 nCameraIndex;
    DisplayList * pDisplayList;
    RActorID idRActor;
    float flDeltaTime;
    float flZ;
    SNO snoRenderTarget;
    RenderLayer eRenderLayer;
};


static void sSpriteDrawFullscreenRActorSpriteDataFunc(
    const void * pDataIn)
{
    const SpriteDrawFullscreenRActorSpriteData * pData = (const SpriteDrawFullscreenRActorSpriteData *)pDataIn;
    SpriteDrawFullscreenRActorSprite(pData->nCameraIndex, pData->pDisplayList, pData->idRActor, pData->flDeltaTime, pData->flZ,
        pData->snoRenderTarget, pData->eRenderLayer);
}

static void sBufferSpriteDrawFullscreenRActorSprite(
    XCommandBuffer * pBuffer,
    int32 nCameraIndex,
    DisplayList * pDisplayList,
    const RActorID idRActor,
    float flDeltaTime,
    const float flZ,
    const SNO snoRenderTarget,
    const RenderLayer eRenderLayer)
{
    void * const pMemory = pBuffer->Reserve(sSpriteDrawFullscreenRActorSpriteDataFunc, sizeof(SpriteDrawFullscreenRActorSpriteData));
    SpriteDrawFullscreenRActorSpriteData * const pData = (SpriteDrawFullscreenRActorSpriteData *)pMemory;
    pData->nCameraIndex = nCameraIndex;
    pData->pDisplayList = pDisplayList;
    pData->idRActor = idRActor;
    pData->flDeltaTime = flDeltaTime;
    pData->flZ = flZ;
    pData->snoRenderTarget = snoRenderTarget;
    pData->eRenderLayer = eRenderLayer;
}








static void sSpriteDrawUISpriteDataFunc(const void * pDataIn)
{
    const SpriteDrawUISpriteData * pData = (const SpriteDrawUISpriteData *)pDataIn;
    SpriteDrawUISprite(pData->pSprite, pData->flZ, pData->eRenderLayer);
}


static void sBufferSpriteDrawUISprite(XCommandBuffer * pBuffer, UISprite * ptSprite, const float flZ, const RenderLayer eRenderLayer)
{
    SpriteDrawUISpriteData * pData = (SpriteDrawUISpriteData *)pBuffer->Reserve(sSpriteDrawUISpriteDataFunc, sizeof(SpriteDrawUISpriteData));
    pData->pSprite = ptSprite;
    pData->flZ = flZ;
    pData->eRenderLayer = eRenderLayer;
}
