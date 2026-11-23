// Original file: X/Client/Console/TaskManager.cpp

#include "TaskManager.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"

// line 18
Console::Online::TaskManager::TaskManager()
    : m_tTasks()
    , m_bRunning(FALSE)
{
}

// line 27
Console::Online::TaskManager::~TaskManager()
{
}

// line 34
void Console::Online::TaskManager::Process(ProcessMode eMode)
{
    m_bRunning = TRUE;

    for (blz::list<bdReference<Console::Online::Task>, blz::allocator<bdReference<Console::Online::Task> > >::iterator it = m_tTasks.begin(); it != m_tTasks.end(); )
    {
        TaskRef ptTask = *it;
        Task::Status eStatus = ptTask->GetStatus();

        if (eStatus == Task::TASK_NOT_STARTED || eStatus == Task::TASK_NOT_STARTED_CANT_START)
        {
            if (eMode == PROCESS_FAIL_NOT_STARTED_TASKS)
            {
                ptTask->OnFailed();
                it = m_tTasks.erase(it);
                continue;
            }
            else if (eMode != PROCESS_HOLD_NOT_STARTED_TASKS && eStatus != Task::TASK_NOT_STARTED_CANT_START)
            {
                ptTask->Start();
            }
        }
        else if (eStatus == Task::TASK_IN_PROGRESS)
        {
        }
        else
        {
            if (eStatus == Task::TASK_DONE)
            {
                ptTask->OnComplete();
            }
            else
            {
                ptTask->OnFailed();
            }
            it = m_tTasks.erase(it);
            continue;
        }

        ++it;
    }

    m_bRunning = FALSE;
}

// line 131
void Console::Online::TaskManager::RemoveTask(TaskRef ptTask)
{
    blz::list<bdReference<Console::Online::Task>, blz::allocator<bdReference<Console::Online::Task> > >::iterator it = blz::find(m_tTasks.begin(), m_tTasks.end(), ptTask);
    if (it != m_tTasks.end())
    {
        m_tTasks.erase(it);
    }
}

// line 144
Console::Online::Task::Task(TaskManager& tTaskManager)
{
    tTaskManager.AddTask(TaskRef(this));
}

// line 153
Console::Online::Task::~Task() {
}

// line 160
void Console::Online::Task::Cancel()
{
    OnCancel();
}
