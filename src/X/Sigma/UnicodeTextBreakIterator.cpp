




#include "UnicodeTextBreakIterator.h"

#include <stdlib.h>
#include <string.h>

#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "DataStructures/XGrowableMap.h"
#include "UnicodeText.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

class BreakIteratorParams {
public:
    BreakIteratorParams() {}
    BOOL operator==(const BreakIteratorParams& rhs) const;
    BreakIteratorParams(BreakStyle eStyle, XLocale eXLocale, BOOL bUnk);

    BreakStyle m_eStyle;
    XLocale m_eXLocale;
    BOOL unk_8;  // 2.7.7: UTBreakIterator ctor's 2nd arg; hashed with the key but not compared by operator==
};

inline BOOL BreakIteratorParams::operator==(const BreakIteratorParams& rhs) const {
    return m_eStyle == rhs.m_eStyle && m_eXLocale == rhs.m_eXLocale;
}

inline BreakIteratorParams::BreakIteratorParams(BreakStyle eStyle, XLocale eXLocale, BOOL bUnk)
    : m_eStyle(eStyle), m_eXLocale(eXLocale), unk_8(bUnk) {}

template <>
inline uint32 XHashKey<const BreakIteratorParams&>(const BreakIteratorParams& tParams) {
    return Blizzard::Util::Hash32(&tParams, sizeof(tParams));
}

class HydraBreakIterator {
public:
    HydraBreakIterator(const BreakIteratorParams& tParams);
    void setText(utf16* pszText, const uint32 nLen);
    BOOL isBoundary(int32 nOffset);
    int32 next();
    int32 previous();
    int32 following(int32 nOffset);
    int32 preceding(int32 nOffset);

private:
    static int32 sUTF16Comparator(const void* a, const void* b);
    BOOL sKinsokuIsForbiddenLineStart(const utf16 nCodepoint) const;
    BOOL sKinsokuIsForbiddenLineEnd(const utf16 nCodepoint) const;
    BOOL sKinsokuBoundary(int32 nOffset) const;
    BOOL sRomanBoundary(int32 nOffset) const;

    BreakIteratorParams m_tParams;
    utf16* m_pszText;
    int32 m_nLen;
    int32 m_nIndex;
};

inline HydraBreakIterator::HydraBreakIterator(const BreakIteratorParams& tParams)
    : m_tParams(tParams), m_pszText(NULL), m_nLen(0), m_nIndex(0) {}

inline void HydraBreakIterator::setText(utf16* pszText, const uint32 nLen) {
    m_pszText = pszText;
    m_nLen = nLen;
    m_nIndex = 0;
}

BOOL HydraBreakIterator::isBoundary(int32 nOffset) {
    switch (m_tParams.m_eXLocale) {
    case XLOCALE_KOREAN:
    case XLOCALE_CHINESE_CHINA:
    case XLOCALE_CHINESE_TAIWAN:
    case XLOCALE_JAPANESE:
    case XLOCALE_UNK_19:
        if (m_tParams.unk_8 || m_pszText[nOffset] <= 0x2AE) {
            return sRomanBoundary(nOffset);
        }
        return sKinsokuBoundary(nOffset);
    default:
        return sRomanBoundary(nOffset);
    }
}

int32 HydraBreakIterator::next() {
    int32 nRetVal = -1;

    switch (m_tParams.m_eStyle) {
    case BREAKSTYLE_CHAR:
        m_nIndex++;
        if (m_nIndex <= m_nLen) {
            nRetVal = m_nIndex;
        }
        break;
    case BREAKSTYLE_WORD:
    case BREAKSTYLE_SENTENCE:
    case BREAKSTYLE_LINE:
        for (int32 i = m_nIndex + 1; i < m_nLen; i++) {
            if (isBoundary(i)) {
                nRetVal = i;
                break;
            }
        }
        break;
    }

    return nRetVal;
}

inline int32 HydraBreakIterator::previous() {
    int32 nRetVal = -1;

    switch (m_tParams.m_eStyle) {
    case BREAKSTYLE_CHAR:
        m_nIndex--;
        if (m_nIndex >= 0) {
            nRetVal = m_nIndex;
        }
        break;
    case BREAKSTYLE_WORD:
    case BREAKSTYLE_SENTENCE:
    case BREAKSTYLE_LINE:
        for (int32 i = m_nIndex - 1; i >= 0; i--) {
            if (isBoundary(i)) {
                nRetVal = i;
                break;
            }
        }
        break;
    }

    return nRetVal;
}

inline int32 HydraBreakIterator::following(int32 nOffset) {
    m_nIndex = nOffset;
    return next();
}

inline int32 HydraBreakIterator::preceding(int32 nOffset) {
    m_nIndex = nOffset;
    return previous();
}

int32 HydraBreakIterator::sUTF16Comparator(const void* a, const void* b) {
    const utf16* pnCodepointA = (const utf16*)a;
    const utf16* pnCodepointB = (const utf16*)b;
    return *pnCodepointA - *pnCodepointB;
}

BOOL HydraBreakIterator::sKinsokuIsForbiddenLineStart(const utf16 nCodepoint) const {
    switch (m_tParams.m_eXLocale) {
    case XLOCALE_JAPANESE: {
        utf16 arJPJP[] = {
            0x0021, 0x0025, 0x0027, 0x0029, 0x002C, 0x002D, 0x002E, 0x002F,
            0x003A, 0x003B, 0x003F, 0x005D, 0x007D, 0x005C, 0x005D, 0x007D,
            0x00A2, 0x00B0, 0x00BB, 0x2010, 0x2013, 0x2014, 0x2019, 0x201D,
            0x2030, 0x2032, 0x2033, 0x203C, 0x2047, 0x2048, 0x2049, 0x2103,
            0x3001, 0x3002, 0x3005, 0x3009, 0x300B, 0x300D, 0x300F, 0x3011,
            0x3015, 0x3017, 0x3019, 0x301C, 0x301F, 0x303B, 0x3041, 0x3043,
            0x3045, 0x3047, 0x3049, 0x3063, 0x3083, 0x3085, 0x3087, 0x308E,
            0x3095, 0x3096, 0x309B, 0x309C, 0x309D, 0x309E, 0x30A1, 0x30A3,
            0x30A5, 0x30A7, 0x30A9, 0x30C3, 0x30E3, 0x30E5, 0x30E7, 0x30EE,
            0x30F5, 0x30F6, 0x30FB, 0x30FC, 0x30FD, 0x30FE, 0x31F0, 0x31F1,
            0x31F2, 0x31F3, 0x31F4, 0x31F5, 0x31F6, 0x31F7, 0x31F8, 0x31F9,
            0x31FA, 0x31FB, 0x31FC, 0x31FD, 0x31FE, 0x31FF, 0xFF01, 0xFF02,
            0xFF05, 0xFF07, 0xFF09, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF0F, 0xFF1A,
            0xFF1B, 0xFF1F, 0xFF3C, 0xFF3D, 0xFF5D, 0xFF5E, 0xFF60, 0xFF61,
            0xFF63, 0xFF64, 0xFF65, 0xFF9E, 0xFF9F, 0xFFE0,
        };
        return bsearch(&nCodepoint, arJPJP, ARRAY_SIZE(arJPJP), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    case XLOCALE_KOREAN: {
        utf16 arKOKR[] = {
            0x0021, 0x0025, 0x0029, 0x002C, 0x002E, 0x003A, 0x003B, 0x003F,
            0x005D, 0x007D, 0xA1A2, 0xA1AF, 0xA1B1, 0xA1B3, 0xA1B5, 0xA1B7,
            0xA1B9, 0xA1BB, 0xA1BD, 0xA1C6, 0xA1C7, 0xA1C8, 0xA1C9, 0xA1CB,
            0xA3A1, 0xA3A5, 0xA3A9, 0xA3AC, 0xA3AE, 0xA3BA, 0xA3BB, 0xA3BF,
            0xA3DC, 0xA3DD, 0xA3FD,
        };
        return bsearch(&nCodepoint, arKOKR, ARRAY_SIZE(arKOKR), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    case XLOCALE_CHINESE_TAIWAN: {
        utf16 arZHTW[] = {
            0x0021, 0x0029, 0x002C, 0x002E, 0x003A, 0x003B, 0x003F, 0x005D,
            0x007D, 0x3002, 0xA141, 0xA142, 0xA143, 0xA144, 0xA145, 0xA146,
            0xA147, 0xA148, 0xA149, 0xA14A, 0xA14B, 0xA14C, 0xA14D, 0xA14E,
            0xA14F, 0xA150, 0xA151, 0xA152, 0xA153, 0xA154, 0xA155, 0xA156,
            0xA157, 0xA158, 0xA159, 0xA15A, 0xA15B, 0xA15C, 0xA15D, 0xA15E,
            0xA160, 0xA162, 0xA164, 0xA166, 0xA168, 0xA16A, 0xA16C, 0xA16E,
            0xA170, 0xA172, 0xA174, 0xA176, 0xA178, 0xA17A, 0xA17C, 0xA17E,
            0xA1A2, 0xA1A4, 0xA1A6, 0xA1A8, 0xA1AA, 0xA1AC,
        };
        return bsearch(&nCodepoint, arZHTW, ARRAY_SIZE(arZHTW), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    case XLOCALE_UNK_19:
    case XLOCALE_CHINESE_CHINA: {
        utf16 arZHCN[] = {
            0x0021, 0x0029, 0x002C, 0x002E, 0x003A, 0x003B, 0x003F, 0x005D,
            0x007D, 0x3002, 0xA1A2, 0xA1A3, 0xA1A4, 0xA1A5, 0xA1A6, 0xA1A7,
            0xA1A8, 0xA1A9, 0xA1AA, 0xA1AB, 0xA1AC, 0xA1AD, 0xA1AF, 0xA1B1,
            0xA1B3, 0xA1B5, 0xA1B7, 0xA1B9, 0xA1BB, 0xA1BD, 0xA1BF, 0xA1C3,
            0xA3A1, 0xA3A2, 0xA3A7, 0xA3A9, 0xA3AC, 0xA3AE, 0xA3BA, 0xA3BB,
            0xA3BF, 0xA3DD, 0xA3E0, 0xA3FC, 0xA3FD,
        };
        return bsearch(&nCodepoint, arZHCN, ARRAY_SIZE(arZHCN), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    default:
        return TRUE;
    }
}

BOOL HydraBreakIterator::sKinsokuIsForbiddenLineEnd(const utf16 nCodepoint) const {
    switch (m_tParams.m_eXLocale) {
    case XLOCALE_JAPANESE: {
        utf16 arJPJP[] = {
            0x0024, 0x0028, 0x005B, 0x005C, 0x007B, 0x00A3, 0x00A5, 0x2018,
            0x201C, 0x3008, 0x300A, 0x300C, 0x300E, 0x3010, 0x3014, 0xFF04,
            0xFF08, 0xFF3B, 0xFF3C, 0xFF5B, 0xFF62, 0xFFE1, 0xFFE5,
        };
        return bsearch(&nCodepoint, arJPJP, ARRAY_SIZE(arJPJP), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    case XLOCALE_KOREAN: {
        utf16 arKOKR[] = {
            0x0028, 0x005B, 0x005C, 0x007B, 0xA1AE, 0xA1B0, 0xA1B2, 0xA1B4,
            0xA1B6, 0xA1B8, 0xA1BA, 0xA1BC, 0xA3A4, 0xA3A8, 0xA3DB, 0xA3DC,
            0xA3FB,
        };
        return bsearch(&nCodepoint, arKOKR, ARRAY_SIZE(arKOKR), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    case XLOCALE_CHINESE_TAIWAN: {
        utf16 arZHTW[] = {
            0x0028, 0x005B, 0x007B, 0xA15D, 0xA14F, 0xA161, 0xA163, 0xA165,
            0xA167, 0xA169, 0xA16B, 0xA16D, 0xA16F, 0xA171, 0xA1A3, 0xA1A5,
            0xA1A7, 0xA1A9, 0xA1AB,
        };
        return bsearch(&nCodepoint, arZHTW, ARRAY_SIZE(arZHTW), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    case XLOCALE_UNK_19:
    case XLOCALE_CHINESE_CHINA: {
        utf16 arZHCN[] = {
            0x0028, 0x005B, 0x007B, 0xA1AE, 0xA1B0, 0xA1B2, 0xA1B4, 0xA1B6,
            0xA1B8, 0xA1BA, 0xA1BC, 0xA1BE, 0xA3A8, 0xA3AE, 0xA3B0, 0xA3B1,
            0xA3B2, 0xA3B3, 0xA3B4, 0xA3B5, 0xA3B6, 0xA3B7, 0xA3B8, 0xA3B9,
            0xA3DB, 0xA3FB,
        };
        return bsearch(&nCodepoint, arZHCN, ARRAY_SIZE(arZHCN), sizeof(utf16), sUTF16Comparator) != NULL;
    }
    default:
        return TRUE;
    }
}

inline BOOL HydraBreakIterator::sKinsokuBoundary(int32 nOffset) const {
    if (nOffset > 0 && sKinsokuIsForbiddenLineEnd(m_pszText[nOffset - 1])) {
        return FALSE;
    }
    if (nOffset < m_nLen && sKinsokuIsForbiddenLineStart(m_pszText[nOffset])) {
        return FALSE;
    }
    return TRUE;
}

inline BOOL HydraBreakIterator::sRomanBoundary(int32 nOffset) const {
    switch (m_pszText[nOffset]) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        return TRUE;
    default:
        break;
    }
    if (nOffset > 0 && m_pszText[nOffset - 1] == '-') {
        return TRUE;
    }
    return FALSE;
}

class BreakIteratorData {
public:
    BreakIteratorData() {}
    ~BreakIteratorData() {
        delete m_ptIterator;
        delete[] m_text;
    }

    HydraBreakIterator* m_ptIterator;
    utf16* m_text;
};

typedef XGrowableMap<BreakIteratorParams, const BreakIteratorParams&, BreakIteratorData*, BreakIteratorData*>
    BreakIteratorMap;

struct UTBreakIteratorGlobals {
    ~UTBreakIteratorGlobals() {}
    UTBreakIteratorGlobals() : tBreakIteratorMap(16, NULL) {}
    void DisposeBreakIterators();

    BreakIteratorMap tBreakIteratorMap;
};

static UTBreakIteratorGlobals sgtUTBIGlobals;

namespace UnicodeTextBreakIterators {
BreakIteratorData* GetBreakIterator(const BreakIteratorParams& tParams);
}  // namespace UnicodeTextBreakIterators

BreakIteratorData* UnicodeTextBreakIterators::GetBreakIterator(const BreakIteratorParams& tParams) {
    BreakIteratorData* ptIteratorData;

    if (sgtUTBIGlobals.tBreakIteratorMap.Lookup(tParams, ptIteratorData)) {
        return ptIteratorData;
    }

    ptIteratorData = new BreakIteratorData;
    ptIteratorData->m_ptIterator = new HydraBreakIterator(tParams);
    ptIteratorData->m_text = NULL;

    sgtUTBIGlobals.tBreakIteratorMap.SetAt_Tracked(tParams, ptIteratorData);

    return ptIteratorData;
}

UTBreakIterator::UTBreakIterator(BreakStyle eBreakStyle, BOOL bUnk) : m_eBreakStyle(eBreakStyle), m_data(NULL) {
    BreakIteratorParams tParams(eBreakStyle, UnicodeText::GetStringDisplayLocale(), bUnk);
    m_data = UnicodeTextBreakIterators::GetBreakIterator(tParams);
}

UTBreakIterator::~UTBreakIterator() {}

void UTBreakIterator::SetText(LPCSTR szText) {
    size_t nUTF16Codepoints;
    int32 nLen = strlen(szText);

    nUTF16Codepoints = 0;

    if (Blizzard::Unicode::CountUTF8Chars(szText, nLen, nUTF16Codepoints)) {
        size_t nBufferSize = nUTF16Codepoints + 1;
        delete[] m_data->m_text;
        m_data->m_text = new utf16[nBufferSize];
        Blizzard::Unicode::ConvertUTF8to16(m_data->m_text, nBufferSize, szText, 0xFFFFFFFF, NULL, NULL);
    }

    m_data->m_ptIterator->setText(m_data->m_text, nUTF16Codepoints);
}

int32 UTBreakIterator::Following(int32 nOffset) {
    return m_data->m_ptIterator->following(nOffset);
}

int32 UTBreakIterator::Preceding(int32 nOffset) {
    return m_data->m_ptIterator->preceding(nOffset);
}

BOOL UTBreakIterator::IsBoundary(int32 nOffset) {
    return m_data->m_ptIterator->isBoundary(nOffset);
}
