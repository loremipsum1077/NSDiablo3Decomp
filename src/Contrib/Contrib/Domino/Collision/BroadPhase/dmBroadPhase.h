// Adopted from DWARF; supported original interfaces.
// Original file: Contrib/Contrib/Domino/Collision/BroadPhase/dmBroadPhase.h

#pragma once

#include "dmDynamicTree.h"
#include "../../Common/dmArray.h"
#include "../../Include/dmCollision.h"
#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

struct dmBroadPhaseCallback;
struct dmRayCastAnyCallback;
struct dmRayCastClosestCallback;
struct dmWorld;

struct dmPair {  // size 0x8
    dm_int32 proxyIdA;  // 0x00
    dm_int32 proxyIdB;  // 0x04
};
static_assert(sizeof(dmPair) == 0x8, "dmPair");

class dmBroadPhase {  // size 0x50
public:
    void Create();
    void Destroy();
    dm_int32 CreateProxy(const dmAABB & aabb, dm_int32 userIndex);
    void DestroyProxy(dm_int32 proxyId);
    void MoveProxy(dm_int32 proxyId, const dmAABB & aabb);
    void TouchProxy(dm_int32 proxyId);
    dmAABB GetFatAABB(dm_int32 proxyId) const;
    dm_int32 GetUserIndex(dm_int32);
#line 136
    bool TestOverlap(dm_int32 proxyIdA, dm_int32 proxyIdB) const
    {
        const dmAABB & aabbA = m_tree.GetFatAABB(proxyIdA);
        const dmAABB & aabbB = m_tree.GetFatAABB(proxyIdB);
        return dmTestOverlap(aabbA, aabbB);
    }
#line 34
    dm_int32 GetProxyCount() const;
    dm_int32 GetHeight() const;
    dm_float32 GetAreaRatio() const;
    void ShiftOrigin(dm_vec4Arg origin);
    dm_int32 GetBytes() const;
private:
    friend class dmDynamicTree;
    void UnBufferMove(dm_int32 proxyId);
    void QueryCallback(dm_int32 proxyId);
    dmDynamicTree m_tree;  // 0x00
    dmArray<int> m_moveBuffer;  // 0x28
    dmArray<dmPair> m_pairBuffer;  // 0x38
    dm_int32 m_proxyCount;  // 0x48
    dm_int32 m_queryProxyId;  // 0x4c
public:
    template <typename T> void UpdatePairs(T * callback); // DWARF dmBroadPhase.h:165
    template <typename T> void QueryAABB(T * callback, const dmAABB & aabb) const; // DWARF :268
    template <typename T> void RayCast(T * callback, const dmRayCastInput * pInput) const; // DWARF :292
    // template instantiation: void RayCast<dmRayCastAnyCallback>(dmRayCastAnyCallback *, const dmRayCastInput *) const;
    // template instantiation: void RayCast<dmBroadPhaseCallback>(dmBroadPhaseCallback *, const dmRayCastInput *) const;
};
static_assert(sizeof(dmBroadPhase) == 0x50, "dmBroadPhase");

// Bodies. The #line directives put each one on its original line.
#include <algorithm>

// Defined at dmBroadPhase.cpp:16; the DWARF does not say which header declares them.
extern dm_int32 dm_moveCallCount;
extern dm_int32 dm_moveMaxCount;
extern dm_int32 dm_moveTotalCount;

#line 116 "../../..\\Domino/Collision/BroadPhase/dmBroadPhase.h"
inline bool dmPairLessThan(const dmPair & pair1, const dmPair & pair2)
{
    if (pair1.proxyIdA < pair2.proxyIdA)
    {
        return true;
    }

    if (pair1.proxyIdA == pair2.proxyIdA)
    {
        return pair1.proxyIdB < pair2.proxyIdB;
    }

    return false;
}

#line 165 "../../..\\Domino/Collision/BroadPhase/dmBroadPhase.h"
template <typename T> void dmBroadPhase::UpdatePairs(T * callback)
{

    dm_int32 iterations = 4;
    m_tree.Optimize(iterations);


    m_pairBuffer.SetCount(0);


    ++dm_moveCallCount;
    dm_moveMaxCount = dmMax(dm_moveMaxCount, m_moveBuffer.GetCount());
    dm_moveTotalCount += m_moveBuffer.GetCount();



    for (dm_int32 i = 0; i < m_moveBuffer.GetCount(); ++i)
    {
        m_queryProxyId = m_moveBuffer[i];
        if (m_queryProxyId == -1)
        {
            continue;
        }



        const dmAABB & fatAABB = m_tree.GetFatAABB(m_queryProxyId);


        m_tree.QueryAABB(this, fatAABB);
    }


    m_moveBuffer.SetCount(0);


    dm_int32 pairCount = m_pairBuffer.GetCount();
    dmPair * pPairs = m_pairBuffer.GetPointer();
    std::sort(pPairs, pPairs + pairCount, dmPairLessThan);


    dm_int32 i = 0;
    while (i < pairCount)
    {
        const dmPair & primaryPair = m_pairBuffer[i];
        dm_uint64 userDataA = m_tree.GetUserData(primaryPair.proxyIdA);
        dm_uint64 userDataB = m_tree.GetUserData(primaryPair.proxyIdB);

        callback->AddPair(userDataA, userDataB);
        ++i;


        while (i < pairCount)
        {
            const dmPair & pair = m_pairBuffer[i];
            if (pair.proxyIdA != primaryPair.proxyIdA || pair.proxyIdB != primaryPair.proxyIdB)
            {
                break;
            }
            ++i;
        }
    }
}

#line 230 "../../..\\Domino/Collision/BroadPhase/dmBroadPhase.h"
inline void dmBroadPhase::QueryCallback(dm_int32 proxyId)
{

    if (proxyId == m_queryProxyId)
    {
        return;
    }

    dmPair pair;
    if (proxyId < m_queryProxyId)
    {
        pair.proxyIdA = proxyId;
        pair.proxyIdB = m_queryProxyId;
    }
    else
    {
        pair.proxyIdA = m_queryProxyId;
        pair.proxyIdB = proxyId;
    }

    m_pairBuffer.PushBack(pair);
}

#line 255 "../../..\\Domino/Collision/BroadPhase/dmBroadPhase.h"
template <typename T> struct dmBroadPhaseQueryWrapper
{
    void QueryCallback(dm_int32 proxyId)
    {
        callback->QueryCallback((dm_int32)tree->GetUserData(proxyId));
    }

    const dmDynamicTree * tree;
    T * callback;
};



template <typename T> inline void dmBroadPhase::QueryAABB(T * callback, const dmAABB & aabb) const
{
    dmBroadPhaseQueryWrapper<T> wrapper;
    wrapper.tree = &m_tree;
    wrapper.callback = callback;

    m_tree.QueryAABB(&wrapper, aabb);
}



template <typename T> struct dmBroadPhaseRayCastWrapper
{
    dmCollisionCode RayCastCallback(dm_vec4 * pLambda, const dmRayCastInput & input, dm_int32 proxyId)
    {
        return callback->RayCastCallback(pLambda, input, (dm_int32)tree->GetUserData(proxyId));
    }

    const dmDynamicTree * tree;
    T * callback;
};



template <typename T> inline void dmBroadPhase::RayCast(T * callback, const dmRayCastInput * pInput) const
{
    dmBroadPhaseRayCastWrapper<T> wrapper;
    wrapper.tree = &m_tree;
    wrapper.callback = callback;

    m_tree.RayCast(&wrapper, pInput);
}


// Templates. The DWARF only has instantiations; one of each is shown.
#if 0
// dmBroadPhaseQueryWrapper (line 255): 1 instantiation(s); shown with T = dmBroadPhaseCallback
struct dmBroadPhaseQueryWrapper<dmBroadPhaseCallback> {  // size 0x10
    void QueryCallback(dm_int32 proxyId);
    const dmDynamicTree * tree;  // 0x00
    dmBroadPhaseCallback * callback;  // 0x08
};

// dmBroadPhaseRayCastWrapper (line 279): 3 instantiation(s); shown with T = dmBroadPhaseCallback
struct dmBroadPhaseRayCastWrapper<dmBroadPhaseCallback> {  // size 0x10
    dmCollisionCode RayCastCallback(dm_vec4 *, const dmRayCastInput &, dm_int32);
    const dmDynamicTree * tree;  // 0x00
    dmBroadPhaseCallback * callback;  // 0x08
};

#endif

// Functions defined in this file (bodies are not in the DWARF):
//   line   116  bool dmPairLessThan(const dmPair & pair1, const dmPair & pair2)  [0x7100bc6970]
//   line   136  bool dmBroadPhase::TestOverlap(dm_int32 proxyIdA, dm_int32 proxyIdB) const  [inlined into 1 functions]
//   line   143  dmAABB dmBroadPhase::GetFatAABB(dm_int32 proxyId) const  [inline]
//   line   148  dm_int32 dmBroadPhase::GetProxyCount() const  [inline]
//   line   153  dm_int32 dmBroadPhase::GetHeight() const  [inline]
//   line   158  dm_float32 dmBroadPhase::GetAreaRatio() const  [inline]
//   line   165  void dmBroadPhase::UpdatePairs<dmWorld>(dmWorld * callback)  [0x7100bc6540]
//   line   230  void dmBroadPhase::QueryCallback(dm_int32 proxyId)  [inlined into 1 functions]
//   line   257  void dmBroadPhaseQueryWrapper<dmBroadPhaseCallback>::QueryCallback(dm_int32 proxyId)  [0x7100bccdd0]
//   line   268  void dmBroadPhase::QueryAABB<dmBroadPhaseCallback>(dmBroadPhaseCallback * callback, const dmAABB & aabb) const  [inlined into 1 functions]
//   line   281  dmCollisionCode dmBroadPhaseRayCastWrapper<dmRayCastClosestCallback>::RayCastCallback(dm_vec4 * pLambda, const dmRayCastInput & input, dm_int32 proxyId)  [inlined into 1 functions]
//   line   292  void dmBroadPhase::RayCast<dmRayCastClosestCallback>(dmRayCastClosestCallback * callback, const dmRayCastInput * pInput) const  [inlined into 1 functions]
//   line   302  void dmBroadPhase::ShiftOrigin(dm_vec4Arg origin)  [inline]
