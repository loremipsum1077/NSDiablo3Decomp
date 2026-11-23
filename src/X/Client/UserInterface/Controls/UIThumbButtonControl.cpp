// Recovered from the original DWARF and instructions; see the DWARF function list
// for each definition's original line.

#include "UIThumbButtonControl.h"

#include "../UI.h"
#include "UIScrollbarControl.h"












UIThumbButton::UIThumbButton(const ButtonCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)



    : UIButton(tParams, vecTL, vecDim)
{
}




UIThumbButton::UIThumbButton(const ButtonCreationParams & tParams, const Rect2D * pRect)


    : UIButton(tParams, pRect)
{
}






































































































BOOL UIThumbButton::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{





















    return UIWindow::HandleMessage(msg, dwParam);
}





UIResult UIThumbButton::sOnDrag(const UIID & id)

{

    const UIID & idParent = UI_GetControlFromID(id).GetParent();
    if (UI_GetControlFromID(idParent).IsType(UIT_SCROLLBAR)) {

        UIScrollbar & tScrollbar = UI_GetScrollbarFromID(idParent);
        tScrollbar.OnDrag();
    }
    return TRUE;
}




UIID UIThumbButton::Create(ButtonCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim, const BOOL fForVerticalScrollbar)




{


    const uint32 dwConstrain = fForVerticalScrollbar ? (1 << UIWINDOW_DRAG_CONSTRAIN_HORIZONTAL_BIT) : (1 << UIWINDOW_DRAG_CONSTRAIN_VERTICAL_BIT);







    tParams.dwFlags |= (1 << UIWINDOW_DRAGGABLE_BIT) | dwConstrain;
    tParams.pfnOnDrag = sOnDrag;

    UIThumbButton & tButton = *new UIThumbButton(tParams, vecTL, vecDim);
    tButton.PostCreate();

    return tButton.GetID();
}
