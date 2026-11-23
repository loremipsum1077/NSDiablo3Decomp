// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdStructBufferDeserializerHelpers.h
#pragma once
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdStructBufferDeserializer.h"
#include "bdStructBufferSerializable.h"

class bdStructBufferSerializable;



class bdStructBufferDeserializerHelpers {  // size 0x1
public:





    // Defined in the class (DWARF decl line 22, params 23/24, no DW_AT_inline): the original
    // emits it out of line at 0x7100b378a0 and calls it from handleAchievementsUpdated.
    static bdBool deserializeFromBuffer(
        bdByteBufferRef buffer,
        bdStructBufferSerializable & serializable)
    {
        bdUInt32 bufferSize = 0;
        bdBool ok = buffer->readStructStart(bufferSize);
        if (ok)
        {


            bdStructBufferDeserializer deserializer(buffer, bufferSize, 0);
            ok = serializable.deserialize(deserializer);
        }
        return ok && buffer->readStructEnd();  // INFERENCE: one statement on line 35; the epilogue sits on the closing brace, line 36
    }
};
static_assert(sizeof(bdStructBufferDeserializerHelpers) == 0x1, "bdStructBufferDeserializerHelpers");
