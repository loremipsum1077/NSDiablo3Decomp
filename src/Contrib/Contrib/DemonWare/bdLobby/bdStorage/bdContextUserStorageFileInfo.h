// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdContextUserStorageFileInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "bdFileInfo.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdContextUserStorageFileInfo : public bdTaskResult {  // size 0xe8
public:
    BD_CLASS_ALLOCATOR(bdContextUserStorageFileInfo);
    bdContextUserStorageFileInfo();
    virtual ~bdContextUserStorageFileInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUInt32 m_createTime;  // 0x08
    bdUInt32 m_modifedTime;  // 0x0c
    bdFileInfo::bdVisibility m_visibility;  // 0x10
    bdUserID m_ownerID;  // 0x18
    bdNChar8 m_fileName[128];  // 0x50
    bdUInt32 m_fileSize;  // 0xd0
};
static_assert(sizeof(bdContextUserStorageFileInfo) == 0xe8, "bdContextUserStorageFileInfo");

