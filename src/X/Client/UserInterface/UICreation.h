// Hand-written; the reference header lists only the CreateControl instantiations.
// Original file: X/Client/UserInterface/UICreation.h

#pragma once

#include "UIID.h"
#include "../../Sigma/Math/Vector.h"

// line 39. Body written from the copies inlined into UIConsoleListBox::sInit and
// UIConsoleListBox::CreateGridRowStackPanel.
template <class TControl, class TParams>
inline UIID CreateControl(TParams & tParams, const Vector2D vecTL, const Vector2D vecDim)
{
    TControl & tControl = *new TControl(tParams, vecTL, vecDim);
    tControl.PostCreate();
    return UIID(tControl.GetID());
}

// UICreation.h:54; live original instantiation in UIConsoleHUD.cpp:705.
template <class ControlType, class CreationParamsType>
UIID CreateControl( // line 54
    CreationParamsType & tParams, // line 55
    const Rect2D & rect) // line 56
{
    ControlType & tControl = *new ControlType(tParams, &rect); // line 58
    tControl.PostCreate();
    return UIID(tControl.GetID());
}
