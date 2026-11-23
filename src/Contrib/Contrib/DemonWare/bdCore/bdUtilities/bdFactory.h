// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdFactory.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

// The member bodies live in bdFactory.inl (~bdCreatorBase line 12, ~bdCreator line 24,
// create line 29, getSizeOf line 35), included at the end.

// line 19
template <typename BASE> class bdCreatorBase {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdCreatorBase);
    virtual ~bdCreatorBase();  // vtable slot 0
    virtual BASE * create() const;  // vtable slot 2
    virtual bdUInt getSizeOf() const = 0;  // vtable slot 3
};

// line 39
template <typename DERIVED, typename BASE> class bdCreator : public bdCreatorBase<BASE> {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdCreator);
    bdCreator() {}
    virtual ~bdCreator();  // vtable slot 0
    virtual BASE * create() const;  // vtable slot 2
    virtual bdUInt getSizeOf() const;  // vtable slot 3
};

#include "bdFactory.inl"
