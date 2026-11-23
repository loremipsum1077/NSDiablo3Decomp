// Original file: Contrib/Contrib/Domino/Include/dmMath.inl
// The DWARF lists these inlines without bodies; each body here is written from
// the callers that inline it. Only the helpers some adopted caller needs are here.

#pragma once

inline dm_vec4 dmVecSet(dm_float32 x, dm_float32 y, dm_float32 z, dm_float32 w)
{
    const dm_vec4 result = { x, y, z, w };
    return result;
}

inline dm_vec4 dmVecSet(dm_float32 x, dm_float32 y, dm_float32 z)
{
    const dm_vec4 result = { x, y, z, 0.0f };
    return result;
}

inline dm_vec4 dmVecSet(dm_float32 s)
{
    return vdupq_n_f32(s);
}

inline dm_float32 dmF32(dm_vec4Arg v)
{
    return vgetq_lane_f32(v, 0);
}

inline dm_vec4 dmV4(dm_float32 s)
{
    return vdupq_n_f32(s);
}

inline dm_vec4 dmSplatX(dm_vec4Arg v)
{
    return vdupq_laneq_f32(v, 0);
}

inline dm_vec4 dmSplatY(dm_vec4Arg v)
{
    return vdupq_laneq_f32(v, 1);
}

inline dm_vec4 dmSplatZ(dm_vec4Arg v)
{
    return vdupq_laneq_f32(v, 2);
}

inline dm_vec4 dmAdd(dm_vec4Arg a, dm_vec4Arg b)
{
    return vaddq_f32(a, b);
}

inline dm_vec4 dmSub(dm_vec4Arg a, dm_vec4Arg b)
{
    return vsubq_f32(a, b);
}

inline dm_vec4 dmMul(dm_vec4Arg a, dm_vec4Arg b)
{
    return vmulq_f32(a, b);
}

inline bool dmAll_LT3(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t c = vcltq_f32(a, b);
    return (vgetq_lane_u32(c, 0) & vgetq_lane_u32(c, 1) & vgetq_lane_u32(c, 2)) != 0;
}

inline bool dmAll_GE3(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t c = vcgeq_f32(a, b);
    return (vgetq_lane_u32(c, 0) & vgetq_lane_u32(c, 1) & vgetq_lane_u32(c, 2)) != 0;
}

inline bool dmLT(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t c = vcltq_f32(a, b);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    return (vget_lane_u32(t2, 0) & vget_lane_u32(t2, 1)) != 0;
}

inline bool dmGT(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t c = vcgtq_f32(a, b);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    return (vget_lane_u32(t2, 0) & vget_lane_u32(t2, 1)) != 0;
}

inline bool dmLEZ(dm_vec4Arg a)
{
    uint32x4_t c = vclezq_f32(a);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    t2 = vand_u32(t2, vrev64_u32(t2));
    return vget_lane_u32(t2, 0) != 0;
}

inline bool dmGTZ(dm_vec4Arg a)
{
    uint32x4_t c = vcgtzq_f32(a);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    t2 = vand_u32(t2, vrev64_u32(t2));
    return vget_lane_u32(t2, 0) != 0;
}

inline dm_vec4 dmNegate(dm_vec4Arg v)
{
    return vnegq_f32(v);
}

inline dm_vec4 dmMin(dm_vec4Arg a, dm_vec4Arg b)
{
    return vminq_f32(a, b);
}

inline dm_vec4 dmMax(dm_vec4Arg a, dm_vec4Arg b)
{
    return vmaxq_f32(a, b);
}

inline dm_vec4 dmAbs(dm_vec4Arg v)
{
    dm_vec4 nv = dmNegate(v);
    return dmMax(nv, v);
}

// DWARF dmMath.inl:1884: t at 1901 (row 1901:17), one surviving vgetq_lane_f32 block at 1902; rows 1902:12 (vdupq_n_f32),
// 1902:45 and 1902:68 (the two '+'), 1902:70 (lane 2) fit a three-lane sum.
inline dm_vec4 dmDot3(dm_vec4Arg a, dm_vec4Arg b)
{
    dm_vec4 t = vmulq_f32(a, b);
    return vdupq_n_f32(vgetq_lane_f32(t, 0) + vgetq_lane_f32(t, 1) + vgetq_lane_f32(t, 2));
}

inline bool dmIsFinite3(dm_vec4Arg v)
{
    uint32x4_t u = vreinterpretq_u32_f32(v);
    uint32x4_t t = vceqzq_u32(vbicq_u32(vreinterpretq_u32_f32(dm_vec4_finite4), u));
    return vgetq_lane_u32(t, 0) == 0 && vgetq_lane_u32(t, 1) == 0 && vgetq_lane_u32(t, 2) == 0;
}

// dmMath.inl:240, written from dmBody_StabilizeTree.
inline bool dmIsFinite(dm_float32 x)
{
    union
    {
        dm_float32 f;
        dm_uint32 i;
    } u;
    u.f = x;
    return (u.i & 0x7F800000) != 0x7F800000;
}

inline dm_vec4 dmSqrt(dm_vec4Arg v)
{
    return vsqrtq_f32(v);
}

inline dm_vec4 dmLengthSqr3(dm_vec4Arg v)
{
    return dmDot3(v, v);
}

inline dm_float32 dmLengthSqr3_f32(dm_vec4Arg v)
{
    return dmF32(dmLengthSqr3(v));
}

inline dm_vec4 dmNormalize3(dm_vec4Arg v)
{
    dm_vec4 h = dm_vec4_oneHalf;
    dm_vec4 d = dmDot3(v, v);
    dm_vec4 e = vrsqrteq_f32(d);
    e = vmulq_f32(e, vrsqrtsq_f32(vmulq_f32(d, e), e));
    dm_vec4 halfD = vmulq_f32(d, h);
    dm_vec4 r = vaddq_f32(e, vmulq_f32(e, vsubq_f32(h, vmulq_f32(halfD, vmulq_f32(e, e)))));
    return vmulq_f32(v, r);
}

inline dm_vec4 dmAccurateNormalize3(dm_vec4Arg v)
{
    dm_vec4 a = vmulq_f32(v, v);
    dm_vec4 xx = vdupq_laneq_f32(a, 0);
    dm_vec4 yy = vdupq_laneq_f32(a, 1);
    dm_vec4 zz = vdupq_laneq_f32(a, 2);
    dm_vec4 result = vaddq_f32(vaddq_f32(xx, yy), zz);
    result = vsqrtq_f32(result);
    return vdivq_f32(v, result);
}

inline bool dmIsZeroSafe3(dm_vec4Arg v)
{
    dm_vec4 d = dmDot3(v, v);
    return dmGT(d, dm_vec4_zeroSafe) == false;
}

inline dm_float32 dmMin(dm_float32 a, dm_float32 b)
{
    return b < a ? b : a;
}

template <typename T>
inline T dmMin(T a, T b, T c)
{
    return dmMin(a, dmMin(b, c));
}

inline dm_float32 dmAbs(dm_float32 s)
{
    return fabsf(s);
}

// DWARF dmMath.inl:1856: t at 1878 (row 1878:17); rows 1879:33/56 (the two '+') and 1879:58 (lane 2) fit a three-lane sum.
inline dm_float32 dmDot3_f32(dm_vec4Arg a, dm_vec4Arg b)
{
    dm_vec4 t = vmulq_f32(a, b);
    return vgetq_lane_f32(t, 0) + vgetq_lane_f32(t, 1) + vgetq_lane_f32(t, 2);
}

inline dm_vec4 dmCross(dm_vec4Arg a, dm_vec4Arg b)  // dmMath.inl:1935: ca/cb/cr declared together at 1937, t at 1962; columns 1962-1971 fit these lines.
{
    dm_vec4 ca, cb, cr;
    float32x2x2_t t = vzip_f32(vrev64_f32(vget_low_f32(a)), vget_high_f32(a));
    ca = vcombine_f32(t.val[0], t.val[1]);
    t = vzip_f32(vget_high_f32(b), vget_low_f32(b));
    cb = vcombine_f32(t.val[0], vrev64_f32(t.val[1]));
    cr = vmulq_f32(ca, cb);
    t = vzip_f32(vget_high_f32(a), vget_low_f32(a));
    ca = vcombine_f32(t.val[0], vrev64_f32(t.val[1]));
    t = vzip_f32(vrev64_f32(vget_low_f32(b)), vget_high_f32(b));
    cb = vcombine_f32(t.val[0], t.val[1]);
    return vsubq_f32(cr, vmulq_f32(ca, cb));

}

// Reconstructed from the original math solver and broad-phase callers.
inline dm_float32 dmGetX(dm_vec4Arg v) { return vgetq_lane_f32(v, 0); }
inline bool dmLE(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t c = vcleq_f32(a, b);
    uint32x2_t t = vand_u32(vget_low_u32(c), vget_high_u32(c));
    return (vget_lane_u32(t, 0) & vget_lane_u32(t, 1)) != 0;
}
inline dm_vec4 dmVecSet(dm_vec4Arg x, dm_vec4Arg y, dm_vec4Arg z)
{
    dm_vec4 r = vcombine_f32(vzip_f32(vget_low_f32(x), vget_low_f32(y)).val[0],
                                  vget_low_f32(z));
    return r;
}
// Original dmMath.inl:1567, m and r at 1578 and 1579.
inline dm_vec4 dmSelect_LT(dm_vec4Arg a, dm_vec4Arg b, dm_vec4Arg t, dm_vec4Arg f)
{
    uint32x4_t m = vcltq_f32(a, b);
    uint32x4_t r = vbslq_u32(m, vreinterpretq_u32_f32(t), vreinterpretq_u32_f32(f));
    return vreinterpretq_f32_u32(r);
}
inline dm_vec4 dmSelect_GT(dm_vec4Arg a, dm_vec4Arg b, dm_vec4Arg t, dm_vec4Arg f)
{
    return vbslq_f32(vcgtq_f32(a, b), t, f);
}
inline dm_vec4 dmSafeReciprocal(dm_vec4Arg v)
{
    dm_vec4 e = vrecpeq_f32(v);
    e = vmulq_f32(e, vrecpsq_f32(v, e));
    dm_vec4 r = dmMul(e, dmSub(dm_vec4_two, dmMul(v, e)));
    return dmSelect_GT(dmAbs(v), dm_vec4_zeroSafe, r, dmVecSet(0.0f));
}
inline dmMtx dmTranspose33(const dmMtx & a)
{
    dmMtx b;
    b.rx = vcombine_f32(vzip_f32(vget_low_f32(a.rx), vget_low_f32(a.ry)).val[0], vzip_f32(vget_low_f32(a.rz), vdup_laneq_f32(a.rz, 3)).val[0]);
    b.ry = vcombine_f32(vzip_f32(vget_low_f32(a.rx), vget_low_f32(a.ry)).val[1], (float32x2_t){vgetq_lane_f32(a.rz, 1), vgetq_lane_f32(a.rz, 3)});
    b.rz = vcombine_f32(vzip_f32(vget_high_f32(a.rx), vget_high_f32(a.ry)).val[0], vget_high_f32(a.rz));
    return b;
}
// DWARF dmMath.inl:3481 (m, v): tmp, acc on 3513, vdupq_laneq_f32 blocks (__s0/__ret) on 3514/3516/3519, no inlined
// calls; rows 3515:11/28, 3517:11, 3518:11, 3520:11/28, 3521:11 fit "acc = vmulq_f32(tmp, m.rx);" (rx at column 28).
inline dm_vec4 dmMul33(const dmMtx & m, dm_vec4Arg v)
{
    dm_vec4 tmp, acc;
    tmp = vdupq_laneq_f32(v, 0);
    acc = vmulq_f32(tmp, m.rx);
    tmp = vdupq_laneq_f32(v, 1);
    tmp = vmulq_f32(tmp, m.ry);
    acc = vaddq_f32(acc, tmp);
    tmp = vdupq_laneq_f32(v, 2);
    tmp = vmulq_f32(tmp, m.rz);
    acc = vaddq_f32(acc, tmp);
    return acc;
}
inline dm_vec4 dmScalarTripleProduct(dm_vec4Arg a, dm_vec4Arg b, dm_vec4Arg c)
{
    dm_vec4 t1 = vcombine_f32(vzip_f32(vdup_n_f32(vgetq_lane_f32(b, 1)), vget_high_f32(b)).val[0], vzip_f32(vget_low_f32(b), vdup_laneq_f32(b, 3)).val[0]);
    dm_vec4 t2 = vcombine_f32(vzip_f32(vget_high_f32(c), vget_low_f32(c)).val[0], (float32x2_t){vgetq_lane_f32(c, 1), vgetq_lane_f32(c, 3)});
    dm_vec4 t3 = vcombine_f32(vzip_f32(vget_high_f32(b), vget_low_f32(b)).val[0], (float32x2_t){vgetq_lane_f32(b, 1), vgetq_lane_f32(b, 3)});
    dm_vec4 t4 = vcombine_f32(vzip_f32(vdup_n_f32(vgetq_lane_f32(c, 1)), vget_high_f32(c)).val[0], vzip_f32(vget_low_f32(c), vdup_laneq_f32(c, 3)).val[0]);
    dm_vec4 bc = dmSub(dmMul(t1, t2), dmMul(t3, t4));
    dm_vec4 d = dmMul(a, bc);
    dm_vec4 dx = dmSplatX(d), dy = dmSplatY(d), dz = dmSplatZ(d);
    dm_vec4 e = dmAdd(dx, dy);
    return dmAdd(e, dz);
}

inline dm_float32 dmGetY(dm_vec4Arg v) { return vgetq_lane_f32(v, 1); }
inline bool dmAll_LE3(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t c = vcleq_f32(a, b);
    return (vgetq_lane_u32(c, 0) & vgetq_lane_u32(c, 1) & vgetq_lane_u32(c, 2)) != 0;
}
inline dm_vec4 dmMulAdd(dm_vec4Arg a, dm_vec4Arg b, dm_vec4Arg d) { return dmAdd(dmMul(a, b), d); }
template <typename T>
inline T dmMax(T a, T b) { return b > a ? b : a; }
template <typename T> inline T dmMin(T a, T b) { return a < b ? a : b; }  // dmMath.inl:85
inline dm_vec4 dmRotate(dm_vec4Arg q, dm_vec4Arg u)  // Quaternion helpers recovered from dmTransformAABB's original inline calls. Body (dmMath.inl:2540, t@2572, v@2576): only dmCross inlined; columns 2574:9/22 and 2576:17/30 fit the NEON calls below.
{
    // qw at 2573 (vdupq_laneq_f32 block, row 2573:18).
    dm_vec4 t = dmCross(q, u);
    dm_vec4 qw = vdupq_laneq_f32(q, 3);
    t = vaddq_f32(t, vmulq_f32(u, qw));
    t = dmCross(q, t);
    dm_vec4 v = vaddq_f32(u, vaddq_f32(t, t));
    return v;
}
inline dm_vec4 dmMul(const dmTransform & xf, dm_vec4Arg v)
{
    dm_vec4 t = dmRotate(xf.q, v);
    return dmAdd(xf.p, t);
}
inline dmMtx dmMtxFromQuat(dm_vec4Arg q)
{
    dm_vec4 q0 = dmAdd(q, q);
    dm_vec4 q1 = dmMul(q, q0);
    dm_vec4 q2 = dmMul(q0, dmSplatX(q));
    float32x2_t q3 = vmul_n_f32(vget_high_f32(q0), vgetq_lane_f32(q, 1));
    float q4 = vgetq_lane_f32(q, 3) * vgetq_lane_f32(q0, 2);
    dmMtx R;
    R.rx = dmVecSet(1.0f - vgetq_lane_f32(q1, 1) - vgetq_lane_f32(q1, 2),
                   vgetq_lane_f32(q2, 1) + q4,
                   vgetq_lane_f32(q2, 2) - vget_lane_f32(q3, 1));
    R.ry = dmVecSet(vgetq_lane_f32(q2, 1) - q4,
                   1.0f - vgetq_lane_f32(q1, 0) - vgetq_lane_f32(q1, 2),
                   vgetq_lane_f32(q2, 3) + vget_lane_f32(q3, 0));
    R.rz = dmVecSet(vgetq_lane_f32(q2, 2) + vget_lane_f32(q3, 1),
                   vget_lane_f32(q3, 0) - vgetq_lane_f32(q2, 3),
                   1.0f - vgetq_lane_f32(q1, 0) - vgetq_lane_f32(q1, 1));
    // dmPrismaticJoint::InitVelocityConstraints stores a zero p row (stp xzr at 131:16) after copying the result.
    R.p = dmVecSet(0.0f);
    return R;
}

// Recovered from the sweep and narrow-phase callers' inline trees.
inline dm_vec4 dmDiv(dm_vec4Arg a, dm_vec4Arg b) { return vdivq_f32(a, b); }
inline bool dmEQZ(dm_vec4Arg a)
{
    uint32x4_t c = vceqzq_f32(a);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    t2 = vand_u32(t2, vdup_lane_u32(t2, 1));
    return vget_lane_u32(t2, 0) != 0;
}
inline dm_vec4 dmDot4(dm_vec4Arg a, dm_vec4Arg b)
{
    dm_vec4 t0 = dmMul(a, b);
    float32x2_t t1 = vpadd_f32(vget_low_f32(t0), vget_high_f32(t0));
    return vdupq_n_f32(vpadds_f32(t1));
}
// dmMath.inl:2713, written from dmWorld_CreateBody. Body shape from original columns 2731:20/30/33, 2732:9, 2733:21/47, 2734:18/39/41 (lax vector conversions; vandq + vceqq folds to bic + cmeq #0).
inline bool dmIsFinite4(dm_vec4Arg v)
{
    uint32x4_t t = vandq_u32(v, dm_vec4_finite4);
    t = vceqq_u32(t, dm_vec4_finite4);
    uint32x2_t t2 = vorr_u32(vget_low_u32(t), vget_high_u32(t));
    return 0 == (vget_lane_u32(t2, 0) | vget_lane_u32(t2, 1));
}
// dmMath.inl:3176, written from dmWorld_CreateBody.
inline bool dmIsZeroSafe4(dm_vec4Arg v)
{
    dm_vec4 d = dmDot4(v, v);
    return dmGT(d, dm_vec4_zeroSafe);
}

inline dm_vec4 dmNormalize4(dm_vec4Arg v)
{
    dm_vec4 d = dmDot4(v, v);
    dm_vec4 h = dm_vec4_oneHalf;
    dm_vec4 e = vrsqrteq_f32(d);
    e = vmulq_f32(e, vrsqrtsq_f32(vmulq_f32(d, e), e));
    dm_vec4 halfD = vmulq_f32(d, h);
    dm_vec4 r = vmulq_f32(e, e);
    r = vmulq_f32(halfD, r);
    r = vsubq_f32(h, r);
    r = vmulq_f32(e, r);
    r = vaddq_f32(e, r);
    return vmulq_f32(v, r);
}
inline dm_vec4 dmQuatNLerp(dm_vec4Arg q1, dm_vec4Arg q2, dm_vec4Arg eta)
{
    dm_vec4 dot = dmDot4(q1, q2);
    dm_vec4 q = dmSelect_GT(dot, dmVecSet(0.0f), q2, dmNegate(q2));
    q = dmAdd(q1, dmMul(eta, dmSub(q, q1)));
    return dmNormalize4(q);
}
// DWARF dmMath.inl:2606 (q, u): t at 2633, qw at 2634 (vdupq_laneq_f32 block), only dmCross inlined (2633, 2636);
// rows 2635:9/22 and 2637:9/22 fit the NEON calls below (the same spelling as dmRotate 2574/2576).
inline dm_vec4 dmInvRotate(dm_vec4Arg q, dm_vec4Arg u)
{
    dm_vec4 t = dmCross(u, q);
    dm_vec4 qw = vdupq_laneq_f32(q, 3);
    t = vaddq_f32(t, vmulq_f32(u, qw));
    t = dmCross(t, q);
    t = vaddq_f32(u, vaddq_f32(t, t));
    return t;
}
inline dm_vec4 dmMulT(const dmTransform & xf, dm_vec4Arg v)
{
    dm_vec4 t = dmSub(v, xf.p);
    return dmInvRotate(xf.q, t);
}
inline dm_vec4 dmCompareGT(dm_vec4Arg a, dm_vec4Arg b)
{
    return vreinterpretq_f32_u32(vcgtq_f32(a, b));
}
inline dm_vec4 dmSelect(dm_vec4Arg mask, dm_vec4Arg t, dm_vec4Arg f)
{
    return vbslq_f32(vreinterpretq_u32_f32(mask), t, f);
}
inline dm_vec4 dmReciprocal(dm_vec4Arg v)
{
    dm_vec4 e = vrecpeq_f32(v);
    e = vmulq_f32(e, vrecpsq_f32(v, e));
    return dmMul(e, dmSub(dm_vec4_two, dmMul(v, e)));
}
inline dmMtx dmMtxFromTransform(const dmTransform & xf)
{
    dmMtx R = dmMtxFromQuat(xf.q);
    R.p = xf.p;
    return R;
}
inline dm_vec4 dmMul34(const dmMtx & a, dm_vec4Arg v)
{
    return dmAdd(a.p, dmMul33(a, v));
}
inline bool dmGE(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t c = vcgeq_f32(a, b);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    return (vget_lane_u32(t2, 0) & vget_lane_u32(t2, 1)) != 0;
}
inline bool dmLTZ(dm_vec4Arg a)
{
    uint32x4_t c = vcltzq_f32(a);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    t2 = vand_u32(t2, vdup_lane_u32(t2, 1));
    return vget_lane_u32(t2, 0) != 0;
}
inline dm_vec4 dmClamp(dm_vec4Arg v, dm_vec4Arg lower, dm_vec4Arg upper)
{
    return dmMax(lower, dmMin(v, upper));
}
inline dm_vec4 dmLength3(dm_vec4Arg v) { return dmSqrt(dmDot3(v, v)); }

// Matrix helpers written from the spherical joint solver, which inlines them
// (original dmMath.inl:1695, 2217, 3528, 3648, 3700, 3839).
inline dmMtx operator+(const dmMtx & a, const dmMtx & b)
{
    dmMtx c;
    c.rx = dmAdd(a.rx, b.rx);
    c.ry = dmAdd(a.ry, b.ry);
    c.rz = dmAdd(a.rz, b.rz);
    c.p = dmAdd(a.p, b.p);
    return c;
}
// Original dmMath.inl:1706, written from sSolveGyroscopic (dmIsland.cpp:307): dmSub inlined
// at 1709-1711 only; the DWARF has no dmSub for p, so c.p is left unset.
inline dmMtx operator-(const dmMtx & a, const dmMtx & b)
{
    dmMtx c;
    c.rx = dmSub(a.rx, b.rx);
    c.ry = dmSub(a.ry, b.ry);
    c.rz = dmSub(a.rz, b.rz);
    return c;
}
// Original dmMath.inl:1729, written from sSolveGyroscopic (dmIsland.cpp:307): dmMul(s, .)
// inlined at 1732-1735 (fmul operand order s first).
inline dmMtx operator*(dm_vec4Arg s, const dmMtx & a)
{
    dmMtx c;
    c.rx = dmMul(s, a.rx);
    c.ry = dmMul(s, a.ry);
    c.rz = dmMul(s, a.rz);
    c.p = dmMul(s, a.p);
    return c;
}
// Original dmMath.inl:2217: q * (v, 0), written from the contact solver's position step.
inline dm_vec4 dmMulQV(dm_vec4Arg q, dm_vec4Arg v)
{
    dm_vec4 qn = dmNegate(q);
    dm_vec4 c1 = vcombine_f32(vrev64_f32(vget_high_f32(q)), vrev64_f32(vget_low_f32(qn)));
    dm_vec4 c2 = vcombine_f32(vzip_f32(vget_high_f32(qn), vrev64_f32(vget_high_f32(q))).val[0],
                              vzip_f32(vget_low_f32(q), vrev64_f32(vget_low_f32(qn))).val[0]);
    dm_vec4 c3 = vcombine_f32(vzip_f32(vrev64_f32(vget_low_f32(q)), vget_low_f32(qn)).val[0],
                              vzip_f32(vrev64_f32(vget_high_f32(q)), vget_high_f32(qn)).val[0]);
    dm_vec4 qv = dmAdd(dmMul(c1, dmSplatX(v)), dmMul(c2, dmSplatY(v)));
    qv = dmAdd(dmMul(c3, dmSplatZ(v)), qv);
    return qv;
}
inline dm_vec4 dmMulT33(const dmMtx & m, dm_vec4Arg v)
{
    dm_vec4 x = dmDot3(m.rx, v);
    dm_vec4 y = dmDot3(m.ry, v);
    dm_vec4 z = dmDot3(m.rz, v);
    dm_vec4 u = vcombine_f32(vzip_f32(vget_low_f32(x), vget_low_f32(y)).val[0], vget_low_f32(z));
    return u;
}
inline dm_vec4 dmSetW(dm_vec4Arg v, dm_vec4Arg w);
inline dmMtx dmMtxSetSkew(dm_vec4Arg v)
{
    dm_vec4 u = dmSetW(v, vdupq_n_f32(0.0f));
    dm_vec4 un = dmNegate(u);
    dmMtx S;
    S.rx = vcombine_f32(vrev64_f32(vget_high_f32(u)), vzip_f32(vget_low_f32(un), vget_high_f32(un)).val[1]);
    S.ry = vcombine_f32(vget_high_f32(un), vzip_f32(vget_low_f32(u), vrev64_f32(vget_high_f32(u))).val[0]);
    S.rz = vcombine_f32(vzip_f32(vdup_laneq_f32(u, 1), vget_low_f32(un)).val[0],
                        vzip_f32(vget_high_f32(u), vget_high_f32(un)).val[1]);
    return S;
}
inline dmMtx dmMul33(const dmMtx & a, const dmMtx & b)
{
    dm_vec4 ax = a.rx, ay = a.ry, az = a.rz;
    dmMtx c;
    c.rx = vaddq_f32(vmulq_laneq_f32(ax, b.rx, 0), vaddq_f32(vmulq_laneq_f32(ay, b.rx, 1), vmulq_laneq_f32(az, b.rx, 2)));
    c.ry = vaddq_f32(vmulq_laneq_f32(ax, b.ry, 0), vaddq_f32(vmulq_laneq_f32(ay, b.ry, 1), vmulq_laneq_f32(az, b.ry, 2)));
    c.rz = vaddq_f32(vmulq_laneq_f32(ax, b.rz, 0), vaddq_f32(vmulq_laneq_f32(ay, b.rz, 1), vmulq_laneq_f32(az, b.rz, 2)));
    return c;
}
// R * A * R^T: B = A * R^T, then C = R * B.
inline dmMtx dmMulRART(const dmMtx & R, const dmMtx & A)
{
    dm_vec4 rx = R.rx, ry = R.ry, rz = R.rz;
    dm_vec4 ax = A.rx, ay = A.ry, az = A.rz;
    dm_vec4 bx = vaddq_f32(vmulq_laneq_f32(ax, rx, 0), vaddq_f32(vmulq_laneq_f32(ay, ry, 0), vmulq_laneq_f32(az, rz, 0)));
    dm_vec4 by = vaddq_f32(vmulq_laneq_f32(ax, rx, 1), vaddq_f32(vmulq_laneq_f32(ay, ry, 1), vmulq_laneq_f32(az, rz, 1)));
    dm_vec4 bz = vaddq_f32(vmulq_laneq_f32(ax, rx, 2), vaddq_f32(vmulq_laneq_f32(ay, ry, 2), vmulq_laneq_f32(az, rz, 2)));
    dm_vec4 cx = vaddq_f32(vmulq_laneq_f32(rx, bx, 0), vaddq_f32(vmulq_laneq_f32(ry, bx, 1), vmulq_laneq_f32(rz, bx, 2)));
    dm_vec4 cy = vaddq_f32(vmulq_laneq_f32(rx, by, 0), vaddq_f32(vmulq_laneq_f32(ry, by, 1), vmulq_laneq_f32(rz, by, 2)));
    dm_vec4 cz = vaddq_f32(vmulq_laneq_f32(rx, bz, 0), vaddq_f32(vmulq_laneq_f32(ry, bz, 1), vmulq_laneq_f32(rz, bz, 2)));
    dmMtx C;
    C.rx = cx;
    C.ry = cy;
    C.rz = cz;
    return C;
}

// Original dmMath.inl:200; scratch name reconstructed for the copy-based swap.
template <class T> inline void dmSwap(T & a, T & b)
{
    T temp = a;
    a = b;
    b = temp;
}

// Original quaternion product uses signed permutations, preserving the four-term sum order.
inline dm_vec4 dmConjugate(dm_vec4Arg q)
{
    return q * dmVecSet(-1.0f, -1.0f, -1.0f, 1.0f);
}
inline dm_vec4 dmMulQQ(dm_vec4Arg a, dm_vec4Arg b)
{
    const dm_vec4 sx = dmVecSet(1.0f, 1.0f, -1.0f, -1.0f);
    const dm_vec4 sy = dmVecSet(-1.0f, 1.0f, 1.0f, -1.0f);
    const dm_vec4 sz = dmVecSet(1.0f, -1.0f, 1.0f, -1.0f);
    // Shape from the DWARF locals (sx..sz 2178-2180, bx..bw 2182-2185, cx 2187, cy 2191, cz 2195,
    // q 2204) and the line rows: each c* is scaled on 2200-2202, then q = a*bw summed on 2205-2207.
    dm_vec4 bx = vdupq_laneq_f32(b, 0);
    dm_vec4 by = vdupq_laneq_f32(b, 1);
    dm_vec4 bz = vdupq_laneq_f32(b, 2);
    dm_vec4 bw = vdupq_laneq_f32(b, 3);
    dm_vec4 cx = vcombine_f32(vrev64_f32(vget_high_f32(a)), vrev64_f32(vget_low_f32(a)));
    dm_vec4 cy = vcombine_f32(vget_high_f32(a), vget_low_f32(a));
    dm_vec4 cz = vcombine_f32(vrev64_f32(vget_low_f32(a)), vrev64_f32(vget_high_f32(a)));
    cx = cx * sx * bx;
    cy = cy * sy * by;
    cz = cz * sz * bz;
    dm_vec4 q = a * bw;
    q = q + cx;
    q = cy + q;
    q = cz + q;
    return q;
}
inline dmTransform dmMulT(const dmTransform & a, const dmTransform & b)
{
    dm_vec4 aqc = dmConjugate(a.q);
    dmTransform c;
    c.q = dmMulQQ(aqc, b.q);
    c.p = dmRotate(aqc, b.p - a.p);
    return c;
}
inline dmTransform dmTransformIdentity()
{
    dmTransform xf;
    xf.q = dm_quat_identity;
    xf.p = dmVecSet(0.0f);
    return xf;
}
// Original dmMath.inl:4150, written from UpdateTriangleBuffer's inlined call (conjugate 4153, rotate and negate 4154).
inline dmTransform dmInvert(const dmTransform & xf)
{
    dmTransform inv;
    inv.q = dmConjugate(xf.q);
    inv.p = dmNegate(dmRotate(inv.q, xf.p));
    return inv;
}

inline dm_float32 dmLength3_f32(dm_vec4Arg v) { return dmF32(dmLength3(v)); }
inline dm_vec4 dmSetW(dm_vec4Arg v, dm_vec4Arg w)
{
    return vcombine_f32(vget_low_f32(v), vzip_f32(vget_high_f32(v), vget_low_f32(w)).val[0]);
}
inline dm_float32 dmGetZ(dm_vec4Arg v) { return vgetq_lane_f32(v, 2); }

inline dm_vec4 dmSplatW(dm_vec4Arg v) { return vdupq_laneq_f32(v, 3); }
// Original dmMath.inl:4159 (xf, plane), written from dmPolytope::Create's inlined copy: locals p (4172, dmRotate inlined) and
// w (4173, dmSplatW, dmDot3 and dmAdd inlined), dmSetW at 4174. Operand order from the original fmul (xf.p * p) and fadd (w + dot).
inline dm_vec4 dmTransformPlane(const dmTransform & xf, dm_vec4Arg plane)
{
    dm_vec4 p = dmRotate(xf.q, plane);
    dm_vec4 w = dmAdd(dmSplatW(plane), dmDot3(xf.p, p));
    return dmSetW(p, w);
}
// Original dmMath.inl:2346: (v, 0) * q, written from the distance joint's position step.
inline dm_vec4 dmMulVQ(dm_vec4Arg v, dm_vec4Arg q)
{
    dm_vec4 qn = dmNegate(q);
    dm_vec4 vx = dmSplatX(v);
    dm_vec4 vy = dmSplatY(v);
    dm_vec4 vz = dmSplatZ(v);
    dm_vec4 c1 = vcombine_f32(vzip_f32(vdup_laneq_f32(q, 3), vget_high_f32(qn)).val[0],
                               vzip_f32(vdup_laneq_f32(q, 1), vget_low_f32(qn)).val[0]);
    dm_vec4 c2 = vcombine_f32(vget_high_f32(q), vget_low_f32(qn));
    dm_vec4 c3 = vcombine_f32(vzip_f32(vdup_lane_f32(vget_low_f32(qn), 1), vget_low_f32(q)).val[0],
                               vzip_f32(vdup_laneq_f32(q, 3), vget_high_f32(qn)).val[0]);
    c1 = dmMul(c1, vx);
    c2 = dmMul(c2, vy);
    c3 = dmMul(c3, vz);
    dm_vec4 vq = c1;
    vq = dmAdd(c2, vq);
    return dmAdd(c3, vq);
}

// Original dmMath.inl:566.
inline dm_float32 dmGetW(dm_vec4Arg v) { return vgetq_lane_f32(v, 3); }

// Original dmMath.inl:672-711. Written from the contact solver, which inlines them.
inline dm_vec4 dmSetX(dm_vec4Arg v, dm_vec4Arg s)
{
    return vcombine_f32(vzip_f32(vget_low_f32(s), vget_low_f32(v)).val[1], vget_high_f32(v));
}
inline dm_vec4 dmSetY(dm_vec4Arg v, dm_vec4Arg s)
{
    return vcombine_f32(vzip_f32(vget_low_f32(v), vget_low_f32(s)).val[0], vget_high_f32(v));
}
inline dm_vec4 dmSetZ(dm_vec4Arg v, dm_vec4Arg s)
{
    return vcombine_f32(vget_low_f32(v), vzip_f32(vget_high_f32(s), vget_low_f32(v)).val[1]);
}

// Original dmMath.inl:840.
inline dm_vec4 dmAnd(dm_vec4Arg a, dm_vec4Arg b)
{
    return vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)));
}

// Original dmMath.inl:2967: normalize with a precomputed squared length.
inline dm_vec4 dmNormalize3(dm_vec4Arg v, dm_vec4Arg d)
{
    dm_vec4 h = dm_vec4_oneHalf;
    dm_vec4 e = vrsqrteq_f32(d);
    e = vmulq_f32(e, vrsqrtsq_f32(vmulq_f32(d, e), e));
    dm_vec4 halfD = vmulq_f32(d, h);
    dm_vec4 r = vaddq_f32(e, vmulq_f32(e, vsubq_f32(h, vmulq_f32(halfD, vmulq_f32(e, e)))));
    return vmulq_f32(v, r);
}

// Original dmMath.inl:430.
inline dm_vec4 dmVecSet(dm_vec4Arg x, dm_vec4Arg y, dm_vec4Arg z, dm_vec4Arg w)
{
    dm_vec4 r = vcombine_f32(vzip_f32(vget_low_f32(x), vget_low_f32(y)).val[0],
                             vzip_f32(vget_low_f32(z), vget_low_f32(w)).val[0]);
    return r;
}

// Original dmMath.inl:4224: a unit axis perpendicular to axis3, and the third axis.
inline void dmComputeAlignedBasis(dm_vec4 * pAxis1, dm_vec4 * pAxis2, dm_vec4Arg axis3)
{
    dm_vec4 x = dmSplatX(axis3);
    dm_vec4 y = dmSplatY(axis3);

    dm_vec4 axis1a = dmVecSet(dmSplatZ(axis3), vdupq_n_f32(0.0f), dmNegate(x));
    dm_vec4 axis1b = dmVecSet(y, dmNegate(x), vdupq_n_f32(0.0f));

    dm_vec4 axis1 = dmSelect_GT(dmAbs(y), dm_vec4_oneHalf, axis1b, axis1a);
    axis1 = dmNormalize3(axis1);

    *pAxis1 = axis1;
    *pAxis2 = dmCross(axis3, axis1);
}

// Original dmMath.inl:211: a cheap atan2 (about 0.01 radian error), written
// from dmRevoluteJoint_SetDesiredMotorAngle, which inlines it.
inline dm_float32 dmEstAtan2(dm_float32 y, dm_float32 x)
{
    if (x == 0.0f)
    {
        if (y > 0.0f) return 1.57079632679f;
        if (y == 0.0f) return 0.0f;
        return -1.57079632679f;
    }

    dm_float32 result;
    dm_float32 z = y / x;
    if (dmAbs(z) < 1.0f)
    {
        result = z / (1.0f + 0.28f * z * z);
        if (x < 0.0f)
        {
            if (y < 0.0f) return result - 3.14159265359f;
            return result + 3.14159265359f;
        }
    }
    else
    {
        result = 1.57079632679f - z / (z * z + 0.28f);
        if (y < 0.0f) return result - 3.14159265359f;
    }

    return result;
}

// Original dmMath.inl:1765 and 1807: three-way component min and max.
inline dm_vec4 dmMin(dm_vec4Arg a, dm_vec4Arg b, dm_vec4Arg c)
{
    return vminq_f32(vminq_f32(a, b), c);
}
inline dm_vec4 dmMax(dm_vec4Arg a, dm_vec4Arg b, dm_vec4Arg c)
{
    return vmaxq_f32(vmaxq_f32(a, b), c);
}

// Recovered from the dmSphere::ComputeMass inline tree (dmMath.inl:829, 1717).
inline dmMtx dmMtxIdentity()
{
    dmMtx E;
    E.rx = dm_vec4_unitX;
    E.ry = dm_vec4_unitY;
    E.rz = dm_vec4_unitZ;
    E.p = dmVecSet(0.0f, 0.0f, 0.0f);
    return E;
}
inline dmMtx operator*(dm_float32 s, const dmMtx & a)
{
    dm_vec4 v = dmV4(s);
    dmMtx c;
    c.rx = dmMul(v, a.rx);
    c.ry = dmMul(v, a.ry);
    c.rz = dmMul(v, a.rz);
    c.p = dmMul(v, a.p);
    return c;
}
// Original dmMath.inl:1740 (local v@1742, dmMul calls on 1743-1746), written from
// dmShoulderJoint::InitVelocityConstraints. The DWARF has no dmV4 call for v.
inline dmMtx & operator*=(dmMtx & a, dm_float32 s)
{
    dm_vec4 v = vdupq_n_f32(s);
    a.rx = dmMul(a.rx, v);
    a.ry = dmMul(a.ry, v);
    a.rz = dmMul(a.rz, v);
    a.p = dmMul(a.p, v);
    return a;
}

// dmMath.inl:1152, written from dmDynamicTree::RayCast (locals t@1171, lane macros@1172).
inline bool dmAll_GT3(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t t = vcgtq_f32(a, b);
    return (vgetq_lane_u32(t, 0) & vgetq_lane_u32(t, 1) & vgetq_lane_u32(t, 2)) != 0;
}

// dmMath.inl:1252, written from dmTreeMesh::RayCast (local t@1271, lane macros@1272).
inline bool dmAny_GT3(dm_vec4Arg a, dm_vec4Arg b)
{
    uint32x4_t t = vcgtq_f32(a, b);
    return (vgetq_lane_u32(t, 0) | vgetq_lane_u32(t, 1) | vgetq_lane_u32(t, 2)) != 0;
}

// dmMath.inl:1979, written from dmDynamicTree::RayCast. The DWARF has ca, cb and cr
// declared together on line 1981 and no lane macros; the statements sit on
// original lines 2006-2008, 2010-2012, 2014, 2015-2017, 2019-2021 and 2023.
inline dm_vec4 dmModifiedCross(dm_vec4Arg a, dm_vec4Arg b)
{
    dm_vec4 ca, cb, cr;
    ca = vcombine_f32(
        vzip_f32(vrev64_f32(vget_low_f32(a)), vget_high_f32(a)).val[0],
        vzip_f32(vget_low_f32(a), vrev64_f32(vget_high_f32(a))).val[0]);

    cb = vcombine_f32(
        vzip_f32(vget_high_f32(b), vget_low_f32(b)).val[0],
        vzip_f32(vget_low_f32(b), vget_high_f32(b)).val[1]);

    cr = vmulq_f32(ca, cb);
    ca = vcombine_f32(
        vzip_f32(vget_high_f32(a), vget_low_f32(a)).val[0],
        vzip_f32(vget_low_f32(a), vget_high_f32(a)).val[1]);

    cb = vcombine_f32(
        vzip_f32(vrev64_f32(vget_low_f32(b)), vget_high_f32(b)).val[0],
        vzip_f32(vget_low_f32(b), vrev64_f32(vget_high_f32(b))).val[0]);

    cr = vaddq_f32(cr, vmulq_f32(ca, cb));
    return cr;
}

// Its assert bakes the original file string and line 4200; the rows' w lanes
// and the translation are left unset (no store survives). This must stay the
// last thing in the file: the #line below renumbers everything after it.
#include "dmSettings.h"
#line 4181 "../../..\\Domino/Include/dmMath.inl"
inline dmMtx dmShiftInertiaToOrigin(const dmMtx & I, dm_vec4Arg c, dm_float32 mass)
{

    dm_float32 Ixx = dmGetX(I.rx);
    dm_float32 Iyy = dmGetY(I.ry);
    dm_float32 Izz = dmGetZ(I.rz);

    dm_float32 Ixy = dmGetY(I.rx);
    dm_float32 Ixz = dmGetZ(I.rx);
    dm_float32 Iyz = dmGetZ(I.ry);

    dm_float32 cx = dmGetX(c);
    dm_float32 cy = dmGetY(c);
    dm_float32 cz = dmGetZ(c);

    Ixx += mass * (cy * cy + cz * cz);
    Iyy += mass * (cz * cz + cx * cx);
    Izz += mass * (cx * cx + cy * cy);

    dmAssert(Ixx >= 0.0f && Iyy >= 0.0f && Izz >= 0.0f);

    Ixy -= mass * cx * cy;
    Ixz -= mass * cx * cz;
    Iyz -= mass * cy * cz;

    dmMtx Io;
    Io.rx = dmVecSet(Ixx, Ixy, Ixz);
    Io.ry = dmVecSet(Ixy, Iyy, Iyz);
    Io.rz = dmVecSet(Ixz, Iyz, Izz);
    return Io;
}

// Original dmMath.inl:177: scalar clamp done in NEON lanes.
inline dm_float32 dmClamp(dm_float32 a, dm_float32 low, dm_float32 high)
{
    float32x2_t sv = vdup_n_f32(a);
    float32x2_t lv = vdup_n_f32(low);
    float32x2_t uv = vdup_n_f32(high);
    float32x2_t t = vmax_f32(lv, vmin_f32(sv, uv));
    return vget_lane_f32(t, 0);
}

// Original dmMath.inl:868.
inline dm_vec4 dmOr(dm_vec4Arg a, dm_vec4Arg b)
{
    return vreinterpretq_f32_u32(vorrq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)));
}

// Original dmMath.inl:1397.
inline dm_vec4 dmCompareLT(dm_vec4Arg a, dm_vec4Arg b)
{
    return vreinterpretq_f32_u32(vcltq_f32(a, b));
}

// Original dmMath.inl:1547; same shape as dmGTZ (one lane extract in the DWARF).
inline bool dmGEZ(dm_vec4Arg a)
{
    uint32x4_t c = vcgezq_f32(a);
    uint32x2_t t2 = vand_u32(vget_low_u32(c), vget_high_u32(c));
    t2 = vand_u32(t2, vrev64_u32(t2));
    return vget_lane_u32(t2, 0) != 0;
}
