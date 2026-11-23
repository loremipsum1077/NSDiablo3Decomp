// X/Client/Definitions/SurfaceDefinition.cpp
//
// Diablo III 2.6.2 (Switch, clang 5.0.1).  Reflection description of the
// "Surface" SNO group (43) plus the SurfaceGroupDef members that are not
// inline in SurfaceDefinition.h.  All line comments are the original file's
// line numbers recorded by the DWARF.
//
// The dynamic initialiser at 0x71001b7c30 builds SurfaceDefinition_Fields
// (:38, six field descriptors) and TypeDescriptor_SurfaceDefinition (:43).
// The DT_* built-ins are defined in X/Sigma/TypeDescriptor/DescribedType.cpp
// (lines 742-767), except DT_SNO, which is defined in X/Common/D3TypeDescriptor/
// D3DescribedType.cpp:321; X/Sigma/TypeDescriptor/TypeDescriptor.h only declares
// them, together with ComplexTypeDescriptor and XTypeDescriptor.  The sibling
// 1.0 build reaches the DT_* declarations through X/Sigma/TypeDescriptor/
// DescribedType.h, which does not exist under src/ or include/ here.

#include "SurfaceDefinition.h"

// For SNONAME_NONE (SNOName.h:142), cgplaceNull (WorldPlace.h:23) and the four RGBAColor
// normals (GfxVB.h:351-354), whose initializers the original runs in this file's static
// initializer (0x71001b7c44..0x71001b7c8c).  The DWARF does not record the include chain
// that reaches them.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
// blz::ignore has a user-provided empty constructor, so the compiler numbers it a code-free
// dynamic initialiser -- the __cxx_global_var_init.6 the DWARF does not list.  It is the
// object whose one byte the merged normals block reserves at its base, which is why the
// four normals written by 0x71001b7c70..0x71001b7cc0 sit at base+4; SoundDefinition.cpp
// includes tuple.h after GfxVB.h for the same reason.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/TagMaps/TagNumbers.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20 (DT_SERIALIZEDATA) and in
// X/Common/D3TypeDescriptor/D3DescribedType.cpp:321 (DT_SNO).  The DWARF records where they
// are defined but not which header declares them, so they are declared here the way the
// sibling definition files do (GOT slots 0x10e9450 and 0x10e9500).
extern const XTypeDescriptor * DT_SERIALIZEDATA;
extern const XTypeDescriptor * DT_SNO;

// :16
enum {
    SURFACE_XML_VERSION = 1,
    SURFACE_COMMON_SNO_VERSION = 21,
    SURFACE_PC_SNO_VERSION = 21,
    SURFACE_CONSOLE_SNO_VERSION = 23,
    SURFACE_AXE_SNO_VERSION = 21,
};

// :38  Six field descriptors.  Offsets, flags and the tag base (0x50000,
// TAG_SURFACE) are the immediates of 0x71001b7c90..0x71001b7d9c.  Elements 1
// and 4 carry proxy field types, so clang constructs them in a temporary and
// copies the 0xc0 bytes in with the copy routine at 0x7100c1d410, exactly as
// the mixed-type aggregate below is lowered.
static const TypeDescriptorField SurfaceDefinition_Fields[6] = {
    TypeDescriptorField("", DT_INT, offsetof(SurfaceDefinition, dwFlags), NULL, 0x80001),  // :39  TDF_XML_SERIALIZATION|TDF_BIT_FLAGS
    FilterTypeDescriptorField("", DT_SNO, offsetof(SurfaceDefinition, snoParticleSystem), "",  // :40
                              27, 0, -1),  // the SNO group filter is the immediate 0x1b
    TypeDescriptorField("", DT_INT, offsetof(SurfaceDefinition, eType), NULL, 1),  // :41
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SurfaceDefinition, serTagMap), NULL, 0),  // :42
    TagMapTypeDescriptorField("", offsetof(SurfaceDefinition, hTagMap), 0x21,     // :42
                              (int32)offsetof(SurfaceDefinition, serTagMap) - (int32)offsetof(SurfaceDefinition, hTagMap),
                              TAG_SURFACE),  // 0x50000 = TAG_SURFACE
    TypeDescriptorField(NULL, DT_NULL, sizeof(SurfaceDefinition), NULL, 0),  // :43
};

// :43  nVersion = SURFACE_XML_VERSION, six fields; the trailing arguments (NULL / 0x80)
// are the immediates of 0x71001b7da0..0x71001b7de4.
ComplexTypeDescriptor TypeDescriptor_SurfaceDefinition("Surface", SURFACE_XML_VERSION, SurfaceDefinition_Fields, 6,
                                                       NULL, 0x80, NULL, NULL, NULL, NULL);

// :43
const XTypeDescriptor * DT_SURFACE = &TypeDescriptor_SurfaceDefinition;

// :49
SurfaceGroupDef * gptGroupDefSurface;

// :55  SNOVersions(21,23,23,23,24) is built at 0x71001b7a50..0x71001b7a6c;
// the group is SURFACE_SNO (43), the uint32 is 40 (sizeof(SurfaceDefinition)),
// the descriptor is DT_SURFACE, the data type is SNODATATYPE_TITLE (0), and the
// flags 0x2005 and the trailing 10 are the immediates at 0x71001b7a90/0x71001b7aa0.
SurfaceGroupDef::SurfaceGroupDef()
    : GroupDefinition("Surface", SURFACE_SNO,
                      SNOVersions(SURFACE_PC_SNO_VERSION,
                                  SURFACE_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  SURFACE_CONSOLE_SNO_VERSION,
                                  SURFACE_CONSOLE_SNO_VERSION,
                                  SURFACE_CONSOLE_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(SurfaceDefinition), DT_SURFACE, 0x2005, 10)
{
}

// :75 (body :78)
uint32 SurfaceGroupDef::GetMaxCount()
{
    return 32;
}

// :87 (body :89)
void SurfaceGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    SurfaceDefinition * pdef = (SurfaceDefinition *)ptDefaultDef;  // :89
    SigmaMemoryClear(ptDefaultDef, sizeof(SurfaceDefinition));     // :90
    pdef->snoParticleSystem = SNO_NONE;                            // :93
}
