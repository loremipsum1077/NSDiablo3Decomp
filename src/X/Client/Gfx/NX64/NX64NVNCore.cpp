// X/Client/Gfx/NX64/NX64NVNCore.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r6-gfx/nvn/frag.cpp by build/try/r6-gfx/gen.py.

#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "NX64NVN.h"
#include "NX64NVN.inl"
#include "NX64NVNHeap.h"
#include "../../../../NintendoSDK/Include/nvn/nvn_CppFuncPtrImpl.h"
#include "../GfxIB.h"
#include "../GfxVB.h"
#include "../../CGame.h"
#include "../../../Common/Definitions/Definitions.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "../../../Sigma/GfxColorFormats.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../../NintendoSDK/Include/nn/util/util_BitUtil.h"
#include <stdlib.h>
typedef nvn::PackagedTextureLayout NVNpackagedTextureLayout;  // original line 16
// X/Common/Definitions/Definitions.h (Definitions.cpp:434, :480)
void * DefinitionMemAllocTracked(GlobalCacheType eType, uint32 dwSize, uint32 dwAlign, BOOL fClear, const char * szFile, uint32 dwLine);
void DefinitionMemFree(GlobalCacheType eType, void * pMem);
// X/Common/Shell/Shell.h (ShellNX64.cpp)
BOOL ShellShouldSelectFullDisplayMode();
// X/Client/Gfx/GfxVB.cpp / GfxIB.cpp
void GfxVBFlushAllDynamic();
void GfxIBFlushAllDynamic();
// X/Sigma/Memory: the graphics persistent heap (XMemoryAllocatorInterface)
extern XMemoryAllocatorInterface * gptGraphicsPersistentMemoryHeap;
extern XRemoteHeap * gptShaderHeap;  // SigmaMemory.cpp:136
const float NVNTexInfo::kReducedFraction = 0.8f;
#include "../../../../NintendoSDK/Include/nvnTool/texpkg/nvnTool_Fd.h"
#include "../../../../NintendoSDK/Include/nvnTool/texpkg/nvnTool_HwTexture.h"
GFXNX64NVN::Globals GFXNX64NVN::globals;
NX64NVNHeap * gptGPUHeap;
#include "../../Definitions/ShaderDefinition.h"
namespace nn { namespace os { void YieldThread(); } }  // nn/os/os_ThreadApi.h (not in src)
static nvn::Format sTextureFormatToNVNFormat(const TextureFormat eTexFormat, NVNTexSwizzle * ptTexSwizzle);
#include "../../../Common/GlobalSNO.h"
BOOL ShellIsInBoostMode();  // X/Common/Shell/Shell.h (ShellNX64.cpp)
void SetUICConversionConstansts();  // X/Client/UserInterface/UIUtils.cpp:289

static const char *const szTotalFrameTimeName = "FrameTotal";
static const char *const szBackBufferSyncWait = "WaitForBackBuffer";

static const size_t GPU_HEAP_SIZE = 0x49c00000;

















void GFXNX64NVN::SetNumBones(int32 nNumBones)
{
}






BOOL GFXNX64NVN::Detect()
{
    return TRUE;
}





void GFXNX64NVN::RenderSuspend()
{
}





void GFXNX64NVN::RenderResume()
{
}






namespace NvMemory {

void * Allocate(size_t size, size_t alignment, void * userPtr)
{
    return aligned_alloc(alignment, nn::util::align_up(size, alignment));
}

void Free(void * addr, void * userPtr)
{
    free(addr);
}

void * Reallocate(void * addr, size_t newSize, void * userPtr)
{
    return realloc(addr, newSize);
}
}


// NintendoSDK nv / nn::vi / nvn bootstrap entry points (imports; no header for them in the DWARF).
// Signatures from the import symbols of the image.
#include "../../../../NintendoSDK/Include/nn/nn_Result.h"
#include "../../../../NintendoSDK/Include/nn/vi/vi_ScalingMode.h"
namespace nv {
void SetGraphicsAllocator(void * (*)(size_t, size_t, void *), void (*)(void *, void *), void * (*)(void *, size_t, void *), void *);
void InitializeGraphics(void *, size_t);
}
namespace nn { namespace vi {
void Initialize();
nn::Result OpenDefaultDisplay(Display **);
nn::Result CreateLayer(Layer **, Display *);
nn::Result SetLayerScalingMode(Layer *, ScalingMode);
nn::Result GetNativeWindow(NativeWindowHandle *, Layer *);
} }
extern "C" nvn::GenericFuncPtrFunc nvnBootstrapLoader(const char *);
#include <new>














void GFXNX64NVN::sInitNVN()
{
    globals.tIDManager.Init();

    sCreateCommandQueue();
    sCreateSwapChain();
}




inline void GFXNX64NVN::sInitNVMemory()
{
    nv::SetGraphicsAllocator(NvMemory::Allocate, NvMemory::Free, NvMemory::Reallocate, NULL);





    size_t graphicsSystemMemorySize = 0x4000000;
    void * graphicsHeap = malloc(graphicsSystemMemorySize);
    nv::InitializeGraphics(graphicsHeap, graphicsSystemMemorySize);
}




inline void GFXNX64NVN::sInitVideo()
{
    nn::vi::Initialize();

    { nn::Result nn_result = nn::vi::OpenDefaultDisplay(&globals.ptViDisplay); }
    { nn::Result nn_result = nn::vi::CreateLayer(&globals.ptViLayer, globals.ptViDisplay); }
    { nn::Result nn_result = nn::vi::SetLayerScalingMode(globals.ptViLayer, nn::vi::ScalingMode_FitToLayer); }
    { nn::Result nn_result = nn::vi::GetNativeWindow(&globals.tViNativeWindow, globals.ptViLayer); }
}









inline void GFXNX64NVN::sInitDevice()
{

    nvn::pfncpp_nvnDeviceGetProcAddress = (nvn::DeviceGetProcAddressFunc)nvnBootstrapLoader("nvnDeviceGetProcAddress");
    nvn::nvnLoadCPPProcs(NULL, nvn::pfncpp_nvnDeviceGetProcAddress);

    int deviceFlags = 0;









    nvn::DeviceBuilder deviceBuilder;
    deviceBuilder.SetDefaults();
    deviceBuilder.SetFlags(deviceFlags);
    globals.tDevice.Initialize(&deviceBuilder);





    nvn::nvnLoadCPPProcs(&globals.tDevice, nvn::pfncpp_nvnDeviceGetProcAddress);


    globals.tDevice.SetWindowOriginMode(nvn::WindowOriginMode::UPPER_LEFT);
    globals.tDevice.SetDepthMode(nvn::DepthMode::NEAR_IS_ZERO);
}




















inline void GFXNX64NVN::sCreateCommandQueue()
{
    nvn::QueueBuilder queueBuilder;

    queueBuilder.SetDevice(&globals.tDevice);
    queueBuilder.SetDefaults();

    queueBuilder.SetComputeMemorySize(0);
    queueBuilder.SetCommandMemorySize(0x10000);
    queueBuilder.SetCommandFlushThreshold(0x10000);

    size_t neededQueueMemorySize = queueBuilder.GetQueueMemorySize();


    globals.pQueueMemory = NVNMemoryAllocator::Allocate(neededQueueMemorySize, 4096, NVNALLOCTYPE_PERSISTENT);
    queueBuilder.SetQueueMemory(globals.pQueueMemory, neededQueueMemorySize);

    globals.tQueue.Initialize(&queueBuilder);
}







void GFXNX64NVN::sCreateSwapChain()
{
    nvn::Texture * targetArray[3];

    for (uint32 i = 0; i < 3; i++)
    {
        NVNTexInfo & rtInfo = globals.tSwapChainBuffer[i];

        rtInfo.eTexFormat = TEXTUREFORMAT_RGBA8888;
        rtInfo.dwMSAALevel = 1;
        rtInfo.dwLastFrameUsed = 0;
        rtInfo.eType = NVNTEXTYPE_2D;
        rtInfo.dwPixelWidth = globals.dwDeviceWidth;
        rtInfo.dwPixelHeight = globals.dwDeviceHeight;
        rtInfo.dwPixelDepth = 1;
        rtInfo.dwMipLevels = 1;

        rtInfo.CreateNVNTexture(FALSE, TRUE, TRUE, FALSE);

        targetArray[i] = rtInfo.ptTexture;
    }

    nvn::WindowBuilder windowBuilder;
    windowBuilder.SetDevice(&globals.tDevice);
    windowBuilder.SetDefaults();
    windowBuilder.SetNativeWindow(globals.tViNativeWindow);
    windowBuilder.SetTextures(3, targetArray);
    windowBuilder.SetPresentInterval(globals.nPresentInterval);
    globals.tWindow.Initialize(&windowBuilder);




    globals.tBackBufferSync.Initialize(&globals.tDevice);
}




void GFXNX64NVN::sInitGpuTimers()
{
    for (int i = 0; i < 3; i++)
    {
        globals.tFrameProfiles[i].Init();
    }
    globals.nCurrentProfileFrameIndex = 0;
}




void GFXNX64NVN::sReleaseGpuTimers()
{
    for (int i = 0; i < 3; i++)
    {
        globals.tFrameProfiles[i].Dispose();
    }
    globals.nCurrentProfileFrameIndex = 0;
}




inline void GFXNX64NVN::sUpdateGpuTimers()
{


    globals.nCurrentProfileFrameIndex = (uint32)(globals.nCurrentProfileFrameIndex + 1) % 3;
    globals.tFrameProfiles[globals.nCurrentProfileFrameIndex].Reset();
}




inline int32 GFXNX64NVN::sGpuTimerWriteIndex()
{
    return globals.nCurrentProfileFrameIndex;
}




inline int32 GFXNX64NVN::sGpuTimerReadIndex()
{
    return (uint32)(globals.nCurrentProfileFrameIndex + 1) % 3;
}




inline void GFXNX64NVN::GpuTimer::Init()
{
    ptStartCounter = (nvn::CounterData *)globals.tGPUEventsMemory.GetOffsetAddress(globals.tGPUEventsMemory.Allocate(sizeof(nvn::CounterData), 16));
    ptEndCounter = (nvn::CounterData *)globals.tGPUEventsMemory.GetOffsetAddress(globals.tGPUEventsMemory.Allocate(sizeof(nvn::CounterData), 16));
    ResetFrameData();
}




inline void GFXNX64NVN::GpuTimer::Dispose()
{

    ptStartCounter = NULL; ptEndCounter = NULL;
}




inline void GFXNX64NVN::GpuTimer::Start(const char * pTimerName, GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals, uint32 dwTimerDepth)
{
    ptStartCounter->counter = COUNTERSTATE_INPROGRESS;
    ptRenderWorkerGlobals->tCommandBuffer.ReportCounter(nvn::CounterType::TIMESTAMP, GetCounterAddress(ptStartCounter));
    pName = pTimerName;
    dwDepth = dwTimerDepth;
}




inline void GFXNX64NVN::GpuTimer::Stop(GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals)
{
    ptEndCounter->counter = COUNTERSTATE_INPROGRESS;
    ptRenderWorkerGlobals->tCommandBuffer.ReportCounter(nvn::CounterType::TIMESTAMP, GetCounterAddress(ptEndCounter));
}




inline void GFXNX64NVN::GpuTimer::ResetFrameData()
{
    ptStartCounter->counter = COUNTERSTATE_INVALID;
    ptEndCounter->counter = COUNTERSTATE_INVALID;
    ptIdleChild = NULL;
}





inline void GFXNX64NVN::GpuTimer::AddIdleChild(GFXNX64NVN::GpuTimer * ptChild)
{
    ptIdleChild = ptChild;
}




inline nvn::BufferAddress GFXNX64NVN::GpuTimer::GetCounterAddress(nvn::CounterData * ptCounter)
{
    return globals.tGPUEventsMemory.tMemContainer.tNVNBufferAddress + globals.tGPUEventsMemory.GetAddressOffset(ptCounter);
}




uint32 GFXNX64NVN::GpuTimer::GetTimeDeltaUs(BOOL bSubtractIdleTime)
{
    uint32 dwTimeDelta = 0;

    volatile nvn::CounterData * ptVolatileStart = ptStartCounter;
    volatile nvn::CounterData * ptVolatileEnd = ptEndCounter;

    if (ptVolatileEnd->counter == COUNTERSTATE_INPROGRESS)
    {

        FrameTimer::IdleScope renderIdle(sgtAppGlobals.renderTimer);
        while (ptVolatileEnd->counter == COUNTERSTATE_INPROGRESS)
        {
            nn::os::YieldThread();
        }
    }

    if (ptVolatileStart->counter == COUNTERSTATE_DONE && ptVolatileEnd->counter == COUNTERSTATE_DONE)
    {
        uint64 dwStartTimeNs = globals.tDevice.GetTimestampInNanoseconds(ptStartCounter);
        uint64 dwEndTimeNs = globals.tDevice.GetTimestampInNanoseconds(ptEndCounter);
        dwTimeDelta = (dwEndTimeNs - dwStartTimeNs) / 1000;
    }

    if (ptIdleChild && bSubtractIdleTime)
    {
        dwTimeDelta -= ptIdleChild->GetTimeDeltaUs(FALSE);
    }

    return dwTimeDelta;
}




inline void GFXNX64NVN::FrameProfile::Init()
{
    for (int i = 0; i < (int)TimerCount; i++)
    {
        tTimers[i].Init();
    }

    tActiveTimerStack = new blz::vector<GpuTimer *>();
}




inline void GFXNX64NVN::FrameProfile::Dispose()
{
    for (int i = 0; i < (int)TimerCount; i++)
    {
        tTimers[i].Dispose();
    }
    delete tActiveTimerStack; tActiveTimerStack = NULL;
}




inline void GFXNX64NVN::FrameProfile::Reset()
{
    for (int i = 0; i < dwNumTimersUsed; i++)
    {
        tTimers[i].ResetFrameData();
    }

    dwNumTimersUsed = 0;
    tActiveTimerStack->clear();
}




void GFXNX64NVN::FrameProfile::StartTimer(const char * pName, GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals, BOOL bIdleTime)
{

    GpuTimer * ptTimer = &tTimers[dwNumTimersUsed];
    ptTimer->Start(pName, ptRenderWorkerGlobals, tActiveTimerStack->size());

    tActiveTimerStack->push_back(ptTimer);

    if (bIdleTime)
    {
        for (size_t i = 0; tActiveTimerStack->size() > i; i++)
        {
            (*tActiveTimerStack)[i]->AddIdleChild(ptTimer);
        }
    }

    dwNumTimersUsed++;
}





inline void GFXNX64NVN::FrameProfile::StopTimer(GFXNX64NVN::RenderWorkerGlobals * ptRenderWorkerGlobals)
{
    GpuTimer * ptTimer = tActiveTimerStack->back();
    ptTimer->Stop(ptRenderWorkerGlobals);
    tActiveTimerStack->pop_back();
}




inline uint32 GFXNX64NVN::FrameProfile::GetFrameTimeUs()
{
    uint32 frameTimeUs = 0;

    if (dwNumTimersUsed != 0)
    {
        GpuTimer & frameTimer = tTimers[0];

        frameTimeUs = frameTimer.GetTimeDeltaUs(1);
    }
    return frameTimeUs;
}









































static void sAddVertElement(VBFormatInfo & tInfo, const uint16 wOffset, const nvn::Format tDataType, const NVNSemantic eSemantic)
{




    VertexElement tElem;
    tElem.wOffset = wOffset;
    tElem.dwType = tDataType;
    tElem.bUsage = eSemantic;
    tElem.bIndex = 0;
    tElem.szSemantic = NULL;

    tInfo.ptElemList->AddTail_Tracked(tElem);
}




static void sVBFormatInfoInit(VBFormatInfo * ptVBFormatInfo)
{

    for (int32 i = 0; i < NUM_VBFORMATS; i++)
    {
        VBFormatInfo & tInfo = ptVBFormatInfo[i];

        tInfo.ptElemList = new VertexElementList;

        switch (i)
        {
        case VBFORMAT_NOTEX_L:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            break;

        case VBFORMAT_BILLBOARD:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8UI, NVNSEMANTIC_NORMAL);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 24, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 28, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_1);
            sAddVertElement(tInfo, 32, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_2);
            sAddVertElement(tInfo, 36, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_3);
            break;

        case VBFORMAT_1TEX_L:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8UI, NVNSEMANTIC_NORMAL);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 24, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            break;

        case VBFORMAT_SPRITE:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 20, nvn::Format::RG32F, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 28, nvn::Format::RG32F, NVNSEMANTIC_TEXCOORD_1);
            sAddVertElement(tInfo, 36, nvn::Format::RG32F, NVNSEMANTIC_TEXCOORD_2);
            break;

        case VBFORMAT_2TEX_L:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8UI, NVNSEMANTIC_NORMAL);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 24, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 28, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_1);
            break;

        case VBFORMAT_SKIN:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8UI, NVNSEMANTIC_NORMAL);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 24, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 28, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_1);
            sAddVertElement(tInfo, 32, nvn::Format::RGBA8UI, NVNSEMANTIC_BLEND_INDICES);
            sAddVertElement(tInfo, 36, nvn::Format::RGBA8, NVNSEMANTIC_BLEND_WEIGHTS);
            break;

        case VBFORMAT_CLOTH:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_TEXCOORD_2);
            break;

        case VBFORMAT_D3DXATLAS:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_1);
            sAddVertElement(tInfo, 32, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_2);
            break;

        case VBFORMAT_2TEX_L_CPOS:
            sAddVertElement(tInfo, 0, nvn::Format::RGBA16SN, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 8, nvn::Format::RGBA8UI, NVNSEMANTIC_NORMAL);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 24, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_1);
            break;

        case VBFORMAT_SKIN_CPOS:
            sAddVertElement(tInfo, 0, nvn::Format::RGBA16SN, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 8, nvn::Format::RGBA8UI, NVNSEMANTIC_NORMAL);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 24, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_1);
            sAddVertElement(tInfo, 28, nvn::Format::RGBA8UI, NVNSEMANTIC_BLEND_INDICES);
            sAddVertElement(tInfo, 32, nvn::Format::RGBA8, NVNSEMANTIC_BLEND_WEIGHTS);
            break;

        case VBFORMAT_BILLBOARD_PARTICLE:
            sAddVertElement(tInfo, 0, nvn::Format::RGB32F, NVNSEMANTIC_POSITION);
            sAddVertElement(tInfo, 12, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_0);
            sAddVertElement(tInfo, 16, nvn::Format::RGBA8, NVNSEMANTIC_COLOR_1);
            sAddVertElement(tInfo, 20, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_0);
            sAddVertElement(tInfo, 24, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_1);
            sAddVertElement(tInfo, 28, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_2);
            sAddVertElement(tInfo, 32, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_3);
            sAddVertElement(tInfo, 36, nvn::Format::RGBA8UI, NVNSEMANTIC_TEXCOORD_4);
            sAddVertElement(tInfo, 40, nvn::Format::RGBA32F, NVNSEMANTIC_TEXCOORD_5);
            break;
        }
    }
}





static void sVBFormatInfoDispose(VBFormatInfo * ptVBFormatInfo)
{

    if (ptVBFormatInfo)
    {
        for (int i = 0; i < NUM_VBFORMATS; i++)
        {
            VBFormatInfo & tInfo = ptVBFormatInfo[i];

            delete tInfo.ptElemList;
            tInfo.ptElemList = NULL;
        }
    }
}




void NVNShaderStateBlock::ClearShaderStateBlock()
{
    eDepthCompareFunc = COMPARE_LESS;
    bDepthWriteEnabled = TRUE;
    bStencilEnabled = FALSE;
    eStencilCompareFunc = COMPARE_ALWAYS;
    nStencilRefValue = 0;
    eStencilOpPass = STENCILOP_KEEP;
    eStencilOpFail = STENCILOP_KEEP;
    eStencilOpZFail = STENCILOP_KEEP;
    bBlendingEnabled = FALSE;
    eBlendOp = BLENDOP_ADD;
    eSourceBlendFactor = BLENDFACTOR_ONE;
    eDestBlendFactor = BLENDFACTOR_ZERO;
    bColorWriteEnabled = TRUE;
    bAlphaWriteEnabled = TRUE;
    eCullMode = CULLMODE_CW;
    eFillMode = FILLMODE_SOLID;
    SigmaMemoryClear(tSamplerSetups, sizeof(tSamplerSetups));
}









inline void NVNState::Clear()
{

    SigmaMemoryFill(&tFlags, 0xff, sizeof(tFlags));
    DirtyAllTextureResources();


    SigmaMemoryClear(ptCurrentRenderTarget, sizeof(ptCurrentRenderTarget));
    ptPixelShader = NULL;
    ptVertexShader = NULL;
    ptCurrentDepthTarget = NULL;

    SigmaMemoryClear(ptPSTextureResources, sizeof(ptPSTextureResources));

    SigmaMemoryClear(hvbCurrent, sizeof(hvbCurrent));
    SigmaMemoryClear(dwCurrentStreamOffset, sizeof(dwCurrentStreamOffset));
    hibCurrent = NULL;

    for (int i = 0; i < 3; i++)
    {
        eStreamVBFormat[i] = (VBFormat)-1;
    }

    tViewportRect.Set(0, 0, 1, 1);

    tDepthStencilState.SetDefaults().SetDepthTestEnable(TRUE);

    bInStateBlock = FALSE;

    ClearShaderStateBlock();
}




inline void GFXNX64NVN::sInitConstantBufferMemory()
{

    globals.dwConstantBufferSize[0] = 0x950;
    globals.dwConstantBufferSize[1] = 0x870;
    globals.dwConstantBufferSize[2] = 0x1830;


    uint32 totalConstantBufferSize = 0;
    for (int i = 0; i < 3; i++)
    {
        totalConstantBufferSize += nn::util::align_up(globals.dwConstantBufferSize[i], 256);  // folded: only the rounded total 0x3000 is in the image
    }


    globals.tConstantBufferMemory.Init(totalConstantBufferSize, 256, NVNGraphicsMemory::MEMTYPE_DYNAMIC_BUFFER);  // alignment assumed: Allocate rounds it up to 4096

    nvn::BufferAddress curBufferAddress = globals.tConstantBufferMemory.tNVNBufferAddress;
    for (int i = 0; i < 3; i++)
    {
        globals.tConstantBuffers[i] = curBufferAddress;
        curBufferAddress += nn::util::align_up(globals.dwConstantBufferSize[i], 256);
    }
}




void GFXNX64NVN::EarlyInit()

{

    SigmaMemoryClear(&globals, sizeof(globals));









    sInitNVMemory();
    sInitVideo();
    sInitDevice();

    void * pGPUHeapMemory = malloc(sizeof(NX64NVNHeap));
    gptGPUHeap = new (pGPUHeapMemory) NX64NVNHeap(GPU_HEAP_SIZE, &globals.tDevice, nvn::MemoryPoolFlags::CPU_CACHED | nvn::MemoryPoolFlags::GPU_CACHED);
    gptGPUHeap->AddHeap(0x4000000, 0x20000, 0x8000000);
    gptGPUHeap->AddHeap(0x800000, 0x4000, 0x18000);
    gptGPUHeap->AddHeap(0x100000, 0x800, 0x3000);
    gptGPUHeap->AddHeap(0x20000, 0x100, 0x600);
    gptGPUHeap->AddHeap(0x4000, 0x20, 0xc0);
    gptGPUHeap->Preallocate(GPU_HEAP_SIZE);
    gptSecondaryDefinitionsMemoryHeap = gptGPUHeap;
}




BOOL GFXNX64NVN::Init(GfxOptions * ptOptions, VBFormatInfo * ptVBFormatInfo)


{
    globals.dwDeviceWidth = 1600;
    globals.dwDeviceHeight = 900;

    globals.dwReducedDeviceWidth = 1280;
    globals.dwReducedDeviceHeight = 720;

    globals.flDeviceAspectRatio = 16.0f / 9.0f;
    globals.dwFrameCount = 0;

    globals.fGammaValue = 1.0f;
    globals.bIsDocked = ShellShouldSelectFullDisplayMode();


    globals.nPresentInterval = 1;




    globals.nSwapChainBackBufferIndex = -1;

    sInitNVN();

    globals.tGPUEventsMemory.Init(0x10000, 4096, NVNGraphicsMemory::MEMTYPE_GPU_EVENTS);  // alignment assumed (any value up to 4096 gives the same code)
    globals.tShaderAllocator.Create(gptShaderHeap);
    {
        nvn::EventBuilder eventBuilder;
        eventBuilder.SetDefaults();
        eventBuilder.SetStorage(globals.tGPUEventsMemory.GetMemoryPool(), globals.tGPUEventsMemory.Allocate(4, 4));
        globals.tProcessedFrameCount.Initialize(&eventBuilder);

        globals.tProcessedFrameCount.Signal(nvn::EventSignalMode::WRITE, globals.dwFrameCount);
    }

    globals.tCommandBufferMemory.Init();

    globals.ptVBFormatInfo = ptVBFormatInfo;
    sVBFormatInfoInit(ptVBFormatInfo);

    for (int32 i = 0; i < NUM_VBFORMATS; i++)
    {
        VBFormatInfo & tInfo = globals.ptVBFormatInfo[i];
        globals.tStreamStates[i].SetDefaults().SetStride(tInfo.dwSize);
    }

    sInitConstantBufferMemory();





    for (int32 worker = 0; worker < 1; worker++)
    {
        workerGlobals[worker].Init();
    }

    for (int32 i = 0; i < 3; i++)
    {
        const uint32 dwPerFrameDynamicBufferSize = 0x2000000;
        globals.tDynamicBuffers[i].Init(dwPerFrameDynamicBufferSize, 4096, NVNGraphicsMemory::MEMTYPE_DYNAMIC_BUFFER);  // alignment assumed, as above
    }
    globals.ptDynamicBuffer = NULL;

    globals.ptVertexAttribMap = new NVNVertexAttribMap(16, NULL);
    globals.ptVertexAttribMap->SetHashTableSize_Tracked(512);



    {
        const uint32 dwMissingTexureSize = 8;

        globals.tMissing2DTexture.eTexFormat = TEXTUREFORMAT_RGBA8888;
        globals.tMissing2DTexture.dwMSAALevel = 1;
        globals.tMissing2DTexture.dwLastFrameUsed = 0;
        globals.tMissing2DTexture.eType = NVNTEXTYPE_2D;
        globals.tMissing2DTexture.dwPixelWidth = dwMissingTexureSize;
        globals.tMissing2DTexture.dwPixelHeight = dwMissingTexureSize;
        globals.tMissing2DTexture.dwPixelDepth = 1;
        globals.tMissing2DTexture.dwMipLevels = 1;
        globals.tMissing2DTexture.CreateNVNTexture(TRUE, FALSE, FALSE, FALSE);

        uint32 * checkerboardMemory = (uint32 *)globals.tMissing2DTexture.tMemory.ptMemory;
        for (int i = 0; i < dwMissingTexureSize; i++)
        {
            for (int j = 0; j < dwMissingTexureSize; j++)
            {
                uint32 color = ((i + j) & 1) ? 0xffffffff : 0xff000000;  // expression inferred: the image has only the 64 folded texel stores
                checkerboardMemory[i * dwMissingTexureSize + j] = color;
            }
        }
    }



















    {
        nvn::SamplerBuilder samplerBuilder;
        samplerBuilder.SetDefaults();
        samplerBuilder.SetDevice(&globals.tDevice);
        samplerBuilder.SetWrapMode(nvn::WrapMode::CLAMP_TO_EDGE, nvn::WrapMode::CLAMP_TO_EDGE, nvn::WrapMode::CLAMP_TO_EDGE);
        samplerBuilder.SetMinMagFilter(nvn::MinFilter::NEAREST, nvn::MagFilter::NEAREST);
        globals.tGenericSampler.tSampler.Initialize(&samplerBuilder);
        globals.tIDManager.RegisterSampler(globals.tGenericSampler);
    }


    sCreateGlobalStateObjects();
    sCreateOcclusionQueries();
    sInitGpuTimers();

    globals.tWorkerLock.Create();




    return TRUE;
}




BOOL GFXNX64NVN::Dispose()

{




    globals.tWorkerLock.Destroy();

    sReleaseGpuTimers();
    sDestroyOcclusionQueries();

    for (int32 worker = 0; worker < 1; worker++)
    {
        workerGlobals[worker].Dispose();
    }




    if (globals.ptVertexAttribMap)
    {
        POS pos = globals.ptVertexAttribMap->GetStartAssoc();
        while (pos)
        {
            NVNVertexInputBindingSet * ptInputBindings;
            uint64 dwHash;
            globals.ptVertexAttribMap->GetNextAssoc(pos, dwHash, ptInputBindings);
            delete ptInputBindings;
        }
        delete globals.ptVertexAttribMap; globals.ptVertexAttribMap = NULL;
    }

    sVBFormatInfoDispose(globals.ptVBFormatInfo);
    globals.ptVBFormatInfo = NULL;

    globals.tIDManager.Dispose();

    globals.tCommandBufferMemory.Dispose();

    globals.tProcessedFrameCount.Finalize();
    globals.tGPUEventsMemory.Dispose();
    globals.tShaderAllocator.Destroy();



    return TRUE;
}





void GFXNX64NVN::GetCaps(DisplayCaps & tCaps)
{
    tCaps.dwFlags = (1 << DISPLAY_SUPPORTS_GAMMA_CONTROL_BIT) | (1 << DISPLAY_SUPPORTS_SM4_BIT);
    tCaps.dwMaxVertexStreams = 16;
    tCaps.dwMaxAnisotropy = 16;
    tCaps.dwMaxSimultaneousRTs = 8;
}



















void GFXNX64NVN::EvictResources()
{
}














BOOL GFXNX64NVN::GetCurrentDisplayMode(DisplayMode & tDisplayMode)
{
    tDisplayMode.nWinLeft = 0;
    tDisplayMode.nWinTop = 0;


    if (ShellShouldSelectFullDisplayMode())
    {
        tDisplayMode.nWinWidth = globals.dwDeviceWidth;
        tDisplayMode.nWinHeight = globals.dwDeviceHeight;
    }
    else
    {
        tDisplayMode.nWinWidth = globals.dwReducedDeviceWidth;
        tDisplayMode.nWinHeight = globals.dwReducedDeviceHeight;
    }
    tDisplayMode.dwUIOptWidth = tDisplayMode.nWinWidth;
    tDisplayMode.dwUIOptHeight = tDisplayMode.nWinHeight;
    tDisplayMode.dwWidth = tDisplayMode.nWinWidth;
    tDisplayMode.dwHeight = tDisplayMode.nWinHeight;










    tDisplayMode.dwMSAALevel = 0;
    tDisplayMode.nRefreshRate = 60;
    tDisplayMode.dwBitDepth = 32;
    tDisplayMode.flAspectRatio = globals.flDeviceAspectRatio;
    return TRUE;
}




inline void GFXNX64NVN::GetDeviceResolution(uint32 & dwDeviceWidth, uint32 & dwDeviceHeight)
{
    dwDeviceWidth = globals.dwDeviceWidth;
    dwDeviceHeight = globals.dwDeviceHeight;
}




inline void GFXNX64NVN::GetReducedDeviceResolution(uint32 & dwReducedDeviceWidth, uint32 & dwReducedDeviceHeight)
{
    dwReducedDeviceWidth = globals.dwReducedDeviceWidth;
    dwReducedDeviceHeight = globals.dwReducedDeviceHeight;
}




void GFXNX64NVN::GetValidDisplayModes(DisplayMode *& ptDisplayModes, uint32 & dwDisplayModeCount)
{
    dwDisplayModeCount = 2;
    ptDisplayModes = (DisplayMode *)SigmaMemoryNew(dwDisplayModeCount * sizeof(DisplayMode), 4, NULL, TRUE);

    DisplayMode & tMode0 = ptDisplayModes[0];
    SigmaMemoryClear(&tMode0, sizeof(tMode0));
    GetDeviceResolution(tMode0.dwWidth, tMode0.dwHeight);
    tMode0.nRefreshRate = 60;
    tMode0.dwBitDepth = 32;
    tMode0.dwMSAALevel = 1;
    tMode0.flAspectRatio = globals.flDeviceAspectRatio;

    DisplayMode & tMode1 = ptDisplayModes[1];
    SigmaMemoryClear(&tMode1, sizeof(tMode1));
    GetReducedDeviceResolution(tMode1.dwWidth, tMode1.dwHeight);
    tMode1.nRefreshRate = 60;
    tMode1.dwBitDepth = 32;
    tMode1.dwMSAALevel = 1;
    tMode1.flAspectRatio = globals.flDeviceAspectRatio;
}



















void GFXNX64NVN::GetDeviceDescription(GfxDeviceDesc & desc)
{
    SigmaMemoryClear(&desc, sizeof(desc));
}





void GFXNX64NVN::GetValidTextureFormats(BOOL * pfTextureFormatValid)
{

    for (int32 i = 0; i < 44; i++)
    {
        nvn::Format eFormat = sTextureFormatToNVNFormat((TextureFormat)i, NULL);
        for (int32 nUsage = 0; nUsage < 3; nUsage++)
        {
            pfTextureFormatValid[nUsage * 44 + i] = eFormat != nvn::Format::NONE;
        }
    }
}




uint32 GFXNX64NVN::GetShadowTech()
{
    return 4;
}





BOOL GFXNX64NVN::CreateSurface(const DisplayMode & tMode)
{
    return TRUE;
}





BOOL GFXNX64NVN::CloseSurface()
{
    return TRUE;
}


inline void GFXNX64NVN::RenderWorkerGlobals::Init()
{
    RenderCommandContext::Init();



    for (int i = 0; i < 3; i++)
    {
        ppWorkingConstantBuffers[i] = (uint8 *)SigmaMemoryNew(globals.dwConstantBufferSize[i], 4, NULL, FALSE);
    }

    Reset();
}

void GFXNX64NVN::RenderWorkerGlobals::Reset()
{

    RenderCommandContext::Reset();

    tState.Clear();

    SigmaMemoryClear(ptConstFastBindingTable, sizeof(ptConstFastBindingTable));
}

inline void GFXNX64NVN::RenderWorkerGlobals::BeginFrame()
{

    for (int i = 0; i < 3; i++)
    {
        tCommandBuffer.BindUniformBuffer(nvn::ShaderStage::VERTEX, i, globals.tConstantBuffers[i], globals.dwConstantBufferSize[i]);
        tCommandBuffer.BindUniformBuffer(nvn::ShaderStage::FRAGMENT, i, globals.tConstantBuffers[i], globals.dwConstantBufferSize[i]);
    }


    tCommandBuffer.SetTexturePool(&globals.tIDManager.tTexturePool);
    tCommandBuffer.SetSamplerPool(&globals.tIDManager.tSamplerPool);
}

void GFXNX64NVN::RenderCommandContext::Init()
{
    tCommandBuffer.Initialize(&globals.tDevice);

    tCommandBuffer.SetMemoryCallback(
        [](nvn::CommandBuffer * cmdBuf, nvn::CommandBufferMemoryEvent::Enum event, size_t minSize, void * callbackData)
        {
            if (event == nvn::CommandBufferMemoryEvent::OUT_OF_COMMAND_MEMORY)
            {
                globals.tCommandBufferMemory.AddCommandMemory(*cmdBuf);
            }
            else
            {

                globals.tCommandBufferMemory.AddControlMemory(*cmdBuf);
            }
        });
}

void GFXNX64NVN::RenderCommandContext::Dispose()
{
    tCommandBuffer.Finalize();
}

void GFXNX64NVN::RenderCommandContext::Reset()
{
    Dispose();
    Init();

    globals.tCommandBufferMemory.AddCommandMemory(tCommandBuffer);
    globals.tCommandBufferMemory.AddControlMemory(tCommandBuffer);
}




BOOL GFXNX64NVN::RenderBegin()

{




    BOOL bWasDocked = globals.bIsDocked;


    globals.bIsDocked = ShellIsInBoostMode();

    if (bWasDocked != globals.bIsDocked)
    {
        SetUICConversionConstansts();
    }


    {
        FrameTimer::IdleScope renderIdle(sgtAppGlobals.renderTimer);
        nvn::WindowAcquireTextureResult result = globals.tWindow.AcquireTexture(&globals.tBackBufferSync, &globals.nSwapChainBackBufferIndex);


    }

    globals.bNeedToWaitForBackBufferSync = TRUE;


    globals.ptDynamicBuffer = &globals.tDynamicBuffers[globals.dwFrameCount % 3];
    globals.ptDynamicBuffer->ClearAllocations();

    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    renderWorkerGlobals.Reset();
    renderWorkerGlobals.tCommandBuffer.BeginRecording();
    renderWorkerGlobals.BeginFrame();

    globals.tFrameProfiles[sGpuTimerWriteIndex()].StartTimer(szTotalFrameTimeName, &renderWorkerGlobals, FALSE);

    return TRUE;
}




void GFXNX64NVN::RenderEnd()
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    globals.tCommandBufferMemory.EndFrame();


    sGammaCorrection();

    globals.tFrameProfiles[sGpuTimerWriteIndex()].StopTimer(&renderWorkerGlobals);
    renderWorkerGlobals.tCommandBuffer.SignalEvent(&globals.tProcessedFrameCount,
                                                   nvn::EventSignalMode::WRITE,
                                                   nvn::EventSignalLocation::BOTTOM,
                                                   nvn::EventSignalFlags::Enum(0),
                                                   globals.dwFrameCount);

    nvn::CommandHandle commandHandle = renderWorkerGlobals.tCommandBuffer.EndRecording();
    globals.tQueue.SubmitCommands(1, &commandHandle);
    globals.ptDynamicBuffer = NULL;




}




void GFXNX64NVN::sGammaCorrection()
{


    SetRenderTargetToSwapChainRT(0);
    TextureSetDepthTarget(NULL, FALSE);

    ScopedShaderDefinitionLockR pdefShader(GlobalSNOGet(GLOBALSNO_SHADER_GAMMA_CORRECTION));

    StateBlockApply(pdefShader->ptPasses->tPostprocessed.ptStateBlock);

    SetVertexShader(pdefShader, 0);
    SetPixelShader(pdefShader, 0);

    TextureSetCurrent(0, &globals.tSwapChainBuffer[globals.nSwapChainBackBufferIndex]);

    uint32 width = sSelectRenderTargetAlias() == NVNTexInfo::TEXSELECTION_FULL ? globals.dwDeviceWidth : globals.dwReducedDeviceWidth;
    uint32 height = sSelectRenderTargetAlias() == NVNTexInfo::TEXSELECTION_FULL ? globals.dwDeviceHeight : globals.dwReducedDeviceHeight;
    IRect2D fullScreen(0, 0, width, height);
    ViewportSet(fullScreen);
    ScissorRectSet(FALSE, fullScreen);


    {
        RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

        const ShaderConstFastBinding & gammaValueBinding = renderWorkerGlobals.ptConstFastBindingTable[0];  // enumerator 0 is VSC_FILTERTAPS0; the gamma shader reuses the slot


        const ShaderConstFastBinding & windowResBinding = renderWorkerGlobals.ptConstFastBindingTable[VSC_WINDOW_RESOLUTION];


        Vector4D gammaConstant(globals.fGammaValue, 0.0f, 0.0f, 0.0f);
        Vector4D windowResConstant(1.0f / globals.dwDeviceWidth, 1.0f / globals.dwDeviceHeight, 0.0f, 0.0f);

        SetPixelShaderConstant(&gammaValueBinding.tPSBinding, &gammaConstant, 1);
        SetPixelShaderConstant(&windowResBinding.tPSBinding, &windowResConstant, 1);
    }

    Draw(DRAWMODE_TRIANGLELIST, 0, 0, 3, 0, 1);
}




void GFXNX64NVN::ResetStates()
{
    GfxVBFlushAllDynamic();
    GfxIBFlushAllDynamic();
}








void GFXNX64NVN::BeginWorker(const int32 nWorkerIndex)
{
}






void GFXNX64NVN::EndWorker()
{
}






void GFXNX64NVN::RenderClear(BOOL fClearTarget,
                             BOOL fClearZBuffer,
                             BOOL fClearStencilBuffer,
                             const RGBAColor & rgbaTargetColor)
{

    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    sApplyRenderTargetState(renderWorkerGlobals.tState);

    if (fClearTarget && renderWorkerGlobals.tState.ptCurrentRenderTarget[0])
    {

        RGBAColorValue color; color.Set(rgbaTargetColor);
        renderWorkerGlobals.tCommandBuffer.ClearColor(0, &color.r, nvn::ClearColorMask::RGBA);
    }

    if ((fClearZBuffer || fClearStencilBuffer) && renderWorkerGlobals.tState.ptCurrentDepthTarget)
    {


        renderWorkerGlobals.tCommandBuffer.SetStencilCullCriteria(nvn::StencilFunc::EQUAL, 1, 0xff);
        renderWorkerGlobals.tCommandBuffer.ClearDepthStencil(1.0f, fClearZBuffer, 0, fClearStencilBuffer ? 0xff : 0);
    }
}




BOOL GFXNX64NVN::ChangeResolution(const DisplayMode & tMode)
{
    return TRUE;
}





BOOL GFXNX64NVN::DeviceReset()
{
    return TRUE;
}










BOOL GFXNX64NVN::RenderPresent()
{






    int width = globals.bIsDocked ? globals.dwDeviceWidth : globals.dwReducedDeviceWidth;
    int height = globals.bIsDocked ? globals.dwDeviceHeight : globals.dwReducedDeviceHeight;

    globals.tWindow.SetCrop(0, 0, width, height);
    globals.tQueue.PresentTexture(&globals.tWindow, globals.nSwapChainBackBufferIndex);


    globals.dwFrameCount++;
    sUpdateGpuTimers();

    return TRUE;
}












uint32 GFXNX64NVN::DeviceFrameTimeUs()
{

    return globals.tFrameProfiles[sGpuTimerReadIndex()].GetFrameTimeUs();
}














void GFXNX64NVN::BlockUntilIdle()
{





    globals.tQueue.Finish();
}




void GFXNX64NVN::AcquireThreadOwnership()
{
}





void GFXNX64NVN::ReleaseThreadOwnership()
{
}


































BOOL GFXNX64NVN::TextureIsBusy(HTexture hTexture)
{
    return NVNIsResourceBusy((NVNTexInfo *)hTexture);
}











































void GFXNX64NVN::SetShaderGPRAllocation(const uint32 dwVertexShaderCount, const uint32 dwPixelShaderCount)
{
}






void GFXNX64NVN::RestoreShaderGPRAllocation()
{
}





void GFXNX64NVN::ScreenGrab(void * lpDestBuffer, const int32 nDestWidth, const int32 nDestHeight, const int32 nDestPitch, const BOOL fBlitCursor)
{
}













































































inline NVNTexInfo::NVNTexInfo()
    : eTexFormat(TEXTUREFORMAT_UNKNOWN)
    , dwMSAALevel(1)
    , dwLastFrameUsed(0)
    , eType((NVNTexType)-1)
    , dwPixelWidth(0)
    , dwPixelHeight(0)
    , dwPixelDepth(1)
    , dwMipLevels(0)
    , nTextureID(0), nReducedTextureID(0), ptTexture(NULL), ptReducedTexture(NULL)
{



    for (uint32 i = 0; i < 1; i++)
    {
        bTargetUpdated[i] = FALSE;
    }
}





NVNTexInfo::~NVNTexInfo()
{
    if (eType != NVNTEXTYPE_2D_ALIAS)
    {







        ptTexture->Finalize();
        delete ptTexture; ptTexture = NULL;
        if (HasReducedRenderTarget())
        {
            ptReducedTexture->Finalize();
            delete ptReducedTexture; ptReducedTexture = NULL;
        }
    }

}




void NVNTexInfo::CreateNVNTexture(BOOL bLinearLayout, BOOL bRenderTarget, BOOL bSwapChainTarget, BOOL bDepthTarget)
{



    bIsWindowTexture = bSwapChainTarget;


    nvn::TextureBuilder textureBuilder;
    textureBuilder.SetDevice(&GFXNX64NVN::globals.tDevice);
    textureBuilder.SetDefaults();

    nvn::TextureFlags texFlags;
    if (bRenderTarget)
    {

        texFlags = bSwapChainTarget ? (nvn::TextureFlags::COMPRESSIBLE | nvn::TextureFlags::DISPLAY) : nvn::TextureFlags::COMPRESSIBLE;



        if (bDepthTarget)
        {

            texFlags |= nvn::TextureFlags::ZCULL_SUPPORT_STENCIL;
        }
    }
    else
    {
        texFlags = bLinearLayout ? nvn::TextureFlags::LINEAR : 0;
    }



    nvn::TextureTarget nvnTarget;

    switch (eType)
    {
    case NVNTEXTYPE_2D:
        nvnTarget = nvn::TextureTarget::TARGET_2D;
        break;
    case NVNTEXTYPE_CUBEMAP:
        nvnTarget = nvn::TextureTarget::TARGET_CUBEMAP;
        break;
    case NVNTEXTYPE_3D:
        nvnTarget = nvn::TextureTarget::TARGET_3D;
        break;
    default:
        DisplayInternalError(NULL, "", 1865, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    textureBuilder.SetFlags(texFlags);
    textureBuilder.SetTarget(nvnTarget);
    NVNTexSwizzle tTexSwizzle;
    textureBuilder.SetFormat(sTextureFormatToNVNFormat(eTexFormat, &tTexSwizzle));
    textureBuilder.SetSwizzle(tTexSwizzle.R, tTexSwizzle.G, tTexSwizzle.B, tTexSwizzle.A);
    textureBuilder.SetSize3D(dwPixelWidth, dwPixelHeight, dwPixelDepth);
    textureBuilder.SetLevels(dwMipLevels);

    if (bLinearLayout)
    {

        uint32 dwRowSize = TextureFormatSize(eTexFormat) * dwPixelWidth;
        uint32 dwLinearStride = nn::util::align_up(dwRowSize, 32);
        textureBuilder.SetStride(dwLinearStride);
    }

    size_t bufferSize = textureBuilder.GetStorageSize();
    size_t bufferAlignment = textureBuilder.GetStorageAlignment();

    NVNGraphicsMemory::MemType memType = bRenderTarget ? NVNGraphicsMemory::MEMTYPE_RENDER_TARGET : NVNGraphicsMemory::MEMTYPE_TEXTURE_RESOURCE;
    tMemory.Init(bufferSize, bufferAlignment, memType);
    textureBuilder.SetStorage(tMemory.ptMemoryPool, tMemory.nAllocOffset);



    ptTexture = new nvn::Texture;
    BOOL bTextureInitialized = ptTexture->Initialize(&textureBuilder);



    if (HasReducedRenderTarget())
    {


        uint32 reducedWidth, reducedHeight; GFXNX64NVN::GetReducedDeviceResolution(reducedWidth, reducedHeight);

        switch (GetDeviceResolutionFraction())
        {
        case TEXFRACTION_FULL:
            break;

        case TEXFRACTION_HALF:
            reducedWidth /= 2;
            reducedHeight /= 2;
            break;

        case TEXFRACTION_QUARTER:
            reducedWidth /= 4;
            reducedHeight /= 4;
            break;
        default:
            break;
        }



        textureBuilder.SetSize3D(reducedWidth, reducedHeight, dwPixelDepth);
        ptReducedTexture = new nvn::Texture;
        bTextureInitialized = ptReducedTexture->Initialize(&textureBuilder);
    }
    GFXNX64NVN::RegisterTexture(*this);
}




inline void GFXNX64NVN::sFlushNVNTexture(const NVNTexInfo * ptTexInfo)
{
    nvn::CopyRegion region = { 0, 0, 0, (int)ptTexInfo->dwPixelWidth, (int)ptTexInfo->dwPixelHeight, (int)ptTexInfo->dwPixelDepth };


    ptTexInfo->ptTexture->FlushTexels(NULL, &region);
}




BOOL GFXNX64NVN::TextureCreate(HTexture * phTexture, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwMipMapLevels, const ResourceUsage, const uint32 dwTexCreateFlags, const uint32 dwMSAALevel, const uint32, const int32, const TextureInitialData *, const char * szTexDebugName)
{















    *phTexture = NULL;
    NVNTexInfo * pTexInfo = new NVNTexInfo;

    pTexInfo->eTexFormat = eTexFormat;
    pTexInfo->dwMSAALevel = 1; pTexInfo->dwLastFrameUsed = 0;
    pTexInfo->eType = NVNTEXTYPE_2D; pTexInfo->dwPixelWidth = dwWidth;

    pTexInfo->dwPixelHeight = dwHeight; pTexInfo->dwPixelDepth = 1;

    pTexInfo->dwMipLevels = dwMipMapLevels;

    BOOL bIsLinear = TextureFormatIsLinear(eTexFormat);
    BOOL bIsDepthTarget = (dwTexCreateFlags >> TEXCREATE_DEPTH_TARGET_BIT) & 1;
    BOOL bIsRenderTarget = bIsDepthTarget ? TRUE : (dwTexCreateFlags & (1 << TEXCREATE_RENDER_TARGET_BIT));

    pTexInfo->CreateNVNTexture(bIsLinear, bIsRenderTarget, FALSE, bIsDepthTarget);
    NVNMarkResourceReadyForDelete(pTexInfo);
    *phTexture = pTexInfo;











    return TRUE;
}






void GFXNX64NVN::TextureDispose(HTexture hTexture)
{
    NVNTexInfo * pTexInfo = (NVNTexInfo *)hTexture; globals.tIDManager.UnregisterTexture(*pTexInfo);
    delete pTexInfo;
}




BOOL GFXNX64NVN::TextureLock(HTexture hTexture, uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwPitch, const uint32)
{
    NVNTexInfo * pTexInfo = (NVNTexInfo *)hTexture;
    lpTexture = pTexInfo->tMemory.ptMemory;








    dwPitch = (uint32)pTexInfo->ptTexture->GetStride();
    return TRUE;
}





void GFXNX64NVN::TextureUnlock(HTexture hTexture, uint32 dwMipMapLevel)
{
    NVNTexInfo * ptTexInfo = (NVNTexInfo *)hTexture;


    sFlushNVNTexture(ptTexInfo);
}




void GFXNX64NVN::sSetRenderTarget(const uint32 dwRTIndex, NVNTexInfo * ptRenderTarget)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    if (renderWorkerGlobals.tState.ptCurrentRenderTarget[dwRTIndex] != ptRenderTarget)
    {
        renderWorkerGlobals.tState.ptCurrentRenderTarget[dwRTIndex] = ptRenderTarget;
        renderWorkerGlobals.tState.tFlags.ColorBufferBindingsDirty = 1;
    }
}






void GFXNX64NVN::sSetDepthTarget(NVNTexInfo * ptDepthTarget)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    if (renderWorkerGlobals.tState.ptCurrentDepthTarget != ptDepthTarget)
    {
        renderWorkerGlobals.tState.ptCurrentDepthTarget = ptDepthTarget;
        renderWorkerGlobals.tState.tFlags.DepthBufferBindingsDirty = 1;
    }
}




void GFXNX64NVN::TextureSetSwapChainRenderTarget(HTexture hTexture)
{
}







void GFXNX64NVN::TextureSetRenderTarget(const uint32 dwRTIndex, HTexture hTexture)
{
    sSetRenderTarget(dwRTIndex, (NVNTexInfo *)hTexture);
}






void GFXNX64NVN::TextureSetDepthTarget(HTexture hTexture, BOOL)
{
    sSetDepthTarget((NVNTexInfo *)hTexture);
}






void GFXNX64NVN::sCopyTarget(NVNTexInfo * ptSrcTex, NVNTexInfo * ptDestTex)
{





    nvn::CopyFlags tCopyFlags = nvn::CopyFlags::NONE;
    if (ptSrcTex->dwPixelWidth != ptDestTex->dwPixelWidth || ptSrcTex->dwPixelHeight != ptDestTex->dwPixelHeight)
    {
        tCopyFlags = nvn::CopyFlags::LINEAR_FILTER;
    }

    sPrepareTargetForWrite(ptDestTex);

    nvn::CopyRegion srcRegion = { 0, 0, 0, (int)ptSrcTex->dwPixelWidth, (int)ptSrcTex->dwPixelHeight, 1 };
    nvn::CopyRegion dstRegion = { 0, 0, 0, (int)ptDestTex->dwPixelWidth, (int)ptDestTex->dwPixelHeight, 1 };



    {
        RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];


        renderWorkerGlobals.tCommandBuffer.CopyTextureToTexture(ptSrcTex->GetTexture(sSelectRenderTargetAlias()), NULL, &srcRegion, ptDestTex->GetTexture(sSelectRenderTargetAlias()), NULL, &dstRegion, tCopyFlags);
    }
}




void GFXNX64NVN::sPrepareTargetForWrite(const NVNTexInfo * ptTexInfo)
{
    if (globals.bNeedToWaitForBackBufferSync && &globals.tSwapChainBuffer[globals.nSwapChainBackBufferIndex] == ptTexInfo)
    {
        RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];




        globals.tFrameProfiles[sGpuTimerWriteIndex()].StartTimer("WaitForBackBuffer", &renderWorkerGlobals, TRUE);
        renderWorkerGlobals.tCommandBuffer.WaitSync(&globals.tBackBufferSync);
        globals.tFrameProfiles[sGpuTimerWriteIndex()].StopTimer(&renderWorkerGlobals);

        globals.bNeedToWaitForBackBufferSync = FALSE;
    }
}

















void GFXNX64NVN::TextureCopyDepthTarget(HTexture hTexture)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    sCopyTarget(renderWorkerGlobals.tState.ptCurrentDepthTarget, (NVNTexInfo *)hTexture);
}





BOOL GFXNX64NVN::TextureMSAASupported()
{
    return FALSE;
}





void GFXNX64NVN::TextureResolveMSAARenderTarget(const uint32 dwRTIndex, const HTexture hDestTexture)
{


    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}




void GFXNX64NVN::TextureResolveMSAADepthTarget(const HTexture hDestTexture)
{

    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}




void GFXNX64NVN::SetRenderTargetToSwapChainRT(const uint32 dwRTIndex)
{
    sSetRenderTarget(dwRTIndex, &globals.tSwapChainBuffer[globals.nSwapChainBackBufferIndex]);
}





uint32 CalculateBlockOffset(const NVNTexInfo * pTexInfo, uint32 xBlock, uint32 yBlock)
{
    if (!TextureFormatIsCompressed(pTexInfo->eTexFormat))
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }





    BOOL bIs4BPP = TextureFormatSize(pTexInfo->eTexFormat) == 8;

    const uint32 dwLinearBlockWidth = bIs4BPP ? 8 : 4;
    const uint32 dwLinearBlockHeight = 128;
    const uint32 dwMortonBlockWidth = bIs4BPP ? 4 : 2;
    const uint32 dwMortonBlockHeight = 8;


    uint32 dwLinearBlockStride = (pTexInfo->dwPixelWidth / 4) / dwLinearBlockWidth;
    uint32 dwLinearBlockCol = xBlock / dwLinearBlockWidth;
    uint32 dwLinearBlockRow = yBlock / dwLinearBlockHeight;
    uint32 dwLinearBlockIndex = dwLinearBlockRow * dwLinearBlockStride + dwLinearBlockCol;
    uint32 dwOffsetToLinearBlockStart = dwLinearBlockIndex * dwLinearBlockWidth * dwLinearBlockHeight;


    const uint32 dwNumMortonBlocksX = dwLinearBlockWidth / dwMortonBlockWidth;
    const uint32 dwNumMortonBlocksY = dwLinearBlockHeight / dwMortonBlockHeight;
    uint32 dwMortonBlockCol = (xBlock % dwLinearBlockWidth) / dwMortonBlockWidth;
    uint32 dwMortonBlockRow = (yBlock % dwLinearBlockHeight) / dwMortonBlockHeight;


    uint32 dwMortonBlockIndex = dwMortonBlockRow * dwNumMortonBlocksX + dwMortonBlockCol;
    uint32 dwOffsetToMortonBlockStart = dwMortonBlockIndex * dwMortonBlockWidth * dwMortonBlockHeight;



    uint32 dwMortonElementCol = xBlock % dwMortonBlockWidth;
    uint32 dwMortonElementRow = yBlock % dwMortonBlockHeight;
    const uint32 dwMortonZSize = 2;
    uint32 dwMortonZCol = dwMortonElementCol / dwMortonZSize;
    uint32 dwMortonZRow = dwMortonElementRow / dwMortonZSize;
    uint32 dwMortonElementOffset = (dwMortonZRow * dwMortonBlockWidth + dwMortonZCol * dwMortonZSize) / dwMortonZSize * (dwMortonZSize * dwMortonZSize);
    uint32 dwMortonCol = xBlock % dwMortonZSize;
    uint32 dwMortonRow = yBlock % dwMortonZSize;
    uint32 dwOffsetToMortonElement = bIs4BPP ? (dwMortonCol + dwMortonRow * 2) : (dwMortonRow + dwMortonCol * 2);
    dwOffsetToMortonElement += dwMortonElementOffset;

    return dwOffsetToLinearBlockStart + dwOffsetToMortonBlockStart + dwOffsetToMortonElement;
}




int32 GFXNX64NVN::GetTiledElementByteOffset(HTexture hTexture, int32 nElementX, int32 nElementY)
{
    NVNTexInfo * pTexInfo = (NVNTexInfo *)hTexture;
    uint32 dwBlockOffset = CalculateBlockOffset(pTexInfo, nElementX, nElementY);
    return dwBlockOffset * TextureFormatSize(pTexInfo->eTexFormat);
}









void GFXNX64NVN::GetRenderTargetCurrentResolution(HTexture hTexture, uint32 & width, uint32 & height)
{
    NVNTexInfo * pTexInfo = (NVNTexInfo *)hTexture;


    NVNTexInfo::TexDeviceResolutionFraction fraction = pTexInfo->GetDeviceResolutionFraction();

    if (fraction == NVNTexInfo::TEXFRACTION_NOT_RELATED || globals.bIsDocked)
    {
        width = pTexInfo->dwPixelWidth;
        height = pTexInfo->dwPixelHeight;
    }
    else
    {
        width = (uint32)(pTexInfo->dwPixelWidth * NVNTexInfo::kReducedFraction);
        height = (uint32)(pTexInfo->dwPixelHeight * NVNTexInfo::kReducedFraction);
    }
}





BOOL GFXNX64NVN::CubeMapCreate(HTexture * phTexture, const TextureFormat eTexFormat, const uint32 dwEdgeWidth, const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32, const int32, const TextureInitialData *, const char * szTexDebugName)
{










    *phTexture = NULL;
    NVNTexInfo * pTexInfo = new NVNTexInfo;

    pTexInfo->eTexFormat = eTexFormat;
    pTexInfo->dwMSAALevel = 1; pTexInfo->dwLastFrameUsed = 0;
    pTexInfo->eType = NVNTEXTYPE_CUBEMAP; pTexInfo->dwPixelWidth = dwEdgeWidth;

    pTexInfo->dwPixelHeight = dwEdgeWidth; pTexInfo->dwPixelDepth = 1;

    pTexInfo->dwMipLevels = dwMipMapLevels;

    BOOL bIsLinear = TextureFormatIsLinear(eTexFormat);
    BOOL bIsRenderTarget = (dwTexCreateFlags & (1 << TEXCREATE_DEPTH_TARGET_BIT)) ? TRUE : (dwTexCreateFlags & (1 << TEXCREATE_RENDER_TARGET_BIT));


    pTexInfo->CreateNVNTexture(bIsLinear, bIsRenderTarget, FALSE, FALSE);
    NVNMarkResourceReadyForDelete(pTexInfo);
    *phTexture = pTexInfo;







    return TRUE;
}














BOOL GFXNX64NVN::CubeMapLock(HTexture hTexture, CubeMapFace eCubeMapFace, uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwPitch, const uint32 dwGfxFlags)
{
    NVNTexInfo * pTexInfo = (NVNTexInfo *)hTexture;




    nvn::TextureView texView;
    texView.SetDefaults();
    texView.SetTarget(nvn::TextureTarget::TARGET_2D);
    texView.SetLayers(eCubeMapFace, 1);
    ptrdiff_t faceOffset = pTexInfo->ptTexture->GetViewOffset(&texView);




    lpTexture = (uint8 *)pTexInfo->tMemory.ptMemory + faceOffset;
    dwPitch = (uint32)pTexInfo->ptTexture->GetStride();

    return TRUE;
}




void GFXNX64NVN::CubeMapUnlock(HTexture hTexture, CubeMapFace eCubeMapFace, uint32 dwMipMapLevel)
{
    NVNTexInfo * ptTexInfo = (NVNTexInfo *)hTexture;
    sFlushNVNTexture(ptTexInfo);
}








void GFXNX64NVN::CubeMapSetRenderTarget(const uint32 dwRTIndex, HTexture hTexture, const CubeMapFace eCubeMapFace)
{




    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}




BOOL GFXNX64NVN::VolumeTexCreate(HTexture * phTexture, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwDepth, const uint32 dwMipMapLevels, const ResourceUsage, const uint32 dwTexCreateFlags, const TextureInitialData *, const char * szTexDebugName)
{










    *phTexture = NULL;
    NVNTexInfo * pTexInfo = new NVNTexInfo;

    pTexInfo->eTexFormat = eTexFormat;
    pTexInfo->dwMSAALevel = 1; pTexInfo->dwLastFrameUsed = 0; pTexInfo->eType = NVNTEXTYPE_3D;
    pTexInfo->dwPixelWidth = dwWidth;

    pTexInfo->dwPixelHeight = dwHeight; pTexInfo->dwPixelDepth = dwDepth;

    pTexInfo->dwMipLevels = dwMipMapLevels;

    BOOL bIsLinear = TextureFormatIsLinear(eTexFormat);
    BOOL bIsRenderTarget = (dwTexCreateFlags & (1 << TEXCREATE_DEPTH_TARGET_BIT)) ? TRUE : (dwTexCreateFlags & (1 << TEXCREATE_RENDER_TARGET_BIT));


    pTexInfo->CreateNVNTexture(bIsLinear, bIsRenderTarget, FALSE, FALSE);
    NVNMarkResourceReadyForDelete(pTexInfo);
    *phTexture = pTexInfo;







    return TRUE;
}


























void GFXNX64NVN::LoadAllMipLevels(SNOElement * ptSNOElement, void *, uint32, HTexture hTexture, const SerializeData & serTexSrc)
{






    BOOL bTextureWasLoaded = FALSE;

    NVNTexInfo * pInfo = (NVNTexInfo *)hTexture;

    uint32 dwSize = serTexSrc.dwSize;
    uint32 dwOffset = serTexSrc.dwOffset;

    void * lpTempBuffer = SigmaMemoryNew(dwSize, 4, NULL, FALSE);
    SNOFileRead(ptSNOElement, dwOffset, dwSize, lpTempBuffer);

    nvnTool::texpkg::NVNHWTextureHeader * ptTexHeader = NULL;
    uint32 dwTextureBlockCount = 0;
    uint8 * pBufferWalker = (uint8 *)lpTempBuffer;

    nvnTool::texpkg::ArchiveHeader * ptArchiveHeader = (nvnTool::texpkg::ArchiveHeader *)pBufferWalker;


    pBufferWalker += sizeof(nvnTool::texpkg::ArchiveHeader);

    while (!bTextureWasLoaded)
    {

        nvnTool::texpkg::BlockHeader * ptBlockHeader = (nvnTool::texpkg::BlockHeader *)pBufferWalker;

        pBufferWalker += ptBlockHeader->offset;

        switch (ptBlockHeader->blockType)
        {
        case 2:
            ptTexHeader = (nvnTool::texpkg::NVNHWTextureHeader *)pBufferWalker;
            pBufferWalker += sizeof(nvnTool::texpkg::NVNHWTextureHeader);
            break;












        case 3:

            SigmaMemoryCopy(pInfo->tMemory.ptMemory, pBufferWalker, ptTexHeader->dataSize);

            pBufferWalker += ptTexHeader->dataSize;
            break;




        case 5:
            bTextureWasLoaded = TRUE;
            break;


        default:

            DisplayInternalError(NULL, "", 2540, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            break;
        }
    }

    SigmaMemoryFree(lpTempBuffer, NULL);
    sFlushNVNTexture(pInfo);
}















BOOL GFXNX64NVN::VolumeTexLock(HTexture hTexture, const uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwRowPitch, uint32 & dwSlicePitch, const uint32 dwGfxFlags)
{






    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    return FALSE;
}




void GFXNX64NVN::VolumeTexUnlock(HTexture hTexture, const uint32 dwMipMapLevel)
{


    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}




void GFXNX64NVN::DeviceGetState(BOOL & fCanRender, BOOL & fMustBeReset, BOOL & fCanBeReset)
{
    fCanRender = TRUE;
    fMustBeReset = FALSE;
    fCanBeReset = TRUE;
}













































inline void GFXNX64NVN::sCreateOcclusionQueries()
{
    globals.pOcclusionQueryAllocation = (OcclusionQuery *)globals.tGPUEventsMemory.GetOffsetAddress(globals.tGPUEventsMemory.Allocate(sizeof(OcclusionQuery) * 256, 16));


    for (uint32 i = 0; i < 256; i++)
    {
        globals.tFreeOcclusionQueries.Add_Tracked(&globals.pOcclusionQueryAllocation[255 - i]);
    }
}

inline void GFXNX64NVN::sDestroyOcclusionQueries()
{
    globals.tFreeOcclusionQueries.EmptyArray();
    globals.pOcclusionQueryAllocation = NULL;
}
OcclusionQuery * GFXNX64NVN::OcclusionQueryCreate()
{

    if (globals.tFreeOcclusionQueries.GetArraySize() != 0)
    {



        int32 nIndex = globals.tFreeOcclusionQueries.GetArraySize() - 1;

        OcclusionQuery * ptQuery = globals.tFreeOcclusionQueries[nIndex];
        globals.tFreeOcclusionQueries.RemoveAt(nIndex);


        ptQuery->counter = 0; ptQuery->timestamp = 0;
        return ptQuery;
    }
    return NULL;
}



void GFXNX64NVN::OcclusionQueryBegin(OcclusionQuery * ptQuery)
{
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];
    ptQuery->counter = 0;
    ptQuery->timestamp = 0;


    renderWorkerGlobals.tCommandBuffer.ResetCounter(nvn::CounterType::SAMPLES_PASSED);
}




void GFXNX64NVN::OcclusionQueryEnd(OcclusionQuery * ptQuery)
{

    nvn::BufferAddress gpuAddress = globals.tGPUEventsMemory.GetGpuAddressFromCpuAddress(ptQuery);
    RenderWorkerGlobals & renderWorkerGlobals = workerGlobals[0];

    renderWorkerGlobals.tCommandBuffer.ReportCounter(nvn::CounterType::SAMPLES_PASSED, gpuAddress);
}




BOOL GFXNX64NVN::OcclusionQueryGetResult(OcclusionQuery * ptQuery, uint32 & nResult)
{
    if (ptQuery->timestamp != 0 || ptQuery->counter != 0)
    {
        nResult = (uint32)ptQuery->counter;
        return TRUE;
    }
    nResult = 0;
    return FALSE;
}









void GFXNX64NVN::OcclusionQueryDispose(OcclusionQuery * ptQuery)
{

    globals.tFreeOcclusionQueries.Add_Tracked(ptQuery);
}




BOOL GFXNX64NVN::OcclusionQueryCompleted(OcclusionQuery * ptQuery)
{
    return ptQuery->timestamp != 0 || ptQuery->counter != 0;
}





uint8 * GFXNX64NVN::sDynamicBufferAlloc(uint32 dwSize, uint32 dwAlign)
{


    Blizzard::Lock::ScopedRMutex tLock(globals.tWorkerLock);
    return (uint8 *)globals.ptDynamicBuffer->GetOffsetAddress(globals.ptDynamicBuffer->Allocate(dwSize, dwAlign));
}




nvn::BufferAddress GFXNX64NVN::sGetDynamicBufferAddress(uint8 * pDynamicBuffer)
{
    ptrdiff_t tOffset = globals.ptDynamicBuffer->GetAddressOffset(pDynamicBuffer);
    return globals.ptDynamicBuffer->tMemContainer.tNVNBufferAddress + tOffset;
}








void * NVNMemoryAllocator::Allocate(size_t nSize, size_t nAlign, NVNAllocType eType)
{
    void * pMemory = NULL;
    switch (eType)
    {
    case NVNALLOCTYPE_PERSISTENT:
        pMemory = gptGraphicsPersistentMemoryHeap->MemAlloc(nSize, nAlign, FALSE);
        break;
    case NVNALLOCTYPE_TRANSIENT:

        pMemory = DefinitionMemAllocTracked(GLOBALCACHE_SECONDARY, (uint32)nSize, (uint32)nAlign, FALSE, "D:/BuildServer/2/work/code/branches/2_6_2/X/Client/Gfx/NX64/NX64NVNCore.cpp", __LINE__);
        break;
    }
    return pMemory;
}










void NVNMemoryAllocator::Free(void * pMem, NVNAllocType eType)
{
    switch (eType)
    {
    case NVNALLOCTYPE_PERSISTENT:
        gptGraphicsPersistentMemoryHeap->MemFree(pMem);
        break;
    case NVNALLOCTYPE_TRANSIENT:

        DefinitionMemFree(GLOBALCACHE_SECONDARY, pMem);
        break;
    }
}








NVNGraphicsMemory::NVNGraphicsMemory()
    : ptMemory(NULL), nAllocSize(0), nAllocOffset(0), ptMemoryPool(NULL), tNVNBufferAddress(0), eMemType((MemType)-1), bOwnsMemory(FALSE), bOwnsMemoryPool(FALSE)
{
}











NVNGraphicsMemory::~NVNGraphicsMemory()
{
    if (ptMemory)
    {
        Dispose();
    }
}




void NVNGraphicsMemory::Init(size_t nSize, size_t nAlign, NVNGraphicsMemory::MemType eType)
{
    Allocate(nSize, nAlign, eType);
    CreateGPUMapping();
}




void NVNGraphicsMemory::Init(void * ptProvidedMemory, size_t nSize, NVNGraphicsMemory::MemType eType)
{
    ptMemory = ptProvidedMemory;
    nAllocSize = nSize;
    eMemType = eType;
    CreateGPUMapping();
}






inline void NVNGraphicsMemory::Allocate(size_t nSize, size_t nAlign, NVNGraphicsMemory::MemType eType)
{

    NVNAllocType eAllocType = GetAllocationType(eType);
    if (eAllocType == NVNALLOCTYPE_PERSISTENT)
    {
        nSize = nn::util::align_up(nSize, 4096);
        nAlign = nn::util::align_up(nAlign, 4096);
    }
    ptMemory = NVNMemoryAllocator::Allocate(nSize, nAlign, eAllocType);
    nAllocSize = nSize;
    eMemType = eType;
    bOwnsMemory = TRUE;
}




void NVNGraphicsMemory::CreateGPUMapping()
{
    NVNAllocType eAllocType = GetAllocationType(eMemType);
    if (eAllocType == NVNALLOCTYPE_TRANSIENT)
    {
        ptMemoryPool = gptGPUHeap->GetMemoryPool(ptMemory, &nAllocOffset);
        tNVNBufferAddress = nAllocOffset + ptMemoryPool->GetBufferAddress();
    }
    else
    {
        ptMemoryPool = new nvn::MemoryPool;
        nvn::MemoryPoolBuilder poolBuilder;

        poolBuilder.SetDevice(&GFXNX64NVN::globals.tDevice);
        poolBuilder.SetDefaults();
        poolBuilder.SetFlags(GetPoolFlags(eMemType));
        poolBuilder.SetStorage(ptMemory, nAllocSize);
        ptMemoryPool->Initialize(&poolBuilder);
        tNVNBufferAddress = ptMemoryPool->GetBufferAddress();
        nAllocOffset = 0;
        bOwnsMemoryPool = TRUE;
    }
}



inline void NVNGraphicsMemory::Dispose()
{


    if (bOwnsMemoryPool)
    {
        ptMemoryPool->Finalize();
        delete ptMemoryPool;
        ptMemoryPool = NULL;
    }
    tNVNBufferAddress = 0;

    if (bOwnsMemory)
    {
        NVNMemoryAllocator::Free(ptMemory, GetAllocationType(eMemType));
    }
    ptMemory = NULL;
}






inline NVNGraphicsMemory::MemType NVNGraphicsMemory::GetMemType() const
{
    return eMemType;
}




inline nvn::MemoryPoolFlags NVNGraphicsMemory::GetPoolFlags(NVNGraphicsMemory::MemType eType)
{
    switch (eType)
    {
    case MEMTYPE_RENDER_TARGET:
        return nvn::MemoryPoolFlags::Enum(nvn::MemoryPoolFlags::COMPRESSIBLE | nvn::MemoryPoolFlags::GPU_CACHED | nvn::MemoryPoolFlags::CPU_NO_ACCESS);
    case MEMTYPE_COMMAND_BUFFER:
        return nvn::MemoryPoolFlags::Enum(nvn::MemoryPoolFlags::GPU_CACHED | nvn::MemoryPoolFlags::CPU_UNCACHED);
    case MEMTYPE_GPU_EVENTS:
        return nvn::MemoryPoolFlags::Enum(nvn::MemoryPoolFlags::GPU_UNCACHED | nvn::MemoryPoolFlags::CPU_UNCACHED);
    case MEMTYPE_DYNAMIC_BUFFER:
        return nvn::MemoryPoolFlags::Enum(nvn::MemoryPoolFlags::GPU_CACHED | nvn::MemoryPoolFlags::CPU_UNCACHED);
    case MEMTYPE_DESCRIPTOR_POOL:
        return nvn::MemoryPoolFlags::Enum(nvn::MemoryPoolFlags::GPU_CACHED | nvn::MemoryPoolFlags::CPU_UNCACHED);
    default:
        return nvn::MemoryPoolFlags::Enum(0);
    }
}




















inline NVNAllocType NVNGraphicsMemory::GetAllocationType(NVNGraphicsMemory::MemType eType)
{
    if (eType == MEMTYPE_STATIC_BUFFER || eType == MEMTYPE_TEXTURE_RESOURCE)
    {
        return NVNALLOCTYPE_TRANSIENT;
    }
    return NVNALLOCTYPE_PERSISTENT;
}




















inline void NVNGraphicsLinearAllocator::Init(size_t nSize, size_t nAlign, NVNGraphicsMemory::MemType eType)
{
    tMemContainer.Init(nSize, nAlign, eType);
}




inline void NVNGraphicsLinearAllocator::Dispose()
{

    tMemContainer.Dispose();
}




inline ptrdiff_t NVNGraphicsLinearAllocator::Allocate(size_t nSize, size_t nAlign)
{


    ptrdiff_t returnVal = nn::util::align_up(nCurOffset, nAlign);





    nCurOffset = returnVal + nSize;


    if (nCurOffset >= tMemContainer.nAllocSize)
    {
        returnVal = -1;
    }
    return returnVal;
}






inline void * NVNGraphicsLinearAllocator::GetOffsetAddress(ptrdiff_t offset)
{

    if (offset < 0)
    {
        return NULL;
    }

    return (uint8 *)tMemContainer.ptMemory + offset;
}




ptrdiff_t NVNGraphicsLinearAllocator::GetAddressOffset(void * pAddr)
{
    ptrdiff_t offset = (uint8 *)pAddr - (uint8 *)tMemContainer.ptMemory;
    return offset;
}





inline nvn::BufferAddress NVNGraphicsLinearAllocator::GetGpuAddressFromCpuAddress(void * pCpuAddress)
{
    return tMemContainer.tNVNBufferAddress + GetAddressOffset(pCpuAddress);
}




inline void NVNGraphicsLinearAllocator::ClearAllocations()
{

    nCurOffset = 0;
}




nvn::Texture * NVNTexInfo::GetTexture(NVNTexInfo::TexSelection eSelection)
{
    if (eSelection == TEXSELECTION_REDUCED && HasReducedRenderTarget())
        return ptReducedTexture;
    return ptTexture;
}


















int NVNTexInfo::GetTextureID(NVNTexInfo::TexSelection eSelection) const
{
    if (eSelection == TEXSELECTION_REDUCED && HasReducedRenderTarget())
        return nReducedTextureID;
    return nTextureID;
}






inline BOOL NVNTexInfo::IsRenderTarget() const
{
    return tMemory.GetMemType() == NVNGraphicsMemory::MEMTYPE_RENDER_TARGET;
}

inline BOOL NVNTexInfo::HasReducedRenderTarget() const
{
    return IsRenderTarget() && !bIsWindowTexture && GetDeviceResolutionFraction() != TEXFRACTION_NOT_RELATED;
}

inline NVNTexInfo::TexDeviceResolutionFraction NVNTexInfo::GetDeviceResolutionFraction() const
{
    uint32 deviceWidth, deviceHeight;

    GFXNX64NVN::GetDeviceResolution(deviceWidth, deviceHeight);

    if (dwPixelWidth == deviceWidth && dwPixelHeight == deviceHeight)
        return TEXFRACTION_FULL;


    if (dwPixelWidth == deviceWidth / 2 && dwPixelHeight == deviceHeight / 2)
        return TEXFRACTION_HALF;


    if (dwPixelWidth == deviceWidth / 4 && dwPixelHeight == deviceHeight / 4)
        return TEXFRACTION_QUARTER;
    return TEXFRACTION_NOT_RELATED;
}









inline GFXNX64NVN::CommandBufferMemoryRing::CommandBufferMemoryRing()
    : pControlMemory(NULL)
{
    dwCurCommandBlock = 0;
    dwCurControlBlock = 0;

    ZeroMemory(CommandBlockFrameFence, sizeof(CommandBlockFrameFence));
    ZeroMemory(ControlBlockFrameFence, sizeof(ControlBlockFrameFence));
}




inline void GFXNX64NVN::CommandBufferMemoryRing::Init()
{
    tCommandMemory.Init(kNumCommandBlocks * 0x80000, 4096, NVNGraphicsMemory::MEMTYPE_COMMAND_BUFFER);  // 0x8000000 in the image; alignment assumed
    pControlMemory = NVNMemoryAllocator::Allocate(kNumControlBlocks * 0x4000, 8, NVNALLOCTYPE_PERSISTENT);
}




inline void GFXNX64NVN::CommandBufferMemoryRing::Dispose()
{
    tCommandMemory.Dispose();
    NVNMemoryAllocator::Free(pControlMemory, NVNALLOCTYPE_PERSISTENT);
}




inline void GFXNX64NVN::CommandBufferMemoryRing::EndFrame()
{

    dwCurCommandBlock = dwCurCommandBlock % kNumCommandBlocks;
    dwCurControlBlock = dwCurControlBlock % kNumControlBlocks;
}




void GFXNX64NVN::CommandBufferMemoryRing::AddCommandMemory(nvn::CommandBuffer & commandBuffer)
{
    uint32 myBlockIndex = (dwCurCommandBlock++) % kNumCommandBlocks;
    uint32 & blockFence = CommandBlockFrameFence[myBlockIndex];
    while (blockFence > globals.tProcessedFrameCount.GetValue())
    {
        static bool assertOnceSingleFrame = true;
        static bool assertOnceStandard = true;

        uint32 labelValue = globals.tProcessedFrameCount.GetValue();
        if (blockFence == labelValue && assertOnceSingleFrame)
        {
            assertOnceSingleFrame = false;
        }

        else if (assertOnceStandard)
        {
            assertOnceStandard = false;
        }
    }


    commandBuffer.AddCommandMemory(tCommandMemory.ptMemoryPool, myBlockIndex * 0x80000, 0x80000);

    blockFence = globals.dwFrameCount;
}




void GFXNX64NVN::CommandBufferMemoryRing::AddControlMemory(nvn::CommandBuffer & commandBuffer)
{
    uint32 myBlockIndex = (dwCurControlBlock++) % kNumControlBlocks;
    uint32 & blockFence = ControlBlockFrameFence[myBlockIndex];
    while (blockFence > globals.tProcessedFrameCount.GetValue())
    {
        static bool assertOnce = false;

        if (!assertOnce)
        {
            assertOnce = true;
        }
    }



    commandBuffer.AddControlMemory((uint8 *)pControlMemory + myBlockIndex * 0x4000, 0x4000);

    blockFence = globals.dwFrameCount;
}


static nvn::Format sTextureFormatToNVNFormat(const TextureFormat eTexFormat, NVNTexSwizzle * ptTexSwizzle)
{
    nvn::Format eNVNFormat = nvn::Format::NONE;

    switch (eTexFormat)
    {
    case TEXTUREFORMAT_ARGB8888:
    case TEXTUREFORMAT_RGBA8888:
    case TEXTUREFORMAT_LIN_RGBA8888:
    case TEXTUREFORMAT_ABGR8888:
        eNVNFormat = nvn::Format::RGBA8;
        break;
    case TEXTUREFORMAT_LIN_ARGB8888:
        eNVNFormat = nvn::Format::RGBA8;
        if (ptTexSwizzle)
        {
            ptTexSwizzle->R = nvn::TextureSwizzle::B;
            ptTexSwizzle->G = nvn::TextureSwizzle::G;
            ptTexSwizzle->B = nvn::TextureSwizzle::R;
            ptTexSwizzle->A = nvn::TextureSwizzle::A;
        }
        break;






    case TEXTUREFORMAT_ARGB0888:
    case TEXTUREFORMAT_LIN_ARGB0888:
        eNVNFormat = nvn::Format::BGRX8;
        break;





    case TEXTUREFORMAT_ARGB1555:
    case TEXTUREFORMAT_LIN_ARGB1555:
        eNVNFormat = nvn::Format::RGB5A1;
        break;





    case TEXTUREFORMAT_RGB565:
    case TEXTUREFORMAT_LIN_RGB565:
        eNVNFormat = nvn::Format::RGB565;
        break;
    case TEXTUREFORMAT_L8:
    case TEXTUREFORMAT_LIN_L8:
        eNVNFormat = nvn::Format::R8;
        if (ptTexSwizzle)
        {
            ptTexSwizzle->R = ptTexSwizzle->G = ptTexSwizzle->B = nvn::TextureSwizzle::R;
            ptTexSwizzle->A = nvn::TextureSwizzle::ONE;


        }
        break;
    case TEXTUREFORMAT_DXT1:
    case TEXTUREFORMAT_DXT1_1BITALPHA:
        eNVNFormat = nvn::Format::RGBA_DXT1;
        break;

    case TEXTUREFORMAT_DXT3:
        eNVNFormat = nvn::Format::RGBA_DXT3;
        break;

    case TEXTUREFORMAT_DXT5:
    case TEXTUREFORMAT_DXT5N:
        eNVNFormat = nvn::Format::RGBA_DXT5;
        break;









    case TEXTUREFORMAT_A8:
        eNVNFormat = nvn::Format::R8;
        if (ptTexSwizzle)
        {
            ptTexSwizzle->R = ptTexSwizzle->G = ptTexSwizzle->B = nvn::TextureSwizzle::ZERO;
            ptTexSwizzle->A = nvn::TextureSwizzle::R;


        }
        break;
    case TEXTUREFORMAT_R32F:
        eNVNFormat = nvn::Format::R32F;
        break;

    case TEXTUREFORMAT_G32FR32F:
        eNVNFormat = nvn::Format::RG32F;
        break;

    case TEXTUREFORMAT_A32B32G32R32F:
        eNVNFormat = nvn::Format::RGBA32F;
        break;

    case TEXTUREFORMAT_R16F:
        eNVNFormat = nvn::Format::R16F;
        break;

    case TEXTUREFORMAT_G16FR16F:
        eNVNFormat = nvn::Format::RG16F;
        break;

    case TEXTUREFORMAT_A16B16G16R16F:
        eNVNFormat = nvn::Format::RGBA16F;
        break;



    case TEXTUREFORMAT_D24X8:
        eNVNFormat = nvn::Format::DEPTH24_STENCIL8;
        break;

    case TEXTUREFORMAT_D16:
        eNVNFormat = nvn::Format::DEPTH16;
        break;

    case TEXTUREFORMAT_ARGB4444:
    case TEXTUREFORMAT_ARGB0555:
    case TEXTUREFORMAT_P8:
    case TEXTUREFORMAT_LIN_ARGB4444:
    case TEXTUREFORMAT_LIN_ARGB0555:
    case TEXTUREFORMAT_RGB888:
    case TEXTUREFORMAT_L16:
    case TEXTUREFORMAT_R16G16B16:
    case TEXTUREFORMAT_A16R16G16B16:
    case TEXTUREFORMAT_DF24:
    case TEXTUREFORMAT_DF16:
    case TEXTUREFORMAT_D32:
    case TEXTUREFORMAT_RAWZ:
    case TEXTUREFORMAT_INTZ:
    case TEXTUREFORMAT_DEPTHOPAQUE:
        break;




    case TEXTUREFORMAT_A16B16G16R16:
        eNVNFormat = nvn::Format::RGBA16;
        break;

    case TEXTUREFORMAT_A2B10G10R10:
        eNVNFormat = nvn::Format::RGB10A2;
        break;

    default:
        DisplayInternalError(NULL, "", 3404, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return eNVNFormat;
}

inline void GFXNX64NVN::RegisterTexture(NVNTexInfo & tTexInfo)
{
    globals.tIDManager.RegisterTexture(tTexInfo);
}

inline void GFXNX64NVN::IDManager::Init()
{

    tTextureIdFreeListLock = new blz::mutex();
    tTextureIdFreeList = new blz::vector<int>();
    {   // Needed = 10000 is the DWARF const value; the image passes 10256 descriptors. Per-descriptor size 32 and alignment 4096 are not in the image (Allocate rounds both up to 4096): 32 is the only integer size that rounds 10256 descriptors to the 0x51000 allocated.
        const uint32 dwNumTextureDescriptorsNeeded = 10000;
        const uint32 dwNumTextureDescriptorsToAllocate = dwNumTextureDescriptorsNeeded + 256;

        tTextureDescriptorMemory.Init(dwNumTextureDescriptorsToAllocate * 32, 4096, NVNGraphicsMemory::MEMTYPE_DESCRIPTOR_POOL);

        tTexturePool.Initialize(tTextureDescriptorMemory.ptMemoryPool, 0, dwNumTextureDescriptorsToAllocate);




        dwCurrentTextureID = 256;
    }

    {   // Needed = 1000 is the DWARF const value; 1256 passed. Size 32 assumed as above (30..32 all round to the 0xa000 allocated).
        const uint32 dwNumSamplerDescriptorsNeeded = 1000;
        const uint32 dwNumSamplerDescriptorsToAllocate = dwNumSamplerDescriptorsNeeded + 256;

        tSamplerDescriptorMemory.Init(dwNumSamplerDescriptorsToAllocate * 32, 4096, NVNGraphicsMemory::MEMTYPE_DESCRIPTOR_POOL);

        tSamplerPool.Initialize(tSamplerDescriptorMemory.ptMemoryPool, 0, dwNumSamplerDescriptorsToAllocate);




        dwCurrentSamplerID = 256;
    }
}
inline void GFXNX64NVN::IDManager::Dispose()
{
    tTexturePool.Finalize();
    tSamplerPool.Finalize();
    delete tTextureIdFreeList;
    delete tTextureIdFreeListLock;
}

void GFXNX64NVN::IDManager::RegisterTexture(NVNTexInfo & tTexInfo)
{
    tTexInfo.nTextureID = 0;
    tTexInfo.nReducedTextureID = 0;

    tTextureIdFreeListLock->lock();

    if (!tTextureIdFreeList->empty())
    {
        tTexInfo.nTextureID = tTextureIdFreeList->back();
        tTextureIdFreeList->pop_back();

        if (tTexInfo.HasReducedRenderTarget() && !tTextureIdFreeList->empty())
        {
            tTexInfo.nReducedTextureID = tTextureIdFreeList->back();
            tTextureIdFreeList->pop_back();
        }
    }
    tTextureIdFreeListLock->unlock();

    if (tTexInfo.nTextureID == 0)
    {
        tTexInfo.nTextureID = dwCurrentTextureID++;
    }

    if (tTexInfo.HasReducedRenderTarget() && tTexInfo.nReducedTextureID == 0)
    {
        tTexInfo.nReducedTextureID = dwCurrentTextureID++;
    }

    tTexturePool.RegisterTexture(tTexInfo.nTextureID, tTexInfo.ptTexture, NULL);

    if (tTexInfo.HasReducedRenderTarget())
    {
        tTexturePool.RegisterTexture(tTexInfo.nReducedTextureID, tTexInfo.GetTexture(NVNTexInfo::TEXSELECTION_REDUCED), NULL);
    }
}




void GFXNX64NVN::IDManager::UnregisterTexture(NVNTexInfo & tTexInfo)
{
    tTextureIdFreeListLock->lock();
    if (tTexInfo.HasReducedRenderTarget())
    {
        tTextureIdFreeList->push_back(tTexInfo.nReducedTextureID);
    }
    tTextureIdFreeList->push_back(tTexInfo.nTextureID);
    tTextureIdFreeListLock->unlock();
}

void GFXNX64NVN::IDManager::RegisterSampler(NVNSampler & tSampler)
{
    tSampler.nSamplerID = dwCurrentSamplerID++;

    tSamplerPool.RegisterSampler(tSampler.nSamplerID, &tSampler.tSampler);
}

NVNTexInfo::TexSelection GFXNX64NVN::sSelectRenderTargetAlias()
{
    return globals.bIsDocked ? NVNTexInfo::TEXSELECTION_FULL : NVNTexInfo::TEXSELECTION_REDUCED;
}
