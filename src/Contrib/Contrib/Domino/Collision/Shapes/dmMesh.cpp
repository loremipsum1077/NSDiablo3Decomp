#include "../../Include/dmMesh.h"
#include "../../Include/dmCollision.h"







bool dmMesh::TestOverlap(const dmSphere & sphere) const
{
    dm_vec4 r = dmV4(sphere.m_radius);
    dmAABB aabb;
    aabb.m_lowerBound = dmSub(sphere.m_p, r);
    aabb.m_upperBound = dmAdd(sphere.m_p, r);

    dmBuffer<int, 128> buffer;
    Query(&buffer, aabb);

    for (dm_int32 i = 0; i < buffer.GetCount(); ++i)
    {
        dmTriangle triangle;
        GetTriangle(&triangle, i);

        bool overlap = triangle.TestOverlap(sphere);
        if (overlap)
        {
            return true;
        }
    }

    return false;
}
