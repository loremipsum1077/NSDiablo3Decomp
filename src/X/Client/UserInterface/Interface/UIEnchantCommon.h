// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIEnchantCommon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

typedef int32 PlayerClass;
struct EnchantAffixChoice;

namespace UIEnchant {

struct AffixInfo {  // size 0x20
    GBID gbidAffix;  // 0x00
    BOOL bIsInherentAffix;  // 0x04
    CRefString sAffixTooltipText;  // 0x08

    // Line 24. Inlined only into the enchant view's constructor; body not written yet.
    AffixInfo();




    AffixInfo(GBID gbidAffix, BOOL bIsInherentAffix, CRefString & sAffixTooltipText)
        : gbidAffix(gbidAffix), bIsInherentAffix(bIsInherentAffix), sAffixTooltipText(sAffixTooltipText) {}
};
static_assert(sizeof(AffixInfo) == 0x20, "AffixInfo");

typedef blz::vector<UIEnchant::AffixInfo, blz::allocator<UIEnchant::AffixInfo> > AffixInfoArray;

typedef blz::vector<CRefString, blz::allocator<CRefString> > ReplacementAffixInfoArray;

void EnumItemAffixes(ACDID idACDItem, PlayerClass ePlayerClass, AffixInfoArray & arAffixes);
void EnumReplacementItemAffixes(ACDID idACDItem, PlayerClass ePlayerClass, GBID gbidSelectedAffix,
                                const EnchantAffixChoice * arEnchantAffixChoices, int32 nEnchantAffixChoicesCount,
                                AffixInfoArray & arAffixes, int32 & nOutOriginalAffixIndex);
void EnumPossibleReplacementAffixes(ACDID idACDItem, PlayerClass ePlayerClass, int32 nAffixLevelOverride,
                                    uint32 dwOverrideFlags, GBID gbidAffixToReplace,
                                    ReplacementAffixInfoArray & arReplacementAffixes);

}  // namespace UIEnchant
