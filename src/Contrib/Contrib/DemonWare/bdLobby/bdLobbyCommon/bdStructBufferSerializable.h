// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdStructBufferSerializable.h

#pragma once

#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdLobbyService;
class bdStructBufferDeserializer;
class bdStructBufferSerializer;

class bdStructBufferSerializable : public virtual bdReferencable {  // size 0x18
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdStructBufferSerializable);
    bdStructBufferSerializable();
    virtual ~bdStructBufferSerializable();  // vtable slot 0
    virtual bdBool serialize(bdStructBufferSerializer & serializer) const;  // vtable slot 2
    virtual bdBool deserialize(bdStructBufferDeserializer & deserializer);  // vtable slot 3
    virtual bdBool serializeWithLobbyService(bdStructBufferSerializer & serializer, bdLobbyService * lobbyService) const;  // vtable slot 4
    virtual bdBool deserializeWithLobbyService(bdStructBufferDeserializer & deserializer, bdLobbyService * lobbyService);  // vtable slot 5
protected:
    bdStructBufferSerializable(const bdStructBufferSerializable &);
};
static_assert(sizeof(bdStructBufferSerializable) == 0x18, "bdStructBufferSerializable");

