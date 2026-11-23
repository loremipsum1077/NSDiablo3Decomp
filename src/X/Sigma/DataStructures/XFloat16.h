#pragma once

#include "../SigmaTypes.h"

class XFloat16 {
public:
    XFloat16() {}
    XFloat16(float fVal) { *this = fVal; }
    XFloat16(uint16 uData) : m_uData(uData) {}

    XFloat16& operator=(float fVal);
    operator float() const;

    static const float GetAbsoluteMaximumValue();

    uint16 m_uData;

private:
    static const int32 m_nSignBit = 0x8000;
    static const int32 m_nOldExponentBits = 8;
    static const int32 m_nOldMantissaBits = 23;
    static const int32 m_nNewExponentBits = 5;
    static const int32 m_nNewMantissaBits = 10;
    static const float m_fAbsoluteMaximum;
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(XFloat16) == 0x2, "XFloat16");
static_assert(offsetof(XFloat16, m_uData) == 0x0, "XFloat16");
