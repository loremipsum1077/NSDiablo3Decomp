// Original file: X/Common/Achievements/Console/ConsoleAchievementsShared.cpp

#include "../../OnlineService/OnlineService.h"
#include "../../OnlineService/Achievements.h"
#include "../../Player.h"
#include "../../../Server/PlayerList.h"
#include "../../Networking/GameMessage.h"
#include "../../Protobuf/GameMessage.pb.h"
#include "../AchievementsEngine/achievements_types.h"
#include "../AchievementsEngine/achievements_internal_static_data_structs.h"
#include "../../../Server/Achievements/AchievementsEngine/achievements_update_structs.h"
#include "../../../Server/Achievements/AchievementsEngine/achievements_progress_data.h"

// Declared here from their definitions; no existing header declares them yet.
Player * PlayerGetByGameAccount(const OnlineService::GameAccountHandle & tGameAccount);  // Player.cpp:1571
void SPlayerAchievementEarned(const OnlineService::GameAccountHandle & tAccountId,
                              const OnlineService::AchievementId & achievementId);  // SPlayer.cpp:12917

namespace Console {
namespace AchievementsShared {

//-------------------------------------------------------------------------------------------------
void SendAchievementAwardedMessage(
    const OnlineService::GameAccountHandle & gameAccount,
    uint32 uAchievementSet,
    const blz::pair<uint32, unsigned long> * ptAchievements,
    size_t uNumAchievements )
{

    Player * ptPlayer = PlayerGetByGameAccount( gameAccount );
    if ( ptPlayer == NULL )
    {
        return;
    }

    PlayerList listPlayers( PlayerListGetElementPool() );
    PlayerListGetAllInGame( listPlayers, -1, FALSE, -1 );

    D3::GameMessage::AchievementsAwardedConsole tMsg;
    tMsg.set_achievement_set( uAchievementSet );
    tMsg.set_player_index( ptPlayer->nPlayerIndex );
    for ( size_t i = 0; i < uNumAchievements; ++i )
    {
        OnlineService::AchievementId idAchievement = ptAchievements[ i ].second;
        SPlayerAchievementEarned( gameAccount, idAchievement );
        tMsg.add_achievements( idAchievement );
    }

    MessageSendToClient( listPlayers, SMSG_ACHIEVEMENT_AWARDED, tMsg );
}

//-------------------------------------------------------------------------------------------------
void SendAchievementCriteriaUpdate(
    const OnlineService::GameAccountHandle & gameAccount,
    uint32 uAchievementSet,
    const OnlineService::AchievementsEngine::CriteriaUpdateRecord * ptUpdates,
    size_t uNumUpdates )
{
    // Unlike the awarded message, this goes only to the owning player.
    //
    // Each record becomes one Criteria entry: the id is the packed 32-bit criteria
    // id with no flags. Records marked TRACKED_CRITERIA_FLAG_DELETE_FROM_DB are
    // sent as deleted; otherwise a non-zero quantity is sent through the uint32
    // protobuf field.
    Player * ptPlayer = PlayerGetByGameAccount( gameAccount );
    if ( ptPlayer == NULL )
    {
        return;
    }

    PlayerList listDestPlayer( PlayerListGetElementPool() );
    PlayerListGetSingle( listDestPlayer, *ptPlayer );
    D3::GameMessage::AchievementsCriteriaUpdateConsole tMessage;
    tMessage.set_achievement_set( uAchievementSet );

    for ( uint32 i = 0; i < uNumUpdates; ++i )
    {
        const OnlineService::AchievementsEngine::CriteriaUpdateRecord & tCriteria = ptUpdates[ i ];
        D3::GameMessage::AchievementsCriteriaUpdateConsole::Criteria & tUpdate = *tMessage.add_criteria();

        tUpdate.set_id( OnlineService::AchievementsEngine::MakeCriteria32AndFlags8( tCriteria.m_criteria->m_id, 0 ) );

        if ( tCriteria.m_flags & OnlineService::AchievementsEngine::TRACKED_CRITERIA_FLAG_DELETE_FROM_DB )
        {
            tUpdate.set_deleted( true );
        }
        else if ( tCriteria.m_quantity != 0 )
        {
            tUpdate.set_quantity( tCriteria.m_quantity );
        }
    }

    MessageSendToClient( listDestPlayer, SMSG_CONSOLE_SYNC_ACCOUNT_ACHIEVEMENT_CRITERIA_UPDATE, tMessage );

}

}  // namespace AchievementsShared
}  // namespace Console
