// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartActorMarker.h

#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct AABB;
class CRefString;
struct InactiveMarkerOwner;
struct Marker;
struct PRTransform;
struct Quaternion;
struct RNG;
struct Sphere;
struct WorldPlace;

class ActorMarker : public MarkerBase {  // size 0x18
public:
    ActorMarker() : MarkerBase(ACTOR_SNO, MARKERTYPE_ACTOR, "Actor") {}
    virtual SNO GetActorForEdit(SNO snoSource) { return snoSource; }  // vtable slot 3; header line 21
    virtual void GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds);  // vtable slot 4
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5
    virtual CRefString GetMarkerTypeString(const Marker * ptMarker) const;  // vtable slot 2
    virtual BOOL IsForClient(const Marker & tMarker);  // vtable slot 6; SmartActorMarker.cpp:107
    virtual BOOL IsSpecialServerMarker(const Marker & tMarker);  // vtable slot 8
    virtual BOOL IsConsideredInMarkerSetBounds(const Marker & tMarker);  // vtable slot 11
    virtual BOOL IsPathMarker(const Marker & tMarker);  // vtable slot 10
    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13
    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
protected:
    BOOL ActorCanWalk(const SNO & snoActor);
    float CreateNewActorScale(const Marker & tMarker, RNG & tRNG);
};
static_assert(sizeof(ActorMarker) == 0x18, "ActorMarker");


// Functions defined in this file (bodies are not in the DWARF):
//   line    13  ActorMarker::~ActorMarker()  [0x71006ca2a0]
//   line    16  ActorMarker::ActorMarker()  [inlined into 1 functions]
//   line    21  SNO ActorMarker::GetActorForEdit(SNO snoSource)  [0x71006ca290]
