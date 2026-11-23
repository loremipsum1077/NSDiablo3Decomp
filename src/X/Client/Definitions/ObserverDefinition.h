// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ObserverDefinition.h
// The client's own copy of the observer definition, a smaller layout than X/Common's.
// Do not include it together with X/Common/Definitions/ObserverDefinition.h.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"

struct ObserverDefinition {  // size 0x54
    RequiredDefinitionHeader tHeader;  // 0x00
    BOOL fOrthographic;  // 0x0c
    Angle aFOV;  // 0x10
    float flZFar;  // 0x14
    float flZNear;  // 0x18
    WorldDistance wdMaxVelForward;  // 0x1c
    Angle aYaw;  // 0x20
    Angle aPitch;  // 0x24
    float flDist;  // 0x28
    Vector3D wvCameraOffsetVector;  // 0x2c
    Vector3D wvLookAtOffsetVector;  // 0x38
    WorldDistance wdMicrophoneHeight;  // 0x44
    WorldDistance wdMicrophoneXYOffset;  // 0x48
    float flRolloffFactor;  // 0x4c
    float flDopplerFactor;  // 0x50
};
static_assert(sizeof(ObserverDefinition) == 0x54, "ObserverDefinition");

// The same group definition X/Common/Definitions/ObserverDefinition.h declares.
class ObserverGroupDef : public GroupDefinition {  // size 0xa0
public:
    ObserverGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(ObserverGroupDef) == 0xa0, "ObserverGroupDef");

extern ObserverGroupDef * gptGroupDefObserver;  // X/Common/Definitions/ObserverDefinition.cpp

typedef ScopedDefinitionLockR<ObserverDefinition> ScopedObserverDefinitionLockR;

template <> inline const ObserverDefinition * DefinitionLockR<ObserverDefinition>(SNO sno) {  // line 78
    return (const ObserverDefinition *)gptGroupDefObserver->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ObserverDefinition>(const ObserverDefinition * pdef) {  // line 79
    void * lpDef = (void *)pdef;
    gptGroupDefObserver->UnlockR(lpDef);
}
