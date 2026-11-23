// Adopted subset of original X/Client/Lights.h.
#pragma once

#include "CGame.h"
#include "Gfx/GfxCommon.h"
#include "AttachmentParent.h"
#include "TexAnimation.h"
#include "../Common/CommonTypes.h"
#include "../Common/InactiveMarker.h"

class XSphereNode;

enum LightingFlags {  // Lights.h
    LIGHT_IS_EXCLUDED_BIT = 0,
    LIGHT_IS_MANUALLY_UPDATED_BIT = 1,
    LIGHT_IS_OFF_BIT = 2,
};

typedef int32 LightingTypes;

typedef int32 LightID;

// X/Client/Lights.h:26 and :171. Only the members CameraManager.cpp reads are declared.
struct Light {  // size 0x170
    LightID id;  // 0x00
    SNO sno;  // 0x04
    uint32 dwFlags;  // 0x08
    float flTimeElapsed;  // 0x0c
    LightState tState;  // 0x10
    float fInnerRadiusInterpolationFraction;  // 0x98
    float fRadiusInterpolationFraction;  // 0x9c
    float fAttenInterpolationFraction[3];  // 0xa0
    float fHotspotInterpolationFraction;  // 0xac
    float fFalloffInterpolationFraction;  // 0xb0
    float fRGBAValueInterpolationFraction;  // 0xb4
    float flRadiusScalar;  // 0xb8
    RenderContext eContext;  // 0xbc
    POS posDynamicList;  // 0xc0
    uint32 dwDynamicIgnoreFlags;  // 0xc8
    int32 nPriorityBoost;  // 0xcc
    int32 nPriority;  // 0xd0
    TexAnimState tTexAnimState;  // 0xd8
    uint32 dwLastSeen;  // 0x120
    SSceneID idSScene;  // 0x124
    InactiveMarkerOwner tMarkerOwner;  // 0x128
    ParentData tParentData;  // 0x134
    XSphereNode * ptSphereNode;  // 0x160
    POS posDirLightList;  // 0x168
};
static_assert(sizeof(Light) == 0x170, "Light");

inline Light * LightGet(LightID id) {  // line 165
    return CGameGet().ptLights->ElementGet(id);
}

inline Light * LightTryToGet(LightID id) {  // line 171
    return CGameGet().ptLights->ElementTryToGet(id);
}


// X/Client/Lights.cpp
void LightsSetLastSeenTime(Frustum & tFrustum);  // Lights.cpp:254
void LightsCheckSeenRecently();  // Lights.cpp:323
void LightsDeactivateInScene(SSceneID idSScene);  // Lights.cpp:357
void LightsDeleteAllForRenderContext(const RenderContext eContext);  // Lights.cpp:392
void LightDelete(LightID idLight);  // Lights.cpp:1177
void LightSetTransform(const LightID id, const PRTransform & transform);  // Lights.cpp:1197
void LightSetScale(const LightID idLight, float flScale);  // Lights.cpp:1233
void LightSetParent(const LightID idLight, const ParentData & tParentData);  // Lights.cpp:1264
