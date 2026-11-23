#pragma once

#include "SigmaTypes.h"

class CCookie {
public:
    void Set();
    void Clear();
    void Check() const;
    void CheckClear() const;
    BOOL IsCleared() const;

    static const int32 COOKIE_VALUE = 0x600DF00D;
    static const int32 CLEARED_VALUE = (int32)0xFEEDFACE;

    int32 m_nCookie;
};

inline void CCookie::Set() {
    m_nCookie = COOKIE_VALUE;
}

inline void CCookie::Clear() {
    m_nCookie = CLEARED_VALUE;
}

inline BOOL CCookie::IsCleared() const {
    return m_nCookie == CLEARED_VALUE || m_nCookie == 0;
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(CCookie) == 0x4, "CCookie");
static_assert(offsetof(CCookie, m_nCookie) == 0x0, "CCookie");
