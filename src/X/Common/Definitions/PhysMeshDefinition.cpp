// Original file: X/Common/Definitions/PhysMeshDefinition.cpp

#include "PhysMeshDefinition.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

enum {
    PHYSMESH_COMMON_SNO_VERSION = 24,
    PHYSMESH_XML_VERSION = 1,
    PHYSMESH_PC_SNO_VERSION = 24,
    PHYSMESH_CONSOLE_SNO_VERSION = 25,
    PHYSMESH_AXE_SNO_VERSION = 25,
};

enum {
    MAX_PHYSMESH_DEFS_IN_MEMORY = 256,
    MAX_PHYSMESH_CACHE_SIZE = 104857600,
};


















































static const TypeDescriptorField dmMeshNode_Fields[8] = {
    TypeDescriptorField("", DT_WORD, offsetof(dmMeshNode, lowerX), NULL, 1), TypeDescriptorField("", DT_WORD, offsetof(dmMeshNode, lowerY), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(dmMeshNode, lowerZ), NULL, 1), TypeDescriptorField("", DT_WORD, offsetof(dmMeshNode, upperX), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(dmMeshNode, upperY), NULL, 1), TypeDescriptorField("", DT_WORD, offsetof(dmMeshNode, upperZ), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(dmMeshNode, data), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(dmMeshNode), NULL, 0),
};


ComplexTypeDescriptor TypeDescriptor_dmMeshNode("dmMeshNode", 0, dmMeshNode_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DOMINO_MESH_NODE = &TypeDescriptor_dmMeshNode;

static const TypeDescriptorField dmMeshTriangle_Fields[9] = {
    TypeDescriptorField("", DT_INT, offsetof(dmMeshTriangle, i1), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(dmMeshTriangle, i2), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(dmMeshTriangle, i3), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(dmMeshTriangle, w1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(dmMeshTriangle, w2), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(dmMeshTriangle, w3), NULL, 1),
    TypeDescriptorField("", DT_WORD, offsetof(dmMeshTriangle, materialIndex), NULL, 1), TypeDescriptorField("", DT_WORD, offsetof(dmMeshTriangle, collisionFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(dmMeshTriangle), NULL, 0),
};


ComplexTypeDescriptor TypeDescriptor_dmMeshTriangle("dmMeshTriangle", 0, dmMeshTriangle_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DOMINO_MESH_TRIANGLE = &TypeDescriptor_dmMeshTriangle;

static const TypeDescriptorField dmFloat3_Fields[4] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(dmFloat3, x), NULL, 1), TypeDescriptorField("", DT_FLOAT, offsetof(dmFloat3, y), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(dmFloat3, z), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(dmFloat3), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_dmFloat3("dmFloat3", 0, dmFloat3_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DOMINO_FLOAT3 = &TypeDescriptor_dmFloat3;

static const TypeDescriptorField dmFloat4_Fields[5] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(dmFloat4, x), NULL, 1), TypeDescriptorField("", DT_FLOAT, offsetof(dmFloat4, y), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(dmFloat4, z), NULL, 1), TypeDescriptorField("", DT_FLOAT, offsetof(dmFloat4, w), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(dmFloat4), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_dmFloat4("dmFloat4", 0, dmFloat4_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DOMINO_FLOAT4 = &TypeDescriptor_dmFloat4;

static const TypeDescriptorField CollisionMesh_Fields[16] = {
    TypeDescriptorField("", DT_INT, offsetof(CollisionMesh, m_nodeCount), NULL, 1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(CollisionMesh, serDominoNodes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_DOMINO_MESH_NODE, offsetof(CollisionMesh, m_pRoot), 0x8001, (int32)offsetof(CollisionMesh, serDominoNodes) - (int32)offsetof(CollisionMesh, m_pRoot)), TypeDescriptorField("", DT_INT, offsetof(CollisionMesh, m_vertexCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(CollisionMesh, serDominoVertices), NULL, 0), VariableArrayTypeDescriptorField("", DT_DOMINO_FLOAT4, offsetof(CollisionMesh, m_aVertices), 0x8001, (int32)offsetof(CollisionMesh, serDominoVertices) - (int32)offsetof(CollisionMesh, m_aVertices)),
    TypeDescriptorField("", DT_INT, offsetof(CollisionMesh, m_triangleCount), NULL, 1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(CollisionMesh, serDominoTriangles), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_DOMINO_MESH_TRIANGLE, offsetof(CollisionMesh, m_aTriangles), 0x21, (int32)offsetof(CollisionMesh, serDominoTriangles) - (int32)offsetof(CollisionMesh, m_aTriangles)), TypeDescriptorField("", DT_DOMINO_FLOAT3, offsetof(CollisionMesh, m_center), NULL, 1),
    TypeDescriptorField("", DT_DOMINO_FLOAT3, offsetof(CollisionMesh, m_extents), NULL, 1), TypeDescriptorField("", DT_DOMINO_FLOAT3, offsetof(CollisionMesh, m_unquantize), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CollisionMesh, m_height), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(CollisionMesh, uFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CollisionMesh, uMaterial), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(CollisionMesh), NULL, 0),
};






ComplexTypeDescriptor TypeDescriptor_CollisionMesh("CollisionMesh", 0, CollisionMesh_Fields, 16, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PHYSICS_MESH = &TypeDescriptor_CollisionMesh;


static const TypeDescriptorField PhysMeshDefinition_Fields[6] = {
    TypeDescriptorField("", DT_INT, offsetof(PhysMeshDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PhysMeshDefinition, serCollisionMeshes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_PHYSICS_MESH, offsetof(PhysMeshDefinition, ptCollisionMeshes), 0x20, (int32)offsetof(PhysMeshDefinition, serCollisionMeshes) - (int32)offsetof(PhysMeshDefinition, ptCollisionMeshes)),
    VariableArrayCountTypeDescriptorField("", offsetof(PhysMeshDefinition, nCollisionMeshCount), 0x10000, DT_PHYSICS_MESH, (int32)offsetof(PhysMeshDefinition, serCollisionMeshes) - (int32)offsetof(PhysMeshDefinition, nCollisionMeshCount)),
    TypeDescriptorField("", DT_INT, offsetof(PhysMeshDefinition, dwMachineTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PhysMeshDefinition), NULL, 0),
};

ComplexTypeDescriptor TypeDescriptor_PhysMeshDefinition("PhysMesh", PHYSMESH_XML_VERSION, PhysMeshDefinition_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PHYSMESH = &TypeDescriptor_PhysMeshDefinition;





PhysMeshGroupDef * gptGroupDefPhysMesh;





PhysMeshGroupDef::PhysMeshGroupDef()
    : GroupDefinition("PhysMesh", PHYSMESH_SNO,
                      SNOVersions(PHYSMESH_PC_SNO_VERSION,
                                  PHYSMESH_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  PHYSMESH_CONSOLE_SNO_VERSION,
                                  PHYSMESH_CONSOLE_SNO_VERSION,
                                  PHYSMESH_CONSOLE_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(PhysMeshDefinition), DT_PHYSMESH, 1, 0x840e) {
}












uint32 PhysMeshGroupDef::GetMaxCount() {
    return MAX_PHYSMESH_DEFS_IN_MEMORY;
}














uint32 PhysMeshGroupDef::GetEstCacheSizeLimit() {
    return MAX_PHYSMESH_CACHE_SIZE;
}






void PhysMeshGroupDef::GetDefaultData(void * ptDefaultDef) const {

    SigmaMemoryClear(ptDefaultDef, sizeof(PhysMeshDefinition));
}





void PhysMeshGroupDef::Postprocess(void * lpDefinition) {


    GroupDefinition::Postprocess(lpDefinition);
}




















void PhysMeshGroupDef::Delete(void * lpDefinition) const {














    GroupDefinition::Delete(lpDefinition);
}






BOOL PhysMeshGroupDef::CacheManagerCanPurgeElement(const DefDataElement & tDefElement,
                                                   BOOL fIgnoreDoNotPurgeBit) const
{
    if (!GroupDefinition::CacheManagerCanPurgeElement(tDefElement, fIgnoreDoNotPurgeBit))
    {
        return FALSE;
    }

    return TRUE;
}





























































































BOOL PhysMeshGroupDef::Write(void * lpDefinition,
                             FileReference & tRef) const
{



    XASSERT(FALSE);
    return FALSE;
}
