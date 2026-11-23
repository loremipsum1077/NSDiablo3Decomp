// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/ConsoleViewController.h

#pragma once

#include "CameraManager.h"
#include "FixedViewController.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/Math/Matrix.h"

class CameraComponent;
struct CameraState;
struct WorldPlace;

class ConsoleViewController : public FixedViewController {  // size 0x158
public:
    ConsoleViewController();
    virtual ~ConsoleViewController();  // vtable slot 0
    virtual void Update(float flDeltaTime, const BOOL b);  // vtable slot 2
    void AddComponent(CameraComponent *);
    virtual void OnWarped();  // vtable slot 3
    void SetComponentObservers(const SNO snoObserver);
    void SetOrbitPlace(const WorldPlace & wpToOrbit);
    void ForceClearOrbitComponent();
    void ClearOrbitPlace();
    BOOL IsOrbiting() { return bIsOrbiting; }  // line 38
    virtual ViewControllerType GetViewControllerType() { return CONSOLE_VIEW_CONTROLLER; }  // vtable slot 5; line 40
private:
    void UpdateComponentStack(float flDeltaTime);
    void UpdateFixedIrradView();
    void ClampFarClip(CameraState & csState);
    typedef XGrowableList<CameraComponent *, CameraComponent *> ComponentList;
    ComponentList listComponents;  // 0xb0
    CameraComponent * ptCameraShake;  // 0x108
    BOOL bIsOrbiting;  // 0x110
    Matrix4x4 matIrradFixedView;  // 0x114
};
static_assert(sizeof(ConsoleViewController) == 0x158, "ConsoleViewController");
