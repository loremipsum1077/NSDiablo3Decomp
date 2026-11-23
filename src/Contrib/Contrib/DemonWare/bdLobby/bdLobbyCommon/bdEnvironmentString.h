// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdEnvironmentString.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

enum bdEnvironment {
    BD_ENVIRONMENT_DEV = 0,
    BD_ENVIRONMENT_CERT = 1,
    BD_ENVIRONMENT_PROD = 2,
    BD_ENVIRONMENT_MAX = 3,
};

class bdEnvironmentString {  // size 0xc00
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdEnvironmentString();
    bdEnvironmentString(const bdNChar8 *, const bdNChar8 *, const bdNChar8 *);
    bdEnvironmentString(const bdEnvironmentString &);
    void setAll(const bdNChar8 *);
    bdBool set(const bdEnvironment, const bdNChar8 *);
    const bdNChar8 * get(const bdEnvironment environment) const;
protected:
    bdNChar8 m_devString[1024];  // 0x00
    bdNChar8 m_certString[1024];  // 0x400
    bdNChar8 m_prodString[1024];  // 0x800
};
static_assert(sizeof(bdEnvironmentString) == 0xc00, "bdEnvironmentString");
