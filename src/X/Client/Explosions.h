// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Explosions.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions/ExplosionDefinition.h"
#include "Gfx/GfxCommon.h"
#include "../Common/CommonTypes.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/SigmaTypes.h"

struct PRTransform;
class RWindow;

enum {
    EXPLOSIONID_INVALID = -1,
};

typedef int32 ExplosionID;

struct ExplosionImpulse {  // size 0x30
    Vector3D ComputeImpulse(const Vector3D & wp) const;
    void DrawDebugView();
    BOOL IsMelee() const;
    RenderContext m_eContext;  // 0x00
    ExplosionType m_eType;  // 0x04
    uint32 m_dwFlags;  // 0x08
    Vector3D m_wpCenter;  // 0x0c
    Vector3D m_wvDirection;  // 0x18
    float m_flMagnitude;  // 0x24
    float m_flMinRadius;  // 0x28
    float m_flDeltaRadius;  // 0x2c
};
static_assert(sizeof(ExplosionImpulse) == 0x30, "ExplosionImpulse");

// Line 34. The body is not in the DWARF; written from the copy inlined into
// X/Client/Behavior/Critter.cpp (BehaviorCritterExplosion tests bit 0 of m_dwFlags).
inline BOOL ExplosionImpulse::IsMelee() const { return m_dwFlags & 1; }

struct ExplosionEntry {  // size 0x148
    enum {
        ACTOR_BUFFER_SIZE = 64,
    };
    void Initialize(SNO snoExplosion, const ExplosionImpulse & tExplosion, float flScale);
    void Update();
    SNO m_snoExplosion;  // 0x00
    RActorID m_idActorBuffer[64];  // 0x04
    int32 m_nActorCount;  // 0x104
    ExplosionImpulse m_tImpulse;  // 0x108
    int32 m_nDuration;  // 0x138
    ExplosionEntry * m_pNext;  // 0x140
};
static_assert(sizeof(ExplosionEntry) == 0x148, "ExplosionEntry");

// Explosion and ExplosionManager need ParentData (AttachmentParent.h), which is not adopted yet.


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  ExplosionImpulse & ExplosionImpulse::operator=(const ExplosionImpulse &)  [inlined into 2 functions]
//   line    34  BOOL ExplosionImpulse::IsMelee() const  [inlined into 2 functions]
//   line    78  Explosion::Explosion()  [inlined into 1 functions]
