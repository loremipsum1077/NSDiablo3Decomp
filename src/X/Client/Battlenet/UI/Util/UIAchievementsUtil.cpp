// Original file: X/Client/Battlenet/UI/Util/UIAchievementsUtil.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "UIAchievementsUtil.h"

#include "../../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../../Common/Game.h"
#include "../../../Console/GamerProfile.h"
#include "../../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../../Common/GlobalSNO.h"
#include "../../../../Common/CosmeticItems.h"
#include "../../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../../Common/Items.h"
#include "../../../../Common/Definitions/StringListDefinition.h"
#include "../../../Gfx/GfxVB.h"
#include "../../../../Common/WorldPlace.h"
#include "../../../../Sigma/Memory/SigmaMemory.h"



template <>
uint32 XHashKey<const OnlineService::GameAccountHandle &>(const OnlineService::GameAccountHandle & gameAccount);  // X/Common/OnlineService/OnlineService.cpp:1010
template <>
inline uint32 XHashKey<const OnlineService::CachedCategoryId &>(const OnlineService::CachedCategoryId & tCachedCategoryId);
struct CachedCategoryPoints
{
    int32 nEarned;
    int32 nTotal;
};

struct CachedCriteriaQuantity
{
    OnlineService::CriteriaQuantity nEarned;
    OnlineService::CriteriaQuantity nTotal;
};

typedef XGrowableMap<OnlineService::CachedCategoryId, const OnlineService::CachedCategoryId &, CachedCategoryPoints, const CachedCategoryPoints &> CategoryPointsMap;
typedef XGrowableMap<unsigned long, const unsigned long, CachedCriteriaQuantity, const CachedCriteriaQuantity &> CriteriaQuantityMap;


struct AchievementsUtilCache
{
    XGrowableMap<OnlineService::GameAccountHandle, const OnlineService::GameAccountHandle &, CategoryPointsMap *, CategoryPointsMap *> m_mapCategoryPointsCache;
    XGrowableMap<OnlineService::GameAccountHandle, const OnlineService::GameAccountHandle &, CriteriaQuantityMap *, CriteriaQuantityMap *> m_mapCriteriaQuantityCache;
};

static AchievementsUtilCache * sgptAchievementsUtilCache = NULL;

namespace OnlineService {








































































































































































































static bool AchievementSortOrderPredicate( const OnlineService::AchievementId & idA, const OnlineService::AchievementId & idB )
{
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    const OnlineService::AchievementDefinition * ptDefinitionA = tAchievements.GetAchievementDefinition( idA );
    const OnlineService::AchievementDefinition * ptDefinitionB = tAchievements.GetAchievementDefinition( idB );

    if ( ptDefinitionA && ptDefinitionB )
        return ptDefinitionA->m_uiOrder < ptDefinitionB->m_uiOrder;

    return ptDefinitionA != NULL;
}














void SortAchievementsByOrder( OnlineService::AchievementIdList & arIds )
{
    blz::sort( arIds.begin(), arIds.end(), AchievementSortOrderPredicate );
}


static OnlineService::GameAccountHandle sidAchievementSortPlayer;










































bool CategorySortPredicate( const OnlineService::CategoryId & idA, const OnlineService::CategoryId & idB )
{
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    const OnlineService::CategoryDefinition * ptDefinitionA = tAchievements.GetCategoryDefinition( idA );
    const OnlineService::CategoryDefinition * ptDefinitionB = tAchievements.GetCategoryDefinition( idB );

    if ( ptDefinitionA && ptDefinitionB )
        return ptDefinitionA->m_uiOrder < ptDefinitionB->m_uiOrder;

    return ptDefinitionA != NULL;
}



















bool CriteriaSortPredicate( const OnlineService::CriteriaId & idA, const OnlineService::CriteriaId & idB )
{
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    const OnlineService::CriteriaDefinition * ptDefinitionA = tAchievements.GetCriteriaDefinition( idA );
    const OnlineService::CriteriaDefinition * ptDefinitionB = tAchievements.GetCriteriaDefinition( idB );

    if ( ptDefinitionA && ptDefinitionB )
        return ptDefinitionA->m_uiOrder < ptDefinitionB->m_uiOrder;

    return ptDefinitionA != NULL;
}


















bool CriteriaSortByCompletedPredicate( const OnlineService::CriteriaId & idA, const OnlineService::CriteriaId & idB )
{
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    const OnlineService::CriteriaDefinition * ptDefinitionA = tAchievements.GetCriteriaDefinition( idA );
    const OnlineService::CriteriaDefinition * ptDefinitionB = tAchievements.GetCriteriaDefinition( idB );

    if ( IsCriteriaEarned( ptDefinitionA->m_id, sub_71000b4be0() ) && !IsCriteriaEarned( ptDefinitionB->m_id, sub_71000b4be0() ) )
        return true;

    if ( IsCriteriaEarned( ptDefinitionB->m_id, sub_71000b4be0() ) && !IsCriteriaEarned( ptDefinitionA->m_id, sub_71000b4be0() ) )
        return false;

    if ( ptDefinitionA && ptDefinitionB )
        return ptDefinitionA->m_uiOrder < ptDefinitionB->m_uiOrder;

    return ptDefinitionA != NULL;
}




































































































































































BOOL CanShowCategory( const OnlineService::CategoryId & idCategory, BOOL fCheckForSeasonAchievements )
{
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    OnlineService::AchievementIdList arAchievementIds;
    if ( tAchievements.GetCategoryAchievements( idCategory, arAchievementIds ) )
    {
        for ( uint32 i = 0; i < arAchievementIds.size(); ++i )
        {
            const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition( arAchievementIds[i] );
            if ( !ptDefinition )
                continue;

            if ( tAchievements.IsAchievementValidForSeason( ptDefinition, fCheckForSeasonAchievements ? GetSeasonNum() : 0 ) )
                return TRUE;
        }
    }

    OnlineService::CategoryIdList arSubCategoryIds;
    if ( tAchievements.GetChildCategories( idCategory, arSubCategoryIds ) )
    {
        for ( uint32 i = 0; i < arSubCategoryIds.size(); ++i )
        {
            BOOL fSuccess = CanShowCategory( arSubCategoryIds[i], fCheckForSeasonAchievements );
            if ( fSuccess )
                return TRUE;
        }
    }

    return FALSE;
}











































































void CalculateCategoryPoints( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::CategoryId & idCategory, uint32 & nTotalPoints, uint32 & nEarnedPoints, uint32 nAchievementSet, BOOL fSeasonalPoints )
{
    CategoryPointsMap * pmapCategoryPoints = NULL;

    OnlineService::CachedCategoryId tCachedCategoryId;
    tCachedCategoryId.tCategoryId = idCategory;
    tCachedCategoryId.bIsSeasonal = fSeasonalPoints;

    CachedCategoryPoints tCachedValues;

    if ( sgptAchievementsUtilCache )
    {
        if ( sgptAchievementsUtilCache->m_mapCategoryPointsCache.Lookup( idPlayer, pmapCategoryPoints ) &&
             pmapCategoryPoints->Lookup( tCachedCategoryId, tCachedValues ) )
        {
            nTotalPoints += tCachedValues.nTotal;
            nEarnedPoints += tCachedValues.nEarned;
            return;
        }

        if ( !pmapCategoryPoints )
        {
            pmapCategoryPoints = new CategoryPointsMap;
            sgptAchievementsUtilCache->m_mapCategoryPointsCache.SetAt_Tracked( idPlayer, pmapCategoryPoints );
        }
    }

    uint32 nTotal = 0;
    uint32 nEarned = 0;

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

    OnlineService::AchievementIdList arAchievementIds;
    if ( tAchievements.GetCategoryAchievements( idCategory, arAchievementIds ) )
    {
        for ( uint32 i = 0; i < arAchievementIds.size(); ++i )
        {
            const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition( arAchievementIds[i] );
            if ( tAchievements.ShouldCountAchievementPoints( ptDefinition ) &&
                 tAchievements.IsAchievementValidForSeason( ptDefinition, fSeasonalPoints ? GetSeasonNum() : 0 ) )
            {
                nTotal += ptDefinition->m_pointsValue;
                OnlineService::AchievementRecord tRecord;
                if ( ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement( idPlayer, nAchievementSet, arAchievementIds[i], tRecord ) )
                    nEarned += ptDefinition->m_pointsValue;
            }
        }
    }

    OnlineService::CategoryIdList arSubCategoryIds;
    if ( tAchievements.GetChildCategories( idCategory, arSubCategoryIds ) )
    {
        for ( uint32 i = 0; i < arSubCategoryIds.size(); ++i )
            CalculateCategoryPoints( idPlayer, arSubCategoryIds[i], nTotal, nEarned, nAchievementSet, fSeasonalPoints );
    }

    nTotalPoints += nTotal;
    nEarnedPoints += nEarned;

    if ( pmapCategoryPoints )
    {
        tCachedValues.nTotal = nTotal;
        tCachedValues.nEarned = nEarned;
        pmapCategoryPoints->SetAt_Tracked( tCachedCategoryId, tCachedValues );
    }
}

























void AnalyzeCriteria( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, OnlineService::CriteriaQuantity & nEarnedPoints, OnlineService::CriteriaQuantity & nTotalPoints, uint32 nAchievementSet )
{
    nEarnedPoints = 0;
    nTotalPoints = 0;

    CriteriaQuantityMap * pmapPoints = NULL;
    CachedCriteriaQuantity tCachedValues;

    if ( sgptAchievementsUtilCache &&
         sgptAchievementsUtilCache->m_mapCriteriaQuantityCache.Lookup( idPlayer, pmapPoints ) &&
         pmapPoints->Lookup( idAchievement, tCachedValues ) )
    {
        nEarnedPoints = tCachedValues.nEarned;
        nTotalPoints = tCachedValues.nTotal;
        return;
    }

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

    OnlineService::CriteriaIdList arCriteriaIds;
    if ( tAchievements.GetAchievementCriteria( idAchievement, arCriteriaIds ) )
    {
        const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition( idAchievement );
        if ( ptDefinition )
        {
            OnlineService::AchievementRecord tAchRecord;
            BOOL fAchievementEarned = ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement( idPlayer, nAchievementSet, idAchievement, tAchRecord );

            for ( uint32 i = 0; i < arCriteriaIds.size(); ++i )
            {
                const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition( arCriteriaIds[i] );
                if ( ptCritDef )
                {
                    OnlineService::CriteriaRecord tRecord;
                    SigmaMemoryClear( &tRecord, sizeof( tRecord ) );

                    if ( ptAchievementsInterface && !ptAchievementsInterface->FindCriteriaRecord( idPlayer, nAchievementSet, ptCritDef->m_id, tRecord ) )
                    {
                        tRecord.m_criteriaId = arCriteriaIds[i];
                        tRecord.m_quantity = fAchievementEarned ? ptCritDef->m_necessaryQuantity : 0;
                    }

                    if ( ptDefinition->m_flags & ( 1 << OnlineService::ACHIEVEMENT_FLAG_CUMULATIVE ) )
                    {
                        if ( nTotalPoints == 0 && ptCritDef->m_uiOrder == 0 )
                            nTotalPoints = ptCritDef->m_necessaryQuantity;
                        nEarnedPoints += tRecord.m_quantity;
                    }
                    else if ( ptDefinition->m_flags & ( 1 << OnlineService::ACHIEVEMENT_FLAG_DISPLAY_HIGHEST ) )
                    {
                        if ( tRecord.m_quantity > nEarnedPoints )
                        {
                            nEarnedPoints = tRecord.m_quantity;
                            nTotalPoints = ptCritDef->m_necessaryQuantity;
                        }
                    }
                    else if ( ptDefinition->m_flags & ( 1 << OnlineService::ACHIEVEMENT_FLAG_CRITERIA_COUNT ) )
                    {
                        if ( tRecord.m_quantity >= ptCritDef->m_necessaryQuantity )
                            ++nEarnedPoints;
                        ++nTotalPoints;
                    }
                    else if ( ptCritDef->m_flags & ( 1 << OnlineService::CRITERIA_FLAG_PROGRESS_BAR ) )
                    {
                        nEarnedPoints = tRecord.m_quantity;
                        nTotalPoints = ptCritDef->m_necessaryQuantity;
                    }
                }
            }

            if ( ( ptDefinition->m_flags & ( 1 << OnlineService::ACHIEVEMENT_FLAG_CRITERIA_COUNT ) ) && ptDefinition->m_nMinRequiredCriteria != 0 )
                nTotalPoints = nTotalPoints > ptDefinition->m_nMinRequiredCriteria ? ptDefinition->m_nMinRequiredCriteria : nTotalPoints;

            nEarnedPoints = nTotalPoints < nEarnedPoints ? nTotalPoints : nEarnedPoints;

            if ( sgptAchievementsUtilCache )
            {
                tCachedValues.nEarned = nEarnedPoints;
                tCachedValues.nTotal = nTotalPoints;

                if ( !pmapPoints )
                    pmapPoints = new CriteriaQuantityMap;

                pmapPoints->SetAt_Tracked( idAchievement, tCachedValues );
                sgptAchievementsUtilCache->m_mapCriteriaQuantityCache.SetAt_Tracked( idPlayer, pmapPoints );
            }
        }
    }
}




















BOOL IsHighestAchievementEarnedInSeries( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, uint32 nAchievementSet )
{
    return GetHighestEarnedAchievementInSeries( idPlayer, idAchievement, nAchievementSet ) == idAchievement;
}





BOOL IsHighestAchievementUnearnedInSeries( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, uint32 nAchievementSet )
{
    return GetHighestUnearnedAchievementInSeries( idPlayer, idAchievement, nAchievementSet ) == idAchievement;
}





OnlineService::AchievementId GetHighestEarnedAchievementInSeries( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, uint32 nAchievementSet )
{
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    OnlineService::AchievementId idHighestUnearned = GetHighestUnearnedAchievementInSeries( idPlayer, idAchievement, nAchievementSet );
    if ( idHighestUnearned != 0 )
    {
        const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition( idHighestUnearned );
        if ( ptDefinition )
            return ptDefinition->m_supersedes;
        return 0;
    }

    return tAchievements.GetSeriesTail( idAchievement );
}















__attribute__((noinline)) OnlineService::AchievementId GetHighestUnearnedAchievementInSeries( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, uint32 nAchievementSet )
{
    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

    OnlineService::AchievementId idHighestUnearned = 0;
    if ( tAchievements.IsAchievementInSeries( idAchievement ) )
    {
        OnlineService::AchievementId idHighestEarned = tAchievements.GetSeriesTail( idAchievement );
        OnlineService::AchievementRecord tRecord;
        if (ptAchievementsInterface && !ptAchievementsInterface->FindAwardedAchievement(idPlayer, nAchievementSet, idHighestEarned, tRecord))
        {
            const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition(idHighestEarned);
            while (ptDefinition)
            {
                idHighestUnearned = ptDefinition->m_id;
                if (!tAchievements.GetAchievementDefinition(ptDefinition->m_supersedes))
                    break;
                idHighestEarned = ptDefinition->m_supersedes;
                if (ptAchievementsInterface->FindAwardedAchievement(idPlayer, nAchievementSet, idHighestEarned, tRecord))
                    break;
                ptDefinition = tAchievements.GetAchievementDefinition(idHighestEarned);
            }
        }
    }

    return idHighestUnearned;
}






















OnlineService::AchievementId GetLowestUnearnedAchievementInSeries( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, uint32 nAchievementSet )
{
    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

    if ( tAchievements.IsAchievementInSeries( idAchievement ) && ptAchievementsInterface )
    {
        OnlineService::AchievementId idSeriesAchievement = tAchievements.GetSeriesTail( idAchievement );
        const OnlineService::AchievementDefinition * ptSeriesDefinition = tAchievements.GetAchievementDefinition( idSeriesAchievement );
        OnlineService::AchievementId idLowestUnearned = 0;
        OnlineService::AchievementRecord tRecord;
        while ( ptSeriesDefinition )
        {
            if ( !ptAchievementsInterface->FindAwardedAchievement( idPlayer, nAchievementSet, idSeriesAchievement, tRecord ) )
                idLowestUnearned = idSeriesAchievement;

            idSeriesAchievement = ptSeriesDefinition->m_supersedes;
            ptSeriesDefinition = tAchievements.GetAchievementDefinition( idSeriesAchievement );
        }
        return idLowestUnearned;
    }

    return 0;
}

















void GetFilteredSupercededAchievements( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, OnlineService::AchievementIdList & arIds, uint32 nAchievementSet )
{
    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

    const OnlineService::AchievementDefinition * ptDefinition = tAchievements.GetAchievementDefinition( idAchievement );
    if ( ptDefinition && tAchievements.IsAchievementInSeries( idAchievement ) )
    {
        OnlineService::AchievementId idSeriesAchievement = ptDefinition->m_id;
        const OnlineService::AchievementDefinition * ptSeriesDefinition = tAchievements.GetAchievementDefinition( idSeriesAchievement );
        while ( ptSeriesDefinition )
        {
            OnlineService::AchievementRecord tRecord;
            if ( !( ptSeriesDefinition->m_flags & ( 1 << OnlineService::ACHIEVEMENT_FLAG_HIDDEN ) ) &&
                 ( !( ptSeriesDefinition->m_flags & ( 1 << OnlineService::ACHIEVEMENT_FLAG_HIDDEN_TILL_AWARDED ) ) ||
                   ( ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement( idPlayer, nAchievementSet, idSeriesAchievement, tRecord ) ) ) )
            {
                arIds.push_back( ptSeriesDefinition->m_id );
            }

            idSeriesAchievement = ptSeriesDefinition->m_supersedes;
            ptSeriesDefinition = tAchievements.GetAchievementDefinition( idSeriesAchievement );
        }
    }
}

















































// Rows: nested points tests at 1133 (`&&` col 44) and 1137 (`&&` col 49); the B-first order at 1137 keeps the original's re-test of A.
static bool sRewardAchievementSort( const OnlineService::AchievementId & idA, const OnlineService::AchievementId & idB )
{
    OnlineService::GameAccountHandle idGameAccount;
    if ( sidAchievementSortPlayer.IsInvalid() )
    {

        idGameAccount.SetInvalid();
    }
    else
    {



        idGameAccount = sidAchievementSortPlayer;
    }


    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptInterface = sgtAppGlobals.ptAchievementsInterface;






    OnlineService::AchievementRecord tRecordA, tRecordB;
    bool bEarnedA = ptInterface && ptInterface->FindAwardedAchievement( idGameAccount, 0, idA, tRecordA );
    bool bEarnedB = ptInterface && ptInterface->FindAwardedAchievement( idGameAccount, 0, idB, tRecordB );
    if ( bEarnedA != bEarnedB )
    {
        return bEarnedA;
    }
    else if ( bEarnedA )
    {
        return tRecordA.m_timestampEarned < tRecordB.m_timestampEarned;
    }
    else
    {
        const OnlineService::AchievementDefinition * ptDefA = tAchievements.GetAchievementDefinition( idA );
        const OnlineService::AchievementDefinition * ptDefB = tAchievements.GetAchievementDefinition( idB );
        if ( ptDefA && !ptDefB )
        {
            return true;
        }
        else if ( !ptDefA && ptDefB )
        {
            return false;
        }
        else if ( ptDefA && ptDefB )
        {
            if ( ptDefA->m_pointsValue == 0 && ptDefB->m_pointsValue != 0 )
            {
                return false;
            }
            else if ( ptDefB->m_pointsValue == 0 && ptDefA->m_pointsValue != 0 )
            {
                return true;
            }
        }
    }








    return idA < idB;

}



void SortRewardParentAchievements( OnlineService::AchievementIdList & arIds, const OnlineService::GameAccountHandle * pidPlayer )
{
    if ( pidPlayer )
        sidAchievementSortPlayer = *pidPlayer;
    else
        sidAchievementSortPlayer.SetInvalid();

    blz::sort( arIds.begin(), arIds.end(), sRewardAchievementSort );

    sidAchievementSortPlayer.SetInvalid();
}







OnlineService::AchievementStatus GetAchievementStatus( const OnlineService::GameAccountHandle & idPlayer, const OnlineService::AchievementId & idAchievement, uint32 nAchievementSet, OnlineService::CriteriaQuantity * pEarned, OnlineService::CriteriaQuantity * pTotal )
{
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    OnlineService::AchievementRecord tRecord;
    BOOL fEarned = ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement( idPlayer, nAchievementSet, idAchievement, tRecord );

    if ( fEarned )
    {
        if ( pEarned || pTotal )
        {
            OnlineService::CriteriaQuantity nTotal = 0;
            OnlineService::CriteriaQuantity nEarned = 0;
            AnalyzeCriteria( idPlayer, idAchievement, nEarned, nTotal, nAchievementSet );
            if ( pEarned )
                *pEarned = nEarned;
            if ( pTotal )
                *pTotal = nTotal;
        }
        return OnlineService::EARNED;
    }

    OnlineService::CriteriaQuantity nTotal = 0;
    OnlineService::CriteriaQuantity nEarned = 0;
    AnalyzeCriteria( idPlayer, idAchievement, nEarned, nTotal, nAchievementSet );
    if ( pEarned )
        *pEarned = nEarned;
    if ( pTotal )
        *pTotal = nTotal;
    return OnlineService::UNEARNED;
}


































































































CRefString GetRewardTypeString( const OnlineService::RewardDefinition & def )
{
    switch ( def.m_type )
    {
        case OnlineService::REWARD_TYPE_BANNER_SIGIL:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeSigil" );
        case OnlineService::REWARD_TYPE_BANNER_SIGIL_ACCENT:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeSigilAccent" );
        case OnlineService::REWARD_TYPE_BANNER_PATTERN:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypePattern" );
        case OnlineService::REWARD_TYPE_BANNER_SHAPE:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeShape" );
        case OnlineService::REWARD_TYPE_BANNER_COLOR:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeDye" );
        case OnlineService::REWARD_TYPE_BANNER_EPIC_BANNER:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeEpicBanner" );
        case OnlineService::REWARD_TYPE_VANITY_TITLE:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeVanityTitle" );
        case OnlineService::REWARD_TYPE_ITEM_TRANSMOG:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeItemTransmog" );
        case OnlineService::REWARD_TYPE_ITEM_CURRENCY:
            switch ( def.m_currency )
            {
                case CURRENCY_GOLD:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeGold" );
                case CURRENCY_BLOODSHARDS:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeBloodShards" );
                case CURRENCY_PLATINUM:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypePlatinum" );
            }
            // fall through
        case OnlineService::REWARD_TYPE_COSMETIC:
        {
            CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID( def.m_id );
            switch ( eCosmeticType )
            {
                case COSMETIC_ITEM_WINGS:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeCosmeticWings" );
                case COSMETIC_ITEM_PENNANT:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeCosmeticPennant" );
                case COSMETIC_ITEM_PET:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeCosmeticPet" );
                case COSMETIC_ITEM_FRAME:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeCosmeticPortrait" );
                default:
                    return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeCosmeticItem" );
            }
        }
        case OnlineService::REWARD_TYPE_ITEM:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeItem" );
        case OnlineService::REWARD_TYPE_STASH_TAB:
            return StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_BNET_ACHIEVEMENTS ), "RewardTypeStashSpace" );
    }

    return CRefString( "" );
}

































void AchievementsUtilInitializeForApp()
{
    sgptAchievementsUtilCache = new AchievementsUtilCache();
}





void AchievementsUtilDisposeForApp()
{
    ClearAchievementsCache();
    delete sgptAchievementsUtilCache;
    sgptAchievementsUtilCache = NULL;
}



void ClearAchievementsCache()
{
    if ( sgptAchievementsUtilCache )
    {
        POS pos;
        pos = sgptAchievementsUtilCache->m_mapCategoryPointsCache.GetStartAssoc();
        while ( pos )
        {
            OnlineService::GameAccountHandle id;
            CategoryPointsMap * ptMap;
            sgptAchievementsUtilCache->m_mapCategoryPointsCache.GetNextAssoc( pos, id, ptMap );
            delete ptMap;
        }
        sgptAchievementsUtilCache->m_mapCategoryPointsCache.RemoveAll();

        pos = sgptAchievementsUtilCache->m_mapCriteriaQuantityCache.GetStartAssoc();
        while ( pos )
        {
            OnlineService::GameAccountHandle id;
            CriteriaQuantityMap * ptMap;
            sgptAchievementsUtilCache->m_mapCriteriaQuantityCache.GetNextAssoc( pos, id, ptMap );
            delete ptMap;
        }
        sgptAchievementsUtilCache->m_mapCriteriaQuantityCache.RemoveAll();
    }
}


void GetNumCriteriaCompleted( OnlineService::AchievementId achievementId, int32 & nNumEarnedCriteria, int32 & nTotalCriteria, uint32 nAchievementSet )
{
    nNumEarnedCriteria = 0;
    nTotalCriteria = 0;

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::CriteriaIdList arCriteriaIds;
    tAchievements.GetAchievementCriteria( achievementId, arCriteriaIds );

    for ( uint32 i = 0; i < arCriteriaIds.size(); ++i )
    {
        const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition( arCriteriaIds[i] );
        if ( ptCritDef && !( ptCritDef->m_flags & ( 1 << OnlineService::CRITERIA_FLAG_HIDDEN ) ) )
        {
            BOOL fEarned = IsCriteriaEarned( arCriteriaIds[i], nAchievementSet );
            if ( fEarned )
                ++nNumEarnedCriteria;
            ++nTotalCriteria;
        }
    }
}























BOOL IsCriteriaEarned( const OnlineService::CriteriaId & criteriaId, uint32 nAchievementSet )
{
    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;

    OnlineService::GameAccountHandle heroId = Console::ClientAchievementsInterface::UserIndexToGameAccount( Console::GamerProfile::GetPrimaryProfileUserIndex() );

    const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition( criteriaId );
    if ( !ptCritDef )
        return FALSE;

    const OnlineService::AchievementDefinition * ptAchievementDef = tAchievements.GetAchievementDefinition( ptCritDef->m_parentAchievementId );
    if ( !ptAchievementDef )
        return FALSE;

    OnlineService::CriteriaRecord tCritRecord;
    if ( ptAchievementsInterface )
    {
        if ( ptAchievementsInterface->FindCriteriaRecord( heroId, nAchievementSet, ptCritDef->m_id, tCritRecord ) )
            return tCritRecord.m_quantity >= ptCritDef->m_necessaryQuantity;

        if ( ptAchievementDef->m_nMinRequiredCriteria == 0 )
        {
            OnlineService::AchievementRecord tRecord;
            return ptAchievementsInterface->FindAwardedAchievement( heroId, nAchievementSet, ptCritDef->m_parentAchievementId, tRecord );
        }
    }

    return FALSE;
}














































































}  // namespace OnlineService





template <>
inline uint32 XHashKey<const OnlineService::CachedCategoryId &>( const OnlineService::CachedCategoryId & tCachedCategoryId )
{
    return XHashKey<OnlineService::CategoryId>( tCachedCategoryId.tCategoryId ) ^ XHashKey<BOOL>( tCachedCategoryId.bIsSeasonal );
}
