#include "X/Client/Camera/Components/CameraComponent.h"
#include "X/Client/Camera/Components/SpringFollowComponent.h"


void SpringFollowComponent::OnFirstTick()
{
    CameraComponent::OnFirstTick();
    m_tLastLocation = GetCentroidLocation();
}

void SpringFollowComponent::OnWarped()
{
    m_bWarp = TRUE;
    m_flCurrentSpeed = 0.0f;
}

#include "X/Client/Camera/CameraLocation.h"
#include "X/Client/CPlayer.h"
#include "X/Client/RActors/RActors.h"
#include "X/Common/ActorCommonData.h"
#include "X/Common/Player.h"
#include "X/Common/Definitions/ObserverDefinition.h"
#include <math.h>

// RActors.cpp:6737, original const-qualified declaration.
WorldPlace RActorGetWorldPlace(const RActor* ptRActor);

static float s_flSpringFollowCameraMinSpeed = 3.0f;
static float s_flSpringFollowCameraMaxSpeed = 100.0f;
static float s_flSpringFollowCameraWarpDistance = 65.0f;
static float s_flSpringFollowCameraMaxSimulationTime = 0.1f;
static float s_flSpringFollowCameraSpringFactor = 200.0f;
static float s_flSpringFollowCameraSpringExponent = 1.5f;
static float s_flSpringFollowCameraSpringAccelerationLimit = 0.3f;
static float s_flSpringFollowCameraSpringAccelerationLimitInactive = 0.005f;

SpringFollowComponent::SpringFollowComponent() : m_bWarp(FALSE), m_flCurrentSpeed(0.0f)
{
}
SpringFollowComponent::~SpringFollowComponent()
{
}
CameraComponentType SpringFollowComponent::GetCameraComponentType()
{
    return SPRING_FOLLOW_COMPONENT;
}
// Original line table: m_bWarp = FALSE at 205 precedes m_tLastLocation = tCurrentLocation (operator= inlined at 207).
void SpringFollowComponent::Update(const CameraState& csLastFrame, CameraState& csThisFrame, float flDeltaSec)
{
    CameraComponent::Update(csLastFrame, csThisFrame, flDeltaSec);
    if (bDisabled) return;
    if (snoObserver != -1)
    {
        ScopedObserverDefinitionLockR pdefObserver(snoObserver);
        s_flSpringFollowCameraMinSpeed = pdefObserver->flConsoleSpringFollowCameraMinSpeed;
        s_flSpringFollowCameraMaxSpeed = pdefObserver->flConsoleSpringFollowCameraMaxSpeed;
        s_flSpringFollowCameraSpringFactor = pdefObserver->flConsoleSpringFollowCameraSpringFactor;
        s_flSpringFollowCameraSpringExponent = pdefObserver->flConsoleSpringFollowCameraSpringExponent;
    }
    uint32 tElapsedMs = (uint32)(flDeltaSec * 1000.0f);
    if (flDeltaSec > s_flSpringFollowCameraMaxSimulationTime) tElapsedMs = 100;
    WorldPlace tCurrentLocation = m_tLastLocation;
    WorldPlace tTargetLocation = GetCentroidLocation();
    if (m_bWarp && m_flCurrentSpeed <= 0.000001f)
    {
        Player* ptPlayer = LocalPlayerGet();
        ActorCommonData* ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
        RActor* ptPlayerRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptPlayerACD || ptPlayerRActor)
        {
            tTargetLocation = ptPlayerRActor ? RActorGetWorldPlace(ptPlayerRActor) : ptPlayerACD->GetPlace();
        }
    }
    if (tElapsedMs > 0 && !m_bWarp)
    {
        if (tCurrentLocation.idSWorld != tTargetLocation.idSWorld) m_bWarp = TRUE;
        else
        {
            if (flDeltaSec > s_flSpringFollowCameraMaxSimulationTime) flDeltaSec = s_flSpringFollowCameraMaxSimulationTime;
            Vector3D vecDir = tTargetLocation.wp - tCurrentLocation.wp;
            float flDirMagnitude = vecDir.Magnitude();
            float flMinDisplacement = flDeltaSec * s_flSpringFollowCameraMinSpeed;
            if (flDirMagnitude > s_flSpringFollowCameraWarpDistance || flDirMagnitude < flMinDisplacement + 0.000001f) m_bWarp = TRUE;
            else
            {
                float flNewDisplacement = 0.0f;
                for (uint32 t = 0; t < tElapsedMs; ++t)
                {
                    float flSpeed = powf((flDirMagnitude - flNewDisplacement) / s_flSpringFollowCameraSpringFactor, s_flSpringFollowCameraSpringExponent);
                    float flDeltaSpeed = flSpeed - m_flCurrentSpeed;
                    if (flDeltaSpeed > 0.0f)
                    {
                        if (m_nMovingPlayers != 0 || PlayerGetLocalPlayerCount() == 1)
                        {
                            if (flDeltaSpeed > s_flSpringFollowCameraSpringAccelerationLimit) flSpeed = s_flSpringFollowCameraSpringAccelerationLimit;
                        }
                        else if (flDeltaSpeed > s_flSpringFollowCameraSpringAccelerationLimitInactive) flSpeed = s_flSpringFollowCameraSpringAccelerationLimitInactive;
                    }
                    m_flCurrentSpeed = flSpeed;
                    flNewDisplacement += flSpeed;
                    if (flNewDisplacement > flDirMagnitude)
                    {
                        flNewDisplacement = flDirMagnitude;
                        break;
                    }
                }
                flNewDisplacement += flMinDisplacement;
                if (flNewDisplacement > flDirMagnitude) flNewDisplacement = flDirMagnitude;
                float flMaxDisplacement = flDeltaSec * s_flSpringFollowCameraMaxSpeed;
                if (flNewDisplacement > flMaxDisplacement) flNewDisplacement = flMaxDisplacement;
                Vector3D vecNormalizedDir = vecDir * (1.0f / flDirMagnitude);
                tCurrentLocation.wp += flNewDisplacement * vecNormalizedDir;
            }
        }
    }
    if (m_bWarp) tCurrentLocation = tTargetLocation;
    m_bWarp = FALSE;
    m_tLastLocation = tCurrentLocation;
    Vector3D wvCameraOffsetVector = csThisFrame.wvCameraOffsetVector;
    Vector3D wvLookAtOffsetVector = csThisFrame.wvLookAtOffsetVector;
    Vector3D wvDirectionVector = wvCameraOffsetVector;
    wvDirectionVector.Normalize();
    Vector3D wpLookFrom = tCurrentLocation.wp + wvCameraOffsetVector + wvDirectionVector;
    Vector3D wpLookAt = wvLookAtOffsetVector + tCurrentLocation.wp;
    csThisFrame.transform.Set(wpLookFrom, wpLookAt, cgwvZAxis);
    csThisFrame.wvLookAt = wpLookAt;
    csThisFrame.wvLookFrom = wpLookFrom;
}
