// Adopted from the DWARF.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdSerializable.h

#pragma once

#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdSerializable : public virtual bdReferencable {  // size 0x18
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdSerializable);
    bdSerializable();
    virtual ~bdSerializable();  // vtable slot 0
    virtual bdBool serialize(bdByteBuffer &) const = 0;  // vtable slot 2
    virtual bdInt serializedSizeOf() const;  // vtable slot 3
protected:
    bdSerializable(const bdSerializable &);
};
static_assert(sizeof(bdSerializable) == 0x18, "bdSerializable");


// Functions defined in this file (bodies are not in the DWARF):
//   line    23  bdSerializable & bdSerializable::operator=(const bdSerializable &)  [inlined into 4 functions]
//   line    27  void bdSerializable::operator delete(void * p)  [inline]
