#pragma once

#include "SigmaTypes.h"
#include "Unicode.h"

void strreplace(LPSTR szStr, LPCSTR szTokSet, CHAR cReplaceChar);
BOOL strreplace(LPCSTR szStr, LPSTR szDest, uint32 nDestBufferLen, LPCSTR szOld, LPCSTR szNew);
void strreplace_safe(LPSTR szStr, uint32 nBufferLen, CHAR cOld, CHAR cNew);
void strcat_safe(LPSTR szDest, const size_t nSizeOfDest, LPCSTR szSrc);
void strncat_safe(LPSTR szDest, const size_t nSizeOfDest, LPCSTR szSrc, const size_t nSizeToCopy);

class StringTokenizer {
public:
    StringTokenizer(const char* szString, const char* szDelimiters);
    BOOL GetNextToken(char* szToken, uint32 nMaxTokenLen);

private:
    const char* m_szString;
    const char* m_szDelimiters;
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(StringTokenizer) == 0x10, "StringTokenizer");
