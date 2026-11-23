// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdGameInfoFactory.h

#pragma once

#include "../../bdCore/bdContainers/bdSingleton.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdUtilities/bdFactory.h"
#include <stddef.h>
#include <stdint.h>

class bdGameInfo;

class bdGameInfoFactoryImpl {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdGameInfoFactoryImpl);
    bdGameInfo * create() const;
    void setClass(bdCreatorBase<bdGameInfo> *const creator);
protected:
    template<class T> friend class bdSingleton;
    bdGameInfoFactoryImpl();
    ~bdGameInfoFactoryImpl();
    bdCreatorBase<bdGameInfo> * m_creator;  // 0x00
};
static_assert(sizeof(bdGameInfoFactoryImpl) == 0x8, "bdGameInfoFactoryImpl");

class bdGameInfoFactory : public bdSingleton<bdGameInfoFactoryImpl> {  // size 0x1
protected:
    BD_CLASS_ALLOCATOR(bdGameInfoFactory);
    bdGameInfoFactory();
};
static_assert(sizeof(bdGameInfoFactory) == 0x1, "bdGameInfoFactory");
