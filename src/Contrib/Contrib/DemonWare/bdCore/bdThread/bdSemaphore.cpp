#include "bdSemaphore.h"
#include "../../bdPlatform/bdPlatformThread/bdPlatformSemaphore.h"
// Blank lines keep the original line numbers








bdSemaphore::bdSemaphore(bdUInt initialCount, bdUInt maxCount)
{
    m_handle = bdPlatformSemaphore::createSemaphore(initialCount, maxCount);
}

bdSemaphore::~bdSemaphore() {}



void bdSemaphore::release() { bdPlatformSemaphore::release(m_handle); }




bdBool bdSemaphore::wait()
{
    return bdPlatformSemaphore::wait(m_handle);
}


void bdSemaphore::destroy()
{
    bdPlatformSemaphore::destroy(m_handle);
    delete this;
}
