// Original file: X/Common/Console/IO/ConsoleIO_TaskThread.cpp

#include "ConsoleIO_TaskThread.h"

#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/SigmaProf.h"
#include "../../ThreadGlobals.h"
#include "ConsoleIO_ArchiveReader.h"
#include "ConsoleIO_Decompress.h"
#include "ConsoleIO_Tasks.h"

// Defined in X/Common/Shell/MainNX64.cpp:246; where the original declares it is not in the DWARF.
void ConfigureTaskThread(Blizzard::Thread::Thread tThread);

namespace Console {
namespace IO {

enum {
    PRIORITY_DELAY_HIGHEST = 0,
    PRIORITY_DELAY_LOWEST = 100,
};

TaskThread::TaskThread(const char * szName, Blizzard::Thread::EThreadPriority ePriority, int32 nProcessor)
    : m_ptFirstTask(NULL),
      m_fDone(FALSE),
      m_nCurrentPriorityWait(0)
{
    Blizzard::Thread::Create(sThreadProc, this, &m_tThread, szName, 1, 0x8000);
    Blizzard::Thread::SetPriority(m_tThread, ePriority);
    ConfigureTaskThread(m_tThread);
    SigmaProfRegisterThread(m_tThread, szName, 0x1000);
    Blizzard::Thread::Resume(m_tThread);
}

int32 TaskThread::GetWaitTimeForTaskPriority(TaskPriorities ePriority)
{
    int32 nMicroSecsPerTaskPrio = ((PRIORITY_DELAY_LOWEST - PRIORITY_DELAY_HIGHEST) * 1000) / (TASKPRIORITY_HIGHEST - TASKPRIORITY_LOWEST);
    int32 nMSTaskWait = (ePriority * nMicroSecsPerTaskPrio) / 1000;
    return nMSTaskWait;
}

void TaskThread::WaitForTask(uint32 nMaxWait)
{
    Blizzard::Time::Millisecond nStartMS = Blizzard::Time::Milliseconds();
    if (nMaxWait == 0)
    {
        m_tTaskKickEvent.WaitForever();
    }
    else
    {
        m_tTaskKickEvent.Wait(nMaxWait);
    }
    m_nCurrentPriorityWait -= Blizzard::Time::Milliseconds() - nStartMS;
    if (m_nCurrentPriorityWait < 0)
        m_nCurrentPriorityWait = 0;
}

TaskThread::~TaskThread()
{
    SigmaProfUnregisterThread(m_tThread);

    m_fDone = TRUE;

    m_tTaskKickEvent.Set();

    Blizzard::Thread::WaitForever(m_tThread);
    Blizzard::Thread::Close(m_tThread);
}

void TaskThread::SubmitTask(Task * ptTask)
{
    Blizzard::Lock::ScopedMutex mutex(m_tTaskListMutex);

    ptTask->SetNext(m_ptFirstTask);
    m_ptFirstTask = ptTask;

    ptTask->SetSubmitted();
    ptTask->AddRef();

    m_tTaskKickEvent.Set();
}

void TaskThread::OnArchiveClose(CPKArchiveReader * ptArchive)
{
}

DecompressionTaskThread::DecompressionTaskThread(const char * szName, Blizzard::Thread::EThreadPriority ePriority, int32 nProcessor)
    : TaskThread(szName, ePriority, nProcessor)
{
    SigmaMemoryClear(&m_tZStream, sizeof(m_tZStream));
    {
        int err = inflateInit(&m_tZStream);
    }
}

DecompressionTaskThread::~DecompressionTaskThread()
{
    inflateEnd(&m_tZStream);
}

BOOL DecompressionTaskThread::DecompressSector(CPKArchiveReader * ptArchive, int32 nCompSector, uint8 * pCompressedBuffer, int32 nCompressedSize, int32 nDecompressedOffset, int32 nDecompressedSize, uint8 * pDecompressedBuffer, int32 nRemainingSize)
{
    uint8 pSkipBuffer[8192];
    BOOL fDestinationIsWriteCombine = FALSE;

    m_compBuffer.SetInvalid();
    m_decompBuffer.SetInvalid();
    m_decompAheadBuffer.SetInvalid();

    int err = inflateReset(&m_tZStream);
    if (err != Z_OK)
        return FALSE;

    m_tZStream.next_in = pCompressedBuffer;
    m_tZStream.avail_in = nCompressedSize;

    uint32 dwCurrentPacketRemainingDecompSize = 0;
    BOOL fInsidePacketHeader = FALSE;

    if (!SkipDecompressData(&m_tZStream, nDecompressedOffset, pSkipBuffer, sizeof(pSkipBuffer), FALSE, dwCurrentPacketRemainingDecompSize, fInsidePacketHeader))
        return FALSE;

    if (!DecompressData(&m_tZStream, nDecompressedSize, pDecompressedBuffer, FALSE, dwCurrentPacketRemainingDecompSize, fInsidePacketHeader))
        return FALSE;

    if (nRemainingSize == 0 || dwCurrentPacketRemainingDecompSize == 0)
        return TRUE;

    int32 nDecompRemainingSize = Math::Min(nRemainingSize, (int32)CPK_MAX_DECOMP_BUFFER_SIZE);
    if (!DecompressData(&m_tZStream, nDecompRemainingSize, m_decompBuffer.GetDecompBuffer(), FALSE, dwCurrentPacketRemainingDecompSize, fInsidePacketHeader))
        return TRUE;

    m_decompBuffer.SetValid(ptArchive, nCompSector, nDecompressedOffset + nDecompressedSize, nDecompRemainingSize);

    nRemainingSize -= nDecompRemainingSize;
    if (nRemainingSize == 0 || dwCurrentPacketRemainingDecompSize == 0)
        return TRUE;

    int32 nDecompAheadRemainingSize = Math::Min(nRemainingSize, (int32)CPK_MAX_DECOMP_BUFFER_SIZE);
    if (!DecompressData(&m_tZStream, nDecompAheadRemainingSize, m_decompAheadBuffer.GetDecompBuffer(), FALSE, dwCurrentPacketRemainingDecompSize, fInsidePacketHeader))
        return TRUE;

    m_decompAheadBuffer.SetValid(ptArchive, nCompSector, nDecompressedOffset + nDecompressedSize + nDecompRemainingSize, nDecompAheadRemainingSize);

    nRemainingSize -= nDecompRemainingSize;
    if (nRemainingSize != 0)
    {
        SigmaMemoryCopy(m_compBuffer.GetCompBuffer(), pCompressedBuffer, nCompressedSize);
        m_compBuffer.SetValid(ptArchive, nCompSector, nCompressedSize);
    }

    return TRUE;
}




#line 587
BOOL DecompressionTaskThread::DecompressAheadSector()
{
    BOOL fFirstValid = m_decompBuffer.IsValid();
    DecompBuffer& curDecompBuffer = fFirstValid ? m_decompAheadBuffer : m_decompBuffer;

    int err = inflateReset(&m_tZStream);
    if (err != Z_OK)
    {
        m_compBuffer.SetInvalid();
        return FALSE;
    }

    m_tZStream.next_in = m_compBuffer.GetCompBuffer();
    m_tZStream.avail_in = m_compBuffer.GetCompSize();
    int32 nCompSector = m_compBuffer.GetCompSector();
    CPKArchiveReader * ptArchive = m_compBuffer.GetArchive();
    int32 nDecompressedSize = ptArchive->GetDecompressedSectorSize(nCompSector);

    uint32 dwCurrentPacketRemainingDecompSize = 0;
    BOOL fInsidePacketHeader = FALSE;
    int32 nSkippedSize = 0;
    if (fFirstValid && m_decompBuffer.GetCompSector() == nCompSector && m_decompBuffer.GetArchive() == ptArchive)
    {
        nSkippedSize = m_decompBuffer.GetSubOffset() + m_decompBuffer.GetDecompSize();
        nDecompressedSize -= nSkippedSize;
        if (nDecompressedSize == 0)
        {
            m_compBuffer.SetInvalid();
            return TRUE;
        }


        uint8 pSkipBuffer[8192];

        if (!SkipDecompressData(&m_tZStream, nSkippedSize, pSkipBuffer, sizeof(pSkipBuffer), FALSE, dwCurrentPacketRemainingDecompSize, fInsidePacketHeader))
        {
            m_decompBuffer.SetInvalid();
            return FALSE;
        }
    }



    int32 nDecompRemainingSize = Math::Min(nDecompressedSize, (int32)CPK_MAX_DECOMP_BUFFER_SIZE);

    if (!DecompressData(&m_tZStream, nDecompRemainingSize, curDecompBuffer.GetDecompBuffer(), FALSE, dwCurrentPacketRemainingDecompSize, fInsidePacketHeader))
    {
        m_compBuffer.SetInvalid();
        return FALSE;
    }






    curDecompBuffer.SetValid(ptArchive, nCompSector, nSkippedSize, nDecompRemainingSize);

    nDecompressedSize -= nDecompRemainingSize;

    if (nDecompressedSize == 0)
    {
        m_compBuffer.SetInvalid();
        return TRUE;
    }

    if (m_decompAheadBuffer.IsValid())
    {
        return TRUE;
    }


    int32 nDecompAheadRemainingSize = Math::Min(nDecompressedSize, (int32)CPK_MAX_DECOMP_BUFFER_SIZE);

    if (!DecompressData(&m_tZStream, nDecompAheadRemainingSize, m_decompAheadBuffer.GetDecompBuffer(), FALSE, dwCurrentPacketRemainingDecompSize, fInsidePacketHeader))
    {
        m_compBuffer.SetInvalid();
        return FALSE;
    }


    m_decompAheadBuffer.SetValid(ptArchive, nCompSector, nSkippedSize + nDecompRemainingSize, nDecompAheadRemainingSize);





    nDecompressedSize -= nDecompAheadRemainingSize;

    if (nDecompressedSize == 0)
    {
        m_compBuffer.SetInvalid();
    }

    return TRUE;
}
#line 235

#line 686
Task * DecompressionTaskThread::GetBestTask(
    uint32 & dwWaitForBestTask)
{
    Blizzard::Lock::ScopedMutex mutex(m_tTaskListMutex);




    uint32 dwHighestPriority = 0;
    Task * ptHighestPriorityTask = NULL;
    Task * ptPrevHighestPriorityTask = NULL;

    Task * ptCurrentTask = m_ptFirstTask;
    Task * ptPrevCurrentTask = NULL;
    while (ptCurrentTask != NULL)
    {

        if (ptCurrentTask->GetPriority() >= dwHighestPriority)
        {
            dwHighestPriority = ptCurrentTask->GetPriority();
            ptHighestPriorityTask = ptCurrentTask;
            ptPrevHighestPriorityTask = ptPrevCurrentTask;
        }

        ptPrevCurrentTask = ptCurrentTask;
        ptCurrentTask = ptCurrentTask->GetNext();
    }

    Task * ptBestTask = ptHighestPriorityTask;
    Task * ptPrevBestTask = ptPrevHighestPriorityTask;



    int32 nWaitTime = GetWaitTimeForTaskPriority((TaskPriorities)ptBestTask->GetPriority());
    if (m_nCurrentPriorityWait - nWaitTime > 0)
    {
        dwWaitForBestTask = m_nCurrentPriorityWait - nWaitTime;
        return NULL;
    }

    m_nCurrentPriorityWait = nWaitTime;
    dwWaitForBestTask = 0;


    if (ptPrevBestTask != NULL)
    {
        // The original stores NULL to the previous task's link before relinking it.
        ptPrevBestTask->SetNext(NULL);
        ptPrevBestTask->SetNext(ptBestTask->GetNext());
    }
    else
    {

        m_ptFirstTask = ptBestTask->GetNext();
    }

    ptBestTask->SetNext(NULL);

    return ptBestTask;
}
#line 282

void DecompressionTaskThread::OnArchiveClose(CPKArchiveReader * ptArchive)
{
    Blizzard::Lock::ScopedMutex mutex(m_tProcessingMutex);

    if (m_decompBuffer.GetArchive() == ptArchive)
    {
        m_decompBuffer.SetInvalid();
    }
    if (m_decompAheadBuffer.GetArchive() == ptArchive)
    {
        m_decompAheadBuffer.SetInvalid();
    }
    if (m_compBuffer.GetArchive() == ptArchive)
    {
        m_compBuffer.SetInvalid();
    }

    {
        Blizzard::Lock::ScopedMutex mutex2(m_tTaskListMutex);

        Task * ptTask = m_ptFirstTask;
        Task * ptTaskPrev = NULL;
        while (ptTask != NULL)
        {
            Task * ptTaskToCheck = ptTask;
            ptTask = ptTask->GetNext();

            if (ptTaskToCheck->GetArchive() == ptArchive)
            {
                if (ptTaskPrev != NULL)
                    ptTaskPrev->SetNext(ptTask);
                else
                    m_ptFirstTask = ptTask;
                ptTaskToCheck->SetNext(NULL);
                ptTaskToCheck->SetFailed(IORESULT_CANCELED);
                ptTaskToCheck->Release();
            }
            else
            {
                ptTaskPrev = ptTaskToCheck;
            }
        }
    }
}




void DecompressionTaskThread::ThreadProc()
{
    ThreadGlobals::InitializeForThread();
    SigmaProfInitializeForThread();

    uint32 dwWaitForTask = 0;
    while (TRUE)
    {
        WaitForTask(dwWaitForTask);

        if (m_fDone)
            break;

        Blizzard::Lock::ScopedMutex mutex(m_tProcessingMutex);

        while (m_ptFirstTask != NULL)
        {
            Task * ptTask = GetBestTask(dwWaitForTask);
            if (ptTask == NULL)
                break;

            if (ptTask->SetInProgress())
            {
                Results res = ptTask->Process(this);
                if (res == IORESULT_SUCCESS)
                {
                    ptTask->SetCompleted(this);
                }
                else
                {
                    ptTask->SetFailed(res);
                }
            }
            else
            {
                ptTask->SetFailed(IORESULT_CANCELED);
            }
            ptTask->Release();
        }

        if (m_compBuffer.IsValid() && (!m_decompBuffer.IsValid() || !m_decompAheadBuffer.IsValid()))
        {
            DecompressAheadSector();
        }
    }

    SigmaProfDisposeForThread();
    ThreadGlobals::DisposeForThread();
}

IOAccessTaskThread::IOAccessTaskThread(const char * szName, Blizzard::Thread::EThreadPriority ePriority, int32 nProcessor)
    : TaskThread(szName, ePriority, nProcessor),
      m_ptLastArchive(NULL),
      m_nLastSector(-1),
      m_ptLastDecompAheadArchive(NULL),
      m_nLastDecompAheadSector(-1),
      m_nCurrentSuggestion(0),
      m_fReady(FALSE)
{
    for (int32 i = 0; i < 2; i++)
    {
        m_tReadAheadSuggestions[i].ptLastArchive = NULL;
    }
}




#line 904
Task * IOAccessTaskThread::GetBestTask(
    uint32 & dwWaitForBestTask)
{
    Blizzard::Lock::ScopedMutex mutex(m_tTaskListMutex);




    uint32 dwHighestPriority = 0;
    Task * ptContiguousTask = NULL;
    Task * ptPrevContiguousTask = NULL;
    Task * ptHighestPriorityTask = NULL;
    Task * ptPrevHighestPriorityTask = NULL;

    Task * ptCurrentTask = m_ptFirstTask;
    Task * ptPrevCurrentTask = NULL;
    while (ptCurrentTask != NULL)
    {

        ptCurrentTask->ComputeBestReadSector(m_ptLastArchive, m_nLastSector);


        if ((ptCurrentTask->GetArchive() == m_ptLastArchive) &&
            ((ptCurrentTask->GetReadSector() == m_nLastSector) || (ptCurrentTask->GetReadSector() == m_nLastSector + 1)))
        {
            ptContiguousTask = ptCurrentTask;
            ptPrevContiguousTask = ptPrevCurrentTask;
        }


        if (ptCurrentTask->GetPriority() >= dwHighestPriority)
        {
            dwHighestPriority = ptCurrentTask->GetPriority();
            ptHighestPriorityTask = ptCurrentTask;
            ptPrevHighestPriorityTask = ptPrevCurrentTask;
        }

        ptPrevCurrentTask = ptCurrentTask;
        ptCurrentTask = ptCurrentTask->GetNext();
    }

    Task * ptBestTask = ptHighestPriorityTask;
    Task * ptPrevBestTask = ptPrevHighestPriorityTask;


    if (ptContiguousTask != NULL && (ptContiguousTask->GetPriority() + PRIORITY_DIFFERENCE_ALLOWING_CONTIGUOUS_READ) >= dwHighestPriority)
    {
        ptBestTask = ptContiguousTask;
        ptPrevBestTask = ptPrevContiguousTask;
    }




    int32 nWaitTime = GetWaitTimeForTaskPriority((TaskPriorities)ptBestTask->GetPriority());
    if (m_nCurrentPriorityWait - nWaitTime > 0)
    {
        dwWaitForBestTask = m_nCurrentPriorityWait - nWaitTime;
        return NULL;
    }

    m_nCurrentPriorityWait = nWaitTime;
    dwWaitForBestTask = 0;


    if (ptPrevBestTask != NULL)
    {
        // The original stores NULL to the previous task's link before relinking it.
        ptPrevBestTask->SetNext(NULL);
        ptPrevBestTask->SetNext(ptBestTask->GetNext());
    }
    else
    {
        m_ptFirstTask = ptBestTask->GetNext();
    }

    ptBestTask->SetNext(NULL);

    return ptBestTask;

}
#line 489

void IOAccessTaskThread::OnArchiveClose(CPKArchiveReader * ptArchive)
{
    if (m_ptLastArchive == ptArchive)
    {
        m_ptLastArchive = NULL;
    }

    for (int32 i = 0; i < 2; i++)
    {
        ReadAheadSuggestion & tReadAheadSuggestion = m_tReadAheadSuggestions[i];
        if (tReadAheadSuggestion.ptLastArchive == ptArchive)
        {
            tReadAheadSuggestion.ptLastArchive = NULL;
        }
        if (tReadAheadSuggestion.ptNextArchive == ptArchive)
        {
            tReadAheadSuggestion.ptLastArchive = NULL;
            tReadAheadSuggestion.ptNextArchive = NULL;
        }
    }

    if (m_readBuffer.GetArchive() == ptArchive)
    {
        m_readBuffer.SetInvalid();
    }
    if (m_readAheadBuffer.GetArchive() == ptArchive)
    {
        m_readAheadBuffer.SetInvalid();
    }

    DecompressionTaskThread * ptDecompThread = GetDecompressionThread();
    ptDecompThread->OnArchiveClose(ptArchive);
}

void IOAccessTaskThread::ThreadProc()
{
    ThreadGlobals::InitializeForThread();
    SigmaProfInitializeForThread();

    uint32 dwWaitForTask = 0;
    while (TRUE)
    {
        BOOL fShouldReadAhead = FALSE;
        int32 nNextCompSector = -1;
        uint32 dwDecompAheadPriotity = 0;
        CPKArchiveReader * ptLastDecompArchive = NULL;

        WaitForTask(dwWaitForTask);

        if (m_fDone)
            break;

        while (m_ptFirstTask != NULL)
        {
            Task * ptTask = GetBestTask(dwWaitForTask);
            if (ptTask == NULL)
                break;

            if (ptTask->SetInProgress())
            {
                Results res = ptTask->Process(this);
                if (res == IORESULT_SUCCESS)
                {
                    CPKArchiveReader * ptArchive = ptTask->GetArchive();

                    if (ptTask->HasReadFromFileSystem())
                    {
                        int32 nSectorRead = m_readBuffer.GetSector();
                        m_ptLastArchive = ptArchive;
                        m_nLastSector = nSectorRead;
                    }

                    fShouldReadAhead = ptTask->ShouldReadAhead() && ptArchive == m_ptLastArchive;

                    ptTask->SetCompleted(this);

                    if (ptTask->ShouldDecompressAhead())
                    {
                        nNextCompSector = ptTask->GetNextCompSector();
                        dwDecompAheadPriotity = ptTask->GetPriority();
                        ptLastDecompArchive = ptArchive;
                    }
                }
                else
                {
                    ptTask->SetFailed(res);
                }
                ptTask->Release();
            }
            else
            {
                ptTask->SetFailed(IORESULT_CANCELED);
                ptTask->Release();
            }
        }

        if (fShouldReadAhead)
        {
            CPKArchiveReader * ptNextArchive = m_ptLastArchive;
            int32 nNextSector = m_nLastSector + 1;

            for (int32 i = 0; i < 2; i++)
            {
                ReadAheadSuggestion & tReadAheadSuggestion = m_tReadAheadSuggestions[i];
                if (m_ptLastArchive == tReadAheadSuggestion.ptLastArchive && m_nLastSector == tReadAheadSuggestion.nLastSector)
                {
                    nNextSector = tReadAheadSuggestion.nNextSector;
                    ptNextArchive = tReadAheadSuggestion.ptNextArchive;
                    tReadAheadSuggestion.ptLastArchive = NULL;
                    break;
                }
            }

            int32 nValidSectorCount = ptNextArchive->GetReadSectorCount();
            if (nValidSectorCount > nNextSector &&
                !m_readBuffer.Contains(ptNextArchive, nNextSector) && !m_readAheadBuffer.Contains(ptNextArchive, nNextSector))
            {
                if (ptNextArchive->ReadSector(m_readAheadBuffer, nNextSector, NULL, NULL, THREADINDEX_COUNT))
                {
                    m_nLastSector = nNextSector;
                }
            }
        }

        if (ptLastDecompArchive != NULL &&
            (ptLastDecompArchive != m_ptLastDecompAheadArchive || nNextCompSector != m_nLastDecompAheadSector))
        {
            m_ptLastDecompAheadArchive = ptLastDecompArchive;
            m_nLastDecompAheadSector = nNextCompSector;

            AssignAheadCompSectorTask * ptDecomp = new AssignAheadCompSectorTask();

            int32 nCompSectorOffsetInReadSector;
            int32 nReadSector = ptLastDecompArchive->CompSectorToReadSector(nNextCompSector, nCompSectorOffsetInReadSector);
            DecompressionTaskThread * ptDecompThread = GetDecompressionThread();

            if (m_readBuffer.Contains(ptLastDecompArchive, nReadSector))
            {
                int32 nOffset = nCompSectorOffsetInReadSector * ptLastDecompArchive->GetCompSectorSize();
                ptDecomp->Initialize(ptLastDecompArchive, nNextCompSector, m_readBuffer.GetReadBuffer() + nOffset, Math::Min(m_readBuffer.GetReadSize() - nOffset, ptLastDecompArchive->GetCompSectorSize()), dwDecompAheadPriotity);
                ptDecompThread->SubmitTask(ptDecomp);
            }
            else if (m_readAheadBuffer.IsValid() && m_readAheadBuffer.GetArchive()->IsCompressed() && m_readAheadBuffer.GetSector() >= m_readAheadBuffer.GetArchive()->GetHeaderReadSectorCount())
            {
                CPKArchiveReader * ptArchive = m_readAheadBuffer.GetArchive();
                ptDecomp->Initialize(ptArchive, ptArchive->ReadSectorToCompSector(m_readAheadBuffer.GetSector()), m_readAheadBuffer.GetReadBuffer(), Math::Min(m_readAheadBuffer.GetReadSize(), ptArchive->GetCompSectorSize()), dwDecompAheadPriotity);
                ptDecompThread->SubmitTask(ptDecomp);
            }

            ptDecomp->Release();
        }
    }

    SigmaProfDisposeForThread();
    ThreadGlobals::DisposeForThread();
}

}  // namespace IO
}  // namespace Console
