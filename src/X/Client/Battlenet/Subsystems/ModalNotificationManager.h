// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/Subsystems/ModalNotificationManager.h
// Subset: the notification parameter types and ModalNotificationManagerBase's interface;
// members whose types are not adopted yet are kept as _unadopted padding at their offsets.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../UserInterface/UIID.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

class UIWindow;

typedef int32 ModalNotificationId;
extern const ModalNotificationId INVALID_MODAL_NOTIFICATION_ID;  // ModalNotificationManager.cpp:35

enum NotificationLevel {
    NOTIFICATIONLEVEL_NORMAL = 0,
    NOTIFICATIONLEVEL_CRITICAL = 1,
    NUM_NOTIFICATION_LEVEL = 2,
};

typedef OnlineService::Callback<void (const UIID &)> NotificationCallback;

typedef OnlineService::Callback<int (const UIID &, const CRefString &)> NotificationInputTextValidationFunction;

typedef OnlineService::Callback<void (int, const CRefString &)> NotificationOnClickOkButtonWithInputText;

struct NotificationCallbacks {  // size 0x30
    NotificationCallbacks() {}  // line 57; declared in the DWARF without a recorded body, taken to be empty
    NotificationCallback pfnOnClickOkButton;  // 0x00
    NotificationCallback pfnOnClickCancelButton;  // 0x08
    NotificationCallback pfnOnClose;  // 0x10
    NotificationCallback pfnOnOpen;  // 0x18
    NotificationInputTextValidationFunction pfnOnInputTextValidate;  // 0x20
    NotificationOnClickOkButtonWithInputText pfnOnClickOkButtonWithInputText;  // 0x28
};
static_assert(sizeof(NotificationCallbacks) == 0x30, "NotificationCallbacks");

struct NotificationCreationParams {  // size 0xc8
    NotificationCreationParams();
    explicit NotificationCreationParams(CRefString, NotificationLevel);
    NotificationLevel eNotificationLevel;  // 0x00
    int32 nControllerIndex;  // 0x04
    Blizzard::Time::Millisecond tAcceptButtonDelay;  // 0x08
    BOOL bShowBlinker;  // 0x0c
    CRefString uszTitle;  // 0x10
    CRefString uszMessageTitle;  // 0x28
    CRefString uszMessage;  // 0x40
    CRefString uszOkButtonText;  // 0x58
    CRefString uszCancelButtonText;  // 0x70
    SNO snoOkButtonSound;  // 0x88
    SNO snoCancelButtonSound;  // 0x8c
    NotificationCallbacks tCallbacks;  // 0x90
    BOOL bLeftJustify;  // 0xc0
};
static_assert(sizeof(NotificationCreationParams) == 0xc8, "NotificationCreationParams");

enum ModalNotificationType {
    NOTIFICATIONTYPE_MESSAGE_ONLY = 0,
    NOTIFICATIONTYPE_OK = 1,
    NOTIFICATIONTYPE_CANCEL = 2,
    NOTIFICATIONTYPE_OKCANCEL = 3,
    NOTIFICATIONTYPE_INPUTPROMPT_OKCANCEL = 4,
};

class ModalNotificationManagerBase {  // size 0x70
public:
    ModalNotificationManagerBase();
    virtual ~ModalNotificationManagerBase();  // vtable slot 0
    ModalNotificationId SetOkNotification(const NotificationCreationParams & tNotificationParam);
    ModalNotificationId SetCancelNotification(const NotificationCreationParams & tNotificationParam);
    ModalNotificationId SetOkCancelNotification(const NotificationCreationParams & tNotificationParam);
    ModalNotificationId SetMessageOnlyNotification(const NotificationCreationParams & tNotificationParam);
    BOOL IsNotificationShowing() const;
    BOOL IsNotificationShowing(ModalNotificationId id) const;
    ModalNotificationId GetCurrentlyShowingNotification() const;
    void CancelCurrentlyShowingNotification();
    void RemoveNotification(ModalNotificationId id);
    void ChangeMessage(ModalNotificationId id, const CRefString & sMessage);
private:
    ModalNotificationId SetNotification(const NotificationCreationParams & tNotificationParam, ModalNotificationType eModalNotificationType);
    static ModalNotificationId GetNextId();
protected:
    struct NotificationInfo {  // size 0xd8
        // Line 196; inlined into SetNotification, which gives the member lines 197 and 198.
        NotificationInfo()
            : id(GetNextId())
            , tModalNotificationType(NOTIFICATIONTYPE_OK) {}
        ModalNotificationId id;  // 0x00
        NotificationCreationParams tNotificationCreationParams;  // 0x08
        ModalNotificationType tModalNotificationType;  // 0xd0
    };
    virtual UIWindow * CreateModalNotification(const ModalNotificationManagerBase::NotificationInfo &) = 0;  // vtable slot 2
    virtual void ShowModalDialog(const UIID &) = 0;  // vtable slot 3
    virtual void CloseModalDialog(const UIID &) = 0;  // vtable slot 4
    virtual void RemoveAllNotifications();  // vtable slot 5
    void NotificationClosed();
    void ProcessNotificationList();
    void HandleCriticalNotification(const ModalNotificationManagerBase::NotificationInfo & tNotificationInfo);
    const ModalNotificationManagerBase::NotificationInfo & GetCurrentNotification() const;
    typedef XGrowableList<ModalNotificationManagerBase::NotificationInfo, const ModalNotificationManagerBase::NotificationInfo &> NotificationInfoList;
    ModalNotificationManagerBase::NotificationInfoList m_notificationList;  // 0x08
    UIID m_currentlyShowingNotificationUIID;  // 0x60
    Blizzard::Time::Millisecond m_tCurrentStartTime;  // 0x68
};
static_assert(sizeof(ModalNotificationManagerBase) == 0x70, "ModalNotificationManagerBase");

class ModalNotificationManager : public ModalNotificationManagerBase {  // size 0x70
public:
    static void UIBattlenet_ModalNotification_OnClick_OkButton(const UIID &);
    static void UIBattlenet_ModalNotification_OnClick_CancelButton(const UIID &);
    static void UIBattlenet_ModalNotification_OnClose(const UIID &);
    static void UIBattlenet_ModalNotification_OnOpen(const UIID &);
    static void UIBattlenet_ModalNotification_OnChange_InputBox(const UIID &);
    static void UIBattlenet_ModalNotification_OnUpdate(const UIID &);
    void InitializeForGame();
    void DisposeForGame();
private:
    ModalNotificationManager() {}  // line 276
    // Line 277. Defined in the class body: the binary has only the deleting destructor, emitted
    // with the vtable in ModalNotificationManager.cpp.
    virtual ~ModalNotificationManager() {}  // vtable slot 0
    virtual UIWindow * CreateModalNotification(const ModalNotificationManagerBase::NotificationInfo & tNotificationInfo);  // vtable slot 2
    virtual void ShowModalDialog(const UIID & idDialog);  // vtable slot 3
    virtual void CloseModalDialog(const UIID & idDialog);  // vtable slot 4
};
static_assert(sizeof(ModalNotificationManager) == 0x70, "ModalNotificationManager");
