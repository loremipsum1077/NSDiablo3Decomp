// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStoreACL.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdObjectStoreACL {  // size 0x4
public:
    enum bdObjectStoreACLType {
        PUBLIC = 0,
        PRIVATE = 1,
    };
public:
    bdObjectStoreACL(bdObjectStoreACL::bdObjectStoreACLType);
    bdObjectStoreACL(const bdNChar8 *);
    bdObjectStoreACL();
    ~bdObjectStoreACL();
    const bdNChar8 * serializeToString() const;
    void reset();
    static bdBool validateString(const bdNChar8 *);
protected:
    bdObjectStoreACL::bdObjectStoreACLType m_aclType;  // 0x00
};
static_assert(sizeof(bdObjectStoreACL) == 0x4, "bdObjectStoreACL");

