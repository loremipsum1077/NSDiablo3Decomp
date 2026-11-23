// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartSceneLocationMarker.h
// The class is declared at line 13 of the original. It declares no destructor: the
// DWARF's ~SceneLocationMarker (vtable slot 0) is DW_AT_artificial, i.e. implicit, and
// its deleting copy (0x710064c010) falls back to the class decl line. IsForClient and
// IsSpecialServerMarker are defined in the class here (their emitted out-of-line copies
// are the weak ones the key TU SmartSceneLocationMarker.cpp keeps).

#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct InactiveMarkerOwner;
struct Marker;
struct Quaternion;
struct WorldPlace;

class SceneLocationMarker : public MarkerBase {  // size 0x18
public:
    // Declared at header line 16. Its body is not in this file's inventory; the DWARF
    // records it only inlined into a caller (helper_ledger: SmartMarker.cpp:9), so no
    // body is invented here.
    SceneLocationMarker() : MarkerBase(SNOGROUP_NONE, MARKERTYPE_SCENE_LOCATION, "Scene Location") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual BOOL IsForClient(const Marker & tMarker)
    {
        return FALSE;
    }  // vtable slot 6
    virtual BOOL IsSpecialServerMarker(const Marker & tMarker)
    {
        return TRUE;
    }  // vtable slot 8
    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13
};
static_assert(sizeof(SceneLocationMarker) == 0x18, "SceneLocationMarker");
