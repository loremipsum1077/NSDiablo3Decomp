// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Physics/Buoyancy.h

#pragma once

#include "../../Sigma/Math/Plane.h"
#include "../../Sigma/Math/Vector.h"

struct WaterPlane {  // size 0x1c
    Plane tPlane;  // 0x00
    Vector3D wvVelocity;  // 0x10
};
static_assert(sizeof(WaterPlane) == 0x1c, "WaterPlane");

