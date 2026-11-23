// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdGetHostByName/bdGetHostByName.h

#pragma once

#include "../../bdCore/bdThread/bdRunnable.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "bdGetHostByNameConfig.h"
#include "../../bdPlatform/bdPlatformConfig/bdPlatformConfig-switch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdPlatform/bdPlatformSocket/bdSockAddr.h"
#include <stddef.h>
#include <stdint.h>
#include "../../bdCore/bdMemory/bdMemory-std.h"
class bdThread;

class bdGetHostByName : public bdRunnable {  // size 0x248
public:
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t nbytes);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    enum bdStatus {
        BD_LOOKUP_UNINITIALIZED = 0,
        BD_LOOKUP_PENDING = 1,
        BD_LOOKUP_SUCCEEDED = 2,
        BD_LOOKUP_FAILED = 3,
        BD_LOOKUP_CANCELLED = 4,
        BD_LOOKUP_TIMED_OUT = 5,
        BD_LOOKUP_ERROR = 6,
    };
    bdGetHostByName(bdUInt);
    virtual ~bdGetHostByName();  // vtable slot 0
    bdBool start(const bdNChar8 * hostname, bdGetHostByNameConfig config);
    void pump();
    void quit();
    bdGetHostByName::bdStatus getStatus() const;
    bdUInt getNumAddresses() const;
    bdInAddr getAddressAt(bdUInt index) const;
protected:
    virtual bdUInt run(void * args);  // vtable slot 2
    void cancelLookup();
    bdStopwatch m_timer;  // 0x10
    bdGetHostByName::bdStatus m_status;  // 0x18
    bdThread * m_thread;  // 0x20
    bdGetHostByNameConfig m_config;  // 0x28
    bdInAddr m_addresses[4];  // 0x2c
    bdUInt m_numAddresses;  // 0x23c
    bdDNSHandle m_handle;  // 0x240
    bdUInt m_threadStackSize;  // 0x244
};
static_assert(sizeof(bdGetHostByName) == 0x248, "bdGetHostByName");


// Functions defined in this file (bodies are not in the DWARF):
//   line    31  void bdGetHostByName::operator delete(void * p)  [inlined into 1 functions]
//   line    31  void * bdGetHostByName::operator new(size_t nbytes)  [inlined into 1 functions]
