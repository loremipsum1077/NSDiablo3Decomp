// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartAdventureMarker.h
// AdventureMarker adds no data member to MarkerBase, so it stays 0x18; the destructor
// is implicit (the DWARF marks it artificial, with no decl_line).
#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

class AdventureMarker : public MarkerBase {  // size 0x18
public:

    AdventureMarker() : MarkerBase(ADVENTURE_SNO, MARKERTYPE_ADVENTURE, "Adventure") {}

    // GetActorForEdit (vtable slot 3) resolves the adventure's symbol actor; IsForClient
    // (slot 6) is inline below; the destructor is implicit.

    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3


    virtual BOOL IsForClient(const Marker & tMarker) {  // vtable slot 6

        return FALSE;
    }



    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition,
                                       const Quaternion & qOrientation, const Marker & tMarker,
                                       const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13

    // CreateForClientGame is on DWARF line 50.














    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene,
                                       const PRTransform & transform, const Marker & tMarker,
                                       const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
};
static_assert(sizeof(AdventureMarker) == 0x18, "AdventureMarker");
