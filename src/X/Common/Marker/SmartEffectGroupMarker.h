// X/Common/Marker/SmartEffectGroupMarker.h
//
// Adopted from build/dwarf/headers/X/Common/Marker/SmartEffectGroupMarker.h (reference-only
// skeleton, sha256 954309ab...). Bases and layout kept as recorded: base MarkerBase, size 0x18.
// The DWARF records two of the class' routines as defined *in this header*:
//   line 13  EffectGroupMarker::~EffectGroupMarker()   [0x71006befd0]  (implicit, empty: `b _ZdlPv`)
//   line 16  EffectGroupMarker::EffectGroupMarker()    [inlined into 1 function]
//   line 28  BOOL EffectGroupMarker::IsForClient(const Marker & tMarker)  [0x71006befc0]
// IsForClient is therefore written inline here (its 8 original bytes: `orr w0,wzr,#1; ret`),
// and the deleting destructor is left implicit over the virtual base.

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

class EffectGroupMarker : public MarkerBase {  // size 0x18
public:
    EffectGroupMarker() : MarkerBase(EFFECTGROUP_SNO, MARKERTYPE_EFFECTGROUP, "EffectGroup") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5
    virtual BOOL IsForClient(const Marker & tMarker) { return TRUE; }  // vtable slot 6; header line 28
    virtual BOOL IsConsideredInMarkerSetBounds(const Marker & tMarker);  // vtable slot 11
    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
    virtual void Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected);  // vtable slot 15
};
static_assert(sizeof(EffectGroupMarker) == 0x18, "EffectGroupMarker");
