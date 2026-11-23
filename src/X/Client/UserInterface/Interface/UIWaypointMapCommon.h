// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIWaypointMapCommon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/Act.h"
#include "../../../Common/SNO/SNOFiles.h"

typedef int32 SoundID;

struct Player;
struct UIID;
class CRefString;

namespace UIWaypointMap {
enum MapTutorialState {
    MAP_TUTORIAL_STATE_INVALID = -1,
    MAP_TUTORIAL_STATE_INTRO = 0,
    MAP_TUTORIAL_STATE_WORLD_VIEW = 1,
    MAP_TUTORIAL_STATE_WORLD_VIEW_HIGHLIGHT_ACTS = 2,
    MAP_TUTORIAL_STATE_ACT_VIEW = 3,
    MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTIES = 4,
    MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTIES_INDICATOR = 5,
    MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTY_REWARDS = 6,
    MAP_TUTORIAL_STATE_CONCLUSION = 7,
    MAP_TUTORIAL_STATE_DONE = 8,
    NUM_MAP_TUTORIAL_STATES = 9,
};

struct TutorialState {  // size 0x10
    BOOL bOpenForTutorial;  // 0x00
    UIWaypointMap::MapTutorialState eMapTutorialState;  // 0x04
    SoundID idTutorialSound;  // 0x08
    uint32 dwTickNextTutorialAdvance;  // 0x0c
    TutorialState();
};
static_assert(sizeof(TutorialState) == 0x10, "TutorialState");

BOOL IsOpen();
BOOL IsOpenForTutorial();  // defined outside this file (0x7100178120)
void ShowActMap(Act eAct);
void ShowWorldMap();
void SetupWorldControls(Player * ptPlayerViewing);
void UpdateWorldControls(Player * ptPlayerViewing, BOOL bHighlightActs);
void CountBountiesAtWaypoints(Act eAct, int32 & nBountyCount, int32 & nCompletedBountyCount);
CRefString GetBountyTooltipTextForAct(Act eAct);
void SetupWaypointControls(Player * ptPlayerViewing, Act eAct);
void UpdateWaypointControls(Player * ptPlayerViewing, Act eAct, BOOL bHighlightBounties);
void SetupBountyOverlay(Player * ptPlayerViewing, Act eAct);
void UpdateBountyRewards(Player * ptPlayer, Act eAct, BOOL bHighlightBountyRewards);
void UpdateBountyOverlay(Player * ptPlayerViewing, Act eAct, BOOL bHighlightBountyRewards);
SNO GetQuestWaypointLevelArea(const Player * ptPlayer);
void UpdateQuestWaypointCallout(const UIID & idWaypoint, SNO snoQuestWaypoint);
void ActivateWaypoint(Player * ptPlayer, const int32 nWaypointIndex);
void TutorialAdvance(Player * ptPlayer, UIWaypointMap::TutorialState & tTutorialState);
BOOL TutorialCanBeClosed(Player * ptPlayer);

}  // namespace UIWaypointMap
