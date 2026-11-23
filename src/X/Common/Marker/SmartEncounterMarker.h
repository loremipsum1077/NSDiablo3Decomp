// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartEncounterMarker.h

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

class EncounterMarker : public MarkerBase {  // size 0x18
public:
    EncounterMarker() : MarkerBase(ENCOUNTER_SNO, MARKERTYPE_ENCOUNTER, "Encounter") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5
    virtual BOOL IsForClient(const Marker & tMarker) { return FALSE; }  // vtable slot 6
    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition, const Quaternion & qOrientation, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13
};
static_assert(sizeof(EncounterMarker) == 0x18, "EncounterMarker");


// Functions defined in this file (bodies are not in the DWARF):
//   line    13  EncounterMarker::~EncounterMarker()  [0x71006cb580]
//   line    16  EncounterMarker::EncounterMarker()  [inlined into 1 functions]
//   line    28  BOOL EncounterMarker::IsForClient(const Marker & tMarker)  [0x71006cb570]
