// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartMinimapMarker.cpp
// MARKERTYPE_MINIMAP's MarkerBase implementation.  It adds no member to MarkerBase, so
// sizeof(MinimapMarker) stays 0x18; GetActorForEdit hands out the minimap actor SNO and the
// two Create*ForGame hooks are placeholders that report DataID(-1) for the minimap marker.
//
// The functions sit on their original lines: GetActorForEdit 17, CreateForServerGame 24,
// CreateForClientGame 37 and GetMarkerTypeString 49.

#include "SmartMinimapMarker.h"

#include "../GlobalSNO.h"
#include "../../Sigma/RefString.h"



SNO MinimapMarker::GetActorForEdit(SNO)
{

    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_MINIMAP);
}


DataID MinimapMarker::CreateForServerGame(const SSceneID,
                                          const WorldPlace &,
                                          const Quaternion &,
                                          const Marker &,
                                          const InactiveMarkerOwner &)
{

    return -1;
}




DataID MinimapMarker::CreateForClientGame(const SWorldID,
                                          const SSceneID,
                                          const PRTransform &,
                                          const Marker &,
                                          const InactiveMarkerOwner &)
{

    return -1;
}



CRefString MinimapMarker::GetMarkerTypeString(const Marker *) const
{

    const LPCSTR sczTypeName = "MinimapMarker_Invalid_Name";
    return CRefString(sczTypeName);
}
