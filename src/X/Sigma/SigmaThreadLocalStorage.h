#pragma once

#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "SigmaTypes.h"

class SigmaThreadLocalStorage {
public:
    SigmaThreadLocalStorage();
    void Initialize();
    void Dispose();
    BOOL IsInitialized();
    void* TLSGetValue();
    void TLSSetValue(void* p);

private:
    Blizzard::Thread::TLSSlot* m_ptTlsSlot;
    uint32 m_nTlsIndex;
};

inline BOOL SigmaThreadLocalStorage::IsInitialized() {
    return m_ptTlsSlot != NULL;
}

inline void* SigmaThreadLocalStorage::TLSGetValue() {
    return Blizzard::Thread::GetLocalStorage(m_ptTlsSlot);
}

inline void SigmaThreadLocalStorage::TLSSetValue(void* p) {
    Blizzard::Thread::SetLocalStorage(m_ptTlsSlot, p);
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(SigmaThreadLocalStorage) == 0x10, "SigmaThreadLocalStorage");
