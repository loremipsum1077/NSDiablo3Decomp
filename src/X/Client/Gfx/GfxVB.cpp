#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx.h"
#include "GfxAPI.h"
#include "GfxCommon.h"
#include "GfxInternal.h"
#include "GfxVB.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"

// X/Common/Game.cpp:1938
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();
// X/Client/CGame.h
typedef void (*AsyncRenderCallback)(void *, void *);
void CGameAsyncRenderRegisterGPUResourceDelayedDeletion(AsyncRenderCallback callback, void * pParam1, void * pParam2);
#include "../Definitions/AppearanceDefinition.h"

extern AppearanceGroupDef * gptGroupDefAppearance;
// X/Client/Gfx/Gfx.cpp:81
extern VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];

struct VertexBuffer {  // size 0x10
    VBID id;  // 0x00
    uint32 dwMaxVertexCount : 16;  // 0x04
    VBFormat eVBFormat : 5;  // 0x04
    ResourceUsage eUsage : 3;  // 0x04
    int32 nRefCount : 8;  // 0x04
    HVB hvb;  // 0x08
};
static_assert(sizeof(VertexBuffer) == 0x10, "VertexBuffer");

VertexBuffer * VBGet(const VBID id);
VertexBuffer * VBTryToGet(const VBID id);
BOOL GfxVBIsValid(const VBID vbid);
void GfxVBFree(const VBID vbid);
void GfxVBFreeAll(const BOOL fHardReset);
static void sVBDispose(VertexBuffer * pvb);
static uint32 sVBGetSize(const VertexBuffer * pvb);

// line 71
void GfxVBInitialize() {
    gptGfxData->ptVBList = TChainedDataArrayCreateTracked<VertexBuffer, int>("VBs", AppGetCurrentMemoryInterface(), 8192);
    gptGfxData->ptVBList->PreAllocateDataBlockSlots();

    gptGfxData->tVBListMutex.Create();

    gptGfxData->tDynVBList.Initialize(NULL);
}

// line 98
void GfxVBDispose() {
    GfxVBFreeAll(TRUE);

    if (gptGfxData->ptVBList) {
        TChainedDataArrayDispose(gptGfxData->ptVBList);
        gptGfxData->ptVBList = NULL;
    }

    gptGfxData->tDynVBList.Dispose();

    gptGfxData->tVBListMutex.Destroy();
}

// line 121
BOOL GfxVBAllocTracked(const ResourceUsage eUsage, const VBFormat eVBFormat, const uint32 dwCount,
                       const BOOL fPurgeIfNecessary, const void * lpInitialData, VBID & vbid, LPCSTR szFile,
                       int32 nLine, LPCSTR szDebugString) {
    HVB hvb;
    uint32 hResultRet = 0;
    uint32 dwPurgeCount;
    BOOL fSuccess;
    VertexBuffer * pvb;

    fSuccess = gptGFXAPI->VBCreate(eVBFormat, &hvb, dwCount, eUsage, lpInitialData, szDebugString, hResultRet);

    while (!fSuccess && fPurgeIfNecessary) {
        if (!gptGroupDefAppearance->CacheManagerPurge(FALSE)) {
            DisplayInternalError(NULL, "", 162, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        fSuccess = gptGFXAPI->VBCreate(eVBFormat, &hvb, dwCount, eUsage, lpInitialData, szDebugString, hResultRet);
    }

    if (!fSuccess) {
        return FALSE;
    }

    {
        Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tVBListMutex);
        vbid = gptGfxData->ptVBList->ElementNew();
        if (eUsage == RESOURCEUSAGE_DYNAMIC) {
            gptGfxData->tDynVBList.AddTail_Tracked(vbid);
        }
    }

    pvb = VBGet(vbid);
    pvb->dwMaxVertexCount = dwCount;
    pvb->eVBFormat = eVBFormat;
    pvb->hvb = hvb;
    pvb->eUsage = eUsage;
    pvb->nRefCount = 1;

    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];
    workerData.tStats.dwVBMemUsage += sVBGetSize(pvb);

    return TRUE;
}

// line 219
BOOL GfxVBAllocProxy(const VBFormat eVBFormat, const uint32 dwCount, VBID & vbid, LPCSTR szDebugString) {
    HVB hvb;
    BOOL fSuccess = gptGFXAPI->VBCreateProxy(eVBFormat, &hvb, dwCount, szDebugString);
    if (!fSuccess) {
        return FALSE;
    }

    {
        Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tVBListMutex);
        vbid = gptGfxData->ptVBList->ElementNew();
    }

    VertexBuffer * pvb = VBGet(vbid);
    pvb->dwMaxVertexCount = dwCount;
    pvb->eVBFormat = eVBFormat;
    pvb->hvb = hvb;
    pvb->eUsage = RESOURCEUSAGE_PROXY;
    pvb->nRefCount = 1;

    return TRUE;
}

// line 253
VBID GfxVBAddRef(const VBID vbid) {
    VertexBuffer * pvb = VBGet(vbid);
    pvb->nRefCount++;
    return vbid;
}

// line 267
static void sVBReallyDispose(void * hvb, void *) {
    gptGFXAPI->VBDispose(hvb);
}

// line 277
static void sVBDispose(VertexBuffer * pvb) {
    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];
    workerData.tStats.dwVBMemUsage -= sVBGetSize(pvb);

    if (gptGFXAPI->VBIsBusy(pvb->hvb)) {
        CGameAsyncRenderRegisterGPUResourceDelayedDeletion(sVBReallyDispose, pvb->hvb, NULL);
    } else {
        gptGFXAPI->VBDispose(pvb->hvb);
    }

    if (pvb->eUsage == RESOURCEUSAGE_DYNAMIC) {
        gptGfxData->tDynVBList.RemoveAt(gptGfxData->tDynVBList.Find(pvb->id));
    }

    gptGfxData->ptVBList->ElementDelete(pvb, FALSE);
}

// line 311
void GfxVBRelease(const VBID vbid) {
    if (!GfxVBIsValid(vbid)) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tVBListMutex);

    VertexBuffer * pvb = VBGet(vbid);
    if (pvb->nRefCount > 0) {
        pvb->nRefCount--;
        if (pvb->nRefCount == 0) {
            sVBDispose(pvb);
        }
    }
}

// line 339
void GfxVBFree(const VBID vbid) {
    if (gptGfxData == NULL || gptGfxData->ptVBList == NULL) {
        return;
    }
    if (!GfxVBIsValid(vbid)) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tVBListMutex);

    VertexBuffer * pvb = VBGet(vbid);
    sVBDispose(pvb);
}

// line 363
void GfxVBFreeAll(const BOOL fHardReset) {
    if (gptGfxData == NULL || gptGfxData->ptVBList == NULL) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tVBListMutex);

    if (fHardReset) {
        TDataArrayIterator<VertexBuffer> tIt;
        while (VertexBuffer * pvb = gptGfxData->ptVBList->Iterate(tIt)) {
            sVBDispose(pvb);
        }
    } else {
        POS pos = gptGfxData->tDynVBList.GetHeadPosition();
        while (pos) {
            VBID vbid = gptGfxData->tDynVBList.GetNext(pos);
            VertexBuffer * pvb = gptGfxData->ptVBList->ElementGet(vbid);
            sVBDispose(pvb);
        }
    }
}

// line 402
void GfxVBFlushAllDynamic() {
    if (gptGfxData == NULL || gptGfxData->ptVBList == NULL) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tVBListMutex);

    POS pos = gptGfxData->tDynVBList.GetHeadPosition();
    while (pos) {
        VBID vbid = gptGfxData->tDynVBList.GetNext(pos);
        VertexBuffer * pvb = gptGfxData->ptVBList->ElementGet(vbid);
        gptGFXAPI->VBFlush(pvb->hvb);
    }
}

// line 428
void GfxVBRestart(VBID vbid) {
    VertexBuffer * pvb = VBGet(vbid);
    switch (pvb->eUsage) {
    case RESOURCEUSAGE_DYNAMIC:
        gptGFXAPI->VBFlush(pvb->hvb);
        break;
    case RESOURCEUSAGE_STATIC:
        gptGFXAPI->VBFlush(pvb->hvb);
        break;
    }
}

// line 454
void * GfxVBLock(const VBID vbid, const uint32 dwCount, uint32 & dwStartIndex) {
    uint32 dwStartOffset;
    uint32 dwVertSize;
    void * lpVertices;
    VertexBuffer * pvb = VBGet(vbid);

    dwVertSize = cgtVBFormatInfo[pvb->eVBFormat].dwSize;
    dwStartOffset = 0;
    lpVertices = gptGFXAPI->VBLock(pvb->hvb, dwVertSize * dwCount, dwStartOffset);
    dwStartIndex = dwStartOffset / dwVertSize;
    return lpVertices;
}

// line 482
void * GfxVBLockAll(const VBID vbid) {
    VertexBuffer * pvb = VBGet(vbid);
    return gptGFXAPI->VBLockAll(pvb->hvb);
}

// line 504
void GfxVBUnlock(const VBID vbid) {
    VertexBuffer * pvb = VBGet(vbid);
    gptGFXAPI->VBUnlock(pvb->hvb);
}

// line 565
void GfxWriteToVB(const VBID vbid, const uint32 dwVertCount, uint32 & dwStartIndex, const void * pSrc,
                  const uint32 dwBytesToWrite) {
    void * pDest = GfxVBLock(vbid, dwVertCount, dwStartIndex);
    if (pDest) {
        SigmaMemoryCopy(pDest, pSrc, dwBytesToWrite);
        GfxVBUnlock(vbid);
    }
}

// line 635
ResourceUsage GfxVBGetUsage(const VBID vbid) {
    return VBGet(vbid)->eUsage;
}

// line 647
BOOL GfxVBIsValid(const VBID vbid) {
    return VBTryToGet(vbid) != NULL;
}

// line 675
VBFormat GfxVBGetFormat(const VBID vbid) {
    return VBGet(vbid)->eVBFormat;
}

// line 697
int32 GfxVBGetMaxCount(const VBID vbid) {
    return VBGet(vbid)->dwMaxVertexCount;
}

// line 708
static uint32 sVBGetSize(const VertexBuffer * pvb) {
    return cgtVBFormatInfo[pvb->eVBFormat].dwSize * pvb->dwMaxVertexCount;
}

// line 717
void GfxVBSetStream(const int32 nStreamIndex, const uint32 dwStreamOffset, const VBID vbid) {
    if (vbid == VBID_INVALID) {
        gptGFXAPI->VBSetStream(nStreamIndex, NULL, 0, VBFORMAT_UNKNOWN);
        return;
    }
    VertexBuffer * pvb = VBGet(vbid);
    gptGFXAPI->VBSetStream(nStreamIndex, pvb->hvb, dwStreamOffset, pvb->eVBFormat);
}

// line 750
inline VertexBuffer * VBGet(const VBID id) {  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    return gptGfxData->ptVBList->ElementGet(id);
}

// line 759
inline VertexBuffer * VBTryToGet(const VBID id) {  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    return gptGfxData->ptVBList->ElementTryToGet(id);
}

// line 768
void GfxVBFormatCopy(void * ptDest, const FatVertex * ptSrc, const int32 nVertCount, const VBFormat eVBFormat) {
    switch (eVBFormat) {
    case VBFORMAT_2TEX_L: {
        Vertex2TexL * pvDest = (Vertex2TexL *)ptDest;
        for (int32 i = 0; i < nVertCount; i++) {
            const FatVertex & vSrc = ptSrc[i];
            Vertex2TexLSet(pvDest[i], vSrc.wpPos, vSrc.wvpNormal, VertexColor(vSrc.rgbaColor[0]),
                           VertexColor(vSrc.rgbaColor[1]), vSrc.vpUVSet[0], vSrc.vpUVSet[1]);
        }
        break;
    }
    case VBFORMAT_1TEX_L: {
        Vertex1TexL * pvDest = (Vertex1TexL *)ptDest;
        for (int32 i = 0; i < nVertCount; i++) {
            const FatVertex & vSrc = ptSrc[i];
            Vertex1TexLSet(pvDest[i], vSrc.wpPos, vSrc.wvpNormal, VertexColor(vSrc.rgbaColor[0]),
                           VertexColor(vSrc.rgbaColor[1]), vSrc.vpUVSet[0]);
        }
        break;
    }
    case VBFORMAT_BUMPBASIS:
        break;
    case VBFORMAT_CLOTH: {
        VertexCloth * pvDest = (VertexCloth *)ptDest;
        for (int32 i = 0; i < nVertCount; i++) {
            const FatVertex & vSrc = ptSrc[i];
            pvDest[i].wpModelSpacePos = vSrc.wpPos;
        }
        break;
    }
    default:
        DisplayInternalError(NULL, "", 845, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

// line 854
void GfxVBFormatCopyIntoVB(const VBID vbid, const FatVertex * ptSrc, const uint32 dwCount, uint32 & dwStartIndex) {
    void * pv = GfxVBLock(vbid, dwCount, dwStartIndex);
    if (pv) {
        GfxVBFormatCopy(pv, ptSrc, dwCount, GfxVBGetFormat(vbid));
        GfxVBUnlock(vbid);
    }
}

// line 928
void GfxVBSetProxy(const VBID vbid, const void * ptSrc) {
    VertexBuffer * pvb = VBGet(vbid);
    gptGFXAPI->VBSetProxy(pvb->hvb, ptSrc);
}

// line 1000
void PackedVBListAllocProxy(PackedVBList * ptPVBList, const VBFormat eVBFormat, LPCSTR szDebugName) {
    if (ptPVBList == NULL) {
        return;
    }

    POS pos = ptPVBList->GetHeadPosition();
    while (pos) {
        PackedVB & tPVB = ptPVBList->GetNext(pos);
        if (!GfxVBIsValid(tPVB.vbid)) {
            GfxVBAllocProxy(eVBFormat, tPVB.nVertCount, tPVB.vbid, szDebugName);
        } else {
            GfxVBAddRef(tPVB.vbid);
        }
    }
}

// line 1047
void PackedVBListFree(PackedVBList * ptPVBList) {
    if (ptPVBList == NULL) {
        return;
    }

    POS pos = ptPVBList->GetHeadPosition();
    while (pos) {
        PackedVB & tPVB = ptPVBList->GetNext(pos);
        GfxVBFree(tPVB.vbid);
    }
}

// line 1155
UByte4 Vector3DNormalToUByte4(const Vector3D & wvNormal) {
    UByte4 packed;
    float x = Math::Pin((wvNormal.x * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    float y = Math::Pin((wvNormal.y * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    float z = Math::Pin((wvNormal.z * 0.5f + 0.5f) * 255.99f, 0.0f, 255.0f);
    packed.r = (uint8)x;
    packed.g = (uint8)y;
    packed.b = (uint8)z;
    packed.a = 0;
    return packed;
}

// line 1199
Vector3D UByte4NormalToVector3D(const UByte4 packed) {
    Vector3D v;
    v.x = (float)packed.r / 255.0f * 2.0f - 1.0f;
    v.y = (float)packed.g / 255.0f * 2.0f - 1.0f;
    v.z = (float)packed.b / 255.0f * 2.0f - 1.0f;
    return v;
}

// line 1213. 8.8 fixed point through the float mantissa: biased by k, the value's low 16 mantissa
// bits are the fixed-point pattern.
UByte4 Vector2DTexCoordToUByte4(const Vector2D & uv) {
    UByte4 packed;
    const float Domain = MaxUVRange;
    const float k = 32768.0f;
    float flU = uv.x + Domain;
    float flV = uv.y + Domain;
    flU = Math::Pin(flU, 0.0f, 2.0f * Domain - 0.01f) * 2.0f + k;
    flV = Math::Pin(flV, 0.0f, 2.0f * Domain - 0.01f) * 2.0f + k;
    uint32 lo = *(uint32 *)&flU & 0xffff;
    uint32 hi = *(uint32 *)&flV << 16;
    *(uint32 *)&packed = lo | hi;
    return packed;
}

// line 1254
Vector2D UByte4TexCoordToVector2D(const UByte4 packed) {
    const float Domain = MaxUVRange;
    Vector2D v;
    v.x = ((float)packed.g + (float)packed.r / 256.0f) * (2.0f * Domain / 256.0f) - Domain;
    v.y = ((float)packed.a + (float)packed.b / 256.0f) * (2.0f * Domain / 256.0f) - Domain;
    return v;
}

// line 1314
Vector2D UByte4TexCoordToVector2DLM(const UByte4 packed) {
    const float Domain = MaxUVRangeLM;
    Vector2D v;
    v.x = ((float)packed.g + (float)packed.r / 256.0f) * (2.0f * Domain / 256.0f) - Domain;
    v.y = ((float)packed.a + (float)packed.b / 256.0f) * (2.0f * Domain / 256.0f) - Domain;
    return v;
}
