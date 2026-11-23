// Original file: X/Client/CSceneCommon.cpp

#include "../Common/ActorCommonData.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/InactiveMarker.h"
#include "../Common/Player.h"
#include "../Common/SceneCommon.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/Math/Frustum.h"
#include "Particle.h"
#include "RActors/RActors.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
// X/Client/CPlayer.cpp:1612 and :1686
Player * LocalPlayerGet();
ACDID LocalPlayerACDGet();
// X/Client/Sound/AmbientSound.cpp:759 and :801
namespace AmbientSounds {
void SetLastSeenTime(Frustum & tFrustum);
void CheckSeenRecently();
}  // namespace AmbientSounds
// X/Client/Lights.cpp:254 and :323
void LightsSetLastSeenTime(Frustum & tFrustum);
void LightsCheckSeenRecently();
// X/Client/Scene.cpp:4838
void CScenesUpdateEnvironmentRActors();
static const WorldDistance DISTANCE_CHANGE_FOR_ACTIVATION_CHECK_SQUARED = 225.0f;  // line 35

static BOOL sShouldTryToActivateScenes(Frustum & tFrustum, Frustum & tAVFrustum, SWorldID & idSWorld) {  // line 37
    GameMode eGameMode = GameGetActiveGameCommonData().GameModeGet();  // line 43
    if (eGameMode == GAMEMODE_PLAYING) {
        Player * ptPlayer = LocalPlayerGet();  // line 56
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);  // line 57
        if (ptACDPlayer != NULL) {
            WorldPlace tPlace = ptACDPlayer->GetPlace();  // line 60
            BOOL bInAVCameraMode = (ptPlayer->uServerFlags & (1 << PSF_IN_AV_CAMERA_MODE)) != 0;  // line 62
            if (!bInAVCameraMode && WorldPlaceInSameWorld(tPlace, ptPlayer->placeCheckedForActivatingScenes)) {
                WorldDistance wdSeparationSquared = WorldPlaceDistanceSquared(tPlace, ptPlayer->placeCheckedForActivatingScenes);  // line 68
                if (wdSeparationSquared < DISTANCE_CHANGE_FOR_ACTIVATION_CHECK_SQUARED) {
                    return FALSE;
                }
            }

            ptPlayer->placeCheckedForActivatingScenes = tPlace;  // line 78

            Vector3D vCenterPos;  // line 80
            SceneCreatePlayerFrustum(ptPlayer, tFrustum, vCenterPos);
            if (ptPlayer->uServerFlags & (1 << PSF_IN_AV_CAMERA_MODE)) {
                SceneCreateAVFrustum(ptPlayer, tAVFrustum);
            }

            idSWorld = ptACDPlayer->GetSWorldID();
            return TRUE;
        }
    }

    return FALSE;
}

void CActivationUpdate() {  // line 101
    Frustum tFrustum;  // line 104
    Frustum tAVFrustum;  // line 105
    SWorldID idSWorld;  // line 106
    Player * ptLocalPlayer = LocalPlayerGet();  // line 107
    if (ptLocalPlayer == NULL) {
        return;
    }

    Frustum * ptAVFrustum = (ptLocalPlayer->uServerFlags & (1 << PSF_IN_AV_CAMERA_MODE)) ? &tAVFrustum : NULL;  // line 112

    if (!sShouldTryToActivateScenes(tFrustum, tAVFrustum, idSWorld)) {  // line 117
        CScenesUpdateEnvironmentRActors();
        return;
    }
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(ptLocalPlayer->nPlayerIndex);  // line 127
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);  // line 128
        WorldLocation & loc = tACDPlayer.GetWorldLocation();  // line 129
        const Vector3D & vCenter = loc.transform.wp;  // line 130
        SceneActivateVisible(idSWorld, tFrustum, tFrustum, ptAVFrustum, vCenter, &ptLocalPlayer, 1);
    }

    ParticleSetLastSeenTime(tFrustum);
    AmbientSounds::SetLastSeenTime(tFrustum);
    LightsSetLastSeenTime(tFrustum);
    RActorSetLastSeenTime(tFrustum);
    SceneDeactivateUnseen();
    ParticleCheckSeenRecently();
    AmbientSounds::CheckSeenRecently();
    LightsCheckSeenRecently();
    RActorCheckSeenRecently();

    WorldLocation & loc = ACDGet(LocalPlayerACDGet()).GetWorldLocation();  // line 164
    Vector3D & vCenterPos = loc.transform.wp;  // line 165
    SceneCheckInactiveMarkers(idSWorld, tFrustum, vCenterPos);
    InactiveMarkerActivateInFrustrumForWorld(idSWorld, tFrustum, vCenterPos);

    CScenesUpdateEnvironmentRActors();
}
