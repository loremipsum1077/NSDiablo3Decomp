#include "UnicodeText.h"

#include <string.h>

#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "DataStructures/XFourCC.h"
#include "DataStructures/XMap.h"
#include "Math/MathGeneral.h"
#include "StringManip.h"

// Contrib/Contrib/blz/include/blz/tuple.h. Nearly every original compile unit sees this
// header-level static. Nothing uses it, but because it is const and has a user-provided
// constructor it is initialised in _GLOBAL__sub_I_* and kept alive (llvm.invariant.start), and
// GlobalMerge then puts it in front of this file's own statics, which shifts their offsets.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"

struct LocaleDef {
    const char* pszName;
    XFourCC tFourCC;
    BOOL bShouldLanguageFilter;
};

static const LocaleDef stLocaleDefs[NUM_XLOCALES] = {
    {"Invalid", XFourCC(), FALSE},
    {"Global", XFourCC("Glbl"), FALSE},
    {"enUS", XFourCC("enUS"), TRUE},
    {"enGB", XFourCC("enGB"), TRUE},
    {"enSG", XFourCC("enSG"), TRUE},
    {"esES", XFourCC("esES"), TRUE},
    {"esMX", XFourCC("esMX"), TRUE},
    {"frFR", XFourCC("frFR"), TRUE},
    {"itIT", XFourCC("itIT"), TRUE},
    {"deDE", XFourCC("deDE"), TRUE},
    {"koKR", XFourCC("koKR"), TRUE},
    {"ptBR", XFourCC("ptBR"), TRUE},
    {"ruRU", XFourCC("ruRU"), TRUE},
    {"zhCN", XFourCC("zhCN"), TRUE},
    {"zhTW", XFourCC("zhTW"), TRUE},
    {"trTR", XFourCC("trTR"), FALSE},
    {"plPL", XFourCC("plPL"), TRUE},
    {"ptPT", XFourCC("ptPT"), FALSE},
    {"jpJP", XFourCC("jpJP"), FALSE},
};

char output8[2048];

struct UnicodeTextGlobals {
    UnicodeTextGlobals();
    XLocale m_eCurrentXLocale;
    XLocale m_eStringListOverrideLocale;
};

static UnicodeTextGlobals sgtUTextGlobals;

inline UnicodeTextGlobals::UnicodeTextGlobals()
    : m_eCurrentXLocale(XLOCALE_INVALID), m_eStringListOverrideLocale(XLOCALE_INVALID) {}

XLocale XLocaleGetBaseLocale(const XLocale eLocale) {
    switch (eLocale) {
    case XLOCALE_ENGLISH_EU:
    case XLOCALE_ENGLISH_SOUTHEAST_ASIA:
        return XLOCALE_ENGLISH_US;
    case XLOCALE_PORTUGUESE_PORTUGAL:
        return XLOCALE_PORTUGUESE_BRAZIL;
    default:
        return eLocale;
    }
}

LPCSTR XLocaleToString(XLocale eLocale) {
    return stLocaleDefs[eLocale].pszName;
}

XLocale XLocaleFromString(LPCSTR pszXLocale) {
    for (int32 i = 0; i < NUM_XLOCALES; i++) {
        if (Blizzard::String::CompareI(pszXLocale, stLocaleDefs[i].pszName) == 0) {
            return i;
        }
    }
    return NUM_XLOCALES;
}

void UnicodeText::SetStringDisplayLocale(const XLocale eXLocale) {
    sgtUTextGlobals.m_eStringListOverrideLocale = eXLocale;
}

XLocale UnicodeText::GetStringDisplayLocale() {
    if (sgtUTextGlobals.m_eStringListOverrideLocale != XLOCALE_INVALID) {
        return sgtUTextGlobals.m_eStringListOverrideLocale;
    }
    return GetCurrentLocale();
}

XLocale UnicodeText::GetCurrentLocale() {
    return sgtUTextGlobals.m_eCurrentXLocale;
}

namespace UnicodeText {
struct U16CasePair {
    uint16 uUpper;
    uint16 uLower;
};

struct LetterPair {
    uint32 m_nLowerCodePoint;
    uint32 m_nUpperCodePoint;
};

LPSTR sConcatCodepoint(LPSTR szReturnEnd, size_t nBufferLen, uint32 nChar);
BOOL sToUpperIsMultipleChars(uint32 nChar);
void sToUpperMultipleChars(uint32 nChar, uint32* ptConv);
}  // namespace UnicodeText






CRefString UnicodeText::Capitalize(LPCSTR szUTF8Str) {
    LPCSTR pszCurr = szUTF8Str;
    uint32 nCodepoint = Blizzard::Unicode::GetCodepointFromUTF8(&pszCurr);

    if (nCodepoint == 0) {
        return CRefString("");
    }

    ToUpper(nCodepoint);

    CRefString sOutput;
    sOutput += Blizzard::Unicode::GetUTF8FromCodepoint(nCodepoint);
    sOutput += pszCurr;
    return sOutput;
}


CRefString UnicodeText::Uncapitalize(LPCSTR szUTF8Str) {
    LPCSTR pszCurr = szUTF8Str;
    uint32 nCodepoint = Blizzard::Unicode::GetCodepointFromUTF8(&pszCurr);

    if (nCodepoint == 0) {
        return CRefString("");
    }

    ToLower(nCodepoint);

    CRefString sOutput;
    sOutput += Blizzard::Unicode::GetUTF8FromCodepoint(nCodepoint);
    sOutput += pszCurr;
    return sOutput;
}

namespace UnicodeText {
static U16CasePair s_u16CaseConversionTable[] = {
    {0x00C9, 0x00E9}, {0x00C8, 0x00E8}, {0x00C7, 0x00E7}, {0x00C0, 0x00E0},
    {0x00C6, 0x00E6}, {0x00C2, 0x00E2}, {0x00D4, 0x00F4}, {0x0152, 0x0153},
    {0x00D6, 0x00F6}, {0x00CA, 0x00EA}, {0x00DC, 0x00FC}, {0x00C0, 0x00E0},
    {0x00C8, 0x00E8}, {0x00C9, 0x00E9}, {0x00CC, 0x00EC}, {0x00D2, 0x00F2},
    {0x00D9, 0x00F9}, {0x00C7, 0x00E7}, {0x00C4, 0x00E4}, {0x00D6, 0x00F6},
    {0x00DC, 0x00FC}, {0x00C1, 0x00E1}, {0x00C9, 0x00E9}, {0x00CD, 0x00ED},
    {0x00D3, 0x00F3}, {0x00DA, 0x00FA}, {0x00DC, 0x00FC}, {0x00D1, 0x00F1},
    {0x0410, 0x0430}, {0x0411, 0x0431}, {0x0412, 0x0432}, {0x0413, 0x0433},
    {0x0414, 0x0434}, {0x0415, 0x0435}, {0x0401, 0x0451}, {0x0416, 0x0436},
    {0x0417, 0x0437}, {0x0418, 0x0438}, {0x0419, 0x0439}, {0x041A, 0x043A},
    {0x041B, 0x043B}, {0x041C, 0x043C}, {0x041D, 0x043D}, {0x041E, 0x043E},
    {0x041F, 0x043F}, {0x0420, 0x0440}, {0x0421, 0x0441}, {0x0422, 0x0442},
    {0x0423, 0x0443}, {0x0424, 0x0444}, {0x0425, 0x0445}, {0x0426, 0x0446},
    {0x0427, 0x0447}, {0x0428, 0x0448}, {0x0429, 0x0449}, {0x042A, 0x044A},
    {0x042B, 0x044B}, {0x042C, 0x044C}, {0x042D, 0x044D}, {0x042E, 0x044E},
    {0x042F, 0x044F}, {0x0104, 0x0105}, {0x0106, 0x0107}, {0x0118, 0x0119},
    {0x0141, 0x0142}, {0x0143, 0x0144}, {0x00D3, 0x00F3}, {0x015A, 0x015B},
    {0x0179, 0x017A}, {0x017B, 0x017C}, {0x00C1, 0x00E1}, {0x00C9, 0x00E9},
    {0x00CD, 0x00ED}, {0x00D3, 0x00F3}, {0x00DA, 0x00FA}, {0x0147, 0x0148},
    {0x00DC, 0x00FC}, {0x00C0, 0x00E0}, {0x00C1, 0x00E1}, {0x00C2, 0x00E2},
    {0x00C3, 0x00E3}, {0x00C9, 0x00E9}, {0x00CA, 0x00EA}, {0x00CD, 0x00ED},
    {0x00D3, 0x00F3}, {0x00D4, 0x00F4}, {0x00D5, 0x00F5}, {0x00DA, 0x00FA},
    {0x00DC, 0x00FC}, {0x00C7, 0x00E7},
};

static XMap<uint32, uint32, uint32, uint32, 256> s_tToUpperCaseConversionTable;
static XMap<uint32, uint32, uint32, uint32, 256> s_tToLowerCaseConversionTable;
}  // namespace UnicodeText


BOOL UnicodeText::Initialize() {
    for (uint32 i = 0; i < sizeof(s_u16CaseConversionTable) / sizeof(s_u16CaseConversionTable[0]); i++) {
        LetterPair tPair;
        tPair.m_nLowerCodePoint = s_u16CaseConversionTable[i].uLower;
        tPair.m_nUpperCodePoint = s_u16CaseConversionTable[i].uUpper;

        uint32 uUpper;
        if (!s_tToUpperCaseConversionTable.Lookup(tPair.m_nLowerCodePoint, uUpper)) {
            s_tToUpperCaseConversionTable.SetAt_Tracked(tPair.m_nLowerCodePoint, tPair.m_nUpperCodePoint);
        }

        uint32 uLower;
        if (!s_tToLowerCaseConversionTable.Lookup(tPair.m_nUpperCodePoint, uLower)) {
            s_tToLowerCaseConversionTable.SetAt_Tracked(tPair.m_nUpperCodePoint, tPair.m_nLowerCodePoint);
        }
    }

    return TRUE;
}

void UnicodeText::Dispose() {}

void UnicodeText::SetLocale(const XLocale eXLocale) {
    sgtUTextGlobals.m_eCurrentXLocale = eXLocale;
}

inline LPSTR UnicodeText::sConcatCodepoint(LPSTR szReturnEnd, size_t nBufferLen, uint32 nChar) {
    if (nBufferLen == 0) {
        return szReturnEnd;
    }

    char szUTF8[8];
    Blizzard::String::Copy(szUTF8, Blizzard::Unicode::GetUTF8FromCodepoint(nChar), sizeof(szUTF8));

    size_t nLen = strlen(szUTF8);
    if (nLen <= nBufferLen - 1) {
        strcpy(szReturnEnd, szUTF8);
        szReturnEnd += nLen;
    }

    return szReturnEnd;
}

inline BOOL UnicodeText::sToUpperIsMultipleChars(uint32 nChar) {
    return nChar == 0xDF;
}

inline void UnicodeText::sToUpperMultipleChars(uint32 nChar, uint32* ptConv) {
    (void)nChar;
    ptConv[0] = 'S';
    ptConv[1] = 'S';
}

inline void UnicodeText::ToUpper(uint32& uChar) {
    if (uChar >= 'a' && uChar <= 'z') {
        uChar -= 'a' - 'A';
        return;
    }

    uint32 uUpper;
    if (s_tToUpperCaseConversionTable.Lookup(uChar, uUpper)) {
        uChar = uUpper;
    }
}


void UnicodeText::ToUpper(LPSTR szUTF8Str, const size_t nLength) {
    size_t nNumChars;
    char szReturn[4096];
    const char* szString;
    char* szReturnEnd;
    BOOL bRes;

    if (szUTF8Str == NULL || nLength == 1) {
        return;
    }

    nNumChars = 0;
    bRes = Blizzard::Unicode::CountUTF8Chars(szUTF8Str, nLength, nNumChars);
    if (!bRes) {
        return;
    }

    szReturn[0] = '\0';
    szString = szUTF8Str;
    szReturnEnd = szReturn;

    for (size_t i = 0; i < nNumChars; i++) {
        uint32 nChar = Blizzard::Unicode::GetCodepointFromUTF8(&szString);

        if (!sToUpperIsMultipleChars(nChar)) {
            ToUpper(nChar);
            szReturnEnd = sConcatCodepoint(szReturnEnd, sizeof(szReturn) - (szReturnEnd - szReturn), nChar);
        } else {
            uint32 uConv[2];
            sToUpperMultipleChars(nChar, uConv);
            szReturnEnd = sConcatCodepoint(szReturnEnd, sizeof(szReturn) - (szReturnEnd - szReturn), uConv[0]);
            szReturnEnd = sConcatCodepoint(szReturnEnd, sizeof(szReturn) - (szReturnEnd - szReturn), uConv[1]);
        }
    }

    Blizzard::String::Copy(szUTF8Str, szReturn, nLength);
}

inline void UnicodeText::ToLower(uint32& uChar) {
    if (uChar >= 'A' && uChar <= 'Z') {
        uChar += 'a' - 'A';
        return;
    }

    uint32 uLower;
    if (s_tToLowerCaseConversionTable.Lookup(uChar, uLower)) {
        uChar = uLower;
    }
}

// NON_MATCHING: size 504 != 500. Everything outside the first copy loop is identical. The original
// enters that loop testing only the character and nDecimals - 3 (in that order), tests nDst, the
// character and nSrc together at the bottom, and does not know nDst < 63 inside the body; ours
// tests nDecimals - 3 before the character. About thirty forms of the loop were tried (all orders
// of the three conditions as while, for, break-at-top, break-at-bottom and guarded do-while).
CRefString UnicodeText::sProcessGrouping(LPCSTR sNumber, uint32 nNumberLen, XLocale eXLocale) {
    LPCTSTR sThousandsSeparator = GetThousandSeparator(eXLocale);

    if (sThousandsSeparator != NULL) {
        LPCSTR sAbsValNumber = sNumber;
        BOOL bIsNegative = sNumber[0] == '-';
        if (bIsNegative) {
            sAbsValNumber++;
        }

        const CHAR* decimalCharPos = Blizzard::String::Find(sAbsValNumber, GetDecimalCharacter(eXLocale), nNumberLen);
        uint32 nSrcLen = strlen(sAbsValNumber);
        size_t nDecimals = decimalCharPos != NULL ? decimalCharPos - sAbsValNumber : nSrcLen;

        if (nDecimals > 4) {
            CHAR sProcessedNumber[64];
            uint32 nDst = 0;
            uint32 nSrc = 0;

            if (bIsNegative) {
                sProcessedNumber[nDst++] = '-';
            }

            while (nDst < 63 && sAbsValNumber[nSrc] != '\0' && nSrc < nDecimals - 3) {
                sProcessedNumber[nDst++] = sAbsValNumber[nSrc];
                if (nDst < 63 && (nDecimals - nSrc) % 3 == 1) {
                    sProcessedNumber[nDst++] = sThousandsSeparator[0];
                }
                nSrc++;
            }

            while (nDst < 63 && nSrc < nSrcLen && sAbsValNumber[nSrc] != '\0') {
                sProcessedNumber[nDst++] = sAbsValNumber[nSrc++];
            }

            sProcessedNumber[nDst] = '\0';
            return CRefString(sProcessedNumber);
        }
    }

    return CRefString(sNumber);
}

CRefString UnicodeText::FormatNumber(const int32 nNumber, const BOOL fGroupDigits, const FormatNumberStyle eStyle,
                                     XLocale eXLocale) {
    CHAR sNumber[64];
    CRefString usResult;

    if (eXLocale == XLOCALE_INVALID) {
        eXLocale = GetStringDisplayLocale();
    }

    sNumber[0] = '\0';
    switch (eStyle) {
    case FNS_NORMAL:
    case FNS_PERCENT:
        Blizzard::String::Format(sNumber, 64, "%d", nNumber);
        break;
    case FNS_CURRENCY:
        Blizzard::String::Format(sNumber, 64, "$%d", nNumber);
        break;
    default:
        break;
    }

    if (fGroupDigits) {
        usResult = sProcessGrouping(sNumber, 64, eXLocale);
    } else {
        usResult = sNumber;
    }

    if (eStyle == FNS_PERCENT) {
        usResult += "%";
    }

    return usResult;
}

CRefString UnicodeText::FormatNumber(const int64 nNumber, const BOOL fGroupDigits, const FormatNumberStyle eStyle,
                                     XLocale eXLocale) {
    CHAR sNumber[64];
    CRefString usResult;

    if (eXLocale == XLOCALE_INVALID) {
        eXLocale = GetStringDisplayLocale();
    }

    sNumber[0] = '\0';
    switch (eStyle) {
    case FNS_NORMAL:
    case FNS_PERCENT:
        Blizzard::String::Format(sNumber, 64, "%lld", nNumber);
        break;
    case FNS_CURRENCY:
        Blizzard::String::Format(sNumber, 64, "$%lld", nNumber);
        break;
    default:
        break;
    }

    if (fGroupDigits) {
        usResult = sProcessGrouping(sNumber, 64, eXLocale);
    } else {
        usResult = sNumber;
    }

    if (eStyle == FNS_PERCENT) {
        usResult += "%";
    }

    return usResult;
}

CRefString UnicodeText::FormatNumber(const uint64 nNumber, BOOL fGroupDigits, const FormatNumberStyle eStyle,
                                     XLocale eXLocale) {
    return FormatNumber((int64)nNumber, fGroupDigits, eStyle, eXLocale);
}

CRefString UnicodeText::FormatNumber(const float flNumber, const BOOL fGroupDigits, const FormatNumberStyle eStyle,
                                     const int32 nMinFractionalDigits, const int32 nMaxFractionalDigits,
                                     XLocale eXLocale) {
    CHAR sNumber[64];
    CRefString usResult;

    if (eXLocale == XLOCALE_INVALID) {
        eXLocale = GetStringDisplayLocale();
    }

    float flTempNumber = flNumber;
    int32 nFractionalDigits = 0;
    while (nFractionalDigits < nMaxFractionalDigits && flTempNumber - (float)(int32)flTempNumber > 0.000001f) {
        flTempNumber *= 10.0f;
        nFractionalDigits++;
    }
    nFractionalDigits = Math::Pin(nFractionalDigits, nMinFractionalDigits, nMaxFractionalDigits);

    sNumber[0] = '\0';
    switch (eStyle) {
    case FNS_NORMAL: {
        CRefString sFormat;
        Blizzard::String::Format(sNumber, 64, sFormat.Format("%%.%df", nFractionalDigits), flNumber);
        break;
    }
    case FNS_CURRENCY:
        Blizzard::String::Format(sNumber, 64, "$%.2f", flNumber);
        break;
    case FNS_PERCENT: {
        CRefString sFormat;
        Blizzard::String::Format(sNumber, 64, sFormat.Format("%%.%df", nFractionalDigits), flNumber * 100.0f);
        break;
    }
    default:
        break;
    }

    CHAR cDecimalCharacter = GetDecimalCharacter(eXLocale);
    if (cDecimalCharacter != '.') {
        strreplace_safe(sNumber, 64, '.', cDecimalCharacter);
    }

    if (fGroupDigits) {
        usResult = sProcessGrouping(sNumber, 64, eXLocale);
    } else {
        usResult = sNumber;
    }

    if (eStyle == FNS_PERCENT) {
        usResult += "%";
    }

    return usResult;
}

CRefString UnicodeText::FormatNumber(LPCSTR szNumber, const FormatNumberStyle eStyle, const int32 nMinFractionalDigits,
                                     const int32 nMaxFractionalDigits, XLocale eXLocale) {
    (void)szNumber;
    (void)eStyle;
    (void)nMinFractionalDigits;
    (void)nMaxFractionalDigits;
    (void)eXLocale;
    return CRefString();
}

CRefString UnicodeText::FormatDate(Blizzard::Time::Timestamp tDate, const FormatDateStyle eDateStyle, XLocale eXLocale) {
    return FormatDateTime(tDate, eDateStyle, FTS_NONE, eXLocale);
}

CRefString UnicodeText::sFormatShortDate(uint32 uDay, uint32 uMonth, uint32 uYear, XLocale eXLocale) {
    CHAR sDate[64];
    sDate[0] = '\0';

    LPCSTR ptSeperator = GetDateSeparator(eXLocale);

    switch (eXLocale) {
    case XLOCALE_ENGLISH_EU:
    case XLOCALE_ENGLISH_SOUTHEAST_ASIA:
    case XLOCALE_SPANISH_EU:
    case XLOCALE_SPANISH_LATAM:
    case XLOCALE_FRENCH:
    case XLOCALE_ITALIAN:
    case XLOCALE_GERMAN:
    case XLOCALE_PORTUGUESE_BRAZIL:
    case XLOCALE_RUSSIAN:
    case XLOCALE_TURKISH:
    case XLOCALE_POLISH:
        Blizzard::String::Format(sDate, 64, "%u%c%u%c%u", uDay, *ptSeperator, uMonth, *ptSeperator, uYear);
        break;
    case XLOCALE_ENGLISH_US:
    case XLOCALE_PORTUGUESE_PORTUGAL:
    case XLOCALE_JAPANESE:
        Blizzard::String::Format(sDate, 64, "%u%c%u%c%u", uMonth, *ptSeperator, uDay, *ptSeperator, uYear);
        break;
    case XLOCALE_KOREAN:
        Blizzard::String::Format(sDate, 64, "%u%c%u%c%u", uYear, *ptSeperator, uMonth, *ptSeperator, uDay);
        break;
    default:
        break;
    }

    return CRefString(sDate);
}

inline CRefString UnicodeText::sFormatTime(uint32 uHour, uint32 uMinute, XLocale eXLocale) {
    CHAR sTime[64];
    sTime[0] = '\0';

    switch (eXLocale) {
    case XLOCALE_ENGLISH_EU:
    case XLOCALE_ENGLISH_SOUTHEAST_ASIA:
    case XLOCALE_SPANISH_EU:
    case XLOCALE_FRENCH:
    case XLOCALE_ITALIAN:
    case XLOCALE_GERMAN:
    case XLOCALE_PORTUGUESE_BRAZIL:
    case XLOCALE_RUSSIAN:
    case XLOCALE_TURKISH:
    case XLOCALE_POLISH:
    case XLOCALE_PORTUGUESE_PORTUGAL:
        Blizzard::String::Format(sTime, 64, "%u:%u", uHour, uMinute);
        break;
    case XLOCALE_ENGLISH_US:
    case XLOCALE_SPANISH_LATAM:
    case XLOCALE_KOREAN:
    case XLOCALE_JAPANESE:
        Blizzard::String::Format(sTime, 64, "%u:%u %s", uHour <= 12 ? uHour : uHour - 12, uMinute,
                                 uHour <= 12 ? "AM" : "PM");
        break;
    default:
        break;
    }

    return CRefString(sTime);
}

CRefString UnicodeText::FormatDateTime(Blizzard::Time::Timestamp tDate, const FormatDateStyle eDateStyle,
                                       const FormatTimeStyle eTimeStyle, XLocale eXLocale) {
    CRefString usResult;

    if (eXLocale == XLOCALE_INVALID) {
        eXLocale = GetStringDisplayLocale();
    }

    Blizzard::Time::TimeRec tRec;
    Blizzard::Time::BreakTime(tDate, tRec);

    switch (eDateStyle) {
    case FDS_SHORT:
    case FDS_MEDIUM:
    case FDS_LONG:
    case FDS_FULL:
        usResult = sFormatShortDate(tRec.day, tRec.month, tRec.year, eXLocale);
        break;
    default:
        break;
    }

    if (eTimeStyle != FTS_NONE) {
        usResult += " ";
        usResult += sFormatTime(tRec.hour, tRec.minute, eXLocale);
    }

    return usResult;
}

inline LPCSTR UnicodeText::GetThousandSeparator(XLocale eXLocale) {
    switch (eXLocale) {
    case XLOCALE_FRENCH:
    case XLOCALE_RUSSIAN:
    case XLOCALE_POLISH:
        return " ";
    case XLOCALE_ENGLISH_US:
    case XLOCALE_ENGLISH_EU:
    case XLOCALE_ENGLISH_SOUTHEAST_ASIA:
    case XLOCALE_SPANISH_LATAM:
    case XLOCALE_KOREAN:
    case XLOCALE_JAPANESE:
        return ",";
    case XLOCALE_SPANISH_EU:
    case XLOCALE_ITALIAN:
    case XLOCALE_GERMAN:
    case XLOCALE_PORTUGUESE_BRAZIL:
    case XLOCALE_TURKISH:
    case XLOCALE_PORTUGUESE_PORTUGAL:
        return ".";
    default:
        return NULL;
    }
}

CHAR UnicodeText::GetDecimalCharacter(XLocale eXLocale) {
    CHAR cDecimalCharacter = '.';

    switch (eXLocale) {
    case XLOCALE_SPANISH_EU:
    case XLOCALE_FRENCH:
    case XLOCALE_ITALIAN:
    case XLOCALE_GERMAN:
    case XLOCALE_PORTUGUESE_BRAZIL:
    case XLOCALE_RUSSIAN:
    case XLOCALE_TURKISH:
    case XLOCALE_POLISH:
    case XLOCALE_PORTUGUESE_PORTUGAL:
        cDecimalCharacter = ',';
        break;
    default:
        break;
    }

    return cDecimalCharacter;
}

inline LPCSTR UnicodeText::GetDateSeparator(XLocale eXLocale) {
    switch (eXLocale) {
    case XLOCALE_ENGLISH_US:
    case XLOCALE_ENGLISH_EU:
    case XLOCALE_ENGLISH_SOUTHEAST_ASIA:
    case XLOCALE_SPANISH_LATAM:
    case XLOCALE_FRENCH:
    case XLOCALE_ITALIAN:
    case XLOCALE_KOREAN:
    case XLOCALE_PORTUGUESE_BRAZIL:
    case XLOCALE_RUSSIAN:
    case XLOCALE_TURKISH:
    case XLOCALE_PORTUGUESE_PORTUGAL:
    case XLOCALE_JAPANESE:
        return "/";
    case XLOCALE_SPANISH_EU:
    case XLOCALE_GERMAN:
    case XLOCALE_POLISH:
        return ".";
    default:
        return NULL;
    }
}
