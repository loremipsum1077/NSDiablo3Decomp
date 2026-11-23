// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheatChallenges.cpp
#include "bdAntiCheatChallenges.h"
#include "bdAntiCheatChallenge.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdContainers/bdArray.h"







bdAntiCheatChallenges::bdAntiCheatChallenges() : m_challenges(0) {}




bdAntiCheatChallenges::~bdAntiCheatChallenges() {
    delete m_challenges;
}





bdBool bdAntiCheatChallenges::deserialize(bdByteBufferRef buffer) {
    bdArray<bdAntiCheatChallenge> * existing = m_challenges;  // invented name: not in the DWARF
    bdUInt16 numChallenges = 0;
    bdAntiCheatChallenge emptyChallenge;
    bdBool ok = !existing && buffer->readUInt16(numChallenges);
    if (ok) {
        m_challenges = new bdArray<bdAntiCheatChallenge>(numChallenges, emptyChallenge);
        for (bdUInt16 i = 0; i < numChallenges && ok; ++i) {
            ok = ok && (*m_challenges)[i].deserialize(buffer);
        }
    }
    return ok;
}
