#include "X/Common/Achievements/AchievementsEngine/achievements_static_data.h"
#include "X/Common/Achievements/AchievementsEngine/achievements_internal_static_data_structs.h"
#include "X/Common/OnlineService/OnlineService.h"
#include "X/Common/Protobuf/AchievementsStaticData.pb.h"
#include "BlizzardCore/Include/Blizzard/String.h"
#include "X/Sigma/DataStructures/stl/checked_delete.h"
#include "Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "X/Sigma/Math/MathGeneral.h"
#include <stdlib.h>

using namespace OnlineService;
using namespace OnlineService::AchievementsEngine;













static const OnlineService::CategoryId CATEGORY_GENERAL = 5505026;
static const OnlineService::CategoryId CATEGORY_FEATS_OF_STRENGTH = 5505067;




namespace OnlineService {
static const char * ACHIEVEMENT_ATTRIBUTE_TITLE = ":title:";

static const char * ACHIEVEMENT_ATTRIBUTE_DESCRIPTION_EARNED = ":description_earned:";
static const char * ACHIEVEMENT_ATTRIBUTE_ICON = ":achievement_icon:";
static const char * ACHIEVEMENT_ATTRIBUTE_EXPORT_TIMESTAMP = ":timestamp:";
static const char * ACHIEVEMENT_ATTRIBUTE_SEASONS_IN_CYCLE = ":seasons_in_cycle:";
static const char * ACHIEVEMENT_ATTRIBUTE_STRING_LABEL = ":stringlabel:";
static const char * ACHIEVEMENT_ATTRIBUTE_PARTITION = "Partition";

static const char * ACHIEVEMENT_ATTRIBUTE_DEPRECATED_AS_OF = "DeprecatedAsOf";
static const char * ACHIEVEMENT_ATTRIBUTE_AVAILABLE_BEGINNING_AT = "AvailableBeginningAt";
static const char * ACHIEVEMENT_CATEGORY_ATTRIBUTE_HIDDEN = "Hidden";
static const char * ACHIEVEMENT_CATEGORY_ATTRIBUTE_SEASON_JOURNEY = "SeasonJourney";
}




const OnlineService::AchievementsEngine::InternalCriteriaDefinition * OnlineService::AchievementsEngine::AchievementsStaticData::GetCriteriaDefinition(uint64 id) const
{
    OnlineService::AchievementsEngine::CriteriaMap::const_iterator i = m_criteria.find(id);
    if (i == m_criteria.end())
        return NULL;
    return i->second;
}
const OnlineService::AchievementsEngine::InternalAchievementDefinition * OnlineService::AchievementsEngine::AchievementsStaticData::GetAchievementDefinition(OnlineService::AchievementId id) const
{
    OnlineService::AchievementsEngine::AchievementMap::const_iterator i = m_achievements.find(id);
    if (i == m_achievements.end())
        return NULL;
    return i->second;
}
const OnlineService::AchievementsEngine::InternalCategoryDefinition * OnlineService::AchievementsEngine::AchievementsStaticData::GetCategoryDefinition(OnlineService::CategoryId id) const
{
    OnlineService::AchievementsEngine::CategoryMap::const_iterator i = m_categories.find(id);
    if (i == m_categories.end())
        return NULL;
    return i->second;
}
const OnlineService::AchievementsEngine::InternalRewardDefinition * OnlineService::AchievementsEngine::AchievementsStaticData::GetRewardDefinition(OnlineService::RewardId id) const
{
    OnlineService::AchievementsEngine::RewardMap::const_iterator i = m_rewards.find(id);
    if (i == m_rewards.end())
        return NULL;
    return i->second;
}
void LinkCategories(const CategoryMap & categories, InternalCategoryDefinition * category)
{
    if (category->m_id != 0)
    {
        CategoryMap::const_iterator itFind = categories.find(category->m_parentCategoryId);
        if (itFind != categories.end())
        {
            InternalCategoryDefinition * parent = itFind->second;
            parent->m_childCategories.push_back(category);
        }
    }
}









void LinkAchievements(const CategoryMap & categories, const AchievementMap & achievements, InternalAchievementDefinition * achievement)
{
    uint64 parentId = achievement->m_sharesCriteria;

    if (parentId != 0)
    {
        AchievementMap::const_iterator achievementParent = achievements.find(parentId);
        if (achievementParent != achievements.end())
        {
            achievementParent->second->m_sharedChildrenAchievements.push_back(achievement);
        }
    }

    CategoryMap::const_iterator category = categories.find(achievement->m_parentCategoryId);
    if (category == categories.end())
    {
        return;
    }

    category->second->m_childAchievements.push_back(achievement);

    if (achievement->m_seriesTailId == NULL && achievement->m_supersedes != 0)
    {
        InternalAchievementDefinition * tail = achievement;
        achievement->m_seriesTailId = tail;

        while (achievement->m_supersedes != 0)
        {
            AchievementMap::const_iterator achievementIter = achievements.find(achievement->m_supersedes);
            if (achievementIter == achievements.end())
            {
                break;
            }


            achievement = achievementIter->second;
            achievement->m_seriesTailId = tail;
        }
    }
}




void LinkCriteria(const AchievementMap & achievements, const EvaluatorClassSet & evaluationScope, PartitionedCriteriaEventMap & startEvents, PartitionedCriteriaEventMap & failEvents, PartitionedCriteriaEventMap & advanceEvents, InternalCriteriaDefinition * criteria)
{

    AchievementMap::const_iterator achievement = achievements.find(criteria->m_parentAchievementId);
    if (achievement == achievements.end())
    {
        return;
    }



    achievement->second->m_criteriaList.push_back(criteria);
    criteria->m_parentAchievement = achievement->second;

    if (!evaluationScope.empty() && evaluationScope.find(criteria->m_evaluationClass.GetValue()) == evaluationScope.end())
        return;
    if (criteria->m_startEvent.m_id != 0)
    {
        EventKey key(
            criteria->m_startEvent.m_id,
            criteria->m_startEvent.m_comparand,
            criteria->m_startEvent.m_usesComparand);

        for (uint32 partition = 0; (1u << partition) <= achievement->second->m_partitionMask; ++partition)
        {
            if (achievement->second->m_partitionMask & (1 << partition))
            {
                startEvents[partition][key].push_back(criteria);
            }
        }
    }

    if (criteria->m_failEvent.m_id != 0)
    {
        EventKey key(
            criteria->m_failEvent.m_id,
            criteria->m_failEvent.m_comparand,
            criteria->m_failEvent.m_usesComparand);

        for (uint32 partition = 0; (1u << partition) <= achievement->second->m_partitionMask; ++partition)
        {
            if (achievement->second->m_partitionMask & (1 << partition))
            {
                failEvents[partition][key].push_back(criteria);
            }
        }
    }

    if (criteria->m_advanceEvent.m_id != 0)
    {
        EventKey key(
            criteria->m_advanceEvent.m_id,
            criteria->m_advanceEvent.m_comparand,
            criteria->m_advanceEvent.m_usesComparand);

        for (uint32 partition = 0; (1u << partition) <= achievement->second->m_partitionMask; ++partition)
        {
            if (achievement->second->m_partitionMask & (1 << partition))
            {
                advanceEvents[partition][key].push_back(criteria);
            }
        }
    }
}



AchievementsStaticData::AchievementsStaticData(const blz::string & contentHandleHash)
    : m_contentHandleHash(contentHandleHash)
    , m_seasonCycleLength(0)
{
}
// NON_MATCHING: first for_each loop (categories) lays out the iterator ++ null check differently (8 insns); the other three loops match
AchievementsStaticData::~AchievementsStaticData()
{
    blz::for_each(
        m_categories.begin(), m_categories.end(), checked_second_deleter<CategoryMap::value_type>());



    blz::for_each(
        m_achievements.begin(), m_achievements.end(), checked_second_deleter<AchievementMap::value_type>());



    blz::for_each(
        m_criteria.begin(), m_criteria.end(), checked_second_deleter<CriteriaMap::value_type>());



    blz::for_each(
        m_rewards.begin(), m_rewards.end(), checked_second_deleter<RewardMap::value_type>());


}

void AchievementsStaticData::InitializeCriteriaEvent(OnlineService::CriteriaEvent & outCriteriaEvent) const
{
    outCriteriaEvent.m_id = 0;
    outCriteriaEvent.m_comparand = 0;
    outCriteriaEvent.m_usesComparand = false;
}

void AchievementsStaticData::CopyCriteriaEventFromProtocol(
    OnlineService::CriteriaEvent & outCriteriaEvent,
    const D3::AchievementsStaticData::CriteriaEvent & criteriaEvent) const
{




    outCriteriaEvent.m_id = criteriaEvent.id();
    outCriteriaEvent.m_usesComparand = criteriaEvent.has_comparand();
    outCriteriaEvent.m_comparand = outCriteriaEvent.m_usesComparand ? criteriaEvent.comparand() : 0;

    int modifierCount = criteriaEvent.modifier_size();

    if (modifierCount > 0)
    {
        outCriteriaEvent.SetModifierCount(modifierCount);
        for (int i = 0; i < modifierCount; ++i)
        {
            const D3::AchievementsStaticData::CriteriaModifier & modifier = criteriaEvent.modifier(i);
            OnlineService::CriteriaModifier outModifier;

            outModifier.m_necessaryCondition = modifier.necessary_condition();
            outModifier.m_target = modifier.target();
            outModifier.m_operand = (CriteriaModifierOperand::Enum)(modifier.operand() != 0);

            outModifier.m_comparand = modifier.comparand();
            outCriteriaEvent.m_modifier[i] = outModifier;
        }
    }
}


InternalCriteriaDefinition * AchievementsStaticData::CreateInternalCriteriaDefinitionFromProtocol(
    const D3::AchievementsStaticData::StaticCriteriaDefinition & criteriaDefinition) const
{
    InternalCriteriaDefinition * internalCriteriaDefinition = new InternalCriteriaDefinition();
    internalCriteriaDefinition->m_parentAchievement = NULL;
    internalCriteriaDefinition->m_proxyCriteria = NULL;
    internalCriteriaDefinition->m_sharingThisCriteria = false;


    const OnlineService::CriteriaId criteriaId = criteriaDefinition.criteria_id();

    internalCriteriaDefinition->m_id = criteriaId;
    internalCriteriaDefinition->m_parentAchievementId = criteriaDefinition.parent_achievement_id();

    InitializeCriteriaEvent(internalCriteriaDefinition->m_startEvent);
    if (criteriaDefinition.has_start_event())
    {
        const D3::AchievementsStaticData::CriteriaEvent & startCriteriaEvent = criteriaDefinition.start_event();
        CopyCriteriaEventFromProtocol(internalCriteriaDefinition->m_startEvent, startCriteriaEvent);

        internalCriteriaDefinition->m_startEvent.m_timerDuration =
            startCriteriaEvent.has_timer_duration() ? startCriteriaEvent.timer_duration() : 0;
    }

    InitializeCriteriaEvent(internalCriteriaDefinition->m_advanceEvent);
    if (criteriaDefinition.has_advance_event())
    {
        CopyCriteriaEventFromProtocol(internalCriteriaDefinition->m_advanceEvent, criteriaDefinition.advance_event());
    }

    InitializeCriteriaEvent(internalCriteriaDefinition->m_failEvent);
    if (criteriaDefinition.has_fail_event())
    {
        CopyCriteriaEventFromProtocol(internalCriteriaDefinition->m_failEvent, criteriaDefinition.fail_event());
    }

    internalCriteriaDefinition->m_necessaryQuantity = criteriaDefinition.necessary_quantity();
    internalCriteriaDefinition->m_uiOrder = criteriaDefinition.order_hint();
    internalCriteriaDefinition->m_evaluationClass = criteriaDefinition.evalutation_class();
    internalCriteriaDefinition->m_flags = criteriaDefinition.flags();

    internalCriteriaDefinition->m_nameKey = "";
    internalCriteriaDefinition->m_stringlabelKey = "";

    for (int32 i = 0; i < criteriaDefinition.attributes_size(); ++i)
    {
        const D3::AchievementsStaticData::Attribute & attribute = criteriaDefinition.attributes(i);

        if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_TITLE))
        {
            internalCriteriaDefinition->m_nameKey = attribute.value();
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_STRING_LABEL))
        {
            internalCriteriaDefinition->m_stringlabelKey = attribute.value();
        }
    }

    internalCriteriaDefinition->m_pszNameKey = internalCriteriaDefinition->m_nameKey.c_str();
    internalCriteriaDefinition->m_pszStringlabelKey = internalCriteriaDefinition->m_stringlabelKey.c_str();

    return internalCriteriaDefinition;
}

InternalAchievementDefinition * AchievementsStaticData::CreateInternalAchievementDefinitionFromProtocol(
    const D3::AchievementsStaticData::StaticAchievementDefinition & achievementDefinition) const
{
    InternalAchievementDefinition * internalAchievementDefinition = new InternalAchievementDefinition();
    internalAchievementDefinition->m_seriesTailId = NULL;



    internalAchievementDefinition->m_id = achievementDefinition.id();
    internalAchievementDefinition->m_supersedes = achievementDefinition.has_superseding_achievement_id() ?
        achievementDefinition.superseding_achievement_id() : 0;

    internalAchievementDefinition->m_parentCategoryId = achievementDefinition.category_id();

    internalAchievementDefinition->m_nMinRequiredCriteria = achievementDefinition.has_minimum_criteria() ?
        achievementDefinition.minimum_criteria() : 0;

    internalAchievementDefinition->m_pointsValue = achievementDefinition.points_value();
    internalAchievementDefinition->m_flags = achievementDefinition.flags();
    internalAchievementDefinition->m_uiOrder = achievementDefinition.order_hint();

    internalAchievementDefinition->m_sharesCriteria = achievementDefinition.has_criteria_shared_with_achievement_id() ?
        achievementDefinition.criteria_shared_with_achievement_id() : 0;

    internalAchievementDefinition->m_profileAwardId = -1;
    internalAchievementDefinition->m_profileAwardAmount = -1;

    internalAchievementDefinition->m_nameKey = "";
    internalAchievementDefinition->m_iconTexture = "";
    internalAchievementDefinition->m_descriptionKey = "";

    internalAchievementDefinition->m_partitionMask = 0;

    for (int32 i = 0; i < achievementDefinition.attributes_size(); ++i)
    {
        const D3::AchievementsStaticData::Attribute & attribute = achievementDefinition.attributes(i);
        if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_TITLE))
        {
            internalAchievementDefinition->m_nameKey = attribute.value();
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_ICON))
        {
            internalAchievementDefinition->m_iconTexture = attribute.value();
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_DESCRIPTION_EARNED))
        {
            internalAchievementDefinition->m_descriptionKey = attribute.value();
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_PARTITION))
        {
            uint32 partition = atoi(attribute.value().c_str());

            if (partition < 32)
            {




                internalAchievementDefinition->m_partitionMask |= (1 << partition);
                if (partition != 0)
                {
                    internalAchievementDefinition->m_seasonMask |= (1 << (partition - 1));
                }
            }
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_DEPRECATED_AS_OF))
        {
            internalAchievementDefinition->m_nDeprecatedAsOf = blz::stoi(attribute.value());
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_AVAILABLE_BEGINNING_AT))
        {
            internalAchievementDefinition->m_nAvailableBeginningAt = blz::stoi(attribute.value());
        }
    }


































    if (internalAchievementDefinition->m_partitionMask == 0)
    {
        internalAchievementDefinition->m_partitionMask = 1;
    }

    internalAchievementDefinition->m_pszNameKey = internalAchievementDefinition->m_nameKey.c_str();
    internalAchievementDefinition->m_pszIconTexture = internalAchievementDefinition->m_iconTexture.c_str();
    internalAchievementDefinition->m_pszDescriptionKey = internalAchievementDefinition->m_descriptionKey.c_str();

    return internalAchievementDefinition;
}
InternalCategoryDefinition * AchievementsStaticData::CreateInternalCategoryDefinitionFromProtocol(
    const D3::AchievementsStaticData::StaticCategoryDefinition & categoryDefinition) const
{
    InternalCategoryDefinition * internalCategoryDefinition = new InternalCategoryDefinition();

    internalCategoryDefinition->m_id = categoryDefinition.id();
    internalCategoryDefinition->m_parentCategoryId = categoryDefinition.has_parent_id() ?
        (CategoryId)categoryDefinition.parent_id() : 0;
    internalCategoryDefinition->m_flags = 0;

    internalCategoryDefinition->m_uiOrder = categoryDefinition.order_hint();

    internalCategoryDefinition->m_nameKey = "";
    for (int32 i = 0; i < categoryDefinition.attributes_size(); ++i)
    {
        const D3::AchievementsStaticData::Attribute & attribute = categoryDefinition.attributes(i);
        if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_TITLE))
        {
            internalCategoryDefinition->m_nameKey = attribute.value().c_str();
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_CATEGORY_ATTRIBUTE_HIDDEN))
        {
            if (Blizzard::String::Equal(attribute.value().c_str(), "Always")) internalCategoryDefinition->m_flags |= 1; else internalCategoryDefinition->m_flags |= 2;
        }








        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_CATEGORY_ATTRIBUTE_SEASON_JOURNEY))
        {
            internalCategoryDefinition->m_flags |= 4;
        }
    }

    internalCategoryDefinition->m_pszNameKey = internalCategoryDefinition->m_nameKey.c_str();

    return internalCategoryDefinition;
}













// NON_MATCHING: default category: original keeps the m_id/m_uiOrder zero stores and trims the value-init memset to start at +8; ours folds them into one memset (4 bytes)
void AchievementsStaticData::ParseFromPb(const D3::AchievementsStaticData::AchievementFile & achievementFile)
{
    const int attributeCount = achievementFile.attributes_size();
    for (int i = 0; i < attributeCount; ++i)
    {
        const D3::AchievementsStaticData::Attribute & attribute = achievementFile.attributes(i);





        if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_EXPORT_TIMESTAMP))
        {
            m_timestamp = attribute.value().c_str();
        }
        else if (Blizzard::String::Equal(attribute.key().c_str(), ACHIEVEMENT_ATTRIBUTE_SEASONS_IN_CYCLE))
        {
            m_seasonCycleLength = atoi(attribute.value().c_str());
        }
    }

    const int criteriaCount = achievementFile.criteria_size();
    m_criteria.reserve(criteriaCount);
    for (int i = 0; i < criteriaCount; ++i)
    {
        const D3::AchievementsStaticData::StaticCriteriaDefinition & staticCriteriaDefinition = achievementFile.criteria(i);


        m_criteria[staticCriteriaDefinition.criteria_id()] = CreateInternalCriteriaDefinitionFromProtocol(staticCriteriaDefinition);
    }

    const int achievementCount = achievementFile.achievement_size();
    m_achievements.reserve(achievementCount);
    for (int i = 0; i < achievementCount; ++i)
    {
        const D3::AchievementsStaticData::StaticAchievementDefinition & staticAchievementDefinition = achievementFile.achievement(i);


        InternalAchievementDefinition * definition = CreateInternalAchievementDefinitionFromProtocol(staticAchievementDefinition);
        m_achievements[staticAchievementDefinition.id()] = definition;
    }

    const int categoryCount = achievementFile.category_size();
    m_categories.reserve(categoryCount + 1);
    for (int i = 0; i < categoryCount; ++i)
    {
        const D3::AchievementsStaticData::StaticCategoryDefinition & staticCategoryDefinition = achievementFile.category(i);


        m_categories[staticCategoryDefinition.id()] = CreateInternalCategoryDefinitionFromProtocol(staticCategoryDefinition);
    }



    InternalCategoryDefinition * internalCategoryDefinition = new InternalCategoryDefinition();
    internalCategoryDefinition->m_id = 0;
    internalCategoryDefinition->m_parentCategoryId = 0;
    internalCategoryDefinition->m_uiOrder = 0;
    internalCategoryDefinition->m_nameKey = "";
    internalCategoryDefinition->m_pszNameKey = internalCategoryDefinition->m_nameKey.c_str();
    m_categories[0] = internalCategoryDefinition;

    AssociateData();



















}

// Rebuilt from the DWARF (progressiveAchievement 606, block with achvFind 610, finds 610/621) and rows 608:21/37, 612, 614:48, 616:17/41, 625; not inline (linker-removed out-of-line copy, 0xe8 = ours).
InternalAchievementDefinition * AchievementsStaticData::GetSeriesHead(InternalAchievementDefinition * achievement)
{
    InternalAchievementDefinition * progressiveAchievement = NULL;

    if (achievement != NULL && achievement->m_supersedes != INVALID_ACHIEVEMENT_ID)
    {
        AchievementMap::const_iterator achvFind = m_achievements.find(achievement->m_supersedes);

        while (achvFind != m_achievements.end())
        {
            progressiveAchievement = achvFind->second;

            if (progressiveAchievement->m_supersedes == INVALID_ACHIEVEMENT_ID)
            {
                break;
            }

            achvFind = m_achievements.find(progressiveAchievement->m_supersedes);
        }
    }

    return progressiveAchievement;
}

// NON_MATCHING: before the series loop the original loads m_criteria bucket count and buckets with one ldp (end() of the LinkCriteria loop); ours loads them separately (4 bytes)
void AchievementsStaticData::AssociateData()
{



    CategoryMap::iterator tCategoryIter = m_categories.find(CATEGORY_FEATS_OF_STRENGTH);
    if (tCategoryIter != m_categories.end())
    {
        tCategoryIter->second->m_parentCategoryId = CATEGORY_GENERAL;
    }



    uint32 largestPartitionMask = 0;
    for (AchievementMap::iterator iter = m_achievements.begin(); iter != m_achievements.end(); iter++)
    {
        largestPartitionMask = Math::Max(largestPartitionMask, iter->second->m_partitionMask);
    }

    uint32 partitionCount;
    for (partitionCount = 0; (1u << partitionCount) <= largestPartitionMask; ++partitionCount);


    m_startEvents.resize(partitionCount);
    m_failEvents.resize(partitionCount);
    m_advanceEvents.resize(partitionCount);

    for (CategoryMap::iterator iter = m_categories.begin(); iter != m_categories.end(); iter++)
    {
        LinkCategories(m_categories, iter->second);
    }

    for (AchievementMap::iterator iter = m_achievements.begin(); iter != m_achievements.end(); iter++)
    {
        LinkAchievements(m_categories, m_achievements, iter->second);
    }

    for (CriteriaMap::iterator iter = m_criteria.begin(); iter != m_criteria.end(); iter++)
    {
        LinkCriteria(m_achievements, m_evaluationScope, m_startEvents, m_failEvents, m_advanceEvents, iter->second);
    }

    for (CriteriaMap::iterator iter = m_criteria.begin(); iter != m_criteria.end(); iter++)
    {
        InternalCriteriaDefinition * criteria = iter->second;
        InternalAchievementDefinition * achievement = criteria->m_parentAchievement;
        if (achievement == NULL)
        {
            continue;
        }




        InternalAchievementDefinition * seriesHead = GetSeriesHead(achievement);
        if (seriesHead != NULL)
        {

            for (size_t i = 0; i < seriesHead->m_criteriaList.size(); ++i)
            {
                InternalCriteriaDefinition * seriesCriteria = seriesHead->m_criteriaList[i];

                if (criteria->m_startEvent == seriesCriteria->m_startEvent &&
                    criteria->m_failEvent == seriesCriteria->m_failEvent &&
                    criteria->m_advanceEvent == seriesCriteria->m_advanceEvent &&
                    criteria->m_evaluationClass == seriesCriteria->m_evaluationClass &&
                    criteria->m_flags == seriesCriteria->m_flags)
                {
                    criteria->m_proxyCriteria = seriesCriteria;
                    seriesCriteria->m_sharingWithCriteria.push_back(criteria);
                    seriesCriteria->m_sharingThisCriteria = true;
                }
            }
        }
    }
}
