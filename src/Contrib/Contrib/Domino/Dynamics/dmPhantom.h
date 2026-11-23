// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/dmPhantom.h

#pragma once

#include "../Include/dmCallbacks.h"
#include "../Include/dmMath.h"
#include "../Include/dmSerialize.h"

struct dmFixtureDef;
struct dmPhantomDef;
struct dmWorld;
struct dmWorldRayCastInput;

struct dmPhantom {  // size 0x50
    void Create(const dmPhantomDef *);
    void Destroy(dmWorld *);
    dm_int32 CreateFixture(dmWorld * pWorld, const dmFixtureDef * pDef);
    void DestroyFixture(dmWorld * pWorld, dm_int32 fixtureIndex);
    void SetTransform(dmWorld * pWorld, const dmTransform & transform);
    void SetActive(dmWorld *, bool);
    dm_vec4 Move(dmWorld *, dm_vec4Arg);
    void RayCast(dmWorld *, dmRayCastPointCollector *, const dmWorldRayCastInput *);
    void ComputeManifolds(dmWorld * pWorld);
    dm_vec4 m_quaternion;  // 0x00
    dm_vec4 m_position;  // 0x10
    union {  // size 0x4
        dm_int32 m_next;  // 0x00
        dm_int32 m_index;  // 0x00
    };  // 0x20
    dm_int32 m_fixtureList;  // 0x24
    void * m_pUserData;  // 0x28
    dm_uint16 m_revision;  // 0x30
    bool m_isActive;  // 0x32
    char m_name[16];  // 0x33
};
static_assert(sizeof(dmPhantom) == 0x50, "dmPhantom");

