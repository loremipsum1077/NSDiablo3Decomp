// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Console/IO/ConsoleIO_TaskThread.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/Zlib/zlib.h"
#include "ConsoleIO.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console { namespace IO { class CPKArchiveReader; } }
namespace Console { namespace IO { class Task; } }

namespace Console {
namespace IO {
class TaskThread {  // size 0x50
    // vtable pointer
public:
    TaskThread(const char *, Blizzard::Thread::EThreadPriority, int32);
    virtual ~TaskThread();  // vtable slot 0
    virtual void OnArchiveClose(Console::IO::CPKArchiveReader * ptArchive);  // vtable slot 2
    void SubmitTask(Console::IO::Task * ptTask);
protected:
    Blizzard::Thread::Thread m_tThread;  // 0x08
    Blizzard::Lock::RMutex m_tTaskListMutex;  // 0x10
    Blizzard::Lock::REvent m_tTaskKickEvent;  // 0x30
    Console::IO::Task * m_ptFirstTask;  // 0x40
    BOOL m_fDone;  // 0x48
    int32 m_nCurrentPriorityWait;  // 0x4c
    virtual void ThreadProc() = 0;  // vtable slot 3
    int32 GetWaitTimeForTaskPriority(Console::IO::TaskPriorities ePriority);
    void WaitForTask(uint32 nMaxWait);
    static uint sThreadProc(void * pParam);
};
static_assert(sizeof(TaskThread) == 0x50, "TaskThread");

class DecompressionTaskThread : public Console::IO::TaskThread {  // size 0x24120
public:
    DecompressionTaskThread(const char *, Blizzard::Thread::EThreadPriority, int32);
    virtual ~DecompressionTaskThread();  // vtable slot 0
    virtual void OnArchiveClose(Console::IO::CPKArchiveReader * ptArchive);  // vtable slot 2
    BOOL DecompressSector(Console::IO::CPKArchiveReader * ptArchive, int32 nCompSector, uint8 * pCompressedBuffer, int32 nCompressedSize, int32 nDecompressedOffset, int32 nDecompressedSize, uint8 * pDecompressedBuffer, int32 nRemainingSize);
    BOOL DecompressAheadSector();
    Console::IO::DecompBuffer & GetDecompBuffer();
    Console::IO::DecompBuffer & GetDecompAheadBuffer();
    Console::IO::CompBuffer & GetCompBuffer();
private:
    z_stream_s m_tZStream;  // 0x50
    Blizzard::Lock::RMutex m_tProcessingMutex;  // 0xc0
    Console::IO::DecompBuffer m_decompBuffer;  // 0xe0
    Console::IO::DecompBuffer m_decompAheadBuffer;  // 0x100f8
    Console::IO::CompBuffer m_compBuffer;  // 0x20110
    Console::IO::Task * GetBestTask(uint32 & dwWaitForBestTask);
    virtual void ThreadProc();  // vtable slot 3
};
static_assert(sizeof(DecompressionTaskThread) == 0x24120, "DecompressionTaskThread");

class IOAccessTaskThread : public Console::IO::TaskThread {  // size 0x200d8
public:
    IOAccessTaskThread(const char *, Blizzard::Thread::EThreadPriority, int32);
    virtual void OnArchiveClose(Console::IO::CPKArchiveReader * ptArchive);  // vtable slot 2
    void AssignAheadReadSector(const Console::IO::ReadAheadSuggestion & tReadAheadSuggestion);
    BOOL IsReady() const;
    void SetReady();
    Console::IO::ReadBuffer & GetReadBuffer();
    Console::IO::ReadBuffer & GetReadAheadBuffer();
    enum {
        PRIORITY_DIFFERENCE_ALLOWING_CONTIGUOUS_READ = 100,
    };
private:
    Console::IO::CPKArchiveReader * m_ptLastArchive;  // 0x50
    int32 m_nLastSector;  // 0x58
    Console::IO::CPKArchiveReader * m_ptLastDecompAheadArchive;  // 0x60
    int32 m_nLastDecompAheadSector;  // 0x68
    Console::IO::ReadAheadSuggestion m_tReadAheadSuggestions[2];  // 0x70
    int32 m_nCurrentSuggestion;  // 0xb0
    BOOL m_fReady;  // 0xb4
    Console::IO::ReadBuffer m_readBuffer;  // 0xb8
    Console::IO::ReadBuffer m_readAheadBuffer;  // 0x100c8
    Console::IO::Task * GetBestTask(uint32 & dwWaitForBestTask);
    virtual void ThreadProc();  // vtable slot 3
};
static_assert(sizeof(IOAccessTaskThread) == 0x200d8, "IOAccessTaskThread");

// ConsoleIO_TaskThread.h:88
inline uint TaskThread::sThreadProc(void * pParam) {
    ((Console::IO::TaskThread *)pParam)->ThreadProc();
    return 0;
}

// ConsoleIO_TaskThread.h:135
inline Console::IO::DecompBuffer & DecompressionTaskThread::GetDecompBuffer() {
    return m_decompBuffer;
}

// ConsoleIO_TaskThread.h:140
inline Console::IO::DecompBuffer & DecompressionTaskThread::GetDecompAheadBuffer() {
    return m_decompAheadBuffer;
}

// ConsoleIO_TaskThread.h:145
inline Console::IO::CompBuffer & DecompressionTaskThread::GetCompBuffer() {
    return m_compBuffer;
}

// ConsoleIO_TaskThread.h:188
inline void IOAccessTaskThread::AssignAheadReadSector(const Console::IO::ReadAheadSuggestion & tReadAheadSuggestion) {
    m_tReadAheadSuggestions[m_nCurrentSuggestion] = tReadAheadSuggestion;
    m_nCurrentSuggestion = (m_nCurrentSuggestion + 1) % 2;
}

// ConsoleIO_TaskThread.h:200
inline void IOAccessTaskThread::SetReady() {
    m_fReady = TRUE;
}

// ConsoleIO_TaskThread.h:205
inline Console::IO::ReadBuffer & IOAccessTaskThread::GetReadBuffer() {
    return m_readBuffer;
}

// ConsoleIO_TaskThread.h:210
inline Console::IO::ReadBuffer & IOAccessTaskThread::GetReadAheadBuffer() {
    return m_readAheadBuffer;
}

// Defined in ConsoleIO.cpp:222; where the original declares it is not in the DWARF.
Console::IO::DecompressionTaskThread * GetDecompressionThread();

}  // namespace IO
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    88  uint Console::IO::TaskThread::sThreadProc(void * pParam)  [0x7100427fa0]
//   line   135  Console::IO::DecompBuffer & Console::IO::DecompressionTaskThread::GetDecompBuffer()  [inlined into 1 functions]
//   line   140  Console::IO::DecompBuffer & Console::IO::DecompressionTaskThread::GetDecompAheadBuffer()  [inlined into 1 functions]
//   line   145  Console::IO::CompBuffer & Console::IO::DecompressionTaskThread::GetCompBuffer()  [inlined into 1 functions]
//   line   174  Console::IO::IOAccessTaskThread::~IOAccessTaskThread()  [0x7100429530]
//   line   188  void Console::IO::IOAccessTaskThread::AssignAheadReadSector(const Console::IO::ReadAheadSuggestion & tReadAheadSuggestion)  [inlined into 1 functions]
//   line   200  void Console::IO::IOAccessTaskThread::SetReady()  [inlined into 1 functions]
//   line   205  Console::IO::ReadBuffer & Console::IO::IOAccessTaskThread::GetReadBuffer()  [inlined into 4 functions]
//   line   210  Console::IO::ReadBuffer & Console::IO::IOAccessTaskThread::GetReadAheadBuffer()  [inlined into 2 functions]
