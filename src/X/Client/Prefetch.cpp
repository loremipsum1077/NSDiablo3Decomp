#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Prefetch.h"
#include "CGame.h"
#include "../Common/Game.h"
#include "../Common/ThreadGlobals.h"
#include "../Sigma/SigmaTiming.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SNOFilesAsync.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/RefString.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Common/GameCommonData.h"
#include "../Common/Definitions/Definitions.h"
// For SNONAME_NONE, cgplaceNull and the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354,
// which the original runs in this file's static initializer ahead of its own statics.
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

struct SNOElement;

struct ActivePrefetchData {  // size 0x8
    SNOElement * ptSNOElement;  // 0x00
};

struct QueryAndType {  // size 0x2
    QueryAndType(uint16, Prefetch::PrefetchType);
    uint16 uQueryID : 13;  // 0x00
    uint16 eType : 3;  // 0x00
};

inline QueryAndType::QueryAndType(uint16 _uQueryID, Prefetch::PrefetchType _eType)
    : uQueryID(_uQueryID)
    , eType(_eType)
{
}

struct PrefetchRun {  // size 0x6
    QueryAndType uQueryAndType;  // 0x00
    uint8 uCountLow;  // 0x02
    uint8 uCountHigh : 4;  // 0x03
    uint8 uStartHigh : 4;  // 0x03
    uint16 uStartLow;  // 0x04
    PrefetchRun() : uQueryAndType(0, Prefetch::PT_BASE), uCountLow(0), uCountHigh(0), uStartHigh(0), uStartLow(0) {}
    uint32 GetCount() const
    {
        return (uCountHigh << 8) | uCountLow;
    }
    uint32 GetStart() const
    {
        return (uStartHigh << 16) | uStartLow;
    }
};

struct PrefetchDynRun {  // size 0x4
    QueryAndType uQueryAndType;  // 0x00
    uint16 uCount;  // 0x02
    PrefetchDynRun();
};

typedef XGrowableMap<int, int, PrefetchDynRun *, PrefetchDynRun *> TSNOToDynRun;
typedef XGrowableMap<CompressedSNOName, const CompressedSNOName &, ActivePrefetchData, const ActivePrefetchData &> TSNOPrefetchActiveMap;
typedef blz::vector<CompressedSNOName, blz::allocator<CompressedSNOName> > TRunArray;

struct DefinitionPrefetchData {  // size 0x378
    BOOL bPrefetchUpdating;  // 0x00
    BOOL bPrefetchLockingDefinition;  // 0x04
    int32 nIsAddingCounter;  // 0x08
    int32 nIsActiveWaitingCounter;  // 0x0c
    BOOL fHighPriorityRequestEnabled;  // 0x10
    TSNOPrefetchActiveMap mapActivePrefetch;  // 0x18
    Blizzard::Lock::Mutex tPrefetchMutex;  // 0x98
    TSNOToDynRun mapSNOToDynRunInfo;  // 0xb8
    uint32 dwSNOToRunInfoCapacity;  // 0x138
    uint32 dwHighestUsedSNOHandle;  // 0x13c
    PrefetchRun ** ptSNOToRunInfo;  // 0x140
    uint32 dwRunCount[1];  // 0x148
    PrefetchRun * ptRunInfo[1];  // 0x150
    TRunArray tRuns[1];  // 0x158
    uint16 uQueryID : 13;  // 0x170
    CHAR sLastScanned[128];  // 0x172
    CHAR sLastPrefetched[128];  // 0x1f2
    CHAR sLastCompleted[128];  // 0x272
    CHAR sLastMissed[128];  // 0x2f2
};
static_assert(sizeof(DefinitionPrefetchData) == 0x378, "DefinitionPrefetchData");

enum PrefetchStatus {
    PREFETCH_UNLOADED = 0,
    PREFETCH_LOADING = 1,
    PREFETCH_LOADED = 2,
};

struct PrefetchRunOnDisk {  // size 0x8
    uint32 uCount;  // 0x00
    uint32 uStart;  // 0x04
};
BOOL sLoadHotfixPrefetchData(const char * szHotfixPrefetchFilename, TSNOToDynRun & mapSNOToRunInfo, uint32 dwHighestUsedSNOHandle, PrefetchRun ** ptSNOToRunInfo);


static const TypeDescriptorField PrefetchRunOnDisk_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(PrefetchRunOnDisk, uCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PrefetchRunOnDisk, uStart), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PrefetchRunOnDisk), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PrefetchRunOnDisk("PrefetchRunOnDisk", 0, PrefetchRunOnDisk_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PREFETCHRUN = &TypeDescriptor_PrefetchRunOnDisk;

// X/Common/SNO/SNOFiles.cpp and SNOFilesAsync.cpp (not declared in src headers yet)
SNOElement * SNOFileLookup(const SNOName & snoname);
void SNOFileClose(SNOElement * ptSNOElement);
void SNOFilesAsyncPause();
void SNOFilesAsyncUnpause();
AsyncStatus SNOFilesBlockUntilCompleted(SNOElement * ptSNOElement);
void SNOFilesDisableMaxCompletedSNO();
void SNOFilesEnableMaxCompletedSNO();
BOOL SNOFilesAsyncHasPriorityPending(SNOFilesAsyncPriority ePriority);
void SNOFilesAsyncWaitForInPlaceLock();
void SNOFilesAsyncChangePriority(SNOElement * ptSNOElement, SNOFilesAsyncPriority ePriority);
void SNOFileStartAsynchronousRead(SNOElement * ptSNOElement, SNOFilesAsyncPriority ePriority);
BOOL SNOFileExists(const SNOName & snoname);
AsyncRecordRef SNOFileGetAsyncRecord(SNOElement * ptSNOElement);
int32 SNOFilesAsyncGetPriorityPendingCount(SNOFilesAsyncPriority ePriority);
BOOL SNOFilesAsyncHasPending();
int32 SNOFilesAsyncGetCompletedSNOs(SNOElement ** pptCompletedSNOElements, int32 nCapacity);
void SNOFilesAsyncStartNewRun();
void SNOFilesAsyncKick();
struct SNOLazyData;
SNOLazyData * SNOFileGetLazyData(SNOElement * ptSNOElement);
// X/Client/Prefetch.h (Prefetch.cpp:1460, inline only).
namespace Prefetch { BOOL HasPending(); }
// X/Common/SNO/SNOFiles.cpp.
CRefString SNOFilesGetMiscPath();
SNO SNOFilesGetHighestUsedSNOHandle();
namespace Prefetch {
CRefString GetMPQFilename(PrefetchType eType);
CRefString GetLooseFilename(PrefetchType eType);
}  // namespace Prefetch





#include "../Common/FileIO/FileIO.h"
namespace FileIO {










}  // namespace FileIO


class PrefetchReader {  // size 0x18
public:
    PrefetchReader(const char * szMPQFilename, const char * szLooseFilename);
    ~PrefetchReader();
    BOOL IsValid() const;
    BOOL FileReadSequential(void * lpBuf, const uint32 dwSize);
    enum State {
        MOPAQ = 0,
        INVALID = 1,
    };
private:
    PrefetchReader::State m_eState;  // 0x00
    FileIO::File m_tFile;  // 0x08
};
static_assert(sizeof(PrefetchReader) == 0x18, "PrefetchReader");





















inline BOOL PrefetchReader::IsValid() const
{
    return m_eState != INVALID;
}



















inline PrefetchReader::PrefetchReader(const char * szMPQFilename, const char * szLooseFilename)
    : m_eState(INVALID)
    , m_tFile((XLocale)1)
{
    if (m_tFile.Open(szLooseFilename, szMPQFilename))
    {
        m_eState = MOPAQ;
    }
}





inline PrefetchReader::~PrefetchReader()
{
}




inline BOOL PrefetchReader::FileReadSequential(void * lpBuf, const uint32 dwSize)
{
    if (m_eState == MOPAQ)
    {
        uint64 dwBytesRead = 0;
        if (!m_tFile.ReadFile(lpBuf, dwSize, &dwBytesRead))
        {
            return FALSE;
        }
        return dwSize == (uint32)dwBytesRead;
    }
    return FALSE;
}










void sLoadPrefetchData(Prefetch::PrefetchType eType, PrefetchRun ** ptSNOToRunInfo, uint32 dwSNOToRunInfoCapacity, uint32 dwHighestUsedSNOHandle, PrefetchRun *& ptRunInfo, uint32 & dwRunCount, TRunArray & tRuns)
{
    ptRunInfo = NULL;
    uint32 dwCount = 0;

    CRefString sMPQPath = Prefetch::GetMPQFilename(eType);
    CRefString sLooseFilename = Prefetch::GetLooseFilename(eType);

    PrefetchReader fileReader(sMPQPath, sLooseFilename);
    if (!fileReader.IsValid())
    {
        goto Error;
    }

    if (!fileReader.FileReadSequential(&dwCount, sizeof(uint32)))
    {
        goto Error;
    }

    {
        dwRunCount = dwCount;
        ptRunInfo = new PrefetchRun[dwCount];

        uint32 dwSize = dwCount * (sizeof(SNOName) + sizeof(PrefetchRunOnDisk));
        uint8 * pBuffer = (uint8 *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
        if (!fileReader.FileReadSequential(pBuffer, dwSize))
        {
            SigmaMemoryFree(pBuffer, NULL);
            goto Error;
        }

        uint8 * pCurBuffer = pBuffer;
        for (uint32 i = 0; i != dwCount; i++)
        {
            SNOName * ptSNOName = (SNOName *)pCurBuffer;
            PrefetchRunOnDisk * ptRunOnDisk = (PrefetchRunOnDisk *)(pCurBuffer + sizeof(SNOName));
            pCurBuffer += sizeof(SNOName) + sizeof(PrefetchRunOnDisk);

            PrefetchRun & tRun = ptRunInfo[i];
            tRun.uQueryAndType = QueryAndType(0, eType);
            tRun.uCountLow = (uint8)ptRunOnDisk->uCount;
            tRun.uCountHigh = (uint8)(ptRunOnDisk->uCount >> 8);
            tRun.uStartHigh = (uint8)(ptRunOnDisk->uStart >> 16);
            tRun.uStartLow = (uint16)ptRunOnDisk->uStart;

            uint32 dwIndex = ptSNOName->snoHandle;
            if (dwIndex <= dwHighestUsedSNOHandle)
            {
                if (ptSNOToRunInfo[dwIndex] != (PrefetchRun *)0xFFFFFFFF)
                {
                    ptSNOToRunInfo[dwIndex] = &ptRunInfo[i];
                }
            }
        }

        SigmaMemoryFree(pBuffer, NULL);
    }

    if (!fileReader.FileReadSequential(&dwCount, sizeof(uint32)))
    {
        goto Error;
    }

    {
        tRuns.resize(dwCount);
        CompressedSNOName * pCompSNONames = tRuns.data();
        uint32 dwSize = dwCount * sizeof(SNOName);
        SNOName * pSNONames = (SNOName *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
        if (!fileReader.FileReadSequential(pSNONames, dwSize))
        {
            SigmaMemoryFree(pSNONames, NULL);
            goto Error;
        }

        for (uint32 i = 0; i < dwCount; i++)
        {
            pCompSNONames[i] = CompressedSNOName(pSNONames[i]);
        }

        SigmaMemoryFree(pSNONames, NULL);
    }
    return;

Error:
    if (ptRunInfo)
    {
        for (uint32 i = 0; i < dwSNOToRunInfoCapacity; i++)
        {
            if (ptSNOToRunInfo[i] != NULL && ptSNOToRunInfo[i] != (PrefetchRun *)0xFFFFFFFF && ptSNOToRunInfo[i]->uQueryAndType.eType == eType)
            {
                ptSNOToRunInfo[i] = NULL;
            }
        }
        delete[] ptRunInfo;
        ptRunInfo = NULL;
    }

    tRuns.~TRunArray();
    new (&tRuns) TRunArray();
}















































































































inline DefinitionPrefetchData * GetPrefetchGlobals()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    return tAppGlobals.ptPrefetchGlobals;
}




void Prefetch::InitForApp()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    DefinitionPrefetchData *& ptPrefetchGlobals = tAppGlobals.ptPrefetchGlobals;

    ptPrefetchGlobals = (DefinitionPrefetchData *)SigmaMemoryNew(sizeof(DefinitionPrefetchData), 4, NULL, TRUE);
    ptPrefetchGlobals->mapActivePrefetch.Initialize(0x10000, FALSE, NULL);
    ptPrefetchGlobals->mapActivePrefetch.SetHashTableSize_Tracked(0x10000);
    ptPrefetchGlobals->mapActivePrefetch.SetGrow(XALLOC_GROW_LINEAR, 512);
    ptPrefetchGlobals->mapActivePrefetch.SetShrinkable(TRUE);
    ptPrefetchGlobals->fHighPriorityRequestEnabled = TRUE;
    for (int32 i = 0; i < PT_COUNT; ++i)
    {
        new (&ptPrefetchGlobals->tRuns[i]) TRunArray();
    }

    Blizzard::Lock::MutexCreate(ptPrefetchGlobals->tPrefetchMutex);

    ptPrefetchGlobals->mapSNOToDynRunInfo.Initialize(16, FALSE, NULL);
    ptPrefetchGlobals->mapSNOToDynRunInfo.SetHashTableSize_Tracked(256);


    ptPrefetchGlobals->dwSNOToRunInfoCapacity = SNOFilesGetHighestUsedSNOHandle() + 1;
    ptPrefetchGlobals->dwHighestUsedSNOHandle = SNOFilesGetHighestUsedSNOHandle();
    ptPrefetchGlobals->ptSNOToRunInfo = new PrefetchRun *[ptPrefetchGlobals->dwSNOToRunInfoCapacity];
    SigmaMemoryClear(ptPrefetchGlobals->ptSNOToRunInfo, ptPrefetchGlobals->dwSNOToRunInfoCapacity * sizeof(PrefetchRun *));

    for (PrefetchType eType = PT_BASE; eType < PT_COUNT; eType = (PrefetchType)(eType + 1))
    {
        sLoadPrefetchData(eType, ptPrefetchGlobals->ptSNOToRunInfo, ptPrefetchGlobals->dwSNOToRunInfoCapacity, ptPrefetchGlobals->dwHighestUsedSNOHandle, ptPrefetchGlobals->ptRunInfo[eType], ptPrefetchGlobals->dwRunCount[eType], ptPrefetchGlobals->tRuns[eType]);
    }
}








BOOL sLoadHotfixPrefetchData(const char * szHotfixPrefetchFilename, TSNOToDynRun & mapSNOToRunInfo, uint32 dwHighestUsedSNOHandle, PrefetchRun ** ptSNOToRunInfo)
{




    PrefetchReader fileReader(szHotfixPrefetchFilename, szHotfixPrefetchFilename);
    if (!fileReader.IsValid())
    {
        return FALSE;
    }

    uint32 dwPrefetchRunInfoCount;
    if (!fileReader.FileReadSequential(&dwPrefetchRunInfoCount, sizeof(uint32)))
    {
        return FALSE;
    }

    uint32 dwSize = dwPrefetchRunInfoCount * (sizeof(SNOName) + sizeof(PrefetchRunOnDisk));
    uint8 * pBuffer = (uint8 *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
    if (!fileReader.FileReadSequential(pBuffer, dwSize))
    {
        SigmaMemoryFree(pBuffer, NULL);
        return FALSE;
    }

    uint32 dwSNONameCount;
    if (!fileReader.FileReadSequential(&dwSNONameCount, sizeof(uint32)))
    {
        SigmaMemoryFree(pBuffer, NULL);
        return FALSE;
    }

    dwSize = dwSNONameCount * sizeof(SNOName);
    SNOName * pSNONames = (SNOName *)SigmaMemoryNew(dwSize, 4, NULL, FALSE);
    if (!fileReader.FileReadSequential(pSNONames, dwSize))
    {
        SigmaMemoryFree(pBuffer, NULL);
        SigmaMemoryFree(pSNONames, NULL);
        return FALSE;
    }

    uint8 * pCurBuffer = pBuffer;
    for (uint32 i = 0; i != dwPrefetchRunInfoCount; i++)
    {
        SNOName * ptSNOName = (SNOName *)pCurBuffer;
        PrefetchRunOnDisk * ptRunOnDisk = (PrefetchRunOnDisk *)(pCurBuffer + sizeof(SNOName));
        pCurBuffer += sizeof(SNOName) + sizeof(PrefetchRunOnDisk);

        PrefetchDynRun * ptDynRun = (PrefetchDynRun *)SigmaMemoryNew((uint32)(sizeof(PrefetchDynRun) + ptRunOnDisk->uCount * sizeof(CompressedSNOName)), 4, NULL, TRUE);
        CompressedSNOName * ptSNONameArray = (CompressedSNOName *)(ptDynRun + 1);


        ptDynRun->uCount = (uint16)ptRunOnDisk->uCount;
        for (uint32 j = 0; j < ptRunOnDisk->uCount; j++)
        {

            ptSNONameArray[j] = CompressedSNOName(pSNONames[ptRunOnDisk->uStart + j]);
        }


        mapSNOToRunInfo.SetAt_Tracked(ptSNOName->snoHandle, ptDynRun);


        uint32 dwIndex = ptSNOName->snoHandle;
        if (dwIndex <= dwHighestUsedSNOHandle)
        {

            ptSNOToRunInfo[dwIndex] = (PrefetchRun *)0xFFFFFFFF;
        }
    }



    SigmaMemoryFree(pBuffer, NULL);
    SigmaMemoryFree(pSNONames, NULL);
    return TRUE;
}
BOOL Prefetch::ApplyHotfix(const char * szHotfixName)
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    DefinitionPrefetchData *& ptPrefetchGlobals = tAppGlobals.ptPrefetchGlobals;

    CRefString strHotfixPrefetchFilename;
    return sLoadHotfixPrefetchData(strHotfixPrefetchFilename.Format("Misc\\Hotfixes\\%s.prefetch", szHotfixName), ptPrefetchGlobals->mapSNOToDynRunInfo, ptPrefetchGlobals->dwHighestUsedSNOHandle, ptPrefetchGlobals->ptSNOToRunInfo);
}




void Prefetch::DisposeForApp()
{
    CancelAllActivePrefetches();

    AppGlobals & tAppGlobals = sgtAppGlobals;
    DefinitionPrefetchData *& ptPrefetchGlobals = tAppGlobals.ptPrefetchGlobals;
    if (!ptPrefetchGlobals)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    for (int32 i = 0; i < PT_COUNT; ++i)
        ptPrefetchGlobals->tRuns[i].~TRunArray();

    POS pos = ptPrefetchGlobals->mapSNOToDynRunInfo.GetStartAssoc();
    while (pos)
    {
        PrefetchDynRun * ptRun;
        SNO key;
        ptPrefetchGlobals->mapSNOToDynRunInfo.GetNextAssoc(pos, key, ptRun);
        SigmaMemoryFree(ptRun, NULL);
    }
    ptPrefetchGlobals->mapSNOToDynRunInfo.Dispose();

    Blizzard::Lock::MutexDestroy(ptPrefetchGlobals->tPrefetchMutex);
    ptPrefetchGlobals->mapActivePrefetch.Dispose();

    if (ptPrefetchGlobals->ptSNOToRunInfo)
        delete[] ptPrefetchGlobals->ptSNOToRunInfo;

    for (int32 i = 0; i < PT_COUNT; ++i)
    {
        if (ptPrefetchGlobals->ptRunInfo[i])
            delete[] ptPrefetchGlobals->ptRunInfo[i];
    }

    SigmaMemoryFree(ptPrefetchGlobals, NULL);
    ptPrefetchGlobals = NULL;
}












































void Prefetch::CancelAllActivePrefetches()
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return;

    {
        Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);

        POS pos = ptPrefetchGlobals->mapActivePrefetch.GetStartAssoc();
        while (pos)
        {
            ActivePrefetchData tActivePrefetchData;
            CompressedSNOName snoname;
            ptPrefetchGlobals->mapActivePrefetch.GetNextAssoc(pos, snoname, tActivePrefetchData);

            if (SNOFilesBlockUntilCompleted(tActivePrefetchData.ptSNOElement) == e_canceled)
                Blizzard::Lock::Atomic::And(~(SNOENTRY_FLAG_ASYNCHRONOUS | SNOENTRY_FLAG_READING), &tActivePrefetchData.ptSNOElement->dwFlags);
            else
                SNOFileClose(tActivePrefetchData.ptSNOElement);
        }

        ptPrefetchGlobals->mapActivePrefetch.RemoveAll();
    }

    SNOFilesAsyncWaitForInPlaceLock();
}

















static void sPrefetchFinishSNO(const SNOName & snoname)
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();

    ptPrefetchGlobals->mapActivePrefetch.RemoveAt(CompressedSNOName(snoname));
}




BOOL Prefetch::TakeSNOElement(const SNOName & snoname, SNOElement *& ptSNOElement)
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return FALSE;

    BOOL bLookup;
    {
        Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);

        ActivePrefetchData tActivePrefetchData;
        bLookup = ptPrefetchGlobals->mapActivePrefetch.Lookup(CompressedSNOName(snoname), tActivePrefetchData);
        if (bLookup)
        {
            ptSNOElement = tActivePrefetchData.ptSNOElement;
            sPrefetchFinishSNO(snoname);
        }
    }
    return bLookup;
}






























void Prefetch::Cancel(const SNOName & snoname)
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return;

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);

    ActivePrefetchData tActivePrefetchData;
    if (ptPrefetchGlobals->mapActivePrefetch.Lookup(CompressedSNOName(snoname), tActivePrefetchData))
    {
        AsyncStatus status = SNOFilesBlockUntilCompleted(tActivePrefetchData.ptSNOElement);
        if (status == e_successful)
            SNOFileClose(tActivePrefetchData.ptSNOElement);
        else
            Blizzard::Lock::Atomic::And(~(SNOENTRY_FLAG_ASYNCHRONOUS | SNOENTRY_FLAG_READING), &tActivePrefetchData.ptSNOElement->dwFlags);

        sPrefetchFinishSNO(snoname);
    }
}















void Prefetch::WasCancelled(const SNOName & snoname)
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return;

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);

    ActivePrefetchData tActivePrefetchData;
    if (ptPrefetchGlobals->mapActivePrefetch.Lookup(CompressedSNOName(snoname), tActivePrefetchData))
    {
        Blizzard::Lock::Atomic::And(~(SNOENTRY_FLAG_ASYNCHRONOUS | SNOENTRY_FLAG_READING), &tActivePrefetchData.ptSNOElement->dwFlags);
        sPrefetchFinishSNO(snoname);
    }
}










static PrefetchStatus sPrefetchSnoGetStatus(const SNOName & snoname)
{
    SNOElement * ptSNOElement = SNOFileLookup(snoname);
    if (!ptSNOElement)
        return PREFETCH_LOADED;

    uint32 dwFlags = ptSNOElement->dwFlags;
    if (dwFlags & SNOENTRY_FLAG_ASYNCHRONOUS)
    {
        if (dwFlags & SNOENTRY_FLAG_READING)
            return PREFETCH_LOADING;
        return PREFETCH_LOADED;
    }

    if (dwFlags & SNOENTRY_FLAG_LOADED)
        return PREFETCH_LOADED;

    if (dwFlags & SNOENTRY_FLAG_LOADING)
        return PREFETCH_LOADING;

    return PREFETCH_UNLOADED;
}




















static BOOL sQueueAndStartAsyncRead(SNOElement * ptSNOElement, SNOFilesAsyncPriority ePriority)
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();

    SNOName snoname(ptSNOElement->tEntry.snoGroup, ptSNOElement->tEntry.uGuid);
    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);

    if (sPrefetchSnoGetStatus(snoname) != PREFETCH_UNLOADED)
        return FALSE;

    if (ptPrefetchGlobals->mapActivePrefetch.LookupPtr(CompressedSNOName(snoname)))
        return FALSE;

    ActivePrefetchData tActivePrefetchData;
    tActivePrefetchData.ptSNOElement = ptSNOElement;
    ptPrefetchGlobals->mapActivePrefetch.SetAt_Tracked(CompressedSNOName(snoname), tActivePrefetchData);

    if (ePriority == 2 && !ptPrefetchGlobals->fHighPriorityRequestEnabled)
        ePriority = 1;

    SNOFileStartAsynchronousRead(ptSNOElement, ePriority);
    if (!SNOFileGetAsyncRecord(ptSNOElement))
        ptPrefetchGlobals->mapActivePrefetch.RemoveAt(CompressedSNOName(snoname));

    return SNOFileGetAsyncRecord(ptSNOElement) != NULL;
}


























static void sPrefetchSno(const SNOName & snoname, SNOFilesAsyncPriority ePriority)
{
    if (snoname.snoHandle == SNO_NONE)
        return;

    BOOL bExists = SNOFileExists(snoname);
    if (!bExists)
        return;

    SNOElement * ptSNOElement = SNOFileLookup(snoname);
    if (!ptSNOElement)
        return;

    sQueueAndStartAsyncRead(ptSNOElement, ePriority);
}







































































void Prefetch::BeginAdd()
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return;

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);
    if (ptPrefetchGlobals->nIsAddingCounter++ == 0)
        SNOFilesAsyncPause();
}











void Prefetch::EndAdd()
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return;

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);
    if (--ptPrefetchGlobals->nIsAddingCounter == 0)
        SNOFilesAsyncUnpause();
}









































void Prefetch::BeginActiveWait()
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return;

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);
    if (ptPrefetchGlobals->nIsActiveWaitingCounter++ == 0)
        SNOFilesDisableMaxCompletedSNO();
}











void Prefetch::EndActiveWait()
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
        return;

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);
    if (--ptPrefetchGlobals->nIsActiveWaitingCounter == 0)
        SNOFilesEnableMaxCompletedSNO();
}












































BOOL Prefetch::AddToQueue(const SNOName & snoname, SNOFilesAsyncPriority ePriority)
{
    if (snoname.snoHandle == SNO_NONE)
        return TRUE;

    PrefetchStatus eStatus = sPrefetchSnoGetStatus(snoname);
    if (eStatus == PREFETCH_UNLOADED)
    {
        sPrefetchSno(snoname, ePriority);
    }
    else if (eStatus == PREFETCH_LOADING)
    {
        SNOElement * ptSNOElement = SNOFileLookup(snoname);
        if (ptSNOElement)
        {
            if (ePriority == 2)
            {
                DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
                if (ptPrefetchGlobals && !ptPrefetchGlobals->fHighPriorityRequestEnabled)
                    ePriority = 1;
            }
            SNOFilesAsyncChangePriority(ptSNOElement, ePriority);
        }
    }

    return eStatus == PREFETCH_LOADED;
}































void Prefetch::BlockUntilPriorityIdle(SNOFilesAsyncPriority ePriority)
{
    Blizzard::Time::Millisecond dwStartMS;
    Blizzard::Time::Millisecond dwCurMS;
    int32 nPendingCount;

    BOOL fMainThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_MAIN;
    dwStartMS = Blizzard::Time::Milliseconds();

    if (fMainThread)
        BeginActiveWait();

    nPendingCount = GetPriorityPendingCount(ePriority);
    dwCurMS = dwStartMS;
    while (nPendingCount > 0)
    {
        if (fMainThread)
        {
            Update(TRUE);

            if (Blizzard::Time::Milliseconds() - dwCurMS > 16)
            {
                dwCurMS = Blizzard::Time::Milliseconds();
                if (CGameLoadingScreenIsEnabled())
                {
                    CGameProgressLoadingScreen(TRUE);
                    if (CGameLoadingScreenHasExpired())
                        break;
                }
            }
        }

        int32 nNewPendingCount;
        Sleep(1);
        nNewPendingCount = GetPriorityPendingCount(ePriority);
        if (Blizzard::Time::Milliseconds() - dwStartMS > 1000 && nNewPendingCount > nPendingCount)
            break;
        nPendingCount = nNewPendingCount;
    }

    if (fMainThread)
        EndActiveWait();
}





























































static void sPrefetchRemoveCompleted(BOOL fUpdateToCompletion)
{
    SNOElement * pptCompletedSNOElements[32];
    SGameID idSGame = GameServerCodeLeave_Tracked(TRUE);

    fUpdateToCompletion = CGameGetGameCommonData().GameModeGet() == GAMEMODE_PLAYING ? (CGameLoadingScreenIsEnabled() || fUpdateToCompletion) : TRUE;

    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    Blizzard::Time::Microsecond tTimeStart = Blizzard::Time::Microseconds();

    Blizzard::Time::Microsecond tTimeLimit = ptPrefetchGlobals->nIsActiveWaitingCounter ? 8000 : 500;

    BOOL fStopProcessing = FALSE;
    int32 nDoneLoadingSNOs;
    do
    {
        nDoneLoadingSNOs = SNOFilesAsyncGetCompletedSNOs(pptCompletedSNOElements, 32);



        for (int32 i = 0; i < nDoneLoadingSNOs; i++)
        {
            Blizzard::Time::Microsecond tTime = Blizzard::Time::Microseconds();
            if (tTime - tTimeStart > tTimeLimit && !fUpdateToCompletion)
            {
                fStopProcessing = TRUE;
                break;
            }

            SNOElement * ptCompletedSNOElement = pptCompletedSNOElements[i];

            const SNOEntry & tEntry = ptCompletedSNOElement->tEntry;
            SNOName snoname(tEntry.snoGroup, tEntry.uGuid);

            if (SNOFileGetLazyData(ptCompletedSNOElement) == NULL)
            {
                Prefetch::Cancel(snoname);
                continue;
            }


            ptPrefetchGlobals->bPrefetchLockingDefinition = TRUE;
            GroupDefinition * ptGroupDef = GameDefinitionGet(snoname.snoGroup);
            if (ptCompletedSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE)
            {
                ptGroupDef->LoadChildFile((SNOElementChild *)ptCompletedSNOElement);
            }
            else
            {
                ptGroupDef->Preload(snoname.snoHandle);
            }
            ptPrefetchGlobals->bPrefetchLockingDefinition = FALSE;
        }
    } while (nDoneLoadingSNOs > 0 && !fStopProcessing);

    SNOFilesAsyncStartNewRun();

    if (!GameServerCodeEnter_Track()) _GameServerCodeEnter(idSGame);
}










BOOL Prefetch::IsPending(const SNOName & snoname)
{
    return sPrefetchSnoGetStatus(snoname) == PREFETCH_LOADING;
}



BOOL Prefetch::IsLoaded(const SNOName & snoname)
{
    return sPrefetchSnoGetStatus(snoname) == PREFETCH_LOADED;
}



BOOL Prefetch::IsLockingDefinition()
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    return ptPrefetchGlobals->bPrefetchLockingDefinition;
}


















inline BOOL Prefetch::HasPending()
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
    {
        return FALSE;
    }
    return ptPrefetchGlobals->mapActivePrefetch.Count() != 0 || SNOFilesAsyncHasPending();
}




void Prefetch::EnableHighPriority(BOOL fEnabled)
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (ptPrefetchGlobals)
        ptPrefetchGlobals->fHighPriorityRequestEnabled = fEnabled;
}







inline int32 Prefetch::GetPriorityPendingCount(SNOFilesAsyncPriority ePriority)
{
    return SNOFilesAsyncGetPriorityPendingCount(ePriority);
}



BOOL Prefetch::HasPriorityPending(SNOFilesAsyncPriority ePriority)
{
    return SNOFilesAsyncHasPriorityPending(ePriority);
}





void Prefetch::Update(BOOL fUpdateToCompletion)
{
    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
    {
        return;
    }
    if (!HasPending())
    {
        return;
    }

    ptPrefetchGlobals->bPrefetchUpdating = TRUE;
    sPrefetchRemoveCompleted(fUpdateToCompletion);
    ptPrefetchGlobals->bPrefetchUpdating = FALSE;

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);
    if (ptPrefetchGlobals->nIsAddingCounter == 0)
    {
        SNOFilesAsyncKick();
    }
}
// X/Common/SNO/SNOName.h:118 (inlined into 16 functions); SNOManager.cpp defines it the same way.
inline CompressedSNOName::operator SNOName() {
    return SNOName(snoGroup, snoHandle);
}






































































































































inline CRefString Prefetch::GetMPQFilename(PrefetchType eType)
{
    return CRefString("Prefetch.dat");
}





















inline CRefString Prefetch::GetLooseFilename(PrefetchType eType)
{
    return CRefString(CRefString().Format("%s\\Prefetch.dat", SNOFilesGetMiscPath().str()));
}




















static void sResetQueryID(PrefetchRun * ptRunInfo, uint32 dwRunCount)
{


    for (uint32 i = 0; i < dwRunCount; i++)
    {
        ptRunInfo[i].uQueryAndType.uQueryID = 0;
    }
}





static void sResetDynQueryID(TSNOToDynRun & mapSNOToRunInfo)
{
    int32 nKey;
    POS pos = mapSNOToRunInfo.GetStartAssoc();
    while (pos)
    {
        PrefetchDynRun * ptRun;
        mapSNOToRunInfo.GetNextAssoc(pos, nKey, ptRun);
        ptRun->uQueryAndType.uQueryID = 0;
    }
}






static BOOL sGetRunChildren(SNO sno, PrefetchRun ** ptSNOToRunInfo, TRunArray * ptRuns, uint32 dwHighestUsedSNOHandle, TSNOToDynRun & mapSNOToDynRun, QueryAndType *& puQueryAndType, uint32 & uCount, CompressedSNOName *& ptRunChildren)
{
    uint32 dwIndex = sno;
    if (dwIndex > dwHighestUsedSNOHandle)
    {
        PrefetchDynRun * ptDynRun;
        if (mapSNOToDynRun.Lookup(sno, ptDynRun))
        {
            uCount = ptDynRun->uCount;
            puQueryAndType = &ptDynRun->uQueryAndType;
            ptRunChildren = (CompressedSNOName *)(ptDynRun + 1);
            return TRUE;
        }
    }
    else
    {
        PrefetchRun * ptRun = ptSNOToRunInfo[dwIndex];
        if (ptRun != NULL)
        {
            if (ptRun == (PrefetchRun *)0xFFFFFFFF)
            {
                PrefetchDynRun * ptDynRun;
                if (mapSNOToDynRun.Lookup(sno, ptDynRun))
                {
                    uCount = ptDynRun->uCount;
                    puQueryAndType = &ptDynRun->uQueryAndType;
                    ptRunChildren = (CompressedSNOName *)(ptDynRun + 1);
                    return TRUE;
                }
            }
            else
            {
                uCount = ptRun->GetCount();
                puQueryAndType = &ptRun->uQueryAndType;
                ptRunChildren = ptRuns[ptRun->uQueryAndType.eType].data() + ptRun->GetStart();
                return TRUE;
            }
        }
    }
    return FALSE;
}


























BOOL Prefetch::GetChildSNOs(const SNOName & snoName, ChildSNOContainer & snoContainer, FilterFunc ptFilterFunc, BOOL fAddBase)
{





    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
    {
        return FALSE;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);

    ptPrefetchGlobals->uQueryID++;
    if (ptPrefetchGlobals->uQueryID == 0)
    {
        for (int32 i = 0; i < PT_COUNT; ++i)
        {
            sResetQueryID(ptPrefetchGlobals->ptRunInfo[i], ptPrefetchGlobals->dwRunCount[i]);
        }

        sResetDynQueryID(ptPrefetchGlobals->mapSNOToDynRunInfo);

        ptPrefetchGlobals->uQueryID++;
    }

    uint16 uCurrentQueryID = ptPrefetchGlobals->uQueryID;
    PrefetchRun ** ptSNOToRunInfo = ptPrefetchGlobals->ptSNOToRunInfo;
    uint32 dwHighestUsedSNOHandle = ptPrefetchGlobals->dwHighestUsedSNOHandle;
    TRunArray * ptRuns = ptPrefetchGlobals->tRuns;
    TSNOToDynRun & mapSNOToDynRun = ptPrefetchGlobals->mapSNOToDynRunInfo;



    XGrowableList<SNOName, const SNOName &> stack;

    stack.AddHead_Tracked(snoName);
    if (fAddBase)
    {
        snoContainer.push_back(snoName);
    }

    while (stack.Count())
    {
        SNOName currentSnoName = stack.GetHead();
        stack.RemoveHead();
        SNO snoHandle = currentSnoName.snoHandle;
        QueryAndType * puQueryAndName = NULL;
        uint32 uCount = 0;
        CompressedSNOName * ptRunChildren = NULL;


        if (snoHandle < 0)
        {
            continue;
        }

        if (!sGetRunChildren(snoHandle, ptSNOToRunInfo, ptRuns, dwHighestUsedSNOHandle, mapSNOToDynRun, puQueryAndName, uCount, ptRunChildren))
        {
            continue;
        }

        if (puQueryAndName->uQueryID == uCurrentQueryID)
        {
            continue;
        }
        puQueryAndName->uQueryID = uCurrentQueryID;

        for (uint i = 0; i != uCount; i++)
        {
            SNOName snonameReference = ptRunChildren[i];
            BOOL fTraverseChildren = TRUE;
            if (ptFilterFunc == NULL || ptFilterFunc(CompressedSNOName(snoName), CompressedSNOName(currentSnoName), CompressedSNOName(snonameReference), fTraverseChildren))
            {
                snoContainer.push_back(snonameReference);
            }

            if (fTraverseChildren)
            {
                stack.AddHead_Tracked(snonameReference);
            }
        }
    }

    return TRUE;
}
















BOOL Prefetch::MultiGetChildSNOs(ChildSNOContainer & snoToProcess, ChildSNOContainer & snoContainer, FilterFunc ptFilterFunc)
{




    DefinitionPrefetchData * ptPrefetchGlobals = GetPrefetchGlobals();
    if (!ptPrefetchGlobals)
    {
        return FALSE;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(ptPrefetchGlobals->tPrefetchMutex);

    ptPrefetchGlobals->uQueryID++;
    if (ptPrefetchGlobals->uQueryID == 0)
    {
        for (int32 i = 0; i < PT_COUNT; ++i)
        {
            sResetQueryID(ptPrefetchGlobals->ptRunInfo[i], ptPrefetchGlobals->dwRunCount[i]);
        }

        sResetDynQueryID(ptPrefetchGlobals->mapSNOToDynRunInfo);

        ptPrefetchGlobals->uQueryID++;
    }



    PrefetchRun ** ptSNOToRunInfo = ptPrefetchGlobals->ptSNOToRunInfo;
    TRunArray * ptRuns = ptPrefetchGlobals->tRuns;
    uint16 uCurrentQueryID = ptPrefetchGlobals->uQueryID;
    uint32 dwHighestUsedSNOHandle = ptPrefetchGlobals->dwHighestUsedSNOHandle;
    uint32 nCount;
    TSNOToDynRun & mapSNOToDynRun = ptPrefetchGlobals->mapSNOToDynRunInfo;



    XGrowableList<SNOName, const SNOName &> stack;

    nCount = snoToProcess.size();
    for (uint32 i = 0; i < nCount; i++)
    {
        SNOName snoName = snoToProcess[i];
        stack.AddHead_Tracked(snoName);
        snoContainer.push_back(snoName);

        while (stack.Count())
        {
            SNOName currentSnoName = stack.GetHead();
            stack.RemoveHead();
            SNO snoHandle = currentSnoName.snoHandle;
            QueryAndType * puQueryAndType = NULL;
            uint32 uCount = 0;
            CompressedSNOName * ptRunChildren = NULL;


            if (snoHandle < 0)
            {
                continue;
            }

            if (!sGetRunChildren(snoHandle, ptSNOToRunInfo, ptRuns, dwHighestUsedSNOHandle, mapSNOToDynRun, puQueryAndType, uCount, ptRunChildren))
            {
                continue;
            }

            if (puQueryAndType->uQueryID == uCurrentQueryID)
            {
                continue;
            }
            puQueryAndType->uQueryID = uCurrentQueryID;

            for (uint i = 0; i != uCount; i++)
            {
                SNOName snonameReference = ptRunChildren[i];
                BOOL fTraverseChildren = TRUE;
                if (ptFilterFunc == NULL || ptFilterFunc(CompressedSNOName(snoName), CompressedSNOName(currentSnoName), CompressedSNOName(snonameReference), fTraverseChildren))
                {
                    snoContainer.push_back(snonameReference);
                }

                if (fTraverseChildren)
                {
                    stack.AddHead_Tracked(snonameReference);
                }
            }
        }
    }

    return TRUE;
}
