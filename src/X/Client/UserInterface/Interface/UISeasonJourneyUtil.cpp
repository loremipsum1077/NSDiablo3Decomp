// Original file: X/Client/UserInterface/Interface/UISeasonJourneyUtil.cpp

#include "UISeasonJourneyUtil.h"

#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Common/Game.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../Battlenet/UI/Util/UIAchievementsUtil.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"

inline OnlineService::Achievements & sGetAchievements()  // line 22
{
    return *sgtAppGlobals.ptAchievements;
}

inline OnlineService::AchievementsInterface * sGetAchievementsInterface()  // line 32
{
    return sgtAppGlobals.ptAchievementsInterface;
}

BOOL SeasonJourney_IsCasualAchievement( const OnlineService::AchievementIdList & arCasualAchievementIds,
                                        const OnlineService::AchievementId & idAchievement )
{
    const OnlineService::AchievementId * tIter = blz::find_if( arCasualAchievementIds.begin(), arCasualAchievementIds.end(),
        [&idAchievement]( const OnlineService::AchievementId & idCurrent ) { return idAchievement == idCurrent; } );
    return ( tIter != arCasualAchievementIds.end() );
}

inline const OnlineService::CategoryDefinition * SeasonJourney_GetCategoryDefinition( OnlineService::Achievements & tAchievements )  // line 63
{
    const OnlineService::CategoryDefinition * ptDefinition = NULL;
    OnlineService::CategoryIdList tTopCategories;

    if ( tAchievements.GetChildCategories( 0, tTopCategories ) )
    {
        for ( uint32 i = 0; i < tTopCategories.size(); ++i )
        {
            ptDefinition = tAchievements.GetCategoryDefinition( tTopCategories[i] );

            if ( ptDefinition != NULL && ( ptDefinition->m_flags & ( 1 << OnlineService::CATEGORY_FLAG_SEASON_JOURNEY ) ) )
                break;

            ptDefinition = NULL;
        }
    }

    return ptDefinition;
}

void SeasonJourney_GetAchievementsToDisplay( OnlineService::AchievementId & idAchDefinitionToDisplay,
                                             SeasonJourneyProgressionView & eSeasonJourneyProgressionView,
                                             OnlineService::AchievementIdList & arCasualAchievementIds,
                                             OnlineService::AchievementIdList & arAdvancedAchievementIds,
                                             const OnlineService::GameAccountHandle & tAccountId )
{
    idAchDefinitionToDisplay = 0;
    eSeasonJourneyProgressionView = SEASON_JOURNEY_VIEW_INVALID;

    const OnlineService::CategoryDefinition * ptSeasonJourneyCategoryDefinition = SeasonJourney_GetCategoryDefinition( sGetAchievements() );

    if ( ptSeasonJourneyCategoryDefinition != NULL )
    {
        OnlineService::Achievements & tAchievements = sGetAchievements();
        OnlineService::AchievementId idSeriesTail = tAchievements.GetSeasonJourneyRankSeriesTail( OnlineService::GetSeasonNum() );
        OnlineService::AchievementIdList arAchievementIds;

        OnlineService::GetFilteredSupercededAchievements( tAccountId, idSeriesTail, arAchievementIds );

        if ( arAchievementIds.size() != 0 )
        {
            for ( uint32 i = 0; i < arAchievementIds.size(); ++i )
            {
                const OnlineService::AchievementDefinition * ptCurrentAchDefinition = tAchievements.GetAchievementDefinition( arAchievementIds[i] );

                if ( ptCurrentAchDefinition != NULL )
                {
                    const OnlineService::CategoryDefinition * ptCategoryDefinition = tAchievements.GetCategoryDefinition( ptCurrentAchDefinition->m_parentCategoryId );

                    if ( ptCategoryDefinition != NULL )
                    {
                        if ( ptCategoryDefinition->m_uiOrder == 0 )
                            arCasualAchievementIds.push_back( ptCurrentAchDefinition->m_id );
                        else
                            arAdvancedAchievementIds.push_back( ptCurrentAchDefinition->m_id );
                    }
                }
            }

            eSeasonJourneyProgressionView = SEASON_JOURNEY_VIEW_INVALID;

            OnlineService::AchievementId idLowestUnearned = OnlineService::GetLowestUnearnedAchievementInSeries( tAccountId, arAdvancedAchievementIds[0] );

            // NOTE: DWARF lines 148 and 151 map both GetAchievementDefinition calls here
            // and discard both results (0x3d8a68, 0x3d8a78); the original emits the calls
            // themselves, so they are kept even though ptLastAchDefinition is unused.
            const OnlineService::AchievementDefinition * ptLastAchDefinition = tAchievements.GetAchievementDefinition( idLowestUnearned );
            ptLastAchDefinition = tAchievements.GetAchievementDefinition( arAdvancedAchievementIds[0] );

            if ( SeasonJourney_IsCasualAchievement( arCasualAchievementIds, idLowestUnearned ) )
            {
                idAchDefinitionToDisplay = idLowestUnearned;
                eSeasonJourneyProgressionView = SEASON_JOURNEY_VIEW_NOT_COMPLETED;
            }
            else
            {
                idAchDefinitionToDisplay = 0;
                eSeasonJourneyProgressionView = SEASON_JOURNEY_VIEW_INVALID;
            }
        }
    }
}

BOOL SeasonJourney_IsAchievementCompleted( const OnlineService::GameAccountHandle & tAccountId,
                                           const OnlineService::AchievementId & idAchievement )
{
    OnlineService::AchievementsInterface * ptAchievementsInterface = sGetAchievementsInterface();
    OnlineService::AchievementRecord tRecord;
    BOOL bShowCompleteHeader = FALSE;

    if ( ptAchievementsInterface != NULL )
        bShowCompleteHeader = ptAchievementsInterface->FindAwardedAchievement( tAccountId, idAchievement, tRecord );

    return bShowCompleteHeader;
}
