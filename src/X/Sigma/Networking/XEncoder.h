// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Networking/XEncoder.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

#include "XEncodeDecodeUtil.h"

// Inline bodies are not in the DWARF; written from their uses.






class XEncoder {  // size 0x20
    // vtable pointer
public:
    XEncoder() : m_data(NULL), m_size(0), m_used(0) {}
    XEncoder(void * data, size_t sizeBytes) { SetData(data, sizeBytes); }
    virtual ~XEncoder() {}  // vtable slot 0
    void ZeroUnused();
    void ZeroUnusedByte();
    void SetData(void * d, size_t sizeBytes);
    void SetUsed(size_t u);
    const void * Data() const;
    void * Data() { return m_data; }
    size_t Size() const { return m_size; }
    size_t Used() const { return m_used; }
    size_t SizeBytes() const { return m_size >> 3; }
    size_t UsedBytes() const { return BitsToBytes(m_used); }
    void Align8();
    void Put(uint32 x, size_t bits);
    void PutAligned8(uint32 x, size_t bits);
    void PutU64(uint64 v, size_t size);
    void PutBits(const uint8 *, size_t);
    void PutBytesAligned8(const uint8 * data, size_t size);
    void PutAsciiStringBytesAligned8(const uint8 * data, size_t length);
    void PutStringBytesAligned8(const uint8 *, size_t);
    void PutBool(BOOL b);
    void Put(uint32 u);
    void Put(uint64);
protected:
    virtual void DataOverflow();  // vtable slot 2
private:
    uint8 * m_data;  // 0x08
    size_t m_size;  // 0x10
    size_t m_used;  // 0x18
};
static_assert(sizeof(XEncoder) == 0x20, "XEncoder");
