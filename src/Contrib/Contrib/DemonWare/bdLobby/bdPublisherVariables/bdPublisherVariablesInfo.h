// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdPublisherVariables/bdPublisherVariablesInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdPublisherVariables;

class bdPublisherVariablesInfo : public bdTaskResult {  // size 0x20
public:
    BD_CLASS_ALLOCATOR(bdPublisherVariablesInfo);
    bdPublisherVariablesInfo(bdPublisherVariables *);
    virtual ~bdPublisherVariablesInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
protected:
    bdPublisherVariablesInfo();
    bdPublisherVariables * m_requestingService;  // 0x08
};
static_assert(sizeof(bdPublisherVariablesInfo) == 0x20, "bdPublisherVariablesInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  bdPublisherVariablesInfo & bdPublisherVariablesInfo::operator=(const bdPublisherVariablesInfo &)  [inline]
//   line    21  void bdPublisherVariablesInfo::operator delete(void * p)  [inlined into 1 functions]
