// Original file: X/Common/OnlineService/OnlineService.cpp

#include "OnlineService.h"

#include "../../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "../../../Contrib/Contrib/blz/include/blz/ostream.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../Protobuf/OnlineService.pb.h"
// For cgplaceNull and the GfxVB.h normals, whose initializers the original runs in this file's
// static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../Networking/HostNameResolver.h"
#include "../../Sigma/buildver.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/DataStructures/XVarCollection.h"
#include "../../Sigma/DataStructures/XVariant.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in BlizzardCore/Source/System/Process/Process_NX64.cpp:37; no adopted header declares
// it yet.
namespace Blizzard { namespace Process { char * GetComputerName(char * computerName, int maxNameSize); } }

// Defined in X/Common/OnlineService/Guilds.cpp:25, X/Common/OnlineService/Achievements.cpp and
// X/Common/Leaderboard.cpp:53; no adopted header declares them yet.
void RegisterGuildSyncedVars();
namespace OnlineService { class Achievements { public: static void RegisterSyncedVars(); }; }
void LeaderboardRegisterSyncedVars();

// Defined in X/Common/Networking/GameMessage.cpp:65; no adopted header declares it yet.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;

namespace OnlineService {
extern ComplexTypeDescriptor TypeDescriptor_GameAccountHandle;
extern const XTypeDescriptor * DT_GAME_ACCOUNT_HANDLE;
extern ComplexTypeDescriptor TypeDescriptor_ItemId;
extern const XTypeDescriptor * DT_ITEM_ID;
extern ComplexTypeDescriptor TypeDescriptor_ItemIdMessage;
extern const XTypeDescriptor * DT_ITEM_ID_MESSAGE;
extern ComplexTypeDescriptor TypeDescriptor_OpenGameToPublicMessage;
extern const XTypeDescriptor * DT_OPEN_GAME_TO_PUBLIC_MESSAGE;
}  // namespace OnlineService

static blz::string GetVersionString()  // line 47
{
    if (D3BuildNum() == 0) {
        char szComputerName[256];
        Blizzard::Process::GetComputerName(szComputerName, 256);
        return szComputerName;
    }
    return BuildVersionFull();
}

const uint32 OnlineService::GLOBAL_SEASON_ID = 0;  // line 64
const OnlineService::HeroId OnlineService::INVALID_HERO_ID = 0;  // line 76
const OnlineService::GameId OnlineService::GameId::INVALID;
const OnlineService::ItemId OnlineService::ItemId::INVALID;
const OnlineService::GameAccountHandle OnlineService::GameAccountHandle::INVALID;

static const bool DEFAULT_IS_PTR = false;  // line 82
static const uint32 DEFAULT_SERVER_CONTENT_FLAGS = 0xffffffff;
static const uint32 DEFAULT_SEASON_NUM = 1;
static const uint32 DEFAULT_SEASON_STATE = 0;
static const uint32 DEFAULT_SEASON_FORCE_MIGRATION_SECONDS = 60;
static const bool DEFAULT_FREE_TO_PLAY = false;
static const uint32 DEFAULT_PRODUCT_NOVELTY_DURATION_DAYS = 7;
static const int64 DEFAULT_ACHIEVEMENT_RESET_DATE = -1;
static const uint32 DEFAULT_EARLIEST_SEASON_NUM = 1;
static const uint32 DEFAULT_LEADERBOARD_ERA = 1;
static const int32 DEFAULT_ANNIVERSARY_EVENT_STATUS = -1;
static const uint32 DEFAULT_CHALLENGE_RIFT_NUMBER = 1;
static const bool DEFAULT_CHALLENGE_RIFT_ENABLED = true;
static const int32 DEFAULT_STORE_STATUS = 4;
const char * DEFAULT_CATALOG_DIGEST = "";
static const uint64 DEFAULT_CATALOG_VERSION = 0;
static const int64 DEFAULT_NEWEST_PRODUCT_TIME = 0;
static const bool DEFAULT_CAN_NECROMANCER_PLAY_CAMPAIGN = true;
static const bool DEFAULT_CAN_NECROMANCER_PLAY_ADVENTURE_MODE = true;
static const uint32 DEFAULT_REGION_ID = 1;

static RNG s_tRng;  // line 103
static BOOL s_bRngInitialized;
static BOOL s_bIsShellDisposing;

const XFourCC OnlineService::PROGRAM_D3("D3");  // line 109
const XFourCC OnlineService::PROGRAM_PHNX("CLNT");
const XFourCC OnlineService::PROGRAM_BNET_APP("App");

const OnlineService::GameTagId OnlineService::GAME_TAG_ID_NONE = 0;  // line 120
const OnlineService::GameTagId OnlineService::GAME_TAG_ID_DEFAULT = 1;
const OnlineService::GameTagId OnlineService::GAME_TAG_ID_BRAWLING = 2;
const OnlineService::GameTagId OnlineService::GAME_TAG_ID_KEYWARDEN = 3;
const OnlineService::GameTagId OnlineService::GAME_TAG_ID_RIFT = 4;
const OnlineService::GameTagId OnlineService::GAME_TAG_ID_GREATER_RIFT = 5;
const OnlineService::GameTagId OnlineService::GAME_TAG_ID_BOUNTIES = 6;

const char * OnlineService::GAME_TAG_NONE = "NoTag";  // line 128
const char * OnlineService::GAME_TAG_DEFAULT = "Questing";
const char * OnlineService::GAME_TAG_BRAWLING = "Brawling";
const char * OnlineService::GAME_TAG_KEYWARDEN = "KeyRun";
const char * OnlineService::GAME_TAG_BOUNTIES = "Bounties";
const char * OnlineService::GAME_TAG_RIFT = "Rift";
const char * OnlineService::GAME_TAG_GREATER_RIFT = "GreaterRift";

static XVarScope s_scopeOnlineService("OnlineService", NULL);  // line 142
XVarString s_varOnlineServiceHost(s_scopeOnlineService, "Host", "");
XVarBool s_varOnlineServicePTR(s_scopeOnlineService, "PTR", DEFAULT_IS_PTR);
XVarUint32 s_varServerContentFlags(s_scopeOnlineService, "ServerContentFlags", DEFAULT_SERVER_CONTENT_FLAGS);
XVarBool s_varFreeToPlay(s_scopeOnlineService, "FreeToPlay", DEFAULT_FREE_TO_PLAY);
XVarUint32 s_varProductNoveltyDurationDays(s_scopeOnlineService, "ProductNoveltyDurationDays", DEFAULT_PRODUCT_NOVELTY_DURATION_DAYS);
XVarInt64 s_varAchievementResetDate(s_scopeOnlineService, "AchievementResetDate", DEFAULT_ACHIEVEMENT_RESET_DATE);
XVarString s_varOnlineServiceVersion(s_scopeOnlineService, "Version", GetVersionString().c_str());
static XVarScope s_scopeMatchmaking(s_scopeOnlineService, "Matchmaking");
XVarBool s_varCanNecromancerPlayCampaign(s_scopeOnlineService, "CanNecromancerPlayCampaign", DEFAULT_CAN_NECROMANCER_PLAY_CAMPAIGN);
XVarBool s_varCanNecromancerPlayAdventureMode(s_scopeOnlineService, "CanNecromancerPlayAdventureMode", DEFAULT_CAN_NECROMANCER_PLAY_ADVENTURE_MODE);
XVarString s_varServerPool(s_scopeMatchmaking, "ServerPool", "");

static XVarScope s_scopeSeason(s_scopeOnlineService, "Season");  // line 155
XVarUint32 s_varSeasonNum(s_scopeSeason, "Num", DEFAULT_SEASON_NUM);
XVarUint32 s_varSeasonState(s_scopeSeason, "State", DEFAULT_SEASON_STATE);
XVarUint32 s_varSeasonForceMigrationSeconds(s_scopeSeason, "ForceMigrationSeconds", DEFAULT_SEASON_FORCE_MIGRATION_SECONDS);
XVarUint32 s_varEarliestSeasonNum(s_scopeSeason, "EarliestSeasonNum", DEFAULT_EARLIEST_SEASON_NUM);
XVarUint32 s_varSeasonMaxRebirths(s_scopeSeason, "MaxRebirths", DEFAULT_NUM_SEASONAL_REBIRTHS);
XVarUint32 s_varSeasonRebirthItemMailExpirationDays(s_scopeSeason, "RebirthItemMailExpirationDays", DEFAULT_REBIRTH_ITEM_MAIL_EXPIRATION_DAYS);

XVarBool s_varSeasonsOverrideEnabled(s_scopeSeason, "OverrideEnabled", false);  // line 163

static XVarScope s_scopeLeaderboard(s_scopeOnlineService, "Leaderboard");  // line 166
XVarUint32 s_varLeaderboardEra(s_scopeLeaderboard, "Era", DEFAULT_LEADERBOARD_ERA);

static XVarScope s_scopeChallenge(s_scopeOnlineService, "Challenge");  // line 169
XVarBool s_varChallengeEnabled(s_scopeChallenge, "Enabled", false);

static XVarScope s_scopeChallengeRift("ChallengeRift", NULL);  // line 172
XVarUint32 s_varChallengeNumber(s_scopeChallengeRift, "ChallengeNumber", DEFAULT_CHALLENGE_RIFT_NUMBER);
XVarBool s_varChallengeRiftEnabled(s_scopeChallengeRift, "ChallengeRiftEnabled", DEFAULT_CHALLENGE_RIFT_ENABLED);

static XVarScope s_scopeStore(s_scopeOnlineService, "Store");  // line 176
XVarInt32 s_varStoreStatus(s_scopeStore, "Status", DEFAULT_STORE_STATUS);
XVarString s_varStoreCatalogDigest(s_scopeStore, "ProductCatalogDigest", DEFAULT_CATALOG_DIGEST);
XVarUint64 s_varStoreCatalogVersion(s_scopeStore, "ProductCatalogVersion", DEFAULT_CATALOG_VERSION);
XVarInt64 s_varStoreNewestProductTime(s_scopeStore, "NewestProductTime", DEFAULT_NEWEST_PRODUCT_TIME);

static XVarScope s_scopeAnniversaryEvent(s_scopeOnlineService, "AnniversaryEvent");  // line 182
XVarInt32 s_varAnniversaryEventStatus(s_scopeAnniversaryEvent, "Status", DEFAULT_ANNIVERSARY_EVENT_STATUS);
XVarInt32 s_varAnniversaryEventStatusOverride(s_scopeAnniversaryEvent, "StatusOverride", -1);

static XVarScope s_scopeBuild("Build", NULL);  // line 186

static XVarString s_varBuildType(s_scopeBuild, "Type", "Release");  // line 192

static XVarScope s_scopeRegion(s_scopeOnlineService, "Region");  // line 205
XVarUint32 s_varRegionId(s_scopeRegion, "Id", DEFAULT_REGION_ID);

static CHostNameResolver s_auroraHostNameResolver;  // line 208

static XVarCollection s_tSyncronizedVarCollection;  // line 210

namespace OnlineService {

static const TypeDescriptorField GameAccountHandle_Fields[4] = {  // line 217
    TypeDescriptorField("", DT_INT, offsetof(GameAccountHandle, m_id), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameAccountHandle, m_program), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameAccountHandle, m_region), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameAccountHandle), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GameAccountHandle("GameAccountHandle", 0, GameAccountHandle_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 221
const XTypeDescriptor * DT_GAME_ACCOUNT_HANDLE = &TypeDescriptor_GameAccountHandle;

static const TypeDescriptorField ItemId_Fields[3] = {  // line 223
    TypeDescriptorField("", DT_INT64, offsetof(ItemId, m_high), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(ItemId, m_low), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ItemId), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ItemId("ItemId", 0, ItemId_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 226
const XTypeDescriptor * DT_ITEM_ID = &TypeDescriptor_ItemId;

static const TypeDescriptorField ItemIdMessage_Fields[3] = {  // line 228
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ItemIdMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_ITEM_ID, offsetof(ItemIdMessage, tItemId), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ItemIdMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ItemIdMessage("ItemIdMessage", 0, ItemIdMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 231
const XTypeDescriptor * DT_ITEM_ID_MESSAGE = &TypeDescriptor_ItemIdMessage;

static const TypeDescriptorField OpenGameToPublicMessage_Fields[4] = {  // line 233
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(OpenGameToPublicMessage, tHeader), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(OpenGameToPublicMessage, szGameTag), "", 1, MAX_GAME_TAG_CHAR_LENGTH + 1, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(OpenGameToPublicMessage, bUserInitiated), NULL, (const void *)0, (const void *)1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(OpenGameToPublicMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_OpenGameToPublicMessage("OpenGameToPublicMessage", 0, OpenGameToPublicMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 237
const XTypeDescriptor * DT_OPEN_GAME_TO_PUBLIC_MESSAGE = &TypeDescriptor_OpenGameToPublicMessage;

// LINKER-REMOVED: not in the binary, the linker dropped it (DWARF concrete DIE at low_pc 0, 12 bytes, line table
// 517-518). It is the reader of s_bIsShellDisposing that keeps SetShellDisposing's store alive.
// GetHost (:456, also dropped) is the function whose `blz::vector<blz::string> hosts` made this TU
// emit blz::vector<blz::string>::push_back(blz::string &&); it is not written because its host
// name literals and its out-of-line callees (lines 461, 491, 492) are not recoverable.
BOOL IsShellDisposing()  // line 516
{
    return s_bIsShellDisposing;
}

void SetShellDisposing(BOOL isDisposing)  // line 523
{
    s_bIsShellDisposing = isDisposing;
}

BOOL IsPTR()  // line 530
{
    return s_varOnlineServicePTR.Get();
}

GameAccountHandle::GameAccountHandle()  // line 570
    : m_id(0)
    , m_program()
    , m_region(0)
{
}

GameAccountHandle::GameAccountHandle(GameAccountId uId, const XFourCC & tProgram, uint32 uRegion)  // line 577
    : m_id(uId)
    , m_program(tProgram)
    , m_region(uRegion)
{
}

GameAccountHandle::GameAccountHandle(const D3::OnlineService::GameAccountHandle & handle)  // line 597
{
    Import(handle);
}

bool GameAccountHandle::operator==(const GameAccountHandle & rhs) const  // line 624
{
    return m_id == rhs.m_id && m_program == rhs.m_program && m_region == rhs.m_region;
}

bool GameAccountHandle::operator!=(const GameAccountHandle & rhs) const  // line 632
{
    return !(*this == rhs);
}

void GameAccountHandle::SetInvalid()  // line 656
{
    m_id = 0;
    m_program = XFourCC::INVALID;
    m_region = 0;
}

bool GameAccountHandle::IsInvalid() const  // line 665
{
    return m_id == 0;
}

void GameAccountHandle::Import(const D3::OnlineService::EntityId & entityId)  // line 678
{
    m_id = entityId.id_low();
}

void GameAccountHandle::Export(D3::OnlineService::EntityId & entityId) const  // line 693
{
    entityId.set_id_high(0);
    entityId.set_id_low(m_id);
}

inline void GameAccountHandle::Import(const D3::OnlineService::GameAccountHandle & handle)  // line 709
{
    m_id = handle.id();
    m_program = handle.program();
    m_region = handle.region();
}

ChannelId::ChannelId()  // line 789
    : m_id(0)
    , m_type(0)
    , m_label(0)
    , m_epoch(0)
{
}

ItemId::ItemId(const D3::OnlineService::ItemId & tId)  // line 973
{
    Import(tId);
}

void ItemId::Export(D3::OnlineService::ItemId & tId) const  // line 981
{
    tId.set_id_high(m_high);
    tId.set_id_low(m_low);
}

void ItemId::Import(const D3::OnlineService::ItemId & tId)  // line 991
{
    m_high = tId.id_high();
    m_low = tId.id_low();
}

CRefString ItemId::ToString() const  // line 1000
{
    CRefString szString;
    szString.Format("{ high: %llu low: %llu }", m_high, m_low);
    return szString;
}

}  // namespace OnlineService

template <>
uint32 XHashKey<const OnlineService::GameAccountHandle &>(const OnlineService::GameAccountHandle & gameAccount)  // line 1010
{
    uint32 uHash = Blizzard::Util::Hash32(&gameAccount.m_id, sizeof(gameAccount.m_id));
    uHash = Blizzard::Util::Hash32More(&gameAccount.m_program, sizeof(gameAccount.m_program), uHash);
    uHash = Blizzard::Util::Hash32More(&gameAccount.m_region, sizeof(gameAccount.m_region), uHash);
    return uHash;
}

namespace OnlineService {

blz::ostream & operator<<(blz::ostream & s, const D3::OnlineService::GameCreateParams & params)  // line 1090
{
    s << "{";
    s << " game_type=" << params.game_type();
    if (params.has_pvp()) {
        const D3::OnlineService::PvPCreateParams & pvp = params.pvp();
        s << " PVP";
        s << " sno_world=" << pvp.sno_world();
    }
    if (params.has_campaign_or_adventure_mode()) {
        const D3::OnlineService::CampaignOrAdventureModeCreateParams & coop = params.campaign_or_adventure_mode();
        s << " COOP";
        if (coop.has_resume_from_save_hero_id()) {
            s << " ResumeHeroId=" << coop.resume_from_save_hero_id().id_low();
        } else {
            s << " StartAtQuest";
        }
        if (coop.has_game_tag()) {
            s << " game_tag=" << coop.game_tag();
        }
        s << " act=" << coop.act();
        s << " sno_quest=" << coop.sno_quest();
        s << " quest_step_id=" << coop.quest_step_id();
        if (coop.has_handicap_level()) {
            s << " handicap_level=" << coop.handicap_level();
        }
        if (coop.has_monster_level()) {
            s << " monster_level=" << coop.monster_level();
        }
    }
    s << " creation_flags=" << params.creation_flags();
    s << "}";
    return s;
}

uint64 GenerateAuthToken()  // line 1135
{
    if (!s_bRngInitialized) {
        RandInit(s_tRng);
        s_bRngInitialized = TRUE;
    }
    uint32 uLow = RandGetDWord(s_tRng);
    uint32 uHigh = RandGetDWord(s_tRng);
    uint64 uAuthToken = ((uint64)uHigh << 32) | uLow;
    return uAuthToken;
}

uint32 DetermineContentFlags(const ContentLicenseArray & tContentLicenses)  // line 1172
{
    uint32 uContentFlags = 0;
    for (const ContentLicense * it = tContentLicenses.begin(); it != tContentLicenses.end(); ++it) {
        if (!it->IsExpired()) {
            uContentFlags |= 1 << it->licenseId;
        }
    }
    return uContentFlags;
}

uint32 GetServerContentFlags()  // line 1191
{
    return s_varServerContentFlags.Get();
}

uint32 GetSeasonNum()  // line 1215
{
    return s_varSeasonNum.Get();
}

SeasonState GetSeasonState()  // line 1231
{
    return (SeasonState)s_varSeasonState.Get();
}

uint32 GetLeaderboardEra()  // line 1276
{
    return s_varLeaderboardEra.Get();
}

BOOL IsFreeToPlay()  // line 1283
{
    return s_varFreeToPlay.Get();
}

BOOL IsChallengeRiftsEnabled()  // line 1311
{
    return s_varChallengeRiftEnabled.Get();
}

XVarCollection & GetSyncronizedXVarCollection()  // line 1374
{
    return s_tSyncronizedVarCollection;
}

void GetDefaultGameTags(XGameTagArray & arGameTagArray)  // line 1435
{
    arGameTagArray.clear();

    GameTag tTag = { GAME_TAG_ID_NONE, GAME_TAG_NONE };
    arGameTagArray.push_back(tTag);

    tTag.m_tagId = GAME_TAG_ID_DEFAULT;
    tTag.m_tag = GAME_TAG_DEFAULT;
    arGameTagArray.push_back(tTag);

    tTag.m_tagId = GAME_TAG_ID_BOUNTIES;
    tTag.m_tag = GAME_TAG_BOUNTIES;
    arGameTagArray.push_back(tTag);

    tTag.m_tagId = GAME_TAG_ID_RIFT;
    tTag.m_tag = GAME_TAG_RIFT;
    arGameTagArray.push_back(tTag);

    tTag.m_tagId = GAME_TAG_ID_GREATER_RIFT;
    tTag.m_tag = GAME_TAG_GREATER_RIFT;
    arGameTagArray.push_back(tTag);

    tTag.m_tagId = GAME_TAG_ID_KEYWARDEN;
    tTag.m_tag = GAME_TAG_KEYWARDEN;
    arGameTagArray.push_back(tTag);

    tTag.m_tagId = GAME_TAG_ID_BRAWLING;
    tTag.m_tag = GAME_TAG_BRAWLING;
    arGameTagArray.push_back(tTag);
}

void InitializeForApp()  // line 1488
{
    s_tSyncronizedVarCollection.Add(s_varOnlineServicePTR, XVariant::MakeBool(DEFAULT_IS_PTR));
    s_tSyncronizedVarCollection.Add(s_varServerContentFlags, XVariant::MakeUint32(DEFAULT_SERVER_CONTENT_FLAGS));
    s_tSyncronizedVarCollection.Add(s_varEarliestSeasonNum, XVariant::MakeUint32(DEFAULT_EARLIEST_SEASON_NUM));
    s_tSyncronizedVarCollection.Add(s_varCanNecromancerPlayCampaign, XVariant::MakeBool(DEFAULT_CAN_NECROMANCER_PLAY_CAMPAIGN));
    s_tSyncronizedVarCollection.Add(s_varCanNecromancerPlayAdventureMode, XVariant::MakeBool(DEFAULT_CAN_NECROMANCER_PLAY_ADVENTURE_MODE));
    s_tSyncronizedVarCollection.Add(s_varSeasonNum, XVariant::MakeUint32(DEFAULT_SEASON_NUM));
    s_tSyncronizedVarCollection.Add(s_varSeasonState, XVariant::MakeUint32(DEFAULT_SEASON_STATE));
    s_tSyncronizedVarCollection.Add(s_varSeasonForceMigrationSeconds, XVariant::MakeUint32(DEFAULT_SEASON_FORCE_MIGRATION_SECONDS));
    s_tSyncronizedVarCollection.Add(s_varFreeToPlay, XVariant::MakeBool(DEFAULT_FREE_TO_PLAY));
    s_tSyncronizedVarCollection.Add(s_varProductNoveltyDurationDays, XVariant::MakeUint32(DEFAULT_PRODUCT_NOVELTY_DURATION_DAYS));
    s_tSyncronizedVarCollection.Add(s_varAchievementResetDate, XVariant::MakeInt64(DEFAULT_ACHIEVEMENT_RESET_DATE));
    s_tSyncronizedVarCollection.Add(s_varSeasonMaxRebirths, XVariant::MakeUint32(DEFAULT_NUM_SEASONAL_REBIRTHS));
    s_tSyncronizedVarCollection.Add(s_varSeasonRebirthItemMailExpirationDays, XVariant::MakeUint32(DEFAULT_REBIRTH_ITEM_MAIL_EXPIRATION_DAYS));
    s_tSyncronizedVarCollection.Add(s_varLeaderboardEra, XVariant::MakeUint32(DEFAULT_LEADERBOARD_ERA));
    s_tSyncronizedVarCollection.Add(s_varAnniversaryEventStatus, XVariant::MakeInt32(DEFAULT_ANNIVERSARY_EVENT_STATUS));
    s_tSyncronizedVarCollection.Add(s_varChallengeNumber, XVariant::MakeUint32(DEFAULT_CHALLENGE_RIFT_NUMBER));
    s_tSyncronizedVarCollection.Add(s_varChallengeRiftEnabled, XVariant::MakeBool(DEFAULT_CHALLENGE_RIFT_ENABLED));
    s_tSyncronizedVarCollection.Add(s_varStoreStatus, XVariant::MakeInt32(DEFAULT_STORE_STATUS));
    s_tSyncronizedVarCollection.Add(s_varStoreCatalogDigest, XVariant::MakeString(DEFAULT_CATALOG_DIGEST));
    s_tSyncronizedVarCollection.Add(s_varStoreCatalogVersion, XVariant::MakeUint64(DEFAULT_CATALOG_VERSION));
    s_tSyncronizedVarCollection.Add(s_varStoreNewestProductTime, XVariant::MakeInt64(DEFAULT_NEWEST_PRODUCT_TIME));
    s_tSyncronizedVarCollection.Add(s_varRegionId, XVariant::MakeUint32(DEFAULT_REGION_ID));

    RegisterGuildSyncedVars();
    Achievements::RegisterSyncedVars();
    LeaderboardRegisterSyncedVars();
}

void DisposeForApp()  // line 1528
{
}

bool ContentLicense::IsExpired() const  // line 1621
{
    if (expireTime == 0) {
        return false;
    }
    return expireTime < Blizzard::Time::GetTimestamp();
}

}  // namespace OnlineService
