




#include "LanguageRules.h"

#include <ctype.h>
#include <string.h>

#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "DataStructures/XArray.h"
#include "DataStructures/XGrowableMap.h"
#include "Memory/SigmaMemory.h"
#include "StringManip.h"
#include "UnicodeText.h"

// X/Sigma/UnicodeText.cpp
extern char output8[2048];

static const uint32 MAX_CASES = 10;

struct DeclensionTableEntry {
    CRefString sDeclension[MAX_CASES];
};

typedef XGrowableMap<CRefString, const CRefString&, DeclensionTableEntry, const DeclensionTableEntry&> DeclensionMap;

static utf16 output16[2048];
static utf16 text16[2048];

static const CHAR cGrammaticalFormStartChar = '[';
static const CHAR cGrammaticalFormEndChar = ']';

static LPCSTR cszNoun1Demarcator = "[n1]";
static const int32 cnNoun1DemarcatorLength = 4;
static LPCSTR cszNoun2Demarcator = "[n2]";
static const int32 cnNoun2DemarcatorLength = 4;

struct LanguageRulesGlobals {
    ~LanguageRulesGlobals() {}
    LanguageRulesGlobals() : m_tDeclensionMap(16, NULL), m_fMale(FALSE) {}

    DeclensionMap m_tDeclensionMap;
    BOOL m_fMale;
};

static LanguageRulesGlobals sgtLangGlobals;

static void sLoadDeclensionLine(LPCSTR szLine) {
    char szNominative[128];
    char szCaseCode[2];
    char szDeclinedWord[128];

    StringTokenizer tLineTokenizer(szLine, "\t");
    if (tLineTokenizer.GetNextToken(szNominative, sizeof(szNominative)) &&
        tLineTokenizer.GetNextToken(szCaseCode, sizeof(szCaseCode)) &&
        tLineTokenizer.GetNextToken(szDeclinedWord, sizeof(szDeclinedWord))) {
        if (szCaseCode[0] >= '1' && szCaseCode[0] <= '9') {
            DeclensionTableEntry tEntry;
            sgtLangGlobals.m_tDeclensionMap.Lookup(szNominative, tEntry);
            tEntry.sDeclension[szCaseCode[0] - '1'] = szDeclinedWord;
            sgtLangGlobals.m_tDeclensionMap.SetAt_Tracked(szNominative, tEntry);
        }
    }
}

void LanguageRules::InitializeDeclensions(const char* pData) {
    DisposeDeclensions();

    if (pData != NULL) {
        StringTokenizer tTokenizer(pData, "\r\n");
        char szToken[1024];
        while (tTokenizer.GetNextToken(szToken, sizeof(szToken))) {
            sLoadDeclensionLine(szToken);
        }
    }
}

void LanguageRules::DisposeDeclensions() {
    sgtLangGlobals.m_tDeclensionMap.RemoveAll();
}

void LanguageRules::SetClientGender(const BOOL fMale) {
    sgtLangGlobals.m_fMale = fMale;
}

static const char* sJongsungRule(const char* text);
static const char* sPossessiveRule(const char* text);
static const char* sDeclensionRule(const char* text);
static const char* sPluralRule(const char* text);
static const char* sGenderRule(const char* text);

// A reconstruction of the five rule helpers the DWARF records as inlined here (sJongsungRule,
// sPossessiveRule, sDeclensionRule, sPluralRule, sGenderRule and what they inline), written from
// the decompiler output.




// sAppendUTF16() and sStrLenUTF16() really do loop without advancing: that is what the original
// compiles to (an endless loop once a replacement matches).
#line 144
const char* LanguageRules::ApplyLanguageRules(
    const char* szUTF8Str)
{
    if (strlen(szUTF8Str) > 2048)
    {





        SigmaMemoryCopy(output8, szUTF8Str, 2048);
        char* ptr = output8;
        ptr[2045] = '\0';
        ptr[2046] = '\0';
        ptr[2047] = '\0';
        szUTF8Str = ptr;
    }
    BOOL fHasLanguageRule;
    do
    {
        fHasLanguageRule = FALSE;

        int32 nCodepoint;
        LPCSTR spot = szUTF8Str;
        while ((nCodepoint = Blizzard::Unicode::GetCodepointFromUTF8(&spot)) != 0)
        {
            if (nCodepoint == '|')
            {
                switch (*spot)
                {

                case '1':
                    szUTF8Str = sJongsungRule(szUTF8Str);
                    fHasLanguageRule = TRUE;
                    break;
                case '2':
                    szUTF8Str = sPossessiveRule(szUTF8Str);
                    fHasLanguageRule = TRUE;
                    break;
                case '3':
                    szUTF8Str = sDeclensionRule(szUTF8Str);
                    fHasLanguageRule = TRUE;
                    break;
                case '4':
                    szUTF8Str = sPluralRule(szUTF8Str);
                    fHasLanguageRule = TRUE;
                    break;
                case '5':
                    szUTF8Str = sGenderRule(szUTF8Str);
                    fHasLanguageRule = TRUE;
                    break;
                default:
                    break;
                }
            }

            if (fHasLanguageRule)
            {
                break;
            }
        }
    } while (fHasLanguageRule);

    return szUTF8Str;
}
#line 152

static const utf16* CheckJongsung(utf16 ch, const utf16* option1, const utf16* option2) {
    if ((ch & 0xFF00) == 0) {
        if (*option1 == 0xC73C) {
            switch (ch) {
            case '1':
            case '7':
            case '8':
            case 'L':
            case 'l':
                return option2;
            default:
                break;
            }
        }

        if (ch >= '0' && ch <= '9') {
            switch (ch) {
            case '0':
            case '1':
            case '3':
            case '6':
            case '7':
            case '8':
                return option1;
            default:
                return option2;
            }
        }

        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            switch (tolower(ch)) {
            case 'b':
            case 'c':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'p':
            case 'q':
                return option1;
            default:
                return option2;
            }
        }

        return option1;
    }

    if (ch < 0xAC00 || ch > 0xD7A3) {
        return option1;
    }

    if ((ch - 0xAC00) % 28 == 8 && *option1 == 0xC73C) {
        return option2;
    }

    if ((ch - 0xAC00) % 28 != 0) {
        return option1;
    }

    return option2;
}

static const char* sJongsungRule(const char* text) {
    Blizzard::Unicode::ConvertUTF8to16(text16, 2048, text, 0xFFFFFFFF, NULL, NULL);

    int32 nEscSeqCount = 0;
    utf16 lastText = 0;
    utf16* writepos = output16;

    for (utf16* spot = text16; *spot != 0; spot++) {
        if (*spot == '{') {
            nEscSeqCount++;
        } else if (*spot == '|') {
            if (spot[1] == '1' && !(spot[2] >= '0' && spot[2] <= '9')) {
                utf16* option2 = spot + 2;
                utf16* option1 = spot + 2;

                while (*option2 != 0 && *option2++ != ';') {
                }

                spot = option2;
                while (*spot != 0 && *spot++ != ';') {
                }
                spot--;

                const utf16* option = CheckJongsung(lastText, option1, option2);
                while (*option != 0 && *option != ';') {
                    *writepos++ = *option++;
                }
            } else {
                *writepos++ = '|';
                *writepos++ = spot[1];
                spot++;
            }
            continue;
        }

        *writepos++ = *spot;

        if (nEscSeqCount > 0) {
            if (*spot == '}') {
                nEscSeqCount--;
            }
        } else if ((*spot >= 'A' && *spot <= 'Z') || (*spot >= 'a' && *spot <= 'z') || (*spot >= '0' && *spot <= '9') ||
                   (*spot >= 0xAC00 && *spot <= 0xD7A3)) {
            lastText = *spot;
        }
    }

    *writepos = 0;
    Blizzard::Unicode::ConvertUTF16to8(output8, 2048, output16, 0xFFFFFFFF, NULL, NULL);
    return output8;
}

static BOOL IsVowel(utf16 ch) {
    switch (ch) {
    case 'A':
    case 'E':
    case 'I':
    case 'O':
    case 'U':
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return TRUE;
    default:
        break;
    }

    return (ch >= 0xC0 && ch <= 0xC6) || (ch >= 0xC8 && ch <= 0xCF) || (ch >= 0xD2 && ch <= 0xD6) ||
           (ch >= 0xD8 && ch <= 0xDC) || (ch >= 0xE0 && ch <= 0xE6) || (ch >= 0xE8 && ch <= 0xEF) ||
           (ch >= 0xF2 && ch <= 0xF6) || (ch >= 0xF8 && ch <= 0xFC);
}

struct UnicodeString {
    UnicodeString();
    UnicodeString(const char* szStr);
    const utf16* getTerminatedBuffer();

private:
    utf16 m_str[64];
};

struct ReplacementEntry {
    UnicodeString find;
    UnicodeString replace;
};

inline UnicodeString::UnicodeString(const char* szStr) {
    Blizzard::Unicode::ConvertUTF8to16(m_str, 64, szStr, 0xFFFFFFFF, NULL, NULL);
}

inline const utf16* UnicodeString::getTerminatedBuffer() {
    return m_str;
}

static uint32 sStrLenUTF16(const utf16* text) {
    uint32 i;
    for (i = 0; *text != 0; i++) {
    }
    return i;
}

static BOOL sStringMatchUTF16(const utf16* src, const utf16* cmp) {
    while (*cmp != 0) {
        if (*src == 0 || *src != *cmp) {
            return FALSE;
        }
        src++;
        cmp++;
    }
    return TRUE;
}

static void sAppendUTF16(utf16*& dst, const utf16* text) {
    while (*text != 0) {
        *dst++ = *text;
    }
}

static BOOL sApplyReplacement(utf16*& text, utf16*& writepos, ReplacementEntry* ptTable,
                              const int32 nNumReplacementEntries) {
    for (int32 i = 0; i < nNumReplacementEntries; i++) {
        if (sStringMatchUTF16(text, ptTable[i].find.getTerminatedBuffer())) {
            text += sStrLenUTF16(ptTable[i].find.getTerminatedBuffer());
            sAppendUTF16(writepos, ptTable[i].replace.getTerminatedBuffer());
            return TRUE;
        }
    }
    return FALSE;
}

static const char* sPossessiveRule(const char* text) {
    XLocale eLocale = UnicodeText::GetStringDisplayLocale();

    Blizzard::Unicode::ConvertUTF8to16(text16, 2048, text, 0xFFFFFFFF, NULL, NULL);

    utf16* writepos = output16;

    for (utf16* spot = text16; *spot != 0; spot++) {
        if (*spot == '|') {
            spot++;
            if (*spot == '2' && !(spot[1] >= '0' && spot[1] <= '9')) {
                spot++;
                while (*spot == ' ') {
                    spot++;
                }

                if (eLocale == XLOCALE_FRENCH) {
                    ReplacementEntry arReplacementTable[] = {
                        {"l'", "de l'"},
                        {"le ", "du "},
                        {"la ", "de la "},
                        {"les ", "des "},

                        {"L'", "de l'"},
                        {"Le ", "du "},
                        {"La ", "de la "},
                        {"Les ", "des "},
                    };
                    if (!sApplyReplacement(spot, writepos, arReplacementTable, 8)) {
                        if (IsVowel(*spot) || *spot == 'h' || *spot == 'H' ||
                            ((*spot == 'y' || *spot == 'Y') &&
                             IsVowel(spot[1]))) {
                            UnicodeString replacement("d'");
                            sAppendUTF16(writepos, replacement.getTerminatedBuffer());
                        } else {
                            UnicodeString replacement("de ");
                            sAppendUTF16(writepos, replacement.getTerminatedBuffer());
                        }
                    }
                } else if (eLocale == XLOCALE_PORTUGUESE_BRAZIL || eLocale == XLOCALE_PORTUGUESE_PORTUGAL) {
                    ReplacementEntry arReplacementTable[] = {
                        {"o ", "do "},
                        {"os ", "dos "},
                        {"a ", "da "},
                        {"as ", "das "},
                    };
                    sApplyReplacement(spot, writepos, arReplacementTable, 4);
                }
            } else {
                *writepos++ = '|';
            }
        }

        *writepos++ = *spot;
    }

    *writepos = 0;
    Blizzard::Unicode::ConvertUTF16to8(output8, 2048, output16, 0xFFFFFFFF, NULL, NULL);
    return output8;
}

static LPCSTR sGetDeclension(LPCSTR szNominative, const int32 nCaseIndex) {
    DeclensionTableEntry tEntry;

    if (sgtLangGlobals.m_tDeclensionMap.Lookup(szNominative, tEntry)) {
        if (tEntry.sDeclension[nCaseIndex].Length() > 0) {
            return tEntry.sDeclension[nCaseIndex].str();
        }
    }

    return szNominative;
}

static const char* sDeclensionRule(const char* text) {
    char temp[2048];
    char* writepos = output8;

    if (text == output8) {
        Blizzard::String::Copy(temp, output8, sizeof(temp));
        text = temp;
    }

    for (const char* spot = text; *spot != '\0'; spot++) {
        if (*spot == '|') {
            spot++;
            if (*spot == '3' && !(spot[1] >= '0' && spot[1] <= '9')) {
                char szNominative[128];
                int32 nCaseIndex = 0;
                char* bufp = szNominative;
                LPCSTR pszEscSeqOpenEnd;
                LPCSTR pszEscSeqCloseStart;
                LPCSTR pszDeclension;

                if (spot[1] == '-') {
                    nCaseIndex = spot[2] - '1';
                    if (nCaseIndex >= 0 && nCaseIndex < (int32)MAX_CASES - 1) {
                        spot += 2;
                        if (nCaseIndex < (int32)MAX_CASES) {
                            spot++;
                        }
                        if (*spot == '(') {
                            spot++;
                        }
                    } else {
                        nCaseIndex = 0;
                    }
                }

                while (*spot != '\0' && *spot != ')') {
                    if (bufp < szNominative + sizeof(szNominative) - 1) {
                        *bufp++ = *spot;
                    }
                    spot++;
                }
                *bufp = '\0';

                if (*spot == '\0') {
                    spot--;
                }

                pszEscSeqOpenEnd = NULL;
                pszEscSeqCloseStart = NULL;
                if (szNominative[0] == '{') {
                    pszEscSeqOpenEnd = strchr(szNominative + 1, '}');
                    if (pszEscSeqOpenEnd != NULL) {
                        pszEscSeqCloseStart = strchr(pszEscSeqOpenEnd, '{');
                    }
                }

                if (pszEscSeqOpenEnd == NULL || pszEscSeqCloseStart == NULL) {
                    pszDeclension = sGetDeclension(szNominative, nCaseIndex);
                    pszEscSeqCloseStart = NULL;
                } else {
                    char szNewNominative[128];
                    LPCSTR pszSrc = szNominative;
                    int32 nLen = (int32)(pszEscSeqOpenEnd - szNominative);

                    while (nLen-- >= 0) {
                        *writepos++ = *pszSrc++;
                    }

                    nLen = (int32)(pszEscSeqCloseStart - (pszEscSeqOpenEnd + 1));
                    Blizzard::String::Copy(szNewNominative, pszEscSeqOpenEnd + 1, nLen + 1);

                    pszDeclension = sGetDeclension(szNewNominative, nCaseIndex);
                }

                while (*pszDeclension != '\0') {
                    *writepos++ = *pszDeclension++;
                }

                if (pszEscSeqCloseStart != NULL) {
                    LPCSTR pszSrc = pszEscSeqCloseStart;
                    while (*pszSrc != '\0') {
                        *writepos++ = *pszSrc++;
                    }
                }
            } else {
                *writepos++ = '|';
                *writepos++ = *spot;
            }
        } else {
            *writepos++ = *spot;
        }
    }

    *writepos = '\0';
    return output8;
}

enum PluralRule {
    PLURAL_RULE_ZERO_IS_SINGULAR = 0,
    PLURAL_RULE_ZERO_IS_PLURAL = 1,
    PLURAL_RULE_RUSSIAN = 2,
    PLURAL_RULE_POLISH = 3,
};

static uint32 sGetPluralIndex(const PluralRule eRule, const int32 ordinal) {
    switch (eRule) {
    case PLURAL_RULE_ZERO_IS_SINGULAR:
        if (ordinal > 1) {
            return 1;
        }
        return 0;

    case PLURAL_RULE_ZERO_IS_PLURAL:
        if (ordinal != 1) {
            return 1;
        }
        return 0;

    case PLURAL_RULE_RUSSIAN:
        if (ordinal % 100 >= 11 && ordinal % 100 <= 14) {
            return 2;
        }
        if (ordinal % 10 >= 2 && ordinal % 10 <= 4) {
            return 1;
        }
        if (ordinal % 10 == 1) {
            return 0;
        }
        return 2;

    case PLURAL_RULE_POLISH:
        if (ordinal == 1) {
            return 0;
        }
        if (ordinal % 10 >= 2 && ordinal % 10 <= 4) {
            return 1;
        }
        return 2;
    }

    return 0;
}

static PluralRule sGetPluralRule() {
    switch (UnicodeText::GetStringDisplayLocale()) {
    case XLOCALE_FRENCH:
    case XLOCALE_KOREAN:
    case XLOCALE_CHINESE_CHINA:
    case XLOCALE_CHINESE_TAIWAN:
    case XLOCALE_TURKISH:
    case XLOCALE_JAPANESE:
        return PLURAL_RULE_ZERO_IS_SINGULAR;
    case XLOCALE_RUSSIAN:
        return PLURAL_RULE_RUSSIAN;
    case XLOCALE_POLISH:
        return PLURAL_RULE_POLISH;
    default:
        return PLURAL_RULE_ZERO_IS_PLURAL;
    }
}

static const char* sPluralRule(const char* text) {
    char* writepos = output8;
    int number = 0;
    BOOL fParsingNumber = FALSE;
    const PluralRule eRule = sGetPluralRule();

    for (const char* spot = text; *spot != '\0'; spot++) {
        if (*spot >= '0' && *spot <= '9') {
            if (!fParsingNumber) {
                number = 0;
            }
            number = number * 10 + (*spot - '0');
            fParsingNumber = TRUE;
            *writepos++ = *spot;
        } else if (*spot == '|') {
            spot++;
            if (*spot == '4' && !(spot[1] >= '0' && spot[1] <= '9')) {
                spot++;

                const uint32 nIndex = sGetPluralIndex(eRule, number);
                for (uint i = 0; i < nIndex; i++) {
                    while (*spot != '\0' && *spot != ':' && *spot != ';') {
                        spot++;
                    }
                    if (*spot != ':') {
                        break;
                    }
                    spot++;
                }

                while (*spot == ' ') {
                    spot++;
                }

                while (*spot != '\0' && *spot != ':' && *spot != ';') {
                    *writepos++ = *spot++;
                }

                while (*spot != '\0' && *spot != ';') {
                    spot++;
                }

                if (*spot == '\0') {
                    break;
                }
            } else {
                *writepos++ = '|';
                *writepos++ = *spot;
            }
            fParsingNumber = FALSE;
        } else {
            fParsingNumber = FALSE;
            *writepos++ = *spot;
        }
    }

    *writepos = '\0';
    return output8;
}

static const char* sGenderRule(const char* text) {
    BOOL fMale = sgtLangGlobals.m_fMale;

    Blizzard::Unicode::ConvertUTF8to16(text16, 2048, text, 0xFFFFFFFF, NULL, NULL);

    utf16* writepos = output16;

    for (utf16* spot = text16; *spot != 0; spot++) {
        if (*spot == '|') {
            spot++;
            if (*spot == '5' && !(spot[1] >= '0' && spot[1] <= '9')) {
                spot++;

                if (!fMale) {
                    while (*spot != ':' && *spot != 0 && *spot != ';') {
                        spot++;
                    }
                    spot++;
                }

                while (*spot == ' ') {
                    spot++;
                }

                while (*spot != 0 && *spot != ':' && *spot != ';') {
                    *writepos++ = *spot++;
                }

                while (*spot != 0 && *spot != ';') {
                    spot++;
                }

                if (*spot == 0) {
                    break;
                }
            } else {
                *writepos++ = '|';
                *writepos++ = *spot;
            }
        } else {
            *writepos++ = *spot;
        }
    }

    *writepos = 0;
    Blizzard::Unicode::ConvertUTF16to8(output8, 2048, output16, 0xFFFFFFFF, NULL, NULL);
    return output8;
}

static void sConvertGrammaticalFormCharToBit(uint32& dwGrammaticalForm, const CHAR uChar) {
    switch (uChar) {
    case 'f':
        dwGrammaticalForm |= 1 << GF_FEMININE_BIT;
        break;
    case 'm':
        dwGrammaticalForm |= 1 << GF_MASCULINE_BIT;
        break;
    case 'n':
        dwGrammaticalForm |= 1 << GF_NEUTER_BIT;
        break;
    case 's':
        dwGrammaticalForm |= 1 << GF_SINGULAR_BIT;
        break;
    case 'p':
        dwGrammaticalForm |= 1 << GF_PLURAL_BIT;
        break;
    case 'c':
        dwGrammaticalForm |= 1 << GF_CONCATENATE_BIT;
        break;
    default:
        break;
    }
}

static void sConvertGrammaticalFormBitsToString(CRefString& sGrammaticalForm, const uint32 dwGrammaticalForm) {
    sGrammaticalForm = "[";

    if (dwGrammaticalForm & (1 << GF_FEMININE_BIT)) {
        sGrammaticalForm += "f";
    }
    if (dwGrammaticalForm & (1 << GF_MASCULINE_BIT)) {
        sGrammaticalForm += "m";
    }
    if (dwGrammaticalForm & (1 << GF_NEUTER_BIT)) {
        sGrammaticalForm += "n";
    }
    if (dwGrammaticalForm & (1 << GF_AH_TITLE_BIT)) {
        sGrammaticalForm += "ah";
    }
    if (dwGrammaticalForm & (1 << GF_SINGULAR_BIT)) {
        sGrammaticalForm += "s";
    }
    if (dwGrammaticalForm & (1 << GF_PLURAL_BIT)) {
        sGrammaticalForm += "p";
    }
    if (dwGrammaticalForm & (1 << GF_CONCATENATE_BIT)) {
        sGrammaticalForm += "c";
    }

    sGrammaticalForm += "]";
}

BOOL LanguageRules::ExtractGrammaticalForm(const CRefString& sInput, CRefString& sNoun1, CRefString& sNoun2,
                                           uint32& dwGrammaticalForm) {
    LPCSTR pszRemainder;
    LPCSTR pszNoun2Pos;

    dwGrammaticalForm = 0;
    sNoun2 = "";

    if (strncmp(sInput, cszNoun1Demarcator, cnNoun1DemarcatorLength) == 0) {
        pszRemainder = (LPCSTR)sInput + cnNoun1DemarcatorLength;
    } else {
        if (((LPCSTR)sInput)[0] != cGrammaticalFormStartChar) {
            sNoun1 = sInput;
            return TRUE;
        }

        LPCSTR pszGrammaticalEnd = strchr(sInput.str(), cGrammaticalFormEndChar);
        if (pszGrammaticalEnd == NULL) {
            sNoun1 = "Missing grammatical form terminator";
            return FALSE;
        }

        size_t nLength = pszGrammaticalEnd - ((LPCSTR)sInput + 1);
        for (size_t i = 0; i < nLength; i++) {
            CHAR uChar = ((LPCSTR)sInput)[i + 1];
            sConvertGrammaticalFormCharToBit(dwGrammaticalForm, uChar);
        }

        pszRemainder = pszGrammaticalEnd + 1;
        if (strncmp(pszRemainder, cszNoun1Demarcator, cnNoun1DemarcatorLength) != 0) {
            sNoun1 = pszRemainder;
            if (sNoun1.Length() == 0) {
                sNoun1 = "Contains a grammatical form but no translation";
                return FALSE;
            }
            return TRUE;
        }
        pszRemainder += cnNoun1DemarcatorLength;
    }

    pszNoun2Pos = Blizzard::String::Find(pszRemainder, cszNoun2Demarcator, 0x7FFFFFFF);
    if (pszNoun2Pos == NULL) {
        sNoun1 = pszRemainder;
        if (sNoun1.Length() == 0) {
            sNoun1 = "Contains a grammatical form but no translation";
            return FALSE;
        }
    } else {
        sNoun1 = CRefString(pszRemainder, (int32)(pszNoun2Pos - pszRemainder));
        if (sNoun1.Length() == 0) {
            sNoun1 = "Contains a grammatical form but no translation";
            return FALSE;
        }
        sNoun2 = pszNoun2Pos + cnNoun2DemarcatorLength;
    }

    return TRUE;
}

BOOL LanguageRules::ExtractMatchingGrammaticalForm(CRefString& usResult, const uint32 dwGrammaticalForm,
                                                   uint32& dwExtractionFlags) {
    dwExtractionFlags = 0;

    if (((LPCSTR)usResult)[0] != cGrammaticalFormStartChar) {
        return TRUE;
    }

    if (dwGrammaticalForm == 0) {
        usResult = "Requires a grammatical form from the caller";
        return FALSE;
    }

    CRefString sGrammaticalForm;
    BOOL fTryWithoutConcatenation = TRUE;
    const CHAR* puszMatchedForm = NULL;

    if (dwGrammaticalForm & (1 << GF_CONCATENATE_BIT)) {
        sConvertGrammaticalFormBitsToString(sGrammaticalForm, 1 << GF_CONCATENATE_BIT);
        puszMatchedForm = Blizzard::String::Find(usResult, sGrammaticalForm.str(), 0x7FFFFFFF);
        if (puszMatchedForm != NULL) {
            dwExtractionFlags |= 1 << GFE_CAN_CONCATENATE_BIT;
            fTryWithoutConcatenation = FALSE;
        }
    }

    if (fTryWithoutConcatenation) {
        uint32 dwGrammaticalFormToUse = dwGrammaticalForm & ~(1 << GF_CONCATENATE_BIT);
        sConvertGrammaticalFormBitsToString(sGrammaticalForm, dwGrammaticalFormToUse);
        puszMatchedForm = Blizzard::String::Find(usResult, sGrammaticalForm.str(), 0x7FFFFFFF);
        if (puszMatchedForm == NULL) {
            usResult.Format("Has no matching grammatical form for '%s'", sGrammaticalForm.str());
            return FALSE;
        }
    }

    CRefString sNewResult;

    puszMatchedForm += sGrammaticalForm.Length();
    if (strncmp(puszMatchedForm, cszNoun1Demarcator, cnNoun1DemarcatorLength) == 0) {
        puszMatchedForm += cnNoun1DemarcatorLength;
    } else if (strncmp(puszMatchedForm, cszNoun2Demarcator, cnNoun2DemarcatorLength) == 0) {
        dwExtractionFlags |= 1 << GFE_USE_NOUN_2_BIT;
        puszMatchedForm += cnNoun2DemarcatorLength;
    }

    size_t nLength = strlen(puszMatchedForm);
    for (size_t i = 0; i < nLength; i++) {
        CHAR uChar = puszMatchedForm[i];
        if (uChar == cGrammaticalFormStartChar) {
            break;
        }
        sNewResult += CRefString().Format("%c", uChar);
    }

    usResult = sNewResult;
    return TRUE;
}

void LanguageRules::HandleConcatenation(CRefString& sPrefix, CRefString& sBase) {
    if (sPrefix.Length() > 0) {
        sBase = sPrefix + UnicodeText::Uncapitalize(sBase);
        sPrefix = "";
    }
}

BOOL LanguageRules::ItemNamesShouldBeCapitalized() {
    XLocale eXLocale = UnicodeText::GetStringDisplayLocale();

    switch (eXLocale) {
    case XLOCALE_SPANISH_EU:
    case XLOCALE_SPANISH_LATAM:
    case XLOCALE_FRENCH:
    case XLOCALE_ITALIAN:
    case XLOCALE_GERMAN:
    case XLOCALE_PORTUGUESE_BRAZIL:
    case XLOCALE_RUSSIAN:
    case XLOCALE_POLISH:
    case XLOCALE_PORTUGUESE_PORTUGAL:
        return TRUE;
    default:
        return FALSE;
    }
}

BOOL LanguageRules::PreferShorterMagicNames() {
    XLocale eXLocale = UnicodeText::GetStringDisplayLocale();

    switch (eXLocale) {
    case XLOCALE_FRENCH:
    case XLOCALE_GERMAN:
    case XLOCALE_RUSSIAN:
        return TRUE;
    default:
        return FALSE;
    }
}

typedef XArray<CRefString, const CRefString&, 3> StringConcatArray;

static void sAddString(StringConcatArray& tStringList, const CRefString& sStr) {
    if (sStr.Length() > 0) {
        tStringList.Add_Tracked(sStr);
    }
}

CRefString LanguageRules::BuildItemName(const CRefString& sPrefix, const CRefString& sItem, const CRefString& sSuffix,
                                        const ItemNameContext eContext, const BOOL bPreventCapitalization) {
    StringConcatArray tStringList;
    BOOL fSupportsSpaces = TRUE;
    XLocale eXLocale = UnicodeText::GetStringDisplayLocale();
    int32 nCount;

    switch (eXLocale) {
    case XLOCALE_SPANISH_EU:
    case XLOCALE_SPANISH_LATAM:
    case XLOCALE_FRENCH:
    case XLOCALE_PORTUGUESE_BRAZIL:
    case XLOCALE_PORTUGUESE_PORTUGAL:
        sAddString(tStringList, sItem);
        sAddString(tStringList, sPrefix);
        sAddString(tStringList, sSuffix);
        break;

    case XLOCALE_ITALIAN:
        if (eContext == INC_RARE_ITEM) {
            sAddString(tStringList, sSuffix);
            sAddString(tStringList, sItem);
            sAddString(tStringList, sPrefix);
        } else {
            sAddString(tStringList, sItem);
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sSuffix);
        }
        break;

    case XLOCALE_JAPANESE:
        fSupportsSpaces = FALSE;
        if (eContext == INC_RARE_ITEM) {
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sItem);
            sAddString(tStringList, sSuffix);
        } else {
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sSuffix);
            sAddString(tStringList, sItem);
        }
        break;

    case XLOCALE_ENGLISH_US:
    case XLOCALE_ENGLISH_EU:
    case XLOCALE_ENGLISH_SOUTHEAST_ASIA:
    case XLOCALE_GERMAN:
        sAddString(tStringList, sPrefix);
        sAddString(tStringList, sItem);
        sAddString(tStringList, sSuffix);
        break;

    case XLOCALE_TURKISH:
        sAddString(tStringList, sPrefix);
        sAddString(tStringList, sSuffix);
        sAddString(tStringList, sItem);
        break;

    case XLOCALE_RUSSIAN:
    case XLOCALE_POLISH:
        if (eContext == INC_LOOT_RUN) {
            sAddString(tStringList, sItem);
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sSuffix);
        } else {
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sItem);
            sAddString(tStringList, sSuffix);
        }
        break;

    case XLOCALE_KOREAN:
        if (eContext == INC_RARE_ITEM) {
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sItem);
            sAddString(tStringList, sSuffix);
        } else {
            sAddString(tStringList, sSuffix);
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sItem);
        }
        break;

    case XLOCALE_CHINESE_TAIWAN:
        fSupportsSpaces = FALSE;
        sAddString(tStringList, sSuffix);
        sAddString(tStringList, sPrefix);
        sAddString(tStringList, sItem);
        break;

    case XLOCALE_CHINESE_CHINA:
        fSupportsSpaces = FALSE;
        if (eContext == INC_RARE_ITEM) {
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sItem);
            sAddString(tStringList, sSuffix);
        } else {
            sAddString(tStringList, sSuffix);
            sAddString(tStringList, sPrefix);
            sAddString(tStringList, sItem);
        }
        break;

    default:
        DisplayInternalError(NULL, "", 1586, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    nCount = tStringList.GetArraySize();
    if (nCount <= 0) {
        return CRefString("");
    }

    if (ItemNamesShouldBeCapitalized() && !bPreventCapitalization) {
        tStringList[0] = UnicodeText::Capitalize(tStringList[0]);
    }

    CRefString sReturn;
    for (int32 i = 0; i < nCount; i++) {
        sReturn += tStringList[i];
        if (fSupportsSpaces && i < nCount - 1) {
            sReturn += " ";
        }
    }

    return sReturn;
}
