// X/Client/Definitions/MaterialDefinition.cpp
//
// The "Material" group definition (MATERIAL_SNO 57) and the reflected
// MaterialDefinition type registration.  Reconstructed from the DWARF and the
// line-attributed listings (build/dwarf/..., build/ghidra/decomp/...).

// blz/tuple.h for blz::ignore, whose initializer the original runs in this file's
// static initializer ahead of SNONAME_NONE.  It must precede MaterialDefinition.h,
// whose include chain (GfxMaterial.h -> SNOFiles.h) reaches SNOName.h.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

#include "MaterialDefinition.h"

// For SNONAME_NONE (SNOName.h:142), cgplaceNull (WorldPlace.h:23) and the four
// RGBAColor normals (GfxVB.h:351-354), whose initializers the original runs in this
// file's static initializer.  The DWARF does not record the include chain that
// reaches them.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"

#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOGroups.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/TagMaps/TagNumbers.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20 (DT_SERIALIZEDATA) and in
// X/Client/Gfx/GfxMaterial.cpp:68 (DT_UBERMATERIAL); GfxMaterial.h does not declare
// DT_UBERMATERIAL yet, so declare them the way the sibling definition files do.
// GOT slots 0x10e9450 / 0x10e95b8 (DT_MATERIALDEFINITION uses 0x10e9000+0x4500).
extern const XTypeDescriptor * DT_SERIALIZEDATA;
extern const XTypeDescriptor * DT_UBERMATERIAL;

// X/Client/Definitions/MaterialDefinition.cpp:20
enum {
    MATERIALDEFINITION_COMMON_SNO_VERSION = 25,
    MATERIALDEFINITION_XML_VERSION = 2,
    MATERIALDEFINITION_PC_SNO_VERSION = 25,
    MATERIALDEFINITION_CONSOLE_SNO_VERSION = 31,
    MATERIALDEFINITION_AXE_SNO_VERSION = 29,
    MATERIALDEFINITION_XBOXONE_SNO_VERSION = 31,
    MATERIALDEFINITION_PS4_SNO_VERSION = 31,
    MATERIALDEFINITION_NX64_SNO_VERSION = 36,
};

// Line 42.  0x71017a6a40, 5 * 0xc0 bytes.  The 3rd entry is stored through a
// TagMapTypeDescriptorField temporary and copy-constructed into the
// TypeDescriptorField array (matches _GLOBAL__sub_I at 0x71001ae560..0x71001ae588).
static const TypeDescriptorField MaterialDefinition_Fields[5] = {
    TypeDescriptorField("", DT_INT, offsetof(MaterialDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MaterialDefinition, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(MaterialDefinition, hTagMap), 0x21,
                              (int32)offsetof(MaterialDefinition, serTagMap) - (int32)offsetof(MaterialDefinition, hTagMap), TAG_APPEARANCE),
    TypeDescriptorField("", DT_UBERMATERIAL, offsetof(MaterialDefinition, tUberMaterial), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MaterialDefinition), NULL, 0),
};

// Line 47.  0x71017a6e00.  DWARF: ComplexTypeDescriptor(LPCSTR szClassName,
// int32 nVersion, const TypeDescriptorField*, uint32 nFieldCount, ...);
// the remaining arguments are NULL / 0x80, matching the sibling descriptors.
ComplexTypeDescriptor TypeDescriptor_MaterialDefinition("Material", 2, MaterialDefinition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);

// Line 47.  0x710103ca30 (8-byte relocatable pointer table next to the vtable).
const XTypeDescriptor * DT_MATERIALDEFINITION = &TypeDescriptor_MaterialDefinition;

// Line 53.  0x71017a6e60 (.bss; the SNOName block the static initializer writes
// starts at +0x10, 0x71017a6e70).
MaterialGroupDef * gptGroupDefMaterial;

// 0x71001ae2c0  size 132
// SNOVersions stores the platform versions with AXE/NX64 already carrying their
// base version (see SNOGroups.h: "left to the caller"): the original stores
// dwVersions = {0x19, 0x1f, 0x1f, 0x1f, 0x25} = {25, 29+2, 31, 31, 36+1}.
MaterialGroupDef::MaterialGroupDef()
    : GroupDefinition("Material", MATERIAL_SNO,
                      SNOVersions(MATERIALDEFINITION_PC_SNO_VERSION,
                                  MATERIALDEFINITION_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  MATERIALDEFINITION_XBOXONE_SNO_VERSION,
                                  MATERIALDEFINITION_PS4_SNO_VERSION,
                                  MATERIALDEFINITION_NX64_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(MaterialDefinition),
                      DT_MATERIALDEFINITION, 0x2241, 0x500a)
{
}

// 0x71001ae350  size 8
uint32 MaterialGroupDef::GetMaxCount()
{
    return 0x80;
}

// 0x71001ae360  size 4
void MaterialGroupDef::PurgeDefinitionAndSNO(const SNO sno)
{
    GroupDefinition::PurgeDefinitionAndSNO(sno);
}

// 0x71001ae370  size 4
BOOL MaterialGroupDef::Write(void * lpDefinition, FileReference & tRef) const
{
    return GroupDefinition::Write(lpDefinition, tRef);
}
