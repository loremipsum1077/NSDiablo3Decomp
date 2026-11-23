// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdQoS/bdQoSRemoteAddr.h

#pragma once

#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../bdCore/bdSocket/bdSecurityID.h"
#include "../../bdCore/bdSocket/bdSecurityKey.h"
#include <stddef.h>
#include <stdint.h>

class bdQoSRemoteAddr {  // size 0x20
public:
    static void operator delete(void *);
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    bdQoSRemoteAddr() {}  // line 20
    bdQoSRemoteAddr(const bdCommonAddrRef &, const bdSecurityID &, const bdSecurityKey &);
    bdCommonAddrRef m_addr;  // 0x00
    bdSecurityID m_id;  // 0x08
    bdSecurityKey m_key;  // 0x10
};
static_assert(sizeof(bdQoSRemoteAddr) == 0x20, "bdQoSRemoteAddr");


// Functions defined in this file (bodies are not in the DWARF):
//   line    14  bdQoSRemoteAddr::~bdQoSRemoteAddr()  [inlined into 3 functions]
//   line    20  bdQoSRemoteAddr::bdQoSRemoteAddr()  [inlined into 1 functions]
