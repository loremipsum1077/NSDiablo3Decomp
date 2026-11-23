// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheatChallenge.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "bdAntiCheatChallengeParam.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAntiCheatChallenge {  // size 0x28
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdAntiCheatChallenge);
    bdAntiCheatChallenge();
    bdAntiCheatChallenge(const bdAntiCheatChallenge &);
    virtual ~bdAntiCheatChallenge();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    bdUInt16 m_functionID;  // 0x08
    bdUInt64 m_challengeID;  // 0x10
    bdChallengeParams m_parameters;  // 0x18
};
static_assert(sizeof(bdAntiCheatChallenge) == 0x28, "bdAntiCheatChallenge");


// Functions defined in this file (bodies are not in the DWARF):
//   line    20  void bdAntiCheatChallenge::operator delete(void * p)  [inlined into 1 functions]
