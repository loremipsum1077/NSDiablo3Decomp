// Adopted DWARF declarations: static achievement record layouts and query API.
// Original file: X/Common/Achievements/AchievementsEngine/achievements_static_data.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/set.h"
#include "../../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../OnlineService/Achievements.h"
#include "../../../../BlizzardCore/Include/Blizzard/Util/Hash.h"

#include <stddef.h>
#include <stdint.h>

namespace D3 { namespace AchievementsStaticData { class AchievementFile; } }
namespace D3 { namespace AchievementsStaticData { class CriteriaEvent; } }
namespace D3 { namespace AchievementsStaticData { class StaticAchievementDefinition; } }
namespace D3 { namespace AchievementsStaticData { class StaticCategoryDefinition; } }
namespace D3 { namespace AchievementsStaticData { class StaticCriteriaDefinition; } }
namespace OnlineService { namespace AchievementsEngine { struct InternalAchievementDefinition; } }
namespace OnlineService { namespace AchievementsEngine { struct InternalCategoryDefinition; } }
namespace OnlineService { namespace AchievementsEngine { struct InternalCriteriaDefinition; } }
namespace OnlineService { namespace AchievementsEngine { struct InternalRewardDefinition; } }

namespace OnlineService {
namespace AchievementsEngine {
struct EventKey {  // size 0x18
    EventKey(uint64, uint64, bool);
    EventKey();
    uint64 m_condition;  // 0x00
    uint64 m_comparand;  // 0x08
    bool m_hasComparand;  // 0x10
};
static_assert(sizeof(EventKey) == 0x18, "EventKey");

struct EventKeyHash {  // size 0x1
    size_t operator()(const OnlineService::AchievementsEngine::EventKey & key);
};
static_assert(sizeof(EventKeyHash) == 0x1, "EventKeyHash");
inline EventKey::EventKey(uint64 condition, uint64 comperand, bool hasComperand)
    : m_condition(condition), m_comparand(comperand), m_hasComparand(hasComperand) {}
inline size_t EventKeyHash::operator()(const OnlineService::AchievementsEngine::EventKey & key) {
    size_t hash = Blizzard::Util::Hash32(&key.m_condition, sizeof(key.m_condition));
    hash ^= Blizzard::Util::Hash32(&key.m_comparand, sizeof(key.m_comparand));
    hash ^= Blizzard::Util::Hash32(&key.m_hasComparand, sizeof(key.m_hasComparand));
    return hash;
}
inline bool operator==(const OnlineService::AchievementsEngine::EventKey & lhs, const OnlineService::AchievementsEngine::EventKey & rhs) {
    return lhs.m_condition == rhs.m_condition && lhs.m_comparand == rhs.m_comparand && lhs.m_hasComparand == rhs.m_hasComparand;
}

typedef blz::set<unsigned int, blz::less<unsigned int>, blz::allocator<unsigned int> > EvaluatorClassSet;

typedef blz::unordered_map<unsigned long, OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, OnlineService::AchievementsEngine::InternalCriteriaDefinition *> > > CriteriaMap;

typedef blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> > CriteriaVector;

typedef blz::unordered_map<unsigned long, OnlineService::AchievementsEngine::InternalAchievementDefinition *, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, OnlineService::AchievementsEngine::InternalAchievementDefinition *> > > AchievementMap;

typedef blz::unordered_map<unsigned int, OnlineService::AchievementsEngine::InternalCategoryDefinition *, blz::hash<unsigned int>, blz::equal_to<unsigned int>, blz::allocator<blz::pair<const unsigned int, OnlineService::AchievementsEngine::InternalCategoryDefinition *> > > CategoryMap;

typedef blz::unordered_map<unsigned int, OnlineService::AchievementsEngine::InternalRewardDefinition *, blz::hash<unsigned int>, blz::equal_to<unsigned int>, blz::allocator<blz::pair<const unsigned int, OnlineService::AchievementsEngine::InternalRewardDefinition *> > > RewardMap;

typedef blz::unordered_map<OnlineService::AchievementsEngine::EventKey, blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> >, OnlineService::AchievementsEngine::EventKeyHash, blz::equal_to<OnlineService::AchievementsEngine::EventKey>, blz::allocator<blz::pair<const OnlineService::AchievementsEngine::EventKey, blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> > > > > CriteriaEventMap;

typedef blz::vector<blz::unordered_map<OnlineService::AchievementsEngine::EventKey, blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> >, OnlineService::AchievementsEngine::EventKeyHash, blz::equal_to<OnlineService::AchievementsEngine::EventKey>, blz::allocator<blz::pair<const OnlineService::AchievementsEngine::EventKey, blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> > > > >, blz::allocator<blz::unordered_map<OnlineService::AchievementsEngine::EventKey, blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> >, OnlineService::AchievementsEngine::EventKeyHash, blz::equal_to<OnlineService::AchievementsEngine::EventKey>, blz::allocator<blz::pair<const OnlineService::AchievementsEngine::EventKey, blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> > > > > > > PartitionedCriteriaEventMap;

typedef blz::map<unsigned int, unsigned long, blz::less<unsigned int>, blz::allocator<blz::pair<const unsigned int, unsigned long> > > SeasonJourneyRankTailsMap;

typedef blz::vector<blz::unordered_map<unsigned long, unsigned long, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, unsigned long> > >, blz::allocator<blz::unordered_map<unsigned long, unsigned long, blz::hash<unsigned long>, blz::equal_to<unsigned long>, blz::allocator<blz::pair<const unsigned long, unsigned long> > > > > SeasonJourneyObjective;

class AchievementsStaticData {  // size 0x1c8
public:
    AchievementsStaticData(const blz::string &);
    ~AchievementsStaticData();
    bool ParseFromArray(const void *, int);
    void ParseFromPb(const D3::AchievementsStaticData::AchievementFile & achievementFile);
    const blz::string & GetContentHandleHash() const;
    OnlineService::AchievementsEngine::InternalAchievementDefinition * GetSeriesHead(OnlineService::AchievementsEngine::InternalAchievementDefinition * achievement);
    const OnlineService::AchievementsEngine::CategoryMap & GetCategoryMap() const;
    const OnlineService::AchievementsEngine::AchievementMap & GetAchievementMap() const { return m_achievements; } // line 110
    const OnlineService::AchievementsEngine::CriteriaMap & GetCriteriaMap() const;
    const OnlineService::AchievementsEngine::RewardMap & GetRewardMap() const;
    OnlineService::AchievementsEngine::RewardMap & GetRewardMap() { return m_rewards; } // line 115
    const OnlineService::AchievementsEngine::PartitionedCriteriaEventMap & GetStartEvents() const;
    const OnlineService::AchievementsEngine::PartitionedCriteriaEventMap & GetFailEvents() const;
    const OnlineService::AchievementsEngine::PartitionedCriteriaEventMap & GetAdvanceEvents() const;
    const OnlineService::AchievementsEngine::SeasonJourneyRankTailsMap & GetSeasonJourneyRankTails() const;
    OnlineService::AchievementsEngine::SeasonJourneyRankTailsMap & GetSeasonJourneyRankTails() { return m_seasonJourneyRankTails; }
    const OnlineService::AchievementsEngine::SeasonJourneyObjective & GetSeasonJourneyObjectiveArray() const { return m_arSeasonJourneyObjectives; }
    OnlineService::AchievementsEngine::SeasonJourneyObjective & GetSeasonJourneyObjectiveArray() { return m_arSeasonJourneyObjectives; } // line 125
    const OnlineService::AchievementsEngine::InternalCriteriaDefinition * GetCriteriaDefinition(uint64 id) const;
    const OnlineService::AchievementsEngine::InternalAchievementDefinition * GetAchievementDefinition(OnlineService::AchievementId id) const;
    const OnlineService::AchievementsEngine::InternalCategoryDefinition * GetCategoryDefinition(OnlineService::CategoryId id) const;
    const OnlineService::AchievementsEngine::InternalRewardDefinition * GetRewardDefinition(OnlineService::RewardId id) const;
    void InitializeCriteriaEvent(OnlineService::CriteriaEvent & outCriteriaEvent) const;
    blz::string GetTimestamp() const;
    int32 GetSeasonCycleLength() const { return m_seasonCycleLength; } // line 140
private:
    void AssociateData();
    void CopyCriteriaEventFromProtocol(OnlineService::CriteriaEvent & outCriteriaEvent, const D3::AchievementsStaticData::CriteriaEvent & criteriaEvent) const;
    OnlineService::AchievementsEngine::InternalCriteriaDefinition * CreateInternalCriteriaDefinitionFromProtocol(const D3::AchievementsStaticData::StaticCriteriaDefinition & criteriaDefinition) const;
    OnlineService::AchievementsEngine::InternalAchievementDefinition * CreateInternalAchievementDefinitionFromProtocol(const D3::AchievementsStaticData::StaticAchievementDefinition & achievementDefinition) const;
    OnlineService::AchievementsEngine::InternalCategoryDefinition * CreateInternalCategoryDefinitionFromProtocol(const D3::AchievementsStaticData::StaticCategoryDefinition & categoryDefinition) const;
    blz::string m_contentHandleHash;  // 0x00
    OnlineService::AchievementsEngine::CategoryMap m_categories;  // 0x28
    OnlineService::AchievementsEngine::AchievementMap m_achievements;  // 0x48
    OnlineService::AchievementsEngine::CriteriaMap m_criteria;  // 0x68
    OnlineService::AchievementsEngine::RewardMap m_rewards;  // 0x88
    OnlineService::AchievementsEngine::EvaluatorClassSet m_evaluationScope;  // 0xa8
    OnlineService::AchievementsEngine::PartitionedCriteriaEventMap m_startEvents;  // 0xd0
    OnlineService::AchievementsEngine::PartitionedCriteriaEventMap m_failEvents;  // 0xe8
    OnlineService::AchievementsEngine::PartitionedCriteriaEventMap m_advanceEvents;  // 0x100
    OnlineService::AchievementsEngine::SeasonJourneyRankTailsMap m_seasonJourneyRankTails;  // 0x118
    OnlineService::AchievementsEngine::SeasonJourneyObjective m_arSeasonJourneyObjectives;  // 0x140
    blz::string m_timestamp;  // 0x158
    uint8 unk_180[0x40];  // 0x180 (2.7.7: 0x40 bytes before m_seasonCycleLength, read at +0x1c0; contents unknown)
    int32 m_seasonCycleLength;  // 0x1c0
};
static_assert(sizeof(AchievementsStaticData) == 0x1c8, "AchievementsStaticData");
inline const OnlineService::AchievementsEngine::CriteriaMap & AchievementsStaticData::GetCriteriaMap() const { return m_criteria; }
inline const OnlineService::AchievementsEngine::PartitionedCriteriaEventMap & AchievementsStaticData::GetStartEvents() const { return m_startEvents; }
inline const OnlineService::AchievementsEngine::PartitionedCriteriaEventMap & AchievementsStaticData::GetFailEvents() const { return m_failEvents; }
inline const OnlineService::AchievementsEngine::PartitionedCriteriaEventMap & AchievementsStaticData::GetAdvanceEvents() const { return m_advanceEvents; }

}  // namespace AchievementsEngine
}  // namespace OnlineService

// Functions defined in this file (bodies are not in the DWARF):
//   line    27  OnlineService::AchievementsEngine::EventKey::EventKey(uint64 condition, uint64 comperand, bool hasComperand)  [inlined into 3 functions]
//   line    48  size_t OnlineService::AchievementsEngine::EventKeyHash::operator()(const OnlineService::AchievementsEngine::EventKey & key)  [inlined into 7 functions]
//   line    58  bool OnlineService::AchievementsEngine::operator==(const OnlineService::AchievementsEngine::EventKey & lhs, const OnlineService::AchievementsEngine::EventKey & rhs)  [inlined into 5 functions]
//   line   110  const OnlineService::AchievementsEngine::AchievementMap & OnlineService::AchievementsEngine::AchievementsStaticData::GetAchievementMap() const  [inline]
//   line   115  OnlineService::AchievementsEngine::RewardMap & OnlineService::AchievementsEngine::AchievementsStaticData::GetRewardMap()  [inlined into 1 functions]
//   line   125  OnlineService::AchievementsEngine::SeasonJourneyObjective & OnlineService::AchievementsEngine::AchievementsStaticData::GetSeasonJourneyObjectiveArray()  [inlined into 1 functions]
//   line   139  blz::string OnlineService::AchievementsEngine::AchievementsStaticData::GetTimestamp() const  [inline]
//   line   140  int32 OnlineService::AchievementsEngine::AchievementsStaticData::GetSeasonCycleLength() const  [inlined into 8 functions]
