// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UITutorials.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UIID.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/Math/Rect.h"

namespace D3 { namespace Account { class SavedDefinition; } }
namespace D3 { namespace GameMessage { class TutorialMessage; } }
struct Player;
class UITutorial;
typedef int32 SNO;  // X/Common/SNO/SNOFiles.h

struct RecurringTutorialData {  // size 0x28
    int32 nPlayerIndex;  // 0x00
    UIID idParent;  // 0x08
    Rect2D rectParent;  // 0x10
    uint32 dwRecurTime;  // 0x20
    int32 nOccurrencesLeftUntilLearned;  // 0x24
};
static_assert(sizeof(RecurringTutorialData) == 0x28, "RecurringTutorialData");

enum {
    IGNORE_FREESTANDING_TUTORIALS = 1,
    IGNORE_ANCHORED_TUTORIALS = 2,
    IGNORE_CHATBALLOON_TUTORIALS = 4,
};

class UITutorialManager {  // size 0xa8
public:
    UITutorialManager();
    void InitializeForGame();
    void DisposeForGame();
    void ClearTrackedTutorials();
    void ClearTrackedTutorial(const SNO);
    UITutorial * ShowTutorial(const UIID & idDesiredParent, const SNO snoTutorial, const Player * ptPlayer, const BOOL fIgnoreIfAlreadyBeingTracked);
    UITutorial * ShowTutorial(const Rect2D & parentRect, const SNO snoTutorial, const Player * ptPlayer, const BOOL fIgnoreIfAlreadyBeingTracked);
    UITutorial * ShowTutorial(const RActorID idRActor, const SNO snoTutorial, const Player * ptPlayer, const BOOL fIgnoreIfAlreadyBeingTracked);
    UITutorial * ShowTutorial(const UIID & idDesiredParent, const SNO snoTutorial, D3::Account::SavedDefinition & tAccountDefinition);
    void QueueTutorial(const UIID & idParent, const SNO snoTutorial, uint32 dwDelayInTicks, int32 nPlayerIndex);
    BOOL CloseTutorialIfOpen(const SNO snoTutorial);
    void Update();
    static void Close(const UIID &);
    static void OnButtonClose(const UIID & id, const int32 nControllerIndex);
    void ShowTutorialFromMessage(const D3::GameMessage::TutorialMessage & tMessage);
    BOOL IsAnyShowing(uint32 dwIgnoreMask) const;
    void PerformedDesiredTutorialBehavior(const SNO snoTutorial);
private:
    void sPositionNewTutorial(UITutorial &, const SNO);
    void sHandleMissingParents();
    BOOL sWasPlayed(const SNO snoTutorial, const Player * ptPlayer);
    void sMarkAsPlayed(const SNO snoTutorial, Player * ptPlayer);
    void sUpdateRecurringTutorials();
    BOOL sHandleShowingTutorial(const Player * ptPlayer, const SNO snoTutorial, const UIID & idParent, const Rect2D & rectParent, const BOOL fIgnoreIfAlreadyBeingTracked, BOOL & fPlaySound);
    XGrowableMap<int, int, RecurringTutorialData, RecurringTutorialData &> m_mapRecurringTutorials;  // 0x00
    BOOL m_bInitialized;  // 0x80
    UITutorial * m_ptTutorial;  // 0x88
    SNO m_snoQueuedTutorial;  // 0x90
    uint32 m_dwQueuedDelay;  // 0x94
    UIID m_idQueuedParent;  // 0x98
    uint32 m_dwTickToDisplay;  // 0xa0
    int32 m_nPlayerIndex;  // 0xa4
};
static_assert(sizeof(UITutorialManager) == 0xa8, "UITutorialManager");


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(RecurringTutorialData) == 0x28, "RecurringTutorialData");
static_assert(offsetof(RecurringTutorialData, nPlayerIndex) == 0x0, "RecurringTutorialData");
static_assert(offsetof(RecurringTutorialData, idParent) == 0x8, "RecurringTutorialData");
static_assert(offsetof(RecurringTutorialData, rectParent) == 0x10, "RecurringTutorialData");
static_assert(offsetof(RecurringTutorialData, dwRecurTime) == 0x20, "RecurringTutorialData");
static_assert(offsetof(RecurringTutorialData, nOccurrencesLeftUntilLearned) == 0x24, "RecurringTutorialData");
static_assert(sizeof(UITutorialManager) == 0xa8, "UITutorialManager");
