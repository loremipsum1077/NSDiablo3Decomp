// Original file: X/Common/Definitions/ObserverDefinition.cpp

#include "ObserverDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/Math/PointSpherical.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

enum {
    OBSERVER_SNO_VERSION = 57,
};

static void Observer_ComputeSphericalFromCameraOffset(ObserverDefinition * pDef);

// The field names are "" in the binary.
static const TypeDescriptorField ObserverDefinition_Fields[28] = {  // line 26
    TypeDescriptorField("", DT_INT, offsetof(ObserverDefinition, fOrthographic), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flOrthographicHeight), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ObserverDefinition, aFOV), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flZFar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flZNear), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, wdMaxVelForward), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ObserverDefinition, aYaw), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(ObserverDefinition, aPitch), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flDist), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ObserverDefinition, wvCameraOffsetVector), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ObserverDefinition, wvLookAtOffsetVector), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, wdMicrophoneHeight), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, wdMicrophoneXYOffset), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, wdMicrophonePanningDistance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flRolloffFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flDopplerFactor), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleSpringFollowCameraMinSpeed), (const void *)0x40400000, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleSpringFollowCameraMaxSpeed), (const void *)0x41f00000, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleSpringFollowCameraSpringFactor), (const void *)0x43480000, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleSpringFollowCameraSpringExponent), (const void *)0x3fc00000, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleCameraZoomSpeed), (const void *)0x3f800000, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleOrbitSLerpScalar), (const void *)0x40400000, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleOrbitTransitionExponentialScalar), (const void *)0xc0e00000, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleOrbitTransitionDuration), (const void *)0x40000000, 0x700001),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ObserverDefinition, wvConsoleOrbitOffsetCenterForAngleLimiting), &cgwvXAxis, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(ObserverDefinition, flConsoleOrbitMaxAngleFromOffsetCenter), NULL, 0x700001),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ObserverDefinition, vConsoleCameraShakeScalar), &cgwvOne, 0x700001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ObserverDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ObserverDefinition("Observer", 0, ObserverDefinition_Fields, 28, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 61
const XTypeDescriptor * DT_OBSERVER = &TypeDescriptor_ObserverDefinition;

ObserverGroupDef * gptGroupDefObserver;  // line 67

ObserverGroupDef::ObserverGroupDef()
    : GroupDefinition("Observer", OBSERVER_SNO, SNOVersions(OBSERVER_SNO_VERSION), SNODATATYPE_TITLE, sizeof(ObserverDefinition),
                      DT_OBSERVER, 0x2005, 30) {
}

uint32 ObserverGroupDef::GetMaxCount() {
    return 48;
}

void ObserverGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(ObserverDefinition));

    ObserverDefinition * pdef = (ObserverDefinition *)ptDefaultDef;
    pdef->fOrthographic = FALSE;
    pdef->flOrthographicHeight = 50.0f;
    pdef->aFOV = PI / 4.0f;
    pdef->flZFar = 1000.0f;
    pdef->flZNear = 1.0f;
    pdef->wdMaxVelForward = 1.0f;
    pdef->wvCameraOffsetVector.Set(0.0f, 1.0f, 1.0f);
    pdef->wvLookAtOffsetVector.Set(0.0f, 0.0f, 2.0f);

    Observer_ComputeSphericalFromCameraOffset(pdef);
}

static void Observer_ComputeSphericalFromCameraOffset(ObserverDefinition * pDef) {
    PointSpherical psOffset;
    psOffset.Set(pDef->wvCameraOffsetVector, cgwpNull);
    pDef->aYaw = psOffset.aYaw;
    pDef->aPitch = psOffset.aPitch;
    pDef->flDist = Math::FloatRoundToXPlaces(psOffset.wdRadius, 2);
}
