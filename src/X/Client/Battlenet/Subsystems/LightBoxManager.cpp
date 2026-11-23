// Original file: X/Client/Battlenet/Subsystems/LightBoxManager.cpp
#include "LightBoxManager.h"
#include "../../UserInterface/UI.h"
#include "../../UserInterface/UIInit.h"
#include "../../UserInterface/Controls/UIControlGroup.h"
#include "../../UserInterface/Controls/UIWindowControl.h"

void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);  // X/Client/UserInterface/UI.cpp

//@ 22
LightBoxManager::LightBoxManager()

    : m_uNextId(0)
{



    m_keyLightBox = UIControlSourceKey::Parse("ConsoleModalNotifications:ConsoleModalNotifications_main.LayoutRoot.LightBox", SNO_NONE, NULL, NULL);

}

//@ 36
LightBoxManager::~LightBoxManager()

{

}

//@ 66
UIID LightBoxManager::CreateLightBoxForWindow(const UIID & idWindow, BOOL bBlockInput)


{
    if (!UI_TestID(idWindow)) return GetUIIDNone();
    UIWindow & tWindow = UI_GetWindowFromID(idWindow);
    if (!UI_TestID(tWindow.GetParent())) return GetUIIDNone();
    UIWindow & tParent = UI_GetWindowFromID(tWindow.GetParent());
    UIID idTargetLightBoxId;
    idTargetLightBoxId.SetFromParent(tParent.GetID(), "LightBox");


    if (!UI_TestID(idTargetLightBoxId))
    {
        DynamicControlCreationParams tCreationParams("LightBox", tParent.GetID());
        UIControl * ptLightBoxControl = UI_CreateControlFromTemplate(m_keyLightBox, tCreationParams, NULL);

        idTargetLightBoxId = ptLightBoxControl->GetID();
    }
    UI_GetWindowFromID(idTargetLightBoxId).SetEatsClicks((EatsClicksValue)(bBlockInput != 0));


    tParent.RemoveChild(idTargetLightBoxId);
    int32 nInsertIndex = -1;
    for (int32 i = 0; (int32)tParent.GetChildList().size(); i++)  // sic: the original tests only the count, never i
    {
        if (tParent.GetChildList()[i] == UIControlItem(idWindow))
        {
            nInsertIndex = i;
            break;
        }
    }


    if (nInsertIndex != -1)
    {
        tParent.InsertChild(idTargetLightBoxId, nInsertIndex);
        return idTargetLightBoxId;
    }


    UI_DeleteControl(idTargetLightBoxId, TRUE);
    return GetUIIDNone();
}

//@ 114
LightBoxId LightBoxManager::PushLightBox(const UIID & idLightBoxWindow)

{
    ShowTopLightBox(FALSE);

    if (UI_TestID(idLightBoxWindow))
    {
        UI_GetWindowFromID(idLightBoxWindow).SetActive(TRUE);
    }

    LightBoxStackEntry tEntry;
    tEntry.idLightBox = idLightBoxWindow;
    tEntry.uId = GetNextLightBoxId();
    m_lightBoxStack.AddTail_Tracked(tEntry);
    return tEntry.uId;
}

//@ 134
UIID LightBoxManager::PopLightBox()

{
    UIID idTail = GetUIIDNone();

    ShowTopLightBox(FALSE);

    if (!m_lightBoxStack.IsEmpty())
    {
        idTail = m_lightBoxStack.GetTail().idLightBox;
        m_lightBoxStack.RemoveTail();
    }

    ShowTopLightBox(TRUE);
    return idTail;
}

//@ 154
void LightBoxManager::RemoveLightBox(LightBoxId idLightBox)

{
    if (m_lightBoxStack.Count() > 0)
    {
        POS pos = m_lightBoxStack.GetHeadPosition();
        while (pos)
        {
            POS originalPOS = pos;
            const LightBoxStackEntry & tEntry = m_lightBoxStack.GetNext(pos);
            if (tEntry.uId == idLightBox)
            {
                BOOL bIsTopLightBox = (originalPOS == m_lightBoxStack.GetTailPosition());
                if (bIsTopLightBox)
                {
                    ShowTopLightBox(FALSE);
                }
                m_lightBoxStack.RemoveAt(originalPOS);
                if (bIsTopLightBox)
                {
                    ShowTopLightBox(TRUE);
                }
                break;
            }
        }
    }
}

//@ 202
void LightBoxManager::ClearAllLightBoxes()

{
    POS pos = m_lightBoxStack.GetTailPosition();
    while (pos)
    {
        const UIID & idLightBoxWindow = m_lightBoxStack.GetPrev(pos).idLightBox;
        if (UI_TestID(idLightBoxWindow))
        {
            UI_GetWindowFromID(idLightBoxWindow).SetActive(FALSE);
        }
    }

    m_lightBoxStack.RemoveAll();
}

//@ 221
void LightBoxManager::ShowTopLightBox(BOOL fShow)

{
    if (!m_lightBoxStack.IsEmpty())
    {
        const UIID & idLightBoxWindow = m_lightBoxStack.GetTail().idLightBox;
        if (UI_TestID(idLightBoxWindow))
        {
            UI_GetWindowFromID(idLightBoxWindow).SetActive(fShow);
        }
    }
}
