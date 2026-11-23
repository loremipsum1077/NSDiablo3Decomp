// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTitleUtilities/bdTimeStamp.cpp
// Blank lines keep the original line numbers.
#include "bdTimeStamp.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"








bdTimeStamp::bdTimeStamp() : m_timeStamp(0) {}





bdBool bdTimeStamp::deserialize(bdByteBufferRef buffer)
{
    return buffer->readUInt32(m_timeStamp);
}



bdUInt bdTimeStamp::sizeOf() { return sizeof(bdTimeStamp); }
