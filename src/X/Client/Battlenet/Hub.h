// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/Hub.h

#pragma once

#include "../../Sigma/SigmaTypes.h"

class ContextMenuManager;
class FanfareManager;
class GameIconManager;
class LightBoxManager;
class ModalNotificationManager;
class NotificationSystem;
namespace OnlineService { class ClientService; }
class ScreenManager;
class SimpleCheckoutManager;
class ToastManager;

class Hub {  // size 0x50
public:
    ~Hub();
    BOOL IsUIInitialized() const;
    ScreenManager & ScreenManagerGet() { return *m_pScreenManager; }
    ToastManager & ToastManagerGet();
    LightBoxManager & LightBoxManagerGet() { return *m_pLightBoxManager; }
    FanfareManager & FanfareManagerGet();
    ModalNotificationManager & ModalNotificationManagerGet() { return *m_pModalNotificationManager; }
    ContextMenuManager & ContextMenuManagerGet();
    OnlineService::ClientService & ClientServiceGet() { return *m_pClientService; }
    NotificationSystem & NotificationSystemGet();
    GameIconManager & GameIconManagerGet();
    SimpleCheckoutManager & SimpleCheckoutManagerGet();
    static BOOL IsInitialized();
    static void InitializeForApp();
    static void DisposeForApp();
    static void InitializeForGame();
    static void DisposeForGame();
    static void InitializeForWorld();
    static void DisposeForWorld();
    static void Process();
private:
    // Not in the DWARF: an empty inline, the members are set by Initialize().
    Hub() {}
    void Initialize();
    ScreenManager * m_pScreenManager;  // 0x00
    ToastManager * m_pToastManager;  // 0x08
    LightBoxManager * m_pLightBoxManager;  // 0x10
    FanfareManager * m_pFanfareManager;  // 0x18
    OnlineService::ClientService * m_pClientService;  // 0x20
    ModalNotificationManager * m_pModalNotificationManager;  // 0x28
    ContextMenuManager * m_pContextMenuManager;  // 0x30
    NotificationSystem * m_pNotificationSystem;  // 0x38
    GameIconManager * m_pGameIconManager;  // 0x40
    SimpleCheckoutManager * m_pSimpleCheckoutManager;  // 0x48
};
static_assert(sizeof(Hub) == 0x50, "Hub");


// Functions defined in this file (bodies are not in the DWARF):
//   line    35  ScreenManager & Hub::ScreenManagerGet()  [inlined into 5 functions]
//   line    47  LightBoxManager & Hub::LightBoxManagerGet()  [inlined into 2 functions]
//   line    59  ModalNotificationManager & Hub::ModalNotificationManagerGet()  [inlined into 3 functions]
//   line    71  OnlineService::ClientService & Hub::ClientServiceGet()  [inlined into 4 functions]
