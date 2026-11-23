// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartSceneLocationMarker.cpp

#include "SmartSceneLocationMarker.h"
#include "../GlobalSNO.h"

// SmartSceneLocationMarker.cpp:17; 0x710064bfd0 (vtable slot 3).
SNO SceneLocationMarker::GetActorForEdit(SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_SCENE);
}

// SmartSceneLocationMarker.cpp:25; 0x710064bfe0 (vtable slot 13).
DataID SceneLocationMarker::CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition,
                                                const Quaternion & qOrientation, const Marker & tMarker,
                                                const InactiveMarkerOwner & tMarkerOwner)
{
    return -1;
}
