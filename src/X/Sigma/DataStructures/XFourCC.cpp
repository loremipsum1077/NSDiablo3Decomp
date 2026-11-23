#include "XFourCC.h"
const XFourCC XFourCC::INVALID;
BOOL XFourCC::ToString(char* pszBuffer, size_t buffSize) const {
    if (buffSize < 5)
        return FALSE;

    LPCSTR cp = (LPCSTR)&m_value;
    int nDest = 0;
    for (int nSrc = 3; nSrc >= 0; nSrc--) {
        if (cp[nSrc] != 0)
            pszBuffer[nDest++] = cp[nSrc];
    }
    pszBuffer[nDest] = 0;
    return TRUE;
}

uint32 XFourCC::StringToValue(LPCSTR s) {
    uint32 value = 0;
    if (s != NULL) {
        while (*s != 0) {
            value = (value << 8) | (uint8)*s;
            s++;
        }
    }
    return value;
}
