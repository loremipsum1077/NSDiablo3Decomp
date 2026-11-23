// Adopted DWARF declarations: static achievement record layouts and query API.
// Original file: X/Common/Achievements/AchievementsEngine/achievements_internal_static_data_structs.h

#pragma once

#include "../../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../OnlineService/Achievements.h"


namespace OnlineService {
namespace AchievementsEngine {
struct InternalAchievementDefinition;
struct InternalCriteriaDefinition : public OnlineService::CriteriaDefinition {  // size 0x118
    OnlineService::AchievementsEngine::InternalCriteriaDefinition * m_proxyCriteria;  // 0x98
    OnlineService::AchievementsEngine::InternalAchievementDefinition * m_parentAchievement;  // 0xa0
    blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> > m_sharingWithCriteria;  // 0xa8
    bool m_sharingThisCriteria;  // 0xc0
    blz::string m_nameKey;  // 0xc8
    blz::string m_stringlabelKey;  // 0xf0
};
static_assert(sizeof(InternalCriteriaDefinition) == 0x118, "InternalCriteriaDefinition");

struct InternalAchievementDefinition : public OnlineService::AchievementDefinition {  // size 0x128
    blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCriteriaDefinition *> > m_criteriaList;  // 0x78
    blz::vector<OnlineService::AchievementsEngine::InternalAchievementDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalAchievementDefinition *> > m_sharedChildrenAchievements;  // 0x90
    OnlineService::AchievementsEngine::InternalAchievementDefinition * m_seriesTailId;  // 0xa8
    blz::string m_nameKey;  // 0xb0
    blz::string m_iconTexture;  // 0xd8
    blz::string m_descriptionKey;  // 0x100
    OnlineService::AchievementsEngine::InternalAchievementDefinition & operator=(const OnlineService::AchievementsEngine::InternalAchievementDefinition &);
};
static_assert(sizeof(InternalAchievementDefinition) == 0x128, "InternalAchievementDefinition");

struct InternalCategoryDefinition : public OnlineService::CategoryDefinition {  // size 0x70
    blz::vector<OnlineService::AchievementsEngine::InternalCategoryDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalCategoryDefinition *> > m_childCategories;  // 0x18
    blz::vector<OnlineService::AchievementsEngine::InternalAchievementDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalAchievementDefinition *> > m_childAchievements;  // 0x30
    blz::string m_nameKey;  // 0x48
    OnlineService::AchievementsEngine::InternalCategoryDefinition & operator=(const OnlineService::AchievementsEngine::InternalCategoryDefinition &);
};
static_assert(sizeof(InternalCategoryDefinition) == 0x70, "InternalCategoryDefinition");

struct InternalRewardDefinition : public OnlineService::RewardDefinition {  // size 0x68
    blz::vector<OnlineService::AchievementsEngine::InternalAchievementDefinition *, blz::allocator<OnlineService::AchievementsEngine::InternalAchievementDefinition *> > m_parentAchievements;  // 0x28
    blz::string m_nameKey;  // 0x40
};
static_assert(sizeof(InternalRewardDefinition) == 0x68, "InternalRewardDefinition");

}  // namespace AchievementsEngine
}  // namespace OnlineService

// Functions defined in this file (bodies are not in the DWARF):
//   line    27  OnlineService::AchievementsEngine::InternalCriteriaDefinition::InternalCriteriaDefinition()  [inlined into 1 functions]
//   line    27  OnlineService::AchievementsEngine::InternalCriteriaDefinition::~InternalCriteriaDefinition()  [inlined into 1 functions]
//   line    39  OnlineService::AchievementsEngine::InternalAchievementDefinition::InternalAchievementDefinition()  [inlined into 1 functions]
//   line    39  OnlineService::AchievementsEngine::InternalAchievementDefinition::~InternalAchievementDefinition()  [inlined into 1 functions]
//   line    59  OnlineService::AchievementsEngine::InternalCategoryDefinition::InternalCategoryDefinition()  [inlined into 2 functions]
//   line    59  OnlineService::AchievementsEngine::InternalCategoryDefinition::~InternalCategoryDefinition()  [inlined into 1 functions]
//   line    76  OnlineService::AchievementsEngine::InternalRewardDefinition::InternalRewardDefinition()  [inlined into 1 functions]
//   line    76  OnlineService::AchievementsEngine::InternalRewardDefinition::~InternalRewardDefinition()  [inlined into 1 functions]
