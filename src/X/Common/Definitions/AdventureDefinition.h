// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/AdventureDefinition.h
// Only what SmartAdventureMarker.cpp needs is written here: the definition struct, the
// group-definition class that owns the locked pointer and the locked pointer itself. The
// MemAlloc/MemFree bodies come from the shared GROUPDEFINITION_MEMORY_FUNCTIONS() macro
// (DWARF line 44); only CanReferenceOtherSNOs (line 47), GetSNOPackDestination (line 49)
// and DefinitionLockR<AdventureDefinition> (line 79) are left without bodies, and none
// of it is called from this file, so no body is invented here.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

struct AdventureDefinition {  // size 0x24, line 21
    RequiredDefinitionHeader tHeader;  // 0x00
    SNO snoSymbolActor;  // 0x0c
    float wdNoSpawnRadius;  // 0x10
    Angle aRotationMin;  // 0x14
    Angle aRotationMax;  // 0x18
    Angle aRotationStep;  // 0x1c
    SNO snoMarkerSet;  // 0x20
};
static_assert(sizeof(AdventureDefinition) == 0x24, "AdventureDefinition");

class AdventureGroupDef : public GroupDefinition {  // size 0xa0, line 37
public:
    AdventureGroupDef();  // AdventureDefinition.cpp:85
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 44; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11, AdventureDefinition.cpp:106
    virtual BOOL CanReferenceOtherSNOs() const;  // vtable slot 13, line 47 (body not written here)
    virtual SNOPackType GetSNOPackDestination(const void *) const;  // vtable slot 7, line 49 (ditto)
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21, AdventureDefinition.cpp:117
};
static_assert(sizeof(AdventureGroupDef) == 0xa0, "AdventureGroupDef");

// Declared here, defined in AdventureDefinition.cpp (line 47). The type descriptor tables
// (DT_ADVENTURE, AdventureDefinition.cpp:44) and the static initializers of that file are
// not adopted here because nothing in this file uses them.
extern AdventureGroupDef * gptGroupDefAdventure;  // AdventureDefinition.cpp:47

typedef ScopedDefinitionLockR<AdventureDefinition> ScopedAdventureDefinitionLockR;

// Line 79. AdventureSpawn (Adventure.cpp:318) has LockRFast from the lock and nothing from an
// unlock, so the unlock is taken to be empty, as for TutorialDefinition.h.
template <> inline const AdventureDefinition * DefinitionLockR<AdventureDefinition>(SNO sno) {  // line 79
    return (const AdventureDefinition *)gptGroupDefAdventure->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<AdventureDefinition>(const AdventureDefinition *) {}

// The two bodies the DWARF places in this header: CanReferenceOtherSNOs (defined at line
// 47) and GetSNOPackDestination (defined at line 49).  Both are virtual overrides of
// GroupDefinition, so AdventureGroupDef's vtable -- emitted from
// src/X/Common/Definitions/AdventureDefinition.cpp, the TU that defines the class's key
// function GetMaxCount -- forces their out-of-line copies into that object file.  The
// earlier "body not written here" note above is superseded for those two.
inline BOOL AdventureGroupDef::CanReferenceOtherSNOs() const
{
    return TRUE;  // 0x710060b060: orr w0, wzr, #1
}

inline SNOPackType AdventureGroupDef::GetSNOPackDestination(const void *) const
{
    return (SNOPackType)0;  // 0x710060b050: mov w0, wzr
}
