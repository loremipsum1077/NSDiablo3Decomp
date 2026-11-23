#include "UIParagonCommon.h"

#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
typedef int32 ToolTipValueColoring;  // X/Common/UserInterface/UIToolTips.h
typedef BOOL (*TagResolutionCallback)(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR, float &, float *, CRefString &, SNO, int32, void *, void *, FontSpecifier &);  // X/Common/UserInterface/UIToolTips.h
CRefString UIToolTips_ProcessTip(const ActorCommonData * ptACDItem, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const SNO snoPower, const int32 nLevel, LPCSTR szTip, const ToolTipValueColoring eColoringType, TagResolutionCallback pTagCallback, void * pTagData0, void * pTagData1);  // X/Common/UserInterface/UIToolTips.cpp
ACDID LocalPlayerACDGet();  // X/Client/CPlayer.cpp
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);  // X/Common/Attributes/FastAttribute.cpp
struct RNG;
float AffixGetFormulaValue(uint8 * pbAttributeFormula, int32 nAttributeFormulaSize, const ActorCommonData * ptACDOwner, GBID gbidItem, RNG * ptRNGOverride, uint32 uFlags, int32 nLegendaryItemLevelOverride);  // X/Common/Definitions/GameBalanceDefinition.cpp

namespace UIParagonBonuses {

// UIParagonCommon.cpp:23 -- the tooltip engine's `{Tag}` callback for a paragon bonus. Both tag
// payloads arrive smuggled through `void *`: `pData1` is the bonus `GBID` and `pData2` the number
// of points spent, which is what the formula's value is multiplied by.
BOOL UIParagonBonuses::ResolveValueTagForParagonBonusTooltip(
    const ActorCommonData * ptACD, AttribGroupID idFastAttribMin, AttribGroupID idFastAttribMax,
    LPCSTR szTag, float & flResultMin, float * pflResultMax, CRefString & rusText, SNO snoParam,
    int32 nParam1, void * pData1, void * pData2, FontSpecifier & eFont )
{
    GBID gbidParagonBonus = (GBID)(intptr_t)pData1;
    int32 nLevel = (int32)(intptr_t)pData2;

    // The lock keeps the record alive across the attribute read and the formula evaluation; the
    // result is never dereferenced.
    ScopedParagonBonusLockR pdefParagonBonus( gbidParagonBonus );

    FastAttribKey tKey = MakeAttribKey( pdefParagonBonus->ptAttributes[0].eAttribute,
                                        pdefParagonBonus->ptAttributes[0].nParam );

    float flAttributeValue = AffixGetFormulaValue( pdefParagonBonus->ptAttributes[0].pbAttributeFormula,
                                                   pdefParagonBonus->ptAttributes[0].serAttributeFormula.dwSize,
                                                   ACDTryToGet( LocalPlayerACDGet() ), -1, NULL, 0, 0 );

    flResultMin = flAttributeValue * (float)nLevel;

    return TRUE;
}

// UIParagonCommon.cpp:55 -- the "what one point in this bonus gives you" line. The bonus's own
// authored name is the suffix of the string-list label (`ParagonBonusValue_<szName>`), the fetched
// entry is run through the tooltip engine with the resolver above, and the result is dropped into
// the screen's own wrapper string.
CRefString UIParagonBonuses::GetStatContribution( const GBID gbidParagonBonus )
{
    ScopedParagonBonusLockR pdefParagonBonus( gbidParagonBonus );

    CRefString sValue = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_SKILLS_UI ),
                                         CRefString().Format( "ParagonBonusValue_%s",
                                                              (LPCSTR)pdefParagonBonus->tHeader.szName ) );

    CRefString sTip = UIToolTips_ProcessTip( NULL, -1, -1, -1, 0, sValue, 0,
                                             ResolveValueTagForParagonBonusTooltip,
                                             (void *)(intptr_t)gbidParagonBonus, (void *)1 );

    CRefString sResult = StringListFetch( "X1_ConsoleParagon:ParagonBonusTooltip" );

    return LocalizedSprintf( sResult, sTip.str(), NULL );
}

}  // namespace UIParagonBonuses
