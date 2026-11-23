// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/OnlineService/OnlineService.h
// Subset: the value types and free functions X/Common/OnlineService/OnlineService.cpp defines.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/iosfwd.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../CommonTypes.h"
#include "../../Sigma/DataStructures/XFourCC.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"

namespace D3 { namespace OnlineService { class EntityId; } }
namespace D3 { namespace OnlineService { class GameAccountHandle; } }
namespace D3 { namespace OnlineService { class GameCreateParams; } }
namespace D3 { namespace OnlineService { class ItemId; } }

class XVarCollection;

enum {
    MAX_GAME_TAG_CHAR_LENGTH = 15,
    DEFAULT_NUM_SEASONAL_REBIRTHS = 3,
    DEFAULT_REBIRTH_ITEM_MAIL_EXPIRATION_DAYS = 30,
};

namespace OnlineService {
typedef uint64 HeroId;

extern const HeroId INVALID_HERO_ID;  // OnlineService.cpp:76

struct GameId {  // size 0x18
    GameId() { SetInvalid(); }
    void SetInvalid() { m_uMatchmakerGuid = 0; m_uGameServerGuid = 0; m_uGameInstanceId = 0; }
    static const OnlineService::GameId INVALID;
protected:
    uint64 m_uMatchmakerGuid;  // 0x00
    uint64 m_uGameServerGuid;  // 0x08
    uint32 m_uGameInstanceId;  // 0x10
};
static_assert(sizeof(GameId) == 0x18, "GameId");

enum ContentLicenseId {
    CONTENT_LICENSE_INVALID = -1,
    CONTENT_LICENSE_LEVEL_60 = 0,
    CONTENT_LICENSE_LEVEL_70 = 1,
    CONTENT_LICENSE_CLASS_CRUSADER = 2,
    CONTENT_LICENSE_ACT1_ACT4 = 3,
    CONTENT_LICENSE_ADVMODE_ACTV = 4,
    CONTENT_LICENSE_D3_COLLECTORS_EDITION = 5,
    CONTENT_LICENSE_ROS_COLLECTORS_EDITION = 6,
    CONTENT_LICENSE_CHARACTER_SLOTS_PLUS_6 = 7,
    CONTENT_LICENSE_CHARACTER_SLOTS_PLUS_2 = 8,
    CONTENT_LICENSE_CHARACTER_SLOTS_PLUS_3 = 9,
    CONTENT_LICENSE_CHARACTER_SLOTS_PLUS_8 = 10,
    CONTENT_LICENSE_CHARACTER_SLOTS_PLUS_1 = 11,
    CONTENT_LICENSE_STASH_TABS_PLUS_3 = 12,
    CONTENT_LICENSE_STASH_TABS_PLUS_2 = 13,
    CONTENT_LICENSE_STASH_TABS_PLUS_1 = 14,
    CONTENT_LICENSE_STASH_TABS_PLUS_4 = 15,
    CONTENT_LICENSE_BOOST_EXPERIENCE = 16,
    CONTENT_LICENSE_BOOST_GOLDFIND = 17,
    CONTENT_LICENSE_BOOST_BLOODSHARD = 18,
    CONTENT_LICENSE_PAID_PENDING_REVIEW = 19,
    CONTENT_LICENSE_CLASS_NECROMANCER = 20,
    CONTENT_LICENSE_CAN_PURCHASE_STASH_TABS_PLUS_2 = 21,
    CONTENT_LICENSE_MAX_COUNT = 22,
};

enum SeasonState {
    SEASON_STATE_PRE = 0,
    SEASON_STATE_ACTIVE = 1,
    SEASON_STATE_ENDED = 2,
};

struct ContentLicense {  // size 0x18
    ContentLicense() : licenseId(CONTENT_LICENSE_INVALID), expireTime(0), quantity(0), propertyFlags(0) {}  bool IsExpired() const;  // ctor: line 232 (CU 0x56aea09; body from the copy inlined into unordered_map<ContentLicenseId, ContentLicense>::operator[], OnlineServiceServer.cpp)
    OnlineService::ContentLicenseId licenseId;  // 0x00
    Blizzard::Time::Timestamp expireTime;  // 0x08
    uint32 quantity;  // 0x10
    uint32 propertyFlags;  // 0x14
};
static_assert(sizeof(ContentLicense) == 0x18, "ContentLicense");

typedef blz::vector<OnlineService::ContentLicense, blz::allocator<OnlineService::ContentLicense> > ContentLicenseArray;

typedef int32 GameTagId;

struct GameTag {  // size 0x20
    OnlineService::GameTagId m_tagId;  // 0x00
    CRefString m_tag;  // 0x08
};
static_assert(sizeof(GameTag) == 0x20, "GameTag");

typedef blz::vector<OnlineService::GameTag, blz::allocator<OnlineService::GameTag> > XGameTagArray;

// X/Common/OnlineService/OnlineService.h:372. The members are inline in the original (lines 378-400);
// constructor and FromSeconds bodies appear below; the remaining members are declarations.
struct Timestamp {  // size 0x8
    Timestamp();
    Timestamp(uint64 microsecondsSinceUnixEpoch);
    uint64 GetMicroseconds() const;
    uint64 GetMilliseconds() const;
    uint64 GetSeconds() const;
    void AddMicroseconds(const uint64 &);
    void AddMilliseconds(const uint64 &);
    void AddSeconds(const uint64 &);
    void SubtractMicroseconds(const uint64 &);
    Blizzard::Time::Timestamp ToBlizzardTimestamp() const;
    static OnlineService::Timestamp FromSeconds(uint64 secondsSinceUnixEpoch);
    static OnlineService::Timestamp FromBlizzardTimestamp(const Blizzard::Time::Timestamp &);
protected:
    uint64 m_microsecondsSinceUnixEpoch;  // 0x00
};
static_assert(sizeof(Timestamp) == 0x8, "Timestamp");

typedef uint32 AccountId;

typedef uint32 GameAccountId;

struct GameAccountHandle {  // size 0xc
    GameAccountHandle();
    GameAccountHandle(OnlineService::GameAccountId, const XFourCC &, uint32);
    GameAccountHandle(const D3::OnlineService::GameAccountHandle &);
    bool operator==(const OnlineService::GameAccountHandle & rhs) const;
    bool operator!=(const OnlineService::GameAccountHandle & rhs) const;
    void SetInvalid();
    bool IsInvalid() const;
    void Import(const D3::OnlineService::EntityId & entityId);
    void Export(D3::OnlineService::EntityId & entityId) const;
    void Import(const D3::OnlineService::GameAccountHandle & handle);
    static const OnlineService::GameAccountHandle INVALID;
    OnlineService::GameAccountId m_id;  // 0x00
    XFourCC m_program;  // 0x04
    uint32 m_region;  // 0x08
};
static_assert(sizeof(GameAccountHandle) == 0xc, "GameAccountHandle");

struct ChannelId {  // size 0x10
    ChannelId();
    uint32 m_id;  // 0x00
    uint32 m_type;  // 0x04
    uint32 m_label;  // 0x08
    uint32 m_epoch;  // 0x0c
};
static_assert(sizeof(ChannelId) == 0x10, "ChannelId");

struct ItemId {  // size 0x10
    ItemId() : m_high(0), m_low(0) {} uint64 GetLow() const { return m_low; }  // GetLow: line 587, body from the copy inlined into SPlayerUpdateLoadoutItemANNs
    ItemId(const D3::OnlineService::ItemId &);
    CRefString ToString() const;
    void Export(D3::OnlineService::ItemId & tId) const;
    void Import(const D3::OnlineService::ItemId & tId);  bool operator==(const OnlineService::ItemId & b) const { return m_high == b.m_high && m_low == b.m_low; }  // line 597, body from the copy inlined into sItemGeneratorSocket (Items.cpp)
    BOOL IsInvalid() const; bool operator!=(const OnlineService::ItemId & b) const;
    static const OnlineService::ItemId INVALID;
    uint64 m_high;  // 0x00
    uint64 m_low;  // 0x08
};
static_assert(sizeof(ItemId) == 0x10, "ItemId");
// line 590, body from the copy inlined into sActorDelete (Actors.cpp)
inline BOOL ItemId::IsInvalid() const { return m_high == 0 && m_low == 0; }
// line 601, body from the copy inlined into sActorDelete (Actors.cpp)
inline bool ItemId::operator!=(const OnlineService::ItemId & b) const { return m_high != b.m_high || m_low != b.m_low; }

struct ItemIdMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    OnlineService::ItemId tItemId;  // 0x08
};
static_assert(sizeof(ItemIdMessage) == 0x18, "ItemIdMessage");

struct OpenGameToPublicMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    CHAR szGameTag[16];  // 0x08
    BOOL bUserInitiated;  // 0x18
};
static_assert(sizeof(OpenGameToPublicMessage) == 0x1c, "OpenGameToPublicMessage");

extern const uint32 GLOBAL_SEASON_ID;

extern const XFourCC PROGRAM_D3;
extern const XFourCC PROGRAM_PHNX;
extern const XFourCC PROGRAM_BNET_APP;

extern const OnlineService::GameTagId GAME_TAG_ID_NONE;
extern const OnlineService::GameTagId GAME_TAG_ID_DEFAULT;
extern const OnlineService::GameTagId GAME_TAG_ID_BRAWLING;
extern const OnlineService::GameTagId GAME_TAG_ID_KEYWARDEN;
extern const OnlineService::GameTagId GAME_TAG_ID_RIFT;
extern const OnlineService::GameTagId GAME_TAG_ID_GREATER_RIFT;
extern const OnlineService::GameTagId GAME_TAG_ID_BOUNTIES;

extern const char * GAME_TAG_NONE;
extern const char * GAME_TAG_DEFAULT;
extern const char * GAME_TAG_BRAWLING;
extern const char * GAME_TAG_KEYWARDEN;
extern const char * GAME_TAG_BOUNTIES;
extern const char * GAME_TAG_RIFT;
extern const char * GAME_TAG_GREATER_RIFT;

void SetShellDisposing(BOOL isDisposing);
BOOL IsPTR();
blz::ostream & operator<<(blz::ostream & s, const D3::OnlineService::GameCreateParams & params);
uint64 GenerateAuthToken();
uint32 DetermineContentFlags(const OnlineService::ContentLicenseArray & tContentLicenses);
uint32 GetServerContentFlags();
uint32 GetSeasonNum();
OnlineService::SeasonState GetSeasonState();
uint32 GetLeaderboardEra();
BOOL IsFreeToPlay();
BOOL IsChallengeRiftsEnabled();
XVarCollection & GetSyncronizedXVarCollection();
void GetDefaultGameTags(OnlineService::XGameTagArray & arGameTagArray);
void InitializeForApp();
void DisposeForApp();

struct GameIDLabel {  // size 0x4
    explicit GameIDLabel(SGameID id) : m_id(id) {} // line 725
    SGameID m_id;  // 0x00
};
static_assert(sizeof(GameIDLabel) == 0x4, "GameIDLabel");
}  // namespace OnlineService

// Inline bodies from DetailsDigest's constructor and HeroDigest::Import.
inline OnlineService::Timestamp::Timestamp() : m_microsecondsSinceUnixEpoch(0) {} // line 378
inline OnlineService::Timestamp::Timestamp(uint64 microsecondsSinceUnixEpoch)
    : m_microsecondsSinceUnixEpoch(microsecondsSinceUnixEpoch) {} // line 379
inline OnlineService::Timestamp OnlineService::Timestamp::FromSeconds(uint64 secondsSinceUnixEpoch) { // line 400
    return Timestamp(secondsSinceUnixEpoch * 1000000);
}

template <typename ARG_KEY> inline uint32 XHashKey(ARG_KEY key);  // blz::hash specializations below: OnlineService.h:768 and :776, inline in the original
template <> uint32 XHashKey<const OnlineService::GameAccountHandle &>(const OnlineService::GameAccountHandle & gameAccount);  // OnlineService.cpp:1010; line 769 calls it out of line
namespace blz {
template <> struct hash<OnlineService::GameAccountHandle> {
    size_t operator()(const OnlineService::GameAccountHandle & key) const { return XHashKey<const OnlineService::GameAccountHandle &>(key); }
};
template <> struct hash<OnlineService::ChannelId> {
    size_t operator()(const OnlineService::ChannelId &) const;
};
}  // namespace blz
