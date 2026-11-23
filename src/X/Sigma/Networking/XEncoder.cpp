#include "XEncoder.h"

#include "../ErrorManager/ErrorManager.h"
#include "../Math/MathGeneral.h"
#include "XEncodeDecodeUtil.h"





























void XEncoder::ZeroUnusedByte() {
    size_t tused = m_used;
    Align8();
    m_used = tused;
}



void XEncoder::SetData(void * d, size_t sizeBytes) {
    m_data = (uint8 *)d;
    m_size = sizeBytes << 3;
    m_used = 0;
}


void XEncoder::SetUsed(size_t u) {
    m_used = u;
}




void XEncoder::DataOverflow() {
    // NOTE: the original's assertion macro is not in the DWARF; this is its expansion.
    DisplayInternalError(NULL, "", 59, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

inline void XEncoder::Align8() {

    size_t o = ::Align8(m_used) - m_used;
    Put(0, o);
}


void XEncoder::Put(uint32 x, size_t bits) {
    while (bits) {
        if (m_used >= m_size)
            DataOverflow();






        size_t p = m_used & 7;
        size_t b = Math::Min(8 - p, bits);
        uint8 m = (uint8)((1 << b) - 1);
        bits -= b;
        uint8 s = (uint8)((x >> bits) & m);
        uint8 & d = m_data[m_used >> 3];
        d = (uint8)((d & ~(m << p)) | (s << p));
        m_used += b;
    }
}




inline void XEncoder::PutAligned8(uint32 x, size_t bits) {
    while (bits) {
        bits -= 8;
        if (m_used >= m_size)
            DataOverflow();
        m_data[m_used >> 3] = (uint8)(x >> bits);
        m_used += 8;
    }
}











void XEncoder::PutU64(uint64 v, size_t size) {
    size_t i = size;
    while (i) {
        size_t bits = Math::Min(i, (size_t)32);
        i -= bits;
        Put((uint32)(v >> i), bits);
    }
}


































void XEncoder::PutBytesAligned8(const uint8 * data, size_t size) {

    Align8();
    size_t bits = size * 8;
    while (bits >= 32) {
        uint32 x = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        data += 4;
        bits -= 32;



        PutAligned8(x, 32);
    }
    if (bits) {
        size_t xsize = bits;
        uint32 x = 0;
        while (bits) {
            x = (x << 8) | *data;
            data++;
            bits -= 8;
        }


        PutAligned8(x, xsize);
    }
}



void XEncoder::PutAsciiStringBytesAligned8(const uint8 * data, size_t length) {

    Align8();
    while (length > 4) {
        uint32 x = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        data += 4;
        length -= 4;









        PutAligned8(x, 32);
    }
    if (length) {
        uint32 x = 0;
        size_t xbits = length * 8;
        while (length) {
            x = (x << 8) | *data;
            data++;
            length--;
        }








        PutAligned8(x, xbits);
    }
}

















































void XEncoder::PutBool(BOOL b) {

    Put(b ? 1 : 0, 1);
}


void XEncoder::Put(uint32 u) {

    Put(u, 32);
}
