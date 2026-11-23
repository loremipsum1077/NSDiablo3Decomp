// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdDTLSRandom.h

#pragma once

#include "../../bdCore/bdContainers/bdSingleton.h"
#include "../../bdCore/bdCrypto/bdCryptoUtils.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include <stddef.h>
#include <stdint.h>

class bdDTLSRandom {  // size 0x21
public:
    BD_CLASS_ALLOCATOR(bdDTLSRandom);
    bdDTLSRandom()
    {
        bdMemset(m_dtlsRandom, 0, sizeof(m_dtlsRandom));
        m_initialized = false;
    }
    bdDTLSRandom(const bdDTLSRandom & other)
    {
        bdMemcpy(m_dtlsRandom, other.m_dtlsRandom, sizeof(m_dtlsRandom));
        m_initialized = true;
    }
    ~bdDTLSRandom()
    {
        bdCryptoUtils::zeroBuffer(m_dtlsRandom, sizeof(m_dtlsRandom));
    }
    void generate()
    {
        bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(m_dtlsRandom, sizeof(m_dtlsRandom));
        m_initialized = true;
    }
    bdBool setData(const bdUByte8 (&data)[32])
    {
        bdMemcpy(m_dtlsRandom, data, sizeof(m_dtlsRandom));
        m_initialized = true;
        return true;
    }
    const bdUByte8 (&getData() const)[32] { return m_dtlsRandom; }
    bdDTLSRandom & operator=(const bdDTLSRandom & other)
    {
        bdMemcpy(m_dtlsRandom, other.m_dtlsRandom, sizeof(m_dtlsRandom));
        m_initialized = true;
        return *this;
    }
    bdBool operator==(const bdDTLSRandom & other) const
    {
        return bdCryptoUtils::constTimeCompare(m_dtlsRandom, other.m_dtlsRandom, sizeof(m_dtlsRandom));
    }
    bdBool operator!=(const bdDTLSRandom & other) const
    {
        return !(*this == other);
    }
protected:
    bdUByte8 m_dtlsRandom[32];  // 0x00
    bdBool m_initialized;  // 0x20
};
static_assert(sizeof(bdDTLSRandom) == 0x21, "bdDTLSRandom");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  bdDTLSRandom::bdDTLSRandom()  [inlined into 2 functions]
//   line    35  bdDTLSRandom::bdDTLSRandom(const bdDTLSRandom & other)  [inlined into 4 functions]
//   line    42  bdDTLSRandom::~bdDTLSRandom()  [inlined into 6 functions]
//   line    48  void bdDTLSRandom::generate()  [inlined into 2 functions]
//   line    55  bdBool bdDTLSRandom::setData(const bdUByte8 (&data)[32])  [inlined into 2 functions]
//   line    70  bdDTLSRandom & bdDTLSRandom::operator=(const bdDTLSRandom & other)  [inlined into 2 functions]
//   line    78  bdBool bdDTLSRandom::operator==(const bdDTLSRandom & other) const  [inlined into 1 functions]
//   line    85  bdBool bdDTLSRandom::operator!=(const bdDTLSRandom & other) const  [inlined into 1 functions]

// Variables defined in this file:
//   line    17  static const bdUInt BD_DTLS_RANDOM_SIZE
