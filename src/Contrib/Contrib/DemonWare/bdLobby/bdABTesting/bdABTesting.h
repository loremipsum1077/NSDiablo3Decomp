// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdABTesting/bdABTesting.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include <stddef.h>
#include <stdint.h>

class bdABTestingEnrollRequest;
class bdABTestingEnrollResponse;
class bdLobbyService;
class bdRemoteTask;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdABTesting {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdABTesting);
    bdABTesting(bdRemoteTaskManager *const, bdLobbyService *const);
    ~bdABTesting();
    bdRemoteTaskRef enroll(bdABTestingEnrollRequest &, bdABTestingEnrollResponse &);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdLobbyService * m_lobbyService;  // 0x08
};
static_assert(sizeof(bdABTesting) == 0x10, "bdABTesting");


// Functions defined in this file (bodies are not in the DWARF):
//   line    28  void bdABTesting::operator delete(void * p)  [inlined into 1 functions]
//   line    28  void * bdABTesting::operator new(size_t nbytes)  [inline]
