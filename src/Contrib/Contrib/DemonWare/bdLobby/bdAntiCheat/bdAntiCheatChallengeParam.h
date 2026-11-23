// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheatChallengeParam.h

#pragma once

#include "../../bdCore/bdContainers/bdArray.h"
#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAntiCheatChallengeParam {  // size 0x18
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdAntiCheatChallengeParam);
    bdAntiCheatChallengeParam();
    bdAntiCheatChallengeParam(const bdUByte8 *, bdUInt32);
    virtual ~bdAntiCheatChallengeParam();  // vtable slot 0
    bdAntiCheatChallengeParam(const bdAntiCheatChallengeParam &);
    bdBool operator==(const bdAntiCheatChallengeParam & other) const;
    bdBool operator!=(const bdAntiCheatChallengeParam &) const;
    bdAntiCheatChallengeParam & operator=(const bdAntiCheatChallengeParam & other);
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUByte8 * m_data;  // 0x08
    bdUInt32 m_size;  // 0x10
};
static_assert(sizeof(bdAntiCheatChallengeParam) == 0x18, "bdAntiCheatChallengeParam");

typedef bdArray<bdAntiCheatChallengeParam> bdChallengeParams;


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  void bdAntiCheatChallengeParam::operator delete(void * p)  [inlined into 1 functions]
