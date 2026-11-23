#include "bdThread.h"
#include "bdRunnable.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformThread/bdPlatformThread.h"
// Blank lines keep the original line numbers











bdThread::bdThread(bdRunnable* runnable, bdInt priority, bdUInt stackSize, bdInt coreAffinity)
    : m_runnable(runnable), m_threadArgs(), m_isRunning(false), m_stackSize(stackSize), m_startPriority(priority),
      m_coreAffinity(coreAffinity), m_stackPointer(0)
{
    m_threadArgs.m_runnable = m_runnable;
    m_threadArgs.m_thread = this;
}










bdThread::~bdThread()
{






    if (m_stackPointer)
    {
        bdMemory::alignedDeallocate(m_stackPointer);
    }









    bdPlatformThread::deleteThread(m_handle);

    m_runnable = 0;
}

bdBool bdThread::start(const void* args, const bdUWord size, const bdNChar8* threadName)
{


    bdBool ok = (m_runnable != 0) && !m_isRunning;


    m_threadArgs.m_args = 0;
    if (ok && size > 0)
    {
        m_threadArgs.m_args = bdMemory::allocate(size);
        bdMemcpy(m_threadArgs.m_args, args, size);
    }


    if (ok)
    {
        m_isRunning = true;
        m_runnable->start();




















        if (m_stackSize > 0)
        {
            m_stackPointer = bdMemory::alignedReallocate(m_stackPointer, m_stackSize, 0x1000);
            ok = m_stackPointer != 0;
        }
        ok = ok && bdPlatformThread::createThread((bdThreadProc)threadProc, &m_threadArgs, m_handle, m_startPriority, m_stackSize, m_stackPointer, m_coreAffinity, threadName);



        ok = ok && bdPlatformThread::startThread(m_handle, &m_threadArgs, sizeof(bdThreadArgs));
        ok = ok && bdPlatformThread::nameThread(m_handle, threadName);
    }

    return ok;
}





















void bdThread::join()
{







    bdPlatformThread::joinThread(m_handle);

    if (m_threadArgs.m_args)
    {
        bdMemory::deallocate(m_threadArgs.m_args);
        m_threadArgs.m_args = 0;
    }
}















void bdThread::cleanup()
{
    if (m_threadArgs.m_args)
    {
        bdMemory::deallocate(m_threadArgs.m_args);
        m_threadArgs.m_args = 0;
    }
    delete this;
}


























void bdThread::wakeupJoiningThread()
{






    m_isRunning = false;
}




bdInt bdThread::threadProc(bdThreadArgs* args)
{


    bdUInt ret = args->m_runnable->run(args->m_args);
    args->m_thread->wakeupJoiningThread();
    return ret;






}
