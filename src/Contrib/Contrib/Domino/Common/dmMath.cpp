#include "../Include/dmMath.h"

// Constant initializers recovered from the original read-only data.
extern const dm_vec4 dm_vec4_oneThird = { 0x1.5555560000000p-2f, 0x1.5555560000000p-2f, 0x1.5555560000000p-2f, 0x1.5555560000000p-2f };
extern const dm_vec4 dm_vec4_oneHalf = { 0x1.0000000000000p-1f, 0x1.0000000000000p-1f, 0x1.0000000000000p-1f, 0x1.0000000000000p-1f };
extern const dm_vec4 dm_vec4_one = { 0x1.0000000000000p+0f, 0x1.0000000000000p+0f, 0x1.0000000000000p+0f, 0x1.0000000000000p+0f };
extern const dm_vec4 dm_vec4_two = { 0x1.0000000000000p+1f, 0x1.0000000000000p+1f, 0x1.0000000000000p+1f, 0x1.0000000000000p+1f };
extern const dm_vec4 dm_vec4_three = { 0x1.8000000000000p+1f, 0x1.8000000000000p+1f, 0x1.8000000000000p+1f, 0x1.8000000000000p+1f };
extern const dm_vec4 dm_vec4_maxFloat = { 0x1.fffffe0000000p+127f, 0x1.fffffe0000000p+127f, 0x1.fffffe0000000p+127f, 0x1.fffffe0000000p+127f };
extern const dm_vec4 dm_vec4_linearSlop = { 0x1.0cc4320000000p-6f, 0x1.0cc4320000000p-6f, 0x1.0cc4320000000p-6f, 0x1.0cc4320000000p-6f };
extern const dm_vec4 dm_vec4_epsilon = { 0x1.0000000000000p-23f, 0x1.0000000000000p-23f, 0x1.0000000000000p-23f, 0x1.0000000000000p-23f };
extern const dm_vec4 dm_vec4_zeroSafe = { 0x1.f400000000000p-117f, 0x1.f400000000000p-117f, 0x1.f400000000000p-117f, 0x1.f400000000000p-117f };
extern const dm_vec4 dm_vec4_unitX = { 0x1.0000000000000p+0f, 0x0.0p+0f, 0x0.0p+0f, 0x0.0p+0f };
extern const dm_vec4 dm_vec4_unitY = { 0x0.0p+0f, 0x1.0000000000000p+0f, 0x0.0p+0f, 0x0.0p+0f };
extern const dm_vec4 dm_vec4_unitZ = { 0x0.0p+0f, 0x0.0p+0f, 0x1.0000000000000p+0f, 0x0.0p+0f };
extern const dm_vec4 dm_vec4_unitW = { 0x0.0p+0f, 0x0.0p+0f, 0x0.0p+0f, 0x1.0000000000000p+0f };
extern const dm_vec4 dm_vec4_pi = { 0x1.921fb60000000p+1f, 0x1.921fb60000000p+1f, 0x1.921fb60000000p+1f, 0x1.921fb60000000p+1f };
extern const dm_vec4 dm_vec4_2pi = { 0x1.921fb60000000p+2f, 0x1.921fb60000000p+2f, 0x1.921fb60000000p+2f, 0x1.921fb60000000p+2f };
extern const dm_vec4 dm_quat_identity = { 0x0.0p+0f, 0x0.0p+0f, 0x0.0p+0f, 0x1.0000000000000p+0f };
extern const dm_vec4 dm_vec4_finite4 = { __builtin_inff(), __builtin_inff(), __builtin_inff(), __builtin_inff() };

dm_vec4 dmSolveSym22(const dmMtx & A, dm_vec4Arg x)
{
    dm_vec4 Axx = dmSplatX(A.rx);
    dm_vec4 Axy = dmSplatY(A.rx);
    dm_vec4 Ayy = dmSplatY(A.ry);
    dm_vec4 det = dmSafeReciprocal(dmSub(dmMul(Axx, Ayy), dmMul(Axy, Axy)));
    dm_vec4 xx = dmSplatX(x);
    dm_vec4 xy = dmSplatY(x);
    dm_vec4 y = dmVecSet(dmSub(dmMul(Ayy, xx), dmMul(Axy, xy)), dmSub(dmMul(Axx, xy), dmMul(Axy, xx)), dmVecSet(0.0f));
    return dmMul(det, y);
}
dmMtx dmInvertSym22(const dmMtx & A)
{
    dm_vec4 Axx = dmSplatX(A.rx);
    dm_vec4 Axy = dmSplatY(A.rx);
    dm_vec4 Ayy = dmSplatY(A.ry);
    dm_vec4 det = dmSafeReciprocal(dmSub(dmMul(Axx, Ayy), dmMul(Axy, Axy)));
    dmMtx invA;
    Axy = dmNegate(dmMul(det, Axy));
    invA.rx = dmVecSet(dmMul(det, Ayy), Axy, dmVecSet(0.0f));
    invA.ry = dmVecSet(Axy, dmMul(det, Axx), dmVecSet(0.0f));
    invA.rz = dmVecSet(0.0f);
    return invA;
}
dmMtx dmInvertSym33(const dmMtx & A)
{
    dm_vec4 e12 = dmCross(A.rx, A.ry);
    dm_vec4 e23 = dmCross(A.ry, A.rz);
    dm_vec4 e31 = dmCross(A.rz, A.rx);
    dm_vec4 det = dmDot3(A.rx, e23);
    det = dmSafeReciprocal(det);
    dmMtx invA;
    invA.rx = dmMul(det, e23);
    invA.ry = dmMul(det, e31);
    invA.rz = dmMul(det, e12);
    return dmTranspose33(invA);
}
dm_vec4 dmSolveSym33(const dmMtx & A, dm_vec4Arg b)
{
    dm_vec4 det = dmScalarTripleProduct(A.rx, A.ry, A.rz);
    dm_vec4 invDet = dmSafeReciprocal(det);
    dm_vec4 x = dmMul(dmScalarTripleProduct(b, A.ry, A.rz), dm_vec4_unitX);
    x = dmAdd(x, dmMul(dmScalarTripleProduct(A.rx, b, A.rz), dm_vec4_unitY));
    x = dmAdd(x, dmMul(dmScalarTripleProduct(A.rx, A.ry, b), dm_vec4_unitZ));
    return dmMul(invDet, x);
}
dm_vec4 dmSolve33(const dmMtx & A, dm_vec4Arg b)
{
    dm_vec4 det = dmScalarTripleProduct(A.rx, A.ry, A.rz);
    dm_vec4 invDet = dmSafeReciprocal(det);
    dm_vec4 xx = dmScalarTripleProduct(b, A.ry, A.rz);
    dm_vec4 yy = dmScalarTripleProduct(A.rx, b, A.rz);
    dm_vec4 zz = dmScalarTripleProduct(A.rx, A.ry, b);
    dm_vec4 x = dmVecSet(xx, yy, zz);
    return dmMul(invDet, x);
}
dmMtx dmInvertOrtho34(const dmMtx & A)
{
    dmMtx invA = dmTranspose33(A);
    invA.p = dmNegate(dmMul33(invA, A.p));
    return invA;
}
