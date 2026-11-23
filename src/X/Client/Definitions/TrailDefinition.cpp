// Original file: X/Client/Definitions/TrailDefinition.cpp

#include "TrailDefinition.h"

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

// Defined in X/Common/SNO/SymbolicValue.cpp:3557; no adopted header declares it yet.
extern const SymbolicEntry gTrailType[4];

enum {  // line 22
    TRAIL_COMMON_SNO_VERSION = 39,
    TRAIL_XML_VERSION = 3,
    TRAIL_PC_SNO_VERSION = 39,
    TRAIL_CONSOLE_SNO_VERSION = 44,
    TRAIL_AXE_SNO_VERSION = 43,
    TRAIL_XBOXONE_SNO_VERSION = 44,
    TRAIL_PS4_SNO_VERSION = 44,
};

static const TypeDescriptorField TrailDefinition_Fields[15] = {  // line 40
    TypeDescriptorField("", DT_INT, offsetof(TrailDefinition, dwTrailDefFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(TrailDefinition, nLength), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TrailDefinition, nLengthDelta), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TrailDefinition, tSystemDurationMin), (const void *)60, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TrailDefinition, tSystemDurationDelta), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TrailDefinition, tOutroDurationMin), (const void *)30, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TrailDefinition, tOutroDurationDelta), NULL, 1),
    TypeDescriptorField("", DT_COLOR_PATH, offsetof(TrailDefinition, pathColorOverTime), &cgpathNullColorPath, 1),
    TypeDescriptorField("", DT_COLOR_PATH, offsetof(TrailDefinition, pathColorOverLength), &cgpathNullColorPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(TrailDefinition, pathAlphaOverTime), &cgpathUnitFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(TrailDefinition, pathAlphaOverLength), &cgpathUnitFloatPath, 1),
    EnumTypeDescriptorField("", offsetof(TrailDefinition, nTrailType), NULL, gTrailType),
    TypeDescriptorField("", DT_INT, offsetof(TrailDefinition, nCircleSegments), NULL, 1),
    TypeDescriptorField("", DT_UBERMATERIAL, offsetof(TrailDefinition, tUberMaterial), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TrailDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TrailDefinition("Trail", TRAIL_XML_VERSION, TrailDefinition_Fields, 15, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 61
const XTypeDescriptor * DT_TRAIL = &TypeDescriptor_TrailDefinition;

TrailGroupDef * gptGroupDefTrail;  // line 67

// line 73
TrailGroupDef::TrailGroupDef()
    : GroupDefinition("Trail", TRAIL_SNO,
                      SNOVersions(TRAIL_PC_SNO_VERSION,
                                  TRAIL_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  TRAIL_XBOXONE_SNO_VERSION,
                                  TRAIL_PS4_SNO_VERSION,
                                  TRAIL_CONSOLE_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(TrailDefinition), DT_TRAIL, 0x2241, 0x501a) {
}

// line 99
uint32 TrailGroupDef::GetMaxCount() {
    return 32;
}

// line 111
void TrailGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(TrailDefinition));

    TrailDefinition * pdef = (TrailDefinition *)ptDefaultDef;
    MaterialSetDefaultValues(pdef->tUberMaterial);

    pdef->nLength = 1;
    pdef->tSystemDurationMin = 60;
    pdef->tSystemDurationDelta = 0;
    pdef->tOutroDurationMin = 30;
    pdef->tOutroDurationDelta = 0;
    pdef->pathColorOverTime = cgpathNullColorPath;
    pdef->pathColorOverLength = cgpathNullColorPath;
    pdef->pathAlphaOverTime = cgpathUnitFloatPath;
    pdef->pathAlphaOverLength = cgpathUnitFloatPath;
    pdef->nTrailType = TRAIL_LINE;
}

// line 134
BOOL TrailGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    return GroupDefinition::Write(lpDefinition, tRef);
}
