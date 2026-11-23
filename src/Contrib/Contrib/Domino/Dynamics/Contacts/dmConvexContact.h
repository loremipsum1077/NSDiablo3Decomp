// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Contacts/dmConvexContact.h

#pragma once

#include "../../Collision/Functions/dmDistance.h"
#include "../../Collision/Functions/dmManifold.h"
#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include "../../Include/dmShapes.h"

struct dmContact;
struct dmManifold;
struct dmMutex;
struct dmSweep;
struct dmTOIOutput;
struct dmWorld;

struct dmConvexContact {  // size 0x10
    void Create(dmShape::Type typeA, dmShape::Type typeB);
    static bool ComputeManifold(const dmManifold &, dmManifold *, dmGJKCache *, dmSATCache *, dmShape *, dmShape *, const dmTransform &, const dmTransform &, dmShape::Type, dmShape::Type, bool);
    bool UpdateManifold(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex, bool isTOIEvent);
    dm_vec4 ShapeCast(dmWorld *, const dmContact *, const dmTransform &, const dmTransform &, dm_vec4Arg);
    void TimeOfImpact(dmTOIOutput * pOutput, dmWorld * pWorld, const dmContact * pBase, const dmSweep & sweepA, const dmSweep & sweepB);
    void Distance(dmDistanceOutput *, dmWorld *, const dmContact *);
    void ClearBaseManifold(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex);
    union {  // size 0x4
        dm_int32 m_index;  // 0x00
        dm_int32 m_next;  // 0x00
    };  // 0x00
    union {  // size 0xc
        dmGJKCache m_gjkCache;  // 0x00
        dmSATCache m_satCache;  // 0x00
    };  // 0x04
};
static_assert(sizeof(dmConvexContact) == 0x10, "dmConvexContact");

