#include "dmFixture.h"
#include "dmWorld.h"
#include "dmPhantom.h"
#include "../Include/dmPolytope.h"
#include "../Include/dmTreeMesh.h"
#include "../Include/dmHeightField.h"
#include "../Collision/Functions/dmDistance.h"
#include <new>
#include "dmBody.h"
#include "Contacts/dmContact.h"
#include "../Include/dmShapes.h"
#include "../Include/dmSettings.h"
extern const dmFixtureId dm_nullFixtureId = { -1, 0, 0xffff };  // line 30
bool operator==(dmFixtureId id1, dmFixtureId id2)
{
    return id1.worldIndex == id2.worldIndex && id1.index == id2.index && id1.revision == id2.revision;
}

bool operator!=(dmFixtureId id1, dmFixtureId id2)
{
    return id1.worldIndex != id2.worldIndex || id1.index != id2.index || id1.revision != id2.revision;
}

bool dmShouldCollide(const dmFilter& filterA, const dmFilter& filterB)
{
    if (filterA.m_groupIndex == filterB.m_groupIndex && filterA.m_groupIndex != 0)
        return filterA.m_groupIndex > 0;
    bool collideA = (filterA.m_includeMask & filterB.m_categoryBits) != 0;
    bool collideB = (filterB.m_includeMask & filterA.m_categoryBits) != 0;
    return collideA && collideB;
}

// The original proxy sentinel is named by the assertion and compares to -1.
static const dm_int32 dm_nullProxy = -1;

#line 326 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/dmFixture.cpp"
void dmFixture::CreateProxy(dmWorld* pWorld, const dmMtx& transform)
{
    dmAssert(m_proxyId == dm_nullProxy);
    m_aabb = m_pShape->GetAABB(transform);
    m_proxyId = pWorld->m_broadPhase.CreateProxy(m_aabb, m_index);
}

void dmFixture::TouchProxy(dmWorld* pWorld)
{
    if (m_proxyId != dm_nullProxy)
        pWorld->m_broadPhase.TouchProxy(m_proxyId);
}

void dmFixture::Synchronize(dmWorld* pWorld, const dmMtx& transform)
{
    if (m_proxyId != dm_nullProxy) {
        m_aabb = m_pShape->GetAABB(transform);
        pWorld->m_broadPhase.MoveProxy(m_proxyId, m_aabb);
    }
}

void dmFixture::Synchronize(dmWorld* pWorld, const dmMtx& transform1, const dmMtx& transform2)
{
    if (m_proxyId != dm_nullProxy) {
        m_aabb = m_pShape->GetSweptAABB(transform1, transform2);
        pWorld->m_broadPhase.MoveProxy(m_proxyId, m_aabb);
    }
}

void dmFixture::DestroyContacts(dmWorld* pWorld)
{
    dm_int32 edgeIndex = m_contactListKey;
    while (edgeIndex != -1) {
        dm_int32 contactIndex = edgeIndex >> 1;
        dmContact::Edge* pEdge = &pWorld->m_contacts.Get(contactIndex)->m_edges[edgeIndex & 1];
        dm_int32 next = pEdge->m_nextKey;
        pWorld->DestroyContact(contactIndex);
        edgeIndex = next;
    }
}

dmFixtureType dmFixture_GetType(dmFixtureId id)
{
    if (id.worldIndex >= 65534) return dm_unknownFixture;
    dmWorld* pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == 0 || !pWorld->m_fixtures.IsValid(id.index) || pWorld->m_fixtures[id.index].m_revision != id.revision)
        return dm_unknownFixture;
    return pWorld->m_fixtures[id.index].m_type;
}

void* dmFixture_GetUserData(dmFixtureId id)
{
    if (id.worldIndex >= 65534) return 0;
    dmWorld* pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == 0 || !pWorld->m_fixtures.IsValid(id.index) || pWorld->m_fixtures[id.index].m_revision != id.revision)
        return 0;
    return pWorld->m_fixtures[id.index].m_pUserData;
}

// dm_nullBodyId is owned by dmBody.cpp; the original reads its revision field.
extern const dmBodyId dm_nullBodyId;

dmBodyId dmFixture_GetOwnerBody(dmFixtureId id)
{
    if (id.worldIndex >= 65534) return dm_nullBodyId;
    if (id.index == -1) return dm_nullBodyId;
    dmWorld* pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == 0 || !pWorld->m_fixtures.IsValid(id.index)) return dm_nullBodyId;
    dmFixture* pFixture = &pWorld->m_fixtures[id.index];
    if (pFixture->m_revision != id.revision || pFixture->m_ownerType != dm_bodyOwner) return dm_nullBodyId;
    dm_int32 bodyIndex = pFixture->m_ownerIndex;
    dmBodyId bodyId;
    bodyId.index = bodyIndex;
    bodyId.revision = pWorld->m_bodies[bodyIndex].m_revision;
    bodyId.worldIndex = id.worldIndex;
    return bodyId;
}

dmAABB dmFixture_GetAABB(dmFixtureId id)
{
    dmAABB aabb;
    aabb.m_lowerBound = dm_vec4{0,0,0,0};
    aabb.m_upperBound = dm_vec4{0,0,0,0};
    if (id.worldIndex >= 65534) return aabb;
    dmWorld* pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == 0 || !pWorld->m_fixtures.IsValid(id.index) || pWorld->m_fixtures[id.index].m_revision != id.revision)
        return aabb;
    return pWorld->m_fixtures[id.index].m_aabb;
}

// dmPool.h:58, instantiated here by GetTransform.
template <typename T> const T* dmPool<T>::Get(dm_int32 index) const
{
    return m_pData + index;
}

// On its DWARF lines 79-181 (relocated below). Member stores in row order: 88 m_ownerIndex, 91 m_proxyId, 92 m_contactListKey, 95 m_pShape, 96 m_minRadius (tab columns 15/12/19/11/14).
#line 79 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/dmFixture.cpp"
void dmFixture::Create(dmWorld* pWorld, const dmFixtureDef* pDef)
{
    m_pUserData = pDef->m_pUserData;
    m_friction = pDef->m_friction;
    m_restitution = pDef->m_restitution;
    m_rollingResistance = pDef->m_rollingResistance;
    m_density = pDef->m_density;


    m_ownerIndex = -1;
    m_ownerNext = -1;

    m_proxyId = dm_nullProxy;
    m_contactListKey = -1;
    m_contactCount = 0;

    m_pShape = 0;
    m_minRadius = 0.0f;
    m_maxRadius = 0.0f;

    m_filter = pDef->m_filter;

    m_isSensor = pDef->m_isSensor;

    m_type = pDef->m_type;

    switch (m_type)
    {
    case dm_sphereFixture:
    {
        const dmSphereDef* pSphereDef = static_cast<const dmSphereDef*>(pDef);
        void* pMemory = pWorld->m_poolAllocator.Allocate(sizeof(dmSphere), false);
        dmSphere* pSphere = new (pMemory) dmSphere;
        pSphere->Create(pSphereDef->m_localPosition, pSphereDef->m_radius);
        m_minRadius = pSphereDef->m_radius;
        m_maxRadius = dmF32(dmLength3(pSphere->m_p));
        m_pShape = pSphere;
        break;
    }

    case dm_capsuleFixture:
    {
        const dmCapsuleDef* pCapsuleDef = static_cast<const dmCapsuleDef*>(pDef);
        if (dmIsZeroSafe3(pCapsuleDef->m_p2 - pCapsuleDef->m_p1) && !dm_assertOnce) { dmAssertFunction("dmIsZeroSafe3(pCapsuleDef->m_p2 - pCapsuleDef->m_p1)", __FILE__, 122, 0, 0); __builtin_trap(); }  // one row (122:4); the original diagnostic omits the negation seen in its branch
        void* pMemory = pWorld->m_poolAllocator.Allocate(sizeof(dmCapsule), false);
        dmCapsule* pCapsule = new (pMemory) dmCapsule;
        pCapsule->Create(pCapsuleDef->m_p1, pCapsuleDef->m_p2, pCapsuleDef->m_radius);
        dm_float32 length1 = dmF32(dmLength3(pCapsule->m_p1));
        dm_float32 length2 = dmF32(dmLength3(pCapsule->m_p2));
        m_minRadius = pCapsuleDef->m_radius;
        m_maxRadius = dmMax(length1, length2);
        m_pShape = pCapsule;
        break;
    }

    case dm_polytopeFixture:
    {
        const dmPolytopeDef* pPolytopeDef = static_cast<const dmPolytopeDef*>(pDef);
        void* pMemory = pWorld->m_poolAllocator.Allocate(sizeof(dmPolytope), false);
        dmPolytope* pPolytope = new (pMemory) dmPolytope;
        pPolytope->Create(pPolytopeDef->m_pPolytope, pPolytopeDef->m_transform, pPolytopeDef->m_scale, &pWorld->m_poolAllocator);


        m_minRadius = pPolytope->ComputeMinRadius();
        m_maxRadius = pPolytope->ComputeMaxRadius();

        m_pShape = pPolytope;
        break;
    }

    case dm_treeMeshFixture:
    {
        const dmTreeMeshDef* pTreeMeshDef = static_cast<const dmTreeMeshDef*>(pDef);
        void* pMemory = pWorld->m_poolAllocator.Allocate(sizeof(dmTreeMesh), false);
        dmTreeMesh* pMesh = new (pMemory) dmTreeMesh;
        pMesh->Create(pTreeMeshDef->m_pData, pTreeMeshDef->m_scale);
        m_pShape = pMesh;

        m_minRadius = pMesh->m_radius;
        m_maxRadius = 0.0f;
        break;
    }

    case dm_heightFieldFixture:
    {
        const dmHeightFieldDef* pHeightFieldDef = static_cast<const dmHeightFieldDef*>(pDef);
        void* pMemory = pWorld->m_poolAllocator.Allocate(sizeof(dmHeightField), false);
        dmHeightField* pField = new (pMemory) dmHeightField;
        pField->Create(pHeightFieldDef->m_pData);
        m_pShape = pField;
        break;
    }
    default:

        dmAssert(false);
        break;
    }


    pWorld->m_newContacts = true;

    ++m_revision;
}
#line 179 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/dmFixture.cpp"

void dmFixture::Destroy(dmWorld* pWorld)
{
    dm_int32 edgeIndex = m_contactListKey;
    while (edgeIndex != -1)
    {
        dm_int32 contactIndex = edgeIndex >> 1;
        dmContact::Edge* pEdge = &pWorld->m_contacts.Get(contactIndex)->m_edges[edgeIndex & 1];
        dm_int32 next = pEdge->m_nextKey;
        pWorld->m_contacts.Get(contactIndex)->Destroy(pWorld);
        pWorld->m_contacts.Free(contactIndex);
        edgeIndex = next;
    }
    if (m_proxyId != dm_nullProxy)
    {
        pWorld->m_broadPhase.DestroyProxy(m_proxyId);
        m_proxyId = dm_nullProxy;
    }
    switch (m_pShape->m_type)
    {
    case dmShape::e_sphere:
    {
        dmSphere* pSphere = static_cast<dmSphere*>(m_pShape);
        pSphere->~dmSphere();
        pWorld->m_poolAllocator.Free(pSphere, sizeof(dmSphere), false);
        break;
    }
    case dmShape::e_capsule:
    {
        dmCapsule* pCapsule = static_cast<dmCapsule*>(m_pShape);
        pCapsule->~dmCapsule();
        pWorld->m_poolAllocator.Free(pCapsule, sizeof(dmCapsule), false);
        break;
    }
    case dmShape::e_polytope:
    {
        dmPolytope* pPolytope = static_cast<dmPolytope*>(m_pShape);
        pPolytope->~dmPolytope();
        pWorld->m_poolAllocator.Free(pPolytope, sizeof(dmPolytope), false);
        break;
    }
    case dmShape::e_mesh:
    {
        dmMesh* pMesh = static_cast<dmMesh*>(m_pShape);
        dmMesh::MeshType meshType = pMesh->m_meshType;
        if (meshType == dmMesh::e_treeMesh)
        {
            dmTreeMesh* pTreeMesh = static_cast<dmTreeMesh*>(pMesh);
            pTreeMesh->~dmTreeMesh();
            pWorld->m_poolAllocator.Free(pTreeMesh, sizeof(dmTreeMesh), false);
        }
        else if (meshType == dmMesh::e_heightField)
        {
            dmHeightField* pHeightField = static_cast<dmHeightField*>(pMesh);
            pHeightField->~dmHeightField();
            pWorld->m_poolAllocator.Free(pHeightField, sizeof(dmHeightField), false);
        }
        else
        {
#line 250 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/dmFixture.cpp"
            dmAssert(false);
        }
        break;
    }
    default:
#line 256 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/dmFixture.cpp"
        dmAssert(false);
        break;
    }
    m_pShape = 0;
}

dmTransform dmFixture::GetTransform(const dmWorld* pWorld) const
{
    dmTransform xf;
    if (m_ownerType == dm_bodyOwner)
    {
        const dmBody* pBody = pWorld->m_bodies.Get(m_ownerIndex);
        if (pWorld->m_useSubStepping && !pWorld->m_isStepComplete)
            xf = pBody->m_sweep.GetTransform(dmV4(0.0f));
        else
            xf = pBody->m_transform;
    }
    else
    {
        const dmPhantom* pPhantom = pWorld->m_phantoms.Get(m_ownerIndex);
        xf.q = pPhantom->m_quaternion;
        xf.p = pPhantom->m_position;
    }
    return xf;
}

void dmFixture::GetClosestPoint(dmDistanceOutput* pOutput, const dmTransform& transform, dm_vec4Arg point) const
{
    if (m_pShape->m_type == dmShape::e_mesh)
    {
        pOutput->distance = dmV4(0.0f);
        pOutput->exitCode = -1;
        pOutput->normal = dmV4(0.0f);
        pOutput->pointB = dmV4(0.0f);
        pOutput->pointA = dmV4(0.0f);
        return;
    }
    dmSphere dummySphere;
    dummySphere.m_p = point; dummySphere.m_radius = 0.0f;  // original rows 445:18 (m_p) and 446:23 (m_radius, a separate 4-byte store)
    dmDistanceInput input;
    input.cache.Clear();
    input.useRadii = true;
    input.proxyA.Set(m_pShape);
    input.proxyB.Set(&dummySphere);
    input.transformA = transform;
    input.transformB = dmTransformIdentity();
    dmDistance(pOutput, &input);
}

// Validation: the original keeps three return blocks (index == -1 / null world, IsValid, revision), so these are three tests.
dm_vec4 dmFixture_GetClosestPoint(dmFixtureId id, dm_vec4Arg point)
{
    if (id.worldIndex >= 65534) return point;
    dmWorld* pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (id.index == -1 || pWorld == 0) return point;
    if (!pWorld->m_fixtures.IsValid(id.index)) return point;
    if (pWorld->m_fixtures[id.index].m_revision != id.revision) return point;
    dmTransform transform = pWorld->m_fixtures[id.index].GetTransform(pWorld);
    dmDistanceOutput output;
    pWorld->m_fixtures[id.index].GetClosestPoint(&output, transform, point);
    return output.pointA;
}

dm_vec4 dmFixture::GetProjectedArea(const dmTransform& transform, dm_vec4Arg dir) const
{
    dm_vec4 localDir = dmInvRotate(transform.q, dir);
    return m_pShape->ComputeProjectedSurfaceArea(localDir);
}

dm_vec4 dmFixture_GetProjectedArea(dmFixtureId id, dm_vec4Arg direction)
{
    dm_vec4 area = dmV4(0.0f);
    if (id.worldIndex >= 65534) return area;
    dmWorld* pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == 0 || !pWorld->m_fixtures.IsValid(id.index) || pWorld->m_fixtures[id.index].m_revision != id.revision)
        return area;
    dmTransform transform = pWorld->m_fixtures[id.index].GetTransform(pWorld);
    area = pWorld->m_fixtures[id.index].GetProjectedArea(transform, direction);
    return area;
}
