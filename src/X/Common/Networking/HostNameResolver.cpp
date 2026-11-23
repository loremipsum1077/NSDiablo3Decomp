// Original file: X/Common/Networking/HostNameResolver.cpp

#include "HostNameResolver.h"

CHostNameResolver::CHostNameResolver()
    : m_dwParam(0), m_dwFlags(0), m_tThread(0) {
    m_szHostName[0] = '\0';
}

CHostNameResolver::~CHostNameResolver() {
    if (m_dwFlags & FLAG_RESOLVING)
        Blizzard::Thread::Close(m_tThread);
    Cancel();
}

void CHostNameResolver::Cancel() {
    m_ThreadDoneEvent.Wait(0);
    m_dwFlags &= ~(FLAG_RESOLVING | FLAG_RESOLVED);
    m_AddressTable.clear();
}
