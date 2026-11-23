// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIQuestComplete.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Common/Protobuf/Quest.pb.h"

struct UIID;

namespace UIRewards {
BOOL IsAnyVisible();
void CloseAll();
void ShowTieredRiftReward(const D3::Quests::QuestReward & tReward, const BOOL bFinishedInTime, const int32 nRiftDepth, const int32 nControllerIndex);
}  // namespace UIRewards

























class UIQuestComplete {  // size 0x60
public:
    UIQuestComplete();
    static void Close(const UIID &);
    static void ConsoleClose(const UIID & id, const int32);
    static void OnQuestReward(const D3::Quests::QuestReward & tReward, const BOOL bCompleted, const BOOL bEquipped, const int32 nControllerIndex);
    static void OnMultiplayerQuestReward(const int32 nControllerIndex, const D3::Quests::QuestReward & tReward, const BOOL bCompleted, const BOOL bEquipped);
    static void HandleItemTooltip(const UIID & id);
    void Update();
    BOOL IsVisible();
    void Show(const BOOL fVisible, const D3::Quests::QuestReward * ptReward, BOOL bEquipped);
    void ShowMultiplayerRewards(const int32 nControllerIndex, const D3::Quests::QuestReward * ptReward);
    void ShowItemReward(BOOL bEquipped);
    const D3::Quests::QuestReward & GetReward() const;
private:
    uint32 m_dwStartTime;  // 0x00
    D3::Quests::QuestReward m_tQuestReward;  // 0x08
};
static_assert(sizeof(UIQuestComplete) == 0x60, "UIQuestComplete");










































class UIBountyReward {  // size 0x58
public:
    void Show(const BOOL, const D3::Quests::QuestReward *);
    void ConsoleShow(const BOOL fVisible, const D3::Quests::QuestReward * ptReward);
    BOOL IsVisible();
    static void Close(const UIID & id);
    static void ConsoleClose(const UIID & id, const int32);
    static void HandleItemTooltip(const UIID &);
    const D3::Quests::QuestReward & GetReward() const;
    D3::Quests::QuestReward m_tQuestReward;  // 0x00
};
static_assert(sizeof(UIBountyReward) == 0x58, "UIBountyReward");






















class UITieredRiftReward {  // size 0x58
public:
    void Show(const D3::Quests::QuestReward * ptReward, const int32 nNewBloodShardCap);
    void Hide();
    BOOL IsVisible();
    static void Close(const UIID & id);
    static void ConsoleClose(const UIID & id, const int32);
    static void ConsoleOpenLeaderboards(const UIID & id, const int32 nControllerIndex);
    static void HandleItemTooltip(const UIID &);
    const D3::Quests::QuestReward & GetReward() const;
    D3::Quests::QuestReward m_tQuestReward;  // 0x00
};
static_assert(sizeof(UITieredRiftReward) == 0x58, "UITieredRiftReward");


























class UITieredRiftScorecard {  // size 0x58
public:
    void Show(const D3::Quests::QuestReward * ptReward, const int32 nRiftDepth);
    void Hide();
    BOOL IsVisible();
    static void Close(const UIID & id);
    static void ConsoleClose(const UIID & id, const int32);
    const D3::Quests::QuestReward & GetReward() const;
    D3::Quests::QuestReward m_tQuestReward;  // 0x00
};
static_assert(sizeof(UITieredRiftScorecard) == 0x58, "UITieredRiftScorecard");
