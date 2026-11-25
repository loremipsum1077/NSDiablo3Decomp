// Original file: X/Server/Achievements/AchievementsEngine/achievements_evaluator.h
// Subset: the AchievementsEvaluator layout (size 0xe8) and the public members
// Console::ServerAchievementsInterface uses. Its destructor is the implicit one
// (line 38), emitted with ConsoleAchievements.cpp.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Common/Achievements/AchievementsEngine/achievements_static_data.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../../Common/OnlineService/OnlineServiceErrorCodes.h"
#include "achievements_update_structs.h"
#include "../../../Common/Achievements/AchievementsEngine/achievements_flags.h"

namespace OnlineService { namespace AchievementsEngine { struct Criteria; } }
namespace OnlineService { namespace AchievementsEngine { struct InternalAchievementDefinition; } }
namespace OnlineService { namespace AchievementsEngine { struct InternalCriteriaDefinition; } }
namespace OnlineService { namespace AchievementsEngine { class Listener; } }
namespace OnlineService { namespace AchievementsEngine { class ProgressData; } }

namespace OnlineService {
namespace AchievementsEngine {
typedef OnlineService::Callback<bool (unsigned long, unsigned long)> CheckEventCallback;

typedef blz::unordered_map<unsigned long, OnlineService::AchievementsEngine::Criteria *, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, OnlineService::AchievementsEngine::Criteria *> > > TrackedCriteriaMap;

typedef blz::unordered_map<unsigned long, OnlineService::AchievementsEngine::Achievement *, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, OnlineService::AchievementsEngine::Achievement *> > > TrackedAchievementMap;

class AchievementsEvaluator {  // size 0xf0
public:
    AchievementsEvaluator(uint32, const OnlineService::AchievementsEngine::AchievementsStaticData *, OnlineService::AchievementsEngine::ProgressData *, uint32);
    bool CheckEvent(OnlineService::CriteriaEventId eventId, OnlineService::CriteriaComparand eventComparand, OnlineService::CriteriaQuantity quantity, uint64 updateTimeSeconds, int32 nUnk, const OnlineService::AchievementsEngine::CheckEventCallback & cb);
    void UpdateStaticData(const OnlineService::AchievementsEngine::AchievementsStaticData *);
    void SetActivePartitionsMask(uint32);
    void LoadCriteria(OnlineService::CriteriaId idCriteria, OnlineService::CriteriaQuantity uQuantity);
    OnlineService::ErrorCode UpdateCriteria(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteriaUpdate, bool fromDB);
    void UpdateAchievement(const OnlineService::AchievementsEngine::AchievementUpdateRecord & achievementUpdate, bool fromDB);
    void Flush();
    void OnFlushCriteria(const OnlineService::AchievementsEngine::CriteriaUpdateRecord &);
    void RegisterListener(blz::shared_ptr<OnlineService::AchievementsEngine::Listener> listener);
    bool UnregisterListener(blz::shared_ptr<OnlineService::AchievementsEngine::Listener> listener);
    void SetStateListener(OnlineService::AchievementsEngine::Listener *);
    void EnableNotifications();
    void SendAccumulatedUpdates();
    Blizzard::Time::Microsecond GetLastFlushTime();
    bool FlushImmediatelyIfSignaled() { return m_flushImmediately; }  // line 85
    blz::vector<unsigned long, blz::allocator<unsigned long> > & GetFailedCriteria();
    void EvaluateAwardAchievement(const OnlineService::AchievementsEngine::InternalAchievementDefinition * staticAchievement, uint64 updateTimeSeconds);
    void EvaluateMetaAchievement(OnlineService::AchievementId achievementId);
    OnlineService::AchievementsEngine::ProgressData & GetSnapshot();
    const OnlineService::AchievementsEngine::AchievementsStaticData * GetStaticData();
    enum AchievementsEventType {
        ADVANCE_EVENT = 0,
        START_EVENT = 1,
        FAIL_EVENT = 2,
    };
private:
    void CheckEventMap(OnlineService::AchievementsEngine::EventKey key, const OnlineService::AchievementsEngine::PartitionedCriteriaEventMap & eventMap, uint64 quantity, uint64 updateTimeSeconds, const OnlineService::AchievementsEngine::CheckEventCallback & cb);
    void CheckEventMap(OnlineService::AchievementsEngine::EventKey key, const OnlineService::AchievementsEngine::CriteriaEventMap & eventMap, uint64 quantity, uint64 updateTimeSeconds, const OnlineService::AchievementsEngine::CheckEventCallback & cb);
    bool EvaluateModifiers(const OnlineService::AchievementsEngine::InternalCriteriaDefinition *, const OnlineService::CriteriaEvent & event, const OnlineService::AchievementsEngine::CheckEventCallback & cb);
    bool ShouldProcessCriteria(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteriaUpdate, const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteria, const OnlineService::AchievementsEngine::InternalAchievementDefinition * achievement) const;
    bool HasSatisfiedCriteria(OnlineService::AchievementsEngine::Criteria * trackedCriteria, const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteriaDefinition) const;
    OnlineService::CriteriaQuantity GetCriteriaQuantity(OnlineService::AchievementsEngine::Criteria * trackedCriteria) const;
    void DeleteAchievementTrackedCriteria(const OnlineService::AchievementsEngine::InternalAchievementDefinition * staticAchievement);
    void DeleteTrackedCriteria(OnlineService::CriteriaId id, const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteria);
    void FreeTrackedCriteria(const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteria);
    void StartCriteria(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteriaUpdate, const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteria, bool fromDB);
    void FailCriteria(const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteria, uint32 flags, uint64 updateTimeSeconds, bool fromDB, bool forceFail);
    void FailAllCriteria(const OnlineService::AchievementsEngine::InternalAchievementDefinition * staticAchievement, uint64 updateTimeSeconds, bool fromDB);
    void UpdateCriteriaQuantity(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteriaUpdate, const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteria, bool fromDB);
    void PropagateSharedCriteriaDBValue(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteriaUpdate, const OnlineService::AchievementsEngine::InternalCriteriaDefinition * criteria, bool fromDB);
    void InformListeners(const OnlineService::AchievementsEngine::CriteriaUpdateRecord & criteriaUpdate) const;
    void InformListeners(const OnlineService::AchievementsEngine::CriteriaDeleteRecord & criteriaDeleted) const;
    void InformListeners(const OnlineService::AchievementsEngine::AchievementUpdateRecord & achievementUpdate) const;
    void MarkDirty(OnlineService::AchievementsEngine::Criteria * trackedCriteria, const OnlineService::AchievementsEngine::InternalCriteriaDefinition * staticCriteria) const;
    void MarkDirty(OnlineService::AchievementsEngine::Achievement * trackedAchievement, const OnlineService::AchievementsEngine::InternalAchievementDefinition *) const;
    void MarkClean(OnlineService::CriteriaId id) const;
    void AccumulateAchievementCriteriaQuantity(const OnlineService::AchievementsEngine::InternalAchievementDefinition * staticAchievement, const OnlineService::AchievementsEngine::AchievementStaticFlags accumulationFlags, uint64 & numCriteriaExceeded, uint64 & totalCriteria);
    void EvaluateAccumulatedAchievementCriteriaQuantity(const OnlineService::AchievementsEngine::InternalAchievementDefinition * staticAchievement, uint64 & numCriteriaExceeded, uint64 & totalCriteria, uint64 updateTimeSeconds);
    void FlushAchievements();
    void FlushCriteria();
    void SignalImmediateFlush() const;
    void UpdateAchievementInternal(const OnlineService::AchievementsEngine::InternalAchievementDefinition * staticAchievement, const OnlineService::AchievementsEngine::Achievement * achievement, const bool fromDB);
    uint64 GetSeconds() const;
    uint32 unk_0;  // 0x00 (2.7.7: first constructor argument, the achievement set this evaluator serves)
    const OnlineService::AchievementsEngine::AchievementsStaticData * m_staticData;  // 0x08
    uint32 m_activePartitionsMask;  // 0x10
    OnlineService::AchievementsEngine::ProgressData * m_snapshot;  // 0x18
    mutable OnlineService::AchievementsEngine::TrackedCriteriaMap m_dirtyCriteria;  // 0x20
    mutable OnlineService::AchievementsEngine::TrackedAchievementMap m_dirtyAchievements;  // 0x40
    typedef blz::unordered_map<OnlineService::AchievementsEngine::EventKey, bool, OnlineService::AchievementsEngine::EventKeyHash, blz::equal_to<OnlineService::AchievementsEngine::EventKey>, blz::allocator<blz::pair<const OnlineService::AchievementsEngine::EventKey, bool> > > HintMap;
    OnlineService::AchievementsEngine::AchievementsEvaluator::HintMap m_subjetModifierHintMap;  // 0x60
    OnlineService::AchievementsEngine::AchievementsEvaluator::HintMap m_objectModifierHintMap;  // 0x80
    typedef blz::vector<blz::shared_ptr<OnlineService::AchievementsEngine::Listener>, blz::allocator<blz::shared_ptr<OnlineService::AchievementsEngine::Listener> > > ListenerVector;
    OnlineService::AchievementsEngine::AchievementsEvaluator::ListenerVector m_listeners;  // 0xa0
    OnlineService::AchievementsEngine::Listener * m_stateListener;  // 0xb8
    mutable bool m_flushImmediately;  // 0xc0
    OnlineService::AchievementsEngine::AchievementsEvaluator::AchievementsEventType m_currentEventType;  // 0xc4
    Blizzard::Time::Microsecond m_lastFlushTime;  // 0xc8
    mutable bool m_checkEventHadSideEffects;  // 0xd0
    bool m_notificationsEnabled;  // 0xd1
    blz::vector<unsigned long, blz::allocator<unsigned long> > m_failedCriteria;  // 0xd8
};
static_assert(sizeof(AchievementsEvaluator) == 0xf0, "AchievementsEvaluator");

}  // namespace AchievementsEngine
}  // namespace OnlineService
