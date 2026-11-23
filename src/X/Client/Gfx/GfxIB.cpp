#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx.h"
#include "GfxAPI.h"
#include "GfxCommon.h"
#include "GfxInternal.h"
#include "GfxIB.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"

// X/Common/Game.cpp:1938
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();
// X/Client/CGame.h
typedef void (*AsyncRenderCallback)(void *, void *);
void CGameAsyncRenderRegisterGPUResourceDelayedDeletion(AsyncRenderCallback callback, void * pParam1, void * pParam2);
#include "../Definitions/AppearanceDefinition.h"

extern AppearanceGroupDef * gptGroupDefAppearance;

typedef uint16 GfxNdx;  // X/Client/Gfx/GfxAPI.h:56

struct IndexBuffer {  // size 0x10
    IBID id;  // 0x00
    uint32 dwMaxIndexCount : 21;  // 0x04
    ResourceUsage eUsage : 3;  // 0x04
    int32 nRefCount : 8;  // 0x04
    HIB hib;  // 0x08
};
static_assert(sizeof(IndexBuffer) == 0x10, "IndexBuffer");

IndexBuffer * IBGet(const IBID id);
IndexBuffer * IBTryToGet(const IBID id);
BOOL GfxIBIsValid(const IBID ibid);
void GfxIBFree(const IBID ibid);
void GfxIBFreeAll(const BOOL fHardReset);
BOOL GfxIBAllocTracked(const ResourceUsage eUsage, const uint32 dwCount, const BOOL fPurgeIfNecessary,
                       const GfxNdx * lpInitialData, IBID & ibid, LPCSTR szFile, int32 nLine, LPCSTR szDebugString);
BOOL GfxIBAllocProxy(const uint32 dwCount, IBID & ibid, LPCSTR szDebugString);
static void sIBPDispose(IndexBuffer * pib);
void sIBReallyDispose(void * hib, void *);


























void GfxIBInitialize() {
    gptGfxData->ptIBList = TChainedDataArrayCreateTracked<IndexBuffer, int>("IBs", AppGetCurrentMemoryInterface(), 4096);
    gptGfxData->ptIBList->PreAllocateDataBlockSlots();

    gptGfxData->tIBListMutex.Create();

    gptGfxData->tDynIBList.Initialize(NULL);
}



















void GfxIBDispose() {
    GfxIBFreeAll(TRUE);

    if (gptGfxData->ptIBList) {
        TChainedDataArrayDispose(gptGfxData->ptIBList);
        gptGfxData->ptIBList = NULL;
    }

    gptGfxData->tDynIBList.Dispose();

    gptGfxData->tIBListMutex.Destroy();
}











BOOL GfxIBAllocTracked(
    const ResourceUsage eUsage,
    const uint32 dwCount,
    const BOOL fPurgeIfNecessary,
    const GfxNdx * lpInitialData,
    IBID & ibid,
    LPCSTR szFile,
    int32 nLine,
    LPCSTR szDebugString)
{




    HIB hib;
    uint32 dwPurgeCount;
    uint32 hResultRet = 0;
    BOOL fSuccess = gptGFXAPI->IBCreate(&hib, dwCount * sizeof(GfxNdx), eUsage, lpInitialData, szDebugString, hResultRet);
    if (!fSuccess && fPurgeIfNecessary)
    {
        // inferred loop shape: rows only on 144 (purge), 149 (retry) and 150 (success test, branches back to 144)
        while (TRUE)
        {

            if (!gptGroupDefAppearance->CacheManagerPurge(FALSE))
            {
                break;
            }

            fSuccess = gptGFXAPI->IBCreate(&hib, dwCount * sizeof(GfxNdx), eUsage, lpInitialData, szDebugString, hResultRet);
            if (fSuccess)
            {
                break;
            }
        }


        // inferred: the fatal error on line 160 follows the loop, reached only when the purge fails
        if (!fSuccess)
        {
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }

    if (!fSuccess)
    {
        return FALSE;
    }


    {
        Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tIBListMutex);


        ibid = gptGfxData->ptIBList->ElementNew();
        if (eUsage == RESOURCEUSAGE_DYNAMIC)
        {
            gptGfxData->tDynIBList.AddTail_Tracked(ibid);
        }
    }

    IndexBuffer * pib = IBGet(ibid);















    pib->dwMaxIndexCount = dwCount;
    pib->hib = hib;
    pib->eUsage = eUsage;
    pib->nRefCount = 1;









    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];

    workerData.tStats.dwIBMemUsage += pib->dwMaxIndexCount * sizeof(GfxNdx);

    return TRUE;
}




BOOL GfxIBAllocProxy(const uint32 dwCount, IBID & ibid, LPCSTR szDebugString) {
    HIB hib;
    BOOL fSuccess = gptGFXAPI->IBCreateProxy(&hib, dwCount * sizeof(GfxNdx), szDebugString);
    if (!fSuccess) {
        return FALSE;
    }

    {
        Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tIBListMutex);
        ibid = gptGfxData->ptIBList->ElementNew();
    }

    IndexBuffer * pib = IBGet(ibid);
    pib->dwMaxIndexCount = dwCount;
    pib->hib = hib;
    pib->eUsage = RESOURCEUSAGE_PROXY;
    pib->nRefCount = 1;

    return TRUE;
}













void GfxStripIBAlloc(const ResourceUsage eUsage, const int32 nLength, const BOOL fPurgeIfNecessary, int32 & nIndexCount, IBID & ibid) {
    nIndexCount = nLength * 2;
    GfxNdx * pw = (GfxNdx *)SigmaMemoryNew(sizeof(GfxNdx) * nIndexCount, 4, NULL, FALSE);
    for (int32 x = 0; x < nIndexCount; x++) {
        pw[x] = (GfxNdx)x;
    }
    GfxIBAllocTracked(eUsage, nIndexCount, fPurgeIfNecessary, pw, ibid, __FILE__, __LINE__, "");
    SigmaMemoryFree(pw, NULL);
}
















void GfxPatchIBAlloc(
	const ResourceUsage eUsage,
	const int32 nHeight,
	const int32 nWidth,
	const int32 nLODFactor,
	const BOOL fPurgeIfNecessary,
	int32 & nIndexCount,
	IBID & ibid,
	const BOOL fReverseCulling)
{

	int32 nLODHeight = nHeight / nLODFactor;
	int32 nLODWidth = nWidth / nLODFactor;
	int32 nRowSkipWidth = nWidth * nLODFactor;
	nIndexCount = ((nLODHeight - 1) * ((nLODWidth * 2) + 2)) - 2;

	uint16 * pw = (uint16 *)SigmaMemoryNew(sizeof(GfxNdx) * nIndexCount, 4, NULL, FALSE);
	int32 nBaseIndex = 0;
	int32 nStripBaseIndex = 0;
	int32 y, x;
	for (y = 0; y < nLODHeight - 1; y++)
	{
		int32 nTempIndex = nStripBaseIndex;
		for (x = 0; x < nWidth; x += nLODFactor)
		{
			if (fReverseCulling)
			{
				pw[nTempIndex++] = (uint16)(nBaseIndex + nRowSkipWidth + x); pw[nTempIndex++] = (uint16)(nBaseIndex + x);
			}
			else
			{
				// inferred: both stores of each pair share one row (305:22, 310:22), so each pair is written on one line
				pw[nTempIndex++] = (uint16)(nBaseIndex + x); pw[nTempIndex++] = (uint16)(nBaseIndex + nRowSkipWidth + x);
			}
		}



		if (y < nLODHeight - 2)
		{
			if (fReverseCulling)
			{
				pw[nTempIndex++] = (uint16)(nBaseIndex + nWidth - nLODFactor);
				pw[nTempIndex++] = (uint16)(nBaseIndex + nRowSkipWidth * 2);
			}
			else
			{
				pw[nTempIndex++] = (uint16)(nBaseIndex + nRowSkipWidth * 2 - 1);
				pw[nTempIndex++] = (uint16)(nBaseIndex + nRowSkipWidth);
			}


			nBaseIndex += nRowSkipWidth;
			nStripBaseIndex += (nLODWidth * 2) + 2;
		}
	}

	GfxIBAllocTracked(eUsage, nIndexCount, fPurgeIfNecessary, pw, ibid, __FILE__, __LINE__, "");

	SigmaMemoryFree(pw, NULL);
}






void GfxQuadlistIBAlloc(
    const ResourceUsage eUsage,
    const int32 nQuadCount,
    const BOOL fPurgeIfNecessary,
    int32 & nIndexCount,
    IBID & ibid)
{




    nIndexCount = nQuadCount * 6;
    uint16 * pw = (uint16 *)SigmaMemoryNew(sizeof(GfxNdx) * nIndexCount, 4, NULL, FALSE);
    {
        int32 nIndex = 0;
        int32 nVertex = 0;


        for (int32 i = 0; i < nQuadCount; i++)
        {
            pw[nIndex++] = (uint16)(nVertex + 0);
            pw[nIndex++] = (uint16)(nVertex + 1);
            pw[nIndex++] = (uint16)(nVertex + 2);
            pw[nIndex++] = (uint16)(nVertex + 2);
            pw[nIndex++] = (uint16)(nVertex + 1);
            pw[nIndex++] = (uint16)(nVertex + 3);
            nVertex += 4;
        }
    }



    GfxIBAllocTracked(eUsage, nIndexCount, fPurgeIfNecessary, pw, ibid, __FILE__, __LINE__, "");

    SigmaMemoryFree(pw, NULL);
}






IBID GfxIBAddRef(const IBID ibid) {
    IndexBuffer * pib = IBGet(ibid);
    pib->nRefCount++;
    return ibid;
}









void sIBReallyDispose(void * hib, void *) {
    gptGFXAPI->IBDispose(hib);
}






static void sIBPDispose(IndexBuffer * pib) {
    RenderWorkerData & workerData = gptGfxData->workerData[GfxGetWorkerIndex()];
    workerData.tStats.dwIBMemUsage -= pib->dwMaxIndexCount * sizeof(GfxNdx);

    if (gptGFXAPI->IBIsBusy(pib->hib)) {
        CGameAsyncRenderRegisterGPUResourceDelayedDeletion(sIBReallyDispose, pib->hib, NULL);
    } else {
        gptGFXAPI->IBDispose(pib->hib);
    }

    if (pib->eUsage == RESOURCEUSAGE_DYNAMIC) {
        gptGfxData->tDynIBList.RemoveAt(gptGfxData->tDynIBList.Find(pib->id));
    }

    gptGfxData->ptIBList->ElementDelete(pib, FALSE);
}


















void GfxIBRelease(const IBID ibid) {
    if (!GfxIBIsValid(ibid)) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tIBListMutex);

    IndexBuffer * pib = IBGet(ibid);
    if (pib->nRefCount > 0) {
        pib->nRefCount--;
        if (pib->nRefCount == 0) {
            sIBPDispose(pib);
        }
    }
}













void GfxIBFree(const IBID ibid) {
    if (gptGfxData == NULL || gptGfxData->ptIBList == NULL) {
        return;
    }
    if (!GfxIBIsValid(ibid)) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tIBListMutex);

    IndexBuffer * pib = IBGet(ibid);
    sIBPDispose(pib);
}











void GfxIBFreeAll(const BOOL fHardReset) {
    if (gptGfxData == NULL || gptGfxData->ptIBList == NULL) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tIBListMutex);

    if (fHardReset) {
        TDataArrayIterator<IndexBuffer> tIt;
        while (IndexBuffer * pib = gptGfxData->ptIBList->Iterate(tIt)) {
            sIBPDispose(pib);
        }
    } else {
        POS pos = gptGfxData->tDynIBList.GetHeadPosition();
        while (pos) {
            IBID ibid = gptGfxData->tDynIBList.GetNext(pos);
            IndexBuffer * pib = gptGfxData->ptIBList->ElementGet(ibid);
            sIBPDispose(pib);
        }
    }
}


















void GfxIBFlushAllDynamic() {
    if (gptGfxData == NULL || gptGfxData->ptIBList == NULL) {
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(gptGfxData->tIBListMutex);

    POS pos = gptGfxData->tDynIBList.GetHeadPosition();
    while (pos) {
        IBID ibid = gptGfxData->tDynIBList.GetNext(pos);
        IndexBuffer * pib = gptGfxData->ptIBList->ElementGet(ibid);
        gptGFXAPI->IBFlush(pib->hib);
    }
}












GfxNdx * GfxIBLock(const IBID ibid, const uint32 dwCount, uint32 & dwStartIndex) {
    uint32 dwStartOffset;
    GfxNdx * lpIndices;
    IndexBuffer * pib = IBGet(ibid);

    dwStartOffset = 0;
    lpIndices = gptGFXAPI->IBLock(pib->hib, dwCount * sizeof(GfxNdx), dwStartOffset);
    dwStartIndex = dwStartOffset / sizeof(GfxNdx);
    return lpIndices;
}

















void GfxIBUnlock(const IBID ibid) {
    IndexBuffer * pib = IBGet(ibid);
    gptGFXAPI->IBUnlock(pib->hib);
}


























































































































void GfxIBSetProxy(const IBID ibid, uint16 * pwSrc) {
    IndexBuffer * pib = IBGet(ibid);
    gptGFXAPI->IBSetProxy(pib->hib, pwSrc);
}














































BOOL GfxIBIsValid(const IBID ibid) {
    return IBTryToGet(ibid) != NULL;
}

























void GfxIBSetCurrent(const IBID ibid) {
    if (ibid != IBID_INVALID) {
        IndexBuffer * pib = IBGet(ibid);
        gptGFXAPI->IBSetCurrent(pib->hib);
    } else {
        gptGFXAPI->IBSetCurrent(NULL);
    }
}










inline IndexBuffer * IBGet(const IBID id) {  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    return gptGfxData->ptIBList->ElementGet(id);
}






inline IndexBuffer * IBTryToGet(const IBID id) {  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    return gptGfxData->ptIBList->ElementTryToGet(id);
}





























































void PackedIBListAllocProxy(PackedIBList * ptPIBList, LPCSTR szDebugName) {
    if (ptPIBList == NULL) {
        return;
    }

    POS pos = ptPIBList->GetHeadPosition();
    while (pos) {
        PackedIB & tPIB = ptPIBList->GetNext(pos);
        if (!GfxIBIsValid(tPIB.ibid)) {
            GfxIBAllocProxy(tPIB.nIndexCount, tPIB.ibid, szDebugName);
        } else {
            GfxIBAddRef(tPIB.ibid);
        }
    }
}































void PackedIBListFree(PackedIBList * ptPIBList) {
    if (ptPIBList == NULL) {
        return;
    }

    POS pos = ptPIBList->GetHeadPosition();
    while (pos) {
        PackedIB & tPIB = ptPIBList->GetNext(pos);
        GfxIBFree(tPIB.ibid);
    }
}
