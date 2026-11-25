// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/Subsystems/FanfareManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ToastManager.h"
#include "../../UserInterface/Controls/UIToolTipControl.h"
#include "../../UserInterface/UIID.h"
#include "../../../Common/Leaderboard.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/RefString.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"

struct PlatinumAchievementAwardedMessage;
namespace OnlineService { typedef uint64 AchievementId; }  // OnlineService/Achievements.h
struct ConsoleScreenManagerData;

typedef int32 EffectGroupID;

class FanfareManager : public ToastManagerBase {  // size 0x1f8
public:
    void SendFanfare(OnlineService::AchievementId idAchievement, int32 nRank, const PlatinumAchievementAwardedMessage * ptMessage);
    void SendFanfare(LeaderboardIds, LeaderboardScope, int32);
    void SendSeasonStartFanfare(uint32);
    void Update();
    static void OnFanfareClicked(const UIID &);
    void ClearFanfareQueue();
    void DisplayFanfareForPlayer(uint32 nPlayerIndex, OnlineService::AchievementId idAchievement);
    BOOL IsActive() const;
    struct UIAnimationPair {  // size 0x10
        UIAnimationPair();
        UIAnimationPair(const UIID &);
        UIAnimationPair(const UIID &, const UIID &);
        UIID idAnimA;  // 0x00
        UIID idAnimB;  // 0x08
    };
    typedef XGrowableList<FanfareManager::UIAnimationPair, const FanfareManager::UIAnimationPair &> UIAnimPairList;
    enum ToastType {
        TT_ACHIEVEMENT = 0,
        TT_REWARD = 1,
        TT_CONQUEST = 2,
        TT_PERSONALBEST = 3,
        TT_SEASONSTART = 4,
        TT_DAILYFIRST = 5,
        TT_SEASON_JOURNEY_PROGRESSION = 6,
        TT_SEASON_JOURNEY_COMPLETION = 7,
    };
    struct FanfareToast : public ToastManagerBase::Toast {  // size 0xd8
        int32 nPoints;  // 0x40
        int32 nRank;  // 0x44
        CRefString sTypeMessage;  // 0x48
        EffectGroupID idEffectFront;  // 0x60
        EffectGroupID idEffectBack;  // 0x64
        FanfareManager::UIAnimPairList listStoryboards;  // 0x68
        FanfareManager::ToastType eToastType;  // 0xc0
        uint32 uPlatinumAwarded;  // 0xc4
        uint64 uPreviousPlatinumBalance;  // 0xc8
        int32 nNumEarnedSeasonJourneyCriteria;  // 0xd0
        int32 nTotalSeasonJourneyCriteria;  // 0xd4
    };
protected:
    virtual const UIID CreateToastVisual(int32, ToastManagerBase::Toast * ptToast);  // vtable slot 4
    virtual void UpdateToastVisual(int32, ToastManagerBase::Toast * ptToast, float);  // vtable slot 6
    virtual void DisposeToastVisual(ToastManagerBase::Toast * ptToast);  // vtable slot 5
private:
    // Not in the DWARF: the owners that construct the private manager.
    friend struct ConsoleScreenManagerData;
    FanfareManager();
    virtual ~FanfareManager();  // vtable slot 0
    static void SetUpToastStoryboards(FanfareManager::FanfareToast &, BOOL, BOOL, BOOL);
    // 2.7.7: ctor stores {8, NULL, 0, 1.0f} at 0x1d8..0x1f0 and the dtors free the chains;
    // sub_71001635d0 hashes a uint64 key (fmix64) into it and edits a uint32 bitmask value.
    blz::unordered_map<unsigned long, uint32> unk_1d8;  // 0x1d8
};
static_assert(sizeof(FanfareManager) == 0x1f8, "FanfareManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    57  FanfareManager::UIAnimationPair & FanfareManager::UIAnimationPair::operator=(const FanfareManager::UIAnimationPair &)  [inlined into 1 functions]
//   line    81  FanfareManager::FanfareToast::FanfareToast()  [inlined into 1 functions]
//   line    81  FanfareManager::FanfareToast::~FanfareToast()  [0x710037db10 (+1 more)]
