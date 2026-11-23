// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartEventMarker.h
// MARKERTYPE_EVENT's MarkerBase implementation. It adds no data member to
// MarkerBase, so sizeof(EventMarker) stays 0x18. The DWARF puts the class on
// line 13, the constructor on line 16, GetActorForEdit on line 21,
// IsForClient on line 24, IsSpecialServerMarker on line 30 and the two
// Create*ForGame hooks on lines 38 and 48; ~EventMarker is implicit.
#pragma once

#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"

class EventMarker : public MarkerBase {  // size 0x18
public:

    EventMarker() : MarkerBase(QUEST_SNO, MARKERTYPE_EVENT, "Event") {}

    // ~EventMarker is implicit (the DWARF marks it artificial); it is virtual
    // because ~MarkerBase is.

    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3


    virtual BOOL IsForClient(const Marker & tMarker) {  // vtable slot 6

        return FALSE;
    }


    virtual BOOL IsSpecialServerMarker(const Marker &
        tMarker) {  // vtable slot 8

        return TRUE;
    }



    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13









    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
};
static_assert(sizeof(EventMarker) == 0x18, "EventMarker");
