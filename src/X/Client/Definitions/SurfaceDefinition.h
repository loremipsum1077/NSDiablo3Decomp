// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/SurfaceDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class CTagMap;  // X/Common/TagMaps/Tags.h; only held through XPtr64 here
class XMemoryAllocatorInterface;

enum SurfaceType {
    SURFACETYPE_GRASS = 0,
    SURFACETYPE_DIRT = 1,
    SURFACETYPE_STONE = 2,
    SURFACETYPE_METAL = 3,
    SURFACETYPE_LAVA = 4,
    SURFACETYPE_ICE = 5,
    SURFACETYPE_FLESH = 6,
    SURFACETYPE_WATER = 7,
    SURFACETYPE_METAL_GRATE = 8,
    SURFACETYPE_PAVEMENT = 9,
    SURFACETYPE_SLIME = 10,
    SURFACETYPE_WOOD = 11,
    SURFACETYPE_CARPET = 12,
    SURFACETYPE_BONE = 13,
    SURFACETYPE_SNOW = 14,
    SURFACETYPE_BLOOD = 15,
    NUM_SURFACETYPES = 16,
};

struct SurfaceDefinition {  // size 0x28
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    SNO snoParticleSystem;  // 0x10
    SurfaceType eType;  // 0x14
    SerializeData serTagMap;  // 0x18
    XPtr64<CTagMap> hTagMap;  // 0x20
};
static_assert(sizeof(SurfaceDefinition) == 0x28, "SurfaceDefinition");

class SurfaceGroupDef : public GroupDefinition {  // size 0xa0
public:
    SurfaceGroupDef();
    // Line 58 of the original header. Inline there, so the deleting destructor
    // (0x71001b7b10) is emitted from this TU together with the vtable.
    virtual ~SurfaceGroupDef() {}
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 63; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    // Inline in the original (lines 66 and 68).
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const { return (SNOPackType)1; }  // vtable slot 7
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(SurfaceGroupDef) == 0xa0, "SurfaceGroupDef");

extern SurfaceGroupDef * gptGroupDefSurface;  // SurfaceDefinition.cpp:49

typedef ScopedDefinitionLockR<SurfaceDefinition> ScopedSurfaceDefinitionLockR;

// Line 96. sSurfaceIsLiquid (StructureOctree.cpp) calls LockRFast and has nothing from an unlock,
// so DefinitionUnlockR<SurfaceDefinition> is taken to be empty, as for FlagSetDefinition.
template <> inline const SurfaceDefinition * DefinitionLockR<SurfaceDefinition>(SNO sno) {
    return (const SurfaceDefinition *)gptGroupDefSurface->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<SurfaceDefinition>(const SurfaceDefinition *) {}

// Functions defined in this file (bodies are not in the DWARF):
//   line    58  SurfaceGroupDef::~SurfaceGroupDef()  [0x71001b7b10]
//   line    63  void * SurfaceGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x71001b7b40 (+1 more)]
//   line    63  void SurfaceGroupDef::MemFree(void *& ptr) const  [0x71001b7b90 (+1 more)]
//   line    66  BOOL SurfaceGroupDef::CanReferenceOtherSNOs() const  [0x71001b7c20]
//   line    68  SNOPackType SurfaceGroupDef::GetSNOPackDestination(const void *) const  [0x71001b7c10]
//   line    96  const SurfaceDefinition * DefinitionLockR<SurfaceDefinition>(SNO sno)  [inlined into 2 functions]
