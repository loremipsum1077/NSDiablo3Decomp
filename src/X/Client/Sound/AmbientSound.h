// Original file: X/Client/Sound/AmbientSound.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../AttachmentParent.h"
#include "../CGame.h"
#include "../Cloth.h"
#include "../Gfx/GfxCommon.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/InactiveMarker.h"
#include "../../Sigma/Math/Transform.h"

struct AmbientSoundDefinition;
struct AudioVolume;
class Frustum;
struct Sphere;
struct Vector3D;

typedef int32 SoundID;

enum {
    AMBIENTSOUNDID_INVALID = -1,
};

struct AmbientSound {  // size 0x94
    static AmbientSoundID Add(const SNO snoAmbient, const PRTransform & transform, const RenderContext eContext, const float flCapsuleLength, const BOOL fGlobalAmbient);
    static float GetRadius(SNO snoAmbient);
    void Update(const AudioVolume * ptVolume);
    void SwitchGlobalAmbient(const SNO snoAmbient);
    void SetParentCloth(const ClothID idCloth) { m_idCloth = idCloth; }  // line 57; body (line 60) from the copy inlined into Cloth::Create
    void SetParent(const ParentData & tParentData);
    void UpdatePropertiesFromParent(const float flDeltaTime);
    void UpdateBackgroundSoundPosition(const float flDeltaTime);
    void SetPosition(const PRTransform & transform, const float flDeltaTime);
    void DrawOverlays(const Vector3D &) const;
    void DeactivateInScene(SSceneID idSScene);
    void CheckSeenRecently();
    void SetLastSeenTime(const Frustum & tFrustum, const float flTime);
    void Delete();
    AmbientSoundID id;  // 0x00
    SSceneID m_idSScene;  // 0x04
    SWorldID m_idSWorld;  // 0x08
    InactiveMarkerOwner m_tMarkerOwner;  // 0x0c
    BOOL m_fLinkedToAudioVolume;  // 0x18
    uint32 m_tMarkerName;  // 0x1c
    ParentData m_tParentData;  // 0x20
    ClothID m_idCloth;  // 0x4c
    SNO m_snoQuestRange;  // 0x50
    RenderContext m_eContext;  // 0x54
private:
    BOOL sCanPlay(const AudioVolume * ptVolume);
    BOOL sBackgroundSoundCanPlay() const;
    void sDeleteBackgroundSound();
    Vector3D sGetBackgroundSoundPosition() const;
    BOOL sGetVelocityFromCloth(float & flVel) const;
    BOOL sGetVelocityFromConstraint(uint32 dwConstraintHash, float & flVel) const;
    BOOL sGetJointVelocitySoundParams(const AmbientSoundDefinition * pdefAmbient, float & flVolume, float & flPitch) const;
    Sphere sGetBoundingSphere() const;
    void sDeactivate();
    SNO m_snoAmbient;  // 0x58
    PRTransform m_transform;  // 0x5c
    float m_flCapsuleLength;  // 0x78
    SoundID m_idBackground;  // 0x7c
    float m_flTimeUntilRandom[2];  // 0x80
    float m_flLastSeen;  // 0x88
    BOOL m_fGlobalAmbient;  // 0x8c
    float m_flLastPlayedTime;  // 0x90
};
static_assert(sizeof(AmbientSound) == 0x94, "AmbientSound");

inline AmbientSound * AmbientSoundGet(AmbientSoundID id) {  // line 176


    return CGameGet().ptAmbientSounds->ElementGet(id);
}

// Line 182; body line 185 (copy inlined into CWorldUpdate).
inline AmbientSound * AmbientSoundTryToGet(AmbientSoundID id) {
    return CGameGet().ptAmbientSounds->ElementTryToGet(id);
}
