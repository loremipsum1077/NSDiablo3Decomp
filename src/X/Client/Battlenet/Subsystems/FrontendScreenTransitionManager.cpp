// Original file: X/Client/Battlenet/Subsystems/FrontendScreenTransitionManager.cpp
// Local declarations stand in for headers not adopted yet; each names its defining file.
#include "FrontendScreenTransitionManager.h"
#include "../../UserInterface/UI.h"
#include "../../UserInterface/UIID.h"
#include "../../UserInterface/UITypes.h"
#include "../../UserInterface/UIStoryboard.h"
#include "../../UserInterface/Controls/UIControlControl.h"
#include "../../UserInterface/Console/ConsoleScreenManager.h"
#include "../../../Sigma/RefString.h"

UIStoryboard & UI_GetStoryboardFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp



















FrontendScreenTransitionManager::ScreenDialogInfo & FrontendScreenTransitionManager::ScreenDialogInfo::operator=(const FrontendScreenTransitionManager::ScreenDialogInfo & rhs)
{

    sScreenName = rhs.sScreenName;
    arScreenDialogs = rhs.arScreenDialogs;
    return *this;
}





FrontendScreenTransitionManager::FrontendScreenTransitionManager()
    : m_idTransitionAnimation(GetUIIDNone())
    , m_eTransitionCurrent(-1)
    , m_eTransitionTarget(-1)
    , m_eTransitionState(TRANSITION_DONE)
{

}




FrontendScreenTransitionManager::~FrontendScreenTransitionManager()
{
}




void FrontendScreenTransitionManager::DoScreenTransition(FrontendScreenTransitionManager::ScreenKey eCurrentScreen, FrontendScreenTransitionManager::ScreenKey eNextScreen, FrontendScreenTransitionManager::TransitionOutCompleteCallback cb)



{
    m_eTransitionCurrent = eCurrentScreen;
    m_eTransitionTarget = eNextScreen;
    UIID idTransitionOut = CheckForScreenTransitionOut(eCurrentScreen, eNextScreen);
    if (UI_TestID(idTransitionOut))
    {
        m_idTransitionAnimation = idTransitionOut;
        m_eTransitionState = TRANSITIONING_OUT;
        m_cbTransitionOutCompleteCallback = cb;
        UI_GetStoryboardFromID(m_idTransitionAnimation).Play(FALSE, 0.0f);

        ::Console::Screens::ShowButtonLegend(FALSE);
    }
    else
    {




        ::Console::Screens::ShowButtonLegend(TRUE);

        UIID idTransitionIn = CheckForScreenTransitionIn(eCurrentScreen, eNextScreen);
        if (UI_TestID(idTransitionIn))
        {
            cb(TRUE, eNextScreen);
            m_idTransitionAnimation = idTransitionIn;
            m_eTransitionState = TRANSITIONING_IN;
            UI_GetStoryboardFromID(m_idTransitionAnimation).Play(FALSE, 0.0f);
        }
        else
        {
            cb(FALSE, eNextScreen);
        }
    }
}











BOOL FrontendScreenTransitionManager::IsTransitioning() const
{

    return m_eTransitionState != TRANSITION_DONE;
}




void FrontendScreenTransitionManager::Update()

{
    if (m_eTransitionState != TRANSITION_DONE
        && UI_TestID(m_idTransitionAnimation)
        && !UI_GetStoryboardFromID(m_idTransitionAnimation).IsPlaying())
    {
        if (m_eTransitionState == TRANSITIONING_IN)
        {
            m_eTransitionState = TRANSITION_DONE;
        }
        else if (m_eTransitionState == TRANSITIONING_OUT)
        {

            m_eTransitionState = TRANSITION_DONE;
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(m_idTransitionAnimation);


            tStoryboard.Play(FALSE, 0.0f);
            tStoryboard.Stop();

            UIID idTransitionIn = CheckForScreenTransitionIn(m_eTransitionCurrent, m_eTransitionTarget);
            BOOL bTransitionInExists = UI_TestID(idTransitionIn);
            if (m_cbTransitionOutCompleteCallback)
            {

                ::Console::Screens::ShowButtonLegend(TRUE);

                m_cbTransitionOutCompleteCallback(bTransitionInExists, m_eTransitionTarget);
            }
            if (bTransitionInExists)
            {
                m_idTransitionAnimation = idTransitionIn;
                m_eTransitionState = TRANSITIONING_IN;
                UI_GetStoryboardFromID(m_idTransitionAnimation).Play(FALSE, 0.0f);
                return;
            }
        }

        m_idTransitionAnimation = GetUIIDNone();
    }
}




void FrontendScreenTransitionManager::RegisterScreenDialogs(FrontendScreenTransitionManager::ScreenKey keyScreen, CRefString sScreenName, const UIIDArray & arScreens)



{
    ScreenDialogInfo tInfo;
    tInfo.sScreenName = sScreenName;
    tInfo.arScreenDialogs = arScreens;
    m_mapScreenDialogs.SetAt_Tracked(keyScreen, tInfo);
}




void FrontendScreenTransitionManager::Reset()

{
    if (m_eTransitionState == TRANSITIONING_IN
        && UI_TestID(m_idTransitionAnimation))
    {
        UI_GetStoryboardFromID(m_idTransitionAnimation).SkipToEnd();
    }
    else if (m_eTransitionState == TRANSITIONING_OUT
        && UI_TestID(m_idTransitionAnimation))
    {
        UI_GetStoryboardFromID(m_idTransitionAnimation).Play(FALSE, 0.0f);
        UI_GetStoryboardFromID(m_idTransitionAnimation).Stop();
    }

    m_idTransitionAnimation = GetUIIDNone();
    m_eTransitionState = TRANSITION_DONE;
}




UIID FrontendScreenTransitionManager::CheckForScreenTransitionOut(FrontendScreenTransitionManager::ScreenKey eCurrentScreen, FrontendScreenTransitionManager::ScreenKey eNextScreen) const


{
    ScreenDialogInfo tCurrentScreenInfo, tNextScreenInfo;
    if (!m_mapScreenDialogs.Lookup(eCurrentScreen, tCurrentScreenInfo))
    {
        return GetUIIDNone();
    }

    UIID idTransitionOut = GetUIIDNone();
    if (m_mapScreenDialogs.Lookup(eNextScreen, tNextScreenInfo))
    {

        for (uint32 i = 0; i < tCurrentScreenInfo.arScreenDialogs.size(); i++)
        {
            const UIID & idScreen = tCurrentScreenInfo.arScreenDialogs[i];
            UIID idTransition = UIIDCreateFromParent(idScreen, CRefString().Format(".OnUnloadTo%s", tNextScreenInfo.sScreenName.str()));
            if (UI_TestID(idTransition) && UI_GetControlFromID(idTransition).IsType(UIT_STORYBOARD))
            {
                idTransitionOut = idTransition;
                break;
            }
        }
    }


    if (!UI_TestID(idTransitionOut))
    {
        for (uint32 i = 0; i < tCurrentScreenInfo.arScreenDialogs.size(); i++)
        {
            const UIID & idScreen = tCurrentScreenInfo.arScreenDialogs[i];
            UIID idTransition = UIIDCreateFromParent(idScreen, ".OnUnload");
            if (UI_TestID(idTransition) && UI_GetControlFromID(idTransition).IsType(UIT_STORYBOARD))
            {
                idTransitionOut = idTransition;
                break;
            }
        }
    }


    return idTransitionOut;
}



UIID FrontendScreenTransitionManager::CheckForScreenTransitionIn(FrontendScreenTransitionManager::ScreenKey ePreviousScreen, FrontendScreenTransitionManager::ScreenKey eCurrentScreen) const


{
    ScreenDialogInfo tPreviousScreenInfo, tCurrentScreenInfo;
    if (!m_mapScreenDialogs.Lookup(eCurrentScreen, tCurrentScreenInfo))
    {
        return GetUIIDNone();
    }

    UIID idTransitionIn = GetUIIDNone();
    if (m_mapScreenDialogs.Lookup(ePreviousScreen, tPreviousScreenInfo))
    {

        for (uint32 i = 0; i < tCurrentScreenInfo.arScreenDialogs.size(); i++)
        {
            const UIID & idScreen = tCurrentScreenInfo.arScreenDialogs[i];
            UIID idTransition = UIIDCreateFromParent(idScreen, CRefString().Format(".OnLoadFrom%s", tPreviousScreenInfo.sScreenName.str()));
            if (UI_TestID(idTransition) && UI_GetControlFromID(idTransition).IsType(UIT_STORYBOARD))
            {
                idTransitionIn = idTransition;
                break;
            }
        }
    }


    if (!UI_TestID(idTransitionIn))
    {
        for (uint32 i = 0; i < tCurrentScreenInfo.arScreenDialogs.size(); i++)
        {
            const UIID & idScreen = tCurrentScreenInfo.arScreenDialogs[i];
            UIID idTransition = UIIDCreateFromParent(idScreen, ".OnLoad");
            if (UI_TestID(idTransition) && UI_GetControlFromID(idTransition).IsType(UIT_STORYBOARD))
            {
                idTransitionIn = idTransition;
                break;
            }
        }
    }


    return idTransitionIn;
}
