// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/PhysMeshDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<PhysMeshGroupDef>), and the definition structs.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../../../Contrib/Contrib/Domino/Include/dmMeshData.h"
#include "../../../X/Sigma/DataStructures/XPtr64.h"
#include "../../../X/Sigma/Files/XSerializeData.h"

class XMemoryAllocatorInterface;

struct CollisionMesh : public dmMeshData {  // size 0x70
    SerializeData serDominoVertices;  // 0x50
    SerializeData serDominoTriangles;  // 0x58
    SerializeData serDominoNodes;  // 0x60
    uint32 uFlags;  // 0x68
    uint32 uMaterial;  // 0x6c
};
static_assert(sizeof(CollisionMesh) == 0x70, "CollisionMesh");

struct PhysMeshDefinition {  // size 0x30
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    int32 nCollisionMeshCount;  // 0x10
    SerializeData serCollisionMeshes;  // 0x14
    XPtr64<CollisionMesh> ptCollisionMeshes;  // 0x20
    uint32 dwMachineTime;  // 0x28
};
static_assert(sizeof(PhysMeshDefinition) == 0x30, "PhysMeshDefinition");

class PhysMeshGroupDef : public GroupDefinition {  // size 0xa0
public:
    PhysMeshGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 80
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_COREDATA; }  // vtable slot 7, line 82
    virtual uint32 GetEstCacheSizeLimit();  // vtable slot 12
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL CacheManagerCanPurgeElement(const DefDataElement & tDefElement, BOOL fIgnoreDoNotPurgeBit) const;  // vtable slot 29
};
static_assert(sizeof(PhysMeshGroupDef) == 0xa0, "PhysMeshGroupDef");

extern PhysMeshGroupDef * gptGroupDefPhysMesh;  // PhysMeshDefinition.cpp
