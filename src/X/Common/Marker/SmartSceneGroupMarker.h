// X/Common/Marker/SmartSceneGroupMarker.h
// Reconstructed from the DWARF header skeleton
// (build/dwarf/headers/X/Common/Marker/SmartSceneGroupMarker.h).  Declaration
// order, bases and layout are the DWARF's.  The destructor (line 13), the
// constructor (line 16) and the two predicates (lines 28 / 34) are inline
// here, as their DWARF definitions are attributed to this header.
#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct AABB;
struct InactiveMarkerOwner;
struct Marker;
struct Quaternion;
struct WorldPlace;

class SceneGroupMarker : public MarkerBase {  // size 0x18
public:
    // SmartSceneGroupMarker.h:13.  Emitted in SmartSceneGroupMarker.cpp as
    // _ZN16SceneGroupMarkerD0Ev [0x710064bfc0].
    virtual ~SceneGroupMarker() {}

    // SmartSceneGroupMarker.h:16; inlined into one caller.
    SceneGroupMarker() : MarkerBase(SCENEGROUP_SNO, MARKERTYPE_SCENEGROUP, "Scene Group") {}

    // vtable slot 3; body at SmartSceneGroupMarker.cpp:24.
    virtual SNO GetActorForEdit(SNO snoSource);

    // vtable slot 5; body at SmartSceneGroupMarker.cpp:31.
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);

    // vtable slot 6; body here.  _ZN16SceneGroupMarker11IsForClientERK6Marker
    // [0x710064bfa0].
    virtual BOOL IsForClient(const Marker & tMarker) { return FALSE; }

    // vtable slot 8; body here.
    // _ZN16SceneGroupMarker21IsSpecialServerMarkerERK6Marker [0x710064bfb0].
    virtual BOOL IsSpecialServerMarker(const Marker & tMarker) { return TRUE; }

    // vtable slot 13; body at SmartSceneGroupMarker.cpp:59.
    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);
};
static_assert(sizeof(SceneGroupMarker) == 0x18, "SceneGroupMarker");
