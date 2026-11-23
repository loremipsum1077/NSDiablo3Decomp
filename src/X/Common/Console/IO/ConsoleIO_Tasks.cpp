// Original file: X/Common/Console/IO/ConsoleIO_Tasks.cpp

#include "ConsoleIO_Tasks.h"

#include <string.h>

#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "ConsoleIO_ArchiveReader.h"
#include "ConsoleIO_TaskThread.h"

namespace Console {
namespace IO {

Task::Task()
    : m_nState(TASKSTATE_CREATED),
      m_nResult(IORESULT_SUCCESS),
      m_ptNext(NULL),
      m_ptArchive(NULL),
      m_ptRecord(NULL),
      m_nOffset(0),
      m_dwPriority(0),
      m_nReadSector(-1),
      m_nBytesRead(0),
      m_nRefCount(1),
      m_nNextCompSector(-1),
      m_fReadFromFileSystem(FALSE),
      m_fShouldDecompressAhead(FALSE),
      m_fShouldReadAhead(FALSE),
      m_fInitialized(FALSE)
{
    ThreadGlobals * ptThreadGlobals = ThreadGlobals::TryToGet();
    m_nCreatorThread = ptThreadGlobals ? ptThreadGlobals->m_nThreadIndex : THREADINDEX_COUNT;
}

void Task::Initialize(CPKArchiveReader * ptArchive, FileRecord * ptRecord, int32 nOffset, uint32 dwPriority)
{
    if (m_fInitialized)
    {
        m_tTaskCompleted.Wait(0);
    }

    m_nState = TASKSTATE_CREATED;
    m_nResult = IORESULT_SUCCESS;
    m_nReadSector = -1;
    m_nBytesRead = 0;
    m_ptNext = NULL;
    m_ptArchive = ptArchive;
    m_ptRecord = ptRecord;
    m_dwPriority = dwPriority;
    m_nOffset = nOffset;
    m_nNextCompSector = -1;
    m_fReadFromFileSystem = FALSE;
    m_fShouldDecompressAhead = FALSE;
    m_fShouldReadAhead = FALSE;
    m_fInitialized = TRUE;
}

void Task::SetSubmitted()
{
    BOOL fSuccess = ChangeState(TASKSTATE_CREATED, TASKSTATE_SUBMITTED);

    m_fReadFromFileSystem = FALSE;
    m_fShouldDecompressAhead = FALSE;
    m_fShouldReadAhead = FALSE;
}

void Task::SetCompleted(TaskThread * ptTaskThread)
{
    BOOL fSuccess = ChangeState(TASKSTATE_IN_PROGRESS, TASKSTATE_COMPLETED);
    if (!fSuccess)
    {
        fSuccess = ChangeState(TASKSTATE_PROCESSING_OUTPUT_BUFFER, TASKSTATE_COMPLETED);
    }

    if (fSuccess)
    {
        m_nResult = IORESULT_SUCCESS;
    }
    else
    {
        m_nResult = IORESULT_CANCELED;
    }

    m_tTaskCompleted.Set();
}

BOOL Task::SetInProgress()
{
    BOOL fSuccess = ChangeState(TASKSTATE_SUBMITTED, TASKSTATE_IN_PROGRESS);
    return fSuccess;
}

void Task::SetFailed(Results nResult)
{
    BOOL fSuccess = ChangeState(TASKSTATE_IN_PROGRESS, TASKSTATE_FAILED);
    if (!fSuccess)
    {
        fSuccess = ChangeState(TASKSTATE_PROCESSING_OUTPUT_BUFFER, TASKSTATE_FAILED);
    }
    if (!fSuccess)
    {
        fSuccess = ChangeState(TASKSTATE_SUBMITTED, TASKSTATE_FAILED);
    }

    if (fSuccess)
    {
        m_nResult = nResult;
    }
    else if (IsCanceled())
    {
        m_nResult = IORESULT_CANCELED;
    }

    m_tTaskCompleted.Set();
}

ParentTask::ParentTask()
    : m_nChildAndSelfCount(0),
      m_nChildResult(IORESULT_SUCCESS),
      m_fHasAnyChild(FALSE),
      m_fAnyChildFailed(FALSE)
{
}

void ParentTask::Initialize(CPKArchiveReader * ptArchive, FileRecord * ptRecord, int32 nOffset, uint32 dwPriority)
{
    if (m_fInitialized)
    {
        m_tChildTasksCompleted.Wait(0);
    }

    Task::Initialize(ptArchive, ptRecord, nOffset, dwPriority);

    m_nChildAndSelfCount = 1;
    m_fAnyChildFailed = FALSE;
    m_nChildResult = IORESULT_SUCCESS;
    m_fHasAnyChild = FALSE;
}

void ParentTask::SetCompleted(TaskThread * ptTaskThread)
{
    if (Blizzard::Lock::Atomic::Decrement(&m_nChildAndSelfCount) == 0)
    {
        Task::SetCompleted(ptTaskThread);
    }
}

void ParentTask::SetFailed(Results nResult)
{
    if (Blizzard::Lock::Atomic::Decrement(&m_nChildAndSelfCount) == 0)
    {
        Task::SetFailed(nResult);
    }
    else
    {
        m_nResult = nResult;
    }
}

void ParentTask::OnChildCompleted(ChildTask * ptChild, TaskThread * ptTaskThread)
{
    int32 nChildAndSelfCount;

    nChildAndSelfCount = Blizzard::Lock::Atomic::Decrement(&m_nChildAndSelfCount);
    if (nChildAndSelfCount != 0)
    {
        if (nChildAndSelfCount == 1)
        {
            m_tChildTasksCompleted.Set();
        }
        return;
    }

    {
        Results nResult = m_nResult;
        m_fHasAnyChild = FALSE;

        if (nResult != IORESULT_SUCCESS)
        {
            Task::SetFailed(nResult);
        }
        else if (HasAnyChildFailed(nResult))
        {
            Task::SetFailed(nResult);
        }
        else
        {
            Task::SetCompleted(ptTaskThread);
        }
    }
}

void ParentTask::OnChildFailed(ChildTask * ptChild, Results nResult)
{
    int32 nChildAndSelfCount;

    m_nChildResult = nResult;
    m_fAnyChildFailed = TRUE;

    nChildAndSelfCount = Blizzard::Lock::Atomic::Decrement(&m_nChildAndSelfCount);
    if (nChildAndSelfCount != 0)
    {
        if (nChildAndSelfCount == 1)
        {
            m_tChildTasksCompleted.Set();
        }
        return;
    }

    m_fHasAnyChild = FALSE;

    if (m_nResult != IORESULT_SUCCESS)
    {
        Task::SetFailed(m_nResult);
    }
    else
    {
        Task::SetFailed(nResult);
    }
}

void ParentTask::SetChildCount(int32 nChildCount)
{
    m_nChildAndSelfCount = nChildCount + 1;
    m_fHasAnyChild = nChildCount > 0;
}

void ParentTask::WaitForChildCompletion()
{
    int32 nChildAndSelfCount;

    if (m_fHasAnyChild)
    {
        m_tChildTasksCompleted.WaitForever();
        nChildAndSelfCount = m_nChildAndSelfCount;
        m_fHasAnyChild = FALSE;
    }
}

ChildTask::ChildTask()
    : m_ptParent(NULL)
{
}

void ChildTask::Initialize(CPKArchiveReader * ptArchive, FileRecord * ptRecord, int32 nOffset, uint32 dwPriority, ParentTask * ptParent)
{
    Task::Initialize(ptArchive, ptRecord, nOffset, dwPriority);

    ptParent->AddRef();
    m_ptParent = ptParent;
}

void ChildTask::SetCompleted(TaskThread * ptTaskThread)
{
    Task::SetCompleted(ptTaskThread);

    if (m_ptParent)
    {
        m_ptParent->OnChildCompleted(this, ptTaskThread);
        m_ptParent->Release();
    }
}

void ChildTask::SetFailed(Results nResult)
{
    Task::SetFailed(nResult);

    if (m_ptParent)
    {
        m_ptParent->OnChildFailed(this, nResult);
        m_ptParent->Release();
    }
}

OpenArchiveTask::OpenArchiveTask()
{
    m_szFilename[0] = '\0';
    m_fIsHotfix = FALSE;
}

void OpenArchiveTask::Initialize(const char * szFilename, BOOL fIsHotfix, BOOL fValidateCRC, uint32 dwPriority)
{
    Task::Initialize(NULL, NULL, 0, dwPriority);

    strncpy(m_szFilename, szFilename, sizeof(m_szFilename) - 1);
    m_fIsHotfix = fIsHotfix;
    m_fValidateCRC = fValidateCRC;
}

Results OpenArchiveTask::Process(TaskThread * ptTaskThread)
{
    Results nResult = IORESULT_SUCCESS;

    m_ptArchive = new CPKArchiveReader(m_szFilename, ((IOAccessTaskThread *)ptTaskThread)->GetReadBuffer(), FALSE, m_fIsHotfix, m_fValidateCRC, CPKArchiveReader::CPK_ARCHIVE_TYPE_STANDARD);

    if (!m_ptArchive->IsValid())
    {
        nResult = m_ptArchive->GetResult();
        ::delete m_ptArchive;
        m_ptArchive = NULL;
    }

    return nResult;
}

OpenArchiveFromMemoryTask::OpenArchiveFromMemoryTask()
{
    m_szFilename[0] = '\0';
    m_pArchiveData = NULL;
    m_nArchiveSize = 0;
    m_fValidateCRC = FALSE;
}

void OpenArchiveFromMemoryTask::Initialize(const char * szFilename, const uint8 * pArchiveData, int32 nArchiveSize, BOOL fValidateCRC, uint32 dwPriority)
{
    Task::Initialize(NULL, NULL, 0, dwPriority);

    strncpy(m_szFilename, szFilename, sizeof(m_szFilename) - 1);
    m_pArchiveData = pArchiveData;
    m_nArchiveSize = nArchiveSize;
    m_fValidateCRC = fValidateCRC;
}

Results OpenArchiveFromMemoryTask::Process(TaskThread * ptTaskThread)
{
    Results nResult = IORESULT_SUCCESS;

    m_ptArchive = new CPKArchiveReader(m_szFilename, m_pArchiveData, m_nArchiveSize, m_fValidateCRC);

    if (!m_ptArchive->IsValid())
    {
        nResult = m_ptArchive->GetResult();
        ::delete m_ptArchive;
        m_ptArchive = NULL;
    }

    return nResult;
}

CloseArchiveTask::CloseArchiveTask()
{
}

void CloseArchiveTask::Initialize(CPKArchiveReader * ptArchive, uint32 dwPriority)
{
    Task::Initialize(ptArchive, NULL, -1, dwPriority);
}

Results CloseArchiveTask::Process(TaskThread * ptTaskThread)
{
    ptTaskThread->OnArchiveClose(m_ptArchive);

    if (m_ptArchive)
    {
        ::delete m_ptArchive;
    }

    return IORESULT_SUCCESS;
}

ReadRecordTask::ReadRecordTask()
    : m_nOffsetInFirstSector(0),
      m_nReadSectorCount(0),
      m_nSizeInLastSector(0),
      m_pDest(NULL),
      m_nSize(0)
{
}

void ReadRecordTask::Initialize(FileRecord * ptRecord, int32 nOffset, int32 nSize, void * pDest, uint32 dwPriority)
{
    Task::Initialize(ptRecord->ptArchive, ptRecord, nOffset, dwPriority);

    m_pDest = (uint8 *)pDest;
    m_nSize = nSize;
}

void ReadRecordTask::ComputeBestReadSector(CPKArchiveReader * ptLastArchive, int32 nLastSector)
{
    if (m_ptRecord)
    {
        m_ptArchive->FindBestReadSectorInfo(ptLastArchive, nLastSector, m_ptRecord, m_nOffset, m_nSize, m_nReadSector, m_nReadSectorCount, m_nOffsetInFirstSector, m_nSizeInLastSector);
    }
}

Results ReadRecordTask::Process(TaskThread * ptTaskThread)
{
    ReadBuffer & readBuffer = ((IOAccessTaskThread *)ptTaskThread)->GetReadBuffer();
    ReadBuffer & readAheadBuffer = ((IOAccessTaskThread *)ptTaskThread)->GetReadAheadBuffer();
    BOOL fSuccess;

    if (!readBuffer.Contains(m_ptArchive, m_nReadSector))
    {
        if (readAheadBuffer.Contains(m_ptArchive, m_nReadSector))
        {
            readBuffer.Copy(readAheadBuffer);
            m_fShouldReadAhead = TRUE;
            readAheadBuffer.SetInvalid();
        }
        else
        {
            fSuccess = m_ptArchive->ReadSector(readBuffer, m_nReadSector, &m_fReadFromFileSystem, &m_fShouldReadAhead, m_nCreatorThread);
            if (!fSuccess)
            {
                return IORESULT_FILE_IO_ERROR;
            }
        }
    }

    if (ChangeState(TASKSTATE_IN_PROGRESS, TASKSTATE_PROCESSING_OUTPUT_BUFFER))
    {
        if (m_nReadSectorCount == 1)
        {
            SigmaMemoryCopy(m_pDest, readBuffer.GetReadBuffer() + m_nOffsetInFirstSector, Math::Min(m_nSizeInLastSector, readBuffer.GetReadSize()) - m_nOffsetInFirstSector);
            m_nBytesRead += Math::Min(m_nSizeInLastSector, readBuffer.GetReadSize()) - m_nOffsetInFirstSector;
        }
        else
        {
            SigmaMemoryCopy(m_pDest, readBuffer.GetReadBuffer() + m_nOffsetInFirstSector, Math::Min(m_ptArchive->GetReadSectorSize(), readBuffer.GetReadSize()) - m_nOffsetInFirstSector);
            m_nBytesRead += Math::Min(m_ptArchive->GetReadSectorSize(), readBuffer.GetReadSize()) - m_nOffsetInFirstSector;
        }

        if (ChangeState(TASKSTATE_PROCESSING_OUTPUT_BUFFER, TASKSTATE_IN_PROGRESS))
        {
            return IORESULT_SUCCESS;
        }
    }

    return IORESULT_CANCELED;
}

void ReadRecordTask::SetCompleted(TaskThread * ptTaskThread)
{
    if (m_nReadSectorCount > 1)
    {
        BOOL fSuccess = ChangeState(TASKSTATE_IN_PROGRESS, TASKSTATE_CREATED);
        if (fSuccess)
        {
            int32 nDataCompleted = m_ptArchive->GetReadSectorSize() - m_nOffsetInFirstSector;

            m_nOffsetInFirstSector = 0;
            m_nReadSectorCount--;
            m_pDest += nDataCompleted;
            m_nOffset += nDataCompleted;
            m_nSize -= nDataCompleted;

            ptTaskThread->SubmitTask(this);
            return;
        }

        Task::SetCompleted(ptTaskThread);
    }
    else
    {
        InternalBuffer * ptInternalBuffer;
        ReadBuffer & readBuffer = ((IOAccessTaskThread *)ptTaskThread)->GetReadBuffer();
        int32 nLastReadSize;

        ptInternalBuffer = m_ptRecord->ptInternalBuffer;
        nLastReadSize = Math::Min(readBuffer.GetReadSize(), ptInternalBuffer->nCapacity);

        SigmaMemoryCopy(ptInternalBuffer->pBuffer, readBuffer.GetReadBuffer(), nLastReadSize);
        ptInternalBuffer->nOffset = (int64)(m_nReadSector - m_ptArchive->GetHeaderReadSectorCount()) * m_ptArchive->GetReadSectorSize();
        ptInternalBuffer->nSize = nLastReadSize;

        Task::SetCompleted(ptTaskThread);
    }
}

DecompressSectorTask::DecompressSectorTask()
    : m_nCompSector(0),
      m_nCompSize(0),
      m_nDecompSize(0),
      m_pDest(NULL),
      m_ptRecordToUpdate(NULL)
{
    m_pSource = new uint8[CPK_COMP_SECTOR_SIZE];
}

DecompressSectorTask::~DecompressSectorTask()
{
    delete[] m_pSource;
}

void DecompressSectorTask::Initialize(CPKArchiveReader * ptArchive, int32 nCompSector, const void * pSource, void * pDest, int32 nOffset, int32 nCompSize, int32 nDecompSize, uint32 dwPriority, ParentTask * ptParent, FileRecord * ptRecordToUpdate)
{
    ChildTask::Initialize(ptArchive, NULL, nOffset, dwPriority, ptParent);

    m_nCompSector = nCompSector;
    m_nCompSize = nCompSize;
    m_nDecompSize = nDecompSize;
    SigmaMemoryCopy(m_pSource, pSource, nCompSize);
    m_pDest = (uint8 *)pDest;
    m_ptRecordToUpdate = ptRecordToUpdate;
}

Results DecompressSectorTask::Process(TaskThread * ptTaskThread)
{
    int32 nRemainingSizeToDecompress;
    int32 nEndOfDest;
    DecompressionTaskThread * ptDecompTaskThread = (DecompressionTaskThread *)ptTaskThread;
    DecompBuffer & decompBuffer = ptDecompTaskThread->GetDecompBuffer();
    DecompBuffer & decompAheadBuffer = ptDecompTaskThread->GetDecompAheadBuffer();
    int32 nCurrentOut;

    nCurrentOut = m_nOffset + m_nDecompSize;
    nRemainingSizeToDecompress = m_ptArchive->GetDecompressedSectorSize(m_nCompSector) - nCurrentOut;
    nEndOfDest = m_nOffset + m_nDecompSize;

    if (!decompBuffer.Contains(m_ptArchive, m_nCompSector, m_nOffset))
    {
        if (!decompAheadBuffer.Contains(m_ptArchive, m_nCompSector, m_nOffset))
        {
            if (!ptDecompTaskThread->DecompressSector(m_ptArchive, m_nCompSector, m_pSource, m_nCompSize, m_nOffset, m_nDecompSize, m_pDest, nRemainingSizeToDecompress))
            {
                return IORESULT_COMPRESSION_ERROR;
            }
        }
        else
        {
            int32 nValidDataSize;

            decompBuffer.Copy(decompAheadBuffer);
            decompAheadBuffer.SetInvalid();

            nValidDataSize = Math::Min(decompBuffer.GetDecompSize() - (m_nOffset - decompBuffer.GetSubOffset()), m_nDecompSize);
            SigmaMemoryCopy(m_pDest, decompBuffer.GetDecompBuffer() + m_nOffset - decompBuffer.GetSubOffset(), nValidDataSize);
            m_pDest += nValidDataSize;
            m_nDecompSize -= nValidDataSize;
            m_nOffset += nValidDataSize;

            if (m_nDecompSize > 0)
            {
                if (!ptDecompTaskThread->DecompressSector(m_ptArchive, m_nCompSector, m_pSource, m_nCompSize, m_nOffset, m_nDecompSize, m_pDest, nRemainingSizeToDecompress))
                {
                    return IORESULT_COMPRESSION_ERROR;
                }
            }
        }
    }
    else
    {
        int32 nValidDataSize = Math::Min(decompBuffer.GetDecompSize() - (m_nOffset - decompBuffer.GetSubOffset()), m_nDecompSize);

        SigmaMemoryCopy(m_pDest, decompBuffer.GetDecompBuffer() + m_nOffset - decompBuffer.GetSubOffset(), nValidDataSize);
        m_pDest += nValidDataSize;
        m_nDecompSize -= nValidDataSize;
        m_nOffset += nValidDataSize;

        if (m_nDecompSize > 0)
        {
            if (decompAheadBuffer.Contains(m_ptArchive, m_nCompSector, m_nOffset))
            {
                decompBuffer.Copy(decompAheadBuffer);
                decompAheadBuffer.SetInvalid();

                nValidDataSize = Math::Min(decompBuffer.GetDecompSize() - (m_nOffset - decompBuffer.GetSubOffset()), m_nDecompSize);
                SigmaMemoryCopy(m_pDest, decompBuffer.GetDecompBuffer() + m_nOffset - decompBuffer.GetSubOffset(), nValidDataSize);
                m_pDest += nValidDataSize;
                m_nDecompSize -= nValidDataSize;
                m_nOffset += nValidDataSize;

                if (m_nDecompSize > 0)
                {
                    if (!ptDecompTaskThread->DecompressSector(m_ptArchive, m_nCompSector, m_pSource, m_nCompSize, m_nOffset, m_nDecompSize, m_pDest, nRemainingSizeToDecompress))
                    {
                        return IORESULT_COMPRESSION_ERROR;
                    }
                }
            }
            else if (!ptDecompTaskThread->DecompressSector(m_ptArchive, m_nCompSector, m_pSource, m_nCompSize, m_nOffset, m_nDecompSize, m_pDest, nRemainingSizeToDecompress))
            {
                return IORESULT_COMPRESSION_ERROR;
            }
        }
    }

    if (m_ptRecordToUpdate)
    {
        InternalBuffer * ptInternalBuffer = m_ptRecordToUpdate->ptInternalBuffer;
        int32 nAdjustedOffset = nEndOfDest - decompBuffer.GetSubOffset();
        int32 nAdjustedDecompSize = decompBuffer.GetDecompSize() - nAdjustedOffset;
        int32 nRemainingSize = Math::Min(nAdjustedDecompSize, ptInternalBuffer->nCapacity);
        DecompBuffer & scopedDecompBuffer = decompBuffer;

        if (nRemainingSize > 0)
        {
            SigmaMemoryCopy(ptInternalBuffer->pBuffer, scopedDecompBuffer.GetDecompBuffer() + nAdjustedOffset, nRemainingSize);
            ptInternalBuffer->nOffset = m_ptArchive->CompSectorToOffset(m_nCompSector) + scopedDecompBuffer.GetSubOffset() + nAdjustedOffset;
            ptInternalBuffer->nSize = nRemainingSize;
        }
        else
        {
            ptInternalBuffer->nSize = 0;
            ptInternalBuffer->nOffset = -1;
        }

        if (nRemainingSize == nAdjustedDecompSize && decompAheadBuffer.IsValid())
        {
            decompBuffer.Copy(decompAheadBuffer);
            decompAheadBuffer.SetInvalid();
        }
    }

    return IORESULT_SUCCESS;
}

AssignAheadCompSectorTask::AssignAheadCompSectorTask()
    : m_nCompSector(0),
      m_nCompSize(0)
{
    m_pSource = new uint8[CPK_COMP_SECTOR_SIZE];
}

AssignAheadCompSectorTask::~AssignAheadCompSectorTask()
{
    delete[] m_pSource;
}

void AssignAheadCompSectorTask::Initialize(CPKArchiveReader * ptArchive, int32 nCompSector, const void * pSource, int32 nCompSize, uint32 dwPriority)
{
    Task::Initialize(ptArchive, NULL, -1, dwPriority);

    m_nCompSector = nCompSector;
    m_nCompSize = nCompSize;
    SigmaMemoryCopy(m_pSource, pSource, nCompSize);
}

Results AssignAheadCompSectorTask::Process(TaskThread * ptTaskThread)
{
    DecompressionTaskThread * ptDecompTaskThread = (DecompressionTaskThread *)ptTaskThread;
    CompBuffer & compBuffer = ptDecompTaskThread->GetCompBuffer();

    if (!compBuffer.IsValid())
    {
        SigmaMemoryCopy(compBuffer.GetCompBuffer(), m_pSource, m_nCompSize);
        compBuffer.SetValid(m_ptArchive, m_nCompSector, m_nCompSize);
    }

    return IORESULT_SUCCESS;
}

AssignAheadReadSectorTask::AssignAheadReadSectorTask()
{
    m_tReadAheadSuggestion.nLastSector = -1;
    m_tReadAheadSuggestion.ptLastArchive = NULL;
    m_tReadAheadSuggestion.nNextSector = -1;
    m_tReadAheadSuggestion.ptNextArchive = NULL;
}

void AssignAheadReadSectorTask::Initialize(const ReadAheadSuggestion & tReadAheadSuggestion, uint32 dwPriority)
{
    Task::Initialize(NULL, NULL, -1, dwPriority);

    m_tReadAheadSuggestion = tReadAheadSuggestion;
}

Results AssignAheadReadSectorTask::Process(TaskThread * ptTaskThread)
{
    IOAccessTaskThread * ptIOAccessTaskThread = (IOAccessTaskThread *)ptTaskThread;

    ptIOAccessTaskThread->AssignAheadReadSector(m_tReadAheadSuggestion);

    return IORESULT_SUCCESS;
}

ReadAndDecompressRecordTask::ReadAndDecompressRecordTask()
    : m_nOffsetInFirstSector(0),
      m_nCompSectorOffsetInFirstReadSector(0),
      m_nCompSector(0),
      m_nCompSectorCount(0),
      m_nSizeInLastSector(0),
      m_pDest(NULL),
      m_nReadSectorCount(0),
      m_nLastBytesRead(0),
      m_nSize(0)
{
}

ReadAndDecompressRecordTask::~ReadAndDecompressRecordTask()
{
}

void ReadAndDecompressRecordTask::Initialize(FileRecord * ptRecord, int32 nOffset, int32 nSize, void * pDest, uint32 dwPriority)
{
    ParentTask::Initialize(ptRecord->ptArchive, ptRecord, nOffset, dwPriority);

    m_pDest = (uint8 *)pDest;
    m_nSize = nSize;
}

void ReadAndDecompressRecordTask::ComputeBestReadSector(CPKArchiveReader * ptLastArchive, int32 nLastSector)
{
    if (m_ptRecord)
    {
        m_ptArchive->FindBestReadAndCompSectorInfo(ptLastArchive, nLastSector, m_ptRecord, m_nOffset, m_nSize, m_nReadSector, m_nReadSectorCount, m_nOffsetInFirstSector, m_nSizeInLastSector, m_nCompSector, m_nCompSectorCount, m_nCompSectorOffsetInFirstReadSector);
    }
}

Results ReadAndDecompressRecordTask::Process(TaskThread * ptTaskThread)
{
    Results nResult;

    if (HasAnyChildFailed(nResult))
    {
        return nResult;
    }

    ReadBuffer & readBuffer = ((IOAccessTaskThread *)ptTaskThread)->GetReadBuffer();
    ReadBuffer & readAheadBuffer = ((IOAccessTaskThread *)ptTaskThread)->GetReadAheadBuffer();
    BOOL fSuccess;

    if (!readBuffer.Contains(m_ptArchive, m_nReadSector))
    {
        if (readAheadBuffer.Contains(m_ptArchive, m_nReadSector))
        {
            readBuffer.Copy(readAheadBuffer);
            m_fShouldReadAhead = TRUE;
            readAheadBuffer.SetInvalid();
        }
        else
        {
            fSuccess = m_ptArchive->ReadSector(readBuffer, m_nReadSector, &m_fReadFromFileSystem, &m_fShouldReadAhead, m_nCreatorThread);
            if (!fSuccess)
            {
                return IORESULT_FILE_IO_ERROR;
            }
        }
    }

    m_nLastBytesRead = readBuffer.GetReadSize();

    if (m_nLastBytesRead <= (Math::Min(m_nCompSectorCount, 4) - 1) * m_ptArchive->GetCompSectorSize())
    {
        return IORESULT_FILE_IO_ERROR;
    }

    if (!ChangeState(TASKSTATE_IN_PROGRESS, TASKSTATE_PROCESSING_OUTPUT_BUFFER))
    {
        return IORESULT_CANCELED;
    }

    WaitForChildCompletion();

    return IORESULT_SUCCESS;
}

void ReadAndDecompressRecordTask::SetCompleted(TaskThread * ptTaskThread)
{
    DecompressionTaskThread * ptDecompThread;
    ReadBuffer & readBuffer = ((IOAccessTaskThread *)ptTaskThread)->GetReadBuffer();
    int32 nCompSectorSize = m_ptArchive->GetCompSectorSize();
    const uint8 * ptCompSectors = readBuffer.GetReadBuffer() + m_nCompSectorOffsetInFirstReadSector * nCompSectorSize;

    ptDecompThread = GetDecompressionThread();

    if (m_nReadSectorCount > 1)
    {
        BOOL fSuccess = ChangeState(TASKSTATE_PROCESSING_OUTPUT_BUFFER, TASKSTATE_CREATED);
        if (fSuccess)
        {
            int32 nCompSectorCount = m_ptArchive->GetCompSectorPerReadSector();
            int32 nInternalOffset = 0;

            SetChildCount(nCompSectorCount - m_nCompSectorOffsetInFirstReadSector);

            for (int32 nCompSector = m_nCompSectorOffsetInFirstReadSector; nCompSector < nCompSectorCount; nCompSector++)
            {
                int32 nSectorSize;
                int32 nDataCompleted;
                DecompressSectorTask * ptDecomp;

                m_nCompSectorCount--;
                nSectorSize = m_ptArchive->GetDecompressedSectorSize(m_nCompSector);

                ptDecomp = &m_tDecompressTasks[nCompSector];
                ptDecomp->Initialize(m_ptArchive, m_nCompSector, ptCompSectors + nInternalOffset, m_pDest, m_nOffsetInFirstSector, Math::Min(nCompSectorSize, m_nLastBytesRead), nSectorSize - m_nOffsetInFirstSector, m_dwPriority, this, NULL);
                ptDecompThread->SubmitTask(ptDecomp);

                nDataCompleted = nSectorSize - m_nOffsetInFirstSector;
                m_nOffsetInFirstSector = 0;
                m_nBytesRead += nDataCompleted;
                m_pDest += nDataCompleted;
                m_nLastBytesRead -= nCompSectorSize;
                m_nOffset += nDataCompleted;
                m_nSize -= nDataCompleted;
                m_nCompSector++;
                nInternalOffset += nCompSectorSize;
            }

            m_nReadSectorCount--;
            m_nCompSectorOffsetInFirstReadSector = 0;

            ptTaskThread->SubmitTask(this);
            return;
        }

        ParentTask::SetCompleted(ptTaskThread);
        return;
    }

    if (!IsCanceled())
    {
        int32 nInternalOffset;
        int32 nCompSectorCount = m_nCompSectorCount;
        int32 nNextCompSector;

        SetChildCount(nCompSectorCount);
        nInternalOffset = m_nCompSectorOffsetInFirstReadSector;

        for (int32 nCompSector = 0; nCompSectorCount != 0; nCompSector++)
        {
            int32 nSectorSize;
            int32 nDataCompleted;
            BOOL fLastSubTask;
            DecompressSectorTask * ptDecomp;

            nCompSectorCount--;
            fLastSubTask = (nCompSectorCount == 0);

            if (fLastSubTask)
            {
                nSectorSize = m_nSizeInLastSector;
            }
            else
            {
                nSectorSize = m_ptArchive->GetDecompressedSectorSize(m_nCompSector);
            }

            ptDecomp = &m_tDecompressTasks[nInternalOffset + nCompSector];
            ptDecomp->Initialize(m_ptArchive, m_nCompSector, ptCompSectors + nCompSector * nCompSectorSize, m_pDest, m_nOffsetInFirstSector, Math::Min(nCompSectorSize, m_nLastBytesRead), nSectorSize - m_nOffsetInFirstSector, m_dwPriority, this, fLastSubTask ? m_ptRecord : NULL);
            ptDecompThread->SubmitTask(ptDecomp);

            nDataCompleted = nSectorSize - m_nOffsetInFirstSector;
            m_nOffsetInFirstSector = 0;
            m_nBytesRead += nDataCompleted;
            m_nLastBytesRead -= nCompSectorSize;
            m_pDest += nDataCompleted;
            m_nCompSector++;
        }

        nNextCompSector = m_nCompSector;
        if (nNextCompSector < m_ptArchive->GetCompSectorCount())
        {
            m_fShouldDecompressAhead = TRUE;
            m_nNextCompSector = nNextCompSector;
        }
    }

    ParentTask::SetCompleted(ptTaskThread);
}

}  // namespace IO
}  // namespace Console
