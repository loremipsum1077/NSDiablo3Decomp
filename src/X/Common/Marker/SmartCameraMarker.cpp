// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartCameraMarker.cpp
// MARKERTYPE_CAMERA's MarkerBase implementation: the camera's edit actor and the
// selected sphere gizmo.  Members sit on their original DWARF lines.

#include "SmartCameraMarker.h"

#include "../GlobalSNO.h"
#include "../TagMaps/MarkerTags.h"
#include "../TagMaps/Tags.h"
#include "Marker.h"
#include "../../Client/Gfx/GfxPrimitives.h"
#include "../../Client/Gfx/GfxCommon.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/RGBAColor.h"

// GfxSphereDrawDeferred is defined in X/Client/Gfx/GfxPrimitives.cpp:1085 and
// declared in its DWARF owner X/Client/Gfx/GfxPrimitives.h; the declaration is
// not repeated locally here, the owner header is included above.  The call
// below is attributed to that owner file.

// The DWARF records this definition on line 27, its parameter on line 28 and its
// return statement on line 30.


SNO CameraMarker::GetActorForEdit(
    SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_CAMERA);
}

// MarkerBase::CreateForClientGame (SmartMarkerBase.cpp, 0x710064b400) raises a fatal
// internal error; the camera marker is for the client (IsForClient is TRUE) but creates
// nothing there, so this override returns DataID(-1).  The DWARF records the definition
// on line 47, its parameters on lines 48..52 and its return statement on line 59.










DataID CameraMarker::CreateForClientGame(
    const SWorldID idSWorld,
    const SSceneID idSScene,
    const PRTransform & transform,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{





    return -1;
}

void CameraMarker::Draw(
    const Marker & tMarker,
    const PRTransform & transform,
    const BOOL bSelected)
{
    if (!bSelected)
        return;



    float flRadius = TagMapGetFloat(tMarker.hTagMap, TAG_MARKER_SCALE);

    Sphere wsAreaOfInfluence;
    wsAreaOfInfluence.Set(transform.wp, flRadius);
    GfxSphereDrawDeferred(wsAreaOfInfluence, RGBAColor(0x3f, 0xff, 0x7f, 0xff), GLOBALSNO_SHADER_3D_PRIMS, RENDERCONTEXT_DEFAULT);
}
