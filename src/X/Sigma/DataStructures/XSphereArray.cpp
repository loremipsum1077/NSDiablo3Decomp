#include "XSphereArray.h"

#include "../Math/Intersect.h"
#include "../MemoryPools.h"















static const float MAX_SPHERE_RADIUS = 1000.0f;





XSphereArray::XSphereArray(XMemoryAllocatorInterface* ptMemoryInterface) {
    m_listSpheres.SetPool(MemoryPoolsGetSphereNodeListPool());
    m_sphereNodeAllocator.SetMemoryInterface(ptMemoryInterface);
    m_funcSphereName = NULL;
    m_needsUpdate = TRUE;
}






XSphereArray::~XSphereArray() {
}






void XSphereArray::SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface) {
    m_sphereNodeAllocator.SetMemoryInterface(ptMemoryInterface);
}






void XSphereArray::Init(LPCSTR szName, const int32 nMaxSpheres, const float wdBoundingSpherePadAmount) {







    Resize(nMaxSpheres);
    m_needsUpdate = TRUE;
}




void XSphereArray::Uninit() {
    while (!m_listSpheres.IsEmpty()) {
        XSphereNodeAndPOS* pSphere = (XSphereNodeAndPOS*)m_listSpheres.GetHead();
        m_listSpheres.RemoveHead();
        m_sphereNodeAllocator.Free(pSphere);
    }

    m_arraySpheres.EmptyArray();
    m_pXSphereNodes.EmptyArray();
    m_needsUpdate = FALSE;
}







void XSphereArray::Resize(int32 nMaxSpheres) {


    m_sphereNodeAllocator.SetBlockCount_Tracked(nMaxSpheres);

    m_sphereNodeAllocator.SetGrow(XALLOC_GROW_LINEAR, 2048);
    m_sphereNodeAllocator.SetShrinkable(TRUE);
}





XSphereNode* XSphereArray::AddSphereNode(const Sphere& ws, void* pUserData) {
    XSphereNodeAndPOS* pSphereNode;








    pSphereNode = m_sphereNodeAllocator.Alloc_Tracked();
    pSphereNode->Init(ws);
    pSphereNode->SetUserData(pUserData);
    pSphereNode->SetPOS(m_listSpheres.AddTail_Tracked(pSphereNode));

    m_needsUpdate = TRUE;

    return pSphereNode;
}


static const float MAX_SPHERE_POSITION = 99900.0f;
BOOL XSphereArray::IsSphereValid(const Sphere& ws) {


    if (!isfinite(ws.wpCenter.x)) return FALSE;
    if (!isfinite(ws.wpCenter.y)) return FALSE;
    if (!isfinite(ws.wpCenter.z)) return FALSE;




    if (Math::Abs(ws.wpCenter.x) >= MAX_SPHERE_POSITION) return FALSE;
    if (Math::Abs(ws.wpCenter.y) >= MAX_SPHERE_POSITION) return FALSE;
    if (Math::Abs(ws.wpCenter.z) >= MAX_SPHERE_POSITION) return FALSE;

    if (ws.wdRadius >= MAX_SPHERE_RADIUS) return FALSE;

    return TRUE;
}










void XSphereArray::RemoveSphereNode(XSphereNode* pSphereNode) {
    POS posList;
    XSphereNodeAndPOS* pSphereNodeAndPOS;

    if (pSphereNode != NULL) {
        pSphereNodeAndPOS = (XSphereNodeAndPOS*)pSphereNode;



        posList = pSphereNodeAndPOS->GetPOS();
        m_listSpheres.RemoveAt(posList);
        m_sphereNodeAllocator.Free(pSphereNodeAndPOS);

        m_needsUpdate = TRUE;
    }
}


void XSphereArray::ModifySphereNode(XSphereNode* pSphereNode, const Sphere& tSphere) {











    pSphereNode->Init(tSphere);
    m_needsUpdate = TRUE;
}






void XSphereArray::Update() {
    volatile BOOL bNeedsUpdate = m_needsUpdate;

    if (bNeedsUpdate) {
        bNeedsUpdate = m_needsUpdate;
        if (bNeedsUpdate) {













            int32 nSpheres = m_listSpheres.Count();
            m_arraySpheres.SetArraySize_Tracked(nSpheres);
            m_pXSphereNodes.SetArraySize_Tracked(nSpheres);

            POS pos = m_listSpheres.GetHeadPosition();

            for (int32 i = 0; i != nSpheres; i++) {
                const XSphereNode* pSphere = m_listSpheres.GetNext(pos);
                m_arraySpheres[i] = pSphere->m_sphere;
                m_pXSphereNodes[i] = pSphere;
            }

            m_needsUpdate = FALSE;
        }
    }
}



void XSphereArray::SphereCull(const Sphere& tSphere, XSphereNodeList& sphereNodeList) {
    Update();




    Sphere* pSpheres = m_arraySpheres.GetPointer();
    for (uint32 i = 0, dwSpheres = m_arraySpheres.GetArraySize(); i < dwSpheres; i++, pSpheres++) {

        if (Intersect::SphereAndSphere(tSphere, *pSpheres)) {




            sphereNodeList.AddTail_Tracked(m_pXSphereNodes[i]);
        }
    }
}



void XSphereArray::PlayerTargetCull(const Sphere& tSphereClose, XSphereNodeList& listSpheresClose,
                                    const Sphere& tSphereCone, const Vector2D& wpConeStart, const Vector2D& wpConeDir,
                                    Angle aRequiredTheta, float flSearchDist, XSphereNodeList& listSpheresCone) {
    Update();













    Sphere* pSpheres = m_arraySpheres.GetPointer();
    for (uint32 i = 0, dwSpheres = m_arraySpheres.GetArraySize(); i < dwSpheres; i++, pSpheres++) {

        Sphere tLocalSphere = *pSpheres;
        if (Intersect::SphereAndSphere(tSphereClose, tLocalSphere)) {

            listSpheresClose.AddTail_Tracked(m_pXSphereNodes[i]);
        }

        if (Intersect::SphereAndSphere(tSphereCone, tLocalSphere)) {

            listSpheresCone.AddTail_Tracked(m_pXSphereNodes[i]);
        }
    }
}





void XSphereArray::FrustumCull(const Frustum& frustum, XSphereNodeList& sphereNodeList) {
    Update();




    Sphere* pSpheres = m_arraySpheres.GetPointer();
    for (uint32 i = 0, dwSpheres = m_arraySpheres.GetArraySize(); i < dwSpheres; i++, pSpheres++) {

        if (Intersect::FrustumAndSphere(frustum, *pSpheres)) {



            sphereNodeList.AddTail_Tracked(m_pXSphereNodes[i]);
        }
    }
}
















































void XSphereArray::PointCull(const Vector3D& wp, XSphereNodeList& sphereNodeList) {
    Update();




    Sphere* pSpheres = m_arraySpheres.GetPointer();
    for (uint32 i = 0, dwSpheres = m_arraySpheres.GetArraySize(); i < dwSpheres; i++, pSpheres++) {

        if (pSpheres->ContainsPoint(wp)) {



            sphereNodeList.AddTail_Tracked(m_pXSphereNodes[i]);
        }
    }
}
























void XSphereArray::LineSegCull(const LineSeg3D& wline, XSphereNodeList& sphereNodeList) {
    Update();




    Sphere* pSpheres = m_arraySpheres.GetPointer();
    for (uint32 i = 0, dwSpheres = m_arraySpheres.GetArraySize(); i < dwSpheres; i++, pSpheres++) {

        float wdT;
        if (Intersect::LineSeg3DAndSphere(wline, *pSpheres, wdT, NULL)) {



            sphereNodeList.AddTail_Tracked(m_pXSphereNodes[i]);
        }
    }
}



void XSphereArray::CapsuleCull(const Capsule& capsule, XSphereNodeList& sphereNodeList) {
    Update();




    Sphere* pSpheres = m_arraySpheres.GetPointer();
    for (uint32 i = 0, dwSpheres = m_arraySpheres.GetArraySize(); i < dwSpheres; i++, pSpheres++) {

        if (Intersect::CapsuleAndSphere(capsule, *pSpheres)) {



            sphereNodeList.AddTail_Tracked(m_pXSphereNodes[i]);
        }
    }
}
