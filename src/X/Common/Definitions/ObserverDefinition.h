// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/ObserverDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;
class XTypeDescriptor;

struct ObserverDefinition {  // size 0x98
    RequiredDefinitionHeader tHeader;  // 0x00
    BOOL fOrthographic;  // 0x0c
    float flOrthographicHeight;  // 0x10
    Angle aFOV;  // 0x14
    float flZFar;  // 0x18
    float flZNear;  // 0x1c
    WorldDistance wdMaxVelForward;  // 0x20
    Angle aYaw;  // 0x24
    Angle aPitch;  // 0x28
    float flDist;  // 0x2c
    Vector3D wvCameraOffsetVector;  // 0x30
    Vector3D wvLookAtOffsetVector;  // 0x3c
    WorldDistance wdMicrophoneHeight;  // 0x48
    WorldDistance wdMicrophoneXYOffset;  // 0x4c
    WorldDistance wdMicrophonePanningDistance;  // 0x50
    float flRolloffFactor;  // 0x54
    float flDopplerFactor;  // 0x58
    float flConsoleSpringFollowCameraMinSpeed;  // 0x5c
    float flConsoleSpringFollowCameraMaxSpeed;  // 0x60
    float flConsoleSpringFollowCameraSpringFactor;  // 0x64
    float flConsoleSpringFollowCameraSpringExponent;  // 0x68
    float flConsoleCameraZoomSpeed;  // 0x6c
    float flConsoleOrbitSLerpScalar;  // 0x70
    float flConsoleOrbitTransitionExponentialScalar;  // 0x74
    float flConsoleOrbitTransitionDuration;  // 0x78
    Vector3D wvConsoleOrbitOffsetCenterForAngleLimiting;  // 0x7c
    float flConsoleOrbitMaxAngleFromOffsetCenter;  // 0x88
    Vector3D vConsoleCameraShakeScalar;  // 0x8c
};
static_assert(sizeof(ObserverDefinition) == 0x98, "ObserverDefinition");

class ObserverGroupDef : public GroupDefinition {  // size 0xa0, line 59
public:
    ObserverGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 64; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 67
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 69
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ObserverGroupDef) == 0xa0, "ObserverGroupDef");

extern const XTypeDescriptor * DT_OBSERVER;  // ObserverDefinition.cpp:61
extern ObserverGroupDef * gptGroupDefObserver;  // ObserverDefinition.cpp:67

typedef ScopedDefinitionLockR<ObserverDefinition> ScopedObserverDefinitionLockR;

// Line 97. As for FlagSetDefinition.h, the unlock is taken to be empty.
template <> inline const ObserverDefinition * DefinitionLockR<ObserverDefinition>(SNO sno) {
    return (const ObserverDefinition *)gptGroupDefObserver->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<ObserverDefinition>(const ObserverDefinition *) {}

// Functions defined in this file (bodies are not in the DWARF):
//   line    59  ObserverGroupDef::~ObserverGroupDef()  [0x710063bac0]
//   line    64  void * ObserverGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x710063baf0 (+1 more)]
//   line    64  void ObserverGroupDef::MemFree(void *& ptr) const  [0x710063bb40 (+1 more)]
//   line    67  BOOL ObserverGroupDef::CanReferenceOtherSNOs() const  [0x710063bbd0]
//   line    69  SNOPackType ObserverGroupDef::GetSNOPackDestination(const void *) const  [0x710063bbc0]
//   line    97  const ObserverDefinition * DefinitionLockR<ObserverDefinition>(SNO sno)  [inlined into 17 functions]
