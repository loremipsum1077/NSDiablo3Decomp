// `X/Client/UserInterface/Interface/UIParagonCommon.h` -- the two paragon helpers the PC and
// console paragon screens share: the `{Value}` tag resolver the tooltip engine calls back for a
// paragon bonus, and the localised line the paragon screen shows for one point spent.
//
// `UIParagonBonuses` is a namespace, not a class -- both symbols mangle `_ZN16UIParagonBonuses...`
// and take no `this`.
#pragma once

#include "../../../Common/GameBalance.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

class ActorCommonData;

typedef int32 AttribGroupID;  // X/Common/UserInterface/UIToolTips.h
typedef int32 FontSpecifier;  // X/Common/UserInterface/UIToolTips.h

namespace UIParagonBonuses {

// UIParagonCommon.cpp:23 -- the tooltip engine's `{Tag}` callback for a paragon bonus. Both tag
// payloads arrive smuggled through `void *`: `pData1` is the bonus `GBID` and `pData2` the number
// of points spent, which is what the formula's value is multiplied by.
BOOL ResolveValueTagForParagonBonusTooltip(const ActorCommonData * ptACD, AttribGroupID idFastAttribMin,
                                           AttribGroupID idFastAttribMax, LPCSTR szTag, float & flResultMin,
                                           float * pflResultMax, CRefString & rusText, SNO snoParam,
                                           int32 nLevel, void * pData1, void * pData2, FontSpecifier & eFont);

// UIParagonCommon.cpp:55 -- the localised "+N to X" line for one paragon bonus, one point spent.
CRefString GetStatContribution(const GBID gbidParagonBonus);

}  // namespace UIParagonBonuses
