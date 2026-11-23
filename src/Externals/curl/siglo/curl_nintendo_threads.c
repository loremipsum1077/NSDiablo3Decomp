/* Nintendo's libcurl thread glue (siglo/curl_nintendo_threads.c): the
 * resolver threads and the garbage collector thread that joins them.
 * There is no public source for this file; it is written from the DWARF
 * (names, types, line numbers) and the binary. */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "Externals/curl/lib/llist.h"

/* From nnc/nn_Result.h, nnc/os, nnc/socket and nn/nn_Abort.h. */
typedef uint32_t nnResultInnerType;
typedef struct nnResult { nnResultInnerType _value; } nnResult;
typedef union nnosThreadType { uint8_t buf[448]; void *alignment_holder; } nnosThreadType;
typedef union nnosEventType { uint8_t buf[40]; void *alignment_holder; } nnosEventType;
typedef union nnosMutexType { uint8_t buf[32]; void *alignment_holder; } nnosMutexType;
_Bool nnResultIsFailure(nnResult result);
nnResult nnosCreateThread(nnosThreadType *thread, void (*function)(void *), void *argument, void *stack, size_t stackSize, int priority);
void nnosDestroyThread(nnosThreadType *thread); void nnosStartThread(nnosThreadType *thread); void nnosWaitThread(nnosThreadType *thread);
nnosThreadType *nnosGetCurrentThread(void); int nnosGetThreadPriority(const nnosThreadType *thread); void nnosSetThreadNamePointer(nnosThreadType *thread, const char *name);
void nnosInitializeMutex(nnosMutexType *mutex, _Bool recursive, int lockLevel); void nnosFinalizeMutex(nnosMutexType *mutex);
void nnosLockMutex(nnosMutexType *mutex); void nnosUnlockMutex(nnosMutexType *mutex);
void nnosInitializeEvent(nnosEventType *event, _Bool signaled, int clearMode); void nnosFinalizeEvent(nnosEventType *event);
void nnosSignalEvent(nnosEventType *event); void nnosWaitEvent(nnosEventType *event);
void nnsocketSetLastErrno(int error); size_t Curl_llist_count(struct curl_llist *list);
void *Curl_SigloMalloc(size_t sizeToAllocate); void Curl_SigloFree(void *pointer);
void *Curl_SigloAllocAligned(size_t alignment, size_t size); void Curl_SigloFreeAligned(void *pointer);
void nndetailDiagAbortFormatImpl(const char *condition, const char *function, const char *fileName, int lineNumber, const char *format, ...) __attribute__((noreturn));
#define NNC_ABORT(...) nndetailDiagAbortFormatImpl("NNC_ABORT", __func__, \
  "D:\\home\\teamcity2\\work\\sdk\\Externals\\curl\\siglo\\curl_nintendo_threads.c", \
  __LINE__, __VA_ARGS__)
#define EAGAIN 11 /* errno.h */
#define nnosEventClearMode_AutoClear 1

/* one resolver (or garbage collector) thread */
typedef struct Curl_SigloThreadContext
{
    /* the thread */
    nnosThreadType m_thread;

    /* what it runs */
    unsigned int (*m_function)(void *);

    /* the argument it runs with */
    void *m_functionArgument;

    /* the thread's stack */
    void *m_stackAligned;
} Curl_SigloThreadContext;

/* The garbage collector: a thread that waits for finished resolver
   threads, so that a resolver can give up on a thread without joining it.
   Threads move from the active list to the inactive list when their
   function returns; the collector then waits for them and frees them. */





typedef struct Curl_SigloThreadGC
{
    /* state of the collector */
    enum
    {
        UNINITIALIZED,
        INITIALIZING,
        WAITING,
        RUNNING,
        FINALIZING
    } m_state;
    /* how many users must finalize before the collector exits */
    unsigned int m_isFinalCount;
    /* the collector's own thread */
    Curl_SigloThreadContext *m_gcThreadContext;
    /* signalled when there is work */
    nnosEventType m_event;
    /* the event above wakes the collector:
         - when a thread moves to the inactive list
         - when the last user finalizes
       the lists below are only touched with m_accessLock held
       (the collector drops it while waiting for a thread)
    */
    /* guards everything below */
    nnosMutexType m_accessLock;

    /* threads that are running */
    struct curl_llist *m_activeThreadList;

    /* threads whose function has returned */
    struct curl_llist *m_inactiveThreadList;

    /* signalled once the collector thread has started */
    nnosEventType m_parentThreadEvent;
} Curl_SigloThreadGC;

Curl_SigloThreadGC g_garbageCollectorThread;

inline int Curl_GetTotalThreadCount(void)
{
    Curl_SigloThreadGC *instance = &g_garbageCollectorThread;
    int returnValue = -1;

    if (instance->m_state != UNINITIALIZED)
    {
        nnosLockMutex(&instance->m_accessLock);

        returnValue = Curl_llist_count(instance->m_activeThreadList) + Curl_llist_count(instance->m_inactiveThreadList);

        nnosUnlockMutex(&instance->m_accessLock);
    }

    return returnValue;
}

void Curl_SigloThreadEntryThunk(void *arg); unsigned int Curl_SigloThreadGCRunLoop(void *argument);
inline void Curl_SigloThreadContextDestructor(Curl_SigloThreadContext *context);



Curl_SigloThreadContext *Curl_SigloThreadContextConstructor(size_t stackSize, unsigned int (*function)(void *), void *functionArgument)


{
    Curl_SigloThreadContext *context;
    nnResult result = { (nnResultInnerType)-1 };
    /* the context, zeroed */
    context = (Curl_SigloThreadContext *)Curl_SigloMalloc(sizeof(Curl_SigloThreadContext));
    if (context != NULL)
    {


        memset(context, 0, sizeof(Curl_SigloThreadContext));

        /* the stack must be page aligned */


        context->m_stackAligned = Curl_SigloAllocAligned(4096, stackSize);
        if (context->m_stackAligned == NULL)
        {



            Curl_SigloFree(context);
            context = NULL;
        }
        else
        {
            result = nnosCreateThread(&context->m_thread,
                                      Curl_SigloThreadEntryThunk,
                                      context,
                                      context->m_stackAligned,
                                      stackSize,
                                      nnosGetThreadPriority(nnosGetCurrentThread()));
            if (!nnResultIsFailure(result))
            {



                nnosSetThreadNamePointer(&context->m_thread, "LibcurlResolver");
                context->m_function = function;
                context->m_functionArgument = functionArgument;
            }
        }
    }
    if (nnResultIsFailure(result) && context != NULL)
    {
        Curl_SigloThreadContextDestructor(context);
        context = NULL;
    }
    return context;
}

inline void Curl_SigloThreadContextDestructor(Curl_SigloThreadContext *context)
{
    if (context != NULL)
    {
        /* the thread must have been waited for already (or never started) */


        nnosDestroyThread(&context->m_thread);

        if (context->m_stackAligned != NULL)
        {
            Curl_SigloFreeAligned(context->m_stackAligned);
            context->m_stackAligned = NULL;
        }

        /* clear the rest */

        context->m_function = NULL;
        context->m_functionArgument = NULL;

        Curl_SigloFree(context);
    }
}

/* the active list does not own its contexts */

static void Curl_SigloThreadContextLListNoopDtor(void *user, void *element)
{
    (void)user;
    (void)element;

}

static void Curl_SigloThreadContextLListDtor(void *user, void *element)
{
    Curl_SigloThreadContext *threadContext = (Curl_SigloThreadContext *)element;
    (void)user;
    Curl_SigloThreadContextDestructor(threadContext);

}

inline void Curl_SigloThreadGCZero(void)
{
    /* everything that Curl_SigloThreadGCInitialize sets up */
    g_garbageCollectorThread.m_state = UNINITIALIZED;
    memset(&g_garbageCollectorThread.m_event, 0, offsetof(Curl_SigloThreadGC, m_parentThreadEvent) - offsetof(Curl_SigloThreadGC, m_event));


}

Curl_SigloThreadGC *Curl_SigloThreadGCInitialize(void)
{
    Curl_SigloThreadGC *instance = &g_garbageCollectorThread;

    /*
     * The collector is started once, by curl_global_init; it runs
     * Curl_SigloThreadGCRunLoop on its own thread until
     * Curl_SigloThreadGCFinalize.
     */






    if (instance->m_state == UNINITIALIZED)
    {
        /* claim it */




        instance->m_state = INITIALIZING;
        instance->m_isFinalCount++;

        instance->m_gcThreadContext = Curl_SigloThreadContextConstructor(16 * 1024, Curl_SigloThreadGCRunLoop, NULL);
        if (instance->m_gcThreadContext != NULL)
        {
            /* the active list does not own the contexts, the inactive list
               does */


            instance->m_activeThreadList = Curl_llist_alloc(Curl_SigloThreadContextLListNoopDtor);
            if (instance->m_activeThreadList != NULL)
            {


                instance->m_inactiveThreadList = Curl_llist_alloc(Curl_SigloThreadContextLListDtor);
                if (instance->m_inactiveThreadList != NULL)
                {
                    /* the lock is taken recursively by the list destructors */




                    nnosInitializeMutex(&instance->m_accessLock, 1, 0);

                    nnosInitializeEvent(&instance->m_event, 0, nnosEventClearMode_AutoClear);

                    nnosInitializeEvent(&instance->m_parentThreadEvent, 0, nnosEventClearMode_AutoClear);

                    nnosStartThread(&instance->m_gcThreadContext->m_thread);

                    nnosWaitEvent(&instance->m_parentThreadEvent);

                    nnosFinalizeEvent(&instance->m_parentThreadEvent);

                    return instance;
                }
            }
        }
    }
    /* undo what was done */
    if (instance->m_inactiveThreadList != NULL)
    {
        Curl_llist_destroy(instance->m_inactiveThreadList, NULL);
    }

    if (instance->m_activeThreadList != NULL)
    {
        Curl_llist_destroy(instance->m_activeThreadList, NULL);
    }

    if (instance->m_gcThreadContext != NULL)
    {
        Curl_SigloThreadContextDestructor(instance->m_gcThreadContext);
    }

    Curl_SigloThreadGCZero();
    instance = NULL;

    return instance;
}

unsigned int Curl_SigloThreadGCRunLoop(void *argument)
{
    Curl_SigloThreadGC *instance = &g_garbageCollectorThread;
    struct curl_llist_element *currentElement;
    Curl_SigloThreadContext *threadContext;
    (void)argument;

    instance->m_state = WAITING;

    /* tell Curl_SigloThreadGCInitialize the thread is up */
    nnosSignalEvent(&instance->m_parentThreadEvent);

    for (;;)
    {
        /* wait for work */
        nnosWaitEvent(&instance->m_event);
        nnosLockMutex(&instance->m_accessLock);

        instance->m_state = RUNNING;

        currentElement = instance->m_inactiveThreadList->head;
        while (currentElement != NULL)
        {
            struct curl_llist_element *nextElement = currentElement->next;
            threadContext = (Curl_SigloThreadContext *)currentElement->ptr;

            /* do not hold the lock while waiting: the thread may still need
               it to move itself to the inactive list
            */
            nnosUnlockMutex(&instance->m_accessLock);

            nnosWaitThread(&threadContext->m_thread);

            nnosLockMutex(&instance->m_accessLock);

            Curl_llist_remove(instance->m_inactiveThreadList, currentElement, NULL); currentElement = nextElement;
        }

        /* exit once finalized and every thread has been collected */
        if (instance->m_isFinalCount == 0 &&
            Curl_llist_count(instance->m_activeThreadList) +
            Curl_llist_count(instance->m_inactiveThreadList) == 0)
        {
            instance->m_state = FINALIZING;
            nnosUnlockMutex(&instance->m_accessLock);
            break;
        }

        nnosUnlockMutex(&instance->m_accessLock);
    }


    return 0;
}

inline int Curl_SigloThreadGCSetActive(Curl_SigloThreadContext *activeThreadContext)
{
    Curl_SigloThreadGC *instance = &g_garbageCollectorThread;
    int rc = 0;
    if (instance->m_state == UNINITIALIZED)
    {
        NNC_ABORT("Adding a thread structure to the garbage collector when it is uninitialized");
    }

    nnosLockMutex(&instance->m_accessLock);

    if (Curl_llist_insert_next(instance->m_activeThreadList,
                               instance->m_activeThreadList->tail, activeThreadContext) == 0)
    {
        rc = -1;
    }






    nnosUnlockMutex(&instance->m_accessLock);

    return rc;
}

inline int Curl_removeThreadContextFromList(struct curl_llist *list, Curl_SigloThreadContext *threadContext)
{
    struct curl_llist_element *currentElement = list->head;
    int rc = -1;

    while (currentElement != NULL)
    {
        if (currentElement->ptr == threadContext)
        {
            Curl_llist_remove(list, currentElement, NULL);
            rc = 0;
            break;
        }
        currentElement = currentElement->next;
    }

    return rc;
}



int Curl_SigloThreadGCSetInactive(Curl_SigloThreadContext *inactiveThreadContext)
{
    Curl_SigloThreadGC *instance = &g_garbageCollectorThread;
    int rc;
    if (instance->m_state == UNINITIALIZED)
    {
        NNC_ABORT("Adding a thread structure to the garbage collector when it is uninitialized");
    }

    /*
     * Called on the resolver thread itself, after its function returned:
     * move it from the active to the inactive list and wake the collector,
     * which waits for the thread to exit and frees it.
     *
     *
     *
     */

    nnosLockMutex(&instance->m_accessLock);

    rc = Curl_removeThreadContextFromList(instance->m_activeThreadList, inactiveThreadContext);
    if (rc != 0)
    {
        NNC_ABORT("attempting to add a thread context (%p) to the inactive list that was never placed in the active list",
                  inactiveThreadContext);
    }

    rc = Curl_llist_insert_next(instance->m_inactiveThreadList,
                                instance->m_inactiveThreadList->tail, inactiveThreadContext);
    if (rc == 0)
    {
        NNC_ABORT("unable to add an inactive thread context to the inactive thread list");
    }

    /* wake the collector */
    nnosSignalEvent(&instance->m_event);

    /* the collector takes the lock before it looks at the list, so the
       context cannot be freed before this thread has let go of it
    */

    nnosUnlockMutex(&instance->m_accessLock);

    return 0;
}

void Curl_SigloThreadGCFinalize(void)
{
    Curl_SigloThreadGC *instance = &g_garbageCollectorThread;

    if (instance->m_state == UNINITIALIZED)
    {
        NNC_ABORT("Adding a thread structure to the garbage collector when it is uninitialized");
    }
    /* one user fewer */

    nnosLockMutex(&instance->m_accessLock);

    instance->m_isFinalCount--;

    nnosUnlockMutex(&instance->m_accessLock);

    nnosSignalEvent(&instance->m_event);

    /* the collector exits once every thread is collected */
    nnosWaitThread(&instance->m_gcThreadContext->m_thread);


    if (instance->m_inactiveThreadList != NULL)
    {
        Curl_llist_destroy(instance->m_inactiveThreadList, NULL);
        instance->m_inactiveThreadList = NULL;
    }

    if (instance->m_activeThreadList != NULL)
    {
        Curl_llist_destroy(instance->m_activeThreadList, NULL);
        instance->m_activeThreadList = NULL;
    }

    nnosFinalizeMutex(&instance->m_accessLock);

    nnosFinalizeEvent(&instance->m_event);

    if (instance->m_gcThreadContext != NULL)
    {
        Curl_SigloThreadContextDestructor(instance->m_gcThreadContext);
        instance->m_gcThreadContext = NULL;
    }

    Curl_SigloThreadGCZero();
}




void Curl_SigloThreadEntryThunk(void *arg)
{
    Curl_SigloThreadContext *context = (Curl_SigloThreadContext *)arg;
    unsigned int rc;

    rc = context->m_function(context->m_functionArgument);
    (void)rc;

    /* every thread but the collector itself goes to the collector when
       it is done
    */

    if (context->m_function != Curl_SigloThreadGCRunLoop)
    {
        Curl_SigloThreadGCSetInactive(context);
    }

}



Curl_SigloThreadContext *Curl_thread_create(unsigned int (*function)(void *), void *argument)
{
    int threadCount;
    Curl_SigloThreadContext *context = NULL;
    threadCount = Curl_GetTotalThreadCount();

    if (threadCount != -1)
    {
        /* limit the number of resolver threads */

        if (threadCount >= 50)
        {

            nnsocketSetLastErrno(EAGAIN);
        }
        else
        {
            context = Curl_SigloThreadContextConstructor(4 * 1024, function, argument);


            if (context != NULL)
            {

                Curl_SigloThreadGCSetActive(context);
                nnosStartThread(&context->m_thread);
            }
        }
    }


    return context;
}

void Curl_thread_destroy(Curl_SigloThreadContext *context)
{
    (void)context;

}

int Curl_thread_join(Curl_SigloThreadContext **hnd)
{
    int rc = -1;
    Curl_SigloThreadContext *context;

    if (hnd != NULL)
    {
        context = *hnd;

        rc = (context != NULL) ? 0 : -1;
    }
    return rc;
}
