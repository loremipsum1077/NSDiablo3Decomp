// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRemoteTaskManager/bdStructBufferTask.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "bdRemoteTask.h"
#include "bdTaskByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdLobbyService;
class bdStructBufferSerializable;

class bdStructBufferTask : public bdRemoteTask {  // size 0x78
public:
    BD_CLASS_ALLOCATOR(bdStructBufferTask);
    bdStructBufferTask(bdLobbyService *);
    virtual ~bdStructBufferTask();  // vtable slot 0
    bdTaskByteBufferRef getTaskBuffer();
    bdBool setRequest(const bdUByte8, const bdUByte8, const bdStructBufferSerializable *);
    bdBool setResponse(bdStructBufferSerializable *);
protected:
    virtual bdBool deserializeTaskReply(const bdByteBufferRef);  // vtable slot 5
    bdStructBufferSerializable * m_structResult;  // 0x60
    bdTaskByteBufferRef m_taskBuffer;  // 0x68
    bdLobbyService * m_lobbyService;  // 0x70
};
static_assert(sizeof(bdStructBufferTask) == 0x78, "bdStructBufferTask");

typedef bdReference<bdStructBufferTask> bdStructBufferTaskRef;


// Functions defined in this file (bodies are not in the DWARF):
//   line    20  void bdStructBufferTask::operator delete(void * p)  [inline]
//   line    20  void * bdStructBufferTask::operator new(size_t nbytes)  [inline]
