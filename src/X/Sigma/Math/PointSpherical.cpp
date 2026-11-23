#include "PointSpherical.h"

#include "Vector.h"

void PointSpherical::Set(const Vector3D& wp, const Vector3D& wpOrigin) {
    Vector3D wv = wp - wpOrigin;
    Vector3D wvXY(wv.x, wv.y, 0.0f);

    wdRadius = wv.Magnitude();
    aPitch = Math::ATan(wv.z, wvXY.Magnitude());
    aYaw = Math::ATan(wv.x, wv.y);
}
