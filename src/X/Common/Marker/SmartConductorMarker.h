// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartConductorMarker.h

#pragma once

#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/SigmaTypes.h"

struct Marker;

class ConductorMarker : public MarkerBase {  // size 0x18
public:
    ConductorMarker() : MarkerBase(CONDUCTOR_SNO, MARKERTYPE_CONDUCTOR, "Conductor") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual BOOL IsForClient(const Marker & tMarker) { return FALSE; }  // vtable slot 6
};
static_assert(sizeof(ConductorMarker) == 0x18, "ConductorMarker");
