#pragma once

#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "RefString.h"
#include "SigmaTypes.h"
#include "Unicode.h"

enum {
    XLOCALE_INVALID = 0,
    XLOCALE_GLOBAL = 1,
    XLOCALE_LOCALE_START = 2,
    XLOCALE_ENGLISH_US = 2,
    XLOCALE_ENGLISH_EU = 3,
    XLOCALE_ENGLISH_SOUTHEAST_ASIA = 4,
    XLOCALE_SPANISH_EU = 5,
    XLOCALE_SPANISH_LATAM = 6,
    XLOCALE_FRENCH = 7,
    XLOCALE_ITALIAN = 8,
    XLOCALE_GERMAN = 9,
    XLOCALE_KOREAN = 10,
    XLOCALE_PORTUGUESE_BRAZIL = 11,
    XLOCALE_RUSSIAN = 12,
    XLOCALE_CHINESE_CHINA = 13,
    XLOCALE_CHINESE_TAIWAN = 14,
    XLOCALE_TURKISH = 15,
    XLOCALE_POLISH = 16,
    XLOCALE_PORTUGUESE_PORTUGAL = 17,
    XLOCALE_JAPANESE = 18,
    NUM_PC_XLOCALES = 18,
    NUM_XLOCALES = 19,
};

typedef int32 XLocale;

enum FormatNumberStyle {
    FNS_NORMAL = 0,
    FNS_CURRENCY = 1,
    FNS_PERCENT = 2,
    NUM_FORMAT_NUMBER_STYLES = 3,
};

enum FormatDateStyle {
    FDS_SHORT = 0,
    FDS_MEDIUM = 1,
    FDS_LONG = 2,
    FDS_FULL = 3,
};

enum FormatTimeStyle {
    FTS_NONE = 0,
    FTS_SHORT = 1,
    FTS_MEDIUM = 2,
    FTS_LONG = 3,
    FTS_FULL = 4,
};

XLocale XLocaleGetBaseLocale(const XLocale eLocale);
LPCSTR XLocaleToString(XLocale eLocale);
XLocale XLocaleFromString(LPCSTR pszXLocale);

namespace UnicodeText {
void SetStringDisplayLocale(const XLocale eXLocale);
XLocale GetStringDisplayLocale();
XLocale GetCurrentLocale();

CRefString Capitalize(LPCSTR szUTF8Str);
CRefString Uncapitalize(LPCSTR szUTF8Str);

BOOL Initialize();
void Dispose();
void SetLocale(const XLocale eXLocale);

void ToUpper(uint32& uChar);
void ToUpper(LPSTR szUTF8Str, const size_t nLength);
void ToLower(uint32& uChar);

CRefString sProcessGrouping(LPCSTR sNumber, uint32 nNumberLen, XLocale eXLocale);

CRefString FormatNumber(const int32 nNumber, const BOOL fGroupDigits = TRUE, const FormatNumberStyle eStyle = FNS_NORMAL, XLocale eXLocale = XLOCALE_INVALID);  // defaults: GamerProfileNX64.cpp row 587:56 (.str() column fits FormatNumber(nGuestIndex + 2)) with w1=1, w2=0, w3=0
CRefString FormatNumber(const int64 nNumber, const BOOL fGroupDigits, const FormatNumberStyle eStyle, XLocale eXLocale);
CRefString FormatNumber(const uint64 nNumber, BOOL fGroupDigits, const FormatNumberStyle eStyle, XLocale eXLocale);
CRefString FormatNumber(const float flNumber, const BOOL fGroupDigits, const FormatNumberStyle eStyle,
                        const int32 nMinFractionalDigits, const int32 nMaxFractionalDigits, XLocale eXLocale);
CRefString FormatNumber(LPCSTR szNumber, const FormatNumberStyle eStyle, const int32 nMinFractionalDigits,
                        const int32 nMaxFractionalDigits, XLocale eXLocale);

CRefString FormatDate(Blizzard::Time::Timestamp tDate, const FormatDateStyle eDateStyle, XLocale eXLocale);
CRefString sFormatShortDate(uint32 uDay, uint32 uMonth, uint32 uYear, XLocale eXLocale);
CRefString sFormatTime(uint32 uHour, uint32 uMinute, XLocale eXLocale);
CRefString FormatDateTime(Blizzard::Time::Timestamp tDate, const FormatDateStyle eDateStyle,
                          const FormatTimeStyle eTimeStyle, XLocale eXLocale);

LPCSTR GetThousandSeparator(XLocale eXLocale);
CHAR GetDecimalCharacter(XLocale eXLocale);
LPCSTR GetDateSeparator(XLocale eXLocale);
}  // namespace UnicodeText
