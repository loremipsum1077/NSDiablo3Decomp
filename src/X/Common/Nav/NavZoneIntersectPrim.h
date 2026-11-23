// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavZoneIntersectPrim.h

#pragma once

#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

enum AABBCollideFeature {
    AABB_COLLIDE_NONE = 0,
    AABB_COLLIDE_XMIN = 1,
    AABB_COLLIDE_XMAX = 2,
    AABB_COLLIDE_YMIN = 3,
    AABB_COLLIDE_YMAX = 4,
    AABB_COLLIDE_RND0 = 5,
    AABB_COLLIDE_RND1 = 6,
    AABB_COLLIDE_RND2 = 7,
    AABB_COLLIDE_RND3 = 8,
    AABB_COLLIDE_VERTEX = 9,
};

inline BOOL AABB2DAndAABB2D(const Vector2D & wpMinA, const Vector2D & wpMaxA, const Vector2D & wpMinB, const Vector2D & wpMaxB) {  // line 128
    if (wpMaxB.x < wpMinA.x || wpMinB.x > wpMaxA.x)
        return FALSE;
    if (wpMaxB.y < wpMinA.y || wpMinB.y > wpMaxA.y)
        return FALSE;
    return TRUE;
}

// Body from the line table with columns (DWARF lines 151-152): one return expression over two
// lines, rows at the `&&` and member columns of
// "\treturn (wpMinA.x >= wpMinB.x && wpMaxA.x <= wpMaxB.x &&" and its aligned continuation.
inline BOOL AABB2DInsideAABB2D(const Vector2D & wpMinA, const Vector2D & wpMaxA, const Vector2D & wpMinB, const Vector2D & wpMaxB) {  // line 145
    return (wpMinA.x >= wpMinB.x && wpMaxA.x <= wpMaxB.x &&
            wpMinA.y >= wpMinB.y && wpMaxA.y <= wpMaxB.y);
}

// Body from the line table with columns (DWARF lines 179-180): one return expression over two
// lines, rows at columns 18/29/31/53/55 = the `x`/`&&` tokens of
// "\treturn (wpPoint.x >= wpMin.x && wpPoint.x <= wpMax.x &&" and its aligned continuation.
inline BOOL Point2DInsideAABB2D(const Vector2D & wpPoint, const Vector2D & wpMin, const Vector2D & wpMax) {  // line 174
    return (wpPoint.x >= wpMin.x && wpPoint.x <= wpMax.x &&
            wpPoint.y >= wpMin.y && wpPoint.y <= wpMax.y);
}

// Math::Min inlined at DWARF lines 196 and 197, Math::Max at 199 (and 200).
inline void AABBFromSweptPoint(const Vector2D & wpStart, const Vector2D & wpDest, Vector2D & tMin, Vector2D & tMax) {  // line 190
    tMin.x = Math::Min(wpStart.x, wpDest.x);  // line 196
    tMin.y = Math::Min(wpStart.y, wpDest.y);  // line 197

    tMax.x = Math::Max(wpStart.x, wpDest.x);  // line 199
    tMax.y = Math::Max(wpStart.y, wpDest.y);  // line 200
}

BOOL Point2DInsideRoundedAABB2D(const Vector2D & wpPoint, const Vector2D & wpNavCellMin, const Vector2D & wpNavCellMax, uint16 uFlags);


// Functions defined in this file (bodies are not in the DWARF):
//   line   128  BOOL AABB2DAndAABB2D(const Vector2D & wpMinA, const Vector2D & wpMaxA, const Vector2D & wpMinB, const Vector2D & wpMaxB)  [inlined into 3 functions]
//   line   145  BOOL AABB2DInsideAABB2D(const Vector2D & wpMinA, const Vector2D & wpMaxA, const Vector2D & wpMinB, const Vector2D & wpMaxB)  [inlined into 3 functions]
//   line   174  BOOL Point2DInsideAABB2D(const Vector2D & wpPoint, const Vector2D & wpMin, const Vector2D & wpMax)  [inlined into 10 functions]
//   line   190  void AABBFromSweptPoint(const Vector2D & wpStart, const Vector2D & wpDest, Vector2D & tMin, Vector2D & tMax)  [inlined into 1 functions]
//   line   222  void AABBFromSweptCircle(const Circle & wc, const Vector2D & wpDest, Vector2D & tMin, Vector2D & tMax)  [inlined into 1 functions]
