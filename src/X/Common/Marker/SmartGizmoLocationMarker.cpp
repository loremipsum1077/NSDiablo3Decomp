// X/Common/Marker/SmartGizmoLocationMarker.cpp
// Diablo III 2.6.2 (Switch, clang 5.0.1) -- byte-matching decompilation of the
// original debug build.
//
// DWARF provenance, X/Common/Marker/SmartGizmoLocationMarker.cpp:
//   line 21  SNO GizmoLocationMarker::GetActorForEdit(SNO snoSource)                    [0x71006cb610]
//   line 29  DataID GizmoLocationMarker::CreateForServerGame(const SSceneID, ...)       [0x71006cb620]
//
// The remaining inventory members are owned by the DWARF header
// X/Common/Marker/SmartGizmoLocationMarker.h and are emitted from there:
//   line 20  GizmoLocationMarker::~GizmoLocationMarker()                                [0x71006cb6e0]
//   line 23  GizmoLocationMarker::GizmoLocationMarker(int32 nIndex)                     [inlined]
//   line 36  CRefString GizmoLocationMarker::GetMarkerTypeString(const Marker *) const  [0x71006cb630]
//   line 43  BOOL GizmoLocationMarker::IsForClient(const Marker &)                      [0x71006cb6b0]
//   line 48  BOOL GizmoLocationMarker::IsSpecialServerMarker(const Marker &)            [0x71006cb6c0]
//   line 53  BOOL GizmoLocationMarker::IsGizmoLocationMarker()                          [0x71006cb6d0]

#include "SmartGizmoLocationMarker.h"

#include "../GlobalSNO.h"

// 0x71006cb610  size 8  _ZN19GizmoLocationMarker15GetActorForEditEi
//   mov  w0, #0x3ca        ; GLOBALSNO_MARKER_ACTOR_GIZMOLOC (X/Common/GlobalSNO.h:49)
//   b    _Z12GlobalSNOGeti ; tail call, X/Common/GlobalSNO.cpp
// Original source line 21.
SNO GizmoLocationMarker::GetActorForEdit(SNO snoSource) {
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_GIZMOLOC);
}

// 0x71006cb620  size 8  _ZN19GizmoLocationMarker19CreateForServerGameEiRK10WorldPlaceRK10QuaternionRK6MarkerRK19InactiveMarkerOwner
//   mov  w0, #-0x1  ; SWORLDID_INVALID / SNO_NONE / GLOBALSNO_INVALID / MARKERTYPE_NONE
//   ret
// Original source line 29.
DataID GizmoLocationMarker::CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition,
                                                const Quaternion & qOrientation, const Marker & tMarker,
                                                const InactiveMarkerOwner & tMarkerOwner) {
    return -1;
}
