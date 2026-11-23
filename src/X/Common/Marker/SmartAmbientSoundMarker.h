// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartAmbientSoundMarker.h

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
struct Sphere;

class AmbientSoundMarker : public MarkerBase {  // size 0x18
public:
    AmbientSoundMarker() : MarkerBase(AMBIENTSOUND_SNO, MARKERTYPE_AMBIENTSOUND, "Ambient Sound") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5
    virtual BOOL IsForClient(const Marker & tMarker) { return TRUE; }  // vtable slot 6
    virtual BOOL IsConsideredInMarkerSetBounds(const Marker & tMarker) { return FALSE; }  // vtable slot 11
    virtual void GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds);  // vtable slot 4
    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
    virtual void Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected);  // vtable slot 15
};
static_assert(sizeof(AmbientSoundMarker) == 0x18, "AmbientSoundMarker");


// Functions defined in this file (bodies are not in the DWARF):
//   line    13  AmbientSoundMarker::~AmbientSoundMarker()  [0x71006ca8c0]
//   line    16  AmbientSoundMarker::AmbientSoundMarker()  [inlined into 1 functions]
//   line    28  BOOL AmbientSoundMarker::IsForClient(const Marker & tMarker)  [0x71006ca8a0]
//   line    34  BOOL AmbientSoundMarker::IsConsideredInMarkerSetBounds(const Marker & tMarker)  [0x71006ca8b0]
