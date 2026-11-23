#include "NX64NVN.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include <string.h>

// line 17
void GFXNX64NVN::FogEnable(BOOL fEnabled)
{
}

// line 25
void GFXNX64NVN::FogSetColor(const RGBAColor & rgbaFogColor)
{
}

// line 35
static nvn::DepthFunc ToNVNDepthFunc(CompareFunc compareMode)
{
    nvn::DepthFunc nvnDepthFuncTable[COMPARE_SIZE] = {
        nvn::DepthFunc::ALWAYS,    // COMPARE_UNKNOWN
        nvn::DepthFunc::NEVER,     // COMPARE_NEVER
        nvn::DepthFunc::LESS,      // COMPARE_LESS
        nvn::DepthFunc::EQUAL,     // COMPARE_EQUAL
        nvn::DepthFunc::LEQUAL,    // COMPARE_LESSEQUAL
        nvn::DepthFunc::GREATER,   // COMPARE_GREATER
        nvn::DepthFunc::NOTEQUAL,  // COMPARE_NOTEQUAL
        nvn::DepthFunc::GEQUAL,    // COMPARE_GREATEREQUAL
        nvn::DepthFunc::ALWAYS,    // COMPARE_ALWAYS
    };

    return nvnDepthFuncTable[compareMode];
}

// line 52
void GFXNX64NVN::ZBufferSetCompareFunc(CompareFunc eCompareFunc)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    if (renderWorkerGlobals.tState.eDepthCompareFunc != eCompareFunc) {
        BOOL depthTestEnabled = (eCompareFunc != COMPARE_ALWAYS) || renderWorkerGlobals.tState.bDepthWriteEnabled;
        renderWorkerGlobals.tState.tDepthStencilState.SetDepthTestEnable(depthTestEnabled).SetDepthFunc(ToNVNDepthFunc(eCompareFunc));
        renderWorkerGlobals.tState.eDepthCompareFunc = eCompareFunc;
        renderWorkerGlobals.tState.tFlags.DepthStateDirty = 1;
    }
}

// line 68
void GFXNX64NVN::ZBufferWriteEnable(BOOL fEnabled)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    if (renderWorkerGlobals.tState.bDepthWriteEnabled != fEnabled) {
        renderWorkerGlobals.tState.tDepthStencilState.SetDepthWriteEnable(fEnabled);
        renderWorkerGlobals.tState.bDepthWriteEnabled = fEnabled;
        renderWorkerGlobals.tState.tFlags.DepthStateDirty = 1;
    }
}

// line 85
void GFXNX64NVN::StencilBufferEnable(const BOOL fEnabled)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    if (renderWorkerGlobals.tState.bStencilEnabled != fEnabled) {
        renderWorkerGlobals.tState.tDepthStencilState.SetStencilTestEnable(fEnabled);
        renderWorkerGlobals.tState.bStencilEnabled = fEnabled;
        renderWorkerGlobals.tState.tFlags.StencilStateDirty = 1;
    }
}

// line 100
static nvn::StencilFunc ToNVNStencilFunc(CompareFunc compareMode)
{
    nvn::StencilFunc nvnStencilFuncTable[COMPARE_SIZE] = {
        nvn::StencilFunc::ALWAYS,    // COMPARE_UNKNOWN
        nvn::StencilFunc::NEVER,     // COMPARE_NEVER
        nvn::StencilFunc::LESS,      // COMPARE_LESS
        nvn::StencilFunc::EQUAL,     // COMPARE_EQUAL
        nvn::StencilFunc::LEQUAL,    // COMPARE_LESSEQUAL
        nvn::StencilFunc::GREATER,   // COMPARE_GREATER
        nvn::StencilFunc::NOTEQUAL,  // COMPARE_NOTEQUAL
        nvn::StencilFunc::GEQUAL,    // COMPARE_GREATEREQUAL
        nvn::StencilFunc::ALWAYS,    // COMPARE_ALWAYS
    };

    return nvnStencilFuncTable[compareMode];
}

// line 117
static nvn::StencilOp ToNVNStencilOp(StencilOp stencilOp)
{
    nvn::StencilOp nvnStencilOpTable[STENCILOP_SIZE] = {
        nvn::StencilOp::KEEP,       // STENCILOP_UNKNOWN
        nvn::StencilOp::KEEP,       // STENCILOP_KEEP
        nvn::StencilOp::ZERO,       // STENCILOP_ZERO
        nvn::StencilOp::REPLACE,    // STENCILOP_REPLACE
        nvn::StencilOp::INCR,       // STENCILOP_INCRSAT
        nvn::StencilOp::DECR,       // STENCILOP_DECRSAT
        nvn::StencilOp::INVERT,     // STENCILOP_INVERT
        nvn::StencilOp::INCR_WRAP,  // STENCILOP_INCR
        nvn::StencilOp::DECR_WRAP,  // STENCILOP_DECR
    };

    return nvnStencilOpTable[stencilOp];
}

// line 134
void GFXNX64NVN::StencilBufferSetParams(const CompareFunc eCompareFunc, const int32 nRefValue, const StencilOp eOpPass, const StencilOp eOpFail, const StencilOp eOpZFail)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    if (renderWorkerGlobals.tState.eStencilCompareFunc != eCompareFunc ||
        renderWorkerGlobals.tState.nStencilRefValue != nRefValue ||
        renderWorkerGlobals.tState.eStencilOpPass != eOpPass ||
        renderWorkerGlobals.tState.eStencilOpFail != eOpFail ||
        renderWorkerGlobals.tState.eStencilOpZFail != eOpZFail) {
        renderWorkerGlobals.tState.tDepthStencilState.SetStencilFunc(nvn::Face::FRONT_AND_BACK, ToNVNStencilFunc(eCompareFunc));
        renderWorkerGlobals.tState.tDepthStencilState.SetStencilOp(nvn::Face::FRONT_AND_BACK, ToNVNStencilOp(eOpFail), ToNVNStencilOp(eOpZFail), ToNVNStencilOp(eOpPass));
        renderWorkerGlobals.tState.eStencilCompareFunc = eCompareFunc;
        renderWorkerGlobals.tState.nStencilRefValue = nRefValue;
        renderWorkerGlobals.tState.eStencilOpPass = eOpPass;
        renderWorkerGlobals.tState.eStencilOpFail = eOpFail;
        renderWorkerGlobals.tState.eStencilOpZFail = eOpZFail;
        renderWorkerGlobals.tState.tFlags.StencilStateDirty = 1;
    }
}

// line 164
void GFXNX64NVN::AlphaTestEnable(const BOOL fEnabled)
{
}

// line 172
void GFXNX64NVN::AlphaTestSetParams(const CompareFunc eCompareFunc, const uint8 bRefValue)
{
}

// line 183
void GFXNX64NVN::AlphaMaskEnable(const BOOL fEnabled)
{
}

// line 194
void GFXNX64NVN::SetTextureAddressMode(const uint32 dwStage, TextureAddressMode eTAModeU, TextureAddressMode eTAModeV, TextureAddressMode eTAModeW)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    NVNShaderStateBlock::SamplerSetup & curSampler = renderWorkerGlobals.tState.tSamplerSetups[dwStage];

    if (curSampler.eTAModeU != eTAModeU || curSampler.eTAModeV != eTAModeV || curSampler.eTAModeW != eTAModeW) {
        curSampler.eTAModeU = eTAModeU;
        curSampler.eTAModeV = eTAModeV;
        curSampler.eTAModeW = eTAModeW;
        renderWorkerGlobals.tState.tFlags.PSTextureResourcesDirty |= (1 << dwStage);
    }
}

// line 217
static NVNFilterMode sConvertToNVNFilterMode(FilterMode filterMode)
{
    NVNFilterMode reducedFilterModeArray[FILTERMODE_SIZE] = {
        NVN_FILTERMODE_POINT_SAMPLED,  // FILTERMODE_UNKNOWN
        NVN_FILTERMODE_POINT_SAMPLED,  // FILTERMODE_POINT_SAMPLED
        NVN_FILTERMODE_POINT_SAMPLED,  // FILTERMODE_UNUSED
        NVN_FILTERMODE_BILINEAR,       // FILTERMODE_BILINEAR
        NVN_FILTERMODE_TRILINEAR,      // FILTERMODE_TRILINEAR
        NVN_FILTERMODE_TRILINEAR,      // FILTERMODE_ANISOTROPIC
        NVN_FILTERMODE_POINT_SAMPLED,  // FILTERMODE_PREF
        NVN_FILTERMODE_POINT_SAMPLED,  // FILTERMODE_QUINCUNX
        NVN_FILTERMODE_POINT_SAMPLED,  // FILTERMODE_PCF
    };

    return reducedFilterModeArray[filterMode];
}

// line 237
void GFXNX64NVN::SetFilterMode(const uint32 dwStage, FilterMode eFilterMode)
{
    sSetFilterMode(dwStage, sConvertToNVNFilterMode(eFilterMode));
}

// line 247
inline void GFXNX64NVN::sSetFilterMode(const uint32 dwStage, NVNFilterMode eNvnFilterMode)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    NVNShaderStateBlock::SamplerSetup & curSampler = renderWorkerGlobals.tState.tSamplerSetups[dwStage];

    if (curSampler.eNVNFilterMode != eNvnFilterMode) {
        curSampler.eNVNFilterMode = eNvnFilterMode;
        renderWorkerGlobals.tState.tFlags.PSTextureResourcesDirty |= (1 << dwStage);
    }
}

// line 264
void GFXNX64NVN::SetMipMapBias(const uint32 dwStage, float flMipMapBias)
{
    int32 iMipBias;
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    NVNShaderStateBlock::SamplerSetup & curSampler = renderWorkerGlobals.tState.tSamplerSetups[dwStage];

    iMipBias = Math::FloatRoundToInt(-flMipMapBias);
    iMipBias = Math::Pin(iMipBias, 0, 3);

    if (curSampler.nMipBiasIndex != iMipBias) {
        curSampler.nMipBiasIndex = (int8)iMipBias;
        renderWorkerGlobals.tState.tFlags.PSTextureResourcesDirty |= (1 << dwStage);
    }
}

// line 286
void GFXNX64NVN::SetWriteMasks(const BOOL fColorWriteEnable, const BOOL fAlphaWriteEnable)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    NVNState & rState = renderWorkerGlobals.tState;

    if (rState.bColorWriteEnabled != fColorWriteEnable || rState.bAlphaWriteEnabled != fAlphaWriteEnable) {
        rState.bColorWriteEnabled = fColorWriteEnable;
        rState.bAlphaWriteEnabled = fAlphaWriteEnable;
        rState.tFlags.ChannelMaskStateDirty = 1;
    }
}

// line 306
void GFXNX64NVN::SetBlendFactors(const BOOL fAlphaBlendEnable, const BlendFactor eSrcBlendFactor, const BlendFactor eDestBlendFactor, const BlendOp eBlendOp)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    NVNState & rState = renderWorkerGlobals.tState;

    if (rState.bBlendingEnabled != fAlphaBlendEnable ||
        rState.eSourceBlendFactor != eSrcBlendFactor ||
        rState.eDestBlendFactor != eDestBlendFactor ||
        rState.eBlendOp != eBlendOp) {
        rState.bBlendingEnabled = fAlphaBlendEnable;
        rState.eSourceBlendFactor = eSrcBlendFactor;
        rState.eDestBlendFactor = eDestBlendFactor;
        rState.eBlendOp = eBlendOp;
        rState.tFlags.BlendStateDirty = 1;
    }
}

// line 332
void GFXNX64NVN::SetZBias(const float flZBias)
{
}

// line 341
void GFXNX64NVN::SetCullMode(CullMode eCullMode)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    NVNState & rState = renderWorkerGlobals.tState;

    if (rState.eCullMode != eCullMode) {
        rState.eCullMode = eCullMode;
        rState.tFlags.PolygonStateDirty = 1;
    }
}

// line 357
void GFXNX64NVN::SetFillMode(FillMode eFillMode)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    NVNState & rState = renderWorkerGlobals.tState;

    if (rState.eFillMode != eFillMode) {
        rState.eFillMode = eFillMode;
        rState.tFlags.PolygonStateDirty = 1;
    }
}

// line 372
void GFXNX64NVN::SetClipPlane(int32 nIndex, const float * pflPlane)
{
}

// line 384
void GFXNX64NVN::ViewportSet(const IRect2D & rectViewport)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    if (rectViewport != renderWorkerGlobals.tState.tViewportRect) {
        renderWorkerGlobals.tState.tViewportRect = rectViewport;
        renderWorkerGlobals.tState.tFlags.ViewportDirty = 1;
    }
}

// line 403
void GFXNX64NVN::ScissorRectSet(const BOOL fEnable, const IRect2D & rectScissor)
{
}

// line 416
void GFXNX64NVN::StateBlockBegin()
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    renderWorkerGlobals.tState.ClearShaderStateBlock();
    renderWorkerGlobals.tState.bInStateBlock = TRUE;
}

// line 429
void GFXNX64NVN::StateBlockEnd(XPtr64<unsigned char> & ptStateBlock)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    NVNShaderStateBlock * ptStateBlockTyped = new NVNShaderStateBlock; // Snapshot only the 0x220-byte base; NVNState adds dirty flags and live bindings.
    memcpy(ptStateBlockTyped, (NVNShaderStateBlock *)&renderWorkerGlobals.tState, sizeof(NVNShaderStateBlock));
    ptStateBlock = (unsigned char *)ptStateBlockTyped; // Opaque API handle; StateBlockApply casts back to the same snapshot type.

    renderWorkerGlobals.tState.bInStateBlock = FALSE;
}

// line 445
void GFXNX64NVN::StateBlockApply(XPtr64<unsigned char> ptStateBlock)
{
    NVNShaderStateBlock * ptStateBlockTyped = (NVNShaderStateBlock *)ptStateBlock.Get();

    ZBufferWriteEnable(ptStateBlockTyped->bDepthWriteEnabled);
    ZBufferSetCompareFunc(ptStateBlockTyped->eDepthCompareFunc);

    StencilBufferEnable(ptStateBlockTyped->bStencilEnabled);
    StencilBufferSetParams(ptStateBlockTyped->eStencilCompareFunc, ptStateBlockTyped->nStencilRefValue, ptStateBlockTyped->eStencilOpPass, ptStateBlockTyped->eStencilOpFail, ptStateBlockTyped->eStencilOpZFail);

    SetBlendFactors(ptStateBlockTyped->bBlendingEnabled, ptStateBlockTyped->eSourceBlendFactor, ptStateBlockTyped->eDestBlendFactor, ptStateBlockTyped->eBlendOp);
    SetWriteMasks(ptStateBlockTyped->bColorWriteEnabled, ptStateBlockTyped->bAlphaWriteEnabled);

    SetFillMode(ptStateBlockTyped->eFillMode);
    SetCullMode(ptStateBlockTyped->eCullMode);

    for (int stage = 0; stage < 24; stage++) {
        const NVNShaderStateBlock::SamplerSetup & curSampler = ptStateBlockTyped->tSamplerSetups[stage];

        SetTextureAddressMode(stage, curSampler.eTAModeU, curSampler.eTAModeV, curSampler.eTAModeW);
        sSetFilterMode(stage, curSampler.eNVNFilterMode);
        SetMipMapBias(stage, (float)-curSampler.nMipBiasIndex);
    }
}

// line 477
void GFXNX64NVN::StateBlockDispose(XPtr64<unsigned char> & ptStateBlock)
{
    NVNShaderStateBlock * ptStateBlockTyped = (NVNShaderStateBlock *)ptStateBlock.Get();

    delete ptStateBlockTyped;
    ptStateBlock.Null();
}
