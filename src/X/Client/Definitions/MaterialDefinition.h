// X/Client/Definitions/MaterialDefinition.h
//
// Material group definition (MATERIAL_SNO 57) and the reflected
// MaterialDefinition payload that backs it.  Reconstructed from the DWARF
// skeleton at build/dwarf/headers/X/Client/Definitions/MaterialDefinition.h.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Gfx/GfxMaterial.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class CTagMap;  // X/Common/TagMaps/Tags.h; only held through XPtr64 here
struct FileReference;
class XMemoryAllocatorInterface;

struct MaterialDefinition {  // size 0x88
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    SerializeData serTagMap;  // 0x10
    XPtr64<CTagMap> hTagMap;  // 0x18
    UberMaterial tUberMaterial;  // 0x20
};
static_assert(sizeof(MaterialDefinition) == 0x88, "MaterialDefinition");

class MaterialGroupDef : public GroupDefinition {  // size 0xa0
public:
    MaterialGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 46; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 49
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 51
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void PurgeDefinitionAndSNO(const SNO sno);  // vtable slot 25
};
static_assert(sizeof(MaterialGroupDef) == 0xa0, "MaterialGroupDef");

extern MaterialGroupDef * gptGroupDefMaterial;  // MaterialDefinition.cpp:53

// MaterialDefinition.h:76-77, inlined into AppearanceDefinition.cpp sPostProcessForRaycast
// (GroupDefinition::LockR(sno, FALSE) and UnlockR through a stack copy of pdef).
template <> inline const MaterialDefinition * DefinitionLockR<MaterialDefinition>(SNO sno) { return (const MaterialDefinition *)gptGroupDefMaterial->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<MaterialDefinition>(const MaterialDefinition * pdef) { gptGroupDefMaterial->UnlockR((void *&)pdef); }

typedef ScopedDefinitionLockR<MaterialDefinition> ScopedMaterialDefinitionLockR;
