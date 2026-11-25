// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Achievements/Console/ConsoleAchievements.h
// Subset: the Console achievements classes AppGlobals (X/Common/Game.h:637-638)
// exposes through the season journey UI: Console::Achievements (:29, size
// 0x118), Console::AchievementsInterfaceBase (:49, size 0x10) and
// Console::ClientAchievementsInterface (:102, size 0x30), and Console::ServerAchievementsInterface
// (size 0x38) with its PlayerAchievementsRecord.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../AchievementsEngine/achievements_static_data.h"
#include "../../OnlineService/Achievements.h"
#include "../../OnlineService/OnlineService.h"
#include "../../OnlineService/OnlineServiceErrorCodes.h"
#include "../../../Server/Achievements/AchievementsEngine/achievements_evaluator.h"
#include "../../../Server/Achievements/AchievementsEngine/achievements_listener.h"
#include "../../../Server/Achievements/AchievementsEngine/achievements_progress_data.h"
#include "../../../Server/Achievements/AchievementsEngine/achievements_update_structs.h"
#include "../../../Sigma/blzext/fixed_vector.h"

namespace D3 { namespace Account { class SavedDefinition; } }
namespace D3 { namespace Achievements { class Snapshot; } }
namespace D3 { namespace GameMessage { class AchievementsAwardedConsole; } }
namespace D3 { namespace GameMessage { class AchievementsCriteriaUpdateConsole; } }
namespace D3 { namespace GameMessage { class AchievementsSnapshot; } }

namespace Console {

class Achievements;
class ClientAchievementsInterface;
class ServerAchievementsInterface;

class Achievements : public OnlineService::Achievements {  // size 0x118
public:
    Achievements();
    virtual ~Achievements();  // vtable slot 0
    virtual OnlineService::AchievementsInterface * CreateAchievementsInterface();  // vtable slot 6
    Console::ClientAchievementsInterface * CreateClientAchievementsInterface();
    Console::ServerAchievementsInterface * CreateServerAchievementsInterface();
    virtual void DestroyAchievementsInterface(OnlineService::AchievementsInterface * ptAchievementsInterface);  // vtable slot 7
    void ReloadStaticData();
    static void InitializeForApp();
    static void DisposeForApp();
    virtual bool IsInitialized();  // vtable slot 2
    virtual void Process();  // vtable slot 4
private:
    void CreateItemReward(const char *, OnlineService::AchievementId, uint32);
    BOOL LoadStaticDataFile();
};
static_assert(sizeof(Achievements) == 0x118, "Achievements");

class AchievementsInterfaceBase : public OnlineService::AchievementsInterface {  // size 0x10
public:
    AchievementsInterfaceBase(Console::Achievements *);
    virtual ~AchievementsInterfaceBase() {}  // vtable slot 0
    virtual void RegisterSubscription(OnlineService::AccountId, const OnlineService::GameAccountHandle &, const OnlineService::Callback<void (int)> &);  // vtable slot 4
    virtual bool CancelSubscription(const OnlineService::GameAccountHandle &, const OnlineService::Callback<void (int)> &);  // vtable slot 5
    virtual void SetSnapshotCacheSize(uint64);  // vtable slot 8
    virtual void SetSnapshotExpireSeconds(double);  // vtable slot 9
    virtual void RequestSnapshot(const OnlineService::GameAccountHandle & gameAccount, const OnlineService::Callback<void (int, const OnlineService::GameAccountHandle &)> & cb);  // vtable slot 10
    virtual bool GetSnapshot(const OnlineService::GameAccountHandle & gameAccount, D3::Achievements::Snapshot & tSnapshot);  // vtable slot 11
    virtual void OnAchievementsSnapshotReceived(D3::GameMessage::AchievementsSnapshot & tSnapshot);  // vtable slot 12
};
static_assert(sizeof(AchievementsInterfaceBase) == 0x10, "AchievementsInterfaceBase");

class ServerAchievementsInterface : public Console::AchievementsInterfaceBase {  // size 0x38
public:
    ServerAchievementsInterface(Console::Achievements *, uint32);
    virtual ~ServerAchievementsInterface() {}  // vtable slot 0; line 152
    virtual void Process();  // vtable slot 2
    virtual void Flush(const OnlineService::GameAccountHandle & gameAccount);  // vtable slot 3
    OnlineService::ErrorCode RegisterPlayer(const OnlineService::GameAccountHandle & gameAccount, const D3::Account::SavedDefinition & tDefinition);
    virtual bool CancelSubscription(const OnlineService::GameAccountHandle & gameAccount, const OnlineService::AchievementsInterface::RegistrationCallback & cb);  // vtable slot 5
    virtual OnlineService::ErrorCode EvaluateEvent(const OnlineService::GameAccountHandle & gameAccount, uint32 eventId, uint64 comparand, uint64 quantity, OnlineService::Timestamp timestamp, uint32 uUnkFlags, const OnlineService::AchievementsInterface::ModifierCallback & cb);  // vtable slot 7
    virtual bool FindAwardedAchievement(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::AchievementId id, OnlineService::AchievementRecord & record);  // vtable slot 13
    virtual bool FindAwardedAchievements(const OnlineService::GameAccountHandle & gameAccount, blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > & list);  // vtable slot 14
    virtual bool FindCriteriaRecord(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::CriteriaId criteriaId, OnlineService::CriteriaRecord & record);  // vtable slot 15
    bool PopulateAccountAchievementSnapshot(const OnlineService::GameAccountHandle & gameAccount, D3::Account::SavedDefinition & tDefinition);
    bool ClearAchievementSnapshot(const OnlineService::GameAccountHandle & gameAccount);
    void ClearAllAchievementSnapshots();
private:
    static bool sModifierCallback(const OnlineService::AchievementsInterface::ModifierCallback &, uint64, uint64);
    struct PlayerAchievementsRecord : public OnlineService::AchievementsEngine::Listener {  // size 0x1090
        OnlineService::AchievementsEngine::ProgressData m_snapshot;  // 0x18
        OnlineService::AchievementsEngine::AchievementsEvaluator m_tAchievementsEvaluator;  // 0x88
        OnlineService::AchievementsEngine::AchievementsEvaluator unk_178;  // 0x178 (2.7.7: second evaluator, achievement set 1, static data [1])
        blzext::fixed_vector<blz::pair<uint32, unsigned long>, 32> m_achievementsBuffer;  // 0x268 (2.7.7: {achievement set, id})
        blzext::fixed_vector<OnlineService::AchievementsEngine::CriteriaUpdateRecord, 64> m_criteriaUpdateBuffer;  // 0x478
        uint32 m_dwTimeLastFlush;  // 0x1088
        PlayerAchievementsRecord(const OnlineService::AchievementsEngine::AchievementsStaticData * ptStaticData, const OnlineService::GameAccountHandle & gameAccount, uint32 partitionMask);
        virtual ~PlayerAchievementsRecord() { SendAccumulatedUpdates(); }  // vtable slot 0; line 242
        virtual void OnCriteriaUpdate(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteria);  // vtable slot 0
        virtual void OnCriteriaDeleted(const OnlineService::AchievementsEngine::CriteriaDeleteRecord & criteria);  // vtable slot 1
        virtual void OnAchievementUpdate(const OnlineService::AchievementsEngine::AchievementUpdateRecord & achievement);  // vtable slot 2
        virtual void SendAccumulatedUpdates();  // vtable slot 3
        virtual void EnableNotifications();  // vtable slot 4
    };
    typedef blz::unordered_map<OnlineService::GameAccountHandle, blz::shared_ptr<Console::ServerAchievementsInterface::PlayerAchievementsRecord>, blz::hash<OnlineService::GameAccountHandle>, blz::equal_to<OnlineService::GameAccountHandle>, blz::allocator<blz::pair<const OnlineService::GameAccountHandle, blz::shared_ptr<Console::ServerAchievementsInterface::PlayerAchievementsRecord> > > > AchievementRecordsMap;
    Console::ServerAchievementsInterface::AchievementRecordsMap m_tAchievementRecordsMap;  // 0x10
    uint32 m_partitionMask;  // 0x30
    blz::shared_ptr<Console::ServerAchievementsInterface::PlayerAchievementsRecord> LoadPlayerAchievementsRecord(const OnlineService::GameAccountHandle & gameAccount, const D3::Account::SavedDefinition & tDefinition);
    void FlushRecord(const OnlineService::GameAccountHandle &, Console::ServerAchievementsInterface::PlayerAchievementsRecord *);
};
static_assert(sizeof(ServerAchievementsInterface) == 0x38, "ServerAchievementsInterface");

class ClientAchievementsInterface : public Console::AchievementsInterfaceBase {  // size 0x30
public:
    ClientAchievementsInterface(Console::Achievements *);
    virtual ~ClientAchievementsInterface() {}  // vtable slot 0; line 276
    virtual void Process();  // vtable slot 2
    virtual bool FindAwardedAchievement(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::AchievementId id, OnlineService::AchievementRecord & record);  // vtable slot 13
    bool FindAwardedAchievement(int32 nUserIndex, uint32 achievementSet, OnlineService::AchievementId id, OnlineService::AchievementRecord & record);
    virtual bool FindAwardedAchievements(const OnlineService::GameAccountHandle & gameAccount, blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > & list);  // vtable slot 14
    bool FindAwardedAchievements(int32 nUserIndex, blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > & list);
    virtual bool FindCriteriaRecord(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::CriteriaId criteriaId, OnlineService::CriteriaRecord & record);  // vtable slot 15
    bool FindCriteriaRecord(int32 nUserIndex, uint32 achievementSet, OnlineService::CriteriaId criteriaId, OnlineService::CriteriaRecord & tRecord);
    bool PopulateAccountAchievementSnapshot(int32 nUserIndex, D3::Account::SavedDefinition & tDefinition);
    void OnAchievementAwarded(int32 nUserIndex, const D3::GameMessage::AchievementsAwardedConsole & tUpdateMsg);
    void OnAchievementCriteriaUpdate(int32 nUserIndex, const D3::GameMessage::AchievementsCriteriaUpdateConsole & tUpdateMsg);
    void OnUserSignedIn(int32 nUserIndex);
    void OnUserSignedOut(int32);
    static OnlineService::GameAccountHandle UserIndexToGameAccount(int32 nUserIndex);
    static int32 GameAccountToUserIndex(const OnlineService::GameAccountHandle & gameAccount);
private:
    void LoadPlayerAchievements(int nUserIndex);
    typedef blz::unordered_map<int, OnlineService::AchievementsEngine::ProgressData, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, OnlineService::AchievementsEngine::ProgressData> > > SnapshotMap;
    Console::ClientAchievementsInterface::SnapshotMap m_snapshots;  // 0x10
};
static_assert(sizeof(ClientAchievementsInterface) == 0x30, "ClientAchievementsInterface");

inline OnlineService::GameAccountHandle ClientAchievementsInterface::UserIndexToGameAccount(int32 nUserIndex)  // DWARF ConsoleAchievements.h:341
{
    return OnlineService::GameAccountHandle(nUserIndex + 1, OnlineService::PROGRAM_D3, 1);
}

// line 347. The user index is the game account id less one.
inline int32 ClientAchievementsInterface::GameAccountToUserIndex(const OnlineService::GameAccountHandle & gameAccount) {
    return gameAccount.m_id - 1;
}

}  // namespace Console
