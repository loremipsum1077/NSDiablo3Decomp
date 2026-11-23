#include "dmDistance.h"
#include "../../Include/dmShapes.h"
#include "../../Include/dmPolytope.h"























void dmGJKProxy::Set(const dmShape * pPrimitive)
{
    switch (pPrimitive->m_type)
    {
    case dmShape::e_sphere:


        m_aVertices = &static_cast<const dmSphere *>(pPrimitive)->m_p;
        m_count = 1;
        break;


    case dmShape::e_capsule:


        m_aVertices = &static_cast<const dmCapsule *>(pPrimitive)->m_p1;
        m_count = 2;
        break;


    case dmShape::e_triangle:


        m_aVertices = static_cast<const dmTriangle *>(pPrimitive)->m_p;
        m_count = 3;
        break;


    case dmShape::e_polytope:


        m_aVertices = static_cast<const dmPolytope *>(pPrimitive)->GetVertices();
        m_count = static_cast<const dmPolytope *>(pPrimitive)->GetVertexCount();
        break;

    default:
#line 64 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Functions/dmDistance.cpp"
        dmAssert(false);
        break;
    }



    m_radius = dmV4(pPrimitive->m_radius);
}



struct dmSimplexVertex
{
    dm_vec4 w;
    dm_vec4 wA;
    dm_vec4 wB;
    dm_vec4 alpha;
    dm_int32 indexA;
    dm_int32 indexB;
};




struct dmSimplex
{

    dm_vec4 ComputeMetric() const
    {
        dm_vec4 metric = dmVecSet(0.0f);
        switch (count)
        {


        case 2: metric = dmLengthSqr3(dmSub(s2.w, s1.w)); break;




        case 3: metric = dmLengthSqr3(dmCross(dmSub(s3.w, s1.w), dmSub(s2.w, s1.w))); break;




        case 4: metric = dmDot3(dmSub(s4.w, s1.w), dmCross(dmSub(s3.w, s1.w), dmSub(s2.w, s1.w))); break;


        }
        return metric;
    }

    dm_vec4 ComputeClosest() const
    {
        switch (count)
        {

        case 1: return s1.w;


        case 2: return dmAdd(dmMul(s1.alpha, s1.w), dmMul(s2.alpha, s2.w));


        case 3: return dmAdd(dmAdd(dmMul(s1.alpha, s1.w), dmMul(s2.alpha, s2.w)), dmMul(s3.alpha, s3.w));


        case 4: return dmAdd(dmAdd(dmAdd(dmMul(s1.alpha, s1.w), dmMul(s2.alpha, s2.w)), dmMul(s3.alpha, s3.w)), dmMul(s4.alpha, s4.w));


        default: return dmVecSet(0.0f);
        }
    }





    void ComputeClosestAB(dm_vec4 * pA, dm_vec4 * pB) const
    {

        switch (count)
        {

        case 1: *pA = s1.wA; *pB = s1.wB; break;



        case 2:
            *pA = dmAdd(dmMul(s1.alpha, s1.wA), dmMul(s2.alpha, s2.wA));
            *pB = dmAdd(dmMul(s1.alpha, s1.wB), dmMul(s2.alpha, s2.wB));
            break;

        case 3:
            *pA = dmAdd(dmAdd(dmMul(s1.alpha, s1.wA), dmMul(s2.alpha, s2.wA)), dmMul(s3.alpha, s3.wA));
            *pB = dmAdd(dmAdd(dmMul(s1.alpha, s1.wB), dmMul(s2.alpha, s2.wB)), dmMul(s3.alpha, s3.wB));
            break;

        case 4:
            *pA = dmAdd(dmAdd(dmAdd(dmMul(s1.alpha, s1.wA), dmMul(s2.alpha, s2.wA)), dmMul(s3.alpha, s3.wA)), dmMul(s4.alpha, s4.wA));
            *pB = *pA;
            break;






        default: *pA = *pB = dmVecSet(0.0f); break;
        }



    }

    dmSimplexVertex s1, s2, s3, s4;
    dm_int32 count;
};
static_assert(sizeof(dmSimplex) == 0x150, "dmSimplex");












static bool dmProcessSegment(dm_vec4 * pDirection, dmSimplex * pSimplex)
{














    dm_vec4 w1 = pSimplex->s1.w;
    dm_vec4 w2 = pSimplex->s2.w;
    dm_vec4 e = dmSub(w2, w1);


    dm_vec4 d12_2 = dmNegate(dmDot3(w1, e));
    if (dmLE(d12_2, dmVecSet(0.0f)))
    {
        pSimplex->s1.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        *pDirection = dmNegate(w1);
        return true;
    }


    dm_vec4 d12_1 = dmDot3(w2, e);
    if (dmLE(d12_1, dmVecSet(0.0f)))
    {
        pSimplex->s2.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        pSimplex->s1 = pSimplex->s2;
        *pDirection = dmNegate(w2);
        return true;
    }


    dm_vec4 d = dmAdd(d12_1, d12_2);
    if (dmLE(d, dmVecSet(0.0f))) return false;



    d = dmSafeReciprocal(d);
    pSimplex->s1.alpha = dmMul(d12_1, d);
    pSimplex->s2.alpha = dmMul(d12_2, d);
    pSimplex->count = 2;
    *pDirection = dmCross(dmCross(dmAdd(w1, w2), e), e);
    return true;
}







static bool dmProcessTriangle(dm_vec4 * pDirection, dmSimplex * pSimplex)
{

    dm_vec4 w1 = pSimplex->s1.w;
    dm_vec4 w2 = pSimplex->s2.w;
    dm_vec4 w3 = pSimplex->s3.w;




    dm_vec4 e12 = dmSub(w2, w1);
    dm_vec4 w1e12 = dmDot3(w1, e12);
    dm_vec4 w2e12 = dmDot3(w2, e12);
    dm_vec4 d12_1 = w2e12;
    dm_vec4 d12_2 = dmNegate(w1e12);




    dm_vec4 e13 = dmSub(w3, w1);
    dm_vec4 w1e13 = dmDot3(w1, e13);
    dm_vec4 w3e13 = dmDot3(w3, e13);
    dm_vec4 d13_1 = w3e13;
    dm_vec4 d13_2 = dmNegate(w1e13);




    dm_vec4 e23 = dmSub(w3, w2);
    dm_vec4 w2e23 = dmDot3(w2, e23);
    dm_vec4 w3e23 = dmDot3(w3, e23);
    dm_vec4 d23_1 = w3e23;
    dm_vec4 d23_2 = dmNegate(w2e23);



    dm_vec4 n123 = dmCross(dmSub(w2, w1), dmSub(w3, w1));


    dm_vec4 d123_1 = dmDot3(n123, dmCross(w2, w3));
    dm_vec4 d123_2 = dmDot3(n123, dmCross(w3, w1));
    dm_vec4 d123_3 = dmDot3(n123, dmCross(w1, w2));


    if (dmLEZ(d12_2) && dmLEZ(d13_2))
    {
        pSimplex->s1.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        *pDirection = dmNegate(w1);
        return true;
    }


    if (dmLEZ(d12_1) && dmLEZ(d23_2))
    {
        pSimplex->s2.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        pSimplex->s1 = pSimplex->s2;
        *pDirection = dmNegate(w2);
        return true;
    }


    if (dmLEZ(d13_1) && dmLEZ(d23_1))
    {
        pSimplex->s3.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        pSimplex->s1 = pSimplex->s3;
        *pDirection = dmNegate(w3);
        return true;
    }


    if (dmGTZ(d12_1) && dmGTZ(d12_2) && dmLEZ(d123_3))
    {
        dm_vec4 d = dmReciprocal(dmAdd(d12_1, d12_2));
        pSimplex->s1.alpha = dmMul(d12_1, d);
        pSimplex->s2.alpha = dmMul(d12_2, d);
        pSimplex->count = 2;
        *pDirection = dmCross(dmCross(dmAdd(w1, w2), e12), e12);
        return true;
    }


    if (dmGTZ(d13_1) && dmGTZ(d13_2) && dmLEZ(d123_2))
    {
        dm_vec4 d = dmReciprocal(dmAdd(d13_1, d13_2));
        pSimplex->s1.alpha = dmMul(d13_1, d);
        pSimplex->s3.alpha = dmMul(d13_2, d);
        pSimplex->s2 = pSimplex->s3;
        pSimplex->count = 2;
        *pDirection = dmCross(dmCross(dmAdd(w1, w3), e13), e13);
        return true;
    }


    if (dmGTZ(d23_1) && dmGTZ(d23_2) && dmLEZ(d123_1))
    {
        dm_vec4 d = dmReciprocal(dmAdd(d23_1, d23_2));
        pSimplex->s2.alpha = dmMul(d23_1, d);
        pSimplex->s3.alpha = dmMul(d23_2, d);
        pSimplex->s1 = pSimplex->s3;
        pSimplex->count = 2;
        *pDirection = dmCross(dmCross(dmAdd(w2, w3), e23), e23);
        return true;
    }


    dm_vec4 d = dmAdd(dmAdd(d123_1, d123_2), d123_3);
    if (dmLEZ(d)) return false;



    d = dmReciprocal(d);
    pSimplex->s1.alpha = dmMul(d123_1, d);
    pSimplex->s2.alpha = dmMul(d123_2, d);
    pSimplex->s3.alpha = dmMul(d123_3, d);
    pSimplex->count = 3;

    dm_vec4 n = n123;
    dm_vec4 sgn = -dmDot3(w1 + w2 + w3, n);
    *pDirection = dmSelect_GT(sgn, dmVecSet(0.0f), n, -n);




    return true;
}




static bool dmProcessTetrahedron(dm_vec4 * pDirection, dmSimplex * pSimplex)
{

    dm_vec4 w1 = pSimplex->s1.w;
    dm_vec4 w2 = pSimplex->s2.w;
    dm_vec4 w3 = pSimplex->s3.w;
    dm_vec4 w4 = pSimplex->s4.w;



    // Orient the tetrahedron so its face tests use consistent signs.
    dm_vec4 n4 = dmCross(dmSub(w2, w1), dmSub(w3, w1));
    dm_vec4 volume = dmDot3(dmSub(w1, w4), n4);
    if (dmLTZ(volume))
    {
        // rows: the swap's memcpys (dmMath.inl dmSwap) and w2/w3 trade registers after it (inferred).
        dmSwap(pSimplex->s2, pSimplex->s3);
        dmSwap(w2, w3);
    }




    dm_vec4 e12 = dmSub(w2, w1);
    dm_vec4 w1e12 = dmDot3(w1, e12);
    dm_vec4 w2e12 = dmDot3(w2, e12);
    dm_vec4 d12_1 = w2e12;
    dm_vec4 d12_2 = dmNegate(w1e12);




    dm_vec4 e13 = dmSub(w3, w1);
    dm_vec4 w1e13 = dmDot3(w1, e13);
    dm_vec4 w3e13 = dmDot3(w3, e13);
    dm_vec4 d13_1 = w3e13;
    dm_vec4 d13_2 = dmNegate(w1e13);




    dm_vec4 e14 = dmSub(w4, w1);
    dm_vec4 w1e14 = dmDot3(w1, e14);
    dm_vec4 w4e14 = dmDot3(w4, e14);
    dm_vec4 d14_1 = w4e14;
    dm_vec4 d14_2 = dmNegate(w1e14);




    dm_vec4 e23 = dmSub(w3, w2);
    dm_vec4 w2e23 = dmDot3(w2, e23);
    dm_vec4 w3e23 = dmDot3(w3, e23);
    dm_vec4 d23_1 = w3e23;
    dm_vec4 d23_2 = dmNegate(w2e23);




    dm_vec4 e24 = dmSub(w4, w2);
    dm_vec4 w2e24 = dmDot3(w2, e24);
    dm_vec4 w4e24 = dmDot3(w4, e24);
    dm_vec4 d24_1 = w4e24;
    dm_vec4 d24_2 = dmNegate(w2e24);




    dm_vec4 e34 = dmSub(w4, w3);
    dm_vec4 w3e34 = dmDot3(w3, e34);
    dm_vec4 w4e34 = dmDot3(w4, e34);
    dm_vec4 d34_1 = w4e34;
    dm_vec4 d34_2 = dmNegate(w3e34);





    dm_vec4 n123 = dmCross(dmSub(w2, w1), dmSub(w3, w1));
    dm_vec4 d123_1 = dmDot3(n123, dmCross(w2, w3));
    dm_vec4 d123_2 = dmDot3(n123, dmCross(w3, w1));
    dm_vec4 d123_3 = dmDot3(n123, dmCross(w1, w2));





    dm_vec4 n142 = dmCross(dmSub(w4, w1), dmSub(w2, w1));
    dm_vec4 d142_1 = dmDot3(n142, dmCross(w4, w2));
    dm_vec4 d142_2 = dmDot3(n142, dmCross(w2, w1));
    dm_vec4 d142_3 = dmDot3(n142, dmCross(w1, w4));





    dm_vec4 n134 = dmCross(dmSub(w3, w1), dmSub(w4, w1));
    dm_vec4 d134_1 = dmDot3(n134, dmCross(w3, w4));
    dm_vec4 d134_2 = dmDot3(n134, dmCross(w4, w1));
    dm_vec4 d134_3 = dmDot3(n134, dmCross(w1, w3));





    dm_vec4 n243 = dmCross(dmSub(w4, w2), dmSub(w3, w2));
    dm_vec4 d243_1 = dmDot3(n243, dmCross(w4, w3));
    dm_vec4 d243_2 = dmDot3(n243, dmCross(w3, w2));
    dm_vec4 d243_3 = dmDot3(n243, dmCross(w2, w4));





    dm_vec4 d1234_1 = dmDot3(dmAdd(dmAdd(w2, w3), w4), n243);
    dm_vec4 d1234_2 = dmDot3(dmAdd(dmAdd(w1, w3), w4), n134);
    dm_vec4 d1234_3 = dmDot3(dmAdd(dmAdd(w1, w4), w2), n142);
    dm_vec4 d1234_4 = dmDot3(dmAdd(dmAdd(w1, w2), w3), n123);


    if (dmLEZ(d12_2) && dmLEZ(d13_2) && dmLEZ(d14_2))
    {
        pSimplex->s1.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        *pDirection = dmNegate(w1);
        return true;
    }


    if (dmLEZ(d12_1) && dmLEZ(d23_2) && dmLEZ(d24_2))
    {
        pSimplex->s2.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        pSimplex->s1 = pSimplex->s2;
        *pDirection = dmNegate(w2);
        return true;
    }


    if (dmLEZ(d13_1) && dmLEZ(d23_1) && dmLEZ(d34_2))
    {
        pSimplex->s3.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        pSimplex->s1 = pSimplex->s3;
        *pDirection = dmNegate(w3);
        return true;
    }


    if (dmLEZ(d14_1) && dmLEZ(d24_1) && dmLEZ(d34_1))
    {
        pSimplex->s4.alpha = dmVecSet(1.0f);
        pSimplex->count = 1;
        pSimplex->s1 = pSimplex->s4;
        *pDirection = dmNegate(w4);
        return true;
    }


    if (dmGTZ(d12_1) && dmGTZ(d12_2) && dmLEZ(d123_3) && dmLEZ(d142_2))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(d12_1, d12_2));
        pSimplex->s1.alpha = dmMul(d12_1, d);
        pSimplex->s2.alpha = dmMul(d12_2, d);
        pSimplex->count = 2;
        *pDirection = dmCross(dmCross(dmAdd(w1, w2), e12), e12);
        return true;
    }


    if (dmGTZ(d13_1) && dmGTZ(d13_2) && dmLEZ(d123_2) && dmLEZ(d134_3))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(d13_1, d13_2));
        pSimplex->s1.alpha = dmMul(d13_1, d);
        pSimplex->s3.alpha = dmMul(d13_2, d);
        pSimplex->count = 2;
        pSimplex->s2 = pSimplex->s3;
        *pDirection = dmCross(dmCross(dmAdd(w1, w3), e13), e13);
        return true;
    }


    if (dmGTZ(d14_1) && dmGTZ(d14_2) && dmLEZ(d142_3) && dmLEZ(d134_2))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(d14_1, d14_2));
        pSimplex->s1.alpha = dmMul(d14_1, d);
        pSimplex->s4.alpha = dmMul(d14_2, d);
        pSimplex->count = 2;
        pSimplex->s2 = pSimplex->s4;
        *pDirection = dmCross(dmCross(dmAdd(w1, w4), e14), e14);
        return true;
    }


    if (dmGTZ(d23_1) && dmGTZ(d23_2) && dmLEZ(d123_1) && dmLEZ(d243_2))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(d23_1, d23_2));
        pSimplex->s2.alpha = dmMul(d23_1, d);
        pSimplex->s3.alpha = dmMul(d23_2, d);
        pSimplex->count = 2;
        pSimplex->s1 = pSimplex->s3;
        *pDirection = dmCross(dmCross(dmAdd(w2, w3), e23), e23);
        return true;
    }


    if (dmGTZ(d24_1) && dmGTZ(d24_2) && dmLEZ(d142_1) && dmLEZ(d243_3))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(d24_1, d24_2));
        pSimplex->s2.alpha = dmMul(d24_1, d);
        pSimplex->s4.alpha = dmMul(d24_2, d);
        pSimplex->count = 2;
        pSimplex->s1 = pSimplex->s4;
        *pDirection = dmCross(dmCross(dmAdd(w2, w4), e24), e24);
        return true;
    }


    if (dmGTZ(d34_1) && dmGTZ(d34_2) && dmLEZ(d134_1) && dmLEZ(d243_1))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(d34_1, d34_2));
        pSimplex->s3.alpha = dmMul(d34_1, d);
        pSimplex->s4.alpha = dmMul(d34_2, d);
        pSimplex->count = 2;
        pSimplex->s1 = pSimplex->s3;
        pSimplex->s2 = pSimplex->s4;
        *pDirection = dmCross(dmCross(dmAdd(w3, w4), e34), e34);
        return true;
    }


    if (dmLEZ(d1234_1) && dmGTZ(d243_1) && dmGTZ(d243_2) && dmGTZ(d243_3))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(dmAdd(d243_1, d243_2), d243_3));
        pSimplex->s2.alpha = dmMul(d243_1, d);
        pSimplex->s4.alpha = dmMul(d243_2, d);
        pSimplex->s3.alpha = dmMul(d243_3, d);
        pSimplex->count = 3;
        pSimplex->s1 = pSimplex->s4;

        dm_vec4 n = n243;
        dm_vec4 sgn = -dmDot3(w4 + w2 + w3, n);
        *pDirection = dmSelect_GT(sgn, dmVecSet(0.0f), n, -n);
        return true;
    }


    if (dmLEZ(d1234_2) && dmGTZ(d134_1) && dmGTZ(d134_2) && dmGTZ(d134_3))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(dmAdd(d134_1, d134_2), d134_3));
        pSimplex->s1.alpha = dmMul(d134_1, d);
        pSimplex->s3.alpha = dmMul(d134_2, d);
        pSimplex->s4.alpha = dmMul(d134_3, d);
        pSimplex->count = 3;
        pSimplex->s2 = pSimplex->s4;

        dm_vec4 n = n134;
        dm_vec4 sgn = -dmDot3(w1 + w4 + w3, n);
        *pDirection = dmSelect_GT(sgn, dmVecSet(0.0f), n, -n);
        return true;
    }


    if (dmLEZ(d1234_3) && dmGTZ(d142_1) && dmGTZ(d142_2) && dmGTZ(d142_3))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(dmAdd(d142_1, d142_2), d142_3));
        pSimplex->s1.alpha = dmMul(d142_1, d);
        pSimplex->s4.alpha = dmMul(d142_2, d);
        pSimplex->s2.alpha = dmMul(d142_3, d);
        pSimplex->count = 3;
        pSimplex->s3 = pSimplex->s4;

        dm_vec4 n = n142;
        dm_vec4 sgn = -dmDot3(w1 + w2 + w4, n);
        *pDirection = dmSelect_GT(sgn, dmVecSet(0.0f), n, -n);
        return true;
    }


    if (dmLEZ(d1234_4) && dmGTZ(d123_1) && dmGTZ(d123_2) && dmGTZ(d123_3))
    {
        dm_vec4 d = dmDiv(dm_vec4_one, dmAdd(dmAdd(d123_1, d123_2), d123_3));
        pSimplex->s1.alpha = dmMul(d123_1, d);
        pSimplex->s2.alpha = dmMul(d123_2, d);
        pSimplex->s3.alpha = dmMul(d123_3, d);
        pSimplex->count = 3;

        dm_vec4 n = n123;
        dm_vec4 sgn = -dmDot3(w1 + w2 + w3, n);
        *pDirection = dmSelect_GT(sgn, dmVecSet(0.0f), n, -n);
        return true;
    }


    dm_vec4 d = dmAdd(dmAdd(dmAdd(d1234_1, d1234_2), d1234_3), d1234_4);
    if (dmLEZ(d)) return false;



    d = dmDiv(dm_vec4_one, d);
    pSimplex->s1.alpha = dmMul(d1234_1, d);
    pSimplex->s2.alpha = dmMul(d1234_2, d);
    pSimplex->s3.alpha = dmMul(d1234_3, d);
    pSimplex->s4.alpha = dmMul(d1234_4, d);
    pSimplex->count = 4;
    *pDirection = dmVecSet(0.0f);
    return true;
}
// NON_MATCHING: 10424 vs 10596 bytes (same 24 calls, 33 vertex copies); simplex arithmetic recovered,
// remaining: scheduling/register allocation of the inlined helpers (mnemonic similarity .858).
void dmDistance(dmDistanceOutput * pOutput, const dmDistanceInput * pInput)
{






    dmMtx Ta = dmMtxFromTransform(pInput->transformA);
    dmMtx Tb = dmMtxFromTransform(pInput->transformB);
    dmMtx RaT = dmTranspose33(Ta);
    dmMtx RbT = dmTranspose33(Tb);

    dmGJKProxy proxyA = pInput->proxyA;
    dmGJKProxy proxyB = pInput->proxyB;


    dmSimplex simplex;
    const dmGJKCache * pCache = &pInput->cache;
    simplex.count = pCache->GetCount();

    dmSimplexVertex * pVertices = &simplex.s1;
    for (dm_int32 i = 0; i < simplex.count; ++i)
    {
        dmSimplexVertex * pVertex = pVertices + i;
        pVertex->indexA = pCache->indexAs[i];
        pVertex->indexB = pCache->indexBs[i];
        dm_vec4 wALocal = proxyA.GetVertex(pVertex->indexA);
        dm_vec4 wBLocal = proxyB.GetVertex(pVertex->indexB);

        pVertex->wA = dmMul34(Ta, wALocal);
        pVertex->wB = dmMul34(Tb, wBLocal);
        pVertex->w = dmSub(pVertex->wB, pVertex->wA);
    }


    if (simplex.count > 1)
    {
        dm_float32 metric1 = pCache->metric;
        dm_float32 metric2 = dmF32(simplex.ComputeMetric());
        if (!(0.25f * metric1 <= metric2) || 4.0f * metric1 < metric2 || metric2 < 1.1920929e-7f)


            simplex.count = 0;
    }


    if (simplex.count == 0)
    {
        dmSimplexVertex * pVertex = pVertices;
        pVertex->indexA = pVertex->indexB = 0;

        dm_vec4 wALocal = proxyA.GetVertex(0);
        dm_vec4 wBLocal = proxyB.GetVertex(0);
        pVertex->wA = dmMul34(Ta, wALocal);
        pVertex->wB = dmMul34(Tb, wBLocal);
        pVertex->w = dmSub(pVertex->wB, pVertex->wA);
        pVertex->alpha = dmVecSet(1.0f);
        simplex.count = 1;
    }


    const dm_int32 k_historyCapacity = 4;
    dm_int32 historyA[k_historyCapacity], historyB[k_historyCapacity];
    dm_int32 historyCount;





    dmSimplex simplex1 = simplex;
    dm_vec4 distanceSqr1 = dm_vec4_maxFloat;
    dm_vec4 distanceSqr2 = dmVecSet(0.0f);
    dm_int32 iter = 0;
    pOutput->exitCode = 0;




    for (;;)
    {




        historyCount = simplex.count;
        for (dm_int32 i = 0; i < historyCount; ++i)
        {
            historyA[i] = pVertices[i].indexA;
            historyB[i] = pVertices[i].indexB;
        }


        bool success = true;
        dm_vec4 direction;
        switch (simplex.count)
        {

        case 1: direction = dmNegate(simplex.s1.w); break;
        case 2: success = dmProcessSegment(&direction, &simplex); break;
        case 3: success = dmProcessTriangle(&direction, &simplex); break;
        case 4: success = dmProcessTetrahedron(&direction, &simplex); break;











        default:
#line 811 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Functions/dmDistance.cpp"
            dmAssert(false);
            direction = dmVecSet(0.0f);
            break;
        }
        if (!success)
        {
            pOutput->exitCode = 6;
            distanceSqr2 = distanceSqr1;
            simplex = simplex1;
            break;
        }


        dm_vec4 p = simplex.ComputeClosest();
        distanceSqr2 = dmLengthSqr3(p);





























        if (dmLE(distanceSqr1, distanceSqr2))
        {
            pOutput->exitCode = 5;
            simplex = simplex1;
            break;
        }



        if (simplex.count == 4)
        {
            pOutput->exitCode = 4;
            pOutput->normal = dmVecSet(0.0f);
            distanceSqr2 = dmVecSet(0.0f);
            break;
        }


        if (dmIsZeroSafe3(direction))
        {
            pOutput->exitCode = 3;
            simplex = simplex1;
            break;
        }



        pOutput->normal = dmNegate(direction);


        dmSimplexVertex * pVertex = pVertices + simplex.count;
        dm_vec4 wALocal;
        pVertex->indexA = proxyA.GetSupport(&wALocal, dmMul33(RaT, dmNegate(direction)));
        pVertex->wA = dmMul34(Ta, wALocal);
        dm_vec4 wBLocal;
        pVertex->indexB = proxyB.GetSupport(&wBLocal, dmMul33(RbT, direction));
        pVertex->wB = dmMul34(Tb, wBLocal);
        pVertex->w = dmSub(pVertex->wB, pVertex->wA);


        ++iter;


        bool foundDuplicate = false;
        for (dm_int32 i = 0; i < historyCount; ++i)

            if (pVertex->indexA == historyA[i] && pVertex->indexB == historyB[i])
            { foundDuplicate = true; break; }














        if (foundDuplicate) { pOutput->exitCode = 2; break; }







        if (iter == 20) { pOutput->exitCode = 0; pOutput->normal = dmNegate(direction); break; }


        simplex1 = simplex;
        distanceSqr1 = distanceSqr2;
        ++simplex.count;
    }










    simplex.ComputeClosestAB(&pOutput->pointA, &pOutput->pointB);
    pOutput->distance = dmSqrt(distanceSqr2);
    pOutput->iterationCount = iter;
    pOutput->cache.Clear();

    for (dm_int32 i = 0; i < simplex.count; ++i)
    {
        dmSimplexVertex * pVertex = pVertices + i;


        pOutput->cache.indexAs[i] = pVertex->indexA;
        pOutput->cache.indexBs[i] = pVertex->indexB;
    }


    pOutput->cache.metric = dmF32(simplex.ComputeMetric());

    if (!dmIsZeroSafe3(pOutput->normal)) pOutput->normal = dmNormalize3(pOutput->normal);
    else pOutput->normal = dmVecSet(0.0f);







    if (pInput->useRadii)
    {
        dm_vec4 rA = proxyA.GetRadius();
        dm_vec4 rB = proxyB.GetRadius();

        if (dmGT(pOutput->distance, dmAdd(rA, rB)) && dmGT(pOutput->distance, dm_vec4_epsilon))
        {
            pOutput->distance = dmSub(pOutput->distance, dmAdd(rA, rB));
            pOutput->pointA = dmAdd(pOutput->pointA, dmMul(rA, pOutput->normal));
            pOutput->pointB = dmSub(pOutput->pointB, dmMul(rB, pOutput->normal));
        }
        else
        {

            pOutput->distance = dmVecSet(0.0f);
            pOutput->pointB = pOutput->pointA;
        }
    }
}


inline dm_vec4 dmDistance(dm_vec4 * pPointA, dm_vec4 * pPointB, const dmSphere * pSphereA, const dmSphere * pSphereB)

{
    dm_vec4 a = pSphereA->m_p;
    dm_vec4 b = pSphereB->m_p;

    dm_vec4 d = dmSub(b, a);
    dm_vec4 dd = dmDot3(d, d);
    dm_vec4 rA = dmVecSet(pSphereA->m_radius);
    dm_vec4 rB = dmVecSet(pSphereB->m_radius);
    dm_vec4 r = dmAdd(rA, rB);

    if (!dmIsZeroSafe3(d))
    {
        if (dmGT(dd, dmMul(r, r)))
        {
            dm_vec4 distance = dmLength3(d);
            d = dmNormalize3(d);
            distance = dmSub(distance, r);
            *pPointA = dmAdd(a, dmMul(rA, d));
            *pPointB = dmSub(b, dmMul(rB, d));
            return distance;
        }


        d = dmNormalize3(d);
        dm_vec4 mid = dmMul(dm_vec4_oneHalf, dmAdd(dmAdd(b, a), dmMul(d, dmSub(rA, rB))));
        *pPointA = mid;
        *pPointB = mid;
        return dmVecSet(0.0f);
    }

    *pPointA = a;
    *pPointB = a;
    return dmVecSet(0.0f);
}



dm_vec4 dmDistance(dm_vec4 * pPointA, dm_vec4 * pPointB, const dmCapsule * pCapsuleA, const dmSphere * pSphereB)
{
    dm_vec4 a1 = pCapsuleA->m_p1;
    dm_vec4 a2 = pCapsuleA->m_p2;
    dm_vec4 b = pSphereB->m_p;

    dm_vec4 d = dmSub(a2, a1);


    dm_vec4 alpha = dmDot3(d, dmSub(b, a1));

    dm_vec4 closestPoint;

    if (dmLEZ(alpha)) closestPoint = a1;
    else
    {




        dm_vec4 dd = dmDot3(d, d);


        if (dmGE(alpha, dd)) closestPoint = a2;
        else
        {





            alpha = dmDiv(alpha, dd);
            closestPoint = dmAdd(a1, dmMul(alpha, d));
        }
    }


    dmSphere sphereA;
    sphereA.m_p = closestPoint;
    sphereA.m_radius = pCapsuleA->m_radius;

    return dmDistance(pPointA, pPointB, &sphereA, pSphereB);
}



dm_vec4 dmDistance(dm_vec4 * pPointA, dm_vec4 * pPointB, const dmCapsule * pCapsuleA, const dmCapsule * pCapsuleB)

{
    dm_vec4 s1 = pCapsuleA->m_p1, t1 = pCapsuleA->m_p2;
    dm_vec4 s2 = pCapsuleB->m_p1, t2 = pCapsuleB->m_p2;



    dm_vec4 d1 = dmSub(t1, s1), d2 = dmSub(t2, s2);

    dm_vec4 c = dmSub(s1, s2);
    dm_vec4 d11 = dmDot3(d1, d1), d22 = dmDot3(d2, d2);


    bool safe1 = dmIsZeroSafe3(d11), safe2 = dmIsZeroSafe3(d22);


    if (safe1 && safe2)
    {
        *pPointA = s1;
        *pPointB = s2;
        return dmSqrt(dmDot3(c, c));
    }
    dm_vec4 cd2 = dmDot3(c, d2);
    dm_vec4 a1 = dmVecSet(0.0f), a2 = dmVecSet(0.0f);


    if (safe1)


        a2 = dmClamp(dmDiv(cd2, d22), dmVecSet(0.0f), dm_vec4_one);
    else
    {

        dm_vec4 cd1 = dmDot3(c, d1);

        if (safe2)
            a1 = dmClamp(dmDiv(dmNegate(cd1), d11), dmVecSet(0.0f), dm_vec4_one);
        else
        {




            dm_vec4 d12 = dmDot3(d1, d2);
            dm_vec4 denominator = dmSub(dmMul(d11, d22), dmMul(d12, d12));


            if (!dmIsZeroSafe3(denominator))

                a1 = dmClamp(dmDiv(dmSub(dmMul(cd2, d12), dmMul(cd1, d22)), denominator), dmVecSet(0.0f), dm_vec4_one);







            a2 = dmDiv(dmAdd(cd2, dmMul(d12, a1)), d22);


            if (dmLTZ(a2))
            {
                a2 = dmVecSet(0.0f);
                a1 = dmClamp(dmDiv(dmNegate(cd1), d11), dmVecSet(0.0f), dm_vec4_one);
            }
            else if (dmGT(a2, dm_vec4_one))
            {
                a2 = dm_vec4_one;
                a1 = dmClamp(dmDiv(dmSub(d12, cd1), d11), dmVecSet(0.0f), dm_vec4_one);
            }
        }
    }

    dmSphere sphereA;
    sphereA.m_p = dmAdd(s1, dmMul(d1, a1));
    sphereA.m_radius = pCapsuleA->m_radius;
    dmSphere sphereB;
    sphereB.m_p = dmAdd(s2, dmMul(d2, a2));
    sphereB.m_radius = pCapsuleB->m_radius;
    return dmDistance(pPointA, pPointB, &sphereA, &sphereB);
}
