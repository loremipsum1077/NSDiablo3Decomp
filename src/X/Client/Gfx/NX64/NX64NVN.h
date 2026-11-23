// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/NX64/NX64NVN.h
//
// Trimmed to what GfxVB.cpp, NX64NVNShaders.cpp and NX64NVNRenderStates.cpp use.
// GFXNX64NVN::Globals keeps its 2.6.2 size and the offsets of the members those files read; the
// members nothing reads yet are padding (named pad_*, not original names).

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../NintendoSDK/Include/nvn/nvn_Cpp.h"
#include "../../../../NintendoSDK/Include/nn/vi/vi_Types.h"
#include "../../Definitions/ShaderCommon.h"
#include "../GfxCommon.h"
#include "../GfxVB.h"
#include "../../../../Contrib/Contrib/blz/include/blz/atomic.h"
#include "../../../../Contrib/Contrib/blz/include/blz/mutex.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Sigma/DataStructures/XArray.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/DataStructures/XPtr64.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include <stddef.h>
#include <stdint.h>

struct GLSLCoutput_rec;
struct IVector2D;
struct OcclusionQuery;
struct TextureInitialData;
struct DisplayMode;
struct DisplayCaps;
struct GfxOptions;
struct GfxDeviceDesc;
struct NVNTexInfo;
struct GFXNVNBuffer;
struct ShaderData;
struct ShaderDefinition;
struct SerializeData;
struct SNOElement;
class XRemoteHeap;

enum NVNCBuffer {
    NVNCBUFFER_MAIN = 0,
    NVNCBUFFER_BONES = 1,
    NVNCBUFFER_LIGHTS = 2,
    NVNCBUFFER_COUNT = 3,
};

enum NVNSemantic {
    NVNSEMANTIC_POSITION = 0,
    NVNSEMANTIC_NORMAL = 1,
    NVNSEMANTIC_COLOR_0 = 2,
    NVNSEMANTIC_COLOR_1 = 3,
    NVNSEMANTIC_TEXCOORD_0 = 4,
    NVNSEMANTIC_TEXCOORD_1 = 5,
    NVNSEMANTIC_TEXCOORD_2 = 6,
    NVNSEMANTIC_TEXCOORD_3 = 7,
    NVNSEMANTIC_TEXCOORD_4 = 8,
    NVNSEMANTIC_TEXCOORD_5 = 9,
    NVNSEMANTIC_TANGENT = 10,
    NVNSEMANTIC_BINORMAL = 11,
    NVNSEMANTIC_BLEND_INDICES = 12,
    NVNSEMANTIC_BLEND_WEIGHTS = 13,
    NVNSEMANTIC_COUNT = 14,
};

enum NVNFilterMode {
    NVN_FILTERMODE_POINT_SAMPLED = 0,
    NVN_FILTERMODE_BILINEAR = 1,
    NVN_FILTERMODE_TRILINEAR = 2,
    NVN_FILTERMODE_SIZE = 3,
};

enum NVNAllocType {
    NVNALLOCTYPE_PERSISTENT = 0,
    NVNALLOCTYPE_TRANSIENT = 1,
};

// Defined in NX64NVNCore.cpp:2763 and :2788. The DWARF has no NVNMemoryAllocator type, only
// these two functions, so it is written as a namespace.
namespace nn { namespace vi { class Display; class Layer; } }  // declarations only in the DWARF (nn/vi)

namespace NVNMemoryAllocator {
void * Allocate(size_t nSize, size_t nAlign, NVNAllocType eType);
void Free(void * pMem, NVNAllocType eType);
}

struct NVNGraphicsMemory {  // size 0x38
    enum MemType {
        MEMTYPE_RENDER_TARGET = 0,
        MEMTYPE_COMMAND_BUFFER = 1,
        MEMTYPE_GPU_EVENTS = 2,
        MEMTYPE_STATIC_BUFFER = 3,
        MEMTYPE_DYNAMIC_BUFFER = 4,
        MEMTYPE_DESCRIPTOR_POOL = 5,
        MEMTYPE_TEXTURE_RESOURCE = 6,
    };
    NVNGraphicsMemory();
    ~NVNGraphicsMemory();
    void Init(size_t nSize, size_t nAlign, NVNGraphicsMemory::MemType eType);
    void Init(void * ptProvidedMemory, size_t nSize, NVNGraphicsMemory::MemType eType);
    void Dispose();
    NVNGraphicsMemory::MemType GetMemType() const;
    static NVNAllocType GetAllocationType(NVNGraphicsMemory::MemType eType);
    static nvn::MemoryPoolFlags GetPoolFlags(NVNGraphicsMemory::MemType eType);
    void * ptMemory;  // 0x00
    size_t nAllocSize;  // 0x08
    ptrdiff_t nAllocOffset;  // 0x10
    nvn::MemoryPool * ptMemoryPool;  // 0x18
    nvn::BufferAddress tNVNBufferAddress;  // 0x20
private:
    NVNGraphicsMemory(const NVNGraphicsMemory &);
    const NVNGraphicsMemory & operator=(const NVNGraphicsMemory &);
    void Allocate(size_t nSize, size_t nAlign, NVNGraphicsMemory::MemType eType);
    void CreateGPUMapping();
    NVNGraphicsMemory::MemType eMemType;  // 0x28
    BOOL bOwnsMemory;  // 0x2c
    BOOL bOwnsMemoryPool;  // 0x30
};
static_assert(sizeof(NVNGraphicsMemory) == 0x38, "NVNGraphicsMemory");

struct NVNGraphicsLinearAllocator {  // size 0x40
    NVNGraphicsLinearAllocator() : nCurOffset(0) {}  // line 154
    void Init(size_t nSize, size_t nAlign, NVNGraphicsMemory::MemType eType);
    void Dispose();
    ptrdiff_t Allocate(size_t nSize, size_t nAlign);
    void * GetOffsetAddress(ptrdiff_t offset);
    ptrdiff_t GetAddressOffset(void * pAddr);
    nvn::BufferAddress GetGpuAddressFromCpuAddress(void * pCpuAddress);
    void ClearAllocations();
    nvn::MemoryPool * GetMemoryPool() { return tMemContainer.ptMemoryPool; }  // line 167; body inferred from NX64NVNEngine.cpp sFlushNVNBuffer
    NVNGraphicsMemory tMemContainer;  // 0x00
private:
    ptrdiff_t nCurOffset;  // 0x38
};
static_assert(sizeof(NVNGraphicsLinearAllocator) == 0x40, "NVNGraphicsLinearAllocator");

enum NVNTexType {
    NVNTEXTYPE_2D = 0,
    NVNTEXTYPE_CUBEMAP = 1,
    NVNTEXTYPE_3D = 2,
    NVNTEXTYPE_2D_ALIAS = 3,
};

struct NVNTexSwizzle {  // size 0x10
    NVNTexSwizzle() : R(nvn::TextureSwizzle::R), G(nvn::TextureSwizzle::G), B(nvn::TextureSwizzle::B), A(nvn::TextureSwizzle::A) {}  // lines 232-236
    nvn::TextureSwizzle R;  // 0x00
    nvn::TextureSwizzle G;  // 0x04
    nvn::TextureSwizzle B;  // 0x08
    nvn::TextureSwizzle A;  // 0x0c
};
static_assert(sizeof(NVNTexSwizzle) == 0x10, "NVNTexSwizzle");

struct NVNTexInfo {  // size 0x78
    NVNTexInfo();
    ~NVNTexInfo();
    TextureFormat eTexFormat;  // 0x00
    uint32 dwMSAALevel;  // 0x04
    uint32 dwLastFrameUsed;  // 0x08
    NVNTexType eType;  // 0x0c
    uint32 dwPixelWidth;  // 0x10
    uint32 dwPixelHeight;  // 0x14
    uint32 dwPixelDepth;  // 0x18
    uint32 dwMipLevels;  // 0x1c
    NVNGraphicsMemory tMemory;  // 0x20
    BOOL bTargetUpdated[1];  // 0x58
    BOOL bIsWindowTexture;  // 0x5c
    int nTextureID;  // 0x60
    int nReducedTextureID;  // 0x64
    enum TexSelection {
        TEXSELECTION_FULL = 0,
        TEXSELECTION_REDUCED = 1,
    };
    static const float kReducedFraction;
    enum TexDeviceResolutionFraction {
        TEXFRACTION_NOT_RELATED = 0,
        TEXFRACTION_FULL = 1,
        TEXFRACTION_HALF = 2,
        TEXFRACTION_QUARTER = 3,
    };
    void CreateNVNTexture(BOOL bLinearLayout, BOOL bRenderTarget, BOOL bSwapChainTarget, BOOL bDepthTarget);
    nvn::Texture * GetTexture(NVNTexInfo::TexSelection eSelection);
    int GetTextureID(NVNTexInfo::TexSelection eSelection) const;
    BOOL IsRenderTarget() const;
    BOOL HasReducedRenderTarget() const;
    NVNTexInfo::TexDeviceResolutionFraction GetDeviceResolutionFraction() const;
    // Private in the original; GFXNX64NVN reads them directly (a friend there).
    nvn::Texture * ptTexture;  // 0x68
    nvn::Texture * ptReducedTexture;  // 0x70
};
static_assert(sizeof(NVNTexInfo) == 0x78, "NVNTexInfo");

struct NVNSampler {  // size 0x68
    nvn::Sampler tSampler;  // 0x00
    int32 nSamplerID;  // 0x60
};
static_assert(sizeof(NVNSampler) == 0x68, "NVNSampler");

// X/Client/Gfx/NX64/NX64NVNCore.cpp:31
struct OcclusionQuery : public nvn::CounterData {  // size 0x10
};
static_assert(sizeof(OcclusionQuery) == 0x10, "OcclusionQuery");

// X/Client/Gfx/NX64/NX64NVNCore.cpp:2211
uint32 CalculateBlockOffset(const NVNTexInfo * pTexInfo, uint32 xBlock, uint32 yBlock);

struct NVNVertexShader {  // size 0x100
    nvn::Program tProgram;  // 0x00
    nvn::Buffer tCommandData;  // 0xc0
    void * pControlData;  // 0xf0
    uint64 qwInputBindings;  // 0xf8
    static const uint32 ShaderStage = 0;
};
static_assert(sizeof(NVNVertexShader) == 0x100, "NVNVertexShader");

struct NVNPixelShader {  // size 0xf8
    nvn::Program tProgram;  // 0x00
    nvn::Buffer tCommandData;  // 0xc0
    void * pControlData;  // 0xf0
    static const uint32 ShaderStage = 1;
};
static_assert(sizeof(NVNPixelShader) == 0xf8, "NVNPixelShader");

struct NVNShaderAllocator {  // size 0x108
    NVNShaderAllocator() : ptHeap(NULL) {}  // line 266
    void Create(XRemoteHeap * pHeap);
    void Destroy();
    ptrdiff_t Allocate(size_t nSize, size_t nAlign);
    void Free(ptrdiff_t allocation);
    void * GetBaseAddressCpu();
    void * GetCpuAddressForAllocation(ptrdiff_t allocationOffset);
    nvn::MemoryPool tMemoryPool;  // 0x00
    XRemoteHeap * ptHeap;  // 0x100
};
static_assert(sizeof(NVNShaderAllocator) == 0x108, "NVNShaderAllocator");

struct GFXNVNBuffer {  // size 0x58
    uint32 dwSize : 24;  // 0x00
    uint32 fIsDynamic : 1;  // 0x00
    uint32 fIsProxy : 1;  // 0x00
    uint32 eFormat : 6;  // 0x00
    uint32 dwLastFrameUsed;  // 0x04
    uint32 dwNextOffsetToLock[1];  // 0x08
    NVNGraphicsMemory tMemory;  // 0x10
    nvn::BufferAddress tBufferAddr;  // 0x48
    uint8 * pDynamicBuffers[1];  // 0x50
};
static_assert(sizeof(GFXNVNBuffer) == 0x58, "GFXNVNBuffer");

struct NVNShaderStateBlock {  // size 0x220
    void ClearShaderStateBlock();
    CompareFunc eDepthCompareFunc;  // 0x00
    BOOL bDepthWriteEnabled;  // 0x04
    BOOL bStencilEnabled;  // 0x08
    CompareFunc eStencilCompareFunc;  // 0x0c
    int32 nStencilRefValue;  // 0x10
    StencilOp eStencilOpPass;  // 0x14
    StencilOp eStencilOpFail;  // 0x18
    StencilOp eStencilOpZFail;  // 0x1c
    BOOL bBlendingEnabled;  // 0x20
    BlendOp eBlendOp;  // 0x24
    BlendFactor eSourceBlendFactor;  // 0x28
    BlendFactor eDestBlendFactor;  // 0x2c
    BOOL bColorWriteEnabled;  // 0x30
    BOOL bAlphaWriteEnabled;  // 0x34
    CullMode eCullMode;  // 0x38
    FillMode eFillMode;  // 0x3c
    struct SamplerSetup {  // size 0x14
        TextureAddressMode eTAModeU;  // 0x00
        TextureAddressMode eTAModeV;  // 0x04
        TextureAddressMode eTAModeW;  // 0x08
        NVNFilterMode eNVNFilterMode;  // 0x0c
        int8 nMipBiasIndex;  // 0x10
    };
    NVNShaderStateBlock::SamplerSetup tSamplerSetups[24];  // 0x40
};
static_assert(sizeof(NVNShaderStateBlock) == 0x220, "NVNShaderStateBlock");

struct NVNState : public NVNShaderStateBlock {  // size 0x378
    void Clear();
    void DirtyAllTextureResources() { tFlags.PSTextureResourcesDirty = (1 << 24) - 1; }  // line 348; one bit per ptPSTextureResources slot
    uint64 GetVertexInputBindingKey();
    struct Wrapper {  // size 0xc
        uint32 VertexShaderDirty : 1;  // 0x00
        uint32 PixelShaderDirty : 1;  // 0x00
        uint32 VertexStreamsDirty : 1;  // 0x00
        uint32 VertexBindingDirty : 1;  // 0x00
        uint32 ColorBufferBindingsDirty : 1;  // 0x00
        uint32 DepthBufferBindingsDirty : 1;  // 0x00
        uint32 ViewportDirty : 1;  // 0x00
        uint32 DepthStateDirty : 1;  // 0x00
        uint32 StencilStateDirty : 1;  // 0x00
        uint32 BlendStateDirty : 1;  // 0x00
        uint32 ChannelMaskStateDirty : 1;  // 0x00
        uint32 PolygonStateDirty : 1;  // 0x00
        uint32 NeedRenderTargetBarrier : 1;  // 0x00
        uint32 PSTextureResourcesDirty;  // 0x04
        uint8 ConstantBuffersDirty;  // 0x08
    };
    NVNState::Wrapper tFlags;  // 0x220
    NVNTexInfo * ptCurrentRenderTarget[2];  // 0x230
    NVNTexInfo * ptCurrentDepthTarget;  // 0x240
    ShaderData * ptVertexShader;  // 0x248
    ShaderData * ptPixelShader;  // 0x250
    NVNTexInfo * ptPSTextureResources[24];  // 0x258
    GFXNVNBuffer * hvbCurrent[3];  // 0x318
    uint32 dwCurrentStreamOffset[3];  // 0x330
    GFXNVNBuffer * hibCurrent;  // 0x340
    VBFormat eStreamVBFormat[3];  // 0x348
    IRect2D tViewportRect;  // 0x354
    nvn::DepthStencilState tDepthStencilState;  // 0x368
    BOOL bInStateBlock;  // 0x370
};
static_assert(sizeof(NVNState) == 0x378, "NVNState");

struct NVNVertexInputBindingSet {  // size 0x10
    // line 395
    NVNVertexInputBindingSet(uint32 dwNum)
        : dwNumAttributes(dwNum),
          ptAttribStates((nvn::VertexAttribState *)SigmaMemoryNew(dwNum * sizeof(nvn::VertexAttribState), 4, NULL, FALSE)) {}
    // line 401
    ~NVNVertexInputBindingSet() { SigmaMemoryFree(ptAttribStates, NULL); }
    uint32 dwNumAttributes;  // 0x00
    nvn::VertexAttribState * ptAttribStates;  // 0x08
};
static_assert(sizeof(NVNVertexInputBindingSet) == 0x10, "NVNVertexInputBindingSet");

typedef XGrowableMap<unsigned long, unsigned long, NVNVertexInputBindingSet *, NVNVertexInputBindingSet *> NVNVertexAttribMap;

class GFXNX64NVN {  // size 0x1048
public:
    // X/Client/Gfx/NX64/NX64NVNCore.cpp:1002
    // Non-virtual: the caller reaches it with a direct `bl _ZN10GFXNX64NVN7DisposeEv`
    // (GfxAPI.cpp:122), and the class has no vtable pointer at offset 0x00.
    BOOL Dispose();
    BOOL VBCreate(VBFormat eVBFormat, HVB * phvb, uint32 dwVertexCount, const ResourceUsage eUsage,
                  const void * pVBInitialData, LPCSTR szVBDebugName, uint32 & hResultRet);
    BOOL VBCreateProxy(VBFormat eVBFormat, HVB * phvb, uint32 dwVertexCount, LPCSTR szVBDebugName);
    void VBSetProxy(HVB hvb, const void * ptSrc);
    void VBDispose(HVB hvb);
    void * VBLock(HVB hvb, uint32 dwLockSize, uint32 & dwStartOffset);
    void * VBLockAll(HVB hvb);
    void VBUnlock(HVB hvb);
    void VBFlush(HVB hvb);
    void VBSetStream(const uint32 dwIndex, const HVB hvb, const uint32 dwStreamOffset, const VBFormat eVBFormat);
    BOOL VBIsBusy(HVB hvb);
    BOOL IBCreate(HIB * phib, uint32 dwSize, const ResourceUsage eUsage, const uint16 * pIBInitialData,
                  LPCSTR szIBDebugName, uint32 & hResultRet);
    BOOL IBCreateProxy(HIB * phib, uint32 dwSize, LPCSTR szIBDebugName);
    void IBSetProxy(HIB hib, uint16 * pwSrc);
    void IBDispose(HIB hib);
    uint16 * IBLock(HIB hib, uint32 dwLockSize, uint32 & dwStartOffset);
    uint16 * IBLockAll(HIB hib);
    void IBUnlock(HIB hib);
    void IBFlush(HIB hib);
    void IBSetCurrent(HIB hib);
    BOOL IBIsBusy(HIB hib);
    void CreateVertexShader(ShaderDefinition * pdefShader, const int32 nPassIndex);
    void DeleteVertexShader(ShaderData * ptVSD);
    BOOL VertexShaderIsBusy(ShaderData * ptVSD);
    void CreatePixelShader(ShaderDefinition * pdefShader, const int32 nPassIndex);
    void DeletePixelShader(ShaderData * ptPSD);
    BOOL PixelShaderIsBusy(ShaderData * ptPSD);
    void SetPixelShader(const ShaderDefinition * pdefShader, const int32 nPassIndex);
    void SetPixelShaderConstant(const ShaderConstBinding * ptBinding, const void * lpData, const uint32 dwSize);
    void SetVertexShader(const ShaderDefinition * pdefShader, const int32 nPassIndex);
    void SetVertexDeclaration();
    void SetVertexShaderConstant(const ShaderConstBinding * ptBinding, const void * lpData, const uint32 dwSize);
    const ShaderConstFastBinding * GetShaderConstantFastBindings();
    void ViewportSet(const IRect2D & rectViewport);
    void ScissorRectSet(const BOOL fEnable, const IRect2D & rectScissor);
    void FogEnable(BOOL fEnabled);
    void FogSetColor(const RGBAColor & rgbaFogColor);
    void ZBufferSetCompareFunc(CompareFunc eCompareFunc);
    void ZBufferWriteEnable(BOOL fEnabled);
    void StencilBufferEnable(const BOOL fEnabled);
    void StencilBufferSetParams(const CompareFunc eCompareFunc, const int32 nRefValue, const StencilOp eOpPass, const StencilOp eOpFail, const StencilOp eOpZFail);
    void AlphaTestEnable(const BOOL fEnabled);
    void AlphaTestSetParams(const CompareFunc eCompareFunc, const uint8 bRefValue);
    void AlphaMaskEnable(const BOOL fEnabled);
    void SetTextureAddressMode(const uint32 dwStage, TextureAddressMode eTAModeU, TextureAddressMode eTAModeV, TextureAddressMode eTAModeW);
    void SetFilterMode(const uint32 dwStage, FilterMode eFilterMode);
    void SetMipMapBias(const uint32 dwStage, float flMipMapBias);
    void SetWriteMasks(const BOOL fColorWriteEnable, const BOOL fAlphaWriteEnable);
    void SetBlendFactors(const BOOL fAlphaBlendEnable, const BlendFactor eSrcBlendFactor, const BlendFactor eDestBlendFactor, const BlendOp eBlendOp);
    void SetZBias(const float flZBias);
    void SetCullMode(CullMode eCullMode);
    void SetFillMode(FillMode eFillMode);
    void SetClipPlane(int32 nIndex, const float * pflPlane);
    void StateBlockBegin();
    void StateBlockEnd(XPtr64<unsigned char> & ptStateBlock);
    void StateBlockApply(XPtr64<unsigned char> ptStateBlock);
    void StateBlockDispose(XPtr64<unsigned char> & ptStateBlock);
    void LoadAllMipLevels(SNOElement * ptSNOElement, void *, uint32, HTexture hTexture, const SerializeData & serTexSrc);
    int32 GetTiledElementByteOffset(HTexture hTexture, int32 nElementX, int32 nElementY);  // NX64NVNCore.cpp:2265
    // Back-end entry points Gfx.cpp calls (NX64NVNCore.cpp unless noted).
    BOOL Detect();
    static void EarlyInit();
    BOOL Init(GfxOptions * ptOptions, VBFormatInfo * ptVBFormatInfo);
    void GetValidDisplayModes(DisplayMode *& ptDisplayModes, uint32 & dwDisplayModeCount);
    void GetDeviceDescription(GfxDeviceDesc & desc);
    BOOL GetCurrentDisplayMode(DisplayMode & tDisplayMode);
    void GetValidTextureFormats(BOOL * pfTextureFormatValid);
    uint32 GetShadowTech();
    void GetCaps(DisplayCaps & tCaps);
    void EvictResources();
    void GammaSet(const float flGammaFactor);
    BOOL CreateSurface(const DisplayMode & tMode);
    BOOL CloseSurface();
    BOOL ChangeResolution(const DisplayMode & tMode);
    void SetRenderTargetToSwapChainRT(const uint32 dwRTIndex);
    void RenderClear(BOOL fClearTarget, BOOL fClearZBuffer, BOOL fClearStencilBuffer, const RGBAColor & rgbaTargetColor);
    void DeviceGetState(BOOL & fCanRender, BOOL & fMustBeReset, BOOL & fCanBeReset);
    BOOL DeviceReset();
    BOOL RenderBegin();
    void RenderEnd();
    void ResetStates();
    BOOL RenderPresent();
    uint32 DeviceFrameTimeUs();
    void RenderSuspend();
    void RenderResume();
    void BlockUntilIdle();
    void AcquireThreadOwnership();
    void ReleaseThreadOwnership();
    BOOL TextureCreate(HTexture * phTexture, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32 dwMSAALevel, const uint32 dwTargetERAMOffset, const int32 nExponentBias, const TextureInitialData * pInitialData, const char * szTexDebugName);
    void TextureDispose(HTexture hTexture);
    BOOL TextureLock(HTexture hTexture, uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwPitch, const uint32 dwFlags);
    void TextureUnlock(HTexture hTexture, uint32 dwMipMapLevel);
    void TextureSetSwapChainRenderTarget(HTexture hTexture);
    void TextureSetRenderTarget(const uint32 dwRTIndex, HTexture hTexture);
    void TextureSetDepthTarget(HTexture hTexture, BOOL fReadOnly);
    void TextureCopyDepthTarget(HTexture hTexture);
    BOOL TextureMSAASupported();
    void TextureResolveMSAARenderTarget(const uint32 dwRTIndex, const HTexture hDestTexture);
    void TextureResolveMSAADepthTarget(const HTexture hDestTexture);
    BOOL TextureIsBusy(HTexture hTexture);
    void GetRenderTargetCurrentResolution(HTexture hTexture, uint32 & width, uint32 & height);
    BOOL CubeMapCreate(HTexture * phTexture, const TextureFormat eTexFormat, const uint32 dwEdgeWidth, const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32 dwTargetERAMOffset, const int32 nExponentBias, const TextureInitialData * pTextureInitialData, const char * szTexDebugName);
    BOOL CubeMapLock(HTexture hTexture, CubeMapFace eCubeMapFace, uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwPitch, const uint32 dwGfxFlags);
    void CubeMapUnlock(HTexture hTexture, CubeMapFace eCubeMapFace, uint32 dwMipMapLevel);
    void CubeMapSetRenderTarget(const uint32 dwRTIndex, HTexture hTexture, const CubeMapFace eCubeMapFace);
    BOOL VolumeTexCreate(HTexture * phTexture, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwDepth, const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const TextureInitialData * pTextureInitialData, const char * szTexDebugName);
    BOOL VolumeTexLock(HTexture hTexture, const uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwRowPitch, uint32 & dwSlicePitch, const uint32 dwGfxFlags);
    void VolumeTexUnlock(HTexture hTexture, const uint32 dwMipMapLevel);
    void Draw(const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount);
    void SetNumBones(int32 nNumBones);
    void BeginWorker(const int32 nWorkerIndex);
    void EndWorker();
    void SetShaderGPRAllocation(const uint32 dwVertexShaderCount, const uint32 dwPixelShaderCount);
    void RestoreShaderGPRAllocation();
    void TextureSetCurrent(const uint32 dwStage, HTexture hTexture);
    void ScreenGrab(void * lpDestBuffer, const int32 nDestWidth, const int32 nDestHeight, const int32 nDestPitch, const BOOL fBlitCursor);
    void CursorSet(HTexture hTexture, const IVector2D & pHotSpot, const uint32 dwFrame);
    BOOL Stereo3DEnabled();
    // Private in the original (NX64NVN.h); public here, access does not change the code.
    static void GetDeviceResolution(uint32 & dwDeviceWidth, uint32 & dwDeviceHeight);
    static void GetReducedDeviceResolution(uint32 & dwReducedDeviceWidth, uint32 & dwReducedDeviceHeight);
    static void RegisterTexture(NVNTexInfo & tTexInfo);  // NX64NVNCore.cpp:3410
    static NVNTexInfo::TexSelection sSelectRenderTargetAlias();
    void sSetRenderTarget(const uint32 dwRTIndex, NVNTexInfo * ptRenderTarget);
    void sApplyRenderTargetState(NVNState & tState);  // NX64NVNEngine.cpp
    void sApplyState(NVNState & rState);  // NX64NVNEngine.cpp
    void sCreateGlobalStateObjects();  // NX64NVNEngine.cpp
    void sFlushNVNBuffer(const GFXNVNBuffer * ptBuffer);  // NX64NVNEngine.cpp
    void * sLockNVNBuffer(GFXNVNBuffer * ptNVNBuffer, uint32 dwLockSize, uint32 & dwStartOffset, BOOL bIsVB);  // NX64NVNEngine.cpp
    void sSetDepthTarget(NVNTexInfo * ptDepthTarget);
    void sFlushNVNTexture(const NVNTexInfo * ptTexInfo);
    void sCopyTarget(NVNTexInfo * ptSrcTex, NVNTexInfo * ptDestTex);
    void sPrepareTargetForWrite(const NVNTexInfo * ptTexInfo);
    uint8 * sDynamicBufferAlloc(uint32 dwSize, uint32 dwAlign);
    nvn::BufferAddress sGetDynamicBufferAddress(uint8 * pDynamicBuffer);
    OcclusionQuery * OcclusionQueryCreate();
    void OcclusionQueryBegin(OcclusionQuery * ptQuery);
    void OcclusionQueryEnd(OcclusionQuery * ptQuery);
    BOOL OcclusionQueryGetResult(OcclusionQuery * ptQuery, uint32 & nResult);
    void OcclusionQueryDispose(OcclusionQuery * ptQuery);
    BOOL OcclusionQueryCompleted(OcclusionQuery * ptQuery);
private:
    NVNVertexShader * sCreateNVNVertexShader(GLSLCoutput_rec * pShaderBinary);
    NVNPixelShader * sCreateNVNPixelShader(GLSLCoutput_rec * pShaderBinary);
    NVNVertexInputBindingSet * sGetVertexAttributeBindings(uint64 qwKey);
    void sSetFilterMode(const uint32 dwStage, NVNFilterMode eNvnFilterMode);
public:
    // Declared private in the original; public here so the file-scope helpers that the
    // original reached them from (NVNShaderAllocator::Create) compile without a friend list.
    struct RenderCommandContext {  // size 0xa0
        void Init();
        void Dispose();
        void Reset();
        nvn::CommandBuffer tCommandBuffer;  // 0x00
    };
    struct CommandBufferMemoryRing {  // size 0x848
        static const uint32 kNumCommandBlocks = 256;
        static const uint32 kNumControlBlocks = 256;
        CommandBufferMemoryRing();
        void Init();
        void Dispose();
        void EndFrame();
        void AddCommandMemory(nvn::CommandBuffer & commandBuffer);
        void AddControlMemory(nvn::CommandBuffer & commandBuffer);
        NVNGraphicsMemory tCommandMemory;  // 0x00
        void * pControlMemory;  // 0x38
        blz::atomic<unsigned int> dwCurCommandBlock;  // 0x40
        blz::atomic<unsigned int> dwCurControlBlock;  // 0x44
        uint32 CommandBlockFrameFence[256];  // 0x48
        uint32 ControlBlockFrameFence[256];  // 0x448
    };
    void sGammaCorrection();
    struct RenderWorkerGlobals;
    struct GpuTimer {  // size 0x28
        enum CounterState {
            COUNTERSTATE_INVALID = -1,
            COUNTERSTATE_DONE = 0,
            COUNTERSTATE_INPROGRESS = 1,
        };
        GpuTimer() : ptStartCounter(NULL), ptEndCounter(NULL), ptIdleChild(NULL) { pName = NULL; dwDepth = 0; }  // lines 1274-1279
        void Init();
        uint32 GetTimeDeltaUs(BOOL bSubtractIdleTime);
        void Dispose();
        void ResetFrameData();
        void Stop(GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals);
        void Start(const char * pTimerName, GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals, uint32 dwTimerDepth);
        void AddIdleChild(GFXNX64NVN::GpuTimer * ptChild);
        static nvn::BufferAddress GetCounterAddress(nvn::CounterData * ptCounter);
        const char * pName;  // 0x00
        uint32 dwDepth;  // 0x08
        nvn::CounterData * ptStartCounter;  // 0x10
        nvn::CounterData * ptEndCounter;  // 0x18
        GFXNX64NVN::GpuTimer * ptIdleChild;  // 0x20
    };
    struct FrameProfile {  // size 0x5010
        static const uint32 TimerCount = 512;
        FrameProfile() : dwNumTimersUsed(0), tActiveTimerStack(NULL) {}  // lines 1312-1314
        void Init();
        uint32 GetFrameTimeUs();
        void Dispose();
        void Reset();
        void StopTimer(GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals);
        void StartTimer(const char * pName, GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals, BOOL bIdleTime);
        GFXNX64NVN::GpuTimer tTimers[512];  // 0x00
        uint32 dwNumTimersUsed;  // 0x5000
        blz::vector<GFXNX64NVN::GpuTimer *, blz::allocator<GFXNX64NVN::GpuTimer *> > * tActiveTimerStack;  // 0x5008
    };
    static int32 sGpuTimerWriteIndex();
    static int32 sGpuTimerReadIndex();
    void sInitGpuTimers();
    void sInitNVN();
    static void sInitNVMemory();
    static void sInitVideo();
    static void sInitDevice();
    static void sCreateCommandQueue();
    void sInitConstantBufferMemory();
    void sCreateOcclusionQueries();
    void sDestroyOcclusionQueries();
    void sCreateSwapChain();
    void sReleaseGpuTimers();
    static void sUpdateGpuTimers();
    struct IDManager {  // size 0xc8
        void Init();
        void Dispose();
        void RegisterTexture(NVNTexInfo & tTexInfo);
        void UnregisterTexture(NVNTexInfo & tTexInfo);
        void RegisterSampler(NVNSampler & tSampler);
        blz::atomic<int> dwCurrentTextureID;  // 0x00
        blz::atomic<int> dwCurrentSamplerID;  // 0x04
        blz::mutex * tTextureIdFreeListLock;  // 0x08
        blz::vector<int, blz::allocator<int> > * tTextureIdFreeList;  // 0x10
        nvn::TexturePool tTexturePool;  // 0x18
        nvn::SamplerPool tSamplerPool;  // 0x38
        NVNGraphicsMemory tTextureDescriptorMemory;  // 0x58
        NVNGraphicsMemory tSamplerDescriptorMemory;  // 0x90
    };
    struct Globals {  // size 0x15900
        uint32 dwDeviceWidth;  // 0x00
        uint32 dwDeviceHeight;  // 0x04
        uint32 dwReducedDeviceWidth;  // 0x08
        uint32 dwReducedDeviceHeight;  // 0x0c
        float flDeviceAspectRatio;  // 0x10
        int32 nPresentInterval;  // 0x14
        uint32 dwFrameCount;  // 0x18
        char pad_1c[0x20 - 0x1c];
        nvn::Event tProcessedFrameCount;  // 0x20
        VBFormatInfo * ptVBFormatInfo;  // 0x60
        NVNTexInfo tSwapChainBuffer[3];  // 0x68
        int32 nSwapChainBackBufferIndex;  // 0x1d0
        NVNTexInfo tMissing2DTexture;  // 0x1d8
        NVNSampler tGenericSampler;  // 0x250
        nn::vi::NativeWindowHandle tViNativeWindow;  // 0x2b8
        nn::vi::Display * ptViDisplay;  // 0x2c0
        nn::vi::Layer * ptViLayer;  // 0x2c8
        nvn::Device tDevice;  // 0x2d0
        nvn::Window tWindow;  // 0x32d0
        nvn::Sync tBackBufferSync;  // 0x3450
        BOOL bNeedToWaitForBackBufferSync;  // 0x3490
        char pad_3494[0x3498 - 0x3494];
        nvn::Queue tQueue;  // 0x3498
        void * pQueueMemory;  // 0x5498
        NVNGraphicsLinearAllocator tGPUEventsMemory;  // 0x54a0
        NVNShaderAllocator tShaderAllocator;  // 0x54e0
        GFXNX64NVN::CommandBufferMemoryRing tCommandBufferMemory;  // 0x55e8
        nvn::VertexStreamState tStreamStates[12];  // 0x5e30
        NVNVertexAttribMap * ptVertexAttribMap;  // 0x5e90
        uint32 dwConstantBufferSize[3];  // 0x5e98
        NVNGraphicsMemory tConstantBufferMemory;  // 0x5ea8
        nvn::BufferAddress tConstantBuffers[3];  // 0x5ee0
        NVNGraphicsLinearAllocator tDynamicBuffers[3];  // 0x5ef8
        NVNGraphicsLinearAllocator * ptDynamicBuffer;  // 0x5fb8
        GFXNX64NVN::IDManager tIDManager;  // 0x5fc0
        Blizzard::Lock::Mutex tWorkerLock;  // 0x6088
        int32 nCurrentProfileFrameIndex;  // 0x60a8
        char pad_60ac[0x60b0 - 0x60ac];
        GFXNX64NVN::FrameProfile tFrameProfiles[3];  // 0x60b0
        OcclusionQuery * pOcclusionQueryAllocation;  // 0x150e0
        XArray<OcclusionQuery *, OcclusionQuery *, 256> tFreeOcclusionQueries;  // 0x150e8
        float fGammaValue;  // 0x158f8
        BOOL bIsDocked;  // 0x158fc
    };
    struct RenderWorkerGlobals : public GFXNX64NVN::RenderCommandContext {  // size 0x1048
        void Init();
        void Reset();
        void BeginFrame();
        NVNState tState;  // 0xa0
        ShaderConstFastBinding ptConstFastBindingTable[86];  // 0x418
        uint8 * ppWorkingConstantBuffers[3];  // 0x1030
    };
    static GFXNX64NVN::Globals globals;
    GFXNX64NVN::RenderWorkerGlobals workerGlobals[1];  // 0x00
};
static_assert(sizeof(GFXNX64NVN) == 0x1048, "GFXNX64NVN");
