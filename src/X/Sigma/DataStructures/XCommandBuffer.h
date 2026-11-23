#pragma once

#include "DataStructuresExternal.h"

#include <stddef.h>
#include <stdint.h>

class XCommandBuffer {
    typedef void (*Func)(const void*);

public:
    inline XCommandBuffer(const char* szName, const size_t uSize);
    inline ~XCommandBuffer();

    inline void* Reserve(const Func pFunc, const size_t uSizeIn);
    inline void Flush();

    struct Command {
        Func pFunc;
        size_t uSize;
    };

private:
    const char* m_szName;
    uintptr_t m_uAddress;
    size_t m_uSize;
    size_t m_uOffset;
};























inline XCommandBuffer::XCommandBuffer(const char* szName, const size_t uSize)


    : m_szName(szName),
      m_uAddress(0),
      m_uSize(uSize),
      m_uOffset(0) {

    void* pMemory = SigmaMemoryNew(uSize, 8, NULL, FALSE);
    m_uAddress = reinterpret_cast<uintptr_t>(pMemory);
}

inline XCommandBuffer::~XCommandBuffer() {


    void* pMemory = reinterpret_cast<void*>(m_uAddress);
    SigmaMemoryFree(pMemory, NULL);
}

inline void* XCommandBuffer::Reserve(const Func pFunc, const size_t uSizeIn) {



    const size_t uSize = (sizeof(Command) + uSizeIn + 7) & ~7;
    if (m_uSize < m_uOffset + uSize) {

        m_uSize *= 2;
        void* pMemory = reinterpret_cast<void*>(m_uAddress);
        pMemory = SigmaMemoryResize(pMemory, NULL, m_uSize, 8);
        m_uAddress = reinterpret_cast<uintptr_t>(pMemory);
    }



    const uintptr_t uAddress = m_uAddress + m_uOffset;
    m_uOffset += uSize;

    Command* pCommand = reinterpret_cast<Command*>(uAddress);
    pCommand->pFunc = pFunc;
    pCommand->uSize = uSize;

    void* pData = reinterpret_cast<void*>(uAddress + sizeof(Command));
    return pData;
}

inline void XCommandBuffer::Flush() {
    size_t uOffset = 0;

    while (uOffset < m_uOffset) {

        const uintptr_t uAddress = m_uAddress + uOffset;
        const Command* pCommand = reinterpret_cast<const Command*>(uAddress);
        const void* pData = reinterpret_cast<const void*>(uAddress + sizeof(Command));
        pCommand->pFunc(pData);
        uOffset += pCommand->uSize;
    }
    m_uOffset = 0;
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XCommandBuffer) == 0x20, "XCommandBuffer");
