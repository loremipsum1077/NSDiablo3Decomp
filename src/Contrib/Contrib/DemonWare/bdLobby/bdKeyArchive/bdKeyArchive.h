// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdKeyArchive/bdKeyArchive.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdEntityIDKeyArchiveMap;
class bdEntityIDKeyValueTriplet;
class bdKeyValuePair;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdKeyArchive {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdKeyArchive);
    ~bdKeyArchive();
    bdRemoteTaskRef write(const bdEntityID, const bdUInt16, const bdKeyValuePair *, const bdUInt);
    bdRemoteTaskRef read(const bdEntityID, const bdUInt16, const bdBool, bdKeyValuePair *, const bdUInt);
    bdRemoteTaskRef readAll(const bdEntityID, const bdUInt16, const bdBool, bdKeyValuePair *, const bdUInt);
    bdRemoteTaskRef readMultipleEntityIDs(const bdEntityID *, const bdUInt, const bdUInt16, const bdBool, const bdUInt16 *, const bdUInt, bdEntityIDKeyArchiveMap **);
    bdRemoteTaskRef writeMultipleEntityIDs(const bdEntityIDKeyValueTriplet *, const bdUInt32, const bdUInt16);
    bdRemoteTaskRef writeMultipleEntityIDs(const bdEntityID *, const bdKeyValuePair *, const bdUInt32, const bdUInt16);
protected:
    bdKeyArchive(bdRemoteTaskManager *const);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdKeyArchive) == 0x8, "bdKeyArchive");


// Functions defined in this file (bodies are not in the DWARF):
//   line    36  void bdKeyArchive::operator delete(void * p)  [inlined into 1 functions]
//   line    36  void * bdKeyArchive::operator new(size_t nbytes)  [inline]
