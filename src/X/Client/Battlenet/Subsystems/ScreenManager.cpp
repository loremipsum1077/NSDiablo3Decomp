// X/Client/Battlenet/Subsystems/ScreenManager.cpp
// Frontend screen/overlay stacks, modal dialogs and their light boxes.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ScreenManager.h"
#include "LightBoxManager.h"
#include "../Hub.h"
#include "../../UserInterface/UI.h"
#include "../../UserInterface/UIID.h"
#include "../../UserInterface/UIIDs.h"
#include "../../UserInterface/Controls/UIControlControl.h"
#include "../../UserInterface/Controls/UIWindowControl.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "ModalNotificationManager.h"
typedef XGrowableList<UIID, const UIID &> UIIDList;
const UIID & UI_GetKeyboardFocus();  // X/Client/UserInterface/UI.cpp
void UI_SetKeyboardFocus(const UIID & idFocus);  // X/Client/UserInterface/UI.cpp
Hub & HubGet();  // X/Client/Battlenet/Hub.cpp
void UI_PopDialog(UIIDList & listDialogStack, BOOL fShowPrevious);  // X/Client/UserInterface/UI.cpp







































































static void sSetSecondaryDialogVisibility(const UIIDList & listDialogs, BOOL fActive, int32 nHeadOffset)
{
    if (listDialogs.Count() > nHeadOffset)
    {
        POS pos = listDialogs.GetHeadPosition();
        UIID id;

        while (nHeadOffset-- > 0)
        {
            id = listDialogs.GetNext(pos);
        }

        while (pos)
        {
            id = listDialogs.GetNext(pos);
            if (UI_TestID(id))
            {
                UI_GetControlFromID(id)
                    .SetActive(fActive);
            }
        }
    }
}
























template <typename TScreen, int nInvalid, int nNumScreens>
class ScreenManagerImpl {
public:
    ScreenManagerImpl() {}
    ~ScreenManagerImpl() {}


    TScreen GetCurrentScreen() const
    {

        return m_eScreenStack.IsEmpty() ? (TScreen)nInvalid : m_eScreenStack.GetHead();
    }
















































































    void HideCurrentScreen() const
    {
        ToggleCurrentScreenVisibility(FALSE);
    }



    void ShowCurrentScreen() const
    {

        ToggleCurrentScreenVisibility(TRUE);
    }


    BOOL CanGoBack() const
    {

        return m_eScreenStack.Count() > 1;
    }









    void ExitAll()
    {
        BOOL fGoBackSuccessful;
        do
        {
            fGoBackSuccessful = GoBack(FALSE);
        } while (fGoBackSuccessful);


        TScreen eCurrentScreen = GetCurrentScreen();
        if (eCurrentScreen != (TScreen)nInvalid)
        {
            sSetSecondaryDialogVisibility(m_arScreenDialogs[eCurrentScreen], FALSE, 1);
            UI_PopDialog(m_listScreenStack, TRUE);
            m_eScreenStack.RemoveAll();
            m_listScreenStack.RemoveAll();
        }
    }


    BOOL GetCurrentScreenVisibility() const
    {
        TScreen eCurrentScreen = GetCurrentScreen();
        if (eCurrentScreen == (TScreen)nInvalid || m_arScreenDialogs[eCurrentScreen].IsEmpty())
        {
            return FALSE;
        }


        return UI_TestID(m_arScreenDialogs[eCurrentScreen].GetHead()) && UI_GetControlFromID(m_arScreenDialogs[eCurrentScreen].GetHead()).IsActive();
    }








    UIID GetRegisteredDialog(TScreen eScreen) const
    {
        if (eScreen < 0 || m_arScreenDialogs[eScreen].IsEmpty())
        {


            return GetUIIDNone();
        }

        return m_arScreenDialogs[eScreen].GetHead();
    }












    typedef XGrowableList<TScreen, TScreen> ScreenStack;
    ScreenStack m_eScreenStack;  // 0x00
    UIIDList m_arScreenDialogs[nNumScreens];  // 0x58
    UIIDList m_listScreenStack;

    // NON_MATCHING: inlined XBaseList::Find compares tData == node (header) where the original compares node == id, plus nAIndex increment placement.
    BOOL GoBack(
        BOOL fShowPrevious)
    {
        if (CanGoBack())
        {
            TScreen eCurrentScreen = m_eScreenStack.GetHead();

            const UIIDList& currentScreens = m_arScreenDialogs[eCurrentScreen];
            m_eScreenStack.RemoveHead();

            const UIIDList& prevScreens = m_arScreenDialogs[m_eScreenStack.GetHead()];
            UIIDList screenDifference;
            GetSecondaryDialogDifference(currentScreens, prevScreens, screenDifference);



            sSetSecondaryDialogVisibility(screenDifference, FALSE, 0);

            UI_PopDialog(m_listScreenStack, fShowPrevious);

            if (fShowPrevious)
            {
                sSetSecondaryDialogVisibility(prevScreens, TRUE, 1);
            }
            return TRUE;

        }
        return FALSE;
    }





















    void ToggleCurrentScreenVisibility(BOOL fActive) const
    {


        if (GetCurrentScreen() == (TScreen)nInvalid)
        {
            return;
        }

        const UIIDList & listDialogs = m_arScreenDialogs[GetCurrentScreen()];

        if (UI_TestID(listDialogs.GetHead()))
        {
            UI_GetControlFromID(listDialogs.GetHead()).SetActive(fActive);
        }
        sSetSecondaryDialogVisibility(listDialogs, fActive, 1);
    }


    void GetSecondaryDialogDifference(const UIIDList & listA, const UIIDList & listB, UIIDList & listDifference)
    {
        POS Apos = listA.GetHeadPosition();
        int32 nAIndex = 0;
        while (Apos)
        {
            UIID idScreenA = listA.GetNext(Apos);
            if (nAIndex++ > 0 && !listB.Find(idScreenA))
                listDifference.AddTail_Tracked(idScreenA);

        }
    }



};













class MainScreenManager : public ScreenManagerImpl<int, -1, 4> {
};

















class OverlayScreenManager : public ScreenManagerImpl<int, -1, 18> {
};





static const char * sgszScreenNames[] = { "Login", "HeroCreate", "HeroSelect", "Campaign" };





































ScreenManager::ScreenManager()
    : m_ePreviousScreen(-1)
    , m_idHighlightEffect(-1)
    , m_idMousedOverRActor(RACTORID_INVALID)
    , m_idClickedRActor(RACTORID_INVALID)
    , m_idFocusPriorToModalDlg(GetUIIDNone())
    , m_idOverlayLightBox(INVALID_MODAL_NOTIFICATION_ID)
{
    m_pMainScreenManager = new MainScreenManager;
    m_pOverlayScreenManager = new OverlayScreenManager;

    for (int32 i = 0; i < 4; i++)
    {
        UIIDArray arUIIDs;
        POS pos = m_pMainScreenManager->m_arScreenDialogs[i].GetHeadPosition();
        while (pos)
        {
            const UIID & id = m_pMainScreenManager->m_arScreenDialogs[i].GetNext(pos);
            arUIIDs.push_back(id);
        }
        m_tScreenTransitionManager.RegisterScreenDialogs((FrontendScreenTransitionManager::ScreenKey)i, sgszScreenNames[i], arUIIDs);
    }
}


ScreenManager::~ScreenManager()
{
    delete m_pMainScreenManager;


    delete m_pOverlayScreenManager;



}




















































































































































































































































































































































































void ScreenManager::ShowModalDialog(const UIID & idDialog)
{
    if (UI_TestID(idDialog))
    {
        m_idFocusPriorToModalDlg = UI_GetKeyboardFocus();
















        if (UI_GetKeyboardFocus() != UITEXT_CHATENTRY_EDITLINE)
        {



            UI_SetKeyboardFocus(GetUIIDNone());
        }
        UI_GetWindowFromID(idDialog)

            .SetActive(TRUE);
    }
}
void ScreenManager::CloseModalDialog(const UIID & idDialog)
{
    if (UI_TestID(idDialog))
    {

        LightBoxManager & tLightBoxManager = HubGet().LightBoxManagerGet();
        LightBoxId idLightBox;
        if (m_mapModalDialogLightBoxes.Lookup(idDialog, idLightBox))
        {
            tLightBoxManager.RemoveLightBox(idLightBox);
            m_mapModalDialogLightBoxes.RemoveAt(idDialog);
        }
        else
        {
            tLightBoxManager.PopLightBox();
        }

        UI_GetWindowFromID(idDialog).SetActive(FALSE);

        if (UI_GetKeyboardFocus() != UITEXT_CHATENTRY_EDITLINE)
        {



            UIID idKeyboardFocus = GetUIIDNone();

            if (UI_TestID(m_idFocusPriorToModalDlg) && UI_GetControlFromID(m_idFocusPriorToModalDlg).IsActive())
            {

                idKeyboardFocus = m_idFocusPriorToModalDlg;
            }




            else if (GetCurrentOverlay() != -1 && m_pOverlayScreenManager->GetCurrentScreenVisibility())
            {
                UIID idOverlay = m_pOverlayScreenManager->GetRegisteredDialog(GetCurrentOverlay());
                if (UI_TestID(idOverlay))
                {
                    idKeyboardFocus = UI_GetControlFromID(idOverlay).GetInitialKeyboardFocus();
                }
            }
            else if (GetCurrentScreen() != -1 && m_pMainScreenManager->GetCurrentScreenVisibility())
            {
                UIID idScreen = m_pMainScreenManager->GetRegisteredDialog(GetCurrentScreen());
                if (UI_TestID(idScreen))
                {
                    idKeyboardFocus = UI_GetControlFromID(idScreen).GetInitialKeyboardFocus();
                }
            }

            if (UI_TestID(idKeyboardFocus))
            {
                UI_SetKeyboardFocus(idKeyboardFocus);
            }
        }
    }
}


inline UIScreen ScreenManager::GetCurrentScreen() const
{

    return m_pMainScreenManager->GetCurrentScreen();
}
















UIOverlay ScreenManager::GetCurrentOverlay() const
{

    return m_pOverlayScreenManager->GetCurrentScreen();
}









void ScreenManager::ShowCurrentScreen() const
{


    if (GetCurrentScreen() != -1)
    {
        m_pMainScreenManager->ShowCurrentScreen();
    }

}


void ScreenManager::HideCurrentOverlay() const
{

    m_pOverlayScreenManager->HideCurrentScreen();
}












































































































































































































































































































































































void ScreenManager::CloseOverlay()
{

    if (m_pOverlayScreenManager->GetCurrentScreen() != -1)
    {
        HubGet().LightBoxManagerGet().RemoveLightBox(m_idOverlayLightBox);
        m_pOverlayScreenManager->ExitAll();
    }
}
