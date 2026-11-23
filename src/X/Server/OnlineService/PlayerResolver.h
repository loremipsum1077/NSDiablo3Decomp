// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/OnlineService/PlayerResolver.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/EncounterDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/OnlineService/OnlineServiceErrorCodes.h"
#include "../../Common/Protobuf/Account.pb.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../Common/Protobuf/Items.pb.h"
#include "../../Common/Protobuf/Leaderboard.pb.h"
#include "BaseResolver.h"
#include "OnlineServiceServer.h"
#include "../../Sigma/DataStructures/XArray.h"

namespace D3 { namespace OnlineService { class CampaignOrAdventureModeCreateParams; } }
namespace D3 { namespace OnlineService { class GameCreateParams; } }

typedef int32 ServerSocketID;

namespace OnlineService {
enum PlayerResolveOptions {
    RESOLVE_OPTION_ACCOUNT_LOCK = 0,
    RESOLVE_OPTION_RESOLVE_LICENSES = 1,
    RESOLVE_OPTION_CHALLENGE_OVERRIDE = 2,
    RESOLVE_OPTION_RESOLVE_BNET_ACCOUNT = 3,
};

enum PlayerResolveResults {
    RESOLVE_RESULT_ACCOUNT_LOCK = 0,
    RESOLVE_RESULT_ACCOUNT_DEFINITION = 1,
    RESOLVE_RESULT_HERO_DEFINITION = 2,
    RESOLVE_RESULT_LICENSES = 3,
    RESOLVE_RESULT_CHALLENGE_OVERRIDDEN = 4,
    RESOLVE_RESULT_ACCOUNT_LOCKED = 5,
    RESOLVE_RESULT_MAIL = 6,
    RESOLVE_RESULT_BNET_ACCOUNT = 7,
};

enum PlayerJoinFlags {
    PLAYER_JOIN_CONNECTED_BIT = 0,
    PLAYER_JOIN_WAITING_ON_WORLD_CREATION = 1,
    PLAYER_JOIN_WAITING_FOR_ACHIEVEMENT_REGISTRATION = 2,
    PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_COMPLETE = 3,
    PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_FAILED = 4,
    PLAYER_JOIN_REPLAY_JOIN_RECORDED = 5,
};

enum PlayerValidateFlags {
    PLAYER_VALIDATE_FLAG_IS_HARDCORE_GAME = 0,
    PLAYER_VALIDATE_FLAG_IS_PAID_GAME = 1,
    PLAYER_VALIDATE_FLAG_IS_PUBLIC_GAME = 2,
    PLAYER_VALIDATE_FLAG_VALIDATE_QUEST_PROGRESSION = 3,
    PLAYER_VALIDATE_FLAG_IGNORE_MONSTER_LEVEL = 4,
    PLAYER_VALIDATE_FLAG_HANDICAP_UNLOCKED = 5,
    PLAYER_VALIDATE_FLAG_ADVENTURE_MODE_UNLOCKED = 6,
    PLAYER_VALIDATE_FLAG_HAS_PLAYER_COMPLETED_SOLO_RIFT = 7,
};

struct PlayerResolveData {  // size 0x2f0
    PlayerResolveData();
    OnlineService::HeroId uHeroId;  // 0x00
    OnlineService::AccountId uBnetAccountId;  // 0x08
    OnlineService::GameAccountHandle tAccountId;  // 0x0c
    char szAccountName[128];  // 0x18
    Team eTeam;  // 0x98
    D3::Account::SavedDefinition tAccountDefinition;  // 0xa0
    D3::Hero::SavedDefinition tHeroDefinition;  // 0x198
    D3::Items::Mails tMails;  // 0x220
    uint32 uResolveResultFlags;  // 0x250
    uint64 uAuthToken;  // 0x258
    OnlineServiceServer::LicenseArray tRawLicenses;  // 0x260
    OnlineService::ContentLicenseArray tContentLicenses;  // 0x278
    uint32 uSessionFlags;  // 0x290
    uint32 uLastChallengeRewardEarned;  // 0x294
    blz::vector<blz::pair<unsigned long, unsigned long>, blz::allocator<blz::pair<unsigned long, unsigned long> > > tChallengeRiftPersonalBests;  // 0x298
    int32 nControllerIndex;  // 0x2b0
    int32 nPartyIndex;  // 0x2b4
    BOOL bIsPrimary;  // 0x2b8
    BOOL bIsGuest;  // 0x2bc
    BOOL bIsLocal;  // 0x2c0
    uint32 uConsoleLicenses;  // 0x2c4
    XLocale eLocale;  // 0x2c8
    ServerSocketID idServerSocket;  // 0x2cc
    uint32 uJoinFlags;  // 0x2d0
    Blizzard::Time::Second tAdded;  // 0x2d4
    uint64 nTeamId;  // 0x2d8
    float teamRating;  // 0x2e0
    float teamVariance;  // 0x2e4
    float individualRating;  // 0x2e8
    float individualVariance;  // 0x2ec
};
static_assert(sizeof(PlayerResolveData) == 0x2f0, "PlayerResolveData");

class PlayerResolver : public OnlineService::BaseResolver {  // size 0xcb8
public:
    PlayerResolver(uint32, const char *);
    virtual ~PlayerResolver();  // vtable slot 0
    void AddPlayer(OnlineService::HeroId uHeroId, const OnlineService::GameAccountHandle & tAccountId, const char * pszAccountName, Team eTeam, uint64 uAuthToken);
    void AddPvpPlayer(OnlineService::HeroId, const OnlineService::GameAccountHandle &, const char *, Team, uint64, uint64, float, float, float, float);
    typedef OnlineService::Callback<void (int)> ResolveCompleteCb;
    void Begin(OnlineService::PlayerResolver::ResolveCompleteCb cb, uint64 gameId);
    OnlineService::ErrorCode ValidatePlayers(SGameID idSGame);
    OnlineService::ErrorCode ValidatePlayers(const D3::OnlineService::GameCreateParams &);
    OnlineService::ErrorCode AppendPlayersToGame(SGameID idSGame);
    int32 GetResolvePlayerCount() const;
    // line 160; the body is the copy inlined into sServerMessageReceive_ConsolePlayerResolved.
    OnlineService::PlayerResolveData & GetResolvedPlayer(uint32 uIndex) { return m_tResolveArray[uIndex]; }
    const OnlineService::PlayerResolveData & GetResolvedPlayer(uint32) const;
    const OnlineService::PlayerResolveData * FindResolvedPlayer(OnlineService::HeroId uHeroId);
protected:
    virtual void ResolvePlayerData(uint32) = 0;  // vtable slot 2
    void CheckAllPlayersResolved();
private:
    void ResolveCompleteInternal();
    OnlineService::ErrorCode _ValidatePlayersCommon(bool bIsSeasonGame);
    OnlineService::ErrorCode _ValidatePlayersCoop(const D3::OnlineService::CampaignOrAdventureModeCreateParams &, bool, uint32);
    OnlineService::ErrorCode _ValidateQuestProgression(const D3::OnlineService::CampaignOrAdventureModeCreateParams &, uint32);
    OnlineService::ErrorCode _ValidateCoopContentLicenses(const D3::OnlineService::CampaignOrAdventureModeCreateParams &, int32, OnlineService::PlayerResolveData &, BOOL);
    OnlineService::ErrorCode _ValidatePlayersChallengeRift(uint32 uValidateFlags);
    OnlineService::ErrorCode _ValidatePlayersPVP();
protected:
    XArray<OnlineService::PlayerResolveData, const OnlineService::PlayerResolveData &, 4> m_tResolveArray;  // 0x60
    OnlineService::PlayerResolver::ResolveCompleteCb m_tResolveCompleteCb;  // 0xc30
    uint64 m_gameId;  // 0xc38
    uint32 m_weeklyChallengeId;  // 0xc40
    bool m_challengeOverrideUsed;  // 0xc44
    D3::Leaderboard::RiftSnapshot m_tRiftSnapshot;  // 0xc48
    uint32 m_uResolveOptions;  // 0xcb0
};
static_assert(sizeof(PlayerResolver) == 0xcb8, "PlayerResolver");

}  // namespace OnlineService

// Functions defined in this file (bodies are not in the DWARF):
//   line    73  OnlineService::PlayerResolveData & OnlineService::PlayerResolveData::operator=(const OnlineService::PlayerResolveData &)  [inlined into 7 functions]
//   line    73  OnlineService::PlayerResolveData::~PlayerResolveData()  [inlined into 13 functions]
//   line   160  OnlineService::PlayerResolveData & OnlineService::PlayerResolver::GetResolvedPlayer(uint32 uIndex)  [inlined into 7 functions]
//   line   173  const OnlineService::PlayerResolveData * OnlineService::PlayerResolver::FindResolvedPlayer(OnlineService::HeroId uHeroId)  [inline]
