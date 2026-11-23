#include "NX64NVN.h"
#include "NX64NVN.inl"
#include "../../../../NintendoSDK/Include/nvnTool/nvnTool_GlslcInterface.h"
#include "../../Definitions/ShaderDefinition.h"
#include "../../../Common/TagMaps/Tags.h"
#include "../../../Common/TagMaps/VertexShaderTags.h"
#include "../../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../../Sigma/DataStructures/XRemoteHeap.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/StringManip.h"
#include <string.h>
#include <strings.h>

extern XRemoteHeap * gptShaderHeap;  // SigmaMemory.cpp:136






const char * SemanticNames[NVNSEMANTIC_COUNT] = {
    "in_POSITION0",
    "in_NORMAL0",
    "in_COLOR0",
    "in_COLOR1",
    "in_TEXCOORD0",
    "in_TEXCOORD1",
    "in_TEXCOORD2",
    "in_TEXCOORD3",
    "in_TEXCOORD4",
    "in_TEXCOORD5",
    "in_TANGENT0",
    "in_BINORMAL0",
    "in_BLENDINDICES0",
    "in_BLENDWEIGHT0",
};









void NVNShaderAllocator::Create(XRemoteHeap * pHeap)
{
    ptHeap = pHeap;

    nvn::MemoryPoolBuilder memoryPoolBuilder;
    nvn::MemoryPoolFlags flags = nvn::MemoryPoolFlags(nvn::MemoryPoolFlags::CPU_UNCACHED) | nvn::MemoryPoolFlags::GPU_CACHED | nvn::MemoryPoolFlags::SHADER_CODE;

    memoryPoolBuilder.SetDevice(&GFXNX64NVN::globals.tDevice);
    memoryPoolBuilder.SetDefaults();
    memoryPoolBuilder.SetFlags(flags);
    memoryPoolBuilder.SetStorage(gptShaderHeap->GetHeapBuffer(), gptShaderHeap->GetHeapSize());
    tMemoryPool.Initialize(&memoryPoolBuilder);
}

void NVNShaderAllocator::Destroy()
{
    tMemoryPool.Finalize();
}

inline ptrdiff_t NVNShaderAllocator::Allocate(size_t nSize, size_t nAlign)
{
    return (uint8 *)ptHeap->MemAlloc(nSize, nAlign, FALSE) - (uint8 *)GetBaseAddressCpu();
}

inline void NVNShaderAllocator::Free(ptrdiff_t allocation)
{
    ptHeap->MemFree((uint8 *)GetBaseAddressCpu() + allocation);
}

inline void * NVNShaderAllocator::GetBaseAddressCpu()
{
    return ptHeap->GetHeapBuffer();
}

inline void * NVNShaderAllocator::GetCpuAddressForAllocation(ptrdiff_t allocationOffset)
{
    return (uint8 *)GetBaseAddressCpu() + allocationOffset;
}




static void sApplyShaderConstBinding(ShaderConstFastBinding * ptConstFastBindingTable, ShaderData * ptOldShader, ShaderData * ptNewShader, BOOL fPixelShader)
{
    if (ptOldShader)
    {
        uint32 dwCount = ptOldShader->dwApplyableBindingCount;
        ApplyableShaderConstBinding * ptBindings = ptOldShader->ptApplyBindingTable;
        for (uint32 i = 0; i < dwCount; ++i)
        {
            if (fPixelShader)
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].dwFlags &= ~(1 << SHADERCONSTFASTBINDING_PIXEL_SHADER_BIT);
            else
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].dwFlags &= ~(1 << SHADERCONSTFASTBINDING_VERTEX_SHADER_BIT);
        }
    }

    if (ptNewShader)
    {
        uint32 dwCount = ptNewShader->dwApplyableBindingCount;
        ApplyableShaderConstBinding * ptBindings = ptNewShader->ptApplyBindingTable;
        for (uint32 i = 0; i < dwCount; ++i)
        {
            if (fPixelShader)
            {
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].tPSBinding.nCBuffer = ptBindings[i].nCBuffer;
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].tPSBinding.nRegister = ptBindings[i].nRegister;
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].tPSBinding.dwSize = ptBindings[i].uSize;
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].dwFlags |= (1 << SHADERCONSTFASTBINDING_PIXEL_SHADER_BIT);
            }
            else
            {
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].tVSBinding.nCBuffer = ptBindings[i].nCBuffer;
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].tVSBinding.nRegister = ptBindings[i].nRegister;
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].tVSBinding.dwSize = ptBindings[i].uSize;
                ptConstFastBindingTable[ptBindings[i].nConstantIndex].dwFlags |= (1 << SHADERCONSTFASTBINDING_VERTEX_SHADER_BIT);
            }
        }
    }
}

























template <class T>
static T * sCreateNVNShader(GLSLCoutput_rec * pShaderBinary, nvn::Device * ptDevice, NVNShaderAllocator & rShaderAllocator)
{
    T * pShader = NULL;

    for (uint32 sec = 0; sec < pShaderBinary->numSections; ++sec)
    {
        GLSLCsectionTypeEnum sectionType = pShaderBinary->headers[sec].genericHeader.common.type;
        if (sectionType == GLSLC_SECTION_TYPE_GPU_CODE)
        {
            const GLSLCgpuCodeHeader & gpuCodeHeader = pShaderBinary->headers[sec].gpuCodeHeader;

            pShader = new T;

            pShader->pControlData = NVNMemoryAllocator::Allocate(gpuCodeHeader.controlSize, 256, NVNALLOCTYPE_PERSISTENT);

            {



                ptrdiff_t commandAllocation = rShaderAllocator.Allocate(gpuCodeHeader.dataSize, 8);
                nvn::BufferBuilder bufferBuilder;
                bufferBuilder.SetDefaults();
                bufferBuilder.SetDevice(ptDevice);
                bufferBuilder.SetStorage(&rShaderAllocator.tMemoryPool, commandAllocation, gpuCodeHeader.dataSize);
                pShader->tCommandData.Initialize(&bufferBuilder);
            }

            {
                uint8 * gpuCodeDataStart = (uint8 *)pShaderBinary + gpuCodeHeader.common.dataOffset;
                uint8 * commandDataAddr = gpuCodeDataStart + gpuCodeHeader.dataOffset;
                uint8 * controlDataAddr = gpuCodeDataStart + gpuCodeHeader.controlOffset;



                SigmaMemoryCopy(rShaderAllocator.GetCpuAddressForAllocation(pShader->tCommandData.GetMemoryOffset()), commandDataAddr, gpuCodeHeader.dataSize);
                SigmaMemoryCopy(pShader->pControlData, controlDataAddr, gpuCodeHeader.controlSize);
            }

            {
                nvn::ShaderData shaderData;

                shaderData.data = pShader->tCommandData.GetAddress();
                shaderData.control = pShader->pControlData;

                pShader->tProgram.Initialize(ptDevice);
                NVNboolean bResult = pShader->tProgram.SetShaders(1, &shaderData);

                XASSERT(bResult);
            }

            return pShader;
        }
    }

    return pShader;
}



template <class T>
static void sDestroyNVNShader(T * pShader, NVNShaderAllocator & rShaderAllocator)
{
    pShader->tProgram.Finalize();

    NVNMemoryAllocator::Free(pShader->pControlData, NVNALLOCTYPE_PERSISTENT);
    pShader->pControlData = NULL;



    ptrdiff_t commandDataOffset = pShader->tCommandData.GetMemoryOffset();
    pShader->tCommandData.Finalize();
    rShaderAllocator.Free(commandDataOffset);
}

NVNVertexShader * GFXNX64NVN::sCreateNVNVertexShader(GLSLCoutput_rec * pShaderBinary)
{
    return sCreateNVNShader<NVNVertexShader>(pShaderBinary, &globals.tDevice, globals.tShaderAllocator);
}

NVNPixelShader * GFXNX64NVN::sCreateNVNPixelShader(GLSLCoutput_rec * pShaderBinary)
{
    return sCreateNVNShader<NVNPixelShader>(pShaderBinary, &globals.tDevice, globals.tShaderAllocator);
}

struct NVNShaderReflector {  // size 0x28

    NVNShaderReflector(GLSLCoutput * pShaderBinary)
        : pReflectionHeader(NULL)
        , pStringPool(NULL)
    {
        for (uint32 sec = 0; sec < pShaderBinary->numSections; ++sec)
        {
            GLSLCsectionTypeEnum sectionType = pShaderBinary->headers[sec].genericHeader.common.type;

            if (sectionType == GLSLC_SECTION_TYPE_REFLECTION)
            {
                pReflectionHeader = &pShaderBinary->headers[sec].programReflectionHeader;

                uint8 * pSectionData = (uint8 *)pShaderBinary + pReflectionHeader->common.dataOffset;

                pInputs = (GLSLCprogramInputInfo *)(pSectionData + pReflectionHeader->programInputsOffset);
                pUniformBlocks = (GLSLCuniformBlockInfo *)(pSectionData + pReflectionHeader->uniformBlockOffset);
                pUniforms = (GLSLCuniformInfo *)(pSectionData + pReflectionHeader->uniformOffset);
                pStringPool = (const char *)(pSectionData + pReflectionHeader->stringPoolOffset);
                break;
            }
        }
    }



    template <class T>
    struct ReflectionInfo {  // size 0x10
        ReflectionInfo(T * pInfo, const char * stringPool)
            : pName(stringPool + pInfo->nameInfo.nameOffset), pReflInfo(pInfo)
        {
        }

        const char * pName;  // 0x00
        T * pReflInfo;  // 0x08
    };

    typedef NVNShaderReflector::ReflectionInfo<GLSLCProgramInputInfo_rec> ShaderInput;
    typedef NVNShaderReflector::ReflectionInfo<GLSLCuniformBlockInfo_rec> UniformBlock;
    typedef NVNShaderReflector::ReflectionInfo<GLSLCuniform_rec> Uniform;

    uint32 GetNumShaderInputs() const { return pReflectionHeader->numProgramInputs; }
    NVNShaderReflector::ShaderInput GetShaderInput(uint32 index) const { return ShaderInput(&pInputs[index], pStringPool); }

    uint32 GetNumUniformBlocks() const { return pReflectionHeader->numUniformBlocks; }
    NVNShaderReflector::UniformBlock GetUniformBlock(uint32 index) const { return UniformBlock(&pUniformBlocks[index], pStringPool); }

    uint32 GetNumUniforms() const { return pReflectionHeader->numUniforms; }
    NVNShaderReflector::Uniform GetUniform(uint32 index) const { return Uniform(&pUniforms[index], pStringPool); }

    int32 GetUniformBlockBindingIndex(const NVNShaderReflector::Uniform & uniform) const
    {
        int32 bindingIndex = -1;

        if (uniform.pReflInfo->isInUBO)
        {
            int32 stageIndex = -1;
            for (int i = 0; i < 6; ++i)
            {
                if (uniform.pReflInfo->stagesReferencedIn & (1 << i))
                {
                    stageIndex = i;
                    break;
                }
            }

            if (stageIndex != -1)
                bindingIndex = GetUniformBlock(uniform.pReflInfo->blockNdx).pReflInfo->bindings[stageIndex];
        }

        return bindingIndex;
    }

private:
    GLSLCprogramReflectionHeader * pReflectionHeader;  // 0x00
    GLSLCprogramInputInfo * pInputs;  // 0x08
    GLSLCuniformBlockInfo * pUniformBlocks;  // 0x10
    GLSLCuniformInfo * pUniforms;  // 0x18
    const char * pStringPool;  // 0x20
};





inline void sGetGLSLCConstantName(int32 nVSEntry, char * pszConstantName)
{
    switch (nVSEntry)
    {
        case VSC_DIRECTIONALLIGHTS:
            strcpy(pszConstantName, "lightDirectionals.wv");
            break;
        case VSC_POINTLIGHTS:
            strcpy(pszConstantName, "lightPoints.wp");
            break;
        case VSC_SPOTLIGHTS:
            strcpy(pszConstantName, "lightSpots[0].wp");
            break;
        case VSC_CYLINDRICALLIGHTS:
            strcpy(pszConstantName, "lightCylindricals.wp");
            break;
        case VSC_POINTLINEARLIGHTS:
            strcpy(pszConstantName, "lightPointLinears[0].wp");
            break;
        case VSC_NUM_LIGHTS_INT:
            strcpy(pszConstantName, "numLights");
            break;
        default:
            strcpy(pszConstantName, cgszVSConstBindingTable[nVSEntry]);
            break;
    }
}













inline void RemoveIfStartsWith(const char *& pConstantName, const char * testString)
{
    if (strncmp(pConstantName, testString, strlen(testString)) == 0)
        pConstantName += strlen(testString);
}









static int32 sGetConstantIndex(const NVNShaderReflector & tReflector, const NVNShaderReflector::Uniform & uniform)
{
    int32 uniformBlockBindingIndex;
    const char * pUniformName = uniform.pName;

    RemoveIfStartsWith(pUniformName, "hlslcc_mtx3x4");
    RemoveIfStartsWith(pUniformName, "hlslcc_mtx4x4");

    uniformBlockBindingIndex = tReflector.GetUniformBlockBindingIndex(uniform);
    if (uniformBlockBindingIndex == -1)
        return -1;
    for (int i = 0; i != NUM_VS_CONSTANT_ENTRIES; ++i)
    {
        char szConstantNameBuffer[256];
        const uint32 dwConstantNameBufferSize = sizeof(szConstantNameBuffer);
        sGetGLSLCConstantName(i, szConstantNameBuffer);
        if (uniform.pReflInfo->isArray)
            strcat_safe(szConstantNameBuffer, dwConstantNameBufferSize, "[0]");

        if (strcmp(pUniformName, szConstantNameBuffer) == 0)
            return i;
    }
    return -1;
}











void sCreateApplyableBindingTable(const NVNShaderReflector & tReflector, uint32 & dwApplyableBindingCount, ApplyableShaderConstBinding *& ptApplyableBindings, CTagMap * hTagMap)
{
    ApplyableShaderConstBinding * ptBinding;

    dwApplyableBindingCount = 0;
    ptApplyableBindings = NULL;



    for (int i = 0; i < tReflector.GetNumUniforms(); ++i)
    {
        NVNShaderReflector::Uniform uniform = tReflector.GetUniform(i);
        if (sGetConstantIndex(tReflector, uniform) != -1)
            ++dwApplyableBindingCount;
    }

    if (dwApplyableBindingCount == 0)
        return;

    ptApplyableBindings = (ApplyableShaderConstBinding *)SigmaMemoryNew(dwApplyableBindingCount * sizeof(ApplyableShaderConstBinding), 4, NULL, FALSE);
    ptBinding = ptApplyableBindings;







    for (int i = 0; i < tReflector.GetNumUniforms(); ++i)
    {
        NVNShaderReflector::Uniform uniform = tReflector.GetUniform(i);
        int32 constantIndex = sGetConstantIndex(tReflector, uniform);
        if (constantIndex == -1)
            continue;

        ptBinding->nConstantIndex = constantIndex;
        switch (constantIndex)
        {
            case VSC_DIRECTIONALLIGHTS:
                ptBinding->uSize = (uint8)Math::Min(TagMapGetInt(hTagMap, TAG_VS_NUM_DIRECTIONAL_LIGHTS), (int)MAX_DIRECTIONAL_LIGHTS) * (sizeof(DirectionalLight) / sizeof(ShaderFloat));
                break;
            case VSC_POINTLIGHTS:
                ptBinding->uSize = (uint8)Math::Min(TagMapGetInt(hTagMap, TAG_VS_NUM_POINT_LIGHTS), (int)MAX_POINT_LIGHTS) * (sizeof(PointLight) / sizeof(ShaderFloat));
                break;
            case VSC_SPOTLIGHTS:
                ptBinding->uSize = (uint8)Math::Min(TagMapGetInt(hTagMap, TAG_VS_NUM_SPOT_LIGHTS), (int)MAX_SPOT_LIGHTS) * (sizeof(SpotLight) / sizeof(ShaderFloat));
                break;
            case VSC_CYLINDRICALLIGHTS:
                ptBinding->uSize = (uint8)Math::Min(TagMapGetInt(hTagMap, TAG_VS_NUM_CYLINDRICAL_LIGHTS), (int)MAX_CYLINDER_LIGHTS) * (sizeof(CylindricalLight) / sizeof(ShaderFloat));
                break;
            case VSC_POINTLINEARLIGHTS:
                ptBinding->uSize = (uint8)Math::Min(TagMapGetInt(hTagMap, TAG_VS_NUM_POINT_LINEAR_LIGHTS), (int)MAX_POINT_LINEAR_LIGHTS) * (sizeof(PointLinearLight) / sizeof(ShaderFloat));
                break;
            default:
                ptBinding->uSize = cgnVSConstSize[constantIndex];
                break;
        }
        ptBinding->nCBuffer = tReflector.GetUniformBlockBindingIndex(uniform);
        ptBinding->nRegister = uniform.pReflInfo->blockOffset;
        ++ptBinding;
    }
}





uint64 sConstructInputBindingMask(const NVNShaderReflector & tReflector)
{
    uint64 inputBindingMask = 0;
    const uint32 storageBitCount = 4;
    int maxBindLocation = 0;




    for (int i = 0; i < tReflector.GetNumShaderInputs(); ++i)
    {
        bool found = false;
        NVNShaderReflector::ShaderInput input = tReflector.GetShaderInput(i);
        int j;
        for (j = 0; j < NVNSEMANTIC_COUNT; ++j)
        {
            if (strcasecmp(SemanticNames[j], input.pName) == 0)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            int bindLocation = input.pReflInfo->location;
            maxBindLocation = Math::Max(bindLocation, maxBindLocation);
            inputBindingMask |= (uint64)j << ((bindLocation + 1) * storageBitCount);
        }
    }

    uint32 numAttrStatesNeeded = maxBindLocation + 1;
    return inputBindingMask | numAttrStatesNeeded;
}






NVNVertexInputBindingSet * GFXNX64NVN::sGetVertexAttributeBindings(uint64 qwKey)
{
    NVNVertexInputBindingSet * ptBindingSet = NULL;




    if (!globals.ptVertexAttribMap->Lookup(qwKey, ptBindingSet))
    {
        uint64 qwInputBindings = ((NVNVertexShader *)workerGlobals[0].tState.ptVertexShader->lpSH)->qwInputBindings;
        uint32 numAttr = qwInputBindings & 0xf;
        const uint32 storageBitCount = 4;
        RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

        ptBindingSet = new NVNVertexInputBindingSet(numAttr);

        for (uint32 i = 0; i < numAttr; ++i)
        {
            BOOL bFound = FALSE;
            NVNSemantic semantic = (NVNSemantic)((qwInputBindings >> ((i + 1) * storageBitCount)) & 0xf);

            for (int streamIdx = 0; streamIdx < 3 && !bFound; ++streamIdx)
            {



                ptBindingSet->ptAttribStates[i].SetDefaults();

                VBFormat eFormat = renderWorkerGlobals.tState.eStreamVBFormat[streamIdx];
                if (eFormat != VBFORMAT_UNKNOWN)
                {
                    VertexElementList * ptElemList = globals.ptVBFormatInfo[eFormat].ptElemList;
                    POS pos = ptElemList->GetHeadPosition();
                    while (pos)
                    {
                        const VertexElement & tElem = ptElemList->GetNext(pos);
                        if (tElem.bUsage == semantic)
                        {


                            ptBindingSet->ptAttribStates[i].SetStreamIndex(streamIdx).SetFormat(nvn::Format::Enum(tElem.dwType), tElem.wOffset);
                            bFound = TRUE;
                            break;
                        }
                    }
                }
            }
        }

        globals.ptVertexAttribMap->SetAt_Tracked(qwKey, ptBindingSet);
    }

    return ptBindingSet;
}
























void GFXNX64NVN::CreateVertexShader(
    ShaderDefinition * pdefShader,
    const int32 nPassIndex)
{
    RenderPass & tPass = pdefShader->ptPasses[nPassIndex];
    ConsoleShaderPassInfo * ptPassInfo = (ConsoleShaderPassInfo *)pdefShader->pConsoleShaderData.Get();
    NX64ShaderProg * ptShaderProg = (NX64ShaderProg *)((uint8 *)&ptPassInfo[pdefShader->nPassCount] + ptPassInfo[nPassIndex].nVertexShaderOffset);

    GLSLCoutput * pShaderCompilerOutput = NULL;
    uint32 dwShaderSize = ptPassInfo[nPassIndex].nVertexShaderSize;










        // Indentation (columns 618:13, 623:61) puts these two statements one level deeper; the enclosing construct is not recoverable.
        if (ptPassInfo[nPassIndex].nVertexShaderSize == 0)
            return;



        pShaderCompilerOutput = (GLSLCoutput *)(ptShaderProg + 1);



    tPass.tPostprocessed.ptVSD->lpSH = sCreateNVNVertexShader(pShaderCompilerOutput);
    if (tPass.tPostprocessed.ptVSD->lpSH == NULL)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    NVNShaderReflector reflector(pShaderCompilerOutput);

    for (uint32 i = 0; i < reflector.GetNumShaderInputs(); ++i)
    {
        NVNShaderReflector::ShaderInput shaderInput = reflector.GetShaderInput(i);

        if (strcasecmp(shaderInput.pName, SemanticNames[NVNSEMANTIC_BLEND_INDICES]) == 0)
            tPass.tPostprocessed.ptVSD->dwFlags |= (1 << VSPP_DOES_HARDWARE_SKINNING_BIT);


        else if (strcasecmp(shaderInput.pName, SemanticNames[NVNSEMANTIC_TANGENT]) == 0)
            tPass.tPostprocessed.ptVSD->dwFlags |= (1 << VSPP_NEEDS_TANGENT_BASIS_BIT);
    }


    ShaderData * ptShaderData = pdefShader->ptPasses[nPassIndex].tPostprocessed.ptVSD;
    sCreateApplyableBindingTable(reflector, ptShaderData->dwApplyableBindingCount, ptShaderData->ptApplyBindingTable, pdefShader->ptPasses[nPassIndex].hTagMap);

    NVNVertexShader * nvnVertexShader = (NVNVertexShader *)tPass.tPostprocessed.ptVSD->lpSH;
    nvnVertexShader->qwInputBindings = sConstructInputBindingMask(reflector);

    NVNMarkResourceReadyForDelete(ptShaderProg);
    tPass.tPostprocessed.ptVSD->dwShaderSize = ptPassInfo[nPassIndex].nVertexShaderSize;















}




void GFXNX64NVN::DeleteVertexShader(ShaderData * ptVSD)
{
    NVNVertexShader * ptVertexShader = (NVNVertexShader *)ptVSD->lpSH;



    sDestroyNVNShader(ptVertexShader, globals.tShaderAllocator);
    delete ptVertexShader;
    ptVSD->lpSH = NULL;

    SigmaMemoryFree(ptVSD->ptApplyBindingTable, NULL);
    ptVSD->ptApplyBindingTable = NULL;
}



BOOL GFXNX64NVN::VertexShaderIsBusy(ShaderData * ptVSD)
{

    return NVNIsResourceBusy(&ptVSD->tNX64Shader);
}




void GFXNX64NVN::CreatePixelShader(ShaderDefinition * pdefShader, const int32 nPassIndex)
{
    ConsoleShaderPassInfo * ptPassInfo;
    uint32 dwShaderSize;
    RenderPass & tPass = pdefShader->ptPasses[nPassIndex];
    ptPassInfo = (ConsoleShaderPassInfo *)pdefShader->pConsoleShaderData.Get();
    NX64ShaderProg * ptShaderProg = (NX64ShaderProg *)((uint8 *)&ptPassInfo[pdefShader->nPassCount] + ptPassInfo[nPassIndex].nPixelShaderOffset);
    GLSLCoutput * pShaderCompilerOutput = (GLSLCoutput *)(ptShaderProg + 1);
    dwShaderSize = ptPassInfo[nPassIndex].nPixelShaderSize;
    if (dwShaderSize == 0)
        return;



















    tPass.tPostprocessed.ptPSD->lpSH = sCreateNVNPixelShader(pShaderCompilerOutput);


    XASSERT(tPass.tPostprocessed.ptPSD->lpSH != NULL);


    ShaderData * ptShaderData = pdefShader->ptPasses[nPassIndex].tPostprocessed.ptPSD;
    NVNShaderReflector reflector(pShaderCompilerOutput);
    sCreateApplyableBindingTable(reflector, ptShaderData->dwApplyableBindingCount, ptShaderData->ptApplyBindingTable, pdefShader->ptPasses[nPassIndex].hTagMap);

    NVNMarkResourceReadyForDelete(ptShaderProg);
    tPass.tPostprocessed.ptPSD->dwShaderSize = ptPassInfo[nPassIndex].nPixelShaderSize;
}




















void GFXNX64NVN::DeletePixelShader(ShaderData * ptPSD)
{
    NVNPixelShader * ptPixelShader = (NVNPixelShader *)ptPSD->lpSH;

    sDestroyNVNShader(ptPixelShader, globals.tShaderAllocator);
    delete ptPixelShader;
    ptPSD->lpSH = NULL;

    SigmaMemoryFree(ptPSD->ptApplyBindingTable, NULL);
    ptPSD->ptApplyBindingTable = NULL;
}





BOOL GFXNX64NVN::PixelShaderIsBusy(ShaderData * ptPSD)
{
    return NVNIsResourceBusy(&ptPSD->tNX64Shader);
}





void GFXNX64NVN::SetVertexShader(const ShaderDefinition * pdefShader, const int32 nPassIndex)
{
    ShaderData * ptVertexShader;
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    ptVertexShader = pdefShader->ptPasses[nPassIndex].tPostprocessed.ptVSD;

    if (ptVertexShader != renderWorkerGlobals.tState.ptVertexShader)
    {
        uint64 qwPreviousBindingMask = renderWorkerGlobals.tState.ptVertexShader ? ((NVNVertexShader *)renderWorkerGlobals.tState.ptVertexShader->lpSH)->qwInputBindings : 0;

        sApplyShaderConstBinding(renderWorkerGlobals.ptConstFastBindingTable, renderWorkerGlobals.tState.ptVertexShader, ptVertexShader, FALSE);

        renderWorkerGlobals.tState.ptVertexShader = ptVertexShader;

        NVNMarkResourceUsedThisFrame(&ptVertexShader->tNX64Shader);

        renderWorkerGlobals.tState.tFlags.VertexShaderDirty = 1;

        if (qwPreviousBindingMask != ((NVNVertexShader *)renderWorkerGlobals.tState.ptVertexShader->lpSH)->qwInputBindings)
            renderWorkerGlobals.tState.tFlags.VertexBindingDirty = 1;
    }
}









void GFXNX64NVN::SetPixelShader(const ShaderDefinition * pdefShader, const int32 nPassIndex)
{
    ShaderData * ptPixelShader = NULL;
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    if (pdefShader)
    {


        ptPixelShader = pdefShader->ptPasses[nPassIndex].tPostprocessed.ptPSD;

        NVNMarkResourceUsedThisFrame(&ptPixelShader->tNX64Shader);
    }

    if (ptPixelShader != renderWorkerGlobals.tState.ptPixelShader)
    {
        sApplyShaderConstBinding(renderWorkerGlobals.ptConstFastBindingTable, renderWorkerGlobals.tState.ptPixelShader, ptPixelShader, TRUE);

        renderWorkerGlobals.tState.ptPixelShader = ptPixelShader;

        renderWorkerGlobals.tState.tFlags.PixelShaderDirty = 1;
    }
}


void GFXNX64NVN::SetVertexDeclaration()
{
}





const ShaderConstFastBinding * GFXNX64NVN::GetShaderConstantFastBindings()
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    return renderWorkerGlobals.ptConstFastBindingTable;
}
























void GFXNX64NVN::SetPixelShaderConstant(const ShaderConstBinding * ptBinding, const void * lpData, const uint32 dwSize)
{
    // Pixel and vertex constants share the working constant buffers.



    SetVertexShaderConstant(ptBinding, lpData, dwSize);
}























void GFXNX64NVN::SetVertexShaderConstant(const ShaderConstBinding * ptBinding, const void * lpData, const uint32 dwSize)
{
    uint32 dwSizeToUse;
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    int32 nCBuffer;







    dwSizeToUse = Math::Min(dwSize, ptBinding->dwSize);
    SigmaMemoryCopy(renderWorkerGlobals.ppWorkingConstantBuffers[ptBinding->nCBuffer] + ptBinding->nRegister, lpData, dwSizeToUse * sizeof(ShaderFloat));
    nCBuffer = ptBinding->nCBuffer;
    renderWorkerGlobals.tState.tFlags.ConstantBuffersDirty |= (1 << nCBuffer);
}
