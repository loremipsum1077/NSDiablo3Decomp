#include "bdMutex.h"
#include "../../bdPlatform/bdPlatformThread/bdPlatformMutex.h"
// Blank lines keep the original line numbers








bdMutex::bdMutex()
    : m_handle(bdPlatformMutex::createMutex())
{
}

bdMutex::~bdMutex()
{
    bdPlatformMutex::destroy(m_handle);
}

bdBool bdMutex::lock()
{
    return bdPlatformMutex::lock(m_handle);
}

bdBool bdMutex::unlock()
{
    return bdPlatformMutex::unlock(m_handle);
}
