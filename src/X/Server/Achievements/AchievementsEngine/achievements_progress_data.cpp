// Original file: X/Server/Achievements/AchievementsEngine/achievements_progress_data.cpp
#include "achievements_progress_data.h"
#include "achievements_update_structs.h"
#include "../../../Common/Achievements/AchievementsEngine/achievements_types.h"
#include "../../../Common/Protobuf/Achievements.pb.h"

namespace OnlineService {
namespace AchievementsEngine {








void ProgressData::MergeToProtocol(D3::Achievements::Snapshot & snapshot, bool includeAchievements, bool includeCriteria, bool) const
{
    // the last parameter is unused



    if (includeAchievements)
    {
        const size_t achievementCount = m_achievements.size();
        for (size_t i = 0; i < achievementCount; ++i)
        {



            D3::Achievements::AchievementUpdateRecord * record = snapshot.add_achievement_snapshot();
            const Achievement * achievement = m_achievements[i];
            record->set_achievement_set(achievement->unk_0);
            record->set_achievement_id(achievement->m_achievementId);
            record->set_completion(achievement->m_completion);
        }
    }

    if (includeCriteria)
    {
        const size_t criteriaCount = m_criteria.size();
        for (size_t i = 0; i < criteriaCount; ++i)
        {
            const Criteria * criteria = m_criteria[i];



            if (criteria->m_startTime != 0)
            {
                continue;
            }




            D3::Achievements::CriteriaUpdateRecord * record = snapshot.add_criteria_snapshot();
            record->set_criteria_id_32_and_flags_8(ExtractCriteriaId32(criteria->m_criteriaId));
            record->set_achievement_set(criteria->unk_0);


            if (criteria->m_startTime != 0)
            {
                record->set_start_time_32(criteria->m_startTime);
            }

            const CriteriaQuantity quantity = criteria->m_baseQuantity + criteria->m_deltaQuantity;
            if (quantity != 0)
            {
                record->set_quantity_32(quantity);
            }
        }
    }
}
bool ProgressData::FindCriteria(uint32 achievementSet, uint64 id, Criteria *& ptr) const
{
    CriteriaSetMap::const_iterator itSet = m_criteriaById.find(achievementSet);
    if (itSet == m_criteriaById.end())
    {
        return false;
    }
    CriteriaMap::const_iterator it = itSet->second.find(id);
    const bool found = it != itSet->second.end();
    if (found)
    {
        ptr = it->second.get();
    }
    return found;
}
bool ProgressData::AllocateTrackedCriteria(uint32 achievementSet, CriteriaId id, Criteria *& ptr)
{
    CriteriaSetMap::const_iterator itSet = m_criteriaById.find(achievementSet);
    const bool shouldAllocate = itSet == m_criteriaById.end() || itSet->second.find(id) == itSet->second.end();
    if (shouldAllocate)
    {
        blz::shared_ptr<Criteria> criteria = blz::make_shared<Criteria>(achievementSet);
        ptr = criteria.get();
        m_criteriaById[achievementSet][id].swap(criteria);
        m_criteria.push_back(ptr);
    }
    else
    {
        ptr = itSet->second.find(id)->second.get();
    }
    return shouldAllocate;
}
bool ProgressData::FreeCriteria(uint32 achievementSet, CriteriaId id)
{
    CriteriaSetMap::iterator itSet = m_criteriaById.find(achievementSet);
    if (itSet == m_criteriaById.end())
    {
        return false;
    }
    CriteriaMap::iterator it = itSet->second.find(id);
    const bool found = it != itSet->second.end();
    if (found)
    {
        for (CriteriaVector::iterator itCrit = m_criteria.begin();
             itCrit != m_criteria.end(); ++itCrit)
        {
            if ((*itCrit)->unk_0 == achievementSet && (*itCrit)->m_criteriaId == id)
            {
                m_criteria.erase(itCrit);
                break;
            }
        }
        itSet->second.erase(id);
        if (itSet->second.empty())
        {
            m_criteriaById.erase(itSet);
        }
    }
    return found;
}
bool ProgressData::FindAchievement(uint32 achievementSet, uint64 id, Achievement *& ptr) const
{
    AchievementSetMap::const_iterator itSet = m_achievementsById.find(achievementSet);
    if (itSet == m_achievementsById.end())
    {
        return false;
    }
    AchievementMap::const_iterator it = itSet->second.find(id);
    const bool found = it != itSet->second.end();
    if (found)
    {
        ptr = it->second.get();
    }
    return found;
}
bool ProgressData::AllocateAchievement(uint32 achievementSet, AchievementId id, Achievement *& ptr)
{
    AchievementSetMap::const_iterator itSet = m_achievementsById.find(achievementSet);
    const bool shouldAllocate = itSet == m_achievementsById.end() || itSet->second.find(id) == itSet->second.end();
    if (shouldAllocate)
    {
        blz::shared_ptr<Achievement> criteria = blz::make_shared<Achievement>(achievementSet);
        ptr = criteria.get();
        m_achievementsById[achievementSet][id].swap(criteria);
        m_achievements.push_back(ptr);
    }
    else
    {
        ptr = itSet->second.find(id)->second.get();
    }
    return shouldAllocate;
}
bool ProgressData::FreeAchievement(uint32 achievementSet, AchievementId id)
{
    AchievementSetMap::iterator itSet = m_achievementsById.find(achievementSet);
    if (itSet == m_achievementsById.end())
    {
        return false;
    }
    AchievementMap::iterator it = itSet->second.find(id);
    if (it == itSet->second.end())
    {
        return false;
    }
    for (AchievementVector::iterator itCrit = m_achievements.begin();
         itCrit != m_achievements.end(); ++itCrit)
    {
        if ((*itCrit)->unk_0 == achievementSet && (*itCrit)->m_achievementId == id)
        {
            m_achievements.erase(itCrit);
            break;
        }
    }
    itSet->second.erase(it);
    if (itSet->second.empty())
    {
        m_achievementsById.erase(itSet);
    }
    return true;
}
}  // namespace AchievementsEngine
}  // namespace OnlineService
