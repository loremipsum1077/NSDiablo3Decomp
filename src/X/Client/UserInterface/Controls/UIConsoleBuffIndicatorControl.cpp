#include "UIConsoleBuffIndicatorControl.h"

#include "../../Definitions/UIDialogDefinition.h"
#include "../UIAttributes.h"

ConsoleBuffIndicatorCreationParams::ConsoleBuffIndicatorCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}

void ConsoleBuffIndicatorCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    WindowCreationParams::Load(snoUI, pControlData);

    pControlData->GetImageAttribute(ATT_IMAGEFRAME, tTextureFrame);
}

inline void ConsoleBuffIndicatorCreationParams::sInit()
{
    tTextureFrame = cgtImageTextureFrameNull;
}

UIConsoleBuffIndicator::UIConsoleBuffIndicator(const ConsoleBuffIndicatorCreationParams & tParams, const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{
    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeCoordsFromCreationParams(vecTL, vecDim, pRect, tParams);
    sInit(tParams, vecTL, vecDim);
}

inline void UIConsoleBuffIndicator::sInit(const ConsoleBuffIndicatorCreationParams & tParams, const Vector2D &, const Vector2D &)
{
    m_idTimer = GetUIIDNone();
    m_idText = GetUIIDNone();
    m_snoPower = -1;
    m_nStackCount = 0;
}

void UIConsoleBuffIndicator::SetBuffData(SNO snoPower, int32 nStackCount)
{
    m_snoPower = snoPower;
    m_nStackCount = nStackCount;
}
