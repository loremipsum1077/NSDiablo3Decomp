#include "AVViewController.h"

// Functions sit on their original lines (AVViewController.cpp:30, 40, 135).

























//-------------------------------------------------------------------------------------------------
AVViewController::AVViewController() {
    Reset();
}






//-------------------------------------------------------------------------------------------------
void AVViewController::Reset() {
    m_flTForward = 0.0f;
    m_flTBackward = 0.0f;
    m_flTStrafeLeft = 0.0f;
    m_flTStrafeRight = 0.0f;
    m_flTUp = 0.0f;
    m_flTDown = 0.0f;
    m_flTPitchUp = 0.0f;
    m_flTPitchDown = 0.0f;
    m_flTYawLeft = 0.0f;
    m_flTYawRight = 0.0f;
    m_flTRollLeft = 0.0f;
    m_flTRollRight = 0.0f;
    m_flTTurretLeft = 0.0f;
    m_flTTurretRight = 0.0f;
    m_flTFOVUp = 0.0f;
    m_flTFOVDown = 0.0f;
    m_aTurret = 0.0f;
}











































































//-------------------------------------------------------------------------------------------------
void AVViewController::Update() {
}
