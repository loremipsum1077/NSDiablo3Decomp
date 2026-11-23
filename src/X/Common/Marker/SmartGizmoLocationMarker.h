// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartGizmoLocationMarker.h
// GizmoLocationMarker (MARKERTYPE_GIZMOLOC_S) adds m_nIndex at 0x18 to MarkerBase. The
// DWARF records its inline members at lines 20 (destroying destructor), 23 (constructor),
// 36 (GetMarkerTypeString), 43 (IsForClient), 48 (IsSpecialServerMarker) and 53
// (IsGizmoLocationMarker); their weak out-of-line copies are emitted from the class's key
// TU, SmartGizmoLocationMarker.cpp, which also holds GetActorForEdit (the key function).
#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/RefString.h"
#include "../GizmoLocs.h"

class GizmoLocationMarker : public MarkerBase {  // size 0x20
public:
    virtual ~GizmoLocationMarker() {}  // vtable slot 0; out-of-line copy 0x71006cb6e0


    GizmoLocationMarker(int32 nIndex) : MarkerBase(SNOGROUP_NONE, (MarkerType)(MARKERTYPE_GIZMOLOC + nIndex), "Gizmo Location"), m_nIndex(nIndex) {}  // inlined into one function

    // GetActorForEdit (vtable slot 3) is this class's key function: its only out-of-line
    // definition lives in SmartGizmoLocationMarker.cpp, so the vtable group and the weak
    // copies of every inline member above/below are emitted from that TU.
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3

    int32 GetGizmoIndex() const { return m_nIndex; }  // inlined at DRLGEncounters.cpp:1734 (load folded into the ldrsw at 1735)





    virtual CRefString GetMarkerTypeString(const Marker *) const  // vtable slot 2

    {

        return CRefString().Format("Gizmo %s", GizmoLocSuffixFromIndex(m_nIndex).str());
    }

    virtual BOOL IsForClient(const Marker &)  // vtable slot 6
    {
        return FALSE;
    }

    virtual BOOL IsSpecialServerMarker(const Marker &)  // vtable slot 8
    {
        return TRUE;
    }

    virtual BOOL IsGizmoLocationMarker()  // vtable slot 9
    {
        return TRUE;
    }

    virtual DataID CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition,
                                       const Quaternion & qOrientation, const Marker & tMarker,
                                       const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 13

protected:
    int32 m_nIndex;  // 0x18
};
static_assert(sizeof(GizmoLocationMarker) == 0x20, "GizmoLocationMarker");
