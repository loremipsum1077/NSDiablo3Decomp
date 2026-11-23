#include "Contrib/Contrib/blz/include/blz/float.h"
#include "X/Client/UserInterface/Controls/UIBlinkerControl.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UIAttributes.h"
#include "../../../Sigma/Math/MathGeneral.h"











const uint32 BLINKER_DURATION_INFINITE = 0xFFFFFFFF;




inline void BlinkerCreationParams::sInit()
{
    dwDuration = 0;
    flMinAlpha = 0.0f;
    flRotSpeed = 0.0f;
    flAlphaSpeed = 0.0f;
}





BlinkerCreationParams::BlinkerCreationParams(const UIType eType, const UIID & id, const UIID & idParent)



    : TextCreationParams(eType, id, idParent)
{
    sInit();
}




BlinkerCreationParams::BlinkerCreationParams(const UIType eType, LPCSTR szName, const UIID & idParent)



    : TextCreationParams(eType, szName, idParent)
{
    sInit();
}




void BlinkerCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{

    if (snoUI == SNO_NONE)
        return;




    if (pControlData->GetBoolValue(ATT_BLINK_FOREVER))

        dwDuration = BLINKER_DURATION_INFINITE;

    flMinAlpha = pControlData->GetFloat32Value(ATT_BLINKER_MIN_ALPHA);
    flRotSpeed = Math::DegToAngle(pControlData->GetFloat32Value(ATT_BLINKER_ROTATION_SPEED));
    flAlphaSpeed = pControlData->GetFloat32Value(ATT_BLINKER_ALPHA_SPEED);

    TextCreationParams::Load(snoUI, pControlData);
}




inline void UIBlinker::sInit(const BlinkerCreationParams & tParams)
{

    SetDuration(tParams.dwDuration);

    m_flMinAlpha = Math::Pin(tParams.flMinAlpha, 0.0f, 1.0f);

    SetRotationSpeed(tParams.flRotSpeed);
    SetAlphaSpeed(tParams.flAlphaSpeed);
}




UIBlinker::UIBlinker(const BlinkerCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)


    : UIText(tParams, vecTL, vecDim)
{

    sInit(tParams);
}




UIBlinker::UIBlinker(const BlinkerCreationParams & tParams, const Rect2D * pRect)


    : UIText(tParams, pRect)
{

    sInit(tParams);
}



UIBlinker::~UIBlinker()
{
}



void UIBlinker::sUpdate()
{

    uint32 dwCurTime = CGameGetRealTicks();
    uint32 dwElapsed = dwCurTime - m_dwStartTime;
    if (m_dwDuration != BLINKER_DURATION_INFINITE && m_dwDuration != 0 && dwElapsed > m_dwDuration)
    {

        m_dwDuration = 0;
    }

    if (m_dwDuration != 0)
    {
        if (m_flMinAlpha < 0.999999f &&
            m_dwAlphaFullCycleTimeInTicks != 0)
        {
            RGBAColor rgbaColor = GetWindowColor();

            float flHalfAlphaRange = (1.0f - m_flMinAlpha) * 0.5f;
            float flOffset = m_flMinAlpha + flHalfAlphaRange;

            uint32 dwTicksRemaining = dwElapsed % m_dwAlphaFullCycleTimeInTicks;

            float flAlpha = flOffset + flHalfAlphaRange * Math::Sin((float)dwTicksRemaining * FULL_CIRCLE / (float)m_dwAlphaFullCycleTimeInTicks - PI / 2.0f);
            rgbaColor.a = (uint8)Math::FloatToInt(flAlpha * 255.0f);

            SetWindowColor(rgbaColor);
        }

        if (Math::Abs(m_flRotSpeed) > EPSILON)
        {

            uint32 dwTicksRemaining = dwElapsed % m_dwRotationFullCycleTimeInTicks;
            Angle aRot = m_flRotSpeed * (float)dwTicksRemaining / 60.0f;
            SetWindowRotation(aRot);
        }
    }
    else
    {

        SetActive(FALSE);
    }
}




void UIBlinker::SetDuration(const uint32 dwDuration)
{

    m_dwStartTime = CGameGetRealTicks();
    m_dwDuration = dwDuration;
}




void UIBlinker::SetAlphaSpeed(const float flBlinksPerSecond)
{

    if (flBlinksPerSecond > EPSILON)

        m_dwAlphaFullCycleTimeInTicks = (uint32)(60.0f / flBlinksPerSecond);

    else



        m_dwAlphaFullCycleTimeInTicks = 1;
}



void UIBlinker::SetRotationSpeed(const Angle flRotSpeed)
{
    m_flRotSpeed = flRotSpeed;
    float flAbsRotSpeed = Math::Abs(flRotSpeed);
    if (flAbsRotSpeed > EPSILON)

        m_dwRotationFullCycleTimeInTicks = (uint32)(FULL_CIRCLE * 60.0f / flAbsRotSpeed);

    else
        m_dwRotationFullCycleTimeInTicks = 1;
}






BOOL UIBlinker::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    switch (msg)
    {
    case UI_INPUT:
    case UI_REGISTER_INPUT_LISTENER:
        return FALSE;

    case UI_UPDATE:
        sUpdate();
        return FALSE;
    }










    return UIText::HandleMessage(msg, dwParam);
}
