// X/Client/Camera/Listener.cpp
//
// 2.6.2 translation unit.  DWARF records two functions:
//   X/Client/Camera/Listener.cpp:14  ListenerSetLocationFromTarget  0x71001a43f0  476 bytes
//   X/Client/Camera/Listener.cpp:38  ListenerSetMicrophoneLocation  0x71001a45d0  464 bytes
//
// Both bodies are pure inlining fronts: every helper they use is defined in its DWARF
// owning file and is only reached here through the includes below.
//   Vector3D::operator*(const Vector3D&, float)      X/Sigma/Math/Vector.inl:578  (line 22)
//   Quaternion::Rotate(const Vector3D&)              X/Sigma/Math/Quaternion.inl:144 (lines 25, 44, 45)
//   Vector3D::Distance(const Vector3D&, const Vector3D&) X/Sigma/Math/Vector.inl:665 (line 26)
//     Math::Sqrt(float)                              X/Sigma/Math/MathGeneral.inl:513 (Vector.inl:674)
//   Math::Min<float>(float, float)                   X/Sigma/Math/MathGeneral.inl:66 (line 27)
//   Vector3D::operator+(const Vector3D&)             X/Sigma/Math/Vector.inl:535  (lines 29 x2, 30)
//     Vector3D::Vector3D(float, float, float)        X/Sigma/Math/Vector.h:196     (Vector.inl:538)
// None of them is redefined here.
//
// The rotations use the return-by-value overload named by the DWARF inlining records
// (_ZNK10Quaternion6RotateERK8Vector3D), not an out-parameter overload.

#include "X/Client/Sound/Sound.h"
#include "X/Sigma/Math/MathGeneral.h"
#include "X/Sigma/Math/Transform.h"
#include "X/Sigma/Math/Vector.h"

// X/Client/Camera/Listener.cpp:14  (0x71001a43f0, 476 bytes)
void ListenerSetLocationFromTarget(PRTransform & transformMic, const Vector3D & wpLookAt,
                                   const WorldDistance wdUpComponent,
                                   const WorldDistance wdViewComponent,
                                   const PRTransform & transform) {
    Vector3D wvUp = cgwvZAxis * wdUpComponent;  // line 22  (Vector.inl:578)
    Vector3D wvView;

    wvView = transform.q.Rotate(cgwvZAxis);  // line 25  (Quaternion.inl:144)
    WorldDistance wdCamToTarget = Vector3D::Distance(transform.wp, wpLookAt) - 1.0f;  // line 26
    WorldDistance wdViewComponentToUse = Math::Min(wdViewComponent, wdCamToTarget);    // line 27

    Vector3D wpLookFrom = transform.wp + wvView * wdViewComponentToUse + wvUp;  // line 29
    Vector3D wpLookAtToUse = wpLookAt + wvUp;                                   // line 30

    transformMic.Set(wpLookFrom, wpLookAtToUse, cgwvZAxis);  // line 32
}

// X/Client/Camera/Listener.cpp:38  (0x71001a45d0, 464 bytes)
void ListenerSetMicrophoneLocation(const PRTransform & volumeTransform,
                                   const PRTransform & panningTransform,
                                   const float flRolloffFactor, const float flDopplerFactor) {
    Vector3D wvView;
    Vector3D wvUp;

    wvView = panningTransform.q.Rotate(cgwvZAxis);  // line 44  (Quaternion.inl:144; GOT 0x71010e55d8 -> cgwvZAxis)
    wvUp = panningTransform.q.Rotate(cgwvXAxis);    // line 45  (Quaternion.inl:144; GOT 0x71010e56c0 -> cgwvXAxis)

    SoundManager::ListenerSet(volumeTransform.wp, panningTransform.wp, wvView, wvUp);  // line 47
    SoundManager::ListenerSetParams(flRolloffFactor, flDopplerFactor);                 // line 49
}
