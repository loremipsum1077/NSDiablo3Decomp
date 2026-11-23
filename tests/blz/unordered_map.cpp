// Template checks for blz/unordered_map.h and blz/chained_hash_table.h.
#include "Contrib/Contrib/blz/include/blz/unordered_map.h"

typedef blz::unordered_map<int, unsigned int> MapIntU;
typedef blz::unordered_map<int, int> MapIntInt;
typedef blz::unordered_map<unsigned long, unsigned long> MapULUL;

// ~chained_hash_table, insert<pair<const int, unsigned>>, insert<pair<int, int>>, rehash
template blz::chained_hash_table<blz::unordered_map_traits<int, unsigned int>, blz::hash<int>, blz::equal_to<int>,
                                 blz::allocator<blz::pair<const int, unsigned int> > >::~chained_hash_table();
template void MapIntU::chained_hash_table::rehash(size_t);
template blz::pair<MapIntU::iterator, bool> MapIntU::chained_hash_table::insert(blz::pair<const int, unsigned int>&&);
template blz::pair<MapIntU::iterator, bool> MapIntU::chained_hash_table::insert(blz::pair<int, int>&&);

template int& MapIntInt::operator[](const int&);
template int& MapIntInt::operator[](int&&);
template void MapIntInt::chained_hash_table::rehash(size_t);

template unsigned long& MapULUL::operator[](const unsigned long&);
template void MapULUL::chained_hash_table::rehash(size_t);

// ---- game key/value types (layouts from the DWARF) ----

typedef int SNO;
typedef int GBID;
typedef int BOOL;
typedef unsigned int uint32;
typedef unsigned long uint64;

enum ACDID { ACDID_INVALID = -1 };  // X/Common/CommonTypes.h
enum LeaderboardIds { LB_NONE = 0, LB_ACHIEVEMENT_POINTS = 1 };  // X/Common/Leaderboard.h (no negative values)

struct CurrencyInfo {  // X/Common/Items.h, size 0x8
    GBID gbidItem;
    BOOL bAutoPickup;
};

struct CraftedLegendaryPowerInfo {  // X/Common/Items.h, size 0x8
    SNO snoPower;
    float flUsePowerValue;
};

struct ActorInventoryDatabaseRecord;

struct SPlayerSaveRecord {  // X/Server/SPlayer.cpp, size 0xc
    ACDID idACDPlayer;
    uint32 dwHeroSaveOptions;
    int reason;
};

namespace D3 {
namespace Items {
class CurrencyData {  // protobuf message (Items.pb.h), size 0x30; constructor out of line
public:
    CurrencyData();
    virtual ~CurrencyData();
    char m_pad[0x28];
};
}  // namespace Items
namespace Items {
class SavedItem {  // protobuf message, size 0x50; constructor out of line
public:
    SavedItem();
    virtual ~SavedItem();
    char m_pad[0x48];
};
}  // namespace Items
namespace Leaderboard {
class Score {  // protobuf message, size 0x50; constructor out of line
public:
    Score();
    virtual ~Score();
    char m_pad[0x48];
};
}  // namespace Leaderboard
}  // namespace D3

namespace OnlineService {
struct QuestId {  // X/Client/OnlineService/OnlineServiceClientBase.h, size 0x4
    QuestId();
    SNO snoQuest;
};
inline bool operator==(const QuestId& a, const QuestId& b) { return a.snoQuest == b.snoQuest; }

enum ContentLicenseId { CONTENT_LICENSE_INVALID = -1 };

struct ContentLicense {  // X/Common/OnlineService/OnlineService.h, size 0x18
    ContentLicense() : licenseId(CONTENT_LICENSE_INVALID), expireTime(0), quantity(0), propertyFlags(0) {}
    ContentLicenseId licenseId;
    long expireTime;
    uint32 quantity;
    uint32 propertyFlags;
};

namespace AchievementsEngine {
struct InternalRewardDefinition;
struct InternalCriteriaDefinition;
struct InternalAchievementDefinition;
struct InternalCategoryDefinition;
struct Criteria;
struct Achievement;
}  // namespace AchievementsEngine
}  // namespace OnlineService

// FNV-1a over the key's 4 bytes (X/Sigma/DataStructures/XBaseMap.h); the
// specialization is defined in OnlineServiceClientBase.cpp.
template <>
inline size_t blz::hash<OnlineService::QuestId>::operator()(const OnlineService::QuestId& key) const {
    uint32 v = key.snoQuest;
    uint32 h = 2166136261u;
    h = (h ^ (v & 0xff)) * 16777619u;
    h = (h ^ ((v >> 8) & 0xff)) * 16777619u;
    h = (h ^ ((v >> 16) & 0xff)) * 16777619u;
    h = (h ^ (v >> 24)) * 16777619u;
    return h;
}

class XFourCC {  // X/Sigma/DataStructures/XFourCC.h, size 0x4
public:
    XFourCC();
    XFourCC(const XFourCC& f) : m_value(f.m_value) {}
    uint32 m_value;
};

namespace OnlineService {
typedef uint32 GameAccountId;
struct GameAccountHandle {  // X/Common/OnlineService/OnlineService.h, size 0xc
    GameAccountHandle();
    GameAccountHandle(const GameAccountHandle& o) : m_id(o.m_id), m_program(o.m_program), m_region(o.m_region) {}
    bool operator==(const GameAccountHandle& rhs) const;
    GameAccountId m_id;
    XFourCC m_program;
    uint32 m_region;
};
}  // namespace OnlineService

template <typename T>
uint32 XHashKey(T key);

// OnlineService.h line 771
template <>
inline size_t blz::hash<OnlineService::GameAccountHandle>::operator()(const OnlineService::GameAccountHandle& key) const {
    return XHashKey<const OnlineService::GameAccountHandle&>(key);
}

class NephalemRiftManager {
public:
    struct GreaterRiftPlayerState {  // size 0x28
        int nDeathCount;
        uint64 uLeaverGearAndSkillHash;
        OnlineService::GameAccountHandle tGameAccount;
        BOOL bHasNewSoloRecord;
        BOOL bEmpoweredRift;
    };
    struct GreaterRiftRewardState {  // size 0x14
        int eChoiceState;
        BOOL bSuccess;
        int nJewelUpgradesMax;
        int nJewelUpgradesUsed;
        int nJewelUpgradesBonus;
    };
};

struct ActorInventoryDatabaseRecord {  // X/Common/ActorInventory.h, size 0x80
    ACDID idACD;
    int eAction;
    int uActionMask;
    int eDestination;
    D3::Items::SavedItem tSavedItem;
    bool bNewItem;
    uint64 firstAccountId;
    uint64 lastAccountId;
    uint32 uUpdateCounter;
};

using namespace OnlineService::AchievementsEngine;

template ACDID& blz::unordered_map<ACDID, ACDID>::operator[](const ACDID&);
template void blz::unordered_map<ACDID, ACDID>::chained_hash_table::rehash(size_t);
template int& blz::unordered_map<ACDID, int>::operator[](const ACDID&);
template void blz::unordered_map<ACDID, int>::chained_hash_table::rehash(size_t);
template int& blz::unordered_map<OnlineService::QuestId, int>::operator[](const OnlineService::QuestId&);
template void blz::unordered_map<OnlineService::QuestId, int>::chained_hash_table::rehash(size_t);
template D3::Leaderboard::Score& blz::unordered_map<LeaderboardIds, D3::Leaderboard::Score>::operator[](LeaderboardIds&&);
template D3::Leaderboard::Score& blz::unordered_map<LeaderboardIds, D3::Leaderboard::Score>::operator[](const LeaderboardIds&);
template void blz::unordered_map<LeaderboardIds, D3::Leaderboard::Score>::chained_hash_table::rehash(size_t);
template blz::pair<unsigned, unsigned>& blz::unordered_map<int, blz::pair<unsigned, unsigned> >::operator[](const int&);
template blz::pair<unsigned, unsigned>& blz::unordered_map<int, blz::pair<unsigned, unsigned> >::operator[](int&&);
template void blz::unordered_map<int, blz::pair<unsigned, unsigned> >::chained_hash_table::rehash(size_t);
template CurrencyInfo& blz::unordered_map<int, CurrencyInfo>::operator[](const int&);
template void blz::unordered_map<int, CurrencyInfo>::chained_hash_table::rehash(size_t);
template CraftedLegendaryPowerInfo& blz::unordered_map<int, CraftedLegendaryPowerInfo>::operator[](const int&);
template void blz::unordered_map<int, CraftedLegendaryPowerInfo>::chained_hash_table::rehash(size_t);
template InternalRewardDefinition*& blz::unordered_map<unsigned, InternalRewardDefinition*>::operator[](const unsigned&);
template void blz::unordered_map<unsigned, InternalRewardDefinition*>::chained_hash_table::rehash(size_t);
template InternalCriteriaDefinition*& blz::unordered_map<unsigned long, InternalCriteriaDefinition*>::operator[](unsigned long&&);
template void blz::unordered_map<unsigned long, InternalCriteriaDefinition*>::chained_hash_table::rehash(size_t);
template InternalAchievementDefinition*& blz::unordered_map<unsigned long, InternalAchievementDefinition*>::operator[](unsigned long&&);
template void blz::unordered_map<unsigned long, InternalAchievementDefinition*>::chained_hash_table::rehash(size_t);
template InternalCategoryDefinition*& blz::unordered_map<unsigned, InternalCategoryDefinition*>::operator[](unsigned&&);
template void blz::unordered_map<unsigned, InternalCategoryDefinition*>::chained_hash_table::rehash(size_t);
template Criteria*& blz::unordered_map<unsigned long, Criteria*>::operator[](const unsigned long&);
template void blz::unordered_map<unsigned long, Criteria*>::chained_hash_table::rehash(size_t);
template Achievement*& blz::unordered_map<unsigned long, Achievement*>::operator[](const unsigned long&);
template void blz::unordered_map<unsigned long, Achievement*>::chained_hash_table::rehash(size_t);
template D3::Items::CurrencyData& blz::unordered_map<int, D3::Items::CurrencyData>::operator[](int&&);
template void blz::unordered_map<int, D3::Items::CurrencyData>::chained_hash_table::rehash(size_t);
template OnlineService::ContentLicense& blz::unordered_map<OnlineService::ContentLicenseId, OnlineService::ContentLicense>::operator[](const OnlineService::ContentLicenseId&);
template void blz::unordered_map<OnlineService::ContentLicenseId, OnlineService::ContentLicense>::chained_hash_table::rehash(size_t);
template NephalemRiftManager::GreaterRiftRewardState& blz::unordered_map<unsigned long, NephalemRiftManager::GreaterRiftRewardState>::operator[](const unsigned long&);
template void blz::unordered_map<unsigned long, NephalemRiftManager::GreaterRiftRewardState>::chained_hash_table::rehash(size_t);
template NephalemRiftManager::GreaterRiftPlayerState& blz::unordered_map<unsigned long, NephalemRiftManager::GreaterRiftPlayerState>::operator[](const unsigned long&);
template void blz::unordered_map<unsigned long, NephalemRiftManager::GreaterRiftPlayerState>::chained_hash_table::rehash(size_t);
template ActorInventoryDatabaseRecord& blz::unordered_map<unsigned long, ActorInventoryDatabaseRecord>::operator[](unsigned long&&);
template void blz::unordered_map<unsigned long, ActorInventoryDatabaseRecord>::chained_hash_table::rehash(size_t);
template SPlayerSaveRecord& blz::unordered_map<unsigned long, SPlayerSaveRecord>::operator[](unsigned long&&);
template void blz::unordered_map<unsigned long, SPlayerSaveRecord>::chained_hash_table::rehash(size_t);

// ---- SNOName keys (X/Common/SNO/SNOName.h, OnlineServiceServerPersist.h) ----

namespace Blizzard {
namespace Util {
uint32 Hash32(const void* data, uint32 size);
}
}  // namespace Blizzard

typedef int SNOGroup;

struct SNOName {  // size 0x8
    SNOName();
    SNOName(const SNOName& o) { Set(o.snoGroup, o.snoHandle); }
    void Set(const SNOGroup _snoGroup, const SNO _snoHandle) {
        snoGroup = _snoGroup;
        snoHandle = _snoHandle;
    }
    bool operator==(const SNOName& snoname) const {
        // Stand-in: the original compiles to this sub/sub/orr test.
        return ((snoname.snoGroup - snoGroup) | (snoname.snoHandle - snoHandle)) == 0;
    }
    SNOGroup snoGroup;
    SNO snoHandle;
};

struct SNONameHasher {
    size_t operator()(const SNOName& name) const { return Blizzard::Util::Hash32(&name, sizeof(name)); }
};

struct SNONameEquals {
    bool operator()(const SNOName& left, const SNOName& right) const { return left == right; }
};

typedef blz::unordered_map<SNOName, unsigned int, SNONameHasher, SNONameEquals> SNONameMap;

template SNONameMap::chained_hash_table::~chained_hash_table();
template void SNONameMap::chained_hash_table::rehash(size_t);
template blz::pair<SNONameMap::iterator, bool> SNONameMap::chained_hash_table::insert(blz::pair<const SNOName, unsigned int>&&);

// ---- EventKey keys (X/Common/Achievements/AchievementsEngine/achievements_static_data.h) ----

namespace OnlineService {
namespace AchievementsEngine {
struct EventKey {  // size 0x18
    EventKey();
    uint64 m_condition;
    uint64 m_comparand;
    bool m_hasComparand;
};

// line 48
struct EventKeyHash {
    size_t operator()(const EventKey& key) {
        uint32 hash = Blizzard::Util::Hash32(&key.m_condition, sizeof(key.m_condition));
        hash ^= Blizzard::Util::Hash32(&key.m_comparand, sizeof(key.m_comparand));
        hash ^= Blizzard::Util::Hash32(&key.m_hasComparand, sizeof(key.m_hasComparand));
        return hash;
    }
};

// line 58
inline bool operator==(const EventKey& lhs, const EventKey& rhs) {
    return lhs.m_condition == rhs.m_condition && lhs.m_comparand == rhs.m_comparand &&
           lhs.m_hasComparand == rhs.m_hasComparand;
}
}  // namespace AchievementsEngine
}  // namespace OnlineService

typedef blz::unordered_map<EventKey, bool, EventKeyHash> EventBoolMap;
template bool& EventBoolMap::operator[](const EventKey&);
template void EventBoolMap::chained_hash_table::rehash(size_t);
