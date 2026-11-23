// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/TagMaps/AnimTags.h

#pragma once
#include "../../Sigma/Unicode.h"

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

struct DeathAnimPair {  // size 0x8
    uint32 dwDeathAnimTag;  // 0x00
    uint32 dwDeadAnimTag;  // 0x04
};
static_assert(sizeof(DeathAnimPair) == 0x8, "DeathAnimPair");

// Declared here from its definition in AnimTags.cpp:590.
void AnimTagGetStartAndEndTags(const uint32 dwAnimTag, uint32 & uAnimTagStart, uint32 & uAnimTagEnd);

// Original AnimTags.cpp public lookup functions.
uint32 AnimTagGetID(LPCSTR szAnimTag);
LPCSTR AnimTagGetName(uint32 dwAnimTag);

// External definitions in AnimTags.cpp; original DWARF variable inventory.
struct AxeTagInfo;
extern const AxeTagInfo tAnimTagInfo[];
extern const int32 gnAnimTagCount;
extern const DeathAnimPair sgtDeathAnimTable[];  // AnimTags.cpp:632
