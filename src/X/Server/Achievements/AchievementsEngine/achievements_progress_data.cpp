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
bool ProgressData::FindCriteria(uint64 id, Criteria *& ptr) const
{
    CriteriaMap::const_iterator it = m_criteriaById.find(id);
    const bool found = it != m_criteriaById.end();
    if (found)
    {

        ptr = it->second.get();
    }
    return found;
}


bool ProgressData::AllocateTrackedCriteria(CriteriaId id, Criteria *& ptr)
{
    CriteriaMap::const_iterator it = m_criteriaById.find(id);
    const bool shouldAllocate = it == m_criteriaById.end();
    if (shouldAllocate)
    {
        blz::shared_ptr<Criteria> criteria = blz::make_shared<Criteria>();
        ptr = criteria.get();
        m_criteriaById[id].swap(criteria);
        m_criteria.push_back(ptr);
    }
    else
    {
        ptr = it->second.get();
    }
    return shouldAllocate;
}



bool ProgressData::FreeCriteria(CriteriaId id)
{
    CriteriaMap::iterator it = m_criteriaById.find(id);
    const bool found = it != m_criteriaById.end();
    if (found)
    {

        for (CriteriaVector::iterator itCrit = m_criteria.begin();

             itCrit != m_criteria.end(); ++itCrit)
        {
            if ((*itCrit)->m_criteriaId == id)
            {

                m_criteria.erase(itCrit);
                break;
            }
        }

        m_criteriaById.erase(it);
    }

    return found;
}
bool ProgressData::FindAchievement(uint64 id, Achievement *& ptr) const
{
    AchievementMap::const_iterator it = m_achievementsById.find(id);
    const bool found = it != m_achievementsById.end();
    if (found)
    {

        ptr = it->second.get();
    }
    return found;
}


bool ProgressData::AllocateAchievement(AchievementId id, Achievement *& ptr)
{
    AchievementMap::const_iterator it = m_achievementsById.find(id);
    const bool shouldAllocate = it == m_achievementsById.end();
    if (shouldAllocate)
    {
        blz::shared_ptr<Achievement> criteria = blz::make_shared<Achievement>();
        ptr = criteria.get();
        m_achievementsById[id].swap(criteria);
        m_achievements.push_back(ptr);
    }
    else
    {
        ptr = it->second.get();
    }
    return shouldAllocate;
}



bool ProgressData::FreeAchievement(AchievementId id)
{
    AchievementMap::iterator it = m_achievementsById.find(id);
    const bool found = it != m_achievementsById.end();
    if (found)
    {

        for (AchievementVector::iterator itCrit = m_achievements.begin();

             itCrit != m_achievements.end(); ++itCrit)
        {
            if ((*itCrit)->m_achievementId == id)
            {

                m_achievements.erase(itCrit);
                break;
            }
        }

        m_achievementsById.erase(it);
    }

    return found;
}

}  // namespace AchievementsEngine
}  // namespace OnlineService
