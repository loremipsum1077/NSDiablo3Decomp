// Original file: X/Client/Camera/Cinematography.cpp
// CinematographyUpdate, sCameraKeyDown and CinematographyCutTo written from the DWARF and line table.
#include "AVViewController.h"
#include "ActorViewController.h"
#include "CameraLocation.h"
#include "Cinematography.h"
#include "ConsoleViewController.h"
#include "HardpointViewController.h"
#include "../CGame.h"
#include "../Gfx/Gfx.h"
#include "../RActors/RActors.h"
#include "../../Common/Definitions/ObserverDefinition.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Memory/SigmaMemory.h"

#include <new>

// Declared here until their headers carry them (signatures from the DWARF).
float GfxGetAspectRatio();  // X/Client/Gfx/Gfx.cpp
void InputReset();  // X/Client/Input/Input.cpp

enum {
    MAX_STORED_CAMERA_STATES = 10,
};

// ViewControllerType values returned by GetViewControllerType (names not adopted yet).
enum { VIEW_CONTROLLER_CONSOLE = 2, VIEW_CONTROLLER_FIXED = 3 };

struct Cinematography {  // size 0xd0
    ActorViewController tHolodeckController;  // 0x00
    FixedViewController * pFixedViewController;  // 0x48
    AVViewController avViewController;  // 0x50
    HardpointViewController tHardpointViewController;  // 0x94
    CinematographyState eCinematographyState;  // 0xc0
    CameraStateList * ptStoredCameraStateList;  // 0xc8
    Cinematography();
    ~Cinematography();
};
static_assert(sizeof(Cinematography) == 0xd0, "Cinematography");

void CinematographyUpdate(float flDeltaTime, const BOOL fForceUpdateOfAllParams);
void CinematographySetState(const CinematographyState eCinState);















inline Cinematography::Cinematography()
{



    pFixedViewController = new (SigmaMemoryNew(sizeof(ConsoleViewController), 4, NULL, TRUE)) ConsoleViewController();
    eCinematographyState = CS_NONE;

    ptStoredCameraStateList = new CameraStateList;
}


Cinematography::~Cinematography()
{
    ::delete pFixedViewController;
    pFixedViewController = NULL;
    delete ptStoredCameraStateList;
    ptStoredCameraStateList = NULL;
}











void CinematographyInitialize()
{

    Cinematography *& ptCinematography = CGameGet().ptCinematography;
    ptCinematography = new (SigmaMemoryNew(sizeof(Cinematography), 4, NULL, TRUE)) Cinematography();
    InputReset();
}





void CinematographyDispose()
{

    Cinematography *& ptCinematography = CGameGet().ptCinematography;
    delete ptCinematography;
    ptCinematography = NULL;
}


static BOOL sCameraKeyDown();  // X/Client/Camera/Cinematography.cpp:767 (defined below, called at line 131)

void CinematographyUpdate(
    float flDeltaTime,
    const BOOL fForceUpdateOfAllParams)
{

    if (!CameraManagerIsInitializedForWorld())
    {
        return;
    }

    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography == NULL)
    {
        return;
    }
    // bHandled has no location in the DWARF; its initial value and later use are not visible.
    BOOL bHandled = FALSE;
    switch (ptCinematography->eCinematographyState)
    {
    case CS_NONE:
        bHandled = sCameraKeyDown();
        break;

    case CS_HOLODECK_VIEW:
        ptCinematography->tHolodeckController.Update();
        break;

    case CS_FIXED_VIEW:
        ptCinematography->pFixedViewController->Update(flDeltaTime, fForceUpdateOfAllParams);
        break;

    case CS_AV_VIEW:
        ptCinematography->avViewController.Update();
        break;

    case CS_STATIC_VIEW:
        break;

    case CS_HARDPOINT_VIEW:
        ptCinematography->tHardpointViewController.Update();
        break;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }







}




void CinematographySetDefaultZoom(const float flZoom, const float flZoomSpeed)
{
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ptCinematography->pFixedViewController->SetDefaultZoom(flZoom);
        ptCinematography->pFixedViewController->SetDefaultZoomSpeed(flZoomSpeed);
    }
}






void CinematographySetZoom(const float flZoom, const float flZoomSpeed)
{


    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        switch (ptCinematography->eCinematographyState)
        {
        case CS_HOLODECK_VIEW:
            ptCinematography->tHolodeckController.SetZoom(flZoom);
            break;
        case CS_FIXED_VIEW:
            ptCinematography->pFixedViewController->SetZoom(flZoom);
            ptCinematography->pFixedViewController->SetZoomSpeed(flZoomSpeed);
            break;
        }
    }
}





















void CinematographySetScriptedZoom(const float flZoom, const BOOL bSnap, const float flZoomDuration)
{
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ptCinematography->pFixedViewController->SetScriptedZoom(flZoom, bSnap, flZoomDuration);
    }
}






void CinematographySetYaw(const Angle aAngle, BOOL bSnap, float flDuration)
{
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ptCinematography->pFixedViewController->SetYaw(aAngle, bSnap, flDuration);
    }
}












































void CinematographySetObserver(const SNO snoObserver)
{


    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ViewControllerType eViewControllerType = ptCinematography->pFixedViewController->GetViewControllerType();
        if (eViewControllerType == VIEW_CONTROLLER_CONSOLE)
        {
            ConsoleViewController * ptConsoleViewController = (ConsoleViewController *)ptCinematography->pFixedViewController;
            ptConsoleViewController->SetComponentObservers(snoObserver);
        }
        else if (eViewControllerType == VIEW_CONTROLLER_FIXED)
        {


            FixedViewController * ptFixedViewController = ptCinematography->pFixedViewController;
            ptFixedViewController->SetObserver(snoObserver);
        }
    }
}



void CinematographySetOrbit(const WorldPlace & wpToOrbit)
{
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ViewControllerType eViewControllerType = ptCinematography->pFixedViewController->GetViewControllerType();
        if (eViewControllerType == VIEW_CONTROLLER_CONSOLE)
        {
            ConsoleViewController * ptConsoleViewController = (ConsoleViewController *)ptCinematography->pFixedViewController;
            ptConsoleViewController->SetOrbitPlace(wpToOrbit);
        }
    }
}









void CinematographyClearOrbit(BOOL bForce)
{




    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ViewControllerType eViewControllerType = ptCinematography->pFixedViewController->GetViewControllerType();
        if (eViewControllerType == VIEW_CONTROLLER_CONSOLE)
        {
            ConsoleViewController * ptConsoleViewController = (ConsoleViewController *)ptCinematography->pFixedViewController;
            if (bForce)
            {
                ptConsoleViewController->ForceClearOrbitComponent();
            }
            ptConsoleViewController->ClearOrbitPlace();
        }
    }
}




BOOL CinematographyIsOrbiting()
{


    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ViewControllerType eViewControllerType = ptCinematography->pFixedViewController->GetViewControllerType();
        if (eViewControllerType == VIEW_CONTROLLER_CONSOLE)
        {
            ConsoleViewController * ptConsoleViewController = (ConsoleViewController *)ptCinematography->pFixedViewController;
            return ptConsoleViewController->IsOrbiting();
        }
    }
    return FALSE;
}





void CinematographyCutTo(
    const PRTransform & transform)
{


    CameraManagerSetCameraTransform(CAMERA_CURRENT, transform);
    CameraManagerSetMicrophoneTransform(CAMERA_CURRENT, transform, transform);
}




void CinematographyStop()
{
    CinematographySetState(CS_NONE);
}





void CinematographyResetCameras()
{
    CameraManagerResetCameras();
    CinematographyUpdate(1.0f / 60.0f, TRUE);
}





void CinematographyStaticView(const CameraState & cms)
{


    CinematographySetState(CS_STATIC_VIEW);
    CameraManagerSetCameraState(-1, cms);
}



void CinematographyHardpointView(const RActorID idTargetRActor, const Hardpoint & tHardpoint)
{



    CinematographySetState(CS_HARDPOINT_VIEW);
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ptCinematography->tHardpointViewController.Attach(idTargetRActor, tHardpoint);
    }
}



void CinematographyFixedView(RActorID idTargetRActor)
{
    CinematographySetState(CS_FIXED_VIEW);
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ptCinematography->pFixedViewController->SetFollow(idTargetRActor, 0.0f);
    }
}









RActorID CinematographyGetFocus()
{


    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        RActorID idFocus = ptCinematography->pFixedViewController->GetFocus();
        if (RActorTryToGet(idFocus) != NULL)
        {
            return idFocus;
        }
    }
    return RACTORID_INVALID;
}



void CinematographyFocus(const RActorID idTargetRActor, BOOL bSnapFocus, float flDuration)
{




    CinematographySetState(CS_FIXED_VIEW);
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        if (idTargetRActor != RACTORID_INVALID)
        {
            ptCinematography->pFixedViewController->SetFocus(idTargetRActor, bSnapFocus, flDuration);
        }
        else
        {
            ptCinematography->pFixedViewController->ClearFocus(bSnapFocus, flDuration);
        }
    }
}




















void CinematographyFixedViewSetWheelPos(int32 nNewWheelPosition)
{
    CGameGet().ptCinematography->pFixedViewController->SetWheelPosition(nNewWheelPosition);
}






CinematographyState CinematographyGetState()
{
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        return ptCinematography->eCinematographyState;
    }
    return CS_NONE;
}

void CinematographySetState(const CinematographyState eCinState)
{





    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ptCinematography->eCinematographyState = eCinState;
        if (eCinState == CS_AV_VIEW)
        {
            ptCinematography->avViewController.Reset();
        }
    }
}








































































































































































void CinematographyPlayerWarped()
{
    Cinematography * ptCinematography = CGameGet().ptCinematography;
    if (ptCinematography != NULL)
    {
        ptCinematography->pFixedViewController->OnWarped();
    }
}







































static BOOL sCameraKeyDown()
{
    // Lines 769-912 held keyboard camera controls; only what the Switch build kept is written here.














    float flDeltaTime = CGameGetRealDeltaTime();
    const CameraState & cmState = CameraManagerGetCameraState(CAMERA_CURRENT);

    Vector3D wvCameraForward;
    Vector3D wvCameraRight;


    Vector3D wvMovement = cgwvNull;
































































    wvMovement.Normalize();

    {
        SNO snoObserver = CameraManagerGetDefaultObserver(FALSE);
        ScopedObserverDefinitionLockR pdefObserver(snoObserver);
        wvMovement *= flDeltaTime * pdefObserver->wdMaxVelForward;
    }

    Vector3D wvAxisOfRotation = cgwvNull;




















    float fRotationalMag = wvAxisOfRotation.Magnitude();
    if (wvMovement.Magnitude() > EPSILON || fRotationalMag > EPSILON)
    {

        PRTransform transformNew = cmState.transform;
        transformNew.wp += wvMovement;

        if (fRotationalMag > EPSILON)
        {
            wvAxisOfRotation.Normalize();
            // constants from the binary: 0.017453292f (PI / 180) and 0.016666668f (1 / 60), multiplied in this order

            const Angle aAngle = Math::NormalizeAngle(flDeltaTime * (PI / 180.0f) * (1.0f / 60.0f));
            Quaternion qRotation;
            qRotation.SetRotation(wvAxisOfRotation, aAngle);
            transformNew.q = qRotation * transformNew.q;
        }

        CinematographyCutTo(transformNew);
        // return value invented: the only caller ignores it
        return TRUE;
    }

    return FALSE;
}











































void CinematographyCutToStaticView(const SNO snoObserver, const Vector3D & wpCenter)
{
    float flAspectRatio = GfxGetAspectRatio();
    Vector3D wpLookTo;
    Vector3D wpLookFrom;
    PRTransform transform;
    CameraState cms;
    ScopedObserverDefinitionLockR pdefObserver(snoObserver);

    wpLookTo = wpCenter + pdefObserver->wvCameraOffsetVector;
    wpLookFrom = wpCenter + pdefObserver->wvLookAtOffsetVector;
    transform.Set(wpLookTo, wpLookFrom, cgwvZAxis);




    CameraStateSetPerspective(cms, transform, pdefObserver->aFOV, flAspectRatio, pdefObserver->flZNear, pdefObserver->flZFar, pdefObserver->wvCameraOffsetVector, pdefObserver->wvLookAtOffsetVector, wpLookTo, wpLookFrom, cgrectViewportDefault, cgvecViewportScaleDefault, Vector2D(0.0f, 0.0f));










    CinematographyStaticView(cms);
}
