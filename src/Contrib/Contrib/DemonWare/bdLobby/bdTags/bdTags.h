// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTags/bdTags.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdFileID;
class bdRemoteTaskManager;
class bdTag;
class bdTagsArray;
class bdTaskResult;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdTags {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdTags);
    ~bdTags();
    static const bdUInt32 BD_TAG_CONTENT_SERVER_USER_STORAGE = 1;
    static const bdUInt32 BD_TAG_CONTENT_SERVER_POOLED_STORAGE = 2;
    static const bdUInt32 BD_TAG_BINARY_EVENT_LOG = 3;
    static const bdUInt32 BD_TAG_CONTENT_SERVER_PUBLISHER_STORAGE = 6;
    bdRemoteTaskRef getTagsForEntityIDs(const bdUInt32, const bdUInt32, const bdEntityID *, bdTagsArray *);
    bdRemoteTaskRef setTagsForEntityID(const bdUInt32, const bdEntityID, const bdUInt, const bdTag *);
    bdRemoteTaskRef appendTagsToEntityID(const bdUInt32, const bdEntityID, const bdUInt, const bdTag *);
    bdRemoteTaskRef removeTagsForEntityID(const bdUInt32, const bdEntityID, const bdUInt, const bdTag *);
    bdRemoteTaskRef removeAllTagsForEntityID(const bdUInt32, const bdEntityID);
    bdRemoteTaskRef searchByTags(const bdUInt32, const bdUInt, const bdUInt, const bdBool, const bdUInt, const bdTag *, bdFileID *);
protected:
    bdTags(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
private:
    bdUInt countValidTags(const bdUInt numTags, const bdTag * tags);
    bdRemoteTaskRef searchByTagsBase(const bdUInt32, const bdUInt, const bdUInt, const bdBool, const bdUInt, const bdTag *, bdTaskResult *);
};
static_assert(sizeof(bdTags) == 0x8, "bdTags");


// Functions defined in this file (bodies are not in the DWARF):
//   line    42  void bdTags::operator delete(void * p)  [inlined into 1 functions]
//   line    42  void * bdTags::operator new(size_t nbytes)  [inline]
