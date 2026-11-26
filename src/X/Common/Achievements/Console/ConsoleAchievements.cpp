// Original file: X/Common/Achievements/Console/ConsoleAchievements.cpp

#include "ConsoleAchievements.h"
#include "../../OnlineService/Achievements.h"
#include "../../OnlineService/OnlineService.h"
#include "../../Game.h"
#include "../../GameCommonData.h"
#include "../../Protobuf/Account.pb.h"
#include "../../Protobuf/Achievements.pb.h"
#include "../AchievementsEngine/achievements_types.h"
#include "../AchievementsEngine/achievements_internal_static_data_structs.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../Protobuf/GameMessage.pb.h"
#include "../../../Client/Console/GamerProfile.h"
extern "C" void sub_7100468fe0(const OnlineService::AchievementsEngine::AchievementsStaticData *, D3::Achievements::Snapshot *);
namespace Console {

// ConsoleAchievementsShared.cpp (no header in this checkout).
namespace AchievementsShared {
void SendAchievementAwardedMessage(const OnlineService::GameAccountHandle & gameAccount, uint32 uUnk, const blz::pair<uint32, unsigned long> * ptAchievements, size_t uNumAchievements);
void SendAchievementCriteriaUpdate(const OnlineService::GameAccountHandle & gameAccount, uint32 uUnk, const OnlineService::AchievementsEngine::CriteriaUpdateRecord * ptUpdates, size_t uNumUpdates);
}

static const uint32 TICKS_BETWEEN_FLUSHES = 300;  // line 40






















inline Achievements::Achievements() {
}






Achievements::~Achievements() {
}




void Achievements::InitializeForApp() {
    AppGlobals & tAppGlobals = sgtAppGlobals;
    tAppGlobals.ptAchievements = new Achievements();
    tAppGlobals.ptAchievements->LoadAchievementsDataByLooseFile();
    tAppGlobals.ptAchievementsInterface = tAppGlobals.ptAchievements->CreateClientAchievementsInterface();
}













void Achievements::DisposeForApp() {
    AppGlobals & tAppGlobals = sgtAppGlobals;
    tAppGlobals.ptAchievements->DestroyAchievementsInterface(tAppGlobals.ptAchievementsInterface);
    tAppGlobals.ptAchievementsInterface = NULL;
    ::delete tAppGlobals.ptAchievements;
    tAppGlobals.ptAchievements = NULL;
}





















OnlineService::AchievementsInterface * Achievements::CreateAchievementsInterface() {
    DisplayInternalError(NULL, "", 109, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}



inline ClientAchievementsInterface * Achievements::CreateClientAchievementsInterface() {
    ClientAchievementsInterface * ptInterface = new ClientAchievementsInterface(this);
    return ptInterface;
}





ServerAchievementsInterface * Achievements::CreateServerAchievementsInterface() {
    uint32 partitionMask = 1;
    if (GameIsSeasonal()) {
        int32 seasonPartition = GetSeasonDataPartition(OnlineService::GetSeasonNum());
        if (seasonPartition > 0)
            partitionMask |= 1 << seasonPartition;
    }
    return new ServerAchievementsInterface(this, partitionMask);
}










void Achievements::DestroyAchievementsInterface(OnlineService::AchievementsInterface * ptAchievementsInterface) {
    ::delete ptAchievementsInterface;
}
















inline ServerAchievementsInterface::PlayerAchievementsRecord::PlayerAchievementsRecord(const OnlineService::AchievementsEngine::AchievementsStaticData * ptStaticData, const OnlineService::GameAccountHandle & gameAccount, uint32 partitionMask)
    : OnlineService::AchievementsEngine::Listener(gameAccount)
    , m_tAchievementsEvaluator(0, ptStaticData, &m_snapshot, partitionMask)
    , unk_178(1, ptStaticData + 1, &m_snapshot, partitionMask), m_dwTimeLastFlush(0) {
}








void ServerAchievementsInterface::PlayerAchievementsRecord::OnCriteriaUpdate(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteria) {
    if (criteria.m_startTime == 0) {
        blz::reverse_iterator<OnlineService::AchievementsEngine::CriteriaUpdateRecord *> it = blz::find_if(m_criteriaUpdateBuffer.rbegin(), m_criteriaUpdateBuffer.rend(),
            [&criteria](const OnlineService::AchievementsEngine::CriteriaUpdateRecord & tRecord) { return tRecord.unk_0 == criteria.unk_0 && tRecord.m_criteria == criteria.m_criteria; });
        if (it != m_criteriaUpdateBuffer.rend() && (!(it->m_flags & OnlineService::AchievementsEngine::TRACKED_CRITERIA_FLAG_DELETE_FROM_DB) || (criteria.m_flags & OnlineService::AchievementsEngine::TRACKED_CRITERIA_FLAG_DELETE_FROM_DB))) {
            *it = criteria;
        } else {
            m_criteriaUpdateBuffer.push_back(criteria);
            if (m_criteriaUpdateBuffer.full())
                SendAccumulatedUpdates();
        }
    }
}





























void ServerAchievementsInterface::PlayerAchievementsRecord::OnCriteriaDeleted(const OnlineService::AchievementsEngine::CriteriaDeleteRecord & criteria) {
    OnlineService::AchievementsEngine::CriteriaUpdateRecord tCriteria(criteria.unk_0);
    tCriteria.m_criteria = criteria.m_criteria;
    tCriteria.m_flags = OnlineService::AchievementsEngine::TRACKED_CRITERIA_FLAG_DELETE_FROM_DB;
    OnCriteriaUpdate(tCriteria);
}





void ServerAchievementsInterface::PlayerAchievementsRecord::OnAchievementUpdate(const OnlineService::AchievementsEngine::AchievementUpdateRecord & achievement) {
    if (m_achievementsBuffer.full())
        SendAccumulatedUpdates();
    m_achievementsBuffer.push_back(blz::pair<uint32, unsigned long>(achievement.unk_0, achievement.m_achievementId));
}

































void ServerAchievementsInterface::PlayerAchievementsRecord::SendAccumulatedUpdates() {
    m_dwTimeLastFlush = GameGetServerTime();
    if (!m_criteriaUpdateBuffer.empty()) {
        const OnlineService::AchievementsEngine::CriteriaUpdateRecord * ptStart = m_criteriaUpdateBuffer.begin();
        const OnlineService::AchievementsEngine::CriteriaUpdateRecord * it = m_criteriaUpdateBuffer.begin();
        for (; it != m_criteriaUpdateBuffer.end(); ++it) {
            if (it->unk_0 != ptStart->unk_0) {
                AchievementsShared::SendAchievementCriteriaUpdate(m_gameAccount, ptStart->unk_0, ptStart, it - ptStart); ptStart = it;
            }
        }
        if (it - ptStart) AchievementsShared::SendAchievementCriteriaUpdate(m_gameAccount, ptStart->unk_0, ptStart, it - ptStart);
        m_criteriaUpdateBuffer.clear();
    }
    if (!m_achievementsBuffer.empty()) {
        const blz::pair<uint32, unsigned long> * ptStart = m_achievementsBuffer.begin();
        const blz::pair<uint32, unsigned long> * it = m_achievementsBuffer.begin();
        for (; it != m_achievementsBuffer.end(); ++it) {
            if (it->first != ptStart->first) {
                AchievementsShared::SendAchievementAwardedMessage(m_gameAccount, ptStart->first, ptStart, it - ptStart); ptStart = it;
            }
        }
        if (it - ptStart) AchievementsShared::SendAchievementAwardedMessage(m_gameAccount, ptStart->first, ptStart, it - ptStart);
        m_achievementsBuffer.clear();
    }
}
void ServerAchievementsInterface::PlayerAchievementsRecord::EnableNotifications() {
    OnlineService::AchievementsEngine::Listener::EnableNotifications();
}







inline AchievementsInterfaceBase::AchievementsInterfaceBase(Achievements * ptAchievements)
    : OnlineService::AchievementsInterface(ptAchievements) {
}





void AchievementsInterfaceBase::RegisterSubscription(OnlineService::AccountId, const OnlineService::GameAccountHandle &,
                                                     const OnlineService::AchievementsInterface::RegistrationCallback &) {
    DisplayInternalError(NULL, "", 350, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}









bool AchievementsInterfaceBase::CancelSubscription(const OnlineService::GameAccountHandle &,
                                                   const OnlineService::AchievementsInterface::RegistrationCallback &) {
    DisplayInternalError(NULL, "", 363, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}









void AchievementsInterfaceBase::SetSnapshotCacheSize(uint64) {
}








void AchievementsInterfaceBase::SetSnapshotExpireSeconds(double) {
}








void AchievementsInterfaceBase::RequestSnapshot(const OnlineService::GameAccountHandle & gameAccount,
                                                const OnlineService::AchievementsInterface::SnapshotCallback & cb) {
    if (cb)
        cb(0, gameAccount);
}













bool AchievementsInterfaceBase::GetSnapshot(const OnlineService::GameAccountHandle & gameAccount,
                                            D3::Achievements::Snapshot & tSnapshot) {
    return false;
}







void AchievementsInterfaceBase::OnAchievementsSnapshotReceived(D3::GameMessage::AchievementsSnapshot & tSnapshot) {
}






inline ServerAchievementsInterface::ServerAchievementsInterface(Achievements * ptAchievements, uint32 partitionMask)
    : AchievementsInterfaceBase(ptAchievements)
    , m_partitionMask(partitionMask) {
}






void ServerAchievementsInterface::Process() {
    uint32 dwNow = GameGetServerTime();
    for (AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.begin(); it != m_tAchievementRecordsMap.end(); ++it) {
        blz::shared_ptr<PlayerAchievementsRecord> & ptPlayerAchievementsRecord = it->second;
        if (!ptPlayerAchievementsRecord->m_achievementsBuffer.empty() || ptPlayerAchievementsRecord->m_tAchievementsEvaluator.FlushImmediatelyIfSignaled() || ptPlayerAchievementsRecord->unk_178.FlushImmediatelyIfSignaled() || dwNow - ptPlayerAchievementsRecord->m_dwTimeLastFlush >= TICKS_BETWEEN_FLUSHES) {
            ptPlayerAchievementsRecord->m_tAchievementsEvaluator.Flush();
            ptPlayerAchievementsRecord->m_tAchievementsEvaluator.SendAccumulatedUpdates();
            ptPlayerAchievementsRecord->unk_178.Flush(); ptPlayerAchievementsRecord->unk_178.SendAccumulatedUpdates();
        }
    }
}















void ServerAchievementsInterface::Flush(const OnlineService::GameAccountHandle & gameAccount) {
    AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.find(gameAccount);
    if (it != m_tAchievementRecordsMap.end()) {
        it->second->m_tAchievementsEvaluator.Flush();
        it->second->m_tAchievementsEvaluator.SendAccumulatedUpdates();
        it->second->unk_178.Flush(); it->second->unk_178.SendAccumulatedUpdates();
    }
}






OnlineService::ErrorCode ServerAchievementsInterface::RegisterPlayer(const OnlineService::GameAccountHandle & gameAccount, const D3::Account::SavedDefinition & tDefinition) {
    if (!m_ptAchievements->IsEnabled())
        return OnlineService::ERROR_API_NOT_INITIALIZED;
    if (m_tAchievementRecordsMap.find(gameAccount) != m_tAchievementRecordsMap.end())
        return OnlineService::ERROR_GLOBAL_INVALID_REQUEST;
    m_tAchievementRecordsMap[gameAccount] = LoadPlayerAchievementsRecord(gameAccount, tDefinition);
    return OnlineService::ERROR_GLOBAL_SUCCESS;
}


















bool ServerAchievementsInterface::CancelSubscription(const OnlineService::GameAccountHandle & gameAccount, const OnlineService::AchievementsInterface::RegistrationCallback & cb) {
    if (!m_ptAchievements->IsEnabled()) {
        if (cb)
            cb(OnlineService::ERROR_API_NOT_INITIALIZED);
        return false;
    }
    bool bFound = false;
    AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.find(gameAccount);
    if (it != m_tAchievementRecordsMap.end()) {
        blz::shared_ptr<PlayerAchievementsRecord> & ptPlayerAchievementsRecord = it->second;
        ptPlayerAchievementsRecord->m_tAchievementsEvaluator.UnregisterListener(ptPlayerAchievementsRecord);
        ptPlayerAchievementsRecord->unk_178.UnregisterListener(ptPlayerAchievementsRecord);
        m_tAchievementRecordsMap.erase(it);
        bFound = true;
    }
    if (cb)
        cb(OnlineService::ERROR_GLOBAL_SUCCESS);
    return bFound;
}

















OnlineService::ErrorCode ServerAchievementsInterface::EvaluateEvent(const OnlineService::GameAccountHandle & gameAccount, uint32 eventId, uint64 comparand, uint64 quantity, OnlineService::Timestamp timestamp, uint32 uUnkFlags, const OnlineService::AchievementsInterface::ModifierCallback & cb) {
    if (!m_ptAchievements->IsEnabled())
        return OnlineService::ERROR_API_NOT_INITIALIZED;
    AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.find(gameAccount);
    if (it != m_tAchievementRecordsMap.end()) {
        PlayerAchievementsRecord * ptRecord = it->second.get();
        OnlineService::AchievementsEngine::AchievementsEvaluator & tFirst = (uUnkFlags & 2) ? ptRecord->unk_178 : ptRecord->m_tAchievementsEvaluator;
        OnlineService::AchievementsEngine::AchievementsEvaluator & tSecond = (uUnkFlags & 2) ? ptRecord->m_tAchievementsEvaluator : ptRecord->unk_178;
        const uint64 updateTimeSeconds = timestamp.GetSeconds();
        tFirst.CheckEvent(eventId, comparand, quantity, updateTimeSeconds, -1, cb);
        tSecond.CheckEvent(eventId, comparand, quantity, updateTimeSeconds, 1, cb); }
    return OnlineService::ERROR_GLOBAL_SUCCESS;
}














bool ServerAchievementsInterface::FindAwardedAchievement(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::AchievementId id, OnlineService::AchievementRecord & record) {
    bool bFoundAch = false;
    if (m_ptAchievements->IsEnabled()) {
        AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.find(gameAccount);
        if (it != m_tAchievementRecordsMap.end()) {
            PlayerAchievementsRecord * ptPlayerAchievementsRecord = it->second.get();
            OnlineService::AchievementsEngine::Achievement * ptAchievement = NULL;
            bFoundAch = ptPlayerAchievementsRecord->m_snapshot.FindAchievement(achievementSet, id, ptAchievement);
            if (bFoundAch) {
                ConvertAchievementRecord(*ptAchievement, record);
            }

        }
    }
    return bFoundAch;
}



















bool ServerAchievementsInterface::FindAwardedAchievements(const OnlineService::GameAccountHandle & gameAccount, blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > & list) {
    if (!m_ptAchievements->IsEnabled())
        return false;
    AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.find(gameAccount);
    if (it == m_tAchievementRecordsMap.end())
        return false;
    PlayerAchievementsRecord * ptPlayerAchievementsRecord = it->second.get();
    list.clear();
    for (size_t i = 0; i < ptPlayerAchievementsRecord->m_snapshot.GetAchievementCount(); ++i) {
        const OnlineService::AchievementsEngine::Achievement & tAch = ptPlayerAchievementsRecord->m_snapshot.GetAchievement(i);
        OnlineService::AchievementRecord tRecord(tAch.unk_0);
        ConvertAchievementRecord(tAch, tRecord);
        list.push_back(tRecord);
    }
    return list.size() > 0;
}




















bool ServerAchievementsInterface::FindCriteriaRecord(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::CriteriaId criteriaId, OnlineService::CriteriaRecord & record) {
    bool bFound = false;
    if (m_ptAchievements->IsEnabled()) {
        AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.find(gameAccount);
        if (it != m_tAchievementRecordsMap.end()) {
            PlayerAchievementsRecord * ptPlayerAchievementsRecord = it->second.get();
            OnlineService::AchievementsEngine::Criteria * ptCriteria = NULL;
            bFound = ptPlayerAchievementsRecord->m_snapshot.FindCriteria(achievementSet, criteriaId, ptCriteria);
            if (bFound) {
                ConvertCriteriaRecord(*ptCriteria, record);
            }

        }
    }
    return bFound;
}




















bool ServerAchievementsInterface::PopulateAccountAchievementSnapshot(const OnlineService::GameAccountHandle & gameAccount, D3::Account::SavedDefinition & tDefinition) {
    tDefinition.mutable_console_data()->clear_achievement_snapshot();
    AchievementRecordsMap::iterator it = m_tAchievementRecordsMap.find(gameAccount);
    if (it == m_tAchievementRecordsMap.end())
        return false;
    it->second->m_snapshot.MergeToProtocol(*tDefinition.mutable_console_data()->mutable_achievement_snapshot(), true, true, true);
    return true;
}
// NON_MATCHING: one ldp vs two ldr when reloading mapSet0 before its clear() (load pairing only)
// Probably: sync the account achievement snapshot between achievement sets 0 and 1 (2.7.7). Criteria and
// achievements whose static definition has partition mask 1 are shared by both sets: an entry present in only
// one set is copied into the other.
extern "C" void sub_7100468fe0(const OnlineService::AchievementsEngine::AchievementsStaticData * ptStaticData, D3::Achievements::Snapshot * ptSnapshot) {
    blz::unordered_map<uint32, int32> mapSet0;
    blz::unordered_map<uint32, int32> mapSet1;
    for (int32 i = 0; i < ptSnapshot->criteria_snapshot_size(); i++) {
        const D3::Achievements::CriteriaUpdateRecord & tRecord = ptSnapshot->criteria_snapshot(i);
        const OnlineService::AchievementsEngine::AchievementsStaticData & tStaticData = tRecord.achievement_set() == 0 ? ptStaticData[0] : ptStaticData[1];
        uint32 uId = tRecord.criteria_id_32_and_flags_8();
        const OnlineService::AchievementsEngine::InternalCriteriaDefinition * ptCriteria = tStaticData.GetCriteriaDefinition(OnlineService::AchievementsEngine::Make64BitCriteriaId(uId));
        if (ptCriteria && ptCriteria->m_parentAchievement && ptCriteria->m_parentAchievement->m_partitionMask == 1)
            (tRecord.achievement_set() == 0 ? mapSet0 : mapSet1)[uId] = i;
    }
    for (blz::unordered_map<uint32, int32>::iterator it = mapSet1.begin(); it != mapSet1.end(); ++it) {
        if (mapSet0.find(it->first) == mapSet0.end()) {
            D3::Achievements::CriteriaUpdateRecord * ptNew = ptSnapshot->add_criteria_snapshot();
            ptNew->CopyFrom(ptSnapshot->criteria_snapshot(it->second));
            ptNew->set_achievement_set(0);
        }
    }
    for (blz::unordered_map<uint32, int32>::iterator it = mapSet0.begin(); it != mapSet0.end(); ++it) {
        if (mapSet1.find(it->first) == mapSet1.end()) {
            D3::Achievements::CriteriaUpdateRecord * ptNew = ptSnapshot->add_criteria_snapshot();
            ptNew->CopyFrom(ptSnapshot->criteria_snapshot(it->second));
            ptNew->set_achievement_set(1);
        }
    }
    mapSet0.clear();
    mapSet1.clear();
    for (int32 i = 0; i < ptSnapshot->achievement_snapshot_size(); i++) {
        const D3::Achievements::AchievementUpdateRecord & tRecord = ptSnapshot->achievement_snapshot(i);
        const OnlineService::AchievementsEngine::AchievementsStaticData & tStaticData = tRecord.achievement_set() == 0 ? ptStaticData[0] : ptStaticData[1];
        uint32 uId = (uint32)(tRecord.achievement_id() & 0xffffff);
        const OnlineService::AchievementsEngine::InternalAchievementDefinition * ptAchievement = tStaticData.GetAchievementDefinition(sub_710046b080(uId));
        if (ptAchievement && ptAchievement->m_partitionMask == 1)
            (tRecord.achievement_set() == 0 ? mapSet0 : mapSet1)[uId] = i;
    }
    for (blz::unordered_map<uint32, int32>::iterator it = mapSet1.begin(); it != mapSet1.end(); ++it) {
        if (mapSet0.find(it->first) == mapSet0.end()) {
            D3::Achievements::AchievementUpdateRecord * ptNew = ptSnapshot->add_achievement_snapshot();
            ptNew->CopyFrom(ptSnapshot->achievement_snapshot(it->second));
            ptNew->set_achievement_set(0);
        }
    }
    for (blz::unordered_map<uint32, int32>::iterator it = mapSet0.begin(); it != mapSet0.end(); ++it) {
        if (mapSet1.find(it->first) == mapSet1.end()) {
            D3::Achievements::AchievementUpdateRecord * ptNew = ptSnapshot->add_achievement_snapshot();
            ptNew->CopyFrom(ptSnapshot->achievement_snapshot(it->second));
            ptNew->set_achievement_set(1);
        }
    }
}










blz::shared_ptr<ServerAchievementsInterface::PlayerAchievementsRecord> ServerAchievementsInterface::LoadPlayerAchievementsRecord(const OnlineService::GameAccountHandle & gameAccount, const D3::Account::SavedDefinition & tDefinition) {
    blz::shared_ptr<PlayerAchievementsRecord> ptPlayerAchievementsRecord = blz::make_shared<PlayerAchievementsRecord>(m_ptAchievements->GetStaticDataFile(), gameAccount, m_partitionMask);
    ptPlayerAchievementsRecord->m_tAchievementsEvaluator.RegisterListener(ptPlayerAchievementsRecord);
    ptPlayerAchievementsRecord->unk_178.RegisterListener(ptPlayerAchievementsRecord);
    D3::Achievements::Snapshot tSnapshot;
    tSnapshot.CopyFrom(tDefinition.console_data().achievement_snapshot());
    sub_7100468fe0(m_ptAchievements->GetStaticDataFile(), &tSnapshot);
    const int criteriaCount = tSnapshot.criteria_snapshot_size();
    for (int i = 0; i < criteriaCount; ++i) {
        const D3::Achievements::CriteriaUpdateRecord & criteriaUpdateRecord = tSnapshot.criteria_snapshot(i);
        if (criteriaUpdateRecord.achievement_set() == 0)
            ptPlayerAchievementsRecord->m_tAchievementsEvaluator.LoadCriteria(OnlineService::AchievementsEngine::ExtractCriteriaId64(criteriaUpdateRecord), criteriaUpdateRecord.quantity_32());
        if (criteriaUpdateRecord.achievement_set() == 1)
            ptPlayerAchievementsRecord->unk_178.LoadCriteria(OnlineService::AchievementsEngine::ExtractCriteriaId64(criteriaUpdateRecord), criteriaUpdateRecord.quantity_32());
    }
    const int achievementCount = tSnapshot.achievement_snapshot_size();
    for (int i = 0; i < achievementCount; ++i) {
        const D3::Achievements::AchievementUpdateRecord & achievementUpdateRecord = tSnapshot.achievement_snapshot(i);
        const bool fromDB = true;
        OnlineService::AchievementsEngine::AchievementUpdateRecord updateRecord;
        updateRecord.unk_0 = achievementUpdateRecord.achievement_set();
        updateRecord.m_achievementId = achievementUpdateRecord.achievement_id();
        updateRecord.m_completion = achievementUpdateRecord.completion();
        if (achievementUpdateRecord.achievement_set() == 0)
            ptPlayerAchievementsRecord->m_tAchievementsEvaluator.UpdateAchievement(updateRecord, fromDB);
        if (achievementUpdateRecord.achievement_set() == 1)
            ptPlayerAchievementsRecord->unk_178.UpdateAchievement(updateRecord, fromDB);
    }
    ptPlayerAchievementsRecord->m_tAchievementsEvaluator.EnableNotifications();
    ptPlayerAchievementsRecord->unk_178.EnableNotifications();
    return ptPlayerAchievementsRecord;
}






























inline ClientAchievementsInterface::ClientAchievementsInterface(Achievements * ptAchievements)
    : AchievementsInterfaceBase(ptAchievements) {
}






inline bool ClientAchievementsInterface::FindAwardedAchievement(int32 nUserIndex, uint32 achievementSet, OnlineService::AchievementId id, OnlineService::AchievementRecord & record) {
    SnapshotMap::iterator snapshotIt = m_snapshots.find(nUserIndex);
    if (snapshotIt == m_snapshots.end())
        return false;
    OnlineService::AchievementsEngine::ProgressData & tProgress = snapshotIt->second;
    OnlineService::AchievementsEngine::Achievement * ptAchievement = NULL;
    if (!tProgress.FindAchievement(achievementSet, id, ptAchievement))
        return false;
    ConvertAchievementRecord(*ptAchievement, record);
    return true;
}














bool ClientAchievementsInterface::FindAwardedAchievements(int32 nUserIndex, blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > & list) {
    SnapshotMap::iterator snapshotIt = m_snapshots.find(nUserIndex);
    if (snapshotIt == m_snapshots.end())
        return false;
    OnlineService::AchievementsEngine::ProgressData & tProgress = snapshotIt->second;
    for (size_t i = 0, end = tProgress.GetAchievementCount(); i < end; ++i)
    {   const OnlineService::AchievementsEngine::Achievement & tAchievement = tProgress.GetAchievement(i);
        ConvertAchievementRecord(tAchievement, list.emplace_back(tAchievement.unk_0)); }
    return true;
}











inline bool ClientAchievementsInterface::FindCriteriaRecord(int32 nUserIndex, uint32 achievementSet, OnlineService::CriteriaId criteriaId, OnlineService::CriteriaRecord & tRecord) {
    SnapshotMap::iterator snapshotIt = m_snapshots.find(nUserIndex);
    if (snapshotIt == m_snapshots.end())
        return false;
    OnlineService::AchievementsEngine::ProgressData & tProgress = snapshotIt->second;
    OnlineService::AchievementsEngine::Criteria * ptCriteria = NULL;
    if (!tProgress.FindCriteria(achievementSet, criteriaId, ptCriteria))
        return false;
    ConvertCriteriaRecord(*ptCriteria, tRecord);
    return true;
}














bool ClientAchievementsInterface::PopulateAccountAchievementSnapshot(int32 nUserIndex, D3::Account::SavedDefinition & tDefinition) {
    D3::Achievements::Snapshot & tProtoSnapshot = *tDefinition.mutable_console_data()->mutable_achievement_snapshot();
    tProtoSnapshot.Clear();
    SnapshotMap::iterator snapshotIt = m_snapshots.find(nUserIndex);
    if (snapshotIt == m_snapshots.end())
        return false;
    OnlineService::AchievementsEngine::ProgressData & tProgress = snapshotIt->second;
    tProgress.MergeToProtocol(tProtoSnapshot, true, true, true);
    return true;
}

















void ClientAchievementsInterface::OnAchievementAwarded(int32 nUserIndex, const D3::GameMessage::AchievementsAwardedConsole & tUpdateMsg) {
    SnapshotMap::iterator snapshotIt = m_snapshots.find(nUserIndex);
    if (snapshotIt == m_snapshots.end())
        return;
    OnlineService::AchievementsEngine::ProgressData & tProgress = snapshotIt->second;
    OnlineService::AchievementsEngine::Achievement * ptAchievement = NULL;
    for (OnlineService::AchievementId idAchievement : tUpdateMsg.achievements()) {
        if (tProgress.AllocateAchievement(tUpdateMsg.achievement_set(), idAchievement, ptAchievement)) {
            ptAchievement->m_achievementId = idAchievement;
            ptAchievement->m_completion = Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp());
        }
    }
}












void ClientAchievementsInterface::OnAchievementCriteriaUpdate(int32 nUserIndex, const D3::GameMessage::AchievementsCriteriaUpdateConsole & tUpdateMsg) {
    SnapshotMap::iterator snapshotIt = m_snapshots.find(nUserIndex);
    if (snapshotIt == m_snapshots.end())
        return;
    OnlineService::AchievementsEngine::ProgressData & tProgress = snapshotIt->second;
    for (const D3::GameMessage::AchievementsCriteriaUpdateConsole_Criteria & tUpdate : tUpdateMsg.criteria()) {
        OnlineService::CriteriaId idCriteria = OnlineService::AchievementsEngine::Make64BitCriteriaId(OnlineService::AchievementsEngine::ExtractCriteriaId32(tUpdate.id()));
        if (!tUpdate.deleted()) {
            OnlineService::AchievementsEngine::Criteria * ptCriteria = NULL;
            if (tProgress.AllocateTrackedCriteria(tUpdateMsg.achievement_set(), idCriteria, ptCriteria))
                ptCriteria->m_criteriaId = idCriteria;
            ptCriteria->m_baseQuantity = tUpdate.quantity();
        } else {
            tProgress.FreeCriteria(tUpdateMsg.achievement_set(), idCriteria);
        }
    }
}



















void ClientAchievementsInterface::OnUserSignedIn(int32 nUserIndex) {
    if (m_ptAchievements->m_bEnabled)
        LoadPlayerAchievements(nUserIndex);
}

















void ClientAchievementsInterface::LoadPlayerAchievements(int nUserIndex) {
    D3::Account::SavedDefinition & tDefinition = GamerProfile::GetAccount(nUserIndex); sub_7100468fe0(m_ptAchievements->GetStaticDataFile(), tDefinition.mutable_console_data()->mutable_achievement_snapshot());
    const D3::Achievements::Snapshot & tSnapshot = GamerProfile::GetAccount(nUserIndex).console_data().achievement_snapshot();
    m_snapshots.erase(nUserIndex);
    OnlineService::AchievementsEngine::ProgressData & tProgress = m_snapshots[nUserIndex];
    for (const D3::Achievements::AchievementUpdateRecord & tRecord : tSnapshot.achievement_snapshot()) {
        OnlineService::AchievementId idAchievement = tRecord.achievement_id();
        OnlineService::AchievementsEngine::Achievement * ptAchievement;
        tProgress.AllocateAchievement(tRecord.achievement_set(), idAchievement, ptAchievement);
        ptAchievement->m_achievementId = idAchievement;
        ptAchievement->m_completion = tRecord.completion();
    }
    for (const D3::Achievements::CriteriaUpdateRecord & tRecord : tSnapshot.criteria_snapshot()) {
        OnlineService::CriteriaId idCriteria = OnlineService::AchievementsEngine::ExtractCriteriaId64(tRecord);
        OnlineService::AchievementsEngine::Criteria * ptCriteria;
        tProgress.AllocateTrackedCriteria(tRecord.achievement_set(), idCriteria, ptCriteria);
        ptCriteria->m_criteriaId = idCriteria;
        ptCriteria->m_baseQuantity = tRecord.quantity_32();
    }
}

















// ConsoleAchievements.h:75 and :81.
bool Achievements::IsInitialized() {
    return true;
}

void Achievements::Process() {
}

// ConsoleAchievements.h:278 and :298.
void ClientAchievementsInterface::Process() {
}

bool ClientAchievementsInterface::FindAwardedAchievement(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::AchievementId id, OnlineService::AchievementRecord & record) {
    return FindAwardedAchievement(GameAccountToUserIndex(gameAccount), achievementSet, id, record);
}

bool ClientAchievementsInterface::FindCriteriaRecord(const OnlineService::GameAccountHandle & gameAccount, uint32 achievementSet, OnlineService::CriteriaId criteriaId, OnlineService::CriteriaRecord & record) {
    return FindCriteriaRecord(GameAccountToUserIndex(gameAccount), achievementSet, criteriaId, record);
}

bool ClientAchievementsInterface::FindAwardedAchievements(
    const OnlineService::GameAccountHandle & gameAccount,
    blz::vector<OnlineService::AchievementRecord, blz::allocator<OnlineService::AchievementRecord> > & list) {
    return FindAwardedAchievements(GameAccountToUserIndex(gameAccount), list);
}

}  // namespace Console
