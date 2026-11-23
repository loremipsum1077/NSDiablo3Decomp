// Adopted from tools/gen_headers.py output (build/dwarf/headers); hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmCallbacks.h
// Subset: the contact listener DominoClient.cpp derives from. The ray and shape
// cast collector typedefs are not adopted yet.

#pragma once

#include "dmId.h"
#include "dmMath.h"

struct dmListenerPoint {  // size 0x30
    dm_vec4 position;  // 0x00
    dm_float32 normalVelocity;  // 0x10
    dm_float32 slidingVelocity;  // 0x14
    dm_float32 normalImpulse;  // 0x18
    dm_int32 triangleIndex;  // 0x1c
    bool isNew : 1;  // 0x20
    bool isEdge : 1;  // 0x20
};
static_assert(sizeof(dmListenerPoint) == 0x30, "dmListenerPoint");

struct dmListenerManifold {  // size 0x110
    dmListenerPoint points[4];  // 0x00
    dm_vec4 normal;  // 0xc0
    dm_vec4 frictionPoint;  // 0xd0
    dm_vec4 frictionImpulse;  // 0xe0
    dmFixtureId fixtureIdA;  // 0xf0
    dmFixtureId fixtureIdB;  // 0xf8
    dm_int32 pointCount;  // 0x100
};
static_assert(sizeof(dmListenerManifold) == 0x110, "dmListenerManifold");

class dmContactListener {  // size 0x8
public:
    virtual ~dmContactListener() {}  // line 78
    virtual void Report(const dmListenerManifold * pManifold) = 0;  // vtable slot 2
};
static_assert(sizeof(dmContactListener) == 0x8, "dmContactListener");
