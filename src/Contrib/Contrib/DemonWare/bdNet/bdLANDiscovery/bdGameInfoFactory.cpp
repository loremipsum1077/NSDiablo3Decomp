// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdGameInfoFactory.cpp

#include "bdGameInfoFactory.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"






template <> bdGameInfoFactoryImpl * bdSingleton<bdGameInfoFactoryImpl>::m_instance = NULL;



bdGameInfo * bdGameInfoFactoryImpl::create() const
{




    return m_creator->create();
}


void bdGameInfoFactoryImpl::setClass(bdCreatorBase<bdGameInfo> *const creator)
{
    if (m_creator)
    {
        bdLogWarn("lan discovery", "A game info creator has already been set.");
    }
    else
    {
        m_creator = creator;
    }
}

bdGameInfoFactoryImpl::bdGameInfoFactoryImpl()
    : m_creator(NULL)
{
}

bdGameInfoFactoryImpl::~bdGameInfoFactoryImpl()
{
    if (m_creator)
    {
        delete m_creator;
    }
}
