// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdSerializable.cpp
#include "bdSerializable.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdSerializable::bdSerializable() {}
bdSerializable::~bdSerializable() {}
bdSerializable::bdSerializable(const bdSerializable &) {}

bdInt bdSerializable::serializedSizeOf() const
{
    bdByteBuffer emptyBuffer(0, true);
    bdBool serializeOk = serialize(emptyBuffer);
    if (!serializeOk)
    {
        // Text inferred; release logging preserves only format arguments.
        bdLogWarn("bdSerializable", "Failed to serialize.");
    }
    bdUInt bufSize = emptyBuffer.getSize();
    serializeOk = serializeOk && static_cast<bdInt>(bufSize) >= 0;
    return serializeOk ? static_cast<bdInt>(bufSize) : -1;
}
