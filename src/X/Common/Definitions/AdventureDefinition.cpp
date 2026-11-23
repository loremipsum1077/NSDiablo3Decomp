// X/Common/Definitions/AdventureDefinition.cpp
//
// Definition group for the "Adventure" SNO group and the reflected AdventureDefinition
// type.  AdventureGroupDef is declared in AdventureDefinition.h; by DWARF its destructor
// (line 37), its MemAlloc/MemFree overrides (line 44), CanReferenceOtherSNOs (line 47)
// and GetSNOPackDestination (line 49) are definitions that belong to that header, while
// the group singleton gptGroupDefAdventure (line 47) and the reflected type tables
// (lines 37 and 44) are definitions that belong to this file.
//
// Addresses in the 2.6.2 launch build (evidence: build/dwarf and build/ghidra listings):
//   0x710060ae90  AdventureGroupDef::AdventureGroupDef()                      136 bytes
//   0x710060af20  AdventureGroupDef::GetMaxCount()                                8
//   0x710060af30  AdventureGroupDef::GetDefaultData(void *) const                20
//   0x710060af50  AdventureGroupDef::~AdventureGroupDef()  (implicit, deleting)  36
//   0x710060af80  AdventureGroupDef::MemAlloc(size_t, uint32) const              28
//   0x710060afa0  AdventureGroupDef::MemAlloc(size_t, uint32, XMemoryAllocatorInterface *) const 40
//   0x710060afd0  AdventureGroupDef::MemFree(void *&) const                      48
//   0x710060b000  AdventureGroupDef::MemFree(void *&, XMemoryAllocatorInterface *) const 72
//   0x710060b050  AdventureGroupDef::GetSNOPackDestination(void const *) const    8
//   0x710060b060  AdventureGroupDef::CanReferenceOtherSNOs() const               8
//   0x710060b070  _GLOBAL__sub_I_AdventureDefinition.cpp                       520
//
// The 520-byte sub-initializer
// additionally contains the dynamic initializers of SNONAME_NONE
// (X/Common/SNO/SNOName.h), of the place global built from Vector3D
// (X/Common/WorldPlace.h + X/Sigma/Math/Vector.h) and of the null/default/axis colour
// constants (X/Client/Gfx/GfxVB.h + X/Sigma/RGBAColor.h), so its exact instruction order
// follows this TU's transitive include graph.  The includes below are the DWARF-owner
// headers of those globals, in the order the DWARF numbers their __cxx_global_var_init
// functions (SNOName.h, WorldPlace.h, GfxVB.h).

#include "X/Common/Definitions/Definitions.h"
#include "X/Common/Definitions/AdventureDefinition.h"
#include "X/Common/SNO/SNOName.h"
#include "X/Common/WorldPlace.h"
#include "X/Client/Gfx/GfxVB.h"
// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "X/Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "X/Common/SNO/SNOGroups.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptor.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "X/Sigma/Memory/SigmaMemory.h"

// AdventureDefinition.cpp:21
enum {
    ADVENTURE_SNO_VERSION = 25,
    ADVENTURE_XML_VERSION = 2,
};

// AdventureDefinition.cpp:37  (static const TypeDescriptorField AdventureDefinition_Fields[7])
// Reflected fields, offsets are AdventureDefinition's byte offsets: snoSymbolActor 0x0c,
// wdNoSpawnRadius 0x10, aRotationMin 0x14, aRotationMax 0x18, aRotationStep 0x1c,
// snoMarkerSet 0x20.  A null-named DT_NULL field terminates the table.  The two SNO
// fields are FilterTypeDescriptorField("", DT_SNO, <offset>, "", n, 0, -1); the derived
// temporaries are sliced into the base array, which is why the initializer emits a
// temporary construction followed by a 0xc0-byte copy.
static const TypeDescriptorField AdventureDefinition_Fields[] = {
    FilterTypeDescriptorField("", DT_SNO, 0xc, "", 1, 0, -1),
    TypeDescriptorField("", DT_FLOAT, 0x10, 0, 1),
    TypeDescriptorField("", DT_ANGLE, 0x14, 0, 1),
    TypeDescriptorField("", DT_ANGLE, 0x18, 0, 1),
    TypeDescriptorField("", DT_ANGLE, 0x1c, 0, 1),
    FilterTypeDescriptorField("", DT_SNO, 0x20, "", 0x18, 0, -1),
    TypeDescriptorField(0, DT_NULL, 0x24, 0, 0),
};

// AdventureDefinition.cpp:44  (ComplexTypeDescriptor TypeDescriptor_AdventureDefinition)
// The inlined ComplexTypeDescriptor constructor (TypeDescriptor.h:701) links the object
// onto ComplexTypeDescriptor::pRegistrationHead (TypeDescriptor.h:725-726).
ComplexTypeDescriptor TypeDescriptor_AdventureDefinition("Adventure", ADVENTURE_XML_VERSION,
                                                         AdventureDefinition_Fields, 7,
                                                         NULL, 0x80, NULL, NULL, NULL, NULL);

// AdventureDefinition.cpp:44  (const XTypeDescriptor * DT_ADVENTURE)
// The descriptor pointer the group definition passes to GroupDefinition; the data slot
// is a relocated pointer to TypeDescriptor_AdventureDefinition.
const XTypeDescriptor * DT_ADVENTURE = &TypeDescriptor_AdventureDefinition;

// AdventureDefinition.cpp:47  (AdventureGroupDef * gptGroupDefAdventure)
// The Adventure group-definition singleton, created on demand by
// DefinitionLockR<AdventureDefinition>.  Zero-initialised (.bss), so it produces no code.
AdventureGroupDef * gptGroupDefAdventure = NULL;

// 0x710060ae90, 136 bytes  (AdventureDefinition.cpp:85)
// SNOVersions(ADVENTURE_SNO_VERSION) is the one-argument SNOVersions constructor
// (SNOGroups.h:70) inlined at line 88; it expands to the five per-platform versions
// 25/27/25/25/26 (0x19/0x1b/0x19/0x19/0x1a).


AdventureGroupDef::AdventureGroupDef()
    : GroupDefinition("Adventure", 2, SNOVersions(ADVENTURE_SNO_VERSION), SNODATATYPE_TITLE,
                      sizeof(AdventureDefinition), DT_ADVENTURE, 32773, 14)
{
}

// 0x710060af20, 8 bytes  (AdventureDefinition.cpp:106; body at line 109)
uint32 AdventureGroupDef::GetMaxCount()
{
    return 340;  // 0x154
}

// 0x710060af30, 20 bytes  (AdventureDefinition.cpp:117; stores at lines 122, 126, 127)
void AdventureGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    AdventureDefinition * ptDefAdventure = (AdventureDefinition *)ptDefaultDef;
    ptDefAdventure->snoSymbolActor = SNO_NONE;    // 0x0c = -1
    ptDefAdventure->wdNoSpawnRadius = 0.0f;       // 0x10 ┐
    ptDefAdventure->aRotationMin = 0.0f;          // 0x14 │ merged into one 16-byte
    ptDefAdventure->aRotationMax = 0.0f;          // 0x18 │ zero store at line 126
    ptDefAdventure->aRotationStep = 0.0f;         // 0x1c ┘
    ptDefAdventure->snoMarkerSet = SNO_NONE;      // 0x20 = -1
}
