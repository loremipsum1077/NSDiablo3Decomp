#pragma once

#include "../SigmaTypes.h"

#include "../Unicode.h"


class XFourCC {
public:
    XFourCC() : m_value(0) {}
    XFourCC(const XFourCC& f) : m_value(f.m_value) {}

    explicit XFourCC(LPCSTR s) : m_value(StringToValue(s)) {}

    XFourCC& operator=(const XFourCC& f) { m_value = f.m_value; return *this; }
    XFourCC& operator=(uint32 v) { m_value = v; return *this; }

    uint32 GetValue() const { return m_value; }

    BOOL ToString(char* pszBuffer, size_t buffSize) const;

    static const XFourCC INVALID;

private:
    static uint32 StringToValue(LPCSTR s);

    uint32 m_value;

    friend bool operator==(const XFourCC& a, const XFourCC& b);
};

inline bool operator==(const XFourCC& a, const XFourCC& b) { return a.m_value == b.m_value; }

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XFourCC) == 0x4, "XFourCC");
