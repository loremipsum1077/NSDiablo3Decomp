#include "Matrix.h"

// Line 17 (compares on lines 21 to 24). Written from CameraState::operator==: the real part first.
inline BOOL Quaternion::operator==(const Quaternion& q) const {
    return wdReal == q.wdReal && wvImag.x == q.wvImag.x && wvImag.y == q.wvImag.y &&
           wvImag.z == q.wvImag.z;
}

// Line 78 (products on lines 83 to 87). The third product of line 83 is written `q.wdReal * wvImag.x`:
// ShakeUpdate (X/Client/Shake.cpp) has q.wdReal as the first fmul operand there, and the other
// matched callers do not tell the two orders apart.
inline Quaternion Quaternion::operator*(const Quaternion& q) const {
    Quaternion qResult;
    qResult.wvImag.x = wvImag.y * q.wvImag.z - wvImag.z * q.wvImag.y + q.wdReal * wvImag.x + wdReal * q.wvImag.x;
    qResult.wvImag.y = wvImag.z * q.wvImag.x - wvImag.x * q.wvImag.z + wvImag.y * q.wdReal + wdReal * q.wvImag.y;
    qResult.wvImag.z = wvImag.x * q.wvImag.y - wvImag.y * q.wvImag.x + wvImag.z * q.wdReal + wdReal * q.wvImag.z;
    qResult.wdReal = wdReal * q.wdReal - wvImag.x * q.wvImag.x - wvImag.y * q.wvImag.y - wvImag.z * q.wvImag.z;
    return qResult;
}

// Line 94. Body from the two copies inlined into ConsoleViewController::UpdateComponentStack (lines 115-118 are the four
// products, 120 the assignment). The DWARF records one local, `const float & i1`; the names used here are not from the DWARF.
inline Quaternion& Quaternion::operator*=(const Quaternion& q) {
    float flReal = wdReal * q.wdReal - wvImag.x * q.wvImag.x - wvImag.y * q.wvImag.y - wvImag.z * q.wvImag.z;
    float flX = wdReal * q.wvImag.x + wvImag.x * q.wdReal + wvImag.y * q.wvImag.z - wvImag.z * q.wvImag.y;
    float flY = wdReal * q.wvImag.y + wvImag.y * q.wdReal + wvImag.z * q.wvImag.x - wvImag.x * q.wvImag.z;
    float flZ = wdReal * q.wvImag.z + wvImag.z * q.wdReal + wvImag.x * q.wvImag.y - wvImag.y * q.wvImag.x;
    *this = Quaternion(Vector3D(flX, flY, flZ), flReal);
    return *this;
}
// Line 144: r, q2 (150), q2 rows 153-155, q2w (158), r rows 161-163, return r (CameraRebuild line table).
inline Vector3D Quaternion::Rotate(const Vector3D& p) const {
    Vector3D r, q2;
    q2.x = wvImag.y * p.z - wvImag.z * p.y + wdReal * p.x;
    q2.y = wvImag.z * p.x - wvImag.x * p.z + wdReal * p.y;
    q2.z = wvImag.x * p.y - wvImag.y * p.x + wdReal * p.z;
    float q2w = wvImag.x * p.x + wvImag.y * p.y + wvImag.z * p.z;

    r.x = wvImag.y * q2.z - wvImag.z * q2.y + wvImag.x * q2w + wdReal * q2.x;
    r.y = wvImag.z * q2.x - wvImag.x * q2.z + wvImag.y * q2w + wdReal * q2.y;
    r.z = wvImag.x * q2.y - wvImag.y * q2.x + wvImag.z * q2w + wdReal * q2.z;
    return r;
}

inline Vector2D Quaternion::Rotate(const Vector2D& p) const {
    Vector2D r;
    r.x = -wvImag.z * p.y + wdReal * p.x;
    r.y = wvImag.z * p.x + wdReal * p.y;
    float q2w = wvImag.x * p.x + wvImag.y * p.y;

    return Vector2D(-wvImag.z * r.y + wvImag.x * q2w + wdReal * r.x,
                    wvImag.z * r.x + wvImag.y * q2w + wdReal * r.y);
}

inline void Quaternion::Set(const Vector3D& cwvImag, const float cwdReal) {
    wvImag = cwvImag;
    wdReal = cwdReal;
}

inline float Quaternion::DotProduct(const Quaternion& q, const Quaternion& r) {
    return q.wvImag.x * r.wvImag.x + q.wvImag.y * r.wvImag.y + q.wvImag.z * r.wvImag.z + q.wdReal * r.wdReal;
}

inline void Quaternion::Normalize() {
    float flNorm = Math::Sqrt(wvImag.x * wvImag.x + wvImag.y * wvImag.y + wvImag.z * wvImag.z + wdReal * wdReal);
    float flInvNorm = 1.0f / flNorm;
    wvImag *= flInvNorm;
    wdReal *= flInvNorm;
}

inline void Quaternion::SetRotation(const Vector3D& cwvAxis, const Angle caRot) {
    Angle aHalfAngle = caRot * 0.5f;
    wvImag = cwvAxis * Math::Sin(aHalfAngle);
    wdReal = Math::Cos(aHalfAngle);
}

inline Quaternion Quaternion::Conjugate() const {
    return Quaternion(-wvImag, wdReal);
}

inline void Quaternion::FromChangeOfBasis(const Vector3D& wvSrcX, const Vector3D& wvSrcY, const Vector3D& wvSrcZ,
                                          const Vector3D& wvDestX, const Vector3D& wvDestY,
                                          const Vector3D& wvDestZ) {
    Matrix3x3 m;
    m.SetChangeBasis(wvSrcX, wvSrcY, wvSrcZ, wvDestX, wvDestY, wvDestZ);
    FromMatrix(m);
}
inline void Quaternion::FromBasisVectors(const Vector3D& wv0, const Vector3D& wv1, const Vector3D& wv2) { Matrix3x3 m; m.SetBasisVectors(wv0, wv1, wv2); FromMatrix(m); }  // line 274; m at 279, SetBasisVectors at 280, FromMatrix at 281 (CameraRebuild)
inline void Quaternion::FromArc(const Vector3D& wv0, const Vector3D& wv1) {
    const float QUATERNION_ARC_EPSILON = 0.001f;

    Vector3D wvNorm0(wv0);
    Vector3D wvNorm1(wv1);

    float wdMag0 = wvNorm0.Normalize();
    float wdMag1 = wvNorm1.Normalize();

    if (wdMag0 < EPSILON || wdMag1 < EPSILON) {
        *this = cgqIdentity;
        return;
    }

    float wdDot = Vector3D::DotProduct(wvNorm0, wvNorm1);

    if (wdDot > 1.0f - QUATERNION_ARC_EPSILON) {
        *this = cgqIdentity;
        return;
    }

    if (wdDot < -1.0f + QUATERNION_ARC_EPSILON) {
        Set(cgwvXAxis, 0.0f);
        return;
    }

    Vector3D wvCross = Vector3D::CrossProduct(wvNorm0, wvNorm1);

    float flS = (1.0f + wdDot) * 2.0f;
    if (flS >= EPSILON) {
        flS = Math::Sqrt(flS);
        float flInvS = 1.0f / flS;

        wvImag = wvCross * flInvS;
        wdReal = flS * 0.5f;
    } else {
        wvImag = wvCross;
        wvImag.Normalize();
        wdReal = 0.0f;
    }
}

inline void Quaternion::FromMatrix(const Matrix3x3& m) {
    float flTrace = m._11 + m._22 + m._33 + 1.0f;

    if (flTrace >= 1.0f) {
        wdReal = flTrace;
        wvImag.x = m._23 - m._32;
        wvImag.y = m._31 - m._13;
        wvImag.z = m._12 - m._21;
    } else if (m._11 >= m._22 && m._11 >= m._33) {
        wvImag.x = m._11 - m._22 - m._33 + 1.0f;
        wvImag.y = m._21 + m._12;
        wvImag.z = m._31 + m._13;
        wdReal = m._23 - m._32;
    } else if (m._22 >= m._11 && m._22 >= m._33) {
        wvImag.y = m._22 - m._33 - m._11 + 1.0f;
        wvImag.z = m._32 + m._23;
        wvImag.x = m._12 + m._21;
        wdReal = m._31 - m._13;
    } else {
        wvImag.z = m._33 - m._11 - m._22 + 1.0f;
        wvImag.x = m._13 + m._31;
        wvImag.y = m._23 + m._32;
        wdReal = m._12 - m._21;
    }

    Normalize();
}

inline void Quaternion::FromChangeOfBasis(const Vector3D& wvSrc, const Vector3D& wvDest) {
    const Vector3D wvWorldUp(0.0f, 0.0f, 1.0f);

    Vector3D wvSrcForward(wvSrc);
    wvSrcForward.Normalize();
    Vector3D wvSrcRight = Vector3D::CrossProduct(wvSrcForward, wvWorldUp);
    wvSrcRight.Normalize();
    Vector3D wvSrcUp = Vector3D::CrossProduct(wvSrcRight, wvSrcForward);

    Vector3D wvDestForward(wvDest);
    wvDestForward.Normalize();
    Vector3D wvDestRight = Vector3D::CrossProduct(wvDestForward, wvWorldUp);
    wvDestRight.Normalize();
    Vector3D wvDestUp = Vector3D::CrossProduct(wvDestRight, wvDestForward);

    FromChangeOfBasis(wvSrcForward, wvSrcRight, wvSrcUp, wvDestForward, wvDestRight, wvDestUp);
}

// Line 54 (q on 55, qResult on 57). Line 59 is the inlined Vector3D subtraction, 60 the real part
// (Ragdoll::InitializeVelocities, X/Client/Physics/Ragdoll.cpp:249: imaginary part in registers first, then wdReal).
inline Quaternion Quaternion::operator-(const Quaternion& q) const {
    Quaternion qResult;
    qResult.wvImag = wvImag - q.wvImag;
    qResult.wdReal = wdReal - q.wdReal;
    return qResult;
}

// Line 134 (flScale on 135, q on 136); line 138 holds the inlined Vector3D * float and the real product
// (Ragdoll::InitializeVelocities, Ragdoll.cpp:249-250; flScale is the first fmul operand of the real part). No local.
inline Quaternion operator*(const float flScale, const Quaternion& q) {
    return Quaternion(q.wvImag * flScale, flScale * q.wdReal);
}

// Line 67 (the inlined Vector3D += is line 70), Cloth::Staple Cloth.cpp:1073.
inline Quaternion& Quaternion::operator+=(const Quaternion& q) {
    wvImag += q.wvImag;
    wdReal += q.wdReal;
    return *this;
}

// Line 41 (q on 42, qResult on 44); line 46 is the inlined Vector3D addition, 47 the real part
// (Ragdoll::ComputeBone, Ragdoll.cpp:679: the three imaginary sums, then the real sum, this + q in each).
inline Quaternion Quaternion::operator+(const Quaternion& q) const {
    Quaternion qResult;
    qResult.wvImag = wvImag + q.wvImag;
    qResult.wdReal = wdReal + q.wdReal;
    return qResult;
}

// Line 216 (q1In 217, q2In 218, flInvDt 219, q1 221, q2 222, dqdt 229, qw 230). From the two copies inlined into
// Ragdoll::ComputeBone (Ragdoll.cpp:650, 703): DotProduct 223 tested < 0 (b.pl skips the flip), q2's real part
// negated at 225 and its imaginary part (Vector3D unary minus) at 226; q2 - q1 and flInvDt * at 229; 2 *, the
// Quaternion product and Conjugate at 230; the Vector3D copy constructor at 231.
inline Vector3D Quaternion::AngularVelocity(const Quaternion& q1In, const Quaternion& q2In, float flInvDt) {
    Quaternion q1 = q1In;
    Quaternion q2 = q2In;
    if (DotProduct(q1, q2) < 0.0f) {
        q2.wdReal = -q2.wdReal;
        q2.wvImag = -q2.wvImag;
    }
    Quaternion dqdt = flInvDt * (q2 - q1);
    Quaternion qw = (2.0f * dqdt) * q1.Conjugate();
    return qw.wvImag;
}

// Line 448 (q 451, cflScale 452, x/y/z/w on 453-456). From the copies inlined into sInterpolateCurve
// (Animation.cpp:1386, 1388): each short is converted and multiplied by 3.05185e-05 (1/32767, the float at
// 0x7100e3c238) in x, y, z, w order into wvImag.x/y/z and wdReal.
inline Quaternion Quaternion16::Decompress() const {
    Quaternion q;
    const float cflScale = 1.0f / 32767.0f;
    q.wvImag.x = x * cflScale;
    q.wvImag.y = y * cflScale;
    q.wvImag.z = z * cflScale;
    q.wdReal = w * cflScale;
    return q;
}
