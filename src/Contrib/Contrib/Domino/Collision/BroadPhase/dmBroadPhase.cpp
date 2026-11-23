// Original file: Contrib/Contrib/Domino/Collision/BroadPhase/dmBroadPhase.cpp
#include "dmBroadPhase.h"
dm_int32 dm_moveCallCount, dm_moveMaxCount, dm_moveTotalCount;  // line 16
void dmBroadPhase::Create() {
    m_proxyCount = 0;
    m_tree.Create();
    m_moveBuffer.Create();
    m_pairBuffer.Create();
    m_moveBuffer.SetCapacity(128);
    m_pairBuffer.SetCapacity(128);
}
void dmBroadPhase::Destroy() {
    m_tree.Destroy(); m_moveBuffer.Destroy(); m_pairBuffer.Destroy();
}
dm_int32 dmBroadPhase::CreateProxy(const dmAABB & aabb, dm_int32 userIndex) {
    dm_int32 proxyId = m_tree.CreateProxy(aabb, userIndex);
    ++m_proxyCount;
    m_moveBuffer.PushBack(proxyId);
    return proxyId;
}
inline void dmBroadPhase::UnBufferMove(dm_int32 proxyId) {
    for (dm_int32 i = 0; i < m_moveBuffer.GetCount(); ++i)
        if (m_moveBuffer[i] == proxyId) m_moveBuffer[i] = -1;
}
void dmBroadPhase::DestroyProxy(dm_int32 proxyId) {
    UnBufferMove(proxyId); --m_proxyCount; m_tree.DestroyProxy(proxyId);
}
void dmBroadPhase::MoveProxy(dm_int32 proxyId, const dmAABB & aabb) {
    bool buffer = m_tree.MoveProxy(proxyId, aabb, dmVecSet(0.0f));
    if (buffer) m_moveBuffer.PushBack(proxyId);
}
void dmBroadPhase::TouchProxy(dm_int32 proxyId) {
    if (proxyId != -1) m_moveBuffer.PushBack(proxyId);
}
