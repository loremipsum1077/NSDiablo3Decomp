// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ExplosionDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;

enum ExplosionType {
    EXPLOSION_TYPE_RADIAL = 0,
    EXPLOSION_TYPE_PARENT_DIRECTION = 1,
    EXPLOSION_TYPE_WORLD_DIRECTION = 2,
};

enum {
    EXPLOSION_MELEE_BIT = 0,
};

struct ExplosionDefinition {  // size 0x40
    RequiredDefinitionHeader tHeader;  // 0x00
    ExplosionType eType;  // 0x0c
    uint32 dwFlags;  // 0x10
    Vector3D wvDirection;  // 0x14
    float flMagnitude;  // 0x20
    float flMinRadius;  // 0x24
    float flDeltaRadius;  // 0x28
    float flMinScale;  // 0x2c
    float flDeltaScale;  // 0x30
    int32 nDuration;  // 0x34
    float flChance;  // 0x38
    uint32 dwPad;  // 0x3c
};
static_assert(sizeof(ExplosionDefinition) == 0x40, "ExplosionDefinition");

class ExplosionGroupDef : public GroupDefinition {  // size 0xa0
public:
    ExplosionGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 57; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 60
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 62
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ExplosionGroupDef) == 0xa0, "ExplosionGroupDef");

extern ExplosionGroupDef * gptGroupDefExplosion;  // ExplosionDefinition.cpp:57

class XTypeDescriptor;
extern const XTypeDescriptor * DT_EXPLOSION;  // ExplosionDefinition.cpp:51


// Functions defined in this file (bodies are not in the DWARF):
//   line    52  ExplosionGroupDef::~ExplosionGroupDef()  [0x71001ac030]
//   line    57  void * ExplosionGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x71001ac060 (+1 more)]
//   line    57  void ExplosionGroupDef::MemFree(void *& ptr) const  [0x71001ac0b0 (+1 more)]
//   line    60  BOOL ExplosionGroupDef::CanReferenceOtherSNOs() const  [0x71001ac140]
//   line    62  SNOPackType ExplosionGroupDef::GetSNOPackDestination(const void *) const  [0x71001ac130]
