// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIHero.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

// The scalar aliases GetKillerName's parameters are spelled with, redeclared here rather than
// pulling their headers in for three int32s.
typedef int32 GBID;           // X/Common/GameBalance.h
typedef int32 MonsterRarity;  // X/Common/ActorCommonDataSwig.h
typedef int32 SNO;            // X/Common/SNO/SNOFiles.h

enum KilledByType {
    KILLED_BY_MYSTERY = 0,
    KILLED_BY_PLAYER = 1,
    KILLED_BY_PLAYER_PET = 2,
    KILLED_BY_NORMAL = 3,
    KILLED_BY_RARE = 4,
    KILLED_BY_COUNT = 5,
};

class CRefString;

namespace UIHero {

// Defined in UIHero.cpp:1087.
KilledByType GetKillerName(int32 nKillerPlayerIndex, SNO snoActor, SNO snoPowerDmgSource,
                           MonsterRarity eMonsterRarity, const GBID * arKillerRareNameGBIDs,
                           CRefString & strKiller, CRefString & strPower);

}  // namespace UIHero
