// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmId.h

#pragma once

#include "dmSerialize.h"

struct dmWorldId {  // size 0x2
    dm_uint16 index;  // 0x00
};
static_assert(sizeof(dmWorldId) == 0x2, "dmWorldId");

struct dmBodyId {  // size 0x8
    dm_int32 index;  // 0x00
    dm_uint16 revision;  // 0x04
    dm_uint16 worldIndex;  // 0x06
};
static_assert(sizeof(dmBodyId) == 0x8, "dmBodyId");

struct dmPhantomId {  // size 0x8
    dm_int32 index;  // 0x00
    dm_uint16 revision;  // 0x04
    dm_uint16 worldIndex;  // 0x06
};
static_assert(sizeof(dmPhantomId) == 0x8, "dmPhantomId");

struct dmFixtureId {  // size 0x8
    dm_int32 index;  // 0x00
    dm_uint16 revision;  // 0x04
    dm_uint16 worldIndex;  // 0x06
};
static_assert(sizeof(dmFixtureId) == 0x8, "dmFixtureId");

struct dmJointId {  // size 0x8
    dm_int32 index;  // 0x00
    dm_uint16 revision;  // 0x04
    dm_uint16 worldIndex;  // 0x06
};
static_assert(sizeof(dmJointId) == 0x8, "dmJointId");

struct dmContactId {  // size 0x8
    dm_int32 index;  // 0x00
    dm_uint16 revision;  // 0x04
    dm_uint16 worldIndex;  // 0x06
};
static_assert(sizeof(dmContactId) == 0x8, "dmContactId");


// Defined in Dynamics/dmWorld.cpp, Dynamics/dmBody.cpp and Dynamics/Joints/dmJoint.cpp.
extern const dmWorldId dm_nullWorldId;
extern const dmBodyId dm_nullBodyId;
extern const dmJointId dm_nullJointId;

bool operator!=(dmWorldId id1, dmWorldId id2);  // Dynamics/dmWorld.cpp:44
