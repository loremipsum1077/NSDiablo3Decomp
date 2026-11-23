// Original file: X/Common/WorldPlace.cpp

#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"

#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Defined in X/Common/SceneCommon.cpp:1240 and X/Common/Nav/NavigationMesh.cpp:771 and :2411; no
// adopted header declares them yet.
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive);
WorldDistance NavMeshGetHeightWorldSpace(const NavMeshHandle & tNavMeshHandle);
BOOL NavMeshHandleIsNone(const NavMeshHandle & tNavMeshHandle);

// The field names are "" in the binary.
static const TypeDescriptorField WorldPlace_Fields[3] = {  // line 18
    TypeDescriptorField("", DT_VECTOR3D, offsetof(WorldPlace, wp), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WorldPlace, idSWorld), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WorldPlace), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldPlace("WorldPlace", 0, WorldPlace_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 21
const XTypeDescriptor * DT_WORLD_PLACE = &TypeDescriptor_WorldPlace;

WorldDistance WorldPlaceDistance(const WorldPlace & tPlace1, const WorldPlace & tPlace2) {  // line 30
    WorldDistance wdDistance;

    wdDistance = Vector3D::Distance(tPlace1.wp, tPlace2.wp);

    return wdDistance;
}

WorldDistance WorldPlaceDistanceSquared(const WorldPlace & tPlace1, const WorldPlace & tPlace2) {  // line 42
    WorldDistance wdDistance;

    wdDistance = Vector3D::DistanceSquared(tPlace1.wp, tPlace2.wp);

    return wdDistance;
}

WorldDistance WorldPlaceXYPlaneDistance(const WorldPlace & tPlace1, const WorldPlace & tPlace2) {  // line 54
    WorldDistance wdDistance;

    wdDistance = Vector3D::Distance2D(tPlace1.wp, tPlace2.wp);

    return wdDistance;
}

void WorldPlaceGetVectorBetween(const WorldPlace & tPlace1, const WorldPlace & tPlace2, Vector3D & wvBetween) {  // line 90
    wvBetween = tPlace2.wp - tPlace1.wp;
}

void WorldPlaceOffset(const WorldPlace & placeStart, const Vector3D & wvBetween, WorldPlace & placeResult) {  // line 102
    placeResult.wp = placeStart.wp + wvBetween;
    placeResult.idSWorld = placeStart.idSWorld;
}

BOOL WorldPlaceEqual(const WorldPlace & tPlace1, const WorldPlace & tPlace2) {  // line 114
    if (tPlace1.idSWorld != tPlace2.idSWorld) {
        return FALSE;
    }

    if (!(tPlace1.wp == tPlace2.wp)) {
        return FALSE;
    }

    return TRUE;
}

BOOL WorldPlaceInSameWorld(const WorldPlace & tPlace1, const WorldPlace & tPlace2) {  // line 136
    return tPlace1.idSWorld == tPlace2.idSWorld;
}

BOOL WorldPlaceSnapToNavMesh(WorldPlace & tPlace, const BOOL fCanActivateScenes) {  // line 152
    NavMeshHandle tNavMeshHandle;

    ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, fCanActivateScenes, FALSE);
    if (NavMeshHandleIsNone(tNavMeshHandle)) {
        return FALSE;
    }

    tPlace.wp.z = NavMeshGetHeightWorldSpace(tNavMeshHandle);
    return TRUE;
}
