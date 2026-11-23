// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/RActors/RActorCollision.h
// Only RActorExactIntersectionResult is adopted so far (HitTestResults needs it).

#pragma once
#include <float.h>
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Triangle.h"
#include <string.h>
struct RActorExactIntersectionResult {  // size 0x34
    Triangle3D wtri;  // 0x00
    WorldDistance wdT;  // 0x24
    int32 nSubobject;  // 0x28
    int32 nSegment;  // 0x2c
    int32 nIndexListIndex;  // 0x30
    // RActorCollision.h:42. wdT: the copy inlined in UIRActorControl.cpp stores FLT_MAX at RActorCollision.h:43.
    // The -1s: one 8-byte + one 4-byte store, both at 45:3 (a call column; a chained assignment sits at an '='),
    // the lowering of a 12-byte memset over nSubobject..nIndexListIndex. Exact memset expression inferred.
    RActorExactIntersectionResult() : wdT(FLT_MAX) {
        memset(&nSubobject, -1, 3 * sizeof(int32));
    }
};
static_assert(sizeof(RActorExactIntersectionResult) == 0x34, "RActorExactIntersectionResult");

void RActorsCollisionDisposeForGame();  // RActorCollision.cpp:139
