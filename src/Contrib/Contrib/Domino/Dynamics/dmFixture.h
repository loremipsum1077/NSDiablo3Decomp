// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/dmFixture.h

#pragma once

#include "../Include/dmCollision.h"
#include "../Include/dmMath.h"
#include "../Include/dmSerialize.h"
#include "../Include/dmStructures.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

struct dmBodyId;
struct dmDistanceOutput;
struct dmPhantomId;
class dmShape;
struct dmWorld;

struct dmFixture {  // size 0x80
    void Create(dmWorld * pWorld, const dmFixtureDef * pDef);
    void Destroy(dmWorld * pWorld);
    dmBodyId GetOwnerBodyId(dmWorld *) const;
    dmPhantomId GetOwnerPhantomId(dmWorld *) const;
    dmTransform GetTransform(const dmWorld * pWorld) const;
    dm_vec4 GetProjectedArea(const dmTransform & transform, dm_vec4Arg dir) const;
    void SetFilter(dmWorld *, const dmFilter &);
    void DestroyContacts(dmWorld * pWorld);
    void GetClosestPoint(dmDistanceOutput * pOutput, const dmTransform & transform, dm_vec4Arg point) const;
    void CreateProxy(dmWorld * pWorld, const dmMtx & transform);
    void DestroyProxy(dmWorld *);
    void TouchProxy(dmWorld * pWorld);
    void Synchronize(dmWorld * pWorld, const dmMtx & transform);
    void Synchronize(dmWorld * pWorld, const dmMtx & transform1, const dmMtx & transform2);
    void Dump(dm_int32, FILE *);
    void DumpMeshShape(FILE *);
    void ApplyScale(dmWorld *, const dmMtx &, dm_vec4Arg);
    dmAABB m_aabb;  // 0x00
    dmFixtureType m_type;  // 0x20
    dmShape * m_pShape;  // 0x28
    void * m_pUserData;  // 0x30
    dm_int32 m_contactListKey;  // 0x38
    dm_int32 m_contactCount;  // 0x3c
    dm_int32 m_ownerIndex;  // 0x40
    dm_int32 m_ownerNext;  // 0x44
    dm_float32 m_density;  // 0x48
    dm_float32 m_minRadius;  // 0x4c
    dm_float32 m_maxRadius;  // 0x50
    dm_float32 m_friction;  // 0x54
    dm_float32 m_restitution;  // 0x58
    dm_float32 m_rollingResistance;  // 0x5c
    dmFilter m_filter;  // 0x60
    union {  // size 0x4
        dm_int32 m_next;  // 0x00
        dm_int32 m_index;  // 0x00
    };  // 0x68
    dm_int32 m_proxyId;  // 0x6c
    dm_uint16 m_revision;  // 0x70
    bool m_isSensor;  // 0x72
    dmOwnerType m_ownerType;  // 0x74
};
static_assert(sizeof(dmFixture) == 0x80, "dmFixture");

