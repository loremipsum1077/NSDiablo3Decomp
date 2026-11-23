// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSecurityKeyMapListener.h

#pragma once

#include <stddef.h>
#include <stdint.h>

class bdSecurityID;

class bdSecurityKeyMapListener {  // size 0x8
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    virtual void onSecurityKeyRemove(const bdSecurityID &) = 0;  // vtable slot 0
    virtual ~bdSecurityKeyMapListener() {}  // vtable slot 0; no out-of-line copy in the binary
};
static_assert(sizeof(bdSecurityKeyMapListener) == 0x8, "bdSecurityKeyMapListener");

