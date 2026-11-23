#include "StringManip.h"

#include <string.h>

#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "Math/MathGeneral.h"
#include "Memory/SigmaMemory.h"

void strreplace(LPSTR szStr, LPCSTR szTokSet, CHAR cReplaceChar) {
    size_t nLen = strlen(szStr);
    size_t nTokSetLen = strlen(szTokSet);
    for (size_t i = 0; i < nLen; i++) {
        for (size_t j = 0; j < nTokSetLen; j++) {
            if (szStr[i] == szTokSet[j])
                szStr[i] = cReplaceChar;
        }
    }
}

BOOL strreplace(LPCSTR szStr, LPSTR szDest, uint32 nDestBufferLen, LPCSTR szOld, LPCSTR szNew) {
    if (szOld == NULL || szNew == NULL || szStr == NULL || nDestBufferLen == 0 || szDest == NULL)
        return FALSE;

    LPCSTR szCurrentPos = szStr;
    LPSTR szCurrentDestPos = szDest;
    size_t szNewLen = strlen(szNew);
    size_t szOldLen = strlen(szOld);
    size_t nStrLen = strlen(szStr);

    LPCSTR szNewPos = strstr(szCurrentPos, szOld);
    while (szNewPos != NULL) {
        size_t nLenToCopy = szNewPos - szCurrentPos;
        if ((size_t)(szCurrentDestPos - szDest) + nLenToCopy >= nDestBufferLen)
            return FALSE;
        if (nLenToCopy != 0)
            SigmaMemoryCopy(szCurrentDestPos, szCurrentPos, nLenToCopy);
        szCurrentDestPos += nLenToCopy;
        if ((size_t)(szCurrentDestPos - szDest) + szNewLen >= nDestBufferLen)
            return FALSE;
        if (szNewLen != 0)
            SigmaMemoryCopy(szCurrentDestPos, szNew, szNewLen);
        szCurrentDestPos += szNewLen;
        szCurrentPos = szNewPos + szOldLen;
        szNewPos = strstr(szCurrentPos, szOld);
    }

    if (szCurrentPos != szStr + nStrLen) {
        size_t nLenToCopy = (szStr + nStrLen) - szCurrentPos;
        if ((size_t)(szCurrentDestPos - szDest) + nLenToCopy >= nDestBufferLen)
            return FALSE;
        SigmaMemoryCopy(szCurrentDestPos, szCurrentPos, nLenToCopy);
        szCurrentDestPos += nLenToCopy;
    }

    if (szCurrentDestPos >= szDest + nDestBufferLen)
        return FALSE;
    *szCurrentDestPos = '\0';
    return TRUE;
}

void strreplace_safe(LPSTR szStr, uint32 nBufferLen, CHAR cOld, CHAR cNew) {
    if (szStr == NULL)
        return;
    for (uint32 i = 0; i < nBufferLen; i++) {
        if (szStr[i] == '\0')
            break;
        if (szStr[i] == cOld)
            szStr[i] = cNew;
    }
}

void strcat_safe(LPSTR szDest, const size_t nSizeOfDest, LPCSTR szSrc) {
    size_t nCurrentDestLen = strlen(szDest);
    if (nCurrentDestLen + 1 >= nSizeOfDest)
        return;
    int32 nSpaceRemaining = nSizeOfDest - nCurrentDestLen - 1;
    int32 nAmountToCopy = Math::Min<int32>(strlen(szSrc), nSpaceRemaining);
    Blizzard::String::Copy(szDest + nCurrentDestLen, szSrc, nAmountToCopy + 1);
}

void strncat_safe(LPSTR szDest, const size_t nSizeOfDest, LPCSTR szSrc, const size_t nSizeToCopy) {
    size_t nCurrentDestLen = strlen(szDest);
    if (nCurrentDestLen + 1 >= nSizeOfDest)
        return;
    int32 nSpaceRemaining = nSizeOfDest - nCurrentDestLen - 1;
    int32 nAmountToCopy = Math::Min<int32>(nSizeToCopy, nSpaceRemaining);
    Blizzard::String::Copy(szDest + nCurrentDestLen, szSrc, nAmountToCopy + 1);
}

StringTokenizer::StringTokenizer(const char* szString, const char* szDelimiters)
    : m_szString(szString), m_szDelimiters(szDelimiters) {}

static BOOL sCharMatchesDelimiter(char c, LPCSTR pszDelimiters) {
    for (const char* pDelim = pszDelimiters; *pDelim != '\0'; pDelim++) {
        if (*pDelim == c)
            return TRUE;
    }
    return FALSE;
}

BOOL StringTokenizer::GetNextToken(char* szToken, uint32 nMaxTokenLen) {
    if (*m_szString == '\0')
        return FALSE;

    uint32 nCounter = 0;
    while (*m_szString != '\0') {
        if (sCharMatchesDelimiter(*m_szString, m_szDelimiters)) {
            m_szString++;
            if (nCounter != 0)
                break;
            continue;
        }
        if (nCounter < nMaxTokenLen - 1) {
            *szToken++ = *m_szString;
            nCounter++;
        }
        m_szString++;
    }
    *szToken = '\0';
    return nCounter != 0;
}
