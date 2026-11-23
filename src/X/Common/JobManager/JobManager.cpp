#include "JobManager.h"

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Sys/Atomic.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/SigmaProf.h"
#include "../ThreadGlobals.h"

using namespace Blizzard::Lock;

// Defined in BlizzardCore/Source/System/Process/Process_NX64.cpp; that header is not adopted yet.
namespace Blizzard {
namespace Process {
bool Sleep(Blizzard::Time::Millisecond milliseconds);
}  // namespace Process
}  // namespace Blizzard

// Defined in X/Common/Console/IO/ConsoleIO.cpp:195 and :205; that header is not adopted yet.
namespace Console {
namespace IO {
void InitializeForThread();
void DisposeForThread();
}  // namespace IO
}  // namespace Console

// Defined in X/Common/Shell/MainNX64.cpp:166.
void ConfigureJobThread(Blizzard::Thread::Thread tThread, uint32 uIndex);

class JobWorker {  // size 0x10
public:
    JobWorker(JobManager * ptManager, uint32 nIndex);
    ~JobWorker();
    void Start();
    Blizzard::Thread::Thread GetThread() {
        return m_tThread;
    }
private:
    Blizzard::Thread::Thread m_tThread;  // 0x00
    JobManager * m_ptManager;  // 0x08
    static uint sJobWorkerProc(void * pParam);
};

JobManager * JobManager::sptManager;

inline JobWorker::JobWorker(JobManager * ptManager, uint32 nIndex) {
    m_ptManager = ptManager;
    Blizzard::Thread::Create(sJobWorkerProc, this, &m_tThread, Blizzard::String::QuickFormat<1024>("JobWorker%d", nIndex), 1, 0x18000);
    SigmaProfRegisterThread(m_tThread, "JobWorker", 0x1000);
}

inline void JobWorker::Start() {
    Blizzard::Thread::Resume(m_tThread);
}

uint JobWorker::sJobWorkerProc(void * pParam) {
    ((JobWorker *)pParam)->m_ptManager->WorkerLoop();
    return 0;
}

inline JobWorker::~JobWorker() {
    SigmaProfUnregisterThread(m_tThread);
    Blizzard::Thread::WaitForever(m_tThread);
    Blizzard::Thread::Close(m_tThread);
}

JobManager::JobManager(int32 nTotalWorkerCount)
    : m_eState(JOBMANAGERSTATE_IDLE), m_nPushingIndex(0), m_nTotalWorkerCount(nTotalWorkerCount), m_fTerminated(FALSE), m_nActiveWorkerCount(0), m_nPoppingIndex(0) {
    m_ptWorkers = new JobWorker *[nTotalWorkerCount];
    for (int32 i = 0; i < nTotalWorkerCount; i++) {
        m_ptWorkers[i] = new JobWorker(this, i);
    }
}

JobManager::~JobManager() {
    m_fTerminated = TRUE;
    for (int32 i = 0; i < m_nTotalWorkerCount; i++) {
        m_tWorking.Set();
    }
    for (int32 i = 0; i < m_nTotalWorkerCount; i++) {
        delete m_ptWorkers[i];
    }
    delete[] m_ptWorkers;
}

void JobManager::InitializeForApp() {
    int32 nWorkerCount = 2;
    sptManager = new JobManager(nWorkerCount);
    for (int32 i = 0; i < nWorkerCount; i++) {
        ConfigureJobThread(sptManager->m_ptWorkers[i]->GetThread(), i);
    }
    sptManager->Start();
}

void JobManager::DisposeForApp() {
    delete sptManager;
}

inline void JobManager::Start() {
    for (int32 i = 0; i < m_nTotalWorkerCount; i++) {
        m_ptWorkers[i]->Start();
    }
}

void JobManager::PushJob(JobFcn * pFcn, void * pParam) {
    int32 nPushingIndex = m_nPushingIndex;
    int32 nNextPushingIndex = (nPushingIndex + 1) % MAX_PENDING_JOBS;
    if (nNextPushingIndex == m_nPoppingIndex || m_nTotalWorkerCount == 0) {
        pFcn(pParam);
        return;
    }
    m_aJobArray[nPushingIndex].pFcn = pFcn;
    m_aJobArray[nPushingIndex].pParam = pParam;
    Atomic::ReadWriteBarrier();
    m_nPushingIndex = nNextPushingIndex;
    if (m_eState == JOBMANAGERSTATE_IDLE) {
        m_eState = JOBMANAGERSTATE_PUSHING;
        for (int32 i = 0; i < m_nTotalWorkerCount; i++) {
            Atomic::Increment(&m_nActiveWorkerCount);
        }
        for (int32 i = 0; i < m_nTotalWorkerCount; i++) {
            m_tWorking.Set();
        }
    }
}

void JobManager::PushingCompleted() {
    if (m_eState == JOBMANAGERSTATE_PUSHING) {
        m_eState = JOBMANAGERSTATE_WORKING;
    }
}

void JobManager::NewFrame() {
    for (int32 i = 0; i < m_nTotalWorkerCount; i++) {
        SigmaProfNewFrame(m_ptWorkers[i]->GetThread());
    }
}

























































































































































void JobManager::Work()
{
    while (true)
    {


        const JobManagerStates eState = m_eState;
        Atomic::ReadWriteBarrier();

        Job job = PopJob();




        if (job.pFcn)
        {
            job.pFcn(job.pParam);
        }
        else
        {

            if (eState != JOBMANAGERSTATE_PUSHING)
            {
                break;
            }






            Blizzard::Process::Sleep(0);
        }
    }
}





void JobManager::WorkerLoop()
{

    Console::IO::InitializeForThread();


    ThreadGlobals::InitializeForThread();

    SigmaProfInitializeForThread();

    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_JOBWORKER;




    WaitForInitialWork();



    while (!m_fTerminated)
    {



        Work();

        WorkCompleted();
    }

    SigmaProfDisposeForThread();

    ThreadGlobals::DisposeForThread();

    Console::IO::DisposeForThread();
}






void JobManager::WaitForIdle()
{


    if (m_eState == JOBMANAGERSTATE_IDLE)
    {
        return;
    }

    if (m_eState == JOBMANAGERSTATE_PUSHING)
    {
        PushingCompleted();
    }

    m_eState = JOBMANAGERSTATE_MAINWORKING;






    Work();





    m_tIdle.WaitForever();

    m_eState = JOBMANAGERSTATE_IDLE;
}




void JobManager::WaitForInitialWork()
{

    m_tWorking.WaitForever();
}




void JobManager::WorkCompleted()
{





    if (Atomic::Decrement(&m_nActiveWorkerCount) == 0)
    {
        m_tIdle.Set();
    }


    m_tWorking.WaitForever();
}




JobManager::Job JobManager::PopJob()
{

    Job job;

    do
    {

        int32 nPoppingIndex = m_nPoppingIndex;

        int32 nNextPoppingIndex = (nPoppingIndex + 1) % MAX_PENDING_JOBS;

        Atomic::ReadWriteBarrier();


        if (m_nPushingIndex == nPoppingIndex)
        {
            break;
        }



        job = m_aJobArray[nPoppingIndex];

        if (!Atomic::CompareAndSwap(nPoppingIndex, nNextPoppingIndex, &m_nPoppingIndex))
        {
            job = Job();
        }
    }
    while (!job.pFcn);

    return job;
}
