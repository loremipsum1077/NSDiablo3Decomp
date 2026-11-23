// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheatChallenge.cpp
#include "bdAntiCheatChallenge.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdContainers/bdArray.inl"









bdAntiCheatChallenge::bdAntiCheatChallenge()
    : m_functionID(0),
      m_challengeID(0), m_parameters()
{
}


bdAntiCheatChallenge::bdAntiCheatChallenge(const bdAntiCheatChallenge & other)
    : m_functionID(other.m_functionID),
      m_challengeID(other.m_challengeID),
      m_parameters(other.m_parameters)
{
}


bdAntiCheatChallenge::~bdAntiCheatChallenge()
{
}

bdBool bdAntiCheatChallenge::deserialize(bdByteBufferRef buffer)
{



    bdUInt16 numParams = 0;
    bdAntiCheatChallengeParam param;

    bdBool ok = buffer->readUInt16(m_functionID);
    ok = ok && buffer->readUInt64(m_challengeID);
    ok = ok && buffer->readUInt16(numParams);

    m_parameters.ensureCapacity(numParams);
    for (bdUInt i = 0; i < numParams && ok; ++i)
    {
        m_parameters.setGrow(i, param);
        ok = ok && m_parameters[i].deserialize(buffer);
    }


    return ok;
}
