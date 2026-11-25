// Original file: X/Common/Sno/SNOFilesAsync.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).
#include "../ThreadGlobals.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Console/IO/ConsoleIO.h"
#include "../Console/IO/ConsoleIO_ArchiveReader.h"
#include "../SNO/SNOFilesAsync.h"
#include "../SNO/SNOFiles.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XTemplate.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../FileIO/FileIO.h"
// BlizzardCore/Include/Blizzard/Process.h (not adopted; Game.cpp declares it the same way).
namespace Blizzard { namespace Process { bool Sleep(Blizzard::Time::Millisecond milliseconds); } }
#include "../Definitions/Definitions.h"
// X/Common/Shell/MainNX64.cpp.
void ConfigureSNOFilesAsyncThread(Blizzard::Thread::Thread thread);
#include "../../Sigma/SigmaTiming.h"
// X/Common/Sno/SNOFiles.cpp.
AsyncRecordRef SNOFileGetAsyncRecord(SNOElement * ptSNOElement);
void SNOFileAssignAsyncRecord(SNOElement * ptSNOElement, AsyncRecordRef pos);
uint64 SNOFileGetFileHash(SNOElement * ptSNOElement, BOOL fWithExtension);
#include <limits.h>
// X/Common/SNO/SNOFiles.h:254 (inlined into 5 functions; not in the adopted header yet).
inline SNOName SNOEntry::ToSNOName() const {
    return SNOName(snoGroup, uGuid);
}

// X/Common/Sno/SNOFiles.cpp.
SNOFileStatResults SNOFileAsyncGenerateStats(SNOElement * ptSNOElement, uint64 dwHash, void * ptArchive, void * ptFileInfo, SNOFileAsyncStats & tStats);  // SNOFiles.cpp:4423
void SNOFileClose(SNOElement * ptSNOElement);  // SNOFiles.cpp:5014
// X/Sigma/Files/XFilesCore.cpp:1146.
BOOL FileRead(FileReference & tFileRef, void * lpBuf, const uint32 dwOffset, const uint32 dwSize, const ErrorCode eErrorCodeIfFails);
// X/Common/Console/IO/SFileWrapper.cpp:155 and :565.
Blizzard::Mopaq::ErrorCode SFileGetLastError();
bool SFileOverrideLocation(HSFILERecord * pFile, int64 nLocationOverride);
// X/Common/Console/IO/ConsoleIO.cpp: SNO group being opened, -1 otherwise.
extern int32 unk_71010aba28;

#include "../../Sigma/UnicodeText.h"
























struct AsyncRecord
{
    int64 nLocation;
    int32 nSize;
    AsyncStatus status;
    bool inUse : 1;
    bool reapable : 1;
    SNOElement *snoElement;
    SNOFilesAsyncPriority ePriority;
    POS posPriorityBoundary;
    Console::IO::CPKArchiveReader *ptArchive;

    struct LocationAndScore
    {
        int64 nLocation;
        int32 nFirstReadSector;
        int32 nLastReadSector;
        int32 nScorePrev;
        int32 nScoreNext;
    };
    Console::IO::FileInfo info;
    int32 nLocationCount;
    LocationAndScore *ptLocationsAndScore;
    int32 nFirstReadSector;
    int32 nLastReadSector;
    int32 nScorePrev;
    int32 nScoreNext;
    uint64 dwHash;

    AsyncRecord()
    {
        ptLocationsAndScore = NULL;
        nLocationCount = 0;
        nLocation = 0;
        nSize = 0;
        status = e_successful;
        inUse = false;
        reapable = false;
        snoElement = NULL;
        ePriority = SNOFilesAsyncPriority_Medium;
        posPriorityBoundary = NULL;
        ptArchive = NULL;
        SigmaMemoryClear(&info, sizeof(info));
        nFirstReadSector = -1;
        nLastReadSector = -1;
        nScorePrev = 0;
        nScoreNext = 0;
        dwHash = 0;
    }

    AsyncRecord(const AsyncRecord &record)
    {

        ptLocationsAndScore = NULL;
        nLocationCount = 0;
        *this = record;
    }

    AsyncRecord &operator=(const AsyncRecord &record)
    {

        if (ptLocationsAndScore)
        {
            delete[] ptLocationsAndScore;
            ptLocationsAndScore = NULL;
        }

        status = record.status;
        inUse = record.inUse;
        reapable = record.reapable;
        snoElement = record.snoElement;
        ptArchive = record.ptArchive;
        info = record.info;
        nLocationCount = record.nLocationCount;
        nLocation = record.nLocation;
        nSize = record.nSize;
        nFirstReadSector = record.nFirstReadSector;
        nLastReadSector = record.nLastReadSector;
        nScorePrev = record.nScorePrev;
        nScoreNext = record.nScoreNext;
        dwHash = record.dwHash;
        ePriority = record.ePriority;
        posPriorityBoundary = record.posPriorityBoundary;
        if (record.nLocationCount > 1)
        {
            ptLocationsAndScore = new LocationAndScore[record.nLocationCount];
            for (int32 i = 0; i < record.nLocationCount; i++)
            {
                ptLocationsAndScore[i] = record.ptLocationsAndScore[i];
            }
        }

        return *this;
    }

    ~AsyncRecord()
    {
        delete[] ptLocationsAndScore;
    }
};




static uint32 SNOFilesAsyncThread(void *);  // X/Common/Sno/SNOFilesAsync.cpp:1901

struct RecordBoundary
{
    POS firstRecord;
    POS lastRecord;
};

struct PriorityBoundaryInfo
{
    RecordBoundary boundary;
    SNOFilesAsyncPriority ePriority;
};

typedef XGrowableMap<Console::IO::CPKArchiveReader *, Console::IO::CPKArchiveReader *, RecordBoundary, const RecordBoundary &> ArchiveBoundaryMap;
typedef XGrowableList<PriorityBoundaryInfo, const PriorityBoundaryInfo &> PriorityBoundaryList;

struct SNOFilesAsyncGlobals
{
    SNOFilesAsyncGlobals()
        : m_asyncThread(NULL),
          m_inPlaceLocking(false),
          m_ptLastArchive(NULL) {
        m_nLastLocation = -1;
        m_pNextRecordPos = NULL;
        m_nMaxCompletedSNO = 32;
        m_recordListDirty = false;
        m_ptLastInsertedRecord = NULL;
        m_recordListForceSort = false;
        m_fSortedRequestDisabled = false;
        m_terminateAsyncThread = false;
        m_paused = false;
        m_hasNewQuerySinceKick = false;
        m_tLockingSNO.snoGroup = SNOGROUP_NONE; m_tLockingSNO.snoHandle = SNO_NONE;

        m_pendingRecordList.SetBlockCount_Tracked(1024);






        m_pendingRecordList.SetGrow(XALLOC_GROW_LINEAR, 512);
        m_pendingRecordList.SetShrinkable(TRUE);
        m_tLastReadAheadSuggestion.ptLastArchive = NULL;
        m_tLastReadAheadSuggestion.nLastSector = -1;
        m_tLastReadAheadSuggestion.ptNextArchive = NULL;
        m_tLastReadAheadSuggestion.nNextSector = -1;
    }





    ~SNOFilesAsyncGlobals() {


        m_terminateAsyncThread = true;
        m_theresWorkToDoEvent.Set();
        Blizzard::Thread::WaitForever(m_asyncThread);
        Blizzard::Thread::Close(m_asyncThread);
    }

    void StartAsyncThread() {
        Blizzard::Thread::Create(SNOFilesAsyncThread, NULL, &m_asyncThread, "SNOFilesAsyncThread", 0, 0x10000);
        ConfigureSNOFilesAsyncThread(m_asyncThread);
    }












    Blizzard::Thread::Thread m_asyncThread;
    Blizzard::Lock::RMutex m_recordListMutex;
    Blizzard::Lock::RMutex m_completedRecordListMutex;
    Blizzard::Lock::REvent m_theresWorkToDoEvent;
    Blizzard::Lock::REvent m_someWorkCompletedEvent;




    XGrowableList<AsyncRecord, const AsyncRecord &> m_pendingRecordList;
    XGrowableList<AsyncRecord, const AsyncRecord &> m_completedRecordList[SNOFilesAsyncPriority_Count];
    volatile bool m_inPlaceLocking;
    ArchiveBoundaryMap m_archiveBoundaryMap;
    PriorityBoundaryList m_priorityBoundaryLists[SNOFilesAsyncPriority_Count];
    Console::IO::CPKArchiveReader *m_ptLastArchive;
    int64 m_nLastLocation;
    POS m_pNextRecordPos;
    int32 m_nMaxCompletedSNO;
    bool m_recordListDirty;

    POS m_ptLastInsertedRecord;
    bool m_recordListForceSort;
    bool m_fSortedRequestDisabled;
    Console::IO::ReadAheadSuggestion m_tLastReadAheadSuggestion;

    bool m_terminateAsyncThread;
    bool m_paused;
    bool m_hasNewQuerySinceKick;
    SNOName m_tLockingSNO;
};

static SNOFilesAsyncGlobals * s_globals;


void SNOFilesAsyncInitialize() {




    s_globals = new SNOFilesAsyncGlobals;
    s_globals->StartAsyncThread();
}


void SNOFilesAsyncDispose() {
    delete s_globals;
    s_globals = NULL;
}





































static void sResetPriorityLocation(const PriorityBoundaryInfo & info, POS priorityPos) {
    AsyncRecordRef checkRecordPos;
    AsyncRecordRef recordPos = info.boundary.firstRecord;
    do {
        checkRecordPos = recordPos;
        AsyncRecord * checkRecord = &s_globals->m_pendingRecordList.GetNext(recordPos);
        checkRecord->posPriorityBoundary = priorityPos;
    } while (checkRecordPos != info.boundary.lastRecord);
}









static void sInsertBestPriorityLocation(const PriorityBoundaryInfo & info) {
    AsyncRecord * pFirstRecord;
    PriorityBoundaryList & tList = s_globals->m_priorityBoundaryLists[info.ePriority];
    pFirstRecord = &s_globals->m_pendingRecordList.GetAt(info.boundary.firstRecord);
    POS pos;
    BOOL fSameArchiveFound;
    POS priorityPos;

    pos = tList.GetTailPosition();
    if (pos) {
        PriorityBoundaryInfo & checkInfo = tList.GetAt(pos);
        AsyncRecord * pCheckLastRecord = &s_globals->m_pendingRecordList.GetAt(checkInfo.boundary.lastRecord);
        if (pCheckLastRecord->ptArchive == pFirstRecord->ptArchive && pCheckLastRecord->nLocation < pFirstRecord->nLocation) {
            POS priorityPos = tList.AddTail_Tracked(info);
            sResetPriorityLocation(info, priorityPos);
            return;
        }
    }

    fSameArchiveFound = FALSE;
    if (!s_globals->m_fSortedRequestDisabled) {
        pos = tList.GetHeadPosition();
        while (pos) {
            POS checkPos = pos;
            PriorityBoundaryInfo & checkInfo = tList.GetNext(pos);
            AsyncRecord * pCheckFirstRecord = &s_globals->m_pendingRecordList.GetAt(checkInfo.boundary.firstRecord);
            if (pCheckFirstRecord->ptArchive == pFirstRecord->ptArchive) {
                fSameArchiveFound = TRUE;
                if (pCheckFirstRecord->nLocation > pFirstRecord->nLocation) {
                    POS priorityPos = tList.InsertBefore_Tracked(checkPos, info);
                    sResetPriorityLocation(info, priorityPos);
                    return;
                }
            } else if (fSameArchiveFound) {
                POS priorityPos = tList.InsertBefore_Tracked(checkPos, info);
                sResetPriorityLocation(info, priorityPos);
                return;
            }
        }
    }

    priorityPos = tList.AddTail_Tracked(info);
    sResetPriorityLocation(info, priorityPos);
}












static void sRemoveFromPriorityList(
    POS pos)
{


    // The original reaches the record and its PriorityBoundaryInfo without an inlined accessor (DWARF 414-415).
    AsyncRecord *ptRecord = (AsyncRecord *)pos;
    PriorityBoundaryInfo &info = *(PriorityBoundaryInfo *)ptRecord->posPriorityBoundary;

    if (info.boundary.firstRecord == pos)
    {
        if (info.boundary.lastRecord == pos)
        {
            s_globals->m_priorityBoundaryLists[info.ePriority].RemoveAt(ptRecord->posPriorityBoundary);
        }
        else
        {
            s_globals->m_pendingRecordList.GetNext(info.boundary.firstRecord);
        }
    }
    else if (info.boundary.lastRecord == pos)
    {
        s_globals->m_pendingRecordList.GetPrev(info.boundary.lastRecord);
    }
}

static void sInsertInPriorityList(
    POS pos,
    const AsyncRecord & record,
    AsyncRecord * ptPreviousRecord,
    AsyncRecord * ptNextRecord)
{
    // DWARF uses direct record/boundary pointer conversions here; no list accessor is inlined at these accesses.
    if (ptPreviousRecord && ptNextRecord && ptPreviousRecord->posPriorityBoundary == ptNextRecord->posPriorityBoundary)
    {
        PriorityBoundaryInfo & info = *(PriorityBoundaryInfo *)ptPreviousRecord->posPriorityBoundary;
        // Inference: the line-zero shared tail at +0x134 upgrades the boundary when the new priority is higher.
        if (info.ePriority < record.ePriority)
        {
            SNOFilesAsyncPriority eOldPriority = info.ePriority;
            POS posOldBoundary = ptPreviousRecord->posPriorityBoundary;
            info.ePriority = record.ePriority;
            sInsertBestPriorityLocation(info);
            s_globals->m_priorityBoundaryLists[eOldPriority].RemoveAt(posOldBoundary);
        }
        else
        {


            AsyncRecord * pRecord = (AsyncRecord *)pos;
            pRecord->posPriorityBoundary = ptPreviousRecord->posPriorityBoundary;
        }
    }

    else if (ptPreviousRecord && ptPreviousRecord->nLocation + ptPreviousRecord->nSize == record.nLocation)
    {

        if (ptNextRecord && record.nLocation + record.nSize == ptNextRecord->nLocation)
        {

            PriorityBoundaryInfo & info1 = *(PriorityBoundaryInfo *)ptPreviousRecord->posPriorityBoundary;
            PriorityBoundaryInfo & info2 = *(PriorityBoundaryInfo *)ptNextRecord->posPriorityBoundary;


            if (ptPreviousRecord->posPriorityBoundary != ptNextRecord->posPriorityBoundary)
            {
                info1.boundary.lastRecord = info2.boundary.lastRecord;


                if (info1.ePriority < record.ePriority)
                {
                    SNOFilesAsyncPriority eOldPriority1 = info1.ePriority;
                    POS posOldBoundary1 = ptPreviousRecord->posPriorityBoundary;
                    SNOFilesAsyncPriority eOldPriority2 = info2.ePriority;
                    POS posOldBoundary2 = ptNextRecord->posPriorityBoundary;

                    info1.ePriority = record.ePriority;
                    sInsertBestPriorityLocation(info1);

                    s_globals->m_priorityBoundaryLists[eOldPriority1].RemoveAt(posOldBoundary1);
                    s_globals->m_priorityBoundaryLists[eOldPriority2].RemoveAt(posOldBoundary2);
                }
                else
                {
                    SNOFilesAsyncPriority eOldPriority2 = info2.ePriority;
                    POS posOldBoundary2 = ptNextRecord->posPriorityBoundary;

                    AsyncRecord * pRecord = (AsyncRecord *)pos;
                    pRecord->posPriorityBoundary = ptPreviousRecord->posPriorityBoundary;
                    sResetPriorityLocation(info2, ptPreviousRecord->posPriorityBoundary);

                    s_globals->m_priorityBoundaryLists[eOldPriority2].RemoveAt(posOldBoundary2);
                }
            }
            else { // Inference: this equal-boundary path shares the same priority-upgrade tail at +0x134.
                if (info1.ePriority < record.ePriority)
                {

                    SNOFilesAsyncPriority eOldPriority1 = info1.ePriority;
                    POS posOldBoundary1 = ptPreviousRecord->posPriorityBoundary;
                    info1.ePriority = record.ePriority;
                    sInsertBestPriorityLocation(info1);
                    s_globals->m_priorityBoundaryLists[eOldPriority1].RemoveAt(posOldBoundary1);
                }
                else
                {


                    AsyncRecord * pRecord = (AsyncRecord *)pos;
                    pRecord->posPriorityBoundary = ptPreviousRecord->posPriorityBoundary;
                }
            }
        }
        else
        {

            PriorityBoundaryInfo & info = *(PriorityBoundaryInfo *)ptPreviousRecord->posPriorityBoundary;
            AsyncRecord * ptLastRecord = ptPreviousRecord;
            if (info.boundary.lastRecord == (POS)ptLastRecord)
            {
                info.boundary.lastRecord = pos;
            }


            if (info.ePriority < record.ePriority)
            {
                SNOFilesAsyncPriority eOldPriority = info.ePriority;
                POS posOldBoundary = ptLastRecord->posPriorityBoundary;
                info.ePriority = record.ePriority;
                sInsertBestPriorityLocation(info);
                s_globals->m_priorityBoundaryLists[eOldPriority].RemoveAt(posOldBoundary);
            }
            else
            {


                AsyncRecord * pRecord = (AsyncRecord *)pos;
                pRecord->posPriorityBoundary = ptLastRecord->posPriorityBoundary;
            }
        }
    }
    else if (ptNextRecord && record.nLocation + record.nSize == ptNextRecord->nLocation)
    {

        PriorityBoundaryInfo & info = *(PriorityBoundaryInfo *)ptNextRecord->posPriorityBoundary;
        AsyncRecord * ptFirstRecord = ptNextRecord;
        if (info.boundary.firstRecord == (POS)ptFirstRecord)
        {
            info.boundary.firstRecord = pos;
        }


        if (info.ePriority < record.ePriority)
        {
            SNOFilesAsyncPriority eOldPriority = info.ePriority;
            POS posOldBoundary = ptFirstRecord->posPriorityBoundary;
            info.ePriority = record.ePriority;
            sInsertBestPriorityLocation(info);
            s_globals->m_priorityBoundaryLists[eOldPriority].RemoveAt(posOldBoundary);
        }
        else
        {


            AsyncRecord * pRecord = (AsyncRecord *)pos;
            pRecord->posPriorityBoundary = ptFirstRecord->posPriorityBoundary;
        }
    }
    else
    {

        PriorityBoundaryInfo info;
        info.ePriority = record.ePriority;
        info.boundary.firstRecord = pos;
        info.boundary.lastRecord = pos;
        sInsertBestPriorityLocation(info);
    }


}

// NON_MATCHING: 1376 vs 1268 bytes; faithful location insertion, but priority insertion inlines at two additional sites and InsertAfter_Tracked stays out of line. Restored DWARF declaration scopes; no codegen-forcing variants tried.
static AsyncRecordRef sInsertBestLocation(
    const AsyncRecord & record)
{
    XGrowableList<AsyncRecord, const AsyncRecord &> & recordList = s_globals->m_pendingRecordList;


    RecordBoundary * ptInfo = s_globals->m_archiveBoundaryMap.LookupPtr(record.ptArchive);
    if (!ptInfo)
    {
        POS pos = recordList.AddTail_Tracked(record);

        sInsertInPriorityList(pos, record, NULL, NULL);

        RecordBoundary tInfo;
        tInfo.firstRecord = pos;
        tInfo.lastRecord = pos;
        s_globals->m_archiveBoundaryMap.SetAt_Tracked(record.ptArchive, tInfo);
        return pos;
    }


    RecordBoundary & tInfo = *ptInfo;

    AsyncRecord * ptFirstRecord1 = (AsyncRecord *)ptInfo->firstRecord;
    AsyncRecord * ptLastRecord1 = (AsyncRecord *)ptInfo->lastRecord;



    if (!s_globals->m_fSortedRequestDisabled && ptFirstRecord1->nLocation < record.nLocation)
    {




        if (ptLastRecord1->nLocation < record.nLocation)
        {

            POS pos = recordList.InsertAfter_Tracked(tInfo.lastRecord, record);

            sInsertInPriorityList(pos, record, ptLastRecord1, NULL);

            tInfo.lastRecord = pos;
            return pos;
        }
        else
        {
            // Inference: no predecessor exists before walking the first archive record; original initializes x23 to zero.
            AsyncRecord * previousRecord = NULL;
            for (AsyncRecordRef recordPos = tInfo.firstRecord; recordPos != tInfo.lastRecord;)
            {
                AsyncRecordRef checkRecordPos = recordPos;
                AsyncRecord * checkRecord = &recordList.GetNext(recordPos);
                if (checkRecord->nLocation > record.nLocation)
                {
                    POS pos = recordList.InsertBefore_Tracked(checkRecordPos, record);

                    sInsertInPriorityList(pos, record, previousRecord, checkRecord);
                    return pos;
                } // Inference: line-zero loop update retains the current record as the next predecessor.
                previousRecord = checkRecord;
            }


            POS pos = recordList.InsertBefore_Tracked(tInfo.lastRecord, record);

            sInsertInPriorityList(pos, record, previousRecord, ptLastRecord1);
            return pos;
        }
    }
    else
    {


        POS pos = recordList.InsertBefore_Tracked(tInfo.firstRecord, record);

        sInsertInPriorityList(pos, record, NULL, ptFirstRecord1);

        tInfo.firstRecord = pos;
        return pos;
    }
}


static void sSNOFilesAsyncRead(SNOElement * ptSnoElement, SNOFilesAsyncPriority ePriority) {

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    if (SNOFileGetAsyncRecord(ptSnoElement)) {
        return;
    }

    AsyncRecord record;
    record.snoElement = ptSnoElement;
    record.ePriority = ePriority;
    uint64 dwHash = SNOFileGetFileHash(ptSnoElement, TRUE);
    Console::IO::FileInfo info;
    unk_71010aba28 = ptSnoElement->tEntry.snoGroup;
    Console::IO::CPKArchiveReader * ptArchive = Console::IO::FindArchiveContaining(dwHash, &info);
    unk_71010aba28 = -1;
    if (!ptArchive) {
        if (!(ptSnoElement->dwFlags & SNOENTRY_FLAG_NOT_IN_CPK)) {
            Blizzard::Lock::Atomic::Or(SNOENTRY_FLAG_NOT_IN_CPK, &ptSnoElement->dwFlags);
        }
        return;
    }

    ptArchive->AddFileReference();
    record.ptArchive = ptArchive;
    record.info = info;
    record.dwHash = dwHash;
    record.nSize = info.nSize;
    record.nLocationCount = info.nLocationCount;
    if (info.nLocationCount == 1) {
        record.nLocation = ptArchive->GetLocation(info.nLocationIndex);
        record.nFirstReadSector = -1;
        record.nLastReadSector = -1;
    } else {
        record.ptLocationsAndScore = new AsyncRecord::LocationAndScore[info.nLocationCount];
        for (int32 i = 0; i < info.nLocationCount; ++i) {
            record.ptLocationsAndScore[i].nLocation = ptArchive->GetLocation(info.nLocationIndex + i);
            record.ptLocationsAndScore[i].nFirstReadSector = -1;
            record.ptLocationsAndScore[i].nLastReadSector = -1;
            record.ptLocationsAndScore[i].nScorePrev = 0;
            record.ptLocationsAndScore[i].nScoreNext = 0;
        }
        record.nLocation = -1;
    }
    record.inUse = false;
    record.reapable = false;
    record.status = e_pending;

    AsyncRecordRef result = sInsertBestLocation(record);
    s_globals->m_pNextRecordPos = NULL;
    if (record.nLocationCount != 1) {
        s_globals->m_recordListDirty = true;
    }
    SNOFileAssignAsyncRecord(record.snoElement, result);
    s_globals->m_hasNewQuerySinceKick = true;
}











































































































void SNOFilesAsyncKick() {

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    if (s_globals->m_hasNewQuerySinceKick) {

        s_globals->m_theresWorkToDoEvent.Set();
        s_globals->m_hasNewQuerySinceKick = false;
    }

}

void SNOFilesAsyncPause() {

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    s_globals->m_paused = true;

}

void SNOFilesAsyncUnpause() {

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    s_globals->m_paused = false;
    if (s_globals->m_pendingRecordList.Count() > 0) {

        s_globals->m_theresWorkToDoEvent.Set();
    }

}
// NON_MATCHING: 556 vs 560 bytes; sSNOFilesAsyncRead is inlined with a slightly different tail (sInsertBestLocation is now out of line).
void SNOFilesAsyncRead(
    SNOElement * ptSNOElement,
    SNOFilesAsyncPriority ePriority)
{
    sSNOFilesAsyncRead(ptSNOElement, ePriority);
}


void SNOFilesAsyncChangePriority(SNOElement * ptSNOElement, SNOFilesAsyncPriority ePriority) {
    AsyncRecord * ptRecord;

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    if (SNOFileGetAsyncRecord(ptSNOElement)) {
        ptRecord = &s_globals->m_pendingRecordList.GetAt(SNOFileGetAsyncRecord(ptSNOElement));
        PriorityBoundaryInfo & info = s_globals->m_priorityBoundaryLists[ptRecord->ePriority].GetAt(ptRecord->posPriorityBoundary);
        ptRecord->ePriority = ePriority;
        if (ptRecord->status == e_pending && info.ePriority < ePriority) {
            SNOFilesAsyncPriority eOldPriority = info.ePriority;
            POS posOldBoundary = ptRecord->posPriorityBoundary;
            info.ePriority = ePriority;
            sInsertBestPriorityLocation(info);
            s_globals->m_priorityBoundaryLists[eOldPriority].RemoveAt(posOldBoundary);
        }
    }
}








AsyncStatus SNOFilesGetAsyncStatus(SNOElement * ptSNOElement) {
    AsyncRecordRef pos;

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

    pos = SNOFileGetAsyncRecord(ptSNOElement);
    if (!pos) {
        return e_failed;
    }

    return s_globals->m_pendingRecordList.GetAt(pos).status;

}





























AsyncStatus SNOFilesBlockUntilCompleted(SNOElement * ptSNOElement) {
    while (TRUE) {
        {
            AsyncRecordRef pos;
            AsyncRecord * pRecord;









            Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

            pos = SNOFileGetAsyncRecord(ptSNOElement);
            if (!pos) {
                return e_failed;
            }

            pRecord = &s_globals->m_pendingRecordList.GetAt(pos);
            if (pRecord->status != e_pending) {
                pRecord->reapable = true;
                SNOFileAssignAsyncRecord(ptSNOElement, NULL);
                return pRecord->status;
            }

            if (!pRecord->inUse) {
                pRecord->reapable = true;
                SNOFileAssignAsyncRecord(ptSNOElement, NULL);
                return e_canceled;
            }
        }






        s_globals->m_someWorkCompletedEvent.WaitForever();
    }
}

































void SNOFilesCloseAsyncRecord(SNOElement * ptSNOElement) {
    while (TRUE) {
        {
            AsyncRecordRef pos;
            AsyncRecord * record;






            Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

            pos = SNOFileGetAsyncRecord(ptSNOElement);
            if (!pos) {
                return;
            }

            record = &s_globals->m_pendingRecordList.GetAt(pos);
            if (!record->inUse) {
                record->reapable = true;
                SNOFileAssignAsyncRecord(ptSNOElement, NULL);
                return;
            }
        }




        s_globals->m_someWorkCompletedEvent.WaitForever();
    }
}




static void sRemoveRecord(
    Console::IO::CPKArchiveReader *ptArchive,
    POS pos)
{
    RecordBoundary &tInfo = *s_globals->m_archiveBoundaryMap.LookupPtr(ptArchive);
    if (tInfo.firstRecord == pos)
    {
        if (tInfo.lastRecord == pos)
        {
            s_globals->m_archiveBoundaryMap.RemoveAt(ptArchive);
        }
        else
        {
            s_globals->m_pendingRecordList.GetNext(tInfo.firstRecord);
        }
    }
    else if (tInfo.lastRecord == pos)
    {
        s_globals->m_pendingRecordList.GetPrev(tInfo.lastRecord);
    }

    sRemoveFromPriorityList(pos);

    s_globals->m_pendingRecordList.RemoveAt(pos);
}

// NON_MATCHING: 1224 vs 1268 bytes. 1091 tests pCurrentRecordPos (original latch cbnz x28 at 0x69c73c; the record pointer is known non-null and folds fRecordFound, the POS does not; --i col 87 vs 85). Remaining: 1164 FALSE path shares the 1286 tail where the original selects (cset/csel at 1164:13); x27=0x1008 hoisted and i spilled in the original.
static BOOL sGetNextRecord(
    AsyncRecordRef pBaseRecordPos,
    AsyncRecord *ptBaseRecord,
    AsyncRecordRef pCurrentRecordPos,
    AsyncRecord *ptCurrentRecord,
    AsyncRecordRef &pNextRecordPosOut,
    AsyncRecord *&ptNextRecordOut,
    BOOL fRemoveReapable)
{
    XGrowableList<AsyncRecord, const AsyncRecord &> &recordList = s_globals->m_pendingRecordList;

    if (!pCurrentRecordPos)
    {
        BOOL fRecordFound = FALSE;
        for (int32 i = SNOFilesAsyncPriority_Count - 1; !pCurrentRecordPos && i >= 0; --i)
        {
            PriorityBoundaryList &tList = s_globals->m_priorityBoundaryLists[i];
            POS pos = tList.GetHeadPosition();
            while (pos && !fRecordFound)
            {
                PriorityBoundaryInfo &info = tList.GetNext(pos);
                AsyncRecordRef recordPos = info.boundary.firstRecord;
                AsyncRecordRef lastRecordPos = info.boundary.lastRecord;
                AsyncRecordRef checkRecordPos;
                do
                {
                    checkRecordPos = recordPos;
                    AsyncRecord *checkRecord = &recordList.GetNext(recordPos);

                    if (checkRecord->reapable)
                    {
                        if (fRemoveReapable)
                        {


                            checkRecord->ptArchive->RemoveFileReference();

                            sRemoveRecord(checkRecord->ptArchive, checkRecordPos);
                        }
                    }
                    else if ((checkRecord->status == e_pending) && (checkRecord->nLocation != -1))
                    {

                        if (checkRecord->nFirstReadSector == -1)
                        {

                            Console::IO::CPKArchiveReader *ptArchive = checkRecord->ptArchive;
                            if (ptArchive->IsCompressed())
                            {
                                int32 nOffsetInSector;
                                int32 nCompSectorOffsetInReadSector;
                                checkRecord->nFirstReadSector = ptArchive->CompSectorToReadSector(ptArchive->OffsetToCompSector(checkRecord->nLocation, nOffsetInSector), nCompSectorOffsetInReadSector);
                                checkRecord->nLastReadSector = ptArchive->CompSectorToReadSector(ptArchive->OffsetToCompSector(checkRecord->nLocation + checkRecord->nSize - 1, nOffsetInSector), nCompSectorOffsetInReadSector);
                            }
                            else
                            {
                                int32 nOffsetInSector;
                                checkRecord->nFirstReadSector = ptArchive->OffsetToReadSector(checkRecord->nLocation, nOffsetInSector, TRUE);
                                checkRecord->nLastReadSector = ptArchive->OffsetToReadSector(checkRecord->nLocation + checkRecord->nSize - 1, nOffsetInSector, TRUE);
                            }
                        }

                        // The first usable record is kept in case no record follows the last read (DWARF 1140).
                        if (!pCurrentRecordPos)
                        {
                            pCurrentRecordPos = checkRecordPos;
                            ptCurrentRecord = checkRecord;
                        }

                        if (checkRecord->ptArchive == s_globals->m_ptLastArchive && checkRecord->nLocation > s_globals->m_nLastLocation)
                        {
                            pCurrentRecordPos = checkRecordPos;
                            ptCurrentRecord = checkRecord;
                            fRecordFound = TRUE;
                            break;
                        }
                    }
                } while (checkRecordPos != lastRecordPos);
            }
        }







        if (pCurrentRecordPos)
        {
            pNextRecordPosOut = pCurrentRecordPos;
            ptNextRecordOut = ptCurrentRecord;
            return TRUE;
        }
    }
    else
    {
        AsyncRecordRef pNextRecordPos = NULL;
        AsyncRecord *ptNextRecord = NULL;
        PriorityBoundaryInfo &baseInfo = *(PriorityBoundaryInfo *)ptBaseRecord->posPriorityBoundary;


        BOOL fKeepGoing = TRUE;
        while (fKeepGoing)
        {

            PriorityBoundaryInfo &info = *(PriorityBoundaryInfo *)ptCurrentRecord->posPriorityBoundary;
            if (info.boundary.lastRecord == pCurrentRecordPos)
            {
                POS posPriorityBoundary = ptCurrentRecord->posPriorityBoundary;
                s_globals->m_priorityBoundaryLists[info.ePriority].GetNext(posPriorityBoundary);
                if (posPriorityBoundary)
                {
                    pNextRecordPos = ((PriorityBoundaryInfo *)posPriorityBoundary)->boundary.firstRecord;
                }
                else
                {

                    if (info.ePriority == baseInfo.ePriority)
                    {
                        PriorityBoundaryInfo &nextInfo = s_globals->m_priorityBoundaryLists[info.ePriority].GetHead(); pNextRecordPos = nextInfo.boundary.firstRecord;
                    }
                    else
                    {
                        pNextRecordPos = NULL;

                        for (int32 i = info.ePriority - 1; i >= 0; --i)
                        {
                            PriorityBoundaryList &tList = s_globals->m_priorityBoundaryLists[i];
                            POS pos = tList.GetHeadPosition();
                            if (pos)
                            {
                                pNextRecordPos = ((PriorityBoundaryInfo *)pos)->boundary.firstRecord;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                pNextRecordPos = pCurrentRecordPos;

                recordList.GetNext(pNextRecordPos);
            }


            if (pNextRecordPos == pBaseRecordPos)
            {
                pNextRecordPos = NULL;
                for (int32 i = info.ePriority - 1; i >= 0; --i)
                {
                    PriorityBoundaryList &tList = s_globals->m_priorityBoundaryLists[i];
                    POS pos = tList.GetHeadPosition();
                    if (pos)
                    {
                        PriorityBoundaryInfo &nextInfo = *(PriorityBoundaryInfo *)pos;
                        pNextRecordPos = nextInfo.boundary.firstRecord;
                        break;
                    }
                }
            }

            if (!pNextRecordPos)
                fKeepGoing = FALSE;
            else
            {
                ptNextRecord = (AsyncRecord *)pNextRecordPos;
                if (ptNextRecord->reapable || (ptNextRecord->status != e_pending) || (ptNextRecord->nLocation == -1))
                {
                    ptCurrentRecord = ptNextRecord;
                    pCurrentRecordPos = pNextRecordPos;
                }
                else
                {
                    fKeepGoing = FALSE;

                    if (ptNextRecord->nFirstReadSector == -1)
                    {

                        Console::IO::CPKArchiveReader *ptArchive = ptNextRecord->ptArchive;
                        if (ptArchive->IsCompressed())
                        {
                            int32 nOffsetInSector;
                            int32 nCompSectorOffsetInReadSector;
                            ptNextRecord->nFirstReadSector = ptArchive->CompSectorToReadSector(ptArchive->OffsetToCompSector(ptNextRecord->nLocation, nOffsetInSector), nCompSectorOffsetInReadSector);
                            ptNextRecord->nLastReadSector = ptArchive->CompSectorToReadSector(ptArchive->OffsetToCompSector(ptNextRecord->nLocation + ptNextRecord->nSize - 1, nOffsetInSector), nCompSectorOffsetInReadSector);
                        }
                        else
                        {
                            int32 nOffsetInSector;
                            ptNextRecord->nFirstReadSector = ptArchive->OffsetToReadSector(ptNextRecord->nLocation, nOffsetInSector, TRUE);
                            ptNextRecord->nLastReadSector = ptArchive->OffsetToReadSector(ptNextRecord->nLocation + ptNextRecord->nSize - 1, nOffsetInSector, TRUE);
                        }
                    }
                }
            }
        }

        if (pNextRecordPos)
        {
            pNextRecordPosOut = pNextRecordPos;
            ptNextRecordOut = ptNextRecord;
            return TRUE;
        }
    }




    pNextRecordPosOut = NULL;
    ptNextRecordOut = NULL;
    return FALSE;
}




struct SortableLocation
{
    Console::IO::CPKArchiveReader *ptArchive;
    int32 nSize;
    int32 nLocationCount;
    int32 nLocationIndex;
    int64 nLocation;
    AsyncRecordRef pos;
};


int32 sSortSortableLocations(const void * pLocation1, const void * pLocation2) {
    SortableLocation * ptLocation1 = (SortableLocation *)pLocation1;
    SortableLocation * ptLocation2 = (SortableLocation *)pLocation2;


    if ((int64)ptLocation1->ptArchive != (int64)ptLocation2->ptArchive) {

        return (int32)((int64)ptLocation1->ptArchive - (int64)ptLocation2->ptArchive);
    }



    if (ptLocation1->nLocation > ptLocation2->nLocation) {
        return 1;
    }
    return -1;
}







int32 sComputeScore(
    int64 nPreviousLocation,
    int32 nPreviousSize,
    Console::IO::CPKArchiveReader *ptPreviousArchive,
    int64 nLocation,
    Console::IO::CPKArchiveReader *ptArchive)
{
    int64 nScore;
    if (ptPreviousArchive == ptArchive)
    {

        nScore = nLocation - (nPreviousLocation + nPreviousSize);
        if (nScore > ptArchive->GetReadSectorSize() * 2)
        {
            nScore *= 2;
        }
    }
    else
    {
        nScore = 0x200000;
    }

    return (int32)Math::Min<int64>(nScore, INT_MAX);
}


static void sSortRecordToProcess()
{
    int32 nCount = 0;
    int32 nMultiLocationRecordCount = 0;
    XGrowableList<AsyncRecord, const AsyncRecord&> tempRecordList;
    BOOL fMoveToTempList = s_globals->m_recordListForceSort;
    for (AsyncRecordRef recordPos = s_globals->m_pendingRecordList.GetHeadPosition(); recordPos; )
    {
        AsyncRecordRef checkRecordPos = recordPos;
        AsyncRecord *checkRecord = &s_globals->m_pendingRecordList.GetNext(recordPos);
        if (checkRecord->reapable)
        {

            checkRecord->ptArchive->RemoveFileReference();
            sRemoveRecord(checkRecord->ptArchive, checkRecordPos);
        }
        else
        {
            int32 nLocationCount = checkRecord->nLocationCount;
            nCount += nLocationCount;
            if (nLocationCount > 1)
            {
                nMultiLocationRecordCount++;
            }

            if (fMoveToTempList)
            {
                AsyncRecord record(*checkRecord);
                sRemoveRecord(checkRecord->ptArchive, checkRecordPos);
                SNOFileAssignAsyncRecord(checkRecord->snoElement, tempRecordList.AddTail_Tracked(record));
            }
        }
    }

    SortableLocation *pAsyncRecords = (SortableLocation*)SigmaMemoryNew(nCount * sizeof(SortableLocation) + nMultiLocationRecordCount * sizeof(AsyncRecordRef), 4, NULL, 0);
    AsyncRecordRef *pMultiRecords = (AsyncRecordRef *)&pAsyncRecords[nCount];

    int32 nLastLeftToMove = -1;
    int32 nLeftToMove = 0;
    BOOL fKeepSorting = TRUE;
    while (fKeepSorting && (nLastLeftToMove != nLeftToMove))
    {
        nLastLeftToMove = nLeftToMove;
        nLeftToMove = 0;
        fKeepSorting = FALSE;

        int32 i = 0;
        int32 nMultiIndex = 0;
        int32 nNewCount = 0;
        XGrowableList<AsyncRecord, const AsyncRecord&> &recordSourceList = fMoveToTempList ? tempRecordList : s_globals->m_pendingRecordList;
        for (AsyncRecordRef recordPos = recordSourceList.GetHeadPosition(); recordPos; )
        {
            AsyncRecordRef checkRecordPos = recordPos;
            AsyncRecord *checkRecord = &recordSourceList.GetNext(recordPos);
            int32 nLocationCount = checkRecord->nLocationCount;
            nNewCount += nLocationCount;
            if (nLocationCount == 1)
            {
                pAsyncRecords[i].ptArchive = checkRecord->ptArchive;
                pAsyncRecords[i].nSize = checkRecord->nSize;
                pAsyncRecords[i].nLocation = checkRecord->nLocation;
                pAsyncRecords[i].nLocationCount = 1;
                pAsyncRecords[i].nLocationIndex = 0;
                pAsyncRecords[i].pos = checkRecordPos;
                i++;
            }
            else
            {

                for (int32 nPos = 0; nPos < nLocationCount; nPos++)
                {
                    pAsyncRecords[i].ptArchive = checkRecord->ptArchive;
                    pAsyncRecords[i].nSize = checkRecord->nSize;
                    pAsyncRecords[i].nLocation = checkRecord->ptLocationsAndScore[nPos].nLocation;
                    pAsyncRecords[i].nLocationCount = nLocationCount;
                    pAsyncRecords[i].nLocationIndex = nPos;
                    pAsyncRecords[i].pos = checkRecordPos;
                    i++;
                }

                pMultiRecords[nMultiIndex++] = checkRecordPos;
            }
        }

        nCount = nNewCount;
        nMultiLocationRecordCount = nMultiIndex;

        qsort(pAsyncRecords, nNewCount, sizeof(SortableLocation), sSortSortableLocations);


        for (i = 0; i < nCount; i++)
        {
            if (pAsyncRecords[i].nLocationCount != 1)
            {
                int32 nScorePrev = 0;
                int32 nScoreNext = 0;
                POS myPos = pAsyncRecords[i].pos;


                SortableLocation *pPrevAsyncRecord = NULL;
                for (int32 j = i - 1; j >= 0; j--)
                {
                    if (pAsyncRecords[j].pos != myPos)
                    {
                        pPrevAsyncRecord = &pAsyncRecords[j];
                        break;
                    }
                }


                SortableLocation *pNextAsyncRecord = NULL;
                for (int32 j = i + 1; j < nCount; j++)
                {
                    if (pAsyncRecords[j].pos != myPos)
                    {
                        pNextAsyncRecord = &pAsyncRecords[j];
                        break;
                    }
                }

                if (pPrevAsyncRecord)
                {
                    nScorePrev = sComputeScore(pPrevAsyncRecord->nLocation, pPrevAsyncRecord->nSize, pPrevAsyncRecord->ptArchive, pAsyncRecords[i].nLocation, pAsyncRecords[i].ptArchive);
                }
                if (pNextAsyncRecord)
                {
                    nScoreNext = sComputeScore(pAsyncRecords[i].nLocation, pAsyncRecords[i].nSize, pAsyncRecords[i].ptArchive, pNextAsyncRecord->nLocation, pNextAsyncRecord->ptArchive);
                }

                if (pAsyncRecords[i].ptArchive == s_globals->m_ptLastArchive && pAsyncRecords[i].nLocation < s_globals->m_nLastLocation)
                {
                    nScorePrev = (int32)Math::Min<int64>(nScorePrev + 100LL, INT_MAX);
                    nScoreNext = (int32)Math::Min<int64>(nScoreNext + 100LL, INT_MAX);
                }

                AsyncRecord *record = &recordSourceList.GetAt(myPos);

                record->ptLocationsAndScore[pAsyncRecords[i].nLocationIndex].nScorePrev = nScorePrev;
                record->ptLocationsAndScore[pAsyncRecords[i].nLocationIndex].nScoreNext = nScoreNext;
            }
            else if (fMoveToTempList)
            {
                AsyncRecord record(tempRecordList.GetAt(pAsyncRecords[i].pos));
                tempRecordList.RemoveAt(pAsyncRecords[i].pos);
                SNOFileAssignAsyncRecord(record.snoElement, sInsertBestLocation(record));
            }
        }


        for (i = 0; i < nMultiLocationRecordCount; i++)
        {
            AsyncRecord record(tempRecordList.GetAt(pMultiRecords[i]));
            int32 nLocationCount = record.nLocationCount;


            if (record.ptLocationsAndScore[0].nFirstReadSector == -1)
            {

                Console::IO::CPKArchiveReader *ptArchive = record.ptArchive;
                if (ptArchive->IsCompressed())
                {
                    for (int32 nLoc = 0; nLoc < nLocationCount; nLoc++)
                    {
                        int32 nOffsetInSector;
                        int32 nCompSectorOffsetInReadSector;
                        record.ptLocationsAndScore[nLoc].nFirstReadSector = ptArchive->CompSectorToReadSector(ptArchive->OffsetToCompSector(record.ptLocationsAndScore[nLoc].nLocation, nOffsetInSector), nCompSectorOffsetInReadSector);
                        record.ptLocationsAndScore[nLoc].nLastReadSector = ptArchive->CompSectorToReadSector(ptArchive->OffsetToCompSector(record.ptLocationsAndScore[nLoc].nLocation + record.nSize - 1, nOffsetInSector), nCompSectorOffsetInReadSector);
                    }
                }
                else
                {

                    for (int32 nLoc = 0; nLoc < nLocationCount; nLoc++)
                    {
                        int32 nOffsetInSector;
                        record.ptLocationsAndScore[nLoc].nFirstReadSector = ptArchive->OffsetToReadSector(record.ptLocationsAndScore[nLoc].nLocation, nOffsetInSector, TRUE);
                        record.ptLocationsAndScore[nLoc].nLastReadSector = ptArchive->OffsetToReadSector(record.ptLocationsAndScore[nLoc].nLocation + record.nSize - 1, nOffsetInSector, TRUE);
                    }
                }
            }


            int64 nBestLocation = record.ptLocationsAndScore[0].nLocation;
            int32 nBestFirstReadSector = record.ptLocationsAndScore[0].nFirstReadSector;
            int32 nBestLastReadSector = record.ptLocationsAndScore[0].nLastReadSector;
            int32 nBestScorePrev = record.ptLocationsAndScore[0].nScorePrev;
            int32 nBestScoreNext = record.ptLocationsAndScore[0].nScoreNext;
            int64 nBestScore = (int64)nBestScorePrev + nBestScoreNext;
            for (int32 nLoc = 1; nLoc < nLocationCount; nLoc++)
            {
                int64 nScore = (int64)record.ptLocationsAndScore[nLoc].nScorePrev + record.ptLocationsAndScore[nLoc].nScoreNext;
                if (nScore < nBestScore)
                {
                    nBestLocation = record.ptLocationsAndScore[nLoc].nLocation;
                    nBestFirstReadSector = record.ptLocationsAndScore[nLoc].nFirstReadSector;
                    nBestLastReadSector = record.ptLocationsAndScore[nLoc].nLastReadSector;
                    nBestScorePrev = record.ptLocationsAndScore[nLoc].nScorePrev;
                    nBestScoreNext = record.ptLocationsAndScore[nLoc].nScoreNext;
                    nBestScore = nScore;
                }
            }

            record.nLocation = nBestLocation;
            record.nFirstReadSector = nBestFirstReadSector;
            record.nLastReadSector = nBestLastReadSector;
            record.nScorePrev = nBestScorePrev;
            record.nScoreNext = nBestScoreNext;

            if (fMoveToTempList)
            {
                tempRecordList.RemoveAt(pMultiRecords[i]);
            }
            else
            {
                sRemoveRecord(record.ptArchive, pMultiRecords[i]);
            }
            SNOFileAssignAsyncRecord(record.snoElement, sInsertBestLocation(record));
        }



        if (nMultiLocationRecordCount > 0)
        {


            AsyncRecord *ptPrevRecord = NULL;
            AsyncRecord *ptRecord = NULL;
            AsyncRecord *ptNextRecord = NULL;
            AsyncRecordRef recordPos = s_globals->m_pendingRecordList.GetHeadPosition();
            AsyncRecordRef checkRecordPos = recordPos;
            if (recordPos)
            {
                ptRecord = &s_globals->m_pendingRecordList.GetNext(recordPos);
            }

            int32 nPrevLocationCount = 0;
            while (ptRecord)
            {

                ptNextRecord = NULL;

                if (recordPos)
                {
                    ptNextRecord = &s_globals->m_pendingRecordList.GetNext(recordPos);
                }





                if (ptRecord->nLocationCount > 1)
                {
                    int32 nScorePrev = 0;
                    int32 nScoreNext = 0;
                    if (ptPrevRecord != NULL)
                    {
                        nScorePrev = sComputeScore(ptPrevRecord->nLocation, ptPrevRecord->nSize, ptPrevRecord->ptArchive, ptRecord->nLocation, ptRecord->ptArchive);
                    }

                    if (ptNextRecord != NULL)
                    {
                        nScoreNext = sComputeScore(ptRecord->nLocation, ptRecord->nSize, ptRecord->ptArchive, ptNextRecord->nLocation, ptNextRecord->ptArchive);
                    }

                    if (ptRecord->ptArchive == s_globals->m_ptLastArchive && ptRecord->nLocation < s_globals->m_nLastLocation)
                    {
                        nScorePrev = (int32)Math::Min<int64>(nScorePrev + 100LL, INT_MAX);
                        nScoreNext = (int32)Math::Min<int64>(nScoreNext + 100LL, INT_MAX);
                    }


                    if ((ptRecord->nScorePrev == nScorePrev && (nPrevLocationCount == 1 || (ptPrevRecord != NULL && ptRecord->nScoreNext == nScoreNext && ptNextRecord != NULL))) || (ptNextRecord != NULL && ptRecord->nScoreNext == nScoreNext && ptNextRecord->nLocationCount == 1))
                    {

                        ptRecord->nLocationCount = 1;
                    }
                    else
                    {
                        nLeftToMove++;
                        fKeepSorting = TRUE;
                    }
                }

                nPrevLocationCount = ptRecord->nLocationCount;
                ptPrevRecord = ptRecord;
                ptRecord = ptNextRecord;
            }
        }

        fMoveToTempList = FALSE;
    }



    s_globals->m_pNextRecordPos = NULL;
    SigmaMemoryFree(pAsyncRecords, NULL);
}



static Console::IO::ReadAheadSuggestion sGetReadAheadSuggestion(
    const AsyncRecordRef pRecordPos)
{
    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

    Console::IO::ReadAheadSuggestion tReadAheadSuggestion;
    tReadAheadSuggestion.ptLastArchive = NULL;
    tReadAheadSuggestion.nLastSector = -1;
    tReadAheadSuggestion.ptNextArchive = NULL;
    tReadAheadSuggestion.nNextSector = -1;

    XGrowableList<AsyncRecord, const AsyncRecord &> &recordList = s_globals->m_pendingRecordList;

    AsyncRecordRef pBaseRecordPos = pRecordPos;
    AsyncRecord *ptBaseRecord = (AsyncRecord *)pBaseRecordPos;

    AsyncRecordRef pCurrentRecordPos = pBaseRecordPos;
    AsyncRecord *ptCurrentRecord = ptBaseRecord;

    AsyncRecordRef pNextRecordPos;
    AsyncRecord *ptNextRecord;
    while (sGetNextRecord(pBaseRecordPos, ptBaseRecord, pCurrentRecordPos, ptCurrentRecord, pNextRecordPos, ptNextRecord, FALSE))
    {
        if ((ptBaseRecord->ptArchive != ptNextRecord->ptArchive) ||
            ((ptNextRecord->nFirstReadSector != ptBaseRecord->nLastReadSector) && (ptNextRecord->nFirstReadSector != ptBaseRecord->nLastReadSector + 1)))
        {
            tReadAheadSuggestion.ptLastArchive = ptBaseRecord->ptArchive;
            tReadAheadSuggestion.nLastSector = ptBaseRecord->nLastReadSector;
            tReadAheadSuggestion.ptNextArchive = ptNextRecord->ptArchive;
            tReadAheadSuggestion.nNextSector = ptNextRecord->nFirstReadSector;
            break;
        }

        if (ptNextRecord->nLastReadSector != ptBaseRecord->nLastReadSector)
        {
            break;
        }


        pCurrentRecordPos = pNextRecordPos;
        ptCurrentRecord = ptNextRecord;
    }

    return tReadAheadSuggestion;
}













































































static AsyncRecordRef sGetBestRecordToProcess()
{

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

    if (s_globals->m_paused)
    {
        return NULL;
    }


    if (s_globals->m_recordListDirty || s_globals->m_recordListForceSort)
    {
        sSortRecordToProcess();
        s_globals->m_recordListDirty = false;
        s_globals->m_recordListForceSort = false;
    }








    AsyncRecordRef pCurrentRecordPos = s_globals->m_pNextRecordPos;
    AsyncRecord *ptCurrentRecord = NULL;
    if (!pCurrentRecordPos)
    {
        if (!sGetNextRecord(NULL, NULL, NULL, NULL, pCurrentRecordPos, ptCurrentRecord, TRUE))
        {
            return NULL;
        }
    }
    else
    {
        ptCurrentRecord = &s_globals->m_pendingRecordList.GetAt(pCurrentRecordPos);
        if (ptCurrentRecord->reapable)
        {
            sGetNextRecord(pCurrentRecordPos, ptCurrentRecord, pCurrentRecordPos, ptCurrentRecord, pCurrentRecordPos, ptCurrentRecord, TRUE);
        }
    }

    s_globals->m_pNextRecordPos = NULL;

    if (pCurrentRecordPos)
    {
        ptCurrentRecord->inUse = true;


        s_globals->m_ptLastArchive = ptCurrentRecord->ptArchive;
        s_globals->m_nLastLocation = ptCurrentRecord->nLocation;


        AsyncRecord *ptNextRecord;
        sGetNextRecord(pCurrentRecordPos, ptCurrentRecord, pCurrentRecordPos, ptCurrentRecord, s_globals->m_pNextRecordPos, ptNextRecord, TRUE);
    }





    return pCurrentRecordPos;
}

// NON_MATCHING: 44 vs 44 bytes; seven instructions differ in register allocation; existing equality operand order.
BOOL SNOFilesAsyncIsLocking(SNOElement * ptSNOElement) {

    return s_globals->m_tLockingSNO == ptSNOElement->tEntry.ToSNOName();
}


void SNOFilesDisableMaxCompletedSNO() {

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    s_globals->m_nMaxCompletedSNO = 512;

}

void SNOFilesEnableMaxCompletedSNO() {

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    s_globals->m_nMaxCompletedSNO = 32;

}

void SNOFilesAsyncStartNewRun() {

    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

    if (s_globals->m_pendingRecordList.Count() > 0) {

        s_globals->m_theresWorkToDoEvent.Set();
    }

}


static void sSuggestReadAhead(
    AsyncRecordRef tRecordRef)
{
    Console::IO::ReadAheadSuggestion tReadAheadSuggestion = sGetReadAheadSuggestion(tRecordRef);
    Console::IO::ReadAheadSuggestion &tLastReadAheadSuggestion = s_globals->m_tLastReadAheadSuggestion;
    if (tLastReadAheadSuggestion.ptLastArchive == tReadAheadSuggestion.ptLastArchive && tLastReadAheadSuggestion.nLastSector == tReadAheadSuggestion.nLastSector &&
        tLastReadAheadSuggestion.ptNextArchive == tReadAheadSuggestion.ptNextArchive && tLastReadAheadSuggestion.nNextSector == tReadAheadSuggestion.nNextSector)
    {
        return;
    }

    tLastReadAheadSuggestion = tReadAheadSuggestion;

    Console::IO::SuggestAheadReadSector(tReadAheadSuggestion, Console::IO::TASKPRIORITY_HIGH);
}








static void sAsyncThreadLoop();  // X/Common/Sno/SNOFilesAsync.cpp:2068

// NON_MATCHING: 6496 vs 6496 bytes; sGetBestRecordToProcess and sAsyncThreadLoop are identical. Inlined sSortRecordToProcess differs: register allocation (nCount/nMulti), the score pass re-reads nPrevLocationCount after the store at 1629 where the original keeps the old count, and the 1626 condition uses branches where the original uses ldp+cset+and. Tried 3 shapes of the 1626 condition.
static uint32 SNOFilesAsyncThread(void *)
{
    ThreadGlobals::InitializeForThread();
    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_ASYNCLOAD;



















    sAsyncThreadLoop();


    ThreadGlobals::DisposeForThread();
    return 0;
}


void SNOFilesAsyncWaitForInPlaceLock() {
    while (s_globals->m_inPlaceLocking) {
        Blizzard::Process::Sleep(1);
    }
}




int32 SNOFilesAsyncGetPendingCount() {
    int32 nCompletedCount = 0;




    for (int32 i = 0; i < SNOFilesAsyncPriority_Count; ++i) {
        nCompletedCount += s_globals->m_completedRecordList[i].Count();
    }
    return s_globals->m_pendingRecordList.Count() + nCompletedCount;
}

int32 SNOFilesAsyncGetPriorityPendingCount(SNOFilesAsyncPriority ePriority) {
    int32 nPendingCount = 0;




    for (int32 i = SNOFilesAsyncPriority_Count - 1; i >= ePriority; --i) {
        nPendingCount += s_globals->m_completedRecordList[i].Count() + s_globals->m_priorityBoundaryLists[i].Count();
    }
    return nPendingCount;
}



void SNOFilesAsyncDisableSortedRequest() {


    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    s_globals->m_fSortedRequestDisabled = true;

}


void SNOFilesAsyncSortRequests() {


    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    s_globals->m_recordListForceSort = true;

}


void SNOFilesAsyncEnableSortedRequest() {


    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
    s_globals->m_fSortedRequestDisabled = false;

}


BOOL SNOFilesAsyncHasPending() {

    return SNOFilesAsyncGetPendingCount() > 0;
}



BOOL SNOFilesAsyncHasPriorityPending(SNOFilesAsyncPriority ePriority) {

    return SNOFilesAsyncGetPriorityPendingCount(ePriority) > 0;
}



int32 SNOFilesAsyncGetCompletedSNOs(SNOElement ** pptCompletedSNOElements, int32 nCapacity) {
    int32 nCount = 0;







    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_completedRecordListMutex);


    for (int32 i = SNOFilesAsyncPriority_Count - 1; i >= 0; --i) {

        AsyncRecordRef recordPos = s_globals->m_completedRecordList[i].GetHeadPosition();
        while (recordPos) {
            AsyncRecordRef checkRecordPos = recordPos;
            AsyncRecord * checkRecord = &s_globals->m_completedRecordList[i].GetNext(recordPos);


            if (checkRecord->reapable) {
                s_globals->m_completedRecordList[i].RemoveAt(checkRecordPos);
            } else {
                if (nCount == nCapacity) {
                    return nCount;
                }
                pptCompletedSNOElements[nCount++] = checkRecord->snoElement;
            }
        }
    }

    return nCount;
}



























static void sAsyncThreadLoop()
{

    do
    {
        s_globals->m_theresWorkToDoEvent.WaitForever();

        while (TRUE)
        {


            int32 nCompletedCount = 0;
            for (int32 i = 0; i < SNOFilesAsyncPriority_Count; ++i)
            {
                nCompletedCount += s_globals->m_completedRecordList[i].Count();
            }

            if (nCompletedCount >= s_globals->m_nMaxCompletedSNO)
            {
                break;
            }


































            AsyncRecordRef recordPos = sGetBestRecordToProcess();


            if (!recordPos)
                break;

            AsyncRecord *record = (AsyncRecord *)recordPos;

            SNOFileAsyncStats tStats;



            SNOEntry &tEntry = record->snoElement->tEntry;





















            if ((tEntry.snoGroup == ANIMATION_SNO || tEntry.snoGroup == APPEARANCE_SNO || tEntry.snoGroup == FONT_SNO ||
                tEntry.snoGroup == TEXTURE_SNO || tEntry.snoGroup == PHYSMESH_SNO) &&


                !(record->snoElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
            {
                s_globals->m_inPlaceLocking = true;


                record->inUse = false;


                s_globals->m_someWorkCompletedEvent.Set();

                s_globals->m_tLockingSNO = tEntry.ToSNOName();

                GroupDefinition *ptGroupDef = GameDefinitionGet(tEntry.snoGroup);


                sSuggestReadAhead(recordPos);

                void *lpDefinition = (void*)ptGroupDef->LockR((SNO)tEntry.uGuid, FALSE);
                ptGroupDef->UnlockR(lpDefinition);

                s_globals->m_tLockingSNO = SNOName(SNOGROUP_NONE, SNO_NONE);

                record->status = e_successful;

                s_globals->m_inPlaceLocking = false;

                {
                    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);
                    record->ptArchive->RemoveFileReference();
                    sRemoveRecord(record->ptArchive, recordPos);
                }

                continue;
            }

            Console::IO::FileInfo info = record->info;


            SNOFileStatResults eStatResult = SNOFileAsyncGenerateStats(record->snoElement, record->dwHash, record->ptArchive, &info, tStats);















            AsyncStatus status = e_successful;

            if ((tStats.m_ptFile != NULL) && (eStatResult == SNOFILESTATRESULT_SUCCESS))
            {
                FileIO::File tFile(XLOCALE_GLOBAL);
                bool bFileOpened = tFile.Open(*tStats.m_ptFile);

                if (bFileOpened)
                {
                    tFile.SetFileCursor(tStats.offset, FileIO::FROM_BEGINNING);

                    SFileOverrideLocation(tFile.GetHSFILE(), record->nLocation);
                    sSuggestReadAhead(recordPos);

                    bool bReadSuccessful = tFile.ReadFile(tStats.buffer, tStats.amount, NULL);



                    status = bReadSuccessful ? e_successful : e_failed;
                }
                else
                {













                    Blizzard::Mopaq::ErrorCode e = SFileGetLastError();


                    char szFilename[4096];
                    if (tStats.m_ptFile->GetFileName(szFilename, sizeof(szFilename)))
                    {
                        int64 nFileSize = tStats.m_ptFile->GetFileSize();








                    }

                    status = e_failed;
                }

            }
            else if (eStatResult == SNOFILESTATRESULT_SUCCESS)
            {

                status = FileRead(*tStats.diskFile, tStats.buffer, tStats.offset, tStats.amount, ERROR_FILE_READ) ? e_successful : e_failed;
            }



























            else if (eStatResult == SNOFILESTATRESULT_FILE_TOO_BIG_FOR_PREFETCH)
            {
                status = e_successful;
            }
            else if (eStatResult == SNOFILESTATRESULT_OUT_OF_MEMORY)
            {


                {
                    Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

                    record->status = e_pending;

                    record->inUse = false;
                }


                s_globals->m_someWorkCompletedEvent.Set();

                Sleep(100);

                continue;
            }
            else
                status = e_failed;
            {
                Blizzard::Lock::ScopedRMutex mutex(s_globals->m_recordListMutex);

                record->status = status;

                record->inUse = false;

                {
                    Blizzard::Lock::ScopedRMutex mutex2(s_globals->m_completedRecordListMutex);
                    SNOFileAssignAsyncRecord(record->snoElement, s_globals->m_completedRecordList[record->ePriority].AddTail_Tracked(*record));
                }

                record->ptArchive->RemoveFileReference();

                sRemoveRecord(record->ptArchive, recordPos);
            }


            s_globals->m_someWorkCompletedEvent.Set();


            nCompletedCount = 0;
            for (int32 i = 0; i < SNOFilesAsyncPriority_Count; ++i)
            {
                nCompletedCount += s_globals->m_completedRecordList[i].Count();
            }

            if (nCompletedCount >= s_globals->m_nMaxCompletedSNO)
            {
                break;
            }
        }
    }
    while (!s_globals->m_terminateAsyncThread);


    for (int32 i = 0; i < SNOFilesAsyncPriority_Count; ++i)
    {
        for (AsyncRecordRef recordPos = s_globals->m_completedRecordList[i].GetHeadPosition(); recordPos; )
        {
            AsyncRecord *checkRecord = &s_globals->m_completedRecordList[i].GetNext(recordPos);
            if (!checkRecord->reapable)
            {








                SNOFileClose(checkRecord->snoElement);
            }
        }
    }
}

