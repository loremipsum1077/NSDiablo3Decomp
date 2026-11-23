// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdDDL/bdDDL.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDDLChecksumResult;
class bdDDLDefinitionChecksum;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdDDL {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdDDL);
    bdDDL(bdRemoteTaskManager *const);
    ~bdDDL();
    bdRemoteTaskRef verifyDDLFiles(const bdUInt32, bdDDLDefinitionChecksum *, bdDDLChecksumResult *);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdDDL) == 0x8, "bdDDL");


// Functions defined in this file (bodies are not in the DWARF):
//   line    30  void bdDDL::operator delete(void * p)  [inlined into 1 functions]
//   line    30  void * bdDDL::operator new(size_t nbytes)  [inline]
