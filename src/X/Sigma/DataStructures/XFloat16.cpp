#include "XFloat16.h"

const float XFloat16::m_fAbsoluteMaximum = 65536.0f;

XFloat16& XFloat16::operator=(float fVal) {
    uint32* pF = (uint32*)&fVal;
    int32 e = ((*pF >> m_nOldMantissaBits) & 0xff) - 112;
    int32 m = (*pF >> (m_nOldMantissaBits - m_nNewMantissaBits)) & 0x3ff;
    e &= ~(e >> 31);
    m_uData = ((*pF >> 16) & m_nSignBit) | (e << m_nNewMantissaBits) | m;
    return *this;
}

XFloat16::operator float() const {
    float fVal;
    uint32* pF = (uint32*)&fVal;
    if (m_uData == 0)
        return 0.0f;
    int32 e = (m_uData >> m_nNewMantissaBits) & 0x1f;
    int32 m = m_uData & 0x3ff;
    *pF = ((m_uData & m_nSignBit) << 16) | ((e + 112) << m_nOldMantissaBits) |
          (m << (m_nOldMantissaBits - m_nNewMantissaBits));
    return fVal;
}

const float XFloat16::GetAbsoluteMaximumValue() {
    return m_fAbsoluteMaximum;
}
