#include "Camera.h"
#include "CameraLocation.h"
#include "../CGame.h"
#include "../RWindow.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
// Inferences (each is a statement or condition with no instruction of its own, or a form the DWARF does not give):
// X/Client/Gfx/Gfx.cpp:2125, :2205 and :2250 (build/dwarf/headers/X/Client/Gfx/Gfx.cpp); no declaration of these is in the DWARF of this file.
void GfxBuildPerspectiveProjectionMatrix(Matrix4x4 & m, const Angle aFOV, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const BOOL fRightHanded);
void GfxBuildOrthographicProjectionMatrix(Matrix4x4 & m, const float flHeight, const float flAspectRatio, const float flNearPlane, const float flFarPlane, const BOOL fRightHanded);
void GfxBuildViewMatrix(Matrix4x4 & m, const Vector3D & wpFrom, const Vector3D & wvView, const Vector3D & wvUp, const BOOL fRightHanded);
// - Camera.cpp:20-22: the values come from tools/data.py (0x7100e3f864/870/87c); they are const-initialised in the original.
// - Camera.cpp:183: left/top are cleared as two member stores (one 64-bit store, line 183); no Set() is inlined in the DWARF.
// - Camera.cpp:187: right/bottom come from the two halves of the IVector2D returned in x0 (stp w0, w8, line 187).
// - Camera.cpp:210-211: the empty-viewport path stores cgwpNull into wp and returns; the epilogue is shared with line 221.

const Vector3D cgwvInitCameraForward(0.0f, 0.0f, 1.0f);
const Vector3D cgwvInitCameraRight(0.0f, 1.0f, 0.0f);
const Vector3D cgwvInitCameraUp(1.0f, 0.0f, 0.0f);





void CameraSetPerspectiveProjection(
Camera & cam,
const Angle aFOV,
const float flAspectRatio,
const float flNearPlane,
const float flFarPlane,
const Vector2D & vecScale,
const Vector2D & vecProjectionTranslate)
{
    GfxBuildPerspectiveProjectionMatrix(cam.mProjectionUnadjusted, aFOV, flAspectRatio, flNearPlane, flFarPlane, 1);

    if (cam.dwHighResScreenshotTilesPerRow != 0) {

        uint32 dwX = cam.dwHighResScreenshotTile % cam.dwHighResScreenshotTilesPerRow;
        uint32 dwY = cam.dwHighResScreenshotTile / cam.dwHighResScreenshotTilesPerRow;
        float flS = (float)cam.dwHighResScreenshotTilesPerRow;
        float flDX = (float)(cam.dwHighResScreenshotTilesPerRow - 1) - (float)dwX * 2.0f;
        float flDY = (float)(cam.dwHighResScreenshotTilesPerRow - 1) - (float)dwY * 2.0f;

        Matrix4x4 m(flS, 0.0f, 0.0f, 0.0f, 0.0f, flS, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, flDX, flDY, 0.0f, 1.0f);





        cam.mProjectionUnadjusted *= m;
    }

    cam.mProjection = cam.mProjectionUnadjusted;
    cam.mProjection._11 = vecScale.x * cam.mProjection._11;
    cam.mProjection._22 = vecScale.y * cam.mProjection._22;
    if (vecProjectionTranslate != Vector2D(0.0f, 0.0f)) {

        Matrix4x4 m(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, vecProjectionTranslate.x, vecProjectionTranslate.y, 0.0f, 1.0f);




        cam.mProjection *= m;
    }

    float flTanHalfFOV = Math::Tan(aFOV * 0.5f);
    cam.flFarHeight = flTanHalfFOV * flFarPlane;
    cam.flFarWidth = cam.flFarHeight * vecScale.y * flAspectRatio / vecScale.x;
    cam.flNearHeight = flTanHalfFOV * flNearPlane;
    cam.flNearWidth = cam.flNearHeight * vecScale.y * flAspectRatio / vecScale.x;


}




void CameraSetOrthographicProjection(
Camera & cam,
const float flHeight,
const float flAspectRatio,
const float flNearPlane,
const float flFarPlane,
const Vector2D & vecScale,
const Vector2D & vecProjectionTranslate)
{
    GfxBuildOrthographicProjectionMatrix(cam.mProjection, flHeight, flAspectRatio, flNearPlane, flFarPlane, 1);
    cam.mProjection._11 = vecScale.x * cam.mProjection._11;
    cam.mProjection._22 = vecScale.y * cam.mProjection._22;
    if (vecProjectionTranslate != Vector2D(0.0f, 0.0f)) {

        Matrix4x4 m(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, vecProjectionTranslate.x,



                    vecProjectionTranslate.y, 0.0f, 1.0f);
        cam.mProjection *= m;
    }
    cam.mProjectionUnadjusted = cam.mProjection;

    cam.flFarHeight = flHeight * 0.5f;
    cam.flFarWidth = cam.flFarHeight * vecScale.y * flAspectRatio / vecScale.x;
    cam.flNearHeight = cam.flFarHeight;
    cam.flNearWidth = cam.flFarWidth;

}





BOOL CameraComputeAABBVisibility(
    const Camera & cam,
    const AABB & aabb)
{
    return Intersect::FrustumAndAABB(cam.tFrustum, aabb);
}














BOOL CameraComputeSphereVisibility(
    const Camera & cam,
    const Sphere & ws)
{
    return Intersect::FrustumAndSphere(cam.tFrustum, ws);
}




Intersect::DetailedFrustumResult CameraComputeDetailedSphereVisibility(
    const Camera & cam,
    const Sphere & ws)
{
    return Intersect::DetailedFrustumAndSphere(cam.tFrustum, ws);
}





























static void sGetScreenRectForCurrentView(
    IRect2D & rectScreen)
{
    rectScreen.left = 0; rectScreen.top = 0;


    IVector2D vecHolodeckSize = GetRWindowMgr()->GetCurrentRWindow().GetPixelDimensions();
    rectScreen.right = vecHolodeckSize.x; rectScreen.bottom = vecHolodeckSize.y;
}





void CameraScreenToWorldPoint(
    const Camera & cam,
    const IVector2D & p,
    Vector3D & wp)
{
    IRect2D rectScreen;
    sGetScreenRectForCurrentView(rectScreen);

    IRect2D rectViewportScreen;
    cam.cmState.rectViewport.FromUnitCoords(rectScreen, rectViewportScreen);


    uint32 dwWidth = rectViewportScreen.Width();
    uint32 dwHeight = rectViewportScreen.Height();
    if (rectViewportScreen.IsEmpty()) {

        wp = cgwpNull;
        return;
    }

    float flX = (float)(p.x - rectViewportScreen.left) / (float)dwWidth;
    float flY = (float)(p.y - rectViewportScreen.top) / (float)dwHeight;


    wp = cam.wp[FC_FAR_TL] + (cam.wp[FC_FAR_TR] - cam.wp[FC_FAR_TL]) * flX + (cam.wp[FC_FAR_BL] - cam.wp[FC_FAR_TL]) * flY;


}


// Camera.cpp:266-267 inference: wvView rotates cgwvInitCameraForward and wvUp rotates cgwvInitCameraUp (the constant folding of the zero products in the original agrees).

void CameraRebuild(
    Camera & cam)
{

    CameraState & cms = cam.cmState;


    if (cms.bPerspective) {








        CameraSetPerspectiveProjection(
            cam,
            cms.aFOV,
            cms.flAspectRatio,
            cms.flNearPlane,
            cms.flFarPlane,
            cms.vecScale,
            cms.vecProjectionTranslate);
    } else {


        CameraSetOrthographicProjection(
            cam,
            cms.flHeight,
            cms.flAspectRatio,
            cms.flNearPlane,
            cms.flFarPlane,
            cms.vecScale,
            cms.vecProjectionTranslate);
    }



    const Vector3D & wpPos = cms.transform.wp;
    Vector3D wvView = cms.transform.q.Rotate(cgwvInitCameraForward);
    Vector3D wvUp = cms.transform.q.Rotate(cgwvInitCameraUp);

    GfxBuildViewMatrix(cam.mView, wpPos, wvView, wvUp, 1);
    cam.wvCameraForward = wvView;

    Vector3D wvRight = Vector3D::CrossProduct(wvView, wvUp);

    {

        Vector3D wvScaledFarUp = wvUp * cam.flFarHeight;
        Vector3D wvScaledFarRight = wvRight * cam.flFarWidth;
        Vector3D wvScaledNearUp = wvUp * cam.flNearHeight;
        Vector3D wvScaledNearRight = wvRight * cam.flNearWidth;


        Vector3D wpFar = wpPos + wvView * cms.flFarPlane;


        cam.wp[FC_FAR_TL] = wpFar + wvScaledFarUp - wvScaledFarRight;
        cam.wp[FC_FAR_TR] = wpFar + wvScaledFarUp + wvScaledFarRight;
        cam.wp[FC_FAR_BL] = wpFar - wvScaledFarUp - wvScaledFarRight;
        cam.wp[FC_FAR_BR] = wpFar - wvScaledFarUp + wvScaledFarRight;


        cam.wp[FC_NEAR_TL] = wpPos + wvScaledNearUp - wvScaledNearRight;
        cam.wp[FC_NEAR_TR] = wpPos + wvScaledNearUp + wvScaledNearRight;
        cam.wp[FC_NEAR_BL] = wpPos - wvScaledNearUp - wvScaledNearRight;
        cam.wp[FC_NEAR_BR] = wpPos - wvScaledNearUp + wvScaledNearRight;


        cam.tFrustum.Set(cam.wp);
    }







    cam.qBillboard.FromBasisVectors(wvRight, wvUp, -wvView);




}







BOOL CameraWorldToScreenPoint(
    const Camera & cam,
    const Vector3D & wp,
    Vector3D & p)
{
    IRect2D rectScreen;
    sGetScreenRectForCurrentView(rectScreen);

    IRect2D rectViewportScreen;
    cam.cmState.rectViewport.FromUnitCoords(rectScreen, rectViewportScreen);


    Matrix4x4 m = cam.mView * cam.mProjection;
    Vector3D wpResult = wp * m;
    float flW = wp.x * m._14 + wp.y * m._24 + wp.z * m._34 + m._44;


    if (wpResult.z >= 0.0f && wpResult.z <= flW) {



        float flInvW = 1.0f / flW;

        uint32 dwWidth = rectViewportScreen.Width();
        uint32 dwHeight = rectViewportScreen.Height();

        p.x = rectViewportScreen.left + ((1.0f + (wpResult.x * flInvW)) * dwWidth * 0.5f);
        p.y = rectViewportScreen.top + ((1.0f - (wpResult.y * flInvW)) * dwHeight * 0.5f);
        p.z = wpResult.z * flInvW;
        return TRUE;
    }



    p.Set(-1.0f, -1.0f, -1.0f);
    return FALSE;
}












































