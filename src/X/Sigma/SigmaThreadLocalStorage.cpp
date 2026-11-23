#include "SigmaThreadLocalStorage.h"

#include <new>

#include "Memory/SigmaMemory.h"

SigmaThreadLocalStorage::SigmaThreadLocalStorage() : m_ptTlsSlot(NULL), m_nTlsIndex(0xFFFFFFFF) {}

void SigmaThreadLocalStorage::Initialize() {
    m_ptTlsSlot = new Blizzard::Thread::TLSSlot;
    m_ptTlsSlot->Prepare();
    BOOL bAllocated = Blizzard::Thread::AllocateLocalStorage(m_ptTlsSlot);
    (void)bAllocated;
}

void SigmaThreadLocalStorage::Dispose() {
    Blizzard::Thread::FreeLocalStorage(m_ptTlsSlot);
    SigmaMemoryFree(m_ptTlsSlot, NULL);
    m_ptTlsSlot = NULL;
}
