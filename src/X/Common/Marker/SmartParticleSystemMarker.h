// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartParticleSystemMarker.h

#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct AABB;
struct InactiveMarkerOwner;
struct Marker;
struct PRTransform;

class ParticleSystemMarker : public MarkerBase {  // size 0x18
public:
    virtual ~ParticleSystemMarker() {}  // DWARF line 13; deleting destructor
    ParticleSystemMarker() : MarkerBase(PARTICLE_SNO, MARKERTYPE_PARTICLESYSTEM, "Particle System") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5
    virtual BOOL IsForClient(const Marker & tMarker) { return TRUE; }  // vtable slot 6
    virtual BOOL IsConsideredInMarkerSetBounds(const Marker & tMarker);  // vtable slot 11
    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
};
static_assert(sizeof(ParticleSystemMarker) == 0x18, "ParticleSystemMarker");


// Functions defined in this file (bodies are not in the DWARF):
//   line    13  ParticleSystemMarker::~ParticleSystemMarker()  [0x710064bc10]
//   line    16  ParticleSystemMarker::ParticleSystemMarker()  [inlined into 1 functions]
//   line    28  BOOL ParticleSystemMarker::IsForClient(const Marker & tMarker)  [0x710064bc00]
