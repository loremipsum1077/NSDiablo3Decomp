// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Contacts/dmContact.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

struct dmDistanceOutput;
struct dmManifold;
struct dmMutex;
struct dmSweep;
struct dmTOIOutput;
struct dmWorld;

enum dmContactType {
    dm_convexContact = 0,
    dm_meshContact = 1,
};

struct dmContact {  // size 0x58
    struct Edge {  // size 0x10
        dm_int32 m_fixtureIndex;  // 0x00
        dm_int32 m_ownerIndex;  // 0x04
        dm_int32 m_prevKey;  // 0x08
        dm_int32 m_nextKey;  // 0x0c
    };
    bool Create(dmWorld * pWorld, dm_int32 fixtureIndexA, dm_int32 fixtureIndexB);
    void Destroy(dmWorld * pWorld);
    void UpdateManifolds(dmWorld * pWorld, dmMutex * pMutex, bool isTOIEvent);
    dm_vec4 ShapeCast(dm_int32 *, dmWorld *, const dmTransform &, const dmTransform &, dm_vec4Arg);
    void TimeOfImpact(dmTOIOutput * pOutput, dmWorld * pWorld, const dmSweep & sweepA, const dmSweep & sweepB, dmMutex * pMutex);
    void Distance(dmDistanceOutput *, dm_int32);
    union {  // size 0x4
        dm_int32 m_index;  // 0x00
        dm_int32 m_next;  // 0x00
    };  // 0x00
    dmContact::Edge m_edges[2];  // 0x04
    dmManifold * m_aManifolds;  // 0x28
    dm_int32 m_manifoldCount;  // 0x30
    dm_int32 m_subIndex;  // 0x34
    dm_float32 m_friction;  // 0x38
    dm_float32 m_restitution;  // 0x3c
    dm_float32 m_rollingResistance;  // 0x40
    dm_int32 m_islandIndexA;  // 0x44
    dm_int32 m_islandIndexB;  // 0x48
    dm_uint32 m_islandStamp;  // 0x4c
    dmContactType m_type;  // 0x50
    dm_uint16 m_revision;  // 0x54
    bool m_isSolid : 1;  // 0x56
    bool m_isTouching : 1;  // 0x56
    bool m_toiValid : 1;  // 0x56
};
static_assert(sizeof(dmContact) == 0x58, "dmContact");

