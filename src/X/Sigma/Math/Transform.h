#pragma once

#include "../SigmaTypes.h"
#include "Quaternion.h"
#include "Vector.h"

struct Matrix3x3;
struct Matrix4x4;
struct PRSTransform;

struct PRTransform {
    Quaternion q;
    Vector3D wp;

    PRTransform() {}
    PRTransform(const Quaternion& qIn, const Vector3D& wpIn) : q(qIn), wp(wpIn) {}

    BOOL operator==(const PRTransform& rhs);

    void Set(const Quaternion& qIn, const Vector3D& wpIn) {
        q = qIn;
        wp = wpIn;
    }
    void Set(const Vector3D& wpLookFrom, const Vector3D& wpLookAt, const Vector3D& wvUp);

    // Line 67. Written from the copies inlined into Minimap::AddQuestMarker and the other marker
    // functions (GetMarkerTransform). Only wp is used there; computing it before q is what gives
    // their load order. `inline` supplies the inlinehint: clang 5 gives an in-class body no hint
    // otherwise, and the recorded cost for this body (about 385) sits above the no-hint threshold
    // (375) but below the hinted threshold (487). No out-of-line copy of _ZNK11PRTransformmlERKS_
    // exists in data/2.6.2/functions.csv, so hinting it cannot contradict the original.
    inline PRTransform operator*(const PRTransform& rhs) const {
        PRTransform transformResult;
        transformResult.wp = q.Rotate(rhs.wp) + wp;
        transformResult.q = q * rhs.q;
        return transformResult;
    }
    // Line 76; body lines 80-81 from the copy inlined into MarkerBase::GetActorAABB.
    PRTransform& operator*=(const PRTransform& rhs) {
        wp = q.Rotate(rhs.wp) + wp;
        q = q * rhs.q;
        return *this;
    }
    Vector3D operator*(const Vector3D& v) const {
        return q.Rotate(v) + wp;
    }

    // Line 91. `wp.As2D()` (not a Vector2D temporary) is what sPaintCircleOnScene
    // (SceneCommon.cpp:646) needs: wp.y is loaded after the x sum.
    Vector2D operator*(const Vector2D& v) const {
        return q.Rotate(v) + wp.As2D();
    }

    Vector3D Rotate(const Vector3D& v) const {
        return q.Rotate(v);
    }

    Vector2D Rotate(const Vector2D& v) const {
        return q.Rotate(v);
    }

    PRTransform Inverse() const {
        PRTransform transformResult;
        transformResult.q = q.Conjugate();
        transformResult.wp = transformResult.q.Rotate(-wp);
        return transformResult;
    }

    void ToMatrix(Matrix4x4& m) const;

    // Line 267. Body below PRSTransform.
    PRTransform& operator=(const PRSTransform& transform);

    static void Lerp(const PRTransform& transform0, const PRTransform& transform1, float flT,
                     PRTransform& transformResult);
    static void SLerp(const PRTransform& transform0, const PRTransform& transform1, float flT,
                      PRTransform& transformResult);
};

struct PRSTransform {
    Quaternion q;
    Vector3D wp;
    float flScale;
    PRSTransform(const PRTransform& transform) : q(transform.q), wp(transform.wp), flScale(1.0f) {}  // Line 156: DWARF declaration only (not explicit, parameter unnamed, no inlined copy anywhere); body from the stores in Ragdoll::ComputeBone 755/782 (q and wp copied, flScale = 1.0f, overwritten at 757/784). Name `transform` as in operator= (267)
    PRSTransform() {}
    PRSTransform(const Quaternion& qIn, const Vector3D& wpIn, const float flScaleIn)
        : q(qIn), wp(wpIn), flScale(flScaleIn) {}
    // Line 160: qIn 161, wpIn 162, flScaleIn 163 (const float), q 165, wp 166
    // (LookAt::ComputeRelativeTransform, LookAt.cpp:273). flScale on 167 inferred: its store
    // has no row of its own.
    void Set(const Quaternion& qIn, const Vector3D& wpIn, const float flScaleIn) {
        q = qIn;
        wp = wpIn;
        flScale = flScaleIn;
    }
    inline PRTransform operator*(const PRTransform& rhs) const { PRTransform transformResult; transformResult.wp = q.Rotate(rhs.wp * flScale) + wp; transformResult.q = q * rhs.q; return transformResult; }  // Line 188: rhs 189, transformResult 194, wp 195, q 196 (Cloth::CacheCollisionPlanes, Ragdoll::ComputeBone 755/782); no scale. `inline` hint needed: without it Cloth::CacheCollisionPlanes calls it out of line (1104 vs 1408 bytes)
    // Line 200. Reconstructed from AttachedRagdoll's inlined composition.
    inline PRSTransform operator*(const PRSTransform& rhs) const {
        PRSTransform transformResult;
        transformResult.wp = q.Rotate(rhs.wp * flScale) + wp;
        transformResult.q = q * rhs.q;
        transformResult.flScale = flScale * rhs.flScale;
        return transformResult;
    }
    PRSTransform& operator*=(const PRSTransform& rhs) { wp = q.Rotate(rhs.wp * flScale) + wp; q = q * rhs.q; flScale *= rhs.flScale; return *this; }  // Line 213: rhs 214, wp 220, q 221 (RActorUpdateClientTransform, RActors.cpp:6096); flScale on 222 inferred from line 200
    // Line 226. Written from ProcessBoneMatrices: scale, rotate, then translate.
    inline Vector3D operator*(const Vector3D& v) const {
        return q.Rotate(v * flScale) + wp;
    }

    // Line 235. Written from Cloth::PreStep: rotation only.
    Vector3D Rotate(const Vector3D& v) const {
        return q.Rotate(v);
    }
    PRSTransform Inverse() const { PRSTransform transformResult; transformResult.flScale = 1.0f / flScale; transformResult.q = q.Conjugate(); transformResult.wp = transformResult.q.Rotate(-wp) * transformResult.flScale; return transformResult; }  // Line 242: transformResult 248, 1/flScale 249, Conjugate 250, -wp / Rotate / * 251 (Ragdoll::ComputeBone 615)
    void ToMatrix(Matrix4x4& m) const;

    static void Lerp(const PRSTransform& transform0, const PRSTransform& transform1, float flT,
                     PRSTransform& transformResult);
};

// Line 55 (the compare is on line 58). Written from CameraState::operator==: position first,
// then the rotation.
inline BOOL PRTransform::operator==(const PRTransform& rhs) {
    return wp == rhs.wp && q == rhs.q;
}

// Line 267. Written from the copy inlined into ParentData::GetTransform: wp is assigned before q.
inline PRTransform& PRTransform::operator=(const PRSTransform& transform) {
    wp = transform.wp;
    q = transform.q;
    return *this;
}

class XTypeDescriptor;

// Defined in Transform.cpp (lines 20 to 34).
extern const XTypeDescriptor * DT_PRTRANSFORM;
extern const XTypeDescriptor * DT_PRSTRANSFORM;
extern const PRTransform cgtPRTransformIdentity;
extern const PRSTransform cgtPRSTransformIdentity;
extern const float MIN_PRS_SCALE;

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(PRTransform) == 0x1c, "PRTransform");
static_assert(offsetof(PRTransform, q) == 0x0, "PRTransform");
static_assert(offsetof(PRTransform, wp) == 0x10, "PRTransform");
static_assert(sizeof(PRSTransform) == 0x20, "PRSTransform");
static_assert(offsetof(PRSTransform, q) == 0x0, "PRSTransform");
static_assert(offsetof(PRSTransform, wp) == 0x10, "PRSTransform");
static_assert(offsetof(PRSTransform, flScale) == 0x1c, "PRSTransform");
