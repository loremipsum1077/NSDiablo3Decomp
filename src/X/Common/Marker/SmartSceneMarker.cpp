// X/Common/Marker/SmartSceneMarker.cpp
// SceneMarker is the MARKERTYPE_SCENE MarkerBase implementation. It adds no data
// members to MarkerBase, so its size stays 0x18; the destructor is implicit
// and the constructor is only declared in SmartSceneMarker.h; its inline body
// is not written here, so GetActorForEdit is the only out-of-line body.
#include "SmartSceneMarker.h"

#include "../GlobalSNO.h"

// The scene marker's edit actor is always the globally registered scene actor
// (GLOBALSNO_MARKER_ACTOR_SCENE = 969); it does not depend on the
// marker's own SNO, so snoSource is unused. The DWARF records this definition on
// line 17, its parameter on line 18 and its return statement on line 20; the
// signature is wrapped the way the other markers wrap theirs.


SNO SceneMarker::GetActorForEdit(
    SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_SCENE);
}
