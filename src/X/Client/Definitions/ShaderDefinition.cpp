// Original file: X/Client/Definitions/ShaderDefinition.cpp

#include "ShaderDefinition.h"

// For the header statics the original initializes in this file's static initializer: blz::ignore
// (tuple.h), cgplaceNull (WorldPlace.h) and the GfxVB.h normals.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"

#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/TagMaps/TagNumbers.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Client/Gfx/Gfx.cpp:931 (Gfx.h is not adopted).
void GfxForceDeviceReset(const BOOL fHardReset);

// Defined in X/Client/Gfx/GfxShader.cpp:2621 and on (GfxShader.h is not adopted).
void GfxShaderStateBlockCreate(ShaderDefinition * pdefShader, const int32 nPassIndex, XPtr64<unsigned char> & ptStateBlock);
void GfxShaderStateBlockDispose(XPtr64<unsigned char> & ptStateBlock);
void GfxCreateVertexShader(ShaderDefinition * pdefShader, const int32 nPassIndex);
void GfxDeleteVertexShader(ShaderDefinition * pdefShader, const int32 nPassIndex);
void GfxCreatePixelShader(ShaderDefinition * pdefShader, const int32 nPassIndex);
void GfxDeletePixelShader(ShaderDefinition * pdefShader, const int32 nPassIndex);

// Defined in X/Common/TagMaps/Tags.cpp:295 and :424 (only part of Tags.h is adopted).
CTagMap * TagMapCreate(const TagBases eTagBase);
void TagMapDispose(XPtr64<CTagMap> & pTagMap);

enum {
    SHADER_COMMON_SNO_VERSION = 150,
    SHADER_XML_VERSION = 6,
    SHADER_PC_SNO_VERSION = 150,
    SHADER_CONSOLE_SNO_VERSION = 177,
    SHADER_AXE_SNO_VERSION = 158,
    SHADER_XBOXONE_SNO_VERSION = 198,
    SHADER_PS4_SNO_VERSION = 206,
    SHADER_NX64_SNO_VERSION = 186,
    RENDERPASS_XML_VERSION = 8,
};

const LPCSTR cgszVSConstBindingTable[NUM_VS_CONSTANT_ENTRIES] = {  // line 57
    "Constant0",  // VSC_FILTERTAPS0
    "Constant1",  // VSC_FILTERTAPS1
    "matWorld",  // VSC_MATRIX_WORLD
    "matView",  // VSC_MATRIX_VIEW
    "matWorldView",  // VSC_MATRIX_WV
    "matWorldViewProj",  // VSC_MATRIX_WVP
    "fogParams",  // VSC_FOGPARAMS
    "matTex0",  // VSC_MATRIX_TEX0
    "matTex1",  // VSC_MATRIX_TEX1
    "matTex2",  // VSC_MATRIX_TEX2
    "matTex3",  // VSC_MATRIX_TEX3
    "matTex4",  // VSC_MATRIX_TEX4
    "matTex5",  // VSC_MATRIX_TEX5
    "matTex6",  // VSC_MATRIX_TEX6
    "matTex7",  // VSC_MATRIX_TEX7
    "matTex8",  // VSC_MATRIX_TEX8
    "matTex9",  // VSC_MATRIX_TEX9
    "matTex10",  // VSC_MATRIX_TEX10
    "matTex11",  // VSC_MATRIX_TEX11
    "viewport",  // VSC_VIEWPORT_UV_SCALE_BIAS
    "windowResolution",  // VSC_WINDOW_RESOLUTION
    "colAmbient",  // VSC_AMBIENT
    "lightDirectionals",  // VSC_DIRECTIONALLIGHTS
    "lightPoints",  // VSC_POINTLIGHTS
    "lightSpots",  // VSC_SPOTLIGHTS
    "lightCylindricals",  // VSC_CYLINDRICALLIGHTS
    "lightPointLinears",  // VSC_POINTLINEARLIGHTS
    "numLightsInt",  // VSC_NUM_LIGHTS_INT
    "numLightsFloat",  // VSC_NUM_LIGHTS_FLOAT
    "matBones",  // VSC_BONEMATRIXSTARTREGISTER
    "waveParams",  // VSC_WAVEPARAMS
    "edgealphaParams",  // VSC_EDGEALPHAPARAMS
    "wpPlayer",  // VSC_PLAYERPOS
    "wvScreenZAxis",  // VSC_SCREEN_Z
    "wpViewPos",  // VSC_VIEW_POS
    "Time",  // VSC_TIME
    "MeshLighting",  // VSC_MESHLIGHTING
    "Factor",  // VSC_FACTOR
    "ShadowColor",  // VSC_SHADOWCOLOR
    "SpecularPower",  // VSC_SPECULARPOWER
    "colLight",  // VSC_LIGHTCOLOR
    "Weights1",  // VSC_FILTERWEIGHTS1
    "Weights2",  // VSC_FILTERWEIGHTS2
    "Weights3",  // VSC_FILTERWEIGHTS3
    "Weights4",  // VSC_FILTERWEIGHTS4
    "WaveConstants",  // VSC_WAVE_CONSTANTS
    "fogColor",  // VSC_FOGCOLOR
    "diffuseOverlay",  // VSC_DIFFUSEOVERLAY
    "appearanceFX",  // VSC_APPEARANCE_FX
    "depthOfFieldParams",  // VSC_DEPTH_OF_FIELD
    "cameraParams",  // VSC_CAMERA_PARAMS
    "depthReconstruction",  // VSC_DEPTH_TEXTURE_RECONSTRUCTION
    "edgeFilter",  // VSC_EDGE_FILTER
    "blurConstants",  // VSC_BLUR_CONSTANTS
    "waterTextureConstants",  // VSC_WATER_TEXTURE_CONSTANTS
    "hitFlashColorAdd",  // VSC_HITFLASHADDCOLOR
    "hitFlashColorMul",  // VSC_HITFLASHMULCOLOR
    "tintRampUV",  // VSC_TINT_RAMP_UV
    "heroTintColorB",  // VSC_HEROTINTCOLOR_B_DEPRECATED
    "weatherIntensity",  // VSC_WEATHER_INTENSITY
    "numBatches",  // VSC_BATCHES
    "randFloat",  // VSC_RAND_FLOAT
    "bUseDyeType",  // VSC_USE_DYE_TYPE
    "deformationScale",  // VSC_DEFORMATION_SCALE
    "colorCorrectionBlend",  // VSC_COLOR_CORRECTION_BLEND
    "targetDimensions",  // VSC_TARGET_DIMENSIONS
    "normalMapScale",  // VSC_NORMALMAPSCALE
    "bannerTintRampUV",  // VSC_BANNER_TINT_RAMP_UV
    "bannerPatternTint",  // VSC_BANNER_PATTERNTINT_DEPRECATED
    "bannerSigilTint",  // VSC_BANNER_SIGILTINT_DEPRECATED
    "bannerPatternDuotoneTint",  // VSC_BANNER_PATTERNDUOTONETINT_DEPRECATED
    "dbgReg0",  // VSC_DBG_REG0
    "lightMapVScale",  // VSC_LIGHTMAP_V_SCALE
    "vecFixedPointPosScale",  // VSC_VEC_FIXED_POINT_POS_SCALE
    "vecFixedPointPosOffset",  // VSC_VEC_FIXED_POINT_POS_OFFSET
    "matWorldIrrad",  // VSC_MATRIX_WORLD_IRRAD
    "alphaTestRef",  // VSC_ALPHATEST_REF
    "qCameraBillboard",  // VSC_QCAMERA_BILLBOARD
    "billboardParams",  // VSC_BILLBOARD_PARAMS
    "matProjection",  // VSC_MATRIX_PROJECTION
    "vecClipPlane",  // VSC_VEC_CLIP_PLANE
    "vecReflectionPlane",  // VSC_VEC_REFLECTION_PLANE
    "matEffect1",  // VSC_MATRIX_EFFECT1
    "matEffect2",  // VSC_MATRIX_EFFECT2
    "matSSAOViewProj",  // VSC_MATRIX_SSAO_VIEW_PROJ
    "matColorTransform",  // VSC_MATRIX_COLOR
};

const int32 cgnVSConstSize[NUM_VS_CONSTANT_ENTRIES] = {  // line 247
    1,  // VSC_FILTERTAPS0
    1,  // VSC_FILTERTAPS1
    4,  // VSC_MATRIX_WORLD
    4,  // VSC_MATRIX_VIEW
    4,  // VSC_MATRIX_WV
    4,  // VSC_MATRIX_WVP
    1,  // VSC_FOGPARAMS
    4,  // VSC_MATRIX_TEX0
    4,  // VSC_MATRIX_TEX1
    4,  // VSC_MATRIX_TEX2
    4,  // VSC_MATRIX_TEX3
    4,  // VSC_MATRIX_TEX4
    4,  // VSC_MATRIX_TEX5
    4,  // VSC_MATRIX_TEX6
    4,  // VSC_MATRIX_TEX7
    4,  // VSC_MATRIX_TEX8
    4,  // VSC_MATRIX_TEX9
    4,  // VSC_MATRIX_TEX10
    4,  // VSC_MATRIX_TEX11
    1,  // VSC_VIEWPORT_UV_SCALE_BIAS
    1,  // VSC_WINDOW_RESOLUTION
    1,  // VSC_AMBIENT
    48,  // VSC_DIRECTIONALLIGHTS
    80,  // VSC_POINTLIGHTS
    96,  // VSC_SPOTLIGHTS
    96,  // VSC_CYLINDRICALLIGHTS
    64,  // VSC_POINTLINEARLIGHTS
    1,  // VSC_NUM_LIGHTS_INT
    1,  // VSC_NUM_LIGHTS_FLOAT
    135,  // VSC_BONEMATRIXSTARTREGISTER
    1,  // VSC_WAVEPARAMS
    1,  // VSC_EDGEALPHAPARAMS
    1,  // VSC_PLAYERPOS
    1,  // VSC_SCREEN_Z
    1,  // VSC_VIEW_POS
    1,  // VSC_TIME
    1,  // VSC_MESHLIGHTING
    1,  // VSC_FACTOR
    1,  // VSC_SHADOWCOLOR
    1,  // VSC_SPECULARPOWER
    1,  // VSC_LIGHTCOLOR
    1,  // VSC_FILTERWEIGHTS1
    1,  // VSC_FILTERWEIGHTS2
    1,  // VSC_FILTERWEIGHTS3
    1,  // VSC_FILTERWEIGHTS4
    1,  // VSC_WAVE_CONSTANTS
    1,  // VSC_FOGCOLOR
    1,  // VSC_DIFFUSEOVERLAY
    1,  // VSC_APPEARANCE_FX
    1,  // VSC_DEPTH_OF_FIELD
    1,  // VSC_CAMERA_PARAMS
    1,  // VSC_DEPTH_TEXTURE_RECONSTRUCTION
    1,  // VSC_EDGE_FILTER
    1,  // VSC_BLUR_CONSTANTS
    1,  // VSC_WATER_TEXTURE_CONSTANTS
    1,  // VSC_HITFLASHADDCOLOR
    1,  // VSC_HITFLASHMULCOLOR
    1,  // VSC_TINT_RAMP_UV
    1,  // VSC_HEROTINTCOLOR_B_DEPRECATED
    1,  // VSC_WEATHER_INTENSITY
    1,  // VSC_BATCHES
    1,  // VSC_RAND_FLOAT
    1,  // VSC_USE_DYE_TYPE
    1,  // VSC_DEFORMATION_SCALE
    1,  // VSC_COLOR_CORRECTION_BLEND
    1,  // VSC_TARGET_DIMENSIONS
    1,  // VSC_NORMALMAPSCALE
    1,  // VSC_BANNER_TINT_RAMP_UV
    1,  // VSC_BANNER_PATTERNTINT_DEPRECATED
    1,  // VSC_BANNER_SIGILTINT_DEPRECATED
    1,  // VSC_BANNER_PATTERNDUOTONETINT_DEPRECATED
    1,  // VSC_DBG_REG0
    1,  // VSC_LIGHTMAP_V_SCALE
    1,  // VSC_VEC_FIXED_POINT_POS_SCALE
    1,  // VSC_VEC_FIXED_POINT_POS_OFFSET
    4,  // VSC_MATRIX_WORLD_IRRAD
    1,  // VSC_ALPHATEST_REF
    1,  // VSC_QCAMERA_BILLBOARD
    1,  // VSC_BILLBOARD_PARAMS
    4,  // VSC_MATRIX_PROJECTION
    6,  // VSC_VEC_CLIP_PLANE
    2,  // VSC_VEC_REFLECTION_PLANE
    4,  // VSC_MATRIX_EFFECT1
    4,  // VSC_MATRIX_EFFECT2
    4,  // VSC_MATRIX_SSAO_VIEW_PROJ
    4,  // VSC_MATRIX_COLOR
};

// line 340. Nothing left in the binary reads it: GetDefaultPass stores these values field by field.
const TextureStageParams cgtStageParamsDefault = {
    SHADERTEX_DIFFUSE, TEXTUREADDRESSMODE_CLAMP, TEXTUREADDRESSMODE_CLAMP, TEXTUREADDRESSMODE_CLAMP, FILTERMODE_BILINEAR, 0.0f,
};

// The field names are "" in the binary.
static const TypeDescriptorField RenderParams_Fields[25] = {  // line 381
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eCullMode), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fZBufferWriteEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eZBufferCompareFunc), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RenderParams, flZBias), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RenderParams, flZSlopeScaleBias), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fStencilBufferEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eStencilBufferCompareFunc), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, nStencilBufferRefValue), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eStencilOpPass), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eStencilOpFail), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eStencilOpZFail), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fAlphaTestEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eAlphaTestCompareFunc), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(RenderParams, bAlphaTestRefValue), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fAlphaMaskEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fFogEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eFillMode), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fColorWriteEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fAlphaWriteEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, fAlphaBlendEnable), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eBlendOp), (const void *)1, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eSrcBlendFactor), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderParams, eDestBlendFactor), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(RenderParams, rgbaTexFactor), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RenderParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RenderParams("RenderParams", 2, RenderParams_Fields, 25, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RENDERPARAMS = &TypeDescriptor_RenderParams;  // line 406

static const TypeDescriptorField TextureStageParams_Fields[7] = {  // line 408
    TypeDescriptorField("", DT_INT, offsetof(TextureStageParams, eShaderTex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureStageParams, eTexAddressModeU), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureStageParams, eTexAddressModeV), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureStageParams, eTexAddressModeW), (const void *)1, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureStageParams, eFilterMode), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TextureStageParams, flMipMapBias), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TextureStageParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TextureStageParams("TextureStageParams", 1, TextureStageParams_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TEXTURESTAGEPARAMS = &TypeDescriptor_TextureStageParams;  // line 415

static const TypeDescriptorField RenderPass_Fields[19] = {  // line 417
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, eRenderLayer), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, eVertexPreprocessStep), NULL, 1),
    TypeDescriptorField("", DT_RENDERPARAMS, offsetof(RenderPass, tRenderParams), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, dwPad), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RenderPass, sertsParams), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_TEXTURESTAGEPARAMS, offsetof(RenderPass, ptsParams), 0x21,
                                     (int32)offsetof(RenderPass, sertsParams) - (int32)offsetof(RenderPass, ptsParams)),
    VariableArrayCountTypeDescriptorField("", offsetof(RenderPass, dwStageCount), 0x10000, DT_TEXTURESTAGEPARAMS,
                                          (int32)offsetof(RenderPass, sertsParams) - (int32)offsetof(RenderPass, dwStageCount)),
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RenderPass, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(RenderPass, hTagMap), 0x21,
                              (int32)offsetof(RenderPass, serTagMap) - (int32)offsetof(RenderPass, hTagMap), TAG_SHADEROPTION),
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, bPostProcessed), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, dwPad2), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RenderPass, serVertexShader), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(RenderPass, pVertexShader), 0x700020,
                                     (int32)offsetof(RenderPass, serVertexShader) - (int32)offsetof(RenderPass, pVertexShader)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RenderPass, serPixelShader), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(RenderPass, pPixelShader), 0x700020,
                                     (int32)offsetof(RenderPass, serPixelShader) - (int32)offsetof(RenderPass, pPixelShader)),
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, dwVSKey), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(RenderPass, dwPSKey), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RenderPass), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RenderPass("RenderPass", RENDERPASS_XML_VERSION, RenderPass_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RENDERPASS = &TypeDescriptor_RenderPass;  // line 441

static const TypeDescriptorField ShaderDefinition_Fields[8] = {  // line 443
    TypeDescriptorField("", DT_INT, offsetof(ShaderDefinition, dwRenderLayers), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ShaderDefinition, dwPad), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ShaderDefinition, serPasses), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_RENDERPASS, offsetof(ShaderDefinition, ptPasses), 0x21,
                                     (int32)offsetof(ShaderDefinition, serPasses) - (int32)offsetof(ShaderDefinition, ptPasses)),
    VariableArrayCountTypeDescriptorField("", offsetof(ShaderDefinition, nPassCount), 0x10000, DT_RENDERPASS,
                                          (int32)offsetof(ShaderDefinition, serPasses) - (int32)offsetof(ShaderDefinition, nPassCount)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ShaderDefinition, serConsoleShaderData), NULL, 0x700000),
    TypeDescriptorField("", DT_INT64, offsetof(ShaderDefinition, pConsoleShaderData), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ShaderDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ShaderDefinition("Shaders", SHADER_XML_VERSION, ShaderDefinition_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SHADER = &TypeDescriptor_ShaderDefinition;  // line 451

ShaderGroupDef * gptGroupDefShader;  // line 457

// line 467
ShaderGroupDef::ShaderGroupDef()
    : GroupDefinition("Shaders", SHADER_SNO,
                      SNOVersions(SHADER_PC_SNO_VERSION, 160, SHADER_XBOXONE_SNO_VERSION, SHADER_PS4_SNO_VERSION, 187),
                      SNODATATYPE_TITLE, sizeof(ShaderDefinition), DT_SHADER, 0x2011, 0x1a) {
}

// line 491
uint32 ShaderGroupDef::GetMaxCount() {
    return 1024;
}

// line 499
void ShaderGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(ShaderDefinition));

    ShaderDefinition * pdef = (ShaderDefinition *)ptDefaultDef;
    pdef->dwRenderLayers |= (1 << RENDERLAYER_OPAQUE);
    pdef->nPassCount = 1;
    pdef->serPasses.dwSize = sizeof(RenderPass) * pdef->nPassCount;
    pdef->ptPasses = (RenderPass *)SigmaMemoryNew(pdef->serPasses.dwSize, 4, NULL, FALSE);

    RenderPass & tPass = pdef->ptPasses[0];
    GetDefaultPass(tPass);
}

// line 520
void ShaderGroupDef::GetDefaultPass(RenderPass & tPass) const {
    SigmaMemoryClear(&tPass, sizeof(RenderPass));

    tPass.eRenderLayer = RENDERLAYER_OPAQUE;
    tPass.hTagMap = TagMapCreate(TAG_SHADEROPTION);

    {
        RenderParams & tRenderParams = tPass.tRenderParams;
        tRenderParams.eCullMode = CULLMODE_CCW;
        tRenderParams.fZBufferWriteEnable = TRUE;
        tRenderParams.eZBufferCompareFunc = COMPARE_LESSEQUAL;
        tRenderParams.flZBias = 0.0f;
        tRenderParams.flZSlopeScaleBias = 0.0f;
        tRenderParams.fStencilBufferEnable = FALSE;
        tRenderParams.eStencilBufferCompareFunc = COMPARE_LESSEQUAL;
        tRenderParams.nStencilBufferRefValue = 0;
        tRenderParams.eStencilOpPass = STENCILOP_KEEP;
        tRenderParams.eStencilOpFail = STENCILOP_KEEP;
        tRenderParams.eStencilOpZFail = STENCILOP_KEEP;
        tRenderParams.fAlphaTestEnable = FALSE;
        tRenderParams.eAlphaTestCompareFunc = COMPARE_GREATER;
        tRenderParams.bAlphaTestRefValue = 0;
        tRenderParams.fAlphaMaskEnable = FALSE;
        tRenderParams.fFogEnable = FALSE;
        tRenderParams.eFillMode = FILLMODE_SOLID;
        tRenderParams.fColorWriteEnable = TRUE;
        tRenderParams.fAlphaWriteEnable = FALSE;
        tRenderParams.fAlphaBlendEnable = FALSE;
        tRenderParams.eBlendOp = BLENDOP_ADD;
        tRenderParams.eSrcBlendFactor = BLENDFACTOR_SRCALPHA;
        tRenderParams.eDestBlendFactor = BLENDFACTOR_INVSRCALPHA;
        tRenderParams.rgbaTexFactor = crgbaBlack;
    }

    tPass.dwStageCount = 1;
    tPass.sertsParams.dwSize = sizeof(TextureStageParams) * tPass.dwStageCount;
    tPass.ptsParams = (TextureStageParams *)SigmaMemoryNew(tPass.sertsParams.dwSize, 4, NULL, FALSE);

    for (uint32 i = 0; i < tPass.dwStageCount; i++) {
        TextureStageParams & tsParams = tPass.ptsParams[i];
        tsParams.eShaderTex = SHADERTEX_DIFFUSE;
        tsParams.eTexAddressModeU = TEXTUREADDRESSMODE_CLAMP;
        tsParams.eTexAddressModeV = TEXTUREADDRESSMODE_CLAMP;
        tsParams.eTexAddressModeW = TEXTUREADDRESSMODE_CLAMP;
        tsParams.eFilterMode = FILTERMODE_BILINEAR;
        tsParams.flMipMapBias = 0.0f;
    }
}

// line 585
BOOL ShaderGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    return FALSE;
}

// line 867
void ShaderGroupDef::WriteExternalDef(const SNO sno, void * lpDefinition) {
    GroupDefinition::WriteExternalDef(sno, lpDefinition);

    GfxForceDeviceReset(FALSE);
}

// line 879
void ShaderGroupDef::DeletePass(RenderPass & tPass) const {
    if (tPass.bPostProcessed) {
        GfxShaderStateBlockDispose(tPass.tPostprocessed.ptStateBlock);
        tPass.bPostProcessed = FALSE;
    }

    TagMapDispose(tPass.hTagMap);
}

// line 911
void ShaderGroupDef::Delete(void * lpDefinition) const {
    ShaderDefinition * pdef = (ShaderDefinition *)lpDefinition;

    for (int32 i = 0; i < pdef->nPassCount; i++) {
        RenderPass & tPass = pdef->ptPasses[i];
        if (tPass.bPostProcessed) {
            GfxShaderStateBlockDispose(tPass.tPostprocessed.ptStateBlock);
            GfxDeleteVertexShader(pdef, i);
            GfxDeletePixelShader(pdef, i);
            tPass.bPostProcessed = FALSE;
        }
    }

    SigmaMemoryFree(pdef->pConsoleShaderData, NULL);
    pdef->pConsoleShaderData = NULL;

    GroupDefinition::Delete(lpDefinition);
}

// line 945
static void sShaderDefinitionForceLoad(ShaderDefinition * pdef) {
    for (int32 i = 0; i < pdef->nPassCount; i++) {
        RenderPass & tPass = pdef->ptPasses[i];
        if (tPass.bPostProcessed)
            continue;

        tPass.bPostProcessed = TRUE;
        GfxCreateVertexShader(pdef, i);
        GfxCreatePixelShader(pdef, i);
        GfxShaderStateBlockCreate(pdef, i, tPass.tPostprocessed.ptStateBlock);
    }

    SigmaMemoryFree(pdef->pConsoleShaderData, NULL);
    pdef->pConsoleShaderData = NULL;
}

// line 1034
void ShaderDefinitionForceLoad(const ShaderDefinition * pdefShader, int32 nPassIndex) {
    if (!pdefShader->ptPasses[nPassIndex].bPostProcessed)
        sShaderDefinitionForceLoad((ShaderDefinition *)pdefShader);
}

// line 1048
static void sDoEarlyOpaquePassRedirection(ShaderDefinition * ptDef) {
    BOOL fUsesStandardOpaque = FALSE;
    BOOL fUsesEarlyOpaque = FALSE;

    for (int i = 0; i < ptDef->nPassCount; i++) {
        RenderPass & tPass = ptDef->ptPasses[i];
        if (tPass.eRenderLayer != RENDERLAYER_OPAQUE)
            continue;

        if (tPass.tRenderParams.fAlphaBlendEnable) {
            fUsesStandardOpaque = TRUE;
        } else {
            // Three unconditional loads (one ldp + ldr feeding a ccmp chain) in the original; a plain
            // && of the fields branches on each, so they are read into locals first. The names are
            // invented.
            BOOL fColorWrite = tPass.tRenderParams.fColorWriteEnable;
            BOOL fZWrite = tPass.tRenderParams.fZBufferWriteEnable;
            BOOL fAlphaWrite = tPass.tRenderParams.fAlphaWriteEnable;
            if (fColorWrite && fZWrite && !fAlphaWrite) {
                tPass.eRenderLayer = RENDERLAYER_EARLY_OPAQUE;
                fUsesEarlyOpaque = TRUE;
            } else {
                fUsesStandardOpaque = TRUE;
            }
        }
    }

    if (fUsesStandardOpaque)
        ptDef->dwRenderLayers |= (1 << RENDERLAYER_OPAQUE);
    else
        ptDef->dwRenderLayers &= ~(1 << RENDERLAYER_OPAQUE);

    if (fUsesEarlyOpaque)
        ptDef->dwRenderLayers |= (1 << RENDERLAYER_EARLY_OPAQUE);
    else
        ptDef->dwRenderLayers &= ~(1 << RENDERLAYER_EARLY_OPAQUE);
}

// line 1095
void ShaderGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    ShaderDefinition * pdef = (ShaderDefinition *)lpDefinition;
    if (!(pdef->tHeader.dwFlags & (1 << DH_IS_EXTERNAL_BIT)))
        sShaderDefinitionForceLoad(pdef);

    if (pdef->dwRenderLayers & (1 << RENDERLAYER_OPAQUE))
        sDoEarlyOpaquePassRedirection(pdef);
}

// line 1661
uint32 ShaderGroupDef::GetContiguousReadSize(SNOElement * ptSNOElement) {
    ShaderDefinition tShaderDef;
    ReadDefinitionHeader(ptSNOElement, &tShaderDef);

    uint32 dwContiguousSize = tShaderDef.serConsoleShaderData.dwOffset;
    return dwContiguousSize;
}

// line 1680
void ShaderGroupDef::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer,
                          uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const {
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);

    ShaderDefinition * pdef = (ShaderDefinition *)lpDefinition;
    pdef->pConsoleShaderData = (unsigned char *)SigmaMemoryNew(pdef->serConsoleShaderData.dwSize, 4, NULL, FALSE);
    SNOFileRead(ptSNOElement, pdef->serConsoleShaderData, pdef->pConsoleShaderData);
}
