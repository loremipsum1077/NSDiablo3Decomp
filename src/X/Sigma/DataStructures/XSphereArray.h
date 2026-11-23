#pragma once

#include "../Math/MathGeneral.h"
#include "DataStructuresExternal.h"
#include "XChainedFixedBlockAllocator.h"
#include "XGrowableArray.h"
#include "XSphereNode.h"
#include "XTemplate.h"

struct Capsule;
struct Frustum;
struct LineSeg3D;
struct Vector2D;
struct Vector3D;

class XSphereNodeAndPOS : public XSphereNode {
public:



    XSphereNodeAndPOS() : m_pos(NULL) {}





    POS GetPOS() {
        return m_pos;
    }


    void SetPOS(POS pos) {
        m_pos = pos;
    }

private:
    POS m_pos;
};

typedef XChainedFixedBlockAllocator<XSphereNodeAndPOS> XSphereNodeAndPOSAllocator;

class XSphereArray {
public:
    XSphereArray(XMemoryAllocatorInterface* ptMemoryInterface = NULL);  // default argument: XChainedFixedBlockAllocator<XSphereArray>::Alloc_Tracked constructs it with no arguments and passes NULL (ACDCollInitializeForWorld)
    ~XSphereArray();

    void SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface);

    void Init(LPCSTR szName, const int32 nMaxSpheres, const float wdBoundingSpherePadAmount);
    void Uninit();
    void Resize(int32 nMaxSpheres);

    void Update();

    XSphereNode* AddSphereNode(const Sphere& ws, void* pUserData);
    BOOL IsSphereValid(const Sphere& ws);
    void RemoveSphereNode(XSphereNode* pSphereNode);
    void ModifySphereNode(XSphereNode* pSphereNode, const Sphere& tSphere);

    void SphereCull(const Sphere& tSphere, XSphereNodeList& sphereNodeList);
    void PlayerTargetCull(const Sphere& tSphereClose, XSphereNodeList& listSpheresClose, const Sphere& tSphereCone,
                          const Vector2D& wpConeStart, const Vector2D& wpConeDir, Angle aRequiredTheta,
                          float flSearchDist, XSphereNodeList& listSpheresCone);
    void PointCull(const Vector3D& wp, XSphereNodeList& sphereNodeList);
    void LineSegCull(const LineSeg3D& wline, XSphereNodeList& sphereNodeList);
    void CapsuleCull(const Capsule& capsule, XSphereNodeList& sphereNodeList);
    void FrustumCull(const Frustum& frustum, XSphereNodeList& sphereNodeList);

    void RegisterSphereNameFunc(SphereNameFunc funcSphereName);

private:
    XSphereNodeAndPOSAllocator m_sphereNodeAllocator;
    SphereNameFunc m_funcSphereName;
    XSphereNodeList m_listSpheres;
    XGrowableArray<Sphere, const Sphere&> m_arraySpheres;
    XGrowableArray<const XSphereNode*, const XSphereNode*> m_pXSphereNodes;
    BOOL m_needsUpdate;
};
































































inline void XSphereArray::RegisterSphereNameFunc(SphereNameFunc funcSphereName) {
    m_funcSphereName = funcSphereName;
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XSphereNodeAndPOS) == 0x38, "XSphereNodeAndPOS");
static_assert(sizeof(XSphereArray) == 0xd8, "XSphereArray");
