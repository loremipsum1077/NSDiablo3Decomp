// Original file: X/Client/Battlenet/Subsystems/ScreenManager.h
// Subset: the ScreenManager class layout (DWARF size 0x498) and the members its callers use.
#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/OnlineService/SubscriptionEvent.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../UserInterface/UIID.h"
#include "FrontendBackgroundCameraManager.h"
#include "FrontendBackgroundHeroManager.h"
#include "FrontendScreenTransitionManager.h"
#include "LightBoxManager.h"

typedef int32 UIScreen;
typedef int32 UIOverlay;
typedef int32 ScreenTransitionState;
typedef int32 EffectGroupID;
class MainScreenManager;
class OverlayScreenManager;

class ScreenManager {  // size 0x498
public:
    ScreenManager();
    ~ScreenManager();
    BOOL GoToScreen(UIScreen eScreen, BOOL bForce, BOOL bAddToStack);
    BOOL GoToOverlay(UIOverlay eOverlay, BOOL bForce);
    UIScreen GetCurrentScreen() const;
    UIOverlay GetCurrentOverlay() const;
    void ShowCurrentScreen() const;
    void HideCurrentOverlay() const;
    void CloseOverlay();
    void ShowModalDialog(const UIID & idDialog);
    void CloseModalDialog(const UIID & idDialog);

    typedef OnlineService::SubscriptionEvent<int> BackgroundBannerEventType;
    typedef OnlineService::SubscriptionEvent<int, int> BackgroundHeroEventType;

    BackgroundBannerEventType m_BackgroundBannerEvent;  // 0x00
    BackgroundHeroEventType m_BackgroundHeroEvent;  // 0x30
    MainScreenManager * m_pMainScreenManager;  // 0x60
    OverlayScreenManager * m_pOverlayScreenManager;  // 0x68
    FrontendBackgroundCameraManager m_tBackgroundCameraManager;  // 0x70
    FrontendBackgroundHeroManager m_tBackgroundHeroManager;  // 0x80
    FrontendScreenTransitionManager m_tScreenTransitionManager;  // 0x350
    UIScreen m_ePreviousScreen;  // 0x3f8
    EffectGroupID m_idHighlightEffect;  // 0x3fc
    RActorID m_idMousedOverRActor;  // 0x400
    RActorID m_idClickedRActor;  // 0x404
    UIID m_idFocusPriorToModalDlg;  // 0x408
    LightBoxId m_idOverlayLightBox;  // 0x410
    XGrowableMap<UIID, const UIID &, unsigned int, unsigned int> m_mapModalDialogLightBoxes;  // 0x418
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(ScreenManager) == 0x498, "ScreenManager");
