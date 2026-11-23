// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ShaderDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Gfx/GfxCommon.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class CTagMap;  // X/Common/TagMaps/Tags.h; only held through XPtr64 here
struct FileReference;
class XMemoryAllocatorInterface;

enum VertexPreprocessStep {
    VPS_NONE = 0,
};

enum {
    VSC_FILTERTAPS0 = 0,
    VSC_FILTERTAPS1 = 1,
    VSC_MATRIX_WORLD = 2,
    VSC_MATRIX_VIEW = 3,
    VSC_MATRIX_WV = 4,
    VSC_MATRIX_WVP = 5,
    VSC_FOGPARAMS = 6,
    VSC_MATRIX_TEX0 = 7,
    VSC_MATRIX_TEX1 = 8,
    VSC_MATRIX_TEX2 = 9,
    VSC_MATRIX_TEX3 = 10,
    VSC_MATRIX_TEX4 = 11,
    VSC_MATRIX_TEX5 = 12,
    VSC_MATRIX_TEX6 = 13,
    VSC_MATRIX_TEX7 = 14,
    VSC_MATRIX_TEX8 = 15,
    VSC_MATRIX_TEX9 = 16,
    VSC_MATRIX_TEX10 = 17,
    VSC_MATRIX_TEX11 = 18,
    VSC_VIEWPORT_UV_SCALE_BIAS = 19,
    VSC_WINDOW_RESOLUTION = 20,
    VSC_AMBIENT = 21,
    VSC_DIRECTIONALLIGHTS = 22,
    VSC_POINTLIGHTS = 23,
    VSC_SPOTLIGHTS = 24,
    VSC_CYLINDRICALLIGHTS = 25,
    VSC_POINTLINEARLIGHTS = 26,
    VSC_NUM_LIGHTS_INT = 27,
    VSC_NUM_LIGHTS_FLOAT = 28,
    VSC_BONEMATRIXSTARTREGISTER = 29,
    VSC_WAVEPARAMS = 30,
    VSC_EDGEALPHAPARAMS = 31,
    VSC_PLAYERPOS = 32,
    VSC_SCREEN_Z = 33,
    VSC_VIEW_POS = 34,
    VSC_TIME = 35,
    VSC_MESHLIGHTING = 36,
    VSC_FACTOR = 37,
    VSC_SHADOWCOLOR = 38,
    VSC_SPECULARPOWER = 39,
    VSC_LIGHTCOLOR = 40,
    VSC_FILTERWEIGHTS1 = 41,
    VSC_FILTERWEIGHTS2 = 42,
    VSC_FILTERWEIGHTS3 = 43,
    VSC_FILTERWEIGHTS4 = 44,
    VSC_WAVE_CONSTANTS = 45,
    VSC_FOGCOLOR = 46,
    VSC_DIFFUSEOVERLAY = 47,
    VSC_APPEARANCE_FX = 48,
    VSC_DEPTH_OF_FIELD = 49,
    VSC_CAMERA_PARAMS = 50,
    VSC_DEPTH_TEXTURE_RECONSTRUCTION = 51,
    VSC_EDGE_FILTER = 52,
    VSC_BLUR_CONSTANTS = 53,
    VSC_WATER_TEXTURE_CONSTANTS = 54,
    VSC_HITFLASHADDCOLOR = 55,
    VSC_HITFLASHMULCOLOR = 56,
    VSC_TINT_RAMP_UV = 57,
    VSC_HEROTINTCOLOR_B_DEPRECATED = 58,
    VSC_WEATHER_INTENSITY = 59,
    VSC_BATCHES = 60,
    VSC_RAND_FLOAT = 61,
    VSC_USE_DYE_TYPE = 62,
    VSC_DEFORMATION_SCALE = 63,
    VSC_COLOR_CORRECTION_BLEND = 64,
    VSC_TARGET_DIMENSIONS = 65,
    VSC_NORMALMAPSCALE = 66,
    VSC_BANNER_TINT_RAMP_UV = 67,
    VSC_BANNER_PATTERNTINT_DEPRECATED = 68,
    VSC_BANNER_SIGILTINT_DEPRECATED = 69,
    VSC_BANNER_PATTERNDUOTONETINT_DEPRECATED = 70,
    VSC_DBG_REG0 = 71,
    VSC_LIGHTMAP_V_SCALE = 72,
    VSC_VEC_FIXED_POINT_POS_SCALE = 73,
    VSC_VEC_FIXED_POINT_POS_OFFSET = 74,
    VSC_MATRIX_WORLD_IRRAD = 75,
    VSC_ALPHATEST_REF = 76,
    VSC_QCAMERA_BILLBOARD = 77,
    VSC_BILLBOARD_PARAMS = 78,
    VSC_MATRIX_PROJECTION = 79,
    VSC_VEC_CLIP_PLANE = 80,
    VSC_VEC_REFLECTION_PLANE = 81,
    VSC_MATRIX_EFFECT1 = 82,
    VSC_MATRIX_EFFECT2 = 83,
    VSC_MATRIX_SSAO_VIEW_PROJ = 84,
    VSC_MATRIX_COLOR = 85,
    NUM_VS_CONSTANT_ENTRIES = 86,
};

// Defined in ShaderDefinition.cpp (:57 and :247).
extern const LPCSTR cgszVSConstBindingTable[NUM_VS_CONSTANT_ENTRIES];
extern const int32 cgnVSConstSize[NUM_VS_CONSTANT_ENTRIES];

enum {
    VS_IS_FIXED_FUNCTION_BIT_DEPRECATED = 0,
    VS_REQUIRES_DYNAMIC_VBS_BIT = 1,
    VS_DOES_HARDWARE_SKINNING_BIT_DEPRECATED = 2,
    VS_NEEDS_TANGENT_BASIS_BIT_DEPRECATED = 3,
    VS_PIXEL_SHADER_DISABLED_BIT = 4,
};

enum {
    VSPP_DOES_HARDWARE_SKINNING_BIT = 0,
    VSPP_NEEDS_TANGENT_BASIS_BIT = 1,
};

struct ApplyableShaderConstBinding {  // size 0xa
    int8 nConstantIndex;  // 0x00
    uint8 uSize;  // 0x01
    int16 nCBuffer;  // 0x02
    int16 nRegister;  // 0x04
    uint16 wNumPatchOffsets;  // 0x06
    uint16 wFirstPatchOffset;  // 0x08
};
static_assert(sizeof(ApplyableShaderConstBinding) == 0xa, "ApplyableShaderConstBinding");

struct NX64ShaderProg {  // size 0x10
    XPtr64<unsigned char> ptAssembly;  // 0x00
    uint32 dwAssemblySize;  // 0x08
    uint32 dwLastFrameUsed;  // 0x0c
};
static_assert(sizeof(NX64ShaderProg) == 0x10, "NX64ShaderProg");

struct ShaderData {  // size 0x48
    uint32 dwKey;  // 0x00
    uint32 dwRefCount;  // 0x04
    uint8 dwNumDirLights;  // 0x08
    uint8 dwNumPointLights;  // 0x09
    uint8 dwNumSpotLights;  // 0x0a
    uint8 dwNumCylindricalLights;  // 0x0b
    uint8 dwNumPointLinearLights;  // 0x0c
    uint32 dwFlags;  // 0x10
    NX64ShaderProg tNX64Shader;  // 0x18
    void * lpSH;  // 0x28
    uint32 dwApplyableBindingCount;  // 0x30
    ApplyableShaderConstBinding * ptApplyBindingTable;  // 0x38
    uint32 dwShaderSize;  // 0x40
};
static_assert(sizeof(ShaderData) == 0x48, "ShaderData");

struct RenderParams {  // size 0x60
    CullMode eCullMode;  // 0x00
    BOOL fZBufferWriteEnable;  // 0x04
    CompareFunc eZBufferCompareFunc;  // 0x08
    float flZBias;  // 0x0c
    float flZSlopeScaleBias;  // 0x10
    BOOL fStencilBufferEnable;  // 0x14
    CompareFunc eStencilBufferCompareFunc;  // 0x18
    int32 nStencilBufferRefValue;  // 0x1c
    StencilOp eStencilOpPass;  // 0x20
    StencilOp eStencilOpFail;  // 0x24
    StencilOp eStencilOpZFail;  // 0x28
    BOOL fAlphaTestEnable;  // 0x2c
    CompareFunc eAlphaTestCompareFunc;  // 0x30
    uint8 bAlphaTestRefValue;  // 0x34
    BOOL fAlphaMaskEnable;  // 0x38
    BOOL fFogEnable;  // 0x3c
    FillMode eFillMode;  // 0x40
    BOOL fColorWriteEnable;  // 0x44
    BOOL fAlphaWriteEnable;  // 0x48
    BOOL fAlphaBlendEnable;  // 0x4c
    BlendOp eBlendOp;  // 0x50
    BlendFactor eSrcBlendFactor;  // 0x54
    BlendFactor eDestBlendFactor;  // 0x58
    RGBAColor rgbaTexFactor;  // 0x5c
};
static_assert(sizeof(RenderParams) == 0x60, "RenderParams");

struct TextureStageParams {  // size 0x18
    ShaderTex eShaderTex;  // 0x00
    TextureAddressMode eTexAddressModeU;  // 0x04
    TextureAddressMode eTexAddressModeV;  // 0x08
    TextureAddressMode eTexAddressModeW;  // 0x0c
    FilterMode eFilterMode;  // 0x10
    float flMipMapBias;  // 0x14
};
static_assert(sizeof(TextureStageParams) == 0x18, "TextureStageParams");

struct RenderPassPostData {  // size 0x18
    XPtr64<unsigned char> ptStateBlock;  // 0x00
    XPtr64<ShaderData> ptVSD;  // 0x08
    XPtr64<ShaderData> ptPSD;  // 0x10
};
static_assert(sizeof(RenderPassPostData) == 0x18, "RenderPassPostData");

struct ConsoleShaderPassInfo {  // size 0x10
    int32 nVertexShaderOffset;  // 0x00
    int32 nVertexShaderSize;  // 0x04
    int32 nPixelShaderOffset;  // 0x08
    int32 nPixelShaderSize;  // 0x0c
};
static_assert(sizeof(ConsoleShaderPassInfo) == 0x10, "ConsoleShaderPassInfo");

struct RenderPass {  // size 0xe0
    RenderLayer eRenderLayer;  // 0x00
    VertexPreprocessStep eVertexPreprocessStep;  // 0x04
    RenderParams tRenderParams;  // 0x08
    uint32 dwPad;  // 0x68
    uint32 dwStageCount;  // 0x6c
    SerializeData sertsParams;  // 0x70
    XPtr64<TextureStageParams> ptsParams;  // 0x78
    uint32 dwFlags;  // 0x80
    SerializeData serTagMap;  // 0x84
    XPtr64<CTagMap> hTagMap;  // 0x90
    RenderPassPostData tPostprocessed;  // 0x98
    BOOL bPostProcessed;  // 0xb0
    uint32 dwPad2;  // 0xb4
    SerializeData serVertexShader;  // 0xb8
    XPtr64<unsigned char> pVertexShader;  // 0xc0
    SerializeData serPixelShader;  // 0xc8
    XPtr64<unsigned char> pPixelShader;  // 0xd0
    uint32 dwVSKey;  // 0xd8
    uint32 dwPSKey;  // 0xdc
};
static_assert(sizeof(RenderPass) == 0xe0, "RenderPass");

struct ShaderDefinition {  // size 0x38
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwRenderLayers;  // 0x0c
    uint32 dwPad;  // 0x10
    int32 nPassCount;  // 0x14
    SerializeData serPasses;  // 0x18
    XPtr64<RenderPass> ptPasses;  // 0x20
    SerializeData serConsoleShaderData;  // 0x28
    XPtr64<unsigned char> pConsoleShaderData;  // 0x30
};
static_assert(sizeof(ShaderDefinition) == 0x38, "ShaderDefinition");

class ShaderGroupDef : public GroupDefinition {  // size 0xa0, line 378
public:
    ShaderGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 383; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 386
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 388
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void DeletePass(RenderPass & tPass) const;  // vtable slot 30
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    void GetDefaultPass(RenderPass & tPass) const;
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
    virtual void WriteExternalDef(const SNO sno, void * lpDefinition);  // vtable slot 24
    virtual uint32 GetContiguousReadSize(SNOElement * ptSNOElement);  // vtable slot 27
};
static_assert(sizeof(ShaderGroupDef) == 0xa0, "ShaderGroupDef");

extern ShaderGroupDef * gptGroupDefShader;  // ShaderDefinition.cpp

typedef ScopedDefinitionLockR<ShaderDefinition> ScopedShaderDefinitionLockR;
// Original inline specializations at line 463; shaders use the resident definition.
template <> inline const ShaderDefinition * DefinitionLockR<ShaderDefinition>(SNO sno) {
    return static_cast<const ShaderDefinition *>(gptGroupDefShader->LockRFast(sno));
}
template <> inline void DefinitionUnlockR<ShaderDefinition>(const ShaderDefinition *) {}

extern const XTypeDescriptor * DT_SHADER;  // ShaderDefinition.cpp:451

// Defined in ShaderDefinition.cpp:1034; the DWARF does not record where it was declared.
void ShaderDefinitionForceLoad(const ShaderDefinition * pdefShader, int32 nPassIndex);


// Functions defined in this file (bodies are not in the DWARF):
//   line   360  ShaderDefinition::ShaderDefinition()  [inlined into 1 functions]
//   line   378  ShaderGroupDef::~ShaderGroupDef()  [0x71001b4910]
//   line   383  void * ShaderGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x71001b4940 (+1 more)]
//   line   383  void ShaderGroupDef::MemFree(void *& ptr) const  [0x71001b4990 (+1 more)]
//   line   386  BOOL ShaderGroupDef::CanReferenceOtherSNOs() const  [0x71001b4a20]
//   line   388  SNOPackType ShaderGroupDef::GetSNOPackDestination(const void *) const  [0x71001b4a10]
//   line   463  const ShaderDefinition * DefinitionLockR<ShaderDefinition>(SNO sno)  [inlined into 25 functions]
