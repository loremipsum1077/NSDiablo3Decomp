// Template checks for blz::vector with blz::unordered_map / blz::shared_ptr
// elements (headers owned by other agents).
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "Contrib/Contrib/blz/include/blz/unordered_map.h"

typedef unsigned long uint64;

namespace OnlineService { namespace AchievementsEngine {
struct InternalCriteriaDefinition;
struct EventKey {  // size 0x18
    EventKey(uint64, uint64, bool);
    EventKey();
    uint64 m_condition;
    uint64 m_comparand;
    bool m_hasComparand;
};
struct EventKeyHash {
    size_t operator()(const EventKey& key);
};
} }  // namespace OnlineService::AchievementsEngine

typedef blz::unordered_map<unsigned long, unsigned long> ULMap;
typedef blz::unordered_map<OnlineService::AchievementsEngine::EventKey,
                           blz::vector<OnlineService::AchievementsEngine::InternalCriteriaDefinition*>,
                           OnlineService::AchievementsEngine::EventKeyHash> EventMap;

template void blz::vector<ULMap>::push_back(ULMap&&);
template void blz::vector<ULMap>::_transfer(ULMap*);
template void blz::vector<EventMap>::resize(size_t);
template void blz::vector<EventMap>::_destroy();
template blz::vector<EventMap>::iterator blz::vector<EventMap>::erase(const EventMap*, const EventMap*);

#include "Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
namespace OnlineService { namespace AchievementsEngine { class Listener; } }
template void blz::vector<blz::shared_ptr<OnlineService::AchievementsEngine::Listener> >::push_back(const blz::shared_ptr<OnlineService::AchievementsEngine::Listener>&);
