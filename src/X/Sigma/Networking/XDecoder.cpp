#include "XDecoder.h"

#include "../Math/MathGeneral.h"
#include "XEncodeDecodeUtil.h"











void XDecoder::SetData(const void * d, size_t sizeBytes) {
    m_data = (const uint8 *)d;
    m_size = sizeBytes << 3;
    m_used = 0;
}


void XDecoder::SetUsed(size_t u) {
    m_used = u;
}






XDecoder::Result XDecoder::DataOverflow() {
    return RESULT_TRUNCATED;
}



inline XDecoder::Result XDecoder::Align8() {
    size_t o = ::Align8(m_used) - m_used;
    m_used += o;
    return RESULT_VALID;
}



XDecoder::Result XDecoder::Get(uint32 & x, size_t bits) {
    x = 0;
    while (bits) {
        if (m_used >= m_size) {
            Result r = DataOverflow();
            if (r != RESULT_VALID)
                return r;
        }
        size_t p = m_used & 7;



        size_t b = Math::Min(8 - p, bits);
        uint8 s = m_data[m_used >> 3];
        x |= ((s >> p) & ((1 << b) - 1)) << (bits - b);
        bits -= b;
        m_used += b;
    }
    return RESULT_VALID;
}



inline XDecoder::Result XDecoder::GetAligned8(uint32 & x, size_t bits) {
    x = 0;
    while (bits) {
        if (m_used >= m_size) {
            Result r = DataOverflow();
            if (r != RESULT_VALID)
                return r;
        }
        bits -= 8;
        x = (x << 8) | m_data[m_used >> 3];
        m_used += 8;
    }
    return RESULT_VALID;
}








XDecoder::Result XDecoder::GetU64(uint64 & v, size_t bits) {
    v = 0;
    size_t i = bits;
    while (i) {
        uint32 xx;
        size_t tempbits = Math::Min(i, (size_t)32);

        Result r = Get(xx, tempbits);
        if (r != RESULT_VALID)
            return r;
        v = (v << tempbits) | xx;
        i -= tempbits;
    }
    return RESULT_VALID;
}

































XDecoder::Result XDecoder::GetBytesAligned8(uint8 * data, size_t size) {
    {
        Result r = Align8();
        if (r != RESULT_VALID)
            return r;
    }
    size *= 8;
    while (size >= 32) {
        uint32 x;
        Result r = GetAligned8(x, 32);
        if (r != RESULT_VALID)
            return r;
        data[0] = (uint8)(x >> 24);
        data[1] = (uint8)(x >> 16);
        data[2] = (uint8)(x >> 8);
        data[3] = (uint8)x;
        data += 4;
        size -= 32;
    }
    if (size) {
        uint32 x;
        Result r = GetAligned8(x, size);
        if (r != RESULT_VALID)
            return r;
        while (size) {
            size -= 8;
            *data++ = (uint8)(x >> size);
        }
    }
    return RESULT_VALID;
}
XDecoder::Result XDecoder::GetAsciiStringBytesAligned8(uint8 * data, size_t length) {
    {
        Result r = Align8();
        if (r != RESULT_VALID)
            return r;
    }
    while (length > 4) {
        uint32 x;
        Result r = GetAligned8(x, 32);
        if (r != RESULT_VALID)
            return r;
        if (!(uint8)(x >> 24))
            return RESULT_CORRUPTED;
        data[0] = (uint8)(x >> 24);
        if (!(uint8)(x >> 16))
            return RESULT_CORRUPTED;
        data[1] = (uint8)(x >> 16);
        if (!(uint8)(x >> 8))
            return RESULT_CORRUPTED;
        data[2] = (uint8)(x >> 8);
        if (!(uint8)x)
            return RESULT_CORRUPTED;
        data[3] = (uint8)x;
        data += 4;
        length -= 4;
    }
    while (length) {
        uint32 x;






        Result r = GetAligned8(x, 8);
        if (r != RESULT_VALID)
            return r;
        if (!x)
            return RESULT_CORRUPTED;
        *data++ = (uint8)x;
        length--;
    }
    *data = 0;
    return RESULT_VALID;
}






























































XDecoder::Result XDecoder::GetBool(BOOL & b) {
    uint32 x;
    {
        Result r = Get(x, 1);
        if (r != RESULT_VALID)
            return r;
    }
    b = x ? TRUE : FALSE;
    return RESULT_VALID;
}
