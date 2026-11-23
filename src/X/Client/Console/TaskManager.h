// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/TaskManager.h

#pragma once

#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../Sigma/SigmaTypes.h"

namespace Console {
namespace Online {

class Task;  // forward declaration

typedef bdReference<Console::Online::Task> TaskRef;

class TaskManager {  // size 0x20
public:
    TaskManager();
    ~TaskManager();
    enum ProcessMode {
        PROCESS_CAN_START_TASKS = 0,
        PROCESS_HOLD_NOT_STARTED_TASKS = 1,
        PROCESS_FAIL_NOT_STARTED_TASKS = 2,
    };
    void Process(Console::Online::TaskManager::ProcessMode eMode);
    void ProcessSynchronous(Console::Online::TaskManager::ProcessMode);
    BOOL IsEmpty() const;
    void AddTask(Console::Online::TaskRef ptTask);
    void RemoveTask(Console::Online::TaskRef ptTask);
    void Clear();
private:
    blz::list<bdReference<Console::Online::Task>, blz::allocator<bdReference<Console::Online::Task> > > m_tTasks;  // 0x00
    BOOL m_bRunning;  // 0x18
};
static_assert(sizeof(TaskManager) == 0x20, "TaskManager");

// line 57 (push_back inlined at line 60)
inline void TaskManager::AddTask(TaskRef ptTask)
{
    m_tTasks.push_back(ptTask);
}

class Task : public bdReferencable {  // size 0x10
public:
    // Public: Console::Online::TaskManager::Process (TaskManager.cpp:34) declares a local
    // of type Console::Online::Task::Status (DWARF block local eStatus) and uses the
    // enumerators from outside Task; the DWARF records no DW_TAG_friend, so no friend
    // grants that access.
    enum Status {
        TASK_NOT_STARTED = 0,
        TASK_NOT_STARTED_CANT_START = 1,
        TASK_IN_PROGRESS = 2,
        TASK_DONE = 3,
        TASK_FAILED = 4,
    };
    Task(Console::Online::TaskManager &);
    virtual ~Task();  // vtable slot 0
    virtual void Start() = 0;  // vtable slot 2
    virtual void Cancel();  // vtable slot 3
    virtual Console::Online::Task::Status GetStatus() = 0;  // vtable slot 4
    virtual void OnComplete();  // vtable slot 5
    virtual void OnFailed();  // vtable slot 6
    virtual void OnCancel();  // vtable slot 7
};
static_assert(sizeof(Task) == 0x10, "Task");

// line 105
inline void Task::OnComplete()
{
}

// line 110
inline void Task::OnFailed()
{
}

// line 115
inline void Task::OnCancel()
{
}

}  // namespace Online
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    57  void Console::Online::TaskManager::AddTask(Console::Online::TaskRef ptTask)  [inlined into 1 functions]
//   line    66  void Console::Online::TaskManager::Clear()  [inlined into 1 functions]

namespace Console {
namespace Online {

// line 66 (69: clear). Inlined into ~LobbyServiceInternal (LobbyService.cpp:1116).
inline void TaskManager::Clear()
{
    m_tTasks.clear();
}

}  // namespace Online
}  // namespace Console
