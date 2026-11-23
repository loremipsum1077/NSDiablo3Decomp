// Original file: X/Client/Definitions/RopeDefinition.cpp

#include "RopeDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../../Common/WorldPlace.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../Gfx/GfxVB.h"
// For MsgTriggeredEvent and DT_MSGTRIGGEREDEVENT.
#include "../../Common/Definitions/AttachmentDefinition.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

enum {  // line 22
    ROPE_COMMON_SNO_VERSION = 77,
    ROPE_XML_VERSION = 4,
    ROPE_PC_SNO_VERSION = 77,
    ROPE_CONSOLE_SNO_VERSION = 86,
    ROPE_AXE_SNO_VERSION = 84,
    ROPE_XBOXONE_SNO_VERSION = 86,
    ROPE_PS4_SNO_VERSION = 86,
    ROPE_NX64_SNO_VERSION = 86,
};

static const TypeDescriptorField RopeDefinition_Fields[39] = {  // line 45
    TypeDescriptorField("", DT_INT, offsetof(RopeDefinition, dwRopeFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(RopeDefinition, nWidth), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RopeDefinition, flKnotsPerFoot), NULL, 1),
    TypeDescriptorField("", DT_MASS, offsetof(RopeDefinition, flMass), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RopeDefinition, flMassTaperFactor), NULL, 1),
    TypeDescriptorField("", DT_MASS, offsetof(RopeDefinition, flMassEnd), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RopeDefinition, fTextureRepeatCount), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RopeDefinition, flStiffness), NULL, 1),
    TypeDescriptorField("", DT_UBERMATERIAL, offsetof(RopeDefinition, tUberMaterial), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(RopeDefinition, snoPhysics), "", PHYSICS_SNO, 0, -1),
    TypeDescriptorField("", DT_TIME, offsetof(RopeDefinition, tSystemDurationMin), (const void *)60, 1),
    TypeDescriptorField("", DT_TIME, offsetof(RopeDefinition, tSystemDurationDelta), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(RopeDefinition, tOutroDurationMin), (const void *)30, 1),
    TypeDescriptorField("", DT_TIME, offsetof(RopeDefinition, tOutroDurationDelta), NULL, 1),
    TypeDescriptorField("", DT_COLOR_PATH, offsetof(RopeDefinition, pathColorOverTime), &cgpathNullColorPath, 1),
    TypeDescriptorField("", DT_COLOR_PATH, offsetof(RopeDefinition, pathColorOverLength), &cgpathNullColorPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathAlphaOverTime), &cgpathUnitFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathAlphaOverLength), &cgpathUnitFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathWidthOverTime), &cgpathUnitFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathWidthOverLength), &cgpathUnitFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathSourcePointWorldUpMotion), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathSourcePointAlongRopeMotion), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathSourcePointPerpendicularMotion), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathDestPointWorldUpMotion), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathDestPointAlongRopeMotion), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathDestPointPerpendicularMotion), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_VECTOR_PATH, offsetof(RopeDefinition, pathJitterAmplitude), &cgpathNullVectorPath, 1),
    TypeDescriptorField("", DT_VELOCITY_PATH, offsetof(RopeDefinition, pathJitterRate), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RopeDefinition, flJitterApproachSpeed), (const void *)0x3c23d70a, 1),  // 0.01f
    TypeDescriptorField("", DT_FLOAT, offsetof(RopeDefinition, flExpectedLength), NULL, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathSeparation), &cgpathUnitFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathXAmpOverLength), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathYAmpOverLength), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathFreqOverTime), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_FLOAT_PATH, offsetof(RopeDefinition, pathPhaseSpeedOverTime), &cgpathNullFloatPath, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RopeDefinition, serMsgTriggeredEvents), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MSGTRIGGEREDEVENT, offsetof(RopeDefinition, ptMsgTriggeredEvents), 0x21,
                                     (int32)offsetof(RopeDefinition, serMsgTriggeredEvents) - (int32)offsetof(RopeDefinition, ptMsgTriggeredEvents)),
    VariableArrayCountTypeDescriptorField("", offsetof(RopeDefinition, nMsgTriggeredEventCount), 0x10000, DT_MSGTRIGGEREDEVENT,
                                          (int32)offsetof(RopeDefinition, serMsgTriggeredEvents) - (int32)offsetof(RopeDefinition, nMsgTriggeredEventCount)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RopeDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RopeDefinition("Rope", ROPE_XML_VERSION, RopeDefinition_Fields, 39, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 89
const XTypeDescriptor * DT_ROPE = &TypeDescriptor_RopeDefinition;

RopeGroupDef * gptGroupDefRope;  // line 95

// line 101
RopeGroupDef::RopeGroupDef()
    : GroupDefinition("Rope", ROPE_SNO,
                      SNOVersions(ROPE_PC_SNO_VERSION,
                                  ROPE_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  ROPE_XBOXONE_SNO_VERSION,
                                  ROPE_PS4_SNO_VERSION,
                                  ROPE_NX64_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(RopeDefinition), DT_ROPE, 0x2241, 0x501a) {
}

// line 131
uint32 RopeGroupDef::GetMaxCount() {
    return 128;
}

// line 143
BOOL RopeGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    return GroupDefinition::Write(lpDefinition, tRef);
}

// line 184
void RopeGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(RopeDefinition));

    RopeDefinition * pdef = (RopeDefinition *)ptDefaultDef;
    pdef->nWidth = 2;
    pdef->flKnotsPerFoot = 0.1f;
    pdef->flMass = 10.0f / 32.2f;
    pdef->flMassTaperFactor = 0.0f;
    pdef->flMassEnd = 10.0f;
    pdef->fTextureRepeatCount = 1.0f;
    pdef->flStiffness = 0.0f;

    MaterialSetDefaultValues(pdef->tUberMaterial);

    pdef->snoPhysics = SNO_NONE;
    pdef->tSystemDurationMin = 60;
    pdef->tSystemDurationDelta = 0;
    pdef->tOutroDurationMin = 30;
    pdef->tOutroDurationDelta = 0;

    pdef->pathColorOverTime = cgpathNullColorPath;
    pdef->pathColorOverLength = cgpathNullColorPath;
    pdef->pathAlphaOverTime = cgpathUnitFloatPath;
    pdef->pathAlphaOverLength = cgpathUnitFloatPath;
    pdef->pathWidthOverTime = cgpathUnitFloatPath;
    pdef->pathWidthOverLength = cgpathUnitFloatPath;
    pdef->pathSourcePointWorldUpMotion = cgpathNullFloatPath;
    pdef->pathSourcePointAlongRopeMotion = cgpathNullFloatPath;
    pdef->pathSourcePointPerpendicularMotion = cgpathNullFloatPath;
    pdef->pathDestPointWorldUpMotion = cgpathNullFloatPath;
    pdef->pathDestPointAlongRopeMotion = cgpathNullFloatPath;
    pdef->pathDestPointPerpendicularMotion = cgpathNullFloatPath;
    pdef->pathJitterAmplitude = cgpathNullVectorPath;
    pdef->pathJitterRate = cgpathNullFloatPath;
    pdef->flJitterApproachSpeed = 0.01f;
    pdef->flExpectedLength = 0.0f;
    pdef->pathXAmpOverLength = cgpathNullFloatPath;
    pdef->pathYAmpOverLength = cgpathNullFloatPath;
    pdef->pathFreqOverTime = cgpathNullFloatPath;
    pdef->pathPhaseSpeedOverTime = cgpathNullFloatPath;
    pdef->pathSeparation = cgpathUnitFloatPath;
}
