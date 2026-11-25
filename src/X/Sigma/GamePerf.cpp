#include "GamePerf.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "DataStructures/XGrowableList.h"
#include "DataStructures/XTemplate.h"
#include "Math/MathGeneral.h"
#include "Memory/SigmaMemory.h"
#include "Sigma.h"
#include "Stopwatch.h"

// These belong to headers in other areas that do not exist in src/ yet.
FILE* fopen_ex(const char* szPath, const char* szMode);
// X/Common/Shell/MainNX64.cpp
void ConfigureGamePerfTestFileThread(Blizzard::Thread::Thread tThread);

enum {
    INTERNALPERFCTR_FULLFRAME = 0,
    INTERNALPERFCTR_UNACCOUNTED = 1,
    NUM_INTERNALPERFCTRS = 2,
};

typedef XGrowableList<POS, POS&> ChildList;

struct PerfCtrData {
    PerfTableEntryName szName;
    Stopwatch tStopwatch;
    Stopwatch tStopwatchWithSubs;
    float flTime;
    float flPct;
    float flTimeWithSubs;
    float flCurrentMaxTime;
    float flCurrentMaxPct;
    float flCurrentMaxTimeWithSubs;
    float flAvgTimeTotal;
    float flAvgPctTotal;
    float flAvgTimeTotalWithSubs;
    float flAvgTime;
    float flAvgPct;
    float flMaxTime;
    float flMaxPct;
    float flAvgTimeWithSubs;
    float flMaxTimeWithSubs;
    POS posMyParent;
    ChildList* ptChildList;
    BOOL fIsCounter;
    uint32 dwParentRef;
};

typedef XGrowableList<PerfCtrData, PerfCtrData&> PerfCtrList;
typedef XGrowableList<PerfCtrData*, PerfCtrData*> PerfCtrPointerList;

struct GamePerfGlobals {
    GamePerfStats tStats;
    PerfCtrList tPerfCtrList;
    PerfCtrPointerList tPerfCtrStack;
    POS posPerfCtrInternal[NUM_INTERNALPERFCTRS];
};

struct GamePerfThreadCommand {
    enum class Type : uint32 {
        TYPE_NONE = 0,
        TYPE_OPEN = 1,
        TYPE_WRITE = 2,
        TYPE_FLUSH = 3,
        TYPE_CLOSE = 4,
        TYPE_QUIT = 5,
        TYPE_COUNT = 6,
    };

    Type eType;
    void* pParams;
};

struct GamePerfThreadData {
    static const uint32 MAX_COMMANDS = 64;

    GamePerfThreadCommand atCommands[MAX_COMMANDS];
    Blizzard::Thread::Thread tThread;
    Blizzard::Lock::Semaphore tReadSemaphore;
    Blizzard::Lock::Semaphore tWriteSemaphore;
    uint32 uReadIndex;
    uint32 uWriteIndex;
    bool bActive;
};

struct GamePerfThreadFile {
    struct Buffer {
        char* pData;
        size_t uSize;
    };

    static const uint32 MAX_BUFFERS = 3;
    static const uint32 MAX_BUFFER_SIZE = 4096;

    FILE* pFile;
    char aPath[4096];
    Buffer atBuffers[MAX_BUFFERS];
    Blizzard::Lock::Semaphore tSemaphore;
    uint32 uReadIndex;
    uint32 uWriteIndex;
};

static uint sGamePerfThreadProc(void* pParam) {
    GamePerfThreadData* pData = (GamePerfThreadData*)pParam;
    bool bActive = true;

    while (bActive) {
        Blizzard::Lock::SemaphoreWaitForever(pData->tReadSemaphore);

        const GamePerfThreadCommand& tCommand = pData->atCommands[pData->uReadIndex];
        switch (tCommand.eType) {
        case GamePerfThreadCommand::Type::TYPE_OPEN: {
            GamePerfThreadFile* pFile = (GamePerfThreadFile*)tCommand.pParams;
            pFile->pFile = fopen_ex(pFile->aPath, "w");
            break;
        }
        case GamePerfThreadCommand::Type::TYPE_WRITE: {
            GamePerfThreadFile* pFile = (GamePerfThreadFile*)tCommand.pParams;
            GamePerfThreadFile::Buffer& tBuffer = pFile->atBuffers[pFile->uReadIndex];
            if (tBuffer.uSize > 0) {
                const size_t uResult = fwrite(tBuffer.pData, 1, tBuffer.uSize, pFile->pFile);
                (void)uResult;
            }
            tBuffer.uSize = 0;
            pFile->uReadIndex = (pFile->uReadIndex + 1) % GamePerfThreadFile::MAX_BUFFERS;
            Blizzard::Lock::SemaphoreSet(pFile->tSemaphore);
            break;
        }
        case GamePerfThreadCommand::Type::TYPE_FLUSH: {
            GamePerfThreadFile* pFile = (GamePerfThreadFile*)tCommand.pParams;
            fflush(pFile->pFile);
            break;
        }
        case GamePerfThreadCommand::Type::TYPE_CLOSE: {
            GamePerfThreadFile* pFile = (GamePerfThreadFile*)tCommand.pParams;
            for (uint32 i = 0; i < GamePerfThreadFile::MAX_BUFFERS; i++) {
                GamePerfThreadFile::Buffer& tBuffer = pFile->atBuffers[i];
                SigmaMemoryFree(tBuffer.pData, NULL);
            }
            Blizzard::Lock::SemaphoreDestroy(pFile->tSemaphore);
            fclose(pFile->pFile);
            SigmaMemoryFree(pFile, NULL);
            break;
        }
        case GamePerfThreadCommand::Type::TYPE_QUIT:
            bActive = false;
            break;
        default:
            break;
        }

        Blizzard::Lock::SemaphoreSet(pData->tWriteSemaphore);
        pData->uReadIndex = (pData->uReadIndex + 1) % GamePerfThreadData::MAX_COMMANDS;
    }

    return 0;
}

static void sInitGamePerfThread(GamePerfThreadData* pData) {
    SigmaMemoryClear(pData, sizeof(GamePerfThreadData));

    Blizzard::Lock::SemaphoreCreate(pData->tReadSemaphore);
    Blizzard::Lock::SemaphoreCreate(pData->tWriteSemaphore);
    for (uint32 i = 0; i < GamePerfThreadData::MAX_COMMANDS; i++) {
        Blizzard::Lock::SemaphoreSet(pData->tWriteSemaphore);
    }

    Blizzard::Thread::Create(sGamePerfThreadProc, pData, &pData->tThread, "GamePerfThread", 0, 0);
    ConfigureGamePerfTestFileThread(pData->tThread);
    Blizzard::Thread::Resume(pData->tThread);
}

static void sSubmitGamePerfThreadCommand(GamePerfThreadData* pData, const GamePerfThreadCommand::Type eType,
                                         void* pParams) {
    pData->tWriteSemaphore.WaitForever();
    pData->atCommands[pData->uWriteIndex].eType = eType;
    pData->atCommands[pData->uWriteIndex].pParams = pParams;
    pData->uWriteIndex = (pData->uWriteIndex + 1) % GamePerfThreadData::MAX_COMMANDS;
    pData->tReadSemaphore.Set();
}

static void sDisposeGamePerfThread(GamePerfThreadData* pData) {
    sSubmitGamePerfThreadCommand(pData, GamePerfThreadCommand::Type::TYPE_QUIT, NULL);

    Blizzard::Thread::WaitForever(pData->tThread);
    Blizzard::Thread::Close(pData->tThread);

    Blizzard::Lock::SemaphoreDestroy(pData->tReadSemaphore);
    Blizzard::Lock::SemaphoreDestroy(pData->tWriteSemaphore);
}

static GamePerfThreadFile* sOpenGamePerfThreadFile(GamePerfThreadData* pThreadData, const char* pPath) {
    GamePerfThreadFile* pFile = (GamePerfThreadFile*)SigmaMemoryNew(sizeof(GamePerfThreadFile), 4, NULL, FALSE);
    SigmaMemoryClear(pFile, sizeof(GamePerfThreadFile));

    strncpy(pFile->aPath, pPath, sizeof(pFile->aPath));
    pFile->aPath[sizeof(pFile->aPath) - 1] = '\0';

    Blizzard::Lock::SemaphoreCreate(pFile->tSemaphore);
    for (uint32 i = 0; i < GamePerfThreadFile::MAX_BUFFERS; i++) {
        pFile->atBuffers[i].pData = (char*)SigmaMemoryNew(GamePerfThreadFile::MAX_BUFFER_SIZE, 4, NULL, FALSE);
        Blizzard::Lock::SemaphoreSet(pFile->tSemaphore);
    }
    Blizzard::Lock::SemaphoreWaitForever(pFile->tSemaphore);

    sSubmitGamePerfThreadCommand(pThreadData, GamePerfThreadCommand::Type::TYPE_OPEN, pFile);

    return pFile;
}

static void sWriteGamePerfThreadFile(GamePerfThreadData* pThreadData, GamePerfThreadFile* pFile, const void* pDataIn,
                                     const size_t uDataSize) {
    size_t uDataOffset = 0;

    while (uDataOffset < uDataSize) {
        GamePerfThreadFile::Buffer& tBuffer = pFile->atBuffers[pFile->uWriteIndex];

        void* pDst = tBuffer.pData + tBuffer.uSize;
        const size_t uDstSize = GamePerfThreadFile::MAX_BUFFER_SIZE - tBuffer.uSize;
        const size_t uSrcSize = uDataSize - uDataOffset;
        const size_t uSize = Math::Min(uDstSize, uSrcSize);
        const void* pSrc = (const char*)pDataIn + uDataOffset;

        SigmaMemoryCopy(pDst, pSrc, uSize);
        tBuffer.uSize += uSize;
        uDataOffset += uSize;

        if (tBuffer.uSize == GamePerfThreadFile::MAX_BUFFER_SIZE) {
            sSubmitGamePerfThreadCommand(pThreadData, GamePerfThreadCommand::Type::TYPE_WRITE, pFile);
            pFile->uWriteIndex = (pFile->uWriteIndex + 1) % GamePerfThreadFile::MAX_BUFFERS;
            pFile->tSemaphore.WaitForever();
        }
    }
}

static void sFlushGamePerfThreadFile(GamePerfThreadData* pThreadData, GamePerfThreadFile* pFile) {
    sSubmitGamePerfThreadCommand(pThreadData, GamePerfThreadCommand::Type::TYPE_WRITE, pFile);
    sSubmitGamePerfThreadCommand(pThreadData, GamePerfThreadCommand::Type::TYPE_FLUSH, pFile);
    pFile->uWriteIndex = (pFile->uWriteIndex + 1) % GamePerfThreadFile::MAX_BUFFERS;
    pFile->tSemaphore.WaitForever();
}

static void sCloseGamePerfThreadFile(GamePerfThreadData* pThreadData, GamePerfThreadFile* pFile) {
    sSubmitGamePerfThreadCommand(pThreadData, GamePerfThreadCommand::Type::TYPE_WRITE, pFile);
    sSubmitGamePerfThreadCommand(pThreadData, GamePerfThreadCommand::Type::TYPE_FLUSH, pFile);
    sSubmitGamePerfThreadCommand(pThreadData, GamePerfThreadCommand::Type::TYPE_CLOSE, pFile);
}

static GamePerfThreadData sGamePerfThreadData;

GamePerfFile::GamePerfFile(const char* pPath) : m_pFile(NULL) {
    m_pFile = sOpenGamePerfThreadFile(&sGamePerfThreadData, pPath);
}

GamePerfFile::~GamePerfFile() {
    if (m_pFile != NULL) {
        sCloseGamePerfThreadFile(&sGamePerfThreadData, m_pFile);
    }
}

void GamePerfFile::Write(const void* pData, const size_t uSize) {
    sWriteGamePerfThreadFile(&sGamePerfThreadData, m_pFile, pData, uSize);
}

void GamePerfFile::Printf(const char* pFormat, ...) {
    static const size_t BUFFER_SIZE = 4096;
    static char sBuffer[BUFFER_SIZE];

    va_list args;
    va_start(args, pFormat);
    int nResult = Blizzard::String::VFormat(sBuffer, BUFFER_SIZE, pFormat, args);
    va_end(args);

    if (nResult > 0 && (size_t)nResult < BUFFER_SIZE) {
        Write(sBuffer, nResult);
    }
}

void GamePerfFile::Flush() {
    sFlushGamePerfThreadFile(&sGamePerfThreadData, m_pFile);
}

BOOL GamePerfFile::IsOpen() {
    return m_pFile != NULL;
}

POS GamePerfCtrAdd(LPCSTR szName, const BOOL fIsCounter, XMemoryAllocatorInterface* ptMemoryInterface);
void GamePerfCtrDelete(POS pos, XMemoryAllocatorInterface* ptMemoryInterface);
void GamePerfCtrStart(const POS pos);
void GamePerfCtrStop(const POS pos);
void GamePerfInitializePerfCtrList(POS* posList, const PerfCtrParams* tPerfCtrList, const uint32 dwNumListEntries,
                                   XMemoryAllocatorInterface* ptMemoryInterface);
void GamePerfDisposePerfCtrList(POS* posList, const uint32 dwNumListEntries,
                                XMemoryAllocatorInterface* ptMemoryInterface);

void GamePerfInitialize() {
    sInitGamePerfThread(&sGamePerfThreadData);

    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;
    tSigmaGlobals.ptGamePerfGlobals = (GamePerfGlobals*)SigmaMemoryNew(sizeof(GamePerfGlobals), 4, NULL, TRUE);
    GamePerfGlobals* ptGamePerfGlobals = tSigmaGlobals.ptGamePerfGlobals;

    ptGamePerfGlobals->tPerfCtrList.Initialize(NULL);
    ptGamePerfGlobals->tPerfCtrStack.Initialize(NULL);

    const int32 MAX_COUNTERS = 100;
    ptGamePerfGlobals->tPerfCtrList.SetBlockCount_Tracked(MAX_COUNTERS);
    ptGamePerfGlobals->tPerfCtrStack.SetBlockCount_Tracked(MAX_COUNTERS);

    PerfCtrParams tPerfCtrList[NUM_INTERNALPERFCTRS] = {
        {INTERNALPERFCTR_FULLFRAME, FALSE, "Full Frame"},
        {INTERNALPERFCTR_UNACCOUNTED, FALSE, "Other"},
    };
    GamePerfInitializePerfCtrList(ptGamePerfGlobals->posPerfCtrInternal, tPerfCtrList, NUM_INTERNALPERFCTRS, NULL);
}

void GamePerfDispose() {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;
    GamePerfGlobals* ptGamePerfGlobals = tSigmaGlobals.ptGamePerfGlobals;

    GamePerfDisposePerfCtrList(ptGamePerfGlobals->posPerfCtrInternal, NUM_INTERNALPERFCTRS, NULL);

    ptGamePerfGlobals->tPerfCtrList.Dispose();
    ptGamePerfGlobals->tPerfCtrStack.Dispose();

    delete tSigmaGlobals.ptGamePerfGlobals->tStats.fileDumpFPS;

    XFREE(tSigmaGlobals.ptGamePerfGlobals, NULL);

    sDisposeGamePerfThread(&sGamePerfThreadData);
}

GamePerfStats& GamePerfStatsGet() {
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;
    return ptGamePerfGlobals->tStats;
}

static void sCollectFrameData(const POS pos, const float flInvPctFrameTime) {
    PerfCtrData& tPerfCtrData = *(PerfCtrData*)pos;
    float flTimePlusChildren = tPerfCtrData.flTime;

    POS posChild = tPerfCtrData.ptChildList->GetHeadPosition();
    while (posChild != NULL) {
        PerfCtrData& tPerfCtrDataChild = *(PerfCtrData*)tPerfCtrData.ptChildList->GetNext(posChild);
        flTimePlusChildren += tPerfCtrDataChild.flTime;
    }

    tPerfCtrData.flPct = flTimePlusChildren * flInvPctFrameTime;
    tPerfCtrData.flCurrentMaxTime = Math::Max(tPerfCtrData.flCurrentMaxTime, flTimePlusChildren);
    tPerfCtrData.flCurrentMaxPct = Math::Max(tPerfCtrData.flCurrentMaxPct, tPerfCtrData.flPct);
    tPerfCtrData.flAvgPctTotal += tPerfCtrData.flPct;
    tPerfCtrData.flAvgTimeTotal += flTimePlusChildren;
    tPerfCtrData.flAvgTimeTotalWithSubs += tPerfCtrData.flTimeWithSubs;
    tPerfCtrData.flCurrentMaxTimeWithSubs = Math::Max(tPerfCtrData.flCurrentMaxTimeWithSubs, tPerfCtrData.flTimeWithSubs);
    tPerfCtrData.flTime = 0.0f;
    tPerfCtrData.flTimeWithSubs = 0.0f;
}

void GamePerfNewFrame() {
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;

    GamePerfCtrStop(ptGamePerfGlobals->posPerfCtrInternal[INTERNALPERFCTR_UNACCOUNTED]);
    GamePerfCtrStop(ptGamePerfGlobals->posPerfCtrInternal[INTERNALPERFCTR_FULLFRAME]);

    GamePerfStats& tPerfStats = GamePerfStatsGet();

    PerfCtrData& tFullFrame = ptGamePerfGlobals->tPerfCtrList.GetAt(ptGamePerfGlobals->posPerfCtrInternal[INTERNALPERFCTR_FULLFRAME]);

    if (tFullFrame.flTime > 0.000001f) {
        if (tPerfStats.fCollectPerfCountersThisFrame) {
            float flInvPctFrameTime = 1.0f / tFullFrame.flTime;

            tPerfStats.flInstantaneousFPS = flInvPctFrameTime;
            if (tPerfStats.nFrameSampleCount == 0) {
                tPerfStats.flCurrentLowestFPS = tPerfStats.flInstantaneousFPS;
                tPerfStats.flCurrentHighestFPS = tPerfStats.flInstantaneousFPS;
            } else {
                if (tPerfStats.flInstantaneousFPS < tPerfStats.flCurrentLowestFPS) {
                    tPerfStats.flCurrentLowestFPS = tPerfStats.flInstantaneousFPS;
                }
                if (tPerfStats.flInstantaneousFPS > tPerfStats.flCurrentHighestFPS) {
                    tPerfStats.flCurrentHighestFPS = tPerfStats.flInstantaneousFPS;
                }
            }

            flInvPctFrameTime *= 100.0f;

            POS pos = ptGamePerfGlobals->tPerfCtrList.GetHeadPosition();
            while (pos != NULL) {
                sCollectFrameData(pos, flInvPctFrameTime);
                ptGamePerfGlobals->tPerfCtrList.GetNext(pos);
            }
        } else {
            tFullFrame.flAvgTimeTotal += tFullFrame.flTime;
            tFullFrame.flAvgTimeTotalWithSubs += tFullFrame.flTimeWithSubs;
            tFullFrame.flTime = 0.0f;
        }

        tPerfStats.nFrameSampleCount++;
    }

    tPerfStats.dwFrameNum++;
    tPerfStats.fCollectPerfCountersThisFrame = tPerfStats.fCollectPerfCounters;

    GamePerfCtrStart(ptGamePerfGlobals->posPerfCtrInternal[INTERNALPERFCTR_FULLFRAME]);
    GamePerfCtrStart(ptGamePerfGlobals->posPerfCtrInternal[INTERNALPERFCTR_UNACCOUNTED]);
}

// On its DWARF lines 570-632 (original numbering); the per-counter statement order comes from the rows
// (590 flAvgPct, 594 flMaxPct, 597 flAvgTime, 600 flAvgTimeWithSubs, 604 flMaxTime, 607 flMaxTimeWithSubs,
// each followed by the zeroing of its source; tab columns 55/31/26/27/35/42/34/35/42 fit).
#line 570
void GamePerfCollectAverages()

{
    GamePerfStats& tPerfStats = GamePerfStatsGet();
    if (tPerfStats.nFrameSampleCount <= 0)
    {
        return;
    }

    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;
    PerfCtrData& tFullFrame = ptGamePerfGlobals->tPerfCtrList.GetAt(ptGamePerfGlobals->posPerfCtrInternal[INTERNALPERFCTR_FULLFRAME]);
    float flInvSampleCount = 1.0f / (float)tPerfStats.nFrameSampleCount;

    if (tPerfStats.fCollectPerfCountersThisFrame)
    {
        POS pos = ptGamePerfGlobals->tPerfCtrList.GetHeadPosition();
        while (pos != NULL)
        {
            PerfCtrData& tPerfCtrData = ptGamePerfGlobals->tPerfCtrList.GetNext(pos);

            tPerfCtrData.flAvgPct = tPerfCtrData.flAvgPctTotal * flInvSampleCount;
            tPerfCtrData.flAvgPctTotal = 0.0f;


            tPerfCtrData.flMaxPct = tPerfCtrData.flCurrentMaxPct;
            tPerfCtrData.flCurrentMaxPct = 0.0f;

            tPerfCtrData.flAvgTime = tPerfCtrData.flAvgTimeTotal * flInvSampleCount;
            tPerfCtrData.flAvgTimeTotal = 0.0f;

            tPerfCtrData.flAvgTimeWithSubs = tPerfCtrData.flAvgTimeTotalWithSubs * flInvSampleCount;
            tPerfCtrData.flAvgTimeTotalWithSubs = 0.0f;


            tPerfCtrData.flMaxTime = tPerfCtrData.flCurrentMaxTime;
            tPerfCtrData.flCurrentMaxTime = 0.0f;

            tPerfCtrData.flMaxTimeWithSubs = tPerfCtrData.flCurrentMaxTimeWithSubs;
            tPerfCtrData.flCurrentMaxTimeWithSubs = 0.0f;
        }
    }
    else
    {
        tFullFrame.flAvgTime = tFullFrame.flAvgTimeTotal * flInvSampleCount;
        tFullFrame.flAvgTimeTotal = 0.0f;

        tFullFrame.flAvgTimeWithSubs = tFullFrame.flAvgTimeTotalWithSubs * flInvSampleCount;
        tFullFrame.flAvgTimeTotalWithSubs = 0.0f;
    }

    tPerfStats.flAvgFPS = 1.0f / tFullFrame.flAvgTime;


    if (tPerfStats.flAvgFPS < tPerfStats.flMinAvgFPS && !tPerfStats.bFPSCollectionStart)
    {
        tPerfStats.flMinAvgFPS = tPerfStats.flAvgFPS;
    }
    tPerfStats.nFrameSampleCount = 0;
    tPerfStats.flRecentLowestFPS = tPerfStats.flCurrentLowestFPS;
    tPerfStats.flRecentHighestFPS = tPerfStats.flCurrentHighestFPS;


}
#line 466

POS GamePerfCtrAdd(LPCSTR szName, const BOOL fIsCounter, XMemoryAllocatorInterface* ptMemoryInterface) {
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;

    PerfCtrData tPerfCtrData;
    SigmaMemoryClear(&tPerfCtrData, sizeof(tPerfCtrData));

    Blizzard::String::Copy(tPerfCtrData.szName, szName, sizeof(tPerfCtrData.szName));
    tPerfCtrData.fIsCounter = fIsCounter;
    tPerfCtrData.ptChildList = new (SigmaMemoryNew(sizeof(ChildList), 4, ptMemoryInterface, FALSE)) ChildList;
    tPerfCtrData.dwParentRef = 0;
    tPerfCtrData.posMyParent = NULL;

    return ptGamePerfGlobals->tPerfCtrList.AddTail_Tracked(tPerfCtrData);
}

// NON_MATCHING: size 1420 != 1424. Same code up to block placement in the final
// tPerfCtrList.RemoveAt(pos): the original moves the "node has no next" half of the inlined
// XBaseList<>::_RemoveFromList() to the end of the function (one more branch); ours keeps it in line. Relaid on DWARF lines 795-861 (o02; posStack/posStackTail declared at 841/842): unchanged.
#line 795
void GamePerfCtrDelete(
    POS pos,
    XMemoryAllocatorInterface* ptMemoryInterface)
{
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;
    PerfCtrData& tPerfCtrData = *(PerfCtrData*)pos;


    if (tPerfCtrData.dwParentRef != 0)
    {
        POS posChild = ptGamePerfGlobals->tPerfCtrList.GetHeadPosition();
        while (posChild != NULL)
        {
            PerfCtrData& tPerfCtrChild = ptGamePerfGlobals->tPerfCtrList.GetNext(posChild);
            if (tPerfCtrChild.posMyParent == pos)
            {
                tPerfCtrChild.posMyParent = tPerfCtrData.posMyParent;
                if (tPerfCtrData.posMyParent != NULL)
                {
                    PerfCtrData& tPerfCtrParentData = *(PerfCtrData*)tPerfCtrData.posMyParent;
                    tPerfCtrParentData.dwParentRef++;
                }
            }
        }
    }

    if (tPerfCtrData.posMyParent != NULL)
    {
        PerfCtrData& tPerfCtrParentData = *(PerfCtrData*)tPerfCtrData.posMyParent;
        tPerfCtrParentData.dwParentRef--;


        POS posChildList = tPerfCtrParentData.ptChildList->GetHeadPosition();
        while (posChildList != NULL)
        {
            POS posOld = posChildList;
            POS posChild = tPerfCtrParentData.ptChildList->GetNext(posChildList);
            if (posChild == pos)
            {
                tPerfCtrParentData.ptChildList->RemoveAt(posOld);
            }
        }
    }

    tPerfCtrData.ptChildList->~ChildList(); SigmaMemoryFree(tPerfCtrData.ptChildList, ptMemoryInterface);  // one row (839 col 2): a single statement or macro in the original

    POS posStack = ptGamePerfGlobals->tPerfCtrStack.Find(&tPerfCtrData);
    POS posStackTail = ptGamePerfGlobals->tPerfCtrStack.GetTailPosition();

    if (posStack == posStackTail)
    {

        if (!tPerfCtrData.fIsCounter)
        {
            GamePerfCtrStop(pos);
        }
    }
    else if (posStack != NULL)
    {

        ptGamePerfGlobals->tPerfCtrStack.RemoveAt(posStack);
    }

    ptGamePerfGlobals->tPerfCtrList.RemoveAt(pos);


}
#line 535

static void sGamePerfCtrStartInternal(PerfCtrData& tPerfCtrData, BOOL bStartLast);

static void sGamePerfCtrStopInternal(PerfCtrData& tPerfCtrData, BOOL bStopCurrent) {
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;

    if (ptGamePerfGlobals->tPerfCtrStack.Count() == 0) {
        return;
    }

    tPerfCtrData.flTime += tPerfCtrData.tStopwatch.Check();

    if (bStopCurrent) {
        tPerfCtrData.flTimeWithSubs += tPerfCtrData.tStopwatchWithSubs.Check();

        ptGamePerfGlobals->tPerfCtrStack.RemoveTail();

        if (ptGamePerfGlobals->tPerfCtrStack.Count() > 1) {
            PerfCtrData* ptPerfCtrDataLast = ptGamePerfGlobals->tPerfCtrStack.GetTail();
            if (ptPerfCtrDataLast != &tPerfCtrData) {
                sGamePerfCtrStartInternal(*ptPerfCtrDataLast, TRUE);
            }
        }
    }
}

static void sGamePerfCtrStartInternal(PerfCtrData& tPerfCtrData, BOOL bStartLast) {
    tPerfCtrData.tStopwatch.Reset();

    if (!bStartLast) {
        tPerfCtrData.tStopwatchWithSubs.Reset();

        GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;

        if (ptGamePerfGlobals->tPerfCtrStack.Count() > 1) {
            PerfCtrData* ptPerfCtrDataLast = ptGamePerfGlobals->tPerfCtrStack.GetTail();
            sGamePerfCtrStopInternal(*ptPerfCtrDataLast, FALSE);
        }

        ptGamePerfGlobals->tPerfCtrStack.AddTail_Tracked(&tPerfCtrData);
    }
}

void GamePerfCtrStart(const POS pos) {
    GamePerfStats& tPerfStats = GamePerfStatsGet();
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;
    (void)ptGamePerfGlobals;

    if (pos == NULL || !tPerfStats.fCollectPerfCountersThisFrame) {
        return;
    }

    PerfCtrData& tPerfCtrData = *(PerfCtrData*)pos;
    sGamePerfCtrStartInternal(tPerfCtrData, FALSE);
}

void GamePerfCtrStop(const POS pos) {
    GamePerfStats& tPerfStats = GamePerfStatsGet();
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;
    (void)ptGamePerfGlobals;

    if (pos == NULL || !tPerfStats.fCollectPerfCountersThisFrame) {
        return;
    }

    PerfCtrData& tPerfCtrData = *(PerfCtrData*)pos;
    sGamePerfCtrStopInternal(tPerfCtrData, TRUE);
}

void GamePerfCtrIncrement(const POS pos, const int32 nAmount) {
    GamePerfStats& tPerfStats = GamePerfStatsGet();
    GamePerfGlobals* ptGamePerfGlobals = gtSigmaGlobals.ptGamePerfGlobals;
    (void)ptGamePerfGlobals;

    if (pos == NULL || !tPerfStats.fCollectPerfCountersThisFrame) {
        return;
    }

    PerfCtrData& tPerfCtrData = *(PerfCtrData*)pos;
    tPerfCtrData.flTime += (float)nAmount;
}

inline void GamePerfInitializePerfCtrList(POS* posList, const PerfCtrParams* tPerfCtrList, const uint32 dwNumListEntries,
                                          XMemoryAllocatorInterface* ptMemoryInterface) {
    for (uint32 i = 0; i < dwNumListEntries; i++) {
        posList[tPerfCtrList[i].nEnumIndex] =
            GamePerfCtrAdd(tPerfCtrList[i].szName, tPerfCtrList[i].fIsCounter, ptMemoryInterface);
    }
}

inline void GamePerfDisposePerfCtrList(POS* posList, const uint32 dwNumListEntries,
                                       XMemoryAllocatorInterface* ptMemoryInterface) {
    for (uint32 i = 0; i < dwNumListEntries; i++) {
        GamePerfCtrDelete(posList[i], ptMemoryInterface);
    }
}
