// Original helper ownership: Domino/Common/Platform/dmThreading_Linux.h.
#pragma once
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/nnmusl_alltypes.h"
// The original includes the NX SDK's <pthread.h>. These are its four entry points that dmMutex
// calls, declared against the recovered 32-byte nnmusl_alltypes.h:35 type instead of the build
// libc's 48-byte one; the C linkage keeps the call symbols unchanged. pthread_mutexattr_t is
// only ever passed as a null pointer here, so it is left incomplete.
struct nnmusl_pthread_mutexattr_t;
extern "C" {
int pthread_mutex_init(nnmusl_pthread_mutex_t *, const nnmusl_pthread_mutexattr_t *);
int pthread_mutex_destroy(nnmusl_pthread_mutex_t *);
int pthread_mutex_lock(nnmusl_pthread_mutex_t *);
int pthread_mutex_unlock(nnmusl_pthread_mutex_t *);
}
struct dmMutex {
#line 85
    void Create()
    {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    void Destroy()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void Lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

    nnmusl_pthread_mutex_t m_mutex;  // original type pthread_mutex_t (nnmusl_alltypes.h:35)
};
