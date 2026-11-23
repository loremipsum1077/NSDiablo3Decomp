// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/WorldCommonCollision.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "Nav/NavCellHandle.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/SigmaTypes.h"

struct ACDCollSettings;

enum {
    COLLISION_CATEGORY_SCENE = 0,
    COLLISION_CATEGORY_RIGID_BODY = 1,
    COLLISION_CATEGORY_PARTIAL_RAGDOLL = 2,
    COLLISION_CATEGORY_FULL_RAGDOLL = 3,
    COLLISION_CATEGORY_BREAKABLE = 4,
    COLLISION_CATEGORY_MOUSE = 5,
    COLLISION_CATEGORY_WATER = 6,
    COLLISION_CATEGORY_FENCE = 7,
    COLLISION_CATEGORY_SEE_THRU = 8,
    COLLISION_CATEGORY_SMOKE = 9,
    COLLISION_CATEGORY_ALL = 65535,
};

struct WorldCollResult {  // size 0x38
    BOOL bCollision;  // 0x00
    float flT;  // 0x04
    Vector3D wpEnd;  // 0x08
    NavCellHandle hNavCellEnd;  // 0x14
    ACDID idACDHit;  // 0x1c
    Vector3D wpContact;  // 0x20
    Vector3D wvNormal;  // 0x2c
};
static_assert(sizeof(WorldCollResult) == 0x38, "WorldCollResult");

struct WorldCollParams {  // size 0x10
    uint16 uOneRequiredStaticFlags;  // 0x00
    uint16 uStaticPreventFlags;  // 0x02
    uint8 eTeam;  // 0x04
    uint8 uPad8;  // 0x05
    uint16 uPad16;  // 0x06
    uint32 uObjectNoOverlapFlags;  // 0x08
    uint32 uSpecialFlags;  // 0x0c
    WorldCollParams();
    WorldCollParams(const ACDCollSettings &);
};
static_assert(sizeof(WorldCollParams) == 0x10, "WorldCollParams");


// Functions defined in this file (bodies are not in the DWARF):
//   line    34  WorldCollResult & WorldCollResult::operator=(const WorldCollResult &)  [inlined into 3 functions]

#include "TeamCommon.h"
struct WorldPlace;
// Declaration placement inferred from the owning API; DWARF definition: WorldCommonCollision.cpp:241.
BOOL WorldCollSearchForLocation(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flLocationRadius, float flSearchRadius, float flSearchRadiusMax, ACDID idACDIgnore, Vector3D & wpEnd, NavCellHandle & hNavCellEnd, Team eTeam, uint16 uObjectNoPassthroughFlags, ACDID idOwner);
