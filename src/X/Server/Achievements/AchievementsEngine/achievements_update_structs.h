// Original file: X/Server/Achievements/AchievementsEngine/achievements_update_structs.h
// Subset: the persisted record consumed by AchievementsInterface.
#pragma once
#include "X/Common/OnlineService/Achievements.h"
namespace OnlineService { namespace AchievementsEngine {
struct Achievement {  // size 0x10
    OnlineService::AchievementId m_achievementId;  // 0x00
    uint64 m_completion;  // 0x08
};
static_assert(sizeof(Achievement) == 0x10, "Achievement");
struct InternalCriteriaDefinition;
struct CriteriaUpdateRecord {  // size 0x28
    CriteriaUpdateRecord();
    const OnlineService::AchievementsEngine::InternalCriteriaDefinition * m_criteria;  // 0x00
    uint64 m_startTime;  // 0x08
    uint64 m_updateTime;  // 0x10
    OnlineService::CriteriaQuantity m_quantity;  // 0x18
    uint32 m_flags;  // 0x20
};
static_assert(sizeof(CriteriaUpdateRecord) == 0x28, "CriteriaUpdateRecord");
// line 22
inline CriteriaUpdateRecord::CriteriaUpdateRecord()
    : m_criteria(NULL), m_startTime(0), m_updateTime(0), m_quantity(0), m_flags(0) {}
struct CriteriaDeleteRecord {  // size 0x8
    CriteriaDeleteRecord();
    const OnlineService::AchievementsEngine::InternalCriteriaDefinition * m_criteria;  // 0x00
};
static_assert(sizeof(CriteriaDeleteRecord) == 0x8, "CriteriaDeleteRecord");
inline CriteriaDeleteRecord::CriteriaDeleteRecord() : m_criteria(NULL) {}
typedef OnlineService::AchievementsEngine::Achievement AchievementUpdateRecord;
} }
