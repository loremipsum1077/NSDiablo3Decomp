// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformPatterns/bdLinkable.h

#pragma once

#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdLinkable {  // size 0x20
    // vtable pointer
public:
    bdLinkable();
    virtual ~bdLinkable();  // vtable slot 0
    void link(bdLinkable ** head);
    void unlink();
    bdLinkable * getNext() const;
    static bdBool find(const bdLinkable *const list, const bdLinkable *const link);
protected:
    bdLinkable * m_previous;  // 0x08
    bdLinkable * m_next;  // 0x10
    bdLinkable ** m_headPtr;  // 0x18
};
static_assert(sizeof(bdLinkable) == 0x20, "bdLinkable");

