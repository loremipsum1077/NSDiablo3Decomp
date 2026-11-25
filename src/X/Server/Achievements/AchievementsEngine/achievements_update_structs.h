// Original file: X/Server/Achievements/AchievementsEngine/achievements_update_structs.h
// Subset: the persisted record consumed by AchievementsInterface.
#pragma once
#include "X/Common/OnlineService/Achievements.h"
namespace OnlineService { namespace AchievementsEngine {
struct Achievement {  // size 0x18
    Achievement() : unk_0(0), m_achievementId(0), m_completion(0) {}
    Achievement(uint32 achievementSet) : unk_0(achievementSet), m_achievementId(0), m_completion(0) {}
    uint32 unk_0;  // 0x00 (2.7.7; the protobuf records call it achievement_set)
    OnlineService::AchievementId m_achievementId;  // 0x08
    uint64 m_completion;  // 0x10
};
static_assert(sizeof(Achievement) == 0x18, "Achievement");
struct InternalCriteriaDefinition;
struct CriteriaUpdateRecord {  // size 0x30
    CriteriaUpdateRecord();
    CriteriaUpdateRecord(uint32 achievementSet);
    uint32 unk_0;  // 0x00 (2.7.7; achievement_set)
    const OnlineService::AchievementsEngine::InternalCriteriaDefinition * m_criteria;  // 0x08
    uint64 m_startTime;  // 0x10
    uint64 m_updateTime;  // 0x18
    OnlineService::CriteriaQuantity m_quantity;  // 0x20
    uint32 m_flags;  // 0x28
};
static_assert(sizeof(CriteriaUpdateRecord) == 0x30, "CriteriaUpdateRecord");
// line 22
inline CriteriaUpdateRecord::CriteriaUpdateRecord()
    : unk_0(0), m_criteria(NULL), m_startTime(0), m_updateTime(0), m_quantity(0), m_flags(0) {}
inline CriteriaUpdateRecord::CriteriaUpdateRecord(uint32 achievementSet)
    : unk_0(achievementSet), m_criteria(NULL), m_startTime(0), m_updateTime(0), m_quantity(0), m_flags(0) {}
struct CriteriaDeleteRecord {  // size 0x10
    CriteriaDeleteRecord();
    CriteriaDeleteRecord(uint32 achievementSet);
    uint32 unk_0;  // 0x00 (2.7.7; achievement_set)
    const OnlineService::AchievementsEngine::InternalCriteriaDefinition * m_criteria;  // 0x08
};
static_assert(sizeof(CriteriaDeleteRecord) == 0x10, "CriteriaDeleteRecord");
inline CriteriaDeleteRecord::CriteriaDeleteRecord() : unk_0(0), m_criteria(NULL) {}
inline CriteriaDeleteRecord::CriteriaDeleteRecord(uint32 achievementSet) : unk_0(achievementSet), m_criteria(NULL) {}
typedef OnlineService::AchievementsEngine::Achievement AchievementUpdateRecord;
} }
