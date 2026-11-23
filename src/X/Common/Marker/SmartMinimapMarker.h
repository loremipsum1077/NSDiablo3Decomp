// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartMinimapMarker.h
#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

class MinimapMarker : public MarkerBase {  // size 0x18
public:
    virtual ~MinimapMarker() {}  // vtable slot 17/18; out-of-line copy 0x710064b910


    MinimapMarker() : MarkerBase(SNOGROUP_NONE, MARKERTYPE_MINIMAP, "Minimap") {}  // inlined into one function


    virtual SNO GetActorForEdit(SNO);  // vtable slot 3


    // IsForClient and IsSpecialServerMarker are inline here (DWARF lines 24 and 30);
    // their out-of-line copies are 0x710064b8f0 and 0x710064b900.
    virtual BOOL IsForClient(const Marker & tMarker) {  // vtable slot 6

        return FALSE;
    }


    virtual BOOL IsSpecialServerMarker(const Marker & tMarker) {  // vtable slot 8

        return TRUE;
    }


    virtual CRefString GetMarkerTypeString(const Marker *) const;  // vtable slot 2


    virtual DataID CreateForServerGame(const SSceneID, const WorldPlace &, const Quaternion &, const Marker &, const InactiveMarkerOwner &);  // vtable slot 13


    virtual DataID CreateForClientGame(const SWorldID, const SSceneID, const PRTransform &, const Marker &, const InactiveMarkerOwner &);  // vtable slot 14
};

static_assert(sizeof(MinimapMarker) == 0x18, "MinimapMarker");
