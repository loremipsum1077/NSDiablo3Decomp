// Original file: X/Client/Battlenet/Subsystems/ModalNotificationManager.cpp
// Local declarations stand in for headers not adopted yet; each names its defining file.
#include "ModalNotificationManager.h"
#include "LightBoxManager.h"
#include "../Hub.h"
#include "../../UserInterface/UI.h"
#include "../../UserInterface/UIID.h"
#include "../../UserInterface/UIInit.h"
#include "../../UserInterface/Controls/UIButtonControl.h"
#include "../../UserInterface/Controls/UIEditControl.h"
#include "../../UserInterface/Controls/UIStackPanelControl.h"
#include "../../UserInterface/Controls/UITextControl.h"
#include "../../UserInterface/Controls/UIWindowControl.h"
#include "../../UserInterface/Console/ConsoleScreenManager.h"
#include "../../Font/FormattedText.h"
#include "../../../Common/Game.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"

extern UIID UIWND_CONSOLE_MODAL_NOTIFICATIONS;  // X/Client/UserInterface/UIIDs.h
extern UIID UIDLG_BATTLENET_MODALNOTIFICATIONS;  // X/Client/UserInterface/UIIDs.h
Hub & HubGet();  // X/Client/Battlenet/Hub.cpp
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp:1292
UIEdit & UI_GetEditFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
void UI_SetKeyboardFocus(const UIID & idFocus);  // X/Client/UserInterface/UI.cpp
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);  // X/Client/UserInterface/UI.cpp
void UIStartup_ClearMenuStack();  // X/Client/UserInterface/Interface/UIStartup.cpp
// X/Client/UserInterface/UI.h:550; src/'s UI.h does not have this template yet.
template <typename T> inline void UI_DeleteControl(T *& ptControl) { if (ptControl) { UIID id(ptControl->GetID()); UI_DeleteControl(id, TRUE); ptControl = NULL; } }



static ModalNotificationId s_nextId;
const ModalNotificationId INVALID_MODAL_NOTIFICATION_ID = -1;





NotificationCreationParams::NotificationCreationParams()
    :
    eNotificationLevel(NOTIFICATIONLEVEL_NORMAL),
    nControllerIndex(-1),
    tAcceptButtonDelay(0),

    bShowBlinker(FALSE),





    uszTitle(StringListFetch("Console:Attention")),
    uszOkButtonText(StringListFetch("Console:AcceptWithButton")),
    uszCancelButtonText(StringListFetch("Console:CancelWithButton")),

    snoOkButtonSound(SNO_NONE),
    snoCancelButtonSound(SNO_NONE),
    bLeftJustify(FALSE)
{
}




NotificationCreationParams::NotificationCreationParams(CRefString szMessage, NotificationLevel eLevel)
    :

    eNotificationLevel(eLevel),
    uszMessage(szMessage),
    nControllerIndex(-1),
    tAcceptButtonDelay(0),

    bShowBlinker(FALSE),





    uszTitle(StringListFetch("Console:Attention")),
    uszOkButtonText(StringListFetch("Console:AcceptWithButton")),
    uszCancelButtonText(StringListFetch("Console:CancelWithButton")),

    snoOkButtonSound(SNO_NONE),
    snoCancelButtonSound(SNO_NONE),
    bLeftJustify(FALSE)
{
}



ModalNotificationManagerBase::ModalNotificationManagerBase()
    : m_currentlyShowingNotificationUIID(GetUIIDNone()), m_tCurrentStartTime(0)
{
}



ModalNotificationManagerBase::~ModalNotificationManagerBase()
{
}



ModalNotificationId ModalNotificationManagerBase::SetNotification(const NotificationCreationParams & tNotificationParam, ModalNotificationType eModalNotificationType)

{


    if (m_currentlyShowingNotificationUIID != GetUIIDNone()
        && GetCurrentNotification().tNotificationCreationParams.eNotificationLevel == NOTIFICATIONLEVEL_CRITICAL)
    {
        return INVALID_MODAL_NOTIFICATION_ID;
    }


    NotificationInfo tNotificationInfo;
    ModalNotificationId id = tNotificationInfo.id;
    tNotificationInfo.tNotificationCreationParams = tNotificationParam;
    tNotificationInfo.tModalNotificationType = eModalNotificationType;
    if (tNotificationInfo.tNotificationCreationParams.eNotificationLevel == NOTIFICATIONLEVEL_CRITICAL)
    {
        HandleCriticalNotification(tNotificationInfo);
    }
    else
    {
        m_notificationList.AddTail_Tracked(tNotificationInfo);
    }

    ProcessNotificationList();


    return id;
}



ModalNotificationId ModalNotificationManagerBase::SetMessageOnlyNotification(const NotificationCreationParams & tNotificationParam)
{














    return SetNotification(tNotificationParam, NOTIFICATIONTYPE_MESSAGE_ONLY);
}




ModalNotificationId ModalNotificationManagerBase::SetOkNotification(const NotificationCreationParams & tNotificationParam)
{














    return SetNotification(tNotificationParam, NOTIFICATIONTYPE_OK);
}




ModalNotificationId ModalNotificationManagerBase::SetCancelNotification(const NotificationCreationParams & tNotificationParam)
{














    return SetNotification(tNotificationParam, NOTIFICATIONTYPE_CANCEL);
}




ModalNotificationId ModalNotificationManagerBase::SetOkCancelNotification(const NotificationCreationParams & tNotificationParam)
{














    return SetNotification(tNotificationParam, NOTIFICATIONTYPE_OKCANCEL);
}


























void ModalNotificationManagerBase::ChangeMessage(ModalNotificationId id, const CRefString & sMessage)


{
    if (GetCurrentlyShowingNotification() == id)
    {
        if (UI_TestID(m_currentlyShowingNotificationUIID))
        {
            UIID idMessage; idMessage.SetFromParent(m_currentlyShowingNotificationUIID, "Content.List.Message");
            UIText & tMessage = UI_GetTextFromID(idMessage);
            tMessage.SetText(sMessage, FALSE);
        }
    }
    else
    {
        POS pos = m_notificationList.GetHeadPosition();
        while (pos)
        {

            NotificationInfo & info = m_notificationList.GetNext(pos);
            if (info.id == id)
            {
                info.tNotificationCreationParams.uszMessage = sMessage;
                break;
            }
        }
    }
}







































BOOL ModalNotificationManagerBase::IsNotificationShowing() const
{
    return m_notificationList.Count() > 0;
}




BOOL ModalNotificationManagerBase::IsNotificationShowing(ModalNotificationId id) const
{

    return m_notificationList.Count() > 0
        && GetCurrentNotification().id == id;
}




ModalNotificationId ModalNotificationManagerBase::GetCurrentlyShowingNotification() const
{
    ModalNotificationId ret = INVALID_MODAL_NOTIFICATION_ID;
    if (m_notificationList.Count() > 0)
    {
        ret = GetCurrentNotification().id;
    }
    return ret;
}




void ModalNotificationManagerBase::CancelCurrentlyShowingNotification()

{
    ModalNotificationManager::UIBattlenet_ModalNotification_OnClick_CancelButton(GetUIIDNone());
}




void ModalNotificationManagerBase::RemoveNotification(ModalNotificationId id)

{
    POS pos = m_notificationList.GetHeadPosition();
    while (pos)
    {
        POS currentPos = pos;

        const NotificationInfo & info = m_notificationList.GetNext(pos);
        if (info.id == id)
        {
            if (GetCurrentNotification().id == id)
            {

                CloseModalDialog(m_currentlyShowingNotificationUIID);
                UI_DeleteControl(m_currentlyShowingNotificationUIID, TRUE);
                m_currentlyShowingNotificationUIID = GetUIIDNone();
                m_notificationList.RemoveHead();
                ProcessNotificationList();
            }
            else
            {

                m_notificationList.RemoveAt(currentPos);
            }
            return;
        }
    }
}




ModalNotificationId ModalNotificationManagerBase::GetNextId()
{
    ModalNotificationId id = s_nextId++;
    if (id == INVALID_MODAL_NOTIFICATION_ID)
    {
        id = s_nextId++;
    }
    return id;
}




void ModalNotificationManagerBase::RemoveAllNotifications()

{
    if (!m_notificationList.IsEmpty())
    {
        UI_DeleteControl(m_currentlyShowingNotificationUIID, TRUE);
        m_notificationList.RemoveAll();
    }

    m_currentlyShowingNotificationUIID = GetUIIDNone();



    UIWindow & twindow = UI_GetWindowFromID(UIWND_CONSOLE_MODAL_NOTIFICATIONS);

    twindow.SetActive(FALSE);
}





void ModalNotificationManagerBase::NotificationClosed()

{
    if (m_notificationList.Count() == 0)
    {
        return;
    }

    NotificationInfo tNotificationInfo = m_notificationList.GetHead();

    if (tNotificationInfo.tNotificationCreationParams.eNotificationLevel == NOTIFICATIONLEVEL_CRITICAL)
    {
        RemoveAllNotifications();





        ::Console::Screens::GetLightBoxManager().ClearAllLightBoxes();
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);




        UIStartup_ClearMenuStack();

        GameStartMenu(400);
    }
    else
    {

        CloseModalDialog(m_currentlyShowingNotificationUIID);
        UI_DeleteControl(m_currentlyShowingNotificationUIID, TRUE);
        m_notificationList.RemoveHead();

        m_currentlyShowingNotificationUIID = GetUIIDNone();

        ProcessNotificationList();
    }
}



void ModalNotificationManagerBase::ProcessNotificationList()

{
    if (!HubGet().IsUIInitialized())
    {
        return;
    }


    if (m_currentlyShowingNotificationUIID != GetUIIDNone())
    {
        return;
    }




    UIWindow & tWindow = UI_GetWindowFromID(UIWND_CONSOLE_MODAL_NOTIFICATIONS);

    if (m_notificationList.Count() > 0)
    {



        tWindow.SetActive(TRUE);

        NotificationInfo tNotificationInfo = m_notificationList.GetHead();
        UIWindow * ptWindow = CreateModalNotification(tNotificationInfo);
        ShowModalDialog(ptWindow->GetID());
        m_currentlyShowingNotificationUIID = ptWindow->GetID();

        if (tNotificationInfo.tModalNotificationType == NOTIFICATIONTYPE_INPUTPROMPT_OKCANCEL)
        {





            UIID idInputBox; idInputBox.SetFromParent(m_currentlyShowingNotificationUIID, "Content.List.InputContainer.InputBox");
            if (UI_TestID(idInputBox))
            {
                UI_SetKeyboardFocus(idInputBox);
            }
        }
    }
    else
    {
        tWindow.SetActive(FALSE);
    }
}




void ModalNotificationManagerBase::HandleCriticalNotification(const ModalNotificationManagerBase::NotificationInfo & tNotificationInfo)

{





    if (m_notificationList.Count() < 2)
    {
        m_notificationList.AddTail_Tracked(tNotificationInfo);
    }
    else
    {




        NotificationInfo tNotificationInfoHead = m_notificationList.GetHead();
        m_notificationList.RemoveHead();
        m_notificationList.AddHead_Tracked(tNotificationInfo);
        m_notificationList.AddHead_Tracked(tNotificationInfoHead);
    }
}


const ModalNotificationManagerBase::NotificationInfo & ModalNotificationManagerBase::GetCurrentNotification() const

{
    return m_notificationList.GetHead();
}




UIWindow * ModalNotificationManager::CreateModalNotification(const ModalNotificationManagerBase::NotificationInfo & tNotificationInfo)

{
    if (!UI_TestID(UIDLG_BATTLENET_MODALNOTIFICATIONS))
        return NULL;
    UIWindow & tWindowParent = UI_GetWindowFromID(UIDLG_BATTLENET_MODALNOTIFICATIONS);

    UIControlSourceKey tTemplateSrcKey(tWindowParent.GetUISrcKey().snoUI, "BattleNetModalNotifications_main.LayoutRoot.Notification");
    UIWindow * ptWindow = (UIWindow *)UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams("ModalNotification", UIDLG_BATTLENET_MODALNOTIFICATIONS), NULL);



    UIID idContent, idContentFrame, idTitle, idSubTitle, idMessage, idOkButton, idCancelButton, idInputContainer;
    idContent.SetFromParent(ptWindow->GetID(), "Content.List");
    idContentFrame.SetFromParent(ptWindow->GetID(), "Content");
    idTitle.SetFromParent(ptWindow->GetID(), "Content.List.Title");
    idSubTitle.SetFromParent(ptWindow->GetID(), "Content.List.SetDungeonTitle");
    idMessage.SetFromParent(ptWindow->GetID(), "Content.List.Message");
    idOkButton.SetFromParent(ptWindow->GetID(), "Buttons.ButtonList.OkButton");
    idCancelButton.SetFromParent(ptWindow->GetID(), "Buttons.ButtonList.Cancel");
    idInputContainer.SetFromParent(ptWindow->GetID(), "Content.List.InputContainer");

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
    if (UI_TestID(idInputContainer))
    {
        UIWindow & tInputContainer = UI_GetWindowFromID(idInputContainer);
        tInputContainer.SetActive(FALSE);
    }

    UIJustifyHorizontal eJustify = tNotificationInfo.tNotificationCreationParams.bLeftJustify ? UIJH_LEFT : UIJH_CENTER;





    tMessage.SetJustification(eJustify, UIJV_MIDDLE);


    switch (tNotificationInfo.tModalNotificationType)
    {
    case NOTIFICATIONTYPE_OK:
    case NOTIFICATIONTYPE_CANCEL:
    {
        tTitle.SetText(tNotificationInfo.tNotificationCreationParams.uszTitle, FALSE);
        tSubtitle.SetActive(tNotificationInfo.tNotificationCreationParams.uszMessageTitle.Length() != 0);
        tSubtitle.SetText(tNotificationInfo.tNotificationCreationParams.uszMessageTitle, FALSE);



        tMessage.SetValidatingHyperlinks(FALSE);
        tMessage.SetText(tNotificationInfo.tNotificationCreationParams.uszMessage, FALSE);
        if (tNotificationInfo.tModalNotificationType == NOTIFICATIONTYPE_OK)
        {
            tOk.SetText(tNotificationInfo.tNotificationCreationParams.uszOkButtonText, FALSE);
        }
        else
        {
            tOk.SetText(tNotificationInfo.tNotificationCreationParams.uszCancelButtonText, FALSE);
        }
        int32 nUIAction = tOk.GetUIAction();
        int32 nUIActionAlt = tCancel.GetUIActionAlt();
        tOk.SetUIAction(nUIAction, nUIActionAlt);


        tCancel.SetActive(FALSE);
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
        tCancel.SetActive(TRUE);
    }
    break;
    case NOTIFICATIONTYPE_INPUTPROMPT_OKCANCEL:
    {
        tTitle.SetText(tNotificationInfo.tNotificationCreationParams.uszTitle, FALSE);
        tSubtitle.SetActive(tNotificationInfo.tNotificationCreationParams.uszMessageTitle.Length() != 0);
        tSubtitle.SetText(tNotificationInfo.tNotificationCreationParams.uszMessageTitle, FALSE);


        tMessage.SetValidatingHyperlinks(FALSE);
        tMessage.SetText(tNotificationInfo.tNotificationCreationParams.uszMessage, FALSE);
        tOk.SetText(tNotificationInfo.tNotificationCreationParams.uszOkButtonText, FALSE);
        tCancel.SetText(tNotificationInfo.tNotificationCreationParams.uszCancelButtonText, FALSE);
        tCancel.SetActive(TRUE);
        if (UI_TestID(idInputContainer))
        {
            UIWindow & tInputContainer = UI_GetWindowFromID(idInputContainer);
            tInputContainer.SetActive(TRUE);
            UIID idInputBox;
            idInputBox.SetFromParent(idInputContainer, "InputBox");
            UIEdit & tInputBox = UI_GetEditFromID(idInputBox);
            tInputBox.SetText("", FALSE);
            if (tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnInputTextValidate)
            {

                CRefString sInputText(tInputBox.GetText().GetText());
                BOOL fIsValid = tNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnInputTextValidate(idInputBox, sInputText);
                tOk.SetEnabled(fIsValid);
            }
        }
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


void ModalNotificationManager::ShowModalDialog(const UIID & idDialog)
{




}



void ModalNotificationManager::CloseModalDialog(const UIID & idDialog)
{




    ::Console::Screens::CloseModalDialog(idDialog);
}

































































void ModalNotificationManager::UIBattlenet_ModalNotification_OnClick_CancelButton(const UIID &)

{
    ModalNotificationManager & modalNotificationManager = HubGet().ModalNotificationManagerGet();

    NotificationInfo tCurrentNotificationInfo = modalNotificationManager.GetCurrentNotification();
    if (tCurrentNotificationInfo.tModalNotificationType == NOTIFICATIONTYPE_MESSAGE_ONLY)
        return;


    if (tCurrentNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickCancelButton)
    {
        tCurrentNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClickCancelButton(modalNotificationManager.m_currentlyShowingNotificationUIID);
    }



    if (tCurrentNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose)
    {
        tCurrentNotificationInfo.tNotificationCreationParams.tCallbacks.pfnOnClose(modalNotificationManager.m_currentlyShowingNotificationUIID);
    }



    if (modalNotificationManager.GetCurrentlyShowingNotification() != INVALID_MODAL_NOTIFICATION_ID
        && tCurrentNotificationInfo.id == modalNotificationManager.GetCurrentlyShowingNotification())
    {
        modalNotificationManager.NotificationClosed();
    }
}
