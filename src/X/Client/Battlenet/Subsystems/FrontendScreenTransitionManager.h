// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/Subsystems/FrontendScreenTransitionManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../UserInterface/Controls/UIControlGroup.h"
#include "../../UserInterface/UIID.h"
#include "../../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/RefString.h"

enum {
    TRANSITIONING_OUT = 0,
    TRANSITIONING_IN = 1,
    TRANSITION_DONE = 2,
};

typedef int32 ScreenTransitionState;

class FrontendScreenTransitionManager {  // size 0xa8
    // vtable pointer
public:
    FrontendScreenTransitionManager();
    virtual ~FrontendScreenTransitionManager();  // vtable slot 0
    typedef int32 ScreenKey;
    typedef OnlineService::Callback<void (int, int)> TransitionOutCompleteCallback;
    void DoScreenTransition(FrontendScreenTransitionManager::ScreenKey eCurrentScreen, FrontendScreenTransitionManager::ScreenKey eNextScreen, FrontendScreenTransitionManager::TransitionOutCompleteCallback cb);
    ScreenTransitionState GetTransitionState() const;
    BOOL IsTransitioning() const;
    void Update();
    void RegisterScreenDialogs(FrontendScreenTransitionManager::ScreenKey keyScreen, CRefString sScreenName, const UIIDArray & arScreens);
    UIID CheckForScreenTransitionOut(FrontendScreenTransitionManager::ScreenKey eCurrentScreen, FrontendScreenTransitionManager::ScreenKey eNextScreen) const;
    UIID CheckForScreenTransitionIn(FrontendScreenTransitionManager::ScreenKey ePreviousScreen, FrontendScreenTransitionManager::ScreenKey eCurrentScreen) const;
    void Reset();
    struct ScreenDialogInfo {  // size 0x30
        ScreenDialogInfo() {}  // line 68; body from the copies inlined into RegisterScreenDialogs and CheckForScreenTransitionOut/In
        ScreenDialogInfo(const FrontendScreenTransitionManager::ScreenDialogInfo &);
        FrontendScreenTransitionManager::ScreenDialogInfo & operator=(const FrontendScreenTransitionManager::ScreenDialogInfo & rhs);
        CRefString sScreenName;  // 0x00
        UIIDArray arScreenDialogs;  // 0x18
    };
private:
    XGrowableMap<int, int, FrontendScreenTransitionManager::ScreenDialogInfo, const FrontendScreenTransitionManager::ScreenDialogInfo &> m_mapScreenDialogs;  // 0x08
    UIID m_idTransitionAnimation;  // 0x88
    FrontendScreenTransitionManager::ScreenKey m_eTransitionCurrent;  // 0x90
    FrontendScreenTransitionManager::ScreenKey m_eTransitionTarget;  // 0x94
    ScreenTransitionState m_eTransitionState;  // 0x98
    FrontendScreenTransitionManager::TransitionOutCompleteCallback m_cbTransitionOutCompleteCallback;  // 0xa0
};
static_assert(sizeof(FrontendScreenTransitionManager) == 0xa8, "FrontendScreenTransitionManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    66  FrontendScreenTransitionManager::ScreenDialogInfo::~ScreenDialogInfo()  [inlined into 5 functions]
//   line    68  FrontendScreenTransitionManager::ScreenDialogInfo::ScreenDialogInfo()  [inlined into 5 functions]
