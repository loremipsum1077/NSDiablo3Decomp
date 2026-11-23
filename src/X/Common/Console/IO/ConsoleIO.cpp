// Original file: X/Common/Console/IO/ConsoleIO.cpp

#include "ConsoleIO.h"

#include <ctype.h>
#include <strings.h>

// The original reaches these through an include chain this file does not reproduce; they are
// here for the globals they add to the static initializer (blz::ignore, SNONAME_NONE,
// cgplaceNull, the GfxVB normals and the nn::ldn addresses).
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../SNO/SNOName.h"
#include "../../WorldPlace.h"
#include "../../../Client/Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

#include "../../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../../../Contrib/Contrib/blz/include/blz/mutex.h"
#include "../../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/SigmaThreadLocalStorage.h"
#include "../../../Sigma/UnicodeText.h"
#include "ConsoleIO_ArchiveReader.h"
#include "ConsoleIO_TaskThread.h"
#include "ConsoleIO_Tasks.h"

namespace Console {
namespace IO {

struct GlobalFileInfo {
    CPKArchiveReader * ptArchive;
    FileInfo info;
    GlobalFileInfo * ptNextInfo;
};

struct GlobalFileInfoBase {
    GlobalFileInfo * ptBase;
    CPKArchiveReader * ptArchive;
};

// NON_MATCHING (XGrowableMap_Sub constructor, emitted out of line from here): 704 vs 680 bytes;
// src XBaseMap::_InitHashTable_Tracked has a size-equality guard the original does not.
static XGrowableMap<uint64, uint64, GlobalFileInfo *, GlobalFileInfo *> stGlobalFileMap(16, NULL);
static XGrowableList<GlobalFileInfoBase *, GlobalFileInfoBase *> stGlobalFileInfoBase;


static IOAccessTaskThread * sptIOAccessThread = NULL;
static DecompressionTaskThread * sptDecompressionThread = NULL;
static XGrowableList<CPKArchiveReader *, CPKArchiveReader *> stOpenedArchives;
static Blizzard::Lock::RMutex stConsoleIOMutex;
static SigmaThreadLocalStorage stThreadData;
static XGrowableList<InternalBuffer *, InternalBuffer *> stAllocatedInternalBuffer;







void InitializeForApp()
{

    stGlobalFileMap.SetHashTableSize_Tracked(128 * 1024);

    stThreadData.Initialize();

    sptIOAccessThread = new IOAccessTaskThread("IOAccessThread", Blizzard::Thread::PRIORITY_HIGHEST, 2);






    sptIOAccessThread->SetReady();

    sptDecompressionThread = new DecompressionTaskThread("DecompressionThread", Blizzard::Thread::PRIORITY_LOWEST, 1);
}








void DisposeForApp()
{
    stThreadData.Dispose();

    ::delete sptIOAccessThread;
    sptIOAccessThread = NULL;
    ::delete sptDecompressionThread;

    sptDecompressionThread = NULL;

    POS pos = stAllocatedInternalBuffer.GetHeadPosition();
    while (pos)
    {
        InternalBuffer * ptBufferInList = stAllocatedInternalBuffer.GetNext(pos);
        delete[] ptBufferInList->pBuffer;
        delete ptBufferInList;
    }
}
void Update()
{
    static uint32 sdwFrameToLog = 0;
    Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);

    sdwFrameToLog++;



    POS pos = stOpenedArchives.GetHeadPosition();
    while (pos)
    {
        CPKArchiveReader * ptArchiveItem = stOpenedArchives.GetNext(pos);
        if (ptArchiveItem->IsClosing())
        {
            int32 nOpenedFileRecordCount = ptArchiveItem->GetOpenedFileRecordCount();
            if (nOpenedFileRecordCount == 0)
            {
                CloseArchive(ptArchiveItem, TASKPRIORITY_HIGHEST);
            }
        }
    }
}







InternalBuffer * CreateInternalBuffer()
{
    InternalBuffer * ptInternalBuffer = new InternalBuffer;
    ptInternalBuffer->pBuffer = new uint8[CPK_READ_SECTOR_SIZE];
    ptInternalBuffer->nCapacity = CPK_READ_SECTOR_SIZE;
    ptInternalBuffer->nSize = -1;
    ptInternalBuffer->nOffset = -1;
    ptInternalBuffer->ptLastArchive = NULL;
    ptInternalBuffer->ptOwnerRecord = NULL;
    {
        Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);
        stAllocatedInternalBuffer.AddTail_Tracked(ptInternalBuffer);
    }
    return ptInternalBuffer;
}


void FreeInternalBuffer(InternalBuffer * ptInternalBuffer)
{
    {
        Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);
        POS pos = stAllocatedInternalBuffer.GetHeadPosition();
        while (pos)
        {
            POS oldPos = pos;
            InternalBuffer * ptBufferInList = stAllocatedInternalBuffer.GetNext(pos);
            if (ptBufferInList == ptInternalBuffer)
            {

                stAllocatedInternalBuffer.RemoveAt(oldPos);
                break;
            }
        }
    }

    delete[] ptInternalBuffer->pBuffer;
    delete ptInternalBuffer;
}





bool IsInitializeForThread()
{
    InternalBuffer * ptInternalBuffer = (InternalBuffer *)stThreadData.TLSGetValue();
    return ptInternalBuffer != NULL;
}







void InitializeForThread()
{
    InternalBuffer * ptInternalBuffer = CreateInternalBuffer();


    stThreadData.TLSSetValue(ptInternalBuffer);
}



void DisposeForThread()
{

    InternalBuffer * ptInternalBuffer = (InternalBuffer *)stThreadData.TLSGetValue();

    FreeInternalBuffer(ptInternalBuffer);

    stThreadData.TLSSetValue(NULL);
}


IOAccessTaskThread * GetIOAccessThread()
{
    return sptIOAccessThread;
}


DecompressionTaskThread * GetDecompressionThread()
{
    return sptDecompressionThread;
}


static Results sRequestReadRecord(FileRecord * ptRecord, void * pBuffer, int32 nFileOffset, int32 nBytesToRead, int32 * pnBytesRead, TaskPriorities nTaskPriority)
{
    Task * ptTask;
    Results nResult;
    int32 nBytesRead;






    if (!ptRecord->ptArchive->IsCompressed())
    {
        ReadRecordTask * ptReadTask = new ReadRecordTask;
        ptReadTask->Initialize(ptRecord, nFileOffset, nBytesToRead, pBuffer, nTaskPriority);
        ptTask = ptReadTask;
    }
    else
    {
        ReadAndDecompressRecordTask * ptReadAndDecompressTask = new ReadAndDecompressRecordTask;
        ptReadAndDecompressTask->Initialize(ptRecord, nFileOffset, nBytesToRead, pBuffer, nTaskPriority);
        ptTask = ptReadAndDecompressTask;
    }


    if (ptRecord->ptArchive->GetArchiveType() == CPKArchiveReader::CPK_ARCHIVE_TYPE_STANDARD)
    {
        GetIOAccessThread()->SubmitTask(ptTask);
    }


    ptTask->WaitForCompletion();
    nResult = ptTask->GetResults();


    if (ptTask->IsFailed())
    {
        ptTask->Release();
        if (pnBytesRead)
        {
            *pnBytesRead = 0;
        }
    }
    else
    {
        nBytesRead = ptTask->GetBytesRead();
        ptTask->Release();
        if (pnBytesRead)
        {
            *pnBytesRead = nBytesRead;
        }
    }
    return nResult;
}



void SuggestAheadReadSector(const ReadAheadSuggestion & tReadAheadSuggestion, TaskPriorities nTaskPriority)
{
    IOAccessTaskThread * ptIOAccessThread;
    AssignAheadReadSectorTask * ptTask;

    if (tReadAheadSuggestion.ptLastArchive == NULL)
        return;


    if (tReadAheadSuggestion.ptLastArchive->GetArchiveType() != tReadAheadSuggestion.ptNextArchive->GetArchiveType())
        return;




    ptIOAccessThread = GetIOAccessThread();
    ptTask = new AssignAheadReadSectorTask;
    ptTask->Initialize(tReadAheadSuggestion, nTaskPriority);
    ptIOAccessThread->SubmitTask(ptTask);
    ptTask->Release();
}





Results ReadRecord(FileRecord * ptRecord, void * pBuffer, int32 nFileOffset, int32 nBytesToRead, int32 * pnBytesRead, TaskPriorities nTaskPriority)
{
    int32 nValidDataSize;
    InternalBuffer * ptInternalBuffer;
    Results nResult;
    uint8 * pValidData;



    nBytesToRead = Math::Min(nBytesToRead, ptRecord->info.nSize - nFileOffset);
    if (nBytesToRead <= 0)
    {
        if (pnBytesRead)
        {
            *pnBytesRead = 0;
        }
        return IORESULT_SUCCESS;
    }


    if (!IsInitializeForThread())
    {
        InitializeForThread();
    }

    ptInternalBuffer = (InternalBuffer *)stThreadData.TLSGetValue();

    nValidDataSize = 0;

    if (ptInternalBuffer->Contains(ptRecord, nFileOffset, pValidData, nValidDataSize))
    {

        nValidDataSize = Math::Min(nValidDataSize, nBytesToRead);
        SigmaMemoryCopy(pBuffer, pValidData, nValidDataSize);
        nBytesToRead -= nValidDataSize;
        pBuffer = (uint8 *)pBuffer + nValidDataSize;
        nFileOffset += nValidDataSize;
    }

    if (nBytesToRead == 0)
    {
        if (pnBytesRead)
        {
            *pnBytesRead = nValidDataSize;
        }
        return IORESULT_SUCCESS;
    }

    ptRecord->ptInternalBuffer = ptInternalBuffer;
    ptInternalBuffer->ptOwnerRecord = ptRecord;






    nResult = sRequestReadRecord(ptRecord, pBuffer, nFileOffset, nBytesToRead, pnBytesRead, nTaskPriority);

    ptInternalBuffer->ptOwnerRecord = NULL;
    ptRecord->ptInternalBuffer = NULL;
    ptInternalBuffer->ptLastArchive = ptRecord->ptArchive;

    if (nResult != IORESULT_SUCCESS)
    {
        return nResult;
    }

    if (pnBytesRead)
    {
        *pnBytesRead += nValidDataSize;
    }
    return IORESULT_SUCCESS;
}







static void sRemoveArchiveFromGlobalList(CPKArchiveReader * ptArchive)
{
    int32 nFileCount;
    Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);
    POS pos = stOpenedArchives.Find(ptArchive);

    stOpenedArchives.RemoveAt(pos);


    nFileCount = ptArchive->GetFileCount();
    for (int32 i = 0; i < nFileCount; i++)
    {
        FileInfo info;
        if (ptArchive->GetFileInfo(i, info))
        {
            GlobalFileInfo * ptLastInfo = NULL;
            GlobalFileInfo * ptGlobalFileInfo;


            if (stGlobalFileMap.Lookup(info.dwHash, ptGlobalFileInfo))
            {
                while (ptGlobalFileInfo)
                {
                    if (ptGlobalFileInfo->ptArchive == ptArchive)
                    {
                        if (ptLastInfo)
                        {
                            ptLastInfo->ptNextInfo = ptGlobalFileInfo->ptNextInfo;
                        }
                        else if (ptGlobalFileInfo->ptNextInfo)
                        {










                            stGlobalFileMap.SetAt_Tracked(info.dwHash, ptGlobalFileInfo->ptNextInfo);
                        }
                        else
                        {
                            stGlobalFileMap.RemoveAt(info.dwHash);
                        }
                        break;
                    }
                    ptLastInfo = ptGlobalFileInfo;
                    ptGlobalFileInfo = ptGlobalFileInfo->ptNextInfo;
                }
            }
        }
    }

    pos = stGlobalFileInfoBase.GetHeadPosition();
    while (pos)
    {
        POS oldPos = pos;
        GlobalFileInfoBase * ptGlobalFileInfoBase = stGlobalFileInfoBase.GetNext(pos);
        if (ptGlobalFileInfoBase->ptArchive == ptArchive)
        {
            delete[] ptGlobalFileInfoBase->ptBase;
            delete ptGlobalFileInfoBase;
            stGlobalFileInfoBase.RemoveAt(oldPos);
            break;
        }
    }
}

static void sAddArchiveToGlobalList(CPKArchiveReader * ptArchive)
{
    BOOL fInserted = FALSE;
    Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);

    int32 nArchivePriority = ptArchive->GetPriority();
    {
        POS pos = stOpenedArchives.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            CPKArchiveReader * ptArchiveItem = stOpenedArchives.GetNext(pos);

            int32 nPriorityItem = ptArchiveItem->GetPriority();
            if (nArchivePriority >= nPriorityItem)
            {
                stOpenedArchives.InsertBefore_Tracked(posLast, ptArchive);
                fInserted = TRUE;
                break;
            }
        }
    }

    if (!fInserted)
    {
        stOpenedArchives.AddTail_Tracked(ptArchive);
    }

    int32 nFileCount = ptArchive->GetFileCount();
    GlobalFileInfoBase * ptGlobalFileInfoBase = new GlobalFileInfoBase;
    ptGlobalFileInfoBase->ptArchive = ptArchive;
    ptGlobalFileInfoBase->ptBase = new GlobalFileInfo[nFileCount];
    stGlobalFileInfoBase.AddTail_Tracked(ptGlobalFileInfoBase);

    for (int32 i = 0; i < nFileCount; i++)
    {
        FileInfo info;
        if (ptArchive->GetFileInfo(i, info))
        {
            GlobalFileInfo * ptLastInfo = NULL;
            GlobalFileInfo * ptGlobalFileInfo;
            GlobalFileInfo * ptMyGlobalFileInfo = &ptGlobalFileInfoBase->ptBase[i];
            ptMyGlobalFileInfo->ptArchive = ptArchive;
            ptMyGlobalFileInfo->info = info;
            ptMyGlobalFileInfo->ptNextInfo = NULL;


            if (!stGlobalFileMap.Lookup(info.dwHash, ptGlobalFileInfo))
            {
                stGlobalFileMap.SetAt_Tracked(info.dwHash, ptMyGlobalFileInfo);
            }
            else
            {
                while (ptGlobalFileInfo)
                {
                    if (ptGlobalFileInfo->ptArchive->GetPriority() <= nArchivePriority)
                        break;
                    ptLastInfo = ptGlobalFileInfo;
                    ptGlobalFileInfo = ptGlobalFileInfo->ptNextInfo;
                }

                if (ptLastInfo == NULL)
                {
                    ptMyGlobalFileInfo->ptNextInfo = ptGlobalFileInfo;
                    stGlobalFileMap.SetAt_Tracked(info.dwHash, ptMyGlobalFileInfo);
                }
                else
                {
                    ptMyGlobalFileInfo->ptNextInfo = ptLastInfo->ptNextInfo;
                    ptLastInfo->ptNextInfo = ptMyGlobalFileInfo;
                }
            }
        }
    }
}

Results OpenArchive(const char * szFilename, int32 nArchivePriority, BOOL fIsHotfix, BOOL fValidateCRC, CPKArchiveReader ** pptArchive, TaskPriorities nTaskPriority)
{
    Results nResult = IORESULT_SUCCESS;
    CPKArchiveReader * ptArchive = NULL;

    {







        Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);
        {
            POS pos = stOpenedArchives.GetHeadPosition();
            while (pos)
            {
                CPKArchiveReader * ptArchiveItem = stOpenedArchives.GetNext(pos);
                if (strcasecmp(ptArchiveItem->GetArchiveName(), szFilename) == 0)
                {
                    if (ptArchiveItem->IsClosing())
                    {
                        ptArchiveItem->SetClosing(FALSE);
                        sRemoveArchiveFromGlobalList(ptArchiveItem);
                        ptArchive = ptArchiveItem;
                        break;
                    }
                }
            }
        }
    }

    if (ptArchive == NULL)
    {
        OpenArchiveTask * ptTask = new OpenArchiveTask;
        ptTask->Initialize(szFilename, fIsHotfix, fValidateCRC, nTaskPriority);
        GetIOAccessThread()->SubmitTask(ptTask);
        ptTask->WaitForCompletion();
        nResult = ptTask->GetResults();

        if (ptTask->IsFailed())
        {
            ptTask->Release();
            return nResult;
        }

        ptArchive = ptTask->GetArchive();
        ptTask->Release();
    }

    ptArchive->SetPriority(nArchivePriority);
    sAddArchiveToGlobalList(ptArchive);

    *pptArchive = ptArchive;
    return nResult;
}




Results OpenArchiveFromMemory(const char * szFilename, const uint8 * pArchiveData, int32 nArchiveSize, int32 nArchivePriority, BOOL fValidateCRC, CPKArchiveReader ** pptArchive, TaskPriorities nTaskPriority)
{
    Results nResult;
    OpenArchiveFromMemoryTask * ptTask;
    CPKArchiveReader * ptArchive;

    ptTask = new OpenArchiveFromMemoryTask;
    ptTask->Initialize(szFilename, pArchiveData, nArchiveSize, fValidateCRC, nTaskPriority);






    GetIOAccessThread()->SubmitTask(ptTask);
    ptTask->WaitForCompletion();
    nResult = ptTask->GetResults();

    if (ptTask->IsFailed())
    {
        ptTask->Release();
        return nResult;
    }

    ptArchive = ptTask->GetArchive();
    ptTask->Release();

    ptArchive->SetPriority(nArchivePriority);
    sAddArchiveToGlobalList(ptArchive);

    *pptArchive = ptArchive;
    return nResult;
}




Results ChangeArchivePriority(CPKArchiveReader * ptArchive, int32 nArchivePriority)
{
    if (ptArchive == NULL)
        return IORESULT_FAILED;

    sRemoveArchiveFromGlobalList(ptArchive);
    ptArchive->SetPriority(nArchivePriority);
    sAddArchiveToGlobalList(ptArchive);

    return IORESULT_SUCCESS;
}













































Results CloseArchive(CPKArchiveReader * ptArchive, TaskPriorities nTaskPriority)
{
    CloseArchiveTask * ptTask;
    Results nResult;

    if (ptArchive == NULL)
        return IORESULT_FAILED;


    if (ptArchive->GetOpenedFileRecordCount() > 0)
    {
        ptArchive->SetClosing(TRUE);
        return IORESULT_SUCCESS;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);


    POS pos = stAllocatedInternalBuffer.GetHeadPosition();
    while (pos)
    {
        InternalBuffer * ptBufferInList = stAllocatedInternalBuffer.GetNext(pos);
        if (ptBufferInList->ptLastArchive == ptArchive)
        {
            ptBufferInList->nOffset = -1;
            ptBufferInList->ptLastArchive = NULL;
        }
    }

    sRemoveArchiveFromGlobalList(ptArchive);

    ptTask = new CloseArchiveTask;
    ptTask->Initialize(ptArchive, nTaskPriority);
    GetIOAccessThread()->SubmitTask(ptTask);
    ptTask->WaitForCompletion();
    nResult = ptTask->GetResults();

    if (ptTask->IsFailed())
    {
    }

    ptTask->Release();

    return nResult;
}



BOOL GetFilename(uint64 dwHash, char * szFilenameBuffer, int32 nFilenameBufferSize)
{
    CPKArchiveReader * ptArchive = FindArchiveContaining(dwHash, NULL);
    if (ptArchive == NULL)
        return FALSE;






    return ptArchive->GetFilename(dwHash, szFilenameBuffer, nFilenameBufferSize);
}









CPKArchiveReader * FindArchiveContaining(const char * szFilename, FileInfo * ptFileInfo)
{
    return FindArchiveContaining(CPKArchive::GetFileHash(szFilename), ptFileInfo);
}




CPKArchiveReader * sFindArchiveContainingInternal(uint64 dwHash, FileInfo * ptFileInfo)
{
    GlobalFileInfo * ptGlobalFileInfo;
    CPKArchiveReader * ptArchive = NULL;



    if (stGlobalFileMap.Lookup(dwHash, ptGlobalFileInfo))
    {
        if (ptFileInfo)
        {
            *ptFileInfo = ptGlobalFileInfo->info;
        }
        ptArchive = ptGlobalFileInfo->ptArchive;
    }
    return ptArchive;
}

















uint64 GetLocaleOverrideHash(uint64 dwFileHash)
{
    static XLocale sLastLocale = NUM_XLOCALES;

    static blz::string sLocalePrefix;

    if (sLastLocale != UnicodeText::GetCurrentLocale())
    {
        sLocalePrefix.format("|%s", XLocaleToString(UnicodeText::GetCurrentLocale()));
        blz::transform(sLocalePrefix.begin(), sLocalePrefix.end(), sLocalePrefix.begin(), tolower);
        sLastLocale = UnicodeText::GetCurrentLocale();
    }

    uint64 dwHash = Blizzard::Util::Hash64More(sLocalePrefix.c_str(), sLocalePrefix.length(), dwFileHash);
    return dwHash;
}


uint64 GetTextLocaleOverrideHash(uint64 dwFileHash)
{
    static blz::recursive_mutex sMutex;
    static XLocale sLastLocale = NUM_XLOCALES;

    static blz::string sLocalePrefix;

    sMutex.lock();
    if (sLastLocale != UnicodeText::GetStringDisplayLocale())
    {
        sLocalePrefix.format("|%s", XLocaleToString(UnicodeText::GetStringDisplayLocale()));
        blz::transform(sLocalePrefix.begin(), sLocalePrefix.end(), sLocalePrefix.begin(), tolower);
        sLastLocale = UnicodeText::GetStringDisplayLocale();
    }
    uint64 dwHash = Blizzard::Util::Hash64More(sLocalePrefix.c_str(), sLocalePrefix.length(), dwFileHash);
    sMutex.unlock();
    return dwHash;
}




CPKArchiveReader * FindArchiveContaining(uint64 dwHash, FileInfo * ptFileInfo)
{
    uint64 dwLocaleOverrideHash;
    CPKArchiveReader * ptArchive;
    Blizzard::Lock::ScopedMutex scopedMutex(stConsoleIOMutex);

    dwLocaleOverrideHash = GetTextLocaleOverrideHash(dwHash);

    ptArchive = sFindArchiveContainingInternal(dwLocaleOverrideHash, ptFileInfo);
    if (ptArchive)
        return ptArchive;

    if (UnicodeText::GetStringDisplayLocale() != UnicodeText::GetCurrentLocale())
    {
        uint64 dwLocaleOverrideHash = GetLocaleOverrideHash(dwHash);
        ptArchive = sFindArchiveContainingInternal(dwLocaleOverrideHash, ptFileInfo);
        if (ptArchive)
            return ptArchive;
    }
    ptArchive = sFindArchiveContainingInternal(dwHash, ptFileInfo);
    return ptArchive;
}


BOOL InternalBuffer::Contains(FileRecord * ptRecord, int32 nFileOffset, uint8 *& pValidData, int32 & nValidDataSize)
{
    if (nOffset == -1)
        return FALSE;

    CPKArchiveReader * ptArchive = ptLastArchive;
    if (ptArchive != ptRecord->ptArchive)
        return FALSE;

    FileInfo & info = ptRecord->info;
    int32 nLocationCount = info.nLocationCount;
    for (int32 nLocation = 0; nLocation < nLocationCount; nLocation++)
    {
        int64 nArchiveOffset = ptArchive->GetLocation(info.nLocationIndex + nLocation) + nFileOffset;
        if (nArchiveOffset >= nOffset && nOffset + nSize > nArchiveOffset)
        {
            int32 nInternalSubOffset = (int32)(nArchiveOffset - nOffset);
            nValidDataSize = nSize - nInternalSubOffset;
            pValidData = pBuffer + nInternalSubOffset;
            return TRUE;
        }
    }
    return FALSE;
}















void ReadBuffer::Copy(const ReadBuffer & readBuffer)
{
    SigmaMemoryCopy(m_pBuffer, readBuffer.m_pBuffer, readBuffer.m_nReadSize);
    m_ptArchiveReader = readBuffer.m_ptArchiveReader;
    m_nSector = readBuffer.m_nSector;
    m_nReadSize = readBuffer.m_nReadSize;
}



void DecompBuffer::Copy(const DecompBuffer & decompBuffer)
{
    SigmaMemoryCopy(m_pBuffer, decompBuffer.m_pBuffer, decompBuffer.m_nDecompSize);
    m_ptArchiveReader = decompBuffer.m_ptArchiveReader;
    m_nCompSector = decompBuffer.m_nCompSector;
    m_nSubOffset = decompBuffer.m_nSubOffset;
    m_nDecompSize = decompBuffer.m_nDecompSize;
}

}  // namespace IO
}  // namespace Console
