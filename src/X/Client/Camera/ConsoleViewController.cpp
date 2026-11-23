// Original file: X/Client/Camera/ConsoleViewController.cpp
#include "ConsoleViewController.h"

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CGame.h"
#include "CameraLocation.h"
#include "CameraManager.h"
#include "Components/CameraComponent.h"
#include "Components/CameraSettingsComponent.h"
#include "Components/LeftTriggerZoomComponent.h"
#include "Components/OrbitFollowComponent.h"
#include "Components/ShakeComponent.h"
#include "Components/SpringFollowComponent.h"
#include "../../Common/Definitions/WorldDefinition.h"
#include "../../Common/World.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Transform.h"








ConsoleViewController::ConsoleViewController()
    : bIsOrbiting(FALSE)
    , matIrradFixedView(cgm4x4Identity)

{
    listComponents.AddTail_Tracked(new CameraSettingsComponent());
    listComponents.AddTail_Tracked(new LeftTriggerZoomComponent());
    listComponents.AddTail_Tracked(new SpringFollowComponent());
    listComponents.AddTail_Tracked(new OrbitFollowComponent());

    ptCameraShake = new ShakeComponent();
}


ConsoleViewController::~ConsoleViewController()
{
    ::delete ptCameraShake; ptCameraShake = NULL;


    while (!listComponents.IsEmpty())
    {
        CameraComponent * ptComponent = listComponents.GetHead();
        ::delete ptComponent;
        listComponents.RemoveHead();
    }
}




void ConsoleViewController::OnWarped()
{
    POS pos = listComponents.GetHeadPosition();
    while (pos)
    {
        CameraComponent * ptComponent = listComponents.GetNext(pos);
        ptComponent->OnWarped();
    }
}










void ConsoleViewController::ClampFarClip(CameraState & csState)




{
    SWorldID idSWorld = CGameGetCurrentSWorldID();









    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
    if (snoWorld != SNO_NONE)
    {
        ScopedWorldDefinitionLockR pdefWorld(snoWorld);

        if (pdefWorld->tEnvironment.flFarPlaneCap > 1.0f)
        {
            csState.flFarPlane = Math::Min(csState.flFarPlane, pdefWorld->tEnvironment.flFarPlaneCap);

            if (csState.flNearPlane > csState.flFarPlane)
            {
                csState.flNearPlane = csState.flFarPlane - 0.1f;
            }
        }
    }
}





void ConsoleViewController::UpdateComponentStack(float flDeltaTime)

{
    CameraState csLast, csThis;
    csLast = CameraManagerGetCameraState(-1);
    csThis = csLast;

    csThis.flZoomFactor = 0.0f;


    POS pos = listComponents.GetHeadPosition();
    while (pos)
    {
        CameraComponent * ptComponent = listComponents.GetNext(pos);
        ptComponent->Update(csLast, csThis, flDeltaTime);
    }

    ClampFarClip(csThis);


    PRTransform preShakeTransform;
    preShakeTransform.Set(csThis.wvLookFrom, csThis.wvLookAt, cgwvZAxis);
    preShakeTransform.q *= csThis.qPostTransformRotation;

    CameraManagerSetMicrophoneTransformFromTarget(-1, preShakeTransform, csThis.wvLookAt);

    ptCameraShake->Update(csLast, csThis, flDeltaTime);

    csThis.transform.Set(csThis.wvLookFrom, csThis.wvLookAt, cgwvZAxis);
    csThis.transform.q *= csThis.qPostTransformRotation;

    if (!bIsOrbiting)
    {
        csThis.matIrrad = cgm4x4Identity;
    }
    else
    {

        Matrix4x4 matView;
        csThis.transform.ToMatrix(matView);
        csThis.matIrrad = matView * matIrradFixedView.Inverse();
    }

    CameraManagerSetCameraState(-1, csThis);
}

void ConsoleViewController::SetComponentObservers(const SNO snoObserver)
{

    POS pos = listComponents.GetHeadPosition();
    while (pos)
    {
        CameraComponent * ptComponent = listComponents.GetNext(pos);
        if (ptComponent)
        {
            ptComponent->SetObserver(snoObserver);
        }
    }
}


void ConsoleViewController::SetOrbitPlace(const WorldPlace & wpToOrbit)
{

    bIsOrbiting = TRUE;

    POS pos = listComponents.GetHeadPosition();
    while (pos)
    {
        CameraComponent * ptComponent = listComponents.GetNext(pos);
        if (ptComponent)
        {
            CameraComponentType eCamComponentType = ptComponent->GetCameraComponentType();
            switch (eCamComponentType)
            {
                case ORBIT_FOLLOW_COMPONENT: {
                    OrbitFollowComponent * ptOrbitFollowComponent = (OrbitFollowComponent *)ptComponent;
                    ptOrbitFollowComponent->SetOrbitPlace(wpToOrbit);
                    ptOrbitFollowComponent->SetDisabled(FALSE);
                    break;
                }
                case FIXED_VIEW_COMPONENT: ptComponent->SetDisabled(TRUE); break;
                case LEFT_TRIGGER_ZOOM_COMPONENT: ptComponent->SetDisabled(TRUE); break;
                case SPRING_FOLLOW_COMPONENT: {
                    // The DWARF local here is `FixedViewComponent * ptFixedViewComponent`; that class is not in src.







                    ptComponent->SetDisabled(TRUE);
                    break;
                }
            }
        }
    }
}
void ConsoleViewController::ForceClearOrbitComponent()
{

    POS pos = listComponents.GetHeadPosition();
    while (pos)
    {
        CameraComponent * ptComponent = listComponents.GetNext(pos);
        if (ptComponent)
        {
            CameraComponentType eCamComponentType = ptComponent->GetCameraComponentType();
            if (eCamComponentType == ORBIT_FOLLOW_COMPONENT)
            {
                ptComponent->SetDisabled(TRUE);
            }
        }
    }
}



void ConsoleViewController::ClearOrbitPlace()

{
    if (!bIsOrbiting)
    {
        return;
    }

    bIsOrbiting = FALSE;

    POS pos = listComponents.GetHeadPosition();
    while (pos)
    {
        CameraComponent * ptComponent = listComponents.GetNext(pos);
        if (ptComponent)
        {
            CameraComponentType eCamComponentType = ptComponent->GetCameraComponentType();
            switch (eCamComponentType)
            {
                case ORBIT_FOLLOW_COMPONENT: {
                    OrbitFollowComponent * ptOrbitFollowComponent = (OrbitFollowComponent *)ptComponent;
                    ptOrbitFollowComponent->EnableExitTransition();
                    break;
                }
                case FIXED_VIEW_COMPONENT: ptComponent->SetDisabled(FALSE); break;
                case LEFT_TRIGGER_ZOOM_COMPONENT: ptComponent->SetDisabled(FALSE); break;
                case SPRING_FOLLOW_COMPONENT: ptComponent->SetDisabled(FALSE); break;
            }
        }
    }
}











void ConsoleViewController::Update(float flDeltaTime, const BOOL b)


{

    if (CGameInScriptedSequence() && !bIsOrbiting)
    {

        FixedViewController::Update(flDeltaTime, b);
    }
    else
    {

        UpdateComponentStack(flDeltaTime);
        if (bIsOrbiting) return;
    }
    UpdateFixedIrradView();
}












void ConsoleViewController::UpdateFixedIrradView()
{
    CameraState csCurrent = CameraManagerGetCameraState(-1);
    csCurrent.transform.ToMatrix(matIrradFixedView);
}
