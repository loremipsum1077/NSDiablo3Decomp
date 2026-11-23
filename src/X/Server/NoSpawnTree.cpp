// Original owner: X/Server/NoSpawnTree.cpp. Bodies reconstructed from DWARF
// inline trees and original line-mapped instructions, not decompiler types.
#include "NoSpawnTree.h"
#include "SWorld.h"
#include "../Common/SceneCommon.h"
#include "../Common/Definitions/SceneDefinition.h"
#include "../Common/Definitions/MarkerSetDefinition.h"
#include "../Sigma/Math/Rect.h"
#include <float.h>

// Original constant, NoSpawnTree.cpp:22; DWARF value and binary 0x42800000.
static const float NOSPAWN_MIN_WIDTH = 64.0f;

void NoSpawnNode::Clear() {
    for (int32 i = 0; i < NUM_CHILDREN; ++i)
        m_arChildren[i] = NULL;
    m_listCircles.SetPool(SWorldGetNoSpawnCirclePool());
}

void NoSpawnNode::AddCircle(const Circle & tCircle) {
    m_listCircles.AddTail_Tracked(tCircle);
}

void NoSpawnTree::Initialize() {
    m_ptTop = NULL;
    m_uCircleCount = 0;
    m_uNodeCount = 0;
    m_listNodes.SetPool(SWorldGetNoSpawnNodePool());
}

void NoSpawnTree::Populate(SWorld & tSWorld) {
    SceneCommonData * ptSCD = NULL;
    SceneCommonDataIterator tIterator;
    Vector2D vWorldMin(FLT_MAX, FLT_MAX);
    Vector2D vWorldMax(-FLT_MAX, -FLT_MAX);
    while (SceneCommonDataArrayIterate(tIterator, ptSCD, SkipHolodeckScenes)) {
        if (ptSCD->idSWorld != tSWorld.id)
            continue;
        AABB aabb;
        Vector3D vSceneMin, vSceneMax;
        SceneGetLogicalAABBInWorldSpace(*ptSCD, aabb);
        aabb.ComputeBounds(vSceneMin, vSceneMax);
        vWorldMin.x = Math::Min(vWorldMin.x, vSceneMin.x);
        vWorldMin.y = Math::Min(vWorldMin.y, vSceneMin.y);
        vWorldMax.x = Math::Max(vWorldMax.x, vSceneMax.x);
        vWorldMax.y = Math::Max(vWorldMax.y, vSceneMax.y);
    }
    Vector2D vDelta = vWorldMax - vWorldMin;
    m_vCenter = vDelta * 0.5f;
    m_vCenter += vWorldMin;
    m_flWidth = Math::Max(vDelta.x, vDelta.y);

    SceneCommonDataIterator tIterator2;
    while (SceneCommonDataArrayIterate(tIterator2, ptSCD, SkipHolodeckScenes)) {
        if (ptSCD->idSWorld != tSWorld.id)
            continue;
        ScopedSceneDefinitionLockR pdefScene(ptSCD->snoScene);
        for (int32 i = 0; i < pdefScene->arMarkerSets.GetArraySize(); ++i) {
            ScopedMarkerSetDefinitionLockR pdefMarkerSet(pdefScene->arMarkerSets[i]);
            GBID gbidLabel = pdefMarkerSet->nLabel;
            if (gbidLabel == -1 || ptSCD->listAppliedLabels.Find(gbidLabel)) {
                for (int32 nCircle = 0; nCircle < pdefMarkerSet->tNoSpawnSet.GetArraySize(); ++nCircle) {
                    const Circle & tLocalCircle = pdefMarkerSet->tNoSpawnSet[nCircle];
                    Circle tWorldCircle = tLocalCircle;
                    Vector3D vPos(tWorldCircle.wpCenter.x, tWorldCircle.wpCenter.y, 0.0f);
                    vPos = ptSCD->transformWorld * vPos;
                    tWorldCircle.wpCenter = Vector2D(vPos.x, vPos.y);
                    tSWorld.tNoSpawnTree.AddCircle(tWorldCircle);
                }
            }
        }
    }
}

void NoSpawnTree::Dispose() {
    FreeNode(&m_ptTop);
    m_listNodes.RemoveAll();
}

void NoSpawnTree::FreeNode(NoSpawnNode ** pptNode) {
    if (*pptNode) {
        (*pptNode)->m_listCircles.RemoveAll();
        for (int32 i = 0; i < NoSpawnNode::NUM_CHILDREN; ++i)
            FreeNode(&(*pptNode)->m_arChildren[i]);
        *pptNode = NULL;
    }
}

NoSpawnNode * NoSpawnTree::NewNode() {
    // The original copies a default-initialized node before Clear initializes its
    // children. tNode is an invented name for that temporary, absent from DWARF.
    NoSpawnNode tNode;
    NoSpawnNode & tNewNode = *(NoSpawnNode *)m_listNodes.AddTail_Tracked(tNode);
    tNewNode.Clear();
    return &tNewNode;
}

void NoSpawnTree::AddCircle(const Circle & tCircle) {
    AddCircle(&m_ptTop, tCircle, m_vCenter, m_flWidth);
}

BOOL NoSpawnTree::CircleIsContained(const Circle & tCircle, const Vector2D & vCenter, float flWidth) {
    float flHalfWidth = flWidth * 0.5f;
    if (tCircle.wpCenter.x - tCircle.wdRadius < vCenter.x - flHalfWidth)
        return FALSE;
    if (tCircle.wpCenter.x + tCircle.wdRadius > vCenter.x + flHalfWidth)
        return FALSE;
    if (tCircle.wpCenter.y - tCircle.wdRadius < vCenter.y - flHalfWidth)
        return FALSE;
    if (tCircle.wpCenter.y + tCircle.wdRadius > vCenter.y + flHalfWidth)
        return FALSE;
    return TRUE;
}

void NoSpawnTree::GetChildCenter(const Vector2D & vParentCenter, Vector2D & vChildCenter, int32 nChildIdx, float flChildWidth) {
    float flChildHalfWidth = flChildWidth * 0.5f;
    if (nChildIdx & 1)
        vChildCenter.x = vParentCenter.x + flChildHalfWidth;
    else
        vChildCenter.x = vParentCenter.x - flChildHalfWidth;
    if (nChildIdx & 2)
        vChildCenter.y = vParentCenter.y + flChildHalfWidth;
    else
        vChildCenter.y = vParentCenter.y - flChildHalfWidth;
}

void NoSpawnTree::AddCircle(NoSpawnNode ** pptNode, const Circle & tCircle, const Vector2D & vCenter, float flWidth) {
    if (!*pptNode) {
        *pptNode = NewNode();
        ++m_uNodeCount;
    }
    float flHalfWidth = flWidth * 0.5f;
    if (flHalfWidth >= NOSPAWN_MIN_WIDTH) {
        for (int32 i = 0; i < NoSpawnNode::NUM_CHILDREN; ++i) {
            Vector2D vChildCenter;
            GetChildCenter(vCenter, vChildCenter, i, flHalfWidth);
            if (CircleIsContained(tCircle, vChildCenter, flHalfWidth)) {
                AddCircle(&(*pptNode)->m_arChildren[i], tCircle, vChildCenter, flHalfWidth);
                return;
            }
        }
    }
    (*pptNode)->AddCircle(tCircle);
    ++m_uCircleCount;
}

void NoSpawnTree::GetCirclesInRect(const Rect2D & tRect, SceneCommonData & tSCD, NoSpawnCallback pfCallback) {
    if (m_ptTop)
        CheckNodeForCollisions(m_ptTop, tRect, tSCD, pfCallback, m_vCenter, m_flWidth);
}

void NoSpawnTree::CheckNodeForCollisions(NoSpawnNode * ptNode, const Rect2D & tRect, SceneCommonData & tSCD, NoSpawnCallback pfCallback, const Vector2D & vCenter, float flWidth) {
    POS pos = ptNode->m_listCircles.GetHeadPosition();
    while (pos) {
        const Circle & tCircle = ptNode->m_listCircles.GetNext(pos);
        if (CircleIntersectsRect(tCircle, tRect))
            pfCallback(tSCD, tCircle);
    }
    float flHalfWidth = flWidth * 0.5f;
    for (int32 i = 0; i < NoSpawnNode::NUM_CHILDREN; ++i) {
        NoSpawnNode * ptChildNode = ptNode->m_arChildren[i];
        if (ptChildNode) {
            Vector2D vChildCenter;
            GetChildCenter(vCenter, vChildCenter, i, flHalfWidth);
            if (NodeIntersectsRect(vChildCenter, flHalfWidth, tRect))
                CheckNodeForCollisions(ptChildNode, tRect, tSCD, pfCallback, vChildCenter, flHalfWidth);
        }
    }
}

BOOL NoSpawnTree::CircleIntersectsRect(const Circle & tCircle, const Rect2D & tRect) {
    return CenterHalfWidthIntersectsRect(tCircle.wpCenter, tCircle.wdRadius, tRect);
}

BOOL NoSpawnTree::NodeIntersectsRect(const Vector2D & vCenter, float flWidth, const Rect2D & tRect) {
    return CenterHalfWidthIntersectsRect(vCenter, flWidth * 0.5f, tRect);
}

BOOL NoSpawnTree::CenterHalfWidthIntersectsRect(const Vector2D & vCenter, float flHalfWidth, const Rect2D & tRect) {
    Rect2D tRectNodeArea;
    tRectNodeArea.Set(vCenter.x - flHalfWidth, vCenter.y - flHalfWidth, vCenter.x + flHalfWidth, vCenter.y + flHalfWidth);
    return Rect2D::Intersect(tRect, tRectNodeArea, NULL);
}
