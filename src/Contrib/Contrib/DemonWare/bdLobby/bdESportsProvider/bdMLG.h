// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdESportsProvider/bdMLG.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include <stddef.h>
#include <stdint.h>

class bdBoolResult;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdMLG {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdMLG);
    bdMLG(bdRemoteTaskManager *const);
    ~bdMLG();
    bdRemoteTaskRef isUserRegistered(bdBoolResult *const);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdMLG) == 0x8, "bdMLG");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  void bdMLG::operator delete(void * p)  [inlined into 1 functions]
//   line    22  void * bdMLG::operator new(size_t nbytes)  [inline]
