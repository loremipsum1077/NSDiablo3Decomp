// Original file: X/Common/Definitions/MarkerSetDefinition.cpp

#include "MarkerSetDefinition.h"

#include <new>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/Marker/Marker.cpp:141; no adopted header declares it yet.
extern const XTypeDescriptor * DT_MARKER;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp:1295; no adopted header declares it yet.
extern const XTypeDescriptor * DT_GBID;

// Defined in X/Common/MarkerSet.cpp:21; no adopted header declares it yet.
void MarkerSetInitialize(MarkerSet & tMarkerSet);

enum {
    MARKERSET_DEFINITION_VERSION = 15,
    MARKERSET_COMMON_SNO_VERSION = 114,
    MARKERSET_PC_SNO_VERSION = 115,
    MARKERSET_CONSOLE_SNO_VERSION = 119,
    MARKERSET_AXE_SNO_VERSION = 116,
};

// The field names are "" in the binary.
static const TypeDescriptorField MarkerSetDefinition_Fields[11] = {  // line 55
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MarkerSetDefinition, serMarkers), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MARKER, offsetof(MarkerSetDefinition, tMarkerSet), 0x23,
                                     (int32)offsetof(MarkerSetDefinition, serMarkers) - (int32)offsetof(MarkerSetDefinition, tMarkerSet)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MarkerSetDefinition, serNoSpawns), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CIRCLE, offsetof(MarkerSetDefinition, tNoSpawnSet), 0x22,
                                     (int32)offsetof(MarkerSetDefinition, serNoSpawns) - (int32)offsetof(MarkerSetDefinition, tNoSpawnSet)),
    TypeDescriptorField("", DT_INT, offsetof(MarkerSetDefinition, dwMarkerSetFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_AABB, offsetof(MarkerSetDefinition, aabbBounds), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(MarkerSetDefinition, nLabel), "", 11, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MarkerSetDefinition, serSpecialIndexList), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_WORD, offsetof(MarkerSetDefinition, pwSpecialIndexList), 0x20,
                                     (int32)offsetof(MarkerSetDefinition, serSpecialIndexList) - (int32)offsetof(MarkerSetDefinition, pwSpecialIndexList)),
    VariableArrayCountTypeDescriptorField("", offsetof(MarkerSetDefinition, nSpecialIndexCount), 0x10000, DT_WORD,
                                          (int32)offsetof(MarkerSetDefinition, serSpecialIndexList) - (int32)offsetof(MarkerSetDefinition, nSpecialIndexCount)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MarkerSetDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MarkerSetDefinition("MarkerSet", MARKERSET_DEFINITION_VERSION, MarkerSetDefinition_Fields, 11, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 64
const XTypeDescriptor * DT_MARKERSET = &TypeDescriptor_MarkerSetDefinition;

MarkerSetGroupDef * gptGroupDefMarkerSet;  // line 70

// line 76
MarkerSetGroupDef::MarkerSetGroupDef()
    : GroupDefinition("MarkerSet", MARKERSET_SNO,
                      SNOVersions(MARKERSET_PC_SNO_VERSION, 118, MARKERSET_CONSOLE_SNO_VERSION, MARKERSET_CONSOLE_SNO_VERSION, 120),
                      SNODATATYPE_TITLE, sizeof(MarkerSetDefinition), DT_MARKERSET, 0x4041, 0x500e) {
}

// line 103
uint32 MarkerSetGroupDef::GetMaxCount() {
    return 256;
}

// line 117
void MarkerSetGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(MarkerSetDefinition));

    MarkerSetDefinition * pMarkerSet = (MarkerSetDefinition *)ptDefaultDef;

    pMarkerSet->serMarkers.dwOffset = 0;
    pMarkerSet->serMarkers.dwSize = 0;
    MarkerSetInitialize(pMarkerSet->tMarkerSet);

    pMarkerSet->serNoSpawns.dwOffset = 0;
    pMarkerSet->serNoSpawns.dwSize = 0;
    new (&pMarkerSet->tNoSpawnSet) NoSpawnSet;

    pMarkerSet->dwMarkerSetFlags = 0;
    pMarkerSet->nLabel = GBID_NONE;
    pMarkerSet->nSpecialIndexCount = 0;
}

// line 142
BOOL MarkerSetGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    BOOL bWritten = GroupDefinition::Write(lpDefinition, tRef);
    return bWritten;
}

// line 262
void MarkerSetGroupDef::Postprocess(void * lpDefinition) {
    MarkerSetDefinition * pdef = (MarkerSetDefinition *)lpDefinition;
    GroupDefinition::Postprocess(pdef);
}

// line 318
void MarkerSetGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}
