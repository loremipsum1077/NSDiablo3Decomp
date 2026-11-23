// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/ConsoleModalNotificationManager.h

#pragma once

#include "../../Battlenet/Subsystems/ModalNotificationManager.h"

class ConsoleModalNotificationManager : public ModalNotificationManagerBase {  // size 0x70
public:
    ConsoleModalNotificationManager() {}  // line 24
    // Line 31. Defined in the class body: the binary has only the deleting destructor, emitted with
    // the vtable in ConsoleModalNotificationManager.cpp.
    virtual ~ConsoleModalNotificationManager() {}  // vtable slot 0
    virtual void RemoveAllNotifications();  // vtable slot 5
    void InitializeForGame();
    void DisposeForGame();
    static void ConsoleModalNotification_OnClick_OkButton(const UIID & id, const int32);
    static void ConsoleModalNotification_OnClick_CancelButton(const UIID & id, const int32);
    static void ConsoleModalNotification_OnUpdate(const UIID & id);
    void ConsoleCancelCurrentlyShowingNotification(const int32 nControllerIndex);
    void RemoveAllNotificationsForController(int32 nControllerIndex);
protected:
    virtual UIWindow * CreateModalNotification(const ModalNotificationManagerBase::NotificationInfo & tNotificationInfo);  // vtable slot 2
    virtual void ShowModalDialog(const UIID & idDialog);  // vtable slot 3
    virtual void CloseModalDialog(const UIID & idDialog);  // vtable slot 4
};
static_assert(sizeof(ConsoleModalNotificationManager) == 0x70, "ConsoleModalNotificationManager");
