// Adopted from the DWARF (tools/gen_headers.py reference); hand-maintained from here on.
// Original file: X/Client/Battlenet/UI/Util/UIAchievementsUtil.h
// Only the season journey declarations are carried; the rest is not adopted.

#pragma once

#include "../../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Common/OnlineService/Achievements.h"
#include "../../../../Sigma/SigmaTypes.h"

namespace OnlineService {

enum {
    UNEARNED = 0,
    INPROGRESS = 1,
    EARNED = 2,
};

typedef int32 AchievementStatus;

struct CachedCategoryId {  // size 0x8
    CachedCategoryId()
    {
        tCategoryId = 0;
        bIsSeasonal = FALSE;
    }
    BOOL operator==(const OnlineService::CachedCategoryId & right)  // line 36
    {
        return tCategoryId == right.tCategoryId && bIsSeasonal == right.bIsSeasonal;
    }
    OnlineService::CategoryId tCategoryId;  // 0x00
    BOOL bIsSeasonal;  // 0x04
};
static_assert(sizeof(CachedCategoryId) == 0x8, "CachedCategoryId");

enum {
    EA_NORMAL_ACHIEVEMENTS = 0,
    EA_SEASON_ACHIEVEMENTS = 1,
};

void SortAchievementsByOrder(OnlineService::AchievementIdList & arIds);
bool CategorySortPredicate(const OnlineService::CategoryId & idA, const OnlineService::CategoryId & idB);
bool CriteriaSortPredicate(const OnlineService::CriteriaId & idA, const OnlineService::CriteriaId & idB);
bool CriteriaSortByCompletedPredicate(const OnlineService::CriteriaId & idA, const OnlineService::CriteriaId & idB);
BOOL CanShowCategory(const OnlineService::CategoryId & idCategory, BOOL fCheckForSeasonAchievements);
void CalculateCategoryPoints(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::CategoryId & idCategory, uint32 & nTotalPoints, uint32 & nEarnedPoints, BOOL fSeasonalPoints);
void AnalyzeCriteria(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, OnlineService::CriteriaQuantity & nEarnedPoints, OnlineService::CriteriaQuantity & nTotalPoints);
BOOL IsHighestAchievementEarnedInSeries(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement);
BOOL IsHighestAchievementUnearnedInSeries(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement);
OnlineService::AchievementId GetHighestEarnedAchievementInSeries(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement);
OnlineService::AchievementId GetHighestUnearnedAchievementInSeries(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement);
void SortRewardParentAchievements(OnlineService::AchievementIdList & arIds, const OnlineService::GameAccountHandle * pidPlayer);
OnlineService::AchievementStatus GetAchievementStatus(const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, OnlineService::CriteriaQuantity * pEarned, OnlineService::CriteriaQuantity * pTotal);
CRefString GetRewardTypeString(const OnlineService::RewardDefinition & def);
void AchievementsUtilInitializeForApp();
void AchievementsUtilDisposeForApp();
void ClearAchievementsCache();
void GetNumCriteriaCompleted(OnlineService::AchievementId achievementId, int32 & nNumEarnedCriteria, int32 & nTotalCriteria);
BOOL IsCriteriaEarned(const OnlineService::CriteriaId & criteriaId);

void GetFilteredSupercededAchievements( const OnlineService::GameAccountHandle & idPlayer,
                                        const OnlineService::AchievementId & idAchievement,
                                        OnlineService::AchievementIdList & arIds );

OnlineService::AchievementId GetLowestUnearnedAchievementInSeries( const OnlineService::GameAccountHandle & idPlayer,
                                                                   const OnlineService::AchievementId & idAchievement );

}  // namespace OnlineService
