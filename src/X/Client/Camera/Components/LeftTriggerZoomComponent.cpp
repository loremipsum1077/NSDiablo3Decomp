#include "X/Client/Camera/Components/LeftTriggerZoomComponent.h"
#include "../CameraLocation.h"
#include "X/Common/Player.h"
#include "X/Common/ActorCommonData.h"
#include "X/Common/Game.h"
#include "X/Common/Definitions/ObserverDefinition.h"
#include "X/Common/Definitions/GlobalDefinition.h"
#include "X/Client/CPlayer.h"
#include "X/Client/CGame.h"
#include "X/Client/CWorld.h"
#include "X/Client/Scene.h"
#include "X/Client/RActors/RActors.h"
#include "X/Sigma/Math/MathGeneral.h"
// Original declarations from DWARF; implementations owned by other translation units.
BOOL PlayerIsLocal(const Player*);
uint32 PlayerGetLocalPlayerCount();
WorldPlace RActorGetWorldPlace(const RActor*);
namespace Console {
namespace ControllerUtils {
float GetLTriggerPosition(int32 nController);
}
}
// Player.h:983: exact member load survives in UpdateLeftTriggerZoom.
inline float ConsoleTargetComponent::GetReticleDistanceRatio()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return flReticleDistanceRatio;
}
#line 28 "X/Client/Camera/Components/LeftTriggerZoomComponent.cpp"
float s_flLocalPlayerDistanceZoomScaling = 0.016f;
#line 31 "X/Client/Camera/Components/LeftTriggerZoomComponent.cpp"
static float sflZoom = 0.08f;
#line 35 "X/Client/Camera/Components/LeftTriggerZoomComponent.cpp"
void LeftTriggerZoomComponent::Update(const CameraState& csLastFrame, CameraState& csThisFrame, float deltaSec)
{
    CameraComponent::Update(csLastFrame, csThisFrame, deltaSec);
    if (bDisabled) return;
    UpdateLeftTriggerZoom(deltaSec);
    float flLocalPlayerDistanceZoom = CalculateLocalPlayerDistanceZoom();
    if (sflZoom - flLocalPlayerDistanceZoom > 0.005f) sflZoom -= 0.005f;
    else sflZoom = flLocalPlayerDistanceZoom;
    if (CGameLoadingScreenIsEnabled()) sflZoom = 0.0f;
    CameraState csStart = csThisFrame;
    SNO snoObserver = GlobalSNOGet(CWorldIsRetro() ? 0x410 : 0x40f);
    CameraStateInterpolate(csThisFrame, csStart, snoObserver, Math::Max(m_flLeftTriggerZoom, sflZoom));
    csThisFrame.flZoomFactor = Math::Max(m_flLeftTriggerZoom, sflZoom);
    csThisFrame.transform = csStart.transform;
}
#line 71 "X/Client/Camera/Components/LeftTriggerZoomComponent.cpp"
void LeftTriggerZoomComponent::UpdateLeftTriggerZoom(
    float deltaSec)
{
    if (PlayerGetLocalPlayerCount() > 1)
    {
        return;
    }

    Player* ptPlayer = LocalPlayerGet();

    float flZoom = 0.0f;
    if (ptPlayer->tTargetComponent.GetInAimMode())
    {
        flZoom = Console::ControllerUtils::GetLTriggerPosition(ptPlayer->nControllerIndex);
    }
    if (ptPlayer->tTargetComponent.GetIsHoldingReticlePowerDown())
    {
        flZoom = Math::Max(flZoom, ptPlayer->tTargetComponent.GetReticleDistanceRatio());
    }


    if ((!ptPlayer->tTargetComponent.GetInAimMode() && !ptPlayer->tTargetComponent.GetIsHoldingReticlePowerDown()) || (flZoom < 0.1f))
    {
        flZoom = 0.0f;
    }
    else if (flZoom > 0.9f)
    {
        flZoom = 1.0f;
    }
    else
    {
        int32 nZoom = static_cast<int32>(flZoom * 100.0f);

        flZoom = nZoom / 100.0f;

    }

    flZoom = Math::Max(flZoom, FindIfAnyLocalPlayerIsInAZoomedOutScene());

    if (snoObserver != -1)
    {
        ScopedObserverDefinitionLockR pdefObserver(snoObserver);
        m_flZoomSpeed = pdefObserver->flConsoleCameraZoomSpeed;
    }

    float flDelta = m_flZoomSpeed * deltaSec;

    float flAbsDiff = Math::Abs(m_flLeftTriggerZoom - flZoom);

    flDelta = Math::Min(flDelta, flAbsDiff);

    if (flZoom < m_flLeftTriggerZoom)
    {
        flDelta = -flDelta;
    }
    else if (flZoom == m_flLeftTriggerZoom)
    {
        flDelta = 0.0f;
    }


    m_flLeftTriggerZoom += flDelta;
    m_flLeftTriggerZoom = Math::Pin(m_flLeftTriggerZoom, 0.0f, 1.0f);
}
#line 137 "X/Client/Camera/Components/LeftTriggerZoomComponent.cpp"
float LeftTriggerZoomComponent::CalculateLocalPlayerDistanceZoom() const
{
    if (PlayerGetLocalPlayerCount() < 2) return 0.0f;
    float flMaxDist = 0.0f;
    for (const Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {
        if (!PlayerIsLocal(ptPlayer)) continue;
        const ActorCommonData& tACD = ACDGet(ptPlayer->idPlayerACD);
        RActor* ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        WorldPlace tPlace = ptRActor ? RActorGetWorldPlace(ptRActor) : tACD.GetPlace();
        for (const Player* ptPlayer2 = ptPlayer; ptPlayer2; ptPlayer2 = PlayerGetNextInGame(ptPlayer2)) {
            if (ptPlayer == ptPlayer2 || !PlayerIsLocal(ptPlayer2)) continue;
            const ActorCommonData& tACD2 = ACDGet(ptPlayer2->idPlayerACD);
            RActor* ptRActor2 = RActorTryToGet(ptPlayer2->idPlayerRActor);
            WorldPlace tPlace2 = ptRActor2 ? RActorGetWorldPlace(ptRActor2) : tACD2.GetPlace();
            float flDist = Vector3D::Distance(tPlace.wp, tPlace2.wp);
            flMaxDist = Math::Max(flDist, flMaxDist);
        }
    }
    float flZoomFactor = flMaxDist * s_flLocalPlayerDistanceZoomScaling;
    return Math::Min(1.0f, flZoomFactor);
}
#line 198 "X/Client/Camera/Components/LeftTriggerZoomComponent.cpp"
float LeftTriggerZoomComponent::FindIfAnyLocalPlayerIsInAZoomedOutScene()
{
    for (const Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {
        if (PlayerIsLocal(ptPlayer)) {
            const ActorCommonData& tACD = ACDGet(ptPlayer->idPlayerACD);
            WorldLocation tLocation = tACD.GetWorldLocation();
            if (tLocation.idSScene != -1) {
                CScene* ptCScene = CSceneGetFromSSceneID(tLocation.idSScene);
                if (ptCScene && (ptCScene->dwFlags & 0x80)) return 1.0f;
            }
        }
    }
    return 0.0f;
}
#line 228 "X/Client/Camera/Components/LeftTriggerZoomComponent.cpp"
void LeftTriggerZoomComponent::OnWarped()
{
    m_flLeftTriggerZoom = 0.0f;
}
// Original header bodies, defined locally to avoid changing shared includers.
#line 20 "X/Client/Camera/Components/LeftTriggerZoomComponent.h"
LeftTriggerZoomComponent::~LeftTriggerZoomComponent()
{
}
#line 26 "X/Client/Camera/Components/LeftTriggerZoomComponent.h"
CameraComponentType LeftTriggerZoomComponent::GetCameraComponentType()
{
    return 3;
}
