#include "X/Client/Camera/HardpointViewController.h"
#include "X/Client/Camera/Camera.h"
#include "X/Client/Camera/CameraLocation.h"
#include "X/Client/Camera/CameraManager.h"
#include "X/Client/RActors/RActors.h"

// Declared here because the header is not adopted yet (only the overload with a bone index is).
// X/Client/RActors/RActorAttachments.cpp
void RActorGetHardpointTransform(const RActor & tRActor, const Hardpoint & tHardpoint, PRTransform & transform);


HardpointViewController::HardpointViewController()
{
    m_idRActor = RACTORID_INVALID;
}

void HardpointViewController::Attach(const RActorID idRActor, const Hardpoint & tHardpoint)
{
    m_idRActor = idRActor;
    m_tHardpoint = tHardpoint;
}



















void HardpointViewController::Update()

{
    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor == NULL)
        return;



    PRTransform transform;
    RActorGetHardpointTransform(*ptRActor, m_tHardpoint, transform);

    Vector3D wvNewForward = transform.Rotate(cgwvXAxis);
    Vector3D wvNewRight = -transform.Rotate(cgwvYAxis);
    Vector3D wvNewUp = transform.Rotate(cgwvZAxis);
    transform.q.FromChangeOfBasis(cgwvInitCameraForward, cgwvInitCameraRight, cgwvInitCameraUp, wvNewForward, wvNewRight, wvNewUp);







    CameraState cmStateNew = CameraManagerGetCameraState(CAMERA_CURRENT);
    cmStateNew.transform = transform;


    cmStateNew.matIrrad = cgm4x4Identity;


    CameraManagerSetCameraState(CAMERA_CURRENT, cmStateNew);
    CameraManagerSetMicrophoneTransform(CAMERA_CURRENT, transform, transform);
}
