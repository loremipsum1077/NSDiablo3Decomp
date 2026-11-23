// X/Client/UserInterface/Controls/UIFloatBubbleControl.cpp

#include "UIFloatBubbleControl.h"

void UIFloatBubble::sInit()
{
    SetIgnoresClipping(TRUE);
    SetResizeToFit(RESIZETOFIT_VERTICAL_ONLY);
}

UIFloatBubble::UIFloatBubble(const TextCreationParams & tParams, const Rect2D * pRect)
    : UIText(tParams, pRect)
{
    sInit();
}

void UIFloatBubble::Draw(const uint8 bLayerAlpha, float & flZSlice)
{
    flZSlice += 1.0f;
    UIText::Draw(bLayerAlpha, flZSlice);
}
