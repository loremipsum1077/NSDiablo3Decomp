// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Networking/HostNameResolver.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Networking/XNetAddress.h"

class CHostNameResolver {  // size 0x138
    enum {
        MAX_HOST_NAME_LENGTH = 256,
    };
public:
    CHostNameResolver();
    ~CHostNameResolver();
    void Cancel();
protected:
    enum {
        FLAG_RESOLVING = 1,
        FLAG_RESOLVED = 2,
    };
    uintptr m_dwParam;  // 0x00
    Blizzard::Lock::REvent m_ThreadDoneEvent;  // 0x08
    uint32 m_dwFlags;  // 0x14
    CHAR m_szHostName[256];  // 0x18
    Blizzard::Thread::Thread m_tThread;  // 0x118
    blz::vector<XNetAddress, blz::allocator<XNetAddress> > m_AddressTable;  // 0x120
};
static_assert(sizeof(CHostNameResolver) == 0x138, "CHostNameResolver");
