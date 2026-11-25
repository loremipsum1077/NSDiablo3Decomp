// Original file: X/Server/Achievements/AchievementsEngine/achievements_progress_data.h
// Subset: the persisted record consumed by AchievementsInterface.
#pragma once
#include "X/Common/OnlineService/Achievements.h"
#include "../../../../Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
namespace D3 { namespace Achievements { class Snapshot; } }
namespace OnlineService { namespace AchievementsEngine {
enum TrackedCriteriaFlags {
    TRACKED_CRITERIA_FLAG_FAILED = 1,
    TRACKED_CRITERIA_FLAG_STARTED = 2,
    TRACKED_CRITERIA_FLAG_SERIALIZED = 4,
    TRACKED_CRITERIA_FLAG_DELETE_FROM_DB = 8,
    TRACKED_CRITERIA_FLAG_CHEAT = 16,
};
struct Criteria {  // size 0x38
    Criteria(uint32 achievementSet)  // line 43 (2.7.7: takes the achievement set)
        : unk_0(achievementSet), m_criteriaId(0), m_baseQuantity(0), m_deltaQuantity(0), m_startTime(0), m_updateTime(0), m_flags(0) {}
    uint32 unk_0;  // 0x00 (2.7.7; achievement_set)
    OnlineService::CriteriaId m_criteriaId;  // 0x08
    OnlineService::CriteriaQuantity m_baseQuantity;  // 0x10
    OnlineService::CriteriaQuantity m_deltaQuantity;  // 0x18
    uint64 m_startTime;  // 0x20
    uint64 m_updateTime;  // 0x28
    uint32 m_flags;  // 0x30
};
static_assert(sizeof(Criteria) == 0x38, "Criteria");
struct Achievement;
class ProgressData {  // size 0x70
public:
    size_t GetAchievementCount() const { return m_achievements.size(); }  // line 67
    const OnlineService::AchievementsEngine::Achievement & GetAchievement(size_t index) const { return *m_achievements[index]; }  // line 68
    bool FindAchievement(uint32 achievementSet, uint64 id, OnlineService::AchievementsEngine::Achievement *& ptr) const;
    bool AllocateAchievement(uint32 achievementSet, OnlineService::AchievementId id, OnlineService::AchievementsEngine::Achievement *& ptr);
    bool FreeAchievement(uint32 achievementSet, OnlineService::AchievementId id);
    size_t GetCriteriaCount() const { return m_criteria.size(); }  // line 78
    const OnlineService::AchievementsEngine::Criteria & GetCriteria(size_t index) const { return *m_criteria[index]; }  // line 79
    bool FindCriteria(uint32 achievementSet, uint64 id, OnlineService::AchievementsEngine::Criteria *& ptr) const;
    bool AllocateTrackedCriteria(uint32 achievementSet, OnlineService::CriteriaId id, OnlineService::AchievementsEngine::Criteria *& ptr);
    bool FreeCriteria(uint32 achievementSet, OnlineService::CriteriaId id);
    void MergeToProtocol(D3::Achievements::Snapshot & snapshot, bool includeAchievements, bool includeCriteria, bool) const;
private:
    void ClearCriteria();
    void ClearAchievements();
    typedef blz::vector<OnlineService::AchievementsEngine::Achievement *> AchievementVector;
    AchievementVector m_achievements;  // 0x00
    typedef blz::unordered_map<unsigned long, blz::shared_ptr<OnlineService::AchievementsEngine::Achievement> > AchievementMap;
    typedef blz::unordered_map<unsigned int, AchievementMap> AchievementSetMap;  // 2.7.7: keyed by achievement set, then id
    AchievementSetMap m_achievementsById;  // 0x18
    typedef blz::vector<OnlineService::AchievementsEngine::Criteria *> CriteriaVector;
    CriteriaVector m_criteria;  // 0x38
    typedef blz::unordered_map<unsigned long, blz::shared_ptr<OnlineService::AchievementsEngine::Criteria> > CriteriaMap;
    typedef blz::unordered_map<unsigned int, CriteriaMap> CriteriaSetMap;  // 2.7.7: keyed by achievement set, then id
    CriteriaSetMap m_criteriaById;  // 0x50
};
static_assert(sizeof(ProgressData) == 0x70, "ProgressData");
} }
