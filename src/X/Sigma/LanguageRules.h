#pragma once

#include "RefString.h"
#include "SigmaTypes.h"
#include "Unicode.h"

enum {
    GF_FEMININE_BIT = 0,
    GF_MASCULINE_BIT = 1,
    GF_NEUTER_BIT = 2,
    GF_SINGULAR_BIT = 3,
    GF_PLURAL_BIT = 4,
    GF_AH_TITLE_BIT = 5,
    GF_CONCATENATE_BIT = 6,
    NUM_GRAMMATICAL_FORM_BITS = 7,
};

enum {
    GFE_CAN_CONCATENATE_BIT = 0,
    GFE_USE_NOUN_2_BIT = 1,
};

enum ItemNameContext {
    INC_ITEM_TYPE = 0,
    INC_MAGIC_ITEM = 1,
    INC_RARE_ITEM = 2,
    INC_LOOT_RUN = 3,
};

namespace LanguageRules {
void InitializeDeclensions(const char* pData);
void DisposeDeclensions();
void SetClientGender(const BOOL fMale);
const char* ApplyLanguageRules(const char* szUTF8Str);
void RemoveLanguageTokens(LPSTR puzString);
BOOL ExtractGrammaticalForm(const CRefString& sInput, CRefString& sNoun1, CRefString& sNoun2, uint32& dwGrammaticalForm);
BOOL ExtractMatchingGrammaticalForm(CRefString& usResult, const uint32 dwGrammaticalForm, uint32& dwExtractionFlags);
void HandleConcatenation(CRefString& sPrefix, CRefString& sBase);
BOOL ItemNamesShouldBeCapitalized();
BOOL PreferShorterMagicNames();
CRefString BuildItemName(const CRefString& sPrefix, const CRefString& sItem, const CRefString& sSuffix,
                         const ItemNameContext eContext, const BOOL bPreventCapitalization);
}  // namespace LanguageRules
