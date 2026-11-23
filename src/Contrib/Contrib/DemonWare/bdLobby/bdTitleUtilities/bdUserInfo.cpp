// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdUserInfo.cpp
// Blank lines keep the original line numbers.
#include "bdUserInfo.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"








bdUserInfo::bdUserInfo() : m_userID(0) {}




bdUserInfo::~bdUserInfo() {}




bdBool bdUserInfo::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readUInt64(m_userID);
    return ok && buffer->readString(m_userName, sizeof(m_userName));
}



bdUInt bdUserInfo::sizeOf() { return sizeof(bdUserInfo); }
