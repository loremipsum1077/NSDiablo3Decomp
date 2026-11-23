#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Vector.h"

void RotateBetweenUnitVectors(const Vector3D & wvStart, const Vector3D & wvEnd, const Angle aAngle,
                              Vector3D & wvResult);









void SmoothMovementAngle(const Angle aCurrentSpeed, const Angle aCurrentPosition, const Angle aMaxSpeed,
                         const Angle aMaxAcceleration, const Angle aMaxDeceleration,
                         const Angle aFinalPosition, Angle & aNewPosition) {
    Angle aUpdatedSpeed = aCurrentSpeed;





    Angle aDistanceToEnd = Math::SignedAngularDifference(aCurrentPosition, aFinalPosition);

    if (Math::Abs(aDistanceToEnd) < 0.000001f) {
        aNewPosition = aCurrentPosition;
        return;
    }

    if ((aUpdatedSpeed > 0.0f && aDistanceToEnd < 0.0f) || (aUpdatedSpeed < 0.0f && aDistanceToEnd > 0.0f)) {
        aUpdatedSpeed = 0.0f;
    }

    Angle aDirection = (aDistanceToEnd < 0.0f) ? -1.0f : 1.0f;














    Angle aNeededDeceleration = (aUpdatedSpeed * aUpdatedSpeed) / Math::Abs(2.0f * aDistanceToEnd);
    Angle aNewSpeed = aUpdatedSpeed;


    if (Math::Abs(aNeededDeceleration) > aMaxDeceleration) {
        aNewSpeed = aUpdatedSpeed - aDirection * aNeededDeceleration;



    } else if (Math::Abs(aUpdatedSpeed) <= aMaxSpeed) {
        aNewSpeed = aDirection * aMaxAcceleration + aUpdatedSpeed;
    }







    if (Math::Abs(aDistanceToEnd) <= Math::Abs(aNewSpeed)) {
        aNewPosition = aFinalPosition;
    } else {



        aNewSpeed = Math::Pin(aNewSpeed, -aMaxSpeed, aMaxSpeed);
        aNewPosition = Math::NormalizeAngle(aCurrentPosition + aNewSpeed);
    }
}



void SmoothMovementFloat(const float fCurrentSpeed, const float fCurrentPosition, const float fMaxSpeed,
                         const float fMaxAcceleration, const float fMaxDeceleration,
                         const float fFinalPosition, float & fNewPosition) {
    float fDistanceToEnd = fFinalPosition - fCurrentPosition;

    if (Math::Abs(fDistanceToEnd) < 0.000001f) {
        return;
    }

    float fUpdatedSpeed = fCurrentSpeed;

    if ((fUpdatedSpeed > 0.0f && fDistanceToEnd < 0.0f) || (fUpdatedSpeed < 0.0f && fDistanceToEnd > 0.0f)) {
        fUpdatedSpeed = 0.0f;
    }

    float fDirection = (fDistanceToEnd < 0.0f) ? -1.0f : 1.0f;
    float fNeededDeceleration = (fUpdatedSpeed * fUpdatedSpeed) / Math::Abs(2.0f * fDistanceToEnd);
    float fNewSpeed = fUpdatedSpeed;

    if (Math::Abs(fNeededDeceleration) > fMaxDeceleration) {
        fNewSpeed = fUpdatedSpeed - fDirection * fNeededDeceleration;
    } else if (Math::Abs(fUpdatedSpeed) <= fMaxSpeed) {
        fNewSpeed = fDirection * fMaxAcceleration + fUpdatedSpeed;
    }

    if (Math::Abs(fDistanceToEnd) <= Math::Abs(fNewSpeed)) {
        fNewPosition = fFinalPosition;
    } else {
        fNewSpeed = Math::Pin(fNewSpeed, -fMaxSpeed, fMaxSpeed);
        fNewPosition = fCurrentPosition + fNewSpeed;
    }
}


































void SmoothMovementUnitVector(const Angle aCurrentSpeed, const Vector3D & wvCurrentPosition,
                              const Angle aMaxSpeed, const Angle aMaxAcceleration,
                              const Angle aMaxDeceleration, const Vector3D & wvFinalPosition,
                              Vector3D & wvNewPosition, Angle & aNewSpeed) {
    Angle aAngleBetween = Vector3D::AngleBetween(wvCurrentPosition, wvFinalPosition);
    Angle aCurrentPosition = 0.0f;






    Angle aFinalPosition = Math::AngularDifference(aAngleBetween, aCurrentPosition);

    if (aFinalPosition == 0.0f) {


        wvNewPosition = wvFinalPosition;
        aNewSpeed = 0.0f;
        return;
    }

    SmoothMovementAngle(aCurrentSpeed, aCurrentPosition, aMaxSpeed, aMaxAcceleration, aMaxDeceleration,
                        aFinalPosition, aNewSpeed);
    RotateBetweenUnitVectors(wvCurrentPosition, wvFinalPosition, aNewSpeed, wvNewPosition);
}











void RotateBetweenUnitVectors(const Vector3D & wvStart, const Vector3D & wvEnd, const Angle aAngle,
                              Vector3D & wvResult) {
    Vector3D wvRotation;
    WorldDistance wdMag;
    Quaternion qRotation;

    wvRotation = Vector3D::CrossProduct(wvStart, wvEnd);
    wdMag = wvRotation.Normalize();
    if (wdMag < 0.000001f) {
        return;
    }





    qRotation.SetRotation(wvRotation, aAngle);

    wvResult = qRotation.Rotate(wvStart);
    wvResult.Normalize();
}
