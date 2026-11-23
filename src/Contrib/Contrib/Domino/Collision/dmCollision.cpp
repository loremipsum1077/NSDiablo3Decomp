#include "../Include/dmCollision.h"

dmAABB dmTransformAABB(const dmTransform & transform, const dmAABB & localAABB)
{
    dmMtx R = dmMtxFromQuat(transform.q);
    dmMtx absR;
    absR.rx = dmAbs(R.rx);
    absR.ry = dmAbs(R.ry);
    absR.rz = dmAbs(R.rz);
    dm_vec4 center = localAABB.GetCenter();
    dm_vec4 extent = localAABB.GetExtents();
    dm_vec4 transformedCenter = dmMul(transform, center);
    dm_vec4 transformedExtent = dmMul33(absR, extent);
    dmAABB aabb;
    aabb.m_lowerBound = dmSub(transformedCenter, transformedExtent);
    aabb.m_upperBound = dmAdd(transformedExtent, transformedCenter);
    return aabb;
}
