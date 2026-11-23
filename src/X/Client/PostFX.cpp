// X/Client/PostFX.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r6-render/postfx/frag.cpp by build/try/r6-render/gen.py.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "PostFX.h"
#include "CGame.h"
#include "CWorld.h"
#include "RWindow.h"
#include "Sprite.h"
#include "Weather.h"
#include "Gfx/Gfx.h"
#include "Gfx/GfxAPI.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxIB.h"
#include "Gfx/GfxVB.h"
#include "Definitions/ShaderDefinition.h"
#include "Definitions/TextureDefinition.h"
#include "../Common/Game.h"
#include "../Common/World.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "Definitions/WeatherDefinition.h"
#include "../Common/GlobalSNO.h"
#include "../Common/ThreadGlobals.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Matrix.h"
#include "../Sigma/Math/Plane.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RGBAColor.h"

// X/Client/Gfx/Gfx.h and GfxShader.h are not in src/ with these yet; declared here.
void GfxWorldMatrixSet(const Matrix4x4 & m);
const Matrix4x4 & GfxViewMatrixGet();
const Matrix4x4 & GfxProjectionMatrixGet();
void GfxViewportSet(const IRect2D & rectViewport);
void GfxViewportScaleBiasSet(const Vector4D & vScaleBias);
void GfxRenderClear(BOOL fClearTarget, BOOL fClearZBuffer, BOOL fClearStencilBuffer, const RGBAColor & rgbaTargetColor);
void GfxSetRenderTarget(const SNO sno, const CubeMapFace eCubeMapFace);
void GfxSetRenderAndDepthTarget(const SNO snoRenderTarget, const SNO snoDepthTarget, const CubeMapFace eCubeMapFace);
void GfxGetRenderTargets(RenderTarget * ptRenderTargets);
void GfxSetRenderTargets(const RenderTarget * ptRenderTargets);
void GfxSetDepthTarget(const SNO sno);
SNO GfxGetDepthTarget();
void GfxGetRenderTargetFloatDimensions(float & flWidth, float & flHeight);
void GfxGetRenderTargetInvFloatDimensions(float & flInvWidth, float & flInvHeight);
BOOL GfxGetMSAAEnabled();
void GfxShaderInit(const SNO sno, const int32 nPassIndex);
void GfxShaderDraw(const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount, const PerfBin ePerfBin);
TextureBinding * GfxGetTextureBinding(const ShaderTex eShaderTex);
void GfxShaderSetTexture(const TextureBinding * ptTexBinding, const SNO sno);
void GfxShaderSetStream(const StreamIndex eStreamIndex, const uint32 dwStreamOffset, const VBID vbid);
void GfxShaderSetIB(const IBID ibid, const uint32 dwIndexOffset);
const ShaderConstBinding * GfxShaderGetConstantBinding(const int32 nIndex);
GfxAPI GfxGetSystemID();
uint32 GfxGetMSAALevel();
void GfxSetShaderGPRAllocation(const uint32 dwVertexShaderCount, const uint32 dwPixelShaderCount);
void GfxRestoreShaderGPRAllocation();
void GfxCopyDepthTarget(const SNO snoTex);
void GfxShaderSetConstant(const ShaderConstBinding * ptBinding_, const void * lpData, const uint32 dwNumFloat4);
void GfxShaderSetConstant(const ShaderConstFastBinding * ptBinding, const void * lpData, const uint32 dwNumFloat4);
const ShaderConstFastBinding * GfxShaderGetConstantFastBindings();
#include "Gfx/GfxShader.h"


enum BlurFilterType { BLUR_FILTER_NONE = 0, BLUR_FILTER_HORZ_LEFT = 1, BLUR_FILTER_HORZ_RIGHT = 2, BLUR_FILTER_VERT_UP = 3, BLUR_FILTER_VERT_DOWN = 4, BLUR_FILTER_HORZ = 5, BLUR_FILTER_VERT = 6, BLUR_FILTER_CROSS = 7, NUM_BLUR_FILTERS = 8 };

enum { RIPPLE_GRID_WIDTH = 20, RIPPLE_GRID_HEIGHT = 15, RIPPLE_GRID_VERT_COUNT = 300 };

enum PostEffect { POSTEFFECT_DISTORTION = 0, POSTEFFECT_BLOOM = 1, POSTEFFECT_RETRO = 2, POSTEFFECT_TONEMAP = 3, POSTEFFECT_BLOOM_AND_TONEMAP = 4, NUM_POSTEFFECTS = 5 };

enum { SSAO_RANDOM_TEXTURE_WIDTH = 128, SSAO_RANDOM_TEXTURE_HEIGHT = 128, RETRO_DITHER_TEXTURE_WIDTH = 32, RETRO_DITHER_TEXTURE_HEIGHT = 1, GAMMA_TEXTURE_WIDTH = 1, GAMMA_TEXTURE_HEIGHT = 1024 };

enum { POSTFX_ENABLE_HDR_BIT = 0, POSTFX_ENABLE_COLOR_CORRECTION_BIT = 1 };

struct RenderData {  // size 0x2c
    uint32 dwFrameFlags;  // 0x00
    int32 nNumPlanes;  // 0x04
    Plane wplaneReflect[2];  // 0x08
    BOOL bReflectDroppedPlane;  // 0x28
};

struct PostFXGlobals {  // size 0x470
    uint32 dwFlags;  // 0x00
    RenderData tRenderData[2];  // 0x04
    VBID vbid;  // 0x5c
    IBID ibid;  // 0x60
    int32 nIndexCount;  // 0x64
    int32 nVertCount;  // 0x68
    VBID vbidRipple;  // 0x6c
    IBID ibidRipple;  // 0x70
    int32 nRippleGridIndexCount;  // 0x74
    BlurFilter tFilter[NUM_BLUR_FILTERS];  // 0x78
    float flFilterCoeff[4];  // 0x378
    PostFXFadeParams tFadeParams;  // 0x388
    float flFadeTimeLeft;  // 0x3a8
    int32 nNumEffects;  // 0x3ac
    PostEffect eFXStack[NUM_POSTEFFECTS];  // 0x3b0
    float flMotionBlurBlendFrac;  // 0x3c4
    SNO snoSSAORandomTexture;  // 0x3c8
    float flSSAOOcclusionPower;  // 0x3cc
    float flSSAOOcclusionRadius;  // 0x3d0
    float flSSAONoOcclusionDist;  // 0x3d4
    float flSSAOBias;  // 0x3d8
    Matrix4x4 matSSAOViewProj;  // 0x3dc
    Matrix4x4 matSSAOViewProjTranspose;  // 0x41c
    SNO snoRetroDitherTexture;  // 0x45c
    float flCurrentGammaValue;  // 0x460
    SNO snoGammaTexture;  // 0x464
    TextureFormat eGammaTextureFormat;  // 0x468
};
static_assert(sizeof(PostFXGlobals) == 0x470, "PostFXGlobals");
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Definitions/PreferencesDefinition.h"
// X/Client/Gfx/GfxIB.cpp:278 and GfxVB.cpp:437; their headers do not declare them yet.
void GfxPatchIBAlloc(const ResourceUsage eUsage, const int32 nHeight, const int32 nWidth, const int32 nLODFactor, const BOOL fPurgeIfNecessary, int32 & nIndexCount, IBID & ibid, const BOOL fReverseCulling);
UByte4 Vector2DTexCoordToUByte4(const Vector2D & uv);
void GfxViewportScaleBiasGet(Vector4D & vScaleBias);  // X/Client/Gfx/Gfx.cpp; not declared in src's Gfx.h yet
// X/Client/Definitions/TextureDefinition.cpp:389 and :404; TextureDefinition.h does not declare them yet.
void * TextureDefinitionLockTexture(const SNO sno, uint32 & dwPitch, uint32 & dwHeight, const uint32 dwMipLevel, const uint32 dwFlags);
void TextureDefinitionUnlockTexture(const SNO sno, void * lpBuf, const uint32 dwMipLevel);












void PostFXInitializeForGame()
{
    PostFXGlobals *& ptFXGlobals = CGameGet().ptPostFXGlobals;
    ptFXGlobals = (PostFXGlobals *)SigmaMemoryNew(sizeof(PostFXGlobals), 4, NULL, TRUE);

    for (int32 i = 0; i < 2; i++)
    {
        for (int nPlane = 0; nPlane < 2; nPlane++)
        {
            ptFXGlobals->tRenderData[i].wplaneReflect[nPlane] = cgwplaneNullZPlane;
        }
    }
}





void PostFXDisposeForGame()
{
    PostFXGlobals *& ptFXGlobals = CGameGet().ptPostFXGlobals;
    SigmaMemoryFree(ptFXGlobals, NULL);
    ptFXGlobals = NULL;
}

// Invented macro: each name is the enumerator's text with " Effect RT" appended, returned
// past the "EFFECTBUFFER_" prefix.
#define POSTFX_RT_NAME(e) case e: return #e " Effect RT" + (sizeof("EFFECTBUFFER_") - 1);

LPCSTR PostFXGetEffectRTDebugName(int32 nType)
{
    switch (nType)
    {

        POSTFX_RT_NAME(EFFECTBUFFER_SCREEN_PING)
        POSTFX_RT_NAME(EFFECTBUFFER_SCREEN_PONG)
        POSTFX_RT_NAME(EFFECTBUFFER_BLUR_DISTORTION)
        POSTFX_RT_NAME(EFFECTBUFFER_SCREEN_HALF_PING)
        POSTFX_RT_NAME(EFFECTBUFFER_REFLECTION_0)
        POSTFX_RT_NAME(EFFECTBUFFER_REFLECTION_1)
        POSTFX_RT_NAME(EFFECTBUFFER_REFLECTION_DEPTH)
        POSTFX_RT_NAME(EFFECTBUFFER_REFRACTION)
        POSTFX_RT_NAME(EFFECTBUFFER_HDR)
        POSTFX_RT_NAME(EFFECTBUFFER_BACKBUFFER)
        POSTFX_RT_NAME(EFFECTBUFFER_DEPTH)
        POSTFX_RT_NAME(EFFECTBUFFER_DEPTH_PREV)
        POSTFX_RT_NAME(EFFECTBUFFER_BLOOM_PONG)
        POSTFX_RT_NAME(EFFECTBUFFER_SCREEN_PONG_X360_MINUS1)
        POSTFX_RT_NAME(EFFECTBUFFER_SCREEN_PONG_X360_MINUS2)
        POSTFX_RT_NAME(EFFECTBUFFER_COLOR)
        POSTFX_RT_NAME(EFFECTBUFFER_BACKBUFFER_DEPTH)
        POSTFX_RT_NAME(EFFECTBUFFER_HIGHLIGHT)
        POSTFX_RT_NAME(EFFECTBUFFER_MSAA_COLOR)
        POSTFX_RT_NAME(EFFECTBUFFER_MSAA_DEPTH)
        POSTFX_RT_NAME(EFFECTBUFFER_SCREEN_PING_LDR)
        POSTFX_RT_NAME(EFFECTBUFFER_SCREEN_PONG_LDR)
        POSTFX_RT_NAME(EFFECTBUFFER_DEFAULT_SOFTPARTICLE_DEPTH)
        POSTFX_RT_NAME(EFFECTBUFFER_SSAO_NORMAL)
        POSTFX_RT_NAME(EFFECTBUFFER_SSAO)
        POSTFX_RT_NAME(EFFECTBUFFER_SSAO_PING)
        POSTFX_RT_NAME(EFFECTBUFFER_SSAO_PONG)
    }









    return "Unknown Effect RT";
}



void sPostFXBlurGaussian5x5(const IRect2D & rectViewport, const Vector4D & vScaleAndBias, SNO snoSrc, SNO snoIntermediate, SNO snoDst, SNO snoHorizontalShader, SNO snoVerticalShader, BOOL fClear, int32 nResolveExp)
{
    const float flBlurKernelX[5] = { 1.0f / 16.0f, 4.0f / 16.0f, 6.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f };
    const float flBlurKernelY[5] = { 1.0f / 16.0f, 4.0f / 16.0f, 6.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f };

    {
        float flBlurWeights[4];
        float flRemainingWeight = 1.0f - flBlurKernelX[2];
        float flWeight01 = flBlurKernelX[0] + flBlurKernelX[1];
        float flWeight34 = flBlurKernelX[3] + flBlurKernelX[4];
        BlurFilter tFilter;

        flBlurWeights[0] = flWeight01 / flRemainingWeight;
        flBlurWeights[1] = flBlurKernelX[2];
        flBlurWeights[2] = flWeight34 / flRemainingWeight;
        flBlurWeights[3] = 0.0f;




















        tFilter.uvSample[0].flU = -(1.0f + flBlurKernelX[0] / flWeight01);
        tFilter.uvSample[0].flV = 0.0f;
        tFilter.uvSample[1].flU = 0.0f;
        tFilter.uvSample[1].flV = 0.0f;
        tFilter.uvSample[2].flU = 1.0f + flBlurKernelX[4] / flWeight34;
        tFilter.uvSample[2].flV = 0.0f;
        ShaderFloatSet(tFilter.tWeight[0], flBlurWeights[0], flBlurWeights[1], flBlurWeights[2], flBlurWeights[3]);

        GfxSetRenderTarget(snoIntermediate, CUBEMAPFACE_POSX);
        if (fClear)
        {
            GfxRenderClear(TRUE, FALSE, FALSE, crgbaBlack);
        }
        GfxViewportSet(rectViewport);
        GfxViewportScaleBiasSet(vScaleAndBias);
        PostFXFilterRender(snoHorizontalShader, snoSrc, tFilter, FALSE);
    }

    {
        float flBlurWeights[4];
        float flRemainingWeight = 1.0f - flBlurKernelY[2];
        float flWeight01 = flBlurKernelY[0] + flBlurKernelY[1];
        float flWeight34 = flBlurKernelY[3] + flBlurKernelY[4];
        BlurFilter tFilter;

        flBlurWeights[0] = flWeight01 / flRemainingWeight;
        flBlurWeights[1] = flBlurKernelY[2];
        flBlurWeights[2] = flWeight34 / flRemainingWeight;
        flBlurWeights[3] = 0.0f;




        tFilter.uvSample[0].flU = 0.0f;
        tFilter.uvSample[0].flV = -(1.0f + flBlurKernelY[0] / flWeight01);
        tFilter.uvSample[1].flU = 0.0f;
        tFilter.uvSample[1].flV = 0.0f;
        tFilter.uvSample[2].flU = 0.0f;
        tFilter.uvSample[2].flV = 1.0f + flBlurKernelY[4] / flWeight34;
        ShaderFloatSet(tFilter.tWeight[0], flBlurWeights[0], flBlurWeights[1], flBlurWeights[2], flBlurWeights[3]);

        GfxSetRenderTarget(snoDst, CUBEMAPFACE_POSX);
        if (fClear)
        {
            GfxRenderClear(TRUE, FALSE, FALSE, crgbaBlack);
        }
        GfxViewportSet(rectViewport);
        GfxViewportScaleBiasSet(vScaleAndBias);
        PostFXFilterRender(snoVerticalShader, snoIntermediate, tFilter, FALSE);
    }
}









































































































Matrix4x4 PostFXGetSSAOViewProjTranspose()
{
    CGame & tClientGame = CGameGet();
    PostFXGlobals * ptFXGlobals = tClientGame.ptPostFXGlobals;
    return ptFXGlobals->matSSAOViewProjTranspose;
}











static void sPostFXGfxSSAOInit()

{
    CGame & tClientGame = CGameGet();
    PostFXGlobals * ptFXGlobals = tClientGame.ptPostFXGlobals;

    {
        uint32 dwWidth = SSAO_RANDOM_TEXTURE_WIDTH;
        uint32 dwHeight = SSAO_RANDOM_TEXTURE_HEIGHT;
        TextureFormat eSupportedFormat;
        ptFXGlobals->flSSAOOcclusionPower = 1.5f;
        ptFXGlobals->flSSAOOcclusionRadius = 0.6f;
        ptFXGlobals->flSSAONoOcclusionDist = 6.0f;
        ptFXGlobals->flSSAOBias = 0.0125f;




        TextureFormat eTexFormat = TEXTUREFORMAT_LIN_RGBA8888;


        ptFXGlobals->snoSSAORandomTexture = TextureDefinitionCreateTexture("SSAO Random", eTexFormat, eSupportedFormat, dwWidth, dwHeight, RESOURCEUSAGE_STATIC, 0, 1, 0, 0);

        uint32 dwPitch;
        uint8 * pBuffer = (uint8 *)TextureDefinitionLockTexture(ptFXGlobals->snoSSAORandomTexture, dwPitch, dwHeight, 0, 0);
        uint8 * pRow = pBuffer;
        for (uint32 j = 0; j < dwHeight; j++)
        {
            Pixel32 * pDest = (Pixel32 *)pRow;
            for (uint32 i = 0; i < dwWidth; i++)
            {
                Angle flPhi = (float)rand() * (FULL_CIRCLE / (float)RAND_MAX);
                float flZ = (float)rand() * (2.0f / (float)RAND_MAX) - 1.0f;
                Angle flTheta = Math::ASin(flZ);
                float flX = Math::Cos(flTheta) * Math::Cos(flPhi);
                float flY = Math::Cos(flTheta) * Math::Sin(flPhi);
                RGBAColor rgbaNormal;
                rgbaNormal.r = (uint8)((flX * 0.5f + 0.5f) * 255.0f);
                rgbaNormal.g = (uint8)((flY * 0.5f + 0.5f) * 255.0f);
                rgbaNormal.b = (uint8)((flZ * 0.5f + 0.5f) * 255.0f);
                rgbaNormal.a = 0;
                *pDest++ = rgbaNormal.ToTexturePixel(eSupportedFormat);
            }

            pRow += dwPitch;
        }
        TextureDefinitionUnlockTexture(ptFXGlobals->snoSSAORandomTexture, pBuffer, 0);
    }

    BOOL fEnableSSAO = (Prefs::GetVideoPrefs().GetUseSSAO() && (GfxGetSystemID() == GFX_D3D || GfxGetSystemID() == GFX_D3D11)) || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D;

    AppDrawFlagSet(APP_DRAW_DEPTH_PREPASS_BIT, fEnableSSAO);
    AppDrawFlagSet(APP_DRAW_SSAO_BIT, fEnableSSAO);
    AppDrawFlagSet(APP_DRAW_SSAO_BLUR_BIT, fEnableSSAO);
}




static void sPostFXGfxRetroInit()

{
    CGame & tClientGame = CGameGet();
    PostFXGlobals * ptFXGlobals = tClientGame.ptPostFXGlobals;

    {
        uint32 dwWidth = RETRO_DITHER_TEXTURE_WIDTH;
        uint32 dwHeight = RETRO_DITHER_TEXTURE_HEIGHT;
        TextureFormat eSupportedFormat;




        TextureFormat eTexFormat = TEXTUREFORMAT_LIN_RGBA8888;


        static const float flDither[] =
        {
            19.0f, 16.0f, 4.0f, 1.0f,
            22.0f, 25.0f, 10.0f, 7.0f,
            13.0f, 17.0f, 6.0f, 20.0f,
            23.0f, 14.0f, 3.0f, 8.0f,
            26.0f, 12.0f, 5.0f, 11.0f,
            9.0f, 15.0f, 27.0f, 24.0f,
            21.0f, 2.0f, 18.0f,
        };



        ptFXGlobals->snoRetroDitherTexture = TextureDefinitionCreateTexture("Retro Dither", eTexFormat, eSupportedFormat, dwWidth, dwHeight, RESOURCEUSAGE_STATIC, 0, 1, 0, 0);

        uint32 dwPitch;
        uint8 * pBuffer = (uint8 *)TextureDefinitionLockTexture(ptFXGlobals->snoRetroDitherTexture, dwPitch, dwHeight, 0, 0);
        uint8 * pRow = pBuffer;
        for (uint32 j = 0; j < dwHeight; j++)
        {
            Pixel32 * pDest = (Pixel32 *)pRow;
            for (uint32 i = 0; i < dwWidth; i++)
            {
                RGBAColor rgbaColor;
                if (i < sizeof(flDither) / sizeof(flDither[0]))
                {
                    rgbaColor.r = (uint8)flDither[i];
                    rgbaColor.g = rgbaColor.b = rgbaColor.a = rgbaColor.r;
                }
                else
                {


                    rgbaColor = crgbaBlack;
                }

                pDest[i] = rgbaColor.ToTexturePixel(eSupportedFormat);
            }

            pRow += dwPitch;
        }
        TextureDefinitionUnlockTexture(ptFXGlobals->snoRetroDitherTexture, pBuffer, 0);
    }
}




void sPostFXGammaUpdateTexture(float flGamma)
{

    CGame & tClientGame = CGameGet();
    PostFXGlobals * ptFXGlobals = tClientGame.ptPostFXGlobals;

    if (ptFXGlobals->eGammaTextureFormat != TEXTUREFORMAT_R32F)
    {
        return;
    }

    uint32 dwWidth = GAMMA_TEXTURE_WIDTH;
    uint32 dwHeight = GAMMA_TEXTURE_HEIGHT;

    float flVideOutGamma = 1.0f - (flGamma - 1.0f);
    flVideOutGamma = (flVideOutGamma - 1.0f) * 0.9812f + 1.0f;

    float flGammaCurve[GAMMA_TEXTURE_HEIGHT];

    for (int i = 0; i < GAMMA_TEXTURE_HEIGHT; i++)
    {
        float flGammaVal = Math::Pow((float)i / (float)GAMMA_TEXTURE_HEIGHT, flVideOutGamma);

        flGammaCurve[i] = flGammaVal;
    }

    uint32 dwPitch;
    uint8 * pBuffer = (uint8 *)TextureDefinitionLockTexture(ptFXGlobals->snoGammaTexture, dwPitch, dwHeight, 0, 0);
    uint8 * pRow = pBuffer;
    for (uint32 j = 0; j < dwHeight; j++)
    {
        float * pDest = (float *)pRow;
        for (uint32 i = 0; i < dwWidth; i++)
        {
            pDest[i] = flGammaCurve[j];
        }

        pRow += dwPitch;
    }
    TextureDefinitionUnlockTexture(ptFXGlobals->snoGammaTexture, pBuffer, 0);
}




static void sPostFXGfxGammaInit()

{
    CGame & tClientGame = CGameGet();
    PostFXGlobals * ptFXGlobals = tClientGame.ptPostFXGlobals;

    uint32 dwWidth = GAMMA_TEXTURE_WIDTH;
    uint32 dwHeight = GAMMA_TEXTURE_HEIGHT;
    TextureFormat eSupportedFormat;

    TextureFormat eTexFormat = TEXTUREFORMAT_R32F;

    ptFXGlobals->snoGammaTexture = TextureDefinitionCreateTexture("Gamma Ramp", eTexFormat, eSupportedFormat, dwWidth, dwHeight, RESOURCEUSAGE_STATIC, 0, 1, 0, 0);
    ptFXGlobals->flCurrentGammaValue = 1.0f;
    ptFXGlobals->eGammaTextureFormat = eSupportedFormat;

    sPostFXGammaUpdateTexture(ptFXGlobals->flCurrentGammaValue);
}




void PostFXGamma()

{
    RWindow & tRWindow = RWindowMgr().GetMainRWindow();
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;


    if (ptFXGlobals->flCurrentGammaValue != Prefs::GetVideoPrefs().flGamma)
    {
        sPostFXGammaUpdateTexture(Prefs::GetVideoPrefs().flGamma);
        ptFXGlobals->flCurrentGammaValue = Prefs::GetVideoPrefs().flGamma;
    }


    if (ptFXGlobals->flCurrentGammaValue == 1.0f)
    {
        return;
    }


    if (ptFXGlobals->eGammaTextureFormat != TEXTUREFORMAT_R32F)
    {
        return;
    }


    RenderTarget tRenderTargets[2];
    GfxGetRenderTargets(tRenderTargets);
    SNO snoDepthTarget = GfxGetDepthTarget();


    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);


    GfxSetRenderTarget(tRWindow.snoEffectBuffer[EFFECTBUFFER_SCREEN_PING], CUBEMAPFACE_POSX);
    GfxSetDepthTarget(SNO_NONE);
    PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_COPY_SHADER), PostFXGetBuffer(EFFECTBUFFER_BACKBUFFER));

    {
        SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_GAMMA_CORRECTION);

        GfxSetRenderTarget(PostFXGetBuffer(EFFECTBUFFER_BACKBUFFER), CUBEMAPFACE_POSX);
        GfxSetDepthTarget(PostFXGetBuffer(EFFECTBUFFER_BACKBUFFER_DEPTH));

        float flWidth, flHeight;
        float flInvWidth, flInvHeight;
        GfxGetRenderTargetFloatDimensions(flWidth, flHeight);
        GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);


        Matrix4x4 mProj; memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));







        GfxProjectionMatrixSet(mProj);

        int32 nPassIndex = 0;
        GfxShaderInit(snoShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), tRWindow.snoEffectBuffer[EFFECTBUFFER_SCREEN_PING]);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_OVERLAY), ptFXGlobals->snoGammaTexture);

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }


    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(snoDepthTarget);


    CameraManagerRestoreCurrentCameraToMainCamera();
}



BOOL PostFXGfxInit()

{
    CGame & tClientGame = CGameGet();
    PostFXGlobals * ptFXGlobals = tClientGame.ptPostFXGlobals;



    ptFXGlobals->nVertCount = 3;




    VertexColor vc(255, 255, 255, 255);
    GfxPatchIBAlloc(RESOURCEUSAGE_IMMUTABLE, 2, 2, 1, FALSE, ptFXGlobals->nIndexCount, ptFXGlobals->ibid, FALSE);

    {
        Vertex2TexL * pv = (Vertex2TexL *)SigmaMemoryNew(ptFXGlobals->nVertCount * sizeof(Vertex2TexL), 4, NULL, FALSE);


        pv[0].wpPos.Set(-1.0f, 1.0f, 0.0f);
        pv[0].wvpNormal = cgcNullNormal;
        pv[0].vcDiffuse = vc;
        pv[0].vpUV0 = Vector2DTexCoordToUByte4(Vector2D(0.0f, 0.0f));

        pv[1].wpPos.Set(-1.0f, -3.0f, 0.0f);
        pv[1].wvpNormal = cgcNullNormal;
        pv[1].vcDiffuse = vc;
        pv[1].vpUV0 = Vector2DTexCoordToUByte4(Vector2D(0.0f, 2.0f));

        pv[2].wpPos.Set(3.0f, 1.0f, 0.0f);
        pv[2].wvpNormal = cgcNullNormal;
        pv[2].vcDiffuse = vc;
        pv[2].vpUV0 = Vector2DTexCoordToUByte4(Vector2D(2.0f, 0.0f));
























        BOOL fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_IMMUTABLE, VBFORMAT_2TEX_L, ptFXGlobals->nVertCount, FALSE, pv, ptFXGlobals->vbid, "", 0, "");


        SigmaMemoryFree(pv, NULL);
    }

    if (Prefs::GetVideoPrefs().GetUseColorCorrection()) ptFXGlobals->dwFlags |= (1 << POSTFX_ENABLE_COLOR_CORRECTION_BIT); else ptFXGlobals->dwFlags &= ~(1 << POSTFX_ENABLE_COLOR_CORRECTION_BIT);


    BOOL bSuccess = TRUE;
    int32 nNumRWindows = GetRWindowMgr()->GetRWindowCount();

    for (int32 x = 0; x < nNumRWindows; x++)
    {
        RWindow & tRWindow = GetRWindowMgr()->GetRWindow(x);
        bSuccess &= tRWindow.PostFXGfxInit();
    }

    if (!bSuccess)
    {
        return FALSE;
    }


    RWindow & tRWindow = GetRWindowMgr()->GetRWindow(0);
    GfxSetRenderTarget(tRWindow.GetDefaultColorTarget(), CUBEMAPFACE_POSX);
    GfxSetDepthTarget(tRWindow.GetDefaultDepthTarget());

    GfxPatchIBAlloc(RESOURCEUSAGE_IMMUTABLE, RIPPLE_GRID_HEIGHT, RIPPLE_GRID_WIDTH, 1, FALSE, ptFXGlobals->nRippleGridIndexCount, ptFXGlobals->ibidRipple, FALSE);
    {
        const float flUMag = 0.0025f;
        const float flVMag = 1.0f / 300.0f;
        VertexColor vcLoc(crgbaWhite);

        Vertex1TexL * pv = (Vertex1TexL *)SigmaMemoryNew(RIPPLE_GRID_VERT_COUNT * sizeof(Vertex1TexL), 4, NULL, FALSE);
        for (int32 y = 0; y < RIPPLE_GRID_HEIGHT; y++)
        {
            float flV = (float)y / (float)(RIPPLE_GRID_HEIGHT - 1);
            Angle aVPhase = -((float)(y * 7) * FULL_CIRCLE / (float)RIPPLE_GRID_HEIGHT);
            Vector2D uvRippled;
            uvRippled.y = flV + Math::Sin(Math::NormalizeAngle(aVPhase)) * flVMag;
            for (int32 x = 0; x < RIPPLE_GRID_WIDTH; x++)
            {
                Angle aUPhase = (float)(x * 9) * FULL_CIRCLE / (float)RIPPLE_GRID_WIDTH;
                float flU = (float)x / (float)(RIPPLE_GRID_WIDTH - 1);
                uvRippled.x = flU + Math::Sin(Math::NormalizeAngle(aUPhase)) * flUMag;

                Vector3D wpPos(flU, flV, 0.0f);
                Vertex1TexLSet(pv[y * RIPPLE_GRID_WIDTH + x],
                               wpPos,
                               cgcZAxisNormal,
                               vcLoc,
                               VertexColor(crgbaBlack),
                               Vector2DTexCoordToUByte4(uvRippled));
            }
        }

        BOOL fSuccess = GfxVBAllocTracked(RESOURCEUSAGE_IMMUTABLE, VBFORMAT_1TEX_L, RIPPLE_GRID_VERT_COUNT, FALSE, pv, ptFXGlobals->vbidRipple, "", 0, "");


        SigmaMemoryFree(pv, NULL);
    }

    sPostFXGfxSSAOInit();
    sPostFXGfxRetroInit();
    sPostFXGfxGammaInit();

    return TRUE;
}

void PostFXGfxDispose()
{
    int32 nNumRWindows = GetRWindowMgr()->GetRWindowCount();
    for (int32 x = 0; x < nNumRWindows; x++)
    {
        RWindow & tRWindow = GetRWindowMgr()->GetRWindow(x);
        tRWindow.PostFXGfxDispose();
    }

    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    GfxVBRelease(ptFXGlobals->vbidRipple);
    GfxIBRelease(ptFXGlobals->ibidRipple);
    GfxIBRelease(ptFXGlobals->ibid);
    GfxVBRelease(ptFXGlobals->vbid);
    gptGroupDefTexture->Purge(ptFXGlobals->snoSSAORandomTexture, 0, 0, 0, GLOBALCACHE_MAIN);
    gptGroupDefTexture->Purge(ptFXGlobals->snoRetroDitherTexture, 0, 0, 0, GLOBALCACHE_MAIN);
    gptGroupDefTexture->Purge(ptFXGlobals->snoGammaTexture, 0, 0, 0, GLOBALCACHE_MAIN);
}

// NON_MATCHING: 680 vs 572; the original keeps the 8-iteration eFilterType loop (jump table, cases 1-7); ours fully unrolls it. Body now on its DWARF lines (854-976, closing brace one line early because sGetColorBlindTransform sits at 977); the relayout did not change the code.










void PostFXInitializeForWorld()

{
    CGame & tClientGame = CGameGet();
    PostFXGlobals * ptFXGlobals = tClientGame.ptPostFXGlobals;

    const WorldCommonData & tWCD = WorldGetCommonData(CGameGetCurrentSWorldID());
    ScopedWorldDefinitionLockR pdefWorld(tWCD.snoWorld);
    const Environment * ptEnvironment = &pdefWorld->tEnvironment;



    ptFXGlobals->nNumEffects = 0;
    ptFXGlobals->eFXStack[ptFXGlobals->nNumEffects++] = POSTEFFECT_DISTORTION;
    ptFXGlobals->eFXStack[ptFXGlobals->nNumEffects++] = POSTEFFECT_BLOOM;
    ptFXGlobals->eFXStack[ptFXGlobals->nNumEffects++] = POSTEFFECT_RETRO;
    ptFXGlobals->eFXStack[ptFXGlobals->nNumEffects++] = POSTEFFECT_TONEMAP;


    float flFilterOffset[4] = { 0.5f, 2.0f, 0.0f, 0.0f };
    float flFilterWeights[4] = { 0.34f, 0.16f, 0.0f, 0.0f };


















    SigmaMemoryClear(ptFXGlobals->tFilter, sizeof(ptFXGlobals->tFilter));

    for (int32 eFilterType = 0; eFilterType < NUM_BLUR_FILTERS; eFilterType++)
    {
        BlurFilter & tFilter = ptFXGlobals->tFilter[eFilterType];
        switch (eFilterType)
        {
            case BLUR_FILTER_HORZ_LEFT:
                for (int32 i = 0; i < 4; i++)
                {
                    tFilter.uvSample[i].flU = -flFilterOffset[i];
                    ShaderFloatSet(tFilter.tWeight[i], flFilterWeights[i]);
                }
                break;

            case BLUR_FILTER_HORZ_RIGHT:
                for (int32 i = 0; i < 4; i++)
                {
                    tFilter.uvSample[i].flU = flFilterOffset[i];
                    ShaderFloatSet(tFilter.tWeight[i], flFilterWeights[i]);
                }
                break;

            case BLUR_FILTER_VERT_UP:
                for (int32 i = 0; i < 4; i++)
                {
                    tFilter.uvSample[i].flV = -flFilterOffset[i];
                    ShaderFloatSet(tFilter.tWeight[i], flFilterWeights[i]);
                }
                break;

            case BLUR_FILTER_VERT_DOWN:
                for (int32 i = 0; i < 4; i++)
                {
                    tFilter.uvSample[i].flV = flFilterOffset[i];
                    ShaderFloatSet(tFilter.tWeight[i], flFilterWeights[i]);
                }
                break;

            case BLUR_FILTER_HORZ:

                tFilter.uvSample[0].flU = -flFilterOffset[0];
                ShaderFloatSet(tFilter.tWeight[0], flFilterWeights[0]);
                tFilter.uvSample[1].flU = -flFilterOffset[1];
                ShaderFloatSet(tFilter.tWeight[1], flFilterWeights[1]);
                tFilter.uvSample[2].flU = flFilterOffset[0];
                ShaderFloatSet(tFilter.tWeight[2], flFilterWeights[0]);
                tFilter.uvSample[3].flU = flFilterOffset[1];
                ShaderFloatSet(tFilter.tWeight[3], flFilterWeights[1]);
                break;

            case BLUR_FILTER_VERT:

                tFilter.uvSample[0].flV = -flFilterOffset[0];
                ShaderFloatSet(tFilter.tWeight[0], flFilterWeights[0]);
                tFilter.uvSample[1].flV = -flFilterOffset[1];
                ShaderFloatSet(tFilter.tWeight[1], flFilterWeights[1]);
                tFilter.uvSample[2].flV = flFilterOffset[0];
                ShaderFloatSet(tFilter.tWeight[2], flFilterWeights[0]);
                tFilter.uvSample[3].flV = flFilterOffset[1];
                ShaderFloatSet(tFilter.tWeight[3], flFilterWeights[1]);
                break;

            case BLUR_FILTER_CROSS:

                tFilter.uvSample[0].flU = -flFilterOffset[0];
                tFilter.uvSample[0].flV = -flFilterOffset[0];
                ShaderFloatSet(tFilter.tWeight[0], 0.25f);
                tFilter.uvSample[1].flU = -flFilterOffset[1];
                tFilter.uvSample[1].flV = flFilterOffset[1];
                ShaderFloatSet(tFilter.tWeight[1], 0.25f);
                tFilter.uvSample[2].flU = flFilterOffset[2];
                tFilter.uvSample[2].flV = -flFilterOffset[2];
                ShaderFloatSet(tFilter.tWeight[2], 0.25f);
                tFilter.uvSample[3].flU = flFilterOffset[3];
                tFilter.uvSample[3].flV = flFilterOffset[3];
                ShaderFloatSet(tFilter.tWeight[3], 0.25f);
                break;
        }
    }



}
static void sGetColorBlindTransform(Matrix4x4 & mColor, const DebugColorBlindMode eMode)
{
    switch (eMode)
    {
        case COLORBLIND_SIM_PROTANOPIA:
            mColor = Matrix4x4(0.567f, 0.433f, 0.0f, 0.0f,
                               0.558f, 0.442f, 0.0f, 0.0f,
                               0.0f, 0.242f, 0.758f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_SIM_PROTANOMALY:
            mColor = Matrix4x4(0.817f, 0.183f, 0.0f, 0.0f,
                               0.337f, 0.667f, 0.0f, 0.0f,
                               0.0f, 0.125f, 0.875f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_SIM_DEUTERANOPIA:
            mColor = Matrix4x4(0.625f, 0.375f, 0.0f, 0.0f,
                               0.7f, 0.3f, 0.0f, 0.0f,
                               0.0f, 0.3f, 0.7f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_SIM_DEUTERANOMALY:
            mColor = Matrix4x4(0.8f, 0.2f, 0.0f, 0.0f,
                               0.258f, 0.742f, 0.0f, 0.0f,
                               0.0f, 0.142f, 0.858f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_SIM_TRITANOPIA:
            mColor = Matrix4x4(0.95f, 0.05f, 0.0f, 0.0f,
                               0.0f, 0.433f, 0.567f, 0.0f,
                               0.0f, 0.475f, 0.525f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_SIM_TRITANOMALY:
            mColor = Matrix4x4(0.967f, 0.033f, 0.0f, 0.0f,
                               0.0f, 0.733f, 0.267f, 0.0f,
                               0.0f, 0.183f, 0.817f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_SIM_ACHROMATOPSIA:
            mColor = Matrix4x4(0.299f, 0.587f, 0.114f, 0.0f,
                               0.299f, 0.587f, 0.114f, 0.0f,
                               0.299f, 0.587f, 0.114f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_SIM_ACHROMATOMALY:
            mColor = Matrix4x4(0.618f, 0.32f, 0.062f, 0.0f,
                               0.163f, 0.775f, 0.062f, 0.0f,
                               0.163f, 0.32f, 0.516f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_DALTONIZE_PROTANOPIA:
            mColor = Matrix4x4(0.0f, 2.02344f, -2.52581f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_DALTONIZE_DEUTERANOPIA:
            mColor = Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
                               0.494207f, 0.0f, 1.24827f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case COLORBLIND_DALTONIZE_TRITANOPIA:
            mColor = Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               -0.395913f, 0.801109f, 0.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);
            break;
        default:
            mColor = cgm4x4Identity;
            break;
    }
}
































static void sPostFXBlend(RWindow & tRWindow, const SNO snoShader, const SNO snoTexA, const SNO snoTexB, const SNO snoTexC)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    Matrix4x4 mProj;
    int32 nPassIndex = 0;
    const ShaderConstBinding * ptBinding;
    const ShaderConstBinding * ptBindingColor;
    uint32 dwStartIndex = 0;

    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);
    memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
    GfxProjectionMatrixSet(mProj);

    GfxShaderInit(snoShader, nPassIndex);
    GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoTexA);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_LIGHTMAP), snoTexB);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA), snoTexC);
    GfxShaderSetIB(ptFXGlobals->ibid, 0);

    ptBinding = GfxShaderGetConstantBinding(VSC_BLUR_CONSTANTS);
    if (ptBinding)
    {
        ShaderFloat tTemp;
        tTemp[0] = ptFXGlobals->flMotionBlurBlendFrac;
        tTemp[1] = 0.0f;
        tTemp[2] = 0.0f;
        tTemp[3] = 0.0f;
        GfxShaderSetConstant(ptBinding, tTemp, 1);
    }

    ptBinding = GfxShaderGetConstantBinding(VSC_DEPTH_OF_FIELD);
    if (ptBinding)
    {
        ShaderFloat tParams;
        Weather::GetDepthOfField(tRWindow.GetWeatherRWindowData(), tParams[0], tParams[1], tParams[2]);
        tParams[3] = 0.0f;
        tParams[2] *= 0.01f;
        GfxShaderSetConstant(ptBinding, tParams, 1);
    }

    ptBindingColor = GfxShaderGetConstantBinding(VSC_MATRIX_COLOR);
    if (ptBindingColor)
    {
        Matrix4x4 mColor;
        sGetColorBlindTransform(mColor, sgtAppGlobals.eDebugColorBlindMode);
        GfxShaderSetConstant(ptBindingColor, &mColor, 4);
    }

    GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
}





















static void sPostFXBlendColorCorrection(const float flBlend, const float flGhosted, const SNO snoShader, const SNO snoTexBloom, const SNO snoTexA, const SNO snoTexB, const SNO snoTexC, const SNO snoTexD, const SNO snoTexE, const SNO snoTexF, const SNO snoTexG)
{
    PostFXGlobals * ptFXGlobals;
    float flWidth, flHeight;
    float flInvWidth, flInvHeight;
    Matrix4x4 mProj;
    int32 nPassIndex = 0;
    uint32 dwStartIndex = 0;

    ptFXGlobals = CGameGet().ptPostFXGlobals;

    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);
    GfxGetRenderTargetFloatDimensions(flWidth, flHeight);
    GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);
    memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
    GfxProjectionMatrixSet(mProj);

    GfxShaderInit(snoShader, nPassIndex);
    GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoTexA);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_LIGHTMAP), snoTexB);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA), snoTexC);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_1), snoTexD);
    if (snoTexE != SNO_NONE)
    {
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_2), snoTexE);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_3), snoTexF);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_4), snoTexG);
    }
    if (snoTexBloom != SNO_NONE)
    {
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_5), snoTexBloom);
    }

    {
        const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding(VSC_COLOR_CORRECTION_BLEND);
        if (ptBinding)
        {
            ShaderFloat tTemp;
            tTemp[0] = flBlend;
            tTemp[1] = flGhosted;
            tTemp[2] = 0.0f;
            tTemp[3] = 0.0f;
            GfxShaderSetConstant(ptBinding, tTemp, 1);
        }
    }

    {
        ShaderFloat flTargetDims;
        flTargetDims[0] = flWidth;
        flTargetDims[1] = flHeight;
        flTargetDims[2] = flInvWidth;
        flTargetDims[3] = flInvHeight;
        const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding(VSC_TARGET_DIMENSIONS);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, flTargetDims, 1);
    }

    GfxShaderSetIB(ptFXGlobals->ibid, 0);
    gptGFXAPI->SetWriteMasks(TRUE, TRUE);

    GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
}
































































void PostFXFilterRender(const SNO snoShader, const SNO snoTexture)
{
    PostFXFilterRender(snoShader, snoTexture, CGameGet().ptPostFXGlobals->tFilter[BLUR_FILTER_NONE], FALSE);
}







void PostFXFilterRender(const SNO snoShader, const SNO snoTexture, const BlurFilter & tFilter, BOOL bNullPixelShader)
{
    PostFXGlobals * ptFXGlobals;
    float flWidth, flHeight;
    float flInvWidth, flInvHeight;
    Matrix4x4 mProj;
    int32 nPassIndex = 0;
    uint32 dwStartIndex = 0;

    ptFXGlobals = CGameGet().ptPostFXGlobals;

    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);
    GfxGetRenderTargetFloatDimensions(flWidth, flHeight);
    GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);
    memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
    GfxProjectionMatrixSet(mProj);

    GfxShaderInit(snoShader, nPassIndex);
    GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoTexture);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_GLOW), snoTexture);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ENVMAP), snoTexture);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_GLOSS), snoTexture);
    GfxShaderSetIB(ptFXGlobals->ibid, 0);

    {
        ShaderFloat flTemp1;
        ShaderFloat flTemp2;
        ShaderFloat flTargetDims;
        const ShaderConstBinding * ptBinding;

        flTemp1[0] = tFilter.uvSample[0].flU * flInvWidth;
        flTemp1[1] = tFilter.uvSample[0].flV * flInvHeight;
        flTemp1[2] = tFilter.uvSample[1].flU * flInvWidth;
        flTemp1[3] = tFilter.uvSample[1].flV * flInvHeight;
        flTemp2[0] = tFilter.uvSample[2].flU * flInvWidth;
        flTemp2[1] = tFilter.uvSample[2].flV * flInvHeight;
        flTemp2[2] = tFilter.uvSample[3].flU * flInvWidth;
        flTemp2[3] = tFilter.uvSample[3].flV * flInvHeight;
        flTargetDims[0] = flWidth;
        flTargetDims[1] = flHeight;
        flTargetDims[2] = flInvWidth;
        flTargetDims[3] = flInvHeight;

        ptBinding = GfxShaderGetConstantBinding(VSC_FILTERTAPS0);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, flTemp1, 1);
        ptBinding = GfxShaderGetConstantBinding(VSC_FILTERTAPS1);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, flTemp2, 1);
        ptBinding = GfxShaderGetConstantBinding(VSC_TARGET_DIMENSIONS);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, flTargetDims, 1);
        ptBinding = GfxShaderGetConstantBinding(VSC_FILTERWEIGHTS1);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, tFilter.tWeight[0], 1);
        ptBinding = GfxShaderGetConstantBinding(VSC_FILTERWEIGHTS2);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, tFilter.tWeight[1], 1);
        ptBinding = GfxShaderGetConstantBinding(VSC_FILTERWEIGHTS3);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, tFilter.tWeight[2], 1);
        ptBinding = GfxShaderGetConstantBinding(VSC_FILTERWEIGHTS4);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, tFilter.tWeight[3], 1);
    }

    if (bNullPixelShader)
        gptGFXAPI->SetPixelShader(NULL, 0);

    GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
}

































void PostFXFilterRender(const SNO snoShader, const SNO snoTexture0, const SNO snoTexture1, const SNO snoTexture2, const SNO snoTexture3)
{
    PostFXGlobals * ptFXGlobals;
    float flWidth, flHeight;
    float flInvWidth, flInvHeight;
    Matrix4x4 mProj;
    int32 nPassIndex = 0;
    uint32 dwStartIndex = 0;

    ptFXGlobals = CGameGet().ptPostFXGlobals;

    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);
    GfxGetRenderTargetFloatDimensions(flWidth, flHeight);
    GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);
    memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
    GfxProjectionMatrixSet(mProj);

    GfxShaderInit(snoShader, nPassIndex);
    GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoTexture0);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_GLOW), snoTexture1);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ENVMAP), snoTexture2);
    GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_GLOSS), snoTexture3);
    GfxShaderSetIB(ptFXGlobals->ibid, 0);

    {
        ShaderFloat flTargetDims;
        const ShaderConstBinding * ptBinding;

        flTargetDims[0] = flWidth;
        flTargetDims[1] = flHeight;
        flTargetDims[2] = flInvWidth;
        flTargetDims[3] = flInvHeight;

        ptBinding = GfxShaderGetConstantBinding(VSC_TARGET_DIMENSIONS);
        if (ptBinding)
            GfxShaderSetConstant(ptBinding, flTargetDims, 1);
    }

    GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
}





















void PostFXRetro(RWindow & tRWindow, const IRect2D & rectViewport, SNO snoDest, SNO snoSource, SNO snoDepthTexture);
void PostFXCopyRenderTarget(const uint32 dwRTIndex, const SNO snoDestTexture);
void sPostFXTonemap(RWindow & tRWindow, const IRect2D & rectViewport, SNO snoDest, SNO snoSource, SNO snoBloomBuffer);
void PostFXComputeViewportUVScaleBias(Vector4D & vViewportScaleBias, uint32 dwWidth, uint32 dwHeight, const IRect2D & rectViewport);
static void PostFXDistortion(RWindow & tRWindow, const IRect2D & rectViewport, const SNO snoSourceBuffer, const SNO snoDestBuffer)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;

    SNO snoShader = GlobalSNOGet(GLOBALSNO_POSTFX_DISTORTION_SHADER);
    GfxSetRenderTarget(snoDestBuffer, CUBEMAPFACE_POSX);
    GfxViewportSet(rectViewport);

    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);

    {
        Matrix4x4 mProj; memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
        GfxProjectionMatrixSet(mProj);

        int32 nPassIndex = 0;
        GfxShaderInit(snoShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoSourceBuffer);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_ALPHA), tRWindow.snoEffectBuffer[EFFECTBUFFER_BLUR_DISTORTION]);

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoDestBuffer);
        GfxSetRenderTarget(snoSourceBuffer, CUBEMAPFACE_POSX);

        ScopedShaderDefinitionLockR pdefCopyShader(GlobalSNOGet(GLOBALSNO_POSTFX_COPY_SHADER));
        gptGFXAPI->SetVertexShader(pdefCopyShader, 0);
        gptGFXAPI->SetPixelShader(pdefCopyShader, 0);

        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);

        ScopedShaderDefinitionLockR pdefDistortionShader(snoShader);
        gptGFXAPI->SetVertexShader(pdefDistortionShader, 0);
        gptGFXAPI->SetPixelShader(pdefDistortionShader, 0);
    }
}














































































static void PostFXBloom(RWindow & tRWindow, const IRect2D & rectViewport, const SNO snoSourceBuffer, const SNO snoDestBuffer)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;

    RenderTarget tRenderTargets[2];
    GfxGetRenderTargets(tRenderTargets);
    SNO snoDepthTarget = GfxGetDepthTarget();
    Vector4D vScaleAndBias;
    GfxViewportScaleBiasGet(vScaleAndBias);

    IRect2D rectBloom;
    const int32 dwRenderTargetReduction = 4;
    rectBloom.left = rectViewport.left / dwRenderTargetReduction;
    rectBloom.top = rectViewport.top / dwRenderTargetReduction;
    rectBloom.right = rectViewport.right / dwRenderTargetReduction;
    rectBloom.bottom = rectViewport.bottom / dwRenderTargetReduction;
    Vector4D vBloomScaleAndBias;
    {
        ScopedTextureDefinitionLockR pdefTexture(tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG]);

        uint32 dwWidth, dwHeight;
        gptGFXAPI->GetRenderTargetCurrentResolution(pdefTexture->tTexData.htex, dwWidth, dwHeight);
        PostFXComputeViewportUVScaleBias(vBloomScaleAndBias, dwWidth, dwHeight, rectBloom);
    }

    GfxSetRenderAndDepthTarget(tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG], SNO_NONE, CUBEMAPFACE_POSX);

    GfxRenderClear(TRUE, FALSE, FALSE, crgbaBlack);
    GfxViewportSet(rectBloom);
    GfxViewportScaleBiasSet(vScaleAndBias);
    PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_BLOOM_SHRINK_CONSOLE_SHADER), snoSourceBuffer);

    int32 nBlurLoops = 5;
    for (int32 i = 0; i < nBlurLoops; i++)
    {
        BOOL fClear = (i == 0);

        sPostFXBlurGaussian5x5(rectBloom,
                               vBloomScaleAndBias,
                               tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG],
                               tRWindow.snoEffectBuffer[EFFECTBUFFER_SCREEN_HALF_PING],
                               tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG],
                               GlobalSNOGet(GLOBALSNO_POSTFX_BLOOM_BLUR_HORIZONTAL_CONSOLE_SHADER),
                               GlobalSNOGet(GLOBALSNO_POSTFX_BLOOM_BLUR_VERTICAL_CONSOLE_SHADER),
                               fClear,
                               0);
    }

    GfxSetRenderAndDepthTarget(snoDestBuffer, SNO_NONE, CUBEMAPFACE_POSX);
    GfxViewportSet(rectViewport);
    GfxViewportScaleBiasSet(vScaleAndBias);

    {
        SNO snoShader = GlobalSNOGet(GLOBALSNO_POSTFX_SOFTFOCUS_CONSOLE_SHADER);

        GfxWorldMatrixSet(cgm4x4Identity);
        GfxViewMatrixSet(cgm4x4Identity);

        Matrix4x4 mProj; memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
        GfxProjectionMatrixSet(mProj);

        int32 nPassIndex = 0;
        GfxShaderInit(snoShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoSourceBuffer);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_LIGHTMAP), tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG]);

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding(VSC_FILTERTAPS0);
        if (ptBinding)
        {
            GfxShaderSetConstant(ptBinding, &vBloomScaleAndBias, 1);
        }

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }

    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(snoDepthTarget);
    GfxViewportSet(rectViewport);
    GfxViewportScaleBiasSet(vScaleAndBias);
}

























BOOL sFXPassEnabled(const PostEffect eEffect)
{
    switch (eEffect)
    {
        case POSTEFFECT_DISTORTION:
            if (!PostFXGetFrameFlag(POSTFX_FRAME_HAS_SOMETHING_DISTORTING))
                return FALSE;
            if (!AppDrawFlagIsSet(APP_DRAW_HWCLASS_DISTORTIONS_BIT))
                return FALSE;
            return !AppDrawFlagIsSet(APP_DRAW_NO_POSTFX_DISTORTION_BIT);
        case POSTEFFECT_BLOOM:
            return !AppDrawFlagIsSet(APP_DRAW_NO_POSTFX_BLOOM_BIT);
        case POSTEFFECT_RETRO:
            return CWorldIsRetro();
        case POSTEFFECT_TONEMAP:
            switch (sgtAppGlobals.eDebugRenderMode)
            {
                case DRM_GRAYSCALE:
                    return TRUE;
                case DRM_MIPMAPS:
                case DRM_LIGHTMAP_MIPMAPS:
                    return FALSE;
                default:
                    break;
            }
            {
                PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
                if ((ptFXGlobals->dwFlags & (1 << POSTFX_ENABLE_COLOR_CORRECTION_BIT)) && !AppDrawFlagIsSet(APP_DRAW_HIGH_RES_SCREENSHOT_BIT))
                {
                    ColorCorrectionParams * ptParams;
                    ColorCorrectionParams * ptParamsPrev;
                    ColorCorrectionParams * ptParamsPrev2;
                    Weather::GetColorCorrection(GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData(), &ptParams, &ptParamsPrev, &ptParamsPrev2);
                    if (ptParams->snoTexSaturationVignette != SNO_NONE)
                        return TRUE;
                }
            }
            return FALSE;
        default:
            return TRUE;
    }
}


































enum eEffectOutput {
    FXOUTPUT_MUST_WRITE_TO_SOURCE = 0,
    FXOUTPUT_MUST_WRITE_TO_DEST = 1,
    FXOUTPUT_CAN_WRITE_TO_SOURCE_OR_DEST = 2,
};
eEffectOutput sFXPassOutput(const PostEffect eEffect)
{

    eEffectOutput fxOut = FXOUTPUT_CAN_WRITE_TO_SOURCE_OR_DEST;

    switch (eEffect)
    {
    case POSTEFFECT_DISTORTION:
        fxOut = FXOUTPUT_MUST_WRITE_TO_SOURCE;
        break;

    case POSTEFFECT_BLOOM:
    case POSTEFFECT_TONEMAP:
    case POSTEFFECT_BLOOM_AND_TONEMAP:
        fxOut = FXOUTPUT_MUST_WRITE_TO_DEST;
        break;

    case POSTEFFECT_RETRO:
        fxOut = FXOUTPUT_CAN_WRITE_TO_SOURCE_OR_DEST;
        break;
    }

    return fxOut;
}
























// LINKER-REMOVED: DWARF size 0x88; address-0 rows: no row at 1835, inner bound `nNumFX - 1` (1837:46 is the `-`), `nNumFX--` at 1842:19 after the loop, closing brace 1846.
void sCombineFXPasses(PostEffect * eFXUsed, int32 & nNumFX)
{
    for (int32 i = 0; i < nNumFX - 1; i++)
    {
        if ((eFXUsed[i] == POSTEFFECT_BLOOM) && (eFXUsed[i + 1] == POSTEFFECT_TONEMAP))
        {
            eFXUsed[i] = POSTEFFECT_BLOOM_AND_TONEMAP;


            for (int32 j = i + 1; j < nNumFX - 1; j++)
            {
                eFXUsed[j] = eFXUsed[j + 1];
            }

            nNumFX--;
            break;
        }
    }
}











// NON_MATCHING: 4116 vs 4124 bytes. Only the third and fourth inlined sPostFXBlurGaussian5x5 passes of PostFXBloom differ: the original keeps the
// constant 61 (vertical blur shader) in w22 and re-materialises 0x3f000000 twice more; ours keeps 0x3f000000 in w22. Everything else is equal.
static void PostFXCombinedBloomAndTonemap(RWindow & tRWindow, const IRect2D & rectViewport, const SNO snoSourceBuffer, const SNO snoDestBuffer);
void PostFXTonemap(RWindow & tRWindow, const IRect2D & rectViewport, SNO snoDest, SNO snoSource);
void PostFXDraw(RWindow & tRWindow, const IRect2D & rectViewport)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;

    if (ptFXGlobals->nNumEffects > 0)
    {
        SNO snoSource;
        SNO snoDest;

        PostEffect eFXUsed[NUM_POSTEFFECTS];
        int32 nNumFX = 0;
        for (int32 i = 0; i < ptFXGlobals->nNumEffects; i++)
        {
            if (sFXPassEnabled(ptFXGlobals->eFXStack[i]))
            {
                eFXUsed[nNumFX++] = ptFXGlobals->eFXStack[i];
            }
        }

        sCombineFXPasses(eFXUsed, nNumFX);

        int32 nPassIndexToSwitchToDefault;
        for (nPassIndexToSwitchToDefault = nNumFX - 1; nPassIndexToSwitchToDefault >= 0; nPassIndexToSwitchToDefault--)
        {
            if (sFXPassOutput(eFXUsed[nPassIndexToSwitchToDefault]) != FXOUTPUT_MUST_WRITE_TO_SOURCE)
            {
                break;
            }
        }

        snoSource = tRWindow.GetDefaultColorTarget();
        snoDest = tRWindow.snoEffectBuffer[EFFECTBUFFER_SCREEN_PING];

        SNO snoResult;

        for (int32 i = 0; i < nNumFX; i++)
        {
            if (i == nPassIndexToSwitchToDefault)
            {
                BOOL bSourceIsDefaultTarget = (snoSource == tRWindow.GetDefaultColorTarget());
                BOOL bCurrentPassCantWriteToSource = (sFXPassOutput(eFXUsed[i]) == FXOUTPUT_CAN_WRITE_TO_SOURCE_OR_DEST);

                if (!(bSourceIsDefaultTarget && bCurrentPassCantWriteToSource))
                {
                    snoDest = tRWindow.GetDefaultColorTarget();
                }
            }

            switch (eFXUsed[i])
            {
            case POSTEFFECT_DISTORTION:
                PostFXDistortion(tRWindow, rectViewport, snoSource, snoDest);
                continue;

            case POSTEFFECT_BLOOM:
                PostFXBloom(tRWindow, rectViewport, snoSource, snoDest);
                break;

            case POSTEFFECT_TONEMAP:
                PostFXTonemap(tRWindow, rectViewport, snoDest, snoSource);
                break;

            case POSTEFFECT_BLOOM_AND_TONEMAP:
                PostFXCombinedBloomAndTonemap(tRWindow, rectViewport, snoSource, snoDest);
                break;

            case POSTEFFECT_RETRO:
            {
                SNO snoDepthTexture;
                if (GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D)
                {
                    snoDepthTexture = PostFXGetBuffer(EFFECTBUFFER_DEPTH_PREV);
                }
                else
                {
                    snoDepthTexture = PostFXGetBuffer(EFFECTBUFFER_DEPTH);
                }

                PostFXRetro(tRWindow, rectViewport, snoDest, snoSource, snoDepthTexture);
                break;
            }
            }

            if (snoSource == tRWindow.GetDefaultColorTarget())
            {
                snoSource = tRWindow.snoEffectBuffer[EFFECTBUFFER_SCREEN_PONG];
            }
            snoResult = snoSource;
            snoSource = snoDest;
            snoDest = snoResult;
        }

        snoResult = snoSource;
        if (snoResult != tRWindow.GetDefaultColorTarget())
        {

            GfxSetRenderAndDepthTarget(snoResult, SNO_NONE, CUBEMAPFACE_POSX);
            PostFXCopyRenderTarget(0, tRWindow.GetDefaultColorTarget());
        }

        CameraManagerRestoreCurrentCameraToMainCamera();
    }
}





























































































void PostFXFadeStart(const PostFXFadeParams & tFadeParams)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    ptFXGlobals->tFadeParams = tFadeParams;
    ptFXGlobals->flFadeTimeLeft = tFadeParams.flFadeTimeInSeconds;
}





void PostFXFadeDisablePermanentFade()
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    ptFXGlobals->tFadeParams.fKeepOn = FALSE;
}





void PostFXFadeDrawDeferred()
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;

    if (ptFXGlobals->flFadeTimeLeft > 1e-6f || ptFXGlobals->tFadeParams.fKeepOn)
    {
        float flDeltaTime = CGameGetRealDeltaTime();
        IRect2D irectViewportPixels;
        CGameGetLetterboxedViewportPixels(irectViewportPixels);
        Rect2D rectViewportPixels;
        rectViewportPixels.Set(irectViewportPixels);
        float flT;
        if (ptFXGlobals->tFadeParams.flFadeTimeInSeconds > 0.0f)
            flT = 1.0f - ptFXGlobals->flFadeTimeLeft / ptFXGlobals->tFadeParams.flFadeTimeInSeconds;
        else
            flT = 1.0f;
        float flAlpha = Math::Lerp(ptFXGlobals->tFadeParams.flStartAlpha, ptFXGlobals->tFadeParams.flEndAlpha, flT);
        uint8 bAlpha = (uint8)Math::FloatRoundToInt(flAlpha * 255.0f);

        RGBAColor rgbaColor(ptFXGlobals->tFadeParams.rgbaColor);
        rgbaColor.a = bAlpha;
        SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_2D_PRIMS_TRANSPARENT);
        SpriteDrawDeferred(cgtImageTextureFrameNull, snoShader, rectViewportPixels, rgbaColor, crgbaBlack, ptFXGlobals->tFadeParams.flZ, NULL, -1, NULL, 0, RENDERLAYER_INVALID, 1);
        ptFXGlobals->flFadeTimeLeft = Math::Max(0.0f, ptFXGlobals->flFadeTimeLeft - flDeltaTime);

        if (ptFXGlobals->tFadeParams.pfnFadeCallback)
            ptFXGlobals->tFadeParams.pfnFadeCallback(flT);
    }
}














SNO PostFXGetBuffer(const EffectBuffer eBuffer)
{
    if (eBuffer == EFFECTBUFFER_NONE)
        return SNO_NONE;

    RWindow & tRWindow = GetRWindowMgr()->GetCurrentRWindow();
    return tRWindow.snoEffectBuffer[eBuffer];
}







SNO PostFXGetDepthBuffer(const EffectBuffer eBuffer)
{
    if (eBuffer == EFFECTBUFFER_NONE)
        return SNO_NONE;

    RWindow & tRWindow = GetRWindowMgr()->GetCurrentRWindow();
    return tRWindow.snoEffectDepthBuffer[eBuffer];
}







BOOL PostFXEnabled()
{
    BOOL fPostFX = !AppDrawFlagIsSet(APP_DRAW_NO_POSTFX_BIT);

    DebugRenderMode eDRM = sgtAppGlobals.eDebugRenderMode;
    switch (eDRM)
    {
        case DRM_AO: case DRM_AO_SM:
        case DRM_ENVMAP: case DRM_GLOSS:
        case DRM_DEPTH_COMPLEXITY:
        case DRM_FLATSHADED:
            fPostFX = FALSE;
            break;
        default:
            break;
    }

    return fPostFX;
}

void PostFXRenderUpdate()
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    ptFXGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].dwFrameFlags = 0;
}





void PostFXComputeViewportUVScaleBias(Vector4D & vViewportScaleBias, uint32 dwWidth, uint32 dwHeight, const IRect2D & rectViewport)
{
    vViewportScaleBias.x = (float)(rectViewport.right - rectViewport.left) / (float)dwWidth;
    vViewportScaleBias.y = (float)rectViewport.left / (float)dwWidth;
    vViewportScaleBias.z = (float)(rectViewport.bottom - rectViewport.top) / (float)dwHeight;
    vViewportScaleBias.w = (float)rectViewport.top / (float)dwHeight;
}








BOOL PostFXGetFrameFlag(const uint32 dwBit)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    return (ptFXGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER)].dwFrameFlags >> dwBit) & 1;
}





void PostFXSetFrameFlag(const uint32 dwBit, const BOOL fValue)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    uint32 dwMask = 1 << dwBit;
    if (fValue)
        ptFXGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER)].dwFrameFlags |= dwMask;
    else
        ptFXGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER)].dwFrameFlags &= ~dwMask;
}



void PostFXResetReflectionPlanes()
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;

    RenderData & tRenderData = ptFXGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER)];
    tRenderData.nNumPlanes = 0;
    tRenderData.bReflectDroppedPlane = FALSE;
}



















































































































int32 PostFXGetNumReflectionPlanes()
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    return ptFXGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER)].nNumPlanes;
}





Plane & PostFXGetReflectionPlane(int32 nPlaneIndex)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    return ptFXGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_ASYNCRENDER)].wplaneReflect[nPlaneIndex];
}























void sPostFXTonemap(RWindow & tRWindow, const IRect2D & rectViewport, SNO snoDest, SNO snoSource, SNO snoBloomBuffer)
{
    static float flCounter = 0.0f;
    static float flGhosted = 1.0f;

    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    const BOOL fCombinedWithBloom = snoBloomBuffer != SNO_NONE;

    if (sgtAppGlobals.eDebugRenderMode == DRM_GRAYSCALE)
    {
        GfxSetRenderTarget(snoDest, CUBEMAPFACE_POSX);
        GfxViewportSet(rectViewport);
        sPostFXBlend(tRWindow, GlobalSNOGet(GLOBALSNO_POSTFX_GRAYSCALE_SHADER), snoSource, SNO_NONE, SNO_NONE);
        return;
    }

    if ((ptFXGlobals->dwFlags & (1 << POSTFX_ENABLE_COLOR_CORRECTION_BIT)) && !AppDrawFlagIsSet(APP_DRAW_HIGH_RES_SCREENSHOT_BIT))
    {
        GfxSetRenderAndDepthTarget(snoDest, SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectViewport);

        ColorCorrectionParams * ptParams;
        ColorCorrectionParams * ptParamsPrev;
        ColorCorrectionParams * ptParamsPrev2;














        Weather::GetColorCorrection(GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData(), &ptParams, &ptParamsPrev, &ptParamsPrev2);

        if (ptParams->snoTexSaturationVignette != SNO_NONE)
        {
            const float flCounterMax = 5.0f;
            const float flGhostedMax = 1.0f;

            if (ptParamsPrev->snoTexSaturationVignette == SNO_NONE || ptParamsPrev2->snoTexSaturationVignette == SNO_NONE)
            {
                *ptParamsPrev2 = *ptParams;
                *ptParamsPrev = *ptParams;
            }




            flCounter += CGameGetClientDeltaTime();
            if (flCounter >= flCounterMax)
            {
                flCounter = 0.0f;
                *ptParamsPrev2 = *ptParamsPrev;
                *ptParamsPrev = *ptParams;
            }





            flGhosted += CGameGetClientDeltaTime();
            flGhosted = Math::Min(flGhosted, flGhostedMax);
            flGhosted *= PostFXGetFrameFlag(POSTFX_FRAME_GHOSTED) ? 0.0f : 1.0f;

            if (GameIsStartUp() && !PostFXGetFrameFlag(POSTFX_FRAME_GHOSTED))
                flGhosted = 1.0f;

            float flActualGhosted = 1.0f - flGhosted;

            if (ptParamsPrev->snoTexSaturationVignette == ptParamsPrev2->snoTexSaturationVignette &&
                ptParamsPrev->snoTexCorrectionA == ptParamsPrev2->snoTexCorrectionA &&
                ptParamsPrev->snoTexCorrectionB == ptParamsPrev2->snoTexCorrectionB)
            {
                SNO snoShader;
                if (flActualGhosted == 0.0f)
                    snoShader = GlobalSNOGet(fCombinedWithBloom ? GLOBALSNO_POSTFX_SOFTFOCUS_TONEMAP_CONSOLE_SHADER : GLOBALSNO_POSTFX_TONEMAP_SHADER);
                else
                    snoShader = GlobalSNOGet(fCombinedWithBloom ? GLOBALSNO_POSTFX_SOFTFOCUS_TONEMAP_GRAY_CONSOLE_SHADER : GLOBALSNO_POSTFX_TONEMAP_GRAY_SHADER);
                sPostFXBlendColorCorrection(0.0f, flActualGhosted, snoShader, snoBloomBuffer, snoSource, ptParamsPrev->snoTexSaturationVignette, ptParamsPrev->snoTexCorrectionA, ptParamsPrev->snoTexCorrectionB, SNO_NONE, SNO_NONE, SNO_NONE);
                flCounter = flCounterMax;
            }
            else
            {
                SNO snoShader;
                if (flActualGhosted == 0.0f)
                    snoShader = GlobalSNOGet(fCombinedWithBloom ? GLOBALSNO_POSTFX_SOFTFOCUS_TONEMAP_BLEND_CONSOLE_SHADER : GLOBALSNO_POSTFX_TONEMAP_BLEND_SHADER);
                else
                    snoShader = GlobalSNOGet(fCombinedWithBloom ? GLOBALSNO_POSTFX_SOFTFOCUS_TONEMAP_BLEND_GRAY_CONSOLE_SHADER : GLOBALSNO_POSTFX_TONEMAP_BLEND_GRAY_SHADER);
                sPostFXBlendColorCorrection(flCounter / flCounterMax, flActualGhosted, snoShader, snoBloomBuffer, snoSource, ptParamsPrev2->snoTexSaturationVignette, ptParamsPrev2->snoTexCorrectionA, ptParamsPrev2->snoTexCorrectionB, ptParamsPrev->snoTexSaturationVignette, ptParamsPrev->snoTexCorrectionA, ptParamsPrev->snoTexCorrectionB);
            }
        }
    }
}

















































void PostFXTonemap(RWindow & tRWindow, const IRect2D & rectViewport, SNO snoDest, SNO snoSource)
{
    sPostFXTonemap(tRWindow, rectViewport, snoDest, snoSource, SNO_NONE);
}

















static void PostFXCombinedBloomAndTonemap(RWindow & tRWindow, const IRect2D & rectViewport, const SNO snoSourceBuffer, const SNO snoDestBuffer)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;

    RenderTarget tRenderTargets[2];
    GfxGetRenderTargets(tRenderTargets);
    SNO snoDepthTarget = GfxGetDepthTarget();
    Vector4D vScaleAndBias;
    GfxViewportScaleBiasGet(vScaleAndBias);

    IRect2D rectBloom;
    const int32 dwRenderTargetReduction = 4;
    rectBloom.left = rectViewport.left / dwRenderTargetReduction;
    rectBloom.top = rectViewport.top / dwRenderTargetReduction;
    rectBloom.right = rectViewport.right / dwRenderTargetReduction;
    rectBloom.bottom = rectViewport.bottom / dwRenderTargetReduction;
    Vector4D vBloomScaleAndBias;
    {
        ScopedTextureDefinitionLockR pdefTexture(tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG]);

        uint32 dwWidth, dwHeight;
        gptGFXAPI->GetRenderTargetCurrentResolution(pdefTexture->tTexData.htex, dwWidth, dwHeight);
        PostFXComputeViewportUVScaleBias(vBloomScaleAndBias, dwWidth, dwHeight, rectBloom);
    }

    GfxSetRenderAndDepthTarget(tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG], SNO_NONE, CUBEMAPFACE_POSX);

    GfxRenderClear(TRUE, FALSE, FALSE, crgbaBlack);
    GfxViewportSet(rectBloom);
    GfxViewportScaleBiasSet(vScaleAndBias);
    PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_BLOOM_SHRINK_CONSOLE_SHADER), snoSourceBuffer);

    int32 nBlurLoops = 5;
    for (int32 i = 0; i < nBlurLoops; i++)
    {
        BOOL fClear = (i == 0);

        sPostFXBlurGaussian5x5(rectBloom,
                               vBloomScaleAndBias,
                               tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG],
                               tRWindow.snoEffectBuffer[EFFECTBUFFER_SCREEN_HALF_PING],
                               tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG],
                               GlobalSNOGet(GLOBALSNO_POSTFX_BLOOM_BLUR_HORIZONTAL_CONSOLE_SHADER),
                               GlobalSNOGet(GLOBALSNO_POSTFX_BLOOM_BLUR_VERTICAL_CONSOLE_SHADER),
                               fClear,
                               0);
    }

    GfxSetRenderAndDepthTarget(snoDestBuffer, SNO_NONE, CUBEMAPFACE_POSX);
    GfxViewportSet(rectViewport);
    GfxViewportScaleBiasSet(vScaleAndBias);

    const ShaderConstBinding * ptBinding = GfxShaderGetConstantBinding(VSC_FILTERTAPS0);
    if (ptBinding)
    {
        GfxShaderSetConstant(ptBinding, &vBloomScaleAndBias, 1);
    }

    sPostFXTonemap(tRWindow, rectViewport, snoDestBuffer, snoSourceBuffer, tRWindow.snoEffectBuffer[EFFECTBUFFER_BLOOM_PONG]);

    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(snoDepthTarget);
    GfxViewportSet(rectViewport);
    GfxViewportScaleBiasSet(vScaleAndBias);
}




















void PostFXRetro(RWindow & tRWindow, const IRect2D & rectViewport, SNO snoDest, SNO snoSource, SNO snoDepthTexture)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;


    RenderTarget tRenderTargets[2];
    GfxGetRenderTargets(tRenderTargets);
    SNO snoDepthTarget = GfxGetDepthTarget();


    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);

    {
        SNO snoRetroShader = GlobalSNOGet(GLOBALSNO_POSTFX_RETRO_SHADER);

        GfxSetRenderAndDepthTarget(snoDest, SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectViewport);

        float flWidth, flHeight;
        float flInvWidth, flInvHeight;
        GfxGetRenderTargetFloatDimensions(flWidth, flHeight);
        GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);


        Matrix4x4 mProj; memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
        GfxProjectionMatrixSet(mProj);

        int32 nPassIndex = 0;
        GfxShaderInit(snoRetroShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoSource);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DEPTH), snoDepthTexture);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_1), ptFXGlobals->snoRetroDitherTexture);

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
        const ShaderConstFastBinding * ptFastBinding;

        const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
        const Matrix4x4 & mProjection = cam.mProjection;
        const Matrix4x4 & mView = cam.mView;

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_TARGET_DIMENSIONS);

        {
            ShaderFloat flTargetDims;
            flTargetDims[0] = flWidth;
            flTargetDims[1] = flHeight;

            flTargetDims[2] = flInvWidth;
            flTargetDims[3] = flInvHeight;


            GfxShaderSetConstant(ptFastBinding, flTargetDims, 1);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT1);

        {
            Matrix4x4 mProjectionInv = mProjection.Inverse();
            Matrix4x4 mProjectionInvTranspose = mProjectionInv.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mProjectionInvTranspose, 4);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT2);

        {
            Matrix4x4 mViewInv = mView.Inverse();
            Matrix4x4 mViewInvTranspose = mViewInv.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mViewInvTranspose, 4);
        }

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }


    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(snoDepthTarget);
    GfxViewportSet(rectViewport);


    CameraManagerRestoreCurrentCameraToMainCamera();
}


















































































void PostFXHighlight(RWindow & tRWindow, const IRect2D & rectViewport)
{
    const float flBlurKernelX[5] = { 1.0f / 16.0f, 4.0f / 16.0f, 6.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f };
    const float flBlurKernelY[5] = { 1.0f / 16.0f, 4.0f / 16.0f, 6.0f / 16.0f, 4.0f / 16.0f, 1.0f / 16.0f };

    SNO snoHorizontalShader = GlobalSNOGet(GLOBALSNO_POSTFX_HIGHLIGHT_HORIZONTAL_BLUR_CONSOLE_PPC_SHADER);
    SNO snoVerticalShader = GlobalSNOGet(GLOBALSNO_POSTFX_HIGHLIGHT_VERTICAL_BLUR_CONSOLE_PPC_SHADER);
    SNO snoHighlightShader = GlobalSNOGet(GLOBALSNO_POSTFX_HIGHLIGHT_BLUR_CONSOLE_PPC_SHADER);














    SNO snoSrc = PostFXGetBuffer(GfxGetMSAAEnabled() ? EFFECTBUFFER_HIGHLIGHT : EFFECTBUFFER_SCREEN_PING);
    SNO snoDstH = PostFXGetBuffer(EFFECTBUFFER_SCREEN_PING_LDR);
    SNO snoDstV = PostFXGetBuffer(EFFECTBUFFER_SCREEN_PONG_LDR);

    {
        float flBlurWeights[4];
        float flRemainingWeight = 1.0f - flBlurKernelX[2];
        float flWeight01 = flBlurKernelX[0] + flBlurKernelX[1];
        float flWeight34 = flBlurKernelX[3] + flBlurKernelX[4];
        BlurFilter tFilter;

        flBlurWeights[0] = flWeight01 / flRemainingWeight;
        flBlurWeights[1] = flBlurKernelX[2];
        flBlurWeights[2] = flWeight34 / flRemainingWeight;
        flBlurWeights[3] = 0.0f;

        tFilter.uvSample[0].flU = -(1.0f + flBlurKernelX[0] / flWeight01);
        tFilter.uvSample[0].flV = 0.0f;
        tFilter.uvSample[1].flU = 0.0f;
        tFilter.uvSample[1].flV = 0.0f;
        tFilter.uvSample[2].flU = 1.0f + flBlurKernelX[4] / flWeight34;
        tFilter.uvSample[2].flV = 0.0f;












        ShaderFloatSet(tFilter.tWeight[0], flBlurWeights[0], flBlurWeights[1], flBlurWeights[2], flBlurWeights[3]);

        GfxSetRenderTarget(snoDstH, CUBEMAPFACE_POSX);
        GfxRenderClear(TRUE, FALSE, FALSE, RGBAColor(0, 0, 0, 0));
        GfxViewportSet(rectViewport);
        PostFXFilterRender(snoHorizontalShader, snoSrc, tFilter, FALSE);
    }

    {
        float flBlurWeights[4];
        float flRemainingWeight = 1.0f - flBlurKernelY[2];
        float flWeight01 = flBlurKernelY[0] + flBlurKernelY[1];
        float flWeight34 = flBlurKernelY[3] + flBlurKernelY[4];
        BlurFilter tFilter;

        flBlurWeights[0] = flWeight01 / flRemainingWeight;
        flBlurWeights[1] = flBlurKernelY[2];
        flBlurWeights[2] = flWeight34 / flRemainingWeight;
        flBlurWeights[3] = 0.0f;

        tFilter.uvSample[0].flU = 0.0f;
        tFilter.uvSample[0].flV = -(1.0f + flBlurKernelY[0] / flWeight01);
        tFilter.uvSample[1].flU = 0.0f;
        tFilter.uvSample[1].flV = 0.0f;
        tFilter.uvSample[2].flU = 0.0f;
        tFilter.uvSample[2].flV = 1.0f + flBlurKernelY[4] / flWeight34;




        ShaderFloatSet(tFilter.tWeight[0], flBlurWeights[0], flBlurWeights[1], flBlurWeights[2], flBlurWeights[3]);

        GfxSetRenderTarget(snoDstV, CUBEMAPFACE_POSX);
        GfxViewportSet(rectViewport);
        PostFXFilterRender(snoVerticalShader, snoDstH, tFilter, FALSE);
    }





    GfxSetRenderTarget(tRWindow.GetDefaultColorTarget(), CUBEMAPFACE_POSX);
    GfxViewportSet(rectViewport);
    sPostFXBlend(tRWindow, snoHighlightShader, snoDstV, GlobalSNOGet(GLOBALSNO_HIGHLIGHT_TEXTURE), snoSrc);
}




















































































































































































SNO PostFXFSAA(RWindow & tRWindow, const IRect2D & rectViewport)
{
    SNO snoFXAASource;
    SNO snoFXAA;
    SNO snoFXAADestination;


    snoFXAASource = tRWindow.GetDefaultColorTarget();

    if (!sFXPassEnabled(POSTEFFECT_TONEMAP))
    {
        SNO snoRGBL = GlobalSNOGet(GLOBALSNO_POSTFX_FSAA_RGBL_SHADER);
        GfxSetRenderAndDepthTarget(snoFXAASource, SNO_NONE, CUBEMAPFACE_POSX);
        sPostFXBlend(tRWindow, snoRGBL, snoFXAASource, SNO_NONE, SNO_NONE);
    }

    snoFXAA = GlobalSNOGet(GLOBALSNO_POSTFX_FSAA_FXAA_SHADER);






    snoFXAADestination = PostFXGetBuffer(EFFECTBUFFER_SCREEN_PING);

    {
        PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
        float flInvWidth, flInvHeight;
        Matrix4x4 mProj;
        int32 nPassIndex = 0;
        uint32 dwStartIndex = 0;

        GfxWorldMatrixSet(cgm4x4Identity);
        GfxViewMatrixSet(cgm4x4Identity);
        GfxSetRenderAndDepthTarget(snoFXAADestination, SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectViewport);
        GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);

        memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
        GfxProjectionMatrixSet(mProj);


        GfxShaderInit(snoFXAA, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), snoFXAASource);
        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        {
            ShaderFloat flConsolePosPosOffsets;
            ShaderFloat flConsoleRcpFrameOpt;
            ShaderFloat flConsoleRcpFrameOpt2;
            const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
            const ShaderConstFastBinding * ptConsolePosPosOffsetsBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_FILTERTAPS1);

            flConsolePosPosOffsets[0] = -0.5f * flInvWidth;
            flConsolePosPosOffsets[1] = -0.5f * flInvHeight;
            flConsolePosPosOffsets[2] = 0.5f * flInvWidth;
            flConsolePosPosOffsets[3] = 0.5f * flInvHeight;
            GfxShaderSetConstant(ptConsolePosPosOffsetsBinding, flConsolePosPosOffsets, 1);

            const ShaderConstFastBinding * ptConsoleRcpFrameOptBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_FILTERWEIGHTS1);
            const ShaderConstFastBinding * ptConsoleRcpFrameOpt2Binding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_FILTERWEIGHTS2);
            flConsoleRcpFrameOpt[0] = -0.5f * flInvWidth;
            flConsoleRcpFrameOpt[1] = -0.5f * flInvHeight;
            flConsoleRcpFrameOpt[2] = 0.5f * flInvWidth;
            flConsoleRcpFrameOpt[3] = 0.5f * flInvHeight;
            flConsoleRcpFrameOpt2[0] = -2.0f * flInvWidth;
            flConsoleRcpFrameOpt2[1] = -2.0f * flInvHeight;
            flConsoleRcpFrameOpt2[2] = 2.0f * flInvWidth;
            flConsoleRcpFrameOpt2[3] = 2.0f * flInvHeight;
            GfxShaderSetConstant(ptConsoleRcpFrameOptBinding, flConsoleRcpFrameOpt, 1);
            GfxShaderSetConstant(ptConsoleRcpFrameOpt2Binding, flConsoleRcpFrameOpt2, 1);
        }

        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }

    return snoFXAADestination;
}






































void PostFXSSAO(RWindow & tRWindow, const IRect2D & rectViewport, SNO snoDepthTexture)
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;

    RenderTarget tRenderTargets[2];
    GfxGetRenderTargets(tRenderTargets);
    SNO snoDepthTarget = GfxGetDepthTarget();
    Vector4D vScaleAndBias;
    GfxViewportScaleBiasGet(vScaleAndBias);

    GfxWorldMatrixSet(cgm4x4Identity);
    GfxViewMatrixSet(cgm4x4Identity);

    uint32 renderTargetReduction = 2;
    IRect2D rectHalfResViewport;
    rectHalfResViewport.left = rectViewport.left / renderTargetReduction;
    rectHalfResViewport.top = rectViewport.top / renderTargetReduction;
    rectHalfResViewport.right = rectViewport.right / renderTargetReduction;
    rectHalfResViewport.bottom = rectViewport.bottom / renderTargetReduction;

    Vector4D vHalfResViewportScaleAndBias;
    {
        ScopedTextureDefinitionLockR pdefHalfResTexture(PostFXGetBuffer(EFFECTBUFFER_SSAO_NORMAL));
        PostFXComputeViewportUVScaleBias(vHalfResViewportScaleAndBias, pdefHalfResTexture->dwWidth, pdefHalfResTexture->dwHeight, rectHalfResViewport);
    }

    {
        const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
        const Matrix4x4 & mView = cam.mView;
        const Matrix4x4 & mProjection = cam.mProjection;
        ptFXGlobals->matSSAOViewProj = mView * mProjection;
        ptFXGlobals->matSSAOViewProjTranspose = ptFXGlobals->matSSAOViewProj.Transpose();
    }

    {
        SNO snoSSAOShader = GlobalSNOGet(GLOBALSNO_POSTFX_SSAO_NORMAL_SHADER);

        GfxSetRenderAndDepthTarget(PostFXGetBuffer(EFFECTBUFFER_SSAO_NORMAL), SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectHalfResViewport);
        GfxViewportScaleBiasSet(vScaleAndBias);

        float flWidth, flHeight;
        float flInvWidth, flInvHeight;
        GfxGetRenderTargetFloatDimensions(flWidth, flHeight);
        GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);

        Matrix4x4 mProj; memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
        GfxProjectionMatrixSet(mProj);

        int32 nPassIndex = 0;
        GfxShaderInit(snoSSAOShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DEPTH), snoDepthTexture);

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
        const ShaderConstFastBinding * ptFastBinding;

        const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
        const Matrix4x4 & mProjection = cam.mProjection;

        float flSSAOWidth, flSSAOHeight;
        float flInvSSAOWidth, flInvSSAOHeight;
        GfxGetRenderTargetFloatDimensions(flSSAOWidth, flSSAOHeight);
        GfxGetRenderTargetInvFloatDimensions(flInvSSAOWidth, flInvSSAOHeight);

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_TARGET_DIMENSIONS);

        {
            ShaderFloat flTargetDims =
            {
                flSSAOWidth,
                flSSAOHeight,
                flInvSSAOWidth,
                flInvSSAOHeight,
            };

            GfxShaderSetConstant(ptFastBinding, flTargetDims, 1);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT1);

        {
            Matrix4x4 mProjectionTranspose = mProjection.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mProjectionTranspose, 4);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT2);

        {
            Matrix4x4 mProjectionInv = mProjection.Inverse();
            Matrix4x4 mProjectionInvTranspose = mProjectionInv.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mProjectionInvTranspose, 4);
        }

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }

    {
        SNO snoSSAOShader = GlobalSNOGet(GLOBALSNO_POSTFX_SSAO_SHADER);

        GfxSetRenderAndDepthTarget(PostFXGetBuffer(EFFECTBUFFER_SSAO_PONG), SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectHalfResViewport);
        GfxViewportScaleBiasSet(vHalfResViewportScaleAndBias);

        int32 nPassIndex = 0;
        GfxShaderInit(snoSSAOShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), PostFXGetBuffer(EFFECTBUFFER_SSAO_NORMAL));
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_1), ptFXGlobals->snoSSAORandomTexture);

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
        const ShaderConstFastBinding * ptFastBinding;

        const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
        const Matrix4x4 & mProjection = cam.mProjection;

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_FILTERTAPS0);
        if (ptFastBinding)
        {
            ShaderFloat flParams1 =
            {
                ptFXGlobals->flSSAOOcclusionPower,
                ptFXGlobals->flSSAOOcclusionRadius,
                (float)SSAO_RANDOM_TEXTURE_WIDTH,
                ptFXGlobals->flSSAONoOcclusionDist,
            };

            GfxShaderSetConstant(ptFastBinding, flParams1, 1);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_FILTERTAPS1);

        {
            ShaderFloat flParams2 =
            {
                ptFXGlobals->flSSAOBias,
                0.0f,
                0.0f,
                0.0f,
            };

            GfxShaderSetConstant(ptFastBinding, flParams2, 1);
        }

        float flSSAOWidth, flSSAOHeight;
        float flInvSSAOWidth, flInvSSAOHeight;
        GfxGetRenderTargetFloatDimensions(flSSAOWidth, flSSAOHeight);
        GfxGetRenderTargetInvFloatDimensions(flInvSSAOWidth, flInvSSAOHeight);

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_TARGET_DIMENSIONS);

        {
            ShaderFloat flTargetDims =
            {
                flSSAOWidth,
                flSSAOHeight,
                flInvSSAOWidth,
                flInvSSAOHeight,
            };

            GfxShaderSetConstant(ptFastBinding, flTargetDims, 1);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT1);

        {
            Matrix4x4 mProjectionTranspose = mProjection.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mProjectionTranspose, 4);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT2);

        {
            Matrix4x4 mProjectionInv = mProjection.Inverse();
            Matrix4x4 mProjectionInvTranspose = mProjectionInv.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mProjectionInvTranspose, 4);
        }

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }

    if (AppDrawFlagIsSet(APP_DRAW_SSAO_BLUR_BIT))
    {
        SNO snoSSAOShader = GlobalSNOGet(GLOBALSNO_POSTFX_SSAO_BLUR_SHADER);

        GfxSetRenderAndDepthTarget(PostFXGetBuffer(EFFECTBUFFER_SSAO_PING), SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectHalfResViewport);
        GfxViewportScaleBiasSet(vHalfResViewportScaleAndBias);

        int32 nPassIndex = 0;
        GfxShaderInit(snoSSAOShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), PostFXGetBuffer(EFFECTBUFFER_SSAO_NORMAL));
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_1), PostFXGetBuffer(EFFECTBUFFER_SSAO_PONG));

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
        const ShaderConstFastBinding * ptFastBinding;

        float flInvSSAOWidth, flInvSSAOHeight;
        GfxGetRenderTargetInvFloatDimensions(flInvSSAOWidth, flInvSSAOHeight);

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_FILTERTAPS0);
        if (ptFastBinding)
        {
            ShaderFloat flOffset =
            {
                flInvSSAOWidth,
                0.0f,
                0.0f,
                0.0f,
            };


            GfxShaderSetConstant(ptFastBinding, flOffset, 1);
        }

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }

    if (AppDrawFlagIsSet(APP_DRAW_SSAO_BLUR_BIT))
    {
        SNO snoSSAOShader = GlobalSNOGet(GLOBALSNO_POSTFX_SSAO_BLUR_SHADER);

        GfxSetRenderAndDepthTarget(PostFXGetBuffer(EFFECTBUFFER_SSAO_PONG), SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectHalfResViewport);
        GfxViewportScaleBiasSet(vHalfResViewportScaleAndBias);

        int32 nPassIndex = 0;
        GfxShaderInit(snoSSAOShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), PostFXGetBuffer(EFFECTBUFFER_SSAO_NORMAL));
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_1), PostFXGetBuffer(EFFECTBUFFER_SSAO_PING));

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
        const ShaderConstFastBinding * ptFastBinding;

        float flInvSSAOWidth, flInvSSAOHeight;
        GfxGetRenderTargetInvFloatDimensions(flInvSSAOWidth, flInvSSAOHeight);

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_FILTERTAPS0);
        if (ptFastBinding)
        {
            ShaderFloat flOffset =
            {
                0.0f,
                flInvSSAOHeight,
                0.0f,
                0.0f,
            };


            GfxShaderSetConstant(ptFastBinding, flOffset, 1);
        }

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }

    {
        SNO snoSSAOUpscaleShader = GlobalSNOGet(GLOBALSNO_POSTFX_SSAO_UPSCALE_SHADER);

        GfxSetRenderAndDepthTarget(PostFXGetBuffer(EFFECTBUFFER_SSAO), SNO_NONE, CUBEMAPFACE_POSX);
        GfxViewportSet(rectViewport);
        GfxViewportScaleBiasSet(vHalfResViewportScaleAndBias);

        float flWidth, flHeight;
        float flInvWidth, flInvHeight;
        GfxGetRenderTargetFloatDimensions(flWidth, flHeight);
        GfxGetRenderTargetInvFloatDimensions(flInvWidth, flInvHeight);

        Matrix4x4 mProj; memcpy(&mProj, &cgm4x4Identity, sizeof(mProj));
        GfxProjectionMatrixSet(mProj);

        int32 nPassIndex = 0;
        GfxShaderInit(snoSSAOUpscaleShader, nPassIndex);
        GfxShaderSetStream(STREAMINDEX_DEFAULT, 0, ptFXGlobals->vbid);

        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE), PostFXGetBuffer(EFFECTBUFFER_SSAO_NORMAL));
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_1), PostFXGetBuffer(EFFECTBUFFER_SSAO_PONG));
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_DIFFUSE_LAYER_2), snoDepthTexture);
        GfxShaderSetTexture(GfxGetTextureBinding(SHADERTEX_VIGNETTE), GlobalSNOGet(GLOBALSNO_SUBOBJECT_VIGNETTE_TEXTURE));

        GfxShaderSetIB(ptFXGlobals->ibid, 0);

        const ShaderConstFastBinding * ptFastBindings = GfxShaderGetConstantFastBindings();
        const ShaderConstFastBinding * ptFastBinding;

        const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
        const Matrix4x4 & mProjection = cam.mProjection;

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_TARGET_DIMENSIONS);

        {
            ShaderFloat flTargetDims =
            {
                flWidth,
                flHeight,
                flInvWidth,
                flInvHeight,
            };

            GfxShaderSetConstant(ptFastBinding, flTargetDims, 1);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT1);

        {
            Matrix4x4 mProjectionTranspose = mProjection.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mProjectionTranspose, 4);
        }

        ptFastBinding = GfxShaderGetConstantFastBinding(ptFastBindings, VSC_MATRIX_EFFECT2);

        {
            Matrix4x4 mProjectionInv = mProjection.Inverse();
            Matrix4x4 mProjectionInvTranspose = mProjectionInv.Transpose();
            GfxShaderSetConstant(ptFastBinding, &mProjectionInvTranspose, 4);
        }

        uint32 dwStartIndex = 0;
        GfxShaderDraw(DRAWMODE_TRIANGLESTRIP, 0, 0, ptFXGlobals->nVertCount, dwStartIndex, ptFXGlobals->nVertCount - 2, PERFBIN_POSTFX);
    }

    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(snoDepthTarget);
    GfxViewportScaleBiasSet(vScaleAndBias);

    CameraManagerRestoreCurrentCameraToMainCamera();
}























































































































































































































































































void PostFXResolveMSAAColorTarget(const uint32 dwRTIndex, const SNO snoDestTexture)
{
    HTexture ptDestTarget;
    ScopedTextureDefinitionLockR pdefDestTexture(snoDestTexture);
    ptDestTarget = pdefDestTexture->tTexData.htex;
    gptGFXAPI->TextureResolveMSAARenderTarget(dwRTIndex, ptDestTarget);
}





void PostFXResolveMSAADepthTarget(const SNO snoDestTexture)
{
    if (GfxGetSystemID() == GFX_D3D11)
    {
        Matrix4x4 viewMat = GfxViewMatrixGet();
        Matrix4x4 projMat = GfxProjectionMatrixGet();
        RenderTarget tRenderTargets[2];

        GfxGetRenderTargets(tRenderTargets);
        SNO snoDepthTarget = GfxGetDepthTarget();
        SNO snoTexture = snoDepthTarget;

        SNO snoShader = GlobalSNOGet(GLOBALSNO_POSTFX_COPY_DEPTH_MS_SHADER);
        switch (GfxGetMSAALevel())
        {
            case 1:
                snoShader = GlobalSNOGet(GLOBALSNO_POSTFX_COPY_DEPTH_MS_SHADER);
                break;
            case 2:
                snoShader = GlobalSNOGet(GLOBALSNO_POSTFX_COPY_DEPTH_MS2_SHADER);
                break;
            case 4:
            case 8:
                snoShader = GlobalSNOGet(GLOBALSNO_POSTFX_COPY_DEPTH_MS4_SHADER);
                break;
            case 16:
                snoShader = GlobalSNOGet(GLOBALSNO_POSTFX_COPY_DEPTH_MS8_SHADER);
                break;
        }

        GfxSetRenderAndDepthTarget(SNO_NONE, snoDestTexture, CUBEMAPFACE_POSX);
        PostFXFilterRender(snoShader, snoTexture);

        GfxSetRenderTargets(tRenderTargets);
        GfxSetDepthTarget(snoDepthTarget);

        GfxViewMatrixSet(viewMat);
        GfxProjectionMatrixSet(projMat);
    }
    else
    {
        HTexture ptDestTarget;
        ScopedTextureDefinitionLockR pdefDestTexture(snoDestTexture);
        ptDestTarget = pdefDestTexture->tTexData.htex;
        gptGFXAPI->TextureResolveMSAADepthTarget(ptDestTarget);
    }
}
























void PostFXCopyRenderTarget(const uint32 dwRTIndex, const SNO snoDestTexture)
{
    GfxSetShaderGPRAllocation(0, 40);

    Matrix4x4 viewMat = GfxViewMatrixGet();
    Matrix4x4 projMat = GfxProjectionMatrixGet();
    RenderTarget tRenderTargets[2];

    GfxGetRenderTargets(tRenderTargets);
    SNO snoDepthTarget = GfxGetDepthTarget();
    SNO snoTexture = tRenderTargets[dwRTIndex].snoRTTex;

    GfxSetRenderAndDepthTarget(snoDestTexture, SNO_NONE, CUBEMAPFACE_POSX);
    PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_COPY_SHADER), snoTexture);

    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(snoDepthTarget);

    GfxViewMatrixSet(viewMat);
    GfxProjectionMatrixSet(projMat);

    GfxRestoreShaderGPRAllocation();
}













void PostFXCopyDepthTarget(const SNO snoDestTexture)
{
    if (GfxGetSystemID() == GFX_D3D11)
    {
        IRect2D rectViewport;
        {
            ScopedTextureDefinitionLockR pdefTexture(snoDestTexture);
            rectViewport.left = 0;
            rectViewport.top = 0;
            rectViewport.right = pdefTexture->dwWidth;
            rectViewport.bottom = pdefTexture->dwHeight;
        }

        Matrix4x4 viewMat = GfxViewMatrixGet();
        Matrix4x4 projMat = GfxProjectionMatrixGet();
        RenderTarget tRenderTargets[2];

        GfxGetRenderTargets(tRenderTargets);
        SNO snoDepthTarget = GfxGetDepthTarget();
        SNO snoTexture = snoDepthTarget;

        GfxSetRenderAndDepthTarget(SNO_NONE, snoDestTexture, CUBEMAPFACE_POSX);
        GfxViewportSet(rectViewport);
        PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_COPY_DEPTH_SHADER), snoTexture);

        GfxSetRenderTargets(tRenderTargets);
        GfxSetDepthTarget(snoDepthTarget);

        GfxViewMatrixSet(viewMat);
        GfxProjectionMatrixSet(projMat);
    }
    else
    {
        GfxCopyDepthTarget(snoDestTexture);
    }
}



















































void PostFXCopyRenderTargetMS(const uint32 dwRTIndex, const SNO snoDestTexture)
{
    GfxSetShaderGPRAllocation(0, 40);

    Matrix4x4 viewMat = GfxViewMatrixGet();
    Matrix4x4 projMat = GfxProjectionMatrixGet();
    RenderTarget tRenderTargets[2];

    GfxGetRenderTargets(tRenderTargets);
    SNO snoDepthTarget = GfxGetDepthTarget();
    SNO snoTexture = tRenderTargets[dwRTIndex].snoRTTex;

    GfxSetRenderAndDepthTarget(snoDestTexture, SNO_NONE, CUBEMAPFACE_POSX);
    PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_COPY_MS_SHADER), snoTexture);

    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(snoDepthTarget);

    GfxViewMatrixSet(viewMat);
    GfxProjectionMatrixSet(projMat);

    GfxRestoreShaderGPRAllocation();
}






















































































































































BOOL PostFXTestEnableHDR()
{
    PostFXGlobals * ptFXGlobals = CGameGet().ptPostFXGlobals;
    return ptFXGlobals->dwFlags & (1 << POSTFX_ENABLE_HDR_BIT);
}
