// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdTaskParams.h

#pragma once

#include "bdBufferParams.h"
#include "bdLobbyServicesCommon.h"
#include "../bdRemoteTaskManager/bdTaskByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdTaskResult;
class bdUserAccountID;

class bdTaskParams : public bdBufferParams {  // size 0x50
    friend class bdRemoteTaskManager;
public:
    BD_CLASS_ALLOCATOR(bdTaskParams);
    bdTaskParams(bdUByte8, bdUByte8, bdUInt, bdUInt);
    virtual ~bdTaskParams();  // vtable slot 0
    bdTaskByteBufferRef getTaskBuffer();
    void addContextUser(const bdUserAccountID & userID);
    void addContextUser(const bdOnlineUserID &);
    void addContext(const bdNChar8 * context);
    void bindResults(bdTaskResult * results, bdUInt maxResults);
    void bindResultRefs(bdTaskResult ** results, bdUInt maxResults);
    bdBool getResultBindings(bdTaskResult *& resultList, bdTaskResult **& resultRefsList, bdUInt & maxResults) const;
    bdUByte8 getServiceID() const;
    bdUByte8 getTaskID() const;
protected:
    bdUByte8 m_serviceID;  // 0x28
    bdUByte8 m_taskID;  // 0x29
    bdTaskResult * m_taskResults;  // 0x30
    bdTaskResult ** m_taskResultRefs;  // 0x38
    bdUInt m_maxResults;  // 0x40
    bdTaskByteBufferRef m_taskBuffer;  // 0x48
    virtual bdBool ensureCapacity(bdUInt paramSize, bdBool typeChecked);  // vtable slot 2
private:
    bdTaskParams(const bdTaskParams &);
};
static_assert(sizeof(bdTaskParams) == 0x50, "bdTaskParams");


// Functions defined in this file (bodies are not in the DWARF):
//   line    44  void bdTaskParams::operator delete(void * p)  [inlined into 1 functions]

#include "bdTaskParams.inl"
