// Original file: X/Client/UserInterface/Console/ConsoleModalNotificationManager.cpp
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIStackPanelControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../Sound/Sound.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"

extern UIID UIWND_CONSOLE_MODAL_NOTIFICATIONS;
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp:1292
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);  // X/Client/UserInterface/UI.cpp
// X/Client/UserInterface/UI.h:550; src/'s UI.h does not have this template yet.
template <typename T>
inline void UI_DeleteControl(T *& ptControl)
{
    if (ptControl)
    {
        UIID id(ptControl->GetID());
        UI_DeleteControl(id, TRUE);
        ptControl = NULL;
    }
}

// ~ConsoleModalNotificationManager (deleting destructor) is emitted here with the vtable.


//@ 32
void ConsoleModalNotificationManager::InitializeForGame()
{
    if (!m_notificationList.IsEmpty())
    {
        ProcessNotificationList();
    }
}

//@ 44
void ConsoleModalNotificationManager::DisposeForGame()
{
    if (m_currentlyShowingNotificationUIID != GetUIIDNone())
    {
        CloseModalDialog(m_currentlyShowingNotificationUIID);
        UI_DeleteControl(m_currentlyShowingNotificationUIID, TRUE);
        m_currentlyShowingNotificationUIID = GetUIIDNone();
    }
}

//@ 58
void ConsoleModalNotificationManager::RemoveAllNotifications()
{

    for (int32 nControllerIndex = -1; nControllerIndex < 4; nControllerIndex++)
    {
        RemoveAllNotificationsForController(nControllerIndex);
    }
}

//@ 70
void ConsoleModalNotificationManager::RemoveAllNotificationsForController(int32 nControllerIndex)
{
    POS pos = m_notificationList.GetHeadPosition();
    while (pos)
    {
        const NotificationInfo & tNotificationInfo = m_notificationList.GetNext(pos);
        if (tNotificationInfo.tNotificationCreationParams.nControllerIndex == nControllerIndex)
        {
            if (tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickCancelButton)
            {
                tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickCancelButton(UIID(tNotificationInfo.id));
            }

            if (tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose)
            {
                tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose(UIID(tNotificationInfo.id));
            }

            RemoveNotification(tNotificationInfo.id);
        }
    }
}

//@ 97
void ConsoleModalNotificationManager::ConsoleCancelCurrentlyShowingNotification(const int32 nControllerIndex)

{
    ConsoleModalNotification_OnClick_CancelButton(GetUIIDNone(), nControllerIndex);
}

//@ 106
void ConsoleModalNotificationManager::ConsoleModalNotification_OnClick_OkButton(const UIID & id, const int32)


{
    ConsoleModalNotificationManager & modalNotificationManager = ::Console::Screens::GetModalNotificationManager();

    NotificationInfo tNotificationInfo = modalNotificationManager.GetCurrentNotification();
    if (tNotificationInfo.tModalNotificationType == NOTIFICATIONTYPE_MESSAGE_ONLY ||
        tNotificationInfo.tModalNotificationType == NOTIFICATIONTYPE_CANCEL)
    {
        return;
    }


    Blizzard::Time::Millisecond tElapsed = Blizzard::Time::Milliseconds() -
        modalNotificationManager.m_tCurrentStartTime;
    if (tElapsed < tNotificationInfo.tNotificationCreationParams.tAcceptButtonDelay)
    {
        return;
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);



    modalNotificationManager.NotificationClosed();

    if (tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickOkButton)
    {
        tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickOkButton(modalNotificationManager.m_currentlyShowingNotificationUIID);
    }



    if (tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose)
    {
        tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose(modalNotificationManager.m_currentlyShowingNotificationUIID);
    }
}

//@ 149
void ConsoleModalNotificationManager::ConsoleModalNotification_OnClick_CancelButton(const UIID & id, const int32)


{
    ConsoleModalNotificationManager & modalNotificationManager = ::Console::Screens::GetModalNotificationManager();

    NotificationInfo tNotificationInfo = modalNotificationManager.GetCurrentNotification();
    if (tNotificationInfo.tModalNotificationType == NOTIFICATIONTYPE_MESSAGE_ONLY ||
        tNotificationInfo.tModalNotificationType == NOTIFICATIONTYPE_OK)
    {
        return;
    }


    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);



    modalNotificationManager.NotificationClosed();

    if (tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickCancelButton)
    {
        tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickCancelButton(modalNotificationManager.m_currentlyShowingNotificationUIID);
    }



    if (tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose)
    {
        tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose(modalNotificationManager.m_currentlyShowingNotificationUIID);
    }
}

//@ 184
void ConsoleModalNotificationManager::ConsoleModalNotification_OnUpdate(const UIID & id)
{

    ConsoleModalNotificationManager & modalNotificationManager = ::Console::Screens::GetModalNotificationManager();
    if (modalNotificationManager.GetCurrentlyShowingNotification() == INVALID_MODAL_NOTIFICATION_ID)
        return;

    NotificationInfo tNotificationInfo = modalNotificationManager.GetCurrentNotification();


    Blizzard::Time::Millisecond tElapsed = Blizzard::Time::Milliseconds() -
        modalNotificationManager.m_tCurrentStartTime;
    uint8 bAlpha = 255;
    CRefString sOkText(tNotificationInfo.tNotificationCreationParams.uszOkButtonText);
    if (tNotificationInfo.tNotificationCreationParams.tAcceptButtonDelay > tElapsed)
    {
        int32 nRemaining = tNotificationInfo.tNotificationCreationParams.tAcceptButtonDelay - tElapsed;
        CRefString sSeconds = UnicodeText::FormatNumber(nRemaining / 1000 + 1, TRUE, FNS_NORMAL, (XLocale)0);
        sOkText.Format("%s (%s)", tNotificationInfo.tNotificationCreationParams.uszOkButtonText.str(), sSeconds.str());
        bAlpha = 64;
    }
    UIID idOkButton = UIIDCreateFromParent(modalNotificationManager.m_currentlyShowingNotificationUIID, ".Buttons.ButtonList.OkButton");
    UIButton & tOk = UI_GetButtonFromID(idOkButton);
    tOk.SetText(sOkText, FALSE);
    tOk.SetTransparency(bAlpha);
}

//@ 226
UIWindow * ConsoleModalNotificationManager::CreateModalNotification(const ModalNotificationManagerBase::NotificationInfo & tNotificationInfo)
{
    UIWindow & tWindowParent = UI_GetWindowFromID(UIWND_CONSOLE_MODAL_NOTIFICATIONS);

    UIControlSourceKey tTemplateSrcKey(tWindowParent.GetUISrcKey().snoUI, "ConsoleModalNotifications_main.LayoutRoot.Notification");
    UIWindow * ptWindow = (UIWindow *)UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams("ModalNotification", UIWND_CONSOLE_MODAL_NOTIFICATIONS), NULL);



    UIID idContent, idContentFrame, idTitle, idSubTitle, idMessage, idOkButton, idCancelButton;
    idContent.SetFromParent(ptWindow->GetID(), "Content.List");
    idContentFrame.SetFromParent(ptWindow->GetID(), "Content");
    idTitle.SetFromParent(ptWindow->GetID(), "Content.List.Title");
    idSubTitle.SetFromParent(ptWindow->GetID(), "Content.List.SetDungeonTitle");
    idMessage.SetFromParent(ptWindow->GetID(), "Content.List.Message");
    idOkButton.SetFromParent(ptWindow->GetID(), "Buttons.ButtonList.OkButton");
    idCancelButton.SetFromParent(ptWindow->GetID(), "Buttons.ButtonList.Cancel");

    UIWindow & tDialogContentFrame = UI_GetWindowFromID(idContentFrame);
    UIStackPanel & tDialogContent = UI_GetStackPanelFromID(idContent);
    UIText & tTitle = UI_GetTextFromID(idTitle);
    UIText & tSubtitle = UI_GetTextFromID(idSubTitle);
    UIText & tMessage = UI_GetTextFromID(idMessage);
    UIButton & tOk = UI_GetButtonFromID(idOkButton);
    if (tNotificationInfo.tNotificationCreationParams.snoOkButtonSound != SNO_NONE)
    {
        tOk.SetClickSound(tNotificationInfo.tNotificationCreationParams.snoOkButtonSound);
    }
    UIButton & tCancel = UI_GetButtonFromID(idCancelButton);
    if (tNotificationInfo.tNotificationCreationParams.snoCancelButtonSound != SNO_NONE)
    {
        tCancel.SetClickSound(tNotificationInfo.tNotificationCreationParams.snoCancelButtonSound);
    }

    UI_GetWindowFromID(UIIDCreateFromParent(ptWindow->GetID(), ".Content.List.Hourglass")).SetActive(tNotificationInfo.tNotificationCreationParams.bShowBlinker);

    UIJustifyHorizontal eJustify = tNotificationInfo.tNotificationCreationParams.bLeftJustify ? UIJH_LEFT : UIJH_CENTER;





    tMessage.SetJustification(eJustify, UIJV_MIDDLE);



    switch (tNotificationInfo.tModalNotificationType)
    {
    case NOTIFICATIONTYPE_OK:
    {
        tTitle.SetText(tNotificationInfo.tNotificationCreationParams.uszTitle, FALSE);
        tSubtitle.SetActive(tNotificationInfo.tNotificationCreationParams.uszMessageTitle.Length() != 0);
        tSubtitle.SetText(tNotificationInfo.tNotificationCreationParams.uszMessageTitle, FALSE);


        tMessage.SetValidatingHyperlinks(FALSE);
        tMessage.SetText(tNotificationInfo.tNotificationCreationParams.uszMessage, FALSE);
        tOk.SetText(tNotificationInfo.tNotificationCreationParams.uszOkButtonText, FALSE);
        tCancel.SetActive(FALSE);
    }
    break;
    case NOTIFICATIONTYPE_CANCEL:
    {
        tTitle.SetText(tNotificationInfo.tNotificationCreationParams.uszTitle, FALSE);
        tSubtitle.SetActive(tNotificationInfo.tNotificationCreationParams.uszMessageTitle.Length() != 0);
        tSubtitle.SetText(tNotificationInfo.tNotificationCreationParams.uszMessageTitle, FALSE);


        tMessage.SetValidatingHyperlinks(FALSE);
        tMessage.SetText(tNotificationInfo.tNotificationCreationParams.uszMessage, FALSE);
        tCancel.SetText(tNotificationInfo.tNotificationCreationParams.uszCancelButtonText, FALSE);
        tOk.SetActive(FALSE);
    }
    break;
    case NOTIFICATIONTYPE_OKCANCEL:
    {
        tTitle.SetText(tNotificationInfo.tNotificationCreationParams.uszTitle, FALSE);
        tSubtitle.SetActive(tNotificationInfo.tNotificationCreationParams.uszMessageTitle.Length() != 0);
        tSubtitle.SetText(tNotificationInfo.tNotificationCreationParams.uszMessageTitle, FALSE);


        tMessage.SetValidatingHyperlinks(FALSE);
        tMessage.SetText(tNotificationInfo.tNotificationCreationParams.uszMessage, FALSE);
        tOk.SetText(tNotificationInfo.tNotificationCreationParams.uszOkButtonText, FALSE);
        tCancel.SetText(tNotificationInfo.tNotificationCreationParams.uszCancelButtonText, FALSE);
        tCancel.SetActive(TRUE);
    }
    break;
    case NOTIFICATIONTYPE_MESSAGE_ONLY:
    {
        tTitle.SetText(tNotificationInfo.tNotificationCreationParams.uszTitle, FALSE);
        tSubtitle.SetActive(tNotificationInfo.tNotificationCreationParams.uszMessageTitle.Length() != 0);
        tSubtitle.SetText(tNotificationInfo.tNotificationCreationParams.uszMessageTitle, FALSE);


        tMessage.SetValidatingHyperlinks(FALSE);
        tMessage.SetText(tNotificationInfo.tNotificationCreationParams.uszMessage, FALSE);
        tOk.SetActive(FALSE);
        tCancel.SetActive(FALSE);
    }
    break;
    default:

        UI_DeleteControl(ptWindow);
        return NULL;
    }



    Vector2D vecFrameTL, vecFrameDim, vecStackTL, vecStackDim;
    tDialogContent.GetRelativePosition(vecStackTL, vecStackDim);
    tDialogContentFrame.GetRelativePosition(vecFrameTL, vecFrameDim);
    vecFrameDim.y = vecStackDim.y;
    tDialogContentFrame.SetRelativePosition(vecFrameTL, vecFrameDim);

    m_tCurrentStartTime = Blizzard::Time::Milliseconds();






    return ptWindow;
}

//@ 353
void ConsoleModalNotificationManager::ShowModalDialog(const UIID & idDialog)

{
    ::Console::Screens::ShowModalDialog(GetCurrentNotification().tNotificationCreationParams.nControllerIndex, idDialog, TRUE, GetUIIDNone());
}

//@ 361
void ConsoleModalNotificationManager::CloseModalDialog(const UIID & idDialog)
{

    ::Console::Screens::CloseModalDialog(idDialog);
}
