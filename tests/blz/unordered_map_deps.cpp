// Template checks for blz/unordered_map.h whose values are other blz types
// (shared_ptr, vector, list_iterator, basic_string, intrusive_ptr). Key and
// value layouts are copied from the DWARF.
#include "Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "Contrib/Contrib/blz/include/blz/vector.h"

typedef unsigned int uint32;
typedef unsigned long uint64;

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

namespace AchievementsEngine {
struct Criteria;
struct Achievement;
struct InternalCriteriaDefinition;

// X/Server/Achievements/AchievementsEngine/achievements_progress_data.h, size 0x70
class ProgressData {
    typedef blz::vector<Achievement*> AchievementVector;
    AchievementVector m_achievements;
    typedef blz::unordered_map<unsigned long, blz::shared_ptr<Achievement> > AchievementMap;
    AchievementMap m_achievementsById;
    typedef blz::vector<Criteria*> CriteriaVector;
    CriteriaVector m_criteria;
    typedef blz::unordered_map<unsigned long, blz::shared_ptr<Criteria> > CriteriaMap;
    CriteriaMap m_criteriaById;
};
}  // namespace AchievementsEngine
}  // namespace OnlineService

namespace Blizzard {
namespace Util {
uint32 Hash32(const void* data, uint32 size);
}
}  // namespace Blizzard

// X/Common/Achievements/AchievementsEngine/achievements_static_data.h
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

template <typename T>
uint32 XHashKey(T key);

// OnlineService.h line 771
template <>
inline size_t blz::hash<OnlineService::GameAccountHandle>::operator()(const OnlineService::GameAccountHandle& key) const {
    return XHashKey<const OnlineService::GameAccountHandle&>(key);
}

namespace Console {
class ServerAchievementsInterface {
public:
    struct PlayerAchievementsRecord;
};
}  // namespace Console

using namespace OnlineService::AchievementsEngine;

typedef blz::shared_ptr<Console::ServerAchievementsInterface::PlayerAchievementsRecord> RecordPtr;
template RecordPtr& blz::unordered_map<OnlineService::GameAccountHandle, RecordPtr>::operator[](const OnlineService::GameAccountHandle&);
template void blz::unordered_map<OnlineService::GameAccountHandle, RecordPtr>::chained_hash_table::rehash(size_t);

template blz::shared_ptr<Criteria>& blz::unordered_map<unsigned long, blz::shared_ptr<Criteria> >::operator[](const unsigned long&);
template void blz::unordered_map<unsigned long, blz::shared_ptr<Criteria> >::chained_hash_table::rehash(size_t);
template blz::shared_ptr<Achievement>& blz::unordered_map<unsigned long, blz::shared_ptr<Achievement> >::operator[](const unsigned long&);
template void blz::unordered_map<unsigned long, blz::shared_ptr<Achievement> >::chained_hash_table::rehash(size_t);

template ProgressData& blz::unordered_map<int, ProgressData>::operator[](const int&);
template void blz::unordered_map<int, ProgressData>::chained_hash_table::rehash(size_t);

typedef blz::unordered_map<EventKey, blz::vector<InternalCriteriaDefinition*>, EventKeyHash> CriteriaEventMap;
template blz::vector<InternalCriteriaDefinition*>& CriteriaEventMap::operator[](const EventKey&);
template void CriteriaEventMap::chained_hash_table::rehash(size_t);

// ---- unsigned long -> list_iterator<LRUCache<...>::node_type> (X/Common/OnlineService/LRUCache.h) ----
#include "Contrib/Contrib/blz/include/blz/list.h"
#include "Contrib/Contrib/blz/include/blz/char_traits.h"

namespace blz {
template <typename T, size_t N, typename Traits, typename Alloc>
class basic_embedded_string;
}  // namespace blz

namespace OnlineService {
template <typename Key, typename Value, typename Time, typename Hash, typename Pred>
class TimedLRUCache {
public:
    struct node_type;
};

template <typename Key, typename Value, typename Hash, typename Pred>
class LRUCache {
public:
    struct node_type;
};
}  // namespace OnlineService

typedef blz::basic_embedded_string<char, 20, blz::char_traits<char>, blz::allocator<char> > EmbeddedString20;
typedef OnlineService::TimedLRUCache<unsigned long, EmbeddedString20, long, blz::hash<unsigned long>,
                                     blz::equal_to<unsigned long> >::node_type TimedNode;
typedef OnlineService::LRUCache<unsigned long, TimedNode, blz::hash<unsigned long>, blz::equal_to<unsigned long> >::node_type
    LRUNode;
typedef blz::unordered_map<unsigned long, blz::list_iterator<LRUNode> > LRUIndexMap;

template blz::list_iterator<LRUNode>& LRUIndexMap::operator[](const unsigned long&);
template void LRUIndexMap::chained_hash_table::rehash(size_t);

// ---- containers of unordered_maps: move constructor, move assignment, destructor ----
// Known differences, not in chained_hash_table.h: blz::move (algo_seq.h) has
// its two increments in the other order, and vector::erase inlines that
// blz::move where the binary calls it.
#include "Contrib/Contrib/blz/include/blz/algo_seq.h"

typedef blz::unordered_map<unsigned long, unsigned long> MapULUL;
template void blz::vector<MapULUL>::push_back(MapULUL&&);
template CriteriaEventMap* blz::move<CriteriaEventMap*, CriteriaEventMap*>(CriteriaEventMap*, CriteriaEventMap*, CriteriaEventMap*);
template void blz::vector<CriteriaEventMap>::resize(size_t);
