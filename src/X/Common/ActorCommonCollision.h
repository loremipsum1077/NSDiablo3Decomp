// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Common/ActorCommonCollision.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "../Sigma/Math/Vector.h"

struct AxialCylinder;
struct OBB;

struct ACDCollSettings {  // size 0x38
    uint16 uStaticOneRequiredFlags;  // 0x00
    uint16 uStaticPreventFlags;  // 0x02
    uint8 eTeam;  // 0x04
    uint8 eCollisionShape;  // 0x05
    uint16 uPad16;  // 0x06
    uint32 uDynamicFlagsNoOverlap;  // 0x08
    uint32 uDynamicMyFlags;  // 0x0c
    union {  // size 0x28
        uint8 arAxialCylinderBytes[20];  // 0x00
        uint8 arOBBBytes[40];  // 0x00
    };  // 0x10
    AxialCylinder & GetCyl();
    OBB & GetOBB();
    const AxialCylinder & GetCyl() const;
    const OBB & GetOBB() const;
};
static_assert(sizeof(ACDCollSettings) == 0x38, "ACDCollSettings");

struct ACDCollInfo {  // size 0x3c
    ACDID idACD;  // 0x00
    ACDCollSettings tCollSettings;  // 0x04
};
static_assert(sizeof(ACDCollInfo) == 0x3c, "ACDCollInfo");

struct ACDCollBroadResult {  // size 0x10
    uint32 uCount;  // 0x00
    ACDCollInfo * arACDISect;  // 0x08
};
static_assert(sizeof(ACDCollBroadResult) == 0x10, "ACDCollBroadResult");

struct ACDCollNarrowResult {  // size 0x34
    int32 nIndexHit;  // 0x00
    Vector3D wpPosition;  // 0x04
    float flDistance;  // 0x10
    float flACDRadius;  // 0x14
    float flT;  // 0x18
    Vector3D wpContact;  // 0x1c
    Vector3D wvNormal;  // 0x28
};
static_assert(sizeof(ACDCollNarrowResult) == 0x34, "ACDCollNarrowResult");

// Lines 45 and 46. The bodies are not in the DWARF; written from sGetSphereForACD (ActorCommonCollision.cpp:139,146).
inline const AxialCylinder & ACDCollSettings::GetCyl() const { return *(const AxialCylinder *)arAxialCylinderBytes; }
inline const OBB & ACDCollSettings::GetOBB() const { return *(const OBB *)arOBBBytes; }

enum {
    COLL_PARAMS_FROM_POWER = 0,
    COLL_PARAMS_IGNORE_WRECKABLES = 1,
    COLL_PARAMS_IGNORE_WALLER_WALLS = 2,
    COLL_PARAMS_NUM_FLAGS = 3,
};

struct ACDCollParams {  // size 0xc
    uint32 uObjectNoOverlapFlags;  // 0x00
    uint8 eTeam;  // 0x04
    uint8 uPad1;  // 0x05
    uint8 uPad2;  // 0x06
    uint8 uPad3;  // 0x07
    uint32 uSpecialFlags;  // 0x08
    ACDCollParams();
};
static_assert(sizeof(ACDCollParams) == 0xc, "ACDCollParams");

enum {
    ACD_CNF_IGNORE_OBJECTS_IF_MOVING_AWAY = 0,
};


// Functions defined in this file (bodies are not in the DWARF):
//   line    45  const AxialCylinder & ACDCollSettings::GetCyl() const  [inlined into 6 functions]
//   line    46  const OBB & ACDCollSettings::GetOBB() const  [inlined into 6 functions]

#include "SNO/SNOFiles.h"
// Owning-header placement inferred; raw DWARF has only ActorCommonCollision.cpp:292, no header declaration.
void ACDCollGetSettings(SNO snoActor, ACDCollSettings & tCollSettings);
