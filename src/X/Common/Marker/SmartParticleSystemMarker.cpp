// X/Common/Marker/SmartParticleSystemMarker.cpp
//
// Byte-matching reconstruction from the packet: build/dwarf/headers/X/Common/Marker/
// SmartParticleSystemMarker.cpp, the line-attributed listings (decl lines :30, :37, :46, :92)
// and build/ghidra/decomp/X/Common/Marker/SmartParticleSystemMarker.cpp. Function order follows
// the DWARF lines; the destructor (:13) and IsForClient (:28) are class-body definitions whose
// weak out-of-line copies this TU emits together with the vtable.

#include "X/Common/Marker/SmartParticleSystemMarker.h"

#include "X/Client/AttachmentParent.h"
#include "X/Client/CGame.h"
#include "X/Client/Particle.h"
#include "X/Client/Scene.h"
#include "X/Common/GlobalSNO.h"
#include "X/Common/Marker/Marker.h"
#include "X/Common/TagMaps/Tags.h"
#include "X/Sigma/DataStructures/DataArraysTemplated.h"
#include "X/Sigma/DataStructures/XPtr64.h"
#include "X/Sigma/Math/Rect.h"
#include "X/Sigma/Math/Quaternion.h"
#include "X/Sigma/Math/Transform.h"
#include "X/Sigma/Math/Vector.h"

// :30, instructions at :33. `snoSource` is unused; the argument is the enumerated constant
// GLOBALSNO_MARKER_ACTOR_PARTICLE = 967 (0x3c7) from X/Common/GlobalSNO.h:49.
SNO ParticleSystemMarker::GetActorForEdit(SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_PARTICLE);
}

// :37, body :41-:43. The inlined helpers are PRTransform::operator* (Transform.h:67, which
// inlines Quaternion::Rotate at Quaternion.inl:144 and Vector3D::operator+ at Vector.inl:535),
// Vector3D::operator= (Vector.inl:502) and Vector3D::Set (Vector.inl:637).
void ParticleSystemMarker::GetAABB(const Marker & tMarker, AABB & aabbBounds)
{
    PRTransform transform = gtransformMarkerToScene * tMarker.transform;
    aabbBounds.wp = transform.wp;
    aabbBounds.wvExt.Set(1.5f, 1.5f, 1.5f);
}

// :46, body :49.
BOOL ParticleSystemMarker::IsConsideredInMarkerSetBounds(const Marker & tMarker)
{
    return FALSE;
}

// :92.
DataID ParticleSystemMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner)
{
    // :99 - ParticleSystemAddParams (Particle.h:137) inlined. The SNO read is
    // tMarker.snoname.snoHandle (Ghidra member names) at Marker+0x2c. `idSWorld` is unused, as
    // the listing shows (w1 is reused for the tag lookup).
    ParticleSystemAddParams tParams(tMarker.snoname.snoHandle, transform);

    // :101
    DataID idObject = ParticleSystemAdd(tParams);

    // :102
    if (idObject != DATAID_INVALID)
    {
        // :104
        ParticleSystem * ptParticleSystem = ParticleSystemGet(idObject);

        // :106
        ptParticleSystem->idSScene = idSScene;

        // :107 - the listing copies 12 bytes (8 at tMarkerOwner+0 via a single ldr x/str x,
        // 4 at +8) into ps+0x1cc: the whole InactiveMarkerOwner (X/Common/InactiveMarker.h:14)
        // of the ParticleSystem (DWARF: ParticleSystem.tMarkerOwner at 0x1cc).
        ptParticleSystem->tMarkerOwner = tMarkerOwner;

        // :108 - tag 0x80207 (tag base 8, tag number 0x207). The argument conversion inlines
        // XPtr64<CTagMap>::operator CTagMap* (XPtr64.h:68, listing line :70).
        // The original named the tag through the tag-number macros; the packet does not record
        // the macro, so the value from the immediate is used.
        int32 nFadeGroup = TagMapGetInt(tMarker.hTagMap, 0x80207);

        // :109
        ptParticleSystem->nFadeGroupSubObjectIndex = SceneGetASubObjectIndexFromFadeGroup(idSScene, nFadeGroup);

        // :112
        if (ParticleSystemIsMergeable(ptParticleSystem))
        {
            // :115 - CGameGet() (CGame.h:760, body at :766) inlined.
            CGame & tCGame = CGameGet();

            // :116
            TDataArrayIterator<ParticleSystem> tIt;

            // :117 - TDataArray<ParticleSystem, int>::Iterate (DataArraysTemplated.h:706, which
            // inlines the (uint16 &, ParticleSystem **) overload at :663).
            for (ParticleSystem * ptBaseParticleSystem = tCGame.ptParticleSystems->Iterate(tIt); ptBaseParticleSystem != NULL; ptBaseParticleSystem = tCGame.ptParticleSystems->Iterate(tIt))
            {
                // :121
                if (ParticleSystemCanMergeMarker(ptBaseParticleSystem, ptParticleSystem))
                {
                    // :123
                    ParticleSystemMergeMarker(ptBaseParticleSystem, ptParticleSystem);

                    // :126
                    idObject = ptBaseParticleSystem->id;
                    break;
                }
            }
        }
    }

    // :133
    return idObject;
}
