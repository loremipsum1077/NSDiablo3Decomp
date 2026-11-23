// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Actors/SComplexEffect.h
// Subset: SComplexEffect and SComplexEffectGet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

typedef int32 SComplexEffectID;

typedef int32 ComplexEffectType;

struct SComplexEffect {  // size 0x20
    SComplexEffectID id;  // 0x00
    SNO snoEffectGroup;  // 0x04
    ComplexEffectType eType;  // 0x08
    ACDID idACDSource;  // 0x0c
    ACDID idACDDest;  // 0x10
    uint32 dwParam1;  // 0x14
    uint32 dwParam2;  // 0x18
    uint32 bIgnoreOwnerAlpha;  // 0x1c
};
static_assert(sizeof(SComplexEffect) == 0x20, "SComplexEffect");

BOOL SComplexEffectGet(const ACDID idACD, const SComplexEffectID idSComplexEffect, SComplexEffect & tSComplexEffect);  // SComplexEffect.cpp:571

class PlayerList;
// Declaration placement inferred from the owning API; DWARF definition: SComplexEffect.cpp:126.
void SComplexEffectSendAllSources(const PlayerList & tListPlayers, const ACDID idACD);
