// Original file: Contrib/Contrib/DemonWare/bdCore/bdCore.cpp
#include "bdCore.h"
#include "bdContainers/bdSingleton.h"
#include "bdMemory/bdMallocMemory-std.h"
#include "bdMemory/bdMemory-std.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdBool bdCore::m_initialized = false;








void bdCore::init(const bdBool defaultMemoryFunctions)
{
    if (!m_initialized)
    {
        if (defaultMemoryFunctions)
        {

            bdMemory::setAllocateFunc(bdMallocMemory::allocate);
            bdMemory::setAlignedAllocateFunc(bdMallocMemory::alignedAllocate);

            bdMemory::setDeallocateFunc(bdMallocMemory::deallocate);
            bdMemory::setAlignedDeallocateFunc(bdMallocMemory::alignedDeallocate);

            bdMemory::setReallocateFunc(bdMallocMemory::reallocate);
            bdMemory::setAlignedReallocateFunc(bdMallocMemory::alignedReallocate);
        }

















        m_initialized = true;
    }
    else
    {
        bdLogWarn("core", "init() has been called twice without an intermediate quit()");
    }
}

void bdCore::quit()
{
    if (m_initialized)
    {
        bdSingleton<bdSingletonRegistryImpl>::getInstance()->cleanUp();


        bdMallocMemory::leakCheck();






        bdMemory::setAllocateFunc(NULL);
        bdMemory::setAlignedAllocateFunc(NULL);

        bdMemory::setDeallocateFunc(NULL);
        bdMemory::setAlignedDeallocateFunc(NULL);

        bdMemory::setReallocateFunc(NULL);
        bdMemory::setAlignedReallocateFunc(NULL);














        m_initialized = false;
    }
    else
    {
        bdLogWarn("core", "quit() has been called without init()");
    }
}
