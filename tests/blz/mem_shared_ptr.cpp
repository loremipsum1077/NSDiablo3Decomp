// Template checks for blz::shared_ptr / intrusive_ptr / atomic.
#include "Contrib/Contrib/blz/include/blz/mem_shared_ptr.h"
#include "Contrib/Contrib/blz/include/blz/intrusive_ptr.h"
#include "Contrib/Contrib/blz/include/blz/ref_count_base.h"
#include "Contrib/Contrib/blz/include/blz/atomic.h"
#include "Contrib/Contrib/blz/include/blz/mem_alloc.h"
#include "Contrib/Contrib/blz/include/blz/string.h"

namespace OnlineService {
namespace AchievementsEngine {
struct Criteria { int a; };
struct Achievement { int a; };
}
}

namespace Console { namespace ServerAchievementsInterface {
struct PlayerAchievementsRecord { void* m_data; ~PlayerAchievementsRecord() { blzext::free(m_data); } };
} }

template struct blz::_shared_ptr_emplace<blz::string>;
template struct blz::_shared_ptr_emplace<OnlineService::AchievementsEngine::Criteria>;
template struct blz::_shared_ptr_emplace<OnlineService::AchievementsEngine::Achievement>;
template struct blz::_shared_ptr_emplace<Console::ServerAchievementsInterface::PlayerAchievementsRecord>;

// Compile-only uses of the inlined members (not in functions.csv).
namespace OnlineService { class ResolverStats : public blz::atomic_ref_count_base<ResolverStats> { public: virtual ~ResolverStats(); }; }
namespace Console { namespace ServerAchievementsInterface { struct Base { int a; }; } }

void use_shared(blz::shared_ptr<OnlineService::AchievementsEngine::Criteria>& a,
                blz::shared_ptr<OnlineService::AchievementsEngine::Criteria>&& b) {
    blz::shared_ptr<OnlineService::AchievementsEngine::Criteria> c(a);
    a = blz::move(b);
    c.swap(a);
    a.reset();
    c = blz::make_shared<OnlineService::AchievementsEngine::Criteria>();
    if (a == c && c.get())
        c->a = (*a).a;
}

void use_intrusive(blz::intrusive_ptr<OnlineService::ResolverStats>& a, blz::intrusive_ptr<OnlineService::ResolverStats> b) {
    blz::intrusive_ptr<OnlineService::ResolverStats> c(a);
    a = b;
    a = blz::move(c);
    a.reset(nullptr);
}

blz::atomic<bool> g_bool;
blz::atomic<unsigned int> g_flags;
blz::atomic<int> g_count;
bool use_atomic(unsigned int v) {
    g_bool = true;
    g_flags |= v;
    g_flags.fetch_or(v);
    ++g_count;
    g_count++;
    g_count += 3;
    g_count -= 2;
    --g_count;
    return g_bool.exchange(false) || g_bool;
}

blz::shared_ptr<blz::string> make_string_ptr() {
    return blz::make_shared<blz::string>();
}
