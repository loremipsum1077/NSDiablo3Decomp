// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartScriptMarker.h
// ScriptMarker is the MARKERTYPE_SCRIPT MarkerBase implementation. It adds no data
// member to MarkerBase, so sizeof(ScriptMarker) stays 0x18. The 2.6.2 DWARF places
// the class on line 13 and gives IsForClient an in-class body (decl_line 24); the
// destructor is implicit. GetActorForEdit and the two Create*ForGame overrides have
// their out-of-line bodies in SmartScriptMarker.cpp.

#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct InactiveMarkerOwner;
struct Marker;
struct PRTransform;
struct Quaternion;
struct WorldPlace;

class ScriptMarker : public MarkerBase {  // size 0x18
public:
    // ~ScriptMarker is implicit (the DWARF marks it artificial); it is virtual
    // because ~MarkerBase is.

    ScriptMarker() : MarkerBase(SCRIPT_SNO, MARKERTYPE_SCRIPT, "Script") {}

    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3

    // Inline body (DWARF: SmartScriptMarker.h:24, body line 26): the script marker
    // is never a client-only marker.
    virtual BOOL IsForClient(const Marker & tMarker) {  // vtable slot 6
        return FALSE;
    }

    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13

    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
};
static_assert(sizeof(ScriptMarker) == 0x18, "ScriptMarker");
