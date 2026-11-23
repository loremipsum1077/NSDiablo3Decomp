#include "bdPlatformMutex.h"
// Blank lines keep the original line numbers

namespace nn { namespace os {
void InitializeMutex(MutexType*, bool, int);
void FinalizeMutex(MutexType*);
void LockMutex(MutexType*);
void UnlockMutex(MutexType*);
} }



bdMutexHandle bdPlatformMutex::createMutex()
{
    bdMutexHandle handle;
    nn::os::InitializeMutex(&handle, true, 0);
    return handle;
}

bdBool bdPlatformMutex::lock(bdMutexHandle& handle)
{
    nn::os::LockMutex(&handle);
    return true;
}

bdBool bdPlatformMutex::unlock(bdMutexHandle& handle)
{
    nn::os::UnlockMutex(&handle);
    return true;
}

bdBool bdPlatformMutex::destroy(bdMutexHandle& handle)
{
    nn::os::FinalizeMutex(&handle);
    return true;
}
