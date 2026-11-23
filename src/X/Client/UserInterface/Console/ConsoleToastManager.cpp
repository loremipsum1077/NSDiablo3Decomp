// Original file: X/Client/UserInterface/Console/ConsoleToastManager.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleToastManager.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../UIInit.h"
#include "../UIStoryboard.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../CGame.h"
#include "../../../Common/Game.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/RefString.h"
// Declared here because their headers are not adopted yet (UI.cpp).
UIStoryboard & UI_GetStoryboardFromID(const UIID & id); void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);








ConsoleToastManager::ConsoleToastManager()
    : ToastManagerBase(1, (float)CONSOLE_TOAST_DURATION_SECONDS, FALSE)
{
}




void ConsoleToastManager::SendToastConsole(LPCSTR szMessage, const uint32 dwToastDurationTicks)
{


    ToastManagerBase::SendToast(szMessage, ImageTextureFrame(), TOAST_MEDIUM, OnlineService::Callback<void ()>(), (float)dwToastDurationTicks);
}



ToastId ConsoleToastManager::SendIndefiniteToastConsole(LPCSTR szMessage)
{

    return ToastManagerBase::SendIndefiniteToast(szMessage, ImageTextureFrame(), TOAST_MEDIUM, OnlineService::Callback<void ()>());
}



const UIID ConsoleToastManager::CreateToastVisual(int32 nToastSlot, ToastManagerBase::Toast * ptToast)
{



    CRefString sName;
    sName.Format("ToastWin_%d", nToastSlot);
    const UIControlSourceKey tSrcKey = UIControlSourceKey::Parse("ConsoleToasts:ConsoleToasts_main.LayoutRoot.ToastWindow", SNO_NONE, NULL, NULL);

    UIWindow * ptToastWin = (UIWindow *)UI_CreateControlFromTemplate(tSrcKey,

        DynamicControlCreationParams(sName, UIDLG_CONSOLE_TOAST_ROOT), NULL);



    const UIID idToastPanel = UIIDCreateFromParent(ptToastWin->GetID(), ".ToastPanel");
    const UIID idContent = UIIDCreateFromParent(idToastPanel, ".Content");
    const UIID idText = UIIDCreateFromParent(idContent, ".ToastTextValue");

    if (!GameIsStartUp())
    {

        Rect2D rectSafeViewport; CGameGetSafeViewportUIC(rectSafeViewport);

        Vector2D vecPanelTL, vecPanelDim; UI_GetWindowFromID(idToastPanel).GetRelativePosition(vecPanelTL, vecPanelDim);

        const float flTop = UI_GetWindowFromID(UIDLG_CONVERSATION).GetWindowRect().top;

        Vector2D vecTL;
        vecTL.y = flTop;
        vecTL.x = rectSafeViewport.left + rectSafeViewport.Width() * 0.5f - vecPanelDim.x * 0.5f;
        ptToastWin->MoveTo(vecTL);
    }

    UIText & tText = UI_GetTextFromID(idText);
    tText.SetText(ptToast->sMessage, FALSE);

    Vector2D vecTextTL, vecTextDim; tText.GetRelativePosition(vecTextTL, vecTextDim);


    UIWindow & tContent = UI_GetWindowFromID(idContent); Vector2D vecContentTL, vecContentDim; tContent.GetRelativePosition(vecContentTL, vecContentDim);
    Vector2D vecWinTL, vecWinDim; ptToastWin->GetRelativePosition(vecWinTL, vecWinDim);
    const float flNewHeight = vecTextTL.y + vecTextDim.y;
    vecWinTL.y -= flNewHeight - vecContentDim.y;
    vecContentDim.y = flNewHeight;
    ptToastWin->SetRelativePosition(vecWinTL, vecWinDim);
    tContent.SetRelativePosition(vecContentTL, vecContentDim);

    UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_TOAST_FADEOUT).Stop();

    UIStoryboard & tFadeIn = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_TOAST_FADEIN);
    tFadeIn.Stop();
    tFadeIn.Play(FALSE, 0.0f);

    return ptToastWin->GetID();
}



void ConsoleToastManager::DisposeToastVisual(ToastManagerBase::Toast * ptToast)
{

    UIID idToastWin = ptToast->idToastWin;

    if (idToastWin != GetUIIDNone())
    {
        UI_DeleteControl(idToastWin, TRUE);
    }

    UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_TOAST_FADEIN).Stop();
    UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_TOAST_FADEOUT).Stop();
}



void ConsoleToastManager::UpdateToastVisual(int32, ToastManagerBase::Toast * ptToast, float flDeltaTime)
{



    UIStoryboard & tFadeIn = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_TOAST_FADEIN);
    UIStoryboard & tFadeOut = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_TOAST_FADEOUT);

    const float flFadeInDuration = tFadeIn.GetDuration(); const float flFadeOutDuration = tFadeOut.GetDuration();
    const float flFadeOutStart = ptToast->flToastDuration - flFadeOutDuration;

    if (flDeltaTime < flFadeInDuration &&
        !tFadeIn.IsPlaying() &&
        ptToast->idToastWin != GetUIIDNone())
    {

        tFadeIn.Play(FALSE, flFadeInDuration);
        tFadeIn.Stop();
    }

    if (flDeltaTime >= flFadeOutStart)
    {
        if (!tFadeOut.IsPlaying())
        {
            tFadeOut.Play(FALSE, flDeltaTime - flFadeOutStart);
        }
    }
    else
    {
        tFadeOut.Stop();
    }
}



void ConsoleToastManager::OutroToastVisual(ToastManagerBase::Toast * ptToast, float flDeltaTime)
{

    ptToast->flToastDuration = UI_GetStoryboardFromID(UISTORYBOARD_CONSOLE_TOAST_FADEOUT).GetDuration() + flDeltaTime;
}
