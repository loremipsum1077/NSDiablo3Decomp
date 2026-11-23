// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/settings/settings_Language.h

#pragma once

namespace nn {
namespace settings {
enum Language {
    Language_Japanese = 0,
    Language_AmericanEnglish = 1,
    Language_French = 2,
    Language_German = 3,
    Language_Italian = 4,
    Language_Spanish = 5,
    Language_Chinese = 6,
    Language_Korean = 7,
    Language_Dutch = 8,
    Language_Portuguese = 9,
    Language_Russian = 10,
    Language_Taiwanese = 11,
    Language_BritishEnglish = 12,
    Language_CanadianFrench = 13,
    Language_LatinAmericanSpanish = 14,
    Language_SimplifiedChinese = 15,
    Language_TraditionalChinese = 16,
};

struct LanguageCode {  // size 0x8
    char string[8];  // 0x00
    static nn::settings::LanguageCode Make(nn::settings::Language);
};
static_assert(sizeof(LanguageCode) == 0x8, "LanguageCode");

}  // namespace settings
}  // namespace nn
