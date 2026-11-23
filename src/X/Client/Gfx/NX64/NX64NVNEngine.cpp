// Original file: X/Client/Gfx/NX64/NX64NVNEngine.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
// Statement shapes not in the DWARF were chosen by what compiles to the original code: the
// `for (worker < 1)` loops in VBCreate/IBCreate/VBLockAll/IBLockAll, the if + do-while around the
// texture loop in sApplyState, `default:` sharing the point-sampled case in
// sCreateGlobalStateObjects. MipBiasSamplerSlots (4) is the last dimension of SamplerStates.
#include "NX64NVN.h"
#include "NX64NVN.inl"
#include "../GfxVB.h"
#include "../../CGame.h"
#include "../../Definitions/ShaderDefinition.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"

extern VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];  // Gfx/Gfx.cpp


static const uint32 MipBiasSamplerSlots = 4;

static nvn::BlendState BlendStates[BLENDFACTOR_SIZE][BLENDFACTOR_SIZE][BLENDOP_SIZE];
static nvn::ColorState ColorStates[2];
static nvn::ChannelMaskState ChannelMaskStates[2][2];
static NVNSampler SamplerStates[TEXTUREADDRESSMODE_SIZE][TEXTUREADDRESSMODE_SIZE][TEXTUREADDRESSMODE_SIZE][NVN_FILTERMODE_SIZE][MipBiasSamplerSlots];
static nvn::PolygonState PolygonState[CULLMODE_SIZE][FILLMODE_SIZE];






BOOL GFXNX64NVN::VBCreate(VBFormat eVBFormat, HVB * phvb, uint32 dwVertexCount, const ResourceUsage eUsage,
                          const void * pVBInitialData, LPCSTR szVBDebugName, uint32 & hResultRet)






{

    hResultRet = 0;

    uint32 dwStride = cgtVBFormatInfo[eVBFormat].dwSize;
    uint32 dwOriginalSize = dwStride * dwVertexCount;
    uint32 dwSize = dwOriginalSize;

    GFXNVNBuffer * ptNVNBuffer = new GFXNVNBuffer;
    *phvb = ptNVNBuffer;

    ptNVNBuffer->fIsProxy = FALSE;
    ptNVNBuffer->eFormat = eVBFormat;
    ptNVNBuffer->fIsDynamic = (eUsage == RESOURCEUSAGE_DYNAMIC);
    ptNVNBuffer->dwSize = dwSize;
    NVNMarkResourceReadyForDelete(ptNVNBuffer);

    if (ptNVNBuffer->fIsDynamic)
    {
        for (int32 worker = 0; worker < 1; worker++)
        {
            ptNVNBuffer->dwNextOffsetToLock[worker] = dwSize;
            ptNVNBuffer->pDynamicBuffers[worker] = NULL;
        }
    }
    else
    {
        ptNVNBuffer->tMemory.Init(ptNVNBuffer->dwSize, 8, NVNGraphicsMemory::MEMTYPE_STATIC_BUFFER);
        ptNVNBuffer->tBufferAddr = ptNVNBuffer->tMemory.tNVNBufferAddress;
        for (int32 worker = 0; worker < 1; worker++)
        {
            ptNVNBuffer->dwNextOffsetToLock[worker] = 0;
            ptNVNBuffer->pDynamicBuffers[worker] = NULL;
        }
    }


    if (pVBInitialData)
    {
        void * ptBuffer = VBLockAll(ptNVNBuffer);
        if (ptBuffer)
        {
            SigmaMemoryCopy(ptBuffer, pVBInitialData, dwOriginalSize);
            VBUnlock(ptNVNBuffer);
        }
    }

    return TRUE;
}




BOOL GFXNX64NVN::VBCreateProxy(VBFormat eVBFormat, HVB * phvb, uint32 dwVertexCount, LPCSTR szVBDebugName)




{


    uint32 dwSize = cgtVBFormatInfo[eVBFormat].dwSize * dwVertexCount;

    GFXNVNBuffer * ptNVNBuffer = new GFXNVNBuffer;
    *phvb = ptNVNBuffer;

    ptNVNBuffer->dwSize = dwSize;
    ptNVNBuffer->fIsDynamic = FALSE;
    ptNVNBuffer->fIsProxy = TRUE;
    ptNVNBuffer->eFormat = eVBFormat;
    NVNMarkResourceReadyForDelete(ptNVNBuffer);
    ptNVNBuffer->tBufferAddr = 0;

    return TRUE;
}




void GFXNX64NVN::VBDispose(HVB hvb)
{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hvb;

    delete ptNVNBuffer;
}















void GFXNX64NVN::VBSetProxy(HVB hvb, const void * ptSrc)


{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hvb;


    ptNVNBuffer->tMemory.Init((void *)ptSrc, ptNVNBuffer->dwSize, NVNGraphicsMemory::MEMTYPE_STATIC_BUFFER);
    ptNVNBuffer->tBufferAddr = ptNVNBuffer->tMemory.tNVNBufferAddress;
    sFlushNVNBuffer(ptNVNBuffer);
}




void * GFXNX64NVN::VBLock(HVB hvb, uint32 dwLockSize, uint32 & dwStartOffset)



{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hvb;
    return sLockNVNBuffer(ptNVNBuffer, dwLockSize, dwStartOffset, TRUE);
}




void * GFXNX64NVN::VBLockAll(HVB hvb)

{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hvb;
    uint32 dwStartOffset;

    if (!ptNVNBuffer->fIsDynamic)
    {
        for (int32 worker = 0; worker < 1; worker++)
            ptNVNBuffer->dwNextOffsetToLock[worker] = 0;
    }


    return sLockNVNBuffer(ptNVNBuffer, ptNVNBuffer->dwSize, dwStartOffset, TRUE);
}




void GFXNX64NVN::VBFlush(HVB hvb)
{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hvb;

    for (int32 worker = 0; worker < 1; worker++)
    {
        ptNVNBuffer->dwNextOffsetToLock[worker] = ptNVNBuffer->fIsDynamic ? ptNVNBuffer->dwSize : 0;
    }
}




void GFXNX64NVN::VBUnlock(HVB hvb)

{
    sFlushNVNBuffer((GFXNVNBuffer *)hvb);
}




BOOL GFXNX64NVN::VBIsBusy(HVB hvb)

{
    return NVNIsResourceBusy((GFXNVNBuffer *)hvb);
}




void GFXNX64NVN::VBSetStream(const uint32 dwIndex, const HVB hvb, const uint32 dwStreamOffset, const VBFormat eVBFormat)
{
    RenderWorkerGlobals * renderWorkerGlobals = &workerGlobals[0];







    if (renderWorkerGlobals->tState.hvbCurrent[dwIndex] != hvb ||
        renderWorkerGlobals->tState.dwCurrentStreamOffset[dwIndex] != dwStreamOffset)
    {
        renderWorkerGlobals->tState.hvbCurrent[dwIndex] = (GFXNVNBuffer *)hvb;
        renderWorkerGlobals->tState.dwCurrentStreamOffset[dwIndex] = dwStreamOffset;
        renderWorkerGlobals->tState.tFlags.VertexStreamsDirty = 1;

        if (renderWorkerGlobals->tState.eStreamVBFormat[dwIndex] != eVBFormat)
        {
            renderWorkerGlobals->tState.eStreamVBFormat[dwIndex] = eVBFormat;
            renderWorkerGlobals->tState.tFlags.VertexBindingDirty = 1;
        }
    }







}





BOOL GFXNX64NVN::IBCreate(HIB * phib, uint32 dwSize, const ResourceUsage eUsage, const uint16 * pIBInitialData,
                          LPCSTR szIBDebugName, uint32 &)





{
    uint32 dwOriginalSize = dwSize;


    GFXNVNBuffer * ptNVNBuffer = new GFXNVNBuffer;
    *phib = ptNVNBuffer;

    ptNVNBuffer->dwSize = dwSize;
    ptNVNBuffer->fIsProxy = FALSE;
    ptNVNBuffer->fIsDynamic = (eUsage == RESOURCEUSAGE_DYNAMIC);
    ptNVNBuffer->eFormat = 0;
    NVNMarkResourceReadyForDelete(ptNVNBuffer);

    if (ptNVNBuffer->fIsDynamic)
    {
        for (int32 worker = 0; worker < 1; worker++)
        {
            ptNVNBuffer->dwNextOffsetToLock[worker] = dwSize;
            ptNVNBuffer->pDynamicBuffers[worker] = NULL;
        }
    }
    else
    {
        ptNVNBuffer->tMemory.Init(ptNVNBuffer->dwSize, 8, NVNGraphicsMemory::MEMTYPE_STATIC_BUFFER);
        ptNVNBuffer->tBufferAddr = ptNVNBuffer->tMemory.tNVNBufferAddress;
        for (int32 worker = 0; worker < 1; worker++)
        {
            ptNVNBuffer->dwNextOffsetToLock[worker] = 0;
            ptNVNBuffer->pDynamicBuffers[worker] = NULL;
        }
    }


    if (pIBInitialData)
    {
        void * ptBuffer = IBLockAll(ptNVNBuffer);
        if (ptBuffer)
        {
            SigmaMemoryCopy(ptBuffer, pIBInitialData, dwOriginalSize);
            IBUnlock(ptNVNBuffer);
        }
    }

    return TRUE;
}




BOOL GFXNX64NVN::IBCreateProxy(HIB * phib, uint32 dwSize, LPCSTR szIBDebugName)



{


    GFXNVNBuffer * ptNVNBuffer = new GFXNVNBuffer;
    *phib = ptNVNBuffer;

    ptNVNBuffer->dwSize = dwSize;
    ptNVNBuffer->fIsDynamic = FALSE;
    ptNVNBuffer->fIsProxy = TRUE;
    ptNVNBuffer->eFormat = 0;
    NVNMarkResourceReadyForDelete(ptNVNBuffer);
    ptNVNBuffer->tBufferAddr = 0;

    return TRUE;
}




void GFXNX64NVN::IBDispose(HIB hib)
{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hib;

    delete ptNVNBuffer;
}















void GFXNX64NVN::IBSetProxy(HIB hib, uint16 * pwSrc)


{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hib;


    ptNVNBuffer->tMemory.Init(pwSrc, ptNVNBuffer->dwSize, NVNGraphicsMemory::MEMTYPE_STATIC_BUFFER);
    ptNVNBuffer->tBufferAddr = ptNVNBuffer->tMemory.tNVNBufferAddress;
    sFlushNVNBuffer(ptNVNBuffer);
}




uint16 * GFXNX64NVN::IBLock(HIB hib, uint32 dwLockSize, uint32 & dwStartOffset)



{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hib;
    return (uint16 *)sLockNVNBuffer(ptNVNBuffer, dwLockSize, dwStartOffset, FALSE);
}




uint16 * GFXNX64NVN::IBLockAll(HIB hib)
{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hib;
    uint32 dwStartOffset;


    if (!ptNVNBuffer->fIsDynamic)
    {
        for (int32 worker = 0; worker < 1; worker++)
            ptNVNBuffer->dwNextOffsetToLock[worker] = 0;
    }


    return (uint16 *)sLockNVNBuffer(ptNVNBuffer, ptNVNBuffer->dwSize, dwStartOffset, FALSE);
}




void GFXNX64NVN::IBFlush(HIB hib)
{
    GFXNVNBuffer * ptNVNBuffer = (GFXNVNBuffer *)hib;

    for (int32 worker = 0; worker < 1; worker++)
    {
        ptNVNBuffer->dwNextOffsetToLock[worker] = ptNVNBuffer->fIsDynamic ? ptNVNBuffer->dwSize : 0;
    }
}




void GFXNX64NVN::IBUnlock(HIB hib)

{
    sFlushNVNBuffer((GFXNVNBuffer *)hib);
}




BOOL GFXNX64NVN::IBIsBusy(HIB hib)

{
    return NVNIsResourceBusy((GFXNVNBuffer *)hib);
}




void GFXNX64NVN::IBSetCurrent(HIB hib)
{
    RenderWorkerGlobals * renderWorkerGlobals = &workerGlobals[0];


    if (renderWorkerGlobals->tState.hibCurrent != hib)
    {
        renderWorkerGlobals->tState.hibCurrent = (GFXNVNBuffer *)hib;
    }
}




void * GFXNX64NVN::sLockNVNBuffer(GFXNVNBuffer * ptNVNBuffer, uint32 dwLockSize, uint32 & dwStartOffset, BOOL bIsVB)
{
    uint8 * baseAddr;




    if (ptNVNBuffer->fIsDynamic)
    {
        if (ptNVNBuffer->dwNextOffsetToLock[0] + dwLockSize > ptNVNBuffer->dwSize)
        {
            ptNVNBuffer->pDynamicBuffers[0] = sDynamicBufferAlloc(ptNVNBuffer->dwSize, 8);
            ptNVNBuffer->dwNextOffsetToLock[0] = 0;


            if (bIsVB)
            {
                workerGlobals[0].tState.tFlags.VertexStreamsDirty = 1;
            }
        }
        baseAddr = ptNVNBuffer->pDynamicBuffers[0];
    }
    else
    {

        baseAddr = (uint8 *)ptNVNBuffer->tMemory.ptMemory;
    }

    dwStartOffset = ptNVNBuffer->dwNextOffsetToLock[0];
    ptNVNBuffer->dwNextOffsetToLock[0] += dwLockSize;

    return baseAddr + dwStartOffset;
}




void GFXNX64NVN::sFlushNVNBuffer(const GFXNVNBuffer * ptBuffer)
{
    uint32 dwSize;
    ptrdiff_t tMemOffset;
    const nvn::MemoryPool * ptMemPool;

    if (ptBuffer->fIsDynamic)
    {
        int32 nWorkerIndex = 0;
        void * ptBufferAddress = ptBuffer->pDynamicBuffers[nWorkerIndex];

        ptMemPool = globals.ptDynamicBuffer->GetMemoryPool();
        tMemOffset = globals.ptDynamicBuffer->GetAddressOffset(ptBufferAddress);
        dwSize = ptBuffer->dwSize;
    }
    else
    {


        tMemOffset = ptBuffer->tMemory.nAllocOffset;
        ptMemPool = ptBuffer->tMemory.ptMemoryPool;
        dwSize = ptBuffer->dwSize;
    }

    if (ptMemPool)
    {
        ptMemPool->FlushMappedRange(tMemOffset, dwSize);
    }
}



void GFXNX64NVN::Draw(const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32, const uint32,
                      const uint32 dwStartIndex, const uint32 dwPrimCount)





{
    RenderWorkerGlobals * renderWorkerGlobals = &workerGlobals[0];

    sApplyState(renderWorkerGlobals->tState);

    uint32 dwNumIndices;
    int32 nWorkerIndex = 0;
    nvn::DrawPrimitive tPrimType;

    switch (eDrawMode)
    {
    case DRAWMODE_POINTLIST:
        tPrimType = nvn::DrawPrimitive::POINTS;
        dwNumIndices = dwPrimCount;
        break;

    case DRAWMODE_TRIANGLESTRIP:
        tPrimType = nvn::DrawPrimitive::TRIANGLE_STRIP;
        dwNumIndices = dwPrimCount + 2;
        break;

    case DRAWMODE_TRIANGLELIST:
        tPrimType = nvn::DrawPrimitive::TRIANGLES;
        dwNumIndices = dwPrimCount * 3;
        break;

    case DRAWMODE_TRIANGLEFAN:
        tPrimType = nvn::DrawPrimitive::TRIANGLE_FAN;
        dwNumIndices = dwPrimCount + 2;
        break;

    case DRAWMODE_LINELIST:
        tPrimType = nvn::DrawPrimitive::LINES;
        dwNumIndices = dwPrimCount * 2;
        break;

    case DRAWMODE_LINESTRIP:
        tPrimType = nvn::DrawPrimitive::LINE_STRIP;
        dwNumIndices = dwPrimCount + 1;
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }

    if (renderWorkerGlobals->tState.hibCurrent == NULL)
    {
        renderWorkerGlobals->tCommandBuffer.DrawArrays(tPrimType, dwBaseIndexOffset + dwStartIndex, dwNumIndices);
    }
    else
    {
        GFXNVNBuffer * ptIB = renderWorkerGlobals->tState.hibCurrent;
        nvn::BufferAddress tIBAddress = ptIB->fIsDynamic ? sGetDynamicBufferAddress(ptIB->pDynamicBuffers[nWorkerIndex]) : ptIB->tBufferAddr;
        renderWorkerGlobals->tCommandBuffer.DrawElementsBaseVertex(tPrimType, nvn::IndexType::UNSIGNED_SHORT, dwNumIndices, tIBAddress + dwStartIndex * 2, dwBaseIndexOffset);
        NVNMarkResourceUsedThisFrame(renderWorkerGlobals->tState.hibCurrent);
    }
}




void GFXNX64NVN::sApplyState(NVNState & rState)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    int32 nWorkerIndex = 0;


    sApplyRenderTargetState(rState);


    if (rState.tFlags.VertexShaderDirty)
    {
        NVNVertexShader * ptNVNShader = (NVNVertexShader *)rState.ptVertexShader->lpSH;
        renderWorkerGlobals.tCommandBuffer.BindProgram(&ptNVNShader->tProgram, nvn::ShaderStageBits::VERTEX);
        rState.tFlags.VertexShaderDirty = 0;
    }


    if (rState.tFlags.PixelShaderDirty)
    {
        nvn::Program * ptProgram = NULL;

        if (rState.ptPixelShader)
        {
            NVNPixelShader * ptNVNShader = (NVNPixelShader *)rState.ptPixelShader->lpSH;
            ptProgram = &ptNVNShader->tProgram;
        }

        renderWorkerGlobals.tCommandBuffer.BindProgram(ptProgram, nvn::ShaderStageBits::FRAGMENT);
        rState.tFlags.PixelShaderDirty = 0;
    }


    if (rState.tFlags.NeedRenderTargetBarrier)
    {
        renderWorkerGlobals.tCommandBuffer.Barrier(nvn::BarrierBits(nvn::BarrierBits::ORDER_FRAGMENTS) | nvn::BarrierBits::INVALIDATE_TEXTURE);
        rState.tFlags.NeedRenderTargetBarrier = 0;
    }


    if (rState.tFlags.PSTextureResourcesDirty)
    {
        do
        {
        int nStage = __builtin_ffs(rState.tFlags.PSTextureResourcesDirty) - 1;


        NVNTexInfo * ptCurrentTexture = rState.ptPSTextureResources[nStage];
        if (ptCurrentTexture == NULL)
        {
            ptCurrentTexture = &globals.tMissing2DTexture;
        }

        const NVNShaderStateBlock::SamplerSetup & curSampler = rState.tSamplerSetups[nStage];
        const int32 nSamplerID = SamplerStates[curSampler.eTAModeU][curSampler.eTAModeV][curSampler.eTAModeW][curSampler.eNVNFilterMode][curSampler.nMipBiasIndex].nSamplerID;

        nvn::TextureHandle texHandle = globals.tDevice.GetTextureHandle(ptCurrentTexture->GetTextureID(sSelectRenderTargetAlias()), nSamplerID);
        renderWorkerGlobals.tCommandBuffer.BindTexture(nvn::ShaderStage::FRAGMENT, nStage, texHandle);

        rState.tFlags.PSTextureResourcesDirty &= ~(1 << nStage);
        } while (rState.tFlags.PSTextureResourcesDirty);
    }


    if (rState.tFlags.ConstantBuffersDirty)
    {
        for (int i = 0; i < NVNCBUFFER_COUNT; i++)
        {
            if (rState.tFlags.ConstantBuffersDirty & (1 << i))
            {
                renderWorkerGlobals.tCommandBuffer.UpdateUniformBuffer(globals.tConstantBuffers[i], globals.dwConstantBufferSize[i], 0, globals.dwConstantBufferSize[i], renderWorkerGlobals.ppWorkingConstantBuffers[i]);
            }
        }
        rState.tFlags.ConstantBuffersDirty = 0;
    }


    if (rState.tFlags.VertexBindingDirty)
    {
        nvn::VertexStreamState streamStatesToBind[3];
        uint64 inputBindingKey = rState.GetVertexInputBindingKey();
        NVNVertexInputBindingSet * ptAttributeBindings = sGetVertexAttributeBindings(inputBindingKey);




        for (int i = 0; i < 3; i++)
        {
            GFXNVNBuffer * vbCurrent = rState.hvbCurrent[i];
            nvn::VertexStreamState & streamStateCurrent = streamStatesToBind[i];

            if (vbCurrent)
            {


                const VBFormat format = rState.eStreamVBFormat[i];
                streamStateCurrent = globals.tStreamStates[format];
            }
            else
            {
                streamStateCurrent.SetDefaults();
            }
        }

        renderWorkerGlobals.tCommandBuffer.BindVertexStreamState(3, streamStatesToBind);
        renderWorkerGlobals.tCommandBuffer.BindVertexAttribState(ptAttributeBindings->dwNumAttributes, ptAttributeBindings->ptAttribStates);

        rState.tFlags.VertexBindingDirty = 0;
    }


    if (rState.tFlags.VertexStreamsDirty)
    {
        nvn::BufferRange buffersToBind[3];


        for (int i = 0; i < 3; i++)
        {
            GFXNVNBuffer * vbCurrent = rState.hvbCurrent[i];
            nvn::BufferRange & bufferCurrent = buffersToBind[i];

            if (vbCurrent)
            {
                const uint32 dwStreamOffset = rState.dwCurrentStreamOffset[i];

                bufferCurrent.address = vbCurrent->fIsDynamic ? sGetDynamicBufferAddress(vbCurrent->pDynamicBuffers[nWorkerIndex]) : vbCurrent->tBufferAddr;
                bufferCurrent.address += dwStreamOffset;
                bufferCurrent.size = vbCurrent->dwSize - dwStreamOffset;

                NVNMarkResourceUsedThisFrame(vbCurrent);
            }
            else
            {

                bufferCurrent.address = 0;
                bufferCurrent.size = 0;
            }
        }
        renderWorkerGlobals.tCommandBuffer.BindVertexBuffers(0, 3, buffersToBind);

        rState.tFlags.VertexStreamsDirty = 0;
    }


    if (rState.tFlags.ViewportDirty)
    {
        renderWorkerGlobals.tCommandBuffer.SetViewport(rState.tViewportRect.left, rState.tViewportRect.top, rState.tViewportRect.Width(), rState.tViewportRect.Height());
        renderWorkerGlobals.tCommandBuffer.SetDepthRange(0.0f, 1.0f);
        rState.tFlags.ViewportDirty = 0;
    }


    if (rState.tFlags.DepthStateDirty || rState.tFlags.StencilStateDirty)
    {
        renderWorkerGlobals.tCommandBuffer.BindDepthStencilState(&rState.tDepthStencilState);


        if (rState.tFlags.StencilStateDirty)
        {
            renderWorkerGlobals.tCommandBuffer.SetStencilRef(nvn::Face::FRONT_AND_BACK, rState.nStencilRefValue);
        }
        rState.tFlags.DepthStateDirty = 0;
        rState.tFlags.StencilStateDirty = 0;
    }


    if (rState.tFlags.BlendStateDirty)
    {
        renderWorkerGlobals.tCommandBuffer.BindColorState(&ColorStates[rState.bBlendingEnabled]);
        renderWorkerGlobals.tCommandBuffer.BindBlendState(&BlendStates[rState.eSourceBlendFactor][rState.eDestBlendFactor][rState.eBlendOp]);


        static const float kClearWhite[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
        renderWorkerGlobals.tCommandBuffer.SetBlendColor(kClearWhite);
        rState.tFlags.BlendStateDirty = 0;
    }


    if (rState.tFlags.ChannelMaskStateDirty)
    {
        renderWorkerGlobals.tCommandBuffer.BindChannelMaskState(&ChannelMaskStates[rState.bColorWriteEnabled][rState.bAlphaWriteEnabled]);
        rState.tFlags.ChannelMaskStateDirty = 0;
    }


    if (rState.tFlags.PolygonStateDirty)
    {
        renderWorkerGlobals.tCommandBuffer.BindPolygonState(&PolygonState[rState.eCullMode][rState.eFillMode]);
        rState.tFlags.PolygonStateDirty = 0;
    }
}

void GFXNX64NVN::sApplyRenderTargetState(NVNState & rState)
{
    int32 nWorkerIndex = 0;
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[nWorkerIndex];

    if (rState.tFlags.ColorBufferBindingsDirty || rState.tFlags.DepthBufferBindingsDirty)
    {
        nvn::Texture * ptColorTarget = NULL;
        nvn::Texture * ptDepthTarget = NULL;

        NVNTexInfo * ptTarget = rState.ptCurrentRenderTarget[0];
        if (ptTarget)
        {
            sPrepareTargetForWrite(ptTarget);
            ptColorTarget = ptTarget->GetTexture(sSelectRenderTargetAlias());
            ptTarget->bTargetUpdated[0] = TRUE;
        }

        if (rState.ptCurrentDepthTarget)
        {
            ptDepthTarget = rState.ptCurrentDepthTarget->GetTexture(sSelectRenderTargetAlias());
            rState.ptCurrentDepthTarget->bTargetUpdated[0] = TRUE;
        }

        renderWorkerGlobals.tCommandBuffer.SetRenderTargets(1, &ptColorTarget, NULL, ptDepthTarget, NULL);

        rState.tFlags.ColorBufferBindingsDirty = 0;
        rState.tFlags.DepthBufferBindingsDirty = 0;
    }
}




uint64 NVNState::GetVertexInputBindingKey()
{
    NVNVertexShader * ptNVNShader;
    const uint64 format0 = eStreamVBFormat[0];
    const uint64 format2 = eStreamVBFormat[2] & 0xf;


    ptNVNShader = (NVNVertexShader *)ptVertexShader->lpSH;

    uint64 key = ptNVNShader->qwInputBindings + (format2 << 56) | (format0 << 60);
    return key;
}




void GFXNX64NVN::TextureSetCurrent(const uint32 dwStage, HTexture hTexture)


{
    NVNTexInfo * ptTexInfo = (NVNTexInfo *)hTexture;
    int32 nWorkerIndex = 0;
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[nWorkerIndex];

    if (renderWorkerGlobals.tState.ptPSTextureResources[dwStage] != ptTexInfo)
    {
        BOOL bNeedBarrier = ptTexInfo && ptTexInfo->bTargetUpdated[0];
        if (bNeedBarrier)
        {
            renderWorkerGlobals.tState.tFlags.NeedRenderTargetBarrier = 1;
            ptTexInfo->bTargetUpdated[0] = FALSE;
        }

        renderWorkerGlobals.tState.ptPSTextureResources[dwStage] = ptTexInfo;
        renderWorkerGlobals.tState.tFlags.PSTextureResourcesDirty |= (1 << dwStage);

        if (ptTexInfo)
        {
            NVNMarkResourceUsedThisFrame(ptTexInfo);
        }
    }
}





void GFXNX64NVN::CursorSet(HTexture hTexture, const IVector2D & pHotSpot, const uint32 dwFrame)
{






}






















BOOL GFXNX64NVN::Stereo3DEnabled()
{

    return FALSE;
}




void GFXNX64NVN::GammaSet(const float flGammaFactor)
{
    const float x = flGammaFactor;
    const float a = 0.36f;
    const float b = -1.54f;
    const float c = 2.18f;




    const float x2 = x * x;

    globals.fGammaValue = Math::Pin(b * x + c + a * x2, 0.68f, 1.5f);
}

static nvn::BlendEquation ToNVNBlendEquation(BlendOp blendOp)
{
    nvn::BlendEquation nvnBlendEqTable[BLENDOP_SIZE] =
    {
        nvn::BlendEquation::ADD,
        nvn::BlendEquation::ADD,
        nvn::BlendEquation::SUB,
        nvn::BlendEquation::REVERSE_SUB,
        nvn::BlendEquation::MIN,
        nvn::BlendEquation::MAX,
    };
    return nvnBlendEqTable[blendOp];
}

static nvn::BlendFunc ToNVNBlendFunc(BlendFactor blendFactor)
{
    nvn::BlendFunc nvnBlendFuncTable[BLENDFACTOR_SIZE] =
    {
        nvn::BlendFunc::ONE,
        nvn::BlendFunc::ZERO,
        nvn::BlendFunc::ONE,
        nvn::BlendFunc::SRC_COLOR,
        nvn::BlendFunc::ONE_MINUS_SRC_COLOR,
        nvn::BlendFunc::SRC_ALPHA,
        nvn::BlendFunc::ONE_MINUS_SRC_ALPHA,
        nvn::BlendFunc::DST_COLOR,
        nvn::BlendFunc::ONE_MINUS_DST_COLOR,
        nvn::BlendFunc::DST_ALPHA,
        nvn::BlendFunc::ONE_MINUS_DST_ALPHA,
        nvn::BlendFunc::CONSTANT_COLOR,
    };
    return nvnBlendFuncTable[blendFactor];
}

static nvn::WrapMode ToNVNWrapMode(TextureAddressMode addressMode)
{
    nvn::WrapMode nvnWrapModeTable[TEXTUREADDRESSMODE_SIZE] =
    {
        nvn::WrapMode::CLAMP_TO_EDGE,
        nvn::WrapMode::CLAMP_TO_EDGE,
        nvn::WrapMode::REPEAT,
        nvn::WrapMode::MIRRORED_REPEAT,
    };
    return nvnWrapModeTable[addressMode];
}

static nvn::Face ToNVNFace(CullMode eCullMode)
{
    nvn::Face nvnFaceTable[CULLMODE_SIZE] =
    {
        nvn::Face::NONE,
        nvn::Face::NONE,
        nvn::Face::BACK,
        nvn::Face::FRONT,
    };
    return nvnFaceTable[eCullMode];
}


void GFXNX64NVN::sCreateGlobalStateObjects()
{
    for (int sourceFactor = 0; sourceFactor < BLENDFACTOR_SIZE; sourceFactor++)
    {
        for (int destFactor = 0; destFactor < BLENDFACTOR_SIZE; destFactor++)
        {
            for (int blendOp = 0; blendOp < BLENDOP_SIZE; blendOp++)
            {
                nvn::BlendState & rBlendState = BlendStates[sourceFactor][destFactor][blendOp];
                nvn::BlendEquation eBlendEq = ToNVNBlendEquation((BlendOp)blendOp);
                nvn::BlendFunc eSourceBlendFunc = ToNVNBlendFunc((BlendFactor)sourceFactor);
                nvn::BlendFunc eDestBlendFunc = ToNVNBlendFunc((BlendFactor)destFactor);

                rBlendState.SetDefaults();
                rBlendState.SetBlendEquation(eBlendEq, eBlendEq);
                rBlendState.SetBlendFunc(eSourceBlendFunc, eDestBlendFunc, eSourceBlendFunc, eDestBlendFunc);
            }
        }
    }

    for (int blendEnabled = 0; blendEnabled < 2; blendEnabled++)
    {
        ColorStates[blendEnabled].SetDefaults();
        ColorStates[blendEnabled].SetBlendEnable(0, blendEnabled);
    }

    for (int colorWriteEnable = 0; colorWriteEnable < 2; colorWriteEnable++)
    {
        for (int alphaWriteEnable = 0; alphaWriteEnable < 2; alphaWriteEnable++)
        {
            ChannelMaskStates[colorWriteEnable][alphaWriteEnable].SetDefaults();
            ChannelMaskStates[colorWriteEnable][alphaWriteEnable].SetChannelMask(0, colorWriteEnable, colorWriteEnable, colorWriteEnable, alphaWriteEnable);
        }
    }

    for (int cullModeIdx = 0; cullModeIdx < CULLMODE_SIZE; cullModeIdx++)
    {
        for (int fillModeIdx = 0; fillModeIdx < FILLMODE_SIZE; fillModeIdx++)
        {
            nvn::PolygonState & nvnPolygonState = PolygonState[cullModeIdx][fillModeIdx];

            nvnPolygonState.SetDefaults();
            nvnPolygonState.SetFrontFace(nvn::FrontFace::CCW);
            nvnPolygonState.SetCullFace(ToNVNFace((CullMode)cullModeIdx));
            nvnPolygonState.SetPolygonMode(fillModeIdx == FILLMODE_WIREFRAME ? nvn::PolygonMode::LINE : nvn::PolygonMode::FILL);
        }
    }

    const float fTrijuiceThreshold = 0.3f;
    nvn::SamplerBuilder SamplerBuilder;
    SamplerBuilder.SetDevice(&globals.tDevice);
    for (int uAddr = 0; uAddr < TEXTUREADDRESSMODE_SIZE; uAddr++)
    {
        for (int vAddr = 0; vAddr < TEXTUREADDRESSMODE_SIZE; vAddr++)
        {
            for (int wAddr = 0; wAddr < TEXTUREADDRESSMODE_SIZE; wAddr++)
            {
                for (int filter = 0; filter < NVN_FILTERMODE_SIZE; filter++)
                {
                    for (int mipBias = 0; mipBias < MipBiasSamplerSlots; mipBias++)
                    {
                        SamplerBuilder.SetDefaults();
                        SamplerBuilder.SetWrapMode(ToNVNWrapMode((TextureAddressMode)uAddr), ToNVNWrapMode((TextureAddressMode)vAddr), ToNVNWrapMode((TextureAddressMode)wAddr));

                        switch (filter)
                        {
                        case NVN_FILTERMODE_POINT_SAMPLED:
                        default:
                            SamplerBuilder.SetMinMagFilter(nvn::MinFilter::NEAREST_MIPMAP_NEAREST, nvn::MagFilter::NEAREST);
                            break;

                        case NVN_FILTERMODE_BILINEAR:
                            SamplerBuilder.SetMinMagFilter(nvn::MinFilter::LINEAR_MIPMAP_NEAREST, nvn::MagFilter::LINEAR);
                            break;

                        case NVN_FILTERMODE_TRILINEAR:
                            SamplerBuilder.SetMinMagFilter(nvn::MinFilter::LINEAR_MIPMAP_LINEAR, nvn::MagFilter::LINEAR);
                            SamplerBuilder.SetLodSnap(fTrijuiceThreshold);
                            break;
                        }

                        SamplerBuilder.SetLodBias((float)-mipBias);

                        NVNSampler & currentSampler = SamplerStates[uAddr][vAddr][wAddr][filter][mipBias];
                        currentSampler.tSampler.Initialize(&SamplerBuilder);
                        globals.tIDManager.RegisterSampler(currentSampler);
                    }
                }
            }
        }
    }
}
