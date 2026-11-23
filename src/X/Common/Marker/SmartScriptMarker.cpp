// X/Common/Marker/SmartScriptMarker.cpp
// ScriptMarker is the MARKERTYPE_SCRIPT MarkerBase implementation. It adds no data
// member to MarkerBase, so its size stays 0x18; this file holds the out-of-line
// bodies GetActorForEdit (original line 21), CreateForServerGame (original line 29)
// and CreateForClientGame (original line 57). IsForClient has its in-class body in
// SmartScriptMarker.h (decl_line 24); the destructor is implicit.
#include "SmartScriptMarker.h"

#include "../GlobalSNO.h"
#include "../InactiveMarker.h"
#include "Marker.h"
#include "../../Server/LuaScript/LuaScript.h"
#include "../../Server/SScene.h"

// The script marker's edit actor is always the globally registered script actor
// (GLOBALSNO_ACTOR_SCRIPT = 1146); snoSource is unused. The DWARF records the
// definition on line 21 and the return statement on line 24.
SNO ScriptMarker::GetActorForEdit(
    SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_ACTOR_SCRIPT);
}

// SmartScriptMarker.cpp:29, body :35 (vtable slot 13).
DataID ScriptMarker::CreateForServerGame(const SSceneID idSScene, const WorldPlace & placePosition,
                                         const Quaternion & qOrientation, const Marker & tMarker,
                                         const InactiveMarkerOwner & tMarkerOwner)
{
    BOOL bResult = MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker,
                                        SCRIPT_SNO, tMarker.snoname.snoHandle);
    if (bResult != 0)
    {
        LuaScriptAttach(SCRIPT_ATTACH_SCENE, idSScene, tMarker.snoname.snoHandle,
                        tMarkerOwner.nAdventureParentIndex);
        SScenePutScriptInScene(idSScene, tMarker.snoname.snoHandle,
                               tMarkerOwner.nAdventureParentIndex);
    }

    return -1;
}

// SmartScriptMarker.cpp:57, body :64 (vtable slot 14).
DataID ScriptMarker::CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene,
                                         const PRTransform & transform, const Marker & tMarker,
                                         const InactiveMarkerOwner & tMarkerOwner)
{
    return -1;
}
