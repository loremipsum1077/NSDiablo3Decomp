#include "SigmaProf.h"

SigmaProfGlobals gtSigmaProfGlobals;

void SigmaProfInitializeForApp() {
    gtSigmaProfGlobals.m_tThreadData.Initialize();
    SigmaProfRegisterThread(Blizzard::Thread::CurrentThread(), "Main", 0);
    SigmaProfInitializeForThread();
    gtSigmaProfGlobals.m_bProfEnabled = TRUE;
}

void SigmaProfDisposeForApp() {
    gtSigmaProfGlobals.m_bProfEnabled = FALSE;
    SigmaProfDisposeForThread();
    SigmaProfUnregisterThread(Blizzard::Thread::CurrentThread());
    gtSigmaProfGlobals.m_tThreadData.Dispose();
}

void SigmaProfNewFrame(Blizzard::Thread::Thread ptThread) {
    (void)ptThread;
}

void SigmaProfInitializeForThread() {
    Blizzard::Thread::Thread ptCurThread;
    void* ptTls = gtSigmaProfGlobals.m_tThreadData.TLSGetValue();
    (void)ptTls;
    ptCurThread = Blizzard::Thread::CurrentThread();
    SigmaProfThread* ptProfThread = SigmaProfFindThread(ptCurThread);
    gtSigmaProfGlobals.m_tThreadData.TLSSetValue(ptProfThread);
}

void SigmaProfDisposeForThread() {
    gtSigmaProfGlobals.m_tThreadData.TLSSetValue(NULL);
}

void SigmaProfRegisterThread(Blizzard::Thread::Thread ptThread, LPCSTR szName, uint16 nProfBufferSize) {
    (void)szName;
    (void)nProfBufferSize;
    if (gtSigmaProfGlobals.m_nProfThreadCount < MAX_SIGMA_PROF_THREAD_COUNT) {
        int nIndex = gtSigmaProfGlobals.m_nProfThreadCount;
        SigmaProfThread* ptProfThread = &gtSigmaProfGlobals.m_arProfThreads[nIndex];
        ptProfThread->ptThread = ptThread;
        gtSigmaProfGlobals.m_nProfThreadCount++;
    }
}

void SigmaProfUnregisterThread(Blizzard::Thread::Thread ptThread) {
    for (int i = 0; i < gtSigmaProfGlobals.m_nProfThreadCount; i++) {
        SigmaProfThread* ptProfThread = &gtSigmaProfGlobals.m_arProfThreads[i];
        if (ptProfThread->ptThread == ptThread) {
            ptProfThread->ptThread = NULL;
            *ptProfThread = gtSigmaProfGlobals.m_arProfThreads[--gtSigmaProfGlobals.m_nProfThreadCount];
            return;
        }
    }
}

inline SigmaProfThread* SigmaProfFindThread(Blizzard::Thread::Thread ptThread) {
    for (int i = 0; i < gtSigmaProfGlobals.m_nProfThreadCount; i++) {
        if (gtSigmaProfGlobals.m_arProfThreads[i].ptThread == ptThread)
            return &gtSigmaProfGlobals.m_arProfThreads[i];
    }
    return NULL;
}
