// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/JobManager/JobManager.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/SigmaTypes.h"

class JobWorker;

enum {
    MAX_PENDING_JOBS = 1024,
};

enum JobManagerStates {
    JOBMANAGERSTATE_IDLE = 0,
    JOBMANAGERSTATE_WORKING = 1,
    JOBMANAGERSTATE_PUSHING = 2,
    JOBMANAGERSTATE_MAINWORKING = 3,
};

typedef void JobFcn(void *);

class JobManager {  // size 0x4058
    friend class JobWorker;
public:
    void PushJob(JobFcn * pFcn, void * pParam);
    void PushingCompleted();
    void WaitForIdle();
    BOOL IsInWorker();
    int32 GetWorkerCount() const;
    static JobManager * Get() { return sptManager; }  // line 59
    void NewFrame();
    static void InitializeForApp();
    static void DisposeForApp();
private:
    static JobManager * sptManager;
    struct Job {  // size 0x10
        JobFcn * pFcn;  // 0x00
        void * pParam;  // 0x08
        Job() : pFcn(NULL), pParam(NULL) {}  // line 79
    };
    volatile JobManagerStates m_eState;  // 0x00
    volatile int32 m_nPushingIndex;  // 0x04
    int32 m_nTotalWorkerCount;  // 0x08
    BOOL m_fTerminated;  // 0x0c
    JobWorker ** m_ptWorkers;  // 0x10
    volatile int32 m_nActiveWorkerCount;  // 0x18
    Blizzard::Lock::REvent m_tIdle;  // 0x1c
    Blizzard::Lock::RSemaphore m_tWorking;  // 0x28
    JobManager::Job m_aJobArray[1024];  // 0x50
    volatile int32 m_nPoppingIndex;  // 0x4050
    JobManager(int32);
    ~JobManager();
    JobManager::Job PopJob();
    void WorkerLoop();
    void WaitForInitialWork();
    void WorkCompleted();
    void Work();
    void Start();
};
static_assert(sizeof(JobManager) == 0x4058, "JobManager");

