// Original file: X/Client/Definitions/ExplosionDefinition.cpp

#include "ExplosionDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../../Common/WorldPlace.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_IMPULSE;

enum {
    EXPLOSION_SNO_COMMON_VERSION = 29,
    EXPLOSION_XML_VERSION = 2,
    EXPLOSION_SNO_PC_VERSION = 29,
    EXPLOSION_SNO_CONSOLE_VERSION = 30,
    EXPLOSION_SNO_AXE_VERSION = 31,
};

static const TypeDescriptorField ExplosionDefinition_Fields[12] = {  // line 37
    TypeDescriptorField("", DT_INT, offsetof(ExplosionDefinition, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ExplosionDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ExplosionDefinition, wvDirection), NULL, 1),
    TypeDescriptorField("", DT_IMPULSE, offsetof(ExplosionDefinition, flMagnitude), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ExplosionDefinition, flMinRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ExplosionDefinition, flDeltaRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ExplosionDefinition, flMinScale), (const void *)0x3f800000, 1),  // 1.0f
    TypeDescriptorField("", DT_FLOAT, offsetof(ExplosionDefinition, flDeltaScale), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ExplosionDefinition, nDuration), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ExplosionDefinition, flChance), (const void *)0x42c80000, 1),  // 100.0f
    TypeDescriptorField("", DT_INT, offsetof(ExplosionDefinition, dwPad), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ExplosionDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ExplosionDefinition("Explosion", EXPLOSION_XML_VERSION, ExplosionDefinition_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 51
const XTypeDescriptor * DT_EXPLOSION = &TypeDescriptor_ExplosionDefinition;

ExplosionGroupDef * gptGroupDefExplosion;  // line 57

// line 63
ExplosionGroupDef::ExplosionGroupDef()
    : GroupDefinition("Explosion", EXPLOSION_SNO,
                      SNOVersions(EXPLOSION_SNO_PC_VERSION,
                                  EXPLOSION_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  EXPLOSION_SNO_CONSOLE_VERSION,
                                  EXPLOSION_SNO_CONSOLE_VERSION,
                                  EXPLOSION_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(ExplosionDefinition), DT_EXPLOSION, 0x2005, 8) {
}

// line 79
uint32 ExplosionGroupDef::GetMaxCount() {
    return 400;
}

// line 91
void ExplosionGroupDef::GetDefaultData(void * ptDefaultDef) const {
    ExplosionDefinition * pdef = static_cast<ExplosionDefinition *>(ptDefaultDef);
    SigmaMemoryClear(pdef, sizeof(ExplosionDefinition));
    pdef->eType = EXPLOSION_TYPE_RADIAL;
    pdef->flMinScale = 1.0f;
    pdef->flChance = 100.0f;
}
