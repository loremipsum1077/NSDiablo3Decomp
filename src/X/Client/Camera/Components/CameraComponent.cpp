#include "Contrib/Contrib/blz/include/blz/float.h"
#include "X/Client/Camera/Components/CameraComponent.h"

#include "X/Client/Camera/CameraManager.h"
#include "X/Client/CGame.h"
#include "X/Client/CPlayer.h"
#include "X/Client/Console/LocalPlayerClamping.h"
#include "X/Client/Gfx/GfxVB.h"
#include "X/Client/RActors/RActors.h"
#include "X/Common/ActorCommonData.h"
#include "X/Common/Player.h"
#include "X/Sigma/Math/Intersect.h"
#include "X/Sigma/Math/Plane.h"
#include "X/Sigma/Math/Ray.h"
#include "X/Sigma/Math/Rect.h"
#include <float.h>

SWorldID CWorldGetCurrentWorld();  // X/Client/CWorld.cpp:1487
namespace LocalPlayerClamping {
BOOL IsPlayerInactive(int32 nControllerIndex);  // LocalPlayerClamping.cpp:256
BOOL IsPlayerMovingOrCasting(int32 nControllerIndex);  // LocalPlayerClamping.cpp:267
}

float s_flMinMovementDistance = 0.1f;
Vector3D s_vLastCentroid = cgwpNull;

CameraComponent::CameraComponent()
    : snoObserver(CameraManagerGetDefaultObserver(FALSE))
    , bDisabled(FALSE)
    , bForceAllPlayersForCentroid(FALSE)
    , m_nActivePlayers(0)
    , m_nMovingPlayers(0)
    , bRanFirstTick(FALSE), m_bUseInactiveCentroid(FALSE)
{
    m_wpCentroidLocation = cgplaceNull;
}


void CameraComponent::Update(const CameraState &, CameraState &, float)
{
    if (!bRanFirstTick)
        OnFirstTick();

    ProcessCentroidLocation();
}






void CameraComponent::ProcessCentroidLocation()
{
    WorldPlace wpPreviousCentroidLocation = m_wpCentroidLocation;

    m_wpCentroidLocation.wp.Set(0.0f, 0.0f, 0.0f);
    m_wpCentroidLocation.idSWorld = SWORLDID_INVALID;

    Rect2D tRect(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);

    float flZ = 0.0f;

    uint32 nPlayerCount = 0;

    uint32 nLastFrameActive = m_nActivePlayers;
    uint32 nLastFrameMoving = m_nMovingPlayers;

    m_nActivePlayers = m_nMovingPlayers = 0;

    uint32 nLivingPlayers = 0;

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!PlayerIsLocal(ptPlayer))
        {
            continue;
        }

        const ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);


        if (tACD.GetPlace().idSWorld != CWorldGetCurrentWorld())
        {
            continue;
        }


        if (tACD.IsAlive())
        {
            nLivingPlayers++;
        }

        if (bForceAllPlayersForCentroid || !LocalPlayerClamping::IsPlayerInactive(ptPlayer->nControllerIndex))
        {



            if (LocalPlayerClamping::IsPlayerMovingOrCasting(ptPlayer->nControllerIndex))
            {
                m_nMovingPlayers++;
            }

            m_nActivePlayers++;
        }
    }
    if (m_bUseInactiveCentroid)
    {
        m_bUseInactiveCentroid = m_nMovingPlayers == 0;
    }
    else
    {
        m_bUseInactiveCentroid = m_nActivePlayers == 0 || (m_nActivePlayers < nLastFrameActive && m_nMovingPlayers == nLastFrameMoving && m_nMovingPlayers == 0);
    }


    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!PlayerIsLocal(ptPlayer))
        {
            continue;
        }

        if (!bForceAllPlayersForCentroid)
        {
            if (!m_bUseInactiveCentroid && LocalPlayerClamping::IsPlayerInactive(ptPlayer->nControllerIndex) && m_nActivePlayers > 0)
            {
                continue;
            }
        }

        const ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);


        if (tACD.GetPlace().idSWorld != CWorldGetCurrentWorld())
        {
            continue;
        }

        if (!bForceAllPlayersForCentroid)
        {
            if (!tACD.IsAlive() && nLivingPlayers > 0)
            {
                continue;
            }
        }

        RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        Vector3D vecWP = ptRActor ? RActorGetWorldLocation(ptRActor).transform.wp : tACD.GetPlace().wp;

        m_wpCentroidLocation.wp = vecWP;

        Vector2D screenPos;

        if (WorldPointToScreenPoint(vecWP, screenPos, CAMERA_CURRENT))
        {



            if (tRect.left > screenPos.x)
            {
                tRect.left = screenPos.x;
            }

            if (tRect.right < screenPos.x)
            {
                tRect.right = screenPos.x;
            }

            if (tRect.top > screenPos.y)
            {
                tRect.top = screenPos.y;
            }

            if (tRect.bottom < screenPos.y)
            {
                tRect.bottom = screenPos.y;
            }

            flZ += vecWP.z;
            nPlayerCount++;
        }
    }
    if (nPlayerCount > 0)
    {
        float flPlayerCount = (float)nPlayerCount;
        flZ /= flPlayerCount;

        LineSeg3D wline;
        IVector2D iVec((int32)tRect.Center().x, (int32)tRect.Center().y);
        ScreenPointToWorldLineSeg(iVec, wline, CAMERA_CURRENT);

        Plane p;
        p.Set(Vector3D(0.0f, 0.0f, flZ), cgwvZAxis);

        Intersect::LineSeg3DAndPlane(wline, p, m_wpCentroidLocation.wp);

        float flDist = Vector3D::DistanceSquared(wpPreviousCentroidLocation.wp, m_wpCentroidLocation.wp);

        if (flDist < s_flMinMovementDistance * s_flMinMovementDistance)
        {
            m_wpCentroidLocation = wpPreviousCentroidLocation;
        }
    }

    s_vLastCentroid = m_wpCentroidLocation.wp;
}
