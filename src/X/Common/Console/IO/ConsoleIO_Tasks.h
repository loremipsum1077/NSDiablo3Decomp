// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Console/IO/ConsoleIO_Tasks.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleIO.h"
#include "../../ThreadGlobals.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console { namespace IO { class CPKArchiveReader; } }
namespace Console { namespace IO { class TaskThread; } }
namespace Console { namespace IO { class ChildTask; } }

namespace Console {
namespace IO {
class Task {  // size 0x68
    // vtable pointer
protected:
    enum States {
        TASKSTATE_CREATED = 0,
        TASKSTATE_SUBMITTED = 1,
        TASKSTATE_IN_PROGRESS = 2,
        TASKSTATE_PROCESSING_OUTPUT_BUFFER = 3,
        TASKSTATE_COMPLETED = 4,
        TASKSTATE_FAILED = 5,
        TASKSTATE_CANCELED = 6,
    };
public:
    Task();
    virtual ~Task();  // vtable slot 0
    virtual void AddRef();  // vtable slot 2
    virtual void Release();  // vtable slot 3
    int32 GetRefCount() const;
    BOOL IsSubmitted() const;
    BOOL IsCompleted() const;
    BOOL IsInProgress() const;
    BOOL IsCanceled() const;
    BOOL IsFailed() const;
    BOOL IsInitialized() const;
    Console::IO::Results GetResults() const;
    void WaitForCompletion();
    void SetSubmitted();
    virtual void SetCompleted(Console::IO::TaskThread * ptTaskThread);  // vtable slot 4
    virtual void ComputeBestReadSector(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector);  // vtable slot 5
    BOOL SetInProgress();
    void SetCanceled();
    virtual void SetFailed(Console::IO::Results nResult);  // vtable slot 6
    void SetNext(Console::IO::Task * ptNext);
    Console::IO::Task * GetNext() const;
    Console::IO::CPKArchiveReader * GetArchive() const;
    int32 GetReadSector() const;
    int32 GetBytesRead() const;
    uint32 GetPriority() const;
    virtual Console::IO::Results Process(Console::IO::TaskThread *) = 0;  // vtable slot 7
    int32 HasReadFromFileSystem() const;
    int32 ShouldDecompressAhead() const;
    int32 GetNextCompSector() const;
    int32 ShouldReadAhead() const;
    ThreadIndex GetCreatorThread() const;
protected:
    Blizzard::Lock::REvent m_tTaskCompleted;  // 0x08
    volatile int32 m_nState;  // 0x14
    Console::IO::Results m_nResult;  // 0x18
    Console::IO::Task * m_ptNext;  // 0x20
    Console::IO::CPKArchiveReader * m_ptArchive;  // 0x28
    Console::IO::FileRecord * m_ptRecord;  // 0x30
    int32 m_nOffset;  // 0x38
    uint32 m_dwPriority;  // 0x3c
    int32 m_nReadSector;  // 0x40
    int32 m_nBytesRead;  // 0x44
    volatile int32 m_nRefCount;  // 0x48
    int32 m_nNextCompSector;  // 0x4c
    BOOL m_fReadFromFileSystem;  // 0x50
    BOOL m_fShouldDecompressAhead;  // 0x54
    BOOL m_fShouldReadAhead;  // 0x58
    BOOL m_fInitialized;  // 0x5c
    ThreadIndex m_nCreatorThread;  // 0x60
    void Initialize(Console::IO::CPKArchiveReader * ptArchive, Console::IO::FileRecord * ptRecord, int32 nOffset, uint32 dwPriority);
    BOOL ChangeState(Console::IO::Task::States nFrom, Console::IO::Task::States nTo);
};
static_assert(sizeof(Task) == 0x68, "Task");

// ConsoleIO_Tasks.h:37
inline Task::~Task() {
}

// ConsoleIO_Tasks.h:42
inline void Task::AddRef() {
    Blizzard::Lock::Atomic::Increment(&m_nRefCount);
}

// ConsoleIO_Tasks.h:47
inline void Task::Release() {
    int32 nRefCount = Blizzard::Lock::Atomic::Decrement(&m_nRefCount);
    if (nRefCount == 0)
        ::delete this;
}

// ConsoleIO_Tasks.h:77
inline BOOL Task::IsCanceled() const {
    return m_nState == TASKSTATE_CANCELED;
}

// ConsoleIO_Tasks.h:82
inline BOOL Task::IsFailed() const {
    return m_nState == TASKSTATE_FAILED;
}

// ConsoleIO_Tasks.h:92
inline Console::IO::Results Task::GetResults() const {
    return m_nResult;
}

// ConsoleIO_Tasks.h:97
inline void Task::WaitForCompletion() {
    m_tTaskCompleted.WaitForever();
}

// ConsoleIO_Tasks.h:107
inline void Task::ComputeBestReadSector(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector) {
}

// ConsoleIO_Tasks.h:123
inline void Task::SetNext(Console::IO::Task * ptNext) {
    m_ptNext = ptNext;
}

// ConsoleIO_Tasks.h:130
inline Console::IO::Task * Task::GetNext() const {
    return m_ptNext;
}

// ConsoleIO_Tasks.h:135
inline Console::IO::CPKArchiveReader * Task::GetArchive() const {
    return m_ptArchive;
}

// ConsoleIO_Tasks.h:140
inline int32 Task::GetReadSector() const {
    return m_nReadSector;
}

// ConsoleIO_Tasks.h:145
inline int32 Task::GetBytesRead() const {
    return m_nBytesRead;
}

// ConsoleIO_Tasks.h:150
inline uint32 Task::GetPriority() const {
    return m_dwPriority;
}

// ConsoleIO_Tasks.h:158
inline int32 Task::HasReadFromFileSystem() const {
    return m_fReadFromFileSystem;
}

// ConsoleIO_Tasks.h:163
inline int32 Task::ShouldDecompressAhead() const {
    return m_fShouldDecompressAhead;
}

// ConsoleIO_Tasks.h:168
inline int32 Task::GetNextCompSector() const {
    return m_nNextCompSector;
}

// ConsoleIO_Tasks.h:173
inline int32 Task::ShouldReadAhead() const {
    return m_fShouldReadAhead;
}

// ConsoleIO_Tasks.h:208
inline BOOL Task::ChangeState(Console::IO::Task::States nFrom, Console::IO::Task::States nTo) {
    return Blizzard::Lock::Atomic::CompareAndSwap(nFrom, nTo, &m_nState);
}

class ParentTask : public Console::IO::Task {  // size 0x80
public:
    ParentTask();
    virtual void SetCompleted(Console::IO::TaskThread * ptTaskThread);  // vtable slot 4
    virtual void SetFailed(Console::IO::Results nResult);  // vtable slot 6
    virtual void OnChildCompleted(Console::IO::ChildTask * ptChild, Console::IO::TaskThread * ptTaskThread);  // vtable slot 8
    virtual void OnChildFailed(Console::IO::ChildTask * ptChild, Console::IO::Results nResult);  // vtable slot 9
    void SetChildCount(int32 nChildCount);
    void WaitForChildCompletion();
    BOOL HasAnyChildFailed(Console::IO::Results & nChildResult) const;
    BOOL HasAnyChild() const;
protected:
    Blizzard::Lock::REvent m_tChildTasksCompleted;  // 0x64
    volatile int32 m_nChildAndSelfCount;  // 0x70
    Console::IO::Results m_nChildResult;  // 0x74
    BOOL m_fHasAnyChild;  // 0x78
    BOOL m_fAnyChildFailed;  // 0x7c
    void Initialize(Console::IO::CPKArchiveReader * ptArchive, Console::IO::FileRecord * ptRecord, int32 nOffset, uint32 dwPriority);
};
static_assert(sizeof(ParentTask) == 0x80, "ParentTask");

// ConsoleIO_Tasks.h:242
inline BOOL ParentTask::HasAnyChildFailed(Console::IO::Results & nChildResult) const {
    nChildResult = m_nChildResult;
    return m_fAnyChildFailed;
}

class ChildTask : public Console::IO::Task {  // size 0x70
public:
    ChildTask();
    virtual void SetCompleted(Console::IO::TaskThread * ptTaskThread);  // vtable slot 4
    virtual void SetFailed(Console::IO::Results nResult);  // vtable slot 6
    virtual void AddRef();  // vtable slot 2
    virtual void Release();  // vtable slot 3
protected:
    Console::IO::ParentTask * m_ptParent;  // 0x68
    void Initialize(Console::IO::CPKArchiveReader * ptArchive, Console::IO::FileRecord * ptRecord, int32 nOffset, uint32 dwPriority, Console::IO::ParentTask * ptParent);
};
static_assert(sizeof(ChildTask) == 0x70, "ChildTask");

// ConsoleIO_Tasks.h:279
inline void ChildTask::AddRef() {
    m_ptParent->AddRef();
    Task::AddRef();
}

// ConsoleIO_Tasks.h:286
inline void ChildTask::Release() {
    Console::IO::ParentTask * ptParent = m_ptParent;
    Task::Release();
    ptParent->Release();
}

class OpenArchiveTask : public Console::IO::Task {  // size 0x1070
public:
    OpenArchiveTask();
    void Initialize(const char * szFilename, BOOL fIsHotfix, BOOL fValidateCRC, uint32 dwPriority);
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
protected:
    char m_szFilename[4096];  // 0x64
    BOOL m_fIsHotfix;  // 0x1064
    BOOL m_fValidateCRC;  // 0x1068
};
static_assert(sizeof(OpenArchiveTask) == 0x1070, "OpenArchiveTask");

class OpenArchiveFromMemoryTask : public Console::IO::Task {  // size 0x1078
public:
    OpenArchiveFromMemoryTask();
    void Initialize(const char * szFilename, const uint8 * pArchiveData, int32 nArchiveSize, BOOL fValidateCRC, uint32 dwPriority);
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
protected:
    char m_szFilename[4096];  // 0x64
    const uint8 * m_pArchiveData;  // 0x1068
    int32 m_nArchiveSize;  // 0x1070
    BOOL m_fValidateCRC;  // 0x1074
};
static_assert(sizeof(OpenArchiveFromMemoryTask) == 0x1078, "OpenArchiveFromMemoryTask");

class CloseArchiveTask : public Console::IO::Task {  // size 0x68
public:
    CloseArchiveTask();
    void Initialize(Console::IO::CPKArchiveReader * ptArchive, uint32 dwPriority);
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
};
static_assert(sizeof(CloseArchiveTask) == 0x68, "CloseArchiveTask");

class ReadRecordTask : public Console::IO::Task {  // size 0x80
public:
    ReadRecordTask();
    void Initialize(Console::IO::FileRecord * ptRecord, int32 nOffset, int32 nSize, void * pDest, uint32 dwPriority);
    virtual void ComputeBestReadSector(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector);  // vtable slot 5
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
    virtual void SetCompleted(Console::IO::TaskThread * ptTaskThread);  // vtable slot 4
protected:
    int32 m_nOffsetInFirstSector;  // 0x64
    int32 m_nReadSectorCount;  // 0x68
    int32 m_nSizeInLastSector;  // 0x6c
    uint8 * m_pDest;  // 0x70
    int32 m_nSize;  // 0x78
};
static_assert(sizeof(ReadRecordTask) == 0x80, "ReadRecordTask");

class DecompressSectorTask : public Console::IO::ChildTask {  // size 0x98
public:
    DecompressSectorTask();
    virtual ~DecompressSectorTask();  // vtable slot 0
    void Initialize(Console::IO::CPKArchiveReader * ptArchive, int32 nCompSector, const void * pSource, void * pDest, int32 nOffset, int32 nCompSize, int32 nDecompSize, uint32 dwPriority, Console::IO::ParentTask * ptParent, Console::IO::FileRecord * ptRecordToUpdate);
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
protected:
    int32 m_nCompSector;  // 0x70
    int32 m_nCompSize;  // 0x74
    int32 m_nDecompSize;  // 0x78
    uint8 * m_pSource;  // 0x80
    uint8 * m_pDest;  // 0x88
    Console::IO::FileRecord * m_ptRecordToUpdate;  // 0x90
};
static_assert(sizeof(DecompressSectorTask) == 0x98, "DecompressSectorTask");

class AssignAheadCompSectorTask : public Console::IO::Task {  // size 0x78
public:
    AssignAheadCompSectorTask();
    virtual ~AssignAheadCompSectorTask();  // vtable slot 0
    void Initialize(Console::IO::CPKArchiveReader * ptArchive, int32 nCompSector, const void * pSource, int32 nCompSize, uint32 dwPriority);
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
private:
    int32 m_nCompSector;  // 0x64
    int32 m_nCompSize;  // 0x68
    uint8 * m_pSource;  // 0x70
};
static_assert(sizeof(AssignAheadCompSectorTask) == 0x78, "AssignAheadCompSectorTask");

class AssignAheadReadSectorTask : public Console::IO::Task {  // size 0x88
public:
    AssignAheadReadSectorTask();
    void Initialize(const Console::IO::ReadAheadSuggestion & tReadAheadSuggestion, uint32 dwPriority);
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
private:
    Console::IO::ReadAheadSuggestion m_tReadAheadSuggestion;  // 0x68
};
static_assert(sizeof(AssignAheadReadSectorTask) == 0x88, "AssignAheadReadSectorTask");

class ReadAndDecompressRecordTask : public Console::IO::ParentTask {  // size 0x310
public:
    ReadAndDecompressRecordTask();
    virtual ~ReadAndDecompressRecordTask();  // vtable slot 0
    void Initialize(Console::IO::FileRecord * ptRecord, int32 nOffset, int32 nSize, void * pDest, uint32 dwPriority);
    virtual void ComputeBestReadSector(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector);  // vtable slot 5
    virtual Console::IO::Results Process(Console::IO::TaskThread * ptTaskThread);  // vtable slot 7
    virtual void SetCompleted(Console::IO::TaskThread * ptTaskThread);  // vtable slot 4
protected:
    int32 m_nOffsetInFirstSector;  // 0x80
    int32 m_nCompSectorOffsetInFirstReadSector;  // 0x84
    int32 m_nCompSector;  // 0x88
    int32 m_nCompSectorCount;  // 0x8c
    int32 m_nSizeInLastSector;  // 0x90
    uint8 * m_pDest;  // 0x98
    int32 m_nReadSectorCount;  // 0xa0
    int32 m_nLastBytesRead;  // 0xa4
    int32 m_nSize;  // 0xa8
    Console::IO::DecompressSectorTask m_tDecompressTasks[4];  // 0xb0
};
static_assert(sizeof(ReadAndDecompressRecordTask) == 0x310, "ReadAndDecompressRecordTask");

}  // namespace IO
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    37  Console::IO::Task::~Task()  [0x71006ac900 (+1 more), inlined into 11 functions]
//   line    42  void Console::IO::Task::AddRef()  [0x71006ac910, inlined into 1 functions]
//   line    47  void Console::IO::Task::Release()  [0x71006ac930, inlined into 1 functions]
//   line    77  BOOL Console::IO::Task::IsCanceled() const  [inlined into 6 functions]
//   line    82  BOOL Console::IO::Task::IsFailed() const  [inlined into 4 functions]
//   line    92  Console::IO::Results Console::IO::Task::GetResults() const  [inlined into 4 functions]
//   line    97  void Console::IO::Task::WaitForCompletion()  [inlined into 4 functions]
//   line   107  void Console::IO::Task::ComputeBestReadSector(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector)  [0x71006ac990]
//   line   123  void Console::IO::Task::SetNext(Console::IO::Task * ptNext)  [inlined into 5 functions]
//   line   130  Console::IO::Task * Console::IO::Task::GetNext() const  [inlined into 3 functions]
//   line   135  Console::IO::CPKArchiveReader * Console::IO::Task::GetArchive() const  [inlined into 4 functions]
//   line   140  int32 Console::IO::Task::GetReadSector() const  [inlined into 1 functions]
//   line   145  int32 Console::IO::Task::GetBytesRead() const  [inlined into 1 functions]
//   line   150  uint32 Console::IO::Task::GetPriority() const  [inlined into 3 functions]
//   line   158  int32 Console::IO::Task::HasReadFromFileSystem() const  [inlined into 1 functions]
//   line   163  int32 Console::IO::Task::ShouldDecompressAhead() const  [inlined into 1 functions]
//   line   168  int32 Console::IO::Task::GetNextCompSector() const  [inlined into 1 functions]
//   line   173  int32 Console::IO::Task::ShouldReadAhead() const  [inlined into 1 functions]
//   line   208  BOOL Console::IO::Task::ChangeState(Console::IO::Task::States nFrom, Console::IO::Task::States nTo)  [inlined into 14 functions]
//   line   218  Console::IO::ParentTask::~ParentTask()  [0x71006ac9a0 (+1 more), inlined into 1 functions]
//   line   242  BOOL Console::IO::ParentTask::HasAnyChildFailed(Console::IO::Results & nChildResult) const  [inlined into 2 functions]
//   line   279  void Console::IO::ChildTask::AddRef()  [0x71006aca00]
//   line   286  void Console::IO::ChildTask::Release()  [0x71006aca40]
//   line   309  Console::IO::OpenArchiveTask::~OpenArchiveTask()  [0x71006acad0]
//   line   330  Console::IO::OpenArchiveFromMemoryTask::~OpenArchiveFromMemoryTask()  [0x71006acb10]
//   line   352  Console::IO::CloseArchiveTask::~CloseArchiveTask()  [0x71006acb50]
//   line   365  Console::IO::ReadRecordTask::~ReadRecordTask()  [0x71006acb90]
//   line   446  Console::IO::AssignAheadReadSectorTask::~AssignAheadReadSectorTask()  [0x71006acbd0]
