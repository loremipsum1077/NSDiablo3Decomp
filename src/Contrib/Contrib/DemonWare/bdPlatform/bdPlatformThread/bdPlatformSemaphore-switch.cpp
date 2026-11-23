#include "bdPlatformSemaphore.h"
// Blank lines keep the original line numbers

namespace nn { namespace os {
void InitializeSemaphore(SemaphoreType*, int, int);
void FinalizeSemaphore(SemaphoreType*);
void AcquireSemaphore(SemaphoreType*);
void ReleaseSemaphore(SemaphoreType*);
} }



bdSemaphoreHandle bdPlatformSemaphore::createSemaphore(bdUInt32 initialCount, bdUInt32 maxCount)
{





    bdSemaphoreHandle handle(new nn::os::SemaphoreType());
    nn::os::InitializeSemaphore(handle.get(), initialCount, maxCount);
    return handle;
}

void bdPlatformSemaphore::destroy(bdSemaphoreHandle& handle)
{
    nn::os::FinalizeSemaphore(handle.get());
}

void bdPlatformSemaphore::release(bdSemaphoreHandle& handle)
{
    nn::os::ReleaseSemaphore(handle.get());
}

bdBool bdPlatformSemaphore::wait(bdSemaphoreHandle& handle)
{
    nn::os::AcquireSemaphore(handle.get());
    return true;
}
