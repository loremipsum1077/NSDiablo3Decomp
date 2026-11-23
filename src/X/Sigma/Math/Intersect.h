#pragma once

#include "../SigmaTypes.h"
#include "Cylinder.h"
#include "Frustum.h"
#include "Plane.h"
#include "Ray.h"
#include "Rect.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Vector.h"

namespace Intersect {
enum DetailedFrustumResult {
    INTERSECT_INSIDE = 0,
    INTERSECT_OUTSIDE = 1,
    INTERSECT_PARTIALLY = 2,
};

BOOL Ray2DAndCircle(const Ray2D& wr, const Circle& wc, float& wdT);
BOOL Ray2DAndRect2D(const Ray2D& wr, const Rect2D& wrect, float& wdT, const float wdTMaxInitial);
BOOL LineSeg2DAndRect2D(const LineSeg2D& wline, const Rect2D& wb, float& wdT, Vector2D* pwpIntersect);
BOOL Ray3DAndSphere(const Ray3D& wr, const Sphere& ws, float& wdT);
BOOL Ray3DAndTriangle3D(const Ray3D& wr, const Triangle3D& wtri, float& wdT);
BOOL Ray3DAndTriangle3DCull(const Ray3D& wr, const Triangle3D& wtri, float& wdT);
BOOL Ray3DAndTriangle3D(const Ray3D& wr, const Triangle3D& wtri, float& flU, float& flV);
BOOL Ray3DAndAABB(const Ray3D& wr, const AABB& aabb, float& wdT, const float wdTMaxInitial);
BOOL LineSeg2DAndCircle(const LineSeg2D& wline, const Circle& wc, float& wdT, Vector2D* pwpIntersect);
BOOL LineSeg3DAndSphere(const LineSeg3D& wline, const Sphere& ws, float& wdT, Vector3D* pwpIntersect);
BOOL LineSeg3DAndTriangle3D(const LineSeg3D& wline, const Triangle3D& wtri, float& wdT, Vector3D* pwpIntersect);
BOOL LineSeg3DAndTriangle3DCull(const LineSeg3D& wline, const Triangle3D& wtri, float& wdT,
                                Vector3D* pwpIntersect);
BOOL LineSeg3DAndAABB(const LineSeg3D& wline, const AABB& wb, float& wdT);
BOOL LineSeg2DAndOBB(const LineSeg2D& wline, const OBB& wb, float& wdT, Vector2D* pwpIntersect);
BOOL Ray3DAndAxialCylinder(const Ray3D& wr, const AxialCylinder& wcyl, float& wdT);
BOOL LineSeg3DAndAxialCylinder(const LineSeg3D& wlineseg, const AxialCylinder& wcyl, float& wdT);
BOOL Ray3DAndPlane(const Ray3D& wr, const Plane& wplane, Vector3D& wpIntersection);
BOOL Ray3DAndPlane(const Ray3D& wr, const Plane& wplane, Vector3D& wpIntersection, float& flT);
BOOL LineSeg3DAndPlane(const LineSeg3D& wline, const Plane& wplane, Vector3D& wpIntersection);
BOOL CapsuleAndSphere(const Capsule& capsule, const Sphere& ws);
BOOL CapsuleAndAxialCylinder(const Capsule& capsule, const AxialCylinder& wcyl, float& wdT);
BOOL AxialCylinderAndSphere(const AxialCylinder& wcyl, const Sphere& ws);
BOOL AxialCylinderAndAxialCylinder(const AxialCylinder& wcylA, const AxialCylinder& wcylB);
BOOL FrustumAndAABB(const Frustum& wf, const AABB& aabb);
BOOL FrustumAndSphere(const Frustum& frustum, const Sphere& ws);
DetailedFrustumResult DetailedFrustumAndSphere(const Frustum& frustum, const Sphere& ws);
BOOL AABBAndSphere(const AABB& aabb, const Sphere& ws);
BOOL SphereAndOBB(const Sphere& ws, const OBB& obb, Vector3D* pwpClosest);

// line 266
inline BOOL SphereAndSphere(const Sphere& wsA, const Sphere& wsB) {
    float wdR = wsA.wdRadius + wsB.wdRadius;
    return Vector3D::DistanceSquared(wsA.wpCenter, wsB.wpCenter) <= wdR * wdR;
}
}  // namespace Intersect
