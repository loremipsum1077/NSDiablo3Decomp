#include "Contrib/Contrib/blz/include/blz/float.h"
#include "X/Client/Battlenet/Subsystems/FrontendBackgroundCameraManager.h"
#include "../../CGame.h"
#include "../../Scene.h"
#include "../../CWorld.h"
#include "../../SceneAttachments.h"
#include "../../Camera/Camera.h"
#include "../../Camera/CameraLocation.h"
#include "../../Gfx/Gfx.h"
#include "../../Sound/Sound.h"
#include "../../UserInterface/Console/ConsoleScreenManager.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/ObserverDefinition.h"
#include "../../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/Math/Vector.h"

BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode);  // X/Client/Scene.cpp:3538
void CinematographyStaticView(const CameraState & cms);  // X/Client/Camera/Cinematography.cpp:414







// Values of the three names below: the strings the original loads in GetCameraPositionHardpoint (0x7100daf625/62f/63a); which name holds which string is inferred from the names (fIs43 picks Wide43 = "HP_Camera2").

const LPCSTR sgszCameraHardpointClose = "HP_Camera";
const LPCSTR sgszCameraHardpointWide = "HP_Camera1";
const LPCSTR sgszCameraHardpointWide43 = "HP_Camera2";



FrontendBackgroundCameraManager::FrontendBackgroundCameraManager()
    : m_eCameraPosition(CAM_POS_WIDE)
    , m_eCameraPositionTarget(CAM_POS_NONE)
    , m_flCameraTransitionElapsed(0.0f)
    , m_flCameraTransitionTotal(0.0f)
{


}



LPCSTR FrontendBackgroundCameraManager::GetCameraPositionHardpoint(
    BackgroundCameraPosition eCameraPosition,
    const float flAspectRatio)
{
    switch (eCameraPosition)
    {
    case CAM_POS_CLOSE:
        return sgszCameraHardpointClose;
    case CAM_POS_WIDE:
        {
            float fl43 = 4.0f / 3.0f;
            float e = 0.01f;
            BOOL fIs43 = flAspectRatio <= fl43 + e;

            return fIs43 ? sgszCameraHardpointWide43 : sgszCameraHardpointWide;
        }
    }

    // The original returns the address of an empty string here (0x7100d6a4a3).
    return "";
}



void FrontendBackgroundCameraManager::TransitionBackgroundCamera(BackgroundCameraPosition eTargetCameraPosition, const float flTransitionDurationInSeconds)
{


    if (flTransitionDurationInSeconds == 0.0f)
    {
        m_eCameraPosition = eTargetCameraPosition;
        m_eCameraPositionTarget = CAM_POS_NONE;
        return;
    }


    m_flCameraTransitionTotal = flTransitionDurationInSeconds;
    m_eCameraPositionTarget = eTargetCameraPosition;
    m_flCameraTransitionElapsed = 0.0f;
}



void FrontendBackgroundCameraManager::UpdateBackgroundCamera()

{
    if (CGameGlobalsGet().idCurrentCWorld != CWORLDID_INVALID)
    {
        CGame & tCGame = CGameGet();
        TDataArrayIterator<CScene> tIt;
        CScene * ptScene = NULL;
        int32 nCount = 0;
        while (CSceneDataArrayIterate(tIt, tCGame, SkipHolodeckScenes))
        {
            ptScene = tIt.ptElement;
            // Inferred: nCount (DWARF :101, no location) is presumably counted here; it leaves no instruction.
            nCount++;
        }

        Console::Screens::UpdateBackgroundLightBeamActor(ptScene);





        SNO snoObserver = GlobalSNOGet(GLOBALSNO_OBSERVER_BATTLENET_MAINSCREEN);






        const ObserverDefinition * pdefObserver = (const ObserverDefinition *)gptGroupDefObserver->LockRFast(snoObserver);
        Angle aFOV = pdefObserver->aFOV;
        float flZNear = pdefObserver->flZNear;
        float flZFar = pdefObserver->flZFar;


        Rect2D rectViewportNDC = cgrectViewportDefault;
        float flAspectRatioToUse = CGameGetLetterboxedViewportNDC(rectViewportNDC);

        Vector3D wvView, wvUp;
        CameraState newState;
        {
            LPCSTR szCameraHardpoint = GetCameraPositionHardpoint(m_eCameraPosition, flAspectRatioToUse);
            PRTransform transform;
            if (CSceneGetHardpointTransformByLabel(ptScene->id, HardpointHash(szCameraHardpoint), transform))
            {
                transform = ptScene->tSCD.transformWorld * transform;

                // Axes: the original loads three const Vector3D globals; X/Z inferred from the folded values, Y is cgwvYAxis.
                Vector3D wvNewForward = transform.Rotate(cgwvXAxis);
                Vector3D wvNewRight = -transform.Rotate(cgwvYAxis);
                Vector3D wvNewUp = transform.Rotate(cgwvZAxis);
                transform.q.FromChangeOfBasis(cgwvInitCameraForward, cgwvInitCameraRight, cgwvInitCameraUp, wvNewForward, wvNewRight, wvNewUp);







                wvView = wvNewForward;
                wvUp = wvNewUp;

                Vector3D vecZero(0.0f, 0.0f, 0.0f);
                CameraStateSetPerspective(newState, transform, aFOV, flAspectRatioToUse, flZNear, flZFar, vecZero, vecZero, vecZero, vecZero, rectViewportNDC, cgvecViewportScaleDefault, Vector2D(0.0f, 0.0f));










                newState.transform = transform;
            }
        }






        if (m_eCameraPositionTarget == CAM_POS_NONE)
        {
            SoundManager::ListenerSet(newState.transform.wp, newState.transform.wp, wvView, wvUp);
            CinematographyStaticView(newState);
        }
        else
        {

            float flT = m_flCameraTransitionElapsed / m_flCameraTransitionTotal;
            flT = Math::Min(1.0f, flT);



            flT = Math::SmootherStep(flT);
            m_flCameraTransitionElapsed += CGameGetRealDeltaTime();
            CameraState newStateTarget;
            PRTransform transform;
            LPCSTR szCameraHardpoint = GetCameraPositionHardpoint(m_eCameraPositionTarget, flAspectRatioToUse);
            CSceneGetHardpointTransformByLabel(ptScene->id, HardpointHash(szCameraHardpoint), transform);
            transform = ptScene->tSCD.transformWorld * transform;


            Vector3D wvNewForward = transform.Rotate(cgwvXAxis);
            Vector3D wvNewRight = -transform.Rotate(cgwvYAxis);
            Vector3D wvNewUp = transform.Rotate(cgwvZAxis);
            transform.q.FromChangeOfBasis(cgwvInitCameraForward, cgwvInitCameraRight, cgwvInitCameraUp, wvNewForward, wvNewRight, wvNewUp);








            PRTransform newTransform;
            PRTransform::Lerp(newState.transform, transform, flT, newTransform);

            Vector3D vecZero(0.0f, 0.0f, 0.0f);
            CameraStateSetPerspective(newStateTarget, newTransform, aFOV, flAspectRatioToUse, flZNear, flZFar, vecZero, vecZero, vecZero, vecZero, rectViewportNDC, cgvecViewportScaleDefault, Vector2D(0.0f, 0.0f));










            newStateTarget.transform = newTransform;
            CinematographyStaticView(newStateTarget);
            SoundManager::ListenerSet(newStateTarget.transform.wp, newStateTarget.transform.wp, wvView, wvUp);

            if (flT >= 0.999999f)
            {
                m_eCameraPosition = m_eCameraPositionTarget;
                m_eCameraPositionTarget = CAM_POS_NONE;
            }
        }
    }


}
