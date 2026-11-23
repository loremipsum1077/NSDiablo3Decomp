// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Networking/XDecoder.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

#include "XEncodeDecodeUtil.h"

class XDecoder {  // size 0x20
    // vtable pointer
public:
    // The DWARF does not record access for nested types; Result is used by XTypeDescriptor::Decode.
    enum Result {
        RESULT_VALID = 0,
        RESULT_TRUNCATED = 1,
        RESULT_CORRUPTED = 2,
    };
public:
    XDecoder();
    XDecoder(const void * data, size_t sizeBytes) { SetData(data, sizeBytes); }
    virtual ~XDecoder() {}  // vtable slot 0
    void SetData(const void * d, size_t sizeBytes);
    void SetUsed(size_t u);
    const void * Data() const { return m_data; }  // XDecoder.h:40, inlined into sReadReplayMessage
    size_t Size() const { return m_size; }
    size_t Used() const { return m_used; }
    size_t SizeBytes() const { return m_size >> 3; }
    size_t UsedBytes() const { return (m_used + 7) >> 3; }
    XDecoder::Result Align8();
    XDecoder::Result Get(uint32 & x, size_t bits);
    XDecoder::Result GetAligned8(uint32 & x, size_t bits);
    XDecoder::Result GetU64(uint64 & v, size_t bits);
    XDecoder::Result GetBits(uint8 *, size_t);
    XDecoder::Result GetBytesAligned8(uint8 * data, size_t size);
    XDecoder::Result GetAsciiStringBytesAligned8(uint8 * data, size_t length);
    XDecoder::Result GetStringBytesAligned8(uint8 *, size_t &, size_t);
    XDecoder::Result GetBool(BOOL & b);
    XDecoder::Result Get(uint32 &);
    XDecoder::Result Get(uint64 &);
protected:
    virtual XDecoder::Result DataOverflow();  // vtable slot 2
private:
    const uint8 * m_data;  // 0x08
    size_t m_size;  // 0x10
    size_t m_used;  // 0x18
};
static_assert(sizeof(XDecoder) == 0x20, "XDecoder");


// Functions defined in this file (bodies are not in the DWARF):
//   line    33  XDecoder::XDecoder(const void * data, size_t sizeBytes)  [inlined into 4 functions]
//   line    34  XDecoder::~XDecoder()  [0x710097f960 (+1 more)]
//   line    40  const void * XDecoder::Data() const  [inlined into 1 functions]
//   line    42  size_t XDecoder::Size() const  [inlined into 2 functions]
//   line    43  size_t XDecoder::Used() const  [inlined into 2 functions]
//   line    45  size_t XDecoder::SizeBytes() const  [inlined into 1 functions]
//   line    46  size_t XDecoder::UsedBytes() const  [inlined into 1 functions]
