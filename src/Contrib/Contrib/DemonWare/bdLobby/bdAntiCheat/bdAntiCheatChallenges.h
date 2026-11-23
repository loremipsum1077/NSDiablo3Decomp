// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheatChallenges.h

#pragma once

#include "../../bdCore/bdContainers/bdArray.h"
#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAntiCheatChallenge;

class bdAntiCheatChallenges : public bdReferencable {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdAntiCheatChallenges);
    bdAntiCheatChallenges();
    virtual ~bdAntiCheatChallenges();  // vtable slot 0
    bdBool deserialize(bdByteBufferRef buffer);
    const bdAntiCheatChallenge & getChallengeByIndex(bdUInt) const;
    bdUInt getNumChallenges() const;
private:
    bdArray<bdAntiCheatChallenge> * m_challenges;  // 0x10
};
static_assert(sizeof(bdAntiCheatChallenges) == 0x18, "bdAntiCheatChallenges");

typedef bdReference<bdAntiCheatChallenges> bdAntiCheatChallengesRef;


// Functions defined in this file (bodies are not in the DWARF):
//   line    19  void bdAntiCheatChallenges::operator delete(void * p)  [inlined into 1 functions]
//   line    19  void * bdAntiCheatChallenges::operator new(size_t nbytes)  [inlined into 1 functions]
