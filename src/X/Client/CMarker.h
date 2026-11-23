// X/Client/CMarker.h
//
// This header has no DWARF skeleton. The definition of the routine declared below
// is at X/Client/CMarker.cpp:244 (build/dwarf/headers/X/Client/CMarker.cpp:10);
// the signature here matches that definition and ../Diablo3Decomp/include/game/
// X/Client/CMarker.h:10. AttachmentID is the typedef at
// src/X/Common/CommonTypes.h:13; MarkerType and SNO are included for the
// declaration.

#pragma once

#include "../Common/CommonTypes.h"
#include "../Common/Marker/MarkerType.h"
#include "../Common/SNO/SNOFiles.h"

// Defined in X/Client/CMarker.cpp:244; called from
// EffectGroupMarker::CreateForClientGame (SmartEffectGroupMarker.cpp:131).
// The call ignores the return value, so its type does not change the bytes.
AttachmentID MarkerAddAttachmentsToPlaceholder(RActorID idRActorPlaceholder, MarkerType eMarkerType, SNO snoMarkerSource);  // CMarker.cpp:244

typedef int32 DataID;  // DataArraysTemplated.h:38 (identical redeclaration)
struct InactiveMarkerOwner;
struct Marker;
struct PRTransform;

void MarkersDeleteForWorld(SWorldID idSWorld);  // CMarker.cpp:65
DataID MarkerCreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // CMarker.cpp:298
