// X/Client/RActors/RActorACDEffects.cpp
// Function bodies sit on the DWARF line numbers from the 2.6.2 build.

#include "RActorACDEffects.h"
#include "RActors.h"
#include "../CGizmo.h"
#include "../Rope.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Definitions/AttachmentDefinition.h"
#include "../../Common/GizmoSwig.h"





















































RopeID RActorACDRopeEffect(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom,
                           const ACDID idACDTo, const RopePosition ePositionTo, const BOOL bIgnoreOwnerAlpha) {








    RActorID idRActorFrom = (RActorID)ACDGetOwner(idACDFrom);
    RActorID idRActorTo = (RActorID)ACDGetOwner(idACDTo);


    if (idRActorFrom == RACTORID_INVALID || idRActorTo == RACTORID_INVALID) {
        return ROPEID_INVALID;
    }



    RopeAttachPoint tRopeAttachPoints[2];


    HardpointHash szHardpointFromName = RopePositionToHardpointName(ePositionFrom);
    if (!(szHardpointFromName == 0U)) {
        // attach to the named hardpoint
        tRopeAttachPoints[0].nRopeAttachType = ROPE_ATTACH_PARENT;
        tRopeAttachPoints[0].tParentData.SetHardpoint(PARENT_RACTOR, idRActorFrom, szHardpointFromName);
    } else {
        // no hardpoint, attach to the center
        tRopeAttachPoints[0].nRopeAttachType = ROPE_ATTACH_PARENT_CENTER;
        tRopeAttachPoints[0].tParentData.SetHardpointIndex(PARENT_RACTOR, idRActorFrom, 0);
    }

    HardpointHash szHardpointToName = RopePositionToHardpointName(ePositionTo);
    if (!(szHardpointToName == 0U)) {
        // attach to the named hardpoint
        tRopeAttachPoints[1].nRopeAttachType = ROPE_ATTACH_PARENT;
        tRopeAttachPoints[1].tParentData.SetHardpoint(PARENT_RACTOR, idRActorTo, szHardpointToName);
    } else {
        // no hardpoint, attach to the center
        tRopeAttachPoints[1].nRopeAttachType = ROPE_ATTACH_PARENT_CENTER;
        tRopeAttachPoints[1].tParentData.SetHardpointIndex(PARENT_RACTOR, idRActorTo, 0);
    }

    return RopeAdd(snoRope, tRopeAttachPoints, bIgnoreOwnerAlpha, RENDERCONTEXT_DEFAULT);
}





RopeID RActorACDRopeEffect(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom,
                           const Vector3D & wpDest, const BOOL bIgnoreOwnerAlpha) {







    RActorID idRActorFrom = (RActorID)ACDGetOwner(idACDFrom);


    if (idRActorFrom == RACTORID_INVALID) {
        return ROPEID_INVALID;
    }



    RopeAttachPoint tRopeAttachPoints[2];



    HardpointHash szHardpointFromName = RopePositionToHardpointName(ePositionFrom);
    if (!(szHardpointFromName == 0U)) {
        // attach to the named hardpoint
        tRopeAttachPoints[0].nRopeAttachType = ROPE_ATTACH_PARENT;
        tRopeAttachPoints[0].tParentData.SetHardpoint(PARENT_RACTOR, idRActorFrom, szHardpointFromName);
    } else {
        // no hardpoint, attach to the center
        tRopeAttachPoints[0].nRopeAttachType = ROPE_ATTACH_PARENT_CENTER;
        tRopeAttachPoints[0].tParentData.SetHardpointIndex(PARENT_RACTOR, idRActorFrom, 0);
    }

    // the far end is a fixed world point
    tRopeAttachPoints[1].nRopeAttachType = ROPE_ATTACH_WORLDPOINT;
    tRopeAttachPoints[1].wpAttach = wpDest;

    RopeID idRope;
    idRope = RopeAdd(snoRope, tRopeAttachPoints, bIgnoreOwnerAlpha, RENDERCONTEXT_DEFAULT);

    return idRope;



}



void RActorACDEffectsAdd(const ACDID idACD) {

    const ActorCommonData & tACD = ACDGet(idACD);

    RActorID idRActor = (RActorID)tACD.idOwner;

    GizmoType nGizmoType = tACD.GetGizmoType();

    // waypoints need their client-side state set up
    if (nGizmoType == ACTORGIZMO_WAYPOINT) {

        GizmoWaypointRActorInitialize(idRActor);
    }
}



void RActorACDEffectsRemove(const ACDID idACD) {
    // the 2.6.2 build has an empty body here
    (void)idACD;

}
